/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

/***************************************************************************
*  �o�O�ѻ��j�ƾǨt�s�@�s�Ҽ��g���C���M�D��� merc ��s�ӨӡM�Ҧ������v    *
*  �N�|�Q�O�d�M���w��j�a�ק�M���ڭ̤]�Ʊ�A�̤]�ണ�ѵ��j�a�M�Ҧ�����    *
*  �~�欰�N���Q���\�C                                                      *
*                                                                          *
*  paul@mud.ch.fju.edu.tw                                                  *
*  lc@mud.ch.fju.edu.tw                                                    *
*                                                                          *
***************************************************************************/

#include <sys/types.h>
#include <sys/time.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"

/* �ϰ��� */
char *  format_obj_to_char      args( ( OBJ_DATA *, CHAR_DATA *, bool, bool ) );
void    show_list_to_char       args( ( OBJ_DATA *, CHAR_DATA *, bool, bool, bool ) );
void    show_char_to_char_0     args( ( CHAR_DATA *, CHAR_DATA * ) );
void    show_char_to_char_1     args( ( CHAR_DATA *, CHAR_DATA * ) );
void    show_char_to_char       args( ( CHAR_DATA *, CHAR_DATA * ) );
void    show_percentage         args( ( CHAR_DATA *, int, int, int, int  ) );
int     who_compare             args( ( const void *, const void * ) );

#define NAME_ROW                ( 80 / NAME_LENGTH )

#ifdef __FreeBSD__
const char * crypt( const char *, const char * );
#endif

static ARRAY_DATA ObjArmor[] =
  { { 501, "\e[1;33m�j�q�W�j\e[0m" },
    { 400, "\e[1;33m�|���W�j\e[0m" },
    { 300, "\e[1;33m�T���W�j\e[0m" },
    { 200, "\e[1;33m�G���W�j\e[0m" },
    { 101, "\e[1;33m�@���W�j\e[0m" },
    { 100, "\e[1;32m���n�L��\e[0m" },
    {  70, "\e[1;35m���׷l�a\e[0m" },
    {  40, "\e[1;34m���׷l�a\e[0m" },
    {  10, "\e[1;36m�Y���l�a\e[0m" },
    {  -1, "\e[1;31m�Ӱe�פF\e[0m" } };

static ARRAY_DATA AlignmentMessage[] =
  { {   900, "\e[1;37m�n������A�@\e[0m" },
    {   700, "\e[1;37m�ѩʯ¯u���}\e[0m" },
    {   350, "\e[1;37m�ݨӺ��¼䪺\e[0m" },
    {   100, "\e[1;37m�٦��@�I�H��\e[0m" },
    {  -100, "\e[1;37m�O���ߩM����\e[0m" },
    {  -350, "\e[1;37m��b�O�n�a��\e[0m" },
    {  -700, "\e[1;37m���ӬO���c��\e[0m" },
    {  -900, "\e[1;37m�@�I���S�H��\e[0m" },
    { -1001, "\e[1;37m²���O�j�c�]\e[0m" } };

/*
static ARRAY_DATA ArmorMessage[] =
  { {  101, "\e[1;36m�b�r�b��T\e[0m"   },
    {   80, "\e[1;36m���ȷȪ��C\e[0m"   },
    {   60, "\e[1;36m�u��@�I��A\e[0m" },
    {   40, "\e[1;36m���I�˳ư�\e[0m"   },
    {   20, "\e[1;36m�˳��٤���\e[0m"   },
    {    0, "\e[1;36m�����˥�\e[0m"     },
    { - 20, "\e[1;36m��O�٤���\e[0m"   },
    { - 40, "\e[1;36m���O���˳�\e[0m"   },
    { - 60, "\e[1;36m�����˥�\e[0m"     },
    { - 80, "\e[1;36m�i��F�ۨ�\e[0m"   },
    { -100, "\e[1;36m���W�Ÿ˥�\e[0m"   },
    { -101, "\e[1;36m�O�ѤU�L��\e[0m"   } };
*/

static ARRAY_DATA HpMessage[] =
  { { 150 , " �A��$E�󦳾��|��������T"    },
    { 100 , " �A���ͩR�O���G��$E�h���֡C"  },
    { 50  , " �A���ͩR�O��$E���X�@�ǨǡC"  },
    { 0   , " $E���ͩR�O�M�A�t���h�C"      },
    { -50 , " $E�ͩR�O��A���X�@�ǡC"      },
    { -100, " $E���ͩR�O���X�A�ܦh�ܦh�C"  } };

static ARRAY_DATA LevelDiffMessage[] =
{ { 5 , "���n�h�۷S$N�M�ӥB�A���|�q�L���W�o�����n�B���T" },
  { 2 , "����$N�n�@�I�ޥ��M���O�A�٬O���g��Ȫ��T"         },
  { -3, "$N�Q���A�X�A�_�I�����šM�ɧA��O�_���L�N��F�T"   },
  { -6, "���o�ع�$N���׸}�N��F�M�ӥB�ٷ|���Ǹg��ȡT"     },
  { -7, "$N�C��A�_�I�����šM�A���Ϊ�Ӧh�u�Ҧb�L���W�C"   } };

static ARRAY_DATA LevelDiff2Message[] =
{ {  10, "�A�i�H���O�j�Ǥ��O�_��$N�C"       },
  {  5 , "$N���O�A�����C"                 },
  {  3 , "$N�ܮe���ѨM�C"                   },
  { -2 , "�A�M$N�X���۷�C"                 },
  { -5 , "�A�i��ݭn�@�ǹB���C"           },
  { -10, "$N�@�������������A�o�Ӧ׸}�C"     },
  { -15, "�K�T�u���M�Fù�����b���եl��A�C" } };

static ARRAY_DATA PryChance [] =
{
  { 300, "����\n\r" },
  { 250, "�ܰ�\n\r" },
  { 200, "��\n\r"   },
  { 150, "�@��\n\r" },
  { 100, "�C\n\r"   },
  { 50 , "�ܧC\n\r" },
  { -1 , "���C\n\r" }
};

char * check_damage( OBJ_DATA * obj )
{
  static char buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "check_damage" );

  if ( obj && obj->max_armor > 0 )
  {
    sprintf( buf, "[%s]��"
      ,return_array( ObjArmor, obj->armor * 100 / UMAX( 1, obj->max_armor )
        , sizeof( ObjArmor ) / sizeof( ObjArmor[0] ) ) );

    RETURN( buf );
  }

  RETURN( "" );
}

char * format_obj_to_char( OBJ_DATA * obj, CHAR_DATA * ch
                            , bool fShort, bool fDamage )
{
  PUSH_FUNCTION( "format_obj_to_char" );

  clear_stack();

  if ( fDamage )
  {
    if ( obj->venom > 0 ) send_to_stack( "�u\e[1;31m�٬r\e[0m�v" );
    send_to_stack( "%s", check_damage( obj ) );
  }

  if ( obj->Invis ) send_to_stack( "\e[1;37m(����)\e[0m " );

  if ( is_affected( ch, SLOT_DETECT_EVIL ) && obj->Evil )
    send_to_stack( "\e[1;34m(����)\e[0m " );

  if ( is_affected( ch, SLOT_DETECT_MAGIC ) && obj->Magic )
    send_to_stack( "\e[1;31m(�]�k)\e[0m " );

  if ( obj->Glow ) send_to_stack( "\e[1;33m(�{�{�o�G)\e[0m " );
  if ( obj->Hum )  send_to_stack( "\e[1;32m(���@�T)\e[0m " );

  if ( obj->Cancachet )
  {
    if ( obj->cachet ) send_to_stack( "\e[1;35m(�ʦL)\e[0m " );
    else               send_to_stack( "\e[1;35m(�i�ʦL)\e[0m " );
  }

  if ( fShort )
  {
    if ( obj->cname ) send_to_stack( "%s", obj_name( ch, obj ) );
  }
  else
  {
    if ( obj->description ) send_to_stack( "%s", obj->description );
  }

  RETURN( return_stack() );
}

/* ��{�@�ꪫ�~�����a, �i�H�B�z���ƪ��˳� */
void show_list_to_char( OBJ_DATA * list, CHAR_DATA * ch , bool fShort
                       , bool fShowNothing, bool fDamage )
{
  char    ** prgpstrShow;
  char    ** pUnit;
  int      * prgnShow;
  char     * pstrShow;
  OBJ_DATA * obj;
  int        nShow;
  int        iShow;
  int        count;
  bool       fCombine;

  PUSH_FUNCTION( "show_list_to_char" );

  if ( !ch || !ch->desc ) RETURN_NULL();

  clear_buffer();
  for ( count = 0, obj = list ; obj; obj = obj->next_content ) count++;

  if ( count > 1000 )
  {
    send_to_char( "�z�M�A���F���b�Ӧh�F��C\n\r", ch );
    RETURN_NULL();
  }

  prgpstrShow = alloc_string( count * sizeof( char * ) );
  pUnit       = alloc_string( count * sizeof( char * ) );
  prgnShow    = alloc_string( count * sizeof( int )    );

  for ( nShow = 0, obj = list ; obj; obj = obj->next_content )
  {
    if ( obj->wear_loc == WEAR_NONE && can_see_obj( ch, obj ) )
    {
      pstrShow = format_obj_to_char( obj, ch, fShort, fDamage );
      fCombine = FALSE;

      if ( IS_NPC(ch) || IS_SET(ch->act, PLR_COMBINE) )
      {
        /* ��M���Ʀ��ۦP���������~ */
        for ( iShow = nShow - 1; iShow >= 0; iShow-- )
        {
          if ( !str_cmp( prgpstrShow[iShow], pstrShow ) )
          {
            prgnShow[iShow]++;
            fCombine = TRUE;
            break;
          }
        }
      }

      /* ���ݭn��z���ƺ��������~ */
      if ( !fCombine )
      {
        prgpstrShow[nShow] = str_dup( pstrShow );
        pUnit      [nShow] = str_dup( obj->unit );
        prgnShow   [nShow] = 1;
        nShow++;
      }
    }
  }

  /* ��X�榡�ƹL�����~��� */
  for ( iShow = 0 ; iShow < nShow ; iShow++ )
  {
    if ( IS_NPC( ch ) || IS_SET( ch->act, PLR_COMBINE ) )
    {
      char buffer[MAX_STRING_LENGTH];

      chinese_number( prgnShow[iShow], buffer );
      send_to_buffer( "%s", buffer );
      send_to_buffer( "%s", pUnit[iShow] );
    }

    send_to_buffer( "%s\n\r" , prgpstrShow[iShow] );
    free_string( prgpstrShow[iShow] );
    free_string( pUnit      [iShow] );
  }


  if ( fShowNothing && nShow == 0 ) send_to_buffer( "�S�����󪺪F��C\n\r" );

  print_buffer( ch );

  /* ������e�t�m���O���� */
  free_string( ( void * ) prgpstrShow );
  free_string( ( void * ) pUnit       );
  free_string( ( void * ) prgnShow    );
  RETURN_NULL();
}

void show_char_to_char_0( CHAR_DATA * victim, CHAR_DATA * ch )
{
  CHAR_DATA      * mount;
  CHAR_DATA      * target;
  MOB_INDEX_DATA * pMob;

  PUSH_FUNCTION( "show_char_to_char_0" );

  clear_buffer();

  if ( is_affected( victim, SLOT_MASK )
    && !can_see_mask( ch, victim )
    && victim->in_room
    && ( pMob = get_mob_index( victim->mask ) ) )
  {
    if ( ( target = create_mobile( pMob, -1 ) ) )
    {
      char_to_room( target, victim->in_room );
      target->position = victim->position;
      target->fighting = victim->fighting;
    }
    else
    {
      target = victim;
    }
  }

  else
  {
    target = victim;
  }

  if ( is_affected( target, SLOT_INVIS )
    || is_affected( target, SLOT_MASS_INVIS ) )
    send_to_buffer( "(\e[1;34m����\e[0m) " );

  if ( is_affected( target, SLOT_MASK ) )
    send_to_buffer( "(\e[1;37m���e\e[0m) " );

  if ( is_affected( target, SLOT_HIDE ) )
    send_to_buffer( "(\e[36m�ð�\e[0m) " );

  if ( is_affected( target, SLOT_CHARM_PERSON ) )
    send_to_buffer( "(\e[32m�g�b\e[0m) " );

  if ( is_affected( target, SLOT_PASS_DOOR ) )
    send_to_buffer( "(\e[33m����\e[0m) " );

  if ( is_affected( target, SLOT_FAERIE_FIRE ) )
    send_to_buffer( "(\e[1;35m�������~\e[0m) " );

  if ( is_affected( target, SLOT_SANCTUARY ) )
    send_to_buffer( "(\e[1;37m�զ���~\e[0m) " );

  if ( IS_EVIL( target ) && is_affected( ch, SLOT_DETECT_EVIL ) )
    send_to_buffer( "(\e[30;47m�¦���~\e[0m) " );

  if ( IS_NPC( target ) && target->bounty )
    send_to_buffer( "(\e[1;36m�a��\e[0m) " );

  if ( target->jade.high > 0 && target->jade.low > 0 && !target->boss )
    send_to_buffer( "(\e[1;36m�g��\e[0m) " );

  if ( !IS_NPC(target) && IS_SET(target->act, PLR_KILLER ) )
    send_to_buffer( "(\e[1;31m����\e[0m) " );

  if ( !IS_NPC(target) && IS_SET(target->act, PLR_THIEF  ) )
    send_to_buffer( "(\e[1;33m�p��\e[0m) " );

  if ( !IS_NPC(target) && IS_SET(target->act, PLR_BOLTER ) )
    send_to_buffer( "(\e[1;31m�P����\e[0m) " );

  if ( !IS_NPC( target ) )
  {
    if ( target->level > LEVEL_HERO )
      send_to_buffer( "(\e[1;33m�����@��\e[0m) " );

    else send_to_buffer( "(\e[1;34m�_�I��\e[0m) " );
  }

  send_to_buffer( "%s", mob_name( ch, target ) );
  send_to_buffer( "\e[0m" );

  if ( ( mount = get_mount( target ) ) )
    send_to_buffer( "�M��%s", mob_name( ch, mount ) );

  switch ( target->position )
  {
  case POS_DEAD:
    send_to_buffer( "���b���B�M�i���ڡT" );
    break;

  case POS_SLEEPING:
    send_to_buffer( "�b�o��ı�C" );
    break;

  case POS_RESTING:
    send_to_buffer( "�b�o�̥𮧡C" );
    break;

  case POS_STANDING:
    send_to_buffer( "���b�o�̡C" );
    break;

  case POS_FIGHTING:

    send_to_buffer( "�b�o�̩M" );

    if ( !target->fighting )
    {
      send_to_buffer( "�Ů𥴬[�S" );
    }

    else if ( target->fighting == ch )
    {
      send_to_buffer( "�A�԰��C" );
    }

    else if ( target->in_room == target->fighting->in_room )
    {
      send_to_buffer( "%s�԰����C", mob_name( ch, target->fighting ) );
    }
    else
      send_to_buffer( "�����W�H���������C" );

    break;
  }

  if ( !IS_NPC( target ) && !target->desc ) send_to_buffer( " <�_�u��>" );

  if ( target->desc && set_descriptor_stack( target->desc ) )
  {
    target->desc->stack = FALSE;
    send_to_buffer( " <\e[1;31m�T���Ȧs\e[0m>" );
  }

  if ( target->desc && is_edit( target->desc ) )
    send_to_buffer( " <\e[1;32m�s�褤\e[0m>" );

  if ( !IS_NPC( target ) && IS_SET( target->act, PLR_WIZINVIS ) )
    send_to_buffer( " <\e[1;33m��������\e[0m>" );

  send_to_buffer( "\e[0m\n\r" );
  print_buffer( ch );

  if ( target != victim )
  {
    target->position = POS_STANDING;
    target->fighting = NULL;
    extract_char( target, TRUE );
  }
  RETURN_NULL();
}

void show_char_to_char_1( CHAR_DATA * victim, CHAR_DATA * ch )
{
  OBJ_DATA   * obj;
  CHAR_DATA  * mount;
  SITUS_DATA * pSitus;
  bool         found;

  PUSH_FUNCTION( "chow_char_to_char_1" );

  clear_buffer();

  if ( can_see( victim, ch ) )
  {
    act( "$n���`���ۧA�C�M�����D��������ϡC" , ch, NULL, victim, TO_VICT );
    act( "$n�`����$N�C"                     , ch, NULL, victim, TO_NOTVICT );

    if ( victim != ch && is_affected( victim, SLOT_MASK ) && victim->mask )
      act( "�A�����e�N���G�Q$N�ѯ}�F�T", victim, NULL, ch, TO_CHAR );
  }

  if ( victim->pcdata )
  {
    if ( victim->pcdata->mater && *victim->pcdata->mater )
    {
      send_to_buffer( "�t���R%s\n\r", victim->pcdata->mater );
    }

    else if ( victim->pcdata->wooer )
    {
      send_to_buffer( "�l�D�̡R%s\n\r", victim->pcdata->wooer->name );
    }

    else
    {
      switch( victim->sex )
      {
      case SEX_FEMALE:
        send_to_buffer( "�B�áR�ݦr�Ӥ��C\n\r" );
        break;

      case SEX_MALE:
        send_to_buffer( "�B�áR���Ҥ@�ӡC\n\r" );
        break;

      default:
        send_to_buffer( "�B�áR�浹�W�Ѧw�ơC\n\r" );
        break;
      }
    }
  }

  ( victim->description[0] )
    ? ( send_to_buffer( "%s", victim->description ) )
    : ( send_to_buffer( "%s�ݰ_�ӨS������S�O���C\n\r"
      , mob_name( ch, victim ) ) );

  send_to_buffer( "\e[0m%s%s\n\r"
    , mob_name( ch, victim ), status_message( victim ) );

  if ( ( mount = get_mount( victim ) ) )
    send_to_buffer( "�y�M�R%s\n\r", mob_name( ch, mount ) );

  if ( ( mount = get_rider( victim ) ) )
    send_to_buffer( "�D�H�R%s\n\r", mob_name( ch, mount ) );

  for ( found = FALSE, pSitus = situs_list; pSitus; pSitus = pSitus->next )
  {
    if ( ( obj = get_eq_char( victim, pSitus->location ) )
      && can_see_obj( ch, obj ) )
    {
      if ( !found )
      {
        send_to_buffer( "\n\r%s���˳ƵۡR\n\r" , mob_name( NULL, victim ) );
        found = TRUE;
      }

      send_to_buffer( "%s%s\n\r"
        , pSitus->eq_name
        , format_obj_to_char( obj, ch, TRUE, FALSE ) );
    }

    if ( buffer_full() ) break;
  }

  print_buffer( ch );

  /* ���s , ���ﯫ�ڥi�H�� */
  if ( victim != ch && !IS_NPC( ch ) && IS_IMMORTAL( ch ) )
  {
    act( "$A$1�A�����a��$N���W����a���F��$0", ch, NULL, victim, TO_CHAR );
    show_list_to_char( victim->carrying, ch, TRUE, TRUE, FALSE );
  }

  RETURN_NULL();
}

void show_char_to_char( CHAR_DATA * list, CHAR_DATA * ch )
{
  CHAR_DATA * rch;

  PUSH_FUNCTION( "show_char_to_char" );

  for ( rch = list; rch; rch = rch->next_in_room )
  {
    if ( rch == ch || rch->mount_by ) continue;

    if ( !IS_NPC(rch)
      && IS_SET(rch->act, PLR_WIZINVIS)
      && get_trust( ch ) < get_trust( rch ) ) continue;

    if ( can_see( ch, rch ) )
    {
      show_char_to_char_0( rch, ch );
    }

    else if ( room_is_dark( ch->in_room )
      && is_affected( rch, SLOT_INFRAVISION ) )
    {
      send_to_char( "\e[1;31m�ϩ����H�b�Y�t�B���ۧA�T\e[0m\n\r", ch );
    }
  }

  RETURN_NULL();
}

bool check_blind( CHAR_DATA * ch )
{
  PUSH_FUNCTION( "check_blind" );

  if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_HOLYLIGHT ) ) RETURN( TRUE );

  if ( is_affected( ch, SLOT_BLINDNESS ) )
  {
    send_to_char( "�n�O�S���M�M�ڤ@�w��ݨ��C���K�K\n\r", ch );
    RETURN( FALSE );
  }

  RETURN( TRUE );
}

FUNCTION( do_look )
{
  extern AREA_DATA * FoggyArea;
  char               buf[MAX_STRING_LENGTH];
  char               buf1[MAX_STRING_LENGTH];
  char               arg1[MAX_INPUT_LENGTH];
  char               arg2[MAX_INPUT_LENGTH];
  EXIT_DATA        * pexit;
  CHAR_DATA        * victim;
  OBJ_DATA         * obj;
  LIQ_DATA         * pLiq;
  SHIP_DATA        * pShip;
  AREA_DATA        * pArea;
  BOARD_DATA       * pBoard;
  MINE_DATA        * pMine;
  char             * pdesc;
  int                door;
  int                loop;
  char               Char;
  bool               foggy;
  bool               fChinese;

  PUSH_FUNCTION( "do_look" );

  if ( !IS_NPC( ch ) && !ch->desc ) RETURN_NULL();

  if ( ch->position < POS_SLEEPING )
  {
    send_to_char( "�A�u�ݨ�P�P�աT\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->position == POS_SLEEPING )
  {
    send_to_char( "�A�b��ı�z���Tz�CZ�� �K�K\n\r", ch );
    RETURN_NULL();
  }

  if ( !check_blind( ch ) ) RETURN_NULL();

  if ( !IS_NPC( ch )
    && !IS_SET( ch->act, PLR_HOLYLIGHT )
    && room_is_dark( ch->in_room ) )
  {
    act( "�藍�_�M$r�º������M�A����]�ݤ���C", ch, NULL, NULL, TO_CHAR );
    show_char_to_char( ch->in_room->people, ch );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );

  if ( !arg1[0] || !str_cmp( arg1, "auto" ) )
  {
    if ( FoggyArea && ch->in_room->area && ch->in_room->area == FoggyArea )
      foggy = TRUE;

    else
      foggy = FALSE;

    act( "$t$7$r$0$A"
      , ch, ch->in_room->Memorize ? "\e[1;32m*\e[0m" : "", NULL, TO_CHAR );

    if ( !arg1[0] || ( !IS_NPC( ch ) && !IS_SET( ch->act, PLR_BRIEF ) ) )
    {
      if ( foggy && str_len( ch->in_room->description ) < sizeof( buf ) - 10 )
      {
        str_cpy( buf, ch->in_room->description );

        for ( fChinese = FALSE, loop = 0; buf[loop]; loop++ )
        {
          Char = buf[loop];

               if ( fChinese )                                 fChinese = FALSE;
          else if ( ( Char >= '\x81' ) && ( Char <= '\xFF' ) ) fChinese =  TRUE;
          else                                                 fChinese = FALSE;

          if ( number_range( 1, 100 ) < FoggyRoom
            && Char != '\n'
            && Char != '\r'
            && Char != '\t')
          {
            buf[loop] = '.';
            if ( fChinese && buf[loop+1] != '\x0' ) buf[loop+1] = '.';
          }

          if ( fChinese && buf[loop+1] != '\x0' )
          {
            fChinese = FALSE;
            loop++;
          }
        }

        send_to_char( buf, ch );
      }

      else
      {
        send_to_char( ch->in_room->description , ch );
        send_to_char( "\e[0m", ch );
      }
    }

    /* ���ж��O���| */
    if ( ch->in_room->Club )
    {
      char buf1[MAX_INPUT_LENGTH];
      char buf2[MAX_INPUT_LENGTH];

      chinese_number( club_notes( ch ) , buf1 );
      chinese_number( club_unread( ch ), buf2 );

      act( "$3�o�̦�$t�����|�H�u$T�ʥ�Ū�v�C$0", ch, buf1, buf2, TO_CHAR );
    }

    show_list_to_char( ch->in_room->contents, ch, TRUE, FALSE, FALSE );

    if ( foggy )
       act( "      $7�o�̪��|�PŢ�n���@���j���M�A�S����k�ݪ��M���T$0"
      , ch, NULL, NULL, TO_CHAR );

    if ( is_owner( ch, ch->in_room, FALSE ) )
      act( "       $2�o�̬O�A�ۤv�R���Фl�M�Aı�o�S�O�ŷx�T$0"
        , ch, NULL, NULL, TO_CHAR );

    /* �d���� */
    if ( ch->pcdata
      && ( pBoard = ch->in_room->board )
      && ch->level >= pBoard->read_level
      && ( !pBoard->lock || IS_IMMORTAL( ch ) ) )
    {
      chinese_number( post_count( pBoard, POST_ALL ), buf );
      chinese_number( unread_post( ch, pBoard ), buf1 );

      print_to_char( ch,
        "\n\r      \e[1;33m%s\e[1;32m�d����(board)�M"
        "�W���@���u\e[1;37m%s\e[1;32m�v�i�d���M�u\e[1;37m%s\e[1;32m�v"
        "�i��Ū�C\e[0m\n\r", pBoard->name, buf, buf1 );
    }

    send_to_char( "\n\r" , ch );

    /* �q�� */
    for ( pMine = ch->in_room->mine; pMine; pMine = pMine->next )
    {
      if ( pMine->mineral
        && pMine->mineral->message
        && *pMine->mineral->message )
      {
        if ( *pMine->mineral->message == '.' )
          act( "$t", ch, pMine->mineral->message + 1, NULL, TO_CHAR );

        else
          act( "$t", ch, pMine->mineral->message, NULL, TO_CHAR );
      }
    }

    /* �����O�_����X�o�I�άO� */
    for ( pShip = ship_list; pShip; pShip = pShip->next )
    {
      if ( pShip->starting == ch->in_room && pShip->waiting_tick > 0 )
      {
        chinese_number( pShip->waiting_tick, buf1 );
        print_to_char( ch, "\n\r\e[1;32m�o�̦���%s�N�b%s�p��"
          "��X�o�e��%s��%s�C\n\r���f�����ȫȽЬd�� ship�C\e[0m\n\r"
          , pShip->name, buf1
        , ( pShip->destination
          && ( pArea = pShip->destination->area )
          && pArea->name ) ? pArea->name : "�����W�ϰ�"
        , pShip->starting && pShip->destination->name
          ? pShip->destination->name : "�����W�a��" );
      }

      if ( pShip->cabin == ch->in_room && pShip->sailing_tick > 0 )
      {
        chinese_number( pShip->sailing_tick, buf1 );
        print_to_char( ch, "\n\r\e[1;33m�o�̥��O%s�M���N�b%s�p��"
          "���F%s��%s�C\e[0m\n\r"
          , pShip->name, buf1
        , ( pShip->destination
          && ( pArea = pShip->destination->area )
          && pArea->name ) ? pArea->name : "�����W�ϰ�"
        , pShip->starting && pShip->destination->name
          ? pShip->destination->name : "�����W�a��" );
      }
    }

    if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_AUTOEXIT ) )
      do_exits( ch, "auto" );

    show_char_to_char( ch->in_room->people, ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg1, "in" ) )
  {
    if ( !arg2[0] )
    {
      send_to_char( "�A�Q�ݤ���F��S\n\r", ch );
      RETURN_NULL();
    }

    if ( !( obj = get_obj_here( ch, arg2 ) ) )
    {
      act( "�A�S�ݨ�o�ӪF�� $2$T$0 �C�C", ch, NULL, arg2, TO_CHAR );
      RETURN_NULL();
    }

    switch ( obj->item_type )
    {
    default:
      act( "$p�̭����i�঳�F��C", ch, obj, NULL, TO_CHAR );
      message_driver( ch, obj, ACT_WHEN_LOOK );
      break;

    case ITEM_URN:

      if ( get_mob_index( obj->value[0] ) )
        act( "$p���ʦL�۬Y�ا����M�Цh�[�p�ߡT", ch, obj, NULL, TO_CHAR );
      else
        act( "$p�q���Q�ʦL�L�M�O�Ӫų��T"      , ch, obj, NULL, TO_CHAR );
      break;

    case ITEM_DRINK_CON:

      if ( obj->value[1] <= 0 )
      {
        act( "��M$p�ŪŦp�]�C", ch, obj, NULL, TO_CHAR );
        message_driver( ch, obj, ACT_WHEN_LOOK );
        break;
      }

      if ( !( pLiq = liq_lookup( obj->value[2] ) ) )
      {
        mudlog( LOG_DEBUG, "do_look: �S���s�� %d ���G��C", obj->value[2] );
        break;
      }

      sprintf( buf, "���ˤF%s��%s�G��C\n\r",
        obj->value[1] < obj->value[0] / 4
          ? "�֩�G�����@"
          : obj->value[1] < 3 * obj->value[0] / 4
            ? "�j���G�����@" : "�j��G�����@"
         , pLiq->color );

      send_to_char( buf, ch );
      message_driver( ch, obj, ACT_WHEN_LOOK );
      break;

    case ITEM_CONTAINER:
    case ITEM_CORPSE_NPC:
    case ITEM_CORPSE_PC:

      if ( IS_SET( obj->value[1], CONT_CLOSED ) )
      {
        act( "$p�O���_�Ӫ��C", ch, obj, NULL, TO_CHAR );
        message_driver( ch, obj, ACT_WHEN_LOOK );
        break;
      }

      act( "$p�̭����R", ch, obj, NULL, TO_CHAR );
      show_list_to_char( obj->contains, ch, TRUE, TRUE, FALSE );

      message_driver( ch, obj, ACT_WHEN_LOOK );
      break;

    case ITEM_MAGICSTONE:
      act( "$p���G���@�Ǥ��@�˪��a��C", ch, obj, NULL, TO_CHAR );
      message_driver( ch, obj, ACT_WHEN_LOOK );
      break;

    case ITEM_SPIRITJADE:
      act( "$p�ݰ_�ӬO�@�����ɡM�������D�n���n�T", ch, obj, NULL, TO_CHAR );
      message_driver( ch, obj, ACT_WHEN_LOOK );
      break;
    }

    RETURN_NULL();
  }

  /* �[�ݯd���O */
  if ( !str_cmp( arg1, "board" ) )
  {
    if ( !( pBoard = ch->in_room->board ) )
    {
      act( "$r�S�S���d���O�M���ݩO�S", ch, NULL, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( pBoard->lock )
    {
      act( "$t�d���O�ثe�������M�ҥH�����[�ݡT"
        , ch, pBoard->name, NULL, TO_CHAR );

      RETURN_NULL();
    }

    if ( pBoard->read_level > ch->level )
    {
      act( "�藍�_�M$p�d���O�O�����Ť�������H�[�ݪ��C"
        , ch, pBoard->name, NULL, TO_CHAR );

      RETURN_NULL();
    }

    if ( !pBoard->post )
    {
      act( "$p�d���O�F�H�ݬz�M���M�b�i�d���]�S���T"
        , ch, NULL, pBoard->name, TO_CHAR );

      RETURN_NULL();
    }

    /* ��ܥX�d�� */
    show_board_title( ch, pBoard );

    RETURN_NULL();
  }

  if ( ( obj = get_obj_here ( ch ,arg1 ) ) )
  {
    switch( obj->item_type )
    {
    default :
      break;

    case ITEM_DRINK_CON:

      if ( obj->value[1] <= 0 )
      {
        act( "$p�̭�����]�S���T", ch, obj, NULL, TO_CHAR );
        message_driver( ch, obj, ACT_WHEN_LOOK );
        RETURN_NULL();
      }

      if ( !( pLiq = liq_lookup( obj->value[2] ) ) )
      {
        mudlog( LOG_DEBUG, "do_look: �S���s�� %d ���G��.", obj->value[2] );
        RETURN_NULL();
      }

      sprintf( buf, "%s�ˤF%s��%s�G��C\n\r",
        obj_name( ch, obj ), obj->value[1] < obj->value[0] / 4
          ? "�֩�G�����@"
          : obj->value[1] < 3 * obj->value[0] / 4
            ? "�j���G�����@" : "�j��G�����@"
         , pLiq->color );

      send_to_char( buf, ch );
      message_driver( ch, obj, ACT_WHEN_LOOK );
      RETURN_NULL();

    case ITEM_CORPSE_NPC:
    case ITEM_CORPSE_PC:

      act( " $p�̭����R\n\r" , ch, obj, NULL, TO_CHAR );
      show_list_to_char( obj->contains, ch, TRUE, TRUE, FALSE );
      message_driver( ch, obj, ACT_WHEN_LOOK );
      RETURN_NULL();
    }
  }

  if ( ( victim = get_char_room( ch, arg1 ) ) )
  {
    show_char_to_char_1( victim, ch );
    RETURN_NULL();
  }

  for ( obj = ch->carrying; obj; obj = obj->next_content )
  {
    if ( can_see_obj( ch, obj ) )
    {
      if ( ( pdesc = get_extra_descr( arg1, obj->extra_descr ) ) )
      {
        act( "$t", ch, pdesc, NULL, TO_CHAR );
        message_driver( ch, obj, ACT_WHEN_LOOK );
        RETURN_NULL();
      }

      if ( ( pdesc = get_extra_descr( arg1, obj->pIndexData->extra_descr ) ) )
      {
        act( "$t", ch, pdesc, NULL, TO_CHAR );
        message_driver( ch, obj, ACT_WHEN_LOOK );
        RETURN_NULL();
      }
    }

    if ( is_name( arg1, obj->name ) )
    {
      act( "$t", ch, obj->description, NULL, TO_CHAR );

      if ( obj->venom > 0 )
        act( "$p�ݰ_�Ӧ��G�O���r���F��C", ch, obj, NULL, TO_CHAR );

      message_driver( ch, obj, ACT_WHEN_LOOK );
      RETURN_NULL();
    }
  }

  for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
  {
    if ( can_see_obj( ch, obj ) )
    {
      if ( ( pdesc = get_extra_descr( arg1, obj->extra_descr ) ) )
      {
        act( "$t", ch, pdesc, NULL, TO_CHAR );
        message_driver( ch, obj, ACT_WHEN_LOOK );
        RETURN_NULL();
      }

      if ( ( pdesc = get_extra_descr( arg1, obj->pIndexData->extra_descr ) ) )
      {
        act( "$t", ch, pdesc, NULL, TO_CHAR );
        message_driver( ch, obj, ACT_WHEN_LOOK );
        RETURN_NULL();
      }
    }

    if ( is_name( arg1, obj->name ) )
    {
      act( "$t", ch, obj->description, NULL, TO_CHAR );

      if ( obj->venom > 0 )
        act( "$p�ݰ_�ӹ��O���r���F��C", ch, obj, NULL, TO_CHAR );

      message_driver( ch, obj, ACT_WHEN_LOOK );
      RETURN_NULL();
    }
  }

  if ( ( pdesc = get_extra_descr( arg1, ch->in_room->extra_descr ) ) )
  {
    send_to_char( pdesc, ch );
    RETURN_NULL();
  }

       if ( !str_prefix( arg1, "north" ) ) door = DIR_NORTH;
  else if ( !str_prefix( arg1, "east"  ) ) door = DIR_EAST;
  else if ( !str_prefix( arg1, "south" ) ) door = DIR_SOUTH;
  else if ( !str_prefix( arg1, "west"  ) ) door = DIR_WEST;
  else if ( !str_prefix( arg1, "up"    ) ) door = DIR_UP;
  else if ( !str_prefix( arg1, "down"  ) ) door = DIR_DOWN;
  else
  {
    send_to_char( "�藍�_�M���̨S���F��T\n\r", ch );
    RETURN_NULL();
  }

  if ( !( pexit = ch->in_room->exit[door] ) || pexit->foggy )
  {
    send_to_char( "�S����S�O���C\n\r", ch );
    RETURN_NULL();
  }

  if ( pexit->description && pexit->description[0] )
    send_to_char( pexit->description, ch );
  else
    send_to_char( "�S����S�O���C\n\r", ch );

  if ( pexit->keyword && pexit->keyword[0] && pexit->keyword[0] != ' ' )
  {
    if ( pexit->closed )
    {
      act( " $d�䪺�����ۡC", ch, NULL, pexit->keyword, TO_CHAR );
    }

    else if ( pexit->isdoor )
    {
      act( " $d�䪺���O�}�۪��C", ch, NULL, pexit->keyword, TO_CHAR );
    }
  }

  RETURN_NULL();
}

FUNCTION( do_examine )
{
  char       arg[MAX_INPUT_LENGTH];
  char       buf[MAX_STRING_LENGTH];
  OBJ_DATA * obj;

  PUSH_FUNCTION( "do_examine" );

  one_argument( argument, arg );

  if ( ( obj = get_obj_here( ch, arg ) ) )
  {
    switch ( obj->item_type )
    {
    default:
      break;

    case ITEM_DRINK_CON:
    case ITEM_CONTAINER:
    case ITEM_CORPSE_NPC:
    case ITEM_CORPSE_PC:
    case ITEM_MAGICSTONE:
    case ITEM_SPIRITJADE:
    case ITEM_URN:
      send_to_char ( "�A�J�Ӥ@�ݡM�b" , ch );
      sprintf( buf, "in %s", arg );
      do_look( ch, buf );
      RETURN_NULL();
    }
  }

  do_look( ch , arg );

  RETURN_NULL();
}

FUNCTION( do_exits )
{
  EXIT_DATA * pexit;
  bool        found = FALSE;
  bool        fAuto;
  int         door;

  PUSH_FUNCTION( "do_exits" );

  /* �����ݤ���X�f */
  if ( !check_blind( ch ) ) RETURN_NULL();

  clear_buffer();
  send_to_buffer( ( fAuto = !str_cmp( argument, "auto" ) ) ?
    "�u\e[1;36m�X�f�R\e[0m" : "\e[1;36m���㪺�X�f�R\e[0m\n\r" );

  for ( door = 0; door < DIR_MAX; door++ )
  {
    if ( ( pexit = ch->in_room->exit[door] )
      && pexit->to_room
      && pexit->closed == FALSE
      && ( !pexit->foggy || IS_IMMORTAL( ch ) ) )
    {
      if ( fAuto )
      {
        found = TRUE;
        send_to_buffer( " %s%s\e[0m"
          , pexit->foggy ? "\e[1;31m" : "\e[1;33m"
          , direction_name( door ) );
      }

      else
      {
        if ( !room_is_dark( pexit->to_room ) || IS_SET( ch->act, PLR_HOLYLIGHT ) )
        {
          found = TRUE;

          send_to_buffer( "%s%s��O�q��\e[1;35m%s\e[0m�C\n\r"
            , pexit->foggy ? "(�g��)" : ""
            ,direction_name( door )
            , pexit->to_room->name );
        }
      }
    }
  }

  if ( !found )
    send_to_buffer( fAuto ? " �L " : "�S���o�{�������G���X�f�C\n\r" );

  if ( fAuto ) send_to_buffer( "�v\n\r\n\r" );
  print_buffer( ch );
  RETURN_NULL();
}

FUNCTION( do_hp )
{
  CHAR_DATA * vi;
  char        pellet[MAX_STRING_LENGTH];
  char        arg[MAX_INPUT_LENGTH];

  PUSH_FUNCTION( "do_hp" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    vi = ch;
  }

  else
  {
    if ( !( vi = get_char_world( ch, arg ) ) )
    {
      act( "�䤣��A�n�[�ݪ���H $2$T$0�C", ch , NULL, arg, TO_CHAR );
      RETURN_NULL();
    }

    if ( IS_NPC( vi ) || !vi->pcdata )
    {
      act( "$N�O�D���a�M�L�k��ݡT", ch, NULL, vi, TO_CHAR );
      RETURN_NULL();
    }

    if ( !biliwick( ch, vi ) )
    {
      act( "�A���v�O������$N�����A�C", ch, NULL, vi, TO_CHAR );
      RETURN_NULL();
    }
  }

  tablize( FORMAT_SET,
"[1;33m\r�~�w�w�w�w�w�s�w�w�w�w�w�w�w�w�s�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w��[0m\n\r"
"[1;33m\r�x[1;32m���šR[0m $LV[1;33m�x[1;32mID�R[0m $NAME      [1;33m�x[1;32m�m�W�R[0m $CNAME                           [1;33m�x[0m\n\r"
"[1;33m\r�u�w�w�w�w�w�r�w�w�w�w�w�w�w�w�r�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t[0m\n\r"
"[1;33m\r�x[0m�ͩR�O�R[0m($HIT  /$MHIT ) [0m�k�O�R[0m($MANA /$MMAN ) [0m��O�R[0m($MOVE /$MMOV )   [1;33m�x[0m\n\r"
"[1;33m\r�x[0m�����R[0m $GOLD        [0m�Ȧ�s�ڡR[0m $BANK        [0m�g��ȡR[0m $EXP             [1;33m�x[0m\n\r"
"[1;33m\r�x[0m�K�����P�R[0m $FIR [0m�i �Ѩc�R[0m $JAI [0m��[0m ��L�R[0m $FAL [0m�p��[0m PK�RWIN �� LOS ��  [1;33m�x[0m\n\r"
"[1;33m\r�x[0m�D�R��H�R[0m $WOOER       [0m�t���R[0m $MATER       [0m�O�_�b�u�W�R[0m $Z           [1;33m�x[0m\n\r"
"[1;33m\r�x[0m���A�R[0m $STA                                                           [1;33m�x[0m\n\r"
"[1;33m\r���w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w��[0m\n\r"
  , 0, NULL, 0 );

  tablize( FORMAT_NUM, NULL, vi->level           , "$LV"    ,  3 );
  tablize( FORMAT_NUM, NULL, vi->hit             , "$HIT"   ,  6 );
  tablize( FORMAT_NUM, NULL, get_curr_hit( vi )  , "$MHIT"  ,  6 );
  tablize( FORMAT_NUM, NULL, vi->mana            , "$MANA"  ,  6 );
  tablize( FORMAT_NUM, NULL, get_curr_mana( vi ) , "$MMAN"  ,  6 );
  tablize( FORMAT_NUM, NULL, vi->move            , "$MOVE"  ,  6 );
  tablize( FORMAT_NUM, NULL, get_curr_move( vi ) , "$MMOV"  ,  6 );
  tablize( FORMAT_NUM, NULL, vi->gold            , "$GOLD"  , 11 );
  tablize( FORMAT_NUM, NULL, vi->bank            , "$BANK"  , 11 );
  tablize( FORMAT_NUM, NULL, vi->exp             , "$EXP"   , 11 );
  tablize( FORMAT_NUM, NULL, vi->firman          , "$FIR"   ,  4 );
  tablize( FORMAT_NUM, NULL, vi->jail            , "$JAI"   ,  4 );
  tablize( FORMAT_NUM, NULL, vi->failed          , "$FAL"   ,  4 );
  tablize( FORMAT_NUM, NULL, vi->pcdata->fight_win , "WIN"    ,  3 );
  tablize( FORMAT_NUM, NULL, vi->pcdata->fight_lost, "LOS"    ,  3 );

  tablize( FORMAT_STRING
    , vi->pcdata->wooer ? vi->pcdata->wooer->name : "(�q��)"
    , 0, "$WOOER" , 12 );

  tablize( FORMAT_STRING
    , vi->pcdata->mater && *vi->pcdata->mater ? vi->pcdata->mater : "(�q��)"
    , 0, "$MATER" , 12 );

  tablize( FORMAT_STRING, YESNO( vi->pcdata->mate ), 0, "$Z", 2 );
  tablize( FORMAT_STRING, vi->name , 0 , "$NAME"  , 11 );
  tablize( FORMAT_STRING, vi->cname, 0 , "$CNAME" , 33 );
  tablize( FORMAT_STRING
    , vi->pcdata->dosage >= DosageLimit ? "�Ī����r" : "�L��"
    , 0 , "$STA" , 10 );

  tablize( FORMAT_RETURN, pellet, 0, NULL, MAX_STRING_LENGTH );
  send_to_char( pellet, ch );

  RETURN_NULL();
}

FUNCTION( do_score )
{
  char          buf[MAX_STRING_LENGTH ];
  char          pellet[MAX_STRING_LENGTH * 2 ];
  char          output[MAX_STRING_LENGTH * 2 ];
  char          arg[MAX_INPUT_LENGTH];
  AFFECT_DATA * paf;
  AREA_DATA   * pArea;
  CHAR_DATA   * vi;
  SKILL_DATA  * pSkill;
  int           iNum;

  PUSH_FUNCTION( "do_score" );

  if ( IS_NPC( ch ) ) RETURN_NULL();
  vi = ch;

  one_argument( argument, arg );

  if ( !arg[0] )
  {
    vi = ch;
  }

  else
  {
    if ( !( vi = get_char_world( ch, arg ) ) )
    {
      act( "�䤣��A�n�[�ݪ���H $2$T$0�C", ch, NULL, arg, TO_CHAR );
      RETURN_NULL();
    }

    if ( IS_NPC( vi ) || !vi->pcdata )
    {
      act( "$N�O�D���a�M�L�k��ݡT", ch, NULL, vi, TO_CHAR );
      RETURN_NULL();
    }

    if ( !biliwick( ch, vi ) )
    {
      act( "�A���v�O������$N�����A�C", ch, NULL, vi, TO_CHAR );
      RETURN_NULL();
    }
  }

  tablize( FORMAT_SET,
"[1;33m\r"
"�~�w�w�s�w�w�s�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�s�w�w�w�w�w�w�w�w��[0m\n\r"
"[1;33m�x[1;32m����[1;33m�x[0m$LVL[1;33m�x[0m$CNAME                                      [1;33m�x [1;32m���y�R [0m$HOME   [1;33m�x[0m\n\r"
"[1;33m�u�w�w�q�w�w�r�w�w�w�w�s�w�w�s�w�w�w�w�w�w�w�w�s�w�w�s�w�s�r�w�s�w�w�w�w�w�w�t[0m\n\r"
"[1;33m�x[1;32m¾�~[1;33m�x[0m$CLASS        [1;33m�x[1;32m�~��[1;33m�x[0mYEAR[0m��([0mREAL[0m�p��)[1;33m�x[1;32m�ʧO[1;33m�x[0mSEXN[1;32m�]��[1;33m�x[0m$GOLD   [0m����[1;33m�x[0m\n\r"
"[1;33m�u�w�s�r�w�w�w�w�w�s�w�r�w�w�r�w�w�w�w�w�w�w�w�r�w�w�r�w�r�w�w�r�w�w�w�w�w�w�t[0m\n\r"
"[1;33m�x[1;32m�H[1;33m�x[0m�O�q[0m( MSSS )[1;33m�x[0m�ͩR�O[0m(HP   /MMHP )[0m �F�O[0m(MANA /MMMNA) [0m��O[0m(MOVE /MMMVE) [1;33m�x[0m\n\r"
"\e[1;33m�x\e[1;32m��\e[1;33m�x\e[0m���O\e[0m( MIII )\e[1;33m�x\e[0m�ޯ�\e[0m(TRA) \e[0m�P��\e[0m(EN) \e[0m�V�m�I��\e[0m(LEAR) \e[0m�Ȧ�s��\e[0m($BANK       )[1;33m�x[0m\n\r"
"\e[1;33m�x\e[1;32m��\e[1;33m�x\e[0m����\e[0m( MWWW )\e[1;33m�x\e[0m��a���~�Rơ�ƶq�R\e[0mNUM /MNU \e[0m�� Ƣ���q�R\e[0mWEI  / MEI  \e[0m����  [1;33m�x[0m\n\r"
"\e[1;33m�x\e[1;32m��\e[1;33m�x\e[0m�ӱ�\e[0m( MDDD )\e[1;33m�x\e[0m�ثe�g��ȡR\e[0m$EXP     \e[0m�I�M�ٻݭn�A \e[0m$LEFT    \e[0m�I�ɤU�@�šC [1;33m�x[0m\n\r"
"\e[1;33m�x\e[1;32m��\e[1;33m�x\e[0m���\e[0m( MCCC )\e[1;33m�x\e[0m�ثe�}��R\e[0mLIGN                                          [1;33m�x[0m\n\r"
"\e[1;33m�x\e[1;32m�p\e[1;33m�x\e[0m����\e[0m( LUML )\e[1;33m�x\e[0m�ثe���A�R\e[0mPOSITION        \e[0m���j�]HU  �f���]DR  �s�K�]DU  [1;33m�x[0m\n\r"
"\e[1;33m�x  \e[1;33m�x\e[0m�ж���(\e[1;32mHMN\e[0m )\e[1;33m�x\e[0m�K�����P�R\e[0m$FIR \e[0m�i �Ѩc�R\e[0m$JAI \e[0m�� ��L�R\e[0m$FAL\e[0m�p��[1;33m          �x[0m\n\r"
"\e[1;33m�u�w�r�w�w�s�w�w�w�r�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t[0m\n\r"
"\e[1;33m�x\e[1;32m�ӤH�]�w\e[1;33m�x\e[0mơ�ͩR�O���� \e[0m$WIMPY \e[0m�H�U�۰ʰk�] Ƣ�C��\e[0m PAG\e[0m��Ȱ� ƣ \e[0m$A \e[0m�������O[1;33m�x[0m\n\r"
"\e[1;33m�x        \e[1;33m�x\e[0m�۰ʥX�f\e[0m�]GEXI \e[0m�۰ʷj��\e[0m�]GLOO \e[0m�۰ʩ^�m����\e[0m�]GSAC \e[0m�D���ѻ�\e[0m�]GGOD [1;33m�x[0m\n\r"
"\e[1;33m�u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t[0m\n\r"
"\e[1;33m�x\e[1;32m��a����\e[1;33m�x\e[0m���� \e[0m$CLUB      \e[0m���M�{�� \e[0m$NOW                                   [1;33m�x[0m\n\r"
"\e[1;33m�u�w�w�w�w�q�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t[0m\n\r"
"\e[1;33m�x\e[1;32m�B�ê��A\e[1;33m�x\e[0m�D�R��H�R\e[0m$WOOER      \e[0m�t���R\e[0m$MATER       \e[0m�O�_�b�u�W�R\e[0m$ON        [1;33m�x[0m\n\r"
"\e[1;33m�u�w�s�w�w�r�w�w�w�w�w�s�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�t[0m\n\r"
"\e[1;33m�x\e[1;32m�Z\e[1;33m�x\e[0m�����R���v\e[0m(HIT )\e[1;33m�x\e[0m���@�O\e[0m(DEF  ) \e[0m\e[0m �Ī����r�R$POI($STA)                 [1;33m�x[0m\n\r"
"\e[1;33m�x\e[1;32m��\e[1;33m�x\e[0m�����ˮ`�O\e[0m(DAM )\e[1;33m�x\e[0m�˳Ƨl���{�סR�ʤ��� $EQ                            [1;33m�x[0m\n\r"  , 0, NULL, 0 );

  tablize( FORMAT_NUM, NULL, vi->eq_capcity            , "$EQ"   , 3 );
  tablize( FORMAT_NUM, NULL, vi->level                 , "$LVL"  , 4 );
  tablize( FORMAT_NUM, NULL, get_age( vi )             , "YEAR"  , 4 );
  tablize( FORMAT_NUM, NULL, ( get_age( vi ) - 17 ) * 2, "REAL"  , 4 );
  tablize( FORMAT_NUM, NULL, vi->gold                  , "$GOLD" , 7 );
  tablize( FORMAT_NUM, NULL, get_curr_str( vi )        , "MSSS"  , 4 );
  tablize( FORMAT_NUM, NULL, get_curr_int( vi )        , "MIII"  , 4 );
  tablize( FORMAT_NUM, NULL, get_curr_wis( vi )        , "MWWW"  , 4 );
  tablize( FORMAT_NUM, NULL, get_curr_dex( vi )        , "MDDD"  , 4 );
  tablize( FORMAT_NUM, NULL, get_curr_con( vi )        , "MCCC"  , 4 );
  tablize( FORMAT_NUM, NULL, vi->limit                 , "LUML"  , 4 );
  tablize( FORMAT_NUM, NULL, vi->hit                   , "HP"    , 5 );
  tablize( FORMAT_NUM, NULL, get_curr_hit( vi )        , "MMHP"  , 5 );
  tablize( FORMAT_NUM, NULL, vi->mana                  , "MANA"  , 5 );
  tablize( FORMAT_NUM, NULL, get_curr_mana( vi )       , "MMMNA" , 5 );
  tablize( FORMAT_NUM, NULL, vi->move                  , "MOVE"  , 5 );
  tablize( FORMAT_NUM, NULL, get_curr_move( vi )       , "MMMVE" , 5 );
  tablize( FORMAT_NUM, NULL, learn_count( vi )         , "TRA"   , 3 );
  tablize( FORMAT_NUM, NULL, enable_count( vi )        , "EN"    , 2 );
  tablize( FORMAT_NUM, NULL, vi->practice              , "LEAR"  , 4 );
  tablize( FORMAT_NUM, NULL, vi->bank                  , "$BANK" ,12 );
  tablize( FORMAT_NUM, NULL, can_carry_n( vi )         , "MNU"   , 3 );
  tablize( FORMAT_NUM, NULL, can_carry_w( vi )         , "MEI"   , 4 );
  tablize( FORMAT_NUM, NULL, vi->carry_number          , "NUM"   , 3 );
  tablize( FORMAT_NUM, NULL, get_carry_weight( vi )    , "WEI"   , 4 );
  tablize( FORMAT_NUM, NULL, vi->wimpy                 , "$WIMPY", 6 );
  tablize( FORMAT_NUM, NULL, vi->pcdata->pagelen       , "PAG"   , 3 );
  tablize( FORMAT_NUM, NULL, vi->exp                   , "$EXP"  , 8 );
  tablize( FORMAT_NUM, NULL, get_alias_count( vi )     , "$A"    , 2 );
  tablize( FORMAT_NUM, NULL, count_home_number( vi )   , "HMN"   , 3 );

  iNum = kill_table[ URANGE( 1, vi->level, MAX_LEVEL - 1 ) ].exp - vi->exp;
  if ( vi->level >= LEVEL_HERO ) iNum = 0;

  tablize( FORMAT_NUM, NULL, iNum                      , "$LEFT", 8 );

  sprintf( buf, "%s \e[0m( \e[1;36m%s \e[0m) %s\e[0m"
    , vi->cname
    , vi->name
    , vi->pcdata->title );
  tablize( FORMAT_STRING, buf, 0, "$CNAME", 42 );

  pArea = get_home( vi );
  tablize( FORMAT_STRING, pArea ? pArea->name : "����", 0, "$HOME", 8 );

  tablize( FORMAT_STRING, club_name( vi ), 0, "$CLUB", 10 );

  tablize( FORMAT_STRING, is_club_class( vi ), 0, "$NOW", 12 );

  tablize( FORMAT_STRING, IS_IMMORTAL( vi ) ?
    "\e[1;33m�t�κ޲z��\e[0m" : class_name( vi->class, FALSE ) , 0, "$CLASS", 14 );

  tablize( FORMAT_STRING, IS_MALE( vi ) ? "\e[1;32m�k\e[1;33m�x" :
    ( IS_FEMALE( vi ) ? "\e[1;31m�k\e[1;33m�x" : "\e[33m��\e[1;33m�x" )
    , 0, "SEXN" , 4 );

  switch ( vi->position )
  {
  default:
    str_cpy( buf, "\e[1;37m���̤��C\e[0m" );
    break;

  case POS_DEAD:
    str_cpy( buf, "\e[1;31m�Q�����F�T\e[0m" );
    break;

  case POS_SLEEPING:
    str_cpy( buf, "\e[1;34m���b��ı�C\e[0m" );
    break;

  case POS_RESTING:
    str_cpy( buf, "\e[1;36m���b�𮧡C\e[0m" );
    break;

  case POS_STANDING:
    str_cpy( buf, "\e[1;37m�����ߵۡC\e[0m" );
    break;

  case POS_FIGHTING:
    str_cpy( buf, "\e[1;31m���b�԰��C\e[0m" );
    break;
  }

  tablize( FORMAT_STRING, buf, 0, "POSITION", 15 );

  if ( !IS_NPC(vi) && vi->pcdata->condition[COND_DRUNK]   > 10 )
    tablize( FORMAT_STRING, "\e[5;1;33m��\e[0m�^", 0, "DU", 4 );
  else
    tablize( FORMAT_STRING, "  �^", 0, "DU", 4 );

  if ( !IS_NPC(vi) && vi->pcdata->condition[COND_THIRST] ==  0 )
    tablize( FORMAT_STRING, "\e[5;1;33m��\e[0m�^", 0, "DR", 4 );
  else
    tablize( FORMAT_STRING, "  �^", 0, "DR", 4 );

  if ( !IS_NPC(vi) && vi->pcdata->condition[COND_FULL]   ==  0 )
    tablize( FORMAT_STRING, "\e[5;1;33m��\e[0m�^", 0, "HU", 4 );
  else
    tablize( FORMAT_STRING, "  �^", 0, "HU", 4 );

  if ( IS_SET( vi->act, PLR_AUTOEXIT ) )
    tablize( FORMAT_STRING, "\e[1;32m��\e[0m�^", 0, "GEXI", 4 );
  else
    tablize( FORMAT_STRING, "  �^", 0, "GEXI", 4 );

  if ( IS_SET( vi->act, PLR_SILENCE  )
    || IS_SET( vi->act, PLR_NO_EMOTE )
    || IS_SET( vi->act, PLR_NO_TELL  ) )
    tablize( FORMAT_STRING, "\e[1;32m��\e[0m�^", 0, "GGOD", 4 );
  else
    tablize( FORMAT_STRING, "  �^", 0, "GGOD", 4 );

  if ( IS_SET( vi->act, PLR_AUTOLOOT ) )
    tablize( FORMAT_STRING, "\e[1;32m��\e[0m�^", 0, "GLOO", 4 );
  else
    tablize( FORMAT_STRING, "  �^", 0, "GLOO", 4 );

  if ( IS_SET( vi->act, PLR_AUTOSAC ) )
    tablize( FORMAT_STRING, "\e[1;32m��\e[0m�^", 0, "GSAC", 4 );
  else
    tablize( FORMAT_STRING, "  �^", 0, "GSAC", 4 );

  str_cpy( buf, return_array( AlignmentMessage, vi->alignment
            , sizeof( AlignmentMessage ) / sizeof( AlignmentMessage[0] ) ) );

  tablize( FORMAT_STRING, buf, 0, "LIGN", 45 );

  /*
  str_cpy( buf, return_array( ArmorMessage, get_ac( vi )
            , sizeof( ArmorMessage ) / sizeof( ArmorMessage[0] ) ) );
  */

  tablize( FORMAT_STRING
    , vi->pcdata->dosage >= DosageLimit ? "���r" : "�S��"
    , 0, "$POI", 4 );

  tablize( FORMAT_NUM, NULL, vi->pcdata->dosage, "$STA", 4 );
  tablize( FORMAT_NUM, NULL, get_ac( vi ),        "DEF", 5 );
  tablize( FORMAT_NUM, NULL, get_hitroll( vi )  , "HIT", 4 );
  tablize( FORMAT_NUM, NULL, get_damroll( vi )  , "DAM", 4 );
  tablize( FORMAT_NUM, NULL, vi->firman         , "$FIR",4 );
  tablize( FORMAT_NUM, NULL, vi->jail           , "$JAI",4 );
  tablize( FORMAT_NUM, NULL, vi->failed         , "$FAL",4 );

  tablize( FORMAT_STRING
    , vi->pcdata->wooer ? vi->pcdata->wooer->name : "(�q��)"
    , 0, "$WOOER" , 12 );

  tablize( FORMAT_STRING
    , vi->pcdata->mater && *vi->pcdata->mater ? vi->pcdata->mater : "(�q��)"
    , 0, "$MATER" , 12 );

  tablize( FORMAT_STRING, YESNO( vi->pcdata->mate ), 0, "$ON", 3 );

  tablize( FORMAT_RETURN, pellet, 0, NULL, MAX_STRING_LENGTH );
  str_cpy( output, pellet );

  if ( vi->affected )
  {
    bool Touch;

    tablize( FORMAT_SET,
      "\e[1;33m\r�u�w�r�w�w�s�w�w�w�w�w�r�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w"
      "�w�w�w�w�w�w�w�w�w�w�w�t\e[0m\n\r", 0, NULL, 0 );

    tablize( FORMAT_RETURN, pellet, 0, NULL, MAX_STRING_LENGTH );

    if ( str_len( pellet ) + str_len( output ) < sizeof( output ) - 10 )
      str_cat( output, pellet );

    for ( Touch = FALSE, paf = vi->affected; paf; paf = paf->next )
    {
      if ( !( pSkill = get_skill( paf->type ) ) )
      {
        mudlog( LOG_DEBUG, "do_score: ���~�����A %d.", paf->type );
        continue;
      }

      if ( !Touch )
      {
        Touch = TRUE;

        tablize( FORMAT_SET,
           "\e[1;33m�x\e[1;32m�k�N����\e[1;33m�x\e[0mSKILLNAME    SKILLCNAME   SKILLAFFECT        "
           "                   \e[1;33m�x\e[0m\n\r" , 0, NULL, 0 );
      }

      else
      {
        tablize( FORMAT_SET,
           "\e[1;33m�x        �x\e[0mSKILLNAME    SKILLCNAME   SKILLAFFECT        "
           "                   \e[1;33m�x\e[0m\n\r" , 0, NULL, 0 );
      }

      tablize( FORMAT_STRING, pSkill->name,  0, "SKILLNAME",  12 );
      tablize( FORMAT_STRING, pSkill->cname, 0, "SKILLCNAME", 12 );

      if ( ch->level > 20 )
      {
        sprintf( buf,
          "\e[0m �v�T \e[1;37m%s %d\e[0m �I�M�ɶ� \e[1;37m%d\e[0m �p��",
          affect_loc_name( paf->location ),
          paf->modifier,
          paf->duration );
        tablize( FORMAT_STRING, buf, 0, "SKILLAFFECT", 37 );
      }

      else
      {
        tablize( FORMAT_STRING, " ", 0, "SKILLAFFECT", 37 );
      }

      tablize( FORMAT_RETURN, pellet, 0, NULL, MAX_STRING_LENGTH );

      if ( str_len( pellet ) + str_len( output ) < sizeof( output ) - 10 )
        str_cat( output, pellet );
    }

    tablize( FORMAT_SET,
      "\e[1;33m���w�w�w�w�r�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w"
      "�w�w�w�w�w�w�w�w�w�w�w��\e[0m\n\r" , 0, NULL, 0 );

    tablize( FORMAT_RETURN, pellet, 0, NULL, MAX_STRING_LENGTH );

    if ( str_len( pellet ) + str_len( output ) < sizeof( output ) - 10 )
      str_cat( output, pellet );
  }

  else
  {
    tablize( FORMAT_SET,
      "\r\e[1;33m���w�r�w�w�w�w�w�w�w�w�r�w�w�w�w�w�w�w�w�w�w�w�w�w�w"
      "�w�w�w�w�w�w�w�w�w�w�w�w��\e[0m\n\r", 0, NULL, 0 );
    tablize( FORMAT_RETURN, pellet, 0, NULL, MAX_STRING_LENGTH );

    if ( str_len( pellet ) + str_len( output ) < sizeof( output ) - 10 )
      str_cat( output, pellet );
  }

  send_to_char( output, ch );
  RETURN_NULL();
}

FUNCTION( do_time )
{
  char             arg[MAX_INPUT_LENGTH];
  struct timeval   sTime;
  CHAR_DATA      * victim;
  CHAR_DATA      * people;
  int              iTime;

  PUSH_FUNCTION( "do_time" );

  one_argument( argument, arg );

  if ( !arg[0] )
  {
    clear_buffer();
    send_to_buffer( "\n\r�A�}���@��M�~�o�{��Ӥw�g�O\e[1;32m%s\e[0m����"
                    "\e[1;33m%s\e[0m����\e[1;31m%s\e[0m�ɤF�C\n\r"
      , MonthName[time_info.month]
      , DayName[time_info.day]
      , HourName[time_info.hour] );

    send_to_buffer( "%s�Ұʩ� %s�C\n\r"
      , mud_name , time_format( mud_boot_time , "%r, %a-%d-%b-%y" ) );

    send_to_buffer( "�{�b�t�Ϊ��ɶ��O %s�C\n\r"
      , time_format( current_time , "%r, %a-%d-%b-%y" ) );

    if ( reboot_time > 0 ) send_to_buffer( "%s\e[0m�U�@�����s�}���ɶ��O%s�C\n\r"
      , mud_name , time_format( reboot_time, "%r, %a-%d-%b-%y" ) );

    if ( shutdown_time > 0 ) send_to_buffer( "%s\e[0m�����ɶ��O%s�C\n\r"
      , mud_name , time_format( shutdown_time, "%r, %a-%d-%b-%y" ) );

    sTime.tv_sec  = current_time - mud_boot_time;
    sTime.tv_usec = 0;

    send_to_buffer( "%s\e[0m��ثe����w�g���`�B�@�F%s�C\n\r"
      , mud_name, get_worktime_string() );

    print_buffer( ch );
  }

  else
  {
    if ( !str_cmp( arg, "all" ) )
    {
      victim = NULL;
    }

    else
    {
      if ( !( victim = get_char_world( ch, arg ) ) )
      {
        act( "�䤣��A�n�d�ߪ��H�� $2$T$0�C", ch, NULL, arg, TO_CHAR );
        RETURN_NULL();
      }

      if ( IS_NPC( victim ) )
      {
        act( "$N�O�D���a�M����d�ߤW�u�ɶ��C", ch, NULL, victim, TO_CHAR );
        RETURN_NULL();
      }
    }

    for ( clear_buffer(), people = char_list; people; people = people->next )
    {
      if ( !verify_char( people ) || IS_NPC( people ) || !can_see( ch, people ) )
        continue;

      if ( !victim || people == victim )
      {
        iTime = people->played + current_time - people->logon;

        send_to_buffer( "%s�W�u�`�ɼƬ�%s�C\n\r"
          , mob_name( ch, people ), date_string( iTime ) );

        if ( buffer_full() ) break;
      }
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  RETURN_NULL();
}

FUNCTION( do_weather )
{
  char arg[MAX_INPUT_LENGTH];

  static char * const sky_look[4] =
  {
    "\e[1;36m���ŸU����\e[0m",
    "\e[1;32m�h����\e[0m",
    "\e[1;34m�U�B��\e[0m",
    "\e[1;33m�]�Ť��a�{�q\e[0m"
  };

  int size = sizeof( sky_look ) / sizeof( sky_look[0] );

  PUSH_FUNCTION( "do_weather" );

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    if ( !IS_OUTSIDE( ch ) )
    {
      send_to_char( "\e[1m�A�L�k�q�Ǥ��ݨ�~�����Ѯ�T\e[0m\n\r", ch );
    }
    else if ( weather_info.sky < 0 || weather_info.sky >= size )
    {
      send_to_char( "�t�εo�Ϳ��~�T\n\r", ch );
    }

    else
    {
      act( "\e[1m�Ѯ�O$t$7�ӥB\e[1m$2$T$0"
        , ch, sky_look[weather_info.sky]
        , weather_info.change >= 0 ? "�n���}�}�j�ӡT" : "�_���N�B�B���j�T"
        , TO_CHAR );
    }
  }

  else if ( !str_prefix( arg, "fog" ) && IS_IMMORTAL( ch ) )
  {
    if ( !FoggyArea )
    {
      send_to_char( "�ثe�S������_�����ϰ�T\n\r", ch );
    }
    else
    {
      act( "�ثe���ϬO$t�C", ch, FoggyArea->name, NULL, TO_CHAR );
    }
  }

  else if ( !str_prefix( arg, "set" ) && IS_IMMORTAL( ch ) )
  {
    if ( FoggyRoom <= 0 )
    {
      send_to_char( "�X�{�������v�p��s�T\n\r", ch );
      RETURN_NULL();
    }

    if ( FoggyArea )
    {
      send_to_char( "�ثe�w�g�����ϤF�M���ݭn�A���Ĳ�o�T\n\r", ch );
      RETURN_NULL();
    }

    foggy_update( FOGGY_SET );
    send_to_char( "�A���Ĳ�o�F�j�������͡T\n\r", ch );
  }

  else
  {
    send_to_char( "�A���y�k���~�M�Ьd�� weather ���ϥΤ�k�T\n\r", ch );
  }

  RETURN_NULL();
}

FUNCTION( do_help )
{
  CMD_DATA   * pCommand;
  SKILL_DATA * pSkill;
  HELP_DATA  * pHelp;
  int          trust;

  PUSH_FUNCTION( "do_help" );

  if ( !argument[0] ) argument = "summary";

  for ( pHelp = help_first; pHelp; pHelp = pHelp->next )
  {
    if ( pHelp->level > get_trust( ch ) ) continue;

    if ( is_name( argument, pHelp->keyword ) )
    {
      if ( pHelp->level >= 0 ) act( "$t", ch, pHelp->keyword, NULL, TO_CHAR );

      if ( pHelp->text[0] == '.' ) send_to_char( pHelp->text+1, ch );
      else                         send_to_char( pHelp->text  , ch );
      RETURN_NULL();
    }
  }

  trust = get_trust( ch );

  for ( pCommand = cmd_list; pCommand; pCommand = pCommand->next )
  {
    if ( pCommand->level <= trust
      && !pCommand->mobonly
      && !str_prefix( argument, pCommand->name )
      && pCommand->help
      && pCommand->help != &str_empty[0] )
    {
      send_to_char( pCommand->help, ch );
      RETURN_NULL();
    }
  }

  if ( ( pSkill = skill_isname( argument ) )
    && pSkill->help
    && pSkill->help != &str_empty[0] )
  {
    send_to_char( pSkill->help, ch );
    RETURN_NULL();
  }

  send_to_char( "�藍�_�M�u�W�S���o�ӻ��U�����C�ЦV�޲z�̤����M���¡C\n\r", ch );
  RETURN_NULL();
}

FUNCTION( do_who )
{
  DESCRIPTOR_DATA * d;
  DESCRIPTOR_DATA ** pMem;
  CLASS_DATA      * pClass = NULL;
  CLUB_DATA       * pClub  = NULL;
  CHAR_DATA       * wch;
  char            * rank;
  char              arg[ MAX_INPUT_LENGTH ];
  char              buf[MAX_STRING_LENGTH];
  char              name[BUFSIZ];
  int               iLevelLower = 0;
  int               iLevelUpper = MAX_LEVEL;
  int               nMatch;
  int               count;
  int               loop;
  int               iLen;
  bool              NotPrintLong;
  bool              fClassRestrict = FALSE;
  bool              fClubRestrict  = FALSE;
  bool              fImmortalOnly  = FALSE;
  bool              CheckNumber    = FALSE;
  bool              Touch          = FALSE;
  bool              bSend;
  bool              fHeroRestrict  = FALSE;

  PUSH_FUNCTION( "do_who" );

  for ( NotPrintLong = TRUE; *argument; NotPrintLong = FALSE )
  {
    argument = one_argument( argument , arg );

    if ( !arg[0] ) break;

    if ( arg[0] != '-' )
    {
      send_to_char( "�����O�@�ӥ��T���d�ߤ覡�M�Ьd�� who ���Ϊk�T\n\r", ch );
      RETURN_NULL();
    }

    switch( LOWER( arg[1] ) )
    {
    default:
      send_to_char( "�����O�@�ӥ��T���ѼơM�Ьd�� who ���Ϊk�T\n\r", ch );
      RETURN_NULL();

    case 'n':

      CheckNumber = TRUE;
      break;

    case 'w':

      fImmortalOnly = TRUE;
      break;

    case 'c':

      if ( !( pClass = arg[2] ? is_class( arg+2, FALSE ) : ch->class ) )
      {
        act( "$t���O�@��¾�~�M�Ьd�� who ���Ϊk�C", ch, arg + 2, NULL, TO_CHAR );
        RETURN_NULL();
      }

      fClassRestrict = TRUE;

      break;

    case 'h':

      fHeroRestrict = TRUE;
      break;

    case 'l':

      NotPrintLong = FALSE;
      break;

    case 'a':

      if ( !arg[2] || !is_number( arg + 2 ) )
      {
        send_to_char( "�п�J�Ʀr�d�ߡC\n\r", ch );
        RETURN_NULL();
      }

      if ( ( iLevelLower = atoi( arg + 2 ) ) < 0 || iLevelLower > MAX_LEVEL )
      {
        send_to_char( "��J�����Ť��X�z�C\n\r", ch );
        RETURN_NULL();
      }
      break;

    case 'b':

      if ( !arg[2] || !is_number( arg + 2 ) )
      {
        send_to_char( "�п�J�Ʀr�d�ߡC\n\r", ch );
        RETURN_NULL();
      }

      if ( ( iLevelUpper = atoi( arg + 2 ) ) < 0 || iLevelUpper > MAX_LEVEL )
      {
        send_to_char( "��J�����Ť��X�z�C\n\r", ch );
        RETURN_NULL();
      }

      break;

    case 'g':

      iLevelLower = UMAX(          1, ch->level - 5 );
      iLevelUpper = UMIN( LEVEL_HERO, ch->level + 5 );
      break;

    case 'z':

      pClub = arg[2] == '\x0' ? ch->club : clubname_lookup( arg+2, CLUB_NAME );

      if ( !pClub )
      {
        send_to_char( "�䤣��A�n�ݪ������W�١C\n\r", ch );
        RETURN_NULL();
      }

      fClubRestrict = TRUE;
      break;
    }
  }

  if ( iLevelUpper < iLevelLower )
  {
    int temp;

    temp        = iLevelUpper;
    iLevelUpper = iLevelLower;
    iLevelLower = temp;
  }

  /* ���p�⦳�X�Ӵy�z�� */
  for ( count = 0, d = descriptor_list; d; d = d->next )
  {
    if ( verify_desc( d )
      && d->character
      && d->connected == CON_PLAYING
      && can_see( ch, d->character ) )
      count++;
  }

  /* �t�m�O���� */
  if ( !( pMem = alloc_string( count * sizeof( DESCRIPTOR_DATA * ) ) ) )
  {
    mudlog( LOG_DEBUG, "do_who: �L�k�t�m�O����." );
    send_to_char( "�t�ο��~�M�L�k�d�ߡT\n\r", ch );
    RETURN_NULL();
  }

  /* �]�w�ܼƪ� */
  for ( loop = 0, d = descriptor_list; loop < count; d = d->next )
  {
    if ( verify_desc( d )
      && d->character
      && d->connected == CON_PLAYING
      && can_see( ch, d->character ) )
      pMem[loop++] = d;
  }

  /* �Ƨ� */
  qsort( pMem, count, sizeof( pMem[0] ), who_compare );

  if ( !CheckNumber )
  {
    tablize( FORMAT_SET,
    "\e[1;32m�~�w�w�w                     \e[1;32m���a�C��"
    " �w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w��\n\r\e[0m", 0, NULL, 0 );

    tablize( FORMAT_LOCATE_STRING, mud_name, 0, "8", 20 );
    tablize( FORMAT_RETURN, buf, 0, NULL, MAX_STRING_LENGTH );
    send_to_buffer( "%s", buf );
  }

  for ( bSend = TRUE, nMatch = loop = 0; loop < count; loop++ )
  {
    wch = pMem[loop]->character;

    if ( wch->level < iLevelLower
      || wch->level > iLevelUpper
      || ( fImmortalOnly  && wch->level < LEVEL_IMMORTAL )
      || ( fClassRestrict && wch->class != pClass )
      || ( fClubRestrict && wch->club != pClub )
      || ( fHeroRestrict && !IS_HERO( wch ) ) ) continue;

    nMatch++;

         if ( CheckNumber ) continue;
    else if ( NotPrintLong )
    {
      int order;

      if ( ( nMatch % NAME_ROW ) == 1 )
      {
        bSend = Touch = FALSE;
        tablize( FORMAT_SET, "\e[1;32m�x\e[0m                                   "
          "                                         \e[1;32m�x\e[0m\n\r"
          , 0 , NULL, 0 );
      }

      if ( ( order = nMatch % NAME_ROW ) == 0 ) order = NAME_ROW;

      sprintf( arg, "%d", order * 12 - 8 );

      if ( is_friend( ch, wch->name ) )
        sprintf( name, "\e[1;33m%s\e[0m", wch->name );

      else if ( is_badfriend( ch, wch->name ) )
        sprintf( name, "\e[0;37m%s\e[0m", wch->name );

      else
        str_cpy( name, wch->name );

      tablize( FORMAT_LOCATE_STRING, name, 0, arg, 12 );

      Touch = TRUE;
      if ( ( nMatch % NAME_ROW ) == 0 ) bSend = TRUE;
    }

    else
    {
      rank = class_name( wch->class, FALSE );

      if ( !fClubRestrict )
      {
        switch ( wch->level )
        {
        default:
          break;

        case MAX_LEVEL :
          rank = "\e[1;33;44m�Х@��\e[0m";
          break;

        case MAX_LEVEL - 1:
          rank = "\e[1;32m�y�@��\e[0m";
          break;

        case MAX_LEVEL - 2:
          rank = "\e[1;32m�j�ů�\e[0m";
          break;

        case MAX_LEVEL - 3:
          rank = "\e[1;32m��ߧů�\e[0m";
          break;

        case MAX_LEVEL - 4:
          rank = "\e[1;32m�j�Ѩ�\e[0m";
          break;

        case MAX_LEVEL - 5:
          rank = "\e[1;32m��ߤѨ�\e[0m";
          break;
        }
      }

      tablize( FORMAT_SET,
        "\e[1;32m�x             \e[1;32m�x\e[0m                   "
        "                                          \e[1;32m�x\n\r\e[0m"
        , 0, NULL, 0 );

      tablize( FORMAT_LOCATE_NUM, NULL, wch->level, "3", 3 );

      tablize( FORMAT_LOCATE_STRING
       , fClubRestrict ? club_class( wch, pClub ) : rank, 0, "7", 8 );

      sprintf( buf, "[0m%s%s%s %s \e[0m( %s ) %s [0m"
        , IS_SET(wch->act, PLR_KILLER ) ? "\e[1;31m(����)\e[0m" : ""
        , IS_SET(wch->act, PLR_THIEF  ) ? "\e[1;32m(�p��)\e[0m" : ""
        , IS_SET(wch->act, PLR_BOLTER ) ? "\e[1;32m(�q��)\e[0m" : ""
        , wch->cname , wch->name , wch->pcdata->title );
      tablize( FORMAT_LOCATE_STRING, buf, 0, "18", 56 );
      bSend = TRUE;
    }

    if ( bSend )
    {
      tablize( FORMAT_RETURN, buf, 0, NULL, MAX_STRING_LENGTH );
      send_to_buffer( "%s", buf );
      bSend = FALSE;
      Touch = FALSE;
    }
  }

  if ( CheckNumber )
  {
    act( "�{�b�� $4$i$0�쪱�a�b$2$T$0�����C", ch, &nMatch, mud_name, TO_CHAR );
    RETURN_NULL();
  }

  if ( !bSend && NotPrintLong && Touch )
  {
    tablize( FORMAT_RETURN, buf, 0, NULL, MAX_STRING_LENGTH );
    send_to_buffer( "%s", buf );
  }

  tablize( FORMAT_SET,
     "\e[1;32m\r���w�A�ݨ� \e[1;34m   \e[0m (\e[1;37m    \e[0m) "
    "\e[1;32m�쪱�a���b�w�w�w�w�w�w�w�w�w�w\e[1;32m�w�w�w�w�w�w"
    "�w�w�w�w�w�w�w��\n\r\e[0m", 0, NULL, 0 );

  tablize( FORMAT_LOCATE_NUM, NULL, nMatch    , "12", 3 );
  tablize( FORMAT_LOCATE_NUM, NULL, MaxConnect, "18", 3 );

  if ( ( iLen = str_len( mud_name ) ) % 2 != 0 ) iLen++;
  tablize( FORMAT_LOCATE_STRING, mud_name, 0, "33", iLen );
  tablize( FORMAT_RETURN, buf, 0, NULL, MAX_STRING_LENGTH );

  send_to_buffer( "%s", buf );
  print_buffer( ch );

  /* ����O���� */
  free_string( ( void * ) pMem );

  RETURN_NULL();
}

/* �Ǧ^�ϥΪ̪��H�� */
size_t get_usernum( void )
{
  DESCRIPTOR_DATA * d;
  size_t            nUser;

  PUSH_FUNCTION( "get_usernum" );

  for ( nUser = 0 , d = descriptor_list; d; d = d->next )
  {
    if ( verify_desc( d )
      && d->character
      && d->connected == CON_PLAYING
      && !IS_SET( d->character->act , PLR_WIZINVIS ) )
    nUser++;
  }

  RETURN( nUser );
}

/* �Ǧ^���ճs�u���H�� */
size_t get_attemptnum( void )
{
  DESCRIPTOR_DATA * d;
  size_t            num;

  PUSH_FUNCTION( "get_attemptnum" );

  for ( num = 0 , d = descriptor_list; d; d = d->next )
    if ( verify_desc( d ) && d->connected != CON_PLAYING ) num++;

  RETURN( num );
}

size_t get_total_user( void )
{
  DESCRIPTOR_DATA * man;
  size_t            num;

  PUSH_FUNCTION( "get_total_user" );

  for ( num = 0, man = descriptor_list; man; man = man->next )
    if ( verify_desc( man ) ) num++;

  RETURN( num );
}

FUNCTION( do_inventory )
{
  CHAR_DATA * victim;
  char        arg[MAX_INPUT_LENGTH];
  char        buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "do_inventory" );

  one_argument( argument, arg );

  if ( !arg[0] )
  {
    victim = ch;
    sprintf( buf, "%4d/%4d", get_carry_weight( ch ), can_carry_w( ch ) );
    act( "�A���W�ثe�a���R�u�t���q�R $t�v$A", ch, buf, NULL, TO_CHAR );
  }

  else
  {
    if ( !( victim = get_char_world( ch, arg ) ) )
    {
      act( "�䤣��A�n�[�ݪ���H $2$T$0�C", ch, NULL, arg, TO_CHAR );
      RETURN_NULL();
    }

    if ( !biliwick( ch, victim ) )
    {
      act( "�A���v�O������$N���˳ơC", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    sprintf( buf, "%4d/%4d", get_carry_weight( victim ), can_carry_w( victim ) );
    act( "$N���W�ثe�a���R�u�t���q�R$t �v$A", ch, buf, victim, TO_CHAR );
  }

  show_list_to_char( victim->carrying, ch, TRUE, TRUE, TRUE );

  if ( victim->firman > 0 )
  {
    if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_EXACT ) )
      act( "$i�i�K�����P(firman)", ch, &victim->firman, NULL, TO_CHAR );
    else
      act( "$i�i�K�����P", ch, &victim->firman, NULL, TO_CHAR );
  }

  RETURN_NULL();
}

FUNCTION( do_equipment )
{
  OBJ_INDEX_DATA * pObjIndex;
  OBJ_INDEX_DATA * zObjIndex;
  SITUS_DATA     * pSitus;
  OBJ_DATA       * obj;
  CHAR_DATA      * mount;
  CHAR_DATA      * victim;
  char             arg[MAX_INPUT_LENGTH];
  int              vnum;
  int              nMatch;
  int              count;
  int              level;
  int              total;
  bool             found;

  PUSH_FUNCTION( "do_equipment" );

  argument = one_argument( argument, arg );

  if ( !arg[0] )
  {
    victim = ch;
    act( "�A���˳ƵۡR�u�˳Ƨl���{�צʤ���$i�v\n\r"
      , ch, &victim->eq_capcity, NULL, TO_CHAR );
  }

  else if ( !str_prefix( arg, "!search" ) )
  {
    if ( ch->gold < EQSearchCost )
    {
      act( "���A�ꨬ�F$x������~��d�ߡT", ch, &EQSearchCost, NULL, TO_CHAR );
      RETURN_NULL();
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\x0' )
    {
      send_to_char( "�U��m�˳ƪ��^���Ӫ�p�U�R\n\r\n\r"
        "�٫��� \e[1;32mfinger\e[0m  ������ \e[1;32mneck\e[0m    "
        "��A�� \e[1;32mbody\e[0m    �Y���� \e[1;32mhead\e[0m\n\r"
        "�@���� \e[1;32mlegs\e[0m    �c�l�� \e[1;32mfeet\e[0m    "
        "��M�� \e[1;32mhands\e[0m   �@�u�� \e[1;32marms\e[0m\n\r"
        "�޵P�� \e[1;32mshield\e[0m  ��T�� \e[1;32mabout\e[0m   "
        "�y�a�� \e[1;32mwaist\e[0m   ����� \e[1;32mwrist\e[0m\n\r"
        "������ \e[1;32mearring\e[0m �Z���� \e[1;32mhold\e[0m    "
        "�Z���� \e[1;32mwield\e[0m   �I�U�� \e[1;32mback\e[0m\n\r"
        , ch );

      RETURN_NULL();
    }

    for ( pSitus = situs_list; pSitus; pSitus = pSitus->next )
    {
      if ( !str_prefix( arg, pSitus->name ) )
      {
        clear_buffer();
        level = -1;
        gold_from_char( ch, 100 );

        for ( nMatch = vnum = 0; nMatch < top_obj_index; vnum++ )
        {
          if ( ( pObjIndex = get_obj_index( vnum ) ) )
          {
            nMatch++;

            if ( pObjIndex->count > 0
              && ( pObjIndex->wear_flags & pSitus->wear )
              && ch->level >= pObjIndex->level )
            {
              if ( level < pObjIndex->level ) level = pObjIndex->level;
            }
          }
        }

        if ( level < 0 )
        {
          send_to_buffer( "�b�A�����Ť��M�S�������w���˳Ƹ�ơT\n\r" );
          RETURN_NULL();
        }

        zObjIndex = NULL;
        for ( count = nMatch = vnum = 0; nMatch < top_obj_index; vnum++ )
        {
          if ( ( pObjIndex = get_obj_index( vnum ) ) )
          {
            nMatch++;
            if ( pObjIndex->count > 0
              && ( pObjIndex->wear_flags & pSitus->wear )
              && pObjIndex->level == level
              && number_range( 0, count++ ) == 0 )
            {
              zObjIndex = pObjIndex;
            }
          }
        }

        if ( zObjIndex )
        {
          send_to_buffer( "�ǻ������ظ˳ƦW��\e[1;32m%s(%s)\e[0m�M�i��O"
            "�A�ݭn���M�A���֥h���T\n\r"
            , zObjIndex->short_descr, zObjIndex->name );
        }

        else
        {
          send_to_buffer( "�b�A�����Ť��M�S�������w���˳Ƹ�ơT\n\r" );
        }

        print_buffer( ch );
        RETURN_NULL();
      }
    }

    send_to_char( "�U��m�˳ƪ��^���Ӫ�p�U�R\n\r\n\r"
      "�٫��� \e[1;32mfinger\e[0m  ������ \e[1;32mneck\e[0m    "
      "��A�� \e[1;32mbody\e[0m    �Y���� \e[1;32mhead\e[0m\n\r"
      "�@���� \e[1;32mlegs\e[0m    �c�l�� \e[1;32mfeet\e[0m    "
      "��M�� \e[1;32mhands\e[0m   �@�u�� \e[1;32marms\e[0m\n\r"
      "�޵P�� \e[1;32mshield\e[0m  ��T�� \e[1;32mabout\e[0m   "
      "�y�a�� \e[1;32mwaist\e[0m   ����� \e[1;32mwrist\e[0m\n\r"
      "������ \e[1;32mearring\e[0m �Z���� \e[1;32mhold\e[0m    "
      "�Z���� \e[1;32mwield\e[0m   �I�U�� \e[1;32mback\e[0m\n\r"
      , ch );

    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "!info" ) )
  {
    clear_buffer();
    found = FALSE;
    total = 0;

    for ( pSitus = situs_list; pSitus; pSitus = pSitus->next )
      total += pSitus->chance;

    if ( total <= 0 ) total = 1;

    for ( pSitus = situs_list; pSitus; pSitus = pSitus->next )
    {
      send_to_buffer( "%s", pSitus->eq_name );

      if ( IS_IMMORTAL( ch ) )
      {
        send_to_buffer( "%4d( %3d%% ) "
          , pSitus->chance, pSitus->chance * 100 / total );
      }

      if ( ( obj = get_eq_char( ch, pSitus->location ) ) )
      {
        send_to_buffer( "%s\n\r"
          , can_see_obj( ch, obj )
            ? format_obj_to_char( obj, ch, TRUE, TRUE )
            : "�Y��ݤ������F��C" );
      }

      else
      {
        send_to_buffer( "�]\e[1;31m�q��\e[0m�^\n\r" );
      }
    }

    if ( ( mount = get_mount( ch ) ) )
      send_to_buffer( "[�y�M]     %s\n\r", mob_name( ch, mount ) );
    else
      send_to_buffer( "[�y�M]     �]\e[1;32m�q��\e[0m�^\n\r" );

    print_buffer( ch );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "!set" ) )
  {
    int degree;

    one_argument( argument, arg );
    if ( arg[0] == '\x0' || !is_number( arg ) )
    {
      send_to_char( "�A���������˳ƪ��l���{�סT\n\r", ch );
      RETURN_NULL();
    }

    if ( ( degree = atoi( arg ) ) < eq_min_capcity || degree > eq_max_capcity )
    {
      act( "�藍�_�M�˳ƪ��l���{�ץu��Ѧʤ���$i��$I�C"
        , ch, &eq_min_capcity, &eq_max_capcity, TO_CHAR );

      RETURN_NULL();
    }

    ch->eq_capcity = degree;
    act( "�A�]�w�˳Ƨl���{�׬��ʤ���$i�C", ch, &degree, NULL, TO_CHAR );
    RETURN_NULL();
  }

  else
  {
    if ( !( victim = get_char_world( ch, arg ) ) )
    {
      act( "�䤣��A�n�[�ݪ���H $2$T$0�C", ch, NULL, arg, TO_CHAR );
      RETURN_NULL();
    }

    if ( IS_NPC( victim ) )
    {
      act( "�A�����[��$N����ơC", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( !biliwick( ch, victim ) )
    {
      act( "�A���v�O������$N���˳ơC", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    act( "$N���˳ƵۡR�u�˳Ƨl���{�צʤ���$i�v"
      , ch, &victim->eq_capcity, victim, TO_CHAR );
  }

  clear_buffer();
  found = FALSE;

  for ( pSitus = situs_list; pSitus; pSitus = pSitus->next )
  {
    if ( !( obj = get_eq_char( victim, pSitus->location ) ) ) continue;

    send_to_buffer( "%s%s\n\r"
      , pSitus->eq_name
      , can_see_obj( victim, obj )
        ? format_obj_to_char( obj, victim, TRUE, TRUE )
        : "�Y��ݤ������F��C" );

    found = TRUE;
  }

  if ( ( mount = get_mount( victim ) ) )
  {
    send_to_buffer( "[�y�M]     %s\n\r", mob_name( ch, mount ) );
    found = TRUE;
  }

  if ( !found ) send_to_buffer( "�S������˳ơT\n\r", ch );
  print_buffer( ch );
  RETURN_NULL();
}

FUNCTION( do_compare )
{
  char       arg1[MAX_INPUT_LENGTH];
  char       arg2[MAX_INPUT_LENGTH];
  OBJ_DATA * obj1;
  OBJ_DATA * obj2;
  int        value1;
  int        value2;
  char     * msg;

  PUSH_FUNCTION( "do_compare" );

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );

  if ( !arg1[0] )
  {
    send_to_char( "�A�n�������˪F��S\n\r", ch );
    RETURN_NULL();
  }

  if ( !( obj1 = get_obj_carry( ch, arg1 ) ) )
  {
    act( "�A�S���o�ӪF�� $2$T$0��T", ch, NULL, arg1, TO_CHAR );
    RETURN_NULL();
  }

  if ( !arg2[0] )
  {
    for ( obj2 = ch->carrying; obj2; obj2 = obj2->next_content )
    {
      if ( obj2->wear_loc != WEAR_NONE
        && can_see_obj( ch, obj2 )
        && obj1->item_type == obj2->item_type )
          break;
    }

    if ( !obj2 )
    {
      send_to_char( "�A���W�S�����تF��i�H����C\n\r", ch );
      RETURN_NULL();
    }
  }
  else
  {
    if ( !( obj2 = get_obj_carry( ch, arg2 ) ) )
    {
      act( "�A�S���o�˸˳� $2$T$0�C", ch, NULL, arg2, TO_CHAR );
      RETURN_NULL();
    }
  }

  /* ���~�O�_�����r */
  if ( poison_char( ch, obj1 ) ) RETURN_NULL();
  if ( poison_char( ch, obj2 ) ) RETURN_NULL();

  msg    = NULL;
  value1 = value2 = 0;

  if ( obj1 == obj2 )
  {
    msg = "�A����$p�M$p�Ӥ���M���̨�Ӥ��O�@�˶ܡS";
  }
  else if ( obj1->item_type != obj2->item_type )
  {
    msg = "�A�L�k��$p�M$P����M�]�����̬O���P�������F��C";
  }
  else
  {
    switch ( obj1->item_type )
    {
    default:
      msg = "�A�S����k���$p�M$P�C";
      break;

    case ITEM_ARMOR:
      value1 = obj1->value[0];
      value2 = obj2->value[0];
      break;

    case ITEM_WEAPON:
      value1 = obj1->value[1] + obj1->value[2];
      value2 = obj2->value[1] + obj2->value[2];
      break;

    case ITEM_ARROW:
      value1 = obj1->value[0] + obj1->value[1];
      value2 = obj2->value[0] + obj2->value[1];
      break;
    }
  }

  if ( !msg )
  {
         if ( value1 == value2 ) msg = "$p�M$P�ݰ_�Ӯt���h�C";
    else if ( value1  > value2 ) msg = "$p�ݰ_�Ӥ�$P�n�C";
    else                         msg = "$p�ݰ_�Ӥ�$P�t�C";
  }

  act( msg, ch, obj1, obj2, TO_CHAR );

  message_driver( ch, obj1, ACT_WHEN_COMPARE );
  message_driver( ch, obj2, ACT_WHEN_COMPARE );

  RETURN_NULL();
}

FUNCTION( do_where )
{
  char              arg[MAX_INPUT_LENGTH];
  char              buf[MAX_STRING_LENGTH];
  CHAR_DATA       * victim;
  DESCRIPTOR_DATA * d;
  bool              found;
  int               gold;

  PUSH_FUNCTION( "do_where" );

  if ( IS_NPC( ch ) || !ch->in_room ) RETURN_NULL();

  if ( ch->in_room->NoWhere )
  {
    act( "�b$r�O�S����k��ť��O�H�����ܪ��T", ch, NULL, NULL, TO_CHAR );
    RETURN_NULL();
  }

  one_argument( argument, arg );

  if ( ch->gold < ( gold = search_cost( ch ) ) )
  {
    act( "�p����A�n�F�n�Y���D�R�A�ܤ֭n��$i�����]�l�~�����A��H�C"
      , ch, &gold, NULL, TO_CHAR );

    RETURN_NULL();
  }

  gold_from_char( ch, gold );

  clear_buffer();
  chinese_number( gold, buf );
  send_to_buffer( "�A��A���W��%s��Ȥl�����]�l�M�Ψ��D�ШӪ��񪺤p"
                  "����ť���񪺤H�M�o�{���R\n\r" , buf );

  if ( !arg[0] )
  {
    for ( found = FALSE , d = descriptor_list; d; d = d->next )
    {
      if ( !verify_desc( d ) ) continue;

      if ( d->connected == CON_PLAYING
        && ( victim = d->character )
        && !IS_NPC( victim )
        && victim->in_room
        && victim->in_room->NoWhere == FALSE
        && victim->in_room->area == ch->in_room->area
        && can_see( ch, victim )
        && can_see_mask( ch, victim )
        && ch != victim )
      {
        gold  = search_cost( ch );
        found = TRUE;
        if ( ch->gold >= gold )
        {
          chinese_number( gold, buf );
          gold_from_char( ch, gold );
          send_to_buffer( "�A��F%s��Ȥl��ť��%s���b%s����X�S�C\n\r",
            buf, mob_name( ch, victim ), victim->in_room->name );

          if ( buffer_full() ) break;
        }

        else
        {
          send_to_buffer( "�p����A���R�u�A���]�l�����M�ڤ���F�MBYEBYE�T�v\n\r" );
          break;
        }
      }
    }

    if ( !found ) send_to_buffer( "�藍�_�M�S�����󪱮a�a��A�T\n\r" );
  }

  else
  {
    for ( found = FALSE, victim = char_list; victim; victim = victim->next )
    {
      if ( !verify_char( victim ) || !victim->in_room ) continue;

      if ( victim->in_room
        && victim->in_room->NoWhere == FALSE
        && victim->in_room->area == ch->in_room->area
        && !is_affected( victim, SLOT_HIDE )
        && !is_affected( victim, SLOT_SNEAK )
        && can_see( ch, victim )
        && can_see_mask( ch, victim )
        && is_name( arg, victim->name ) )
      {
        gold  = search_cost( ch );
        found = TRUE;
        if ( ch->gold >= gold )
        {
          found = TRUE;
          gold_from_char( ch, gold );
          chinese_number( gold, buf );
          send_to_buffer( "�A��F%s��Ȥl��ť��%s\e[0m���b%s����X�S�C\n\r"
            , buf , mob_name( ch, victim ), victim->in_room->name );

          if ( buffer_full() ) break;
        }

        else
        {
          send_to_buffer( "�p����A���R�u�A���]�l�����M�ڤ���F�MBYEBYE�T�v\n\r" );
          break;
        }
      }
    }
    if ( !found ) send_to_buffer( "�藍�_�M�A�S���o�{����%s�T\n\r", arg );
  }

  print_buffer( ch );
  RETURN_NULL();
}

FUNCTION( do_lore )
{
  CHAR_DATA * victim;
  char        buf[MAX_STRING_LENGTH];
  char        arg[MAX_INPUT_LENGTH];

  PUSH_FUNCTION( "do_lore" );

  one_argument( argument, arg );

  if ( !( victim = get_char_room( ch, arg ) ) )
  {
    act( "�A�Q���q����H $2$T$0 ���b�o�̡C", ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  if ( !IS_NPC( victim )
    || ( IS_NPC( victim ) && IS_SET( victim->act, ACT_NOKILL ) ) )
  {
    send_to_char( "�ѯ������\\�A���o�ب��c����Q�C\n\r", ch );
    RETURN_NULL();
  }

  if ( victim->mount_by )
  {
    act( "$N�O�O�H�����M�C�T", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( victim->boss )
  {
    act( "$N�O�O�H�i���p���C�T", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  str_cpy( buf, return_array( LevelDiffMessage, victim->level - ch->level
            , sizeof( LevelDiffMessage ) / sizeof( LevelDiffMessage[0] ) ) );

  act( smash_act_keyword( buf, "nesNES" ), ch, NULL, victim, TO_CHAR );
  RETURN_NULL();
}

FUNCTION( do_consider )
{
  char        arg[MAX_INPUT_LENGTH];
  char        buf[MAX_STRING_LENGTH];
  CHAR_DATA * victim;
  int         diff;
  int         hpdiff;

  PUSH_FUNCTION( "do_consider" );

  one_argument( argument, arg );

  if ( !( victim = get_char_room( ch, arg ) ) )
  {
    act( "�A�Q���q����H $2$T$0 ���b�o�̡C", ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  if ( !IS_NPC( victim )
    || ( IS_NPC( victim ) && IS_SET( victim->act, ACT_NOKILL ) ) )
  {
    send_to_char( "�ѯ������\\�A���o�ب��c����Q�C\n\r", ch );
    RETURN_NULL();
  }

  if ( victim->mount_by )
  {
    act( "$N�O�O�H�����M�C�T", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( victim->boss )
  {
    act( "$N�O�O�H�i���p���C�T", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  diff = victim->level - ch->level;

  str_cpy( buf, return_array( LevelDiff2Message, -diff
            , sizeof( LevelDiff2Message ) / sizeof( LevelDiff2Message[0] ) ) );

  act( smash_act_keyword( buf, "nesNES" ) , ch, NULL, victim, TO_CHAR );

  hpdiff = ch->hit - victim->hit;

  if ( ( ( diff >= 0 ) && ( hpdiff <= 0 ) )
    || ( ( diff <= 0 ) && ( hpdiff >= 0 ) ) )
  {
    send_to_char( "�ӥB�M", ch );
  }
  else
  {
    send_to_char( "���O�M", ch );
  }

  str_cpy( buf, return_array( HpMessage, hpdiff
     , sizeof( HpMessage ) / sizeof( HpMessage[0] ) ) );

  act( smash_act_keyword( buf, "NESnes" ), ch, NULL, victim, TO_CHAR );
  RETURN_NULL();
}

void set_title( CHAR_DATA * ch, char * title )
{
  PUSH_FUNCTION( "set_title" );

  if ( IS_NPC( ch ) || !ch->pcdata || !ch->pcdata->title || !title )
    RETURN_NULL();

  free_string( ch->pcdata->title );
  ch->pcdata->title = str_dup( title );
  RETURN_NULL();
}

FUNCTION( do_cname )
{
  char buf[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "do_cname" );

  if ( IS_NPC( ch ) || !ch->pcdata ) RETURN_NULL();

  if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_SILENCE ) )
  {
    send_to_char( "�藍�_�M�A���h�F��W�����T\n\r", ch );
    RETURN_NULL();
  }

  if ( argument[0] == '\x0' )
  {
    if ( !ch->cname || !*ch->cname )
      act( "�A�ثe�S������W�r�M�A�٦�$i����W�r�����|�C"
        , ch, &ch->pcdata->rechristen, NULL, TO_CHAR );

    else
      act( "�A������W�r�� $t�M�A�٦�$I����W�r�����|�C"
        , ch, ch->cname, &ch->pcdata->rechristen, TO_CHAR );

    RETURN_NULL();
  }

  if ( ch->pcdata->rechristen <= 0 )
  {
    send_to_char( "�藍�_�M�A�S�����|�i�H��A������W���o�T\n\r", ch );
    RETURN_NULL();
  }

  if ( MaxCNameLen < MAX_INPUT_LENGTH && str_len( argument ) > MaxCNameLen )
    argument[ MaxCNameLen ] = '\x0';

  if ( str_len( argument ) < MinCNameLen )
  {
    send_to_char( "��M�A���W�r�]�ӵu�F�a�T\n\r", ch );
    RETURN_NULL();
  }

  if ( check_cname_xname( argument ) )
  {
    send_to_char( "�o�ӦW�r���n�a�T���ӦW�r�ݬݡC\n\r", ch );
    RETURN_NULL();
  }

  filter_ansi( argument );
  ansi_transcribe( argument, buf );
  smash_tilde( buf );
  smash_point( buf );
  fix_color( buf );

  sprintf( buf2, "%s�q����W���m��%s\e[0m�M�o�Ӯ����ܧִN�ǤF�}�ӡT"
    , mob_name( NULL, ch ), buf );

  free_string( ch->cname );

  ch->cname = str_dup( buf );
  ch->pcdata->rechristen--;

  act( "�A��A������W�r�令$t�M�A�٦�$I�����|�襤��W�١C"
    , ch, ch->cname, &ch->pcdata->rechristen, TO_CHAR );

  talk_channel_2( buf2, CHANNEL_BULLETIN, "�ֳ�" );

  RETURN_NULL();
}

FUNCTION( do_email )
{
  PUSH_FUNCTION( "do_email" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  if ( argument[0] == '\x0' )
  {
    if ( !ch->email || !*ch->email )
      send_to_char( "�A�ثe�S���q�l�l��H�c��}�T\n\r", ch );

    else
      act( "�A���q�l�l��H�c��}�� $t�C", ch, ch->email, NULL, TO_CHAR );

    RETURN_NULL();
  }

  if ( str_len( argument ) > 50 ) argument[50] = '\x0';

  if ( str_len( argument ) < 10 )
  {
    send_to_char( "�A���q�l�H�c����}�]�ӵu�F�a�T\n\r", ch );
    RETURN_NULL();
  }

  if ( !check_email_address( argument ) )
  {
    send_to_char( "�A���q�l�l��H�c��}���X�k�T\n\r", ch );
    RETURN_NULL();
  }

  smash_tilde( argument );
  smash_point( argument );
  free_string( ch->email );
  ch->email = str_dup( argument );
  act( "�A��A���q�l�H�c��}�令 $t�C" , ch, ch->email, NULL, TO_CHAR );
  RETURN_NULL();
}

FUNCTION( do_title )
{
  char buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "do_title" );

  if ( IS_NPC( ch ) || !ch->pcdata ) RETURN_NULL();

  if ( argument[0] == '\x0' )
  {
    if ( !ch->pcdata->title || !*ch->pcdata->title )
      send_to_char( "�A�S��������Y�W�١T\n\r", ch );

    else
      act( "�A�����Y�W�٬� $t�C", ch, ch->pcdata->title, NULL, TO_CHAR );

    RETURN_NULL();
  }

  if ( MaxCNameLen < sizeof( argument ) && str_len( argument ) > MaxCNameLen )
    argument[ MaxCNameLen ] = '\x0';

  if ( str_len( argument ) < MinCNameLen )
  {
    send_to_char( "�A�����Y�]�ӵu�F�a�T\n\r", ch );
    RETURN_NULL();
  }

  if ( check_cname_xname( argument ) )
  {
    send_to_char( "�o�ө��Y���n�a�T���ө��Y�ݬݡC\n\r", ch );
    RETURN_NULL();
  }

  ansi_transcribe( argument, buf );
  filter_ansi( buf );
  smash_tilde( buf );
  smash_point( buf );
  fix_color( buf );
  set_title( ch, buf );
  act( "�A��A�����Y�令 $t�C" , ch, ch->pcdata->title, NULL, TO_CHAR );
  RETURN_NULL();
}

FUNCTION( do_description )
{
  char arg[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "do_description" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' || !str_prefix( arg, "show" ) )
  {
    act( "�A���y�z�O�R$A$t$0", ch
      , ch->description && *ch->description ? ch->description : "�u�S���v"
      , NULL, TO_CHAR );
  }

  else if ( !str_prefix( arg, "edit" ) )
  {
    if ( !ch->desc ) RETURN_NULL();

    ch->desc->edit_mode = EDIT_SELF_DESCRIPTION;
    show_editor( ch );

    free_string( ch->stack );
    ch->stack = str_dup( "" );
  }

  else if ( !str_prefix( arg, "clear" ) )
  {
    free_string( ch->description );
    ch->description = str_dup( "" );
    send_to_char( "�t�Τw�g�M���A���y�z�C\n\r", ch );
  }

  else
  {
    send_to_char( "�A���y�k���~�M�Ьd�� description ���ϥΤ�k�T\n\r", ch );
  }

  RETURN_NULL();
}

FUNCTION( do_report )
{
  char buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "do_report" );

  print_to_char( ch
    , "�A�^���R�ͩR�O %d/%d �k�O %d/%d ��O %d/%d �g��� %d\n\r"
       , ch->hit,  get_curr_hit( ch ) , ch->mana, get_curr_mana( ch )
       , ch->move, get_curr_move( ch ) , ch->exp );

  sprintf( buf,
    "$n�^���R�ͩR�O %d/%d �k�O %d/%d ��O %d/%d �g��� %d"
    , ch->hit,  get_curr_hit( ch )  , ch->mana, get_curr_mana( ch )
    , ch->move, get_curr_move( ch ) , ch->exp   );

  act( smash_act_keyword( buf, "nesNES" ), ch, NULL, NULL, TO_ROOM );

  RETURN_NULL();
}

FUNCTION( do_wimpy )
{
  char arg[MAX_INPUT_LENGTH];
  int  wimpy;

  PUSH_FUNCTION( "do_wimpy" );

  one_argument( argument, arg );

  if ( ( wimpy = arg[0] ? atoi( arg ) : get_curr_hit( ch ) / 5 ) < 0 )
  {
    send_to_char( "�A���i��i�šM�i�O�Ч�k�]�I�ƦA�]���@�I�C\n\r", ch );
    RETURN_NULL();
  }

  if ( wimpy > get_curr_hit( ch ) )
  {
    send_to_char( "�A�������x�p�ܡS�k�]�I�Ƴ��M��A���ͩR�O�Ӫ����C\n\r", ch );
    RETURN_NULL();
  }

  ch->wimpy = wimpy;
  act( "�A�]�w���C��ͩR�O$i�I�}�l���հk�]�C", ch, &wimpy, NULL, TO_CHAR );
  RETURN_NULL();
}

FUNCTION( do_password )
{
  char   arg1[MAX_INPUT_LENGTH];
  char   arg2[MAX_INPUT_LENGTH];
  char   cEnd;
  char * pArg;
  char * pwdnew;
  char * p;

  PUSH_FUNCTION( "do_password" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  pArg = arg1;
  while ( isSpace( ( int ) *argument ) ) argument++;

  cEnd = ' ';
  if ( *argument == '\'' || *argument == '"' ) cEnd = *argument++;

  while ( *argument )
  {
    if ( *argument == cEnd )
    {
      argument++;
      break;
    }
    *pArg++ = *argument++;
  }
  *pArg = '\x0';

  pArg = arg2;
  while ( isSpace( ( int ) *argument ) ) argument++;

  cEnd = ' ';
  if ( *argument == '\'' || *argument == '"' ) cEnd = *argument++;

  while ( *argument )
  {
    if ( *argument == cEnd )
    {
      argument++;
      break;
    }
    *pArg++ = *argument++;
  }
  *pArg = '\x0';

  if ( !arg1[0] || !arg2[0] )
  {
    send_to_char( "�y�k�Rpassword <�±K�X> <�s�K�X>\n\r", ch );
    RETURN_NULL();
  }

  if ( str_cmp( crypt( arg1, ch->pcdata->pwd ), ch->pcdata->pwd ) )
  {
    send_to_char( "�±K�X���~�M���K�X�ʧ@�����C\n\r", ch );
    RETURN_NULL();
  }

  if ( !check_password( arg2 ) )
  {
    const char * pMessage;

    pMessage = ( strict_password )
      ? "�A���K�X��²��F�M�г]�w�]�[�Ʀr�M�r���B���Ӧr�H�W���K�X�T"
        "\n\r�п�J�K�X�R"
      : "�A���K�X��²��F�M�г]�w���Ӧr�H�W���K�X�T\n\r�п�J�K�X�R";

    send_to_char( pMessage, ch );
    RETURN_NULL();
  }

  pwdnew = ( char * ) crypt( arg2, ch->name );
  for ( p = pwdnew; *p; p++ )
  {
    if ( *p == '~' )
    {
      send_to_char( "�t�εL�k�����A���s�K�X�M�Э��s���C\n\r", ch );
      RETURN_NULL();
    }
  }

  free_string( ch->pcdata->pwd );
  ch->pcdata->pwd = str_dup( pwdnew );
  save_char_obj( ch , SAVE_FILE );
  send_to_char( "���K�X�ʧ@�����C\n\r", ch );
  RETURN_NULL();
}

FUNCTION( do_socials )
{
  SOCIAL_DATA * social;
  int           col = 0;

  PUSH_FUNCTION( "do_social" );

  clear_buffer();
  send_to_buffer( "\e[1;33m�Ҧ���������O�H�Ψ䤤�廡���M"
                  "�Y����L�n��������O�Чi�D�ڭ̡C\e[0m\n\r");

  for ( social = social_first; social; social = social->next )
  {
    send_to_buffer( "\e[1;37m%-10s \e[1;32m%-15s\e[0m"
      , social->name , social->chinese_name );

    if ( ++col % 3 == 0 ) send_to_buffer( "\n\r" );
    if ( buffer_full() ) break;
  }

  if ( col % 3 ) send_to_buffer( "\n\r" );
  print_buffer( ch );
  RETURN_NULL();
}

FUNCTION( do_commands )
{
  CMD_DATA * pCommand;
  int        col;

  PUSH_FUNCTION( "do_commands" );

  clear_buffer();

  for ( col = 0, pCommand = cmd_list; pCommand; pCommand = pCommand->next )
  {
    if ( pCommand->level < LEVEL_HERO && pCommand->level <= get_trust( ch ) )
    {
      send_to_buffer( "%-12s %-20s", pCommand->name , pCommand->cname );
      if ( ++col % 2 == 0 ) send_to_buffer( "\n\r" );
      if ( buffer_full() ) break;
    }
  }

  if ( col % 2 ) send_to_buffer( "\n\r" );
  print_buffer( ch );
  RETURN_NULL();
}

FUNCTION( do_channels )
{
  char arg[MAX_INPUT_LENGTH];
  bool fClear;
  int  bit;

  PUSH_FUNCTION( "do_channels" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  one_argument( argument, arg );

  if ( !arg[0] )
  {
    if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_SILENCE ) )
    {
      send_to_char( "�藍�_�M�A�Ҧ����W�D���Q�����F�C\n\r", ch );
      RETURN_NULL();
    }

    clear_buffer();
    send_to_buffer( "[AUCTION]  ����W�D �R [%s]\n\r"
                    "[CHAT]     �����W�D �R [%s]\n\r"
                    "[IMMTALK]  �����W�D �R [%s]\n\r"
                    "[MUSIC]    �����W�D �R [%s]\n\r"
                    "[QUESTION] �o���W�D �R [%s]\n\r"
                    "[SHOUT]    �R���W�D �R [%s]\n\r"
                    "[YELL]     �y�s�W�D �R [%s]\n\r"
                    "[GAMBLE]   ����W�D �R [%s]\n\r"
                    "[CLASS]    ¾�~�W�D �R [%s]\n\r"
                    "[CLUB]     �����W�D �R [%s]\n\r"
                    "[SEMOTE]   �ʧ@�W�D �R [%s]\n\r"
                    "[WEATHER]  �Ѯ�w�� �R [%s]\n\r"
                    "[PHONE]    �I�s�W�D �R [%s]\n\r"
                    "[SUICIDE]  �۵��W�D �R [%s]\n\r"
                    "[RUMOR]    �����W�D �R [%s]\n\r"
                    "[BULLETIN] �ֳ��W�D �R [%s]\n\r"
                    "[GROUP]    �ն��W�D �R [%s]\n\r"
                    "[PK]       �ޢ��W�D �R [%s]\n\r"
      , YESNO( !IS_SET( ch->deaf, CHANNEL_AUCTION  ) )
      , YESNO( !IS_SET( ch->deaf, CHANNEL_CHAT     ) )
      , YESNO( !IS_SET( ch->deaf, CHANNEL_IMMTALK  ) )
      , YESNO( !IS_SET( ch->deaf, CHANNEL_MUSIC    ) )
      , YESNO( !IS_SET( ch->deaf, CHANNEL_QUESTION ) )
      , YESNO( !IS_SET( ch->deaf, CHANNEL_SHOUT    ) )
      , YESNO( !IS_SET( ch->deaf, CHANNEL_YELL     ) )
      , YESNO( !IS_SET( ch->deaf, CHANNEL_GAMBLE   ) )
      , YESNO( !IS_SET( ch->deaf, CHANNEL_CLASS    ) )
      , YESNO( !IS_SET( ch->deaf, CHANNEL_CLUB     ) )
      , YESNO( !IS_SET( ch->deaf, CHANNEL_SEMOTE   ) )
      , YESNO( !IS_SET( ch->deaf, CHANNEL_WEATHER  ) )
      , YESNO( !IS_SET( ch->deaf, CHANNEL_PHONE    ) )
      , YESNO( !IS_SET( ch->deaf, CHANNEL_SUICIDE  ) )
      , YESNO( !IS_SET( ch->deaf, CHANNEL_RUMOR    ) )
      , YESNO( !IS_SET( ch->deaf, CHANNEL_BULLETIN ) )
      , YESNO( !IS_SET( ch->deaf, CHANNEL_GROUP    ) )
      , YESNO( !IS_SET( ch->deaf, CHANNEL_PK       ) ) );

    print_buffer( ch );
  }

  else
  {
         if ( arg[0] == '+' ) fClear = TRUE;
    else if ( arg[0] == '-' ) fClear = FALSE;
    else
    {
      send_to_char( "�A�n���}(\e[1;32m+\e[0m)�άO����"
        "(\e[1;32m-\e[0m)�����W�D�S\n\r", ch );
      RETURN_NULL();
    }

         if ( !str_prefix( arg+1, "auction"  ) ) bit = CHANNEL_AUCTION;
    else if ( !str_prefix( arg+1, "chat"     ) ) bit = CHANNEL_CHAT;
    else if ( !str_prefix( arg+1, "immtalk"  ) ) bit = CHANNEL_IMMTALK;
    else if ( !str_prefix( arg+1, "music"    ) ) bit = CHANNEL_MUSIC;
    else if ( !str_prefix( arg+1, "question" ) ) bit = CHANNEL_QUESTION;
    else if ( !str_prefix( arg+1, "shout"    ) ) bit = CHANNEL_SHOUT;
    else if ( !str_prefix( arg+1, "yell"     ) ) bit = CHANNEL_YELL;
    else if ( !str_prefix( arg+1, "gamble"   ) ) bit = CHANNEL_GAMBLE;
    else if ( !str_prefix( arg+1, "class"    ) ) bit = CHANNEL_CLASS;
    else if ( !str_prefix( arg+1, "club"     ) ) bit = CHANNEL_CLUB;
    else if ( !str_prefix( arg+1, "semote"   ) ) bit = CHANNEL_SEMOTE;
    else if ( !str_prefix( arg+1, "weather"  ) ) bit = CHANNEL_WEATHER;
    else if ( !str_prefix( arg+1, "phone"    ) ) bit = CHANNEL_PHONE;
    else if ( !str_prefix( arg+1, "suicide"  ) ) bit = CHANNEL_SUICIDE;
    else if ( !str_prefix( arg+1, "rumor"    ) ) bit = CHANNEL_RUMOR;
    else if ( !str_prefix( arg+1, "bulletin" ) ) bit = CHANNEL_BULLETIN;
    else if ( !str_prefix( arg+1, "group"    ) ) bit = CHANNEL_GROUP;
    else if ( !str_prefix( arg+1, "pk"       ) ) bit = CHANNEL_PK;
    else
    {
      send_to_char( "�A�O�n�}��(\e[1;32m+\e[0m)�άO����"
        "(\e[1;32m-\e[0m)���@���W�D�S\n\r", ch );
      RETURN_NULL();
    }

    if ( fClear ) REMOVE_BIT( ch->deaf, bit );
    else          SET_BIT   ( ch->deaf, bit );
    do_channels( ch , "" );
  }

  RETURN_NULL();
}

FUNCTION( do_turn )
{
  char arg[MAX_INPUT_LENGTH];
  bool fSet;
  int  bit;

  PUSH_FUNCTION( "do_turn" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  one_argument( argument , arg );

  if ( !arg[0] )
  {
    clear_buffer();

    send_to_buffer( "[  ����r  ]     �N�q              �ﶵ\n\r"
                    "[ \e[1;32mrebirth\e[0m  ] ���O�H��¾             %s\n\r"
                    "[ \e[1;32mtrain\e[0m    ] ���O�H�ɯ��ݩ�         %s\n\r"
                    "[ \e[1;32mpractice\e[0m ] ���O�H�V�m�ޯ�         %s\n\r"
                    "[ \e[1;32mansi\e[0m     ] �O�_�L�o�m���X       %s\n\r"
                    "[ \e[1;32mlotto\e[0m    ] �O�_�q�ʼֳz�ֳ�       %s\n\r"
      , YESNO( IS_SET( ch->turn, PLR_REBIRTH  ) )
      , YESNO( IS_SET( ch->turn, PLR_TRAIN    ) )
      , YESNO( IS_SET( ch->turn, PLR_PRACTICE ) )
      , YESNO( IS_SET( ch->turn, PLR_ANSI     ) )
      , YESNO( IS_SET( ch->turn, PLR_LOTTO    ) ) );

    print_buffer( ch );
  }

  else
  {
         if ( arg[0] == '+' ) fSet = TRUE;
    else if ( arg[0] == '-' ) fSet = FALSE;
    else
    {
      send_to_char( "Turn [\e[1;32m-\e[0m]�ﶵ �άO "
        "[\e[1;32m+\e[0m]�ﶵ\n\r", ch );

      RETURN_NULL();
    }

         if ( !str_prefix( arg+1, "rebirth"  ) ) bit = PLR_REBIRTH;
    else if ( !str_prefix( arg+1, "train"    ) ) bit = PLR_TRAIN;
    else if ( !str_prefix( arg+1, "practice" ) ) bit = PLR_PRACTICE;
    else if ( !str_prefix( arg+1, "ansi"     ) ) bit = PLR_ANSI;
    else if ( !str_prefix( arg+1, "lotto"    ) ) bit = PLR_LOTTO;
    else
    {
      send_to_char( "�䤣��A�n�]�w���ﶵ�M�Ьd�� turn ���ϥΤ�k�C\n\r", ch );
      RETURN_NULL();
    }

    if ( fSet ) SET_BIT   ( ch->turn , bit );
    else        REMOVE_BIT( ch->turn , bit );
    do_turn( ch , "" );
  }

  RETURN_NULL();
}

FUNCTION( do_notify )
{
  char arg[ MAX_INPUT_LENGTH ];
  bool fSet;
  int  bit;

  PUSH_FUNCTION( "do_notify" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  one_argument( argument , arg );

  if ( !arg[0] )
  {
    clear_buffer();

    send_to_buffer(
      "[  ����r  ]     �N�q              �ﶵ\n\r"
      "[ \e[1;32mdebug   \e[0m ]   �����T��            [%s]\n\r"
      "[ \e[1;32minfo    \e[0m ]   ��T�T��            [%s]\n\r"
      "[ \e[1;32mwizard  \e[0m ]   ��w���ڰT��        [%s]\n\r"
      "[ \e[1;32mnewplayer\e[0m]   �s���a�T��          [%s]\n\r"
      "[ \e[1;32mfailpass\e[0m ]   �K�X���~�T��        [%s]\n\r"
      "[ \e[1;32memerg   \e[0m ]   �Y�����~�T��        [%s]\n\r"
      "[ \e[1;32mcrit    \e[0m ]   �����~�T��        [%s]\n\r"
      "[ \e[1;32merr     \e[0m ]   �{�ɰT��            [%s]\n\r"
      "[ \e[1;32mxname   \e[0m ]   �����r�T��          [%s]\n\r"
      "[ \e[1;32mnet     \e[0m ]   ���ڳs�u�T��        [%s]\n\r"
      , YESNO( !IS_SET( ch->notify, NOTIFY_DEBUG     ) )
      , YESNO( !IS_SET( ch->notify, NOTIFY_INFO      ) )
      , YESNO( !IS_SET( ch->notify, NOTIFY_WIZARD    ) )
      , YESNO( !IS_SET( ch->notify, NOTIFY_NEWPLAYER ) )
      , YESNO( !IS_SET( ch->notify, NOTIFY_FAILPASS  ) )
      , YESNO( !IS_SET( ch->notify, NOTIFY_EMERG     ) )
      , YESNO( !IS_SET( ch->notify, NOTIFY_CRIT      ) )
      , YESNO( !IS_SET( ch->notify, NOTIFY_ERR       ) )
      , YESNO( !IS_SET( ch->notify, NOTIFY_XNAME     ) )
      , YESNO( !IS_SET( ch->notify, NOTIFY_NET       ) ) );

    print_buffer( ch );
  }

  else
  {
         if ( arg[0] == '+' ) fSet = TRUE;
    else if ( arg[0] == '-' ) fSet = FALSE;
    else
    {
      send_to_char( "notify [\e[1;32m-\e[0m]�ﶵ �άO "
        "[\e[1;32m+\e[0m]�ﶵ\n\r", ch );
      RETURN_NULL();
    }

         if ( !str_prefix( arg+1, "debug"     ) ) bit = NOTIFY_DEBUG;
    else if ( !str_prefix( arg+1, "info"      ) ) bit = NOTIFY_INFO;
    else if ( !str_prefix( arg+1, "wizard"    ) ) bit = NOTIFY_WIZARD;
    else if ( !str_prefix( arg+1, "newplayer" ) ) bit = NOTIFY_NEWPLAYER;
    else if ( !str_prefix( arg+1, "failpass"  ) ) bit = NOTIFY_FAILPASS;
    else if ( !str_prefix( arg+1, "emerg"     ) ) bit = NOTIFY_EMERG;
    else if ( !str_prefix( arg+1, "crit"      ) ) bit = NOTIFY_CRIT;
    else if ( !str_prefix( arg+1, "err"       ) ) bit = NOTIFY_ERR;
    else if ( !str_prefix( arg+1, "xname"     ) ) bit = NOTIFY_XNAME;
    else if ( !str_prefix( arg+1, "net"       ) ) bit = NOTIFY_NET;
    else
    {
      send_to_char( "�䤣��A�n�]�w���ﶵ�M�Ьd�� notify�C\n\r", ch );
      RETURN_NULL();
    }

    if ( fSet ) REMOVE_BIT( ch->notify , bit );
    else        SET_BIT   ( ch->notify , bit );
    do_notify( ch , "" );
  }

  RETURN_NULL();
}

FUNCTION( do_config )
{
  char arg[MAX_INPUT_LENGTH];
  bool fSet;
  int  bit;

  PUSH_FUNCTION( "do_config" );

  if ( IS_NPC(ch) ) RETURN_NULL();

  one_argument( argument, arg );

  if ( !arg[0] )
  {
    clear_buffer();

    send_to_buffer(
      "[  ����r   ]  �N�q         �ﶵ\n\r"
      "[ \e[1;32mautoexit \e[0m ]  �۰ʥX�f      %s\n\r"
      "[ \e[1;32mautoloot \e[0m ]  �۰ʷj�M����  %s\n\r"
      "[ \e[1;32mautosac  \e[0m ]  �۰ʩ^�m����  %s\n\r"
      "[ \e[1;32mblank    \e[0m ]  ���ܲŸ��Ŧ�  %s\n\r"
      "[ \e[1;32mbrief    \e[0m ]  ����ԭz      %s\n\r"
      "[ \e[1;32mcombine  \e[0m ]  ���~�զX�Ҧ�  %s\n\r"
      "[ \e[1;32mprompt   \e[0m ]  ���ܲŸ�      %s\n\r"
      "[ \e[1;32mexact    \e[0m ]  ����W��      %s\n\r"
      "[ \e[1;32mmessage  \e[0m ]  �����ԭz      %s\n\r"
      "[ \e[1;32mflee     \e[0m ]  �_�u�۰ʰk�]  %s\n\r"
      "[ \e[1;32mangel    \e[0m ]  �s��u�@��    %s\n\r"
      "[ \e[1;32mautofood \e[0m ]  �۰ʦY����    %s\n\r"
      "[ \e[1;32mautodrink\e[0m ]  �۰ʳܤ�      %s\n\r"
      "[ --------  ]  ����          %s\n\r"
      "[ --------  ]  ���వ EMOTE  %s\n\r"
      "[ --------  ]  ����ϥκ���  %s\n\r"
      ,  YESNO( IS_SET( ch->act, PLR_AUTOEXIT  ) )
      ,  YESNO( IS_SET( ch->act, PLR_AUTOLOOT  ) )
      ,  YESNO( IS_SET( ch->act, PLR_AUTOSAC   ) )
      ,  YESNO( IS_SET( ch->act, PLR_BLANK     ) )
      ,  YESNO( IS_SET( ch->act, PLR_BRIEF     ) )
      ,  YESNO( IS_SET( ch->act, PLR_COMBINE   ) )
      ,  YESNO( IS_SET( ch->act, PLR_PROMPT    ) )
      ,  YESNO( IS_SET( ch->act, PLR_EXACT     ) )
      ,  YESNO( IS_SET( ch->act, PLR_MESSAGE   ) )
      ,  YESNO( IS_SET( ch->act, PLR_FLEE      ) )
      ,  YESNO( IS_SET( ch->act, PLR_ANGEL     ) )
      ,  YESNO( IS_SET( ch->act, PLR_AUTOFOOD  ) )
      ,  YESNO( IS_SET( ch->act, PLR_AUTODRINK ) )
      ,  YESNO( IS_SET( ch->act, PLR_SILENCE   ) )
      ,  YESNO( IS_SET( ch->act, PLR_NO_EMOTE  ) )
      ,  YESNO( IS_SET( ch->act, PLR_NO_TELL   ) ) );

    print_buffer( ch );
  }
  else
  {
         if ( arg[0] == '+' ) fSet = TRUE;
    else if ( arg[0] == '-' ) fSet = FALSE;
    else
    {
      send_to_char( "config [\e[1;32m-\e[0m]�ﶵ �άO "
        "[\e[1;32m+\e[0m]�ﶵ�T\n\r", ch );
      RETURN_NULL();
    }

         if ( !str_prefix( arg+1, "autoexit"  ) ) bit = PLR_AUTOEXIT;
    else if ( !str_prefix( arg+1, "autoloot"  ) ) bit = PLR_AUTOLOOT;
    else if ( !str_prefix( arg+1, "autosac"   ) ) bit = PLR_AUTOSAC;
    else if ( !str_prefix( arg+1, "blank"     ) ) bit = PLR_BLANK;
    else if ( !str_prefix( arg+1, "brief"     ) ) bit = PLR_BRIEF;
    else if ( !str_prefix( arg+1, "combine"   ) ) bit = PLR_COMBINE;
    else if ( !str_prefix( arg+1, "prompt"    ) ) bit = PLR_PROMPT;
    else if ( !str_prefix( arg+1, "telnetga"  ) ) bit = PLR_TELNET_GA;
    else if ( !str_prefix( arg+1, "exact"     ) ) bit = PLR_EXACT;
    else if ( !str_prefix( arg+1, "message"   ) ) bit = PLR_MESSAGE;
    else if ( !str_prefix( arg+1, "angel"     ) ) bit = PLR_ANGEL;
    else if ( !str_prefix( arg+1, "flee"      ) ) bit = PLR_FLEE;
    else if ( !str_prefix( arg+1, "autofood"  ) ) bit = PLR_AUTOFOOD;
    else if ( !str_prefix( arg+1, "autodrink" ) ) bit = PLR_AUTODRINK;
    else
    {
      send_to_char( "�аݧA�n�]�w���@�ӿﶵ�S\n\r", ch );
      RETURN_NULL();
    }

    if ( fSet ) SET_BIT   ( ch->act, bit);
    else        REMOVE_BIT( ch->act, bit);

    do_config( ch , "" );
  }

  RETURN_NULL();
}

FUNCTION( do_autoexit )
{
  PUSH_FUNCTION( "do_autoexit" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  ( IS_SET ( ch->act, PLR_AUTOEXIT )
    ? do_config( ch, "-autoexit" )
    : do_config( ch, "+autoexit" ) );

  RETURN_NULL();
}

FUNCTION( do_autoloot )
{
  PUSH_FUNCTION( "do_autoloot" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  ( IS_SET( ch->act, PLR_AUTOLOOT )
    ? do_config( ch, "-autoloot" )
    : do_config( ch, "+autoloot" ) );

  RETURN_NULL();
}

FUNCTION( do_autosac )
{
  PUSH_FUNCTION( "do_autosac" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  ( IS_SET( ch->act, PLR_AUTOSAC )
    ? do_config( ch, "-autosac" )
    : do_config( ch, "+autosac" ) );

  RETURN_NULL();
}

FUNCTION( do_mode )
{
  char arg[MAX_INPUT_LENGTH];

  PUSH_FUNCTION( "do_mode" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    send_to_char( "�A�n��A����X�Ҧ��令����ˤl�O�M�Ьd�� mode /?\n\r", ch );
  }

  else if ( !str_cmp( arg, "1" ) || !str_cmp( arg, "merc" ) )
  {
    SET_BIT( ch->act, PLR_PROMPT );
    SET_BIT( ch->act, PLR_BLANK  );
    send_to_char( "�A���X�Ҧ��令 \e[1;32mMerc Mud\e[0m �Ҧ��T\n\r", ch );
  }

  else if ( !str_cmp( arg, "2" ) || !str_cmp( arg, "lp" ) )
  {
    REMOVE_BIT( ch->act, PLR_PROMPT );
    REMOVE_BIT( ch->act, PLR_BLANK  );
    send_to_char( "�A���X�Ҧ��令 \e[1;32mLP Mud\e[0m �Ҧ��T\n\r", ch );
  }

  else
  {
    send_to_char( "�A���Ѽƿ��~�M�Ьd�� mode �����T�ϥΤ�k�T\n\r", ch );
  }

  RETURN_NULL();
}

FUNCTION( do_blank )
{
  PUSH_FUNCTION( "do_blank" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  ( IS_SET( ch->act, PLR_BLANK )
    ? do_config( ch, "-blank" )
    : do_config( ch, "+blank" ) );

  RETURN_NULL();
}

FUNCTION( do_brief )
{
  PUSH_FUNCTION( "do_brief" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  ( IS_SET( ch->act, PLR_BRIEF )
    ? do_config( ch, "-brief" )
    : do_config( ch, "+brief" ) );

  RETURN_NULL();
}

FUNCTION( do_combine )
{
  PUSH_FUNCTION( "do_combine" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  ( IS_SET( ch->act, PLR_COMBINE )
    ? do_config( ch, "-combine" )
    : do_config( ch, "+combine" ) );

  RETURN_NULL();
}

FUNCTION( do_pagelen )
{
  char arg[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  int  lines;

  PUSH_FUNCTION( "do_pagelen" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  one_argument( argument, arg );

  lines = arg[0] == '\x0' ? 20 : atoi( arg );

  if ( lines < MIN_PAGELEN )
  {
    chinese_number( MIN_PAGELEN, buf );
    act( "�C�����檺�Ʀr����p��$t�C", ch, buf, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( lines > MAX_PAGELEN )
  {
    chinese_number( MAX_PAGELEN, buf );
    act( "�C�����檺�Ʀr����j��$t�C", ch, buf, NULL, TO_CHAR );
    RETURN_NULL();
  }

  ch->pcdata->pagelen = lines;
  act( "�A�]�w�C$i�洫���C", ch, &lines, NULL, TO_CHAR );
  RETURN_NULL();
}

FUNCTION( do_prompt )
{
  char buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "do_prompt" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  if ( !argument[0] )
  {
    ( IS_SET( ch->act, PLR_PROMPT )
      ? do_config( ch, "-prompt" )
      : do_config( ch, "+prompt" ) );
    RETURN_NULL();
  }

  if ( !str_cmp( argument, "all" ) )
     str_cpy( buf, "<�ͩR�O ^hp1 �k�O ^mp1 ��O ^mv1 ���� ^gol> ");

  else
  {
   if ( str_len( argument ) > 50 ) argument[50] = '\x0';
   str_cpy( buf, argument );
   smash_tilde( buf );
   smash_point( buf );
  }

  free_string( ch->prompt );
  ch->prompt = str_dup( buf );
  send_to_char( "�պA�]�w�����C\n\r", ch );
  RETURN_NULL();
}

FUNCTION( do_auto )
{
  PUSH_FUNCTION( "do_auto" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  do_config( ch, "" );
  RETURN_NULL();
}

/* �M���ù� */
FUNCTION( do_cls )
{
  PUSH_FUNCTION( "do_cls" );
  send_to_char( "\e[2J\e[H\e[0m" , ch );
  RETURN_NULL();
}

/* �[�ݦa�� �� robin ���� */
FUNCTION( do_map )
{
  PUSH_FUNCTION( "do_map" );
  do_help( ch , "map" );
  RETURN_NULL();
}

FUNCTION( do_credit )
{
  PUSH_FUNCTION( "do_credit" );
  do_help( ch, "credit" );
  RETURN_NULL();
}

FUNCTION( do_status )
{
  CMD_DATA  * pCommand;
  CMD_DATA  * aCommand;
  CMD_DATA  * bCommand;
  CMD_DATA ** zCommand;
  char        arg[ MAX_INPUT_LENGTH ];
  bool        Found;
  int         loop;
  int         count;
  int         how;
  int         total;

  PUSH_FUNCTION( "do_status" );

  one_argument( argument, arg );
  clear_buffer();

  if ( !str_cmp( arg, "all" ) )
  {
    for ( pCommand = cmd_list; pCommand; pCommand = pCommand->next )
    {
      send_to_buffer(
        "\e[1;32m%-12s\e[0m [%6d] "
        "\e[1;33m%-14s\e[0m"
        "���A�R\e[1;35m%4s\e[0m "
        "���šR[ \e[1;31m%3d\e[0m ] "
        "�аO�R\e[1;36m%-10s\e[0m\n\r"
          , pCommand->name
          , pCommand->exec
          , pCommand->cname
          , get_position( pCommand->position )
          , pCommand->level
          , get_log( pCommand->log ) );

       if ( buffer_full() ) break;
     }
  }

  else if ( !str_cmp( arg, "sort" ) )
  {
    total = count = 0;
    for ( pCommand = cmd_list; pCommand; pCommand = pCommand->next )
    {
      total += pCommand->exec;
      count++;
    }

    zCommand = alloc_string( count * sizeof( CMD_DATA * ) );

    loop = 0;
    for ( pCommand = cmd_list; pCommand; pCommand = pCommand->next, loop++ )
      zCommand[loop] = pCommand;

    for ( loop = 0; loop < count - 1; loop++ )
    {
      for ( how = loop + 1; how < count; how++ )
      {
        aCommand = zCommand[loop];
        bCommand = zCommand[how];

        if ( aCommand->exec < bCommand->exec )
        {
          zCommand[loop] = bCommand;
          zCommand[how ] = aCommand;
        }
      }
    }

    clear_buffer();
    for ( how = loop = 0; loop < count; loop++ )
    {
      pCommand = zCommand[loop];

      if ( pCommand->exec <= 0 ) continue;

      send_to_buffer(
        "\e[1;37m%3d.\e[0m \e[1;32m%-12s\e[0m %6d %3d.%d%% "
        "\e[1;33m%-14s\e[0m"
        "���A�R\e[1;35m%4s\e[0m "
        "���šR\e[1;31m%3d\e[0m\n\r"
          , ++how
          , pCommand->name
          , pCommand->exec
          , pCommand->exec * 100 / total
          , ( pCommand->exec * 1000 / total ) % 10
          , pCommand->cname
          , get_position( pCommand->position )
          , pCommand->level );

        if ( buffer_full() ) break;
    }

    free_string( ( void * ) zCommand );
  }

  else
  {
    Found = FALSE;
    for ( pCommand = cmd_list; pCommand; pCommand = pCommand->next )
    {
      if ( !str_prefix( arg, pCommand->name ) )
      {
        send_to_buffer(
          "���O�W��  �R%s\n\r"
          "����W��  �R%s\n\r"
          "�̧C���A  �R%s\n\r"
          "�аO�覡  �R%s\n\r"
          "�̧C����  �R%d\n\r"
          "���榸��  �R%d\n\r"
          "�O�_����w�R%s\n\r"
          "�O�_�w�W��R%s\n\r"
          "�O�_�n����R%s\n\r"
          "�O�_�n�ѼơR%s\n\r"
          "�Ǫ����O  �R%s\n\r"
          "����аO  �R%s\n\r"
          "������w  �R%s\n\r"
          "�ʺ�����  �R%s\n\r"
          "���`����  �R%s\n\r"
          "�R�O����  �R%s\n\r"
          "���Ѱ���  �R%s\n\r"
          "�ݩʭ���  �R%s\n\r"
          "����l��  �R%s\n\r"
          , pCommand->name, pCommand->cname
          , get_position( pCommand->position ), get_log( pCommand->log )
          , pCommand->level, pCommand->exec
          , YESNO( pCommand->canlock  )
          , YESNO( pCommand->lock     )
          , YESNO( pCommand->exact    )
          , YESNO( pCommand->argument )
          , YESNO( pCommand->mobonly  )
          , YESNO( pCommand->chat     )
          , YESNO( pCommand->wizlog   )
          , YESNO( pCommand->jail     )
          , YESNO( pCommand->dead     )
          , YESNO( pCommand->order    )
          , YESNO( pCommand->lost     )
          , YESNO( pCommand->limit    )
          , YESNO( pCommand->trace    ) );

        Found = TRUE;
        break;
      }
    }

    if ( !Found ) send_to_buffer( "�����۪񪺫��O���C\n\r" );
  }

  print_buffer( ch );
  RETURN_NULL();
}

/* �C�L�X�ͩR�O�k�O�άO��O���ʤ����� */
void show_percentage( CHAR_DATA * ch , int value , int max
                      , int color ,int frag )
{
  char buf[ MAX_STRING_LENGTH ];

  PUSH_FUNCTION( "show_percentage" );

  sprintf( buf , "\e[1;%dm%s\e[0m" , color
    , return_percentage( ch , value , max , frag ) );

  send_to_char( buf , ch );
  RETURN_NULL();
}

char * return_percentage( CHAR_DATA * ch , int value , int max , int frag )
{
  static char buf[ MAX_STRING_LENGTH ];
  int         loop;

  PUSH_FUNCTION( "return_percentage" );

  if ( max < frag )
  {
    mudlog( LOG_DEBUG , "show_percentage: %s���򥻼ƭȤp�� %d. �L�k�C�L�ʤ���."
      , ch->name ,frag );

    buf[0] = '\x0';
    RETURN( buf );
  }

  for ( buf[0] = '\x0', loop = 0 ; loop < frag ; loop++ )
  {
    if ( str_len( buf ) > sizeof( buf ) - 10 ) break;

    if ( value >= ( ( max * ( loop + 1 ) ) / frag ) ) str_cat( buf , "��" );
    else                                              str_cat( buf , "��" );
  }
  RETURN( buf );
}

/* ��ݨt�θ귽 */
FUNCTION( do_system )
{
  extern bool   signal_segv;
  extern bool   signal_fpe;
  extern bool   signal_bus;
  extern bool   signal_ill;
  extern int    traceloop;
  extern int    FunctionPointer;
  extern char * FunctionName[MAX_DEBUG];
  int           loop;
  int           count;

  PUSH_FUNCTION( "do_system" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  clear_buffer();
  send_to_buffer(
    "�C�������R%s\n\r"
    "�����Ǹ��譱�R\n\r"
    "�ثe�t�Ϊ��y���Ǹ� %d-%d�C\n\r"
    "�C���ثe%s�Q��w�C\n\r"
    "%s����h���s�u�C\n\r"
    "�t�αĥ�%s�K�X�覡�C\n\r"
    "�t�αĥ�%s�覡�ˬd�q�l�H�c�C\n\r"
    "����p�ƾ�%s�Q��w�C\n\r"
    "%s���������~�Ҧ��C\n\r"
    "����L�a�j��O�ƾ� %d\n\r"
    "�̤j�s�u�H��       %d\n\r"
    "�ثe�����ж��s��   %d\n\r"
    "�̦h�i�H���X������ %d\n\r"
    "�ثe�������ƥ�     %d\n\r"
    "�̦h�����ƥ�       %d\n\r"
    "�W�L�X�ʫH�t�βM�� %d\n\r"
    "�C�쪱�a�n�ͦW��   %d\n\r"
    "�̦h�C�쪱�a����   %d\n\r"
    "�̦h����s�u��}   %d\n\r"
    "�̦h�������Ц���   %d\n\r"
    "�̦h�C�쪱�a�P��   %d\n\r"
    "�̦h��a�ħL����   %d\n\r"
    "�̧C�൹��������   %d\n\r"
    "�ֳz�ֳ�����       %d\n\r"
    "���a�ɮ׫O�d�Ѽ�   %d\n\r"
    "���a�ɮ׭���j�p   %d �줸��\n\r"
    "�O�_����LFQDN   %s\n\r"
    "%s �s���D�k��} �T��\n\r"
    "%s �B�I�B����~ �T��\n\r"
    "%s �ѦҤ����} �T��\n\r"
    "%s �D�k���O��   �T��\n\r\n\r"
    , MUD_VERSION
    , serial_time
    , serial_loop
    , wizlock         ? "�w�g" : "�|��"
    , multilogin      ? "�w�g" : "�|��"
    , strict_password ? "�Y��" : "�@��"
    , strict_email    ? "�Y��" : "�@��"
    , tick_permit     ? "�|��" : "�w�g"
    , internal_debug  ? "�}��" : "����"
    , traceloop
    , max_connect
    , club_room
    , max_club
    , club_count()
    , max_board
    , max_notes
    , max_friend
    , max_alias
    , max_ip
    , alias_repeat
    , max_enable
    , max_hire
    , level_limit
    , lotto_paper
    , hold_day
    , file_quota
    , YESNO( fqdn_limit )
    , signal_segv ? "�}��" : "����"
    , signal_fpe  ? "�}��" : "����"
    , signal_bus  ? "�}��" : "����"
    , signal_ill  ? "�}��" : "����" );

  /* �O�_�}�Ұ����Ҧ� */
  if ( internal_debug )
  {
    SystemCrash = TRUE;
    for ( count = 0, loop = FunctionPointer + 1; loop < MAX_DEBUG; loop++ )
    {
      if ( !FunctionName[loop] ) break;
      send_to_buffer( "(%2d) ��ơR%-20s\n\r", ++count, FunctionName[loop] );
      if ( buffer_full() ) break;
    }

    SystemCrash = FALSE;
  }

  print_buffer( ch );
  RETURN_NULL();
}

/* ��ݦۤv���e����J */
FUNCTION( do_history )
{
  int               loop;
  char              arg[MAX_INPUT_LENGTH];
  CHAR_DATA       * victim;
  DESCRIPTOR_DATA * pDesc;

  PUSH_FUNCTION( "do_history" );

  /* �����Ǫ��@�w������� */
  if ( IS_NPC( ch ) ) RETURN_NULL();

  one_argument( argument, arg );
  clear_buffer();

  if ( !arg[0] )
  {
    send_to_buffer( "\n\r�A�e%d�ӿ�J�R�O���C��R\n\n\r" , MAX_HISTORY );
    victim = ch;
  }

  else if ( !IS_IMMORTAL( ch ) )
  {
    send_to_char( "�A���v�O�����ݧO�H�����O�C\n\r", ch );
    RETURN_NULL();
  }

  else
  {
    if ( !( victim = get_char_world( ch, arg ) ) )
    {
      act( "�䤣��A�n�[�ݪ���H $2$T$0�C", ch, NULL, arg, TO_CHAR );
      RETURN_NULL();
    }

    if ( IS_NPC( victim ) )
    {
      act( "$N�O�S�����O�Ȧs�Ϫ��C", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( get_trust( ch ) < get_trust( victim ) )
    {
      act( "�A���v�O������$N�����O�C", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    send_to_buffer( "\n\r%s�e%d�ӿ�J�R�O���C��R\n\n\r"
      , mob_name( ch, victim ), MAX_HISTORY );
  }

  if ( !( pDesc = victim->desc ) )
  {
    act( "$N���u�B�S���y�z��C", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  for ( loop = MAX_HISTORY - 1; loop >= 0; loop-- )
  {
    send_to_buffer( "!%d = %s\n\r" , loop + 1
      , ( pDesc->inlast[loop][0] ) ? pDesc->inlast[loop] : "(��)");

    if ( buffer_full() ) break;
  }

  print_buffer( ch );
  RETURN_NULL();
}

char * get_worktime_string( void )
{
  static char buffer[ MAX_STRING_LENGTH ];
  int         iTime;

  PUSH_FUNCTION( "get_worktime_string" );

  iTime = ( int ) difftime( current_time, mud_boot_time );
  str_cpy( buffer, date_string( iTime ) );

  RETURN( buffer );
}

char * date_string( int iTime )
{
  static char   buf[MAX_STRING_LENGTH];
  char          temp[MAX_INPUT_LENGTH];
  char          chinese[MAX_INPUT_LENGTH];
  unsigned int  work_second;
  unsigned int  work_minute;
  unsigned int  work_hour;
  unsigned int  work_day;
  bool          Status;

  PUSH_FUNCTION( "date_string" );

  buf[0] = '\x0';

  if ( iTime < 0 ) iTime = 0;

  work_second   = iTime % 60;
  iTime        /= 60;
  work_minute   = iTime % 60;
  iTime        /= 60;
  work_hour     = iTime % 24;
  work_day      = iTime / 24;
  Status        = FALSE;

  if ( work_day > 0 )
  {
    Status = TRUE;
    chinese_number( work_day, chinese );
    sprintf( temp , "%s��" , chinese );
    str_cat( buf, temp );
  }

  if ( work_hour || Status )
  {
    Status = TRUE;
    chinese_number( work_hour, chinese );
    sprintf( temp, "%s�p��" , chinese );
    str_cat( buf, temp );
  }

  if ( work_minute || Status )
  {
    chinese_number( work_minute, chinese );
    sprintf( temp, "%s��" , chinese );
    str_cat( buf, temp );
  }

  chinese_number( work_second, chinese );
  sprintf( temp, "%s��" , chinese );
  str_cat( buf, temp );

  RETURN( buf );
}

char * time_format( time_t clock, const char * formats )
{
  static char buf[ MAX_STRING_LENGTH ];
  PUSH_FUNCTION( "time_format" );
  strftime( buf, sizeof( buf ), formats, localtime( &clock ) );
  RETURN( buf );
}

char * time_string( struct timeval * timeval_info )
{
  static char buf[MAX_STRING_LENGTH];
  int    day;
  int    hour;
  int    minute;
  int    second;
  char * pString;

  PUSH_FUNCTION( "time_string" );

  str_cpy( buf , "00-00:00:00.00" );

  if ( timeval_info )
  {
    second = timeval_info->tv_sec;
    buf[0] = '\x0';

    day     = 0;
    hour    = second / 3600;
    second -= hour   * 3600;
    minute  = second / 60;
    second -= minute * 60;
    day     = hour / 24;
    hour    = hour % 24;

    sprintf( buf , "%2d-%2d:%2d:%2d.%2d"
      , UMIN( 99, day ), hour, minute , second
      , ( int ) ( timeval_info->tv_usec / 100000 ) );

    for ( pString = buf; *pString; pString++ )
      if ( *pString == ' ' ) *pString = '0';
  }

  RETURN( buf );
}

char * showtime( int iTime )
{
  static char buf[MAX_STRING_LENGTH];
  int         num;

  PUSH_FUNCTION( "showtime" );

  str_cpy( buf , "00:00:00" );

  if ( iTime > 0 )
  {
    iTime -= ( num = ( iTime / 36000 ) ) * 36000;
    buf[0] += UMIN( 9, num );

    iTime -= ( num = ( iTime / 3600 ) ) * 3600;
    buf[1] += UMIN( 9, num );

    iTime -= ( num = ( iTime / 600 ) ) * 600;
    buf[3] += UMIN( 9, num );

    iTime -= ( num = ( iTime / 60 ) ) * 60;
    buf[4] += UMIN( 9, num );

    iTime -= ( num = ( iTime / 10 ) ) * 10;
    buf[6] += UMIN( 9, num );

    buf[7] += UMIN( 9, iTime );
  }

  RETURN( buf );
}

FUNCTION( do_pry )
{
  char             arg[MAX_INPUT_LENGTH];
  char             buf[MAX_STRING_LENGTH];
  int              count;
  int              choice;
  CHAR_DATA      * victim;
  SKILL_DATA     * pSkill;
  SKILL_DATA     * aSkill;
  CLASS_DATA     * pClass;
  LIMIT_DATA     * pLimit;
  TEACH_DATA     * teach;
  RESTRICT_DATA  * pRest;
  MOB_INDEX_DATA * pMobIndex;

  char           * PryMessage[] =
  {
    "��L��",
    "�ӥB�L�Q���ַN",
    "�ڷQ�L�|�ַN",
    "�L�i��|",
    "�L���ӬO�|",
    "���O�L���ӷQ",
    "���O�L���@�N",
    "���O�L�Q�����@�N",
    "�A�i��n�ոչB��ݥL�O�_�|"
  };

  PUSH_FUNCTION( "do_pry" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  if ( ch->gold < UMAX( 1, PryCost ) )
  {
    act( "���A�ꨬ�F$i��Ȥl�A�Ӱݧa�T", ch, &PryCost, NULL, TO_CHAR );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    send_to_char( "�A�n�d�ߨ��ӧޯ�O�S\n\r", ch );
    RETURN_NULL();
  }

  if ( !( pSkill = skill_isname( arg ) ) || !pSkill->valid )
  {
    act( "�藍�_�M�䤣�쨺�ӧޯ� $2$T$0 �C�T", ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  gold_from_char( ch, UMAX( 1, PryCost ) );

  clear_buffer();
  send_to_buffer(
    "�^��W�١R%s\n\r"
    "����W�١R%s\n\r"
    "�����Z���R%s\n\r"
    "���۱оɡR%s\n\r"
    "�ޯ�\\��R%s\n\r"
    "�ޯ�����R%s\n\r"
    "���O�ƭȡR"
    , pSkill->name
    , pSkill->cname
    , weapon_type_name( pSkill->weapon )
    , pSkill->teach ? "�i�H" : "���i�H"
    , skill_type( pSkill->type )
    , pSkill->rating ? skill_rating( pSkill->rating ) : "�q��" );

  switch( pSkill->cost_type )
  {
  default:        send_to_buffer( "����\n\r"   ); break;
  case COST_HIT:  send_to_buffer( "�ͩR�O\n\r" ); break;
  case COST_MOVE: send_to_buffer( "��O\n\r"   ); break;
  case COST_MANA: send_to_buffer( "�k�O\n\r"   ); break;
  case COST_GOLD: send_to_buffer( "����\n\r"   ); break;
  }

  send_to_buffer( "�⮩�ޯ�R" );
  if ( ( aSkill = get_skill( pSkill->associate ) ) )
  {
    send_to_buffer( "%s(%s)\n\r", aSkill->cname, aSkill->name );
    send_to_buffer( "�⮩���v�R" );

    send_to_buffer( "%s", return_array( PryChance, pSkill->degree
            , sizeof( PryChance ) / sizeof( PryChance[0] ) ) );
  }
  else
  {
    send_to_buffer( "�q��\n\r" );
  }

  send_to_buffer( "�w�ƥ\\�ҡR" );

  for ( aSkill = skill_list; aSkill; aSkill = aSkill->next )
    if ( aSkill->associate == pSkill->slot ) break;

  if ( aSkill )
  {
    send_to_buffer( "%s(%s)\n\r", aSkill->cname, aSkill->name );
    send_to_buffer( "�⮩���v�R" );

    send_to_buffer( "%s", return_array( PryChance, aSkill->degree
            , sizeof( PryChance ) / sizeof( PryChance[0] ) ) );
  }
  else
  {
    send_to_buffer( "�q��\n\r" );
  }

  /* �ʧO���� */
  if ( pSkill->sex && pSkill->sex > ERRORCODE && pSkill->sex < MAX_SEX )
  {
    send_to_buffer( "\n\r¾�~�ʧO����R%s\n\r", sex_name( pSkill->sex ) );
  }

  send_to_buffer( "\n\r¾�~����R\n\r" );
  for ( pClass = class_list; pClass; pClass = pClass->next )
  {
    for ( pLimit = pSkill->limit; pLimit; pLimit = pLimit->next )
    {
      if ( pClass == pLimit->class )
      {
        chinese_number( pLimit->level, buf );

        send_to_buffer(
          "%s�h���ų̧C�n%4s�Ť~��ǲߡM���m�׳̰��i�H��%s�T\n\r"
          , class_name( pClass, FALSE )
          , buf
          , adeptation_name( pLimit->adept ) );

        break;
      }
    }
  }

  for ( pClass = class_list; pClass; pClass = pClass->next )
  {
    for ( pLimit = pSkill->limit; pLimit; pLimit = pLimit->next )
      if ( pClass == pLimit->class ) break;

    if ( !pLimit )
    {
      send_to_buffer( "%s����ǲߦ��ޯ�T\n\r", class_name( pClass, FALSE ) );
    }
  }

  send_to_buffer( "\n\r��    ��R\n\r" );
  count = 0;

  for ( pRest = pSkill->restrict; pRest; pRest = pRest->next )
  {
    chinese_number( ++count, buf );
    send_to_buffer( "%s�B���ޯ�%s\n\r", buf, restrict_value( pRest, ch ) );
  }

  if ( count == 0 ) send_to_buffer( "���ޯ�S�����󭭨�T\n\r" );

  send_to_buffer( "\n\r��    �ɡR\n\r" );
  count  = 0;
  choice = sizeof( PryMessage ) / sizeof( PryMessage[0] );

  for ( victim = char_list; victim; victim = victim->next )
  {
    if ( !verify_char( victim )
      || !IS_NPC( victim )
      || !( pMobIndex = victim->pIndexData )
      || !victim->in_room
      || !victim->in_room->area ) continue;

    for ( teach = pMobIndex->teach; teach; teach = teach->next )
    {
      if ( pSkill->slot == teach->slot )
      {
        int sel;

        chinese_number( ++count, buf );
        sel = UMAX( 0, UMIN( choice - 1, teach->inventory * choice / 100 ) );

        send_to_buffer( "%s�B%s��%s���о�%s�M%s�оɧA�M�O�� %d ��C\n\r"
          , buf, victim->in_room->area->name, mob_name( ch, victim )
          , pSkill->cname, PryMessage[sel], teach->cost );
      }
    }
  }

  if ( count == 0 ) send_to_buffer( "�藍�_�M�o�اޯ�S���H�оɡT\n\r" );
  print_buffer( ch );

  RETURN_NULL();
}

size_t add_firman( CHAR_DATA * ch, int amount )
{
  size_t count;

  PUSH_FUNCTION( "add_firman" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "add_firman: �ӷ������T." );
    RETURN( 0 );
  }

  if ( amount <= 0 )
  {
    mudlog( LOG_DEBUG, "add_firman: �W�[�ƥ� %d ���X�z.", amount );
    RETURN( 0 );
  }

  if ( ( count = UMIN( amount, MaxFirman - ch->firman ) ) <= 0 )
  {
    send_to_char( "�藍�_�M�A���K�����P�ƤӦh�F�M�ҥH�L�k�e���A�T\n\r", ch );
  }

  else
  {
    ch->firman += count;
  }

  RETURN( count );
}

int who_compare( const void * a, const void * b )
{
  char ** c = ( char ** ) a;
  char ** d = ( char ** ) b;
  DESCRIPTOR_DATA * aDesc;
  DESCRIPTOR_DATA * bDesc;

  aDesc = ( DESCRIPTOR_DATA * ) *c;
  bDesc = ( DESCRIPTOR_DATA * ) *d;
  return( bDesc->character->level > aDesc->character->level );
}

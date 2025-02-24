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
#include <stdio.h>
#include <stdlib.h>
#include "merc.h"

char *  mate_name       args( ( CHAR_DATA * ) );

void check_mate( CHAR_DATA * ch )
{
  CHAR_DATA * mate;

  PUSH_FUNCTION( "check_mate" );

  if ( !ch || !ch->pcdata )
  {
    mudlog( LOG_DEBUG, "check_mate: �ӷ������T." );
    RETURN_NULL();
  }

  ch->pcdata->mate = NULL;

  if ( !ch->pcdata->mater || !*ch->pcdata->mater )
  {
    ch->pcdata->couple.high = 0;
    ch->pcdata->couple.low  = 0;
    RETURN_NULL();
  }

  if ( !is_exist( ch->pcdata->mater ) )
  {
     act( "�A��$t$T�w�g���}�Х@�o�C"
       , ch, mate_name( ch ), ch->pcdata->mater, TO_CHAR );

    free_string( ch->pcdata->mater );

    ch->pcdata->mater       = str_dup( "" );
    ch->pcdata->couple.high = 0;
    ch->pcdata->couple.low  = 0;

    RETURN_NULL();
  }

  for ( mate = char_list; mate; mate = mate->next )
  {
    if ( verify_char( mate )
      && !IS_NPC( mate )
      && mate->desc
      && mate->desc->connected == CON_PLAYING
      && !str_cmp( mate->name, ch->pcdata->mater ) )
    {
      if ( !mate->pcdata
        || !mate->pcdata->mater
        || mate->pcdata->mate
        || str_cmp( mate->pcdata->mater, ch->name ) )
      {
        mudlog( LOG_DEBUG, "check_mate: �]�w�ɹ�H %s ���~.", ch->name );

        act( "�A��$t$T���g���äF�C"
          , ch , mate_name( ch ), ch->pcdata->mater, TO_CHAR );

        free_string( ch->pcdata->mater );
        ch->pcdata->mater = str_dup( "" );

        ch->pcdata->couple.high = 0;
        ch->pcdata->couple.low  = 0;

        RETURN_NULL();
      }

      ch->pcdata->mate   = mate;
      mate->pcdata->mate = ch;

      ch->pcdata->couple.high = mate->serial.high;
      ch->pcdata->couple.low  = mate->serial.low;

      if ( mate->pcdata )
      {
        mate->pcdata->couple.high = ch->serial.high;
        mate->pcdata->couple.low  = ch->serial.low;
      }

      act( "�A��$t$N�b�u�W��C", ch, mate_name( ch ), mate, TO_CHAR );
      act( "�A��$t$N�W�u�o�C", mate, mate_name( mate ), ch, TO_CHAR );

      RETURN_NULL();
    }
  }

  RETURN_NULL();
}

char * mate_name( CHAR_DATA * ch )
{
  PUSH_FUNCTION( "mate_name" );

  switch( ch->sex )
  {
  default:
    RETURN( "�t��" );

  case SEX_MALE:
    RETURN( "�d�l" );

  case SEX_FEMALE:
    RETURN( "�V��" );
  }
}

FUNCTION( do_marry )
{
  char        arg1[MAX_INPUT_LENGTH];
  char        arg2[MAX_INPUT_LENGTH];
  char        arg3[MAX_INPUT_LENGTH];
  char        buf[MAX_STRING_LENGTH];
  CHAR_DATA * victim;
  CHAR_DATA * other;
  CHAR_DATA * husband = NULL;
  CHAR_DATA * wife    = NULL;
  OBJ_DATA  * pObj;
  bool        selection;

  PUSH_FUNCTION( "do_marry" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  if ( !ch->pcdata )
  {
    mudlog( LOG_DEBUG, "do_marry: �ӷ������~." );
    RETURN_NULL();
  }

  if ( get_age( ch ) < MarryAge )
  {
    send_to_char( "�A���~���Ӥp�F�M�����j�H��A���a�T\n\r", ch );
    RETURN_NULL();
  }

  if ( is_affected( ch, SLOT_CHANGE_SEX ) )
  {
    send_to_char( "�ݨӧA���ʧO���I���ӹ�C�T\n\r", ch );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg1 );

  if ( arg1[0] == '\x0' )
  {
    send_to_char( "�Ѽƿ��~�M�Ьd�ߥ��T���ϥΤ�k�C\n\r", ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg1, "!woo" ) )
  {
    argument = one_argument( argument, arg2 );
               one_argument( argument, arg3 );

    if ( arg2[0] == '\x0' || arg3[0] == '\x0' )
    {
      send_to_char( "�Ѽƿ��~�M�Ьd�ߥ��T���ϥΤ�k�C\n\r", ch );
      RETURN_NULL();
    }

    if ( !( victim = get_char_room( ch, arg2 ) ) )
    {
      act( "�䤣��A�n�l�D����H $2$T$0�C", ch, NULL, arg2, TO_CHAR );
      RETURN_NULL();
    }

    if ( IS_NPC( victim ) || !victim->pcdata )
    {
      act( "$N���O�A�z�Q����Q�a�C", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( is_affected( victim, SLOT_MASK ) )
    {
      act( "$N���I�i���e�N�T", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( is_affected( victim, SLOT_CHANGE_SEX ) )
    {
      act( "�ݨ�$N�ʧO���I���ӹ�C�C", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( get_age( victim ) < MarryAge )
    {
      act( "$N���~���Ӥp�F�M���L���j�H��A���a�T", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( victim == ch )
    {
      send_to_char( "�A�O���O�Ӧ��ʤF�T\n\r", ch );
      RETURN_NULL();
    }

    if ( !can_see( victim, ch ) )
    {
      act( "$N�ݤ���A�C�C", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( victim->pcdata->mate )
    {
      act( "$N�w�g���B�F�C�M�ݨӧA�C�F�@�B�F�C", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( victim->pcdata->wooer )
    {
      act( "$N�w�g���䥦�l�D�̤F�M�ݨӧA�C�F�@�B�F�C", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( victim->position != POS_STANDING )
    {
      act( "$N���b���C�C", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( ( ch->sex ==   SEX_MALE && victim->sex == SEX_FEMALE )
      || ( ch->sex == SEX_FEMALE && victim->sex == SEX_MALE   ) )
      selection = TRUE;
    else
      selection = FALSE;

    if ( selection == FALSE )
    {
      act( "�l�D$N�O�ܦn�M���]����ʧO�˲M���C", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( !( pObj = get_obj_carry( ch, arg3 ) ) )
    {
      send_to_char( "���X�A���۷N�ӹ��T�D�B�`�����꿼�a�C\n\r", ch );
      RETURN_NULL();
    }

    if ( pObj->wear_loc != WEAR_NONE )
    {
      act( "�A�٧�$p��b���W�C�M���⥦��F�a�C", ch, pObj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( pObj->address )
    {
      act( "$p�O�n�浹�O�H���H�M���i�H�e���O�H�C", ch, pObj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    /* �ݬݬO���O�i�H��� */
    if ( !can_drop_obj( ch, pObj ) )
    {
      act( "$p�O�A���Ǯa���_�M���i�H�e���O�H�C", ch, pObj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( victim->carry_number + get_obj_number( pObj )
      > can_carry_n( victim ) )
    {
      act( "$N���G�����ʨ���h�F��C", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( get_carry_weight( victim ) + get_obj_weight( pObj )
      > can_carry_w( victim ) )
    {
      act( "$N���G�����ʨ��򭫪��F��C", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    obj_from_char( pObj );
    obj_to_char( pObj, victim );

    act( "$n����$p��@�V$N�D�B��§���T", ch, pObj, victim, TO_NOTVICT );
    act( "$n����$p��@�V�A�D�B��§���T"  , ch, pObj, victim, TO_VICT    );
    act( "�A����$p��@�V$N�D�B��§���T"  , ch, pObj, victim, TO_CHAR    );

    victim->pcdata->wooer = ch;

    if ( ch->pcdata->wooer )
    {
      if ( ch->pcdata->wooer != victim )
      {
        send_to_char( "�A���l�D�̸�O�H�]�F�C\n\r", ch->pcdata->wooer );
        ch->pcdata->wooer = NULL;
      }
      else
      {
        act( "$n�M$N�ݨӬO�ѥͪ��@��T", ch, NULL, victim, TO_NOTVICT );
        act( "$n�M�A�ݨӬO�ѥͪ��@��T"  , ch, NULL, victim, TO_VICT    );
        act( "�A�M$N�ݨӬO�ѥͪ��@��T"  , ch, NULL, victim, TO_CHAR    );
      }
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg1, "!check" ) )
  {
    if ( ch->pcdata->mate )
    {
      send_to_char( "���w�g���B�F�M�٬d����d�C\n\r", ch );
      RETURN_NULL();
    }

    if ( !( victim = ch->pcdata->wooer ) )
    {
      send_to_char( "���M�藍�_�M�A�S���l�D�̡C\n\r", ch );
      RETURN_NULL();
    }

    act( "�A���l�D�̬O����$i��$N�C", ch, &victim->level, victim, TO_CHAR );

    RETURN_NULL();
  }

  else if ( !str_prefix( arg1, "!nod" ) )
  {
    if ( ch->pcdata->mate )
    {
      send_to_char( "���w�g���B�F�M�ٵ����M�A���ȧO�H�����ڡC\n\r", ch );
      RETURN_NULL();
    }

    if ( !( victim = ch->pcdata->wooer ) || !victim->pcdata )
    {
      send_to_char( "�A�O���O�Q���B�Q�ƤF�S\n\r", ch );
      RETURN_NULL();
    }

    if ( !victim->in_room || victim->in_room != ch->in_room )
    {
      send_to_char( "���֧�A���l�D�̥s�^�ӧa�C\n\r", ch );
      RETURN_NULL();
    }

    if ( is_affected( victim, SLOT_CHANGE_SEX ) )
    {
      act( "�ݨ�$N�ʧO���I���ӹ�C�C", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( !can_see( victim, ch ) )
    {
      act( "$N�ݤ���A�C�C", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( victim->pcdata->wooer && victim->pcdata->wooer != ch )
    {
      act( "$N�w�g���䥦���l�D�̤F�C", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( victim->pcdata->wooer == ch )
    {
      act( "$N�w�g���D�A�����F�ڡC", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    victim->pcdata->wooer = ch;

    act( "$n�M$N�ݨӬO�ѥͪ��@��T", ch, NULL, victim, TO_NOTVICT );
    act( "$n�M�A�ݨӬO�ѥͪ��@��T", ch, NULL, victim, TO_VICT    );
    act( "�A�M$N�ݨӬO�ѥͪ��@��T", ch, NULL, victim, TO_CHAR    );

    RETURN_NULL();
  }

  else if ( !str_prefix( arg1, "!witness" ) )
  {
    if ( !IS_IMMORTAL( ch ) )
    {
      send_to_char( "�A���O�޲z�̡M������O�H�ұB�O�S\n\r", ch );
      RETURN_NULL();
    }

    argument = one_argument( argument, arg2 );
               one_argument( argument, arg3 );

    if ( arg2[0] == '\x0' || arg3[0] == '\x0' )
    {
      send_to_char( "�s�ѼƳ����F�M������H�a�ұB�M�{�u�@�I�աT\n\r", ch );
      RETURN_NULL();
    }

    if ( !( victim = get_char_room( ch, arg2 ) )
      || !( other  = get_char_room( ch, arg3 ) ) )
    {
      send_to_char( "�䤣��A�n�ұB���p�Ǥf�C�T\n\r", ch );
      RETURN_NULL();
    }

    if ( IS_NPC( victim )
      || IS_NPC( other )
      || !victim->pcdata
      || !other->pcdata )
    {
      send_to_char( "�A�u���n���u���v���ұB�ܡS\n\r", ch );
      RETURN_NULL();
    }

    if ( victim == ch || other == ch )
    {
      send_to_char( "�A���i�H�ۤv���ۤv�ұB�աC\n\r", ch );
      RETURN_NULL();
    }

    if ( is_affected( victim, SLOT_CHANGE_SEX )
      || is_affected( other,  SLOT_CHANGE_SEX )
      || is_affected( victim, SLOT_MASK )
      || is_affected( other,  SLOT_MASK ) )
    {
      send_to_char( "�L�̤������H�I�i�_�Ǫ��k�N�T\n\r", ch );
      RETURN_NULL();
    }

    if ( victim->sex == SEX_MALE   ) husband = victim;
    if ( other->sex  == SEX_MALE   ) husband = other;
    if ( victim->sex == SEX_FEMALE ) wife    = victim;
    if ( other->sex  == SEX_FEMALE ) wife    = other;

    if ( !husband || !wife )
    {
      send_to_char( "�L�̪��ʧO���äF�T\n\r", ch );
      RETURN_NULL();
    }

    if ( husband->pcdata->wooer != wife || wife->pcdata->wooer  != husband )
    {
      send_to_char( "�L�̨èS�����۬۷R�M�󥲫j�j�C\n\r", ch );
      RETURN_NULL();
    }

    if ( husband->pcdata->mate || wife->pcdata->mate
      || ( husband->pcdata->mater && *husband->pcdata->mater )
      || ( wife->pcdata->mater  && *wife->pcdata->mater  ) )
    {
      mudlog( LOG_DEBUG, "do_marry: �ӷ������p��." );
      husband->pcdata->wooer = NULL;
      wife->pcdata->wooer    = NULL;

      send_to_char( "�L�̨䤤���@��w�g���L�B�F�C\n\r", ch );
      RETURN_NULL();
    }

    /* �e���������H�����D */
    sprintf( buf, "�{���@���r���n�T�M���U���Ǭ��밪���M�i�O�����M�߮�v�v�C"
         "\n\r���ȭ̵�ö�����M�ɯɨ�{�����P�M%s(%s)�M%s(%s)�w�w�����ұB�j��"
         "���e�诸�w�C\n\r���ڭ̮��H�L�̤���N�����s�z�M�@�P�ͬ��T\n\r"
         ,husband->cname, husband->name
         , wife->cname  , wife->name );

    send_to_all_char( buf );

    str_cpy( buf, "�@���Ѧa�M�G������M�ҩd����M�e�J�}�СT" );

    ChatRecord = FALSE;
    do_chat( ch, buf );
    ChatRecord = TRUE;

    act( "�q������M�A�N�O$N���V�ҤF�T", husband, NULL, wife, TO_CHAR );
    act( "�q������M�A�N�O$N���d�l�F�T", wife, NULL, husband, TO_CHAR );

    husband->pcdata->mater = str_dup( wife->name    );
    wife->pcdata->mater    = str_dup( husband->name );

    husband->pcdata->mate  = wife;
    wife->pcdata->mate     = husband;

    husband->pcdata->wooer = NULL;
    wife->pcdata->wooer    = NULL;

    husband->pcdata->couple.high = wife->serial.high;
    husband->pcdata->couple.low  = wife->serial.low;

    wife->pcdata->couple.high    = husband->serial.high;
    wife->pcdata->couple.low     = husband->serial.low;

    RETURN_NULL();
  }

  else if ( !str_prefix( arg1, "!refuse" ) )
  {
    if ( !( victim = ch->pcdata->wooer ) )
    {
      send_to_char( "�A�S�S���D�R��H�M���ڵ��S\n\r", ch );
      RETURN_NULL();
    }

    act( "�A���D�R��H$N�ڵ��A�F�T", victim, NULL, ch, TO_CHAR );
    act( "�A�ڵ��F$N���D�R�ʧ@�T", ch, NULL, victim, TO_CHAR );

    ch->pcdata->wooer = NULL;
    RETURN_NULL();
  }

  else if ( !str_prefix( arg1, "!divorce" ) )
  {
    if ( !IS_IMMORTAL( ch ) )
    {
      send_to_char( "�A���O�޲z�̡M������O�H���B�O�S\n\r", ch );
      RETURN_NULL();
    }

    argument = one_argument( argument, arg2 );
               one_argument( argument, arg3 );

    if ( arg2[0] == '\x0' || arg3[0] == '\x0' )
    {
      send_to_char( "�s�ѼƳ����F�M������H�a���B�O�T\n\r", ch );
      RETURN_NULL();
    }

    if ( !( victim = get_char_room( ch, arg2 ) )
      || !( other  = get_char_room( ch, arg3 ) ) )
    {
      send_to_char( "�䤣��A�n���B���ޮa�C�T\n\r", ch );
      RETURN_NULL();
    }

    if ( IS_NPC( victim )
      || IS_NPC( other )
      || !victim->pcdata
      || !other->pcdata )
    {
      send_to_char( "�A�u���n���u���v�����B�ܡS\n\r", ch );
      RETURN_NULL();
    }

    if ( victim == ch || other == ch )
    {
      send_to_char( "�A���i�H�ۤv���ۤv���B�աC\n\r", ch );
      RETURN_NULL();
    }

    if ( victim->pcdata->mate != other || other->pcdata->mate != victim )
    {
      send_to_char( "�L�̥��ӴN���O�ҩd�F�ڡM������B�C\n\r", ch );
      RETURN_NULL();
    }

    if ( !victim->pcdata->mater
      || !*victim->pcdata->mater
      || !other->pcdata->mater
      || !*other->pcdata->mater )
    {
      send_to_char( "�L�̪���Ʀ����D�M�L�k���B�T\n\r", ch );
      mudlog( LOG_DEBUG, "do_marry: ���B�ɵ��c�����D." );
      RETURN_NULL();
    }

    free_string( victim->pcdata->mater );
    victim->pcdata->mater = str_dup( "" );
    victim->pcdata->mate = NULL;

    free_string( other->pcdata->mater );
    other->pcdata->mater  = str_dup( "" );
    other->pcdata->mate = NULL;

    victim->pcdata->couple.high = 0;
    victim->pcdata->couple.low  = 0;

    other->pcdata->couple.high  = 0;
    other->pcdata->couple.low   = 0;

    print_to_char( ch, "�A��%s(%s)�M%s(%s)���B�F�C\n\r"
      , victim->cname, victim->name, other->cname, other->name );

    act( "�A�q���M$N�A�]�S���������Y�F�C", victim, NULL, other, TO_CHAR );
    act( "�A�q���M$N�A�]�S���������Y�F�C", other, NULL, victim, TO_CHAR );

    RETURN_NULL();
  }

  else
  {
    send_to_char( "�Ѽƿ��~�M�Ьd�ߥ��T���ϥΤ�k�C\n\r", ch );
    RETURN_NULL();
  }

  RETURN_NULL();
}

bool is_couple( CHAR_DATA * ch, CHAR_DATA * victim )
{
  PUSH_FUNCTION( "is_couple" );

  if ( !ch || !victim )
  {
    mudlog( LOG_DEBUG, "is_couple: �ӷ������T." );
    RETURN( FALSE );
  }

  if ( IS_NPC( ch )
    || IS_NPC( victim )
    || !ch->pcdata
    || !ch->pcdata->mate
    || !victim->pcdata
    || !victim->pcdata->mate
    || ch->pcdata->mate != victim
    || victim->pcdata->mate != ch ) RETURN( FALSE );

  RETURN( TRUE );
}

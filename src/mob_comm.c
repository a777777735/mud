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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"

char * mprog_type_to_name      args( ( int ) );

char * mprog_type_to_name( int type )
{
  PUSH_FUNCTION( "mprog_type_to_name" );

  switch ( type )
  {
  case IN_FILE_PROG:   RETURN( "in_file_prog"   );
  case ACT_PROG:       RETURN( "act_prog"       );
  case SPEECH_PROG:    RETURN( "speech_prog"    );
  case RAND_PROG:      RETURN( "rand_prog"      );
  case FIGHT_PROG:     RETURN( "fight_prog"     );
  case HITPRCNT_PROG:  RETURN( "hitprcnt_prog"  );
  case DEATH_PROG:     RETURN( "death_prog"     );
  case ENTRY_PROG:     RETURN( "entry_prog"     );
  case GREET_PROG:     RETURN( "greet_prog"     );
  case ALL_GREET_PROG: RETURN( "all_greet_prog" );
  case GIVE_PROG:      RETURN( "give_prog"      );
  case BRIBE_PROG:     RETURN( "bribe_prog"     );
  default:             RETURN( "ERROR_PROG"     );
  }
}

FUNCTION( do_mpstat )
{
  char         arg[ MAX_INPUT_LENGTH  ];
  MPROG_DATA * mprg;
  CHAR_DATA  * victim;

  PUSH_FUNCTION( "do_mpstat" );

  one_argument( argument, arg );

  if ( !( victim = get_char_world( ch, arg ) ) )
  {
    act( "�A�n�[�ݪ���H $T ���b�o��.", ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  if ( !IS_NPC( victim ) || !victim->pIndexData )
  {
    act( "���a$N�O�S�� MOB ���{�����T", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( !( victim->pIndexData->progtypes ) )
  {
    act( "$N�S������ MOB �{���T", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( victim->disable_prog )
  {
    act( "$N�� MOB �{���Q�����T", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  clear_buffer();
  send_to_buffer(
    "�W    �r�R%s  "
    "�������X�R%d \n\r"
    "�u �y �z�R%s\n\r"
    "�� �y �z�R%s"
    "�� �R �O�R%d/%d  "
    "�k    �O�R%d/%d  "
    "�� �� �O�R%d/%d\n\r"
    "��    �šR%d "
    "¾    �~�R%s "
    "�}    ��R%d\n\r"
    "�� �@ �O�R%d "
    "��    ���R%d "
    "�g �� �ȡR%d\n\r"
    , victim->name
    , victim->pIndexData->vnum
    , victim->byname
    , victim->long_descr[0] ? victim->long_descr : "(�q��)\n\r"
    , victim->hit
    , get_curr_hit( victim )
    , victim->mana
    , get_curr_mana( victim )
    , victim->move
    , get_curr_move( victim )
    , victim->level
    , class_name( victim->class, TRUE )
    , victim->alignment
    , get_ac( victim )
    , victim->gold
    , victim->exp );

  for ( mprg = victim->pIndexData->mobprogs; mprg ; mprg = mprg->next )
  {
    send_to_buffer( ">%s %s\n\r%s\n\r"
      , mprog_type_to_name( mprg->type )
      , mprg->arglist
      , mprg->comlist );
  }

  print_buffer( ch );
  RETURN_NULL();
}

FUNCTION( do_mpasound )
{
  ROOM_INDEX_DATA * was_in_room;
  EXIT_DATA       * pexit;
  int               door;

  PUSH_FUNCTION( "do_mpasound" );

  if ( !ch->in_room )
  {
    mudlog( LOG_DEBUG, "do_mpasound: %d �ʥF�ӷ�.", ch->pIndexData->vnum );
    RETURN_NULL();
  }

  if ( !argument[0] )
  {
    mudlog( LOG_DEBUG , "Mpasound: MOB ���X %d �S���޼�."
      , ch->pIndexData->vnum );
    RETURN_NULL();
  }

  for ( was_in_room = ch->in_room, door = 0; door < DIR_MAX; door++ )
  {
    if ( ( pexit = was_in_room->exit[door] )
      && pexit->to_room
      && pexit->to_room != was_in_room )
    {
      ch->in_room = pexit->to_room;
      MOBtrigger  = FALSE;
      act( smash_act_keyword( argument, "nes" ), ch, NULL, NULL, TO_ROOM );
    }
  }

  ch->in_room = was_in_room;
  RETURN_NULL();
}

FUNCTION( do_mpkill )
{
  char        arg[ MAX_INPUT_LENGTH ];
  CHAR_DATA * victim;
  int         vnum;

  PUSH_FUNCTION( "do_mpkill" );

  vnum = ch->pIndexData->vnum;
  one_argument( argument, arg );

  if ( !arg[0] )
  {
    mudlog( LOG_DEBUG , "MpKill: MOB ���X %d �S�����󪺤޼�.", vnum );
    RETURN_NULL();
  }

  if ( !( victim = get_char_room( ch, arg ) ) )
  {
    mudlog( LOG_DEBUG , "MpKill: MOB ���X %d �n������H %s ���b�ж���."
      , vnum, arg );

    if ( ( victim = get_char_world( ch, arg ) ) )
    {
      mudlog( LOG_DEBUG, "MpKill: %d(�и�:%d)�n������H%s�b%d."
        , vnum
        , ch->in_room ? ch->in_room->vnum : -1
        , victim->name
        , victim->in_room ? victim->in_room->vnum : -1 );
    }

    RETURN_NULL();
  }

  if ( victim == ch )
  {
    mudlog( LOG_DEBUG , "MpKill: MOB ���X %d �n������H�O�ۤv.", vnum );
    RETURN_NULL();
  }

  if ( is_affected( ch, SLOT_CHARM_PERSON ) && ch->master == victim )
  {
    mudlog( LOG_DEBUG , "MpKill: �Q�g�b�� MOB ���X %d ���ۤv���D�H.", vnum );
    RETURN_NULL();
  }

  if ( ch->position == POS_FIGHTING )
  {
    mudlog( LOG_DEBUG , "MpKill: MOB ���X %d �w�g�}�l�԰��F.", vnum );
    RETURN_NULL();
  }

  striking( ch, victim, TYPE_UNDEFINED );
  RETURN_NULL();
}

FUNCTION( do_mpjunk )
{
  char       arg[ MAX_INPUT_LENGTH ];
  OBJ_DATA * obj;
  OBJ_DATA * obj_next;

  PUSH_FUNCTION( "do_mpjunk" );

  one_argument( argument, arg );

  if ( !arg[0] )
  {
    mudlog( LOG_DEBUG , "Mpjunk: MOB ���X %d �S�����󪺤޼�."
      , ch->pIndexData->vnum );
    RETURN_NULL();
  }

  if ( str_cmp( arg, "all" ) && str_prefix( "all.", arg ) )
  {
    if ( ( obj = get_obj_wear( ch, arg ) ) )
    {
      unequip_char( ch, obj, FALSE );
      extract_obj( obj );
      RETURN_NULL();
    }

    if ( !( obj = get_obj_carry( ch, arg ) ) ) RETURN_NULL();
    extract_obj( obj );
  }

  else
  {
    for ( obj = ch->carrying; obj; obj = obj_next )
    {
      obj_next = obj->next_content;
      if ( !arg[3] || is_name( &arg[4], obj->name ) )
      {
        if ( obj->wear_loc != WEAR_NONE ) unequip_char( ch, obj, FALSE );
        extract_obj( obj );
      }
    }
  }
  RETURN_NULL();
}

FUNCTION( do_mpsay )
{
  char arg[MAX_INPUT_LENGTH];

  PUSH_FUNCTION( "do_mpsay" );

  if ( !argument[0] )
  {
    mudlog( LOG_DEBUG , "Mpsay: MOB ���X %d �S�����󪺤޼�."
      , ch->pIndexData->vnum );
    RETURN_NULL();
  }

  /* �w�g����X�w�İϤF */
  if ( ch->talk_buf ) RETURN_NULL();

  argument = one_statement( argument, arg );

  /* �Y�O�Ĥ@�y�ܦ����e */
  if ( arg[0] ) do_say( ch, arg );

  /* �Y�O�᭱�٦����� */
  if ( argument[0] )
  {
    ch->talk_buf  = str_dup( argument );
    ch->talk_mode = MODE_SAY;
  }

  RETURN_NULL();
}

FUNCTION( do_mpchat )
{
  char arg[MAX_INPUT_LENGTH];

  PUSH_FUNCTION( "do_mpchat" );

  if ( !argument[0] )
  {
    mudlog( LOG_DEBUG , "Mpchat: MOB ���X %d �S�����󪺤޼�."
      , ch->pIndexData->vnum );
    RETURN_NULL();
  }

  /* �w�g����X�w�İϤF */
  if ( ch->talk_buf ) RETURN_NULL();

  argument = one_statement( argument, arg );

  /* �Y�O�Ĥ@�y�ܦ����e */
  if ( arg[0] )
  {
    ChatRecord = FALSE;
    do_chat( ch, arg );
    ChatRecord = TRUE;
  }

  /* �Y�O�᭱�٦����� */
  if ( argument[0] )
  {
    ch->talk_buf  = str_dup( argument );
    ch->talk_mode = MODE_CHAT;
  }

  RETURN_NULL();
}

FUNCTION( do_mpechoaround )
{
  char        arg[ MAX_INPUT_LENGTH ];
  CHAR_DATA * victim;

  PUSH_FUNCTION( "do_mpechoaround" );

  argument = one_argument( argument, arg );

  if ( !arg[0] )
  {
    mudlog( LOG_DEBUG , "Mpechoaround: MOB ���X %d �S�����󪺤޼�."
      , ch->pIndexData->vnum );
    RETURN_NULL();
  }

  if ( !( victim = get_char_room( ch, arg ) ) )
  {
    mudlog( LOG_DEBUG , "Mpechoaround: MOB ���X %d ����H %s ���s�b."
      , ch->pIndexData->vnum, arg );

    if ( ( victim = get_char_world( ch, arg ) ) )
    {
      mudlog( LOG_DEBUG, "Mpechoaround: %d(�и�:%d)����H%s�b%d."
        , ch->pIndexData->vnum
        , ch->in_room ? ch->in_room->vnum : -1
        , victim->name
        , victim->in_room ? victim->in_room->vnum : -1 );
    }

    RETURN_NULL();
  }

  act( smash_act_keyword( argument, "nesNES" ) , ch, NULL, victim, TO_NOTVICT );
  RETURN_NULL();
}

FUNCTION( do_mpechoat )
{
  char        arg[ MAX_INPUT_LENGTH ];
  CHAR_DATA * victim;

  PUSH_FUNCTION( "do_mpechoat" );

  argument = one_argument( argument, arg );

  if ( !arg[0] || !argument[0] )
  {
    mudlog( LOG_DEBUG , "Mpechoat: MOB ���X %d �S�����󪺤޼�."
       , ch->pIndexData->vnum );
    RETURN_NULL();
  }

  if ( !( victim = get_char_room( ch, arg ) ) )
  {
    mudlog( LOG_DEBUG , "Mpechoat: MOB ���X %d ����H %s ���s�b."
      , ch->pIndexData->vnum, arg );

    if ( ( victim = get_char_world( ch, arg ) ) )
    {
      mudlog( LOG_DEBUG, "Mpechoat: %d(�и�:%d)����H%s�b%d."
        , ch->pIndexData->vnum
        , ch->in_room ? ch->in_room->vnum : -1
        , victim->name
        , victim->in_room ? victim->in_room->vnum : -1 );
    }

    RETURN_NULL();
  }

  act( smash_act_keyword( argument, "nesNES" ) , ch, NULL, victim, TO_VICT );
  RETURN_NULL();
}

FUNCTION( do_mpecho )
{
  PUSH_FUNCTION( "do_mpecho" );

  if ( !argument[0] )
  {
    mudlog( LOG_DEBUG , "Mpecho: MOB ���X %d �S�����󪺤޼�."
      , ch->pIndexData->vnum );
    RETURN_NULL();
  }

  act( smash_act_keyword( argument, "nes" ), ch, NULL, NULL, TO_ROOM );
  RETURN_NULL();
}

FUNCTION( do_mplset )
{
  CHAR_DATA   * victim;
  SKILL_DATA  * pSkill;
  ENABLE_DATA * pEnable;
  char          arg1[MAX_INPUT_LENGTH];
  char          arg2[MAX_INPUT_LENGTH];
  char          arg3[MAX_INPUT_LENGTH];
  int           value;

  PUSH_FUNCTION( "do_mplset" );

  if ( !IS_NPC( ch ) ) RETURN_NULL();

  /* �����X�T�ӰѼ� */
  argument = one_argument( argument , arg1 );
  argument = one_argument( argument , arg2 );
  one_argument( argument , arg3 );

  /* ��ݬO�_�ѼƳ������T */
  if ( !arg1[0] || !arg2[0] || !arg3[0] )
  {
    mudlog( LOG_DEBUG, "Mplset: �Ǫ� %d �̦��]�w�����T�ޯ�."
      , ch->pIndexData->vnum );
    RETURN_NULL();
  }

  /* �����ݬO�_���o�ӤH */
  if ( !( victim = get_char_room( ch , arg1 ) ) )
  {
    mudlog( LOG_DEBUG, "Mplset: �Ǫ� %d �̧䤣��H %s �]�w�ޯ�."
      , ch->pIndexData->vnum, arg1 );

    if ( ( victim = get_char_world( ch, arg1 ) ) )
    {
      mudlog( LOG_DEBUG, "Mplset: %d(�и�:%d)����H%s�b%d."
        , ch->pIndexData->vnum
        , ch->in_room ? ch->in_room->vnum : -1
        , victim->name
        , victim->in_room ? victim->in_room->vnum : -1 );
    }

    RETURN_NULL();
  }

  /* ����@�Φb�D���a���W */
  if ( IS_NPC( victim ) ) RETURN_NULL();

  /* ��M�ޯ�W��, �Y�O�ĤG�ӰѼƬ� all�N���Χ�F */
  if ( !( pSkill = skill_isname( arg2 ) ) )
  {
    mudlog( LOG_DEBUG, "Mplset: �Ǫ� %d �̳]�w���~�ޯ� %s."
      , ch->pIndexData->vnum, arg2 );
    RETURN_NULL();
  }

  /* �ĤT�ӼƭȤ��ର�D�Ʀr */
  if ( !is_number( arg3 ) )
  {
    mudlog( LOG_DEBUG, "Mplset: �Ǫ� %d �̳]�w���~���m�� %s."
      , ch->pIndexData->vnum , arg3 );
    RETURN_NULL();
  }

  if ( ( value = atoi( arg3 ) ) < 0 || value > 100 )
  {
    mudlog( LOG_DEBUG, "Mplset: �Ǫ� %d �ҳ]�w���ޯ���m %d ���X�z.",
      ch->pIndexData->vnum, value );
    RETURN_NULL();
  }

  /* �ˬd�O�_�i�H�Ǩ� */
  if ( value > 0 && !check_skill_restrict( victim, pSkill, FALSE ) )
    RETURN_NULL();

  if ( ( victim->skill[pSkill->slot] = value ) <= 0 )
  {
    /* �����L���o���ޯ� */
    for ( pEnable = victim->enable; pEnable; pEnable = pEnable->next )
    {
      if ( pEnable->skill == pSkill )
      {
        extract_an_enable( victim, pSkill );
        break;
      }
    }
  }

  RETURN_NULL();
}

FUNCTION( do_mpmload )
{
  char             arg[ MAX_INPUT_LENGTH ];
  MOB_INDEX_DATA * pMobIndex;
  CHAR_DATA      * victim;

  PUSH_FUNCTION( "do_mpmload" );

  one_argument( argument, arg );

  if ( !arg[0] || !is_number( arg ) )
  {
    mudlog( LOG_DEBUG , "Mpmload - ���X %d �Ǫ����{���̦����~���Ѽ� %s."
      , ch->pIndexData->vnum, arg );
    RETURN_NULL();
  }

  if ( !( pMobIndex = get_mob_index( atoi( arg ) ) ) )
  {
    mudlog( LOG_DEBUG , "Mpmload - ���X %d �Ǫ����{���̦����~���Ѽ� %s."
      , ch->pIndexData->vnum, arg );
    RETURN_NULL();
  }

  if ( ch->in_room ) victim = shape_mobile( pMobIndex, ch->in_room );
  RETURN_NULL();
}

FUNCTION( do_mpoload )
{
  char             arg1[ MAX_INPUT_LENGTH ];
  char             arg2[ MAX_INPUT_LENGTH ];
  OBJ_INDEX_DATA * pObjIndex;
  OBJ_DATA       * obj;
  int              level;

  PUSH_FUNCTION( "do_mpoload" );

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );

  if ( !arg1[0] || !is_number( arg1 ) )
  {
    mudlog( LOG_DEBUG , "Mpoload - ���X %d �y�k���~�M�ѼƬ� %s."
      , ch->pIndexData->vnum, arg1 );
    RETURN_NULL();
  }

  if ( !arg2[0] )
  {
    level = get_trust( ch );
  }

  else
  {
    if ( !is_number( arg2 ) )
    {
      mudlog( LOG_DEBUG , "Mpoload - ���X %d �y�k %s ���~."
        , ch->pIndexData->vnum, arg2 );

      RETURN_NULL();
    }

    if ( ( level = atoi( arg2 ) ) < 0 )
    {
      mudlog( LOG_DEBUG , "Mpoload - ���X %d ���� %s ���~."
        , ch->pIndexData->vnum, arg2 );
      RETURN_NULL();
    }
  }

  if ( level > get_trust( ch ) )
  {
    mudlog( LOG_DEBUG , "Mpoload - ���X %d ���J���~ %d ���Ť�ۤv�� %d."
      , ch->pIndexData->vnum, level, ch->level );

    RETURN_NULL();
  }

  if ( !( pObjIndex = get_obj_index( atoi( arg1 ) ) ) )
  {
    mudlog( LOG_DEBUG , "Mpoload - ���X %d �Ѽ� %s ���~."
      , ch->pIndexData->vnum, arg1 );
    RETURN_NULL();
  }

  obj = create_object( pObjIndex, level );

  if ( obj->Takeable    ) obj_to_char( obj, ch );
  else if ( ch->in_room ) obj_to_room( obj, ch->in_room );

  RETURN_NULL();
}

FUNCTION( do_mppurge )
{
  char        arg[ MAX_INPUT_LENGTH ];
  CHAR_DATA * victim;
  CHAR_DATA * vnext;
  OBJ_DATA  * obj;
  OBJ_DATA  * obj_next;

  PUSH_FUNCTION( "do_mppurge" );

  if ( !ch->in_room )
  {
    mudlog( LOG_DEBUG, "do_mppurge: %d �ӷ������T.", ch->pIndexData->vnum );
    RETURN_NULL();
  }

  one_argument( argument, arg );

  if ( !arg[0] )
  {
    for ( victim = ch->in_room->people; victim; victim = vnext )
    {
      vnext = victim->next_in_room;
      if ( IS_NPC( victim ) && victim != ch ) extract_char( victim, TRUE );
    }

    for ( obj = ch->in_room->contents; obj; obj = obj_next )
    {
      obj_next = obj->next_content;
      extract_obj( obj );
    }

    RETURN_NULL();
  }

  if ( !( victim = get_char_room( ch, arg ) ) )
  {
    if ( ( obj = get_obj_here( ch, arg ) ) ) extract_obj( obj );
    else mudlog( LOG_DEBUG , "Mppurge - ���X %d �Ѽ� %s ���~."
      , ch->pIndexData->vnum, arg );
    RETURN_NULL();
  }

  if ( !IS_NPC( victim ) )
  {
    mudlog( LOG_DEBUG , "Mppurge - ���X %d ���~�M�����a %s."
      , ch->pIndexData->vnum, victim->name );
    RETURN_NULL();
  }

  extract_char( victim, TRUE );
  RETURN_NULL();
}

FUNCTION( do_mpgoto )
{
  char             arg[ MAX_INPUT_LENGTH ];
  ROOM_INDEX_DATA *location;

  PUSH_FUNCTION( "do_mpgoto" );

  one_argument( argument, arg );
  if ( !arg[0] )
  {
    mudlog( LOG_DEBUG , "Mpgoto - ���X %d �S���Ѽ�" , ch->pIndexData->vnum );
    RETURN_NULL();
  }

  if ( !( location = find_location( ch, arg ) ) )
  {
    mudlog( LOG_DEBUG , "Mpgoto - ���X %d �h���a�� %s �q��."
      , ch->pIndexData->vnum, arg );
    RETURN_NULL();
  }

  if ( ch->fighting ) stop_fighting( ch, TRUE );

  char_from_room( ch );
  char_to_room( ch, location );

  RETURN_NULL();
}

FUNCTION( do_mpat )
{
  char              arg[ MAX_INPUT_LENGTH ];
  ROOM_INDEX_DATA * location;
  ROOM_INDEX_DATA * original;
  CHAR_DATA       * wch;

  PUSH_FUNCTION( "do_mpat" );

  argument = one_argument( argument, arg );

  if ( !arg[0] || !argument[0] )
  {
    mudlog( LOG_DEBUG , "Mpat - ���X %d ���~���Ѽ�." , ch->pIndexData->vnum );
    RETURN_NULL();
  }

  if ( !( location = find_location( ch, arg ) ) )
  {
    mudlog( LOG_DEBUG , "Mpat - ���X %d �ާ@���a�� %s ���s�b."
      , ch->pIndexData->vnum, arg );
    RETURN_NULL();
  }

  original = ch->in_room;
  char_from_room( ch );
  char_to_room( ch, location );
  interpret( ch, argument );

  for ( wch = char_list; wch; wch = wch->next )
  {
    if ( !verify_char( wch ) ) continue;

    if ( wch == ch )
    {
      char_from_room( ch );
      char_to_room( ch, original );
      break;
    }
  }
  RETURN_NULL();
}

FUNCTION( do_mptransfer )
{
  char              arg1[ MAX_INPUT_LENGTH ];
  char              arg2[ MAX_INPUT_LENGTH ];
  ROOM_INDEX_DATA * location;
  DESCRIPTOR_DATA * d;
  CHAR_DATA       * victim;

  PUSH_FUNCTION( "do_mptransfer" );

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );

  if ( !arg1[0] )
  {
    mudlog( LOG_DEBUG , "Mptransfer - ���X %d �y�k���~."
      , ch->pIndexData->vnum );
    RETURN_NULL();
  }

  if ( !str_cmp( arg1, "all" ) )
  {
    for ( d = descriptor_list; d; d = d->next )
    {
      if ( !verify_desc( d ) ) continue;

      if ( d->connected == CON_PLAYING
        && d->character != ch
        && d->character->in_room
        && can_see( ch, d->character ) )
      {
        char buf[MAX_STRING_LENGTH];

        sprintf( buf, "%s %s", d->character->name, arg2 );
        do_transfer( ch, buf );
      }
    }
    RETURN_NULL();
  }

  if ( !arg2[0] )
  {
    location = ch->in_room;
  }
  else
  {
    if ( !( location = find_location( ch, arg2 ) ) )
    {
      mudlog( LOG_DEBUG , "Mptransfer : ���X %d �ާ@���a�� %s ���s�b."
        , ch->pIndexData->vnum, arg2 );
      RETURN_NULL();
    }

    if ( room_is_private( location ) )
    {
      mudlog( LOG_DEBUG , "Mptransfer : ���X %d �ާ@���a��O�p�H�a�� %d."
        , ch->pIndexData->vnum, location->vnum );
      RETURN_NULL();
    }
  }

  if ( !( victim = get_char_world( ch, arg1 ) ) )
  {
    mudlog( LOG_DEBUG , "Mptransfer : ���X %d �䤣��H %s."
      , ch->pIndexData->vnum , arg1 );
    RETURN_NULL();
  }

  if ( !victim->in_room )
  {
    mudlog( LOG_DEBUG , "Mptransfer : ���X %d ����H %s �b Limbo."
      , ch->pIndexData->vnum, victim->name );
    RETURN_NULL();
  }

  if ( victim->fighting ) stop_fighting( victim, TRUE );

  char_from_room( victim );
  char_to_room( victim, location );

  RETURN_NULL();
}

FUNCTION( do_mpforce )
{
  char        arg[ MAX_INPUT_LENGTH ];
  CHAR_DATA * vch;
  CHAR_DATA * vch_next;
  CHAR_DATA * victim;

  PUSH_FUNCTION( "do_mpforce" );

  argument = one_argument( argument, arg );

  if ( !arg[0] || !argument[0] )
  {
    mudlog( LOG_DEBUG , "Mpforce: ���X %d �y�k���~.", ch->pIndexData->vnum );
    RETURN_NULL();
  }

  if ( !str_cmp( arg, "all" ) )
  {
    /* ���o���ĤH�� */
    for ( vch = char_list; vch; vch = vch_next )
    {
      vch_next = vch->next;

      if ( !verify_char( vch ) ) continue;

      if ( vch->in_room == ch->in_room
        && get_trust( vch ) < get_trust( ch )
        && can_see( ch, vch ) )
      {
        interpret( vch, argument );
      }
    }
  }
  else
  {
    if ( !( victim = get_char_room( ch, arg ) ) )
    {
      mudlog( LOG_DEBUG , "Mpforce : ���X %d �䤣���H %s."
        , ch->pIndexData->vnum, arg );
      RETURN_NULL();
    }

    if ( victim == ch )
    {
      mudlog( LOG_DEBUG , "Mpforce : ���X %d �R�O�ۤv."
        , ch->pIndexData->vnum );
      RETURN_NULL();
    }

    interpret( victim, argument );
  }

  RETURN_NULL();
}

FUNCTION( do_mpadd )
{
  CHAR_DATA * victim;
  char        arg1[MAX_INPUT_LENGTH];
  char        arg2[MAX_INPUT_LENGTH];
  char        arg3[MAX_INPUT_LENGTH];
  int         location;
  int         value;

  PUSH_FUNCTION( "do_mpadd" );

  if ( !ch->in_room )
  {
    mudlog( LOG_DEBUG, "do_mpadd: ���X %d �ж��ӷ����~.", ch->pIndexData->vnum );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );
  argument = one_argument( argument, arg3 );

  if ( !arg1[0] || !arg2[0] || !arg3[0] )
  {
    mudlog( LOG_DEBUG , "do_mpadd: ���X %d �y�k���~.", ch->pIndexData->vnum );
    RETURN_NULL();
  }

       if ( !str_cmp( arg2, "str"        ) ) location = LOC_STR;
  else if ( !str_cmp( arg2, "int"        ) ) location = LOC_INT;
  else if ( !str_cmp( arg2, "wis"        ) ) location = LOC_WIS;
  else if ( !str_cmp( arg2, "dex"        ) ) location = LOC_DEX;
  else if ( !str_cmp( arg2, "con"        ) ) location = LOC_CON;
  else if ( !str_cmp( arg2, "train"      ) ) location = LOC_TRAIN;
  else if ( !str_cmp( arg2, "nskill"     ) ) location = LOC_NSKILL;
  else if ( !str_cmp( arg2, "firman"     ) ) location = LOC_FIRMAN;
  else if ( !str_cmp( arg2, "level"      ) ) location = LOC_LEVEL;
  else if ( !str_cmp( arg2, "pills"      ) ) location = LOC_PILLS;
  else if ( !str_cmp( arg2, "dosage"     ) ) location = LOC_DOSAGE;
  else if ( !str_cmp( arg2, "rechristen" ) ) location = LOC_RECHR;
  else if ( !str_cmp( arg2, "maxhit"     ) ) location = LOC_MAXHIT;
  else if ( !str_cmp( arg2, "maxmana"    ) ) location = LOC_MAXMANA;
  else if ( !str_cmp( arg2, "maxmove"    ) ) location = LOC_MAXMOVE;
  else if ( !str_cmp( arg2, "hit"        ) ) location = LOC_HIT;
  else if ( !str_cmp( arg2, "mana"       ) ) location = LOC_MANA;
  else if ( !str_cmp( arg2, "move"       ) ) location = LOC_MOVE;
  else
  {
    mudlog( LOG_DEBUG , "do_mpadd: ���X %d �Ѽ� 2 (%s) ���~."
      , ch->pIndexData->vnum, arg2 );
    RETURN_NULL();
  }

  if ( !is_number( arg3 ) )
  {
    mudlog( LOG_DEBUG , "do_mpadd: ���X %d �Ѽ�3 ���~.", ch->pIndexData->vnum );
    RETURN_NULL();
  }

  value = atoi( arg3 );

  if ( !( victim = get_char_room( ch, arg1 ) ) )
  {
    mudlog( LOG_DEBUG, "do_mpadd: �Ǫ����X %d �䤣��H�W %s."
      , ch->pIndexData->vnum, arg1 );
    RETURN_NULL();
  }

  if ( IS_NPC( victim ) )
  {
    mudlog( LOG_DEBUG, "do_mpadd: %d ��H�O�D���a.", ch->pIndexData->vnum );
    RETURN_NULL();
  }

  switch( location )
  {
  default:
    mudlog( LOG_DEBUG , "do_mpadd: ���X %d �Ѽ� 2 (%d) ���~."
      , ch->pIndexData->vnum, location );

    RETURN_NULL();

  case LOC_STR:
    victim->now_str = UMIN( MaxStr, UMAX( 1, victim->now_str + value ) );
    break;

  case LOC_WIS:
    victim->now_wis = UMIN( MaxWis, UMAX( 1, victim->now_wis + value ) );
    break;

  case LOC_INT:
    victim->now_int = UMIN( MaxInt, UMAX( 1, victim->now_int + value ) );
    break;

  case LOC_DEX:
    victim->now_dex = UMIN( MaxDex, UMAX( 1, victim->now_dex + value ) );
    break;

  case LOC_CON:
    victim->now_con = UMIN( MaxCon, UMAX( 1, victim->now_con + value ) );
    break;

  case LOC_TRAIN:
    victim->practice = UMIN( MaxPractice, UMAX( 0, victim->practice + value ) );
    break;

  case LOC_NSKILL:
    victim->nskill = UMIN( MaxNSkill, UMAX( 0, victim->nskill + value ) );
    break;

  case LOC_FIRMAN:
    victim->firman = UMIN( MaxFirman, UMAX( 0, victim->firman + value ) );
    break;

  case LOC_LEVEL:
    victim->level = UMIN( LEVEL_HERO, UMAX( 1, victim->level + value ) );
    break;

  case LOC_PILLS:

    if ( victim->pcdata ) victim->pcdata->pills =
      UMIN( 10000, UMAX( 0, victim->pcdata->pills + value ) );

    break;

  case LOC_DOSAGE:

    if ( victim->pcdata ) victim->pcdata->dosage =
      UMIN( 10000, UMAX( 0, victim->pcdata->dosage + value ) );

    break;

  case LOC_RECHR:

    if ( victim->pcdata ) victim->pcdata->rechristen =
      UMIN( 100, UMAX( 0, victim->pcdata->rechristen + value ) );

    break;

  case LOC_MAXHIT:
    victim->max_hit = UMIN( MaxHit, UMAX( 1, get_curr_hit( victim ) + value ) );
    break;

  case LOC_HIT:
    victim->hit = UMIN( victim->max_hit, UMAX( 1, victim->hit + value ) );
    break;

  case LOC_MAXMANA:
    victim->max_mana = UMIN( MaxMana, UMAX( 1, get_curr_mana( victim ) + value ) );
    break;

  case LOC_MANA:
    victim->mana = UMIN( victim->max_mana, UMAX( 0, victim->mana + value ) );
    break;

  case LOC_MAXMOVE:
    victim->max_move = UMIN( MaxMove, UMAX( 1, get_curr_move( victim ) + value ) );
    break;

  case LOC_MOVE:
    victim->move = UMIN( victim->max_move, UMAX( 0, victim->move + value ) );
    break;
  }

  RETURN_NULL();
}

FUNCTION( do_mpclean )
{
  char       arg1[MAX_INPUT_LENGTH];
  char       arg2[MAX_INPUT_LENGTH];
  bool       bExtract;
  OBJ_DATA * pObj;
  OBJ_DATA * zObj;

  PUSH_FUNCTION( "do_mpclean" );

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );

  if ( !arg1[0] )
  {
    mudlog( LOG_DEBUG , "do_mpclean: ���X %d �ʥF�ѼƤ@."
      , ch->pIndexData->vnum );
    RETURN_NULL();
  }

  if ( !str_cmp( arg1, "gold" )
    || !str_cmp( arg1, "coin" )
    || !str_cmp( arg1, "coins" ) )
  {
    for ( bExtract = FALSE, pObj = ch->carrying; pObj; pObj = zObj )
    {
      zObj = pObj->next_content;

      if ( pObj->wear_loc == WEAR_NONE && pObj->item_type == ITEM_MONEY )
      {
        bExtract = TRUE;
        obj_from_char( pObj );
        extract_obj( pObj );
      }
    }

    if ( bExtract == FALSE )
      mudlog( LOG_DEBUG , "do_mpclean: �Ǫ����X %d �S�������������."
      , ch->pIndexData->vnum, arg2 );

    RETURN_NULL();
  }

  else if ( !str_cmp( arg1, "obj" ) )
  {
    /* �S���ѼƴN�O�����P�� */
    if ( arg2[0] == '\x0' )
    {
      for ( bExtract = FALSE, pObj = ch->carrying; pObj; pObj = zObj )
      {
        zObj = pObj->next_content;

        if ( pObj->wear_loc == WEAR_NONE )
        {
          bExtract = TRUE;
          obj_from_char( pObj );
          extract_obj( pObj );
        }
      }

      if ( bExtract == FALSE )
        mudlog( LOG_DEBUG , "do_mpclean: �Ǫ����X %d �S���������󪫫~."
        , ch->pIndexData->vnum, arg2 );

      RETURN_NULL();
    }

    for ( pObj = ch->carrying; pObj; pObj = pObj->next_content )
    {
      if ( !str_cmp( arg2, pObj->name ) && pObj->wear_loc == WEAR_NONE )
      {
        obj_from_char( pObj );
        extract_obj( pObj );
        RETURN_NULL();
      }
    }

    mudlog( LOG_DEBUG , "do_mpclean: ���X %d �Ѽ�(2) %s ���~."
      , ch->pIndexData->vnum, arg2 );

    RETURN_NULL();
  }

  else
  {
    mudlog( LOG_DEBUG , "do_mpclean: ���X %d �Ѽ�(1) %s ���~."
      , ch->pIndexData->vnum, arg1 );

    RETURN_NULL();
  }

  RETURN_NULL();
}

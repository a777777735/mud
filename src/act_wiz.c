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
#include <stdlib.h>
#include <string.h>
#include "merc.h"

#define NEXT_COLUMN	2

FUNCTION( do_wizhelp )
{
  CMD_DATA * pCommand;
  int        turn;
  char       command[MAX_INPUT_LENGTH];

  PUSH_FUNCTION( "do_wizhelp" );

  one_argument( argument , command );
  clear_buffer();

  if ( !command[0] )
  {
    for ( turn = 0, pCommand = cmd_list; pCommand; pCommand = pCommand->next )
    {
      if ( pCommand->level > LEVEL_HERO
        && pCommand->level <= get_trust( ch )
        && !pCommand->mobonly )
      {
        send_to_buffer( "%-12s %-20s" , pCommand->name , pCommand->cname );
        if ( ( ++turn % NEXT_COLUMN ) == 0 ) send_to_buffer( "\n\r" );
        if ( buffer_full() ) break;
      }
    }
    send_to_buffer( "\n\r" );
  }

  else if ( !str_cmp( command , "-a" ) )
  {
    for ( pCommand = cmd_list; pCommand; pCommand = pCommand->next )
    {
      if ( pCommand->level > LEVEL_HERO
        && pCommand->level <= get_trust( ch ) )
        send_to_buffer( "���O \e[1;32m%-12s\e[0m "
                        "���� \e[1;33m%-14s\e[0m "
                        "���A:\e[1;35m%-4s\e[0m "
                        "����:[ \e[1;31m%3d\e[0m ] "
                        "�аO���A:\e[1;36m%-10s\e[0m\n\r"
          , pCommand->name
          , pCommand->cname
          , get_position( pCommand->position )
          , pCommand->level
          , get_log( pCommand->log ) );

       if ( buffer_full() ) break;
     }
   }

   else
   {
     send_to_char( "�Ѽƿ��~�C\n\r", ch );
     RETURN_NULL();
   }

   print_buffer( ch );
   RETURN_NULL();
}

#undef NEXT_COLUMN

/* ��ﯫ�ڲ{���� bamfin */
FUNCTION( do_bamfin )
{
  char buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "do_bamfin" );

  if ( !IS_NPC( ch ) )
  {
    ansi_transcribe( argument, buf );
    smash_tilde( argument );

    free_string( ch->pcdata->bamfin );
    ch->pcdata->bamfin = str_dup( buf );

    if ( *argument )
      act( "�A�⯫�ڲ{���r��אּ$t�C", ch, buf, NULL, TO_CHAR );

    else
      send_to_char( "�A��A�����ڲ{���r��R���F�C\n\r" , ch );
  }

  RETURN_NULL();
}

/* ��ﯫ�������r�� bamfout */
FUNCTION( do_bamfout )
{
  char buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "do_bamfout" );

  if ( !IS_NPC( ch ) )
  {
    ansi_transcribe( argument, buf );
    smash_tilde( argument );

    free_string( ch->pcdata->bamfout );
    ch->pcdata->bamfout = str_dup( buf );

    if ( * argument )
      act( "�A�⯫�����Φr��אּ$t�C", ch, buf, NULL, TO_CHAR );

    else
      send_to_char( "�A��A���������Φr��R���F�C\n\r" , ch );
  }

  RETURN_NULL();
}

/* �⪱�a�C���ڵ����Ӥ� */
FUNCTION( do_deny )
{
  char        arg[MAX_INPUT_LENGTH];
  CHAR_DATA * victim;

  PUSH_FUNCTION( "do_deny" );

  one_argument( argument, arg );

  if ( !( victim = get_char_world( ch, arg ) ) )
  {
    send_to_char( "�䤣��A�n�ڵ����H�C\n\r", ch );
    RETURN_NULL();
  }

  if ( IS_NPC( victim ) || !victim->pcdata )
  {
    act( "����Φb�D���a$N���W�C", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( get_trust( victim ) >= get_trust( ch ) )
  {
    act( "�A�����ŨèS����$N���C", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  victim->pcdata->deny = TRUE;
  send_to_char( "�A�w�g�Q�C�J�ڵ����Ӥ�F�C\n\r", victim );
  act( "�n�F�M�A��$N���C�J�ڵ����ӨϥΪ̡C", ch, NULL, victim, TO_CHAR );
  do_quit( victim, "" );
  RETURN_NULL();
}

FUNCTION( do_disconnect )
{
  char              arg[MAX_INPUT_LENGTH];
  DESCRIPTOR_DATA * d;
  CHAR_DATA       * victim;

  PUSH_FUNCTION( "do_disconnect" );

  one_argument( argument, arg );

  if ( !( victim = get_char_world( ch, arg ) ) )
  {
    send_to_char( "�䤣��A�Q���_�s�u���H�C\n\r", ch );
    RETURN_NULL();
  }

  if ( !victim->desc )
  {
    act( "$N�èS���y�z��M�L�k���_�s�u�C", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  for ( d = descriptor_list; d ; d = d->next )
  {
    if ( !verify_desc( d ) ) continue;

    if ( d == victim->desc )
    {
      act( "�n�F�M�A��$N���s�u�����_�F�C", ch, NULL, victim, TO_CHAR );
      close_socket( d );
      RETURN_NULL();
    }
  }

  mudlog( LOG_DEBUG , "Do_disconnect: �y�z��䤣��." );
  act( "���o�Ӫ��a$N�M�o�䤣��L���y�z��C", ch, NULL, victim, TO_CHAR );
  RETURN_NULL();
}

/* �Ǯ����a */
FUNCTION( do_pardon )
{
  char         arg1[MAX_INPUT_LENGTH];
  char         arg2[MAX_INPUT_LENGTH];
  int          count;
  CHAR_DATA  * victim;
  ENEMY_DATA * pEnemy;

  PUSH_FUNCTION( "do_pardon" );

  argument = one_argument( argument, arg1 );

  if ( arg1[0] == '!' )
  {
    if ( arg1[1] == '\x0' )
    {
      send_to_char( "�A�n�d�߭��@�ӤH�O�S\n\r", ch );
      RETURN_NULL();
    }

    if ( !( victim = get_char_world( ch, arg1 + 1 ) ) )
    {
      send_to_char( "�䤣��A�n�d�ߪ��H�C\n\r", ch );
      RETURN_NULL();
    }

    if ( IS_NPC( victim ) )
    {
      act( "�D���a$N�S���o�@�譱����ơC", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    clear_buffer();
    send_to_buffer(
      "%s���Ǹo��Ʀ��R\n\r���H�� %s\n\r�s��   %s\n\r�q���� %s\n\r"
      , mob_name( NULL, victim )
      , YESNO( IS_SET( victim->act, PLR_KILLER ) )
      , YESNO( IS_SET( victim->act, PLR_THIEF  ) )
      , YESNO( IS_SET( victim->act, PLR_BOLTER ) ) );

    if ( victim->enemy )
    {
      send_to_buffer( "���ĸ�Ʀ��R\n\r" );
      for ( count = 0, pEnemy = victim->enemy; pEnemy; pEnemy = pEnemy->next )
        send_to_buffer( "%2d. %s\n\r", ++count, pEnemy->city );
    }

    print_buffer( ch );

    RETURN_NULL();
  }

  one_argument( argument, arg2 );

  if ( !arg1[0] || !arg2[0] )
  {
    send_to_char( "�A���y�k���~�M�Ьd�ߨϥΤ�k�C\n\r", ch );
    RETURN_NULL();
  }

  if ( !( victim = get_char_world( ch, arg1 ) ) )
  {
    send_to_char( "�䤣��A�n�Ǯ����H�C\n\r", ch );
    RETURN_NULL();
  }

  if ( IS_NPC( victim ) )
  {
    act( "�D���a $N �S���o�@�譱����ơC", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "killer" ) )
  {
    if ( IS_SET( victim->act, PLR_KILLER ) )
    {
      REMOVE_BIT( victim->act, PLR_KILLER );
      act( "$N���H�Ǫ��X�Фw�g�M�������C", ch, NULL, victim, TO_CHAR );
      send_to_char( "�P�¯��M�A���A�O���H�ǤF�C\n\r", victim );
    }

    else
    {
      act( "$N���ӴN���O���H�ǡM�ʧ@�����C" , ch, NULL, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "thief" ) )
  {
    if ( IS_SET( victim->act, PLR_THIEF ) )
    {
      REMOVE_BIT( victim->act, PLR_THIEF );
      act( "$N�Ѹ�X�Фw�g���������C", ch, NULL, victim, TO_CHAR );
      send_to_char( "�P�¯��M�A���A�O�Ѹ�F�C\n\r", victim );
    }

    else
    {
      act( "$N���ӴN���O�Ѹ�M�ʧ@�����C" , ch, NULL, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "bolter" ) )
  {
    if ( IS_SET( victim->act, PLR_BOLTER ) )
    {
      REMOVE_BIT( victim->act, PLR_BOLTER );
      act( "$N�q���̪��X�Фw�g�M�������C", ch, NULL, victim, TO_CHAR );
      send_to_char( "�P�¯��M�A���A�O�q���̤F�C\n\r", victim );
    }

    else
    {
      act( "$N���ӴN���O�q���̡M�ʧ@�����C", ch, NULL, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  if ( check_enemy( victim, arg2 ) )
  {
    act( "�A����$N��$t���ĺX�СC", ch, arg2, victim, TO_CHAR );

    if ( victim != ch )
      act( "�P�¯��M�����A$t�����ĺX�СC", victim, arg2, NULL, TO_CHAR );

    rem_enemy( victim, arg2 );
    RETURN_NULL();
  }

  send_to_char( "�S���o������r�M�Ьd�� pardon �ϥΤ�k�C\n\r", ch );
  RETURN_NULL();
}

FUNCTION( do_aecho )
{
  char              arg[MAX_INPUT_LENGTH];
  char              buf[MAX_STRING_LENGTH];
  int               count;
  DESCRIPTOR_DATA * pDesc;

  PUSH_FUNCTION( "do_aecho" );

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' || argument[0] == '\x0' )
  {
    send_to_char( "�A�n��o�Ӧ�}�s������O�S\n\r", ch );
    RETURN_NULL();
  }

  ansi_transcribe( argument, buf );

  for ( count = 0, pDesc = descriptor_list; pDesc; pDesc = pDesc->next )
  {
    if ( verify_desc( pDesc )
      && pDesc->connected == CON_PLAYING
      && pDesc->character
      && pDesc->character != ch
      && ( !str_cmp( pDesc->host, arg ) || !str_cmp( pDesc->address, arg ) ) )
    {
      act( "�u$2��}�s��$0�v $7$t$0", pDesc->character, buf, NULL, TO_CHAR );
      count++;
    }
  }

  if ( count <= 0 )
  {
    send_to_char( "�藍�_�M�䤣��o�Ӧ�}�����󪱮a�C\n\r", ch );
  }
  else
  {
    act( "�A�`�@��$i�쪱�a�s���C", ch, &count, NULL, TO_CHAR );
  }

  RETURN_NULL();
}

/* ���ڼs�� */
FUNCTION( do_echo )
{
  DESCRIPTOR_DATA * man;
  char              buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "do_echo" );

  if ( !argument || !*argument ) RETURN_NULL();

  ansi_transcribe( argument, buf );

  for ( man = descriptor_list; man; man = man->next )
  {
    if ( verify_desc( man )
      && man->connected == CON_PLAYING
      && man->character )
    {
      act( "�u$2����s��$0�v $7$t$0", man->character, buf, NULL, TO_CHAR );
    }
  }

  RETURN_NULL();
}

/* �b�Y���ж����s�� */
FUNCTION( do_recho )
{
  DESCRIPTOR_DATA * d;
  char              buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "do_recho" );

  if ( !ch || !ch->in_room )
  {
    mudlog( LOG_DEBUG, "do_recho: �Ѽƿ��~." );
    RETURN_NULL();
  }

  ansi_transcribe( argument, buf );
  for ( d = descriptor_list; d; d = d->next )
  {
    if ( !verify_desc( d ) ) continue;

    if ( d->connected == CON_PLAYING
      && d->character
      && d->character->in_room == ch->in_room )
    {
      act( "�u$2�ж��s��$0�v $7$t$0", d->character, buf, NULL, TO_CHAR );
    }
  }
  RETURN_NULL();
}

ROOM_INDEX_DATA * find_location( CHAR_DATA * ch, char * arg )
{
  CHAR_DATA * victim;
  OBJ_DATA  * obj;

  PUSH_FUNCTION( "find_location" );

  if ( is_number(arg) ) RETURN( get_room_index( atoi( arg ) ) );
  if ( ( victim = get_char_world( ch, arg ) ) ) RETURN( victim->in_room );
  if ( ( obj = get_obj_world( ch, arg ) ) )     RETURN( obj->in_room );
  RETURN( NULL );
}

FUNCTION( do_transfer )
{
  char              arg1[MAX_INPUT_LENGTH];
  char              arg2[MAX_INPUT_LENGTH];
  char              arg3[ MAX_INPUT_LENGTH];
  ROOM_INDEX_DATA * location;
  DESCRIPTOR_DATA * d;
  CHAR_DATA       * victim;

  PUSH_FUNCTION( "do_transfer" );

  argument = one_argument( argument, arg1 );
             one_argument( argument, arg2 );

  if ( !str_cmp( arg1, "all" ) )
  {
    for ( d = descriptor_list; d; d = d->next )
    {
      if ( !verify_desc( d ) ) continue;

      if ( d->connected == CON_PLAYING
        && d->character
        && d->character != ch
        && d->character->in_room
        && can_see( ch, d->character ) )
      {
        sprintf( arg3, "%s %s", d->character->name, arg2 );
        do_transfer( ch, arg3 );
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
      send_to_char( "�S���o�Ӧa��C\n\r", ch );
      RETURN_NULL();
    }

    if ( room_is_private( location ) )
    {
      send_to_char( "�����ж��O�p�H�ж��C\n\r", ch );
      RETURN_NULL();
    }
  }

  if ( !( victim = get_char_world( ch, arg1 ) ) )
  {
    send_to_char( "�䤣��A�n�ǰe���H�C\n\r", ch );
    RETURN_NULL();
  }

  if ( ch == victim )
  {
    send_to_char( "�F�ܶǰe�ۤv�O�S\n\r", ch );
    RETURN_NULL();
  }

  if ( !victim->in_room )
  {
    act( "$N�b��L�ƴ���Limbo�M�ҥH����Q�ǰe�C", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( is_dead( victim ) )
  {
    act( "$N�٦b�B�z�L������j�ƭC�T", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( victim->fighting ) stop_fighting( victim, TRUE );

  act( "$n�����b�@�ε��������C", victim, NULL, NULL, TO_ROOM );
  char_from_room( victim );
  char_to_room( victim, location );

  act( "$n�b�@�ε��������F�X�ӡC", victim, NULL, NULL, TO_ROOM );
  if ( ch != victim ) act( "$n���b�ǰe�A�C", ch, NULL, victim, TO_VICT );

  do_look( victim, "auto" );
  clear_trace( victim, TRUE );

  send_to_char( "�n�F�M�ǰe�����C\n\r", ch );
}

FUNCTION( do_at )
{
  char              arg[MAX_INPUT_LENGTH];
  ROOM_INDEX_DATA * location;
  ROOM_INDEX_DATA * original;
  CHAR_DATA       * wch;

  PUSH_FUNCTION( "do_at" );

  argument = one_argument( argument, arg );

  if ( !arg[0] || !argument[0] )
  {
    send_to_char( "�y�k�Rat <�a�I> <�ҭn���檺�ʧ@>\n\r", ch );
    RETURN_NULL();
  }

  if ( !( location = find_location( ch, arg ) ) )
  {
    send_to_char( "�S���A�n���a��C\n\r", ch );
    RETURN_NULL();
  }

  if ( room_is_private( location ) )
  {
    send_to_char( "�o�O�@���p�H�ж��C\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->in_room == location )
  {
    send_to_char( "�o�O�ۦP���ж��M���ݭn�o�˰��a�T\n\r", ch );
    RETURN_NULL();
  }

  original = ch->in_room;
  char_from_room( ch );
  char_to_room( ch, location );
  interpret( ch, argument );

  /* See if 'ch' still exists before continuing!
     Handles 'at XXXX quit' case. */

  for ( wch = char_list; wch; wch = wch->next )
  {
    if ( verify_char( wch ) && wch == ch )
    {
      char_from_room( ch );
      char_to_room( ch, original );
      break;
    }
  }

  RETURN_NULL();
}

FUNCTION( do_goto )
{
  CHAR_DATA       * victim;
  DESCRIPTOR_DATA * pDesc;
  char              arg[MAX_INPUT_LENGTH];
  ROOM_INDEX_DATA * location;

  PUSH_FUNCTION( "do_goto" );

  one_argument( argument, arg );

  if ( ch->position == POS_DEAD )
  {
    act( "�Х��_���A���a�T", ch, NULL, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( arg[0] == '@' )
  {
    if ( arg[1] == '\x0' || !ch->in_room )
    {
      send_to_char( "�A�n������쪱�a����O�S\n\r", ch );
      RETURN_NULL();
    }

    location = NULL;
    for ( pDesc = descriptor_list; pDesc; pDesc = pDesc->next )
    {
      if ( verify_desc( pDesc )
        && ( victim = pDesc->character )
        && pDesc->connected == CON_PLAYING
        && !str_cmp( victim->name, arg + 1 )
        && can_see( ch, victim )
        && victim->in_room )
      {
        location = victim->in_room ;
        break;
      }
    }
  }

  else
  {
    location = find_location( ch, arg );
  }

  if ( !location )
  {
    send_to_char( "�S���o�Ӧa��T\n\r", ch );
    RETURN_NULL();
  }

  if ( room_is_private( location ) )
  {
    send_to_char( "�藍�_�M�o�өж��O�p�H�ж��C\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->in_room == location )
  {
    send_to_char( "�A�h���a��M�A�ثe���Ҧb�O�@�˪��a��C\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->fighting ) stop_fighting( ch, TRUE );

  if ( !IS_NPC( ch ) && !IS_SET( ch->act, PLR_WIZINVIS ) )
  {
    act( "$n$T�C", ch, NULL,
        ( ch->pcdata && *ch->pcdata->bamfout )
        ? ch->pcdata->bamfout
        : "�b�@�}�g���ɫB���v�������L�����v", TO_ROOM );
  }

  char_from_room( ch );
  char_to_room( ch, location );

  if ( !IS_NPC( ch ) && !IS_SET( ch->act, PLR_WIZINVIS ) )
  {
    act( "$n$T�C", ch, NULL,
        ( ch->pcdata && *ch->pcdata->bamfin )
        ? ch->pcdata->bamfin
        : "�b�@�}�g���ɫB���v���{�X�L���j�����v", TO_ROOM );
  }

  do_look( ch, "auto" );
  clear_trace( ch, TRUE );

  RETURN_NULL();
}

/* �[�ݩж������A */
FUNCTION( do_rstat )
{
  char              buf[MAX_STRING_LENGTH];
  char              arg[MAX_INPUT_LENGTH];
  ROOM_INDEX_DATA * location;
  OBJ_DATA        * obj;
  CHAR_DATA       * rch;
  CHAR_DATA       * owner;
  SHIP_DATA       * pShip;
  MINE_DATA       * pMine;
  ENQUIRE_DATA    * pEnquire;
  int               door;
  int               count;
  bool              found;

  PUSH_FUNCTION( "do_rstat" );

  one_argument( argument, arg );

  if ( !( location = arg[0] ? find_location( ch, arg ) : ch->in_room ) )
  {
    send_to_char( "�S���o�Ӧa��C\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->in_room != location && room_is_private( location ) )
  {
    send_to_char( "�o���ж��O�p�H�ж��C\n\r", ch );
    RETURN_NULL();
  }

  owner  = owner_name( location );

  clear_buffer();
  send_to_buffer(
    "�Ҧb�W�١R\e[1;32m[\e[0m%s\e[1;32m]\e[0m\n\r"
    "�Ҧb�ϰ�R\e[1;32m[\e[0m%s\e[1;32m]\e[0m �ϰ��ݩʡR%s\n\r"
    "�ɮצW�١R\e[1;32m[\e[0m%s\e[1;32m]\e[0m\n\r"
    "�s���R(\e[1;36m%5d\e[0m)  "
    "�ϰ��ݩʡR%d ���|�R%s �����R%s\n\r"
    "�a�κX�СR\e[1;34m%s\e[0m. "
    "�ө��P�_�R\e[1;35m%s\e[0m (%d)\n\r"
    "�Ǫ����͡R%s%d\e[0m\n\r"
    "�ж��X�m�R%s\n\r"
    "�ж��H�ơR%d �H �ж����~�ơR%d ��\n\r"
    "�ж��D�H�R%d-%d �W�١R%s\n\r"
    "�O�_�i�g�^�R%s �g�a���ȡR%d �g�a�֭p���B�R%d\n\r"
    "���a������y�z�R\n\r%s\e[0m\n\r"
    , location->name
    , location->area->name
    , area_bit_name( location->area )
    , location->filename
    , location->vnum
    , location->area->attribute
    , location->club ? location->club->cname : "�S��"
    , location->board ? location->board->name : "(�S��)"
    , location->sector->cname
    , YESNO( !room_is_dark( location ) )
    , location->light
    , location->mob_reborn > 0 ? "\e[1;32m" : "\e[0m"
    , location->mob_reborn
    , room_bit_name( location )
    , location->nplayer
    , location->nobject
    , location->owner.high
    , location->owner.low
    , location->owner.high > 0 && location->owner.low > 0
      ? fullname( &( location->owner ) ) : "�S���D�H"
    , YESNO( location->writeable )
    , location->cost
    , location->gold
    , location->description );

  if ( location->job )
    send_to_buffer( "�o�өж����S���禡�C\n\r" );

  if ( location->extra_descr )
  {
    EXTRA_DESCR_DATA * ed;

    send_to_buffer( "���[�y�z����r�R'" );
    for ( ed = location->extra_descr; ed; ed = ed->next )
    {
      send_to_buffer( "%s", ed->keyword );
      if ( ed->next ) send_to_buffer( " " );
    }

    send_to_buffer( "'.\n\r" );
  }

  send_to_buffer( "���a���H���R" );
  for ( found = FALSE, rch = location->people; rch; rch = rch->next_in_room )
  {
    found = TRUE;
    send_to_buffer( " " );
    one_argument( rch->name, buf );
    send_to_buffer( "%s", buf );
    if ( buffer_full() ) break;
  }

  if ( !found ) send_to_buffer( "(�L)" );
  send_to_buffer( "�C\n\r���a�����~�R" );
  found = FALSE;

  for ( obj = location->contents; obj; obj = obj->next_content )
  {
    found = TRUE;
    send_to_buffer( " " );
    one_argument( obj->name, buf );
    send_to_buffer( "%s", buf );
    if ( buffer_full() ) break;
  }

  if ( !found ) send_to_buffer( "(�L)" );
  send_to_buffer( "�C\n\r���a���q���R" );
  found = FALSE;

  for ( pMine = location->mine; pMine; pMine = pMine->next )
  {
    if ( pMine->mineral && pMine->mineral->mineral )
    {
      send_to_buffer( " " );
      one_argument( pMine->mineral->mineral->name, buf );
      send_to_buffer( "%s", buf );
      found = TRUE;

      if ( buffer_full() ) break;
    }
  }

  if ( !found ) send_to_buffer( "(�L)" );
  send_to_buffer( "�C\n\r" );

  for ( door = 0; door < DIR_MAX; door++ )
  {
    EXIT_DATA * pexit;

    if ( ( pexit = location->exit[door] ) )
    {
      send_to_buffer(
        "��%s��ж� %d  "
        "�_�͡R%d.  "
        "�X�f���A�R%s\n\r"
        "����r�R%s  "
        "�y�z�R%s\n\r",
        direction_name( door ),
        pexit->to_room ? pexit->to_room->vnum : 0,
        pexit->key,
        exit_status( pexit ),
        ( pexit->keyword && pexit->keyword[0] ) ? pexit->keyword : "(�S��)",
        ( pexit->description && pexit->description[0] )
         ? pexit->description : "(�S��)." );
    }
  }

  if ( location->enquire )
  {
    send_to_buffer( "�ݸ�����r�R" );

    for ( pEnquire = location->enquire; pEnquire; pEnquire = pEnquire->next )
      send_to_buffer( " %s", pEnquire->keyword );

    send_to_buffer( "\n\r" );
  }

  send_to_buffer( "����ơR" );
  count = 1;
  for ( found = FALSE, pShip = ship_list; pShip; pShip = pShip->next, count++ )
  {
    if ( pShip->starting == location )
    {
      found = TRUE;
      send_to_buffer( " �X�o�I(%d)", count );
    }

    if ( pShip->destination == location )
    {
      found = TRUE;
      send_to_buffer( " �ت��a(%d)", count );
    }

    if ( pShip->cabin == location )
    {
      found = TRUE;
      send_to_buffer( " �(%d)"  , count );
    }
  }

  if ( !found ) send_to_buffer( "(�L)" );
  send_to_buffer( "\n\r" );
  print_buffer( ch );
  RETURN_NULL();
}

FUNCTION( do_olist )
{
  char             arg[ MAX_INPUT_LENGTH ];
  OBJ_INDEX_DATA * pObjIndex;
  AFFECT_DATA    * paf;
  RESTRICT_DATA  * pRest;
  MESSAGE_DATA   * pMessage;

  PUSH_FUNCTION( "do_olist" );

  one_argument( argument , arg );

  if ( ( pObjIndex = get_obj_protype( arg ) ) )
  {
    clear_buffer();
    send_to_buffer( "�W�r�R%s  "
                    "�s���R%d ���R%s �����R%s\n\r"
                    "�u�y�z�R%s\n\r"
                    "���y�z�R%s\n\r"
                    "�����R%s ( %d )\n\r"
                    "�B�~�X�СR%s\n\r"
                    "�����X�СR%s ( %d ) �i���R%s\n\r"
                    "�ƶq�R%d  "
                    "���q�R%d  "
                    "���šR%d  "
                    "���@�O�R%d  "
                    "���ȡR%d "
                    "�g��ȡR�U���� %d\n\r",
                    pObjIndex->name,
                    pObjIndex->vnum,
                    pObjIndex->unit ? pObjIndex->unit : "(�S��)",
                    YESNO( pObjIndex->disappear ),
                    pObjIndex->short_descr,
                    pObjIndex->description,
                    item_type_name( pObjIndex ),
                    pObjIndex->item_type,
                    extra_bit_name_2( pObjIndex ) ,
                    wear_bit_name( pObjIndex->wear_flags ),
                    pObjIndex->wear_flags,
                    YESNO( pObjIndex->Takeable ),
                    pObjIndex->count,
                    pObjIndex->weight,
                    pObjIndex->level,
                    pObjIndex->armor,
                    pObjIndex->cost,
                    pObjIndex->exp );

    send_to_buffer( "%s", get_obj_value_usage( pObjIndex->item_type ,
      pObjIndex->value ) );

    for ( pRest = pObjIndex->restrict; pRest; pRest = pRest->next )
      send_to_buffer( "����A�R%s\n\r", restrict_value( pRest, ch ) );

    for ( pMessage = pObjIndex->message; pMessage; pMessage = pMessage->next )
      send_to_buffer(
        "\n\r�ԭz�ɾ�: %s\n\r��X���ۤv�r��:%s\n\r��X���O�H�r��:%s\n\r"
        , message_type( pMessage->type ), pMessage->self, pMessage->others );

    for ( paf = pObjIndex->affected; paf; paf = paf->next )
    {
      send_to_buffer( "��%s�y�� %d ���v�T�C\n\r"
                      , affect_loc_name( paf->location )
                      , paf->modifier );
    }

    for ( paf = pObjIndex->impact; paf; paf = paf->next )
    {
      send_to_buffer( "[�]��] ��%s�y�� %d ���v�T�C\n\r"
                      , affect_loc_name( paf->location )
                      , paf->modifier );
    }

    print_buffer( ch );
  }

  else
  {
    send_to_char( "�S�����A�n�䪺���~�쫬�C\n\r" , ch );
  }

  RETURN_NULL();
}

FUNCTION( do_ostat )
{
  char             arg[MAX_INPUT_LENGTH];
  AFFECT_DATA    * paf;
  OBJ_DATA       * obj;
  RESTRICT_DATA  * pRest;
  MESSAGE_DATA   * pMess;
  CACHET_DATA    * pCachet;
  OBJ_INDEX_DATA * pIndex;

  PUSH_FUNCTION( "do_ostat" );

  one_argument( argument, arg );

  if ( !( obj = get_obj_world( ch, arg ) ) )
  {
    send_to_char( "�@�ɤW�S���A�n���F��C\n\r", ch );
    RETURN_NULL();
  }

  clear_buffer();
  send_to_buffer( "�W�١R%s\n\r"
                  "���X:%5d  "
                  "�����R%s ���R%s%s ����: %s �r�ġR%d�p�� \n\r"
                  "�ɮצW�١R%s\n\r"
                  "²�u���y�z�R%s\n\r"
                  "�ԲӪ��y�z�R%s\n\r"
                  "�����X�СR%s(%d) �i���R%s\n\r"
                  "���[�X�СR%s\n\r"
                  "�`��ơR%d  "
                  "���q�R%d/%d "
                  "�g��ȡR�U���� %d\n\r"
                  "���ȡR%d  "
                  "�p�ɾ��R%d  "
                  "���šR%d\n\r"
                  "�y���Ǹ��R%d-%d �֦��̧Ǹ��R%d-%d\n\r"
                  "���T�Ҧb�a�R%s\n\r"
                  "�˳Ʀ�m�R%s ( %d )\n\r",
                  obj->name ,
                  obj->pIndexData->vnum ,
                  item_type_name( obj->pIndexData ) ,
                  obj->unit,
                  obj->pIndexData->unit ? "" : "<\e[1;32m�w�]��\e[0m>",
                  YESNO( obj->disappear ),
                  obj->venom,
                  obj->pIndexData->filename ,
                  obj->cname,
                  obj->description ,
                  wear_bit_name( obj->wear_flags ),
                  obj->wear_flags,
                  YESNO( obj->Takeable ),
                  extra_bit_name( obj ) ,
                  get_obj_number( obj ),
                  obj->weight,
                  get_obj_weight( obj ),
                  obj->pIndexData ? obj->pIndexData->exp : -1,
                  obj->cost,
                  obj->timer,
                  obj->level ,
                  obj->serial.high,
                  obj->serial.low,
                  obj->owner.high,
                  obj->owner.low,
                  object_locate( obj , ch ),
                  wear_location_string( obj->wear_loc ),
                  obj->wear_loc );

  /* �����������@�O */
  if ( is_armor( obj ) && obj->max_armor > 0 )
  {
    send_to_buffer( "�̤j���@�O�R%d �ثe���@�O�R%d�C (%d%%)\n\r"
      , obj->max_armor, obj->armor
      , obj->armor * 100 / obj->max_armor );
  }

  for ( pRest = obj->pIndexData->restrict; pRest; pRest = pRest->next )
    send_to_buffer( "����A�R%s\n\r", restrict_value( pRest, ch ) );

  for ( pMess = obj->pIndexData->message; pMess; pMess = pMess->next )
    send_to_buffer(
      "\n\r�ԭz�ɾ�: %s\n\r��X���ۤv�r��:%s\n\r��X���O�H�r��:%s\n\r"
        , message_type( pMess->type ), pMess->self, pMess->others );

  send_to_buffer( "%s", get_obj_value_usage( obj->item_type , obj->value ) );

  if ( obj->extra_descr || obj->pIndexData->extra_descr )
  {
    EXTRA_DESCR_DATA *ed;

    send_to_buffer( "�B�~�y�z����r�R'" );

    for ( ed = obj->extra_descr; ed; ed = ed->next )
    {
      send_to_buffer( "%s", ed->keyword );
      if ( ed->next ) send_to_buffer( " " );
    }

    for ( ed = obj->pIndexData->extra_descr; ed; ed = ed->next )
    {
      send_to_buffer( "%s", ed->keyword );
      if ( ed->next ) send_to_buffer( " " );
    }

    send_to_buffer( "'\n\r" );
  }

  for ( paf = obj->affected; paf; paf = paf->next )
  {
    send_to_buffer( "��%s�y�� %d ���v�T�C\n\r"
                    , affect_loc_name( paf->location )
                    , paf->modifier );
  }

  for ( paf = obj->pIndexData->affected; paf; paf = paf->next )
  {
    send_to_buffer( "��%s�y�� %d ���v�T�C\n\r"
      , affect_loc_name( paf->location )
      , paf->modifier );
  }

  for ( paf = obj->pIndexData->impact; paf; paf = paf->next )
  {
    send_to_buffer( "[�]��] ��%s�y�� %d ���v�T�C\n\r"
      , affect_loc_name( paf->location )
      , paf->modifier );
  }

  for ( pCachet = obj->cachet; pCachet; pCachet = pCachet->next )
  {
    if ( !( pIndex = get_obj_index( pCachet->vnum ) ) ) continue;

    for ( paf = pIndex->impact; paf; paf = paf->next )
    {
      send_to_buffer( "[�ʦL�]��] ��%s�y�� %d ���v�T�C\n\r"
        , affect_loc_name( paf->location )
      , paf->modifier );
    }
  }

  print_buffer( ch );
  RETURN_NULL();
}

FUNCTION( do_mlist )
{
  char             arg[ MAX_INPUT_LENGTH ];
  MOB_INDEX_DATA * pMobIndex;
  TEACH_DATA     * teach;
  SKILL_DATA     * pSkill;
  ENABLE_DATA    * pEnable;
  AFFECT_DATA    * pAffect;
  int              count;

  PUSH_FUNCTION( "do_mlist" );

  one_argument( argument , arg );

  if ( ( pMobIndex = get_char_protype( argument ) ) )
  {
    clear_buffer();
    send_to_buffer( "�W�r�R%s  "
                    "�s���R%d\n\r"
                    "�ɮצW�١R%s\n\r"
                    "�u�ԭz�R%s\n\r"
                    "���ԭz�R%s\n\r"
                    "����y�z�R%s\n\r"
                    "�ƶq�R%d  "
                    "�Q���F %d �� "
                    "�ʧO�R%s  "
                    "�����R%d  "
                    "���šR%d  "
                    "�}��R%d\n\r"
                    "�O�q�R%d  "
                    "���O�R%d  "
                    "���ѡR%d  "
                    "�ӱ��R%d  "
                    "���R%d\n\r"
                    "�X�СR%s ( %d )\n\r"
                    "�R���v�R%d "
                    "���@�O�R%d "
                    "�g��ȥ[���R%d "
                    "�E�����v�R %d%% "
                    "���ܡR %s\n\r"
                    "�����[���R�ʤ��� %d "
                    "�{���[���R�ʤ��� %d\n\r"
                    "Hitnodice�R%d  "
                    "HitSizeDice�R%d  "
                    "HitPlus�R%d\n\r"
                    "DamNoDice�R%d  "
                    "DamSizeDice�R%d  "
                    "DamPlus�R%d\n\r",
     pMobIndex->name,
     pMobIndex->vnum,
     pMobIndex->filename ,
     pMobIndex->short_descr,
     pMobIndex->long_descr,
     pMobIndex->description,
     pMobIndex->count,
     pMobIndex->killed,
     sex_name( pMobIndex->sex ),
     pMobIndex->gold,
     pMobIndex->level,
     pMobIndex->alignment,
     pMobIndex->now_str,
     pMobIndex->now_int,
     pMobIndex->now_wis,
     pMobIndex->now_dex,
     pMobIndex->now_con,
     act_bit_name( pMobIndex->act ),
     pMobIndex->act,
     pMobIndex->hitroll,
     pMobIndex->ac,
     pMobIndex->multipile,
     pMobIndex->migrate,
     YESNO( pMobIndex->speak ),
     pMobIndex->attack_ratio,
     pMobIndex->dodge_ratio,
     pMobIndex->hitnodice,
     pMobIndex->hitsizedice,
     pMobIndex->hitplus,
     pMobIndex->damnodice,
     pMobIndex->damsizedice,
     pMobIndex->damplus );

    send_to_buffer( "�ͩR�O�R%d  �k�O�R%d  ��O�R%d\n\r¾�~�R%s\n\r"
      , pMobIndex->hit, pMobIndex->mana, pMobIndex->move
      , class_name( pMobIndex->class, TRUE ) );

    send_to_buffer( "�{�����A�R" );
    if ( pMobIndex->progtypes == ERROR_PROG )
    {
      send_to_buffer( "���~�����A. (%d) \n\r" , pMobIndex->progtypes );
    }
    else
    {
       send_to_buffer( "%s (%d)\n\r" ,
                        progtypes_bit_name( pMobIndex->progtypes ),
                        pMobIndex->progtypes );
    }

    send_to_buffer( "�оɪ��ޯ�R\n\r" );
    for ( teach = pMobIndex->teach, count = 0; teach ; teach = teach->next )
    {
      if ( ( pSkill = get_skill( teach->slot ) ) )
      {
        send_to_buffer( "( %2d ) �s���R%d �k�N�W�١R%s "
                        "���m�סR%d �бª��R%d ���áR%d%%\n\r"
          , ++count
          , pSkill->slot
          , pSkill->name
          , teach->adept
          , teach->cost
          , teach->inventory );
      }
    }

    if ( count == 0 ) send_to_buffer( "�S�����󪺧ޯ�оɡC\n\r" );
    send_to_buffer( "�P�઺�ޯ�R\n\r" );

    for ( count = 0, pEnable = pMobIndex->enable; pEnable; pEnable = pEnable->next )
    {
      if ( ( pSkill = pEnable->skill ) )
      {
        send_to_buffer( "( %2d ) �s���R%3d �k�N�W�١R%-30s ���m�סR%2d%%\n\r"
          , ++count, pSkill->slot, pSkill->name
          , pMobIndex->skill[pSkill->slot] );
      }
    }

    if ( count == 0 ) send_to_buffer( "�S�����󪺧ޯ�P��C\n\r" );

    for ( pAffect = pMobIndex->affected; pAffect; pAffect = pAffect->next )
    {
      if ( !( pSkill = get_skill( pAffect->type ) ) )
      {
        mudlog( LOG_DEBUG, "do_mstat: ���~�����A %d.", pAffect->type );
        continue;
      }

      send_to_buffer( "�k�N�R%s��%s�y�� %d �v�T�٦� %d �p��\n\r",
        pSkill->cname,
        affect_loc_name( pAffect->location ),
        pAffect->modifier,
        pAffect->duration );
    }

    print_buffer( ch );
  }

  else
  {
    send_to_char( "�S�����A�n�䪺�Ǫ��쫬�C\n\r" , ch );
  }

  RETURN_NULL();
}

FUNCTION( do_mstat )
{
  char              arg[MAX_INPUT_LENGTH];
  AFFECT_DATA     * paf;
  CHAR_DATA       * victim;
  SKILL_DATA      * pSkill;
  ENROLL_DATA     * pEnroll;
  DESCRIPTOR_DATA * pDesc;
  QUEST_DATA      * pQuest;
  int               loop;

  PUSH_FUNCTION( "do_mstat" );

  one_argument( argument, arg );

  if ( !( victim = get_char_world( ch, arg ) ) )
  {
    send_to_char( "�A���ؼФ��b�o�̡C\n\r", ch );
    RETURN_NULL();
  }

  clear_buffer();
  send_to_buffer( "�W�r�R%s  "
                  "�Ǹ��R%d-%d "
                  "����W�r�R%s "
                  "���� %s\n\r"
                  "���X�R%d  "
                  "�ʧO�R%s  "
                  "�Ҧb�ж��R%d "
                  "���ݰϰ�R%s\n\r"
                  "�D�H�Ǹ��R%d-%d\n\r"
                  "�O�q�R%d "
                  "���z�R%d "
                  "���ѡR%d "
                  "�t�סR%d "
                  "���R%d\n\r"
                  "�����ޡR%d �K�����P�R%d "
                  "�ʺ��ѼơR%d ��L�R%d �����r�R%d ��W�R%d\n\r"
                  "�ͩR�R%d/%d "
                  "�k�O�R%d/%d "
                  "��O�R%d/%d "
                  "�ǲ��I�ơR%d\n\r"
                  "���šR%d "
                  "¾�~�R%s "
                  "�}��R%d "
                  "���@�O�R%d "
                  "�ޯ�[���R %d\n\r"
                  "�����R%d "
                  "�����R%d "
                  "�g��ȡR%d \n\r"
                  "�R���v�R%d "
                  "�ˮ`�O�R%d "
                  "�k�]�I�ơR%d "
                  "���Ѯɶ��R%d ",
      victim->name , victim->serial.high, victim->serial.low,
      IS_NPC( victim ) ? "(�q��)" : victim->cname ,
      club_name( victim ),
      IS_NPC( victim ) ? victim->pIndexData->vnum : 0 ,
      sex_name( victim->sex ),
      !victim->in_room ? -1 : victim->in_room->vnum ,
      ( victim->in_room ) ? victim->in_room->area->name : "" ,
      victim->jade.high, victim->jade.low,
      get_curr_str(victim),
      get_curr_int(victim),
      get_curr_wis(victim),
      get_curr_dex(victim),
      get_curr_con(victim),
      victim->limit, victim->firman, victim->jail, victim->failed,
      victim->pcdata ? victim->pcdata->xname : 0,
      victim->pcdata ? victim->pcdata->rechristen : 0,
      victim->hit,         get_curr_hit( victim ),
      victim->mana,        get_curr_mana( victim ),
      victim->move,        get_curr_move( victim ),
      victim->practice,
      victim->level,
      class_name( victim->class, TRUE ),
      victim->alignment,
      get_ac(victim),
      ch->nskill,
      victim->gold,
      victim->bank,
      victim->exp,
      get_hitroll( victim ),
      get_damroll( victim ),
      victim->wimpy,
      victim->pcdata ? victim->pcdata->steal_panity : 0 );

  send_to_buffer( "���A�R%s\n\r", get_position( victim->position ) );
  send_to_buffer( "����X�СR%s�C\n\r", YESNO( !( verify_char( ch ) ) ) );

  if ( victim->spirit && !victim->boss )
  {
    CHAR_DATA * spirit;

    send_to_buffer( "����p���R" );

    for ( spirit = victim->spirit; spirit; spirit = spirit->spirit )
      send_to_buffer( " %s", mob_name( NULL, spirit ) );

    send_to_buffer( "\n\r" );
  }

  if ( victim->boss )
    send_to_buffer( "�p���֦��̡R%s\n\r", victim->boss->name );

  if ( !IS_NPC( victim ) && victim->pcdata )
  {
    send_to_buffer( "�D�B�̡R%s �t���R%s �O�_�b�u�W�R%s\n\r"
      , victim->pcdata->wooer
        ? victim->pcdata->wooer->name : "(�q��)"
      , victim->pcdata->mater && *victim->pcdata->mater
        ? victim->pcdata->mater : "(�q��)"
      , YESNO( victim->pcdata->mate ) );

    send_to_buffer( "PKĹ�R%d PK��R%d ���Ĺ: %d ��Կ�: %d"
                    "�����Ǫ� %d ���M�@ %d �� ���� %d ��\n\r"
      , victim->pcdata->pkwin
      , victim->pcdata->pklost
      , victim->pcdata->fightwin
      , victim->pcdata->fightlost
      , victim->pcdata->kills
      , victim->pcdata->kill_level
      , ( victim->pcdata->kills == 0 ) ? 0 :
          victim->pcdata->kill_level / victim->pcdata->kills );

    send_to_buffer( "������ơR%d. "
                    "���骬�A�R%s. "
                    , victim->pcdata->pagelen
                    , victim->pcdata->corpse ? "��" : "�S��" );

    if ( victim->pcdata->corpse )
      send_to_buffer( "(����Ҧb�a�R%d ) " ,
        victim->pcdata->corpse->in_room ?
        victim->pcdata->corpse->in_room->vnum : -1 );

    send_to_buffer(
      "�����`�ӼơR%d �w�]�ޯ��`�ơR%d\n\r"
      "�q�l�l��H�c�R%s\n\r"
      , get_alias_count( victim )
      , enable_count( victim )
      , victim->email && *victim->email ? victim->email : "(�q��)" );

    if ( ( pDesc = victim->desc ) )
    {
      check_multi_update();

      send_to_buffer( "�p�ƪ��A�R%s �p�ƾ��ɯߡR%d �p�ƾ��R�O�R%s\n\r"
        , pDesc->tick_count >= 0 ? "���}" : "����"
        , pDesc->tick
        , pDesc->tick == -1 ? "�q��" : pDesc->tick_command );

      send_to_buffer( "�s����R%5d �h���s�u�R%d ���a�Ǹ� %d "
                      "�s�u��}�R%s\n\r"
        , pDesc->port
        , pDesc->multi
        , pDesc->id_number
        , host_name( pDesc ) );
    }

    else
    {
      send_to_buffer( "�_�u���M�S���y�z��C\n\r" );
    }
  }

  send_to_buffer( "���b�若����H�R%s\n\r"
    , victim->fighting ? victim->fighting->name : "(�S��)" );

  if ( !IS_NPC(victim) )
  {
    send_to_buffer( "�f���{�סR%d%% "
                    "�{�l�j�{�סR%d%% "
                    "�s�K�{�סR%d%% "
                    "Saving throw�R%d "
                    "�Q�����ơR%d\n\r",
        victim->pcdata->condition[COND_THIRST],
        victim->pcdata->condition[COND_FULL],
        victim->pcdata->condition[COND_DRUNK],
        victim->saving_throw,
        victim->pcdata->imprison );
  }

  send_to_buffer( "���b��a���~���`��ơR%d�C "
                  "���b��a���~���`���R%d�C\n\r"
                  "�~�֡R%d  "
                  "�W�u�ɼơR%d  "
                  "�p�ɾ��R%d\n\r"
                  "���ڥN�z�v�R%s ( %d ) �g�@�ɶ��R%d\n\r"
                  "�X�СR%s ( %s )\n\r"
                  "�P��R%s ( %d )\n\r"
                  "�D�H�R%s. "
                  "��ɪ̡R%s�C\n\r"
                  "�R�F��R%s�C "
                  "��F��R%s�C\n\r" ,

      victim->carry_number,
      get_carry_weight( victim ),
      get_age( victim ),
      (int) victim->played,
      victim->timer ,
      IS_NPC( victim ) ? "�D���a" : wiz_bit_name( victim->wizflags ),
      victim->wizflags,
      victim->tribunal_timer,
      act_bit_name( victim->act ),
      binary_bit( victim->act ),
      IS_NPC( victim) ? "�q��" : turn_bit_name( victim->turn ) ,
      IS_NPC( victim) ? -1     : victim->turn,
      victim->master      ? victim->master->name   : "(�S��)",
      victim->leader      ? victim->leader->name   : "(�S��)",
      YESNO( auction_info->buyer == victim ),
      YESNO( auction_info->seller == victim ) );

  send_to_buffer( "²�u���y�z�R%s�C\n\r"
                  "�ԲӪ��y�z�R%s",
      victim->byname,
      victim->long_descr[0] != '\x0' ? victim->long_descr : "(�S��)�C\n\r" );

  if ( IS_NPC( victim ) && victim->spec_fun )
    send_to_buffer( "�o���Ǫ����S����ơC\n\r" );

  if ( IS_NPC( victim ) && victim->pIndexData )
    send_to_buffer(
      "�Ǫ��ƥ� %d �g��ȥ[�� %d%% �E�����v %d%% ���� %s\n\r"
      "�����[���R�ʤ��� %d �{���[���R�ʤ��� %d\n\r"
      , get_mob_count( victim->pIndexData->vnum )
      , victim->pIndexData->multipile
      , victim->pIndexData->migrate
      , YESNO( victim->pIndexData->speak )
      , victim->pIndexData->attack_ratio
      , victim->pIndexData->dodge_ratio );

  /* ��X�g�P�a�� */
  if ( victim->pcdata )
  {
    send_to_buffer( "�g�P�a��R" );

    for ( loop = 0; loop < MAX_RECALL; loop++ )
      if ( victim->pcdata->recall[loop] > 0 )
        send_to_buffer( " %d", victim->pcdata->recall[loop] );
  }

  send_to_buffer( "\n\r" );

  /* ��X�Ѱg������ */
  if ( !IS_NPC( victim ) )
  {
    send_to_buffer( "�Ѱg�R" );
    for ( pQuest = victim->quest; pQuest; pQuest = pQuest->next )
    {
      if ( !pQuest->link ) continue;
      send_to_buffer( " %s", pQuest->link->mark );
    }

    send_to_buffer( "\n\r" );
  }

  if ( victim->enroll )
  {
    send_to_buffer( "�O����H�R" );

    for ( pEnroll = victim->enroll; pEnroll; pEnroll = pEnroll->next )
      send_to_buffer( " %s", pEnroll->name );

    send_to_buffer( "\n\r" );
  }

  if ( IS_NPC( victim ) && victim->pIndexData )
  {
    /* ��X�w�İ� */
    if ( victim->talk_buf )
    {
      send_to_buffer( "����X�w�İ� �Ҧ��R" );

      switch( victim->talk_mode )
      {
      default:
      case MODE_ABSENT: send_to_buffer( "�Ҧ�����" ); break;
      case MODE_SAY:    send_to_buffer( "�P�满��" ); break;
      case MODE_CHAT:   send_to_buffer( "����Ҧ�" ); break;
      }

      send_to_buffer( "\n\r" );
    }

    if ( victim->spiritjade > 0 )
      send_to_buffer( "���ɸ��X: %d\n\r", victim->spiritjade );

    /* �о� */
    if ( victim->pIndexData->teach )
    {
      TEACH_DATA * teach;
      SKILL_DATA * pSkill;
      int          count = 0;

      send_to_buffer( "�оɪ��ޯ�R\n\r" );
      for ( teach = victim->pIndexData->teach; teach ; teach = teach->next )
      {
        if ( ( pSkill = get_skill( teach->slot ) ) )
        {
          send_to_buffer( "( %2d ) �s���R%d �k�N�W�١R%s "
                          "���m�סR%d �бª��R%d ���áR%d%%\n\r"
            , ++count
            , pSkill->slot
            , pSkill->name
            , teach->adept
            , teach->cost
            , teach->inventory );
        }
      }
    }

    /* ���� */
    if ( victim->pIndexData->reborn_vnum > 0 )
    {
      MOB_INDEX_DATA * pMobIndex;

      if ( ( pMobIndex = get_mob_index( victim->pIndexData->reborn_vnum ) ) )
      {
        send_to_buffer( "���ͩǪ����X�R%d �W�� %s\n\r"
          , pMobIndex->vnum
          , mob_index_name( NULL, pMobIndex ) );
      }
    }

    /* �y�M */
    if ( victim->tractable > 0 )
      send_to_buffer( "���Ƶ{�סR%d%%\n\r", victim->tractable );
  }

  send_to_buffer( "�y�M�R%s �Q�M�R%s\n\r"
    , victim->mount    ? mob_name( NULL, victim->mount    ) : "�L"
    , victim->mount_by ? mob_name( NULL, victim->mount_by ) : "�L" );

  for ( paf = victim->affected; paf; paf = paf->next )
  {
    if ( !( pSkill = get_skill( paf->type ) ) )
    {
      mudlog( LOG_DEBUG, "do_mstat: ���~�����A %d.", paf->type );
      continue;
    }

    send_to_buffer( "�k�N�R%s��%s�y�� %d �v�T�٦� %d �p��\n\r",
      pSkill->cname,
      affect_loc_name( paf->location ),
      paf->modifier,
      paf->duration );
  }

  if ( victim->pIndexData )
  {
    for ( paf = victim->pIndexData->affected; paf; paf = paf->next )
    {
      if ( !( pSkill = get_skill( paf->type ) ) )
      {
        mudlog( LOG_DEBUG, "do_mstat: ���~�����A %d.", paf->type );
        continue;
      }

      send_to_buffer( "�k�N�R%s��%s�y�� %d �v�T�٦� %d �p��\n\r",
        pSkill->cname,
        affect_loc_name( paf->location ),
        paf->modifier,
        paf->duration );
    }
  }

  print_buffer( ch );
  RETURN_NULL();
}

FUNCTION( do_mfind )
{
  extern int       top_mob_index;
  char             arg1[MAX_INPUT_LENGTH];
  char             arg2[MAX_INPUT_LENGTH];
  MOB_INDEX_DATA * pMobIndex;
  int              vnum;
  int              nMatch;
  bool             fAll;
  bool             found;
  int              up;
  int              down;
  int              number;

  PUSH_FUNCTION( "do_mfind" );

  argument = one_argument( argument, arg1 );
  one_argument( argument , arg2 );

  clear_buffer();

  fAll   = !str_cmp( arg1, "all" );
  found  = FALSE;
  nMatch = 0;

  if ( is_number( arg1 ) && is_number( arg2 ) )
  {
    up   = UMAX( atoi( arg1 ) , atoi( arg2 ) );
    down = UMIN( atoi( arg1 ) , atoi( arg2 ) );

    for ( vnum = 0; nMatch < top_mob_index; vnum++ )
    {
      if ( ( pMobIndex = get_mob_index( vnum ) ) )
      {
        nMatch++;
        if ( pMobIndex->vnum >= down && pMobIndex->vnum <= up )
        {
          found = TRUE;
          send_to_buffer( "\e[1;32m���X\e[0m%6d "
                          "\e[1;33m�y����\e[0m%5d "
                          "\e[1;35m�y�z\e[0m %s\n\r",
            pMobIndex->vnum,
            nMatch,
            mob_index_name( NULL, pMobIndex ) );

          if ( buffer_full() ) break;
        }
      }
    }
  }

  else
  {
    number = atoi( arg1 );
    for ( vnum = 0; nMatch < top_mob_index; vnum++ )
    {
      if ( ( pMobIndex = get_mob_index( vnum ) ) )
      {
        nMatch++;
        if ( fAll || is_name( arg1, pMobIndex->name )
          || number == pMobIndex->vnum )
        {
          found = TRUE;
          send_to_buffer( "\e[1;32m���X\e[0m%6d "
                          "\e[1;33m�y����\e[0m%5d "
                          "\e[1;35m�y�z\e[0m %s\n\r",
                          pMobIndex->vnum,
                          nMatch,
                          mob_index_name( NULL, pMobIndex ) );

          if ( buffer_full() ) break;
        }
      }
    }
  }

  if ( !found )
  {
    send_to_char( "�䤣��A���w����H�C\n\r", ch );
    RETURN_NULL();
  }

  print_buffer( ch );
  RETURN_NULL();
}

FUNCTION( do_ofind )
{
  extern int       top_obj_index;
  char             arg1[MAX_INPUT_LENGTH];
  char             arg2[MAX_INPUT_LENGTH];
  OBJ_INDEX_DATA * pObjIndex;
  int              vnum;
  int              nMatch;
  int              number;
  bool             fAll;
  bool             found;
  int              up;
  int              down;

  PUSH_FUNCTION( "do_ofind" );

  argument = one_argument( argument, arg1 );
  one_argument( argument , arg2 );

  fAll   = !str_cmp( arg1, "all" );
  found  = FALSE;
  nMatch = 0;

  clear_buffer();

  if ( is_number( arg1 ) && is_number( arg2 ) )
  {
    up   = UMAX( atoi( arg1 ) , atoi( arg2 ) );
    down = UMIN( atoi( arg1 ) , atoi( arg2 ) );
    for ( vnum = 0; nMatch < top_obj_index; vnum++ )
    {
      if ( ( pObjIndex = get_obj_index( vnum ) ) )
      {
        nMatch++;
        if ( pObjIndex->vnum >= down && pObjIndex->vnum <= up )
        {
          found = TRUE;
          send_to_buffer( "\e[1;32m���X\e[0m%6d "
                          "\e[1;33m�y����\e[0m%5d "
                          "\e[1;36m����\e[0m%4d "
                          "\e[1;35m�y�z\e[0m %s(%s)\n\r",
                          pObjIndex->vnum,
                          nMatch,
                          pObjIndex->level,
                          pObjIndex->short_descr,
                          pObjIndex->name );

          if ( buffer_full() ) break;
        }
      }
    }
  }
  else
  {
    number = atoi( arg1 );
    for ( vnum = 0; nMatch < top_obj_index; vnum++ )
    {
      if ( ( pObjIndex = get_obj_index( vnum ) ) )
      {
        nMatch++;
        if ( fAll || is_name( arg1, pObjIndex->name )
          || number == pObjIndex->vnum )
        {
          found = TRUE;
          send_to_buffer( "\e[1;32m���X\e[0m%6d "
                          "\e[1;33m�y����\e[0m%5d "
                          "\e[1;36m����\e[0m%4d "
                          "\e[1;34m�ƶq\e[0m%3d "
                          "\e[1;35m�y�z\e[0m %s(%s)\n\r",
                          pObjIndex->vnum,
                          nMatch,
                          pObjIndex->level,
                          pObjIndex->count,
                          pObjIndex->short_descr,
                          pObjIndex->name );

          if ( buffer_full() ) break;
        }
      }
    }
  }

  if ( !found ) send_to_buffer( "�䤣��A�ҫ��w���F��C\n\r" );
  print_buffer( ch );

  RETURN_NULL();
}

FUNCTION( do_mwhere )
{
  char        arg[MAX_INPUT_LENGTH];
  CHAR_DATA * victim;
  int         counter;
  bool        fAll = FALSE;

  PUSH_FUNCTION( "do_mwhere" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  one_argument( argument, arg );

  if ( !str_cmp( arg , "all" ) ) fAll = TRUE;

  clear_buffer();
  counter = 1;

  for ( victim = char_list; victim; victim = victim->next )
  {
    if ( !verify_char( victim ) ) continue;

    if ( ( is_name( arg , victim->name ) || fAll )
       &&  IS_NPC( victim ) && victim->in_room )
    {
      send_to_buffer( "[ %3d ] %s [ %d ] "
                      "�b %s [ %d ]�C\n\r"
                      , counter++
                      , mob_name( ch, victim )
                      , victim->pIndexData->vnum
                      , victim->in_room->name
                      , victim->in_room->vnum );

      if ( buffer_full() ) break;
    }
  }

  if ( counter == 1 ) send_to_buffer( "�èS�����A�n�䪺�Ǫ��C\n\r" );
  print_buffer( ch );
  RETURN_NULL();
}

/* ���s�}�� */
FUNCTION( do_reboot )
{
  char        arg[ MAX_INPUT_LENGTH];
  char        buf[ MAX_STRING_LENGTH];
  int         minutes;
  time_t      future;
  extern bool merc_down;

  PUSH_FUNCTION( "do_reboot" );

  argument = one_argument( argument, arg );

  if ( !arg[0] || !str_cmp( arg, "now" ) )
  {
    if ( ch && !IS_NPC( ch ) && !IS_SET( ch->act, PLR_WIZINVIS ) )
    {
      sprintf( buf, "��%s��\e[1;32m%s\e[0m"
        "���s�}���M�ЦA���ݴX�����a�T", mob_name( NULL, ch ), mud_name );
    }

    else
    {
     str_cpy( buf,  "�t�έ��s�ҰʡM�еy��A���{�T" );
    }

    talk_channel_2( buf, CHANNEL_NOTICE, "" );
    mudlog( LOG_INFO , " " );
    close_all_socket();
    mudlog( LOG_INFO , " " );
    merc_down = TRUE;
  }

  else if ( !str_cmp( arg, "cancel" ) )
  {
    if ( reboot_time > 0 )
      talk_channel_2( "�����t�έ��m�ʧ@�C" , CHANNEL_NOTICE, "" );

    else if ( shutdown_time > 0 )
      talk_channel_2( "�����t�������ʧ@�C" , CHANNEL_NOTICE, "" );

    else if ( ch )
    {
      send_to_char( "�t�ΨS�����󭫸m�������ʧ@�M���O�L�ġC\n\r", ch );
      RETURN_NULL();
    }

    str_cpy( buf,
       "\e[1m�ѭ����L�M�N�Ҧ������Ǯ����M�j�a�S��_�M���C\n\r\e[0m" );

    send_to_all_char( buf );
    first_warn  = second_warn = third_warn = TRUE;
    reboot_time = shutdown_time = ( time_t ) 0;
    wizlock     = FALSE;
  }

  else if ( !str_cmp( arg, "at" ) )
  {
    one_argument( argument, arg );

    if ( ( future = time_at( arg ) ) <= current_time )
    {
      if ( ch ) send_to_char( "�ɶ��]�w���~�M�Ьd�ߨϥΤ�k�C\n\r", ch );
      RETURN_NULL();
    }

    minutes       = ( ( reboot_time = future ) - current_time ) / 60;
    shutdown_time = ( time_t ) 0;

    first_warn    = minutes >= FIRST_WARN  ? TRUE : FALSE;
    second_warn   = minutes >= SECOND_WARN ? TRUE : FALSE;
    third_warn    = minutes >= THIRD_WARN  ? TRUE : FALSE;

    if ( ch && !IS_NPC( ch ) && !IS_SET( ch->act, PLR_WIZINVIS ) )
    {
      sprintf( buf, "\e[1m����ǨӤF%s\e[1m���l���l���M�b�o���v�O�ʭ���"
        "�ɥN�M���s�}�l�O�̨γ~�|�C\n\r�ߦ��}�a�~���س]�M���ڭ�"
        "�ɱ����ɨ��o�ӯ}�a���Pı�a�T\n\r\n\r\e[0m"
        , mob_name( NULL, ch ) );
    }
    else
    {
      sprintf( buf, "\e[1m����ǨӤF�@�}���l���l���M�b�o���v�O�ʭ���"
        "�ɥN�M���s�}�l�O�̨γ~�|�C\n\r�ߦ��}�a�~���س]�M���ڭ�"
        "�ɱ����ɨ��o�ӯ}�a���Pı�a�T\n\r\n\r\e[0m" );
    }

    send_to_all_char( buf );
    sprintf( buf, "�t�αN�b %s ���m�C\n\r"
      , time_format( future, "%r, %a-%d-%b-%y" ) );

    send_to_all_char( buf );
  }

  else if ( is_number( arg ) )
  {
    if ( ( minutes = atoi( arg ) ) <= 0 || minutes > 60 )
    {
      if ( ch )
        send_to_char( "�t�έ��m�ɶ����X�k( 1 �� 60 ���� )�C\n\r", ch );

      RETURN_NULL();
    }

    if ( ch && !IS_NPC( ch ) && !IS_SET( ch->act, PLR_WIZINVIS ) )
    {
      sprintf( buf, "\e[1m����ǨӤF%s\e[1m���l���l���M�b�o���v�O�ʭ���"
        "�ɥN�M���s�}�l�O�̨γ~�|�C\n\r�ߦ��}�a�~���س]�M���ڭ�"
        "�ɱ����ɨ��o�ӯ}�a���Pı�a�T\n\r\n\r\e[0m"
        , mob_name( NULL, ch ) );
    }
    else
    {
      sprintf( buf, "\e[1m����ǨӤF�@�}���l���l���M�b�o���v�O�ʭ���"
        "�ɥN�M���s�}�l�O�̨γ~�|�C\n\r�ߦ��}�a�~���س]�M���ڭ�"
        "�ɱ����ɨ��o�ӯ}�a���Pı�a�T\n\r\n\r\e[0m" );
    }

    send_to_all_char( buf );
    sprintf( buf, "�t�αN�b %d�����᭫�m�T\n\r", minutes );
    send_to_all_char( buf );

    reboot_time   = current_time + minutes * 60;
    shutdown_time = ( time_t ) 0;
    wizlock       = TRUE;
    first_warn    = minutes > FIRST_WARN  ? TRUE : FALSE;
    second_warn   = minutes > SECOND_WARN ? TRUE : FALSE;
    third_warn    = minutes > THIRD_WARN  ? TRUE : FALSE;
  }

  else
  {
    if ( ch ) send_to_char( "�y�k���~�M�Ьd�ߨϥΤ�k�C\n\r", ch );
  }

  RETURN_NULL();
}

/* ���� MUD */
FUNCTION( do_shutdown )
{
  char        buf[MAX_STRING_LENGTH];
  char        arg[MAX_INPUT_LENGTH];
  time_t      future;
  int         minutes;
  extern bool merc_down;

  PUSH_FUNCTION( "do_shutdown" );

  argument = one_argument( argument, arg );

  if ( !arg[0] || !str_cmp( arg, "now" ) )
  {
    if ( ch )
      sprintf( buf, "��%s����\e[1;32m%s\e[0m�T", mob_name( NULL, ch ), mud_name );

    else
      str_cpy( buf, "�t�������M�ФU���A���{�M���¡C" );

    mudlog( LOG_SHUTDOWN , "%s", buf );
    talk_channel_2( buf, CHANNEL_NOTICE, "" );

    mudlog( LOG_INFO , " " );
    close_all_socket();
    mudlog( LOG_INFO , " " );

    merc_down = TRUE;
  }

  else if ( !str_cmp( arg, "cancel" ) )
  {
    if ( reboot_time   > 0 )
      talk_channel_2( "�����t�έ��m�ʧ@�C", CHANNEL_NOTICE, "" );

    else if ( shutdown_time > 0 )
      talk_channel_2( "�����t�������ʧ@�C", CHANNEL_NOTICE, "" );

    else if ( ch )
    {
      send_to_char( "�t�ΨS�����󭫸m�������ʧ@�M���O�L�ġC\n\r", ch );
      RETURN_NULL();
    }

    str_cpy( buf,
      "\e[1m�ѭ����L�M�N�Ҧ������Ǯ����M�j�a�S��_�M���T\n\r\e[0m" );

    send_to_all_char( buf );
    first_warn  = second_warn = third_warn = TRUE;
    reboot_time = shutdown_time = ( time_t ) 0;
    wizlock     = FALSE;
  }

  else if ( !str_cmp( arg, "at" ) )
  {
    one_argument( argument, arg );

    if ( ( future = time_at( arg ) ) <= current_time )
    {
      if ( ch ) send_to_char( "�ɶ��]�w���~�M�Ьd�ߨϥΤ�k�C\n\r", ch );
      RETURN_NULL();
    }

    minutes     = ( ( shutdown_time = future ) - current_time ) / 60;
    reboot_time = ( time_t ) 0;

    first_warn  = minutes >= FIRST_WARN  ? TRUE : FALSE;
    second_warn = minutes >= SECOND_WARN ? TRUE : FALSE;
    third_warn  = minutes >= THIRD_WARN  ? TRUE : FALSE;

    if ( ch && !IS_NPC( ch ) && !IS_SET( ch->act, PLR_WIZINVIS ) )
    {
      sprintf( buf, "\e[1m����ǨӤF%s\[1m���l���l���M�b�o���v�O�ʭ���"
        "�ɥN�M���s�}�l�O�̨γ~�|�C\n\r�ߦ��}�a�~���س]�M���ڭ�"
        "�ɱ����ɨ��o�ӯ}�a���Pı�a�T\n\r\n\r\e[0m"
        , mob_name( NULL, ch ) );
    }
    else
    {
      sprintf( buf, "\e[1m����ǨӤF�@�}���l���l���M�b�o���v�O�ʭ���"
        "�ɥN�M���s�}�l�O�̨γ~�|�C\n\r�ߦ��}�a�~���س]�M���ڭ�"
        "�ɱ����ɨ��o�ӯ}�a���Pı�a�T\n\r\n\r\e[0m" );
    }

    send_to_all_char( buf );

    sprintf( buf, "�t�αN�b %s �����T\n\r"
      , time_format( future, "%r, %a-%d-%b-%y" ) );

    send_to_all_char( buf );
  }

  else if ( is_number( arg ) )
  {
    if ( ( minutes = atoi( arg ) ) <= 0 || minutes > 60 )
    {
      if ( ch )
        send_to_char( "�t�������ɶ����X�k( 1 �� 60 ���� )�C\n\r", ch );

      RETURN_NULL();
    }

    if ( ch && !IS_NPC( ch ) && !IS_SET( ch->act, PLR_WIZINVIS ) )
    {
      sprintf( buf, "\e[1m����ǨӤF%s���l���l���M�b�o���v�O�ʭ���"
        "�ɥN�M���s�}�l�O�̨γ~�|�C\n\r�ߦ��}�a�~���س]�M���ڭ�"
        "�ɱ����ɨ��o�ӯ}�a���Pı�a�T\n\r\n\r\e[0m"
        , mob_name( NULL, ch ) );
    }
    else
    {
      sprintf( buf, "\e[1m����ǨӤF�@�}���l���l���M�b�o���v�O�ʭ���"
        "�ɥN�M���s�}�l�O�̨γ~�|�C\n\r�ߦ��}�a�~���س]�M���ڭ�"
        "�ɱ����ɨ��o�ӯ}�a���Pı�a�T\n\r\n\r\e[0m" );
    }

    send_to_all_char( buf );
    sprintf( buf, "�t�αN�b %d�����������C\n\r", minutes );
    send_to_all_char( buf );

    shutdown_time = current_time + minutes * 60;
    reboot_time   = ( time_t ) 0;
    wizlock       = TRUE;
    first_warn    = minutes > FIRST_WARN  ? TRUE : FALSE;
    second_warn   = minutes > SECOND_WARN ? TRUE : FALSE;
    third_warn    = minutes > THIRD_WARN  ? TRUE : FALSE;
  }

  else
  {
    if ( ch ) send_to_char( "�y�k���~�M�Ьd�ߨϥΤ�k�C\n\r", ch );
  }

  RETURN_NULL();
}

/* �ʵ����a���ʧ@ */
FUNCTION( do_snoop )
{
  char              arg[MAX_INPUT_LENGTH];
  DESCRIPTOR_DATA * d;
  CHAR_DATA       * victim;
  int               count;

  PUSH_FUNCTION( "do_snoop" );

  one_argument( argument, arg );

  if ( !str_prefix( arg, "!list" ) )
  {
    clear_buffer();
    for ( count = 0, d = descriptor_list; d; d = d->next )
    {
      if ( verify_desc( d )
        && d->snoop_by == ch->desc
        && ( victim = d->character ) )
      {
        if ( count == 0 )
          send_to_buffer( "\e[1;33;44m���� �� �a �W ��  ���� "
            "����W��                                             \e[0m\n\r" );

        send_to_buffer( "%3d. %-12s %4d %-s\n\r"
          , ++count, victim->name, victim->level, victim->cname );

        if ( buffer_full() ) break;
      }
    }

    if ( count == 0 ) send_to_buffer( "�A�S���ʵ�����H�T\n\r", ch );
    print_buffer( ch );

    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "!clear" ) )
  {
    for ( count = 0, d = descriptor_list; d; d = d->next )
    {
      if ( verify_desc( d ) && d->snoop_by == ch->desc )
      {
        count++;
        d->snoop_by = NULL;
      }
    }

    if ( count == 0 ) send_to_char( "�A�쥻�N�S���ʵ��֡T\n\r", ch );
    else send_to_char( "�M���A�Ҧ��ʵ��H����X�C\n\r", ch );

    RETURN_NULL();
  }

  if ( !( victim = get_char_world( ch, arg ) ) )
  {
    send_to_char( "�䤣��A�Q�ʵ����H�C\n\r", ch );
    RETURN_NULL();
  }

  if ( !victim->desc )
  {
    act( "$N�S���y�z��M�ҥH����ʵ��C", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( victim == ch )
  {
    send_to_char( "�����Ҧ��ʵ��ʧ@�C\n\r", ch );

    for ( d = descriptor_list; d; d = d->next )
      if ( verify_desc( d ) && d->snoop_by == ch->desc ) d->snoop_by = NULL;

    RETURN_NULL();
  }

  if ( victim->desc->snoop_by )
  {
    act( "$N�w�g�Q�䥦���޲z�̺ʵ��F�C", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( get_trust( victim ) >= get_trust( ch ) )
  {
    act( "�A�����ŨS��$N�����M�ҥH���ѤF�C", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( ch->desc )
  {
    for ( d = ch->desc->snoop_by; d; d = d->snoop_by )
    {
      if ( d->character == victim )
      {
        send_to_char( "�S������ʵ���{�C\n\r", ch );
        RETURN_NULL();
      }
    }
  }

  victim->desc->snoop_by = ch->desc;
  act( "�ʵ��]�w�M�����п�J $2snoop $t$0�C",ch, ch->name, NULL, TO_CHAR );
  RETURN_NULL();
}

FUNCTION( do_mload )
{
  char             arg[MAX_INPUT_LENGTH];
  MOB_INDEX_DATA * pMobIndex;
  CHAR_DATA      * victim;

  PUSH_FUNCTION( "do_mload" );

  one_argument( argument, arg );

  if ( !arg[0] || !is_number(arg) )
  {
    send_to_char( "�y�k�R mload <���X>�C\n\r", ch );
    RETURN_NULL();
  }

  if ( !( pMobIndex = get_mob_index( atoi( arg ) ) ) )
  {
    send_to_char( "���X���~�M�S���@��Ǫ������Ӹ��X�C\n\r", ch );
    RETURN_NULL();
  }

  if ( ( victim = shape_mobile( pMobIndex, ch->in_room ) ) )
  {
    smash_point( victim->name        );
    smash_point( victim->byname      );
    smash_point( victim->long_descr  );
    smash_point( victim->description );

    act( "$n�гy�X�@��$N�T", ch, NULL, victim, TO_ROOM );
    act( "�n�F�M�A�гy�X�@��$N�C", ch, NULL, victim, TO_CHAR );
  }
  else
  {
    send_to_char( "�ƻs�Ǫ����ѡC\n\r" , ch );
  }
  RETURN_NULL();
}

FUNCTION( do_oload )
{
  char             arg1[MAX_INPUT_LENGTH];
  char             arg2[MAX_INPUT_LENGTH];
  OBJ_INDEX_DATA * pObjIndex;
  OBJ_DATA       * obj;
  int              level;

  PUSH_FUNCTION( "do_oload" );

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );

  if ( !arg1[0] || !is_number( arg1 ) )
  {
    send_to_char( "�y�k�Roload <���X> <����>�C\n\r", ch );
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
      send_to_char( "�y�k�Roload <���X> <����>�C\n\r", ch );
      RETURN_NULL();
    }

    if ( ( level = atoi( arg2 ) ) < 0 || level > get_trust( ch ) )
    {
      send_to_char( "�A���ѤF�M�]���гy�X�Ӫ��F���A�����ٰ��C\n\r", ch );
      RETURN_NULL();
    }
  }

  if ( !( pObjIndex = get_obj_index( atoi( arg1 ) ) ) )
  {
    send_to_char( "���X���~�M�S���@�˪��~�����X�O�A�Q�n�����X�C\n\r", ch );
    RETURN_NULL();
  }

  if ( pObjIndex == ObjProtype )
  {
    send_to_char( "�藍�_�M�o�Ӫ��~�O����гy���T\n\r", ch );
    RETURN_NULL();
  }

  obj = create_object( pObjIndex, level );
  smash_point( obj->name        );
  smash_point( obj->cname       );
  smash_point( obj->description );

  if ( obj->Takeable
    && obj->item_type != ITEM_CORPSE_NPC
    && obj->item_type != ITEM_CORPSE_PC )
  {
    obj_to_char( obj, ch );
  }
  else
  {
    obj_to_room( obj, ch->in_room );
    act( "$n�гy�X�@��$p�T", ch, obj, NULL, TO_ROOM );
  }

  act( "�A�гy�X�@$T$p�C", ch, obj, obj->unit, TO_CHAR );
  RETURN_NULL();
}

FUNCTION( do_purge )
{
  char        arg[MAX_INPUT_LENGTH];
  CHAR_DATA * victim;
  OBJ_DATA  * obj;

  PUSH_FUNCTION( "do_purge" );

  one_argument( argument, arg );

  if ( !arg[0] )
  {
    CHAR_DATA * vnext;
    OBJ_DATA  * obj_next;

    for ( victim = ch->in_room->people; victim; victim = vnext )
    {
      vnext = victim->next_in_room;
      if ( IS_NPC( victim ) && victim != ch ) extract_char( victim, TRUE );
    }

    for ( obj = ch->in_room->contents; obj; obj = obj_next )
    {
      obj_next = obj->next_content;
      if ( obj->item_type != ITEM_CORPSE_PC ) extract_obj( obj );
    }

    act( "$n�M���o���ж��T", ch, NULL, NULL, TO_ROOM);
    send_to_char( "�n�F�M�M�����ж������C\n\r", ch );
    RETURN_NULL();
  }

  if ( !( victim = get_char_world( ch, arg ) ) )
  {
    send_to_char( "�䤣��A�n�M������H�C\n\r", ch );
    RETURN_NULL();
  }

  if ( !IS_NPC( victim ) )
  {
    act( "����缾�a$N�U�o�ӫ��O�C", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  act( "$n�M���F$N�T", ch, NULL, victim, TO_NOTVICT );
  extract_char( victim, TRUE );
  RETURN_NULL();
}

FUNCTION( do_habilitate )
{
  char        arg1[MAX_INPUT_LENGTH];
  char        arg2[MAX_INPUT_LENGTH];
  CHAR_DATA * victim;
  CHAR_DATA * vch;
  int         number;

  PUSH_FUNCTION( "do_habilitate" );

  argument = one_argument( argument, arg1 );
             one_argument( argument, arg2 );

  if ( !arg1[0] )
  {
    send_to_char( "�y�k�Rhabilitate <��H> <�N�z��O>�C\n\r", ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg1, "!list" ) )
  {
    clear_buffer();

    send_to_buffer( "�ثe%s�����ڥN�z���a�p�U�R\n\r", mud_name );
    for ( number = 0, vch = char_list; vch; vch = vch->next )
    {
      if ( !verify_char( vch )
        || IS_NPC( vch )
        || vch->wizflags <= 0 ) continue;

      send_to_buffer( "%2d. %-23s ( %-12s ) �R%s\n\r"
        , number++
        , vch->cname
        , vch->name
        , wiz_bit_name( vch->wizflags ) );

      if ( buffer_full() ) break;
    }

    send_to_buffer( "\n\r" );
    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !arg2[0] )
  {
    send_to_char( "�y�k�Rhabilitate <���a> <��O>�C\n\r", ch );
    RETURN_NULL();
  }

  if ( !( victim = get_char_room( ch, arg1 ) ) )
  {
    send_to_char( "�䤣��A�n������O����H�C\n\r", ch);
    RETURN_NULL();
  }

  if ( IS_NPC( victim ) )
  {
    act( "���൹���Ǫ�$N�o�د�O�C", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( !IS_HERO( victim ) || IS_IMMORTAL( victim ) )
  {
    act( "$N����������ιL���M�Ф��n�ᤩ�L��O�T", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  switch( arg2[0] )
  {
  default:
    send_to_char( "�S���o�د��ڥN�z��O�T\n\r", ch );
    RETURN_NULL();

  case '!':
    if ( !str_prefix( arg2 + 1, "delete" ) )
    {
      victim->wizflags = 0;
      send_to_char( "�������ڥN�z��O�����T\n\r", ch );
      act( "�A�����ڥN�z��O���Q$N�����F�T", victim, NULL, ch, TO_CHAR );

      mudlog( LOG_WIZFLAGS, "%s ���� %s �����N�z��O�C"
        , ch->name, victim->name );
    }

    break;

  case 'n':
    if ( !str_prefix( arg2, "notell" ) )
    {
      if ( IS_SET( victim->wizflags, WIZ_NOTELL ) )
      {
        REMOVE_BIT( victim->wizflags, WIZ_NOTELL );

        act( "����$N���ڥN�z��O$t�����T"
          , ch,  wiz_bit_name( WIZ_NOTELL ), victim, TO_CHAR );

        act( "�A�����ڥN�z��O$t�Q$N�����F�T"
          , victim,  wiz_bit_name( WIZ_NOTELL ), ch, TO_CHAR );

        mudlog( LOG_WIZFLAGS, "%s ���� %s �� %s �N�z��O�C"
          , ch->name, victim->name, wiz_bit_name( WIZ_NOTELL ) );
      }

      else
      {
        SET_BIT( victim->wizflags, WIZ_NOTELL );

        act( "�]�w$N���ڥN�z��O$t�����T"
          , ch,  wiz_bit_name( WIZ_NOTELL ), victim, TO_CHAR );

        act( "�A�Q$N�ᤩ$t�����ڥN�z��O�T"
          , victim,  wiz_bit_name( WIZ_NOTELL ), ch, TO_CHAR );

        send_to_char( "�Х� help tribunal �ӹ�ݬ����ƶ��C\n\r", victim );

        mudlog( LOG_WIZFLAGS, "%s �]�w %s �� %s �N�z��O�C"
          , ch->name, victim->name, wiz_bit_name( WIZ_NOTELL ) );
      }
    }
    break;

  case 's':
    if ( !str_prefix( arg2, "silence" ) )
    {
      if ( IS_SET( victim->wizflags, WIZ_SILENCE ) )
      {
        REMOVE_BIT( victim->wizflags, WIZ_SILENCE );

        act( "����$N���ڥN�z��O$t�����T"
          , ch, wiz_bit_name( WIZ_SILENCE ), victim, TO_CHAR );

        act( "�A�����ڥN�z��O$t�Q$N�����F�T"
          , victim, wiz_bit_name( WIZ_SILENCE ), ch, TO_CHAR );

        mudlog( LOG_WIZFLAGS, "%s ���� %s �� %s �N�z��O�C"
          , ch->name, victim->name, wiz_bit_name( WIZ_SILENCE ) );
      }

      else
      {
        SET_BIT( victim->wizflags, WIZ_SILENCE );

        act( "�]�w$N���ڥN�z��O$t�����T"
          , ch, wiz_bit_name( WIZ_SILENCE ), victim, TO_CHAR );

        act( "�A�Q$N�ᤩ$t�����ڥN�z��O�T"
          , victim, wiz_bit_name( WIZ_SILENCE ), ch, TO_CHAR );

        send_to_char( "�Х� help tribunal �ӹ�ݬ����ƶ��C\n\r", victim );
        mudlog( LOG_WIZFLAGS, "%s �]�w %s �� %s �N�z��O�C"
          , ch->name, victim->name, wiz_bit_name( WIZ_SILENCE ) );
      }
    }
    break;

  }

  RETURN_NULL();
}

FUNCTION( do_advance )
{
  char        arg1[MAX_INPUT_LENGTH];
  char        arg2[MAX_INPUT_LENGTH];
  char        buf[MAX_STRING_LENGTH];
  CHAR_DATA * victim;
  int         level;
  int         iLevel;

  PUSH_FUNCTION( "do_advance" );

  argument = one_argument( argument, arg1 );
             one_argument( argument, arg2 );

  if ( !arg1[0] || !arg2[0] || !is_number( arg2 ) )
  {
    send_to_char( "�y�k�Radvance �u��H�v �u���šv\n\r", ch );
    RETURN_NULL();
  }

  if ( !( victim = get_char_room( ch, arg1 ) ) )
  {
    send_to_char( "�䤣��A�n�ɯŪ���H�C\n\r", ch);
    RETURN_NULL();
  }

  if ( IS_NPC( victim ) )
  {
    act( "����Φb�D���a$N���W�C", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( ( level = atoi( arg2 ) ) < 1 || level > LEVEL_HERO )
  {
    chinese_number( LEVEL_HERO, buf );
    act( "�̰��u��ɨ�$t�šC", ch, buf, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( level > get_trust( ch ) )
  {
    send_to_char( "���@�����Ť�A�ۤv�������ٰ��M�ҥH���ѡC\n\r", ch );
    RETURN_NULL();
  }

  if ( level <= victim->level )
  {
    send_to_char( "���C���a�����šC\n\r", ch );
    send_to_char( "�A�O���O�F�F�����a�ơM���M���Q�����šC\n\r", victim );

    victim->level    = 1;
    victim->exp      = 0;
    victim->max_hit  = 10;
    victim->max_mana = 100;
    victim->max_move = 100;
    victim->practice = 0;
    victim->hit      = get_curr_hit( victim );
    victim->mana     = get_curr_mana( victim );
    victim->move     = get_curr_move( victim );
    advance_level( victim );
  }

  else
  {
    act( "���@���a$N�����šC", ch, NULL, victim, TO_CHAR );
    send_to_char(
      "���M�A�O����j�H���S���M�Х@����������A���@���šS\n\r", victim );
  }

  for ( iLevel = victim->level ; iLevel < level; iLevel++ )
  {
    send_to_char( "\e[1;32m�A�ɯŤF�T\e[0m", victim );
    victim->level++;
    advance_level( victim );
  }

  victim->exp   = 0;
  victim->trust = 0;
  RETURN_NULL();
}

FUNCTION( do_trust )
{
  char        arg1[MAX_INPUT_LENGTH];
  char        arg2[MAX_INPUT_LENGTH];
  char        buf[MAX_STRING_LENGTH];
  int         level;
  CHAR_DATA * victim;

  PUSH_FUNCTION( "do_trust" );

  argument = one_argument( argument, arg1 );
             one_argument( argument, arg2 );

  if ( !arg1[0] || !arg2[0] || !is_number( arg2 ) )
  {
    send_to_char( "�y�k�Rtrust <��H> <����>�C\n\r", ch );
    RETURN_NULL();
  }

  if ( !( victim = get_char_room( ch, arg1 ) ) )
  {
    send_to_char( "�A�n���@�H��ת����a���b�o�̡C\n\r", ch );
    RETURN_NULL();
  }

  if ( IS_NPC( victim ) )
  {
    act( "�A���ണ�@�D���a$N���H��סC", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( ( level = atoi( arg2 ) ) < 0 || level > MAX_LEVEL - 1 )
  {
    chinese_number( MAX_LEVEL - 1, buf );
    act( "���ų̰��u���$t�šC", ch, buf, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( level > get_trust( ch ) )
  {
    send_to_char( "�A���@���H��פ�A�ۤv�ٰ��M�ҥH���ѤF�C\n\r", ch );
    RETURN_NULL();
  }

  victim->trust = level;
  RETURN_NULL();
}

FUNCTION( do_restore )
{
  char        arg[MAX_INPUT_LENGTH];
  CHAR_DATA * victim;

  PUSH_FUNCTION( "do_restore" );

  one_argument( argument, arg );

  if ( arg[0] == '!' )
  {
    if ( !( victim = get_char_world( ch, arg + 1 ) ) )
    {
      send_to_char( "�A�Q��_����H���b�o�̡C\n\r", ch );
      RETURN_NULL();
    }

    while ( victim->affected ) affect_remove( victim, victim->affected );

    act( "$n�����Ҧ��A���W���k�N�C", ch, NULL, victim, TO_VICT );
    act( "����$N���W�Ҧ����k�N�C", ch, NULL, victim, TO_CHAR );

    RETURN_NULL();
  }

  if ( !( victim = get_char_world( ch, arg ) ) )
  {
    send_to_char( "�A�Q��_����H���b�o�̡C\n\r", ch );
    RETURN_NULL();
  }

  victim->hit  = get_curr_hit( victim );
  victim->mana = get_curr_mana( victim );
  victim->move = get_curr_move( victim );
  update_pos( victim );

  act( "$n��_�A�����A�C", ch, NULL, victim, TO_VICT );
  act( "��_$N���A�����C", ch, NULL, victim, TO_CHAR );

  RETURN_NULL();
}

FUNCTION( do_freeze )
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  PUSH_FUNCTION( "do_freeze" );

  one_argument( argument, arg );

  if ( !( victim = get_char_world( ch, arg ) ) )
  {
    send_to_char( "�A�n�N�᪺��H���b�o�̡C\n\r", ch );
    RETURN_NULL();
  }

  if ( IS_NPC( victim ) )
  {
    act( "�����D���a$N���o�Ӱʧ@�C", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( get_trust( victim ) >= get_trust( ch ) )
  {
    act( "$N���H��פ�A�٭n���M�ҥH�A���ѤF�C", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( IS_SET( victim->act, PLR_FREEZE ) )
  {
    REMOVE_BIT(victim->act, PLR_FREEZE);
    send_to_char( "�A�i�H�~�򪱤F�C\n\r", victim );
    act( "$N���N��X�в��������C", ch, NULL, victim, TO_CHAR );
  }
  else
  {
    SET_BIT( victim->act, PLR_FREEZE );
    send_to_char( "�藍�_�M�A�{�b���వ����ʧ@�C\n\r", victim );
    act( "$N���N��X�г]�w�����C", ch, NULL, victim, TO_CHAR );
  }

  save_char_obj( victim , SAVE_FILE );
  RETURN_NULL();
}

FUNCTION( do_log )
{
  char        arg[MAX_INPUT_LENGTH];
  CMD_DATA  * pCommand;
  CHAR_DATA * victim;

  PUSH_FUNCTION( "do_log" );

  argument = one_argument( argument, arg );

  if ( !str_cmp( arg, "cmd" ) )
  {
    if ( argument[0] == '\x0' )
    {
      send_to_char( "�A�n�l�ܨ��ӫ��O�S\n\r", ch );
      RETURN_NULL();
    }

    for ( pCommand = cmd_list; pCommand; pCommand = pCommand->next )
    {
      if ( !str_cmp( pCommand->name, argument ) )
      {
        if ( pCommand->level > ch->level )
        {
          act( "���O $2$t$0 �����浥�Ť�A���Ű��M�L�k�l�ܡT"
            , ch, pCommand->name, NULL, TO_CHAR );
          RETURN_NULL();
        }

        if ( pCommand->mobonly )
        {
          act( "�u���Ǫ��~�|����o�ӫ��O $2$t$0�M�ҥH�l�ܵL�ġT"
            , ch, argument, NULL, TO_CHAR );
          RETURN_NULL();
        }

        if ( pCommand->trace )
        {
          act( "�A�������O$t�l�ܪ��A�T", ch, argument, NULL, TO_CHAR );
          pCommand->trace = FALSE;
        }
        else
        {
          act( "�A����O$t�]�w���l�ܡT", ch, argument, NULL, TO_CHAR );
          pCommand->trace = TRUE;
        }

        RETURN_NULL();
      }
    }

    act( "�䤣��A�n�l�ܪ����O $2$t$0�C", ch, argument, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( !str_cmp( arg, "all" ) )
  {
    if ( fLogAll )
    {
      fLogAll = FALSE;
      send_to_char( "�Ҧ����a���аO�X�г��Q�����C\n\r", ch );
    }

    else
    {
      fLogAll = TRUE;
      send_to_char( "�Ҧ����a���Q�аO�_�ӤF�C\n\r", ch );
    }
    RETURN_NULL();
  }

  if ( !( victim = get_char_world( ch, arg ) ) )
  {
    send_to_char( "�A�n�аO����H���b�o�̡C\n\r", ch );
    RETURN_NULL();
  }

  if ( IS_NPC( victim ) )
  {
    act( "�����D���a$N���o�Ӱʧ@�C", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( IS_SET( victim->act, PLR_LOG ) )
  {
    REMOVE_BIT(victim->act, PLR_LOG );
    act( "$N���аO�X�гQ�����F�C", ch, NULL, victim, TO_CHAR );
  }
  else
  {
    SET_BIT( victim->act, PLR_LOG );
    act( "$N���аO�X�гQ�аO�F�C", ch, NULL, victim, TO_CHAR );
  }

  RETURN_NULL();
}

FUNCTION( do_noemote )
{
  char        arg[MAX_INPUT_LENGTH];
  CHAR_DATA * victim;

  PUSH_FUNCTION( "do_noemote" );

  one_argument( argument, arg );

  if ( !( victim = get_char_world( ch, arg ) ) )
  {
    send_to_char( "�A����H���b�o�̡C\n\r", ch );
    RETURN_NULL();
  }

  if ( IS_NPC( victim ) )
  {
    act( "����Φb�D���a$N���W�C", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( get_trust( victim ) >= get_trust( ch ) )
  {
    act( "�A���H��פ�$N�C�M�ҥH���ѤF�C", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( IS_SET( victim->act, PLR_NO_EMOTE ) )
  {
    REMOVE_BIT(victim->act, PLR_NO_EMOTE);
    send_to_char( "�A�i�H���s�ϥ�emote�o�ӫ��O�C\n\r", victim );
    act( "$N�� NO_EMOTE�X�в��������C", ch, NULL, victim, TO_CHAR );
  }
  else
  {
    SET_BIT( victim->act, PLR_NO_EMOTE );
    send_to_char( "�藍�_�M�A����ϥ�emote�o�ӫ��O�F�C\n\r", victim );
    act( "$N�� NO_EMOTE�X�г]�w�����C", ch, NULL, victim, TO_CHAR );
  }

  RETURN_NULL();
}

FUNCTION( do_notell )
{
  char        arg[MAX_INPUT_LENGTH];
  char        arg2[MAX_INPUT_LENGTH];
  CHAR_DATA * victim;
  char        buf[MAX_STRING_LENGTH];
  int         timer;

  PUSH_FUNCTION( "do_notell" );

  argument = one_argument( argument, arg );
             one_argument( argument, arg2 );

  if ( arg[0] == '\x0' || arg2[0] == '\x0' || !is_number( arg2 ) )
  {
    send_to_char( "��J���~�M�Ш� notell /? \n\r", ch );
    RETURN_NULL();
  }

  if ( get_trust( ch ) < LEVEL_IMMORTAL
    && !IS_SET( ch->wizflags, WIZ_NOTELL ) )
  {
    send_to_char( "�A���v�O�����M����]�w�O�H���\\�i�D�O�H�ܻy�T\n\r", ch );
    RETURN_NULL();
  }

  if ( !( victim = get_char_world( ch, arg ) ) )
  {
    send_to_char( "�A����H���b�o�̡C\n\r", ch );
    RETURN_NULL();
  }

  if ( IS_NPC( victim ) )
  {
    act( "�����D���a$N���o�Ӱʧ@�C", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( get_trust( victim ) >= get_trust( ch ) )
  {
    act( "�A���H��פ�$N�C�M�ҥH�A���ѤF�C", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( ( timer = atoi( arg2 ) ) > MAX_TRIBUNAL || timer <= 0 )
  {
    chinese_number( MAX_TRIBUNAL, buf );
    act( "�A�]�w���ɶ�����W�L$t�Τp��s�T", ch, buf, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( get_trust( ch ) == LEVEL_HERO
    && IS_SET( ch->wizflags, WIZ_NOTELL )
    && timer > MAX_TRIBUNAL_PC )
  {
    chinese_number( MAX_TRIBUNAL_PC, buf );
    act( "�A���H�g�@���ɶ����i�H�W�L$t�C", ch, buf, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( victim->tribunal_timer > 0
    && !IS_SET( victim->act, PLR_NO_TELL ) )
  {
    act( "$N�w�g�b���g�@�F�M���n�A�g�@�L�F�T", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( IS_SET( victim->act, PLR_NO_TELL ) )
  {
    REMOVE_BIT(victim->act, PLR_NO_TELL);
    victim->tribunal_timer = 0;

    send_to_char( "�A�i�H�~��ϥ� tell �o�ӫ��O�F�C\n\r", victim );
    act( "$N�� NO_TELL �X�в��������C", ch, NULL, victim, TO_CHAR );

    sprintf( buf, "%s�Q���� %s ���B�m�F�T"
      , mob_name( NULL, victim ), act_bit_name( PLR_NO_TELL ) );

    talk_channel_2( buf, CHANNEL_BULLETIN, "���" );

    if ( IS_IMMORTAL( ch ) ) mudlog( LOG_WIZFLAGS, "%s ���� %s ���T TELL ."
        , ch->name, mob_name( NULL, victim ) );
  }
  else
  {
    SET_BIT( victim->act, PLR_NO_TELL );
    victim->tribunal_timer = timer;

    send_to_char( "�A����ϥ� tell �o�ӫ��O�F�C\n\r", victim );
    act( "$N�� NO_TELL �X�г]�w�����C", ch, NULL, victim, TO_CHAR );

    sprintf( buf, "%s�D�� %s ���B�m�F�T"
      , mob_name( NULL, victim ), act_bit_name( PLR_NO_TELL ) );

    talk_channel_2( buf, CHANNEL_BULLETIN, "���" );

    if ( IS_IMMORTAL( ch ) )
      mudlog( LOG_WIZFLAGS, "%s �]�w %s ����ϥ� TELL %d ����."
        , ch->name, mob_name( NULL, victim ) , timer );
  }

  RETURN_NULL();
}

FUNCTION( do_silence )
{
  char        arg[MAX_INPUT_LENGTH];
  char        arg2[MAX_INPUT_LENGTH];
  char        buf[MAX_STRING_LENGTH];
  CHAR_DATA * victim;
  int         timer;

  PUSH_FUNCTION( "do_silence" );

  argument = one_argument( argument, arg );
             one_argument( argument, arg2 );

  if ( arg[0] == '\x0' || arg2[0] == '\x0' || !is_number( arg2 ) )
  {
    send_to_char( "�A����J����M�Ш� silence /? \n\r", ch );
    RETURN_NULL();
  }

  if ( get_trust( ch ) < LEVEL_IMMORTAL
    && !IS_SET( ch->wizflags, WIZ_SILENCE ) )
  {
    send_to_char( "�A���v�O�����M����]�w�O�H�T���T\n\r", ch );
    RETURN_NULL();
  }

  if ( !( victim = get_char_world( ch, arg ) ) )
  {
    send_to_char( "�A����H���b�o�̡C\n\r", ch );
    RETURN_NULL();
  }

  if ( IS_NPC( victim ) )
  {
    act( "�����D���a$N���o�ӫ��O�C", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( get_trust( victim ) >= get_trust( ch ) )
  {
    act( "$N���H��פ�A���M�ҥH�A���ѤF�C", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( ( timer = atoi( arg2 ) ) > MAX_TRIBUNAL || timer <= 0 )
  {
    chinese_number( MAX_TRIBUNAL, buf );
    act( "�A�]�w���ɶ�����W�L$t�Τp��s�T", ch, buf, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( IS_IMMORTAL( ch )
    && IS_SET( ch->wizflags, WIZ_SILENCE )
    && timer > MAX_TRIBUNAL_PC )
  {
    chinese_number( MAX_TRIBUNAL_PC, buf );
    act( "�A���H�g�@���ɶ����i�H�W�L$t�ѡC", ch, buf, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( victim->tribunal_timer > 0
    && !IS_SET( victim->act, PLR_SILENCE ) )
  {
    act( "$N�w�g�b���g�@�F�M���n�A�g�@�L�F�T", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( IS_SET( victim->act, PLR_SILENCE ) )
  {
    REMOVE_BIT( victim->act, PLR_SILENCE );
    victim->tribunal_timer = 0;

    send_to_char( "�A�i�H�~��ϥΧA���W�D�C\n\r", victim );
    act( "$N�� SILENCE�X�в��������C", ch, NULL, victim, TO_CHAR );

    sprintf( buf, "%s�Q���� %s ���B�m�F�T"
      , mob_name( NULL, victim ), act_bit_name( PLR_SILENCE ) );

    talk_channel_2( buf, CHANNEL_BULLETIN, "���" );

    if ( IS_IMMORTAL( ch ) ) mudlog( LOG_WIZFLAGS, "%s ���� %s ���T��."
        , ch->name, mob_name( NULL, victim ) );
  }
  else
  {
    SET_BIT( victim->act, PLR_SILENCE );
    victim->tribunal_timer = timer;

    send_to_char( "�A�S���A���W�D�F�C\n\r", victim );
    act( "$N�� SILENCE�X�г]�w�����C", ch, NULL, victim, TO_CHAR );

    sprintf( buf, "%s�Q�аO %s ���B�m�F�T"
      , mob_name( NULL, victim ), act_bit_name( PLR_SILENCE ) );

    talk_channel_2( buf, CHANNEL_BULLETIN, "���" );

    if ( IS_IMMORTAL( ch ) )
      mudlog( LOG_WIZFLAGS, "%s �]�w %s �T�� %d ����."
        , ch->name, mob_name( NULL, victim ) , timer );
  }

  RETURN_NULL();
}

FUNCTION( do_peace )
{
  CHAR_DATA * rch;

  PUSH_FUNCTION( "do_peace" );

  for ( rch = ch->in_room->people; rch; rch = rch->next_in_room )
    if ( rch->fighting ) stop_fighting( rch, TRUE );

  sendmsg_to_room( "�藍�_�M�԰�����T\n\r", ch );
  RETURN_NULL();
}

FUNCTION( do_ban )
{
  char       arg [MAX_INPUT_LENGTH];
  char       type[MAX_INPUT_LENGTH];
  BAN_DATA * pban;
  int        format;

  PUSH_FUNCTION( "do_ban" );

  argument = one_argument( argument, arg );
             one_argument( argument , type );

  if ( !arg[0] )
  {
    int Found = FALSE;

    clear_buffer();
    send_to_buffer( "�T��s������}�R\n\r" );

    for ( pban = ban_list; pban; pban = pban->next )
    {
      Found = TRUE;
      send_to_buffer( "%-20s �X�СR%s\n\r"
        , pban->name, ban_bit_name( pban->type ) );

      if ( buffer_full() ) break;
    }

    if ( !Found ) send_to_buffer( "�S������Q�T���}�C\n\r" );
    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !type[0] )
  {
    send_to_char( "�A�����ޤJ��}�X�СC\n\r" , ch );
    RETURN_NULL();
  }

  switch( ( format = atoi( type ) ) )
  {
  default :
    send_to_char( "�A����}�X�Ф���C\n\r" , ch );
    RETURN_NULL();

  case BAN_LOGIN  :
  case BAN_FQDN   :
  case BAN_FINGER :
    break;
  }

  for ( pban = ban_list; pban ; pban = pban->next )
  {
    if ( !str_cmp( arg, pban->name ) )
    {
      act( "�o�Ӧ�} $t �w�g�Q�T��s�u�F�T", ch, arg, NULL, TO_CHAR );
      RETURN_NULL();
    }
  }

  /* �t�m�O���� */
  pban = alloc_struct( STRUCT_BAN_DATA );

  pban->name = str_dup( arg );
  pban->type = format;
  pban->next = ban_list;
  ban_list   = pban;

  act( "�T���} $t �]�w�����C", ch, arg, NULL, TO_CHAR );
  RETURN_NULL();
}

FUNCTION( do_allow )
{
  char       arg[MAX_INPUT_LENGTH];
  BAN_DATA * prev;
  BAN_DATA * curr;

  PUSH_FUNCTION( "do_allow" );

  one_argument( argument, arg );

  for ( curr = ban_list, prev = NULL; curr; prev = curr, curr = curr->next )
  {
    if ( !str_cmp( arg, curr->name ) )
    {
      if ( curr->type == BAN_LOGIN )
      {
        if ( !prev ) ban_list   = ban_list->next;
        else         prev->next = curr->next;

        free_string( curr->name );
        curr->type  = 0;

        /* ����O���� */
        free_struct( curr , STRUCT_BAN_DATA );

        send_to_char( "�o�Ӧ�}���A�Q�T��s�u�i�ӡC\n\r", ch );
        RETURN_NULL();
      }
      else
      {
        send_to_char( "�o�Ӧ�}���X�ШëD�T��s�u�C\n\r" , ch );
        RETURN_NULL();
      }
    }
  }

  send_to_char( "���o�{��A�Q�n����}�C\n\r", ch );
  RETURN_NULL();
}

FUNCTION( do_wizlock )
{
  char arg[MAX_INPUT_LENGTH];

  PUSH_FUNCTION( "do_wizlock" );

  argument = one_argument( argument, arg );

  if ( arg[0] != '\x0' && !str_cmp( arg, "new" ) )
  {
    NewLock = !NewLock;

    if ( NewLock ) send_to_char( "�s���a�Q�޲z����w�i�J�T\n\r", ch );
    else           send_to_char( "�s���a�Q�޲z�̸Ѱ���w�T\n\r", ch );
  }

  else
  {
    wizlock = !wizlock;
    if ( wizlock ) send_to_char( "�C���Q�Х@������w�F�C\n\r", ch );
    else           send_to_char( "�Х@�����Ѱ��C����w�C\n\r", ch );
  }

  RETURN_NULL();
}

/* �d�ߧޯ�άO�k�N�����A */
FUNCTION( do_llookup )
{
  SKILL_DATA    * pSkill;
  SKILL_DATA    * aSkill;
  LIMIT_DATA    * pLimit;
  DAMAGE_DATA   * pDamage;
  RESTRICT_DATA * pRestrict;
  AFFECT_DATA   * pAffect;
  int             loop;
  int             count;
  char            arg[MAX_INPUT_LENGTH];
  char            chinese[MAX_INPUT_LENGTH];

  PUSH_FUNCTION( "do_llookup" );

  if ( !skill_list )
  {
    act( "$t�̨S������ޯ���J.", ch, mud_name, NULL, TO_CHAR );
    RETURN_NULL();
  }

  one_argument( argument , arg );

  if ( !str_prefix( arg , "!list" ) )
  {
    clear_buffer();
    send_to_buffer( "\e[1;33;44m%6s %20s %20s %-20s\e[0m\n\r"
      , "�s��" , "�ޯ�^��W��" , "�ޯत��W��", "����" );

    for ( pSkill = skill_list; pSkill; pSkill = pSkill->next )
    {
      send_to_buffer( "%5d  %20s %20s %-10s\n\r"
        , pSkill->slot
        , pSkill->name
        , pSkill->cname ? pSkill->cname : "�q��"
        , pSkill->rating ? skill_rating( pSkill->rating ) : "�q��" );

      if ( buffer_full() ) break;
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "!innate" ) )
  {
    clear_buffer();

    for ( loop = count = 0; loop < nInnate; loop++ )
    {
      if ( !( pSkill = get_skill( iInnate[loop] ) ) ) continue;

      if ( count == 0 ) send_to_buffer(
         "\e[1;33;44m%4s %6s %8s %6s %20s %20s %-10s\e[0m\n\r"
         , "����" , "�s��" , "����H��", "���m��" , "�ޯ�^��W��"
         , "�ޯत��W��", "����" );

      send_to_buffer( "%4d %5d  %8d %6d %20s %20s %-10s\n\r"
        , ++count
        , pSkill->slot
        , pSkill->choosen
        , pSkill->adeptation
        , pSkill->name
        , pSkill->cname ? pSkill->cname : "�q��"
        , pSkill->rating ? skill_rating( pSkill->rating ) : "�q��" );

      if ( buffer_full() ) break;
    }

    if ( count == 0 ) send_to_buffer( "���t�ΨS������w�]�ޯ�T\n\r" );
    print_buffer( ch );
    RETURN_NULL();
  }

  if ( is_number( arg ) ) pSkill = get_skill( atoi( arg ) );
  else                    pSkill = skill_isname( arg );

  if ( !pSkill )
  {
    send_to_char( "�䤣��A�n���k�N�άO�ޯ�W�١C\n\r" , ch );
    RETURN_NULL();
  }

  for ( loop = 0, pDamage = pSkill->damage; pDamage; pDamage = pDamage->next )
    loop++;

  clear_buffer();
  send_to_buffer(
    "�k�N���X[%4d]\n\r"
    "�^��W�� %s\n\r"
    "����W�� %s\n\r"
    "�k�N������ %s\n\r"
    "�۫g�t�k�N %s\n\r"
    "�k�N�������T�� %s\n\r"
    "�I�k���ݮɶ� %d\n\r"
    "�I�k�g������v �U���� %d\n\r"
    "�I�k���O���ƭ� %d\n\r"
    "�w�����H��   %d\n\r"
    "�w�諸���m��   %d\n\r"
    "�O�_�i�H���۾ǲߡR%s\n\r"
    "�O�_�i�H�Q�P��R%s\n\r"
    "�O�_���n��X�G�y�R%s\n\r"
    "�O�_���N�Ӥ������R%s\n\r"
    "�O�_�i�H�I�i�R%s\n\r"
    "�O�_���w�]�ޯ�R%s\n\r"
    "�O�_�i�H�d�ߡR%s\n\r"
    "�O�_�}��R%s\n\r"
    "�ޯ୭��ʧO�R%s(%d)\n\r"
    "�m�߾��v�R%d\n\r"
    "�m�߭���ӼơR%d\n\r"
    "�v�T�����R%s\n\r"
    "���~�I�k�R%s\n\r"
    "��˱ԭz�R%d\n\r"
    "�����Z���R%s\n\r"
    "���v�`�M�R%d\n\r"
    "��������T���R\n\r%s\n\r"
    , pSkill->slot
    , pSkill->name
    , pSkill->cname   ? pSkill->cname   : "�q��"
    , pSkill->rating  ? skill_rating( pSkill->rating ) : "�q��"
    , pSkill->antirating ? skill_rating( pSkill->antirating ) : "�q��"
    , pSkill->msg_off ? pSkill->msg_off : "�q��"
    , pSkill->wait
    , pSkill->exp
    , pSkill->cost
    , pSkill->innate ? pSkill->choosen    : -1
    , pSkill->innate ? pSkill->adeptation : -1
    , YESNO( pSkill->teach         )
    , YESNO( pSkill->enable        )
    , YESNO( pSkill->say_spell     )
    , YESNO( pSkill->concentration )
    , YESNO( pSkill->cast          )
    , YESNO( pSkill->innate        )
    , YESNO( pSkill->canask        )
    , YESNO( pSkill->valid         )
    , sex_name( pSkill->sex )
    , pSkill->sex
    , pSkill->ply
    , pSkill->qutoient
    , skill_affect( pSkill->affect )
    , obj_cast_location( pSkill->affect_id )
    , loop, weapon_type_name( pSkill->weapon )
    , pSkill->chance
    , pSkill->message ? pSkill->message : "�q��" );

  send_to_buffer( "�I�k���O���ƭȡR" );

  switch( pSkill->cost_type )
  {
  default        : send_to_buffer( "����"   ); break;
  case COST_HIT  : send_to_buffer( "�ͩR�O" ); break;
  case COST_MANA : send_to_buffer( "�k�O"   ); break;
  case COST_MOVE : send_to_buffer( "���ʤO" ); break;
  case COST_GOLD : send_to_buffer( "����"   ); break;
  }

  send_to_buffer( "\n\r�I�k���̧C���A�R%s\n\r", get_position( pSkill->position ) );
  send_to_buffer( "�I�k���������A�R" );

  switch( pSkill->type )
  {
    default                 : send_to_buffer( "����"         ); break;
    case TAR_IGNORE         : send_to_buffer( "�������"     ); break;
    case TAR_CHAR_OFFENSIVE : send_to_buffer( "�������"     ); break;
    case TAR_CHAR_DEFENSIVE : send_to_buffer( "�������m"     ); break;
    case TAR_CHAR_SELF      : send_to_buffer( "�@�Υ���"     ); break;
    case TAR_OBJ_INV        : send_to_buffer( "�@�Ϊ�������" ); break;
    case TAR_DODGE          : send_to_buffer( "�����{��"     ); break;
    case TAR_OBJ_ROOM       : send_to_buffer( "�@�Ωж����~" ); break;
  }

  send_to_buffer( "\n\r�k�N�I�s��ƫ��A�R " );
  if ( !pSkill->function )
  {
    send_to_buffer( "�q�ʡC\n\r" );
  }
  else if ( pSkill->function == cast_null )
  {
    send_to_buffer( "���I�s��ơC\n\r" );
  }
  else
  {
    send_to_buffer( "���I�s��ƨӳB�z�C\n\r" );
  }

  if ( pSkill->associate >= 0 && ( aSkill = get_skill( pSkill->associate ) ) )
  {
    send_to_buffer( "�����s����Ƹ��X�R%d �����s���{�סR�U���� %d\n\r"
                    "�����s��ƪ��W�� %s %s\n\r"
      , aSkill->slot, pSkill->degree
      , aSkill->name, aSkill->cname );
  }

  for ( pLimit = pSkill->limit; pLimit; pLimit = pLimit->next )
  {
    if ( pLimit->class )
    {
      send_to_buffer( "%s���ų̧C %d �šM���m�׳̰���� %3d%%�C\n\r"
      , class_name( pLimit->class, FALSE )
      , pLimit->level
      , pLimit->adept );
    }
  }

  for ( count = 0, pRestrict = pSkill->restrict;
    pRestrict;
    pRestrict = pRestrict->next, count++ )
  {
    chinese_number( count + 1, chinese );
    send_to_buffer( "%s.���ޯ�%s\n\r"
      , chinese, restrict_value( pRestrict, ch ) );
  }

  if ( ( pAffect = pSkill->affected ) )
  {
    send_to_buffer( "�k�N�R%s��%s�y�� %d �v�T�٦� %d �p��\n\r",
      pSkill->cname,
      affect_loc_name( pAffect->location ),
      pAffect->modifier,
      pAffect->duration );
  }

  if ( count == 0 ) send_to_buffer( "���ޯ�S�����󪺭���C\n\r" );
  print_buffer( ch );
  RETURN_NULL();
}

/* �d�ߤ���ޯ঳����Ǫ��б� */
FUNCTION( do_lfind )
{
  int              vnum;
  int              nMatch;
  int              count;
  char             arg[MAX_INPUT_LENGTH];
  TEACH_DATA     * teach;
  MOB_INDEX_DATA * pMobIndex;
  SKILL_DATA     * pSkill;
  CHAR_DATA      * man;

  PUSH_FUNCTION( "do_lfind" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  one_argument( argument , arg );

  clear_buffer();

  if ( arg[0] == '!' )
  {
    if ( !( pSkill = skill_isname( arg + 1 ) ) )
    {
      send_to_char( "�S���o�اޯ�άO�k�N�M�Шϥ� llookup �d�ߡC\n\r" , ch );
      RETURN_NULL();
    }

    send_to_buffer( "���о� %s �o�ӧޯ઺�Ǫ����R\n\r", pSkill->cname );

    count = 0;
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man )
        || !IS_NPC( man )
        || !man->pIndexData
        || !man->pIndexData->teach )
        continue;

      for ( teach = man->pIndexData->teach; teach; teach = teach->next )
      {
        if ( pSkill->slot == teach->slot )
        {
          send_to_buffer( "(%2d) %-30s (���X�R%5d �Ҧb�a�R%5d) \n\r"
            , ++count
            , mob_name( NULL, man )
            , man->pIndexData->vnum
            , man->in_room ? man->in_room->vnum : -1 );
        }
      }

      if ( buffer_full() ) break;
    }

    if ( count == 0 ) send_to_buffer( "�S���@���Ǫ��б¦��ޯ�C\n\r" );
  }

  else
  {
    if ( !( pSkill = skill_isname( arg ) ) )
    {
      send_to_char( "�S���o�اޯ�άO�k�N�M�Шϥ� llookup �d�ߡC\n\r" , ch );
      RETURN_NULL();
    }

    send_to_buffer( "���о� %s �o�ӧޯ઺�Ǫ����R\n\r", pSkill->cname );

    for ( count = nMatch = vnum = 0; nMatch < top_mob_index; vnum ++ )
    {
      if ( ( pMobIndex = get_mob_index( vnum ) ) )
      {
        nMatch++;
        for ( teach = pMobIndex->teach; teach; teach = teach->next )
        {
          if ( pSkill->slot == teach->slot )
          {
            send_to_buffer( "(%2d) %s (���X�R%5d �ƶq�R%3d )\n\r"
              , ++count
              , mob_index_name( NULL, pMobIndex )
              , pMobIndex->vnum
              , pMobIndex->count );
          }
        }

        if ( buffer_full() ) break;
      }
    }

    if ( count == 0 ) send_to_buffer( "�S���@���Ǫ��б¦��ޯ�C\n\r" );
  }

  print_buffer( ch );
  RETURN_NULL();
}

/* �]�w�ޯ઺���m�� */
FUNCTION( do_lset )
{
  CHAR_DATA   * victim;
  SKILL_DATA  * pSkill;
  ENABLE_DATA * pEnable;
  ENABLE_DATA * zEnable;
  char          arg1[MAX_INPUT_LENGTH];
  char          arg2[MAX_INPUT_LENGTH];
  char          arg3[MAX_INPUT_LENGTH];
  bool          fAll;
  int           value;

  PUSH_FUNCTION( "do_lset" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  /* �����X�T�ӰѼ� */
  argument = one_argument( argument , arg1 );
  argument = one_argument( argument , arg2 );
  one_argument( argument , arg3 );

  /* ��ݬO�_�ѼƳ������T */
  if ( !arg1[0] || !arg2[0] || !arg3[0] )
  {
    send_to_char( "�y�k�Rlset <��H> <�ޯ�> <���m��>\n\r"
                  "�άO  lset <��H> all    <���m��>\n\r" , ch );
    RETURN_NULL();
  }

  /* �����ݬO�_���o�ӤH */
  if ( !( victim = get_char_world( ch , arg1 ) ) )
  {
    send_to_char( "�䤣��A�n����H�C\n\r", ch );
    RETURN_NULL();
  }

  /* ����@�Φb�D���a���W */
  if ( IS_NPC( victim ) )
  {
    act( "�����D���a$N���o�Ӱʧ@�C" , ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  fAll = !str_cmp( arg2 , "all" );

  /* ��M�ޯ�W��, �Y�O�ĤG�ӰѼƬ� all�N���Χ�F */
  if ( !( ( pSkill = skill_isname( arg2 ) ) ) && !fAll )
  {
    send_to_char( "�䤣��o���k�N�άO�ޯ�C\n\r" , ch );
    RETURN_NULL();
  }

  /* �ĤT�ӼƭȤ��ର�D�Ʀr */
  if ( !is_number( arg3 ) )
  {
    send_to_char( "���m�׳o�@���@�w�n�O�Ʀr�C\n\r" , ch );
    RETURN_NULL();
  }

  if ( ( value = atoi( arg3 ) ) < 0 || value > 100 )
  {
    send_to_char( "���m�ת��Ʀr���X�z�C\n\r", ch );
    RETURN_NULL();
  }

  if ( fAll )
  {
    for ( pSkill = skill_list; pSkill; pSkill = pSkill->next )
    {
      if ( pSkill->slot < 0 || pSkill->slot >= MAX_SKILL )
      {
        mudlog( LOG_DEBUG, "do_lset: �ޯู�X %d ���X�z.", pSkill->slot );
        continue;
      }

      /* �ˬd�O�_�i�H�Ǩ� */
      if ( value > 0 && !check_skill_restrict( victim, pSkill, FALSE ) )
        continue;

      if ( ( victim->skill[pSkill->slot] = value ) <= 0 )
      {
        /* �����L���o���ޯ� */
        for ( pEnable = victim->enable; pEnable; pEnable = zEnable )
        {
          zEnable = pEnable->next;

          if ( pEnable->skill == pSkill )
            extract_an_enable( victim, pSkill );
        }
      }
    }
  }

  else
  {
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
  }

  send_to_char( "�]�w�ޯ৹���C\n\r" , ch );
  RETURN_NULL();
}

FUNCTION( do_mset )
{
  char         arg1[MAX_INPUT_LENGTH];
  char         arg2[MAX_INPUT_LENGTH];
  char         arg3[MAX_INPUT_LENGTH];
  char         buf[MAX_STRING_LENGTH];
  char         buf1[MAX_STRING_LENGTH];
  CHAR_DATA  * victim;
  CLASS_DATA * pClass;
  int          value;

  PUSH_FUNCTION( "do_mset" );

  smash_tilde( argument );
  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );
  str_cpy( arg3, argument );

  if ( !arg1[0] || !arg2[0] || !arg3[0] )
  {
    send_to_char(
      "�y�k�Rmset <��H> <����> <�ƭ�>\n\r"
      "�άO  mset <��H> <�r��> <�ƭ�>\n\r\n\r"
      "���إi�H���U�C��ܡR\n\r"
      "  str int wis dex con sex class level nskill\n\r"
      "  gold hp mana limit move practice align xname rechristen\n\r"
      "  thirst drunk full recall bank firman murder steal dosage\n\r"
      "  donate flags migrate tractable affect\n\r"
      "�r��i�H���U�C��ܡR\n\r"
      "  name short long description title\n\r", ch );
    RETURN_NULL();
  }

  if ( !( victim = get_char_world( ch, arg1 ) ) )
  {
    send_to_char( "�A����H���b�o�̡C\n\r", ch );
    RETURN_NULL();
  }

  value = is_number( arg3 ) ? atoi( arg3 ) : -1;

  if ( !str_prefix( arg2, "flags" ) )
  {
    if ( !IS_NPC( victim ) )
    {
      act( "$N���O�D���a�T", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( !str_prefix( arg3, "sentinel" ) )
    {
      if ( IS_SET( victim->act, ACT_SENTINEL ) )
      {
        REMOVE_BIT( victim->act, ACT_SENTINEL );
        act( "$N���A�O���i���ʡT", ch, NULL, victim, TO_CHAR );
      }
      else
      {
        SET_BIT( victim->act, ACT_SENTINEL );
        act( "$N�S��_���i���ʪ��ʽ�T", ch, NULL, victim, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "scavenger" ) )
    {
      if ( IS_SET( victim->act, ACT_SCAVENGER ) )
      {
        REMOVE_BIT( victim->act, ACT_SCAVENGER );
        act( "$N���A�O�|�۰ʾߪ��~�T", ch, NULL, victim, TO_CHAR );
      }
      else
      {
        SET_BIT( victim->act, ACT_SCAVENGER );
        act( "$N�S��_�|�۰ʾߪ��~���ʽ�T", ch, NULL, victim, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "aggressive" ) )
    {
      if ( IS_SET( victim->act, ACT_AGGRESSIVE ) )
      {
        REMOVE_BIT( victim->act, ACT_AGGRESSIVE );
        act( "$N���A�O�|�۰ʧ����T", ch, NULL, victim, TO_CHAR );
      }
      else
      {
        SET_BIT( victim->act, ACT_AGGRESSIVE );
        act( "$N�S��_�|�۰ʧ������ʽ�T", ch, NULL, victim, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "stayarea" ) )
    {
      if ( IS_SET( victim->act, ACT_STAY_AREA ) )
      {
        REMOVE_BIT( victim->act, ACT_STAY_AREA );
        act( "$N���A�O�|���d�b���ϰ�T", ch, NULL, victim, TO_CHAR );
      }
      else
      {
        SET_BIT( victim->act, ACT_STAY_AREA );
        act( "$N�S��_�|���d�b���ϰ쪺�ʽ�T", ch, NULL, victim, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "wimpy" ) )
    {
      if ( IS_SET( victim->act, ACT_WIMPY ) )
      {
        REMOVE_BIT( victim->act, ACT_WIMPY );
        act( "$N���A�O�|���˰k�]�T", ch, NULL, victim, TO_CHAR );
      }
      else
      {
        SET_BIT( victim->act, ACT_WIMPY );
        act( "$N�S��_�|���˰k�]���ʽ�T", ch, NULL, victim, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "train" ) )
    {
      if ( IS_SET( victim->act, ACT_TRAIN ) )
      {
        REMOVE_BIT( victim->act, ACT_TRAIN );
        act( "$N���A�O�V�m�ݩʪ̡T", ch, NULL, victim, TO_CHAR );
      }
      else
      {
        SET_BIT( victim->act, ACT_TRAIN );
        act( "$N�S��_�V�m�ݩʪ̪��ʽ�T", ch, NULL, victim, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "rebirth" ) )
    {
      if ( IS_SET( victim->act, ACT_REBIRTH ) )
      {
        REMOVE_BIT( victim->act, ACT_REBIRTH );
        act( "$N���A�O��¾�̡T", ch, NULL, victim, TO_CHAR );
      }
      else
      {
        SET_BIT( victim->act, ACT_REBIRTH );
        act( "$N�S��_��¾�̪��ʽ�T", ch, NULL, victim, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "fight" ) )
    {
      if ( IS_SET( victim->act, ACT_REBORN_FIGHT ) )
      {
        REMOVE_BIT( victim->act, ACT_REBORN_FIGHT );
        act( "$N���A�O�|�A�ͦ���L�Ǫ��T", ch, NULL, victim, TO_CHAR );
      }
      else
      {
        SET_BIT( victim->act, ACT_REBORN_FIGHT );
        act( "$N�S��_�|�A�ͦ���L�Ǫ����ʽ�T", ch, NULL, victim, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "ask" ) )
    {
      if ( IS_SET( victim->act, ACT_ASK ) )
      {
        REMOVE_BIT( victim->act, ACT_ASK );
        act( "$N���A�O�|�s�H���u�T", ch, NULL, victim, TO_CHAR );
      }
      else
      {
        SET_BIT( victim->act, ACT_ASK );
        act( "$N�S��_�|�s�H���u���ʽ�T", ch, NULL, victim, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "noreborn" ) )
    {
      if ( IS_SET( victim->act, ACT_NOREBORN ) )
      {
        REMOVE_BIT( victim->act, ACT_NOREBORN );
        act( "$N���A�O���|���͡T", ch, NULL, victim, TO_CHAR );
      }
      else
      {
        SET_BIT( victim->act, ACT_NOREBORN );
        act( "$N�S��_�|���|���ͪ��ʽ�T", ch, NULL, victim, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "nokill" ) )
    {
      if ( IS_SET( victim->act, ACT_NOKILL ) )
      {
        REMOVE_BIT( victim->act, ACT_NOKILL );
        act( "$N���A�O���i���`�T", ch, NULL, victim, TO_CHAR );
      }
      else
      {
        SET_BIT( victim->act, ACT_NOKILL );
        act( "$N�S��_���i���`���ʽ�T", ch, NULL, victim, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "enroll" ) )
    {
      if ( IS_SET( victim->act, ACT_ENROLL ) )
      {
        REMOVE_BIT( victim->act, ACT_ENROLL );
        act( "$N���A�O�|�O��T", ch, NULL, victim, TO_CHAR );
      }
      else
      {
        SET_BIT( victim->act, ACT_ENROLL );
        act( "$N�S��_�O�몺�ʽ�T", ch, NULL, victim, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "good" ) )
    {
      if ( IS_SET( victim->act, ACT_ALIGN_GOOD ) )
      {
        REMOVE_BIT( victim->act, ACT_ALIGN_GOOD );
        act( "$N���A�O���}���H�T", ch, NULL, victim, TO_CHAR );
      }
      else
      {
        SET_BIT( victim->act, ACT_ALIGN_GOOD );
        act( "$N�S��_���}���H���ʽ�T", ch, NULL, victim, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "evil" ) )
    {
      if ( IS_SET( victim->act, ACT_ALIGN_EVIL ) )
      {
        REMOVE_BIT( victim->act, ACT_ALIGN_EVIL );
        act( "$N���A�O���c���H�T", ch, NULL, victim, TO_CHAR );
      }
      else
      {
        SET_BIT( victim->act, ACT_ALIGN_EVIL );
        act( "$N�S��_���c���H���ʽ�T", ch, NULL, victim, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "nosummon" ) )
    {
      if ( IS_SET( victim->act,ACT_NOSUMMON ) )
      {
        REMOVE_BIT( victim->act, ACT_NOSUMMON );
        act( "$N���A�O���i�l�ꤧ�H�T", ch, NULL, victim, TO_CHAR );
      }
      else
      {
        SET_BIT( victim->act, ACT_NOSUMMON );
        act( "$N�S��_���i�l�ꤧ�H���ʽ�T", ch, NULL, victim, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "speak" ) )
    {
      if ( victim->speak )
      {
        victim->speak = FALSE;
        act( "$N���A�O���|���ܤ��H�T", ch, NULL, victim, TO_CHAR );
      }
      else
      {
        victim->speak = TRUE;
        act( "$N�S��_�|���ܪ��ʽ�T", ch, NULL, victim, TO_CHAR );
      }
    }

    else
    {
      send_to_char(
        "�H�����X�Хu�� sentinel scavenger aggressive stayarea wimpy\n\r"
        "               train rebirth fight ask noreborn nokill nosummon\n\r"
        "               enroll good evil speak\n\r"
        , ch );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "strength" ) )
  {
    if ( value < 0 || value > MaxStr )
    {
      act( "�u����ܤO�q�� 0 �� $x�C" , ch, &MaxStr, NULL, TO_CHAR );
    }

    else
    {
      victim->now_str = value;
      act( "�A�]�w$N�O�q�� $x�C", ch, &value, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "intelligence" ) )
  {

    if ( value < 0 || value > MaxInt )
    {
      act( "�u�഼�O�� 0 �� $x�C", ch , &MaxInt, NULL, TO_CHAR );
    }

    else
    {
      victim->now_int = value;
      act( "�A�]�w$N���O�� $x�C", ch, &value, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "wisdom" ) )
  {
    if ( value < 0 || value > MaxWis )
    {
      act( "�u����ܾ��ѥ� 0 �� $x�C", ch , &MaxWis, NULL, TO_CHAR );
    }

    else
    {
      victim->now_wis = value;
      act( "�A�]�w$N���Ѭ� $x�C", ch, &value, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "dexterity" ) )
  {
    if ( value < 0 || value > MaxDex )
    {
      act( "�u����ܱӱ��� 0 �� $x�C" , ch, &MaxDex, NULL, TO_CHAR );
    }

    else
    {
      victim->now_dex = value;
      act( "�A�]�w$N�ӱ��� $x�C", ch, &value, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "constitutin" ) )
  {
    if ( value < 0 || value > MaxCon )
    {
      act( "�u��������� 0 �� $x�C" , ch, &MaxCon, NULL, TO_CHAR );
    }

    else
    {
      victim->now_con = value;
      act( "�A�]�w$N��欰 $x�C", ch, &value, victim, TO_CHAR );
    }

   RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "sex" ) )
  {
         if ( !str_prefix( arg3, "male"   ) ) value = SEX_MALE;
    else if ( !str_prefix( arg3, "female" ) ) value = SEX_FEMALE;

    switch( value )
    {
    default:
      print_to_char( ch, "�ʧO�u��%s(%d)�M%s(%d)�C\n\r"
        , sex_name( SEX_MALE )  , SEX_MALE
        , sex_name( SEX_FEMALE ), SEX_FEMALE );
      break;

    case SEX_FEMALE:
      victim->sex = SEX_FEMALE;
      act( "�A��$N�ʧO�令$t�F�C", ch, sex_name( victim->sex ), victim, TO_CHAR );
      break;

    case SEX_MALE:
      victim->sex = SEX_MALE;
      act( "�A��$N�ʧO�令$t�F�C", ch, sex_name( victim->sex ), victim, TO_CHAR );
      break;
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "class" ) )
  {
    if ( !( pClass = is_class( arg3, TRUE ) ) )
    {
      send_to_char( "¾�~�W�٤��X�k�C\n\r" , ch );
      RETURN_NULL();
    }

    victim->class = pClass;
    act( "�A��$N¾�~�令$t�F�C", ch , class_name( pClass, TRUE ), victim, TO_CHAR );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "level" ) )
  {
    if ( !IS_NPC( victim ) )
    {
      act( "����缾�a$N���o�Ӱʧ@�C", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( value < 0 || value >= LEVEL_HERO )
    {
      chinese_number( LEVEL_HERO - 1, buf );
      act( "���Žd��� 0 �� $t�C", ch ,buf, NULL, TO_CHAR );
    }

    else
    {
      victim->level = value;
      act( "�A�]�w$N���Ŭ�$i�C", ch, &value, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "gold" ) )
  {
    if ( value < 0 )
    {
      send_to_char( "������ܦ��t�ȡC\n\r" , ch );
    }
    else
    {
      victim->gold = value;
      act( "�A�]�w$N������ $i ������C", ch, &value, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "steal" ) )
  {
    if ( IS_NPC( victim ) || !victim->pcdata )
    {
      act( "�����D���a$N���o�Ӱʧ@�C", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( value < 0 )
    {
      send_to_char( "������ܦ��t�ȡC\n\r" , ch );
    }

    else
    {
      victim->pcdata->steal = value;
      act( "�A�]�w$N���Ѧ��Ƭ� $i ���C", ch, &value, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "dosage" ) )
  {
    if ( IS_NPC( victim ) || !victim->pcdata )
    {
      act( "�����D���a$N���o�Ӱʧ@�C", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( value < 0 )
    {
      send_to_char( "������ܦ��t�ȡC\n\r" , ch );
    }

    else
    {
      victim->pcdata->dosage = value;
      act( "�A�]�w$N�ľ���$i�C", ch, &value, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "donate" ) )
  {
    if ( IS_NPC( victim ) )
    {
      act( "�����D���a$N���o�Ӱʧ@�C", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( value < 0 )
    {
      send_to_char( "������ܦ��t�ȡC\n\r" , ch );
    }

    else
    {
      victim->donate = value;
      act( "�A�]�w$N�������ٮɶ���$i�C", ch, &value, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "murder" ) )
  {
    if ( IS_NPC( victim ) || !victim->pcdata )
    {
      act( "�����D���a$N���o�Ӱʧ@�C", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( value < 0 )
    {
      send_to_char( "������ܦ��t�ȡC\n\r" , ch );
    }
    else
    {
      victim->pcdata->murder = value;
      act( "�A�]�w$N�ѱ����Ƭ�$i���C", ch, &value, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "bank" ) )
  {
    if ( IS_NPC( victim ) )
    {
      act( "�����D���a$N���o�Ӱʧ@�C", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( value < 0 )
    {
      send_to_char( "������ܦ��t�ȡC\n\r" , ch );
    }

    else
    {
      victim->bank = value;
      act( "�A�]�w$N�Ȧ�s�ڬ�$i��Ȥl�C", ch, &value, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "firman" ) )
  {
    if ( IS_NPC( victim ) )
    {
      act( "�����D���a$N���o�Ӱʧ@�C", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( value < 0 )
    {
         send_to_char( "������ܦ��t�ȡC\n\r" , ch );
    }

    else if ( value > MaxFirman )
    {
      send_to_char( "�ƭȤӤj�F�C\n\r", ch );
    }

    else
    {
      victim->firman = value;
      act( "�A�]�w$N�K�����P�Ƭ�$i�i�C", ch, &value, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "nskill" ) )
  {
    if ( IS_NPC( victim ) )
    {
      act( "�����D���a$N���o�Ӱʧ@�C", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( value < 0 )
    {
      send_to_char( "������ܦ��t�ȡC\n\r" , ch );
    }

    else if ( value > MaxNSkill )
    {
      send_to_char( "�ƭȤӤj�F�C\n\r", ch );
    }

    else
    {
      victim->nskill = value;
      act( "�A�]�w$N�B�~�ޯ�Ƭ�$i�C", ch, &value, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "rechristen" ) )
  {
    if ( IS_NPC( victim ) || !victim->pcdata )
    {
      act( "�����D���a$N���o�Ӱʧ@�C", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( value < 0 )
    {
      send_to_char( "������ܦ��t�ȡC\n\r" , ch );
    }
    else
    {
      victim->pcdata->rechristen = value;
      act( "�A�]�w$N�襤��W�٦��Ƭ�$i���C", ch, &value, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "xname" ) )
  {
    if ( IS_NPC( victim ) || !victim->pcdata )
    {
      act( "�����D���a$N���o�Ӱʧ@�C", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( value < 0 )
    {
      send_to_char( "������ܦ��t�ȡC\n\r" , ch );
    }

    else
    {
      victim->pcdata->xname = value;
      act( "�A�]�w$N�K�����r���Ƭ�$i���C", ch, &value, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "hp" ) )
  {
    if ( value < 0 || value > MaxHit )
    {
      act( "�ͩR�O�d��� 0 �� $x�C", ch, &MaxHit, NULL, TO_CHAR );
    }

    else
    {
      victim->max_hit = value;
      act( "�A�]�w$N�ͩR�O�� $x�C", ch, &value, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "mana" ) )
  {
    if ( value < 0 || value > MaxMana )
    {
      act( "�k�O�d��� 0 �� $x�C", ch, &MaxMana, NULL, TO_CHAR );
    }

    else
    {
      victim->max_mana = value;
      act( "�A�]�w$N�k�O�� $x�C", ch, &value, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "move" ) )
  {
    if ( value < 0 || value > MaxMove )
    {
      act( "���ʤO�d��� 0 �� $x�C", ch, &MaxMove, NULL, TO_CHAR );
    }

    else
    {
      victim->max_move = value;
      act( "�A�]�w$N���ʤO�� $x�C", ch, &value, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "limit" ) )
  {
    if ( value < 0 || value > MAX_LIMIT_VALUE  )
    {
      chinese_number( MAX_LIMIT_VALUE, buf );
      act( "���ʤO�d��� 0 �� $t�C", ch, buf, NULL, TO_CHAR );
    }

    else
    {
      victim->limit = value;
      act( "�A�]�w$N�����ޭȬ� $x�C", ch, &value, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "practice" ) )
  {
    if ( value < 0 || value > MaxPractice )
    {
      act( "�ǲ��I�ƽd��� 0 �� $x �I�C", ch, &MaxPractice, NULL, TO_CHAR );
    }

    else
    {
      victim->practice = value;
      act( "�A�]�w$N�ǲ��I�Ƭ� $x�C", ch, &value, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "alignment" ) )
  {
    if ( value < -AlignRange || value > AlignRange )
    {
      chinese_number( -AlignRange, buf  );
      chinese_number(  AlignRange, buf1 );

      act( "�}��d��� $t �� $T�C", ch, buf, buf1, TO_CHAR );
    }

    else
    {
      victim->alignment = value;
      act( "�A�]�w$N�}�笰 $x�C", ch, &value, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "thirst" ) )
  {
    if ( IS_NPC( victim ) || !victim->pcdata )
    {
      act( "�����D���a$N���o�Ӱʧ@�C", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( value < 0 || value > 100 )
    {
      send_to_char( "�f���{�׽d��� 0 �� 100�C\n\r", ch );
    }

    else
    {
      victim->pcdata->condition[COND_THIRST] = value;
      act( "�A�]�w$N�f���{�׬� $x�C", ch, &value, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "drunk" ) )
  {
    if ( IS_NPC( victim ) || !victim->pcdata )
    {
      act( "�����D���a$N���o�Ӱʧ@�C", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( value < 0 || value > 100 )
    {
      send_to_char( "�s�K�{�׽d��� 0 �� 100�C\n\r", ch );
    }

    else
    {
      victim->pcdata->condition[COND_DRUNK] = value;
      act( "�A�]�w$N�s�K�{�׬� $x�C", ch, &value, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "full" ) )
  {
    if ( IS_NPC( victim ) || !victim->pcdata )
    {
      act( "�����D���a$N���o�Ӱʧ@�C", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( value < 0 || value > 100 )
    {
      send_to_char( "�{�l�j�{�׽d��� 0 �� 100�C\n\r", ch );
    }

    else
    {
      victim->pcdata->condition[COND_FULL] = value;
      act( "�]�w$N�{�l�j�{�׬� $x�C", ch, &value, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "name" ) )
  {
    if ( !IS_NPC( victim ) )
    {
      act( "�����D���a$N���o�Ӱʧ@�C", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    free_string( victim->name );
    victim->name = str_dup( arg3 );
    act( "�]�w$N�W�٬� $t�C", ch, arg3, victim, TO_CHAR );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "short" ) )
  {
    if ( !IS_NPC( victim ) )
    {
      act( "�����D���a$N���o�Ӱʧ@�C", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    free_string( victim->byname );
    victim->byname = str_dup( arg3 );
    act( "�]�w$N�u�ԭz�� $t�C", ch, arg3, victim, TO_CHAR );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "long" ) )
  {
    if ( !IS_NPC( victim ) )
    {
      act( "�����D���a$N���o�Ӱʧ@�C", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    free_string( victim->long_descr );
    victim->long_descr = str_dup( arg3 );
    act( "�]�w$N���ԭz�� $t�C", ch, arg3, victim, TO_CHAR );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "title" ) )
  {
    if ( IS_NPC( victim ) )
    {
      act( "�����D���a$N���o�Ӱʧ@�C", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    set_title( victim, arg3 );
    act( "�]�w$N���Y�� $t�C", ch, arg3, victim, TO_CHAR );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "migrate" ) )
  {
    if ( !IS_NPC( victim ) || !victim->pIndexData )
    {
      act( "����缾�a$N���o�Ӱʧ@�C", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( value < 0 || value > 100 )
    {
      send_to_char( "�E�����v�u��� 0 �� 100�C\n\r", ch );
    }
    else
    {
      victim->migrate = value;
      act( "�A�]�w$N�E���v���ʤ��� $i�C", ch, &value, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "tractable" ) )
  {
    if ( !IS_NPC( victim ) || !victim->pIndexData )
    {
      act( "����缾�a$N���o�Ӱʧ@�C", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( value < 0 || value > 100 )
    {
      send_to_char( "���A���v�u��� 0 �� 100�C\n\r", ch );
    }
    else
    {
      victim->tractable = value;
      act( "�A�]�w$N���A�v���ʤ��� $i�C", ch, &value, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  /* �p�G������, �h�X�{���~�T�� */
  do_mset( ch, "" );
  RETURN_NULL();
}

FUNCTION( do_oset )
{
  char         arg1[MAX_INPUT_LENGTH];
  char         arg2[MAX_INPUT_LENGTH];
  char         arg3[MAX_INPUT_LENGTH];
  char         buf[MAX_STRING_LENGTH];
  OBJ_DATA   * obj;
  SITUS_DATA * pSitus;
  SITUS_DATA * aSitus;
  int          correct;
  int          value;

  PUSH_FUNCTION( "do_oset" );

  smash_tilde( argument );
  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );
  str_cpy( arg3, argument );

  if ( !arg1[0] || !arg2[0] || !arg3[0] )
  {
    send_to_char(
      "�y�k oset <���~> <����> <�r��>\n\r"
      "�άO oset <���~> <�r��> <�r��>\n\r\n\r"
      "���إi�H���U�C��ܡR\n\r"
      "  v0 v1 v2 v3 .... v14\n\r"
      "  level weight cost timer\n\r"
      "�r��i�H���U�C��ܡR\n\r"
      "  name short long ed wear flags\n\r", ch );
    RETURN_NULL();
  }

  if ( !( obj = get_obj_world( ch, arg1 ) ) )
  {
    send_to_char( "�䤣��A�ҭn���ܪ����~�C\n\r", ch );
    RETURN_NULL();
  }

  /* Ū�J�ƭ�, ���]���@�w�ݭn�O�ƭ�. */
  value = atoi( arg3 );

  if ( arg2[0] == 'v' && is_number( arg2 + 1 ) )
  {
    int  count;
    char cNumber[MAX_INPUT_LENGTH];

    if ( ( count = atoi( arg2 + 1 ) ) < 0 || count >= MAX_OBJECT_VALUE )
    {
      chinese_number( MAX_OBJECT_VALUE, buf );
      act( "�ƭȥu��� 0 �� $t�C", ch, buf, NULL, TO_CHAR );
      RETURN_NULL();
    }

    chinese_number( count, cNumber );

    str_cpy( buf, check_obj_value( obj->item_type, count, value, &correct, TRUE ) );

    if ( buf[0] )
    {
      act( "���$p�ƭȿ��~�M��]�R$T", ch, obj, buf, TO_CHAR );
      RETURN_NULL();
    }

    obj->value[count] = value;
    sprintf( buf, "�A�]�w$p���ƭ�%s��%d�C", cNumber, value );
    act( smash_act_keyword( buf, "nesp" ), ch, obj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "wear" ) )
  {
    if ( obj->wear_loc != WEAR_NONE )
    {
      act( "$p�ٳQ��b���W�C�M�ҥH�L�k���C", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    for ( pSitus = situs_list; pSitus; pSitus = pSitus->next )
    {
      if ( !str_cmp( arg3, pSitus->name ) )
      {
        if ( CAN_WEAR( obj, pSitus->wear ) )
        {
          REMOVE_BIT( obj->wear_flags, pSitus->wear );
          act( "�A����$p��$T���X�СT", ch, obj, pSitus->situs_name, TO_CHAR );
          RETURN_NULL();
        }
        else
        {
          /* ��������L���X�� */
          for ( aSitus = situs_list; aSitus; aSitus = aSitus->next )
            REMOVE_BIT( obj->wear_flags, aSitus->wear );

          SET_BIT( obj->wear_flags, pSitus->wear );
          act( "�A�]�w$p��$T���X�СT", ch, obj, pSitus->situs_name, TO_CHAR );
          RETURN_NULL();
        }
      }
    }

    clear_buffer();
    send_to_buffer( "���~�����X�Хu���p�U�R\n\r" );

    for ( value = 1, pSitus = situs_list; pSitus; pSitus = pSitus->next )
    {
      send_to_buffer( "%2d) %-12s %s\n\r"
        , value++, pSitus->name, pSitus->situs_name );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "flags" ) )
  {
    if ( !str_prefix( arg3, "glow" ) )
    {
      if ( obj->Glow )
      {
        obj->Glow = FALSE;
        act( "$p���A�{ģ�ʤH�T", ch, obj, NULL, TO_CHAR );
      }
      else
      {
        obj->Glow = TRUE;
        act( "$p�S��_�{ģ�ʤH�T", ch, obj, NULL, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "hum" ) )
    {
      if ( obj->Hum )
      {
        obj->Hum = FALSE;
        act( "$p���A���@�T�T", ch, obj, NULL, TO_CHAR );
      }
      else
      {
        obj->Hum = TRUE;
        act( "$p�S��_���@�T�T", ch, obj, NULL, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "dark" ) )
    {
      if ( obj->Dark )
      {
        obj->Dark = FALSE;
        act( "$p���A�·t�L���T", ch, obj, NULL, TO_CHAR );
      }
      else
      {
        obj->Dark = TRUE;
        act( "$p�S��_�·t�L���T", ch, obj, NULL, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "evil" ) )
    {
      if ( obj->Evil )
      {
        obj->Evil = FALSE;
        act( "$p���A���c�T", ch, obj, NULL, TO_CHAR );
      }
      else
      {
        obj->Evil = TRUE;
        act( "$p�S��_���c�T", ch, obj, NULL, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "invis" ) )
    {
      if ( obj->Invis )
      {
        obj->Invis = FALSE;
        act( "$p���A���ΡT", ch, obj, NULL, TO_CHAR );
      }
      else
      {
        obj->Invis = TRUE;
        act( "$p�S��_���ΡT", ch, obj, NULL, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "magic" ) )
    {
      if ( obj->Magic )
      {
        obj->Magic = FALSE;
        act( "$p���A���]�k�ʽ�T", ch, obj, NULL, TO_CHAR );
      }
      else
      {
        obj->Magic = TRUE;
        act( "$p�S��_�]�k�ʽ�T", ch, obj, NULL, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "nodrop" ) )
    {
      if ( obj->NoDrop )
      {
        obj->NoDrop = FALSE;
        act( "$p���A���i���T", ch, obj, NULL, TO_CHAR );
      }
      else
      {
        obj->NoDrop = TRUE;
        act( "$p�S��_���i���ʽ�T", ch, obj, NULL, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "bless" ) )
    {
      if ( obj->Bless )
      {
        obj->Bless = FALSE;
        act( "$p���A�㯬�֩ʽ�T", ch, obj, NULL, TO_CHAR );
      }
      else
      {
        obj->Bless = TRUE;
        act( "$p�S��_���֩ʽ�T", ch, obj, NULL, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "antigood" ) )
    {
      if ( obj->AntiGood )
      {
        obj->AntiGood = FALSE;
        act( "$p���A��ܵ��}�ʽ�T", ch, obj, NULL, TO_CHAR );
      }
      else
      {
        obj->AntiGood = TRUE;
        act( "$p�S��_�ܵ��}�ʽ�T", ch, obj, NULL, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "antievil" ) )
    {
      if ( obj->AntiEvil )
      {
        obj->AntiEvil = FALSE;
        act( "$p���A��ܨ��c�ʽ�T", ch, obj, NULL, TO_CHAR );
      }
      else
      {
        obj->AntiEvil = TRUE;
        act( "$p�S��_�ܨ��c�ʽ�T", ch, obj, NULL, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "antineutral" ) )
    {
      if ( obj->AntiNeutral )
      {
        obj->AntiNeutral = FALSE;
        act( "$p���A��ܤ��ʩʽ�T", ch, obj, NULL, TO_CHAR );
      }
      else
      {
        obj->AntiNeutral = TRUE;
        act( "$p�S��_�ܤ��ʩʽ�T", ch, obj, NULL, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "noremove" ) )
    {
      if ( obj->NoRemove )
      {
        obj->NoRemove = FALSE;
        act( "$p���A���i�����T", ch, obj, NULL, TO_CHAR );
      }
      else
      {
        obj->NoRemove = TRUE;
        act( "$p�S��_���i�����ʽ�T", ch, obj, NULL, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "inventory" ) )
    {
      if ( obj->Inventory )
      {
        obj->Inventory = FALSE;
        act( "$p���A����(inventory)�T", ch, obj, NULL, TO_CHAR );
      }
      else
      {
        obj->Inventory = TRUE;
        act( "$p�S��_����(inventory)�ʽ�T", ch, obj, NULL, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "nosave" ) )
    {
      if ( obj->NoSave )
      {
        obj->NoSave = FALSE;
        act( "$p���A���i�x�s�T", ch, obj, NULL, TO_CHAR );
      }
      else
      {
        obj->NoSave = TRUE;
        act( "$p�S��_���i�x�s�ʽ�T", ch, obj, NULL, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "contraband" ) )
    {
      if ( obj->Contraband )
      {
        obj->Contraband = FALSE;
        act( "$p���A�㦳�H�T�~�ʽ�T", ch, obj, NULL, TO_CHAR );
      }
      else
      {
        obj->Contraband = TRUE;
        act( "$p�S��_�H�T�~�ʽ�T", ch, obj, NULL, TO_CHAR );
      }
    }

    else
    {
      send_to_char(
        "���~���X�Хu�� glow hum dark evil invis magic nodrop\n\r"
        "               bless antigood antievil antineutral noremove\n\r"
        "               nosave contraband\n\r", ch );
    }
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "armor" ) )
  {
    obj->armor = value;
    act( "�A�]�w$p���ثe���@�� $X�C", ch, obj, &value, TO_CHAR );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "maxarmor" ) )
  {
    obj->max_armor = value;
    act( "�A�]�w$p���̤j���@�� $X�C", ch, obj, &value, TO_CHAR );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "level" ) )
  {
    obj->level = value;
    act( "�A�]�w$p�����Ŭ� $X�C", ch, obj, &value, TO_CHAR );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "weight" ) )
  {
    obj->weight = value;

    act( "�A�]�w$p�����q�� $X�C", ch, obj, &value, TO_CHAR );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "cost" ) )
  {
    obj->cost = value;
    act( "�]�w$p�����Ȭ� $X�C", ch, obj, &value, TO_CHAR );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "timer" ) )
  {
    obj->timer = value;
    act( "�A�]�w$p���ɮĬ� $X�C", ch, obj, &value, TO_CHAR );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "name" ) )
  {
    smash_tilde( arg3 );
    act( "�A�]�w$p���W�٬� $T�C", ch, obj, arg3, TO_CHAR );
    free_string( obj->name );
    obj->name = str_dup( arg3 );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "short" ) )
  {
    smash_tilde( arg3 );
    act( "�]�w$p���u�ԭz�� $T�C", ch, obj, arg3, TO_CHAR );
    free_string( obj->cname );
    obj->cname = str_dup( arg3 );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "long" ) )
  {
    smash_tilde( arg3 );
    act( "�]�w$p�����ԭz�� $T�C", ch, obj, arg3, TO_CHAR );
    free_string( obj->description );
    obj->description = str_dup( arg3 );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "ed" ) )
  {
    EXTRA_DESCR_DATA * ed;

    argument = one_argument( argument, arg3 );

    if ( !argument )
    {
      send_to_char( "�y�k�Roset <���~> ed <����r> <�r��>�C\n\r" , ch );
      RETURN_NULL();
    }

    /* �t�m�O���� */
    ed = alloc_struct( STRUCT_EXTRA_DESCR_DATA );

    smash_tilde( arg3     );
    smash_tilde( argument );

    ed->keyword      = str_dup( arg3     );
    ed->description  = str_dup( argument );
    ed->next         = obj->extra_descr;
    obj->extra_descr = ed;

    sprintf( buf, "%s %s", arg3, argument );
    act( "�A�]�w$p���B�~�y�z�� $T�C", ch, obj, buf, TO_CHAR );
    RETURN_NULL();
  }

  /* �p�G�ﶵ���~, ��X���~���e�� */
  do_oset( ch, "" );
  RETURN_NULL();
}

FUNCTION( do_rset )
{
  ROOM_INDEX_DATA * location;
  char              arg1[MAX_INPUT_LENGTH];
  char              arg2[MAX_INPUT_LENGTH];
  char              arg3[MAX_INPUT_LENGTH];
  int               tick;

  PUSH_FUNCTION( "do_rset" );

  smash_tilde( argument );
  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );
  str_cpy( arg3, argument );

  if ( arg1[0] == '\x0' || arg2[0] == '\x0' )
  {
    send_to_char( "�y�k�Rrset <��m> <����> <�ƭ�>�C\n\r\n\r"
                  "���إi�H�O�U�C��ܡR\n\r "
                  "writeable dark nomob indoors private safe\n\r"
                  "foreverlight norecall noquit nofight stock\n\r"
                  "depositmoney storeroom memorize nowhere fane\n\r"
                  "killer standard waterful foodful manacure\n\r"
                  "movecure hitcure mobreborn practice\n\r", ch );
    RETURN_NULL();
  }

  if ( !( location = find_location( ch, arg1 ) ) )
  {
    send_to_char( "�䤣��A�n���ܪ���m�C\n\r", ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "writeable" ) )
  {
    if ( location->writeable )
    {
      location->writeable = FALSE;
      send_to_char( "�A��o���ж��i�g�J�X�вM���T\n\r", ch );
    }

    else
    {
      location->writeable = TRUE;
      send_to_char( "�A�]�w�o���ж��i�g�J�X�СT\n\r", ch );
    }

    if ( location->player ) write_new_room( location );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "dark" ) )
  {
    if ( location->Dark )
    {
      location->Dark = FALSE;
      send_to_char( "�A��o���ж��·t�X�вM���T\n\r", ch );
    }

    else
    {
      location->Dark = TRUE;
      send_to_char( "�A�]�w�o���ж��·t�X�СT\n\r", ch );
    }

    if ( location->player ) write_new_room( location );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "nomob" ) )
  {
    if ( location->NoMob )
    {
      location->NoMob = FALSE;
      send_to_char( "�A��o���ж��L�ͪ��X�вM���T\n\r", ch );
    }

    else
    {
      location->NoMob = TRUE;
      send_to_char( "�A�]�w�o���ж��L�ͪ��X�СT\n\r", ch );
    }

    if ( location->player ) write_new_room( location );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "waterful" ) )
  {
    if ( location->Waterful )
    {
      location->Waterful = FALSE;
      send_to_char( "�A��o���ж��h���X�вM���T\n\r", ch );
    }

    else
    {
      location->Waterful = TRUE;
      send_to_char( "�A�]�w�o���ж��h���X�СT\n\r", ch );
    }

    if ( location->player ) write_new_room( location );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "manacure" ) )
  {
    if ( location->ManaCure )
    {
      location->ManaCure = FALSE;
      send_to_char( "�A��o���ж��k�O�E���ҺX�вM���T\n\r", ch );
    }

    else
    {
      location->ManaCure = TRUE;
      send_to_char( "�A�]�w�o���ж��k�O�E���ҺX�СT\n\r", ch );
    }

    if ( location->player ) write_new_room( location );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "movecure" ) )
  {
    if ( location->MoveCure )
    {
      location->MoveCure = FALSE;
      send_to_char( "�A��o���ж���O�E���ҺX�вM���T\n\r", ch );
    }

    else
    {
      location->MoveCure = TRUE;
      send_to_char( "�A�]�w�o���ж���O�E���ҺX�СT\n\r", ch );
    }

    if ( location->player ) write_new_room( location );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "hitcure" ) )
  {
    if ( location->HitCure )
    {
      location->HitCure = FALSE;
      send_to_char( "�A��o���ж��ͩR�O�E���ҺX�вM���T\n\r", ch );
    }

    else
    {
      location->HitCure = TRUE;
      send_to_char( "�A�]�w�o���ж��ͩR�O�E���ҺX�СT\n\r", ch );
    }

    if ( location->player ) write_new_room( location );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "practice" ) )
  {
    if ( location->PracRoom )
    {
      location->PracRoom = FALSE;
      send_to_char( "�A��o���ж��m�\\���X�вM���T\n\r", ch );
    }

    else
    {
      location->PracRoom = TRUE;
      send_to_char( "�A�]�w�o���ж��m�\\���X�СT\n\r", ch );
    }

    if ( location->player ) write_new_room( location );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "foodful" ) )
  {
    if ( location->Foodful )
    {
      location->Foodful = FALSE;
      send_to_char( "�A��o���ж��h�����X�вM���T\n\r", ch );
    }

    else
    {
      location->Foodful = TRUE;
      send_to_char( "�A�]�w�o���ж��h�����X�СT\n\r", ch );
    }

    if ( location->player ) write_new_room( location );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "indoors" ) )
  {
    if ( location->Indoors )
    {
      location->Indoors = FALSE;
      send_to_char( "�A��o���ж��Ǥ����X�вM���T\n\r", ch );
    }

    else
    {
      location->Indoors = TRUE;
      send_to_char( "�A�]�w�o���ж��Ǥ��X�СT\n\r", ch );
    }

    if ( location->player ) write_new_room( location );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "private" ) )
  {
    if ( location->Private )
    {
      location->Private = FALSE;
      send_to_char( "�A��o���ж��p�H�X�вM���T\n\r", ch );
    }

    else
    {
      location->Private = TRUE;
      send_to_char( "�A�]�w�o���ж��p�H�X�СT\n\r", ch );
    }

    if ( location->player ) write_new_room( location );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "safe" ) )
  {
    if ( location->Safe )
    {
      location->Safe = FALSE;
      send_to_char( "�A��o���ж��w���X�вM���T\n\r", ch );
    }

    else
    {
      location->Safe = TRUE;
      send_to_char( "�A�]�w�o���ж��w���X�СT\n\r", ch );
    }

    if ( location->player ) write_new_room( location );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "foreverlight" ) )
  {
    if ( location->ForeverLight )
    {
      location->ForeverLight = FALSE;
      send_to_char( "�A��o���ж��é��X�вM���T\n\r", ch );
    }

    else
    {
      location->ForeverLight = TRUE;
      send_to_char( "�A�]�w�o���ж��é��X�СT\n\r", ch );
    }

    if ( location->player ) write_new_room( location );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "norecall" ) )
  {
    if ( location->NoRecall )
    {
      location->NoRecall = FALSE;
      send_to_char( "�A��o���ж�����ǰe�X�вM���T\n\r", ch );
    }

    else
    {
      location->NoRecall = TRUE;
      send_to_char( "�A�]�w�o���ж�����ǰe�X�СT\n\r", ch );
    }

    if ( location->player ) write_new_room( location );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "depositmoney" ) )
  {
    if ( location->DepositMoney )
    {
      location->DepositMoney = FALSE;
      send_to_char( "�A��o���ж������X�вM���T\n\r", ch );
    }

    else
    {
      location->DepositMoney = TRUE;
      send_to_char( "�A�]�w�o���ж������X�СT\n\r", ch );
    }

    if ( location->player ) write_new_room( location );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "storeroom" ) )
  {
    if ( location->StoreRoom )
    {
      location->StoreRoom = FALSE;
      send_to_char( "�A��o���ж��ߧ��X�вM���T\n\r", ch );
    }

    else
    {
      location->StoreRoom = TRUE;
      send_to_char( "�A�]�w�o���ж��ߧ��X�СT\n\r", ch );
    }

    if ( location->player ) write_new_room( location );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "nofight" ) )
  {
    if ( location->NoFight )
    {
      location->NoFight = FALSE;
      send_to_char( "�A��o���ж�����԰��X�вM���T\n\r", ch );
    }

    else
    {
      location->NoFight = TRUE;
      send_to_char( "�A�]�w�o���ж�����԰��X�СT\n\r", ch );
    }

    if ( location->player ) write_new_room( location );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "noquit" ) )
  {
    if ( location->NoQuit )
    {
      location->NoQuit = FALSE;
      send_to_char( "�A��o���ж��������}�X�вM���T\n\r", ch );
    }

    else
    {
      location->NoQuit = TRUE;
      send_to_char( "�A�]�w�o���ж��������}�X�СT\n\r", ch );
    }

    if ( location->player ) write_new_room( location );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "memorize" ) )
  {
    if ( location->Memorize )
    {
      location->Memorize = FALSE;
      send_to_char( "�A��o���ж��O���I�X�вM���T\n\r", ch );
    }

    else
    {
      location->Memorize = TRUE;
      send_to_char( "�A�]�w�o���ж��O���I�X�СT\n\r", ch );
    }

    if ( location->player ) write_new_room( location );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "stock" ) )
  {
    if ( location->Stock )
    {
      location->Stock = FALSE;
      send_to_char( "�A��o���ж����کҺX�вM���T\n\r", ch );
    }

    else
    {
      location->Stock = TRUE;
      send_to_char( "�A�]�w�o���ж����کҺX�СT\n\r", ch );
    }

    if ( location->player ) write_new_room( location );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "nowhere" ) )
  {
    if ( location->NoWhere )
    {
      location->NoWhere = FALSE;
      send_to_char( "�A��o���ж����ॴť�X�вM���T\n\r", ch );
    }

    else
    {
      location->NoWhere = TRUE;
      send_to_char( "�A�]�w�o���ж����ॴť�X�СT\n\r", ch );
    }

    if ( location->player ) write_new_room( location );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "fane" ) )
  {
    if ( location->Fane )
    {
      location->Fane = FALSE;
      send_to_char( "�A��o���ж��x�q�X�вM���T\n\r", ch );
    }

    else
    {
      location->Fane = TRUE;
      send_to_char( "�A�]�w�o���ж��x�q�X�СT\n\r", ch );
    }

    if ( location->player ) write_new_room( location );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "killer" ) )
  {
    if ( location->Killer )
    {
      location->Killer = FALSE;
      send_to_char( "�A��o���ж��K�P�����X�вM���T\n\r", ch );
    }

    else
    {
      location->Killer = TRUE;
      send_to_char( "�A�]�w�o���ж��K�P�����X�СT\n\r", ch );
    }

    if ( location->player ) write_new_room( location );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "standard" ) )
  {
    if ( location->Standard )
    {
      location->Standard = FALSE;
      send_to_char( "�A��o���ж��зǺX�вM���T\n\r", ch );
    }

    else
    {
      location->Standard = TRUE;
      send_to_char( "�A�]�w�o���ж��зǺX�СT\n\r", ch );
    }

    if ( location->player ) write_new_room( location );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "mobreborn" ) )
  {
    if ( arg3[0] == '\x0' || !is_number( arg3 ) )
    {
      send_to_char( "�A�n��o���ж��Ǫ����ͮɶ��אּ�h�֡S\n\r", ch );
      RETURN_NULL();
    }

    if ( ( tick = atoi( arg3 ) ) < 0 )
    {
      send_to_char( "�A��J���ɶ������j��s�T\n\r", ch );
      RETURN_NULL();
    }

    location->mob_reborn = tick;
    act( "�A��$t���Ǫ����ͮɶ��אּ$X�T", ch, location->name, &tick, TO_CHAR );
    RETURN_NULL();
  }

  /* �p�G�ﶵ���~, ��X�D�U�e�� */
  do_rset( ch, "" );

  RETURN_NULL();
}

FUNCTION( do_users )
{
  DESCRIPTOR_DATA * d;
  DESCRIPTOR_DATA * man = NULL;
  CHAR_DATA       * victim;
  char              arg[MAX_INPUT_LENGTH];
  char              buf[MAX_STRING_LENGTH];
  int               count;
  int               port = ERRORCODE;

  PUSH_FUNCTION( "do_users" );

  argument = one_argument( argument, arg );

  if ( arg[0] )
  {
    if ( !str_prefix( arg, "!port" ) )
    {
      one_argument( argument, arg );

      if ( !arg[0] || !is_number( arg ) )
      {
        send_to_char( "�A�n�[�ݧA�@�ӳs���𪺪��p�S\n\r", ch );
        RETURN_NULL();
      }

      for ( port = atoi( arg ), count = 0; count < MAX_PORT; count++ )
        if ( MudPort[count] != ERRORCODE && MudPort[count] == port ) break;

      if ( count == MAX_PORT )
      {
        act( "$t�S�����ӳs���� $X�T", ch, mud_name, &port, TO_CHAR );
        RETURN_NULL();
      }
    }

    else
    {
      if ( !( victim = get_char_world( ch, arg ) ) )
      {
        send_to_char( "�A���ؼФ��b�o�̡C\n\r", ch );
        RETURN_NULL();
      }

      if ( IS_NPC( victim ) )
      {
        act( "�A���ؼ�$N�����O���a�~�i�H�C", ch, NULL, victim, TO_CHAR );
        RETURN_NULL();
      }

      if ( !( man = victim->desc ) )
      {
        act( "$N���u�M�S���y�z��C", ch, NULL, victim, TO_CHAR );
        RETURN_NULL();
      }
    }
  }

  send_to_char( "\e[1;33;44m[��{( �C�����A) ] ���d�ɶ� ���m �� �} �u "
    "�� �a �W ��  �s    �u    ��    �}    \e[0m\n\r" , ch );

  for ( count = 0, clear_buffer(), d = descriptor_list; d; d = d->next )
  {
    if ( !verify_desc( d )
      || ( port != ERRORCODE && d->port != port )
      || ( man && d != man ) )
      continue;

    if ( d->character
      && d->connected == CON_PLAYING
      && IS_SET( d->character->act, PLR_WIZINVIS )
      && get_trust( d->character ) > get_trust( ch ) ) continue;

    count++;
    str_cpy( buf, host_name( d ) );
    buf[25] = '\x0';

    send_to_buffer( "\e[1;37m[%3d(%10s) ] \e[1;33m%s\e[0m"
                    " \e[1;32m%4d\e[0m \e[1;37m%5d\e[0m \e[1;36m%s\e[0m "
                    "\e[1;31m%-12.12s\e[0m \e[1;31m%s\e[0m\n\r"
      , d->descriptor , player_status( d->connected )
      , showtime( current_time - d->timekeeper )
      , d->character ? d->character->timer : -1
      , d->port
      , CheckServer ? YESNO( d->server ) : "�S"
      , d->character ? d->character->name : "(����)"
      , buf );

    if ( buffer_full() ) break;
  }

  if ( !man || port != ERRORCODE )
  {
    send_to_buffer( "�`�@�� %d ��ϥΪ̦b%s�C\n\r", count, mud_name );
    for ( count = 0, victim = char_list; victim; victim = victim->next )
    {
      if ( verify_char( victim ) && !IS_NPC( victim ) && !victim->desc )
      {
        if ( count == 0 )
          send_to_buffer( "\n\r�t�~�ثe�����`�_�u�����a���R\n\r" );

        send_to_buffer( "%2d. �W�r�R%-12s %s\e[0m�u�_�u�`�ɶ��R%d�v\n\r"
         , ++count, victim->name, victim->cname, victim->timer );
      }
    }
  }

  print_buffer( ch );
  RETURN_NULL();
}

FUNCTION( do_force )
{
  CMD_DATA  * pCommand;
  CHAR_DATA * vch;
  char        arg[MAX_INPUT_LENGTH];
  int         trust;

  PUSH_FUNCTION( "do_force" );

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' || argument[0] == '\x0' )
  {
    send_to_char( "�A�Q�j��ְ�����ơS\n\r", ch );
    RETURN_NULL();
  }

  /* �b���O��椤��ŦX�����O, 200 ��ܬO�Ǫ������O */
  trust = get_trust( ch );
  for ( pCommand = cmd_list; pCommand; pCommand = pCommand->next )
  {
    if ( !str_prefix( argument , pCommand->name )
      && ( pCommand->level > trust && pCommand->level != 200 ) )
    {
      send_to_char( "�o�ӫ��O�s�A�ۤv�����|���C�T\n\r", ch );
      RETURN_NULL();
    }
  }

  if ( !str_cmp( arg, "all" ) )
  {
    for ( vch = char_list; vch; vch = vch->next )
    {
      if ( !verify_char( vch ) ) continue;

      if ( !IS_NPC( vch ) && get_trust( vch ) < get_trust( ch ) )
      {
        MOBtrigger = FALSE;
        act( "$n�j��A $t�C", ch, argument, vch, TO_VICT );
        interpret( vch, argument );
      }
    }
  }
  else
  {
    if ( !( vch = get_char_world( ch, arg ) ) )
    {
      send_to_char( "�A����H���b�o�̡C\n\r", ch );
      RETURN_NULL();
    }

    if ( vch == ch )
    {
      send_to_char( "�j��ۤv�M���p���֦ۤv�h���a�T\n\r", ch );
      RETURN_NULL();
    }

    if ( get_trust( vch ) >= get_trust( ch ) )
    {
      act( "�A�����Ť�$N�C�M�ҥH���ѤF�T", ch, NULL, vch, TO_CHAR );
      RETURN_NULL();
    }

    MOBtrigger = FALSE;
    act( "$n�j��A $t�C", ch, argument, vch, TO_VICT );
    interpret( vch, argument );
  }

  send_to_char( "FORCE �ʧ@���槹���C\n\r", ch );
  RETURN_NULL();
}

FUNCTION( do_invis )
{
  PUSH_FUNCTION( "do_invis" );

  if ( IS_NPC(ch) ) RETURN_NULL();

  if ( IS_SET(ch->act, PLR_WIZINVIS) )
  {
    REMOVE_BIT( ch->act, PLR_WIZINVIS );
    act( "�b�@�}�j���]�򤧤U�M$n�v���{�X�L�����v�C", ch, NULL, NULL, TO_ROOM );
    send_to_char( "�b�@�}�j���]�򤧤U�M�A�v���{�X�A�����v�C\n\r", ch );
  }
  else
  {
    SET_BIT( ch->act, PLR_WIZINVIS );
    act( "�b�@������Ţ�n���U�M$n���v�v�������F�C" , ch, NULL, NULL, TO_ROOM );
    send_to_char( "�b�@������Ţ�n���U�M�A�����v�v�������F�C\n\r", ch );
  }

  RETURN_NULL();
}

FUNCTION( do_nodeath )
{
  PUSH_FUNCTION( "do_nodeath" );

  if ( IS_NPC(ch) ) RETURN_NULL();

  if ( IS_SET( ch->act, PLR_NODEATH ) )
  {
    REMOVE_BIT( ch->act, PLR_NODEATH );
    send_to_char( "�Fù���S�H�ͦb�A���ǤF�C\n\r", ch );
  }
  else
  {
    SET_BIT(ch->act, PLR_NODEATH );
    send_to_char( "�Fù�����A���h�F�C\n\r", ch );
  }

  RETURN_NULL();
}

FUNCTION( do_holylight )
{
  PUSH_FUNCTION( "do_holylight" );

  if ( IS_NPC(ch) ) RETURN_NULL();

  if ( IS_SET(ch->act, PLR_HOLYLIGHT) )
  {
    REMOVE_BIT(ch->act, PLR_HOLYLIGHT);
    send_to_char( "�A�����t���~�v�����������F�C\n\r", ch );
  }
  else
  {
    SET_BIT(ch->act, PLR_HOLYLIGHT);
    send_to_char( "�A�����Q���t���~��Ţ�n�ۡC\n\r", ch );
  }

  RETURN_NULL();
}

FUNCTION( do_owhere )
{
  char       arg[ MAX_INPUT_LENGTH ];
  OBJ_DATA * obj;
  int        counter;
  bool       fAll = FALSE;

  PUSH_FUNCTION( "do_owhere" );

  if ( IS_NPC( ch ) ) RETURN_NULL();
  one_argument( argument, arg );

  if ( !str_cmp( arg , "all" ) ) fAll = TRUE;
  clear_buffer();

  for ( counter = 1, obj = object_list; obj ; obj = obj->next )
  {
    if ( !verify_obj( obj ) ) continue;

    if ( (!is_name( arg,obj->name ) || !can_see_obj( ch ,obj )) && !fAll )
      continue;

    send_to_buffer( "[%3d] %s\e[0m %s\e[0m\n\r"
                    , counter++
                    , obj_name( ch, obj )
                    , object_locate( obj , ch ) );

   if ( buffer_full() ) break;
  }

  if ( counter == 1 ) send_to_buffer( "�䤣��A�n���F��C\n\r" );
  print_buffer( ch );
  RETURN_NULL();
}

FUNCTION( do_mlevel )
{
  MOB_INDEX_DATA * pMobIndex;
  extern int       top_mob_index;
  char             arg1[MAX_INPUT_LENGTH];
  char             arg2[MAX_INPUT_LENGTH];
  int              up;
  int              down;
  int              vnum;
  int              nMatch;
  bool             found;

  PUSH_FUNCTION( "do_mlevel" );

  argument = one_argument( argument, arg1 );
  one_argument( argument , arg2 );

  if ( arg1[0] == '\x0' || !is_number( arg1 ) )
  {
    send_to_char( "�y�k�Rmlevel <����> <����>�C\n\r" , ch );
    RETURN_NULL();
  }

  down = atoi( arg1 );

  if ( arg2[0] == '\x0')
  {
    up = down;
  }
  else
  {
    if ( !is_number( arg2 ) )
    {
      do_mlevel( ch , "" );
      RETURN_NULL();
    }
    else
    {
      up = atoi( arg2 );
    }
  }

  if ( down > up )
  {
    down = down - up ;
    up   = down + up ;
    down = up - down ;
  }

  clear_buffer();
  for ( found = FALSE, nMatch = vnum = 0; nMatch < top_mob_index; vnum++ )
  {
    if ( ( pMobIndex = get_mob_index( vnum ) ) )
    {
      nMatch++;
      if ( pMobIndex->level >= down && pMobIndex->level <= up )
      {
        found = TRUE;
        send_to_buffer( "\e[1;32m���X\e[0m%6d "
                        "\e[1;33m����\e[0m%4d "
                         "\e[1;34m�Q��\e[0m%4d "
                         "\e[1;36m�ƶq\e[0m%4d "
                         "\e[1;35m�y�z\e[0m %s\n\r",
          pMobIndex->vnum,
          pMobIndex->level,
          pMobIndex->killed,
          pMobIndex->count,
          mob_index_name( NULL, pMobIndex ) );

        if ( buffer_full() ) break;
      }
    }
  }

  if ( !found ) send_to_buffer( "�䤣��A���w����H�C\n\r" );
  print_buffer( ch );
  RETURN_NULL();
}

FUNCTION( do_olevel )
{
  OBJ_INDEX_DATA * pObjIndex;
  extern int       top_obj_index;
  char             arg1[MAX_INPUT_LENGTH];
  char             arg2[MAX_INPUT_LENGTH];
  int              up;
  int              down;
  int              vnum;
  int              nMatch;
  bool             found;

  PUSH_FUNCTION( "do_olevel" );

  argument = one_argument( argument, arg1 );
  one_argument( argument , arg2 );

  if ( arg1[0] == '\x0' || !is_number( arg1 ) )
  {
    send_to_char( "�y�k�Rolevel <����> <����>�C\n\r" , ch );
    RETURN_NULL();
  }

  down = atoi( arg1 );

  if ( arg2[0] == '\x0')
  {
    up = down;
  }

  else
  {
    if ( !is_number( arg2 ) )
    {
      do_olevel( ch , "" );
      RETURN_NULL();
    }
    else
    {
      up = atoi( arg2 );
    }
  }

  if ( down > up )
  {
    down = down - up ;
    up   = down + up ;
    down = up - down ;
  }

  clear_buffer();
  for ( found = FALSE, nMatch = vnum = 0; nMatch < top_obj_index; vnum++ )
  {
    if ( ( pObjIndex = get_obj_index( vnum ) ) )
    {
      nMatch++;
      if ( pObjIndex->level >= down && pObjIndex->level <= up )
      {
        found = TRUE;
        send_to_buffer( "\e[1;32m���X\e[0m%6d "
                        "\e[1;33m����\e[0m%4d "
                        "\e[1;34m�ƶq\e[0m%4d "
                        "\e[1;35m�y�z\e[0m %s(%s)\n\r",
          pObjIndex->vnum,
          pObjIndex->level,
          pObjIndex->count,
          pObjIndex->short_descr,
          pObjIndex->name );

        if ( buffer_full() ) break;
      }
    }
  }

  if ( !found ) send_to_buffer( "�䤣��A���w�����~�C\n\r" );
  print_buffer( ch );
  RETURN_NULL();
}

FUNCTION( do_oaf )
{
  OBJ_INDEX_DATA * pObjIndex;
  AFFECT_DATA    * paf;
  extern int       top_obj_index;
  char             arg[MAX_INPUT_LENGTH];
  int              vnum;
  int              nMatch;
  int              field;
  bool             found;

  PUSH_FUNCTION( "do_oaf" );

  one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    send_to_char( "�y�k�Roaf <�d�ߦr��>\n\r\n\r"
                  "<�d�ߦr��>�Rstr int wis dex con sex class level\n\r"
                  "            age height weight mana hp move gold\n\r"
                  "            exp ac hitroll damroll saving_para\n\r"
                  "            saving_rod saving_petri saving_breath\n\r"
                  "            saving_spell\n\r" , ch );

    RETURN_NULL();
  }

  field = 0;
  if ( !str_prefix( arg , "str"           ) ) field = APPLY_STR          ;
  if ( !str_prefix( arg , "int"           ) ) field = APPLY_INT          ;
  if ( !str_prefix( arg , "wis"           ) ) field = APPLY_WIS          ;
  if ( !str_prefix( arg , "dex"           ) ) field = APPLY_DEX          ;
  if ( !str_prefix( arg , "con"           ) ) field = APPLY_CON          ;
  if ( !str_prefix( arg , "sex"           ) ) field = APPLY_SEX          ;
  if ( !str_prefix( arg , "class"         ) ) field = APPLY_CLASS        ;
  if ( !str_prefix( arg , "level"         ) ) field = APPLY_LEVEL        ;
  if ( !str_prefix( arg , "age"           ) ) field = APPLY_AGE          ;
  if ( !str_prefix( arg , "height"        ) ) field = APPLY_HEIGHT       ;
  if ( !str_prefix( arg , "weight"        ) ) field = APPLY_WEIGHT       ;
  if ( !str_prefix( arg , "mana"          ) ) field = APPLY_MANA         ;
  if ( !str_prefix( arg , "hp"            ) ) field = APPLY_HIT          ;
  if ( !str_prefix( arg , "move"          ) ) field = APPLY_MOVE         ;
  if ( !str_prefix( arg , "gold"          ) ) field = APPLY_GOLD         ;
  if ( !str_prefix( arg , "exp"           ) ) field = APPLY_EXP          ;
  if ( !str_prefix( arg , "ac"            ) ) field = APPLY_AC           ;
  if ( !str_prefix( arg , "hitroll"       ) ) field = APPLY_HITROLL      ;
  if ( !str_prefix( arg , "damroll"       ) ) field = APPLY_DAMROLL      ;
  if ( !str_prefix( arg , "saving_para"   ) ) field = APPLY_SAVING_PARA  ;
  if ( !str_prefix( arg , "saving_rod"    ) ) field = APPLY_SAVING_ROD   ;
  if ( !str_prefix( arg , "saving_petri"  ) ) field = APPLY_SAVING_PETRI ;
  if ( !str_prefix( arg , "saving_breath" ) ) field = APPLY_SAVING_BREATH;
  if ( !str_prefix( arg , "saving_spell"  ) ) field = APPLY_SAVING_SPELL ;

  if ( field == 0 )
  {
    do_oaf( ch , "" );
    RETURN_NULL();
  }

  clear_buffer();
  for ( nMatch = vnum = 0, found = FALSE; nMatch < top_obj_index; vnum++ )
  {
    if ( ( pObjIndex = get_obj_index( vnum ) ) )
    {
      nMatch++;
      for ( paf = pObjIndex->affected; paf; paf = paf->next )
      {
        if ( paf->location == field )
        {
          found = TRUE;
          send_to_buffer( "[\e[1;32m%5d\e[0m] "
                          "(\e[1;34m�v�T\e[0m�R\e[1;32m%5d\e[0m) "
                          "(\e[1;35m����\e[0m�R\e[1;32m%4d\e[0m) "
                          "%s(%s)�C\n\r",
            pObjIndex->vnum,
            paf->modifier,
            pObjIndex->level,
            pObjIndex->short_descr,
            pObjIndex->name );

          if ( buffer_full() ) break;
        }
      }
    }
  }

  if ( !found ) send_to_buffer( "�å����[�o���ݩʪ����~�C\n\r" );
  print_buffer( ch );
  RETURN_NULL();
}

FUNCTION( do_see )
{
  int         count;
  char        arg[ MAX_INPUT_LENGTH ];
  OBJ_DATA *  obj;
  OBJ_DATA  * in_obj;
  CHAR_DATA * victim;
  bool        see_carrying = TRUE;
  bool        see_deposit  = TRUE;

  PUSH_FUNCTION( "do_see" );

  if ( !ch->desc ) RETURN_NULL();
  argument = one_argument( argument, arg );

  /* �u�[�ݤH�����W�����~, �Ӥ��[�ݱH�񪺪��~ */
  if ( arg[0] == '!' )
  {
    see_carrying = TRUE;
    see_deposit  = FALSE;
    str_cpy( arg , arg+1 );
  }

  /* �u�[�ݤH���H�񪺪��~, �Ӥ��[�ݨ��W�����~ */
  if ( arg[0] == '@' && see_deposit )
  {
    see_carrying = FALSE;
    see_deposit  = TRUE;
    str_cpy( arg , arg+1 );
  }

  if ( !( victim = get_char_world( ch, arg ) )  )
  {
    send_to_char( "�䤣��A�ҭn�[�ݪ���H�C\n\r", ch );
    RETURN_NULL();
  }

  clear_buffer();

  if ( ch != victim )
    send_to_buffer( "�A�[�ݩҦ��ݩ�%s�����~�R\n\r\n\r", mob_name( ch, victim ) );

  for ( count = 1, obj = object_list; obj ; obj = obj->next )
  {
    if ( !verify_obj( obj ) ) continue;

    for ( in_obj = obj; in_obj->in_obj ; in_obj = in_obj->in_obj);

    if ( in_obj->carried_by == victim && see_carrying )
    {
      send_to_buffer( "���W[%3d] %s �������X %d �y�����X %d-%d\n\r"
        , count++
        , obj_name( ch, obj )
        , obj->pIndexData->vnum
        , obj->serial.high
        , obj->serial.low );

      if ( buffer_full() ) break;
    }

    if ( in_obj->deposit_by == victim && see_deposit )
    {
      send_to_buffer( "�H��[%3d] %s �������X %d �y�����X %d-%d\n\r"
        , count++
        , obj_name( ch, obj )
        , obj->pIndexData->vnum
        , obj->serial.high
        , obj->serial.low );

      if ( buffer_full() ) break;
    }
  }

  if ( count == 1 ) send_to_buffer( "�S�������󪫫~�C\n\r" );
  print_buffer( ch );
  RETURN_NULL();
}

FUNCTION( do_otype )
{
  extern int       top_obj_index;
  OBJ_INDEX_DATA * obj;
  int              count;
  int              vnum;
  int              nMatch;
  char             arg[MAX_INPUT_LENGTH];
  int              type = -1;

  PUSH_FUNCTION( "do_otype" );

  argument = one_argument( argument, arg );

       if ( arg[0] == '\x0'                )   type = -1;
  else if ( !str_prefix( arg, "light"      ) ) type = ITEM_LIGHT;
  else if ( !str_prefix( arg, "scroll"     ) ) type = ITEM_SCROLL;
  else if ( !str_prefix( arg, "wand"       ) ) type = ITEM_WAND;
  else if ( !str_prefix( arg, "staff"      ) ) type = ITEM_STAFF;
  else if ( !str_prefix( arg, "weapon"     ) ) type = ITEM_WEAPON;
  else if ( !str_prefix( arg, "treasure"   ) ) type = ITEM_TREASURE;
  else if ( !str_prefix( arg, "armor"      ) ) type = ITEM_ARMOR;
  else if ( !str_prefix( arg, "potion"     ) ) type = ITEM_POTION;
  else if ( !str_prefix( arg, "furniture"  ) ) type = ITEM_FURNITURE;
  else if ( !str_prefix( arg, "trash"      ) ) type = ITEM_TRASH;
  else if ( !str_prefix( arg, "container"  ) ) type = ITEM_CONTAINER;
  else if ( !str_prefix( arg, "drink_con"  ) ) type = ITEM_DRINK_CON;
  else if ( !str_prefix( arg, "key"        ) ) type = ITEM_KEY;
  else if ( !str_prefix( arg, "food"       ) ) type = ITEM_FOOD;
  else if ( !str_prefix( arg, "money"      ) ) type = ITEM_MONEY;
  else if ( !str_prefix( arg, "boat"       ) ) type = ITEM_BOAT;
  else if ( !str_prefix( arg, "corpse_npc" ) ) type = ITEM_CORPSE_NPC;
  else if ( !str_prefix( arg, "corpse_pc"  ) ) type = ITEM_CORPSE_PC;
  else if ( !str_prefix( arg, "fountain"   ) ) type = ITEM_FOUNTAIN;
  else if ( !str_prefix( arg, "pill"       ) ) type = ITEM_PILL;
  else if ( !str_prefix( arg, "magicstone" ) ) type = ITEM_MAGICSTONE;
  else if ( !str_prefix( arg, "spiritjade" ) ) type = ITEM_SPIRITJADE;
  else if ( !str_prefix( arg, "mystery"    ) ) type = ITEM_MYSTERY;
  else if ( !str_prefix( arg, "letter"     ) ) type = ITEM_LETTER;
  else if ( !str_prefix( arg, "ore"        ) ) type = ITEM_ORE;
  else if ( !str_prefix( arg, "goldmine"   ) ) type = ITEM_GOLDMINE;
  else if ( !str_prefix( arg, "sigil"      ) ) type = ITEM_SIGIL;
  else if ( !str_prefix( arg, "vellum"     ) ) type = ITEM_VELLUM;
  else if ( !str_prefix( arg, "firework"   ) ) type = ITEM_FIREWORK;
  else if ( !str_prefix( arg, "urn"        ) ) type = ITEM_URN;
  else if ( !str_prefix( arg, "backbag"    ) ) type = ITEM_BACKBAG;
  else if ( !str_prefix( arg, "arrow"      ) ) type = ITEM_ARROW;
  if ( type < 0 )
  {
    send_to_char( "�A������r�����O�U�C���r��R\n\r\n\r"
      "    light       scroll      wand        staff       weapon\n\r"
      "    treasure    armor       potion      furniture   trash\n\r"
      "    container   drink_con   key         food        money\n\r"
      "    boat        corpse_npc  coprse_pc   fountain    pill\n\r"
      "    magicstone  spiritjade  mystery     letter      ore\n\r"
      "    goldmine    sigil       vellum      firework    urn\n\r"
      "    backbag     arrow\n\r\n\r", ch );

    RETURN_NULL();
  }

  clear_buffer();
  send_to_buffer( "�Ҧ�����%s���A�����~�C��R\n\r", item_kind_name( type ) );

  for ( vnum = count = nMatch = 0; nMatch < top_obj_index; vnum++ )
  {
    if ( ( obj = get_obj_index( vnum ) ) )
    {
      nMatch++;
      if ( obj->item_type == type )
      {
        send_to_buffer( "%3d %5d %25s %s(%s)\n\r"
          , ++count, obj->vnum, obj->name
          , obj->short_descr, obj->name );

        if ( buffer_full() ) break;
      }
    }
  }

  if ( count == 0 ) send_to_buffer( "�S���o�{�����W�z���A�����~�T\n\r" );
  print_buffer( ch );
  RETURN_NULL();
}

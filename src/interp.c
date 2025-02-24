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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "merc.h"

bool    check_social    args( ( CHAR_DATA * , char * , char * ) );
bool    MP_Commands     args( ( CHAR_DATA * ) );

bool        fLogAll = FALSE;
CMD_DATA  * LastCommand;
CHAR_DATA * LastChar;
char        LastArgument[MAX_INPUT_LENGTH];

void interpret( CHAR_DATA * ch, char * argument )
{
  JOB_MESSAGE   pMessage;
  JOB_DATA    * pJob;
  CMD_DATA    * pCommand;
  OBJ_DATA    * pObj;
  CHAR_DATA   * victim;
  char          command[MAX_INPUT_LENGTH];
  char          logline[MAX_INPUT_LENGTH * 2 ];
  bool          found;
  int           trust;

  PUSH_FUNCTION( "interpret" );

  while ( isSpace( ( int ) *argument ) ) argument++;
  if ( !argument[0] ) RETURN_NULL();

  if ( is_affected( ch, SLOT_HIDE ) ) affect_release( ch, SLOT_HIDE );

  if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_FREEZE ) )
  {
    send_to_char( "�藍�_�M�A�Q�T��U������O�C\n\r", ch );
    RETURN_NULL();
  }

  str_cpy( logline, argument );

  /* �קK ' �o�ӫ��O�|�Q one_argument() �Y�� */
  if ( !isalpha( ( int ) argument[0] ) && !isdigit( ( int ) argument[0] ) )
  {
    command[0] = argument[0];
    command[1] = '\x0';
    argument++;
    while ( isSpace( ( int ) *argument ) ) argument++;
  }

  else
  {
    argument = one_argument( argument, command );
  }

  trust = ( ch->adviser == TRUE ) ? LEV_IMM : get_trust( ch );
  found = FALSE;

  for ( pCommand = cmd_list; pCommand; pCommand = pCommand->next )
  {
    if ( ( pCommand->level <= trust && ( IS_NPC( ch ) || !pCommand->mobonly ) )
      || MP_Commands( ch ) )
    {
      if ( ( !pCommand->exact && !str_prefix( command, pCommand->name ) )
        || (  pCommand->exact && !str_cmp   ( command, pCommand->name ) ) )
      {
        found = TRUE;
        sprintf( logline, "%s%s%s", pCommand->name, argument[0] ? " " : "" , argument );
        break;
      }
    }
  }

  /* �B�z�аO���r�� */
  if ( pCommand )
  {
    if ( pCommand->log == LOG_NEVER )
      str_cpy( logline, "XXXXXXXX XXXXXXXX XXXXXXXX" );

    if ( ( pCommand->log == LOG_WIZ && !IS_NPC( ch ) )
      || ( pCommand->wizlog && !IS_NPC( ch ) && IS_IMMORTAL( ch ) ) )
    {
      mudlog( LOG_WIZARD , "��w%s���ʧ@: %s", ch->name , logline );
      player_log( ch, logline );
    }
  }

  if ( ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_LOG ) )
    || fLogAll
    || ( pCommand && ( pCommand->trace || pCommand->log == LOG_ALWAYS ) ) )
  {
    mudlog( LOG_INFO , "��w%s���ʧ@: %s", ch->name, logline );
  }

  /* ��X�ʵ����r�� */
  if ( ch->desc && ch->desc->snoop_by )
  {
    char buf[MAX_STRING_LENGTH ];

    if ( pCommand )
      sprintf( buf , "%% %s(%s %s)\n\r" , logline, pCommand->name, argument );
    else
      sprintf( buf, "%% %s\n\r", logline );

    write_to_buffer( ch->desc->snoop_by , buf , 0 );
  }

  if ( ch->in_room && command[0] && !IS_NPC( ch ) )
  {
    pMessage.room = NULL;
    pMessage.obj  = NULL;
    pMessage.mob  = NULL;

    for ( pJob = ch->in_room->job; pJob; pJob = pJob->next )
    {
      if ( pJob->function
        && !str_cmp( command, pJob->keyword )
        && ch->position >= pJob->position )
      {
        pMessage.room = ch->in_room;
        ( *pJob->function ) ( ch, argument, &pMessage );
        RETURN_NULL();
      }
    }

    for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
      if ( !IS_NPC( victim ) || !victim->pIndexData ) continue;

      for ( pJob = victim->pIndexData->job; pJob; pJob = pJob->next )
      {
        if ( pJob->function
          && !str_cmp( command, pJob->keyword )
          && ch->position >= pJob->position )
        {
          pMessage.mob = victim;
          ( *pJob->function ) ( ch, argument, &pMessage );
          RETURN_NULL();
        }
      }
    }

    for ( pObj = ch->in_room->contents; pObj; pObj = pObj->next_content )
    {
      if ( !pObj->pIndexData ) continue;

      for ( pJob = pObj->pIndexData->job; pJob; pJob = pJob->next )
      {
        if ( pJob->function
          && !str_cmp( command, pJob->keyword )
          && ch->position >= pJob->position )
        {
          pMessage.obj = pObj;
          ( *pJob->function ) ( ch, argument, &pMessage );
          RETURN_NULL();
        }
      }
    }
  }

  if ( !found )
  {
    if ( !check_social( ch, command, argument ) )
    {
      send_to_char( "���O���~�M�Ьd��(\e[1;32mhelp\e[0m) command "
        "�άO cmds�C\n\r", ch );

      /* �Ǫ������� MOBPROGRAM */
      if ( IS_NPC( ch ) && ch->pIndexData && ch->inprocess && *argument )
      {
        mudlog( LOG_DEBUG, "interpret: �Ǫ� %d ���O %s �Ѽ� %s ���~."
          , ch->pIndexData->vnum, command, argument );
      }
    }

    RETURN_NULL();
  }

  if ( !str_cmp( argument , "/?" )
    || !str_cmp( argument , "-?" ) )
  {
    if ( !pCommand->help || pCommand->help == &str_empty[0] )
      send_to_char( "�藍�_�M�o�ӫ��O�S���u�W�D�U�C\n\r" , ch );

    else
      send_to_char( pCommand->help, ch );

    RETURN_NULL();
  }

  /* �ˬd�O�_���Ѽƪ����O */
  if ( pCommand->argument == TRUE && argument[0] == '\x0' )
  {
    act( "�o�ӫ��O�������ѼơM�п�J $7$t /?$0 �d�ߡC"
      , ch, pCommand->name, NULL, TO_CHAR );

    RETURN_NULL();
  }

  if ( ch->position < pCommand->position )
  {
    switch( ch->position )
    {
    case POS_DEAD:
      send_to_char( "�A�w�g���F�C�M�п�J \e[1;32mhelp death\e[0m�C\n\r", ch );
      break;

    case POS_SLEEPING:
      send_to_char( "�O�A��ı�ɪ��ۼv�٬O�A�����J�]�F�S\n\r", ch );
      break;

    case POS_RESTING:
      send_to_char( "�ڡM�A�ΪA�������_�ӤF�C\n\r", ch);
      break;

    case POS_FIGHTING:
      send_to_char( "����M�A�٦b�԰��C�C\n\r", ch);
      break;
    }
    RETURN_NULL();
  }

  if ( !pCommand->dead && is_dead( ch ) )
  {
    send_to_char( "�藍�_�M�o�ӫ��O��A���`�ɡM�O������檺�T\n\r", ch );
    RETURN_NULL();
  }

  /* ���O�O�_�Q��w */
  if ( pCommand->lock == TRUE )
  {
    send_to_char( "�o�ӫ��O�Q��w�M�]���������C\n\r" , ch );
    RETURN_NULL();
  }

  if ( !IS_NPC( ch ) && ch->jail > 0 && !pCommand->jail )
  {
    send_to_char( "�A���b���D���M�o�ӫ��O�A����ϥΡT\n\r", ch );
    RETURN_NULL();
  }

  if ( !IS_NPC( ch ) && ch->failed > 0 && !pCommand->lost )
  {
    send_to_char( "�A���b��L���M�o�ӫ��O�A����ϥΡT\n\r", ch );
    RETURN_NULL();
  }

  if ( !IS_NPC( ch ) && pCommand->limit )
  {
    if ( get_curr_str( ch ) <= 0 )
    {
      send_to_char( "�A���O�q���t�ȡM�o�ӫ��O�������T\n\r", ch );
      RETURN_NULL();
    }

    if ( get_curr_int( ch ) <= 0 )
    {
      send_to_char( "�A�����z���t�ȡM�o�ӫ��O�������T\n\r", ch );
      RETURN_NULL();
    }

    if ( get_curr_wis( ch ) <= 0 )
    {
      send_to_char( "�A�����Ѭ��t�ȡM�o�ӫ��O�������T\n\r", ch );
      RETURN_NULL();
    }

    if ( get_curr_dex( ch ) <= 0 )
    {
      send_to_char( "�A���ӱ����t�ȡM�o�ӫ��O�������T\n\r", ch );
      RETURN_NULL();
    }

    if ( get_curr_con( ch ) <= 0 )
    {
      send_to_char( "�A����欰�t�ȡM�o�ӫ��O�������T\n\r", ch );
      RETURN_NULL();
    }
  }

  if ( pCommand->chat && !IS_NPC( ch ) )
    mudlog( LOG_CHAT, "%-12s %s", ch->name, argument );

  /* �O���ϥΪ����O���� */
  if ( !IS_NPC( ch ) ) pCommand->exec++;

  /* �O���̪�ϥΪ��p */
  LastCommand = pCommand;
  LastChar    = ch;
  str_cpy( LastArgument , argument );

  /* ����R�O */
  ( *pCommand->function ) ( ch , argument );

  /* �����̪�ϥΪ��p */
  LastCommand     = NULL;
  LastChar        = NULL;
  LastArgument[0] = '\x0';

  tail_chain();
  RETURN_NULL();
}

bool check_social( CHAR_DATA * ch, char * command, char * argument )
{
  CHAR_DATA   * victim;
  char          arg[MAX_INPUT_LENGTH];
  SOCIAL_DATA * social;

  PUSH_FUNCTION( "check_social" );

  if ( !ch || !ch->in_room )
  {
    mudlog( LOG_DEBUG, "check_social: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  for ( social = social_first; social; social = social->next )
  {
    if ( command[0] == social->name[0]
      && !str_prefix( command, social->name ) ) break;
  }

  if ( !social ) RETURN( FALSE );

  /* �p�G�O���M�O�S�����檺���O */
  if ( IS_NPC( ch ) && get_rider( ch ) ) RETURN( FALSE );

  if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_NO_EMOTE ) )
  {
    send_to_char( "�藍�_�M�A�Q�T����󪺪�����O�C\n\r", ch );
    RETURN( TRUE );
  }

  switch ( ch->position )
  {
  case POS_DEAD:

    send_to_char( "�A�w�g���F�M��򰵪�����O�ڡC\n\r", ch );
    RETURN( TRUE );

  case POS_SLEEPING:

    if ( !str_cmp( social->name, "snore" ) )  break;
    send_to_char( "�A�b��ı�F�C�M���వ������O�C\n\r", ch );
    RETURN( TRUE );
  }

  one_argument( argument, arg );
  victim = NULL;

  if ( arg[0] == '\x0' )
  {
    act( smash_act_keyword( social->char_no_arg  , "nesNES" )
      , ch, NULL, victim, TO_CHAR );
    act( smash_act_keyword( social->others_no_arg, "nesNES" )
      , ch, NULL, victim, TO_ROOM );
  }

  else if ( ( victim = get_char_room( ch, arg ) ) == NULL )
  {
    send_to_char( "�A����H���b�o�̡C\n\r", ch );
  }

  else if ( get_rider( victim ) )
  {
    send_to_char( "�A����理�M��������O�C\n\r", ch );
  }

  else if ( victim->hirer )
  {
    send_to_char( "�A�����ħL������ʧ@�C\n\r", ch );
  }

  else if ( victim->boss )
  {
    send_to_char( "�A�����O�H���p��������ʧ@�T\n\r", ch );
  }

  else if ( victim == ch )
  {
    act( smash_act_keyword( social->char_auto  , "nesNES" )
      , ch, NULL, victim, TO_CHAR );

    act( smash_act_keyword( social->others_auto, "nesNES" )
      , ch, NULL, victim, TO_ROOM );
  }
  else
  {
    act( smash_act_keyword( social->char_found, "nesNES"   )
      , ch, NULL, victim, TO_CHAR    );

    act( smash_act_keyword( social->vict_found, "nesNES"   )
      , ch, NULL, victim, TO_VICT    );

    act( smash_act_keyword( social->others_found, "nesNES" )
      , ch, NULL, victim, TO_NOTVICT );

    if ( is_affected( victim, SLOT_MASK ) )
      send_to_char( "�A�����e�N���G�Q�ѯ}�F�T\n\r", victim );

    if ( !IS_NPC( ch )
      && IS_NPC( victim )
      && !is_affected( victim, SLOT_CHARM_PERSON )
      && IS_AWAKE( victim )
      && !victim->mount_by
      && victim->position != POS_DEAD )
    {
      switch ( number_bits( 4 ) )
      {
      case 0:
        if ( ( victim->level < ch->level )
          && !( victim->fighting )
          && !IS_SET( victim->act, ACT_NOKILL )
          && ch->in_room->Safe == FALSE )
          striking( victim, ch, TYPE_UNDEFINED );
        break;

      case 1: case 2: case 3: case 4:
      case 5: case 6: case 7: case 8:
        act( smash_act_keyword( social->char_found, "nesNES"   )
          , victim, NULL, ch, TO_CHAR    );

        act( smash_act_keyword( social->vict_found, "nesNES"   )
          , victim, NULL, ch, TO_VICT    );

        act( smash_act_keyword( social->others_found, "nesNES" )
          , victim, NULL, ch, TO_NOTVICT );
        break;

      case 9: case 10: case 11: case 12:
        act( "$n���F$N�@�ڴx�C", victim, NULL, ch, TO_NOTVICT );
        act( "�A���F$N�@�ڴx�C", victim, NULL, ch, TO_CHAR    );
        act( "$n���F�A�@�ڴx�C", victim, NULL, ch, TO_VICT    );
        break;
      }
    }
  }
  RETURN( TRUE );
}

int number_argument( char * argument, char * arg )
{
  char * pdot;
  int    number;

  PUSH_FUNCTION( "number_argument" );

  for ( pdot = argument; *pdot != '\x0'; pdot++ )
  {
    if ( *pdot == '.' )
    {
      *pdot = '\x0';
      number = atoi( argument );
      *pdot = '.';
      str_cpy( arg, pdot+1 );
      RETURN( number);
    }
  }

  str_cpy( arg, argument );
  RETURN(1);
}

char * one_argument( char * argument, char * arg_first )
{
  char cEnd;

  PUSH_FUNCTION( "one_argument" );

  if ( !argument || !arg_first )
  {
    mudlog( LOG_DEBUG, "one_argument: �ӷ������T." );
    RETURN( NULL );
  }

  while ( isSpace( ( int ) *argument ) ) argument++;

  if ( *argument == '\'' || *argument == '"' ) cEnd = *argument++;
  else                                         cEnd =         ' ';

  while ( *argument != '\x0' )
  {
    if ( *argument == cEnd )
    {
      argument++;
      break;
    }
    *arg_first++ = *argument++;
  }

  *arg_first = '\x0';
  while ( isSpace( ( int ) *argument ) ) argument++;

  RETURN( argument );
}

char * one_statement( char * argument, char * arg_first )
{
  PUSH_FUNCTION( "one_statement" );

  if ( !argument || !arg_first )
  {
    mudlog( LOG_DEBUG, "one_statement: �ӷ������T." );
    RETURN( NULL );
  }

  while ( isSpace( ( int ) *argument ) ) argument++;

  while ( *argument != '\x0' )
  {
    if ( *argument == '+' )
    {
      argument++;
      break;
    }
    *arg_first++ = *argument++;
  }

  *arg_first = '\x0';
  while ( isSpace( ( int ) *argument ) ) argument++;

  RETURN( argument );
}

bool MP_Commands( CHAR_DATA * ch )
{
  PUSH_FUNCTION( "MP_Commands" );

  if ( IS_NPC( ch )
    && ch->pIndexData->progtypes
    && !is_affected( ch, SLOT_CHARM_PERSON )
    && !ch->boss )
    RETURN( TRUE );

  RETURN( FALSE );
}

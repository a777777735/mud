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

extern bool check_quest args( ( CHAR_DATA *, const char * ) );
bool check_quest        args( ( CHAR_DATA *, const char * ) );

void set_quest( CHAR_DATA * ch, char * mark )
{
  QUEST_DATA * pQuest;
  QUEST_INFO * aQuest;

  PUSH_FUNCTION( "set_quest" );

  if ( !ch || !mark || !*mark )
  {
    mudlog( LOG_DEBUG, "set_quest: �ӷ������D." );
    RETURN_NULL();
  }

  if ( IS_NPC( ch ) )
  {
    mudlog( LOG_DEBUG, "set_quest: ��H�O�D���a." );
    RETURN_NULL();
  }

  /* �קK�s�ɮɦ� ~ �o�ӲŸ� */
  smash_tilde( mark );

  if ( !( aQuest = quest_lookup( mark ) ) )
  {
    mudlog( LOG_DEBUG, "set_quest: ���~����r%s.", mark );
    RETURN_NULL();
  }

  /* �ˬd�O�_������ */
  for ( pQuest = ch->quest; pQuest; pQuest = pQuest->next )
    if ( pQuest->link == aQuest ) RETURN_NULL();

  pQuest       = alloc_struct( STRUCT_QUEST_DATA );
  pQuest->link = aQuest;
  pQuest->next = ch->quest;
  ch->quest    = pQuest;

  RETURN_NULL();
}

bool check_quest( CHAR_DATA * ch, const char * mark )
{
  QUEST_INFO * aQuest;
  QUEST_DATA * pQuest;

  PUSH_FUNCTION( "check_quest" );

  if ( !ch || !mark || !*mark )
  {
    mudlog( LOG_DEBUG, "check_quest: �ӷ������~." );
    RETURN( FALSE );
  }

  if ( IS_NPC( ch ) )
  {
    mudlog( LOG_DEBUG, "check_quest: ��H�O�D���a." );
    RETURN( FALSE );
  }

  if ( !( aQuest = quest_lookup( mark ) ) )
  {
    mudlog( LOG_DEBUG, "check_quest: ���~����r%s.", mark );
    RETURN( FALSE );
  }

  for ( pQuest = ch->quest; pQuest; pQuest = pQuest->next )
    if ( pQuest->link == aQuest ) RETURN( TRUE );

  RETURN( FALSE );
}

QUEST_INFO * quest_lookup( const char * mark )
{
  QUEST_INFO * pQuest;

  PUSH_FUNCTION( "quest_lookup" );

  if ( !mark || !*mark ) RETURN( NULL );

  for ( pQuest = quest_list; pQuest; pQuest = pQuest->next )
    if ( !str_cmp( pQuest->mark, mark ) ) RETURN( pQuest );

  RETURN( NULL );
}

void rem_quest( CHAR_DATA * ch, const char * mark )
{
  QUEST_INFO * aQuest;
  QUEST_DATA * pQuest;
  QUEST_DATA * prev;

  PUSH_FUNCTION( "rem_quest" );

  if ( !ch || !mark || !*mark )
  {
    mudlog( LOG_DEBUG, "rem_quest: �ӷ������T." );
    RETURN_NULL();
  }

  if ( !( aQuest = quest_lookup( mark ) ) )
  {
    mudlog( LOG_DEBUG, "rem_quest: ���~����r%s.", mark );
    RETURN_NULL();
  }

  for ( prev = NULL, pQuest = ch->quest; pQuest; pQuest = pQuest->next )
  {
    if ( pQuest->link == aQuest )
    {
      if ( !prev ) ch->quest  = ch->quest->next;
      else         prev->next = pQuest->next;

      free_struct( pQuest, STRUCT_QUEST_DATA );
      RETURN_NULL();
    }

    prev = pQuest;
  }

  mudlog( LOG_DEBUG, "rem_quest: �䤣��аO %s.", mark );
  RETURN_NULL();
}

FUNCTION( do_qstat )
{
  QUEST_INFO * aQuest;
  QUEST_DATA * pQuest;
  CHAR_DATA  * victim;
  char         arg[MAX_INPUT_LENGTH];
  int          count;

  PUSH_FUNCTION( "do_qstat" );

  argument = one_argument( argument, arg );

  if ( !arg[0] )
  {
    if ( !quest_list )
    {
      act( "$t�S�����������T�T", ch, mud_name, NULL, TO_CHAR );
      RETURN_NULL();
    }

    clear_buffer();
    send_to_buffer( "\e[1;33;44m���� ��������r           �����ѻ��T��"
      "                  ���                  \n\r\e[0m" );
    for ( count = 0, aQuest = quest_list; aQuest; aQuest = aQuest->next )
    {
      send_to_buffer( "%3d. %-20s %-30s %s\n\r"
        , ++count, aQuest->mark, aQuest->info, YESNO( aQuest->show ) );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !( victim = get_char_world( ch, arg ) ) )
  {
    act( "�S�����A����H $2$T$0�C", ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  one_argument( argument, arg );

  if ( !arg[0] )
  {
    clear_buffer();
    for ( count = 0, pQuest = victim->quest; pQuest; pQuest = pQuest->next )
    {
      if ( !( aQuest = pQuest->link ) ) continue;
      if ( count == 0 ) send_to_buffer( "%s���Ѱg����r��\n\r"
        , mob_name( ch, victim ) );

      send_to_buffer( "%2d. %s %s\n\r", ++count, aQuest->mark, aQuest->info );
    }

    if ( count == 0 ) send_to_buffer( "%s�S�����󪺸Ѱg����r�C\n\r"
      , mob_name( ch, victim ) );

    print_buffer( ch );
  }

  else
  {
    if ( !( aQuest = quest_lookup( arg ) ) )
    {
      act( "�t�ΨS��$t�Ѱg����r�C", ch, arg, NULL, TO_CHAR );
      RETURN_NULL();
    }

    for ( pQuest = victim->quest; pQuest; pQuest = pQuest->next )
    {
      if ( pQuest->link == aQuest )
      {
        act( "$N��$t�o�ӸѰg����r�C", ch, aQuest->mark, victim, TO_CHAR );
        RETURN_NULL();
      }
    }

    act( "$N�S��$t�Ѱg����r�C", ch, arg, victim, TO_CHAR );
  }
  RETURN_NULL();
}

FUNCTION( do_qset )
{
  CHAR_DATA  * victim;
  char         arg[MAX_INPUT_LENGTH];

  PUSH_FUNCTION( "do_qset" );

  argument = one_argument( argument, arg );

  if ( !arg[0] )
  {
    send_to_char( "�A���y�k���~�M�Ьd�� qset\n\r", ch );
    RETURN_NULL();
  }

  if ( !( victim = get_char_world( ch, arg ) ) )
  {
    act( "�䤣��A����H $2$T$0�T", ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  if ( IS_NPC( victim ) )
  {
    act( "����ϥΦb�D���a$N���W�T", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  one_argument( argument, arg );

  switch( arg[0] )
  {
  default:

    send_to_char( "�A���y�k���~�M�Ьd�� qset�C\n\r", ch );
    break;

  case '+':

    if ( !arg[1] )
    {
      send_to_char( "�A�n�[�J���@������r�S\n\r", ch );
      RETURN_NULL();
    }

    if ( check_quest( victim, arg + 1 ) )
    {
      act( "�藍�_�M�o������r $t �w�g���F��T", ch, arg + 1, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( !quest_lookup( arg + 1 ) )
    {
      act( "�藍�_�M�t�ΨS���o�Ӹ�������r $t�C", ch, arg + 1, NULL, TO_CHAR );
      RETURN_NULL();
    }

    set_quest( victim, arg + 1 );
    act( "�A��$N�[�J�F$t�o������r�C", ch, arg + 1, victim, TO_CHAR );
    break;

  case '-':

    if ( !arg[1] )
    {
      send_to_char( "�A�n�R�����@������r�S\n\r", ch );
      RETURN_NULL();
    }

    if ( !quest_lookup( arg + 1 ) )
    {
      act( "�t�Τ��S�� $2$T$0 �o������r�C", ch, NULL, arg+1, TO_CHAR );
      RETURN_NULL();
    }

    if ( !check_quest( victim, arg + 1 ) )
    {
      act( "$N�S���o������r $t�C", ch, arg + 1, victim, TO_CHAR );
      RETURN_NULL();
    }

    act( "�A���h�F$N������r$t�C", ch, arg+1, victim, TO_CHAR );
    rem_quest( victim, arg + 1 );
    break;
  }

  RETURN_NULL();
}

bool is_quest( CHAR_DATA * ch, char * opr, char * val )
{
  char arg[MAX_INPUT_LENGTH];
  bool found;

  PUSH_FUNCTION( "is_quest" );

  if ( IS_NPC( ch ) )
  {
    mudlog( LOG_DEBUG, "is_quest: ��H�O�D���a." );
    RETURN( FALSE );
  }

  one_argument( val, arg );
  if ( !arg[0] || !opr || !*opr )
  {
    mudlog( LOG_DEBUG, "is_quest: �S���޼�" );
    RETURN( FALSE );
  }

  found = check_quest( ch, arg );

  if ( !str_cmp( opr, "!=" ) ) RETURN( !found );
  if ( !str_cmp( opr, "==" ) ) RETURN( found );

  mudlog( LOG_DEBUG, "is_quest: ���~���B�⤸ %s.", opr );
  RETURN( FALSE );
}

FUNCTION( do_mpsetquest )
{
  CHAR_DATA * victim;
  char        arg1[MAX_INPUT_LENGTH];
  char        arg2[MAX_INPUT_LENGTH];
  int         vnum;

  PUSH_FUNCTION( "do_mpsetquest" );

  vnum     = ch->pIndexData->vnum;
  argument = one_argument( argument, arg1 );
             one_argument( argument, arg2 );

  if ( !arg1[0] || !arg2[0] )
  {
    mudlog( LOG_DEBUG, "do_mpsetquest: MOB %d �޼ƿ��~.", vnum );
    RETURN_NULL();
  }

  if ( !( victim = get_pc_room( ch, arg1 ) ) )
  {
    mudlog( LOG_DEBUG, "do_mpsetquest: MOB %d ��H���s�b.", vnum );
    RETURN_NULL();
  }

  if ( !IS_NPC( victim ) && !check_quest( victim, arg2 ) )
    set_quest( victim, arg2 );

  RETURN_NULL();
}

FUNCTION( do_mpremquest )
{
  CHAR_DATA * victim;
  char        arg1[MAX_INPUT_LENGTH];
  char        arg2[MAX_INPUT_LENGTH];
  int         vnum;

  PUSH_FUNCTION( "do_mpremquest" );

  vnum     = ch->pIndexData->vnum;
  argument = one_argument( argument, arg1 );
             one_argument( argument, arg2 );

  if ( !arg1[0] || !arg2[0] )
  {
    mudlog( LOG_DEBUG, "do_mpremquest: MOB %d �޼ƿ��~.", vnum );
    RETURN_NULL();
  }

  if ( !( victim = get_pc_room( ch, arg1 ) ) )
  {
    mudlog( LOG_DEBUG, "do_mpremquest: MOB %d ��H���s�b.", vnum );
    RETURN_NULL();
  }

  if ( !IS_NPC( victim ) && check_quest( victim, arg2 ) )
    rem_quest( victim, arg2 );

  RETURN_NULL();
}

FUNCTION( do_task )
{
  char         arg[MAX_INPUT_LENGTH];
  char         buf[MAX_INPUT_LENGTH];
  int          count = 0;
  CHAR_DATA  * victim;
  QUEST_DATA * pQuest;
  QUEST_INFO * aQuest;

  PUSH_FUNCTION( "do_task" );

  if ( !ch || !verify_char( ch ) )
  {
    mudlog( LOG_DEBUG, "do_task: �ӷ������T." );
    RETURN_NULL();
  }

  if ( IS_NPC( ch ) ) RETURN_NULL();

  argument = one_argument( argument, arg );

  if ( !arg[0] )
  {
    victim = ch;
  }

  else if ( !str_prefix( arg, "!info" ) )
  {
    one_argument( argument, arg );

    if ( !arg[0] )
    {
      victim = ch;
    }

    else if ( !IS_IMMORTAL( ch ) )
    {
      send_to_char( "�A���v�O�����ݧO�H�����ȫ��ܡC\n\r", ch );
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
        act( "�D���a$N�O�S�����ȫ��ܡC", ch, NULL, victim, TO_CHAR );
        RETURN_NULL();
      }

      if ( get_trust( ch ) < get_trust( victim ) )
      {
        act( "�A���v�O������$N�����ȫ��ܡC", ch, NULL, victim, TO_CHAR );
        RETURN_NULL();
      }
    }

    clear_buffer();
    for ( aQuest = quest_list; aQuest; aQuest = aQuest->next )
    {
      if ( !aQuest->show  && !IS_IMMORTAL( victim ) )               continue;
      if ( !aQuest->title
        && !check_quest( victim, aQuest->mark )
        && !IS_IMMORTAL( victim ) ) continue;
      if ( count == 0 ) send_to_buffer( "%s�b%s�������Ȧ��R\n\r"
        , mob_name( ch, victim ), mud_name );

      chinese_number( ++count, buf );
      send_to_buffer( "%12s�B %-30s %s%2s\e[0m\n\r"
        , buf, aQuest->info
        , check_quest( victim, aQuest->mark ) ? "\e[1;32m" : ""
        , YESNO( check_quest( victim, aQuest->mark ) ) );
    }

    if ( count == 0 ) send_to_buffer( "%s�b%s�S��������㪺�����ʧ@�C\n\r"
      , mob_name( ch, victim ), mud_name );

    print_buffer( ch );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "!help" ) )
  {
    one_argument( argument, arg );

    if ( !quest_list )
    {
      act( "$t�S��������ȸ����ʧ@�C", ch, mud_name, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( !arg[0] )
    {
      send_to_char( "�A�n�ݨ��@�ӥ��Ȫ����ܩO�S\n\r", ch );
      RETURN_NULL();
    }

    for ( aQuest = quest_list; aQuest; aQuest = aQuest->next )
      if ( !strcmp( aQuest->info, arg ) ) break;

    if ( !aQuest
      || ( !aQuest->show && !IS_IMMORTAL( ch ) )
      || !aQuest->help
      || !aQuest->help[0] )
    {
      send_to_char( "�藍�_�M�S������o�ӥ��Ȫ����ܡM�A�Q�Ӧh�F�C\n\r", ch );
      RETURN_NULL();
    }

    act( "����$t�����ȴ��ܦp�U�R$A$T", ch, aQuest->info, aQuest->help, TO_CHAR );
    RETURN_NULL();
  }

  else if ( !IS_IMMORTAL( ch ) )
  {
    send_to_char( "�A���v�O�����ݧO�H�����ȫ��ܡC\n\r", ch );
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
      act( "�D���a$N�O�S�����ȫ��ܡC", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( get_trust( ch ) < get_trust( victim ) )
    {
      act( "�A���v�O������$N�����ȫ��ܡC", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }
  }

  clear_buffer();
  for ( pQuest = victim->quest; pQuest; pQuest = pQuest->next )
  {
    if ( !( aQuest = pQuest->link ) || !aQuest->show ) continue;

    if ( count == 0 ) send_to_buffer( "%s�����ȫ��ܦp�U�R\n\r"
      , mob_name( ch, victim ) );

    chinese_number( ++count, buf );
    send_to_buffer( "%4s�B%-30s\n\r", buf, aQuest->info );
  }

  if ( count == 0 ) send_to_buffer( "%s�S��������ȫ��ܡC\n\r"
    , mob_name( ch, victim ) );

  print_buffer( ch );
  RETURN_NULL();
}

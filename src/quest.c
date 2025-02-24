/***************************************************************************
*  這是由輔大化學系製作群所撰寫的遊戲﹐主體由 merc 改編而來﹐所有的版權    *
*  將會被保留﹐但歡迎大家修改﹐但我們也希望你們也能提供給大家﹐所有的商    *
*  業行為將不被允許。                                                      *
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
    mudlog( LOG_DEBUG, "set_quest: 來源有問題." );
    RETURN_NULL();
  }

  if ( IS_NPC( ch ) )
  {
    mudlog( LOG_DEBUG, "set_quest: 對象是非玩家." );
    RETURN_NULL();
  }

  /* 避免存檔時有 ~ 這個符號 */
  smash_tilde( mark );

  if ( !( aQuest = quest_lookup( mark ) ) )
  {
    mudlog( LOG_DEBUG, "set_quest: 錯誤關鍵字%s.", mark );
    RETURN_NULL();
  }

  /* 檢查是否有重複 */
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
    mudlog( LOG_DEBUG, "check_quest: 來源有錯誤." );
    RETURN( FALSE );
  }

  if ( IS_NPC( ch ) )
  {
    mudlog( LOG_DEBUG, "check_quest: 對象是非玩家." );
    RETURN( FALSE );
  }

  if ( !( aQuest = quest_lookup( mark ) ) )
  {
    mudlog( LOG_DEBUG, "check_quest: 錯誤關鍵字%s.", mark );
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
    mudlog( LOG_DEBUG, "rem_quest: 來源不正確." );
    RETURN_NULL();
  }

  if ( !( aQuest = quest_lookup( mark ) ) )
  {
    mudlog( LOG_DEBUG, "rem_quest: 錯誤關鍵字%s.", mark );
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

  mudlog( LOG_DEBUG, "rem_quest: 找不到標記 %s.", mark );
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
      act( "$t沒有任何解謎資訊﹗", ch, mud_name, NULL, TO_CHAR );
      RETURN_NULL();
    }

    clear_buffer();
    send_to_buffer( "\e[1;33;44m順序 解謎關鍵字           解謎解說訊息"
      "                  顯示                  \n\r\e[0m" );
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
    act( "沒有找到你的對象 $2$T$0。", ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  one_argument( argument, arg );

  if ( !arg[0] )
  {
    clear_buffer();
    for ( count = 0, pQuest = victim->quest; pQuest; pQuest = pQuest->next )
    {
      if ( !( aQuest = pQuest->link ) ) continue;
      if ( count == 0 ) send_to_buffer( "%s的解迷關鍵字有\n\r"
        , mob_name( ch, victim ) );

      send_to_buffer( "%2d. %s %s\n\r", ++count, aQuest->mark, aQuest->info );
    }

    if ( count == 0 ) send_to_buffer( "%s沒有任何的解迷關鍵字。\n\r"
      , mob_name( ch, victim ) );

    print_buffer( ch );
  }

  else
  {
    if ( !( aQuest = quest_lookup( arg ) ) )
    {
      act( "系統沒有$t解迷關鍵字。", ch, arg, NULL, TO_CHAR );
      RETURN_NULL();
    }

    for ( pQuest = victim->quest; pQuest; pQuest = pQuest->next )
    {
      if ( pQuest->link == aQuest )
      {
        act( "$N有$t這個解迷關鍵字。", ch, aQuest->mark, victim, TO_CHAR );
        RETURN_NULL();
      }
    }

    act( "$N沒有$t解迷關鍵字。", ch, arg, victim, TO_CHAR );
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
    send_to_char( "你的語法錯誤﹐請查詢 qset\n\r", ch );
    RETURN_NULL();
  }

  if ( !( victim = get_char_world( ch, arg ) ) )
  {
    act( "找不到你的對象 $2$T$0﹗", ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  if ( IS_NPC( victim ) )
  {
    act( "不能使用在非玩家$N身上﹗", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  one_argument( argument, arg );

  switch( arg[0] )
  {
  default:

    send_to_char( "你的語法錯誤﹐請查詢 qset。\n\r", ch );
    break;

  case '+':

    if ( !arg[1] )
    {
      send_to_char( "你要加入哪一個關鍵字﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( check_quest( victim, arg + 1 ) )
    {
      act( "對不起﹐這個關鍵字 $t 已經有了喔﹗", ch, arg + 1, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( !quest_lookup( arg + 1 ) )
    {
      act( "對不起﹐系統沒有這個解謎關鍵字 $t。", ch, arg + 1, NULL, TO_CHAR );
      RETURN_NULL();
    }

    set_quest( victim, arg + 1 );
    act( "你為$N加入了$t這個關鍵字。", ch, arg + 1, victim, TO_CHAR );
    break;

  case '-':

    if ( !arg[1] )
    {
      send_to_char( "你要刪除哪一個關鍵字﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( !quest_lookup( arg + 1 ) )
    {
      act( "系統內沒有 $2$T$0 這個關鍵字。", ch, NULL, arg+1, TO_CHAR );
      RETURN_NULL();
    }

    if ( !check_quest( victim, arg + 1 ) )
    {
      act( "$N沒有這個關鍵字 $t。", ch, arg + 1, victim, TO_CHAR );
      RETURN_NULL();
    }

    act( "你移去了$N的關鍵字$t。", ch, arg+1, victim, TO_CHAR );
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
    mudlog( LOG_DEBUG, "is_quest: 對象是非玩家." );
    RETURN( FALSE );
  }

  one_argument( val, arg );
  if ( !arg[0] || !opr || !*opr )
  {
    mudlog( LOG_DEBUG, "is_quest: 沒有引數" );
    RETURN( FALSE );
  }

  found = check_quest( ch, arg );

  if ( !str_cmp( opr, "!=" ) ) RETURN( !found );
  if ( !str_cmp( opr, "==" ) ) RETURN( found );

  mudlog( LOG_DEBUG, "is_quest: 錯誤的運算元 %s.", opr );
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
    mudlog( LOG_DEBUG, "do_mpsetquest: MOB %d 引數錯誤.", vnum );
    RETURN_NULL();
  }

  if ( !( victim = get_pc_room( ch, arg1 ) ) )
  {
    mudlog( LOG_DEBUG, "do_mpsetquest: MOB %d 對象不存在.", vnum );
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
    mudlog( LOG_DEBUG, "do_mpremquest: MOB %d 引數錯誤.", vnum );
    RETURN_NULL();
  }

  if ( !( victim = get_pc_room( ch, arg1 ) ) )
  {
    mudlog( LOG_DEBUG, "do_mpremquest: MOB %d 對象不存在.", vnum );
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
    mudlog( LOG_DEBUG, "do_task: 來源不正確." );
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
      send_to_char( "你的權力不夠看別人的任務指示。\n\r", ch );
      RETURN_NULL();
    }

    else
    {
      if ( !( victim = get_char_world( ch, arg ) ) )
      {
        act( "找不到你要觀看的對象 $2$T$0。", ch, NULL, arg, TO_CHAR );
        RETURN_NULL();
      }

      if ( IS_NPC( victim ) )
      {
        act( "非玩家$N是沒有任務指示。", ch, NULL, victim, TO_CHAR );
        RETURN_NULL();
      }

      if ( get_trust( ch ) < get_trust( victim ) )
      {
        act( "你的權力不夠看$N的任務指示。", ch, NULL, victim, TO_CHAR );
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
      if ( count == 0 ) send_to_buffer( "%s在%s解謎任務有﹕\n\r"
        , mob_name( ch, victim ), mud_name );

      chinese_number( ++count, buf );
      send_to_buffer( "%12s、 %-30s %s%2s\e[0m\n\r"
        , buf, aQuest->info
        , check_quest( victim, aQuest->mark ) ? "\e[1;32m" : ""
        , YESNO( check_quest( victim, aQuest->mark ) ) );
    }

    if ( count == 0 ) send_to_buffer( "%s在%s沒有任何明顯的解謎動作。\n\r"
      , mob_name( ch, victim ), mud_name );

    print_buffer( ch );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "!help" ) )
  {
    one_argument( argument, arg );

    if ( !quest_list )
    {
      act( "$t沒有任何任務解謎動作。", ch, mud_name, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( !arg[0] )
    {
      send_to_char( "你要看那一個任務的提示呢﹖\n\r", ch );
      RETURN_NULL();
    }

    for ( aQuest = quest_list; aQuest; aQuest = aQuest->next )
      if ( !strcmp( aQuest->info, arg ) ) break;

    if ( !aQuest
      || ( !aQuest->show && !IS_IMMORTAL( ch ) )
      || !aQuest->help
      || !aQuest->help[0] )
    {
      send_to_char( "對不起﹐沒有關於這個任務的提示﹐你想太多了。\n\r", ch );
      RETURN_NULL();
    }

    act( "關於$t的任務提示如下﹕$A$T", ch, aQuest->info, aQuest->help, TO_CHAR );
    RETURN_NULL();
  }

  else if ( !IS_IMMORTAL( ch ) )
  {
    send_to_char( "你的權力不夠看別人的任務指示。\n\r", ch );
    RETURN_NULL();
  }

  else
  {
    if ( !( victim = get_char_world( ch, arg ) ) )
    {
      act( "找不到你要觀看的對象 $2$T$0。", ch, NULL, arg, TO_CHAR );
      RETURN_NULL();
    }

    if ( IS_NPC( victim ) )
    {
      act( "非玩家$N是沒有任務指示。", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( get_trust( ch ) < get_trust( victim ) )
    {
      act( "你的權力不夠看$N的任務指示。", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }
  }

  clear_buffer();
  for ( pQuest = victim->quest; pQuest; pQuest = pQuest->next )
  {
    if ( !( aQuest = pQuest->link ) || !aQuest->show ) continue;

    if ( count == 0 ) send_to_buffer( "%s的任務指示如下﹕\n\r"
      , mob_name( ch, victim ) );

    chinese_number( ++count, buf );
    send_to_buffer( "%4s、%-30s\n\r", buf, aQuest->info );
  }

  if ( count == 0 ) send_to_buffer( "%s沒有任何任務指示。\n\r"
    , mob_name( ch, victim ) );

  print_buffer( ch );
  RETURN_NULL();
}

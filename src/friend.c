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
#include <ctype.h>
#include <stdio.h>
#include "merc.h"

/* 每個人最多可以有幾個好友名單 */
#define MAX_FRIEND      30

int     max_friend      = MAX_FRIEND;

void            extract_a_friend args( ( CHAR_DATA *, const char * ) );

FUNCTION( do_friend )
{
  FRIEND_DATA * pFriend;
  BASIC_DATA  * pBasic;
  CLASS_DATA  * pClass;
  CHAR_DATA   * victim;
  CHAR_DATA   * vch;
  char          arg[MAX_INPUT_LENGTH];
  char          buf[MAX_STRING_LENGTH];
  int           count;

  PUSH_FUNCTION( "do_friend" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  argument = one_argument( argument, arg );

  if ( arg[0] == '+' )
  {
    if ( arg[1] == '\x0' )
    {
      send_to_char( "你要加入哪一位玩家為好友名單﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( is_friend( ch, arg + 1 ) )
    {
      act( "$t本來就在你的好友名單之中。", ch, arg + 1, NULL, TO_CHAR );
      RETURN_NULL();
    }

    smash_tilde( arg + 1 );

    if ( str_len( arg + 1 ) > sizeof( pFriend->name ) )
    {
      send_to_char( "你的好友名稱長度不合法﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !str_cmp( ch->name, arg + 1 ) )
    {
      send_to_char( "你要把自己當作好朋友嗎﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( friend_count( ch ) >= max_friend && !IS_IMMORTAL( ch ) )
    {
      act( "你的好友名單超過$i個﹐請先刪除一些吧﹗", ch, &max_friend, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( !is_exist( arg + 1 ) )
    {
      act( "整個宇宙之中﹐找不到這位 $t 喔﹗", ch, arg + 1, NULL, TO_CHAR );
      RETURN_NULL();
    }

    pFriend = alloc_struct( STRUCT_FRIEND_DATA );
    str_cpy( pFriend->name, arg + 1 );
    pFriend->next = ch->friend;
    ch->friend    = pFriend;

    act( "你把$t設為你的好友名單之中。", ch, arg + 1, NULL, TO_CHAR );
    RETURN_NULL();
  }

  else if ( arg[0] == '-' )
  {
    if ( arg[1] == '\x0' )
    {
      send_to_char( "你要刪除哪一位為好友名單﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( !is_friend( ch, arg + 1 ) )
    {
      act( "$t 本來就不在你的好友名單之中囉﹗", ch, arg + 1, NULL, TO_CHAR );
      RETURN_NULL();
    }

    act( "你把$t從你的好友名單中移除了﹗", ch, arg + 1, NULL, TO_CHAR );
    extract_a_friend( ch, arg + 1 );
    RETURN_NULL();
  }

  else
  {
    if ( arg[0] == '\x0' )
    {
      victim = ch;
    }
    else
    {
      if ( !IS_IMMORTAL( ch ) )
      {
        send_to_char( "你的權力不足以觀看其他人的好友名單﹗\n\r", ch );
        RETURN_NULL();
      }

      if ( !( victim = get_char_world( ch, arg ) ) )
      {
        act( "找不到你要觀看的對象 $2$T$0。", ch, NULL, arg, TO_CHAR );
        RETURN_NULL();
      }

      if ( IS_NPC( victim ) )
      {
        act( "非玩家$N是沒有好友名單的。", ch, NULL, victim, TO_CHAR );
        RETURN_NULL();
      }

      if ( get_trust( ch ) < get_trust( victim ) )
      {
        act( "你的權力不夠看$N的好友名單。", ch, NULL, victim, TO_CHAR );
        RETURN_NULL();
      }
    }

    if ( !victim->friend )
    {
      act( "$N並沒有任何的好友名單。", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    clear_buffer();
    count = 0;

    send_to_buffer(
      "\e[1;33;44m順序 上線 名字         中文名字                        "
      " 等級 年齡 性別 職  業  \e[0m\n\r" );

    for ( pFriend = victim->friend; pFriend; pFriend = pFriend->next )
    {
      if ( ( pBasic = name2basic( pFriend->name ) ) )
      {
        pClass = is_class( pBasic->class, TRUE );
        vch = is_online( pFriend->name );

        tablize( FORMAT_SET,
          "LOOP ONLI NAME         CNAME                            \e[0mLEVV "
          "AGEE SEXX CLASSS \e[0m\n\r"
          , 0, NULL, 0 );

        tablize( FORMAT_STRING, pFriend->name  , 0, "NAME" , 12 );
        tablize( FORMAT_STRING, pBasic->cname  , 0, "CNAME", 32 );
        tablize( FORMAT_STRING, YESNO( vch && can_see( ch, vch ) ), 0, "ONLI" ,  4 );
        tablize( FORMAT_STRING, class_name( pClass, FALSE )
          , 0, "CLASSS" , 6 );

        tablize( FORMAT_NUM, NULL, ++count      , "LOOP" ,  4 );
        tablize( FORMAT_NUM, NULL, pBasic->level, "LEVV" ,  4 );

        tablize( FORMAT_NUM   , NULL
          , ( pBasic->played / 14400 ) + BornAge, "AGEE",  4 );

        tablize( FORMAT_STRING, sex_name( pBasic->sex ), 0, "SEXX", 4 );
        tablize( FORMAT_RETURN, buf, 0, NULL, sizeof( buf ) - 1 );
        send_to_buffer( "%s", buf );

        free_basic( pBasic );

        if ( buffer_full() ) break;
      }
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  RETURN_NULL();
}

/* 銷毀好友名單 */
void extract_friend( CHAR_DATA * ch )
{
  FRIEND_DATA * pFriend;
  FRIEND_DATA * zFriend;

  PUSH_FUNCTION( "extract_friend" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "extract_friend: 來源不正確." );
    RETURN_NULL();
  }

  for ( pFriend = ch->friend; pFriend; pFriend = zFriend )
  {
    zFriend = pFriend->next;
    free_struct( pFriend, STRUCT_FRIEND_DATA );
  }

  ch->friend = NULL;
  RETURN_NULL();
}

void extract_a_friend( CHAR_DATA * ch, const char * name )
{
  FRIEND_DATA * pFriend;
  FRIEND_DATA * zFriend;

  PUSH_FUNCTION( "extract_a_friend" );

  if ( !ch || !name || !*name )
  {
    mudlog( LOG_DEBUG, "extract_a_friend: 缺乏來源." );
    RETURN_NULL();
  }

  for ( zFriend = NULL, pFriend = ch->friend;
        pFriend; zFriend = pFriend, pFriend = pFriend->next )
  {
    if ( !str_cmp( pFriend->name, name ) )
    {
      if ( !zFriend ) ch->friend    = ch->friend->next;
      else            zFriend->next = pFriend->next;
      free_struct( pFriend, STRUCT_FRIEND_DATA );
      RETURN_NULL();
    }
  }

  mudlog( LOG_DEBUG, "extract_a_friend: 沒發現要銷毀的結構." );
  RETURN_NULL();
}

size_t friend_count( CHAR_DATA * ch )
{
  FRIEND_DATA * pFriend;
  size_t        count = 0;

  PUSH_FUNCTION( "friend_count" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "friend_count: 來源不正確." );
    RETURN( 0 );
  }

  for ( pFriend = ch->friend; pFriend; pFriend = pFriend->next ) count++;

  RETURN( count );
}

FRIEND_DATA * is_friend( CHAR_DATA * ch, const char * name )
{
  FRIEND_DATA * pFriend;

  PUSH_FUNCTION( "is_friend" );

  if ( !ch || !name || !*name )
  {
    mudlog( LOG_DEBUG, "is_friend: 來源不正確." );
    RETURN( NULL );
  }

  for ( pFriend = ch->friend; pFriend; pFriend = pFriend->next )
    if ( !str_cmp( pFriend->name, name ) ) RETURN( pFriend );

  RETURN( NULL );
}

void friend_msg( CHAR_DATA * ch )
{
  DESCRIPTOR_DATA * man;
  FRIEND_DATA     * pFriend;
  CHAR_DATA       * victim;

  PUSH_FUNCTION( "friend_msg" );

  if ( !ch || IS_NPC( ch ) )
  {
    mudlog( LOG_DEBUG, "friend_msg: 來源有問題." );
    RETURN_NULL();
  }

  for ( man = descriptor_list; man; man = man->next )
  {
    if ( verify_desc( man )
      && man->connected == CON_PLAYING
      && verify_char( ( victim = man->character ) ) )
    {
      for ( pFriend = victim->friend; pFriend; pFriend = pFriend->next )
      {
        if ( !str_cmp( pFriend->name, ch->name ) )
        {
          act( "你的好友$N上線囉﹗\a\a", victim, NULL, ch, TO_CHAR );
          RETURN_NULL();
        }
      }
    }
  }

  RETURN_NULL();
}

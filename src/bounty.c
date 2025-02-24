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

int     NowBounty       =       0;
int     BountyLimit     =      10;

FUNCTION( do_bounty )
{
  BOUNTY_DATA     * pBounty;
  ROOM_INDEX_DATA * pRoom;
  CHAR_DATA       * pMob;
  char              arg[MAX_INPUT_LENGTH];
  char              buf[MAX_STRING_LENGTH];
  char              buf1[MAX_STRING_LENGTH];
  int               loop;
  int               slot;

  PUSH_FUNCTION( "do_bounty" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  if ( !bounty_list )
  {
    act( "對不起﹐$t沒有任何的懸賞資料﹗", ch, mud_name, NULL, TO_CHAR );
    RETURN_NULL();
  }

  argument = one_argument( argument ,arg );

  if ( arg[0] == '\x0' )
  {
    if ( NowBounty <= 0 )
    {
      act( "目前$t沒有任何懸賞的名單﹐請耐心地等候吧﹗", ch, mud_name, NULL, TO_CHAR );
      RETURN_NULL();
    }

    clear_buffer();
    for ( loop = 0, pMob = char_list; pMob; pMob = pMob->next )
    {
      if ( !verify_char( pMob )
        || !( pBounty = pMob->bounty )
        || !pMob->in_room
        || !pMob->in_room->area
        || !pMob->in_room->area->name )
        continue;

      if ( loop == 0 )
        send_to_buffer( "\e[1;33;44m順序 所在區域名稱 怪物名稱             "
          "      獎賞                              \e[0m\n\r" );

      chinese_number( pBounty->value, buf );
      send_to_buffer( "%4d %-12s %-26s %s%s%s\n\r"
        , ++loop
        , pMob->in_room->area->name
        , mob_name( ch, pMob )
        , bounty_type( pBounty->type )
        , buf
        , bounty_unit( pBounty->type ) );

      if ( buffer_full() ) break;
    }

    if ( loop == 0 )
      send_to_buffer( "目前沒有任何懸賞的名單﹐請耐心地等候吧﹗\n\r" );

    if ( loop != NowBounty ) mudlog( LOG_DEBUG, "do_bounty: 總數不符合." );

    print_buffer( ch );
  }

  else if ( !str_prefix( arg, "lock" ) && IS_IMMORTAL( ch ) )
  {
    one_argument( argument, arg );

    if ( arg[0] == '\x0' || !is_number( arg ) )
    {
      send_to_char( "你想鎖定第幾個懸賞呢﹖\n\r", ch );
      RETURN_NULL();
    }

    slot = atoi( arg );
    loop = 1;
    for ( pBounty = bounty_list; pBounty; pBounty = pBounty->next, loop++ )
    {
      if ( slot == loop )
      {
        if ( pBounty->lock )
        {
          pBounty->lock = FALSE;
          act( "你解除了第$i號的懸賞鎖定﹗", ch, &slot, NULL, TO_CHAR );
        }

        else
        {
          pBounty->lock = TRUE;
          act( "你設定了第$i號的懸賞鎖定﹗", ch, &slot, NULL, TO_CHAR );

          for ( pMob = char_list; pMob; pMob = pMob->next )
          {
            if ( verify_char( pMob ) && pMob->bounty == pBounty )
            {
              pMob->bounty = NULL;
              pBounty->count--;
              pBounty->killed++;
              NowBounty--;

              act( "$n不再是被懸賞的人了﹗", pMob, NULL, NULL, TO_ROOM );
            }
          }
        }

        RETURN_NULL();
      }
    }

    send_to_char( "對不起﹐沒有那個順序的資料﹗\n\r", ch );
  }

  else if ( !str_prefix( arg, "go" ) && IS_IMMORTAL( ch ) )
  {
    one_argument( argument, arg );
    if ( arg[0] == '\x0' || !is_number( arg ) )
    {
      send_to_char( "你想讓第幾個懸賞立刻啟動﹖\n\r", ch );
      RETURN_NULL();
    }

    slot = atoi( arg );
    loop = 1;
    for ( pBounty = bounty_list; pBounty; pBounty = pBounty->next, loop++ )
    {
      if ( loop == slot )
      {
        if ( pBounty->lock )
        {
          send_to_char( "這項資料被鎖定﹐所以無法啟動﹗\n\r", ch );
          RETURN_NULL();
        }

        if ( !pBounty->mob || !pBounty->msg || !*pBounty->msg )
        {
          send_to_char( "資料不完全﹐所以無法啟動﹗\n\r", ch );
          RETURN_NULL();
        }

        if ( pBounty->count >= pBounty->max )
        {
          send_to_char( "數量已達上限﹐所以無法啟動。\n\r", ch );
          RETURN_NULL();
        }

        if ( !( pRoom = random_room( get_room_area( pBounty->room ) ) ) )
        {
          send_to_char( "找不到出現的房間﹐啟動失敗﹗\n\r", ch );
          RETURN_NULL();
        }

        if ( !( pMob = shape_mobile( pBounty->mob, pRoom ) ) )
        {
          send_to_char( "對不起﹐無法產生怪物﹐觸發失敗﹗\n\r", ch );
          RETURN_NULL();
        }

        NowBounty++;
        pBounty->count++;
        pBounty->occurred++;
        pMob->bounty = pBounty;
        pMob->gold   = 0;

        chinese_number( pBounty->value, buf1 );
        sprintf( buf, "%s﹐凡是可以除掉%s\e[1;33m的人﹐都可以領賞%s%s%s﹗"
          , pBounty->msg
          , mob_name( NULL, pMob )
          , bounty_type( pBounty->type )
          , buf1
          , bounty_unit( pBounty->type ) );

        talk_channel_2( buf, CHANNEL_BULLETIN, "懸賞" );

        send_to_char( "你手動觸發這次的懸賞事件﹗\n\r", ch );
        RETURN_NULL();
      }
    }

    send_to_char( "對不起﹐沒有那個順序的資料﹗\n\r", ch );
  }

  else if ( !str_prefix( arg, "status" ) && IS_IMMORTAL( ch ) )
  {
    one_argument( argument, arg );

    if ( arg[0] == '\x0' )
    {
      clear_buffer();
      send_to_buffer( "\e[1;33;44m順序 怪 物 名 稱          號 碼 機率"
        " 房 間 數量 Max 型    態 數值 遷徙 發生 被殺\e[0m\n\r" );

      loop = 0;
      for ( pBounty = bounty_list; pBounty; pBounty = pBounty->next, loop++ )
      {
        if ( !pBounty->mob ) continue;
        send_to_buffer( "%4d %-20s %5d %4d %5d %4d %3d %-8s %4d %4d %4d %4d\n\r"
          , loop + 1
          , mob_index_name( NULL, pBounty->mob )
          , pBounty->mob->vnum
          , pBounty->chance
          , pBounty->room
          , pBounty->count
          , pBounty->max
          , bounty_type( pBounty->type )
          , pBounty->value
          , pBounty->migration
          , pBounty->occurred
          , pBounty->killed );

        if ( buffer_full() ) break;
      }

      send_to_buffer( "\n\r至少要 %d 人以上才會開啟懸賞事件﹗\n\r"
        , BountyLimit );

      print_buffer( ch );
    }

    else if ( !is_number( arg ) )
    {
      send_to_char( "你的順序必須是號碼﹗\n\r", ch );
      RETURN_NULL()
    }

    else
    {
      if ( ( slot = atoi( arg ) ) <= 0 )
      {
        send_to_char( "你的順序數字不合理﹗\n\r", ch );
        RETURN_NULL();
      }

      loop = 1;
      for ( pBounty = bounty_list; pBounty; pBounty = pBounty->next, loop++ )
      {
        if ( loop == slot && pBounty->mob && pBounty->msg )
        {
          print_to_char( ch,
            "順    序﹕ %d\n\r"
            "怪物號碼﹕ %d\n\r"
            "怪物名稱﹕ %s\n\r"
            "出現房間﹕ %d\n\r"
            "出現機率﹕ 千分之 %d\n\r"
            "目前數量﹕ %d\n\r"
            "最大數量﹕ %d\n\r"
            "獎賞型態﹕ %s\n\r"
            "獎賞數值﹕ %d\n\r"
            "遷徙機率﹕ 百分之 %d\n\r"
            "產生次數﹕ %d 次\n\r"
            "被 完 成﹕ %d 次\n\r"
            "被 鎖 定﹕ %s\n\r"
            "輸出敘述﹕\n\r%s\n\r"
            , loop
            , pBounty->mob->vnum
            , mob_index_name( NULL, pBounty->mob )
            , pBounty->room
            , pBounty->chance
            , pBounty->count
            , pBounty->max
            , bounty_type( pBounty->type )
            , pBounty->value
            , pBounty->migration
            , pBounty->occurred
            , pBounty->killed
            , YESNO( pBounty->lock )
            , pBounty->msg );

          RETURN_NULL();
        }
      }

      send_to_char( "對不起﹐沒有那個順序的資料﹗\n\r", ch );
    }

    RETURN_NULL();
  }

  else
  {
    send_to_char( "你的語法錯誤﹐請查詢 bounty 的正確使用方法﹗\n\r", ch );
  }

  RETURN_NULL();
}

void bounty_update( void )
{
  BOUNTY_DATA     * pBounty;
  ROOM_INDEX_DATA * pRoom;
  CHAR_DATA       * pMob;
  char              buf[MAX_STRING_LENGTH];
  char              buf1[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "bounty_update" );

  if ( get_usernum() < BountyLimit ) RETURN_NULL();

  /* 系統重置, 關閉 */
  if ( ( reboot_time   > 0 && ( ( reboot_time   - current_time ) < 300 ) )
    || ( shutdown_time > 0 && ( ( shutdown_time - current_time ) < 300 ) ) )
    RETURN_NULL();

  for ( pBounty = bounty_list; pBounty; pBounty = pBounty->next )
  {
    if ( pBounty->lock
      || pBounty->count >= pBounty->max
      || number_range( 1, 1000 ) > pBounty->chance
      || !( pRoom = random_room( get_room_area( pBounty->room ) ) )
      || !pBounty->mob
      || !pBounty->msg
      || !*pBounty->msg
      || !( pMob = shape_mobile( pBounty->mob, pRoom ) ) ) continue;

    NowBounty++;
    pBounty->count++;
    pBounty->occurred++;
    pMob->bounty = pBounty;
    pMob->gold   = 0;

    chinese_number( pBounty->value, buf1 );
    sprintf( buf, "%s﹐凡是可以除掉%s\e[1;33m的人﹐都可以領賞%s%s%s﹗"
      , pBounty->msg
      , mob_name( NULL, pMob )
      , bounty_type( pBounty->type )
      , buf1
      , bounty_unit( pBounty->type ) );

    talk_channel_2( buf, CHANNEL_BULLETIN, "懸賞" );
  }

  if ( NowBounty <= 0 ) RETURN_NULL();

  for ( pMob = char_list; pMob; pMob = pMob->next )
  {
    if ( !verify_char( pMob )
      || !IS_NPC( pMob )
      || !( pBounty = pMob->bounty )
      || number_range( 1, 100 ) > pBounty->migration
      || !( pRoom = random_room( get_room_area( pBounty->room ) ) )
      || !pMob->in_room
      || pMob->mount_by
      || pMob->mount
      || pMob->boss
      || pMob->talk_buf
      || pMob->position != POS_STANDING ) continue;

    act( "$n周圍出現一團紫氣﹐不久﹐哪還有$n的身影呢﹖"
       , pMob, NULL, NULL, TO_ROOM );

    char_from_room( pMob );
    char_to_room( pMob, pRoom );
    act( "不好意思﹐$n來借住一下。", pMob, NULL, NULL, TO_ROOM );
  }

  RETURN_NULL();
}

void check_bounty( CHAR_DATA * ch, CHAR_DATA * victim )
{
  BOUNTY_DATA * pBounty;
  char          buf[MAX_STRING_LENGTH];
  char          buf1[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "check_bounty" );

  if ( !ch || !victim || !verify_char( ch ) || !verify_char( victim ) )
  {
    mudlog( LOG_DEBUG, "check_bounty: 來源不正確." );
    RETURN_NULL();
  }

  if ( IS_NPC( ch ) || !IS_NPC( victim ) || !( pBounty = victim->bounty ) )
    RETURN_NULL();

  pBounty->killed++;
  pBounty->count--;
  NowBounty--;
  victim->bounty = NULL;

  switch( pBounty->type )
  {
  default:
    mudlog( LOG_DEBUG, "check_bounty: 錯誤的型態 %d.", pBounty->type );
    RETURN_NULL();

  case BOUNTY_GOLD:

    if ( pBounty->value <= 0 )
    {
      mudlog( LOG_DEBUG, "check_bounty: 錯誤的數值 %d.", pBounty->value );
      RETURN_NULL();
    }

    if ( !over_scale( ch ) ) gold_to_char( ch, pBounty->value );
    else send_to_char( "對不起﹐你的總資產已達上限﹐所以沒拿到錢﹗\n\r", ch );
    break;

  case BOUNTY_FIRMAN:

    if ( pBounty->value <= 0 )
    {
      mudlog( LOG_DEBUG, "check_bounty: 錯誤的數值 %d.", pBounty->value );
      RETURN_NULL();
    }

    add_firman( ch, pBounty->value );
    break;
  }

  chinese_number( pBounty->value, buf1 );
  sprintf( buf, "因為%s\e[1;33m(%s)除掉了懸賞中的%s﹐"
    "所以特地獎賞%s%s%s﹗\e[0m"
    , ch->cname
    , ch->name
    , mob_name( NULL, victim )
    , bounty_type( pBounty->type )
    , buf1
    , bounty_unit( pBounty->type ) );

  talk_channel_2( buf, CHANNEL_BULLETIN, "公告" );

  print_to_char( ch
    , "因為你除掉了懸賞中的%s﹐所以特地獎賞你%s%s%s﹗\e[0m\n\r"
    , mob_name( NULL, victim )
    , bounty_type( pBounty->type )
    , buf1
    , bounty_unit( pBounty->type ) );

  RETURN_NULL();
}

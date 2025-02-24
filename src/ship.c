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

FUNCTION( do_ship )
{
  char        arg[MAX_INPUT_LENGTH];
  char        arg1[MAX_INPUT_LENGTH];
  char        arg2[MAX_INPUT_LENGTH];
  char        buf[MAX_STRING_LENGTH];
  char        buf1[MAX_STRING_LENGTH];
  SHIP_DATA * pShip;
  AREA_DATA * pArea;
  int         count;
  int         slot;

  PUSH_FUNCTION( "do_ship" );

  if ( !ship_list )
  {
    act( "$t沒有任何的交通工具資料﹗", ch, mud_name, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( IS_NPC( ch ) ) RETURN_NULL();

  argument = one_argument( argument, arg );

  if ( !arg[0] )
  {
    clear_buffer();
    send_to_buffer( "\e[1;33;44m出 發 區 域 出   發   點 "
      "目 的 區 域 目   的   地 費用 時 間 航   行   狀    態\e[0m\n\r" );

    for ( count = 1, pShip = ship_list; pShip; pShip = pShip->next, count++ )
    {
      if ( pShip->lock ) continue;

      send_to_buffer( "%-11s %-12s %-11s %-12s %4d %2d:%2d %-17s\n\r"
        , ( pShip->starting
          && ( pArea = pShip->starting->area )
          && pArea->name ) ? pArea->name : "不知名區域"
        , pShip->starting && pShip->starting->name
          ? pShip->starting->name : "不知名地方"
        , ( pShip->destination
          && ( pArea = pShip->destination->area )
          && pArea->name ) ? pArea->name : "不知名區域"
        , pShip->starting && pShip->destination->name
          ? pShip->destination->name : "不知名地方"
        , pShip->cost, pShip->sailing, pShip->waiting
        , ship_status( pShip ) );
    }

    for ( pShip = ship_list; pShip; pShip = pShip->next )
    {
      if ( pShip->lock ) continue;

      else if ( pShip->starting == ch->in_room )
      {
        chinese_number( pShip->sailing, buf );
        chinese_number( pShip->cost, buf1 );

        send_to_buffer(
          "\n\r你正在%s的出發點上﹐它的目的地是%s的%s﹐航行時間估計"
          "約為%s小時﹗\n\r而且航費高貴不貴﹐只花你%s兩銀子。\n\r"
          , pShip->name
          , pShip->destination && pShip->destination->area
            ? pShip->destination->area->name : "某不知名區域"
          , pShip->destination ? pShip->destination->name : "某不知名地方"
          , buf, buf1 );

        if ( pShip->waiting_tick > 0 )
        {
          chinese_number( pShip->waiting_tick, buf );

          send_to_buffer( "不過請你盡快登上交通工具﹐因為%s將在%s小時後出發。\n\r"
            , pShip->name, buf );
        }
        else
        {
          chinese_number( pShip->sailing_tick, buf );

          send_to_buffer( "但是%s已經出發了﹐下班交通工具將在%s小時後出發。\n\r"
            , pShip->name, buf );
        }

        if ( pShip->pirate >= 1000 )
          send_to_buffer( "不過﹐提醒你這一帶強盜頻傳﹐奉勸你小心喔﹗\n\r" );

        else if ( pShip->pirate >= 500 )
          send_to_buffer( "聽說﹐強盜曾在這一帶出沒﹐請你要多加小心﹗\n\r" );

        else if ( pShip->pirate >= 100 )
          send_to_buffer( "傳聞﹐這裡層出現過強盜﹐請你要多多小心﹗\n\r" );

        else if ( pShip->pirate >= 30 )
          send_to_buffer( "雖然強盜很少在此出沒﹐但是還是小心的好﹗\n\r" );

        else
          send_to_buffer( "強盜極少在這一帶出沒﹐應該是風平浪靜才對吧﹗\n\r" );
      }

      else if ( pShip->cabin == ch->in_room )
      {
        chinese_number( pShip->sailing, buf );

        send_to_buffer( "\n\r你正在%s的甲板裡﹐它的目的地是%s的%s﹐"
          "航行時間約為%s小時﹗\n\r"
          , pShip->name
          , pShip->destination && pShip->destination->area
            ? pShip->destination->area->name : "某不知名區域"
          , pShip->destination ? pShip->destination->name : "某不知名地方"
          , buf );

        if ( pShip->waiting_tick > 0 )
        {
          chinese_number( pShip->waiting_tick, buf );
          send_to_buffer( "請你稍做休息﹐交通工具將在%s小時後出發﹗謝謝﹗\n\r"
            , buf );
        }
        else
        {
          chinese_number( pShip->sailing_tick, buf );
          send_to_buffer( "請你耐心等候﹐交通工具將在%s小時後到達目的地﹗謝謝﹗\n\r"
            , buf );
        }

        if ( pShip->pirate >= 1000 )
          send_to_buffer( "不過﹐提醒你這一帶強盜頻傳﹐奉勸你小心喔﹗\n\r" );

        else if ( pShip->pirate >= 500 )
          send_to_buffer( "聽說﹐強盜曾在這一帶出沒﹐請你要多加小心﹗\n\r" );

        else if ( pShip->pirate >= 100 )
          send_to_buffer( "傳聞﹐這裡層出現過強盜﹐請你要多多小心﹗\n\r" );

        else if ( pShip->pirate >= 30 )
          send_to_buffer( "雖然強盜很少在此出沒﹐但是還是小心的好﹗\n\r" );

        else
          send_to_buffer( "強盜極少在這一帶出沒﹐應該是風平浪靜才對吧﹗\n\r" );
      }
    }

    print_buffer( ch );
  }

  else if ( !str_cmp( arg, "on" ) )
  {
    for ( pShip = ship_list; pShip; pShip = pShip->next )
      if ( pShip->starting == ch->in_room ) break;

    if ( !pShip || IS_NPC( ch ) )
    {
      act( "在$r是搭不到交通工具的﹐你被騙了吧﹗", ch, NULL, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( pShip->lock )
    {
      send_to_char( "交通工具老大跑路去了﹐這艘交通工具沒有辦法為你服務了﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( pShip->waiting_tick <= 0 || pShip->sailing_tick > 0 )
    {
      send_to_char( "對不起﹐這艘交通工具還沒有靠岸耶﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( ( reboot_time   > 0 && ( ( reboot_time   - current_time ) < 600 ) )
      || ( shutdown_time > 0 && ( ( shutdown_time - current_time ) < 600 ) ) )
    {
      act( "$t即將關閉﹐所以沒有辦法搭上交通工具。", ch, mud_name, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch->gold < pShip->cost )
    {
      send_to_char( "等你有足夠的錢再來搭吧﹐這裡可不是慈善事業﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->position != POS_STANDING )
    {
      send_to_char( "你還在忙咧﹐待會再搭吧﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( get_mount( ch ) )
    {
      send_to_char( "對不起﹐為維護整潔﹐嚴禁動物入內﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->spirit )
    {
      act( "你帶著$N是會嚇到其他旅客的﹗", ch, NULL, ch->spirit, TO_CHAR );
      RETURN_NULL();
    }

    if ( ( IS_SET( ch->act, PLR_KILLER ) || IS_SET( ch->act, PLR_THIEF ) )
      && !IS_IMMORTAL( ch ) )
    {
      send_to_char( "根據上級規定﹐我們是不能幫你偷渡的﹗\n\r", ch );
      RETURN_NULL();
    }

    /* 扣錢 */
    gold_from_char( ch, pShip->cost );
    pShip->count++;

    /* 搬移人物 */
    act( "$n付了錢﹐登上了$t。", ch, pShip->name, NULL, TO_ALL );

    char_from_room( ch );
    char_to_room( ch, pShip->cabin );
    act( "$n付了錢﹐緩緩地從甲板那端出現。", ch, NULL, NULL, TO_ROOM );
    do_look( ch, "auto" );

    /* 清除追蹤紀錄點 */
    clear_trace( ch, TRUE );
  }

  else if ( !str_cmp( arg, "off" ) )
  {
    for ( pShip = ship_list; pShip; pShip = pShip->next )
      if ( pShip->cabin == ch->in_room ) break;

    if ( !pShip || IS_NPC( ch ) )
    {
      send_to_char( "你又沒有在交通工具上﹐怎麼能夠下去呢﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( pShip->waiting_tick <= 0 )
    {
      send_to_char( "對不起﹐交通工具已經出發了﹐所以沒有辦法下去了﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->position != POS_STANDING )
    {
      send_to_char( "你還在忙咧﹐所以沒有辦法下去交通工具哩﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( get_mount( ch ) )
    {
      send_to_char( "甲板不大﹐所以請你先從你的坐騎上下來吧﹗\n\r", ch );
      RETURN_NULL();
    }

    /* 還錢 */
    gold_to_char( ch, pShip->cost / 2 );
    pShip->count = UMAX( 0, pShip->count - 1 );

    /* 搬移人物 */
    act( "交通工具老大把渡資還給$n﹐$n緩緩地走下$t了﹗"
      , ch, pShip->name, NULL, TO_ALL );

    char_from_room( ch );
    char_to_room( ch, pShip->starting );
    act( "$n拎著包包﹐從$t走了下來。", ch, pShip->name, NULL, TO_ROOM );
    do_look( ch, "auto" );

    /* 清除追蹤紀錄點 */
    clear_trace( ch, TRUE );
  }

  else if ( is_number( arg ) && IS_IMMORTAL( ch ) )
  {
    slot = atoi( arg );
    for ( count = 0, pShip = ship_list; pShip; pShip = pShip->next )
    {
      if ( ++count == slot )
      {
        print_to_char( ch,
          "編號          ﹕%d\n\r"
          "船隻名稱      ﹕%s\n\r"
          "出發點房間號碼﹕%d\n\r"
          "目的地房間號碼﹕%d\n\r"
          "船艙房間號碼  ﹕%d\n\r"
          "船艙的敘述    ﹕%s"
          "渡船費用      ﹕%d 兩銀子\n\r"
          "靠站敘述      ﹕%s"
          "進站敘述      ﹕%s"
          "渡船人數      ﹕%d\n\r"
          "強盜機率      ﹕萬分之%d\n\r"
          "強盜出現次數  ﹕%d\n\r"
          "延期機率      ﹕萬分之%d\n\r"
          "航行時間      ﹕%d\n\r"
          "待機時間      ﹕%d\n\r"
          "目前航行時間  ﹕%d\n\r"
          "目前等待時間  ﹕%d\n\r"
          "目前狀態      ﹕%s\n\r"
          "鎖定          ﹕%s\n\r"
          , count
          , pShip->name && *pShip->name ? pShip->name : "從缺"
          , pShip->starting    ? pShip->starting->vnum    : -1
          , pShip->destination ? pShip->destination->vnum : -1
          , pShip->cabin       ? pShip->cabin->vnum       : -1
          , pShip->description ? pShip->description : "從缺"
          , pShip->cost
          , pShip->msg_land     ? pShip->msg_land     : "從缺"
          , pShip->msg_entrance ? pShip->msg_entrance : "從缺"
          , pShip->count, pShip->pirate, pShip->pirate_count
          , pShip->delay, pShip->sailing, pShip->waiting
          , pShip->sailing_tick, pShip->waiting_tick
          , ship_status( pShip )
          , YESNO( pShip->lock ) );
        RETURN_NULL();
      }
    }

    send_to_char( "沒有這個編號的船隻﹗\n\r", ch );
  }

  else if ( !str_prefix( arg, "info" ) && IS_IMMORTAL( ch ) )
  {
    clear_buffer();
    send_to_buffer( "\e[1;33;44m順序 名        稱 起始點 目的地 船  艙 "
       "花  費 人  數 海  盜 次  數 鎖定\e[0m\n\r" );

    for ( count = 0, pShip = ship_list; pShip; pShip = pShip->next )
    {
      send_to_buffer( "%4d %12s %6d %6d %6d %6d %6d %6d %6d %s\n\r"
        , ++count
        , pShip->name
        , pShip->starting    ? pShip->starting->vnum    : -1
        , pShip->destination ? pShip->destination->vnum : -1
        , pShip->cabin       ? pShip->cabin->vnum       : -1
        , pShip->cost
        , pShip->count, pShip->pirate, pShip->pirate_count
        , YESNO( pShip->lock ) );
    }

    print_buffer( ch );
  }

  else if ( !str_prefix( arg, "lock" ) && IS_IMMORTAL( ch ) )
  {
    argument = one_argument( argument, arg );
    if ( !arg[0] || !is_number( arg ) )
    {
      send_to_char( "你要鎖定或是解除鎖定那一隻船隻﹖\n\r", ch );
      RETURN_NULL();
    }

    slot = atoi( arg );

    for ( count = 0, pShip = ship_list; pShip; pShip = pShip->next )
    {
      if ( ++count == slot )
      {
        chinese_number( count, buf );
        pShip->lock = pShip->lock ? FALSE : TRUE;

        act( "你$t第$T艘船隻的狀態﹗"
          , ch, pShip->lock ? "鎖定" : "解除鎖定", buf, TO_CHAR );

        mudlog( LOG_WIZARD, "%s%s第%s艘船隻狀態."
          , ch->name, pShip->lock ? "鎖定" : "解除鎖定", buf );

        if ( !pShip->lock )
        {
          sendmsg_to_someroom( pShip->msg_entrance, pShip->starting );
          pShip->sailing_tick = -1;
          pShip->waiting_tick = pShip->waiting;
        }

        else
        {
          sendmsg_to_someroom( "啊﹗遭了﹐船老大跑路去了﹐這艘船即將關閉﹗"
            , pShip->cabin );

          sendmsg_to_someroom( "船老大跑路去了﹐你再也搭不到這艘船了﹗"
            , pShip->starting );
        }

        RETURN_NULL();
      }
    }

    send_to_char( "找不到你要的船隻號碼﹐動作取消﹗\n\r", ch );
  }

  else if ( !str_prefix( arg, "set" ) && IS_IMMORTAL( ch ) )
  {
    argument = one_argument( argument, arg  );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( !arg[0] || !arg1[0] || !arg2[0] )
    {
      send_to_char( "你要更改哪一艘船隻的哪些設定﹖請查詢 ship 用法\n\r", ch );
      RETURN_NULL();
    }

    if ( !is_number( arg ) )
    {
      send_to_char( "對不起﹐船隻的號碼必須是數字﹗\n\r", ch );
      RETURN_NULL();
    }

    slot = atoi( arg );
    for ( count = 0, pShip = ship_list; pShip; pShip = pShip->next )
    {
      if ( ++count == slot )
      {
        chinese_number( slot, buf );
        if ( !str_prefix( arg1, "name" ) )
        {
          free_string( pShip->name );
          pShip->name = str_dup( arg2 );
          act( "你更改第$t艘名稱為$T。", ch, buf, arg2, TO_CHAR );
          mudlog( LOG_DEBUG, "%s更改第%s艘船隻的名稱", ch->name, buf );
          RETURN_NULL();
        }

        else if ( !str_prefix( arg1, "cost" ) )
        {
          if ( !is_number( arg2 ) )
          {
            send_to_char( "渡船費用必須是數字﹗\n\r", ch );
            RETURN_NULL();
          }

          if ( ( slot = atoi( arg2 ) ) <= 0 || slot >= MAX_ASSET )
          {
            act( "渡船費用 $i 不合理﹗", ch, &slot, NULL, TO_CHAR );
            RETURN_NULL();
          }

          chinese_number( ( pShip->cost = slot ), buf1 );
          act( "你更改第$t艘船隻渡船費用為$T。", ch, buf, buf1, TO_CHAR );
          mudlog( LOG_WIZARD, "%s更改第%s艘船隻渡船費用為%s。\n\r"
            , ch->name, buf, buf1 );

          RETURN_NULL();
        }

        else if ( !str_prefix( arg1, "pirate" ) )
        {
          if ( !is_number( arg2 ) )
          {
            send_to_char( "遇到強盜機率必須是數字﹗\n\r", ch );
            RETURN_NULL();
          }

          if ( ( slot = atoi( arg2 ) ) < 0 || slot >= 10000 )
          {
            act( "強盜出現機率 $i 不合理﹗", ch, &slot, NULL, TO_CHAR );
            RETURN_NULL();
          }

          chinese_number( ( pShip->pirate = slot ), buf1 );
          act( "你更改第$t艘船隻強盜機率為萬分之$T。", ch, buf, buf1, TO_CHAR );
          mudlog( LOG_WIZARD, "%s更改第%s艘船隻強盜機率為萬分之%s。\n\r"
            , ch->name, buf, buf1 );

          RETURN_NULL();
        }

        else if ( !str_prefix( arg1, "delay" ) )
        {
          if ( !is_number( arg2 ) )
          {
            send_to_char( "延期的機率必須是數字﹗\n\r", ch );
            RETURN_NULL();
          }

          if ( ( slot = atoi( arg2 ) ) < 0 || slot >= 10000 )
          {
            act( "延期的機率 $i 不合理﹗", ch, &slot, NULL, TO_CHAR );
            RETURN_NULL();
          }

          chinese_number( ( pShip->pirate = slot ), buf1 );
          act( "你更改第$t艘船隻延期的機率為萬分之$T。", ch, buf, buf1, TO_CHAR );
          mudlog( LOG_WIZARD, "%s更改第%s艘船隻延期的機率為萬分之%s。\n\r"
            , ch->name, buf, buf1 );

          RETURN_NULL();
        }

        else if ( !str_prefix( arg1, "sailing" ) )
        {
          if ( !is_number( arg2 ) )
          {
            send_to_char( "航行時間必須是數字﹗\n\r", ch );
            RETURN_NULL();
          }

          if ( ( slot = atoi( arg2 ) ) <= 0 )
          {
            act( "航行時間 $i 不合理﹗", ch, &slot, NULL, TO_CHAR );
            RETURN_NULL();
          }

          chinese_number( ( pShip->sailing = slot ), buf1 );
          act( "你更改第$t艘船隻航行時間為$T小時。", ch, buf, buf1, TO_CHAR );
          mudlog( LOG_WIZARD, "%s更改第%s艘船隻航行時間為%s小時。\n\r"
            , ch->name, buf, buf1 );

          RETURN_NULL();
        }

        else if ( !str_prefix( arg1, "waiting" ) )
        {
          if ( !is_number( arg2 ) )
          {
            send_to_char( "待機時間必須是數字﹗\n\r", ch );
            RETURN_NULL();
          }

          if ( ( slot = atoi( arg2 ) ) <= 0 )
          {
            act( "待機時間 $i 不合理﹗", ch, &slot, NULL, TO_CHAR );
            RETURN_NULL();
          }

          chinese_number( ( pShip->waiting = slot ), buf1 );
          act( "你更改第$t艘船隻待機時間為$T小時。", ch, buf, buf1, TO_CHAR );
          mudlog( LOG_WIZARD, "%s更改第%s艘船隻待機時間為%s小時。\n\r"
            , ch->name, buf, buf1 );

          RETURN_NULL();
        }

        else if ( !str_prefix( arg1, "entrance" ) )
        {
          free_string( pShip->msg_entrance );
          pShip->msg_entrance = str_dup( arg2 );
          act( "你更改第$t艘進港敘述為$T。", ch, buf, arg2, TO_CHAR );
          mudlog( LOG_DEBUG, "%s更改第%s艘船隻的進港敘述", ch->name, buf );
          RETURN_NULL();
        }

        else if ( !str_prefix( arg1, "land" ) )
        {
          free_string( pShip->msg_land );
          pShip->msg_land = str_dup( arg2 );
          act( "你更改第$t艘靠港敘述為$T。", ch, buf, arg2, TO_CHAR );
          mudlog( LOG_DEBUG, "%s更改第%s艘船隻的靠港敘述", ch->name, buf );
          RETURN_NULL();
        }

        send_to_char( "設定的選項錯誤﹐請查詢 ship 用法﹗\n\r", ch );
        RETURN_NULL();
      }
    }

    send_to_char( "找不到你要的船隻號碼﹐動作取消﹗\n\r", ch );
  }

  else
  {
    send_to_char( "參數錯誤﹐請查詢 ship 的使用方法﹗\n\r", ch );
  }

  RETURN_NULL();
}

void ship_update( void )
{
  SHIP_DATA * pShip;
  CHAR_DATA * ch;
  CHAR_DATA * ch_next;
  OBJ_DATA  * obj;
  OBJ_DATA  * obj_next;
  char        buf[MAX_STRING_LENGTH];
  char        buf1[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "ship_update" );

  if ( !ship_list ) RETURN_NULL();

  for ( pShip = ship_list; pShip; pShip = pShip->next )
  {
    if ( pShip->waiting <= 0 || pShip->sailing <= 0 )
    {
      mudlog( LOG_DEBUG, "ship_update: 船隻初始計時器錯誤." );
      pShip->waiting = 2;
      pShip->sailing = 10;

      if ( pShip->lock )
      {
        pShip->sailing_tick = -1;
        pShip->waiting_tick = -1;
      }
      else
      {
        sendmsg_to_someroom( pShip->msg_entrance, pShip->starting );
        pShip->waiting_tick = pShip->waiting;
        pShip->sailing_tick = -1;
      }

      continue;
    }

    if ( pShip->lock )
    {
      if ( pShip->cabin )
      {
        mudlog( LOG_DEBUG, "ship_update: 船艙房間不正確." );
      }

      else
      {
        for ( ch = pShip->cabin->people; ch; ch = ch_next )
        {
          ch_next = ch->next_in_room;

          if ( ch->fighting ) stop_fighting( ch, TRUE );

          if ( ch->position == POS_RESTING || ch->position == POS_SLEEPING )
          {
            send_to_char( "喂﹐船到了﹐該起床了。\n\r" , ch );

            /* 如果是昏睡 */
            if ( !is_affected( ch, SLOT_SLEEP ) ) ch->position = POS_STANDING;
          }

          send_to_char( "船老大不做你的生意了﹐滾吧﹗\n\r", ch );
          act( "$n被船老大從$t趕了下去。", ch, pShip->name, NULL, TO_ROOM );
          char_from_room( ch );
          char_to_room( ch, pShip->destination );

          act( "$n被船老大從$t上趕了下來。", ch, pShip->name, NULL, TO_ROOM );
          do_look( ch, "auto" );

          /* 清除追蹤紀錄點 */
          clear_trace( ch, TRUE );
        }

        pShip->waiting_tick = -1;
        pShip->sailing_tick = -1;
        continue;
      }
    }

    if ( ( pShip->waiting_tick < 0 && pShip->sailing_tick < 0 )
      || ( pShip->waiting_tick > 0 && pShip->sailing_tick > 0 ) )
    {
      mudlog( LOG_DEBUG, "ship_update: 船隻計時器錯誤." );
      pShip->waiting_tick = pShip->waiting;
      pShip->sailing_tick = -1;
      continue;
    }

    if ( pShip->waiting_tick > 0 )
    {
      if ( --pShip->waiting_tick <= 0 )
      {
        sprintf( buf, "%s\e[0m出發了﹐請旅客找個好位置坐下。\n\r順便欣賞沿途"
          "明媚的風光﹐%s\e[0m竭誠感謝你的光臨﹗\n\r"
          , pShip->name, mud_name );

        sendmsg_to_someroom( buf, pShip->cabin );

        chinese_number( pShip->sailing, buf1 );
        sprintf( buf, "%s\e[0m出發了﹐請欲搭乘本班船的旅客﹐"
          "再等%s小時等下一班﹗\n\r", pShip->name, buf1 );

        sendmsg_to_someroom( buf, pShip->starting );

        pShip->sailing_tick = pShip->sailing;
        pShip->waiting_tick = -1;
      }
      continue;
    }

    if ( pShip->sailing_tick > 0 )
    {
      pShip->sailing_tick--;

      if ( pShip->delay > 0 && number_range( 1, 10000 ) <= pShip->delay )
      {
        sendmsg_to_someroom(
          "遠處烏雲密佈﹐風雨欲來﹐船隻可能沒有辦法如期到達目的地了﹗\n\r"
          , pShip->cabin );

        pShip->sailing_tick++;
      }

      if ( pShip->sailing_tick <= 0 )
      {
        if ( !pShip->cabin )
        {
          mudlog( LOG_DEBUG, "ship_update: 船艙房間不正確." );
        }

        else
        {
          if ( pShip->msg_land && *pShip->msg_land )
            sendmsg_to_someroom( pShip->msg_land, pShip->cabin );

          for ( ch = pShip->cabin->people; ch; ch = ch_next )
          {
            ch_next = ch->next_in_room;

            if ( ch->fighting ) stop_fighting( ch, TRUE );

            if ( ch->position == POS_RESTING || ch->position == POS_SLEEPING )
            {
              send_to_char( "喂﹐船到了﹐該起床了。\n\r" , ch );

              /* 如果是昏睡 */
              if ( !is_affected( ch, SLOT_SLEEP ) ) ch->position = POS_STANDING;
            }

            act( "$n從$t走了下去。", ch, pShip->name, NULL, TO_ROOM );
            char_from_room( ch );
            char_to_room( ch, pShip->destination );

            act( "$n從$t上走了下來。", ch, pShip->name, NULL, TO_ROOM );
            do_look( ch, "auto" );

            /* 清除追蹤紀錄點 */
            clear_trace( ch, TRUE );
          }
        }

        if ( pShip->msg_entrance && *pShip->msg_entrance )
          sendmsg_to_someroom( pShip->msg_entrance, pShip->starting );

        pShip->waiting_tick = pShip->waiting;
        pShip->sailing_tick = -1;
      }

      /* 強盜 */
      else if ( pShip->sailing_tick == 1
        && pShip->pirate > 0
        && number_range( 1, 10000 ) <= pShip->pirate )
      {
        sendmsg_to_someroom(
          "遠處一艘艘的鐵殼船慢慢地出現﹐遭了﹐那不就是傳聞的強盜嗎﹖\n\r"
          "只見毫無抵抗能力的船老大悲傷地舉起白旗﹐向無情的強盜們投降。\n\r"
          "唉﹗真是倒楣﹐出外旅遊竟然碰到了強盜﹐當然身上的金錢是被洗劫一空。\n\r"
          "雖說錢是沒了﹐但幸好還能保住一條小命。唉﹗留的青山在﹐不怕沒材燒。\n\r"
          "經過一番凌虐後﹐一艘艘的鐵殼船又慢慢地消失在地平線了﹗\n\r"
          "真是的﹐該死的強盜們﹗下次就不要被我給逮到﹗\n\r\n\r", pShip->cabin );

        for ( ch = pShip->cabin->people; ch; ch = ch_next )
        {
          ch_next = ch->next_in_room;

          /* if ( IS_IMMORTAL( ch ) ) continue; */

          if ( ch->fighting )
          {
            stop_fighting( ch, TRUE );
            send_to_char( "老子來搶劫﹐你還在打架﹖找死啊﹗\n\r", ch );
          }

          if ( ch->position == POS_RESTING || ch->position == POS_SLEEPING )
          {
            send_to_char( "喂﹐強盜搶劫﹐起來受死吧﹗哈哈哈﹗\n\r" , ch );

            /* 如果是昏睡 */
            if ( !is_affected( ch, SLOT_SLEEP ) ) ch->position = POS_STANDING;
          }

          /* 搶錢 */
          if ( ch->gold > 0 )
          {
            gold_from_char( ch, ch->gold );
            send_to_char( "小子﹐把錢給交出來吧﹐哈哈哈﹐老子要定了﹗\n\r"
              , ch );
          }

          else
          {
            send_to_char( "臭小子﹐怎麼身上一毛錢都沒有﹐去去去﹗"
              "當強盜都比你強多了\n\r", ch );
          }

          /* 搶身上的東西, 先丟到地上 */
          for ( obj = ch->carrying; obj; obj = obj_next )
          {
            obj_next = obj->next_content;

            if ( obj->wear_loc == WEAR_NONE
              && can_drop_obj( ch, obj )
              && !obj->address )
            {
              act( "喂﹐你的$p我們兄弟們要定了。", ch, obj, NULL, TO_CHAR );
              act( "$n的$p被強盜搶走了。", ch, obj, NULL, TO_ROOM );
              obj_from_char( obj );
              obj_to_room( obj, ch->in_room );
            }
          }
        }

        /* 清除房間裡面所有的東西 */
        for ( obj = pShip->cabin->contents; obj; obj = obj_next )
        {
          obj_next = obj->next_content;
          if ( obj->item_type != ITEM_CORPSE_PC ) extract_obj( obj );
        }

        sprintf( buf, "開往%s%s的%s遭到強盜洗劫一空﹐船上的旅客無一倖免。"
          , pShip->destination && pShip->destination->area
            ? pShip->destination->area->name : "某不知名區域"
          , pShip->destination ? pShip->destination->name : "某不知名地方"
          , pShip->name );

        talk_channel_2( buf, CHANNEL_BULLETIN, "快報" );
        pShip->pirate_count++;
      }

      continue;
    }
  }

  RETURN_NULL();
}

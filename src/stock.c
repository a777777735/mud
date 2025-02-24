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

int     stock_win    = 0;
int     stock_lost   = 0;
int     StockShock   = STOCK_SHOCK;
int     StockTax     = STOCK_TAX;
int     StockBote    = STOCK_BOTE;
int     StockQuota   = STOCK_QUOTA;
int     StockRelease = STOCK_RELEASE;

int     stock_cost      args( ( int, int, int, bool ) );

FUNCTION( do_stock )
{
  int         loop;
  int         diff;
  int         cost;
  int         level;
  int         sn;
  int         cycle;
  char        arg[MAX_INPUT_LENGTH];
  char        arg1[MAX_INPUT_LENGTH];
  char        buf[MAX_STRING_LENGTH];
  char        buf1[MAX_STRING_LENGTH];
  bool        found = FALSE;
  bool        bStock = FALSE;
  CHAR_DATA * victim;

  PUSH_FUNCTION( "do_stock" );

  if ( IS_NPC( ch ) || !ch->pcdata || !ch->in_room ) RETURN_NULL();

  stock_value( ch );

  if ( ch->trade == FALSE )
  {
    send_to_char( "你的總資產太可怕了﹐所以無法執行此指令﹗\n\r", ch );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg );

  if ( !arg[0] || ( arg[0] == '!' && IS_IMMORTAL( ch ) ) )
  {
    clear_buffer();

    if ( arg[0] == '\x0' )
    {
      victim = ch;
    }

    else
    {
      if ( arg[1] == '\x0' )
      {
        send_to_char( "你要察看誰的股票狀態﹖\n\r", ch );
        RETURN_NULL();
      }

      if ( !( victim = get_char_world( ch, arg + 1 ) ) )
      {
        act( "找不到你的對象 $2$T$0。", ch, NULL, arg + 1, TO_CHAR );
        RETURN_NULL();
      }

      if ( get_trust( ch ) < get_trust( victim ) )
      {
        act( "你的權力不夠看$N的股票﹗", ch, NULL, victim, TO_CHAR );
        RETURN_NULL();
      }

      if ( IS_NPC( victim ) || !victim->pcdata )
      {
        act( "$N是不會有任何股票的.", ch, NULL, victim, TO_CHAR );
        RETURN_NULL();
      }
    }

    for ( loop = 0; loop < MAX_STOCK; loop++ )
    {
      if ( victim->pcdata->stock[loop] > 0 )
      {
        if ( !found )
        {
          send_to_buffer( "\e[1;33m「%s\e[1;33m手上的股票有﹕」\e[0m\n\r"
            , ch == victim ? "你" : victim->name );

          send_to_buffer( "\e[1;33;44m統 一 編 號 股    票    名    稱"
            " 持有張數 購入成本 目前價格 價錢差額\e[0m\n\r", ch );
        }

        found  = TRUE;
        bStock = ( IS_IMMORTAL( ch ) || ch->gold > 0 ) ? TRUE : FALSE;

        send_to_buffer( "統一編號%3d %-20s %8d %8d %s%8d"
          "\e[0m %s%8d\e[0m\n\r"
          , loop + 1, stock_data[loop].name
          , victim->pcdata->stock[loop], victim->pcdata->asset[loop]
          , ( bStock && victim->pcdata->asset[loop] > stock_data[loop].cost )
            ? "\e[1;31m" : "\e[1;32m"
          , bStock ? stock_data[loop].cost : 0
          , ( bStock && victim->pcdata->asset[loop] > stock_data[loop].cost )
            ? "\e[1;31m" : "\e[1;32m"
          , bStock ? ( stock_data[loop].cost - victim->pcdata->asset[loop] ) : 0 );

        if ( !IS_IMMORTAL( ch ) && bStock ) ch->gold = UMAX( 0, ch->gold - 1 );
      }
    }

    if ( !found )
    {
      send_to_buffer( "%s沒有任何的股票。\n\r"
      , ( victim == ch ) ? "你" : victim->name );
    }

    else if ( IS_IMMORTAL( ch ) )
    {
      chinese_number( stock_value( victim ), buf );
      send_to_buffer( "\e[1;33m股票總面值為 %s兩銀子。\e[0m\n\r", buf );
    }

    print_buffer( ch );
  }

  else if ( !str_prefix( arg, "history" ) )
  {
    int max  = MIN_STOCK_COST;
    int min  = MAX_STOCK_COST;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\x0' || !is_number( arg ) )
    {
      send_to_char( "語法錯誤﹐請查詢 stock。\n\r", ch );
      RETURN_NULL();
    }

    sn = atoi( arg ) -1;

    if ( sn < 0 || sn >= MAX_STOCK )
    {
      send_to_char( "範圍錯誤﹗請查詢 stock 使用方法﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !stock_data[sn].name || !*( stock_data[sn].name ) )
    {
      send_to_char( "這家公司尚未上櫃﹐無法查詢。\n\r", ch );
      RETURN_NULL();
    }

    for ( loop = 0; loop < MAX_STOCK_HISTORY; loop++ )
    {
      if ( stock_data[sn].history[loop] < MIN_STOCK_COST
        || stock_data[sn].history[loop] > MAX_STOCK_COST ) break;

      if ( stock_data[sn].history[loop] > max )
        max = stock_data[sn].history[loop];

      if ( stock_data[sn].history[loop] < min )
        min = stock_data[sn].history[loop];
    }

    if ( stock_data[sn].cost > max ) max = stock_data[sn].cost;
    if ( stock_data[sn].cost < min ) min = stock_data[sn].cost;

    if ( max == min )
    {
      act( "$t尚未有資料﹐無法追蹤﹗", ch, stock_data[sn].name, NULL, TO_CHAR );
      RETURN_NULL();
    }

    diff = ( ( max - min ) / 55 ) + 1;

    clear_buffer();
    send_to_buffer( "%7d | \e[1;32m", stock_data[sn].cost );
    level = ( stock_data[sn].cost - min ) / diff + 10;
    for ( loop = 0; loop < level; loop++ ) send_to_buffer( "#" );
    send_to_buffer( "\e[0m\n\r" );

    for ( loop = 0; loop < MAX_STOCK_HISTORY; loop++ )
    {
      if ( stock_data[sn].history[loop] < MIN_STOCK_COST
        || stock_data[sn].history[loop] > MAX_STOCK_COST ) break;

      send_to_buffer( "%7d | \e[1;32m", stock_data[sn].history[loop] );
      level = ( stock_data[sn].history[loop] - min ) / diff + 10;
      for ( cycle = 0; cycle < level; cycle++ ) send_to_buffer( "#" );
      send_to_buffer( "\e[0m\n\r" );
    }

    print_buffer( ch );
  }

  else if ( !str_prefix( arg, "status" ) && IS_IMMORTAL( ch ) )
  {
    print_to_char( ch,
      "股市每日最大漲幅  ﹕千分之 %d\n\r"
      "股市每日最大跌幅  ﹕千分之 %d\n\r"
      "股市每日交易影響量﹕%d 張\n\r"
      "股市證交稅        ﹕千分之 %d\n\r"
      "股市崩盤賠償率    ﹕原價的 %d 之一\n\r"
      "股市交易配額      ﹕%d 張\n\r"
      "每張股票最多量    ﹕%d 張\n\r"
      , stock_win
      , stock_lost
      , StockShock
      , StockTax
      , StockBote
      , StockQuota
      , StockRelease );
  }

  else if ( !str_prefix( arg, "list" ) )
  {
    if ( ch->in_room->Stock == FALSE && !IS_IMMORTAL( ch ) )
    {
      if ( ch->bank < 10 )
      {
        send_to_char( "抱歉﹐你銀行裡戶頭錢實在太少了﹗\n\r", ch );
        RETURN_NULL();
      }

      ch->bank -= 10;
    }

    clear_buffer();
    send_to_buffer( "\e[1;33;44m編號 公司行號             買進 賣出 "
      "今日價格 昨日價格 漲跌幅 鎖定          \e[0m\n\r" );

    for ( loop = 0; loop < MAX_STOCK; loop++ )
    {
      if ( !stock_data[loop].name ) continue;

      diff = stock_data[loop].cost - stock_data[loop].history[0];

      send_to_buffer( "%4d %-20s %4d %4d %s%8d\e[0m %8d %s%3d%%%%  %s\n\r"
        , loop + 1
        , stock_data[loop].name
        , stock_data[loop].today_buy
        , stock_data[loop].today_sell
        , diff > 0 ? "\e[1;31m" : "\e[1;32m"
        , stock_data[loop].cost
        , stock_data[loop].history[0]
        , diff > 0 ? "\e[1;31m+\e[0m" : ( diff == 0 ? " " : "\e[1;34m-\e[0m" )
        , abs( diff ) * 1000 / ( UMAX( 1, stock_data[loop].history[0] ) )
        , YESNO( stock_data[loop].lock ) );
    }

    print_buffer( ch );
  }

  else if ( !str_prefix( arg, "sell" ) )
  {
    if ( over_scale( ch ) )
    {
      send_to_char( "你的總資產已達上限﹐無法買賣股票﹗\n\r", ch );
      RETURN_NULL();
    }

    bStock = TRUE;
    if ( ch->in_room->Stock == FALSE && !IS_IMMORTAL( ch ) ) bStock = FALSE;

    argument = one_argument( argument, arg );

    if ( !arg[0] || !is_number( arg ) )
    {
      send_to_char( "語法錯誤﹐請查詢 stock 的用法。\n\r", ch );
      RETURN_NULL();
    }

    argument = one_argument( argument, arg1 );
    level    = 1;

    if ( arg1[0] )
    {
      if ( !is_number( arg1 ) )
      {
        send_to_char( "你必須輸入你想賣的張數﹗\n\r", ch );
        RETURN_NULL();
      }

      if ( ( level = atoi( arg1 ) ) <= 0 )
      {
        send_to_char( "你來耍寶的嗎﹖\n\r", ch );
        RETURN_NULL();
      }

      if ( level > max_stock_buy )
      {
        send_to_char( "這裡不接受大盤進場干預喔﹗\n\r", ch );
        RETURN_NULL();
      }
    }

    loop = atoi( arg ) - 1;
    if ( loop < 0
      || loop >= MAX_STOCK
      || !stock_data[loop].name
      || ch->pcdata->stock[loop] < level )
    {
      send_to_char( "你並沒有那麼多張股票喔﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( stock_data[loop].lock )
    {
      act( "$t目前暫停交易。", ch, stock_data[loop].name, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( StockQuota > 0
      && stock_data[loop].today_sell + level > StockQuota )
    {
      act( "$t今天交易量已經超過配額﹗", ch, stock_data[loop].name, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( ( cost = stock_cost( loop, level, STOCK_SELL_TAX, bStock ) ) <= 0 )
    {
      send_to_char( "你交易的量實在太大了。\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->bank < cost )
    {
      send_to_char( "你銀行裡的存款不夠付證交稅。\n\r", ch );
      RETURN_NULL();
    }

    if ( ( cost = stock_cost( loop, level, STOCK_SELL, bStock ) ) <= 0
      || cost + ch->bank <= 0
      || cost + ch->bank >= MAX_ASSET )
    {
      send_to_char( "你交易的金額太大了﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( ( ch->pcdata->stock[loop] -= level ) <= 0 )
      ch->pcdata->asset[loop] = 0;

    ch->bank += cost;

    chinese_number( stock_data[loop].cost * level, buf );
    chinese_number( level, buf1 );
    print_to_char( ch, "你以%s兩銀子賣了%s張%s的股票。\n\r"
      , buf, buf1, stock_data[loop].name );

    stock_data[loop].sell       += level;
    stock_data[loop].today_sell += level;
    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "buy" ) )
  {
    if ( !IS_NPC( ch ) && ch->level < level_limit )
    {
      act( "在你升級到$i級前﹐請先不要投資。", ch, &level_limit, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch->donate > 0 )
    {
      send_to_char( "在你領救濟金的這段時間﹐請不要投資股票﹗\n\r", ch );
      RETURN_NULL();
    }

    argument = one_argument( argument, arg );
    if ( !arg[0] || !is_number( arg ) )
    {
      send_to_char( "語法錯誤﹐請查詢 stock 的用法。\n\r", ch );
      RETURN_NULL();
    }

    bStock = TRUE;
    if ( ch->in_room->Stock == FALSE && !IS_IMMORTAL( ch ) ) bStock = FALSE;

    argument = one_argument( argument, arg1 );
    level    = 1;

    if ( arg1[0] )
    {
      if ( !is_number( arg1 ) )
      {
        send_to_char( "你必須輸入你想買的張數﹗\n\r", ch );
        RETURN_NULL();
      }

      level = atoi( arg1 );
      if ( level <= 0 )
      {
        send_to_char( "你來耍寶的嗎﹖\n\r", ch );
        RETURN_NULL();
      }

      if ( level > max_stock_buy )
      {
        send_to_char( "這裡不接受大盤進場干預喔﹗\n\r", ch );
        RETURN_NULL();
      }
    }

    loop = atoi( arg ) - 1;
    if ( loop < 0 || loop >= MAX_STOCK || !stock_data[loop].name )
    {
      send_to_char( "票據所裡沒有那張股票耶。\n\r", ch );
      RETURN_NULL();
    }

    if ( stock_data[loop].lock )
    {
      act( "$t目前暫停交易。", ch, stock_data[loop].name, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( StockRelease > 0
      && ( stock_data[loop].buy - stock_data[loop].sell ) > StockRelease )
    {
      act( "$t發行量已達到上限﹗", ch, stock_data[loop].name, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch->pcdata->stock[loop] + level >= max_stock_buy )
    {
      act( "你$t太多了﹐請投資在其他地方﹗", ch, stock_data[loop].name, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( StockQuota > 0
      && stock_data[loop].today_buy + level > StockQuota )
    {
      act( "$t今天交易量已經超過配額﹗", ch, stock_data[loop].name, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( ( cost = stock_cost( loop, level, STOCK_BUY, bStock ) ) <= 0 )
    {
      send_to_char( "已超出系統的計算範圍。\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->bank < cost )
    {
      send_to_char( "你銀行裡的存款不夠買這一次股票。\n\r", ch );
      RETURN_NULL();
    }

    /* 修正購買股票成本 */
    {
      float Total;
      float Count;

      Total  = ( float ) ch->pcdata->asset[loop]
             * ( float ) ch->pcdata->stock[loop];
      Total += ( float ) cost;
      Count  = ( float ) ( UMAX( 1, ch->pcdata->stock[loop] + level ) );
      ch->pcdata->asset[loop] = Total / Count;
    }

    ch->pcdata->stock[loop] += level;
    ch->bank                 = UMAX( 0, ch->bank - cost );

    chinese_number( cost, buf );
    chinese_number( level, buf1 );
    print_to_char( ch, "你花了%s兩銀子買了%s張%s的股票。\n\r"
      , buf, buf1, stock_data[loop].name );

    stock_data[loop].buy       += level;
    stock_data[loop].today_buy += level;

    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "information" ) && IS_IMMORTAL( ch ) )
  {
    send_to_buffer( "\e[1;33;44m編號 公司行號           買進 賣出 "
      "今價格 昨價格 漲跌 總買入 總賣出 總張數 利益比\e[0m\n\r" );

    for ( loop = 0; loop < MAX_STOCK; loop++ )
    {
      if ( !stock_data[loop].name ) continue;

      diff = stock_data[loop].cost - stock_data[loop].history[0];

      send_to_buffer( "%s%3d %-18s %4d %4d %s%6d\e[0m %6d %s%3d "
        "%6d %6d %6d %2d--%2d\n\r"
        , stock_data[loop].lock ? "\e[1;32m*\e[0m" : " "
        , loop + 1
        , stock_data[loop].name
        , stock_data[loop].today_buy
        , stock_data[loop].today_sell
        , diff > 0 ? "\e[1;31m" : "\e[1;32m"
        , stock_data[loop].cost
        , stock_data[loop].history[0]
        , diff > 0 ? "\e[1;31m+\e[0m" : ( diff == 0 ? " " : "\e[1;34m-\e[0m" )
        , abs( diff ) * 1000 / ( UMAX( 1, stock_data[loop].history[0] ) )
        , stock_data[loop].buy
        , stock_data[loop].sell
        , stock_data[loop].buy - stock_data[loop].sell
        , stock_data[loop].win
        , stock_data[loop].lost );
    }

    print_buffer( ch );
  }

  else if ( !str_prefix( arg, "add" ) && IS_GOD( ch ) )
  {
    for ( sn = 0; sn < MAX_STOCK; sn++ ) if ( !stock_data[sn].name ) break;

    if ( sn == MAX_STOCK )
    {
      send_to_char( "抱歉沒有額外的空間來增加新的上櫃公司。\n\r", ch );
      RETURN_NULL();
    }

    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg1 );

    if ( arg[0] == '\x0' || arg1[0] == '\x0' )
    {
      send_to_char( "設定錯誤﹐請查詢 stock 使用方法﹗\n\r", ch );
      RETURN_NULL();
    }

    for ( loop = 0; loop < MAX_STOCK; loop++ )
    {
      if ( stock_data[loop].name && !str_cmp( stock_data[loop].name, arg ) )
      {
        send_to_char( "這個上櫃公司名稱已經有使用過了。\n\r", ch );
        RETURN_NULL();
      }
    }

    if ( !is_number( arg1 ) || ( ( cost = atoi( arg1 ) ) < 10000 ) )
    {
      send_to_char( "上櫃價錢至少要一萬元以上。\n\r", ch );
      RETURN_NULL();
    }

    for ( loop = 0; loop < MAX_STOCK_HISTORY; loop++ )
      stock_data[sn].history[loop] = -1;

    stock_data[sn].cost       = cost;
    stock_data[sn].history[0] = cost;
    stock_data[sn].name       = str_dup( arg );
    stock_data[sn].buy        = 0;
    stock_data[sn].sell       = 0;
    stock_data[sn].today_sell = 0;
    stock_data[sn].today_buy  = 0;
    stock_data[sn].win        = stock_win;
    stock_data[sn].lost       = stock_lost;
    stock_data[sn].lock       = FALSE;

    sprintf( buf, "%s股票上櫃喔﹐請大家多多購買﹗謝謝﹗", arg );
    talk_channel_2( buf, CHANNEL_BULLETIN, "股票快報" );
  }

  else if ( !str_prefix( arg, "refresh" ) && IS_GOD( ch ) )
  {
    send_to_char( "你重新刷新股市交易情形﹗\n\r", ch );
    stock_update();
  }

  else if ( !str_prefix( arg, "set" ) && IS_GOD( ch ) )
  {
    argument = one_argument( argument, arg );
    if ( arg[0] == '\x0' || !is_number( arg ) )
    {
      send_to_char( "語法錯誤﹐請查詢 stock。\n\r", ch );
      RETURN_NULL();
    }

    sn = atoi( arg ) -1;
    if ( sn < 0 || sn >= MAX_STOCK )
    {
      send_to_char( "範圍錯誤﹗請查詢 stock 使用方法﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !stock_data[sn].name )
    {
      send_to_char( "這家公司尚未上櫃﹐無法修改。\n\r", ch );
      RETURN_NULL();
    }

    argument = one_argument( argument, arg  );
    argument = one_argument( argument, arg1 );

    if ( !arg[0] || !arg1[0] )
    {
      send_to_char( "你要修改哪一個選項呢﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( !str_cmp( arg, "name" ) )
    {
      for ( loop = 0; loop < MAX_STOCK; loop++ )
      {
        if ( stock_data[loop].name && !str_cmp( stock_data[loop].name, arg1 ) )
        {
          send_to_char( "這個上櫃公司名稱已經有使用過了。\n\r", ch );
          RETURN_NULL();
        }
      }

      smash_tilde( arg1 );
      sprintf( buf, "上櫃公司%s從此改名為%s﹐請繼續光臨﹗"
        , stock_data[sn].name, arg1 );

      free_string( stock_data[sn].name );
      stock_data[sn].name = str_dup( arg1 );
      talk_channel_2( buf, CHANNEL_BULLETIN, "股票快報" );
      RETURN_NULL();
    }

    else if ( !str_cmp( arg, "win" ) )
    {
      if ( ( cost = atoi( arg1 ) ) <= 0 || cost >= stock_win * 2 )
      {
        send_to_char( "這樣很容易造成股市崩盤的喔﹗\n\r", ch );
        RETURN_NULL();
      }

      stock_data[sn].win = cost;
      act( "你把$t股票的漲幅調為$I", ch, stock_data[sn].name, &cost, TO_CHAR );

      mudlog( LOG_WIZARD, "%s修改股市%s的漲幅為%d"
        , ch->name, stock_data[sn].name, cost );

      str_cpy( buf, "大盤進場干預囉﹗大家小心囉﹗" );
      talk_channel_2( buf, CHANNEL_BULLETIN, "股票快報" );
      RETURN_NULL();
    }

    else if ( !str_cmp( arg, "lost" ) )
    {
      if ( ( cost = atoi( arg1 ) ) <= 0 || cost >= stock_lost * 2 )
      {
        send_to_char( "這樣很容易造成股市崩盤的喔﹗\n\r", ch );
        RETURN_NULL();
      }

      stock_data[sn].lost = cost;
      act( "你把$t股票的跌幅調為$I", ch, stock_data[sn].name, &cost, TO_CHAR );

      mudlog( LOG_WIZARD, "%s修改股市%s的跌幅為%d"
        , ch->name, stock_data[sn].name, cost );

      str_cpy( buf, "有大盤進場干預囉﹗大家小心囉﹗" );
      talk_channel_2( buf, CHANNEL_BULLETIN, "股票快報" );
      RETURN_NULL();
    }

    else if ( !str_prefix( arg, "lock" ) )
    {
      mudlog( LOG_WIZARD, "%s鎖定股市%s狀態.", ch->name, stock_data[sn].name );

      stock_data[sn].lock = stock_data[sn].lock ? FALSE : TRUE;

      act( "你$t$T的交易行為。"
        , ch
        , stock_data[sn].lock ? "暫停" : "恢復"
        , stock_data[sn].name
        , TO_CHAR );

      sprintf( buf, "%s%s交易囉﹗"
        , stock_data[sn].lock ? "暫停" : "恢復"
        , stock_data[sn].name );

      talk_channel_2( buf, CHANNEL_BULLETIN, "股票快報" );

      RETURN_NULL();
    }

    else if ( !str_prefix( arg, "cost" ) )
    {
      if ( ( cost = atoi( arg1 ) ) < MIN_STOCK_COST
        || cost > MAX_STOCK_COST )
      {
        send_to_char( "這樣很容易造成股市崩盤的喔﹗\n\r", ch );
        RETURN_NULL();
      }

      set_stock_value( sn, cost );
      act( "你把$t股票的跌幅調為$I", ch, stock_data[sn].name, &cost, TO_CHAR );

      mudlog( LOG_WIZARD, "%s修改股市%s的價錢為%d"
        , ch->name, stock_data[sn].name, cost );

      str_cpy( buf, "有大盤進場干預囉﹐大家小心囉﹗" );
      talk_channel_2( buf, CHANNEL_BULLETIN, "股票快報" );
      RETURN_NULL();
    }

    else
    {
      send_to_char( "選項錯誤﹐請查詢 stock。\n\r", ch );
      RETURN_NULL();
    }
  }

  else
  {
    send_to_char( "指令錯誤﹐請查詢 stock。\n\r", ch );
  }

  RETURN_NULL();
}

void stock_update( void )
{
  int sn;
  int value;
  int win;
  int lost;

  PUSH_FUNCTION( "stock_update" );

  for ( sn = 0; sn < MAX_STOCK; sn++ )
  {
    if ( !stock_data[sn].name ) continue;

    /* 修正漲跌幅度 */
    if ( StockShock > 0 )
    {
      stock_data[sn].win  +=
        UMIN( stock_win, ( stock_data[sn].today_buy  / StockShock ) );

      stock_data[sn].lost +=
        UMIN( stock_lost, ( stock_data[sn].today_sell / StockShock ) );

      if ( stock_data[sn].today_buy == 0 && stock_data[sn].win > stock_win )
        stock_data[sn].win--;

      if ( stock_data[sn].today_sell == 0 && stock_data[sn].lost > stock_lost )
        stock_data[sn].lost--;
    }

    stock_data[sn].today_sell = 0;
    stock_data[sn].today_buy  = 0;

    if ( stock_data[sn].lock ) continue;

    lost  = stock_data[sn].lost * stock_data[sn].cost / 1000;
    win   = stock_data[sn].win  * stock_data[sn].cost / 1000;
    value = number_range( -lost, win );

    set_stock_value( sn,
      UMIN( MAX_STOCK_COST, UMAX( MIN_STOCK_COST, stock_data[sn].cost + value ) ) );

    while ( stock_data[sn].win > stock_win
      && stock_data[sn].lost > stock_lost )
    {
      stock_data[sn].win--;
      stock_data[sn].lost--;
    }
  }

  save_stock();
  RETURN_NULL();
}

void save_stock( void )
{
  FILE * pFile;
  int    loop;

  PUSH_FUNCTION( "save_stock" );

  if ( ( pFile = FOPEN( stock_file, "w" ) ) )
  {
    for ( loop = 0; loop < MAX_STOCK; loop++ )
    {
      if ( !stock_data[loop].name ) continue;
      fprintf( pFile, "Stock   %-7d %-23s %-7d %-7d %-7d\n"
        , loop + 1
        , stock_data[loop].name
        , stock_data[loop].cost
        , stock_data[loop].sell
        , stock_data[loop].buy );
    }

    FCLOSE( pFile );

    /* 改變檔案存取模式 */
    set_file_mode( stock_file );

  }
  else
  {
    mudlog( LOG_DEBUG, "save_stock: 無法寫入股票記錄檔 %s.", stock_file );
  }

  RETURN_NULL();
}

int stock_value( CHAR_DATA * ch )
{
  float Total = 0;
  int   slot;

  PUSH_FUNCTION( "stock_value" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "stock_value: 缺乏來源." );
    RETURN( -1 );
  }

  if ( IS_NPC( ch ) || !ch->pcdata ) RETURN( 0 );

  if ( !IS_NPC( ch ) )
  {
    for ( slot = 0; slot < MAX_STOCK; slot++ )
    {
      if ( ch->pcdata->stock[slot] > 0 && stock_data[slot].name )
      {
        Total += ( float ) stock_data[slot].cost
               * ( float ) ch->pcdata->stock[slot];

        if ( Total < 0 || Total >= MaxNumber || Total > MAX_ASSET )
        {
          ch->trade = FALSE;
          mudlog( LOG_DEBUG, "stock_value: %s 股票資產溢位.", ch->name );
          RETURN( -1 );
        }
      }
    }
  }

  RETURN( ( ( int ) Total ) );
}

void bote_stock( CHAR_DATA * ch )
{
  char buf[MAX_STRING_LENGTH];
  int  loop;
  int  count;
  int  value;
  int  stock;
  int  total;

  PUSH_FUNCTION( "bote_stock" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "bote_stock: 缺乏來源." );
    RETURN_NULL();
  }

  if ( IS_NPC( ch ) || StockBote <= 0 || !ch->pcdata ) RETURN_NULL();

  for ( total = loop = 0; loop < MAX_STOCK; loop++ )
  {
    if ( ( count = ch->pcdata->stock[loop] ) > 0
      && ( stock = ch->pcdata->asset[loop] ) > 0 )
    {
      if ( ( value  = ( stock * count / StockBote ) ) <= 0 ) continue;
      total += value;

      if ( ch->bank + value <= MAX_ASSET )
      {
        chinese_number( value, buf );
        print_to_char( ch, "您的股票\e[1;37m%s\e[0m因為崩盤﹐所以"
          "%s賠償你部份的損失 %s 兩銀子﹗\n\r"
          , stock_data[loop].name
          , mud_name
          , buf );

        ch->bank += value;
      }

      ch->pcdata->stock[loop] = 0;
      ch->pcdata->asset[loop] = 0;
    }
  }

  RETURN_NULL();
}

int stock_cost( int slot, int count, int type, bool bStock )
{
  float Cost;
  float Value;
  float Count;
  float Tax;
  int   tax;

  PUSH_FUNCTION( "stock_cost" );

  if ( slot < 0 || slot >= MAX_STOCK || !stock_data[slot].name )
  {
    mudlog( LOG_DEBUG, "stock_cost: 來源不正確." );
    RETURN( -1 );
  }

  if ( count <= 0 || count > max_stock_buy )
  {
    mudlog( LOG_DEBUG, "stock_cost: 數量 %d 不合理.", count );
    RETURN( -1 );
  }

  switch( type )
  {
  default:
    mudlog( LOG_DEBUG, "stock_cost: 錯誤的選項 %d.", type );
    RETURN( -1 );

  case STOCK_BUY:

    tax = ( bStock ) ? ( 1000 + StockTax ) : ( 1000 + ( StockTax * 2 ) );
    break;

  case STOCK_BUY_TAX:
  case STOCK_SELL_TAX:

    tax = ( bStock ) ? StockTax : ( 2 * StockTax );
    break;

  case STOCK_SELL:

    tax = bStock ? ( 1000 - StockTax ) : ( 1000 - ( StockTax * 2 ) );
    break;
  }

  Value = ( float ) stock_data[slot].cost;
  Count = ( float ) count;
  Tax   = ( float ) tax;
  Cost  = Value * Count * Tax / 1000;

  if ( Cost < 0
    || Cost >= MaxNumber
    || Cost > MAX_ASSET
    || Cost > MAX_STOCK_TRADE ) RETURN( -1 );

  RETURN( ( ( int) Cost ) );
}

void set_stock_value( int slot, int cost )
{
  int loop;

  PUSH_FUNCTION( "set_stock_value" );

  if ( slot < 0 || slot >= MAX_STOCK || !stock_data[slot].name )
  {
    mudlog( LOG_DEBUG, "set_stock_value: 來源 %d 錯誤.", slot );
    RETURN_NULL();
  }

  if ( cost < MIN_STOCK_COST || cost > MAX_STOCK_COST )
  {
    mudlog( LOG_DEBUG, "set_stock_value: 序號 %d 價格 %d 不合理."
      , slot, cost );

    RETURN_NULL();
  }

  for ( loop = MAX_STOCK_HISTORY - 2; loop >= 0; loop-- )
    stock_data[slot].history[loop+1] = stock_data[slot].history[loop];

  if ( stock_data[slot].cost < MIN_STOCK_COST
    || stock_data[slot].cost > MAX_STOCK_COST )
  {
    stock_data[slot].history[0] = cost;
  }

  else
  {
    stock_data[slot].history[0] = stock_data[slot].cost;
  }

  stock_data[slot].cost = cost;
  RETURN_NULL();
}

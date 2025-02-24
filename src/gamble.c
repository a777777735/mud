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
#include <stdlib.h>
#include <string.h>
#include "merc.h"

#define MAX_GAMBLE_MESSAGE      8
#define MIN_GAMBLE_GOLD         100
#define MAX_GAMBLE_GOLD         10000
#define LOTTO_CONGULATION       200000

#define GAMBLE_GOLD             1

/* 區域變數 */
int     last_hour;
int     gamble_tick;
int     horse_step [ MAX_HORSES ];
int     horse_data [ MAX_HORSES ];
int     horse_order[ MAX_HORSES ];

char * const horse_name[ MAX_HORSES ] =
{
  "\e[1;31m飛天豬\e[0m" ,
  "\e[1;32m大懶豬\e[0m" ,
  "\e[1;33m頑皮豬\e[0m" ,
  "\e[1;34m精靈豬\e[0m" ,
  "\e[1;35m暴力豬\e[0m" ,
  "\e[1;36m硬毛豬\e[0m"
};

char * gamble_message[ MAX_GAMBLE_MESSAGE ] =
{
  "\e[1;33m小賭可以安家﹐大賭可以興邦﹗\e[0m"   ,
  "\e[1;31m買定離手﹐買定離手﹗\e[0m"           ,
  "\e[1;35m看賭的幾百個﹐下注的沒幾個﹗\e[0m"   ,
  "\e[1;32m天大地大﹐賭博最大﹗\e[0m"           ,
  "\e[1;34m有錢來賭博﹐沒錢賣老婆﹗\e[0m"       ,
  "\e[1;36m先賭先贏﹐後賭的會輸到不行﹗\e[0m"   ,
  "\e[1;31m有錢賭錢﹐沒錢賭爛﹗\e[0m"           ,
  "\e[1;32m有錢請下注﹐沒錢請回家抱老婆﹗\e[0m"
};

void   gamble_order       args( ( void ) );
char * return_horse_name  args( ( GAMBLE_DATA * gamble ) );
int    multipile          args( ( int number ) );
int    check_gamble       args( ( GAMBLE_DATA * gamble ) );
int    check_gamble_valid args( ( CHAR_DATA * ch , GAMBLE_DATA * gamble ) );

FUNCTION( do_gamble )
{
  GAMBLE_DATA * gamble;
  char          arg[MAX_INPUT_LENGTH];
  char          buf[MAX_STRING_LENGTH];
  int           loop;
  int           count;
  int           money;
  int           horse;
  bool          found;

  PUSH_FUNCTION( "do_gamble" );

  /* 非玩家不能賭博 */
  if ( IS_NPC( ch ) ) RETURN_NULL();

  argument = one_argument( argument , arg );

  if ( !arg[0] )
  {
    send_to_char( "如果對飆豬比賽不懂的﹐請輸入 \e[1;32mgamble /?\e[0m\n\r"
      , ch );
    RETURN_NULL();
  }

  /* 察看賭博的相關資訊 */
  if ( !str_prefix( arg , "!information" ) && IS_GOD( ch ) )
  {
    clear_buffer();
    send_to_buffer( "賭博的計數器﹕%d。\n\r" , gamble_tick );
    for ( loop = 0; loop < MAX_HORSES; loop++ )
    {
      send_to_buffer( "[%2d] %s的腳步數 %5d。\n\r"
        , loop+1 , horse_name[loop] , horse_step[loop] );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  /* 修改每一隻豬的腳步數 */
  if ( !str_prefix( arg , "!set" ) && IS_GOD( ch ) )
  {
    argument = one_argument( argument , arg );

    if ( ( horse = atoi( arg ) ) < 1 || horse > MAX_HORSES )
    {
      chinese_number( MAX_HORSES, buf );
      act( "修改的豬只能從 1 到 $t。" , ch, buf, NULL, TO_CHAR );
      RETURN_NULL();
    }

    argument = one_argument( argument , arg );
    if ( ( count = atoi( arg ) ) < 0 )
    {
      send_to_char( "修改的腳步數不能小於 0。\n\r" , ch );
      RETURN_NULL();
    }

    horse_step[ horse - 1 ] = count;
    act( "你修改了第$i隻豬的腳步數為$I。", ch, &horse, &count, TO_CHAR );

    /* 標記 */
    mudlog( LOG_INFO , "[GAMBLE] %s 修改了飆豬的資料." , ch->name );

    RETURN_NULL();
  }

  /* 察看每一隻豬的情況 */
  if ( !str_prefix( arg , "!condition" ) )
  {
    for ( clear_buffer(), loop = 0; loop < MAX_HORSES; loop++ )
    {
      send_to_buffer( "[%d] %s贏過 %5d 次第一名。\n\r"
        , loop + 1 , horse_name[ loop ] , horse_data[ loop ] );
    }

    send_to_buffer( "\n\r目前飆豬的情況是﹕" );
    switch ( gamble_tick )
    {
    default : break;
    case -1 : send_to_buffer( "籌措獎金中。\n\r" ); break;
    case  0 : send_to_buffer( "發送獎金中。\n\r" ); break;
    case  1 : send_to_buffer( "整理場地中。\n\r" ); break;
    case  2 :
    case  3 :
    case  4 : send_to_buffer( "接受下注中。\n\r" ); break;
    case  5 :
    case  6 :
    case  7 :
    case  8 : send_to_buffer( "比賽進行中。\n\r" ); break;
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  /* 報導比賽的情況 */
  if ( !str_prefix( arg , "!report" ) )
  {
    if ( gamble_tick < 5 )
    {
      send_to_char( "比賽都還沒開始﹐怎麼觀看比賽的情形﹖\n\r" , ch );
      RETURN_NULL();
    }

    for ( gamble_order(), clear_buffer(), loop = 0; loop < MAX_HORSES; loop++ )
    {
      send_to_buffer( "現在跑第 %d 的豬是﹕%s(%d)\n\r"
        , loop + 1
        , horse_name[horse_order[loop]]
        , horse_order[loop] + 1 );
    }

    send_to_buffer( "\n\r\n\r" );
    print_buffer( ch );
    RETURN_NULL();
  }

  /* 檢查下注的情況 */
  if ( !str_prefix( arg , "!check" ) )
  {
    clear_buffer();
    send_to_buffer( "你目前所下的注有﹕\n\r" );
    for ( found = FALSE, gamble = ch->gamble; gamble; gamble = gamble->next )
    {
      found = TRUE;
      send_to_buffer( "你花了%d兩銀子買了%s。\n\r"
        , gamble->mount , return_horse_name( gamble ) );
    }

    if ( !found ) send_to_buffer( "你沒有下任何的注。\n\r" );
    print_buffer( ch );
    RETURN_NULL();
  }

  /* 下注金錢的主要程式 */
  if ( !str_prefix( arg , "gold" ) || !str_prefix( arg , "coin" ) )
  {
    /* 不是下注的時間 */
    if ( gamble_tick < 2 || gamble_tick >=5 )
    {
      send_to_char( "現在不能下注耶﹐請等一下吧。\n\r" , ch );
      RETURN_NULL();
    }

    if ( ch->trade == FALSE )
    {
      send_to_char( "你的總資產太可怕了﹐所以無法執行這個指令﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !IS_NPC( ch ) && ch->level < level_limit )
    {
      act( "在你升級到$i級前﹐請不要以父母的金錢決勝負。"
        , ch, &level_limit, NULL, TO_CHAR );

      RETURN_NULL();
    }

    if ( ch->donate > 0 )
    {
      send_to_char( "救濟金可不是讓你拿來賭博用的﹗\n\r", ch );
      RETURN_NULL();
    }

    argument = one_argument( argument , arg );

    /* 沒有下一個引述 */
    if ( !arg[0] )
    {
      send_to_char( "看來你不是賭林高手﹐還是先查詢 gamble 吧。\n\r" , ch );
      RETURN_NULL();
    }

    /* 賭注太少 */
    if ( ( money = atoi( arg ) ) < MIN_GAMBLE_GOLD )
    {
      send_to_char( "這麼小的賭注﹐丟到水溝裡面算了。\n\r" , ch );
      RETURN_NULL();
    }

    /* 賭注太多 */
    if ( money > MAX_GAMBLE_GOLD )
    {
      send_to_char( "嗯﹐這麼大的賭注﹐我怕會倒莊耶。\n\r" , ch );
      RETURN_NULL();
    }

    /* 身上的錢不夠來賭 */
    if ( money > ch->gold )
    {
      send_to_char( "真是賭性堅強﹐不過你先籌一些錢來賭吧。\n\r" , ch );
      RETURN_NULL();
    }

    /* 配置賭博結構的記憶體 */
    gamble = alloc_struct( STRUCT_GAMBLE_DATA );

    /* 清除馬匹資料 */
    for ( count = 0; count < MAX_HORSES; count++ ) gamble->horse[ count ] = -1;

    count    = 0;
    argument = one_argument( argument , arg );

    while ( count < MAX_HORSES && arg[0] )
    {
      if ( ( horse = atoi( arg ) ) > MAX_HORSES || horse < 1 )
      {
        count = 0;
        break;
      }

      gamble->horse[ count ] = horse - 1;
      count++;
      argument = one_argument( argument , arg );
    }

    /* 根本沒有輸入豬隻的號碼或是輸入方式不對 */
    if ( count == 0 )
    {
      for ( count = 0; count < MAX_HORSES; count++ ) gamble->horse[count] = -1;

      /* 釋放結構的記憶體 */
      free_struct( gamble , STRUCT_GAMBLE_DATA );

      send_to_char( "你下注的方式不對﹐請查詢 gamble 的使用方法。\n\r" , ch );
      RETURN_NULL();
    }

    /* 檢查是否有重複或是錯誤 */
    if ( !( check_gamble_valid( ch , gamble ) ) )
    {
      for ( count = 0; count < MAX_HORSES; count++ ) gamble->horse[count] = -1;

      /* 釋放結構的記憶體 */
      free_struct( gamble , STRUCT_GAMBLE_DATA );

      send_to_char( "你下注的彩卷是無效的﹐請查詢 GAMBLE。\n\r" , ch );
      RETURN_NULL();
    }

    act( "你花了$i兩銀子買了$T。"
      , ch, &money, return_horse_name( gamble ), TO_CHAR );

    /* 把資料填入結構之中 */
    gamble->format = GAMBLE_GOLD;
    gamble->mount  = money;

    /* 載入玩家的 gamble 的結構中 */
    gamble->next = ch->gamble;
    ch->gamble   = gamble;

    /* 扣掉彩金 */
    gold_from_char( ch, money );

    /* 強制打開賭博頻道 */
    if ( IS_SET( ch->deaf , CHANNEL_GAMBLE ) )
    {
      REMOVE_BIT( ch->deaf , CHANNEL_GAMBLE );
      send_to_char( "豬場老大順便幫你把賭博頻道打開了。\n\r" , ch );
    }

    RETURN_NULL();
  }

  do_gamble( ch , "" );
  RETURN_NULL();
}

/* 啟始化賭博的參數 */
void initial_gamble( void )
{
  int loop;

  PUSH_FUNCTION( "inital_gamble" );

  gamble_tick =  1;
  last_hour   = -1;

  /* 清除第一名的資料 */
  for ( loop = 0 ; loop < MAX_HORSES; loop++ ) horse_data[loop] = 0;

  RETURN_NULL();
}

/* 系統飆豬的主要程式 */
void gamble_update( void )
{
  int               loop;
  int               lotto;
  char              buf[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA * d;
  GAMBLE_DATA     * gamble;
  CHAR_DATA       * ch;

  PUSH_FUNCTION( "gamble_update" );

  /* 亂數決定豬跑的步數 */
  if ( gamble_tick >= 5 )
  {
    for ( loop = 0; loop < MAX_HORSES; loop++ )
      horse_step[loop] += number_range( 1 , 5 );
  }

  /* 每一個小時作一次動作, 不然沒有動作 */
  if ( time_info.hour == last_hour ) RETURN_NULL();

  /* 記錄上次的時間 , 並且把 tick 增加一 */
  last_hour = time_info.hour;
  gamble_tick++;

  /* 結束, 並且算錢 */
  if ( gamble_tick == 0 )
  {
    /* 清除所有玩家的賭博資料 */
    for ( d = descriptor_list; d; d = d->next )
    {
      if ( !verify_desc( d )
        || d->connected != CON_PLAYING
        || !( ch = d->character ) ) continue;

      for ( gamble = ch->gamble; gamble; gamble = gamble->next )
      {
        if ( gamble->format == GAMBLE_GOLD )
        {
          if ( ( lotto = check_gamble( gamble ) ) > 0 )
          {
            if ( number_range( 1 , GambleLost ) != GambleLost )
            {
              act( "恭禧你﹐你的$t贏得$I兩銀子。"
                , ch, return_horse_name( gamble ) , &lotto, TO_CHAR );

              if ( !over_scale( ch ) ) gold_to_char( ch, lotto );

              /* 送出恭禧的訊息 */
              if ( lotto > LOTTO_CONGULATION )
              {
                sprintf( buf , "%s狗屎運中了大獎﹐趕快跟他分紅吧。"
                  , mob_name( NULL, ch ) );

                talk_channel_2( buf, CHANNEL_GAMBLE, "" );
              }
            }

            else
            {
              act( "真抱歉﹐你的$t上沒有本老大的正字標記﹐所以不算數。"
                , ch, return_horse_name( gamble ), NULL, TO_CHAR );

              if ( lotto > LOTTO_CONGULATION )
              {
                sprintf( buf , "%s狗屎運中了大獎﹐不過被豬場"
                  "老大給黑吃黑了。", mob_name( NULL, ch ) );

                talk_channel_2( buf, CHANNEL_GAMBLE, "" );
              }
            }
          }

          else
          {
            act( "真抱歉﹐你的$t被當作豬會老大的娶老婆本了。"
              , ch, return_horse_name( gamble ), NULL, TO_CHAR );
          }
        }
      }
      extract_gamble( d->character );
    }
    RETURN_NULL();
  }

  /* 開始下注 */
  if ( gamble_tick == 2 )
  {
    talk_channel_2( "飆豬比賽又開始了﹐請各位大爺下注吧。"
      , CHANNEL_GAMBLE, "" );
    RETURN_NULL();
  }

  /* 下注時間, 並且隨機送出下注的訊息 */
  if ( gamble_tick < 5 && gamble_tick > 2 )
  {
    talk_channel_2(
      gamble_message[ number_range( 0, MAX_GAMBLE_MESSAGE- 1) ]
      , CHANNEL_GAMBLE, "" );

    RETURN_NULL();
  }

  /* 開始飆豬 */
  if ( gamble_tick == 5 )
  {
    /* 把每一隻馬的跑的步數清除為零 */
    for ( loop = 0; loop < MAX_HORSES; loop++ ) horse_step[loop] = 0;

    talk_channel_2( "不能再下注了﹐開始飆豬了。", CHANNEL_GAMBLE, "" );
    RETURN_NULL();
  }

  /* 結束飆豬 */
  if ( gamble_tick == 8 )
  {
    /* 把飆豬的結果排一下順序 */
    gamble_order();

    /* 宣告比賽結束, 並且公佈名次 */
    clear_stack();
    send_to_stack( "飆豬名次是 " );

    for ( loop = 0; loop < MAX_HORSES; loop++ )
    {
      send_to_stack( "%s(%d)"
        , horse_name[horse_order[loop]]
        , horse_order[loop] + 1 );
      if ( loop != MAX_HORSES - 1 ) send_to_stack( "-" );
    }

    talk_channel_2( return_stack(), CHANNEL_GAMBLE, "" );

    /* 第一名的豬把記錄加一 */
    horse_data[ horse_order[0] ]++;

    /* 把記數器歸零, 下一部就是發放獎金 */
    gamble_tick = -1;

    RETURN_NULL();
  }
  RETURN_NULL();
}

/* 把豬跑的順序排列一下 */
void gamble_order( void )
{
  int horse_temp[ MAX_HORSES ];
  int loop1;
  int loop2;
  int temp;

  PUSH_FUNCTION( "gmable_order" );

  for ( loop1 = 0; loop1 < MAX_HORSES; loop1++ )
  {
    horse_temp[loop1]  = horse_step[loop1];
    horse_order[loop1] = loop1;
  }

  for ( loop1 = 0; loop1 < MAX_HORSES - 1 ; loop1++ )
  {
    for ( loop2 = loop1 + 1; loop2 < MAX_HORSES; loop2++ )
    {
      if ( horse_temp[loop1] < horse_temp[loop2] )
      {
        temp               = horse_temp[loop1];
        horse_temp[loop1]  = horse_temp[loop2];
        horse_temp[loop2]  = temp;

        temp               = horse_order[loop1];
        horse_order[loop1] = horse_order[loop2];
        horse_order[loop2] = temp;
      }
    }
  }
  RETURN_NULL();
}

/* 傳回買的彩卷的豬隻號碼 */
char * return_horse_name( GAMBLE_DATA * gamble )
{
  int loop;

  PUSH_FUNCTION( "return_horse_name" );

  clear_stack();

  for ( loop = 0; loop < MAX_HORSES; loop++ )
  {
    if ( gamble->horse[loop] == -1 ) break;

    send_to_stack( "%s(%d) "
      , horse_name[gamble->horse[loop]] , gamble->horse[loop] + 1 );
  }
  RETURN( return_stack() );
}

/* 刪除某玩家的賭博資料結構 */
void extract_gamble( CHAR_DATA * ch )
{
  GAMBLE_DATA * temp;
  GAMBLE_DATA * temp_next;

  PUSH_FUNCTION( "extract_gamble" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG , "銷毀玩家賭博資料時人物是空的." );
    RETURN_NULL();
  }

  for ( temp = ch->gamble; temp; temp = temp_next )
  {
    temp_next    = temp->next;

    temp->format = -1;
    temp->mount  =  0;

    /* 釋放結構的記憶體 */
    free_struct( temp , STRUCT_GAMBLE_DATA );
  }

  ch->gamble = NULL;
  RETURN_NULL();
}

/* 檢查是否和結果一樣 */
int check_gamble( GAMBLE_DATA * gamble )
{
  int loop;
  int right;

  PUSH_FUNCTION( "check_gamble" );

  for ( loop = 0, right = 0; loop < MAX_HORSES; loop++ )
  {
    if ( gamble->horse[loop] == -1 ) break;
    if ( gamble->horse[loop] != horse_order[loop] )
    {
      right = 0;
      break;
    }
    right++;
  }

  RETURN ( multipile( right ) * gamble->mount );
}

/* 算出出獎金的倍數 */
int multipile( int number )
{
  int loop;
  int count;

  PUSH_FUNCTION( "multipile" );

  /* 如果小於等於0 , 倍數為 0 */
  if ( number <= 0 ) RETURN( 0 );

  count = 1;
  loop  = MAX_HORSES;

  while ( number-- > 0 ) count *= loop--;

  RETURN( count );
}

/* 檢查買的彩卷是否有效 */
int check_gamble_valid( CHAR_DATA * ch , GAMBLE_DATA * gamble )
{
  int           loop;
  int           same;
  GAMBLE_DATA * temp;
  int           repeat_table[ MAX_HORSES ];

  PUSH_FUNCTION( "check_gamble_valid" );

  for ( temp = ch->gamble ; temp; temp = temp->next )
  {
    for ( same = TRUE, loop = 0; loop < MAX_HORSES; loop++ )
      if ( temp->horse[loop] != gamble->horse[loop] ) same = FALSE;

    if ( same == TRUE ) RETURN( FALSE );
  }

  for ( loop = 0; loop < MAX_HORSES; loop++ ) repeat_table[loop] = -1;

  for ( loop = 0; loop < MAX_HORSES; loop++ )
  {
    if ( gamble->horse[loop] == -1 ) break;
    if ( repeat_table[gamble->horse[loop]] != -1 ) RETURN( FALSE );
    repeat_table[gamble->horse[loop]]++;
  }
  RETURN( TRUE );
}

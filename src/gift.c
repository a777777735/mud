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
#include <time.h>
#include "merc.h"

void set_gift_time( GIFT_DATA * pGift )
{
  struct tm * DateData;
  struct tm   GiftDate;
  time_t      nowtime;

  PUSH_FUNCTION( "set_gift_time" );

  if ( !pGift )
  {
    mudlog( LOG_DEBUG, "set_gift_time: 缺乏來源." );
    RETURN_NULL();
  }

  nowtime = time( NULL );

  if ( !( DateData = localtime( &nowtime ) ) )
  {
    pGift->starting = -1;
    pGift->ending   = -1;
    RETURN_NULL();
  }

  GiftDate.tm_sec   = 0;
  GiftDate.tm_min   = 0;
  GiftDate.tm_hour  = 0;
  GiftDate.tm_mday  = pGift->day;
  GiftDate.tm_mon   = pGift->month - 1;
  GiftDate.tm_year  = DateData->tm_year;
  GiftDate.tm_wday  = DateData->tm_wday;
  GiftDate.tm_wday  = DateData->tm_wday;
  GiftDate.tm_isdst = DateData->tm_isdst;
  pGift->starting = mktime( &GiftDate );

  GiftDate.tm_sec   = 0;
  GiftDate.tm_min   = 0;
  GiftDate.tm_hour  = 0;
  GiftDate.tm_mday  = pGift->day + pGift->days;
  GiftDate.tm_mon   = pGift->month - 1;
  GiftDate.tm_year  = DateData->tm_year;
  GiftDate.tm_wday  = DateData->tm_wday;
  GiftDate.tm_wday  = DateData->tm_wday;
  GiftDate.tm_isdst = DateData->tm_isdst;

  pGift->ending = mktime( &GiftDate );

  RETURN_NULL();
}

void check_gift( void )
{
  GIFT_DATA * pGift;
  time_t      nowtime;

  PUSH_FUNCTION( "check_gift" );

  if ( !gift_list ) RETURN_NULL();
  nowtime = time( NULL );

  for ( pGift = gift_list; pGift; pGift = pGift->next )
  {
    if ( pGift->starting <= nowtime && pGift->ending >= nowtime )
    {
      if ( !pGift->send )
      {
        pGift->send = TRUE;
        pGift->tick = pGift->duration;
      }
    }

    else
    {
      pGift->send = FALSE;
      pGift->tick = -1;
    }
  }

  RETURN_NULL();
}

void gift_update( void )
{
  GIFT_DATA       * pGift;
  CHAR_DATA       * ch;
  DESCRIPTOR_DATA * zDesc;
  DESCRIPTOR_DATA * pDesc;
  char              buf[MAX_STRING_LENGTH];
  int               count;

  PUSH_FUNCTION( "gift_update" );

  if ( !gift_list ) RETURN_NULL();

  check_gift();

  for ( pGift = gift_list; pGift; pGift = pGift->next )
  {
    if ( !pGift->send || !pGift->gift || --pGift->tick > 0 ) continue;

    zDesc = NULL;
    count = 0;
    for ( pDesc = descriptor_list; pDesc; pDesc = pDesc->next )
    {
      if ( verify_desc( pDesc )
        && pDesc->connected == CON_PLAYING
        && verify_char( ( ch = pDesc->character ) )
        && !IS_NPC( ch )
        && ch->level >= pGift->low
        && ch->level <= pGift->high
        && check_gift_stamp( ch, pGift->stamp )
        && ch->jail <= 0
        && ch->in_room
        && !ch->fighting
        && ch->position == POS_STANDING
        && ( ch->pcdata && ch->pcdata->file_size <= file_quota )
        && !IS_SET( ch->act, PLR_KILLER )
        && !IS_SET( ch->act, PLR_THIEF )
        && !IS_SET( ch->act, PLR_BOLTER )
        && number_range( 0, count++ ) == 0 ) zDesc = pDesc;
    }

    if ( zDesc && ( ch = zDesc->character ) )
    {
      MOB_INDEX_DATA * pMobIndex;
      OBJ_DATA       * Gift;

      if ( ( pMobIndex = get_mob_index( pGift->sender ) )
        && ( Gift = create_object( pGift->gift, -1 ) ) )
      {
        if ( pGift->message && ch->in_room )
          sendmsg_to_someroom( pGift->message, ch->in_room );

        sprintf( buf, "%s說道﹕「這是送給你的禮物\e[1;32m%s\e[0m﹐"
          "請你收下﹐祝你\e[1;33m%s\e[0m愉快﹗」\n\r"
          , mob_index_name( ch, pMobIndex )
          , obj_name( ch, Gift )
          , pGift->title );

        send_to_char( buf, ch );

        if ( Gift->Takeable
          || ( ch->carry_number + get_obj_number( Gift ) > can_carry_n( ch ) )
          || ( get_carry_weight( ch ) + get_obj_weight( Gift ) > can_carry_w( ch ) ) )
        {
          act( "因為你拿不動$p﹐所以就把它給到地上了﹗", ch, Gift, NULL, TO_CHAR );
          obj_to_room( Gift, ch->in_room );
          WAIT_STATE( ch, 10 );
        }

        else
        {
          obj_to_char( Gift, ch );
        }

        if ( pGift->gold > 0 && !over_scale( ch ) )
        {
          sprintf( buf, "%s說道﹕「這是送給你的禮金 \e[1;32m%d\e[0m 兩﹐"
            "請你收下﹐祝你\e[1;33m%s\e[0m愉快﹗」\n\r"
            , mob_index_name( ch, pMobIndex )
            , pGift->gold
            , pGift->title );

          send_to_char( buf, ch );
          gold_to_char( ch, pGift->gold );
        }

        set_gift_stamp( ch, pGift->stamp );
        pGift->count++;
      }
    }

    pGift->tick = pGift->duration;
  }

  RETURN_NULL();
}

FUNCTION( do_gift )
{
  MOB_INDEX_DATA * pIndex;
  GIFT_DATA      * pGift;
  char             arg[MAX_INPUT_LENGTH];
  int              slot;
  int              loop;

  PUSH_FUNCTION( "do_gift" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  if ( !gift_list )
  {
    act( "對不起﹐$t沒有任何禮物的資料﹗", ch, mud_name, NULL, TO_CHAR );
    RETURN_NULL();
  }

  argument = one_argument( argument ,arg );

  if ( arg[0] == '\x0' )
  {
    clear_buffer();

    send_to_buffer( "\e[1;33;44m順序 禮  物  標  題     日 期 "
      "發送 次數 序    號 間隔 剩餘 發送者 等   級 禮  金\e[0m\n\r" );

    for ( loop = 1, pGift = gift_list; pGift; pGift = pGift->next, loop++ )
    {
      send_to_buffer( "%4d %-18s %2d-%2d  %2s  %4d %8d %4d %4d %6d"
        " %3d-%3d %6d\n\r"
        , loop
        , pGift->title
        , pGift->month
        , pGift->day
        , YESNO( pGift->send )
        , pGift->count
        , pGift->stamp
        , pGift->duration
        , pGift->tick
        , pGift->sender
        , pGift->low
        , pGift->high
        , pGift->gold );
    }

    print_buffer( ch );
  }

  else if ( is_number( arg ) )
  {
    slot = atoi( arg );

    for ( loop = 1, pGift = gift_list; pGift; pGift = pGift->next, loop++ )
      if ( loop == slot ) break;

    if ( !pGift )
    {
      send_to_char( "對不起﹐沒有那個序號的禮物資料﹗\n\r", ch );
      RETURN_NULL();
    }

    pIndex = get_mob_index( pGift->sender );

    print_to_char( ch,
      "編    號﹕%d\n\r"
      "禮物標題﹕%s\n\r"
      "發送日期﹕%d 月 %d 號\n\r"
      "是否到期﹕%s\n\r"
      "舉辦天數﹕%d 天\n\r"
      "發送次數﹕%d\n\r"
      "禮物序號﹕%d\n\r"
      "發送間隔﹕%d 小時\n\r"
      "剩餘間隔﹕%d 小時\n\r"
      "發送時脈﹕%d\n\r"
      "截止時脈﹕%d\n\r"
      "目前時脈﹕%d\n\r"
      "發 送 者﹕%s 「編號﹕%d」\n\r"
      "等級限制﹕%d 到 %d 之間\n\r"
      "禮品名稱﹕%s 「編號﹕%d」\n\r"
      "禮金數量﹕%d 兩黃金\n\r"
      "發送訊息﹕%s\n\r"
      , loop
      , pGift->title
      , pGift->month
      , pGift->day
      , YESNO( pGift->send )
      , pGift->days
      , pGift->count
      , pGift->stamp
      , pGift->duration
      , pGift->tick
      , pGift->starting
      , pGift->ending
      , time( NULL )
      , pIndex ? mob_index_name( NULL, pIndex ) : "從缺"
      , pGift->sender
      , pGift->low
      , pGift->high
      , pGift->gift ? pGift->gift->short_descr : "從缺"
      , pGift->gift ? pGift->gift->vnum : -1
      , pGift->gold
      , pGift->message );
  }

  else
  {
    send_to_char( "對不起﹐你的語法錯誤﹐請查詢 gift 的使用方法﹗\n\r", ch );
  }
  RETURN_NULL();
}

bool check_gift_stamp( CHAR_DATA * ch, int number )
{
  STAMP_DATA * pStamp;

  PUSH_FUNCTION( "check_gift_stamp" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "check_gift_stamp: 來源不正確." );
    RETURN( FALSE );
  }

  if ( IS_NPC( ch ) ) RETURN( FALSE );

  for ( pStamp = ch->stamp; pStamp; pStamp = pStamp->next )
    if ( pStamp->number == number ) RETURN( FALSE );

  RETURN( TRUE );
}

void set_gift_stamp( CHAR_DATA * ch, int number )
{
  STAMP_DATA * pStamp;

  PUSH_FUNCTION( "set_gift_stamp" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "set_gift_stamp: 缺乏來源." );
    RETURN_NULL();
  }

  if ( IS_NPC( ch ) ) RETURN_NULL();

  for ( pStamp = ch->stamp; pStamp; pStamp = pStamp->next )
    if ( pStamp->number == number ) RETURN_NULL();

  pStamp = alloc_struct( STRUCT_STAMP_DATA );
  pStamp->number = number;
  pStamp->next   = ch->stamp;
  ch->stamp      = pStamp;

  RETURN_NULL();
}

bool is_gift_stamp( int number )
{
  GIFT_DATA * pGift;

  PUSH_FUNCTION( "is_gift_stamp" );

  for ( pGift = gift_list; pGift; pGift = pGift->next )
  {
    if ( pGift->stamp == number && time( NULL ) <= pGift->ending + 864000 )
      RETURN( TRUE );
  }

  RETURN( FALSE );
}

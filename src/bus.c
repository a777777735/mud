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

int     bus_tick        =       0;
char *  company_name    =       "三國國營驛站公司";
char *  platform_descr  =
  "  這裡是驛站的候車處﹐若是想坐車請於此等候帶位\n\r";

char *  loge_descr      =
  "這是三國最大的國營驛馬車﹐光看這規模就知道的確花了一番心思來佈置﹐" \
  "雕樑畫棟﹐跑起來是如此的舒服快捷﹐令人想一坐再坐。\n\r";

char *  platform_short  =       "候車處";
char *  loge_short      =       "車廂";

BUS_DATA *      bus_next        args( ( BUS_DATA * ) );

FUNCTION( do_bus )
{
  char       arg[MAX_INPUT_LENGTH];
  char       buf[MAX_STRING_LENGTH];
  BUS_DATA * pBus;
  int        count;

  PUSH_FUNCTION( "do_bus" );

  if ( IS_NPC( ch ) || !ch->in_room ) RETURN_NULL();

  if ( !bus_first )
  {
    act( "對不起﹐$t沒有任何驛站。", ch, mud_name, NULL, TO_CHAR );
    RETURN_NULL();
  }

  one_argument( argument, arg );

  /* 沒有則是查詢驛站 */
  if ( !arg[0] )
  {
    if ( ( pBus = is_platform( ch->in_room ) ) )
    {
      clear_buffer();

      send_to_buffer( "本站是%s﹐下一站是%s。 "
        , pBus->name, bus_next( pBus )->name );

      if ( bus_tick > 0 && bus_tick < 100 )
      {
        chinese_number( 100 - bus_tick, buf );
        send_to_buffer( "本列車在%s分後開動。\n\r", buf );
      }

      else
      {
        chinese_number( 300 - bus_tick, buf );
        send_to_buffer( "下班車在%s分後到達。\n\r", buf );
      }

      print_buffer( ch );
    }

    else if ( ( pBus = is_loge( ch->in_room ) ) )
    {
      chinese_number( 300 - bus_tick, buf );
      act( "你正在往$t的列車上﹐預估在$T分後到達。"
        , ch, bus_next( pBus )->name, buf, TO_CHAR );
    }

    else
    {
      clear_buffer();
      send_to_buffer( "%s的列車資料如下﹕\n\r"
        "\e[1;33;44m編    號 站            名 價      格\e[0m\n\r", mud_name );

      for ( count = 0, pBus = bus_first; pBus; pBus = pBus->next )
      {
        send_to_buffer( "第 %2d 站 「\e[1;33m%-12s\e[0m」 "
          "「$\e[1;32m%5d\e[0m」\n\r"
          , ++count, pBus->name, pBus->cost );

        if ( buffer_full() ) break;
      }
      print_buffer( ch );
    }

    RETURN_NULL();
  }

  /* 查詢路徑 */
  else if ( !str_prefix( arg, "!path" ) && IS_IMMORTAL( ch ) )
  {
    clear_buffer();
    send_to_buffer( "\e[1;33;44m站數 站              名   售 票 處   "
      "月    台   車    廂  車    資  人    數\e[0m\n\r" );

    for ( count = 0, pBus = bus_first; pBus; pBus = pBus->next )
    {
      send_to_buffer( " %3d %-16s    %6d     %6d     %6d     %6d   %6d\n\r"
        , ++count , pBus->name, pBus->station->vnum
        , pBus->platform->vnum, pBus->loge->vnum, pBus->cost, pBus->count );

      if ( buffer_full() ) break;
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  send_to_char( "對不起﹐請用 bus /? 查詢 bus 的正確使用方法。\n\r" , ch );
  RETURN_NULL();
}

/* 檢查是否可以是地鐵上下站 */
bool check_station( ROOM_INDEX_DATA * pRoom )
{
  PUSH_FUNCTION( "check_station" );
  if ( !pRoom || pRoom->exit[DIR_DOWN] ) RETURN( FALSE );
  RETURN( TRUE );
}

/* 建立地鐵站月台 */
ROOM_INDEX_DATA * create_platform( int vnum, char * name, AREA_DATA * pArea )
{
  ROOM_INDEX_DATA * pRoom;
  int               iHash;
  int               loop;
  char              buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "create_platform" );

  if ( vnum <= 0 || !pArea ) RETURN( NULL );

  /* 先建立起站名 */
  sprintf( buf, "%s%s", name, platform_short );

  pRoom = alloc_struct( STRUCT_ROOM_INDEX_DATA );

  pRoom->people          = NULL;
  pRoom->contents        = NULL;
  pRoom->extra_descr     = NULL;
  pRoom->club            = NULL;
  pRoom->job             = NULL;
  pRoom->area            = pArea;
  pRoom->name            = str_dup( buf );
  pRoom->description     = str_dup( platform_descr );
  pRoom->filename        = str_dup( "系統建立" );
  pRoom->vnum            = vnum;
  pRoom->Safe            = TRUE;
  pRoom->NoRecall        = TRUE;
  pRoom->NoWhere         = TRUE;
  pRoom->NoQuit          = TRUE;
  pRoom->NoMob           = TRUE;
  pRoom->NoLink          = TRUE;
  pRoom->light           = 0;
  pRoom->sector          = DefaultSector;
  iHash                  = pRoom->vnum % MAX_KEY_HASH;
  pRoom->next            = room_index_hash[iHash];
  room_index_hash[iHash] = pRoom;
  top_room++;

  for ( loop = 0; loop < DIR_MAX; loop++ ) pRoom->exit[loop] = NULL;
  RETURN( pRoom );
}

/* 建立地鐵站車廂 */
ROOM_INDEX_DATA * create_loge( int vnum , char * name , AREA_DATA * pArea )
{
  ROOM_INDEX_DATA * pRoom;
  int               iHash;
  int               loop;
  char              buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "create_loge" );

  if ( vnum <= 0 || !pArea ) RETURN( NULL );

  /* 先建立起名稱 */
  sprintf( buf, "%s%s", name, loge_short );

  pRoom = alloc_struct( STRUCT_ROOM_INDEX_DATA );

  pRoom->people          = NULL;
  pRoom->contents        = NULL;
  pRoom->extra_descr     = NULL;
  pRoom->area            = pArea;
  pRoom->name            = str_dup( buf );
  pRoom->description     = str_dup( loge_descr );
  pRoom->filename        = str_dup( "系統建立" );
  pRoom->vnum            = vnum;
  pRoom->Safe            = TRUE;
  pRoom->NoRecall        = TRUE;
  pRoom->NoWhere         = TRUE;
  pRoom->NoQuit          = TRUE;
  pRoom->NoMob           = TRUE;
  pRoom->NoLink          = TRUE;
  pRoom->light           = 0;
  pRoom->sector          = DefaultSector;
  iHash                  = pRoom->vnum % MAX_KEY_HASH;
  pRoom->next            = room_index_hash[iHash];
  room_index_hash[iHash] = pRoom;
  top_room++;

  for ( loop = 0; loop < DIR_MAX; loop++ ) pRoom->exit[loop] = NULL;
  RETURN( pRoom );
}

/* 是否為地鐵的月台﹐或是包廂﹐這些是系統建立起來的﹐實際不存在的 */
BUS_DATA * is_station( ROOM_INDEX_DATA * pRoom )
{
  BUS_DATA * pBus;

  PUSH_FUNCTION( "is_station" );

  if ( !bus_first || !pRoom ) RETURN( NULL );

  for ( pBus = bus_first; pBus; pBus = pBus->next )
    if ( pBus->platform == pRoom || pBus->loge == pRoom ) RETURN( pBus );

  RETURN( NULL );
}

/* 是否為月台 */
BUS_DATA * is_platform( ROOM_INDEX_DATA * pRoom )
{
  BUS_DATA * pBus;

  PUSH_FUNCTION( "is_platform" );

  if ( !bus_first || !pRoom ) RETURN( NULL );

  for ( pBus = bus_first; pBus; pBus = pBus->next )
    if ( pBus->platform == pRoom ) RETURN( pBus );

  RETURN( NULL );
}

/* 是否為車廂 */
BUS_DATA * is_loge( ROOM_INDEX_DATA * pRoom )
{
  BUS_DATA * pBus;

  PUSH_FUNCTION( "is_loge" );

  if ( !bus_first || !pRoom ) RETURN( NULL );

  for ( pBus = bus_first; pBus; pBus = pBus->next )
    if ( pBus->loge == pRoom ) RETURN( pBus );

  RETURN( NULL );
}

/* 處理是否在地鐵站存檔 */
int save_room( ROOM_INDEX_DATA * pRoom )
{
  BUS_DATA        * pBus;

  PUSH_FUNCTION( "save_room" );

  if ( ( pBus = is_station( pRoom ) ) ) RETURN( pBus->station->vnum )
  RETURN( pRoom->vnum );
}

/* 上下連結車站路線 */
bool link_path( ROOM_INDEX_DATA * aRoom, ROOM_INDEX_DATA * bRoom )
{
  EXIT_DATA * aExit;
  EXIT_DATA * bExit;

  PUSH_FUNCTION( "link_path" );

  /* 配置記憶體 */
  set_exit_default( aExit = alloc_struct( STRUCT_EXIT_DATA ) );
  set_exit_default( bExit = alloc_struct( STRUCT_EXIT_DATA ) );

  if ( !aRoom || !bRoom || !aExit || !bExit ) RETURN( FALSE );

  /* 清除基本值 */
  aExit->description = bExit->description = str_dup( "" );
  aExit->keyword     = bExit->keyword     = str_dup( "" );
  aExit->closed      = bExit->closed      = FALSE;
  aExit->isdoor      = bExit->isdoor      = FALSE;
  aExit->locked      = bExit->locked      = FALSE;
  aExit->pick        = bExit->pick        = FALSE;
  aExit->key         = bExit->key         = 0;

  aExit->message     = str_dup( "你走進了驛站裡面﹗" );
  bExit->message     = str_dup( "你從驛站走了出來﹗" );

  /* 建立連結 */
  aExit->vnum = bRoom->vnum;
  bExit->vnum = aRoom->vnum;

  aRoom->exit[DIR_DOWN] = aExit;
  bRoom->exit[DIR_UP  ] = bExit;

  RETURN( TRUE );
}

void bus_update( void )
{
  BUS_DATA        * pBus;
  CHAR_DATA       * ch;
  CHAR_DATA       * ch_next;
  ROOM_INDEX_DATA * aRoom;
  ROOM_INDEX_DATA * bRoom;
  char              buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "bus_update" );

  /* 如果系統沒有載入任何驛站系統 */
  if ( !bus_first ) RETURN_NULL();

  /* 計數 */
  if ( ++bus_tick > 300 ) bus_tick = 0;

  switch( bus_tick )
  {
  /* 送出到站訊息 */
  case 0:

    for ( pBus = bus_first; pBus; pBus = pBus->next )
    {
      sprintf( buf,
        "\e[1;33m%s站到了﹐請要下車的旅客在此下車﹐"
        "出口方向在上方\e[0m。\n\r %s 竭誠歡迎你的搭乘。\n\r"
        , bus_next( pBus )->name , company_name );
      sendmsg_to_someroom( buf, pBus->loge );
    }

    for ( pBus = bus_first; pBus; pBus = pBus->next )
    {
      str_cpy( buf,
        "\e[1;33m列車進站了﹐請大家不要靠在鐵路邊﹐"
        "以防危險。\e[0m\n\r" );

      sendmsg_to_someroom( buf, bus_next( pBus )->platform );
    }

    break;

  /* 搬移旅客 */
  case 1:

    for ( pBus = bus_first; pBus; pBus = pBus->next )
    {
      /* 計算先前的和之後的房間 */
      aRoom = pBus->loge;
      bRoom = bus_next( pBus )->platform;

      for ( ch = aRoom->people; ch; ch = ch_next )
      {
        ch_next = ch->next_in_room;

        if ( ch->fighting ) stop_fighting( ch, TRUE );

        if ( ch->position == POS_RESTING || ch->position == POS_SLEEPING )
        {
          send_to_char( "喂﹐列車到站了﹐該起床了。\n\r" , ch );

          /* 如果是昏睡 */
          if ( !is_affected( ch, SLOT_SLEEP ) ) ch->position = POS_STANDING;
        }

        act( "$n從這班列車走了出去。", ch, NULL, NULL, TO_ALL );

        char_from_room( ch );
        char_to_room( ch, bRoom );
        act( "$n從這班列車上走了出來。", ch, NULL, NULL, TO_ROOM );
        do_look( ch, "auto" );

        /* 清除追蹤紀錄點 */
        clear_trace( ch, TRUE );
      }
    }
    break;

  /* 送出即將開車的訊息 */
  case 20:
  case 80:

    for ( pBus = bus_first; pBus; pBus = pBus->next )
    {
      sprintf( buf,
        "本列車即將開往%s﹐要下車的旅客請儘速由上方出口離開。\n\r"
        "候車的旅客請稍等片刻等候開車﹐%s竭誠歡迎你的光臨。\n\r"
        , bus_next( pBus)->name , company_name );

      sendmsg_to_someroom( buf, pBus->platform );
    }
    break;

  /* 開車的訊息 */
  case 99:

    for ( pBus = bus_first; pBus; pBus = pBus->next )
    {
      sprintf( buf, "\e[1;33m本列車馬上要出發﹐請旅客們稍作休息。\e[0m\n\r"
        "若不欲上車的旅客﹐請盡速由上方離開。\n\r"
        "本列車發車後即將開往%s﹐%s竭誠歡迎你的光臨。\n\r"
        , bus_next( pBus )->name , company_name );

      sendmsg_to_someroom( buf, pBus->loge );
    }
    break;


  /* 搬移旅客 */
  case 100:

    /* 系統重置, 地鐵關閉 */
    if ( ( reboot_time   > 0 && ( ( reboot_time   - current_time ) < 300 ) )
      || ( shutdown_time > 0 && ( ( shutdown_time - current_time ) < 300 ) ) )
      break;

    for ( pBus = bus_first; pBus; pBus = pBus->next )
    {
      /* 計算先前的和之後的房間 */
      aRoom = pBus->platform;
      bRoom = pBus->loge;

      for ( ch = aRoom->people; ch; ch = ch_next )
      {
        ch_next = ch->next_in_room;

        /* 若是怪物或是錢不夠的不搬 */
        if ( IS_NPC( ch )
          || ch->gold < pBus->cost
          || ch->position != POS_STANDING )
        {
          if ( ch->gold < pBus->cost )
            send_to_char( "你的錢不夠﹐無法搭乘本列車。\n\r" , ch );
          continue;
        }

        /* 扣錢 */
        gold_from_char( ch, pBus->cost );
        pBus->count++;

        /* 搬移人物 */
        act( "$n付了錢﹐搭上了地鐵。", ch, NULL, NULL, TO_ALL );
        char_from_room( ch );
        char_to_room( ch, bRoom );
        act( "$n付了錢﹐走上了這班地鐵。", ch, NULL, NULL, TO_ROOM );

        do_look( ch, "auto" );

        /* 清除追蹤紀錄點 */
        clear_trace( ch, TRUE );
      }
    }
    break;

  /* 開車的訊息 */
  case 101:

    for ( pBus = bus_first; pBus; pBus = pBus->next )
    {
      sprintf( buf, "\e[1;33m本列車已經出發﹐請旅客們稍作休息。\e[0m\n\r"
        "本列車即將開往%s。 %s竭誠歡迎你的光臨。\n\r"
        , bus_next( pBus )->name , company_name );

      sendmsg_to_someroom( buf, pBus->loge );
      sendmsg_to_someroom( "\e[1;33m列車已經出發﹐欲乘坐者﹐請"
        "稍待下一班。\e[0m\n\r", pBus->platform );
    }

    break;

  /* 送出即將到站的訊息 */
  case 150:
  case 200:
  case 250:

    for ( pBus = bus_first; pBus; pBus = pBus->next )
    {
      sprintf( buf, "本列車正開往下一站%s﹐請旅客稍作休息﹐耐心等候。\n\r"
        "%s竭誠歡迎你的光臨。\n\r"
        , bus_next( pBus )->name , company_name );
      sendmsg_to_someroom( buf, pBus->loge );
    }

    break;
  }

  RETURN_NULL();
}

/* 下一站 */
BUS_DATA * bus_next( BUS_DATA * pBus )
{
  PUSH_FUNCTION( "bus_next" );

  if ( !pBus )      RETURN( NULL );
  if ( pBus->next ) RETURN( pBus->next );
  RETURN( bus_first );
}

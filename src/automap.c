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

char *          show_a_trace    args( ( TRACE_DATA * ) );

FUNCTION( do_automap )
{
  char              arg[MAX_INPUT_LENGTH];
  char              buf[MAX_STRING_LENGTH];
  PC_DATA         * pc;
  ROOM_INDEX_DATA * pRoom;
  ROOM_INDEX_DATA * zRoom;
  ROOM_INDEX_DATA * aRoom;
  AREA_DATA       * aStart;
  AREA_DATA       * aEnd;
  TRACE_DATA      * pTrace;
  DESCRIPTOR_DATA * pDesc;
  int               loop;
  bool              HasExit;

  PUSH_FUNCTION( "do_automap" );

  if ( IS_NPC( ch ) || !( pc = ch->pcdata ) || !( pRoom = ch->in_room ) )
    RETURN_NULL();

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    if ( !( pTrace = pc->trace ) )
    {
      send_to_char( "對不起﹐你目前沒有鎖定紀錄路徑﹗\n\r", ch );
      RETURN_NULL();
    }

    print_to_char( ch, "你目前鎖定的路徑資料為﹕\n\r%s%s%s"
      , VERTICAL_LINE, show_a_trace( pTrace ) , VERTICAL_LINE );
  }

  else if ( arg[0] == '!' )
  {
    if ( arg[1] == '\x0' )
    {
      send_to_char( "請註明你的幾號紀錄﹖\n\r", ch );
      RETURN_NULL();
    }

    loop = atoi( arg + 1 ) - 1;

    if ( loop < 0 || loop >= MAX_TRACE )
    {
      send_to_char( "對不起﹐你的號碼不合法﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !( pTrace = pc->map[loop] ) )
    {
      send_to_char( "對不起﹐你那個紀錄是空的﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( pTrace->ending != pRoom->vnum )
    {
      send_to_char( "對不起﹐目前不是在那個終點﹐所以無法啟動﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !( pDesc = ch->desc ) )
    {
      send_to_char( "此人無描述表﹐無法啟動﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->position != POS_STANDING )
    {
      send_to_char( "你還是先忙完你手邊的東西吧﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( is_pk( ch ) )
    {
      send_to_char( "你還是專心你的武鬥大會吧﹗\n\r", ch );
      RETURN_NULL();
    }

    for ( buf[0] = '\x0', loop = MAX_PATH - 1 ; loop >= 0; loop-- )
    {
      if ( pTrace->path[loop] == ERRORCODE ) continue;

      switch( pTrace->path[loop] )
      {
      case DIR_NORTH:
        str_cat( buf, "s\n\r" );
        break;

      case DIR_SOUTH:
        str_cat( buf, "n\n\r" );
        break;

      case DIR_EAST:
        str_cat( buf, "w\n\r" );
        break;

      case DIR_WEST:
        str_cat( buf, "e\n\r" );
        break;

      case DIR_DOWN:
        str_cat( buf, "u\n\r" );
        break;

      case DIR_UP:
        str_cat( buf, "d\n\r" );
        break;

      case DIR_ENTER:
        str_cat( buf, "ou\n\r" );
        break;

      case DIR_OUT:
        str_cat( buf, "en\n\r" );
        break;
      }
    }

    if ( buf[0] == '\x0' )
    {
      send_to_char( "無效的紀錄﹗\n\r", ch );
      RETURN_NULL();
    }

    buf[ UMAX( 0, sizeof( pDesc->inbuf ) - 100 ) ] = '\x0';

    send_to_char( "啟動紀錄路徑模式﹗\n\r", ch );
    str_cpy( pDesc->inbuf , buf );
    RETURN_NULL();
  }

  else if ( is_number( arg ) )
  {
    loop = atoi( arg ) - 1;

    if ( loop < 0 || loop >= MAX_TRACE )
    {
      send_to_char( "對不起﹐你的號碼不合法﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !( pTrace = pc->map[loop] ) )
    {
      send_to_char( "對不起﹐你那個紀錄是空的﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( pTrace->starting != pRoom->vnum )
    {
      send_to_char( "對不起﹐目前不是在那個起始點﹐所以無法啟動﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !( pDesc = ch->desc ) )
    {
      send_to_char( "此人無描述表﹐無法啟動﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->position != POS_STANDING )
    {
      send_to_char( "你還是先忙完你手邊的東西吧﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( is_pk( ch ) )
    {
      send_to_char( "你還是專心你的武鬥大會吧﹗\n\r", ch );
      RETURN_NULL();
    }

    for ( buf[0] = '\x0', loop = 0; loop < MAX_PATH; loop++ )
    {
      if ( pTrace->path[loop] == ERRORCODE ) break;

      switch( pTrace->path[loop] )
      {
      case DIR_NORTH:
        str_cat( buf, "n\n\r" );
        break;

      case DIR_SOUTH:
        str_cat( buf, "s\n\r" );
        break;

      case DIR_EAST:
        str_cat( buf, "e\n\r" );
        break;

      case DIR_WEST:
        str_cat( buf, "w\n\r" );
        break;

      case DIR_DOWN:
        str_cat( buf, "e\n\r" );
        break;

      case DIR_UP:
        str_cat( buf, "u\n\r" );
        break;

      case DIR_ENTER:
        str_cat( buf, "en\n\r" );
        break;

      case DIR_OUT:
        str_cat( buf, "ou\n\r" );
        break;
      }
    }

    if ( buf[0] == '\x0' )
    {
      send_to_char( "無效的紀錄﹗\n\r", ch );
      RETURN_NULL();
    }

    buf[ UMAX( 0, sizeof( pDesc->inbuf ) - 100 ) ] = '\x0';

    send_to_char( "啟動紀錄路徑模式﹗\n\r", ch );
    str_cpy( pDesc->inbuf , buf );
  }

  else if ( !str_prefix( arg, "lock" ) )
  {
    if ( ch->position != POS_STANDING
      || is_dead( ch )
      || ch->failed > 0
      || ch->jail > 0 )
    {
      send_to_char( "你目前的狀態不適合追蹤路徑﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( is_pk( ch ) )
    {
      send_to_char( "你還是專心的武鬥大會吧﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( is_station( pRoom ) || pRoom->Sail )
    {
      send_to_char( "在這裡是無法紀錄追蹤路徑的喔﹗\n\r", ch );
      RETURN_NULL();
    }

    for ( HasExit = FALSE, loop = 0; loop < DIR_MAX; loop++ )
    {
      if ( pRoom->exit[loop] )
      {
        HasExit = TRUE;
        break;
      }
    }

    if ( !HasExit )
    {
      send_to_char( "對不起﹐這裡沒有出口﹐所以無法追蹤路徑﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !( pTrace = pc->trace ) )
    {
      pTrace     = alloc_struct( STRUCT_TRACE_DATA );
      pc->trace  = pTrace;
    }

    pTrace->starting = pRoom->vnum;
    pTrace->ending   = ERRORCODE;
    for ( loop = 0; loop < MAX_PATH; loop++ ) pTrace->path[loop] = ERRORCODE;

    act( "你鎖定了$2$t$0為紀錄的起點﹗", ch, pRoom->name, NULL, TO_CHAR );
  }

  else if ( !str_prefix( arg, "clear" ) )
  {
    if ( !( pTrace = pc->trace ) )
    {
      send_to_char( "對不起﹐你必須先得有鎖定才能清除﹗\n\r", ch );
      RETURN_NULL();
    }

    free_struct( pc->trace, STRUCT_TRACE_DATA );
    pc->trace = NULL;
    send_to_char( "你已經清除之前的鎖定追蹤﹗\n\r", ch );
  }

  else if ( !str_prefix( arg, "delete" ) )
  {
    if ( argument[0] == '\x0' || !is_number( argument ) )
    {
      send_to_char( "你要刪除哪一個追蹤路徑紀錄﹖\n\r", ch );
      RETURN_NULL();
    }

    loop = atoi( argument ) - 1;

    if ( loop < 0 || loop >= MAX_TRACE )
    {
      send_to_char( "對不起﹐你的號碼不合法﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !pc->map[loop] )
    {
      send_to_char( "對不起﹐你那個紀錄是空的﹗\n\r", ch );
      RETURN_NULL();
    }

    free_struct( pc->map[loop], STRUCT_TRACE_DATA );
    pc->map[loop] = NULL;

    chinese_number( loop + 1, buf );
    act( "你刪除了第$t號的路徑紀錄﹗\n\r", ch, buf, NULL, TO_CHAR );
  }

  else if ( !str_prefix( arg, "show" ) )
  {
    if ( argument[0] == '\x0' || !is_number( argument ) )
    {
      send_to_char( "你要顯示哪一個追蹤路徑紀錄﹖\n\r", ch );
      RETURN_NULL();
    }

    loop = atoi( argument ) - 1;

    if ( loop < 0 || loop >= MAX_TRACE )
    {
      send_to_char( "對不起﹐你的號碼不合法﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !pc->map[loop] )
    {
      send_to_char( "對不起﹐你那個紀錄是空的﹗\n\r", ch );
      RETURN_NULL();
    }

    chinese_number( loop + 1, buf );
    print_to_char( ch, "你第%s號路徑資料為﹕\n\r%s%s%s"
      , buf , VERTICAL_LINE, show_a_trace( pc->map[loop] ) , VERTICAL_LINE );
  }

  else if ( !str_prefix( arg, "list" ) )
  {
    clear_buffer();

    for ( loop = 0; loop < MAX_TRACE; loop++ )
    {
      chinese_number( loop + 1, buf );

      if ( pc->map[loop] )
      {
        aRoom = get_room_index( pc->map[loop]->starting );
        zRoom = get_room_index( pc->map[loop]->ending   );

        aStart = aRoom ? aRoom->area : NULL;
        aEnd   = zRoom ? zRoom->area : NULL;

        send_to_buffer( "%s、%s的%s\e[0m 到 %s的%s\e[0m\n\r"
          , buf
          , aStart ? aStart->name : "某不知名區域"
          , aRoom  ? aRoom->name  : "某不知名地方"
          , aEnd   ? aEnd->name   : "某不知名區域"
          , zRoom  ? zRoom->name  : "某不知名地方" );
      }

      else
      {
        send_to_buffer( "%s、沒有紀錄﹗\n\r", buf );
      }

      if ( buffer_full() ) break;
    }

    print_buffer( ch );
  }

  else if ( !str_prefix( arg, "dump" ) )
  {
    if ( !( pTrace = pc->trace ) )
    {
      send_to_char( "對不起﹐你必須先得有鎖定才能寫入﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !( zRoom = get_room_index( pTrace->starting ) ) )
    {
      send_to_char( "你的紀錄起點已經消失了﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( zRoom == pRoom )
    {
      send_to_char( "你的起始點和終點根本就一樣﹐不需要紀錄﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( pTrace->path[0] == ERRORCODE )
    {
      send_to_char( "你的路徑裡根本沒有紀錄﹐動作取消﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( argument[0] == '\x0' || !is_number( argument ) )
    {
      send_to_char( "你要把追蹤紀錄到哪一個號碼裡﹖\n\r", ch );
      RETURN_NULL();
    }

    loop = atoi( argument ) - 1;

    if ( loop < 0 || loop >= MAX_TRACE )
    {
      send_to_char( "對不起﹐你的輸出號碼不合法﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( pc->map[loop] )
    {
      free_struct( pc->map[loop], STRUCT_TRACE_DATA );
      pc->map[loop] = NULL;
    }

     pTrace->ending = pRoom->vnum;
     pc->map[loop]  = pTrace;
     pc->trace      = NULL;

     chinese_number( loop + 1, buf );
     print_to_char( ch, "你把%s\e[0m到%s\e[0m的路徑紀錄到第%s號紀錄裡﹗\n\r"
       , zRoom->name, pRoom->name, buf );

     RETURN_NULL();
  }

  else
  {
    send_to_char( "你的參數錯誤﹐請查詢 automap 的用法﹗\n\r", ch );
  }

  RETURN_NULL();
}

void clear_trace( CHAR_DATA * ch, bool bShow )
{
  TRACE_DATA * pTrace;

  PUSH_FUNCTION( "clear_trace" );

  if ( !ch || !ch->pcdata || !( pTrace = ch->pcdata->trace ) ) RETURN_NULL();

  free_struct( pTrace, STRUCT_TRACE_DATA );
  ch->pcdata->trace = NULL;

  if ( bShow ) send_to_char( "你走的路徑有點詭異﹐系統無法追蹤﹗\n\r", ch );
  RETURN_NULL();
}

void set_map( CHAR_DATA * ch, int dir )
{
  TRACE_DATA * pTrace;
  int          loop;

  PUSH_FUNCTION( "set_map" );

  if ( !ch || !ch->pcdata || !( pTrace = ch->pcdata->trace ) ) RETURN_NULL();

  for ( loop = 0; loop < MAX_PATH; loop++ )
  {
    if ( pTrace->path[loop] == ERRORCODE )
    {
      switch( dir )
      {
      default:
        break;

      case DIR_NORTH:
      case DIR_EAST:
      case DIR_SOUTH:
      case DIR_WEST:
      case DIR_UP:
      case DIR_DOWN:
      case DIR_ENTER:
      case DIR_OUT:
        pTrace->path[loop] = dir;
        break;
      }

      RETURN_NULL();
    }
  }

  send_to_char( "你的追蹤紀錄點已滿﹐無法繼續紀錄﹗\n\r", ch );
  RETURN_NULL();
}

char * show_a_trace( TRACE_DATA * pTrace )
{
  int               loop;
  bool              bAttach;
  static char       buf[MAX_STRING_LENGTH];
  ROOM_INDEX_DATA * pStart;
  ROOM_INDEX_DATA * pEnd;
  AREA_DATA       * aStart;
  AREA_DATA       * aEnd;

  PUSH_FUNCTION( "show_a_trace" );

  if ( !pTrace )
  {
    mudlog( LOG_DEBUG, "show_a_trace: 缺乏來源." );
    RETURN( "" );
  }

  pStart = get_room_index( pTrace->starting );
  pEnd   = get_room_index( pTrace->ending   );

  aStart = pStart ? pStart->area : NULL;
  aEnd   = pEnd   ? pEnd->area   : NULL;

  sprintf( buf, "%s%s\e[0m的%s\e[0m往%s%s\e[0m的%s\e[0m\n\r路徑﹕"
    , ""
    , aStart ? aStart->name : "不知名區域"
    , pStart ? pStart->name : "不知名地方"
    , ""
    , aEnd   ? aEnd->name   : "不知名區域"
    , pEnd   ? pEnd->name   : "不知名地方" );

  for ( bAttach = FALSE, loop = 0; loop < MAX_PATH; loop++ )
  {
    if ( pTrace->path[loop] == ERRORCODE ) break;

    str_cat( buf, " " );
    str_cat( buf, direction_name( pTrace->path[loop] ) );

    bAttach = TRUE;
  }

  if ( !bAttach ) str_cat( buf, "無" );
  str_cat( buf, "\n\r" );
  RETURN( buf );
}

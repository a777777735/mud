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

#ifdef __FreeBSD__
#include <sys/socket.h>
#endif

#if defined(sun)
#include <sys/socket.h>
#include <strings.h>
#endif

#include <netinet/in.h>
#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <netdb.h>
#include "merc.h"

int             internal_port;

/* 網際連線函數 */
FUNCTION( do_mud )
{
  NET_DATA * pNet;
  char       arg[MAX_INPUT_LENGTH];
  char       arg1[MAX_INPUT_LENGTH];
  char       buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "do_mud" );

  /* 非玩家不能連線傳消息 */
  if ( IS_NPC( ch ) || !ch->desc ) RETURN_NULL();

  argument = one_argument( argument, arg );

  /* 沒有參數就是查詢 */
  if ( !arg[0] )
  {
    /* 呼叫其他函數來工作 */
    do_netstat( ch, "" );
    RETURN_NULL();
  }

  /* 切斷某友站的雙向連線 */
  else if ( !str_prefix( arg, "!disconnect" ) && IS_GOD( ch ) )
  {
    argument = one_argument( argument, arg1 );

    if ( !arg1[0] )
    {
      send_to_char( "你必須註明哪一個站名。\n\r", ch );
      RETURN_NULL();
    }

    if ( !( pNet = get_net_data( arg1, NET_NAME ) ) )
    {
      act( "找不到 $t 站名﹐請用 mud 查詢。", ch, arg1, NULL, TO_CHAR );
      RETURN_NULL();
    }

    /* 檢查輸入管道是否已經連線 */
    if ( valid_socket( pNet, NET_IMPORT ) )
    {
      close_net_socket( pNet, NET_IMPORT );
      act( "切斷對連線 $t 輸入。", ch , pNet->name, NULL, TO_CHAR );
    }

    else
    {
      act( "站名 $t 輸入連線原本就關閉。", ch, pNet->name, NULL, TO_CHAR );
    }

    /* 檢查輸出管道是否已經連線 */
    if ( valid_socket( pNet, NET_EXPORT ) )
    {
      close_net_socket( pNet, NET_EXPORT );
      act( "切斷對連線 $t 輸出。", ch, pNet->name, NULL, TO_CHAR );
    }
    else
    {
      act( "站名 $t 輸出連線原本就關閉。", ch, pNet->name, NULL, TO_CHAR );
    }

    RETURN_NULL();
  }

  /* 恢復某友站的連線 */
  else if ( !str_prefix( arg, "!reconnect" ) && IS_GOD( ch ) )
  {
    argument = one_argument( argument, arg1 );

    if ( !arg1[0] )
    {
      send_to_char( "你必須註明網站。\n\r", ch );
      RETURN_NULL();
    }

    if ( !( pNet = get_net_data( arg1, NET_NAME ) ) )
    {
      act( "找不到 $t 站名﹐請用 mud 查詢。", ch, arg1, NULL, TO_CHAR );
      RETURN_NULL();
    }

    /* 檢查是否已經連線了 */
    if ( valid_socket( pNet, NET_EXPORT ) )
      act( "網站 $t 已經取得輸出連線。", ch, pNet->name, NULL, TO_CHAR );

    /* 看看是否禁止連線 */
    else if ( pNet->export == -2 )
      act( "網站 $t 禁止輸出連線。", ch, pNet->name, NULL, TO_CHAR );

    /* 連線 */
    else init_client( pNet );

    RETURN_NULL();
  }

  /* 清除連線的資料 */
  else if ( !str_prefix( arg, "!clean" ) && IS_GOD( ch ) )
  {
    argument = one_argument( argument, arg1 );

    if ( !arg1[0] )
    {
      send_to_char( "你必須註明網站。\n\r", ch );
      RETURN_NULL();
    }

    if ( !( pNet = get_net_data( arg1, NET_NAME ) ) )
    {
      act( "找不到 $t 站名﹐請用 mud 查詢。", ch, arg1, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( !valid_socket( pNet, NET_EXPORT ) )
    {
      act( "網站 $t 沒有取得連線。", ch, arg, NULL, TO_CHAR );
      RETURN_NULL();
    }

    pNet->message[0] = '\x0';
    act( "網站 $t已經清除輸出緩衝區。", ch, pNet->name, NULL, TO_CHAR );
    RETURN_NULL();
  }

  /* 設定標記狀況 */
  else if ( !str_prefix( arg, "!log" ) && IS_GOD( ch ) )
  {
    argument = one_argument( argument, arg1 );

    if ( !arg1[0] )
    {
      send_to_char( "你必須註明網站。\n\r", ch );
      RETURN_NULL();
    }

    if ( !( pNet = get_net_data( arg1, NET_NAME ) ) )
    {
      act( "找不到 $t 站名﹐請用 mud 查詢。", ch, arg1, NULL, TO_CHAR );
      RETURN_NULL();
    }

    /* 假設之前就已經被標記, 則清除 */
    if ( pNet->log )
    {
      pNet->log = FALSE;
      act( "站名 $t 清除標記。", ch, pNet->name, NULL, TO_CHAR );
    }
    else
    {
      pNet->log = TRUE;
      act( "站名 $t 設定標記。", ch, pNet->name, NULL, TO_CHAR );
    }

    RETURN_NULL();
  }

  /* 禁止雙向連線 */
  else if ( !str_prefix( arg, "!banexternal" ) && IS_GOD( ch ) )
  {
    argument = one_argument( argument, arg1 );

    if ( !arg1[0] )
    {
      send_to_char( "你必須註明網站。\n\r", ch );
      RETURN_NULL();
    }

    if ( !( pNet = get_net_data( arg1, NET_NAME ) ) )
    {
      act( "找不到 $t 站名﹐請用 mud 查詢。", ch, arg1, NULL, TO_CHAR );
      RETURN_NULL();
    }

    /* 輸入部份, 且原本就禁止 */
    if ( pNet->import == -2 )
    {
      pNet->import = -1;
      act( "$t 恢復輸入連線。", ch, arg1, NULL, TO_CHAR );
    }

    else
    {
      /* 如果連線中, 切斷他 */
      if ( valid_socket( pNet, NET_IMPORT ) )
        close_net_socket( pNet, NET_IMPORT );

      /* 標記不准連線 */
      pNet->import = -2;
      act( "$t 不准輸入連線。", ch, arg1, NULL, TO_CHAR );
    }

    /* 輸出埠份, 且原本就禁止 */
    if ( pNet->export == -2 )
    {
      pNet->export = -1;
      init_client( pNet );
      act( "$t 恢復輸出連線。", ch, arg1, NULL, TO_CHAR );
    }
    else
    {
      /* 如果連線中, 切斷他 */
      if ( valid_socket( pNet, NET_EXPORT ) )
        close_net_socket( pNet, NET_EXPORT );

      /* 標記不准連線 */
      pNet->export = -2;
      act( "$t 不准輸出連線。", ch, arg1, NULL, TO_CHAR );
    }
    RETURN_NULL();
  }

  else
  {
    if ( !( pNet = get_net_data( arg, NET_NAME ) ) )
    {
      act( "找不到你的網站 $t。", ch, arg, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( !valid_socket( pNet, NET_EXPORT ) )
    {
      act( "網站 $t 沒有取得連線。", ch, arg, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( !argument[0] )
    {
      send_to_char( "你要隔空傳音什麼﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( pNet->message[0] )
    {
      act( "網站 $t 忙線中。", ch, arg, NULL, TO_CHAR );
      RETURN_NULL();
    }

    sprintf( buf, "%s %s %s", ch->name, host_name( ch->desc ), argument );
    buf[sizeof( pNet->message ) - 1] = '\x0';
    str_cpy( pNet->message, buf );

    send_to_char( "已經幫你傳遞消息了。\n\r", ch );
  }

  RETURN_NULL();
}

/* 查詢網路連線狀況 */
FUNCTION( do_netstat )
{
  NET_DATA * pNet;
  char       arg[MAX_INPUT_LENGTH];
  int        count;

  PUSH_FUNCTION( "do_netstat" );

  one_argument( argument, arg );

  /* 沒有參數就是查詢 */
  if ( !arg[0] )
  {
    clear_buffer();

    send_to_buffer( "順序 網站名稱        網站位址             通訊埠 "
      "輸入 輸出\n\r" );

    for ( count = 0, pNet = net_list; pNet; pNet = pNet->next )
    {
      send_to_buffer( "%-3d. %-15s %-20s %-6d %s   %s\n\r"
        , ++count , pNet->name, pNet->address, pNet->port
        , YESNO( pNet->import >= 0 )
        , YESNO( pNet->export >= 0 ) );

      if ( buffer_full() ) break;
    }

    if ( count == 0 ) send_to_char( "沒有任何的網際連線站名的設定。\n\r", ch );
    else print_buffer( ch );

    RETURN_NULL();
  }

  if ( !( pNet = get_net_data( arg, NET_NAME ) )
    && !( pNet = get_net_data( arg, NET_ADDRESS ) ) )
  {
    act( "沒發現你的站名 $t﹐請輸入 netstat 查詢。", ch, arg, NULL, TO_CHAR );
    RETURN_NULL();
  }

  print_to_char( ch, "站名﹕%s\n\r"
                     "中文站名﹕%s\n\r"
                     "位址﹕%s ( 連接埠 = %5d )\n\r"
                     "輸出﹕[%s] 輸入﹕[%s] 承認﹕[%s] 記錄﹕[%s]\n\r"
                     "讀寫次數 %5d﹕%5d\n\r"
                     "時間限制﹕%5d 輸入時間﹕%5d 輸出時間﹕%5d\n\r"
     , pNet->name, pNet->cname ? pNet->cname : "(從缺)"
     , pNet->address, pNet->port
     , YESNO( pNet->export >= 0 )
     , YESNO( pNet->import >= 0 )
     , YESNO( pNet->valid )
     , YESNO( pNet->log )
     , pNet->nRead, pNet->nWrite, pNet->timer
     , pNet->import_duration, pNet->export_duration );

  RETURN_NULL();
}

/* 是否為有效的連線 */
bool valid_socket( NET_DATA * pNet, int type )
{
  PUSH_FUNCTION( "valid_socket" );

  if ( !pNet ) RETURN( FALSE );

  switch( type )
  {
  default:
    RETURN( FALSE );

  /* 輸入方面 */
  case NET_IMPORT:

    if ( pNet->import >= 0 ) RETURN( TRUE );
    break;

  /* 輸出方面 */
  case NET_EXPORT:

    if ( pNet->export >= 0 ) RETURN( TRUE );
    break;
  }

  RETURN( FALSE );
}

/* 關閉網路連線 */
void close_net_socket( NET_DATA * pNet, int type )
{
  PUSH_FUNCTION( "close_net_socket" );

  if ( !pNet ) RETURN_NULL();

  switch( type )
  {
  default:
    mudlog( LOG_DEBUG, "close_net_socket: 型態 %d 錯誤 .", type );
    RETURN_NULL();

  /* 關閉輸入管道 */
  case NET_IMPORT:

    if ( pNet->import >= 0 )
    {
      close( pNet->import );
      pNet->import          = -1;
      pNet->import_duration = 0;
    }

    RETURN_NULL();

  /* 關閉輸出管道 */
  case NET_EXPORT:

    if ( pNet->export >= 0 )
    {
      close( pNet->export );
      pNet->export          = -1;
      pNet->export_duration = 0;
      pNet->message[0]      = '\x0';
    }

    RETURN_NULL();
  }

  RETURN_NULL();
}

/* 取得某網際資料結構 */
NET_DATA * get_net_data( char * name, int type )
{
  NET_DATA * pNet;

  PUSH_FUNCTION( "get_net_data" );

  switch( type )
  {
  default:
    mudlog( LOG_DEBUG, "get_net_data: 錯誤型態 %d.", type );
    RETURN( NULL );

  /* 用名稱查詢 */
  case NET_NAME:

    for ( pNet = net_list; pNet; pNet = pNet->next )
      if ( !str_cmp( name, pNet->name ) ) RETURN( pNet );

    break;

  /* 用位址查詢 */
  case NET_ADDRESS:

    for ( pNet = net_list; pNet; pNet = pNet->next )
      if ( !str_cmp( name, pNet->address ) ) RETURN( pNet );

    break;
  }

  RETURN( NULL );
}

/* 初始化網際連線 */
void launch_internal( int internal )
{
  struct sockaddr_in   sock;
  NET_DATA           * pNet;
  char                 buf[MAX_HOSTNAME+1];
  int                  size;
  int                  desc;
  int                  addr;

  PUSH_FUNCTION( "launch_internal" );

  size = sizeof( sock );
  getsockname( internal, ( struct sockaddr * ) &sock, &size );

  if ( ( desc = accept( internal, (struct sockaddr *) &sock, &size) ) < 0 )
  {
    mudlog( LOG_INFO, strerror( errno ) );
    mudlog( LOG_INFO, "launch_internal: accept 有問題." );
    RETURN_NULL();
  }

  if ( fcntl( desc, F_SETFL, ( fcntl( desc , F_GETFL ) | O_NONBLOCK ) ) < 0 )
  {
    mudlog( LOG_INFO, strerror( errno ) );
    mudlog( LOG_INFO, "launch_internal: fcntl 有問題" );
    RETURN_NULL();
  }

  size = sizeof(sock);
  if ( getpeername( desc, (struct sockaddr *) &sock, &size ) < 0 )
  {
    mudlog( LOG_NET, "[網際連線] 查詢不到其位址." );
    close( desc );
    RETURN_NULL();
  }

  addr = ntohl( sock.sin_addr.s_addr );
  sprintf( buf, "%d.%d.%d.%d"
    , ( addr >> 24 ) & 0xFF, ( addr >> 16 ) & 0xFF
    , ( addr >>  8 ) & 0xFF, ( addr       ) & 0xFF );

  /* 檢查是否在表格之中, 不是者不能連線 */
  if ( !( pNet = get_net_data( buf, NET_ADDRESS ) ) )
  {
    mudlog( LOG_NET, "[網際連線] %s 未在限定站中.", buf );
    close( desc );
    RETURN_NULL();
  }

  /* 檢查是否標記不准連線 */
  if ( pNet->import == -2 )
  {
    mudlog( LOG_NET, "[網際連線] %s 不准連線.", buf );
    close( desc );
    RETURN_NULL();
  }

  if ( pNet->import >= 0 )
  {
    mudlog( LOG_NET, "[網際連線] %s 重複連線.", buf );
    close_net_socket( pNet, NET_IMPORT );
  }

  pNet->import          = desc;
  pNet->import_duration = 0;

  mudlog( LOG_NET, "[網際連線] %s完成連線", buf );

  /* 檢查雙向 */
  if ( pNet->export == -1 ) init_client( pNet );
  RETURN_NULL();
}

/* 初始化遠端連線 */
bool init_client( NET_DATA * pNet )
{
  int                  sockfd;
  struct sockaddr_in   serv;
  struct hostent     * hp;

  PUSH_FUNCTION( "init_client" );

  if ( !pNet ) RETURN( FALSE );

  if ( ( sockfd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
  {
    mudlog( LOG_NET, "init_client: 無法開啟 socket." );
    RETURN( FALSE );
  }

  serv.sin_family = AF_INET;
  if ( ( hp = gethostbyname( pNet->address ) ) == NULL )
  {
    mudlog( LOG_NET, "init_client: %s 無法取得.", pNet->address );
    close( sockfd );
    RETURN( FALSE );
  }

  bcopy( ( char * ) hp->h_addr, ( char * ) &serv.sin_addr, hp->h_length );
  serv.sin_port = htons( pNet->port );

  if ( connect( sockfd, ( struct sockaddr * ) &serv, sizeof( serv ) ) < 0 )
  {
    mudlog( LOG_NET, "init_client: 無法連接 %s@%d."
      , pNet->address, pNet->port );

    close( sockfd );
    RETURN( FALSE );
  }

  pNet->export          = sockfd;
  pNet->export_duration = 0;
  pNet->message[0]      = '\x0';

  mudlog( LOG_NET, "取得連接 %s@%d", pNet->address, pNet->port );

  RETURN( TRUE );
}

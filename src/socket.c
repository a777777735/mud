/***************************************************************************
*  這是由輔大化學系製作群所撰寫的遊戲﹐主體由 merc 改編而來﹐所有的版權    *
*  將會被保留﹐但歡迎大家修改﹐但我們也希望你們也能提供給大家﹐所有的商    *
*  業行為將不被允許。                                                      *
*                                                                          *
*  paul@mud.ch.fju.edu.tw                                                  *
*  lc@mud.ch.fju.edu.tw                                                    *
*                                                                          *
***************************************************************************/

#include <stdio.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <ctype.h>
#include "merc.h"
#include <arpa/inet.h>

#define TCP_FILENAME    "/proc/net/tcp"

int     hex_to_int      args( ( char *, int ) );

static  jmp_buf timebuf;

/* 限時 */
static void timeout( int sig )
{
  longjmp( timebuf, sig );
}

char * get_user_name( int desc, struct sockaddr_in * there )
{
  struct sockaddr_in   here;          /* 本地的結構 */
  struct sockaddr_in   sin;           /* 對方的結構*/
  int                  length;
  int                  s;
  unsigned int         remote;
  unsigned int         local;
  static char          user[256];
  FILE               * fp;
  char               * cp;
  static char        * result;

  PUSH_FUNCTION( "get_user_name" );

  result = FromUnknown;

  /* 找出本地的位址以及阜 */
  length = sizeof( here );
  if ( getsockname( desc, (struct sockaddr *) & here, &length ) < 0 )
    RETURN( result );

  if ( ( s = socket( AF_INET , SOCK_STREAM , 0 ) ) < 0 ) RETURN( result );

  sin          = here;
  sin.sin_port = 0;
  if ( bind( s, (struct sockaddr *)&sin, sizeof(sin) ) < 0 ) RETURN( result );

  /* 設定限時 */
  signal( SIGALRM, timeout );

  /* 第一次呼叫一定為 0, 若是 timeout 後才會跳到這裡 */
  if ( setjmp( timebuf ) != 0 )
  {
    close( s );
    RETURN( FromUnknown );
  }

  alarm( SocketTimeOut );

  sin          = *there;
  sin.sin_port = htons( AuthPort );

  if ( connect( s, (struct sockaddr *) &sin, sizeof(sin) ) < 0
    || ( fp = fdopen( s, "w+" ) ) == NULL )
  {
    close( s );
    alarm( 0 );
    RETURN( result );
  }

  fprintf( fp, "%u,%u\r\n", ntohs(there->sin_port), ntohs(here.sin_port) );
  fflush( fp );

  setbuf( fp , ( char * ) NULL );
  if ( fscanf( fp, "%u , %u : USERID :%*[^:]:%255s"
    , &remote , &local , user ) == 3
      && ferror( fp )           == 0
      && feof( fp )             == 0
      && ntohs(there->sin_port) == remote
      && ntohs(here.sin_port)   == local )
  {
    if ( ( cp = strchr( user, '\r' ) ) ) *cp = '\x0';
    result = user;
  }

  alarm( 0 );
  fclose( fp );
  RETURN( result );
}

/* 檢查多重連線的指令 */
FUNCTION( do_multi )
{
  char              arg[MAX_INPUT_LENGTH];
  char              buf[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA * d;
  CHAR_DATA       * victim;
  int               max;
  int               loop;
  int               count;

  PUSH_FUNCTION( "do_multi" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  check_multi_update();
  one_argument( argument , arg );

  if ( !arg[0] )
  {
    /* 檢查最大值 */
    for ( max = 0 , d = descriptor_list; d; d = d->next )
    {
      if ( !verify_desc( d )
        || d->connected != CON_PLAYING
        || !d->character ) continue;
      max = UMAX( max , d->multi );
    }

    /* 根本沒有人多重連線 */
    if ( max == 0 )
    {
      send_to_char( "沒有發現有人多重連線﹗\n\r" , ch );
      RETURN_NULL();
    }

    clear_buffer();
    send_to_buffer( "發現有 %d 組多重連線。分別是﹕" , max );

    for ( loop = 0; loop < max; loop++ )
    {
      send_to_buffer( "\n\r" );
      for ( count = 0 , d = descriptor_list; d; d = d->next )
      {
        if ( !verify_desc( d )
          || d->connected != CON_PLAYING
          || !d->character ) continue;

        if ( d->multi == loop + 1 )
        {
          send_to_buffer( "[%2d-%2d] 工作站: %s %s\e[0m(%s)\e[80C\e[35D"
            , loop + 1
            , ++count
            , CheckServer ? YESNO( d->server ) : "﹖"
            , d->character->cname
            , d->character->name );

          sprintf( buf, "%s(%s)", host_name( d ), d->address );
          buf[35] = '\x0';
          send_to_buffer( "%s\n\r", buf );
        }
      }
    }

    print_buffer( ch );
  }

  else
  {
    if ( ( victim = get_char_world( ch , arg ) ) == NULL )
    {
      act( "沒有找到你的對象 $2$T$0。" , ch, NULL, arg, TO_CHAR );
      RETURN_NULL();
    }

    if ( IS_NPC( victim ) )
    {
      act( "$N是虛擬怪物。", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( !victim->desc )
    {
      act( "$N離線﹐因此沒有描述表可供查詢。", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    check_multi_update();
    if ( victim->desc->multi == 0 )
    {
      act( "$N沒有多重連線的情形。" , ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    clear_buffer();

    send_to_buffer( "與%s 多重連線的有﹕\n\r", mob_name( NULL, victim ) );

    for ( d = descriptor_list , count = 0; d; d = d->next )
    {
      if ( !verify_desc( d )
        || d->connected != CON_PLAYING
        || !d->character
        || d == victim->desc
        || d->multi != victim->desc->multi ) continue;

      send_to_buffer( "[%2d] 工作站: %s %s\e[0m(%s)\e[80C\e[35D"
        , ++count
        , CheckServer ? YESNO( d->server ) : "﹖"
        , d->character->cname
        , d->character->name );

      sprintf( buf, "%s(%s)", host_name( d ), d->address );
      buf[35] = '\x0';
      send_to_buffer( "%s\n\r", buf );
    }

    print_buffer( ch );
    RETURN_NULL();
  }
  RETURN_NULL();
}

/* 檢查是否有多重連線 */
void check_multi_update( void )
{
  DESCRIPTOR_DATA * d;
  DESCRIPTOR_DATA * d_next;
  int               loop;
  bool              touch;

  PUSH_FUNCTION( "check_multi_update" );

  /* 清除資料 */
  for ( d = descriptor_list; d; d = d->next )
    if ( verify_desc( d ) ) d->multi = 0;

  /* 檢查是否有相同簽入位址 */
  for ( loop = 1 , d = descriptor_list; d; d = d->next )
  {
    /* 若未連線完畢者, 或是反查詢未完成不列入計算 */
    if ( !verify_desc( d )
      || d->connected != CON_PLAYING
      || !d->character
      || d->naddr == 0 )
      continue;

    for ( touch = FALSE, d_next = d->next; d_next; d_next = d_next->next )
    {
      /* 若未連線完畢者, 或是反查詢未完成不列入計算 */
      if ( !verify_desc( d_next )
        || d_next->connected != CON_PLAYING
        || !d_next->character
        || d->naddr != d_next->naddr ) continue;

      if ( d->remote
        && d->remote[0]
        && d_next->remote
        && d_next->remote[0]
        && strcmp( d->remote, d_next->remote ) )
        continue;

      if ( d->multi == 0 )
      {
        d->multi = loop;
        touch    = TRUE;
      }

      d_next->multi = d->multi;
    }

    if ( touch ) loop++;
  }

  RETURN_NULL();
}

FUNCTION( do_finger )
{
  CHAR_DATA       * victim;
  DESCRIPTOR_DATA * pDesc;
  DATABASE_DATA   * pData;
  BASIC_DATA      * pBasic;
  CLUB_DATA       * pClub;
  CLASS_DATA      * pClass;
  char              arg[ MAX_INPUT_LENGTH ];
  char              buf[MAX_STRING_LENGTH];
  char              buf1[MAX_INPUT_LENGTH];
  char              buf2[MAX_INPUT_LENGTH];
  char              buf3[MAX_INPUT_LENGTH];
  char              buf4[MAX_INPUT_LENGTH];
  char              buf5[MAX_INPUT_LENGTH];
  char              timebuf[MAX_STRING_LENGTH];
  char              name[MAX_INPUT_LENGTH * 2];
  char              couple[MAX_INPUT_LENGTH];
  int               iTime;
  int               level;
  struct stat       sData;
  struct tm       * sTime;

  PUSH_FUNCTION( "do_finger" );

  one_argument( argument , arg );

  if ( !arg[0] )
  {
    send_to_char( "你想查詢誰的資料﹖\n\r" , ch );
    RETURN_NULL();
  }

  if ( arg[0] == '@' )
  {
    if ( !IS_IMMORTAL( ch ) )
    {
      send_to_char( "你沒有足夠的權力來觀看其他人的資料。\n\r", ch );
      RETURN_NULL();
    }

    if ( !arg[1] )
    {
      send_to_char( "你要觀看誰的資料呢﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( !( victim = get_char_world( ch, arg + 1 ) ) )
    {
      act( "找不到你指定的人物 $2$T$0。" , ch, NULL, arg+1, TO_CHAR );
      RETURN_NULL();
    }

    if ( IS_NPC( victim ) )
    {
      act( "不能查詢非玩家$N。", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( !( pDesc = victim->desc ) )
    {
      act( "$N沒有描述表﹐無法查詢資料。" , ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( pDesc->finger_data && pDesc->finger_data[0] )
      act( "$t", ch, pDesc->finger_data, NULL, TO_CHAR );

    else
      act( "描述表內沒有關於$N的資料。", ch, NULL, victim, TO_CHAR );
  }

  else
  {
    if ( !( pData = name2database( arg ) ) )
    {
      act( "對不起﹐$t沒有 $2$T$0 這一號人物﹗", ch, mud_name, arg, TO_CHAR );
      RETURN_NULL();
    }

    if ( !pData->exist )
    {
      act( "對不起﹐$t已經成為過去式了﹗", ch, arg, NULL, TO_CHAR );
      RETURN_NULL();
    }

    str_cpy( name, file_name( arg, SAVE_FILE ) );
    if ( access( name , R_OK ) != 0 )
    {
      act( "對不起﹐$t沒有$T這一號人物﹗", ch, mud_name, arg, TO_CHAR );
      RETURN_NULL();
    }

    if ( stat( name, &sData ) != 0 )
    {
      act( "讀取 $2$T$0 資料發生錯誤﹗", ch, NULL, arg, TO_CHAR );
      RETURN_NULL();
    }

    if ( !( sTime = localtime( &sData.st_mtime ) ) )
    {
      act( "計算對方 $2$T$0 資料時發生錯誤﹗", ch, NULL, arg, TO_CHAR );
      RETURN_NULL();
    }

    chinese_number( sTime->tm_year - 11 , buf1 );
    chinese_number( sTime->tm_mon  +  1 , buf2 );
    chinese_number( sTime->tm_mday      , buf3 );
    chinese_number( sTime->tm_min       , buf5 );
    chinese_number( sTime->tm_hour > 12 ? sTime->tm_hour - 12 : sTime->tm_hour , buf4 );

    sprintf( timebuf, "民國%s年%s月%s號%s%s點%s分"
      , buf1
      , buf2
      , buf3
      , sTime->tm_hour < 12 ? "早上" : "下午"
      , buf4
      , buf5 );

    for ( victim = char_list; victim; victim = victim->next )
    {
      if ( verify_char( victim )
        && !str_cmp( victim->name, arg )
        && !IS_NPC( victim ) ) break;
    }

    buf[0] = '\x0';
    if ( victim && victim->desc ) str_cpy( buf, host_name( victim->desc ) );

    if ( !( pBasic = serial2basic( &( pData->serial ) ) ) )
    {
      act( "對不起﹐無法讀取$t的資料﹗", ch, arg, NULL, TO_CHAR );
      RETURN_NULL();
    }

    iTime = victim ? victim->played + current_time - victim->logon : pBasic->played;
    level = victim ? victim->level : pBasic->level;

    if ( level >= LEVEL_IMMORTAL && level > ch->level )
      str_cpy( timebuf, "-------------------" );

    if ( pBasic->couple.high > 0 && pBasic->couple.low > 0 )
    {
      BASIC_DATA * pCouple;

      if ( ( pCouple = serial2basic( &( pBasic->couple ) ) ) )
      {
        sprintf( couple, "%s\e[0m(%s)", pCouple->cname, pCouple->name );
        free_basic( pCouple );
      }
      else
      {
        str_cpy( couple, "找不到配偶資料" );
      }
    }
    else
    {
      str_cpy( couple, "沒有配偶" );
    }

    if ( pBasic->club[0] ) pClub = clubname_lookup( pBasic->club, CLUB_NAME );
    else                   pClub = NULL;

    if ( pBasic->class[0] ) pClass = is_class( pBasic->class, TRUE );
    else                    pClass = NULL;

    print_to_char( ch,
      "英文代號    ﹕%s\n\r"
      "中文代號    ﹕%s\e[0m\n\r"
      "電子信箱    ﹕%s\n\r"
      "玩家年齡    ﹕%d 歲\n\r"
      "連線總數    ﹕%s\n\r"
      "等    級    ﹕%d\n\r"
      "性    別    ﹕%s\n\r"
      "配    偶    ﹕%s\n\r"
      "幫    派    ﹕%s\e[0m\n\r"
      "職    業    ﹕%s\n\r"
      "連線位址    ﹕%s\n\r"
      "最後連線時間﹕%s\n\r"
      "檔案大小    ﹕%s 位元組\n\r"
      , arg
      , victim ? victim->cname : pBasic->cname
      , victim ? victim->email : pBasic->email
      , victim ? get_age( victim ) : ( pBasic->played / 14400 ) + BornAge
      , iTime > 0 ? date_string( iTime ) : "不詳"
      , victim ? victim->level : pBasic->level
      , sex_name( pBasic->sex )
      , couple
      , pClub ? pClub->cname : "沒有幫派"
      , class_name( pClass, TRUE )
      , victim ? buf : pBasic->address
      , timebuf
      , numberize( sData.st_size, NULL ) );

    if ( IS_IMMORTAL( ch ) )
    {
      print_to_char( ch,
        "%s"
        "檔案位置    ﹕%s\n\r"
        "檔案擁有者  ﹕%s\n\r"
        "檔案族群者  ﹕%s\n\r"
        "檔案屬性    ﹕%s\n\r"
        , VERTICAL_LINE
        , name
        , pw_lookup( sData.st_uid )
        , gr_lookup( sData.st_gid )
        , perm_of_file( sData.st_mode ) );
    }

    free_basic( pBasic );
    RETURN_NULL();
  }

  RETURN_NULL();
}

char * address_mode( const char * address, int mode )
{
  ADDRESS_DATA * pAddress;

  PUSH_FUNCTION( "address_mode" );

  if ( !address || !*address )
  {
    mudlog( LOG_DEBUG, "address_mode: 來源錯誤." );
    RETURN( "" );
  }

  for ( pAddress = address_list; pAddress; pAddress = pAddress->next )
  {
    if ( !strcmp( pAddress->number, address )
      || !strcmp( pAddress->alpha, address ) )
    {
      switch( mode )
      {
      default:
        RETURN( "" );

      case ADDRESS_ALPHA:
        RETURN( pAddress->alpha );

      case ADDRESS_NUMBER:
        RETURN( pAddress->number );
      }
    }
  }

  RETURN( "" );
}

char * address_lookup( const char * number )
{
  ADDRESS_DATA * pAddress;

  PUSH_FUNCTION( "address_lookup" );

  for ( pAddress = address_list; pAddress; pAddress = pAddress->next )
    if ( !strcmp( pAddress->number , number ) )
      RETURN( pAddress->alpha );

  RETURN( NULL );
}

void set_address_data( const char * number , const char * alpha )
{
  ADDRESS_DATA * pAddress;

  PUSH_FUNCTION( "set_address_data" );

  for ( pAddress = address_list; pAddress; pAddress = pAddress->next )
  {
    if ( !strcmp( pAddress->number, number ) )
    {
      pAddress->count++;
      RETURN_NULL();
    }
  }

  pAddress = alloc_struct( STRUCT_ADDRESS_DATA );
  pAddress->count  = 1;
  pAddress->number = str_dup( number );
  pAddress->alpha  = str_dup( alpha  );
  pAddress->next   = address_list;
  address_list     = pAddress;
  RETURN_NULL();
}

FUNCTION( do_address )
{
  ADDRESS_DATA * pAddress;
  bool           bFound = FALSE;
  char           arg[MAX_INPUT_LENGTH];
  FILE         * pFile;

  PUSH_FUNCTION( "do_address" );

  one_argument( argument , arg );

  if ( !arg[0] )
  {
    clear_buffer();
    for ( pAddress = address_list; pAddress; pAddress = pAddress->next )
    {
      bFound = TRUE;
      send_to_buffer( "%-30s %-30s %5d\n\r"
        , pAddress->number, pAddress->alpha, pAddress->count );
    }

    if ( !bFound ) send_to_buffer( "位址表中沒有任何資料。\n\r" );
    print_buffer( ch );
  }

  else if ( !str_prefix( arg , "!save" ) )
  {
    if ( ( pFile = FOPEN( address_file , "w" ) ) )
    {
      for ( pAddress = address_list; pAddress; pAddress = pAddress->next )
      {
        fprintf( pFile , "%-20s %-30s %5d\n"
          , pAddress->number , pAddress->alpha , pAddress->count );
      }

      if ( ch ) send_to_char( "位址資訊存檔完畢﹗\n\r" , ch );
      FCLOSE( pFile );

      /* 改變檔案存取模式 */
      set_file_mode( address_file );

    }
    else
    {
      mudlog( LOG_DEBUG, "do_address: 無法寫入位址檔案 %s.", address_file );
    }
  }

  else
  {
    clear_buffer();
    for ( pAddress = address_list; pAddress; pAddress = pAddress->next )
    {
      if ( !str_prefix( arg, pAddress->number )
        || !str_prefix( arg, pAddress->alpha  ) )
      {
        bFound = TRUE;
        send_to_buffer( "%-30s %-30s %5d\n\r"
          , pAddress->number, pAddress->alpha, pAddress->count );
      }
    }

    if ( !bFound ) send_to_buffer( "沒有找到相關的位址﹗\n\r" );
    print_buffer( ch );
  }

  RETURN_NULL();
}

int hex_to_int( char * buf, int len )
{
  char * pString;
  char * pChar;
  char   mode[] = "0123456789ABCDEF";
  int    number = 0;
  int    shift  = 1;

  PUSH_FUNCTION( "hex_to_int" );

  if ( !buf || len <= 0 )
  {
    mudlog( LOG_DEBUG, "hex_to_int: 來源不正確." );
    RETURN( -1 );
  }

  for ( pString = buf; *pString; pString++ ) if ( len-- <= 0 ) break;

  for ( --pString; pString >= buf; pString-- )
  {
    if ( *pString != ' ' )
    {
      if ( ( pChar = strchr( mode, toupper( *pString ) ) ) == NULL )
      RETURN( -1 );

      number += ( pChar - mode ) * shift;
    }

    shift *= 16;
  }

  RETURN( number );
}

FUNCTION( do_tcp )
{
  FILE * pFile;
  char   arg[MAX_INPUT_LENGTH];
  char   buf[MAX_STRING_LENGTH];
  char   remote[BUFSIZ];
  bool   fAll = FALSE;
  bool   fOpen;
  int    port;
  int    loop;
  int    count;
  int    listen;
  int    LocalPort;

  PUSH_FUNCTION( "do_listen" )

  if ( IS_NPC( ch ) ) RETURN_NULL();

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    fAll   = TRUE;
    listen = ERRORCODE;
  }
  else
  {
    if ( !is_number( arg ) )
    {
      send_to_char( "你必須輸入數字來表示輸出埠﹗\n\r", ch );
      RETURN_NULL();
    }

    port = atoi( arg );

    for ( listen = ERRORCODE, loop = 0; loop < MAX_PORT; loop++ )
    {
      if ( MudPort[loop] == port )
      {
        listen = port;
        fAll   = FALSE;
        break;
      }
    }

    if ( listen == ERRORCODE )
    {
      send_to_char( "你指定的輸出埠不是合法的﹗\n\r", ch );
      RETURN_NULL();
    }
  }

  if ( !( pFile = fopen( TCP_FILENAME, "r" ) ) )
  {
    act( "無法開啟觀察檔案 $t﹗", ch, TCP_FILENAME, NULL, TO_CHAR );
    RETURN_NULL();
  }

  fseek( pFile, 128, SEEK_SET );

  for ( count = 0, clear_buffer(); ; )
  {
    if ( feof( pFile ) ) break;
    fread( &buf, 128, 1, pFile );

    buf[126] = '\x0';

    if ( hex_to_int( buf + 12, 2 ) == 0
      && hex_to_int( buf + 10, 2 ) == 0
      && hex_to_int( buf + 10, 2 ) == 0
      && hex_to_int( buf + 10, 2 ) == 0 ) continue;

    LocalPort = hex_to_int( buf + 15 , 4 );
    for ( fOpen = FALSE, loop = 0; loop < MAX_PORT; loop++ )
    {
      if ( ( MudPort[loop] == LocalPort && fAll ) || listen == LocalPort )
      {
        fOpen = TRUE;
        break;
      }
    }

    if ( !fOpen ) continue;

    sprintf( remote, "%d.%d.%d.%d"
      , hex_to_int( buf + 26, 2 )
      , hex_to_int( buf + 24, 2 )
      , hex_to_int( buf + 22, 2 )
      , hex_to_int( buf + 20, 2 ) );

    if ( count == 0 ) send_to_buffer( "\e[1;33;44m編號 本地埠 "
      "遠  端  位  址  遠端埠 逾  時 狀態              "
      "                    \e[0m\n\r" );

    send_to_buffer( "%4d %6d %-15s %6d %6d %-s\n\r"
      , ++count
      , hex_to_int( buf + 15, 4 )
      , remote
      , hex_to_int( buf + 29, 4 )
      , hex_to_int( buf + 83, 7 )
      , tcp_status( hex_to_int( buf + 34, 2 ) ) );
  }

  if ( count == 0 ) send_to_buffer( "這個輸出埠沒有任何資料﹗\n\r" );

  print_buffer( ch );
  fclose( pFile );
  RETURN_NULL();
}

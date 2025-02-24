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
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
#include "merc.h"

BOARD_DATA *    load_board_index        args( ( const char * ) );
void            load_post               args( ( const char *, BOARD_DATA * ) );
POST_DATA  *    load_a_post             args( ( const char * ) );
void            set_board_default       args( ( BOARD_DATA * ) );
void            set_post_default        args( ( POST_DATA * ) );
void            show_a_post             args( ( POST_DATA *, CHAR_DATA * ) );
void            purge_post              args( ( BOARD_DATA * ) );
void            attach_post             args( ( POST_DATA *, BOARD_DATA *, bool ) );
void            remove_post             args( ( POST_DATA * ) );
void            write_post              args( ( POST_DATA * ) );
void            rewrite_board_list      args( ( BOARD_DATA * ) );
const char *    post_status             args( ( POST_DATA *, int ) );

#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value ) \
  if ( !str_cmp( word, literal ) )   \
  {                                  \
    field  = value;                  \
    fMatch = TRUE;                   \
    break;                           \
  }

#define POSTER_FIELD    30
#define TITLE_FIELD     30
#define MAX_RESPONSE    500

BOARD_DATA * board_lookup( int slot )
{
  BOARD_DATA * pBoard;

  PUSH_FUNCTION( "board_lookup" );

  for ( pBoard = board_list; pBoard; pBoard = pBoard->next )
    if ( pBoard->slot == slot ) RETURN( pBoard );

  RETURN( NULL );
}

FUNCTION( do_board )
{
  BOARD_DATA * pBoard;
  char         arg[MAX_INPUT_LENGTH];
  int          count;

  PUSH_FUNCTION( "do_board" );

  if ( !board_list )
  {
    act( "$t裡沒有任何的版﹗", ch, mud_name, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( IS_NPC( ch ) || !ch->pcdata ) RETURN_NULL();

  argument = one_argument( argument, arg );
  if ( arg[0] == '\x0' )
  {
    clear_buffer();
    send_to_buffer( "\e[1;33;44m編號 版      名 房間號碼 讀取 "
      "寫入 數量 限制 鎖定                               \e[0m\n\r" );

    for ( count = 0, pBoard = board_list; pBoard; pBoard = pBoard->next )
    {
      send_to_buffer( "%4d %10s %8d %4d %4d %4d %4d  %2s\n\r"
        , pBoard->slot
        , pBoard->name
        , pBoard->location ? pBoard->location->vnum : -1
        , pBoard->read_level
        , pBoard->write_level
        , post_count( pBoard, POST_ALL )
        , pBoard->capcity
        , YESNO( pBoard->lock ) );

      if ( buffer_full() ) break;
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "!lock" ) && IS_IMMORTAL( ch ) )
  {

    if ( !ch->in_room || !( pBoard = ch->in_room->board ) )
    {
      send_to_char( "你這個地方沒有留言版﹗\n\r", ch );
      RETURN_NULL();
    }

    pBoard->lock = pBoard->lock ? FALSE : TRUE;

    act( "$n$t$T留言版﹗"
      , ch, pBoard->lock ? "關閉" : "開放", pBoard->name, TO_ALL );

    RETURN_NULL();
  }

  else if ( is_number( arg ) )
  {
    if ( !( pBoard = board_lookup( atoi( arg ) ) ) )
    {
      send_to_char( "沒有那個編號的版面﹗\n\r", ch );
      RETURN_NULL();
    }

    print_to_char( ch,
      "版面編號  ﹕%d\n\r"
      "版面名稱  ﹕%s\n\r"
      "歡迎字串  ﹕%s\n\r"
      "所在地號碼﹕%d\n\r"
      "列表檔案  ﹕%s\n\r"
      "路徑名稱  ﹕%s\n\r"
      "讀取等級  ﹕%d\n\r"
      "寫入等級  ﹕%d\n\r"
      "容量限制  ﹕%d\n\r"
      "目前數量  ﹕%d\n\r"
      "是否被鎖定﹕%s\n\r"
      , pBoard->slot
      , pBoard->name
      , pBoard->welcome
      , pBoard->location ? pBoard->location->vnum : -1
      , pBoard->listfile
      , pBoard->pathname
      , pBoard->read_level
      , pBoard->write_level
      , pBoard->capcity
      , post_count( pBoard, POST_ALL )
      , YESNO( pBoard->lock ) );

    RETURN_NULL();
  }

  send_to_char( "參數錯誤﹐請查詢指令的用法﹗\n\r", ch );
  RETURN_NULL();
}

void open_board_directory( void )
{
  char         directory    [ MAXNAMLEN ];
  char         buf          [ MAXNAMLEN ];
  char         filename     [ MAXNAMLEN ];
  char         work_dir_name[ MAXNAMLEN ];
  char         list_file    [ MAXNAMLEN ];
  char         index        [ MAXNAMLEN ];
  char       * word;
  FILE_DATA  * pFile;
  BOARD_DATA * pBoard;

  PUSH_FUNCTION( "open_board_directory" );

  /* 先找到目錄並且察看是否要填入目錄的記號 */
  fill_path( str_cpy( directory, board_dir ) );

  /* 找到區域檔案目錄列表檔 */
  sprintf( filename , "%s%s" , directory , board_lst );

  /* 看看是否可以開檔 */
  if ( ( pFile = f_open( filename , "r" ) ) )
  {
    /* 一直讀到檔案末端才停止 */
    while ( !if_eof( pFile ) )
    {
      word = fread_word( pFile );

      /* 一行讀完 */
      fread_to_eol( pFile );

      if ( word[0] == '*' ) continue;

      /* 讀入想要讀取的版面目錄 */
      sprintf( buf , "%s%s" , directory , word );

      /* 檢查是否為可以存取的子目錄 */
      if ( is_directory( buf ) )
      {
        /* 再察看是否有索引檔 */
        sprintf( work_dir_name , "%s%s" , directory , word );
        fill_path( work_dir_name );
        sprintf( index , "%s%s" , work_dir_name , board_index );

        /* 如果有索引檔則讀入所有資料 */
        if ( is_regular( index ) )
        {
          if ( !( pBoard = load_board_index( work_dir_name ) ) )
          {
            mudlog( LOG_ERR, "open_board_directory: 無法讀取版面%s."
              , work_dir_name );
          }

          /* 再察看是否有索引檔 */
          sprintf( list_file, "%s%s" , work_dir_name, board_sheet );

          /* 記錄索引檔 */
          pBoard->listfile = str_dup( list_file );
          pBoard->pathname = str_dup( work_dir_name );

          if ( is_regular( list_file ) )
          {
            load_post( work_dir_name, pBoard );
          }

          /* 不然送出錯誤的訊息 */
          else
          {
            mudlog( LOG_ERR , "open_board_directory: 版面目錄 %s 沒有清單檔"
              , work_dir_name );
          }
        }

        /* 不然送出錯誤的訊息 */
        else
        {
          mudlog( LOG_ERR , "open_board_directory: 版面目錄 %s 沒有索引檔"
            , work_dir_name );
        }
      }

      /* 區域目錄根本不能存取 */
      else
      {
        mudlog( LOG_ERR , "open_board_directory: 版面目錄 %s 無法存取" , buf );
      }
    }
    f_close( pFile );
  }

  /* 根本沒有區域目錄列表檔, 送出錯誤訊息 */
  else
  {
    mudlog( LOG_ERR , "沒有版面子目錄列表檔" );
  }

  RETURN_NULL();
}

BOARD_DATA * load_board_index( const char * pathname )
{
  char       * word;
  char         filename[ MAXNAMLEN ];
  bool         fMatch;
  FILE_DATA  * pFile;
  BOARD_DATA * pBoard;

  PUSH_FUNCTION( "load_board" );

  /* 先設定索引檔的檔名 */
  sprintf( filename , "%s%s" , pathname , board_index );

  if ( !( pFile = f_open( filename , "r" ) ) )
  {
    mudlog( LOG_ERR, "load_board_index: 無法開啟索引檔%s.", filename );
    RETURN( NULL );
  }

  set_board_default( ( pBoard = alloc_struct( STRUCT_BOARD_DATA ) ) );

  for ( ;; )
  {
    word   = if_eof( pFile ) ? "End" : fread_word( pFile );
    fMatch = FALSE;

    switch ( UPPER( word[0] ) )
    {
    /* 註解 */
    case '*':
      fMatch = TRUE;
      fread_to_eol( pFile );
      break;

    case 'C':

      if ( !str_cmp( word, "Capcity" ) )
      {
        if ( ( pBoard->capcity = fread_number( pFile ) ) <= 0 )
          mudlog( LOG_DEBUG, "load_board_index: 版面容量不正確 %d."
            , pBoard->capcity );

        fMatch = TRUE;
        break;
      }

      break;

    case 'E':

      if ( !str_cmp( word, "End" ) )
      {
        if ( !pBoard->location )
          mudlog( LOG_DEBUG, "load_board_index: 版面沒有區域房間。" );

        if ( !pBoard->name )
          mudlog( LOG_DEBUG, "load_board_index: 版面沒有相關名稱。" );

        if ( !pBoard->welcome )
          mudlog( LOG_DEBUG, "load_board_index: 版面沒有歡迎訊息。" );

        if ( IS_ERROR( pBoard->slot ) )
          mudlog( LOG_DEBUG, "load_board_index: 版面沒有編號。" );

        if ( IS_ERROR( pBoard->capcity ) )
          mudlog( LOG_DEBUG, "load_board_index: 版面沒有容量限制。" );

        if ( !board_list ) board_list      = pBoard;
        if ( board_last  ) board_last->next = pBoard;

        board_last              = pBoard;
        pBoard->next            = NULL;
        pBoard->location->board = pBoard;

        top_board++;
        f_close( pFile );
        RETURN( pBoard );
      }

      break;

    case 'L':

      if ( !str_cmp( word, "Location" ) )
      {
        int               iRoom;
        ROOM_INDEX_DATA * pRoom;

        iRoom = fread_number( pFile );

        if ( !( pRoom = get_room_index( iRoom ) ) )
          mudlog( LOG_DEBUG, "load_board_index: 沒有 %d 號房間.", iRoom );

        if ( pRoom->board )
          mudlog( LOG_DEBUG, "load_board_index: 房間 %d 版面重複.", iRoom );

        pBoard->location = pRoom;
        fMatch           = TRUE;
        break;
      }

      if ( !str_cmp( word, "Lock" ) )
      {
        pBoard->lock = fread_number( pFile ) == TRUE ? TRUE : FALSE;
        fMatch = TRUE;
        break;
      }
      break;

    case 'N':
      KEY( "Name", pBoard->name, fread_string( pFile ) );
      break;

    case 'R':

      if ( !str_cmp( word, "Read" ) )
      {
        int level;

        level = fread_number( pFile );
        if ( level < 0 || level > MAX_LEVEL )
          mudlog( LOG_DEBUG, "load_board_index: 讀取等級 %d 不合理.", level );

        pBoard->read_level = level;
        fMatch             = TRUE;
        break;
      }
      break;

    case 'S':

      if ( !str_cmp( word, "Slot" ) )
      {
        int slot;

        if ( ( slot = fread_number( pFile ) ) < 0 || slot >= MAX_BOARD )
          mudlog( LOG_DEBUG, "load_board_index: 房間 %d 編號不合理.", slot );

        if ( board_lookup( slot ) )
          mudlog( LOG_DEBUG, "load_board_index: 房間 %d 編號重複.", slot );

        pBoard->slot = slot;
        fMatch = TRUE;
        break;
      }
      break;

    case 'W':

      KEY( "Welcome", pBoard->welcome, fread_string( pFile ) );

      if ( !str_cmp( word, "Write" ) )
      {
        int level;

        level = fread_number( pFile );
        if ( level < 0 || level > MAX_LEVEL )
          mudlog( LOG_DEBUG, "load_board_index: 寫入等級 %d 不合理.", level );

        pBoard->write_level = level;
        fMatch              = TRUE;
        break;
      }
      break;
    }

    /* 指令型態錯誤 */
    if ( !fMatch )
      mudlog( LOG_DEBUG , "load_board_index: 命令 %s 不正確." , word );
  }

  RETURN( NULL );
}

void load_post( const char * pathname, BOARD_DATA * pBoard )
{
  char      * word;
  char        filename[ MAXNAMLEN ];
  char        postfile[ MAXNAMLEN ];
  char        postname[ MAXNAMLEN ];
  bool        fMatch;
  FILE_DATA * pFile;
  POST_DATA * pPost;

  PUSH_FUNCTION( "load_post" );

  /* 先設定索引檔的檔名 */
  sprintf( filename , "%s%s" , pathname , board_sheet );

  if ( !( pFile = f_open( filename , "r" ) ) )
  {
    mudlog( LOG_ERR, "load_post: 無法開啟清單檔 %s.", filename );
    RETURN_NULL();
  }

  for ( ;; )
  {
    word   = if_eof( pFile ) ? "End" : fread_word( pFile );
    fMatch = FALSE;

    switch ( UPPER( word[0] ) )
    {
    case EOF:
      f_close( pFile );
      RETURN_NULL();

    /* 註解 */
    case '*':
      fMatch = TRUE;
      fread_to_eol( pFile );
      break;

    case 'E':

      if ( !str_cmp( word, "End" ) )
      {
        f_close( pFile );
        RETURN_NULL();
      }

      break;

    case 'F':

      if ( !str_cmp( word, "File" ) )
      {
        str_cpy( postname, fread_word( pFile ) );

        sprintf( postfile, "%s%s" , pathname, postname );

        if ( is_regular( postfile ) )
        {
          pPost           = load_a_post( postfile );
          pPost->filename = str_dup( postname );
        }

        else
        {
          mudlog( LOG_ERR , "load_post: 沒有 %s 這篇文章.", postfile );
          RETURN_NULL();
        }

        attach_post( pPost, pBoard, FALSE );
        fMatch = TRUE;
        break;
      }

      break;
    }

    /* 指令型態錯誤 */
    if ( !fMatch )
      mudlog( LOG_DEBUG , "load_post: 命令 %s 不正確." , word );
  }

  RETURN_NULL();
}

POST_DATA * load_a_post( const char * filename )
{
  char      * word;
  bool        fMatch;
  FILE_DATA * pFile;
  POST_DATA * pPost;

  PUSH_FUNCTION( "load_a_post" );

  if ( !( pFile = f_open( filename , "r" ) ) )
  {
    mudlog( LOG_ERR, "load_a_post: 無法開啟索引檔%s.", filename );
    RETURN( NULL );
  }

  set_post_default( ( pPost = alloc_struct( STRUCT_POST_DATA ) ) );

  for ( ;; )
  {
    word   = if_eof( pFile ) ? "End" : fread_word( pFile );
    fMatch = FALSE;

    switch ( UPPER( word[0] ) )
    {
    /* 註解 */
    case '*':
      fMatch = TRUE;
      fread_to_eol( pFile );
      break;

    case 'A':
      KEY( "Attach", pPost->attach, fread_number( pFile ) );
      break;

    case 'E':

      if ( !str_cmp( word, "End" ) )
      {
        if ( !pPost->title )
          mudlog( LOG_DEBUG, "load_a_post: 文章沒有標題." );

        if ( !pPost->text )
          mudlog( LOG_DEBUG, "load_a_post: 文章沒有本文." );

        if ( !pPost->poster )
          mudlog( LOG_DEBUG, "load_a_post: 文章沒有發信者." );

        if ( !pPost->owner )
          mudlog( LOG_DEBUG, "load_a_post: 文章沒有擁有者." );

        if ( IS_ERROR( pPost->timer ) )
          mudlog( LOG_DEBUG, "load_a_post: 文章沒有時間標籤." );

        f_close( pFile );
        RETURN( pPost );
      }

      break;

    case 'M':

      if ( !str_cmp( word, "Mark" ) )
      {
        pPost->mark = fread_number( pFile ) == TRUE ? TRUE : FALSE;
        fMatch      = TRUE;
        break;
      }

      break;

    case 'O':

      KEY( "Owner", pPost->owner, fread_string( pFile ) );
      break;

    case 'P':

      KEY( "Poster", pPost->poster, fread_string( pFile ) );
      break;

    case 'T':

      KEY( "Title", pPost->title, fread_string( pFile ) );
      KEY( "Timer", pPost->timer, fread_number( pFile ) );

      if ( !str_cmp( word, "Text" ) )
      {
        fread_to_eol( pFile );
        pPost->text = fread_string( pFile );
        fMatch = TRUE;
        break;
      }

      break;
    }

    /* 指令型態錯誤 */
    if ( !fMatch )
      mudlog( LOG_DEBUG , "load_a_post: 命令 %s 不正確." , word );
  }

  RETURN( NULL );
}

void set_board_default( BOARD_DATA * pBoard )
{
  PUSH_FUNCTION( "set_board_default" );

  if ( pBoard )
  {
    pBoard->post      = NULL;
    pBoard->location     = NULL;
    pBoard->name         = NULL;
    pBoard->welcome      = NULL;
    pBoard->listfile     = NULL;
    pBoard->pathname     = NULL;
    pBoard->capcity      = ERRORCODE;
    pBoard->slot         = ERRORCODE;
    pBoard->read_level   = 1;
    pBoard->write_level  = level_limit;
    pBoard->lock         = FALSE;
  }

  RETURN_NULL();
}

void set_post_default( POST_DATA * pPost )
{
  PUSH_FUNCTION( "set_post_default" );

  if ( pPost )
  {
    pPost->board    = NULL;
    pPost->title    = NULL;
    pPost->text     = NULL;
    pPost->owner    = NULL;
    pPost->poster   = NULL;
    pPost->filename = NULL;
    pPost->timer    = ERRORCODE;
    pPost->attach   = 0;
    pPost->mark     = FALSE;
  }

  RETURN_NULL();
}

int post_count( BOARD_DATA * pBoard, int type )
{
  int         count = 0;
  POST_DATA * pPost;

  PUSH_FUNCTION( "post_count" );

  if ( !pBoard )
  {
    mudlog( LOG_DEBUG, "post_count: 來源不正確." );
    RETURN( 0 );
  }

  switch( type )
  {
  case POST_ALL:
    for ( pPost = pBoard->post; pPost; pPost = pPost->next )
      count++;
    break;

  case POST_NOT_MARK:

    for ( pPost = pBoard->post; pPost; pPost = pPost->next )
      if ( !pPost->mark ) count++;
    break;

  case POST_MARK:
    for ( pPost = pBoard->post; pPost; pPost = pPost->next )
      if ( pPost->mark ) count++;
    break;
  }

  RETURN( count );
}

int unread_post( CHAR_DATA * ch, BOARD_DATA * pBoard )
{
  int         count;
  int         timer;
  POST_DATA * pPost;

  PUSH_FUNCTION( "unread_post" );

  if ( !ch || !pBoard )
  {
    mudlog( LOG_DEBUG, "unread_post: 來源不正確." );
    RETURN( 0 );
  }

  if ( !ch->pcdata ) RETURN( 0 );

  timer = ch->pcdata->board[pBoard->slot];
  count = 0;

  for ( pPost = pBoard->post; pPost; pPost = pPost->next )
    if ( timer < pPost->timer ) count++;

  RETURN( count );
}

void show_board_title( CHAR_DATA * ch, BOARD_DATA * pBoard )
{
  POST_DATA * pPost;
  char      * timestr;
  int         count;
  time_t      timer;

  PUSH_FUNCTION( "show_board_title" );

  if ( !ch || !pBoard )
  {
    mudlog( LOG_DEBUG, "show_board_title: 缺乏來源." );
    RETURN_NULL();
  }

  if ( IS_NPC( ch ) || !ch->pcdata || !pBoard->post ) RETURN_NULL();

  clear_buffer();
  count = 0;

  send_to_buffer( "%s\n\r\e[1;33;44m 順序 標     題                      "
    "發     信     者               發 信 日 期 \e[0m\n\r",  pBoard->welcome );

  for ( pPost = pBoard->post; pPost; pPost = pPost->next )
  {
    timer   = ( time_t ) UMAX( 1, pPost->timer );
    timestr = ctime( &timer );
    timestr[ UMAX( 0, str_len( timestr ) - 9 ) ] = '\x0';

    send_to_buffer( "%s%4d "
      , post_status( pPost, ch->pcdata->board[pBoard->slot] ), ++count );

    send_to_buffer( "%s ", format_string(
      pPost->title && *pPost->title ? pPost->title : "(不詳)", TITLE_FIELD ) );

    send_to_buffer( "%s ", format_string(
      pPost->poster && *pPost->poster ? pPost->poster : "(不詳)"
      , POSTER_FIELD ) );

    send_to_buffer( "%s\n\r", timestr + 4 );
    if ( buffer_full() ) break;
  }

  print_buffer( ch );
  RETURN_NULL();
}

void show_a_post( POST_DATA * pPost, CHAR_DATA * ch )
{
  char   * timestr;
  time_t   timer;

  PUSH_FUNCTION( "show_a_post" );

  if ( !pPost || !ch )
  {
    mudlog( LOG_DEBUG, "show_a_post: 缺乏來源." );
    RETURN_NULL();
  }

  timer   = ( time_t ) UMAX( 1, pPost->timer );
  timestr = ctime( &timer );
  timestr[ UMAX( 0, str_len( timestr ) - 1 ) ] = '\x0';

  clear_buffer();

  send_to_buffer( "標  題﹕%s\n\r發信者﹕%s\e[0m\n\r時  間﹕%s\n\r"
    , pPost->title, pPost->poster, timestr );

  if ( IS_IMMORTAL( ch ) ) send_to_buffer( "檔  案﹕%s\n\r擁有者﹕%s\n\r"
      , pPost->filename, pPost->owner );

  send_to_buffer( "內  容﹕\n\r%s%s\e[0m%s"
    , VERTICAL_LINE, pPost->text, VERTICAL_LINE );

  print_buffer( ch );

  RETURN_NULL();
}

FUNCTION( do_read )
{
  char         arg[MAX_INPUT_LENGTH];
  char         buf[MAX_STRING_LENGTH];
  int          slot;
  int          count;
  int          timer;
  BOARD_DATA * pBoard;
  POST_DATA  * pPost;

  PUSH_FUNCTION( "do_read" );

  if ( IS_NPC( ch ) || !ch->in_room || !ch->pcdata ) RETURN_NULL();

  argument = one_argument( argument, arg );

  if ( !( pBoard = ch->in_room->board ) )
  {
    send_to_char( "對不起﹐這裡沒有留言板﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( !pBoard->post )
  {
    send_to_char( "對不起﹐這個留言板沒有任何留言﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( ( pBoard->lock || ch->level < pBoard->read_level ) && !IS_IMMORTAL( ch ) )
  {
    send_to_char( "對不起﹐這個留言板你是不能讀的﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( arg[0] )
  {
    if ( !str_prefix( arg, "!all" ) )
    {
      for ( pPost = pBoard->post; pPost; pPost = pPost->next )
      {
        ch->pcdata->board[pBoard->slot]
          = UMAX( pPost->timer, ch->pcdata->board[pBoard->slot] );
      }

      send_to_char( "你將這裡所有的留言設為已經讀取﹗\n\r", ch );
      RETURN_NULL();
    }

    else if ( !str_prefix( arg, "!mark" ) && IS_IMMORTAL( ch ) )
    {
      one_argument( argument, arg );
      if ( !arg[0] || !is_number( arg ) )
      {
        send_to_char( "你要標記哪一篇文章呢﹖\n\r", ch );
        RETURN_NULL();
      }

      slot = atoi( arg );
      chinese_number( slot, buf );

      for ( pPost = pBoard->post; pPost; pPost = pPost->next )
      {
        if ( --slot == 0 )
        {
          pPost->mark = pPost->mark ? FALSE : TRUE;

          act( "你把第$t篇文章$T標記﹗", ch, buf
            , pPost->mark ? "設定" : "解除", TO_CHAR );

          write_post( pPost );
          RETURN_NULL();
        }
      }

      send_to_char( "沒有找到你要標記的文章編號﹗\n\r", ch );
      RETURN_NULL();
    }

    else if ( !str_prefix( arg, "!delete" ) )
    {
      one_argument( argument, arg );
      if ( !arg[0] || !is_number( arg ) )
      {
        send_to_char( "你要刪除哪一篇文章呢﹖\n\r", ch );
        RETURN_NULL();
      }

      slot = atoi( arg );
      chinese_number( slot, buf );

      for ( pPost = pBoard->post; pPost; pPost = pPost->next )
      {
        if ( --slot == 0 )
        {
          if ( pPost->mark )
          {
            send_to_char( "這一篇文章被標記﹐不能刪除。\n\r", ch );
            RETURN_NULL();
          }

          if ( !IS_IMMORTAL( ch ) && str_cmp( pPost->owner, ch->name ) )
          {
            send_to_char( "你沒有權力刪除這篇文章﹗\n\r", ch );
            RETURN_NULL();
          }

          pBoard = pPost->board;
          remove_post( pPost );

          /* 重新更新列表 */
          rewrite_board_list( pBoard );

          act( "你刪除第$t篇文章﹗", ch, buf, NULL, TO_CHAR );
          RETURN_NULL();
        }
      }

      send_to_char( "沒有找到你要刪除的文章編號﹗\n\r", ch );
      RETURN_NULL();
    }

    else if ( !is_number( arg ) )
    {
      send_to_char( "你必須輸入數字來表示編號﹗\n\r", ch );
      RETURN_NULL();
    }

    slot = atoi( arg );
  }

  else
  {
    timer = ch->pcdata->board[pBoard->slot];
    slot = 1;

    for ( pPost = pBoard->post; pPost; pPost = pPost->next, slot++ )
      if ( timer < pPost->timer ) break;

    if ( !pPost )
    {
      send_to_char( "你已經讀完這裡的所有留言﹗\n\r", ch );
      RETURN_NULL();
    }
  }

  for ( count = 0, pPost = pBoard->post; pPost; pPost = pPost->next )
  {
    if ( ++count == slot )
    {
      show_a_post( pPost, ch );

      ch->pcdata->board[pBoard->slot] =
        UMAX( pPost->timer, ch->pcdata->board[pBoard->slot] );

      RETURN_NULL();
    }
  }

  send_to_char( "對不起﹐沒有那篇留言。\n\r", ch );
  RETURN_NULL();
}

FUNCTION( do_post )
{
  char         arg[MAX_INPUT_LENGTH];
  char         buf[MAX_STRING_LENGTH];
  char         buf1[MAX_STRING_LENGTH];
  char         buf2[MAX_STRING_LENGTH];
  int          slot;
  char       * aString;
  char       * pString;
  BOARD_DATA * pBoard;
  POST_DATA  * pPost;
  POST_DATA  * aPost;

  PUSH_FUNCTION( "do_post" );

  if ( IS_NPC( ch ) || !ch->in_room || !ch->pcdata ) RETURN_NULL();

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    send_to_char( "你必須註明標題或是回第幾篇文章﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( !( pBoard = ch->in_room->board ) )
  {
    send_to_char( "對不起﹐這裡沒有留言板﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( ( pBoard->lock || ch->level < pBoard->write_level )
    && !IS_IMMORTAL( ch ) )
  {
    send_to_char( "對不起﹐這個留言板你是不能發表文章的﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg, "edit" ) )
  {
    if ( !ch->desc ) RETURN_NULL();

    ch->desc->edit_mode = EDIT_POST;
    show_editor( ch );

    free_string( ch->stack );
    ch->stack = str_dup( "" );

    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "show" ) )
  {
    if ( !ch->editing || !*ch->editing )
    {
      send_to_char( "你的編輯緩衝區裡沒有任何東西﹗\n\r", ch );
      RETURN_NULL();
    }

    print_to_char( ch, "你的文章的內容為﹕\n\r%s%s"
      , VERTICAL_LINE, ch->editing );

    RETURN_NULL();
  }

  else if ( arg[0] == '!' )
  {
    if ( !ch->editing || !*ch->editing )
    {
      send_to_char( "你的編輯緩衝區裡沒有任何東西﹐請查詢 edit﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( arg[1] == '\x0' )
    {
      send_to_char( "你必須註明回第幾篇文章﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !is_number( arg + 1 ) )
    {
      send_to_char( "對不起﹐文章編號一定是數字﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( str_len( ch->editing ) > sizeof( buf1 ) - ( MAX_RESPONSE * 2 ) )
    {
      send_to_char( "你的編輯緩衝區太大了﹐請刪除一些吧﹗\n\r", ch );
      RETURN_NULL();
    }

    slot = atoi( arg + 1 );
    chinese_number( slot, buf2 );

    for ( aPost = pBoard->post; aPost; aPost = aPost->next )
    {
      if ( --slot == 0 )
      {
        /* 配置記憶體並且清除 */
        set_post_default( ( pPost = alloc_struct( STRUCT_POST_DATA ) ) );

        /* 設定作者 */
        str_cpy( buf,  mob_name( NULL, ch ) );
        buf[POSTER_FIELD] = '\x0';
        smash_tilde( buf );
        pPost->poster = str_dup( buf );
        pPost->owner  = str_dup( ch->name );

        /* 設定標題 */
        if ( aPost->attach > 0 ) str_cpy( buf, aPost->title           );
        else                     sprintf( buf, "Re: %s", aPost->title );

        buf[TITLE_FIELD] = '\x0';
        smash_tilde( buf );
        pPost->title = str_dup( buf );

        /* 設定內容 */
        sprintf( buf, "\e[36m:【 在 %s 的大作中提到 】\n  :", aPost->poster );
        aString = buf + str_len( buf ) - 1;

        for ( pString = aPost->text; *pString; pString++ )
        {
          if ( *pString == '\r' ) continue;
          if ( *pString == '\n' )
          {
            *aString = '\x0';
            str_cat( buf, "\n\e[36m  :" );
            aString = buf + str_len( buf );
          }
          else
          {
            *aString++ = *pString;
          }
        }

        *aString = '\x0';
        str_cat( buf, "\e[0m\n\n" );

        if ( str_len( buf ) >= MAX_RESPONSE )
        {
          buf[MAX_RESPONSE] = '\x0';
          str_cat( buf, "\e[0m\n\n" );
        }

        str_cpy( buf1, ch->editing );
        buf1[sizeof(buf1) - str_len( buf ) - 200 ] = '\x0';
        str_cat( buf, buf1 );

        smash_tilde( buf );
        pPost->text = str_dup( buf );

        /* 設定檔名 */
        sprintf( buf, "%d", ( pPost->timer = current_time ) );
        pPost->filename = str_dup( buf );

        /* 設定雜項 */
        pPost->mark   = FALSE;
        pPost->attach = aPost->timer;
        pPost->timer  = current_time;
        pPost->board  = pBoard;

        /* 寫回檔案 */
        attach_post( pPost, pBoard, TRUE );
        write_post( pPost );

        act( "$n在$t回了第$T篇的文章。", ch, pBoard->name, buf2, TO_ALL );

        /* 釋放編輯緩衝區 */
        free_string( ch->editing );
        ch->editing = str_dup( "" );

        /* 編輯的 Stamp 修正 */
        ch->pcdata->board[pBoard->slot] = current_time;

        RETURN_NULL();
      }
    }

    send_to_char( "找不到那篇文章可以回應﹗\n\r", ch );
    RETURN_NULL();
  }

  else if ( arg[0] == '*' )
  {
    if ( !ch->editing || !*ch->editing )
    {
      send_to_char( "你的編輯緩衝區裡沒有任何東西﹐請查詢 edit﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( arg[1] == '\x0' )
    {
      send_to_char( "你必須註明回第幾篇文章﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !is_number( arg + 1 ) )
    {
      send_to_char( "對不起﹐文章編號一定是數字﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( str_len( ch->editing ) > sizeof( buf1 ) - 200 )
    {
      send_to_char( "你的編輯緩衝區太大了﹐請刪除一些吧﹗\n\r", ch );
      RETURN_NULL();
    }

    slot = atoi( arg + 1 );
    chinese_number( slot, buf2 );

    for ( aPost = pBoard->post; aPost; aPost = aPost->next )
    {
      if ( --slot == 0 )
      {
        /* 配置記憶體並且清除 */
        set_post_default( ( pPost = alloc_struct( STRUCT_POST_DATA ) ) );

        /* 設定作者 */
        str_cpy( buf, mob_name( NULL, ch ) );
        buf[POSTER_FIELD] = '\x0';
        smash_tilde( buf );
        pPost->poster = str_dup( buf );
        pPost->owner  = str_dup( ch->name );

        /* 設定標題 */
        if ( aPost->attach > 0 ) str_cpy( buf, aPost->title           );
        else                     sprintf( buf, "Re: %s", aPost->title );

        buf[TITLE_FIELD] = '\x0';
        smash_tilde( buf );
        pPost->title = str_dup( buf );

        /* 設定內容 */
        str_cpy( buf, ch->editing );
        smash_tilde( buf );
        pPost->text = str_dup( buf );

        /* 設定檔名 */
        sprintf( buf, "%d", ( pPost->timer = current_time ) );
        pPost->filename = str_dup( buf );

        /* 設定雜項 */
        pPost->mark   = FALSE;
        pPost->attach = aPost->timer;
        pPost->timer  = current_time;
        pPost->board  = pBoard;

        /* 寫回檔案 */
        attach_post( pPost, pBoard, TRUE );
        write_post( pPost );

        act( "$n在$t回了第$T篇的文章。", ch, pBoard->name, buf2, TO_ALL );

        /* 釋放編輯緩衝區 */
        free_string( ch->editing );
        ch->editing = str_dup( "" );

        /* 編輯的 Stamp 修正 */
        ch->pcdata->board[pBoard->slot] = current_time;

        RETURN_NULL();
      }
    }

    send_to_char( "找不到那篇文章可以回應﹗\n\r", ch );
    RETURN_NULL();
  }

  else
  {
    if ( !ch->editing || !*ch->editing )
    {
      send_to_char( "你的編輯緩衝區裡沒有任何東西﹐請查詢 edit﹗\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查內容是否有不雅字 */
    if ( check_chat_xname( ch, arg ) == TRUE ) RETURN_NULL();

    if ( str_len( ch->editing ) > sizeof( buf ) - 200 )
    {
      send_to_char( " 你的編輯緩衝區太大了﹐請先刪除一些吧﹗\n\r", ch );
      RETURN_NULL();
    }

    /* 配置記憶體並且清除 */
    set_post_default( ( pPost = alloc_struct( STRUCT_POST_DATA ) ) );

    /* 設定作者 */
    str_cpy( buf, mob_name( NULL, ch ) );
    buf[POSTER_FIELD] = '\x0';
    smash_tilde( buf );
    pPost->poster = str_dup( buf );
    pPost->owner  = str_dup( ch->name );

    /* 設定標題 */
    str_cpy( buf, arg );
    buf[TITLE_FIELD] = '\x0';
    smash_tilde( buf );
    pPost->title = str_dup( buf );

    /* 設定內容 */
    str_cpy( buf, ch->editing );
    smash_tilde( buf );
    pPost->text = str_dup( buf );

    /* 設定檔名 */
    sprintf( buf, "%d", ( pPost->timer = current_time ) );
    pPost->filename = str_dup( buf );

    /* 設定雜項 */
    pPost->mark   = FALSE;
    pPost->attach = 0;
    pPost->timer  = current_time;
    pPost->board  = pBoard;

    /* 寫回檔案 */
    attach_post( pPost, pBoard, TRUE );
    write_post( pPost );

    act( "$n在$t發表了一篇有關$T的文章。", ch, pBoard->name, arg, TO_ALL );

    /* 釋放編輯緩衝區 */
    free_string( ch->editing );
    ch->editing = str_dup( "" );

    /* 編輯的 Stamp 修正 */
    ch->pcdata->board[pBoard->slot] = current_time;

    RETURN_NULL();
  }

  RETURN_NULL();
}

/* 清除過量的文章 */
void board_update( void )
{
  BOARD_DATA * pBoard;

  PUSH_FUNCTION( "board_update" );

  for ( pBoard = board_list; pBoard; pBoard = pBoard->next )
    purge_post( pBoard );

  RETURN_NULL();
}

void purge_post( BOARD_DATA * pBoard )
{
  POST_DATA * pPost;
  POST_DATA * zPost;
  int         count;

  PUSH_FUNCTION( "purge_post" );

  if ( !pBoard )
  {
    mudlog( LOG_DEBUG, "purge_post: 來源不正確." );
    RETURN_NULL();
  }

  /* 如果還不到容量限制 */
  if ( ( count = post_count( pBoard, POST_NOT_MARK ) - pBoard->capcity ) <= 0 )
    RETURN_NULL();

  for ( pPost = pBoard->post; pPost && count > 0; pPost = zPost )
  {
    zPost = pPost->next;
    if ( !pPost->mark )
    {
      count--;
      remove_post( pPost );
    }
  }

  /* 重新更新列表 */
  rewrite_board_list( pBoard );
  RETURN_NULL();
}

void remove_post( POST_DATA * pPost )
{
  BOARD_DATA * pBoard;
  POST_DATA  * aPost;
  POST_DATA  * zPost;
  char         filename[MAX_FILE_LENGTH];

  PUSH_FUNCTION( "remove_post" );

  if ( !pPost || !( pBoard = pPost->board ) )
  {
    mudlog( LOG_DEBUG, "remove_post: 來源錯誤." );
    RETURN_NULL();
  }

  /* 刪除檔案 */
  sprintf( filename, "%s%s", pBoard->pathname, pPost->filename );

  if ( unlink( filename ) != 0 )
    mudlog( LOG_DEBUG, "remove_post: 無法刪除 %s.", filename );

  for ( zPost = NULL, aPost = pBoard->post;
        aPost;
        zPost = aPost, aPost = aPost->next )
  {
    if ( aPost == pPost )
    {
      if ( !zPost ) pBoard->post  = aPost->next;
      else          zPost->next   = aPost->next;

      free_string( pPost->title    );
      free_string( pPost->text     );
      free_string( pPost->poster   );
      free_string( pPost->owner    );
      free_string( pPost->filename );

      free_struct( pPost , STRUCT_POST_DATA );
      RETURN_NULL();
    }
  }

  mudlog( LOG_DEBUG, "remove_post: 未發現到欲刪除的文章." );
  RETURN_NULL();
}

/* 重新更新列表 */
void rewrite_board_list( BOARD_DATA * pBoard )
{
  POST_DATA * pPost;
  FILE      * pFile;

  PUSH_FUNCTION( "rewrite_board_list" );

  if ( !pBoard || !pBoard->listfile || !*pBoard->listfile )
  {
    mudlog( LOG_DEBUG, "rewrite_board_list: 來源錯誤." );
    RETURN_NULL();
  }

  if ( ( pFile = FOPEN( pBoard->listfile, "w" ) ) )
  {
    for ( pPost = pBoard->post; pPost; pPost = pPost->next )
      fprintf( pFile, "File            %s\n", pPost->filename );

    FCLOSE( pFile );
  }

  else
  {
    mudlog( LOG_DEBUG, "rewrite_board_list: 無法開啟檔案 %s."
      , pBoard->listfile );
  }

  RETURN_NULL();
}

void attach_post( POST_DATA * pPost, BOARD_DATA * pBoard, bool fWrite )
{
  POST_DATA  * aPost;

  PUSH_FUNCTION( "attach_post" );

  if ( !pPost || !pBoard )
  {
    mudlog( LOG_DEBUG, "attach_post: 來源錯誤." );
    RETURN_NULL();
  }

  pPost->next  = NULL;
  pPost->board = pBoard;

  if ( !pBoard->post )
  {
    pBoard->post = pPost;
  }

  else
  {
    for ( aPost = pBoard->post; aPost->next; aPost = aPost->next );
    aPost->next = pPost;
  }

  /* 如果要更新檔案列表 */
  if ( fWrite ) rewrite_board_list( pBoard );

  RETURN_NULL();
}

void write_post( POST_DATA * pPost )
{
  BOARD_DATA * pBoard;
  char         filename[MAX_FILE_LENGTH];
  FILE       * pFile;

  PUSH_FUNCTION( "write_post" );

  if ( !pPost || !( pBoard = pPost->board ) || !pBoard->pathname )
  {
    mudlog( LOG_DEBUG, "write_post: 來源不正確." );
    RETURN_NULL();
  }

  sprintf( filename, "%s%s", pBoard->pathname, pPost->filename );

  if ( !( pFile = FOPEN( filename, "w" ) ) )
  {
    mudlog( LOG_DEBUG, "write_post: 無法開啟檔案 %s.", filename );
    RETURN_NULL();
  }

  fprintf( pFile, "%s%s~\n"
    , str_space( "Poster" , BLANK_LENGTH ), pPost->poster );

  fprintf( pFile, "%s%s~\n"
    , str_space( "Owner" , BLANK_LENGTH ), pPost->owner );

  fprintf( pFile, "%s%s~\n"
    , str_space( "Title"  , BLANK_LENGTH ), pPost->title  );

  fprintf( pFile, "%s%d\n"
    , str_space( "Mark"   , BLANK_LENGTH ), pPost->mark   );

  fprintf( pFile, "%s%d\n"
    , str_space( "Attach" , BLANK_LENGTH ), pPost->attach );

  fprintf( pFile, "%s%d\n"
    , str_space( "Timer"  , BLANK_LENGTH ), pPost->timer  );

  fprintf( pFile, "Text\n%s~\n", pPost->text );
  fprintf( pFile, "End" );
  FCLOSE( pFile );

  RETURN_NULL();
}

/* 顯示文章對某人的狀態 */
const char * post_status( POST_DATA * pPost, int timer )
{
  PUSH_FUNCTION( "post_status" );

  if ( pPost->timer > timer && pPost->mark ) RETURN( "\e[1;35mM\e[0m" );
  if ( pPost->mark ) RETURN( "\e[1;33mm\e[0m" );

  if ( pPost->timer > timer ) RETURN( "\e[1;32mN\e[0m" );

  RETURN( " " );
}

FUNCTION( do_edit )
{
  char   arg[MAX_INPUT_LENGTH];
  char   buf[MAX_STRING_LENGTH];
  char   buf1[MAX_STRING_LENGTH];
  char * pString;

  PUSH_FUNCTION( "do_edit" );

  if ( IS_NPC( ch ) || !ch->editing ) RETURN_NULL();

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    if ( !*ch->editing )
    {
      send_to_char( "你並沒有編輯任何東西﹗\n\r", ch );
      RETURN_NULL();
    }

    print_to_char( ch, "你目前編輯的內容為﹕\n\r%s%s%s"
      , VERTICAL_LINE, ch->editing, VERTICAL_LINE );

    RETURN_NULL();
  }

  else if ( !str_cmp( arg, "!clear" ) )
  {
    free_string( ch->editing );
    ch->editing = str_dup( "" );
    send_to_char( "你已經清除你的編輯緩衝區的內容。\n\r", ch );
    RETURN_NULL();
  }

  else if ( !str_cmp( arg, "+" ) )
  {
    str_cpy( buf, ch->editing );

    if ( str_len( buf ) + str_len( argument ) >= ( sizeof( buf ) - 200 ) )
    {
      send_to_char( "對不起﹐編輯內容太長了。\n\r", ch );
      RETURN_NULL();
    }

    /* 消掉關鍵字 */
    smash_tilde( argument );

    /* 檢查內容是否有不雅字 */
    if ( check_chat_xname( ch, argument ) == TRUE ) RETURN_NULL();

    ansi_transcribe( argument, buf1 );

    /* 以避免空白被刪除 */
    if ( buf1[0] == '.' ) buf1[0] = ' ';

    str_cat( buf, buf1 );
    str_cat( buf, "\n" );
    free_string( ch->editing );
    ch->editing = str_dup( buf );

    send_to_char( "增加編輯的內容。\n\r", ch );
    RETURN_NULL();
  }

  else if ( !str_cmp( arg, "-" ) )
  {
    if ( !*ch->editing )
    {
      send_to_char( "你的編輯緩衝區沒有內容﹐無法刪除﹗\n\r", ch );
      RETURN_NULL();
    }

    str_cpy( buf, ch->editing );

    for ( pString = buf + str_len( buf ) - 2; pString > buf; pString-- )
    {
      if ( *pString == '\n' )
      {
        *( pString + 1 ) = '\x0';
        free_string( ch->editing );
        ch->editing = str_dup( buf );

        send_to_char( "刪除編輯緩衝區上一行﹗\n\r", ch );
        RETURN_NULL();
      }
    }

    free_string( ch->editing );
    ch->editing = str_dup( "" );
    send_to_char( "因為你的緩衝區只有一行﹐所以整個被刪除了﹗\n\r", ch );
    RETURN_NULL();
  }

  else
  {
    send_to_char( "語法錯誤﹐請查詢相關用法﹗\n\r", ch );
    RETURN_NULL();
  }
  RETURN_NULL();
}

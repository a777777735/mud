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
#include <stdarg.h>
#include <unistd.h>
#include <time.h>
#include "merc.h"

static bool     BufferFull = FALSE;
static bool     StackFull  = FALSE;
static char     Buffer[ MAX_OUTPUT_LENGTH ];
static char     Stack[ MAX_OUTPUT_LENGTH ];
char * const    Buffer_full_messages =
  "\n\r\n\r\e[1;31m**** 你的輸出緩衝區太大﹐系統自動調整。****\n\r\e[0m";

static char     BugBuffer[BUFSIZ] = { '\x0' };
char            FileOop[BUFSIZ];
static int      BugTimes = 0;
extern bool     fReload;
int             FileLine;
int             FileWord;

void            file_seek       args( ( FILE_DATA * ) );

/* 清除緩衝區 */
void clear_buffer( void )
{
  PUSH_FUNCTION( "clear_buffer" );
  Buffer[0] = '\x0';
  BufferFull = FALSE;
  RETURN_NULL();
}

/* 清除堆疊區 */
void clear_stack( void )
{
  PUSH_FUNCTION( "clear_stack" );
  Stack[0] = '\x0';
  StackFull = FALSE;
  RETURN_NULL();
}

bool buffer_full( void )
{
  return( BufferFull );
}

bool stack_full( void )
{
  return( StackFull );
}

/* 增加字串到緩衝區 */
void send_to_buffer( const char * string , ... )
{
  va_list args;
  char    buf[ MAX_OUTPUT_LENGTH ];
  int     location;

  PUSH_FUNCTION( "send_to_buffer" );

  va_start( args , string );
  vsprintf( buf  , string , args );
  va_end( args );

  if ( BufferFull ) RETURN_NULL();

  /* 是否超出界線, 是者則切斷他 */
  if ( str_len( Buffer ) + str_len( buf ) >= sizeof( buf ) - 1 )
  {
    location = sizeof( buf ) - str_len( Buffer ) - str_len( buf ) - 1;
    buf[ UMAX( 0, location ) ] = '\x0';
    BufferFull = TRUE;
  }

  /* 拷貝到目的區裡 */
  str_cat( Buffer, buf );

  /* 若緩衝區已滿, 則送出警告字串 */
  if ( BufferFull )
  {
    Buffer[ sizeof( buf ) - str_len( Buffer_full_messages ) - 1] = '\x0';
    str_cat( Buffer, Buffer_full_messages );
    mudlog( LOG_INFO , "「BUFFER」 輸出緩衝區已滿." );
  }

  RETURN_NULL();
}

/* 增加字串到緩衝區 */
void send_to_stack( const char * string , ... )
{
  va_list args;
  char    buf[ MAX_OUTPUT_LENGTH ];
  int     location;

  PUSH_FUNCTION( "send_to_stack" );

  va_start( args , string );
  vsprintf( buf, string , args );
  va_end( args );

  if ( StackFull ) RETURN_NULL();

  /* 是否超出界線, 是者則切斷他 */
  if ( str_len( Stack ) + str_len( buf ) >= sizeof( buf ) - 1 )
  {
    location = sizeof( buf ) - str_len( Stack ) - str_len( buf ) - 1;
    buf[ UMAX( 0, location ) ] = '\x0';
    StackFull = TRUE;
  }

  /* 拷貝到目的區裡 */
  str_cat( Stack, buf );

  /* 若緩衝區已滿, 則送出警告字串 */
  if ( StackFull )
  {
    Stack[ sizeof( buf ) - str_len( Buffer_full_messages ) - 1] = '\x0';
    str_cat( Stack, Buffer_full_messages );
    mudlog( LOG_INFO , "「STACK」 輸出堆疊區已滿." );
  }

  RETURN_NULL();
}

/* 把緩衝區的資料全部送出去給玩家 */
void print_buffer( CHAR_DATA * ch )
{
  PUSH_FUNCTION( "print_buffer" );
  send_to_char( Buffer , ch );
  clear_buffer();
  RETURN_NULL();
}

/* 把堆疊區的資料全部送出去給玩家 */
void print_stack( CHAR_DATA * ch )
{
  PUSH_FUNCTION( "print_stack" );
  send_to_char( Stack , ch );
  clear_stack();
  RETURN_NULL();
}

/* 把堆疊區的資料回傳給函數 */
char * return_stack( void )
{
  PUSH_FUNCTION( "return_stack" );
  RETURN( Stack );
}

/* 輸出給玩家, 可以有變數形式 */
void print_to_char( CHAR_DATA * ch , const char * format , ... )
{
  va_list args;
  char    buf[ MAX_OUTPUT_LENGTH ];

  PUSH_FUNCTION( "print_to_char" );

  va_start( args, format );
  vsprintf( buf , format , args );
  va_end( args );

  send_to_char( buf, ch );
  RETURN_NULL();
}

void mudlog( int type , const char * buffer , ... )
{
  extern char      * FunctionName[MAX_DEBUG];
  extern int         FunctionPointer;
  extern bool        fBootDb;
  extern CMD_DATA  * LastCommand;
  extern CHAR_DATA * LastChar;
  extern char        LastArgument[];
  va_list            args;
  char               buf[ MAX_OUTPUT_LENGTH ];
  FILE             * pFile;
  char             * pString;
  int                loop;
  int                count;
  static int         depth     = 0;

  PUSH_FUNCTION( "mudlog" );

  /* 如果不是母行程 */
  if ( getpid() != merc_pid ) RETURN_NULL();

  va_start( args , buffer );
  vsprintf( buf  , buffer , args );
  va_end( args );
  depth++;

  switch( type )
  {
  default :
    break;

  /* 系統嚴重錯誤, 但是系統還是可以重置 */
  case LOG_EMERG:
  case LOG_STACK:

    if ( ( pFile = FOPEN( error_file , "a" ) ) )
    {
      fprintf( pFile , "%s\n"
        , time_format( time( NULL ), "%r, %a-%d-%b-%y" ) );

      switch( type )
      {
      default :
        break;

      case LOG_EMERG:
        fprintf( pFile , "「系統臨界錯誤」 %s\n" , buffer );
        pop_function();
        break;

      case LOG_STACK:
        fprintf( pFile , "「函數堆疊錯誤」 %s\n" , buffer );
        break;
      }

      /* 是否開啟除錯模式 */
      if ( internal_debug )
      {
        for ( count = 0, loop = FunctionPointer + 1; loop < MAX_DEBUG; loop++ )
        {
          if ( !FunctionName[loop] ) break;
          fprintf( pFile , "(%2d) 函數﹕%-20s\n", ++count, FunctionName[loop] );
        }

        fprintf( pFile, "\n" );
      }

      fflush( pFile );
      FCLOSE( pFile );
    }

    if ( LastCommand    ) mudlog( LOG_INFO, "最後命令 %s"  ,LastCommand->name);
    if ( LastChar       ) mudlog( LOG_INFO, "最後使用者 %s", LastChar->name  );
    if ( LastArgument[0]) mudlog( LOG_INFO, "最後參數 %s"  , LastArgument    );

    /* 嘗試關閉所有的 socket */
    if ( merc_exec == TRUE )
    {
      mudlog( LOG_INFO , ""               );
      mudlog( LOG_INFO , "系統不正常終止" );
      close_all_socket();
    }

    merc_exit( -1 );

  /* 緊急狀況, 但是還是可以重置系統 */
  case LOG_CRIT:

    mudlog( LOG_INFO , "%s", buf );

    if ( ( pFile = FOPEN( bugs_file , "a" ) ) )
    {
      fprintf( pFile ,"(%s) %s\n"
        , time_format( time( NULL ) , "%r, %a-%d-%b-%y" ), buf );

      fflush( pFile );
      FCLOSE( pFile );
    }

    if ( LastCommand    ) mudlog( LOG_INFO, "最後命令 %s" ,LastCommand->name);
    if ( LastChar       ) mudlog( LOG_INFO, "最後使用者 %s", LastChar->name );
    if ( LastArgument[0]) mudlog( LOG_INFO, "最後參數 %s"  , LastArgument   );

    /* 嘗試關閉所有的 socket */
    if ( merc_exec == TRUE )
    {
      mudlog( LOG_INFO , ""               );
      mudlog( LOG_INFO , "系統不正常終止" );
      close_all_socket();
    }

    merc_exit( -1 );

  /* 緊急狀況, 系統重置建議不要 */
  case LOG_ERR:

    mudlog( LOG_SHUTDOWN , "系統關閉" );
    mudlog( LOG_CRIT     , "%s", buf  );
    break;

  /* 鎖定神族指令 */
  case LOG_WIZARD:

    if ( merc_exec && depth == 1 ) notify( L_GOD, NOTIFY_WIZARD, buf );

    if ( ( pFile = FOPEN( wizard_file , "a" ) ) )
    {
      fprintf( pFile ,"(%s) %s\n"
        , time_format( time( NULL ) , "%r, %a-%d-%b-%y" ), buf );
      FCLOSE( pFile );
    }

    mudlog( LOG_INFO , "%s", buf );
    break;

  case LOG_NEWPLAYER:

    if ( merc_exec && depth == 1 ) notify( LEV_IMM, NOTIFY_NEWPLAYER, buf );

    if ( ( pFile = FOPEN( newplayer_file , "a" ) ) )
    {
      fprintf( pFile ,"(%s) %s\n"
        , time_format( time( NULL ) , "%r, %a-%d-%b-%y" ), buf );
      FCLOSE( pFile );
    }

    mudlog( LOG_INFO , "%s", buf );
    break;

  case LOG_DEBUG:

    if ( fReload )
    {
      FileError |= ERROR_OTHER;

      if ( str_len( FileOop ) + str_len( buf ) <= sizeof( FileOop ) - 3 )
      {
        str_cat( FileOop, buf    );
        str_cat( FileOop, "\n\r" );
      }

      file_seek( Now_File );
    }

    else
    {
      if ( merc_exec && depth == 1 ) notify( L_SUP, NOTIFY_DEBUG, buf );

      if ( BugBuffer[0] && !str_cmp( BugBuffer, buf ) )
      {
        BugTimes++;
        mudlog( LOG_INFO , "[***] BUG: %s" , buf );
        break;
      }

      if ( BugTimes > 0 )
      {
        if ( ( pFile = FOPEN( bugs_file , "a" ) ) )
        {
          fprintf( pFile ,"(%s) [***] BUG: 上述訊息持續 %d 次。\n"
            , time_format( time( NULL ) , "%r, %a-%d-%b-%y" ), BugTimes );

          fflush( pFile );
          FCLOSE( pFile );
        }
      }

      str_cpy( BugBuffer, buf );
      BugTimes = 0;
    }

    if ( ( pFile = FOPEN( bugs_file , "a" ) ) )
    {
      fprintf( pFile ,"(%s) [***] BUG: %s\n"
        , time_format( time( NULL ) , "%r, %a-%d-%b-%y" ), buf );

      fflush( pFile );
      FCLOSE( pFile );
    }

    mudlog( LOG_INFO , "[***] BUG: %s" , buf );

    if ( fBootDb )
    {
      if ( Now_File && Now_File->fp != stdin )
      {
        char errormsg[ MAX_STRING_LENGTH ];

        FileLine   = 1;
        FileWord   = 1;
        pString = Now_File->address;

        for ( ; pString < Now_File->pointer; pString++ )
        {
          if ( *pString == '\n' )
          {
            FileWord = 1;
            FileLine++;
            continue;
          }

          FileWord++;
        }

        sprintf( errormsg , "檔案 %s 第 %d 行 第 %d 個字之前有錯誤."
          , Now_File->filename, FileLine, FileWord );

        mudlog( LOG_INFO , "%s", errormsg );

        if ( ( pFile = FOPEN( bugs_file , "a" ) ) )
        {
          fprintf( pFile ,"(%s) [***] BUG: %s\n"
            , time_format( time( NULL ) , "%r, %a-%d-%b-%y" ), errormsg );

          fflush( pFile );
          FCLOSE( pFile );
        }
      }

      mudlog( LOG_SHUTDOWN , "系統關閉" );
      merc_exit( -1 );
    }

    break;

  case LOG_FAILPASS:

    if ( merc_exec && depth == 1 ) notify( L_SUP, NOTIFY_FAILPASS, buf );

    if ( ( pFile = FOPEN( failpass_file , "a" ) ) )
    {
      fprintf( pFile ,"(%s) %s\n"
        , time_format( time( NULL ) , "%r, %a-%d-%b-%y" ), buf );
      FCLOSE( pFile );

      /* 改變檔案存取模式 */
      set_file_mode( failpass_file );
    }

    mudlog( LOG_INFO, "%s", buf );
    break;

  case LOG_FAILENABLE:

    if ( ( pFile = FOPEN( failenable_file , "a" ) ) )
    {
      fprintf( pFile ,"(%s) %s\n"
        , time_format( time( NULL ) , "%r, %a-%d-%b-%y" ), buf );
      FCLOSE( pFile );

      /* 改變檔案存取模式 */
      set_file_mode( failpass_file );
    }

    break;

  case LOG_BADOBJECT:

    if ( ( pFile = FOPEN( badobject_file , "a" ) ) )
    {
      fprintf( pFile ,"%s\n", buf );
      FCLOSE( pFile );

      /* 改變檔案存取模式 */
      set_file_mode( badobject_file );
    }

    break;

  case LOG_BADFILE:

    if ( ( pFile = FOPEN( bad_file , "a" ) ) )
    {
      fprintf( pFile ,"(%s) %s\n"
        , time_format( time( NULL ) , "%r, %a-%d-%b-%y" ), buf );
      FCLOSE( pFile );

      /* 改變檔案存取模式 */
      set_file_mode( bad_file );
    }

    break;

  case LOG_SUSPECT:

    if ( ( pFile = FOPEN( suspect_file , "a" ) ) )
    {
      fprintf( pFile ,"(%s) %s\n"
        , time_format( time( NULL ) , "%r, %a-%d-%b-%y" ), buf );
      FCLOSE( pFile );

      /* 改變檔案存取模式 */
      set_file_mode( suspect_file );
    }

    break;

  /* 神族代理檔案 */
  case LOG_WIZFLAGS:

    if ( ( pFile = FOPEN( wizflags_log , "a" ) ) )
    {
      fprintf( pFile ,"(%s) %s\n"
        , time_format( time( NULL ) , "%r, %a-%d-%b-%y" ), buf );
      FCLOSE( pFile );

      /* 改變檔案存取模式 */
      set_file_mode( wizflags_log );
    }

    break;

  /* 一般資料 */
  case LOG_INFO:

    if ( merc_exec && depth == 1 ) notify( L_DEI, NOTIFY_INFO, buf );

    fprintf( stderr ,"(%s) %s\n"
      , time_format( time( NULL ) , "%r, %a-%d-%b-%y" ), buf );
    break;

  case LOG_FAILEXIT:

    if ( ( pFile = FOPEN( failexit_file , "a" ) ) )
    {
      fprintf( pFile ,"(%s) %s\n"
        , time_format( time( NULL ) , "%r, %a-%d-%b-%y" ), buf );
      FCLOSE( pFile );

      /* 改變檔案存取模式 */
      set_file_mode( failexit_file );
    }

    mudlog( LOG_INFO, "%s", buf );
    break;

  case LOG_FAILLOAD:

    if ( ( pFile = FOPEN( failload_file , "a" ) ) )
    {
      fprintf( pFile ,"(%s) %s\n"
        , time_format( time( NULL ) , "%r, %a-%d-%b-%y" ), buf );

      FCLOSE( pFile );

      /* 改變檔案存取模式 */
      set_file_mode( failload_file );
    }

    mudlog( LOG_INFO, "%s", buf );
    break;

  case LOG_SHUTDOWN:

    if ( ( pFile = FOPEN( SHUTDOWN_FILE , "a" ) ) )
    {
      fprintf( pFile ,"(%s) %s\n"
        , time_format( time( NULL ) , "%r, %a-%d-%b-%y" ), buf );

      fflush( pFile );
      FCLOSE( pFile );
    }
    break;

  case LOG_IDEA:

    if ( ( pFile = FOPEN( ideas_file , "a" ) ) )
    {
      fprintf( pFile ,"(%s) %s\n"
        , time_format( time( NULL ) , "%r, %a-%d-%b-%y" ), buf );
      FCLOSE( pFile );

      /* 改變檔案存取模式 */
      set_file_mode( ideas_file );
    }

    break;

  /* 記錄不雅字 */
  case LOG_LOWLIFE:

    if ( merc_exec && depth == 1 ) notify( L_ANG, NOTIFY_XNAME, buf );

    if ( ( pFile = FOPEN( xname_log , "a" ) ) )
    {
      fprintf( pFile ,"(%s) %s\n"
        , time_format( time( NULL ) , "%r, %a-%d-%b-%y" ), buf );
      FCLOSE( pFile );

      /* 改變檔案存取模式 */
      set_file_mode( xname_log );
    }

    mudlog( LOG_INFO, "%s", buf );
    break;

  case LOG_TYPO:

    if ( ( pFile = FOPEN( typo_file , "a" ) ) )
    {
      fprintf( pFile ,"(%s) %s\n"
        , time_format( time( NULL ) , "%r, %a-%d-%b-%y" ), buf );
      FCLOSE( pFile );

      /* 改變檔案存取模式 */
      set_file_mode( typo_file );
    }
    break;

  /* 鎖定網際連線 */
  case LOG_NET:

    if ( merc_exec && depth == 1 ) notify( L_ANG, NOTIFY_NET, buf );

    if ( ( pFile = FOPEN( net_file , "a" ) ) )
    {
      fprintf( pFile ,"(%s) %s\n"
        , time_format( time( NULL ) , "%r, %a-%d-%b-%y" ), buf );
      FCLOSE( pFile );

      /* 改變檔案存取模式 */
      set_file_mode( net_file );
    }

    mudlog( LOG_INFO, "%s", buf );
    break;

  case LOG_CHAT:


    if ( ChatLog && ( pFile = FOPEN( chat_log , "a" ) ) )
    {
      fprintf( pFile ,"(%s) %s\n"
        , time_format( time( NULL ) , "%r, %a-%d-%b-%y" ), buf );

      FCLOSE( pFile );
    }

    break;

  /* 標記自殺 */
  case LOG_SUICIDE:

    if ( ( pFile = FOPEN( suicide_log , "a" ) ) )
    {
      fprintf( pFile ,"(%s) %s\n"
        , time_format( time( NULL ) , "%r, %a-%d-%b-%y" ), buf );

      FCLOSE( pFile );

      /* 改變檔案存取模式 */
      set_file_mode( suicide_log );
    }

    break;

  /* 標記清除玩家檔案 */
  case LOG_PURGE:

    if ( ( pFile = FOPEN( purge_file , "a" ) ) )
    {
      fprintf( pFile ,"(%s) %s\n"
        , time_format( time( NULL ) , "%r, %a-%d-%b-%y" ), buf );

      FCLOSE( pFile );

      /* 改變檔案存取模式 */
      set_file_mode( purge_file );
    }

    break;
  }

  depth--;
  RETURN_NULL();
}

void notify( int level , int toggler , const char * buffer , ... )
{
  va_list           args;
  char              buf[ MAX_OUTPUT_LENGTH ];
  DESCRIPTOR_DATA * man;

  PUSH_FUNCTION( "notify" );

  va_start( args , buffer );
  vsprintf( buf  , buffer , args );
  va_end( args );

  for ( man = descriptor_list; man; man = man->next )
  {
    if ( !verify_desc( man ) ) continue;

    if ( man->character
      && man->connected == CON_PLAYING
      && get_trust( man->character ) >= level
      && !IS_SET( man->character->notify, toggler ) )
    {
      print_to_char( man->character, "「\e[1;32m系統精靈\e[0m」%s\n\r", buf );
    }
  }

  RETURN_NULL();
}

void output_buffer( void )
{
  FILE * pFile;

  PUSH_FUNCTION( "output_buffer" );

  if ( BugTimes > 0 && BugBuffer[0] )
  {
    if ( ( pFile = FOPEN( bugs_file , "a" ) ) )
    {
      fprintf( pFile ,"(%s) [***] BUG: 上述訊息持續 %d 次。\n"
        , time_format( time( NULL ) , "%r, %a-%d-%b-%y" ), BugTimes );

      fflush( pFile );
      FCLOSE( pFile );
    }

    BugTimes     = 0;
    BugBuffer[0] = '\x0';
  }

  RETURN_NULL();
}

void file_seek( FILE_DATA * pFile )
{
  char * pString;

  PUSH_FUNCTION( "file_seek" );

  FileLine = 0;
  FileWord = 0;

  if ( !pFile || pFile->fp == stdin ) RETURN_NULL();

  FileLine   = 1;
  FileWord   = 1;
  pString = Now_File->address;

  for ( ; pString < Now_File->pointer; pString++ )
  {
    if ( *pString == '\n' )
    {
      FileWord = 1;
      FileLine++;
      continue;
    }

    FileWord++;
  }

  RETURN_NULL();
}

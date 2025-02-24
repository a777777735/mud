#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "merc.h"

FILE_DATA *     file_list       = NULL;
FILE_DATA *     Now_File        = NULL;
int             file_errno      = NO_ERROR;
int             top_file        = 0;
int             total_file      = 0;
unsigned int    FileError       = 0;
bool            ReadNumberError = FALSE;
extern bool     fBootDb;
extern bool     fReload;

const char *    file_err_msg    [] =
{
  /*  0 */ "沒有任何錯誤"                       ,
  /*  1 */ "缺乏檔案名稱"                       ,
  /*  2 */ "開啟檔案失敗(fopen)"                ,
  /*  3 */ "檔案已經存在﹐無法開啟"             ,
  /*  4 */ "目的檔案是一個目錄"                 ,
  /*  5 */ "權限無法開啟檔案或目錄"             ,
  /*  6 */ "檔案或目錄路徑太長"                 ,
  /*  7 */ "路徑內容不存在或是不合法"           ,
  /*  8 */ "目的不是一個目錄"                   ,
  /*  9 */ "O_NONBLOCK O_WRONLY 同時被設定"     ,
  /* 10 */ "沒有這項硬體設備"                   ,
  /* 11 */ "目的檔案是唯讀設備"                 ,
  /* 12 */ "設備繁忙中"                         ,
  /* 13 */ "路徑位址不合法"                     ,
  /* 14 */ "太多符號連結"                       ,
  /* 15 */ "硬體沒有足夠的空及來儲存"           ,
  /* 16 */ "核心記憶體不足"                     ,
  /* 17 */ "行程開啟檔案總數已達上限"           ,
  /* 18 */ "系統檔案已達上限"                   ,
  /* 19 */ "檔案重複被開啟"                     ,
  /* 20 */ "關檔時來源不正確"                   ,
  /* 21 */ "關檔時結構釋放有問題"               ,
  /* 22 */ "無法取得檔案資料"                   ,
  /* 23 */ "無法取得共享記憶體"                 ,
  /* 24 */ "已達檔案尾端"                       ,
  /* 25 */ "讀取字串太大"                       ,
  ""
};

FILE_DATA * f_open( const char * path, const char * mode )
{
  struct stat   pSt;
  FILE        * fp;
  FILE_DATA   * pFile;
  void        * region;

  PUSH_FUNCTION( "f_open" );

  if ( !path || !*path )
  {
    file_errno = 1;
    RETURN( NULL );
  }

  for ( pFile = file_list; pFile; pFile = pFile->next )
  {
    if ( !strcmp( pFile->filename, path ) )
    {
      pFile->error = 19;
      file_errno   = 19;
      RETURN( NULL );
    }
  }

  if ( stat( path, &pSt ) != 0 )
  {
    file_errno = 22;
    RETURN( NULL );
  }

  if ( !( fp = FOPEN( path, mode ) ) )
  {
    switch( errno )
    {
    default:            file_errno =  2; break;
    case EEXIST:        file_errno =  3; break;
    case EISDIR:        file_errno =  4; break;
    case EACCES:        file_errno =  5; break;
    case ENAMETOOLONG:  file_errno =  6; break;
    case ENOENT:        file_errno =  7; break;
    case ENOTDIR:       file_errno =  8; break;
    case ENXIO:         file_errno =  9; break;
    case ENODEV:        file_errno = 10; break;
    case EROFS:         file_errno = 11; break;
    case ETXTBSY:       file_errno = 12; break;
    case EFAULT:        file_errno = 13; break;
    case ELOOP:         file_errno = 14; break;
    case ENOSPC:        file_errno = 15; break;
    case ENOMEM:        file_errno = 16; break;
    case EMFILE:        file_errno = 17; break;
    case ENFILE:        file_errno = 18; break;
    }
  }

  pFile           = alloc_struct( STRUCT_FILE_DATA );
  pFile->fp       = fp;
  pFile->size     = pSt.st_size;
  pFile->error    = NO_ERROR;
  pFile->filename = str_dup( path );
  pFile->fd       = fileno( fp );
  pFile->timer    = current_time;
  file_errno      = NO_ERROR;

  region = mmap( NULL, pSt.st_size, PROT_READ, MAP_SHARED, pFile->fd, 0 );

  if ( region == ( caddr_t ) -1 )
  {
    file_errno = 23;
    FCLOSE( fp );
    free_struct( pFile, STRUCT_FILE_DATA );
    RETURN( NULL );
  }

  pFile->address  = region;
  pFile->pointer  = region;
  pFile->boundary = pFile->address + pFile->size;
  pFile->next     = file_list;
  file_list       = pFile;
  Now_File        = pFile;

  top_file++;
  total_file++;

  RETURN( pFile );
}

bool f_close( FILE_DATA * pFile )
{
  FILE_DATA * aFile;

  PUSH_FUNCTION( "f_close" );

  if ( !pFile )
  {
    file_errno = 20;
    RETURN( FALSE );
  }

  if ( pFile == file_list )
  {
    file_list = pFile->next;
  }

  else
  {
    for ( aFile = file_list; aFile; aFile = aFile->next )
    {
      if ( aFile->next == pFile )
      {
        aFile->next = pFile->next;
        break;
      }
    }

    if ( !aFile )
    {
      pFile->error = file_errno = 21;
      RETURN( FALSE );
    }
  }

  munmap( pFile->address, pFile->size );

  FCLOSE( pFile->fp );
  top_file--;
  free_struct( pFile, STRUCT_FILE_DATA );

  file_errno = NO_ERROR;
  Now_File   = NULL;

  RETURN( TRUE );
}

FUNCTION( do_fcntl )
{
  char        arg[MAX_INPUT_LENGTH];
  char        buf[MAX_STRING_LENGTH];
  char        buf2[MAX_STRING_LENGTH];
  int         count;
  FILE_DATA * pFile;

  PUSH_FUNCTION( "do_fcntl" );

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    print_to_char( ch, "系統保留檔案 %s 「號碼﹕%d」\n\r"
      , YESNO( fileno( fpReserve ) >= 0 ), fileno( fpReserve ) );

    act( "系統目前總共開啟過 $x 個檔案。$A", ch, &total_file, NULL, TO_CHAR );

    if ( !file_list )
    {
      act( "目前$t沒有開啟任何檔案﹗", ch, mud_name, NULL, TO_CHAR );
      RETURN_NULL();
    }

    clear_buffer();
    send_to_buffer( "\e[1;33;44m順序 號碼 時  間 檔  名                "
      "        錯誤 錯 誤 訊 息                 \e[0m\n\r" );

    for ( count = 0, pFile = file_list; pFile; pFile = pFile->next )
    {
      if ( !str_prefix( home_dir, pFile->filename ) )
      {
        buf[0] = '~';
        str_cpy( buf + 1, pFile->filename + str_len( home_dir ) );
      }
      else
      {
        str_cpy( buf, pFile->filename );
      }
      buf[29] = '\x0';

      str_cpy( buf2, error_string( pFile ) );
      buf2[28] = '\x0';

      send_to_buffer( "%s%3d %4d %6d %-29s %4d %-28s\n\r"
        , Now_File == pFile ? "\e[1;32m*\e[0m" : " "
        , ++count, pFile->fd
        , current_time - pFile->timer
        , buf, pFile->error, buf2 );
    }

    print_buffer( ch );
  }

  else
  {
    send_to_char( "對不起﹐你的參數錯誤﹐請查詢 fcntl 的使用方法﹗\n\r", ch );
  }

  RETURN_NULL();
}

const char * error_string( FILE_DATA * pFile )
{
  int size;

  PUSH_FUNCTION( "error_string" );

  if ( !pFile )
  {
    mudlog( LOG_DEBUG, "error_string: 來源不正確." );
    RETURN( "\x0" );
  }

  size = sizeof( file_err_msg ) / sizeof( file_err_msg[0] ) - 1;

  if ( pFile->error < 0 || pFile->error >= size )
  {
    mudlog( LOG_DEBUG, "error_string: 超出範圍(%d)", pFile->error );
    RETURN( "\x0" );
  }

  RETURN( file_err_msg[pFile->error] );
}

char fread_letter( FILE_DATA * pFile )
{
  char Char;

  PUSH_FUNCTION( "fread_letter" );

  Now_File = pFile;

  do
  {
    Char = *pFile->pointer++;

    if ( pFile->pointer >= pFile->boundary )
    {
      pFile->error = 24;

      mudlog( fBootDb || fReload ? LOG_DEBUG : LOG_ERR,
        "fread_letter: 已達檔案盡頭." );

      FileError |= ERROR_LETTER;
      RETURN( '\x0' );
    }

  } while ( isSpace( ( int ) Char ) );

  RETURN( Char );
}

int fread_number( FILE_DATA * pFile )
{
  int  number;
  bool sign;
  char Char;
  char word[MAX_INPUT_LENGTH];

  PUSH_FUNCTION( "fread_number" );

  Now_File        = pFile;
  number          = 0;
  sign            = FALSE;
  ReadNumberError = FALSE;

  do
  {
    Char = *pFile->pointer++;
  } while ( isSpace( ( int ) Char ) );

  if ( Char == '+' || Char == '|' )
  {
    Char = *pFile->pointer++;
  }

  else if ( Char == '-' )
  {
    sign = TRUE;
    Char = *pFile->pointer++;
  }

  if ( !isdigit( ( int ) Char ) )
  {
    pFile->pointer--;

    if ( !isalpha( ( int ) Char ) )
    {
      mudlog( fBootDb || fReload ? LOG_DEBUG : LOG_ERR
        , "Fread_number: 非字串常數." );

      FileError |= ERROR_NUMBER;
      RETURN( 0 );
    }

    str_cpy( word, fread_alpha( pFile ) );

    if ( symbol_lookup( word, &number ) == FALSE )
    {
      if ( merc_pid != getpid() )
      {
        mudlog( LOG_DEBUG, "Fread_number: 子行程讀取定義字串 %s 錯誤.", word );
        FileError |= ERROR_NUMBER;
        ReadNumberError = TRUE;
        RETURN( -1 );
      }

      mudlog( fBootDb || fReload ? LOG_DEBUG : LOG_ERR,
        "Fread_number: 讀取檔案已定義字串 %s 錯誤.", word );

      FileError |= ERROR_NUMBER;
      RETURN( 0 );
    }

    Char = *pFile->pointer;

    if ( Char != '+' && Char != '-' && Char != '|' ) RETURN( number );
    RETURN( number + fread_number( pFile ) );
  }

  while ( isdigit( ( int ) Char ) )
  {
    number = number * 10 + Char - '0';
    Char   = *pFile->pointer++;
  }

  if ( sign ) number = 0 - number;

       if ( Char == '|'                 ) number += fread_number( pFile );
  else if ( Char != ' ' && Char != '\t' ) pFile->pointer--;

  RETURN( number );
}

char * fread_string( FILE_DATA * pFile )
{
  char spool[ MAX_STRING_LENGTH ];
  char Char;
  int  len;
  bool fChinese;
  bool fLast;

  PUSH_FUNCTION( "fread_string" );

  Now_File = pFile;

  do
  {
    Char = *pFile->pointer++;

    if ( Char == '~' ) RETURN( &str_empty[0] );

    if ( pFile->pointer >= pFile->boundary )
    {
      pFile->error = 24;

      mudlog( fBootDb || fReload ? LOG_DEBUG : LOG_ERR
        , "fread_string: 讀取到檔尾." );

      FileError |= ERROR_STRING;
      RETURN( "" );
    }

  } while ( isSpace( ( int ) Char ) );

  pFile->pointer--;

  for ( len = 0, fChinese = FALSE ;; )
  {
    if ( pFile->pointer >= pFile->boundary )
    {
      pFile->error = 24;

      mudlog( fBootDb || fReload ? LOG_DEBUG : LOG_ERR
        , "fread_string: 讀取到檔尾." );

      FileError |= ERROR_STRING;
      RETURN( "" );
    }

    if ( len > sizeof( spool ) - 5 )
    {
      pFile->error = 25;

      mudlog( fBootDb || fReload ? LOG_DEBUG : LOG_CRIT
        , "fread_string: 讀取字串太大." );

      FileError |= ERROR_STRING;
      RETURN( "" );
    }

    fLast = fChinese;
    Char  = *pFile->pointer++;

         if ( fChinese )                                 fChinese = FALSE;
    else if ( ( Char >= '\x81' ) && ( Char <= '\xFF' ) ) fChinese =  TRUE;
    else                                                 fChinese = FALSE;

    switch ( Char )
    {
    default:
      spool[len++] = Char;
      break;

    case '\n':
      spool[len++] = '\n';
      spool[len++] = '\r';
      break;

    case '\r':
      break;

    case '<':

      Char = *pFile->pointer++;
      if ( Char == '<' )
      {
        spool[len++] = '\e';
        spool[len++] =  '[';
      }

      else
      {
        spool[len++] = '<';
        if ( pFile->pointer < pFile->boundary ) pFile->pointer--;
      }

      break;

    case '~':

      if ( fLast )
      {
        spool[len++] = '~';
        break;
      }

      spool[len] = '\x0';
      RETURN( str_dup( spool ) );
    }
  }
}

/* 讀取 alpha 字元..除了 + - | 以外 ..專為  fread_number 設計的函數 */
char * fread_alpha( FILE_DATA * pFile )
{
  static char   word[MAX_INPUT_LENGTH];
  char        * pWord;
  char          Char;

  PUSH_FUNCTION( "fread_alpha" );

  do
  {
    Char = *pFile->pointer++;

    if ( pFile->pointer >= pFile->boundary )
    {
      pFile->error = 24;

      mudlog( fBootDb || fReload ? LOG_DEBUG : LOG_ERR
        , "fread_alpha: 已達檔案盡頭." );

      FileError |= ERROR_ALPHA;
      RETURN( "" );
    }

  } while ( isSpace( ( int ) Char ) );

  pFile->pointer--;

  for ( pWord = word; pWord < word + sizeof( word ) - 1; pWord++ )
  {
    if ( pFile->pointer >= pFile->boundary )
    {
      *pWord = '\x0';
      RETURN( word );
    }

    *pWord = *pFile->pointer++;
    switch( *pWord )
    {
    case ' ' :
    case '|' :
    case '\n':
    case '\r':
    case '-' :
    case '+' :
    case '\t':
      pFile->pointer--;
      *pWord = '\x0';
      RETURN( word );
    }
  }

  mudlog( fBootDb || fReload ? LOG_DEBUG : LOG_ERR, "fread_alpha: 字串太長." );
  FileError |= ERROR_ALPHA;

  RETURN( "" );
}

char * fread_word( FILE_DATA * pFile )
{
  static char   word[ MAX_INPUT_LENGTH ];
  char        * pWord;
  char          cEnd;

  PUSH_FUNCTION( "fread_word" );

  Now_File = pFile;

  do
  {
    cEnd = *pFile->pointer++;
  } while ( isSpace( ( int ) cEnd ) && pFile->pointer < pFile->boundary );

  if ( cEnd == '\'' || cEnd == '"' )
  {
    pWord = word;
  }

  else if ( pFile->pointer >= pFile->boundary )
  {
    mudlog( fBootDb || fReload ? LOG_DEBUG : LOG_ERR
      , "fread_word: 已達檔案盡頭." );

    FileError |= ERROR_WORD;
    RETURN( "" );
  }

  else
  {
    word[0] = cEnd;
    pWord   = word + 1;
    cEnd    = ' ';
  }

  for ( ; pWord < word + sizeof( word ) - 1; pWord++ )
  {
    if ( pFile->pointer >= pFile->boundary )
    {
      *pWord = '\x0';
      RETURN( word );
    }

    *pWord = *pFile->pointer++;

    if ( cEnd == ' ' ? isSpace( ( int ) *pWord ) : *pWord == cEnd )
    {
      if ( cEnd == ' ' ) pFile->pointer--;
      *pWord = '\x0';
      RETURN( word );
    }
  }

  mudlog( fBootDb || fReload ? LOG_DEBUG : LOG_ERR , "fread_word: 字組太長." );
  FileError |= ERROR_WORD;

  RETURN( "" );
}

/* 測試是否到達檔案尾端 */
bool if_eof( FILE_DATA * pFile )
{
  char Char;

  PUSH_FUNCTION( "if_eof" );

  for ( ;; )
  {
    if ( pFile->pointer >= pFile->boundary ) RETURN( TRUE );

    Char = *pFile->pointer++;


    /* if ( Char != ' '  && Char != '\n' && Char != '\r' )
         && Char != '\t' && Char != '\v' ) */

    if ( Char != ' ' && Char != '\n' && Char != '\r' && Char != '\t' )
    {
      pFile->pointer--;
      RETURN( FALSE );
    }
  }
}

/* 測試是否已達到檔案某一行的行末 */
bool fread_if_eol( FILE_DATA * pFile )
{
  char Char;

  PUSH_FUNCTION( "fread_if_eol" );

  do
  {
    Char = *pFile->pointer++;
  } while ( Char == ' ' || Char == '\t' );

  if ( pFile->pointer >= pFile->boundary ) RETURN( TRUE );

  pFile->pointer--;

  if ( Char == '\n' || Char == '\r' ) RETURN( TRUE );

  RETURN( FALSE );
}

void fread_to_eol( FILE_DATA * pFile )
{
  char Char;

  PUSH_FUNCTION( "fread_to_eol" );

  do
  {
    Char = *pFile->pointer++;
    if ( pFile->pointer >= pFile->boundary ) RETURN_NULL();
  } while( Char != '\n'&& Char != '\r' );

  do
  {
    Char = *pFile->pointer++;
    if ( pFile->pointer >= pFile->boundary ) RETURN_NULL();
  } while ( Char == '\n' || Char == '\r' );

  pFile->pointer--;
  RETURN_NULL();
}

char * map_file( FILE_DATA * pFile )
{
  static char buf[MAX_STRING_LENGTH];
  int         loop;

  PUSH_FUNCTION( "map_file" );

  for ( loop = 0; pFile->pointer < pFile->boundary; loop++ )
  {
    if ( loop >= sizeof( buf ) - 1 ) break;
    buf[loop] = *pFile->pointer++;
  }

  buf[loop] = '\x0';
  RETURN( buf );
}

void fw_string( FILE_DATA * pFile, char * title, const char * string )
{
  PUSH_FUNCTION( "fw_string" );

  Now_File = pFile;
  fprintf( pFile->fp, "%s%s~\n", str_space( title, BLANK_LENGTH ), string );

  RETURN_NULL();
}

void fw_word( FILE_DATA * pFile, char * title, const char * string )
{
  PUSH_FUNCTION( "fw_word" );

  Now_File = pFile;
  fprintf( pFile->fp, "%s'%s'\n", str_space( title, BLANK_LENGTH ), string );

  RETURN_NULL();
}

void fw_number( FILE_DATA * pFile, char * title, int number )
{
  PUSH_FUNCTION( "fw_number" );

  Now_File = pFile;
  fprintf( pFile->fp, "%s%d\n" , str_space( title, BLANK_LENGTH ), number );

  RETURN_NULL();
}

void fw_out( FILE_DATA * pFile, const char * title )
{
  PUSH_FUNCTION( "fw_out" );

  Now_File = pFile;
  fprintf( pFile->fp, "%s\n", title );

  RETURN_NULL();
}

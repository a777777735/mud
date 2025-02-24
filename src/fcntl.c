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
  /*  0 */ "�S��������~"                       ,
  /*  1 */ "�ʥF�ɮצW��"                       ,
  /*  2 */ "�}���ɮץ���(fopen)"                ,
  /*  3 */ "�ɮפw�g�s�b�M�L�k�}��"             ,
  /*  4 */ "�ت��ɮ׬O�@�ӥؿ�"                 ,
  /*  5 */ "�v���L�k�}���ɮשΥؿ�"             ,
  /*  6 */ "�ɮשΥؿ����|�Ӫ�"                 ,
  /*  7 */ "���|���e���s�b�άO���X�k"           ,
  /*  8 */ "�ت����O�@�ӥؿ�"                   ,
  /*  9 */ "O_NONBLOCK O_WRONLY �P�ɳQ�]�w"     ,
  /* 10 */ "�S���o���w��]��"                   ,
  /* 11 */ "�ت��ɮ׬O��Ū�]��"                 ,
  /* 12 */ "�]���c����"                         ,
  /* 13 */ "���|��}���X�k"                     ,
  /* 14 */ "�Ӧh�Ÿ��s��"                       ,
  /* 15 */ "�w��S���������ŤΨ��x�s"           ,
  /* 16 */ "�֤߰O���餣��"                     ,
  /* 17 */ "��{�}���ɮ��`�Ƥw�F�W��"           ,
  /* 18 */ "�t���ɮפw�F�W��"                   ,
  /* 19 */ "�ɮ׭��ƳQ�}��"                     ,
  /* 20 */ "���ɮɨӷ������T"                   ,
  /* 21 */ "���ɮɵ��c���񦳰��D"               ,
  /* 22 */ "�L�k���o�ɮ׸��"                   ,
  /* 23 */ "�L�k���o�@�ɰO����"                 ,
  /* 24 */ "�w�F�ɮק���"                       ,
  /* 25 */ "Ū���r��Ӥj"                       ,
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
    print_to_char( ch, "�t�ΫO�d�ɮ� %s �u���X�R%d�v\n\r"
      , YESNO( fileno( fpReserve ) >= 0 ), fileno( fpReserve ) );

    act( "�t�Υثe�`�@�}�ҹL $x ���ɮסC$A", ch, &total_file, NULL, TO_CHAR );

    if ( !file_list )
    {
      act( "�ثe$t�S���}�ҥ����ɮסT", ch, mud_name, NULL, TO_CHAR );
      RETURN_NULL();
    }

    clear_buffer();
    send_to_buffer( "\e[1;33;44m���� ���X ��  �� ��  �W                "
      "        ���~ �� �~ �T ��                 \e[0m\n\r" );

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
    send_to_char( "�藍�_�M�A���Ѽƿ��~�M�Ьd�� fcntl ���ϥΤ�k�T\n\r", ch );
  }

  RETURN_NULL();
}

const char * error_string( FILE_DATA * pFile )
{
  int size;

  PUSH_FUNCTION( "error_string" );

  if ( !pFile )
  {
    mudlog( LOG_DEBUG, "error_string: �ӷ������T." );
    RETURN( "\x0" );
  }

  size = sizeof( file_err_msg ) / sizeof( file_err_msg[0] ) - 1;

  if ( pFile->error < 0 || pFile->error >= size )
  {
    mudlog( LOG_DEBUG, "error_string: �W�X�d��(%d)", pFile->error );
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
        "fread_letter: �w�F�ɮ׺��Y." );

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
        , "Fread_number: �D�r��`��." );

      FileError |= ERROR_NUMBER;
      RETURN( 0 );
    }

    str_cpy( word, fread_alpha( pFile ) );

    if ( symbol_lookup( word, &number ) == FALSE )
    {
      if ( merc_pid != getpid() )
      {
        mudlog( LOG_DEBUG, "Fread_number: �l��{Ū���w�q�r�� %s ���~.", word );
        FileError |= ERROR_NUMBER;
        ReadNumberError = TRUE;
        RETURN( -1 );
      }

      mudlog( fBootDb || fReload ? LOG_DEBUG : LOG_ERR,
        "Fread_number: Ū���ɮפw�w�q�r�� %s ���~.", word );

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
        , "fread_string: Ū�����ɧ�." );

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
        , "fread_string: Ū�����ɧ�." );

      FileError |= ERROR_STRING;
      RETURN( "" );
    }

    if ( len > sizeof( spool ) - 5 )
    {
      pFile->error = 25;

      mudlog( fBootDb || fReload ? LOG_DEBUG : LOG_CRIT
        , "fread_string: Ū���r��Ӥj." );

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

/* Ū�� alpha �r��..���F + - | �H�~ ..�M��  fread_number �]�p����� */
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
        , "fread_alpha: �w�F�ɮ׺��Y." );

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

  mudlog( fBootDb || fReload ? LOG_DEBUG : LOG_ERR, "fread_alpha: �r��Ӫ�." );
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
      , "fread_word: �w�F�ɮ׺��Y." );

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

  mudlog( fBootDb || fReload ? LOG_DEBUG : LOG_ERR , "fread_word: �r�դӪ�." );
  FileError |= ERROR_WORD;

  RETURN( "" );
}

/* ���լO�_��F�ɮק��� */
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

/* ���լO�_�w�F���ɮ׬Y�@�檺�楽 */
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

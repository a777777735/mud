/***************************************************************************
*  �o�O�ѻ��j�ƾǨt�s�@�s�Ҽ��g���C���M�D��� merc ��s�ӨӡM�Ҧ������v    *
*  �N�|�Q�O�d�M���w��j�a�ק�M���ڭ̤]�Ʊ�A�̤]�ണ�ѵ��j�a�M�Ҧ�����    *
*  �~�欰�N���Q���\�C                                                      *
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
  "\n\r\n\r\e[1;31m**** �A����X�w�İϤӤj�M�t�Φ۰ʽվ�C****\n\r\e[0m";

static char     BugBuffer[BUFSIZ] = { '\x0' };
char            FileOop[BUFSIZ];
static int      BugTimes = 0;
extern bool     fReload;
int             FileLine;
int             FileWord;

void            file_seek       args( ( FILE_DATA * ) );

/* �M���w�İ� */
void clear_buffer( void )
{
  PUSH_FUNCTION( "clear_buffer" );
  Buffer[0] = '\x0';
  BufferFull = FALSE;
  RETURN_NULL();
}

/* �M�����|�� */
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

/* �W�[�r���w�İ� */
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

  /* �O�_�W�X�ɽu, �O�̫h���_�L */
  if ( str_len( Buffer ) + str_len( buf ) >= sizeof( buf ) - 1 )
  {
    location = sizeof( buf ) - str_len( Buffer ) - str_len( buf ) - 1;
    buf[ UMAX( 0, location ) ] = '\x0';
    BufferFull = TRUE;
  }

  /* ������ت��ϸ� */
  str_cat( Buffer, buf );

  /* �Y�w�İϤw��, �h�e�Xĵ�i�r�� */
  if ( BufferFull )
  {
    Buffer[ sizeof( buf ) - str_len( Buffer_full_messages ) - 1] = '\x0';
    str_cat( Buffer, Buffer_full_messages );
    mudlog( LOG_INFO , "�uBUFFER�v ��X�w�İϤw��." );
  }

  RETURN_NULL();
}

/* �W�[�r���w�İ� */
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

  /* �O�_�W�X�ɽu, �O�̫h���_�L */
  if ( str_len( Stack ) + str_len( buf ) >= sizeof( buf ) - 1 )
  {
    location = sizeof( buf ) - str_len( Stack ) - str_len( buf ) - 1;
    buf[ UMAX( 0, location ) ] = '\x0';
    StackFull = TRUE;
  }

  /* ������ت��ϸ� */
  str_cat( Stack, buf );

  /* �Y�w�İϤw��, �h�e�Xĵ�i�r�� */
  if ( StackFull )
  {
    Stack[ sizeof( buf ) - str_len( Buffer_full_messages ) - 1] = '\x0';
    str_cat( Stack, Buffer_full_messages );
    mudlog( LOG_INFO , "�uSTACK�v ��X���|�Ϥw��." );
  }

  RETURN_NULL();
}

/* ��w�İϪ���ƥ����e�X�h�����a */
void print_buffer( CHAR_DATA * ch )
{
  PUSH_FUNCTION( "print_buffer" );
  send_to_char( Buffer , ch );
  clear_buffer();
  RETURN_NULL();
}

/* ����|�Ϫ���ƥ����e�X�h�����a */
void print_stack( CHAR_DATA * ch )
{
  PUSH_FUNCTION( "print_stack" );
  send_to_char( Stack , ch );
  clear_stack();
  RETURN_NULL();
}

/* ����|�Ϫ���Ʀ^�ǵ���� */
char * return_stack( void )
{
  PUSH_FUNCTION( "return_stack" );
  RETURN( Stack );
}

/* ��X�����a, �i�H���ܼƧΦ� */
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

  /* �p�G���O����{ */
  if ( getpid() != merc_pid ) RETURN_NULL();

  va_start( args , buffer );
  vsprintf( buf  , buffer , args );
  va_end( args );
  depth++;

  switch( type )
  {
  default :
    break;

  /* �t���Y�����~, ���O�t���٬O�i�H���m */
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
        fprintf( pFile , "�u�t���{�ɿ��~�v %s\n" , buffer );
        pop_function();
        break;

      case LOG_STACK:
        fprintf( pFile , "�u��ư��|���~�v %s\n" , buffer );
        break;
      }

      /* �O�_�}�Ұ����Ҧ� */
      if ( internal_debug )
      {
        for ( count = 0, loop = FunctionPointer + 1; loop < MAX_DEBUG; loop++ )
        {
          if ( !FunctionName[loop] ) break;
          fprintf( pFile , "(%2d) ��ơR%-20s\n", ++count, FunctionName[loop] );
        }

        fprintf( pFile, "\n" );
      }

      fflush( pFile );
      FCLOSE( pFile );
    }

    if ( LastCommand    ) mudlog( LOG_INFO, "�̫�R�O %s"  ,LastCommand->name);
    if ( LastChar       ) mudlog( LOG_INFO, "�̫�ϥΪ� %s", LastChar->name  );
    if ( LastArgument[0]) mudlog( LOG_INFO, "�̫�Ѽ� %s"  , LastArgument    );

    /* ���������Ҧ��� socket */
    if ( merc_exec == TRUE )
    {
      mudlog( LOG_INFO , ""               );
      mudlog( LOG_INFO , "�t�Τ����`�פ�" );
      close_all_socket();
    }

    merc_exit( -1 );

  /* ��檬�p, ���O�٬O�i�H���m�t�� */
  case LOG_CRIT:

    mudlog( LOG_INFO , "%s", buf );

    if ( ( pFile = FOPEN( bugs_file , "a" ) ) )
    {
      fprintf( pFile ,"(%s) %s\n"
        , time_format( time( NULL ) , "%r, %a-%d-%b-%y" ), buf );

      fflush( pFile );
      FCLOSE( pFile );
    }

    if ( LastCommand    ) mudlog( LOG_INFO, "�̫�R�O %s" ,LastCommand->name);
    if ( LastChar       ) mudlog( LOG_INFO, "�̫�ϥΪ� %s", LastChar->name );
    if ( LastArgument[0]) mudlog( LOG_INFO, "�̫�Ѽ� %s"  , LastArgument   );

    /* ���������Ҧ��� socket */
    if ( merc_exec == TRUE )
    {
      mudlog( LOG_INFO , ""               );
      mudlog( LOG_INFO , "�t�Τ����`�פ�" );
      close_all_socket();
    }

    merc_exit( -1 );

  /* ��檬�p, �t�έ��m��ĳ���n */
  case LOG_ERR:

    mudlog( LOG_SHUTDOWN , "�t������" );
    mudlog( LOG_CRIT     , "%s", buf  );
    break;

  /* ��w���ګ��O */
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
          fprintf( pFile ,"(%s) [***] BUG: �W�z�T������ %d ���C\n"
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

        sprintf( errormsg , "�ɮ� %s �� %d �� �� %d �Ӧr���e�����~."
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

      mudlog( LOG_SHUTDOWN , "�t������" );
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

      /* �����ɮצs���Ҧ� */
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

      /* �����ɮצs���Ҧ� */
      set_file_mode( failpass_file );
    }

    break;

  case LOG_BADOBJECT:

    if ( ( pFile = FOPEN( badobject_file , "a" ) ) )
    {
      fprintf( pFile ,"%s\n", buf );
      FCLOSE( pFile );

      /* �����ɮצs���Ҧ� */
      set_file_mode( badobject_file );
    }

    break;

  case LOG_BADFILE:

    if ( ( pFile = FOPEN( bad_file , "a" ) ) )
    {
      fprintf( pFile ,"(%s) %s\n"
        , time_format( time( NULL ) , "%r, %a-%d-%b-%y" ), buf );
      FCLOSE( pFile );

      /* �����ɮצs���Ҧ� */
      set_file_mode( bad_file );
    }

    break;

  case LOG_SUSPECT:

    if ( ( pFile = FOPEN( suspect_file , "a" ) ) )
    {
      fprintf( pFile ,"(%s) %s\n"
        , time_format( time( NULL ) , "%r, %a-%d-%b-%y" ), buf );
      FCLOSE( pFile );

      /* �����ɮצs���Ҧ� */
      set_file_mode( suspect_file );
    }

    break;

  /* ���ڥN�z�ɮ� */
  case LOG_WIZFLAGS:

    if ( ( pFile = FOPEN( wizflags_log , "a" ) ) )
    {
      fprintf( pFile ,"(%s) %s\n"
        , time_format( time( NULL ) , "%r, %a-%d-%b-%y" ), buf );
      FCLOSE( pFile );

      /* �����ɮצs���Ҧ� */
      set_file_mode( wizflags_log );
    }

    break;

  /* �@���� */
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

      /* �����ɮצs���Ҧ� */
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

      /* �����ɮצs���Ҧ� */
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

      /* �����ɮצs���Ҧ� */
      set_file_mode( ideas_file );
    }

    break;

  /* �O�������r */
  case LOG_LOWLIFE:

    if ( merc_exec && depth == 1 ) notify( L_ANG, NOTIFY_XNAME, buf );

    if ( ( pFile = FOPEN( xname_log , "a" ) ) )
    {
      fprintf( pFile ,"(%s) %s\n"
        , time_format( time( NULL ) , "%r, %a-%d-%b-%y" ), buf );
      FCLOSE( pFile );

      /* �����ɮצs���Ҧ� */
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

      /* �����ɮצs���Ҧ� */
      set_file_mode( typo_file );
    }
    break;

  /* ��w���ڳs�u */
  case LOG_NET:

    if ( merc_exec && depth == 1 ) notify( L_ANG, NOTIFY_NET, buf );

    if ( ( pFile = FOPEN( net_file , "a" ) ) )
    {
      fprintf( pFile ,"(%s) %s\n"
        , time_format( time( NULL ) , "%r, %a-%d-%b-%y" ), buf );
      FCLOSE( pFile );

      /* �����ɮצs���Ҧ� */
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

  /* �аO�۱� */
  case LOG_SUICIDE:

    if ( ( pFile = FOPEN( suicide_log , "a" ) ) )
    {
      fprintf( pFile ,"(%s) %s\n"
        , time_format( time( NULL ) , "%r, %a-%d-%b-%y" ), buf );

      FCLOSE( pFile );

      /* �����ɮצs���Ҧ� */
      set_file_mode( suicide_log );
    }

    break;

  /* �аO�M�����a�ɮ� */
  case LOG_PURGE:

    if ( ( pFile = FOPEN( purge_file , "a" ) ) )
    {
      fprintf( pFile ,"(%s) %s\n"
        , time_format( time( NULL ) , "%r, %a-%d-%b-%y" ), buf );

      FCLOSE( pFile );

      /* �����ɮצs���Ҧ� */
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
      print_to_char( man->character, "�u\e[1;32m�t�κ��F\e[0m�v%s\n\r", buf );
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
      fprintf( pFile ,"(%s) [***] BUG: �W�z�T������ %d ���C\n"
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

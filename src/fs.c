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
#include <sys/stat.h>

#ifdef __linux__
#include <sys/sysmacros.h>
#endif

#if defined(sun)
#include <sys/mkdev.h>
#endif

#include <sys/mman.h>
#include <errno.h>
#include <stdlib.h>
#include <glob.h>
#include <ctype.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <fcntl.h>
#include "merc.h"

#define MAX_FILE        1000
#define MAX_PATH_DEPTH  32
#define STYLE_READ      1
#define STYLE_WRITE     2

char   type_of_file     args( ( mode_t ) );
char * file_info        args( ( char *, char *, struct stat *, bool ) );
char * smash_path       args( ( char * ) );
char * mud_path         args( ( const char * ) );
void   clear_workpath   args( ( void ) );
bool   set_workpath     args( ( CHAR_DATA *, const char *, int ) );
bool   send_to_path     args( ( CHAR_DATA *, const char *, int ) );
bool   can_access       args( ( CHAR_DATA *, char *, int ) );
void   print_no_access  args( ( CHAR_DATA *, int ) );
struct stat * mud_lstat args( ( char *, char *, struct stat * ) );
char * return_path      args( ( void ) );
void   scan_directory   args( ( CHAR_DATA *, char *, char * ) );
char    WorkPath[MAX_PATH_DEPTH][MAX_FILE_LENGTH];
int     WorkPathPointer;

void clear_workpath( void )
{
  int loop;

  PUSH_FUNCTION( "clear_workpath" );

  for ( WorkPathPointer = loop = 0; loop < MAX_PATH_DEPTH; loop++ )
    memset( WorkPath[loop], 0, MAX_FILE_LENGTH );

  RETURN_NULL();
}

bool set_workpath( CHAR_DATA * ch, const char * name, int style )
{
  char buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "set_workpath" );

  clear_workpath();

  if ( !ch || !ch->desc )
  {
    mudlog( LOG_DEBUG, "set_workpath: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( !name ) RETURN( TRUE );

  if ( *name == '/' )
  {
    while ( *name == '/' ) name++;
    str_cpy( buf, name );
  }

  else if ( ch->desc->path && *ch->desc->path )
  {
    sprintf( buf, "%s/%s", ch->desc->path, name );
  }

  else
  {
    str_cpy( buf, name );
  }

  if ( !send_to_path( ch, buf, style ) ) RETURN( FALSE );

  /* �O�_���ڥؿ� */
  if ( WorkPathPointer == 0 && !can_access( ch, "", style ) )
  {
    print_no_access( ch, style );
    RETURN( FALSE );
  }

  RETURN( TRUE );
}

bool send_to_path( CHAR_DATA * ch, const char * path, int style )
{
  struct stat  pSt;
  char         buf[MAX_STRING_LENGTH];
  char         access[MAX_STRING_LENGTH];
  char       * pString;
  int          loop;
  int          len;

  PUSH_FUNCTION( "send_to_path" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "send_to_path: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( !path || !*path ) RETURN( can_access( ch, "", style ) );

  if ( str_len( path ) >= sizeof( buf ) - 10 )
  {
    send_to_char( "���|���׶W�L�t�γ]�w�ȡC\n\r", ch );
    RETURN( FALSE );
  }

  str_cpy( buf, path );
  if ( ( len = str_len( buf ) ) > 0 && buf[len-1] != '/' ) str_cat( buf, "/"  );

  for ( loop = 0, pString = buf; *pString != '\x0'; pString++ )
  {
    if ( *pString == '/' )
    {
      if ( loop == 0 ) continue;

      if ( WorkPathPointer >= MAX_PATH_DEPTH )
      {
        clear_workpath();
        send_to_char( "�A�����|�Ӧh�h�F�C\n\r", ch );
        RETURN( FALSE );
      }

      if ( !mud_lstat( "", return_path(), &pSt ) )
      {
        act( "$t�R �䤣��o�ӥؿ��C", ch, return_path(), NULL, TO_CHAR );
        clear_workpath();
        RETURN( FALSE );
      }

      WorkPath[WorkPathPointer][loop] = '\x0';

      if ( !strcmp( WorkPath[WorkPathPointer] , "." ) )
      {
        memset( WorkPath[WorkPathPointer], 0, MAX_FILE_LENGTH );
      }

      else if ( !strcmp( WorkPath[WorkPathPointer], ".." ) )
      {
        if ( WorkPathPointer <= 0 )
        {
          clear_workpath();
          send_to_char( "�w�g�b�ڥؿ��M�L�k�b�^��W�@�h�C\n\r", ch );
          RETURN( FALSE );
        }

        memset( WorkPath[  WorkPathPointer], 0, MAX_FILE_LENGTH );
        memset( WorkPath[--WorkPathPointer], 0, MAX_FILE_LENGTH );
      }

      else
      {
        WorkPathPointer++;
      }

      loop = 0;
      str_cpy( access, return_path() );

      if ( mud_lstat( "", access, &pSt )
        && S_ISDIR( pSt.st_mode )
        && !can_access( ch, access, style ) )
      {
        print_no_access( ch, style );
        RETURN( FALSE );
      }
    }

    else
    {
      if ( WorkPathPointer >= MAX_PATH_DEPTH || loop >= MAX_FILE_LENGTH - 1 )
      {
        clear_workpath();
        send_to_char( "�A���ؿ����ɮצW�٤Ӫ��F�C\n\r", ch );
        RETURN( FALSE );
      }

      WorkPath[WorkPathPointer][loop++] = *pString;
    }
  }

       if ( loop > 0            ) WorkPath[WorkPathPointer][loop] = '\x0';
  else if ( WorkPathPointer > 0 ) WorkPathPointer--;


  RETURN( TRUE );
}

char * return_path( void )
{
  int         loop;
  static char buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "return_path" );

  buf[0] = '\x0';
  for ( loop = 0; loop < WorkPathPointer; loop++ )
  {
    str_cat( buf, WorkPath[loop] );
    str_cat( buf, "/"            );
  }

  RETURN( buf );
}

FUNCTION( do_md )
{
  struct stat   pSt;
  FILE        * pFile;
  char          arg[MAX_INPUT_LENGTH];
  char          filename[MAX_STRING_LENGTH];
  char          pathname[MAX_STRING_LENGTH];
  char          buf[MAX_STRING_LENGTH];
  char          temp[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "do_md" );

  if ( IS_NPC( ch ) || !ch->desc ) RETURN_NULL();

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    send_to_char( "�藍�_�M�A�n�إ߭��ӥؿ��O�S\n\r", ch );
    RETURN_NULL();
  }

  if ( !set_workpath( ch, arg, STYLE_WRITE ) ) RETURN_NULL();

  str_cpy( pathname, return_path() );
  str_cpy( filename, WorkPath[WorkPathPointer] );

  if ( !strcmp( filename, PermFile ) )
  {
    send_to_char( "�o�O�t�έ��n�ɮסM����ϥγo�ؤ覡���͡C\n\r", ch );
    RETURN_NULL();
  }

  if ( mud_lstat( pathname, filename, &pSt ) )
  {
    act( "$t�R �w�g�s�b�M�L�k���͡C", ch, arg, NULL, TO_CHAR );
    RETURN_NULL();
  }

  sprintf( buf, "%s/", smash_path( mud_path( pathname ) ) );
  str_cat( buf, filename );
  str_cpy( temp, buf );
  str_cat( temp, "/" );
  str_cat( temp, PermFile );

  if ( mkdir( smash_path( buf ), 0770 ) < 0 )
  {
    act( "$t�R$T�R���ͥؿ����ѡC", ch, arg, strerror( errno ), TO_CHAR );
    RETURN_NULL();
  }

  act( "���ͷs���ؿ� $t�C", ch, arg, NULL, TO_CHAR );

  if ( ( pFile = FOPEN( temp, "w+" ) ) )
  {
    fprintf( pFile, "R %s\nW %s\nA\nB\n", ch->name, ch->name );
    FCLOSE( pFile );
  }

  RETURN_NULL();
}

FUNCTION( do_touch )
{
  struct stat pSt;
  char        arg[MAX_INPUT_LENGTH];
  char        filename[MAX_STRING_LENGTH];
  char        pathname[MAX_STRING_LENGTH];
  char        buf[MAX_STRING_LENGTH];
  int         fd;

  PUSH_FUNCTION( "do_touch" );

  if ( IS_NPC( ch ) || !ch->desc ) RETURN_NULL();

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    send_to_char( "�藍�_�M�A�n���ͭ����ɮשO�S\n\r", ch );
    RETURN_NULL();
  }

  if ( !set_workpath( ch, arg, STYLE_WRITE ) ) RETURN_NULL();

  str_cpy( pathname, return_path() );
  str_cpy( filename, WorkPath[WorkPathPointer] );

  if ( !strcmp( filename, PermFile ) )
  {
    send_to_char( "�o�O�t�έ��n�ɮסM����ϥγo�ؤ覡���͡C\n\r", ch );
    RETURN_NULL();
  }

  if ( mud_lstat( pathname, filename, &pSt ) )
  {
    act( "$t�R�w�g�s�b�M�L�k���͡C", ch, arg, NULL, TO_CHAR );
    RETURN_NULL();
  }

  sprintf( buf, "%s/", smash_path( mud_path( pathname ) ) );
  str_cat( buf, filename );

  if ( ( fd = open( smash_path( buf ), O_CREAT, 0660 ) ) < 0 )
  {
    act( "$t�R$T�R�����ɮץ��ѡC", ch, arg, strerror( errno ), TO_CHAR );
    RETURN_NULL();
  }

  act( "���ͷs���ɮ� $t�C", ch, arg, NULL, TO_CHAR );
  close( fd );
  RETURN_NULL();
}

FUNCTION( do_rm )
{
  glob_t      result;
  struct stat pSt;
  char        arg[MAX_INPUT_LENGTH];
  char        filename[MAX_STRING_LENGTH];
  char        pathname[MAX_STRING_LENGTH];
  char        dirname[MAX_STRING_LENGTH];
  char        buf[MAX_STRING_LENGTH];
  int         count;
  int         loop;
  int         rc;

  PUSH_FUNCTION( "do_rm" );

  if ( IS_NPC( ch ) || !ch->desc ) RETURN_NULL();

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    send_to_char( "�藍�_�M�A�n�R�������ɮשO�S\n\r", ch );
    RETURN_NULL();
  }

  if ( !set_workpath( ch, arg, STYLE_WRITE ) ) RETURN_NULL();

  str_cpy( pathname, return_path() );
  str_cpy( filename, WorkPath[WorkPathPointer] );
  str_cpy( dirname, mud_path( pathname ) );

  if ( chdir( dirname ) != 0 )
  {
    send_to_char( "�L�k�������|�C\n\r", ch );
    chdir( CurrentDir );
    RETURN_NULL();
  }

  rc    = glob( filename, 0, NULL, &result );
  count = 0;
  chdir( CurrentDir );

  if ( rc == GLOB_NOSPACE )
  {
    send_to_char( "�O���餣���C\n\r", ch );
    RETURN_NULL();
  }

  for ( count = loop = 0; loop < result.gl_pathc; loop++ )
  {
    if ( !strcmp( result.gl_pathv[loop], "." )
      || !strcmp( result.gl_pathv[loop], ".." )
      || !strcmp( result.gl_pathv[loop], PermFile ) ) continue;

    if ( !mud_lstat( pathname, result.gl_pathv[loop], &pSt ) )
    {
      count++;
      mudlog( LOG_DEBUG, "do_rm: lstat: ���~(file)." );
      continue;
    }

    if ( S_ISDIR( pSt.st_mode ) )
    {
      count++;
      act( "$t�R�O�@�ӥؿ��M�L�k�R���C"
        , ch, result.gl_pathv[loop], NULL, TO_CHAR );
      continue;
    }

    if ( !S_ISREG( pSt.st_mode ) )
    {
      act( "$t�R���O�@�Ӹ���ɮסC"
        , ch, result.gl_pathv[loop], NULL, TO_CHAR );
      continue;
    }

    sprintf( buf, "%s/", smash_path( mud_path( pathname ) ) );
    str_cat( buf, result.gl_pathv[loop] );

    count++;
    if ( unlink( buf ) != 0 )
    {
      act( "$t�R$T�R�R���ɮץ��ѡC", ch, result.gl_pathv[loop]
        , strerror( errno ), TO_CHAR );
    }

    else
    {
      act( "�R���ɮ� $t�C", ch, result.gl_pathv[loop], NULL, TO_CHAR );
    }
  }

  globfree( &result );

  if ( count <= 0 )
  {
    if ( arg[0] == '\x0' ) send_to_char( "�o�ӥؿ��O�Ū��C\n\r", ch );
    else  act( "$t�R�䤣��o���ɮשΥؿ��C", ch, arg, NULL, TO_CHAR );
    RETURN_NULL();
  }

  RETURN_NULL();
}

FUNCTION( do_rd )
{
  DIR           * pDir;
  struct dirent * next;
  struct stat     pSt;
  char            arg[MAX_INPUT_LENGTH];
  char            pathname[MAX_STRING_LENGTH];
  char            buf[MAX_STRING_LENGTH];
  char            temp[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "do_rd" );

  if ( IS_NPC( ch ) || !ch->desc ) RETURN_NULL();

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    send_to_char( "�藍�_�M�A�n�R�����ӥؿ��O�S\n\r", ch );
    RETURN_NULL();
  }

  if ( !set_workpath( ch, arg, STYLE_WRITE ) ) RETURN_NULL();

  str_cpy( pathname, return_path() );
  str_cat( pathname, WorkPath[WorkPathPointer] );

  if ( !mud_lstat( "", pathname, &pSt ) )
  {
    act( "$t�R�S�������ɮשάO�ؿ��C", ch, arg, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( !S_ISDIR( pSt.st_mode ) )
  {
    act( "$t�R���O�@�ӥؿ��C", ch, arg, NULL, TO_CHAR );
  }

  else
  {
    str_cpy( temp, smash_path( mud_path( pathname ) ) );

    if ( ( pDir = opendir( temp ) ) )
    {
      while ( ( next = readdir( pDir ) ) )
      {
        if ( !strcmp( next->d_name, "." )
          || !strcmp( next->d_name, ".." )
          || !strcmp( next->d_name, PermFile ) )
          continue;

        act( "$t�R�ؿ����O�Ū��C", ch, arg, NULL, TO_CHAR );
        RETURN_NULL();
      }

      closedir( pDir );
    }
    else
    {
      act( "$t�R�}�ҥؿ��C����~�C", ch, arg, NULL, TO_CHAR );
      RETURN_NULL();
    }

    str_cpy( buf, smash_path( mud_path( pathname ) ) );
    str_cpy( temp, buf );
    str_cat( temp, "/" );
    str_cat( temp, PermFile );

    if ( unlink( temp ) != 0 )
    {
      act( "$t�R�R���ؿ����t���ɿ��~�C", ch, arg, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( rmdir( buf ) < 0 )
    {
      act( "$t�R$T�R�R���ؿ����ѡC", ch, arg, strerror( errno ), TO_CHAR );
    }
    else
    {
      act( "�R���ؿ� $t�C", ch, arg, NULL, TO_CHAR );
    }
  }

  RETURN_NULL();
}

FUNCTION( do_cp )
{
  FILE        * fp_source;
  FILE        * fp_target;
  struct stat   pSt;
  char          transfer;
  char          arg1[MAX_INPUT_LENGTH];
  char          arg2[MAX_INPUT_LENGTH];
  char          filename[MAX_STRING_LENGTH];
  char          pathname[MAX_STRING_LENGTH];
  char          source[MAX_STRING_LENGTH];
  char          target[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "do_cp" );

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );

  if ( arg1[0] == '\x0' || arg2[0] == '\x0' )
  {
    send_to_char( "�A�n���������ɮשO�S\n\r", ch );
    RETURN_NULL();
  }

  if ( !set_workpath( ch, arg1, STYLE_READ ) ) RETURN_NULL();

  str_cpy( pathname, return_path() );
  str_cpy( filename, WorkPath[WorkPathPointer] );

  if ( !mud_lstat( pathname, filename, &pSt ) )
  {
    act( "$t�R�S���o���ɮשΥؿ��C", ch, arg1, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( S_ISDIR( pSt.st_mode ) )
  {
    act( "$t�R�O�@�ӥؿ��M�L�k�i������C", ch, arg1, NULL, TO_CHAR );
    RETURN_NULL();
  }

  else if ( !S_ISREG( pSt.st_mode ) )
  {
    act( "$t�R���O�@���ɮסM�L�k�i������C", ch, arg1, NULL, TO_CHAR );
    RETURN_NULL();
  }

  sprintf( source, "%s/", smash_path( mud_path( pathname ) ) );
  str_cat( source, filename );

  if ( !set_workpath( ch, arg2, STYLE_WRITE ) ) RETURN_NULL();

  str_cpy( pathname, return_path() );
  str_cpy( filename, WorkPath[WorkPathPointer] );

  if ( !strcmp( filename, PermFile ) )
  {
    send_to_char( "�o�O�t�έ��n�ɮסM����惡������ʧ@�C\n\r", ch );
    RETURN_NULL();
  }

  if ( mud_lstat( pathname, filename, &pSt ) )
  {
    act( "$t�R�ɮשάO�ؿ��w�g�s�b�M�L�k�мg�T", ch, arg2, NULL, TO_CHAR );
    RETURN_NULL();
  }

  sprintf( target, "%s/", smash_path( mud_path( pathname ) ) );
  str_cat( target, filename );

  if ( ( fp_source = fopen( source, "r+" ) ) )
  {
    if ( ( fp_target = fopen( target, "w+" ) ) )
    {
      while ( ( transfer = fgetc( fp_source ) ) != EOF )
        fputc( transfer , fp_target );

      fclose( fp_target );

      act( "�����ɮ� $t �� $T�C", ch, arg1, arg2, TO_CHAR );
    }

    else
    {
      act( "$t�R�L�k�}�ҥت��ɮסC", ch, arg2, NULL, TO_CHAR );
    }

    fclose( fp_source );
  }

  else
  {
    act( "$t�R�L�k�}�Ҩӷ��ɮסC", ch, arg1, NULL, TO_CHAR );
  }

  RETURN_NULL();
}

FUNCTION( do_pwd )
{
  struct stat   pSt;
  void        * region;
  char        * pString;
  char        * aString;
  char        * path;
  char          filename[MAX_STRING_LENGTH];
  char          text[MAX_STRING_LENGTH];
  bool          empty;
  int           mode;
  int           fd;
  int           loop;
  int           count;
  int           level;

  PUSH_FUNCTION( "do_pwd" );

  if ( IS_NPC( ch ) || !ch->desc ) RETURN_NULL();

  path = ch->desc->path;

  clear_buffer();
  send_to_buffer( "�A�ثe���ؿ��� %s%s/\n\r"
    , ( path && *path ) ? "/" : "" , path );

  if ( !mud_lstat( path, PermFile, &pSt ) )
  {
    send_to_buffer( "�o�ӥؿ��S���]�w�ɡM�Ҧ��H���i�HŪ���M������g�J�T\n\r" );
  }

  else
  {
    sprintf( filename, "%s/%s/%s", home_dir, path, PermFile );

    if ( ( fd = open( filename, O_RDONLY ) ) < 0 )
    {
      send_to_buffer( "�}�ҳ]�w�ɿ��~�M�Ҧ��H���i�HŪ���M������g�J�T\n\r" );
    }

    else
    {
      region = mmap( NULL, pSt.st_size, PROT_READ, MAP_SHARED, fd, 0 );
      if ( region == ( caddr_t ) -1 )
      {
        close( fd );
        send_to_buffer( "Ū�����ؿ��]�w�ɿ��~�M�Ҧ��H���i�HŪ���M"
          "������g�J�T\n\r" );
      }

      else
      {
        pString = region;
        empty   = TRUE;

        for ( count = loop = 0; loop < pSt.st_size; loop++, pString++ )
        {
          switch( *pString )
          {
          default:

            if ( count <= sizeof( text ) - 10 ) text[count++] = *pString;
            if ( loop < pSt.st_size - 1 ) break;

          case '\n':
            text[count] = '\x0';

            if ( count > 0 )
            {
              switch( UPPER( text[0] ) )
              {
              default:
              case '*':
                count = 0;
                continue;

              case 'A':

                count = 0;
                empty = FALSE;
                send_to_buffer( "�H�U���H���L�k�i��Ū���ʧ@�T\n\r" );
                continue;

              case 'B':

                count = 0;
                empty = FALSE;
                send_to_buffer( "�H�U���H���L�k�i��g�J�ʧ@�T\n\r" );
                continue;

              case 'R':
                mode = STYLE_READ;
                break;

              case 'W':
                mode = STYLE_WRITE;
                break;
              }

              for ( aString = text + 1; *aString == ' '; aString++ );

              if ( *aString == '\x0' )
              {
                count = 0;
                continue;
              }

              else if ( is_number( aString ) )
              {
                level = atoi( aString );
                switch( mode )
                {
                case STYLE_READ:

                  empty = FALSE;
                  send_to_buffer( "���� %d �H�W�i�H�i��Ū���T\n\r", level );
                  break;

                case STYLE_WRITE:

                  empty = FALSE;
                  send_to_buffer( "���� %d �H�W�i�H�i��g�J�T\n\r", level );
                  break;
                }
              }

              else
              {
                switch( mode )
                {
                case STYLE_READ:

                  empty = FALSE;
                  send_to_buffer( "�W�r�� %s �i�H�i��Ū���C\n\r", aString );
                  break;

                case STYLE_WRITE:

                  empty = FALSE;
                  send_to_buffer( "�W�r�� %s �i�H�i��g�J�C\n\r", aString );
                  break;
                }
              }
            }

            count = 0;
            break;

          case '\r':
            break;

          case '\x0':
          case '\xFF':

            empty = FALSE;
            send_to_buffer( "�]�w�ɤ��e���~�M�Ҧ��H���i�HŪ���M"
              "������g�J�C\n\r" );
            break;
          }
        }

        munmap( region, pSt.st_size );

        if ( empty )
          send_to_buffer( "���ؿ��]�w�ɨS�����e�M�Ҧ��H������"
            "Ū���μg�J�T\n\r" );
      }

      close( fd );
    }
  }

  print_buffer( ch );
  RETURN_NULL();
}

FUNCTION( do_cd )
{
  struct stat       pSt;
  char              arg[MAX_INPUT_LENGTH];
  char              pathname[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA * pDesc;

  PUSH_FUNCTION( "do_cd" );

  if ( IS_NPC( ch ) || !( pDesc = ch->desc ) ) RETURN_NULL();

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    free_string( pDesc->path );
    pDesc->path = str_dup( "" );
    RETURN_NULL();
  }

  if ( !set_workpath( ch, arg, STYLE_READ ) ) RETURN_NULL();

  str_cpy( pathname, return_path() );
  str_cat( pathname, WorkPath[WorkPathPointer] );

  if ( !mud_lstat( "", pathname, &pSt ) )
  {
    act( "$t�R�䤣��o�ӥؿ��C", ch, arg, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( !S_ISDIR( pSt.st_mode ) )
  {
    act( "$t�R���O�@�ӥؿ��C", ch, arg, NULL, TO_CHAR );
    RETURN_NULL();
  }

  free_string( pDesc->path );
  pDesc->path = str_dup( pathname );
  RETURN_NULL();
}

FUNCTION( do_cat )
{
  struct stat   pSt;
  void        * region;
  char        * pString;
  char          arg[MAX_INPUT_LENGTH];
  char          arg1[MAX_INPUT_LENGTH];
  char          arg2[MAX_INPUT_LENGTH];
  char          pathname[MAX_STRING_LENGTH];
  char          filename[MAX_STRING_LENGTH];
  char          buf[MAX_OUTPUT_LENGTH];
  bool          regular;
  int           fd;
  int           loop;
  int           count;
  int           aLine;
  int           iLine = 1;
  int           LineCount = MAX_OUTPUT_LENGTH;

  PUSH_FUNCTION( "do_cat" );

  if ( IS_NPC( ch ) || !ch->desc ) RETURN_NULL();

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    send_to_char( "�A�n�[�ݭ����ɮשO�S\n\r", ch );
    RETURN_NULL();
  }

  if ( argument[0] != '\x0' )
  {
    argument = one_argument( argument, arg1 );

    if ( arg1[0] == '\x0' || !is_number( arg1 ) )
    {
      send_to_char( "�A�n�q�ĴX��}�l�[�ݡS\n\r", ch );
      RETURN_NULL();
    }

    if ( ( iLine = atoi( arg1 ) ) <= 0 )
    {
      send_to_char( "�A�}�l����Ƥ��X�k�C\n\r", ch );
      RETURN_NULL();
    }

    if ( argument[0] != '\x0' )
    {
      argument = one_argument( argument, arg2 );
      if ( arg2[0] == '\x0' || !is_number( arg2 ) )
      {
        send_to_char( "�A����n�ݴX��O�S\n\r", ch );
        RETURN_NULL();
      }

      if ( ( LineCount = atoi( arg2 ) ) <= 0 )
      {
        send_to_char( "�A����Ƥ��X�k�C\n\r", ch );
        RETURN_NULL();
      }
    }
  }

  if ( !set_workpath( ch, arg, STYLE_READ ) ) RETURN_NULL();

  str_cpy( pathname, return_path() );
  str_cpy( filename, WorkPath[WorkPathPointer] );

  if ( !strcmp( filename, PermFile ) )
  {
    send_to_char( "�o�O�t�έ��n�ɮסM����ϥγo�ؤ覡�[�ݡC\n\r", ch );
    RETURN_NULL();
  }

  if ( !mud_lstat( pathname, filename, &pSt ) )
  {
    act( "$t�R�S���o���ɮשΥؿ��C", ch, arg, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( !S_ISREG( pSt.st_mode ) )
  {
    act( "$t�R���O�@���ɮסC", ch, arg, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( pSt.st_size <= 0 )
  {
    act( "$t�R�ɮ׬O�Ū��C", ch, arg, NULL, TO_CHAR );
    RETURN_NULL();
  }

  str_cat( pathname, filename );
  str_cpy( buf, mud_path( pathname ) );

  if ( ( fd = open( smash_path( buf ), O_RDONLY ) ) < 0 )
  {
    act( "$t�R�L�k�}���ɮסC", ch, arg, NULL, TO_CHAR );
    RETURN_NULL();
  }

  region = mmap( NULL, pSt.st_size, PROT_READ, MAP_SHARED, fd, 0 );

  if ( region == ( caddr_t ) -1 )
  {
    act( "$t�R�L�k�ɮ׹����O����C", ch, arg, NULL, TO_CHAR );
    close( fd );
    RETURN_NULL();
  }

  pString = region;
  regular = TRUE;
  aLine   = 1;
  for ( count = loop = 0; loop < pSt.st_size; loop++, pString++ )
  {
    if ( count >= sizeof( buf ) - 50 )
    {
      const char * warning = "\n\r\n\r----�ɮפӤj-----\n\r";
      buf[count] = '\x0';
      str_cat( buf, warning );
      count += str_len( warning );
      break;
    }

    switch( *pString )
    {
    default:
      if ( aLine >= iLine ) buf[count++] = *pString;
      break;

    case '\n':

      if ( aLine >= iLine )
      {
        LineCount--;
        buf[count++] = '\n';
        buf[count++] = '\r';
      }

      aLine++;
      break;

    case '\r':
      break;

    case '\x0' :
    case '\xFF':
      regular = FALSE;
      break;
    }

    if ( LineCount <= 0 ) break;
  }

  if ( count == 0 )
  {
    send_to_char( "�b�A���w���d�򤺡M�S�������ơT\n\r", ch );
  }

  else if ( regular )
  {
    buf[count] = '\x0';
    clear_buffer();
    send_to_buffer( "%s", buf );
    print_buffer( ch );
  }

  else
  {
    act( "$t�R���O�@���r�ɮסC", ch, arg, NULL, TO_CHAR );
  }

  close( fd );
  munmap( region, pSt.st_size );
  RETURN_NULL();
}

FUNCTION( do_grep )
{
  struct stat   pSt;
  glob_t        result;
  char          arg1[MAX_INPUT_LENGTH];
  char          arg2[MAX_INPUT_LENGTH];
  char          filename[MAX_STRING_LENGTH];
  char          pathname[MAX_STRING_LENGTH];
  char          dirname[MAX_STRING_LENGTH];
  char        * pString[MAX_FILE];
  char          cLine[MAX_STRING_LENGTH];
  char        * pAddress;
  void        * region;
  bool          regular;
  int           iLine;
  int           loop;
  int           size;
  int           len;
  int           rc;
  int           count;
  int           fd;
  bool          bFound;

  PUSH_FUNCTION( "do_grep" );

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );

  if ( arg1[0] == '\x0' || arg2[0] == '\x0' )
  {
    send_to_char( "�A�n�q�����ɮק�M����r��S\n\r", ch );
    RETURN_NULL();
  }

  if ( !set_workpath( ch, arg2, STYLE_READ ) ) RETURN_NULL();

  str_cpy( pathname, return_path() );
  str_cpy( filename, WorkPath[WorkPathPointer] );

  for ( loop = 0; loop < MAX_FILE; loop++ ) pString[loop] = NULL;

  str_cpy( dirname, mud_path( pathname ) );

  if ( chdir( dirname ) != 0 )
  {
    send_to_char( "�L�k�������|�C\n\r", ch );
    chdir( CurrentDir );
    RETURN_NULL();
  }

  rc    = glob( filename, 0, NULL, &result );
  count = 0;
  chdir( CurrentDir );

  if ( rc == GLOB_NOSPACE )
  {
    send_to_char( "�O���餣���C\n\r", ch );
    RETURN_NULL();
  }

  for ( loop = 0; loop < result.gl_pathc; loop++ )
    pString[count++] = str_dup( result.gl_pathv[loop] );

  globfree( &result );

  if ( count <= 0 )
  {
    if ( arg2[0] == '\x0' )
    {
      send_to_char( "�o�ӥؿ��O�Ū��C\n\r", ch );
    }
    else
    {
      act( "$t�R�䤣��o���ɮשΥؿ��C", ch, arg2, NULL, TO_CHAR );
    }

    RETURN_NULL();
  }

  clear_buffer();
  for ( bFound = FALSE, loop = 0; loop < count; loop++ )
  {
    if ( !mud_lstat( pathname, pString[loop], &pSt ) )
    {
      mudlog( LOG_DEBUG, "do_grep: lstat: ���~(file)." );
      continue;
    }

    if ( S_ISDIR( pSt.st_mode ) )
    {
      send_to_buffer( "%s�R�O�@�ӥؿ��M�L�k�j�M�T\n\r", pString[loop] );
      continue;
    }

    if ( !S_ISREG( pSt.st_mode ) )
    {
      send_to_buffer( "%s�R���O�@���ɮסT\n\r", pString[loop] );
      continue;
    }

    sprintf( filename, "%s%s", dirname, pString[loop] );

    if ( ( fd = open( filename, O_RDONLY ) ) < 0 )
    {
      send_to_buffer( "%s�R�L�k�}���ɮסC\n\r", pString[loop] );
      continue;
    }

    region = mmap( NULL, pSt.st_size, PROT_READ, MAP_SHARED, fd, 0 );

    if ( region == ( caddr_t ) -1 )
    {
      send_to_buffer( "%s�R�L�k�ɮ׹����O����T\n\r", pString[loop] );
      close( fd );
      continue;
    }

    pAddress = region;
    regular  = TRUE;

    for ( iLine = 1, size = len = 0; len < pSt.st_size; len++, pAddress++ )
    {
      switch( *pAddress )
      {
      default:
        if ( size <= sizeof( cLine ) - 100 ) cLine[size++] = *pAddress;
        break;

      case '\n':

        cLine[size] = '\x0';

        if ( size > 0 && strstr( cLine, arg1 ) )
        {
          send_to_buffer( "%s�R%d�R%s\n\r", pString[loop], iLine, cLine );
          bFound = TRUE;
        }

        cLine[ ( size = 0 ) ] = '\x0';
        iLine++;
        break;

      case '\r':
        break;

      case '\x0' :
      case '\xFF':
        regular = FALSE;
        break;
      }

      if ( !regular ) break;
    }

    close( fd );
    munmap( region, pSt.st_size );
  }

  if ( !bFound ) send_to_buffer( "�S���o�{����ŦX����ơC\n\r" );
  print_buffer( ch );

  for ( loop = 0; loop < MAX_FILE; loop++ )
    if ( pString[loop] ) free_string( pString[loop] );

  RETURN_NULL();
}

FUNCTION( do_whereis )
{
  char arg[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "do_whereis" );

  if ( IS_NPC( ch ) || !ch->desc ) RETURN_NULL();

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    send_to_char( "�藍�_�M�A�n������ɮשO�S\n\r", ch );
    RETURN_NULL();
  }

  clear_buffer();
  scan_directory( ch, ch->desc->path, arg );
  print_buffer( ch );
  RETURN_NULL();
}

FUNCTION( do_ls )
{
  struct stat   pSt;
  glob_t        result;
  char          buf[MAX_STRING_LENGTH];
  char          pathname[MAX_STRING_LENGTH];
  char          filename[MAX_STRING_LENGTH];
  char          dirname[MAX_INPUT_LENGTH];
  char          arg[MAX_INPUT_LENGTH];
  char        * pString[MAX_FILE];
  char        * temp;
  int           count = 0;
  int           iLine;
  int           max_len = 0;
  int           loop;
  int           rc;
  int           flags;
  bool          fLong = FALSE;

  PUSH_FUNCTION( "do_ls" );

  if ( IS_NPC( ch ) || !ch->desc ) RETURN_NULL();

  if ( str_len( home_dir ) + str_len( ch->desc->path ) > sizeof( arg ) - 3 )
  {
    mudlog( LOG_DEBUG, "do_ls: ���|�Ӫ�." );
    send_to_char( "���|���׶W�L�t�Τ��w�ȡC\n\r", ch );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg );
  loop     = 0;
  flags    = 0;

  while ( arg[0] == '-' )
  {
    loop++;
    switch( UPPER( arg[loop] ) )
    {
    case '\x0':
      send_to_char( "�Ьd�� ls �Ϊk�T\n\r", ch );
      RETURN_NULL();

    default:
      print_to_char( ch, "���~���Ѽ� %c�M�Ьd�� ls �Ϊk�T\n\r", arg[loop] );
      RETURN_NULL();

    #ifdef __linux__
    case 'A':
      flags |= GLOB_PERIOD;
      break;
    #endif

    case 'L':
      fLong = TRUE;
      break;

    case 'N':
      flags |= GLOB_NOSORT;
      break;
    }

    if ( arg[loop + 1 ] == '\x0' )
    {
      argument = one_argument( argument, arg );
      loop     = 0;
    }
  }

  if ( !set_workpath( ch, arg, STYLE_READ ) ) RETURN_NULL();

  str_cpy( pathname, return_path() );
  str_cpy( filename, WorkPath[WorkPathPointer] );

  for ( loop = 0; loop < MAX_FILE; loop++ ) pString[loop] = NULL;

  if ( mud_lstat( pathname, filename, &pSt ) && S_ISDIR( pSt.st_mode ) )
  {
    str_cat( pathname, "/" );
    str_cat( pathname, filename );
    str_cpy( filename, "*" );
  }

  str_cpy( dirname, mud_path( pathname ) );

  if ( chdir( dirname ) != 0 )
  {
    send_to_char( "�L�k�������|�T\n\r", ch );
    chdir( CurrentDir );
    RETURN_NULL();
  }

  rc    = glob( filename, flags, NULL, &result );
  count = 0;
  chdir( CurrentDir );

  if ( rc == GLOB_NOSPACE )
  {
    send_to_char( "�O���餣���C\n\r", ch );
    RETURN_NULL();
  }

  for ( loop = 0; loop < result.gl_pathc; loop++ )
  {
    if ( !strcmp( pathname, "/" ) && !strcmp( result.gl_pathv[loop], ".." ) )
      continue;

    if ( !strcmp( result.gl_pathv[loop], PermFile ) ) continue;

    pString[count] = str_dup( result.gl_pathv[loop] );
    max_len       = UMAX( max_len, str_len( pString[count] ) );
    count++;
  }

  globfree( &result );

  if ( count <= 0 )
  {
    if ( arg[0] == '\x0' )
      send_to_char( "�o�ӥؿ��O�Ū��C\n\r", ch );

    else
      act( "$t�R�䤣��o���ɮשΥؿ��C", ch, arg, NULL, TO_CHAR );

    RETURN_NULL();
  }

  clear_buffer();
  max_len =  80 / ( iLine = UMAX( 1, 80 / ( max_len + 3 ) ) );

  tablize( FORMAT_SET, "                                        "
                       "                                        "
    , 0, NULL, 0 );

  if ( fLong ) send_to_buffer( "�`�ơR%d\n\r", count );

  for ( loop = 0; loop < count; loop++ )
  {
    if ( !mud_lstat( pathname, pString[loop], &pSt ) )
    {
      mudlog( LOG_DEBUG, "do_ls: lstat: ���~(file)." );
      continue;
    }

    temp = file_info( pathname, pString[loop], &pSt, fLong );

    if ( fLong )
    {
      send_to_buffer( "%s\n\r", temp );
    }

    else
    {
      sprintf( buf, "%d", ( ( loop % iLine ) * max_len ) + 1 );
      tablize( FORMAT_LOCATE_STRING, temp, 0, buf ,max_len );

      if ( ( ( loop + 1 ) % iLine ) == 0 || loop == count - 1 )
      {
        tablize( FORMAT_RETURN, buf, 0, NULL, MAX_STRING_LENGTH );
        str_cat( buf, "\n\r" );
        send_to_buffer( "%s", buf );

        if ( loop != count - 1 )
        {
          tablize( FORMAT_SET, "                                        "
                               "                                        "
          , 0, NULL, 0 );
        }
      }
    }
  }

  print_buffer( ch );

  for ( loop = 0; loop < MAX_FILE; loop++ )
    if ( pString[loop] ) free_string( pString[loop] );

  RETURN_NULL();
}

char * file_info( char * path, char * filename, struct stat * pSt, bool fLong )
{
  char buf[ MAX_FILE_LENGTH+ 1];
  char link[MAX_STRING_LENGTH];
  int  bytes;

  PUSH_FUNCTION( "file_info" );

  clear_stack();

  if ( fLong )
  {
    send_to_stack( "%5d %c%s %3d %8s %8s"
      , pSt->st_blocks
      , type_of_file( pSt->st_mode )
      , perm_of_file( pSt->st_mode )
      , pSt->st_nlink
      , pw_lookup( pSt->st_uid )
      , gr_lookup( pSt->st_gid ) );

    if ( !S_ISCHR( pSt->st_mode ) && !S_ISBLK( pSt->st_mode ) )
    {
      send_to_stack( "%9d ", pSt->st_size );
    }
    else
    {
      send_to_stack( "%4d.%4d "
        , major( pSt->st_rdev ), minor( pSt->st_rdev ) );
    }

    send_to_stack( "%.12s ", ctime( &pSt->st_mtime ) + 4 );

    switch( pSt->st_mode & S_IFMT )
    {
    case S_IFREG:
      send_to_stack( "\e[1;32m%s\e[0m", filename );
      break;

    case S_IFDIR:
      send_to_stack( "\e[1;34m%s\e[0m/", filename );
      break;

    case S_IFCHR:
    case S_IFBLK:
      send_to_stack( "\e[1;31m%s\e[0m", filename );
      break;

    case S_IFLNK:

      sprintf( link, "%s%s", mud_path( path ), filename );

      if ( ( bytes = readlink( link, buf, sizeof( buf ) - 1 ) ) >= 0 )
      {
        buf[bytes] = '\x0';
        send_to_stack( "\e[1;36m%s\e[0m -> %s", filename, buf );
      }

      else
      {
        send_to_stack( "\e[1;36m%s\e[0m -> �S", filename );
      }

      break;

    case S_IFIFO:
    case S_IFSOCK:
      send_to_stack( "\e[1;33m%s\e[0m", filename );
      break;
    }
  }

  else
  {
    switch( pSt->st_mode & S_IFMT )
    {
    case S_IFREG:
      send_to_stack( "\e[1;32m%s\e[0m", filename );
      break;

    case S_IFDIR:
      send_to_stack( "\e[1;34m%s\e[0m/", filename );
      break;

    case S_IFCHR:
    case S_IFBLK:
      send_to_stack( "\e[1;31m%s\e[0m", filename );
      break;

    case S_IFLNK:
      send_to_stack( "\e[1;36m%s\e[0m@", filename );
      break;

    case S_IFIFO:
    case S_IFSOCK:
      send_to_stack( "\e[1;33m%s\e[0m", filename );
      break;
    }
  }

  RETURN( return_stack() );
}

char type_of_file( mode_t mode )
{
  PUSH_FUNCTION( "type_of_file" );

  switch( mode & S_IFMT )
  {
  case S_IFREG:  RETURN( '-' );
  case S_IFDIR:  RETURN( 'd' );
  case S_IFCHR:  RETURN( 'c' );
  case S_IFBLK:  RETURN( 'b' );
  case S_IFLNK:  RETURN( 'l' );
  case S_IFIFO:  RETURN( 'p' );
  case S_IFSOCK: RETURN( 's' );
  }

  RETURN( '?' );
}

char * perm_of_file( mode_t mode )
{
  static char perm[10];

  PUSH_FUNCTION( "perm_of_file" );

  str_cpy( perm, "---------" );

  if ( mode & S_IRUSR ) perm[0] = 'r';
  if ( mode & S_IRGRP ) perm[3] = 'r';
  if ( mode & S_IROTH ) perm[6] = 'r';

  if ( mode & S_IWUSR ) perm[1] = 'w';
  if ( mode & S_IWGRP ) perm[4] = 'w';
  if ( mode & S_IWOTH ) perm[7] = 'w';

  if ( mode & S_IXUSR ) perm[2] = 'x';
  if ( mode & S_IXGRP ) perm[5] = 'x';
  if ( mode & S_IXOTH ) perm[8] = 'x';

  if ( mode & S_ISUID ) perm[2] = 's';
  if ( mode & S_ISGID ) perm[5] = 'S';
  if ( mode & S_ISVTX ) perm[8] = 't';

  RETURN( perm );
}

char * pw_lookup( int slot )
{
  static char     login[MAX_INPUT_LENGTH];
  PW_DATA       * pPw;
  struct passwd * pw;

  PUSH_FUNCTION( "pw_lookup" );

  for ( pPw = pw_list; pPw; pPw = pPw->next )
    if ( pPw->slot == slot ) RETURN( pPw->name );

  if ( ( pw = getpwuid( slot ) ) )
  {
    pPw       = alloc_struct( STRUCT_PW_DATA );
    pPw->slot = slot;
    pPw->name = str_dup( pw->pw_name );
    pPw->next = pw_list;
    pw_list   = pPw;

    RETURN( pPw->name );
  }

  sprintf( login, "%d", slot );
  RETURN( login );
}

char * gr_lookup( int slot )
{
  static char    group[MAX_INPUT_LENGTH];
  GR_DATA      * pGr;
  struct group * gr;

  PUSH_FUNCTION( "gr_lookup" );

  for ( pGr = gr_list; pGr; pGr = pGr->next )
    if ( pGr->slot == slot ) RETURN( pGr->name );

  if ( ( gr = getgrgid( slot ) ) )
  {
    pGr       = alloc_struct( STRUCT_GR_DATA );
    pGr->slot = slot;
    pGr->name = str_dup( gr->gr_name );
    pGr->next = gr_list;
    gr_list   = pGr;

    RETURN( pGr->name );
  }

  sprintf( group, "%d", slot );
  RETURN( group );
}

char * smash_path( char * path )
{
  static char smash[MAX_INPUT_LENGTH];
  int         len;

  PUSH_FUNCTION( "smash_path" );

  for ( len = str_len( str_cpy( smash, path ) ); len > 0; len-- )
  {
    if ( smash[len-1] != '/' ) break;
    smash[len-1] = '\x0';
  }

  RETURN( smash );
}

struct stat * mud_lstat( char * pathname, char * filename, struct stat * pSt )
{
  char buf[MAX_FILE_LENGTH];
  char home[MAX_FILE_LENGTH];
  char directory[MAX_FILE_LENGTH];

  PUSH_FUNCTION( "mud_lstat" );

  if ( !pathname || !filename ) RETURN( NULL );

  str_cpy( home, home_dir );
  smash_path( home );

  if ( *pathname )
  {
    str_cpy( directory, pathname );
    smash_path( directory );
    sprintf( buf, "%s/%s/%s", home, directory, filename );
  }

  else
  {
    sprintf( buf, "%s/%s", home, filename );
  }

  if ( lstat( buf, pSt ) == 0 ) RETURN( pSt );
  RETURN( NULL );
}

char * mud_path( const char * pathname )
{
  static char buf[MAX_FILE_LENGTH];
  char        home[MAX_FILE_LENGTH];
  char        directory[MAX_FILE_LENGTH];

  PUSH_FUNCTION( "mud_path" );

  str_cpy( home, home_dir );
  smash_path( home );

  if ( pathname && *pathname )
  {
    str_cpy( directory, pathname );
    smash_path( directory );
    sprintf( buf, "%s/%s/", home_dir, directory );
  }

  else
  {
    sprintf( buf, "%s/", home_dir );
  }

  RETURN( buf );
}

bool can_access( CHAR_DATA * ch, char * path, int style )
{
  struct stat   pSt;
  void        * region;
  char        * pString;
  char        * aString;
  char          filename[MAX_STRING_LENGTH];
  char          text[MAX_STRING_LENGTH];
  int           mode;
  int           fd;
  int           loop;
  int           count;
  int           level;

  PUSH_FUNCTION( "can_access" );

  if ( !mud_lstat( path, PermFile, &pSt ) )
  {
    switch( style )
    {
    default:
    case STYLE_READ:
      RETURN( TRUE );

    case STYLE_WRITE:
      RETURN( FALSE );
    }
  }

  sprintf( filename, "%s/%s/%s", home_dir, path, PermFile );

  if ( ( fd = open( filename, O_RDONLY ) ) < 0 )
  {
    switch( style )
    {
    default:
    case STYLE_READ:
      RETURN( TRUE );

    case STYLE_WRITE:
      RETURN( FALSE );
    }
  }

  region = mmap( NULL, pSt.st_size, PROT_READ, MAP_SHARED, fd, 0 );

  if ( region == ( caddr_t ) -1 )
  {
    close( fd );

    switch( style )
    {
    default:
    case STYLE_READ:
      RETURN( TRUE );

    case STYLE_WRITE:
      RETURN( FALSE );
    }
  }

  pString = region;
  for ( count = loop = 0; loop < pSt.st_size; loop++, pString++ )
  {
    switch( *pString )
    {
    default:

      if ( count <= sizeof( text ) - 10 ) text[count++] = *pString;
      if ( loop < pSt.st_size - 1 ) break;

    case '\n':

      text[count] = '\x0';

      if ( count > 0 )
      {
        switch( UPPER( text[0] ) )
        {
        default:
        case '*':
          count = 0;
          continue;

        case 'A':
          if ( style == STYLE_READ )
          {
            close( fd );
            munmap( region, pSt.st_size );
            RETURN( FALSE );
          }

          count = 0;
          continue;

        case 'B':
          if ( style == STYLE_WRITE )
          {
            close( fd );
            munmap( region, pSt.st_size );
            RETURN( FALSE );
          }

          count = 0;
          continue;

        case 'R':
          mode = STYLE_READ;
          break;

        case 'W':
          mode = STYLE_WRITE;
          break;
        }

        if ( style != mode )
        {
          count = 0;
          continue;
        }

        for ( aString = text + 1; *aString == ' '; aString++ );

        if ( *aString == '\x0' )
        {
          count = 0;
          continue;
        }

        else if ( is_number( aString ) )
        {
          level = atoi( aString );
          if ( get_trust( ch ) >= level )
          {
            close( fd );
            munmap( region, pSt.st_size );
            RETURN( TRUE );
          }
        }

        else
        {
          if ( !str_cmp( ch->name, aString ) )
          {
            close( fd );
            munmap( region, pSt.st_size );
            RETURN( TRUE );
          }
        }
      }

      count = 0;
      break;

    case '\r':
      break;

    case '\x0':
    case '\xFF':

      close( fd );
      munmap( region, pSt.st_size );

      switch( style )
      {
      default:
      case STYLE_READ:
        RETURN( TRUE );

      case STYLE_WRITE:
        RETURN( FALSE );
      }
    }
  }

  close( fd );
  munmap( region, pSt.st_size );
  RETURN( FALSE );
}

void scan_directory( CHAR_DATA * ch, char * path, char * str )
{
  struct stat     pSt;
  DIR           * pDir;
  struct dirent * next;
  glob_t          result;
  char            pathname[MAX_STRING_LENGTH];
  char            filename[MAX_STRING_LENGTH];
  int             rc;
  int             loop;

  PUSH_FUNCTION( "scan_directory" );

  str_cpy( pathname, mud_path( path ) );

  if ( chdir( pathname ) != 0 )
  {
    chdir( CurrentDir );
    RETURN_NULL();
  }

  rc = glob( str, 0, NULL, &result );
  chdir( CurrentDir );

  if ( rc == GLOB_NOSPACE ) RETURN_NULL();

  for ( loop = 0; loop < result.gl_pathc; loop++ )
  {
    if ( !strcmp( result.gl_pathv[loop], "." )
      || !strcmp( result.gl_pathv[loop], ".." )
      || !strcmp( result.gl_pathv[loop], PermFile )
      || !mud_lstat( path, result.gl_pathv[loop], &pSt )
      || !S_ISREG( pSt.st_mode ) )
      continue;

    sprintf( filename, "%s%s/%s"
      , ( path && *path == '/' ) ? "" : "/"
      , path, result.gl_pathv[loop] );

    send_to_buffer( "%s�R%s\n\r", str, filename );
  }

  globfree( &result );

  if ( ( pDir = opendir( pathname ) ) )
  {
    while ( ( next = readdir( pDir ) ) )
    {
      if ( !strcmp( next->d_name, "." )
        || !strcmp( next->d_name, ".." )
        || !strcmp( next->d_name, PermFile ) )
        continue;

      if ( mud_lstat( path, next->d_name, &pSt ) && S_ISDIR( pSt.st_mode ) )
      {
        str_cpy( filename, smash_path( path ) );
        str_cat( filename, "/" );
        str_cat( filename, next->d_name );

        if ( can_access( ch, filename, STYLE_READ ) )
          scan_directory( ch, filename, str );
      }
    }

    closedir( pDir );
  }

  RETURN_NULL();
}

void print_no_access( CHAR_DATA * ch, int mode )
{
  PUSH_FUNCTION( "print_no_access" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "print_no_access: �ʥF�ӷ�." );
    RETURN_NULL();
  }

  switch( mode )
  {
  default:
    send_to_char( "�o�ӥؿ��A�S���v�Q�g�J��Ū���C\n\r", ch );
    break;

  case STYLE_READ:
    send_to_char( "�o�ӥؿ��A�S���v�QŪ���C\n\r", ch );
    break;

  case STYLE_WRITE:
    send_to_char( "�o�ӥؿ��A�S���v�Q�g�J�C\n\r", ch );
    break;
  }

  RETURN_NULL();
}

int glob_file( const char * path, const char * ext, glob_t * result )
{
  char buf[MAX_FILE_LENGTH];
  int  rc;

  PUSH_FUNCTION( "glob_file" );

  if ( chdir( path ) != 0 ) RETURN( -1 );

  if ( ext && *ext ) sprintf( buf, "*.%s", ext );
  else               strcpy( buf, "*" );

  rc = glob( buf, GLOB_NOSORT, NULL, result );
  chdir( CurrentDir );

  if ( rc == GLOB_NOSPACE )
  {
    mudlog( LOG_DEBUG, "glob_file: �O���餣���C" );
    RETURN( -1 );
  }

  RETURN( result->gl_pathc );
}

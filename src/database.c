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
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include "merc.h"

void            update_database         args( ( DATABASE_DATA * ) );
bool            fread_a_line            args( ( FILE *, char * ) );

void set_database( CHAR_DATA * ch )
{
  DATABASE_DATA * pData;
  char          * pString;
  int             iHash;

  PUSH_FUNCTION( "set_database" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "set_database: 缺乏來源." );
    RETURN_NULL();
  }

  if ( IS_NPC( ch ) ) RETURN_NULL();

  iHash = ch->serial.low % MAX_KEY_HASH;
  for ( pData = data_index_hash[iHash]; pData; pData = pData->hash )
  {
    if ( pData->serial.low  == ch->serial.low
      && pData->serial.high == ch->serial.high ) RETURN_NULL();
  }

  set_database_default( pData = alloc_struct( STRUCT_DATABASE_DATA ) );

  pData->name        = str_dup( ch->name );
  pData->serial.high = ch->serial.high;
  pData->serial.low  = ch->serial.low;
  pData->exist       = TRUE;

  for ( pString = pData->name; *pString; pString++ )
    if ( *pString >= 'A' && *pString <= 'Z' ) *pString += 'a' - 'A';

  if ( !database_list ) database_list       = pData;
  if ( database_last  ) database_last->next = pData;

  database_last = pData;
  pData->next   = NULL;

  iHash                  = ch->serial.low % MAX_KEY_HASH;
  pData->hash            = data_index_hash[iHash];
  data_index_hash[iHash] = pData;

  update_database( pData );

  RETURN_NULL();
}

void delete_database( CHAR_DATA * ch )
{
  DATABASE_DATA * pData;
  int             iHash;

  PUSH_FUNCTION( "delete_database" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "delete_database: 缺乏來源." );
    RETURN_NULL();
  }

  if ( IS_NPC( ch ) ) RETURN_NULL();

  iHash = ch->serial.low % MAX_KEY_HASH;

  for ( pData = data_index_hash[iHash]; pData; pData = pData->hash )
  {
    if ( pData->serial.low  == ch->serial.low
      && pData->serial.high == ch->serial.high )
    {
      pData->exist = FALSE;
      update_database( pData );
      RETURN_NULL();
    }
  }

  mudlog( LOG_DEBUG, "delete_database: 找不到目標 %s.", ch->name );
  RETURN_NULL();
}

void erase_database( const char * name )
{
  DATABASE_DATA * pData;

  PUSH_FUNCTION( "erase_database" );

  if ( !name || !*name )
  {
    mudlog( LOG_DEBUG, "erase_database: 缺乏來源." );
    RETURN_NULL();
  }

  for ( pData = database_list; pData; pData = pData->next )
  {
    if ( !str_cmp( pData->name, name ) && pData->exist == TRUE )
    {
      pData->exist = FALSE;
      update_database( pData );
      RETURN_NULL();
    }
  }

  mudlog( LOG_DEBUG, "delete_database: 找不到目標 %s.", name );
  RETURN_NULL();
}

void update_database( DATABASE_DATA * pData )
{
  FILE * pFile;

  PUSH_FUNCTION( "update_database" );

  if ( !( pFile = FOPEN( database_file, "a" ) ) )
  {
    mudlog( LOG_DEBUG, "update_database: 無法開啟檔案 %s.", database_file );
    RETURN_NULL();
  }

  fprintf( pFile, "Player %-12s %9d %9d %d\n"
    , pData->name, pData->serial.high, pData->serial.low, pData->exist );

  FCLOSE( pFile );

  RETURN_NULL();
}

DATABASE_DATA * database_lookup( int high, int low )
{
  DATABASE_DATA * pData;
  int             iHash;

  PUSH_FUNCTION( "database_lookup" );

  if ( low < 0 || high < 0 ) RETURN( NULL );

  iHash = low % MAX_KEY_HASH;

  for ( pData = data_index_hash[iHash]; pData; pData = pData->hash )
  {
    if ( pData->serial.high == high && pData->serial.low == low )
      RETURN( pData );
  }

  RETURN( NULL );
}

size_t database_count( int type )
{
  size_t          count = 0;
  DATABASE_DATA * pData;

  PUSH_FUNCTION( "database_count" );

  switch( type )
  {
  default:
    mudlog( LOG_DEBUG, "database_count: 錯誤的選項 %d.", type );
    RETURN( 0 );

  case DATABASE_ALL:

    for ( pData = database_list; pData; pData = pData->next ) count++;
    RETURN( count );

  case DATABASE_EXIST:

    for ( pData = database_list; pData; pData = pData->next )
      if ( pData->exist ) count++;

    RETURN( count );

  case DATABASE_NO_EXIST:

    for ( pData = database_list; pData; pData = pData->next )
      if ( !pData->exist ) count++;

    RETURN( count );
  }

  RETURN( count );
}

BASIC_DATA * load_basic( const char * name )
{
  BASIC_DATA * pBasic;
  FILE       * pFile;
  char       * pString;
  bool         fMatch;
  char         filename[MAX_FILE_LENGTH];
  char         buf[MAX_STRING_LENGTH];
  char         word[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "load_basic" );

  if ( !name || !*name )
  {
    mudlog( LOG_DEBUG, "load_basic: 缺乏來源." );
    RETURN( NULL );
  }

  str_cpy( filename, file_name( name, BASIC_FILE ) );
  if ( access( filename, R_OK ) != 0 ) RETURN( NULL );

  if ( !( pFile = FOPEN( filename, "r" ) ) )
  {
    mudlog( LOG_DEBUG, "load_basic: 無法開啟檔案 %s.", filename );
    RETURN( NULL );
  }

  pBasic = alloc_struct( STRUCT_BASIC_DATA );

  pBasic->name        = str_dup( "" );
  pBasic->cname       = str_dup( "" );
  pBasic->address     = str_dup( "" );
  pBasic->email       = str_dup( "" );
  pBasic->club        = str_dup( "" );
  pBasic->class       = str_dup( "" );
  pBasic->serial.high = 0;
  pBasic->serial.low  = 0;
  pBasic->level       = 1;
  pBasic->played      = 0;
  pBasic->sex         = SEX_MALE;

  while ( !feof( pFile ) )
  {
    if ( !fread_a_line( pFile, buf ) )
    {
      mudlog( LOG_DEBUG, "load_basic: 檔案 %s 不是文字檔.", filename );
      FCLOSE( pFile );
      RETURN( NULL );
    }

    if ( buf[0] == '\x0' ) continue;

    pString = one_argument( buf, word );
    fMatch  = FALSE;

    switch( UPPER( word[0] ) )
    {
    case 'C':

      if ( !str_cmp( word, "CName" ) )
      {
        free_string( pBasic->cname );
        pBasic->cname = str_dup( pString );
        fMatch        = TRUE;
        break;
      }

      if ( !str_cmp( word, "Club" ) )
      {
        free_string( pBasic->club );
        pBasic->club = str_dup( pString );
        fMatch       = TRUE;
        break;
      }


      if ( !str_cmp( word, "Class" ) )
      {
        free_string( pBasic->class );
        pBasic->class = str_dup( pString );
        fMatch        = TRUE;
        break;
      }

      if ( !str_cmp( word, "CoupleHigh" ) )
      {
        pBasic->couple.high = atoi( pString );
        fMatch              = TRUE;
        break;
      }

      if ( !str_cmp( word, "CoupleLow" ) )
      {
        pBasic->couple.low = atoi( pString );
        fMatch             = TRUE;
        break;
      }

      break;

    case 'E':

      if ( !str_cmp( word, "End" ) )
      {
        FCLOSE( pFile );
        RETURN( pBasic );
      }

      if ( !str_cmp( word, "Email" ) )
      {
        free_string( pBasic->email );
        pBasic->email = str_dup( pString );
        fMatch        = TRUE;
        break;
      }

      break;

    case 'L':

      if ( !str_cmp( word, "Level" ) )
      {
        pBasic->level = atoi( pString );
        fMatch        = TRUE;
        break;
      }

      if ( !str_cmp( word, "LastHost" ) )
      {
        free_string( pBasic->address );
        pBasic->address = str_dup( pString );
        fMatch          = TRUE;
        break;
      }

      break;

    case 'N':

      if ( !str_cmp( word, "Name" ) )
      {
        free_string( pBasic->name );
        pBasic->name = str_dup( pString );
        fMatch       = TRUE;
        break;
      }

      break;

    case 'P':

      if ( !str_cmp( word, "Played" ) )
      {
        pBasic->played = atoi( pString );
        fMatch         = TRUE;
        break;
      }

      break;

    case 'S':

      if ( !str_cmp( word, "Sex" ) )
      {
        pBasic->sex = atoi( pString );
        fMatch      = TRUE;
        break;
      }

      if ( !str_cmp( word, "SerialHigh" ) )
      {
        pBasic->serial.high = atoi( pString );
        fMatch              = TRUE;
        break;
      }

      if ( !str_cmp( word, "SerialLow" ) )
      {
        pBasic->serial.low = atoi( pString );
        fMatch             = TRUE;
        break;
      }
      break;

    }

    if ( !fMatch )
    {
      free_basic( pBasic );

      mudlog( LOG_DEBUG, "load_basic: %s 錯誤的命令 %s.", filename, word );
      FCLOSE( pFile );
      RETURN( NULL );
    }
  }

  FCLOSE( pFile );
  RETURN( NULL );
}

bool fread_a_line( FILE * pFile, char * buffer )
{
  char Char;

  PUSH_FUNCTION( "fread_a_line" );

  *buffer = '\x0';

  /* 先把空白先給弄掉 */
  do
  {
    Char = getc( pFile );
  } while ( isspace( ( int ) Char ) && Char != EOF );

  if ( Char != EOF ) ungetc( Char , pFile );

  for ( ;; )
  {
    switch( ( Char = getc( pFile ) ) )
    {
    case '\x0':
      *buffer = '\x0';
      RETURN( FALSE );

    case EOF:
    case '\n':
    case '\r':
      *buffer = '\x0';
      RETURN( TRUE );

    case '\b':
    case '\a':
    case '\v':
    case '\f':
      break;

    case '\t':
      *buffer++ = ' ';
      break;

    default:
      *buffer++ = Char;
      break;
    }
  }

  RETURN( TRUE );
}

BASIC_DATA * serial2basic( SERIAL_DATA * pSerial )
{
  DATABASE_DATA * pData;
  int             iHash;

  PUSH_FUNCTION( "serial2basic" );

  iHash = pSerial->low % MAX_KEY_HASH;

  for ( pData = data_index_hash[iHash]; pData; pData = pData->hash )
  {
    if ( pData->serial.high == pSerial->high
      && pData->serial.low  == pSerial->low
      && pData->exist )
    {
      RETURN( load_basic( pData->name ) );
    }
  }

  RETURN( NULL );
}

char * fullname( SERIAL_DATA * pSerial )
{
  static char   buf[MAX_STRING_LENGTH];
  BASIC_DATA  * pBasic;

  PUSH_FUNCTION( "fullname" );

  if ( ( pBasic = serial2basic( pSerial ) ) )
  {
    sprintf( buf, "%s\e[0m(%s)", pBasic->cname, pBasic->name );
    free_basic( pBasic );
    RETURN( buf );
  }

  RETURN( "資料庫未有此人" );
}

DATABASE_DATA * name2database( const char * name )
{
  DATABASE_DATA * pData;
  DATABASE_DATA * zData;

  PUSH_FUNCTION( "name2database" );

  if ( !name || !*name )
  {
    mudlog( LOG_DEBUG, "name2serial: 缺乏來源." );
    RETURN( NULL );
  }

  for ( zData = NULL, pData = database_list; pData; pData = pData->next )
  {
    if ( !str_cmp( pData->name, name ) )
    {
      if ( pData->exist ) RETURN( pData );
      zData = pData;
    }
  }

  RETURN( zData );
}

BASIC_DATA * name2basic( const char * name )
{
  PUSH_FUNCTION( "name2basic" );

  if ( !name || !*name ) RETURN( NULL );
  RETURN( load_basic( name ) );
}

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
#include "merc.h"

#define         RETURN_TRUE     1
#define         RETURN_FALSE    2
#define         RETURN_ERROR    3

char *  extract_script  args( ( char *, char *, char *, char * ) );
int     test_argument   args( ( const char *, CHAR_DATA * ) );
char *  find_endif      args( ( const char *, bool ) );
char *  find_else       args( ( const char * ) );

int     mob_vnum;

int test_argument( const char * str, CHAR_DATA * ch )
{
  PUSH_FUNCTION( "test_argument" );

  for ( ; *str; str++ )
  {
    if ( *str == '0' ) RETURN( RETURN_FALSE );
    if ( *str == '1' ) RETURN( RETURN_TRUE  );
  }

  RETURN( 0 );
}

char * find_endif( const char * str, bool bElse )
{
  int    depth;
  int    count;
  char   cLine[MAX_INPUT_LENGTH];
  char   word[MAX_INPUT_LENGTH];
  char   argument[MAX_INPUT_LENGTH];
  char * next_point;

  PUSH_FUNCTION( "find_endif" );

  count = bElse ? 0 : -1;

  for ( depth = 0; *str; )
  {
    next_point = extract_script( ( char * ) str, cLine, word, argument );

    if ( word[0] == '\x0' )
    {
      str = next_point;
      continue;
    }

    else if ( !str_cmp( word, "if" ) )
    {
      count = depth++;
    }

    else if ( !str_cmp( word, "else" ) )
    {
      if ( ++count > depth )
      {
        for ( ; *str; str++ );
        mudlog( LOG_DEBUG, "find_endif: %d too more else.", mob_vnum );
        RETURN( ( char * ) str );
      }
    }

    else if ( !str_cmp( word, "endif" ) )
    {
      if ( --depth < 0 ) RETURN( next_point );
      count = depth -1;
    }

    str = next_point;
  }

  if ( depth > 0 )
    mudlog( LOG_DEBUG, "%d if and endif or else not match.", mob_vnum );

  RETURN( ( char * ) str );
}

char * find_else( const char * str )
{
  int    depth;
  int    count;
  char   cLine[MAX_INPUT_LENGTH];
  char   word[MAX_INPUT_LENGTH];
  char   argument[MAX_INPUT_LENGTH];
  char * next_point;

  PUSH_FUNCTION( "find_else" );

  for ( count = -1, depth = 0; *str; )
  {
    next_point = extract_script( ( char * ) str, cLine, word, argument );

    if ( word[0] == '\x0' )
    {
      str = next_point;
      continue;
    }

    else if ( !str_cmp( word, "if" ) )
    {
      count = depth++;
    }

    else if ( !str_cmp( word, "else" ) )
    {
      if ( ++count > depth )
      {
        mudlog( LOG_DEBUG, "find_else: %d too more else. %d %d %s"
          , mob_vnum, depth , count, str );

        for ( ; *str; str++ );
        RETURN( ( char * ) str );
      }

      if ( depth <= 0 ) RETURN( next_point );
    }

    else if ( !str_cmp( word, "endif" ) )
    {
      if ( --depth < 0 ) RETURN( next_point );
      count = depth - 1;
    }

    str = next_point;
  }

  if ( depth > 0 )
    mudlog( LOG_DEBUG, "%d if and endif or else not match.", mob_vnum );

  RETURN( ( char * ) str );
}

char * extract_script( char * str, char * line, char * com, char * arg )
{
  char   * target;
  bool     bArg;
  size_t   len;

  PUSH_FUNCTION( "extract_script" );

  while ( isspace( ( int ) *str ) ) str++;
  *line = '\x0';
  *com  = '\x0';
  *arg  = '\x0';

  target = com;
  bArg   = FALSE;
  len    = 0;

  for ( ; ; str++ )
  {
    if ( len > MAX_INPUT_LENGTH - 2 )
    {
      mudlog( LOG_DEBUG, "extract_script: 字串太長." );
      RETURN( str );
    }

    switch( *str )
    {
    case '\r':
      break;

    case '\n':
      *line   = '\x0';
      *target = '\x0';

      while ( isspace( ( int ) *str ) ) str++;

      for ( target = arg + str_len( arg ) - 1; target >= arg; target-- )
      {
        if ( *target == ' ' ) *target = '\x0';
        else break;
      }

      RETURN( str );

    case '\x0':
      *line   = '\x0';
      *target = '\x0';

      for ( target = arg + str_len( arg ) - 1; target >= arg; target-- )
      {
        if ( *target == ' ' ) *target = '\x0';
        else break;
      }

      RETURN( str );

    case ' ':
    case '\t':

      if ( bArg == FALSE )
      {
        bArg    = TRUE;
        *target = '\x0';
        target  = arg;
      }
      else if ( target != arg )
      {
        *target++ = ' ';
      }

      *line++ = ' ';
      len++;
      break;

    default:
      *line++   = *str;
      *target++ = *str;
      len++;
      break;
    }
  }

  RETURN( str );
}

char * exec_script( char * str, CHAR_DATA * ch, int * depth )
{
  char   cLine[MAX_INPUT_LENGTH];
  char   word[MAX_INPUT_LENGTH];
  char   argument[MAX_INPUT_LENGTH];
  char * next_point;

  PUSH_FUNCTION( "exec_script" );

  if ( !str || !ch )
  {
    mudlog( LOG_DEBUG, "exec_script: 來源不正確." );
    *depth = 0;
    RETURN( NULL );
  }

  mob_vnum = ch->pIndexData ? ch->pIndexData->vnum : -1;

  while ( *str )
  {
    next_point = extract_script( str, cLine, word, argument );

    if ( word[0] == '\x0' )
    {
      str = next_point;
      continue;
    }

    mudlog( LOG_INFO, "%s", cLine );

    if ( !str_cmp( word, "if" ) )
    {
      switch( test_argument( argument, ch ) )
      {
      case RETURN_ERROR:
        for ( ; *str; str++ );
        *depth = 0;
        RETURN( str );

      case RETURN_TRUE:

        ( *depth )++;
        next_point = exec_script( next_point, ch, depth );
        break;

      case RETURN_FALSE:

        ( *depth )++;
        next_point = find_else( next_point );
        next_point = exec_script( next_point, ch, depth );
        break;
      }
    }

    else if ( !str_cmp( word, "exit" ) )
    {
      if ( argument[0] != '\x0' )
        mudlog( LOG_DEBUG, "exec_script: %d error exit.", mob_vnum );

      for ( ; *str; str++ );
      *depth = 0;
      RETURN( str );
    }

    else if ( !str_cmp( word, "break" ) )
    {
      if ( *depth <= 0 )
      {
        mudlog( LOG_DEBUG, "exec_script: %d error break.", mob_vnum );
        for ( ; *str; str++ );
        *depth = 0;
        RETURN( str );
      }

      if ( argument[0] != '\x0' )
      {
        mudlog( LOG_DEBUG, "exec_script: %d error break.", mob_vnum );
        for ( ; *str; str++ );
        *depth = 0;
        RETURN( str );
      }

      next_point = find_endif( next_point, FALSE );
      ( *depth )--;
      RETURN( next_point );
    }

    else if ( !str_cmp( word, "endif" ) )
    {
      if ( *depth <= 0 )
      {
        mudlog( LOG_DEBUG, "exec_script: %d too more endif.", mob_vnum );
        for ( ; *str; str++ );
        *depth = 0;
        RETURN( str );
      }

      if ( argument[0] != '\x0' )
      {
        mudlog( LOG_DEBUG, "exec_script: %d error endif.", mob_vnum );
        for ( ; *str; str++ );
        *depth = 0;
        RETURN( str );
      }

      ( *depth )--;
      RETURN( next_point );
    }

    else if ( !str_cmp( word, "else" ) )
    {
      if ( *depth <= 0 )
      {
        mudlog( LOG_DEBUG, "exec_script: %d error else.", mob_vnum );
        for ( ; *str; str++ );
        *depth = 0;
        RETURN( str );
      }

      if ( argument[0] != '\x0' )
      {
        mudlog( LOG_DEBUG, "exec_script: %d error else.", mob_vnum );
        for ( ; *str; str++ );
        *depth = 0;
        RETURN( str );
      }

      next_point = find_endif( next_point, TRUE );
      ( *depth )--;
      RETURN( next_point );
    }

    else
    {
      print_to_char( ch, "%s\n\r", cLine );
    }

    str = next_point;
  }

  if ( *depth < 0 )
    mudlog( LOG_DEBUG, "exec_script: %d error if, endif absent.", mob_vnum );

  RETURN( str );
}

/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

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
#include <unistd.h>
#include "merc.h"

#define MAX_DIGITAL     5

/* 檢查某字串裡是否有包含某個字串 */
bool is_name( const char * str, char * namelist )
{
  char name[ MAX_INPUT_LENGTH ];

  PUSH_FUNCTION( "is_name" );

  for ( ;; )
  {
    namelist = one_argument( namelist, name );
    if ( !name[0]                 ) RETURN( FALSE );
    if ( !str_prefix( str, name ) ) RETURN( TRUE  );
  }
}

bool is_fullname( const char * str, char * namelist )
{
  char name[MAX_INPUT_LENGTH];

  PUSH_FUNCTION( "is_fullname" );

  for ( ;; )
  {
    namelist = one_argument( namelist, name );
    if ( !name[0] )              RETURN( FALSE );
    if ( !str_cmp( str, name ) ) RETURN( TRUE );
  }
}

FUNCTION( do_test )
{
  FILE            * pFile;
  MOB_INDEX_DATA  * pMobIndex;
  extern int        top_mob_index;
  int               i[11];
  int               j[11];
  int               vnum;
  int               loop;
  int               nMatch;
  int               count;
  char            * color;
  char            * amount;
  bool              found;

  PUSH_FUNCTION( "do_test" );

  {
    print_to_char( ch, "%s\n\r", symbol_string( 1, argument ) );
  }

  RETURN_NULL();

  {
    MOB_INDEX_DATA * pIndex;
    int              vnum;
    char           * pString;
    int              temp;

    vnum = atoi( argument );
    if ( !( pIndex = get_mob_index( vnum ) ) )
    {
      send_to_char( "No such vnum.\n\r", ch );
      RETURN_NULL();
    }

    pString = pIndex->temp;
    if ( !pString || !*pString )
    {
      send_to_char( "No program.\n\r", ch );
      RETURN_NULL();
    }

    temp = 0;
    /* exec_script( pString, ch, &temp ); */
  }

  RETURN_NULL();

  if ( !( pFile = fopen( "data", "w" ) ) )
  {
    send_to_char( "無法開啟檔案。\n\r", ch );
    RETURN_NULL();
  }

  for ( loop = 0; loop < 10; loop++ ) i[loop] = j[loop] = 0;

  for ( loop = 1; loop <= 100; loop++ )
  {
    count = 0;
    for ( found = FALSE, nMatch = vnum = 0; nMatch < top_mob_index; vnum++ )
    {
      if ( ( pMobIndex = get_mob_index( vnum ) ) )
      {
        nMatch++;
        if ( pMobIndex->level == loop && pMobIndex->count > 0 )
        {
               if ( pMobIndex->count > 10 ) color = "\e[1;31m";
          else if ( pMobIndex->count > 5  ) color = "\e[1;34m";
          else if ( pMobIndex->count >= 3 ) color = "\e[1;32m";
          else                              color = "";

               if ( pMobIndex->count >  10 ) amount = "極多";
          else if ( pMobIndex->count >  5  ) amount = "很多";
          else if ( pMobIndex->count >= 3  ) amount = "多  ";
          else                               amount = "普通";

          found  = TRUE;
          count += pMobIndex->count;
          i[loop / 10 ] += pMobIndex->count;
          j[loop / 10 ] ++;

          fprintf( pFile, "%s等級﹕%3d 數量﹕%s 區域﹕%10s 名稱﹕%s%s\n"
            , color
            , pMobIndex->level
            , amount
            , pMobIndex->area->name
            , pMobIndex->short_descr
          , *color ? "\e[0m" : "" );
        }
      }
    }

    /*
    if ( !found )
      fprintf( pFile, "\e[1;37m等級 %d 沒有怪物。\e[0m\n", loop );

    else
      fprintf( pFile, "等級 \e[1;35m%d\e[0m 總數目為 \e[1;36m%d\e[0m。\n", loop, count );

    */

    fprintf( pFile, "\n" );
  }

  for ( loop = 0; loop < 10; loop++ )
  {
    fprintf( pFile, "等級﹕%3d-%3d  總隻數﹕ %4d  種類﹕ %4d\n"
      , loop * 10, ( loop + 1 ) * 10 - 1, i[loop], j[loop] );
  }

  fclose( pFile );
  RETURN_NULL();
}

/* 測試字串是否是數字 */
bool is_number( char * argument )
{
  PUSH_FUNCTION( "is_number" );
  if ( !argument || !*argument ) RETURN( FALSE );
  if ( *argument == '+' || *argument == '-' ) argument++;
  while ( *argument ) if ( !isdigit( ( int ) *argument++ ) ) RETURN( FALSE );
  RETURN( TRUE );
}

char * test_number( char * argument , int * number )
{
  char buf[ MAX_DIGITAL ];
  int  loop;
  int  factor;

  PUSH_FUNCTION( "test_number" );

  /* 清除陣列 */
  for ( loop = 0; loop < MAX_DIGITAL; loop++ ) buf[loop] = '\x0';
  *number = 0;

  for ( *number = -1, loop = 0; *argument != ' '; loop++, argument++ )
  {
    if ( !isdigit( ( int ) *argument ) || loop >= MAX_DIGITAL ) RETURN( NULL );
    buf[loop] = *argument - '0';
  }

  /* 若都沒有則回傳 -1 */
  if ( loop == 0 ) RETURN( NULL );

  factor = 1;
  while ( loop-- > 1 ) factor *= 10;

  for ( *number = 0, loop = 0; factor > 0; loop++ , factor /= 10 )
    *number += buf[loop] * factor;

  while ( *argument == ' ' ) argument++;
  RETURN( argument );
}

/* 計算字串長度 */
size_t str_len( const char * argument )
{
  register size_t len;

  PUSH_FUNCTION( "str_len" );

  for ( len = 0; *argument; argument++ , len++ );

  RETURN( len );
}

/* 計算字串長度﹐但不包含 ansi code */
size_t ansi_str_len( char * pString )
{
  size_t len;
  char   Char;

  PUSH_FUNCTION( "ansi_str_len" );

  for ( len = 0; *pString; )
  {
    pString = ansi_code( pString, &Char, NULL );
    if ( Char ) len++;
  }

  RETURN( len );
}

/* 把兩個字串連接起來 */
char * str_cat( char * dest , const char * source )
{
  char * buf;

  PUSH_FUNCTION( "str_cat" );

  /* 先到字串末端 */
  for ( buf = dest; *dest; dest++ );

  /* 連接字串 */
  for ( ; *source; *dest++ = *source++ );

  /* 把字串末端加上空字串 */
  *dest = '\x0';

  RETURN( buf );
}

/* 拷貝字串 */
char * str_cpy( char * dest , const char * source )
{
  char * buf;
  PUSH_FUNCTION( "str_cpy" );
  for ( buf = dest; *source; *dest++ = *source++ );
  *dest = '\x0';
  RETURN( buf );
}

/* 取某字串的前 n 個字元 */
char * str_ncpy( char * dest, const char * source, size_t len )
{
  char * buf;

  PUSH_FUNCTION( "str_ncpy" );

  for ( buf = dest; *source && len-- > 0; *dest++ = *source++ );
  *dest = '\x0';

  RETURN( buf );
}

char * str_str( char * haystack, char * needle )
{
  char * aString;
  char * bString;
  int    alen;
  int    blen;

  PUSH_FUNCTION( "str_str" );

  if ( !haystack
    || !needle
    || ( alen = str_len( haystack ) ) <= 0
    || ( blen = str_len( needle   ) ) <= 0 ) RETURN( NULL );

  while ( *haystack && alen >= blen )
  {
    for ( aString = haystack, bString = needle; ; aString++, bString++ )
    {
      if ( *bString == '\x0' ) RETURN( haystack );
      if ( UPPER( *aString ) != UPPER( *bString ) ) break;
    }

    haystack++;
    alen--;
  }

  RETURN( NULL );
}

bool str_scmp( const char * astr, const char * bstr, bool * value )
{
  PUSH_FUNCTION( "str_scmp" );

  if ( !astr || !bstr )
  {
    mudlog( LOG_DEBUG , "str_scmp: 空的來源字串." );
    RETURN( TRUE );
  }

  for ( ; *astr || *bstr; astr++, bstr++ )
    if ( tolower( *astr ) != tolower( *bstr ) ) RETURN( TRUE );

  *value = TRUE;
  RETURN( FALSE );
}

bool str_cmp( const char * astr, const char * bstr )
{
  PUSH_FUNCTION( "str_cmp" );

  if ( !astr || !bstr )
  {
    mudlog( LOG_DEBUG , "str_cmp: 空的來源字串." );
    RETURN( TRUE );
  }

  for ( ; *astr || *bstr; astr++, bstr++ )
    if ( tolower( *astr ) != tolower( *bstr ) ) RETURN( TRUE );

  RETURN( FALSE );
}

bool str_prefix( const char * astr, const char * bstr )
{
  PUSH_FUNCTION( "str_prefix" );

  if ( !astr )
  {
    mudlog( LOG_DEBUG , "str_prefix: 空的來源字串." );
    RETURN( TRUE );
  }

  if ( !bstr )
  {
    mudlog( LOG_DEBUG , "str_prefix: 空的目的字串." );
    RETURN( TRUE );
  }

  for ( ; *astr; astr++, bstr++ )
    if ( tolower( *astr ) != tolower( *bstr ) ) RETURN( TRUE );

  RETURN( FALSE );
}

bool str_infix( const char * astr, const char * bstr )
{
  int  sstr1;
  int  sstr2;
  int  ichar;
  char c0;

  PUSH_FUNCTION( "str_infix" );

  if ( !( c0 = tolower( astr[0] ) ) ) RETURN( FALSE );

  sstr1 = str_len( astr );
  sstr2 = str_len( bstr );

  for ( ichar = 0; ichar <= sstr2 - sstr1; ichar++ )
  {
    if ( c0 == tolower(bstr[ichar] ) && !str_prefix( astr, bstr + ichar ) )
      RETURN( FALSE );
  }

  RETURN( TRUE );
}

bool str_suffix( const char * astr, const char * bstr )
{
  int sstr1;
  int sstr2;

  PUSH_FUNCTION( "str_suffix" );

  sstr1 = str_len(astr);
  sstr2 = str_len(bstr);

  if ( sstr1 <= sstr2 && !str_cmp( astr, bstr+sstr2-sstr1 ) ) RETURN( FALSE );
  RETURN( TRUE );
}

char * normalize( const char * str )
{
  static char strcap[ MAX_STRING_LENGTH];
  int         loop;

  PUSH_FUNCTION( "normalize" );

  for ( loop = 0; str[loop]; strcap[loop] = tolower( str[loop] ) , loop++ );

  strcap[loop] = '\x0';

  RETURN( strcap );

}

void smash_char( char * string, char Char )
{
  int len;

  PUSH_FUNCTION( "smash_char" );

  for ( len = str_len( string ); len > 0; len-- )
  {
    if ( *( string + len - 1 ) != Char ) break;
    *(string -1 + len ) = '\x0';
  }

  RETURN_NULL()
}

char * str_space( char * title , size_t length )
{
  int         loop;
  static char buffer[ MAX_INPUT_LENGTH ];

  PUSH_FUNCTION( "str_space" );

  length = UMIN( length , MAX_INPUT_LENGTH );
  for ( loop = 0; loop < length - 1; loop++ )
  {
    if ( *title ) buffer[loop] = *title++;
    else          buffer[loop] = ' ';
  }

  buffer[loop] = '\x0';

  RETURN( buffer );
}

char * chinese_number( int i, char * pString )
{
  long long mod;
  static char * c_dig[] =
   { "零", "十", "百", "千", "萬", "億", "兆" };

  static char * c_num[] =
    { "零", "一", "二", "三", "四", "五", "六", "七", "八", "九", "十" };

  PUSH_FUNCTION( "chinese_number" );

  *pString = '\x0';

  if ( i < 0 )
  {
    if ( ( i = -i ) > 0 )
    {
      pString += sprintf( pString, "%s", "負" );
      pString  = chinese_number( i, pString  );
    }

    else
    {
      pString += sprintf( pString, "%s", "負無限大" );
    }
  }

  else if ( i < 11 )
  {
    pString += sprintf( pString, "%s", c_num[i] );
  }

  else if ( i < 20 )
  {
    pString += sprintf( pString, "%s%s", c_num[10], c_num[i-10] );
  }

  else if ( i < 100 )
  {
    pString += sprintf( pString, "%s%s", c_num[i/10], c_dig[1] );
    if ( i % 10 ) pString += sprintf( pString, "%s", c_num[ i % 10 ] );
  }

  else if ( i < 1000 )
  {
    if ( ( mod = ( i % 100 ) ) == 0 )
    {
      pString += sprintf( pString, "%s%s", c_num[i/100], c_dig[2] );
    }

    else if ( mod < 10 )
    {
      pString += sprintf( pString, "%s%s%s", c_num[i/100],c_dig[2],c_num[0] );
      pString = chinese_number( mod, pString );
    }

    else if ( mod < 20 )
    {
      pString += sprintf( pString, "%s%s%s", c_num[i/100],c_dig[2],c_num[1] );
      pString = chinese_number( mod, pString );
    }
    else
    {
      pString += sprintf( pString, "%s%s", c_num[i/100], c_dig[2] );
      pString = chinese_number( mod, pString );
    }
  }

  else if ( i < 10000 )
  {
    if ( ( mod = ( i % 1000 ) ) == 0 )
    {
      pString += sprintf( pString, "%s%s", c_num[i/1000], c_dig[3] );
    }

    else if ( mod < 100 )
    {
      pString += sprintf( pString, "%s%s%s", c_num[i/1000],c_dig[3],c_dig[0] );
      pString = chinese_number( mod, pString );
    }

    else
    {
      pString += sprintf( pString, "%s%s", c_num[i/1000], c_dig[3] );
      pString = chinese_number( mod, pString );
    }
  }

  else if ( i < 100000000 )
  {
    if ( ( mod = ( i % 10000 ) ) == 0 )
    {
      pString  = chinese_number( i/10000, pString );
      pString += sprintf( pString, "%s", c_dig[4] );
    }

    else if ( mod < 1000 )
    {
      pString  = chinese_number( i/10000 , pString            );
      pString += sprintf( pString, "%s%s", c_dig[4], c_dig[0] );
      pString  = chinese_number( mod, pString                 );
    }

    else
    {
      pString  = chinese_number( i/10000, pString );
      pString += sprintf( pString, "%s", c_dig[4] );
      pString  = chinese_number( mod,     pString );
    }
  }

  else if ( i < 1000000000 )
  {
    if ( ( mod = ( i % 100000000 ) ) == 0 )
    {
      pString += sprintf( pString, "%s%s", c_num[i/100000000], c_dig[5] );
    }

    else if ( mod < 10000000 )
    {
      pString +=
        sprintf( pString, "%s%s%s", c_num[i/100000000], c_dig[5], c_num[0] );
      pString  = chinese_number( mod , pString );
    }

    else
    {
      pString += sprintf( pString, "%s%s", c_num[i/100000000], c_dig[5] );
      pString  = chinese_number( mod, pString );
    }
  }

  else
  {
    pString += sprintf( pString, "%s", "無限大" );
  }

  RETURN( pString );
}

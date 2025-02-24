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
#include <unistd.h>
#include "merc.h"

#define MAX_DIGITAL     5

/* �ˬd�Y�r��̬O�_���]�t�Y�Ӧr�� */
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
    send_to_char( "�L�k�}���ɮסC\n\r", ch );
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

               if ( pMobIndex->count >  10 ) amount = "���h";
          else if ( pMobIndex->count >  5  ) amount = "�ܦh";
          else if ( pMobIndex->count >= 3  ) amount = "�h  ";
          else                               amount = "���q";

          found  = TRUE;
          count += pMobIndex->count;
          i[loop / 10 ] += pMobIndex->count;
          j[loop / 10 ] ++;

          fprintf( pFile, "%s���šR%3d �ƶq�R%s �ϰ�R%10s �W�١R%s%s\n"
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
      fprintf( pFile, "\e[1;37m���� %d �S���Ǫ��C\e[0m\n", loop );

    else
      fprintf( pFile, "���� \e[1;35m%d\e[0m �`�ƥج� \e[1;36m%d\e[0m�C\n", loop, count );

    */

    fprintf( pFile, "\n" );
  }

  for ( loop = 0; loop < 10; loop++ )
  {
    fprintf( pFile, "���šR%3d-%3d  �`���ơR %4d  �����R %4d\n"
      , loop * 10, ( loop + 1 ) * 10 - 1, i[loop], j[loop] );
  }

  fclose( pFile );
  RETURN_NULL();
}

/* ���զr��O�_�O�Ʀr */
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

  /* �M���}�C */
  for ( loop = 0; loop < MAX_DIGITAL; loop++ ) buf[loop] = '\x0';
  *number = 0;

  for ( *number = -1, loop = 0; *argument != ' '; loop++, argument++ )
  {
    if ( !isdigit( ( int ) *argument ) || loop >= MAX_DIGITAL ) RETURN( NULL );
    buf[loop] = *argument - '0';
  }

  /* �Y���S���h�^�� -1 */
  if ( loop == 0 ) RETURN( NULL );

  factor = 1;
  while ( loop-- > 1 ) factor *= 10;

  for ( *number = 0, loop = 0; factor > 0; loop++ , factor /= 10 )
    *number += buf[loop] * factor;

  while ( *argument == ' ' ) argument++;
  RETURN( argument );
}

/* �p��r����� */
size_t str_len( const char * argument )
{
  register size_t len;

  PUSH_FUNCTION( "str_len" );

  for ( len = 0; *argument; argument++ , len++ );

  RETURN( len );
}

/* �p��r����סM�����]�t ansi code */
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

/* ���Ӧr��s���_�� */
char * str_cat( char * dest , const char * source )
{
  char * buf;

  PUSH_FUNCTION( "str_cat" );

  /* ����r�꥽�� */
  for ( buf = dest; *dest; dest++ );

  /* �s���r�� */
  for ( ; *source; *dest++ = *source++ );

  /* ��r�꥽�ݥ[�W�Ŧr�� */
  *dest = '\x0';

  RETURN( buf );
}

/* �����r�� */
char * str_cpy( char * dest , const char * source )
{
  char * buf;
  PUSH_FUNCTION( "str_cpy" );
  for ( buf = dest; *source; *dest++ = *source++ );
  *dest = '\x0';
  RETURN( buf );
}

/* ���Y�r�ꪺ�e n �Ӧr�� */
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
    mudlog( LOG_DEBUG , "str_scmp: �Ū��ӷ��r��." );
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
    mudlog( LOG_DEBUG , "str_cmp: �Ū��ӷ��r��." );
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
    mudlog( LOG_DEBUG , "str_prefix: �Ū��ӷ��r��." );
    RETURN( TRUE );
  }

  if ( !bstr )
  {
    mudlog( LOG_DEBUG , "str_prefix: �Ū��ت��r��." );
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
   { "�s", "�Q", "��", "�d", "�U", "��", "��" };

  static char * c_num[] =
    { "�s", "�@", "�G", "�T", "�|", "��", "��", "�C", "�K", "�E", "�Q" };

  PUSH_FUNCTION( "chinese_number" );

  *pString = '\x0';

  if ( i < 0 )
  {
    if ( ( i = -i ) > 0 )
    {
      pString += sprintf( pString, "%s", "�t" );
      pString  = chinese_number( i, pString  );
    }

    else
    {
      pString += sprintf( pString, "%s", "�t�L���j" );
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
    pString += sprintf( pString, "%s", "�L���j" );
  }

  RETURN( pString );
}

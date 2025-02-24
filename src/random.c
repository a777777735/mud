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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "merc.h"

int     number_mm       args( ( void ) );

static  int     rgiState[2+55];

/* 啟始化亂數表 */
void init_mm( )
{
  int * piState;
  int   iState;

  PUSH_FUNCTION( "init_mm" );

  piState     = &rgiState[2];
  piState[-2] = 55 - 55;
  piState[-1] = 55 - 24;
  piState[ 0] = (( int ) current_time ) & ( ( 1 << 30 ) - 1 );
  piState[ 1] = 1;
  for ( iState = 2; iState < 55; iState++ )
  {
    piState[iState] = (piState[iState-1] + piState[iState-2])
                    & (( 1 << 30 ) - 1);
  }
  RETURN_NULL();
}

/* 亂數產生一個數 */
int number_mm( void )
{
  int * piState;
  int   iState1;
  int   iState2;
  int   iRand;

  PUSH_FUNCTION( "number_mm" );

  piState = &rgiState[2];
  iState1 = piState[-2];
  iState2 = piState[-1];
  iRand   = ( piState[iState1] + piState[iState2]) & ( ( 1 << 30 ) - 1 );
  piState[iState1] = iRand;
  if ( ++iState1 == 55 ) iState1 = 0;
  if ( ++iState2 == 55 ) iState2 = 0;
  piState[-2] = iState1;
  piState[-1] = iState2;
  RETURN( iRand >> 6 );
}

/* 亂數產生一個方向 */
int number_door( void )
{
  int door;

  PUSH_FUNCTION( "number_door" );
  while ( ( door = number_mm() & ( 8-1 ) ) > 5 ) ;
  RETURN( door );
}

int number_fuzzy( int number )
{
  PUSH_FUNCTION( "number_fuzzy" );

  switch ( number_bits(2) )
  {
  case 0:  number--; break;
  case 3:  number++; break;
  }

  RETURN( UMAX( 1, number ) );
}

/* 產生一個由 from 到 to 的亂數 */
int number_range( int from, int to )
{
  int power;
  int number;

  PUSH_FUNCTION( "number_range" );

  /* 數值太大會陷入無窮回圈 */
  to   = UMIN( 1000000, to   );
  from = UMIN( 1000000, from );

  if ( from > to ) SWAP( from , to );
  if ( ( to = to - from + 1 ) <= 1 ) RETURN( from );

  for ( power = 2; power < to; power <<= 1 );
  while ( ( number = number_mm() & ( power - 1 ) ) >= to );
  RETURN( from + number );
}

/* 隨機產生一個百分比 */
int number_percent( void )
{
  int percent;

  PUSH_FUNCTION( "number_percent" );
  while ( ( percent = number_mm() & ( 128 - 1 ) ) > 99 );
  RETURN( 1 + percent );
}

int number_bits( int width )
{
  PUSH_FUNCTION( "number_bits" );
  RETURN( number_mm() & ( ( 1 << width ) - 1 ) );
}

int dice( int number, int size )
{
  int idice;
  int sum;

  PUSH_FUNCTION( "dice" );

  switch ( size )
  {
  case 0: RETURN( 0 );
  case 1: RETURN( number );
  }

  for ( idice = 0, sum = 0; idice < number; idice++ )
    sum += number_range( 1, size );

  RETURN( sum );
}

int variation( int value, int percentage )
{
  int shift;

  PUSH_FUNCTION( "variation" );

  if ( value > percentage && percentage >= 0 && percentage < 100 )
  {
    shift = value * percentage / 100;
    value = number_range( value - shift, value + shift );
  }

  RETURN( value );
}

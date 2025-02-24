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
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "merc.h"

struct rusage   SelfRusage;
struct rusage   ChildRusage;

TOP_DATA *      NowTop          =       NULL;
int             NowMinute       =       -1;

void top_update( void )
{
  TOP_DATA        * pTop;
  DESCRIPTOR_DATA * pDesc;
  struct rusage     usage;
  struct tm       * sTime;
  time_t            nowtime;
  int               loop;
  int               count;
  int               stime;
  int               ustime;

  PUSH_FUNCTION( "update_top" );

  nowtime = time( NULL );
  if ( !( sTime = localtime( &nowtime ) ) ) RETURN_NULL();

  if ( !top_list
    || top_list->day  != sTime->tm_mday
    || top_list->hour != sTime->tm_hour )
  {
    pTop          = alloc_struct( STRUCT_TOP_DATA );
    pTop->day     = sTime->tm_mday;
    pTop->hour    = sTime->tm_hour;
    pTop->nRead   = 0;
    pTop->nWrite  = 0;
    pTop->number  = 0;
    pTop->loading = 0;

    for ( count = 0; count < 60; count++ )
    {
      pTop->time[count].stime  = -1;
      pTop->time[count].ustime = -1;
    }

    /* �]�w�ثe�����c */
    NowTop = pTop;

    pTop->next = top_list;
    top_list   = pTop;
  }

  for ( count = 0, pDesc = descriptor_list; pDesc; pDesc = pDesc->next )
    if ( !pDesc->delete ) count++;

  top_list->number = UMAX( top_list->number, count );

  /* �O���t���q */
  if ( sTime->tm_min != NowMinute )
  {
    NowMinute = UMIN( 59, sTime->tm_min );

    if ( getrusage( RUSAGE_SELF, &usage ) == 0 )
    {
      NowTop->time[NowMinute].stime = usage.ru_utime.tv_sec
                                    + usage.ru_stime.tv_sec
                                    - SelfRusage.ru_utime.tv_sec
                                    - SelfRusage.ru_stime.tv_sec;

      NowTop->time[NowMinute].ustime = usage.ru_utime.tv_usec
                                     + usage.ru_stime.tv_usec
                                     - SelfRusage.ru_utime.tv_usec
                                     - SelfRusage.ru_stime.tv_usec;
    }

    SelfRusage = usage;

    if ( getrusage( RUSAGE_CHILDREN, &usage ) == 0 )
    {
      NowTop->time[NowMinute].stime += usage.ru_utime.tv_sec
                                     + usage.ru_stime.tv_sec
                                     - ChildRusage.ru_utime.tv_sec
                                     - ChildRusage.ru_stime.tv_sec;

      NowTop->time[NowMinute].ustime += usage.ru_utime.tv_usec
                                      + usage.ru_stime.tv_usec
                                      - ChildRusage.ru_utime.tv_usec
                                      - ChildRusage.ru_stime.tv_usec;
    }

    ChildRusage = usage;

    while ( NowTop->time[NowMinute].ustime < 0 )
    {
      NowTop->time[NowMinute].ustime += 1000000;
      NowTop->time[NowMinute].stime--;
    }

    for ( stime = ustime = count = loop = 0; loop < 60; loop++ )
    {
      if ( NowTop->time[loop].stime >= 0 && NowTop->time[loop].ustime >= 0 )
      {
        count++;

        stime  += NowTop->time[loop].stime;
        ustime += NowTop->time[loop].ustime;

        while ( ustime >= 1000000 )
        {
          stime++;
          ustime -= 1000000;
        }
      }
    }

    if ( count > 0 )
    {
      NowTop->loading = ( stime * 10000 + ustime / 100 ) / ( count * 60 );
    }
    else
    {
      NowTop->loading = 0;
    }
  }

  RETURN_NULL();
}

FUNCTION( do_top )
{
  TOP_DATA * pTop;
  TOP_DATA * pMax;
  TOP_DATA * pMin;
  int        max = -1;
  int        min = max_connect;
  int        number;
  int        loop;

  PUSH_FUNCTION( "do_top" );

  if ( !top_list ) top_update();

  pMax = top_list;
  pMin = top_list;

  clear_buffer();
  send_to_buffer( "\e[1;33;44m�� �� �� ��  �t ��  �g �J  Ū �J �H��"
    " ��  �N  ��                                \e[0m\n\r" );

  for ( pTop = top_list; pTop; pTop = pTop->next )
  {
    number = UMAX( 1, ( pTop->number * 42 ) / max_connect );

    send_to_buffer( "%2d��%s%2d�I %2d.%d%d %6d %6d %4d \e[1;32m"
      , pTop->day
      , pTop->hour < 12 ? "���W" : ( pTop->hour >= 18 ? "�ߤW" : "�U��" )
      , pTop->hour % 12
      , pTop->loading / 100
      , ( pTop->loading % 100 ) / 10
      , ( pTop->loading % 100 ) % 10
      , pTop->nWrite / 1000
      , pTop->nRead  / 1000
      , pTop->number );

    for ( loop = 0; loop < number; loop++ ) send_to_buffer( "*" );
    send_to_buffer( "\e[0m\n\r" );

    if ( pTop->number > max )
    {
      max  = pTop->number;
      pMax = pTop;
    }

    if ( pTop->number < min )
    {
      min  = pTop->number;
      pMin = pTop;
    }
  }

  send_to_buffer(
    "\n\r���%2d��%2d�I�ɡM���̤j�W�u�H�� %d�H�C"
    "\n\r���%2d��%2d�I�ɡM���̤֤W�u�H�� %d�H�C\n\r"
    , pMax->day, pMax->hour, max
    , pMin->day, pMin->hour, min );

  print_buffer( ch );
  RETURN_NULL();
}

bool get_exec_time( int min, int * stime, int * ustime )
{
  int        loop;
  int        location;
  TOP_DATA * pTop;

  PUSH_FUNCTION( "get_exec_time" );

  if ( min <= 0 ) RETURN( FALSE );

  *stime   = 0;
  *ustime  = 0;
  location = NowMinute;
  pTop     = NowTop;

  for ( loop = 0; loop < min; loop++ )
  {
    if ( !pTop
      || pTop->time[location].stime < 0
      || pTop->time[location].ustime < 0 ) RETURN( FALSE );

    *stime  += pTop->time[location].stime;
    *ustime += pTop->time[location].ustime;

    while ( *ustime >= 1000000 )
    {
      *ustime -= 1000000;
      (*stime)++;
    }

    if ( --location <= 0 )
    {
      pTop     = pTop->next;
      location = 59;
    }
  }

  RETURN( TRUE );
}

int calc_loading( int min, int stime, int ustime )
{
  int loading;
  int total;

  PUSH_FUNCTION( "calc_loading" );

  if ( min <= 0 || stime < 0 || ustime < 0 ) RETURN( 0 );

  total  = stime * 10000;
  total += ustime / 100;

  loading = total / ( min * 60 );

  RETURN( loading );
}

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
#include <stdio.h>
#include "merc.h"

SCAN_DATA       ScanData[MAX_SCAN_COUNT];
int             ScanPointer = 0;

bool    add_scan        args( ( int, int, int[] ) );

void clean_scan( void )
{
  int loop;
  int count;

  PUSH_FUNCTION( "clean_scan" );

  for ( loop = 0; loop < MAX_SCAN_COUNT; loop++ )
  {
    ScanData[loop].vnum  = -1;
    ScanData[loop].depth = -1;

    for ( count = 0; count < MAX_SCAN_DEPTH; count++ )
      ScanData[loop].dir[count] = -1;
  }

  ScanPointer = 0;
  RETURN_NULL();
}

void scan_room( int vnum, int depth, int source )
{
  extern const sh_int  rev_dir [];
  static int           ScanDir[MAX_SCAN_DEPTH + 1];
  static int           ScanDepth = 0;
  ROOM_INDEX_DATA    * pRoom;
  ROOM_INDEX_DATA    * pEnd;
  EXIT_DATA          * pExit;
  int                  dir;

  PUSH_FUNCTION( "scan_room" );

  if ( depth >= MAX_SCAN_DEPTH ) depth = MAX_SCAN_DEPTH - 1;
  ScanDepth++;

  if ( !( pRoom = get_room_index( vnum ) ) || depth <= 0 )
  {
    ScanDepth--;
    RETURN_NULL();
  }

  for ( dir = 0; dir < DIR_MAX; dir++ )
  {
    ScanDir[ScanDepth-1] = dir;
    ScanDir[ScanDepth]   = -1;

    if ( ( pExit = pRoom->exit[dir] )
      && ( pEnd = pExit->to_room )
      && pEnd->area == pRoom->area
      && rev_dir[dir] != source
      && add_scan( pEnd->vnum, depth, ScanDir ) )
    {
      scan_room( pEnd->vnum, depth - 1, dir );
    }
  }

  ScanDepth--;
  RETURN_NULL();
}

bool add_scan( int vnum, int depth, int dir[] )
{
  int loop;
  int count;

  PUSH_FUNCTION( "add_scan" );

  for ( loop = 0; loop < ScanPointer; loop++ )
  {
    if ( ScanData[loop].vnum == vnum )
    {
      if ( ScanData[loop].depth < depth )
      {
        for ( count = 0; count < MAX_SCAN_DEPTH; count++ )
          ScanData[loop].dir[count] = -1;

        for ( count = 0; count < MAX_SCAN_DEPTH; count++ )
        {
          if ( dir[count] == -1 ) break;
          ScanData[loop].dir[count] = dir[count];
        }

        ScanData[loop].depth = depth;
        RETURN( TRUE );
      }

      RETURN( FALSE );
    }
  }

  if ( ScanPointer < MAX_SCAN_COUNT - 1 )
  {
    ScanData[loop].vnum  = vnum;
    ScanData[loop].depth = depth;
    for ( count = 0; count < MAX_SCAN_DEPTH; count++ )
      ScanData[loop].dir[count] = -1;

    for ( count = 0; count < MAX_SCAN_DEPTH; count++ )
    {
      if ( dir[count] == -1 ) break;
      ScanData[loop].dir[count] = dir[count];
    }

    ScanPointer++;

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

void print_scan( CHAR_DATA * ch, OBJ_DATA * pFire )
{
  extern const sh_int   rev_dir [];
  char                  buf[MAX_STRING_LENGTH];
  ROOM_INDEX_DATA     * pSource;
  ROOM_INDEX_DATA     * pRoom;
  int                   loop;
  int                   count;
  int                   dir;

  PUSH_FUNCTION( "print_scan" );

  if ( !ch || !pFire )
  {
    mudlog( LOG_DEBUG, "print_scan: �ʥF�ӷ�." );
    RETURN_NULL();
  }

  if ( !( pSource = ch->in_room ) )
  {
    mudlog( LOG_DEBUG, "print_scan: �ӷ��ж������T." );
    RETURN_NULL();
  }

  for ( loop = 0; loop < ScanPointer; loop++ )
  {
    if ( !( pRoom = get_room_index( ScanData[loop].vnum ) ) ) continue;

    char_from_room( ch );
    char_to_room( ch, pRoom );

    str_cpy( buf, "�b�A��$2" );

    for ( count = MAX_SCAN_DEPTH - 1; count >= 0; count-- )
    {
      if ( ( dir = ScanData[loop].dir[count] ) < 0 || dir >= DIR_MAX ) continue;
      str_cat( buf, direction_name( rev_dir[ScanData[loop].dir[count]] ) );
    }

    str_cat( buf, "$0��V�X�{$7$p$0���Ϥ������M�n���}�G�T" );
    act( smash_act_keyword( buf, "nesp" ) , ch, pFire, NULL, TO_ROOM );

    char_from_room( ch );
    char_to_room( ch, pSource );
  }

  RETURN_NULL();
}

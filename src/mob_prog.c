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
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "merc.h"

/* 區域函數 */
#define CD CHAR_DATA
#define OD OBJ_DATA
char *  mprog_next_command args( ( char * ) );
bool    mprog_seval        args( ( char *, char * , char * ) );
bool    mprog_veval        args( ( int, char *, int ) );
int     mprog_do_ifchck    args( ( char *, CD *, CD *, OD *, void *, CD * ) );
char *  mprog_process_if   args( ( char *, char *, CD *, CD *, OD *, void *
                                 , CD * ) );
void    mprog_translate    args( ( char, char *, CD *, CD *
                                   , OD *, void *, CD * ) );
void    mprog_process_cmnd args( ( char *, CD *, CD *, OD *, void *, CD * ) );
#undef  CD
#undef  OD

char * mprog_next_command( char * clist )
{
  char * pointer = clist;

  PUSH_FUNCTION( "mprog_next_command" );

  while ( *pointer != '\n' && *pointer != '\x0' ) pointer++;
  if ( *pointer == '\n' || *pointer == '\r' ) *pointer++ = '\x0';

  RETURN( pointer );
}

bool mprog_seval( char * lhs, char * opr, char * rhs )
{
  PUSH_FUNCTION( "mprog_seval" );
  if ( !str_cmp( opr, "==" ) ) RETURN( !str_cmp  ( lhs, rhs ) );
  if ( !str_cmp( opr, "!=" ) ) RETURN(  str_cmp  ( lhs, rhs ) );
  if ( !str_cmp( opr, "/"  ) ) RETURN( !str_infix( rhs, lhs ) );
  if ( !str_cmp( opr, "!/" ) ) RETURN(  str_infix( rhs, lhs ) );
  mudlog( LOG_DEBUG , "mprog_seval: MOBPROG 不正當的運算" );
  RETURN( FALSE );
}

bool mprog_veval( int lhs, char * opr, int rhs )
{
  PUSH_FUNCTION( "mprog_reval" );
  if ( !str_cmp( opr, "==" ) ) RETURN( lhs == rhs );
  if ( !str_cmp( opr, "!=" ) ) RETURN( lhs != rhs );
  if ( !str_cmp( opr, ">"  ) ) RETURN( lhs  > rhs );
  if ( !str_cmp( opr, "<"  ) ) RETURN( lhs  < rhs );
  if ( !str_cmp( opr, "<=" ) ) RETURN( lhs <= rhs );
  if ( !str_cmp( opr, ">=" ) ) RETURN( lhs >= rhs );
  if ( !str_cmp( opr, "&"  ) ) RETURN( lhs  & rhs );
  if ( !str_cmp( opr, "|"  ) ) RETURN( lhs  | rhs );
  mudlog( LOG_DEBUG , "mprog_veval: MOBPROG 不正當的運算." );
  RETURN( FALSE );
}

int mprog_do_ifchck( char * ifchck, CHAR_DATA * mob, CHAR_DATA * actor,
                     OBJ_DATA * obj, void * vo, CHAR_DATA * rndm )
{
  char        buf[ MAX_INPUT_LENGTH ];
  char        arg[ MAX_INPUT_LENGTH ];
  char        opr[ MAX_INPUT_LENGTH ];
  char        val[ MAX_INPUT_LENGTH ];
  CHAR_DATA * vict  = ( CHAR_DATA * ) vo;
  OBJ_DATA  * v_obj = ( OBJ_DATA  * ) vo;
  char      * bufpt = buf;
  char      * argpt = arg;
  char      * oprpt = opr;
  char      * valpt = val;
  char      * point = ifchck;
  int         vnum;
  int         num;

  PUSH_FUNCTION( "mprog_do_ifchck" );

  vnum = mob->pIndexData->vnum;

  if ( *point == '\x0' )
  {
    mudlog( LOG_DEBUG , "mprog_do_ifchck: %d 空的 ifchck", vnum );
    RETURN( ERRORCODE );
  }

  NOSPACE( point );

  /* 找第一個函數 */
  while ( *point != '(' )
  {
    if ( *point == '\x0' )
    {
      mudlog( LOG_DEBUG , "mprog_do_ifchck: %d ifchck 語法錯誤", vnum );
      RETURN( ERRORCODE );
    }

    else
    {
      if ( *point == ' ' || *point == '\t' ) point++;
      else *bufpt++ = *point++;
    }
  }

  *bufpt = '\x0';
  point++;

  /* 找第一個函數的引數 */
  while ( *point != ')' )
  {
    if ( *point == '\x0' )
    {
      mudlog( LOG_DEBUG , "mprog_do_ifchck: %d ifchck 語法錯誤.", vnum );
      RETURN( ERRORCODE );
    }
    else
    {
      if ( *point == ' ' || *point == '\t' ) point++;
      else *argpt++ = *point++;
    }
  }

  *argpt = '\x0';
  point++;
  NOSPACE( point );

  /* 找運算元以及數值 */
  if ( *point == '\x0' )
  {
    *opr = *val = '\x0';
  }

  else
  {
    /* 找出運算元 */
    while ( ( *point != ' ' ) && ( !isalnum( ( int ) *point ) ) )
    {
      if ( *point == '\x0' )
      {
        mudlog( LOG_DEBUG , "mprog_do_ifchck: 怪物 %d ifchck 運算沒有數值", vnum );
        RETURN( ERRORCODE );
      }
      else
      {
        *oprpt++ = *point++;
      }
    }

    *oprpt = '\x0';
    NOSPACE( point );

    /* 找出數值 */
    for( ;; )
    {
      if ( ( *point != ' ' ) && ( *point == '\x0' ) ) break;
      else                                            *valpt++ = *point++;
    }

    *valpt = '\x0';
  }

  num = atoi( val );

  if ( !str_cmp( buf, "rand" ) ) RETURN( number_percent() <= atoi( arg ) );

  if ( !str_cmp( buf, "ispc" ) )
  {
    switch ( arg[1] )
    {
    case 'i':
      RETURN( FALSE );

    case 'n':
      if ( actor ) RETURN( !IS_NPC( actor ) );
      RETURN( ERRORCODE );

    case 't':
      if ( vict ) RETURN( !IS_NPC( vict ) );
      RETURN( ERRORCODE );

    case 'r':
      if ( rndm ) RETURN( !IS_NPC( rndm ) );
      RETURN( ERRORCODE );

    default:
      mudlog( LOG_DEBUG , "mprog_do_ifchck: %d 'ispc' 有錯誤參數 %c.", vnum , arg[1] );
      RETURN( ERRORCODE );
    }
  }

  if ( !str_cmp( buf, "isnpc" ) )
  {
    switch ( arg[1] )
    {
    case 'i':
      RETURN( TRUE );

    case 'n':
      if ( actor ) RETURN( IS_NPC( actor ) );
      RETURN( ERRORCODE );

    case 't':
      if ( vict ) RETURN( IS_NPC( vict ) );
      RETURN( ERRORCODE );

    case 'r':
      if ( rndm ) RETURN( IS_NPC( rndm ) );
      RETURN( ERRORCODE );

    default:
      mudlog( LOG_DEBUG , "mprog_do_ifchck: %d 'isnpc' 裡有錯誤參數.", vnum );
      RETURN( ERRORCODE );
    }
  }

  if ( !str_cmp( buf, "isgood" ) )
  {
    switch ( arg[1] )
    {
    case 'i':
      RETURN( IS_GOOD( mob ) );

    case 'n':
      if ( actor ) RETURN( IS_GOOD( actor ) );
      RETURN( ERRORCODE );

    case 't':
      if ( vict ) RETURN( IS_GOOD( vict ) );
      RETURN( ERRORCODE );

    case 'r':
      if ( rndm ) RETURN( IS_GOOD( rndm ) );
      RETURN( ERRORCODE );

    default:
      mudlog( LOG_DEBUG , "mprog_do_ifchck: %d 'isgood' 裡有錯誤參數.", vnum );
      RETURN( ERRORCODE );
    }
  }

  if ( !str_cmp( buf, "isquest" ) )
  {
    switch ( arg[1] )
    {
    case 'i':
      RETURN( TRUE );

    case 'n':
      if ( actor ) RETURN( is_quest( actor, opr, val ) );
      RETURN( ERRORCODE );

    case 't':
      if ( vict ) RETURN( is_quest( vict, opr, val ) );
      RETURN( ERRORCODE );

    case 'r':
      if ( rndm ) RETURN( is_quest( rndm, opr, val ) );
      RETURN( ERRORCODE );

    default:
      mudlog( LOG_DEBUG , "mprog_do_ifchck: %d 'isquest' 裡有錯誤的參數.", vnum );
      RETURN( ERRORCODE );
    }
  }

  if ( !str_cmp( buf, "isenemy" ) )
  {
    switch ( arg[1] )
    {
    case 'i':
      RETURN( TRUE );

    case 'n':
      if ( actor ) RETURN( is_enemy( actor, opr, val ) );
      RETURN( ERRORCODE );

    case 't':
      if ( vict ) RETURN( is_enemy( vict, opr, val ) );
      RETURN( ERRORCODE );

    case 'r':
      if ( rndm ) RETURN( is_enemy( rndm, opr, val ) );
      RETURN( ERRORCODE );

    default:
      mudlog( LOG_DEBUG , "mprog_do_ifchck: %d 'isenemy' 裡有錯誤的參數.", vnum );
      RETURN( ERRORCODE );
    }
  }

  if ( !str_cmp( buf, "isfight" ) )
  {
    switch ( arg[1] )
    {
    case 'i':
      RETURN( ( mob->fighting ) ? TRUE : FALSE );

    case 'n':
      if ( actor ) RETURN( ( actor->fighting ) ? TRUE : FALSE );
      RETURN( ERRORCODE );

    case 't':
      if ( vict ) RETURN( ( vict->fighting ) ? TRUE : FALSE );
      RETURN( ERRORCODE );

    case 'r':
      if ( rndm ) RETURN( ( rndm->fighting ) ? TRUE : FALSE );
      RETURN( ERRORCODE );

    default:
      mudlog( LOG_DEBUG , "mprog_do_ifchck: %d 'isfight' 裡有錯誤的參數.", vnum );
      RETURN( ERRORCODE );
    }
  }

  if ( !str_cmp( buf, "isimmort" ) )
  {
    switch ( arg[1] )
    {
    case 'i':
      RETURN( get_trust( mob ) > LEVEL_IMMORTAL );

    case 'n':
      if ( actor ) RETURN( get_trust( actor ) > LEVEL_IMMORTAL );
      RETURN( ERRORCODE );

    case 't':
      if ( vict ) RETURN( get_trust( vict ) > LEVEL_IMMORTAL );
      RETURN( ERRORCODE );

    case 'r':
      if ( rndm ) RETURN( get_trust( rndm ) > LEVEL_IMMORTAL );
      RETURN( ERRORCODE );

    default:
      mudlog( LOG_DEBUG , "mprog_do_ifchck: %d 'isimmort' 裡有錯誤的參數.", vnum );
      RETURN( ERRORCODE );
    }
  }

  if ( !str_cmp( buf, "isimminvis" ) )
  {
    switch ( arg[1] )
    {
    case 'i':
      RETURN( ERRORCODE );

    case 'n':
      if ( actor ) RETURN( !IS_NPC( actor ) ?
        ( IS_SET( actor->act, PLR_WIZINVIS ) ? TRUE : FALSE ) : FALSE );
      RETURN( ERRORCODE );

    case 't':
      if ( vict ) RETURN( !IS_NPC( vict ) ?
        ( IS_SET( vict->act, PLR_WIZINVIS ) ? TRUE : FALSE ) : FALSE );
      RETURN( ERRORCODE );

    case 'r':
      if ( rndm ) RETURN( !IS_NPC( rndm ) ?
        ( IS_SET( rndm->act, PLR_WIZINVIS ) ? TRUE : FALSE ) : FALSE );
      RETURN( ERRORCODE );

    default:
      mudlog( LOG_DEBUG , "mprog_do_ifchck: %d 'isimminvis' 裡有錯誤的參數.", vnum );
      RETURN( ERRORCODE );
    }
  }

  if ( !str_cmp( buf, "align" ) )
  {
    switch ( arg[1] )
    {
    case 'i':
      if ( mob ) RETURN( mprog_veval( mob->alignment, opr, num ) );
      RETURN( ERRORCODE );

    case 'n':
      if ( actor ) RETURN( mprog_veval( actor->alignment, opr, num ));
      RETURN( ERRORCODE );

    case 't':
      if ( vict ) RETURN( mprog_veval( vict->alignment, opr, num ) );
      RETURN( ERRORCODE );

    case 'r':
      if ( rndm ) RETURN( mprog_veval( rndm->alignment, opr, num ) );
      RETURN( ERRORCODE );

    default:
      mudlog( LOG_DEBUG , "mprog_do_ifchck: %d 'align' 有錯誤參數 %c.", vnum , arg[1] );
      RETURN( ERRORCODE );
    }
  }

  if ( !str_cmp( buf, "str" ) )
  {
    switch ( arg[1] )
    {
    case 'i':
      if ( mob ) RETURN( mprog_veval( get_curr_str( mob ), opr, num ) );
      RETURN( ERRORCODE );

    case 'n':
      if ( actor ) RETURN( mprog_veval( get_curr_str( actor ), opr, num ));
      RETURN( ERRORCODE );

    case 't':
      if ( vict ) RETURN( mprog_veval( get_curr_str( vict ), opr, num ) );
      RETURN( ERRORCODE );

    case 'r':
      if ( rndm ) RETURN( mprog_veval( get_curr_str( rndm ), opr, num ) );
      RETURN( ERRORCODE );

    default:
      mudlog( LOG_DEBUG , "mprog_do_ifchck: %d 'str' 有錯誤參數 %c.", vnum , arg[1] );
      RETURN( ERRORCODE );
    }
  }

  if ( !str_cmp( buf, "int" ) )
  {
    switch ( arg[1] )
    {
    case 'i':
      if ( mob ) RETURN( mprog_veval( get_curr_int( mob ), opr, num ) );
      RETURN( ERRORCODE );

    case 'n':
      if ( actor ) RETURN( mprog_veval( get_curr_int( actor ), opr, num ));
      RETURN( ERRORCODE );

    case 't':
      if ( vict ) RETURN( mprog_veval( get_curr_int( vict ), opr, num ) );
      RETURN( ERRORCODE );

    case 'r':
      if ( rndm ) RETURN( mprog_veval( get_curr_int( rndm ), opr, num ) );
      RETURN( ERRORCODE );

    default:
      mudlog( LOG_DEBUG , "mprog_do_ifchck: %d 'int' 有錯誤參數 %c.", vnum , arg[1] );
      RETURN( ERRORCODE );
    }
  }

  if ( !str_cmp( buf, "wis" ) )
  {
    switch ( arg[1] )
    {
    case 'i':
      if ( mob ) RETURN( mprog_veval( get_curr_wis( mob ), opr, num ) );
      RETURN( ERRORCODE );

    case 'n':
      if ( actor ) RETURN( mprog_veval( get_curr_wis( actor ), opr, num ));
      RETURN( ERRORCODE );

    case 't':
      if ( vict ) RETURN( mprog_veval( get_curr_wis( vict ), opr, num ) );
      RETURN( ERRORCODE );

    case 'r':
      if ( rndm ) RETURN( mprog_veval( get_curr_wis( rndm ), opr, num ) );
      RETURN( ERRORCODE );

    default:
      mudlog( LOG_DEBUG , "mprog_do_ifchck: %d 'wis' 有錯誤參數 %c.", vnum , arg[1] );
      RETURN( ERRORCODE );
    }
  }

  if ( !str_cmp( buf, "dex" ) )
  {
    switch ( arg[1] )
    {
    case 'i':
      if ( mob ) RETURN( mprog_veval( get_curr_dex( mob ), opr, num ) );
      RETURN( ERRORCODE );

    case 'n':
      if ( actor ) RETURN( mprog_veval( get_curr_dex( actor ), opr, num ));
      RETURN( ERRORCODE );

    case 't':
      if ( vict ) RETURN( mprog_veval( get_curr_dex( vict ), opr, num ) );
      RETURN( ERRORCODE );

    case 'r':
      if ( rndm ) RETURN( mprog_veval( get_curr_dex( rndm ), opr, num ) );
      RETURN( ERRORCODE );

    default:
      mudlog( LOG_DEBUG , "mprog_do_ifchck: %d 'dex' 有錯誤參數 %c.", vnum , arg[1] );
      RETURN( ERRORCODE );
    }
  }

  if ( !str_cmp( buf, "con" ) )
  {
    switch ( arg[1] )
    {
    case 'i':
      if ( mob ) RETURN( mprog_veval( get_curr_con( mob ), opr, num ) );
      RETURN( ERRORCODE );

    case 'n':
      if ( actor ) RETURN( mprog_veval( get_curr_con( actor ), opr, num ));
      RETURN( ERRORCODE );

    case 't':
      if ( vict ) RETURN( mprog_veval( get_curr_con( vict ), opr, num ) );
      RETURN( ERRORCODE );

    case 'r':
      if ( rndm ) RETURN( mprog_veval( get_curr_con( rndm ), opr, num ) );
      RETURN( ERRORCODE );

    default:
      mudlog( LOG_DEBUG , "mprog_do_ifchck: %d 'con' 有錯誤參數 %c.", vnum , arg[1] );
      RETURN( ERRORCODE );
    }
  }

  if ( !str_cmp( buf, "ischarmed" ) )
  {
    switch ( arg[1] )
    {
    case 'i':
      RETURN( is_affected( mob, SLOT_CHARM_PERSON ) );

    case 'n':
      if ( actor ) RETURN( is_affected( actor, SLOT_CHARM_PERSON ) );
      RETURN( ERRORCODE );

    case 't':
      if ( vict ) RETURN( is_affected( vict, SLOT_CHARM_PERSON ) );
      RETURN( ERRORCODE );

    case 'r':
      if ( rndm ) RETURN( is_affected( rndm, SLOT_CHARM_PERSON ) );
      RETURN( ERRORCODE );

    default:
      mudlog( LOG_DEBUG , "mprog_do_ifchck: %d 'ischarmed' 裡有錯誤的參數.", vnum );
      RETURN( ERRORCODE );
    }
  }

  if ( !str_cmp( buf, "isfollow" ) )
  {
    switch ( arg[1] )
    {
    case 'i':
      RETURN( ( mob->master && mob->master->in_room == mob->in_room ) );

    case 'n':
      if ( actor ) RETURN( ( actor->master && actor->master->in_room == actor->in_room ) );
      RETURN( ERRORCODE );

    case 't':
      if ( vict ) RETURN( ( vict->master && vict->master->in_room == vict->in_room ) );
      RETURN( ERRORCODE );

    case 'r':
      if ( rndm ) RETURN( ( rndm->master && rndm->master->in_room == rndm->in_room ) );
      RETURN( ERRORCODE );

    default:
      mudlog( LOG_DEBUG , "mprog_do_ifchck: %d 'isfollow' 裡有錯誤的參數.", vnum );
      RETURN( ERRORCODE );
    }
  }

  if ( !str_cmp( buf, "hitprcnt" ) )
  {
    switch ( arg[1] )
    {
    case 'i':
      RETURN( mprog_veval( mob->hit / get_curr_hit( mob ), opr , num ) );

    case 'n':
      if ( actor ) RETURN( mprog_veval( actor->hit / get_curr_hit( actor ), opr, num ) );
      RETURN( ERRORCODE );

    case 't':
      if ( vict ) RETURN( mprog_veval( vict->hit / get_curr_hit( vict ), opr, num ) );
      RETURN( ERRORCODE );

    case 'r':
      if ( rndm ) RETURN( mprog_veval( rndm->hit/ get_curr_hit( rndm ), opr, num ) );
      RETURN( ERRORCODE );

    default:
      mudlog( LOG_DEBUG , "mprog_do_ifchck: %d 'hitprcnt' 裡有錯誤的參數.", vnum );
      RETURN( ERRORCODE );
    }
  }

  if ( !str_cmp( buf, "inroom" ) )
  {
    switch ( arg[1] )
    {
    case 'i':
      RETURN( mprog_veval( mob->in_room->vnum, opr, num ) );

    case 'n':
      if ( actor ) RETURN( mprog_veval( actor->in_room->vnum, opr, num ) );
      RETURN( ERRORCODE );

    case 't':
      if ( vict ) RETURN( mprog_veval( vict->in_room->vnum, opr, num ) );
      RETURN( ERRORCODE );

    case 'r':
      if ( rndm ) RETURN( mprog_veval( rndm->in_room->vnum, opr, num ) );
      RETURN( ERRORCODE );

    default:
      mudlog( LOG_DEBUG , "mprog_do_ifchck: %d 'inroom' 裡有錯誤的參數.", vnum );
      RETURN( ERRORCODE );
    }
  }

  if ( !str_cmp( buf, "sex" ) )
  {
    switch ( arg[1] )
    {
    case 'i':
      RETURN( mprog_veval( mob->sex, opr, num ) );

    case 'n':
      if ( actor ) RETURN( mprog_veval( actor->sex, opr, num ) );
      RETURN( ERRORCODE );

    case 't':
      if ( vict ) RETURN( mprog_veval( vict->sex, opr, num ) );
      RETURN( ERRORCODE );

    case 'r':
      if ( rndm ) RETURN( mprog_veval( rndm->sex, opr , num ) );
      RETURN( ERRORCODE );

    default:
      mudlog( LOG_DEBUG , "mprog_do_ifchck: %d 'sex' 錯誤的參數.", vnum );
      RETURN( ERRORCODE );
    }
  }

  if ( !str_cmp( buf, "position" ) )
  {
    switch ( arg[1] )
    {
    case 'i':
      RETURN( mprog_veval( mob->position, opr, num ) );

    case 'n':
      if ( actor ) RETURN( mprog_veval( actor->position, opr, num ) );
      RETURN( ERRORCODE );

    case 't':
      if ( vict ) RETURN( mprog_veval( vict->position, opr, num ) );
      RETURN( ERRORCODE );

    case 'r':
      if ( rndm ) RETURN( mprog_veval( rndm->position, opr, num ) );
      RETURN( ERRORCODE );

    default:
      mudlog( LOG_DEBUG , "mprog_do_ifchck: %d 'position' 裡有錯誤的參數.", vnum );
      RETURN( ERRORCODE );
    }
  }

  if ( !str_cmp( buf, "level" ) )
  {
    switch ( arg[1] )
    {
    case 'i':
      RETURN( mprog_veval( get_trust( mob ) , opr, num ) );

    case 'n':
      if ( actor ) RETURN( mprog_veval( get_trust( actor ) , opr, num ) );
      RETURN( ERRORCODE );

    case 't':
      if ( vict ) RETURN( mprog_veval( get_trust( vict ) , opr, num ) );
      RETURN( ERRORCODE );

    case 'r':
      if ( rndm ) RETURN( mprog_veval( get_trust( rndm ) , opr, num ) );
      RETURN( ERRORCODE );

    default:
      mudlog( LOG_DEBUG , "mprog_do_ifchck: %d 'level' 裡有錯誤的參數.", vnum );
      RETURN( ERRORCODE );
    }
  }

  if ( !str_cmp( buf, "goldamt" ) )
  {
    switch ( arg[1] )
    {
    case 'i':
      RETURN( mprog_veval( mob->gold, opr, num ) );

    case 'n':
      if ( actor ) RETURN( mprog_veval( actor->gold, opr, num ) );
      RETURN( ERRORCODE );

    case 't':
      if ( vict ) RETURN( mprog_veval( vict->gold, opr, num ) );
      RETURN( ERRORCODE );

    case 'r':
      if ( rndm ) RETURN( mprog_veval( rndm->gold, opr, num ) );
      RETURN( ERRORCODE );

    default:
      mudlog( LOG_DEBUG , "mprog_do_ifchck: %d 'goldamt' 裡有錯誤的參數.", vnum );
      RETURN( ERRORCODE );
    }
  }

  if ( !str_cmp( buf, "class" ) )
  {
    switch ( arg[1] )
    {
    case 'i':
      if ( mob && mob->class )
        RETURN( mprog_veval( mob->class->vnum, opr, num ) );

      RETURN( ERRORCODE );

    case 'n':
      if ( actor && actor->class )
        RETURN( mprog_veval( actor->class->vnum, opr, num ) );

      RETURN( ERRORCODE );

    case 't':
      if ( vict && vict->class )
        RETURN( mprog_veval( vict->class->vnum, opr, num ) );
      RETURN( ERRORCODE );

    case 'r':
      if ( rndm && rndm->class )
        RETURN( mprog_veval( rndm->class->vnum, opr, num ) );
      RETURN( ERRORCODE );

    default:
      mudlog( LOG_DEBUG , "mprog_do_ifchck: %d 'class' 裡有錯誤的參數.", vnum );
      RETURN( ERRORCODE );
    }
  }

  if ( !str_cmp( buf, "objtype" ) )
  {
    switch ( arg[1] )
    {
    case 'o':
      if ( obj ) RETURN( mprog_veval( obj->item_type, opr, num ) );
      RETURN( ERRORCODE );

    case 'p':
      if ( v_obj ) RETURN( mprog_veval( v_obj->item_type, opr, num ) );
      RETURN( ERRORCODE );

    default:
      mudlog( LOG_DEBUG , "mprog_do_ifchck: %d 'objtype' 裡有錯誤的參數.", vnum );
      RETURN( ERRORCODE );
    }
  }

  if ( !str_cmp( buf, "objval0" ) )
  {
    switch ( arg[1] )
    {
    case 'o':
      if ( obj ) RETURN( mprog_veval( obj->value[0], opr, num ) );
      RETURN( ERRORCODE );

    case 'p':
      if ( v_obj ) RETURN( mprog_veval( v_obj->value[0], opr, num ) );
      RETURN( ERRORCODE );

    default:
      mudlog( LOG_DEBUG, "mprog_do_ifchck: %d 'objval0' 裡有錯誤的參數.", vnum );
      RETURN( ERRORCODE );
    }
  }

  if ( !str_cmp( buf, "objval1" ) )
  {
    switch ( arg[1] )
    {
    case 'o':
      if ( obj ) RETURN( mprog_veval( obj->value[1], opr, num ) );
      RETURN( ERRORCODE );

    case 'p':
      if ( v_obj ) RETURN( mprog_veval( v_obj->value[1], opr, num ) );
      RETURN( ERRORCODE );

    default:
      mudlog( LOG_DEBUG , "mprog_do_ifchck: %d 'objval1' 裡有錯誤的參數.", vnum );
      RETURN( ERRORCODE );
    }
  }

  if ( !str_cmp( buf, "objval2" ) )
  {
    switch ( arg[1] )
    {
    case 'o':
      if ( obj ) RETURN( mprog_veval( v_obj->value[2], opr, num ) );
      RETURN( ERRORCODE );

    case 'p':
      if ( v_obj ) RETURN( mprog_veval( v_obj->value[2], opr, num ) );
      RETURN( ERRORCODE );

    default:
      mudlog( LOG_DEBUG , "mprog_do_ifchck: %d 'objval2' 有錯誤的參數.", vnum );
      RETURN( ERRORCODE );
    }
  }

  if ( !str_cmp( buf, "objval3" ) )
  {
    switch ( arg[1] )
    {
    case 'o':
      if ( obj ) RETURN( mprog_veval( obj->value[3], opr, num ) );
      RETURN( ERRORCODE );

    case 'p':
      if ( v_obj ) RETURN( mprog_veval( v_obj->value[3], opr, num ) );
      RETURN( ERRORCODE );

    default:
      mudlog( LOG_DEBUG , "mprog_do_ifchck: %d 'objval3' 裡有錯誤的參數.", vnum );
      RETURN( ERRORCODE );
    }
  }

  if ( !str_cmp( buf, "number" ) )
  {
    switch ( arg[1] )
    {
    case 'i':
      RETURN( mprog_veval( mob->gold, opr, num ) );

    case 'n':
      if ( actor && IS_NPC( actor ) )
        RETURN( mprog_veval( actor->pIndexData->vnum, opr, num ) );

      RETURN( ERRORCODE );

    case 't':
      if ( vict && IS_NPC( actor ) )
        RETURN( mprog_veval( vict->pIndexData->vnum, opr, num ) );

      RETURN( ERRORCODE );

    case 'r':
      if ( rndm && IS_NPC( actor ) )
         RETURN( mprog_veval( rndm->pIndexData->vnum, opr, num ) );

      RETURN( ERRORCODE );

    case 'o':
      if ( obj ) RETURN( mprog_veval( obj->pIndexData->vnum, opr, num ) );
      RETURN( ERRORCODE );

    case 'p':
      if ( v_obj ) RETURN( mprog_veval( v_obj->pIndexData->vnum, opr, num ) );
      RETURN( ERRORCODE );

    default:
      mudlog( LOG_DEBUG , "mprog_do_ifchck: %d 'number' 裡有錯誤的參數.", vnum );
      RETURN( ERRORCODE );
    }
  }

  if ( !str_cmp( buf, "name" ) )
  {
    switch ( arg[1] )
    {
    case 'i':
      RETURN( mprog_seval( mob->name, opr, val ) );

    case 'n':
      if ( actor ) RETURN( mprog_seval( actor->name, opr, val ) );
      RETURN( ERRORCODE );

    case 't':
      if ( vict ) RETURN( mprog_seval( vict->name, opr, val ) );
      RETURN( ERRORCODE );

    case 'r':
      if ( rndm ) RETURN( mprog_seval( rndm->name, opr, val ) );
      RETURN( ERRORCODE );

    case 'o':
      if ( obj ) RETURN( mprog_seval( obj->name, opr, val ) );
      RETURN( ERRORCODE );

    case 'p':
      if ( v_obj ) RETURN( mprog_seval( v_obj->name, opr, val ) );
      RETURN( ERRORCODE );

    default:
      mudlog( LOG_DEBUG , "mprog_do_ifchck: %d 'name' 裡有錯誤的參數.", vnum );
      RETURN( ERRORCODE );
    }
  }

  mudlog( LOG_DEBUG , "mprog_do_ifchck: %d 未知的 ifchck %s.", vnum , buf );
  RETURN( ERRORCODE );
}

char * mprog_process_if( char * ifchck, char * com_list, CHAR_DATA * mob,
      CHAR_DATA * actor, OBJ_DATA * obj, void * vo, CHAR_DATA * rndm )
{
  char   buf[ MAX_INPUT_LENGTH ];
  char * morebuf = '\x0';
  char * cmnd = '\x0';
  bool   loopdone = FALSE;
  bool   flag = FALSE;
  int    legal;
  int    vnum;

  PUSH_FUNCTION( "mprog_process_if" );

  if ( !mob->pIndexData )
  {
    mudlog( LOG_DEBUG, "mprog_process_if: 來源錯誤." );
    RETURN( NULL );
  }

  vnum = mob->pIndexData->vnum;

  if ( ( legal = mprog_do_ifchck( ifchck, mob, actor, obj, vo, rndm ) ) )
  {
    if ( legal == TRUE ) flag = TRUE;
    else                 RETURN( NULL );
  }

  while ( loopdone == FALSE )
  {
    cmnd     = com_list;
    com_list = mprog_next_command( com_list );
    while ( *cmnd == ' ' ) cmnd++;

    if ( *cmnd == '\x0' )
    {
      mudlog( LOG_DEBUG , "mprog_process_if: %d IF/OR 之後沒有命令.", vnum );
      RETURN( NULL );
    }

    morebuf = one_argument( cmnd, buf );

    if ( !str_cmp( buf, "or" ) )
    {
      if ( ( legal = mprog_do_ifchck( morebuf, mob, actor, obj, vo, rndm ) ) )
      {
        if ( legal == TRUE ) flag = TRUE;
        else                 RETURN( NULL );
      }
    }
    else
    {
      loopdone = TRUE;
    }
  }

  if ( flag )
  {
    for ( ;; )
    {
      if ( !str_cmp( buf, "if" ) )
      {
        com_list = mprog_process_if(morebuf,com_list,mob,actor,obj,vo,rndm);

        if ( com_list == NULL ) RETURN( NULL );
        while ( *cmnd == ' ' ) cmnd++;
        if ( *com_list == '\x0' ) RETURN( NULL );

        cmnd     = com_list;
        com_list = mprog_next_command( com_list );
        morebuf  = one_argument( cmnd,buf );
        continue;
      }

      if ( !str_cmp( buf, "break" ) ) RETURN( NULL );
      if ( !str_cmp( buf, "endif" ) ) RETURN( com_list );
      if ( !str_cmp( buf, "else" ) )
      {
        while ( str_cmp( buf, "endif" ) )
        {
          cmnd     = com_list;
          com_list = mprog_next_command( com_list );
          while ( *cmnd == ' ' ) cmnd++;

          if ( *cmnd == '\x0' )
          {
            mudlog( LOG_DEBUG , "mprog_process_if: %d else 後面沒有 endif.", vnum );
            RETURN( NULL );
          }
          morebuf = one_argument( cmnd,buf );
        }
        RETURN( com_list );
      }

      mprog_process_cmnd( cmnd, mob, actor, obj, vo, rndm );

      cmnd     = com_list;
      com_list = mprog_next_command( com_list );
      while ( *cmnd == ' ' ) cmnd++;

      if ( *cmnd == '\x0' )
      {
        mudlog( LOG_DEBUG , "mprog_process_if: %d 缺 else 或是 endif", vnum );
        RETURN( NULL );
      }
      morebuf = one_argument( cmnd, buf );
    }
  }

  else
  {
    while ( ( str_cmp( buf, "else" ) ) && ( str_cmp( buf, "endif" ) ) )
    {
      cmnd     = com_list;
      com_list = mprog_next_command( com_list );
      while ( *cmnd == ' ' ) cmnd++;

      if ( *cmnd == '\x0' )
      {
        mudlog( LOG_DEBUG , "mprog_process_if: %d 缺 else 或是 endif", vnum );
        RETURN( NULL );
      }
      morebuf = one_argument( cmnd, buf );
    }

    if ( !str_cmp( buf, "endif" ) ) RETURN( com_list );

    cmnd     = com_list;
    com_list = mprog_next_command( com_list );
    while ( *cmnd == ' ' ) cmnd++;

    if ( *cmnd == '\x0' )
    {
      mudlog( LOG_DEBUG , "mprog_process_if: %d 缺 endif", vnum );
      RETURN( NULL );
    }
    morebuf = one_argument( cmnd, buf );

    for ( ;; )
    {
      if ( !str_cmp( buf, "if" ) )
      {
        com_list = mprog_process_if( morebuf, com_list, mob, actor
                                     , obj, vo, rndm );

        if ( com_list == NULL ) RETURN( NULL );

        while ( *cmnd == ' ' ) cmnd++;
        if ( *com_list == '\x0' ) RETURN( NULL );

        cmnd     = com_list;
        com_list = mprog_next_command( com_list );
        morebuf  = one_argument( cmnd,buf );
        continue;
      }

      if ( !str_cmp( buf, "else" ) )
      {
        mudlog( LOG_DEBUG , "mprog_process_if: %d else 裡面還有不合法的 else.", vnum );
        RETURN( NULL );
      }

      if ( !str_cmp( buf, "break" ) ) RETURN( NULL );
      if ( !str_cmp( buf, "endif" ) ) RETURN( com_list );

      mprog_process_cmnd( cmnd, mob, actor, obj, vo, rndm );
      cmnd     = com_list;
      com_list = mprog_next_command( com_list );
      while ( *cmnd == ' ' ) cmnd++;

      if ( *cmnd == '\x0' )
      {
        mudlog( LOG_DEBUG , "mprog_process_if: %d else 後面少一個 endif.", vnum );
        RETURN( NULL );
      }
      morebuf = one_argument( cmnd, buf );
    }
  }
}

void mprog_translate( char ch, char * t, CHAR_DATA * mob, CHAR_DATA * actor,
                    OBJ_DATA * obj, void * vo, CHAR_DATA * rndm )
{
  extern char * he_she[];
  extern char * his_her[];
  CHAR_DATA   * vict  = ( CHAR_DATA * ) vo;
  CHAR_DATA   * rch;
  OBJ_DATA    * v_obj = ( OBJ_DATA  * ) vo;
  char          name[MAX_INPUT_LENGTH];
  char          buf[MAX_STRING_LENGTH];
  int           number;

  *t = '\x0';

  PUSH_FUNCTION( "mprog_translate" );

  switch ( ch )
  {
  case '1':
    str_cpy( t, "\e[1;31m" );
    break;

  case '2':
    str_cpy( t, "\e[1;32m" );
    break;

  case '3':
    str_cpy( t, "\e[1;33m" );
    break;

  case '4':
    str_cpy( t, "\e[1;34m" );
    break;

  case '5':
    str_cpy( t, "\e[1;35m" );
    break;

  case '6':
    str_cpy( t, "\e[1;36m" );
    break;

  case '7':
    str_cpy( t, "\e[1;37m" );
    break;

  case '0':
    str_cpy( t, "\e[0m" );
    break;

  case 'i':

    if ( !mob ) mudlog( LOG_DEBUG, "mprog_translate: 缺乏來源.(i)" );
    else        one_argument( mob->name, t );

    break;

  case 'I':

    if ( !mob ) mudlog( LOG_DEBUG, "mprog_translate: 缺乏來源.(I)" );
    else        str_cpy( t, mob_name( NULL, mob ) );

    break;

  case 'n':

    if ( !actor )
    {
      mudlog( LOG_DEBUG, "mprog_translate: $n 缺乏主格." );
    }

    else if ( actor->in_room )
    {
      one_argument( actor->name, name );
      number = 0;
      str_cpy( buf, name );

      for ( rch = actor->in_room->people; rch; rch = rch->next_in_room )
      {
        if ( can_see( mob, rch ) && is_name( name, rch->name ) ) number++;

        if ( rch == actor )
        {
          if ( number > 1 ) sprintf( buf, "%d.%s", number, name );
          break;
        }
      }

      str_cpy( t, buf );
    }
    else
    {
      mudlog( LOG_DEBUG, "mprog_translate: $n 找不到適當的人物." );
    }

    break;

  case 'N':

    if ( actor )
    {
      if ( can_see( mob, actor ) )
      {
        if ( IS_NPC( actor ) )
        {
          str_cpy( t, mob_name( NULL, actor ) );
        }
        else
        {
          if ( IS_SET( actor->act, PLR_EXACT ) )
            str_cpy( t, mob_name( NULL, actor ) );
          else
            sprintf( t, "%s\e[0m", actor->cname );
        }
      }
      else
      {
        str_cpy( t, "某不知名人物" );
      }
    }
    else
    {
      mudlog( LOG_DEBUG, "mprog_translate: $N 找不到適當的人物." );
    }

    break;

  case 't':

    if ( !vict )
    {
      mudlog( LOG_DEBUG, "mprog_translate: $t 缺乏受格." );
    }

    else if ( vict->in_room )
    {
      one_argument( vict->name, name );
      number = 0;
      str_cpy( buf, name );

      for ( rch = vict->in_room->people; rch; rch = rch->next_in_room )
      {
        if ( can_see( mob, rch ) && is_name( name, rch->name ) ) number++;

        if ( rch == vict )
        {
          if ( number > 1 ) sprintf( buf, "%d.%s", number, name );
          break;
        }
      }

      str_cpy( t, buf );
    }
    else
    {
      mudlog( LOG_DEBUG, "mprog_translate: $t 找不到適當的人物." );
    }

    break;

  case 'T':

    if ( vict )
    {
      if ( can_see( mob, vict ) )
      {
        if ( IS_NPC( vict ) )
        {
          str_cpy( t, mob_name( NULL, vict ) );
        }
        else
        {
          if ( IS_SET( vict->act, PLR_EXACT ) )
            str_cpy( t, mob_name( NULL, vict ) );
          else
            sprintf( t, "%s\e[0m", vict->cname );
        }
      }
      else
      {
        str_cpy( t, "某不知名人物" );
      }
    }
    else
    {
      mudlog( LOG_DEBUG, "mprog_translate: $T 找不到適當的人物." );
    }

    break;

  case 'r':
    if ( !rndm )
    {
      mudlog( LOG_DEBUG, "mprog_translate: $r 缺乏隨機者." );
    }

    else if ( rndm->in_room )
    {
      one_argument( rndm->name, name );
      number = 0;
      str_cpy( buf, name );

      for ( rch = rndm->in_room->people; rch; rch = rch->next_in_room )
      {
        if ( can_see( mob, rch ) && is_name( name, rch->name ) ) number++;

        if ( rch == rndm )
        {
          if ( number > 1 ) sprintf( buf, "%d.%s", number, name );
          break;
        }
      }

      str_cpy( t, buf );
    }
    else
    {
      mudlog( LOG_DEBUG, "mprog_translate: $r 找不到適當的人物." );
    }

    break;

  case 'R':

    if ( rndm )
    {
      if ( can_see( mob, rndm ) )
      {
        if ( IS_NPC( rndm ) )
        {
          str_cpy( t, mob_name( NULL, rndm ) );
        }
        else
        {
          if ( IS_SET( rndm->act, PLR_EXACT ) )
            str_cpy( t, mob_name( NULL, rndm ) );
          else
            sprintf( t, "%s\e[0m", rndm->cname );
        }
      }
      else
      {
        str_cpy( t, "某不知名人物" );
      }
    }
    else
    {
      mudlog( LOG_DEBUG, "mprog_translate: $T 找不到適當的人物." );
    }

    break;

  case 'e':
  case 'm':
    if ( actor && mob ) can_see( mob, actor ) ? str_cpy( t, he_she[ actor->sex ] )
                                              : str_cpy( t, "某不知名人物" );
    break;

  case 's':
    if ( actor && mob ) can_see( mob, actor ) ? str_cpy( t, his_her[ actor->sex ] )
                                       : str_cpy( t, "某不知名人物的" );
    break;

  case 'E':
  case 'M':
    if ( vict && mob ) can_see( mob, vict ) ? str_cpy( t, he_she[ vict->sex ] )
                                            : str_cpy( t, "某不知名人物" );
    break;

  case 'S':
    if ( vict && mob ) can_see( mob, vict ) ? str_cpy( t, his_her[ vict->sex ] )
                                            : str_cpy( t, "某不知名人物的" );
    break;

  case 'j':
  case 'k':
    if ( mob ) str_cpy( t, he_she[ mob->sex ] );
    break;

  case 'l':
    if ( mob ) str_cpy( t, his_her[ mob->sex ] );
    break;

  case 'J':
  case 'K':
    if ( rndm && mob ) can_see( mob, rndm ) ? str_cpy( t, he_she[ rndm->sex ] )
                                            : str_cpy( t, "某不知名人物" );
    break;

  case 'L':
    if ( rndm && mob ) can_see( mob, rndm ) ? str_cpy( t, his_her[ rndm->sex ] )
                                            : str_cpy( t, "某不知名人物" );
    break;

  case 'o':
    if ( obj && mob ) can_see_obj( mob, obj ) ? one_argument( obj->name, t )
                                              : str_cpy( t, "某物" );
    break;

  case 'O':
    if ( obj && mob ) can_see_obj( mob, obj ) ? str_cpy( t, obj->cname )
                                              : str_cpy( t, "某物" );
    break;

  case 'p':
    if ( v_obj && mob ) can_see_obj( mob, v_obj ) ? one_argument( v_obj->name, t )
                                                  : str_cpy( t, "某物" );
    break;

  case 'P':
    if ( v_obj && mob ) can_see_obj( mob, v_obj ) ? str_cpy( t, v_obj->cname )
                                                  : str_cpy( t, "某物" );
    break;

  case 'a':
    if ( obj ) str_cpy( t, "一個" );
    break;

  case 'A':
    if ( v_obj ) str_cpy( t, "一個" );
    break;

  case '$':
    str_cpy( t, "$" );
    break;

  default:
    mudlog( LOG_DEBUG , "mprog_translate: %d 錯誤的 $var", mob->pIndexData->vnum );
    break;
  }
  RETURN_NULL();
}

void mprog_process_cmnd( char * cmnd, CHAR_DATA * mob, CHAR_DATA * actor,
                        OBJ_DATA * obj, void * vo, CHAR_DATA * rndm )
{
  char buf[ MAX_INPUT_LENGTH ];
  char tmp[ MAX_INPUT_LENGTH ];
  char *str;
  char *i;
  char *point;

  PUSH_FUNCTION( "mprog_process_cmnd" );

  point = buf;
  str   = cmnd;

  while ( *str != '\x0' )
  {
    if ( *str != '$' )
    {
      *point++ = *str++;
      continue;
    }

    str++;
    *tmp = '\x0';
    mprog_translate( *str, tmp, mob, actor, obj, vo, rndm );
    i = tmp;
    str++;
    while ( ( *point = *i ) != '\x0' ) ++point, ++i;
  }

  if ( point - buf > 1 )
  {
    *point = '\x0';
    interpret( mob, buf );
  }

  RETURN_NULL();
}

void mprog_driver( char * com_list, CHAR_DATA * mob, CHAR_DATA * actor,
                   OBJ_DATA * obj, void * vo )
{
  char        tmpcmndlst[ MAX_STRING_LENGTH ];
  char        buf       [ MAX_INPUT_LENGTH ];
  char      * morebuf;
  char      * command_list;
  char      * cmnd;
  CHAR_DATA * rndm = NULL;
  CHAR_DATA * vch  = NULL;
  int        count = 0;

  PUSH_FUNCTION( "mprog_driver" );

  if ( !mob || !verify_char( mob ) || !mob->in_room )
  {
    mudlog( LOG_DEBUG, "mprog_driver: 缺乏來源." );
    RETURN_NULL();
  }

  if ( is_affected( mob, SLOT_CHARM_PERSON ) || mob->disable_prog )
    RETURN_NULL();

  /* 此時進入 mob_prog */
  mob->inprocess = TRUE;

  /* 隨便找出一個人 */
  for ( vch = mob->in_room->people; vch; vch = vch->next_in_room )
  {
    if ( !IS_NPC( vch ) && vch->level < LEVEL_IMMORTAL && can_see( mob, vch ) )
    {
      if ( number_range( 0, count ) == 0 ) rndm = vch;
      count++;
    }
  }

  if ( str_len( com_list ) > sizeof( tmpcmndlst ) - 1 )
  {
    mob->inprocess = FALSE;
    mudlog( LOG_DEBUG, "mprog_driver: 命令過長." );
    RETURN_NULL();
  }

  str_cpy( tmpcmndlst, com_list );
  command_list = tmpcmndlst;
  cmnd         = command_list;
  command_list = mprog_next_command( command_list );

  while ( *cmnd != '\x0' )
  {
    morebuf = one_argument( cmnd, buf );

    /* if 的命令 */
    if ( !str_cmp( buf, "if" ) )
    {
      command_list =
        mprog_process_if( morebuf, command_list, mob, actor, obj, vo, rndm );

      if ( command_list == NULL )
      {
        mob->inprocess = FALSE;
        RETURN_NULL();
      }
    }

    else
    {
      mprog_process_cmnd( cmnd, mob, actor, obj, vo, rndm );
    }

    cmnd         = command_list;
    command_list = mprog_next_command( command_list );
  }

  mob->inprocess = FALSE;
  RETURN_NULL();
}

void mprog_wordlist_check( char * arg, CHAR_DATA * mob, CHAR_DATA * actor,
                          OBJ_DATA * obj, void * vo, int type )
{
  char         temp1[ MAX_STRING_LENGTH ];
  char         temp2[ MAX_INPUT_LENGTH ];
  char         word[ MAX_INPUT_LENGTH ];
  MPROG_DATA * mprg;
  char       * list;
  char       * start;
  char       * dupl;
  char       * end;
  int          i;

  PUSH_FUNCTION( "mprog_wordlist_check" );

  for ( mprg = mob->pIndexData->mobprogs; mprg; mprg = mprg->next )
  {
    if ( mprg->type & type )
    {
      str_cpy( temp1, mprg->arglist );

      for ( list = temp1, i = 0; i < (int) str_len( list ); i++ )
        list[i] = LOWER( list[i] );

      str_cpy( temp2, arg );

      for ( dupl = temp2, i = 0; i < (int) str_len( dupl ); i++ )
        dupl[i] = LOWER( dupl[i] );

      if ( ( list[0] == 'p' ) && ( list[1] == ' ' ) )
      {
        list += 2;
        while ( ( start = strstr( dupl, list ) ) )
        {
          if ( ( start == dupl || *( start - 1 ) == ' ' )
            && ( *( end = start + str_len( list ) ) == ' '
              || *end == '\n' || *end == '\r' || *end == '\x0' ) )
          {
            mprog_driver( mprg->comlist, mob, actor, obj, vo );
            break;
          }
          else
          {
            dupl = start+1;
          }
        }
      }
      else
      {
        list = one_argument( list, word );
        for( ; word[0] != '\x0'; list = one_argument( list, word ) )
        {
          while ( ( start = strstr( dupl, word ) ) )
          {
            if ( ( start == dupl || *( start - 1 ) == ' ' )
              && ( *( end = start + str_len( word ) ) == ' '
                || *end == '\n'
                || *end == '\r'
                || *end == '\x0' ) )
            {
              mprog_driver( mprg->comlist, mob, actor, obj, vo );
              break;
            }
            else
            {
              dupl = start+1;
            }
          }
        }
      }
    }
  }
  RETURN_NULL();
}

void mprog_percent_check( CHAR_DATA * mob, CHAR_DATA * actor, OBJ_DATA * obj,
                         void * vo, int type )
{
 MPROG_DATA * mprg;

 PUSH_FUNCTION( "mprog_percent_check" );

 for ( mprg = mob->pIndexData->mobprogs; mprg; mprg = mprg->next )
 {
   if ( ( mprg->type & type) && ( number_percent() < atoi( mprg->arglist ) ) )
   {
     mprog_driver( mprg->comlist, mob, actor, obj, vo );
     if ( type != GREET_PROG && type != ALL_GREET_PROG ) break;
   }
 }
 RETURN_NULL();
}

void mprog_act_trigger( char * buf, CHAR_DATA * mob, CHAR_DATA * ch,
                       OBJ_DATA * obj, void * vo)
{
  MPROG_ACT_LIST * tmp_act;

  PUSH_FUNCTION( "mprog_act_trigger" );

  if ( IS_NPC( mob ) && ( mob->pIndexData->progtypes & ACT_PROG ) )
  {
    tmp_act = alloc_struct( STRUCT_MPROG_ACT_LIST );

    if ( mob->mpactnum > 0 ) tmp_act->next = mob->mpact->next;
    else                     tmp_act->next = NULL;

    mob->mpact      = tmp_act;
    mob->mpact->buf = str_dup( buf );
    mob->mpact->ch  = ch;
    mob->mpact->obj = obj;
    mob->mpact->vo  = vo;
    mob->mpactnum++;
  }
  RETURN_NULL();
}

void mprog_bribe_trigger( CHAR_DATA * mob, CHAR_DATA * ch, int amount )
{
  char         buf[ MAX_STRING_LENGTH ];
  MPROG_DATA * mprg;
  MPROG_DATA * mprg_now;
  OBJ_DATA   * obj;

  PUSH_FUNCTION( "mprog_bribe_trigger" );

  /* 當有 bribe_prog 才會進入 */
  if ( IS_NPC( mob ) && ( mob->pIndexData->progtypes & BRIBE_PROG ) )
  {
    /* 創造錢的物品 */
    obj = create_object( ObjMoneySome, 1 );
    sprintf( buf, obj->cname, amount );
    free_string( obj->cname );

    obj->cname    = str_dup( buf );
    obj->value[0] = amount;

    obj_to_char( obj, mob );
    gold_from_char( mob, amount );

    mprg_now = NULL;
    for ( mprg = mob->pIndexData->mobprogs; mprg; mprg = mprg->next )
    {
      if ( ( mprg->type & BRIBE_PROG ) && amount >= atoi( mprg->arglist ) )
        mprg_now = mprg;
    }

    if ( !mprg_now ) RETURN_NULL();

    /* 執行最符合的怪物程式 */
    mprog_driver( mprg_now->comlist, mob, ch, obj, NULL );

  }
  RETURN_NULL();
}

void mprog_death_trigger( CHAR_DATA * mob, CHAR_DATA * victim )
{
  PUSH_FUNCTION( "mprog_death_trigger" );

  if ( IS_NPC( mob ) && ( mob->pIndexData->progtypes & DEATH_PROG ) )
    mprog_percent_check( mob, victim, NULL, NULL, DEATH_PROG );

  death_cry( mob );
  RETURN_NULL();
}

void mprog_entry_trigger( CHAR_DATA * mob )
{
  PUSH_FUNCTION( "mprog_entry_trigger" );

  if ( IS_NPC( mob ) && ( mob->pIndexData->progtypes & ENTRY_PROG ) )
    mprog_percent_check( mob, NULL, NULL, NULL, ENTRY_PROG );

  RETURN_NULL();
}

void mprog_fight_trigger( CHAR_DATA * mob, CHAR_DATA * ch )
{
  PUSH_FUNCTION( "mprog_fight_trigger" );

  if ( IS_NPC( mob ) && ( mob->pIndexData->progtypes & FIGHT_PROG ) )
    mprog_percent_check( mob, ch, NULL, NULL, FIGHT_PROG );

  RETURN_NULL();
}

void mprog_give_trigger( CHAR_DATA * mob, CHAR_DATA * ch, OBJ_DATA * obj )
{
  char         arg[MAX_INPUT_LENGTH];
  MPROG_DATA * mprg;

  PUSH_FUNCTION( "mprog_give_trigger" );

  if ( IS_NPC( mob ) && ( mob->pIndexData->progtypes & GIVE_PROG ) )
  {
    for ( mprg = mob->pIndexData->mobprogs; mprg; mprg = mprg->next )
    {
      one_argument( mprg->arglist, arg );

      if ( ( mprg->type & GIVE_PROG )
        && ( ( !str_cmp( obj->name, mprg->arglist ) )
        || ( !str_cmp( "all", arg ) ) ) )
      {
        mprog_driver( mprg->comlist, mob, ch, obj, NULL );
        break;
      }
    }
  }
  RETURN_NULL();
}

void mprog_greet_trigger( CHAR_DATA * mob )
{
  CHAR_DATA * vmob;

  PUSH_FUNCTION( "mprog_greet_trigger" );

  for ( vmob = mob->in_room->people; vmob; vmob = vmob->next_in_room )
  {
    if ( IS_NPC( vmob )
      && mob != vmob
      && can_see( vmob, mob )
      && !vmob->fighting
      && IS_AWAKE( vmob )
      && ( vmob->pIndexData->progtypes & GREET_PROG ) )
    {
      mprog_percent_check( vmob, mob, NULL, NULL, GREET_PROG );
    }

    else
    {
      if ( IS_NPC( vmob )
        && !vmob->fighting
        && IS_AWAKE( vmob )
        && ( vmob->pIndexData->progtypes & ALL_GREET_PROG ) )
      {
        mprog_percent_check( vmob, mob, NULL, NULL, ALL_GREET_PROG );
      }
    }
  }
  RETURN_NULL();
}

void mprog_hitprcnt_trigger( CHAR_DATA * mob, CHAR_DATA * ch )
{
  MPROG_DATA * mprg;

  PUSH_FUNCTION( "mprog_hitprcnt_trigger" );

  if ( IS_NPC( mob ) && ( mob->pIndexData->progtypes & HITPRCNT_PROG ) )
  {
    for ( mprg = mob->pIndexData->mobprogs; mprg; mprg = mprg->next )
    {
      if ( ( mprg->type & HITPRCNT_PROG )
        && ( ( 100 * mob->hit / get_curr_hit( mob ) ) < atoi( mprg->arglist ) ) )
      {
        mprog_driver( mprg->comlist, mob, ch, NULL, NULL );
        break;
      }
    }
  }
  RETURN_NULL();
}

void mprog_random_trigger( CHAR_DATA * mob )
{
  PUSH_FUNCTION( "mprog_random_trigger" );

  if ( mob->pIndexData->progtypes & RAND_PROG)
    mprog_percent_check( mob, NULL, NULL, NULL, RAND_PROG);

  RETURN_NULL();
}

void mprog_speech_trigger( char * txt, CHAR_DATA * mob )
{
  CHAR_DATA * vmob;

  PUSH_FUNCTION( "mprog_speech_trigger" );

  for ( vmob = mob->in_room->people; vmob; vmob = vmob->next_in_room )
  {
    if ( IS_NPC( vmob ) && ( vmob->pIndexData->progtypes & SPEECH_PROG ) )
      mprog_wordlist_check( txt, vmob, mob, NULL, NULL, SPEECH_PROG );
  }

  RETURN_NULL();
}

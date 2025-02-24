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
#include "merc.h"

void set_enemy( CHAR_DATA * ch, char * city )
{
  ENEMY_DATA * pEnemy;

  PUSH_FUNCTION( "set_enemy" );

  if ( !ch || !city || !*city )
  {
    mudlog( LOG_DEBUG, "set_enemy: 來源有問題." );
    RETURN_NULL();
  }

  if ( IS_NPC( ch ) )
  {
    mudlog( LOG_DEBUG, "set_enemy: 對象是非玩家 %s.", ch->name );
    RETURN_NULL();
  }

  /* 避免存檔時有 ~ 這個符號 */
  smash_tilde( city );

  /* 檢查是否有重複 */
  for ( pEnemy = ch->enemy; pEnemy; pEnemy = pEnemy->next )
    if ( !str_cmp( city, pEnemy->city ) ) RETURN_NULL();

  pEnemy       = alloc_struct( STRUCT_ENEMY_DATA );
  pEnemy->city = str_dup( city );
  pEnemy->next = ch->enemy;
  ch->enemy    = pEnemy;

  RETURN_NULL();
}

bool check_enemy( CHAR_DATA * ch, const char * city )
{
  ENEMY_DATA * pEnemy;

  PUSH_FUNCTION( "check_enemy" );

  if ( !ch || !city || !*city )
  {
    mudlog( LOG_DEBUG, "check_enemy: 來源有錯誤." );
    RETURN( FALSE );
  }

  if ( IS_NPC( ch ) )
  {
    mudlog( LOG_DEBUG, "check_enemy: 對象是非玩家 %s.", ch->name );
    RETURN( FALSE );
  }

  for ( pEnemy = ch->enemy; pEnemy; pEnemy = pEnemy->next )
    if ( !str_cmp( pEnemy->city, city ) ) RETURN( TRUE );

  RETURN( FALSE );
}

void rem_enemy( CHAR_DATA * ch, const char * city )
{
  ENEMY_DATA * pEnemy;
  ENEMY_DATA * prev;

  PUSH_FUNCTION( "rem_enemy" );

  if ( !ch || !city || !*city )
  {
    mudlog( LOG_DEBUG, "rem_enemy: 來源不正確." );
    RETURN_NULL();
  }

  for ( prev = NULL, pEnemy = ch->enemy; pEnemy; pEnemy = pEnemy->next )
  {
    if ( !str_cmp( pEnemy->city , city ) )
    {
      if ( !prev ) ch->enemy  = ch->enemy->next;
      else         prev->next = pEnemy->next;

      free_string( pEnemy->city );
      free_struct( pEnemy, STRUCT_ENEMY_DATA );
      RETURN_NULL();
    }

    prev = pEnemy;
  }

  mudlog( LOG_DEBUG, "rem_enemy: 找不到標記 %s.", city );
  RETURN_NULL();
}

bool is_enemy( CHAR_DATA * ch, char * opr, char * val )
{
  char arg[MAX_INPUT_LENGTH];
  bool found;

  PUSH_FUNCTION( "is_enemy" );

  if ( IS_NPC( ch ) )
  {
    mudlog( LOG_DEBUG, "is_enemy: 對象是非玩家." );
    RETURN( FALSE );
  }

  one_argument( val, arg );
  if ( arg[0] == '\x0' || !opr || !*opr )
  {
    mudlog( LOG_DEBUG, "is_enemy: 沒有引數." );
    RETURN( FALSE );
  }

  found = check_enemy( ch, arg );

  if ( !strcmp( opr, "!=" ) ) RETURN( !found );
  if ( !strcmp( opr, "==" ) ) RETURN( found );

  mudlog( LOG_DEBUG, "is_enemy: 錯誤的運算元 %s.", opr );
  RETURN( FALSE );
}

FUNCTION( do_mpsetenemy )
{
  CHAR_DATA * victim;
  char        arg1[MAX_INPUT_LENGTH];
  char        arg2[MAX_INPUT_LENGTH];
  int         vnum;

  PUSH_FUNCTION( "do_mpsetenemy" );

  if ( !verify_char( ch ) ) RETURN_NULL();

  vnum     = ch->pIndexData->vnum;
  argument = one_argument( argument, arg1 );
             one_argument( argument, arg2 );

  if ( arg1[0] == '\x0' || arg2[0] == '\x0' )
  {
    mudlog( LOG_DEBUG, "do_mpsetenemy: MOB %d 引數錯誤.", vnum );
    RETURN_NULL();
  }

  if ( !( victim = get_char_room( ch, arg1 ) ) )
  {
    mudlog( LOG_DEBUG, "do_mpsetenemy: MOB %d 對象 %s 不存在.", vnum, arg1 );
    RETURN_NULL();
  }

  if ( !IS_NPC( victim ) && !check_enemy( victim, arg2 ) )
    set_enemy( victim, arg2 );

  RETURN_NULL();
}

FUNCTION( do_mpremenemy )
{
  CHAR_DATA * victim;
  char        arg1[MAX_INPUT_LENGTH];
  char        arg2[MAX_INPUT_LENGTH];
  int         vnum;

  PUSH_FUNCTION( "do_mpremenemy" );

  vnum     = ch->pIndexData->vnum;
  argument = one_argument( argument, arg1 );
             one_argument( argument, arg2 );

  if ( arg1[0] == '\x0' || arg2[0] == '\x0' )
  {
    mudlog( LOG_DEBUG, "do_mpremenemy: MOB %d 引數錯誤.", vnum );
    RETURN_NULL();
  }

  if ( !( victim = get_char_room( ch, arg1 ) ) )
  {
    mudlog( LOG_DEBUG, "do_mpremenemy: MOB %d 對象 %s 不存在." , vnum, arg1 );
    RETURN_NULL();
  }

  if ( !IS_NPC( victim ) && check_enemy( victim, arg2 ) )
    rem_enemy( victim, arg2 );

  RETURN_NULL();
}

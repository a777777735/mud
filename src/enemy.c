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
#include <stdlib.h>
#include "merc.h"

void set_enemy( CHAR_DATA * ch, char * city )
{
  ENEMY_DATA * pEnemy;

  PUSH_FUNCTION( "set_enemy" );

  if ( !ch || !city || !*city )
  {
    mudlog( LOG_DEBUG, "set_enemy: �ӷ������D." );
    RETURN_NULL();
  }

  if ( IS_NPC( ch ) )
  {
    mudlog( LOG_DEBUG, "set_enemy: ��H�O�D���a %s.", ch->name );
    RETURN_NULL();
  }

  /* �קK�s�ɮɦ� ~ �o�ӲŸ� */
  smash_tilde( city );

  /* �ˬd�O�_������ */
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
    mudlog( LOG_DEBUG, "check_enemy: �ӷ������~." );
    RETURN( FALSE );
  }

  if ( IS_NPC( ch ) )
  {
    mudlog( LOG_DEBUG, "check_enemy: ��H�O�D���a %s.", ch->name );
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
    mudlog( LOG_DEBUG, "rem_enemy: �ӷ������T." );
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

  mudlog( LOG_DEBUG, "rem_enemy: �䤣��аO %s.", city );
  RETURN_NULL();
}

bool is_enemy( CHAR_DATA * ch, char * opr, char * val )
{
  char arg[MAX_INPUT_LENGTH];
  bool found;

  PUSH_FUNCTION( "is_enemy" );

  if ( IS_NPC( ch ) )
  {
    mudlog( LOG_DEBUG, "is_enemy: ��H�O�D���a." );
    RETURN( FALSE );
  }

  one_argument( val, arg );
  if ( arg[0] == '\x0' || !opr || !*opr )
  {
    mudlog( LOG_DEBUG, "is_enemy: �S���޼�." );
    RETURN( FALSE );
  }

  found = check_enemy( ch, arg );

  if ( !strcmp( opr, "!=" ) ) RETURN( !found );
  if ( !strcmp( opr, "==" ) ) RETURN( found );

  mudlog( LOG_DEBUG, "is_enemy: ���~���B�⤸ %s.", opr );
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
    mudlog( LOG_DEBUG, "do_mpsetenemy: MOB %d �޼ƿ��~.", vnum );
    RETURN_NULL();
  }

  if ( !( victim = get_char_room( ch, arg1 ) ) )
  {
    mudlog( LOG_DEBUG, "do_mpsetenemy: MOB %d ��H %s ���s�b.", vnum, arg1 );
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
    mudlog( LOG_DEBUG, "do_mpremenemy: MOB %d �޼ƿ��~.", vnum );
    RETURN_NULL();
  }

  if ( !( victim = get_char_room( ch, arg1 ) ) )
  {
    mudlog( LOG_DEBUG, "do_mpremenemy: MOB %d ��H %s ���s�b." , vnum, arg1 );
    RETURN_NULL();
  }

  if ( !IS_NPC( victim ) && check_enemy( victim, arg2 ) )
    rem_enemy( victim, arg2 );

  RETURN_NULL();
}

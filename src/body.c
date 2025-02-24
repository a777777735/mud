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
#include "merc.h"

int eq_max_capcity              = EQ_MAX_CAPCITY;
int eq_min_capcity              = EQ_MIN_CAPCITY;

int random_situs( CHAR_DATA * ch, int location )
{
  SITUS_DATA * pSitus;
  int          chance;
  int          total;
  int          max;

  PUSH_FUNCTION( "random_situs" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "random_situs: �ӷ��O�Ū�." );
    RETURN( 0 );
  }

  if ( location == ATTACK_RANDOM || location == 0 ) location = ~0;

  max   = 0;
  total = 0;

  for ( pSitus = situs_list; pSitus; pSitus = pSitus->next )
  {
    if ( location & pSitus->attack ) max += pSitus->chance;
    total += pSitus->chance;
  }

  if ( max <= 0 )
  {
    max      = total;
    location = ~0;
    mudlog( LOG_DEBUG, "random_situs: �䤣�쳡��." );
  }

  chance = number_range( 1, max );

  for ( pSitus = situs_list; pSitus; pSitus = pSitus->next )
  {
    if ( location & pSitus->attack )
    {
      if ( ( chance -= pSitus->chance ) <= 0 )
      {
        ch->situs = pSitus->location;
        RETURN( pSitus->location );
      }
    }
  }

  mudlog( LOG_DEBUG, "random_situs: �䤣�쳡��." );
  ch->situs = situs_list ? situs_list->location : 0;
  RETURN( ch->situs );
}

int damage_situs( CHAR_DATA * victim, int dam, int location )
{
  CHAR_DATA  * ch;
  CLASS_DATA * pClass;
  SITUS_DATA * pSitus;
  OBJ_DATA   * pObj;
  OBJ_DATA   * zObj;
  int          eq_damage;
  int          char_damage;
  int          percentage;
  int          count;

  PUSH_FUNCTION( "damage_situs" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "damage_situs: �ӷ��O�Ū�." );
    RETURN( -1 );
  }

  if ( dam < 0 )
  {
    mudlog( LOG_DEBUG, "damage_situs: �ˮ`�p�� 0 ( %d ).", dam );
    RETURN( -1 );
  }

  /* ��X�i�H�ɯŪ��˳� */
  zObj  = NULL;
  count = 0;

  for ( pSitus = situs_list; pSitus; pSitus = pSitus->next )
  {
    if ( !( pObj = get_eq_char( victim, pSitus->location ) )
      || !pObj->pIndexData
      || pObj->pIndexData->exp <= 0
      || pObj->level >= victim->level ) continue;

    if ( number_range( 0, count++ ) == 0 ) zObj = pObj;
  }

  /* �w�g���i�H�ɯŪ��˳� */
  if ( zObj && number_range( 0, 10000 ) <= zObj->pIndexData->exp )
  {
    zObj->level = UMIN( victim->level, zObj->level + 1 );
    switch( zObj->item_type )
    {
    default:
      break;

    case ITEM_WEAPON:
      zObj->max_armor += 150;
      zObj->armor     += 150;
      zObj->value[2]++;

      act( "$A$1$n$1���W��$p$1�g�L�@�f���X��M�S��W�@�h�ӤF�T$0$A"
        , victim, zObj, NULL, TO_CHAR );

      break;

    case ITEM_ARMOR:
      zObj->max_armor += 200;
      zObj->armor     += 200;

      act( "$A$1$n$1���W��$p$1�g�L�@�f���X��M�S��W�@�h�ӤF�T$0$A"
        , victim, zObj, NULL, TO_CHAR );

      break;
    }
  }

  if ( ( ch     = victim->fighting )
    && ( pClass = ch->class )
    && pClass->msg_limit
    && pClass->limit >= 0
    && pClass->multiplier >= 1
    && ch->limit >= pClass->limit )
  {
    sact( ch, victim, NULL, pClass->msg_limit );
    ch->limit -= pClass->limit;
    dam *= pClass->multiplier;
  }

  /* ����˳��� */
  while ( situs_lookup( victim->situs ) == NULL )
    random_situs( victim, location );

  /* �S����˳��� */
  if ( !( pObj = get_eq_char( victim, victim->situs ) ) )
  {
    victim->hit -= dam;
    RETURN( dam );
  }

  if ( IS_NPC( victim ) )
  {
    eq_damage   = 1;
    char_damage = dam -1;
  }

  else
  {
    eq_damage   = dam * ( UMIN( UMAX( 1, victim->eq_capcity ) , 100 ) ) / 100;
    char_damage = dam - eq_damage;
  }

  if ( eq_damage >= pObj->armor )
  {
    char_damage += eq_damage - pObj->armor;
    victim->hit -= char_damage;

    act( "�A��$p�L�k�Ө��o��Ӫ��r�P�@���M�H���F��b�T"
      , victim, pObj, NULL, TO_CHAR );
    act( "$n���W��$p�Q�r�P�@���M�H���F��b�T", victim, pObj, NULL, TO_ROOM );

    unequip_char( victim, pObj, FALSE );
    extract_obj( pObj );
  }

  else
  {
    victim->hit -= char_damage;
    pObj->armor -= eq_damage;
    percentage   = pObj->armor * 100 / pObj->max_armor;

    if ( percentage <= 10 )
      act( "$B$1�A��$p�Ӱe�פF�T$0", victim, pObj, NULL, TO_CHAR );
  }

  RETURN( char_damage );
}

/* �ˬd�O�_�������� */
bool is_armor( OBJ_DATA * pObj )
{
  SITUS_DATA * pSitus;

  PUSH_FUNCTION( "is_armor" );

  if ( !pObj )
  {
    mudlog( LOG_DEBUG, "is_armor: �ӷ��O�Ū�." );
    RETURN( FALSE );
  }

 for ( pSitus = situs_list; pSitus; pSitus = pSitus->next )
   if ( CAN_WEAR( pObj, pSitus->wear ) ) RETURN( TRUE );

  RETURN( FALSE );
}

/* �ײz���~ */
FUNCTION( do_repair )
{
  char              arg1[MAX_INPUT_LENGTH];
  char              arg2[MAX_INPUT_LENGTH];
  CHAR_DATA       * pSmith;
  OBJ_DATA        * pObj;
  SHOP_DATA       * pShop;
  EXIT_DATA       * pExit;
  ROOM_INDEX_DATA * to_room;
  int               iType;
  int               cost;
  int               paid;
  int               door;

  PUSH_FUNCTION( "do_repair" );

  /* ����X�f */
  for ( pExit = NULL, door = 0; door < DIR_MAX; door++ )
    if ( ( pExit = ch->in_room->exit[door] ) ) break;

  /* �ݬݬO�_���X�f���ж� */
  to_room = pExit ? pExit->to_room : NULL;

  /* �^���Ѽ� */
  argument = one_argument( argument, arg1 );
             one_argument( argument, arg2 );

  /* �ˬd���W�O�_���o�˪��~ */
  if ( !( pObj = get_obj_carry( ch, arg1 ) ) )
  {
    act( "�A�èS������ $2$T$0�M�A�ڰڡC", ch, NULL, arg1, TO_CHAR );
    RETURN_NULL();
  }

  /* �ݬݬO�_���u��o�ӰѼ� */
  if ( !arg2[0] || ( paid = atoi( arg2 ) ) <= 0 )
  {
    act( "�n��I�I�u����T���M����$p�C", ch, pObj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  /* �ݬݬO�_�I�o�_�u�� */
  if ( ch->gold < paid )
  {
    act( "�A���I�X�u��M�����A��$p�F�C" , ch, pObj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  /* �ݬݬO�_���a���K�K */
  if ( !( pSmith = find_keeper( ch, SHOP_SMITH ) )
    || !pSmith->pIndexData
    || !( pShop = pSmith->pIndexData->pShop ) ) RETURN_NULL();

  /* ���~�O�_�����r */
  if ( poison_char( ch, pObj ) ) RETURN_NULL();

  /* �ݬ��K�K�O�_�O�b�𮧩άO��ı */
  if ( IS_REST( pSmith ) )
  {
    act( "�N�M$N���b�𮧩O�T", ch, NULL, pSmith, TO_CHAR );
    RETURN_NULL();
  }

  /* �Y���A���O���ߤ]���� */
  if ( pSmith->position != POS_STANDING )
  {
    act( "�A�S�ݨ�$N���b���ܡS", ch, NULL, pSmith, TO_CHAR );
    RETURN_NULL();
  }

  if ( is_affected( pSmith, SLOT_CHARM_PERSON ) || pSmith->master )
  {
    act( "$N�߯����áM�L�k�ײz�˳ơT", ch, NULL, pSmith, TO_CHAR );
    RETURN_NULL();
  }

  /* �ݬ��K�K�O�_�i�H�ݪ��� */
  if ( !can_see_obj( pSmith, pObj ) )
  {
    act( "$N�ݤ���o�˪��~�M���M�A�ڡC", ch, NULL, pSmith, TO_CHAR );

    if ( to_room )
    {
      act( "$n�]�A$N�ӳQ���F�X�h�C", ch, NULL, pSmith, TO_ALL );
      char_from_room( ch );
      char_to_room( ch, to_room );

      /* �M���l�ܬ����I */
      clear_trace( ch, TRUE );
    }

    RETURN_NULL();
  }

  if ( pObj->address )
  {
    act( "$p�O�n�浹�O�H���H�M���i�H�����浹�O�H�C", ch, pObj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( !can_drop_obj( ch, pObj ) )
  {
    act( "�]��������$p�M�ҥH�A����ײz���C", ch, pObj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  /* �ݬݬO�_�i�H�ײz�����~ */
  if ( !is_armor( pObj ) )
  {
    act( "$p�i�H�ײz�ܡS�A�T�w�ܡS", ch, pObj, NULL, TO_CHAR );

    if ( to_room )
    {
      act( "$n�]�A$N�ӳQ���F�X�h�C", ch, NULL, pSmith, TO_ALL );
      char_from_room( ch );
      char_to_room( ch, to_room );

      /* �M���l�ܬ����I */
      clear_trace( ch, TRUE );
    }

    RETURN_NULL();
  }

  /* ��M�o���K�K�쩳�|���|�ײz�o�˸˳� */
  for ( iType = 0; iType < MAX_TRADE; iType++ )
    if ( IS_SET( pObj->wear_flags, pShop->buy_type[iType] ) ) break;

  /* �p�G���| */
  if ( iType == MAX_TRADE )
  {
    act( "�ڤ��|�ײz$p�M�A�t�а����a�T", ch, pObj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  /* �Y�O�����w�g�O���ꪺ�˳Ƥ]����, �����Ӧ� */
  if ( pObj->max_armor <= 100 )
  {
    act( "$p�צn�F�]�S���ΡM���פ]�}�M���L���ڦ��F�C", ch, pObj, NULL, TO_CHAR );
    gold_from_char( ch, paid );

    message_driver( ch, pObj, ACT_WHEN_REPAIR );
    RETURN_NULL();
  }

  /* ���ŶV�����˳ƶV���i��Q�S�� */
  if ( number_range( 0, 2000 ) <= pObj->level
    && !IS_IMMORTAL( ch )
    && ( paid <= pObj->cost * 2 ) )
  {
    act( "$n�@��$p�V�h�M���G..$p���M�H����b�T�o..."
      , pSmith, pObj, NULL, TO_ROOM );

    message_driver( ch, pObj, ACT_WHEN_REPAIR );
    extract_obj( pObj );
    RETURN_NULL();
  }

  /* �Y�a�F�|���H�W, �h�̤j���@�O�U�� */
  if ( ( pObj->armor * 100 / pObj->max_armor ) <= 60 )
    pObj->max_armor = ( pObj->armor + pObj->max_armor ) / 2;

  /* �p��u�����u��, �åB���� */
  cost         = UMAX( 0, pObj->max_armor - pObj->armor );
  pObj->armor  = UMIN( pObj->max_armor, pObj->armor + paid );
  gold_from_char( ch, paid );

  message_driver( ch, pObj, ACT_WHEN_REPAIR );

  /* �ھڤ��P���u���X�ԭz */
  if ( paid >= cost )
  {
    do_emote( pSmith, "�@�����ۦ����M���W�a�����K��M�ײz���l���˳ơT" );
    act( "$N�i�D�A�R�צn�F�M���ݧa�C", ch, NULL, pSmith, TO_CHAR );
  }

  else
  {
    do_emote( pSmith, "�H�K�a�ݬݡM�@�ƪ��~�~���ˤl�ײz���l���˳ơT" );
    act( "$N�i�D�A�R���Ӧn�F�M�A�ոլݧa�M���檺�ܦA���a�T"
      , ch, NULL, pSmith, TO_CHAR );
  }

  RETURN_NULL();
}

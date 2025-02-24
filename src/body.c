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
    mudlog( LOG_DEBUG, "random_situs: 來源是空的." );
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
    mudlog( LOG_DEBUG, "random_situs: 找不到部位." );
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

  mudlog( LOG_DEBUG, "random_situs: 找不到部位." );
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
    mudlog( LOG_DEBUG, "damage_situs: 來源是空的." );
    RETURN( -1 );
  }

  if ( dam < 0 )
  {
    mudlog( LOG_DEBUG, "damage_situs: 傷害小於 0 ( %d ).", dam );
    RETURN( -1 );
  }

  /* 找出可以升級的裝備 */
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

  /* 已經找到可以升級的裝備 */
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

      act( "$A$1$n$1身上的$p$1經過一番的焠鍊﹐又更上一層樓了﹗$0$A"
        , victim, zObj, NULL, TO_CHAR );

      break;

    case ITEM_ARMOR:
      zObj->max_armor += 200;
      zObj->armor     += 200;

      act( "$A$1$n$1身上的$p$1經過一番的焠鍊﹐又更上一層樓了﹗$0$A"
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

  /* 取攻傷部位 */
  while ( situs_lookup( victim->situs ) == NULL )
    random_situs( victim, location );

  /* 沒有攻傷部位 */
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

    act( "你的$p無法承受這突來的猛烈一擊﹐碎成了兩半﹗"
      , victim, pObj, NULL, TO_CHAR );
    act( "$n身上的$p被猛烈一擊﹐碎成了兩半﹗", victim, pObj, NULL, TO_ROOM );

    unequip_char( victim, pObj, FALSE );
    extract_obj( pObj );
  }

  else
  {
    victim->hit -= char_damage;
    pObj->armor -= eq_damage;
    percentage   = pObj->armor * 100 / pObj->max_armor;

    if ( percentage <= 10 )
      act( "$B$1你的$p該送修了﹗$0", victim, pObj, NULL, TO_CHAR );
  }

  RETURN( char_damage );
}

/* 檢查是否為防具類 */
bool is_armor( OBJ_DATA * pObj )
{
  SITUS_DATA * pSitus;

  PUSH_FUNCTION( "is_armor" );

  if ( !pObj )
  {
    mudlog( LOG_DEBUG, "is_armor: 來源是空的." );
    RETURN( FALSE );
  }

 for ( pSitus = situs_list; pSitus; pSitus = pSitus->next )
   if ( CAN_WEAR( pObj, pSitus->wear ) ) RETURN( TRUE );

  RETURN( FALSE );
}

/* 修理物品 */
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

  /* 先找出口 */
  for ( pExit = NULL, door = 0; door < DIR_MAX; door++ )
    if ( ( pExit = ch->in_room->exit[door] ) ) break;

  /* 看看是否有出口的房間 */
  to_room = pExit ? pExit->to_room : NULL;

  /* 擷取參數 */
  argument = one_argument( argument, arg1 );
             one_argument( argument, arg2 );

  /* 檢查身上是否有這樣物品 */
  if ( !( pObj = get_obj_carry( ch, arg1 ) ) )
  {
    act( "你並沒有那件 $2$T$0﹐耍我啊。", ch, NULL, arg1, TO_CHAR );
    RETURN_NULL();
  }

  /* 看看是否有工資這個參數 */
  if ( !arg2[0] || ( paid = atoi( arg2 ) ) <= 0 )
  {
    act( "好歹付點工資嘛﹗不然怎麼修$p。", ch, pObj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  /* 看看是否付得起工資 */
  if ( ch->gold < paid )
  {
    act( "你不付出工資﹐不幫你修$p了。" , ch, pObj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  /* 看看是否此地有鐵匠 */
  if ( !( pSmith = find_keeper( ch, SHOP_SMITH ) )
    || !pSmith->pIndexData
    || !( pShop = pSmith->pIndexData->pShop ) ) RETURN_NULL();

  /* 物品是否有餵毒 */
  if ( poison_char( ch, pObj ) ) RETURN_NULL();

  /* 看看鐵匠是否是在休息或是睡覺 */
  if ( IS_REST( pSmith ) )
  {
    act( "噓﹐$N正在休息呢﹗", ch, NULL, pSmith, TO_CHAR );
    RETURN_NULL();
  }

  /* 若狀態不是站立也不行 */
  if ( pSmith->position != POS_STANDING )
  {
    act( "你沒看到$N正在忙嗎﹖", ch, NULL, pSmith, TO_CHAR );
    RETURN_NULL();
  }

  if ( is_affected( pSmith, SLOT_CHARM_PERSON ) || pSmith->master )
  {
    act( "$N心神錯亂﹐無法修理裝備﹗", ch, NULL, pSmith, TO_CHAR );
    RETURN_NULL();
  }

  /* 看看鐵匠是否可以看的到 */
  if ( !can_see_obj( pSmith, pObj ) )
  {
    act( "$N看不到這樣物品﹐竟然耍我。", ch, NULL, pSmith, TO_CHAR );

    if ( to_room )
    {
      act( "$n因耍$N而被趕了出去。", ch, NULL, pSmith, TO_ALL );
      char_from_room( ch );
      char_to_room( ch, to_room );

      /* 清除追蹤紀錄點 */
      clear_trace( ch, TRUE );
    }

    RETURN_NULL();
  }

  if ( pObj->address )
  {
    act( "$p是要交給別人的信﹐怎麼可以輕易交給別人。", ch, pObj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( !can_drop_obj( ch, pObj ) )
  {
    act( "因為不能丟棄$p﹐所以你不能修理它。", ch, pObj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  /* 看看是否可以修理的物品 */
  if ( !is_armor( pObj ) )
  {
    act( "$p可以修理嗎﹖你確定嗎﹖", ch, pObj, NULL, TO_CHAR );

    if ( to_room )
    {
      act( "$n因耍$N而被趕了出去。", ch, NULL, pSmith, TO_ALL );
      char_from_room( ch );
      char_to_room( ch, to_room );

      /* 清除追蹤紀錄點 */
      clear_trace( ch, TRUE );
    }

    RETURN_NULL();
  }

  /* 找尋這位鐵匠到底會不會修理這樣裝備 */
  for ( iType = 0; iType < MAX_TRADE; iType++ )
    if ( IS_SET( pObj->wear_flags, pShop->buy_type[iType] ) ) break;

  /* 如果不會 */
  if ( iType == MAX_TRADE )
  {
    act( "我不會修理$p﹐你另請高明吧﹗", ch, pObj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  /* 若是本身已經是很爛的裝備也不修, 但錢照收 */
  if ( pObj->max_armor <= 100 )
  {
    act( "$p修好了也沒有用﹐不修也罷﹐不過錢我收了。", ch, pObj, NULL, TO_CHAR );
    gold_from_char( ch, paid );

    message_driver( ch, pObj, ACT_WHEN_REPAIR );
    RETURN_NULL();
  }

  /* 等級越高的裝備越有可能被沒收 */
  if ( number_range( 0, 2000 ) <= pObj->level
    && !IS_IMMORTAL( ch )
    && ( paid <= pObj->cost * 2 ) )
  {
    act( "$n一鎚往$p敲去﹐結果..$p竟然碎成兩半﹗這..."
      , pSmith, pObj, NULL, TO_ROOM );

    message_driver( ch, pObj, ACT_WHEN_REPAIR );
    extract_obj( pObj );
    RETURN_NULL();
  }

  /* 若壞了四成以上, 則最大防護力下降 */
  if ( ( pObj->armor * 100 / pObj->max_armor ) <= 60 )
    pObj->max_armor = ( pObj->armor + pObj->max_armor ) / 2;

  /* 計算真正的工資, 並且扣除 */
  cost         = UMAX( 0, pObj->max_armor - pObj->armor );
  pObj->armor  = UMIN( pObj->max_armor, pObj->armor + paid );
  gold_from_char( ch, paid );

  message_driver( ch, pObj, ACT_WHEN_REPAIR );

  /* 根據不同的工資輸出敘述 */
  if ( paid >= cost )
  {
    do_emote( pSmith, "一面擦著汗水﹐辛苦地揮著鐵鎚﹐修理受損的裝備﹗" );
    act( "$N告訴你﹕修好了﹐穿穿看吧。", ch, NULL, pSmith, TO_CHAR );
  }

  else
  {
    do_emote( pSmith, "隨便地看看﹐一副門外漢的樣子修理受損的裝備﹗" );
    act( "$N告訴你﹕應該好了﹐你試試看吧﹐不行的話再說吧﹗"
      , ch, NULL, pSmith, TO_CHAR );
  }

  RETURN_NULL();
}

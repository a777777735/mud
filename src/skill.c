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
#include "merc.h"

void murmur_spell       args( ( CHAR_DATA * , SKILL_DATA * ) );
bool lost_concentration args( ( CHAR_DATA * , int ) );
void driver_kill        args( ( SKILL_DATA *, CHAR_DATA *, CHAR_DATA * ) );
bool learn_expert_skill args( ( CHAR_DATA *, SKILL_DATA * ) );
int  effect_wand        args( ( CHAR_DATA *, SKILL_DATA * ) );

int effect_wand( CHAR_DATA * ch, SKILL_DATA * pSkill )
{
  OBJ_DATA   * pObj;

  PUSH_FUNCTION( "effect_wand" );

  if ( !ch || !pSkill || !verify_char( ch ) ) RETURN( ERRORCODE );

  switch( pSkill->rating )
  {
  case RATING_SKILL:
  case RATING_LOST:
  case RATING_CURE:
  case RATING_SING:
  case RATING_FIGHT:
  case RATING_MURDER:
  case RATING_CREATE:
  case RATING_THIEF:
    RETURN( 100 );

  default:
    break;
  }

  for ( pObj = ch->carrying; pObj; pObj = pObj->next_content )
  {
    if ( pObj->wear_loc == WEAR_WIELD
      && ( pObj->item_type == ITEM_STAFF || pObj->item_type == ITEM_WAND ) )
      break;
  }

  if ( pObj )
  {
    if ( pSkill->rating == pObj->value[4] )
    {
      if ( pObj->value[6] < pObj->value[7] )
      {
        act( "$p的法力消耗殆盡﹐無法再輔助你了﹗", ch, pObj, NULL, TO_CHAR );
        extract_obj( pObj );
        RETURN( 100 );
      }

      act( "$3$n手上的$p發出了一陣耀眼的光芒﹐隨即又恢復正常﹗$0"
        , ch, pObj, NULL, TO_ALL );

      pObj->value[6] = UMAX( 0, pObj->value[6] - pObj->value[7] );
      RETURN( pObj->value[5] );
    }
    else
    {
      if ( pObj->value[4] == RATING_ALL )
      {
        if ( pObj->value[6] < pObj->value[7] )
        {
          act( "$p的法力消耗殆盡﹐無法再輔助你了﹗", ch, pObj, NULL, TO_CHAR );
          extract_obj( pObj );
          RETURN( 100 );
        }

        act( "$3$n手上的$p發出了一陣耀眼的光芒﹐隨即又恢復正常﹗$0"
          , ch, pObj, NULL, TO_ALL );

        pObj->value[6] = UMAX( 0, pObj->value[6] - pObj->value[7] );
        RETURN( pObj->value[5] );
      }

      RETURN( 100 );
    }
  }
  else
  {
    RETURN( 100 );
  }

  RETURN( ERRORCODE );
}

FUNCTION( do_llink )
{
  char         arg[MAX_INPUT_LENGTH];
  int          count = 0;
  SKILL_DATA * pSkill;

  PUSH_FUNCTION( "do_llink" );

  one_argument( argument, arg );

  if ( !arg[0] )
  {
    send_to_char( "你要查詢哪一個技能的連結﹖\n\r", ch );
    RETURN_NULL();
  }

  if ( !( pSkill = skill_isname( arg ) ) )
  {
    act( "找不到你要的法術或是技能名稱 $2$T$0。", ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  if ( !( get_skill( pSkill->associate ) ) )
  {
    act( "$w沒有相關的連結技能。", ch, pSkill, NULL, TO_CHAR );
    RETURN_NULL();
  }

  for ( clear_buffer(); pSkill; pSkill = get_skill( pSkill->associate ) )
  {
    send_to_buffer( "順序﹕%2d 機率﹕%4d%%%% %-20s %-20s\n\r"
      , ++count, pSkill->degree, pSkill->name, pSkill->cname );
  }

  print_buffer( ch );
  RETURN_NULL();
}

FUNCTION( do_cast )
{
  CHAR_DATA     * victim;
  OBJ_DATA      * obj;
  SKILL_DATA    * pSkill;
  RESTRICT_DATA * pRest;
  LIMIT_DATA    * pLimit;
  void          * vo;
  char          * pString;
  char            arg1[MAX_INPUT_LENGTH ];
  char            arg2[MAX_INPUT_LENGTH ];
  int             sn;
  int             cost;
  int             adept;
  int             prac;
  int             prac_point;
  bool            attack;
  bool            gain = TRUE;
  bool            pFind = FALSE;

  PUSH_FUNCTION( "do_cast" );

  /* 如果虛擬怪物被迷惑則不能施展法術或技能 */
  if ( IS_NPC( ch ) && is_affected( ch, SLOT_CHARM_PERSON ) )
    RETURN_NULL();

  if ( !ch->in_room ) RETURN_NULL();

  pString = argument = one_argument( argument , arg1 );
  one_argument( argument , arg2 );

  /* 沒有任何的參數 */
  if ( !arg1[0] )
  {
    send_to_char( "你要施展什麼技能呢﹖\n\r" , ch );
    RETURN_NULL();
  }

  /* 先檢查是否可以施展這樣法術 */
  pSkill = skill_isname( arg1 );

  if ( !pSkill || ( !IS_NPC( ch ) && ch->skill[pSkill->slot] <= 0 ) )
  {
    send_to_char( "你還不會這項技能吧﹗\n\r" , ch );
    RETURN_NULL();
  }

  if ( is_dead( ch ) )
  {
    act( "你已經死了, 無法施展$w.", ch, pSkill, NULL, TO_CHAR );
    RETURN_NULL();
  }

  /* 技能性別的限制 */
  switch( pSkill->sex )
  {
  default:
    break;

  case SEX_FEMALE:
  case SEX_MALE:
  case SEX_NEUTRAL:

    if ( ch->sex != pSkill->sex )
    {
      send_to_char( "你不能使用不是你性別的技能﹗\n\r", ch );
      RETURN_NULL();
    }
    break;
  }

  /* 檢查技能的限制 */
  for ( pRest = pSkill->restrict; pRest; pRest = pRest->next )
  {
    switch( pRest->type )
    {
    default:
      if ( !check_restrict( ch, pRest, TRUE )
        && !IS_IMMORTAL( ch )
        && !IS_NPC( ch ) )
        RETURN_NULL();
      break;

    case RES_SKILL:
      break;

    case RES_CLASS:
    case RES_TRUST:
    case RES_LEVEL:
    case RES_SEX:
    case RES_NOSKILL:

      if ( !check_restrict( ch, pRest, FALSE )
        && !IS_IMMORTAL( ch )
        && !IS_NPC( ch ) )
      {
        act( "$B$B你已經漸漸地遺忘$1$w$0這項技能了﹗", ch, pSkill, NULL, TO_CHAR );
        cancel_skill( ch, pSkill );
        RETURN_NULL();
      }
    }
  }

  for ( pLimit = pSkill->limit; pLimit; pLimit = pLimit->next )
  {
    if ( pLimit->class == ch->class )
    {
      pFind = TRUE;

      if ( ( ch->level < pLimit->level
        || ch->skill[ pSkill->slot ] > pLimit->adept )
        && !IS_IMMORTAL( ch )
        && !IS_NPC( ch ) )
      {
        act( "你無法使用如此艱深的$w﹗", ch, pSkill, NULL, TO_CHAR );
        RETURN_NULL();
      }
      break;
    }
  }

  if ( pFind == FALSE && !IS_IMMORTAL( ch ) && !IS_NPC( ch ) )
  {
    act( "$w不適合你目前的職業。", ch, pSkill, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( !check_valid_rating( ch, pSkill, TRUE ) ) RETURN_NULL();

  switch( pSkill->rating )
  {
  default: break;

  case RATING_SAINT:
    if ( !IS_GOOD( ch ) && !IS_IMMORTAL( ch ) && !IS_NPC( ch ) )
    {
      act( "你太邪惡了﹐無法使出$w﹗", ch, pSkill, NULL, TO_CHAR );
      RETURN_NULL();
    }
    break;

  case RATING_EVIL:
    if ( !IS_EVIL( ch ) && !IS_IMMORTAL( ch ) && !IS_NPC( ch ) )
    {
      act( "你的心地太善良﹐無法使出$w﹗", ch, pSkill, NULL, TO_CHAR );
      RETURN_NULL();
    }
    break;

  }

  if ( pSkill->cast == FALSE )
  {
    act( "$w是無法用這種方法施展。", ch, pSkill, NULL, TO_CHAR );
    RETURN_NULL();
  }

  /* 必須有合理的狀態才能施展法術 */
  if ( ch->position < pSkill->position )
  {
    act( "你無法專心施展$w。", ch, pSkill, NULL, TO_CHAR );
    RETURN_NULL();
  }

  /* 定位目標 */
  victim = NULL;
  obj    = NULL;
  vo     = NULL;
  sn     = pSkill->slot;

  /* 計算消耗的數值量 */
  cost   = pSkill->cost;
  adept  = UMIN( 100, UMAX( ch->skill[sn], 1 ) );
  cost   = UMAX( 1, UMAX( cost / 4, cost * adept / 100 ) );
  attack = TRUE;

  /* 檢查是否需要第二個參數 */
  switch ( pSkill->type )
  {
  default :

    mudlog( LOG_DEBUG , "Do_cast: 法術%s的型態錯誤", pSkill->name );
    RETURN_NULL();

  /* 不需要目標 */
  case TAR_IGNORE:
    break;

  /* 攻擊性法術 */
  case TAR_CHAR_OFFENSIVE:

    if ( !arg2[0] )
    {
      if ( !( victim = ch->fighting ) )
      {
        act( "要對誰施展$w呢﹖", ch, pSkill, NULL, TO_CHAR );
        RETURN_NULL();
      }
    }

    else
    {
      if ( !( victim = get_char_room( ch, arg2 ) ) )
      {
        act( "你的對象 $2$t$0 並不在這裡。", ch, arg2, NULL, TO_CHAR );
        RETURN_NULL();
      }
      else
      {
        if ( victim == ch )
        {
          send_to_char( "人生短短數十年﹐何必自裁﹗\n\r" , ch );
          RETURN_NULL();
        }
      }
    }

    if ( !can_damage( victim ) )
    {
      act( "你無法對$N再進行攻擊了.", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( is_pk( ch ) && is_pk( victim ) )
    {
      if ( fight_info->status != PK_FIGHT )
      {
        send_to_char( "在這段時間﹐還是好好地休息一下吧﹗\n\r", ch );
        RETURN_NULL();
      }
    }

    /* 檢查是否可以發生戰鬥 */
    if ( !can_fight( ch , victim ) ) RETURN_NULL();

    vo = ( void * ) victim;
    break;

  /* 防禦性法術 */
  case TAR_CHAR_DEFENSIVE:

    /* 如果沒有參數對象就是自己 */
    if ( !arg2[0] )
    {
      victim = ch;
    }

    else
    {
      if ( !( victim = get_char_room( ch, arg2 ) ) )
      {
        act( "找不到你的對象 $2$T$0﹗", ch, NULL, arg2, TO_CHAR );
        RETURN_NULL();
      }
    }

    if ( !can_damage( victim ) )
    {
      act( "你無法對$N施展$w.", ch, pSkill, victim, TO_CHAR );
      RETURN_NULL();
    }

    vo = ( void * ) victim;
    break;

  /* 只能施展在自己身上的法術 */
  case TAR_CHAR_SELF:

    if ( arg2[0] && !is_name( arg2, ch->name ) )
    {
      act( "$w只能施展在自己身上。", ch, pSkill, NULL, TO_CHAR );
      RETURN_NULL();
    }

    vo = ( void * ) ch;
    break;

  /* 施展在自己所攜帶的武器上 */
  case TAR_OBJ_INV:

    /* 一定要有參數, 這個參數是裝備的名字 */
    if ( !arg2[0] )
    {
      act( "你要施展$w到你的什麼裝備上呢﹖", ch, pSkill, NULL, TO_CHAR );
      RETURN_NULL();
    }

    /* 看看是否有攜帶這樣的武器 */
    if ( !( obj = get_obj_carry( ch, arg2 ) ) )
    {
      act( "你並沒有攜帶物品 $2$T$0。", ch, NULL, arg2, TO_CHAR );
      RETURN_NULL();
    }

    vo = ( void * ) obj;
    break;

  /* 閃躲技能或是偷窺技 */
  case TAR_DODGE:

    act( "$w只能致能時﹐自動施展﹗", ch, pSkill, NULL, TO_CHAR );
    RETURN_NULL();

  case TAR_ARGUMENT:

    if ( !pString || !*pString )
    {
      act( "對不起﹐$w後面必須加入參數﹗", ch, pSkill, NULL, TO_CHAR );
      RETURN_NULL();
    }

    vo = ( char * ) pString;
    break;

  case TAR_STRING:

    if ( arg2[0] == '\x0' )
    {
      act( "對不起﹐$w後面必須加入參數。", ch, pSkill, NULL, TO_CHAR );
      RETURN_NULL();
    }

    vo = ( char * ) arg2;
    break;

  /* 逃跑技能 */
  case TAR_FLEE:

    if ( !ch->fighting )
    {
      send_to_char( "你沒有在戰鬥為何要逃跑﹖\n\r", ch );
      RETURN_NULL();
    }
    break;

  /* 雜項 */
  case TAR_NO_CAST:

    act( "$w是無法用這種方式施展出來。", ch, pSkill, NULL, TO_CHAR );
    RETURN_NULL();

  /* 騎術 */
  case TAR_MOUNT:

    act( "$w是當你騎馬時﹐才會自動使用﹗", ch, pSkill, NULL, TO_CHAR );
    RETURN_NULL();

  /* 冥想類 */
  case TAR_MEDITATION:

    act( "$w是當在你睡覺時﹐會自動施展﹗", ch, pSkill, NULL, TO_CHAR );
    RETURN_NULL();

  case TAR_OBJ_ROOM:

    /* 一定要有參數, 這個參數是裝備的名字 */
    if ( !arg2[0] || !ch->in_room )
    {
      act( "你要施展$w到地上什麼物品上呢﹖", ch, pSkill, NULL, TO_CHAR );
      RETURN_NULL();
    }

    /* 看看是否有攜帶這樣的武器 */
    if ( !( obj = get_obj_list( ch, arg2, ch->in_room->contents ) ) )
    {
      act( "這裡並沒有 $2$T$0 耶﹗", ch, NULL, arg2, TO_CHAR );
      RETURN_NULL();
    }

    vo = ( void * ) obj;
    break;
  }

  /* 檢查是否有足夠的法力或是體力或是其他....*/
  switch( pSkill->cost_type )
  {
  default :

    mudlog( LOG_DEBUG , "do_cast: 法術 %s 的耗損型態錯誤", pSkill->name );
    RETURN_NULL();

  /* 消耗生命力 */
  case COST_HIT:

    if ( ch->hit < cost )
    {
      act( "你的生命力太低﹐無法施展$w。", ch, pSkill, NULL, TO_CHAR );
      RETURN_NULL();
    }

    break;

  /* 消耗法力 */
  case COST_MANA:

    if ( ch->mana < cost )
    {
      act( "你沒有足夠的法力來施展$w。", ch, pSkill, NULL, TO_CHAR );
      RETURN_NULL();
    }

    break;

  /* 消耗體力 */
  case COST_MOVE:

    if ( ch->move < cost )
    {
      act( "你的體力不濟﹐無法施展$w。", ch, pSkill, NULL, TO_CHAR );
      RETURN_NULL();
    }

    break;

  /* 消耗金錢 */
  case COST_GOLD:

    if ( ch->gold < cost )
    {
      act( "你沒有足夠的銀兩﹐無法施展$w。", ch, pSkill, NULL, TO_CHAR );
      RETURN_NULL();
    }

    break;
  }

  /* 看看是否需要唸出咒語 */
  if ( pSkill->say_spell ) murmur_spell( ch , pSkill );

  /* 法術或是技能的等待時間 */
  WAIT_STATE( ch, pSkill->wait );

  /* 檢查是否有意志不集中的情形 */
  if ( pSkill->concentration == TRUE && lost_concentration( ch, sn ) == TRUE )
  {
    cost   = UMAX( 1, cost / 2 );
    attack = FALSE;
  }

  /* 扣除數值 */
  switch( pSkill->cost_type )
  {
  default :
    mudlog( LOG_DEBUG , "Do_cast: 法術%s的耗損型態錯誤.", pSkill->name );
    RETURN_NULL();

  /* 消耗生命力 */
  case COST_HIT:
    ch->hit = UMAX( 1, ch->hit - cost );
    break;

  /* 消耗法力 */
  case COST_MANA:
    ch->mana = UMAX( 0, ch->mana - cost );
    break;

  /* 消耗體力 */
  case COST_MOVE:
    ch->move = UMAX( 0, ch->move - cost );
    break;

  /* 消耗金錢 */
  case COST_GOLD:
    gold_from_char( ch, cost );
    break;
  }

  /* 檢查是否可以施法 */
  if ( pSkill->check && ( ( *pSkill->check ) ( ch, TRUE ) ) == FALSE )
    RETURN_NULL();

  /* 意志不集中的時候 */
  if ( attack == FALSE )
  {
    act( "你的精神意志不集中﹐$w無法施展。", ch, pSkill, NULL, TO_CHAR );
  }

  /* 真正要施展法術 */
  else
  {
    /* 是否有固定模式 */
    if ( pSkill->damage )
    {
      switch ( pSkill->type )
      {
      default:
        mudlog( LOG_DEBUG, "%s 的沒有固定模式(%d)."
          , pSkill->name, pSkill->type );
        break;

      /* 攻擊性法術 */
      case TAR_CHAR_OFFENSIVE:
        driver_kill( pSkill, ch, ( CHAR_DATA * ) vo );
        break;

      /* 逃跑 */
      case TAR_FLEE:

        driver_flee( pSkill, ch );
        break;
      }
    }

    /* 沒有則呼叫函數來運算 */
    else
    {
      if ( !pSkill->function )
      {
        mudlog( LOG_DEBUG, "do_cast: 技能 %s 沒有操作函數.", pSkill->name );
        RETURN_NULL();
      }

      else
      {
        if ( ( prac = effect_wand( ch, pSkill ) ) == ERRORCODE )
        {
          mudlog( LOG_DEBUG, "do_cast: 無法獲得法杖加成." );
          prac = 100;
        }

        prac_point = ( get_practice( ch, sn ) * prac ) / 100;
        prac_point = number_range( get_practice( ch, sn ), prac_point );
        prac_point = UMAX( 1, UMIN( prac_point, MAX_LEVEL ) );
        gain = ( *pSkill->function ) ( sn, prac_point, ch, vo );
      }
    }
  }

  /* 獲得技能的熟練度 */
  if ( gain ) gain_skill_exp( ch , sn , attack );

  /* 若其中有一者死亡, 則不做下列動作 */
  if ( !can_damage( ch ) || !can_damage( victim ) ) RETURN_NULL();

  /* 對方要知道戰鬥了, 可以出招了, 但是對方不能是自己或是自己的部屬 */
  if ( pSkill->type == TAR_CHAR_OFFENSIVE
    && victim->master != ch
    && victim != ch
    && ch->in_room )
  {
    CHAR_DATA * vch;
    CHAR_DATA * vch_next;

    for ( vch = ch->in_room->people; vch; vch = vch_next )
    {
      vch_next = vch->next_in_room;

      if ( victim == vch && !victim->fighting && can_damage( victim ) )
      {
        striking( victim, ch, TYPE_UNDEFINED );
        break;
      }
    }
  }

  RETURN_NULL();
}

void driver_kill( SKILL_DATA * pSkill, CHAR_DATA * ch, CHAR_DATA * victim )
{
  int           total;
  int           chance;
  int           sn;
  int           practice;
  int           value;
  int           multi;
  int           min;
  DAMAGE_DATA * pDam;
  EFFECT_DATA * pEffect;
  OBJ_DATA    * pObj;

  PUSH_FUNCTION( "driver_kill" );

  if ( !pSkill || !ch || !victim || !verify_char( ch )
    || !verify_char( victim ) )
  {
    mudlog( LOG_DEBUG, "driver_kill: 來源是空的." );
    RETURN_NULL();
  }

  if ( !pSkill->damage )
  {
    mudlog( LOG_DEBUG, "driver_kill: 技能 %s 沒有攻傷敘述.", pSkill->name );
    RETURN_NULL();
  }

  pObj = get_wield( ch );

  if ( pSkill->weapon != ERRORCODE
    && ( !pObj
      || pObj->item_type != ITEM_WEAPON
      || pObj->value[3] != pSkill->weapon ) ) RETURN_NULL();

  if ( ( total = pSkill->chance ) <= 0 )
  {
    mudlog( LOG_DEBUG, "driver_kill: 技能%s的攻傷機率總和小於等於零."
      , pSkill->name );
    RETURN_NULL();
  }

  practice = UMIN( 100, UMAX( 0, get_practice( ch, sn = pSkill->slot ) ) );

  if ( IS_NPC( ch ) )
  {
    chance = UMIN( number_range( 1, practice * total ), total * 100 );
  }
  else
  {
    min = UMAX( 1, ( UMIN( practice, ch->level / 2 ) * total ) );
    chance = UMIN( number_range( min, practice * total ), total * 100 );
  }

  for ( pDam = pSkill->damage; pDam; pDam = pDam->next )
  {
    if ( ( chance -= pDam->chance * 100 ) <= 0 )
    {
      if ( pSkill->message && *pSkill->message )
        sact( ch, victim, pObj, pSkill->message );

      if ( pDam->description && *pDam->description )
        sact( ch , victim , pObj, pDam->description );

      /* 是否有特攻 */
      if ( pDam->routine )
      {
        ( *pDam->routine ) ( ch, victim, practice );

        /* 看看剛剛的動作是否使的雙方死亡 */
        if ( !verify_char( ch )
          || !can_damage( ch )
          || !verify_char( victim )
          || !can_damage( victim )
          || ( pObj && !verify_obj( pObj ) ) )
          RETURN_NULL();
      }

      /* 是否有特殊效應 */
      for ( pEffect = pDam->effect; pEffect; pEffect = pEffect->next )
      {
        /* 呼叫函數操作 */
        effect_driver( ch, victim, pEffect );

        /* 看看剛剛的動作是否使的雙方死亡 */
        if ( !verify_char( ch )
          || !can_damage( ch )
          || !verify_char( victim )
          || !can_damage( victim )
          || ( pObj && !verify_obj( pObj ) ) )
          RETURN_NULL();
      }

      value = pDam->value;

      /* 技能的法器加成 */
      if ( pObj
        && verify_obj( pObj )
        && pDam->obj_vnum == pObj->pIndexData->vnum )
        value = value * pDam->multiple / 100;

      /* 武器的加成 */
      if ( !IS_NPC( ch )
        && pObj
        && pObj->item_type == ITEM_WEAPON
        && pSkill->weapon == pObj->value[3] )
      {
        pObj->value[1] = UMIN( 1000, pObj->value[1] );
        pObj->value[2] = UMIN( 1000, pObj->value[2] );

        multi = number_range( pObj->value[1], pObj->value[2] );
      }
      else
      {
        multi = 100;
      }

      value = variation( ( value * multi / 100 ), 10 );
      damage( ch, victim, value, sn, pDam->situs );

      RETURN_NULL();
    }
  }

  mudlog( LOG_DEBUG, "driver_kill: 找不到敘述." );
  RETURN_NULL();
}

void driver_flee( SKILL_DATA * pSkill, CHAR_DATA * ch )
{
  CHAR_DATA       * victim;
  ROOM_INDEX_DATA * was_in;
  ROOM_INDEX_DATA * now_in;
  EXIT_DATA       * pExit;
  DAMAGE_DATA     * pDam;
  EFFECT_DATA     * pEffect;
  char              buf[MAX_STRING_LENGTH];
  int               total;
  int               chance;
  int               sn;
  int               practice;
  int               door;
  int               attemp;

  PUSH_FUNCTION( "driver_flee" );

  if ( !pSkill || !ch || !( victim = ch->fighting ) )
  {
    mudlog( LOG_DEBUG, "driver_flee: 來源是空的." );
    RETURN_NULL();
  }

  if ( !pSkill->damage )
  {
    mudlog( LOG_DEBUG, "driver_flee: 技能%s沒有攻傷敘述.", pSkill->name );
    RETURN_NULL();
  }

  if ( ( total = pSkill->chance ) <= 0 )
  {
    mudlog( LOG_DEBUG, "driver_flee: 技能%s的攻傷機率總和小於等於零."
      , pSkill->name );
    RETURN_NULL();
  }

  if ( !( was_in = ch->in_room ) )
  {
    mudlog( LOG_DEBUG, "driver_flee: 人物所在房間不存在." );
    RETURN_NULL();
  }

  practice = UMIN( 100, UMAX( 0, get_practice( ch, sn = pSkill->slot ) ) );
  chance   = UMIN( number_range( 1, practice * total ), total * 100 );

  for ( pDam = pSkill->damage; pDam; pDam = pDam->next )
  {
    if ( ( chance -= pDam->chance * 100 ) <= 0 )
    {
      if ( pSkill->message ) sact( ch, victim, NULL, pSkill->message );

      if ( pDam->description && *pDam->description )
        sact( ch, victim, NULL, pDam->description );

      /* 是否有特攻 */
      if ( pDam->routine )
      {
        ( *pDam->routine ) ( ch, victim, practice );

        /* 看看剛剛的動作是否使的雙方死亡 */
        if ( !verify_char( ch )
          || !can_damage( ch )
          || !verify_char( victim )
          || !can_damage( victim ) ) RETURN_NULL();
      }

      /* 是否有特殊效應 */
      for ( pEffect = pDam->effect; pEffect; pEffect = pEffect->next )
      {
        /* 呼叫函數操作 */
        effect_driver( ch, victim, pEffect );

        /* 看看剛剛的動作是否使的雙方死亡 */
        if ( !verify_char( ch )
          || !can_damage( ch )
          || !verify_char( victim )
          || !can_damage( victim ) ) RETURN_NULL();
      }

      /* 先找出口 */
      for ( attemp = pDam->value; attemp > 0; attemp-- )
      {
        if ( number_range( 0, victim->level - pDam->vicevalue ) > ch->level
          || !( pExit = was_in->exit[ ( door = number_door() ) ] )
          || !pExit->to_room
          || pExit->closed
          || ( IS_NPC( ch )
            && ( pExit->to_room->NoMob
              || ( IS_SET( ch->act, ACT_STAY_AREA )
              && pExit->to_room->area != ch->in_room->area ) ) ) )
          continue;

        move_char( ch, door );
        if ( ( now_in = ch->in_room ) == was_in ) continue;

        /* 座騎會迷失 */
        if ( ch->mount ) unmount_char( ch, ch->mount );

        /* 傭兵也會不見 */
        if ( ch->mercenary ) all_mercenary_from_char( ch );

        if ( IS_NPC( ch ) && ch->talk_buf )
        {
          free_string( ch->talk_buf );
          ch->talk_buf  = NULL;
          ch->talk_mode = MODE_ABSENT;
        }

        ch->in_room = was_in;
        act( "$n不敵﹐抱頭鼠竄逃去﹗", ch, NULL, NULL, TO_ROOM );
        ch->in_room = now_in;

        if ( !IS_NPC( ch ) )
        {
          send_to_char("你順利從戰鬥中逃離﹐不過你喪失了25點經驗值﹗\n\r", ch );
          gain_exp( ch, -25 );
        }

        if ( is_pk( ch ) && is_pk( victim ) )
        {
          sprintf( buf, "%s由武鬥大會對決中﹐竟然逃跑﹗", mob_name( NULL, ch ) );
          talk_channel_2( buf, CHANNEL_PK, "" );
        }

        stop_fighting( ch, TRUE );
        RETURN_NULL();
      }

      send_to_char( "你逃跑失敗﹐你喪失 \e[1;33m10\e[0m 點經驗值﹗\n\r", ch );
      gain_exp( ch, -10 );
      RETURN_NULL();
    }
  }

  mudlog( LOG_DEBUG, "driver_flee: 找不到敘述." );
  RETURN_NULL();
}

/* 唸出咒語 */
void murmur_spell( CHAR_DATA * ch , SKILL_DATA * pSkill )
{
  char         buf [MAX_STRING_LENGTH] = { 0 };
  char         buf1[MAX_STRING_LENGTH];
  CHAR_DATA  * rch;
  char       * pName;
  int          iSyl;
  int          length;

  struct syl_type
  {
    char *  old;
    char *  new;
  };

  static const struct syl_type syl_table[] =
  {
    { " ",          " "             },
    { "ar",         "abra"          },
    { "au",         "kada"          },
    { "bless",      "fido"          },
    { "blind",      "nose"          },
    { "bur",        "mosa"          },
    { "cu",         "judi"          },
    { "de",         "oculo"         },
    { "en",         "unso"          },
    { "light",      "dies"          },
    { "lo",         "hi"            },
    { "mor",        "zak"           },
    { "move",       "sido"          },
    { "ness",       "lacri"         },
    { "ning",       "illa"          },
    { "per",        "duda"          },
    { "ra",         "gru"           },
    { "re",         "candus"        },
    { "son",        "sabru"         },
    { "tect",       "infra"         },
    { "tri",        "cula"          },
    { "ven",        "nofo"          },
    { "a", "a" }, { "b", "b" }, { "c", "q" }, { "d", "e" },
    { "e", "z" }, { "f", "y" }, { "g", "o" }, { "h", "p" },
    { "i", "u" }, { "j", "y" }, { "k", "t" }, { "l", "r" },
    { "m", "w" }, { "n", "i" }, { "o", "a" }, { "p", "s" },
    { "q", "d" }, { "r", "f" }, { "s", "g" }, { "t", "h" },
    { "u", "j" }, { "v", "z" }, { "w", "x" }, { "x", "n" },
    { "y", "l" }, { "z", "k" },
    { "", "" }
  };

  PUSH_FUNCTION( "murmur_spell" );

  if ( !ch->in_room || !pSkill->name|| !*pSkill->name ) RETURN_NULL();

  for ( pName = pSkill->name; *pName; pName += length )
  {
    for ( iSyl = 0; (length = str_len(syl_table[iSyl].old)) != 0; iSyl++ )
    {
      if ( !str_prefix( syl_table[iSyl].old, pName ) )
      {
        str_cat( buf, syl_table[iSyl].new );
        break;
      }
    }
    if ( length == 0 ) length = 1;
  }

  sprintf( buf1, "$n口中唸唸有詞說道: '%s'", buf );
  sprintf( buf,  "$n口中唸唸有詞說道: '%s'", pSkill->name );

  /* 送給房間裡的每一個人 */
  for ( rch = ch->in_room->people; rch; rch = rch->next_in_room )
  {
    if ( rch != ch )
      act( is_same_class( rch, ch ) ? buf : buf1, ch, NULL, rch, TO_VICT );
  }
  RETURN_NULL();
}

/* 檢查是否意志不集中 */
bool lost_concentration( CHAR_DATA * ch , int sn )
{
  sh_int     chance;

  PUSH_FUNCTION( "lost_concentration" );

  if ( !ch || sn < 0 || sn >= MAX_SKILL )
  {
    mudlog( LOG_DEBUG, "lost_concentration: 來源不合理." );
    RETURN( TRUE );
  }

  /* 計算機率 */
  if ( !IS_NPC( ch ) )
    chance = 50 - ( ( ch->skill[sn] + get_curr_wis( ch ) ) / 2 );
  else chance = ( 50 - ( ch->level + get_curr_wis( ch ) ) ) / 2;

  chance = URANGE( 1, chance, 100 );

  /* 沒有失去專心 -- 可以發出技能 */
  if ( number_percent() >= chance ) RETURN( FALSE );

  /* 不專心, 無法施出技能 */
  RETURN( TRUE );
}

/* 檢查是否可以提昇熟練度 */
bool gain_skill_exp( CHAR_DATA * ch , int sn , bool fTrue )
{
  SKILL_DATA * pSkill;
  LIMIT_DATA * pLimit;
  char         buf[MAX_STRING_LENGTH];
  int          chance_base = 10000;
  int          chance      = 100;
  bool         pFind       = FALSE;

  PUSH_FUNCTION( "gain_skill_exp" );

  if ( !ch || sn < 0 || sn >= MAX_SKILL )
  {
    mudlog( LOG_DEBUG, "gain_skill_exp: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( !( pSkill = get_skill( sn ) )
    || ( IS_NPC( ch ) && !ch->boss )
    || fTrue == FALSE
    || !ch->class ) RETURN( FALSE );

  /* 領會其他武功 */
  if ( pSkill->associate != ERRORCODE ) learn_expert_skill( ch, pSkill );

  /* 技能已滿 */
  if ( ch->skill[sn] >= 100 ) RETURN( FALSE );

  /* 打馬也沒有 */
  if ( ch->fighting && ch->fighting->tractable > 0 ) RETURN( FALSE );

  /* 性別不對 */
  if ( pSkill->sex && pSkill->sex > ERRORCODE && pSkill->sex < MAX_SEX )
    if ( ch->sex != pSkill->sex ) RETURN( FALSE );

  /* 是否超出職業的限制 */
  for ( pLimit = pSkill->limit; pLimit; pLimit = pLimit->next )
  {
    if ( pLimit->class == ch->class )
    {
      pFind = TRUE;
      if ( ch->level < pLimit->level && !IS_NPC( ch ) ) RETURN( FALSE );
      if ( ch->skill[sn] >= pLimit->adept && !IS_NPC( ch ) ) RETURN( FALSE );
      break;
    }
  }

  /* 若是找不到也不能升熟練度 */
  if ( pFind == FALSE && !IS_NPC( ch ) ) RETURN( FALSE );

  /* 設定機率 */
  chance_base = 10000 + ( 200 * ( ch->skill[sn] + ch->level ) / 2 );
  chance      = get_curr_wis( ch ) + pSkill->exp;

  /* 失敗 */
  if ( number_range( 1, chance_base ) >= chance ) RETURN( FALSE );

  /* 獲得一點熟練度 */
  sprintf( buf , "\n\r\e[1;33m你的努力苦練終於獲得回報了﹐你的%s"
                 "技能熟練度增加了﹗\e[0m\n\r\n\r", pSkill->cname );

  act( smash_act_keyword( buf, "nes" ), ch->boss ? ch->boss : ch , NULL, NULL, TO_CHAR );
  ch->skill[sn]++;

  RETURN( TRUE );
}

/* 獲得熟練度 */
int get_practice( CHAR_DATA * ch , int sn )
{
  PUSH_FUNCTION( "get_practice" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "get_practice: 人物是空的." );
    RETURN( 0 );
  }

  /* 怪物部份 */
  if ( IS_NPC( ch ) && ch->skill[sn] <= 0 ) RETURN( ch->level );

  /* 玩家部份 */
  RETURN( ch->skill[sn] );
}

SKILL( skill_sneak )
{
  AFFECT_DATA pAf;

  PUSH_FUNCTION( "skill_sneak" );

  send_to_char( "你試著輕移你的腳步。\n\r", ch );
  affect_release( ch, SLOT_SNEAK );

  pAf.type      = sn;
  pAf.duration  = level;
  pAf.location  = APPLY_NONE;
  pAf.modifier  = 0;
  pAf.level     = ch->level;
  affect_to_char( ch, &pAf );

  RETURN( TRUE );
}

SKILL( skill_mask )
{
  AFFECT_DATA      pAf;
  MOB_INDEX_DATA * pMob;
  MOB_INDEX_DATA * zMob;
  int              vnum;
  int              nMatch;
  int              count;

  PUSH_FUNCTION( "skill_mask" );

  if ( is_affected( ch, sn ) )
  {
    send_to_char( "你已經易容了﹗\n\r", ch );
    RETURN( FALSE );
  }

  if ( ch->position != POS_STANDING )
  {
    send_to_char( "你目前的狀態不適合易容﹗\n\r", ch );
    RETURN( FALSE );
  }

  if ( is_pk( ch ) )
  {
    send_to_char( "對不起﹐你目前不能施展這種法術﹗\n\r", ch );
    RETURN( FALSE );
  }

  for ( zMob = NULL, nMatch = count = vnum = 0; nMatch < top_mob_index; vnum++ )
  {
    if ( ( pMob = get_mob_index( vnum ) ) )
    {
      nMatch++;
      if ( pMob->level <= level
        && pMob->count > 0
        && pMob->speak
        && number_range( 0, count++ ) == 0 ) zMob = pMob;
    }
  }

  if ( !zMob )
  {
    send_to_char( "對不起﹐找不到可以易容的對象。\n\r", ch );
    RETURN( FALSE );
  }

  pAf.type      = sn;
  pAf.duration  = UMIN( 5, UMAX( 1, level / 20 ) );
  pAf.location  = APPLY_NONE;
  pAf.modifier  = 0;
  pAf.level     = ch->level;
  affect_to_char( ch, &pAf );

  act( "$n動了些手腳﹐易容成$t了﹐這﹖"
    , ch, mob_index_name( ch, zMob ), NULL, TO_ALL );

  ch->mask = zMob->vnum;

  RETURN( TRUE );
}

SKILL( skill_hide )
{
  AFFECT_DATA pAffect;

  PUSH_FUNCTION( "skill_hide" );

  send_to_char( "你試著隱藏自己。\n\r", ch );

  if ( is_affected( ch, sn ) ) RETURN( FALSE );

  pAffect.type      = sn;
  pAffect.duration  = -1;
  pAffect.location  = APPLY_NONE;
  pAffect.modifier  = 0;
  pAffect.level     = ch->level;
  affect_to_char( ch, &pAffect );

  RETURN( TRUE );
}

FUNCTION( do_forsake )
{
  char          arg[MAX_INPUT_LENGTH];
  int           sn;
  SKILL_DATA  * pSkill;
  ENABLE_DATA * pEnable;

  PUSH_FUNCTION( "do_forsake" );

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    send_to_char( "你要取消那一個技能﹐請查詢 forsake。\n\r", ch );
    RETURN_NULL();
  }

  if ( !( pSkill = skill_isname( arg ) ) )
  {
    act( "找不到你要取消的技能 $2$T$0﹗", ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  if ( ch->skill[ ( sn = pSkill->slot ) ] <= 0 )
  {
    act( "你自己也不會$w﹐怎麼取消﹖", ch, pSkill, NULL, TO_CHAR );
    RETURN_NULL();
  }

  for ( pEnable = ch->enable; pEnable; pEnable = pEnable->next )
  {
    if ( pEnable->skill == pSkill )
    {
      act( "$w還在致能﹐先取消它吧﹗", ch, pSkill, NULL, TO_CHAR );
      RETURN_NULL();
    }
  }

  cancel_skill( ch, pSkill );
  act( "你自廢了$w這項技能了。", ch, pSkill, NULL, TO_CHAR );
  RETURN_NULL();
}

bool learn_expert_skill( CHAR_DATA * ch, SKILL_DATA * pSkill )
{
  LIMIT_DATA * pLimit;
  SKILL_DATA * aSkill;
  int          slot;
  int          max;

  PUSH_FUNCTION( "learn_expert_skill" );

  if ( !ch || !pSkill )
  {
    mudlog( LOG_DEBUG, "learn_expert_skill: 錯誤的來源." );
    RETURN( FALSE );
  }

  if ( pSkill->associate == ERRORCODE
    || pSkill->degree == ERRORCODE
    || !( aSkill = get_skill( pSkill->associate ) ) )
    RETURN( FALSE );

  /* 技能性別限制 */
  if ( aSkill->sex && aSkill->sex > ERRORCODE && aSkill->sex < MAX_SEX )
    RETURN( FALSE );

  if ( ( slot = aSkill->slot ) < 0 || slot >= MAX_SKILL )
  {
    mudlog( LOG_DEBUG, "learn_expert_skill: 超出範圍." );
    RETURN( FALSE );
  }

  if ( ch->skill[slot] > 0
    || !ch->class
    || learn_count( ch ) >= get_learnable( ch )
    || !check_skill_restrict( ch, aSkill, FALSE )
    || !check_valid_rating( ch, aSkill, FALSE )
    || number_range( 1, 10000 - ( ch->skill[slot] * 20 ) ) > pSkill->degree )
    RETURN( FALSE );

  for ( max = 0, pLimit = aSkill->limit; pLimit; pLimit = pLimit->next )
  {
    if ( pLimit->class == ch->class )
    {
      if ( ch->level < pLimit->level ) RETURN( FALSE );
      max = UMIN( number_range( 1, 5 ), pLimit->adept );
      break;
    }
  }

  if ( max <= 0 ) RETURN( FALSE );
  ch->skill[slot] = max;

  act( "$A$A$A$7恭禧你從$w領會到$2$W$7這一門功\夫﹗$0$A$A$A"
    , ch, pSkill, aSkill, TO_CHAR );

  act( "$A$A$A$7$n$7得天獨厚﹐學會了$2$W$7這一項功\夫﹗$0$A$A$A"
    , ch, NULL, aSkill, TO_ROOM );

  RETURN( TRUE );
}

bool effect_driver( CHAR_DATA * ch, CHAR_DATA * victim, EFFECT_DATA * pEffect )
{
  int         oldvalue;
  AFFECT_DATA pAffect;

  PUSH_FUNCTION( "effect_driver" );

  if ( !ch || !victim || !pEffect )
  {
    mudlog( LOG_DEBUG, "effect_driver: 來源錯誤." );
    RETURN( FALSE );
  }

  if ( pEffect->value[2] > 0 )
    if ( number_percent() > pEffect->value[2] ) RETURN( FALSE );

  switch( pEffect->type )
  {
  default:
    mudlog( LOG_DEBUG, "effect_driver: 選項 %d 錯誤.", pEffect->type );
    RETURN( FALSE );

  /* 更改對方法力 */
  case EFFECT_VICTIM_MANA:

    oldvalue = victim->mana;

    victim->mana =
       UMAX( 0, UMIN( victim->max_mana, victim->mana - pEffect->value[0] ) );

    if ( oldvalue < victim->mana )
    {
      act( "$3$n的法力突然之間上升了﹗$0", victim, NULL, NULL, TO_ALL );
    }

    else if ( oldvalue > victim->mana )
    {
      act( "$1$n的法力突然之間下降了﹗$0", victim, NULL, NULL, TO_ALL );
    }

    if ( victim->mana <= 0 )
    {
      act( "$2$n的法力已經耗盡﹐眼看是不行了﹗$0", victim, NULL, NULL, TO_ALL );
    }

    break;

  /* 更改自己法力 */
  case EFFECT_SELF_MANA:

    oldvalue = ch->mana;

    ch->mana =
       UMAX( 0, UMIN( ch->max_mana, ch->mana - pEffect->value[0] ) );

    if ( oldvalue < ch->mana )
    {
      act( "$3$n的法力突然之間上升了﹗$0", ch, NULL, NULL, TO_ALL );
    }

    else if ( oldvalue > ch->mana )
    {
      act( "$1$n的法力突然之間下降了﹗$0", ch, NULL, NULL, TO_ALL );
    }

    if ( ch->mana <= 0 )
    {
      act( "$2$n的法力已經耗盡﹐眼看是不行了﹗$0", ch, NULL, NULL, TO_ALL );
    }

    break;

  /* 更改對方體力 */
  case EFFECT_VICTIM_MOVE:

    oldvalue = victim->move;

    victim->move =
       UMAX( 0, UMIN( victim->max_move, victim->move - pEffect->value[0] ) );

    if ( oldvalue < victim->move )
    {
      act( "$3$n的體力突然之間上升了﹗$0", victim, NULL, NULL, TO_ALL );
    }

    else if ( oldvalue > victim->move )
    {
      act( "$1$n的體力突然之間下降了﹗$0", victim, NULL, NULL, TO_ALL );
    }

    if ( victim->move <= 0 )
    {
      act( "$2$n的體力已經耗盡﹐眼看是不行了﹗$0", victim, NULL, NULL, TO_ALL );
    }

    break;

  /* 更改自己體力 */
  case EFFECT_SELF_MOVE:
    oldvalue = ch->move;

    ch->move =
       UMAX( 0, UMIN( ch->max_move, ch->move - pEffect->value[0] ) );

    if ( oldvalue < ch->move )
    {
      act( "$3$n的體力突然之間上升了﹗$0", ch, NULL, NULL, TO_ALL );
    }

    else if ( oldvalue > ch->move )
    {
      act( "$1$n的體力突然之間下降了﹗$0", ch, NULL, NULL, TO_ALL );
    }

    if ( ch->move <= 0 )
    {
      act( "$2$n的體力已經耗盡﹐眼看是不行了﹗$0", ch, NULL, NULL, TO_ALL );
    }

    break;

  /* 對方眼盲 */
  case EFFECT_VICTIM_BLINDNESS:

    /* 若已經眼盲則不動作 */
    if ( is_affected( victim, SLOT_BLINDNESS ) ) break;

    pAffect.type      = SLOT_BLINDNESS;
    pAffect.location  = APPLY_HITROLL;
    pAffect.modifier  = 0 - pEffect->value[0];
    pAffect.duration  = pEffect->value[1];
    pAffect.level     = ch->level;
    affect_to_char( victim, &pAffect );

    act( "強光一閃﹐$n暫時失明了﹗", victim, NULL, NULL, TO_ALL );
    break;

  /* 對方詛咒 */
  case EFFECT_VICTIM_CURSE:

    /* 若已經被詛咒則不動作 */
    if ( is_affected( victim, SLOT_CURSE ) ) break;

    pAffect.type      = SLOT_CURSE;
    pAffect.duration  = pEffect->value[1];
    pAffect.location  = APPLY_HITROLL;
    pAffect.modifier  = 0 - pEffect->value[0];
    pAffect.level     = ch->level;
    affect_to_char( victim, &pAffect );

    pAffect.location  = APPLY_SAVING_SPELL;
    pAffect.modifier  = 1;
    affect_to_char( victim, &pAffect );

    act( "$n感到一陣暗黑壓抑著$n的力量。", victim, NULL, NULL, TO_ALL );

    break;

  /* 對方中毒 */
  case EFFECT_VICTIM_POISON:

    if ( get_curr_str( victim ) > 2 )
    {
      /* 這個會累計 */
      pAffect.type      = SLOT_POISON;
      pAffect.duration  = pEffect->value[1];
      pAffect.location  = APPLY_STR;
      pAffect.modifier  = -2;
      pAffect.level     = ch->level;
      affect_join( victim, &pAffect );

      act( "$n臉上一陣青一陣白﹐似乎是中了毒﹗", victim, NULL, NULL, TO_ALL );
    }

    break;

  /* 對方昏睡 */
  case EFFECT_VICTIM_SLEEP:
    break;

  /* 對方麻痺 */
  case EFFECT_VICTIM_PARA:
    /* 怪物沒有麻痺 */
    if ( IS_NPC( victim ) ) RETURN( FALSE );

    WAIT_STATE( victim, pEffect->value[0] );
    act( "$2$n腿被踢到一下﹐暫時不太會動﹗$0", victim, NULL, NULL, TO_ALL );
    break;

  /* 對方精靈之火 */
  case EFFECT_VICTIM_FAERIE_FIRE:

    /* 若已經被施法則不動作 */
    if ( is_affected( victim, SLOT_FAERIE_FIRE ) ) break;

    pAffect.type      = SLOT_FAERIE_FIRE;
    pAffect.duration  = pEffect->value[1];
    pAffect.location  = APPLY_AC;
    pAffect.modifier  = pEffect->value[0];
    pAffect.level     = ch->level;
    affect_to_char( victim, &pAffect );

    act( "$n被一團粉紅煙霧包圍著。", victim, NULL, NULL, TO_ALL );

    break;

  /* 更改對方生命力 */
  case EFFECT_VICTIM_HIT:

    oldvalue = victim->hit;

    victim->hit =
       UMAX( 0, UMIN( victim->max_hit, victim->hit - pEffect->value[0] ) );

    if ( oldvalue < victim->hit )
    {
      act( "$3$n的生命力突然之間上升了﹗$0", victim, NULL, NULL, TO_ALL );
    }

    else if ( oldvalue > victim->hit )
    {
      act( "$1$n的生命力突然之間下降了﹗$0", victim, NULL, NULL, TO_ALL );
    }

    if ( victim->hit <= 0 )
    {
      act( "$2$n的生命力已經耗盡﹐見閻王去吧﹗$0", victim, NULL, NULL, TO_ALL );
    }
    break;

  /* 更改自己生命力 */
  case EFFECT_SELF_HIT:

    oldvalue = ch->hit;

    ch->hit =
       UMAX( 0, UMIN( ch->max_hit, ch->hit - pEffect->value[0] ) );

    if ( oldvalue < ch->hit )
    {
      act( "$3$n的生命力突然之間上升了﹗$0", ch, NULL, NULL, TO_ALL );
    }

    else if ( oldvalue > ch->hit )
    {
      act( "$1$n的生命力突然之間下降了﹗$0", ch, NULL, NULL, TO_ALL );
    }

    if ( ch->hit <= 0 )
    {
      act( "$2$n的生命力已經耗盡﹐地府見﹗$0", ch, NULL, NULL, TO_ALL );
    }

    break;

  /* 一擊必殺 */
  case EFFECT_HIT_DIE:
    if ( victim->hit >= 0 ) victim->hit = 0;
    act( "\n\r$2$n發出必殺一擊﹐$N的魂魄瞬間就被死神勾走了﹗$0\n\r"
      , ch, NULL, victim, TO_CHAR );
    act( "\n\r$2$n發出必殺一擊﹐$N的魂魄瞬間就被死神勾走了﹗$0\n\r"
      , ch, NULL, victim, TO_VICT );
    act( "\n\r$2$n發出必殺一擊﹐$N的魂魄瞬間就被死神勾走了﹗$0\n\r"
      , ch, NULL, victim, TO_NOTVICT );
    break;
  }

  RETURN( TRUE );
}

FUNCTION( do_leftskill )
{
  int          loop;
  int          count;
  int          cost;
  bool         CanInc;
  SKILL_DATA * pSkill;
  LIMIT_DATA * pLimit;

  PUSH_FUNCTION( "do_leftskill" );

  if ( IS_NPC( ch ) || !ch->class ) RETURN_NULL();

  cost = UMAX( ch->level * LeftSkillCost, ch->level );

  if ( ch->gold < cost )
  {
    act( "等你湊足$i兩銀子﹐我就告訴你﹗", ch, &cost, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( learn_count( ch ) >= get_learnable( ch ) )
  {
    act( "對不起﹐你已經超過$T可以學習技能的總數﹗"
      , ch, NULL, class_name( ch->class, TRUE ), TO_CHAR );

    RETURN_NULL();
  }

  clear_buffer();

  send_to_buffer( "\e[1;33;44m順序 技 能 中 文 名 稱    技 能 英 文 名 稱"
    "    教導 分    類  最高熟練 目前熟練 \e[0m\n\r" );

  for ( count = loop = 0; loop < MAX_SKILL; loop++ )
  {
    if ( !( pSkill = get_skill( loop ) )
      || !pSkill->valid
      || !check_skill_restrict( ch, pSkill, FALSE )
      || !check_valid_rating( ch, pSkill, FALSE ) )
      continue;

    CanInc = FALSE;
    for ( pLimit = pSkill->limit; pLimit; pLimit = pLimit->next )
    {
      if ( pLimit->class == ch->class )
      {
        if ( ch->level >= pLimit->level && ch->skill[loop] < pLimit->adept )
          CanInc = TRUE;

        break;
      }
    }

    if ( !pLimit ) continue;

    send_to_buffer( "%4d %-20s %-20s %4s %8s %s%s %s\n\r"
      , ++count, pSkill->cname, pSkill->name
      , pSkill->teach ? "可以" : "不可"
      , skill_type( pSkill->type )
      , CanInc ? " " : "\e[1;32m*\e[0m"
      , adeptation_name( pLimit->adept )
      , adeptation_name( ch->skill[loop] ) );
  }

  if ( count == 0 ) send_to_buffer( "你已經沒有技能可以學習了﹗\n\r" );
  else gold_from_char( ch, cost );

  print_buffer( ch );

  RETURN_NULL();
}

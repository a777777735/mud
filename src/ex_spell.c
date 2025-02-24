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

extern  int     spell_damage    args( ( CHAR_DATA *, int, int, int ) );

SKILL( cast_restore_staff )
{
  OBJ_DATA * pObj = ( OBJ_DATA * ) vo;

  PUSH_FUNCTION( "cast_restore_staff" );

  if ( !ch || !verify_char( ch ) ) RETURN( FALSE );

  if ( pObj->item_type == ITEM_STAFF
    || pObj->item_type == ITEM_WAND )
  {
    if ( pObj->value[6] >= pObj->value[7] * 20 )
    {
      act( "$p的法力已經灌滿了﹐無法再加強了﹗", ch, pObj, NULL, TO_CHAR );
      RETURN( FALSE );
    }

    pObj->value[6] = UMIN( pObj->value[7] * 20, pObj->value[6] + level * 10 );

    act( "$6$p發出了一陣柔和的藍光﹐你感到法力已經加強了﹗$0"
      , ch, pObj, NULL, TO_ALL );
  }
  else
  {
    act( "你無法將$p灌予法力﹐這不是法杖﹗", ch, pObj, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  RETURN( FALSE );
}

SKILL( cast_fire_elf )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         dam;
  int         kk;
  int         ran;

  PUSH_FUNCTION( "cast_fire_elf" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "cast_fire_elf:來源不正確." );
    RETURN( FALSE );
  }

  /* 前置訊息 */
  act( "$1來自地獄的火之精靈啊﹗聽我虔誠的祈求吧﹗$0", ch, NULL, victim, TO_ALL );

  dam = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_FIV );
  dam = number_range( dam * 3 / 4, dam * 5 / 4 );
  dam = URANGE( 0, dam, LEVEL_FIV_DAM );

  if ( number_percent() > ( ran = number_fuzzy( level ) ) )
  {
    sact( ch, victim, NULL,
      "$n前方的召喚陣式突然間崩潰﹐$n遭到反撲﹗" );
    damage( ch, ch, UMAX( 1, dam / 10 ), sn, ATTACK_RANDOM );
    RETURN( TRUE );
  }

  sact( ch, victim, NULL,
    "$R手持火叉的火精靈由地心竄出﹐依照血的契約來到這個世界﹗\n\r"
    "火之精靈不耐的傲視全場﹐接著手持火叉向著$N衝去﹗\n\r"
    "有如烈火燎原﹐$N受到極嚴重的火傷﹗" );
  damage( ch, victim, dam, sn, ATTACK_RANDOM );

  if ( level > 65 && can_damage( victim ) )
  {
    sact( ch, victim, NULL,
      "$A$R$n所發出的火之精靈的餘火不停的燒灼著$N﹗$Z" );

    for ( kk = 1; kk <= number_fuzzy( level / 20 ); kk++ )
    {
      if ( !can_damage( victim )
        || ( ch->in_room
          && victim->in_room
          && victim->in_room != ch->in_room ) ) break;

      damage( ch, victim, dam / 4 , sn , ATTACK_RANDOM );
    }
  }

  RETURN( TRUE );
}

SKILL( cast_bystander_fist )
{
  ROOM_INDEX_DATA * to_room;
  ROOM_INDEX_DATA * origin;
  CHAR_DATA       * people;
  CHAR_DATA       * victim = NULL;
  char            * pString = ( char * ) vo;
  int               door;
  int               dam;

  PUSH_FUNCTION( "cast_bystander_fist" );

  if ( !ch || !pString || !*pString || !ch->in_room )
  {
    mudlog( LOG_DEBUG, "cast_bystander_fist: 來源錯誤." );
    RETURN( FALSE );
  }

  if ( ch->position != POS_STANDING )
  {
    send_to_char( "你得放下手邊的工作才能施展這招﹗\n\r", ch );
    RETURN( FALSE );
  }
       if ( !str_prefix( pString, "north" ) ) door = DIR_NORTH;
  else if ( !str_prefix( pString, "east"  ) ) door = DIR_EAST;
  else if ( !str_prefix( pString, "south" ) ) door = DIR_SOUTH;
  else if ( !str_prefix( pString, "west"  ) ) door = DIR_WEST;
  else if ( !str_prefix( pString, "up"    ) ) door = DIR_UP;
  else if ( !str_prefix( pString, "down"  ) ) door = DIR_DOWN;
  else if ( !str_prefix( pString, "enter" ) ) door = DIR_ENTER;
  else if ( !str_prefix( pString, "out"   ) ) door = DIR_OUT;
  else
  {
    send_to_char( "你的方向不對。\n\r", ch );
    RETURN( FALSE );
  }

  if ( !ch->in_room->exit[door]
    || !( to_room = ch->in_room->exit[door]->to_room ) )
  {
    act( "你的$t方根本沒有出口﹗", ch, direction_name( door ), NULL, TO_CHAR );
    RETURN( FALSE );
  }

  for ( people = to_room->people; people; people = people->next_in_room )
    if ( is_same_group( ch, people ) && ( victim = people->fighting ) ) break;

  if ( !people || !victim || !can_damage( victim ) )
  {
    act( "你的$t方根本沒有你的隊友正在打鬥中﹗"
      , ch, direction_name( door ), NULL, TO_CHAR );

    RETURN( FALSE );
  }

  if ( !IS_NPC( victim ) )
  {
    send_to_char( "你不能用這招打玩家﹗\n\r", ch );
    RETURN( FALSE );
  }

  if ( victim->hit <= 1 )
  {
    send_to_char( "你不能用這招結束他的生命﹗\n\r", ch );
    RETURN( FALSE );
  }

  dam = number_range( level, 2 * level );
  if ( victim->hit - dam <= 1 ) dam = victim->hit - 1;

  origin = ch->in_room;
  char_from_room( ch );
  char_to_room( ch, to_room );

  if ( dam <= 0 )
  {
    sact( ch, victim, NULL
      , "$n從隔壁施展出隔山打牛﹐$N倒是沒打到﹐只打到蒼蠅﹗" );
  }

  else
  {
    sact( ch, victim, NULL
      , "$n從某不知名地方師展出隔山打牛﹐這一招打的結實﹐"
        "看來$N目前是受傷不輕﹗" );

    damage_situs( victim, dam, ATTACK_RANDOM );
  }

  char_from_room( ch );
  char_to_room( ch, origin );

  RETURN( TRUE );
}

SKILL( cast_power_word )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         dam;
  int         tt;

  PUSH_FUNCTION( "cast_power_word" );

  if ( !ch || !verify_char( ch ) )
  {
    mudlog( LOG_DEBUG, "cast_power_word: 來源錯誤." );
    RETURN( FALSE );
  }

  /* 前置訊息 */
  sact( ch, victim, NULL,
    "$W天上降下一朵詳雲﹐包覆著$n﹐漸漸的﹐你看不清$e的身影了﹗$A"
    "$W你只聽見耳邊不停的傳來一些梵音﹐漸漸的越來越清楚﹗$Z$A" );

  /* 設定攻傷 */
  dam = spell_damage( ch, level, TYPE_WIS, SPELL_CLASS_FIV );
  dam = number_range( dam / 2, dam * 3 / 4 );
  dam = URANGE( 0, dam, LEVEL_FIV_DAM );

  if ( ch->alignment < ( AlignRange / 2 ) || !IS_NPC( victim ) )
  {
    sact( ch, victim, NULL,
      "$W$n不停地唸誦箴言﹐只見$N倒在地上打滾﹐似乎受不了這些聲音﹗$Z" );
    damage( ch, victim, dam, sn, ATTACK_RANDOM );
    RETURN( TRUE );
  }

  else if ( ch->alignment < ( AlignRange * 3 / 4 ) )
  {
    for ( tt = 0; tt <= ch->skill[sn] / 20; tt++ )
    {
      if ( !verify_char( victim )
        || !can_damage( victim )
        || victim->in_room != ch->in_room ) break;

      sact( ch, victim, NULL,
        "$W一段段的真言不停地以有形的文字打在$N的身上…$Z" );
      dam = dam + level;
      damage( ch, victim, dam / 3, sn, ATTACK_RANDOM );
    }

    RETURN( TRUE );
  }

  else
  {
    sact( ch, victim, NULL,
      "$W遠方的天空出現了一朵白雲﹐一個身著法衣的菩薩身坐其中。$A$Z" );

    for ( tt = 0; tt <= ch->skill[sn] / 15 ; tt++ )
    {
      if ( !verify_char( victim )
        || victim->in_room != ch->in_room ) break;

      sact( ch, victim, NULL,
        "$W菩薩手一指﹐發出一道光芒淨化$N﹐$E痛苦難耐﹗$Z" );
      dam = dam + level;
      damage( ch, victim, dam / 2 , sn, ATTACK_RANDOM );
    }

    sact( ch, victim, NULL,
      "$W金光一閃﹐菩薩已經轉入大氣中﹐不見蹤影了﹗$A" );
    RETURN( TRUE );
  }

  RETURN( FALSE );
}

SKILL( cast_haste )
{
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA   af;

  PUSH_FUNCTION( "cast_haste" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_haste: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( is_affected( victim, sn ) )
  {
    act( "$N已經非常的迅捷了﹗", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  af.type      = sn;
  af.duration  = number_range( level / 6, level / 4 );
  af.location  = APPLY_DEX;
  af.modifier  = 1 + ( level >= 18 ) + ( level >= 25 );
  af.level     = level;
  affect_to_char( victim, &af );

  send_to_char( "一陣電流傳到你的腳上﹐你感到速度變快了。\n\r"
    , victim );
  if ( ch != victim ) send_to_char( "好了﹐施法迅捷術完畢。\n\r", ch );
  RETURN( TRUE );
}

SKILL( cast_super_refresh )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         refresh;

  PUSH_FUNCTION( "cast_super_refresh" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_super_refresh: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( victim->move >= get_curr_move( victim ) )
  {
    act( "$N已經完全不會疲倦了﹗", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  refresh = level * 5 + ( number_range( 1, get_curr_int( ch ) * 4 ) );
  victim->move = UMIN( victim->move + refresh, get_curr_move( victim ) );

  send_to_char( "你感到非常的不疲倦了。\n\r", victim );
  if ( ch != victim ) send_to_char( "好了﹐體力恢復完畢。\n\r", ch );
  RETURN( TRUE );
}

SKILL( cast_backstab )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  OBJ_DATA  * pObj;
  int         chance;
  int         dam;

  PUSH_FUNCTION( "cast_backstab" );

  if ( victim->fighting )
  {
    act( "$N正在忙呢﹗你是沒有辦法繞道$E背後的﹗", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  if ( victim->hit <= get_curr_hit( victim ) / 3 )
  {
    act( "$N損血很多﹐提高了警覺性﹐你沒有辦法繞到$E背後﹗"
      , ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  if ( !( pObj = get_eq_char( ch, WEAR_WIELD ) ) )
  {
    send_to_char( "你的武器似乎不太對﹗\n\r", ch );
    RETURN( FALSE );
  }

  if ( !can_see( ch, victim ) )
    chance = 1;
  else
  {
    chance = ( level + get_curr_dex( ch ) ) / 2;

    if ( is_affected( ch, SLOT_SNEAK ) ) chance += chance / 2;
    if ( !can_see( victim, ch ) )        chance += chance / 2;

    chance = UMAX( 1, UMIN( 100, chance ) );
  }

  /* 訊息 */
  sact( ch, victim, pObj, "但見黃光一閃﹐$n手中$O突向$N刺去﹗\n\r" );

  if ( number_percent() >= chance )
  {
    dam = get_curr_str( ch );
    sact( ch, victim, NULL, "但不知如何﹐卻被$N發現了﹗" );
  }
  else
  {
    dam = spell_damage( ch, level, TYPE_DEX, SPELL_CLASS_MAG );
    dam = number_range( dam * 3 / 4, dam * 5 / 4 );
    dam = URANGE( 0, dam, MAG_ATTACK_DAM );
    sact( ch, victim, pObj, "轉瞬間﹐$O已經刺入$N的身體了﹐$n突襲成功\﹗" );
  }

  damage( ch, victim, dam, sn, ATTACK_RANDOM );

  RETURN( TRUE );
}

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

#define MIN_USE_TIME 5
#define MAX_USE_TIME 20

extern  int     spell_damage    args( ( CHAR_DATA *, int, int, int ) );

void    scan                    args( ( CHAR_DATA *, int ) );
int     spell_damage            args( ( CHAR_DATA *, int, int, int ) );
bool    can_steal               args( ( CHAR_DATA *, CHAR_DATA * ) );

int spell_damage( CHAR_DATA * ch, int level, int suit, int class )
{
  int dam;
  int type_number;

  PUSH_FUNCTION( "spell_damage" );

  if ( !ch || !verify_char( ch ) ) RETURN( 0 );

  switch( suit )
  {
  default:
    mudlog( LOG_DEBUG, "spell_damage: 錯誤型態: %d.", suit );
    RETURN( 0 );

  case TYPE_STR:
    type_number = get_curr_str( ch );
    break;

  case TYPE_INT:
    type_number = get_curr_int( ch );
    break;

  case TYPE_WIS:
    type_number = get_curr_wis( ch );
    break;

  case TYPE_DEX:
    type_number = get_curr_dex( ch );
    break;

  case TYPE_CON:
    type_number = get_curr_con( ch );
    break;

  case TYPE_MOB:
    type_number = ch->level;
    break;
  }

  switch( class )
  {
  default:
    mudlog( LOG_DEBUG, "spell_damage: 錯誤層級: %d.", class );
    RETURN( 0 );

  case SPELL_CLASS_ONE:
    dam = 40 + ( level + ch->level + type_number ) * 3;
    dam = number_range( dam * 2 / 3, dam * 4 / 3 );
    dam = UMAX( 0, UMIN( dam, LEVEL_ONE_DAM ) );
    break;

  case SPELL_CLASS_TWO:
    dam = 200 + ( level + ch->level + type_number ) * 6;
    dam = number_range( dam * 3 / 4, dam * 5 / 4 );
    dam = UMAX( 0, UMIN( dam, LEVEL_TWO_DAM ) );
    break;

  case SPELL_CLASS_THR:
    dam = 350 + ( level + ch->level + type_number ) * 9;
    dam = number_range( dam * 3 / 4, dam * 5 / 4 );
    dam = UMAX( 0, UMIN( dam, LEVEL_THR_DAM ) );
    break;

  case SPELL_CLASS_FOU:
    dam = 500 + ( level + ch->level + type_number ) * 12;
    dam = number_range( dam * 3 / 4, dam * 5 / 4 );
    dam = UMAX( 0, UMIN( dam, LEVEL_FOU_DAM ) );
    break;

  case SPELL_CLASS_FIV:
    dam = 750 + ( level + ch->level + type_number ) * 15;
    dam = number_range( dam * 3 / 4, dam * 5 / 4 );
    dam = UMAX( 0, UMIN( dam, LEVEL_FIV_DAM ) );
    break;

  case SPELL_CLASS_MOB:
    dam = 1000 + ( level + ch->level + type_number ) * 18;
    dam = number_range( dam, dam * 5 / 4 );
    dam = UMAX( 0, UMIN( dam, MOB_ATTACK_DAM ) );
    break;

  case SPELL_CLASS_MAG:
    dam = 800 + ( level + ch->level + type_number ) * 16;
    dam = number_range( dam, dam * 7 / 5 );
    dam = UMAX( 0, UMIN( dam, MAG_ATTACK_DAM ) );
    break;
  }

  RETURN( dam );
}


SKILL( spell_make_spring )
{
  OBJ_DATA * spring;
  int        choice[] = { 33, 66, 100, -1 };

  PUSH_FUNCTION( "spell_make_water" );

  if ( !ch->in_room )
  {
    mudlog( LOG_DEBUG, "spell_make_spring: 來源錯誤." );
    RETURN( FALSE );
  }

  if ( !can_see_room( ch ) )
  {
    act( "$r黑漆漆的﹐你什麼都看不到。" , ch, NULL, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  if ( obj_type_room( ch, ITEM_FOUNTAIN ) > 0 )
  {
    act( "$r水源豐富﹐已不需要再做出水了。" , ch, NULL, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  if ( is_loge( ch->in_room ) || ch->in_room->Sail )
  {
    act( "你是沒有辦法在$r造出水池的﹗", ch, NULL, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  if ( ch->in_room->nobject >= MaxRoomObject )
  {
    send_to_char( "這裡的物品太多了﹐請先奉獻掉一些吧﹗\n\r", ch );
    RETURN( FALSE );
  }

  spring = create_object( ObjSpring , 1 );
  spring->timer = number_range( MIN_USE_TIME, MAX_USE_TIME );
  obj_to_room( spring, ch->in_room );

  switch( dimemsion_position( choice, level, TRUE ) )
  {
  case ERRORCODE:
    mudlog( LOG_DEBUG, "技能 %d 的訊息設定有問題." , sn );
    RETURN( FALSE );

  case 0:

    act( "$6$n使出了渾身解數﹐希望造出一個小池塘。$A"
      "瞬間﹐在五彩繽紛的光芒籠罩下﹐小水滴漸漸集合﹐"
      "形成了一個$p。$0", ch, spring, NULL, TO_ALL );
    break;

  case 1:

    act( "$6但見$n隨手一揮﹐瞬間此地的水分全給集中起來﹐"
      "並且土地下陷﹐馬上就形成了一個$p﹐更可觀的是居然還有"
      "鴨子在上面游呢﹗$0", ch, spring, NULL, TO_ALL );

    break;

  case 2:

    act( "$6$n四周突然開始起霧﹐籠罩著整個地方﹐"
      "瞬間﹐雲霧化做七彩水柱﹐由高空降至地面﹐"
      "形成一個詩情畫意的$p﹐雲霧繚繞﹐久久不散﹗$0"
      , ch, spring, NULL, TO_ALL );

    break;
  }

  RETURN( TRUE );
}

SKILL( spell_make_light )
{
  OBJ_DATA * light;
  int        choice[] = { 33, 66, 100, -1 };

  PUSH_FUNCTION( "spell_make_light" );

  if ( obj_type_char( ch, ITEM_LIGHT ) > 0 )
  {
    send_to_char( "你身上已經夠亮囉﹐不需要光了﹗\n\r" , ch );
    RETURN( FALSE );
  }

  if ( obj_type_room( ch, ITEM_LIGHT ) > 0 )
  {
    act( "$r已經夠亮囉﹐不需要光了。" , ch, NULL, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  if ( ch->in_room->nobject >= MaxRoomObject )
  {
    send_to_char( "這裡的物品太多了﹐請先奉獻掉一些吧﹗\n\r", ch );
    RETURN( FALSE );
  }

  light = create_object( ObjLight, 1 );
  light->value[2] = number_range( MIN_USE_TIME / 2, MAX_USE_TIME / 2 );
  light->value[3] = number_range( MIN_USE_TIME * 2, MAX_USE_TIME * 2 );
  obj_to_char( light, ch );

  switch( dimemsion_position( choice, level, TRUE ) )
  {
  case ERRORCODE:
    mudlog( LOG_DEBUG, "技能%d的訊息設定有問題." , sn );
    RETURN( FALSE );

  case 0:

    act( "$6$n順手一揮﹐只見無數的小光點開始快數的轉動﹐"
         "接著繞著$n轉一圈之後便集中起來﹐形成了$p﹗$0"
         , ch , light , NULL , TO_ALL );

    break;

  case 1:

    act( "$7轉眼之間﹐一團團由小光點所組成的光圈由地上升起﹐"
         "接著越轉越快﹐就在快到達看不清楚之時﹐才發現它們已經聚集一起"
         "形成了$p﹗$0", ch, light, NULL, TO_ALL );

    break;

  case 2:

    act( "$7一時間沒有什麼變化﹐但是隱隱雷聲不斷﹐"
         "頗給人風雨前的寧靜之勢﹐突然之間﹐仰頭望去"
         "﹐發現星星全部向著$n聚集﹐轉眼間已經形成了$p﹗$0"
         , ch, light, NULL, TO_ALL );

    break;

  }

  RETURN( TRUE );
}

#undef MIN_USE_TIME
#undef MAX_USE_TIME

SKILL( spell_cast_armor )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA aff;
  int         choice[] = { 33, 66, 100, -1 };

  PUSH_FUNCTION( "spell_cast_armor" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "spell_cast_armor: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( is_affected( victim, sn ) )
  {
    act( "$N的保護看來已經十分足夠了﹗", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  aff.type      = sn;
  aff.duration  = UMAX( 1, number_range( level / 10, level / 5 ) );
  aff.modifier  = -( UMAX( 1, number_range( level / 2, level ) ) );
  aff.location  = APPLY_AC;
  aff.level     = level;
  affect_to_char( victim, &aff );

  switch( dimemsion_position( choice, level, TRUE ) )
  {
  case ERRORCODE:
    mudlog( LOG_DEBUG, "技能 %d 的訊息設定有問題." , sn );
    RETURN( FALSE );

  case 0:

    act( "$3$n$3身邊一陣晃動﹐你感到空氣一陣凝結﹐接著"
         "形成一個護盾圍繞在$N$3身前﹐保護著$N﹗$0"
         , ch, NULL, victim, TO_ALL );

    break;

  case 1:

    act( "$3$n$3大喊一聲﹐接著大地震動﹐風雲變色﹐無數的精靈紛紛"
         "出現在$N$3身前形成一護盾﹗$0"
         , ch, NULL, victim, TO_ALL );

    break;

  case 2:

    act( "$3天上一陣金光閃閃﹐接著無數的天兵天將如潮水湧了下來﹐"
         " 圍繞著$N$3﹐緩緩的在他身邊形成一圈像是保護膜一樣的東西﹐"
         "感覺好像十分堅固﹗$0" , ch, NULL, victim, TO_ALL );

    break;

  }

  RETURN( TRUE );
}

SKILL( spell_cast_bless )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         choice[] = { 33, 66, 100, -1 };
  AFFECT_DATA aff;

  PUSH_FUNCTION( "spell_cast_bless" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "spell_cast_bless: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( is_affected( victim, sn ) )
  {
    act( "$N的祝福看來已經十分足夠了﹐你不需要再祝福了。"
      , ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  aff.type      = sn;
  aff.duration  = UMAX( 1, number_range( level / 10, level / 5 ) );
  aff.modifier  = 0;
  aff.location  = APPLY_SAVING_SPELL;
  aff.level     = level;
  affect_to_char( victim, &aff );

  aff.location  = APPLY_HITROLL;
  aff.modifier  = UMAX( 1, number_range( level / 8, level / 7 ) );
  aff.level     = level;
  affect_to_char( victim, &aff );

  switch( dimemsion_position( choice, level, TRUE ) )
  {
  case ERRORCODE:
    mudlog( LOG_DEBUG, "技能 %d 的訊息設定有問題.", sn );
    RETURN( FALSE );

  case 0:
    act( "$7$n$7順手一揮﹐許\多的小精靈從空中冒了下來"
         "﹐圍繞著$N$7唱著聖歌﹐祝福著$E。$0", ch, NULL, victim, TO_ALL );
    break;

  case 1:

    act( "$7$n$7輕聲吟唱著祝福之頌﹐將滿滿的祝福送與$N$7﹐轉眼間只見$E"
         "慢慢的發出閃閃的銀色光芒﹐宛如天邊的一漣明月。$0"
         , ch, NULL, victim, TO_ALL );

    break;

  case 2:
    act( "$7$n$7緩緩的揮舞著手﹐接著許\許\多多的皎潔的光之精靈慢慢的漂浮了"
         "過來﹐圍繞著$N$7並且包圍著$E﹐緩緩的形成了一面明亮的光之祝福﹐接"
         "著天上的天使都探頭下來爭賭這天上人間奇景。$0"
         , ch, NULL, victim, TO_ALL );
    break;
  }

  RETURN( TRUE );
}

SKILL( spell_cast_fire )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         dam;
  int         dam_plus;
  int         choice[] =
              { 25, 35, 45, 55, 65, 75, 80, 85, 90, 95, 98, 99, 100, -1 };

  PUSH_FUNCTION( "spell_cast_fire" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "spell_cast_fire: 缺乏來源." );
    RETURN( FALSE );
  }

  dam = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_FOU );
  dam = dam / 2;

  /* 前置訊息 */
  act( "$1$n$1運集心中的憤怒在雙手﹐轉眼間雙手血紅如火﹗﹗$A$0"
    , ch, NULL, victim, TO_ALL );

  switch( UMIN( 3, dam_plus = dimemsion_position( choice, level , TRUE ) ) )
  {
  case ERRORCODE:
    mudlog( LOG_DEBUG, "技能 %d 的訊息設定有問題." , sn );
    RETURN( FALSE );

  case 0:
    sact( ch, victim, NULL,
      "$n用出全部力量﹐擠出一個拳頭般大的火球﹐轟向$N﹗" );
    dam_plus = 1;
    break;

  case 1:
    sact( ch, victim, NULL,
      "$n雙手連擊﹐發出了兩個小火球﹐接著後者追上前者﹐形成一個火球轟向$N﹗" );
    break;

  case 2:
    sact( ch, victim, NULL,
      "$n雙手合一﹐略一用力﹐發出了一個巴掌大的火球﹐像$N飛去﹗" );
    break;

  case 3:
    sact( ch, victim, NULL,
      "$n將雙手用力旋轉﹐互相碰撞﹐猛地爆出許\多火花﹐接著一個個火花"
      "像是有生命一般向著$N追去﹗" );
    break;

  case 4:
    sact( ch, victim, NULL,
      "$n剎地高舉雙手﹐接著一隻舉著火叉的精靈應邀前來﹗$A"
      "火精靈向著$N投以不屑的一眼﹐就將火紅的$R火叉$Z猛向$E丟去﹗" );
    break;

  case 5:
    sact( ch, victim, NULL,
      "$n高舉雙手﹐收集了大自然的熱量﹐融合形成一個高熱的"
      "$R火球$Z﹐接著$R火球$Z像是有生命似的急奔$N而去﹗" );
    break;

  case 6:
    sact( ch, victim, NULL,
      "$n坐了下來﹐集中精神默唸一些聽不懂的咒語﹗$A"
      "接著在$N身邊突然出現許\多的$R火焰$Z﹐將$E吞沒﹗﹗" );
    break;

  case 7:
    sact( ch, victim, NULL,
      "$R$n$R揮舞雙手﹐發出了一連串高熱的火球"
      "﹐一個個猛地向$N狂擊﹐使得$E有一點喘不過氣來﹗﹗$Z" );
    break;

  case 8:
    sact( ch, victim, NULL,
      "$R$n集中精神﹐並且反覆不斷的唸誦一些咒文。"
      "$A$A接著一隻火精靈出現在$n身旁﹐臉上帶著些微的微笑﹗"
      "$A$A接著精靈狂喊一聲﹐發瘋似的衝向$N﹗$Z" );
    break;

  case 9:
    sact( ch, victim, NULL,
      "$R$n將雙手合一﹐不停的使勁﹐只見$e臉上漸漸轉為血紅色。"
      "$A$A接著只聽$e喊道﹕『$Y可惡的$N﹐納命來﹗﹗"
      "$R』$A接著一顆巨大的火球狂奔向$E﹗﹗$Z" );
    break;

  case 10:
    sact( ch, victim, NULL,
      "$R$n臥地沈思﹐卻發出一股巨大的震撼力﹗$A$A"
      "接著你見到天邊掉下一顆巨大著火的大火球衝向$N﹗$Z" );
    break;

  case 11:
    sact( ch, victim, NULL,
      "$R$n雙手一揮﹐不見任何事發生。\n\r\n\r"
      "幾秒鐘過後﹐一位全身火焰的童子急奔而至﹗\n\r"
      "二話不說他便向$N$R噴出一連串的『$Y三昧真火$R』。$Z" );
    break;

  case 12:
    sact( ch, victim, NULL,
      "$n身邊突然出現一些七彩的珠珠﹐圍繞著$e﹐剎是好看﹗$A"
      "然後圓珠漸漸消失﹐卻聽見大地開始震動﹐似乎即將發生一些什麼事"
      "﹗﹗$A剎時$n大喊一聲﹕『 $R巴   哈   姆   特 ﹗﹗﹗"
      "$Z』$A$A轉眼間在外太空某處一隻全身火紅的巨龍收到感應"
      "﹐向著此地噴出一個超高熱的火球﹗﹗$A$R火球即將轟到之時"
      "﹐此處已呈一片火海﹐眾草不生﹗﹗$A接著此地"
      "的人都失去知覺﹐全體都受到波及﹐尤其是$N﹗﹗$Z" );

    send_to_char( "\e[1;31m你也身受波及﹐火焰焚身﹗﹗\e[0m\n\r", ch );
    damage( ch, ch, dam * dam_plus , sn, ATTACK_RANDOM );

    /* 若施法者不幸陣亡 */
    if ( !can_damage( ch ) ) RETURN( TRUE );

    dam_plus *= 4;
    break;
  }

  damage( ch, victim, UMIN( LEVEL_FOU_DAM, dam * dam_plus ), sn, ATTACK_RANDOM );
  RETURN( TRUE );
}

SKILL( spell_cast_lightning )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         dam;
  int         dam_plus;
  int         choice[] =
              { 25, 35, 45, 55, 65, 75, 80, 85, 90, 95, 98, 99, 100, -1 };

  PUSH_FUNCTION( "spell_cast_lightining" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "spell_cast_lightning: 缺乏來源." );
    RETURN( FALSE );
  }

  dam = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_FOU );
  dam = dam / 2;

  switch( UMIN( 3, dam_plus = dimemsion_position( choice, level, TRUE ) ) )
  {
  case ERRORCODE:
    mudlog( LOG_DEBUG, "技能 %d 的訊息設定有問題." , sn );
    RETURN( FALSE );

  case 0:
    sact( ch, victim, NULL,
      "$S$n雙手一抖﹐發出一個小電球擊向$N﹗$Z" );
    break;

  case 1: case 2: case 3: case  4: case  5: case  6:
  case 7: case 8: case 9: case 10: case 11: case 12:
    sact( ch, victim, NULL,
      "$S$n兩手一伸﹐就從手中發出兩道霹靂有聲的閃電﹐猛惡的攻向$N﹗$Z" );
    break;
  }

  damage( ch, victim, UMIN( LEVEL_FOU_DAM, dam * dam_plus ), sn, ATTACK_RANDOM );

  RETURN( TRUE );
}

SKILL( cast_control_weather )
{
  PUSH_FUNCTION( "cast_control_weather" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "cast_control_weather: 來源錯誤. " );
    RETURN( FALSE );
  }

  if ( number_percent() > ( number_percent() / 2 + level - 10 ) )
  {
    send_to_char( "天氣似乎沒有什麼變化﹗\n\r"
      "你想把天氣變好 (\e[1;32mbetter\e[0m) "
      "或是變壞 (\e[1;32mworse\e[0m)﹖\n\r", ch );
    RETURN( FALSE );
  }

  if ( !str_cmp( ( char * ) vo , "better" ) )
  {
    weather_info.change += dice( level / 3, 4 );
    send_to_char( "好了﹐天氣越來越好了﹗\n\r", ch );
  }

  else if ( !str_cmp( ( char * ) vo , "worse" ) )
  {
    weather_info.change -= dice( level / 3, 4 );
    send_to_char( "好了﹐天氣越來越壞了﹗\n\r", ch );
  }

  else
  {
    send_to_char( "你想把天氣變好 (\e[1;32mbetter\e[0m) "
      "或是變壞 (\e[1;32mworse\e[0m)﹖\n\r", ch );

    RETURN( FALSE );
  }

  RETURN( TRUE );
}

SKILL( cast_sparkler )
{
  extern const sh_int   rev_dir [];
  char                  buf[MAX_STRING_LENGTH];
  CHAR_DATA           * pMount;
  ROOM_INDEX_DATA     * pRoom;
  ROOM_INDEX_DATA     * pSource;
  int                   count;
  int                   degree;
  int                   loop;
  int                   dir;

  PUSH_FUNCTION( "cast_sparkler" );

  if ( !( pSource = ch->in_room ) )
  {
    send_to_char( "對不起﹐但你目前在那裡呢﹖\n\r", ch );
    RETURN( FALSE );
  }

  if ( !IS_OUTSIDE( ch ) )
  {
    act( "你無法在$r施展這個煙火術﹗", ch, NULL, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  if ( ch->in_room->area == FoggyArea )
  {
    act( "目前$r一片霧濛濛﹐這時施展煙火術是沒有用的﹗", ch, NULL, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  if ( weather_info.sky == SKY_RAINING )
  {
    send_to_char( "下雨天是沒有辦法施展煙火術﹗\n\r", ch );
    RETURN( FALSE );
  }

  if ( ( pMount = get_mount( ch ) ) )
  {
    act( "你坐在$N上是沒有辦法施展煙火術的。", ch, NULL, pMount, TO_CHAR );
    RETURN( FALSE );
  }

  degree = UMIN( MAX_SCAN_DEPTH - 1, UMAX( 2, level / 10 ) );

  clean_scan();
  scan_room( ch->in_room->vnum, degree, -1 );

  for ( loop = 0; loop < ScanPointer; loop++ )
  {
    if ( !( pRoom = get_room_index( ScanData[loop].vnum ) ) ) continue;

    char_from_room( ch );
    char_to_room( ch, pRoom );

    str_cpy( buf, "在你的$2" );

    for ( count = MAX_SCAN_DEPTH - 1; count >= 0; count-- )
    {
      if ( ( dir = ScanData[loop].dir[count] < 0 ) || dir >= DIR_MAX )
        continue;

      str_cat( buf, direction_name( rev_dir[ScanData[loop].dir[count]] ) );
    }

    str_cat( buf, "$0方向出現某人施放的煙火火光﹐好不漂亮﹗" );
    act( smash_act_keyword( buf, "nes" ), ch, NULL, NULL, TO_ROOM );

    char_from_room( ch );
    char_to_room( ch, pSource );
  }

  clean_scan();

  act( "$n雙手反握﹐施展出煙火術﹐剎時天空一片火亮﹗", ch, NULL, NULL, TO_ALL );
  RETURN( TRUE );
}

SKILL( cast_search )
{
  ROOM_INDEX_DATA * pSource;
  ROOM_INDEX_DATA * pRoom;
  CHAR_DATA       * victim;
  char              buf[MAX_STRING_LENGTH];
  char            * pString = ( char * ) vo;
  int               loop;
  int               count;
  int               degree;
  int               height;
  int               number;

  PUSH_FUNCTION( "cast_search" );

  if ( !pString || *pString == '\x0' )
  {
    send_to_char( "你想要尋找什麼人呢﹖\n\r", ch );
    RETURN( FALSE );
  }

  if ( !( pSource = ch->in_room ) )
  {
    send_to_char( "對不起﹐但你目前在那裡呢﹖\n\r", ch );
    RETURN( FALSE );
  }

  if ( !IS_OUTSIDE( ch ) )
  {
    act( "你無法在$r施展這個騰空尋人術﹗", ch, NULL, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  if ( ch->in_room->area == FoggyArea )
  {
    act( "目前$r一片霧濛濛﹐這時施展騰空尋人是沒有用的﹗", ch, NULL, NULL, TO_CHAR  );
    RETURN( FALSE );
  }

  if ( weather_info.sky == SKY_RAINING )
  {
    send_to_char( "下雨天是沒有辦法施展騰空尋人術﹗\n\r", ch );
    RETURN( FALSE );
  }

  if ( ( victim = get_mount( ch ) ) )
  {
    act( "你坐在$N上是沒有辦法施展騰空尋人術的。", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  degree = UMIN( MAX_SCAN_DEPTH - 1, UMAX( 2, level / 10 ) );
  height = degree * 2;

  act( "$n縱身一跳﹐約有$i丈高﹐正努力往下觀察地面形勢﹗"
    , ch, &height, NULL, TO_ALL );

  clean_scan();
  scan_room( ch->in_room->vnum, degree, -1 );

  for ( number = loop = 0; loop < ScanPointer; loop++ )
  {
    if ( !( pRoom = get_room_index( ScanData[loop].vnum ) ) ) continue;

    char_from_room( ch );
    char_to_room( ch, pRoom );

    act( "遠方$n縱身一跳﹐約有$i丈高﹐正努力往下觀察﹗"
      , ch, &height, NULL, TO_ROOM );

    for ( victim = pRoom->people; victim; victim = victim->next_in_room )
    {
      if ( ch != victim
        && can_see( ch, victim )
        && is_fullname( pString, victim->name ) )
      {
        number++;
        str_cpy( buf, "在你的$2" );

        for ( count = 0; count < MAX_SCAN_DEPTH; count++ )
        {
          if ( ScanData[loop].dir[count] < 0 ) break;
          str_cat( buf, direction_name( ScanData[loop].dir[count] ) );
        }

        str_cat( buf, "$0有$N﹐不知道是不是你要找的對象﹗" );
        act( smash_act_keyword( buf, "nesNES" ), ch, NULL, victim, TO_CHAR );
      }
    }

    char_from_room( ch );
    char_to_room( ch, pSource );

    if ( number >= UMAX( 1, degree / 2 ) ) break;
  }

  if ( number <= 0 ) send_to_char( "對不起﹐找不到你要尋找的人耶﹗\n\r", ch );

  RETURN( TRUE );
}

SKILL( cast_locate_object )
{
  int        count;
  char       buf[MAX_STRING_LENGTH];
  OBJ_DATA * obj;
  OBJ_DATA * in_obj;
  char     * pString = ( char * ) vo;
  bool       found;

  PUSH_FUNCTION( "cast_locate_object" );

  if ( !pString || *pString == '\x0' )
  {
    send_to_char( "你要如何展現你的大神通呢﹖\n\r", ch );
    RETURN( FALSE );
  }

  act( "$n低頭閉目﹐開始施展遊識神通。", ch, NULL, NULL, TO_ALL );
  buf[0] = '\x0';

  clear_buffer();
  for ( count = 0, found = FALSE, obj = object_list; obj; obj = obj->next )
  {
    if ( buffer_full() ) break;
    if ( count >= ( level / 2 ) )
    {
      if ( found ) send_to_buffer( "\n\r你搜尋能力已經耗盡﹗\n\r" );
      break;
    }

    if ( !verify_obj( obj )
      || !can_see_obj( ch, obj )
      || !is_name( pString, obj->name ) )
      continue;

    for ( in_obj = obj; in_obj->in_obj; in_obj = in_obj->in_obj );
    if ( !in_obj || !verify_obj( in_obj ) ) continue;

    found = TRUE;
    count++;
    if ( in_obj->carried_by )
    {
      send_to_buffer( "一陣雷電過後﹐你閃過一個念頭﹐啊﹐%s 正由 %s 攜帶。\n\r"
        , obj_name( ch, obj )
        , mob_name( ch, in_obj->carried_by ) );
    }
    else
    {
      if ( level < ( LEVEL_HERO / 2 ) )
      {
        send_to_buffer( "遠方傳來一陣聲音﹐雖然被你發現了﹐"
          "可是以你的功\力是找不到我的﹐哈哈﹗\n\r" );
      }
      else
      {
        send_to_buffer( "突然閃過一陣白光﹐你發現原來 %s\e[0m 在 %s\e[0m 裡面﹗\n\r"
          , obj_name( ch, obj )
          , in_obj->in_room == NULL ? "某個地方" : in_obj->in_room->name );
      }
    }

  }

  if ( !found )
  {
    send_to_char( "你聽見幽幽處傳來一陣聲音﹐"
      "這附近找不到你要的物品﹐讓你覺得有一股毛骨悚然的感覺。\n\r", ch );
  }
  else
  {
    print_buffer( ch );
  }

  RETURN( TRUE );
}

SKILL( cast_summon )
{
  CHAR_DATA * victim;

  PUSH_FUNCTION( "cast_summon" );

  if ( !ch->in_room )
  {
    mudlog( LOG_DEBUG, "cast_summon: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( !( victim = get_char_area( ch, (char *) vo ) )
    || !verify_char( victim )
    || !victim->in_room )
  {
    act( "這附近沒有這個人喔﹗", ch, NULL, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  if ( victim == ch )
  {
    act( "你沒事召喚自己做什麼﹖", ch, NULL, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  if ( ch->in_room == victim->in_room )
  {
    act( "$n跟$N只有一線之隔吧﹗何必還要召喚呢﹖"
      , ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  if ( is_loge( victim->in_room )
    || victim->in_room->Safe
    || victim->in_room->Private
    || victim->in_room->NoRecall
    || victim->in_room->NoWhere
    || victim->in_room->Sail
    || victim->in_room->NoQuit
    || ch->in_room->NoWhere )
  {
    act( "$n的法術失敗了﹐$N的位置看來不能夠使用召喚術喔﹗"
      , ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  if ( victim->fighting
    || victim->mount
    || victim->mount_by
    || victim->boss
    || victim->spirit
    || victim->position != POS_STANDING )
  {
    act( "$n失敗了﹐$N正在忙呢﹐聽也聽不到你的召喚﹗"
      , ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  if ( IS_NPC( victim ) && IS_SET( victim->act, ACT_TRAIN ) )
  {
    act( "請不要把$N叫來叫去﹗", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  if ( is_affected( victim, SLOT_CURSE )
    || is_affected( ch, SLOT_CURSE )
    || victim->level >= level + 3
    || is_keeper( victim )
    || IS_SET( victim->act, ACT_NOSUMMON ) )
  {
    send_to_char( "天邊傳來一陣笑聲嘲笑你﹐你的召喚失敗了。\n\r", ch );
    RETURN( FALSE );
  }

  if ( is_affected( victim, SLOT_FIXITY ) )
  {
    act( "$N好像聽不到你的召喚﹗", ch, NULL, victim, TO_CHAR );
    RETURN( TRUE );
  }

  act( "$n在一陣白煙籠罩之下突然消失不見了﹗", victim, NULL, NULL, TO_ROOM );
  char_from_room( victim );
  char_to_room( victim, ch->in_room );
  act( "$n突然之間從一道升起的門裡面走了出來﹗", victim, NULL, NULL, TO_ROOM );
  act( "$N正在嘗試著召喚你﹗", ch, NULL, victim,   TO_VICT );
  do_look( victim, "auto" );

  /* 清除追蹤紀錄點 */
  clear_trace( victim, TRUE );

  RETURN( TRUE );
}

SKILL( cast_ventriloquate )
{
  char      * pString = ( char * ) vo;
  char        buf1[MAX_STRING_LENGTH];
  char        buf2[MAX_STRING_LENGTH];
  char        speaker[MAX_INPUT_LENGTH];
  CHAR_DATA * vch;

  PUSH_FUNCTION( "cast_ventriloquate" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "cast_ventriloquate: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( !ch->in_room || !can_see_room( ch ) ) RETURN( FALSE );

  pString = one_argument( pString, speaker );
  sprintf( buf1, "%s 說道: '%s'。\n\r",     speaker, pString );
  sprintf( buf2, "某人叫 %s 說 '%s'。\n\r", speaker, pString );

  for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
  {
    if ( !is_name( speaker, vch->name ) )
      send_to_char( ( vch->level >= ch->level ) ? buf2 : buf1, vch );
  }

  RETURN( TRUE );
}

SKILL( cast_acid_blast )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         dam;

  PUSH_FUNCTION( "cast_acid_blast" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_acid_blast: 缺乏來源." );
    RETURN( FALSE );
  }

  dam = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_ONE );

  act( "$n催動法力﹐將口中變成強酸﹐一口朝$N吐去。"
    , ch, NULL, victim, TO_ALL );

  damage( ch, victim, dam, sn, ATTACK_RANDOM );
  if ( !can_damage( victim ) ) RETURN( TRUE );

  if ( level > 50 )
  {
    sact( ch, victim, NULL,
      "$n口中吐出的酸液落下﹐冒出陣陣白煙﹐這對$N造成很大的傷害。" );
    damage( ch, victim, dam * 2, sn, ATTACK_RANDOM );
  }

  RETURN( TRUE );
}

SKILL( cast_blindness )
{
  CHAR_DATA   * vic = ( CHAR_DATA * ) vo;
  AFFECT_DATA   af;
  int           chance;

  PUSH_FUNCTION( "cast_blindness" );

  if ( !vic )
  {
    mudlog( LOG_DEBUG, "cast_blindess: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( !can_fight( ch, vic ) )
  {
    act( "你不能弄瞎$N的眼。", ch, NULL, vic, TO_CHAR );
    RETURN( FALSE );
  }

  if ( is_affected( vic, sn ) )
  {
    act( "對不起﹐$N已經眼盲了﹗", ch, NULL, vic, TO_CHAR );
    RETURN( FALSE );
  }

  if ( !can_damage( vic ) )
  {
    act( "你目前沒有辦法弄瞎$N﹗", ch, NULL, vic, TO_CHAR );
    RETURN( FALSE );
  }

  chance = UMAX( 30 , level ) + get_curr_dex( ch ) - get_curr_dex( vic );
  chance = UMAX( 5, chance );

  if ( number_range( 1, 100 ) > chance )
  {
    send_to_char( "對不起﹐你失敗了﹗\n\r", ch );
    RETURN( FALSE );
  }

  act( "$n用無比的心靈震盪召來黑暗﹐剎那間﹐雙手黑暗的$n往$N一揮﹐"
       "將此黑暗附在$N身上。", ch, NULL, vic, TO_ALL );

  af.type      = sn;
  af.location  = APPLY_HITROLL;
  af.modifier  = 0 - ( level / 2 );
  af.duration  = 1 + ( level / 5 );
  af.level      = level;
  affect_to_char( vic, &af );
  send_to_char( "黑光一閃﹐你暫時眼盲了﹗\n\r", vic );
  if ( ch != vic ) send_to_char( "成功\施展眼盲術了﹗\n\r", ch );
  RETURN( TRUE );
}

SKILL( cast_burning_hands )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         dam    = 0;

  PUSH_FUNCTION( "cast_burning_hands" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_burning_hands: 缺乏來源." );
    RETURN( FALSE );
  }

  /* 設定敘述 */
  if ( number_percent() > level )
  {
    sact( ch, victim, NULL,
      "$n使盡全力﹐奮力將雙掌催上法力使之發紅起$R火$Z﹐向$N飛去﹗" );
  }
  else
  {
    sact( ch, victim, NULL,
      "$n順手一揮﹐一個$R冒火的手掌$Z在手上形成﹐立即以很快的速度向"
      "$N飛去﹐發出一道紅紅的弧線﹗" );
  }

  /* 設定攻傷 */
  dam = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_ONE );

  damage( ch, victim, dam, sn, ATTACK_RANDOM );
  RETURN( TRUE );
}

SKILL( cast_call_lightning )
{
  CHAR_DATA * vch;
  CHAR_DATA * vch_next;
  int         dam;

  PUSH_FUNCTION( "cast_call_lighting" );

  if ( !IS_OUTSIDE( ch ) )
  {
    send_to_char( "你必須要在戶外﹐不然沒辦法放電。\n\r", ch );
    RETURN( FALSE );
  }

  if ( level < 70 && weather_info.sky < SKY_RAINING )
  {
    send_to_char( "天氣好的很﹐無法施展這個法術。\n\r", ch );
    RETURN( FALSE );
  }

  /* 設定攻傷 */
  dam = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_THR );
  dam = dam / 2;

  if ( level < 50 )
    send_to_char( "天空突然打起雷來﹗\n\r", ch );
  else
    send_to_char( "你呼叫一片雨雲﹐帶來一陣閃電﹐攻擊所有在場的敵人﹗\n\r"
      ,ch );

  act( "$n呼叫天雷﹐打向這裡所有的敵人﹗", ch, NULL, NULL, TO_ROOM );

  for ( vch = char_list; vch; vch = vch_next )
  {
    vch_next = vch->next;

    if ( !verify_char( vch ) || !vch->in_room ) continue;

    if ( vch->in_room == ch->in_room )
    {
      if ( vch != ch && ( IS_NPC( ch ) ? !IS_NPC( vch ) : IS_NPC( vch ) ) )
        damage( ch, vch, saves_spell( level, vch ) ? dam/2 : dam, sn, ATTACK_RANDOM );

      continue;
    }

    if ( vch->in_room->area == ch->in_room->area
      && IS_OUTSIDE( vch )
      && IS_AWAKE( vch ) )
      send_to_char( "一下閃電劃過天邊﹐照耀著整個天空。\n\r", vch );
  }

  RETURN( TRUE );
}

SKILL( cast_cause_light )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         dam;

  PUSH_FUNCTION( "cast_cause_light" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_cause_light: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( level < 50 )
    sact( ch, victim, NULL,
      "$n輕念咒語﹐瞬間在$N四周出現不知名的黑暗﹐侵蝕著$E﹗" );
  else
    sact( ch, victim, NULL,
      "$n唸出一段艱深的咒語﹐將$N包附在黑暗之中﹐毫無脫身餘地﹗" );

  /* 設定攻傷 */
  dam = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_ONE );

  damage( ch, victim, dam, sn, ATTACK_RANDOM );
  RETURN( TRUE );
}

SKILL( cast_cause_critical )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         dam;

  PUSH_FUNCTION( "cast_cause_critical" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_cause_critical: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( level < 50 )
    sact( ch, victim, NULL,
      "$n雙手一覆﹐閃出一片黑暗﹐侵蝕著$N的心靈。\n\r"
      "只見$N在一旁慘嚎著﹐似乎很痛苦﹗" );
  else
    sact( ch, victim, NULL,
      "$n雙手畫圓﹐推出一個可見的黑暗球體飛向$N。\n\r"
      "$N被球體包覆﹐似乎極為痛苦﹗" );

  dam = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_FOU );

  damage( ch, victim, dam, sn, ATTACK_RANDOM );
  RETURN( TRUE );
}

SKILL( cast_cause_serious )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         dam;

  PUSH_FUNCTION( "cast_cause_serious" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_cause_serious: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( level < 50 )
    sact( ch, victim, NULL,
      "$n向撒旦祈求黑暗﹐並將此黑暗附在$N身上。" );
  else
    sact( ch, victim, NULL,
      "$n突然跪下祈求﹕「萬能的撒旦啊﹗將無比的黑暗附在$N身上吧﹗" );

  /* 設定攻傷 */
  dam = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_TWO );

  damage( ch, victim, dam, sn, ATTACK_RANDOM );
  RETURN( TRUE );
}

SKILL( cast_change_sex )
{
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA   af;

  PUSH_FUNCTION( "cast_change_sex" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_change_sex: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( is_affected( victim, sn ) )
  {
    act( "這看起來對$N似乎起不了作用﹗", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  af.type      = sn;
  af.duration  = 10 * level;
  af.location  = APPLY_SEX;

  switch( victim->sex )
  {
  case SEX_FEMALE:
    af.modifier = SEX_MALE - SEX_FEMALE;
    break;

  case SEX_MALE:
    af.modifier = SEX_FEMALE - SEX_MALE;
    break;

  default:
    act( "$N無法進行變性.", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  af.level     = level;
  affect_to_char( victim, &af );

  act( "$n手結聖印﹐專心祈禱﹐天邊突然降下一朵七彩的雲﹗$A"
       "$n瞬間被這朵雲包著﹐不知道發生了什麼事﹗$A"
       "接著﹐雲淡風清﹐似乎什麼都沒有發生過。"
       , ch, NULL, victim, TO_ALL );

  send_to_char( "你感覺身上似乎起了一些怪怪的變化﹗\n\r", victim );
  if ( ch != victim ) send_to_char( "好了﹐變性完畢。\n\r", ch );
  RETURN( TRUE );
}

SKILL( cast_charm_person )
{
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA   af;

  PUSH_FUNCTION( "cast_charm_person" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_charm_person: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( victim == ch )
  {
    send_to_char( "生命是很可貴的﹐請多加珍惜﹗\n\r", ch );
    RETURN( FALSE );
  }

  if ( !can_damage( victim ) )
  {
    act( "你目前沒有辦法迷惑$N﹗\n\r", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  /* 若對象是玩家且不是盜賊或是殺人犯則不行 */
  if ( is_pk( ch ) && is_pk( victim ) )
  {
    if ( fight_info->status != PK_FIGHT )
    {
      send_to_char( "在這段時間﹐還是好好地休息一下吧﹗\n\r", ch );
      RETURN( FALSE );
    }
  }

  else if ( !IS_NPC( victim ) )
  {
    if ( !IS_SET( victim->act, PLR_KILLER )
      && !IS_SET( victim->act, PLR_THIEF  )
      && !IS_SET( victim->act, PLR_BOLTER ) )
    {
      act( "你要攻擊$N﹐請用 $2MURDER$0 這個指令。", ch, NULL, victim, TO_CHAR );
      RETURN( FALSE );
    }
  }

  if ( !can_fight( ch , victim ) ) RETURN( FALSE );

  if ( victim->boss )
  {
    act( "對不起﹐你不能迷惑$N的小鬼﹗", ch, NULL, victim->boss, TO_CHAR );
    RETURN( FALSE );
  }

  if ( is_affected( victim, sn )
    || is_affected( ch, sn )
    || victim->fighting
    || saves_spell( level, victim ) )
  {
    send_to_char( "哎呀﹗迷惑的法術失敗了﹗\n\r", ch );
    RETURN( TRUE );
  }

  if ( victim->level > ( level / 10  + number_range( 1, level ) ) )
  {
    sact( ch, victim, NULL, "$N突然不受$n的控制﹐對$n展開攻擊﹗" );
    set_fighting( victim, ch );
    RETURN( FALSE );
  }

  if ( victim->master ) stop_follower( victim );

  add_follower( victim, ch );
  af.type      = sn;
  af.duration  = number_fuzzy( level / 5 );
  af.location  = 0;
  af.modifier  = 0;
  af.level     = level;
  affect_to_char( victim, &af );

  act( "突然之間﹐你覺得$n好像你失散多年的主人﹗", ch, NULL, victim, TO_VICT );
  if ( ch != victim ) send_to_char( "好了﹐迷惑成功\﹗\n\r", ch );
  RETURN( TRUE );
}

SKILL( cast_chill_touch )
{
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA   af;
  int           dam;

  PUSH_FUNCTION( "cast_chill_touch" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_chill_touch: 缺乏來源." );
    RETURN( FALSE );
  }

  dam = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_ONE );

  if ( !saves_spell( level, victim ) && get_curr_str( victim ) > 1 )
  {
    af.type      = sn;
    af.duration  = number_range( level / 15, level / 12 );
    af.location  = APPLY_STR;
    af.modifier  = -1;
    af.level     = level;
    affect_join( victim, &af );
  }
  else
  {
    dam >>= 1;
  }

  damage( ch, victim, dam, sn, ATTACK_RANDOM );
  RETURN( TRUE );
}

SKILL( cast_colour_spray )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         dam;

  PUSH_FUNCTION( "cast_colour_spray" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "cast_colour_spray: 缺乏來源." );
    RETURN( FALSE );
  }

  /* 攻傷敘述 */
  sact( ch, victim, NULL,
    "$n雙手一放﹐手中發出一道$R七$Y色$G彩$S虹$Z擊向$N﹗" );

  /* 設定攻傷 */
  dam = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_ONE );

  if ( saves_spell( level, victim ) ) dam /= 2;
  damage( ch, victim, dam, sn, ATTACK_RANDOM );
  RETURN( TRUE );
}

SKILL( cast_create_food )
{
  OBJ_DATA * mushroom;
  OBJ_DATA * obj;
  int        choice[] = { 35, 55, 70, 100, -1 };

  PUSH_FUNCTION( "cast_create_food" );

  if ( !ch->in_room )
  {
    mudlog( LOG_DEBUG, "cast_create_food: 缺乏來源." );
    RETURN( FALSE );
  }

  for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
    if ( obj->item_type == ITEM_FOOD ) break;

  if ( obj )
  {
    act( "$r裡食物豐富﹐快吃吧﹐還做什麼食物﹗", ch, NULL, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  if ( ch->in_room->nobject >= MaxRoomObject )
  {
    send_to_char( "這裡的物品太多了﹐請先奉獻掉一些吧﹗\n\r", ch );
    RETURN( FALSE );
  }

  switch( dimemsion_position( choice, level , TRUE ) )
  {
  default:

    mudlog( LOG_DEBUG, "cast_create_food: 超出範圍." );
    RETURN( FALSE );

  case 0:

    if ( !( mushroom = create_object( ObjMushroom, 1 ) ) )
    {
      mudlog( LOG_DEBUG, "cast_create_food: 沒有食物: %d.", ObjMushroomVnum );
      RETURN( FALSE );
    }
    break;

  case 1:

    if ( !( mushroom = create_object( ObjDumpling, 1 ) ) )
    {
      mudlog( LOG_DEBUG, "cast_create_food: 沒有食物: %d.", ObjDumplingVnum );
      RETURN( FALSE );
    }
    break;

  case 2:

    if ( !( mushroom = create_object( ObjBougi, 1 ) ) )
    {
      mudlog( LOG_DEBUG, "cast_create_food: 沒有食物: %d.", ObjBougiVnum );
      RETURN( FALSE );
    }
    break;

  case 3:

    if ( !( mushroom = create_object( ObjPon, 1 ) ) )
    {
      mudlog( LOG_DEBUG, "cast_create_food: 沒有食物: %d.", ObjPonVnum );
      RETURN( FALSE );
    }
    break;

  case 4:

    if ( !( mushroom = create_object( ObjChicken, 1 ) ) )
    {
      mudlog( LOG_DEBUG, "cast_create_food: 沒有食物: %d.", ObjChickenVnum );
      RETURN( FALSE );
    }
    break;
  }

  mushroom->value[0] = 5 + level;
  obj_to_room( mushroom, ch->in_room );
  act( "$p突然由地上冒了出來。", ch, mushroom, NULL, TO_ROOM );
  act( "$p在一堆白光中由地上冒了出來。", ch, mushroom, NULL, TO_CHAR );
  RETURN( TRUE );
}

SKILL( cast_create_water )
{
  OBJ_DATA * obj = ( OBJ_DATA * ) vo;
  int water;

  PUSH_FUNCTION( "cast_create_water" );

  if ( !obj )
  {
    mudlog( LOG_DEBUG, "cast_create_water: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( obj->item_type != ITEM_DRINK_CON )
  {
    act( "$p不可以拿來裝水喔。", ch, obj, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  if ( obj->value[2] != liq_water->slot && obj->value[1] != 0 )
  {
    act( "$p裡面已經有其他液體囉。", ch, obj, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  water = UMIN( level * ( weather_info.sky >= SKY_RAINING ? 4 : 2 )
                , obj->value[0] - obj->value[1] );

  if ( water > 0 )
  {
    obj->value[2]  = liq_water->slot;
    obj->value[1] += water;

    if ( !is_name( "water", obj->name ) )
    {
      char buf[MAX_STRING_LENGTH];

      sprintf( buf, "%s water", obj->name );
      free_string( obj->name );
      obj->name = str_dup( buf );
    }

    act( "$p的水快滿出來了﹗", ch, obj, NULL, TO_CHAR );
  }

  RETURN( TRUE );
}

SKILL( cast_cure_blindness )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;

  PUSH_FUNCTION( "cast_cure_blindness" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_cure_blindness: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( is_affected( victim, SLOT_BLINDNESS ) )
  {
    affect_release( victim, SLOT_BLINDNESS );
    send_to_char( "你又重見光明了﹗\n\r", victim );

    if ( ch != victim ) send_to_char( "醫治眼盲成功\﹗\n\r", ch );
    RETURN( TRUE );
  }
  else
  {
    act( "$N的眼睛十分的正常啊﹗", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  RETURN( FALSE );
}

SKILL( cast_cure_critical )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         heal;

  PUSH_FUNCTION( "cast_cure_critical" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_cure_critical: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( victim->hit >= get_curr_hit( victim ) )
  {
    act( "$N的生命已經足夠了﹗", ch , NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  heal = ( number_range( 50, 100 ) + level * 2 ) * get_curr_int( ch ) / 4;
  victim->hit = UMIN( victim->hit + heal, get_curr_hit( victim ) );

  if ( victim->position == POS_SLEEPING && victim != ch )
    act( "$N的醫治行動驚動了你﹗", victim, NULL, ch, TO_CHAR );

  update_pos( victim );
  send_to_char( "你看來似乎比較好了﹗\n\r", victim );

  if ( victim->hit == get_curr_hit( victim ) )
    send_to_char( "你全身的傷口都消失了﹗\n\r", victim );

  if ( ch != victim ) send_to_char( "好了﹐醫治成功\﹗\n\r", ch );
  RETURN( TRUE );
}

SKILL( cast_cure_overdose )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         num;

  PUSH_FUNCTION( "cast_cure_overdose" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_cure_overdose: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( !victim->pcdata || victim->pcdata->dosage <= 0 )
  {
    act( "$N並沒有藥物中毒喔﹗", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  num = UMAX( 1, number_range( level / 4 , level / 2 ) );
  victim->pcdata->dosage = UMAX( 0, victim->pcdata->dosage - num );

  if ( victim->position == POS_SLEEPING && victim != ch )
    act( "$N的醫治行動驚動了你﹗", victim, NULL, ch, TO_CHAR );

  update_pos( victim );

  if ( victim->pcdata->dosage <= 0 )
  {
    send_to_char( "你身上的藥物餘毒都消失了﹗\n\r", victim );
  }
  else
  {
    send_to_char( "你身上的餘毒消退了不少﹗\n\r", victim );
  }

  if ( ch != victim ) send_to_char( "施法好了。\n\r", ch );

  RETURN( TRUE );
}

SKILL( cast_remeday )
{
  CHAR_DATA * victim;
  int         percent;
  int         heal;

  PUSH_FUNCTION( "cast_remeday" );

  if ( !ch->in_room )
  {
    send_to_char( "你身處絕境﹐無法施展此法術﹗\n\r", ch );
    RETURN( FALSE );
  }

  percent = number_range( level * get_curr_int( ch ) * 70
                        , level * get_curr_int( ch ) * 90 );

  percent = UMAX( 1, UMIN( 100, percent / 7000 ) );

  for ( victim = char_list; victim; victim = victim->next )
  {
    if ( verify_char( victim )
      && victim->in_room
      && is_same_group( victim, ch )
      && victim->in_room->vnum == ch->in_room->vnum
      && victim->hit < victim->max_hit )
    {
      heal = victim->max_hit * percent / 100;
      victim->hit = UMIN( victim->max_hit, victim->hit + heal );
      act( "$N的妙手回春﹐使你由死亡邊緣拉了回來﹗", victim, NULL, ch, TO_CHAR );
    }
  }

  send_to_char( "你的元氣大傷﹐得好好修養﹗\n\r", ch );
  RETURN( TRUE );
}

SKILL( cast_cure_light )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         heal;

  PUSH_FUNCTION( "cast_cure_light" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_cure_light: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( victim->hit >= get_curr_hit( victim ) )
  {
    act( "$N的生命已經足夠了﹗", ch , NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  heal = ( number_range( 40, 80 ) + level / 2 ) * get_curr_int( ch ) / 4;
  victim->hit = UMIN( victim->hit + heal, get_curr_hit( victim ) );

  if ( victim->position == POS_SLEEPING && victim != ch )
    act( "$N的醫治行動驚動了你﹗", victim, NULL, ch, TO_CHAR );

  update_pos( victim );
  send_to_char( "你感覺好多了﹗\n\r", victim );

  if ( victim->hit == get_curr_hit( victim ) )
    send_to_char( "你全身的傷口都消失了﹗\n\r", victim );

  if ( ch != victim ) send_to_char( "施法好了。\n\r", ch );

  RETURN( TRUE );
}

SKILL( cast_cure_poison )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;

  PUSH_FUNCTION( "cast_cure_poison" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_cure_poison: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( is_affected( victim, SLOT_POISON ) )
  {
    affect_release( victim, SLOT_POISON );
    act( "$N看起來好多了", ch, NULL, victim, TO_NOTVICT );
    send_to_char( "一陣清流流入你的水中﹐不舒服的感覺不再。\n\r", victim );
    send_to_char( "好了。\n\r", ch );
    RETURN( TRUE );
  }

  else
  {
    act( "$N覺得沒有什麼改變﹗", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  RETURN( FALSE );
}

SKILL( cast_cure_serious )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         heal;

  PUSH_FUNCTION( "cast_cure_serious" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_cure_serious: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( victim->hit >= get_curr_hit( victim ) )
  {
    act( "$N的生命已經非常足夠了﹗", ch , NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  heal = ( number_range( 50, 100 ) + level ) * get_curr_int( ch ) / 4;
  victim->hit = UMIN( victim->hit + heal, get_curr_hit( victim ) );

  if ( victim->position == POS_SLEEPING && victim != ch )
    act( "$N的醫治行動驚動了你﹗", victim, NULL, ch, TO_CHAR );

  update_pos( victim );
  send_to_char( "你感覺好多了﹗\n\r", victim );

  if ( victim->hit == get_curr_hit( victim ) )
    send_to_char( "你的傷口都已經消失了﹗\n\r", victim );

  if ( ch != victim ) send_to_char( "好了。\n\r", ch );

  RETURN( TRUE );
}

SKILL( cast_curse )
{
  CHAR_DATA   * vic = ( CHAR_DATA * ) vo;
  AFFECT_DATA   af;

  PUSH_FUNCTION( "cast_curse" );

  if ( !vic )
  {
    mudlog( LOG_DEBUG, "cast_curse: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( is_affected( vic, sn ) || saves_spell( level, vic ) )
  {
    act( "你無法詛咒$N﹗", ch, NULL, vic, TO_CHAR );
    RETURN( FALSE );
  }

  if ( vic->alignment > 0 && ( vic->alignment / 10 > level ) )
  {
    act( "由$N的頭上升起了一團的白色光芒﹐阻止了$n的詛咒﹗"
        , ch, NULL, vic, TO_ALL );

    RETURN( FALSE );
  }

  af.type      = sn;
  af.duration  = UMAX( 1, number_range( level / 4, level / 2 ) );
  af.location  = APPLY_HITROLL;
  af.modifier  = -1;
  af.level     = level;
  affect_to_char( vic, &af );

  af.location  = APPLY_SAVING_SPELL;
  af.modifier  = 1;
  affect_to_char( vic, &af );

  send_to_char( "你感到一陣暗黑壓抑著你的力量。\n\r", vic );
  if ( ch != vic ) send_to_char( "詛咒成功\。\n\r", ch );
  RETURN( TRUE );
}

SKILL( cast_detect_evil )
{
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA   af;

  PUSH_FUNCTION( "cast_detect_evil" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_detect_evil: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( is_affected( victim, sn ) )
  {
    send_to_char( "這看起來似乎沒有任何效果﹗\n\r", ch );
    RETURN( FALSE );
  }

  af.type      = sn;
  af.duration  = number_fuzzy( level / 3 );
  af.modifier  = 0;
  af.location  = APPLY_NONE;
  af.level     = level;
  affect_to_char( victim, &af );
  send_to_char( "你的眼睛發出了一陣火紅的光芒。\n\r", victim );
  if ( ch != victim ) send_to_char( "好了。\n\r", ch );
  RETURN( TRUE );
}

SKILL( cast_detect_hidden )
{
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA   af;

  PUSH_FUNCTION( "cast_detect_hidden" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_detect_hidden: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( is_affected( victim, sn ) )
  {
    send_to_char( "你的眼睛已經夠銳利﹗\n\r", victim );
    RETURN( FALSE );
  }

  af.type      = sn;
  af.duration  = level;
  af.location  = APPLY_NONE;
  af.modifier  = 0;
  af.level     = level;
  affect_to_char( victim, &af );
  send_to_char( "你覺得四周都看得很清楚。\n\r", victim );
  if ( ch != victim ) send_to_char( "好了。\n\r", ch );
  RETURN( TRUE );
}

SKILL( cast_detect_invis )
{
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA   af;

  PUSH_FUNCTION( "cast_detect_invis" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_detect_invis: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( is_affected( victim, sn ) )
  {
    send_to_char( "你的眼睛已經夠犀利了﹗\n\r", ch );
    RETURN( FALSE );
  }

  af.type      = sn;
  af.duration  = number_range( level / 5, level / 3 );
  af.modifier  = 0;
  af.location  = APPLY_NONE;
  af.level     = level;
  affect_to_char( victim, &af );

  send_to_char( "你似乎可以看到一些不同的東西。\n\r", victim );
  if ( ch != victim ) send_to_char( "好了﹗\n\r", ch );
  RETURN( TRUE );
}

SKILL( cast_detect_magic )
{
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA  af;

  PUSH_FUNCTION( "cast_detect_magic" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_detect_magic: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( is_affected( victim, sn ) )
  {
    send_to_char( "這似乎起不了任何作用﹗\n\r", victim );
    RETURN( FALSE );
  }

  af.type      = sn;
  af.duration  = level;
  af.modifier  = 0;
  af.location  = APPLY_NONE;
  af.level     = level;
  affect_to_char( victim, &af );
  send_to_char( "你的眼睛更銳利了。\n\r", victim );
  if ( ch != victim ) send_to_char( "好了。\n\r", ch );
  RETURN( TRUE );
}

SKILL( cast_detect_mask )
{
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA  af;

  PUSH_FUNCTION( "cast_detect_mask" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_detect_mask: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( is_affected( victim, sn ) )
  {
    send_to_char( "這似乎起不了任何作用﹗\n\r", victim );
    RETURN( FALSE );
  }

  af.type      = sn;
  af.duration  = UMAX( 1, level / 5 );
  af.modifier  = 0;
  af.location  = APPLY_NONE;
  af.level     = level;
  affect_to_char( victim, &af );

  send_to_char( "你的眼睛可以看到易容的人了。\n\r", victim );
  if ( ch != victim ) send_to_char( "好了。\n\r", ch );

  RETURN( TRUE );
}

SKILL( cast_detect_poison )
{
  OBJ_DATA * obj = ( OBJ_DATA * ) vo;

  PUSH_FUNCTION( "cast_detect_poison" );

  if ( !ch || !obj )
  {
    mudlog( LOG_DEBUG, "cast_detect_poison: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( obj->item_type == ITEM_DRINK_CON || obj->item_type == ITEM_FOOD )
  {
    if ( obj->value[3] == 0 ) act( "$p看來不錯吃。", ch, obj, NULL, TO_CHAR );
    else  act( "$p好像有一點異味。", ch, obj, NULL, TO_CHAR );

    RETURN( TRUE );
  }
  else
  {
    act( "$p看起來並沒有毒吧﹗", ch, obj, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  RETURN( FALSE );
}

SKILL( cast_dispel_magic )
{
  AFFECT_DATA * paf;
  AFFECT_DATA * paf_next;
  SKILL_DATA  * pSkill;
  int           chance;
  int           lev;

  PUSH_FUNCTION( "cast_dispel_magic" );

  if ( !( paf = ch->affected ) )
  {
    send_to_char( "對不起﹐你身上沒有任何的法術影響﹗\n\r", ch );
    RETURN( FALSE );
  }

  for ( paf = ch->affected; paf; paf = paf_next )
  {
    paf_next = paf->next;
    chance   = 0;
    lev      = UMAX( 1, UMIN( MAX_LEVEL, paf->level ) );

    if ( !( pSkill = get_skill( paf->type ) ) ) continue;

    switch( pSkill->type )
    {
    case TAR_CHAR_SELF:
    case TAR_CHAR_DEFENSIVE:
      chance = 50 + ( level - paf->level ) * 2;
      break;

    default:
    case TAR_CHAR_OFFENSIVE:
      chance = 20 + ( level - paf->level ) * 3;
      break;
    }

    if ( number_range( 1, 100) <= chance )
    {
      act( "影響你身體的$3$w$0已經被移除了﹗", ch, pSkill, NULL, TO_CHAR );
      affect_remove( ch, paf );
    }

    else
    {
      act( "你身上的$3$w$0尚未被移除﹗", ch, pSkill, NULL, TO_CHAR );
    }
  }

  RETURN( TRUE );
}

SKILL( cast_dispel_evil )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         dam;

  PUSH_FUNCTION( "cast_dispel_evil" );

  if ( !IS_NPC( ch ) && IS_EVIL( ch ) ) victim = ch;

  if ( IS_GOOD( victim ) )
  {
    act( "$N受到天神的保護﹐你傷不到他。", ch, NULL, victim, TO_ROOM );
    RETURN( TRUE );
  }

  if ( IS_NEUTRAL( victim ) )
  {
    act( "$N看來沒什麼影響。", ch, NULL, victim, TO_CHAR );
    RETURN( TRUE );
  }

  dam = spell_damage( ch, level, TYPE_WIS, SPELL_CLASS_ONE );
  if ( saves_spell( level, victim ) ) dam /= 2;
  damage( ch, victim, dam, sn, ATTACK_RANDOM );
  RETURN( TRUE );
}

SKILL( cast_earthquake )
{
  CHAR_DATA * vch;
  CHAR_DATA * vch_next;
  int         dam;

  PUSH_FUNCTION( "cast_earthquake" );

  act( "$r附近地表強烈地震動著﹗", ch, NULL, NULL, TO_ALL );

  /* 設定攻傷 */
  dam = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_TWO );

  for ( vch = char_list; vch; vch = vch_next )
  {
    vch_next = vch->next;

    if ( !verify_char( vch ) || !vch->in_room ) continue;

    if ( vch->in_room == ch->in_room )
    {
      if ( vch != ch && ( IS_NPC( ch ) ? !IS_NPC( vch ) : IS_NPC( vch ) ) )
      {
        if ( !is_affected( vch, SLOT_FIXITY ) )
          damage( ch, vch, dam, sn, ATTACK_RANDOM );
      }

      continue;
    }

    if ( vch->in_room->area == ch->in_room->area )
      send_to_char( "大地不停地搖動著﹗\n\r", vch );
  }

  RETURN( TRUE );
}

SKILL( cast_enchant_weapon )
{
  OBJ_DATA    * obj = ( OBJ_DATA * ) vo;
  AFFECT_DATA * paf;

  PUSH_FUNCTION( "cast_enchant_weapon" );

  if ( !obj )
  {
    mudlog( LOG_DEBUG, "cast_enchant_weapon: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( obj->item_type != ITEM_WEAPON || obj->Magic || obj->affected )
  {
    act( "$p已經不能再加強了﹗", ch, obj, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  if ( obj->level > ch->level )
  {
    act( "$p不是$n所能隨意控制的!", ch, obj, NULL, TO_CHAR );
    RETURN( FALSE );
  }

   /* 配置記憶體 */
  paf            = alloc_struct( STRUCT_AFFECT_DATA );
  paf->type      = sn;
  paf->duration  = -1;
  paf->location  = APPLY_HITROLL;
  paf->modifier  = level / 5;
  paf->level     = level;
  paf->next      = obj->affected;
  obj->affected  = paf;

  /* 配置記憶體 */
  paf            = alloc_struct( STRUCT_AFFECT_DATA );
  paf->type      = -1;
  paf->duration  = -1;
  paf->location  = APPLY_DAMROLL;
  paf->modifier  = level / 10;
  paf->level     = level;
  paf->next      = obj->affected;
  obj->affected  = paf;
  obj->level     = ch->level;

  if ( IS_GOOD( ch ) )
  {
    obj->AntiEvil = TRUE;
    act( "$p發出一片淡淡的$5藍光$0。", ch, obj, NULL, TO_CHAR );
  }

  else if ( IS_EVIL( ch ) )
  {
    obj->AntiGood = TRUE;
    act( "$p發出一片淡淡的$1紅光$0。", ch, obj, NULL, TO_CHAR );
  }
  else
  {
    obj->AntiEvil = TRUE;
    obj->AntiGood = TRUE;
    act( "$p發出一片淡淡的$3黃光$0。", ch, obj, NULL, TO_CHAR );
  }

  send_to_char( "好了。\n\r", ch );
  RETURN( TRUE );
}


SKILL( cast_fireball )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         dam;
  int         choice[] =
              { 35, 55, 75, 90, 95, 100, -1 };

  PUSH_FUNCTION( "cast_fireball" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_fireball: 缺乏來源." );
    RETURN( FALSE );
  }

  /* 設定攻傷 */
  dam = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_THR );

  switch( dimemsion_position( choice, level, TRUE ) )
  {
  case ERRORCODE:
    mudlog( LOG_DEBUG, "技能 %d 的訊息設定有問題." , sn );
    RETURN( FALSE );

  case 0:
    sact( ch, victim, NULL,
      "$n將手上的熱源集中﹐形成一個拇指般大小的火球﹐以次火球丟向$N﹗" );
    break;

  case 1:
    sact( ch, victim, NULL,
      "$n聚集了大自然一些散在空中的熱量﹐集中在手掌處﹗$A"
      "驀地﹐$e將這一些小熱球丟向$N﹐一道道火光向$E飆去﹗" );
    break;

  case 2:
    sact( ch, victim, NULL,
      "$n屏氣凝神﹐神遊太虛﹐接著一顆巴掌般大小的火球出現在$e眼前﹗$A"
      "$n一使力﹐這一顆灼熱的火球便向$N急衝而去﹐氣勢驚人﹗" );
    break;

  case 3:
    sact( ch, victim, NULL,
      "只見$n一個發氣﹐許\多的如石塊般大的火球出現在$e四周不停圍繞著﹗$A"
      "接著這一顆顆火球四面八方的擊向$N﹐頗為壯觀﹗" );
    break;

  case 4:
    sact( ch, victim, NULL,
      "$n唸完咒語﹐一顆極大的火球像是$Y元氣玉$Z一般出現在$e上空﹐碧麗輝煌。$A"
      "$n向著$N一指﹐火球一般便像有生命一樣直直的飛往$E﹐讓$E避無可避。" );
    break;

  case 5:
    sact( ch, victim, NULL,
      "$n在四周劃下一個結界﹐只見四周的景色開始變動﹐似乎來到了一個"
      "不一樣的地方﹐這裡林木青翠﹐炊煙裊裊﹗$A"
      "只見$n浮出不一樣的微笑﹐一種鄙視的笑﹐一陣沈默﹗$A"
      "$R                     極 溫 火 球   $Z               $A"
      "一個極大的高溫火球挾著勁風向著$N狂飆而去﹗﹗" );
    dam = number_range( dam, UMIN( LEVEL_TWO_DAM, dam * 2 ) );
    break;
  }

  damage( ch, victim, dam, sn, ATTACK_RANDOM );
  RETURN( TRUE );
}

#define MAX_MULTIPLE               5
SKILL( cast_flamestrike )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  CHAR_DATA * vic;
  CHAR_DATA * vic_next;
  int         dam;
  int         loop;

  PUSH_FUNCTION( "cast_flamestrike" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_flamestrike: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( !ch->in_room || !victim->in_room ) RETURN( FALSE );

  sact( ch, victim, NULL, "$R$n$R默唸咒語﹐轉眼間你覺得四周似"
    "乎被火燒灼過一樣﹐寸草不生﹗$Z$A" );

  /* 設定攻傷 */
  dam = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_TWO );

  if ( !IS_NPC( victim ) )
  {
    sact( ch, victim, NULL, "$R$n$R手順勢一揮﹐一股極熱的氣流"
      "挾帶乾旱的狂風向著$N$R逼近。$Z" );

    damage( ch, victim, UMIN( LEVEL_TWO_DAM, dam * 2 ), sn, ATTACK_RANDOM );
    RETURN( TRUE );
  }

  for ( loop = 0, vic = ch->in_room->people; vic; vic = vic_next )
  {
    vic_next = vic->next_in_room;

    /* 怪物不打怪物 */
    if ( IS_NPC( ch ) && IS_NPC( vic ) ) continue;

    if ( check_can_kill( ch, vic ) == TRUE )
    {
      if ( loop > 3 ) sact( ch, vic, NULL,
        "四周一片火海﹐$n的$R火焰$Z不停的燒灼著$N的生命﹗" );

      else sact( ch, vic, NULL,
        "$n帶起一場$R烈焰$Z將$N漸漸地吞噬﹐要將$E置於死地﹗" );

      dam = UMAX( 0, number_range( dam
        , dam + ( UMIN( MAX_MULTIPLE, loop++ ) * get_curr_int( ch ) ) ) );

      damage( ch, vic, UMIN( LEVEL_TWO_DAM, dam ), sn, ATTACK_RANDOM );
    }

    continue;
  }

  if ( loop > 6 && victim && can_damage( victim ) )
  {
    sact( ch, victim, NULL, "$n的火焰燒灼了一切﹐一些亂彈的"
      "$R灼熱火焰$Z不小心又燒傷了$N。" );
    damage( ch, victim, dam, sn, ATTACK_RANDOM );
  }

  RETURN( TRUE );
}
#undef  MAX_MULTIPLE

SKILL( cast_faerie_fire )
{
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA   af;

  PUSH_FUNCTION( "cast_faerie_fire" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_faerie_fire: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( is_affected( victim, sn ) ) RETURN( FALSE );

  af.type      = sn;
  af.duration  = level;
  af.location  = APPLY_AC;
  af.modifier  = number_range( level, level * 3 );
  af.level     = level;
  affect_to_char( victim, &af );
  send_to_char( "你被一團粉紅的煙霧包圍﹐就像一隻粉紅豹\一樣。\n\r", victim );
  act( "$n被一團粉紅煙霧包圍著。", victim, NULL, NULL, TO_ROOM );
  RETURN( TRUE );
}

SKILL( cast_faerie_fog )
{
  CHAR_DATA * ich;

  PUSH_FUNCTION( "cast_faerie_fog" );

  if ( !ch->in_room )
  {
    mudlog( LOG_DEBUG, "cast_farie_fog: 來源不正確." );
    RETURN( FALSE );
  }

  act( "$n像一隻蟾蜍一樣吐出一團紫色的煙霧。", ch, NULL, NULL, TO_ROOM );
  send_to_char( "你吐出一團紫色的煙霧。\n\r", ch );

  for ( ich = ch->in_room->people; ich; ich = ich->next_in_room )
  {
    if ( !IS_NPC( ich) && IS_SET( ich->act, PLR_WIZINVIS ) ) continue;
    if ( ich == ch || saves_spell( level, ich ) ) continue;

    affect_release( ich, SLOT_INVIS      );
    affect_release( ich, SLOT_MASS_INVIS );
    affect_release( ich, SLOT_SNEAK      );
    affect_release( ich, SLOT_MASK       );

    act( "$n的身形慢慢地出現了﹗", ich, NULL, NULL, TO_ROOM );
    send_to_char( "你現形了﹗\n\r", ich );
  }

  RETURN( TRUE );
}

SKILL( cast_fly )
{
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA   af;

  PUSH_FUNCTION( "cast_fly" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_fly: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( victim->mount )
  {
    act( "因為你騎著$N﹐所以無法施展這種法術﹗"
      , victim, NULL, victim->mount, TO_CHAR );

    RETURN( FALSE );
  }

  if ( victim->mount_by )
  {
    act( "因為你正被$N騎著﹐所以無法施展這種法術﹗"
      , victim, NULL, victim->mount_by , TO_CHAR );

    RETURN( FALSE );
  }

  if ( is_affected( victim, sn ) )
  {
    act( "$n等了半天﹐連點動靜都沒發生。", ch, NULL, NULL, TO_ALL );
    RETURN( FALSE );
  }

  af.type      = sn;
  af.duration  = level + 3;
  af.location  = 0;
  af.modifier  = 0;
  af.level     = level;
  affect_to_char( victim, &af );

  act( "只見$n使出道家仙術之「騰雲飛仙」﹐手中拂塵一甩﹐一朵白雲"
    "平地昇起﹐緩緩的將$n托了起來。", ch, NULL, NULL, TO_ALL );

  RETURN( TRUE );
}

SKILL( cast_gate )
{
  CHAR_DATA * mob;
  CHAR_DATA * ghost;

  PUSH_FUNCTION( "cast_gate" );

  if ( !ch || !ch->in_room )
  {
    mudlog( LOG_DEBUG, "cast_gate: 缺乏來源。" );
    RETURN( FALSE );
  }

  if ( is_pk( ch ) && fight_info->status != PK_FIGHT )
  {
    send_to_char( "在這段時間﹐還是好好地休息一下吧﹗\n\r", ch );
    RETURN( FALSE );
  }

  for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
    if ( mob->pIndexData && mob->pIndexData == MobVampire ) break;

  if ( mob )
  {
    act( "$r附近到處都是$N﹐好可怕喔﹗", ch, NULL, mob, TO_CHAR );
    RETURN( FALSE );
  }

  if ( !( ghost = create_mobile( MobVampire, -1 ) ) )
  {
    mudlog( LOG_DEBUG, "cast_gate: 無法製造女鬼: %d.", MobVampireVnum );
    RETURN( FALSE );
  }

  act( "$2$n低聲唸完咒語﹐轉眼間眼前土地分開﹐由裡頭迸出一隻$N。$0"
    , ch, NULL, ghost, TO_ALL );

  char_to_room( ghost, ch->in_room );

  if ( ch->fighting )
  {
    sact( ghost, ch->fighting, NULL,
      "$n一見到$N彷彿見到仇人一樣﹐立即對$N展開攻擊﹗" );

    set_fighting( ghost, ch->fighting );
  }

  RETURN( TRUE );
}

SKILL( cast_general_purpose )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         dam;
  int         guess;

  PUSH_FUNCTION( "cast_general_purpose" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_general_purpose: 缺乏來源." );
    RETURN( FALSE );
  }

  guess = number_range( 1, 5 );
  dam = spell_damage( ch, level, guess, SPELL_CLASS_ONE );
  if ( saves_spell( level, victim ) ) dam /= 2;
  damage( ch, victim, dam, sn, ATTACK_RANDOM );
  RETURN( TRUE );
}

SKILL( cast_giant_strength )
{
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA   af;

  PUSH_FUNCTION( "cast_gaint_strength" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_gaint_strength: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( is_affected( victim, sn ) )
  {
    act( "$N已經非常的強壯了﹗", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  af.type      = sn;
  af.duration  = number_range( level / 6, level / 4 );
  af.location  = APPLY_STR;
  af.modifier  = 1 + (level >= 18) + (level >= 25);
  af.level     = level;
  affect_to_char( victim, &af );

  send_to_char( "不知哪來的力量傳遍你身體﹐你感到可以舉起一頭牛。\n\r"
    , victim );
  if ( ch != victim ) send_to_char( "好了。\n\r", ch );
  RETURN( TRUE );
}

SKILL( cast_harm )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         dam;

  PUSH_FUNCTION( "cast_harm" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_harm: 缺乏來源." );
    RETURN( FALSE );
  }

  dam = spell_damage( ch, level, TYPE_WIS, SPELL_CLASS_FOU );
  dam = number_range( dam / 2, dam );
  damage( ch, victim, dam, sn, ATTACK_RANDOM );
  RETURN( TRUE );
}

SKILL( cast_heal )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         heal;

  PUSH_FUNCTION( "cast_heal" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_heal: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( !can_damage( victim ) )
  {
    send_to_char( "你是救不活一個死人的﹗\n\r", victim );
    RETURN( FALSE );
  }

  if ( victim->hit >= get_curr_hit( victim ) )
  {
    act( "$N已經沒有傷口了﹗", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  heal = ( number_range( 100, 150 ) + level ) * get_curr_int( ch ) / 4;
  victim->hit = UMIN( victim->hit + heal , get_curr_hit( victim ) );

  if ( victim->position == POS_SLEEPING && victim != ch )
    act( "$N的醫治行動驚動了你﹗", victim, NULL, ch, TO_CHAR );

  update_pos( victim );

  send_to_char( "天上降下一道雷電﹐打在你的身上。\n\r"
    "你感到一陣暖流流遍全身﹐溫溫的好舒服﹗\n\r", victim );

  if ( ch != victim ) send_to_char( "醫治完畢。\n\r", ch );
  RETURN( TRUE );
}

SKILL( cast_high_explosive )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         dam;

  PUSH_FUNCTION( "cast_high_explosive" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_high_explosive: 缺乏來源." );
    RETURN( FALSE );
  }

  dam = spell_damage( ch, level, TYPE_WIS, SPELL_CLASS_THR );
  if ( saves_spell( level, victim ) ) dam /= 2;
  damage( ch, victim, dam, sn, ATTACK_RANDOM );
  RETURN( TRUE );
}

SKILL( cast_judge )
{
  char             buf1[MAX_STRING_LENGTH];
  SKILL_DATA     * pSkill;
  LIMIT_DATA     * pLimit;
  char           * pString = ( char * ) vo;
  RESTRICT_DATA * pRestrict;
  int             count;
  char            chinese[MAX_INPUT_LENGTH];

  PUSH_FUNCTION( "cast_judge" );

  if ( !( pSkill = skill_isname( pString ) ) )
  {
    send_to_char( "這種技能好像失傳很久囉﹗\n\r", ch );
    RETURN( FALSE );
  }

  if ( pSkill->canask == FALSE )
  {
    send_to_char( "你頭昏腦脹﹐什麼都感應不到﹗\n\r", ch );
    RETURN( FALSE );
  }

  clear_buffer();
  send_to_buffer( "\e[1;36m你坐了下來﹐接受神明給你的指導。\e[0m\n\r\n\r" );
  send_to_buffer( "你鑑賞%s技能的結果是﹕\n\r", pSkill->cname );
  send_to_buffer( "這是一項%s類的"
    , pSkill->rating ? skill_rating( pSkill->rating ) : "未知" );
  if ( pSkill->type        == TAR_DODGE ) send_to_buffer( " 防禦類" );
  else if ( pSkill->type   == TAR_FLEE )  send_to_buffer( " 逃跑類" );
  else send_to_buffer( "%s的技能﹐主要是用來"
    , weapon_type_name( pSkill->weapon ) );

  switch( pSkill->type )
  {
  default                 : send_to_buffer( "未明。\n\r"     ); break;
  case TAR_DODGE          : send_to_buffer( "本身閃躲。\n\r" ); break;
  case TAR_IGNORE         : send_to_buffer( "沒有目標。\n\r" ); break;
  case TAR_CHAR_OFFENSIVE : send_to_buffer( "攻擊對手。\n\r" ); break;
  case TAR_CHAR_DEFENSIVE : send_to_buffer( "本身防禦。\n\r" ); break;
  case TAR_CHAR_SELF      : send_to_buffer( "作用本身。\n\r" ); break;
  case TAR_OBJ_INV        : send_to_buffer( "作用物體。\n\r" ); break;
  case TAR_MEDITATION     : send_to_buffer( "冥想。\n\r"     ); break;
  case TAR_OBJ_ROOM       : send_to_buffer( "房間物品。\n\r" ); break;
  }

  if ( level < 20 )
  {
    print_buffer( ch );
    RETURN( TRUE );
  }

  send_to_buffer( "施展主要浪費" );

  switch( pSkill->cost_type )
  {
  default        : send_to_buffer( "未知"   ); break;
  case COST_HIT  : send_to_buffer( "生命力" ); break;
  case COST_MANA : send_to_buffer( "法力"   ); break;
  case COST_MOVE : send_to_buffer( "移動力" ); break;
  case COST_GOLD : send_to_buffer( "金錢"   ); break;
  }

  send_to_buffer( "的數值看來" );

  if ( pSkill->cost <= 20 )      send_to_buffer( "很少。\n\r" );
  else if ( pSkill->cost < 100 ) send_to_buffer( "一些。\n\r" );
  else if ( pSkill->cost < 300 ) send_to_buffer( "不少。\n\r" );
  else                           send_to_buffer( "很多。\n\r" );

  if ( level < 40 )
  {
    print_buffer( ch );
    RETURN( TRUE );
  }

  if ( pSkill->limit )
    send_to_buffer( "\n\r依感應來的感覺﹐這項技能有很多的限制﹕\n\r" );

  for ( pLimit = pSkill->limit; pLimit; pLimit = pLimit->next )
  {
    if ( pLimit->class )
    {
      chinese_number( pLimit->level, buf1 );
      send_to_buffer( "若是%s則等級最低要%s級才能學﹐熟練度最高能到%s"
      , class_name( pLimit->class, FALSE )
      , buf1
      , adeptation_name( ( pLimit->adept ) ) );

      if ( SkillValue || IS_IMMORTAL( ch ) )
        send_to_buffer( "  %3d%%", pLimit->adept );

      send_to_buffer( "。\n\r" );
    }
  }

  if ( level < 60 )
  {
    print_buffer( ch );
    RETURN( TRUE );
  }

  for ( count = 0, pRestrict = pSkill->restrict;
    pRestrict;
    pRestrict = pRestrict->next, count++ )
  {
    chinese_number( count + 1, chinese );
    send_to_buffer( "%s、本技能%s\n\r"
      , chinese, restrict_value( pRestrict, ch ) );
  }

  if ( count == 0 ) send_to_buffer( "本技能沒有任何的限制。\n\r" );
  print_buffer( ch );
  RETURN( TRUE );
}

SKILL( cast_pry )
{
  SKILL_DATA     * pSkill;
  TEACH_DATA     * teach;
  MOB_INDEX_DATA * pMobIndex;
  CHAR_DATA      * victim;
  char           * pString = ( char * ) vo;
  int              count;
  int              choice;
  char           * message[] =
    {
      "找他準",
      "而且他十分樂意",
      "我想他會樂意",
      "他可能會",
      "他應該是會",
      "但是他不太想",
      "但是他不願意",
      "但是他十分不願意",
      "你可能要試試運氣看他是否會"
    };

  PUSH_FUNCTION( "cast_pry" );

  if ( !pString || *pString == '\x0' )
  {
    send_to_char( "你要打聽哪種技能呢﹖\n\r", ch );
    RETURN( FALSE );
  }

  if ( !( pSkill = skill_isname( pString ) ) )
  {
    send_to_char( "這種技能好像失傳很久囉﹗\n\r", ch );
    RETURN( FALSE );
  }

  choice = sizeof( message ) / sizeof( message[0] );
  clear_buffer();

  for ( count = 0, victim = char_list; victim; victim = victim->next )
  {
    if ( !verify_char( victim )
      || !IS_NPC( victim )
      || !( pMobIndex = victim->pIndexData )
      || !victim->in_room
      || !victim->in_room->area ) continue;

    for ( teach = pMobIndex->teach; teach; teach = teach->next )
    {
      if ( pSkill->slot == teach->slot
        && number_range( 1, 100 + level * 2 ) > teach->inventory )
      {
        int sel;

        count++;

        sel = UMAX( 0, UMIN( choice - 1, teach->inventory * choice / 100 ) );
        send_to_buffer( "你打聽到%s的%s有教導%s﹐%s教導你。\n\r"
          , victim->in_room->area->name, mob_name( ch, victim )
          , pSkill->cname, message[sel] );
      }
    }
  }

  if ( count == 0 ) send_to_buffer( "這種技能好像失傳很久囉﹗\n\r" );
  print_buffer( ch );
  RETURN( TRUE );
}

SKILL( cast_identify )
{
  OBJ_DATA * obj = ( OBJ_DATA * ) vo;

  PUSH_FUNCTION( "cast_identify" );

  if ( !obj )
  {
    mudlog( LOG_DEBUG, "cast_identify: 缺乏來源." );
    RETURN( FALSE );
  }

  identify_obj( ch, obj );
  RETURN( TRUE );
}

SKILL( cast_infravision )
{
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA   af;

  PUSH_FUNCTION( "cast_infravision" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_infravision: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( is_affected( victim, sn ) ) RETURN( FALSE );

  act( "$n感到血液沸騰﹐瞬間眼睛紅了起來。\n\r", ch, NULL, NULL, TO_ROOM );
  af.type      = sn;
  af.duration  = number_range( level / 4 , level / 2 );
  af.location  = APPLY_NONE;
  af.modifier  = 0;
  af.level     = level;
  affect_to_char( victim, &af );
  send_to_char( "你感到血液沸騰﹐瞬間眼睛紅了起來。\n\r", victim );
  if ( ch != victim ) send_to_char( "好了。\n\r", ch );
  RETURN( TRUE );
}

SKILL( cast_invis )
{
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA   af;

  PUSH_FUNCTION( "cast_invis" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_invis: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( is_affected( victim, SLOT_INVIS )
    || is_affected( victim, SLOT_MASS_INVIS ) )
  {
    act( "$N的身影已經不見了﹗", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  if ( ( IS_NPC( victim ) && IS_SET( victim->act, ACT_TRAIN ) )
    || ( IS_NPC( victim ) && IS_SET( victim->act, ACT_REBIRTH ) ) )
  {
    act( "你沒有辦法把$N施隱形法術。", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  if ( victim->pIndexData && victim->pIndexData->pShop )
  {
    act( "$N是不能隱形起來的喔﹗", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  act( "$n的身影淡出了一個黑影﹐瞬間走進黑影消失了。"
    , victim, NULL, NULL, TO_ROOM );
  af.type      = sn;
  af.duration  = number_range( level / 5, level / 3 );
  af.location  = APPLY_NONE;
  af.modifier  = 0;
  af.level     = level;
  affect_to_char( victim, &af );
  send_to_char( "你的身影淡出了一個黑影﹐瞬間走進黑影消失了。\n\r", victim );
  if ( ch != victim ) send_to_char( "好了。\n\r", ch );
  RETURN( TRUE );
}

SKILL( cast_know_alignment )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  char      * msg;
  int         ap;

  PUSH_FUNCTION( "cast_know_alignment" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_know_alignment: 缺乏來源." );
    RETURN( FALSE );
  }

  ap = victim->alignment;

       if ( ap >  999 ) msg = "$N幾乎是宇宙無敵世界霹靂純潔的啦。";
  else if ( ap >  700 ) msg = "$N似乎連天神都自嘆不如他的純潔。";
  else if ( ap >  350 ) msg = "$N像一個天使般的聖潔。";
  else if ( ap >  100 ) msg = "$N看到小動物都會給予特別的照顧喔。";
  else if ( ap > -100 ) msg = "$N用肉眼實在看不出有什麼不一樣的。";
  else if ( ap > -350 ) msg = "$N看到動物就會想好不好吃。";
  else if ( ap > -700 ) msg = "$N將劍尖指著你說道﹕「來單挑吧﹗」";
  else if ( ap > -999 ) msg = "$N散發出一陣黑色的光芒﹐好看極了﹗";
  else msg = "$N是奸淫擄掠、燒殺搶劫、無所不做的喔﹗";

  act( smash_act_keyword( msg, "nesNES" ), ch, NULL, victim, TO_CHAR );
  RETURN( TRUE );
}

SKILL( cast_lightning_bolt )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         dam;

  PUSH_FUNCTION( "cast_lightning_bolt" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_lightning_blot: 缺乏來源." );
    RETURN( FALSE );
  }

  /* 設定攻傷 */
  dam = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_TWO );

  act( "$n自手中激射出一道閃電﹐直直的射向$N。", ch, NULL, victim, TO_ALL );

  if ( saves_spell( level, victim ) ) dam /= 2;
  damage( ch, victim, dam, sn, ATTACK_RANDOM );
  RETURN( TRUE );
}

SKILL( cast_magic_missile )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         dam;

  PUSH_FUNCTION( "cast_magic_missile" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_magic_missile: 缺乏來源." );
    RETURN( FALSE );
  }

  /* 攻傷敘述 */
  act( "$n$2揮著雙手﹐凝聚法力成為一個飛彈﹐倏地向著$N飛去﹗$0"
    , ch, NULL, victim, TO_ALL );

  /* 設定攻傷 */
  dam = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_ONE );

  if ( saves_spell( level, victim ) ) dam /= 2;
  damage( ch, victim, dam, sn, ATTACK_RANDOM );
  RETURN( TRUE );
}

SKILL( cast_wind_blade )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         dam;

  PUSH_FUNCTION( "cast_wind_blade" );

  if ( !ch || !verify_char( ch ) || !victim || !verify_char( victim ) )
  {
    mudlog( LOG_DEBUG, "cast_wind_blade: 缺乏來源." );
    RETURN( FALSE );
  }

  /* 攻傷敘述 */
  act( "$n$7將右手伸直﹐往上一劃﹐大地刮起一陣烈風﹐猶如$2利刃$7般"
       "地切向$N﹐$N身上立即多了數道傷痕﹗$0", ch, NULL, victim, TO_ALL );

  /* 設定攻傷 */
  dam = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_TWO );

  if ( saves_spell( level, victim ) ) dam /= 2;
  damage( ch, victim, dam, sn, ATTACK_RANDOM );

  RETURN( TRUE );
}

#define MAX_MULTIPLE               5

SKILL( cast_vacuum_wind )
{
  CHAR_DATA * victim;
  CHAR_DATA * vic_next;
  int         dam;
  int         loop;

  PUSH_FUNCTION( "cast_vacuum_wind " );

  if ( !ch || !verify_char( ch ) || !ch->in_room )
  {
    mudlog( LOG_DEBUG, "cast_wind_blade: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( level > 45 )
  {
    /* 尋找對手 */
    for ( loop = 0, victim = ch->in_room->people; victim; victim = vic_next )
    {
      vic_next = victim->next_in_room;

      /* 怪物不打怪物 */
      if ( IS_NPC( ch ) && IS_NPC( victim ) ) continue;

      if ( !check_can_kill( ch, victim ) ) continue;
      if ( loop > MAX_MULTIPLE )           break;

      /* 攻傷敘述 */
      sact( ch, victim, NULL,
        "$n$W凝神入定﹐口唸[$R狂風咒$W]﹐在$N的周圍吹"
        "起數陣強風﹐沒有規律的侵襲$N﹐其強烈又狂亂的風勢將$N狠狠地"
        "重摔落地﹗$Z" );

      dam = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_THR );
      if ( saves_spell( level, victim ) ) dam /= 2;
      dam = number_range( dam * 3 / 4, dam );

      loop++;
      damage( ch, victim, dam, sn, ATTACK_RANDOM );
    }

    RETURN( TRUE );
  }
  else
  {
    victim = ( CHAR_DATA * ) vo;

    /* 攻傷敘述 */
    sact( ch, victim, NULL,
      "$n$W口唸$R[狂風咒]$W﹐接著在$N周圍造成真空般的風暴"
      "﹐席捲著$N的身體﹗$Z" );

    dam = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_THR );
    if ( saves_spell( level, victim ) ) dam /= 2;
    dam = number_range( dam * 3 / 4, dam );

    damage( ch, victim, dam, sn, ATTACK_RANDOM );
    RETURN( TRUE );
  }

  RETURN( TRUE );
}

SKILL( cast_mass_invis )
{
  AFFECT_DATA   af;
  CHAR_DATA   * gch;

  PUSH_FUNCTION( "cast_mass_invis" );

  if ( !ch->in_room )
  {
    mudlog( LOG_DEBUG, "cast_mass_invis: 缺乏來源." );
    RETURN( FALSE );
  }

  for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
  {
    if ( !is_same_group( gch, ch )
      || is_affected( gch, SLOT_INVIS )
      || is_affected( gch, SLOT_MASS_INVIS ) )
      continue;

    act( "$n的身影淡出了一個黑影﹐瞬間走進黑影消失了。"
      , gch, NULL, NULL, TO_ROOM );

    send_to_char( "你的身影淡出了一個黑影﹐瞬間走進黑影消失了。\n\r", gch );

    af.type      = sn;
    af.duration  = number_range( level / 18, level / 12 );
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.level     = level;
    affect_to_char( gch, &af );
  }

  send_to_char( "好了。\n\r", ch );
  RETURN( TRUE );
}

SKILL( cast_null )
{
  PUSH_FUNCTION( "cast_null" );
  send_to_char( "對不起﹐這不是一項法術﹗\n\r", ch );
  RETURN( FALSE );
}

SKILL( cast_pass_door )
{
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA   af;

  PUSH_FUNCTION( "cast_pass_door" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_pass_door: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( is_affected( victim, sn ) ) RETURN( FALSE );

  af.type      = sn;
  af.duration  = number_fuzzy( level / 4 );
  af.location  = APPLY_NONE;
  af.modifier  = 0;
  af.level     = level;
  affect_to_char( victim, &af );
  act( "$n的身體似乎變的有一點透明。", victim, NULL, NULL, TO_ROOM );
  send_to_char( "你感到身體起了一些變化﹐變的有一點透明。\n\r", victim );
  RETURN( TRUE );
}

SKILL( cast_fixity )
{
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA   af;

  PUSH_FUNCTION( "cast_fixity" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_fixity: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( is_affected( victim, sn ) ) RETURN( FALSE );

  af.type      = sn;
  af.duration  = number_fuzzy( level / 4 );
  af.location  = APPLY_NONE;
  af.modifier  = 0;
  af.level     = level;
  affect_to_char( victim, &af );

  act( "$n呼叫不動明王﹐之後$n就定住不動了。", victim, NULL, NULL, TO_ALL );
  RETURN( TRUE );
}

SKILL( cast_poison )
{
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA   af;

  PUSH_FUNCTION( "cast_poison" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_poison: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( saves_spell( level, victim ) || get_curr_str( victim ) <= 2 )
  {
    act( "$N不受你毒害術的影響。", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  af.type      = sn;
  af.duration  = number_range( 1, level / 5 );
  af.location  = APPLY_STR;
  af.modifier  = -2;
  af.level     = 0;
  affect_join( victim, &af );
  send_to_char( "你臉上一陣青一陣白﹐不知道發生了什麼事﹐卻感到很難過。\n\r"
    , victim );
  if ( ch != victim ) send_to_char( "好了。\n\r", ch );
  RETURN( TRUE );
}

SKILL( cast_protection )
{
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA   af;

  PUSH_FUNCTION( "cast_protection" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_protection: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( is_affected( victim, sn ) )
  {
    send_to_char( "你已經施展了「靈神訣」﹐小心經脈逆行。\n\r", ch );
    RETURN( FALSE );
  }

  af.type      = sn;
  af.duration  = 12 * UMIN( 1, level / 40 );
  af.location  = APPLY_NONE;
  af.modifier  = 0;
  af.level     = 0;
  affect_to_char( victim, &af );

  act( "$n閉目凝神﹐用穀\衣心法的內力運轉了一次「靈神訣」。$A"
       "一股青氣從$n身上散出﹐匯聚在$e的頂心﹐然後緩緩淡去。"
       , ch, NULL, NULL, TO_ALL );

  RETURN( TRUE );
}

SKILL( cast_refresh )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         refresh;

  PUSH_FUNCTION( "cast_refresh" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_refresh: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( victim->move >= get_curr_move( victim ) )
  {
    act( "$N已經完全不會疲倦了﹗", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  refresh = level * 3 + ( number_range( 1, get_curr_int( ch ) * 2 ) );
  victim->move = UMIN( victim->move + refresh, get_curr_move( victim ) );

  send_to_char( "你感到較不疲倦了。\n\r", victim );
  if ( ch != victim ) send_to_char( "好了。\n\r", ch );
  RETURN( TRUE );
}

SKILL( cast_remove_curse )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;

  PUSH_FUNCTION( "cast_remove_curse" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_remove_curse: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( is_affected( victim, SLOT_CURSE ) )
  {
    affect_release( victim, SLOT_CURSE );

    send_to_char( "你感到好多了。\n\r", victim );
    if ( ch != victim ) send_to_char( "好了。\n\r", ch );
    RETURN( TRUE );
  }

  RETURN( TRUE );
}

SKILL( cast_sanctuary )
{
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA   af;

  PUSH_FUNCTION( "cast_sanctuary" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_sanctuary: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( is_affected( victim, sn ) )
  {
    act( "$N已經擁有聖光了﹗", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  af.type      = sn;
  af.duration  = URANGE( 1, number_fuzzy( level / 20 ), 20 );
  af.location  = APPLY_NONE;
  af.modifier  = 0;
  af.level     = 0;
  affect_to_char( victim, &af );
  act( "$n身旁升起了一陣白霧﹐久久不去。", victim, NULL, NULL, TO_ROOM );
  send_to_char( "你被一團奇怪的白霧所圍繞。\n\r", victim );
  RETURN( TRUE );
}

SKILL( cast_shield )
{
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA   aff;

  PUSH_FUNCTION( "cast_shield" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_shield: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( ch != victim )
  {
    send_to_char( "對不起﹐這項法術只能施展在自己的身上﹗\n\r", ch );
    RETURN( FALSE );
  }

  if ( is_affected( victim, sn ) )
  {
    act( "$N的防護已經好的沒話說了﹗", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  aff.type      = sn;
  aff.duration  = number_range( level / 5, level / 4 );
  aff.modifier  = -( UMAX( 1, number_range( level , level * 3 ) ) );
  aff.location  = APPLY_AC;
  aff.level      = level;
  affect_to_char( victim, &aff );

  act( "$n被一個力場圍繞著。", victim, NULL, NULL, TO_ROOM );
  send_to_char( "你被一個力場圍繞著﹐保護提昇了些許\。\n\r", victim );
  RETURN( TRUE );
}

SKILL( cast_gold_shield )
{
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA   aff;

  PUSH_FUNCTION( "cast_gold_shield" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_gold_shield: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( ch != victim )
  {
    send_to_char( "對不起﹐這一門功\夫只能施展在自己身上﹗\n\r", ch );
    RETURN( FALSE );
  }

  if ( victim->hit * 2 <= get_curr_hit( victim ) )
  {
    act( "對不起﹐$N受傷過重﹐真氣暫時無法凝聚﹗", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  if ( is_affected( victim, sn ) )
  {
    act( "$N已經施展了金鐘罩﹐真氣無法再凝聚﹗", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  aff.type      = sn;
  aff.duration  = number_range( 1, 2 );
  aff.modifier  = -( UMAX( 1, number_range( level , level * 3 ) ) );
  aff.location  = APPLY_AC;
  aff.level     = level;
  affect_to_char( victim, &aff );

  act( "$n氣聚丹田﹐遊走四方的真氣在身上形成了一層保護﹗", victim, NULL, NULL, TO_ALL );
  RETURN( TRUE );
}

SKILL( cast_iron_shield )
{
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA   aff;

  PUSH_FUNCTION( "cast_iron_shield" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_iron_shield: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( ch != victim )
  {
    send_to_char( "對不起﹐這一門功\夫只能施展在自己身上﹗\n\r", ch );
    RETURN( FALSE );
  }

  if ( victim->hit * 2 <= get_curr_hit( victim ) )
  {
    act( "對不起﹐$N受傷過重﹐真氣一時無法凝聚﹗", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  if ( is_affected( victim, sn ) )
  {
    act( "$N已經施展了鐵布衫﹐真氣無法再凝聚﹗", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  aff.type      = sn;
  aff.duration  = 1;
  aff.modifier  = -( UMAX( 1, number_range( level / 2, level * 3 ) ) );
  aff.location  = APPLY_AC;
  aff.level     = level;
  affect_to_char( victim, &aff );

  act( "$n大聲一呼﹐遊走四方的真氣有如鋼鐵般地保護著﹗", victim, NULL, NULL, TO_ALL );
  RETURN( TRUE );
}

SKILL( cast_deva_shield )
{
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA   aff;

  PUSH_FUNCTION( "cast_deva_shield" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_deva_shield: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( ch != victim )
  {
    send_to_char( "對不起﹐這一門功\夫只能施展在自己身上﹗\n\r", ch );
    RETURN( FALSE );
  }

  if ( !IS_GOOD( victim ) )
  {
    send_to_char( "對不起﹐你的陣營不夠好﹐無法施展這項法術﹗\n\r", victim );
    RETURN( FALSE );
  }

  if ( is_affected( victim, sn ) )
  {
    act( "對不起﹐$N已經施展了金剛護體﹗", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  aff.type      = sn;
  aff.duration  = UMAX( 1, level / 10 );
  aff.modifier  = -( UMAX( 1, number_range( level * 2 , level * 4 ) ) );
  aff.location  = APPLY_AC;
  aff.level     = level;
  affect_to_char( victim, &aff );

  act( "$n口誦佛號﹐四大金剛尋聲感應﹐法力加持﹗", victim, NULL, NULL, TO_ALL );
  RETURN( TRUE );
}

SKILL( cast_shocking_grasp )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         dam;

  PUSH_FUNCTION( "cast_shocking_grasp" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_shocking_grasp: 缺乏來源." );
    RETURN( FALSE );
  }

  dam = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_ONE );

  act( "$n發出了一個雷電集中體﹐轟向$N﹗", ch, NULL, victim, TO_ALL );

  if ( saves_spell( level, victim ) ) dam /= 2;
  damage( ch, victim, dam, sn, ATTACK_RANDOM );
  RETURN( TRUE );
}

SKILL( cast_sleep )
{
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA   af;

  PUSH_FUNCTION( "cast_sleep" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_sleep: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( ch == victim )
  {
    send_to_char( "你直接睡就可以了吧﹐如果真的不夠﹐那就考慮長眠吧﹗\n\r"
      , ch );
    RETURN( FALSE );
  }

  if ( !can_damage( victim ) )
  {
    act( "你目前沒有辦法催眠$N﹗", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  /* 若對象是玩家且不是盜賊或是殺人犯則不行 */
  if ( is_pk( ch ) && is_pk( victim ) )
  {
    if ( fight_info->status != PK_FIGHT )
    {
      send_to_char( "在這段時間﹐還是好好地休息一下吧﹗\n\r", ch );
      RETURN( FALSE );
    }
  }

  else if ( !IS_NPC( victim ) )
  {
    if ( !IS_SET( victim->act, PLR_KILLER )
      && !IS_SET( victim->act, PLR_THIEF  )
      && !IS_SET( victim->act, PLR_BOLTER )
      && ( victim->in_room
      && victim->in_room->area
      && !( victim->in_room->area->fight ) ) )
    {
      act( "你要攻擊$N﹐請用 $2MURDER$0 這個指令。", ch, NULL, victim, TO_CHAR );
      RETURN( FALSE );
    }
  }

  if ( !can_fight( ch , victim ) ) RETURN( FALSE );

  if ( is_affected( victim, SLOT_SLEEP ) )
  {
    act( "$N不受你催眠術的影響﹗", ch, NULL, victim, TO_CHAR );
    if ( IS_AWAKE( victim ) && !victim->fighting ) set_fighting( victim, ch );
    RETURN( FALSE );
  }

  if ( victim->fighting )
  {
    act( "$N戰意正高﹐不受你的催眠的影響﹗", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  if ( victim->mount_by )
  {
    act( "要催眠也得看主人$N肯不肯吧﹗", ch, NULL, victim->mount_by, TO_CHAR );
    RETURN( FALSE );
  }

  if ( victim->boss )
  {
    act( "你不能催眠$N的小鬼﹗", ch, NULL, victim->boss, TO_CHAR );
    RETURN( FALSE );
  }

  if ( number_range( 1, level ) <= victim->level )
  {
    act( "$N打醒精神﹐不受$n的催眠﹗", ch, NULL, victim, TO_ALL );
    set_fighting( victim, ch );
    RETURN( FALSE );
  }

  af.type      = sn;
  af.duration  = 4 + number_fuzzy( level / 5 );
  af.location  = APPLY_NONE;
  af.modifier  = 0;
  af.level     = level;
  affect_join( victim, &af );

  if ( IS_AWAKE( victim ) )
  {
    if ( IS_NPC( victim ) && victim->talk_buf )
    {
      free_string( victim->talk_buf );
      victim->talk_buf  = NULL;
      victim->talk_mode = MODE_ABSENT;
    }

    send_to_char( "一陣暈眩﹐你突然感到很想睡﹗\n\r", victim );
    act( "$N被你發出的一團黑氣纏住就睡著了。", ch, NULL, victim, TO_CHAR );
    act( "$n突然之間倒在地上呼呼大睡。", victim, NULL, NULL, TO_ROOM );
    victim->position = POS_SLEEPING;

    if ( victim->mount ) unmount_char( victim, victim->mount );
  }

  RETURN( TRUE );
}

SKILL( cast_stone_skin )
{
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA   af;

  PUSH_FUNCTION( "cast_stone_skin" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_stone_skin: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( is_affected( victim, sn ) ) RETURN( FALSE );

  af.type      = sn;
  af.duration  = level;
  af.location  = APPLY_AC;
  af.modifier  = -( UMAX( 1, number_range( level / 2, level ) ) );
  af.level     = level;
  affect_to_char( victim, &af );

  act( "$n向梅度沙祈禱﹐突然之間$e的皮膚變的跟石頭一樣。"
    , victim, NULL, NULL, TO_ROOM );

  send_to_char( "你的皮膚硬的和石頭一樣﹗\n\r", victim );
  RETURN( TRUE );
}

SKILL( cast_teleport )
{
  CHAR_DATA       * victim = ( CHAR_DATA * ) vo;
  ROOM_INDEX_DATA * pRoomIndex;

  PUSH_FUNCTION( "cast_teleport" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_teleport: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( is_pk( victim ) )
  {
    send_to_char( "對不起﹐你正在武鬥大會裡﹐沒有辦法施展這個法術﹗\n\r", ch );
    RETURN( FALSE );
  }

  act( "$n祭起一張陰界傳送符。忽然四周吹起了一陣涼颼颼的陰風﹐陰風之中彷彿"
       "有許\多人影在晃動﹐然後一陣火光突然爆起﹗", ch, NULL, NULL, TO_CHAR );

  if ( !victim->in_room
    || victim->mount
    || victim->mount_by
    || victim->spirit
    || victim->boss
    || is_affected( victim, SLOT_CURSE )
    || victim->in_room->NoRecall
    || ( !IS_NPC( ch ) && victim->fighting != NULL )
    || ( victim != ch
      && ( saves_spell( level, victim ) || saves_spell( level, victim ) ) ) )
  {
    send_to_char( "但是什麼事也沒有發生。\n\r", ch );
    RETURN( FALSE );
  }

  for ( ; ; )
  {
    pRoomIndex = get_room_index( number_range( 0, 65535 ) );

    if ( pRoomIndex
      && pRoomIndex->NoQuit  == FALSE
      && pRoomIndex->Safe    == FALSE
      && pRoomIndex->Sail    == FALSE
      && pRoomIndex->Private == FALSE
      && !is_loge( pRoomIndex ) ) break;
  }

  act( "$n只覺得身子不斷下墜﹐一直下墜﹐一直下墜﹐一直下墜﹐一直下墜。"
      , victim, NULL, NULL, TO_ALL );

  char_from_room( victim );
  char_to_room( victim, pRoomIndex );

  act( "不知道什麼時候﹐$n發現其實一直都站在堅硬的地面上﹐可是四周的景物"
       "卻不一樣了。" , ch, NULL, NULL, TO_ALL );

  do_look( victim, "auto" );

  /* 清除追蹤紀錄點 */
  clear_trace( victim, TRUE );

  RETURN( TRUE );
}

SKILL( cast_weaken )
{
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA   af;

  PUSH_FUNCTION( "cast_weaken" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "cast_weaken: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( is_affected( victim, sn )
    || saves_spell( level, victim )
    || get_curr_str( victim ) <= 2 )
  {
    act( "你的法術對$N並沒有效﹗", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  af.type      = sn;
  af.duration  = number_range( level / 22, level / 2 );
  af.location  = APPLY_STR;
  af.modifier  = -2;
  af.level     = level;
  affect_to_char( victim, &af );
  send_to_char( "你感覺全身虛弱了一些﹐好像連抓雞的力量都沒了﹗\n\r", victim );
  if ( ch != victim ) send_to_char( "好了﹗\n\r", ch );
  RETURN( TRUE );
}

SKILL( cast_word_of_recall )
{
  PUSH_FUNCTION( "cast_word_of_recall" );
  do_recall( ( CHAR_DATA * ) vo, "" );
  RETURN( TRUE );
}

SKILL( cast_acid_breath )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  OBJ_DATA  * obj_lose;
  OBJ_DATA  * obj_next;
  int         dam;

  PUSH_FUNCTION( "cast_acid_breath" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_acid_breath: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( level < 40 )
  {
    if ( saves_spell( level, victim ) ) dam /= 2;

    act( "$n怒吼一聲﹐在一陣白煙中只見$e已幻化成一支巨龍。"
      , ch, NULL, NULL, TO_ALL );
  }
  else
  {
    if ( saves_spell( level, victim ) ) dam /= 2;
    act( "$n的身影驀地消失不見﹐在$N失神的時候﹐一支巨大的神龍從天而降"
         "﹐將$N嚇了一跳﹗", ch, NULL, victim, TO_ALL );
  }

  sact( ch, victim, NULL,
    "巨龍翅膀奮力一揮﹐只見塵煙瀰漫﹐接著一陣巨大的吐息由$n向"
    "$N發出。\n\r緊接著黑暗一片﹐你什麼都看不到﹐腦中感受到一股酸味"
    "﹐你感到一陣暈眩。" );

  dam = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_MOB );

  if ( number_percent() < 2 * level && !saves_spell( level, victim ) )
  {
    for ( obj_lose = ch->carrying; obj_lose; obj_lose = obj_next )
    {
      int iWear;

      obj_next = obj_lose->next_content;
      if ( number_bits( 2 ) != 0 ) continue;

      switch ( obj_lose->item_type )
      {
      case ITEM_ARMOR:
        if ( obj_lose->value[0] > 0 )
        {
          act( "$p發出陣陣白煙﹐接著在酸液中毀壞了﹗"
            , victim, obj_lose, NULL, TO_CHAR );

          if ( ( iWear = obj_lose->wear_loc ) != WEAR_NONE )
            victim->armor -= apply_ac( obj_lose, iWear );

          obj_lose->value[0] -= 1;
          obj_lose->cost      = 0;

          if ( iWear != WEAR_NONE )
            victim->armor += apply_ac( obj_lose, iWear );
        }
        break;

      case ITEM_CONTAINER:
        act( "$p被酸液腐蝕﹐冒了一陣煙便消失了﹗"
          , victim, obj_lose, NULL, TO_CHAR );
        extract_obj( obj_lose );
        break;
      }
    }
  }

  damage( ch, victim, dam, sn, ATTACK_RANDOM );
  RETURN( TRUE );
}

SKILL( cast_fire_breath )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  OBJ_DATA  * obj_lose;
  OBJ_DATA  * obj_next;
  int         dam;

  PUSH_FUNCTION( "cast_fire_breath" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_fire_breath: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( number_percent() < 2 * level && !saves_spell( level, victim ) )
  {
    for ( obj_lose = victim->carrying; obj_lose; obj_lose = obj_next )
    {
      char * msg;

      obj_next = obj_lose->next_content;

      if ( number_bits( 2 ) != 0 ) continue;

      switch ( obj_lose->item_type )
      {
      default:             continue;
      case ITEM_CONTAINER: msg = "$p起火燃燒了﹗";         break;
      case ITEM_POTION:    msg = "$p冒出氣泡而沸騰了﹗";   break;
      case ITEM_SCROLL:    msg = "$p起火燃燒了﹗";         break;
      case ITEM_STAFF:     msg = "$p冒煙而變成木炭了﹗";   break;
      case ITEM_WAND:      msg = "$p冒煙而碎裂了﹗";       break;
      case ITEM_FOOD:      msg = "$p變成焦碳了﹗";         break;
      case ITEM_PILL:      msg = "$p熔化而滴落地面﹗";     break;
      }

      act( smash_act_keyword( msg, "nesp" ) , victim, obj_lose, NULL, TO_CHAR );
      extract_obj( obj_lose );
    }
  }

  act( "$1$n$1巨口一張﹐一股火焰般的吐息由口中噴出﹐直逼$N﹗$0"
    , ch, NULL, victim, TO_CHAR );

  dam  = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_MOB );
  if ( saves_spell( level, victim ) ) dam /= 2;
  damage( ch, victim, dam, sn, ATTACK_RANDOM );
  RETURN( TRUE );
}

SKILL( cast_frost_breath )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  OBJ_DATA  * obj_lose;
  OBJ_DATA  * obj_next;
  int         dam;

  PUSH_FUNCTION( "cast_frost_breath" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_frost_breath: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( number_percent() < 2 * level && !saves_spell( level, victim ) )
  {
    for ( obj_lose = victim->carrying; obj_lose; obj_lose = obj_next )
    {
      obj_next = obj_lose->next_content;

      if ( number_bits( 2 ) != 0 ) continue;

      switch ( obj_lose->item_type )
      {
      default:
        continue;

      case ITEM_CONTAINER:
      case ITEM_DRINK_CON:
      case ITEM_POTION:

        act( "$p遭到毀壞了﹗", victim, obj_lose, NULL, TO_CHAR );
        extract_obj( obj_lose );
        break;
      }
    }
  }

  sact( ch, victim, NULL,
    "$S$n$S將口中的$w冰凍$s吐息吐向$N﹐四周的溫度急遽降到冰點以下﹗$Z" );

  dam  = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_MOB );
  if ( saves_spell( level, victim ) ) dam /= 2;
  damage( ch, victim, dam, sn , ATTACK_RANDOM );
  RETURN( TRUE );
}

SKILL( cast_gas_breath )
{
  CHAR_DATA * vch;
  CHAR_DATA * vch_next;
  int         dam;

  PUSH_FUNCTION( "cast_gas_breath" );

  if ( !ch->in_room )
  {
    mudlog( LOG_DEBUG, "cast_gas_breath: 缺乏來源." );
    RETURN( FALSE );
  }

  for ( vch = ch->in_room->people; vch; vch = vch_next )
  {
    vch_next = vch->next_in_room;

    /* 怪物不打怪物 */
    if ( IS_NPC( ch ) && IS_NPC( vch ) ) continue;

    if ( !check_can_kill( ch, vch ) ) continue;

    sact( ch, vch, NULL,
      "$G$n$G張口噴出劇毒的$b瓦斯$R風暴﹐現場一片混亂﹗$Z" );

    if ( IS_NPC( ch ) ? !IS_NPC( vch ) : IS_NPC( vch ) )
    {
      dam  = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_MOB );
      if ( saves_spell( level, vch ) ) dam /= 2;
      damage( ch, vch, dam, sn , ATTACK_RANDOM );
    }
  }

  RETURN( TRUE );
}

SKILL( cast_lightning_breath )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         dam;

  PUSH_FUNCTION( "cast_lightning_breath" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_lightning_breath: 缺乏來源." );
    RETURN( FALSE );
  }

  sact( ch, victim, NULL,
    "$W$n$W背上翅膀一開一闔之際即由上空激射出數道"
    "$s雷電$W﹐筆直的貫穿$N的身體﹗$Z" );

  dam  = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_MOB );
  if ( saves_spell( level, victim ) ) dam /= 2;
  damage( ch, victim, dam, sn, ATTACK_RANDOM );
  RETURN( TRUE );
}

SKILL( cast_scan )
{
  int    dir;
  bool   found;
  char * pString = ( char * ) vo;

  PUSH_FUNCTION( "cast_scan" );

  if ( !check_blind( ch ) || IS_NPC( ch ) ) RETURN( FALSE );

  if ( !pString[0] )
  {
    act( "$n向四面八方遠眺。", ch, NULL, NULL, TO_ROOM );

    for ( found = FALSE, dir = 0; dir < DIR_MAX; dir++ )
    {
      if ( ch->in_room->exit[dir] )
      {
        scan( ch, dir );
        found = TRUE;
      }
    }

    if ( !found ) act( "$r這兒沒有任何出口﹗", ch, NULL, NULL, TO_CHAR );
  }

  else
  {
         if ( !str_prefix( pString, "north" ) ) dir = DIR_NORTH;
    else if ( !str_prefix( pString, "east"  ) ) dir = DIR_EAST;
    else if ( !str_prefix( pString, "south" ) ) dir = DIR_SOUTH;
    else if ( !str_prefix( pString, "west"  ) ) dir = DIR_WEST;
    else if ( !str_prefix( pString, "up"    ) ) dir = DIR_UP;
    else if ( !str_prefix( pString, "down"  ) ) dir = DIR_DOWN;
    else if ( !str_prefix( pString, "out"   ) ) dir = DIR_OUT;
    else
    {
      act( "$T 不是個方向﹗", ch, NULL, pString, TO_CHAR );
      RETURN( TRUE );
    }

    act( "$n向$T方遠眺。", ch, NULL, direction_name( dir ), TO_ROOM );

    if ( !ch->in_room->exit[dir] )
    {
      act( "$T方沒有出路﹗", ch, NULL, direction_name( dir ), TO_CHAR );
      RETURN( TRUE );
    }

    scan( ch, dir );
  }

  RETURN( TRUE );
}

void scan( CHAR_DATA * ch , int door )
{
  int               distance;
  int               visibility = 6;
  bool              found;
  ROOM_INDEX_DATA * was_in_room;
  char * const dist_name [] =
    { "", "很近處", "不遠處", "稍遠處", "遠處", "極遠處", "最遠處" };

  PUSH_FUNCTION( "scan" );

  if ( !ch || !ch->in_room )
  {
    mudlog( LOG_DEBUG, "scan: 缺乏來源" );
    RETURN_NULL();
  }

  if ( door < 0 || door >= DIR_MAX )
  {
    mudlog( LOG_DEBUG, "scan: 出口 %d 不對.", door );
    RETURN_NULL();
  }

  if ( !IS_SET( ch->act, PLR_HOLYLIGHT ) )
  {
    switch( weather_info.sunlight )
    {
    case SUN_SET:   visibility = 4; break;
    case SUN_DARK:  visibility = 2; break;
    case SUN_RISE:  visibility = 4; break;
    case SUN_LIGHT: visibility = 6; break;
    }

    switch( weather_info.sky )
    {
    case SKY_CLOUDLESS:                  break;
    case SKY_CLOUDY:    visibility -= 1; break;
    case SKY_RAINING:   visibility -= 2; break;
    case SKY_LIGHTNING: visibility -= 3; break;
    }
  }

  was_in_room = ch->in_room;
  for ( found = FALSE ,distance = 1; distance <= DIR_MAX; distance++ )
  {
    EXIT_DATA * pExit;
    CHAR_DATA * list;
    CHAR_DATA * rch;

    if ( ( pExit = ch->in_room->exit[door] )
      && pExit->to_room
      && pExit->to_room != was_in_room )
    {
      if ( pExit->closed )
      {
        char door_name[MAX_STRING_LENGTH];

        one_argument( pExit->keyword, door_name );
        if ( !door_name[0] ) str_cat( door_name, "門" );

        print_to_char( ch , "一個被關起來的%s在%s方%s。\n\r"
          , door_name, direction_name( door ), dist_name[distance-1] );

        found = TRUE;
        break;
      }

      if ( pExit->to_room->Dark )
      {
        visibility--;
        continue;
      }

      ch->in_room = pExit->to_room;
      if ( IS_OUTSIDE( ch ) ? distance > visibility : distance > 4 ) break;

      list = ch->in_room->people;
      for ( rch = list; rch; rch = rch->next_in_room )
      {
        if ( can_see( ch, rch ) )
        {
          print_to_char( ch , "%s%s就在你的%s方%s。\n\r"
            ,mob_name( ch, rch )
            ,IS_NPC( rch ) ? "" : "「玩家」"
            ,direction_name( door )
            ,dist_name[distance] );
          found = TRUE;
        }
      }
    }
  }

  ch->in_room = was_in_room;

  if ( !found ) act( "你的$t方沒有任何值得注意的人。"
    , ch, direction_name( door ), NULL, TO_CHAR );

  RETURN_NULL();
}

SKILL( cast_seven_key_numen )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         dam;

  PUSH_FUNCTION( "cast_seven_key_numen" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_seven_key_numen: 缺乏來源." );
    RETURN( FALSE );
  }

  /* 前置訊息 */
  sact( ch, victim, NULL,
    "$R        來自冥界的七賢者啊﹗$A        $Y用七把鑰匙打開地獄之門﹗"
    "$A$A        $G究極魔法﹗七鑰守護神﹗$Z$A" );

  dam = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_MAG );
  dam = number_range( dam, dam * 3 );

  if ( ch->alignment < -( AlignRange / 2 ) )
  {
    sact( ch, victim, NULL, "$R冥界的火焰漸漸的延燒到$N身上﹗$Z" );
    dam = number_range( dam, dam * ( ( 0 - ch->alignment ) / 300 ) );
    dam = UMAX( 0, UMIN( MAG_ATTACK_DAM, dam ) );
  }

  if ( ch->skill[sn] > 85 && number_percent() > 70 )
  {
    sact( ch, victim, NULL,
      "$S一陣豔藍的火焰之中﹐大天使$R路西法$S由天而降﹗$A"
      "$R路西法$S接著便用黑色的翅膀拍擊$N﹗$Z$A" );

    dam = number_range( 3 * dam / 2, dam * 3 );
    dam = UMAX( 0, UMIN( MAG_ATTACK_DAM, dam ) );
  }

  damage( ch, victim, dam, sn, ATTACK_RANDOM );

  RETURN( TRUE );
}

/* 龜息大法 */
SKILL( cast_dormancy )
{
  int  old;
  int  value;
  bool change = FALSE;

  PUSH_FUNCTION( "cast_dormancy" );

  old      = ch->hit;
  value    = number_range( get_curr_con( ch ), get_curr_con( ch ) * 2 );
  ch->hit  = UMIN( get_curr_hit( ch ), level + value + ch->hit  );
  if ( ch->hit > old ) change = TRUE;

  old      = ch->mana;
  value    = number_range( get_curr_int( ch ), get_curr_int( ch ) * 2 );
  ch->mana = UMIN( get_curr_mana( ch ), level + value + ch->mana );
  if ( ch->mana > old ) change = TRUE;

  old      = ch->move;
  value    = number_range( get_curr_dex( ch ), get_curr_dex( ch ) * 3 );
  ch->move = UMIN( get_curr_move( ch ) , level + value + ch->move );
  if ( ch->move > old ) change = TRUE;

  if ( change )
  {
    /* 獲得技能的熟練度 */
    gain_skill_exp( ch , sn , TRUE );

    send_to_char( "你的呼吸趨於緩慢﹐身體也真正得到解脫。\n\r", ch );
  }

  else
  {
    send_to_char( "你的呼吸又回復到正常的樣子。\n\r", ch );
  }

  RETURN( TRUE );
}

/* 易筋經 */
SKILL( cast_physique )
{
  int old;
  int value;

  PUSH_FUNCTION( "cast_physique" );

  old      = ch->move;
  value    = number_range( get_curr_dex( ch ), get_curr_dex( ch ) * 2 );
  ch->move = UMIN( get_curr_move( ch ) ,level + ch->move + value );

  if ( ch->move > old )
  {
    /* 獲得技能的熟練度 */
    gain_skill_exp( ch , sn , TRUE );

    switch( level / 20 )
    {
    default:
    case 0:
      send_to_char( "你似乎感到有數道真氣遊走於體內。\n\r", ch );
      break;

    case 1:
      send_to_char( "你體內的真氣越來越強烈﹐你快要控制不住。", ch );
      break;

    case 2:
      send_to_char( "你體內的真氣猶如龍行於天﹐如虛如幻。\n\r", ch );
      break;

    case 3:
      send_to_char( "只見你汗流浹背﹐數道真氣猶如火山般冒了出來。\n\r", ch );
      break;

    case 4:
      send_to_char( "你臉上一陣紫﹐一陣紅﹐體內說不出的快活。\n\r", ch );
      break;
    }
  }

  else
  {
    send_to_char( "你體內的數道真氣慢慢散去消失了﹗\n\r", ch );
  }

  RETURN( TRUE );
}

/* 打禪七 */
SKILL( cast_meditation )
{
  int old;
  int value;

  PUSH_FUNCTION( "cast_meditation" );

  old      = ch->mana;
  value   = number_range( get_curr_int( ch ), get_curr_int( ch ) * 2 );
  ch->mana = UMIN( get_curr_mana( ch ) ,level + ch->mana + value );

  if ( ch->mana > old )
  {
    /* 獲得技能的熟練度 */
    gain_skill_exp( ch , sn , TRUE );
    send_to_char( "你氣息調勻﹐呼吸漸慢﹐心中卻有如千軍萬馬奔騰。\n\r", ch );
  }

  else
  {
    send_to_char( "你已經無法繼續冥想了。\n\r", ch );
  }

  RETURN( TRUE );
}

/* 經脈逆行 */
SKILL( cast_anti_physique )
{
  int old;
  int value;

  PUSH_FUNCTION( "cast_anti_physique" );

  old     = ch->hit;
  value   = number_range( get_curr_con( ch ) * 2, get_curr_con( ch ) * 4 );
  ch->hit = UMIN( get_curr_hit( ch ), ch->hit + value + level );

  if ( ch->hit > old )
  {
    /* 獲得技能的熟練度 */
    gain_skill_exp( ch , sn , TRUE );

    switch( level / 50 )
    {
    default:
    case 0:
      send_to_char( "身上氣血翻轉﹐經脈逆行﹗\n\r", ch );
      break;

    case 1:
      send_to_char( "身上奇經八脈皆逆行﹐但有種說不出來的痛快。\n\r", ch );
      break;
    }
  }

  else
  {
    send_to_char( "你的血氣又恢復正常。\n\r", ch );
  }

  RETURN( TRUE );
}

SKILL( cast_venom )
{
  OBJ_DATA * obj = ( OBJ_DATA * ) vo;
  int        timer;

  PUSH_FUNCTION( "cast_venom" );

  if ( !obj )
  {
    mudlog( LOG_DEBUG, "cast_venom: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( obj->venom > 0 )
  {
    act( "對不起﹐$p已經抹毒了﹗", ch, obj, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  switch( obj->item_type )
  {
  default:
    break;

  case ITEM_MONEY:
  case ITEM_BOAT:
  case ITEM_CORPSE_NPC:
  case ITEM_CORPSE_PC:
  case ITEM_FOUNTAIN:
  case ITEM_LETTER:

    act( "你是沒有辦法在$p上面抹毒的﹗", ch, obj, NULL, TO_CHAR );
    RETURN( TRUE );
  }

  obj->venom = timer = UMAX( 1, level /5 );

  act( "小心﹐$n已經在$p上抹了毒﹐時效是$I小時。", ch, obj, &timer, TO_ALL );
  RETURN( TRUE );
}

SKILL( cast_develop )
{
  OBJ_DATA * obj = ( OBJ_DATA * ) vo;
  int        max;
  int        diff;
  int        enforce;

  PUSH_FUNCTION( "cast_develop" );

  if ( !obj )
  {
    mudlog( LOG_DEBUG, "cast_develop: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( !is_armor( obj ) || obj->max_armor <= 0 )
  {
    act( "$p是無法增強的喔﹗", ch, obj, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  if ( obj->armor < ( ( obj->max_armor * 80 ) / 100 ) )
  {
    act( "你先把$p修理好一點﹐再來加強吧﹗", ch, obj, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  max = UMIN( MAX_ARMOR , ( ( level / 33 ) + 1 ) * obj->max_armor );

  if ( ( diff = max - obj->armor ) <= 0 )
  {
    act( "$p已經增強達到頂極了﹗", ch, obj, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  enforce    = number_range( 1, level * 2 );
  obj->armor = UMIN( max, obj->armor + enforce );

  act( "$n把$p的防護力又提昇了不少﹗", ch, obj, NULL, TO_ALL );
  RETURN( TRUE );
}

SKILL( cast_repair )
{
  OBJ_DATA * obj = ( OBJ_DATA * ) vo;
  int        max;
  int        enforce;

  PUSH_FUNCTION( "cast_repair" );

  if ( !obj )
  {
    mudlog( LOG_DEBUG, "cast_repair: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( !is_armor( obj ) || obj->max_armor <= 0 )
  {
    act( "$p目前無法修理的喔﹗", ch, obj, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  if ( ( max = obj->max_armor - obj->armor ) <= 0 )
  {
    act( "$p好的很﹐不用修理了﹗", ch, obj, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  enforce = UMIN( number_range( 1, level * 20 ), max );
  obj->armor = UMIN( obj->armor + enforce, obj->max_armor );

  if ( obj->armor >= obj->max_armor )
    act( "$n努力地修理$p﹐$p看起來和新的一樣了﹗", ch, obj, NULL, TO_ALL );
  else
    act( "$n盡力地修理$p﹐但技術不夠成熟﹐還得努力﹗", ch, obj, NULL, TO_ALL );

  RETURN( TRUE );
}

SKILL( cast_preserve )
{
  OBJ_DATA * obj = ( OBJ_DATA * ) vo;

  PUSH_FUNCTION( "cast_preserve" );

  if ( !obj )
  {
    mudlog( LOG_DEBUG, "cast_preserve: 缺乏來源." );
    RETURN( FALSE );
  }

  switch( obj->item_type )
  {
  default:
    act( "$p是無法做防腐處理的﹗", ch, obj, NULL, TO_CHAR );
    RETURN( FALSE );

  case ITEM_FOOD:
    break;
  }

  if ( obj->timer >= level * 30 )
  {
    act( "$p的保存期限已經超出你能的極限了﹗", ch, obj, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  obj->timer += number_range( 10, ( level / 2 ) * 10 );

  act( "$n施展防腐術﹐只見$p的表面籠罩著一股特殊氣流。", ch, obj, NULL, TO_ALL );
  RETURN( TRUE );
}

SKILL( cast_cook )
{
  OBJ_DATA * obj = ( OBJ_DATA * ) vo;
  OBJ_DATA * meat;

  PUSH_FUNCTION( "cast_cook" );

  if ( !obj || !ch->in_room )
  {
    mudlog( LOG_DEBUG, "cast_cook: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( ch->in_room->nobject >= MaxRoomObject )
  {
    send_to_char( "這裡的物品太多了﹐請先奉獻掉一些吧﹗\n\r", ch );
    RETURN( FALSE );
  }

  switch( obj->item_type )
  {
  default:
    act( "$p是無法拿來料理的﹗", ch, obj, NULL, TO_CHAR );
    RETURN( FALSE );

  case ITEM_CORPSE_NPC:
    break;
  }

  if ( !( meat = create_object( ObjMeat, 1 ) ) )
  {
    send_to_char( "對不起﹐系統作不出肉塊出來﹗\n\r", ch );
    mudlog( LOG_DEBUG, "cast_cook: 沒有肉塊 %d.", ObjMeatVnum );
    RETURN( FALSE );
  }

  act( "$n用刀把$p切了切﹐料理成$P。", ch, obj, meat, TO_ALL );

  extract_obj( obj );
  obj_to_room( meat, ch->in_room );
  RETURN( TRUE );
}

SKILL( cast_steal )
{
  char         buf[MAX_STRING_LENGTH];
  char         arg1[MAX_INPUT_LENGTH];
  char         arg2[MAX_INPUT_LENGTH];
  CHAR_DATA  * victim;
  OBJ_DATA   * obj;
  char       * argument = ( char * ) vo;
  int          percent;
  int          amount;

  PUSH_FUNCTION( "cast_steal" );

  if ( !ch || !ch->in_room )
  {
    mudlog( LOG_DEBUG, "cast_steal: 來源錯誤." );
    RETURN( FALSE );
  }

  if ( !argument || !*argument )
  {
    send_to_char( "你要從誰身上偷什麼東西﹖\n\r", ch );
    RETURN( FALSE );
  }

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );

  if ( arg1[0] == '\x0' || arg2[0] == '\x0' )
  {
    send_to_char( "你想從誰身上偷什麼東西呢﹖\n\r", ch );
    RETURN( FALSE );
  }

  if ( ch->pcdata && ch->pcdata->steal_panity > 0 )
  {
    act( "嗯﹐不要偷的太勤快喔﹗會被人發現喔﹗", ch, NULL, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  if ( !( victim = get_char_room( ch, arg2 ) ) )
  {
    act( "你要偷的人 $2$T$0 不在這裡﹗", ch, NULL, arg2, TO_CHAR );
    RETURN( FALSE );
  }

  if ( victim == ch )
  {
    send_to_char( "你要偷你自己的東西﹐你有沒有搞錯﹗\n\r", ch );
    RETURN( FALSE );
  }

  if ( IS_IMMORTAL( victim ) )
  {
    act( "你不可以偷大神$N的裝備﹗", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  if ( !IS_NPC( ch ) && excess_filequota( ch ) )
  {
    send_to_char( "對不起﹐你的檔案太大﹐無法執行這個命令﹗\n\r", ch );
    RETURN( FALSE );
  }

  if ( !can_steal( ch, victim ) ) RETURN( FALSE );

  percent = number_percent() + ( IS_AWAKE( victim ) ? 20 : -30 );

  if ( ch->level + 5 < victim->level
    || victim->position == POS_FIGHTING
    || ( !IS_NPC( ch ) && percent > level ) )
  {
    /* 失敗 */
    send_to_char( "真是糟糕﹐竟然失手啦﹐手氣真背﹗\n\r", ch );
    act( "$n嘗試從你身上偷點東西當作紀念品。", ch, NULL, victim, TO_VICT );
    act( "$n嘗試從$N身上偷點東西來當作紀念品。",  ch, NULL, victim, TO_NOTVICT );

    sprintf( buf, "%s是個下流的小偷﹗", mob_name( NULL, ch ) );

    ChatRecord = FALSE;
    do_yell( victim, buf );
    ChatRecord = TRUE;

    if ( !IS_NPC( ch ) )
    {
      if ( IS_NPC( victim )
        && !( victim->fighting )
        && !IS_SET( victim->act, ACT_NOKILL )
        && ch->in_room->Safe == FALSE )
      {
        striking( victim, ch, TYPE_UNDEFINED );
      }

      else
      {
        mudlog( LOG_INFO , "[STEAL] %s 偷東西失敗﹐對象﹕ %s﹗"
          , ch->name, victim->name );

        if ( !IS_SET( ch->act, PLR_THIEF ) )
        {
          SET_BIT( ch->act, PLR_THIEF );
          send_to_char( "你現在是一個小偷﹗\n\r", ch );
          save_char_obj( ch , SAVE_FILE );
        }
      }
    }

    RETURN( TRUE );
  }

  if ( !str_prefix( arg1, "coins" ) || !str_cmp( arg1, "gold" ) )
  {
    amount = victim->gold * number_range( 1, 10 ) / 100;
    if ( amount <= 0 || over_scale( ch ) )
    {
      act( "你沒辦法從$N身上再搞到錢了。", ch, NULL, victim, TO_CHAR );
      RETURN( TRUE );
    }

    mudlog( LOG_INFO, "[STEAL] %s 偷錢成功\﹐對象﹕%s."
      , ch->name, victim->name );
    gold_to_char( ch, amount );
    gold_from_char( victim, amount );
    act( "真是美妙﹗你偷到了$i兩黃金。", ch, &amount, NULL, TO_CHAR );
    if ( ch->pcdata ) ch->pcdata->steal_panity = 3;
    RETURN( TRUE );
  }

  if ( !( obj = get_obj_carry( victim, arg1 ) ) )
  {
    send_to_char( "翻來翻去﹐竟然找不到你要的東西﹐哇哩咧﹗\n\r", ch );
    RETURN( TRUE );
  }

  if ( !can_see_obj( ch , obj ) ) RETURN( TRUE );

  if ( !can_drop_obj( ch, obj )
    || obj->address
    || obj->Inventory
    || obj->level > ch->level )
  {
    act( "你偷不到$p耶。", ch, obj, NULL, TO_CHAR );
    RETURN( TRUE );
  }

  if ( ch->carry_number + get_obj_number( obj ) > can_carry_n( ch ) )
  {
    send_to_char( "你自己身上已經攜帶太多物品了。\n\r", ch );
    RETURN( TRUE );
  }

  if ( ch->carry_weight + get_obj_weight( obj ) > can_carry_w( ch ) )
  {
    send_to_char( "你自己身上已經攜帶太重的物品了。\n\r", ch );
    RETURN( TRUE );
  }

  mudlog( LOG_INFO, "[STEAL] %s 偷東西成功\﹐對象﹕%s 物品 %s ."
    , ch->name, victim->name , obj->name );

  obj_from_char( obj );
  obj_to_char( obj, ch );
  act( "$p到手了﹗", ch, obj, NULL, TO_CHAR );
  if ( ch->pcdata ) ch->pcdata->steal_panity = 5;

  RETURN( TRUE );
}

SKILL( cast_pick )
{
  extern const sh_int  rev_dir [];
  char                 arg[MAX_INPUT_LENGTH];
  CHAR_DATA          * gch;
  OBJ_DATA           * obj;
  int                  door;
  char               * argument = ( char * ) vo;
  ROOM_INDEX_DATA    * to_room;
  EXIT_DATA          * pexit;
  EXIT_DATA          * pexit_rev;

  PUSH_FUNCTION( "cast_pick" );

  if ( !ch || !ch->in_room )
  {
    mudlog( LOG_DEBUG, "cast_pick: 來源錯誤." );
    RETURN( FALSE );
  }

  if ( !argument || !*argument )
  {
    send_to_char( "你要打開哪一個鎖呢﹖\n\r", ch );
    RETURN( FALSE );
  }

  one_argument( argument, arg );

  for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
  {
    if ( IS_NPC( gch ) && IS_AWAKE( gch ) && ch->level + 5 < gch->level )
    {
      act( "$N就在你身邊﹐所以無法打開。", ch, NULL, gch, TO_CHAR );
      RETURN( FALSE );
    }
  }

  if ( !IS_NPC( ch ) && number_percent() > level )
  {
    send_to_char( "你失敗了﹗\n\r", ch);
    RETURN( TRUE );
  }

  if ( ( obj = get_obj_here( ch, arg ) ) )
  {
    if ( obj->item_type != ITEM_CONTAINER )
    {
      act( "$p不是一個容器﹗", ch, obj, NULL, TO_CHAR );
      RETURN( FALSE );
    }

    if ( !IS_SET( obj->value[1], CONT_CLOSED ) )
    {
      act( "$p並沒有關起來啊﹗", ch, obj, NULL, TO_CHAR );
      RETURN( FALSE );
    }

    if ( obj->value[2] < 0 )
    {
      act( "你不能打開$p﹗", ch, obj, NULL, TO_CHAR );
      RETURN( FALSE );
    }

    if ( !IS_SET( obj->value[1], CONT_LOCKED ) )
    {
      act( "$p已經打開了﹗", ch, obj, NULL, TO_CHAR );
      RETURN( FALSE );
    }

    if ( IS_SET( obj->value[1], CONT_PICKPROOF ) )
    {
      send_to_char( "你失敗了﹗\n\r", ch );
      RETURN( FALSE );
    }

    REMOVE_BIT(obj->value[1], CONT_LOCKED);
    send_to_char( "喀啦一聲﹐你把鎖打開了﹗\n\r", ch );
    act( "$n把$p的鎖打開了。", ch, obj, NULL, TO_ROOM );
    RETURN( TRUE );
  }

  if ( ( door = find_door( ch, arg ) ) >= 0 )
  {
    pexit = ch->in_room->exit[door];

    if ( !pexit->closed )
    {
      send_to_char( "它並沒有關起來。\n\r", ch );
      RETURN( FALSE );
    }

    if ( pexit->key < 0 )
    {
      send_to_char( "你不能打開它。\n\r", ch );
      RETURN( FALSE );
    }

    if ( !pexit->locked )
    {
      send_to_char( "它並沒有鎖起來。\n\r", ch );
      RETURN( FALSE );
    }

    if ( pexit->pick )
    {
      send_to_char( "你失敗了。\n\r", ch );
      RETURN( TRUE );
    }

    pexit->locked = FALSE;
    send_to_char( "喀啦一聲﹐你把鎖打開了。\n\r", ch );
    act( "$n把$d的鎖打開了。", ch, NULL, pexit->keyword, TO_ROOM );

    if ( ( to_room   = pexit->to_room               )
      && ( pexit_rev = to_room->exit[rev_dir[door]] )
      && pexit_rev->to_room == ch->in_room )
    {
      pexit_rev->locked = FALSE;
    }
  }
  RETURN( TRUE );
}

SKILL( cast_defloration )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         move;
  int         add;

  PUSH_FUNCTION( "cast_deloration" );

  if ( !ch || !victim )
  {
    mudlog( LOG_DEBUG, "cast_defloration" );
    RETURN( FALSE );
  }

  if ( victim->move <= 1 )
  {
    act( "$N的精華已經被你吸乾了。", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  move = UMAX( 5, number_range( 1, level * 2 ) );
  victim->move = UMAX( 1, victim->move - move );

  if ( level > 20 )
  {
    add = move * UMIN( 30, level ) / 100;
    if ( add > 0 ) ch->move = UMIN( ch->max_move, ch->move + add );
  }

  act( "$n伸出手指對著$N一勾﹗無數的力量湧入$n的體內﹐使$n舒暢無比﹗"
       "而$N頓覺全身虛脫無力﹗剎那間﹐$N覺得自己開始神經耗弱﹗"
       , ch, NULL, victim, TO_ALL );

  RETURN( TRUE );
}

SKILL( cast_cannibal )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         hit;
  int         add;

  PUSH_FUNCTION( "cast_cannibal" );

  if ( !ch || !victim )
  {
    mudlog( LOG_DEBUG, "cast_cannibal" );
    RETURN( FALSE );
  }

  if ( victim->hit <= 1 )
  {
    act( "$N的精氣已經被你吸乾了。", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  hit = UMAX( 5, number_range( 1, level * 2 ) );
  victim->hit = UMAX( 1, victim->hit - hit );

  if ( level > 20 )
  {
    add = hit * UMIN( 30, level ) / 100;
    if ( add > 0 ) ch->hit = UMIN( ch->max_hit, ch->hit + add );
  }

  act( "$n對著$N一指﹐一道如血般腥紅的光線向$N射去﹐$N的生命能源被$n吸收了。"
       "$n由手中發出一道紅光擊中了$N﹐$N感到自己體內的鮮血正不停的流失﹗"
       , ch, NULL, victim, TO_ALL );

  /* 若對手尚未戰鬥, 則開啟戰鬥 */
  if ( !victim->fighting ) set_fighting( victim, ch );

  /* 若主角尚未戰鬥, 則開啟戰鬥 */
  if ( !ch->fighting ) set_fighting( ch, victim );

  RETURN( TRUE );
}

SKILL( cast_energy_drain )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         mana;
  int         add;

  PUSH_FUNCTION( "cast_energy_drain" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_energy_drain: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( victim->mana <= 1 )
  {
    act( "$N的精氣已經被你吸乾了。", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  mana = UMAX( 5, number_range( 1, level * 2 ) );
  victim->mana = UMAX( 1, victim->mana - mana );

  if ( level > 20 )
  {
    add = mana * UMIN( 30, level ) / 100;
    if ( add > 0 ) ch->mana = UMIN( ch->max_mana, ch->mana + add );
  }

  act( "$n對著$N一指﹐一道如血般腥紅的光線向$N射去﹐$N的生命能源被$n吸收了。"
       "$n由手中發出一道紅光擊中了$N﹐$N感到自己體內的鮮血正不停的流失﹗"
       , ch, NULL, victim, TO_ALL );

  /* 若對手尚未戰鬥, 則開啟戰鬥 */
  if ( !victim->fighting ) set_fighting( victim, ch );

  /* 若主角尚未戰鬥, 則開啟戰鬥 */
  if ( !ch->fighting ) set_fighting( ch, victim );

  RETURN( TRUE );
}

SKILL( cast_wakeup )
{
  AFFECT_DATA * pAffect;
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;

  PUSH_FUNCTION( "cast_wakeup" );

    if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_wakeup: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( !is_affected( victim, SLOT_SLEEP ) )
  {
    act( "$N並沒有睡著啊﹗", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  if ( !is_affected( victim, SLOT_SLEEP ) )
  {
    act( "$N並沒有受到催眠術的影響﹗", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  for ( pAffect = victim->affected; pAffect; pAffect = pAffect->next )
  {
    if ( pAffect->type == SLOT_SLEEP ) break;
  }

  if ( !pAffect && is_affected( victim, SLOT_SLEEP ) )
  {
    act( "$N睡的不怎麼深沈﹐$n把$N叫醒了﹗", ch, NULL, victim, TO_ALL );
    RETURN( TRUE );
  }

  else if ( pAffect )
  {
    if ( pAffect->duration > ( level / 5 + 5 ) )
    {
      send_to_char( "你的法術失效了。\n\r", ch );
      RETURN( FALSE );
    }

    act( "一陣輕輕的微風吹過﹐解除了$N的催眠狀態。$A"
      "$n現在可以將$N搖醒($2wake$0)﹗", ch, NULL, victim, TO_ALL );

    affect_release( victim, SLOT_SLEEP );
    RETURN( TRUE );
  }

  else
  {
    act( "$N並沒有在睡眠喔﹗", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  RETURN( FALSE );
}

SKILL( cast_evil_power )
{
  AFFECT_DATA af;

  PUSH_FUNCTION( "cast_evil_power" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "cast_evil_power:來源錯誤." );
    RETURN( FALSE );
  }

  if ( is_affected( ch, sn ) )
  {
    send_to_char( "你已經夠邪惡了﹗\n\r", ch );
    RETURN( FALSE );
  }

  act( "天上的邪魔啊﹐賜給$n永遠無窮的力量吧﹗", ch, NULL, NULL, TO_ALL );

  af.type      = sn;
  af.duration  = number_range( level / 6, level / 5 );
  af.location  = APPLY_STR;
  af.modifier  = 1;
  af.level     = level;
  affect_join( ch, &af );

  RETURN( TRUE );
}

SKILL( cast_crash_weapon )
{
  CHAR_DATA  * victim = ( CHAR_DATA * ) vo;
  OBJ_DATA   * obj;
  OBJ_DATA   * crash;
  SITUS_DATA * pSitus;
  int          count;
  int          damage;

  PUSH_FUNCTION( "cast_crash_weapon" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_crash_weapon" );
    RETURN( FALSE );
  }

  crash = NULL;
  count = 0;

  for ( pSitus = situs_list; pSitus; pSitus = pSitus->next )
  {
    if ( !( obj = get_eq_char( victim, pSitus->location ) )
      || !can_see_obj( victim, obj ) ) continue;

    if ( number_range( 0, count++ ) == 0 ) crash = obj;
  }

  if ( crash == NULL || crash->max_armor <= 0 )
  {
    act( "$N身上已經沒有可以攻擊的裝備了﹗", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  sact( ch, victim, crash,
    "$n小心觀望﹐一找到機會就往$N的$O重重攻擊下去﹗" );

  damage = number_range( level * 2, level * 4 );
  crash->armor -= damage;

  if ( crash->armor <= 0 )
  {
    act( "$N身上的$p無法承受這突來的猛烈一擊﹐碎成了兩半﹗"
      , ch, crash, victim, TO_ALL );

    unequip_char( victim, crash, FALSE );
    extract_obj( crash );
  }

  /* 若對手尚未戰鬥, 則開啟戰鬥 */
  if ( !victim->fighting ) set_fighting( victim, ch );

  /* 若主角尚未戰鬥, 則開啟戰鬥 */
  if ( !ch->fighting ) set_fighting( ch, victim );

  RETURN( TRUE );
}

SKILL( cast_flying_stone )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         dam;

  PUSH_FUNCTION( "cast_flying_stone" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "cast_flying_stone: 來源錯誤." );
    RETURN( FALSE );
  }

  /* 攻傷敘述 */
  act( "$n雙手煽動﹐揮舞起一故巨大的旋風﹐挾帶著無數的風沙向著$N飛去﹗"
    , ch, NULL, victim, TO_ALL );

  /* 設定攻傷 */
  dam = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_ONE );

  damage( ch, victim, dam, sn, ATTACK_RANDOM );

  RETURN( TRUE );
}

SKILL( cast_slow )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA af;

  PUSH_FUNCTION( "cast_slow" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "cast_slow: 來源錯誤." );
    RETURN( FALSE );
  }

  if ( saves_spell( level, victim ) && get_curr_dex( victim ) < 1 )
  {
    act( "你對$N的遲滯術似乎沒有什麼用。", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  act( "$n將精神集中﹐以雙手揮出一團黑雲困住$N的行動﹗"
    , ch, NULL, victim, TO_ALL );

  if ( ( number_percent() + victim->level / 2 )
    > ( number_percent() + level ) )
  {
    act( "但是$N瞬間很輕巧的閃過了這個黑雲﹗", ch, NULL, victim, TO_ALL );
    RETURN( FALSE );
  }

  act( "這團黑雲絆住了$N的行動﹗", ch, NULL, victim, TO_ALL );

  af.type      = sn;
  af.duration  = number_range( level / 20, level / 15 );
  af.location  = APPLY_DEX;
  af.modifier  = -1;
  af.level     = level;
  affect_join( victim, &af );

  RETURN( TRUE );
}

SKILL( cast_mint )
{
  OBJ_DATA       * obj = ( OBJ_DATA * ) vo;
  OBJ_DATA       * target;
  OBJ_INDEX_DATA * pIndex;

  PUSH_FUNCTION( "cast_mint" );

  if ( !obj )
  {
    mudlog( LOG_DEBUG, "cast_repair: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( obj->item_type != ITEM_ORE
    || !( pIndex = get_obj_index( obj->value[0] ) )
    || obj->value[2] <= 0 )
  {
    act( "$p是無法進行鑄造的﹗", ch, obj, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  if ( !IS_NPC( ch ) && excess_filequota( ch ) )
  {
    send_to_char( "對不起﹐你的檔案太大﹐無法執行這個命令﹗\n\r", ch );
    RETURN( FALSE );
  }

  if ( level < obj->value[1] )
  {
    act( "看來$p非你能力所及﹐請別人來試試看﹗", ch, obj, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  act( "$n揮著汗水﹐一邊努力地精鍊著$p﹗", ch, obj, NULL, TO_ALL );

  if ( number_range( 0, 10000 - level * 10 ) >= obj->value[2] )
  {
    act( "看來$p還有的你忙了﹐加油吧﹗", ch, obj, NULL, TO_CHAR );
  }

  else
  {
    target = create_object( pIndex, -1 );

    act( "大功\告成﹐$n終於從$p打造出$P﹗", ch, obj, target, TO_ALL );

    if ( target->Takeable == FALSE
      || ( ch->carry_number + get_obj_number( target ) > can_carry_n( ch ) )
      || ( get_carry_weight( ch ) + get_obj_weight( target ) > can_carry_w( ch ) ) )
    {
      act( "因為$n無法拿起$p﹐所以掉到地上去了﹗", ch, target, NULL, TO_ALL );
      obj_to_room( target, ch->in_room );
    }

    else
    {
      act( "你突然發現到你的身上多了一樣$p﹗", ch, target, NULL, TO_CHAR );
      obj_to_char( target, ch );
    }

    obj_from_char( obj );
    extract_obj( obj );
  }

  RETURN( TRUE );
}

SKILL( cast_magic_power )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         chance;

  PUSH_FUNCTION( "cast_magic_power" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_magic_power: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( IS_NPC( victim ) )
  {
    chance = level + ( get_curr_dex( ch ) * 3 - get_curr_dex( victim ) * 2 );
  }

  else
  {
    chance = get_curr_dex( ch ) - get_curr_dex( victim );
  }

  RETURN( TRUE );
}

/* 看看是否可以偷竊 ch 是施法者, victim 是對象.*/
bool can_steal( CHAR_DATA * ch , CHAR_DATA * victim )
{
  PUSH_FUNCTION( "can_fight" );

  if ( !ch || !victim || !ch->in_room )
  {
    mudlog( LOG_DEBUG, "can_fight: 缺乏來源." );
    RETURN( FALSE );
  }

  /* 如果房間的旗標是安全的, 一定不能發生戰鬥 */
  if ( ch->in_room->Safe )
  {
    act( "對不起﹐$r裡是不能偷竊的。" , ch, NULL, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  if ( ( fight_info->status == PK_JOIN || fight_info->status == PK_BET )
    && is_pk( ch ) )
  {
    send_to_char( "休息中請勿偷竊﹗\n\r", ch );
    RETURN( FALSE );
  }

  /* 玩家如果被迷惑, 不能偷自己的主人 */
  if ( is_affected( ch, SLOT_CHARM_PERSON ) && ch->master == victim )
  {
    act( "請不要Ａ主人$N的東西。", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  /* 若是主人一定不能偷, 要先宣戰 */
  if ( ch->master == victim || victim->master == ch )
  {
    send_to_char( "你沒有自主﹐還可以亂偷嗎﹖\n\r" , ch );
    RETURN( FALSE );
  }

  if ( is_same_group( ch, victim ) && ch != victim )
  {
    act( "$N是你親愛的隊友ㄟ﹐居然偷$E東西﹖", ch, NULL, victim, TO_CHAR );
    act( "快逃吧﹗$n想要偷你的東西﹗"        , ch, NULL, victim, TO_VICT );
    RETURN( FALSE );
  }

  if ( victim->mount_by )
  {
    act( "要偷東西先問主人$N吧﹗", ch, NULL, victim->mount_by, TO_CHAR );
    RETURN( FALSE );
  }

  if ( victim->boss && victim->boss == ch )
  {
    act( "有沒有搞錯﹐偷自己$N的寶物﹖", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  if ( victim->boss )
  {
    act( "$n很害怕地躲到$N身後﹐因為有人想要偷他東西﹗"
      , victim, NULL, victim->boss, TO_ALL );

     RETURN( FALSE );
  }

  if ( victim->hirer )
  {
    act( "你不能偷別人的傭兵$N。", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  /* 怪物的屬性是不能殺害 */
  if ( IS_NPC( victim ) && IS_SET( victim->act, ACT_NOKILL ) )
  {
    act( "你不能動$N就是不能動。", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

   /* 如果死了, 還沒奉獻出屍體時不能施展法術 */
  if ( !IS_NPC( ch ) && ch->pcdata->corpse )
  {
    send_to_char( "你覺得渾身使不上力﹐根本沒辦法偷東西。\n\r" , ch );
    RETURN( FALSE );
  }

  if ( !can_damage( victim ) )
  {
    act( "你就放過$N吧﹐你是不會損失多少的﹗", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  RETURN( TRUE );
}

SKILL( cast_abstract )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  OBJ_DATA  * pUrn;
  int         base;
  int         chance;

  PUSH_FUNCTION( "cast_abstract" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_abstract: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( victim == ch )
  {
    send_to_char( "什麼﹐你要封印你自己嗎﹖\n\r", ch );
    RETURN( FALSE );
  }

  if ( !can_damage( victim ) )
  {
    act( "你目前沒有辦法封印$N﹗", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  if ( !IS_NPC( victim ) || !victim->pIndexData )
  {
    act( "你只能封印非玩家﹐$N是不可以被封印的﹗", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  if ( !can_fight( ch , victim ) ) RETURN( FALSE );

  if ( victim->mount_by || victim->boss )
  {
    act( "對不起﹐你不能封印別人的坐騎或小鬼$N﹗", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  base   = UMAX( 100, number_range( 1, victim->level * victim->level ) );
  chance = ch->level + level * 2 + get_curr_wis( ch ) * 2;

  if ( base > chance )
  {
    act( "$n的道行還不夠﹐似乎無法將$N封印住﹗", ch, NULL, victim, TO_ALL );
  }

  else
  {
    pUrn = create_object( ObjUrn, -1 );
    pUrn->value[0] = victim->pIndexData->vnum;
    pUrn->value[1] = victim->hit;
    pUrn->value[2] = victim->mana;
    pUrn->value[3] = victim->move;

    obj_to_char( pUrn, ch );

    act( "突然﹐$N被$n封印住了﹐而$N的身影也慢慢消失在$p之中﹗"
      , ch, pUrn, victim, TO_ALL );

    raw_kill( ch, victim, FALSE );
  }

  RETURN( TRUE );
}

SKILL( cast_dark_hell )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         dam;

  PUSH_FUNCTION( "cast_dark_hell" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_dark_hell: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( ch->alignment > -( AlignRange / 5 ) && number_percent() > 50 )
  {
    act( "$n的雙手突然不聽使喚﹐無法施展法術﹗", ch, NULL, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  /* 前置訊息 */
  act( "$A$n雙手像天一指﹐兩眼發出紅光﹐口中喃喃唸著不知名的咒語…$A"
    , ch, NULL, victim, TO_ALL );

  dam = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_MAG );

  if ( ch->skill[sn] > 45 && number_percent() > 60 )
  {
    sact( ch, victim, NULL, "地底下傳來一陣聲音﹕「召喚我的人啊﹗"
      "我依你的引導前來了﹗」$A一個巨大身影顯現眼前﹐全身漆黑"
      "什麼都看不見﹐只見一陣漆黑慢慢擴散﹐包覆所有的物體﹗$A"
      "$A$A$A$b                    死黑核爆裂地獄              $Z"
      "         $A$A$A爆破聲充滿了你的耳朵﹐你覺得非常難過﹗" );

    dam = number_range( dam * 4 / 3, dam * 2 );
    dam = UMAX( 0, UMIN( MAG_ATTACK_DAM, dam ) );

    if ( ch->skill[sn] > 80 )
    {
      sact( ch, victim, NULL, "$A爆破的碎片瞬間又對$N造成第二次傷害﹗" );
      dam = number_range( dam, dam * 4 / 3 );
      dam = UMAX( 0, UMIN( MAG_ATTACK_DAM, dam ) );
    }
  }
  else
  {
    sact( ch, victim, NULL, "$n雙手發出一陣黑光﹐漸漸漲大無法控制﹐"
      "$A於是使力將兩手之黑球合而為一﹐丟向$N﹗$A$A$A"
      "                    $b         死黑核地獄          "
      "               $Z$A$A$A黑球融合所產生的極度熱量燃燒著$N﹗$Z" );
    dam = number_range( dam, dam * 4 / 3 );
    dam = UMAX( 0, UMIN( MAG_ATTACK_DAM, dam ) );
  }

  damage( ch, victim, dam, sn, ATTACK_RANDOM );

  RETURN( TRUE );
}

SKILL( cast_six_god )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         dam;
  int         choice[] = { 20, 40, 60, 80, 95, 100, -1 };

  PUSH_FUNCTION( "cast_six_god" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_six_god: 來源錯誤." );
    RETURN( FALSE );
  }

  if ( IS_EVIL( ch ) )
  {
    act( "$n太邪惡了﹐無法召喚神佛﹗", ch, NULL, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  dam = spell_damage( ch, level, TYPE_WIS, SPELL_CLASS_FIV );
  dam = number_range( dam * 3 / 4, dam * 5 / 4 );
  dam = URANGE( 0, dam, LEVEL_FIV_DAM );

  switch( dimemsion_position( choice, level, TRUE ) )
  {
  case ERRORCODE:
    mudlog( LOG_DEBUG, "技能 %d 的訊息設定有問題.", sn );
    RETURN( FALSE );

  case 0:
    sact( ch, victim, NULL, "$n口中唸唸有詞﹐手結$Y『八大金剛印』$Z使出"
      "金剛勁﹐雄渾霸道﹐勁力如錐疾鑽向$N。" );
    break;

  case 1:
    sact( ch, victim, NULL, "面對$N攻勢﹐$n不慌不忙﹐結起$B『"
      "降龍羅漢印』$Z﹐口中唸唸有詞﹐周圍突然產生強軔柔勁﹐四兩"
      "撥千斤﹐盡卸$N攻勢﹗" );
    break;

  case 2:
    sact( ch, victim, NULL, "$n結起$S『觀音亂影印』$Z喝道﹕$W『尊請千手"
      "觀音現真身』$Z﹐剎那間﹐$N彷彿看見$W千手觀音$Z駕臨﹐$W千手觀音$Z突然"
      "展開千手﹐迅捷紛亂的旋渦招式﹐順間對$N轟出千百擊﹗" );
    break;

  case 3:
    sact( ch, victim, NULL, "$N一招劈來﹐$n往右一閃﹐急結$P『怒雷"
      "震心印』$Z﹐雷神附體﹐霎時快若奔雷﹐急若飛電﹐早繞到$N身後"
      "﹐祭起強大電勁﹐朝$N後心猛擊﹗" );
    break;

  case 4:
    sact( ch, victim, NULL, "$n衝向$N﹐手結$R『菩薩弘法印』$Z只見"
      "$n突然被烈火包圍﹐$N閃避不及﹐被法印轟個正著﹐$N只覺的"
      "凜冽灼熱的$r火燄氣勁$Z在體內竄湧﹐$N哀叫一聲﹐登時五內如焚"
      "﹐不支倒地﹗" );
    break;

  case 5:
    sact( ch, victim, NULL, "$n腳一蹬﹐一飛衝天﹐在空中盤腿而"
      "坐﹐手結$Y『大業如來印』$Z﹐只見天際金光萬道﹐彩霞片片﹐$n"
      "口中唸唸有詞﹐突然暴喝$Y『人神合一 天上地下 唯我獨尊』$Z"
      "﹐西天如來赫然駕臨﹐法相莊嚴﹐$N只覺得心神一震﹐身體突"
      "然被一股巨力貫穿﹗" );
    break;
  }

  if ( ch->alignment > ( AlignRange * 3 / 4 ) )
  {
    sact( ch, victim, NULL, "$n的$W強烈聖潔$Z對$N造成了額外的傷害﹗" );
    dam = number_range( dam * 5 / 4, dam * 2 );
    dam = URANGE( 0, dam, LEVEL_FIV_DAM );
  }
  else if ( ch->alignment < ( AlignRange / 2 ) )
  {
    sact( ch, victim, NULL, "$n的心不夠純淨﹐威力大減﹗" );
    dam = number_range( dam / 2, dam * 2 / 3 );
    dam = URANGE( 0, dam, LEVEL_FIV_DAM );
  }

  damage( ch, victim, dam, sn, ATTACK_RANDOM );

  RETURN( TRUE );
}

SKILL( cast_water_cloud_blast )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         choice[] = { 9, 19, 29, 39, 49, 59, 69, 79, 89, 100, -1 };
  int         tt;
  int         multi;
  int         value;
  int         dam = 0;
  OBJ_DATA  * pObj;

  PUSH_FUNCTION( "cast_water_cloud_blast" );

  if ( !( pObj = get_wield( ch ) ) )
  {
    mudlog( LOG_DEBUG, "cast_water_cloud_blast: 武器錯誤." );
    RETURN( FALSE );
  }

  level = number_range( level / 2, level );
  switch( dimemsion_position( choice, level, TRUE ) )
  {
  case ERRORCODE:
    mudlog( LOG_DEBUG, "cast_water_cloud_blast: 出界了." );
    RETURN( FALSE );

  case 0:
    sact( ch, victim, pObj,
      "$n輕輕一笑﹐緩緩地轉身﹐手指慢慢地搭上了$O弓弦﹐突然錚的一聲﹐弓弦"
      "疾振﹐$S水雲箭法$Z之「$R柔情$S似水$Z」使出﹐一箭射向了$N$Z。" );
    dam = 300;
    break;

  case 1:
    sact( ch, victim, pObj,
      "$n身形疾移﹐動作之快令人目不暇給﹐彷彿有幾十個$n包圍住$N﹐搞得"
      "$N「$S滿頭霧水$Z」﹐一臉茫然﹐$n趁機向$N背後射出一箭。$Z" );
    dam = 350;
    break;

  case 2:
    sact( ch, victim, pObj,
      "$n輕提身法﹐將真氣往丹田集中﹐心中殺意頓時消減無蹤﹐臉上露出慈悲"
      "和藹的笑容﹐取$O﹐搭箭﹐拉滿弓﹐所有動作一氣呵成﹐如「$S行雲流水$Z」"
      "般將箭射向$N。" );
    dam = 400;
    break;

  case 3:
    sact( ch, victim, pObj,
      "$n將雙眼慢慢閤上﹐收斂心神﹐將各種感官提昇到極限﹐心中「$S明鏡如水"
      "$Z」反應出$N的所有動作﹐在看清一切後﹐一箭射向$N的要害。$Z" );
    dam = 450;
    break;

  case 4:
    sact( ch, victim, pObj,
      "$n運轉真氣﹐將勁力灌滿雙腿﹐縱身一躍﹐飛上半空中﹐由上而下睥睨地"
      "上萬物﹐嘴角一揚﹐$S水雲箭法$Z如「$W水銀$S瀉地$Z」般狂擊而下。" );
    dam = 500;
    break;

  case 5:
    sact( ch, victim, pObj,
      "$n身形急轉﹐幻化出雲霧般的光影﹐四周頓時陷入一片迷濛連日光都"
      "照不進來﹐一道金光由$n身上的$O透出﹐一式「$S雲嵐破日$Z」$Y金$Z光一"
      "閃﹐穿透了$N。$Z" );
    dam = 600;
    break;

  case 6:
    sact( ch, victim, pObj,
      "在緊張的戰鬥中$n突然停了下來﹐旁若無人地拿出$R玫瑰花煙$Z﹐開始抽"
      "了起來﹐「$S吞雲吐霧$Z」三輪之後﹐$n精神大振﹐重重一箭幾乎射穿了"
      "$N$Z。" );
    dam = 700;
    break;

  case 7:
    sact( ch, victim, pObj,
      "$n雙手一揚﹐黑色的氣體由$O中散出﹐形成一團巨大的黑雲﹐遮住"
      "了日月光芒﹐$S水雲箭法$Z中至邪一招「$B烏雲蔽日$Z」使出﹐$N不禁打了"
      "個冷顫。$Z" );
    dam = 800;
    break;

  case 8:
    sact( ch, victim, pObj,
      "$n提氣三巡﹐氣貫全身經脈﹐身行螺旋急轉﹐直衝而上﹐強大的氣勁"
      "籠罩大地﹐$n彷彿變大了數十倍﹐雙眼$R血紅$Z狂態畢露﹐如「$R殃雲$S天降"
      "$Z」直衝而下﹐讓$N$Z防備不及。" );
    dam = 900;
    break;

  case 9:
    sact( ch, victim, pObj,
      "$n將精氣神三者匯一﹐斷絕一切對外氣息﹐四周開始產生變化﹐$G風起$B"
      "雲湧$Z﹐$y大地震動$Z﹐$Y日月$b無光$Z﹐$N身處$n所使出的「$S雲萊$P魔境$Z」之中"
      "﹐只能呆呆被亂箭射死了。$Z" );
    dam = 1000;
    break;
  }

  for ( tt = 0; tt <= level / 33; tt++ )
  {
    if ( !verify_char( victim )
      || !can_damage( victim )
      || victim->in_room != ch->in_room ) break;

    /* 武器的加成 */
    if ( !IS_NPC( ch )
      && pObj
      && pObj->item_type == ITEM_WEAPON
      && pObj->value[3] == WEAPON_BOW )
    {
      pObj->value[1] = UMIN( pObj->value[1], 1000 );
      pObj->value[2] = UMIN( pObj->value[2], 1000 );

      multi = number_range( pObj->value[1], pObj->value[2] );
    }
    else
    {
      multi = 100;
    }

    value = variation( ( dam * multi / 100 ), 10 );
    value = value + tt * level;
    damage( ch, victim, value, sn, ATTACK_RANDOM );
  }

  RETURN( TRUE );
}

SKILL( cast_fun_wu_blast )
{

  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         choice[] = { 9, 19, 29, 39, 49, 59, 69, 79, 89, 100, -1 };
  int         tt;
  int         multi;
  int         value;
  int         dam = 0;
  OBJ_DATA  * pObj;

  PUSH_FUNCTION( "cast_fun_wu_blast" );

  if ( !( pObj = get_wield( ch ) ) )
  {
    mudlog( LOG_DEBUG, "cast_water_cloud_blast: 武器錯誤." );
    RETURN( FALSE );
  }

  level = number_range( level / 2, level );
  switch( dimemsion_position( choice, level, TRUE ) )
  {
  case ERRORCODE:
    mudlog( LOG_DEBUG, "cast_water_cloud_blast: 出界了." );
    RETURN( FALSE );

  case 0:
    sact( ch, victim, pObj,
      "$A             $P 鳳舞箭法 $Z之$R 一鳳沖天$Z$A$A"
      "$n輕移腳步﹐在$N的猛烈攻擊中東竄西閃﹐好不容易找到一個空檔"
      "﹐$n立即拿出你的$O﹐勁貫右手﹐拉滿弓弦﹐整張弓幾乎拉成"
      "了圓形﹐猛然一箭射向天空﹐$n射出的箭幻化成一隻$R火鳳凰$Z直衝上天"
      "﹐在空中盤旋數周後﹐往$N狂撲而去。" );
    dam = 450;
    break;

  case 1:
    sact( ch, victim, pObj,
      "$A              $P 鳳舞箭法 $Z之 $G二鳳交旋$Z$A$A"
      "$n將所有動作放慢﹐任自己的身軀承受$N無情的猛烈打擊﹐站穩步"
      "伐﹐將重心放在下盤﹐穩如泰山﹐隨手抽出二支$W銀箭$Z﹐搭在$O"
      "上﹐真氣一提﹐雙腳入土三分﹐$W雙箭$Z同時疾射而出﹐貫上螺旋氣勁﹐"
      "$W雙箭$Z如兩隻$R血鳳凰$Z交錯飛翔﹐本身還自行螺旋轉動﹐交織成一道強大"
      "的$R血紅$S光箭$Z﹐往$N直射而去。" );
    dam = 500;
    break;

  case 2:
    sact( ch, victim, pObj,
      "$A               $P鳳舞箭法 $Z之 $Y破鳳三射$A$Z$A"
      "$n彎弓搭箭﹐射出第一枝箭﹐在箭剛射出的同時馬上再射出第二箭﹐"
      "接著第三箭緊隨而出﹐三箭幾乎是連在一起的一支超級長箭一樣﹐猶"
      "如拖著長長尾巴的破滅鳳凰﹐狠狠往$N的身體衝去﹐一箭接著一箭"
      "﹐造成$N身上$l被攻擊了三次﹐幾乎要了$N的狗命。" );
    dam = 600;
    break;

  case 3:
    sact( ch, victim, pObj,
      "$A               $P鳳舞箭法$Z 之$B 鳳遊四海$Z$A$A"
      "望著天上的白雲﹐$n心情不自覺輕鬆了起來﹐眼前的$N看起來也覺得"
      "可愛﹐施展開$P鳳舞身法$Z閃開$N的致命攻擊之後﹐$n頭也不回地飄逸"
      "而去﹐心中只想悠閒的遊山玩水﹐再也不想戰鬥﹐就在$n一時鬆懈之"
      "時﹐$N狠狠攻擊$n的背後﹐巨大的痛覺逼使$n不得不使出殺招﹐為"
      "了配合心情﹐$P鳳舞箭法$Z中最優雅的「$B鳳遊四海$Z」使出﹐$N被突如其"
      "來的$S水鳳凰$Z吞噬了。" );
    dam = 700;
    break;

  case 4:
    sact( ch, victim, pObj,
      "$P              $P鳳舞箭法 $Z之$P 彩鳳五劈$Z$A$A"
      "面對$N的煩人攻擊﹐$n只能左格右擋﹐抽不出時間來抽出背上的$O"
      "攻擊﹐加上想起了昨天跟小風吵架﹐被莫名其妙地冤枉﹐最後還被拋"
      "棄﹐一股怨氣由心中升起﹐複雜的$R悲傷、憤怒、無奈$Z等情緒糾結在心"
      "中﹐$n將無處發洩的怨恨通通加諸在$N身上﹐連箭也懶得拔了﹐直"
      "接把$O舉了起來﹐狠狠往$N的$l上劈了五下﹐鮮血飛濺"
      "而出﹐在陽光照射下﹐交織成美麗的$R五彩鳳凰$Z。" );
    dam = 800;
    break;

  case 5:
    sact( ch, victim, pObj,
      "$P              $P鳳舞箭法 $Z之 $R火鳳六擊$Z$A$A"
      "$n使出$P鳳舞身法$Z至極一式「$Y幻鳳舞化$Z」﹐身形一分為六﹐分站$N的"
      "六角方位﹐形成一個$S六芒星伏魔陣$Z﹐將$N封在其中﹐六個$n同時彎"
      "弓搭箭﹐射向天空﹐六箭在空中交相互擊﹐幻化出巨大的$R烈火鳳凰$Z﹐"
      "牠雙翼一揚﹐六顆巨大的$R火球$Z從天而降﹐砸向伏魔陣中﹐$N"
      "因為無法閃躲﹐只能眼睜睜被砸。" );
    dam = 900;
    break;

  case 6:
    sact( ch, victim, pObj,
      "$A$P              鳳舞箭法$Z 之$S 狂鳳七殺$Z$A$A"
      "$n雙眼血紅﹐形態若狂﹐披頭散髮﹐全身上下散發出無以倫比的凶猛"
      "殺意﹐$n將身上原本的箭袋通通丟棄﹐從懷中掏出另一個箭袋﹐袋中"
      "滿是折成七截的短箭﹐將箭一拉﹐成為一枝枝淬著劇毒的七殺毒箭﹐"
      "晶藍色的箭頭閃閃發亮﹐映著$n冰霜般的冷笑﹐見血封喉的毒箭讓人"
      "覺得$N的死期大概不遠了。" );
    dam = 1000;
    break;

  case 7:
    sact( ch, victim, pObj,
      "$A$P               鳳舞箭法$Z 之$Y 翔鳳八方$Z$A$A"
      "$n凝聚真氣﹐將$P鳳舞身法$Z發揮的淋漓盡致﹐在戰場中穿梭自如﹐完全"
      "不將$N放在眼裡﹐在真氣快速流轉﹐身形愈來愈快之下﹐$N已經"
      "被搞得昏頭轉向了﹐迷濛中$n的身影已經化成了$R七$G彩$B翔$Y鳳$Z﹐在醞釀足"
      "夠的氣勢之後﹐一式$S鳳翼天翔$Z使出﹐幻箭如雨般從四面八方射向$N"
      "﹐真正定睛一看﹐$N身上插了八枝箭﹐枝枝貫穿$N的身體。" );
    dam = 1100;
    break;

  case 8:
    sact( ch, victim, pObj,
      "$A$P              鳳舞箭法$Z 之$R 鳳舞九天$A$Z$A"
      "$n將全身的精竅閉鎖﹐五感完全封閉﹐斷絕一切與外界的聯繫﹐漸漸"
      "地﹐$S風$Z的流動﹐$Y氣$Z的變化﹐完完全全映入心眼之中﹐外界的一切"
      "變化都能清楚感受到﹐$N的一舉一動、肌肉變化﹐都在掌握之"
      "中﹐$n猛然睜眼一瞪﹐$N被氣勢所鎮攝﹐一連八箭射出﹐箭箭"
      "都算準了$N的退路﹐八箭之後﹐$N已經避無可避﹐奪命的第九箭"
      "直接射中了$N的$l。" );
    dam = 1200;
    break;

  case 9:
    sact( ch, victim, pObj,
      "$A$P              鳳舞箭法 $Z之 $R千鳳$S嵐$Y舞$A$Z$A"
      "$n運起$P鳳舞箭法$Z中唯一的心法--$Y「$G一無所有$Z﹐$B無所不有$Y」$Z﹐將真氣灌"
      "進弓弦﹐蓄滿氣勁的弓弦頓時$S嗡嗡作響$Z﹐輕輕一提真氣﹐握穩弓柄﹐"
      "左手一撥弓弦﹐勁氣頓時疾衝而出﹐千道萬道氣箭分別化成千萬隻不"
      "同的$R鳳凰$Z﹐舞出各形各色詭異曼妙的舞姿﹐傳說中的詛咒之舞讓$N"
      "看得心曠神怡﹐完全不知道自己已經被氣箭割得體無完膚了。" );
    dam = 1400;
    break;
  }

  for ( tt = 0; tt <= level / 20; tt++ )
  {
    if ( !verify_char( victim )
      || !can_damage( victim )
      || victim->in_room != ch->in_room ) break;

    /* 武器的加成 */
    if ( !IS_NPC( ch )
      && pObj
      && pObj->item_type == ITEM_WEAPON
      && pObj->value[3] == WEAPON_BOW )
    {
      pObj->value[1] = UMIN( pObj->value[1], 1000 );
      pObj->value[2] = UMIN( pObj->value[2], 1000 );

      multi = number_range( pObj->value[1], pObj->value[2] );
    }
    else
    {
      multi = 100;
    }

    value = variation( ( dam * multi / 100 ), 10 );
    value = value + tt * level;
    damage( ch, victim, value, sn, ATTACK_RANDOM );
  }

  RETURN( TRUE );
}

SKILL( cast_make_sword )
{
  OBJ_INDEX_DATA * pObj;
  OBJ_DATA       * pSword;

  PUSH_FUNCTION( "cast_make_sword" );

  if ( !ch || !verify_char( ch ) || !( pObj = get_obj_index( 410 ) ) )
    RETURN( FALSE );

  if ( !( pSword = create_object( pObj, ch->level ) ) )
  {
    send_to_char( "無法創出魔法劍﹗\n\r", ch );
    RETURN( FALSE );
  }

  pSword->timer    = 3 + ( level / 10 );
  pSword->value[1] = UMAX( 100, 100 + level / 5 );
  pSword->value[2] = UMAX( 100, 100 + level / 2  );

  if ( ( ch->carry_number + 1 > can_carry_n( ch ) )
    || ( ch->carry_weight + get_obj_weight( pSword ) > can_carry_w( ch ) ) )
    obj_to_room( pSword, ch->in_room );
  else
    obj_to_char( pSword, ch );

  act( "一陣閃光過後﹐$p從$n的手上凝聚出來﹗", ch, pSword, NULL, TO_ALL );

  RETURN( TRUE );
}

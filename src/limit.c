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

bool    check_weapon      args( ( CHAR_DATA *, int, bool ) );

#define CHECK_CHAR( character )                   \
{                                                 \
  if ( !character )                               \
  {                                               \
    mudlog( LOG_DEBUG , "檢查技能, ch 是空的." ); \
    RETURN( FALSE );                              \
  }                                               \
}

bool check_hand( CHAR_DATA * ch , int type )
{
  OBJ_DATA   * obj;
  SITUS_DATA * pSitus;
  int          kind;

  PUSH_FUNCTION( "check_hand" );

  for ( kind = 0 ,obj = ch->carrying; obj; obj = obj->next_content )
  {
    if ( ( obj->item_type != ITEM_WEAPON )
      || ( type != WEAPON_ALL && obj->value[3] != type ) ) continue;

    for ( pSitus = situs_list; pSitus; pSitus = pSitus->next )
      if ( pSitus->wield && pSitus->location == obj->wear_loc ) RETURN( TRUE );
  }

  RETURN( FALSE );
}

/* 確定是否可以再攻擊 */
bool check_continue( CHAR_DATA * ch, CHAR_DATA * victim )
{
  PUSH_FUNCTION( "check_continue" );

  if ( !ch || !victim
    || ch->in_room      != victim->in_room
    || ch->position     != POS_FIGHTING
    || victim->position != POS_FIGHTING ) RETURN( FALSE );

  RETURN( TRUE );
}

bool check_same_weapon( CHAR_DATA * ch, bool fPrint )
{
  OBJ_DATA * objr;
  OBJ_DATA * objl;

  PUSH_FUNCTION( "check_same_weapon" );

  objr = get_eq_char( ch, WEAR_WIELD );
  objl = get_eq_char( ch, WEAR_HOLD  );

  if ( objr
    && objl
    && objr->item_type == ITEM_WEAPON
    && objl->item_type == ITEM_WEAPON
    && objr->value[3] != objl->value[3] )
  {
    if ( fPrint ) act( "$n還不會雙手持武器﹐無法施展招式﹗"
      , ch, NULL, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  RETURN( TRUE );
}

/* 檢查武器型態是否正確 */
bool check_weapon( CHAR_DATA * ch, int type, bool fPrint )
{
  PUSH_FUNCTION( "check_weapon" );

  /* 檢查人物 */
  CHECK_CHAR( ch );

  switch( type )
  {
  default:
    RETURN( FALSE );

  /* 空手招式 */
  case WEAPON_HAND:

    if ( check_hand( ch , WEAPON_ALL ) )
    {
      if ( fPrint )
        send_to_char( "這招式必需空手﹐你無法施展這個技能﹗\n\r" , ch );
      RETURN( FALSE );
    }

    if ( check_same_weapon( ch, fPrint ) == FALSE ) RETURN( FALSE );
    RETURN( TRUE );

  /* 檢查是否拿劍 */
  case WEAPON_SWORD:

    if ( !check_hand( ch, WEAPON_SWORD ) )
    {
      if ( fPrint ) send_to_char( "你必須要拿劍才能施展劍術﹗\n\r" , ch );
      RETURN( FALSE );
    }

    if ( check_same_weapon( ch, fPrint ) == FALSE ) RETURN( FALSE );
    RETURN( TRUE );

  /* 檢查是否拿刀 */
  case WEAPON_BLADE:

    if ( !check_hand( ch, WEAPON_BLADE ) )
    {
      if ( fPrint ) send_to_char( "你必須要拿刀才能施展刀術﹗\n\r" , ch );
      RETURN( FALSE );
    }

    if ( check_same_weapon( ch, fPrint ) == FALSE ) RETURN( FALSE );
    RETURN( TRUE );

  /* 檢查是否拿匕首 */
  case WEAPON_DAGGER:

    if ( !check_hand( ch, WEAPON_DAGGER ) )
    {
      if ( fPrint ) send_to_char( "你必須要拿匕首才能施展匕首術﹗\n\r" , ch );
      RETURN( FALSE );
    }

    if ( check_same_weapon( ch, fPrint ) == FALSE ) RETURN( FALSE );
    RETURN( TRUE );

  /* 檢查是否拿斧 */
  case WEAPON_AXE:

    if ( !check_hand( ch, WEAPON_AXE ) )
    {
      if ( fPrint ) send_to_char( "你必須要拿斧頭才能施展斧頭術﹗\n\r" , ch );
      RETURN( FALSE );
    }

    if ( check_same_weapon( ch, fPrint ) == FALSE ) RETURN( FALSE );
    RETURN( TRUE );

  /* 檢查是否拿鞭 */
  case WEAPON_WHIP:

    if ( !check_hand( ch, WEAPON_WHIP ) )
    {
      if ( fPrint ) send_to_char( "你必須要拿鞭才能施展鞭術﹗\n\r" , ch );
      RETURN( FALSE );
    }

    if ( check_same_weapon( ch, fPrint ) == FALSE ) RETURN( FALSE );
    RETURN( TRUE );

  /* 檢查是否拿槍 */
  case WEAPON_SPEAR:

    if ( !check_hand( ch, WEAPON_SPEAR ) )
    {
      if ( fPrint ) send_to_char( "你必須要拿槍才能施展槍術﹗\n\r" , ch );
      RETURN( FALSE );
    }

    if ( check_same_weapon( ch, fPrint ) == FALSE ) RETURN( FALSE );
    RETURN( TRUE );

  /* 檢查是否拿筆 */
  case WEAPON_PEN:

    if ( !check_hand( ch, WEAPON_PEN ) )
    {
      if ( fPrint ) send_to_char( "你必須要拿筆才能施展筆術﹗\n\r" , ch );
      RETURN( FALSE );
    }

    if ( check_same_weapon( ch, fPrint ) == FALSE ) RETURN( FALSE );
    RETURN( TRUE );

  /* 檢查是否拿鎚 */
  case WEAPON_HAMMER:

    if ( !check_hand( ch, WEAPON_HAMMER ) )
    {
      if ( fPrint ) send_to_char( "你必須要拿鎚才能施展鎚術﹗\n\r" , ch );
      RETURN( FALSE );
    }

    if ( check_same_weapon( ch, fPrint ) == FALSE ) RETURN( FALSE );
    RETURN( TRUE );

  /* 檢查是否拿棒 */
  case WEAPON_CLUB:

    if ( !check_hand( ch, WEAPON_CLUB ) )
    {
      if ( fPrint ) send_to_char( "你必須要拿棒才能施展棒術﹗\n\r" , ch );
      RETURN( FALSE );
    }

    if ( check_same_weapon( ch, fPrint ) == FALSE ) RETURN( FALSE );
    RETURN( TRUE );

  /* 檢查是否拿弓 */
  case WEAPON_BOW:

    if ( !check_hand( ch, WEAPON_BOW ) )
    {
      if ( fPrint ) send_to_char( "你必須要拿弓才能施展弓術﹗\n\r" , ch );
      RETURN( FALSE );
    }

    if ( check_same_weapon( ch, fPrint ) == FALSE ) RETURN( FALSE );
    RETURN( TRUE );

  }
}

/* 檢查是否拿劍 */
bool check_sword_attack( CHAR_DATA * ch, bool fPrint )
{
  PUSH_FUNCTION( "check_sword_attack" );

  /* 檢查人物 */
  CHECK_CHAR( ch );

  /* 檢查是否拿劍 */
  if ( check_weapon( ch, WEAPON_SWORD, fPrint ) == FALSE ) RETURN( FALSE );
  RETURN( TRUE );
}

/* 檢查是否可以拳腳 */
bool check_unrigid_attack( CHAR_DATA * ch, bool fPrint )
{
  PUSH_FUNCTION( "check_unrigid_attack" );

  /* 檢查人物 */
  CHECK_CHAR( ch );

  /* 手上有武器則不能施展 */
  if ( check_weapon( ch, WEAPON_HAND, fPrint ) == FALSE ) RETURN( FALSE );
  RETURN( TRUE );
}

/* 檢查是否怪物可以用的拳腳 */
bool check_pc_attack( CHAR_DATA * ch, bool fPrint )
{
  PUSH_FUNCTION( "check_pc_attack" );

  /* 檢查人物 */
  CHECK_CHAR( ch );

  /* 手上有武器則不能施展 */
  if ( check_weapon( ch, WEAPON_HAND, fPrint ) == FALSE ) RETURN( FALSE );

  /* 怪物無法讀書 */
  if ( IS_NPC( ch ) ) RETURN( FALSE );

  RETURN( TRUE );
}

/* 檢查是否拿槍 */
bool check_spear_attack( CHAR_DATA * ch, bool fPrint )
{
  PUSH_FUNCTION( "check_spear_attack" );

  /* 檢查人物 */
  CHECK_CHAR( ch );

  /* 檢查是否拿槍 */
  if ( check_weapon( ch, WEAPON_SPEAR, fPrint ) == FALSE ) RETURN( FALSE );
  RETURN( TRUE );
}

/* 檢查是否拿刀 */
bool check_blade_attack( CHAR_DATA * ch, bool fPrint )
{
  PUSH_FUNCTION( "check_blade_attack" );

  /* 檢查人物 */
  CHECK_CHAR( ch );

  /* 檢查是否拿刀 */
  if ( check_weapon( ch, WEAPON_BLADE, fPrint ) == FALSE ) RETURN( FALSE );
  RETURN( TRUE );
}

/* 檢查是否拿鞭 */
bool check_whip_attack( CHAR_DATA * ch, bool fPrint )
{
  PUSH_FUNCTION( "check_whip_attack" );

  /* 檢查人物 */
  CHECK_CHAR( ch );

  /* 檢查是否拿鞭 */
  if ( check_weapon( ch, WEAPON_WHIP, fPrint ) == FALSE ) RETURN( FALSE );
  RETURN( TRUE );
}

/* 檢查是否拿匕首 */
bool check_dagger_attack( CHAR_DATA * ch, bool fPrint )
{
  PUSH_FUNCTION( "check_dagger_attack" );

  /* 檢查人物 */
  CHECK_CHAR( ch );

  /* 檢查是否拿匕首 */
  if ( check_weapon( ch, WEAPON_DAGGER, fPrint ) == FALSE ) RETURN( FALSE );
  RETURN( TRUE );
}

/* 檢查是否拿斧 */
bool check_axe_attack( CHAR_DATA * ch, bool fPrint )
{
  PUSH_FUNCTION( "check_axe_attack" );

  /* 檢查人物 */
  CHECK_CHAR( ch );

  /* 檢查是否拿斧 */
  if ( check_weapon( ch, WEAPON_AXE, fPrint ) == FALSE ) RETURN( FALSE );
  RETURN( TRUE );
}

/* 檢查是否拿棍棒 */
bool check_club_attack( CHAR_DATA * ch, bool fPrint )
{
  PUSH_FUNCTION( "check_club_attack" );

  /* 檢查人物 */
  CHECK_CHAR( ch );

  /* 檢查是否拿棍棒 */
  if ( check_weapon( ch, WEAPON_CLUB, fPrint ) == FALSE ) RETURN( FALSE );
  RETURN( TRUE );
}

/* 檢查是否拿筆 */
bool check_pen_attack( CHAR_DATA * ch, bool fPrint )
{
  PUSH_FUNCTION( "check_pen_attack" );

  /* 檢查人物 */
  CHECK_CHAR( ch );

  /* 檢查是否拿筆 */
  if ( check_weapon( ch, WEAPON_PEN, fPrint ) == FALSE ) RETURN( FALSE );
  RETURN( TRUE );
}

/* 檢查是否拿弓 */
bool check_bow_attack( CHAR_DATA * ch, bool fPrint )
{
  PUSH_FUNCTION( "check_bow_attack" );

  /* 檢查人物 */
  CHECK_CHAR( ch );

  /* 檢查是否拿弓 */
  if ( check_weapon( ch, WEAPON_BOW, fPrint ) == FALSE ) RETURN( FALSE );
  RETURN( TRUE );
}

/* 檢查是否拿槌子 */
bool check_hammer_attack( CHAR_DATA * ch, bool fPrint )
{
  PUSH_FUNCTION( "check_hammer_attack" );

  /* 檢查人物 */
  CHECK_CHAR( ch );

  /* 檢查是否拿槌子 */
  if ( check_weapon( ch, WEAPON_HAMMER, fPrint ) == FALSE ) RETURN( FALSE );
  RETURN( TRUE );
}

bool check_can_kill( CHAR_DATA * ch, CHAR_DATA * victim )
{
  PUSH_FUNCTION( "check_can_kill" );

  if ( !can_see( ch, victim )
    || !IS_NPC( victim )
    || IS_IMMORTAL( victim )
    || !can_damage( victim ) ) RETURN( FALSE );

  RETURN( TRUE );
}

int check_skill_rating( char * argument )
{
  PUSH_FUNCTION( "check_skill_rating" );

  switch( argument[0] )
  {
  default:
    RETURN( ERRORCODE );

  case 'c':
    if ( !str_prefix( argument, "create"    ) ) RETURN( RATING_CREATE );
    if ( !str_prefix( argument, "cure"      ) ) RETURN( RATING_CURE  );
    break;

  case 'd':
    if ( !str_prefix( argument, "darkness"  ) ) RETURN( RATING_DARKNESS );
    break;

  case 'e':
    if ( !str_prefix( argument, "earth"     ) ) RETURN( RATING_EARTH );
    if ( !str_prefix( argument, "evil"      ) ) RETURN( RATING_EVIL  );
    break;

  case 'f':
    if ( !str_prefix( argument, "fight"     ) )  RETURN( RATING_FIGHT  );
    if ( !str_prefix( argument, "fire"      ) )  RETURN( RATING_FIRE  );
    break;

  case 'l':
    if ( !str_prefix( argument, "light"     ) ) RETURN( RATING_LIGHT );
    if ( !str_prefix( argument, "lightning" ) ) RETURN( RATING_LIGHTNING );
    if ( !str_prefix( argument, "lost"      ) ) RETURN( RATING_LOST );
    break;

  case 'm':
    if ( !str_prefix( argument, "murder"    ) ) RETURN( RATING_MURDER );
    break;

  case 'p':
    if ( !str_prefix( argument, "poison"    ) ) RETURN( RATING_POISON );
    break;

  case 's':
    if ( !str_prefix( argument, "saint"     ) ) RETURN( RATING_SAINT );
    if ( !str_prefix( argument, "sing"      ) ) RETURN( RATING_SING  );
    if ( !str_prefix( argument, "skill"     ) ) RETURN( RATING_SKILL );
    break;

  case 't':
    if ( !str_prefix( argument, "thief"     ) ) RETURN( RATING_THIEF );
    break;

  case 'w':
    if ( !str_prefix( argument, "water"     ) ) RETURN( RATING_WATER );
    if ( !str_prefix( argument, "wind"      ) ) RETURN( RATING_WIND  );
    break;
  }

  RETURN( ERRORCODE );
}

bool check_bow_content( CHAR_DATA * ch )
{
  PUSH_FUNCTION( "check_bow_content" );
  RETURN( TRUE );
}

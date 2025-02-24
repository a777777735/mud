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

char * check_obj_value( int type, int loc, int value, int * ret, bool unused )
{
  int              flags;
  OBJ_INDEX_DATA * pObjIndex;

  PUSH_FUNCTION( "check_obj_value" );

  *ret = 0;
  if ( loc < 0 || loc >= MAX_OBJECT_VALUE ) RETURN( "陣列超出界線﹗" );

  switch( type )
  {
  default:
    RETURN( "不明的型態﹗" );

  case ITEM_LIGHT:

    switch( loc )
    {
    default:
      if ( unused && value != 0 ) RETURN( "數值未使用﹐而卻設定此數值﹗" );
      RETURN( "" );

    case 2:

      if ( value == 0 )
      {
        *ret = 1;
        RETURN( "燈籠類物品照明時間必須不能等於零﹗" );
      }
    }

    break;

  case ITEM_SCROLL:
  case ITEM_POTION:
  case ITEM_PILL:

    switch( loc )
    {
    default:
      if ( unused && value != 0 ) RETURN( "數值未使用﹐而卻設定此數值﹗" );
      RETURN( "" );

    case 0:

      if ( value < -10000 || value > 10000 )
      {
        *ret = 0;
        RETURN( "藥品類物品影響的數值必須介於 -10000 到 10000 之間﹗" );
      }
      break;

    case 1:

      if ( value > 0 )
      {
        switch( value )
        {
        default:
          *ret = 0;
          RETURN( "藥品類物品影響型態數值不在定義內﹗" );

        case OBJ_CAST_HIT:
        case OBJ_CAST_MANA:
        case OBJ_CAST_MOVE:
        case OBJ_CAST_IDENT:
        case OBJ_CURE_POISON:
        case OBJ_DETECT_INVS:
        case OBJ_CAST_FLY:
        case OBJ_CURE_BLIND:
        case OBJ_GIANT_STR:
        case OBJ_DETECT_HIDE:
        case OBJ_SNEAK:
        case OBJ_DETECT_EVIL:
        case OBJ_CHANGE_SEX:
        case OBJ_DETECT_MAGIC:
        case OBJ_DETECT_POISON:
        case OBJ_FAERIE_FOG:
        case OBJ_GATE:
        case OBJ_FIXITY:
        case OBJ_CAST_ALL:
        case OBJ_DETECT_MASK:
          break;
        }
      }
      break;

    case 2:

      if ( value > 0 && !get_skill( value ) )
      {
        *ret = 0;
        RETURN( "藥品類物品所施展的法術號碼不合理﹗" );
      }
      break;

    case 3:

      if ( value < 0 || value > 10000 )
      {
        *ret = 0;
        RETURN( "藥品類的藥劑量必須介於 0 到 10000 之間﹗" );
      }
      break;
    }

    break;

  case ITEM_WAND:
  case ITEM_STAFF:

    switch( loc )
    {
    default:
      if ( unused && value != 0 ) RETURN( "數值未使用﹐而卻設定此數值﹗" );
      RETURN( "" );

    case 4:
      if ( value > 0 )
      {
        switch( value )
        {
        default:
          *ret = 0;
          RETURN( "法杖內加強分類的號碼不存在﹗" );

        case RATING_SKILL:
        case RATING_WIND:
        case RATING_EARTH:
        case RATING_LIGHTNING:
        case RATING_POISON:
        case RATING_FIRE:
        case RATING_WATER:
        case RATING_DARKNESS:
        case RATING_LIGHT:
        case RATING_SAINT:
        case RATING_EVIL:
        case RATING_LOST:
        case RATING_CURE:
        case RATING_SING:
        case RATING_FIGHT:
        case RATING_MURDER:
        case RATING_CREATE:
        case RATING_THIEF:
        case RATING_ALL:
          break;
        }
      }
      break;

    case 5:

      if ( value < 0 || value > 1000 )
      {
        *ret = 0;
        RETURN( "法杖加強強度必須介於 0 到 1000 之間﹗" );
      }
      break;

    case 6:

      if ( value < 0 || value > MaxMana )
      {
        *ret = 0;
        RETURN( "法杖內最大法力必須大於等於零﹗" );
      }

      break;

    case 7:

      if ( value < 0 || value > MaxMana )
      {
        *ret = 0;
        RETURN( "法杖內目前法力必須大於等於零﹗" );
      }

      break;
    }

    break;

  case ITEM_WEAPON:

    switch( loc )
    {
    default:
      if ( unused && value != 0 ) RETURN( "數值未使用﹐而卻設定此數值﹗" );
      RETURN( "" );

    case 1:
      if ( value <= 0 || value > 1000 )
      {
        *ret = 100;
        RETURN( "武器最小傷害值必須在 0 到 1000 之間﹗" );
      }
      break;

    case 2:
      if ( value <= 0 || value > 1000 )
      {
        *ret = 100;
        RETURN( "武器最大傷害值必須在 0 到 1000 之間﹗" );
      }
      break;

    case 3:
      switch( value )
      {
      default:
        *ret = WEAPON_HAND;
        RETURN( "武器的種類在未定義範圍內﹗" );

      case WEAPON_HAND:
      case WEAPON_DAGGER:
      case WEAPON_SWORD:
      case WEAPON_BLADE:
      case WEAPON_AXE:
      case WEAPON_WHIP:
      case WEAPON_SPEAR:
      case WEAPON_PEN:
      case WEAPON_HAMMER:
      case WEAPON_CLUB:
      case WEAPON_BOW:
      case WEAPON_FORCE:
      case WEAPON_STEPS:
      case WEAPON_FLEE:
        break;
      }
      break;
    }

    break;

  case ITEM_TREASURE:
  case ITEM_FURNITURE:
  case ITEM_KEY:
  case ITEM_TRASH:
  case ITEM_BOAT:
  case ITEM_CORPSE_PC:
  case ITEM_CORPSE_NPC:
  case ITEM_FOUNTAIN:
  case ITEM_SIGIL:
  case ITEM_VELLUM:

    switch( loc )
    {
    default:
      if ( unused && value != 0 ) RETURN( "數值未使用﹐而卻設定此數值﹗" );
      RETURN( "" );
    }

    break;

  case ITEM_ARMOR:

    switch( loc )
    {
    default:
      if ( unused && value != 0 ) RETURN( "數值未使用﹐而卻設定此數值﹗" );
      RETURN( "" );

    case 0:
      if ( value < -1000 || value > 1000 )
      {
        *ret = 0;
        RETURN( "防具的防護等級必須在 -1000 到 1000 之間" );
      }
    }

    break;

  case ITEM_CONTAINER:

    switch( loc )
    {
    default:
      if ( unused && value != 0 ) RETURN( "數值未使用﹐而卻設定此數值﹗" );
      RETURN( "" );

    case 0:

      if ( value <= 0 || value > 1000 )
      {
        *ret = 1;
        RETURN( "容器的最大容量必須在 1 到 1000 之間﹗" );
      }

      break;

    case 1:
      flags = CONT_CLOSEABLE | CONT_PICKPROOF | CONT_CLOSED | CONT_LOCKED;
      if ( ( value & ~flags ) != 0 )
      {
        *ret = 0;
        RETURN( "容器的狀態旗標不對﹗" );
      }
      break;

    case 2:

      if ( value != 0 )
      {
        if ( !( pObjIndex = get_obj_index( value ) ) )
        {
          *ret = 0;
          RETURN( "容器的鑰匙號碼物品不存在﹗" );
        }

        if ( pObjIndex->item_type != ITEM_KEY )
        {
          *ret = 0;
          RETURN( "容器鑰匙物品型態不是鑰匙﹗" );
        }
      }

      break;
    }

    break;

  case ITEM_DRINK_CON:

    switch( loc )
    {
    default:
      if ( unused && value != 0 ) RETURN( "數值未使用﹐而卻設定此數值﹗" );
      RETURN( "" );

    case 0:

      if ( value <= 0 || value > 1000 )
      {
        *ret = 1;
        RETURN( "液體容器的最大容量不合法﹗" );
      }

      break;

    case 1:
      if ( value <= 0 || value > 1000 )
      {
        *ret = 1;
        RETURN( "液體容器的目前容量不合法﹗" );
      }

      break;

    case 2:

      if ( value != 0 && !liq_lookup( value ) )
      {
        *ret = liq_water->slot;
        RETURN( "液體容器內液體號碼不正確﹗" );
      }

      break;

    case 3:
      if ( value != TRUE && value != FALSE )
      {
        *ret = FALSE;
        RETURN( "液體容器是否有毒數值只能是 0 或 1﹗" );
      }

      break;
    }

    break;

  case ITEM_FOOD:

    switch( loc )
    {
    default:
      if ( unused && value != 0 ) RETURN( "數值未使用﹐而卻設定此數值﹗" );
      RETURN( "" );

    case 0:
      if ( value <= 0 || value > 1000 )
      {
        *ret = 1;
        RETURN( "食物的腐壞時間必須介於 1 到 1000 之間﹗" );
      }

      break;

    case 1:

      if ( value < 0 || value > 1000 )
      {
        *ret = 1;
        RETURN( "食物的現有的可食用次數必須介於 0 到 1000﹗" );
      }

      break;

    case 2:

      if ( value <= 0 || value > 1000 )
      {
        *ret = 1;
        RETURN( "食物的一口食量必須介於 1 到 1000 之間﹗" );
      }
      break;

    case 3:
      if ( value != TRUE && value != FALSE )
      {
        *ret = FALSE;
        RETURN( "食物是否有毒數值只能是 0 或 1﹗" );
      }
      break;

    case 4:

      if ( value != 0 && !get_obj_index( value ) )
      {
        *ret = 0;
        RETURN( "食物產生的物品號碼不存在﹗" );
      }

      break;
    }

    break;

  case ITEM_MONEY:

    switch( loc )
    {
    default:
      if ( unused && value != 0 ) RETURN( "數值未使用﹐而卻設定此數值﹗" );
      RETURN( "" );

    case 0:
      if ( value < 0 )
      {
        *ret = 0;
        RETURN( "金錢的數量小於零﹗" );
      }
      break;
    }

    break;

  case ITEM_MAGICSTONE:

    switch( loc )
    {
    default:
      if ( unused && value != 0 ) RETURN( "數值未使用﹐而卻設定此數值﹗" );
      RETURN( "" );

    case 0:
      break;

    case 1:
      break;

    case 2:
      break;

    case 3:
      break;

    case 4:
      break;

    case 5:
      break;
    }

    break;

  case ITEM_SPIRITJADE:

    switch( loc )
    {
    default:
      if ( unused && value != 0 ) RETURN( "數值未使用﹐而卻設定此數值﹗" );
      RETURN( "" );

    case 0:

      if ( value != 0 && !get_mob_index( value ) )
      {
        *ret = 0;
        RETURN( "鬼玉的小鬼號碼不存在﹗" );
      }

      break;

    case 1:
      if ( value < 0 || value > LEVEL_HERO )
      {
        *ret = 0;
        RETURN( "鬼玉內小鬼等級必須在 0 到 100 之間﹗" );
      }
      break;

    case 2:

      if ( value < 0 )
      {
        *ret = 0;
        RETURN( "鬼玉內小鬼經驗值必須大於等於零﹗" );
      }
      break;

    case 3:

      if ( !get_skill( value ) )
      {
        *ret = 1;
        RETURN( "小鬼的技能號碼不存在﹗" );
      }
      break;

    case 4:

      if ( value <= 0 || value > 100 )
      {
        *ret = 1;
        RETURN( "小鬼技能的熟練度必須在 1 到 100 之間﹗" );
      }
      break;
    }

    break;

  case ITEM_MYSTERY:

    switch( loc )
    {
    default:
      if ( unused && value != 0 ) RETURN( "數值未使用﹐而卻設定此數值﹗" );
      RETURN( "" );

    case 0:

      if ( !get_skill( value ) )
      {
        *ret = 1;
        RETURN( "秘笈的技能號碼不存在﹗" );
      }
      break;

    case 1:

      if ( value <= 0 || value > 100 )
      {
        *ret = 1;
        RETURN( "秘笈的熟練度必須介於 1 到 100 之間﹗" );
      }
      break;
    }

    break;

  case ITEM_LETTER:

    switch( loc )
    {
    default:
      if ( unused && value != 0 ) RETURN( "數值未使用﹐而卻設定此數值﹗" );
      RETURN( "" );

    case 0:

      if ( value < 0 )
      {
        *ret = 0;
        RETURN( "信件的剩餘時間必須大於等於零﹗" );
      }
      break;
    }

    break;

  case ITEM_ORE:

    switch( loc )
    {
    default:
      if ( unused && value != 0 ) RETURN( "數值未使用﹐而卻設定此數值﹗" );
      RETURN( "" );

    case 0:

      if ( !get_obj_index( value ) )
      {
        *ret = ObjProtypeVnum;
        RETURN( "礦物打造出物品號碼不存在﹗" );
      }

      break;

    case 1:

      if ( value <= 0 || value > LEVEL_HERO )
      {
         *ret = 1;
         RETURN( "礦物打造者等級必須介於 1 到 100 之間﹗" );
      }

      break;

    case 2:

      if ( value <= 0 || value > 10000 )
      {
        *ret = 1;
        RETURN( "礦物打造成功\機率必須介於 1 到 10000﹗" );
      }
      break;

    case 3:
      if ( value <= 0 )
      {
        *ret = 1;
        RETURN( "礦物打造的費用必須大於零﹗" );
      }
      break;
    }

    break;

  case ITEM_GOLDMINE:

    switch( loc )
    {
    default:
      if ( unused && value != 0 ) RETURN( "數值未使用﹐而卻設定此數值﹗" );
      RETURN( "" );

    case 0:

      if ( value <= 0 )
      {
        *ret = 1;
        RETURN( "聚寶盆的最大容量必須大於零﹗" );
      }

      break;

    case 1:

      if ( value < 0 )
      {
        *ret = 0;
        RETURN( "聚寶盆的累積容量必須大於等於零﹗" );
      }
      break;

    case 2:
      if ( value <= 0 || value > MAX_LEVEL )
      {
        *ret = 1;
        RETURN( "聚寶盆丟入物品最低等級必須介於 1 到 120 之間﹗" );
      }
      break;

    case  5:
    case  6:
    case  7:
    case  8:
    case  9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:

      if ( value > 0 && !get_obj_index( value ) )
      {
        *ret = ObjProtypeVnum;
        RETURN( "聚寶盆產生的物品不存在﹗" );
      }

      break;
    }

    break;

  case ITEM_FIREWORK:

    switch( loc )
    {
    default:
      if ( unused && value != 0 ) RETURN( "數值未使用﹐而卻設定此數值﹗" );
      RETURN( "" );

    case 0:

      if ( value <= 0 )
      {
        *ret = 1;
        RETURN( "煙火施放的距離必須大於零﹗" );
      }
      break;

    case 1:
      if ( value <= 0 || value > 10000 )
      {
        *ret = 1;
        RETURN( "煙火施放成功\機率必須介於 1 到 10000 之間﹗" );
      }
      break;
    }

    break;

  case ITEM_URN:

    switch( loc )
    {
    default:
      if ( unused && value != 0 ) RETURN( "數值未使用﹐而卻設定此數值﹗" );
      RETURN( "" );

    case 0:
      if ( value > 0 && !get_mob_index( value ) )
      {
        *ret = 0;
        RETURN( "妖壺裡封印的怪物號碼不存在﹗" );
      }
      break;

    case 1:

      if ( value < 0 || value > MaxHit )
      {
        *ret = 0;
        RETURN( "妖壺裡怪物的生命值必須大於零﹗" );
      }

      break;

    case 2:

      if ( value < 0 || value > MaxMana )
      {
        *ret = 0;
        RETURN( "妖壺裡怪物的法力數值必須大於零﹗" );
      }

      break;

    case 3:

      if ( value < 0 || value > MaxMove )
      {
        *ret = 0;
        RETURN( "妖壺裡怪物的體力數值必須大於零﹗" );
      }

      break;
    }

    break;

  case ITEM_BACKBAG:

    switch( loc )
    {
    default:
      if ( unused && value != 0 ) RETURN( "數值未使用﹐而卻設定此數值﹗" );
      RETURN( "" );

    case 0:

      if ( value <= 0 )
      {
        *ret = 1;
        RETURN( "箭袋裡最大裝載量必須大於零﹗" );
      }
      break;

    case 1:

      if ( value < 0 )
      {
        *ret = 0;
        RETURN( "箭袋裡目前裝載量必須大於等於零﹗" );
      }
      break;

    case 2:

      if ( value > 0 )
      {
        if ( !( pObjIndex = get_obj_index( value ) ) )
        {
          *ret = 0;
          RETURN( "箭袋裡的武器號碼不存在﹗" );
        }
        else if ( pObjIndex->item_type != ITEM_WEAPON )
        {
          *ret = 0;
          RETURN( "箭袋裡裝的物品不是武器類的東西﹗" );
        }
      }

      break;
    }

    break;

  case ITEM_ARROW:

    switch( loc )
    {
    default:
      if ( unused && value != 0 ) RETURN( "數值未使用﹐而卻設定此數值﹗" );
      RETURN( "" );

    case 0:

      if ( value <= 0 || value > 1000 )
      {
        *ret = 100;
        RETURN( "弓箭最大傷害值必須在 0 到 1000 之間﹗" );
      }
      break;

    case 1:

      if ( value <= 0 || value > 1000 )
      {
        *ret = 100;
        RETURN( "弓箭最小傷害值必須在 0 到 1000 之間﹗" );
      }
      break;
    }

    break;
  }

  RETURN( "" );
}

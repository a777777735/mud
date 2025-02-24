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

#ifdef __linux__
#include <linux/tcp.h>
#endif

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "merc.h"

const char * direction_name( int direction )
{
  int    size;
  const char * dir_name [] =
    {  "北", "東", "南", "西", "上", "下", "進入", "出去" };

  PUSH_FUNCTION( "direction_name" );

  size = sizeof( dir_name ) / sizeof( dir_name[0] );
  if ( direction < 0 || direction >= size ) RETURN( "未知" );
  RETURN( dir_name[direction] );
}

const char * direction_ename( int direction )
{
  int    size;
  const char * dir_ename [] =
    { "north", "east", "south", "west", "up", "down", "enter", "out" };

  PUSH_FUNCTION( "direction_name" );

  size = sizeof( dir_ename ) / sizeof( dir_ename[0] );
  if ( direction < 0 || direction >= size ) RETURN( "未知" );
  RETURN( dir_ename[direction] );
}

const char * get_homename( int sn )
{
  AREA_DATA * pArea;

  PUSH_FUNCTION( "get_homename" );

  for ( pArea = area_first; pArea; pArea = pArea->next )
    if ( pArea->serial == sn ) RETURN( pArea->name );

  RETURN( "不詳" );
}

const char * status_message( CHAR_DATA * ch )
{
  char * const StatusMessage [] =
  {
     "\e[1;31m兩眼一睜﹐兩腿一伸﹐相信很快就可以和閻羅王泡茶了﹗\e[0m",
     "\e[1;31m已經受傷過重﹐奄奄一息﹐命在旦夕﹗\e[0m",
     "\e[1;31m處於半昏迷狀態﹐隨時都可能摔倒昏去﹐眼睛也開始模糊﹗\e[0m",
     "\e[1;31m受傷過重﹐有如風中殘燭﹐隨時有可能斷氣﹗\e[0m",
     "\e[1;31m受了相當重的傷﹐只怕有生命危險﹐看來情況不太妙﹗\e[0m",

     "\e[1;32m一副頭重腳輕的模樣﹐正在勉強支撐不倒下﹗\e[0m",
     "\e[1;32m氣息沈重﹐動作開始散亂﹐看來所受的傷著實不輕﹗\e[0m",
     "\e[1;32m開始氣喘如牛﹐下盤不穩﹐出招已有明顯的破綻﹗\e[0m",
     "\e[1;32m似乎十分疲憊﹐看來需要好好休息﹗\e[0m",
     "\e[1;32m氣喘吁吁﹐看來狀況不太好﹐需要調理調理﹗\e[0m",

     "\e[1;33m受傷不輕﹐看不太清楚狀況是如何﹗\e[0m",
     "\e[1;33m受了幾處小傷﹐可是應該不礙事﹗\e[0m",
     "\e[1;33m動作似乎開始有點不靈光﹐但是仍然有條不紊﹗\e[0m",
     "\e[1;33m充滿了活力﹐看來是打出興趣來了﹗\e[0m",
     "\e[1;33m紅光滿面﹐氣血飽滿﹐正處於最佳狀況﹗\e[0m"
  };

  int size = sizeof( StatusMessage ) / sizeof( StatusMessage[0] );
  int choice;

  PUSH_FUNCTION( "status_message" );

  choice = UMAX( 0 , UMIN( get_curr_hit( ch ) , ch->hit ) )
              * ( size - 1 ) / ( UMAX( 1, get_curr_hit( ch ) ) );

  RETURN( StatusMessage[choice] );
}

#ifndef TCP_ESTABLISHED
#define TCP_ESTABLISHED         1
#endif

#ifndef TCP_SYN_SENT
#define TCP_SYN_SENT            2
#endif

#ifndef TCP_SYN_RECV
#define TCP_SYN_RECV            3
#endif

#ifndef TCP_FIN_WAIT1
#define TCP_FIN_WAIT1           4
#endif

#ifndef TCP_FIN_WAIT2
#define TCP_FIN_WAIT2           5
#endif

#ifndef TCP_TIME_WAIT
#define TCP_TIME_WAIT           6
#endif

#ifndef TCP_CLOSE
#define TCP_CLOSE               7
#endif

#ifndef TCP_CLOSE_WAIT
#define TCP_CLOSE_WAIT          8
#endif

#ifndef TCP_LAST_ACK
#define TCP_LAST_ACK            9
#endif

#ifndef TCP_LISTEN
#define TCP_LISTEN              10
#endif

#ifndef TCP_CLOSING
#define TCP_CLOSING             11
#endif

const char * tcp_status( int status )
{
  PUSH_FUNCTION( "tcp_status" );

  switch( status )
  {
  default:              RETURN( "TCP_UNKOWN"  );
  case TCP_ESTABLISHED: RETURN( "ESTABLISHED" );
  case TCP_SYN_SENT:    RETURN( "SYN_SENT"    );
  case TCP_SYN_RECV:    RETURN( "SYNC_RECV"   );
  case TCP_FIN_WAIT1:   RETURN( "FIN_WAIT1"   );
  case TCP_FIN_WAIT2:   RETURN( "FIN_WAIT2"   );
  case TCP_TIME_WAIT:   RETURN( "WAIT"        );
  case TCP_CLOSE:       RETURN( "CLOSE"       );
  case TCP_CLOSE_WAIT:  RETURN( "CLOSE_WAIT"  );
  case TCP_LAST_ACK:    RETURN( "LAST_ACK"    );
  case TCP_LISTEN:      RETURN( "LISTEN"      );
  case TCP_CLOSING:     RETURN( "CLOSING"     );
  }
  RETURN( "Unknown" );
}

const char * adeptation_name( int adept )
{
  int     size;
  int     count;
  char *  const adeptation [] =
  {
    "\e[0;37m毫無根基\e[0m" ,
    "\e[0;31m初學乍練\e[0m" ,
    "\e[0;32m粗通皮毛\e[0m" ,
    "\e[0;33m初窺門徑\e[0m" ,
    "\e[0;34m馬馬虎虎\e[0m" ,
    "\e[0;35m心神領會\e[0m" ,
    "\e[1;31m出類拔萃\e[0m" ,
    "\e[1;32m神乎其技\e[0m" ,
    "\e[1;33m出神入化\e[0m" ,
    "\e[1;34m登峰造極\e[0m" ,
    "\e[1;36m一代宗師\e[0m" ,
    "\e[1;37m深不可測\e[0m"
  };

  PUSH_FUNCTION( "adeptation_name" );

  if ( adept <= 0 ) RETURN( adeptation[0] );

  size  = sizeof( adeptation ) / sizeof( adeptation[0] );
  count = ( adept / 10 ) + 1;

  if ( count < 0 || count >= size ) RETURN( "未知" );

  RETURN( adeptation[count] );
}

char * effect_type_name( int type )
{
  PUSH_FUNCTION( "effect_type_name" );

  switch( type )
  {
  default:
  case EFFECT_NONE:                     RETURN( "未知從缺" );
  case EFFECT_VICTIM_MANA:              RETURN( "對方法力" );
  case EFFECT_SELF_MANA:                RETURN( "自己法力" );
  case EFFECT_VICTIM_MOVE:              RETURN( "對方體力" );
  case EFFECT_SELF_MOVE:                RETURN( "自己體力" );
  case EFFECT_VICTIM_BLINDNESS:         RETURN( "對方眼盲" );
  case EFFECT_VICTIM_CURSE:             RETURN( "對方詛咒" );
  case EFFECT_VICTIM_POISON:            RETURN( "對方中毒" );
  case EFFECT_VICTIM_SLEEP:             RETURN( "對方昏睡" );
  case EFFECT_VICTIM_PARA:              RETURN( "對方麻痺" );
  case EFFECT_VICTIM_FAERIE_FIRE:       RETURN( "對方精靈之火" );
  case EFFECT_VICTIM_HIT:               RETURN( "對方生命" );
  case EFFECT_SELF_HIT:                 RETURN( "自己生命" );
  case EFFECT_HIT_DIE:                  RETURN( "一擊必殺" );
  }
}

char * get_position( int number )
{
  PUSH_FUNCTION( "get_position" );

  switch( number )
  {
  default:           RETURN( "未知" );
  case POS_DEAD:     RETURN( "死亡" );
  case POS_SLEEPING: RETURN( "睡覺" );
  case POS_RESTING:  RETURN( "休息" );
  case POS_FIGHTING: RETURN( "戰鬥" );
  case POS_STANDING: RETURN( "站著" );
  }
}

char * get_log( int number )
{
  PUSH_FUNCTION( "get_log" );

  switch( number )
  {
  default:          RETURN( "未知"     );
  case LOG_NORMAL:  RETURN( "正常標記" );
  case LOG_ALWAYS:  RETURN( "永遠標記" );
  case LOG_NEVER :  RETURN( "永不標記" );
  case LOG_WIZ   :  RETURN( "神族標記" );
  }
}

char * sex_name( int sex )
{
  PUSH_FUNCTION( "sex_name" );
  switch( sex )
  {
  case SEX_MALE:    RETURN( "男性" );
  case SEX_FEMALE:  RETURN( "女性" );
  case SEX_NEUTRAL: RETURN( "中性" );
  default:          RETURN( "不明" );
  }
}

char * obj_cast_location( int location )
{
  PUSH_FUNCTION( "obj_cast_location" );

  switch( location )
  {
  default:                RETURN( "未知從缺" );
  case OBJ_CAST_HIT:      RETURN( "影響生命" );
  case OBJ_CAST_MANA:     RETURN( "影響法力" );
  case OBJ_CAST_MOVE:     RETURN( "影響體力" );
  case OBJ_CAST_IDENT:    RETURN( "鑑定術"   );
  case OBJ_CURE_POISON:   RETURN( "解毒術"   );
  case OBJ_DETECT_INVS:   RETURN( "偵測隱形" );
  case OBJ_DETECT_MASK:   RETURN( "偵測易容" );
  case OBJ_CAST_FLY:      RETURN( "飛行術"   );
  case OBJ_CURE_BLIND:    RETURN( "治療眼盲" );
  case OBJ_GIANT_STR:     RETURN( "巨人之力" );
  case OBJ_DETECT_HIDE:   RETURN( "偵測藏匿" );
  case OBJ_SNEAK:         RETURN( "潛行"     );
  case OBJ_DETECT_EVIL:   RETURN( "偵測邪惡" );
  case OBJ_CHANGE_SEX:    RETURN( "變性術"   );
  case OBJ_DETECT_MAGIC:  RETURN( "偵測魔法" );
  case OBJ_DETECT_POISON: RETURN( "偵測毒物" );
  case OBJ_FAERIE_FOG:    RETURN( "現身術"   );
  case OBJ_GATE:          RETURN( "招鬼術"   );
  case OBJ_FIXITY:        RETURN( "定身術"   );
  case OBJ_CAST_ALL:      RETURN( "回復所有" );
  }
}

const char * bounty_type( int type )
{
  PUSH_FUNCTION( "bounty_type" );

  switch( type )
  {
  default:            RETURN( "不詳" );
  case BOUNTY_GOLD:   RETURN( "黃金" );
  case BOUNTY_FIRMAN: RETURN( "免死金牌" );
  }

  RETURN( "不詳" );
}

const char * bounty_unit( int type )
{
  PUSH_FUNCTION( "bounty_unit" );

  switch( type )
  {
  default:            RETURN( ""   );
  case BOUNTY_GOLD:   RETURN( "兩" );
  case BOUNTY_FIRMAN: RETURN( "張" );
  }

  RETURN( "" );
}
char * message_type( int type )
{
  PUSH_FUNCTION( "message_type" );

  switch( type )
  {
  default:                 RETURN( "(未知)"         );
  case ACT_WHEN_EAT:       RETURN( "吃的動作"       );
  case ACT_WHEN_WEAR:      RETURN( "穿的動作"       );
  case ACT_WHEN_DROP:      RETURN( "丟的動作"       );
  case ACT_WHEN_REMOVE:    RETURN( "移除的動作"     );
  case ACT_WHEN_SACRIFICE: RETURN( "奉獻的動作"     );
  case ACT_WHEN_GET:       RETURN( "撿起的動作"     );
  case ACT_WHEN_PUT:       RETURN( "放入的動作"     );
  case ACT_WHEN_GIVE:      RETURN( "贈與的動作"     );
  case ACT_WHEN_FILL:      RETURN( "裝填的動作"     );
  case ACT_WHEN_DRINK:     RETURN( "喝的動作"       );
  case ACT_WHEN_RECITE:    RETURN( "念誦的動作"     );
  case ACT_WHEN_BUY:       RETURN( "買的動作"       );
  case ACT_WHEN_SELL:      RETURN( "賣的動作"       );
  case ACT_WHEN_VALUE:     RETURN( "估價的動作"     );
  case ACT_WHEN_LOOK:      RETURN( "觀看的動作"     );
  case ACT_WHEN_COMPARE:   RETURN( "比較的動作"     );
  case ACT_WHEN_SEND:      RETURN( "郵寄的動作"     );
  case ACT_WHEN_OPEN:      RETURN( "打開的動作"     );
  case ACT_WHEN_CLOSE:     RETURN( "關上的動作"     );
  case ACT_WHEN_LOCK:      RETURN( "鎖上的動作"     );
  case ACT_WHEN_UNLOCK:    RETURN( "開鎖的動作"     );
  case ACT_WHEN_AUCTION:   RETURN( "拍賣的動作"     );
  case ACT_WHEN_BET:       RETURN( "喊價的動作"     );
  case ACT_WHEN_REPAIR:    RETURN( "修理的動作"     );
  case ACT_WHEN_DEPOSIT:   RETURN( "寄放的動作"     );
  case ACT_WHEN_WITHDRAW:  RETURN( "領回的動作"     );
  case ACT_WHEN_CACHET:    RETURN( "裝上魔石的動作" );
  case ACT_WHEN_SLUP:      RETURN( "卸下魔石的動作" );
  }
}

char * restrict_value( RESTRICT_DATA * pRestrict, CHAR_DATA * ch )
{
  CLASS_DATA * pClass;
  SKILL_DATA * pSkill;
  static char  buf[512];
  char         chinese[512];

  PUSH_FUNCTION( "restrict_value" );

  if ( !pRestrict || !ch ) RETURN( "(空結構)" );

  switch( pRestrict->type )
  {
  case RES_STR:
    chinese_number( pRestrict->value, chinese );
    sprintf( buf, "限制力量不能低於%s。", chinese );
    break;

  case RES_INT:
    chinese_number( pRestrict->value, chinese );
    sprintf( buf, "限制智力不能低於%s。", chinese );
    break;

  case RES_WIS:
    chinese_number( pRestrict->value, chinese );
    sprintf( buf, "限制學識不能低於%s。", chinese );
    break;

  case RES_DEX:
    chinese_number( pRestrict->value, chinese );
    sprintf( buf, "限制敏捷不能低於%s。", chinese );
    break;

  case RES_CON:
    chinese_number( pRestrict->value, chinese );
    sprintf( buf, "限制體格不能低於%s。", chinese );
    break;

  case RES_HP:
    chinese_number( pRestrict->value, chinese );
    sprintf( buf, "限制生命力不能低於%s。", chinese );
    break;

  case RES_MANA:
    chinese_number( pRestrict->value, chinese );
    sprintf( buf, "限制法力不能低於%s。", chinese );
    break;

  case RES_MOVE:
    chinese_number( pRestrict->value, chinese );
    sprintf( buf, "限制體力不能低於%s。", chinese );
    break;

  case RES_CLASS:
    str_cpy( buf, "限制職業限制為" );

    for ( pClass = class_list; pClass; pClass = pClass->next )
    {
      if ( pRestrict->value & pClass->mask )
      {
        str_cat( buf, " (" );
        str_cat( buf, pClass->cname );
        str_cat( buf, ")" );
      }
    }

    break;

  case RES_TRUST:

    chinese_number( pRestrict->value, chinese );
    sprintf( buf, "限制信任值不能低於%s。", chinese );
    break;

  case RES_SKILL:

    if ( !( pSkill = get_skill( pRestrict->value ) ) ) RETURN( "未知" );

    chinese_number( pRestrict->vicevalue, chinese );
    sprintf( buf, "限制技能%s熟練度不能低於為%s%s%s%s。"
      , pSkill->cname, adeptation_name( pRestrict->vicevalue )
      , SkillValue || IS_IMMORTAL( ch ) ? "「熟練度﹕" : ""
      , SkillValue || IS_IMMORTAL( ch ) ? chinese      : ""
      , SkillValue || IS_IMMORTAL( ch ) ? "」"         : "" );

    break;

  case RES_SEX:

    switch( pRestrict->value )
    {
    default:          send_to_buffer( "限制性別不詳。"   ); break;
    case SEX_NEUTRAL: send_to_buffer( "限制性別為中性。" ); break;
    case SEX_MALE:    send_to_buffer( "限制性別為男性。" ); break;
    case SEX_FEMALE:  send_to_buffer( "限制性別為女性。" ); break;
    }

    break;

  case RES_LEVEL:
    chinese_number( pRestrict->value, chinese );
    sprintf( buf, "等級不能低於%s。", chinese );
    break;

  case RES_NOSKILL:

    if ( !( pSkill = get_skill( pRestrict->value ) ) ) RETURN( "未知" );

    chinese_number( pRestrict->vicevalue, chinese );
    sprintf( buf, "限制相剋技能%s熟練不能高於%s%s%s%s。"
      , pSkill->cname, adeptation_name( pRestrict->vicevalue )
      , SkillValue || IS_IMMORTAL( ch ) ? "「熟練度﹕" : ""
      , SkillValue || IS_IMMORTAL( ch ) ? chinese      : ""
      , SkillValue || IS_IMMORTAL( ch ) ? "」"         : "" );

    break;

  case RES_ALIGN:

    chinese_number( pRestrict->value, chinese );
    sprintf( buf, "陣營必須%s%s。"
      , pRestrict->vicevalue ? "高於" : "低於", chinese );
    break;

  default:
    str_cpy( buf, "未知" );
    break;
  }

  RETURN( buf );
}

char * ship_status( SHIP_DATA * pShip )
{
  static char buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "ship_status" );

  if ( !pShip )
  {
    mudlog( LOG_DEBUG, "ship_status: 來源不正確." );
    RETURN( "未知" );
  }

  if ( pShip->sailing_tick > 0 )
  {
    sprintf( buf, "\e[1;32m航行\e[0m﹐%2d小時後到達", pShip->sailing_tick );
    RETURN( buf );
  }

  if ( pShip->waiting_tick > 0 )
  {
    sprintf( buf, "\e[1;33m登船\e[0m﹐%2d小時後出發", pShip->waiting_tick );
    RETURN( buf );
  }

  RETURN( "狀態未明" );
}

char * player_status( int vector )
{
  PUSH_FUNCTION( "player_status" );

  switch( vector )
  {
  case CON_PLAYING              : RETURN( "正在遊戲中" );
  case CON_GET_FQDN             : RETURN( "取得FQDN中" );
  case CON_GET_FINGER           : RETURN( "查玩家資料" );
  case CON_GET_NAME             : RETURN( "輸入姓名中" );
  case CON_CHECK_FILE           : RETURN( "檢查檔案中" );
  case CON_VERIFY               : RETURN( "確認檔案中" );
  case CON_GET_OLD_PASSWORD     : RETURN( "輸入舊密碼" );
  case CON_CONFIRM_NEW_NAME     : RETURN( "確認新名稱" );
  case CON_GET_NEW_PASSWORD     : RETURN( "輸入新密碼" );
  case CON_CONFIRM_NEW_PASSWORD : RETURN( "確認新密碼" );
  case CON_GET_SEX              : RETURN( "輸入性別中" );
  case CON_READ_MOTD            : RETURN( "進站畫面中" );
  case CON_GET_CNAME            : RETURN( "輸入中文名" );
  case CON_CHOOSE_SKILL         : RETURN( "選取技能中" );
  case CON_GET_EMAIL_ADDRESS    : RETURN( "設電子信箱" );
  case CON_CHOOSE_CLASS         : RETURN( "選擇職業中" );
  case CON_CHOOSE_MODE          : RETURN( "選輸出模式" );
  case CON_CHOOSE_ANGEL         : RETURN( "選擇守護神" );
  case CON_CHOOSE_HOME          : RETURN( "選擇出生地" );
  case CON_CONTRACT             : RETURN( "簽合約"     );
  }

  RETURN( "未知的類別" );
}

char * skill_type( int vector )
{
  PUSH_FUNCTION( "skill_type" );

  switch( vector )
  {
  case TAR_IGNORE:         RETURN( "未知"     );
  case TAR_CHAR_OFFENSIVE: RETURN( "攻擊類"   );
  case TAR_CHAR_DEFENSIVE: RETURN( "防禦類"   );
  case TAR_CHAR_SELF:      RETURN( "未知"     );
  case TAR_OBJ_INV:        RETURN( "物品類"   );
  case TAR_DODGE:          RETURN( "閃躲類"   );
  case TAR_STRING:         RETURN( "未知"     );
  case TAR_ARGUMENT:       RETURN( "需要字串" );
  case TAR_FLEE:           RETURN( "逃跑類"   );
  case TAR_MEDITATION:     RETURN( "冥想類"   );
  case TAR_OBJ_ROOM:       RETURN( "房間物品" );
  case TAR_MOUNT:          RETURN( "騎術"     );
  case TAR_NO_CAST:        RETURN( "雜項"     );
  }

  RETURN( "未知" );
}

char * skill_rating( int group )
{
  PUSH_FUNCTION( "skill_rating" );

  switch( group )
  {
  case RATING_SKILL:     RETURN( "技能"   );
  case RATING_WIND:      RETURN( "風系"   );
  case RATING_EARTH:     RETURN( "土系"   );
  case RATING_LIGHTNING: RETURN( "雷電系" );
  case RATING_POISON:    RETURN( "毒系"   );
  case RATING_FIRE:      RETURN( "火系"   );
  case RATING_WATER:     RETURN( "水系"   );
  case RATING_DARKNESS:  RETURN( "暗系"   );
  case RATING_LIGHT:     RETURN( "光系"   );
  case RATING_SAINT:     RETURN( "聖系"   );
  case RATING_EVIL:      RETURN( "邪系"   );
  case RATING_LOST:      RETURN( "法師系" );
  case RATING_CURE:      RETURN( "醫療系" );
  case RATING_SING:      RETURN( "吟唱系" );
  case RATING_FIGHT:     RETURN( "格鬥系" );
  case RATING_MURDER:    RETURN( "暗殺系" );
  case RATING_CREATE:    RETURN( "鑄造系" );
  case RATING_THIEF:     RETURN( "盜賊系" );
  case RATING_ALL:       RETURN( "全部"   );
  }

  RETURN( "未知" );
}

/* 幫派狀態 */
char * club_status( CLUB_DATA * pClub )
{
  int type;

  PUSH_FUNCTION( "club_status" );

  if ( pClub )
  {
    type = pClub->status;
    if ( type == CLUB_STATUS_COUNTERSIGN ) RETURN( "連署中"   );
    if ( type == CLUB_STATUS_UNIONIZE    ) RETURN( "已成立"   );
    if ( type == CLUB_STATUS_UNKNOW      ) RETURN( "狀態不明" );
  }

  RETURN( "未知" );
}

char * shop_type_name( int iType )
{
  PUSH_FUNCTION( "shop_type_name" );

  switch( iType )
  {
  case SHOP_STORE:     RETURN( "商店"   );
  case SHOP_SMITH:     RETURN( "鐵鋪"   );
  case SHOP_MERCENARY: RETURN( "傭兵所" );
  case SHOP_COPER:     RETURN( "馬店"   );
  default:             RETURN( "未知"   );
  }
}

char * item_type_name( OBJ_INDEX_DATA * obj )
{
  PUSH_FUNCTION( "item_type_name" );
  RETURN( item_kind_name( obj->item_type ) );
}

char * item_kind_name( int iType )
{
  PUSH_FUNCTION( "item_kind_name" );

  switch ( iType )
  {
  case ITEM_LIGHT:      RETURN( "照明類"     );
  case ITEM_SCROLL:     RETURN( "捲軸"       );
  case ITEM_WAND:       RETURN( "法杖"       );
  case ITEM_STAFF:      RETURN( "法杖"       );
  case ITEM_WEAPON:     RETURN( "武器"       );
  case ITEM_TREASURE:   RETURN( "寶物"       );
  case ITEM_ARMOR:      RETURN( "防具"       );
  case ITEM_BACKBAG:    RETURN( "背袋"       );
  case ITEM_POTION:     RETURN( "藥水"       );
  case ITEM_FURNITURE:  RETURN( "家具"       );
  case ITEM_TRASH:      RETURN( "垃圾"       );
  case ITEM_CONTAINER:  RETURN( "容器"       );
  case ITEM_DRINK_CON:  RETURN( "裝水容器"   );
  case ITEM_KEY:        RETURN( "鑰匙"       );
  case ITEM_FOOD:       RETURN( "食物"       );
  case ITEM_MONEY:      RETURN( "金錢"       );
  case ITEM_BOAT:       RETURN( "船"         );
  case ITEM_CORPSE_NPC: RETURN( "怪物的屍體" );
  case ITEM_CORPSE_PC:  RETURN( "玩家的屍體" );
  case ITEM_FOUNTAIN:   RETURN( "泉水"       );
  case ITEM_PILL:       RETURN( "藥丸"       );
  case ITEM_MAGICSTONE: RETURN( "魔石"       );
  case ITEM_SPIRITJADE: RETURN( "鬼玉"       );
  case ITEM_MYSTERY:    RETURN( "秘笈"       );
  case ITEM_LETTER:     RETURN( "書信"       );
  case ITEM_ORE:        RETURN( "礦物"       );
  case ITEM_GOLDMINE:   RETURN( "聚寶盆"     );
  case ITEM_SIGIL:      RETURN( "符咒"       );
  case ITEM_VELLUM:     RETURN( "羊皮紙"     );
  case ITEM_FIREWORK:   RETURN( "煙火"       );
  case ITEM_URN:        RETURN( "妖壺"       );
  case ITEM_ARROW:      RETURN( "箭"         );
  }

  mudlog( LOG_DEBUG , "Item_type_name: 未知的型態 %d.", iType );
  RETURN( "(未知)");
}

char * affect_loc_name( int location )
{
  PUSH_FUNCTION( "affect_loc_name" );

  switch ( location )
  {
  case APPLY_NONE:          RETURN( "無" );
  case APPLY_STR:           RETURN( "力量" );
  case APPLY_DEX:           RETURN( "敏捷" );
  case APPLY_INT:           RETURN( "智力" );
  case APPLY_WIS:           RETURN( "學識" );
  case APPLY_CON:           RETURN( "體格" );
  case APPLY_SEX:           RETURN( "性別" );
  case APPLY_CLASS:         RETURN( "職業" );
  case APPLY_LEVEL:         RETURN( "等級" );
  case APPLY_AGE:           RETURN( "年齡" );
  case APPLY_MANA:          RETURN( "法力" );
  case APPLY_HIT:           RETURN( "生命力" );
  case APPLY_MOVE:          RETURN( "體力" );
  case APPLY_GOLD:          RETURN( "金錢" );
  case APPLY_EXP:           RETURN( "經驗值" );
  case APPLY_AC:            RETURN( "防護等級" );
  case APPLY_HITROLL:       RETURN( "命中率" );
  case APPLY_DAMROLL:       RETURN( "傷害點數" );
  case APPLY_SAVING_PARA:   RETURN( "save vs paralysis" );
  case APPLY_SAVING_ROD:    RETURN( "save vs rod" );
  case APPLY_SAVING_PETRI:  RETURN( "save vs petrification" );
  case APPLY_SAVING_BREATH: RETURN( "save vs breath" );
  case APPLY_SAVING_SPELL:  RETURN( "法術庇祐" );
  }

  mudlog( LOG_DEBUG , "Affect_location_name: 未知地方 %d.", location );
  RETURN( "(未知)" );
}

char * skill_affect( int type )
{
  PUSH_FUNCTION( "skill_affect" );

  switch( type )
  {
  default:                RETURN( "沒有"     );
  case SLOT_BLINDNESS:    RETURN( "眼盲"     );
  case SLOT_CHARM_PERSON: RETURN( "迷惑"     );
  case SLOT_CURSE:        RETURN( "詛咒"     );
  case SLOT_INVIS:        RETURN( "隱形"     );
  case SLOT_MASS_INVIS:   RETURN( "全體隱形" );
  case SLOT_POISON:       RETURN( "中毒"     );
  case SLOT_SLEEP:        RETURN( "昏睡"     );
  case SLOT_SNEAK:        RETURN( "潛行"     );
  }
}

const char * exit_status( EXIT_DATA * pExit )
{
  static char buf[512];

  PUSH_FUNCTION( "exit_status" );

  buf[0] = '\x0';
  if ( pExit->closed ) str_cat( buf, " 關閉"   );
  if ( pExit->locked ) str_cat( buf, " 鎖著"   );
  if ( pExit->pick   ) str_cat( buf, " 可撬開" );
  if ( pExit->isdoor ) str_cat( buf, " 有門"   );

  RETURN( ( buf[0] != '\x0' )  ? buf+1 : "(沒有)" );
}

char * area_bit_name( AREA_DATA * pArea )
{
  static char buf[512];

  PUSH_FUNCTION( "area_bit_name" );

  buf[0] = '\x0';
  if ( pArea->pk      ) str_cat( buf , " ＰＫ"   );
  if ( pArea->newhand ) str_cat( buf , " 新手區" );
  if ( pArea->fight   ) str_cat( buf , " 狼煙區" );

  RETURN( buf[0] != '\x0' ? buf+1 : "(沒有)" );
}

char * sector_flag_name( SECTOR_DATA * pSector )
{
  static char buf[512];

  PUSH_FUNCTION( "sector_flag_name" );

  buf[0] = '\x0';
  if ( pSector->fly  ) str_cat( buf , " 飛行" );
  if ( pSector->swim ) str_cat( buf , " 游泳" );
  RETURN( buf[0] != '\x0' ? buf+1 : "(沒有)" );
}

/* 房間型態的中文名稱 */
char * room_bit_name( ROOM_INDEX_DATA * pRoom )
{
  static char buf[512];

  PUSH_FUNCTION( "room_bit_name" );

  buf[0] = '\x0';
  if ( pRoom->player        ) str_cat( buf, " 玩家自製" );
  if ( pRoom->Dark          ) str_cat( buf, " 黑暗"     );
  if ( pRoom->NoMob         ) str_cat( buf, " 沒有怪物" );
  if ( pRoom->Indoors       ) str_cat( buf, " 室內"     );
  if ( pRoom->Private       ) str_cat( buf, " 私人"     );
  if ( pRoom->Safe          ) str_cat( buf, " 安全"     );
  if ( pRoom->NoRecall      ) str_cat( buf, " 不能傳送" );
  if ( pRoom->ForeverLight  ) str_cat( buf, " 永明"     );
  if ( pRoom->DepositMoney  ) str_cat( buf, " 錢莊"     );
  if ( pRoom->StoreRoom     ) str_cat( buf, " 鏢局"     );
  if ( pRoom->Club          ) str_cat( buf, " 幫會"     );
  if ( pRoom->Memorize      ) str_cat( buf, " 記憶"     );
  if ( pRoom->Stock         ) str_cat( buf, " 票據所"   );
  if ( pRoom->NoQuit        ) str_cat( buf, " 不能離線" );
  if ( pRoom->Sail          ) str_cat( buf, " 船艙"     );
  if ( pRoom->Fane          ) str_cat( buf, " 寺廟"     );
  if ( pRoom->Killer        ) str_cat( buf, " 八仙飯店" );
  if ( pRoom->Standard      ) str_cat( buf, " 標準房間" );
  if ( pRoom->NoFight       ) str_cat( buf, " 不能戰鬥" );
  if ( pRoom->NoLink        ) str_cat( buf, " 不能連結" );
  if ( pRoom->Waterful      ) str_cat( buf, " 多水"     );
  if ( pRoom->Foodful       ) str_cat( buf, " 多食物"   );
  if ( pRoom->ManaCure      ) str_cat( buf, " 法力診療所"   );
  if ( pRoom->MoveCure      ) str_cat( buf, " 體力診療所"   );
  if ( pRoom->HitCure       ) str_cat( buf, " 生命力診療所" );
  if ( pRoom->PracRoom      ) str_cat( buf, " 練功\場"   );

  RETURN( ( buf[0] != '\x0' ) ? buf+1 : "(沒有)" );
}

char * attack_situs( int flag )
{
  static char   buf[512];
  SITUS_DATA  * pSitus;

  PUSH_FUNCTION( "attack_situs" );

  buf[0] = '\x0';

  for ( pSitus = situs_list; pSitus; pSitus = pSitus->next )
  {
    if ( flag & pSitus->attack )
    {
      str_cat( buf, " " );
      str_cat( buf, pSitus->situs_name );
    }
  }

  RETURN( ( buf[0] ) ? buf+1 : "隨機" );
}

/* 穿戴地方的名稱 */
char * wear_location_string( int type )
{
  SITUS_DATA * pSitus;

  PUSH_FUNCTION( "wear_location_string" );

  if ( type == WEAR_NONE ) RETURN( "尚未裝備" );

  for ( pSitus = situs_list; pSitus; pSitus = pSitus->next )
    if ( pSitus->location == type ) RETURN( pSitus->situs_name );

  RETURN( "未知" );
}

/* 穿戴旗標的名稱 */
char * wear_bit_name( int wear_flags )
{
  static char   buf[512];
  SITUS_DATA  * pSitus;

  PUSH_FUNCTION( "wear_bit_name" );

  buf[0] = '\x0';

  for ( pSitus = situs_list; pSitus; pSitus = pSitus->next )
  {
    if ( wear_flags & pSitus->wear )
    {
      str_cat( buf, " " );
      str_cat( buf, pSitus->situs_name );
    }
  }

  RETURN( ( buf[0] != '\x0' ) ? buf+1 : "(沒有)" );
}

char * xnames_bit_name( int type )
{
  PUSH_FUNCTION( "xnames_bit_name" );

  switch ( type )
  {
  case XNAMES_SYSTEM  : RETURN( "系統保留字"     );
  case XNAMES_CACONYM : RETURN( "不雅字"         );
  case XNAMES_CHAT    : RETURN( "閒聊保留字"     );
  case XNAMES_CNAME   : RETURN( "中文名稱保留字" );
  }

  RETURN( "未知" );
}

char * ban_bit_name( int type )
{
  PUSH_FUNCTION( "ban_bit_name" );

  switch ( type )
  {
  default         : RETURN( "未知"                   );
  case BAN_LOGIN  : RETURN( "禁止連線"               );
  case BAN_FQDN   : RETURN( "不做位址反查詢"         );
  case BAN_FINGER : RETURN( "禁止查詢這位址玩家資料" );
  }
}

/* 物品額外旗標的名稱 */
char * extra_bit_name( OBJ_DATA * pObj )
{
  static char buf[512];

  PUSH_FUNCTION( "extra_bit_name" );

  buf[0] = '\x0';

  if ( pObj->Glow         ) str_cat( buf, " 閃閃發光"   );
  if ( pObj->Hum          ) str_cat( buf, " 嗡嗡作響"   );
  if ( pObj->Dark         ) str_cat( buf, " 黑暗"       );
  if ( pObj->Evil         ) str_cat( buf, " 邪惡"       );
  if ( pObj->Invis        ) str_cat( buf, " 隱形"       );
  if ( pObj->Magic        ) str_cat( buf, " 魔法"       );
  if ( pObj->NoDrop       ) str_cat( buf, " 不可丟棄"   );
  if ( pObj->Bless        ) str_cat( buf, " 祝福"       );
  if ( pObj->AntiGood     ) str_cat( buf, " 抗善良陣營" );
  if ( pObj->AntiEvil     ) str_cat( buf, " 抗邪惡陣營" );
  if ( pObj->AntiNeutral  ) str_cat( buf, " 抗中立陣營" );
  if ( pObj->NoRemove     ) str_cat( buf, " 不可移除"   );
  if ( pObj->Inventory    ) str_cat( buf, " 商品"       );
  if ( pObj->Cancachet    ) str_cat( buf, " 可封印"     );
  if ( pObj->UserSet      ) str_cat( buf, " 自設數值"   );
  if ( pObj->NoSave       ) str_cat( buf, " 不可儲存"   );
  if ( pObj->Contraband   ) str_cat( buf, " 違禁品"     );
  if ( pObj->Loadable     ) str_cat( buf, " 可裝填"     );

  RETURN( ( buf[0] != '\x0' ) ? buf+1 : "(沒有)" );
}

/* 物品額外旗標的名稱 */
char * extra_bit_name_2( OBJ_INDEX_DATA * pObjIndex )
{
  static char buf[512];

  PUSH_FUNCTION( "extra_bit_name_2" );

  buf[0] = '\x0';

  if ( pObjIndex->Glow         ) str_cat( buf, " 閃閃發光"   );
  if ( pObjIndex->Hum          ) str_cat( buf, " 嗡嗡作響"   );
  if ( pObjIndex->Dark         ) str_cat( buf, " 黑暗"       );
  if ( pObjIndex->Evil         ) str_cat( buf, " 邪惡"       );
  if ( pObjIndex->Invis        ) str_cat( buf, " 隱形"       );
  if ( pObjIndex->Magic        ) str_cat( buf, " 魔法"       );
  if ( pObjIndex->NoDrop       ) str_cat( buf, " 不可丟棄"   );
  if ( pObjIndex->Bless        ) str_cat( buf, " 祝福"       );
  if ( pObjIndex->AntiGood     ) str_cat( buf, " 抗善良陣營" );
  if ( pObjIndex->AntiEvil     ) str_cat( buf, " 抗邪惡陣營" );
  if ( pObjIndex->AntiNeutral  ) str_cat( buf, " 抗中立陣營" );
  if ( pObjIndex->NoRemove     ) str_cat( buf, " 不可移除"   );
  if ( pObjIndex->Inventory    ) str_cat( buf, " 商品"       );
  if ( pObjIndex->Cancachet    ) str_cat( buf, " 可封印"     );
  if ( pObjIndex->UserSet      ) str_cat( buf, " 自設數值"   );
  if ( pObjIndex->NoSave       ) str_cat( buf, " 不可儲存"   );
  if ( pObjIndex->Contraband   ) str_cat( buf, " 違禁品"     );
  if ( pObjIndex->Loadable     ) str_cat( buf, " 可裝填"     );

  RETURN( ( buf[0] != '\x0' ) ? buf+1 : "(沒有)" );
}

/* 武器型態的中文名稱 */
char * weapon_type_name( int value )
{
  PUSH_FUNCTION( "weapon_type_name" );

  switch ( value )
  {
  default : RETURN( "\e[1;31m空手\e[0m" );
  case  0 : RETURN( "無設定" );
  case  1 : RETURN( "拳掌"   );
  case  2 : RETURN( "短兵"   );
  case  3 : RETURN( "長劍"   );
  case  4 : RETURN( "大刀"   );
  case  5 : RETURN( "斧"     );
  case  6 : RETURN( "鞭"     );
  case  7 : RETURN( "槍矛"   );
  case  8 : RETURN( "筆扇"   );
  case  9 : RETURN( "鎚"     );
  case 10 : RETURN( "棍棒"   );
  case 11 : RETURN( "弓"     );
  case 12 : RETURN( "氣"     );
  case 13 : RETURN( "步法"   );
  case 14 : RETURN( "逃跑"   );
  }
}

/* 容器的旗標名稱 */
char * container_bit_name( int value )
{
  static char buf[512];

  PUSH_FUNCTION( "container_bit_name" );

  buf[0] = '\x0';
  if ( value & CONT_CLOSEABLE ) str_cat( buf , " 可以開關" );
  if ( value & CONT_PICKPROOF ) str_cat( buf , " 無法撬開" );
  if ( value & CONT_CLOSED    ) str_cat( buf , " 關著"     );
  if ( value & CONT_LOCKED    ) str_cat( buf , " 鎖著"     );
  RETURN( ( buf[0] != '\x0' ) ? buf+1 : "(沒有)" );
}

char * turn_bit_name( int turn )
{
  static char buf[512];

  PUSH_FUNCTION( "turn_bit_name" );
  buf[0] = '\x0';

  if ( turn & PLR_REBIRTH  ) str_cat( buf, " 轉職者"   );
  if ( turn & PLR_TRAIN    ) str_cat( buf, " 升級屬性" );
  if ( turn & PLR_PRACTICE ) str_cat( buf, " 訓練技能" );
  if ( turn & PLR_ANSI     ) str_cat( buf, " 過濾色彩" );
  if ( turn & PLR_LOTTO    ) str_cat( buf, " 樂透快報" );
  RETURN( ( buf[0] ) ? buf+1 : "(沒有)" );
}


/* 神族代理權 */
char * wiz_bit_name( int wizflags )
{
  static char buf[512];

  PUSH_FUNCTION( "wiz_bit_name" );
  buf[0] = '\x0';

  if ( wizflags == 0 )          str_cat( buf, " 沒有神族代理權" );

  if ( wizflags & WIZ_SILENCE ) str_cat( buf, " 禁言"       );
  if ( wizflags & WIZ_NOTELL  ) str_cat( buf, " 不准告訴"   );
  if ( wizflags & WIZ_NOEMOTE ) str_cat( buf, " 不能用社交" );

  RETURN( ( buf[0] ) ? buf + 1 : "(沒有) " );
}

/* 人物屬性名稱 */
char * act_bit_name( int act )
{
  PUSH_FUNCTION( "act_bit_name" );

  clear_stack();

  if ( ( act & PLR_IS_NPC ) == 0 )
  {
    send_to_stack( "玩家" );
    if ( act & PLR_AUTOEXIT     ) send_to_stack( " 自動出口"     );
    if ( act & PLR_AUTOSAC      ) send_to_stack( " 自動奉獻"     );
    if ( act & PLR_BLANK        ) send_to_stack( " 有空白列"     );
    if ( act & PLR_BRIEF        ) send_to_stack( " 簡短描述"     );
    if ( act & PLR_COMBINE      ) send_to_stack( " 組合"         );
    if ( act & PLR_PROMPT       ) send_to_stack( " 提示列"       );
    if ( act & PLR_HOLYLIGHT    ) send_to_stack( " 聖光"         );
    if ( act & PLR_WIZINVIS     ) send_to_stack( " 神族隱形"     );
    if ( act & PLR_SILENCE      ) send_to_stack( " 消音"         );
    if ( act & PLR_NO_EMOTE     ) send_to_stack( " 不准emote"    );
    if ( act & PLR_NO_TELL      ) send_to_stack( " 不准tell"     );
    if ( act & PLR_LOG          ) send_to_stack( " 被log"        );
    if ( act & PLR_FREEZE       ) send_to_stack( " 被freeze"     );
    if ( act & PLR_THIEF        ) send_to_stack( " 盜賊"         );
    if ( act & PLR_KILLER       ) send_to_stack( " 殺人犯"       );
    if ( act & PLR_BOLTER       ) send_to_stack( " 叛幫者"       );
    if ( act & PLR_EXACT        ) send_to_stack( " 完整名稱"     );
    if ( act & PLR_MESSAGE      ) send_to_stack( " 攻擊敘述"     );
    if ( act & PLR_FLEE         ) send_to_stack( " 斷線自動逃跑" );
    if ( act & PLR_ANGEL        ) send_to_stack( " 守護神"       );
    if ( act & PLR_AUTOFOOD     ) send_to_stack( " 自動吃食物"   );
    if ( act & PLR_AUTODRINK    ) send_to_stack( " 自動喝水"     );
    if ( act & PLR_NODEATH      ) send_to_stack( " 不死"         );
  }

  else
  {
    send_to_stack( " 非玩家" );
    if ( act & ACT_SENTINEL     ) send_to_stack( " 不移動"         );
    if ( act & ACT_SCAVENGER    ) send_to_stack( " 撿屍體"         );
    if ( act & ACT_AGGRESSIVE   ) send_to_stack( " 自動攻擊"       );
    if ( act & ACT_STAY_AREA    ) send_to_stack( " 不會離開此區域" );
    if ( act & ACT_WIMPY        ) send_to_stack( " 自動逃跑"       );
    if ( act & ACT_PET          ) send_to_stack( " 寵物"           );
    if ( act & ACT_TRAIN        ) send_to_stack( " 訓練者"         );
    if ( act & ACT_PRACTICE     ) send_to_stack( " 練習者"         );
    if ( act & ACT_REBIRTH      ) send_to_stack( " 轉職者"         );
    if ( act & ACT_REBORN_FIGHT ) send_to_stack( " 重生戰鬥"       );
    if ( act & AUTOSET_VALUE    ) send_to_stack( " 自動設定數值"   );
    if ( act & ACT_NOREBORN     ) send_to_stack( " 不會重生"       );
    if ( act & ACT_NOKILL       ) send_to_stack( " 不能殺害"       );
    if ( act & ACT_NOSUMMON     ) send_to_stack( " 不能招喚"       );
    if ( act & ACT_ENROLL       ) send_to_stack( " 記恨"           );
  }

  RETURN( return_stack() );
}

/* MOB 程式的名稱 */
char * progtypes_bit_name( int progtypes )
{
  static char buf[512];

  PUSH_FUNCTION( "progtypes_bit_name" );

  buf[0] = '\x0';

  if ( progtypes & IN_FILE_PROG   ) str_cat( buf , " IN_FILE"   );
  if ( progtypes & ACT_PROG       ) str_cat( buf , " ACT"       );
  if ( progtypes & SPEECH_PROG    ) str_cat( buf , " SPEECH"    );
  if ( progtypes & RAND_PROG      ) str_cat( buf , " RAND"      );
  if ( progtypes & FIGHT_PROG     ) str_cat( buf , " FIGHT"     );
  if ( progtypes & DEATH_PROG     ) str_cat( buf , " DEATH"     );
  if ( progtypes & HITPRCNT_PROG  ) str_cat( buf , " HITPRCNT"  );
  if ( progtypes & ENTRY_PROG     ) str_cat( buf , " ENTRY"     );
  if ( progtypes & GREET_PROG     ) str_cat( buf , " GREET"     );
  if ( progtypes & ALL_GREET_PROG ) str_cat( buf , " ALL_GREET" );
  if ( progtypes & GIVE_PROG      ) str_cat( buf , " GIVE"      );
  if ( progtypes & BRIBE_PROG     ) str_cat( buf , " BRIBE"     );

  RETURN( ( buf[0] != '\x0' ) ? buf+1 : "(沒有)" );
}

char * get_obj_value_usage( int type, int * array )
{
  static char buf[ MAX_STRING_LENGTH ];

  PUSH_FUNCTION( "get_obj_value_usage" );

  switch ( type )
  {
  default :

    sprintf( buf,
      "V0  (用途未明)=%6d "
      "V1  (用途未明)=%6d "
      "V2  (用途未明)=%6d\n\r"
      "V3  (用途未明)=%6d "
      "V4  (用途未明)=%6d "
      "V5  (用途未明)=%6d\n\r"
      "V6  (用途未明)=%6d "
      "V7  (用途未明)=%6d "
      "V8  (用途未明)=%6d\n\r"
      "V9  (用途未明)=%6d "
      "V10 (用途未明)=%6d "
      "V11 (用途未明)=%6d\n\r"
      "V12 (用途未明)=%6d "
      "V13 (用途未明)=%6d "
      "V14 (用途未明)=%6d\n\r"
      ,*array      ,*(array+1)  ,*(array+2)  ,*(array+3)  ,*(array+4)
      ,*(array+5)  ,*(array+6)  ,*(array+7)  ,*(array+8)  ,*(array+9)
      ,*(array+10) ,*(array+11) ,*(array+12) ,*(array+13) ,*(array+14) );

    break;

  case ITEM_LIGHT:

    sprintf( buf ,
      "V0  (沒有使用)=%6d "
      "V1  (沒有使用)=%6d "
      "V2  (使用時間)=%6d\n\r"
      "V3  (沒有使用)=%6d "
      "V4  (沒有使用)=%6d "
      "V5  (沒有使用)=%6d\n\r"
      "V6  (沒有使用)=%6d "
      "V7  (沒有使用)=%6d "
      "V8  (沒有使用)=%6d\n\r"
      "V9  (沒有使用)=%6d "
      "V10 (沒有使用)=%6d "
      "V11 (沒有使用)=%6d\n\r"
      "V12 (沒有使用)=%6d "
      "V13 (沒有使用)=%6d "
      "V14 (沒有使用)=%6d\n\r"
      ,*array      ,*(array+1)  ,*(array+2)  ,*(array+3)  ,*(array+4)
      ,*(array+5)  ,*(array+6)  ,*(array+7)  ,*(array+8)  ,*(array+9)
      ,*(array+10) ,*(array+11) ,*(array+12) ,*(array+13) ,*(array+14) );

    break;

  case ITEM_MYSTERY:
    sprintf( buf ,
      "V0  (法術編號)=%6d "
      "V1  ( 熟練度 )=%6d "
      "V2  (沒有使用)=%6d\n\r"
      "V3  (沒有使用)=%6d "
      "V4  (沒有使用)=%6d "
      "V5  (沒有使用)=%6d\n\r"
      "V6  (沒有使用)=%6d "
      "V7  (沒有使用)=%6d "
      "V8  (沒有使用)=%6d\n\r"
      "V9  (沒有使用)=%6d "
      "V10 (沒有使用)=%6d "
      "V11 (沒有使用)=%6d\n\r"
      "V12 (沒有使用)=%6d "
      "V13 (沒有使用)=%6d "
      "V14 (沒有使用)=%6d\n\r"
      ,*array      , *(array+1) ,*(array+2)
      ,*(array+3)  , *(array+4) ,*(array+5)
      ,*(array+6)  ,*(array+7)  ,*(array+8)  ,*(array+9)
      ,*(array+10) ,*(array+11) ,*(array+12) ,*(array+13) ,*(array+14) );
    break;

  case ITEM_SCROLL:
  case ITEM_POTION:
  case ITEM_PILL:

    sprintf( buf ,
      "V0  (影響數值)=%6d "
      "V1  (法術名稱)=%8s "
      "V2  (法術編號)=%6d\n\r"
      "V3  (藥物劑量)=%6d "
      "V4  (沒有使用)=%8d "
      "V5  (沒有使用)=%6d\n\r"
      "V6  (沒有使用)=%6d "
      "V7  (沒有使用)=%8d "
      "V8  (沒有使用)=%6d\n\r"
      "V9  (沒有使用)=%6d "
      "V10 (沒有使用)=%8d "
      "V11 (沒有使用)=%6d\n\r"
      "V12 (沒有使用)=%6d "
      "V13 (沒有使用)=%8d "
      "V14 (沒有使用)=%6d\n\r"
      ,*array      , obj_cast_location( *(array+1) ) ,*(array+2)
      ,*(array+3)  , *(array+4) ,*(array+5)
      ,*(array+6)  ,*(array+7)  ,*(array+8)  ,*(array+9)
      ,*(array+10) ,*(array+11) ,*(array+12) ,*(array+13) ,*(array+14) );

    break;

  case ITEM_STAFF:
  case ITEM_WAND:

    sprintf( buf ,
      "V0  (沒有使用)=%6d "
      "V1  (沒有使用)=%6d "
      "V2  (沒有使用)=%6d\n\r"
      "V3  (沒有使用)=%6d "
      "V4  (加強分類)=%6d "
      "V5  (加強程度)=%6d\n\r"
      "V6  (目前法力)=%6d "
      "V7  (消耗法力)=%6d "
      "V8  (沒有使用)=%6d\n\r"
      "V9  (沒有使用)=%6d "
      "V10 (沒有使用)=%6d "
      "V11 (沒有使用)=%6d\n\r"
      "V12 (沒有使用)=%6d "
      "V13 (沒有使用)=%6d "
      "V14 (沒有使用)=%6d\n\r"
      ,*array      ,*(array+1)  ,*(array+2)  ,*(array+3)  ,*(array+4)
      ,*(array+5)  ,*(array+6)  ,*(array+7)  ,*(array+8)  ,*(array+9)
      ,*(array+10) ,*(array+11) ,*(array+12) ,*(array+13) ,*(array+14) );

    break;

  case ITEM_WEAPON:

    sprintf( buf ,
      "V0  (沒有使用)=%6d "
      "V1  (最小傷害)=%6d "
      "V2  (最大傷害)=%6d\n\r"
      "V3  (武器種類)= %s (%d)\n\r"
      "V4  (沒有使用)=%6d "
      "V5  (沒有使用)=%6d "
      "V6  (沒有使用)=%6d\n\r"
      "V7  (沒有使用)=%6d "
      "V8  (沒有使用)=%6d "
      "V9  (沒有使用)=%6d\n\r"
      "V10 (沒有使用)=%6d "
      "V11 (沒有使用)=%6d "
      "V12 (沒有使用)=%6d\n\r"
      "V13 (沒有使用)=%6d "
      "V14 (沒有使用)=%6d\n\r"
      ,*array      ,*(array+1)  ,*(array+2)
      , weapon_type_name( *(array+3 ) ) , *( array+3 ), *(array+4)
      ,*(array+5)  ,*(array+6)  ,*(array+7)  ,*(array+8)  ,*(array+9)
      ,*(array+10) ,*(array+11) ,*(array+12) ,*(array+13) ,*(array+14) );

    break;

  case ITEM_ARROW:

    sprintf( buf ,
      "V0  (最大傷害)=%6d "
      "V1  (最小傷害)=%6d "
      "V2  (沒有使用)=%6d\n\r"
      "V3  (沒有使用)= %s (%d)\n\r"
      "V4  (沒有使用)=%6d "
      "V5  (沒有使用)=%6d "
      "V6  (沒有使用)=%6d\n\r"
      "V7  (沒有使用)=%6d "
      "V8  (沒有使用)=%6d "
      "V9  (沒有使用)=%6d\n\r"
      "V10 (沒有使用)=%6d "
      "V11 (沒有使用)=%6d "
      "V12 (沒有使用)=%6d\n\r"
      "V13 (沒有使用)=%6d "
      "V14 (沒有使用)=%6d\n\r"
      ,*array      ,*(array+1)  ,*(array+2)
      , weapon_type_name( *(array+3 ) ) , *( array+3 ), *(array+4)
      ,*(array+5)  ,*(array+6)  ,*(array+7)  ,*(array+8)  ,*(array+9)
      ,*(array+10) ,*(array+11) ,*(array+12) ,*(array+13) ,*(array+14) );

    break;

  case ITEM_FURNITURE:
  case ITEM_TRASH:
  case ITEM_KEY:
  case ITEM_BOAT:
  case ITEM_CORPSE_PC:
  case ITEM_CORPSE_NPC:
  case ITEM_FOUNTAIN:
  case ITEM_TREASURE:
  case ITEM_SIGIL:
  case ITEM_VELLUM:

    sprintf( buf ,
      "V0  (沒有使用)=%6d "
      "V1  (沒有使用)=%6d "
      "V2  (沒有使用)=%6d\n\r"
      "V3  (沒有使用)=%6d "
      "V4  (沒有使用)=%6d "
      "V5  (沒有使用)=%6d\n\r"
      "V6  (沒有使用)=%6d "
      "V7  (沒有使用)=%6d "
      "V8  (沒有使用)=%6d\n\r"
      "V9  (沒有使用)=%6d "
      "V10 (沒有使用)=%6d "
      "V11 (沒有使用)=%6d\n\r"
      "V12 (沒有使用)=%6d "
      "V13 (沒有使用)=%6d "
      "V14 (沒有使用)=%6d\n\r"
      ,*array      ,*(array+1)  ,*(array+2)  ,*(array+3)  ,*(array+4)
      ,*(array+5)  ,*(array+6)  ,*(array+7)  ,*(array+8)  ,*(array+9)
      ,*(array+10) ,*(array+11) ,*(array+12) ,*(array+13) ,*(array+14) );

    break;

  case ITEM_FIREWORK:

    sprintf( buf ,
      "V0  (施放距離)=%6d "
      "V1  (失敗機率)=%6d "
      "V2  (沒有使用)=%6d\n\r"
      "V3  (沒有使用)=%6d "
      "V4  (沒有使用)=%6d "
      "V5  (沒有使用)=%6d\n\r"
      "V6  (沒有使用)=%6d "
      "V7  (沒有使用)=%6d "
      "V8  (沒有使用)=%6d\n\r"
      "V9  (沒有使用)=%6d "
      "V10 (沒有使用)=%6d "
      "V11 (沒有使用)=%6d\n\r"
      "V12 (沒有使用)=%6d "
      "V13 (沒有使用)=%6d "
      "V14 (沒有使用)=%6d\n\r"
      ,*array      ,*(array+1)  ,*(array+2)  ,*(array+3)  ,*(array+4)
      ,*(array+5)  ,*(array+6)  ,*(array+7)  ,*(array+8)  ,*(array+9)
      ,*(array+10) ,*(array+11) ,*(array+12) ,*(array+13) ,*(array+14) );

    break;

  case ITEM_URN:

    sprintf( buf ,
      "V0  (怪物號碼)=%6d "
      "V1  (生 命 力)=%6d "
      "V2  (法力數值)=%6d\n\r"
      "V3  (體力數值)=%6d "
      "V4  (沒有使用)=%6d "
      "V5  (沒有使用)=%6d\n\r"
      "V6  (沒有使用)=%6d "
      "V7  (沒有使用)=%6d "
      "V8  (沒有使用)=%6d\n\r"
      "V9  (沒有使用)=%6d "
      "V10 (沒有使用)=%6d "
      "V11 (沒有使用)=%6d\n\r"
      "V12 (沒有使用)=%6d "
      "V13 (沒有使用)=%6d "
      "V14 (沒有使用)=%6d\n\r"
      ,*array      ,*(array+1)  ,*(array+2)  ,*(array+3)  ,*(array+4)
      ,*(array+5)  ,*(array+6)  ,*(array+7)  ,*(array+8)  ,*(array+9)
      ,*(array+10) ,*(array+11) ,*(array+12) ,*(array+13) ,*(array+14) );

    break;

  case ITEM_LETTER:

    sprintf( buf ,
      "V0  (剩餘時間)=%6d "
      "V1  (沒有使用)=%6d "
      "V2  (沒有使用)=%6d\n\r"
      "V3  (沒有使用)=%6d "
      "V4  (沒有使用)=%6d "
      "V5  (沒有使用)=%6d\n\r"
      "V6  (沒有使用)=%6d "
      "V7  (沒有使用)=%6d "
      "V8  (沒有使用)=%6d\n\r"
      "V9  (沒有使用)=%6d "
      "V10 (沒有使用)=%6d "
      "V11 (沒有使用)=%6d\n\r"
      "V12 (沒有使用)=%6d "
      "V13 (沒有使用)=%6d "
      "V14 (沒有使用)=%6d\n\r"
      ,*array      ,*(array+1)  ,*(array+2)  ,*(array+3)  ,*(array+4)
      ,*(array+5)  ,*(array+6)  ,*(array+7)  ,*(array+8)  ,*(array+9)
      ,*(array+10) ,*(array+11) ,*(array+12) ,*(array+13) ,*(array+14) );

    break;

  case ITEM_ORE:

    sprintf( buf ,
      "V0  (出現物品)=%6d "
      "V1  (法術等級)=%6d "
      "V2  (成功\機會)=%6d\n\r"
      "V3  (打造價格)=%6d "
      "V4  (沒有使用)=%6d "
      "V5  (沒有使用)=%6d\n\r"
      "V6  (沒有使用)=%6d "
      "V7  (沒有使用)=%6d "
      "V8  (沒有使用)=%6d\n\r"
      "V9  (沒有使用)=%6d "
      "V10 (沒有使用)=%6d "
      "V11 (沒有使用)=%6d\n\r"
      "V12 (沒有使用)=%6d "
      "V13 (沒有使用)=%6d "
      "V14 (沒有使用)=%6d\n\r"
      ,*array      ,*(array+1)  ,*(array+2)  ,*(array+3)  ,*(array+4)
      ,*(array+5)  ,*(array+6)  ,*(array+7)  ,*(array+8)  ,*(array+9)
      ,*(array+10) ,*(array+11) ,*(array+12) ,*(array+13) ,*(array+14) );

    break;

  case ITEM_GOLDMINE:

    sprintf( buf ,
      "V0  (總共等級)=%6d "
      "V1  (累積等級)=%6d "
      "V2  (最小等級)=%6d\n\r"
      "V3  (沒有使用)=%6d "
      "V4  (沒有使用)=%6d "
      "V5  (物品號碼)=%6d\n\r"
      "V6  (物品號碼)=%6d "
      "V7  (物品號碼)=%6d "
      "V8  (物品號碼)=%6d\n\r"
      "V9  (物品號碼)=%6d "
      "V10 (物品號碼)=%6d "
      "V11 (物品號碼)=%6d\n\r"
      "V12 (物品號碼)=%6d "
      "V13 (物品號碼)=%6d "
      "V14 (物品號碼)=%6d\n\r"
      ,*array      ,*(array+1)  ,*(array+2)  ,*(array+3)  ,*(array+4)
      ,*(array+5)  ,*(array+6)  ,*(array+7)  ,*(array+8)  ,*(array+9)
      ,*(array+10) ,*(array+11) ,*(array+12) ,*(array+13) ,*(array+14) );

    break;

  case ITEM_ARMOR:
    sprintf( buf ,
      "V0  (防護等級)=%6d "
      "V1  (沒有使用)=%6d "
      "V2  (沒有使用)=%6d\n\r"
      "V3  (沒有使用)=%6d "
      "V4  (沒有使用)=%6d "
      "V5  (沒有使用)=%6d\n\r"
      "V6  (沒有使用)=%6d "
      "V7  (沒有使用)=%6d "
      "V8  (沒有使用)=%6d\n\r"
      "V9  (沒有使用)=%6d "
      "V10 (沒有使用)=%6d "
      "V11 (沒有使用)=%6d\n\r"
      "V12 (沒有使用)=%6d "
      "V13 (沒有使用)=%6d "
      "V14 (沒有使用)=%6d\n\r"
      ,*array      ,*(array+1)  ,*(array+2)  ,*(array+3)  ,*(array+4)
      ,*(array+5)  ,*(array+6)  ,*(array+7)  ,*(array+8)  ,*(array+9)
      ,*(array+10) ,*(array+11) ,*(array+12) ,*(array+13) ,*(array+14) );

    break;

  case ITEM_BACKBAG:
    sprintf( buf ,
      "V0  (最多數量)=%6d "
      "V1  (目前數量)=%6d "
      "V2  (武器號碼)=%6d\n\r"
      "V3  (沒有使用)=%6d "
      "V4  (沒有使用)=%6d "
      "V5  (沒有使用)=%6d\n\r"
      "V6  (沒有使用)=%6d "
      "V7  (沒有使用)=%6d "
      "V8  (沒有使用)=%6d\n\r"
      "V9  (沒有使用)=%6d "
      "V10 (沒有使用)=%6d "
      "V11 (沒有使用)=%6d\n\r"
      "V12 (沒有使用)=%6d "
      "V13 (沒有使用)=%6d "
      "V14 (沒有使用)=%6d\n\r"
      ,*array      ,*(array+1)  ,*(array+2)  ,*(array+3)  ,*(array+4)
      ,*(array+5)  ,*(array+6)  ,*(array+7)  ,*(array+8)  ,*(array+9)
      ,*(array+10) ,*(array+11) ,*(array+12) ,*(array+13) ,*(array+14) );

    break;

  case ITEM_CONTAINER:
    sprintf( buf ,
      "V0  (存放重量)=%6d "
      "V1  (裝置旗標)= %s (%d)\n\r"
      "V2  (鑰匙號碼)=%6d "
      "V3  (沒有使用)=%6d "
      "V4  (沒有使用)=%6d\n\r"
      "V5  (沒有使用)=%6d "
      "V6  (沒有使用)=%6d "
      "V7  (沒有使用)=%6d\n\r"
      "V8  (沒有使用)=%6d "
      "V9  (沒有使用)=%6d "
      "V10 (沒有使用)=%6d\n\r"
      "V11 (沒有使用)=%6d "
      "V12 (沒有使用)=%6d "
      "V13 (沒有使用)=%6d\n\r"
      "V14 (沒有使用)=%6d\n\r"
      ,*array      , container_bit_name( *(array+1) ), *(array+1)
      ,*(array+2)  ,*(array+3)  ,*(array+4)
      ,*(array+5)  ,*(array+6)  ,*(array+7)  ,*(array+8)  ,*(array+9)
      ,*(array+10) ,*(array+11) ,*(array+12) ,*(array+13) ,*(array+14) );

    break;

  case ITEM_DRINK_CON:
    sprintf( buf ,
       "V0  (最大容量)=%6d "
       "V1  (目前容量)=%6d "
       "V2  (液體號碼)=%6d\n\r"
       "V3  (是否有毒)= %s (%d)\n\r"
       "V4  (沒有使用)=%6d "
       "V5  (沒有使用)=%6d "
       "V6  (沒有使用)=%6d\n\r"
       "V7  (沒有使用)=%6d "
       "V8  (沒有使用)=%6d "
       "V9  (沒有使用)=%6d\n\r"
       "V10 (沒有使用)=%6d "
       "V11 (沒有使用)=%6d "
       "V12 (沒有使用)=%6d\n\r"
       "V13 (沒有使用)=%6d "
       "V14 (沒有使用)=%6d\n\r"
      ,*array      ,*(array+1)  ,*(array+2)
      , *(array+3) == 0 ? "無毒" : "有毒" ,*(array+3)  ,*(array+4)
      ,*(array+5)  ,*(array+6)  ,*(array+7)  ,*(array+8)  ,*(array+9)
      ,*(array+10) ,*(array+11) ,*(array+12) ,*(array+13) ,*(array+14) );

    break;

  case ITEM_FOOD:
    sprintf( buf ,
      "V0  (有效期限)=%6d "
      "V1  (現有次數)=%6d "
      "V2  (一口食量)=%6d\n\r"
      "V3  (是否有毒)=%s (%d)\n\r"
      "V4  (產生物品)=%6d "
      "V5  (沒有使用)=%6d "
      "V6  (沒有使用)=%6d\n\r"
      "V7  (沒有使用)=%6d "
      "V8  (沒有使用)=%6d "
      "V9  (沒有使用)=%6d\n\r"
      "V10 (沒有使用)=%6d "
      "V11 (沒有使用)=%6d "
      "V12 (沒有使用)=%6d\n\r"
      "V13 (沒有使用)=%6d "
      "V14 (沒有使用)=%6d\n\r"
      ,*array      ,*(array+1)  ,*(array+2)
      , *(array+3) == 0 ? "無毒" : "有毒" ,*(array+3)  ,*(array+4)
      ,*(array+5)  ,*(array+6)  ,*(array+7)  ,*(array+8)  ,*(array+9)
      ,*(array+10) ,*(array+11) ,*(array+12) ,*(array+13) ,*(array+14) );

    break;

  case ITEM_MONEY:
    sprintf( buf ,
      "V0  (錢的數量)=%6d "
      "V1  (沒有使用)=%6d "
      "V2  (沒有使用)=%6d\n\r"
      "V3  (沒有使用)=%6d "
      "V4  (沒有使用)=%6d "
      "V5  (沒有使用)=%6d\n\r"
      "V6  (沒有使用)=%6d "
      "V7  (沒有使用)=%6d "
      "V8  (沒有使用)=%6d\n\r"
      "V9  (沒有使用)=%6d "
      "V10 (沒有使用)=%6d "
      "V11 (沒有使用)=%6d\n\r"
      "V12 (沒有使用)=%6d "
      "V13 (沒有使用)=%6d "
      "V14 (沒有使用)=%6d\n\r"
      ,*array      ,*(array+1)  ,*(array+2)  ,*(array+3)  ,*(array+4)
      ,*(array+5)  ,*(array+6)  ,*(array+7)  ,*(array+8)  ,*(array+9)
      ,*(array+10) ,*(array+11) ,*(array+12) ,*(array+13) ,*(array+14) );

    break;

  case ITEM_MAGICSTONE:
    sprintf( buf ,
      "V0  (魔石種類)=%6d "
      "V1  (法術設定)=%6d "
      "V2  (經驗值  )=%6d\n\r"
      "V3  (魔石狀態)=%6d "
      "V4  (影響程度)=%6d "
      "V5  (魔石等級)=%6d\n\r"
      "V6  (沒有使用)=%6d "
      "V7  (沒有使用)=%6d "
      "V8  (沒有使用)=%6d\n\r"
      "V9  (沒有使用)=%6d "
      "V10 (沒有使用)=%6d "
      "V11 (沒有使用)=%6d\n\r"
      "V12 (沒有使用)=%6d "
      "V13 (沒有使用)=%6d "
      "V14 (沒有使用)=%6d\n\r"
      ,*array      ,*(array+1)  ,*(array+2)  ,*(array+3)  ,*(array+4)
      ,*(array+5)  ,*(array+6)  ,*(array+7)  ,*(array+8)  ,*(array+9)
      ,*(array+10) ,*(array+11) ,*(array+12) ,*(array+13) ,*(array+14) );

    break;

  /* 鬼玉 */
  case ITEM_SPIRITJADE:
    sprintf( buf ,
      "V0  (小鬼號碼)=%6d "
      "V1  (小鬼等級)=%6d "
      "V2  (小鬼經驗)=%6d\n\r"
      "V3  (技能號碼)=%6d "
      "V4  (技能等級)=%6d "
      "V5  (沒有使用)=%6d\n\r"
      "V6  (沒有使用)=%6d "
      "V7  (沒有使用)=%6d "
      "V8  (沒有使用)=%6d\n\r"
      "V9  (沒有使用)=%6d "
      "V10 (沒有使用)=%6d "
      "V11 (沒有使用)=%6d\n\r"
      "V12 (沒有使用)=%6d "
      "V13 (沒有使用)=%6d "
      "V14 (沒有使用)=%6d\n\r"
      ,*array      ,*(array+1)  ,*(array+2)  ,*(array+3)  ,*(array+4)
      ,*(array+5)  ,*(array+6)  ,*(array+7)  ,*(array+8)  ,*(array+9)
      ,*(array+10) ,*(array+11) ,*(array+12) ,*(array+13) ,*(array+14) );

    break;
  }
  RETURN( buf );
}

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

/*
鬼玉數值表示方法

V0  小鬼號碼
V1  小鬼等級
V2  小鬼經驗
V3  技能號碼
V4  技能等級
V5  小鬼改名
*/

#define SPIRIT_VNUM             0
#define SPIRIT_LEVEL            1
#define SPIRIT_EXP              2
#define SPIRIT_SKILL_VNUM       3
#define SPIRIT_SKILL_EXP        4
#define SPIRIT_RENAME           5

void            smash_spirit    args( ( CHAR_DATA * ) );
OBJ_DATA *      spirit_to_jade  args( ( CHAR_DATA * ) );

FUNCTION( do_call )
{
  char         arg[MAX_INPUT_LENGTH];
  char         buf[MAX_STRING_LENGTH];
  OBJ_DATA   * obj;
  CHAR_DATA  * spirit;
  SKILL_DATA * pSkill;
  int          loop;
  int          adept;
  int          sn;
  int          pFind;
  bool         Touch;

  PUSH_FUNCTION( "do_call" );

  if ( IS_NPC( ch ) || !ch->in_room ) RETURN_NULL();

  argument = one_argument( argument, arg );

  if ( !arg[0] )
  {
    if ( !( spirit = ch->spirit ) )
    {
      send_to_char( "你身邊沒有任何的小鬼喔﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !spirit->in_room || spirit->in_room != ch->in_room )
    {
      act( "你的$N跟你失去聯絡了﹗", ch, NULL, spirit, TO_CHAR );
      RETURN_NULL();
    }

    clear_buffer();
    send_to_buffer(
      "小鬼名稱﹕%s\n\r"
      "小鬼等級﹕%d\n\r"
      "小鬼生命﹕%-6d ﹕ %-6d\n\r"
      "小鬼法力﹕%-6d ﹕ %-6d\n\r"
      "小鬼體力﹕%-6d ﹕ %-6d\n\r"
      "經 驗 值﹕%d\n\r"
      "升級經驗﹕%d\n\r"
      , mob_name( ch, spirit )
      , spirit->level
      , spirit->hit,  get_curr_hit( spirit )
      , spirit->mana, get_curr_mana( spirit )
      , spirit->move, get_curr_move( spirit )
      , spirit->exp
      , kill_table[ URANGE( 1, spirit->level, MAX_LEVEL - 1 )].exp - spirit->exp );

    for ( loop = 0; loop < MAX_SKILL; loop++ )
    {
      if ( spirit->skill[loop] > 0 && ( pSkill = get_skill( loop ) ) )
      {
        send_to_buffer( "學習技能﹕%s\n\r技能熟練﹕%s"
          , pSkill->cname
          , adeptation_name( spirit->skill[loop] ) );

        if ( SkillValue || IS_IMMORTAL( ch ) )
          send_to_buffer( "(%3d%%)", spirit->skill[loop] );

        send_to_buffer( "\n\r" );
      }
    }

    print_buffer( ch );

    RETURN_NULL();
  }

  if ( !str_prefix( arg, "!back" ) )
  {
    if ( ch->position != POS_STANDING )
    {
      send_to_char( "你先放下手邊的工作吧﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !( spirit = ch->spirit ) )
    {
      send_to_char( "你根本沒有小鬼耶﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !spirit->in_room || spirit->in_room != ch->in_room )
    {
      act( "你的$N不在你的身邊噎﹗", ch, NULL, spirit, TO_CHAR );
      RETURN_NULL();
    }

    if ( spirit->position != POS_STANDING )
    {
      act( "你的$N還正在忙咧﹗", ch, NULL, spirit, TO_CHAR );
      RETURN_NULL();
    }

    if ( !spirit_to_jade( spirit ) )
    {
      act( "遭了﹐$N竟然收不回去了﹗", ch, NULL, spirit, TO_CHAR );
      RETURN_NULL();
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "!phone" ) )
  {
    if ( ch->spirit )
    {
      act( "你已經有$N了﹗", ch, NULL, ch->spirit, TO_CHAR );
      RETURN_NULL();
    }

    for ( pFind = 0, spirit = char_list; spirit; spirit = spirit->next )
    {
      if ( !verify_char( spirit )
        || !IS_NPC( spirit )
        || !spirit->pIndexData
        || spirit->jade.high != ch->serial.high
        || spirit->jade.low  != ch->serial.low
        || spirit->boss
        || spirit->position != POS_STANDING
        || spirit->mount_by
        || is_affected( spirit, SLOT_CHARM_PERSON )
        || !spirit->in_room
        || spirit->in_room != ch->in_room ) continue;

      pFind = 1;
      free_string( spirit->byname );
      sprintf( buf, "屬於%s的小鬼(%s)", mob_name( NULL, ch ), spirit->name );
      spirit->byname = str_dup( buf );

      free_string( spirit->description );
      sprintf( buf, "這是由%s召喚出來的%s﹐看來挺有幫助的﹗\n\r"
        , mob_name( NULL, ch )
        , mob_index_name( NULL, spirit->pIndexData ) );

      spirit->description = str_dup( buf );

      ch->spirit   = spirit;
      spirit->boss = ch;

      act( "$n呼喚$N的心靈深處﹐接著$N就由失魂的遠方回來了﹗"
        , ch, NULL, spirit, TO_ALL );

      RETURN_NULL();
    }

    if ( pFind == 0 )
    {
      send_to_char( "你無法呼喚到你的小鬼的心靈深處﹗\n\r", ch );
      RETURN_NULL();
    }
  }

  else if ( !str_prefix( arg, "!search" ) )
  {
    if ( ch->spirit )
    {
      act( "你的$N並沒有迷失﹗", ch, NULL, ch->spirit, TO_CHAR );
      RETURN_NULL();
    }

    send_to_char( "你由心中發出呼喊﹐希望迷失的小鬼能找到回家的路﹗\n\r", ch );

    for ( pFind = 0, spirit = char_list; spirit; spirit = spirit->next )
    {
      if ( !verify_char( spirit )
        || !IS_NPC( spirit )
        || spirit->jade.high != ch->serial.high
        || spirit->jade.low  != ch->serial.low
        || !spirit->in_room
        || spirit->boss ) continue;

      act( "你發現你的小鬼正在$t的$r遊盪﹐快去找它吧﹗"
        , ch
        , spirit->in_room->area->name
        , NULL
        , TO_CHAR );

      pFind++;
    }

    if ( pFind == 0 ) send_to_char( "你並沒有小鬼在這世界﹗\n\r", ch );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "!skill" ) )
  {
    if ( ch->position != POS_STANDING )
    {
      send_to_char( "你先放下手邊的工作吧﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !( spirit = ch->spirit ) )
    {
      send_to_char( "你根本沒有小鬼耶﹐你怎麼灌技能給他呢﹖\n\r", ch );
      RETURN_NULL();
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\x0' )
    {
      act( "你要灌什麼技能給你的$N呢﹖", ch, NULL, spirit, TO_CHAR );
      RETURN_NULL();
    }

    if ( !( pSkill = skill_isname( arg ) ) )
    {
      act( "對不起﹐沒有那種技能 $2$T$0 可以灌給他﹗", ch, NULL, arg, TO_CHAR );
      RETURN_NULL();
    }

    if ( !pSkill->enable || pSkill->slot == SLOT_FLEE )
    {
      act( "對不起﹐$w無法致能﹐所以不要教給$N﹗", ch, pSkill, spirit, TO_CHAR );

      RETURN_NULL();
    }

    if ( ch->skill[( sn = pSkill->slot )] <= 30 )
    {
      act( "$w你也沒有很熟﹐怎麼教給$N呢﹖", ch, pSkill, spirit, TO_CHAR );
      RETURN_NULL();
    }

    /* 消掉技能 */
    for ( loop = 0; loop < MAX_SKILL; loop++ )
      if ( loop != SLOT_FLEE ) spirit->skill[loop] = 0;

    /* 消掉致能 */
    if ( spirit->enable ) extract_enable( spirit );

    spirit->skill[sn]  = 10;
    ch->skill[sn]     -= 10;

    set_enable( spirit, pSkill );

    act( "$n已經將部份$w的功\力灌給$N﹗", ch, pSkill, spirit, TO_ALL );

    if ( IS_IMMORTAL( ch ) )
      mudlog( LOG_WIZARD, "%s 灌輸小鬼技能 %s.", ch->name, pSkill->name );

    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "!fill" ) )
  {
    if ( !( spirit = ch->spirit ) )
    {
      send_to_char( "你根本沒有小鬼耶﹐你怎麼飼養他呢﹖\n\r", ch );
      RETURN_NULL();
    }

    Touch = FALSE;

    /* 注入生命力 */
    if ( ( adept = UMIN( spirit->max_hit - spirit->hit, ch->hit - 1 ) ) > 0 )
    {
      act( "你把$i點的生命力灌入$N中。", ch, &adept, spirit, TO_CHAR );

      spirit->hit += adept;
      ch->hit     -= adept;
      Touch        = TRUE;
    }

    /* 注入法力 */
    if ( ( adept = UMIN( spirit->max_mana - spirit->mana, ch->mana - 1 ) ) > 0 )
    {
      act( "你把$i點的法力灌入$N中。", ch, &adept, spirit, TO_CHAR );

      spirit->mana += adept;
      ch->mana     -= adept;
      Touch         = TRUE;
    }

    /* 注入體力 */
    if ( ( adept = UMIN( spirit->max_move - spirit->move, ch->move - 1 ) ) > 0 )
    {
      act( "你把$i點的體力灌入$N中。", ch, &adept, spirit, TO_CHAR );

      spirit->move += adept;
      ch->move     -= adept;
      Touch         = TRUE;
    }

    if ( !Touch ) act( "你什麼也沒有餵給你的$N﹗", ch, NULL, spirit, TO_CHAR );
    RETURN_NULL();
  }

  else
  {
    if ( ch->position != POS_STANDING )
    {
      send_to_char( "你先放下手邊的工作吧﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( room_is_private( ch->in_room ) || ch->in_room->NoMob )
    {
      act( "$r陽氣太重了﹐小鬼是不敢出來的﹗", ch, NULL, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( is_platform( ch->in_room ) || ch->in_room->Sail )
    {
      act( "對不起﹐你不能在$r召喚出你的小鬼﹗", ch, NULL, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( !( obj = get_obj_carry( ch, arg ) ) )
    {
      act( "你身上根本沒有這顆鬼玉 $2$T$0 耶﹗", ch, NULL, arg, TO_CHAR );
      RETURN_NULL();
    }

    if ( obj->item_type != ITEM_SPIRITJADE )
    {
      act( "$p根本不是鬼玉﹐你是不是被騙了﹖", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch->spirit )
    {
      send_to_char( "你沒有辦法召喚出更多的小鬼出來了﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( obj->value[SPIRIT_LEVEL] > ch->level )
    {
      send_to_char( "看來你是沒有辦法控制這個小鬼﹐他的法力太強了﹗\n\r", ch );
      RETURN_NULL();
    }

    for ( spirit = char_list; spirit; spirit = spirit->next )
    {
      if ( verify_char( spirit )
        && IS_NPC( spirit )
        && spirit->jade.high == ch->serial.high
        && spirit->jade.low  == ch->serial.low )
      {
        spirit->jade.high  = 0;
        spirit->jade.low   = 0;
        spirit->spiritjade = 0;
      }
    }

    if ( !( spirit = jade_to_spirit( ch, obj ) ) )
    {
      act( "你這顆$p看來根本沒有辦法召喚出小鬼﹐丟了算了﹗"
        , ch, obj, NULL, TO_CHAR );

      RETURN_NULL();
    }
  }

  RETURN_NULL();
}

OBJ_DATA * spirit_to_jade(  CHAR_DATA * spirit )
{
  OBJ_INDEX_DATA * pIndexData;
  OBJ_DATA       * obj;
  CHAR_DATA      * boss;
  int              sn;
  int              loop;

  PUSH_FUNCTION( "spirit_to_jade" );

  if ( !spirit
    || !spirit->pIndexData
    || !( boss = spirit->boss )
    || !boss->in_room )
  {
    mudlog( LOG_DEBUG, "spirit_to_jade: 來源錯誤." );
    RETURN( NULL );
  }

  if ( ( sn = spirit->spiritjade ) <= 0 )
  {
    mudlog( LOG_DEBUG, "spirit_to_jade: 來源無法變為鬼玉." );
    RETURN( NULL );
  }

  if ( !( pIndexData = get_obj_index( sn ) )
    || pIndexData->item_type != ITEM_SPIRITJADE )
  {
    mudlog( LOG_DEBUG, "spirit_to_jade: 鬼玉號碼 %d 錯誤.", sn );
    RETURN( NULL );
  }

  if ( boss->in_room != spirit->in_room )
  {
    mudlog( LOG_DEBUG, "spirit_to_jade: 兩者地方不同." );
    RETURN( NULL );
  }

  /* 製造出鬼玉 */
  obj = create_object( pIndexData, UMIN( LEVEL_IMMORTAL -1, spirit->level ) );

  /* 清除數值 */
  for ( loop = 0; loop < MAX_OBJECT_VALUE; loop++ ) obj->value[loop] = 0;

  /* 填回數值 */
  obj->value[SPIRIT_VNUM]  = spirit->pIndexData->vnum;
  obj->value[SPIRIT_LEVEL] = UMAX( 1, UMIN( LEVEL_IMMORTAL -1, spirit->level ) );
  obj->value[SPIRIT_EXP]   = UMAX( 0, spirit->exp );

  /* 填回技能數值, 小鬼只會一種技能 */
  for ( loop = 0; loop < MAX_SKILL; loop++ )
  {
    if ( spirit->skill[loop] > 0 && loop != SLOT_FLEE )
    {
      obj->value[SPIRIT_SKILL_VNUM] = loop;
      obj->value[SPIRIT_SKILL_EXP]  = UMAX( 1, UMIN( 100, spirit->skill[loop] ) );
      break;
    }
  }

  /* 看看是否能拿, 或是能夠拿的動 */
  if ( obj->Takeable
    && boss->carry_number + 1 <= can_carry_n( boss )
    && get_carry_weight( boss ) + get_obj_weight( obj ) <= can_carry_w( boss ) )
  {
    obj_to_char( obj, boss );

    act( "突然﹐只見你的$N冒出一陣青煙﹐等你回神時﹐你身上就多了$p﹗這﹖"
      , boss, obj, spirit, TO_CHAR );

    act( "突然﹐$n的$N冒出一陣青煙﹐等你回神時﹐哪還有$N的身影﹗這﹖"
      , boss, NULL, spirit, TO_ROOM );
  }

  else
  {
    obj_to_room( obj, boss->in_room );

    act( "突然﹐只見你的$N冒出一陣青煙﹐等你回神時﹐地上就多出$p﹗這﹖"
      , boss, obj, spirit, TO_CHAR );

    act( "突然﹐$n的$N冒出一陣青煙﹐等你回神時﹐地上就多出$p﹗這﹖"
      , boss, obj, spirit, TO_ROOM );
  }

  /* 消掉精靈 */
  spirit_from_char( spirit );
  extract_char( spirit, TRUE );

  RETURN( obj );
}

CHAR_DATA * jade_to_spirit( CHAR_DATA * ch , OBJ_DATA * obj )
{
  CHAR_DATA      * spirit;
  SKILL_DATA     * pSkill;
  MOB_INDEX_DATA * pMobIndex;
  char             buf[MAX_STRING_LENGTH];
  int              loop;
  int              level;

  PUSH_FUNCTION( "jade_to_spirit" );

  if ( !ch || !obj )
  {
    mudlog( LOG_DEBUG, "jade_to_spirit: 來源缺乏." );
    RETURN( NULL );
  }

  if ( IS_NPC( ch ) || obj->item_type != ITEM_SPIRITJADE || !ch->in_room )
  {
    mudlog( LOG_DEBUG, "jade_to_spirit: 來源不正確." );
    RETURN( NULL );
  }

  if ( ch->serial.high == 0 || ch->serial.low == 0 )
  {
    mudlog( LOG_DEBUG, "jade_to_spirit: 主人缺乏序號." );
    RETURN( NULL );
  }

  if ( !( pMobIndex = get_mob_index( obj->value[SPIRIT_VNUM] ) ) )
  {
    mudlog( LOG_DEBUG, "jade_to_spirit: 物品%d小鬼號碼錯誤."
      , obj->pIndexData->vnum );
    RETURN( NULL );
  }

  if ( ( level = obj->value[SPIRIT_LEVEL] ) <= 0
    || level >= LEVEL_IMMORTAL
    || obj->value[SPIRIT_EXP]        < 0
    || obj->value[SPIRIT_SKILL_VNUM] < 0
    || obj->value[SPIRIT_SKILL_VNUM] >= MAX_SKILL
    || obj->value[SPIRIT_SKILL_EXP]  < 0
    || obj->value[SPIRIT_SKILL_EXP]  > 100
    || obj->value[SPIRIT_RENAME]     < 0 )
    RETURN( NULL );

  /* 配置記憶體給他 */
  clear_char( spirit  = alloc_struct( STRUCT_CHAR_DATA ) );

  spirit->pIndexData  = pMobIndex;

  spirit->name        = str_dup( pMobIndex->name       );
  spirit->long_descr  = str_dup( pMobIndex->long_descr );

  sprintf( buf, "屬於%s的小鬼", mob_name( NULL, ch ) );
  spirit->byname = str_dup( buf );

  sprintf( buf, "這是由%s召喚出來的%s﹐看來挺有幫助的﹗\n\r"
    , mob_name( NULL, ch )
    , mob_index_name( NULL, pMobIndex ) );

  spirit->description = str_dup( buf );

  spirit->prompt      = str_dup( "<^hp1 ^mp1 ^mv1>" );
  spirit->email       = str_dup( "<none>" );
  spirit->spec_fun    = NULL;

  spirit->level       = level;
  spirit->act         = ACT_IS_NPC | ACT_SENTINEL | ACT_STAY_AREA;
  spirit->alignment   = ch->alignment;
  spirit->sex         = pMobIndex->sex;
  spirit->class       = pMobIndex->class;
  spirit->speak       = pMobIndex->speak;

  spirit->now_str     = level * LOAD_ATTRIB_PLUS;
  spirit->now_int     = level * LOAD_ATTRIB_PLUS;
  spirit->now_wis     = level * LOAD_ATTRIB_PLUS;
  spirit->now_dex     = level * LOAD_ATTRIB_PLUS;
  spirit->now_con     = level * LOAD_ATTRIB_PLUS;

  spirit->armor       = level * LOAD_ARMOR_PLUS;
  spirit->gold        = 0;
  spirit->hitroll     = 0;
  spirit->damroll     = level * LOAD_DAMROLL_PLUS;

  spirit->max_hit     = level * load_hit_plus;
  spirit->mod_hit     = 0;
  spirit->hit         = UMAX( 0, UMIN( spirit->max_hit, ch->hit - 1 ) );
  ch->hit            -= spirit->hit;

  spirit->max_mana    = level * load_mana_plus;
  spirit->mod_mana    = 0;
  spirit->mana        = UMAX( 0, UMIN( spirit->max_mana, ch->mana - 1 ) );
  ch->mana           -= spirit->mana;

  spirit->max_move    = level * load_move_plus;
  spirit->mod_move    = 0;
  spirit->move        = UMAX( 0, UMIN( spirit->max_move, ch->move - 1 ) );
  ch->move           -= spirit->move;

  spirit->enable      = NULL;
  spirit->tractable   = 0;
  spirit->spiritjade  = obj->pIndexData->vnum;

  /* 設定熟練度 */
  for ( loop = 0; loop < MAX_SKILL; loop++ ) spirit->skill[loop] = 0;

  spirit->skill[obj->value[SPIRIT_SKILL_VNUM]] = obj->value[SPIRIT_SKILL_EXP];
  spirit->skill[SLOT_FLEE] = UMAX( 1, spirit->level / 5 );

  /* 設定致能, 而且小鬼只會一種技能 */
  if ( ( pSkill = get_skill( obj->value[SPIRIT_SKILL_VNUM] ) )
    && pSkill->enable )
  {
    set_enable( spirit, pSkill );
  }

  set_enable( spirit, get_skill( SLOT_FLEE ) );

  spirit->jade.high = ch->serial.high;
  spirit->jade.low  = ch->serial.low;

  /* 設定他的經驗值 */
  spirit->exp = obj->value[SPIRIT_EXP];
  pMobIndex->count++;

  /* 修正連結 */
  ch->spirit   = spirit;
  spirit->boss = ch;

  char_to_room( spirit, ch->in_room );

  /* 送出訊息給房間內所有的人 */
  act( "$n從$p召喚出$N﹐這到底怎麼一回事呢﹖", ch, obj, spirit, TO_ALL );

  /* 送出訊息﹐並且把物品銷毀 */
  do_say( spirit, "主人﹐奉血的誓約﹐我來協助你了﹗" );
  act( "你身上的$p在同時也消失不見﹗", ch, obj, NULL, TO_CHAR );
  extract_obj( obj );

  RETURN( spirit );
}

void spirit_from_char( CHAR_DATA * spirit )
{
  CHAR_DATA      * boss;
  MOB_INDEX_DATA * pIndex;

  PUSH_FUNCTION( "spirit_from_char" );

  if ( !spirit
    || !( boss = spirit->boss )
    || !( pIndex = spirit->pIndexData ) )
  {
    mudlog( LOG_DEBUG, "spirit_from_char: 來源錯誤." );
    RETURN_NULL();
  }

  spirit->boss = NULL;
  boss->spirit = NULL;

  /* 修正名稱, 且再也召喚不回去了 */
  free_string( spirit->byname );
  spirit->byname = str_dup( spirit->pIndexData->short_descr );

  free_string( spirit->description );
  spirit->description = str_dup( spirit->pIndexData->description );

  RETURN_NULL();
}

void smash_spirit( CHAR_DATA * spirit )
{
  PUSH_FUNCTION( "smash_spirit" );

  if ( !spirit || !spirit->pIndexData )
  {
    mudlog( LOG_DEBUG, "smash_spirit: 缺乏來源." );
    RETURN_NULL();
  }

  spirit->boss       = NULL;
  spirit->spiritjade = 0;

  /* 修正名稱, 且再也召喚不回去了 */
  free_string( spirit->byname );
  spirit->byname = str_dup( spirit->pIndexData->short_descr );

  free_string( spirit->description );
  spirit->description = str_dup( spirit->pIndexData->description );

  RETURN_NULL();
}

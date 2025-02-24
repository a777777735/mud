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

CHAR_DATA * get_rebirth_char args( ( CHAR_DATA * , char * ) );

/* 轉職 */
FUNCTION( do_rebirth )
{
  char         arg[MAX_INPUT_LENGTH];
  CHAR_DATA  * mob;
  CLASS_DATA * pClass;
  AREA_DATA  * area;
  int          count;

  PUSH_FUNCTION( "do_rebirth" );

  /* 非玩家不能轉職 */
  if ( IS_NPC( ch ) ) RETURN_NULL();

  if ( !ch->class )
  {
    send_to_char( "你的職業未明﹐無法轉職。\n\r", ch );
    RETURN_NULL();
  }

  argument = one_argument( argument , arg );

  /* 輸入字串為空字串 */
  if ( !arg[0] )
  {
    send_to_char( "你如果對轉職不瞭解﹐請查詢 rebirth。\n\r" , ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg, "!list" ) )
  {
    clear_buffer();
    send_to_buffer( "\e[1;33;44m順序 區 域名 稱 同區 職業名 中文名稱"
      "                                            \e[0m\n\r" );

    for ( count = 0, mob = char_list; mob; mob = mob->next )
    {
      if ( verify_char( mob )
        && IS_NPC( mob )
        && IS_SET( mob->act,  ACT_REBIRTH ) )
      {
        area = mob->in_room && mob->in_room->area ? mob->in_room->area : NULL;
        send_to_buffer( "%3d. %10s  %2s  %6s %-s\n\r"
          , ++count
          , area ? area->name : "從缺"
          , YESNO( area && ch->in_room && ch->in_room->area == area )
          , class_name( mob->class, FALSE )
          , mob_name( ch, mob ) );

        if ( buffer_full() ) break;
      }
    }

    if ( count == 0 ) send_to_char( "沒有可以轉職的人﹗\n\r", ch );
    else print_buffer( ch );

    RETURN_NULL();
  }

  /* IMM 不能轉職 */
  if ( IS_IMMORTAL( ch ) )
  {
    send_to_char( "你的職業是系統管理員﹐轉什麼職啊。\n\r" , ch );
    RETURN_NULL();
  }

  /* 找到同一個想要轉成職業的人 */
  if ( !( mob = get_rebirth_char( ch , arg ) ) )
  {
    act( "這裡沒有轉職者 $T﹐還是向別人打聽看看吧。" , ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  /* 是否願意幫別人轉職, 非玩家部份 */
  if ( (  IS_NPC( mob ) && !IS_SET( mob->act,  ACT_REBIRTH ) )
    || ( !IS_NPC( mob ) && !IS_SET( mob->turn, PLR_REBIRTH ) ) )
  {
    act( "對不起﹐$N無法幫你轉職﹗", ch, NULL, mob, TO_CHAR );
    RETURN_NULL();
  }

  /* 不能重複轉職 */
  if ( ch->level >= ch->class->low_rebirth
    && ch->level <= ch->class->high_rebirth )
  {
    send_to_char( "對不起﹐你不能重複轉職。", ch );
    RETURN_NULL();
  }

  if ( !( pClass = mob->class ) )
  {
    send_to_char( "系統職業有問題。" , ch );
    mudlog( LOG_DEBUG , "do_rebirth: 怪物 %d 職業有問題"
      , mob->pIndexData ? mob->pIndexData->vnum : -1 );
    RETURN_NULL();
  }

  /* 等級尚未達到 */
  if ( ch->level < pClass->low_rebirth || ch->level > pClass->high_rebirth )
  {
    act( "你不適合現在轉任$T。", ch, NULL, class_name( pClass, TRUE ), TO_CHAR );
    RETURN_NULL();
  }

  /* 看看夠不夠轉職金 */
  if ( ch->gold < pClass->rebirth_gold )
  {
    send_to_char( "抱歉﹐你身上的錢不太夠拿來轉職耶。" , ch );
    RETURN_NULL();
  }

  /* 某些職業不能轉職得到 */
  if ( pClass->associate != ch->class->vnum )
  {
    act( "抱歉﹐你不能這樣轉職成$T。" , ch, NULL, class_name( pClass, TRUE ), TO_CHAR );
    RETURN_NULL();
  }

  /* 等級已經達到轉職標準 */
  ch->class = mob->class;
  gold_from_char( ch, pClass->rebirth_gold );
  set_title( ch, pClass->title );

  act( "恭禧你﹐你已經轉職成$t這項職業了。你付了$I兩銀子。"
    , ch, class_name( pClass, TRUE ) , &pClass->rebirth_gold, TO_CHAR );

  RETURN_NULL();
}

/* 查詢職業 */
FUNCTION( do_vocation )
{
  int           count;
  extern char * const attr_name[];
  char          arg[ MAX_INPUT_LENGTH ];
  CLASS_DATA *  pClass;
  CLASS_DATA *  pAssociate;

  PUSH_FUNCTION( "do_vocation" );

  if IS_NPC( ch ) RETURN_NULL();

  one_argument( argument , arg );

  /* 沒有參數則列印所有的職業簡表 */
  if ( !arg[0] )
  {
    for ( pClass = class_list; pClass; pClass = pClass->next )
    {
      send_to_buffer( "(%2d) %6s %20s %10d\n\r"
        , pClass->vnum, pClass->cname, pClass->name, pClass->mask );

      if ( buffer_full() ) break;
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  /* 先找出看看是否符合職業的選項 */
  if ( ( pClass = is_class( arg, TRUE ) ) )
  {
    clear_buffer();
    send_to_buffer( "職業號碼﹕%d\n\r"
                    "英文名稱﹕%s\n\r"
                    "中文名稱﹕%s\n\r"
                    "預設抬頭﹕%s\n\r"
                    "轉職等級下限﹕%d\n\r"
                    "轉職等級上限﹕%d\n\r"
                    "轉職所需金錢﹕%d\n\r"
                    "轉職警告等級﹕%d\n\r"
                    "可學技能數目﹕%d\n\r"
                    "限制點數    ﹕%d\n\r"
                    "限制倍數    ﹕%d\n\r"
                    "限制敘述    ﹕%s\n\r"
                    "能否得到法力﹕%s\n\r"
                    "預設職業﹕    %s\n\r"
                    "強制轉職職業﹕%s\n\r"
                    "是否可以選擇﹕%s\n\r"
      , pClass->vnum
      , pClass->name
      , pClass->cname
      , pClass->title
      , pClass->low_rebirth
      , pClass->high_rebirth
      , pClass->rebirth_gold
      , pClass->warn
      , pClass->nskill
      , pClass->limit
      , pClass->multiplier
      , pClass->msg_limit ? pClass->msg_limit : "(從缺)"
      , YESNO( pClass->fMana    )
      , YESNO( pClass->rudiment )
      , YESNO( pClass->force    )
      , YESNO( pClass->select   ) );

      if ( ( pAssociate = class_lookup( pClass->associate ) ) )
        send_to_buffer( "由什麼職業轉職而來﹕%s\n\r"
          , pAssociate->cname );

      for ( count = 0; count < MAX_ATTR; count++ )
      {
        send_to_buffer( "%2d.%s屬性的上限﹕%2d 屬性的因數﹕%3d "
          "預設上限﹕%2d 預設下限﹕%2d 英雄加成﹕%2d\n\r"
          , count + 1
          , attr_name[count]
          , pClass->attr[count]
          , pClass->factor[count]
          , pClass->max_default[count]
          , pClass->min_default[count]
          , pClass->hero[count] );
      }
      print_buffer( ch );
      RETURN_NULL();
  }

  send_to_char( "沒有找到你要查詢的職業資料﹐請查詢 vocation。\n\r", ch );
  RETURN_NULL();
}

CHAR_DATA * get_rebirth_char( CHAR_DATA * ch , char * arg )
{
  CHAR_DATA * mob;

  PUSH_FUNCTION( "get_rebirth_char" );

  if ( !( mob = get_char_room( ch , arg ) ) ) RETURN( NULL );

  /* 看看是不是系統管理員, 是的話都可以幫別人轉 */
  if ( IS_IMMORTAL( mob ) ) RETURN( mob );

  RETURN( mob );
}

char * class_name( CLASS_DATA * pClass, bool bFull )
{
  static char buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "class_name" );

  if ( !pClass ) RETURN( "不  詳" );

  if ( bFull ) sprintf( buf, "%s(%s)", pClass->cname, pClass->name );
  else         str_cpy( buf, pClass->cname );

  RETURN( buf );
}

CLASS_DATA * class_lookup( int vnum )
{
  CLASS_DATA * pClass;

  PUSH_FUNCTION( "class_lookup" );

  for ( pClass = class_list; pClass; pClass = pClass->next )
    if ( pClass->vnum == vnum ) RETURN( pClass );

  RETURN( NULL );
}

CLASS_DATA * is_class( char * name, bool Exact )
{
  CLASS_DATA * pClass;

  PUSH_FUNCTION( "is_class" );

  if ( !name || *name == '\x0' ) RETURN( NULL );

  for ( pClass = class_list; pClass; pClass = pClass->next )
  {
    if ( Exact )
    {
      if ( !str_cmp( name, pClass->name ) ) break;
    }
    else
    {
      if ( !str_prefix( name, pClass->name ) ) break;
    }
  }

  RETURN( pClass );
}

/* 是否為同一個職業 */
bool is_same_class( CHAR_DATA * ach, CHAR_DATA * bch )
{
  PUSH_FUNCTION( "is_same_class" );

  if ( !ach || !bch || !ach->class || !bch->class || ach->class != bch->class )
    RETURN( FALSE );

  RETURN( TRUE );
}

int get_skill_class_level( SKILL_DATA * pSkill, CLASS_DATA * pClass )
{
  LIMIT_DATA * pLimit;

  PUSH_FUNCTION( "get_skill_class_level" );

  for ( pLimit = pSkill->limit; pLimit; pLimit = pLimit->next )
    if ( pLimit->class == pClass ) RETURN( pLimit->level );

  RETURN( MAX_LEVEL + 100 );
}

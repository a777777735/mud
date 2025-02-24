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
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include "merc.h"

void    system_cleanup  args( ( void ) );

IMMLIST_DATA * imm_lookup( const char * name )
{
  IMMLIST_DATA * pImmlist;

  PUSH_FUNCTION( "imm_lookup" );

  if ( !name || !*name ) RETURN( NULL );

  for ( pImmlist = imm_list; pImmlist; pImmlist = pImmlist->next )
    if ( !str_cmp( pImmlist->name, name ) ) RETURN( pImmlist );

  RETURN( NULL );
}

FUNCTION( do_data )
{
  CHAR_DATA  * man;
  char         arg[ MAX_STRING_LENGTH ];
  char         buf1[MAX_STRING_LENGTH];
  char         buf2[MAX_STRING_LENGTH];
  char         buf3[MAX_STRING_LENGTH];
  int          aNum;
  int          bNum;
  int          loop;
  int          count;
  int          total;

  PUSH_FUNCTION( "do_data" );

  argument = one_argument( argument , arg );

  if ( !arg[0] )
  {
    send_to_char( "請查詢 data 的正確使用方法﹐謝謝。\n\r" , ch );
    RETURN_NULL();
  }

  clear_buffer();

  if ( !str_prefix( arg , "money" ) || !str_prefix( arg , "gold" ) )
  {
    send_to_buffer( "%-15s %13s %13s %13s %13s\n\r"
      , "玩家姓名" , "身上銀兩" , "錢莊銀兩", "股票資產", "總資產額" );

    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) || !man->pcdata ) continue;

      count = man->gold + man->bank + stock_value( man );
      total = personal_asset( man );

      send_to_buffer( "%-15s %13s %13s %13s %s%13s\e[0m\n\r"
        , man->name
        , numberize( man->gold, NULL )
        , numberize( man->bank, buf1 )
        , numberize( stock_value( man ), buf2 )
        , count != total ? "\e[1;31m(溢位)" : ""
        , numberize( total, buf3 ) );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg, "age" ) )
  {
    send_to_buffer( "%-20s %6s %6s\n\r", "玩家姓名", "等級", "年齡" );

    /* 取得有效人物 */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) ) continue;

      send_to_buffer( "%-20s %6d %6d\n\r"
        , man->name, man->level, get_age( man ) );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg, "Imprison" ) )
  {
    send_to_buffer( "%-20s %6s %8s\n\r", "玩家姓名" , "等級", "被關幾次" );

    /* 取得有效人物 */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || !man->pcdata ) continue;

      send_to_buffer( "%-20s %6d %8d\n\r"
        , man->name
        , man->level
        , man->pcdata->imprison );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg , "attribute" ) )
  {
    send_to_buffer( "%-20s %6s %6s %6s %6s %6s %6s\n\r"
      , "玩家姓名" , "等級", "力量" , "智力" , "學識" , "敏捷" , "體格" );

    /* 取得有效人物 */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) ) continue;

      send_to_buffer( "%-20s %6d %6d %6d %6d %6d %6d\n\r"
        , man->name
        , man->level
        , get_curr_str( man )
        , get_curr_int( man )
        , get_curr_wis( man )
        , get_curr_dex( man )
        , get_curr_con( man ) );
    }
    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg , "basic" ) )
  {
    send_to_buffer( "%-20s %8s %8s %8s %8s\n\r"
      , "玩家姓名" , "等級", "生命力" , "法力" , "移動力" );

    /* 取得有效人物 */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) ) continue;

      send_to_buffer( "%-20s %8d %8d %8d %8d\n\r"
        , man->name
        , man->level
        , get_curr_hit( man )
        , get_curr_mana( man )
        , get_curr_move( man ) );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg, "hitroll" ) )
  {
    send_to_buffer( "%-20s %8s %8s\n\r", "玩家姓名" , "等級", "命中率" );

    /* 取得有效人物 */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) ) continue;

      send_to_buffer( "%-20s %8d %8d\n\r"
        , man->name
        , man->level
        , get_hitroll( man ) );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg, "damroll" ) )
  {
    send_to_buffer( "%-20s %8s %8s\n\r", "玩家姓名" , "等級", "攻擊力" );

    /* 取得有效人物 */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) ) continue;

      send_to_buffer( "%-20s %8d %8d\n\r"
        , man->name
        , man->level
        , get_damroll( man ) );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg, "armor" ) )
  {
    send_to_buffer( "%-20s %8s %8s\n\r", "玩家姓名" , "等級", "防護力" );

    /* 取得有效人物 */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) ) continue;

      send_to_buffer( "%-20s %8d %8d\n\r"
        , man->name
        , man->level
        , get_ac( man ) );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg , "filesize" ) )
  {
    send_to_buffer( "%-20s %8s %8s\n\r", "玩家姓名" , "等級", "檔案大小" );

    /* 取得有效人物 */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) || !man->pcdata ) continue;

      send_to_buffer( "%-20s %8d %s%s\e[0m\n\r"
        , man->name
        , man->level
        , man->pcdata->file_size > file_quota ? "\e[1;33m" : ""
        , numberize( man->pcdata->file_size, NULL ) );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg, "weight" ) )
  {
    send_to_buffer( "%-20s %8s %8s %8s\n\r"
      , "玩家姓名" , "等級", "負重上限", "負重" );

    /* 取得有效人物 */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) ) continue;

      aNum = can_carry_w( man );
      bNum = get_carry_weight( man );

      send_to_buffer( "%-20s %8d %8d %s%8d\e[0m\n\r"
        , man->name, man->level, aNum
        , bNum > aNum ? "\e[1;31m" : ""
        , bNum );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg, "number" ) )
  {
    send_to_buffer( "%-20s %8s %8s %8s\n\r"
      , "玩家姓名" , "等級", "數量上限", "數量" );

    /* 取得有效人物 */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) ) continue;

      aNum = can_carry_n( man );
      bNum = man->carry_number;

      send_to_buffer( "%-20s %8d %8d %s%8d\e[0m\n\r"
        , man->name, man->level
        , aNum
        , bNum > aNum ? "\e[1;31m" : ""
        , bNum );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg , "alignment" ) )
  {
    send_to_buffer( "%-20s %8s %8s\n\r", "玩家姓名" , "等級", "陣營" );

    /* 取得有效人物 */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) ) continue;

      send_to_buffer( "%-20s %8d %8d\n\r"
        , man->name, man->level, man->alignment );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg , "exp" ) )
  {
    send_to_buffer( "%-20s %8s %8s\n\r", "玩家姓名" , "等級", "經驗值" );

    /* 取得有效人物 */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) ) continue;

      send_to_buffer( "%-20s %8d %8d\n\r"
        , man->name, man->level, man->exp );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg , "eq" ) )
  {
    send_to_buffer( "%-20s %8s %8s\n\r", "玩家姓名" , "等級", "裝備吸收" );

    /* 取得有效人物 */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) ) continue;

      send_to_buffer( "%-20s %8d %8d\n\r"
        , man->name, man->level, man->eq_capcity );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg , "limit" ) )
  {
    send_to_buffer( "%-20s %8s %8s\n\r", "玩家姓名" , "等級", "極限值" );

    /* 取得有效人物 */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) ) continue;

      send_to_buffer( "%-20s %8d %8d\n\r"
        , man->name, man->level, man->limit );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg , "sex" ) )
  {
    send_to_buffer( "%-20s %8s %8s\n\r", "玩家姓名" , "等級", "性別" );

    /* 取得有效人物 */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) ) continue;

      send_to_buffer( "%-20s %8d %8s\n\r"
        , man->name, man->level, sex_name( man->sex ) );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg , "firman" ) )
  {
    send_to_buffer( "%-20s %8s %8s\n\r", "玩家姓名" , "等級", "免死金牌" );

    /* 取得有效人物 */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) ) continue;

      send_to_buffer( "%-20s %8d %8d\n\r"
        , man->name, man->level, man->firman );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg , "jail" ) )
  {
    send_to_buffer( "%-20s %8s %8s\n\r", "玩家姓名" , "等級", "受刑時間" );

    /* 取得有效人物 */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) ) continue;

      send_to_buffer( "%-20s %8d %8d\n\r"
        , man->name, man->level, man->jail );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg , "friend" ) )
  {
    send_to_buffer( "%-20s %8s %8s\n\r", "玩家姓名" , "等級", "好友數目" );

    /* 取得有效人物 */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) ) continue;

      send_to_buffer( "%-20s %8d %8d\n\r"
        , man->name, man->level, friend_count( man ) );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg , "alias" ) )
  {
    send_to_buffer( "%-20s %8s %8s\n\r", "玩家姓名" , "等級", "巨集數目" );

    /* 取得有效人物 */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) ) continue;

      send_to_buffer( "%-20s %8d %8d\n\r"
        , man->name, man->level, get_alias_count( man ) );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg , "enable" ) )
  {
    send_to_buffer( "%-20s %8s %8s\n\r", "玩家姓名" , "等級", "致能數目" );

    /* 取得有效人物 */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) ) continue;

      send_to_buffer( "%-20s %8d %8d\n\r"
        , man->name, man->level, enable_count( man ) );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg , "deposit" ) )
  {
    send_to_buffer( "%-20s %8s %8s\n\r", "玩家姓名" , "等級", "寄放數目" );

    /* 取得有效人物 */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) ) continue;

      send_to_buffer( "%-20s %8d %8d\n\r"
        , man->name, man->level, deposit_count( man ) );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg , "steal" ) )
  {
    send_to_buffer( "%-20s %8s %8s\n\r", "玩家姓名" , "等級", "偷竊次數" );

    /* 取得有效人物 */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) || !man->pcdata ) continue;

      send_to_buffer( "%-20s %8d %8d\n\r"
        , man->name, man->level, man->pcdata->steal );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg , "murder" ) )
  {
    send_to_buffer( "%-20s %8s %8s\n\r", "玩家姓名" , "等級", "謀殺次數" );

    /* 取得有效人物 */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) || !man->pcdata ) continue;

      send_to_buffer( "%-20s %8d %8d\n\r"
        , man->name, man->level, man->pcdata->murder );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg, "notepad" ) )
  {
    send_to_buffer( "%-20s %8s %8s\n\r", "玩家姓名" , "等級", "記事本" );

    /* 取得有效人物 */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) || !ch->pcdata ) continue;

      for ( count = loop = 0; loop < MAX_NOTEPAD; loop++ )
      {
        if ( ch->pcdata->notepad[loop] && *( ch->pcdata->notepad[loop] ) )
          count++;
      }

      send_to_buffer( "%-20s %8d %8d\n\r"
        , man->name, man->level, count );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg , "automap" ) )
  {
    send_to_buffer( "%-20s %8s %8s\n\r", "玩家姓名" , "等級", "自動地圖" );

    /* 取得有效人物 */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || !man->pcdata ) continue;

      for ( count = loop = 0; loop < MAX_TRACE; loop++ )
        if ( man->pcdata->map[loop] ) count++;

      send_to_buffer( "%-20s %8d %8d\n\r"
        , man->name, man->level, count );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg, "cradle" ) )
  {
    send_to_buffer( "%-20s %8s %8s\n\r", "玩家姓名" , "等級", "出生地" );

    /* 取得有效人物 */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) ) continue;

      send_to_buffer( "%-20s %8d %8s\n\r"
        , man->name, man->level, get_homename( man->home ) );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg , "xname" ) )
  {
    send_to_buffer( "%-20s %8s %8s\n\r", "玩家姓名" , "等級", "不雅字" );

    /* 取得有效人物 */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) || !man->pcdata ) continue;

      send_to_buffer( "%-20s %8d %8d\n\r"
        , man->name, man->level, man->pcdata->xname );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg , "nskill" ) )
  {
    send_to_buffer( "%-20s %8s %8s %8s\n\r"
      , "玩家姓名" , "等級", "技能個數", "可學總數" );

    /* 取得有效人物 */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) ) continue;

      send_to_buffer( "%-20s %8d %8d %8d\n\r"
        , man->name, man->level, learn_count( man ), get_learnable( ch ) );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg, "club" ) )
  {
    send_to_buffer( "%-20s %8s %8s\n\r", "玩家姓名" , "等級", "幫派名稱" );

    /* 取得有效人物 */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) ) continue;

      send_to_buffer( "%-20s %8d %s\n\r"
        , man->name, man->level, club_name( man ) );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg, "class" ) )
  {
    send_to_buffer( "%-20s %8s %8s\n\r", "玩家姓名" , "等級", "職業名稱" );

    /* 取得有效人物 */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) ) continue;

      send_to_buffer( "%-20s %8d %s\n\r"
        , man->name, man->level, class_name( man->class, FALSE ) );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg, "marry" ) )
  {
    send_to_buffer( "%-20s %8s %8s\n\r", "玩家姓名" , "等級", "婚姻狀態" );

    /* 取得有效人物 */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) || !man->pcdata ) continue;

      send_to_buffer( "%-20s %8d %s\n\r"
        , man->name, man->level
        , YESNO( man->pcdata->mater && *man->pcdata->mater ) );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg, "fight" ) )
  {
    send_to_buffer( "%-20s %8s %8s %8s %8s\n\r"
      , "玩家姓名" , "等級", "殺敵總數", "殺敵等級", "平均等級" );

    /* 取得有效人物 */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) || !man->pcdata ) continue;

      send_to_buffer( "%-20s %8d %8d %8d %8d\n\r"
        , man->name, man->level
        , man->pcdata->kills
        , man->pcdata->kill_level
        , man->pcdata->kills <= 0
          ? 0 : man->pcdata->kill_level / man->pcdata->kills );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg, "stock" ) )
  {
    int sn;

    one_argument( argument, arg );

    if ( arg[0] == '\x0' )
    {
      send_to_char( "你要看哪一張股票的持有數﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( !is_number( arg ) || ( sn = atoi( arg ) - 1 ) < 0 || sn >= MAX_STOCK )
    {
      send_to_char( "範圍錯誤﹗請查詢 stock \n\r", ch );
      RETURN_NULL();
    }

    if ( !stock_data[sn].name )
    {
      send_to_char( "這家公司尚未上櫃﹐無法觀察。\n\r", ch );
      RETURN_NULL();
    }

    send_to_buffer( "%-20s %8s %s\n\r", "玩家姓名" , "等級"
      , stock_data[sn].name );

    /* 取得有效人物 */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) || !man->pcdata ) continue;

      send_to_buffer( "%-20s %8d %-4d\n\r"
        , man->name, man->level, man->pcdata->stock[sn] );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg, "gift" ) )
  {
    GIFT_DATA * pGift;
    int         slot;
    int         loop;

    if ( !gift_list )
    {
      act( "$t沒有任何禮物﹐無法查詢﹗", ch, mud_name, NULL, TO_CHAR );
      RETURN_NULL();
    }

    one_argument( argument, arg );

    if ( arg[0] == '\x0' || !is_number( arg ) )
    {
      send_to_char( "對不起﹐你要查詢哪一樣禮物發送情形﹖\n\r", ch );
      RETURN_NULL();
    }

    slot = atoi( arg );
    for ( loop = 1, pGift = gift_list; pGift; pGift = pGift->next, loop++ )
      if ( loop == slot ) break;

    if ( !pGift )
    {
      send_to_char( "對不起﹐沒有那個編號的禮物﹗\n\r", ch );
      RETURN_NULL();
    }

   clear_buffer();
    send_to_buffer( "%-20s %8s %s(%d)\n\r"
      , "玩家姓名" , "等級", pGift->title, slot );

    /* 取得有效人物 */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) ) continue;

      send_to_buffer( "%-20s %8d %s\n\r"
        , man->name, man->level
        , YESNO( !check_gift_stamp( man, pGift->stamp ) ) );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg, "skill" ) )
  {
    SKILL_DATA * pSkill;

    one_argument( argument, arg );

    if ( arg[0] == '\x0' )
    {
      send_to_char( "你要看哪一種技能的熟練度﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( !( pSkill = skill_isname( arg ) ) )
    {
      act( "對不起﹐沒有你要的技能 $2$T$0。", ch, NULL, arg, TO_CHAR );
      RETURN_NULL();
    }

    send_to_buffer( "%-20s %s熟練度\n\r", "玩家姓名", pSkill->cname );

    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) ) continue;
      send_to_buffer( "%-20s %3d\n\r", man->name, man->skill[pSkill->slot] );
    }
    print_buffer( ch );
    RETURN_NULL();
  }

  do_data( ch , "" );
  RETURN_NULL();
}

FUNCTION( do_xnames )
{
  XNAMES_DATA * xnames;
  XNAMES_DATA * prev;
  XNAMES_DATA * curr;
  char          arg[ MAX_INPUT_LENGTH ];
  char          type[ MAX_INPUT_LENGTH ];
  char        * msg;
  int           count;
  int           format;

  PUSH_FUNCTION( "do_xnames" );

  /* 非玩家不准執行這個指令 */
  if ( IS_NPC( ch ) ) RETURN_NULL();

  argument = one_argument( argument , arg );

  if ( !arg[0] )
  {
    clear_buffer();
    send_to_buffer( "系統載入的保留字有﹕\n\r" );
    count = 0;

    for ( xnames = xnames_first, count = 0; xnames; xnames = xnames->next )
    {
      send_to_buffer( "%-18s 旗標 %s%s\n\r"
        , xnames->type == XNAMES_CHAT && xnames->word && *xnames->word == '@'
          ? xnames->word + 1 : xnames->word
        , xnames_bit_name( xnames->type )
        , xnames->type == XNAMES_CHAT && xnames->word && *xnames->word == '@'
          ? " \x1b[1;32m不考慮大小寫\x1b[0m" : "" );
      count++;
    }

    if ( count == 0 ) send_to_buffer( "沒有任何系統保留字被載入。\n\r" );
    else              send_to_buffer( "\n\r共有 %d 系統保留字。\n\r" , count );

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( arg[0] == '+' )
  {
    one_argument( argument , type );

    for ( xnames = xnames_first; xnames; xnames = xnames->next )
    {
      if ( !str_cmp( xnames->word , arg + 1 ) )
      {
        act( "對不起﹐這個保留字 $2$T$0 已經有了。" , ch, NULL, arg+1, TO_CHAR );
        RETURN_NULL();
      }
    }

    if ( !type[0] || !is_number( type ) )
    {
      send_to_char( "請查詢 xnames 正確的方法。\n\r" , ch );
      RETURN_NULL();
    }

    switch( ( format = atoi( type ) ) )
    {
    default :
      send_to_char( "你的保留字旗標不對。\n\r" , ch );
      RETURN_NULL();

    case XNAMES_SYSTEM:
      msg = "系統保留字";
      break;

    case XNAMES_CACONYM:
      msg = "不雅字";
      break;

    case XNAMES_CHAT :
      msg = "閒聊保留字";
      break;

    case XNAMES_CNAME:
      msg = "中文名字保留字";
      break;
    }

    /* 配置記憶體 */
    xnames = alloc_struct( STRUCT_XNAME_DATA );

    xnames->word = str_dup( arg + 1 );
    xnames->type = format;
    xnames->next = xnames_first;
    xnames_first = xnames;

    act( "你把$t設定為$T。", ch, arg+1, msg, TO_CHAR );
    RETURN_NULL();
  }

  else if ( arg[0] == '-' )
  {
    prev = NULL;
    for ( curr = xnames_first; curr; prev = curr, curr = curr->next )
    {
      if ( !str_cmp( arg + 1 , curr->word ) )
      {
        if ( curr->type == XNAMES_SYSTEM )
        {
          act( "對不起﹐$2$T$0 是系統保留字﹐不能刪除。" , ch, NULL, arg+1, TO_CHAR );
          RETURN_NULL();
        }

        else
        {
          if ( prev == NULL ) xnames_first = xnames_first->next;
          else                prev->next   = curr->next;

          free_string( curr->word );
          curr->type  = 0;

          /* 釋放結構 */
          free_struct( curr , STRUCT_XNAME_DATA );

          act( "$2$T$0 不再是保留字。", ch, NULL, arg+1, TO_CHAR );
          RETURN_NULL();
        }
      }
    }

    send_to_char( "找不到你要刪除的保留字。\n\r" , ch );
    RETURN_NULL();
  }

  else
  {
    send_to_char( "請查詢 xnames 正確的方法。\n\r" , ch );
    RETURN_NULL();
  }

  RETURN_NULL();
}

FUNCTION( do_lockcmd )
{
  CMD_DATA * pCommand;
  int        trust;

  PUSH_FUNCTION( "do_lockcmd" );

  trust = get_trust( ch );

  for ( pCommand = cmd_list; pCommand; pCommand = pCommand->next )
  {
    if ( str_cmp( pCommand->name , argument ) || pCommand->level > trust )
      continue;

    if ( pCommand->canlock == FALSE )
    {
      act( "你不能鎖定這個指令 $2$T$0。" , ch , NULL, argument, TO_CHAR );
      RETURN_NULL();
    }

    if ( pCommand->lock == TRUE )
    {
      pCommand->lock = FALSE;
      act( "你解開指令$t的鎖定。" , ch, pCommand->name, NULL, TO_CHAR );
    }

    else
    {
      pCommand->lock = TRUE;
      act( "你鎖住了$t這個指令。" , ch, pCommand->name, NULL, TO_CHAR );
    }

    RETURN_NULL();
  }

  send_to_char( "沒有找到你要鎖住的指令。\n\r" , ch );
  RETURN_NULL();
}

FUNCTION( do_sector )
{
  SECTOR_DATA * pSector;
  char          arg[MAX_INPUT_LENGTH];
  bool          Found   = FALSE;
  bool          fAll    = FALSE;
  int           iSector = 0;
  int           count;

  PUSH_FUNCTION( "do_sector" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  one_argument( argument , arg );

  if ( !arg[0] )
  {
    fAll = TRUE;
  }

  else
  {
    if ( !is_number( arg ) )
    {
      send_to_char( "地形參數必須是數字﹐請查詢 sector 的用法。\n\r" , ch );
      RETURN_NULL();
    }

    iSector = atoi( arg );
  }

  clear_buffer();
  for ( count = 0, pSector = sector_list; pSector; pSector = pSector->next )
  {
    if ( fAll || pSector->vnum == iSector )
    {
      if ( !Found ) send_to_buffer(
           "\e[1;33;44m順序 編號 中文名稱             移動  "
           "數量 天黑 旗標\e[0m\n\r"
           VERTICAL_LINE );
      Found = TRUE;
      send_to_buffer( "%2d). %4d %-20s %4d %5d %4s %s\n\r"
        , ++count
        , pSector->vnum
        , pSector->cname
        , pSector->movement
        , pSector->count
        , YESNO( pSector->dark )
        , sector_flag_name( pSector ) );
    }
  }

  if ( !Found ) send_to_buffer( "沒有找到符合你要求的資料。\n\r" );
  else send_to_buffer( "%s", VERTICAL_LINE );

  print_buffer( ch );
  RETURN_NULL();
}

FUNCTION( do_shop )
{
  char              arg[MAX_INPUT_LENGTH];
  char              buf[MAX_STRING_LENGTH];
  char              buf1[MAX_STRING_LENGTH];
  char              buf2[MAX_STRING_LENGTH];
  SHOP_DATA       * pShop;
  CHAR_DATA       * victim;
  MOB_INDEX_DATA  * pMobIndex;
  ROOM_INDEX_DATA * pRoom;
  int               count;
  int               iTrade;

  PUSH_FUNCTION( "do_shop" );

  one_argument( argument, arg );

  if ( !arg[0] )
  {
    clear_buffer();
    for ( count = 0, pShop = shop_first; pShop; pShop = pShop->next )
    {
      if ( !( pMobIndex = get_mob_index( pShop->keeper ) ) ) continue;

      if ( count++ == 0 ) send_to_buffer(
        "\e[1;33;44m順序 型態 販賣者            編號 營業時間 買"
        "賣利益  地點 種類                   \e[0m\n\r" );

      str_cpy( buf, mob_index_name( NULL, pMobIndex ) );
      buf[16] = '\x0';

      send_to_buffer( "%4d %4s %-16s %5d  %2d - %2d  %s%3d\e[0m"
         "%s-%3d\e[0m %5d"
         , count, shop_type_name( pShop->type )
         , buf
         , pMobIndex->vnum
         , pShop->open_hour, pShop->close_hour
         , pShop->profit_buy <= 100 ? "\e[1;31m" : ""
         , pShop->profit_buy
         , pShop->profit_sell >= 100 ? "\e[1;31m" : ""
         , pShop->profit_sell
         , pShop->sential ? pShop->sential->vnum : -1 );

      buf1[0] = '\x0';
      switch( pShop->type )
      {
      default:
        break;

      case SHOP_COPER:

        for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
        {
          if ( pShop->buy_type[iTrade] <= 0
            || !( pMobIndex = get_mob_index( pShop->buy_type[iTrade] ) ) )
            break;

          sprintf( buf2, " %d", pShop->buy_type[iTrade] );
          str_cat( buf1, buf2 );
        }

        break;

      case SHOP_STORE:

        for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
        {
          if ( pShop->buy_type[iTrade] <= 0 ) break;
          str_cat( buf1, " " );
          str_cat( buf1, item_kind_name( pShop->buy_type[iTrade] ) );
        }

        break;

      case SHOP_SMITH:

        for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
        {
          if ( pShop->buy_type[iTrade] <= 0 ) break;
          str_cat( buf1, " " );
          str_cat( buf1, wear_bit_name( pShop->buy_type[iTrade] ) );
        }
        break;
      }

      buf1[24] = '\x0';
      send_to_buffer( "%-24s\n\r", buf1 );
    }

    if ( count == 0 ) send_to_buffer( "沒有發現任何的商店型態。\n\r" );
    print_buffer( ch );
  }

  else if ( is_number( arg ) )
  {
    if ( !( pRoom = get_room_index( atoi( arg ) ) ) )
    {
      act( "沒有這個地方 $2$T$0﹗", ch, NULL, arg, TO_CHAR );
      RETURN_NULL();
    }

    clear_buffer();
    count = 0;
    for ( victim = pRoom->people; victim; victim = victim->next_in_room )
    {
      if ( verify_char( victim )
        && IS_NPC( victim )
        && ( pMobIndex = victim->pIndexData )
        && ( pShop = pMobIndex->pShop ) )
      {
        if ( count++ != 0 ) send_to_buffer( "\n\r" );
        send_to_buffer(
          "順序    ﹕%d\n\r"
          "型態    ﹕%s\n\r"
          "販賣者  ﹕%s\n\r"
          "主人編號﹕%d\n\r"
          "開幕時間﹕%d\n\r"
          "打烊時間﹕%d\n\r"
          "利益比  ﹕%d-%d\n\r"
          "營業地點﹕%d\n\r"
          "種類    :"
          , count, shop_type_name( pShop->type )
          , mob_index_name( NULL, pMobIndex )
          , pMobIndex->vnum, pShop->open_hour, pShop->close_hour
          , pShop->profit_buy, pShop->profit_sell
          , pShop->sential ? pShop->sential->vnum : -1 );

        switch( pShop->type )
        {
        case SHOP_STORE:

          for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
          {
            if ( pShop->buy_type[iTrade] <= 0 ) break;
            send_to_buffer( " %s", item_kind_name( pShop->buy_type[iTrade] ) );
          }
          break;

        case SHOP_SMITH:

          for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
          {
            if ( pShop->buy_type[iTrade] <= 0 ) break;
            send_to_buffer( " %s", wear_bit_name( pShop->buy_type[iTrade] ) );
          }
          break;
        }

        send_to_buffer( "\n\r" );
      }
    }

    if ( count == 0 ) send_to_buffer( "那裡沒有任何的商店型態﹗\n\r" );
    print_buffer( ch );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "here" ) )
  {
    if ( !ch->in_room )
    {
      mudlog( LOG_DEBUG, "do_shop: 來源錯誤." );
      RETURN_NULL();
    }

    clear_buffer();
    count = 0;
    for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
      if ( verify_char( victim )
        && IS_NPC( victim )
        && ( pMobIndex = victim->pIndexData )
        && ( pShop = pMobIndex->pShop ) )
      {
        if ( count++ != 0 ) send_to_buffer( "\n\r" );
        send_to_buffer(
          "順序    ﹕%d\n\r"
          "型態    ﹕%s\n\r"
          "販賣者  ﹕%s\n\r"
          "主人編號﹕%d\n\r"
          "開幕時間﹕%d\n\r"
          "打烊時間﹕%d\n\r"
          "利益比  ﹕%d-%d\n\r"
          "營業地點﹕%d\n\r"
          "種類    ﹕"
          , count, shop_type_name( pShop->type )
          , mob_index_name( NULL, pMobIndex )
          , pMobIndex->vnum, pShop->open_hour, pShop->close_hour
          , pShop->profit_buy, pShop->profit_sell
          , pShop->sential ? pShop->sential->vnum : -1 );

        switch( pShop->type )
        {
        case SHOP_STORE:

          for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
          {
            if ( pShop->buy_type[iTrade] <= 0 ) break;
            send_to_buffer( " %s", item_kind_name( pShop->buy_type[iTrade] ) );
          }
          break;

        case SHOP_SMITH:

          for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
          {
            if ( pShop->buy_type[iTrade] <= 0 ) break;
            send_to_buffer( " %s", wear_bit_name( pShop->buy_type[iTrade] ) );
          }
          break;
        }

        send_to_buffer( "\n\r" );
      }
    }

    if ( count == 0 ) send_to_buffer( "這裡沒有任何的商店型態。\n\r" );
    print_buffer( ch );
    RETURN_NULL();
  }

  else
  {
    send_to_char( "你的參數錯誤﹐請查詢 shop 的使用方法。\n\r", ch );
    RETURN_NULL();
  }

  RETURN_NULL();
}

FUNCTION( do_site )
{
  int               count = 0;
  DESCRIPTOR_DATA * man;
  CHAR_DATA       * victim;

  PUSH_FUNCTION( "do_site" );

  clear_buffer();

  for ( man = descriptor_list; man; man = man->next )
  {
    if ( !verify_desc( man )
      || !man->host
      || !( victim = man->character )
      || str_prefix( argument, man->host ) ) continue;

    send_to_buffer( "%3d. %12s %25s\n\r"
      , ++count, victim->name, man->host );
  }

  if ( count == 0 ) send_to_char( "沒有找到符合你條件位址的玩家。\n\r", ch );
  else  print_buffer( ch );

  RETURN_NULL();
}

FUNCTION( do_damage )
{
  SKILL_DATA  * pSkill;
  DAMAGE_DATA * pDam;
  EFFECT_DATA * pEffect;
  int           count = 0;
  int           loop;
  int           Number;
  int           chance = 0;
  char          arg[MAX_INPUT_LENGTH];
  char          buf[MAX_INPUT_LENGTH];

  PUSH_FUNCTION( "do_damage" );

  argument = one_argument( argument, arg );

  if ( !arg[0] || !( pSkill = skill_isname( arg ) ) )
  {
    act( "找不到你要的法術或是技能名稱 $2$T$0。" , ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  if ( !pSkill->damage )
  {
    act( "對不起﹐$w沒有攻傷敘述。", ch, pSkill, NULL, TO_CHAR );
    RETURN_NULL();
  }

  for ( pDam = pSkill->damage; pDam; pDam = pDam->next )
  {
    chance += pDam->chance;
    count++;
  }

  one_argument( argument, arg );
  chinese_number( count, buf );

  if ( !arg[0] || !is_number( arg ) )
  {
    clear_buffer();
    send_to_buffer( "%s(%s)共有%s招。 機率總和為 %d﹐分別為﹕\n\r"
      , pSkill->cname, pSkill->name , buf, pSkill->chance );

    for ( loop = 0, pDam = pSkill->damage; pDam; pDam = pDam->next )
      send_to_buffer( "%2d. %3d(%3d%%) 數值 %3d 副數值 %d\n\r"
        , ++loop, pDam->chance
        , chance <= 0 ? 0 : pDam->chance * 100 / chance
        , pDam->value, pDam->vicevalue );

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( ( Number = atoi( arg ) ) <= 0 || Number > count )
  {
    act( "$w只有$T個攻傷敘述。", ch, pSkill, buf, TO_CHAR );
    RETURN_NULL();
  }

  for ( loop = 1, pDam = pSkill->damage; pDam; pDam = pDam->next )
    if ( loop++ == Number ) break;

  if ( !pDam )
  {
    act( "找不到你要的$w的第$T號攻傷敘述。", ch, pSkill, buf, TO_CHAR );
    RETURN_NULL();
  }

  clear_buffer();

  send_to_buffer(
    "法術名稱﹕ %s(%s) 機率總和﹕ %d\n\r"
    "順序﹕%2d/%2d 機率 %3d/%3d(%3d%%) 數值 %d 副數值 %d\n\r"
    "格擋﹕%3d 特殊部位﹕%s\n\r"
    , pSkill->cname, pSkill->name, pSkill->chance
    , Number, count, pDam->chance, chance
    , chance <= 0 ? 0 : pDam->chance * 100 / chance
    , pDam->value, pDam->vicevalue
    , pDam->parry, attack_situs( pDam->situs ) );

  /* 是否有特殊武器 */
  if ( pDam->innate )
  {
    send_to_buffer( "特殊武器﹕%s(%s)(%d) 特殊武器倍數﹕%d%%\n\r"
    , pDam->innate->short_descr
    , pDam->innate->name
    , pDam->innate->vnum
    , pDam->multiple );
  }

  if ( pDam->routine ) send_to_buffer( "這個攻傷有特攻﹗\n\r" );

  for ( pEffect = pDam->effect; pEffect; pEffect = pEffect->next )
  {
    send_to_buffer( "效應﹕%s 數值﹕" , effect_type_name( pEffect->type ) );

    for ( loop = 0; loop < MAX_EFFECT_VALUE; loop++ )
      send_to_buffer( " %d", pEffect->value[loop] );

    send_to_buffer( "\n\r" );
  }

  send_to_buffer( "描述﹕\n\r%s\n\r", pDam->description );
  print_buffer( ch );
  RETURN_NULL();
}

FUNCTION( do_liquid )
{
  LIQ_DATA * pLiq;

  PUSH_FUNCTION( "do_liq" );

  if ( !liq_list )
  {
    act( "$t沒有任何的液體的資料。" , ch, mud_name, NULL, TO_CHAR );
    RETURN_NULL();
  }

  send_to_char(
    "\e[1;42m編號     液體名稱 液體顏色 止渴 飢餓 酒醉  水               "
    "                   \e[0m\n\r", ch );
  for ( pLiq = liq_list; pLiq; pLiq = pLiq->next )
  {
    print_to_char( ch, "%4d %12s %8s %4d %4d %4d  %s\n\r"
      , pLiq->slot
      , pLiq->name
      , pLiq->color
      , pLiq->thirst
      , pLiq->full
      , pLiq->drunk
      , YESNO( pLiq->water ) );
  }
  RETURN_NULL();
}

#define MEN_PER_LINE    5

FUNCTION( do_wizlist )
{
  IMMLIST_DATA * pImmlist;
  char           arg[MAX_INPUT_LENGTH];
  char           buf[1024];
  char           buf1[1024] = { '\x0' };
  char           buf2[1024] = { '\x0' };
  char           buf3[1024] = { '\x0' };
  char           buf4[1024] = { '\x0' };
  char           buf5[1024] = { '\x0' };
  int            imm_god = 0;
  int            imm_sup = 0;
  int            imm_dei = 0;
  int            imm_ang = 0;
  int            imm_her = 0;
  int            imm_adv = 0;

  PUSH_FUNCTION( "do_wizlist" );

  if ( !imm_list )
  {
    act( "對不起﹐$t裡沒有任何的管理者﹗", ch, mud_name, NULL, TO_CHAR );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg );
  if ( !arg[0] )
  {
    for ( pImmlist = imm_list; pImmlist; pImmlist = pImmlist->next )
    {
      switch ( UMAX( pImmlist->level, pImmlist->trust ) )
      {
      case L_GOD:

        if ( imm_god++ > 1 && ( imm_god - 1 ) % MEN_PER_LINE == 0 )
          str_cat( buf1, "\n\r" );

        if ( on_line( pImmlist->name, ch ) )
        {
          sprintf( buf, "\e[1;32m*%-14s\e[0m", pImmlist->name );
        }

        else if ( pImmlist->adviser == TRUE )
        {
          imm_adv++;
          sprintf( buf, "\e[1;36m%-15s\e[0m", pImmlist->name );
        }

        else
        {
          sprintf( buf, "%-15s", pImmlist->name );
        }

        str_cat( buf1, buf );
        break;

      case L_SUP:

        if ( imm_sup++ > 1 && ( imm_sup - 1 ) % MEN_PER_LINE == 0 )
          str_cat( buf2, "\n\r" );

        if ( on_line( pImmlist->name, ch ) )
        {
          sprintf( buf, "\e[1;32m*%-14s\e[0m", pImmlist->name );
        }

        else if ( pImmlist->adviser == TRUE )
        {
          imm_adv++;
          sprintf( buf, "\e[1;36m%-15s\e[0m", pImmlist->name );
        }

        else
        {
          sprintf( buf, "%-15s", pImmlist->name );
        }

        str_cat( buf2, buf );
        break;

      case L_DEI:

        if ( imm_dei++ > 1 && ( imm_dei - 1 ) % MEN_PER_LINE == 0 )
          str_cat( buf3, "\n\r" );

        if ( on_line( pImmlist->name, ch ) )
        {
          sprintf( buf, "\e[1;32m*%-14s\e[0m", pImmlist->name );
        }

        else if ( pImmlist->adviser == TRUE )
        {
          imm_adv++;
          sprintf( buf, "\e[1;36m%-15s\e[0m", pImmlist->name );
        }

        else
        {
          sprintf( buf, "%-15s", pImmlist->name );
        }

        str_cat( buf3, buf );
        break;

      case L_ANG:

        if ( imm_ang++ > 1 && ( imm_ang - 1 ) % MEN_PER_LINE == 0 )
          str_cat( buf4, "\n\r" );

        if ( on_line( pImmlist->name, ch ) )
        {
          sprintf( buf, "\e[1;32m*%-14s\e[0m", pImmlist->name );
        }

        else if ( pImmlist->adviser == TRUE )
        {
          imm_adv++;
          sprintf( buf, "\e[1;36m%-15s\e[0m", pImmlist->name );
        }
        else
        {
          sprintf( buf, "%-15s", pImmlist->name );
        }

        str_cat( buf4, buf );
        break;

      case L_HER:

        if ( imm_her++ > 1 && ( imm_her - 1 ) % MEN_PER_LINE == 0 )
          str_cat( buf5, "\n\r" );

        if ( on_line( pImmlist->name, ch ) )
        {
          sprintf( buf, "\e[1;32m*%-14s\e[0m", pImmlist->name );
        }

        else if ( pImmlist->adviser == TRUE )
        {
          imm_adv++;
          sprintf( buf, "\e[1;36m%-15s\e[0m", pImmlist->name );
        }
        else
        {
          sprintf( buf, "%-15s", pImmlist->name );
        }

        str_cat( buf5, buf );
        break;
      }
    }

    clear_buffer();
    send_to_buffer( "\e[1;31m%s\e[0m的管理員有﹕(目前共有顧問 %d 位)\n\r"
      , mud_name, imm_adv );

    if ( imm_god > 0 )
    {
      send_to_buffer( "----------------------------\e[1;37m    天帝      "
        "  地皇 \e[0m   ----------------------------\n\r" );
      send_to_buffer( "%s\n\r", buf1 );
    }

    if ( imm_sup > 0 )
    {
      send_to_buffer( "----------------------------\e[1;35m  混世魔王    "
        "羅煞女王\e[0m  ----------------------------\n\r" );
      send_to_buffer( "%s\n\r", buf2 );
    }

    if ( imm_dei > 0 )
    {
      send_to_buffer( "---------------------------- \e[1;36m   菩薩       "
        "夜叉王\e[0m   ----------------------------\n\r" );
      send_to_buffer( "%s\n\r", buf3 );
    }

    if ( imm_ang > 0 )
    {
      send_to_buffer( "---------------------------- \e[1;31m   天仙     "
        "   判官\e[0m    ----------------------------\n\r" );
      send_to_buffer( "%s\n\r", buf4 );
    }

    if ( imm_her > 0 )
    {
      send_to_buffer( "----------------------------    \e[1;32m護法      "
        "  無常    \e[0m----------------------------\n\r" );
      send_to_buffer( "%s\n\r", buf5 );
    }

    send_to_buffer( "------------------------------------------"
        "--------------------------------------\n\r" );
    print_buffer( ch );
    RETURN_NULL();
  }

  else
  {
    if ( !( pImmlist = imm_lookup( arg ) ) )
    {
      act( "對不起﹐沒有這位管理員 $2$T$0 喔﹗", ch, NULL, arg, TO_CHAR );
      RETURN_NULL();
    }

    print_to_char( ch, "管理員名稱﹕%s\n\r"
                       "等級      ﹕%d\n\r"
                       "是否在線上﹕%s\n\r"
                       "顧問      ﹕%s\n\r"
      , pImmlist->name, UMAX( pImmlist->trust, pImmlist->level )
      , YESNO( on_line( pImmlist->name, ch ) )
      , YESNO( pImmlist->adviser ) );

   if ( pImmlist->description && *pImmlist->description )
   {
     char * pString;

     pString = pImmlist->description;

     act( "自我介紹  ﹕$A$t"
       , ch, *pString == '.' ? pString + 1 : pString, NULL, TO_CHAR );
   }

   else
   {
     act( "抱歉﹐$2$T$0 沒有自我介紹。", ch, NULL, arg, TO_CHAR );
   }
  }

  RETURN_NULL();
}

#undef MEN_PER_LINE

FUNCTION( do_cleanup )
{
  PUSH_FUNCTION( "do_cleanup" );

  if ( !IS_NPC( ch ) )
  {
    system_cleanup();
    send_to_char( "清除過期的玩家檔案﹗\n\r", ch );
  }
  RETURN_NULL();
}

void system_cleanup( void )
{
  char            directory[MAX_FILE_LENGTH];
  char            filename[MAX_FILE_LENGTH];
  char            exec_cmd[MAX_STRING_LENGTH];
  char            header;
  DIR           * reading;
  struct dirent * next;
  struct stat     Status;
  time_t          nTimes;
  time_t          nHold;

  PUSH_FUNCTION( "system_cleanup" );

  nTimes = time( NULL );
  nHold  = hold_day * 24 * 60 * 60;

  for ( header = 'a'; header <= 'z'; header++ )
  {
    sprintf( directory, "%s/%c/", player_dir, header );

    /* 開啟子目錄 */
    if ( ( reading = opendir( directory ) ) )
    {
      /* 讀取目錄的內容 */
      while ( ( next = readdir( reading ) ) )
      {
        if ( !strcmp( next->d_name, "." ) || !strcmp( next->d_name, ".." ) )
          continue;

        sprintf( filename, "%s%s", directory, next->d_name );
        if ( !is_directory( filename ) ) continue;

        sprintf( filename, "%s%s/data", directory, next->d_name );

        if ( stat( filename, &Status ) == 0 )
        {
          if ( S_ISREG( Status.st_mode ) )
          {
            if ( imm_lookup( next->d_name ) ) continue;

            if ( ( nTimes - Status.st_mtime ) > nHold || Status.st_size <= 0 )
            {
              sprintf( exec_cmd, "rm -rf %s%s", directory, next->d_name );
              system( exec_cmd );

              /* 更正英雄榜 */
              check_hero( next->d_name );

              /* 更正資料庫 */
              erase_database( next->d_name );

              /* 更正幫派 */
              name_from_club( next->d_name, FALSE );
            }
          }
        }

        else
        {
          mudlog( LOG_DEBUG, "do_cleanup: 無法取得%s的檔案狀態.", filename );
        }
      }

      closedir( reading );
      save_club();
    }
    else
    {
      mudlog( LOG_DEBUG, "do_cleanup: 無法開啟目錄 %s.", directory );
    }
  }

  RETURN_NULL();
}

FUNCTION( do_jail )
{
  ROOM_INDEX_DATA * location;
  CHAR_DATA       * victim;
  DESCRIPTOR_DATA * pDesc;
  char              arg1[MAX_INPUT_LENGTH];
  char              arg2[MAX_INPUT_LENGTH];
  char              buf[MAX_STRING_LENGTH];
  int               duration;
  int               count;

  PUSH_FUNCTION( "do_jail" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  if ( !RoomJail )
  {
    act( "$t中沒有天牢﹐所有動作取消。", ch, mud_name, NULL, TO_CHAR );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );

  if ( arg1[0] == '\x0' || arg2[0] == '\x0' )
  {
    send_to_char( "參數錯誤﹐請查詢正確的使用方法﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg1, "!address" ) )
  {
    one_argument( argument, arg1 );

    if ( !is_number( arg1 ) || ( duration = atoi( arg1 ) ) <= 0 )
    {
      act( "你要關 $2$t$0 多久咧﹖", ch, arg1, NULL, TO_CHAR );
      RETURN_NULL();
    }

    for ( count = 0, pDesc = descriptor_list; pDesc; pDesc = pDesc->next )
    {
      if ( !verify_desc( pDesc )
        || !( victim = pDesc->character )
        || pDesc->connected != CON_PLAYING
        || strcmp( arg2, pDesc->host )
        || victim == ch )
        continue;

      act( "$N符合位址 $t 的條件。", ch, arg2, victim, TO_CHAR );

      if ( jail_someone( ch, victim, duration, TRUE ) )
      {
        sprintf( buf, "%s犯了天條﹐被管理者捉起來了。"
          , mob_name( NULL, victim ) );

        talk_channel_2( buf, CHANNEL_BULLETIN, "快報" );
        count++;
      }
    }

    if ( count <= 0 ) send_to_char( "對不起﹐找不到這個位址的玩家。\n\r", ch );
    else act( "你總共關了 $i 位玩家。", ch, &count, NULL, TO_CHAR );

    RETURN_NULL();
  }

  if ( !str_prefix( arg1, "!set" ) )
  {
    if ( !( victim = get_char_world( ch, arg2 ) ) )
    {
      act( "找不到你要關的人 $2$T$0 耶。", ch, NULL, arg2, TO_CHAR );
      RETURN_NULL();
    }

    if ( IS_NPC( victim ) )
    {
      act( "$N本來就不會被關了。", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( victim->jail <= 0 )
    {
      act( "$N本來就沒有被關起來。", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    one_argument( argument, arg2 );

    if ( arg2[0] == '\x0'
      || !is_number( arg2 )
      || ( duration = atoi( arg2 ) ) <= 0 )
    {
      send_to_char( "你打算改變他的服刑時間為多少咧﹖\n\r", ch );
      RETURN_NULL();
    }

    act( "管理者把你的服刑時間改成$i天﹗", victim, &duration, NULL, TO_CHAR );
    act( "你把$N的服刑時間改成$i天﹗", ch, &duration, victim, TO_CHAR );

    victim->jail = duration;
    RETURN_NULL();
  }

  else if ( !str_prefix( arg1, "!release" ) )
  {
    if ( !( victim = get_char_world( ch, arg2 ) ) )
    {
      act( "找不到你要釋放的人 $2$T$0 耶﹗", ch, NULL, arg2, TO_CHAR );
      RETURN_NULL();
    }

    if ( IS_NPC( victim ) )
    {
      act( "非玩家$N本來就不會被關了﹗", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( victim->jail <= 0 )
    {
      act( "$N本來就沒有被關起來﹗", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( !victim->in_room || victim->in_room != RoomJail )
    {
      act( "$N逃獄了。", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( victim->position != POS_STANDING )
    {
      act( "先把$N叫起來吧。", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( !( location = get_hometown( victim ) ) )
    {
      act( "外面世界太黑暗了﹐干脆叫$N待在這裡吧﹗", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( victim->fighting ) stop_fighting( victim, TRUE );
    if ( victim->mount    ) unmount_char( victim, victim->mount    );
    if ( victim->mount_by ) unmount_char( victim->mount_by, victim );

    /* 修正小鬼 */
    if ( ch->spirit ) spirit_from_char( ch->spirit );

    send_to_char( "喂﹐你可以出獄囉﹐希望你好好做人﹗\n\r", victim );
    act( "恭喜$n可以出獄囉。", victim, NULL, NULL, TO_ROOM );
    char_from_room( victim );
    char_to_room( victim, location );
    act( "$n在這裡開始從頭做人。", victim, NULL, NULL, TO_ROOM );
    do_look( victim, "auto" );

    /* 清除追蹤紀錄點 */
    clear_trace( victim, TRUE );

    victim->jail = 0;

    act( "你把$N給釋放了。", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  else
  {
    if ( !is_number( arg2 ) || ( duration = atoi( arg2 ) ) <= 0 )
    {
      send_to_char( "你要關他多久咧﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( !( victim = get_char_world( ch, arg1 ) ) )
    {
      act( "找不到你要關的對象 $2$T$0﹗", ch, NULL, arg1, TO_CHAR );
      RETURN_NULL();
    }

    if ( jail_someone( ch, victim, duration, TRUE ) )
    {
      sprintf( buf, "%s犯了天條﹐被管理者捉起來了。"
        , mob_name( NULL, victim ) );

      talk_channel_2( buf, CHANNEL_BULLETIN, "快報" );
    }

    RETURN_NULL();
  }

  RETURN_NULL();
}

FUNCTION( do_sset )
{
  PUSH_FUNCTION( "do_sset" );
  RETURN_NULL();
}

FUNCTION( do_level )
{
  int  level;
  char arg[MAX_INPUT_LENGTH];

  PUSH_FUNCTION( "do_level" );

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    send_to_char( "這個指令需要參數﹐請查詢指令用法。\n\r", ch );
    RETURN_NULL();
  }

  if ( is_number( arg ) )
  {
    if ( ( level = atoi( arg ) ) < 0 || level >= MAX_LEVEL )
    {
      send_to_char( "你要查詢的等級不合理﹗\n\r", ch );
      RETURN_NULL();
    }

    print_to_char( ch,
      "玩家等級       %-d\n\r"
      "升級的隻數     %-d\n\r"
      "升級的經驗值   %-d\n\r"
      "怪物的隻數     %-d\n\r"
      "怪物的死亡隻數 %-d\n\r"
      "怪物的傷害力   %-d\n\r"
      "怪物的閃躲力   %-d\n\r"
      , level
      , kill_table[level].promotion
      , kill_table[level].exp
      , kill_table[level].number
      , kill_table[level].killed
      , kill_table[level].damage
      , kill_table[level].dodge );
  }

  else
  {
    send_to_char( "錯誤的參數﹐請查詢指令的用法。\n\r", ch );
  }
  RETURN_NULL();
}

FUNCTION( do_sale )
{
  SALE_DATA      * pSale;
  CHAR_DATA      * victim;
  OBJ_INDEX_DATA * obj;
  MOB_INDEX_DATA * mob;
  int              count;

  PUSH_FUNCTION( "do_sale" );

  if ( !sale_list )
  {
    act( "對不起﹐$t沒有任何的拍賣品喔 ﹗", ch, mud_name, NULL, TO_CHAR );
    RETURN_NULL();
  }

  clear_buffer();
  send_to_buffer( "\e[1;33;44m順序 號 碼 售   價 拍賣所得 隱藏 次數 賣出 "
    "拍賣物品名稱                         \e[0m\n\r" );

  for ( count = 1, pSale = sale_list; pSale; pSale = pSale->next, count++ )
  {
    if ( !( obj = pSale->obj ) ) continue;

    send_to_buffer( "%3d. %5d %7d %8d  %s  %4d %4d %s(%s)\n\r"
      , count, obj->vnum, pSale->cost, pSale->gold, YESNO( pSale->visible )
      , pSale->times, pSale->sold, obj->short_descr, obj->name );
  }

  for ( count = 0, victim = char_list; victim; victim = victim->next )
  {
    if ( verify_char( victim )
      && ( mob = victim->pIndexData )
      && mob->auction
      && *mob->auction )
    {
      if ( ++count == 1 )
        send_to_buffer( "\n\r\e[1;33;44m順序 號 碼 拍賣怪物名稱           "
          "                                              \e[0m\n\r" );
      send_to_buffer( "%3d. %5d %s\n\r"
        , count, mob->vnum, mob_index_name( NULL, mob ) );
    }
  }

  if ( count == 0 ) send_to_buffer( "系統目前沒有任何的拍賣者﹗\n\r" );

  send_to_buffer( "\n\r拍賣機率﹕千分之 %d。\n\r上線人數下限﹕%d 人\n\r"
    , SaleChance, SaleLimit );

  print_buffer( ch );
  RETURN_NULL();
}

bool jail_someone( CHAR_DATA * ch, CHAR_DATA * victim, int dur, bool fPrint )
{
  PUSH_FUNCTION( "jail_someone" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "jail_someone: 來源錯誤." );
    RETURN( FALSE );
  }

  if ( dur <= 0 )
  {
    mudlog( LOG_DEBUG, "jail_someone: 時間不合理 %d.", dur );
    RETURN( FALSE );
  }

  if ( !RoomJail )
  {
    if ( ch && fPrint ) send_to_char( "系統中沒有天牢﹐所有動作取消。\n\r", ch );
    RETURN( FALSE );
  }

  if ( IS_NPC( victim ) )
  {
    if ( ch && fPrint )
      act( "$N那麼無辜﹐你幹嘛關它呢﹖", ch, NULL, victim, TO_CHAR );

    RETURN( FALSE );
  }

  if ( IS_IMMORTAL( victim ) )
  {
    if ( ch && fPrint )
      act( "你要關$N嗎﹖他是神耶﹐要開會通過吧﹗", ch, NULL, victim, TO_CHAR );

    RETURN( FALSE );
  }

  if ( victim->jail > 0 )
  {
    if ( ch && fPrint ) act( "$N已經在受刑囉﹗", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  if ( !victim->in_room )
  {
    if ( ch && fPrint ) act( "$N的房間有錯誤﹗", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  if ( victim->pcdata && victim->pcdata->corpse )
  {
    if ( ch && fPrint )
      act( "讓他撿完屍体再關吧﹗", ch, NULL, victim, TO_CHAR );

    RETURN( FALSE );
  }

  if ( victim->fighting ) stop_fighting( victim, TRUE );
  if ( victim->mount    ) unmount_char( victim, victim->mount    );
  if ( victim->mount_by ) unmount_char( victim->mount_by, victim );

  /* 把他的小鬼全部消掉 */
  if ( victim->spirit ) spirit_from_char( victim->spirit );

  /* 把他的傭兵全部拿掉 */
  if ( victim->mercenary ) all_mercenary_from_char( victim );

  /* 如果在思過中﹐則不用讓他思過 */
  victim->failed = 0;

  victim->jail = dur;

  /* 清除追蹤紀錄點 */
  clear_trace( victim, TRUE );

  send_to_char( "對不起﹐你犯了天條﹐你被監禁了﹗\n\r", victim );
  act( "$n犯了天條﹐被保安捉去關了﹗", victim, NULL, NULL, TO_ROOM );
  char_from_room( victim );
  char_to_room( victim, RoomJail );
  act( "$n犯了天條﹐從今天起就是你們的新伙伴了﹗", victim, NULL, NULL, TO_ROOM );
  do_look( victim, "auto" );

  if ( victim->pcdata ) victim->pcdata->imprison++;
  RETURN( TRUE );
}

FUNCTION( do_nuke )
{
  int                loop;
  char               arg[MAX_INPUT_LENGTH];
  char               exec_cmd[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA * man;
  CHAR_DATA       * victim;

  PUSH_FUNCTION( "do_nuke" );

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    send_to_char( "對不起﹐你要砍那個人的檔案呢﹖\n\r", ch );
    RETURN_NULL();
  }

   /* 轉換成小寫 */
   for ( loop = 0; arg[loop]; loop++ )
     if ( arg[loop] >= 'A' && arg[loop] <= 'Z' ) arg[loop] += 'a' - 'A';

  if ( imm_lookup( arg ) )
  {
    act( "$T 是神族成員之一﹐不能用這種方式來砍檔﹗", ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  if ( !is_exist( arg ) )
  {
    act( "$t裡沒有$T這一號人物喔﹗", ch, mud_name, arg, TO_CHAR );
    RETURN_NULL();
  }

  if ( ( victim = is_online( arg ) ) )
  {
    act( "對不起﹐$N正在線上﹐必須等$E離線才能砍檔﹗", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  for ( man = descriptor_list; man; man = man->next )
  {
    if ( verify_desc( man ) && !str_cmp( man->username, arg ) )
    {
      act( "對不起﹐$T 正在進入系統﹐無法砍檔﹗", ch, NULL, arg, TO_CHAR );
      RETURN_NULL();
    }
  }

  sprintf( exec_cmd, "rm -rf %s/%c/%s", player_dir, arg[0], arg );
  system( exec_cmd );

  /* 更正英雄榜 */
  check_hero( arg );

  /* 更正資料庫 */
  erase_database( arg );

  /* 更正幫派 */
  if ( name_from_club( arg, TRUE ) ) save_club();

  act( "你已經將 $t 這人從$T永久地移除了﹗", ch, arg, mud_name, TO_CHAR );
  RETURN_NULL();
}

FUNCTION( do_miset )
{
  MOB_INDEX_DATA * pIndex;
  char             arg1[MAX_INPUT_LENGTH];
  char             arg2[MAX_INPUT_LENGTH];
  char             arg3[MAX_INPUT_LENGTH];
  char             buf[MAX_STRING_LENGTH];
  char             buf1[MAX_STRING_LENGTH];
  char             mobname[MAX_STRING_LENGTH];
  CLASS_DATA     * pClass;
  int              value;
  int              vnum;

  PUSH_FUNCTION( "do_miset" );

  smash_tilde( argument );
  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );
  str_cpy( arg3, argument );

  if ( !arg1[0] || !arg2[0] || !arg3[0] )
  {
    send_to_char(
      "語法﹕miset <對象號碼> <項目> <數值>\n\r"
      "或是  miset <對象號碼> <字串> <數值>\n\r\n\r"
      "項目可以有下列選擇﹕\n\r"
      "  flags strength intelligence wisdom dexterity constitutin sex\n\r"
      "  class gold hp mana move aligment name short long dodge attack\n\r"
      "  tractable migrate multipile deadmsg auction descrption affect\n\r\n\r"
      , ch );

    RETURN_NULL();
  }

  if ( !is_number( arg1 ) )
  {
    send_to_char( "你必須註明怪物的號碼﹐才能更改﹗\n\r", ch );
    RETURN_NULL();
  }

  vnum = atoi( arg1 );
  if ( !( pIndex = get_mob_index( vnum ) ) )
  {
    act( "沒有號碼 $i 的怪物﹗", ch, &vnum, NULL, TO_CHAR );
    RETURN_NULL();
  }

  str_cpy( mobname, mob_index_name( NULL, pIndex ) );

  value = is_number( arg3 ) ? atoi( arg3 ) : -1;

  if ( !str_prefix( arg2, "flags" ) )
  {
    if ( !str_prefix( arg3, "sentinel" ) )
    {
      if ( IS_SET( pIndex->act, ACT_SENTINEL ) )
      {
        REMOVE_BIT( pIndex->act, ACT_SENTINEL );
        act( "$T不再是不可移動﹗", ch, NULL, mobname, TO_CHAR );
      }
      else
      {
        SET_BIT( pIndex->act, ACT_SENTINEL );
        act( "$T又恢復不可移動的性質﹗", ch, NULL, mobname, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "scavenger" ) )
    {
      if ( IS_SET( pIndex->act, ACT_SCAVENGER ) )
      {
        REMOVE_BIT( pIndex->act, ACT_SCAVENGER );
        act( "$T不再是會自動撿物品﹗", ch, NULL, mobname, TO_CHAR );
      }
      else
      {
        SET_BIT( pIndex->act, ACT_SCAVENGER );
        act( "$T又恢復會自動撿物品的性質﹗", ch, NULL, mobname, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "aggressive" ) )
    {
      if ( IS_SET( pIndex->act, ACT_AGGRESSIVE ) )
      {
        REMOVE_BIT( pIndex->act, ACT_AGGRESSIVE );
        act( "$T不再是會自動攻擊﹗", ch, NULL, mobname, TO_CHAR );
      }
      else
      {
        SET_BIT( pIndex->act, ACT_AGGRESSIVE );
        act( "$T又恢復會自動攻擊的性質﹗", ch, NULL, mobname, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "stayarea" ) )
    {
      if ( IS_SET( pIndex->act, ACT_STAY_AREA ) )
      {
        REMOVE_BIT( pIndex->act, ACT_STAY_AREA );
        act( "$T不再是會停留在本區域﹗", ch, NULL, mobname, TO_CHAR );
      }
      else
      {
        SET_BIT( pIndex->act, ACT_STAY_AREA );
        act( "$T又恢復會停留在本區域的性質﹗", ch, NULL, mobname, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "wimpy" ) )
    {
      if ( IS_SET( pIndex->act, ACT_WIMPY ) )
      {
        REMOVE_BIT( pIndex->act, ACT_WIMPY );
        act( "$T不再是會受傷逃跑﹗", ch, NULL, mobname, TO_CHAR );
      }
      else
      {
        SET_BIT( pIndex->act, ACT_WIMPY );
        act( "$T又恢復會受傷逃跑的性質﹗", ch, NULL, mobname, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "train" ) )
    {
      if ( IS_SET( pIndex->act, ACT_TRAIN ) )
      {
        REMOVE_BIT( pIndex->act, ACT_TRAIN );
        act( "$T不再是訓練屬性者﹗", ch, NULL, mobname, TO_CHAR );
      }
      else
      {
        SET_BIT( pIndex->act, ACT_TRAIN );
        act( "$T又恢復訓練屬性者的性質﹗", ch, NULL, mobname, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "rebirth" ) )
    {
      if ( IS_SET( pIndex->act, ACT_REBIRTH ) )
      {
        REMOVE_BIT( pIndex->act, ACT_REBIRTH );
        act( "$T不再是轉職者﹗", ch, NULL, mobname, TO_CHAR );
      }
      else
      {
        SET_BIT( pIndex->act, ACT_REBIRTH );
        act( "$T又恢復轉職者的性質﹗", ch, NULL, mobname, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "fight" ) )
    {
      if ( IS_SET( pIndex->act, ACT_REBORN_FIGHT ) )
      {
        REMOVE_BIT( pIndex->act, ACT_REBORN_FIGHT );
        act( "$T不再是會再生成其他怪物﹗", ch, NULL, mobname, TO_CHAR );
      }
      else
      {
        SET_BIT( pIndex->act, ACT_REBORN_FIGHT );
        act( "$T又恢復會再生成其他怪物的性質﹗", ch, NULL, mobname, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "ask" ) )
    {
      if ( IS_SET( pIndex->act, ACT_ASK ) )
      {
        REMOVE_BIT( pIndex->act, ACT_ASK );
        act( "$T不再是會叫人打工﹗", ch, NULL, mobname, TO_CHAR );
      }
      else
      {
        SET_BIT( pIndex->act, ACT_ASK );
        act( "$T又恢復會叫人打工的性質﹗", ch, NULL, mobname, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "noreborn" ) )
    {
      if ( IS_SET( pIndex->act, ACT_NOREBORN ) )
      {
        REMOVE_BIT( pIndex->act, ACT_NOREBORN );
        act( "$T不再是不會重生﹗", ch, NULL, mobname, TO_CHAR );
      }
      else
      {
        SET_BIT( pIndex->act, ACT_NOREBORN );
        act( "$T又恢復會不會重生的性質﹗", ch, NULL, mobname, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "nokill" ) )
    {
      if ( IS_SET( pIndex->act, ACT_NOKILL ) )
      {
        REMOVE_BIT( pIndex->act, ACT_NOKILL );
        act( "$T不再是不可殺害﹗", ch, NULL, mobname, TO_CHAR );
      }
      else
      {
        SET_BIT( pIndex->act, ACT_NOKILL );
        act( "$T又恢復不可殺害的性質﹗", ch, NULL, mobname, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "enroll" ) )
    {
      if ( IS_SET( pIndex->act, ACT_ENROLL ) )
      {
        REMOVE_BIT( pIndex->act, ACT_ENROLL );
        act( "$T不再是會記恨﹗", ch, NULL, mobname, TO_CHAR );
      }
      else
      {
        SET_BIT( pIndex->act, ACT_ENROLL );
        act( "$T又恢復記恨的性質﹗", ch, NULL, mobname, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "good" ) )
    {
      if ( IS_SET( pIndex->act, ACT_ALIGN_GOOD ) )
      {
        REMOVE_BIT( pIndex->act, ACT_ALIGN_GOOD );
        act( "$T不再是善良之人﹗", ch, NULL, mobname, TO_CHAR );
      }
      else
      {
        SET_BIT( pIndex->act, ACT_ALIGN_GOOD );
        act( "$T又恢復善良之人的性質﹗", ch, NULL, mobname, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "evil" ) )
    {
      if ( IS_SET( pIndex->act, ACT_ALIGN_EVIL ) )
      {
        REMOVE_BIT( pIndex->act, ACT_ALIGN_EVIL );
        act( "$T不再是邪惡之人﹗", ch, NULL, mobname, TO_CHAR );
      }
      else
      {
        SET_BIT( pIndex->act, ACT_ALIGN_EVIL );
        act( "$T又恢復邪惡之人的性質﹗", ch, NULL, mobname, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "nosummon" ) )
    {
      if ( IS_SET( pIndex->act,ACT_NOSUMMON ) )
      {
        REMOVE_BIT( pIndex->act, ACT_NOSUMMON );
        act( "$T不再是不可召喚之人﹗", ch, NULL, mobname, TO_CHAR );
      }
      else
      {
        SET_BIT( pIndex->act, ACT_NOSUMMON );
        act( "$T又恢復不可召喚之人的性質﹗", ch, NULL, mobname, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "speak" ) )
    {
      if ( pIndex->speak )
      {
        pIndex->speak = FALSE;
        act( "$T不再是不會講話之人﹗", ch, NULL, mobname, TO_CHAR );
      }
      else
      {
        pIndex->speak = TRUE;
        act( "$T又恢復會講話的性質﹗", ch, NULL, mobname, TO_CHAR );
      }
    }

    else
    {
      send_to_char(
        "人物的旗標只有 sentinel scavenger aggressive stayarea wimpy\n\r"
        "               train rebirth fight ask noreborn nokill nosummon\n\r"
        "               enroll good evil speak\n\r"
        , ch );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "strength" ) )
  {
    if ( value < 0 || value > MaxStr )
    {
      act( "只能改變力量由 0 到 $x。" , ch, &MaxStr, NULL, TO_CHAR );
    }

    else
    {
      pIndex->now_str = value;
      act( "你設定$T力量為 $x。", ch, &value, mobname, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "intelligence" ) )
  {

    if ( value < 0 || value > MaxInt )
    {
      act( "只能智力由 0 到 $x。", ch , &MaxInt, NULL, TO_CHAR );
    }

    else
    {
      pIndex->now_int = value;
      act( "你設定$T智力為 $x。", ch, &value, mobname, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "wisdom" ) )
  {
    if ( value < 0 || value > MaxWis )
    {
      act( "只能改變學識由 0 到 $x。", ch , &MaxWis, NULL, TO_CHAR );
    }

    else
    {
      pIndex->now_wis = value;
      act( "你設定$T學識為 $x。", ch, &value, mobname, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "dexterity" ) )
  {
    if ( value < 0 || value > MaxDex )
    {
      act( "只能改變敏捷由 0 到 $x。" , ch, &MaxDex, NULL, TO_CHAR );
    }

    else
    {
      pIndex->now_dex = value;
      act( "你設定$T敏捷為 $x。", ch, &value, mobname, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "constitutin" ) )
  {
    if ( value < 0 || value > MaxCon )
    {
      act( "只能改變體格由 0 到 $x。" , ch, &MaxCon, NULL, TO_CHAR );
    }

    else
    {
      pIndex->now_con = value;
      act( "你設定$T體格為 $x。", ch, &value, mobname, TO_CHAR );
    }

   RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "sex" ) )
  {
         if ( !str_prefix( arg3, "male"   ) ) value = SEX_MALE;
    else if ( !str_prefix( arg3, "female" ) ) value = SEX_FEMALE;

    switch( value )
    {
    default:
      print_to_char( ch, "性別只有 %s(%d) 和%s(%d)。\n\r"
        , sex_name( SEX_MALE )  , SEX_MALE
        , sex_name( SEX_FEMALE ), SEX_FEMALE );
      break;

    case SEX_FEMALE:
      pIndex->sex = SEX_FEMALE;
      act( "你把$T性別改成$t了。", ch, sex_name( pIndex->sex ), mobname, TO_CHAR );
      break;

    case SEX_MALE:
      pIndex->sex = SEX_MALE;
      act( "你把$T性別改成$t了。", ch, sex_name( pIndex->sex ), mobname, TO_CHAR );
      break;
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "class" ) )
  {
    if ( !( pClass = is_class( arg3, TRUE ) ) )
    {
      act( "職業 $2$t$0 名稱不合法。" , ch, arg3, NULL, TO_CHAR );
      RETURN_NULL();
    }

    pIndex->class = pClass;
    act( "你把$T職業改成$t了。", ch , class_name( pClass, TRUE ), mobname, TO_CHAR );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "gold" ) )
  {
    if ( value < 0 )
    {
      send_to_char( "不能改變金錢成負值。\n\r" , ch );
    }
    else
    {
      pIndex->gold = value;
      act( "你設定$T金錢為 $i 兩黃金。", ch, &value, mobname, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "hp" ) )
  {
    if ( value < 0 || value > MaxHit )
    {
      act( "生命力範圍由 0 到 $x。", ch, &MaxHit, NULL, TO_CHAR );
    }

    else
    {
      pIndex->hit = value;
      act( "你設定$T生命力為 $x。", ch, &value, mobname, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "mana" ) )
  {
    if ( value < 0 || value > MaxMana )
    {
      act( "法力範圍由 0 到 $x。", ch, &MaxMana, NULL, TO_CHAR );
    }

    else
    {
      pIndex->mana = value;
      act( "你設定$T法力為 $x。", ch, &value, mobname, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "move" ) )
  {
    if ( value < 0 || value > MaxMove )
    {
      act( "移動力範圍由 0 到 $x。", ch, &MaxMove, NULL, TO_CHAR );
    }

    else
    {
      pIndex->move = value;
      act( "你設定$T移動力為 $x。", ch, &value, mobname, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "alignment" ) )
  {
    if ( value < -AlignRange || value > AlignRange )
    {
      chinese_number( -AlignRange, buf  );
      chinese_number(  AlignRange, buf1 );

      act( "陣營範圍只能由 $t 到 $T。", ch, buf, buf1, TO_CHAR );
    }

    else
    {
      pIndex->alignment = value;
      act( "你設定$T陣營為 $x。", ch, &value, mobname, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "name" ) )
  {
    free_string( pIndex->name );
    pIndex->name = str_dup( arg3 );
    act( "設定$T英文名稱為 $t。", ch, arg3, mobname, TO_CHAR );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "short" ) )
  {
    free_string( pIndex->short_descr );
    pIndex->short_descr = str_dup( arg3 );
    act( "設定$T中文名稱為 $t。", ch, arg3, mobname, TO_CHAR );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "long" ) )
  {
    free_string( pIndex->long_descr );
    pIndex->long_descr = str_dup( arg3 );
    act( "設定$T長敘述為 $t。", ch, arg3, mobname, TO_CHAR );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "deadmsg" ) )
  {
    free_string( pIndex->deadmsg );
    pIndex->deadmsg = str_dup( arg3 );
    act( "設定$T死亡敘述為 $t。", ch, arg3, mobname, TO_CHAR );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "auction" ) )
  {
    free_string( pIndex->auction );
    pIndex->auction = str_dup( arg3 );
    act( "設定$T拍賣敘述為 $t。", ch, arg3, mobname, TO_CHAR );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "description" ) )
  {
    free_string( pIndex->description );
    pIndex->description = str_dup( arg3 );
    act( "設定$T長敘述為 $t。", ch, arg3, mobname, TO_CHAR );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "dodge" ) )
  {
    if ( value < 0 )
    {
      send_to_char( "不能改變閃躲率成負值。\n\r" , ch );
    }
    else
    {
      pIndex->dodge_ratio = value;
      act( "你設定$T閃躲率為百分之 $i。", ch, &value, mobname, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "attack" ) )
  {
    if ( value < 0 )
    {
      send_to_char( "不能改變攻擊率成負值。\n\r" , ch );
    }
    else
    {
      pIndex->attack_ratio = value;
      act( "你設定$T攻擊率為百分之 $i。", ch, &value, mobname, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "migrate" ) )
  {
    if ( value < 0 || value > 100 )
    {
      send_to_char( "遷移機率只能由 0 到 100。\n\r", ch );
    }
    else
    {
      pIndex->migrate = value;
      act( "你設定$T遷移率為百分之 $i。", ch, &value, mobname, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "tractable" ) )
  {
    if ( value < 0 || value > 100 )
    {
      send_to_char( "馴服機率只能由 0 到 100。\n\r", ch );
    }
    else
    {
      pIndex->tractable = value;
      act( "你設定$T馴服率為百分之 $i。", ch, &value, mobname, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "multipile" ) )
  {
    if ( value <= 0 || value > 1000 )
    {
      send_to_char( "經驗值加成只能由 1 到 1000。\n\r", ch );
    }
    else
    {
      pIndex->multipile = value;
      act( "你設定$T 經驗值加成為原先百分之$i。", ch, &value, mobname, TO_CHAR );
    }

    RETURN_NULL();
  }

  /* 如果都不對, 則出現錯誤訊息 */
  do_miset( ch, "" );
  RETURN_NULL();
}

FUNCTION( do_filename )
{
  char              arg[MAX_INPUT_LENGTH];
  OBJ_INDEX_DATA  * pObjIndex;
  ROOM_INDEX_DATA * pRoomIndex;
  MOB_INDEX_DATA  * pMobIndex;
  int               sn;
  int               len;

  PUSH_FUNCTION( "do_filename" );

  argument = one_argument( argument, arg );

  if ( !arg[0] || !argument[0] )
  {
    send_to_char( "請查詢 filename 的正確使用方法﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( !is_number( argument ) || ( sn = atoi( argument ) ) <= 0 )
  {
    send_to_char( "對不起﹐第二個參數必須是數字﹗\n\r", ch );
    RETURN_NULL();
  }

  len = str_len( home_dir );

  if ( !str_cmp( arg, "obj" ) )
  {
    if ( !( pObjIndex = get_obj_index( sn ) ) )
    {
      act( "對不起﹐找不到物品編號 $x 的原型﹗", ch, &sn, NULL, TO_CHAR );
      RETURN_NULL();
    }

    act( "編號 $x 物品的檔案路徑為 $2$T$0"
      , ch, &sn, pObjIndex->filename + len, TO_CHAR );
  }

  else if ( !str_cmp( arg, "roo" ) )
  {
    if ( !( pRoomIndex = get_room_index( sn ) ) )
    {
      act( "對不起﹐找不到房間編號 $x 的原型﹗", ch, &sn, NULL, TO_CHAR );
      RETURN_NULL();
    }

    act( "編號 $x 房間的檔案路徑為 $2$T$0"
      , ch, &sn, pRoomIndex->filename + len, TO_CHAR );
  }

  else if ( !str_cmp( arg, "mob" ) )
  {
    if ( !( pMobIndex = get_mob_index( sn ) ) )
    {
      act( "對不起﹐找不到怪物編號 $x 的原型﹗", ch, &sn, NULL, TO_CHAR );
      RETURN_NULL();
    }

    act( "編號 $x 的怪物檔案路徑為 $2$T$0"
      , ch, &sn, pMobIndex->filename + len, TO_CHAR );
  }

  else
  {
    send_to_char( "對不起﹐只能查詢 roo mob obj 三種格式﹗\n\r", ch );
  }

  RETURN_NULL();
}

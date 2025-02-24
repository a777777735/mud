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

FUNCTION( do_angel )
{
  ANGEL_DATA * pAngel;
  ANGEL_DATA * zAngel;
  char arg[MAX_INPUT_LENGTH];
  int  count;

  PUSH_FUNCTION( "do_angel" );

  if ( IS_NPC( ch ) || !ch->pcdata ) RETURN_NULL();

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    if ( AngelComing == FALSE && !IS_IMMORTAL( ch ) )
    {
      send_to_char( "對不起﹐守護神正在忙﹐稍後再請他出來好了﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !IS_SET( ch->act, PLR_ANGEL ) )
    {
      send_to_char( "你沒有把守護神選項打開﹐請輸入 config +angel 來啟動﹗\n\r", ch );
      RETURN_NULL();
    }

    act( "你設定守護神週期為$i秒﹐再$I秒後守護神會再來一次﹗"
      , ch, &ch->pcdata->angel_set, &ch->pcdata->angel_left, TO_CHAR );

    AngelComing = FALSE;
    count       = 0;
    zAngel      = NULL;

    for ( pAngel = angel_list; pAngel; pAngel = pAngel->next )
    {
      if ( ( pAngel->description && pAngel->description[0] )
        || ch->level > pAngel->higher
        || ch->level < pAngel->lower
        || ch->position != pAngel->position ) continue;

      if ( ( ( *pAngel->function ) ( ch, FALSE ) ) == TRUE
        && number_range( 0, count++ ) == 0 )
        zAngel = pAngel;
    }

    if ( zAngel )
    {
      ch->pcdata->angel_left = ch->pcdata->angel_set;

      print_to_char( ch, "\e[1;32m你的耳邊又傳來守護神的叮嚀﹕"
          "\e[0m%s\e[0m表現的不錯喔﹐繼續加油﹐我給你的建議是\n\r%s"
          , ch->cname, VERTICAL_LINE );

      ( *zAngel->function ) ( ch, TRUE );

      print_to_char( ch, "%s", VERTICAL_LINE );
    }
    else
    {
      send_to_char( "唉﹐你目前的狀況我也不知道要怎麼辦﹐請教玩家吧﹗\n\r", ch );
    }
    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "!set" ) )
  {
    one_argument( argument, arg );
    if ( arg[0] == '\x0' || !is_number( arg ) )
    {
      send_to_char( "你必須輸入數字來代表守護神出現的週期﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( ( count = atoi( arg ) ) < 10 )
    {
      send_to_char( "這種週期守護神是會累死的﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( count > 300 )
    {
      send_to_char( "總不能讓守護神等太久吧﹗\n\r", ch );
      RETURN_NULL();
    }

    ch->pcdata->angel_left = ch->pcdata->angel_set = count;
    act( "你設定每$i秒守護神會出現一次﹗", ch, &count, NULL, TO_CHAR );

    if ( !IS_SET( ch->act, PLR_ANGEL ) )
    {
      SET_BIT( ch->act, PLR_ANGEL );
      send_to_char( "對了﹐順便幫你把守護神選項打開了﹗\n\r", ch );
    }
  }

  else
  {
    send_to_char( "你的語法不對﹐請查詢 angel﹐謝謝﹗\n\r", ch );
  }

  RETURN_NULL();
}

void angel_update( void )
{
  DESCRIPTOR_DATA * man;
  ANGEL_DATA      * pAngel;
  ANGEL_DATA      * zAngel;
  CHAR_DATA       * ch;
  int               count;

  PUSH_FUNCTION( "angel_update" );

  for ( man = descriptor_list; man; man = man->next )
  {
    if ( verify_desc( man )
      && man->connected == CON_PLAYING
      && verify_char( ( ch = man->character ) )
      && ch->pcdata
      && IS_SET( ch->act, PLR_ANGEL ) )
    {
      /* 設定時間 */
      if ( --ch->pcdata->angel_left > 0 ) continue;
      ch->pcdata->angel_left = ch->pcdata->angel_set;

      zAngel = NULL;
      count  = 0;

      for ( pAngel = angel_list; pAngel; pAngel = pAngel->next )
      {
        if ( pAngel->function
          || ch->level > pAngel->higher
          || ch->level < pAngel->lower
          || ch->position != pAngel->position ) continue;

        if ( number_range( 0, count++ ) == 0 ) zAngel = pAngel;
      }

      if ( zAngel )
      {
        print_to_char( ch, "\e[1;32m你的耳邊又傳來守護神的叮嚀﹕"
          "\e[0m%s\e[0m表現的不錯喔﹐繼續加油﹐我給你的建議是\n\r%s"
          "\e[0m%s\e[0m%s"
          , ch->cname, VERTICAL_LINE, zAngel->description, VERTICAL_LINE );
      }
    }
  }
  RETURN_NULL();
}

bool angel_dead( CHAR_DATA * ch, bool fPrint )
{
  PUSH_FUNCTION( "angel_dead" );
  RETURN( FALSE );
}

ANGEL( angel_mount )
{
  PUSH_FUNCTION( "angel_mount" );

  if ( !ch->mount && !ch->mount_by )
  {
    if ( fPrint ) send_to_char(
       "你是不是可以考慮找一匹馬來騎騎(\e[1;32mmount\[0m)﹖"
       "你覺得如何呢﹖\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_unmount )
{
  PUSH_FUNCTION( "angel_unmount" );

  if ( ch->mount )
  {
    if ( fPrint ) send_to_char(
      "你正騎著一匹馬﹐不錯﹐下馬請輸入 \e[1;32munmount\e[0m 。\n\r", ch );

    RETURN( TRUE );
  }
  RETURN( FALSE );
}

ANGEL( angel_sleep )
{
  PUSH_FUNCTION( "angel_sleep" );

  if ( ch->hit  < get_curr_hit( ch )  / 3
    || ch->mana < get_curr_mana( ch ) / 3
    || ch->move < get_curr_move( ch ) / 3 )
  {
    if ( fPrint ) send_to_char(
      "看你受傷很嚴重﹐躺著睡覺(\e[1;32msleep\e[0m)或是"
      "休息(\e[1;32mrest\e[0m)好嗎﹖\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_drink )
{
  PUSH_FUNCTION( "angel_drink" );

  if ( ch->pcdata && ch->pcdata->condition[COND_THIRST] <= 0 )
  {
    if ( fPrint ) send_to_char(
      "看你好像很口渴的樣子﹐去找水井(\e[1;32mwell\e[0m)或是買"
      "水袋來喝(\e[1;32mdrink\e[0m)。\n\r", ch );
    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_eat )
{
  PUSH_FUNCTION( "angel_eat" );

  if ( ch->pcdata && ch->pcdata->condition[COND_FULL] <= 0 )
  {
    if ( fPrint ) send_to_char(
      "看你好像很餓的樣子﹐去買一些食物來吃(\e[1;32meat\e[0m)吧。\n\r", ch );
    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_marry )
{
  PUSH_FUNCTION( "angel_marry" );

  if ( get_age( ch ) > 20 && ch->pcdata && ch->pcdata->wooer )
  {
    if ( fPrint ) send_to_char(
      "有了追求者﹐你可以請線上的大神幫你們證婚吧﹗\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_wedding )
{
  PUSH_FUNCTION( "angel_wedding" );

  if ( get_age( ch ) > 20 && ch->pcdata && !ch->pcdata->wooer )
  {
    if ( fPrint ) send_to_char(
      "你年紀也不小了﹐是不是可以考慮結婚(\e[1;32mmarry\e[0m)了呢﹖\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_alias )
{
  PUSH_FUNCTION( "angel_alias" );

  if ( get_alias_count( ch ) <= 2 )
  {
    if ( fPrint ) send_to_char(
      "看來你不太會使用巨集指令(\e[1;32malias\e[0m)﹐不妨試試看。\n\r", ch );

    RETURN( TRUE );
  }
  RETURN( FALSE );
}

ANGEL( angel_auction )
{
  PUSH_FUNCTION( "angel_auction" );

  if ( ch->carry_number >= 20 )
  {
    if ( fPrint ) send_to_char(
      "哇﹐身上那麼多東西﹐拿幾件出來拍賣(\e[1;32mauction\e[0m)"
      "看看嘛﹗\n\r", ch );

    RETURN( TRUE );
  }
  RETURN( FALSE );
}

ANGEL( angel_bet )
{
  PUSH_FUNCTION( "angel_bet" );

  if ( auction_info->obj
    && ch->gold > auction_info->price
    && auction_info->obj->level <= ch->level )
  {
    if ( fPrint ) send_to_char(
      "你可以考慮買下(\e[1;32mbet\e[0m)現在拍賣的東西﹐"
      "或是看看也好(\e[1;32mbet help\e[0m)﹗\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_areas )
{
  PUSH_FUNCTION( "angel_areas" );

  if ( ch->played <= 60 * 30 )
  {
    if ( fPrint ) send_to_char(
      "你好﹐新手﹐可以查詢區域資料(\e[1;32mareas\e[0m)﹐"
      "之後坐馬車(\e[1;32mbus\e[0m)來逛逛吧﹗\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_xname )
{
  PUSH_FUNCTION( "angel_xname" );

  if ( ch->pcdata && ch->pcdata->xname >= 5 )
  {
    if ( fPrint ) send_to_char(
      "看來你常常使用不雅字喔﹐提醒你﹐這可是不好的習慣喔﹗\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_immortal )
{
  PUSH_FUNCTION( "angel_immortal" );

  if ( IS_IMMORTAL( ch ) )
  {
    if ( fPrint ) send_to_char( "當了大神了﹐還在混﹐去寫區域啦﹗\n\r", ch );
    RETURN( TRUE );
  }
  RETURN( FALSE );
}

ANGEL( angel_deposit_gold )
{
  PUSH_FUNCTION( "angel_deposit_gold" );

  if ( ch->gold > 50000 )
  {
    if ( fPrint ) send_to_char(
      "正所謂財不露白﹐趕緊去把錢存(\e[1;32mdeposit\e[0m)起來吧﹗\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_deposit_obj )
{
  PUSH_FUNCTION( "angel_deposit_obj" );

  if ( ch->carry_number >= can_carry_n( ch ) - 2 )
  {
    if ( fPrint ) send_to_char(
      "哇﹐你身上的裝備可不少啊﹐趕緊去存(\e[1;32mdeposit\e[0m)"
      "起來吧﹗\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_stock )
{
  PUSH_FUNCTION( "angel_stock" );

  if ( ch->bank > 1000000 )
  {
    if ( fPrint ) send_to_char(
      "看來你的銀行存款不少喔﹐考慮投資股票(\e[1;32mstock\e[0m)怎樣﹖"
      "\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_gamble )
{
  PUSH_FUNCTION( "angel_gamble" );

  if ( ch->gold > 1000000 )
  {
    if ( fPrint ) send_to_char(
      "看你麥克麥克的樣子﹐來賭博(\e[1;32mgamble\e[0m)試試手氣嘛﹗\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_direction )
{
  EXIT_DATA * pexit;
  int         door;
  int         count;
  int         rem;

  PUSH_FUNCTION( "angel_direction" );

  if ( !ch->in_room ) RETURN( FALSE );

  for ( rem = ERRORCODE, count = door = 0; door < DIR_MAX; door++ )
  {
    if ( ( pexit = ch->in_room->exit[door] )
      && pexit->to_room
      && pexit->closed
      && number_range( 0, count++ ) == 0 )
      rem = door;
  }

  if ( rem != ERRORCODE )
  {
    if ( fPrint ) act( "你的$t方好像有出口﹐不妨去冒險看看﹗"
      , ch, direction_name( rem ), NULL, TO_CHAR );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_shop )
{
  CHAR_DATA * keeper;

  PUSH_FUNCTION( "angel_shop" );

  if ( !ch->in_room ) RETURN( FALSE );

  for ( keeper = ch->in_room->people; keeper; keeper = keeper->next_in_room )
  {
    if ( IS_NPC( keeper )
      && keeper->pIndexData
      && keeper->pIndexData->pShop
      && keeper->pIndexData->pShop->type == SHOP_STORE )
    {
      if ( fPrint ) send_to_char(
        "你這裡好像有商人耶﹐不妨買(\e[1;32mbuy\e[0m)個東西"
        "﹐看看(\e[1;32mlist\e[0m)也好﹗\n\r", ch );

      RETURN( TRUE );
    }
  }
  RETURN( FALSE );
}

ANGEL( angel_smith )
{
  CHAR_DATA * keeper;

  PUSH_FUNCTION( "angel_smith" );

  if ( !ch->in_room ) RETURN( FALSE );

  for ( keeper = ch->in_room->people; keeper; keeper = keeper->next_in_room )
  {
    if ( IS_NPC( keeper )
      && keeper->pIndexData
      && keeper->pIndexData->pShop
      && keeper->pIndexData->pShop->type == SHOP_SMITH )
    {
      if ( fPrint ) send_to_char(
        "你這裡好像有鐵匠耶﹐不妨修理(\e[1;32mrepair\e[0m)"
        "個東西看看﹗\n\r", ch );

      RETURN( TRUE );
    }
  }
  RETURN( FALSE );
}

ANGEL( angel_mercenary )
{
  CHAR_DATA * keeper;

  PUSH_FUNCTION( "angel_mercenary" );

  if ( !ch->in_room ) RETURN( FALSE );

  for ( keeper = ch->in_room->people; keeper; keeper = keeper->next_in_room )
  {
    if ( IS_NPC( keeper )
      && keeper->pIndexData
      && keeper->pIndexData->pShop
      && keeper->pIndexData->pShop->type == SHOP_MERCENARY )
    {
      if ( fPrint ) send_to_char(
        "這裡有個傭兵所﹐不妨雇請(\e[1;32mhire\e[0m)傭兵來幫忙﹗\n\r", ch );

      RETURN( TRUE );
    }
  }
  RETURN( FALSE );
}

ANGEL( angel_coper )
{
  CHAR_DATA * keeper;

  PUSH_FUNCTION( "angel_coper" );

  if ( !ch->in_room ) RETURN( FALSE );

  for ( keeper = ch->in_room->people; keeper; keeper = keeper->next_in_room )
  {
    if ( IS_NPC( keeper )
      && keeper->pIndexData
      && keeper->pIndexData->pShop
      && keeper->pIndexData->pShop->type == SHOP_COPER )
    {
      if ( fPrint ) send_to_char(
        "這裡有馬販﹐你可以跟他買(\e[1;32mtrade\e[0m)匹馬騎騎看﹗\n\r", ch );

      RETURN( TRUE );
    }
  }
  RETURN( FALSE );
}

ANGEL( angel_kill )
{
  CHAR_DATA * victim;

  PUSH_FUNCTION( "angel_kill" );

  if ( !ch->in_room ) RETURN( FALSE );

  for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
  {
    if ( IS_NPC( victim )
      && abs( victim->level - ch->level ) <= 3 )
    {
      if ( fPrint ) act( "你這裡有個$N﹐宰掉($2kill$0)應該可以有不少經驗值喔﹗"
        , ch, NULL, victim, TO_CHAR );

      RETURN( TRUE );
    }
  }

  RETURN( FALSE );
}

ANGEL( angel_bank )
{
  PUSH_FUNCTION( "angel_bank" );

  if ( ch->in_room && ch->in_room->DepositMoney )
  {
    if ( fPrint ) send_to_char(
      "這裡剛好有個錢莊﹐不妨存錢(\e[1;32mdeposit\e[0m)"
      "或是查詢(\e[1;32mcheck\e[0m)存款﹗\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_obj_bank )
{
  PUSH_FUNCTION( "angel_obj_bank" );

  if ( ch->in_room && ch->in_room->StoreRoom )
  {
    if ( fPrint ) send_to_char(
      "這裡剛好有間鏢局﹐不妨存(\e[1;32mdeposit\e[0m)物品﹐"
      "或是查詢(\e[1;32mcheck\e[0m)看看。\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_get )
{
  OBJ_DATA * obj;

  PUSH_FUNCTION( "angel_get" );

  if ( !ch->in_room ) RETURN( FALSE );

  for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
  {
    if ( can_see_obj( ch, obj )
      && obj->Takeable
      && obj->item_type != ITEM_CORPSE_PC
      && obj->item_type != ITEM_CORPSE_NPC
      && obj->item_type != ITEM_TRASH )
    {
      if ( fPrint ) send_to_char(
        "地上好像有東西﹐你可以試著把它給撿(\e[1;32mget\e[0m)起來﹗\n\r", ch );

      RETURN( TRUE );
    }
  }

  RETURN( FALSE );
}

ANGEL( angel_drop )
{
  OBJ_DATA * obj;

  PUSH_FUNCTION( "angel_drop" );

  if ( !ch->in_room ) RETURN( FALSE );

  for ( obj = ch->carrying; obj; obj = obj->next_content )
  {
    if ( can_see_obj( ch, obj ) && obj->item_type == ITEM_TRASH )
    {
      if ( fPrint ) send_to_char(
        "你身上好像有些垃圾﹐最好把它給丟掉(\e[1;32mdrop\e[0m)。\n\r", ch );

      RETURN( TRUE );
    }
  }

  RETURN( FALSE );
}

ANGEL( angel_withdraw )
{
  PUSH_FUNCTION( "angel_withdraw" );

  if ( ch->deposit )
  {
    if ( fPrint ) send_to_char(
      "你在鏢局裡有存些東西﹐別忘了要把它給領(\e[1;32mwithdraw\e[0m)出來喔。\n\r", ch );

    RETURN( TRUE );
  }
  RETURN( FALSE );
}

ANGEL( angel_call )
{
  OBJ_DATA * obj;

  PUSH_FUNCTION( "angel_call" );

  if ( !ch->in_room ) RETURN( FALSE );

  for ( obj = ch->carrying; obj; obj = obj->next_content )
  {
    if ( can_see_obj( ch, obj ) && obj->item_type == ITEM_SPIRITJADE )
    {
      if ( fPrint ) send_to_char(
        "你身上有顆鬼玉耶﹐不妨召喚(\e[1;32mcall\e[0m)出來看看。\n\r", ch );

      RETURN( TRUE );
    }
  }

  RETURN( FALSE );
}

ANGEL( angel_bus )
{
  PUSH_FUNCTION( "angel_bus" );

  if ( ch->in_room && is_platform( ch->in_room ) )
  {
    if ( fPrint ) send_to_char(
      "你目前正在地鐵站裡﹐可以查詢(\e[1;32mbus\e[0m)看看你的車來了嗎﹖\n\r", ch );

    RETURN( TRUE );
  }
  RETURN( FALSE );
}

ANGEL( angel_ship )
{
  SHIP_DATA * pShip;

  PUSH_FUNCTION( "angel_shop" );

  if ( !ch->in_room ) RETURN( FALSE );

  for ( pShip = ship_list; pShip; pShip = pShip->next )
  {
    if ( pShip->starting == ch->in_room )
    {
      if ( fPrint ) send_to_char(
        "你目前正在搭船的起始點上﹐可以查詢(\e[1;32mship\e[0m)看看你"
        "的船來了沒﹖\n\r", ch );

      RETURN( TRUE );
    }
  }

  RETURN( FALSE );
}

ANGEL( angel_lore )
{
  CHAR_DATA * victim;

  PUSH_FUNCTION( "angel_lore" );

  if ( !ch->in_room ) RETURN( FALSE );

  for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
  {
    if ( IS_NPC( victim ) )
    {
      if ( fPrint ) send_to_char(
        "這裡有怪物可以打﹐但是為了小心﹐還是先評量"
        "(\e[1;32mlore\e[0m)看看吧﹗\n\r", ch );
      RETURN( TRUE );
    }
  }

  RETURN( FALSE );
}

ANGEL( angel_badman )
{
  CHAR_DATA * victim;

  PUSH_FUNCTION( "angel_badman" );

  if ( !ch->in_room ) RETURN( FALSE );

  for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
  {
    if ( IS_NPC( victim ) && victim->level - ch->level >= 10 )
    {
      if ( fPrint ) send_to_char(
        "這裡有隻很強的怪物﹐還是離他遠一點吧﹗\n\r", ch );

      RETURN( TRUE );
    }
  }


  RETURN( FALSE );
}

ANGEL( angel_wake )
{
  PUSH_FUNCTION( "angel_wake" );

  if ( fPrint ) send_to_char(
    "你正在休息或是睡覺﹐不錯﹐記得要起床"
    "(\e[1;32mwake\e[0m 或 \e[1;32mstand\e[0m)喔﹗\n\r", ch );

  RETURN( TRUE );
}

ANGEL( angel_lotto )
{
  PUSH_FUNCTION( "angel_lotto" );

  if ( !IS_SET( ch->turn, PLR_LOTTO ) )
  {
    if ( fPrint ) send_to_char(
      "看你還沒有訂(\e[1;32mturn\e[0m)樂透快報喔﹐訂訂看﹐"
      "中了可是有免死金牌喔。\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_hero )
{
  PUSH_FUNCTION( "angel_hero_1" );

  if ( ch->level >= ( LEVEL_IMMORTAL - 3 ) )
  {
    if ( fPrint ) send_to_char(
      "努力一點喔﹐就快升到英雄(\e[1;32mhero\e[0m)了﹐加油加油﹗\n\r", ch );
    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_autosac )
{
  PUSH_FUNCTION( "angel_autosac" );

  if ( !IS_SET( ch->act, PLR_AUTOSAC ) )
  {
    if ( fPrint ) send_to_char(
      "為了避免屍橫遍野﹐請打開 \e[1;32mautosac\e[0m 吧﹗\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_exact )
{
  PUSH_FUNCTION( "angel_exact" );

  if ( !IS_SET( ch->act, PLR_EXACT ) )
  {
    if ( fPrint ) send_to_char(
      "你常常覺得只看到中文名稱而沒有英文名稱"
      "而麻煩嗎﹖試試 \e[1;32mconfig +exact\e[0m\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_message )
{
  PUSH_FUNCTION( "angel_message" );

  if ( IS_SET( ch->act, PLR_MESSAGE ) )
  {
    if ( fPrint ) send_to_char(
      "如果你覺得網路很慢﹐你可以關掉戰鬥"
      "敘述(\e[1;32mconfig -message\e[0m)。\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_config_flee )
{
  PUSH_FUNCTION( "angel_config_flee" );

  if ( !IS_SET( ch->act, PLR_FLEE ) )
  {
    if ( fPrint ) send_to_char(
      "如果你常常戰鬥斷線﹐請設定 \e[1;32mconfig +flee\e[0m。\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_autoloot )
{
  PUSH_FUNCTION( "angel_autoloot" );

  if ( !IS_SET( ch->act, PLR_AUTOLOOT ) )
  {
    if ( fPrint ) send_to_char(
      "為了得到更多戰利品﹐考慮打開 \e[1;32mautoloot\e[0m 吧﹗\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_friend )
{
  PUSH_FUNCTION( "angel_friend" );

  if ( friend_count( ch ) <= 0 )
  {
    if ( fPrint ) send_to_char(
      "你可以設定你的好友名單(\e[1;32mfriend\e[0m)﹐來通知"
      "你他是否上線。\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_wimpy )
{
  PUSH_FUNCTION( "angel_wimpy" );

  if ( ch->wimpy * 5 <= get_curr_hit( ch ) )
  {
    if ( fPrint ) send_to_char(
      "為了你的身家安全﹐請把逃跑點數(\e[1;32mwimpy\e[0m)調高"
      "一點吧﹗\n\r", ch );

    RETURN( TRUE );
  }
  RETURN( FALSE );
}

ANGEL( angel_enable )
{
  PUSH_FUNCTION( "angel_enabel" );

  if ( enable_count( ch ) <= 3 )
  {
    if ( fPrint ) send_to_char(
      "你的致能(\e[1;32menable\e[0m)太少了喔﹐注意一下喔﹐"
      "要有致能(\e[1;32menable\e[0m)﹐系統才會幫你出招喔。\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_flee )
{
  ENABLE_DATA * pEnable;
  SKILL_DATA  * pSkill;

  PUSH_FUNCTION( "angel_flee" );

  for ( pEnable = ch->enable; pEnable; pEnable = pEnable->next )
  {
    if ( ( pSkill = pEnable->skill ) && pSkill->type == TAR_FLEE )
      RETURN( FALSE );
  }

  if ( fPrint ) send_to_char(
    "你沒有致能(\e[1;32menable\e[0m)逃跑的技能喔﹐這樣很危險耶﹗\n\r", ch );

  RETURN( TRUE );
}

ANGEL( angel_attack )
{
  ENABLE_DATA * pEnable;
  SKILL_DATA  * pSkill;

  PUSH_FUNCTION( "angel_attack" );

  for ( pEnable = ch->enable; pEnable; pEnable = pEnable->next )
  {
    if ( ( pSkill = pEnable->skill ) && pSkill->type == TAR_CHAR_OFFENSIVE )
      RETURN( FALSE );
  }

  if ( fPrint ) send_to_char(
    "你沒有致能(\e[1;32menable\e[0m)攻擊類的技能喔﹐"
    "這樣不是很危險嗎﹖\n\r", ch );

  RETURN( TRUE );
}

ANGEL( angel_meditation )
{
  ENABLE_DATA * pEnable;
  SKILL_DATA  * pSkill;

  PUSH_FUNCTION( "angel_meditation" );

  for ( pEnable = ch->enable; pEnable; pEnable = pEnable->next )
  {
    if ( ( pSkill = pEnable->skill ) && pSkill->type == TAR_MEDITATION )
      RETURN( FALSE );
  }

  if ( fPrint ) send_to_char(
    "你沒有致能(\e[1;32menable\e[0m)冥想類的技能喔﹐"
    "趕快去學(\e[1;32mlearn\e[0m)一個來吧﹗\n\r", ch );

  RETURN( TRUE );
}

ANGEL( angel_description )
{
  PUSH_FUNCTION( "angel_description" );

  if ( !ch->description || !*ch->description )
  {
    if ( fPrint ) send_to_char(
      "你沒有描述(\e[1;32mdescription\e[0m)﹐"
      "趕快去編輯一個來看看吧﹗\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_club )
{
  PUSH_FUNCTION( "angel_club" );

  if ( !ch->club )
  {
    if ( fPrint ) send_to_char(
      "你好像還沒有加入幫派(\e[1;32mclub\e[0m)﹐考慮參加看看吧﹗\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_group )
{
  PUSH_FUNCTION( "angel_group" );

  if ( ch->master || ch->leader )
  {
    if ( fPrint ) send_to_char(
      "團結就是力量﹐你可以用 \e[1;32mgtell\e[0m 來在團體內講話。\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_split )
{
  PUSH_FUNCTION( "angel_split" );

  if ( ch->master || ch->leader )
  {
    if ( fPrint ) send_to_char(
      "既然組隊(\e[1;32mgroup\e[0m)了﹐打死怪物的銀子最"
      "好能平分(\e[1;32msplit\e[0m)。\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_nogroup )
{
  PUSH_FUNCTION( "angel_nogroup" );

  if ( !ch->master || !ch->leader )
  {
    if ( fPrint ) send_to_char(
      "正所謂團結就是力量﹐考慮組隊(\e[1;32mgroup\e[0m)吧﹗\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_reply )
{
  PUSH_FUNCTION( "angel_reply" );

  if ( ch->reply )
  {
    if ( fPrint ) send_to_char(
      "你知道回答別人的話只須用 \e[1;32mreply\e[0m﹐"
      "不必再用 \e[1;32mtell\e[0m 了。\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_fountain )
{
  OBJ_DATA * obj;

  PUSH_FUNCTION( "angel_fountain" );

  if ( !ch->in_room ) RETURN( FALSE );

  for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
  {
    if ( obj->item_type == ITEM_FOUNTAIN )
    {
      if ( fPrint ) send_to_char(
        "這裡剛好有個水池﹐喝喝(\e[1;32mdrink\e[0m)看吧﹗\n\r", ch );

      RETURN( TRUE );
    }
  }

  RETURN( FALSE  );
}

ANGEL( angel_food )
{
  OBJ_DATA * obj;

  PUSH_FUNCTION( "angel_fountain" );

  if ( !ch->in_room ) RETURN( FALSE );

  for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
  {
    if ( obj->item_type == ITEM_FOOD )
    {
      if ( fPrint ) send_to_char(
        "這裡剛好有些食物﹐趕快撿(\e[1;32mget\e[0m)起來﹐"
        "吃吃(\e[1;32meat\e[0m)看吧﹗\n\r", ch );

      RETURN( TRUE );
    }
  }

  RETURN( FALSE  );
}

ANGEL( angel_gambleset )
{
  PUSH_FUNCTION( "angel_gambleset" );

  if ( IS_SET( ch->deaf , CHANNEL_GAMBLE ) )
  {
    if ( fPrint ) send_to_char(
      "你是不是覺得有時候賭博頻道(\e[1;32mchannel\e[0m)還有點吵﹐"
      "你可以用 \e[1;32mchannel -gamble\e[0m 關掉它喔。\n\r", ch );

    RETURN( TRUE );
  }
  RETURN( FALSE );
}

ANGEL( angel_weather )
{
  PUSH_FUNCTION( "angel_weather" );

  if ( IS_SET( ch->deaf , CHANNEL_WEATHER ) )
  {
    if ( fPrint ) send_to_char(
      "你是不是覺得天氣頻道對你沒有用呢﹖你可以用 "
      "\e[1;32mchannel -weather\e[0m 關掉它喔。\n\r", ch );

    RETURN( TRUE );
  }
  RETURN( FALSE );
}

ANGEL( angel_repair )
{
  OBJ_DATA   * obj;
  SITUS_DATA * pSitus;

  PUSH_FUNCTION( "angel_repair" );

  for ( pSitus = situs_list; pSitus; pSitus = pSitus->next )
  {
    if ( ( obj = get_eq_char( ch, pSitus->location ) )
      && obj->max_armor > 100
      && obj->armor * 2 <= obj->max_armor )
    {
      if ( fPrint ) send_to_char(
        "你身上有些裝備快要壞了﹐趕緊找鐵匠或是鑄劍師"
        "修理(\e[1;32mrepair\e[0m)吧﹗\n\r", ch );

      RETURN( TRUE );
    }
  }

  RETURN( FALSE );
}

ANGEL( angel_crave )
{
  PUSH_FUNCTION( "angel_crave" );

  if ( ch->pcdata && ch->pcdata->corpse && ch->pcdata->corpse->in_room )
  {
    if ( fPrint ) send_to_char(
      "還等什麼等﹖趕快取回(\e[1;32mcrave\e[0m)屍體吧﹗\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_search )
{
  PUSH_FUNCTION( "angel_search" );

  if ( ch->pcdata && ch->pcdata->corpse && ch->pcdata->corpse->in_room )
  {
    if ( fPrint ) send_to_char(
      "還等什麼等﹖趕快搜尋(\e[1;32msearch\e[0m)你的屍體吧﹗\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_sac )
{
  PUSH_FUNCTION( "angel_sac" );

  if ( ch->pcdata && ch->pcdata->corpse && ch->pcdata->corpse->in_room )
  {
    if ( fPrint ) send_to_char(
      "趕快從你的屍體裡撿回所有東西(\e[1;32mget all corpse\e[0m)﹐"
      "就可以還陽(\e[1;32msac corpse\e[0m)。\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_wear )
{
  SITUS_DATA * pSitus;

  PUSH_FUNCTION( "angel_wear" );

  for ( pSitus = situs_list; pSitus; pSitus = pSitus->next )
    if ( get_eq_char( ch, pSitus->location ) ) RETURN( FALSE );

  if ( fPrint ) send_to_char(
    "你都不穿(\e[1;32mwear\e[0m)裝備的喔﹐小心著涼喔﹗\n\r", ch );

  RETURN( TRUE );
}

ANGEL( angel_study )
{
  OBJ_DATA * obj;

  PUSH_FUNCTION( "angel_study" );

  for ( obj = ch->carrying; obj; obj = obj->next_content )
  {
    if ( can_see_obj( ch, obj ) && obj->item_type == ITEM_MYSTERY )
    {
      if ( fPrint ) send_to_char(
        "喔﹐你身上有本秘笈﹐趕快拿來讀讀看(\e[1;32mstudy\e[0m)﹗\n\r", ch );

      RETURN( TRUE );
    }
  }

  RETURN( FALSE );
}

ANGEL( angel_learn )
{
  PUSH_FUNCTION( "angel_learn" );

  if ( learn_count( ch ) <= 5 )
  {
    if ( fPrint ) send_to_char(
      "你是不是應該出去冒險多多學習(\e[1;32mlearn\e[0m)一些"
      "技能(\e[1;32mskill\e[0m)呢﹖\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_recall )
{
  PUSH_FUNCTION( "angel_recall" );

  if ( ch->in_room && ch->in_room->Memorize )
  {
    if ( fPrint ) send_to_char(
      "看來這裡可以成為傳送點﹐設定(\e[1;32mrecall\e[0m)他吧。\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_recallset )
{
  int loop;

  PUSH_FUNCTION( "angel_recallset" );

  if ( !ch->pcdata ) RETURN( FALSE );

  for ( loop = 0; loop < MAX_RECALL; loop++ )
    if ( ch->pcdata->recall[loop] > 0 ) RETURN( FALSE );

  if ( fPrint ) send_to_char(
    "看來你一個傳送點都沒有設定﹐設定(\e[1;32mrecall\e[0m)一下吧﹗\n\r", ch );

  RETURN( TRUE );
}

ANGEL( angel_train )
{
  PUSH_FUNCTION( "angel_train" );

  if ( ch->practice >= get_str_train( ch )
    || ch->practice >= get_int_train( ch )
    || ch->practice >= get_wis_train( ch )
    || ch->practice >= get_dex_train( ch )
    || ch->practice >= get_con_train( ch ) )
  {
    if ( fPrint ) send_to_char(
      "有了訓練點數﹐不要浪費﹐趕緊去訓練(\e[1;32mtrain\e[0m)吧﹗\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_forsake )
{
  PUSH_FUNCTION( "angel_forsake" );

  if ( ch->class && learn_count( ch ) >= ( get_learnable( ch ) - 2 ) )
  {
    if ( fPrint ) send_to_char(
      "技能練了那麼多﹐是不是可以自廢一些"
      "沒用的武功\(\e[1;32mforsake\e[0m)呢﹖\n\r", ch );

    RETURN( TRUE );
  }
  RETURN( FALSE );
}

ANGEL( angel_enemy )
{
  PUSH_FUNCTION( "angel_enemy" );

  if ( ch->enemy )
  {
    if ( fPrint ) send_to_char(
      "你可是某個城的公敵﹐冒險時請你多多小心。\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_blind )
{
  PUSH_FUNCTION( "angel_blind" );

  if ( is_affected( ch, SLOT_BLINDNESS ) )
  {
    if ( fPrint ) send_to_char(
      "你的眼睛已經瞎了﹐趕緊找醫生去醫治喔﹗聽說洛陽的醫生不錯喔﹗\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_poison )
{
  PUSH_FUNCTION( "angel_poison" );

  if ( is_affected( ch, SLOT_POISON ) )
  {
    if ( fPrint ) send_to_char(
      "你已經中毒了﹐趕緊找醫生去醫治喔﹗聽說洛陽的醫生不錯喔﹗\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_board )
{
  PUSH_FUNCTION( "angel_board" );

  if ( ch->in_room && ch->in_room->board )
  {
    if ( fPrint ) send_to_char(
      "嗯﹐這裡有個留言版﹐試試看能不能閱\讀(\e[1;32mread\e[0m)"
      "裡面的文章。\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_money )
{
  PUSH_FUNCTION( "angel_money" );

  if ( ch->gold <= 100 )
  {
    if ( fPrint ) send_to_char(
      "出門在外怎麼可以不帶點錢呢﹐趕快去領(\e[1;32mdeposit\e[1;32m)"
      "或是去賺啦﹗\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_firman )
{
  PUSH_FUNCTION( "angel_firman" );

  if ( ch->firman <= 0 )
  {
    if ( fPrint ) send_to_char(
      "小心喔﹐你的免死金牌可是用光了喔﹐再死會扣技能的喔﹗\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_fighting_1 )
{
  PUSH_FUNCTION( "angel_fighting" );

  if ( ch->hit * 2 > get_curr_hit( ch ) )
  {
    if ( fPrint ) send_to_char( "怕什麼﹐趕緊把他給宰掉就是了。\n\r", ch );
    RETURN( TRUE );
  }
  RETURN( FALSE );
}

ANGEL( angel_fighting_2 )
{
  PUSH_FUNCTION( "angel_fighting" );

  if ( ch->hit * 2 <= get_curr_hit( ch ) )
  {
    if ( fPrint ) send_to_char(
      "都快掛點了﹐叫神也沒用﹐趕緊逃跑(\e[1;32mflee\e[0m)吧。\n\r", ch );

    RETURN( TRUE );
  }
  RETURN( FALSE );
}

ANGEL( angel_cname )
{
  PUSH_FUNCTION( "angel_cname" );

  if ( ch->pcdata && ch->pcdata->rechristen <= 2 )
  {
    if ( fPrint ) send_to_char(
      "這裡改名字的次數可是有限制的﹐別用光了喔﹗\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_nothing )
{
  PUSH_FUNCTION( "angel_nothing" );

  if ( ch->level == LEVEL_HERO )
  {
    if ( fPrint ) send_to_char(
      "恭禧你升到英雄﹐你可以考慮去解謎(\e[1;32mtask\e[0m)囉﹗\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_reboot )
{
  PUSH_FUNCTION( "angel_reboot" );

  if ( wizlock )
  {
    if ( fPrint ) send_to_char(
      "系統快要重新開機了﹐請趕快收拾收拾離線(\e[1;32mquit\e[0m)﹐"
      "待會再來吧﹗\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_mobile )
{
  CHAR_DATA * victim;
  CHAR_DATA * mob;
  int         count;

  PUSH_FUNCTION( "angel_mobile" );

  count = 0;
  mob   = NULL;

  for ( victim = char_list; victim; victim = victim->next )
  {
    if ( verify_char( victim )
      && IS_NPC( victim )
      && victim->in_room
      && !IS_SET( victim->act, ACT_NOKILL )
      && abs( victim->level - ch->level ) <= 3
      && number_range( 0, count++ ) == 0 )
    {
      mob = victim;
      if ( count >= 10 ) break;
    }
  }

  if ( mob )
  {
    if ( fPrint )
      act( "位於$t的$N很適合你升級的怪物﹐趕快去找他吧﹗"
        , ch, mob->in_room->area->name, mob, TO_CHAR );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_note )
{
  int         notes;
  NOTE_DATA * pnote;

  PUSH_FUNCTION( "angel_note" );

  for ( notes = 0, pnote = note_list; pnote; pnote = pnote->next )
  {
    if ( is_note_to( ch, pnote ) && str_cmp( ch->name, pnote->sender )
      && pnote->date_stamp > ch->last_note )
    {
      if ( fPrint ) send_to_char(
        "你有新的信件進來﹐趕快用 \e[1;32mnotes\e[0m "
        "來閱\讀這封信吧﹗\n\r", ch );

      RETURN( TRUE );
    }
  }

  RETURN( FALSE );
}

ANGEL( angel_leftskill )
{
  PUSH_FUNCTION( "agel_leftskill" );

  if ( !ch->class ) RETURN( FALSE );

  if ( learn_count( ch ) * 2 < get_learnable( ch ) )
  {
    if ( fPrint ) send_to_char(
      "看來你會的技能不多喔﹐用用 \e[1;32mleftskill\e[0m 來看看"
      "你還可以學哪些技能﹗\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_north )
{
  PUSH_FUNCTION( "angel_north" );

  if ( ch->in_room && ch->in_room->exit[DIR_NORTH] )
  {
    if ( fPrint ) send_to_char(
      "你的北方有出路﹐你可以輸入 \e[1;32mnorth\e[0m 往北走﹗\n\r" , ch );
    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_south )
{
  PUSH_FUNCTION( "angel_south" );

  if ( ch->in_room && ch->in_room->exit[DIR_SOUTH] )
  {
    if ( fPrint ) send_to_char(
      "你的南方有出路﹐你可以輸入 \e[1;32msouth\e[0m 往南走﹗\n\r" , ch );
    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_east )
{
  PUSH_FUNCTION( "angel_east" );

  if ( ch->in_room && ch->in_room->exit[DIR_EAST] )
  {
    if ( fPrint ) send_to_char(
      "你的東方有出路﹐你可以輸入 \e[1;32meast\e[0m 往東走﹗\n\r" , ch );
    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_west )
{
  PUSH_FUNCTION( "angel_west" );

  if ( ch->in_room && ch->in_room->exit[DIR_WEST] )
  {
    if ( fPrint ) send_to_char(
      "你的西方有出路﹐你可以輸入 \e[1;32mwest\e[0m 往西走﹗\n\r" , ch );
    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_up )
{
  PUSH_FUNCTION( "angel_up" );

  if ( ch->in_room && ch->in_room->exit[DIR_UP] )
  {
    if ( fPrint ) send_to_char(
      "你的上方有出路﹐你可以輸入 \e[1;32mup\e[0m 往上走﹗\n\r" , ch );
    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_down )
{
  PUSH_FUNCTION( "angel_down" );

  if ( ch->in_room && ch->in_room->exit[DIR_DOWN] )
  {
    if ( fPrint ) send_to_char(
      "你的下方有出路﹐你可以輸入 \e[1;32mdown\e[0m 往下走﹗\n\r" , ch );
    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_enter )
{
  PUSH_FUNCTION( "angel_enter" );

  if ( ch->in_room && ch->in_room->exit[DIR_ENTER] )
  {
    if ( fPrint ) send_to_char(
      "你可以輸入 \e[1;32menter\e[0m 來進入某間房間﹗\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_out )
{
  PUSH_FUNCTION( "angel_out" );

  if ( ch->in_room && ch->in_room->exit[DIR_OUT] )
  {
    if ( fPrint ) send_to_char(
      "你可以輸入 \e[1;32mout\e[0m 來進入某間房間﹗\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_spy )
{
  int dir;

  PUSH_FUNCTION( "angel_spy" );

  if ( !ch->in_room ) RETURN( FALSE );

  for ( dir = 0; dir < DIR_MAX; dir++ )
  {
    if ( ch->in_room->exit[dir] )
    {
      if ( fPrint ) print_to_char( ch,
        "你的%s方有出口﹐你可以去觀察(\e[1;32mspy %s\e[0m)看看喔﹗"
        , direction_name( dir ), direction_ename( dir ) );
      RETURN( TRUE );
    }
  }
  RETURN( FALSE );
}

ANGEL( angel_exits )
{
  int dir;

  PUSH_FUNCTION( "angel_exits" );

  if ( !ch->in_room ) RETURN( FALSE );

  for ( dir = 0; dir < DIR_MAX; dir++ )
  {
    if ( ch->in_room->exit[dir] )
    {
      if ( fPrint ) print_to_char( ch,
        "你可以輸入 \e[1;32mexits\e[0m 來觀察你四周的出口﹗\n\r" );

      RETURN( TRUE );
    }
  }
  RETURN( FALSE );
}

ANGEL( angel_cast )
{
  int          loop;
  SKILL_DATA * pSkill;

  PUSH_FUNCTION( "angel_cast" );

  for ( loop = 0; loop < MAX_SKILL; loop++ )
  {
    if ( ch->skill[loop] > 0
      && ( pSkill = get_skill( loop ) )
      && pSkill->cast )
    {
      if ( fPrint ) print_to_char( ch,
        "你學會了%s(%s)這項技能﹐你可以使用 \e[1;32mcast '%s'\e[0m "
        "來施展出來﹗\n\r"
        , pSkill->cname, pSkill->name, pSkill->name );

      RETURN( TRUE );
    }
  }

  RETURN( FALSE );

}

ANGEL( angel_ply )
{
  int          loop;
  SKILL_DATA * pSkill;

  PUSH_FUNCTION( "angel_ply" );

  for ( loop = 0; loop < MAX_SKILL; loop++ )
  {
    if ( ch->skill[loop] > 0
      && ( pSkill = get_skill( loop ) )
      && pSkill->ply
      && ( ch->skill[loop] < ( ch->level / UMAX( 1, pSkill->qutoient ) ) ) )
    {
      if ( fPrint ) print_to_char( ch,
        "你學會了%s(%s)這項技能﹐你可以使用 \e[1;32mply '%s'\e[0m "
        "來練習看看﹗\n\r"
        , pSkill->cname, pSkill->name, pSkill->name );

      RETURN( TRUE );
    }
  }

  RETURN( FALSE );

}

ANGEL( angel_inventory )
{
  PUSH_FUNCTION( "angel_inventory" );

  if ( ch->carrying )
  {
    if ( fPrint ) send_to_char(
      "你身上有些裝備﹐你可以輸入 \e[1;32minventory\e[0m 來察看﹗\n\r", ch );

    RETURN( TRUE );
  }
  RETURN( FALSE );
}

ANGEL( angel_fight_join )
{
  PUSH_FUNCTION( "angel_fight_join" );

  if ( ch->level > PK_LEVEL
    && get_age( ch ) > pk_age
    && !fight_info->lock
    && fight_info->status == PK_JOIN
    && !ch->leader
    && !ch->master
    && auction_info->seller != ch
    && auction_info->buyer != ch
    && ch->position == POS_STANDING
    && ch->failed <= 0
    && ch->jail <= 0
    && !IS_SET( ch->act, PLR_KILLER )
    && !IS_SET( ch->act, PLR_BOLTER )
    && !IS_SET( ch->act, PLR_THIEF )
    && !ch->mount
    && !ch->spirit
    && ch->in_room
    && !ch->in_room->NoQuit
    && !is_station( ch->in_room )
    && !ch->in_room->Sail
    && !is_pk( ch ) )
  {
    if ( fPrint ) send_to_char(
      "目前有場PK大賽﹐你可以輸入 \e[1;32mfight join\e[0m 來參加﹗\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_fight_bet )
{
  PUSH_FUNCTION( "angel_fight_bet" );

  if ( fight_info->status == PK_BET
    && !fight_info->lock
    && ch->level >= level_limit
    && ch->donate <= 0 )
  {
    if ( fPrint ) send_to_char(
      "目前有場PK大賽﹐你可以輸入 \e[1;32mfight bet <玩家號碼> <金錢"
      "\e[0m 來下注﹗\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

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
#include <ctype.h>
#include "merc.h"

void            system_auction       args( ( void ) );
extern void     set_sale_default     args( ( SALE_DATA     * ) );

void init_auction( void )
{
  auction_info->price      =    0;
  auction_info->times      =    0;
  auction_info->timer      =    0;
  auction_info->visible    = TRUE;
  auction_info->obj        = NULL;
  auction_info->seller     = NULL;
  auction_info->buyer      = NULL;
  auction_info->sale       = NULL;
}

FUNCTION( do_auction )
{
  char       buf[MAX_STRING_LENGTH];
  char       buf1[MAX_STRING_LENGTH];
  char       arg[MAX_INPUT_LENGTH];
  OBJ_DATA * auc_obj = auction_info->obj;
  OBJ_DATA * pObj;
  int        amount;
  int        sn;

  PUSH_FUNCTION( "do_auction" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  if( argument[0] == '\x0' )
  {
    if( auction_info->times == 0 || !auction_info->obj )
    {
      send_to_char( "對不起﹐目前的市面上沒有任何貨物在流通喔﹗\n\r", ch );
      RETURN_NULL();
    }

    chinese_number( auction_info->price, buf );

    if ( auction_info->visible )
    {
      chinese_number( auc_obj->level, buf1 );

      print_to_char( ch , "目前最看好的商品是 %s﹐現場已經喊到了%s"
                          "兩黃金﹐別再看了﹐快點出價吧﹗等級是%s。\n\r"
        , obj_name( ch, auction_info->obj ), buf, buf1 );
    }

    else
    {
      act( "目前盒中物已經喊到$t兩黃金﹐趕快喊上去吧﹐慢了就來不及了﹗"
        , ch, buf, NULL, TO_CHAR );
    }

    RETURN_NULL();
  }

  if ( !str_prefix( argument , "!information" ) && IS_IMMORTAL( ch ) )
  {
    if ( ch->level < LEVEL_NODEATH )
    {
      act( "抱歉﹐$n還不能看流通貨物的資料﹗", ch, NULL, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( auction_info->seller )
    {
      print_to_char( ch , "賣主           ﹕%s\n\r"
                          "買主           ﹕%s\n\r"
                          "價錢           ﹕%d\n\r"
                          "時間           ﹕%d\n\r"
                          "原物品計時器   ﹕%d\n\r"
                          "物品名稱       ﹕%s\n\r"
                          "中文名稱       ﹕%s\n\r"
                          "物品等級       ﹕%d\n\r"
                          "物品時效       ﹕%d\n\r\n\r" ,
      auction_info->seller->name  ,
      auction_info->buyer ? auction_info->buyer->name : "(從缺) " ,
      auction_info->price ,
      auction_info->times ,
      auction_info->timer ,
      auction_info->obj->name ,
      obj_name( ch, auction_info->obj ) ,
      auction_info->obj->level ,
      auction_info->obj->timer );

      identify_obj( ch, auction_info->obj );
    }

    else
    {
      send_to_char( "目前並沒有在拍賣物品﹐所以無法查詢。\n\r" , ch );
    }
    RETURN_NULL();
  }

  if ( !str_prefix( argument , "!cancel" ) && IS_GOD( ch ) )
  {
    if ( auction_info->seller )
    {
      if ( auction_info->buyer && !over_scale( auction_info->buyer ) )
        gold_to_char( auction_info->buyer, auction_info->price );

      if ( !IS_NPC( auction_info->seller ) )
      {
        auction_info->obj->timer     = auction_info->timer;
        obj_to_char( auction_info->obj , auction_info->seller );

        act( "對不起﹐你的$p賣不出去被退了回來了﹗"
          , auction_info->seller, auction_info->obj, NULL, TO_CHAR );
      }

      else
      {
        extract_obj( auction_info->obj );
      }

      init_auction();

      talk_channel_2( "對不起﹐取消了這次的拍賣。", CHANNEL_NOTICE, "" );
      send_to_char( "你取消了這次的拍賣。\n\r" , ch );
      RETURN_NULL();
    }

    else
    {
      send_to_char( "對不起﹐沒有物品正在拍賣﹐所以無法取消。\n\r" , ch );
      RETURN_NULL();
    }
  }

  if ( !str_prefix( argument , "!set" ) && IS_GOD( ch ) )
  {
    if ( auction_info->seller && auction_info->buyer)
    {
      if ( !IS_NPC( auction_info->seller ) )
      {
        gold_to_char( auction_info->seller, auction_info->price );
        auction_info->obj->timer     = auction_info->timer;
        obj_to_char( auction_info->obj , auction_info->buyer );

        act( "你標得的$p給您送過來了﹗"
           , auction_info->buyer, auction_info->obj, NULL, TO_CHAR );
      }

      else
      {
        extract_obj( auction_info->obj );
      }

      init_auction();
      talk_channel_2( "對不起﹐加快完成了這次的拍賣。" , CHANNEL_NOTICE, "" );
      send_to_char( "你加快完成了這次的拍賣。\n\r" , ch );
      RETURN_NULL();
    }

    else
    {
      send_to_char( "沒有物品正在拍賣﹐或是買家﹐所以無法設定。\n\r" , ch );
      RETURN_NULL();
    }
  }

  if ( auction_info->times )
  {
    if ( auction_info->seller == ch && !auction_info->buyer )
    {
      act( "看來是沒有人對$p有興趣了﹐你還是快點準備點其他的貨物吧﹗"
        , ch, auction_info->obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    send_to_char( "想要賣別的東東啊﹖先把這個買走吧﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( ( reboot_time   > 0 && ( ( reboot_time   - current_time ) < 300 ) )
    || ( shutdown_time > 0 && ( ( shutdown_time - current_time ) < 300 ) ) )
  {
    send_to_char( "系統即將關閉﹐所以無法進行交易。\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->trade == FALSE )
  {
    send_to_char( "你的總資產太可怕了﹐所以無法執行這個指令﹗\n\r", ch );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg );

  if ( !( pObj = get_obj_carry( ch, arg ) ) )
  {
    send_to_char( "你強盜啊﹖沒有的東西也敢拿出來賣﹗\n\r", ch );
    RETURN_NULL();
  }

  /* 物品是否有餵毒 */
  if ( poison_char( ch, pObj ) ) RETURN_NULL();

  if ( pObj->address )
  {
    act( "$p是要交給別人的信﹐怎麼可以拿來拍賣。", ch, pObj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  /* 看看是不是可以丟棄 */
  if ( !can_drop_obj( ch, pObj ) )
  {
    act( "$p是你的傳家之寶﹐所以不可以拿來拍賣。", ch, pObj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  one_argument( argument, arg );
  if ( ( amount = atoi( arg ) ) <= 0 )
  {
    act( "你想要免費贈送啊﹖嗯﹐$p該不會是贓品吧﹗", ch, pObj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( ( amount = atoi( arg ) ) >= UMAX( 1, MaxAuctionGold ) )
  {
    send_to_char( "請勿破壞市場價格﹗\n\r", ch );
    RETURN_NULL();
  }

  switch( pObj->item_type )
  {
  case ITEM_FOOD:
    if ( pObj->value[3] != 0 )
    {
      act( "有毒的$p都敢出來賣﹗", ch, pObj, NULL, TO_CHAR );
      RETURN_NULL();
    }
    break;

  case ITEM_DRINK_CON:
    if ( pObj->value[3] != 0 )
    {
      act( "有毒的$p你都敢出來賣﹗", ch, pObj, NULL, TO_CHAR );
      RETURN_NULL();
    }
    break;

  case ITEM_TRASH:
  case ITEM_CORPSE_PC:
  case ITEM_CORPSE_NPC:
  case ITEM_CONTAINER:
  case ITEM_MONEY:

    act( "對不起﹐拍賣會場是不拍賣$p﹗", ch, pObj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( ( sn = pObj->cost ) < 20 )
  {
    act( "$p這麼便宜﹐還是不要拿出來賣好了﹗", ch, pObj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( sn > amount )
  {
    act( "雖說不是什麼好東西﹐但是總有$i兩黃金的價值吧﹖", ch, &sn, NULL, TO_CHAR );
    RETURN_NULL();
  }

  auction_info->seller     = ch;
  auction_info->obj        = pObj;
  auction_info->price      = amount;
  auction_info->buyer      = NULL;
  auction_info->times      = 1;
  auction_info->visible    = TRUE;
  auc_obj                  = auction_info->obj;
  auction_info->timer      = auc_obj->timer;
  auc_obj->timer           = 0;

  chinese_number( auction_info->price, buf1 );
  obj_from_char( auction_info->obj );

  sprintf( buf, "傳家之寶 %s 現在只賣 %s 兩黃金﹗手快有﹐"
                "手慢無﹐好評發售中。要買要快喔﹗\e[0m"
    , obj_name( NULL, auction_info->obj ), buf1 );

  talk_channel( ch, buf , CHANNEL_AUCTION , "\e[1;33m大聲叫賣著" );
  message_driver( ch, auction_info->obj, ACT_WHEN_AUCTION );
  RETURN_NULL();
}

FUNCTION( do_bet )
{
  char buf[MAX_STRING_LENGTH];
  char arg[MAX_INPUT_LENGTH];
  int  price;
  int  money;

  PUSH_FUNCTION( "do_bet" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  if ( !auction_info->times || !auction_info->obj )
  {
    send_to_char( "現在沒有人在賣東西﹐你提供一個來賣吧﹗\n\r", ch );
    RETURN_NULL();
  }

  stock_value( ch );

  if ( ch->trade == FALSE )
  {
    send_to_char( "你的總資產太可怕了﹐所以無法執行這個指令﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( argument[0] == '\x0' )
  {
    act( "目前的行情水漲船高﹐已經漲到$i兩黃金了。再不買就買不起了﹗"
      , ch, &auction_info->price, NULL, TO_CHAR );

    if ( auction_info->visible )
    {
      money = UMAX( 20, auction_info->obj->cost / 20 );
      act( "想要知道物品的資料($2bet help$0)﹐你必須付$i兩銀子喔﹗"
        , ch, &money, NULL, TO_CHAR );
    }

    RETURN_NULL();
  }

  if ( auction_info->seller == ch )
  {
    send_to_char( "這是你自己的東西耶﹐想要哄抬價格啊﹖強盜﹗\n\r", ch );
    RETURN_NULL();
  }

  one_argument( argument, arg );

  if ( !str_prefix( arg, "information" )
    || !str_prefix( arg, "help" ) )
  {
    if ( auction_info->visible == FALSE )
    {
      send_to_char( "對不起﹐這個是裝在盒子裡﹐我也不知道是什麼﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->gold >= ( money = UMAX( 20, auction_info->obj->cost / 20 ) ) )
    {
      identify_obj( ch, auction_info->obj );
      gold_from_char( ch, money );
    }

    else
    {
      send_to_char( "想要知道更詳盡的貨物情報﹐就準備點錢來吧﹗\n\r", ch );
    }

    RETURN_NULL();
  }

  if ( ch->donate > 0 )
  {
    send_to_char( "在你領救濟金的這段時間﹐就只能看人拍賣﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( excess_filequota( ch ) )
  {
    send_to_char( "對不起﹐你的檔案太大﹐無法執行這個命令﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( is_pk( ch ) )
  {
    send_to_char( "你還是專心在武鬥大會吧﹐別管拍賣了﹗\n\r", ch );
    RETURN_NULL();
  }

  if( ( price = atoi( arg ) ) <= 0 )
  {
    send_to_char( "你是在開玩笑嗎﹖\n\r", ch );
    RETURN_NULL();
  }

  if( price > ch->gold )
  {
    send_to_char( "我們不接受刷卡喔﹗\n\r", ch );
    RETURN_NULL();
  }

  if( price < auction_info->price * 12 / 10 )
  {
    act( "現在已經喊到了$i兩了。你出這種價錢不會很寒酸嗎﹖\n\r至少也多兩成嘛﹗"
      , ch, &auction_info->price, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if( price <= auction_info->price )
  {
    send_to_char( "這位客倌﹐拍賣的價格是只升不降的﹐"
                  "如果你想要這件物品﹐那就出高點吧﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( auction_info->buyer && !over_scale( auction_info->buyer ) )
    gold_to_char( auction_info->buyer, auction_info->price );

  auction_info->buyer = ch;
  auction_info->price = price;
  auction_info->times = 1;
  gold_from_char( ch, price );

  if ( auction_info->visible )
  {
    sprintf( buf, "我出價 %d 兩黃金買 %s﹗\e[0m"
      , price , obj_name( NULL, auction_info->obj ) );
  }

  else
  {
    sprintf( buf, "我出價 %d 兩黃金買盒中物﹗\e[0m", price );
  }

  talk_channel( ch, buf, CHANNEL_AUCTION, "\e[1;33m嘶喊著" );
  message_driver( ch, auction_info->obj, ACT_WHEN_BET );
  RETURN_NULL();
}

void auction_update( void )
{
  char             buf[MAX_STRING_LENGTH];
  char             buf1[MAX_STRING_LENGTH];
  char             buf2[MAX_STRING_LENGTH];
  SALE_DATA      * pSale;
  OBJ_DATA       * auc_obj = NULL;
  OBJ_INDEX_DATA * pIndex;

  PUSH_FUNCTION( "auction_update" );

  if ( !auction_info->times < 0
    || !auction_info->seller
    || !auction_info->obj )
  {
    auction_info->times = 0;

    /* 是否由系統拍賣東西 */
    system_auction();

    RETURN_NULL();
  }

  if ( auction_info->times++ < 3 )
  {
    if ( auction_info->buyer )
    {
      if ( auction_info->visible )
      {
        chinese_number( auction_info->times -1, buf1 );
        chinese_number( auction_info->price   , buf2 );
        sprintf( buf, "%s 目前由 %s 叫價 \e[1;32m%s\e[0m 兩黃金第%s次﹗\e[0m"
          , obj_name( NULL, auction_info->obj ), auction_info->buyer->name
          , buf2, buf1 );
      }

      else
      {
        chinese_number( auction_info->times - 1, buf1 );
        chinese_number( auction_info->price   , buf2 );
        sprintf( buf, "盒中物目前由 %s 喊價 \e[1;32m%s\e[0m"
                      " 兩黃金﹐第%s次﹗\e[0m"
          , auction_info->buyer->name, buf2, buf1 );
      }

      talk_channel( auction_info->seller, buf, CHANNEL_AUCTION
        , "\e[1;33m大聲嘶喊著" );
    }

    else
    {
      if ( auction_info->visible )
      {
        chinese_number( auction_info->price, buf1 );
        sprintf( buf, "%s 目前的出價是%s兩黃金﹗有沒有人要出更高"
                      "的價錢啊﹖\e[0m"
        , obj_name( NULL, auction_info->obj ), buf1 );
      }

      else
      {
        chinese_number( auction_info->price, buf1 );
        sprintf( buf, "盒中物目前的出價是%s兩黃金﹗有沒有人要出更高"
          "的價錢啊﹖\e[0m", buf1 );
      }

      talk_channel( auction_info->seller, buf, CHANNEL_AUCTION
        , "\e[1;33m大聲叫賣著" );
    }
  }

  else
  {
    if ( !auction_info->buyer )
    {
      str_cpy( buf, "\e[1;33m唉﹐沒有識貨的人啊﹐下次再拿出來賣吧﹗\e[0m" );

      if ( !IS_NPC( auction_info->seller ) )
      {
        auc_obj        = auction_info->obj;
        auc_obj->timer = auction_info->timer;
        obj_to_char( auc_obj, auction_info->seller );

        act( "對不起﹐你的$p賣不出去被退了回來了﹗"
          , auction_info->seller, auction_info->obj, NULL, TO_CHAR );
      }

      else
      {
        extract_obj( auction_info->obj );
      }
    }

    else
    {
      if ( auction_info->visible )
      {
        chinese_number( auction_info->price, buf1 );
        sprintf( buf, "%s 以%s兩黃金的價格賣給了 %s﹗\e[0m"
          , obj_name( NULL, auction_info->obj ), buf1
          , ( !is_affected( auction_info->buyer, SLOT_INVIS )
            && !is_affected( auction_info->buyer, SLOT_MASS_INVIS ) )
            ? auction_info->buyer->name : "某不知名人物");

        pIndex = auction_info->obj->pIndexData;

        pIndex->auction_times++;
        pIndex->auction_gold += auction_info->price;

        if ( !pIndex->Sale
         && ( pIndex->auction_times > AuctionTimes
           || pIndex->auction_gold   > AuctionGold ) )
        {
          set_sale_default( pSale = alloc_struct( STRUCT_SALE_DATA ) );

          pSale->obj     = pIndex;
          pSale->visible = TRUE;
          pSale->cost    = pIndex->auction_gold / pIndex->auction_times;
          pIndex->Sale   = TRUE;

          pSale->next = sale_list;
          sale_list   = pSale;
        }
      }

      else
      {
        chinese_number( auction_info->price, buf1 );
        sprintf( buf, "盒中物以%s兩黃金的價格賣給了 %s﹗\e[0m"
          , buf1
          , ( !is_affected( auction_info->buyer, SLOT_INVIS )
            && !is_affected( auction_info->buyer, SLOT_MASS_INVIS ) )
            ? auction_info->buyer->name : "某不知名人物");
      }

      if ( !IS_NPC( auction_info->seller )
        && !over_scale( auction_info->seller ) )
      {
        gold_to_char( auction_info->seller, auction_info->price );
      }

      auc_obj                     = auction_info->obj;
      auc_obj->timer              = auction_info->timer;
      obj_to_char( auc_obj, auction_info->buyer );

      act( "你標得的$p給您送過來了﹗"
        , auction_info->buyer, auction_info->obj, NULL, TO_CHAR );


      if ( auction_info->sale )
      {
        auction_info->sale->gold += auction_info->price;
        auction_info->sale->sold++;
      }
    }

    talk_channel( auction_info->seller, buf, CHANNEL_AUCTION,
      "\e[1;33m宣布道" );

    init_auction();
  }

  RETURN_NULL();
}

void system_auction( void )
{
  MOB_INDEX_DATA * mob;
  SALE_DATA      * pSale;
  SALE_DATA      * zSale;
  CHAR_DATA      * victim;
  CHAR_DATA      * seller;
  OBJ_DATA       * pObj;
  int              count = 0;
  char           * message;
  char             buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "system_auction" );

  if ( !sale_list
    || number_range( 1, 1000 ) > SaleChance
    || get_usernum() < SaleLimit )
    RETURN_NULL();

  if ( ( reboot_time   > 0 && ( ( reboot_time   - current_time ) < 300 ) )
    || ( shutdown_time > 0 && ( ( shutdown_time - current_time ) < 300 ) ) )
    RETURN_NULL();

  count = 0;
  zSale = NULL;

  for ( pSale = sale_list; pSale; pSale = pSale->next )
    if ( pSale->obj && ( number_range( 0, count++ ) == 0 ) ) zSale = pSale;

  if ( zSale == NULL ) RETURN_NULL();

  count  = 0;
  seller = NULL;

  for ( count = 0, victim = char_list; victim; victim = victim->next )
  {
    if ( verify_char( victim )
      && ( mob = victim->pIndexData )
      && mob->auction
      && *mob->auction
      && number_range( 0, count++ ) == 0 )
      seller = victim;
  }

  if ( seller == NULL ) RETURN_NULL();

  if ( !( pObj = create_object( zSale->obj, -1 ) ) ) RETURN_NULL();

  auction_info->seller     = seller;
  auction_info->obj        = pObj;
  auction_info->price      = zSale->cost;
  auction_info->buyer      = NULL;
  auction_info->times      = 1;
  auction_info->timer      = pObj->timer;
  auction_info->visible    = zSale->visible;
  auction_info->sale       = zSale;

  zSale->times++;
  pObj->timer = 0;

  message = seller->pIndexData->auction;
  talk_channel( seller, message , CHANNEL_AUCTION , "\e[1;33m說道" );

  if ( zSale->visible )
  {
    sprintf( buf, "傳家之寶 %s 現在只賣 %d 兩黃金﹗手快有﹐"
                  "手慢無﹐好評發售中。要買要快喔﹗\e[0m"
      , obj_name( NULL, pObj ), zSale->cost );
  }

  else
  {
    sprintf( buf, "盒子裡不知道裝了什麼鬼﹐反正俗俗賣﹐只要 %d 兩黃金﹗"
      "﹐請各位有錢捧個生意場﹐沒錢的捧個人場吧﹗"
      , zSale->cost );
  }

  talk_channel( seller, buf , CHANNEL_AUCTION , "\e[1;33m大聲叫賣著" );
  RETURN_NULL();
}

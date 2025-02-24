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

void event_update( void )
{
  EVENT_DATA * pEvent;

  PUSH_FUNCTION( "event_update" );

  for ( pEvent = event_list; pEvent; pEvent = pEvent->next )
  {
    if ( pEvent->chance > 0
      && !pEvent->lock
      && number_range( 1, 10000 ) <= pEvent->chance )
    {
      ( *pEvent->function ) ();
      pEvent->count++;
    }
  }

  RETURN_NULL();
}

FUNCTION( do_event )
{
  char         arg[MAX_INPUT_LENGTH];
  int          loop;
  int          count;
  EVENT_DATA * pEvent;

  PUSH_FUNCTION( "do_event" );

  if ( !event_list )
  {
    act( "對不起﹐$t沒有任何的觸發事件﹗", ch, mud_name, NULL, TO_CHAR );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    clear_buffer();

    send_to_buffer( "\e[1;33;44m 順序 名      稱          "
      "執  行 鎖定 機  率                                    \e[0m\n\r" );

    for ( loop = 1, pEvent = event_list; pEvent; pEvent = pEvent->next )
    {
      send_to_buffer( "%4d %-20s %6d  %s  %6d\n\r"
        , loop++, pEvent->title
        , pEvent->count, YESNO( pEvent->lock ), pEvent->chance );
    }

    print_buffer( ch );
  }

  else if ( !str_prefix( arg, "lock" ) )
  {
    if ( argument[0] == '\x0' || !is_number( argument ) )
    {
      send_to_char( "你要鎖定幾號的觸發呢﹖\n\r", ch );
      RETURN_NULL();
    }

    count = atoi( argument );
    for ( loop = 0, pEvent = event_list; pEvent; pEvent = pEvent->next )
      if ( ++loop == count ) break;

    if ( !pEvent )
    {
      send_to_char( "對不起﹐沒有那號觸發﹐鎖定取消﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( pEvent->lock )
    {
      act( "你設定了$t這個觸發動作﹗", ch, pEvent->title, NULL, TO_CHAR );
      pEvent->lock = FALSE;
    }

    else
    {
      act( "你鎖定了$t這個觸發動作﹗", ch, pEvent->title, NULL, TO_CHAR );
      pEvent->lock = TRUE;
    }
  }

  else if ( !str_prefix( arg, "go" ) )
  {
    if ( argument[0] == '\x0' || !is_number( argument ) )
    {
      send_to_char( "對不起﹐你要手動觸發哪一號事件呢﹖\n\r", ch );
      RETURN_NULL();
    }

    count = atoi( argument );
    for ( loop = 0, pEvent = event_list; pEvent; pEvent = pEvent->next )
    {
      if ( ++loop == count )
      {
        if ( pEvent->lock )
        {
          act( "$t這個事件已經被鎖定無法執行。"
            , ch, pEvent->title, NULL, TO_CHAR );
          RETURN_NULL();
        }

        act( "你手動觸發了$t這個事件﹗", ch, pEvent->title, NULL, TO_CHAR );
        ( *pEvent->function ) ();
        pEvent->count++;

        RETURN_NULL();
      }
    }

    send_to_char( "對不起﹐沒有那號觸發﹐取消執行﹗\n\r", ch );
  }

  else
  {
    send_to_char( "你的參數錯誤﹐請查詢 event 的使用方法﹗\n\r", ch );
  }
  RETURN_NULL();
}

void event_thunder( void )
{
  DESCRIPTOR_DATA * pDesc;
  DESCRIPTOR_DATA * zDesc;
  CHAR_DATA       * ch;
  char              buf[MAX_STRING_LENGTH];
  int               count;
  int               loop;

  PUSH_FUNCTION( "event_thunder" );

  zDesc = NULL;
  for ( count = 1, pDesc = descriptor_list; pDesc; pDesc = pDesc->next )
  {
    if ( verify_desc( pDesc )
      && pDesc->connected == CON_PLAYING
      && pDesc->character
      && pDesc->character->alignment <= ( -AlignRange + 100 )
      && can_damage( pDesc->character )
      && ( loop = number_range( 1, count++ ) ) == 1 )
      zDesc = pDesc;
  }

  if ( zDesc && ( ch = zDesc->character ) )
  {
    sprintf( buf, "突然天上一陣雷聲隆隆﹐平日作惡多端的%s"
      "剛好就被這陣雷打到﹗", mob_name( NULL, ch ) );

    talk_channel_2( buf, CHANNEL_BULLETIN, "天災" );

    send_to_char( "誰叫你平日作惡多端﹐你看﹐被雷打到了吧﹗\n\r", ch );

    ch->hit  = 1;
    ch->mana = 1;
    ch->move = 1;
  }

  RETURN_NULL();
}

void event_solar_eclipse( void )
{
  DESCRIPTOR_DATA * pDesc;
  CHAR_DATA       * ch;
  char              buf[MAX_STRING_LENGTH];
  int               value;

  PUSH_FUNCTION( "event_solar_eclipse" );

  str_cpy( buf, "突然出現了日蝕﹐一時人心惶惶﹐竊賊四起﹐很多人就在"
    "一陣兵慌馬亂之中﹐丟了不少錢﹗" );

  talk_channel_2( buf, CHANNEL_BULLETIN, "天災" );

  for ( pDesc = descriptor_list; pDesc; pDesc = pDesc->next )
  {
    if ( verify_desc( pDesc )
      && pDesc->connected == CON_PLAYING
      && ( ch = pDesc->character )
      && ch->level > level_limit
      && ch->gold >= 1000 )
    {
      if ( ( value = ch->gold * 10 / 100 ) > 0 )
      {
        act( "你遺失了$i兩黃金。", ch, &value, NULL, TO_CHAR );
        gold_from_char( ch, value );
      }
    }
  }

  RETURN_NULL();
}

void event_month_eclipse( void )
{
  DESCRIPTOR_DATA * pDesc;
  CHAR_DATA       * ch;
  char              buf[MAX_STRING_LENGTH];
  int               value;

  PUSH_FUNCTION( "event_month_eclipse" );

  str_cpy( buf, "天生異象﹐竟出現了月蝕﹐大家人心惶惶﹐不少人竟捲款而逃﹐"
    "裡面竟然有你們錢莊的老闆﹗" );

  talk_channel_2( buf, CHANNEL_BULLETIN, "天災" );

  for ( pDesc = descriptor_list; pDesc; pDesc = pDesc->next )
  {
    if ( verify_desc( pDesc )
      && pDesc->connected == CON_PLAYING
      && ( ch = pDesc->character )
      && ch->level >= level_limit
      && ch->bank >= 1000 )
    {
      if ( ( value = ch->bank * 10 / 100 ) > 0 )
      {
        act( "你的錢莊戶頭遺失了$i兩黃金。", ch, &value, NULL, TO_CHAR );
        ch->bank = UMAX( 0, ch->bank - value );
      }
    }
  }

  RETURN_NULL();
}

void event_drought( void )
{
  CHAR_DATA * ch;
  OBJ_DATA  * pObj;
  OBJ_DATA  * zObj;
  char        buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "event_drought" );

  str_cpy( buf, "因為久久未降雨水﹐各地旱災頻傳﹗" );
  talk_channel_2( buf, CHANNEL_BULLETIN, "天災" );

  for ( pObj = object_list; pObj; pObj = zObj )
  {
    zObj = pObj->next;

    if ( verify_obj( pObj )
      && pObj->item_type == ITEM_FOUNTAIN
      && !pObj->in_obj
      && !pObj->deposit_by
      && !pObj->carried_by
      && pObj != auction_info->obj
      && pObj->in_room )
    {
      sprintf( buf, "%s因為旱災而乾枯\了﹗\n\r", obj_name( NULL, pObj ) );
      sendmsg_to_someroom( buf, pObj->in_room );
      extract_obj( pObj );
    }
  }

  for ( ch = char_list; ch; ch = ch->next )
  {
    if ( verify_char( ch )
      && ch->desc
      && ch->desc->connected == CON_PLAYING
      && ch->in_room
      && ch->pcdata )
    {
      ch->pcdata->condition[COND_THIRST] = 0;
      send_to_char( "\e[33m你覺得口乾舌燥﹐似乎可以喝下一桶水。\e[0m\n\r"
        , ch );
    }
  }

  RETURN_NULL();
}

void event_locust( void )
{
  CHAR_DATA * ch;
  OBJ_DATA  * pObj;
  OBJ_DATA  * zObj;
  char        buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "event_locust" );

  str_cpy( buf, "屋漏偏逢連夜雨﹐農作物欠收又遇到蝗害﹐各地災害頻傳﹗" );
  talk_channel_2( buf, CHANNEL_BULLETIN, "天災" );

  for ( pObj = object_list; pObj; pObj = zObj )
  {
    zObj = pObj->next;

    if ( verify_obj( pObj )
      && pObj->item_type == ITEM_FOOD
      && !pObj->in_obj
      && !pObj->deposit_by
      && !pObj->carried_by
      && pObj != auction_info->obj
      && pObj->in_room )
    {
      sprintf( buf, "%s因為蝗害而消失了﹗\n\r", obj_name( NULL, pObj ) );
      sendmsg_to_someroom( buf, pObj->in_room );
      extract_obj( pObj );
    }
  }

  for ( ch = char_list; ch; ch = ch->next )
  {
    if ( verify_char( ch )
      && ch->desc
      && ch->desc->connected == CON_PLAYING
      && ch->in_room
      && ch->pcdata )
    {
      ch->pcdata->condition[COND_FULL] = 0;
      send_to_char( "\e[33m咕嚕嚕一聲﹐你的肚子餓了。\e[0m\n\r", ch );
    }
  }

  RETURN_NULL();
}

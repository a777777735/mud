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

#define TICKET_TOTAL    10
#define TICKET_COST     100
#define TICKET_RESET    2

TICKET_DATA *   ticket_list = NULL;
ORDER_DATA  *   order_list  = NULL;
int             TicketTotal = TICKET_TOTAL;
int             TicketCost  = TICKET_COST;
int             TicketReset = TICKET_RESET;
int             ticketreset = TICKET_RESET;

void            set_ticket_default      args( ( TICKET_DATA * ) );

void generate_ticket( int howmany )
{
  TICKET_DATA * pTicket;
  ORDER_DATA  * pOrder;
  int           loop;
  int           count;

  PUSH_FUNCTION( "generate_tick" );

  if ( howmany < 0 )
    mudlog( LOG_ERR, "generate_ticket: 來源不合理." );

  if ( !order_list )
    mudlog( LOG_ERR, "generate_ticket: 沒有中獎的資料." );

  for ( count = 0, pOrder = order_list; pOrder; pOrder = pOrder->next )
    count += pOrder->howmany;

  if ( count > howmany )
    mudlog( LOG_ERR, "generate_ticket: 中獎組數大於全部組數." );

  for ( loop = 0; loop < howmany; loop++ )
  {
    set_ticket_default( pTicket = alloc_struct( STRUCT_TICKET_DATA ) );
    pTicket->next = ticket_list;
    ticket_list   = pTicket;
  }

  /* 重置彩券 */
  dice_ticket();

  RETURN_NULL();
}

void dice_ticket( void )
{
  TICKET_DATA * pTicket;
  TICKET_DATA * zTicket;
  ORDER_DATA  * pOrder;
  int           loop;
  int           count;

  PUSH_FUNCTION( "dice_ticket" );

  if ( !ticket_list || !order_list ) RETURN_NULL();

  /* 清除資料 */
  for ( pTicket = ticket_list; pTicket; pTicket = pTicket->next )
    set_ticket_default( pTicket );

  for ( pOrder = order_list; pOrder; pOrder = pOrder->next )
  {
    for ( loop = 0; loop < pOrder->howmany; loop++ )
    {
      zTicket = NULL;
      count   = 0;

      for ( pTicket = ticket_list; pTicket; pTicket = pTicket->next )
      {
        if ( !pTicket->order && ( number_range( 0, count++ ) == 0 ) )
          zTicket = pTicket;
      }

      if ( zTicket ) zTicket->order = pOrder;
    }
  }

  ticketreset = TicketReset;

  RETURN_NULL();
}

void set_ticket_default( TICKET_DATA * pTicket )
{
  PUSH_FUNCTION( "set_ticket_default" );

  pTicket->order = NULL;
  pTicket->sold  = FALSE;
  RETURN_NULL();
}

FUNCTION( do_chance )
{
  TICKET_DATA * pTicket;
  TICKET_DATA * zTicket;
  ORDER_DATA  * pOrder;
  CHAR_DATA   * victim;
  char        * msg;
  char          arg[MAX_INPUT_LENGTH];
  int           sold;
  int           count;
  int           loop;

  PUSH_FUNCTION( "do_chance" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  if ( !ticket_list || !order_list )
  {
    act( "對不起﹐$t沒有任何彩券販賣﹗", ch, mud_name, NULL, TO_CHAR );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    clear_buffer();

    send_to_buffer(
      "每次產生﹕ %d 組。\n\r"
      "每組價錢﹕ %d 兩。\n\r"
      "重置時間﹕ %d 天。\n\r"
      , TicketTotal, TicketCost, TicketReset );

    for ( sold = 0, pTicket = ticket_list; pTicket; pTicket = pTicket->next )
      if ( pTicket->sold ) sold++;

    send_to_buffer(
      "已經賣出﹕ %d 組。\n\r"
      "剩餘彩券﹕ %d 組。\n\r"
      , sold, TicketTotal - sold );

    for ( pOrder = order_list; pOrder; pOrder = pOrder->next )
    {
      send_to_buffer( "中獎設定﹕ 第 %d 特獎﹐有 %3d 組﹐每組 %5d 兩。\n\r"
        , pOrder->order, pOrder->howmany, pOrder->gold );

      if ( buffer_full() ) break;
    }

    print_buffer( ch );
  }

  else if ( !str_prefix( arg, "setting" ) && IS_IMMORTAL( ch ) )
  {
    clear_buffer();

    send_to_buffer(
      "每次產生﹕ %d 組。\n\r"
      "每組價錢﹕ %d 兩。\n\r"
      "重置時間﹕ %d 天。\n\r"
      , TicketTotal, TicketCost, TicketReset );

    for ( sold = 0, pTicket = ticket_list; pTicket; pTicket = pTicket->next )
      if ( pTicket->sold ) sold++;

    send_to_buffer(
      "已經賣出﹕ %d 組。\n\r"
      "剩餘彩券﹕ %d 組。\n\r"
      , sold, TicketTotal - sold );

    for ( count = 0, pOrder = order_list; pOrder; pOrder = pOrder->next )
      count += pOrder->gold * pOrder->howmany;

    send_to_buffer( "彩券利益﹕ 總售額﹕%d 總獎金﹕%d\n\r"
      , TicketCost * TicketTotal , count );

    for ( pOrder = order_list; pOrder; pOrder = pOrder->next )
    {
      send_to_buffer( "中獎設定﹕ 第 %d 特獎﹐有 %3d 組﹐每組 %5d 兩。\n\r"
        , pOrder->order, pOrder->howmany, pOrder->gold );
    }

    for ( pOrder = order_list; pOrder; pOrder = pOrder->next )
    {
      send_to_buffer( "\n\r第 %d 特獎順序:\n\r", pOrder->order );

      count = 1;
      loop = 0;
      for ( pTicket = ticket_list; pTicket; pTicket = pTicket->next, count++ )
      {
        if ( pTicket->order == pOrder )
        {
          if ( loop > 0 && ( loop % 15 ) == 0 ) send_to_buffer( "\n\r" );
          send_to_buffer( " %s%4d\e[0m"
            , pTicket->sold ? "\e[1;32m" : ""
            , count );
          loop++;
        }
      }

      send_to_buffer( "\n\r" );
    }
    print_buffer( ch );
  }

  else if ( !str_prefix( arg, "buy" ) )
  {
    if ( !IS_NPC( ch ) && ch->level < level_limit )
    {
      act( "在你升級到$i級之前﹐請注意你的金錢﹗"
        , ch, &level_limit, NULL, TO_CHAR );

      RETURN_NULL();
    }

    if ( ch->donate > 0 )
    {
      send_to_char( "救濟金可不是讓你拿來買彩券的﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->trade == FALSE )
    {
      send_to_char( "你的總資產太可怕了﹐所以無法執行這個指令﹗\n\r", ch );
      RETURN_NULL();
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\x0' )
    {
      send_to_char( "對不起﹐你要跟誰買彩券呢﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( !( victim = get_char_room( ch, arg ) ) )
    {
      act( "這裡沒有 $2$T$0﹐所以你買不到彩券。", ch, NULL, arg, TO_CHAR );
      RETURN_NULL();
    }

    if ( !IS_NPC( victim ) )
    {
      act( "對不起﹐$N是不賣彩券的。", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( victim->position != POS_STANDING )
    {
      act( "$N正在忙呢﹐等一下好了﹗", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( !can_see( victim, ch ) )
    {
      act( "$N看不到你耶。", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch->gold < TicketCost )
    {
      act( "這是公益彩券﹐一張要$i兩銀子﹗", ch, &TicketCost, NULL, TO_CHAR );
      RETURN_NULL();
    }

    zTicket = NULL;
    loop    = 0;

    for ( pTicket = ticket_list; pTicket; pTicket = pTicket->next )
    {
      if ( !pTicket->sold && ( number_range( 0, loop++ ) == 0 ) )
        zTicket = pTicket;
    }

    if ( !zTicket )
    {
      send_to_char( "對不起﹐所有的彩券都賣完了﹐下次請早﹗\n\r", ch );
      RETURN_NULL();
    }

    gold_from_char( ch, TicketCost );
    zTicket->sold = TRUE;

    act( "$N賣了一張公益彩券給$n﹐並祝$e中大獎﹗", ch, NULL, victim, TO_ALL );

    if ( !( pOrder = zTicket->order ) )
    {
      switch( number_range( 0, 3 ) )
      {
      default:
      case 0:
        msg = "好心有好報﹐銘謝惠顧";
        break;

      case 1:
        msg = "謝謝你熱心公益﹐祝你下次中大獎";
        break;

      case 2:
        msg = "彩券在手﹐希望無窮";
        break;
      }

      act( "你刮一了刮﹐上面寫著﹕『$t』", ch, msg, NULL, TO_CHAR );
      RETURN_NULL();
    }

    act( "恭禧你﹐這張彩券中了第 $2$i$0 特獎﹐獎金是 $3$I$0 兩銀子。"
      , ch, &pOrder->order, &pOrder->gold, TO_CHAR );

    if ( !over_scale( ch ) ) gold_to_char( ch, pOrder->gold );
    else send_to_char( "對不起﹐你的總資產已達上限﹐所以沒拿到錢﹗\n\r", ch );
  }

  else
  {
    send_to_char( "你的語法錯誤﹐請查詢 chance 的用法﹗\n\r", ch );
  }

  RETURN_NULL();
}

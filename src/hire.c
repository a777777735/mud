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

size_t  hire_count      args( ( CHAR_DATA * ) );

int hire_cost( MOB_INDEX_DATA * pIndex, SHOP_DATA * pShop, int type )
{
  int cost;

  PUSH_FUNCTION( "hire_cost" );

  if ( !pIndex || !pShop )
  {
    mudlog( LOG_DEBUG, "hire_cost: 缺乏來源" );
    RETURN( ( ( ( unsigned ) - 1 ) / 2 ) ) ;
  }

  switch( type )
  {
  default:
    mudlog( LOG_DEBUG, "hire_cost: 錯誤的形態 %d.", type );
    cost = ( ( unsigned ) - 1 ) / 2;
    break;

  case SHOP_BUY:

    cost = UMAX( 100, pIndex->level * 100 * pShop->profit_buy / 100 );
    break;

  case SHOP_SELL:

    cost = UMAX( 100, pIndex->level * 100 * pShop->profit_sell / 100 );
    break;
  }

  RETURN( cost );
}

FUNCTION( do_hire )
{
  char             arg[MAX_INPUT_LENGTH];
  char             name[MAX_STRING_LENGTH];
  CHAR_DATA      * keeper;
  CHAR_DATA      * mercenary;
  CHAR_DATA      * victim;
  SHOP_DATA      * pShop;
  MOB_INDEX_DATA * pIndex;
  int              loop;
  int              slot;
  int              cost;
  int              count;

  PUSH_FUNCTION( "do_hire" );

  if ( IS_NPC( ch ) || ch->hirer )
  {
    send_to_char( "你沒有權力雇請傭兵。\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->leader || ch->master )
  {
    send_to_char( "你有其他的隊友﹐所以不能雇請傭兵。\n\r", ch );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {

    if ( !ch->mercenary )
    {
      send_to_char( "你沒有任何的傭兵。\n\r", ch );
      RETURN_NULL();
    }

    clear_buffer();
    send_to_buffer(
      "\e[1;33;44m順序 等級 同 生  命 力 法     力 體     力 稱  謂 "
      "                        \e[0m\n\r" );

    count = 0;
    for ( victim = ch->mercenary; victim; victim = victim->mercenary )
    {
      send_to_buffer( "%3d. %4d %2s %4d/%4d %4d/%4d %4d/%4d %-20s\n\r"
        , ++count
        , victim->level
        , YESNO( ch->in_room == victim->in_room )
        , victim->hit, get_curr_hit( victim )
        , victim->mana, get_curr_mana( victim )
        , victim->move, get_curr_move( victim )
        , mob_name( ch, victim ) );
    }

    if ( count == 0 ) send_to_buffer( "你沒有任何的傭兵。\n\r" );
    print_buffer( ch );
  }

  else if ( !str_prefix( arg, "!list" ) )
  {
    if ( !( keeper = find_keeper( ch, SHOP_MERCENARY ) )
      || !keeper->pIndexData
      || !( pShop = keeper->pIndexData->pShop ) ) RETURN_NULL();

    if ( ch->position == POS_FIGHTING )
    {
      send_to_char( "你正在戰鬥﹐無法僱請傭兵﹗\n\r", ch );
      RETURN_NULL();
    }

    clear_buffer();

    send_to_buffer( "%s可以僱請的傭兵有﹕\n\r"
      "\e[1;33;44m順序 等級 傭  金 稱謂                                   "
      "                        \e[0m\n\r"
      , mob_name( ch, keeper ) );

    for ( count = loop = 0; loop < MAX_TRADE; loop++ )
    {
      if ( pShop->buy_type[loop] <= 0
        || !( pIndex = get_mob_index( pShop->buy_type[loop] ) ) ) continue;

      send_to_buffer( " %2d.  %3d %6d %s(%s)\n\r"
        , ++count
        , pIndex->level
        , hire_cost( pIndex, pShop, SHOP_BUY )
        , pIndex->short_descr
        , pIndex->name );
    }
    print_buffer( ch );
  }

  else if ( !str_prefix( arg, "!dismiss" ) )
  {
    if ( !( keeper = find_keeper( ch, SHOP_MERCENARY ) )
      || !keeper->pIndexData
      || !( pShop = keeper->pIndexData->pShop ) ) RETURN_NULL();

    if ( ch->position == POS_FIGHTING )
    {
      send_to_char( "你正在戰鬥﹐無法解雇傭兵﹗\n\r", ch );
      RETURN_NULL();
    }

    one_argument( argument, arg );

    if ( arg[0] == '\x0' || !is_number( arg ) )
    {
      send_to_char( "你要解雇那一位傭兵﹖\n\r", ch );
      RETURN_NULL();
    }

    count = atoi( arg );
    loop  = 0;

    for ( victim = ch->mercenary; victim; victim = victim->mercenary )
    {
      if ( ++loop == count )
      {
        if ( victim->in_room != ch->in_room )
        {
          send_to_char( "你那位傭兵不在這裡耶。\n\r", ch );
          RETURN_NULL();
        }

        if ( victim->hirer != ch )
        {
          send_to_char( "那位不是你的傭兵。\n\r", ch );
          RETURN_NULL();
        }

        if ( victim->position != POS_STANDING )
        {
          act( "$N的狀態目前不適合解雇。", ch, NULL, victim, TO_CHAR );
          RETURN_NULL();
        }

        mercenary_from_char( victim, ch );
        act( "$N不再是$n的傭兵了。", ch, NULL, victim, TO_ALL );

        if ( !over_scale( ch ) )
          gold_to_char( ch, hire_cost( victim->pIndexData, pShop, SHOP_SELL ) );

        extract_char( victim, TRUE );
        RETURN_NULL();
      }
    }

    send_to_char( "你沒有那位傭兵。\n\r", ch );
  }

  else if ( is_number( arg ) )
  {
    if ( !( keeper = find_keeper( ch, SHOP_MERCENARY ) )
      || !keeper->pIndexData
      || !( pShop = keeper->pIndexData->pShop ) ) RETURN_NULL();

    if ( ch->position == POS_FIGHTING )
    {
      send_to_char( "你正在戰鬥﹐無法僱請傭兵﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( ( loop = atoi( arg ) - 1 ) < 0
      || loop >= MAX_TRADE
      || ( slot = pShop->buy_type[loop] ) <= 0
      || !( pIndex = get_mob_index( slot ) ) )
    {
      send_to_char( "你的順序不對﹐無法雇請傭兵。\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->gold <= ( cost = hire_cost( pIndex, pShop, SHOP_BUY ) ) )
    {
      act( "你身上的錢不夠雇請$t($T)。"
        , ch, pIndex->short_descr, pIndex->name, TO_CHAR );
      RETURN_NULL();
    }

    if ( hire_count( ch ) >= max_hire )
    {
      send_to_char( "你攜帶的傭兵已經太多了。\n\r", ch );
      RETURN_NULL();
    }

    mercenary = shape_mobile( pIndex, ch->in_room );
    act( "$n花了$i兩銀子雇請$N。", ch, &cost, mercenary, TO_ALL );
    gold_from_char( ch, cost );

    sprintf( name, "%s\e[0m(%s)雇請的%s"
      , ch->cname, ch->name, mob_name( NULL, mercenary ) );

    free_string( mercenary->byname );
    mercenary->byname = str_dup( name );

    mercenary->hirer     = ch;
    mercenary->mercenary = ch->mercenary;
    ch->mercenary        = mercenary;
  }

  else
  {
    send_to_char( "指令錯誤﹐請查詢 hire 的正確用法﹗\n\r", ch );
  }

  RETURN_NULL();
}

size_t hire_count( CHAR_DATA * ch )
{
  size_t      count = 0;
  CHAR_DATA * victim;

  PUSH_FUNCTION( "hire_count" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "hire_count: 來源不正確." );
    RETURN( 0 );
  }

  for ( victim = ch->mercenary; victim; victim = victim->mercenary )
    count++;

  RETURN( count );
}

void all_mercenary_from_char( CHAR_DATA * ch )
{
  CHAR_DATA * victim;
  CHAR_DATA * zVictim;

  PUSH_FUNCTION( "all_mercenary_from_char" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "all_mercenary_from_char: 來源錯誤." );
    RETURN_NULL();
  }

  for ( victim = ch->mercenary; victim; victim = zVictim )
  {
    zVictim           = victim->mercenary;
    victim->hirer     = NULL;
    victim->mercenary = NULL;

    /* 修正名稱 */
    if ( IS_NPC( victim ) && victim->pIndexData )
    {
      free_string( victim->byname );
      victim->byname = str_dup( victim->pIndexData->short_descr );
    }
  }

  RETURN_NULL();
}

void mercenary_from_char( CHAR_DATA * victim, CHAR_DATA * ch )
{
  CHAR_DATA * prev;

  PUSH_FUNCTION( "mercenary_from_char" );

  if ( !ch || !victim || !ch->mercenary )
  {
    mudlog( LOG_DEBUG, "mercenary_from_char: 來源錯誤." );
    RETURN_NULL();
  }

  if ( victim == ch->mercenary )
  {
    ch->mercenary = victim->mercenary;
  }

  else
  {
    for ( prev = ch->mercenary; prev; prev = prev->mercenary )
    {
      if ( prev->mercenary == victim )
      {
        prev->mercenary = victim->mercenary;
        break;
      }
    }

    if ( !prev ) mudlog( LOG_DEBUG , "mercenary_from_char: 未發現到人物." );
  }

  victim->boss = NULL;

  /* 修正名稱 */
  if ( IS_NPC( victim ) && victim->pIndexData )
  {
    free_string( victim->byname );
    victim->byname = str_dup( victim->pIndexData->short_descr );
  }

  RETURN_NULL();
}

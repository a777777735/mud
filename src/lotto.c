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

#define LOTTO_BASE              2
#define LOTTO_MAX               6
#define LOTTO_PAPER_GOLD        100

int  LottoFirst   = 10;
int  LottoSecond  =  5;
int  LottoThird   =  2;
int  LottoForth   =  1;
int  lotto_count  =  0;
int  lotto_paper  =  LOTTO_PAPER_GOLD;
char nLotto[LOTTO_MAX];

int  lotto_check        args( ( OBJ_DATA * ) );

FUNCTION( do_lotto )
{
  char arg[MAX_INPUT_LENGTH];
  char buf[MAX_INPUT_LENGTH];
  int  loop;

  PUSH_FUNCTION( "do_lotto" );

  argument = one_argument( argument , arg );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  if ( !arg[0] )
  {
    clear_buffer();
    chinese_number( lotto_count, buf );
    send_to_buffer( "本期「第%s期」的樂透號碼是\e[1;32m " , buf );

    for ( loop = 0; loop < LOTTO_MAX; loop++ )
      send_to_buffer( "%c", ( nLotto[loop] + '0' ) );

    chinese_number( lotto_paper, buf );
    send_to_buffer( "\e[0m﹐每份樂透快報價錢為 %s 兩。\n\r", buf );
    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg, "check" ) )
  {
    OBJ_DATA * pObj;
    int        check;

    one_argument( argument, arg );

    if ( !arg[0] )
    {
      send_to_char( "你要拿那個物品兌獎﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( !( pObj = get_obj_carry( ch, arg ) ) )
    {
      act( "你身上沒有這樣的物品 $2$T$0 喔。" , ch, NULL, arg, TO_CHAR );
      RETURN_NULL();
    }

    /* 物品是否有餵毒 */
    if ( poison_char( ch, pObj ) ) RETURN_NULL();

    if ( ( check = lotto_check( pObj ) ) == ERRORCODE )
    {
      act( "你的$p並沒有中樂透獎。", ch, pObj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    chinese_number( check, buf );

    /* 根據第幾特獎來給獎品 */
    switch( check )
    {
    default:
      break;

    case 1:

      if ( LottoFirst > 0 )
      {
        act( "恭禧你﹐你的$p中了第$T特獎。", ch, pObj, buf, TO_CHAR );
        chinese_number( add_firman( ch, LottoFirst ), buf );
        act( "樂透公會送你$t張免死金牌。", ch, buf, NULL, TO_CHAR );
        mudlog( LOG_INFO, "[LOTTO] %s得到第一特獎.", ch->name );
      }

      break;

    case 2:

      if ( LottoSecond > 0 )
      {
        act( "恭禧你﹐你的$p中了第$T特獎。", ch, pObj, buf, TO_CHAR );
        chinese_number( add_firman( ch, LottoSecond ), buf );
        act( "樂透公會送你$t張免死金牌。", ch, buf, NULL, TO_CHAR );
        mudlog( LOG_INFO, "[LOTTO] %s得到第二特獎.", ch->name );
      }

      break;

    case 3:

      if ( LottoThird > 0 )
      {
        act( "恭禧你﹐你的$p中了第$T特獎。", ch, pObj, buf, TO_CHAR );
        chinese_number( add_firman( ch, LottoThird ), buf );
        act( "樂透公會送你$t張免死金牌。", ch, buf, NULL, TO_CHAR );
        mudlog( LOG_INFO, "[LOTTO] %s得到第三特獎.", ch->name );
      }

      break;

    case 4:

      if ( LottoForth > 0 )
      {
        act( "恭禧你﹐你的$p中了第$T特獎。", ch, pObj, buf, TO_CHAR );
        chinese_number( add_firman( ch, LottoForth ), buf );
        act( "樂透公會送你$t張免死金牌。", ch, buf, NULL, TO_CHAR );
        mudlog( LOG_INFO, "[LOTTO] %s得到第四特獎.", ch->name );
      }

      break;
    }

    /* 先銷毀原來的物品 */
    extract_obj( pObj );

    RETURN_NULL();
  }

  /* 重建號碼, 只有神族可以 */
  if ( !str_prefix( arg , "!reborn" ) && IS_GOD( ch ) )
  {
    mudlog( LOG_INFO, "[LOTTO] %s 重置樂透號碼.", ch->name );
    send_to_char( "你重新設定樂透號碼。\n\r" , ch );
    lotto_generate( -1 );
    do_lotto( ch , "" );
    RETURN_NULL();
  }

  /* 設定號碼, 只有神族可以 */
  if ( !str_prefix( arg, "!set" ) && IS_GOD( ch ) )
  {
    int  number;

    one_argument( argument , arg );

    if ( !is_number( arg ) || ( number = atoi( arg ) ) <= 0 )
    {
      send_to_char( "設定號碼錯誤。\n\r" , ch );
      RETURN_NULL();
    }

    mudlog( LOG_INFO, "[LOTTO] %s 設定樂透號碼.", ch->name );
    lotto_generate( number );
    do_lotto( ch, "" );
    RETURN_NULL();
  }

  send_to_char( "請查詢 lotto 正確的使用方法。\n\r" , ch );
  RETURN_NULL();
}

/* 每天更換樂透號碼 */
void lotto_update( void )
{
  DESCRIPTOR_DATA * man;
  CHAR_DATA       * ch;
  OBJ_DATA        * pObj;
  int               check;

  PUSH_FUNCTION( "lotto_update" );

  /* 先產生樂透號碼 */
  lotto_generate( -1 );

  /* 送出樂透快報 */
  for ( man = descriptor_list; man; man = man->next )
  {
    if ( !verify_desc( man )
      || man->connected != CON_PLAYING
      || !( ch = man->character )
      || !IS_SET( ch->turn, PLR_LOTTO )
      || ch->gold < lotto_paper
      || ch->position != POS_STANDING ) continue;

    /* 扣除快報的錢 */
    if ( lotto_paper > 0 ) gold_from_char( ch, lotto_paper );

    /* 檢查身上的物品 */
    for ( pObj = ch->carrying; pObj; pObj = pObj->next_content )
    {
      if ( pObj->wear_loc == WEAR_NONE
        && can_see_obj( ch, pObj )
        && ( check = lotto_check( pObj ) ) != ERRORCODE )
      {
        act( "$B「樂透快報」 你的$p中了第$I特獎。", ch, pObj, &check, TO_CHAR );
      }
    }
  }

  RETURN_NULL();
}

/* 產生一組樂透號碼 */
void lotto_generate( int number )
{
  int loop;

  PUSH_FUNCTION( "lotto_generate" );

  for ( loop = LOTTO_MAX - 1; loop >= 0; loop-- )
  {
    if ( number < 0 )
    {
      nLotto[loop] = ( char ) number_range( 0, 9 );
    }
    else
    {
      nLotto[loop] = number % 10;
      number      /= 10;
    }
  }

  lotto_count++;
  RETURN_NULL();
}

int lotto_check( OBJ_DATA * obj )
{
  int  loop;
  int  serial;
  int  check;

  PUSH_FUNCTION( "lotto_check" );

  /* 如果物品是空的, 一定有錯誤 */
  if ( !obj || ( serial = obj->serial.low ) == 0 ) RETURN ( ERRORCODE );

  /* 物品分類 */
  switch( obj->item_type )
  {
  default:
    mudlog( LOG_DEBUG, "lotto_check: 物品種類 %d 錯誤.", obj->item_type );
    RETURN( ERRORCODE );

  case ITEM_POTION:
  case ITEM_FURNITURE:
  case ITEM_TRASH:
  case ITEM_KEY:
  case ITEM_FOOD:
  case ITEM_MONEY:
  case ITEM_BOAT:
  case ITEM_CORPSE_NPC:
  case ITEM_CORPSE_PC:
  case ITEM_FOUNTAIN:
  case ITEM_PILL:
  case ITEM_LIGHT:
  case ITEM_ARROW:
  case ITEM_CONTAINER:
  case ITEM_DRINK_CON:

    RETURN( ERRORCODE );

  case ITEM_SCROLL:
  case ITEM_WAND:
  case ITEM_STAFF:
  case ITEM_WEAPON:
  case ITEM_TREASURE:
  case ITEM_ARMOR:
  case ITEM_MAGICSTONE:
  case ITEM_SPIRITJADE:
  case ITEM_MYSTERY:
  case ITEM_LETTER:
  case ITEM_ORE:
  case ITEM_GOLDMINE:
  case ITEM_SIGIL:
  case ITEM_VELLUM:
  case ITEM_FIREWORK:
  case ITEM_URN:
  case ITEM_BACKBAG:
    break;
  }

  /* 對號 */
  for ( check = 0, loop = LOTTO_MAX -1; loop >= 0; loop-- )
  {
    if ( nLotto[loop] == ( serial % 10 ) ) check++;
    else                                   break;
    serial /= 10;
  }

  /* 如果沒有傳回 -1, 有的話依序 1, 2, 3 傳回 */
  RETURN( ( ( check > LOTTO_BASE ) ? ( LOTTO_MAX + 1 - check ) : ERRORCODE ) );
}

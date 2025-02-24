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

/* 存放物品或是金錢的主程式 */
FUNCTION( do_deposit )
{
  char        arg1[ MAX_INPUT_LENGTH ];
  char        arg2[ MAX_INPUT_LENGTH ];
  char        buf[MAX_STRING_LENGTH];
  OBJ_DATA  * obj;
  CHAR_DATA * victim;
  int         money;
  int         fee;

  PUSH_FUNCTION( "do_deposit" );

  if ( IS_NPC( ch ) || !ch->in_room ) RETURN_NULL();

  argument = one_argument( argument , arg1 );

  if ( !arg1[0] )
  {
    send_to_char( "你想存款或是寄放東西﹐請查詢 \e[1;32mdeposit\e[0m。\n\r" , ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg1, "!donate" ) )
  {
    if ( ch->in_room->DepositMoney == FALSE )
    {
      act( "對不起﹐$r並不是錢莊喔﹗" , ch, NULL, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( !IS_NPC( ch ) && ch->level < level_limit )
    {
      act( "在你未升級到$i級前﹐請不要當闊少爺。"
        , ch, &level_limit, NULL, TO_CHAR );

      RETURN_NULL();
    }

    if ( ch->donate > 0 )
    {
      send_to_char( "在你領救濟金的這段時間﹐請不要裝富有﹗\n\r", ch );
      RETURN_NULL();
    }

    argument = one_argument( argument, arg1 );

    if ( arg1[0] == '\x0' )
    {
      send_to_char( "你要轉帳給誰呢﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( !( victim = get_char_world( ch, arg1 ) ) )
    {
      act( "找不到你的對象 $2$T$0 耶﹗", ch, NULL, arg1, TO_CHAR );
      RETURN_NULL();
    }

    if ( victim == ch )
    {
      send_to_char( "有沒有搞錯啊﹐你要轉帳給自己﹖\n\r", ch );
      RETURN_NULL();
    }

    argument = one_argument( argument, arg1 );

    if ( arg1[0] == '\x0' || !is_number( arg1 ) )
    {
      act( "你要轉多少帳給$N呢﹖", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( ( money = atoi( arg1 ) ) <= 0 )
    {
      act( "你要送紙錢給$N嗎﹖", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( money > ch->bank )
    {
      send_to_char( "你戶頭裡沒有那麼多耶﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( ( fee = money / 10 ) < 10 )
    {
      send_to_char( "轉帳金額太少了啦﹗手續費不夠啦﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( over_scale( victim ) )
    {
      act( "對不起﹐$N的資產太大了﹐無法繼續存錢﹗", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    ch->bank     -= money;
    victim->bank += ( money - fee );

    act( "你轉了$i兩銀子給$N。", ch, &money, victim, TO_CHAR );

    chinese_number( money - fee, buf );
    act( "$n轉了$t兩銀子給你。", ch, buf, victim, TO_VICT );

    RETURN_NULL();
  }

  /* 關鍵字可以是 gold coin 或是 coins 其中一個 */
  if ( !str_cmp( arg1 , "gold"  )
    || !str_cmp( arg1 , "coin"  )
    || !str_cmp( arg1 , "coins" ) )
  {
    if ( ch->in_room->DepositMoney == FALSE )
    {
      act( "$r並不是錢莊喔﹗" , ch, NULL, NULL, TO_CHAR );
      RETURN_NULL();
    }

    one_argument( argument , arg2 );

    if ( !arg2[0] || !is_number( arg2 ) )
    {
      do_deposit( ch , "" );
      RETURN_NULL();
    }

    if ( ( money = atoi( arg2 ) ) <= 0 || money > ch->gold )
    {
      send_to_char( "你存的是紙錢嗎﹖\n\r" , ch );
      RETURN_NULL();
    }

    if ( over_scale( ch ) )
    {
      send_to_char( "對不起﹐你的資產太大了﹐無法繼續存錢。\n\r", ch );
      RETURN_NULL();
    }

    gold_from_char( ch, money );
    ch->bank += money;

    act( "你存了 $i 兩﹐現在總共有 $I 兩在錢莊裡﹐謝謝你的光臨。"
      , ch, &money , &ch->bank, TO_CHAR );

    RETURN_NULL();
  }

  /* 存放物品 */
    if ( ch->in_room->StoreRoom == FALSE )
  {
    act( "$r並不是鏢局喔﹗" , ch, NULL, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( !( obj = get_obj_carry( ch, arg1 ) ) )
  {
    act( "你沒有那樣 $2$T$0 可以寄放喔。", ch, NULL, arg1, TO_CHAR );
    RETURN_NULL();
  }

  if ( obj->wear_loc != WEAR_NONE )
  {
    act( "你還把$p穿在身上耶﹐先把它脫了吧。", ch, obj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  /* 物品是否有餵毒 */
  if ( poison_char( ch, obj ) ) RETURN_NULL();

  if ( excess_filequota( ch ) )
  {
    send_to_char( "對不起﹐你的檔案太大﹐無法執行這個命令﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( !can_store( obj ) )
  {
    act( "抱歉﹐我們不能幫你存放$p。" , ch, obj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( obj->address )
  {
    act( "$p是要交給別人的信﹐怎麼自己存起來。", ch, obj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( !can_drop_obj( ch, obj ) )
  {
    act( "你不能存放$p。", ch, obj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  act( "你將$p$0寄放在本鏢局﹐謝謝光臨。", ch, obj, NULL, TO_CHAR );

  message_driver( ch, obj, ACT_WHEN_DEPOSIT );

  obj_from_char( obj );
  obj_to_char_deposit( obj , ch );

  RETURN_NULL();
}

/* 領回金錢或是物品 */
FUNCTION( do_withdraw )
{
  OBJ_DATA * obj;
  char       arg1[ MAX_INPUT_LENGTH ];
  char       arg2[ MAX_INPUT_LENGTH ];
  int        money;

  PUSH_FUNCTION( "do_withdraw" );

  if ( IS_NPC( ch ) || !ch->in_room ) RETURN_NULL();

  argument = one_argument( argument , arg1 );

  if ( !arg1[0] )
  {
    send_to_char( "你想提款或是領回東西﹐請查詢 withdraw﹗\n\r" , ch );
    RETURN_NULL();
  }

  if ( !str_cmp( arg1 , "gold"  )
    || !str_cmp( arg1 , "coin"  )
    || !str_cmp( arg1 , "coins" ) )
  {
    if ( ch->trade == FALSE )
    {
      send_to_char( "你的總資產太可怕了﹐所以無法執行這個指令﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->in_room->DepositMoney == FALSE )
    {
      act( "$r並不是錢莊喔。", ch, NULL, NULL, TO_CHAR );
      RETURN_NULL();
    }

    one_argument( argument , arg2 );

    if ( !arg2[0] || !is_number( arg2 ) )
    {
      do_withdraw( ch , "" );
      RETURN_NULL();
    }

    if ( ( money = atoi( arg2 ) ) <= 0 || money > ch->bank )
    {
      send_to_char( "你領的是冥錢嗎﹖\n\r" , ch );
      RETURN_NULL();
    }

    if ( over_scale( ch ) )
    {
      send_to_char( "你的總資產已達上限﹐無法領出錢來﹗\n\r", ch );
      RETURN_NULL();
    }

    gold_to_char( ch, money );
    ch->bank -= money;

    act( "你現在一共存了$i兩在錢莊裡﹐謝謝你的光臨。"
      , ch, &ch->bank, NULL, TO_CHAR );

    RETURN_NULL();
  }

  /* 領回物品 */
    if ( ch->in_room->StoreRoom == FALSE )
  {
    act( "$r並不是鏢局喔。", ch, NULL, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( !( obj = get_obj_deposit( ch, arg1 ) ) )
  {
    act( "你沒有那樣 $2$T$0 放在鏢局。", ch, NULL, arg1, TO_CHAR );
    RETURN_NULL();
  }

  /* 物品是否有餵毒 */
  if ( poison_char( ch, obj ) ) RETURN_NULL();

  act( "你已經把$p$0領回了﹐謝謝光臨。", ch, obj, NULL, TO_CHAR );
  obj_from_char_deposit( obj );
  message_driver( ch, obj, ACT_WHEN_WITHDRAW );

  if ( ch->carry_number + 1 > can_carry_n( ch )
    ||  get_carry_weight( ch ) + get_obj_weight( obj ) > can_carry_w( ch ) )
  {
    act( "你無法背負$p﹐所以把它放在地上﹗", ch, obj, NULL, TO_CHAR );
    obj_to_room( obj, ch->in_room );
  }
  else
  {
    obj_to_char( obj , ch );
  }

  RETURN_NULL();
}

/* 檢查鏢局或是錢莊寄放的金錢或是物品 */
FUNCTION( do_check )
{
  OBJ_DATA * obj;
  bool       found;

  PUSH_FUNCTION( "do_check" );

  if ( IS_NPC( ch ) || !ch->in_room ) RETURN_NULL();

  if ( ch->in_room->DepositMoney )
  {
    act( "你現在一共存了 $i 兩在錢莊裡﹐謝謝你的光臨。"
      , ch, &ch->bank, NULL, TO_CHAR );

    RETURN_NULL();
  }

  if ( ch->in_room->StoreRoom )
  {
    clear_buffer();
    for ( found = FALSE, obj = ch->deposit; obj; obj = obj->next_content )
    {
      found = TRUE;
      send_to_buffer( "你把%s寄放在鏢局裡。\n\r" , obj_name( ch, obj ) );
    }

    if ( !found ) send_to_buffer( "你並沒有任何的東西在這裡寄放。\n\r" );
    print_buffer( ch );

    RETURN_NULL();
  }

  act( "對不起﹐$r並不是錢莊或是鏢局。" , ch, NULL, NULL, TO_CHAR );
  RETURN_NULL();
}

/* 把東西放到人物寄放的地方 */
void obj_to_char_deposit( OBJ_DATA * obj, CHAR_DATA * ch )
{
  PUSH_FUNCTION( "obj_to_char_deposit" );

  obj->next_content = ch->deposit;
  ch->deposit       = obj;
  obj->deposit_by   = ch;
  obj->in_room      = NULL;
  obj->in_obj       = NULL;
  RETURN_NULL();
}

/* 把東西從人物的存放單裡剔除 */
void obj_from_char_deposit( OBJ_DATA * obj )
{
  CHAR_DATA * ch;
  OBJ_DATA  * prev;

  PUSH_FUNCTION( "obj_from_char_deposit" );

  if ( !( ch = obj->deposit_by ) )
  {
    mudlog( LOG_DEBUG , "Obj_from_char_deposit: 人物是空的." );
    RETURN_NULL();
  }

  if ( ch->deposit == obj )
  {
    ch->deposit = obj->next_content;
  }

  else
  {
    for ( prev = ch->deposit; prev; prev = prev->next_content )
    {
      if ( prev->next_content == obj )
      {
        prev->next_content = obj->next_content;
        break;
      }
    }

    if ( !prev )
    {
      mudlog( LOG_DEBUG , "Obj_from_char_deposit: 物品並沒有在表中." );
    }
  }

  obj->deposit_by   = NULL;
  obj->next_content = NULL;
  RETURN_NULL();
}

OBJ_DATA * get_obj_deposit( CHAR_DATA * ch, char * argument )
{
  char       arg[MAX_INPUT_LENGTH];
  OBJ_DATA * obj;
  int        number;
  int        count = 0;

  PUSH_FUNCTION( "get_obj_deposit" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG , "get_obj_deposit: 人物是空的." );
    RETURN( NULL );
  }

  number = number_argument( argument, arg );

  for ( obj = ch->deposit; obj; obj = obj->next_content )
    if ( is_name( arg, obj->name ) && ++count == number ) RETURN( obj );

  RETURN( NULL );
}

void obj_to_obj_dep( OBJ_DATA * obj, OBJ_DATA * obj_to )
{
  PUSH_FUNCTION( "obj_to_obj_dep" );

  if ( !obj || !obj_to )
  {
    mudlog( LOG_DEBUG , "obj_to_obj_dep: obj 或 obj_to 是空的" );
    RETURN_NULL();
  }

  obj->next_content = obj_to->contains;
  obj_to->contains  = obj;
  obj->in_obj       = obj_to;
  obj->in_room      = NULL;
  obj->deposit_by   = NULL;
  RETURN_NULL();
}

size_t deposit_count( CHAR_DATA * ch )
{
  int        count = 0;
  OBJ_DATA * pObj;

  PUSH_FUNCTION( "deposit_count" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "deposit_count: 來源不正確." );
    RETURN( 0 );
  }

  for ( pObj = ch->deposit; pObj; pObj = pObj->next_content ) count++;
  RETURN( count );
}

void deposit_update( void )
{
  CHAR_DATA * ch;
  int         count;
  int         money;

  PUSH_FUNCTION( "deposit_update" );

  if ( DepositMoney <= 0 ) RETURN_NULL();

  for ( ch = char_list; ch; ch = ch->next )
  {
    if ( !verify_char( ch )
      || IS_NPC( ch )
      || ( count = deposit_count( ch ) ) <= 0 ) continue;

    money = count * DepositMoney;

    if ( ch->bank >= money )
    {
      ch->bank -= money;
    }

    else
    {
      gold_from_char( ch, UMIN( ch->gold, money - ch->bank ) );
      ch->bank = 0;
    }
  }

  RETURN_NULL();
}

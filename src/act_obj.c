/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

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
#include "merc.h"

/* 區域函數 */
#define CD CHAR_DATA
#define OD OBJ_DATA
void    get_obj                  args( ( CD * , OD * , OD * ) );
bool    remove_obj               args( ( CD * , int , bool, bool ) );
int     get_cost                 args( ( CD * , OD * , bool ) );
void    obj_cast                 args( ( OD * , CD *, CD *, OD *K ) );
#undef  CD
#undef  OD

void get_obj( CHAR_DATA * ch, OBJ_DATA * obj, OBJ_DATA * container )
{
  PUSH_FUNCTION( "get_obj" )

  if ( !ch || !obj )
  {
    mudlog( LOG_DEBUG, "get_obj: 缺乏來源." );
    RETURN_NULL();
  }

  if ( obj->Takeable == FALSE )
  {
    act( "你無法拿起$p。", ch, obj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( obj->item_type == ITEM_CORPSE_PC || obj->item_type == ITEM_CORPSE_NPC )
  {
    act( "你想扛著$p幹什麼﹖", ch, obj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( ch->carry_number + 1 > can_carry_n( ch ) )
  {
    act( "$t﹕你不能攜帶這麼多的東西。", ch , obj->name, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( get_carry_weight( ch ) + get_obj_weight( obj ) > can_carry_w( ch ) )
  {
    act( "$t﹕你不能攜帶這麼重的東西。" ,ch , obj->name, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( !IS_NPC( ch ) && excess_filequota( ch ) )
  {
    act( "$t﹕你的檔案太大﹐這個命令取消﹗", ch, obj->name, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( container )
  {
    act( "$n從$P拿出$p。", ch, obj, container, TO_ALL );
    message_driver( ch, obj, ACT_WHEN_GET );
    obj_from_obj( obj );
  }

  else
  {
    act( "$n撿起$p。", ch, obj, NULL, TO_ALL );
    message_driver( ch, obj, ACT_WHEN_GET );
    obj_from_room( obj );
  }

  if ( obj->item_type == ITEM_MONEY )
  {
    if ( !over_scale( ch ) )
    {
      gold_to_char( ch, obj->value[0] );
    }
    else
    {
      send_to_char( "你的總資產已達上限﹐無法繼續拿錢﹗\n\r", ch );
    }

    extract_obj( obj );
  }

  else
  {
    obj_to_char( obj, ch );
  }

  RETURN_NULL();
}

FUNCTION( do_search )
{
  bool       found;
  OBJ_DATA * corpse;
  OBJ_DATA * obj;
  OBJ_DATA * obj_next;

  PUSH_FUNCTION( "do_search" );

  if ( IS_NPC( ch ) || !ch->pcdata || !ch->in_room ) RETURN_NULL();

  if ( !ch->pcdata
    || !( corpse = ch->pcdata->corpse )
    || !corpse->in_room
    || corpse->item_type != ITEM_CORPSE_PC
    || corpse->corpse_owner != ch )
  {
    send_to_char( "對不起﹐目前沒有發現到你的屍體﹐所以無法搜尋。\n\r", ch );
    RETURN_NULL();
  }

  if ( corpse->in_room != ch->in_room )
  {
    send_to_char( "對不起﹐你還沒有取回(\e[1;32mcrave\e[0m)你的屍體喔﹗\n\r"
      , ch );
    RETURN_NULL();
  }

  for ( found = FALSE, obj = corpse->contains; obj; obj = obj_next )
  {
    obj_next = obj->next_content;

    if ( can_see_obj( ch, obj ) )
    {
      found = TRUE;

      /* 物品是否有餵毒 */
      if ( !poison_char( ch, obj ) ) get_obj( ch, obj, corpse );
    }
  }

  if ( !found )
    send_to_char( "你屍體裡沒有任何你看的到的裝備了﹐你可以奉獻你的屍體了"
      "(\e[1;32msac corpse\e[0m)。\n\r", ch );
  RETURN_NULL();
}

FUNCTION( do_get )
{
  char       arg1[MAX_INPUT_LENGTH];
  char       arg2[MAX_INPUT_LENGTH];
  OBJ_DATA * obj;
  OBJ_DATA * obj_next;
  OBJ_DATA * container;
  bool       found;

  PUSH_FUNCTION( "do_get" );

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );

  if ( !arg2[0] )
  {
    if ( str_cmp( arg1, "all" ) && str_prefix( "all.", arg1 ) )
    {
      if ( !( obj = get_obj_list( ch, arg1, ch->in_room->contents ) ) )
      {
        act( "我在$r沒有看到任何的$t。" , ch, arg1, NULL, TO_CHAR );
        RETURN_NULL();
      }

      /* 物品是否有餵毒 */
      if ( !poison_char( ch, obj ) ) get_obj( ch, obj, NULL );
    }

    else
    {
      for ( found = FALSE, obj = ch->in_room->contents; obj; obj = obj_next )
      {
        obj_next = obj->next_content;
        if ( ( !arg1[3] || is_name( &arg1[4], obj->name ) )
          && can_see_obj( ch, obj ) )
        {
          found = TRUE;

          /* 物品是否有餵毒 */
          if ( !poison_char( ch, obj ) ) get_obj( ch, obj, NULL );
        }
      }

      if ( !found )
      {
        if ( !arg1[3] )
          send_to_char( "這裡並沒有看到任何東西。\n\r", ch );
        else
          act( "我在$r沒有看到任何的$2$t$0。", ch, &arg1[4], NULL, TO_CHAR );
      }
    }
  }

  else
  {
    if ( !str_cmp( arg2, "all" ) || !str_prefix( "all.", arg2 ) )
    {
      send_to_char( "你不能這樣做。\n\r", ch );
      RETURN_NULL();
    }

    if ( !( container = get_obj_here( ch, arg2 ) ) )
    {
      act( "我在$r並沒有看到任何的$t。",ch , arg2, NULL, TO_CHAR );
      RETURN_NULL();
    }

    /* 物品是否有餵毒 */
    if ( poison_char( ch, container ) ) RETURN_NULL();

    switch ( container->item_type )
    {
    default:
      act( "$p並不是一個容器。", ch, container, NULL, TO_CHAR );
      RETURN_NULL();

    case ITEM_CONTAINER :
    case ITEM_CORPSE_NPC:
      break;

    case ITEM_CORPSE_PC :

      if ( container->corpse_owner != ch )
      {
        send_to_char( "這並不是你的屍體喔﹐你不能拿取他人屍"
          "體裡的東西。\n\r", ch );
        RETURN_NULL();
      }

      if ( IS_NPC( ch ) )
      {
        send_to_char( "你不能這樣做。\n\r", ch );
        RETURN_NULL();
      }

      break;
    }

    if ( IS_SET( container->value[1], CONT_LOCKED ) )
    {
      act( "$p被鎖住了﹐打不開﹗", ch, container, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( IS_SET( container->value[1], CONT_CLOSED ) )
    {
      act( "$p被關了起來了。", ch , container, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( str_cmp( arg1, "all" ) && str_cmp( "all.", arg1 ) )
    {
      if ( !( obj = get_obj_list( ch, arg1, container->contains ) ) )
      {
        act( "我在$t並沒有看到任何的東西。",ch , arg2, NULL, TO_CHAR );
        RETURN_NULL();
      }

      /* 物品是否有餵毒 */
      if ( !poison_char( ch, obj ) ) get_obj( ch, obj, container );
    }

    else
    {
      for ( found = FALSE, obj = container->contains; obj; obj = obj_next )
      {
        obj_next = obj->next_content;
        if ( ( !arg1[3] || is_name( &arg1[4], obj->name ) )
          && can_see_obj( ch, obj ) )
        {
          found = TRUE;

          /* 物品是否有餵毒 */
          if ( !poison_char( ch, obj ) ) get_obj( ch, obj, container );
        }
      }

      if ( !found )
      {
        if ( !arg1[3] )
          act( "我在$t並沒有看到任何的東西。", ch, arg2, NULL, TO_CHAR );

        else
          act( "我在$t並沒有看到你想要的東西。\n\r",ch ,arg2 ,NULL, TO_CHAR );
      }
    }
  }
  RETURN_NULL();
}

FUNCTION( do_toss )
{
  char             arg1[MAX_INPUT_LENGTH];
  char             arg2[MAX_INPUT_LENGTH];
  OBJ_DATA       * pGold;
  OBJ_DATA       * obj;
  OBJ_DATA       * treasure;
  OBJ_INDEX_DATA * pIndex;
  OBJ_INDEX_DATA * pTreasure;
  int              level;
  int              loop;
  int              count;

  PUSH_FUNCTION( "do_toss" );

  if ( IS_NPC( ch ) || !ch->in_room ) RETURN_NULL();

  if ( !IS_NPC( ch ) && ch->level < level_limit )
  {
    act( "在你升級到$i級前﹐請不要亂扔東西。", ch, &level_limit, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( ch->donate > 0 )
  {
    send_to_char( "在你領救濟金的這段時間﹐請不要亂丟東西好嗎﹖\n\r", ch );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg1 );
             one_argument( argument, arg2 );

  if ( arg1[0] == '\x0' || arg2[0] == '\x0' )
  {
    send_to_char( "你要把什麼東西扔到那裡去呢﹖\n\r", ch );
    RETURN_NULL();
  }

  if ( !( pGold = get_obj_here( ch, arg2 ) ) )
  {
    act( "我在$r沒有看到任何的$T。", ch, NULL, arg2, TO_CHAR );
    RETURN_NULL();
  }

  if ( pGold->item_type != ITEM_GOLDMINE )
  {
    act( "對不起﹐$p不是個聚寶盆耶﹐所以你不能扔東西到那裡去﹗"
      , ch, pGold, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( !( obj = get_obj_carry( ch, arg1 ) ) )
  {
    act( "你沒有那樣 $2$T$0 吧。", ch, NULL, arg1, TO_CHAR );
    RETURN_NULL();
  }

  if ( obj->wear_loc != WEAR_NONE )
  {
    act( "你還把$p穿在身上耶﹐先把它脫了吧。", ch, obj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  /* 物品是否有餵毒 */
  if ( poison_char( ch, obj ) ) RETURN_NULL();

  if ( !can_drop_obj( ch, obj ) )
  {
    act( "你不能丟棄$p﹐所以不能亂扔。", ch, obj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( obj->address )
  {
    act( "$p是要交給別人的信﹐怎麼可以亂扔。", ch, obj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  obj_from_char( obj );

  level = 0;

  if ( obj->level >= pGold->value[2] )
  {
    level = obj->level;
    if ( obj->max_armor > 0 ) level = level * obj->armor / obj->max_armor;
  }

  pGold->value[1] += UMIN( MAX_LEVEL, UMAX( 1, level  ) );

  if ( pGold->value[1] >= pGold->value[0] )
  {
    pTreasure = NULL;
    count     = 0;

    for ( loop = 5; loop < MAX_OBJECT_VALUE; loop++ )
    {
      if ( ( pIndex = get_obj_index( pGold->value[loop] )  )
        && number_range( 0, count++ ) == 0 )
        pTreasure = pIndex;
    }

    if ( pTreasure )
    {
      treasure =
        create_object( pTreasure, URANGE( 1, pTreasure->level, LEVEL_HERO ) );

      obj_to_room( treasure, ch->in_room );

      act( "突然﹐$P冒出一陣濃煙﹐等到大家回神時﹐地上竟然多了一件$p。"
        , ch, treasure, pGold, TO_ALL );
    }

    else
    {
      send_to_char( "唉﹐真是悲慘﹐我們的努力竟然被上天忽略了﹗\n\r", ch );
    }

    pGold->value[1] = 0;
  }

  else
  {
    act( "$n扔了$p到$P﹐$p剎那間就成了一陣青煙﹐但是耳邊傳來一陣聲音﹐不夠不夠﹗"
      , ch, obj, pGold, TO_ALL );
  }

  extract_obj( obj );
  RETURN_NULL();
}

FUNCTION( do_put )
{
  char       arg1[MAX_INPUT_LENGTH];
  char       arg2[MAX_INPUT_LENGTH];
  OBJ_DATA * container;
  OBJ_DATA * obj;
  OBJ_DATA * obj_next;

  PUSH_FUNCTION( "do_put" );

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );

  if ( !arg1[0] || !arg2[0] )
  {
    send_to_char( "把什麼東西放到什麼地方去﹖\n\r", ch );
    RETURN_NULL();
  }

  if ( !str_cmp( arg2, "all" ) || !str_cmp( "all.", arg2 ) )
  {
    send_to_char( "你不能這樣做。\n\r", ch );
    RETURN_NULL();
  }

  if ( !( container = get_obj_here( ch, arg2 ) ) )
  {
    act( "我在$r沒有看到任何的$T。", ch, NULL, arg2, TO_CHAR );
    RETURN_NULL();
  }

  if ( container->item_type != ITEM_CONTAINER )
  {
    act( "$p不是一個容器。", ch, container, NULL, TO_CHAR );
    RETURN_NULL();
  }

    if ( IS_SET( container->value[1], CONT_LOCKED ) )
    {
      act( "$p被鎖住了﹐打不開﹗", ch, container, NULL, TO_CHAR );
      RETURN_NULL();
    }

  if ( IS_SET( container->value[1], CONT_CLOSED ) )
  {
    act( "$p被關起來了。", ch, container, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( str_cmp( arg1, "all" ) && str_prefix( "all.", arg1 ) )
  {
    if ( !( obj = get_obj_carry( ch, arg1 ) ) )
    {
      act( "你並沒有那個 $2$T$0﹐不是嗎﹖", ch, NULL, arg1, TO_CHAR );
      RETURN_NULL();
    }

    if ( obj == container )
    {
      act( "你不能把$p放到其他的東西裡面。", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( obj->address )
    {
      act( "$p是要交給別人的信﹐怎麼自己收起來。", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( !can_drop_obj( ch, obj ) )
    {
      act( "你不能丟棄$p。", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( get_obj_weight( obj ) + get_obj_weight( container )
         > container->value[0] )
    {
      act( "$p太小了﹐裝不下$P。", ch, container, obj, TO_CHAR );
      RETURN_NULL();
    }

    obj_from_char( obj );
    obj_to_obj( obj, container );
    act( "$n把$p放到$P裡面去。", ch, obj, container, TO_ALL );

    message_driver( ch, obj, ACT_WHEN_PUT );
  }

  else
  {
    for ( obj = ch->carrying; obj; obj = obj_next )
    {
      obj_next = obj->next_content;

      if ( ( !arg1[3] || is_name( &arg1[4], obj->name ) )
        && can_see_obj( ch, obj )
        && obj->wear_loc == WEAR_NONE
        && obj != container
        && can_drop_obj( ch, obj )
        && !obj->address
        && get_obj_weight( obj ) + get_obj_weight( container )
           <= container->value[0] )
      {
        obj_from_char( obj );
        obj_to_obj( obj, container );
        act( "$n把$p放到$P裡面去。", ch, obj, container, TO_ALL );
        message_driver( ch, obj, ACT_WHEN_PUT );
      }
    }
  }
  RETURN_NULL();
}

FUNCTION( do_store )
{
  int         loop;
  int         level;
  int         count;
  int         total;
  char        arg[MAX_INPUT_LENGTH];
  OBJ_DATA  * obj;
  CLUB_DATA * pClub;

  PUSH_FUNCTION( "do_store" );

  if ( IS_NPC( ch ) || !ch->in_room ) RETURN_NULL();

  if ( !IS_NPC( ch ) && ch->level < level_limit )
  {
    act( "在你升級到$i級前﹐請不要亂送東西。"
      , ch, &level_limit, NULL, TO_CHAR );

    RETURN_NULL();
  }

  if ( ch->donate > 0 )
  {
    send_to_char( "在你領救濟金的這段時間﹐請不要亂丟東西﹗\n\r", ch );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg );

  if ( !str_prefix( arg, "!list" ) )
  {
    argument = one_argument( argument, arg );

    if ( !ch->club )
    {
      send_to_char( "你沒有參加任何幫派﹐怎麼能存放裝備呢﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( !( pClub = ch->in_room->club ) || pClub != ch->club )
    {
      send_to_char( "這裡不是你幫派的聚集點﹐所以不能查詢裝備﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( arg[0] == '\x0' )
    {
      clear_buffer();
      for ( total = 0, level = 1; level <= LEVEL_HERO; level++ )
      {
        for ( count = loop = 0; loop < MAX_STAFF; loop++ )
          if ( pClub->staff[level][loop] ) count++;

        if ( count <= 0 ) continue;
        if ( total > 0 && ( total % 4 ) == 0 ) send_to_buffer( "\n\r" );

        total++;
        send_to_buffer( "等級﹕%2d 件數﹕\e[1;32m%2d\e[0m   "
          , level, count );
      }

      if ( total == 0 ) send_to_buffer( "你的幫派真是窮啊﹐半個裝備也沒有﹗\n\r" );
      else              send_to_buffer( "\n\r" );

      print_buffer( ch );
      RETURN_NULL();
    }

    if ( !is_number( arg )
      || ( level = atoi( arg ) ) <= 0
      || level > LEVEL_HERO )
    {
      send_to_char( "對不起﹐你的等級參數不合理﹗\n\r", ch );
      RETURN_NULL();
    }

    clear_buffer();
    send_to_buffer( "%s\e[0m(%s) 的幫庫﹐等級 %d 的所有裝備﹕\n\r"
      , pClub->cname, pClub->name, level );

    for ( count = loop = 0; loop < MAX_STAFF; loop++ )
    {
      if ( !( obj = pClub->staff[level][loop] ) ) continue;

      if ( count == 0 )
        send_to_buffer( "\e[1;33;44m順序 裝備的類別 裝備的名稱           "
          "                                           \e[0m\n\r" );

      send_to_buffer( "%4d %-10s %s\n\r"
        , ++count
        , item_kind_name( obj->item_type )
        , obj_name( ch, obj ) );
    }

    if ( count == 0 ) send_to_buffer( "沒有任何的裝備﹗\n\r", ch );
    print_buffer( ch );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "!withdraw" ) )
  {
    if ( !ch->club )
    {
      send_to_char( "你沒有參加任何幫派﹐怎麼能存放裝備呢﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( !( pClub = ch->in_room->club ) || pClub != ch->club )
    {
      send_to_char( "這裡不是你幫派的聚集點﹐所以不能存放裝備﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( excess_filequota( ch ) )
    {
      send_to_char( "對不起﹐你的檔案太大﹐無法執行這個命令﹗\n\r", ch );
      RETURN_NULL();
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\x0'
      || !is_number( arg )
      || ( level = atoi( arg ) ) <= 0
      || level > LEVEL_HERO )
    {
      send_to_char( "你的等級參數不合理﹗\n\r", ch );
      RETURN_NULL();
    }

    argument = one_argument( argument, arg );
    if ( arg[0] == '\x0'
      || !is_number( arg )
      || ( count = atoi( arg ) ) <= 0
      || count >= MAX_STAFF )
    {
      send_to_char( "對不起﹐你的順序參數不合理﹗\n\r", ch );
      RETURN_NULL();
    }

    for ( total = loop = 0; loop < MAX_STAFF; loop++ )
    {
      if ( ( obj = pClub->staff[level][loop] ) && ++total == count )
      {
        obj->club                 = NULL;
        pClub->staff[level][loop] = NULL;

        obj_to_room( obj, ch->in_room );

        act( "$n把幫庫裡的$p拿了出來﹗", ch, obj, NULL, TO_ALL );
        RETURN_NULL();
      }
    }

    send_to_char( "對不起﹐幫庫裡沒有這項裝備﹗\n\r", ch );
    RETURN_NULL();
  }

  else
  {
    if ( !ch->club )
    {
      send_to_char( "你沒有參加任何幫派﹐怎麼能存放裝備呢﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( !( pClub = ch->in_room->club ) || pClub != ch->club )
    {
      send_to_char( "這裡不是你幫派的聚集點﹐所以不能存放裝備﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !IS_NPC( ch ) && ch->level < level_limit )
    {
      act( "在你升級到$i級前﹐請不要隨便丟棄裝備。"
        , ch, &level_limit, NULL, TO_CHAR );

      RETURN_NULL();
    }

    if ( ch->donate > 0 )
    {
      send_to_char( "在你領救濟金的這段時間﹐請不要亂丟裝備﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !( obj = get_obj_carry( ch, arg ) ) )
    {
      act( "對不起﹐你沒有那樣物品 $2$T$0。", ch, NULL, arg, TO_CHAR );
      RETURN_NULL();
    }

    if ( obj->wear_loc != WEAR_NONE )
    {
      act( "你還把$p穿在身上耶﹐先把它脫了吧。", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    /* 物品是否有餵毒 */
    if ( poison_char( ch, obj ) ) RETURN_NULL();

    if ( !can_store( obj ) )
    {
      act( "抱歉﹐$p我們不能幫你存放。" , ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( obj->address )
    {
      act( "$p是要交給別人的信﹐怎麼可以輕易丟棄。", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( !can_drop_obj( ch, obj ) )
    {
      act( "$p是你的傳家寶﹐你不能丟棄它。", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( ( level = obj->level ) <= 0 || level > LEVEL_HERO )
    {
      act( "對不起﹐$p的等級不適合來存放﹗", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    for ( loop = 0; loop < MAX_STAFF; loop++ )
    {
      if ( pClub->staff[level][loop] == NULL )
      {
        obj_from_char( obj );

        obj->club                 = pClub;
        pClub->staff[level][loop] = obj;

        act( "$n已經把$p放在$Y幫庫之中﹗", ch, obj, pClub, TO_ALL );

        RETURN_NULL();
      }
    }

    send_to_char( "對不起﹐你的幫庫已經滿載﹐無法再存放了﹗\n\r", ch );
    RETURN_NULL();
  }

  RETURN_NULL();
}

FUNCTION( do_drop )
{
  char       arg[MAX_INPUT_LENGTH];
  bool       found;
  OBJ_DATA * obj;
  OBJ_DATA * obj_next;
  int        amount;

  PUSH_FUNCTION( "do_drop" );

  if ( ch->trade == FALSE )
  {
    send_to_char( "你的總資產太可怕了﹐所以無法執行這個指令﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( !IS_NPC( ch ) && ch->level < level_limit )
  {
    act( "在你升級到$t級前﹐請不要隨便丟東西。"
      , ch, &level_limit, NULL, TO_CHAR );

    RETURN_NULL();
  }

  if ( ch->donate > 0 )
  {
    send_to_char( "在你領救濟金的這段時間﹐請不要隨便丟東西﹗\n\r", ch );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg );

  if ( is_number( arg ) )
  {
    amount   = atoi(arg);
    argument = one_argument( argument, arg );

    if ( amount <= 0 || ( str_prefix( arg, "coins" ) ) )
    {
      act( "對不起﹐你沒有那樣東西 $2$T$0。", ch, NULL, arg, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch->gold < amount )
    {
      send_to_char( "少來了﹐你沒有那麼多錢。\n\r", ch );
      RETURN_NULL();
    }

    gold_from_char( ch, amount );
    if ( ch->in_room ) gold_to_room( ch->in_room, amount );

    act( "$n丟掉一些金子在地上。", ch, NULL, NULL, TO_ALL );
    RETURN_NULL();
  }

  if ( str_cmp( arg, "all" ) && str_prefix( "all.", arg ) )
  {
    if ( !( obj = get_obj_carry( ch, arg ) ) )
    {
      act( "你沒有那樣物品 $2$T$0。", ch, NULL, arg, TO_ALL );
      RETURN_NULL();
    }

    if ( ch->in_room && ch->in_room->club && can_store( obj ) )
    {
      act( "$p可以用存放($2store$0)的喔﹗", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( obj->address )
    {
      act( "$p是要交給別人的信﹐怎麼可以輕易丟棄。", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( !can_drop_obj( ch, obj ) )
    {
      act( "你不能丟棄$p。", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    act( "$n丟掉$p。", ch, obj, NULL, TO_ALL );

    message_driver( ch, obj, ACT_WHEN_DROP );

    obj_from_char( obj );

    if ( obj->disappear )
    {
      act( "$p看起來一文不值﹐沒有人會注意到它的存在。", ch, obj, NULL, TO_ROOM );
      extract_obj( obj );
    }

    else
    {
      obj_to_room( obj, ch->in_room );
    }
  }

  else
  {
    for ( found = FALSE, obj = ch->carrying; obj; obj = obj_next )
    {
      obj_next = obj->next_content;

      if ( ( !arg[3] || is_name( &arg[4], obj->name ) )
        && can_see_obj( ch, obj )
        && obj->wear_loc == WEAR_NONE
        && can_drop_obj( ch, obj )
        && !obj->address )
      {
        found = TRUE;

        if ( ch->in_room && ch->in_room->club && can_store( obj ) )
        {
          act( "$p可以用存放($2store$0)的喔!", ch, obj, NULL, TO_CHAR );
          continue;
        }

        act( "$n丟掉$p。", ch, obj, NULL, TO_ALL );

        message_driver( ch, obj, ACT_WHEN_DROP );

        obj_from_char( obj );

        if ( obj->disappear )
        {
          act( "$p看起來一文不值﹐沒有人會注意到它的存在。"
            , ch, obj, NULL, TO_ROOM );
          extract_obj( obj );
        }

        else
        {
          obj_to_room( obj, ch->in_room );
        }
      }
    }

    if ( !found )
    {
      if ( !arg[3] )
        send_to_char( "你身上沒有任何東西﹐怎麼丟呢﹖\n\r", ch );
      else
        act( "你身上並沒有攜帶任何的$T。", ch, NULL, &arg[4], TO_CHAR );
    }
  }

  RETURN_NULL();
}

FUNCTION( do_give )
{
  char             arg1[MAX_INPUT_LENGTH];
  char             arg2[MAX_INPUT_LENGTH];
  CHAR_DATA      * victim;
  OBJ_DATA       * obj;
  OBJ_DATA       * target;
  OBJ_INDEX_DATA * pIndex;
  int              amount;

  PUSH_FUNCTION( "do_give" );

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );

  if ( !IS_NPC( ch ) && ch->level < level_limit )
  {
    act( "在你升級到$i級前﹐請不要隨便給其他人東西。"
      , ch, &level_limit, NULL, TO_CHAR );

    RETURN_NULL();
  }

  if ( ch->donate > 0 )
  {
    send_to_char( "在你領救濟金的這段時間﹐請不要隨便給別人東西﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->trade == FALSE )
  {
    send_to_char( "你的總資產太可怕了﹐所以無法執行這個指令﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( !arg1[0] || !arg2[0] )
  {
    send_to_char( "你要把什麼東西給誰﹖\n\r", ch );
    RETURN_NULL();
  }

  if ( is_number( arg1 ) )
  {
    if ( ( amount = atoi( arg1 ) ) <= 0 || ( str_prefix( arg2, "coins" ) ) )
    {
      act( "對不起﹐你沒有那個東西 $2$T$0。", ch, NULL, arg1, TO_CHAR );
      RETURN_NULL();
    }

    argument = one_argument( argument, arg2 );

    if ( !arg2[0] )
    {
      send_to_char( "你要把什麼東西給誰﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( !( victim = get_char_room( ch, arg2 ) ) )
    {
      act( "你要給的人 $2$T$0 不在這間房間裡。", ch, NULL, arg2, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch == victim )
    {
      send_to_char( "你幹嘛給自己錢啊﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->gold < amount )
    {
      send_to_char( "嘿嘿﹐你沒有那麼多錢吧。\n\r", ch );
      RETURN_NULL();
    }

    gold_from_char( ch, amount );

    if ( over_scale( victim ) )
    {
      act( "$N的總資產已達上限﹗", ch, NULL, victim, TO_CHAR );
      act( "$N的總資產已達上限﹗", ch, NULL, victim, TO_VICT );
      RETURN_NULL();
    }

    else
    {
      gold_to_char( victim, amount );
    }

    act( "$n給$N一些金子。", ch, NULL, victim, TO_ALL );

    if ( is_affected( victim, SLOT_MASK ) )
      act( "你的易容術似乎被$N識破了﹗", ch, NULL, victim, TO_CHAR );

    send_to_char( "好了﹗\n\r", ch );
    mprog_bribe_trigger( victim, ch, amount );
    RETURN_NULL();
  }

  if ( !( obj = get_obj_carry( ch, arg1 ) ) )
  {
    act( "你沒有那樣東西 $2$T$0 吧。", ch, NULL, arg1, TO_CHAR );
    RETURN_NULL();
  }

  if ( obj->wear_loc != WEAR_NONE )
  {
    act( "你還把$p穿在身上耶﹐先把它脫了吧。", ch, obj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( !( victim = get_char_room( ch, arg2 ) ) )
  {
    act( "你要給的人 $2$T$0 不在這間房間裡面。", ch, NULL, arg2, TO_CHAR );
    RETURN_NULL();
  }

  if ( ch == victim )
  {
    send_to_char( "你幹嗎給自己東西呢﹖\n\r", ch );
    RETURN_NULL();
  }

  /* 物品是否有餵毒 */
  if ( poison_char( ch, obj ) ) RETURN_NULL();

  if ( obj->address )
  {
    if ( obj->address != victim )
    {
      act( "$p不是給我的吧﹐你搞錯了。", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    act( "$N說道﹕「謝謝﹐這封信我已經收到了﹗」", ch, NULL, victim, TO_CHAR );

    obj_from_char( obj );
    extract_obj( obj );


    if ( ch->level >= LEVEL_HERO )
    {
      if ( number_range( 0, 20 ) == 0 )
      {
        if ( add_firman( ch, 1 ) > 0 )
          send_to_char( "因為你的奔波﹐我特地送給你我的傳家寶﹐"
                        "免死金牌一張。\n\r", ch );
      }

      else
      {
        if ( !over_scale( ch ) )
        {
          amount = number_range( obj->cost, obj->cost + 500 );
          gold_to_char( ch, amount );

          act( "因為你的長途奔波﹐我特地送你$i兩銀子。"
            , ch, &amount, NULL, TO_CHAR );
        }
        else
        {
          send_to_char( "因為你的總資產已達上限﹐無法拿到半毛錢﹗\n\r", ch );
        }
      }
    }

    else
    {
      if ( number_range( 0, 3000 ) <= ch->level )
      {
        if ( add_firman( ch, 1 ) > 0 )
          send_to_char( "因為你的奔波﹐我特地送給你我的傳家寶﹐"
                        "免死金牌一張。\n\r", ch );
      }

      else
      {
        if ( !over_scale( ch ) )
        {
          amount = number_range( obj->cost, obj->cost + 800 );

          gold_to_char( ch, amount );
          act( "因為你的長途奔波﹐我特地送你$i兩銀子。"
            , ch, &amount, NULL, TO_CHAR );
        }

        else
        {
          send_to_char( "因為你的總資產已達上限﹐無法拿到半毛錢﹗\n\r", ch );
        }
      }
    }

    RETURN_NULL();
  }

  /* 若是礦物 */
  if ( obj->item_type == ITEM_ORE
    && victim->pIndexData
    && victim->pIndexData->pShop
    && victim->pIndexData->pShop->type == SHOP_SMITH )
  {
    if ( !IS_NPC( ch ) && excess_filequota( ch ) )
    {
      send_to_char( "對不起﹐你的檔案太大﹐這個指令無效﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !( pIndex = get_obj_index( obj->value[0] ) )
      || obj->value[2] <= 0
      || ( amount = obj->value[3] ) <= 0 )
    {
      act( "$N說道﹕「看來這個$p平常的打緊﹐給我也沒用﹗」"
        , ch, obj, victim, TO_CHAR );

      RETURN_NULL();
    }

    if ( victim->level < obj->value[1] )
    {
      act( "$N說道﹕「這顆$p的確不錯﹐但是我無能為力﹐另請高明吧﹗」"
        , ch, obj, victim, TO_CHAR );

      RETURN_NULL();
    }

    if ( ch->gold < obj->value[3] )
    {
      act( "$N說道﹕「打造這個東西至少要$i兩銀子吧﹐還不一定會成功\﹗」"
        , ch, &amount, victim, TO_CHAR );

      RETURN_NULL();
    }

    act( "$N揮著汗水﹐一邊努力地精鍊著$p﹗", ch, obj, victim, TO_ALL );

    if ( number_range( 0, 10000 - victim->level * 10 ) >= obj->value[2] )
    {
      amount = UMAX( 20, amount / 10 );

      act( "$N說道﹕「這個東西好像無法一時搞定﹐先收點工本費$i兩銀子吧﹗」"
        , ch, &amount, victim, TO_CHAR );

      gold_from_char( ch, amount );
      RETURN_NULL();
    }

    else
    {
      target = create_object( pIndex, -1 );
      act( "大功\告成﹐$p終於打造成$P﹐拿去穿穿看吧﹗", ch, obj, target, TO_CHAR );

      if ( target->Takeable == FALSE
        || ( ch->carry_number + get_obj_number( target ) > can_carry_n( ch ) )
        || ( get_carry_weight( ch ) + get_obj_weight( target ) > can_carry_w( ch ) ) )
      {
        act( "$N說道﹕「因為$n拿不動$p﹐所以我把它放到地上了﹗」"
          , ch, target, victim, TO_ALL );

        obj_to_room( target, ch->in_room );
      }

      else
      {
        act( "$N把打造好的$p拿給了$n﹗", ch, target, victim, TO_ALL );
        obj_to_char( target, ch );
      }

      act( "$N向你收了$i兩銀子以當工資﹗", ch, &amount, victim, TO_CHAR );

      obj_from_char( obj );
      extract_obj( obj );
    }

    RETURN_NULL();
  }

  if ( !can_drop_obj( ch, obj ) )
  {
    act( "你不能丟棄$p﹐所以不能給別人。", ch, obj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( victim->carry_number + get_obj_number( obj ) > can_carry_n( victim ) )
  {
    act( "$N似乎拿不動那麼多東西。", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( get_carry_weight( victim ) + get_obj_weight( obj ) > can_carry_w( victim ) )
  {
    act( "$N似乎拿不動那麼重的東西。", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( !can_see_obj( victim, obj ) )
  {
    act( "$N看不見這樣東西。", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( !IS_NPC( victim ) && excess_filequota( victim ) )
  {
    act( "對不起﹐$N的檔案太大﹐無法再把東西給$E﹗", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  obj_from_char( obj );
  obj_to_char( obj, victim );
  MOBtrigger = FALSE;

  act( "$n把$p給了$N。" , ch, obj, victim, TO_NOTVICT );
  act( "$n給你$p。"     , ch, obj, victim, TO_VICT    );
  act( "你把$p給了$N。" , ch, obj, victim, TO_CHAR    );

  if ( is_affected( victim, SLOT_MASK ) )
    act( "遭了﹐你的易容術似乎被$N給識破了﹗", victim, NULL, ch, TO_CHAR );

  message_driver( ch, obj, ACT_WHEN_GIVE );
  mprog_give_trigger( victim , ch , obj );
  RETURN_NULL();
}

FUNCTION( do_fill )
{
  char       arg[MAX_INPUT_LENGTH];
  bool       found;
  OBJ_DATA * obj;
  OBJ_DATA * fountain;

  PUSH_FUNCTION( "do_fill" );

  one_argument( argument, arg );

  if ( !( obj = get_obj_carry( ch, arg ) ) )
  {
    act( "你沒有那樣物品 $2$T$0。", ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  /* 物品是否有餵毒 */
  if ( poison_char( ch, obj ) ) RETURN_NULL();

  for ( found = FALSE, fountain = ch->in_room->contents; fountain;
      fountain = fountain->next_content )
  {
    if ( fountain->item_type == ITEM_FOUNTAIN )
    {
      found = TRUE;
      break;
    }
  }

  if ( !found )
  {
    act( "$r並沒有池塘或是噴水池。", ch, NULL, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( obj->item_type != ITEM_DRINK_CON )
  {
    act( "你不能把$p給裝滿。", ch, obj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( obj->value[1] != 0 && obj->value[2] != liq_water->slot )
  {
    act( "$p已經有其他液體了﹐你想做化學實驗嗎﹖", ch, obj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( obj->value[1] >= obj->value[0] )
  {
    act( "$p已經滿了。", ch, obj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  act( "你把$p裝滿了。", ch, obj, NULL, TO_CHAR );
  obj->value[2] = liq_water->slot;
  obj->value[1] = obj->value[0];

  message_driver( ch, obj, ACT_WHEN_FILL );
  RETURN_NULL();
}

FUNCTION( do_drink )
{
  char          arg[MAX_INPUT_LENGTH];
  int           amount;
  OBJ_DATA    * obj;
  LIQ_DATA    * pLiq;
  AFFECT_DATA   af;

  PUSH_FUNCTION( "do_drink" );

  one_argument( argument, arg );

  if ( !arg[0] )
  {
    for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
      if ( obj->item_type == ITEM_FOUNTAIN ) break;

    if ( !obj )
    {
      send_to_char( "你要喝什麼﹖\n\r", ch );
      RETURN_NULL();
    }
  }
  else
  {
    if ( !( obj = get_obj_here( ch, arg ) ) )
    {
      act( "你找不到那個東西 $2$T$0。", ch, NULL, arg, TO_CHAR );
      RETURN_NULL();
    }
  }

  /* 物品是否有餵毒 */
  if ( poison_char( ch, obj ) ) RETURN_NULL();

  if ( !IS_NPC( ch ) && ch->pcdata->condition[ COND_DRUNK ] > 10 )
  {
    send_to_char( "你已經醉的沒辦法對準口。\n\r", ch );
    RETURN_NULL();
  }

  switch ( obj->item_type )
  {
  default:
    act( "你沒有辦法從$p喝東西。", ch, obj, NULL, TO_CHAR );
    break;

  case ITEM_FOUNTAIN:

    if ( !IS_NPC(ch) ) ch->pcdata->condition[COND_THIRST] = 100;
    act( "$n從水池裡喝水﹐好喝的水。", ch, NULL, NULL, TO_ROOM );
    send_to_char( "你已經不渴了。\n\r", ch );

    message_driver( ch, obj, ACT_WHEN_DRINK );
    break;

  case ITEM_POTION:

    if ( obj->carried_by != ch )
    {
      act( "$p不是你的﹐所以你不能喝。", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    act( "$n奮不顧身地喝下$p。", ch, obj, NULL, TO_ROOM );
    act( "你喝下了$p。"        , ch, obj, NULL ,TO_CHAR );

    message_driver( ch, obj, ACT_WHEN_DRINK );
    obj_cast( obj, ch, ch, NULL );
    extract_obj( obj );

    break;

  case ITEM_DRINK_CON:

    if ( obj->value[1] <= 0 )
    {
      act( "已經沒有東西在$p裡面。", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( !( pLiq = liq_lookup( obj->value[2] ) ) )
    {
      act( "已經沒有東西在$p裡面。", ch, obj, NULL, TO_CHAR );
      mudlog( LOG_DEBUG, "do_drink: 沒有編號 %d 的液體.", obj->value[2] );
      RETURN_NULL();
    }

    act( "$n從$p喝$T。", ch, obj, pLiq->name, TO_ALL );

    message_driver( ch, obj, ACT_WHEN_DRINK );
    amount = UMIN( number_range( 3 , 10 ) , obj->value[1] );

    gain_condition( ch, COND_DRUNK , amount * pLiq->drunk  );
    gain_condition( ch, COND_FULL  , amount * pLiq->full   );
    gain_condition( ch, COND_THIRST, amount * pLiq->thirst );

    if ( !IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK]  > 10 )
      send_to_char( "你已經醉了。\n\r", ch );

    if ( !IS_NPC(ch) && ch->pcdata->condition[COND_FULL]   > 40 )
      send_to_char( "你灌飽了。\n\r", ch );

    if ( !IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] > 40 )
      send_to_char( "你已經不渴了。\n\r", ch );

    if ( obj->value[3] != 0 )
    {
      act( "$n看起來好像是中毒了。", ch, NULL, NULL, TO_ROOM );
      send_to_char( "糟了﹐你覺得渾身不太對勁。\n\r", ch );
      af.type      = SLOT_POISON;
      af.duration  = 3 * amount;
      af.location  = APPLY_NONE;
      af.modifier  = 0;
      af.level     = obj->level;
      affect_join( ch, &af );
    }

    if ( ( obj->value[1] -= amount ) <= 0 )
    {
      act( "不好了﹐$p突然消失了。", ch, obj, NULL, TO_CHAR );
      extract_obj( obj );
    }

    break;
  }
  RETURN_NULL();
}

FUNCTION( do_feed )
{
  char          arg1[MAX_INPUT_LENGTH];
  char          arg2[MAX_INPUT_LENGTH];
  OBJ_DATA    * obj;
  OBJ_DATA    * obj_cr;
  CHAR_DATA   * victim;
  AFFECT_DATA   af;
  bool          DigestObj = TRUE;
  int           condition;

  PUSH_FUNCTION( "do_feed" );

  if ( IS_NPC( ch ) || !ch->pcdata ) RETURN_NULL();

  argument = one_argument( argument, arg1 );
  one_argument( argument, arg2 );

  if ( arg1[0] == '\x0' || arg2[0] == '\x0' )
  {
    send_to_char( "你要餵什麼東西給什麼人﹖\n\r", ch );
    RETURN_NULL();
  }

  if ( !( obj = get_obj_carry( ch, arg1 ) ) )
  {
    act( "你找不到那個東西 $2$T$0。", ch, NULL, arg1, TO_CHAR );
    RETURN_NULL();
  }

  if ( !( victim = get_char_room( ch, arg2 ) ) )
  {
    send_to_char( "你要餵給什麼人呢﹖\n\r", ch );
    RETURN_NULL();
  }

  if ( ( IS_NPC( victim ) || !victim->pcdata || !is_couple( ch, victim ) )
    && !IS_IMMORTAL( ch ) )
  {
    send_to_char( "你只能餵東西給你的配偶﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( ch == victim )
  {
    send_to_char( "你無法餵給自己吃東西﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( victim->position != POS_STANDING && victim->position != POS_RESTING )
  {
    act( "你現在無法餵$N東西吃﹗", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  /* 物品是否有餵毒 */
  if ( poison_char( ch, obj ) ) RETURN_NULL();

  switch( obj->item_type )
  {
  default:
    send_to_char( "你只能餵藥或是食物給你的配偶﹗\n\r", ch );
    RETURN_NULL();

  case ITEM_PILL:
    break;

  case ITEM_FOOD:

    if ( !IS_NPC( victim ) && victim->pcdata->condition[COND_FULL] > 40 )
    {
      act( "$N已經吃飽了﹐再也吃不下東西了﹗", ch, NULL, victim, TO_CHAR );
      act( "$n嘗試餵你吃$p﹗"                , ch, obj , victim, TO_VICT );
      RETURN_NULL();
    }

    break;
  }

  act( "$n拿了$p餵給$N﹐好不幸福﹗", ch, obj, victim, TO_NOTVICT );
  act( "你餵$p給$N吃。"            , ch, obj, victim, TO_CHAR    );
  act( "$N餵你吃$p。"              , ch, obj, victim, TO_VICT    );

  if ( victim->mask )
    act( "不太對勁﹐你的易容術似乎被$N給識破了﹗", victim, NULL, ch, TO_CHAR );

  message_driver( victim, obj, ACT_WHEN_EAT );

  switch( obj->item_type )
  {
  default:
    break;

  case ITEM_PILL:

    obj_cast( obj, victim, victim, NULL );
    break;

  case ITEM_FOOD:

    if ( obj->value[4] == 0 )
    {
      if ( obj->value[1] < obj->value[2] || obj->value[1] <= 0 )
      {
        act( "$n大口地吃光了$p﹐還一副不滿足的模樣。"
          , victim, obj, NULL, TO_ALL );
      }

      else
      {
        DigestObj = FALSE;
      }
    }

    else
    {
      if ( !( obj_cr = create_object(get_obj_index(obj->value[4]), -1 ) ) )
      {
        mudlog( LOG_DEBUG, "Do_eat: 無法產生物品 %d.", obj->value[4] );
        RETURN_NULL();
      }

      if ( obj->value[1] <= obj->value[2] || obj->value[1] <= 0 )
      {
        act( "$n大口地吃光了$p﹐還一副不滿足的模樣。"
          , victim, obj, NULL, TO_ALL );

        act( "$n滿嘴的油膩往身上一抹﹐突然間手上多了$p。"
          , victim, obj_cr, NULL, TO_ALL );

        if ( obj_cr->Takeable     ) obj_to_char( obj_cr, victim );
        else if ( victim->in_room ) obj_to_room( obj_cr, victim->in_room );

        DigestObj = TRUE;
      }
      else
      {
        DigestObj = FALSE;
        extract_obj( obj_cr );
      }
    }

    if ( !IS_NPC( victim ) )
    {
      condition = victim->pcdata->condition[COND_FULL];
      gain_condition( victim, COND_FULL, obj->value[2] );
      obj->value[1] -= obj->value[2];

      if ( condition == 0 && victim->pcdata->condition[COND_FULL] > 0 )
        send_to_char( "你已經不餓了。\n\r", victim );

      else if ( victim->pcdata->condition[COND_FULL] > 40 )
        send_to_char( "你吃飽了。\n\r", victim );
    }

    if ( obj->value[3] != 0 )
    {
      act( "$n看起來好像中毒了﹗", victim, NULL, NULL, TO_ROOM );
      send_to_char( "遭了﹐你覺得渾身不太對勁。\n\r", victim );

      af.type      = SLOT_POISON;
      af.duration  = 2 * obj->value[1];
      af.location  = APPLY_HIT;
      af.modifier  = 0 - obj->value[2];
      af.level     = obj->level;
      affect_join( victim, &af );
    }
    break;
  }

  if ( DigestObj ) extract_obj( obj );

  RETURN_NULL();
}

FUNCTION( do_eat )
{
  char          arg[MAX_INPUT_LENGTH];
  OBJ_DATA    * obj;
  OBJ_DATA    * obj_cr;
  AFFECT_DATA   af;
  int           condition;
  bool          DigestObj = TRUE;

  PUSH_FUNCTION( "do_eat" );
  one_argument( argument, arg );

  if ( !( obj = get_obj_carry( ch, arg ) ) )
  {
    act( "你找不到那個東西 $2$T$0。", ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  /* 物品是否有餵毒 */
  if ( poison_char( ch, obj ) ) RETURN_NULL();

  if ( !IS_IMMORTAL( ch ) )
  {
    switch( obj->item_type )
    {
    default:
      act( "$p不是拿來吃的﹐你是鐵胃嗎﹖", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();

    case ITEM_PILL:

      if ( ch->pcdata && ch->pcdata->pills >= PillsPerDay )
      {
        act( "$p吃的那麼多﹐不怕中毒嗎﹖", ch, obj, NULL, TO_CHAR );
        RETURN_NULL();
      }

      break;

    case ITEM_FOOD:

      if ( !IS_NPC( ch ) && ch->pcdata->condition[COND_FULL] > 40 )
      {
        send_to_char( "你已經吃飽了﹐吃不下了。\n\r", ch );
        RETURN_NULL();
      }
      break;
    }
  }

  act( "$n拿起$p張口大嚼﹐渾然不覺他人的存在。", ch, obj, NULL, TO_ROOM );
  act( "你吃了一口$p。"    , ch, obj, NULL, TO_CHAR );

  message_driver( ch, obj, ACT_WHEN_EAT );

  switch ( obj->item_type )
  {
  default:
    act( "你把$p給消滅掉了﹗", ch, obj, NULL, TO_CHAR );
    break;

  case ITEM_FOOD:

    if ( obj->value[4] == 0 )
    {
      if ( obj->value[1] < obj->value[2] || obj->value[1] <= 0 )
      {
        act( "$n大口地吃光了$p﹐還一副不滿足的模樣。"
          , ch, obj, NULL, TO_ALL );

      } else DigestObj = FALSE;
    }
    else
    {
      if ( !( obj_cr = create_object(get_obj_index(obj->value[4]), -1 ) ) )
      {
        mudlog( LOG_DEBUG, "Do_eat: 無法產生物品 %d.", obj->value[4] );
        RETURN_NULL();
      }

      if ( obj->value[1] <= obj->value[2] || obj->value[1] <= 0 )
      {
        act( "$n大口地吃光了$p﹐還一副不滿足的模樣。"
          , ch, obj, NULL, TO_ALL );

        act( "$n滿嘴的油膩往身上一抹﹐突然間手上多了 $p。"
          , ch, obj_cr, NULL, TO_ALL );

        if ( obj_cr->Takeable ) obj_to_char( obj_cr, ch );
        else if ( ch->in_room ) obj_to_room( obj_cr, ch->in_room );

        DigestObj = TRUE;
      }
      else
      {
        DigestObj = FALSE;
        extract_obj( obj_cr );
      }
    }

    if ( !IS_NPC( ch ) )
    {
      condition = ch->pcdata->condition[COND_FULL];
      gain_condition( ch, COND_FULL, obj->value[2] );
      obj->value[1] -= obj->value[2];

      if ( condition == 0 && ch->pcdata->condition[COND_FULL] > 0 )
        send_to_char( "你已經不餓了。\n\r", ch );

      else if ( ch->pcdata->condition[COND_FULL] > 40 )
        send_to_char( "你吃飽了。\n\r", ch );
    }

    if ( obj->value[3] != 0 )
    {
      act( "$n看起來好像中毒了﹗", ch, NULL, NULL, TO_ROOM );
      send_to_char( "遭了﹐你覺得渾身不太對勁。\n\r", ch );

      af.type      = SLOT_POISON;
      af.duration  = 2 * obj->value[1];
      af.location  = APPLY_HIT;
      af.modifier  = 0 - obj->value[2];
      af.level     = obj->level;
      affect_join( ch, &af );
    }
    break;

  case ITEM_PILL:

    obj_cast( obj, ch, ch, NULL );
    if ( !IS_NPC( ch ) && ch->pcdata ) ch->pcdata->pills++;
    break;
  }

  if ( DigestObj ) extract_obj( obj );

  RETURN_NULL();
}

bool remove_obj( CHAR_DATA * ch, int iWear, bool fReplace, bool fShow )
{
  OBJ_DATA * obj;

  PUSH_FUNCTION( "remove_obj" );

  if ( !( obj = get_eq_char( ch, iWear ) ) ) RETURN( TRUE );
  if ( !fReplace )                           RETURN( FALSE );

  if ( obj->NoRemove )
  {
    if ( fShow ) act( "你不能移除$p這項裝備。", ch, obj, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  unequip_char( ch, obj, TRUE );

  if ( fShow ) act( "$n暫停裝備$p。", ch, obj, NULL, TO_ALL );
  RETURN( TRUE );
}

bool wear_obj( CHAR_DATA * ch, OBJ_DATA * obj, bool fReplace, bool fShow )
{
  SITUS_DATA * pSitus;
  SITUS_DATA * zSitus;

  PUSH_FUNCTION( "wear_obj" );

  if ( !ch || !obj )
  {
    mudlog( LOG_DEBUG, "wear_obj: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( ch->level < obj->level )
  {
    if ( fShow )
    {
      act( "你必需等級$i以上才能使用這物品。", ch, &obj->level, NULL, TO_CHAR );
      act( "$n嘗試穿上$p﹐但是歷練不足﹐無法穿上。", ch, obj, NULL, TO_ROOM );
    }

    RETURN( FALSE );
  }

  if ( !check_obj_restrict( ch, obj, fShow ) ) RETURN( FALSE );

  for ( pSitus = situs_list; pSitus; pSitus = pSitus->next )
  {
    if ( !CAN_WEAR( obj, pSitus->wear ) ) continue;

    if ( ( zSitus = pSitus->associate ) )
    {
      if ( get_eq_char( ch, pSitus->location )
        && get_eq_char( ch, zSitus->location )
        && !remove_obj( ch, pSitus->location, fReplace, fShow )
        && !remove_obj( ch, zSitus->location, fReplace, fShow ) )
        RETURN( FALSE );

      if ( !get_eq_char( ch, pSitus->location ) )
      {
        if ( fShow )
        {
          act( smash_act_keyword( pSitus->msg2, "nesp" ), ch, obj, NULL, TO_CHAR );
          act( smash_act_keyword( pSitus->msg1, "nesp" ), ch, obj, NULL, TO_ROOM );
        }

        equip_char( ch, obj, pSitus->location );
        RETURN( TRUE );
      }

      if ( !get_eq_char( ch, zSitus->location ) )
      {
        if ( fShow )
        {
          act( smash_act_keyword( zSitus->msg2, "nesp" ), ch, obj, NULL, TO_CHAR );
          act( smash_act_keyword( zSitus->msg1, "nesp" ), ch, obj, NULL, TO_ROOM );
        }

        equip_char( ch, obj, zSitus->location );
        RETURN( TRUE );
      }

      mudlog( LOG_DEBUG, "wear_obj: 太多%s裝備.", pSitus->situs_name );
      RETURN( FALSE );
    }

    if ( remove_obj( ch, pSitus->location, fReplace, fShow ) )
    {
      if ( fShow )
      {
        act( smash_act_keyword( pSitus->msg1, "nesp" ) , ch, obj, NULL, TO_ROOM );
        act( smash_act_keyword( pSitus->msg2, "nesp" ) , ch, obj, NULL, TO_CHAR );
      }

      equip_char( ch, obj, pSitus->location );
      RETURN( TRUE );
    }

    RETURN( FALSE );
  }

  if ( fReplace && fShow )
    act( "對不起﹐你無法裝備或握住$p。", ch, obj, NULL, TO_CHAR );

  RETURN( FALSE );
}

FUNCTION( do_wear )
{
  char       arg[MAX_INPUT_LENGTH];
  OBJ_DATA * obj;
  OBJ_DATA * obj_next;
  bool       bWear;

  PUSH_FUNCTION( "do_wear" );

  one_argument( argument, arg );

  if ( !str_cmp( arg, "all" ) )
  {
    for ( bWear = FALSE, obj = ch->carrying; obj; obj = obj_next )
    {
      obj_next = obj->next_content;

      if ( !poison_char( ch, obj )
        && obj->wear_loc == WEAR_NONE
        && obj->item_type != ITEM_MAGICSTONE
        && can_see_obj( ch, obj )
        && wear_obj( ch, obj, FALSE, TRUE ) ) bWear = TRUE;
    }

    if ( !bWear ) send_to_char( "對不起﹐你沒有任何物品可以裝備了﹗\n\r", ch );
  }

  else
  {
    if ( !( obj = get_obj_carry( ch, arg ) ) )
    {
      act( "你沒有那件物品 $2$T$0 耶。", ch, NULL, arg, TO_CHAR );
      RETURN_NULL();
    }

    /* 物品是否有餵毒 */
    if ( poison_char( ch, obj ) ) RETURN_NULL();

    if ( obj->item_type == ITEM_MAGICSTONE )
    {
      act( "$p是不能裝備的﹗", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    wear_obj( ch, obj, TRUE, TRUE );
  }

  RETURN_NULL();
}

FUNCTION( do_remove )
{
  char       arg[MAX_INPUT_LENGTH];
  OBJ_DATA * obj;
  OBJ_DATA * obj_next;
  bool       bRemove;

  PUSH_FUNCTION( "do_remove" );

  one_argument( argument, arg );

  if ( !str_cmp( arg, "all" ) )
  {
    for ( bRemove = FALSE, obj = ch->carrying; obj; obj = obj_next )
    {
      obj_next = obj->next_content;
      if ( obj->wear_loc >= 0 && remove_obj( ch , obj->wear_loc, TRUE, TRUE ) )
        bRemove = TRUE;
    }

    if ( !bRemove ) send_to_char( "對不起﹐你沒有任何物品可以移除了﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( !( obj = get_obj_wear( ch, arg ) ) )
  {
    act( "你並沒有那件物品 $2$T$0。", ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  remove_obj( ch, obj->wear_loc, TRUE, TRUE );

  RETURN_NULL();
}

/* 奉獻出屍體之後就可以還陽 */
FUNCTION( do_crave )
{
  PUSH_FUNCTION( "do_crave" );

  if ( IS_NPC( ch ) ) RETURN_NULL();
  if ( !ch->pcdata || !ch->pcdata->corpse || !ch->pcdata->corpse->in_room )
  {
    send_to_char( "沒有發現你的屍體﹐沒有辦法還給你。\n\r" , ch );
    RETURN_NULL();
  }

  if ( ch->in_room == ch->pcdata->corpse->in_room )
  {
    send_to_char( "你已經取回你的屍體了。\n\r", ch );
    RETURN_NULL();
  }

  char_from_room( ch );
  char_to_room( ch , ch->pcdata->corpse->in_room );

  send_to_char( "\n\r\n\r\a\a\a好吧﹐把屍體還給你﹐失敗總會過去﹐"
    "請你繼續努力奮鬥吧﹗\n\r"
    "如果要回到原來的世界﹐將屍體奉獻( \e[1;33msac corpse \e[0m)即可。\n\r"
    "不要忘了取回(\e[1;32msearch\e[0m)屍體裡的裝備喔﹗\n\r\n\r"
      , ch );

  ch->position = POS_STANDING;
  do_look( ch , "auto" );
  clear_trace( ch, TRUE );

  RETURN_NULL();
}

FUNCTION( do_sacrifice )
{
  char              arg[MAX_INPUT_LENGTH];
  ROOM_INDEX_DATA * pRoomIndex;
  OBJ_DATA        * obj;
  int               amount;

  PUSH_FUNCTION( "do_sacrifice" );

  one_argument( argument, arg );

  if ( !arg[0] || !str_cmp( arg, ch->name ) )
  {
    act( "$n要把$e自己奉獻給天神﹐他是不是瘋了啊。", ch, NULL, NULL, TO_ROOM );
    send_to_char( "天神等一下就會考慮你的奉獻。\n\r", ch );
    RETURN_NULL();
  }

  if ( !( obj = get_obj_list( ch, arg, ch->in_room->contents ) ) )
  {
    act( "你找不到那件物品 $2$T$0。", ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  if ( obj->Takeable == FALSE || !str_cmp( obj->name, "turd" ) )
  {
    act( "天神不喜歡像$p這樣的奉獻。", ch, obj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( obj->item_type == ITEM_CORPSE_PC )
  {
    if ( IS_NPC( ch ) || !ch->pcdata || ch->pcdata->corpse != obj )
    {
      act( "$p不是你的屍體﹐你不能奉獻它。", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    message_driver( ch, obj, ACT_WHEN_SACRIFICE );

    act( "天上降下一道紫色光芒﹐天神幫$n還陽了﹗" , ch, NULL, NULL, TO_ALL );

    ch->pcdata->corpse = NULL;
    char_from_room( ch );

    pRoomIndex = get_hometown( ch );
    char_to_room( ch , pRoomIndex );

    extract_obj( obj );
    save_char_obj( ch , SAVE_FILE );

    do_look( ch , "auto" );
    clear_trace( ch, TRUE );

    RETURN_NULL();
  }

  switch( obj->item_type )
  {
  default:
    amount = UMAX( 1, SacGold );
    break;

  case ITEM_MONEY:
    do_get( ch, "coin" );
    RETURN_NULL();
  }

  if ( !over_scale( ch ) )
  {
    act( "由於你虔誠的奉獻﹐萬能的天神賜給你$i兩黃金。"
      , ch, &amount, NULL, TO_CHAR );

    gold_to_char( ch, amount );
  }

  else
  {
    send_to_char( "因為你的總資產已達上限﹐無法再領到任何錢﹗\n\r", ch );
  }

  act( "$n將$p虔誠地奉獻給偉大的神。", ch, obj, NULL, TO_ROOM );
  message_driver( ch, obj, ACT_WHEN_SACRIFICE );
  extract_obj( obj );
  RETURN_NULL();
}

FUNCTION( do_study )
{
  char         arg[MAX_INPUT_LENGTH];
  int          slot;
  int          adept;
  OBJ_DATA   * pObj;
  SKILL_DATA * pSkill;
  LIMIT_DATA * pLimit;

  PUSH_FUNCTION( "do_study" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    send_to_char( "你要閱\讀什麼秘笈﹖\n\r", ch );
    RETURN_NULL();
  }

  if ( !( pObj = get_obj_carry( ch, arg ) ) )
  {
    act( "你身上沒有這本 $2$T$0 秘笈﹗", ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  if ( pObj->item_type != ITEM_MYSTERY )
  {
    act( "$p看起來不像是本秘笈﹗", ch, pObj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( ( adept = pObj->value[1] ) <= 0
    || adept > 100
    || ( slot = pObj->value[0] ) < 0
    || slot >= MAX_SKILL
    || !( pSkill = get_skill( slot ) ) )
  {
    act( "你這本$p是不是過期了﹗", ch, pObj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( ch->skill[slot] > 0 )
  {
    act( "這本$p﹐$n看起來很眼熟嘛﹗", ch, pObj, NULL, TO_ALL );
    RETURN_NULL();
  }

  if ( !check_skill_restrict( ch, pSkill, FALSE )
     || !check_valid_rating( ch, pSkill, FALSE ) )
  {
    act( "這本$p你不適合學﹐學了也沒有用。", ch, pObj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( !ch->class || learn_count( ch ) > get_learnable( ch ) )
  {
    send_to_char( "你的腦袋就這麼大﹐記不住那麼多招式﹗\n\r", ch );
    RETURN_NULL();
  }

  ch->skill[slot] = adept;

  for ( pLimit = pSkill->limit; pLimit; pLimit = pLimit->next )
  {
    if ( pLimit->class == ch->class )
    {
      ch->skill[slot] = UMIN( ch->skill[slot], pLimit->adept );
      break;
    }
  }

  if ( ch->skill[slot] == 0 )
  {
    act( "你根本就不適合學$w﹐放棄好了﹗", ch, pSkill, NULL, TO_CHAR );
    RETURN_NULL();
  }

  message_driver( ch, pObj, ACT_WHEN_STUDY );

  act( "恭禧$n從$p領會到$2$W$0這一門功\夫﹐但是$p慢慢化作一陣青煙消失了﹗"
    , ch, pObj, pSkill, TO_ALL );

  extract_obj( pObj );
  RETURN_NULL();
}

FUNCTION( do_load )
{
  char       arg1[MAX_INPUT_LENGTH];
  char       arg2[MAX_INPUT_LENGTH];
  OBJ_DATA * pContainer;
  OBJ_DATA * pArrow;

  PUSH_FUNCTION( "do_load" );

  argument = one_argument( argument, arg2 );
             one_argument( argument, arg1 );

  if ( !arg1[0] || !arg2[0] )
  {
    send_to_char( "你要把那個東西裝填到那裡去呢﹖\n\r", ch );
    RETURN_NULL();
  }

  if ( !( pContainer = get_obj_carry( ch, arg1 ) ) )
  {
    act( "對不起﹐你沒有那個背袋 $2$T$0﹗", ch, NULL, arg1, TO_CHAR );
    RETURN_NULL();
  }

  if ( pContainer->item_type != ITEM_BACKBAG )
  {
    act( "$p不是一種背袋﹐無法裝東西﹗", ch, pContainer, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( !can_drop_obj( ch, pContainer ) )
  {
    act( "你不能丟棄$p。", ch, pContainer, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( !( pArrow = get_obj_carry( ch, arg2 ) ) )
  {
    act( "對不起﹐你沒有那個武器 $2$T$0 來裝填﹗", ch, NULL, arg1, TO_CHAR );
    RETURN_NULL();
  }

  if ( !pArrow->Loadable )
  {
    act( "$p是不可裝填的﹗", ch, pArrow, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( pContainer->value[2] > 0
    && pContainer->value[2] != pArrow->pIndexData->vnum )
  {
    act( "$p裡面已經裝了其他東西了﹗", ch, pContainer, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( pContainer->value[0] <= pContainer->value[1] )
  {
    act( "$p已經裝滿了$P﹗", ch, pContainer, pArrow, TO_CHAR );
    RETURN_NULL();
  }

  pContainer->value[1]++;
  pContainer->value[2] = pArrow->pIndexData->vnum;
  extract_obj( pArrow );

  act( "$n把$p裝到$P裡面去﹗", ch, pArrow, pContainer, TO_ALL );

  RETURN_NULL();
}

FUNCTION( do_fire )
{
  CHAR_DATA * pMount;
  char        arg[MAX_INPUT_LENGTH];
  OBJ_DATA  * pFire;
  int         distance;

  PUSH_FUNCTION( "do_fire" );

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    send_to_char( "你要放什麼煙火﹖\n\r", ch );
    RETURN_NULL();
  }

  if ( !ch->in_room )
  {
    send_to_char( "對不起﹐但你目前在那裡呢﹖\n\r", ch );
    RETURN_NULL();
  }

  if ( !IS_OUTSIDE( ch ) )
  {
    act( "你無法在室內$r施放煙火的!", ch, NULL, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( ch->in_room->area == FoggyArea )
  {
    act( "目前$r一片霧濛濛﹐這時放煙火是沒有用的.", ch , NULL, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( ( pMount = get_mount( ch ) ) )
  {
    act( "你坐在$N上是沒有辦法放煙火的.", ch, NULL, pMount, TO_CHAR );
    RETURN_NULL();
  }

  if ( !( pFire = get_obj_carry( ch, arg ) ) )
  {
    act( "找不到你的煙火 $2$T$0 耶﹗", ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  /* 物品是否有餵毒 */
  if ( poison_char( ch, pFire ) ) RETURN_NULL();

  if ( pFire->item_type != ITEM_FIREWORK )
  {
    act( "$p並不是一種煙火﹐所以不能施放﹗", ch, pFire, NULL, TO_CHAR );
    RETURN_NULL();
  }

  distance = pFire->value[0];
  if ( weather_info.sky == SKY_RAINING ) distance--;

  if ( distance <= 0 || distance >= MAX_SCAN_DEPTH )
  {
    act( "$p看來是過期的煙火﹐這個沒用了﹗", ch, pFire, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( number_range( 1, 10000 ) <= pFire->value[1] )
  {
    act( "突然﹐$p在還沒升空前就已經爆炸﹐$n被炸的血肉模糊﹗趕快送醫吧﹗"
      , ch, pFire, NULL, TO_ALL );

    act( "你的$p爆炸消失不見了。" , ch, pFire, NULL, TO_CHAR );
    extract_obj( pFire );

    ch->hit  = UMAX( 1, ch->hit  / 2 );
    ch->mana = UMAX( 1, ch->mana / 2 );
    ch->move = UMAX( 1, ch->move / 2 );

    RETURN_NULL();
  }

  act( "$n施放$p﹐天空高處出現絢爛的色彩﹗", ch, pFire, NULL, TO_ALL );

  clean_scan();
  scan_room( ch->in_room->vnum, distance, -1 );
  print_scan( ch, pFire );
  clean_scan();

  act( "你的$p消失不見了。" , ch, pFire, NULL, TO_CHAR );
  extract_obj( pFire );

  RETURN_NULL();
}

FUNCTION( do_release )
{
  char             arg[MAX_INPUT_LENGTH];
  OBJ_DATA       * pUrn;
  MOB_INDEX_DATA * pMobIndex;
  CHAR_DATA      * victim;

  PUSH_FUNCTION( "do_release" );

  if ( IS_NPC( ch ) || !ch->in_room ) RETURN_NULL();

  if ( ch->mana < get_curr_mana( ch ) / 5 )
  {
    act( "$n已經沒有法力可以控制這一隻怪物了﹐所以$n放不出來﹗"
      , ch, NULL, NULL, TO_CHAR );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    send_to_char( "你想要釋放那個妖壺呢﹖\n\r", ch );
    RETURN_NULL();
  }

  if ( !( pUrn = get_obj_carry( ch, arg ) ) )
  {
    act( "你身上沒有這罐妖壺 $2$T$0 耶﹗", ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  /* 物品是否有餵毒 */
  if ( poison_char( ch, pUrn ) ) RETURN_NULL();

  if ( pUrn->item_type != ITEM_URN )
  {
    act( "$p並不是妖壺﹐所以不能釋放封印﹗", ch, pUrn, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( ch->position != POS_STANDING )
  {
    send_to_char( "你先放下手邊的工作吧﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( room_is_private( ch->in_room ) || ch->in_room->NoMob )
  {
    act( "$r陽氣太重了﹐所以無法釋放封印", ch, NULL, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( is_platform( ch->in_room ) || ch->in_room->Sail )
  {
    act( "對不起﹐你不能在$r這裡釋放封印﹗", ch, NULL, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( !( pMobIndex = get_mob_index( pUrn->value[0] ) ) )
  {
    act( "對不起﹐你的$p沒有被封印過﹗", ch, pUrn, NULL, TO_CHAR );
    RETURN_NULL();
  }

  victim               = create_mobile( pMobIndex, -1 );
  victim->disable_prog = TRUE;
  victim->release      = TRUE;
  victim->hit          = UMAX( 1, UMIN( victim->max_hit , pUrn->value[1] ) );
  victim->mana         = UMAX( 1, UMIN( victim->max_mana, pUrn->value[2] ) );
  victim->move         = UMAX( 1, UMIN( victim->max_move, pUrn->value[3] ) );

  if ( IS_SET( victim->act, ACT_AGGRESSIVE ) )
    REMOVE_BIT( victim->act, ACT_AGGRESSIVE );

  char_to_room( victim, ch->in_room );

  act( "$n手上的$p突然冒出一陣青煙﹐不久﹐逐漸現出$N的身影﹐"
       "隨後﹐$p也消失不見了﹗"
    , ch, pUrn, victim, TO_CHAR );

  act( "$n手上的$p突然冒出一陣青煙﹐不久﹐逐漸現出$N的身影﹗"
    , ch, pUrn, victim, TO_ROOM );

  ch->mana -= get_curr_mana( ch ) / 5;
  act( "$n的法力消耗去控制怪物了﹗", ch, NULL, NULL, TO_CHAR );

  extract_obj( pUrn );

  RETURN_NULL();
}

FUNCTION( do_recite )
{
  char       arg1[MAX_INPUT_LENGTH];
  char       arg2[MAX_INPUT_LENGTH];
  OBJ_DATA * scroll;
  OBJ_DATA * obj = NULL;

  PUSH_FUNCTION( "do_recite" );

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );

  if ( !( scroll = get_obj_carry( ch, arg1 ) ) )
  {
    act( "你沒有可以唸誦的捲軸 $2$T$0。", ch, NULL, arg1, TO_CHAR );
    RETURN_NULL();
  }

  /* 物品是否有餵毒 */
  if ( poison_char( ch, scroll ) ) RETURN_NULL();

  if ( scroll->item_type != ITEM_SCROLL )
  {
    send_to_char( "你只能用卷軸類物品來唸誦。\n\r", ch );
    RETURN_NULL();
  }

  if ( arg2[0] == '\x0' )
  {
    send_to_char( "你要如何施法﹖\n\r", ch );
    RETURN_NULL();
  }

  if ( !( obj = get_obj_here( ch, arg2 ) ) )
  {
    act( "你找不到那個物品 $2$T$0。", ch, NULL, arg2, TO_CHAR );
    RETURN_NULL();
  }

  if ( obj->item_type == ITEM_SCROLL )
  {
    act( "你不能用$p來唸誦捲軸﹗", ch, obj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  act( "$n唸誦$p。", ch, scroll, NULL, TO_ALL );

  message_driver( ch, scroll, ACT_WHEN_RECITE );
  obj_cast( scroll, ch, NULL, obj );

  act( "你的$p消失不見了。", ch, scroll, NULL, TO_CHAR );
  extract_obj( scroll );

  RETURN_NULL();
}

CHAR_DATA * find_keeper( CHAR_DATA * ch , int type )
{
  char        buf[MAX_STRING_LENGTH];
  CHAR_DATA * keeper;
  SHOP_DATA * pShop = NULL;

  PUSH_FUNCTION( "find_keeper" );

  if ( !ch || !ch->in_room )
  {
    mudlog( LOG_DEBUG, "find_keeper: 來源錯誤." );
    RETURN( NULL );
  }

  for ( keeper = ch->in_room->people; keeper; keeper = keeper->next_in_room )
  {
    if ( IS_NPC( keeper )
      && keeper->pIndexData
      && keeper->pIndexData->pShop
      && keeper->pIndexData->pShop->type == type )
    {
      pShop = keeper->pIndexData->pShop;
      break;
    }
  }

  if ( !pShop )
  {
    switch( type )
    {
    case SHOP_STORE:

      act( "你在$r找不到商店﹐問問人吧。", ch, NULL, NULL, TO_CHAR );
      break;

    case SHOP_SMITH:
      send_to_char( "找不到打鐵鋪耶﹐問問別人吧。\n\r", ch );
      break;

    case SHOP_MERCENARY:
      send_to_char( "這裡沒有傭兵所耶﹐問問別人吧﹗\n\r", ch );
      break;

    case SHOP_COPER:
      act( "你在$r是買不到馬的﹐請教別人吧﹗", ch, NULL, NULL, TO_CHAR );
      break;
    }

    RETURN( NULL );
  }

  if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_KILLER ) && !IS_IMMORTAL( ch ) )
  {
    do_say( keeper, "老實告訴你好了﹗這裡不歡迎兇手﹗" );
    sprintf( buf, "殺人犯 %s 來我這裡喔﹐趕快來捉他﹗", mob_name( keeper, ch ) );
    do_shout( keeper, buf );
    RETURN( NULL );
  }

  switch( keeper->position )
  {
  default:
  case POS_DEAD:
    act( "$N他是不是瘋了﹖", ch, NULL, keeper, TO_CHAR );
    RETURN( NULL );

  case POS_STANDING:
    break;

  case POS_FIGHTING:
    act( "你沒看到$N忙嗎﹖", ch, NULL, keeper, TO_CHAR );
    RETURN( NULL );

  case POS_RESTING:
    act( "$N正在休息咧﹐趕快叫他做生意吧﹗", ch, NULL, keeper, TO_CHAR );
    RETURN( NULL );

  case POS_SLEEPING:
    act( "$N竟然在打盹﹐叫他起床做生意吧﹗", ch, NULL, keeper, TO_CHAR );
    RETURN( NULL );
  }

  if ( !IS_NPC(ch) && IS_SET( ch->act, PLR_THIEF ) && !IS_IMMORTAL( ch ) )
  {
    do_say( keeper, "這裡不歡迎小偷﹗" );
    sprintf( buf, "竊賊 %s 來這裡喔﹐趕快來捉他﹗", mob_name( keeper, ch ) );
    do_shout( keeper, buf );
    RETURN( NULL );
  }

  if ( !can_see( keeper, ch ) && !IS_IMMORTAL( ch ) )
  {
    do_say( keeper, "對不起﹐我不跟我看不見的人做生意。" );
    RETURN( NULL );
  }

  if ( time_info.hour < pShop->open_hour )
  {
    do_say( keeper, "抱歉﹐請晚點再來。" );
    RETURN( NULL );
  }

  if ( time_info.hour > pShop->close_hour )
  {
    do_say( keeper, "抱歉﹐明日請早。" );
    RETURN( NULL );
  }

  RETURN( keeper );
}

int get_cost( CHAR_DATA * keeper, OBJ_DATA * obj, bool fBuy )
{
  SHOP_DATA * pShop;
  OBJ_DATA  * obj2;
  int         cost;
  int         itype;

  PUSH_FUNCTION( "get_cost" );

  if ( !keeper || !keeper->pIndexData )
  {
    mudlog( LOG_DEBUG, "get_cost: 來源錯誤." );
    RETURN( 0 );
  }

  if ( !obj || !( pShop = keeper->pIndexData->pShop ) ) RETURN( 0 );

  if ( fBuy )
  {
    cost = obj->cost * pShop->profit_buy / 100;
  }
  else
  {
    for ( cost = 0 , itype = 0; itype < MAX_TRADE; itype++ )
    {
      if ( obj->item_type == pShop->buy_type[itype] )
      {
        cost = obj->cost * pShop->profit_sell / 100;
        break;
      }
    }

    for ( obj2 = keeper->carrying; obj2; obj2 = obj2->next_content )
      if ( obj->pIndexData == obj2->pIndexData ) cost /= 2;
  }

  if ( obj->item_type == ITEM_STAFF || obj->item_type == ITEM_WAND )
    cost = cost * obj->value[2] / obj->value[1];

  /* 防具類有損害的話 */
  if ( is_armor( obj ) && obj->max_armor > 0 )
    cost = UMAX( 0, ( cost * obj->armor / obj->max_armor ) );

  RETURN( cost );
}

FUNCTION( do_buy )
{
  char        arg[MAX_INPUT_LENGTH];
  CHAR_DATA * keeper;
  OBJ_DATA  * obj;
  OBJ_DATA  * sale;
  int         cost;
  int         loop;
  int         count;

  PUSH_FUNCTION( "do_buy" );

  if ( ch->trade == FALSE )
  {
    send_to_char( "你的總資產太可怕了﹐所以無法執行這個指令﹗\n\r", ch );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg );

  if ( !( keeper = find_keeper( ch, SHOP_STORE ) ) ) RETURN_NULL();

  if ( ch->position == POS_FIGHTING )
  {
    send_to_char( "你正在戰鬥﹐無法買賣東西﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( arg[0] == '@' )
  {
    if ( arg[1] == '\x0' || !is_number( arg + 1 ) )
    {
      send_to_char( "你想買編號第幾的物品呢﹖請查詢 buy 的使用方法﹗\n\r", ch );
      RETURN_NULL();
    }

    count = atoi( arg + 1 );
    obj   = NULL;

    for ( loop = 1, sale = keeper->carrying; sale; sale = sale->next_content )
    {
      if ( sale->wear_loc == WEAR_NONE
        && can_see_obj( ch, sale )
        && ( cost = get_cost( keeper, sale, TRUE ) ) > 0 )
      {
        if ( loop++ == count )
        {
          obj = sale;
          break;
        }
      }
    }

    if ( !obj )
    {
      act( "$n告訴你﹕我沒賣這樣物品﹐請打 list 看看本店的特產。"
        , keeper, NULL, ch, TO_VICT );
      ch->reply = keeper;
      RETURN_NULL();
    }
  }

  else
  {
    obj  = get_obj_carry( keeper, arg );
  }

  cost = get_cost( keeper, obj, TRUE );

  if ( cost <= 0 || !can_see_obj( ch, obj ) )
  {
    act( "$n告訴你﹕我沒賣這樣物品﹐請打 list 看看本店的特產。"
      , keeper, NULL, ch, TO_VICT );
    ch->reply = keeper;
    RETURN_NULL();
  }

  if ( ch->gold < cost )
  {
    act( "$n告訴你﹕你買不起$p﹐等你有足夠錢再說吧。"
      , keeper, obj, ch, TO_VICT );
    ch->reply = keeper;
    RETURN_NULL();
  }

  if ( obj->level > ch->level )
  {
    act( "$n告訴你﹕你等級太低﹐請不要買$p。"
      , keeper, obj, ch, TO_VICT );
    ch->reply = keeper;
    RETURN_NULL();
  }

  /* 物品是否有餵毒 */
  if ( poison_char( ch, obj ) ) RETURN_NULL();

  if ( ch->carry_number + get_obj_number( obj ) > can_carry_n( ch ) )
  {
    send_to_char( "你已經帶太多東西了。\n\r", ch );
    RETURN_NULL();
  }

  if ( get_carry_weight( ch ) + get_obj_weight( obj ) > can_carry_w( ch ) )
  {
    send_to_char( "你的重量已經超出負荷。\n\r", ch );
    RETURN_NULL();
  }

  if ( !IS_NPC( ch ) && excess_filequota( ch ) )
  {
    send_to_char( "對不起﹐你的檔案太大﹐無法再買任何東西了﹗\n\r", ch );
    RETURN_NULL();
  }

  act( "$n毫不考慮地買下$p﹗", ch, obj, NULL, TO_ALL );

  message_driver( ch, obj, ACT_WHEN_BUY );

  gold_from_char( ch, cost );

  if ( obj->Inventory ) obj = create_object( obj->pIndexData, -1 );
  else                  obj_from_char( obj );

  obj_to_char( obj, ch );
  RETURN_NULL();
}

FUNCTION( do_list )
{
  char        arg[MAX_INPUT_LENGTH];
  CHAR_DATA * keeper;
  OBJ_DATA  * obj;
  int         cost;
  int         count;
  int         alevel = 0;
  int         blevel = MAX_LEVEL;
  bool        found;
  bool        level = FALSE;

  PUSH_FUNCTION( "do_list" );

  if ( !( keeper = find_keeper( ch, SHOP_STORE ) ) ) RETURN_NULL();

  if ( ch->position == POS_FIGHTING )
  {
    send_to_char( "你正在戰鬥﹐無法買賣東西﹗\n\r", ch );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg );

  if ( arg[0] && is_number( arg ) )
  {
    if ( ( alevel = atoi( arg ) ) <= 0 || alevel > MAX_LEVEL )
    {
      send_to_char( "輸入的等級不合理﹗\n\r", ch );
      RETURN_NULL();
    }

    one_argument( argument, arg );

    if ( arg[0] )
    {
      if ( !is_number( arg ) )
      {
        send_to_char( "必須輸入數字來查詢﹗\n\r", ch );
        RETURN_NULL();
      }

      if ( ( blevel = atoi( arg ) ) <= 0 || blevel > MAX_LEVEL )
      {
        send_to_char( "輸入的等級不合理﹗\n\r", ch );
        RETURN_NULL();
      }

      if ( alevel > blevel ) SWAP( alevel, blevel );
    }

    arg[0] = '\x0';
    level  = TRUE;
  }

  clear_buffer();
  count = 0;

  for ( found = FALSE, obj = keeper->carrying; obj; obj = obj->next_content )
  {
    if ( obj->wear_loc == WEAR_NONE
      && can_see_obj( ch, obj )
      && ( cost = get_cost( keeper, obj, TRUE ) ) > 0
      && ( !arg[0] || is_name( arg, obj->name ) )
      && ( alevel <= obj->level && blevel >= obj->level ) )
    {
      if ( !found )
      {
        found = TRUE;
        send_to_buffer( "\e[1;33;44m[編號 等級   價格] 物品名稱      "
          "                                \e[0m\n\r" );
      }

      send_to_buffer( "[\e[1;32m%4d\e[0m \e[1;33m%4d\e[0m "
        "\e[1;37m%6d\e[0m] %s\n\r"
        , ++count, obj->level, cost, obj_name( ch, obj ) );
    }
  }

  if ( !found )
  {
    if ( !arg[0] && level == FALSE )
      act( "你被耍了﹐在$r買不到任何的東西。", ch, NULL, NULL, TO_CHAR );
    else
      act( "你在$r買不到你要的東西﹐請到別的商店看看吧。"
        , ch, NULL, NULL, TO_CHAR );

    RETURN_NULL();
  }

  print_buffer( ch );
  RETURN_NULL();
}

FUNCTION( do_sell )
{
  char        buf[MAX_STRING_LENGTH];
  char        arg[MAX_INPUT_LENGTH];
  char        chinese[MAX_STRING_LENGTH];
  CHAR_DATA * keeper;
  OBJ_DATA  * obj;
  int         cost;

  PUSH_FUNCTION( "do_sell" );

  if ( ch->trade == FALSE )
  {
    send_to_char( "你的總資產太可怕了﹐所以無法執行這個指令﹗\n\r", ch );
    RETURN_NULL();
  }

  one_argument( argument, arg );

  if ( !( keeper = find_keeper( ch, SHOP_STORE ) ) ) RETURN_NULL();

  if ( ch->position == POS_FIGHTING )
  {
    send_to_char( "你正在戰鬥﹐無法買賣東西﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( !( obj = get_obj_carry( ch, arg ) ) )
  {
    act( "你並沒有那件東西 $2$T$0﹐耍我啊。", ch , NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  /* 物品是否有餵毒 */
  if ( poison_char( ch, obj ) ) RETURN_NULL();


  if ( !can_drop_obj( ch, obj ) )
  {
    act( "你不能丟棄$p﹐所以你不能賣它。", ch, obj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( obj->address )
  {
    act( "$p是要交給別人的信﹐怎麼可以賣了呢。", ch, obj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( ( cost = get_cost( keeper, obj, FALSE ) ) <= 0 )
  {
    act( "$n看起來對$p沒什麼興趣﹐你就死了這條心吧﹗"
      , keeper, obj, ch, TO_VICT );
    RETURN_NULL();
  }

  chinese_number( cost, chinese );
  act( "$n忍痛地賣了$p﹗", ch, obj, NULL, TO_ROOM );

  sprintf( buf, "唉﹐為了錢﹐你只好忍痛以%s兩黃金把$p給割愛了。", chinese );
  act( smash_act_keyword( buf, "nesp" ) , ch, obj, NULL, TO_CHAR );
  message_driver( ch, obj, ACT_WHEN_SELL );

  if ( !over_scale( ch ) ) gold_to_char( ch, cost );
  gold_from_char( keeper, UMIN( keeper->gold, cost ) );

  if ( obj->item_type == ITEM_TRASH )
  {
    act( "這個$p還是丟掉吧﹐垃圾一件", ch, obj, NULL, TO_CHAR );
    extract_obj( obj );
  }
  else
  {
    obj_from_char( obj );
    obj_to_char( obj, keeper );
  }

  RETURN_NULL();
}

FUNCTION( do_value )
{
  char        buf[MAX_STRING_LENGTH];
  char        arg[MAX_INPUT_LENGTH];
  char        chinese[MAX_STRING_LENGTH];
  CHAR_DATA * keeper;
  OBJ_DATA  * obj;
  int         cost;

  PUSH_FUNCTION( "do_value" );
  one_argument( argument, arg );

  if ( !( keeper = find_keeper( ch, SHOP_STORE ) ) ) RETURN_NULL();

  if ( ch->position == POS_FIGHTING )
  {
    send_to_char( "你正在戰鬥﹐無法買賣東西﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( !( obj = get_obj_carry( ch, arg ) ) )
  {
    act( "你並沒有那個東西 $2$T$0﹐你耍我啊。", ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  /* 物品是否有餵毒 */
  if ( poison_char( ch, obj ) ) RETURN_NULL();

  if ( obj->address )
  {
    act( "$p是要交給別人的信﹐怎麼可以拿來估價。", ch, obj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( !can_drop_obj( ch, obj ) )
  {
    act( "$p是你的傳家之寶﹐還拿來估價﹐你是不是窮瘋了。", ch, obj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( ( cost = get_cost( keeper, obj, FALSE ) ) <= 0 )
  {
    act( "$n看起來對$p沒什麼興趣﹐你就死了這條心吧。"
      , keeper, obj, ch, TO_VICT );
    RETURN_NULL();
  }

  chinese_number( cost, chinese );
  sprintf( buf, "$n告訴你﹕「我願意出價%s兩黃金來買你的$p。」", chinese );
  act( smash_act_keyword( buf, "nesNESp" ) , keeper, obj, ch, TO_VICT );
  ch->reply = keeper;

  message_driver( ch, obj, ACT_WHEN_VALUE );

  RETURN_NULL();
}

void obj_cast( OBJ_DATA * pMagic, CHAR_DATA * ch, CHAR_DATA * victim
               , OBJ_DATA * obj )
{
  SKILL_DATA * pSkill;
  int          old;
  int          sn;
  int          type;
  int          value;
  int          vnum;

  PUSH_FUNCTION( "obj_cast" );

  if ( !ch || !pMagic )
  {
    mudlog( LOG_DEBUG, "obj_cast: 缺乏人物來源." );
    RETURN_NULL();
  }

  value = pMagic->value[0];
  type  = pMagic->value[1];
  sn    = pMagic->value[2];
  vnum  = pMagic->pIndexData ? pMagic->pIndexData->vnum : -1;

  /* 如果號碼小於零則不用作任何事 */
  if ( type <= 0 ) RETURN_NULL();

  switch( type )
  {
  default:
    mudlog( LOG_DEBUG, "obj_cast: 錯誤的型態 %d.", type );
    RETURN_NULL();

  /* 全補 */
  case OBJ_CAST_ALL:

    old = ch->hit;

    ch->hit = UMIN( get_curr_hit( ch ), ch->hit + value );
    ch->hit = UMAX( ch->hit, 0 );

    send_to_char( ch->hit == old
      ? "你的生命力似乎一點也沒有改善。\n\r" : ( ch->hit > old
        ? "你的生命力增進了不少。\n\r"
        : "你的生命力耗損了不少。\n\r" ), ch );

    old = ch->mana;

    ch->mana = UMIN( get_curr_mana( ch ), ch->mana + value );
    ch->mana = UMAX( ch->mana, 0 );

    send_to_char( ch->mana == old
      ? "你的法力似乎一點也沒有改善。\n\r" : ( ch->mana > old
        ? "你的法力增進了不少。\n\r"
        : "你的法力耗損了不少。\n\r" ), ch );

    old = ch->move;

    ch->move = UMIN( get_curr_move( ch ), ch->move + value );
    ch->move = UMAX( ch->move, 0 );

    send_to_char( ch->move == old
      ? "你的體力似乎一點也沒有改善。\n\r" : ( ch->move > old
        ? "你的體力增進了不少。\n\r"
        : "你的體力耗損了不少。\n\r" ), ch );

    break;

  /* 生命力 */
  case OBJ_CAST_HIT:

    old = ch->hit;

    ch->hit = UMIN( get_curr_hit( ch ), ch->hit + value );
    ch->hit = UMAX( ch->hit, 0 );

    send_to_char( ch->hit == old
      ? "你的生命力似乎一點也沒有改善。\n\r" : ( ch->hit > old
        ? "你的生命力增進了不少。\n\r"
        : "你的生命力耗損了不少。\n\r" ), ch );

    break;

  /* 法力 */
  case OBJ_CAST_MANA:

    old = ch->mana;

    ch->mana = UMIN( get_curr_mana( ch ), ch->mana + value );
    ch->mana = UMAX( ch->mana, 0 );

    send_to_char( ch->mana == old
      ? "你的法力似乎一點也沒有改善。\n\r" : ( ch->mana > old
        ? "你的法力增進了不少。\n\r"
        : "你的法力耗損了不少。\n\r" ), ch );

    break;

  /* 移動力 */
  case OBJ_CAST_MOVE:

    old = ch->move;

    ch->move = UMIN( get_curr_move( ch ), ch->move + value );
    ch->move = UMAX( ch->move, 0 );

    send_to_char( ch->move == old
      ? "你的體力似乎一點也沒有改善。\n\r" : ( ch->move > old
        ? "你的體力增進了不少。\n\r"
        : "你的體力耗損了不少。\n\r" ), ch );

    break;

  /* 對像是物品 */
  case OBJ_CAST_IDENT:    /* 鑑定 */
  case OBJ_DETECT_POISON: /* 偵測毒物 */

    if ( !obj )
    {
      mudlog( LOG_DEBUG, "obj_cast: 缺乏來源." );
      RETURN_NULL();
    }

    if ( !( pSkill = get_skill( sn ) ) )
    {
      mudlog( LOG_DEBUG, "obj_cast: 物品編號 %d 找不到法術編號 %d.", vnum, sn );
      RETURN_NULL();
    }

    if ( !pSkill->function )
    {
      mudlog( LOG_DEBUG, "obj_cast: 物品編號 %d 函數錯誤.", vnum );
      RETURN_NULL();
    }

    /* 呼叫函數 */
    ( *pSkill->function ) ( pSkill->slot, value, ch, obj );
    break;

  /* 對像是人 */
  case OBJ_CURE_POISON:   /* 解毒     */
  case OBJ_DETECT_INVS:   /* 偵測隱形 */
  case OBJ_CAST_FLY:      /* 飛行術   */
  case OBJ_CURE_BLIND:    /* 治療眼盲 */
  case OBJ_GIANT_STR:     /* 巨人之力 */
  case OBJ_DETECT_HIDE:   /* 偵測藏匿 */
  case OBJ_SNEAK:         /* 潛行     */
  case OBJ_DETECT_EVIL:   /* 偵測邪惡 */
  case OBJ_CHANGE_SEX:    /* 變性術   */
  case OBJ_DETECT_MAGIC:  /* 偵測魔法 */
  case OBJ_FAERIE_FOG:    /* 現身術   */
  case OBJ_GATE:          /* 招鬼術   */
  case OBJ_FIXITY:        /* 定身數   */
  case OBJ_DETECT_MASK:   /* 偵測易容 */

    if ( !victim )
    {
      mudlog( LOG_DEBUG, "obj_cast: 物品編號 %d 來源錯誤.", vnum );
      RETURN_NULL();
    }

    if ( !( pSkill = get_skill( sn ) ) )
    {
      mudlog( LOG_DEBUG, "obj_cast: 物品編號 %d 找不到法術編號 %d.", vnum, sn );
      RETURN_NULL();
    }

    if ( !pSkill->function )
    {
      mudlog( LOG_DEBUG, "obj_cast: 物品編號 %d 函數錯誤.", vnum );
      RETURN_NULL();
    }

    /* 呼叫函數 */
    ( *pSkill->function ) ( pSkill->slot, value, ch, victim );
    break;
  }

  /* 劑量 */
  if ( pMagic->value[3] > 0
    && verify_char( ch )
    && !is_dead( ch )
    && ch->pcdata )
  {
    ch->pcdata->dosage += pMagic->value[3];
    if ( ch->pcdata->dosage >= DosageLimit )
    {
      send_to_char( "因為服用過多的藥物﹐你發現你手心不停地冒冷汗﹗\n\r", ch );
    }
  }

  RETURN_NULL();
}

void message_driver( CHAR_DATA * ch, OBJ_DATA * obj, int type )
{
  char         * pSelf;
  char         * pOthers;
  MESSAGE_DATA * pMess;

  PUSH_FUNCTION( "message_driver" );

  if ( !ch || !obj || !obj->pIndexData )
  {
    mudlog( LOG_DEBUG, "message_driver: 來源不正確." );
    RETURN_NULL();
  }

  for ( pMess = obj->pIndexData->message; pMess; pMess = pMess->next )
    if ( pMess->type == type ) break;

  if ( !pMess || !ch->in_room ) RETURN_NULL();

  if ( ( pSelf   = pMess->self   ) && *pSelf   == '.' ) pSelf++;
  if ( ( pOthers = pMess->others ) && *pOthers == '.' ) pOthers++;

  if ( pSelf   && *pSelf   ) act( smash_act_keyword( pSelf, "nesp" )  , ch, obj, NULL, TO_CHAR );
  if ( pOthers && *pOthers ) act( smash_act_keyword( pOthers, "nesp" ), ch, obj, NULL, TO_ROOM );

  RETURN_NULL();
}

FUNCTION( do_trade )
{
  int              count;
  int              loop;
  int              cost;
  char             arg[MAX_INPUT_LENGTH];
  char             buf[MAX_STRING_LENGTH];
  CHAR_DATA      * keeper;
  CHAR_DATA      * horse;
  CHAR_DATA      * rch;
  CHAR_DATA      * zch;
  SHOP_DATA      * pShop;
  MOB_INDEX_DATA * pIndex;

  PUSH_FUNCTION( "do_trade" );

  if ( IS_NPC( ch ) || !ch->in_room ) RETURN_NULL();

  if ( !( keeper = find_keeper( ch, SHOP_COPER ) ) ) RETURN_NULL();

  if ( ch->position == POS_FIGHTING )
  {
    send_to_char( "你正在戰鬥﹐無法買馬﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( !keeper->pIndexData || !( pShop = keeper->pIndexData->pShop ) )
  {
    act( "$N目前是不能賣馬的﹗", ch, NULL, keeper, TO_CHAR );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    send_to_char( "這個指令必須有參數﹐請輸入 trade /? 查詢﹗\n\r", ch );
  }

  else if ( !str_prefix( arg, "list" ) )
  {
    send_to_buffer( "你在這裡可以買到的馬匹有﹕\n\r"
      "\e[1;33;44m順序 等級 費  用 體  力 生命力 名  稱                   "
      "                        \e[0m\n\r" );

    for ( count = loop = 0; loop < MAX_TRADE; loop++ )
    {
      if ( pShop->buy_type[loop] <= 0
        || !( pIndex = get_mob_index( pShop->buy_type[loop] ) )
        || pIndex->tractable <= 0 ) continue;

      send_to_buffer( " %2d.  %3d %6d %6d %6d %s(%s)\n\r"
        , ++count
        , pIndex->level
        , hire_cost( pIndex, pShop, SHOP_BUY )
        , pIndex->move
        , pIndex->hit
        , pIndex->short_descr
        , pIndex->name );
    }

    print_buffer( ch );
  }

  else if ( !str_prefix( arg, "buy" ) )
  {
    if ( !ch->in_room )
    {
      send_to_char( "你在什麼鬼地方啊﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->mount )
    {
      act( "你都已經在騎$N了﹐為什麼還來買馬呢﹖", ch, NULL, ch->mount, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch->position != POS_STANDING )
    {
      send_to_char( "你先忙完你的事吧﹐不然很難做買賣的﹗\n\r", ch );
      RETURN_NULL();
    }

    /* 不能有飛行術 */
    if ( is_affected( ch, SLOT_FLY ) )
    {
      send_to_char( "你雙腳離地﹐無法購買馬匹。\n\r", ch );
      RETURN_NULL();
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\x0' )
    {
      send_to_char( "你要買哪一匹馬呢﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( !is_number( arg ) )
    {
      send_to_char( "請以數字代表你要買的馬匹﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( ( count = atoi( arg ) ) <= 0 || count > MAX_TRADE )
    {
      send_to_char( "你的順序不對喔﹐請重新輸入﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !( pIndex = get_mob_index( pShop->buy_type[count - 1] ) )
      || pIndex->tractable <= 0 )
    {
      act( "在$r是買不到那匹馬的喔﹗", ch, NULL, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch->gold < ( cost = hire_cost( pIndex, pShop, SHOP_BUY ) ) )
    {
      send_to_char( "你身上的錢不夠買這匹馬﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !( horse = create_mobile( pIndex, -1 ) ) )
    {
      send_to_char( "對不起﹐老闆找不到那匹馬喔﹗\n\r", ch );
      RETURN_NULL();
    }

    /* 先消掉此地的馬匹 */
    for ( rch = ch->in_room->people; rch; rch = zch )
    {
      zch = rch->next_in_room;
      if ( rch->tractable > 0 && !rch->mount_by && !rch->fighting )
      {
        act( "$n說道﹕「$N好像是一匹野馬﹐我就把它牽回來馴服吧﹗」"
          , keeper, NULL, rch, TO_ROOM );

        extract_char( rch, TRUE );
      }
    }

    chinese_number( cost, buf );
    char_to_room( horse, ch->in_room );

    ch->mount       = horse;
    horse->mount_by = ch;

    act( "$n花了$t兩銀子向老闆買了一匹$N﹐之後就騎了上去了。"
      , ch, buf, horse, TO_ALL );

    gold_from_char( ch, cost );
  }

  else if ( !str_prefix( arg, "sell" ) )
  {
    if ( !( horse = ch->mount ) )
    {
      send_to_char( "你沒有騎著馬﹐怎麼賣你的馬呢﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->position != POS_STANDING || horse->position != POS_STANDING )
    {
      send_to_char( "你先忙的你吧﹐等你有空在賣吧﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !( pIndex = horse->pIndexData ) || horse->tractable <= 0 )
    {
      act( "$N看起來不太像是一匹坐騎﹗", ch, NULL, horse, TO_CHAR );
      RETURN_NULL();
    }

    for ( loop = 0; loop < MAX_TRADE; loop++ )
    {
      if ( pShop->buy_type[loop] == pIndex->vnum )
      {
        ch->mount       = NULL;
        horse->mount_by = NULL;

        cost = hire_cost( pIndex, pShop, SHOP_SELL )
               * UMIN( horse->max_move, horse->move )
               / UMAX( 1, get_curr_move( horse ) );

        if ( !over_scale( ch ) ) gold_to_char( ch, cost );
        chinese_number( cost, buf );

        act( "$n牽了一匹$N﹐拿來給賣給老闆﹗", ch, NULL, horse, TO_ALL );

        act( "你的$N一共賣了$t兩銀子。", ch, buf, horse, TO_CHAR );
        act( "之後﹐$n就把$N牽到後面去綁著了。", keeper, NULL, horse, TO_ROOM );

        extract_char( horse, TRUE );
        RETURN_NULL();
      }
    }

    act( "$N是什麼鬼﹐我是不可能買你的這個鬼東西﹗", ch, NULL, horse, TO_CHAR );
  }

  else
  {
    send_to_char( "參數錯誤﹐請輸入 trade /? 查詢﹗\n\r", ch );
  }
  RETURN_NULL();
}

FUNCTION( do_rename )
{
  char       arg[MAX_INPUT_LENGTH];
  char       buf[MAX_STRING_LENGTH];
  char       buf1[MAX_STRING_LENGTH];
  OBJ_DATA * sigil;
  OBJ_DATA * obj;

  PUSH_FUNCTION( "do_rename" );

  argument = one_argument( argument, arg );

  switch( arg[0] )
  {
  default:
    send_to_char( "你的參數錯誤﹐請查詢 rename 的用法。\n\r", ch );
    RETURN_NULL();

  case '1':

    for ( sigil = ch->carrying; sigil; sigil = sigil->next_content )
      if ( sigil->item_type == ITEM_SIGIL ) break;

    if ( !sigil )
    {
      send_to_char( "你身上沒有可以改名字的符咒﹐買張來用用吧﹗\n\r", ch );
      RETURN_NULL();
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\x0' )
    {
      send_to_char( "你要改那個東西的名稱呢﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( !( obj = get_obj_carry( ch, arg ) ) )
    {
      act( "你身上沒有這個物品 $2$T$0 喔﹐先看看清楚﹗", ch, NULL, arg, TO_CHAR );
      RETURN_NULL();
    }

    if ( obj == sigil )
    {
      act( "你不能拿$p來改符咒的名字啦﹗", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    argument = one_argument( argument, arg );

    if ( str_len( arg ) <= 3 )
    {
      act( "你要把$p改成什麼名稱呢﹖", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    arg[MAX_NAME_LENGTH] = '\x0';

    if ( check_cname_xname( arg ) )
    {
      send_to_char( "這個名字不好吧﹗換個名字看看。\n\r", ch );
      RETURN_NULL();
    }

    ansi_transcribe( arg, buf1 );
    filter_ansi( buf1 );
    smash_tilde( buf1 );
    smash_point( buf1 );
    fix_color( buf1 );

    sprintf( buf, "%s\e[0m", buf1 );

    if ( obj->item_type == ITEM_SPIRITJADE && obj->value[5] < 99 )
      obj->value[5]++;

    act( "$n重新幫$p命名為$T﹗", ch, obj, buf, TO_ALL );

    free_string( obj->cname );
    obj->cname = str_dup( buf );

    act( "你身上的$p也隨著消失了﹗", ch, sigil, NULL, TO_CHAR );

    obj_from_char( sigil );
    extract_obj( sigil );
    break;
  }

  RETURN_NULL();
}

FUNCTION( do_paint )
{
  char       arg[MAX_INPUT_LENGTH];
  char       buf[MAX_STRING_LENGTH];
  char       buf1[MAX_STRING_LENGTH];
  OBJ_DATA * vellum;
  int        size;

  PUSH_FUNCTION( "do_paint" );

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    send_to_char( "你要在那裡書寫你的文字呢﹖\n\r", ch );
    RETURN_NULL();
  }

  if ( !( vellum = get_obj_carry( ch, arg ) ) )
  {
    act( "對不起﹐你好像沒有那張紙 $2$T$0 可以書寫耶﹗", ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  if ( poison_char( ch, vellum ) ) RETURN_NULL();

  if ( vellum->item_type != ITEM_VELLUM )
  {
    act( "看來$p是無法寫上任何東西的﹗", ch, vellum, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( argument[0] == '\x0' )
  {
    act( "你要在$p上面寫哪些東西呢﹖", ch, vellum, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( check_chat_xname( ch, argument ) == TRUE ) RETURN_NULL();
  if ( vellum->description == NULL ) vellum->description = str_dup( "" );

  ansi_transcribe( argument, buf1 );
  filter_ansi( buf1 );
  smash_tilde( buf1 );
  smash_point( buf1 );
  fix_color( buf1 );

  size = str_len( vellum->description ) + str_len( buf1 );

  if ( size > MAX_VELLUM_LENGTH || size > sizeof( buf ) - 3 )
  {
    act( "看來$p是無法再寫什麼東西下去了﹗", ch, vellum, NULL, TO_CHAR );
    RETURN_NULL();
  }

  sprintf( buf, "%s%s\n\r", vellum->description, buf1 );
  free_string( vellum->description );
  vellum->description = str_dup( buf );

  act( "$n在$p寫了些東西﹐搞不好是什麼機密喔﹗", ch, vellum, NULL, TO_ALL );

  RETURN_NULL();
}

FUNCTION( do_pick )
{
  MINE_DATA       * pMine;
  MINE_DATA       * zMine;
  MINERAL_DATA    * pMineral;
  ROOM_INDEX_DATA * pRoom;
  OBJ_INDEX_DATA  * pIndex;
  OBJ_DATA        * pObj;

  PUSH_FUNCTION( "do_pick" )

  if ( IS_NPC( ch ) || !( pRoom = ch->in_room ) ) RETURN_NULL();

  if ( ch->position != POS_STANDING )
  {
    send_to_char( "你目前的狀態不適合採集物品。\n\r", ch );
    RETURN_NULL();
  }

  if ( !( pMine = pRoom->mine )
    || !( pMineral = pMine->mineral )
    || !( pIndex = pMineral->mineral ) )
  {
    act( "$n辛勤地在這理開墾﹐但什麼也沒找到﹗", ch, NULL, NULL, TO_ALL );
    RETURN_NULL();
  }

  pObj = create_object( pIndex, -1 );
  obj_to_room( pObj, pRoom );

  act( "$n辛勤地在這理開墾﹐終於採集到了$p﹗", ch, pObj, NULL, TO_ALL );

  if ( pRoom->mine == pMine )
  {
    pRoom->mine = pMine->next;
  }

  else
  {
    for ( zMine = pRoom->mine; zMine; zMine = zMine->next )
    {
      if ( zMine->next == pMine )
      {
        zMine->next = pMine->next;
        break;
      }
    }

    if ( !zMine ) mudlog( LOG_DEBUG , "do_pick: 表中未有此礦物." );
  }

  free_struct( pMine, STRUCT_MINE_DATA );
  RETURN_NULL();
}

bool can_store( OBJ_DATA * obj )
{
  PUSH_FUNCTION( "can_store" );

  if ( !obj )
  {
    mudlog( LOG_DEBUG, "can_store: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( obj->timer       != 0
    || obj->cachet
    || obj->item_type   == ITEM_CONTAINER
    || obj->item_type   == ITEM_SIGIL
    || obj->item_type   == ITEM_VELLUM
    || obj->item_type   == ITEM_URN
    || ( obj->item_type == ITEM_LIGHT && obj->value[2] > 0 ) )
    RETURN( FALSE );

  RETURN( TRUE );
}

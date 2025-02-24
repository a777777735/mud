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

void    set_cachet_default      args( ( CACHET_DATA * ) );

/* 設定初值 */
void set_cachet_default( CACHET_DATA * pCachet )
{
  PUSH_FUNCTION( "set_cachet_default" );
  pCachet->vnum = ERRORCODE;
  RETURN_NULL();
}

FUNCTION( do_cachet )
{
  int              loop;
  char             arg1[MAX_INPUT_LENGTH];
  char             arg2[MAX_INPUT_LENGTH];
  char             buf[MAX_STRING_LENGTH];
  OBJ_DATA       * obj;
  OBJ_DATA       * stone_obj;
  OBJ_INDEX_DATA * pIndex;
  CACHET_DATA    * pCachet;
  CACHET_DATA    * pCachet_next;

  PUSH_FUNCTION( "do_cachet" );

  argument = one_argument( argument, arg1 );
  one_argument( argument, arg2 );

  if ( !arg1[0] || !arg2[0] )
  {
    send_to_char( "參數錯誤﹐請查詢 cachet /?\n\r", ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg1, "!list" ) )
  {
    if ( !( obj = get_obj_carry( ch, arg2 ) ) )
    {
      send_to_char( "你並沒有這個裝備﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !obj->cachet )
    {
      act( "$p並沒有封印。", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    clear_buffer();
    send_to_buffer( "%s封印的魔石有﹕\n\r", obj_name( ch, obj ) );

    for ( loop = 0, pCachet = obj->cachet; pCachet; pCachet = pCachet->next )
    {
      if ( !( pIndex = get_obj_index( pCachet->vnum ) ) ) continue;
      send_to_buffer( "%3d. %s(%s)\n\r"
        , ++loop, pIndex->short_descr, pIndex->name );
    }

    if ( loop == 0 ) send_to_buffer( "%s沒有封印任何的魔石。"
      , obj_name( ch, obj ) );

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg1, "!back" ) )
  {
    if ( !( obj = get_obj_carry( ch, arg2 ) ) )
    {
      send_to_char( "你並沒有這個裝備﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !obj->cachet )
    {
      act( "$p並沒有封印。", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( obj->wear_loc != WEAR_NONE )
    {
      act( "$p目前無法解除封印。", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    for ( pCachet = obj->cachet; pCachet; pCachet = pCachet_next )
    {
      pCachet_next = pCachet->next;

      if ( ( pIndex = get_obj_index( pCachet->vnum ) ) )
      {
        stone_obj = create_object( ObjMagicStone, -1 );

        if ( !stone_obj )
        {
          send_to_char( "對不起﹐系統無法產生魔石﹗\n\r", ch );
          mudlog( LOG_DEBUG, "do_cachet: 無法產生魔石." );
          continue;
        }

        /* 把物品給此人 */
        obj_to_char( stone_obj, ch );

        act( "$n將$p的封印卸下﹐$p不再晶光耀眼了﹐不過$n身上似乎多了一顆$P﹗"
          , ch, obj, stone_obj, TO_ALL );
      }

      free_struct( pCachet, STRUCT_CACHET_DATA );
    }

    obj->cachet = NULL;

    save_char_obj( ch, SAVE_FILE );
    RETURN_NULL();
  }

  if ( !( stone_obj = get_obj_carry( ch, arg1 ) ) )
  {
    send_to_char( "你身上並沒有這一顆魔石﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( !is_magic_stone( stone_obj ) )
  {
    act( "$p並不是一顆魔石﹗", ch, stone_obj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( !( obj = get_obj_carry( ch, arg2 ) ) )
  {
    send_to_char( "你身上並沒有這一個裝備﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( !can_cachet( obj ) || !stone_obj->pIndexData->impact )
  {
    act( "$p根本沒辦法封印魔石。", ch, obj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( obj_stone( obj ) > MAX_CAN_CACHET )
  {
    chinese_number( MAX_CAN_CACHET, buf );
    act( "$p已經封印超過最大封印限制數 $T 了﹗", ch, obj, buf, TO_CHAR );
    RETURN_NULL();
  }

  for ( pCachet = obj->cachet; pCachet; pCachet = pCachet->next )
  {
    if ( pCachet->vnum == stone_obj->pIndexData->vnum )
    {
      act( "$p已經封印有相同種類的魔石了。", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }
  }

  /* 配置記憶體 */
  set_cachet_default( pCachet = alloc_struct( STRUCT_CACHET_DATA ) );
  pCachet->vnum = stone_obj->pIndexData->vnum;
  pCachet->next = obj->cachet;
  obj->cachet   = pCachet;

  act( "$n把$p封印到$P裡面﹐$p發出一陣閃光之後就消失了。$n"
    "感到$P有一些不一樣的地方﹐發出陣陣的光芒。"
    , ch, stone_obj, obj, TO_ALL );

  message_driver( ch, stone_obj, ACT_WHEN_CACHET );

  /* 釋放物品 */
  extract_obj( stone_obj );

  /* 儲存裝備 */
  save_char_obj( ch, SAVE_FILE );

  RETURN_NULL();
}

/* 檢查物品是否為魔石 */
bool is_magic_stone( OBJ_DATA * obj )
{
  PUSH_FUNCTION( "is_magic_stone" );

  if ( !obj )
  {
    mudlog( LOG_DEBUG, "is_magic_stone: 傳入物品不存在." );
    RETURN( FALSE );
  }

  if ( obj->item_type != ITEM_MAGICSTONE || !obj->pIndexData->impact )
    RETURN( FALSE );

  RETURN( TRUE );
}

/* 察看物品是否可以封印 */
bool can_cachet( OBJ_DATA * obj )
{
  PUSH_FUNCTION( "can_cachet" );

  if ( !obj )
  {
    mudlog( LOG_DEBUG, "can_cachet: 傳入的物品不正確." );
    RETURN( FALSE );
  }

  if ( !is_armor( obj ) || !obj->Cancachet ) RETURN( FALSE );

  RETURN( TRUE );
}

/* 傳回物品封印魔石的數量 */
int obj_stone( OBJ_DATA * obj )
{
  int           number;
  CACHET_DATA * pCachet;

  PUSH_FUNCTION( "obj_stone" );

  if ( !obj ) RETURN( ERRORCODE );

  number = 0;
  for ( pCachet = obj->cachet; pCachet; pCachet = pCachet->next ) number++;

  RETURN( number );
}

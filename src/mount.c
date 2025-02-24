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
#include "merc.h"

bool    mount_char      args( ( CHAR_DATA *, CHAR_DATA * ) );

/* 騎上馬 */
FUNCTION( do_mount )
{
  CHAR_DATA * mount;
  char        arg[MAX_INPUT_LENGTH];
  char        buf[MAX_STRING_LENGTH];
  int         adept;

  PUSH_FUNCTION( "do_mount" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  one_argument( argument, arg );

  /* 沒有參數就是查詢坐騎 */
  if ( !arg[0] )
  {
    if ( ( mount = ch->mount ) )
    {
      sprintf( buf, "%d/%d", mount->move, get_curr_move( mount ) );
      act( "你的座騎是$N(體力﹕$t)", ch, buf, mount, TO_CHAR );
    }

    else
    {
      send_to_char( "你沒有任何的座騎喔。\n\r", ch );
    }

    RETURN_NULL();
  }

  if ( ( adept = ch->skill[SLOT_MOUNT] )  <= 0 )
  {
    send_to_char( "對不起﹐你還不會騎術這一招耶﹗\n\r", ch );
    RETURN_NULL();
  }

  /* 不能重複有坐騎 */
  if ( ch->mount )
  {
    act( "你已經騎著$N了。", ch, NULL, ch->mount, TO_CHAR );
    RETURN_NULL();
  }

  /* 檢查是否自己是別人的坐騎 */
  if ( ch->mount_by )
  {
    act( "你自己已經被$N騎著了。", ch, NULL, ch->mount_by, TO_CHAR );
    RETURN_NULL();
  }

  /* 檢查是否自己本身合適與否 */
  if ( ch->tractable > 0 )
  {
    send_to_char( "你不適合有座騎。\n\r", ch );
    RETURN_NULL();
  }

  /* 不能有飛行術 */
  if ( is_affected( ch, SLOT_FLY ) )
  {
    send_to_char( "你雙腳離地﹐無法騎上你的坐騎。\n\r", ch );
    RETURN_NULL();
  }

  /* 找尋房間裡面的坐騎 */
  if ( !( mount = get_char_room( ch, arg ) ) )
  {
    act( "找不到你的座騎 $2$T$0。", ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  /* 看看這隻動物是否已經被騎走了 */
  if ( !IS_NPC( mount ) || mount->tractable <= 0 )
  {
    act( "$N竟然想騎到你的頭上。", mount, NULL, ch, TO_CHAR );
    act( "你認錯你的座騎了$N。", ch, NULL, mount, TO_CHAR );
    RETURN_NULL();
  }

  /* 看看是否被迷惑或是有主人的 */
  if ( mount->master || mount->leader )
  {
    act( "$N有其它的主人了。", ch, NULL, mount, TO_CHAR );
    RETURN_NULL();
  }

  /* 已經是別人的坐騎的 */
  if ( mount->mount_by )
  {
    act( "$N竟然想搶你的座騎耶。", mount->mount_by, NULL, ch, TO_CHAR );
    act( "$N已經是其它人的座騎了。", ch, NULL, mount, TO_CHAR );
    RETURN_NULL();
  }

  /* 看看坐騎的狀態是否正確 */
  if ( mount->position != POS_STANDING )
  {
    act( "$N目前暫時沒辦法騎。", ch, NULL, mount, TO_CHAR );
    RETURN_NULL();
  }

  /* 坐騎是否可以看到你 */
  if ( !can_see( mount, ch ) )
  {
    act( "$N看不到你耶。", ch, NULL, mount, TO_CHAR );
    RETURN_NULL();
  }

  /* 看看是否體力是否足夠 */
  if ( ch->move < MountCost )
  {
    send_to_char( "你的體力不濟﹐雙腳發軟﹐使不上力。\n\r", ch );
    ch->move = 0;
    RETURN_NULL();
  }

  /* 扣除體力耗損 */
  ch->move = UMAX( 0, ch->move - MountCost );

  /* 檢查機率 */
  if ( number_range( UMIN( 100, adept * 5 ) , 100 ) < mount->tractable )
  {
    act( "$N突然兇性大發﹐$n從$N身上摔了下來。", ch, NULL, mount, TO_ALL );
    ch->move  = UMAX( 0, ch->move - MountCost );

    RETURN_NULL();
  }

  if ( !mount_char( ch, mount ) )
  {
    send_to_char( "你失敗了。\n\r" , ch );
    RETURN_NULL();
  }

  if ( IS_NPC( mount ) && mount->talk_buf )
  {
    free_string( mount->talk_buf );
    mount->talk_buf  = NULL;
    mount->talk_mode = MODE_ABSENT;
  }

  /* 輸出敘述 */
  act( "$N成為$n的座騎了。"  , ch, NULL, mount, TO_ALL );

  /* 獲得經驗值 */
  gain_skill_exp( ch, SLOT_MOUNT, TRUE );

  RETURN_NULL();
}

/* 走下坐騎 */
FUNCTION( do_unmount )
{
  PUSH_FUNCTION( "do_unmount" );

  /* 檢查是否已經是否有坐騎 */
  if ( !ch->mount )
  {
    send_to_char( "你並沒有任何的座騎喔。\n\r", ch );
    RETURN_NULL();
  }

  if ( !unmount_char( ch, ch->mount ) )
  {
    send_to_char( "你失敗了。\n\r", ch );
    RETURN_NULL();
  }

  adjust_carry( ch );
  RETURN_NULL();
}

/* 檢查與套上坐騎 */
bool mount_char( CHAR_DATA * rider, CHAR_DATA * ridden )
{
  PUSH_FUNCTION( "mount_char" );

  /* 檢查條件 */
  if ( rider->mount
    || rider->mount_by
    || rider->tractable > 0
    || !IS_NPC( ridden )
    || ridden->tractable <= 0
    || ridden->mount_by
    || ridden->master
    || ridden->leader )
    RETURN( FALSE );

  rider->mount     = ridden;
  ridden->mount_by = rider;

  RETURN( TRUE );
}

/* 坐騎解套 */
bool unmount_char( CHAR_DATA * rider, CHAR_DATA * ridden )
{
  PUSH_FUNCTION( "unmount_char" );

  /* 先檢查來源 */
  if ( !rider || !ridden || !rider->mount ) RETURN( FALSE );

  /* 檢查其與被騎是否一致 */
  if ( rider->mount != ridden || ridden->mount_by != rider )
  {
    mudlog( LOG_DEBUG, "unmount_char: 騎與被騎不吻合" );
    RETURN( FALSE );
  }

  rider->mount     = NULL;
  ridden->mount_by = NULL;

  /* 輸出敘述 */
  if ( rider->in_room && rider->in_room == ridden->in_room )
    act( "$n從$N身上滾了下來。", rider, NULL, ridden, TO_ALL );

  RETURN( TRUE );
}

/* 傳回某人的坐騎 */
CHAR_DATA * get_mount( CHAR_DATA * ch )
{
  PUSH_FUNCTION( "get_mount" );
  if ( !ch || !ch->mount || ch->in_room != ch->mount->in_room ) RETURN( NULL );
  RETURN( ch->mount );
}

/* 傳回某坐騎的主人 */
CHAR_DATA * get_rider( CHAR_DATA * ch )
{
  PUSH_FUNCTION( "get_rider" );

  if ( !ch || !ch->mount_by || ch->in_room != ch->mount_by->in_room )
    RETURN( NULL );

  RETURN( ch->mount_by );
}

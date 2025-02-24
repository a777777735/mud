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

extern void set_exit_default args( ( EXIT_DATA * ) );

const   sh_int  rev_dir [] =
{
  DIR_SOUTH, DIR_WEST, DIR_NORTH, DIR_EAST,
  DIR_DOWN,  DIR_UP,   DIR_OUT  , DIR_ENTER
};

char * const attr_name[ MAX_ATTR ] =
{
  "力量" , "智力" , "學識" , "敏捷" , "體格"
};

/* 區域函數 */
bool    has_key         args( ( CHAR_DATA * , int ) );

bool move_char( CHAR_DATA * ch, int door )
{
  CHAR_DATA       * fch;
  CHAR_DATA       * fch_next;
  CHAR_DATA       * mount;
  CHAR_DATA       * walker;
  ROOM_INDEX_DATA * in_room;
  ROOM_INDEX_DATA * to_room;
  SECTOR_DATA     * in_sector;
  SECTOR_DATA     * to_sector;
  SKILL_DATA      * pSwim;
  EXIT_DATA       * pExit;
  OBJ_DATA        * obj;
  BUS_DATA        * pBus;
  bool              found;
  bool              same;
  int               canswim;
  int               move;

  PUSH_FUNCTION( "move_char" );

  switch( door )
  {
  default:
    mudlog( LOG_DEBUG , "Do_move: 錯誤的方向 %d.", door );
    RETURN( FALSE );

  case DIR_NORTH:
  case DIR_SOUTH:
  case DIR_EAST:
  case DIR_WEST:
  case DIR_UP:
  case DIR_DOWN:
  case DIR_ENTER:
  case DIR_OUT:
    break;
  }

  if ( !( in_room = ch->in_room ) || !( in_sector = in_room->sector ) )
  {
    mudlog( LOG_DEBUG, "move_char: 沒有所在房間." );
    RETURN( FALSE );
  }

  if ( !( pExit = in_room->exit[door] )
    || !( to_room = pExit->to_room )
    || !( to_sector = to_room->sector ) )
  {
    send_to_char( "對不起﹐那裡沒有路﹐請用 \e[1;32mexit\e[0m "
      "來查詢出口﹗\n\r", ch );

    RETURN( FALSE );
  }

  if ( pExit->closed && !is_affected( ch, SLOT_PASS_DOOR ) )
  {
    act( "真是糟糕﹐$t方的門被關起來了。"
      , ch, direction_name( door ), NULL, TO_CHAR );
    RETURN( FALSE );
  }

  if ( ( is_affected( ch, SLOT_CHARM_PERSON ) && ch->master
    && in_room == ch->master->in_room )
    || ch->mount_by )
  {
    send_to_char( "對不起﹐你不能離開你的主人喔。\n\r", ch );
    RETURN( FALSE );
  }

  if ( room_is_private( to_room ) )
  {
    act( "對不起﹐$t是私人房間。", ch, to_room->name, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  /* 檢查是否為驛站, 檢查是否有攜帶坐騎 */
  if ( ( pBus = is_platform( to_room ) ) && get_mount( ch ) )
  {
    send_to_char( "為維護整潔﹐嚴禁攜帶動物入內。\n\r", ch );
    RETURN( FALSE );
  }

  if ( is_platform( to_room ) && ch->spirit )
  {
    act( "根據規定﹐你是不能帶$N進來的﹗", ch, NULL, ch->spirit, TO_CHAR );
    RETURN( FALSE );
  }

  walker  = ( mount = get_mount( ch ) ) ? mount : ch;
  canswim = -1;

  if ( mount && mount->position != POS_STANDING )
  {
    act( "你的$N無法跟你行動。", ch, NULL, mount, TO_CHAR );
    RETURN( FALSE );
  }

  if ( ( in_sector->fly || to_sector->fly )
    && !is_affected( walker, SLOT_FLY ) )
  {
    send_to_char( "唉﹐你還不能飛。\n\r", ch );
    RETURN( FALSE );
  }

  if ( in_sector->swim || to_sector->swim )
  {
    found = is_affected( walker, SLOT_FLY );

    if ( !( pSwim = get_skill( SLOT_SWIM ) ) || mount ) canswim = 0;
    else canswim = ch->skill[SLOT_SWIM];

    for ( obj = ch->carrying; obj; obj = obj->next_content )
    {
      if ( obj->item_type == ITEM_BOAT )
      {
        found = TRUE;
        break;
      }
    }

    if ( !found )
    {
      if ( canswim <= 0 )
      {
        send_to_char( "你目前需要一艘船。\n\r", ch );
        RETURN( FALSE );
      }

      /* 開始遊泳 */
      gain_skill_exp( ch, SLOT_SWIM, TRUE );

      if ( number_range( 1, 100 ) > canswim + get_curr_dex( ch ) )
      {
        send_to_char( "對不起﹐你沒有游過去。\n\r", ch );

        ch->move = UMAX( 0, ch->move - in_sector->movement );
        RETURN( FALSE );
      }
    }
  }

  move = in_sector->movement + to_sector->movement;

  if ( walker->move < move )
  {
    if ( canswim <= 0 )
    {
      if ( walker == ch ) send_to_char( "你太累了﹐你已經走不動了。\n\r", ch );
      else                act( "你的$N太累了。", ch, NULL, walker, TO_CHAR );
    }
    else
    {
      send_to_char( "你太累了﹐你已經游不動了。\n\r", ch );
    }

    RETURN( FALSE );
  }

  walker->move -= move;

  if ( !is_affected( ch, SLOT_SNEAK )
    && ( IS_NPC( ch ) || !IS_SET( ch->act, PLR_WIZINVIS ) ) )
  {
    if ( canswim <= 0 )
    {
      if ( mount ) act( "$n騎著$N往$t方離去。"
        , ch, direction_name( door ), mount, TO_ROOM );

      else act( "$n往$t方離去。"
        , ch, direction_name( door ), NULL, TO_ROOM );
     }
     else
     {
       act( "$n往$t方游去。" , ch, direction_name( door ), NULL, TO_ROOM );
     }
  }

  /* 如果房間是一模沒有兩樣 */
  same = ( in_room == to_room ) ? TRUE : FALSE;

  char_from_room( ch );
  char_to_room( ch, to_room );

  if ( mount )
  {
    char_from_room( mount );
    char_to_room( mount, to_room );
  }

  if ( !is_affected( ch, SLOT_SNEAK )
    && ( IS_NPC( ch ) || !IS_SET( ch->act, PLR_WIZINVIS ) ) )
  {
    if ( canswim <= 0 )
    {
      if ( mount ) act( "$n騎著$N往這邊過來。", ch, NULL, mount, TO_ROOM );
      else         act( "$n往這邊走來。"      , ch, NULL, NULL,  TO_ROOM );
    }
    else
    {
      act( "$n往這裡游來。", ch, NULL, NULL, TO_ROOM );
    }

    if ( IS_SET( ch->act, PLR_THIEF ) )
      act( "$n是一個盜賊喔﹗大家請小心財物﹗", ch, NULL, NULL, TO_ROOM );
  }

  if ( pExit->message && *pExit->message )
    act( "$t", ch, pExit->message, NULL, TO_CHAR );

  do_look( ch, "auto" );

  if ( same ) RETURN( FALSE );

  for ( fch = in_room->people; fch; fch = fch_next )
  {
    fch_next = fch->next_in_room;

    if ( fch->boss == ch && fch->position == POS_STANDING )
    {
      act( "你跟隨著你的主人$N。", fch, NULL, ch, TO_CHAR );
      move_char( fch, door );
    }
  }

  for ( fch = in_room->people; fch; fch = fch_next )
  {
    fch_next = fch->next_in_room;

    if ( fch->master == ch && fch->position == POS_STANDING )
    {
      act( "你跟隨著$N。", fch, NULL, ch, TO_CHAR );
      move_char( fch, door );
    }
  }

  if ( !is_dead( ch ) ) mprog_entry_trigger( ch );
  if ( !is_dead( ch ) ) mprog_greet_trigger( ch );

  /* 辨識記仇 */
  if ( !is_dead( ch ) && IS_NPC( ch ) ) mprog_enroll_trigger( ch );

  RETURN( TRUE );
}

FUNCTION( do_north )
{
  PUSH_FUNCTION( "do_north" );
  if ( move_char( ch, DIR_NORTH ) ) set_map( ch, DIR_NORTH );
  RETURN_NULL();
}

FUNCTION( do_east )
{
  PUSH_FUNCTION( "do_east" );
  if ( move_char( ch, DIR_EAST ) ) set_map( ch, DIR_EAST );
  RETURN_NULL();
}

FUNCTION( do_south )
{
  PUSH_FUNCTION( "do_south" );
  if ( move_char( ch, DIR_SOUTH ) ) set_map( ch, DIR_SOUTH );
  RETURN_NULL();
}

FUNCTION( do_west )
{
  PUSH_FUNCTION( "do_west" );
  if ( move_char( ch, DIR_WEST ) ) set_map( ch, DIR_WEST );
  RETURN_NULL();
}

FUNCTION( do_up )
{
  PUSH_FUNCTION( "do_up" );
  if ( move_char( ch, DIR_UP ) ) set_map( ch, DIR_UP );
  RETURN_NULL();
}

FUNCTION( do_down )
{
  PUSH_FUNCTION( "do_down" );

  if ( ch->in_room && ch->in_room == RoomSchool )
  {
    char_from_room( ch );
    char_to_room( ch, get_hometown( ch ) );
    do_look( ch, "auto" );
    clear_trace( ch, TRUE );
  }

  else
  {
    if ( move_char( ch, DIR_DOWN ) ) set_map( ch, DIR_DOWN );
  }

  RETURN_NULL();
}

FUNCTION( do_enter )
{
  PUSH_FUNCTION( "do_enter" );
  if ( move_char( ch, DIR_ENTER ) ) set_map( ch, DIR_ENTER );
  RETURN_NULL();
}

FUNCTION( do_out )
{
  PUSH_FUNCTION( "do_out" );
  if ( move_char( ch, DIR_OUT ) ) set_map( ch, DIR_OUT );
  RETURN_NULL();
}

FUNCTION( do_spy )
{
  ROOM_INDEX_DATA * pRoom;
  ROOM_INDEX_DATA * zRoom;
  SECTOR_DATA     * pSector;
  SECTOR_DATA     * zSector;
  CHAR_DATA       * victim;
  EXIT_DATA       * pExit;
  OBJ_DATA        * obj;
  char              arg[MAX_INPUT_LENGTH];
  int               door;
  int               move;
  int               canswim;
  bool              found;

  PUSH_FUNCTION( "do_spy" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    send_to_char( "你要往那個方向偵察呢﹖\n\r", ch );
    RETURN_NULL();
  }

  if ( !( pRoom = ch->in_room ) || !( pSector = pRoom->sector ) )
  {
    send_to_char( "你到底身在何處呢﹖\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->position != POS_STANDING )
  {
    send_to_char( "你目前的狀態不適合偵察﹗\n\r", ch );
    RETURN_NULL();
  }

       if ( !str_prefix( arg, "north" ) ) door = DIR_NORTH;
  else if ( !str_prefix( arg, "east"  ) ) door = DIR_EAST;
  else if ( !str_prefix( arg, "south" ) ) door = DIR_SOUTH;
  else if ( !str_prefix( arg, "west"  ) ) door = DIR_WEST;
  else if ( !str_prefix( arg, "up"    ) ) door = DIR_UP;
  else if ( !str_prefix( arg, "down"  ) ) door = DIR_DOWN;
  else if ( !str_prefix( arg, "enter" ) ) door = DIR_ENTER;
  else if ( !str_prefix( arg, "out"   ) ) door = DIR_OUT;
  else
  {
    send_to_char( "那不是一個正確的方向﹐請查詢 direction﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( !( pExit = pRoom->exit[door] )
    || !( zRoom = pExit->to_room )
    || !( zSector = zRoom->sector ) )
  {
    send_to_char( "對不起﹐那裡沒有出口喔﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( pExit->closed && !is_affected( ch, SLOT_PASS_DOOR ) )
  {
    act( "真是糟糕﹐$t方的門被關起來了。"
      , ch, direction_name( door ), NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( ( is_affected( ch, SLOT_CHARM_PERSON ) && ch->master
    && pRoom == ch->master->in_room )
    || ch->mount_by )
  {
    send_to_char( "對不起﹐你不能離開你的主人喔。\n\r", ch );
    RETURN_NULL();
  }

  if ( room_is_private( zRoom ) )
  {
    act( "對不起﹐$t是私人房間﹐所以你不能去偵察﹗", ch, zRoom->name, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( get_mount( ch ) || ch->spirit )
  {
    send_to_char( "你沒有辦法騎著坐騎或是帶著小鬼去偵察的﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( ( pSector->fly || zSector->fly ) && !is_affected( ch, SLOT_FLY ) )
  {
    send_to_char( "唉﹐你還不能飛﹐導致你無法去偵察﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( pSector->swim || zSector->swim )
  {
    found   = is_affected( ch, SLOT_FLY );
    canswim = ch->skill[SLOT_SWIM];

    for ( obj = ch->carrying; obj; obj = obj->next_content )
    {
      if ( obj->item_type == ITEM_BOAT )
      {
        found = TRUE;
        break;
      }
    }

    if ( !found )
    {
      if ( canswim <= 0 )
      {
        send_to_char( "你需要一艘船來偵察﹗\n\r", ch );
        RETURN_NULL();
      }

      send_to_char( "你沒有辦法游泳去偵察﹗\n\r", ch );
      RETURN_NULL();
    }
  }

  move = pSector->movement + zSector->movement;

  if ( ch->move < move )
  {
    send_to_char( "你太累了﹐你已經無法進行偵察﹗\n\r", ch );
    RETURN_NULL();
  }

  ch->move -= move;

  act( "$n往$T方向偵察﹗", ch, NULL, direction_name( door ), TO_ALL );

  char_from_room( ch );
  char_to_room( ch, zRoom );

  do_look( ch, "auto" );

  act( "$n偷偷摸摸往這裡觀望﹐不知道要幹什麼﹗", ch, NULL, NULL, TO_ROOM );

  if ( !is_dead( ch ) ) mprog_entry_trigger( ch );
  if ( !is_dead( ch ) ) mprog_greet_trigger( ch );

  /* 辨識記仇 */
  if ( !is_dead( ch ) && IS_NPC( ch ) ) mprog_enroll_trigger( ch );

  /* 有問題的就不送回去 */
  for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    if ( victim->fighting && ch == victim->fighting ) break;

  if ( victim )
  {
    act( "$A$1$n$1的行蹤被$N$1發現了!$0$A"
      , ch, NULL, victim, TO_CHAR );
    act( "$1$n的行蹤雖隱密, 但依然被$N發現, 兩人大打出手!$0"
      , ch, NULL, victim, TO_ROOM );

    clear_trace( ch, TRUE );

    RETURN_NULL();
  }

  if ( !is_dead( ch )
    && ch->position == POS_STANDING
    && !ch->fighting )
  {
    char_from_room( ch );
    char_to_room( ch, pRoom );
    WAIT_STATE( ch, 5 );
  }

  RETURN_NULL();
}

int find_door( CHAR_DATA * ch, char * arg )
{
  EXIT_DATA * pExit;
  int         door;

  PUSH_FUNCTION( "find_door" );

  if ( !ch || !ch->in_room )
  {
    mudlog( LOG_DEBUG, "find_door: 缺乏來源." );
    RETURN( ERRORCODE );
  }
       if ( !str_prefix( arg, "north" ) ) door = DIR_NORTH;
  else if ( !str_prefix( arg, "east"  ) ) door = DIR_EAST;
  else if ( !str_prefix( arg, "south" ) ) door = DIR_SOUTH;
  else if ( !str_prefix( arg, "west"  ) ) door = DIR_WEST;
  else if ( !str_prefix( arg, "up"    ) ) door = DIR_UP;
  else if ( !str_prefix( arg, "down"  ) ) door = DIR_DOWN;
  else if ( !str_prefix( arg, "enter" ) ) door = DIR_ENTER;
  else if ( !str_prefix( arg, "out"   ) ) door = DIR_OUT;
  else
  {
    for ( door = 0; door < DIR_MAX; door++ )
    {
      if ( ( pExit = ch->in_room->exit[door] )
        && pExit->isdoor
        && pExit->keyword
        && is_name( arg, pExit->keyword ) )
          RETURN( door );
    }

    act( "奇怪﹐我在$t那裡沒有看到任何的門耶。",ch , arg, NULL, TO_CHAR );
    RETURN( ERRORCODE );
  }

  if ( !( pExit = ch->in_room->exit[door] ) )
  {
    act( "我在$t方並沒有看到任何的門耶。\n\r"
      , ch, direction_name( door ), NULL, TO_CHAR );
    RETURN( ERRORCODE );
  }

  if ( !pExit->isdoor )
  {
    send_to_char( "那裡雖然有路但是並不是門﹐所以你不能這樣做。\n\r", ch );
    RETURN( ERRORCODE );
  }

  RETURN( door );
}

FUNCTION( do_open )
{
  char              arg[MAX_INPUT_LENGTH];
  OBJ_DATA        * obj;
  CHAR_DATA       * rch;
  int               door;
  ROOM_INDEX_DATA * to_room;
  EXIT_DATA       * pExit;
  EXIT_DATA       * pExit_rev;

  PUSH_FUNCTION( "do_open" );
  one_argument( argument, arg );

  if ( ( obj = get_obj_here( ch, arg ) ) )
  {
    /* 物品是否有餵毒 */
    if ( poison_char( ch, obj ) ) RETURN_NULL();

    if ( obj->item_type != ITEM_CONTAINER )
    {
      act( "$p並不是一個容器。", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( !IS_SET( obj->value[1], CONT_CLOSED ) )
    {
      act( "$p已經被打開了。", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( !IS_SET( obj->value[1], CONT_CLOSEABLE ) )
    {
      send_to_char( "對不起﹐你不能那樣做。\n\r", ch );
      RETURN_NULL();
    }

    if ( IS_SET( obj->value[1], CONT_LOCKED ) )
    {
      act( "$p是鎖上的。", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    REMOVE_BIT( obj->value[1], CONT_CLOSED );
    send_to_char( "好了﹐打開了。\n\r", ch );
    act( "$n將$p打開了。", ch, obj, NULL, TO_ROOM );
    message_driver( ch, obj, ACT_WHEN_OPEN );
    RETURN_NULL();
  }

  if ( ( door = find_door( ch, arg ) ) >= 0 )
  {
    pExit = ch->in_room->exit[door];

    if ( !pExit->closed )
    {
      send_to_char( "門它已經被打開了。\n\r", ch );
      RETURN_NULL();
    }

    if ( pExit->locked )
    {
      send_to_char( "它是鎖上的。\n\r", ch );
      RETURN_NULL();
    }

    pExit->closed = FALSE;

    act( "$n打開$T方的門。", ch, NULL, direction_name( door ), TO_ROOM );
    send_to_char( "好了﹐門打開了。\n\r", ch );

    if ( ( to_room   = pExit->to_room               )
      && ( pExit_rev = to_room->exit[rev_dir[door]] )
      && pExit_rev->to_room == ch->in_room )
    {
      pExit_rev->closed = FALSE;

      for ( rch = to_room->people; rch; rch = rch->next_in_room )
        act( "$d打開了。", rch, NULL, pExit_rev->keyword, TO_CHAR );
    }
  }
  RETURN_NULL();
}

FUNCTION( do_close )
{
  char              arg[MAX_INPUT_LENGTH];
  OBJ_DATA        * obj;
  CHAR_DATA       * rch;
  int               door;
  ROOM_INDEX_DATA * to_room;
  EXIT_DATA       * pExit;
  EXIT_DATA       * pExit_rev;

  PUSH_FUNCTION( "do_close" );
  one_argument( argument, arg );

  if ( ( obj = get_obj_here( ch, arg ) ) )
  {
    /* 物品是否有餵毒 */
    if ( poison_char( ch, obj ) ) RETURN_NULL();

    if ( obj->item_type != ITEM_CONTAINER )
    {
      act( "$p並不是一個容器喔。", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( IS_SET(obj->value[1], CONT_CLOSED) )
    {
      act( "$p已經被關起來了。", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( !IS_SET(obj->value[1], CONT_CLOSEABLE) )
    {
      act( "$p不能關起來的﹐所以你不能這樣做。", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    SET_BIT(obj->value[1], CONT_CLOSED);
    act( "好了﹐你已經把$p關起來了。", ch, obj, NULL, TO_CHAR );
    act( "$n把$p關起來。", ch, obj, NULL, TO_ROOM );
    message_driver( ch, obj, ACT_WHEN_CLOSE );
    RETURN_NULL();
  }

  if ( ( door = find_door( ch, arg ) ) >= 0 )
  {
    pExit = ch->in_room->exit[door];
    if ( pExit->closed )
    {
      send_to_char( "那邊的門已經被關起來了。\n\r", ch );
      RETURN_NULL();
    }

    pExit->closed = TRUE;

    act( "$n關了$T方的門。", ch, NULL, direction_name( door ), TO_ROOM );
    send_to_char( "好了﹐你把那邊的門關起來了。\n\r", ch );

    if ( ( to_room   = pExit->to_room )
      && ( pExit_rev = to_room->exit[rev_dir[door]] ) != 0
      && pExit_rev->to_room == ch->in_room )
    {
      pExit->closed = TRUE;

      for ( rch = to_room->people; rch; rch = rch->next_in_room )
        act( "$d關起來。", rch, NULL, pExit_rev->keyword, TO_CHAR );
    }
  }
  RETURN_NULL();
}

bool has_key( CHAR_DATA * ch, int key )
{
  OBJ_DATA * obj;

  PUSH_FUNCTION( "has_key" );

  for ( obj = ch->carrying; obj; obj = obj->next_content )
    if ( obj->pIndexData->vnum == key ) RETURN( TRUE );

  RETURN( FALSE );
}

FUNCTION( do_lock )
{
  char              arg[MAX_INPUT_LENGTH];
  OBJ_DATA        * obj;
  int               door;
  ROOM_INDEX_DATA * to_room;
  EXIT_DATA       * pExit;
  EXIT_DATA       * pExit_rev;

  PUSH_FUNCTION( "do_lock" );

  one_argument( argument, arg );

  if ( ( obj = get_obj_here( ch, arg ) ) )
  {
    /* 物品是否有餵毒 */
    if ( poison_char( ch, obj ) ) RETURN_NULL();

    if ( obj->item_type != ITEM_CONTAINER )
    {
      act( "$p並不是一個容器。", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( !IS_SET(obj->value[1], CONT_CLOSED) )
    {
      act( "$p並沒有被鎖起來。", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( obj->value[2] < 0 )
    {
      act( "你不能把$p鎖起來。", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( !has_key( ch, obj->value[2] ) )
    {
      send_to_char( "你沒有那根鑰匙。\n\r", ch );
      RETURN_NULL();
    }

    if ( IS_SET(obj->value[1], CONT_LOCKED) )
    {
      act( "$p已經被鎖起來了。", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    SET_BIT(obj->value[1], CONT_LOCKED);
    act( "喀啦一聲﹐你已經把$p鎖起來了。", ch, obj, NULL, TO_CHAR );
    act( "$n把$p鎖起來了。", ch, obj, NULL, TO_ROOM );
    message_driver( ch, obj, ACT_WHEN_LOCK );
    RETURN_NULL();
  }

  if ( ( door = find_door( ch, arg ) ) >= 0 )
  {
    pExit = ch->in_room->exit[door];

    if ( !pExit->closed )
    {
      send_to_char( "你要先把它關起來才能鎖。\n\r", ch );
      RETURN_NULL();
    }

    if ( pExit->key < 0 )
    {
      send_to_char( "你不能把它鎖起來。\n\r", ch );
      RETURN_NULL();
    }

    if ( !has_key( ch, pExit->key ) )
    {
      send_to_char( "你沒有那支鑰匙。\n\r" , ch );
      RETURN_NULL();
    }

    if ( pExit->locked )
    {
      send_to_char( "它已經被鎖起來了。\n\r", ch );
      RETURN_NULL();
    }

    pExit->locked = TRUE;

    send_to_char( "喀啦一聲﹐你把門鎖起來了。\n\r", ch );
    act( "$n把$T方的門鎖起來了。", ch, NULL, direction_name( door ), TO_ROOM );

    if ( ( to_room   = pExit->to_room               )
      && ( pExit_rev = to_room->exit[rev_dir[door]] ) != 0
      && pExit_rev->to_room == ch->in_room )
    {
      pExit_rev->locked = TRUE;
    }
  }
  RETURN_NULL();
}

FUNCTION( do_unlock )
{
  char              arg[MAX_INPUT_LENGTH];
  OBJ_DATA        * obj;
  int               door;
  ROOM_INDEX_DATA * to_room;
  EXIT_DATA       * pExit;
  EXIT_DATA       * pExit_rev;

  PUSH_FUNCTION( "do_unlock" );

  one_argument( argument, arg );

  if ( ( obj = get_obj_here( ch, arg ) ) )
  {
    /* 物品是否有餵毒 */
    if ( poison_char( ch, obj ) ) RETURN_NULL();

    if ( obj->item_type != ITEM_CONTAINER )
    {
      act( "$p並不是一個容器。", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( !IS_SET( obj->value[1], CONT_CLOSED) )
    {
      act( "$p並沒有被關起來。", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( obj->value[2] < 0 )
    {
      act( "你無法替$p開鎖。", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( !has_key( ch, obj->value[2] ) )
    {
      send_to_char( "你沒有那支鑰匙。\n\r", ch );
      RETURN_NULL();
    }

    if ( !IS_SET(obj->value[1], CONT_LOCKED) )
    {
      act( "$p的鎖已經被打開了。", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    REMOVE_BIT(obj->value[1], CONT_LOCKED);
    act( "喀啦一聲﹐你把$p的鎖打開了。", ch, obj, NULL, TO_CHAR );
    act( "$n把$p的鎖打開了。", ch, obj, NULL, TO_ROOM );
    message_driver( ch, obj, ACT_WHEN_UNLOCK );
    RETURN_NULL();
  }

  if ( ( door = find_door( ch, arg ) ) >= 0 )
  {
    pExit = ch->in_room->exit[door];

    if ( !pExit->closed )
    {
      send_to_char( "那邊的門並沒有被關起來。\n\r", ch );
      RETURN_NULL();
    }

    if ( pExit->key < 0 )
    {
      send_to_char( "那邊的門並不能開鎖。\n\r", ch );
      RETURN_NULL();
    }

    if ( !has_key( ch, pExit->key) )
    {
      send_to_char( "你沒有那支鑰匙。\n\r", ch );
      RETURN_NULL();
    }

    if ( !pExit->locked )
    {
      send_to_char( "那邊門的鎖已經被打開了。\n\r",  ch );
      RETURN_NULL();
    }

    pExit->locked = FALSE;

    send_to_char( "喀啦一聲﹐你把那邊門的鎖打開了。\n\r", ch );
    act( "$n把$T方門的鎖打開了。", ch, NULL, direction_name( door ), TO_ROOM );

    if ( ( to_room   = pExit->to_room               )
      && ( pExit_rev = to_room->exit[rev_dir[door]] )
      && pExit_rev->to_room == ch->in_room )
    {
      pExit->locked = FALSE;
    }
  }
  RETURN_NULL();
}

FUNCTION( do_stand )
{
  CHAR_DATA * mount;

  PUSH_FUNCTION( "do_stand" );

  mount = get_mount( ch );

  switch ( ch->position )
  {
  case POS_SLEEPING:

    if ( is_affected( ch, SLOT_SLEEP ) )
    {
      send_to_char( "你無法醒過來﹗\n\r", ch );
      RETURN_NULL();
    }

    send_to_char( "你清醒並站了起來。\n\r", ch );
    act( "$n清醒並站了起來。", ch, NULL, NULL, TO_ROOM );
    ch->position = POS_STANDING;
    if ( mount ) do_stand( mount, "" );
    break;

  case POS_RESTING:
    send_to_char( "你站了起來。\n\r", ch );
    act( "$n站了起來。", ch, NULL, NULL, TO_ROOM );
    ch->position = POS_STANDING;
    if ( mount ) do_stand( mount, "" );
    break;

  case POS_STANDING:
    send_to_char( "你已經站了起來。\n\r", ch );
    break;

  case POS_FIGHTING:
    send_to_char( "你還在戰鬥﹗\n\r", ch );
    break;
  }

  RETURN_NULL();
}

FUNCTION( do_rest )
{
  CHAR_DATA * mount;

  PUSH_FUNCTION( "do_rest" );

  if ( ( mount = get_rider( ch ) ) )
  {
    send_to_char( "你還在載人﹐不能休息。\n\r" , ch );
    RETURN_NULL();
  }

  if ( is_pk( ch ) )
  {
    send_to_char( "都什麼時候了﹐還在休息﹐趕快去把敵人給做掉吧﹗\n\r", ch );
    RETURN_NULL();
  }

  switch ( ch->position )
  {
  case POS_SLEEPING:
    send_to_char( "你已經在睡覺了。\n\r", ch );
    break;

  case POS_RESTING:
    send_to_char( "你已經在休息了。\n\r", ch );
    break;

  case POS_STANDING:

    if ( ch->in_room && ch->club && ch->club == ch->in_room->club )
    {
      send_to_char( "你躺在這裡休息﹐特別覺得安全﹗\n\r", ch );
    }
    else
    {
      send_to_char( "你坐下來休息。\n\r", ch );
    }

    act( "$n 坐下來休息。", ch, NULL, NULL, TO_ROOM );
    ch->position = POS_RESTING;
    break;

  case POS_FIGHTING:
    send_to_char( "對不起﹐你還在戰鬥﹗\n\r", ch );
    break;
  }

  RETURN_NULL();
}

FUNCTION( do_sleep )
{
  CHAR_DATA   * mount;
  ENABLE_DATA * pEnable;
  SKILL_DATA  * pSkill = NULL;

  PUSH_FUNCTION( "do_sleep" );

  if ( ( mount = get_mount( ch ) ) )
  {
    act( "你必須先從$N上下來才能睡覺。",ch , NULL, mount, TO_CHAR );
    RETURN_NULL();
  }

  if ( ( mount = get_rider( ch ) ) )
  {
    send_to_char( "你還在載人﹐不能睡覺。\n\r" , ch );
    RETURN_NULL();
  }

  if ( is_pk( ch ) )
  {
    send_to_char( "都什麼時候了﹐還在睡覺﹐趕快去把敵人給做掉吧﹗\n\r", ch );
    RETURN_NULL();
  }

  switch ( ch->position )
  {
  case POS_SLEEPING:
    send_to_char( "你已經睡的不醒人事了。\n\r", ch );
    break;

  case POS_RESTING:
  case POS_STANDING:

    if ( IS_NPC( ch ) && ch->talk_buf )
    {
      free_string( ch->talk_buf );
      ch->talk_buf  = NULL;
      ch->talk_mode = MODE_ABSENT;
    }

    for ( pEnable = ch->enable; pEnable; pEnable = pEnable->next )
    {
      if ( ( pSkill = pEnable->skill ) && pSkill->type == TAR_MEDITATION )
        break;
    }

    if ( pEnable && pSkill && !is_dead( ch ) && ch->jail <= 0 )
    {
      act( "$n施展$w之後﹐運氣於丹田﹐閉目養神﹗", ch, pSkill, NULL, TO_ALL );
      ch->position = POS_SLEEPING;
    }

    else
    {
      if ( ch->in_room && ch->club && ch->club == ch->in_room->club )
      {
        send_to_char( "你躺在這裡睡覺﹐特別覺得香甜﹗\n\r", ch );
      }
      else
      {
        send_to_char( "你躺下來睡覺。\n\r", ch );
      }

      act( "$n躺下來睡覺。", ch, NULL, NULL, TO_ROOM );
      ch->position = POS_SLEEPING;
    }

    break;

  case POS_FIGHTING:
    send_to_char( "你還在戰鬥﹗\n\r", ch );
    break;
  }

  RETURN_NULL();
}

FUNCTION( do_wake )
{
  char        arg[MAX_INPUT_LENGTH];
  CHAR_DATA * victim;
  CHAR_DATA * mount;

  PUSH_FUNCTION( "do_wake" );

  one_argument( argument, arg );

  if ( !arg[0] )
  {
    do_stand( ch, argument );
    RETURN_NULL();
  }

  if ( arg[0] == '@' && IS_IMMORTAL( ch ) )
  {
    if ( arg[1] == '\x0' )
    {
      send_to_char( "你要打斷誰的暫存模式﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( !( victim = get_char_room( ch, arg + 1 ) ) )
    {
      act( "找不到你要打斷的對象 $2$T$0。", ch, NULL, arg+1, TO_CHAR );
      RETURN_NULL();
    }

    if ( IS_NPC( victim ) || !victim->desc )
    {
      act( "無法打斷$N的暫存模式﹗", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( !set_descriptor_stack( victim->desc ) && !is_edit( victim->desc ) )
    {
      act( "對不起﹐$N目前不是在暫存或是編輯模式﹗", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    victim->desc->stack = FALSE;
    cease_stack( victim->desc );
    act( "你已經打斷$N的暫存模式了﹗", ch, NULL, victim, TO_CHAR );

    send_to_char( "\n\r                        ---閱\讀或是編輯模式被中斷---"
      "\a\a\n\r", victim );

    RETURN_NULL();
  }

  if ( !IS_AWAKE( ch ) )
  {
    send_to_char( "你讓自己睡著了﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( !( victim = get_char_room( ch, arg ) ) )
  {
    act( "找不到你的對象 $2$T$0。", ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  if ( IS_AWAKE( victim ) )
  {
    act( "$N已經醒過來了。", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( is_affected( victim, SLOT_SLEEP ) )
  {
    act( "你不能搖醒$N﹗", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  act( "你搖醒$N﹗", ch, NULL, victim, TO_CHAR );
  act( "$n搖醒你﹗", ch, NULL, victim, TO_VICT );
  victim->position = POS_STANDING;

  /* 座騎也要起來 */
  if ( ( mount = get_mount( victim ) ) ) do_stand( mount, "" );

  RETURN_NULL();
}

FUNCTION( do_visible )
{
  PUSH_FUNCTION( "do_visible" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  affect_release( ch, SLOT_INVIS      );
  affect_release( ch, SLOT_SNEAK      );
  affect_release( ch, SLOT_MASS_INVIS );

  REMOVE_BIT( ch->act, PLR_WIZINVIS );

  send_to_char( "在一團紫霧之中﹐你現出你的原形。\n\r", ch );
  act( "$n在一陣紫霧之中現出他的原形。", ch, NULL, NULL, TO_ROOM );
  RETURN_NULL();
}

FUNCTION( do_recall )
{
  char              arg[MAX_INPUT_LENGTH];
  char              buf[MAX_STRING_LENGTH];
  CHAR_DATA       * victim;
  ROOM_INDEX_DATA * location;
  int               lose;
  int               loop;
  int               mem;
  bool              bSub = TRUE;

  PUSH_FUNCTION( "do_recall" );

  /* 怪物沒有家 */
  if ( IS_NPC( ch ) ) RETURN_NULL();

  if ( ch->mount || ch->mount_by )
  {
    send_to_char( "你有坐騎耶﹐先下來吧。\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->spirit )
  {
    if ( ch->spirit->in_room
      && ch->in_room
      && ch->spirit->in_room == ch->in_room )
    {
      act( "你先把你的$N收起來吧﹐不然$E是找不到你的﹗"
        , ch, NULL, ch->spirit, TO_CHAR );
      RETURN_NULL();
    }
  }

  if ( is_pk( ch ) )
  {
    send_to_char( "你正在武鬥大會裡﹐沒有辦法傳送你耶﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->failed > 0 )
  {
    send_to_char( "對不起﹐你還在思過中﹐無法傳送﹗\n\r", ch );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg );

  /* 如果沒有參數, 就是預定點 */
  if ( !arg[0] )
  {
    act( "$n祈禱天神傳送回家。", ch, NULL, NULL, TO_ROOM );

    if ( !( location = get_hometown( ch ) ) )
    {
      send_to_char( "你完全失敗了﹐天神遺棄了你。\n\r", ch );
      RETURN_NULL();
    }
  }

  else if ( is_number( arg ) )
  {
    if ( !ch->pcdata ) RETURN_NULL();

    if ( ( mem = atoi( arg ) ) <= 0 || mem > MAX_RECALL )
    {
      send_to_char( "你的傳送點的範圍不正確。\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->pcdata->recall[mem-1] == 0
      || !( location = get_room_index( ch->pcdata->recall[mem-1] ) ) )
    {
      send_to_char( "你沒有設定這個傳送點喔。\n\r", ch );
      RETURN_NULL();
    }

    if ( !location->area || !ch->in_room->area )
    {
      send_to_char( "這個地方似乎已經不存在了。\n\r", ch );
      RETURN_NULL();
    }

    /* 同一區域扣的比較少 */
    lose = ( location->area == ch->in_room->area ) ? 50 : 150;

    if ( ch->move < lose )
    {
      send_to_char( "你的體力不濟﹐無法傳送。\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->in_room == location
      || ch->in_room->NoRecall
      || is_affected( ch, SLOT_CURSE ) )
    {
      send_to_char( "你不要浪費你的體力吧﹗\n\r", ch );
      RETURN_NULL();
    }

    ch->move -= lose;
    bSub      = FALSE;
    act( "你嘗試回到$t這個地方。",ch ,location->name, NULL, TO_CHAR );
  }

  /* club 參數是回到幫派聚集點 */
  else if ( !str_prefix( arg, "club" ) )
  {
    if ( ch->jail > 0 )
    {
      send_to_char( "對不起﹐你還在坐牢﹐無法傳送﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( is_pk( ch ) )
    {
      send_to_char( "還是專心你的武鬥大會吧﹐別想東想西的﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !ch->club )
    {
      send_to_char( "你沒有參加任何幫派。\n\r", ch );
      RETURN_NULL();
    }

    act( "$n祈禱傳送回幫派聚集點。", ch, NULL, NULL, TO_ROOM );

    if ( !( location = club_recall( ch ) ) )
    {
      send_to_char( "找不到你幫派的聚集點。\n\r", ch );
      RETURN_NULL();
    }
  }

  /* 列出記憶的地方 */
  else if ( !str_prefix( arg, "list" ) && ch->pcdata )
  {
    clear_buffer();
    for ( loop = 0; loop < MAX_RECALL; loop++ )
    {
      location = ch->pcdata->recall[loop] > 0
               ? get_room_index( ch->pcdata->recall[loop] ) : NULL;

      send_to_buffer( "第 %2d 傳送點 %s %s\n\r"
        , loop + 1, location ? location->name : "尚未設定"
        , location && location->area ? location->area->name : "" );

      if ( buffer_full() ) break;
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  /* 記憶 */
  else if ( !str_prefix( arg, "set" ) && ch->pcdata )
  {
    one_argument( argument, arg );

    if ( !arg[0] || !is_number( arg ) )
    {
      send_to_char( "語法錯誤﹐請查詢 recall 用法。\n\r", ch );
      RETURN_NULL();
    }

    if ( ( mem = atoi( arg ) ) <= 0 || mem > MAX_RECALL )
    {
      chinese_number( MAX_RECALL, buf );
      act( "傳送點的範圍在一到$t。", ch, buf, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( !ch->in_room || ch->in_room->Memorize == FALSE )
    {
      send_to_char( "這個地方看起來是記不住的喔。\n\r", ch );
      RETURN_NULL();
    }

    for ( loop = 0; loop < MAX_RECALL; loop++ )
    {
      if ( ch->pcdata->recall[loop] == ch->in_room->vnum )
      {
        act( "$r之前就已經記過了。", ch, NULL, NULL, TO_CHAR );
        RETURN_NULL();
      }
    }

    act( "你以此地當作第$i個傳送點。", ch, &mem, NULL, TO_CHAR );
    ch->pcdata->recall[mem-1] = ch->in_room->vnum;

    RETURN_NULL();
  }

  else if ( !str_cmp( arg, "cancel" ) && ch->pcdata )
  {
    one_argument( argument, arg );

    if ( !arg[0] || !is_number( arg ) )
    {
      send_to_char( "語法錯誤﹐請查詢 recall 用法。\n\r", ch );
      RETURN_NULL();
    }

    if ( ( mem = atoi( arg ) ) <= 0 || mem > MAX_RECALL )
    {
      chinese_number( MAX_RECALL, buf );
      act( "傳送點的範圍在一到$t。", ch, buf, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch->pcdata->recall[mem-1] > 0 )
    {
      act( "你取消了第$i點的傳送點。", ch, &mem, NULL, TO_CHAR );
      ch->pcdata->recall[mem-1] = 0;
    }
    else
    {
      send_to_char( "這個傳送點本來就不存在。\n\r", ch );
    }

    RETURN_NULL();
  }

  else
  {
    send_to_char( "參數錯誤﹐請查詢 recall 這個指令的用法。\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->in_room == location )
  {
    send_to_char( "天啊﹗什麼事也沒有發生。\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->in_room->NoRecall || is_affected( ch, SLOT_CURSE ) )
  {
    send_to_char( "神遺棄了你。\n\r", ch );
    RETURN_NULL();
  }

  if ( ( victim = ch->fighting ) )
  {
    if ( number_bits( 1 ) == 0 )
    {
      WAIT_STATE( ch, 4 );
      lose = ( ch->desc ) ? 50 : 100;
      gain_exp( ch, 0 - lose );
      act( "你失敗了﹗損失了\e[1;5;31m$i$0經驗值。", ch, &lose, NULL, TO_CHAR );
      RETURN_NULL();
    }

    lose = IS_NPC( ch ) ? 200 : 100;
    gain_exp( ch, 0 - lose );
    act( "你成功\脫逃﹗損失了\e[1;5;31m$i$0經驗值。", ch, &lose, NULL, TO_CHAR );
    stop_fighting( ch, TRUE );
  }

  if ( bSub ) ch->move /= 2;

  act( "只見一道閃電劃過﹐$n消失的無影無蹤。", ch, NULL, NULL, TO_ROOM );
  char_from_room( ch );
  char_to_room( ch, location );
  act( "一陣雷電交加後﹐$n突然出現在這裡。", ch, NULL, NULL, TO_ROOM );
  do_look( ch, "auto" );
  clear_trace( ch, TRUE );

  if ( ch->spirit ) act( "你和你的$N迷失了﹗", ch, NULL, ch->spirit, TO_CHAR );

  RETURN_NULL();
}

FUNCTION( do_ply )
{
  int          slot;
  int          cost;
  int          chance;
  int          chance_base;
  bool         pFind;
  char         arg[MAX_INPUT_LENGTH];
  CHAR_DATA  * rch;
  LIMIT_DATA * pLimit;
  SKILL_DATA * pSkill;

  PUSH_FUNCTION( "do_ply" );

  if ( IS_NPC( ch ) || !ch->in_room ) RETURN_NULL();

  argument = one_argument( argument, arg );

  if ( ch->answer )
  {
    send_to_char( "你先回答完你的問題吧﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->position != POS_STANDING )
  {
    send_to_char( "你目前的狀態不適合練習喔﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->in_room == RoomDead || ch->in_room == RoomCorpse )
  {
    act( "對不起﹐在$r是不可以練功\的﹗", ch, NULL, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( arg[0] == '\x0' )
  {
    send_to_char( "你要練習什麼技能呢﹖\n\r", ch );
    RETURN_NULL();
  }

  if ( !( pSkill = skill_isname( arg ) )
    || ch->skill[ ( slot = URANGE( 0,pSkill->slot, MAX_SKILL - 1 ) )] <= 0 )
  {
    send_to_char( "你不會這項技能吧﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( pSkill->ply <= 0 )
  {
    act( "$w是不能練習喔﹗", ch, pSkill, NULL, TO_CHAR );
    RETURN_NULL();
  }

  for ( rch = ch->in_room->people; rch; rch = rch->next_in_room )
  {
    if ( rch->position == POS_FIGHTING )
    {
      act( "你附近有人在打鬥﹐導致你無法專心練惜$w﹗", ch, pSkill, NULL, TO_CHAR );
      RETURN_NULL();
    }
  }

  if ( ch->skill[slot] >= ( ch->level / UMAX( 1, pSkill->qutoient ) ) )
  {
    act( "你已經無法再繼續練$w﹐你應該去冒險了﹗", ch, pSkill, NULL, TO_CHAR );
    RETURN_NULL();
  }

  /* 是否超出職業的限制 */
  for ( pFind = FALSE, pLimit = pSkill->limit; pLimit; pLimit = pLimit->next )
  {
    if ( pLimit->class == ch->class )
    {
      pFind = TRUE;

      if ( ch->level < pLimit->level || ch->skill[slot] >= pLimit->adept )
      {
        act( "你這門$w無法在精鍊下去了﹗", ch, pSkill, NULL, TO_CHAR );
        RETURN_NULL();
      }
      break;
    }
  }

  /* 若是找不到也不能升熟練度 */
  if ( pFind == FALSE )
  {
    act( "你這門$w無法在精鍊下去了﹗", ch, pSkill, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( pSkill->check && ( ( *pSkill->check ) ( ch, TRUE ) ) == FALSE )
    RETURN_NULL();

  cost = pSkill->cost;

  /* 檢查是否有足夠的法力或是體力或是其他....*/
  switch( pSkill->cost_type )
  {
  default :
    mudlog( LOG_DEBUG , "do_cast: 法術 %s 的耗損型態錯誤", pSkill->name );
    RETURN_NULL();

  /* 消耗生命力 */
  case COST_HIT:

    if ( ch->hit < cost )
    {
      act( "你的生命力太低﹐無法練習$w。", ch, pSkill, NULL, TO_CHAR );
      RETURN_NULL();
    }

    ch->hit -= cost;
    break;

  /* 消耗法力 */
  case COST_MANA:

    if ( ch->mana < cost )
    {
      act( "你沒有足夠的法力來練習$w。", ch, pSkill, NULL, TO_CHAR );
      RETURN_NULL();
    }

    ch->mana -= cost;
    break;

  /* 消耗體力 */
  case COST_MOVE:

    if ( ch->move < cost )
    {
      act( "你的體力不濟﹐無法練習$w。", ch, pSkill, NULL, TO_CHAR );
      RETURN_NULL();
    }

    ch->move -= cost;
    break;

  /* 消耗金錢 */
  case COST_GOLD:

    if ( ch->gold < cost )
    {
      act( "你沒有足夠的銀兩﹐無法練習$w。", ch, pSkill, NULL, TO_CHAR );
      RETURN_NULL();
    }

    gold_from_char( ch, cost );
    break;
  }

  if ( ch->pcdata && !IS_IMMORTAL( ch ) ) ch->pcdata->ply++;

  /* 法術或是技能的等待時間 */
  if ( !IS_IMMORTAL( ch ) ) WAIT_STATE( ch, pSkill->wait );

  act( "$n靜靜站在一旁﹐偷偷地練起$w。", ch, pSkill, NULL, TO_ALL );

  /* 設定機率 */
  ch->skill[slot] = UMIN( ch->skill[slot], 100 );
  chance_base     = 10000 + ( 200 * ( ch->skill[slot] + ch->level ) / 2 );
  chance          = get_curr_wis( ch ) + pSkill->ply;

  if ( number_range( 1, chance_base ) < chance )
  {
    act( "$A$A$3恭禧你﹐由於你的勤奮練習﹐你的$w熟練度增加了﹗$0"
      , ch, pSkill, NULL, TO_CHAR );

    ch->skill[slot]++;
  }

  RETURN_NULL();
}

/* 增加屬性 */
FUNCTION( do_train )
{
  CHAR_DATA  * mob;
  CLASS_DATA * pClass;
  AREA_DATA  * area;
  sh_int     * MobAbility;
  sh_int     * CharAbility;
  int          cost;
  int          pType;
  int          limit;
  int          count;
  int          value;
  char         arg[ MAX_INPUT_LENGTH  ];
  char         buf[ MAX_STRING_LENGTH ];

  PUSH_FUNCTION( "do_train" );

  /* 非玩家不能升級屬性 */
  if ( IS_NPC( ch ) ) RETURN_NULL();

  argument = one_argument( argument , arg );

  /* 輸入字串為空字串 */
  if ( !arg[0] )
  {
    print_to_char( ch , "你目前的學習點數共有 \e[1;37m%d\e[0m 點。\n\r"
      "你目前力量(\e[1;33mstr\e[0m)點數為 %3d ﹐"
      "升下一級需要 %s%6d\e[0m 點數。\n\r"
      "你目前智力(\e[1;33mint\e[0m)點數為 %3d ﹐"
      "升下一級需要 %s%6d\e[0m 點數。\n\r"
      "你目前學識(\e[1;33mwis\e[0m)點數為 %3d ﹐"
      "升下一級需要 %s%6d\e[0m 點數。\n\r"
      "你目前敏捷(\e[1;33mdex\e[0m)點數為 %3d ﹐"
      "升下一級需要 %s%6d\e[0m 點數。\n\r"
      "你目前體格(\e[1;33mcon\e[0m)點數為 %3d ﹐"
      "升下一級需要 %s%6d\e[0m 點數。\n\r"
      , ch->practice
      , get_curr_str ( ch )
      , ch->practice >= get_str_train( ch ) ? "\e[1;32m" : "\e[1;31m"
      , get_str_train( ch )
      , get_curr_int ( ch )
      , ch->practice >= get_int_train( ch ) ? "\e[1;32m" : "\e[1;31m"
      , get_int_train( ch )
      , get_curr_wis ( ch )
      , ch->practice >= get_wis_train( ch ) ? "\e[1;32m" : "\e[1;31m"
      , get_wis_train( ch )
      , get_curr_dex ( ch )
      , ch->practice >= get_dex_train( ch ) ? "\e[1;32m" : "\e[1;31m"
      , get_dex_train( ch )
      , get_curr_con ( ch )
      , ch->practice >= get_con_train( ch ) ? "\e[1;32m" : "\e[1;31m"
      , get_con_train( ch ) );

    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "!list" ) )
  {
    clear_buffer();
    send_to_buffer( "\e[1;33;44m順序 區域名稱   同區 中文名稱      "
      "                     力量 智慧 學識 敏捷 體格\e[0m\n\r" );

    for ( count = 0, mob = char_list; mob; mob = mob->next )
    {
      if ( verify_char( mob )
        && IS_NPC( mob )
        && IS_SET( mob->act, ACT_TRAIN ) )
      {
        area = mob->in_room && mob->in_room->area ? mob->in_room->area : NULL;
        str_cpy( buf, mob_name( ch, mob ) );
        buf[34] = '\x0';

        send_to_buffer( "%3d. %-10s  %2s  %-33s   %2s   %2s   %2s   %2s   %2s\n\r"
          , ++count
          , area ? area->name : "從缺"
          , YESNO( area && ch->in_room && ch->in_room->area == area )
          , buf
          , YESNO( get_curr_str( mob ) > ch->now_str + 10 )
          , YESNO( get_curr_int( mob ) > ch->now_int + 10 )
          , YESNO( get_curr_wis( mob ) > ch->now_wis + 10 )
          , YESNO( get_curr_dex( mob ) > ch->now_dex + 10 )
          , YESNO( get_curr_con( mob ) > ch->now_con + 10 ) );
      }
    }

    if ( count == 0 ) send_to_char( "系統裡沒有任何的轉職者﹗\n\r", ch );
    else print_buffer( ch );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "!maxinum" ) )
  {
    if ( !( pClass = ch->class ) )
    {
      send_to_char( "對不起﹐你的職業未明﹐無法查詢﹗\n\r", ch );
      RETURN_NULL();
    }

    clear_buffer();
    send_to_buffer( "你的職業為%s﹐各項屬性的限制為﹕\n\r"
      , class_name( pClass, TRUE ) );

    for ( count = 0; count < MAX_ATTR; count++ )
    {
      switch( count )
      {
      default:       value = 0;           break;
      case STR_ITEM: value = ch->now_str; break;
      case INT_ITEM: value = ch->now_int; break;
      case WIS_ITEM: value = ch->now_wis; break;
      case DEX_ITEM: value = ch->now_dex; break;
      case CON_ITEM: value = ch->now_con; break;
      }

      send_to_buffer( "[%s%s\e[0m] %s屬性的上限﹕%3d「目前數值﹕%3d」\n\r"
        , pClass->attr[count] > value ? "\e[1;32m" : "\e[1;31m"
        , YESNO( pClass->attr[count] > value )
        , attr_name[count]
        , pClass->attr[count]
        , value );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  /* IMM 不能升級屬性 */
  if ( IS_IMMORTAL( ch ) )
  {
    send_to_char( "你是系統管理員﹐不可以升級屬性。\n\r" , ch );
    RETURN_NULL();
  }

  /* 看看是否能夠找到訓練者 */
  if ( !( mob = get_char_room( ch , arg ) ) )
  {
    send_to_char( "這裡沒有那個訓練者﹐還是向別人打聽看看吧。\n\r" , ch );
    RETURN_NULL();
  }

  if ( mob == ch )
  {
    send_to_char( "你怎麼可以自己訓練自己﹐開什麼玩笑﹖\n\r" , ch );
    RETURN_NULL();
  }

  if ( !( pClass = ch->class ) )
  {
    send_to_char( "你的職業未明﹐無法幫你訓練。\n\r", ch );
    RETURN_NULL();
  }

  one_argument( argument , arg );

  if ( !arg[0] )
  {
    send_to_char( "你的語法錯誤﹐請查詢 train。\n\r" , ch );
    RETURN_NULL();
  }

  /* 是否願意幫別人升級屬性, 非玩家部份 */
  if ( IS_NPC( mob ) )
  {
    if ( !IS_SET( mob->act, ACT_TRAIN ) )
    {
      sprintf( buf , "%s 對不起﹐我不能幫你訓練﹗" , ch->name );
      do_tell( mob , buf );
      RETURN_NULL();
    }
  }

  /* 是否願意幫別人升級屬性, 玩家部份 */
  else
  {
    if ( !IS_SET( mob->turn , PLR_TRAIN ) )
    {
      sprintf( buf , "%s 對不起﹐我不能幫你訓練﹗" , ch->name );
      do_tell( mob , buf );
      RETURN_NULL();
    }
  }

  /* 想要訓練力量 */
  if ( !str_cmp( arg , "str" ) )
  {
    if ( get_curr_str( ch ) >= MaxStr )
    {
      send_to_char( "對不起﹐你的力量已達系統的最大值﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->now_str >= pClass->attr[STR_ITEM] )
    {
      send_to_char( "對不起﹐你的力量已經不能再提升了﹗\n\r", ch );
      RETURN_NULL();
    }

    CharAbility = &ch->now_str;
    MobAbility  = &mob->now_str;
    cost        = get_str_train( ch );
    pType       = STR_ITEM;
    limit       = get_curr_str( ch ) >= pClass->attr[pType] ? TRUE : FALSE;
  }

  /* 有關智慧 */
  else if ( !str_cmp( arg , "int" ) )
  {
    if ( get_curr_int( ch ) >= MaxInt )
    {
      send_to_char( "對不起﹐你的智慧已達系統的最大值﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->now_int >= pClass->attr[INT_ITEM] )
    {
      send_to_char( "對不起﹐你的智力已經不能再提升了﹗\n\r", ch );
      RETURN_NULL();
    }

    CharAbility = &ch->now_int;
    MobAbility  = &mob->now_int;
    cost        = get_int_train( ch );
    pType       = INT_ITEM;
    limit       = get_curr_int( ch ) >= pClass->attr[pType] ? TRUE : FALSE;
  }

  /* 想要訓練學識 */
  else if ( !str_cmp( arg , "wis" ) )
  {
    if ( get_curr_wis( ch ) >= MaxWis )
    {
      send_to_char( "對不起﹐你的學識已達系統的最大值﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->now_wis >= pClass->attr[WIS_ITEM] )
    {
      send_to_char( "對不起﹐你的學識已經不能再提升了﹗\n\r", ch );
      RETURN_NULL();
    }

    CharAbility = &ch->now_wis;
    MobAbility  = &mob->now_wis;
    cost        = get_wis_train( ch );
    pType       = WIS_ITEM;
    limit       = get_curr_wis( ch ) >= pClass->attr[pType] ? TRUE : FALSE;
  }

  /* 想要訓練敏捷 */
  else if ( !str_cmp( arg , "dex" ) )
  {
    if ( get_curr_dex( ch ) >= MaxDex )
    {
      send_to_char( "對不起﹐你的敏捷已達系統的最大值﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->now_dex >= pClass->attr[DEX_ITEM] )
    {
      send_to_char( "對不起﹐你的敏捷已經不能再提升了﹗\n\r", ch );
      RETURN_NULL();
    }

    CharAbility = &ch->now_dex;
    MobAbility  = &mob->now_dex;
    cost        = get_dex_train( ch );
    pType       = DEX_ITEM;
    limit       = get_curr_dex( ch ) >= pClass->attr[pType] ? TRUE : FALSE;
  }

  /* 想要訓練體格 */
  else if ( !str_cmp( arg , "con" ) )
  {
    if ( get_curr_con( ch ) >= MaxCon )
    {
      send_to_char( "對不起﹐你的體格已達系統的最大值﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->now_con >= pClass->attr[CON_ITEM] )
    {
      send_to_char( "對不起﹐你的體格已經不能再提升了﹗\n\r", ch );
      RETURN_NULL();
    }

    CharAbility =  &ch->now_con;
    MobAbility  = &mob->now_con;
    cost        = get_con_train( ch );
    pType       = CON_ITEM;
    limit       = get_curr_con( ch ) >= pClass->attr[pType] ? TRUE : FALSE;
  }

  /* 都不是其中的關鍵字 */
  else
  {
    send_to_char( "你的語法錯誤﹐請查詢 train。\n\r" , ch );
    RETURN_NULL();
  }

  /* 學習點數不夠的時候 */
  if ( ch->practice < cost )
  {
    sprintf( buf , "%s 等你的學習點數多一點時候﹐再來吧﹗" , ch->name );
    do_tell( mob , buf );
    RETURN_NULL();
  }

  /* 看看等級之間的差別或是職業先天限制來決定是否成功 */
  if ( number_range( 1 , *MobAbility - *CharAbility ) < 2  || limit )
  {
    sprintf( buf , "%s 對不起﹐讓你白費苦心了﹐你的%s並沒有進步﹗"
      , ch->name , attr_name[ pType ] );
    do_tell( mob , buf );
    ch->practice = UMAX( 0 , ch->practice - ( *CharAbility * 5 ) );
    RETURN_NULL();
  }

  /* 送出字串 */
  sprintf( buf, "在$N的細心教導之下﹐你的%s似乎稍有進步。", attr_name[pType] );
  act( smash_act_keyword( buf, "nesNES" ) , ch , NULL , mob , TO_ALL );

  (*CharAbility)++;
  ch->practice -= cost;
  RETURN_NULL();
}

FUNCTION( do_learn )
{
  char             arg[ MAX_INPUT_LENGTH ];
  char             buf[ MAX_STRING_LENGTH ];
  CHAR_DATA      * mob;
  TEACH_DATA     * teach;
  TEACH_DATA     * prev;
  SKILL_DATA     * pSkill;
  LIMIT_DATA     * pLimit;
  MOB_INDEX_DATA * pIndex;
  int              cost;
  int              inventory;
  int              sn;
  int              pWeapon = 0;
  int              fTeach;
  int              adept;
  int              count;
  bool             CanInc;
  size_t           pRating;

  PUSH_FUNCTION( "do_learn" );

  /* 非玩家與沒有職業者不能做這個動作 */
  if ( IS_NPC( ch ) || !ch->class ) RETURN_NULL();

  argument = one_argument( argument , arg );

  /* 沒有任何參數就是查詢自己的技能 */
  if ( !arg[0] )
  {
    clear_buffer();
    send_to_buffer( "你所會的技能有 %d 個﹐還可以學 %d 個技能﹕\n\r"
      , learn_count( ch )
      , UMAX( 0, ( get_learnable( ch ) - learn_count( ch ) ) ) );

    for ( pSkill = skill_list; pSkill; pSkill = pSkill->next )
    {
      if ( ( adept = ch->skill[pSkill->slot] ) > 0 )
      {
        CanInc = FALSE;

        /* 是否超出職業的限制 */
        for ( pLimit = pSkill->limit; pLimit; pLimit = pLimit->next )
        {
          if ( pLimit->class == ch->class )
          {
            if ( ch->level < pLimit->level || adept >= pLimit->adept )
              break;

            CanInc = TRUE;
            break;
          }
        }

        adept = UMIN( 100 , adept );
        send_to_buffer( "%-20s %-20s %s\e[0m%s"
          , pSkill->name
          , pSkill->cname
          , CanInc ? " " : "\e[1;32m*"
          , adeptation_name( ( adept ) ) );

        if ( SkillValue || IS_IMMORTAL( ch ) )
          send_to_buffer( "  %3d%%"  , adept );

        send_to_buffer( " [%s] ", YESNO(check_skill_enable( ch, pSkill ) ) );

             if ( pSkill->type == TAR_DODGE  ) send_to_buffer( " 防禦類技能" );
        else if ( pSkill->type == TAR_FLEE   ) send_to_buffer( " 逃跑類技能" );
        else if ( pSkill->rating == RATING_SKILL )
          send_to_buffer( " %s類技能" , weapon_type_name( pSkill->weapon ) );

        else send_to_buffer( " %s的法術"
            , pSkill->rating ? skill_rating( pSkill->rating ) : "未知" );

        send_to_buffer( "\n\r" );
      }
    }
    print_buffer( ch );
    RETURN_NULL();
  }

  if ( arg[0] == '$' )
  {
    if ( !arg[1] || !is_number( arg + 1 ) )
    {
      send_to_char( "你必須註明怪物的虛擬號碼﹗\n\r", ch );
      RETURN_NULL();
    }

    sn = atoi( arg + 1 );
    if ( !( pIndex = get_mob_index( sn ) ) )
    {
      act( "找不到編號 $i 的虛擬怪物﹗", ch, &sn, NULL, TO_CHAR );
      RETURN_NULL();
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\x0' )
    {
      send_to_char( "你必須註明教導的技能名稱﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !( pSkill = skill_isname( arg ) ) )
    {
      act( "找不到 $t 這種技能名稱﹗", ch, arg, NULL, TO_CHAR );
      RETURN_NULL();
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\x0' || !is_number( arg ) )
    {
      act( "你必須註明$w的熟練度﹗", ch, pSkill, NULL, TO_CHAR );
      RETURN_NULL();
    }

    adept = atoi( arg );
    if ( adept < 0 || adept > 100 )
    {
      act( "$w技能的熟練度 $x 不合理﹗", ch, pSkill, &adept, TO_CHAR );
      RETURN_NULL();
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\x0' || !is_number( arg ) )
    {
      act( "你必須註明$w的教導價錢﹗", ch, pSkill, NULL, TO_CHAR );
      RETURN_NULL();
    }

    cost = atoi( arg );
    if ( cost < 0 )
    {
      act( "$w技能的教導金 $x 不合理﹗", ch, pSkill, &cost, TO_CHAR );
      RETURN_NULL();
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\x0' || !is_number( arg ) )
    {
      act( "你必須註明$w的隱藏程度﹗", ch, pSkill, NULL, TO_CHAR );
      RETURN_NULL();
    }

    inventory = atoi( arg );
    if ( inventory < 0 || inventory > 100 )
    {
      act( "$w技能的隱藏程度 $x 不合理﹗", ch, pSkill, &inventory, TO_CHAR );
      RETURN_NULL();
    }

    if ( adept <= 0 )
    {
      for ( prev = NULL, teach = pIndex->teach; teach;
            prev = teach, teach = teach->next )
      {
        if ( teach->slot == pSkill->slot )
        {
          if ( !prev ) pIndex->teach = pIndex->teach->next;
          else         prev->next    = teach->next;

          free_struct( teach , STRUCT_TEACH_DATA );

          act( "你取消了$t的$W技能教導﹗"
            , ch, mob_index_name( NULL, pIndex ), pSkill, TO_CHAR );

          top_teach--;
          RETURN_NULL();
        }
      }

      act( "$t本來就沒有教導$W這種技能﹗"
        , ch, mob_index_name( NULL, pIndex ), pSkill, TO_CHAR );

      RETURN_NULL();
    }

    else
    {
      for ( teach = pIndex->teach; teach; teach = teach->next )
      {
        if ( teach->slot == pSkill->slot )
        {
          teach->slot      = pSkill->slot;
          teach->cost      = cost;
          teach->adept     = adept;
          teach->inventory = inventory;

          sprintf( buf, "你把%s的$w教導資料改為﹕$A教授金﹕%d 兩$A熟練度﹕"
            "%d%%$A隱藏度﹕%d%%"
            , mob_index_name( NULL, pIndex )
            , cost, adept, inventory );

          act( smash_act_keyword( buf, "nesw" ), ch, pSkill, NULL, TO_CHAR );
          RETURN_NULL();
        }
      }

      teach = alloc_struct( STRUCT_TEACH_DATA );

      teach->slot      = pSkill->slot;
      teach->cost      = cost;
      teach->adept     = adept;
      teach->inventory = inventory;

      teach->next      = pIndex->teach;
      pIndex->teach    = teach;
      top_teach++;

      sprintf( buf, "你把%s的$w教導資料設定﹕$A教授金﹕%d 兩$A熟練度﹕"
         "%d%%$A隱藏度﹕%d%%"
         , mob_index_name( NULL, pIndex )
         , cost, adept, inventory );

      act( smash_act_keyword( buf, "nesw" ), ch, pSkill, NULL, TO_CHAR );
      RETURN_NULL();
    }

    RETURN_NULL();
  }

  if ( arg[0] == '#' )
  {
    if ( !arg[1] )
    {
      act( "請輸入要查詢的技能種類﹗", ch, NULL, NULL, TO_CHAR );
      RETURN_NULL();
    }

    switch( arg[1] )
    {
    default:
      act( "沒有這種類的武器喔﹗", ch, NULL, NULL, TO_CHAR );
      RETURN_NULL();

    case 'a':
      if ( !str_prefix( arg + 1, "axe" ) )    pWeapon = WEAPON_AXE;
      break;

    case 'b':
      if ( !str_prefix( arg + 1, "blade" ) )  pWeapon = WEAPON_BLADE;
      if ( !str_prefix( arg + 1, "bow" ) )    pWeapon = WEAPON_BOW;
      break;

    case 'c':
      if ( !str_prefix( arg + 1, "club" ) )   pWeapon = WEAPON_CLUB;
      break;

    case 'd':
      if ( !str_prefix( arg + 1, "dagger" ) ) pWeapon = WEAPON_DAGGER;
      break;

    case 'f':
      if ( !str_prefix( arg + 1, "flee"  ) )  pWeapon = WEAPON_FLEE;
      if ( !str_prefix( arg + 1, "force" ) )  pWeapon = WEAPON_FORCE;
      break;

    case 'h':
      if ( !str_prefix( arg + 1, "hammer" ) ) pWeapon = WEAPON_HAMMER;
      if ( !str_prefix( arg + 1, "hand" ) )   pWeapon = WEAPON_HAND;
      break;

    case 'p':
      if ( !str_prefix( arg + 1, "pen" ) )    pWeapon = WEAPON_PEN;
      break;

    case 's':
      if ( !str_prefix( arg + 1, "spear" ) )  pWeapon = WEAPON_SPEAR;
      if ( !str_prefix( arg + 1, "steps" ) )  pWeapon = WEAPON_STEPS;
      if ( !str_prefix( arg + 1, "sword" ) )  pWeapon = WEAPON_SWORD;
      break;

    case 'w':
      if ( !str_prefix( arg + 1, "whip" ) )   pWeapon = WEAPON_WHIP;
      break;
    }

    if ( pWeapon == 0 )
    {
      act( "沒有你所輸入的技能種類喔﹗", ch, NULL, NULL, TO_CHAR );
      RETURN_NULL();
    }

    clear_buffer();
    send_to_buffer( "你所會的%s類技能有﹕\n\r"
      , weapon_type_name( pWeapon ) );

    for ( pSkill = skill_list; pSkill; pSkill = pSkill->next )
    {
      if ( ( adept = ch->skill[pSkill->slot] ) > 0
        && ( pSkill->weapon == pWeapon
        || ( pSkill->type == TAR_DODGE && pWeapon == WEAPON_STEPS )
        || ( pSkill->type == TAR_FLEE  && pWeapon == WEAPON_FLEE  ) ) )
      {
        adept = UMIN( 100 , adept );
        send_to_buffer( "%-20s %-20s %s"
          , pSkill->name
          , pSkill->cname
          , adeptation_name( ( adept ) ) );

        if ( SkillValue || IS_IMMORTAL( ch ) )
          send_to_buffer( "  %3d%%"  , adept );

        send_to_buffer( " [%s] ", YESNO(check_skill_enable( ch, pSkill ) ) );

             if ( pSkill->type == TAR_DODGE ) send_to_buffer( " 防禦類技能" );
        else if ( pSkill->type == TAR_FLEE  ) send_to_buffer( " 逃跑類技能" );
        else send_to_buffer( " %s類技能", weapon_type_name( pSkill->weapon ) );
        send_to_buffer( "\n\r" );
      }
    }
    print_buffer( ch );
    RETURN_NULL();
  }

  if ( arg[0] == '@' )
  {
    if ( !arg[1] )
    {
      send_to_char( "你要查詢那一類的技能呢﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( ( pRating = check_skill_rating( arg + 1 ) ) <= 0 )
    {
      act( "沒有這類技能 $2$T$0 啦﹗", ch, NULL, arg +1, TO_CHAR );
      RETURN_NULL();
    }

    if ( learn_rating_count( ch, pRating ) <= 0 )
    {
      send_to_char( "你不會這一類的技能﹗\n\r" , ch );
      RETURN_NULL();
    }

    clear_buffer();
    send_to_buffer( "你所會的%s類技能有 %d 個﹕\n\r"
      , skill_rating( pRating )
      , learn_rating_count( ch, pRating ) );

    for ( pSkill = skill_list; pSkill; pSkill = pSkill->next )
    {
      if ( ( adept = ch->skill[pSkill->slot] ) > 0
        && pSkill->rating == pRating )
      {
        adept = UMIN( 100 , adept );
        send_to_buffer( "%-20s %-20s %s"
          , pSkill->name
          , pSkill->cname
          , adeptation_name( ( adept ) ) );

        if ( SkillValue || IS_IMMORTAL( ch ) )
          send_to_buffer( "  %3d%%"  , adept );

        send_to_buffer( " [%s] ", YESNO(check_skill_enable( ch, pSkill ) ) );

             if ( pSkill->type == TAR_DODGE ) send_to_buffer( " 防禦類技能" );
        else if ( pSkill->type == TAR_FLEE  ) send_to_buffer( " 逃跑類技能" );
        else if ( pRating == RATING_SKILL   ) send_to_buffer( " %s類技能"
            , weapon_type_name( pSkill->weapon ) );
        else send_to_buffer( " %s法術", skill_rating( pRating ) );
        send_to_buffer( "\n\r" );
      }

    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( arg[0] == '!' )
  {
    if ( !arg[1] )
    {
      mob = ch;
    }

    else if ( !( mob = get_char_world( ch, arg + 1 ) ) )
    {
      act( "找不到你的對象 $2$T$0。", ch, NULL, arg+1, TO_CHAR );
      RETURN_NULL();
    }

    if ( !biliwick( ch, mob ) )
    {
      act( "你的權力不夠觀看$N的技能﹗", ch, NULL, mob, TO_CHAR );
      RETURN_NULL();
    }

    clear_buffer();
    send_to_buffer( "%s所會的技能有 %d 個﹕\n\r"
      , mob_name( ch, mob )
      , learn_count( mob ) );

    for ( pSkill = skill_list; pSkill; pSkill = pSkill->next )
    {
      if ( ( adept = mob->skill[pSkill->slot] ) > 0 )
      {
        CanInc = FALSE;

        /* 是否超出職業的限制 */
        for ( pLimit = pSkill->limit; pLimit; pLimit = pLimit->next )
        {
          if ( pLimit->class == ch->class )
          {
            if ( mob->level < pLimit->level || adept >= pLimit->adept )
              break;

            CanInc = TRUE;
            break;
          }
        }

        adept = UMIN( 100 , adept );
        send_to_buffer( "%-20s %-20s %s\e[0m%s"
          , pSkill->name
          , pSkill->cname
          , CanInc ? " " : "\e[1;32m*"
          , adeptation_name( ( adept ) ) );

        if ( SkillValue || IS_IMMORTAL( ch ) )
          send_to_buffer( "  %3d%%"  , adept );

        send_to_buffer( " [%s] ", YESNO(check_skill_enable( mob, pSkill ) ) );

        if ( pSkill->type        == TAR_DODGE )
          send_to_buffer( " 防禦類技能" );
        else if ( pSkill->type   == TAR_FLEE )
          send_to_buffer( " 逃跑類技能" );
        else if ( pSkill->rating == RATING_SKILL )
          send_to_buffer( " %s類技能"
            , weapon_type_name( pSkill->weapon ) );
        else
          send_to_buffer( " %s類法術"
            , pSkill->rating ? skill_rating( pSkill->rating ) : "未知" );

        send_to_buffer( "\n\r" );
      }
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  /* 看看是否能夠找到訓練者 */
  if ( !( mob = get_char_room( ch , arg ) ) )
  {
    act( "這裡沒有那個訓練者 $2$T$0﹐還是向別人打聽看看吧。" , ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  if ( mob == ch )
  {
    send_to_char( "想要自創技能﹖你可能還要努力努力喔。\n\r" , ch );
    RETURN_NULL();
  }

  if ( !IS_NPC( mob ) && !IS_SET( mob->turn , PLR_PRACTICE ) )
  {
    act( "看來$N不太想傳授技能給你。", ch, NULL, mob, TO_CHAR );
    RETURN_NULL();
  }

  one_argument( argument , arg );

  if ( !arg[0] )
  {
    send_to_char( "對不起﹐你的語法不對﹐請查詢 learn。\n\r" , ch );
    RETURN_NULL();
  }

  switch( mob->position )
  {
  default: break;

  case POS_SLEEPING:
    act( "看起來$N還沒睡飽喔﹗", ch, NULL, mob, TO_CHAR );
    RETURN_NULL();

  case POS_DEAD:
    if ( IS_NPC( mob ) )
    {
      mudlog( LOG_DEBUG, "do_learn: 怪物死亡。" );
      RETURN_NULL();
    }

    sprintf( buf , "%s 如果我能復活再教你吧﹗。" , ch->name );
    do_tell( mob , buf );
    RETURN_NULL();

  case POS_FIGHTING:
    sprintf( buf , "%s 等我打完再說吧﹗。" , ch->name );
    do_tell( mob , buf );
    RETURN_NULL();

  }

  /* 想知道那個人的技能有哪些 */
  if ( !str_prefix( arg , "!list" ) )
  {
    /* 不能打聽玩家的技能 */
    if ( !IS_NPC( mob ) )
    {
      act( "抱歉﹐你不能打聽$N所會技能。", ch, NULL, mob, TO_CHAR );
      RETURN_NULL();
    }

    /* 先看看是否有足夠的體力去問技能 */
    if ( ch->move < PRACTICE_MOVE )
    {
      send_to_char( "你的體力不濟﹐什麼也問不到。\n\r" , ch );
      RETURN_NULL();
    }

    count     = 0;
    ch->move -= PRACTICE_MOVE;
    clear_buffer();

    for ( teach = mob->pIndexData->teach; teach; teach = teach->next )
    {
      if ( ( ( number_percent() - 1 ) >= teach->inventory )
        && ( ( pSkill = get_skill( teach->slot ) ) ) )
      {
        if ( count == 0 ) send_to_buffer( "%s\e[0m會下列的技能﹕\n\r\n\r"
          , mob_name( ch, mob ) );

        send_to_buffer( "\t[%2d] %s %s\n\r"
          , ++count , pSkill->name , pSkill->cname );
      }
    }

    if ( count == 0 ) send_to_buffer(
      "看起來%s什麼技能也不會﹐平常的打緊。\n\r" , mob_name( ch, mob ) );

    print_buffer( ch );
    RETURN_NULL();
  }

  /* 先找是否真的有這項技能 */
  if ( !( pSkill = skill_isname( arg ) ) )
  {
    send_to_char( "這種技能相傳已經失傳了很久﹐可能沒有人會了吧﹖\n\r" , ch );
    RETURN_NULL();
  }

  sn     = pSkill->slot;
  fTeach = FALSE;
  teach  = NULL;

  /* 先看看玩家會不會 */
  if ( ch->skill[sn] > 0 )
  {
    act( "你都已經會$w了﹐幹嗎來找我呢﹖" , ch, pSkill, NULL, TO_CHAR );
    RETURN_NULL();
  }

  /* 看看是否會真的會這項技能 */
  if ( IS_NPC( mob ) )
  {
    for ( teach = mob->pIndexData->teach; teach; teach = teach->next )
    {
      if ( teach->slot == sn )
      {
        if ( ch->gold < teach->cost )
        {
          sprintf( buf , "%s 你身上的錢不夠耶﹗" , ch->name );
          do_tell( mob , buf );
          RETURN_NULL();
        }

        else
        {
          gold_from_char( ch, teach->cost );
        }

        fTeach = TRUE;
        break;
      }
    }
  }

  else
  {
    if ( mob->skill[sn] > 0 ) fTeach = TRUE;
  }

  if ( fTeach == FALSE )
  {
    sprintf( buf , "%s 我也不太會這項技能耶﹐不如你教我吧。" , ch->name );
    do_tell( mob , buf );
    RETURN_NULL();
  }

  /* 徒弟的體力不夠 */
  if ( ch->move < PRACTICE_MOVE )
  {
    send_to_char( "你太累了﹐什麼也學不到。\n\r" , ch );
    RETURN_NULL();
  }

  /* 老師的體力不夠 */
  if ( !IS_NPC( mob ) && mob->move < PRACTICE_MOVE )
  {
    act( "$N的體力不濟﹐你什麼也學不到。" , ch, NULL, mob, TO_CHAR );
    RETURN_NULL();
  }

  /* 檢查技能的限制 */
  if ( !check_skill_restrict( ch, pSkill, TRUE ) ) RETURN_NULL();
  if ( !check_valid_rating( ch, pSkill, TRUE ) ) RETURN_NULL();

  /* 扣掉學習所耗的體力 */
  ch->move -= PRACTICE_MOVE;
  if ( !IS_NPC( mob ) ) mob->move -= PRACTICE_MOVE;

  /* 老師是玩家, 但熟練度不夠 */
  if ( !IS_NPC( mob ) && mob->skill[sn] < 30 )
  {
    sprintf( buf , "%s 抱歉﹐我自己也不太會耶﹗" , ch->name );
    do_tell( mob , buf );
    RETURN_NULL();
  }

  if ( !ch->class || learn_count( ch ) > get_learnable( ch ) )
  {
    sprintf( buf , "%s 對不起﹐你再也記不住那麼多招式了﹗\n\r" , ch->name );
    do_tell( mob , buf );
    RETURN_NULL();
  }

  /* 學習的等級限制 */
  if ( ch->level < get_skill_class_level( pSkill, ch->class ) )
  {
    sprintf( buf , "%s 你的等級或是職業不足以學習這項招式﹗" , ch->name );
    do_tell( mob , buf );
    RETURN_NULL();
  }

  /* 是否能由玩家教導 */
  if ( !IS_NPC( mob ) && pSkill->teach == FALSE )
  {
    sprintf( buf , "%s 對不起﹐這項絕世技能是不能外傳的﹗" , ch->name );
    do_tell( mob , buf );
    RETURN_NULL();
  }

  /* 老師是非玩家而且教導失敗 */
  if ( IS_NPC( mob ) && ( ( number_percent() - 1 ) > teach->adept * 2 ) )
  {
    sprintf( buf , "%s 抱歉﹐今天身體不佳﹐改天再教你﹗" , ch->name );
    do_tell( mob , buf );
    RETURN_NULL();
  }

  /* 老師是玩家而且教導失敗 */
  if ( !IS_NPC( mob ) &&
     ( ( number_percent() - 1 ) > mob->skill[sn] * 2 ) )
  {
    sprintf( buf , "%s 抱歉﹐今天招式突然忘了﹐改天再教你﹗" , ch->name );
    do_tell( mob , buf );
    RETURN_NULL();
  }

  IS_NPC(mob) ? ( adept = teach->adept ) : ( adept = mob->skill[sn] );

  /* 計算徒弟的熟練度, 由 1 到 10 */
  adept = number_range( 1 , UMIN( 10, adept / 3 ) );
  adept = UMIN( adept , 10 );
  adept = UMAX( adept , 1  );

  /* 老師是玩家, 則徒弟的熟練度要從老師這裡扣 */
  if ( !IS_NPC( mob ) ) mob->skill[sn] -= adept;

  if ( !IS_NPC( mob ) && IS_IMMORTAL( mob ) )
    mudlog( LOG_INFO, "[TEACH] 神族 %s 教導 %s 技能 %s ."
      , mob->name, ch->name, pSkill->name );

  /* 設定徒弟的熟練度 */
  ch->skill[sn] = adept;

  act( "恭禧你﹐你學會了$w這項技能。",ch , pSkill, NULL, TO_CHAR );
  RETURN_NULL();
}

size_t learn_count( CHAR_DATA * ch )
{
  SKILL_DATA * pSkill;
  size_t       count = 0;

  PUSH_FUNCTION( "learn_count" );

  for ( pSkill = skill_list; pSkill; pSkill = pSkill->next )
    if ( ch->skill[ pSkill->slot ] > 0 ) count++;

  RETURN( count );
}

size_t learn_rating_count( CHAR_DATA * ch, int pRating )
{
  SKILL_DATA * pSkill;
  size_t       count = 0;

  PUSH_FUNCTION( "learn_rating_count" );

  for ( pSkill = skill_list; pSkill; pSkill = pSkill->next )
    if ( ch->skill[ pSkill->slot ] > 0
      && pSkill->rating == pRating ) count++;

  RETURN( count );
}

FUNCTION( do_find )
{
  CHAR_DATA * victim;
  CHAR_DATA * mob;
  int         count;

  PUSH_FUNCTION( "do_find" );

  if ( IS_NPC( ch ) || !ch->in_room ) RETURN_NULL();

  if ( FindCost > 0 && ch->gold < FindCost && ch->level > AngelLevel )
  {
    act( "等你湊足了$i兩銀子﹐再來打聽吧﹗", ch, &FindCost, NULL, TO_CHAR );
    RETURN_NULL();
  }

  count = 0;
  mob   = NULL;

  for ( victim = char_list; victim; victim = victim->next )
  {
    if ( verify_char( victim )
      && IS_NPC( victim )
      && victim->in_room
      && victim->in_room->area == ch->in_room->area
      && !victim->fighting
      && !victim->mount_by
      && !victim->boss
      && victim->migrate <= 0
      && !IS_SET( victim->act, ACT_NOKILL )
      && abs( victim->level - ch->level ) <= 3
      && number_range( 0, count++ ) == 0 )
    {
      mob = victim;
    }
  }

  if ( mob )
  {
    if ( ch->level > AngelLevel && FindCost > 0 )
      gold_from_char( ch, FindCost );

    act( "在你附近$r的$N很適合你升級的怪物﹐趕快去找他吧﹗"
      , ch, NULL, mob, TO_CHAR );

    RETURN_NULL();
  }

  count = 0;
  mob   = NULL;

  for ( victim = char_list; victim; victim = victim->next )
  {
    if ( verify_char( victim )
      && IS_NPC( victim )
      && victim->in_room
      && !victim->fighting
      && !victim->mount_by
      && !victim->boss
      && !IS_SET( victim->act, ACT_NOKILL )
      && abs( victim->level - ch->level ) <= 3
      && number_range( 0, count++ ) == 0 )
    {
      mob = victim;
    }
  }

  if ( mob )
  {
    if ( ch->level > AngelLevel && FindCost > 0 )
      gold_from_char( ch, FindCost );

    act( "位於$t的$N很適合你升級的怪物﹐趕快去找他吧﹗"
      , ch, mob->in_room->area->name, mob, TO_CHAR );
  }

  else
  {
    send_to_char( "對不起﹐找不到你要升級的怪物耶﹗\n\r", ch );
  }

  RETURN_NULL();
}

EXIT_DATA * create_exit( int vHere, int vGoto, int nExit )
{
  EXIT_DATA       * pExit;
  ROOM_INDEX_DATA * tRoom;
  ROOM_INDEX_DATA * pRoom;

  PUSH_FUNCTION( "create_exit" );

  if ( vHere <= 0 || vGoto <= 0 || nExit < 0 || nExit >= DIR_MAX )
  {
    mudlog( LOG_DEBUG, "create_exit: 來源錯誤." );
    RETURN( NULL );
  }

  if ( !( pRoom = get_room_index( vHere ) ) )
  {
    mudlog( LOG_DEBUG, "create_exit: 傳入開門房間有錯誤: %d.", vHere );
    RETURN( NULL );
  }

  if ( !( tRoom = get_room_index( vGoto ) ) )
  {
    mudlog( LOG_DEBUG, "create_exit: 傳入出口房間有誤: %d.", vGoto );
    RETURN( NULL );
  }

  if ( pRoom->exit[nExit] )
  {
    mudlog( LOG_DEBUG, "create_exit: 傳入房間出口已經存在: %d->%d."
      , vHere, nExit );
    RETURN( NULL );
  }

  /* 配置記憶體 */
  set_exit_default( ( pExit = alloc_struct( STRUCT_EXIT_DATA ) ) );

  /* 設定數值與預設值 */
  pRoom->exit[nExit] = pExit;
  pExit->vnum        = vGoto;
  pExit->to_room     = tRoom;
  pExit->description = str_dup( "" );
  pExit->keyword     = str_dup( "" );
  pExit->isdoor      = FALSE;
  pExit->pick        = FALSE;
  pExit->locked      = FALSE;
  pExit->closed      = FALSE;
  pExit->key         = 0;

  RETURN( pExit );
}

void close_exit( int vRoom, int vExit )
{
  ROOM_INDEX_DATA * pRoom;

  PUSH_FUNCTION( "close_exit" );

  if ( vRoom <= 0 || vExit < 0 || vExit >= DIR_MAX )
  {
    mudlog( LOG_DEBUG, "close_exit: 來源錯誤." );
    RETURN_NULL();
  }

  if ( !( pRoom = get_room_index( vRoom ) ) )
  {
    mudlog( LOG_DEBUG, "close_exit: 房間 %d 不存在.", vRoom );
    RETURN_NULL();
  }

  if ( pRoom->exit[vExit] == NULL )
  {
    mudlog( LOG_DEBUG, "close_exit: 房間出口 %d -> %d 不存在卻要釋放."
      , vRoom, vExit );
    RETURN_NULL();
  }

  free_struct( pRoom->exit[vExit], STRUCT_EXIT_DATA );
  pRoom->exit[vExit] = NULL;

  RETURN_NULL();
}

FUNCTION( do_enquire )
{
  int               level;
  int               depth;
  int               loop;
  int               count;
  int               total;
  int               rem;
  char              arg1[MAX_INPUT_LENGTH];
  char              arg2[MAX_INPUT_LENGTH];
  char              buf[MAX_STRING_LENGTH];
  CHAR_DATA       * victim;
  ROOM_INDEX_DATA * pRoom;
  ROOM_INDEX_DATA * zRoom;
  ENQUIRE_DATA    * pEnquire;

  PUSH_FUNCTION( "do_enquire" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );

  if ( arg1[0] == '\x0' || arg2[0] == '\x0' )
  {
    send_to_char( "你要向誰問有關什麼路呢﹖\n\r", ch );
    RETURN_NULL();
  }

  if ( !( pRoom = ch->in_room ) )
  {
    send_to_char( "對不起﹐但你目前身在何處呢﹖\n\r", ch );
    RETURN_NULL();
  }

  if ( !( victim = get_char_room( ch, arg1 ) ) )
  {
    act( "對不起﹐這裡沒有這個人 $2$T$0 喔﹗", ch, NULL, arg1, TO_CHAR );
    RETURN_NULL();
  }

  if ( !can_see( victim, ch ) )
  {
    act( "$N看不到你耶﹐怎麼問路呢﹖", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( !IS_NPC( victim ) || victim->speak == FALSE )
  {
    act( "對不起﹐你不能向$N問路﹗", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( !victim->speak )
  {
    act( "對不起﹐$N不會講話耶﹗", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( victim->position != POS_STANDING )
  {
    act( "$N正在忙耶﹐等一下再問吧﹗", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( ( level = victim->level / 20 ) <= 0 )
  {
    act( "$N還小﹐他什麼都不懂﹐問別人吧﹗", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  act( "$n正在向$N問路﹗", ch, NULL, victim, TO_ALL );

  /* 先看看這裡是否就是 */
  for ( pEnquire = pRoom->enquire; pEnquire; pEnquire = pEnquire->next )
  {
    if ( !str_cmp( pEnquire->keyword, arg2 ) )
    {
      sprintf( buf, "%s 我看你是外地來的吧﹐這裡不就是%s了啦﹗\n\r"
        , ch->name, arg2 );

      do_tell( victim, buf );
      RETURN_NULL();
    }
  }

  depth = UMAX( 1, UMIN( level * 3, MAX_SCAN_DEPTH - 1 ) );

  clean_scan();
  scan_room( pRoom->vnum, depth, -1 );

  buf[0] = '\x0';
  for ( rem = -1, total = loop = 0; loop < ScanPointer; loop++ )
  {
    if ( !( zRoom = get_room_index( ScanData[loop].vnum ) ) ) continue;

    for ( pEnquire = zRoom->enquire; pEnquire; pEnquire = pEnquire->next )
    {
      if ( !str_cmp( arg2, pEnquire->keyword )
        && number_range( 0, total++ ) == 0 )
      {
        rem = loop;
        break;
      }
    }
  }


  if ( rem >= 0 && rem < MAX_SCAN_COUNT )
  {
    sprintf( buf, "%s %s﹐好像之前有聽過﹐你試試看這裡的", ch->name, arg2 );

    for ( count = 0; count < MAX_SCAN_DEPTH; count++ )
    {
       if ( ScanData[rem].dir[count] < 0 ) break;
       str_cat( buf, direction_name( ScanData[rem].dir[count] ) );
    }

    str_cat( buf, "方﹐好像就在那裡﹗" );
    do_tell( victim, buf );
    clean_scan();
    RETURN_NULL();
  }

  clean_scan();

  sprintf( buf, "%s 對不起﹐我也不知道%s在那裡耶﹐你問問其他人吧﹗"
    , ch->name, arg2 );

  do_tell( victim, buf );

  sprintf( buf, "pat %s", victim->name );
  interpret( victim, buf );

  RETURN_NULL();
}


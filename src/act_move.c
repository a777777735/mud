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
*  �o�O�ѻ��j�ƾǨt�s�@�s�Ҽ��g���C���M�D��� merc ��s�ӨӡM�Ҧ������v    *
*  �N�|�Q�O�d�M���w��j�a�ק�M���ڭ̤]�Ʊ�A�̤]�ണ�ѵ��j�a�M�Ҧ�����    *
*  �~�欰�N���Q���\�C                                                      *
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
  "�O�q" , "���O" , "����" , "�ӱ�" , "���"
};

/* �ϰ��� */
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
    mudlog( LOG_DEBUG , "Do_move: ���~����V %d.", door );
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
    mudlog( LOG_DEBUG, "move_char: �S���Ҧb�ж�." );
    RETURN( FALSE );
  }

  if ( !( pExit = in_room->exit[door] )
    || !( to_room = pExit->to_room )
    || !( to_sector = to_room->sector ) )
  {
    send_to_char( "�藍�_�M���̨S�����M�Х� \e[1;32mexit\e[0m "
      "�Ӭd�ߥX�f�T\n\r", ch );

    RETURN( FALSE );
  }

  if ( pExit->closed && !is_affected( ch, SLOT_PASS_DOOR ) )
  {
    act( "�u�O�V�|�M$t�誺���Q���_�ӤF�C"
      , ch, direction_name( door ), NULL, TO_CHAR );
    RETURN( FALSE );
  }

  if ( ( is_affected( ch, SLOT_CHARM_PERSON ) && ch->master
    && in_room == ch->master->in_room )
    || ch->mount_by )
  {
    send_to_char( "�藍�_�M�A�������}�A���D�H��C\n\r", ch );
    RETURN( FALSE );
  }

  if ( room_is_private( to_room ) )
  {
    act( "�藍�_�M$t�O�p�H�ж��C", ch, to_room->name, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  /* �ˬd�O�_���毸, �ˬd�O�_����a���M */
  if ( ( pBus = is_platform( to_room ) ) && get_mount( ch ) )
  {
    send_to_char( "�����@���M�Y�T��a�ʪ��J���C\n\r", ch );
    RETURN( FALSE );
  }

  if ( is_platform( to_room ) && ch->spirit )
  {
    act( "�ھڳW�w�M�A�O����a$N�i�Ӫ��T", ch, NULL, ch->spirit, TO_CHAR );
    RETURN( FALSE );
  }

  walker  = ( mount = get_mount( ch ) ) ? mount : ch;
  canswim = -1;

  if ( mount && mount->position != POS_STANDING )
  {
    act( "�A��$N�L�k��A��ʡC", ch, NULL, mount, TO_CHAR );
    RETURN( FALSE );
  }

  if ( ( in_sector->fly || to_sector->fly )
    && !is_affected( walker, SLOT_FLY ) )
  {
    send_to_char( "���M�A�٤��୸�C\n\r", ch );
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
        send_to_char( "�A�ثe�ݭn�@����C\n\r", ch );
        RETURN( FALSE );
      }

      /* �}�l�C�a */
      gain_skill_exp( ch, SLOT_SWIM, TRUE );

      if ( number_range( 1, 100 ) > canswim + get_curr_dex( ch ) )
      {
        send_to_char( "�藍�_�M�A�S����L�h�C\n\r", ch );

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
      if ( walker == ch ) send_to_char( "�A�Ӳ֤F�M�A�w�g�����ʤF�C\n\r", ch );
      else                act( "�A��$N�Ӳ֤F�C", ch, NULL, walker, TO_CHAR );
    }
    else
    {
      send_to_char( "�A�Ӳ֤F�M�A�w�g�夣�ʤF�C\n\r", ch );
    }

    RETURN( FALSE );
  }

  walker->move -= move;

  if ( !is_affected( ch, SLOT_SNEAK )
    && ( IS_NPC( ch ) || !IS_SET( ch->act, PLR_WIZINVIS ) ) )
  {
    if ( canswim <= 0 )
    {
      if ( mount ) act( "$n�M��$N��$t�����h�C"
        , ch, direction_name( door ), mount, TO_ROOM );

      else act( "$n��$t�����h�C"
        , ch, direction_name( door ), NULL, TO_ROOM );
     }
     else
     {
       act( "$n��$t���h�C" , ch, direction_name( door ), NULL, TO_ROOM );
     }
  }

  /* �p�G�ж��O�@�ҨS����� */
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
      if ( mount ) act( "$n�M��$N���o��L�ӡC", ch, NULL, mount, TO_ROOM );
      else         act( "$n���o�䨫�ӡC"      , ch, NULL, NULL,  TO_ROOM );
    }
    else
    {
      act( "$n���o�̴�ӡC", ch, NULL, NULL, TO_ROOM );
    }

    if ( IS_SET( ch->act, PLR_THIEF ) )
      act( "$n�O�@�ӵs���T�j�a�Фp�߰]���T", ch, NULL, NULL, TO_ROOM );
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
      act( "�A���H�ۧA���D�H$N�C", fch, NULL, ch, TO_CHAR );
      move_char( fch, door );
    }
  }

  for ( fch = in_room->people; fch; fch = fch_next )
  {
    fch_next = fch->next_in_room;

    if ( fch->master == ch && fch->position == POS_STANDING )
    {
      act( "�A���H��$N�C", fch, NULL, ch, TO_CHAR );
      move_char( fch, door );
    }
  }

  if ( !is_dead( ch ) ) mprog_entry_trigger( ch );
  if ( !is_dead( ch ) ) mprog_greet_trigger( ch );

  /* ���ѰO�� */
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
    send_to_char( "�A�n�����Ӥ�V����O�S\n\r", ch );
    RETURN_NULL();
  }

  if ( !( pRoom = ch->in_room ) || !( pSector = pRoom->sector ) )
  {
    send_to_char( "�A�쩳���b��B�O�S\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->position != POS_STANDING )
  {
    send_to_char( "�A�ثe�����A���A�X����T\n\r", ch );
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
    send_to_char( "�����O�@�ӥ��T����V�M�Ьd�� direction�T\n\r", ch );
    RETURN_NULL();
  }

  if ( !( pExit = pRoom->exit[door] )
    || !( zRoom = pExit->to_room )
    || !( zSector = zRoom->sector ) )
  {
    send_to_char( "�藍�_�M���̨S���X�f��T\n\r", ch );
    RETURN_NULL();
  }

  if ( pExit->closed && !is_affected( ch, SLOT_PASS_DOOR ) )
  {
    act( "�u�O�V�|�M$t�誺���Q���_�ӤF�C"
      , ch, direction_name( door ), NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( ( is_affected( ch, SLOT_CHARM_PERSON ) && ch->master
    && pRoom == ch->master->in_room )
    || ch->mount_by )
  {
    send_to_char( "�藍�_�M�A�������}�A���D�H��C\n\r", ch );
    RETURN_NULL();
  }

  if ( room_is_private( zRoom ) )
  {
    act( "�藍�_�M$t�O�p�H�ж��M�ҥH�A����h����T", ch, zRoom->name, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( get_mount( ch ) || ch->spirit )
  {
    send_to_char( "�A�S����k�M�ۧ��M�άO�a�ۤp���h����T\n\r", ch );
    RETURN_NULL();
  }

  if ( ( pSector->fly || zSector->fly ) && !is_affected( ch, SLOT_FLY ) )
  {
    send_to_char( "���M�A�٤��୸�M�ɭP�A�L�k�h����T\n\r", ch );
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
        send_to_char( "�A�ݭn�@����Ӱ���T\n\r", ch );
        RETURN_NULL();
      }

      send_to_char( "�A�S����k��a�h����T\n\r", ch );
      RETURN_NULL();
    }
  }

  move = pSector->movement + zSector->movement;

  if ( ch->move < move )
  {
    send_to_char( "�A�Ӳ֤F�M�A�w�g�L�k�i�氻��T\n\r", ch );
    RETURN_NULL();
  }

  ch->move -= move;

  act( "$n��$T��V����T", ch, NULL, direction_name( door ), TO_ALL );

  char_from_room( ch );
  char_to_room( ch, zRoom );

  do_look( ch, "auto" );

  act( "$n�����N�N���o���[��M�����D�n�F����T", ch, NULL, NULL, TO_ROOM );

  if ( !is_dead( ch ) ) mprog_entry_trigger( ch );
  if ( !is_dead( ch ) ) mprog_greet_trigger( ch );

  /* ���ѰO�� */
  if ( !is_dead( ch ) && IS_NPC( ch ) ) mprog_enroll_trigger( ch );

  /* �����D���N���e�^�h */
  for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    if ( victim->fighting && ch == victim->fighting ) break;

  if ( victim )
  {
    act( "$A$1$n$1�����ܳQ$N$1�o�{�F!$0$A"
      , ch, NULL, victim, TO_CHAR );
    act( "$1$n�����������K, ���̵M�Q$N�o�{, ��H�j���X��!$0"
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
    mudlog( LOG_DEBUG, "find_door: �ʥF�ӷ�." );
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

    act( "�_�ǡM�ڦb$t���̨S���ݨ���󪺪��C�C",ch , arg, NULL, TO_CHAR );
    RETURN( ERRORCODE );
  }

  if ( !( pExit = ch->in_room->exit[door] ) )
  {
    act( "�ڦb$t��èS���ݨ���󪺪��C�C\n\r"
      , ch, direction_name( door ), NULL, TO_CHAR );
    RETURN( ERRORCODE );
  }

  if ( !pExit->isdoor )
  {
    send_to_char( "�������M�������O�ä��O���M�ҥH�A����o�˰��C\n\r", ch );
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
    /* ���~�O�_�����r */
    if ( poison_char( ch, obj ) ) RETURN_NULL();

    if ( obj->item_type != ITEM_CONTAINER )
    {
      act( "$p�ä��O�@�Ӯe���C", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( !IS_SET( obj->value[1], CONT_CLOSED ) )
    {
      act( "$p�w�g�Q���}�F�C", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( !IS_SET( obj->value[1], CONT_CLOSEABLE ) )
    {
      send_to_char( "�藍�_�M�A���਺�˰��C\n\r", ch );
      RETURN_NULL();
    }

    if ( IS_SET( obj->value[1], CONT_LOCKED ) )
    {
      act( "$p�O��W���C", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    REMOVE_BIT( obj->value[1], CONT_CLOSED );
    send_to_char( "�n�F�M���}�F�C\n\r", ch );
    act( "$n�N$p���}�F�C", ch, obj, NULL, TO_ROOM );
    message_driver( ch, obj, ACT_WHEN_OPEN );
    RETURN_NULL();
  }

  if ( ( door = find_door( ch, arg ) ) >= 0 )
  {
    pExit = ch->in_room->exit[door];

    if ( !pExit->closed )
    {
      send_to_char( "�����w�g�Q���}�F�C\n\r", ch );
      RETURN_NULL();
    }

    if ( pExit->locked )
    {
      send_to_char( "���O��W���C\n\r", ch );
      RETURN_NULL();
    }

    pExit->closed = FALSE;

    act( "$n���}$T�誺���C", ch, NULL, direction_name( door ), TO_ROOM );
    send_to_char( "�n�F�M�����}�F�C\n\r", ch );

    if ( ( to_room   = pExit->to_room               )
      && ( pExit_rev = to_room->exit[rev_dir[door]] )
      && pExit_rev->to_room == ch->in_room )
    {
      pExit_rev->closed = FALSE;

      for ( rch = to_room->people; rch; rch = rch->next_in_room )
        act( "$d���}�F�C", rch, NULL, pExit_rev->keyword, TO_CHAR );
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
    /* ���~�O�_�����r */
    if ( poison_char( ch, obj ) ) RETURN_NULL();

    if ( obj->item_type != ITEM_CONTAINER )
    {
      act( "$p�ä��O�@�Ӯe����C", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( IS_SET(obj->value[1], CONT_CLOSED) )
    {
      act( "$p�w�g�Q���_�ӤF�C", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( !IS_SET(obj->value[1], CONT_CLOSEABLE) )
    {
      act( "$p�������_�Ӫ��M�ҥH�A����o�˰��C", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    SET_BIT(obj->value[1], CONT_CLOSED);
    act( "�n�F�M�A�w�g��$p���_�ӤF�C", ch, obj, NULL, TO_CHAR );
    act( "$n��$p���_�ӡC", ch, obj, NULL, TO_ROOM );
    message_driver( ch, obj, ACT_WHEN_CLOSE );
    RETURN_NULL();
  }

  if ( ( door = find_door( ch, arg ) ) >= 0 )
  {
    pExit = ch->in_room->exit[door];
    if ( pExit->closed )
    {
      send_to_char( "���䪺���w�g�Q���_�ӤF�C\n\r", ch );
      RETURN_NULL();
    }

    pExit->closed = TRUE;

    act( "$n���F$T�誺���C", ch, NULL, direction_name( door ), TO_ROOM );
    send_to_char( "�n�F�M�A�⨺�䪺�����_�ӤF�C\n\r", ch );

    if ( ( to_room   = pExit->to_room )
      && ( pExit_rev = to_room->exit[rev_dir[door]] ) != 0
      && pExit_rev->to_room == ch->in_room )
    {
      pExit->closed = TRUE;

      for ( rch = to_room->people; rch; rch = rch->next_in_room )
        act( "$d���_�ӡC", rch, NULL, pExit_rev->keyword, TO_CHAR );
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
    /* ���~�O�_�����r */
    if ( poison_char( ch, obj ) ) RETURN_NULL();

    if ( obj->item_type != ITEM_CONTAINER )
    {
      act( "$p�ä��O�@�Ӯe���C", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( !IS_SET(obj->value[1], CONT_CLOSED) )
    {
      act( "$p�èS���Q��_�ӡC", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( obj->value[2] < 0 )
    {
      act( "�A�����$p��_�ӡC", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( !has_key( ch, obj->value[2] ) )
    {
      send_to_char( "�A�S�������_�͡C\n\r", ch );
      RETURN_NULL();
    }

    if ( IS_SET(obj->value[1], CONT_LOCKED) )
    {
      act( "$p�w�g�Q��_�ӤF�C", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    SET_BIT(obj->value[1], CONT_LOCKED);
    act( "�ذդ@�n�M�A�w�g��$p��_�ӤF�C", ch, obj, NULL, TO_CHAR );
    act( "$n��$p��_�ӤF�C", ch, obj, NULL, TO_ROOM );
    message_driver( ch, obj, ACT_WHEN_LOCK );
    RETURN_NULL();
  }

  if ( ( door = find_door( ch, arg ) ) >= 0 )
  {
    pExit = ch->in_room->exit[door];

    if ( !pExit->closed )
    {
      send_to_char( "�A�n���⥦���_�Ӥ~����C\n\r", ch );
      RETURN_NULL();
    }

    if ( pExit->key < 0 )
    {
      send_to_char( "�A����⥦��_�ӡC\n\r", ch );
      RETURN_NULL();
    }

    if ( !has_key( ch, pExit->key ) )
    {
      send_to_char( "�A�S�������_�͡C\n\r" , ch );
      RETURN_NULL();
    }

    if ( pExit->locked )
    {
      send_to_char( "���w�g�Q��_�ӤF�C\n\r", ch );
      RETURN_NULL();
    }

    pExit->locked = TRUE;

    send_to_char( "�ذդ@�n�M�A�����_�ӤF�C\n\r", ch );
    act( "$n��$T�誺����_�ӤF�C", ch, NULL, direction_name( door ), TO_ROOM );

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
    /* ���~�O�_�����r */
    if ( poison_char( ch, obj ) ) RETURN_NULL();

    if ( obj->item_type != ITEM_CONTAINER )
    {
      act( "$p�ä��O�@�Ӯe���C", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( !IS_SET( obj->value[1], CONT_CLOSED) )
    {
      act( "$p�èS���Q���_�ӡC", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( obj->value[2] < 0 )
    {
      act( "�A�L�k��$p�}��C", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( !has_key( ch, obj->value[2] ) )
    {
      send_to_char( "�A�S�������_�͡C\n\r", ch );
      RETURN_NULL();
    }

    if ( !IS_SET(obj->value[1], CONT_LOCKED) )
    {
      act( "$p����w�g�Q���}�F�C", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    REMOVE_BIT(obj->value[1], CONT_LOCKED);
    act( "�ذդ@�n�M�A��$p���ꥴ�}�F�C", ch, obj, NULL, TO_CHAR );
    act( "$n��$p���ꥴ�}�F�C", ch, obj, NULL, TO_ROOM );
    message_driver( ch, obj, ACT_WHEN_UNLOCK );
    RETURN_NULL();
  }

  if ( ( door = find_door( ch, arg ) ) >= 0 )
  {
    pExit = ch->in_room->exit[door];

    if ( !pExit->closed )
    {
      send_to_char( "���䪺���èS���Q���_�ӡC\n\r", ch );
      RETURN_NULL();
    }

    if ( pExit->key < 0 )
    {
      send_to_char( "���䪺���ä���}��C\n\r", ch );
      RETURN_NULL();
    }

    if ( !has_key( ch, pExit->key) )
    {
      send_to_char( "�A�S�������_�͡C\n\r", ch );
      RETURN_NULL();
    }

    if ( !pExit->locked )
    {
      send_to_char( "���������w�g�Q���}�F�C\n\r",  ch );
      RETURN_NULL();
    }

    pExit->locked = FALSE;

    send_to_char( "�ذդ@�n�M�A�⨺������ꥴ�}�F�C\n\r", ch );
    act( "$n��$T������ꥴ�}�F�C", ch, NULL, direction_name( door ), TO_ROOM );

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
      send_to_char( "�A�L�k���L�ӡT\n\r", ch );
      RETURN_NULL();
    }

    send_to_char( "�A�M���ï��F�_�ӡC\n\r", ch );
    act( "$n�M���ï��F�_�ӡC", ch, NULL, NULL, TO_ROOM );
    ch->position = POS_STANDING;
    if ( mount ) do_stand( mount, "" );
    break;

  case POS_RESTING:
    send_to_char( "�A���F�_�ӡC\n\r", ch );
    act( "$n���F�_�ӡC", ch, NULL, NULL, TO_ROOM );
    ch->position = POS_STANDING;
    if ( mount ) do_stand( mount, "" );
    break;

  case POS_STANDING:
    send_to_char( "�A�w�g���F�_�ӡC\n\r", ch );
    break;

  case POS_FIGHTING:
    send_to_char( "�A�٦b�԰��T\n\r", ch );
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
    send_to_char( "�A�٦b���H�M����𮧡C\n\r" , ch );
    RETURN_NULL();
  }

  if ( is_pk( ch ) )
  {
    send_to_char( "������ɭԤF�M�٦b�𮧡M���֥h��ĤH�������a�T\n\r", ch );
    RETURN_NULL();
  }

  switch ( ch->position )
  {
  case POS_SLEEPING:
    send_to_char( "�A�w�g�b��ı�F�C\n\r", ch );
    break;

  case POS_RESTING:
    send_to_char( "�A�w�g�b�𮧤F�C\n\r", ch );
    break;

  case POS_STANDING:

    if ( ch->in_room && ch->club && ch->club == ch->in_room->club )
    {
      send_to_char( "�A���b�o�̥𮧡M�S�Oı�o�w���T\n\r", ch );
    }
    else
    {
      send_to_char( "�A���U�ӥ𮧡C\n\r", ch );
    }

    act( "$n ���U�ӥ𮧡C", ch, NULL, NULL, TO_ROOM );
    ch->position = POS_RESTING;
    break;

  case POS_FIGHTING:
    send_to_char( "�藍�_�M�A�٦b�԰��T\n\r", ch );
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
    act( "�A�������q$N�W�U�Ӥ~���ı�C",ch , NULL, mount, TO_CHAR );
    RETURN_NULL();
  }

  if ( ( mount = get_rider( ch ) ) )
  {
    send_to_char( "�A�٦b���H�M�����ı�C\n\r" , ch );
    RETURN_NULL();
  }

  if ( is_pk( ch ) )
  {
    send_to_char( "������ɭԤF�M�٦b��ı�M���֥h��ĤH�������a�T\n\r", ch );
    RETURN_NULL();
  }

  switch ( ch->position )
  {
  case POS_SLEEPING:
    send_to_char( "�A�w�g�Ϊ������H�ƤF�C\n\r", ch );
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
      act( "$n�I�i$w����M�B��󤦥СM���ؾi���T", ch, pSkill, NULL, TO_ALL );
      ch->position = POS_SLEEPING;
    }

    else
    {
      if ( ch->in_room && ch->club && ch->club == ch->in_room->club )
      {
        send_to_char( "�A���b�o�̺�ı�M�S�Oı�o�����T\n\r", ch );
      }
      else
      {
        send_to_char( "�A���U�Ӻ�ı�C\n\r", ch );
      }

      act( "$n���U�Ӻ�ı�C", ch, NULL, NULL, TO_ROOM );
      ch->position = POS_SLEEPING;
    }

    break;

  case POS_FIGHTING:
    send_to_char( "�A�٦b�԰��T\n\r", ch );
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
      send_to_char( "�A�n���_�֪��Ȧs�Ҧ��S\n\r", ch );
      RETURN_NULL();
    }

    if ( !( victim = get_char_room( ch, arg + 1 ) ) )
    {
      act( "�䤣��A�n���_����H $2$T$0�C", ch, NULL, arg+1, TO_CHAR );
      RETURN_NULL();
    }

    if ( IS_NPC( victim ) || !victim->desc )
    {
      act( "�L�k���_$N���Ȧs�Ҧ��T", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( !set_descriptor_stack( victim->desc ) && !is_edit( victim->desc ) )
    {
      act( "�藍�_�M$N�ثe���O�b�Ȧs�άO�s��Ҧ��T", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    victim->desc->stack = FALSE;
    cease_stack( victim->desc );
    act( "�A�w�g���_$N���Ȧs�Ҧ��F�T", ch, NULL, victim, TO_CHAR );

    send_to_char( "\n\r                        ---�\\Ū�άO�s��Ҧ��Q���_---"
      "\a\a\n\r", victim );

    RETURN_NULL();
  }

  if ( !IS_AWAKE( ch ) )
  {
    send_to_char( "�A���ۤv�εۤF�T\n\r", ch );
    RETURN_NULL();
  }

  if ( !( victim = get_char_room( ch, arg ) ) )
  {
    act( "�䤣��A����H $2$T$0�C", ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  if ( IS_AWAKE( victim ) )
  {
    act( "$N�w�g���L�ӤF�C", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( is_affected( victim, SLOT_SLEEP ) )
  {
    act( "�A����n��$N�T", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  act( "�A�n��$N�T", ch, NULL, victim, TO_CHAR );
  act( "$n�n���A�T", ch, NULL, victim, TO_VICT );
  victim->position = POS_STANDING;

  /* �y�M�]�n�_�� */
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

  send_to_char( "�b�@�ε��������M�A�{�X�A����ΡC\n\r", ch );
  act( "$n�b�@�}���������{�X�L����ΡC", ch, NULL, NULL, TO_ROOM );
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

  /* �Ǫ��S���a */
  if ( IS_NPC( ch ) ) RETURN_NULL();

  if ( ch->mount || ch->mount_by )
  {
    send_to_char( "�A�����M�C�M���U�ӧa�C\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->spirit )
  {
    if ( ch->spirit->in_room
      && ch->in_room
      && ch->spirit->in_room == ch->in_room )
    {
      act( "�A����A��$N���_�ӧa�M���M$E�O�䤣��A���T"
        , ch, NULL, ch->spirit, TO_CHAR );
      RETURN_NULL();
    }
  }

  if ( is_pk( ch ) )
  {
    send_to_char( "�A���b�Z���j�|�̡M�S����k�ǰe�A�C�T\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->failed > 0 )
  {
    send_to_char( "�藍�_�M�A�٦b��L���M�L�k�ǰe�T\n\r", ch );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg );

  /* �p�G�S���Ѽ�, �N�O�w�w�I */
  if ( !arg[0] )
  {
    act( "$n��ë�ѯ��ǰe�^�a�C", ch, NULL, NULL, TO_ROOM );

    if ( !( location = get_hometown( ch ) ) )
    {
      send_to_char( "�A�������ѤF�M�ѯ����F�A�C\n\r", ch );
      RETURN_NULL();
    }
  }

  else if ( is_number( arg ) )
  {
    if ( !ch->pcdata ) RETURN_NULL();

    if ( ( mem = atoi( arg ) ) <= 0 || mem > MAX_RECALL )
    {
      send_to_char( "�A���ǰe�I���d�򤣥��T�C\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->pcdata->recall[mem-1] == 0
      || !( location = get_room_index( ch->pcdata->recall[mem-1] ) ) )
    {
      send_to_char( "�A�S���]�w�o�Ӷǰe�I��C\n\r", ch );
      RETURN_NULL();
    }

    if ( !location->area || !ch->in_room->area )
    {
      send_to_char( "�o�Ӧa����G�w�g���s�b�F�C\n\r", ch );
      RETURN_NULL();
    }

    /* �P�@�ϰ즩������� */
    lose = ( location->area == ch->in_room->area ) ? 50 : 150;

    if ( ch->move < lose )
    {
      send_to_char( "�A����O���١M�L�k�ǰe�C\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->in_room == location
      || ch->in_room->NoRecall
      || is_affected( ch, SLOT_CURSE ) )
    {
      send_to_char( "�A���n���O�A����O�a�T\n\r", ch );
      RETURN_NULL();
    }

    ch->move -= lose;
    bSub      = FALSE;
    act( "�A���զ^��$t�o�Ӧa��C",ch ,location->name, NULL, TO_CHAR );
  }

  /* club �ѼƬO�^�������E���I */
  else if ( !str_prefix( arg, "club" ) )
  {
    if ( ch->jail > 0 )
    {
      send_to_char( "�藍�_�M�A�٦b���c�M�L�k�ǰe�T\n\r", ch );
      RETURN_NULL();
    }

    if ( is_pk( ch ) )
    {
      send_to_char( "�٬O�M�ߧA���Z���j�|�a�M�O�Q�F�Q�誺�T\n\r", ch );
      RETURN_NULL();
    }

    if ( !ch->club )
    {
      send_to_char( "�A�S���ѥ[���������C\n\r", ch );
      RETURN_NULL();
    }

    act( "$n��ë�ǰe�^�����E���I�C", ch, NULL, NULL, TO_ROOM );

    if ( !( location = club_recall( ch ) ) )
    {
      send_to_char( "�䤣��A�������E���I�C\n\r", ch );
      RETURN_NULL();
    }
  }

  /* �C�X�O�Ъ��a�� */
  else if ( !str_prefix( arg, "list" ) && ch->pcdata )
  {
    clear_buffer();
    for ( loop = 0; loop < MAX_RECALL; loop++ )
    {
      location = ch->pcdata->recall[loop] > 0
               ? get_room_index( ch->pcdata->recall[loop] ) : NULL;

      send_to_buffer( "�� %2d �ǰe�I %s %s\n\r"
        , loop + 1, location ? location->name : "�|���]�w"
        , location && location->area ? location->area->name : "" );

      if ( buffer_full() ) break;
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  /* �O�� */
  else if ( !str_prefix( arg, "set" ) && ch->pcdata )
  {
    one_argument( argument, arg );

    if ( !arg[0] || !is_number( arg ) )
    {
      send_to_char( "�y�k���~�M�Ьd�� recall �Ϊk�C\n\r", ch );
      RETURN_NULL();
    }

    if ( ( mem = atoi( arg ) ) <= 0 || mem > MAX_RECALL )
    {
      chinese_number( MAX_RECALL, buf );
      act( "�ǰe�I���d��b�@��$t�C", ch, buf, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( !ch->in_room || ch->in_room->Memorize == FALSE )
    {
      send_to_char( "�o�Ӧa��ݰ_�ӬO�O������C\n\r", ch );
      RETURN_NULL();
    }

    for ( loop = 0; loop < MAX_RECALL; loop++ )
    {
      if ( ch->pcdata->recall[loop] == ch->in_room->vnum )
      {
        act( "$r���e�N�w�g�O�L�F�C", ch, NULL, NULL, TO_CHAR );
        RETURN_NULL();
      }
    }

    act( "�A�H���a��@��$i�Ӷǰe�I�C", ch, &mem, NULL, TO_CHAR );
    ch->pcdata->recall[mem-1] = ch->in_room->vnum;

    RETURN_NULL();
  }

  else if ( !str_cmp( arg, "cancel" ) && ch->pcdata )
  {
    one_argument( argument, arg );

    if ( !arg[0] || !is_number( arg ) )
    {
      send_to_char( "�y�k���~�M�Ьd�� recall �Ϊk�C\n\r", ch );
      RETURN_NULL();
    }

    if ( ( mem = atoi( arg ) ) <= 0 || mem > MAX_RECALL )
    {
      chinese_number( MAX_RECALL, buf );
      act( "�ǰe�I���d��b�@��$t�C", ch, buf, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch->pcdata->recall[mem-1] > 0 )
    {
      act( "�A�����F��$i�I���ǰe�I�C", ch, &mem, NULL, TO_CHAR );
      ch->pcdata->recall[mem-1] = 0;
    }
    else
    {
      send_to_char( "�o�Ӷǰe�I���ӴN���s�b�C\n\r", ch );
    }

    RETURN_NULL();
  }

  else
  {
    send_to_char( "�Ѽƿ��~�M�Ьd�� recall �o�ӫ��O���Ϊk�C\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->in_room == location )
  {
    send_to_char( "�ѰڡT����Ƥ]�S���o�͡C\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->in_room->NoRecall || is_affected( ch, SLOT_CURSE ) )
  {
    send_to_char( "�����F�A�C\n\r", ch );
    RETURN_NULL();
  }

  if ( ( victim = ch->fighting ) )
  {
    if ( number_bits( 1 ) == 0 )
    {
      WAIT_STATE( ch, 4 );
      lose = ( ch->desc ) ? 50 : 100;
      gain_exp( ch, 0 - lose );
      act( "�A���ѤF�T�l���F\e[1;5;31m$i$0�g��ȡC", ch, &lose, NULL, TO_CHAR );
      RETURN_NULL();
    }

    lose = IS_NPC( ch ) ? 200 : 100;
    gain_exp( ch, 0 - lose );
    act( "�A���\\��k�T�l���F\e[1;5;31m$i$0�g��ȡC", ch, &lose, NULL, TO_CHAR );
    stop_fighting( ch, TRUE );
  }

  if ( bSub ) ch->move /= 2;

  act( "�u���@�D�{�q���L�M$n�������L�v�L�ܡC", ch, NULL, NULL, TO_ROOM );
  char_from_room( ch );
  char_to_room( ch, location );
  act( "�@�}�p�q��[��M$n��M�X�{�b�o�̡C", ch, NULL, NULL, TO_ROOM );
  do_look( ch, "auto" );
  clear_trace( ch, TRUE );

  if ( ch->spirit ) act( "�A�M�A��$N�g���F�T", ch, NULL, ch->spirit, TO_CHAR );

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
    send_to_char( "�A���^�����A�����D�a�T\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->position != POS_STANDING )
  {
    send_to_char( "�A�ثe�����A���A�X�m�߳�T\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->in_room == RoomDead || ch->in_room == RoomCorpse )
  {
    act( "�藍�_�M�b$r�O���i�H�m�\\���T", ch, NULL, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( arg[0] == '\x0' )
  {
    send_to_char( "�A�n�m�ߤ���ޯ�O�S\n\r", ch );
    RETURN_NULL();
  }

  if ( !( pSkill = skill_isname( arg ) )
    || ch->skill[ ( slot = URANGE( 0,pSkill->slot, MAX_SKILL - 1 ) )] <= 0 )
  {
    send_to_char( "�A���|�o���ޯ�a�T\n\r", ch );
    RETURN_NULL();
  }

  if ( pSkill->ply <= 0 )
  {
    act( "$w�O����m�߳�T", ch, pSkill, NULL, TO_CHAR );
    RETURN_NULL();
  }

  for ( rch = ch->in_room->people; rch; rch = rch->next_in_room )
  {
    if ( rch->position == POS_FIGHTING )
    {
      act( "�A���񦳤H�b�����M�ɭP�A�L�k�M�߽m��$w�T", ch, pSkill, NULL, TO_CHAR );
      RETURN_NULL();
    }
  }

  if ( ch->skill[slot] >= ( ch->level / UMAX( 1, pSkill->qutoient ) ) )
  {
    act( "�A�w�g�L�k�A�~��m$w�M�A���ӥh�_�I�F�T", ch, pSkill, NULL, TO_CHAR );
    RETURN_NULL();
  }

  /* �O�_�W�X¾�~������ */
  for ( pFind = FALSE, pLimit = pSkill->limit; pLimit; pLimit = pLimit->next )
  {
    if ( pLimit->class == ch->class )
    {
      pFind = TRUE;

      if ( ch->level < pLimit->level || ch->skill[slot] >= pLimit->adept )
      {
        act( "�A�o��$w�L�k�b����U�h�F�T", ch, pSkill, NULL, TO_CHAR );
        RETURN_NULL();
      }
      break;
    }
  }

  /* �Y�O�䤣��]����ɼ��m�� */
  if ( pFind == FALSE )
  {
    act( "�A�o��$w�L�k�b����U�h�F�T", ch, pSkill, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( pSkill->check && ( ( *pSkill->check ) ( ch, TRUE ) ) == FALSE )
    RETURN_NULL();

  cost = pSkill->cost;

  /* �ˬd�O�_���������k�O�άO��O�άO��L....*/
  switch( pSkill->cost_type )
  {
  default :
    mudlog( LOG_DEBUG , "do_cast: �k�N %s ���ӷl���A���~", pSkill->name );
    RETURN_NULL();

  /* ���ӥͩR�O */
  case COST_HIT:

    if ( ch->hit < cost )
    {
      act( "�A���ͩR�O�ӧC�M�L�k�m��$w�C", ch, pSkill, NULL, TO_CHAR );
      RETURN_NULL();
    }

    ch->hit -= cost;
    break;

  /* ���Ӫk�O */
  case COST_MANA:

    if ( ch->mana < cost )
    {
      act( "�A�S���������k�O�ӽm��$w�C", ch, pSkill, NULL, TO_CHAR );
      RETURN_NULL();
    }

    ch->mana -= cost;
    break;

  /* ������O */
  case COST_MOVE:

    if ( ch->move < cost )
    {
      act( "�A����O���١M�L�k�m��$w�C", ch, pSkill, NULL, TO_CHAR );
      RETURN_NULL();
    }

    ch->move -= cost;
    break;

  /* ���Ӫ��� */
  case COST_GOLD:

    if ( ch->gold < cost )
    {
      act( "�A�S���������Ȩ�M�L�k�m��$w�C", ch, pSkill, NULL, TO_CHAR );
      RETURN_NULL();
    }

    gold_from_char( ch, cost );
    break;
  }

  if ( ch->pcdata && !IS_IMMORTAL( ch ) ) ch->pcdata->ply++;

  /* �k�N�άO�ޯ઺���ݮɶ� */
  if ( !IS_IMMORTAL( ch ) ) WAIT_STATE( ch, pSkill->wait );

  act( "$n�R�R���b�@�ǡM�����a�m�_$w�C", ch, pSkill, NULL, TO_ALL );

  /* �]�w���v */
  ch->skill[slot] = UMIN( ch->skill[slot], 100 );
  chance_base     = 10000 + ( 200 * ( ch->skill[slot] + ch->level ) / 2 );
  chance          = get_curr_wis( ch ) + pSkill->ply;

  if ( number_range( 1, chance_base ) < chance )
  {
    act( "$A$A$3���H�A�M�ѩ�A���ԾĽm�ߡM�A��$w���m�׼W�[�F�T$0"
      , ch, pSkill, NULL, TO_CHAR );

    ch->skill[slot]++;
  }

  RETURN_NULL();
}

/* �W�[�ݩ� */
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

  /* �D���a����ɯ��ݩ� */
  if ( IS_NPC( ch ) ) RETURN_NULL();

  argument = one_argument( argument , arg );

  /* ��J�r�ꬰ�Ŧr�� */
  if ( !arg[0] )
  {
    print_to_char( ch , "�A�ثe���ǲ��I�Ʀ@�� \e[1;37m%d\e[0m �I�C\n\r"
      "�A�ثe�O�q(\e[1;33mstr\e[0m)�I�Ƭ� %3d �M"
      "�ɤU�@�Żݭn %s%6d\e[0m �I�ơC\n\r"
      "�A�ثe���O(\e[1;33mint\e[0m)�I�Ƭ� %3d �M"
      "�ɤU�@�Żݭn %s%6d\e[0m �I�ơC\n\r"
      "�A�ثe����(\e[1;33mwis\e[0m)�I�Ƭ� %3d �M"
      "�ɤU�@�Żݭn %s%6d\e[0m �I�ơC\n\r"
      "�A�ثe�ӱ�(\e[1;33mdex\e[0m)�I�Ƭ� %3d �M"
      "�ɤU�@�Żݭn %s%6d\e[0m �I�ơC\n\r"
      "�A�ثe���(\e[1;33mcon\e[0m)�I�Ƭ� %3d �M"
      "�ɤU�@�Żݭn %s%6d\e[0m �I�ơC\n\r"
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
    send_to_buffer( "\e[1;33;44m���� �ϰ�W��   �P�� ����W��      "
      "                     �O�q ���z ���� �ӱ� ���\e[0m\n\r" );

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
          , area ? area->name : "�q��"
          , YESNO( area && ch->in_room && ch->in_room->area == area )
          , buf
          , YESNO( get_curr_str( mob ) > ch->now_str + 10 )
          , YESNO( get_curr_int( mob ) > ch->now_int + 10 )
          , YESNO( get_curr_wis( mob ) > ch->now_wis + 10 )
          , YESNO( get_curr_dex( mob ) > ch->now_dex + 10 )
          , YESNO( get_curr_con( mob ) > ch->now_con + 10 ) );
      }
    }

    if ( count == 0 ) send_to_char( "�t�θ̨S��������¾�̡T\n\r", ch );
    else print_buffer( ch );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "!maxinum" ) )
  {
    if ( !( pClass = ch->class ) )
    {
      send_to_char( "�藍�_�M�A��¾�~�����M�L�k�d�ߡT\n\r", ch );
      RETURN_NULL();
    }

    clear_buffer();
    send_to_buffer( "�A��¾�~��%s�M�U���ݩʪ�����R\n\r"
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

      send_to_buffer( "[%s%s\e[0m] %s�ݩʪ��W���R%3d�u�ثe�ƭȡR%3d�v\n\r"
        , pClass->attr[count] > value ? "\e[1;32m" : "\e[1;31m"
        , YESNO( pClass->attr[count] > value )
        , attr_name[count]
        , pClass->attr[count]
        , value );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  /* IMM ����ɯ��ݩ� */
  if ( IS_IMMORTAL( ch ) )
  {
    send_to_char( "�A�O�t�κ޲z���M���i�H�ɯ��ݩʡC\n\r" , ch );
    RETURN_NULL();
  }

  /* �ݬݬO�_������V�m�� */
  if ( !( mob = get_char_room( ch , arg ) ) )
  {
    send_to_char( "�o�̨S�����ӰV�m�̡M�٬O�V�O�H��ť�ݬݧa�C\n\r" , ch );
    RETURN_NULL();
  }

  if ( mob == ch )
  {
    send_to_char( "�A���i�H�ۤv�V�m�ۤv�M�}���򪱯��S\n\r" , ch );
    RETURN_NULL();
  }

  if ( !( pClass = ch->class ) )
  {
    send_to_char( "�A��¾�~�����M�L�k���A�V�m�C\n\r", ch );
    RETURN_NULL();
  }

  one_argument( argument , arg );

  if ( !arg[0] )
  {
    send_to_char( "�A���y�k���~�M�Ьd�� train�C\n\r" , ch );
    RETURN_NULL();
  }

  /* �O�_�@�N���O�H�ɯ��ݩ�, �D���a���� */
  if ( IS_NPC( mob ) )
  {
    if ( !IS_SET( mob->act, ACT_TRAIN ) )
    {
      sprintf( buf , "%s �藍�_�M�ڤ������A�V�m�T" , ch->name );
      do_tell( mob , buf );
      RETURN_NULL();
    }
  }

  /* �O�_�@�N���O�H�ɯ��ݩ�, ���a���� */
  else
  {
    if ( !IS_SET( mob->turn , PLR_TRAIN ) )
    {
      sprintf( buf , "%s �藍�_�M�ڤ������A�V�m�T" , ch->name );
      do_tell( mob , buf );
      RETURN_NULL();
    }
  }

  /* �Q�n�V�m�O�q */
  if ( !str_cmp( arg , "str" ) )
  {
    if ( get_curr_str( ch ) >= MaxStr )
    {
      send_to_char( "�藍�_�M�A���O�q�w�F�t�Ϊ��̤j�ȡT\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->now_str >= pClass->attr[STR_ITEM] )
    {
      send_to_char( "�藍�_�M�A���O�q�w�g����A���ɤF�T\n\r", ch );
      RETURN_NULL();
    }

    CharAbility = &ch->now_str;
    MobAbility  = &mob->now_str;
    cost        = get_str_train( ch );
    pType       = STR_ITEM;
    limit       = get_curr_str( ch ) >= pClass->attr[pType] ? TRUE : FALSE;
  }

  /* �������z */
  else if ( !str_cmp( arg , "int" ) )
  {
    if ( get_curr_int( ch ) >= MaxInt )
    {
      send_to_char( "�藍�_�M�A�����z�w�F�t�Ϊ��̤j�ȡT\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->now_int >= pClass->attr[INT_ITEM] )
    {
      send_to_char( "�藍�_�M�A�����O�w�g����A���ɤF�T\n\r", ch );
      RETURN_NULL();
    }

    CharAbility = &ch->now_int;
    MobAbility  = &mob->now_int;
    cost        = get_int_train( ch );
    pType       = INT_ITEM;
    limit       = get_curr_int( ch ) >= pClass->attr[pType] ? TRUE : FALSE;
  }

  /* �Q�n�V�m���� */
  else if ( !str_cmp( arg , "wis" ) )
  {
    if ( get_curr_wis( ch ) >= MaxWis )
    {
      send_to_char( "�藍�_�M�A�����Ѥw�F�t�Ϊ��̤j�ȡT\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->now_wis >= pClass->attr[WIS_ITEM] )
    {
      send_to_char( "�藍�_�M�A�����Ѥw�g����A���ɤF�T\n\r", ch );
      RETURN_NULL();
    }

    CharAbility = &ch->now_wis;
    MobAbility  = &mob->now_wis;
    cost        = get_wis_train( ch );
    pType       = WIS_ITEM;
    limit       = get_curr_wis( ch ) >= pClass->attr[pType] ? TRUE : FALSE;
  }

  /* �Q�n�V�m�ӱ� */
  else if ( !str_cmp( arg , "dex" ) )
  {
    if ( get_curr_dex( ch ) >= MaxDex )
    {
      send_to_char( "�藍�_�M�A���ӱ��w�F�t�Ϊ��̤j�ȡT\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->now_dex >= pClass->attr[DEX_ITEM] )
    {
      send_to_char( "�藍�_�M�A���ӱ��w�g����A���ɤF�T\n\r", ch );
      RETURN_NULL();
    }

    CharAbility = &ch->now_dex;
    MobAbility  = &mob->now_dex;
    cost        = get_dex_train( ch );
    pType       = DEX_ITEM;
    limit       = get_curr_dex( ch ) >= pClass->attr[pType] ? TRUE : FALSE;
  }

  /* �Q�n�V�m��� */
  else if ( !str_cmp( arg , "con" ) )
  {
    if ( get_curr_con( ch ) >= MaxCon )
    {
      send_to_char( "�藍�_�M�A�����w�F�t�Ϊ��̤j�ȡT\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->now_con >= pClass->attr[CON_ITEM] )
    {
      send_to_char( "�藍�_�M�A�����w�g����A���ɤF�T\n\r", ch );
      RETURN_NULL();
    }

    CharAbility =  &ch->now_con;
    MobAbility  = &mob->now_con;
    cost        = get_con_train( ch );
    pType       = CON_ITEM;
    limit       = get_curr_con( ch ) >= pClass->attr[pType] ? TRUE : FALSE;
  }

  /* �����O�䤤������r */
  else
  {
    send_to_char( "�A���y�k���~�M�Ьd�� train�C\n\r" , ch );
    RETURN_NULL();
  }

  /* �ǲ��I�Ƥ������ɭ� */
  if ( ch->practice < cost )
  {
    sprintf( buf , "%s ���A���ǲ��I�Ʀh�@�I�ɭԡM�A�ӧa�T" , ch->name );
    do_tell( mob , buf );
    RETURN_NULL();
  }

  /* �ݬݵ��Ť������t�O�άO¾�~���ѭ���ӨM�w�O�_���\ */
  if ( number_range( 1 , *MobAbility - *CharAbility ) < 2  || limit )
  {
    sprintf( buf , "%s �藍�_�M���A�նO�W�ߤF�M�A��%s�èS���i�B�T"
      , ch->name , attr_name[ pType ] );
    do_tell( mob , buf );
    ch->practice = UMAX( 0 , ch->practice - ( *CharAbility * 5 ) );
    RETURN_NULL();
  }

  /* �e�X�r�� */
  sprintf( buf, "�b$N���Ӥ߱оɤ��U�M�A��%s���G�y���i�B�C", attr_name[pType] );
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

  /* �D���a�P�S��¾�~�̤��వ�o�Ӱʧ@ */
  if ( IS_NPC( ch ) || !ch->class ) RETURN_NULL();

  argument = one_argument( argument , arg );

  /* �S������ѼƴN�O�d�ߦۤv���ޯ� */
  if ( !arg[0] )
  {
    clear_buffer();
    send_to_buffer( "�A�ҷ|���ޯ঳ %d �ӡM�٥i�H�� %d �ӧޯ�R\n\r"
      , learn_count( ch )
      , UMAX( 0, ( get_learnable( ch ) - learn_count( ch ) ) ) );

    for ( pSkill = skill_list; pSkill; pSkill = pSkill->next )
    {
      if ( ( adept = ch->skill[pSkill->slot] ) > 0 )
      {
        CanInc = FALSE;

        /* �O�_�W�X¾�~������ */
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

             if ( pSkill->type == TAR_DODGE  ) send_to_buffer( " ���m���ޯ�" );
        else if ( pSkill->type == TAR_FLEE   ) send_to_buffer( " �k�]���ޯ�" );
        else if ( pSkill->rating == RATING_SKILL )
          send_to_buffer( " %s���ޯ�" , weapon_type_name( pSkill->weapon ) );

        else send_to_buffer( " %s���k�N"
            , pSkill->rating ? skill_rating( pSkill->rating ) : "����" );

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
      send_to_char( "�A���������Ǫ����������X�T\n\r", ch );
      RETURN_NULL();
    }

    sn = atoi( arg + 1 );
    if ( !( pIndex = get_mob_index( sn ) ) )
    {
      act( "�䤣��s�� $i �������Ǫ��T", ch, &sn, NULL, TO_CHAR );
      RETURN_NULL();
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\x0' )
    {
      send_to_char( "�A���������оɪ��ޯ�W�١T\n\r", ch );
      RETURN_NULL();
    }

    if ( !( pSkill = skill_isname( arg ) ) )
    {
      act( "�䤣�� $t �o�اޯ�W�١T", ch, arg, NULL, TO_CHAR );
      RETURN_NULL();
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\x0' || !is_number( arg ) )
    {
      act( "�A��������$w�����m�סT", ch, pSkill, NULL, TO_CHAR );
      RETURN_NULL();
    }

    adept = atoi( arg );
    if ( adept < 0 || adept > 100 )
    {
      act( "$w�ޯ઺���m�� $x ���X�z�T", ch, pSkill, &adept, TO_CHAR );
      RETURN_NULL();
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\x0' || !is_number( arg ) )
    {
      act( "�A��������$w���оɻ����T", ch, pSkill, NULL, TO_CHAR );
      RETURN_NULL();
    }

    cost = atoi( arg );
    if ( cost < 0 )
    {
      act( "$w�ޯ઺�оɪ� $x ���X�z�T", ch, pSkill, &cost, TO_CHAR );
      RETURN_NULL();
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\x0' || !is_number( arg ) )
    {
      act( "�A��������$w�����õ{�סT", ch, pSkill, NULL, TO_CHAR );
      RETURN_NULL();
    }

    inventory = atoi( arg );
    if ( inventory < 0 || inventory > 100 )
    {
      act( "$w�ޯ઺���õ{�� $x ���X�z�T", ch, pSkill, &inventory, TO_CHAR );
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

          act( "�A�����F$t��$W�ޯ�оɡT"
            , ch, mob_index_name( NULL, pIndex ), pSkill, TO_CHAR );

          top_teach--;
          RETURN_NULL();
        }
      }

      act( "$t���ӴN�S���о�$W�o�اޯ�T"
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

          sprintf( buf, "�A��%s��$w�оɸ�Ƨאּ�R$A�бª��R%d ��$A���m�סR"
            "%d%%$A���ëסR%d%%"
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

      sprintf( buf, "�A��%s��$w�оɸ�Ƴ]�w�R$A�бª��R%d ��$A���m�סR"
         "%d%%$A���ëסR%d%%"
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
      act( "�п�J�n�d�ߪ��ޯ�����T", ch, NULL, NULL, TO_CHAR );
      RETURN_NULL();
    }

    switch( arg[1] )
    {
    default:
      act( "�S���o�������Z����T", ch, NULL, NULL, TO_CHAR );
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
      act( "�S���A�ҿ�J���ޯ������T", ch, NULL, NULL, TO_CHAR );
      RETURN_NULL();
    }

    clear_buffer();
    send_to_buffer( "�A�ҷ|��%s���ޯ঳�R\n\r"
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

             if ( pSkill->type == TAR_DODGE ) send_to_buffer( " ���m���ޯ�" );
        else if ( pSkill->type == TAR_FLEE  ) send_to_buffer( " �k�]���ޯ�" );
        else send_to_buffer( " %s���ޯ�", weapon_type_name( pSkill->weapon ) );
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
      send_to_char( "�A�n�d�ߨ��@�����ޯ�O�S\n\r", ch );
      RETURN_NULL();
    }

    if ( ( pRating = check_skill_rating( arg + 1 ) ) <= 0 )
    {
      act( "�S���o���ޯ� $2$T$0 �աT", ch, NULL, arg +1, TO_CHAR );
      RETURN_NULL();
    }

    if ( learn_rating_count( ch, pRating ) <= 0 )
    {
      send_to_char( "�A���|�o�@�����ޯ�T\n\r" , ch );
      RETURN_NULL();
    }

    clear_buffer();
    send_to_buffer( "�A�ҷ|��%s���ޯ঳ %d �ӡR\n\r"
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

             if ( pSkill->type == TAR_DODGE ) send_to_buffer( " ���m���ޯ�" );
        else if ( pSkill->type == TAR_FLEE  ) send_to_buffer( " �k�]���ޯ�" );
        else if ( pRating == RATING_SKILL   ) send_to_buffer( " %s���ޯ�"
            , weapon_type_name( pSkill->weapon ) );
        else send_to_buffer( " %s�k�N", skill_rating( pRating ) );
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
      act( "�䤣��A����H $2$T$0�C", ch, NULL, arg+1, TO_CHAR );
      RETURN_NULL();
    }

    if ( !biliwick( ch, mob ) )
    {
      act( "�A���v�O�����[��$N���ޯ�T", ch, NULL, mob, TO_CHAR );
      RETURN_NULL();
    }

    clear_buffer();
    send_to_buffer( "%s�ҷ|���ޯ঳ %d �ӡR\n\r"
      , mob_name( ch, mob )
      , learn_count( mob ) );

    for ( pSkill = skill_list; pSkill; pSkill = pSkill->next )
    {
      if ( ( adept = mob->skill[pSkill->slot] ) > 0 )
      {
        CanInc = FALSE;

        /* �O�_�W�X¾�~������ */
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
          send_to_buffer( " ���m���ޯ�" );
        else if ( pSkill->type   == TAR_FLEE )
          send_to_buffer( " �k�]���ޯ�" );
        else if ( pSkill->rating == RATING_SKILL )
          send_to_buffer( " %s���ޯ�"
            , weapon_type_name( pSkill->weapon ) );
        else
          send_to_buffer( " %s���k�N"
            , pSkill->rating ? skill_rating( pSkill->rating ) : "����" );

        send_to_buffer( "\n\r" );
      }
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  /* �ݬݬO�_������V�m�� */
  if ( !( mob = get_char_room( ch , arg ) ) )
  {
    act( "�o�̨S�����ӰV�m�� $2$T$0�M�٬O�V�O�H��ť�ݬݧa�C" , ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  if ( mob == ch )
  {
    send_to_char( "�Q�n�۳Чޯ�S�A�i���٭n�V�O�V�O��C\n\r" , ch );
    RETURN_NULL();
  }

  if ( !IS_NPC( mob ) && !IS_SET( mob->turn , PLR_PRACTICE ) )
  {
    act( "�ݨ�$N���ӷQ�Ǳ§ޯ൹�A�C", ch, NULL, mob, TO_CHAR );
    RETURN_NULL();
  }

  one_argument( argument , arg );

  if ( !arg[0] )
  {
    send_to_char( "�藍�_�M�A���y�k����M�Ьd�� learn�C\n\r" , ch );
    RETURN_NULL();
  }

  switch( mob->position )
  {
  default: break;

  case POS_SLEEPING:
    act( "�ݰ_��$N�٨S�ι���T", ch, NULL, mob, TO_CHAR );
    RETURN_NULL();

  case POS_DEAD:
    if ( IS_NPC( mob ) )
    {
      mudlog( LOG_DEBUG, "do_learn: �Ǫ����`�C" );
      RETURN_NULL();
    }

    sprintf( buf , "%s �p�G�گ�_���A�ЧA�a�T�C" , ch->name );
    do_tell( mob , buf );
    RETURN_NULL();

  case POS_FIGHTING:
    sprintf( buf , "%s ���ڥ����A���a�T�C" , ch->name );
    do_tell( mob , buf );
    RETURN_NULL();

  }

  /* �Q���D���ӤH���ޯ঳���� */
  if ( !str_prefix( arg , "!list" ) )
  {
    /* ���ॴť���a���ޯ� */
    if ( !IS_NPC( mob ) )
    {
      act( "��p�M�A���ॴť$N�ҷ|�ޯ�C", ch, NULL, mob, TO_CHAR );
      RETURN_NULL();
    }

    /* ���ݬݬO�_����������O�h�ݧޯ� */
    if ( ch->move < PRACTICE_MOVE )
    {
      send_to_char( "�A����O���١M����]�ݤ���C\n\r" , ch );
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
        if ( count == 0 ) send_to_buffer( "%s\e[0m�|�U�C���ޯ�R\n\r\n\r"
          , mob_name( ch, mob ) );

        send_to_buffer( "\t[%2d] %s %s\n\r"
          , ++count , pSkill->name , pSkill->cname );
      }
    }

    if ( count == 0 ) send_to_buffer(
      "�ݰ_��%s����ޯ�]���|�M���`������C\n\r" , mob_name( ch, mob ) );

    print_buffer( ch );
    RETURN_NULL();
  }

  /* ����O�_�u�����o���ޯ� */
  if ( !( pSkill = skill_isname( arg ) ) )
  {
    send_to_char( "�o�اޯ�۶Ǥw�g���ǤF�ܤ[�M�i��S���H�|�F�a�S\n\r" , ch );
    RETURN_NULL();
  }

  sn     = pSkill->slot;
  fTeach = FALSE;
  teach  = NULL;

  /* ���ݬݪ��a�|���| */
  if ( ch->skill[sn] > 0 )
  {
    act( "�A���w�g�|$w�F�M�F�ܨӧ�کO�S" , ch, pSkill, NULL, TO_CHAR );
    RETURN_NULL();
  }

  /* �ݬݬO�_�|�u���|�o���ޯ� */
  if ( IS_NPC( mob ) )
  {
    for ( teach = mob->pIndexData->teach; teach; teach = teach->next )
    {
      if ( teach->slot == sn )
      {
        if ( ch->gold < teach->cost )
        {
          sprintf( buf , "%s �A���W���������C�T" , ch->name );
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
    sprintf( buf , "%s �ڤ]���ӷ|�o���ޯ�C�M���p�A�Чڧa�C" , ch->name );
    do_tell( mob , buf );
    RETURN_NULL();
  }

  /* �{�̪���O���� */
  if ( ch->move < PRACTICE_MOVE )
  {
    send_to_char( "�A�Ӳ֤F�M����]�Ǥ���C\n\r" , ch );
    RETURN_NULL();
  }

  /* �Ѯv����O���� */
  if ( !IS_NPC( mob ) && mob->move < PRACTICE_MOVE )
  {
    act( "$N����O���١M�A����]�Ǥ���C" , ch, NULL, mob, TO_CHAR );
    RETURN_NULL();
  }

  /* �ˬd�ޯ઺���� */
  if ( !check_skill_restrict( ch, pSkill, TRUE ) ) RETURN_NULL();
  if ( !check_valid_rating( ch, pSkill, TRUE ) ) RETURN_NULL();

  /* �����ǲߩүӪ���O */
  ch->move -= PRACTICE_MOVE;
  if ( !IS_NPC( mob ) ) mob->move -= PRACTICE_MOVE;

  /* �Ѯv�O���a, �����m�פ��� */
  if ( !IS_NPC( mob ) && mob->skill[sn] < 30 )
  {
    sprintf( buf , "%s ��p�M�ڦۤv�]���ӷ|�C�T" , ch->name );
    do_tell( mob , buf );
    RETURN_NULL();
  }

  if ( !ch->class || learn_count( ch ) > get_learnable( ch ) )
  {
    sprintf( buf , "%s �藍�_�M�A�A�]�O������h�ۦ��F�T\n\r" , ch->name );
    do_tell( mob , buf );
    RETURN_NULL();
  }

  /* �ǲߪ����ŭ��� */
  if ( ch->level < get_skill_class_level( pSkill, ch->class ) )
  {
    sprintf( buf , "%s �A�����ũάO¾�~�����H�ǲ߳o���ۦ��T" , ch->name );
    do_tell( mob , buf );
    RETURN_NULL();
  }

  /* �O�_��Ѫ��a�о� */
  if ( !IS_NPC( mob ) && pSkill->teach == FALSE )
  {
    sprintf( buf , "%s �藍�_�M�o�����@�ޯ�O����~�Ǫ��T" , ch->name );
    do_tell( mob , buf );
    RETURN_NULL();
  }

  /* �Ѯv�O�D���a�ӥB�оɥ��� */
  if ( IS_NPC( mob ) && ( ( number_percent() - 1 ) > teach->adept * 2 ) )
  {
    sprintf( buf , "%s ��p�M���Ѩ��餣�ΡM��ѦA�ЧA�T" , ch->name );
    do_tell( mob , buf );
    RETURN_NULL();
  }

  /* �Ѯv�O���a�ӥB�оɥ��� */
  if ( !IS_NPC( mob ) &&
     ( ( number_percent() - 1 ) > mob->skill[sn] * 2 ) )
  {
    sprintf( buf , "%s ��p�M���ѩۦ���M�ѤF�M��ѦA�ЧA�T" , ch->name );
    do_tell( mob , buf );
    RETURN_NULL();
  }

  IS_NPC(mob) ? ( adept = teach->adept ) : ( adept = mob->skill[sn] );

  /* �p��{�̪����m��, �� 1 �� 10 */
  adept = number_range( 1 , UMIN( 10, adept / 3 ) );
  adept = UMIN( adept , 10 );
  adept = UMAX( adept , 1  );

  /* �Ѯv�O���a, �h�{�̪����m�׭n�q�Ѯv�o�̦� */
  if ( !IS_NPC( mob ) ) mob->skill[sn] -= adept;

  if ( !IS_NPC( mob ) && IS_IMMORTAL( mob ) )
    mudlog( LOG_INFO, "[TEACH] ���� %s �о� %s �ޯ� %s ."
      , mob->name, ch->name, pSkill->name );

  /* �]�w�{�̪����m�� */
  ch->skill[sn] = adept;

  act( "���H�A�M�A�Ƿ|�F$w�o���ޯ�C",ch , pSkill, NULL, TO_CHAR );
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
    act( "���A�ꨬ�F$i��Ȥl�M�A�ӥ�ť�a�T", ch, &FindCost, NULL, TO_CHAR );
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

    act( "�b�A����$r��$N�ܾA�X�A�ɯŪ��Ǫ��M���֥h��L�a�T"
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

    act( "���$t��$N�ܾA�X�A�ɯŪ��Ǫ��M���֥h��L�a�T"
      , ch, mob->in_room->area->name, mob, TO_CHAR );
  }

  else
  {
    send_to_char( "�藍�_�M�䤣��A�n�ɯŪ��Ǫ��C�T\n\r", ch );
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
    mudlog( LOG_DEBUG, "create_exit: �ӷ����~." );
    RETURN( NULL );
  }

  if ( !( pRoom = get_room_index( vHere ) ) )
  {
    mudlog( LOG_DEBUG, "create_exit: �ǤJ�}���ж������~: %d.", vHere );
    RETURN( NULL );
  }

  if ( !( tRoom = get_room_index( vGoto ) ) )
  {
    mudlog( LOG_DEBUG, "create_exit: �ǤJ�X�f�ж����~: %d.", vGoto );
    RETURN( NULL );
  }

  if ( pRoom->exit[nExit] )
  {
    mudlog( LOG_DEBUG, "create_exit: �ǤJ�ж��X�f�w�g�s�b: %d->%d."
      , vHere, nExit );
    RETURN( NULL );
  }

  /* �t�m�O���� */
  set_exit_default( ( pExit = alloc_struct( STRUCT_EXIT_DATA ) ) );

  /* �]�w�ƭȻP�w�]�� */
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
    mudlog( LOG_DEBUG, "close_exit: �ӷ����~." );
    RETURN_NULL();
  }

  if ( !( pRoom = get_room_index( vRoom ) ) )
  {
    mudlog( LOG_DEBUG, "close_exit: �ж� %d ���s�b.", vRoom );
    RETURN_NULL();
  }

  if ( pRoom->exit[vExit] == NULL )
  {
    mudlog( LOG_DEBUG, "close_exit: �ж��X�f %d -> %d ���s�b�o�n����."
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
    send_to_char( "�A�n�V�ְݦ���������O�S\n\r", ch );
    RETURN_NULL();
  }

  if ( !( pRoom = ch->in_room ) )
  {
    send_to_char( "�藍�_�M���A�ثe���b��B�O�S\n\r", ch );
    RETURN_NULL();
  }

  if ( !( victim = get_char_room( ch, arg1 ) ) )
  {
    act( "�藍�_�M�o�̨S���o�ӤH $2$T$0 ��T", ch, NULL, arg1, TO_CHAR );
    RETURN_NULL();
  }

  if ( !can_see( victim, ch ) )
  {
    act( "$N�ݤ���A�C�M���ݸ��O�S", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( !IS_NPC( victim ) || victim->speak == FALSE )
  {
    act( "�藍�_�M�A����V$N�ݸ��T", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( !victim->speak )
  {
    act( "�藍�_�M$N���|���ܭC�T", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( victim->position != POS_STANDING )
  {
    act( "$N���b���C�M���@�U�A�ݧa�T", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( ( level = victim->level / 20 ) <= 0 )
  {
    act( "$N�٤p�M�L���򳣤����M�ݧO�H�a�T", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  act( "$n���b�V$N�ݸ��T", ch, NULL, victim, TO_ALL );

  /* ���ݬݳo�̬O�_�N�O */
  for ( pEnquire = pRoom->enquire; pEnquire; pEnquire = pEnquire->next )
  {
    if ( !str_cmp( pEnquire->keyword, arg2 ) )
    {
      sprintf( buf, "%s �ڬݧA�O�~�a�Ӫ��a�M�o�̤��N�O%s�F�աT\n\r"
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
    sprintf( buf, "%s %s�M�n�����e��ť�L�M�A�ոլݳo�̪�", ch->name, arg2 );

    for ( count = 0; count < MAX_SCAN_DEPTH; count++ )
    {
       if ( ScanData[rem].dir[count] < 0 ) break;
       str_cat( buf, direction_name( ScanData[rem].dir[count] ) );
    }

    str_cat( buf, "��M�n���N�b���̡T" );
    do_tell( victim, buf );
    clean_scan();
    RETURN_NULL();
  }

  clean_scan();

  sprintf( buf, "%s �藍�_�M�ڤ]�����D%s�b���̭C�M�A�ݰݨ�L�H�a�T"
    , ch->name, arg2 );

  do_tell( victim, buf );

  sprintf( buf, "pat %s", victim->name );
  interpret( victim, buf );

  RETURN_NULL();
}


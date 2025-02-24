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

/* �ϰ��� */
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
    mudlog( LOG_DEBUG, "get_obj: �ʥF�ӷ�." );
    RETURN_NULL();
  }

  if ( obj->Takeable == FALSE )
  {
    act( "�A�L�k���_$p�C", ch, obj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( obj->item_type == ITEM_CORPSE_PC || obj->item_type == ITEM_CORPSE_NPC )
  {
    act( "�A�Q����$p�F����S", ch, obj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( ch->carry_number + 1 > can_carry_n( ch ) )
  {
    act( "$t�R�A������a�o��h���F��C", ch , obj->name, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( get_carry_weight( ch ) + get_obj_weight( obj ) > can_carry_w( ch ) )
  {
    act( "$t�R�A������a�o�򭫪��F��C" ,ch , obj->name, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( !IS_NPC( ch ) && excess_filequota( ch ) )
  {
    act( "$t�R�A���ɮפӤj�M�o�өR�O�����T", ch, obj->name, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( container )
  {
    act( "$n�q$P���X$p�C", ch, obj, container, TO_ALL );
    message_driver( ch, obj, ACT_WHEN_GET );
    obj_from_obj( obj );
  }

  else
  {
    act( "$n�߰_$p�C", ch, obj, NULL, TO_ALL );
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
      send_to_char( "�A���`�겣�w�F�W���M�L�k�~�򮳿��T\n\r", ch );
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
    send_to_char( "�藍�_�M�ثe�S���o�{��A������M�ҥH�L�k�j�M�C\n\r", ch );
    RETURN_NULL();
  }

  if ( corpse->in_room != ch->in_room )
  {
    send_to_char( "�藍�_�M�A�٨S�����^(\e[1;32mcrave\e[0m)�A�������T\n\r"
      , ch );
    RETURN_NULL();
  }

  for ( found = FALSE, obj = corpse->contains; obj; obj = obj_next )
  {
    obj_next = obj->next_content;

    if ( can_see_obj( ch, obj ) )
    {
      found = TRUE;

      /* ���~�O�_�����r */
      if ( !poison_char( ch, obj ) ) get_obj( ch, obj, corpse );
    }
  }

  if ( !found )
    send_to_char( "�A����̨S������A�ݪ��쪺�˳ƤF�M�A�i�H�^�m�A������F"
      "(\e[1;32msac corpse\e[0m)�C\n\r", ch );
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
        act( "�ڦb$r�S���ݨ����$t�C" , ch, arg1, NULL, TO_CHAR );
        RETURN_NULL();
      }

      /* ���~�O�_�����r */
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

          /* ���~�O�_�����r */
          if ( !poison_char( ch, obj ) ) get_obj( ch, obj, NULL );
        }
      }

      if ( !found )
      {
        if ( !arg1[3] )
          send_to_char( "�o�̨èS���ݨ����F��C\n\r", ch );
        else
          act( "�ڦb$r�S���ݨ����$2$t$0�C", ch, &arg1[4], NULL, TO_CHAR );
      }
    }
  }

  else
  {
    if ( !str_cmp( arg2, "all" ) || !str_prefix( "all.", arg2 ) )
    {
      send_to_char( "�A����o�˰��C\n\r", ch );
      RETURN_NULL();
    }

    if ( !( container = get_obj_here( ch, arg2 ) ) )
    {
      act( "�ڦb$r�èS���ݨ����$t�C",ch , arg2, NULL, TO_CHAR );
      RETURN_NULL();
    }

    /* ���~�O�_�����r */
    if ( poison_char( ch, container ) ) RETURN_NULL();

    switch ( container->item_type )
    {
    default:
      act( "$p�ä��O�@�Ӯe���C", ch, container, NULL, TO_CHAR );
      RETURN_NULL();

    case ITEM_CONTAINER :
    case ITEM_CORPSE_NPC:
      break;

    case ITEM_CORPSE_PC :

      if ( container->corpse_owner != ch )
      {
        send_to_char( "�o�ä��O�A�������M�A���ள���L�H��"
          "��̪��F��C\n\r", ch );
        RETURN_NULL();
      }

      if ( IS_NPC( ch ) )
      {
        send_to_char( "�A����o�˰��C\n\r", ch );
        RETURN_NULL();
      }

      break;
    }

    if ( IS_SET( container->value[1], CONT_LOCKED ) )
    {
      act( "$p�Q���F�M�����}�T", ch, container, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( IS_SET( container->value[1], CONT_CLOSED ) )
    {
      act( "$p�Q���F�_�ӤF�C", ch , container, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( str_cmp( arg1, "all" ) && str_cmp( "all.", arg1 ) )
    {
      if ( !( obj = get_obj_list( ch, arg1, container->contains ) ) )
      {
        act( "�ڦb$t�èS���ݨ���󪺪F��C",ch , arg2, NULL, TO_CHAR );
        RETURN_NULL();
      }

      /* ���~�O�_�����r */
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

          /* ���~�O�_�����r */
          if ( !poison_char( ch, obj ) ) get_obj( ch, obj, container );
        }
      }

      if ( !found )
      {
        if ( !arg1[3] )
          act( "�ڦb$t�èS���ݨ���󪺪F��C", ch, arg2, NULL, TO_CHAR );

        else
          act( "�ڦb$t�èS���ݨ�A�Q�n���F��C\n\r",ch ,arg2 ,NULL, TO_CHAR );
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
    act( "�b�A�ɯŨ�$i�ūe�M�Ф��n�å��F��C", ch, &level_limit, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( ch->donate > 0 )
  {
    send_to_char( "�b�A����٪����o�q�ɶ��M�Ф��n�å�F��n�ܡS\n\r", ch );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg1 );
             one_argument( argument, arg2 );

  if ( arg1[0] == '\x0' || arg2[0] == '\x0' )
  {
    send_to_char( "�A�n�⤰��F�襵�쨺�̥h�O�S\n\r", ch );
    RETURN_NULL();
  }

  if ( !( pGold = get_obj_here( ch, arg2 ) ) )
  {
    act( "�ڦb$r�S���ݨ����$T�C", ch, NULL, arg2, TO_CHAR );
    RETURN_NULL();
  }

  if ( pGold->item_type != ITEM_GOLDMINE )
  {
    act( "�藍�_�M$p���O�ӻE�_�֭C�M�ҥH�A���ॵ�F��쨺�̥h�T"
      , ch, pGold, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( !( obj = get_obj_carry( ch, arg1 ) ) )
  {
    act( "�A�S������ $2$T$0 �a�C", ch, NULL, arg1, TO_CHAR );
    RETURN_NULL();
  }

  if ( obj->wear_loc != WEAR_NONE )
  {
    act( "�A�٧�$p��b���W�C�M���⥦��F�a�C", ch, obj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  /* ���~�O�_�����r */
  if ( poison_char( ch, obj ) ) RETURN_NULL();

  if ( !can_drop_obj( ch, obj ) )
  {
    act( "�A������$p�M�ҥH����å��C", ch, obj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( obj->address )
  {
    act( "$p�O�n�浹�O�H���H�M���i�H�å��C", ch, obj, NULL, TO_CHAR );
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

      act( "��M�M$P�_�X�@�}�@�ϡM����j�a�^���ɡM�a�W���M�h�F�@��$p�C"
        , ch, treasure, pGold, TO_ALL );
    }

    else
    {
      send_to_char( "���M�u�O�d�G�M�ڭ̪��V�O���M�Q�W�ѩ����F�T\n\r", ch );
    }

    pGold->value[1] = 0;
  }

  else
  {
    act( "$n���F$p��$P�M$p�b�����N���F�@�}�C�ϡM���O����ǨӤ@�}�n���M���������T"
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
    send_to_char( "�⤰��F���줰��a��h�S\n\r", ch );
    RETURN_NULL();
  }

  if ( !str_cmp( arg2, "all" ) || !str_cmp( "all.", arg2 ) )
  {
    send_to_char( "�A����o�˰��C\n\r", ch );
    RETURN_NULL();
  }

  if ( !( container = get_obj_here( ch, arg2 ) ) )
  {
    act( "�ڦb$r�S���ݨ����$T�C", ch, NULL, arg2, TO_CHAR );
    RETURN_NULL();
  }

  if ( container->item_type != ITEM_CONTAINER )
  {
    act( "$p���O�@�Ӯe���C", ch, container, NULL, TO_CHAR );
    RETURN_NULL();
  }

    if ( IS_SET( container->value[1], CONT_LOCKED ) )
    {
      act( "$p�Q���F�M�����}�T", ch, container, NULL, TO_CHAR );
      RETURN_NULL();
    }

  if ( IS_SET( container->value[1], CONT_CLOSED ) )
  {
    act( "$p�Q���_�ӤF�C", ch, container, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( str_cmp( arg1, "all" ) && str_prefix( "all.", arg1 ) )
  {
    if ( !( obj = get_obj_carry( ch, arg1 ) ) )
    {
      act( "�A�èS������ $2$T$0�M���O�ܡS", ch, NULL, arg1, TO_CHAR );
      RETURN_NULL();
    }

    if ( obj == container )
    {
      act( "�A�����$p����L���F��̭��C", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( obj->address )
    {
      act( "$p�O�n�浹�O�H���H�M���ۤv���_�ӡC", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( !can_drop_obj( ch, obj ) )
    {
      act( "�A������$p�C", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( get_obj_weight( obj ) + get_obj_weight( container )
         > container->value[0] )
    {
      act( "$p�Ӥp�F�M�ˤ��U$P�C", ch, container, obj, TO_CHAR );
      RETURN_NULL();
    }

    obj_from_char( obj );
    obj_to_obj( obj, container );
    act( "$n��$p���$P�̭��h�C", ch, obj, container, TO_ALL );

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
        act( "$n��$p���$P�̭��h�C", ch, obj, container, TO_ALL );
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
    act( "�b�A�ɯŨ�$i�ūe�M�Ф��n�ðe�F��C"
      , ch, &level_limit, NULL, TO_CHAR );

    RETURN_NULL();
  }

  if ( ch->donate > 0 )
  {
    send_to_char( "�b�A����٪����o�q�ɶ��M�Ф��n�å�F��T\n\r", ch );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg );

  if ( !str_prefix( arg, "!list" ) )
  {
    argument = one_argument( argument, arg );

    if ( !ch->club )
    {
      send_to_char( "�A�S���ѥ[���������M����s��˳ƩO�S\n\r", ch );
      RETURN_NULL();
    }

    if ( !( pClub = ch->in_room->club ) || pClub != ch->club )
    {
      send_to_char( "�o�̤��O�A�������E���I�M�ҥH����d�߸˳ơT\n\r", ch );
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
        send_to_buffer( "���šR%2d ��ơR\e[1;32m%2d\e[0m   "
          , level, count );
      }

      if ( total == 0 ) send_to_buffer( "�A�������u�O�a�ڡM�b�Ӹ˳Ƥ]�S���T\n\r" );
      else              send_to_buffer( "\n\r" );

      print_buffer( ch );
      RETURN_NULL();
    }

    if ( !is_number( arg )
      || ( level = atoi( arg ) ) <= 0
      || level > LEVEL_HERO )
    {
      send_to_char( "�藍�_�M�A�����ŰѼƤ��X�z�T\n\r", ch );
      RETURN_NULL();
    }

    clear_buffer();
    send_to_buffer( "%s\e[0m(%s) �����w�M���� %d ���Ҧ��˳ơR\n\r"
      , pClub->cname, pClub->name, level );

    for ( count = loop = 0; loop < MAX_STAFF; loop++ )
    {
      if ( !( obj = pClub->staff[level][loop] ) ) continue;

      if ( count == 0 )
        send_to_buffer( "\e[1;33;44m���� �˳ƪ����O �˳ƪ��W��           "
          "                                           \e[0m\n\r" );

      send_to_buffer( "%4d %-10s %s\n\r"
        , ++count
        , item_kind_name( obj->item_type )
        , obj_name( ch, obj ) );
    }

    if ( count == 0 ) send_to_buffer( "�S�����󪺸˳ơT\n\r", ch );
    print_buffer( ch );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "!withdraw" ) )
  {
    if ( !ch->club )
    {
      send_to_char( "�A�S���ѥ[���������M����s��˳ƩO�S\n\r", ch );
      RETURN_NULL();
    }

    if ( !( pClub = ch->in_room->club ) || pClub != ch->club )
    {
      send_to_char( "�o�̤��O�A�������E���I�M�ҥH����s��˳ơT\n\r", ch );
      RETURN_NULL();
    }

    if ( excess_filequota( ch ) )
    {
      send_to_char( "�藍�_�M�A���ɮפӤj�M�L�k����o�өR�O�T\n\r", ch );
      RETURN_NULL();
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\x0'
      || !is_number( arg )
      || ( level = atoi( arg ) ) <= 0
      || level > LEVEL_HERO )
    {
      send_to_char( "�A�����ŰѼƤ��X�z�T\n\r", ch );
      RETURN_NULL();
    }

    argument = one_argument( argument, arg );
    if ( arg[0] == '\x0'
      || !is_number( arg )
      || ( count = atoi( arg ) ) <= 0
      || count >= MAX_STAFF )
    {
      send_to_char( "�藍�_�M�A�����ǰѼƤ��X�z�T\n\r", ch );
      RETURN_NULL();
    }

    for ( total = loop = 0; loop < MAX_STAFF; loop++ )
    {
      if ( ( obj = pClub->staff[level][loop] ) && ++total == count )
      {
        obj->club                 = NULL;
        pClub->staff[level][loop] = NULL;

        obj_to_room( obj, ch->in_room );

        act( "$n�����w�̪�$p���F�X�ӡT", ch, obj, NULL, TO_ALL );
        RETURN_NULL();
      }
    }

    send_to_char( "�藍�_�M���w�̨S���o���˳ơT\n\r", ch );
    RETURN_NULL();
  }

  else
  {
    if ( !ch->club )
    {
      send_to_char( "�A�S���ѥ[���������M����s��˳ƩO�S\n\r", ch );
      RETURN_NULL();
    }

    if ( !( pClub = ch->in_room->club ) || pClub != ch->club )
    {
      send_to_char( "�o�̤��O�A�������E���I�M�ҥH����s��˳ơT\n\r", ch );
      RETURN_NULL();
    }

    if ( !IS_NPC( ch ) && ch->level < level_limit )
    {
      act( "�b�A�ɯŨ�$i�ūe�M�Ф��n�H�K���˳ơC"
        , ch, &level_limit, NULL, TO_CHAR );

      RETURN_NULL();
    }

    if ( ch->donate > 0 )
    {
      send_to_char( "�b�A����٪����o�q�ɶ��M�Ф��n�å�˳ơT\n\r", ch );
      RETURN_NULL();
    }

    if ( !( obj = get_obj_carry( ch, arg ) ) )
    {
      act( "�藍�_�M�A�S�����˪��~ $2$T$0�C", ch, NULL, arg, TO_CHAR );
      RETURN_NULL();
    }

    if ( obj->wear_loc != WEAR_NONE )
    {
      act( "�A�٧�$p��b���W�C�M���⥦��F�a�C", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    /* ���~�O�_�����r */
    if ( poison_char( ch, obj ) ) RETURN_NULL();

    if ( !can_store( obj ) )
    {
      act( "��p�M$p�ڭ̤������A�s��C" , ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( obj->address )
    {
      act( "$p�O�n�浹�O�H���H�M���i�H�������C", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( !can_drop_obj( ch, obj ) )
    {
      act( "$p�O�A���Ǯa�_�M�A�����󥦡C", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( ( level = obj->level ) <= 0 || level > LEVEL_HERO )
    {
      act( "�藍�_�M$p�����Ť��A�X�Ӧs��T", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    for ( loop = 0; loop < MAX_STAFF; loop++ )
    {
      if ( pClub->staff[level][loop] == NULL )
      {
        obj_from_char( obj );

        obj->club                 = pClub;
        pClub->staff[level][loop] = obj;

        act( "$n�w�g��$p��b$Y���w�����T", ch, obj, pClub, TO_ALL );

        RETURN_NULL();
      }
    }

    send_to_char( "�藍�_�M�A�����w�w�g�����M�L�k�A�s��F�T\n\r", ch );
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
    send_to_char( "�A���`�겣�ӥi�ȤF�M�ҥH�L�k����o�ӫ��O�T\n\r", ch );
    RETURN_NULL();
  }

  if ( !IS_NPC( ch ) && ch->level < level_limit )
  {
    act( "�b�A�ɯŨ�$t�ūe�M�Ф��n�H�K��F��C"
      , ch, &level_limit, NULL, TO_CHAR );

    RETURN_NULL();
  }

  if ( ch->donate > 0 )
  {
    send_to_char( "�b�A����٪����o�q�ɶ��M�Ф��n�H�K��F��T\n\r", ch );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg );

  if ( is_number( arg ) )
  {
    amount   = atoi(arg);
    argument = one_argument( argument, arg );

    if ( amount <= 0 || ( str_prefix( arg, "coins" ) ) )
    {
      act( "�藍�_�M�A�S�����˪F�� $2$T$0�C", ch, NULL, arg, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch->gold < amount )
    {
      send_to_char( "�֨ӤF�M�A�S������h���C\n\r", ch );
      RETURN_NULL();
    }

    gold_from_char( ch, amount );
    if ( ch->in_room ) gold_to_room( ch->in_room, amount );

    act( "$n�ᱼ�@�Ǫ��l�b�a�W�C", ch, NULL, NULL, TO_ALL );
    RETURN_NULL();
  }

  if ( str_cmp( arg, "all" ) && str_prefix( "all.", arg ) )
  {
    if ( !( obj = get_obj_carry( ch, arg ) ) )
    {
      act( "�A�S�����˪��~ $2$T$0�C", ch, NULL, arg, TO_ALL );
      RETURN_NULL();
    }

    if ( ch->in_room && ch->in_room->club && can_store( obj ) )
    {
      act( "$p�i�H�Φs��($2store$0)����T", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( obj->address )
    {
      act( "$p�O�n�浹�O�H���H�M���i�H�������C", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( !can_drop_obj( ch, obj ) )
    {
      act( "�A������$p�C", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    act( "$n�ᱼ$p�C", ch, obj, NULL, TO_ALL );

    message_driver( ch, obj, ACT_WHEN_DROP );

    obj_from_char( obj );

    if ( obj->disappear )
    {
      act( "$p�ݰ_�Ӥ@�夣�ȡM�S���H�|�`�N�쥦���s�b�C", ch, obj, NULL, TO_ROOM );
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
          act( "$p�i�H�Φs��($2store$0)����!", ch, obj, NULL, TO_CHAR );
          continue;
        }

        act( "$n�ᱼ$p�C", ch, obj, NULL, TO_ALL );

        message_driver( ch, obj, ACT_WHEN_DROP );

        obj_from_char( obj );

        if ( obj->disappear )
        {
          act( "$p�ݰ_�Ӥ@�夣�ȡM�S���H�|�`�N�쥦���s�b�C"
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
        send_to_char( "�A���W�S������F��M����O�S\n\r", ch );
      else
        act( "�A���W�èS����a����$T�C", ch, NULL, &arg[4], TO_CHAR );
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
    act( "�b�A�ɯŨ�$i�ūe�M�Ф��n�H�K����L�H�F��C"
      , ch, &level_limit, NULL, TO_CHAR );

    RETURN_NULL();
  }

  if ( ch->donate > 0 )
  {
    send_to_char( "�b�A����٪����o�q�ɶ��M�Ф��n�H�K���O�H�F��T\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->trade == FALSE )
  {
    send_to_char( "�A���`�겣�ӥi�ȤF�M�ҥH�L�k����o�ӫ��O�T\n\r", ch );
    RETURN_NULL();
  }

  if ( !arg1[0] || !arg2[0] )
  {
    send_to_char( "�A�n�⤰��F�赹�֡S\n\r", ch );
    RETURN_NULL();
  }

  if ( is_number( arg1 ) )
  {
    if ( ( amount = atoi( arg1 ) ) <= 0 || ( str_prefix( arg2, "coins" ) ) )
    {
      act( "�藍�_�M�A�S�����ӪF�� $2$T$0�C", ch, NULL, arg1, TO_CHAR );
      RETURN_NULL();
    }

    argument = one_argument( argument, arg2 );

    if ( !arg2[0] )
    {
      send_to_char( "�A�n�⤰��F�赹�֡S\n\r", ch );
      RETURN_NULL();
    }

    if ( !( victim = get_char_room( ch, arg2 ) ) )
    {
      act( "�A�n�����H $2$T$0 ���b�o���ж��̡C", ch, NULL, arg2, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch == victim )
    {
      send_to_char( "�A�F�����ۤv���ڡS\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->gold < amount )
    {
      send_to_char( "�K�K�M�A�S������h���a�C\n\r", ch );
      RETURN_NULL();
    }

    gold_from_char( ch, amount );

    if ( over_scale( victim ) )
    {
      act( "$N���`�겣�w�F�W���T", ch, NULL, victim, TO_CHAR );
      act( "$N���`�겣�w�F�W���T", ch, NULL, victim, TO_VICT );
      RETURN_NULL();
    }

    else
    {
      gold_to_char( victim, amount );
    }

    act( "$n��$N�@�Ǫ��l�C", ch, NULL, victim, TO_ALL );

    if ( is_affected( victim, SLOT_MASK ) )
      act( "�A�����e�N���G�Q$N�ѯ}�F�T", ch, NULL, victim, TO_CHAR );

    send_to_char( "�n�F�T\n\r", ch );
    mprog_bribe_trigger( victim, ch, amount );
    RETURN_NULL();
  }

  if ( !( obj = get_obj_carry( ch, arg1 ) ) )
  {
    act( "�A�S�����˪F�� $2$T$0 �a�C", ch, NULL, arg1, TO_CHAR );
    RETURN_NULL();
  }

  if ( obj->wear_loc != WEAR_NONE )
  {
    act( "�A�٧�$p��b���W�C�M���⥦��F�a�C", ch, obj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( !( victim = get_char_room( ch, arg2 ) ) )
  {
    act( "�A�n�����H $2$T$0 ���b�o���ж��̭��C", ch, NULL, arg2, TO_CHAR );
    RETURN_NULL();
  }

  if ( ch == victim )
  {
    send_to_char( "�A�F�ܵ��ۤv�F��O�S\n\r", ch );
    RETURN_NULL();
  }

  /* ���~�O�_�����r */
  if ( poison_char( ch, obj ) ) RETURN_NULL();

  if ( obj->address )
  {
    if ( obj->address != victim )
    {
      act( "$p���O���ڪ��a�M�A�d���F�C", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    act( "$N���D�R�u���¡M�o�ʫH�ڤw�g����F�T�v", ch, NULL, victim, TO_CHAR );

    obj_from_char( obj );
    extract_obj( obj );


    if ( ch->level >= LEVEL_HERO )
    {
      if ( number_range( 0, 20 ) == 0 )
      {
        if ( add_firman( ch, 1 ) > 0 )
          send_to_char( "�]���A���b�i�M�گS�a�e���A�ڪ��Ǯa�_�M"
                        "�K�����P�@�i�C\n\r", ch );
      }

      else
      {
        if ( !over_scale( ch ) )
        {
          amount = number_range( obj->cost, obj->cost + 500 );
          gold_to_char( ch, amount );

          act( "�]���A�����~�b�i�M�گS�a�e�A$i��Ȥl�C"
            , ch, &amount, NULL, TO_CHAR );
        }
        else
        {
          send_to_char( "�]���A���`�겣�w�F�W���M�L�k����b����T\n\r", ch );
        }
      }
    }

    else
    {
      if ( number_range( 0, 3000 ) <= ch->level )
      {
        if ( add_firman( ch, 1 ) > 0 )
          send_to_char( "�]���A���b�i�M�گS�a�e���A�ڪ��Ǯa�_�M"
                        "�K�����P�@�i�C\n\r", ch );
      }

      else
      {
        if ( !over_scale( ch ) )
        {
          amount = number_range( obj->cost, obj->cost + 800 );

          gold_to_char( ch, amount );
          act( "�]���A�����~�b�i�M�گS�a�e�A$i��Ȥl�C"
            , ch, &amount, NULL, TO_CHAR );
        }

        else
        {
          send_to_char( "�]���A���`�겣�w�F�W���M�L�k����b����T\n\r", ch );
        }
      }
    }

    RETURN_NULL();
  }

  /* �Y�O�q�� */
  if ( obj->item_type == ITEM_ORE
    && victim->pIndexData
    && victim->pIndexData->pShop
    && victim->pIndexData->pShop->type == SHOP_SMITH )
  {
    if ( !IS_NPC( ch ) && excess_filequota( ch ) )
    {
      send_to_char( "�藍�_�M�A���ɮפӤj�M�o�ӫ��O�L�ġT\n\r", ch );
      RETURN_NULL();
    }

    if ( !( pIndex = get_obj_index( obj->value[0] ) )
      || obj->value[2] <= 0
      || ( amount = obj->value[3] ) <= 0 )
    {
      act( "$N���D�R�u�ݨӳo��$p���`������M���ڤ]�S�ΡT�v"
        , ch, obj, victim, TO_CHAR );

      RETURN_NULL();
    }

    if ( victim->level < obj->value[1] )
    {
      act( "$N���D�R�u�o��$p���T�����M���O�ڵL�ର�O�M�t�а����a�T�v"
        , ch, obj, victim, TO_CHAR );

      RETURN_NULL();
    }

    if ( ch->gold < obj->value[3] )
    {
      act( "$N���D�R�u���y�o�ӪF��ܤ֭n$i��Ȥl�a�M�٤��@�w�|���\\�T�v"
        , ch, &amount, victim, TO_CHAR );

      RETURN_NULL();
    }

    act( "$N���ۦ����M�@��V�O�a�����$p�T", ch, obj, victim, TO_ALL );

    if ( number_range( 0, 10000 - victim->level * 10 ) >= obj->value[2] )
    {
      amount = UMAX( 20, amount / 10 );

      act( "$N���D�R�u�o�ӪF��n���L�k�@�ɷd�w�M�����I�u���O$i��Ȥl�a�T�v"
        , ch, &amount, victim, TO_CHAR );

      gold_from_char( ch, amount );
      RETURN_NULL();
    }

    else
    {
      target = create_object( pIndex, -1 );
      act( "�j�\\�i���M$p�ש󥴳y��$P�M���h���ݧa�T", ch, obj, target, TO_CHAR );

      if ( target->Takeable == FALSE
        || ( ch->carry_number + get_obj_number( target ) > can_carry_n( ch ) )
        || ( get_carry_weight( ch ) + get_obj_weight( target ) > can_carry_w( ch ) ) )
      {
        act( "$N���D�R�u�]��$n������$p�M�ҥH�ڧ⥦���a�W�F�T�v"
          , ch, target, victim, TO_ALL );

        obj_to_room( target, ch->in_room );
      }

      else
      {
        act( "$N�⥴�y�n��$p�����F$n�T", ch, target, victim, TO_ALL );
        obj_to_char( target, ch );
      }

      act( "$N�V�A���F$i��Ȥl�H��u��T", ch, &amount, victim, TO_CHAR );

      obj_from_char( obj );
      extract_obj( obj );
    }

    RETURN_NULL();
  }

  if ( !can_drop_obj( ch, obj ) )
  {
    act( "�A������$p�M�ҥH���൹�O�H�C", ch, obj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( victim->carry_number + get_obj_number( obj ) > can_carry_n( victim ) )
  {
    act( "$N���G�����ʨ���h�F��C", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( get_carry_weight( victim ) + get_obj_weight( obj ) > can_carry_w( victim ) )
  {
    act( "$N���G�����ʨ��򭫪��F��C", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( !can_see_obj( victim, obj ) )
  {
    act( "$N�ݤ����o�˪F��C", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( !IS_NPC( victim ) && excess_filequota( victim ) )
  {
    act( "�藍�_�M$N���ɮפӤj�M�L�k�A��F�赹$E�T", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  obj_from_char( obj );
  obj_to_char( obj, victim );
  MOBtrigger = FALSE;

  act( "$n��$p���F$N�C" , ch, obj, victim, TO_NOTVICT );
  act( "$n���A$p�C"     , ch, obj, victim, TO_VICT    );
  act( "�A��$p���F$N�C" , ch, obj, victim, TO_CHAR    );

  if ( is_affected( victim, SLOT_MASK ) )
    act( "�D�F�M�A�����e�N���G�Q$N���ѯ}�F�T", victim, NULL, ch, TO_CHAR );

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
    act( "�A�S�����˪��~ $2$T$0�C", ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  /* ���~�O�_�����r */
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
    act( "$r�èS������άO�Q�����C", ch, NULL, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( obj->item_type != ITEM_DRINK_CON )
  {
    act( "�A�����$p���˺��C", ch, obj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( obj->value[1] != 0 && obj->value[2] != liq_water->slot )
  {
    act( "$p�w�g����L�G��F�M�A�Q���ƾǹ���ܡS", ch, obj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( obj->value[1] >= obj->value[0] )
  {
    act( "$p�w�g���F�C", ch, obj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  act( "�A��$p�˺��F�C", ch, obj, NULL, TO_CHAR );
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
      send_to_char( "�A�n�ܤ���S\n\r", ch );
      RETURN_NULL();
    }
  }
  else
  {
    if ( !( obj = get_obj_here( ch, arg ) ) )
    {
      act( "�A�䤣�쨺�ӪF�� $2$T$0�C", ch, NULL, arg, TO_CHAR );
      RETURN_NULL();
    }
  }

  /* ���~�O�_�����r */
  if ( poison_char( ch, obj ) ) RETURN_NULL();

  if ( !IS_NPC( ch ) && ch->pcdata->condition[ COND_DRUNK ] > 10 )
  {
    send_to_char( "�A�w�g�K���S��k��Ǥf�C\n\r", ch );
    RETURN_NULL();
  }

  switch ( obj->item_type )
  {
  default:
    act( "�A�S����k�q$p�ܪF��C", ch, obj, NULL, TO_CHAR );
    break;

  case ITEM_FOUNTAIN:

    if ( !IS_NPC(ch) ) ch->pcdata->condition[COND_THIRST] = 100;
    act( "$n�q�����̳ܤ��M�n�ܪ����C", ch, NULL, NULL, TO_ROOM );
    send_to_char( "�A�w�g�����F�C\n\r", ch );

    message_driver( ch, obj, ACT_WHEN_DRINK );
    break;

  case ITEM_POTION:

    if ( obj->carried_by != ch )
    {
      act( "$p���O�A���M�ҥH�A����ܡC", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    act( "$n�Ĥ��U���a�ܤU$p�C", ch, obj, NULL, TO_ROOM );
    act( "�A�ܤU�F$p�C"        , ch, obj, NULL ,TO_CHAR );

    message_driver( ch, obj, ACT_WHEN_DRINK );
    obj_cast( obj, ch, ch, NULL );
    extract_obj( obj );

    break;

  case ITEM_DRINK_CON:

    if ( obj->value[1] <= 0 )
    {
      act( "�w�g�S���F��b$p�̭��C", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( !( pLiq = liq_lookup( obj->value[2] ) ) )
    {
      act( "�w�g�S���F��b$p�̭��C", ch, obj, NULL, TO_CHAR );
      mudlog( LOG_DEBUG, "do_drink: �S���s�� %d ���G��.", obj->value[2] );
      RETURN_NULL();
    }

    act( "$n�q$p��$T�C", ch, obj, pLiq->name, TO_ALL );

    message_driver( ch, obj, ACT_WHEN_DRINK );
    amount = UMIN( number_range( 3 , 10 ) , obj->value[1] );

    gain_condition( ch, COND_DRUNK , amount * pLiq->drunk  );
    gain_condition( ch, COND_FULL  , amount * pLiq->full   );
    gain_condition( ch, COND_THIRST, amount * pLiq->thirst );

    if ( !IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK]  > 10 )
      send_to_char( "�A�w�g�K�F�C\n\r", ch );

    if ( !IS_NPC(ch) && ch->pcdata->condition[COND_FULL]   > 40 )
      send_to_char( "�A�鹡�F�C\n\r", ch );

    if ( !IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] > 40 )
      send_to_char( "�A�w�g�����F�C\n\r", ch );

    if ( obj->value[3] != 0 )
    {
      act( "$n�ݰ_�Ӧn���O���r�F�C", ch, NULL, NULL, TO_ROOM );
      send_to_char( "�V�F�M�Aı�o�������ӹ�l�C\n\r", ch );
      af.type      = SLOT_POISON;
      af.duration  = 3 * amount;
      af.location  = APPLY_NONE;
      af.modifier  = 0;
      af.level     = obj->level;
      affect_join( ch, &af );
    }

    if ( ( obj->value[1] -= amount ) <= 0 )
    {
      act( "���n�F�M$p��M�����F�C", ch, obj, NULL, TO_CHAR );
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
    send_to_char( "�A�n������F�赹����H�S\n\r", ch );
    RETURN_NULL();
  }

  if ( !( obj = get_obj_carry( ch, arg1 ) ) )
  {
    act( "�A�䤣�쨺�ӪF�� $2$T$0�C", ch, NULL, arg1, TO_CHAR );
    RETURN_NULL();
  }

  if ( !( victim = get_char_room( ch, arg2 ) ) )
  {
    send_to_char( "�A�n��������H�O�S\n\r", ch );
    RETURN_NULL();
  }

  if ( ( IS_NPC( victim ) || !victim->pcdata || !is_couple( ch, victim ) )
    && !IS_IMMORTAL( ch ) )
  {
    send_to_char( "�A�u�����F�赹�A���t���T\n\r", ch );
    RETURN_NULL();
  }

  if ( ch == victim )
  {
    send_to_char( "�A�L�k�����ۤv�Y�F��T\n\r", ch );
    RETURN_NULL();
  }

  if ( victim->position != POS_STANDING && victim->position != POS_RESTING )
  {
    act( "�A�{�b�L�k��$N�F��Y�T", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  /* ���~�O�_�����r */
  if ( poison_char( ch, obj ) ) RETURN_NULL();

  switch( obj->item_type )
  {
  default:
    send_to_char( "�A�u�����ĩάO�������A���t���T\n\r", ch );
    RETURN_NULL();

  case ITEM_PILL:
    break;

  case ITEM_FOOD:

    if ( !IS_NPC( victim ) && victim->pcdata->condition[COND_FULL] > 40 )
    {
      act( "$N�w�g�Y���F�M�A�]�Y���U�F��F�T", ch, NULL, victim, TO_CHAR );
      act( "$n�������A�Y$p�T"                , ch, obj , victim, TO_VICT );
      RETURN_NULL();
    }

    break;
  }

  act( "$n���F$p����$N�M�n�����֡T", ch, obj, victim, TO_NOTVICT );
  act( "�A��$p��$N�Y�C"            , ch, obj, victim, TO_CHAR    );
  act( "$N���A�Y$p�C"              , ch, obj, victim, TO_VICT    );

  if ( victim->mask )
    act( "���ӹ�l�M�A�����e�N���G�Q$N���ѯ}�F�T", victim, NULL, ch, TO_CHAR );

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
        act( "$n�j�f�a�Y���F$p�M�٤@�Ƥ��������ҼˡC"
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
        mudlog( LOG_DEBUG, "Do_eat: �L�k���ͪ��~ %d.", obj->value[4] );
        RETURN_NULL();
      }

      if ( obj->value[1] <= obj->value[2] || obj->value[1] <= 0 )
      {
        act( "$n�j�f�a�Y���F$p�M�٤@�Ƥ��������ҼˡC"
          , victim, obj, NULL, TO_ALL );

        act( "$n���L���o�������W�@�١M��M����W�h�F$p�C"
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
        send_to_char( "�A�w�g���j�F�C\n\r", victim );

      else if ( victim->pcdata->condition[COND_FULL] > 40 )
        send_to_char( "�A�Y���F�C\n\r", victim );
    }

    if ( obj->value[3] != 0 )
    {
      act( "$n�ݰ_�Ӧn�����r�F�T", victim, NULL, NULL, TO_ROOM );
      send_to_char( "�D�F�M�Aı�o�������ӹ�l�C\n\r", victim );

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
    act( "�A�䤣�쨺�ӪF�� $2$T$0�C", ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  /* ���~�O�_�����r */
  if ( poison_char( ch, obj ) ) RETURN_NULL();

  if ( !IS_IMMORTAL( ch ) )
  {
    switch( obj->item_type )
    {
    default:
      act( "$p���O���ӦY���M�A�O�K�G�ܡS", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();

    case ITEM_PILL:

      if ( ch->pcdata && ch->pcdata->pills >= PillsPerDay )
      {
        act( "$p�Y������h�M���Ȥ��r�ܡS", ch, obj, NULL, TO_CHAR );
        RETURN_NULL();
      }

      break;

    case ITEM_FOOD:

      if ( !IS_NPC( ch ) && ch->pcdata->condition[COND_FULL] > 40 )
      {
        send_to_char( "�A�w�g�Y���F�M�Y���U�F�C\n\r", ch );
        RETURN_NULL();
      }
      break;
    }
  }

  act( "$n���_$p�i�f�j�Z�M���M��ı�L�H���s�b�C", ch, obj, NULL, TO_ROOM );
  act( "�A�Y�F�@�f$p�C"    , ch, obj, NULL, TO_CHAR );

  message_driver( ch, obj, ACT_WHEN_EAT );

  switch ( obj->item_type )
  {
  default:
    act( "�A��$p���������F�T", ch, obj, NULL, TO_CHAR );
    break;

  case ITEM_FOOD:

    if ( obj->value[4] == 0 )
    {
      if ( obj->value[1] < obj->value[2] || obj->value[1] <= 0 )
      {
        act( "$n�j�f�a�Y���F$p�M�٤@�Ƥ��������ҼˡC"
          , ch, obj, NULL, TO_ALL );

      } else DigestObj = FALSE;
    }
    else
    {
      if ( !( obj_cr = create_object(get_obj_index(obj->value[4]), -1 ) ) )
      {
        mudlog( LOG_DEBUG, "Do_eat: �L�k���ͪ��~ %d.", obj->value[4] );
        RETURN_NULL();
      }

      if ( obj->value[1] <= obj->value[2] || obj->value[1] <= 0 )
      {
        act( "$n�j�f�a�Y���F$p�M�٤@�Ƥ��������ҼˡC"
          , ch, obj, NULL, TO_ALL );

        act( "$n���L���o�������W�@�١M��M����W�h�F $p�C"
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
        send_to_char( "�A�w�g���j�F�C\n\r", ch );

      else if ( ch->pcdata->condition[COND_FULL] > 40 )
        send_to_char( "�A�Y���F�C\n\r", ch );
    }

    if ( obj->value[3] != 0 )
    {
      act( "$n�ݰ_�Ӧn�����r�F�T", ch, NULL, NULL, TO_ROOM );
      send_to_char( "�D�F�M�Aı�o�������ӹ�l�C\n\r", ch );

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
    if ( fShow ) act( "�A���ಾ��$p�o���˳ơC", ch, obj, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  unequip_char( ch, obj, TRUE );

  if ( fShow ) act( "$n�Ȱ��˳�$p�C", ch, obj, NULL, TO_ALL );
  RETURN( TRUE );
}

bool wear_obj( CHAR_DATA * ch, OBJ_DATA * obj, bool fReplace, bool fShow )
{
  SITUS_DATA * pSitus;
  SITUS_DATA * zSitus;

  PUSH_FUNCTION( "wear_obj" );

  if ( !ch || !obj )
  {
    mudlog( LOG_DEBUG, "wear_obj: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( ch->level < obj->level )
  {
    if ( fShow )
    {
      act( "�A���ݵ���$i�H�W�~��ϥγo���~�C", ch, &obj->level, NULL, TO_CHAR );
      act( "$n���լ�W$p�M���O���m�����M�L�k��W�C", ch, obj, NULL, TO_ROOM );
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

      mudlog( LOG_DEBUG, "wear_obj: �Ӧh%s�˳�.", pSitus->situs_name );
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
    act( "�藍�_�M�A�L�k�˳Ʃδ���$p�C", ch, obj, NULL, TO_CHAR );

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

    if ( !bWear ) send_to_char( "�藍�_�M�A�S�����󪫫~�i�H�˳ƤF�T\n\r", ch );
  }

  else
  {
    if ( !( obj = get_obj_carry( ch, arg ) ) )
    {
      act( "�A�S�����󪫫~ $2$T$0 �C�C", ch, NULL, arg, TO_CHAR );
      RETURN_NULL();
    }

    /* ���~�O�_�����r */
    if ( poison_char( ch, obj ) ) RETURN_NULL();

    if ( obj->item_type == ITEM_MAGICSTONE )
    {
      act( "$p�O����˳ƪ��T", ch, obj, NULL, TO_CHAR );
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

    if ( !bRemove ) send_to_char( "�藍�_�M�A�S�����󪫫~�i�H�����F�T\n\r", ch );
    RETURN_NULL();
  }

  if ( !( obj = get_obj_wear( ch, arg ) ) )
  {
    act( "�A�èS�����󪫫~ $2$T$0�C", ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  remove_obj( ch, obj->wear_loc, TRUE, TRUE );

  RETURN_NULL();
}

/* �^�m�X���餧��N�i�H�ٶ� */
FUNCTION( do_crave )
{
  PUSH_FUNCTION( "do_crave" );

  if ( IS_NPC( ch ) ) RETURN_NULL();
  if ( !ch->pcdata || !ch->pcdata->corpse || !ch->pcdata->corpse->in_room )
  {
    send_to_char( "�S���o�{�A������M�S����k�ٵ��A�C\n\r" , ch );
    RETURN_NULL();
  }

  if ( ch->in_room == ch->pcdata->corpse->in_room )
  {
    send_to_char( "�A�w�g���^�A������F�C\n\r", ch );
    RETURN_NULL();
  }

  char_from_room( ch );
  char_to_room( ch , ch->pcdata->corpse->in_room );

  send_to_char( "\n\r\n\r\a\a\a�n�a�M������ٵ��A�M�����`�|�L�h�M"
    "�ЧA�~��V�O�İ��a�T\n\r"
    "�p�G�n�^���Ӫ��@�ɡM�N����^�m( \e[1;33msac corpse \e[0m)�Y�i�C\n\r"
    "���n�ѤF���^(\e[1;32msearch\e[0m)����̪��˳Ƴ�T\n\r\n\r"
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
    act( "$n�n��$e�ۤv�^�m���ѯ��M�L�O���O�ƤF�ڡC", ch, NULL, NULL, TO_ROOM );
    send_to_char( "�ѯ����@�U�N�|�Ҽ{�A���^�m�C\n\r", ch );
    RETURN_NULL();
  }

  if ( !( obj = get_obj_list( ch, arg, ch->in_room->contents ) ) )
  {
    act( "�A�䤣�쨺�󪫫~ $2$T$0�C", ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  if ( obj->Takeable == FALSE || !str_cmp( obj->name, "turd" ) )
  {
    act( "�ѯ������w��$p�o�˪��^�m�C", ch, obj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( obj->item_type == ITEM_CORPSE_PC )
  {
    if ( IS_NPC( ch ) || !ch->pcdata || ch->pcdata->corpse != obj )
    {
      act( "$p���O�A������M�A����^�m���C", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    message_driver( ch, obj, ACT_WHEN_SACRIFICE );

    act( "�ѤW���U�@�D������~�M�ѯ���$n�ٶ��F�T" , ch, NULL, NULL, TO_ALL );

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
    act( "�ѩ�A�@�۪��^�m�M�U�઺�ѯ��絹�A$i������C"
      , ch, &amount, NULL, TO_CHAR );

    gold_to_char( ch, amount );
  }

  else
  {
    send_to_char( "�]���A���`�겣�w�F�W���M�L�k�A��������T\n\r", ch );
  }

  act( "$n�N$p�@�ۦa�^�m�����j�����C", ch, obj, NULL, TO_ROOM );
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
    send_to_char( "�A�n�\\Ū�����D�S\n\r", ch );
    RETURN_NULL();
  }

  if ( !( pObj = get_obj_carry( ch, arg ) ) )
  {
    act( "�A���W�S���o�� $2$T$0 ���D�T", ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  if ( pObj->item_type != ITEM_MYSTERY )
  {
    act( "$p�ݰ_�Ӥ����O�����D�T", ch, pObj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( ( adept = pObj->value[1] ) <= 0
    || adept > 100
    || ( slot = pObj->value[0] ) < 0
    || slot >= MAX_SKILL
    || !( pSkill = get_skill( slot ) ) )
  {
    act( "�A�o��$p�O���O�L���F�T", ch, pObj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( ch->skill[slot] > 0 )
  {
    act( "�o��$p�M$n�ݰ_�ӫܲ������T", ch, pObj, NULL, TO_ALL );
    RETURN_NULL();
  }

  if ( !check_skill_restrict( ch, pSkill, FALSE )
     || !check_valid_rating( ch, pSkill, FALSE ) )
  {
    act( "�o��$p�A���A�X�ǡM�ǤF�]�S���ΡC", ch, pObj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( !ch->class || learn_count( ch ) > get_learnable( ch ) )
  {
    send_to_char( "�A�����U�N�o��j�M�O������h�ۦ��T\n\r", ch );
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
    act( "�A�ڥ��N���A�X��$w�M���n�F�T", ch, pSkill, NULL, TO_CHAR );
    RETURN_NULL();
  }

  message_driver( ch, pObj, ACT_WHEN_STUDY );

  act( "���H$n�q$p��|��$2$W$0�o�@���\\�ҡM���O$p�C�C�Ƨ@�@�}�C�Ϯ����F�T"
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
    send_to_char( "�A�n�⨺�ӪF��˶�쨺�̥h�O�S\n\r", ch );
    RETURN_NULL();
  }

  if ( !( pContainer = get_obj_carry( ch, arg1 ) ) )
  {
    act( "�藍�_�M�A�S�����ӭI�U $2$T$0�T", ch, NULL, arg1, TO_CHAR );
    RETURN_NULL();
  }

  if ( pContainer->item_type != ITEM_BACKBAG )
  {
    act( "$p���O�@�حI�U�M�L�k�˪F��T", ch, pContainer, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( !can_drop_obj( ch, pContainer ) )
  {
    act( "�A������$p�C", ch, pContainer, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( !( pArrow = get_obj_carry( ch, arg2 ) ) )
  {
    act( "�藍�_�M�A�S�����ӪZ�� $2$T$0 �Ӹ˶�T", ch, NULL, arg1, TO_CHAR );
    RETURN_NULL();
  }

  if ( !pArrow->Loadable )
  {
    act( "$p�O���i�˶񪺡T", ch, pArrow, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( pContainer->value[2] > 0
    && pContainer->value[2] != pArrow->pIndexData->vnum )
  {
    act( "$p�̭��w�g�ˤF��L�F��F�T", ch, pContainer, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( pContainer->value[0] <= pContainer->value[1] )
  {
    act( "$p�w�g�˺��F$P�T", ch, pContainer, pArrow, TO_CHAR );
    RETURN_NULL();
  }

  pContainer->value[1]++;
  pContainer->value[2] = pArrow->pIndexData->vnum;
  extract_obj( pArrow );

  act( "$n��$p�˨�$P�̭��h�T", ch, pArrow, pContainer, TO_ALL );

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
    send_to_char( "�A�n�񤰻�Ϥ��S\n\r", ch );
    RETURN_NULL();
  }

  if ( !ch->in_room )
  {
    send_to_char( "�藍�_�M���A�ثe�b���̩O�S\n\r", ch );
    RETURN_NULL();
  }

  if ( !IS_OUTSIDE( ch ) )
  {
    act( "�A�L�k�b�Ǥ�$r�I��Ϥ���!", ch, NULL, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( ch->in_room->area == FoggyArea )
  {
    act( "�ثe$r�@�������ۡM�o�ɩ�Ϥ��O�S���Ϊ�.", ch , NULL, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( ( pMount = get_mount( ch ) ) )
  {
    act( "�A���b$N�W�O�S����k��Ϥ���.", ch, NULL, pMount, TO_CHAR );
    RETURN_NULL();
  }

  if ( !( pFire = get_obj_carry( ch, arg ) ) )
  {
    act( "�䤣��A���Ϥ� $2$T$0 �C�T", ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  /* ���~�O�_�����r */
  if ( poison_char( ch, pFire ) ) RETURN_NULL();

  if ( pFire->item_type != ITEM_FIREWORK )
  {
    act( "$p�ä��O�@�طϤ��M�ҥH����I��T", ch, pFire, NULL, TO_CHAR );
    RETURN_NULL();
  }

  distance = pFire->value[0];
  if ( weather_info.sky == SKY_RAINING ) distance--;

  if ( distance <= 0 || distance >= MAX_SCAN_DEPTH )
  {
    act( "$p�ݨӬO�L�����Ϥ��M�o�ӨS�ΤF�T", ch, pFire, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( number_range( 1, 10000 ) <= pFire->value[1] )
  {
    act( "��M�M$p�b�٨S�ɪūe�N�w�g�z���M$n�Q������׼ҽk�T���ְe��a�T"
      , ch, pFire, NULL, TO_ALL );

    act( "�A��$p�z�����������F�C" , ch, pFire, NULL, TO_CHAR );
    extract_obj( pFire );

    ch->hit  = UMAX( 1, ch->hit  / 2 );
    ch->mana = UMAX( 1, ch->mana / 2 );
    ch->move = UMAX( 1, ch->move / 2 );

    RETURN_NULL();
  }

  act( "$n�I��$p�M�ѪŰ��B�X�{���ꪺ��m�T", ch, pFire, NULL, TO_ALL );

  clean_scan();
  scan_room( ch->in_room->vnum, distance, -1 );
  print_scan( ch, pFire );
  clean_scan();

  act( "�A��$p���������F�C" , ch, pFire, NULL, TO_CHAR );
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
    act( "$n�w�g�S���k�O�i�H����o�@���Ǫ��F�M�ҥH$n�񤣥X�ӡT"
      , ch, NULL, NULL, TO_CHAR );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    send_to_char( "�A�Q�n���񨺭ӧ����O�S\n\r", ch );
    RETURN_NULL();
  }

  if ( !( pUrn = get_obj_carry( ch, arg ) ) )
  {
    act( "�A���W�S���o������ $2$T$0 �C�T", ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  /* ���~�O�_�����r */
  if ( poison_char( ch, pUrn ) ) RETURN_NULL();

  if ( pUrn->item_type != ITEM_URN )
  {
    act( "$p�ä��O�����M�ҥH��������ʦL�T", ch, pUrn, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( ch->position != POS_STANDING )
  {
    send_to_char( "�A����U���䪺�u�@�a�T\n\r", ch );
    RETURN_NULL();
  }

  if ( room_is_private( ch->in_room ) || ch->in_room->NoMob )
  {
    act( "$r����ӭ��F�M�ҥH�L�k����ʦL", ch, NULL, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( is_platform( ch->in_room ) || ch->in_room->Sail )
  {
    act( "�藍�_�M�A����b$r�o������ʦL�T", ch, NULL, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( !( pMobIndex = get_mob_index( pUrn->value[0] ) ) )
  {
    act( "�藍�_�M�A��$p�S���Q�ʦL�L�T", ch, pUrn, NULL, TO_CHAR );
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

  act( "$n��W��$p��M�_�X�@�}�C�ϡM���[�M�v���{�X$N�����v�M"
       "�H��M$p�]���������F�T"
    , ch, pUrn, victim, TO_CHAR );

  act( "$n��W��$p��M�_�X�@�}�C�ϡM���[�M�v���{�X$N�����v�T"
    , ch, pUrn, victim, TO_ROOM );

  ch->mana -= get_curr_mana( ch ) / 5;
  act( "$n���k�O���ӥh����Ǫ��F�T", ch, NULL, NULL, TO_CHAR );

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
    act( "�A�S���i�H��w�����b $2$T$0�C", ch, NULL, arg1, TO_CHAR );
    RETURN_NULL();
  }

  /* ���~�O�_�����r */
  if ( poison_char( ch, scroll ) ) RETURN_NULL();

  if ( scroll->item_type != ITEM_SCROLL )
  {
    send_to_char( "�A�u��Ψ��b�����~�Ӱ�w�C\n\r", ch );
    RETURN_NULL();
  }

  if ( arg2[0] == '\x0' )
  {
    send_to_char( "�A�n�p��I�k�S\n\r", ch );
    RETURN_NULL();
  }

  if ( !( obj = get_obj_here( ch, arg2 ) ) )
  {
    act( "�A�䤣�쨺�Ӫ��~ $2$T$0�C", ch, NULL, arg2, TO_CHAR );
    RETURN_NULL();
  }

  if ( obj->item_type == ITEM_SCROLL )
  {
    act( "�A�����$p�Ӱ�w���b�T", ch, obj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  act( "$n��w$p�C", ch, scroll, NULL, TO_ALL );

  message_driver( ch, scroll, ACT_WHEN_RECITE );
  obj_cast( scroll, ch, NULL, obj );

  act( "�A��$p���������F�C", ch, scroll, NULL, TO_CHAR );
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
    mudlog( LOG_DEBUG, "find_keeper: �ӷ����~." );
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

      act( "�A�b$r�䤣��ө��M�ݰݤH�a�C", ch, NULL, NULL, TO_CHAR );
      break;

    case SHOP_SMITH:
      send_to_char( "�䤣�쥴�K�Q�C�M�ݰݧO�H�a�C\n\r", ch );
      break;

    case SHOP_MERCENARY:
      send_to_char( "�o�̨S���ħL�ҭC�M�ݰݧO�H�a�T\n\r", ch );
      break;

    case SHOP_COPER:
      act( "�A�b$r�O�R���차���M�бЧO�H�a�T", ch, NULL, NULL, TO_CHAR );
      break;
    }

    RETURN( NULL );
  }

  if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_KILLER ) && !IS_IMMORTAL( ch ) )
  {
    do_say( keeper, "�ѹ�i�D�A�n�F�T�o�̤��w�����T" );
    sprintf( buf, "���H�� %s �ӧڳo�̳�M���֨Ӯ��L�T", mob_name( keeper, ch ) );
    do_shout( keeper, buf );
    RETURN( NULL );
  }

  switch( keeper->position )
  {
  default:
  case POS_DEAD:
    act( "$N�L�O���O�ƤF�S", ch, NULL, keeper, TO_CHAR );
    RETURN( NULL );

  case POS_STANDING:
    break;

  case POS_FIGHTING:
    act( "�A�S�ݨ�$N���ܡS", ch, NULL, keeper, TO_CHAR );
    RETURN( NULL );

  case POS_RESTING:
    act( "$N���b�𮧫��M���֥s�L���ͷN�a�T", ch, NULL, keeper, TO_CHAR );
    RETURN( NULL );

  case POS_SLEEPING:
    act( "$N���M�b���ڡM�s�L�_�ɰ��ͷN�a�T", ch, NULL, keeper, TO_CHAR );
    RETURN( NULL );
  }

  if ( !IS_NPC(ch) && IS_SET( ch->act, PLR_THIEF ) && !IS_IMMORTAL( ch ) )
  {
    do_say( keeper, "�o�̤��w��p���T" );
    sprintf( buf, "�Ѹ� %s �ӳo�̳�M���֨Ӯ��L�T", mob_name( keeper, ch ) );
    do_shout( keeper, buf );
    RETURN( NULL );
  }

  if ( !can_see( keeper, ch ) && !IS_IMMORTAL( ch ) )
  {
    do_say( keeper, "�藍�_�M�ڤ���ڬݤ������H���ͷN�C" );
    RETURN( NULL );
  }

  if ( time_info.hour < pShop->open_hour )
  {
    do_say( keeper, "��p�M�б��I�A�ӡC" );
    RETURN( NULL );
  }

  if ( time_info.hour > pShop->close_hour )
  {
    do_say( keeper, "��p�M����Ц��C" );
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
    mudlog( LOG_DEBUG, "get_cost: �ӷ����~." );
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

  /* ���������l�`���� */
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
    send_to_char( "�A���`�겣�ӥi�ȤF�M�ҥH�L�k����o�ӫ��O�T\n\r", ch );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg );

  if ( !( keeper = find_keeper( ch, SHOP_STORE ) ) ) RETURN_NULL();

  if ( ch->position == POS_FIGHTING )
  {
    send_to_char( "�A���b�԰��M�L�k�R��F��T\n\r", ch );
    RETURN_NULL();
  }

  if ( arg[0] == '@' )
  {
    if ( arg[1] == '\x0' || !is_number( arg + 1 ) )
    {
      send_to_char( "�A�Q�R�s���ĴX�����~�O�S�Ьd�� buy ���ϥΤ�k�T\n\r", ch );
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
      act( "$n�i�D�A�R�ڨS��o�˪��~�M�Х� list �ݬݥ������S���C"
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
    act( "$n�i�D�A�R�ڨS��o�˪��~�M�Х� list �ݬݥ������S���C"
      , keeper, NULL, ch, TO_VICT );
    ch->reply = keeper;
    RETURN_NULL();
  }

  if ( ch->gold < cost )
  {
    act( "$n�i�D�A�R�A�R���_$p�M���A���������A���a�C"
      , keeper, obj, ch, TO_VICT );
    ch->reply = keeper;
    RETURN_NULL();
  }

  if ( obj->level > ch->level )
  {
    act( "$n�i�D�A�R�A���ŤӧC�M�Ф��n�R$p�C"
      , keeper, obj, ch, TO_VICT );
    ch->reply = keeper;
    RETURN_NULL();
  }

  /* ���~�O�_�����r */
  if ( poison_char( ch, obj ) ) RETURN_NULL();

  if ( ch->carry_number + get_obj_number( obj ) > can_carry_n( ch ) )
  {
    send_to_char( "�A�w�g�a�Ӧh�F��F�C\n\r", ch );
    RETURN_NULL();
  }

  if ( get_carry_weight( ch ) + get_obj_weight( obj ) > can_carry_w( ch ) )
  {
    send_to_char( "�A�����q�w�g�W�X�t���C\n\r", ch );
    RETURN_NULL();
  }

  if ( !IS_NPC( ch ) && excess_filequota( ch ) )
  {
    send_to_char( "�藍�_�M�A���ɮפӤj�M�L�k�A�R����F��F�T\n\r", ch );
    RETURN_NULL();
  }

  act( "$n�@���Ҽ{�a�R�U$p�T", ch, obj, NULL, TO_ALL );

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
    send_to_char( "�A���b�԰��M�L�k�R��F��T\n\r", ch );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg );

  if ( arg[0] && is_number( arg ) )
  {
    if ( ( alevel = atoi( arg ) ) <= 0 || alevel > MAX_LEVEL )
    {
      send_to_char( "��J�����Ť��X�z�T\n\r", ch );
      RETURN_NULL();
    }

    one_argument( argument, arg );

    if ( arg[0] )
    {
      if ( !is_number( arg ) )
      {
        send_to_char( "������J�Ʀr�Ӭd�ߡT\n\r", ch );
        RETURN_NULL();
      }

      if ( ( blevel = atoi( arg ) ) <= 0 || blevel > MAX_LEVEL )
      {
        send_to_char( "��J�����Ť��X�z�T\n\r", ch );
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
        send_to_buffer( "\e[1;33;44m[�s�� ����   ����] ���~�W��      "
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
      act( "�A�Q�A�F�M�b$r�R������󪺪F��C", ch, NULL, NULL, TO_CHAR );
    else
      act( "�A�b$r�R����A�n���F��M�Ш�O���ө��ݬݧa�C"
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
    send_to_char( "�A���`�겣�ӥi�ȤF�M�ҥH�L�k����o�ӫ��O�T\n\r", ch );
    RETURN_NULL();
  }

  one_argument( argument, arg );

  if ( !( keeper = find_keeper( ch, SHOP_STORE ) ) ) RETURN_NULL();

  if ( ch->position == POS_FIGHTING )
  {
    send_to_char( "�A���b�԰��M�L�k�R��F��T\n\r", ch );
    RETURN_NULL();
  }

  if ( !( obj = get_obj_carry( ch, arg ) ) )
  {
    act( "�A�èS������F�� $2$T$0�M�A�ڰڡC", ch , NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  /* ���~�O�_�����r */
  if ( poison_char( ch, obj ) ) RETURN_NULL();


  if ( !can_drop_obj( ch, obj ) )
  {
    act( "�A������$p�M�ҥH�A����楦�C", ch, obj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( obj->address )
  {
    act( "$p�O�n�浹�O�H���H�M���i�H��F�O�C", ch, obj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( ( cost = get_cost( keeper, obj, FALSE ) ) <= 0 )
  {
    act( "$n�ݰ_�ӹ�$p�S���򿳽�M�A�N���F�o���ߧa�T"
      , keeper, obj, ch, TO_VICT );
    RETURN_NULL();
  }

  chinese_number( cost, chinese );
  act( "$n�Եh�a��F$p�T", ch, obj, NULL, TO_ROOM );

  sprintf( buf, "���M���F���M�A�u�n�Եh�H%s�������$p���ηR�F�C", chinese );
  act( smash_act_keyword( buf, "nesp" ) , ch, obj, NULL, TO_CHAR );
  message_driver( ch, obj, ACT_WHEN_SELL );

  if ( !over_scale( ch ) ) gold_to_char( ch, cost );
  gold_from_char( keeper, UMIN( keeper->gold, cost ) );

  if ( obj->item_type == ITEM_TRASH )
  {
    act( "�o��$p�٬O�ᱼ�a�M�U���@��", ch, obj, NULL, TO_CHAR );
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
    send_to_char( "�A���b�԰��M�L�k�R��F��T\n\r", ch );
    RETURN_NULL();
  }

  if ( !( obj = get_obj_carry( ch, arg ) ) )
  {
    act( "�A�èS�����ӪF�� $2$T$0�M�A�A�ڰڡC", ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  /* ���~�O�_�����r */
  if ( poison_char( ch, obj ) ) RETURN_NULL();

  if ( obj->address )
  {
    act( "$p�O�n�浹�O�H���H�M���i�H���Ӧ����C", ch, obj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( !can_drop_obj( ch, obj ) )
  {
    act( "$p�O�A���Ǯa���_�M�ٮ��Ӧ����M�A�O���O�a�ƤF�C", ch, obj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( ( cost = get_cost( keeper, obj, FALSE ) ) <= 0 )
  {
    act( "$n�ݰ_�ӹ�$p�S���򿳽�M�A�N���F�o���ߧa�C"
      , keeper, obj, ch, TO_VICT );
    RETURN_NULL();
  }

  chinese_number( cost, chinese );
  sprintf( buf, "$n�i�D�A�R�u���@�N�X��%s������ӶR�A��$p�C�v", chinese );
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
    mudlog( LOG_DEBUG, "obj_cast: �ʥF�H���ӷ�." );
    RETURN_NULL();
  }

  value = pMagic->value[0];
  type  = pMagic->value[1];
  sn    = pMagic->value[2];
  vnum  = pMagic->pIndexData ? pMagic->pIndexData->vnum : -1;

  /* �p�G���X�p��s�h���Χ@����� */
  if ( type <= 0 ) RETURN_NULL();

  switch( type )
  {
  default:
    mudlog( LOG_DEBUG, "obj_cast: ���~�����A %d.", type );
    RETURN_NULL();

  /* ���� */
  case OBJ_CAST_ALL:

    old = ch->hit;

    ch->hit = UMIN( get_curr_hit( ch ), ch->hit + value );
    ch->hit = UMAX( ch->hit, 0 );

    send_to_char( ch->hit == old
      ? "�A���ͩR�O���G�@�I�]�S���ﵽ�C\n\r" : ( ch->hit > old
        ? "�A���ͩR�O�W�i�F���֡C\n\r"
        : "�A���ͩR�O�ӷl�F���֡C\n\r" ), ch );

    old = ch->mana;

    ch->mana = UMIN( get_curr_mana( ch ), ch->mana + value );
    ch->mana = UMAX( ch->mana, 0 );

    send_to_char( ch->mana == old
      ? "�A���k�O���G�@�I�]�S���ﵽ�C\n\r" : ( ch->mana > old
        ? "�A���k�O�W�i�F���֡C\n\r"
        : "�A���k�O�ӷl�F���֡C\n\r" ), ch );

    old = ch->move;

    ch->move = UMIN( get_curr_move( ch ), ch->move + value );
    ch->move = UMAX( ch->move, 0 );

    send_to_char( ch->move == old
      ? "�A����O���G�@�I�]�S���ﵽ�C\n\r" : ( ch->move > old
        ? "�A����O�W�i�F���֡C\n\r"
        : "�A����O�ӷl�F���֡C\n\r" ), ch );

    break;

  /* �ͩR�O */
  case OBJ_CAST_HIT:

    old = ch->hit;

    ch->hit = UMIN( get_curr_hit( ch ), ch->hit + value );
    ch->hit = UMAX( ch->hit, 0 );

    send_to_char( ch->hit == old
      ? "�A���ͩR�O���G�@�I�]�S���ﵽ�C\n\r" : ( ch->hit > old
        ? "�A���ͩR�O�W�i�F���֡C\n\r"
        : "�A���ͩR�O�ӷl�F���֡C\n\r" ), ch );

    break;

  /* �k�O */
  case OBJ_CAST_MANA:

    old = ch->mana;

    ch->mana = UMIN( get_curr_mana( ch ), ch->mana + value );
    ch->mana = UMAX( ch->mana, 0 );

    send_to_char( ch->mana == old
      ? "�A���k�O���G�@�I�]�S���ﵽ�C\n\r" : ( ch->mana > old
        ? "�A���k�O�W�i�F���֡C\n\r"
        : "�A���k�O�ӷl�F���֡C\n\r" ), ch );

    break;

  /* ���ʤO */
  case OBJ_CAST_MOVE:

    old = ch->move;

    ch->move = UMIN( get_curr_move( ch ), ch->move + value );
    ch->move = UMAX( ch->move, 0 );

    send_to_char( ch->move == old
      ? "�A����O���G�@�I�]�S���ﵽ�C\n\r" : ( ch->move > old
        ? "�A����O�W�i�F���֡C\n\r"
        : "�A����O�ӷl�F���֡C\n\r" ), ch );

    break;

  /* �ﹳ�O���~ */
  case OBJ_CAST_IDENT:    /* Ų�w */
  case OBJ_DETECT_POISON: /* �����r�� */

    if ( !obj )
    {
      mudlog( LOG_DEBUG, "obj_cast: �ʥF�ӷ�." );
      RETURN_NULL();
    }

    if ( !( pSkill = get_skill( sn ) ) )
    {
      mudlog( LOG_DEBUG, "obj_cast: ���~�s�� %d �䤣��k�N�s�� %d.", vnum, sn );
      RETURN_NULL();
    }

    if ( !pSkill->function )
    {
      mudlog( LOG_DEBUG, "obj_cast: ���~�s�� %d ��ƿ��~.", vnum );
      RETURN_NULL();
    }

    /* �I�s��� */
    ( *pSkill->function ) ( pSkill->slot, value, ch, obj );
    break;

  /* �ﹳ�O�H */
  case OBJ_CURE_POISON:   /* �Ѭr     */
  case OBJ_DETECT_INVS:   /* �������� */
  case OBJ_CAST_FLY:      /* ����N   */
  case OBJ_CURE_BLIND:    /* �v������ */
  case OBJ_GIANT_STR:     /* ���H���O */
  case OBJ_DETECT_HIDE:   /* �����ð� */
  case OBJ_SNEAK:         /* ���     */
  case OBJ_DETECT_EVIL:   /* �������c */
  case OBJ_CHANGE_SEX:    /* �ܩʳN   */
  case OBJ_DETECT_MAGIC:  /* �����]�k */
  case OBJ_FAERIE_FOG:    /* �{���N   */
  case OBJ_GATE:          /* �۰��N   */
  case OBJ_FIXITY:        /* �w����   */
  case OBJ_DETECT_MASK:   /* �������e */

    if ( !victim )
    {
      mudlog( LOG_DEBUG, "obj_cast: ���~�s�� %d �ӷ����~.", vnum );
      RETURN_NULL();
    }

    if ( !( pSkill = get_skill( sn ) ) )
    {
      mudlog( LOG_DEBUG, "obj_cast: ���~�s�� %d �䤣��k�N�s�� %d.", vnum, sn );
      RETURN_NULL();
    }

    if ( !pSkill->function )
    {
      mudlog( LOG_DEBUG, "obj_cast: ���~�s�� %d ��ƿ��~.", vnum );
      RETURN_NULL();
    }

    /* �I�s��� */
    ( *pSkill->function ) ( pSkill->slot, value, ch, victim );
    break;
  }

  /* ���q */
  if ( pMagic->value[3] > 0
    && verify_char( ch )
    && !is_dead( ch )
    && ch->pcdata )
  {
    ch->pcdata->dosage += pMagic->value[3];
    if ( ch->pcdata->dosage >= DosageLimit )
    {
      send_to_char( "�]���A�ιL�h���Ī��M�A�o�{�A��ߤ����a�_�N���T\n\r", ch );
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
    mudlog( LOG_DEBUG, "message_driver: �ӷ������T." );
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
    send_to_char( "�A���b�԰��M�L�k�R���T\n\r", ch );
    RETURN_NULL();
  }

  if ( !keeper->pIndexData || !( pShop = keeper->pIndexData->pShop ) )
  {
    act( "$N�ثe�O����氨���T", ch, NULL, keeper, TO_CHAR );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    send_to_char( "�o�ӫ��O�������ѼơM�п�J trade /? �d�ߡT\n\r", ch );
  }

  else if ( !str_prefix( arg, "list" ) )
  {
    send_to_buffer( "�A�b�o�̥i�H�R�쪺���Ǧ��R\n\r"
      "\e[1;33;44m���� ���� �O  �� ��  �O �ͩR�O �W  ��                   "
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
      send_to_char( "�A�b���򰭦a��ڡS\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->mount )
    {
      act( "�A���w�g�b�M$N�F�M�������٨ӶR���O�S", ch, NULL, ch->mount, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch->position != POS_STANDING )
    {
      send_to_char( "�A�������A���Ƨa�M���M�������R�檺�T\n\r", ch );
      RETURN_NULL();
    }

    /* ���঳����N */
    if ( is_affected( ch, SLOT_FLY ) )
    {
      send_to_char( "�A���}���a�M�L�k�ʶR���ǡC\n\r", ch );
      RETURN_NULL();
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\x0' )
    {
      send_to_char( "�A�n�R���@�ǰ��O�S\n\r", ch );
      RETURN_NULL();
    }

    if ( !is_number( arg ) )
    {
      send_to_char( "�ХH�Ʀr�N��A�n�R�����ǡT\n\r", ch );
      RETURN_NULL();
    }

    if ( ( count = atoi( arg ) ) <= 0 || count > MAX_TRADE )
    {
      send_to_char( "�A�����Ǥ����M�Э��s��J�T\n\r", ch );
      RETURN_NULL();
    }

    if ( !( pIndex = get_mob_index( pShop->buy_type[count - 1] ) )
      || pIndex->tractable <= 0 )
    {
      act( "�b$r�O�R���쨺�ǰ�����T", ch, NULL, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch->gold < ( cost = hire_cost( pIndex, pShop, SHOP_BUY ) ) )
    {
      send_to_char( "�A���W���������R�o�ǰ��T\n\r", ch );
      RETURN_NULL();
    }

    if ( !( horse = create_mobile( pIndex, -1 ) ) )
    {
      send_to_char( "�藍�_�M����䤣�쨺�ǰ���T\n\r", ch );
      RETURN_NULL();
    }

    /* ���������a������ */
    for ( rch = ch->in_room->people; rch; rch = zch )
    {
      zch = rch->next_in_room;
      if ( rch->tractable > 0 && !rch->mount_by && !rch->fighting )
      {
        act( "$n���D�R�u$N�n���O�@�ǳ����M�ڴN�⥦�o�^�ӹ��A�a�T�v"
          , keeper, NULL, rch, TO_ROOM );

        extract_char( rch, TRUE );
      }
    }

    chinese_number( cost, buf );
    char_to_room( horse, ch->in_room );

    ch->mount       = horse;
    horse->mount_by = ch;

    act( "$n��F$t��Ȥl�V����R�F�@��$N�M����N�M�F�W�h�F�C"
      , ch, buf, horse, TO_ALL );

    gold_from_char( ch, cost );
  }

  else if ( !str_prefix( arg, "sell" ) )
  {
    if ( !( horse = ch->mount ) )
    {
      send_to_char( "�A�S���M�۰��M����A�����O�S\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->position != POS_STANDING || horse->position != POS_STANDING )
    {
      send_to_char( "�A�������A�a�M���A���Ŧb��a�T\n\r", ch );
      RETURN_NULL();
    }

    if ( !( pIndex = horse->pIndexData ) || horse->tractable <= 0 )
    {
      act( "$N�ݰ_�Ӥ��ӹ��O�@�ǧ��M�T", ch, NULL, horse, TO_CHAR );
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

        act( "$n�o�F�@��$N�M���ӵ��浹����T", ch, NULL, horse, TO_ALL );

        act( "�A��$N�@�@��F$t��Ȥl�C", ch, buf, horse, TO_CHAR );
        act( "����M$n�N��$N�o��᭱�h�j�ۤF�C", keeper, NULL, horse, TO_ROOM );

        extract_char( horse, TRUE );
        RETURN_NULL();
      }
    }

    act( "$N�O���򰭡M�ڬO���i��R�A���o�Ӱ��F��T", ch, NULL, horse, TO_CHAR );
  }

  else
  {
    send_to_char( "�Ѽƿ��~�M�п�J trade /? �d�ߡT\n\r", ch );
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
    send_to_char( "�A���Ѽƿ��~�M�Ьd�� rename ���Ϊk�C\n\r", ch );
    RETURN_NULL();

  case '1':

    for ( sigil = ch->carrying; sigil; sigil = sigil->next_content )
      if ( sigil->item_type == ITEM_SIGIL ) break;

    if ( !sigil )
    {
      send_to_char( "�A���W�S���i�H��W�r���ũG�M�R�i�ӥΥΧa�T\n\r", ch );
      RETURN_NULL();
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\x0' )
    {
      send_to_char( "�A�n�墨�ӪF�誺�W�٩O�S\n\r", ch );
      RETURN_NULL();
    }

    if ( !( obj = get_obj_carry( ch, arg ) ) )
    {
      act( "�A���W�S���o�Ӫ��~ $2$T$0 ��M���ݬݲM���T", ch, NULL, arg, TO_CHAR );
      RETURN_NULL();
    }

    if ( obj == sigil )
    {
      act( "�A���ள$p�ӧ�ũG���W�r�աT", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    argument = one_argument( argument, arg );

    if ( str_len( arg ) <= 3 )
    {
      act( "�A�n��$p�令����W�٩O�S", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    arg[MAX_NAME_LENGTH] = '\x0';

    if ( check_cname_xname( arg ) )
    {
      send_to_char( "�o�ӦW�r���n�a�T���ӦW�r�ݬݡC\n\r", ch );
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

    act( "$n���s��$p�R�W��$T�T", ch, obj, buf, TO_ALL );

    free_string( obj->cname );
    obj->cname = str_dup( buf );

    act( "�A���W��$p�]�H�ۮ����F�T", ch, sigil, NULL, TO_CHAR );

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
    send_to_char( "�A�n�b���̮Ѽg�A����r�O�S\n\r", ch );
    RETURN_NULL();
  }

  if ( !( vellum = get_obj_carry( ch, arg ) ) )
  {
    act( "�藍�_�M�A�n���S�����i�� $2$T$0 �i�H�Ѽg�C�T", ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  if ( poison_char( ch, vellum ) ) RETURN_NULL();

  if ( vellum->item_type != ITEM_VELLUM )
  {
    act( "�ݨ�$p�O�L�k�g�W����F�誺�T", ch, vellum, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( argument[0] == '\x0' )
  {
    act( "�A�n�b$p�W���g���ǪF��O�S", ch, vellum, NULL, TO_CHAR );
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
    act( "�ݨ�$p�O�L�k�A�g����F��U�h�F�T", ch, vellum, NULL, TO_CHAR );
    RETURN_NULL();
  }

  sprintf( buf, "%s%s\n\r", vellum->description, buf1 );
  free_string( vellum->description );
  vellum->description = str_dup( buf );

  act( "$n�b$p�g�F�ǪF��M�d���n�O������K��T", ch, vellum, NULL, TO_ALL );

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
    send_to_char( "�A�ثe�����A���A�X�Ķ����~�C\n\r", ch );
    RETURN_NULL();
  }

  if ( !( pMine = pRoom->mine )
    || !( pMineral = pMine->mineral )
    || !( pIndex = pMineral->mineral ) )
  {
    act( "$n���Ԧa�b�o�z�}���M������]�S���T", ch, NULL, NULL, TO_ALL );
    RETURN_NULL();
  }

  pObj = create_object( pIndex, -1 );
  obj_to_room( pObj, pRoom );

  act( "$n���Ԧa�b�o�z�}���M�ש�Ķ���F$p�T", ch, pObj, NULL, TO_ALL );

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

    if ( !zMine ) mudlog( LOG_DEBUG , "do_pick: ���������q��." );
  }

  free_struct( pMine, STRUCT_MINE_DATA );
  RETURN_NULL();
}

bool can_store( OBJ_DATA * obj )
{
  PUSH_FUNCTION( "can_store" );

  if ( !obj )
  {
    mudlog( LOG_DEBUG, "can_store: �ʥF�ӷ�." );
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

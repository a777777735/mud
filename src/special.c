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

DECLARE_SPEC_FUN( spec_thief        );
DECLARE_SPEC_FUN( spec_new_doctor   );
DECLARE_SPEC_FUN( spec_get_button   );

SPEC_FUN * spec_lookup( const char * name )
{
  PUSH_FUNCTION( "spec_lookup" );
  if ( !str_cmp( name, "spec_thief"        ) ) RETURN( spec_thief        );
  if ( !str_cmp( name, "spec_new_doctor"   ) ) RETURN( spec_new_doctor   );
  RETURN( NULL );
}

bool spec_new_doctor( CHAR_DATA * ch )
{
  CHAR_DATA * victim;
  CHAR_DATA * victim_next;
  OBJ_DATA  * obj;
  char        buf[MAX_INPUT_LENGTH];

  PUSH_FUNCTION( "spec_new_doctor" );

  if ( !ch
    || !verify_char( ch )
    || !IS_AWAKE( ch )
    || is_affected( ch, SLOT_CHARM_PERSON )
    || ch->mana <= 0
    || ch->fighting
    || !ch->in_room )
    RETURN( FALSE );

  for ( victim = ch->in_room->people; victim; victim = victim_next )
  {
    victim_next = victim->next_in_room;

    if ( !verify_char( victim ) || IS_NPC( victim ) ) continue;

    if ( victim != ch
      && can_see( ch, victim )
      && number_bits( 2 ) == 0 )
      break;
  }

  if ( !victim ) RETURN( FALSE );

  switch ( number_range( 1, 9 ) )
  {
  case 1:
  default:
    act( "$n���n�u�D�R�u�@���O�@�A�C�v", ch, NULL, NULL, TO_ROOM );
    sprintf( buf, " 'armor' %s", victim->name );
    do_cast( ch, buf );
    RETURN( TRUE );

  case 2:
    act( "$n���n�u�D�R�u�@�����֧A�C�v", ch, NULL, NULL, TO_ROOM );
    sprintf( buf, " 'bless' %s", victim->name );
    do_cast( ch, buf );
    RETURN( TRUE );

  case 3:
    act( "$n���n�u�D�R�u���h���~���L�U�����^�_�a�C�v", ch, NULL, NULL, TO_ROOM );
    sprintf( buf, " 'cure blindness' %s", victim->name );
    do_cast( ch, buf );
    RETURN( TRUE );

  case 4:
    act( "$n���n�u�D�R�u�@���^�_�A���C�K���O�C�v", ch, NULL, NULL, TO_ROOM );
    sprintf( buf, " 'cure light' %s", victim->name );
    do_cast( ch, buf );
    RETURN( TRUE );

  case 5:
    act( "$n���n�u�D�R�u�c�r���A���{�j�a�C�v", ch, NULL, NULL, TO_ROOM );
    sprintf( buf, " 'cure poison' %s", victim->name );
    do_cast( ch, buf );
    RETURN( TRUE );

  case 6:
    act( "$n���n�u�D�R�u�@����_�A����O�u���C�v", ch, NULL, NULL, TO_ROOM );
    sprintf( buf, " 'refresh' %s", victim->name );
    do_cast( ch, buf );
    RETURN( TRUE );

  case 7:
    for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
      if ( obj->item_type == ITEM_FOUNTAIN ) RETURN( FALSE );

    act( "$n���n�u�D�R�u�j�a�״I���u����X�a�C�v", ch, NULL, NULL, TO_ROOM );
    do_cast( ch, " 'make spring'" );
    RETURN( TRUE );

  case 8:
    for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
      if ( obj->item_type == ITEM_FOOD ) RETURN( FALSE );

    act( "$n���n�u�D�R�u�j�aĭ�t�������X�ӧa�C�v", ch, NULL, NULL, TO_ROOM );
    do_cast( ch, " 'create food'" );
    RETURN( TRUE );

  case 9:
    act( "$n���n�u�D�R�u�r�����A���{�T�v", ch, NULL, NULL, TO_ROOM );
    sprintf( buf, " 'cure overdose' %s", victim->name );
    do_cast( ch, buf );
    RETURN( TRUE );
  }

  RETURN( FALSE );
}

bool spec_thief( CHAR_DATA * ch )
{
  CHAR_DATA * victim;
  CHAR_DATA * v_next;
  int         gold;

  PUSH_FUNCTION( "spec_thief" );

  if ( ch->position != POS_STANDING || !ch->in_room ) RETURN( FALSE );

  for ( victim = ch->in_room->people; victim; victim = v_next )
  {
    v_next = victim->next_in_room;

    if ( !verify_char( victim )
      || IS_NPC( victim )
      || victim->level >= LEVEL_IMMORTAL
      || number_bits( 2 ) != 0
      || !can_see( ch, victim ) )
        continue;

    if ( IS_AWAKE( victim )
      && over_scale( ch )
      && number_range( 0, ch->level ) == 0 )
    {
      act( "�A�o�{$n���������J�A���֥]�̡T", ch, NULL, victim, TO_VICT );
      act( "$N�o�{$n���ⰽ�����J$S���֥]�̡T", ch, NULL, victim, TO_NOTVICT );
      RETURN( TRUE );
    }
    else
    {
      gold = victim->gold * number_range( 1, 20 ) / 100;

      gold_to_char( ch, 7 * gold / 8 );
      gold_from_char( victim, gold );

      RETURN( TRUE );
    }
  }

  RETURN( FALSE );
}

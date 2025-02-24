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
    act( "$n輕聲吟道﹕「願神保護你。」", ch, NULL, NULL, TO_ROOM );
    sprintf( buf, " 'armor' %s", victim->name );
    do_cast( ch, buf );
    RETURN( TRUE );

  case 2:
    act( "$n輕聲吟道﹕「願神祝福你。」", ch, NULL, NULL, TO_ROOM );
    sprintf( buf, " 'bless' %s", victim->name );
    do_cast( ch, buf );
    RETURN( TRUE );

  case 3:
    act( "$n輕聲吟道﹕「失去光芒的無助眼神回復吧。」", ch, NULL, NULL, TO_ROOM );
    sprintf( buf, " 'cure blindness' %s", victim->name );
    do_cast( ch, buf );
    RETURN( TRUE );

  case 4:
    act( "$n輕聲吟道﹕「願神回復你的青春活力。」", ch, NULL, NULL, TO_ROOM );
    sprintf( buf, " 'cure light' %s", victim->name );
    do_cast( ch, buf );
    RETURN( TRUE );

  case 5:
    act( "$n輕聲吟道﹕「惡毒不再降臨大地。」", ch, NULL, NULL, TO_ROOM );
    sprintf( buf, " 'cure poison' %s", victim->name );
    do_cast( ch, buf );
    RETURN( TRUE );

  case 6:
    act( "$n輕聲吟道﹕「願神恢復你的體力泉源。」", ch, NULL, NULL, TO_ROOM );
    sprintf( buf, " 'refresh' %s", victim->name );
    do_cast( ch, buf );
    RETURN( TRUE );

  case 7:
    for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
      if ( obj->item_type == ITEM_FOUNTAIN ) RETURN( FALSE );

    act( "$n輕聲吟道﹕「大地豐富的泉源湧出吧。」", ch, NULL, NULL, TO_ROOM );
    do_cast( ch, " 'make spring'" );
    RETURN( TRUE );

  case 8:
    for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
      if ( obj->item_type == ITEM_FOOD ) RETURN( FALSE );

    act( "$n輕聲吟道﹕「大地蘊含的食物出來吧。」", ch, NULL, NULL, TO_ROOM );
    do_cast( ch, " 'create food'" );
    RETURN( TRUE );

  case 9:
    act( "$n輕聲吟道﹕「毒神不再降臨﹗」", ch, NULL, NULL, TO_ROOM );
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
      act( "你發現$n偷偷把手伸入你的皮包裡﹗", ch, NULL, victim, TO_VICT );
      act( "$N發現$n的手偷偷伸入$S的皮包裡﹗", ch, NULL, victim, TO_NOTVICT );
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

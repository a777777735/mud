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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "merc.h"

extern void extract_all_stone args( ( OBJ_DATA * ) );

/* �ϰ��� */
void    affect_modify_attr args( ( CHAR_DATA * , AFFECT_DATA * , bool ) );

static ARRAY_DATA ObjArmor[] =
  { { 501, "\e[1;33m�j�q�W�j\e[0m" },
    { 400, "\e[1;33m�|���W�j\e[0m" },
    { 300, "\e[1;33m�T���W�j\e[0m" },
    { 200, "\e[1;33m�G���W�j\e[0m" },
    { 101, "\e[1;33m�@���W�j\e[0m" },
    { 100, "\e[1;32m���n�L��\e[0m" },
    {  70, "\e[1;35m���׷l�a\e[0m" },
    {  40, "\e[1;34m���׷l�a\e[0m" },
    {  10, "\e[1;36m�Y���l�a\e[0m" },
    {  -1, "\e[1;31m�Ӱe�פF\e[0m" }
};

/* ���o�Y�H�������ũάO�Q�H������ */
int get_trust( CHAR_DATA * ch )
{
  PUSH_FUNCTION( "get_trust" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "get_trust: �ӷ����~" );
    RETURN( ERRORCODE );
  }

  if ( ch->trust > 0 ) RETURN( ch->trust );
  if ( IS_NPC(ch) && ch->level >= LEVEL_HERO ) RETURN(( LEVEL_HERO - 1 ) );
  RETURN( ch->level );
}

/* ���o�Y�H�����~�� */
int get_age( CHAR_DATA * ch )
{
  PUSH_FUNCTION( "get_age" );
  RETURN( BornAge
    + ( ch->played + (int) ( current_time - ch->logon ) ) / 14400 );

  /* 12240 assumes 30 second hours, 24 hours a day, 20 day */
}

/* �Ǧ^�u���ͩR�O�ƭ� */
int get_curr_hit( CHAR_DATA * ch )
{
  PUSH_FUNCTION( "get_curr_hit" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "get_curr_hit: �ǤJ���H�������T." );
    RETURN( ERRORCODE );
  }

  RETURN( ch->max_hit + ch->mod_hit );
}

/* �Ǧ^�u���k�O���ƭ� */
int get_curr_mana( CHAR_DATA * ch )
{
  PUSH_FUNCTION( "get_curr_mana" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "get_curr_mana: �ǤJ���H�������T." );
    RETURN( ERRORCODE );
  }

  RETURN( ch->max_mana + ch->mod_mana );
}

/* �Ǧ^�u����O���ƭ� */
int get_curr_move( CHAR_DATA * ch )
{
  PUSH_FUNCTION( "get_curr_move" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "get_curr_move: �ǤJ���H�������T." );
    RETURN( ERRORCODE );
  }

  RETURN( ch->max_move + ch->mod_move );
}

/* �Ǧ^�u���O�q���ƭ� */
int get_curr_str( CHAR_DATA * ch )
{
  PUSH_FUNCTION( "get_curr_str" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "get_curr_str: �ӷ������T." );
    RETURN( ERRORCODE );
  }

  if ( IS_NPC( ch ) ) RETURN( ( ch->now_str ) );
  RETURN( ( ch->now_str + ch->pcdata->mod_str ) );
}

/* �Ǧ^�ɯŤO�q���I�� */
int get_str_train( CHAR_DATA * ch )
{
  PUSH_FUNCTION( "get_str_train" );

  if ( !ch || !ch->class ) RETURN( ( ( ( unsigned ) -1 ) / 2 ) );

  RETURN ( SQUARE( ch->now_str + 1 ) * STR_UPGRADE *
    ch->class->factor[STR_ITEM] / 100 );
}

/* �Ǧ^�u�����O���ƭ� */
int get_curr_int( CHAR_DATA * ch )
{
  PUSH_FUNCTION( "get_curr_int" );

  if ( IS_NPC( ch ) ) RETURN( ch->now_int );

  RETURN ( ch->now_int + ch->pcdata->mod_int );
}

/* �Ǧ^�ɯŴ��O���I�� */
int get_int_train( CHAR_DATA * ch )
{
  PUSH_FUNCTION( "get_int_train" );

  if ( !ch || !ch->class ) RETURN( ( ( ( unsigned ) -1 ) / 2 ) );

  RETURN( SQUARE( ch->now_int + 1 ) * INT_UPGRADE *
    ch->class->factor[INT_ITEM] / 100 );
}

/* �Ǧ^�u�����Ѫ��ƭ� */
int get_curr_wis( CHAR_DATA * ch )
{
  PUSH_FUNCTION( "get_curr_wis" );

  if ( IS_NPC( ch ) ) RETURN( ch->now_wis );

  RETURN( ch->now_wis + ch->pcdata->mod_wis );
}

/* �Ǧ^�ɯž��Ѫ��I�� */
int get_wis_train( CHAR_DATA * ch )
{
  PUSH_FUNCTION( "get_wis_train" );

  if ( !ch || !ch->class ) RETURN( ( ( ( unsigned ) -1 ) / 2 ) );

  RETURN ( SQUARE( ch->now_wis + 1 ) * WIS_UPGRADE *
    ch->class->factor[WIS_ITEM] / 100 );
}

/* �Ǧ^�u���ӱ����ƭ� */
int get_curr_dex( CHAR_DATA * ch )
{
  PUSH_FUNCTION( "get_curr_dex" );

  if ( IS_NPC( ch ) ) RETURN( ch->now_dex );

  RETURN ( ch->now_dex + ch->pcdata->mod_dex );
}

/* �Ǧ^�ɯűӱ����I�� */
int get_dex_train( CHAR_DATA * ch )
{
  PUSH_FUNCTION( "get_dex_train" );

  if ( !ch || !ch->class ) RETURN( ( ( ( unsigned ) -1 ) / 2 ) );

  RETURN ( SQUARE( ch->now_dex + 1 ) * DEX_UPGRADE *
    ch->class->factor[DEX_ITEM] / 100 );
}

/* �Ǧ^�u����檺�ƭ� */
int get_curr_con( CHAR_DATA * ch )
{
  PUSH_FUNCTION( "get_curr_con" );

  if ( IS_NPC( ch ) ) RETURN ( ch->now_con );

  RETURN ( ch->now_con + ch->pcdata->mod_con );
}

/* �Ǧ^�ɯ���檺�I�� */
int get_con_train( CHAR_DATA * ch )
{
  PUSH_FUNCTION( "get_con_train" );

  if ( !ch || !ch->class ) RETURN( ( ( ( unsigned ) -1 ) / 2 ) );

  RETURN ( SQUARE( ch->now_con + 1 ) * CON_UPGRADE *
    ch->class->factor[CON_ITEM] / 100 );
}

/* �Ǧ^�Y�H���i�H��a�̦h�X�󪫫~ */
int can_carry_n( CHAR_DATA * ch )
{
  CHAR_DATA  * mount;
  SITUS_DATA * pSitus;
  int          modify = 0;
  int          total;

  PUSH_FUNCTION( "can_carry_n" );
  if ( !IS_NPC( ch ) && ch->level >= LEVEL_IMMORTAL ) RETURN( MAX_CARRY_N );
  if ( IS_NPC( ch ) && IS_SET( ch->act, ACT_PET ) )   RETURN( 0 );
  if ( IS_NPC( ch ) && get_rider( ch ) )              RETURN( 0 );

  if ( ( mount = get_mount( ch ) ) ) modify = get_curr_dex( mount );

  total = 0;
  for ( pSitus = situs_list; pSitus; pSitus = pSitus->next ) total++;

  RETURN( total + modify + get_curr_dex( ch ) );
}

/* �Ǧ^�Y�H���i�H��a���~�����q */
int can_carry_w( CHAR_DATA * ch )
{
  CHAR_DATA * mount;
  int         modify = 0;

  PUSH_FUNCTION( "can_carry_w" );
  if ( !IS_NPC(ch) && ch->level >= LEVEL_IMMORTAL ) RETURN( MAX_CARRY_W );
  if ( IS_NPC( ch ) && IS_SET(ch->act, ACT_PET ) )  RETURN( 0 );
  if ( IS_NPC( ch ) && get_rider( ch ) )            RETURN( 0 );

  if ( ( mount = get_mount( ch ) ) ) modify = get_curr_str( mount ) * 20;

  RETURN( modify + get_curr_str( ch ) * 20 );
}

int get_carry_weight( CHAR_DATA * ch )
{
  PUSH_FUNCTION( "get_carry_weight" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "get_carry_weight: �ӷ������T." );
    RETURN( 0 );
  }

  RETURN( ch->carry_weight + ch->gold / ( UMAX( GoldWeight, 1 ) ) );
}

void affect_modify( CHAR_DATA * ch, AFFECT_DATA * paf, bool fAdd )
{
  int mod;

  PUSH_FUNCTION( "affect_modify" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  /* fAdd �Y���u�h�W�[�׹���, �Ϥ��ۤ� */
  mod = fAdd ? paf->modifier : -paf->modifier;

  switch ( paf->location )
  {
  default:
    mudlog( LOG_DEBUG , "Affect_modify: �������a�� %d.", paf->location );
    RETURN_NULL();

  case APPLY_NONE:
  case APPLY_CLASS:
  case APPLY_LEVEL:
  case APPLY_AGE:
  case APPLY_HEIGHT:
  case APPLY_WEIGHT:
  case APPLY_GOLD:
  case APPLY_EXP:                                       break;
  case APPLY_STR:           ch->pcdata->mod_str += mod; break;
  case APPLY_DEX:           ch->pcdata->mod_dex += mod; break;
  case APPLY_INT:           ch->pcdata->mod_int += mod; break;
  case APPLY_WIS:           ch->pcdata->mod_wis += mod; break;
  case APPLY_CON:           ch->pcdata->mod_con += mod; break;
  case APPLY_SEX:           ch->sex             += mod; break;
  case APPLY_MANA:          ch->max_mana        += mod; break;
  case APPLY_HIT:           ch->max_hit         += mod; break;
  case APPLY_MOVE:          ch->max_move        += mod; break;
  case APPLY_AC:            ch->armor           += mod; break;
  case APPLY_HITROLL:       ch->hitroll         += mod; break;
  case APPLY_DAMROLL:       ch->damroll         += mod; break;
  case APPLY_SAVING_PARA:   ch->saving_throw    += mod; break;
  case APPLY_SAVING_ROD:    ch->saving_throw    += mod; break;
  case APPLY_SAVING_PETRI:  ch->saving_throw    += mod; break;
  case APPLY_SAVING_BREATH: ch->saving_throw    += mod; break;
  case APPLY_SAVING_SPELL:  ch->saving_throw    += mod; break;
  }

  /*
  if ( ( wield = get_eq_char( ch, WEAR_WIELD ) ) )
  {
    static int depth;

    if ( depth == 0 )
    {
      depth++;
      act( "$n�ᱼ$p�C", ch, wield, NULL, TO_ALL );
      obj_from_char( wield );
      obj_to_room( wield, ch->in_room );
      depth--;
    }
  }

  */

  RETURN_NULL();
}

/* �b�Y�H���W�W�[�@�Ӽv�T */
void affect_to_char( CHAR_DATA * ch, AFFECT_DATA * paf )
{
  AFFECT_DATA * paf_new;

  PUSH_FUNCTION( "affect_to_char" );

  if ( !ch || !paf )
  {
    mudlog( LOG_DEBUG, "affect_to_char: �ӷ������T." );
    RETURN_NULL();
  }

  /* �t�m�O���� */
  paf_new = alloc_struct( STRUCT_AFFECT_DATA );

  *paf_new      = *paf;
  paf_new->next = ch->affected;
  ch->affected  = paf_new;
  affect_modify( ch, paf_new, TRUE );
  RETURN_NULL();
}

/* �b�Y�H���W�����@�Ӽv�T */
void affect_remove( CHAR_DATA * ch, AFFECT_DATA * paf )
{
  AFFECT_DATA * prev;

  PUSH_FUNCTION( "affect_remove" );

  if ( !ch || !paf )
  {
    mudlog( LOG_DEBUG, "affect_remove: �ӷ������T." );
    RETURN_NULL();
  }

  if ( !ch->affected )
  {
    mudlog( LOG_DEBUG , "Affect_remove: �S�����󪺲���." );
    RETURN_NULL();
  }

  affect_modify( ch, paf, FALSE );

  if ( paf == ch->affected )
  {
    ch->affected = paf->next;
  }
  else
  {
    for ( prev = ch->affected; prev; prev = prev->next )
    {
      if ( prev->next == paf )
      {
        prev->next = paf->next;
        break;
      }
    }

    if ( !prev )
    {
      mudlog( LOG_DEBUG , "Affect_remove: �o�{���� paf." );
      RETURN_NULL();
    }
  }

  /* �p�G�O���e�N */
  if ( paf->type == SLOT_MASK ) ch->mask = -1;

  free_struct( paf , STRUCT_AFFECT_DATA );
  RETURN_NULL();
}

void affect_release( CHAR_DATA * ch, int type )
{
  AFFECT_DATA * pAffect;
  AFFECT_DATA * zAffect;
  SKILL_DATA  * pSkill;

  PUSH_FUNCTION( "affect_release" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "affect_release: �ʥF�ӷ�." );
    RETURN_NULL();
  }

  for ( pAffect = ch->affected; pAffect; pAffect = zAffect )
  {
    zAffect = pAffect->next;
    if ( !( pSkill = get_skill( pAffect->type ) ) ) continue;
    if ( pSkill->affect == type ) affect_remove( ch, pAffect );
  }

  RETURN_NULL();
}

bool is_affected( CHAR_DATA * ch, int sn )
{
  AFFECT_DATA * pAffect;

  PUSH_FUNCTION( "is_affected" );

  if ( sn < 0 ) RETURN( FALSE );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "is_affected: �ӷ������T." );
    RETURN( FALSE );
  }

  for ( pAffect = ch->affected; pAffect; pAffect = pAffect->next )
    if ( pAffect->type == sn ) RETURN( TRUE );

  if ( ch->pIndexData )
  {
    for ( pAffect = ch->pIndexData->affected; pAffect; pAffect = pAffect->next )
      if ( pAffect->type == sn ) RETURN( TRUE );
  }

  RETURN( FALSE );
}

void affect_join( CHAR_DATA * ch, AFFECT_DATA * paf )
{
  AFFECT_DATA * paf_old;

  PUSH_FUNCTION( "affect_join" );

  if ( !ch || !paf )
  {
    mudlog( LOG_DEBUG, "affect_join: �ӷ������T." );
    RETURN_NULL();
  }

  for ( paf_old = ch->affected; paf_old; paf_old = paf_old->next )
  {
    if ( paf_old->type == paf->type )
    {
      paf->duration += paf_old->duration;
      paf->modifier += paf_old->modifier;
      affect_remove( ch, paf_old );
      break;
    }
  }
  affect_to_char( ch, paf );
  RETURN_NULL();
}

void char_from_room( CHAR_DATA * ch )
{
  CHAR_DATA * prev;

  PUSH_FUNCTION( "char_from_room" );

  if ( !ch || !ch->in_room )
  {
    mudlog( LOG_DEBUG , "Char_from_room: �Ū��ӷ�." );
    RETURN_NULL();
  }

  /* �ץ�����w�İ� */
  if ( IS_NPC( ch ) && ch->talk_buf )
  {
    free_string( ch->talk_buf );
    ch->talk_buf  = NULL;
    ch->talk_mode = MODE_ABSENT;
  }

  if ( !IS_NPC( ch ) ) --ch->in_room->area->nplayer;
  ch->in_room->nplayer--;

  light_adjust( ch , get_eq_char( ch, WEAR_LIGHT ) , FALSE );

  if ( ch == ch->in_room->people )
  {
    ch->in_room->people = ch->next_in_room;
  }
  else
  {
    for ( prev = ch->in_room->people; prev; prev = prev->next_in_room )
    {
      if ( prev->next_in_room == ch )
      {
        prev->next_in_room = ch->next_in_room;
        break;
      }
    }

    if ( !prev ) mudlog( LOG_DEBUG , "Char_from_room: ���o�{��H��." );
  }

  ch->in_room      = NULL;
  ch->next_in_room = NULL;
  RETURN_NULL();
}

void char_to_room( CHAR_DATA * ch, ROOM_INDEX_DATA * pRoomIndex )
{
  PUSH_FUNCTION( "char_to_room" );

  if ( !ch || !pRoomIndex )
  {
    mudlog( LOG_DEBUG , "Char_to_room: �Ū��ж�." );
    RETURN_NULL();
  }

  ch->in_room        = pRoomIndex;
  ch->next_in_room   = pRoomIndex->people;
  pRoomIndex->people = ch;

  if ( !IS_NPC( ch ) ) ch->in_room->area->nplayer++;
  ch->in_room->nplayer++;
  light_adjust( ch , get_eq_char( ch , WEAR_LIGHT ), TRUE );

  RETURN_NULL();
}

void gold_to_char( CHAR_DATA * ch, int amount )
{
  int temp;
  int max;
  int canget;

  PUSH_FUNCTION( "gold_to_char" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "gold_to_char: �ӷ������T." );
    RETURN_NULL();
  }

  if ( amount < 0 )
  {
    mudlog( LOG_DEBUG, "gold_to_char: %s ��������B %d �p��s."
      , ch->name, amount );

    RETURN_NULL();
  }

  if ( ch->gold < 0 )
  {
    ch->gold = 0;
    mudlog( LOG_DEBUG, "gold_to_char: %s �����O�t���C", ch->name );
    RETURN_NULL();
  }

  if ( amount == 0 ) RETURN_NULL();

  if ( amount + ch->gold < 0 || over_scale( ch ) )
  {
    send_to_char( "�A��a�Ȥl�w�g�W�X�t�Ϊ��t���F�T\n\r", ch );

    if ( ch->in_room )
    {
      gold_to_room( ch->in_room, amount );
      act( "$n�����ʳo��h�Ȥl�M�]�Ӵ����@�a�T", ch, NULL, NULL, TO_ALL );
     }

    RETURN_NULL();
  }

  if ( IS_NPC( ch ) )
  {
    ch->gold += amount;
    RETURN_NULL();
  }

  temp = ch->gold;
  ch->gold = 0;
  max = ( can_carry_w( ch ) - get_carry_weight( ch ) + 1 ) * GoldWeight - 1;

  if ( max >= temp + amount )
  {
    ch->gold = temp + amount;
  }

  else
  {
    if ( ( canget = max - temp ) > 0 )
    {
      ch->gold = temp + canget;
      if ( ch->in_room )
      {
        gold_to_room( ch->in_room, UMAX( 1, amount - canget ) );
        act( "$n�����ʳo��h�Ȥl�M�]�Ӵ����@�a�T", ch, NULL, NULL, TO_ALL );
      }
    }

    else
    {
      ch->gold = temp;

      if ( ch->in_room )
      {
        gold_to_room( ch->in_room, amount );
        act( "$n�����ʳo��h�Ȥl�M�]�Ӵ����@�a�T", ch, NULL, NULL, TO_ALL );
      }
    }
  }

  RETURN_NULL();
}

void gold_from_char( CHAR_DATA * ch, int amount )
{
  PUSH_FUNCTION( "gold_from_char" );

  if ( !ch || amount < 0 )
  {
    mudlog( LOG_DEBUG, "gold_from_char: �ӷ������T." );
    RETURN_NULL();
  }

  if ( ch->gold < 0 )
  {
    ch->gold = 0;
    mudlog( LOG_DEBUG, "gold_from_char: %s �����O�t���C", ch->name );
    RETURN_NULL();
  }

  if ( amount == 0 ) RETURN_NULL();

  if ( ch->gold - amount < 0 )
  {
    amount = ch->gold;
    mudlog( LOG_DEBUG, "gold_from_char: %s �t�����G.", ch->name );
  }

  ch->gold -= amount;
  RETURN_NULL();
}

void gold_to_room( ROOM_INDEX_DATA * room, int amount )
{
  OBJ_DATA * obj;
  OBJ_DATA * obj_next;

  PUSH_FUNCTION( "gold_to_room" );

  if ( !room || amount < 0 )
  {
    mudlog( LOG_DEBUG, "�ӷ������T." );
    RETURN_NULL();
  }

  for ( obj = room->contents; obj; obj = obj_next )
  {
    obj_next = obj->next_content;
    if ( !obj->pIndexData ) continue;

    if ( obj->pIndexData == ObjMoney )
    {
      amount++;
      extract_obj( obj );
    }

    else if ( obj->pIndexData == ObjMoneySome )
    {
      amount += obj->value[0];
      extract_obj( obj );
    }
  }

  obj_to_room( create_money( UMAX( amount, 1 ) ), room );

  RETURN_NULL();
}

void fix_gold_weight( CHAR_DATA * ch )
{
  int temp;

  PUSH_FUNCTION( "fix_gold_weight" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "fix_gold_weight: �ӷ������T." );
    RETURN_NULL();
  }

  temp     = ch->gold;
  ch->gold = 0;

  gold_to_char( ch, temp );
  RETURN_NULL();
}

/* ��F�赹�H�� */
void obj_to_char( OBJ_DATA * obj, CHAR_DATA * ch )
{
  PUSH_FUNCTION( "obj_to_char" );

  if ( !ch || !obj )
  {
    mudlog( LOG_DEBUG, "obj_to_char: �ӷ��O�Ū�." );
    RETURN_NULL();
  }

  obj->next_content = ch->carrying;
  ch->carrying      = obj;
  obj->carried_by   = ch;
  obj->in_room      = NULL;
  obj->in_obj       = NULL;
  ch->carry_number += 1;
  ch->carry_weight += get_obj_weight( obj );
  RETURN_NULL();
}

/* ��F�貾�� */
void obj_from_char( OBJ_DATA * obj )
{
  CHAR_DATA * ch;
  OBJ_DATA  * prev;

  PUSH_FUNCTION( "obj_from_char" );

  if ( !obj || !( ch = obj->carried_by ) )
  {
    mudlog( LOG_DEBUG , "Obj_from_char: �H���O�Ū�." );
    RETURN_NULL();
  }

  if ( obj->wear_loc != WEAR_NONE ) unequip_char( ch, obj, FALSE );

  if ( ch->carrying == obj )
  {
    ch->carrying = obj->next_content;
  }
  else
  {
    for ( prev = ch->carrying; prev; prev = prev->next_content )
    {
      if ( prev->next_content == obj )
      {
        prev->next_content = obj->next_content;
        break;
      }
    }
    if ( !prev ) mudlog( LOG_DEBUG , "Obj_from_char: �����������~." );
  }

  obj->carried_by    = NULL;
  obj->next_content  = NULL;
  ch->carry_number  -= 1;
  ch->carry_weight  -= get_obj_weight( obj );
  RETURN_NULL();
}

int apply_ac( OBJ_DATA * obj, int iWear )
{
  int          val;
  SITUS_DATA * pSitus;

  PUSH_FUNCTION( "apply_ac" );

  /* �������~�� */
  if ( !obj || obj->item_type != ITEM_ARMOR ) RETURN( 0 );

  for ( pSitus = situs_list; pSitus; pSitus = pSitus->next )
  {
    if ( pSitus->location == iWear )
    {
      val = obj->value[0] * pSitus->apply_ac / 100;
      RETURN( val );
    }
  }

  RETURN( 0 );
}

OBJ_DATA * get_eq_char( CHAR_DATA * ch, int iWear )
{
  OBJ_DATA * pObj;

  PUSH_FUNCTION( "get_eq_char" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "get_eq_char: �H���O�Ū�." );
    RETURN( NULL );
  }

  for ( pObj = ch->carrying; pObj; pObj = pObj->next_content )
    if ( pObj->wear_loc == iWear ) RETURN( pObj );

  RETURN( NULL );
}

void equip_char( CHAR_DATA * ch, OBJ_DATA * obj, int iWear )
{
  AFFECT_DATA    * paf;
  CACHET_DATA    * pCachet;
  OBJ_INDEX_DATA * pIndex;
  char           * pString = NULL;
  int              loop;

  PUSH_FUNCTION( "equip_char" );

  if ( !ch || !obj )
  {
    mudlog( LOG_DEBUG, "equip_char: �ӷ��O�Ū�" );
    RETURN_NULL();
  }

  if ( get_eq_char( ch, iWear ) )
  {
    mudlog( LOG_DEBUG , "Equip_char: %s(%d) �w�g�˳ƤF %d."
      , ch->name , ch->pIndexData ? ch->pIndexData->vnum : -1, iWear );
    RETURN_NULL();
  }

  if ( ( obj->AntiEvil    && IS_EVIL( ch ) )
    || ( obj->AntiGood    && IS_GOOD( ch ) )
    || ( obj->AntiNeutral && IS_NEUTRAL( ch ) ) )
  {
    act( "�A�Q$p���M�ҥH�������ӱ���a�W�C", ch, obj, NULL, TO_CHAR );
    act( "$n���F$p�ӱN������a�W�C"    , ch, obj, NULL, TO_ROOM );
    obj_from_char( obj );
    obj_to_room( obj, ch->in_room );
    RETURN_NULL();
  }

  message_driver( ch, obj, ACT_WHEN_WEAR );

  if ( obj->pIndexData
    && obj->pIndexData->wearmsg
    && *obj->pIndexData->wearmsg )
  {
    pString = obj->pIndexData->wearmsg;
    if ( pString && *pString == '.' ) pString++;
  }

  if ( pString && !IS_NPC( ch ) ) sendmsg_to_room( pString, ch );

  ch->armor     -= apply_ac( obj, iWear );
  obj->wear_loc  = iWear;

  for ( paf = obj->pIndexData->affected; paf; paf = paf->next )
    affect_modify( ch, paf, TRUE );

  for ( paf = obj->affected; paf; paf = paf->next )
    affect_modify( ch, paf, TRUE );

  for ( pCachet = obj->cachet; pCachet; pCachet = pCachet->next )
  {
    if ( !( pIndex = get_obj_index( pCachet->vnum ) ) ) continue;

    for ( paf = pIndex->impact; paf; paf = paf->next )
      affect_modify( ch, paf, TRUE );
  }

  light_adjust( ch, obj, TRUE );

  /* �]�w�˳ƾ֦��� */
  if ( !IS_NPC( ch ) && obj->owner.high == 0 && obj->owner.low == 0 )
  {
    obj->owner.high = ch->serial.high;
    obj->owner.low  = ch->serial.low;

    act( "�q�{�b�}�l�M$p�}�����H$n�T", ch, obj, NULL, TO_ALL );
  }

  else if ( obj->owner.high != ch->serial.high
    || obj->owner.low != ch->serial.low )
  {
    obj->owner.high = ch->serial.high;
    obj->owner.low  = ch->serial.low;

    if ( ( pIndex = obj->pIndexData ) )
    {
      obj->level = pIndex->level;

      switch( obj->item_type )
      {
      default:
      case ITEM_LIGHT:
      case ITEM_MYSTERY:
      case ITEM_SCROLL:
      case ITEM_POTION:
      case ITEM_PILL:
      case ITEM_FURNITURE:
      case ITEM_TRASH:
      case ITEM_KEY:
      case ITEM_BOAT:
      case ITEM_CORPSE_PC:
      case ITEM_CORPSE_NPC:
      case ITEM_FOUNTAIN:
      case ITEM_TREASURE:
      case ITEM_LETTER:
      case ITEM_ORE:
      case ITEM_GOLDMINE:
      case ITEM_CONTAINER:
      case ITEM_DRINK_CON:
      case ITEM_FOOD:
      case ITEM_MONEY:
      case ITEM_SPIRITJADE:
      case ITEM_SIGIL:
      case ITEM_VELLUM:
      case ITEM_FIREWORK:
      case ITEM_STAFF:
      case ITEM_WAND:
      case ITEM_MAGICSTONE:
      case ITEM_URN:
      case ITEM_BACKBAG:
      case ITEM_ARROW:
        break;

      case ITEM_WEAPON:
      case ITEM_ARMOR:

        for ( loop = 0; loop < MAX_OBJECT_VALUE; loop++ )
          obj->value[loop] = pIndex->value[loop];

        obj->max_armor = UMIN( obj->max_armor, pIndex->armor );
        obj->armor     = UMIN( obj->armor    , pIndex->armor );

        act( "$n���W��$p�S�^�_���Ӫ����A�M���A�{ģ�ʤH�T"
          , ch, obj, NULL, TO_ALL );

        break;
      }
    }
  }

  RETURN_NULL();
}

void unequip_char( CHAR_DATA * ch, OBJ_DATA * obj, bool fPrint )
{
  OBJ_INDEX_DATA * pIndex;
  CACHET_DATA    * pCachet;
  AFFECT_DATA    * paf;
  char           * pString = NULL;

  PUSH_FUNCTION( "unequip_char" );

  if ( obj->wear_loc == WEAR_NONE )
  {
    mudlog( LOG_DEBUG , "Unequip_char: �w�g�Ѱ��˳�." );
    RETURN_NULL();
  }

  message_driver( ch, obj, ACT_WHEN_REMOVE );

  if ( obj->pIndexData
    && obj->pIndexData->remmsg
    && *obj->pIndexData->remmsg )
  {
    pString = obj->pIndexData->remmsg;
    if ( *pString == '.' ) pString++;
  }

  if ( fPrint && pString && !IS_NPC( ch ) ) sendmsg_to_room( pString, ch );

  ch->armor     += apply_ac( obj, obj->wear_loc );
  obj->wear_loc  = -1;

  for ( paf = obj->pIndexData->affected; paf; paf = paf->next )
    affect_modify( ch, paf, FALSE );

  for ( paf = obj->affected; paf; paf = paf->next )
    affect_modify( ch, paf, FALSE );

  for ( pCachet = obj->cachet; pCachet; pCachet = pCachet->next )
  {
    if ( !( pIndex = get_obj_index( pCachet->vnum ) ) ) continue;

    for ( paf = pIndex->impact; paf; paf = paf->next )
      affect_modify( ch, paf, FALSE );
  }

  light_adjust( ch , obj , FALSE );
  RETURN_NULL();
}

int count_obj_list( OBJ_INDEX_DATA * pObjIndex, OBJ_DATA * list )
{
  OBJ_DATA * obj;
  int        nMatch;

  PUSH_FUNCTION( "count_obj_list" );

  for ( obj = list, nMatch = 0; obj; obj = obj->next_content )
    if ( obj->pIndexData == pObjIndex ) nMatch++;

  RETURN( nMatch );
}

void obj_from_room( OBJ_DATA * obj )
{
  ROOM_INDEX_DATA * in_room;
  OBJ_DATA        * prev;

  PUSH_FUNCTION( "obj_from_room" );

  if ( !( in_room = obj->in_room ) )
  {
    mudlog( LOG_DEBUG , "obj_from_room: �Ū��ж�." );
    RETURN_NULL();
  }

  in_room->nobject--;

  if ( obj == in_room->contents )
  {
    in_room->contents = obj->next_content;
  }
  else
  {
    for ( prev = in_room->contents; prev; prev = prev->next_content )
    {
      if ( prev->next_content == obj )
      {
        prev->next_content = obj->next_content;
        break;
      }
    }

    if ( !prev )
    {
      mudlog( LOG_DEBUG , "Obj_from_room: �S���o�{���~." );
      RETURN_NULL();
    }
  }

  obj->in_room      = NULL;
  obj->next_content = NULL;
  RETURN_NULL();
}

void obj_to_room( OBJ_DATA * obj, ROOM_INDEX_DATA * pRoomIndex )
{
  PUSH_FUNCTION( "obj_to_room" );

  obj->next_content    = pRoomIndex->contents;
  pRoomIndex->contents = obj;
  obj->in_room         = pRoomIndex;
  obj->carried_by      = NULL;
  obj->in_obj          = NULL;
  pRoomIndex->nobject++;
  RETURN_NULL();
}

void obj_to_obj( OBJ_DATA * obj, OBJ_DATA * obj_to )
{
  PUSH_FUNCTION( "obj_to_obj" );

  obj->next_content = obj_to->contains;
  obj_to->contains  = obj;
  obj->in_obj       = obj_to;
  obj->in_room      = NULL;
  obj->carried_by   = NULL;

  for ( ; obj_to; obj_to = obj_to->in_obj )
  {
    if ( obj_to->carried_by )
      obj_to->carried_by->carry_weight += get_obj_weight( obj );
  }
  RETURN_NULL();
}

void obj_from_obj( OBJ_DATA * obj )
{
  OBJ_DATA * obj_from;
  OBJ_DATA * prev;

  PUSH_FUNCTION( "obj_from_obj" );

  if ( !( obj_from = obj->in_obj ) )
  {
    mudlog( LOG_DEBUG , "Obj_from_obj: obj_from �O�Ū�." );
    RETURN_NULL();
  }

  if ( obj == obj_from->contains )
  {
    obj_from->contains = obj->next_content;
  }
  else
  {
    for ( prev = obj_from->contains; prev; prev = prev->next_content )
    {
      if ( prev->next_content == obj )
      {
        prev->next_content = obj->next_content;
        break;
      }
    }

    if ( !prev )
    {
      mudlog( LOG_DEBUG , "Obj_from_obj: ���o�{�쪫�~." );
      RETURN_NULL();
    }
  }

  obj->next_content = NULL;
  obj->in_obj       = NULL;

  for ( ; obj_from; obj_from = obj_from->in_obj )
  {
    if ( obj_from->carried_by )
      obj_from->carried_by->carry_weight -= get_obj_weight( obj );
  }

  RETURN_NULL();
}

void extract_obj( OBJ_DATA * obj )
{
  OBJ_DATA         * obj_content;
  OBJ_DATA         * obj_next;
  CHAR_DATA        * ch;
  AFFECT_DATA      * paf;
  AFFECT_DATA      * paf_next;
  CACHET_DATA      * pCachet;
  CACHET_DATA      * pCachet_next;
  EXTRA_DESCR_DATA * ed;
  EXTRA_DESCR_DATA * ed_next;

  PUSH_FUNCTION( "extract_obj" );

       if ( obj->in_room    ) obj_from_room( obj );
  else if ( obj->carried_by ) obj_from_char( obj );
  else if ( obj->in_obj     ) obj_from_obj(  obj );

  for ( obj_content = obj->contains; obj_content; obj_content = obj_next )
  {
    obj_next = obj_content->next_content;
    extract_obj( obj->contains );
  }

  for ( paf = obj->affected; paf; paf = paf_next )
  {
    paf_next = paf->next;
    free_struct( paf , STRUCT_AFFECT_DATA );
  }

  obj->affected = NULL;

  for ( pCachet = obj->cachet; pCachet; pCachet = pCachet_next )
  {
    pCachet_next = pCachet->next;
    free_struct( pCachet, STRUCT_CACHET_DATA );
  }

  obj->cachet = NULL;

  for ( ed = obj->extra_descr; ed; ed = ed_next )
  {
    ed_next = ed->next;
    free_string( ed->description );
    free_string( ed->keyword     );
    free_struct( ed , STRUCT_EXTRA_DESCR_DATA );
  }

  /* �ץ����� */
  if ( obj->corpse_owner )
  {
    for ( ch = char_list; ch; ch = ch->next )
    {
      if ( verify_char( ch ) && obj->corpse_owner == ch && ch->pcdata )
        ch->pcdata->corpse = NULL;
    }
  }

  /* �״_�e�H�� */
  obj->address = NULL;

  obj->pIndexData->count--;
  free_struct( obj , STRUCT_OBJ_DATA );
  RETURN_NULL();
}

/* �P���H�� */
void extract_char( CHAR_DATA * ch, bool fPull )
{
  CHAR_DATA  * wch;
  OBJ_DATA   * obj;
  OBJ_DATA   * obj_next;
  ENEMY_DATA * pEnemy;
  ENEMY_DATA * zEnemy;
  STAMP_DATA * pStamp;
  STAMP_DATA * zStamp;

  PUSH_FUNCTION( "extract_char" );

  if ( !ch->in_room )
  {
    mudlog( LOG_DEBUG , "Extract_char: �Ҧb�a�O�Ū�." );
    RETURN_NULL();
  }

  if ( fPull ) die_follower( ch );
  stop_fighting( ch, TRUE );

  /* �ץ��y�M */
  if ( ch->mount    ) unmount_char( ch, ch->mount    );
  if ( ch->mount_by ) unmount_char( ch->mount_by, ch );

  /* �ץ����F */
  if ( ch->boss   ) spirit_from_char( ch         );
  if ( ch->spirit ) spirit_from_char( ch->spirit );

  /* �ץ��ħL */
  if ( ch->hirer ) mercenary_from_char( ch, ch->hirer );
  if ( ch->mercenary && !ch->hirer ) all_mercenary_from_char( ch );

  if ( ch->bounty )
  {
    ch->bounty->count--;
    ch->bounty->killed++;
    NowBounty--;
    ch->bounty = NULL;
  }

  /* �ץ���� */
  if ( ch == auction_info->seller )
  {
    /* ���h���R���H */
    if ( auction_info->buyer )
    {
      send_to_char( "�藍�_�M�檺�H���F�M���ٵ��A�a�T\n\r"
        , auction_info->buyer );
      gold_to_char( auction_info->buyer, auction_info->price );
    }

    if ( auction_info->obj ) extract_obj( auction_info->obj );

    talk_channel( auction_info->seller
      , "���M�s��ӪF��M���|�Q���a�l���M����F�T"
      , CHANNEL_AUCTION
      , "\e[1;33m�t�۲\\���D" );

    init_auction();
  }

  /* �ץ��e�H�� */
  for ( obj = object_list; obj; obj = obj->next )
  {
    if ( verify_obj( obj ) && obj->address == ch )
    {
      if ( obj->carried_by )
        send_to_char( "�ܥi���M�A�����H�H�w�g�b�o�ӥ@�ɮ����F�T\n\r"
        , obj->carried_by );

      free_string( obj->description );
      obj->description = str_dup( "�@�ʤw�g�S���Ϊ��H��(letter)�C" );
      obj->address = NULL;
      obj->cost    = 1;
    }
  }

  for ( obj = ch->carrying; obj; obj = obj_next )
  {
    obj_next = obj->next_content;
    extract_obj( obj );
  }

  for ( pEnemy = ch->enemy; pEnemy; pEnemy = zEnemy )
  {
    zEnemy = pEnemy->next;

    free_string( pEnemy->city );
    free_struct( pEnemy, STRUCT_ENEMY_DATA );
  }

  ch->enemy = NULL;

  /* ����§�����аO */
  for ( pStamp = ch->stamp; pStamp; pStamp = zStamp )
  {
    zStamp = pStamp->next;
    free_struct( pStamp, STRUCT_STAMP_DATA );
  }

  ch->stamp = NULL;

  char_from_room( ch );

  if ( !fPull )
  {
    char_to_room( ch, RoomDead );

    /* �M���l�ܬ����I */
    clear_trace( ch, FALSE );

    RETURN_NULL();
  }

  if ( IS_NPC( ch ) )  ch->pIndexData->count--;

  for ( wch = char_list; wch; wch = wch->next )
  {
    if ( !verify_char( wch ) ) continue;
    if ( wch->reply == ch ) wch->reply = NULL;

    if ( wch->pcdata
      && wch->pcdata->wooer
      && !str_cmp( wch->pcdata->wooer->name, ch->name ) )
    {
      act( "�A���l�D��$N���]�F�T", wch, NULL, ch, TO_CHAR );
      wch->pcdata->wooer = NULL;
    }

    if ( wch->pcdata && wch->pcdata->mate == ch )
    {
      act( "�A��$t$N���u�F�T", wch, mate_name( wch ), ch, TO_CHAR );
      wch->pcdata->mate = NULL;
    }
  }

  if ( ch->desc ) ch->desc->character = NULL;
  free_char( ch );
  RETURN_NULL();
}

CHAR_DATA * get_pc_room( CHAR_DATA * ch, const char * argument )
{
  CHAR_DATA * victim;

  PUSH_FUNCTION( "get_pc_room" );

  if ( !ch || !ch->in_room || !argument || !*argument )
  {
    mudlog( LOG_DEBUG, "get_pc_room: �ӷ������T." );
    RETURN( NULL );
  }

  for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
  {
    if ( verify_char( victim )
      && can_see( ch, victim )
      && !IS_NPC( victim )
      && !str_cmp( argument, victim->name ) ) RETURN( victim );
  }

  RETURN( NULL );
}

CHAR_DATA * get_pc_world( CHAR_DATA * ch, const char * argument )
{
  CHAR_DATA * victim;

  PUSH_FUNCTION( "get_pc_world" );

  if ( !ch || !ch->in_room || !argument || !*argument )
  {
    mudlog( LOG_DEBUG, "get_pc_room: �ӷ������T." );
    RETURN( NULL );
  }

  for ( victim = char_list; victim; victim = victim->next )
  {
    if ( verify_char( victim )
      && can_see( ch, victim )
      && !IS_NPC( victim )
      && !str_cmp( argument, victim->name ) ) RETURN( victim );
  }

  RETURN( NULL );
}

CHAR_DATA * get_char_room( CHAR_DATA * ch, char * argument )
{
  char        arg[MAX_INPUT_LENGTH];
  CHAR_DATA * rch;
  int         number;
  int         count;

  PUSH_FUNCTION( "get_char_room" );

  if ( !ch || !ch->in_room || !argument || !*argument )
  {
    mudlog( LOG_DEBUG, "get_char_room: �ӷ������T." );
    RETURN( NULL );
  }

  number = number_argument( argument, arg );
  for ( count = 0, rch = ch->in_room->people; rch; rch = rch->next_in_room )
  {
    if ( !can_see( ch, rch ) || !is_name( arg, rch->name ) ) continue;
    if ( ++count == number ) RETURN( rch );
  }

  RETURN( NULL );
}

MOB_INDEX_DATA * get_char_protype( char * argument )
{
  extern int       top_mob_index;
  char             arg[MAX_INPUT_LENGTH];
  MOB_INDEX_DATA * pMobIndex;
  int              number;
  int              count;
  int              nMatch;
  int              vnum;
  int              serial;

  PUSH_FUNCTION( "get_char_protype" );

  is_number( argument ) ? ( serial = atoi( argument ) ) : ( serial = -1 );
  number = number_argument( argument, arg );

  for ( count = nMatch = vnum = 0; nMatch < top_mob_index; vnum++ )
  {
    if ( ( pMobIndex = get_mob_index( vnum ) ) )
    {
      nMatch++;
      if ( is_name( arg, pMobIndex->name )
        || pMobIndex->vnum == serial )
      {
        if ( ++count == number ) RETURN( pMobIndex );
      }
    }
  }
  RETURN( NULL );
}

int get_char_order( CHAR_DATA * ch, CHAR_DATA * victim )
{
  CHAR_DATA * rch;
  char        name[MAX_INPUT_LENGTH];
  int         count;

  PUSH_FUNCTION( "get_char_order" );

  if ( !ch || !victim )
  {
    mudlog( LOG_DEBUG, "get_char_order: �ʥF�ӷ�." );
    RETURN( -1 );
  }

  if ( !ch->in_room )
  {
    mudlog( LOG_DEBUG, "get_char_order: �H���ж����s�b." );
    RETURN( -1 );
  }

  one_argument( victim->name, name );

  /* ����ж��̪� */
  for ( count = 0, rch = ch->in_room->people; rch; rch = rch->next_in_room )
  {
    if ( !can_see( ch, rch ) || !is_name( name, rch->name ) ) continue;
    count++;

    if ( rch == victim ) RETURN( count );
  }

  /* ���o����H�� */
  for ( count = 0, rch = char_list; rch; rch = rch->next )
  {
    if ( !verify_char( rch )
      || !can_see( ch, rch )
      || !is_name( name, rch->name ) ) continue;

    count++;
    if ( rch == victim ) RETURN( count );
  }

  RETURN( -1 );
}

CHAR_DATA * get_char_world( CHAR_DATA * ch, char * argument )
{
  char        arg[MAX_INPUT_LENGTH];
  CHAR_DATA * wch;
  int         number;
  int         count = 0;

  PUSH_FUNCTION( "get_char_world" );

  if ( !ch || !ch->in_room || !argument || !*argument )
  {
    mudlog( LOG_DEBUG, "get_char_world: �ӷ������T." );
    RETURN( NULL );
  }

  if ( ( wch = get_char_room( ch, argument ) ) ) RETURN( wch );
  number = number_argument( argument, arg );

  /* ���o���ĤH�� */
  for ( wch = char_list; wch; wch = wch->next )
  {
    if ( !verify_char( wch ) ) continue;
    if ( !can_see( ch, wch ) || !is_name( arg, wch->name ) ) continue;
    if ( ++count == number ) RETURN( wch );
  }

  RETURN( NULL );
}

CHAR_DATA * get_char_area( CHAR_DATA * ch, char * argument )
{
  char        arg[MAX_INPUT_LENGTH];
  CHAR_DATA * wch;
  int         number;
  int         count = 0;

  PUSH_FUNCTION( "get_char_area" );

  if ( !ch || !ch->in_room || !argument || !*argument )
  {
    mudlog( LOG_DEBUG, "get_char_world: �ӷ������T." );
    RETURN( NULL );
  }

  if ( ( wch = get_char_room( ch, argument ) ) ) RETURN( wch );
  number = number_argument( argument, arg );

  /* ���o���ĤH�� */
  for ( wch = char_list; wch; wch = wch->next )
  {
    if ( !verify_char( wch ) ) continue;
    if ( !wch->in_room || wch->in_room->area != ch->in_room->area ) continue;
    if ( !can_see( ch, wch ) || !is_name( arg, wch->name ) ) continue;
    if ( ++count == number ) RETURN( wch );
  }

  RETURN( NULL );
}

OBJ_DATA * get_obj_type( OBJ_INDEX_DATA * pObjIndex )
{
  OBJ_DATA * obj;

  PUSH_FUNCTION( "get_obj_type" );

  for ( obj = object_list; obj; obj = obj->next )
  {
    if ( !verify_obj( obj ) ) continue;
    if ( obj->pIndexData == pObjIndex ) RETURN( obj );
  }

  RETURN( NULL );
}

OBJ_DATA * get_obj_list( CHAR_DATA * ch, char * argument, OBJ_DATA * list )
{
  char       arg[MAX_INPUT_LENGTH];
  OBJ_DATA * obj;
  int        num;
  int        count;

  PUSH_FUNCTION( "get_obj_list" );

  num = number_argument( argument, arg );
  for ( count = 0, obj = list; obj; obj = obj->next_content )
  {
    if ( can_see_obj( ch, obj )
      && is_name( arg, obj->name )
      && ++count == num )
       RETURN( obj );
  }

  RETURN( NULL );
}

/* �����Y�����������~���ƶq */
int obj_type_room( CHAR_DATA * ch, int type )
{
  int        count = 0;
  OBJ_DATA * obj;

  PUSH_FUNCTION( "obj_type_room" );

  if ( !ch || !ch->in_room )
  {
    mudlog( LOG_DEBUG, "obj_type_room: �ӷ��O�Ū�." );
    RETURN( -1 );
  }

  for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
    if ( can_see_obj( ch, obj ) && obj->item_type == type ) count++;

  RETURN( count );
}

/* ���o���W�Y���������~���ƶq */
int obj_type_char( CHAR_DATA * ch, int type )
{
  int        count = 0;
  OBJ_DATA * obj;

  PUSH_FUNCTION( "obj_type_char" );

  if ( !ch || !ch->in_room )
  {
    mudlog( LOG_DEBUG, "obj_type_char: �ӷ��O�Ū�." );
    RETURN( -1 );
  }

  for ( obj = ch->carrying; obj; obj = obj->next_content )
    if ( can_see_obj( ch, obj ) && obj->item_type == type ) count++;

  RETURN( count );
}

OBJ_DATA * get_obj_carry( CHAR_DATA * ch, char * argument )
{
  char       arg[MAX_INPUT_LENGTH];
  OBJ_DATA * obj;
  int        number;
  int        count;

  PUSH_FUNCTION( "get_obj_carry" );

  number = number_argument( argument, arg );
  for ( count = 0, obj = ch->carrying; obj; obj = obj->next_content )
  {
    if ( obj->wear_loc == WEAR_NONE
      && can_see_obj( ch, obj )
      && is_name( arg, obj->name )
      && ++count == number )
        RETURN( obj );
  }
  RETURN( NULL );
}

OBJ_DATA * get_obj_wear( CHAR_DATA * ch, char * argument )
{
  char       arg[MAX_INPUT_LENGTH];
  OBJ_DATA * obj;
  int        number;
  int        count;

  PUSH_FUNCTION( "get_obj_wear" );

  number = number_argument( argument, arg );
  for ( count = 0, obj = ch->carrying; obj; obj = obj->next_content )
  {
    if ( obj->wear_loc != WEAR_NONE
      && can_see_obj( ch, obj )
      && is_name( arg, obj->name )
      && ++count == number )
        RETURN( obj );
  }
  RETURN( NULL );
}

OBJ_DATA * get_obj_here( CHAR_DATA * ch, char * argument )
{
  OBJ_DATA * obj;

  PUSH_FUNCTION( "get_obj_here" );

  if ( ( obj = get_obj_list( ch, argument, ch->in_room->contents ) ) )
    RETURN( obj );

  if ( ( obj = get_obj_carry( ch, argument ) ) ) RETURN( obj );
  if ( ( obj = get_obj_wear( ch, argument ) )  ) RETURN( obj );

  RETURN( NULL );
}

ROOM_INDEX_DATA * get_obj_room( OBJ_DATA * obj )
{
  PUSH_FUNCTION( "get_obj_room" );

  if ( obj->in_room ) RETURN( ( ROOM_INDEX_DATA * ) obj->in_room );

  if ( obj->carried_by && obj->carried_by->in_room )
    RETURN( ( ROOM_INDEX_DATA * ) obj->carried_by->in_room );

  while ( ( obj = obj->in_obj ) )
    if ( obj->in_room ) RETURN( ( ROOM_INDEX_DATA * ) obj->in_room );

  RETURN( NULL );
}

OBJ_INDEX_DATA * get_obj_protype( char * argument )
{
  extern int       top_obj_index;
  char             arg[MAX_INPUT_LENGTH];
  OBJ_INDEX_DATA * obj;
  int              number;
  int              count;
  int              vnum;
  int              nMatch;
  int              serial;

  PUSH_FUNCTION( "get_obj_protype" );

  is_number( argument ) ? ( serial = atoi( argument ) ) : ( serial = -1 );
  number = number_argument( argument, arg );

  for ( vnum = count = nMatch = 0; nMatch < top_obj_index; vnum ++ )
  {
    if ( ( obj = get_obj_index( vnum ) ) )
    {
      nMatch++;
      if ( is_name( arg , obj->name ) || obj->vnum == serial )
      {
        if ( ++count == number ) RETURN( obj );
      }
    }
  }

  RETURN( NULL );
}

OBJ_DATA * get_obj_world( CHAR_DATA * ch, char * argument )
{
  char       arg[MAX_INPUT_LENGTH];
  OBJ_DATA * obj;
  int        number;
  int        count;

  PUSH_FUNCTION( "get_obj_world" );

  if ( ( obj = get_obj_here( ch, argument ) ) ) RETURN( obj );

  number = number_argument( argument, arg );
  for ( count = 0, obj = object_list; obj; obj = obj->next )
  {
    if ( !verify_obj( obj ) ) continue;

    if ( can_see_obj( ch, obj )
      && is_name( arg, obj->name )
      && ++count == number )
        RETURN( obj );
  }

  RETURN( NULL );
}

OBJ_DATA * create_money( int amount )
{
  char       buf[MAX_STRING_LENGTH];
  OBJ_DATA * obj;

  PUSH_FUNCTION( "create_money" );

  if ( amount <= 0 )
  {
    mudlog( LOG_DEBUG, "Create_money: �����ƥ� %d �p��ε���0.", amount );
    amount = 1;
  }

  if ( amount == 1 )
  {
    obj = create_object( ObjMoney, 1 );
    obj->value[0] = 1;
  }

  else
  {
    obj = create_object( ObjMoneySome, 1 );
    sprintf( buf, obj->cname, amount );
    free_string( obj->cname );
    obj->cname   = str_dup( buf );
    obj->value[0] = amount;
  }

  RETURN( obj );
}

int get_obj_number( OBJ_DATA * obj )
{
  int number = 1;

  PUSH_FUNCTION( "get_obj_number" );

  if ( obj->item_type == ITEM_CONTAINER )
  {
    for ( obj = obj->contains; obj; obj = obj->next_content )
      number += get_obj_number( obj );
  }

  RETURN( number );
}

int get_obj_weight( OBJ_DATA * obj )
{
  int wei;

  PUSH_FUNCTION( "get_obj_weight" );

  for ( wei = obj->weight, obj = obj->contains; obj; obj = obj->next_content )
    wei += get_obj_weight( obj );

  RETURN( wei );
}

bool room_is_dark( ROOM_INDEX_DATA * pRoomIndex )
{
  PUSH_FUNCTION( "room_is_dark" );

  if ( pRoomIndex->light > 0 )             RETURN( FALSE );
  if ( pRoomIndex->Dark      )             RETURN( TRUE  );
  if ( pRoomIndex->sector->dark == FALSE ) RETURN( FALSE );

  if ( weather_info.sunlight == SUN_SET || weather_info.sunlight == SUN_DARK )
    RETURN( TRUE );

  RETURN( TRUE );
}

bool room_is_private( ROOM_INDEX_DATA * pRoomIndex )
{
  CHAR_DATA * rch;
  int         count;

  PUSH_FUNCTION( "room_is_private" );

  for ( count = 0, rch = pRoomIndex->people; rch; rch = rch->next_in_room )
    count++;

  if ( pRoomIndex->Private && count >= 2 ) RETURN( TRUE );
  RETURN( FALSE );
}

bool can_see_mask( CHAR_DATA * ch, CHAR_DATA * victim )
{
  PUSH_FUNCTION( "can_see_mask" );

  if ( ch == victim
    || IS_NPC( ch )
    || !is_affected( victim, SLOT_MASK )
    || is_affected( ch, SLOT_DETECT_MASK )
    || ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_HOLYLIGHT ) ) )
    RETURN( TRUE );

  RETURN( FALSE );
}

bool can_see( CHAR_DATA * ch, CHAR_DATA * victim )
{
  PUSH_FUNCTION( "can_see" );

  if ( ch == victim ) RETURN( TRUE );
  if ( IS_NPC( ch ) && ch->inprocess ) RETURN( TRUE );

  if ( !IS_NPC( victim )
    && IS_SET( victim->act, PLR_WIZINVIS )
    && get_trust( ch ) < get_trust( victim ) )
      RETURN( FALSE );

  if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_HOLYLIGHT ) ) RETURN( TRUE );

  if ( !IS_NPC( ch ) && is_affected( ch, SLOT_BLINDNESS ) )
    RETURN( FALSE );

  if ( !IS_NPC( ch )
    && room_is_dark( ch->in_room )
    && !is_affected( ch, SLOT_INFRAVISION ) )
    RETURN( FALSE );

  if ( !IS_NPC( ch )
    && ( is_affected( victim, SLOT_INVIS ) || is_affected( victim, SLOT_MASS_INVIS ) )
    && !is_affected( ch, SLOT_DETECT_INVIS ) )
    RETURN( FALSE );

  if ( is_affected( victim, SLOT_HIDE )
    && !is_affected( ch, SLOT_DETECT_HIDDEN )
    && !victim->fighting
    && !IS_NPC( ch ) )
    RETURN( FALSE );
  RETURN( TRUE );
}

bool can_see_obj( CHAR_DATA * ch, OBJ_DATA * obj )
{
  PUSH_FUNCTION( "can_see_obj" );

  if ( IS_NPC( ch ) && ch->inprocess                     )  RETURN( TRUE );
  if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_HOLYLIGHT ) )  RETURN( TRUE );
  if ( obj->item_type == ITEM_POTION )                      RETURN( TRUE );
  if ( !IS_NPC( ch ) && is_affected( ch, SLOT_BLINDNESS ) ) RETURN( FALSE );
  if ( obj->item_type == ITEM_LIGHT && obj->value[2] != 0 ) RETURN( TRUE );

  if ( !IS_NPC( ch )
    && room_is_dark( ch->in_room )
    && !is_affected( ch, SLOT_INFRAVISION ) )
    RETURN( FALSE );

  if ( obj->Invis && !is_affected( ch, SLOT_DETECT_INVIS ) )
    RETURN( FALSE );

  RETURN( TRUE );
}

bool can_see_room( CHAR_DATA * ch )
{
  PUSH_FUNCTION( "can_see_room" );

  if ( !ch || !ch->in_room )
  {
    mudlog( LOG_DEBUG,  "can_see_room: �ӷ������D." );
    RETURN( FALSE );
  }

  if ( IS_NPC( ch ) && ch->inprocess ) RETURN( TRUE );

  if ( !IS_NPC( ch )
    && !IS_SET( ch->act, PLR_HOLYLIGHT )
    && room_is_dark( ch->in_room ) )
    RETURN( FALSE );

  RETURN( TRUE );
}

bool can_drop_obj( CHAR_DATA * ch, OBJ_DATA * obj )
{
  PUSH_FUNCTION( "can_drop_obj" );

  if ( !obj->NoDrop                                 ) RETURN( TRUE );
  if ( !IS_NPC( ch ) && ch->level >= LEVEL_IMMORTAL ) RETURN( TRUE );
  RETURN( FALSE );
}

CHAR_DATA * object_owner( OBJ_DATA * obj )
{
  OBJ_DATA * in_obj;

  PUSH_FUNCTION( "object_owner" );

  if ( !obj )
  {
    mudlog( LOG_DEBUG, "object_owner: �ʥF�ӷ�." );
    RETURN( NULL );
  }

  /* �H��b�� */
  if ( obj->deposit_by ) RETURN( obj->deposit_by );

  /* �Y�O��b�e����, ��X�̥~�h���e�� */
  for ( in_obj = obj; in_obj->in_obj; in_obj = in_obj->in_obj );

  if ( !in_obj || !verify_obj( in_obj ) )
  {
    mudlog( LOG_DEBUG, "object_owner: ���~������." );
    RETURN( NULL );
  }

  if ( in_obj->carried_by ) RETURN( in_obj->carried_by );

  RETURN( NULL );
}

char * object_locate( OBJ_DATA * obj , CHAR_DATA * ch )
{
  OBJ_DATA    * in_obj;
  CLUB_DATA   * club;
  static char   locate_name[ MAX_STRING_LENGTH ];

  PUSH_FUNCTION( "object_locate" );

  locate_name[0] = '\x0';

  /* �H��b�� */
  if ( obj->deposit_by )
  {
    sprintf( locate_name , "��%s�Ҧs��b��"
      , mob_name( ch, obj->deposit_by ) );

    RETURN( locate_name );
  }

  /* �H��b���w */
  if ( ( club = obj->club ) )
  {
    sprintf( locate_name, "�H��b%s(%s)���w(�Ҧb�a: %d)��"
      , club->cname
      , club->name
      , club->location ? club->location->vnum : -1 );

    RETURN( locate_name );
  }

  /* ���p�O�b��� */
  if ( auction_info->obj == obj )
  {
    if ( !auction_info->seller )
    {
      str_cpy( locate_name, "���b��椤(��D����)." );
      RETURN( locate_name );
    }

    sprintf( locate_name, "����%s��椤"
      , mob_name( NULL, auction_info->seller ) );

    RETURN( locate_name );
  }

  /* �Y�O��b�e����, ��X�̥~�h���e�� */
  for ( in_obj = obj; in_obj->in_obj; in_obj = in_obj->in_obj );

  if ( !in_obj || !verify_obj( in_obj ) )
  {
    mudlog( LOG_DEBUG, "object_locate: ���~������." );
    RETURN( "���~" );
  }

   /* ���H��a�� */
  if ( in_obj->carried_by )
  {
    if ( in_obj == obj )
      sprintf( locate_name , "��%s (�Ҧb�a: %d) ����a��"
        , mob_name( ch, in_obj->carried_by )
        , ( in_obj->carried_by->in_room ) ?
          in_obj->carried_by->in_room->vnum : ERRORCODE );

    else
      sprintf( locate_name , "��%s (�Ҧb�a: %d) �˦b�L��%s�̭�"
        , mob_name( ch, in_obj->carried_by )
        , ( in_obj->carried_by->in_room ) ?
          in_obj->carried_by->in_room->vnum : ERRORCODE
        , obj_name( NULL, in_obj ) );
  }

  /* �S���H��a�� */
  else
  {
    if ( in_obj == obj )
    {
      sprintf( locate_name , "�b%s (�Ҧb�a: %d) �̭�"
        , ( in_obj->in_room ) ? in_obj->in_room->name : "�Y�����W�a��"
        , ( in_obj->in_room ) ? in_obj->in_room->vnum : ERRORCODE );
    }

    else
    {
      sprintf( locate_name , "�b%s (�Ҧb�a: %d) �̪�%s��"
        , ( in_obj->in_room ) ? in_obj->in_room->name : "�Y�����W�a��"
        , ( in_obj->in_room ) ? in_obj->in_room->vnum : ERRORCODE
        , obj_name( NULL, in_obj ) );
    }
  }

  RETURN( locate_name );
}

/* ��ť�H�����O�� */
int search_cost( CHAR_DATA * ch )
{
  PUSH_FUNCTION( "search_cost" );
  if ( IS_NPC( ch )
    || IS_IMMORTAL( ch )
    || MaxWhereGold <= 0
    || MinWhereGold <= 0 ) RETURN( 0 );

  RETURN( number_range( MinWhereGold , MaxWhereGold ) );
}

/* �o���˳ƪ����~ */
OBJ_DATA * get_wield( CHAR_DATA * ch )
{
  OBJ_DATA   * obj;
  SITUS_DATA * pSitus;

  PUSH_FUNCTION( "get_wield" );

  for ( obj = ch->carrying; obj; obj = obj->next_content )
  {
    if ( obj->item_type == ITEM_WEAPON )
    {
      for ( pSitus = situs_list; pSitus; pSitus = pSitus->next )
      {
        if ( pSitus->wield && pSitus->location == obj->wear_loc )
          RETURN( obj );
      }
    }
  }

  RETURN( NULL );
}

void light_adjust( CHAR_DATA * ch, OBJ_DATA * obj, bool bAdd )
{
  PUSH_FUNCTION( "light_adjust" );

  if ( obj && obj->item_type == ITEM_LIGHT && obj->value[2] != 0
    && ch && ch->in_room )
  {
     if ( bAdd ) ch->in_room->light++;
     else        ch->in_room->light--;

     ch->in_room->light  = UMAX( 0, ch->in_room->light );
  }

  RETURN_NULL();
}

int dimemsion_position( int dimemsion[], int number , bool Randomize )
{
  int size;
  int position;

  PUSH_FUNCTION( "dimemsion_position" );

  /* �۰ʨ��ü� */
  if ( Randomize == TRUE ) number = number_range( 0, number );

  /* �վ� */
  number = UMIN( 100, UMAX( 0, number ) );

  /* �p��}�C�j�p */
  for ( size = 0; dimemsion[size] >= 0 && dimemsion[size] <= 100; size++ );

  /* �Y�O�}�C�j�p�� 0 */
  if ( size == 0 ) RETURN( ERRORCODE );

  /* �D�o��m */
  for ( position = 0; position < size; position++ )
    if ( dimemsion[position] >= number ) break;

  RETURN( ( position < size ) ? position : ERRORCODE );
}

/* �ˬd�O�_�����ۦP���ܻy, �ΰʧ@ */
bool check_repeat( CHAR_DATA * ch, const char * arg )
{
  DESCRIPTOR_DATA * man;
  int loop;

  PUSH_FUNCTION( "check_repeat" );

  /* �p�G�O�D���a�άO�S���y�z��h���� */
  if ( IS_NPC( ch ) || !( man = ch->desc ) || !*arg || !ChatRecord )
    RETURN( FALSE );

  for ( loop = 0; loop < CHAT_REPEAT; loop++ )
  {
    if ( !man->last_chat[loop] || !*(man->last_chat[loop] ) ) continue;

    /* �p�G�O���ܪ��g�@ */
    if ( !str_cmp( man->last_chat[loop] , arg ) )
    {
      man->chat_error += chat_penalty;
      RETURN( TRUE );
    }
  }

  if ( str_len( arg ) >= ( sizeof( man->last_chat[loop] ) - 1 ) )
  {
    mudlog( LOG_DEBUG, "check_repeat: �r��Ӫ�." );
    RETURN( FALSE );
  }

  /* �վ�}�C */
  for ( loop = 1; loop < CHAT_REPEAT; loop++ )
    str_cpy( man->last_chat[loop - 1] , man->last_chat[loop] );

  /* �O���o�@�����ԭz */
  str_cpy( man->last_chat[CHAT_REPEAT-1] , arg );

  RETURN( FALSE );
}

void delete_chat( CHAR_DATA * ch )
{
  DESCRIPTOR_DATA * man;
  int               loop;

  PUSH_FUNCTION( "delete_chat" );

  /* �p�G�O�D���a�άO�S���y�z��h���� */
  if ( IS_NPC( ch ) || !( man = ch->desc ) ) RETURN_NULL();

  for ( loop = CHAT_REPEAT - 2; loop >= 0; loop-- )
    str_cpy( man->last_chat[loop + 1], man->last_chat[loop] );

  man->last_chat[0][0] = '\x0';
  RETURN_NULL();
}

bool check_cname_xname( char * arg )
{
  XNAMES_DATA * pXname;

  PUSH_FUNCTION( "check_name_xname" );

  /* �W�٪��פ@�w���i�H */
  if ( !arg || !*arg ) RETURN( TRUE );

  for ( pXname = xnames_first; pXname; pXname = pXname->next )
  {
    if ( pXname->type == XNAMES_CNAME && strstr( arg, pXname->word ) )
      RETURN( TRUE );

    /* ����O�d�r�]���i�H��@����W�� */
    if ( pXname->type == XNAMES_CHAT && pXname->word && *pXname->word )
    {
      if ( ( *pXname->word == '@' && str_str( arg, pXname->word + 1 ) )
        || ( *pXname->word != '@' && strstr(  arg, pXname->word     ) ) )
        RETURN( TRUE );
    }
  }

  RETURN( FALSE );
}

bool check_chat_xname( CHAR_DATA * ch, char * arg )
{
  XNAMES_DATA * pXname;
  bool          Case;
  bool          Same;
  bool          fChinese;
  char        * pSource;
  char        * pString;
  char        * pTarget;
  char          Char1;
  char          Char2;
  int           loop;
  int           iLen;
  int           sLen;
  int           dLen;

  PUSH_FUNCTION( "check_chat_xname" );

  if ( !ch || !verify_char( ch ) )
  {
    mudlog( LOG_DEBUG, "check_chat_xname: �ӷ������T." );
    RETURN( FALSE );
  }

  if ( !arg || !*arg ) RETURN( FALSE );

  iLen = str_len( arg );

  for ( pXname = xnames_first; pXname; pXname = pXname->next )
  {
    if ( pXname->type == XNAMES_CHAT && pXname->word && *pXname->word )
    {
      if ( *pXname->word == '@' )
      {
        Case    = TRUE;
        pString = pXname->word + 1;
      }

      else
      {
        Case    = FALSE;
        pString = pXname->word;
      }

      dLen     = str_len( pString );
      fChinese = FALSE;

      for ( sLen = iLen, loop = 0; arg[loop] != '\x0'; loop++, sLen-- )
      {
        if ( dLen > sLen ) break;

        if ( fChinese )
        {
          fChinese = FALSE;
          continue;
        }

        if ( arg[loop] >= '\x81' && arg[loop] <= '\xFF' ) fChinese = TRUE;
        else                                              fChinese = FALSE;

        for ( Same = TRUE, pSource = pString, pTarget = &( arg[loop] )
            ; *pSource
            ; pSource++, pTarget++ )
        {
          Char1 = *pSource;

          if ( ( Char2 = *pTarget ) == '\x0' )
          {
            Same = FALSE;
            break;
          }

          if ( Case )
          {
            Char1 = UPPER( Char1 );
            Char2 = UPPER( Char2 );
          }

          if ( Char1 != Char2 )
          {
            Same = FALSE;
            break;
          }
        }

        if ( Same == TRUE )
        {
          if ( !IS_NPC( ch ) || ch->desc )
          {
            ch->desc->chat_error += chat_penalty;
            if ( ch->pcdata ) ch->pcdata->xname++;
            send_to_char( "�·Ъ`�N�A�����ܥε��T\n\r" , ch );
          }

          mudlog( LOG_LOWLIFE, "%s%s �ϥΤ����r %s"
            , IS_NPC( ch ) ? "[�Ǫ�]" : "", ch->name, arg );

          RETURN( TRUE );
        }
      }
    }
  }

  RETURN( FALSE );
}

CHAR_DATA * shape_mobile( MOB_INDEX_DATA * pMobIndex, ROOM_INDEX_DATA * pRoom )
{
  CHAR_DATA      * mob;
  AREA_DATA      * pArea;
  RESET_DATA     * pReset;
  OBJ_INDEX_DATA * pObjIndex;
  OBJ_DATA       * obj;
  int              vnum;
  int              change;

  PUSH_FUNCTION( "shape_mobile" );

  if ( !pMobIndex || !pRoom )
  {
    mudlog( LOG_DEBUG, "shape_mobile: �ӷ��O�Ū�." );
    RETURN( NULL );
  }

  if ( !( mob = create_mobile( pMobIndex, -1 ) ) )
  {
    mudlog( LOG_DEBUG, "shape_mobile: �ƻs�Ǫ� %d ���~.", pMobIndex->vnum );
    RETURN( NULL );
  }

  char_to_room( mob, pRoom );

  if ( !( pArea = pRoom->area ) )
  {
    mudlog( LOG_DEBUG, "shape_mobile: �Ǫ� %d �䤣���l�ϰ�."
      , pMobIndex->vnum );
    RETURN( mob );
  }

  for ( vnum = pMobIndex->vnum, change = 0, pReset = pArea->reset_first;
    pReset && change != 2; pReset = pReset->next )
  {
    switch( pReset->command )
    {
    default :
      if ( change == 1 ) change = 2;
      break;

    case 'M':

           if ( change != 0 )          change = 2;
      else if ( pReset->arg1 == vnum ) change = 1;
      break;

    case 'E':
    case 'G':

      if ( change != 1 ) continue;

      if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
      {
        mudlog( LOG_DEBUG, "shape_mobile: �Ǫ� %d �䤣�쪫�~���X %d."
          , pMobIndex->vnum , pReset->arg2 );
        continue;
      }

      if ( !( obj = create_object( pObjIndex, -1 ) ) )
      {
        mudlog( LOG_DEBUG, "shape_mobile: �Ǫ� %d �ƻs���~���X %d ���~."
          , pMobIndex->vnum, pReset->arg2 );
        continue;
      }

      obj_to_char( obj, mob );
      if ( pReset->command == 'E' ) equip_char( mob, obj, pReset->arg3 );
      break;
    }
  }
  RETURN( mob );
}

/* �ץ��t���ƶq�άO���q */
void adjust_carry( CHAR_DATA * ch )
{
  OBJ_DATA * obj;
  OBJ_DATA * obj_next;

  PUSH_FUNCTION( "adjust_carry_number" );

  /* �ˬd�ӷ� */
  if ( !ch || !ch->in_room )
  {
    mudlog( LOG_DEBUG, "adjust_carry_number: �ӷ��O�Ū�." );
    RETURN_NULL();
  }

  /* ���ڳ����ݭn�ˬd */
  if ( IS_IMMORTAL( ch ) ) RETURN_NULL();

  for ( obj = ch->carrying; obj; obj = obj_next )
  {
    /* �p�G�ƥةάO���q�W�L */
    if ( ch->carry_number <= can_carry_n( ch )
      && get_carry_weight( ch ) <= can_carry_w( ch ) ) break;

    obj_next = obj->next_content;

    /* �ˬd�ण��� */
    if ( obj->wear_loc == WEAR_NONE
       && can_see_obj( ch, obj )
       && !can_drop_obj( ch, obj )
       && !obj->address )
    {
      obj_from_char( obj );
      obj_to_room( obj, ch->in_room );
      act( "$n�]�t�����F�ӥᱼ$p�C", ch, obj, NULL, TO_ALL );
    }
  }
  RETURN_NULL();
}

void identify_obj( CHAR_DATA * ch, OBJ_DATA * obj )
{
  RESTRICT_DATA  * pRest;
  AFFECT_DATA    * paf;
  CACHET_DATA    * pCachet;
  OBJ_INDEX_DATA * pIndex;
  MOB_INDEX_DATA * pMob;

  PUSH_FUNCTION( "identify_obj" );

  if ( !obj )
  {
    mudlog( LOG_DEBUG, "identify_obj: �ʥF�ӷ�." );
    RETURN_NULL();
  }

  clear_buffer();
  send_to_buffer( "���~�W�١R%s\n\r����W�١R%s\n\r", obj->name, obj->cname );

  if ( str_cmp( obj->pIndexData->short_descr, obj->cname ) )
    send_to_buffer( "��    �W�R%s\n\r", obj->pIndexData->short_descr );

  send_to_buffer(
    "���[�X�СR%s\n\r"
    "��    �O�R%s\n\r"
    "��    �q�R%d ����\n\r"
    "��    �ȡR%d ��\n\r"
    "��    �šR%d ��\n\r"
    "�O�_���r�R%s\n\r"
    "�H �T �~�R%s\n\r"
    "�i�H�ɯšR%s\n\r"
    "�i�H�H��R%s\n\r"
    "�l�a���A�R"
    , extra_bit_name( obj )
    , item_type_name( obj->pIndexData )
    , obj->weight , obj->cost , obj->level
    , YESNO( ( obj->venom > 0 ) )
    , YESNO( obj->Contraband )
    , YESNO( ( obj->pIndexData && obj->pIndexData->exp > 0 ) )
    , YESNO( can_store( obj ) ) );

  if ( is_armor( obj ) && obj->max_armor > 0 )
  {
    send_to_buffer( "%s\n\r"
      ,return_array( ObjArmor, obj->armor * 100 / obj->max_armor
        , sizeof( ObjArmor ) / sizeof( ObjArmor[0] ) ) );
  }

  else
  {
    send_to_buffer( "�o���˳Ƥ��|�l�a\n\r" );
  }

  switch ( obj->item_type )
  {
  default:
  case ITEM_MYSTERY:
  case ITEM_SCROLL:
  case ITEM_POTION:
  case ITEM_PILL:
  case ITEM_FURNITURE:
  case ITEM_TRASH:
  case ITEM_KEY:
  case ITEM_BOAT:
  case ITEM_CORPSE_PC:
  case ITEM_CORPSE_NPC:
  case ITEM_FOUNTAIN:
  case ITEM_TREASURE:
  case ITEM_MONEY:
  case ITEM_MAGICSTONE:
  case ITEM_LETTER:
  case ITEM_GOLDMINE:
  case ITEM_SIGIL:
    break;

  case ITEM_URN:
    send_to_buffer( "�ʦL�H���R" );

    if ( ( pMob = get_mob_index( obj->value[0] ) ) )
      send_to_buffer( "%s\n\r", mob_index_name( NULL, pMob ) );
    else
      send_to_buffer( "�q��\n\r" );

    break;

  case ITEM_FIREWORK:
    send_to_buffer( "�I��Z���R%d ��\n\r", obj->value[0] );

    send_to_buffer( "�M�I�{�סR%s\n\r",
      ( obj->value[1] <= 0 ) ? "�S��" :
      ( ( obj->value[1] <= 50 ) ? "�ǳ\\" : "�ܰ�" ) );

    break;

  case ITEM_VELLUM:
    send_to_buffer( "�ثe���e�R%s\n\r",
      ( obj->description && str_len( obj->description ) >= 2 )
      ? "�w�Q�Ѽg" : "�ťյL�r" );

    break;

  case ITEM_ORE:
    send_to_buffer( "���y����R%d\n\r", obj->value[3] );
    break;

  case ITEM_CONTAINER:
    send_to_buffer( "�s�񭫶q�R%d\n\r", obj->value[0] );
    break;

  case ITEM_DRINK_CON:
    send_to_buffer( "�̤j�e�q�R%d\n\r", obj->value[0] );
    send_to_buffer( "�ثe�e�q�R%d\n\r", obj->value[1] );
    send_to_buffer( "�O�_���r�R%s\n\r", obj->value[3] == 0 ? "�L�r" : "���r" );
    break;

  case ITEM_FOOD:
    send_to_buffer( "�{�����ơR%d\n\r", obj->value[2] );
    send_to_buffer( "�O�_���r�R%s\n\r", obj->value[3] == 0 ? "�L�r" : "���r" );
    break;

  case ITEM_LIGHT:
    send_to_buffer( "�ϥήɶ��R%d �p��\n\r", obj->value[2] );
    break;

  case ITEM_ARROW:
    send_to_buffer( "�����ˮ`�R�� %d �� %d�u�����ˮ` %d�v\n\r"
      , obj->value[0], obj->value[1]
      , ( obj->value[0] + obj->value[1] ) / 2 );

    break;

  case ITEM_WEAPON:
    send_to_buffer( "�Z�������R%s\n\r", weapon_type_name( obj->value[3] ) );
    send_to_buffer( "�����ˮ`�R�� %d �� %d�u�����ˮ` %d�v\n\r"
      , obj->value[1], obj->value[2]
      , ( obj->value[1] + obj->value[2] ) / 2 );
    break;

  case ITEM_ARMOR:
    send_to_buffer( "���@���šR%d ��\n\r", obj->value[0] );
    break;

  case ITEM_BACKBAG:
    send_to_buffer( "�̦h�ƶq�R%d ��\n\r", obj->value[0] );
    send_to_buffer( "�ثe�ƶq�R%d ��\n\r", obj->value[1] );

    if ( ( pIndex = get_obj_index( obj->value[2] ) ) )
    {
      send_to_buffer( "�Z���W�١R%s(%s)\n\r"
        , pIndex->short_descr, pIndex->name );
    }
    else
    {
      send_to_buffer( "�Z���W�١R�q��\n\r" );
    }

    break;

  case ITEM_SPIRITJADE:
    send_to_buffer( "�p�����šR%d ��\n\r", obj->value[1] );
    send_to_buffer( "�p���g��R%d\n\r"   , obj->value[2] );
    break;

  case ITEM_STAFF:
  case ITEM_WAND:
    send_to_buffer( "�W�j�k�N�t�R%s \n\r", skill_rating( obj->value[4] ) );
    send_to_buffer( "�[�j�¤O�R%d %%\n\r", obj->value[5] );
    send_to_buffer( "�{���k�O�R%d�C\n\r", obj->value[6] );
    break;
  }

  for ( paf = obj->pIndexData->affected; paf; paf = paf->next )
  {
    if ( paf->location != APPLY_NONE && paf->modifier != 0 )
    {
      send_to_buffer( "�v    �T�R%s %d �I\n\r"
        , affect_loc_name( paf->location ), paf->modifier );
    }
  }

  for ( paf = obj->affected; paf; paf = paf->next )
  {
    if ( paf->location != APPLY_NONE && paf->modifier != 0 )
    {
      send_to_buffer( "�v    �T�R%s %d �I\n\r"
        , affect_loc_name( paf->location ), paf->modifier );
    }
  }

  for ( paf = obj->pIndexData->impact; paf; paf = paf->next )
  {
    if ( paf->location != APPLY_NONE && paf->modifier != 0 )
    {
      send_to_buffer( "�]�ۮ����R%s %d �I\n\r"
        , affect_loc_name( paf->location ), paf->modifier );
    }
  }

  for ( pCachet = obj->cachet; pCachet; pCachet = pCachet->next )
  {
    if ( !( pIndex = get_obj_index( pCachet->vnum ) ) ) continue;

    for ( paf = pIndex->impact; paf; paf = paf->next )
    {
      send_to_buffer( "�ʦL�]�ۡR%s %d �I\n\r"
        , affect_loc_name( paf->location ), paf->modifier );
    }
  }

  for ( pRest = obj->pIndexData->restrict; pRest; pRest = pRest->next )
    send_to_buffer( "���~����R%s\n\r", restrict_value( pRest, ch ) );

  print_buffer( ch );
  RETURN_NULL();
}

LIQ_DATA * liq_lookup( int slot )
{
  LIQ_DATA * pLiq;

  PUSH_FUNCTION( "liq_lookup" );

  for ( pLiq = liq_list; pLiq; pLiq = pLiq->next )
    if ( pLiq->slot == slot ) break;

  RETURN( pLiq );
}

bool check_restrict( CHAR_DATA * ch, RESTRICT_DATA * pRest, bool fPrint )
{
  SKILL_DATA * pSkill;

  PUSH_FUNCTION( "check_restrict" );

  if ( !ch || !pRest ) RETURN( FALSE );

  switch( pRest->type )
  {
  default:
    if ( fPrint ) send_to_char( "�o�O���򰭪F��M�ϥ��A����ΡC\n\r", ch );
    RETURN( FALSE );

  case RES_STR:
    if ( get_curr_str( ch ) >= pRest->value ) RETURN( TRUE );
    if ( fPrint ) send_to_char( "�藍�_�M�A���O�q�����C\n\r", ch );
    RETURN( FALSE );

  case RES_INT:
    if ( get_curr_int( ch ) >= pRest->value ) RETURN( TRUE );
    if ( fPrint ) send_to_char( "�藍�_�M�A�����O�����C\n\r", ch );
    RETURN( FALSE );

  case RES_WIS:
    if ( get_curr_wis( ch ) >= pRest->value ) RETURN( TRUE );
    if ( fPrint ) send_to_char( "�藍�_�M�A�����Ѥ����C\n\r", ch );
    RETURN( FALSE );

  case RES_DEX:
    if ( get_curr_dex( ch ) >= pRest->value ) RETURN( TRUE );
    if ( fPrint ) send_to_char( "�藍�_�M�A���ӱ������C\n\r", ch );
    RETURN( FALSE );

  case RES_CON:
    if ( get_curr_con( ch ) >= pRest->value ) RETURN( TRUE );
    if ( fPrint ) send_to_char( "�藍�_�M�A����椣���C\n\r", ch );
    RETURN( FALSE );

  case RES_HP:
    if ( get_curr_hit( ch ) >= pRest->value ) RETURN( TRUE );
    if ( fPrint ) send_to_char( "�藍�_�M�A���ͩR�O�����C\n\r", ch );
    RETURN( FALSE );

  case RES_MANA:
    if ( get_curr_mana( ch ) >= pRest->value ) RETURN( TRUE );
    if ( fPrint ) send_to_char( "�藍�_�M�A���k�O�����C\n\r", ch );
    RETURN( FALSE );

  case RES_MOVE:
    if ( get_curr_move( ch ) >= pRest->value ) RETURN( TRUE );
    if ( fPrint ) send_to_char( "�藍�_�M�A�����ʤO�����C\n\r", ch );
    RETURN( FALSE );

  case RES_CLASS:

    if ( ch->class && IS_SET( pRest->value, ch->class->mask ) ) RETURN( TRUE );
    if ( fPrint ) send_to_char( "�藍�_�M�A��¾�~����C\n\r", ch );
    RETURN( FALSE );

  case RES_TRUST:
    if ( get_trust( ch ) >= pRest->value ) RETURN( TRUE );
    if ( fPrint ) send_to_char( "�藍�_�M�A���H���פ����C\n\r", ch );
    RETURN( FALSE );

  case RES_SKILL:

    if ( pRest->value < 0
      || pRest->value >= MAX_SKILL
      || ch->skill[pRest->value] < pRest->vicevalue )
    {
      pSkill = get_skill( pRest->value );
      if ( fPrint ) act( "�藍�_�M�A��$t�m���٤���a�C"
        , ch , pSkill ? pSkill->cname : "�Y�اޯ�", NULL, TO_CHAR );

      RETURN( FALSE );
    }

    RETURN( TRUE );

  case RES_NOSKILL:

    if ( pRest->value < 0
      || pRest->value >= MAX_SKILL
      || ch->skill[pRest->value] > pRest->vicevalue )
    {
      pSkill = get_skill( pRest->value );
      if ( fPrint ) act( "�藍�_�M�A��$t�M���۽Ĭ۫g�T"
        , ch , pSkill ? pSkill->cname : "�Y�اޯ�", NULL, TO_CHAR );

      RETURN( FALSE );
    }

    RETURN( TRUE );

  case RES_SEX:

    if ( ch->sex == pRest->value ) RETURN( TRUE );
    if ( fPrint ) send_to_char( "�藍�_�M�A���ʧO����C\n\r", ch );
    RETURN( FALSE );

  case RES_LEVEL:
    if ( ch->level >= pRest->value ) RETURN( TRUE );
    if ( fPrint ) send_to_char( "�藍�_�M�A�����Ť����C\n\r", ch );
    RETURN( FALSE );

  case RES_ALIGN:

    if ( pRest->vicevalue )
    {
      if ( ch->alignment >= pRest->value ) RETURN( TRUE );
      if ( fPrint ) send_to_char( "�藍�_�M�A���}���٤����n�T\n\r", ch );
      RETURN( FALSE );
    }

    if ( ch->alignment <= pRest->value ) RETURN( TRUE );
    if ( fPrint ) send_to_char( "�藍�_�M�A���}���٤������c�T\n\r", ch );
    RETURN( FALSE );
  }

  RETURN( TRUE );
}

bool check_obj_restrict( CHAR_DATA * ch, OBJ_DATA * obj , bool fPrint )
{
  RESTRICT_DATA * pRest;

  PUSH_FUNCTION( "check_obj_restrict" );

  if ( !ch || !obj || !obj->pIndexData )
  {
    mudlog( LOG_DEBUG, "check_obj_restrict: �ӷ������T." );
    RETURN( FALSE );
  }

  for ( pRest = obj->pIndexData->restrict; pRest; pRest = pRest->next )
    if ( !check_restrict( ch, pRest, fPrint ) ) RETURN( FALSE );

  RETURN( TRUE );
}

bool check_skill_restrict( CHAR_DATA * ch, SKILL_DATA * pSkill, bool fPrint )
{
  RESTRICT_DATA * pRest;

  PUSH_FUNCTION( "check_skill_restrict" );

  if ( !ch || !pSkill )
  {
    mudlog( LOG_DEBUG, "check_skill_restrict: �ӷ������T." );
    RETURN( FALSE );
  }

  for ( pRest = pSkill->restrict; pRest; pRest = pRest->next )
    if ( !check_restrict( ch, pRest, fPrint ) ) RETURN( FALSE );

  RETURN( TRUE );
}

bool check_skill_cast( CHAR_DATA * ch, SKILL_DATA * pSkill, bool fPrint )
{
  RESTRICT_DATA * pRest;

  PUSH_FUNCTION( "check_skill_cast" );

  if ( !ch || !pSkill )
  {
    mudlog( LOG_DEBUG, "check_skill_cast: �ӷ������T." );
    RETURN( FALSE );
  }

  for ( pRest = pSkill->restrict; pRest; pRest = pRest->next )
  {
    switch( pRest->type )
    {
    default:
      if ( !check_restrict( ch, pRest, fPrint ) ) RETURN( FALSE );
      break;

    case RES_SKILL:
      break;
    }
  }

  RETURN( TRUE );
}

time_t time_at( char * timestring )
{
  char        sTime[MAX_INPUT_LENGTH];
  char      * sHour;
  char      * sMin;
  char      * sSec;
  struct tm * brokendown;
  time_t      nowtime;
  time_t      future;
  int         hour;
  int         min;
  int         sec;
  int         toggle = 0;

  PUSH_FUNCTION( "time_at" );

  if ( !timestring || *timestring == '\x0' ) RETURN( -1 );
  str_cpy( sTime, timestring );

  if ( UPPER( *( sTime + str_len( sTime ) - 1 ) ) == 'P' )
  {
    *( sTime + str_len( sTime ) - 1 ) = '\x0';
    toggle = 12;
  }

  nowtime = time( NULL );
  if ( !( brokendown = localtime( &nowtime ) ) ) RETURN( -1 );

  sHour = sTime;
  if ( !( sMin  = strchr( sTime   , ':' ) ) ) RETURN( -1 );
  if ( !( sSec  = strchr( sMin + 1, ':' ) ) ) RETURN( -1 );

  *sMin = '\x0';
  *sSec = '\x0';
  sMin++;
  sSec++;

  if ( !is_number( sHour ) || !is_number( sMin ) || !is_number( sSec ) )
    RETURN( -1 );

  if ( ( hour = atoi( sHour ) + toggle ) >= 24 || hour < 0 ) RETURN( -1 );
  if ( ( min  = atoi( sMin ) )           >= 60 || min  < 0 ) RETURN( -1 );
  if ( ( sec  = atoi( sSec ) )           >= 60 || min  < 0 ) RETURN( -1 );

  brokendown->tm_hour = hour;
  brokendown->tm_min  = min;
  brokendown->tm_sec  = sec;

  if ( ( future = mktime( brokendown ) ) < nowtime )
  {
    brokendown->tm_mday++;
    future = mktime( brokendown );
  }

  RETURN( future );
}

bool on_line( const char * name, CHAR_DATA * ch )
{
  DESCRIPTOR_DATA * man;
  CHAR_DATA       * victim;

  PUSH_FUNCTION( "on_line" );

  if ( !ch || !name || !*name )
  {
    mudlog( LOG_DEBUG, "on_line: �ӷ������T." );
    RETURN( FALSE );
  }

  if ( name && *name )
  {
    for ( man = descriptor_list; man; man = man->next )
    {
      if ( verify_desc( man )
        && man->connected == CON_PLAYING
        && ( victim = man->character )
        && victim->name
        && !str_cmp( name, victim->name )
        && can_see( ch, victim ) )
          RETURN( TRUE );
    }
  }

  RETURN( FALSE );
}

bool is_keeper( CHAR_DATA * pChar )
{
  PUSH_FUNCTION( "is_keeper" );

  if ( !pChar )
  {
    mudlog( LOG_DEBUG, "is_keeper: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( !verify_char( pChar )
    || !IS_NPC( pChar )
    || !pChar->pIndexData
    || !pChar->pIndexData->pShop ) RETURN( FALSE );

  RETURN( TRUE );
}

bool biliwick( CHAR_DATA * ch, CHAR_DATA * victim )
{
  PUSH_FUNCTION( "biliwick" );

  if ( !ch || !victim )
  {
    mudlog( LOG_DEBUG, "biliwick: �ӷ����~." );
    RETURN( FALSE );
  }

  if ( ch == victim
    || is_couple( ch, victim )
    || ( IS_IMMORTAL( ch ) && get_trust( ch ) >= get_trust( victim ) ) )
    RETURN( TRUE );

  RETURN( FALSE );
}

void fix_color( char * name )
{
  const char * sample = "\e[0m";
  char       * pStr;

  PUSH_FUNCTION( "fix_color" );

  if ( !name )
  {
    mudlog( LOG_DEBUG, "fix_color: �ʥF�ӷ�" );
    RETURN_NULL();
  }

  for ( pStr = name; *pStr; pStr++ )
  {
    if ( *pStr == '\e' && *( pStr + 1 ) == '[' )
    {
      if ( ( pStr = name + str_len( name ) - str_len( sample ) ) < name
        || !str_cmp( pStr, sample ) ) RETURN_NULL();

      str_cat( name, sample );
      RETURN_NULL();
    }
  }
  RETURN_NULL();
}

bool can_damage( CHAR_DATA * ch )
{
  PUSH_FUNCTION( "can_damage" );

  if ( !ch || !verify_char( ch ) || is_dead( ch ) || ch->cease )
    RETURN( FALSE );

  RETURN( TRUE );
}

bool is_dead( CHAR_DATA * ch )
{
  PUSH_FUNCTION( "is_dead" );

  if ( !ch || !verify_char( ch ) )
  {
    mudlog( LOG_DEBUG, "is_dead: �H���ӷ������T." );
    RETURN( TRUE );
  }

  if ( ch->position == POS_DEAD || ( ch->pcdata && ch->pcdata->corpse ) )
    RETURN( TRUE );

  RETURN( FALSE );
}

bool check_valid_rating( CHAR_DATA * ch, SKILL_DATA * pSkill, bool fPrint )
{
  SKILL_DATA * aSkill;

  PUSH_FUNCTION( "check_valid_rating" );

  if ( !ch || !pSkill )
  {
    mudlog( LOG_DEBUG, "check_valid_rating: �ӷ����~." );
    RETURN( FALSE );
  }

  for ( aSkill = skill_list; aSkill; aSkill = aSkill->next )
  {
    if ( ch->skill[ aSkill->slot ] > 0
      && aSkill->rating     > 0
      && pSkill->antirating > 0
      && pSkill->antirating == aSkill->rating
      && !IS_IMMORTAL( ch ) )
    {
      if ( fPrint ) act( "�A��$w�ޯ�P$W�ޯ�۽Ĭ�C"
        , ch, pSkill, aSkill, TO_CHAR );

      if ( ch->skill[ pSkill->slot ] != 0 ) cancel_skill( ch, pSkill );

      RETURN( FALSE );
    }
  }

  RETURN( TRUE );
}

bool can_char_from_room( CHAR_DATA * ch, bool fPrint )
{
  PUSH_FUNCTION( "can_char_from_room" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "can_char_from_room: �ӷ������T." );
    RETURN( FALSE );
  }

  if ( !ch->in_room )
  {
    if ( fPrint ) send_to_char( "�A�b�e�z���ҡM�Q���������F�T\n\r", ch );
    RETURN( FALSE );
  }

  if ( ch->mount )
  {
    if ( fPrint ) send_to_char( "�A�٦b���M�W�C�M���U�ӧa�T\n\r", ch );
    RETURN( FALSE );
  }

  if ( ch->mount_by )
  {
    if ( fPrint ) send_to_char( "�A�ٳQ�H�M�۫��T\n\r", ch );
    RETURN( FALSE );
  }

  RETURN( TRUE );
}

/* �����ޯ�åB�����P�� */
void cancel_skill( CHAR_DATA * ch, SKILL_DATA * pSkill )
{
  ENABLE_DATA * pEnable;
  ENABLE_DATA * zEnable;
  int           slot;

  PUSH_FUNCTION( "cancel_skill" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "cancel_skill: �ʥF�ӷ�." );
    RETURN_NULL();
  }

  /* �Ǫ��@�w�������� */
  if ( IS_NPC( ch ) ) RETURN_NULL();

  slot = pSkill->slot;
  if ( slot < 0 || slot >= MAX_SKILL )
  {
    mudlog( LOG_DEBUG, "cancel_skill: �ޯู�X %d ���X�z.", slot );
    RETURN_NULL();
  }

  if ( ch->skill[slot] <= 0 )
  {
    mudlog( LOG_DEBUG, "cancel_skill: �쥻�ޯ���m�׬O�s." );
    RETURN_NULL();
  }

  ch->skill[slot] = 0;

  /* �����L���o���ޯ઺�P�� */
  for ( pEnable = ch->enable; pEnable; pEnable = zEnable )
  {
    zEnable = pEnable->next;
    if ( pEnable->skill == pSkill ) extract_an_enable( ch, pSkill );
  }

  RETURN_NULL();
}

char * host_name( DESCRIPTOR_DATA * pDesc )
{
  static char buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "host_name" );

  if ( !pDesc )
  {
    mudlog( LOG_DEBUG, "host_name: �ʥF�ӷ�." );
    RETURN( "����" );
  }

  sprintf( buf, "%s%s%s"
    , pDesc->remote && pDesc->remote[0] ? pDesc->remote : ""
    , pDesc->remote && pDesc->remote[0] ? "@"           : ""
    , pDesc->host   && pDesc->host[0]   ? pDesc->host   : "(����)" );

  RETURN( buf );
}

bool poison_char( CHAR_DATA * ch, OBJ_DATA * obj )
{
  AFFECT_DATA af;

  PUSH_FUNCTION( "poison_obj" );

  if ( !ch || !obj )
  {
    mudlog( LOG_DEBUG, "poison_obj: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( obj->venom <= 0 || get_curr_str( ch ) <= 2 ) RETURN( FALSE );

  act( "�V�F�M$p�ݰ_�ӬO���٬r���M�A���r�F�T", ch, obj, NULL, TO_CHAR );
  act( "$n�N��$p�M���O$p�ݰ_�ӬO���r���T"    , ch, obj, NULL, TO_ROOM );

  af.type      = SLOT_POISON;
  af.duration  = obj->venom;
  af.location  = APPLY_STR;
  af.modifier  = -2;
  af.level     = obj->level;
  affect_join( ch, &af );

  RETURN( TRUE );
}

int get_adeptation( SKILL_DATA * pSkill, MOB_INDEX_DATA * pIndex )
{
  int           adept;
  int           total;
  int           value;
  int           chance;
  int           loop;
  int           totalvalue;
  int           level;
  DAMAGE_DATA * pDam;

  PUSH_FUNCTION( "get_adeptation" );

  if ( !pSkill
    || !pSkill->damage
    || !pIndex
    || ( level = pIndex->level ) <= 0
    || level >= MAX_LEVEL )
    RETURN( -1 );

  for ( total = 0, pDam = pSkill->damage; pDam; pDam = pDam->next )
    total += pDam->chance;

  if ( total <= 0 ) RETURN( -1 );

  switch( pSkill->type )
  {
  default:
    RETURN( -1 );

  case TAR_CHAR_OFFENSIVE:

    value = kill_table[level].damage * pIndex->attack_ratio / 100;
    break;

  case TAR_DODGE:

    value = kill_table[level].dodge * pIndex->dodge_ratio / 100;
    break;
  }

  pDam = pSkill->damage;

  for ( totalvalue = chance = 0, loop = 1; loop <= total; loop++ )
  {
    if ( ++chance > pDam->chance )
    {
      if ( ( pDam = pDam->next ) == NULL ) RETURN( -1 );
      chance = 1;
    }

    totalvalue += pDam->value;
    if ( ( totalvalue / loop ) > value ) break;
  }

  adept = UMAX( 1, ( UMIN( 100, ( loop * 100 / total ) ) ) );
  RETURN( adept );
}

void calculate_armor( CHAR_DATA * ch )
{
  SITUS_DATA  * pSitus;
  OBJ_DATA    * obj;
  AFFECT_DATA * pAff;

  PUSH_FUNCTION( "calculate_armor" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "calculate_armor: �ʥF�ӷ�." );
    RETURN_NULL();
  }

  ch->armor   = 0;
  ch->hitroll = 0;

  for ( pSitus = situs_list; pSitus; pSitus = pSitus->next )
  {
    if ( !( obj = get_eq_char( ch, pSitus->location ) ) || !obj->pIndexData )
      continue;

    ch->armor -= apply_ac( obj, obj->wear_loc );

    for ( pAff = obj->pIndexData->affected; pAff; pAff = pAff->next )
    {
      if ( pAff->location == APPLY_AC      ) ch->armor   += pAff->modifier;
      if ( pAff->location == APPLY_HITROLL ) ch->hitroll += pAff->modifier;
    }
  }

  for ( pAff = ch->affected; pAff; pAff = pAff->next )
  {
    if ( pAff->location == APPLY_AC      ) ch->armor   += pAff->modifier;
    if ( pAff->location == APPLY_HITROLL ) ch->hitroll += pAff->modifier;
  }

  RETURN_NULL();
}

AREA_DATA * get_home( CHAR_DATA * ch )
{
  AREA_DATA * pArea;

  PUSH_FUNCTION( "check_home" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "check_home: �ӷ������T." );
    RETURN( NULL );
  }

  if ( ch->home < 0 || IS_NPC( ch ) ) RETURN( NULL );

  for ( pArea = area_first; pArea; pArea = pArea->next )
    if ( pArea->capital && pArea->serial == ch->home ) RETURN( pArea );

  ch->home = -1;
  RETURN( NULL );
}

ROOM_INDEX_DATA * get_hometown( CHAR_DATA * ch )
{
  AREA_DATA       * pArea;

  PUSH_FUNCTION( "get_hometown" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "get_hometown: �ʥF�ӷ�." );
    RETURN( RoomRecall );
  }

  if ( IS_NPC( ch ) || !( pArea = get_home( ch ) ) || !pArea->capital )
    RETURN( RoomRecall );

  RETURN( pArea->capital );
}

void set_variable( const char * keyword, int value, int hours )
{
  VARIABLE_DATA * pVar;

  PUSH_FUNCTION( "set_variable" );

  if ( !keyword || !*keyword )
  {
    mudlog( LOG_DEBUG, "set_variable: �ʥF�ӷ�." );
    RETURN_NULL();
  }

  if ( hours <= 0 )
  {
    mudlog( LOG_DEBUG, "set_variable: �ɶ����X�z." );
    RETURN_NULL();
  }

  for ( pVar = variable_list; pVar; pVar = pVar->next )
  {
    if ( pVar->keyword
      && *pVar->keyword
      && !str_cmp( keyword, pVar->keyword ) ) break;
  }

  if ( !pVar )
  {
    pVar          = alloc_struct( STRUCT_VARIABLE_DATA );
    pVar->keyword = str_dup( keyword );
    pVar->next    = variable_list;
    variable_list = pVar;
  }

  pVar->value = value;
  pVar->hours = hours;

  RETURN_NULL();
}

int check_variable( const char * keyword, int value )
{
  VARIABLE_DATA * pVar;

  PUSH_FUNCTION( "check_variable" );

  if ( !keyword || !*keyword )
  {
    mudlog( LOG_DEBUG, "set_variable: �ʥF�ӷ�." );
    RETURN( VARIABLE_ILLEGAL );
  }

  for ( pVar = variable_list; pVar; pVar = pVar->next )
  {
    if ( pVar->keyword
      && * pVar->keyword
      && !str_cmp( pVar->keyword, keyword ) )
    {
      if ( pVar->value == value ) RETURN( VARIABLE_SAME );
      RETURN( VARIABLE_DIFFERENT );
    }
  }

  RETURN( VARIABLE_NOT_DEFINE );
}

bool char_autofood( CHAR_DATA * ch )
{
  OBJ_DATA * pObj;

  PUSH_FUNCTION( "char_autofood" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "char_autofood: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( IS_NPC( ch ) || !ch->desc ) RETURN( FALSE );

  for ( pObj = ch->carrying; pObj; pObj = pObj->next_content )
    act( "$t $P", ch, YESNO( pObj->item_type == ITEM_FOOD ), pObj, TO_CHAR );

  RETURN( TRUE );
}

size_t get_learnable( CHAR_DATA * ch )
{
  PUSH_FUNCTION( "get_learnable" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "get_learnable: �ʥF�ӷ�." );
    RETURN( 0 );
  }

  if ( IS_NPC( ch ) || IS_IMMORTAL( ch ) ) RETURN( MAX_SKILL );

  if ( !ch->class )
  {
    mudlog( LOG_DEBUG, "get_learnable: %s �ʥF¾�~.", ch->name );
    RETURN( 0 );
  }

  RETURN( ch->class->nskill + ch->nskill );
}

const char * numberize( int number, char * address )
{
  static char   buf[MAX_STRING_LENGTH];
  char        * pString;
  char        * source;
  int           num;
  int           factor;
  int           digit;
  int           loop;

  PUSH_FUNCTION( "numberize" );

  source = pString = address ? address : buf;

  if ( number < 0 )
  {
    number = -number;
    if ( number < 0 )
    {
      sprintf( pString, "-%u", -number );
      RETURN( source );
    }

    *pString++ = '-';
  }

  num = number;
  for ( num = number, digit = 1; ; digit++ )
  {
    num /= 10;
    if ( num <= 0 ) break;
  }

  for ( loop = digit, factor = 1; digit > 1; factor *= 10, digit-- );

  while ( factor > 0 )
  {
    *pString++ = ( number / factor ) + '0';

    if ( ( --loop % 3 ) == 0 && loop > 0 ) *pString++ = ',';
    number %= factor;
    factor /= 10;
  }

  *pString = '\x0';
  RETURN( source );
}

bool is_server( const char * address )
{
  SERVER_DATA * pServer;
  char          alpha[MAX_STRING_LENGTH];
  char          number[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "is_server" );

  if ( !address || !*address ) RETURN( FALSE );

  str_cpy( alpha , address_mode( address, ADDRESS_ALPHA  ) );
  str_cpy( number, address_mode( address, ADDRESS_NUMBER ) );

  for ( pServer = server_list; pServer; pServer = pServer->next )
  {
    if ( !str_cmp( pServer->address, alpha )
      || !str_cmp( pServer->address, number ) ) RETURN( TRUE );
  }

  RETURN( FALSE );
}

int personal_asset( CHAR_DATA * ch )
{
  int stock;
  int total;

  PUSH_FUNCTION( "personal_asset" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "personal_asset: �ʥF�ӷ�." );
    RETURN( 0 );
  }

  if ( ch->gold < 0 )
  {
    ch->trade = FALSE;
    RETURN( MaxNumber );
  }

  if ( ch->bank < 0 )
  {
    ch->trade = FALSE;
    RETURN( MaxNumber );
  }

  if ( ( stock = stock_value( ch ) ) < 0 )
  {
    ch->trade = FALSE;
    RETURN( MaxNumber );
  }

  total = ch->gold + ch->bank;
  if ( total < 0 || total < ch->gold || total < ch->bank )
  {
    ch->trade = FALSE;
    RETURN( MaxNumber );
  }

  total += stock;
  if ( total < 0 || total < stock )
  {
    ch->trade = FALSE;
    RETURN( MaxNumber );
  }

  RETURN( total );
}

bool over_scale( CHAR_DATA * ch )
{
  int total1;
  int total2;

  PUSH_FUNCTION( "over_scale" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "over_scale: �ʥF�ӷ�." );
    RETURN( TRUE );
  }

  total1 = ch->gold + ch->bank + stock_value( ch );
  total2 = personal_asset( ch );

  if ( total1 != total2 || total2 > MAX_ASSET ) RETURN( TRUE );

  RETURN( FALSE );
}

char * return_array( ARRAY_DATA pArray[], int number, int size )
{
  int loop;

  PUSH_FUNCTION( "return_array" );

  if ( number >= pArray[0].number ) RETURN( pArray[0].message );

  for ( loop = 0; loop < size - 1; loop++ )
    if ( number >= pArray[loop].number ) RETURN( pArray[loop].message );

  RETURN( pArray[size-1].message );
}

char * obj_name( CHAR_DATA * ch, OBJ_DATA * obj )
{
  static char buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "obj_name" );

  buf[0] = '\x0';
  if ( !ch || IS_NPC( ch ) || IS_SET( ch->act, PLR_EXACT ) )
  {
    sprintf( buf, "%s(%s)", obj->cname, obj->name );
  }

  else
  {
    str_cpy( buf, obj->cname );
  }
  RETURN( buf );
}

char * mob_name( CHAR_DATA * ch, CHAR_DATA * victim )
{
  static char buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "mob_name" );

  buf[0] = '\x0';

  if ( ch && !can_see( ch, victim ) )
  {
    RETURN( "�Y�����W�H��" );
  }
  else if ( ch && ch == victim )
  {
    RETURN( "�A" );
  }

  if ( IS_NPC( victim ) )
  {
    if ( !ch || IS_NPC( ch ) || IS_SET( ch->act, PLR_EXACT ) )
    {
      sprintf( buf, "%s(%s)", victim->byname, victim->name );
    }

    else
    {
      str_cpy( buf, victim->byname );
    }
  }
  else
  {
    if ( !ch || IS_NPC( ch ) || IS_SET( ch->act, PLR_EXACT ) )
    {
      sprintf( buf, "%s(%s)", victim->cname, victim->name );
    }

    else
    {
      str_cpy( buf, victim->byname );
    }
  }
  RETURN( buf );
}

char * mob_index_name( CHAR_DATA * ch, MOB_INDEX_DATA * pIndex )
{
  static char buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "mob_index_name" );

  buf[0] = '\x0';
  if ( !ch || IS_NPC( ch ) || IS_SET( ch->act, PLR_EXACT ) )
  {
    sprintf( buf, "%s(%s)", pIndex->short_descr, pIndex->name );
  }

  else
  {
    str_cpy( buf, pIndex->short_descr );
  }
  RETURN( buf );
}

void clear_return( char * source, char * target )
{
  PUSH_FUNCTION( "clear_return" );

  if ( !source || !target )
  {
    mudlog( LOG_DEBUG, "clear_return: �ʥF�ӷ�." );
    RETURN_NULL();
  }

  for ( ; *source; source++ ) if ( *source != '\r' ) *target++ = *source;
  *target = '\x0';

  RETURN_NULL();
}

bool can_leaveroom( CHAR_DATA * ch, bool fPrint )
{
  PUSH_FUNCTION( "can_leaveroom" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "can_leaveroom: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( !ch->in_room ) RETURN( FALSE );

  if ( ch->position != POS_STANDING )
  {
    if ( fPrint ) send_to_char( "�藍�_�M�Х��ѨM�A���Ʊ��a�T\n\r", ch );
    RETURN( FALSE );
  }

  if ( ch->mount || ch->mount_by )
  {
    if ( fPrint ) send_to_char( "�A�����M�C�M���U�ӧa�C\n\r", ch );
    RETURN( FALSE );
  }

  if ( ch->spirit )
  {
    if ( fPrint ) act( "�A����A��$N���_�ӧa�M���M$N�O�䤣��A���T"
      , ch, NULL, ch->spirit, TO_CHAR );

    RETURN( FALSE );
  }

  if ( is_pk( ch ) )
  {
    if ( fPrint ) send_to_char( "�A���b�Z���j�|�̡M�S����k�ǰe�A�C�T\n\r", ch );
    RETURN( FALSE );
  }

  if ( ch->failed > 0 )
  {
    if ( fPrint ) send_to_char( "�藍�_�M�A�٦b��L���M�L�k�ǰe�T\n\r", ch );
    RETURN( FALSE );
  }

  if ( ch->jail > 0 )
  {
    if ( fPrint ) send_to_char( "�藍�_�M�A�٦b���c�M�L�k�ǰe�T\n\r", ch );
    RETURN( FALSE );
  }

  if ( ch->in_room->NoRecall || is_affected( ch, SLOT_CURSE ) )
  {
    if ( fPrint ) send_to_char( "�藍�_�M�����F�A�C\n\r", ch );
    RETURN( FALSE );
  }

  RETURN( TRUE );
}

bool is_exist( const char * name )
{
  char filename[MAX_FILE_LENGTH];

  PUSH_FUNCTION( "is_exist" );

  if ( !name || !*name || !check_parse_name( (char * ) name ) ) RETURN( FALSE );
  str_cpy( filename, file_name( name, SAVE_FILE ) );
  if ( !access( filename, R_OK ) ) RETURN( TRUE );

  RETURN( FALSE );
}

char * binary_bit( int number )
{
  int         loop;
  static char buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "binary_bit" );

  loop = sizeof( buf ) - 1;
  buf[loop--] = '\x0';
  buf[loop--] = 'b';

  for ( ; ; loop-- )
  {
    buf[loop] = ( number % 2 ) + '0';
    number /= 2;

    if ( number <= 0 ) RETURN( buf + loop );
  }

  RETURN( buf );
}

SITUS_DATA * situs_lookup( int location )
{
  SITUS_DATA * pSitus;

  PUSH_FUNCTION( "situs_lookup" );

  for ( pSitus = situs_list; pSitus; pSitus = pSitus->next )
    if ( pSitus->location == location ) RETURN( pSitus );

  RETURN( NULL );
}

bool affect_login_check( CHAR_DATA * ch )
{
  PUSH_FUNCTION( "affect_login_check" );

  if ( IS_NPC( ch ) || !verify_char( ch ) )
    RETURN( TRUE );

  affect_recount( ch );

  if ( !check_class_attr( ch ) ) RETURN( FALSE );

  RETURN( TRUE );
}

bool check_class_attr( CHAR_DATA * ch )
{
  CLASS_DATA * pClass;

  PUSH_FUNCTION( "check_class_attr" );

  /* �Ǫ��S�������ݩ�, ���ڤ]�S�� */
  if ( !ch
    || !verify_char( ch )
    || IS_NPC( ch )
    || !( pClass = ch->class )
    || IS_IMMORTAL( ch ) )
    RETURN( TRUE );

  if ( ch->now_str > pClass->attr[STR_ITEM]
    || ch->now_int > pClass->attr[INT_ITEM]
    || ch->now_wis > pClass->attr[WIS_ITEM]
    || ch->now_dex > pClass->attr[DEX_ITEM]
    || ch->now_con > pClass->attr[CON_ITEM] )
    RETURN( FALSE );

  RETURN( TRUE );
}

void affect_recount( CHAR_DATA * ch )
{
  OBJ_DATA       * pObj;
  AFFECT_DATA    * paf;
  OBJ_INDEX_DATA * pIndex;
  CACHET_DATA    * pCachet;
  SITUS_DATA     * pSitus;

  PUSH_FUNCTION( "affect_recount" );

  if ( !ch || IS_NPC( ch ) || !verify_char( ch ) ) RETURN_NULL();

  /* ���k�s */
  if ( ch->pcdata )
  {
    ch->pcdata->mod_str = 0;
    ch->pcdata->mod_int = 0;
    ch->pcdata->mod_wis = 0;
    ch->pcdata->mod_dex = 0;
    ch->pcdata->mod_con = 0;
  }

  for ( pSitus = situs_list; pSitus; pSitus = pSitus->next )
  {
    if ( !( pObj = get_eq_char( ch, pSitus->location ) ) ) continue;

    for ( paf = pObj->pIndexData->affected; paf; paf = paf->next )
      affect_modify_attr( ch, paf, TRUE );

    for ( paf = pObj->affected; paf; paf = paf->next )
      affect_modify_attr( ch, paf, TRUE );

    for ( pCachet = pObj->cachet; pCachet; pCachet = pCachet->next )
    {
      if ( !( pIndex = get_obj_index( pCachet->vnum ) ) ) continue;

      for ( paf = pIndex->impact; paf; paf = paf->next )
        affect_modify_attr( ch, paf, TRUE );
    }
  }

  for ( paf = ch->affected; paf; paf = paf->next )
    affect_modify_attr( ch, paf, TRUE );

  RETURN_NULL();
}

/* �����s�p���ݩʥ� */
void affect_modify_attr( CHAR_DATA * ch, AFFECT_DATA * paf, bool fAdd )
{
  int mod;

  PUSH_FUNCTION( "affect_modify_attr" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  /* fAdd �Y���u�h�W�[�׹���, �Ϥ��ۤ� */
  mod = fAdd ? paf->modifier : -paf->modifier;

  switch ( paf->location )
  {
  default:
    mudlog( LOG_DEBUG , "Affect_modify_attr: �������a�� %d.", paf->location );
    RETURN_NULL();

  case APPLY_NONE:
  case APPLY_CLASS:
  case APPLY_LEVEL:
  case APPLY_AGE:
  case APPLY_HEIGHT:
  case APPLY_WEIGHT:
  case APPLY_GOLD:
  case APPLY_EXP:
  case APPLY_SEX:
  case APPLY_MANA:
  case APPLY_HIT:
  case APPLY_MOVE:
  case APPLY_AC:
  case APPLY_HITROLL:
  case APPLY_DAMROLL:
  case APPLY_SAVING_PARA:
  case APPLY_SAVING_ROD:
  case APPLY_SAVING_PETRI:
  case APPLY_SAVING_BREATH:
  case APPLY_SAVING_SPELL:
    break;

  case APPLY_STR:           ch->pcdata->mod_str += mod; break;
  case APPLY_DEX:           ch->pcdata->mod_dex += mod; break;
  case APPLY_INT:           ch->pcdata->mod_int += mod; break;
  case APPLY_WIS:           ch->pcdata->mod_wis += mod; break;
  case APPLY_CON:           ch->pcdata->mod_con += mod; break;
  }

  RETURN_NULL();
}

char * smash_act_keyword( char * buf, char * key )
{
  int           loop;
  bool          bFound;
  static char   spool[MAX_STRING_LENGTH];
  char        * pString;

  PUSH_FUNCTION( "smash_act_keyword" );

  for ( loop = 0; buf[loop]; loop++ )
  {
    if ( buf[loop] == '$' )
    {
      switch( buf[loop+1] )
      {
      case '$': case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case 'A': case 'B': case 'C':
        bFound = TRUE;
        break;

      default:

        for ( bFound = FALSE, pString = key; *pString; pString++ )
        {
          if ( buf[loop+1] == *pString )
          {
            bFound = TRUE;
            break;
          }
        }

        break;
      }

      spool[loop] = bFound ? '$' : '^';
    }
    else
    {
      spool[loop] = buf[loop];
    }
  }

  spool[loop] = '\x0';
  RETURN( spool );
}

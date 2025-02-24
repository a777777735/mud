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

extern  int     spell_damage    args( ( CHAR_DATA *, int, int, int ) );

SKILL( cast_restore_staff )
{
  OBJ_DATA * pObj = ( OBJ_DATA * ) vo;

  PUSH_FUNCTION( "cast_restore_staff" );

  if ( !ch || !verify_char( ch ) ) RETURN( FALSE );

  if ( pObj->item_type == ITEM_STAFF
    || pObj->item_type == ITEM_WAND )
  {
    if ( pObj->value[6] >= pObj->value[7] * 20 )
    {
      act( "$p���k�O�w�g�麡�F�M�L�k�A�[�j�F�T", ch, pObj, NULL, TO_CHAR );
      RETURN( FALSE );
    }

    pObj->value[6] = UMIN( pObj->value[7] * 20, pObj->value[6] + level * 10 );

    act( "$6$p�o�X�F�@�}�X�M���ť��M�A�P��k�O�w�g�[�j�F�T$0"
      , ch, pObj, NULL, TO_ALL );
  }
  else
  {
    act( "�A�L�k�N$p�餩�k�O�M�o���O�k���T", ch, pObj, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  RETURN( FALSE );
}

SKILL( cast_fire_elf )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         dam;
  int         kk;
  int         ran;

  PUSH_FUNCTION( "cast_fire_elf" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "cast_fire_elf:�ӷ������T." );
    RETURN( FALSE );
  }

  /* �e�m�T�� */
  act( "$1�Ӧۦa�����������F�ڡTť�ڰ@�۪���D�a�T$0", ch, NULL, victim, TO_ALL );

  dam = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_FIV );
  dam = number_range( dam * 3 / 4, dam * 5 / 4 );
  dam = URANGE( 0, dam, LEVEL_FIV_DAM );

  if ( number_percent() > ( ran = number_fuzzy( level ) ) )
  {
    sact( ch, victim, NULL,
      "$n�e�誺�l��}����M���Y��M$n�D��ϼ��T" );
    damage( ch, ch, UMAX( 1, dam / 10 ), sn, ATTACK_RANDOM );
    RETURN( TRUE );
  }

  sact( ch, victim, NULL,
    "$R������e�������F�Ѧa��«�X�M�̷Ӧ媺�����Ө�o�ӥ@�ɡT\n\r"
    "�������F���@���Ƶ������M���ۤ�����e�V��$N�ĥh�T\n\r"
    "���p�P���R��M$N���췥�Y�������ˡT" );
  damage( ch, victim, dam, sn, ATTACK_RANDOM );

  if ( level > 65 && can_damage( victim ) )
  {
    sact( ch, victim, NULL,
      "$A$R$n�ҵo�X���������F���l���������N�`��$N�T$Z" );

    for ( kk = 1; kk <= number_fuzzy( level / 20 ); kk++ )
    {
      if ( !can_damage( victim )
        || ( ch->in_room
          && victim->in_room
          && victim->in_room != ch->in_room ) ) break;

      damage( ch, victim, dam / 4 , sn , ATTACK_RANDOM );
    }
  }

  RETURN( TRUE );
}

SKILL( cast_bystander_fist )
{
  ROOM_INDEX_DATA * to_room;
  ROOM_INDEX_DATA * origin;
  CHAR_DATA       * people;
  CHAR_DATA       * victim = NULL;
  char            * pString = ( char * ) vo;
  int               door;
  int               dam;

  PUSH_FUNCTION( "cast_bystander_fist" );

  if ( !ch || !pString || !*pString || !ch->in_room )
  {
    mudlog( LOG_DEBUG, "cast_bystander_fist: �ӷ����~." );
    RETURN( FALSE );
  }

  if ( ch->position != POS_STANDING )
  {
    send_to_char( "�A�o��U���䪺�u�@�~��I�i�o�ۡT\n\r", ch );
    RETURN( FALSE );
  }
       if ( !str_prefix( pString, "north" ) ) door = DIR_NORTH;
  else if ( !str_prefix( pString, "east"  ) ) door = DIR_EAST;
  else if ( !str_prefix( pString, "south" ) ) door = DIR_SOUTH;
  else if ( !str_prefix( pString, "west"  ) ) door = DIR_WEST;
  else if ( !str_prefix( pString, "up"    ) ) door = DIR_UP;
  else if ( !str_prefix( pString, "down"  ) ) door = DIR_DOWN;
  else if ( !str_prefix( pString, "enter" ) ) door = DIR_ENTER;
  else if ( !str_prefix( pString, "out"   ) ) door = DIR_OUT;
  else
  {
    send_to_char( "�A����V����C\n\r", ch );
    RETURN( FALSE );
  }

  if ( !ch->in_room->exit[door]
    || !( to_room = ch->in_room->exit[door]->to_room ) )
  {
    act( "�A��$t��ڥ��S���X�f�T", ch, direction_name( door ), NULL, TO_CHAR );
    RETURN( FALSE );
  }

  for ( people = to_room->people; people; people = people->next_in_room )
    if ( is_same_group( ch, people ) && ( victim = people->fighting ) ) break;

  if ( !people || !victim || !can_damage( victim ) )
  {
    act( "�A��$t��ڥ��S���A�����ͥ��b�������T"
      , ch, direction_name( door ), NULL, TO_CHAR );

    RETURN( FALSE );
  }

  if ( !IS_NPC( victim ) )
  {
    send_to_char( "�A����γo�ۥ����a�T\n\r", ch );
    RETURN( FALSE );
  }

  if ( victim->hit <= 1 )
  {
    send_to_char( "�A����γo�۵����L���ͩR�T\n\r", ch );
    RETURN( FALSE );
  }

  dam = number_range( level, 2 * level );
  if ( victim->hit - dam <= 1 ) dam = victim->hit - 1;

  origin = ch->in_room;
  char_from_room( ch );
  char_to_room( ch, to_room );

  if ( dam <= 0 )
  {
    sact( ch, victim, NULL
      , "$n�q�j���I�i�X�j�s�����M$N�ˬO�S����M�u����a�ǡT" );
  }

  else
  {
    sact( ch, victim, NULL
      , "$n�q�Y�����W�a��v�i�X�j�s�����M�o�@�ۥ�������M"
        "�ݨ�$N�ثe�O���ˤ����T" );

    damage_situs( victim, dam, ATTACK_RANDOM );
  }

  char_from_room( ch );
  char_to_room( ch, origin );

  RETURN( TRUE );
}

SKILL( cast_power_word )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         dam;
  int         tt;

  PUSH_FUNCTION( "cast_power_word" );

  if ( !ch || !verify_char( ch ) )
  {
    mudlog( LOG_DEBUG, "cast_power_word: �ӷ����~." );
    RETURN( FALSE );
  }

  /* �e�m�T�� */
  sact( ch, victim, NULL,
    "$W�ѤW���U�@���Զ��M�]�е�$n�M�������M�A�ݤ��M$e�����v�F�T$A"
    "$W�A�uť�����䤣�����ǨӤ@�Ǳ뭵�M�������V�ӶV�M���T$Z$A" );

  /* �]�w��� */
  dam = spell_damage( ch, level, TYPE_WIS, SPELL_CLASS_FIV );
  dam = number_range( dam / 2, dam * 3 / 4 );
  dam = URANGE( 0, dam, LEVEL_FIV_DAM );

  if ( ch->alignment < ( AlignRange / 2 ) || !IS_NPC( victim ) )
  {
    sact( ch, victim, NULL,
      "$W$n�����a��w�e���M�u��$N�˦b�a�W���u�M���G�����F�o���n���T$Z" );
    damage( ch, victim, dam, sn, ATTACK_RANDOM );
    RETURN( TRUE );
  }

  else if ( ch->alignment < ( AlignRange * 3 / 4 ) )
  {
    for ( tt = 0; tt <= ch->skill[sn] / 20; tt++ )
    {
      if ( !verify_char( victim )
        || !can_damage( victim )
        || victim->in_room != ch->in_room ) break;

      sact( ch, victim, NULL,
        "$W�@�q�q���u�������a�H���Ϊ���r���b$N�����W�K$Z" );
      dam = dam + level;
      damage( ch, victim, dam / 3, sn, ATTACK_RANDOM );
    }

    RETURN( TRUE );
  }

  else
  {
    sact( ch, victim, NULL,
      "$W���誺�ѪťX�{�F�@���ն��M�@�Ө��۪k�窺���Ĩ����䤤�C$A$Z" );

    for ( tt = 0; tt <= ch->skill[sn] / 15 ; tt++ )
    {
      if ( !verify_char( victim )
        || victim->in_room != ch->in_room ) break;

      sact( ch, victim, NULL,
        "$W���Ĥ�@���M�o�X�@�D���~�b��$N�M$E�h�W���@�T$Z" );
      dam = dam + level;
      damage( ch, victim, dam / 2 , sn, ATTACK_RANDOM );
    }

    sact( ch, victim, NULL,
      "$W�����@�{�M���Ĥw�g��J�j�𤤡M�����ܼv�F�T$A" );
    RETURN( TRUE );
  }

  RETURN( FALSE );
}

SKILL( cast_haste )
{
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA   af;

  PUSH_FUNCTION( "cast_haste" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_haste: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( is_affected( victim, sn ) )
  {
    act( "$N�w�g�D�`�������F�T", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  af.type      = sn;
  af.duration  = number_range( level / 6, level / 4 );
  af.location  = APPLY_DEX;
  af.modifier  = 1 + ( level >= 18 ) + ( level >= 25 );
  af.level     = level;
  affect_to_char( victim, &af );

  send_to_char( "�@�}�q�y�Ǩ�A���}�W�M�A�P��t���ܧ֤F�C\n\r"
    , victim );
  if ( ch != victim ) send_to_char( "�n�F�M�I�k�����N�����C\n\r", ch );
  RETURN( TRUE );
}

SKILL( cast_super_refresh )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         refresh;

  PUSH_FUNCTION( "cast_super_refresh" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_super_refresh: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( victim->move >= get_curr_move( victim ) )
  {
    act( "$N�w�g�������|�h�¤F�T", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  refresh = level * 5 + ( number_range( 1, get_curr_int( ch ) * 4 ) );
  victim->move = UMIN( victim->move + refresh, get_curr_move( victim ) );

  send_to_char( "�A�P��D�`�����h�¤F�C\n\r", victim );
  if ( ch != victim ) send_to_char( "�n�F�M��O��_�����C\n\r", ch );
  RETURN( TRUE );
}

SKILL( cast_backstab )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  OBJ_DATA  * pObj;
  int         chance;
  int         dam;

  PUSH_FUNCTION( "cast_backstab" );

  if ( victim->fighting )
  {
    act( "$N���b���O�T�A�O�S����k¶�D$E�I�᪺�T", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  if ( victim->hit <= get_curr_hit( victim ) / 3 )
  {
    act( "$N�l��ܦh�M�����Fĵı�ʡM�A�S����k¶��$E�I��T"
      , ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  if ( !( pObj = get_eq_char( ch, WEAR_WIELD ) ) )
  {
    send_to_char( "�A���Z�����G���ӹ�T\n\r", ch );
    RETURN( FALSE );
  }

  if ( !can_see( ch, victim ) )
    chance = 1;
  else
  {
    chance = ( level + get_curr_dex( ch ) ) / 2;

    if ( is_affected( ch, SLOT_SNEAK ) ) chance += chance / 2;
    if ( !can_see( victim, ch ) )        chance += chance / 2;

    chance = UMAX( 1, UMIN( 100, chance ) );
  }

  /* �T�� */
  sact( ch, victim, pObj, "���������@�{�M$n�⤤$O��V$N��h�T\n\r" );

  if ( number_percent() >= chance )
  {
    dam = get_curr_str( ch );
    sact( ch, victim, NULL, "�������p��M�o�Q$N�o�{�F�T" );
  }
  else
  {
    dam = spell_damage( ch, level, TYPE_DEX, SPELL_CLASS_MAG );
    dam = number_range( dam * 3 / 4, dam * 5 / 4 );
    dam = URANGE( 0, dam, MAG_ATTACK_DAM );
    sact( ch, victim, pObj, "�������M$O�w�g��J$N������F�M$n��ŧ���\\�T" );
  }

  damage( ch, victim, dam, sn, ATTACK_RANDOM );

  RETURN( TRUE );
}

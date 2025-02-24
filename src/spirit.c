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
#include "merc.h"

/*
���ɼƭȪ�ܤ�k

V0  �p�����X
V1  �p������
V2  �p���g��
V3  �ޯู�X
V4  �ޯ൥��
V5  �p����W
*/

#define SPIRIT_VNUM             0
#define SPIRIT_LEVEL            1
#define SPIRIT_EXP              2
#define SPIRIT_SKILL_VNUM       3
#define SPIRIT_SKILL_EXP        4
#define SPIRIT_RENAME           5

void            smash_spirit    args( ( CHAR_DATA * ) );
OBJ_DATA *      spirit_to_jade  args( ( CHAR_DATA * ) );

FUNCTION( do_call )
{
  char         arg[MAX_INPUT_LENGTH];
  char         buf[MAX_STRING_LENGTH];
  OBJ_DATA   * obj;
  CHAR_DATA  * spirit;
  SKILL_DATA * pSkill;
  int          loop;
  int          adept;
  int          sn;
  int          pFind;
  bool         Touch;

  PUSH_FUNCTION( "do_call" );

  if ( IS_NPC( ch ) || !ch->in_room ) RETURN_NULL();

  argument = one_argument( argument, arg );

  if ( !arg[0] )
  {
    if ( !( spirit = ch->spirit ) )
    {
      send_to_char( "�A����S�����󪺤p����T\n\r", ch );
      RETURN_NULL();
    }

    if ( !spirit->in_room || spirit->in_room != ch->in_room )
    {
      act( "�A��$N��A���h�p���F�T", ch, NULL, spirit, TO_CHAR );
      RETURN_NULL();
    }

    clear_buffer();
    send_to_buffer(
      "�p���W�١R%s\n\r"
      "�p�����šR%d\n\r"
      "�p���ͩR�R%-6d �R %-6d\n\r"
      "�p���k�O�R%-6d �R %-6d\n\r"
      "�p����O�R%-6d �R %-6d\n\r"
      "�g �� �ȡR%d\n\r"
      "�ɯŸg��R%d\n\r"
      , mob_name( ch, spirit )
      , spirit->level
      , spirit->hit,  get_curr_hit( spirit )
      , spirit->mana, get_curr_mana( spirit )
      , spirit->move, get_curr_move( spirit )
      , spirit->exp
      , kill_table[ URANGE( 1, spirit->level, MAX_LEVEL - 1 )].exp - spirit->exp );

    for ( loop = 0; loop < MAX_SKILL; loop++ )
    {
      if ( spirit->skill[loop] > 0 && ( pSkill = get_skill( loop ) ) )
      {
        send_to_buffer( "�ǲߧޯ�R%s\n\r�ޯ���m�R%s"
          , pSkill->cname
          , adeptation_name( spirit->skill[loop] ) );

        if ( SkillValue || IS_IMMORTAL( ch ) )
          send_to_buffer( "(%3d%%)", spirit->skill[loop] );

        send_to_buffer( "\n\r" );
      }
    }

    print_buffer( ch );

    RETURN_NULL();
  }

  if ( !str_prefix( arg, "!back" ) )
  {
    if ( ch->position != POS_STANDING )
    {
      send_to_char( "�A����U���䪺�u�@�a�T\n\r", ch );
      RETURN_NULL();
    }

    if ( !( spirit = ch->spirit ) )
    {
      send_to_char( "�A�ڥ��S���p���C�T\n\r", ch );
      RETURN_NULL();
    }

    if ( !spirit->in_room || spirit->in_room != ch->in_room )
    {
      act( "�A��$N���b�A������O�T", ch, NULL, spirit, TO_CHAR );
      RETURN_NULL();
    }

    if ( spirit->position != POS_STANDING )
    {
      act( "�A��$N�٥��b�����T", ch, NULL, spirit, TO_CHAR );
      RETURN_NULL();
    }

    if ( !spirit_to_jade( spirit ) )
    {
      act( "�D�F�M$N���M�����^�h�F�T", ch, NULL, spirit, TO_CHAR );
      RETURN_NULL();
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "!phone" ) )
  {
    if ( ch->spirit )
    {
      act( "�A�w�g��$N�F�T", ch, NULL, ch->spirit, TO_CHAR );
      RETURN_NULL();
    }

    for ( pFind = 0, spirit = char_list; spirit; spirit = spirit->next )
    {
      if ( !verify_char( spirit )
        || !IS_NPC( spirit )
        || !spirit->pIndexData
        || spirit->jade.high != ch->serial.high
        || spirit->jade.low  != ch->serial.low
        || spirit->boss
        || spirit->position != POS_STANDING
        || spirit->mount_by
        || is_affected( spirit, SLOT_CHARM_PERSON )
        || !spirit->in_room
        || spirit->in_room != ch->in_room ) continue;

      pFind = 1;
      free_string( spirit->byname );
      sprintf( buf, "�ݩ�%s���p��(%s)", mob_name( NULL, ch ), spirit->name );
      spirit->byname = str_dup( buf );

      free_string( spirit->description );
      sprintf( buf, "�o�O��%s�l��X�Ӫ�%s�M�ݨӮ������U���T\n\r"
        , mob_name( NULL, ch )
        , mob_index_name( NULL, spirit->pIndexData ) );

      spirit->description = str_dup( buf );

      ch->spirit   = spirit;
      spirit->boss = ch;

      act( "$n�I��$N�����F�`�B�M����$N�N�ѥ������^�ӤF�T"
        , ch, NULL, spirit, TO_ALL );

      RETURN_NULL();
    }

    if ( pFind == 0 )
    {
      send_to_char( "�A�L�k�I���A���p�������F�`�B�T\n\r", ch );
      RETURN_NULL();
    }
  }

  else if ( !str_prefix( arg, "!search" ) )
  {
    if ( ch->spirit )
    {
      act( "�A��$N�èS���g���T", ch, NULL, ch->spirit, TO_CHAR );
      RETURN_NULL();
    }

    send_to_char( "�A�Ѥߤ��o�X�I�ۡM�Ʊ�g�����p������^�a�����T\n\r", ch );

    for ( pFind = 0, spirit = char_list; spirit; spirit = spirit->next )
    {
      if ( !verify_char( spirit )
        || !IS_NPC( spirit )
        || spirit->jade.high != ch->serial.high
        || spirit->jade.low  != ch->serial.low
        || !spirit->in_room
        || spirit->boss ) continue;

      act( "�A�o�{�A���p�����b$t��$r�C���M�֥h�䥦�a�T"
        , ch
        , spirit->in_room->area->name
        , NULL
        , TO_CHAR );

      pFind++;
    }

    if ( pFind == 0 ) send_to_char( "�A�èS���p���b�o�@�ɡT\n\r", ch );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "!skill" ) )
  {
    if ( ch->position != POS_STANDING )
    {
      send_to_char( "�A����U���䪺�u�@�a�T\n\r", ch );
      RETURN_NULL();
    }

    if ( !( spirit = ch->spirit ) )
    {
      send_to_char( "�A�ڥ��S���p���C�M�A�����ޯ൹�L�O�S\n\r", ch );
      RETURN_NULL();
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\x0' )
    {
      act( "�A�n�餰��ޯ൹�A��$N�O�S", ch, NULL, spirit, TO_CHAR );
      RETURN_NULL();
    }

    if ( !( pSkill = skill_isname( arg ) ) )
    {
      act( "�藍�_�M�S�����اޯ� $2$T$0 �i�H�鵹�L�T", ch, NULL, arg, TO_CHAR );
      RETURN_NULL();
    }

    if ( !pSkill->enable || pSkill->slot == SLOT_FLEE )
    {
      act( "�藍�_�M$w�L�k�P��M�ҥH���n�е�$N�T", ch, pSkill, spirit, TO_CHAR );

      RETURN_NULL();
    }

    if ( ch->skill[( sn = pSkill->slot )] <= 30 )
    {
      act( "$w�A�]�S���ܼ��M���е�$N�O�S", ch, pSkill, spirit, TO_CHAR );
      RETURN_NULL();
    }

    /* �����ޯ� */
    for ( loop = 0; loop < MAX_SKILL; loop++ )
      if ( loop != SLOT_FLEE ) spirit->skill[loop] = 0;

    /* �����P�� */
    if ( spirit->enable ) extract_enable( spirit );

    spirit->skill[sn]  = 10;
    ch->skill[sn]     -= 10;

    set_enable( spirit, pSkill );

    act( "$n�w�g�N����$w���\\�O�鵹$N�T", ch, pSkill, spirit, TO_ALL );

    if ( IS_IMMORTAL( ch ) )
      mudlog( LOG_WIZARD, "%s ���p���ޯ� %s.", ch->name, pSkill->name );

    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "!fill" ) )
  {
    if ( !( spirit = ch->spirit ) )
    {
      send_to_char( "�A�ڥ��S���p���C�M�A���}�i�L�O�S\n\r", ch );
      RETURN_NULL();
    }

    Touch = FALSE;

    /* �`�J�ͩR�O */
    if ( ( adept = UMIN( spirit->max_hit - spirit->hit, ch->hit - 1 ) ) > 0 )
    {
      act( "�A��$i�I���ͩR�O��J$N���C", ch, &adept, spirit, TO_CHAR );

      spirit->hit += adept;
      ch->hit     -= adept;
      Touch        = TRUE;
    }

    /* �`�J�k�O */
    if ( ( adept = UMIN( spirit->max_mana - spirit->mana, ch->mana - 1 ) ) > 0 )
    {
      act( "�A��$i�I���k�O��J$N���C", ch, &adept, spirit, TO_CHAR );

      spirit->mana += adept;
      ch->mana     -= adept;
      Touch         = TRUE;
    }

    /* �`�J��O */
    if ( ( adept = UMIN( spirit->max_move - spirit->move, ch->move - 1 ) ) > 0 )
    {
      act( "�A��$i�I����O��J$N���C", ch, &adept, spirit, TO_CHAR );

      spirit->move += adept;
      ch->move     -= adept;
      Touch         = TRUE;
    }

    if ( !Touch ) act( "�A����]�S�������A��$N�T", ch, NULL, spirit, TO_CHAR );
    RETURN_NULL();
  }

  else
  {
    if ( ch->position != POS_STANDING )
    {
      send_to_char( "�A����U���䪺�u�@�a�T\n\r", ch );
      RETURN_NULL();
    }

    if ( room_is_private( ch->in_room ) || ch->in_room->NoMob )
    {
      act( "$r����ӭ��F�M�p���O�����X�Ӫ��T", ch, NULL, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( is_platform( ch->in_room ) || ch->in_room->Sail )
    {
      act( "�藍�_�M�A����b$r�l��X�A���p���T", ch, NULL, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( !( obj = get_obj_carry( ch, arg ) ) )
    {
      act( "�A���W�ڥ��S���o������ $2$T$0 �C�T", ch, NULL, arg, TO_CHAR );
      RETURN_NULL();
    }

    if ( obj->item_type != ITEM_SPIRITJADE )
    {
      act( "$p�ڥ����O���ɡM�A�O���O�Q�F�F�S", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch->spirit )
    {
      send_to_char( "�A�S����k�l��X��h���p���X�ӤF�T\n\r", ch );
      RETURN_NULL();
    }

    if ( obj->value[SPIRIT_LEVEL] > ch->level )
    {
      send_to_char( "�ݨӧA�O�S����k����o�Ӥp���M�L���k�O�ӱj�F�T\n\r", ch );
      RETURN_NULL();
    }

    for ( spirit = char_list; spirit; spirit = spirit->next )
    {
      if ( verify_char( spirit )
        && IS_NPC( spirit )
        && spirit->jade.high == ch->serial.high
        && spirit->jade.low  == ch->serial.low )
      {
        spirit->jade.high  = 0;
        spirit->jade.low   = 0;
        spirit->spiritjade = 0;
      }
    }

    if ( !( spirit = jade_to_spirit( ch, obj ) ) )
    {
      act( "�A�o��$p�ݨӮڥ��S����k�l��X�p���M��F��F�T"
        , ch, obj, NULL, TO_CHAR );

      RETURN_NULL();
    }
  }

  RETURN_NULL();
}

OBJ_DATA * spirit_to_jade(  CHAR_DATA * spirit )
{
  OBJ_INDEX_DATA * pIndexData;
  OBJ_DATA       * obj;
  CHAR_DATA      * boss;
  int              sn;
  int              loop;

  PUSH_FUNCTION( "spirit_to_jade" );

  if ( !spirit
    || !spirit->pIndexData
    || !( boss = spirit->boss )
    || !boss->in_room )
  {
    mudlog( LOG_DEBUG, "spirit_to_jade: �ӷ����~." );
    RETURN( NULL );
  }

  if ( ( sn = spirit->spiritjade ) <= 0 )
  {
    mudlog( LOG_DEBUG, "spirit_to_jade: �ӷ��L�k�ܬ�����." );
    RETURN( NULL );
  }

  if ( !( pIndexData = get_obj_index( sn ) )
    || pIndexData->item_type != ITEM_SPIRITJADE )
  {
    mudlog( LOG_DEBUG, "spirit_to_jade: ���ɸ��X %d ���~.", sn );
    RETURN( NULL );
  }

  if ( boss->in_room != spirit->in_room )
  {
    mudlog( LOG_DEBUG, "spirit_to_jade: ��̦a�褣�P." );
    RETURN( NULL );
  }

  /* �s�y�X���� */
  obj = create_object( pIndexData, UMIN( LEVEL_IMMORTAL -1, spirit->level ) );

  /* �M���ƭ� */
  for ( loop = 0; loop < MAX_OBJECT_VALUE; loop++ ) obj->value[loop] = 0;

  /* ��^�ƭ� */
  obj->value[SPIRIT_VNUM]  = spirit->pIndexData->vnum;
  obj->value[SPIRIT_LEVEL] = UMAX( 1, UMIN( LEVEL_IMMORTAL -1, spirit->level ) );
  obj->value[SPIRIT_EXP]   = UMAX( 0, spirit->exp );

  /* ��^�ޯ�ƭ�, �p���u�|�@�اޯ� */
  for ( loop = 0; loop < MAX_SKILL; loop++ )
  {
    if ( spirit->skill[loop] > 0 && loop != SLOT_FLEE )
    {
      obj->value[SPIRIT_SKILL_VNUM] = loop;
      obj->value[SPIRIT_SKILL_EXP]  = UMAX( 1, UMIN( 100, spirit->skill[loop] ) );
      break;
    }
  }

  /* �ݬݬO�_�ள, �άO��������� */
  if ( obj->Takeable
    && boss->carry_number + 1 <= can_carry_n( boss )
    && get_carry_weight( boss ) + get_obj_weight( obj ) <= can_carry_w( boss ) )
  {
    obj_to_char( obj, boss );

    act( "��M�M�u���A��$N�_�X�@�}�C�ϡM���A�^���ɡM�A���W�N�h�F$p�T�o�S"
      , boss, obj, spirit, TO_CHAR );

    act( "��M�M$n��$N�_�X�@�}�C�ϡM���A�^���ɡM���٦�$N�����v�T�o�S"
      , boss, NULL, spirit, TO_ROOM );
  }

  else
  {
    obj_to_room( obj, boss->in_room );

    act( "��M�M�u���A��$N�_�X�@�}�C�ϡM���A�^���ɡM�a�W�N�h�X$p�T�o�S"
      , boss, obj, spirit, TO_CHAR );

    act( "��M�M$n��$N�_�X�@�}�C�ϡM���A�^���ɡM�a�W�N�h�X$p�T�o�S"
      , boss, obj, spirit, TO_ROOM );
  }

  /* �������F */
  spirit_from_char( spirit );
  extract_char( spirit, TRUE );

  RETURN( obj );
}

CHAR_DATA * jade_to_spirit( CHAR_DATA * ch , OBJ_DATA * obj )
{
  CHAR_DATA      * spirit;
  SKILL_DATA     * pSkill;
  MOB_INDEX_DATA * pMobIndex;
  char             buf[MAX_STRING_LENGTH];
  int              loop;
  int              level;

  PUSH_FUNCTION( "jade_to_spirit" );

  if ( !ch || !obj )
  {
    mudlog( LOG_DEBUG, "jade_to_spirit: �ӷ��ʥF." );
    RETURN( NULL );
  }

  if ( IS_NPC( ch ) || obj->item_type != ITEM_SPIRITJADE || !ch->in_room )
  {
    mudlog( LOG_DEBUG, "jade_to_spirit: �ӷ������T." );
    RETURN( NULL );
  }

  if ( ch->serial.high == 0 || ch->serial.low == 0 )
  {
    mudlog( LOG_DEBUG, "jade_to_spirit: �D�H�ʥF�Ǹ�." );
    RETURN( NULL );
  }

  if ( !( pMobIndex = get_mob_index( obj->value[SPIRIT_VNUM] ) ) )
  {
    mudlog( LOG_DEBUG, "jade_to_spirit: ���~%d�p�����X���~."
      , obj->pIndexData->vnum );
    RETURN( NULL );
  }

  if ( ( level = obj->value[SPIRIT_LEVEL] ) <= 0
    || level >= LEVEL_IMMORTAL
    || obj->value[SPIRIT_EXP]        < 0
    || obj->value[SPIRIT_SKILL_VNUM] < 0
    || obj->value[SPIRIT_SKILL_VNUM] >= MAX_SKILL
    || obj->value[SPIRIT_SKILL_EXP]  < 0
    || obj->value[SPIRIT_SKILL_EXP]  > 100
    || obj->value[SPIRIT_RENAME]     < 0 )
    RETURN( NULL );

  /* �t�m�O���鵹�L */
  clear_char( spirit  = alloc_struct( STRUCT_CHAR_DATA ) );

  spirit->pIndexData  = pMobIndex;

  spirit->name        = str_dup( pMobIndex->name       );
  spirit->long_descr  = str_dup( pMobIndex->long_descr );

  sprintf( buf, "�ݩ�%s���p��", mob_name( NULL, ch ) );
  spirit->byname = str_dup( buf );

  sprintf( buf, "�o�O��%s�l��X�Ӫ�%s�M�ݨӮ������U���T\n\r"
    , mob_name( NULL, ch )
    , mob_index_name( NULL, pMobIndex ) );

  spirit->description = str_dup( buf );

  spirit->prompt      = str_dup( "<^hp1 ^mp1 ^mv1>" );
  spirit->email       = str_dup( "<none>" );
  spirit->spec_fun    = NULL;

  spirit->level       = level;
  spirit->act         = ACT_IS_NPC | ACT_SENTINEL | ACT_STAY_AREA;
  spirit->alignment   = ch->alignment;
  spirit->sex         = pMobIndex->sex;
  spirit->class       = pMobIndex->class;
  spirit->speak       = pMobIndex->speak;

  spirit->now_str     = level * LOAD_ATTRIB_PLUS;
  spirit->now_int     = level * LOAD_ATTRIB_PLUS;
  spirit->now_wis     = level * LOAD_ATTRIB_PLUS;
  spirit->now_dex     = level * LOAD_ATTRIB_PLUS;
  spirit->now_con     = level * LOAD_ATTRIB_PLUS;

  spirit->armor       = level * LOAD_ARMOR_PLUS;
  spirit->gold        = 0;
  spirit->hitroll     = 0;
  spirit->damroll     = level * LOAD_DAMROLL_PLUS;

  spirit->max_hit     = level * load_hit_plus;
  spirit->mod_hit     = 0;
  spirit->hit         = UMAX( 0, UMIN( spirit->max_hit, ch->hit - 1 ) );
  ch->hit            -= spirit->hit;

  spirit->max_mana    = level * load_mana_plus;
  spirit->mod_mana    = 0;
  spirit->mana        = UMAX( 0, UMIN( spirit->max_mana, ch->mana - 1 ) );
  ch->mana           -= spirit->mana;

  spirit->max_move    = level * load_move_plus;
  spirit->mod_move    = 0;
  spirit->move        = UMAX( 0, UMIN( spirit->max_move, ch->move - 1 ) );
  ch->move           -= spirit->move;

  spirit->enable      = NULL;
  spirit->tractable   = 0;
  spirit->spiritjade  = obj->pIndexData->vnum;

  /* �]�w���m�� */
  for ( loop = 0; loop < MAX_SKILL; loop++ ) spirit->skill[loop] = 0;

  spirit->skill[obj->value[SPIRIT_SKILL_VNUM]] = obj->value[SPIRIT_SKILL_EXP];
  spirit->skill[SLOT_FLEE] = UMAX( 1, spirit->level / 5 );

  /* �]�w�P��, �ӥB�p���u�|�@�اޯ� */
  if ( ( pSkill = get_skill( obj->value[SPIRIT_SKILL_VNUM] ) )
    && pSkill->enable )
  {
    set_enable( spirit, pSkill );
  }

  set_enable( spirit, get_skill( SLOT_FLEE ) );

  spirit->jade.high = ch->serial.high;
  spirit->jade.low  = ch->serial.low;

  /* �]�w�L���g��� */
  spirit->exp = obj->value[SPIRIT_EXP];
  pMobIndex->count++;

  /* �ץ��s�� */
  ch->spirit   = spirit;
  spirit->boss = ch;

  char_to_room( spirit, ch->in_room );

  /* �e�X�T�����ж����Ҧ����H */
  act( "$n�q$p�l��X$N�M�o�쩳���@�^�ƩO�S", ch, obj, spirit, TO_ALL );

  /* �e�X�T���M�åB�⪫�~�P�� */
  do_say( spirit, "�D�H�M�^�媺�}���M�ڨӨ�U�A�F�T" );
  act( "�A���W��$p�b�P�ɤ]���������T", ch, obj, NULL, TO_CHAR );
  extract_obj( obj );

  RETURN( spirit );
}

void spirit_from_char( CHAR_DATA * spirit )
{
  CHAR_DATA      * boss;
  MOB_INDEX_DATA * pIndex;

  PUSH_FUNCTION( "spirit_from_char" );

  if ( !spirit
    || !( boss = spirit->boss )
    || !( pIndex = spirit->pIndexData ) )
  {
    mudlog( LOG_DEBUG, "spirit_from_char: �ӷ����~." );
    RETURN_NULL();
  }

  spirit->boss = NULL;
  boss->spirit = NULL;

  /* �ץ��W��, �B�A�]�l�ꤣ�^�h�F */
  free_string( spirit->byname );
  spirit->byname = str_dup( spirit->pIndexData->short_descr );

  free_string( spirit->description );
  spirit->description = str_dup( spirit->pIndexData->description );

  RETURN_NULL();
}

void smash_spirit( CHAR_DATA * spirit )
{
  PUSH_FUNCTION( "smash_spirit" );

  if ( !spirit || !spirit->pIndexData )
  {
    mudlog( LOG_DEBUG, "smash_spirit: �ʥF�ӷ�." );
    RETURN_NULL();
  }

  spirit->boss       = NULL;
  spirit->spiritjade = 0;

  /* �ץ��W��, �B�A�]�l�ꤣ�^�h�F */
  free_string( spirit->byname );
  spirit->byname = str_dup( spirit->pIndexData->short_descr );

  free_string( spirit->description );
  spirit->description = str_dup( spirit->pIndexData->description );

  RETURN_NULL();
}

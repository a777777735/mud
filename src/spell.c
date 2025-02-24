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

#define MIN_USE_TIME 5
#define MAX_USE_TIME 20

extern  int     spell_damage    args( ( CHAR_DATA *, int, int, int ) );

void    scan                    args( ( CHAR_DATA *, int ) );
int     spell_damage            args( ( CHAR_DATA *, int, int, int ) );
bool    can_steal               args( ( CHAR_DATA *, CHAR_DATA * ) );

int spell_damage( CHAR_DATA * ch, int level, int suit, int class )
{
  int dam;
  int type_number;

  PUSH_FUNCTION( "spell_damage" );

  if ( !ch || !verify_char( ch ) ) RETURN( 0 );

  switch( suit )
  {
  default:
    mudlog( LOG_DEBUG, "spell_damage: ���~���A: %d.", suit );
    RETURN( 0 );

  case TYPE_STR:
    type_number = get_curr_str( ch );
    break;

  case TYPE_INT:
    type_number = get_curr_int( ch );
    break;

  case TYPE_WIS:
    type_number = get_curr_wis( ch );
    break;

  case TYPE_DEX:
    type_number = get_curr_dex( ch );
    break;

  case TYPE_CON:
    type_number = get_curr_con( ch );
    break;

  case TYPE_MOB:
    type_number = ch->level;
    break;
  }

  switch( class )
  {
  default:
    mudlog( LOG_DEBUG, "spell_damage: ���~�h��: %d.", class );
    RETURN( 0 );

  case SPELL_CLASS_ONE:
    dam = 40 + ( level + ch->level + type_number ) * 3;
    dam = number_range( dam * 2 / 3, dam * 4 / 3 );
    dam = UMAX( 0, UMIN( dam, LEVEL_ONE_DAM ) );
    break;

  case SPELL_CLASS_TWO:
    dam = 200 + ( level + ch->level + type_number ) * 6;
    dam = number_range( dam * 3 / 4, dam * 5 / 4 );
    dam = UMAX( 0, UMIN( dam, LEVEL_TWO_DAM ) );
    break;

  case SPELL_CLASS_THR:
    dam = 350 + ( level + ch->level + type_number ) * 9;
    dam = number_range( dam * 3 / 4, dam * 5 / 4 );
    dam = UMAX( 0, UMIN( dam, LEVEL_THR_DAM ) );
    break;

  case SPELL_CLASS_FOU:
    dam = 500 + ( level + ch->level + type_number ) * 12;
    dam = number_range( dam * 3 / 4, dam * 5 / 4 );
    dam = UMAX( 0, UMIN( dam, LEVEL_FOU_DAM ) );
    break;

  case SPELL_CLASS_FIV:
    dam = 750 + ( level + ch->level + type_number ) * 15;
    dam = number_range( dam * 3 / 4, dam * 5 / 4 );
    dam = UMAX( 0, UMIN( dam, LEVEL_FIV_DAM ) );
    break;

  case SPELL_CLASS_MOB:
    dam = 1000 + ( level + ch->level + type_number ) * 18;
    dam = number_range( dam, dam * 5 / 4 );
    dam = UMAX( 0, UMIN( dam, MOB_ATTACK_DAM ) );
    break;

  case SPELL_CLASS_MAG:
    dam = 800 + ( level + ch->level + type_number ) * 16;
    dam = number_range( dam, dam * 7 / 5 );
    dam = UMAX( 0, UMIN( dam, MAG_ATTACK_DAM ) );
    break;
  }

  RETURN( dam );
}


SKILL( spell_make_spring )
{
  OBJ_DATA * spring;
  int        choice[] = { 33, 66, 100, -1 };

  PUSH_FUNCTION( "spell_make_water" );

  if ( !ch->in_room )
  {
    mudlog( LOG_DEBUG, "spell_make_spring: �ӷ����~." );
    RETURN( FALSE );
  }

  if ( !can_see_room( ch ) )
  {
    act( "$r�º������M�A���򳣬ݤ���C" , ch, NULL, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  if ( obj_type_room( ch, ITEM_FOUNTAIN ) > 0 )
  {
    act( "$r�����״I�M�w���ݭn�A���X���F�C" , ch, NULL, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  if ( is_loge( ch->in_room ) || ch->in_room->Sail )
  {
    act( "�A�O�S����k�b$r�y�X�������T", ch, NULL, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  if ( ch->in_room->nobject >= MaxRoomObject )
  {
    send_to_char( "�o�̪����~�Ӧh�F�M�Х��^�m���@�ǧa�T\n\r", ch );
    RETURN( FALSE );
  }

  spring = create_object( ObjSpring , 1 );
  spring->timer = number_range( MIN_USE_TIME, MAX_USE_TIME );
  obj_to_room( spring, ch->in_room );

  switch( dimemsion_position( choice, level, TRUE ) )
  {
  case ERRORCODE:
    mudlog( LOG_DEBUG, "�ޯ� %d ���T���]�w�����D." , sn );
    RETURN( FALSE );

  case 0:

    act( "$6$n�ϥX�F�����ѼơM�Ʊ�y�X�@�Ӥp����C$A"
      "�����M�b���m�}�ɪ����~Ţ�n�U�M�p���w�������X�M"
      "�Φ��F�@��$p�C$0", ch, spring, NULL, TO_ALL );
    break;

  case 1:

    act( "$6����$n�H��@���M�������a���������������_�ӡM"
      "�åB�g�a�U���M���W�N�Φ��F�@��$p�M��i�[���O�~�M�٦�"
      "�n�l�b�W����O�T$0", ch, spring, NULL, TO_ALL );

    break;

  case 2:

    act( "$6$n�|�P��M�}�l�_���MŢ�n�۾�Ӧa��M"
      "�����M�����ư��C�m���W�M�Ѱ��ŭ��ܦa���M"
      "�Φ��@�Ӹֱ��e�N��$p�M����·¶�M�[�[�����T$0"
      , ch, spring, NULL, TO_ALL );

    break;
  }

  RETURN( TRUE );
}

SKILL( spell_make_light )
{
  OBJ_DATA * light;
  int        choice[] = { 33, 66, 100, -1 };

  PUSH_FUNCTION( "spell_make_light" );

  if ( obj_type_char( ch, ITEM_LIGHT ) > 0 )
  {
    send_to_char( "�A���W�w�g���G�o�M���ݭn���F�T\n\r" , ch );
    RETURN( FALSE );
  }

  if ( obj_type_room( ch, ITEM_LIGHT ) > 0 )
  {
    act( "$r�w�g���G�o�M���ݭn���F�C" , ch, NULL, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  if ( ch->in_room->nobject >= MaxRoomObject )
  {
    send_to_char( "�o�̪����~�Ӧh�F�M�Х��^�m���@�ǧa�T\n\r", ch );
    RETURN( FALSE );
  }

  light = create_object( ObjLight, 1 );
  light->value[2] = number_range( MIN_USE_TIME / 2, MAX_USE_TIME / 2 );
  light->value[3] = number_range( MIN_USE_TIME * 2, MAX_USE_TIME * 2 );
  obj_to_char( light, ch );

  switch( dimemsion_position( choice, level, TRUE ) )
  {
  case ERRORCODE:
    mudlog( LOG_DEBUG, "�ޯ�%d���T���]�w�����D." , sn );
    RETURN( FALSE );

  case 0:

    act( "$6$n����@���M�u���L�ƪ��p���I�}�l�ּƪ���ʡM"
         "����¶��$n��@�餧��K�����_�ӡM�Φ��F$p�T$0"
         , ch , light , NULL , TO_ALL );

    break;

  case 1:

    act( "$7�಴�����M�@�ιΥѤp���I�Ҳզ�������Ѧa�W�ɰ_�M"
         "���۶V��V�֡M�N�b�֨�F�ݤ��M�����ɡM�~�o�{���̤w�g�E���@�_"
         "�Φ��F$p�T$0", ch, light, NULL, TO_ALL );

    break;

  case 2:

    act( "$7�@�ɶ��S�������ܤơM���O�����p�n���_�M"
         "�ᵹ�H���B�e�����R���աM��M�����M���Y��h"
         "�M�o�{�P�P�����V��$n�E���M�಴���w�g�Φ��F$p�T$0"
         , ch, light, NULL, TO_ALL );

    break;

  }

  RETURN( TRUE );
}

#undef MIN_USE_TIME
#undef MAX_USE_TIME

SKILL( spell_cast_armor )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA aff;
  int         choice[] = { 33, 66, 100, -1 };

  PUSH_FUNCTION( "spell_cast_armor" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "spell_cast_armor: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( is_affected( victim, sn ) )
  {
    act( "$N���O�@�ݨӤw�g�Q�������F�T", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  aff.type      = sn;
  aff.duration  = UMAX( 1, number_range( level / 10, level / 5 ) );
  aff.modifier  = -( UMAX( 1, number_range( level / 2, level ) ) );
  aff.location  = APPLY_AC;
  aff.level     = level;
  affect_to_char( victim, &aff );

  switch( dimemsion_position( choice, level, TRUE ) )
  {
  case ERRORCODE:
    mudlog( LOG_DEBUG, "�ޯ� %d ���T���]�w�����D." , sn );
    RETURN( FALSE );

  case 0:

    act( "$3$n$3����@�}�̰ʡM�A�P��Ů�@�}�����M����"
         "�Φ��@���@�޳�¶�b$N$3���e�M�O�@��$N�T$0"
         , ch, NULL, victim, TO_ALL );

    break;

  case 1:

    act( "$3$n$3�j�ۤ@�n�M���ۤj�a�_�ʡM�����ܦ�M�L�ƪ����F�ɯ�"
         "�X�{�b$N$3���e�Φ��@�@�ޡT$0"
         , ch, NULL, victim, TO_ALL );

    break;

  case 2:

    act( "$3�ѤW�@�}�����{�{�M���۵L�ƪ��ѧL�ѱN�p�����F�U�ӡM"
         " ��¶��$N$3�M�w�w���b�L����Φ��@�鹳�O�O�@���@�˪��F��M"
         "�Pı�n���Q����T�T$0" , ch, NULL, victim, TO_ALL );

    break;

  }

  RETURN( TRUE );
}

SKILL( spell_cast_bless )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         choice[] = { 33, 66, 100, -1 };
  AFFECT_DATA aff;

  PUSH_FUNCTION( "spell_cast_bless" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "spell_cast_bless: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( is_affected( victim, sn ) )
  {
    act( "$N�����֬ݨӤw�g�Q�������F�M�A���ݭn�A���֤F�C"
      , ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  aff.type      = sn;
  aff.duration  = UMAX( 1, number_range( level / 10, level / 5 ) );
  aff.modifier  = 0;
  aff.location  = APPLY_SAVING_SPELL;
  aff.level     = level;
  affect_to_char( victim, &aff );

  aff.location  = APPLY_HITROLL;
  aff.modifier  = UMAX( 1, number_range( level / 8, level / 7 ) );
  aff.level     = level;
  affect_to_char( victim, &aff );

  switch( dimemsion_position( choice, level, TRUE ) )
  {
  case ERRORCODE:
    mudlog( LOG_DEBUG, "�ޯ� %d ���T���]�w�����D.", sn );
    RETURN( FALSE );

  case 0:
    act( "$7$n$7����@���M�\\�h���p���F�q�Ť��_�F�U��"
         "�M��¶��$N$7�۵۸t�q�M���ֵ�$E�C$0", ch, NULL, victim, TO_ALL );
    break;

  case 1:

    act( "$7$n$7���n�u�۵ۯ��֤��|�M�N���������ְe�P$N$7�M�಴���u��$E"
         "�C�C���o�X�{�{���Ȧ���~�M�{�p���䪺�@������C$0"
         , ch, NULL, victim, TO_ALL );

    break;

  case 2:
    act( "$7$n$7�w�w�����R�ۤ�M���۳\\�\\�h�h�����䪺�������F�C�C���}�B�F"
         "�L�ӡM��¶��$N$7�åB�]���$E�M�w�w���Φ��F�@�����G���������֡M��"
         "�ۤѤW���Ѩϳ����Y�U�Ӫ���o�ѤW�H���_���C$0"
         , ch, NULL, victim, TO_ALL );
    break;
  }

  RETURN( TRUE );
}

SKILL( spell_cast_fire )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         dam;
  int         dam_plus;
  int         choice[] =
              { 25, 35, 45, 55, 65, 75, 80, 85, 90, 95, 98, 99, 100, -1 };

  PUSH_FUNCTION( "spell_cast_fire" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "spell_cast_fire: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  dam = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_FOU );
  dam = dam / 2;

  /* �e�m�T�� */
  act( "$1$n$1�B���ߤ�������b����M�಴���������p���T�T$A$0"
    , ch, NULL, victim, TO_ALL );

  switch( UMIN( 3, dam_plus = dimemsion_position( choice, level , TRUE ) ) )
  {
  case ERRORCODE:
    mudlog( LOG_DEBUG, "�ޯ� %d ���T���]�w�����D." , sn );
    RETURN( FALSE );

  case 0:
    sact( ch, victim, NULL,
      "$n�ΥX�����O�q�M���X�@�Ӯ��Y��j�����y�M�F�V$N�T" );
    dam_plus = 1;
    break;

  case 1:
    sact( ch, victim, NULL,
      "$n����s���M�o�X�F��Ӥp���y�M���۫�̰l�W�e�̡M�Φ��@�Ӥ��y�F�V$N�T" );
    break;

  case 2:
    sact( ch, victim, NULL,
      "$n����X�@�M���@�ΤO�M�o�X�F�@�Ӥڴx�j�����y�M��$N���h�T" );
    break;

  case 3:
    sact( ch, victim, NULL,
      "$n�N����ΤO����M���۸I���M�r�a�z�X�\\�h����M���ۤ@�ӭӤ���"
      "���O���ͩR�@��V��$N�l�h�T" );
    break;

  case 4:
    sact( ch, victim, NULL,
      "$n�b�a���|����M���ۤ@���|�ۤ��e�����F���ܫe�ӡT$A"
      "�����F�V��$N��H���h���@���M�N�N������$R���e$Z�r�V$E��h�T" );
    break;

  case 5:
    sact( ch, victim, NULL,
      "$n���|����M�����F�j�۵M�����q�M�ĦX�Φ��@�Ӱ�����"
      "$R���y$Z�M����$R���y$Z���O���ͩR������b$N�ӥh�T" );
    break;

  case 6:
    sact( ch, victim, NULL,
      "$n���F�U�ӡM�����믫�q��@��ť�������G�y�T$A"
      "���ۦb$N�����M�X�{�\\�h��$R���K$Z�M�N$E�]�S�T�T" );
    break;

  case 7:
    sact( ch, victim, NULL,
      "$R$n$R���R����M�o�X�F�@�s�갪�������y"
      "�M�@�ӭӲr�a�V$N�g���M�ϱo$E���@�I�ݤ��L��ӡT�T$Z" );
    break;

  case 8:
    sact( ch, victim, NULL,
      "$R$n�����믫�M�åB���Ф��_����w�@�ǩG��C"
      "$A$A���ۤ@�������F�X�{�b$n���ǡM�y�W�a�ۨǷL���L���T"
      "$A$A���ۺ��F�g�ۤ@�n�M�o�Ʀ����ĦV$N�T$Z" );
    break;

  case 9:
    sact( ch, victim, NULL,
      "$R$n�N����X�@�M�������ϫl�M�u��$e�y�W�����ର�����C"
      "$A$A���ۥuť$e�۹D�R�y$Y�i�c��$N�M�ǩR�ӡT�T"
      "$R�z$A���ۤ@�����j�����y�g�b�V$E�T�T$Z" );
    break;

  case 10:
    sact( ch, victim, NULL,
      "$R$n�צa�H��M�o�o�X�@�ѥ��j���_�٤O�T$A$A"
      "���ۧA������䱼�U�@�����j�ۤ����j���y�ĦV$N�T$Z" );
    break;

  case 11:
    sact( ch, victim, NULL,
      "$R$n����@���M��������Ƶo�͡C\n\r\n\r"
      "�X�����L��M�@��������K�����l��b�ӦܡT\n\r"
      "�G�ܤ����L�K�V$N$R�Q�X�@�s�ꪺ�y$Y�T�N�u��$R�z�C$Z" );
    break;

  case 12:
    sact( ch, victim, NULL,
      "$n�����M�X�{�@�ǤC�m���]�]�M��¶��$e�M�b�O�n�ݡT$A"
      "�M���]���������M�oť���j�a�}�l�_�ʡM���G�Y�N�o�ͤ@�Ǥ����"
      "�T�T$A�b��$n�j�ۤ@�n�R�y $R��   ��   �i   �S �T�T�T"
      "$Z�z$A$A�಴���b�~�ӪŬY�B�@���������������s����P��"
      "�M�V�ۦ��a�Q�X�@�ӶW���������y�T�T$A$R���y�Y�N�F�줧��"
      "�M���B�w�e�@�������M���󤣥͡T�T$A���ۦ��a"
      "���H�����h��ı�M���鳣����i�ΡM�ר�O$N�T�T$Z" );

    send_to_char( "\e[1;31m�A�]�����i�ΡM���K�I���T�T\e[0m\n\r", ch );
    damage( ch, ch, dam * dam_plus , sn, ATTACK_RANDOM );

    /* �Y�I�k�̤����}�` */
    if ( !can_damage( ch ) ) RETURN( TRUE );

    dam_plus *= 4;
    break;
  }

  damage( ch, victim, UMIN( LEVEL_FOU_DAM, dam * dam_plus ), sn, ATTACK_RANDOM );
  RETURN( TRUE );
}

SKILL( spell_cast_lightning )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         dam;
  int         dam_plus;
  int         choice[] =
              { 25, 35, 45, 55, 65, 75, 80, 85, 90, 95, 98, 99, 100, -1 };

  PUSH_FUNCTION( "spell_cast_lightining" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "spell_cast_lightning: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  dam = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_FOU );
  dam = dam / 2;

  switch( UMIN( 3, dam_plus = dimemsion_position( choice, level, TRUE ) ) )
  {
  case ERRORCODE:
    mudlog( LOG_DEBUG, "�ޯ� %d ���T���]�w�����D." , sn );
    RETURN( FALSE );

  case 0:
    sact( ch, victim, NULL,
      "$S$n����@�ݡM�o�X�@�Ӥp�q�y���V$N�T$Z" );
    break;

  case 1: case 2: case 3: case  4: case  5: case  6:
  case 7: case 8: case 9: case 10: case 11: case 12:
    sact( ch, victim, NULL,
      "$S$n���@���M�N�q�⤤�o�X��D�R�E���n���{�q�M�r�c����V$N�T$Z" );
    break;
  }

  damage( ch, victim, UMIN( LEVEL_FOU_DAM, dam * dam_plus ), sn, ATTACK_RANDOM );

  RETURN( TRUE );
}

SKILL( cast_control_weather )
{
  PUSH_FUNCTION( "cast_control_weather" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "cast_control_weather: �ӷ����~. " );
    RETURN( FALSE );
  }

  if ( number_percent() > ( number_percent() / 2 + level - 10 ) )
  {
    send_to_char( "�Ѯ���G�S�������ܤơT\n\r"
      "�A�Q��Ѯ��ܦn (\e[1;32mbetter\e[0m) "
      "�άO���a (\e[1;32mworse\e[0m)�S\n\r", ch );
    RETURN( FALSE );
  }

  if ( !str_cmp( ( char * ) vo , "better" ) )
  {
    weather_info.change += dice( level / 3, 4 );
    send_to_char( "�n�F�M�Ѯ�V�ӶV�n�F�T\n\r", ch );
  }

  else if ( !str_cmp( ( char * ) vo , "worse" ) )
  {
    weather_info.change -= dice( level / 3, 4 );
    send_to_char( "�n�F�M�Ѯ�V�ӶV�a�F�T\n\r", ch );
  }

  else
  {
    send_to_char( "�A�Q��Ѯ��ܦn (\e[1;32mbetter\e[0m) "
      "�άO���a (\e[1;32mworse\e[0m)�S\n\r", ch );

    RETURN( FALSE );
  }

  RETURN( TRUE );
}

SKILL( cast_sparkler )
{
  extern const sh_int   rev_dir [];
  char                  buf[MAX_STRING_LENGTH];
  CHAR_DATA           * pMount;
  ROOM_INDEX_DATA     * pRoom;
  ROOM_INDEX_DATA     * pSource;
  int                   count;
  int                   degree;
  int                   loop;
  int                   dir;

  PUSH_FUNCTION( "cast_sparkler" );

  if ( !( pSource = ch->in_room ) )
  {
    send_to_char( "�藍�_�M���A�ثe�b���̩O�S\n\r", ch );
    RETURN( FALSE );
  }

  if ( !IS_OUTSIDE( ch ) )
  {
    act( "�A�L�k�b$r�I�i�o�ӷϤ��N�T", ch, NULL, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  if ( ch->in_room->area == FoggyArea )
  {
    act( "�ثe$r�@�������ۡM�o�ɬI�i�Ϥ��N�O�S���Ϊ��T", ch, NULL, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  if ( weather_info.sky == SKY_RAINING )
  {
    send_to_char( "�U�B�ѬO�S����k�I�i�Ϥ��N�T\n\r", ch );
    RETURN( FALSE );
  }

  if ( ( pMount = get_mount( ch ) ) )
  {
    act( "�A���b$N�W�O�S����k�I�i�Ϥ��N���C", ch, NULL, pMount, TO_CHAR );
    RETURN( FALSE );
  }

  degree = UMIN( MAX_SCAN_DEPTH - 1, UMAX( 2, level / 10 ) );

  clean_scan();
  scan_room( ch->in_room->vnum, degree, -1 );

  for ( loop = 0; loop < ScanPointer; loop++ )
  {
    if ( !( pRoom = get_room_index( ScanData[loop].vnum ) ) ) continue;

    char_from_room( ch );
    char_to_room( ch, pRoom );

    str_cpy( buf, "�b�A��$2" );

    for ( count = MAX_SCAN_DEPTH - 1; count >= 0; count-- )
    {
      if ( ( dir = ScanData[loop].dir[count] < 0 ) || dir >= DIR_MAX )
        continue;

      str_cat( buf, direction_name( rev_dir[ScanData[loop].dir[count]] ) );
    }

    str_cat( buf, "$0��V�X�{�Y�H�I�񪺷Ϥ������M�n���}�G�T" );
    act( smash_act_keyword( buf, "nes" ), ch, NULL, NULL, TO_ROOM );

    char_from_room( ch );
    char_to_room( ch, pSource );
  }

  clean_scan();

  act( "$n����ϴ��M�I�i�X�Ϥ��N�M�b�ɤѪŤ@�����G�T", ch, NULL, NULL, TO_ALL );
  RETURN( TRUE );
}

SKILL( cast_search )
{
  ROOM_INDEX_DATA * pSource;
  ROOM_INDEX_DATA * pRoom;
  CHAR_DATA       * victim;
  char              buf[MAX_STRING_LENGTH];
  char            * pString = ( char * ) vo;
  int               loop;
  int               count;
  int               degree;
  int               height;
  int               number;

  PUSH_FUNCTION( "cast_search" );

  if ( !pString || *pString == '\x0' )
  {
    send_to_char( "�A�Q�n�M�䤰��H�O�S\n\r", ch );
    RETURN( FALSE );
  }

  if ( !( pSource = ch->in_room ) )
  {
    send_to_char( "�藍�_�M���A�ثe�b���̩O�S\n\r", ch );
    RETURN( FALSE );
  }

  if ( !IS_OUTSIDE( ch ) )
  {
    act( "�A�L�k�b$r�I�i�o���˪ŴM�H�N�T", ch, NULL, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  if ( ch->in_room->area == FoggyArea )
  {
    act( "�ثe$r�@�������ۡM�o�ɬI�i�˪ŴM�H�O�S���Ϊ��T", ch, NULL, NULL, TO_CHAR  );
    RETURN( FALSE );
  }

  if ( weather_info.sky == SKY_RAINING )
  {
    send_to_char( "�U�B�ѬO�S����k�I�i�˪ŴM�H�N�T\n\r", ch );
    RETURN( FALSE );
  }

  if ( ( victim = get_mount( ch ) ) )
  {
    act( "�A���b$N�W�O�S����k�I�i�˪ŴM�H�N���C", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  degree = UMIN( MAX_SCAN_DEPTH - 1, UMAX( 2, level / 10 ) );
  height = degree * 2;

  act( "$n�a���@���M����$i�V���M���V�O���U�[��a���ζաT"
    , ch, &height, NULL, TO_ALL );

  clean_scan();
  scan_room( ch->in_room->vnum, degree, -1 );

  for ( number = loop = 0; loop < ScanPointer; loop++ )
  {
    if ( !( pRoom = get_room_index( ScanData[loop].vnum ) ) ) continue;

    char_from_room( ch );
    char_to_room( ch, pRoom );

    act( "����$n�a���@���M����$i�V���M���V�O���U�[��T"
      , ch, &height, NULL, TO_ROOM );

    for ( victim = pRoom->people; victim; victim = victim->next_in_room )
    {
      if ( ch != victim
        && can_see( ch, victim )
        && is_fullname( pString, victim->name ) )
      {
        number++;
        str_cpy( buf, "�b�A��$2" );

        for ( count = 0; count < MAX_SCAN_DEPTH; count++ )
        {
          if ( ScanData[loop].dir[count] < 0 ) break;
          str_cat( buf, direction_name( ScanData[loop].dir[count] ) );
        }

        str_cat( buf, "$0��$N�M�����D�O���O�A�n�䪺��H�T" );
        act( smash_act_keyword( buf, "nesNES" ), ch, NULL, victim, TO_CHAR );
      }
    }

    char_from_room( ch );
    char_to_room( ch, pSource );

    if ( number >= UMAX( 1, degree / 2 ) ) break;
  }

  if ( number <= 0 ) send_to_char( "�藍�_�M�䤣��A�n�M�䪺�H�C�T\n\r", ch );

  RETURN( TRUE );
}

SKILL( cast_locate_object )
{
  int        count;
  char       buf[MAX_STRING_LENGTH];
  OBJ_DATA * obj;
  OBJ_DATA * in_obj;
  char     * pString = ( char * ) vo;
  bool       found;

  PUSH_FUNCTION( "cast_locate_object" );

  if ( !pString || *pString == '\x0' )
  {
    send_to_char( "�A�n�p��i�{�A���j���q�O�S\n\r", ch );
    RETURN( FALSE );
  }

  act( "$n�C�Y���ءM�}�l�I�i�C�ѯ��q�C", ch, NULL, NULL, TO_ALL );
  buf[0] = '\x0';

  clear_buffer();
  for ( count = 0, found = FALSE, obj = object_list; obj; obj = obj->next )
  {
    if ( buffer_full() ) break;
    if ( count >= ( level / 2 ) )
    {
      if ( found ) send_to_buffer( "\n\r�A�j�M��O�w�g�ӺɡT\n\r" );
      break;
    }

    if ( !verify_obj( obj )
      || !can_see_obj( ch, obj )
      || !is_name( pString, obj->name ) )
      continue;

    for ( in_obj = obj; in_obj->in_obj; in_obj = in_obj->in_obj );
    if ( !in_obj || !verify_obj( in_obj ) ) continue;

    found = TRUE;
    count++;
    if ( in_obj->carried_by )
    {
      send_to_buffer( "�@�}�p�q�L��M�A�{�L�@�ө��Y�M�ڡM%s ���� %s ��a�C\n\r"
        , obj_name( ch, obj )
        , mob_name( ch, in_obj->carried_by ) );
    }
    else
    {
      if ( level < ( LEVEL_HERO / 2 ) )
      {
        send_to_buffer( "����ǨӤ@�}�n���M���M�Q�A�o�{�F�M"
          "�i�O�H�A���\\�O�O�䤣��ڪ��M�����T\n\r" );
      }
      else
      {
        send_to_buffer( "��M�{�L�@�}�ե��M�A�o�{��� %s\e[0m �b %s\e[0m �̭��T\n\r"
          , obj_name( ch, obj )
          , in_obj->in_room == NULL ? "�Y�Ӧa��" : in_obj->in_room->name );
      }
    }

  }

  if ( !found )
  {
    send_to_char( "�Ať���իճB�ǨӤ@�}�n���M"
      "�o����䤣��A�n�����~�M���Aı�o���@�Ѥ򰩮��M���Pı�C\n\r", ch );
  }
  else
  {
    print_buffer( ch );
  }

  RETURN( TRUE );
}

SKILL( cast_summon )
{
  CHAR_DATA * victim;

  PUSH_FUNCTION( "cast_summon" );

  if ( !ch->in_room )
  {
    mudlog( LOG_DEBUG, "cast_summon: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( !( victim = get_char_area( ch, (char *) vo ) )
    || !verify_char( victim )
    || !victim->in_room )
  {
    act( "�o����S���o�ӤH��T", ch, NULL, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  if ( victim == ch )
  {
    act( "�A�S�ƥl��ۤv������S", ch, NULL, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  if ( ch->in_room == victim->in_room )
  {
    act( "$n��$N�u���@�u���j�a�T���٭n�l��O�S"
      , ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  if ( is_loge( victim->in_room )
    || victim->in_room->Safe
    || victim->in_room->Private
    || victim->in_room->NoRecall
    || victim->in_room->NoWhere
    || victim->in_room->Sail
    || victim->in_room->NoQuit
    || ch->in_room->NoWhere )
  {
    act( "$n���k�N���ѤF�M$N����m�ݨӤ�����ϥΥl��N��T"
      , ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  if ( victim->fighting
    || victim->mount
    || victim->mount_by
    || victim->boss
    || victim->spirit
    || victim->position != POS_STANDING )
  {
    act( "$n���ѤF�M$N���b���O�Mť�]ť����A���l��T"
      , ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  if ( IS_NPC( victim ) && IS_SET( victim->act, ACT_TRAIN ) )
  {
    act( "�Ф��n��$N�s�ӥs�h�T", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  if ( is_affected( victim, SLOT_CURSE )
    || is_affected( ch, SLOT_CURSE )
    || victim->level >= level + 3
    || is_keeper( victim )
    || IS_SET( victim->act, ACT_NOSUMMON ) )
  {
    send_to_char( "����ǨӤ@�}���n�J���A�M�A���l�ꥢ�ѤF�C\n\r", ch );
    RETURN( FALSE );
  }

  if ( is_affected( victim, SLOT_FIXITY ) )
  {
    act( "$N�n��ť����A���l��T", ch, NULL, victim, TO_CHAR );
    RETURN( TRUE );
  }

  act( "$n�b�@�}�շ�Ţ�n���U��M���������F�T", victim, NULL, NULL, TO_ROOM );
  char_from_room( victim );
  char_to_room( victim, ch->in_room );
  act( "$n��M�����q�@�D�ɰ_�����̭����F�X�ӡT", victim, NULL, NULL, TO_ROOM );
  act( "$N���b���յۥl��A�T", ch, NULL, victim,   TO_VICT );
  do_look( victim, "auto" );

  /* �M���l�ܬ����I */
  clear_trace( victim, TRUE );

  RETURN( TRUE );
}

SKILL( cast_ventriloquate )
{
  char      * pString = ( char * ) vo;
  char        buf1[MAX_STRING_LENGTH];
  char        buf2[MAX_STRING_LENGTH];
  char        speaker[MAX_INPUT_LENGTH];
  CHAR_DATA * vch;

  PUSH_FUNCTION( "cast_ventriloquate" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "cast_ventriloquate: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( !ch->in_room || !can_see_room( ch ) ) RETURN( FALSE );

  pString = one_argument( pString, speaker );
  sprintf( buf1, "%s ���D: '%s'�C\n\r",     speaker, pString );
  sprintf( buf2, "�Y�H�s %s �� '%s'�C\n\r", speaker, pString );

  for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
  {
    if ( !is_name( speaker, vch->name ) )
      send_to_char( ( vch->level >= ch->level ) ? buf2 : buf1, vch );
  }

  RETURN( TRUE );
}

SKILL( cast_acid_blast )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         dam;

  PUSH_FUNCTION( "cast_acid_blast" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_acid_blast: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  dam = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_ONE );

  act( "$n�ʰʪk�O�M�N�f���ܦ��j�ġM�@�f��$N�R�h�C"
    , ch, NULL, victim, TO_ALL );

  damage( ch, victim, dam, sn, ATTACK_RANDOM );
  if ( !can_damage( victim ) ) RETURN( TRUE );

  if ( level > 50 )
  {
    sact( ch, victim, NULL,
      "$n�f���R�X���ĲG���U�M�_�X�}�}�շϡM�o��$N�y���ܤj���ˮ`�C" );
    damage( ch, victim, dam * 2, sn, ATTACK_RANDOM );
  }

  RETURN( TRUE );
}

SKILL( cast_blindness )
{
  CHAR_DATA   * vic = ( CHAR_DATA * ) vo;
  AFFECT_DATA   af;
  int           chance;

  PUSH_FUNCTION( "cast_blindness" );

  if ( !vic )
  {
    mudlog( LOG_DEBUG, "cast_blindess: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( !can_fight( ch, vic ) )
  {
    act( "�A����˽M$N�����C", ch, NULL, vic, TO_CHAR );
    RETURN( FALSE );
  }

  if ( is_affected( vic, sn ) )
  {
    act( "�藍�_�M$N�w�g�����F�T", ch, NULL, vic, TO_CHAR );
    RETURN( FALSE );
  }

  if ( !can_damage( vic ) )
  {
    act( "�A�ثe�S����k�˽M$N�T", ch, NULL, vic, TO_CHAR );
    RETURN( FALSE );
  }

  chance = UMAX( 30 , level ) + get_curr_dex( ch ) - get_curr_dex( vic );
  chance = UMAX( 5, chance );

  if ( number_range( 1, 100 ) > chance )
  {
    send_to_char( "�藍�_�M�A���ѤF�T\n\r", ch );
    RETURN( FALSE );
  }

  act( "$n�εL�񪺤��F�_���l�Ӷ·t�M�b�����M����·t��$n��$N�@���M"
       "�N���·t���b$N���W�C", ch, NULL, vic, TO_ALL );

  af.type      = sn;
  af.location  = APPLY_HITROLL;
  af.modifier  = 0 - ( level / 2 );
  af.duration  = 1 + ( level / 5 );
  af.level      = level;
  affect_to_char( vic, &af );
  send_to_char( "�¥��@�{�M�A�Ȯɲ����F�T\n\r", vic );
  if ( ch != vic ) send_to_char( "���\\�I�i�����N�F�T\n\r", ch );
  RETURN( TRUE );
}

SKILL( cast_burning_hands )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         dam    = 0;

  PUSH_FUNCTION( "cast_burning_hands" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_burning_hands: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  /* �]�w�ԭz */
  if ( number_percent() > level )
  {
    sact( ch, victim, NULL,
      "$n�Ϻɥ��O�M�ĤO�N���x�ʤW�k�O�Ϥ��o���_$R��$Z�M�V$N���h�T" );
  }
  else
  {
    sact( ch, victim, NULL,
      "$n����@���M�@��$R�_������x$Z�b��W�Φ��M�ߧY�H�ܧ֪��t�צV"
      "$N���h�M�o�X�@�D���������u�T" );
  }

  /* �]�w��� */
  dam = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_ONE );

  damage( ch, victim, dam, sn, ATTACK_RANDOM );
  RETURN( TRUE );
}

SKILL( cast_call_lightning )
{
  CHAR_DATA * vch;
  CHAR_DATA * vch_next;
  int         dam;

  PUSH_FUNCTION( "cast_call_lighting" );

  if ( !IS_OUTSIDE( ch ) )
  {
    send_to_char( "�A�����n�b��~�M���M�S��k��q�C\n\r", ch );
    RETURN( FALSE );
  }

  if ( level < 70 && weather_info.sky < SKY_RAINING )
  {
    send_to_char( "�Ѯ�n���ܡM�L�k�I�i�o�Ӫk�N�C\n\r", ch );
    RETURN( FALSE );
  }

  /* �]�w��� */
  dam = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_THR );
  dam = dam / 2;

  if ( level < 50 )
    send_to_char( "�ѪŬ�M���_�p�ӡT\n\r", ch );
  else
    send_to_char( "�A�I�s�@���B���M�a�Ӥ@�}�{�q�M�����Ҧ��b�����ĤH�T\n\r"
      ,ch );

  act( "$n�I�s�ѹp�M���V�o�̩Ҧ����ĤH�T", ch, NULL, NULL, TO_ROOM );

  for ( vch = char_list; vch; vch = vch_next )
  {
    vch_next = vch->next;

    if ( !verify_char( vch ) || !vch->in_room ) continue;

    if ( vch->in_room == ch->in_room )
    {
      if ( vch != ch && ( IS_NPC( ch ) ? !IS_NPC( vch ) : IS_NPC( vch ) ) )
        damage( ch, vch, saves_spell( level, vch ) ? dam/2 : dam, sn, ATTACK_RANDOM );

      continue;
    }

    if ( vch->in_room->area == ch->in_room->area
      && IS_OUTSIDE( vch )
      && IS_AWAKE( vch ) )
      send_to_char( "�@�U�{�q���L����M��ģ�۾�ӤѪšC\n\r", vch );
  }

  RETURN( TRUE );
}

SKILL( cast_cause_light )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         dam;

  PUSH_FUNCTION( "cast_cause_light" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_cause_light: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( level < 50 )
    sact( ch, victim, NULL,
      "$n�����G�y�M�����b$N�|�P�X�{�����W���·t�M�I�k��$E�T" );
  else
    sact( ch, victim, NULL,
      "$n��X�@�q�}�`���G�y�M�N$N�]���b�·t�����M�@�L�樭�l�a�T" );

  /* �]�w��� */
  dam = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_ONE );

  damage( ch, victim, dam, sn, ATTACK_RANDOM );
  RETURN( TRUE );
}

SKILL( cast_cause_critical )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         dam;

  PUSH_FUNCTION( "cast_cause_critical" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_cause_critical: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( level < 50 )
    sact( ch, victim, NULL,
      "$n����@�СM�{�X�@���·t�M�I�k��$N�����F�C\n\r"
      "�u��$N�b�@�ǺG�z�ۡM���G�ܵh�W�T" );
  else
    sact( ch, victim, NULL,
      "$n����e��M���X�@�ӥi�����·t�y�魸�V$N�C\n\r"
      "$N�Q�y��]�СM���G�����h�W�T" );

  dam = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_FOU );

  damage( ch, victim, dam, sn, ATTACK_RANDOM );
  RETURN( TRUE );
}

SKILL( cast_cause_serious )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         dam;

  PUSH_FUNCTION( "cast_cause_serious" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_cause_serious: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( level < 50 )
    sact( ch, victim, NULL,
      "$n�V������D�·t�M�ñN���·t���b$N���W�C" );
  else
    sact( ch, victim, NULL,
      "$n��M���U��D�R�u�U�઺�����ڡT�N�L�񪺶·t���b$N���W�a�T" );

  /* �]�w��� */
  dam = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_TWO );

  damage( ch, victim, dam, sn, ATTACK_RANDOM );
  RETURN( TRUE );
}

SKILL( cast_change_sex )
{
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA   af;

  PUSH_FUNCTION( "cast_change_sex" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_change_sex: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( is_affected( victim, sn ) )
  {
    act( "�o�ݰ_�ӹ�$N���G�_���F�@�ΡT", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  af.type      = sn;
  af.duration  = 10 * level;
  af.location  = APPLY_SEX;

  switch( victim->sex )
  {
  case SEX_FEMALE:
    af.modifier = SEX_MALE - SEX_FEMALE;
    break;

  case SEX_MALE:
    af.modifier = SEX_FEMALE - SEX_MALE;
    break;

  default:
    act( "$N�L�k�i���ܩ�.", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  af.level     = level;
  affect_to_char( victim, &af );

  act( "$n�⵲�t�L�M�M�߬�ë�M�����M���U�@���C�m�����T$A"
       "$n�����Q�o�����]�ۡM�����D�o�ͤF����ơT$A"
       "���ۡM���H���M�M���G���򳣨S���o�͹L�C"
       , ch, NULL, victim, TO_ALL );

  send_to_char( "�A�Pı���W���G�_�F�@�ǩǩǪ��ܤơT\n\r", victim );
  if ( ch != victim ) send_to_char( "�n�F�M�ܩʧ����C\n\r", ch );
  RETURN( TRUE );
}

SKILL( cast_charm_person )
{
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA   af;

  PUSH_FUNCTION( "cast_charm_person" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_charm_person: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( victim == ch )
  {
    send_to_char( "�ͩR�O�ܥi�Q���M�Цh�[�ñ��T\n\r", ch );
    RETURN( FALSE );
  }

  if ( !can_damage( victim ) )
  {
    act( "�A�ثe�S����k�g�b$N�T\n\r", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  /* �Y��H�O���a�B���O�s��άO���H�ǫh���� */
  if ( is_pk( ch ) && is_pk( victim ) )
  {
    if ( fight_info->status != PK_FIGHT )
    {
      send_to_char( "�b�o�q�ɶ��M�٬O�n�n�a�𮧤@�U�a�T\n\r", ch );
      RETURN( FALSE );
    }
  }

  else if ( !IS_NPC( victim ) )
  {
    if ( !IS_SET( victim->act, PLR_KILLER )
      && !IS_SET( victim->act, PLR_THIEF  )
      && !IS_SET( victim->act, PLR_BOLTER ) )
    {
      act( "�A�n����$N�M�Х� $2MURDER$0 �o�ӫ��O�C", ch, NULL, victim, TO_CHAR );
      RETURN( FALSE );
    }
  }

  if ( !can_fight( ch , victim ) ) RETURN( FALSE );

  if ( victim->boss )
  {
    act( "�藍�_�M�A����g�b$N���p���T", ch, NULL, victim->boss, TO_CHAR );
    RETURN( FALSE );
  }

  if ( is_affected( victim, sn )
    || is_affected( ch, sn )
    || victim->fighting
    || saves_spell( level, victim ) )
  {
    send_to_char( "�u�r�T�g�b���k�N���ѤF�T\n\r", ch );
    RETURN( TRUE );
  }

  if ( victim->level > ( level / 10  + number_range( 1, level ) ) )
  {
    sact( ch, victim, NULL, "$N��M����$n������M��$n�i�}�����T" );
    set_fighting( victim, ch );
    RETURN( FALSE );
  }

  if ( victim->master ) stop_follower( victim );

  add_follower( victim, ch );
  af.type      = sn;
  af.duration  = number_fuzzy( level / 5 );
  af.location  = 0;
  af.modifier  = 0;
  af.level     = level;
  affect_to_char( victim, &af );

  act( "��M�����M�Aı�o$n�n���A�����h�~���D�H�T", ch, NULL, victim, TO_VICT );
  if ( ch != victim ) send_to_char( "�n�F�M�g�b���\\�T\n\r", ch );
  RETURN( TRUE );
}

SKILL( cast_chill_touch )
{
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA   af;
  int           dam;

  PUSH_FUNCTION( "cast_chill_touch" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_chill_touch: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  dam = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_ONE );

  if ( !saves_spell( level, victim ) && get_curr_str( victim ) > 1 )
  {
    af.type      = sn;
    af.duration  = number_range( level / 15, level / 12 );
    af.location  = APPLY_STR;
    af.modifier  = -1;
    af.level     = level;
    affect_join( victim, &af );
  }
  else
  {
    dam >>= 1;
  }

  damage( ch, victim, dam, sn, ATTACK_RANDOM );
  RETURN( TRUE );
}

SKILL( cast_colour_spray )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         dam;

  PUSH_FUNCTION( "cast_colour_spray" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "cast_colour_spray: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  /* ��˱ԭz */
  sact( ch, victim, NULL,
    "$n����@��M�⤤�o�X�@�D$R�C$Y��$G�m$S�i$Z���V$N�T" );

  /* �]�w��� */
  dam = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_ONE );

  if ( saves_spell( level, victim ) ) dam /= 2;
  damage( ch, victim, dam, sn, ATTACK_RANDOM );
  RETURN( TRUE );
}

SKILL( cast_create_food )
{
  OBJ_DATA * mushroom;
  OBJ_DATA * obj;
  int        choice[] = { 35, 55, 70, 100, -1 };

  PUSH_FUNCTION( "cast_create_food" );

  if ( !ch->in_room )
  {
    mudlog( LOG_DEBUG, "cast_create_food: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
    if ( obj->item_type == ITEM_FOOD ) break;

  if ( obj )
  {
    act( "$r�̭����״I�M�֦Y�a�M�ٰ����򭹪��T", ch, NULL, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  if ( ch->in_room->nobject >= MaxRoomObject )
  {
    send_to_char( "�o�̪����~�Ӧh�F�M�Х��^�m���@�ǧa�T\n\r", ch );
    RETURN( FALSE );
  }

  switch( dimemsion_position( choice, level , TRUE ) )
  {
  default:

    mudlog( LOG_DEBUG, "cast_create_food: �W�X�d��." );
    RETURN( FALSE );

  case 0:

    if ( !( mushroom = create_object( ObjMushroom, 1 ) ) )
    {
      mudlog( LOG_DEBUG, "cast_create_food: �S������: %d.", ObjMushroomVnum );
      RETURN( FALSE );
    }
    break;

  case 1:

    if ( !( mushroom = create_object( ObjDumpling, 1 ) ) )
    {
      mudlog( LOG_DEBUG, "cast_create_food: �S������: %d.", ObjDumplingVnum );
      RETURN( FALSE );
    }
    break;

  case 2:

    if ( !( mushroom = create_object( ObjBougi, 1 ) ) )
    {
      mudlog( LOG_DEBUG, "cast_create_food: �S������: %d.", ObjBougiVnum );
      RETURN( FALSE );
    }
    break;

  case 3:

    if ( !( mushroom = create_object( ObjPon, 1 ) ) )
    {
      mudlog( LOG_DEBUG, "cast_create_food: �S������: %d.", ObjPonVnum );
      RETURN( FALSE );
    }
    break;

  case 4:

    if ( !( mushroom = create_object( ObjChicken, 1 ) ) )
    {
      mudlog( LOG_DEBUG, "cast_create_food: �S������: %d.", ObjChickenVnum );
      RETURN( FALSE );
    }
    break;
  }

  mushroom->value[0] = 5 + level;
  obj_to_room( mushroom, ch->in_room );
  act( "$p��M�Ѧa�W�_�F�X�ӡC", ch, mushroom, NULL, TO_ROOM );
  act( "$p�b�@��ե����Ѧa�W�_�F�X�ӡC", ch, mushroom, NULL, TO_CHAR );
  RETURN( TRUE );
}

SKILL( cast_create_water )
{
  OBJ_DATA * obj = ( OBJ_DATA * ) vo;
  int water;

  PUSH_FUNCTION( "cast_create_water" );

  if ( !obj )
  {
    mudlog( LOG_DEBUG, "cast_create_water: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( obj->item_type != ITEM_DRINK_CON )
  {
    act( "$p���i�H���Ӹˤ���C", ch, obj, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  if ( obj->value[2] != liq_water->slot && obj->value[1] != 0 )
  {
    act( "$p�̭��w�g����L�G���o�C", ch, obj, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  water = UMIN( level * ( weather_info.sky >= SKY_RAINING ? 4 : 2 )
                , obj->value[0] - obj->value[1] );

  if ( water > 0 )
  {
    obj->value[2]  = liq_water->slot;
    obj->value[1] += water;

    if ( !is_name( "water", obj->name ) )
    {
      char buf[MAX_STRING_LENGTH];

      sprintf( buf, "%s water", obj->name );
      free_string( obj->name );
      obj->name = str_dup( buf );
    }

    act( "$p�����ֺ��X�ӤF�T", ch, obj, NULL, TO_CHAR );
  }

  RETURN( TRUE );
}

SKILL( cast_cure_blindness )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;

  PUSH_FUNCTION( "cast_cure_blindness" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_cure_blindness: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( is_affected( victim, SLOT_BLINDNESS ) )
  {
    affect_release( victim, SLOT_BLINDNESS );
    send_to_char( "�A�S���������F�T\n\r", victim );

    if ( ch != victim ) send_to_char( "��v�������\\�T\n\r", ch );
    RETURN( TRUE );
  }
  else
  {
    act( "$N�������Q�������`�ڡT", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  RETURN( FALSE );
}

SKILL( cast_cure_critical )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         heal;

  PUSH_FUNCTION( "cast_cure_critical" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_cure_critical: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( victim->hit >= get_curr_hit( victim ) )
  {
    act( "$N���ͩR�w�g�����F�T", ch , NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  heal = ( number_range( 50, 100 ) + level * 2 ) * get_curr_int( ch ) / 4;
  victim->hit = UMIN( victim->hit + heal, get_curr_hit( victim ) );

  if ( victim->position == POS_SLEEPING && victim != ch )
    act( "$N����v�����ʤF�A�T", victim, NULL, ch, TO_CHAR );

  update_pos( victim );
  send_to_char( "�A�ݨӦ��G����n�F�T\n\r", victim );

  if ( victim->hit == get_curr_hit( victim ) )
    send_to_char( "�A�������ˤf�������F�T\n\r", victim );

  if ( ch != victim ) send_to_char( "�n�F�M��v���\\�T\n\r", ch );
  RETURN( TRUE );
}

SKILL( cast_cure_overdose )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         num;

  PUSH_FUNCTION( "cast_cure_overdose" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_cure_overdose: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( !victim->pcdata || victim->pcdata->dosage <= 0 )
  {
    act( "$N�èS���Ī����r��T", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  num = UMAX( 1, number_range( level / 4 , level / 2 ) );
  victim->pcdata->dosage = UMAX( 0, victim->pcdata->dosage - num );

  if ( victim->position == POS_SLEEPING && victim != ch )
    act( "$N����v�����ʤF�A�T", victim, NULL, ch, TO_CHAR );

  update_pos( victim );

  if ( victim->pcdata->dosage <= 0 )
  {
    send_to_char( "�A���W���Ī��l�r�������F�T\n\r", victim );
  }
  else
  {
    send_to_char( "�A���W���l�r���h�F���֡T\n\r", victim );
  }

  if ( ch != victim ) send_to_char( "�I�k�n�F�C\n\r", ch );

  RETURN( TRUE );
}

SKILL( cast_remeday )
{
  CHAR_DATA * victim;
  int         percent;
  int         heal;

  PUSH_FUNCTION( "cast_remeday" );

  if ( !ch->in_room )
  {
    send_to_char( "�A���B���ҡM�L�k�I�i���k�N�T\n\r", ch );
    RETURN( FALSE );
  }

  percent = number_range( level * get_curr_int( ch ) * 70
                        , level * get_curr_int( ch ) * 90 );

  percent = UMAX( 1, UMIN( 100, percent / 7000 ) );

  for ( victim = char_list; victim; victim = victim->next )
  {
    if ( verify_char( victim )
      && victim->in_room
      && is_same_group( victim, ch )
      && victim->in_room->vnum == ch->in_room->vnum
      && victim->hit < victim->max_hit )
    {
      heal = victim->max_hit * percent / 100;
      victim->hit = UMIN( victim->max_hit, victim->hit + heal );
      act( "$N������^�K�M�ϧA�Ѧ��`��t�ԤF�^�ӡT", victim, NULL, ch, TO_CHAR );
    }
  }

  send_to_char( "�A������j�ˡM�o�n�n�׾i�T\n\r", ch );
  RETURN( TRUE );
}

SKILL( cast_cure_light )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         heal;

  PUSH_FUNCTION( "cast_cure_light" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_cure_light: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( victim->hit >= get_curr_hit( victim ) )
  {
    act( "$N���ͩR�w�g�����F�T", ch , NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  heal = ( number_range( 40, 80 ) + level / 2 ) * get_curr_int( ch ) / 4;
  victim->hit = UMIN( victim->hit + heal, get_curr_hit( victim ) );

  if ( victim->position == POS_SLEEPING && victim != ch )
    act( "$N����v�����ʤF�A�T", victim, NULL, ch, TO_CHAR );

  update_pos( victim );
  send_to_char( "�A�Pı�n�h�F�T\n\r", victim );

  if ( victim->hit == get_curr_hit( victim ) )
    send_to_char( "�A�������ˤf�������F�T\n\r", victim );

  if ( ch != victim ) send_to_char( "�I�k�n�F�C\n\r", ch );

  RETURN( TRUE );
}

SKILL( cast_cure_poison )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;

  PUSH_FUNCTION( "cast_cure_poison" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_cure_poison: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( is_affected( victim, SLOT_POISON ) )
  {
    affect_release( victim, SLOT_POISON );
    act( "$N�ݰ_�Ӧn�h�F", ch, NULL, victim, TO_NOTVICT );
    send_to_char( "�@�}�M�y�y�J�A�������M���ΪA���Pı���A�C\n\r", victim );
    send_to_char( "�n�F�C\n\r", ch );
    RETURN( TRUE );
  }

  else
  {
    act( "$Nı�o�S��������ܡT", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  RETURN( FALSE );
}

SKILL( cast_cure_serious )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         heal;

  PUSH_FUNCTION( "cast_cure_serious" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_cure_serious: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( victim->hit >= get_curr_hit( victim ) )
  {
    act( "$N���ͩR�w�g�D�`�����F�T", ch , NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  heal = ( number_range( 50, 100 ) + level ) * get_curr_int( ch ) / 4;
  victim->hit = UMIN( victim->hit + heal, get_curr_hit( victim ) );

  if ( victim->position == POS_SLEEPING && victim != ch )
    act( "$N����v�����ʤF�A�T", victim, NULL, ch, TO_CHAR );

  update_pos( victim );
  send_to_char( "�A�Pı�n�h�F�T\n\r", victim );

  if ( victim->hit == get_curr_hit( victim ) )
    send_to_char( "�A���ˤf���w�g�����F�T\n\r", victim );

  if ( ch != victim ) send_to_char( "�n�F�C\n\r", ch );

  RETURN( TRUE );
}

SKILL( cast_curse )
{
  CHAR_DATA   * vic = ( CHAR_DATA * ) vo;
  AFFECT_DATA   af;

  PUSH_FUNCTION( "cast_curse" );

  if ( !vic )
  {
    mudlog( LOG_DEBUG, "cast_curse: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( is_affected( vic, sn ) || saves_spell( level, vic ) )
  {
    act( "�A�L�k�A�G$N�T", ch, NULL, vic, TO_CHAR );
    RETURN( FALSE );
  }

  if ( vic->alignment > 0 && ( vic->alignment / 10 > level ) )
  {
    act( "��$N���Y�W�ɰ_�F�@�Ϊ��զ���~�M����F$n���A�G�T"
        , ch, NULL, vic, TO_ALL );

    RETURN( FALSE );
  }

  af.type      = sn;
  af.duration  = UMAX( 1, number_range( level / 4, level / 2 ) );
  af.location  = APPLY_HITROLL;
  af.modifier  = -1;
  af.level     = level;
  affect_to_char( vic, &af );

  af.location  = APPLY_SAVING_SPELL;
  af.modifier  = 1;
  affect_to_char( vic, &af );

  send_to_char( "�A�P��@�}�t������ۧA���O�q�C\n\r", vic );
  if ( ch != vic ) send_to_char( "�A�G���\\�C\n\r", ch );
  RETURN( TRUE );
}

SKILL( cast_detect_evil )
{
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA   af;

  PUSH_FUNCTION( "cast_detect_evil" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_detect_evil: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( is_affected( victim, sn ) )
  {
    send_to_char( "�o�ݰ_�Ӧ��G�S������ĪG�T\n\r", ch );
    RETURN( FALSE );
  }

  af.type      = sn;
  af.duration  = number_fuzzy( level / 3 );
  af.modifier  = 0;
  af.location  = APPLY_NONE;
  af.level     = level;
  affect_to_char( victim, &af );
  send_to_char( "�A�������o�X�F�@�}���������~�C\n\r", victim );
  if ( ch != victim ) send_to_char( "�n�F�C\n\r", ch );
  RETURN( TRUE );
}

SKILL( cast_detect_hidden )
{
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA   af;

  PUSH_FUNCTION( "cast_detect_hidden" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_detect_hidden: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( is_affected( victim, sn ) )
  {
    send_to_char( "�A�������w�g���U�Q�T\n\r", victim );
    RETURN( FALSE );
  }

  af.type      = sn;
  af.duration  = level;
  af.location  = APPLY_NONE;
  af.modifier  = 0;
  af.level     = level;
  affect_to_char( victim, &af );
  send_to_char( "�Aı�o�|�P���ݱo�ܲM���C\n\r", victim );
  if ( ch != victim ) send_to_char( "�n�F�C\n\r", ch );
  RETURN( TRUE );
}

SKILL( cast_detect_invis )
{
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA   af;

  PUSH_FUNCTION( "cast_detect_invis" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_detect_invis: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( is_affected( victim, sn ) )
  {
    send_to_char( "�A�������w�g���R�Q�F�T\n\r", ch );
    RETURN( FALSE );
  }

  af.type      = sn;
  af.duration  = number_range( level / 5, level / 3 );
  af.modifier  = 0;
  af.location  = APPLY_NONE;
  af.level     = level;
  affect_to_char( victim, &af );

  send_to_char( "�A���G�i�H�ݨ�@�Ǥ��P���F��C\n\r", victim );
  if ( ch != victim ) send_to_char( "�n�F�T\n\r", ch );
  RETURN( TRUE );
}

SKILL( cast_detect_magic )
{
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA  af;

  PUSH_FUNCTION( "cast_detect_magic" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_detect_magic: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( is_affected( victim, sn ) )
  {
    send_to_char( "�o���G�_���F����@�ΡT\n\r", victim );
    RETURN( FALSE );
  }

  af.type      = sn;
  af.duration  = level;
  af.modifier  = 0;
  af.location  = APPLY_NONE;
  af.level     = level;
  affect_to_char( victim, &af );
  send_to_char( "�A��������U�Q�F�C\n\r", victim );
  if ( ch != victim ) send_to_char( "�n�F�C\n\r", ch );
  RETURN( TRUE );
}

SKILL( cast_detect_mask )
{
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA  af;

  PUSH_FUNCTION( "cast_detect_mask" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_detect_mask: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( is_affected( victim, sn ) )
  {
    send_to_char( "�o���G�_���F����@�ΡT\n\r", victim );
    RETURN( FALSE );
  }

  af.type      = sn;
  af.duration  = UMAX( 1, level / 5 );
  af.modifier  = 0;
  af.location  = APPLY_NONE;
  af.level     = level;
  affect_to_char( victim, &af );

  send_to_char( "�A�������i�H�ݨ���e���H�F�C\n\r", victim );
  if ( ch != victim ) send_to_char( "�n�F�C\n\r", ch );

  RETURN( TRUE );
}

SKILL( cast_detect_poison )
{
  OBJ_DATA * obj = ( OBJ_DATA * ) vo;

  PUSH_FUNCTION( "cast_detect_poison" );

  if ( !ch || !obj )
  {
    mudlog( LOG_DEBUG, "cast_detect_poison: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( obj->item_type == ITEM_DRINK_CON || obj->item_type == ITEM_FOOD )
  {
    if ( obj->value[3] == 0 ) act( "$p�ݨӤ����Y�C", ch, obj, NULL, TO_CHAR );
    else  act( "$p�n�����@�I�����C", ch, obj, NULL, TO_CHAR );

    RETURN( TRUE );
  }
  else
  {
    act( "$p�ݰ_�ӨèS���r�a�T", ch, obj, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  RETURN( FALSE );
}

SKILL( cast_dispel_magic )
{
  AFFECT_DATA * paf;
  AFFECT_DATA * paf_next;
  SKILL_DATA  * pSkill;
  int           chance;
  int           lev;

  PUSH_FUNCTION( "cast_dispel_magic" );

  if ( !( paf = ch->affected ) )
  {
    send_to_char( "�藍�_�M�A���W�S�����󪺪k�N�v�T�T\n\r", ch );
    RETURN( FALSE );
  }

  for ( paf = ch->affected; paf; paf = paf_next )
  {
    paf_next = paf->next;
    chance   = 0;
    lev      = UMAX( 1, UMIN( MAX_LEVEL, paf->level ) );

    if ( !( pSkill = get_skill( paf->type ) ) ) continue;

    switch( pSkill->type )
    {
    case TAR_CHAR_SELF:
    case TAR_CHAR_DEFENSIVE:
      chance = 50 + ( level - paf->level ) * 2;
      break;

    default:
    case TAR_CHAR_OFFENSIVE:
      chance = 20 + ( level - paf->level ) * 3;
      break;
    }

    if ( number_range( 1, 100) <= chance )
    {
      act( "�v�T�A���骺$3$w$0�w�g�Q�����F�T", ch, pSkill, NULL, TO_CHAR );
      affect_remove( ch, paf );
    }

    else
    {
      act( "�A���W��$3$w$0�|���Q�����T", ch, pSkill, NULL, TO_CHAR );
    }
  }

  RETURN( TRUE );
}

SKILL( cast_dispel_evil )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         dam;

  PUSH_FUNCTION( "cast_dispel_evil" );

  if ( !IS_NPC( ch ) && IS_EVIL( ch ) ) victim = ch;

  if ( IS_GOOD( victim ) )
  {
    act( "$N����ѯ����O�@�M�A�ˤ���L�C", ch, NULL, victim, TO_ROOM );
    RETURN( TRUE );
  }

  if ( IS_NEUTRAL( victim ) )
  {
    act( "$N�ݨӨS����v�T�C", ch, NULL, victim, TO_CHAR );
    RETURN( TRUE );
  }

  dam = spell_damage( ch, level, TYPE_WIS, SPELL_CLASS_ONE );
  if ( saves_spell( level, victim ) ) dam /= 2;
  damage( ch, victim, dam, sn, ATTACK_RANDOM );
  RETURN( TRUE );
}

SKILL( cast_earthquake )
{
  CHAR_DATA * vch;
  CHAR_DATA * vch_next;
  int         dam;

  PUSH_FUNCTION( "cast_earthquake" );

  act( "$r����a��j�P�a�_�ʵۡT", ch, NULL, NULL, TO_ALL );

  /* �]�w��� */
  dam = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_TWO );

  for ( vch = char_list; vch; vch = vch_next )
  {
    vch_next = vch->next;

    if ( !verify_char( vch ) || !vch->in_room ) continue;

    if ( vch->in_room == ch->in_room )
    {
      if ( vch != ch && ( IS_NPC( ch ) ? !IS_NPC( vch ) : IS_NPC( vch ) ) )
      {
        if ( !is_affected( vch, SLOT_FIXITY ) )
          damage( ch, vch, dam, sn, ATTACK_RANDOM );
      }

      continue;
    }

    if ( vch->in_room->area == ch->in_room->area )
      send_to_char( "�j�a�����a�n�ʵۡT\n\r", vch );
  }

  RETURN( TRUE );
}

SKILL( cast_enchant_weapon )
{
  OBJ_DATA    * obj = ( OBJ_DATA * ) vo;
  AFFECT_DATA * paf;

  PUSH_FUNCTION( "cast_enchant_weapon" );

  if ( !obj )
  {
    mudlog( LOG_DEBUG, "cast_enchant_weapon: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( obj->item_type != ITEM_WEAPON || obj->Magic || obj->affected )
  {
    act( "$p�w�g����A�[�j�F�T", ch, obj, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  if ( obj->level > ch->level )
  {
    act( "$p���O$n�ү��H�N���!", ch, obj, NULL, TO_CHAR );
    RETURN( FALSE );
  }

   /* �t�m�O���� */
  paf            = alloc_struct( STRUCT_AFFECT_DATA );
  paf->type      = sn;
  paf->duration  = -1;
  paf->location  = APPLY_HITROLL;
  paf->modifier  = level / 5;
  paf->level     = level;
  paf->next      = obj->affected;
  obj->affected  = paf;

  /* �t�m�O���� */
  paf            = alloc_struct( STRUCT_AFFECT_DATA );
  paf->type      = -1;
  paf->duration  = -1;
  paf->location  = APPLY_DAMROLL;
  paf->modifier  = level / 10;
  paf->level     = level;
  paf->next      = obj->affected;
  obj->affected  = paf;
  obj->level     = ch->level;

  if ( IS_GOOD( ch ) )
  {
    obj->AntiEvil = TRUE;
    act( "$p�o�X�@���H�H��$5�ť�$0�C", ch, obj, NULL, TO_CHAR );
  }

  else if ( IS_EVIL( ch ) )
  {
    obj->AntiGood = TRUE;
    act( "$p�o�X�@���H�H��$1����$0�C", ch, obj, NULL, TO_CHAR );
  }
  else
  {
    obj->AntiEvil = TRUE;
    obj->AntiGood = TRUE;
    act( "$p�o�X�@���H�H��$3����$0�C", ch, obj, NULL, TO_CHAR );
  }

  send_to_char( "�n�F�C\n\r", ch );
  RETURN( TRUE );
}


SKILL( cast_fireball )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         dam;
  int         choice[] =
              { 35, 55, 75, 90, 95, 100, -1 };

  PUSH_FUNCTION( "cast_fireball" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_fireball: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  /* �]�w��� */
  dam = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_THR );

  switch( dimemsion_position( choice, level, TRUE ) )
  {
  case ERRORCODE:
    mudlog( LOG_DEBUG, "�ޯ� %d ���T���]�w�����D." , sn );
    RETURN( FALSE );

  case 0:
    sact( ch, victim, NULL,
      "$n�N��W�����������M�Φ��@�ө����j�p�����y�M�H�����y��V$N�T" );
    break;

  case 1:
    sact( ch, victim, NULL,
      "$n�E���F�j�۵M�@�Ǵ��b�Ť������q�M�����b��x�B�T$A"
      "�Z�a�M$e�N�o�@�Ǥp���y��V$N�M�@�D�D�����V$E�t�h�T" );
    break;

  case 2:
    sact( ch, victim, NULL,
      "$n�̮𾮯��M���C�ӵ�M���ۤ@���ڴx��j�p�����y�X�{�b$e���e�T$A"
      "$n�@�ϤO�M�o�@���`�������y�K�V$N��Ħӥh�M�����H�T" );
    break;

  case 3:
    sact( ch, victim, NULL,
      "�u��$n�@�ӵo��M�\\�h���p�۶���j�����y�X�{�b$e�|�P������¶�ۡT$A"
      "���۳o�@�������y�|���K�誺���V$N�M�ᬰ���[�T" );
    break;

  case 4:
    sact( ch, victim, NULL,
      "$n�᧹�G�y�M�@�����j�����y���O$Y�����$Z�@��X�{�b$e�W�šM���R���סC$A"
      "$n�V��$N�@���M���y�@��K�����ͩR�@�˪���������$E�M��$E�׵L�i�סC" );
    break;

  case 5:
    sact( ch, victim, NULL,
      "$n�b�|�P���U�@�ӵ��ɡM�u���|�P������}�l�ܰʡM���G�Ө�F�@��"
      "���@�˪��a��M�o�̪L��C�A�M���ϸ͸͡T$A"
      "�u��$n�B�X���@�˪��L���M�@�ػ��������M�@�}�H�q�T$A"
      "$R                     �� �� �� �y   $Z               $A"
      "�@�ӷ��j�����Ť��y���۫l���V��$N�g�t�ӥh�T�T" );
    dam = number_range( dam, UMIN( LEVEL_TWO_DAM, dam * 2 ) );
    break;
  }

  damage( ch, victim, dam, sn, ATTACK_RANDOM );
  RETURN( TRUE );
}

#define MAX_MULTIPLE               5
SKILL( cast_flamestrike )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  CHAR_DATA * vic;
  CHAR_DATA * vic_next;
  int         dam;
  int         loop;

  PUSH_FUNCTION( "cast_flamestrike" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_flamestrike: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( !ch->in_room || !victim->in_room ) RETURN( FALSE );

  sact( ch, victim, NULL, "$R$n$R�q��G�y�M�಴���Aı�o�|�P��"
    "�G�Q���N�`�L�@�ˡM�o�󤣥͡T$Z$A" );

  /* �]�w��� */
  dam = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_TWO );

  if ( !IS_NPC( victim ) )
  {
    sact( ch, victim, NULL, "$R$n$R�ⶶ�դ@���M�@�ѷ�������y"
      "���a���򪺨g���V��$N$R�G��C$Z" );

    damage( ch, victim, UMIN( LEVEL_TWO_DAM, dam * 2 ), sn, ATTACK_RANDOM );
    RETURN( TRUE );
  }

  for ( loop = 0, vic = ch->in_room->people; vic; vic = vic_next )
  {
    vic_next = vic->next_in_room;

    /* �Ǫ������Ǫ� */
    if ( IS_NPC( ch ) && IS_NPC( vic ) ) continue;

    if ( check_can_kill( ch, vic ) == TRUE )
    {
      if ( loop > 3 ) sact( ch, vic, NULL,
        "�|�P�@�������M$n��$R���K$Z�������N�`��$N���ͩR�T" );

      else sact( ch, vic, NULL,
        "$n�a�_�@��$R�P�K$Z�N$N�����a�]���M�n�N$E�m�󦺦a�T" );

      dam = UMAX( 0, number_range( dam
        , dam + ( UMIN( MAX_MULTIPLE, loop++ ) * get_curr_int( ch ) ) ) );

      damage( ch, vic, UMIN( LEVEL_TWO_DAM, dam ), sn, ATTACK_RANDOM );
    }

    continue;
  }

  if ( loop > 6 && victim && can_damage( victim ) )
  {
    sact( ch, victim, NULL, "$n�����K�N�`�F�@���M�@�Ƕüu��"
      "$R�`�����K$Z���p�ߤS�N�ˤF$N�C" );
    damage( ch, victim, dam, sn, ATTACK_RANDOM );
  }

  RETURN( TRUE );
}
#undef  MAX_MULTIPLE

SKILL( cast_faerie_fire )
{
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA   af;

  PUSH_FUNCTION( "cast_faerie_fire" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_faerie_fire: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( is_affected( victim, sn ) ) RETURN( FALSE );

  af.type      = sn;
  af.duration  = level;
  af.location  = APPLY_AC;
  af.modifier  = number_range( level, level * 3 );
  af.level     = level;
  affect_to_char( victim, &af );
  send_to_char( "�A�Q�@�ί����������]��M�N���@�������\\�@�ˡC\n\r", victim );
  act( "$n�Q�@�ί��������]��ۡC", victim, NULL, NULL, TO_ROOM );
  RETURN( TRUE );
}

SKILL( cast_faerie_fog )
{
  CHAR_DATA * ich;

  PUSH_FUNCTION( "cast_faerie_fog" );

  if ( !ch->in_room )
  {
    mudlog( LOG_DEBUG, "cast_farie_fog: �ӷ������T." );
    RETURN( FALSE );
  }

  act( "$n���@������@�˦R�X�@�ε��⪺�����C", ch, NULL, NULL, TO_ROOM );
  send_to_char( "�A�R�X�@�ε��⪺�����C\n\r", ch );

  for ( ich = ch->in_room->people; ich; ich = ich->next_in_room )
  {
    if ( !IS_NPC( ich) && IS_SET( ich->act, PLR_WIZINVIS ) ) continue;
    if ( ich == ch || saves_spell( level, ich ) ) continue;

    affect_release( ich, SLOT_INVIS      );
    affect_release( ich, SLOT_MASS_INVIS );
    affect_release( ich, SLOT_SNEAK      );
    affect_release( ich, SLOT_MASK       );

    act( "$n�����κC�C�a�X�{�F�T", ich, NULL, NULL, TO_ROOM );
    send_to_char( "�A�{�ΤF�T\n\r", ich );
  }

  RETURN( TRUE );
}

SKILL( cast_fly )
{
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA   af;

  PUSH_FUNCTION( "cast_fly" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_fly: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( victim->mount )
  {
    act( "�]���A�M��$N�M�ҥH�L�k�I�i�o�تk�N�T"
      , victim, NULL, victim->mount, TO_CHAR );

    RETURN( FALSE );
  }

  if ( victim->mount_by )
  {
    act( "�]���A���Q$N�M�ۡM�ҥH�L�k�I�i�o�تk�N�T"
      , victim, NULL, victim->mount_by , TO_CHAR );

    RETURN( FALSE );
  }

  if ( is_affected( victim, sn ) )
  {
    act( "$n���F�b�ѡM�s�I���R���S�o�͡C", ch, NULL, NULL, TO_ALL );
    RETURN( FALSE );
  }

  af.type      = sn;
  af.duration  = level + 3;
  af.location  = 0;
  af.modifier  = 0;
  af.level     = level;
  affect_to_char( victim, &af );

  act( "�u��$n�ϥX�D�a�P�N���u�˶����P�v�M�⤤�عФ@�ϡM�@���ն�"
    "���a�@�_�M�w�w���N$n���F�_�ӡC", ch, NULL, NULL, TO_ALL );

  RETURN( TRUE );
}

SKILL( cast_gate )
{
  CHAR_DATA * mob;
  CHAR_DATA * ghost;

  PUSH_FUNCTION( "cast_gate" );

  if ( !ch || !ch->in_room )
  {
    mudlog( LOG_DEBUG, "cast_gate: �ʥF�ӷ��C" );
    RETURN( FALSE );
  }

  if ( is_pk( ch ) && fight_info->status != PK_FIGHT )
  {
    send_to_char( "�b�o�q�ɶ��M�٬O�n�n�a�𮧤@�U�a�T\n\r", ch );
    RETURN( FALSE );
  }

  for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
    if ( mob->pIndexData && mob->pIndexData == MobVampire ) break;

  if ( mob )
  {
    act( "$r�����B���O$N�M�n�i�ȳ�T", ch, NULL, mob, TO_CHAR );
    RETURN( FALSE );
  }

  if ( !( ghost = create_mobile( MobVampire, -1 ) ) )
  {
    mudlog( LOG_DEBUG, "cast_gate: �L�k�s�y�k��: %d.", MobVampireVnum );
    RETURN( FALSE );
  }

  act( "$2$n�C�n�᧹�G�y�M�಴�����e�g�a���}�M�Ѹ��Y�n�X�@��$N�C$0"
    , ch, NULL, ghost, TO_ALL );

  char_to_room( ghost, ch->in_room );

  if ( ch->fighting )
  {
    sact( ghost, ch->fighting, NULL,
      "$n�@����$N�ϩ����줳�H�@�ˡM�ߧY��$N�i�}�����T" );

    set_fighting( ghost, ch->fighting );
  }

  RETURN( TRUE );
}

SKILL( cast_general_purpose )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         dam;
  int         guess;

  PUSH_FUNCTION( "cast_general_purpose" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_general_purpose: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  guess = number_range( 1, 5 );
  dam = spell_damage( ch, level, guess, SPELL_CLASS_ONE );
  if ( saves_spell( level, victim ) ) dam /= 2;
  damage( ch, victim, dam, sn, ATTACK_RANDOM );
  RETURN( TRUE );
}

SKILL( cast_giant_strength )
{
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA   af;

  PUSH_FUNCTION( "cast_gaint_strength" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_gaint_strength: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( is_affected( victim, sn ) )
  {
    act( "$N�w�g�D�`���j���F�T", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  af.type      = sn;
  af.duration  = number_range( level / 6, level / 4 );
  af.location  = APPLY_STR;
  af.modifier  = 1 + (level >= 18) + (level >= 25);
  af.level     = level;
  affect_to_char( victim, &af );

  send_to_char( "�������Ӫ��O�q�ǹM�A����M�A�P��i�H�|�_�@�Y���C\n\r"
    , victim );
  if ( ch != victim ) send_to_char( "�n�F�C\n\r", ch );
  RETURN( TRUE );
}

SKILL( cast_harm )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         dam;

  PUSH_FUNCTION( "cast_harm" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_harm: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  dam = spell_damage( ch, level, TYPE_WIS, SPELL_CLASS_FOU );
  dam = number_range( dam / 2, dam );
  damage( ch, victim, dam, sn, ATTACK_RANDOM );
  RETURN( TRUE );
}

SKILL( cast_heal )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         heal;

  PUSH_FUNCTION( "cast_heal" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_heal: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( !can_damage( victim ) )
  {
    send_to_char( "�A�O�Ϥ����@�Ӧ��H���T\n\r", victim );
    RETURN( FALSE );
  }

  if ( victim->hit >= get_curr_hit( victim ) )
  {
    act( "$N�w�g�S���ˤf�F�T", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  heal = ( number_range( 100, 150 ) + level ) * get_curr_int( ch ) / 4;
  victim->hit = UMIN( victim->hit + heal , get_curr_hit( victim ) );

  if ( victim->position == POS_SLEEPING && victim != ch )
    act( "$N����v�����ʤF�A�T", victim, NULL, ch, TO_CHAR );

  update_pos( victim );

  send_to_char( "�ѤW���U�@�D�p�q�M���b�A�����W�C\n\r"
    "�A�P��@�}�x�y�y�M�����M�ŷŪ��n�ΪA�T\n\r", victim );

  if ( ch != victim ) send_to_char( "��v�����C\n\r", ch );
  RETURN( TRUE );
}

SKILL( cast_high_explosive )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         dam;

  PUSH_FUNCTION( "cast_high_explosive" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_high_explosive: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  dam = spell_damage( ch, level, TYPE_WIS, SPELL_CLASS_THR );
  if ( saves_spell( level, victim ) ) dam /= 2;
  damage( ch, victim, dam, sn, ATTACK_RANDOM );
  RETURN( TRUE );
}

SKILL( cast_judge )
{
  char             buf1[MAX_STRING_LENGTH];
  SKILL_DATA     * pSkill;
  LIMIT_DATA     * pLimit;
  char           * pString = ( char * ) vo;
  RESTRICT_DATA * pRestrict;
  int             count;
  char            chinese[MAX_INPUT_LENGTH];

  PUSH_FUNCTION( "cast_judge" );

  if ( !( pSkill = skill_isname( pString ) ) )
  {
    send_to_char( "�o�اޯ�n�����ǫܤ[�o�T\n\r", ch );
    RETURN( FALSE );
  }

  if ( pSkill->canask == FALSE )
  {
    send_to_char( "�A�Y�����ȡM���򳣷P������T\n\r", ch );
    RETURN( FALSE );
  }

  clear_buffer();
  send_to_buffer( "\e[1;36m�A���F�U�ӡM�����������A�����ɡC\e[0m\n\r\n\r" );
  send_to_buffer( "�AŲ��%s�ޯ઺���G�O�R\n\r", pSkill->cname );
  send_to_buffer( "�o�O�@��%s����"
    , pSkill->rating ? skill_rating( pSkill->rating ) : "����" );
  if ( pSkill->type        == TAR_DODGE ) send_to_buffer( " ���m��" );
  else if ( pSkill->type   == TAR_FLEE )  send_to_buffer( " �k�]��" );
  else send_to_buffer( "%s���ޯ�M�D�n�O�Ψ�"
    , weapon_type_name( pSkill->weapon ) );

  switch( pSkill->type )
  {
  default                 : send_to_buffer( "�����C\n\r"     ); break;
  case TAR_DODGE          : send_to_buffer( "�����{���C\n\r" ); break;
  case TAR_IGNORE         : send_to_buffer( "�S���ؼСC\n\r" ); break;
  case TAR_CHAR_OFFENSIVE : send_to_buffer( "�������C\n\r" ); break;
  case TAR_CHAR_DEFENSIVE : send_to_buffer( "�������m�C\n\r" ); break;
  case TAR_CHAR_SELF      : send_to_buffer( "�@�Υ����C\n\r" ); break;
  case TAR_OBJ_INV        : send_to_buffer( "�@�Ϊ���C\n\r" ); break;
  case TAR_MEDITATION     : send_to_buffer( "�߷Q�C\n\r"     ); break;
  case TAR_OBJ_ROOM       : send_to_buffer( "�ж����~�C\n\r" ); break;
  }

  if ( level < 20 )
  {
    print_buffer( ch );
    RETURN( TRUE );
  }

  send_to_buffer( "�I�i�D�n���O" );

  switch( pSkill->cost_type )
  {
  default        : send_to_buffer( "����"   ); break;
  case COST_HIT  : send_to_buffer( "�ͩR�O" ); break;
  case COST_MANA : send_to_buffer( "�k�O"   ); break;
  case COST_MOVE : send_to_buffer( "���ʤO" ); break;
  case COST_GOLD : send_to_buffer( "����"   ); break;
  }

  send_to_buffer( "���ƭȬݨ�" );

  if ( pSkill->cost <= 20 )      send_to_buffer( "�ܤ֡C\n\r" );
  else if ( pSkill->cost < 100 ) send_to_buffer( "�@�ǡC\n\r" );
  else if ( pSkill->cost < 300 ) send_to_buffer( "���֡C\n\r" );
  else                           send_to_buffer( "�ܦh�C\n\r" );

  if ( level < 40 )
  {
    print_buffer( ch );
    RETURN( TRUE );
  }

  if ( pSkill->limit )
    send_to_buffer( "\n\r�̷P���Ӫ��Pı�M�o���ޯ঳�ܦh������R\n\r" );

  for ( pLimit = pSkill->limit; pLimit; pLimit = pLimit->next )
  {
    if ( pLimit->class )
    {
      chinese_number( pLimit->level, buf1 );
      send_to_buffer( "�Y�O%s�h���ų̧C�n%s�Ť~��ǡM���m�׳̰����%s"
      , class_name( pLimit->class, FALSE )
      , buf1
      , adeptation_name( ( pLimit->adept ) ) );

      if ( SkillValue || IS_IMMORTAL( ch ) )
        send_to_buffer( "  %3d%%", pLimit->adept );

      send_to_buffer( "�C\n\r" );
    }
  }

  if ( level < 60 )
  {
    print_buffer( ch );
    RETURN( TRUE );
  }

  for ( count = 0, pRestrict = pSkill->restrict;
    pRestrict;
    pRestrict = pRestrict->next, count++ )
  {
    chinese_number( count + 1, chinese );
    send_to_buffer( "%s�B���ޯ�%s\n\r"
      , chinese, restrict_value( pRestrict, ch ) );
  }

  if ( count == 0 ) send_to_buffer( "���ޯ�S�����󪺭���C\n\r" );
  print_buffer( ch );
  RETURN( TRUE );
}

SKILL( cast_pry )
{
  SKILL_DATA     * pSkill;
  TEACH_DATA     * teach;
  MOB_INDEX_DATA * pMobIndex;
  CHAR_DATA      * victim;
  char           * pString = ( char * ) vo;
  int              count;
  int              choice;
  char           * message[] =
    {
      "��L��",
      "�ӥB�L�Q���ַN",
      "�ڷQ�L�|�ַN",
      "�L�i��|",
      "�L���ӬO�|",
      "���O�L���ӷQ",
      "���O�L���@�N",
      "���O�L�Q�����@�N",
      "�A�i��n�ոչB��ݥL�O�_�|"
    };

  PUSH_FUNCTION( "cast_pry" );

  if ( !pString || *pString == '\x0' )
  {
    send_to_char( "�A�n��ť���اޯ�O�S\n\r", ch );
    RETURN( FALSE );
  }

  if ( !( pSkill = skill_isname( pString ) ) )
  {
    send_to_char( "�o�اޯ�n�����ǫܤ[�o�T\n\r", ch );
    RETURN( FALSE );
  }

  choice = sizeof( message ) / sizeof( message[0] );
  clear_buffer();

  for ( count = 0, victim = char_list; victim; victim = victim->next )
  {
    if ( !verify_char( victim )
      || !IS_NPC( victim )
      || !( pMobIndex = victim->pIndexData )
      || !victim->in_room
      || !victim->in_room->area ) continue;

    for ( teach = pMobIndex->teach; teach; teach = teach->next )
    {
      if ( pSkill->slot == teach->slot
        && number_range( 1, 100 + level * 2 ) > teach->inventory )
      {
        int sel;

        count++;

        sel = UMAX( 0, UMIN( choice - 1, teach->inventory * choice / 100 ) );
        send_to_buffer( "�A��ť��%s��%s���о�%s�M%s�оɧA�C\n\r"
          , victim->in_room->area->name, mob_name( ch, victim )
          , pSkill->cname, message[sel] );
      }
    }
  }

  if ( count == 0 ) send_to_buffer( "�o�اޯ�n�����ǫܤ[�o�T\n\r" );
  print_buffer( ch );
  RETURN( TRUE );
}

SKILL( cast_identify )
{
  OBJ_DATA * obj = ( OBJ_DATA * ) vo;

  PUSH_FUNCTION( "cast_identify" );

  if ( !obj )
  {
    mudlog( LOG_DEBUG, "cast_identify: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  identify_obj( ch, obj );
  RETURN( TRUE );
}

SKILL( cast_infravision )
{
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA   af;

  PUSH_FUNCTION( "cast_infravision" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_infravision: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( is_affected( victim, sn ) ) RETURN( FALSE );

  act( "$n�P���G�m�ˡM�����������F�_�ӡC\n\r", ch, NULL, NULL, TO_ROOM );
  af.type      = sn;
  af.duration  = number_range( level / 4 , level / 2 );
  af.location  = APPLY_NONE;
  af.modifier  = 0;
  af.level     = level;
  affect_to_char( victim, &af );
  send_to_char( "�A�P���G�m�ˡM�����������F�_�ӡC\n\r", victim );
  if ( ch != victim ) send_to_char( "�n�F�C\n\r", ch );
  RETURN( TRUE );
}

SKILL( cast_invis )
{
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA   af;

  PUSH_FUNCTION( "cast_invis" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_invis: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( is_affected( victim, SLOT_INVIS )
    || is_affected( victim, SLOT_MASS_INVIS ) )
  {
    act( "$N�����v�w�g�����F�T", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  if ( ( IS_NPC( victim ) && IS_SET( victim->act, ACT_TRAIN ) )
    || ( IS_NPC( victim ) && IS_SET( victim->act, ACT_REBIRTH ) ) )
  {
    act( "�A�S����k��$N�I���Ϊk�N�C", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  if ( victim->pIndexData && victim->pIndexData->pShop )
  {
    act( "$N�O�������ΰ_�Ӫ���T", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  act( "$n�����v�H�X�F�@�Ӷ¼v�M�������i�¼v�����F�C"
    , victim, NULL, NULL, TO_ROOM );
  af.type      = sn;
  af.duration  = number_range( level / 5, level / 3 );
  af.location  = APPLY_NONE;
  af.modifier  = 0;
  af.level     = level;
  affect_to_char( victim, &af );
  send_to_char( "�A�����v�H�X�F�@�Ӷ¼v�M�������i�¼v�����F�C\n\r", victim );
  if ( ch != victim ) send_to_char( "�n�F�C\n\r", ch );
  RETURN( TRUE );
}

SKILL( cast_know_alignment )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  char      * msg;
  int         ap;

  PUSH_FUNCTION( "cast_know_alignment" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_know_alignment: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  ap = victim->alignment;

       if ( ap >  999 ) msg = "$N�X�G�O�t�z�L�ĥ@���R�E�¼䪺�աC";
  else if ( ap >  700 ) msg = "$N���G�s�ѯ����۹Ĥ��p�L���¼�C";
  else if ( ap >  350 ) msg = "$N���@�ӤѨϯ몺�t��C";
  else if ( ap >  100 ) msg = "$N�ݨ�p�ʪ����|�����S�O�����U��C";
  else if ( ap > -100 ) msg = "$N�Φײ���b�ݤ��X�����򤣤@�˪��C";
  else if ( ap > -350 ) msg = "$N�ݨ�ʪ��N�|�Q�n���n�Y�C";
  else if ( ap > -700 ) msg = "$N�N�C�y���ۧA���D�R�u�ӳ�D�a�T�v";
  else if ( ap > -999 ) msg = "$N���o�X�@�}�¦⪺���~�M�n�ݷ��F�T";
  else msg = "$N�O�l�]�۱��B�N���m�T�B�L�Ҥ�������T";

  act( smash_act_keyword( msg, "nesNES" ), ch, NULL, victim, TO_CHAR );
  RETURN( TRUE );
}

SKILL( cast_lightning_bolt )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         dam;

  PUSH_FUNCTION( "cast_lightning_bolt" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_lightning_blot: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  /* �]�w��� */
  dam = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_TWO );

  act( "$n�ۤ⤤�E�g�X�@�D�{�q�M�������g�V$N�C", ch, NULL, victim, TO_ALL );

  if ( saves_spell( level, victim ) ) dam /= 2;
  damage( ch, victim, dam, sn, ATTACK_RANDOM );
  RETURN( TRUE );
}

SKILL( cast_magic_missile )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         dam;

  PUSH_FUNCTION( "cast_magic_missile" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_magic_missile: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  /* ��˱ԭz */
  act( "$n$2��������M���E�k�O�����@�ӭ��u�M���a�V��$N���h�T$0"
    , ch, NULL, victim, TO_ALL );

  /* �]�w��� */
  dam = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_ONE );

  if ( saves_spell( level, victim ) ) dam /= 2;
  damage( ch, victim, dam, sn, ATTACK_RANDOM );
  RETURN( TRUE );
}

SKILL( cast_wind_blade )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         dam;

  PUSH_FUNCTION( "cast_wind_blade" );

  if ( !ch || !verify_char( ch ) || !victim || !verify_char( victim ) )
  {
    mudlog( LOG_DEBUG, "cast_wind_blade: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  /* ��˱ԭz */
  act( "$n$7�N�k������M���W�@���M�j�a��_�@�}�P���M�S�p$2�Q�b$7��"
       "�a���V$N�M$N���W�ߧY�h�F�ƹD�˲��T$0", ch, NULL, victim, TO_ALL );

  /* �]�w��� */
  dam = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_TWO );

  if ( saves_spell( level, victim ) ) dam /= 2;
  damage( ch, victim, dam, sn, ATTACK_RANDOM );

  RETURN( TRUE );
}

#define MAX_MULTIPLE               5

SKILL( cast_vacuum_wind )
{
  CHAR_DATA * victim;
  CHAR_DATA * vic_next;
  int         dam;
  int         loop;

  PUSH_FUNCTION( "cast_vacuum_wind " );

  if ( !ch || !verify_char( ch ) || !ch->in_room )
  {
    mudlog( LOG_DEBUG, "cast_wind_blade: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( level > 45 )
  {
    /* �M���� */
    for ( loop = 0, victim = ch->in_room->people; victim; victim = vic_next )
    {
      vic_next = victim->next_in_room;

      /* �Ǫ������Ǫ� */
      if ( IS_NPC( ch ) && IS_NPC( victim ) ) continue;

      if ( !check_can_kill( ch, victim ) ) continue;
      if ( loop > MAX_MULTIPLE )           break;

      /* ��˱ԭz */
      sact( ch, victim, NULL,
        "$n$W�����J�w�M�f��[$R�g���G$W]�M�b$N���P��j"
        "�_�ư}�j���M�S���W�ߪ��Iŧ$N�M��j�P�S�g�ê����ձN$N�����a"
        "���L���a�T$Z" );

      dam = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_THR );
      if ( saves_spell( level, victim ) ) dam /= 2;
      dam = number_range( dam * 3 / 4, dam );

      loop++;
      damage( ch, victim, dam, sn, ATTACK_RANDOM );
    }

    RETURN( TRUE );
  }
  else
  {
    victim = ( CHAR_DATA * ) vo;

    /* ��˱ԭz */
    sact( ch, victim, NULL,
      "$n$W�f��$R[�g���G]$W�M���ۦb$N�P��y���u�ů몺����"
      "�M�u����$N������T$Z" );

    dam = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_THR );
    if ( saves_spell( level, victim ) ) dam /= 2;
    dam = number_range( dam * 3 / 4, dam );

    damage( ch, victim, dam, sn, ATTACK_RANDOM );
    RETURN( TRUE );
  }

  RETURN( TRUE );
}

SKILL( cast_mass_invis )
{
  AFFECT_DATA   af;
  CHAR_DATA   * gch;

  PUSH_FUNCTION( "cast_mass_invis" );

  if ( !ch->in_room )
  {
    mudlog( LOG_DEBUG, "cast_mass_invis: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
  {
    if ( !is_same_group( gch, ch )
      || is_affected( gch, SLOT_INVIS )
      || is_affected( gch, SLOT_MASS_INVIS ) )
      continue;

    act( "$n�����v�H�X�F�@�Ӷ¼v�M�������i�¼v�����F�C"
      , gch, NULL, NULL, TO_ROOM );

    send_to_char( "�A�����v�H�X�F�@�Ӷ¼v�M�������i�¼v�����F�C\n\r", gch );

    af.type      = sn;
    af.duration  = number_range( level / 18, level / 12 );
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.level     = level;
    affect_to_char( gch, &af );
  }

  send_to_char( "�n�F�C\n\r", ch );
  RETURN( TRUE );
}

SKILL( cast_null )
{
  PUSH_FUNCTION( "cast_null" );
  send_to_char( "�藍�_�M�o���O�@���k�N�T\n\r", ch );
  RETURN( FALSE );
}

SKILL( cast_pass_door )
{
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA   af;

  PUSH_FUNCTION( "cast_pass_door" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_pass_door: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( is_affected( victim, sn ) ) RETURN( FALSE );

  af.type      = sn;
  af.duration  = number_fuzzy( level / 4 );
  af.location  = APPLY_NONE;
  af.modifier  = 0;
  af.level     = level;
  affect_to_char( victim, &af );
  act( "$n��������G�ܪ����@�I�z���C", victim, NULL, NULL, TO_ROOM );
  send_to_char( "�A�P�쨭��_�F�@���ܤơM�ܪ����@�I�z���C\n\r", victim );
  RETURN( TRUE );
}

SKILL( cast_fixity )
{
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA   af;

  PUSH_FUNCTION( "cast_fixity" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_fixity: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( is_affected( victim, sn ) ) RETURN( FALSE );

  af.type      = sn;
  af.duration  = number_fuzzy( level / 4 );
  af.location  = APPLY_NONE;
  af.modifier  = 0;
  af.level     = level;
  affect_to_char( victim, &af );

  act( "$n�I�s���ʩ����M����$n�N�w���ʤF�C", victim, NULL, NULL, TO_ALL );
  RETURN( TRUE );
}

SKILL( cast_poison )
{
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA   af;

  PUSH_FUNCTION( "cast_poison" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_poison: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( saves_spell( level, victim ) || get_curr_str( victim ) <= 2 )
  {
    act( "$N�����A�r�`�N���v�T�C", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  af.type      = sn;
  af.duration  = number_range( 1, level / 5 );
  af.location  = APPLY_STR;
  af.modifier  = -2;
  af.level     = 0;
  affect_join( victim, &af );
  send_to_char( "�A�y�W�@�}�C�@�}�աM�����D�o�ͤF����ơM�o�P������L�C\n\r"
    , victim );
  if ( ch != victim ) send_to_char( "�n�F�C\n\r", ch );
  RETURN( TRUE );
}

SKILL( cast_protection )
{
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA   af;

  PUSH_FUNCTION( "cast_protection" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_protection: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( is_affected( victim, sn ) )
  {
    send_to_char( "�A�w�g�I�i�F�u�F���Z�v�M�p�߸g�߰f��C\n\r", ch );
    RETURN( FALSE );
  }

  af.type      = sn;
  af.duration  = 12 * UMIN( 1, level / 40 );
  af.location  = APPLY_NONE;
  af.modifier  = 0;
  af.level     = 0;
  affect_to_char( victim, &af );

  act( "$n���ؾ����M�ν\\��ߪk�����O�B��F�@���u�F���Z�v�C$A"
       "�@�ѫC��q$n���W���X�M�׻E�b$e�����ߡM�M��w�w�H�h�C"
       , ch, NULL, NULL, TO_ALL );

  RETURN( TRUE );
}

SKILL( cast_refresh )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         refresh;

  PUSH_FUNCTION( "cast_refresh" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_refresh: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( victim->move >= get_curr_move( victim ) )
  {
    act( "$N�w�g�������|�h�¤F�T", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  refresh = level * 3 + ( number_range( 1, get_curr_int( ch ) * 2 ) );
  victim->move = UMIN( victim->move + refresh, get_curr_move( victim ) );

  send_to_char( "�A�P������h�¤F�C\n\r", victim );
  if ( ch != victim ) send_to_char( "�n�F�C\n\r", ch );
  RETURN( TRUE );
}

SKILL( cast_remove_curse )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;

  PUSH_FUNCTION( "cast_remove_curse" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_remove_curse: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( is_affected( victim, SLOT_CURSE ) )
  {
    affect_release( victim, SLOT_CURSE );

    send_to_char( "�A�P��n�h�F�C\n\r", victim );
    if ( ch != victim ) send_to_char( "�n�F�C\n\r", ch );
    RETURN( TRUE );
  }

  RETURN( TRUE );
}

SKILL( cast_sanctuary )
{
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA   af;

  PUSH_FUNCTION( "cast_sanctuary" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_sanctuary: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( is_affected( victim, sn ) )
  {
    act( "$N�w�g�֦��t���F�T", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  af.type      = sn;
  af.duration  = URANGE( 1, number_fuzzy( level / 20 ), 20 );
  af.location  = APPLY_NONE;
  af.modifier  = 0;
  af.level     = 0;
  affect_to_char( victim, &af );
  act( "$n���Ǥɰ_�F�@�}�����M�[�[���h�C", victim, NULL, NULL, TO_ROOM );
  send_to_char( "�A�Q�@�Ω_�Ǫ������ҳ�¶�C\n\r", victim );
  RETURN( TRUE );
}

SKILL( cast_shield )
{
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA   aff;

  PUSH_FUNCTION( "cast_shield" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_shield: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( ch != victim )
  {
    send_to_char( "�藍�_�M�o���k�N�u��I�i�b�ۤv�����W�T\n\r", ch );
    RETURN( FALSE );
  }

  if ( is_affected( victim, sn ) )
  {
    act( "$N�����@�w�g�n���S�ܻ��F�T", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  aff.type      = sn;
  aff.duration  = number_range( level / 5, level / 4 );
  aff.modifier  = -( UMAX( 1, number_range( level , level * 3 ) ) );
  aff.location  = APPLY_AC;
  aff.level      = level;
  affect_to_char( victim, &aff );

  act( "$n�Q�@�ӤO����¶�ۡC", victim, NULL, NULL, TO_ROOM );
  send_to_char( "�A�Q�@�ӤO����¶�ۡM�O�@���@�F�ǳ\\�C\n\r", victim );
  RETURN( TRUE );
}

SKILL( cast_gold_shield )
{
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA   aff;

  PUSH_FUNCTION( "cast_gold_shield" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_gold_shield: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( ch != victim )
  {
    send_to_char( "�藍�_�M�o�@���\\�ҥu��I�i�b�ۤv���W�T\n\r", ch );
    RETURN( FALSE );
  }

  if ( victim->hit * 2 <= get_curr_hit( victim ) )
  {
    act( "�藍�_�M$N���˹L���M�u��ȮɵL�k���E�T", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  if ( is_affected( victim, sn ) )
  {
    act( "$N�w�g�I�i�F�����n�M�u��L�k�A���E�T", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  aff.type      = sn;
  aff.duration  = number_range( 1, 2 );
  aff.modifier  = -( UMAX( 1, number_range( level , level * 3 ) ) );
  aff.location  = APPLY_AC;
  aff.level     = level;
  affect_to_char( victim, &aff );

  act( "$n��E���СM�C���|�誺�u��b���W�Φ��F�@�h�O�@�T", victim, NULL, NULL, TO_ALL );
  RETURN( TRUE );
}

SKILL( cast_iron_shield )
{
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA   aff;

  PUSH_FUNCTION( "cast_iron_shield" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_iron_shield: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( ch != victim )
  {
    send_to_char( "�藍�_�M�o�@���\\�ҥu��I�i�b�ۤv���W�T\n\r", ch );
    RETURN( FALSE );
  }

  if ( victim->hit * 2 <= get_curr_hit( victim ) )
  {
    act( "�藍�_�M$N���˹L���M�u��@�ɵL�k���E�T", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  if ( is_affected( victim, sn ) )
  {
    act( "$N�w�g�I�i�F�K���m�M�u��L�k�A���E�T", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  aff.type      = sn;
  aff.duration  = 1;
  aff.modifier  = -( UMAX( 1, number_range( level / 2, level * 3 ) ) );
  aff.location  = APPLY_AC;
  aff.level     = level;
  affect_to_char( victim, &aff );

  act( "$n�j�n�@�I�M�C���|�誺�u�𦳦p���K��a�O�@�ۡT", victim, NULL, NULL, TO_ALL );
  RETURN( TRUE );
}

SKILL( cast_deva_shield )
{
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA   aff;

  PUSH_FUNCTION( "cast_deva_shield" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_deva_shield: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( ch != victim )
  {
    send_to_char( "�藍�_�M�o�@���\\�ҥu��I�i�b�ۤv���W�T\n\r", ch );
    RETURN( FALSE );
  }

  if ( !IS_GOOD( victim ) )
  {
    send_to_char( "�藍�_�M�A���}�礣���n�M�L�k�I�i�o���k�N�T\n\r", victim );
    RETURN( FALSE );
  }

  if ( is_affected( victim, sn ) )
  {
    act( "�藍�_�M$N�w�g�I�i�F�����@��T", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  aff.type      = sn;
  aff.duration  = UMAX( 1, level / 10 );
  aff.modifier  = -( UMAX( 1, number_range( level * 2 , level * 4 ) ) );
  aff.location  = APPLY_AC;
  aff.level     = level;
  affect_to_char( victim, &aff );

  act( "$n�f�w�򸹡M�|�j����M�n�P���M�k�O�[���T", victim, NULL, NULL, TO_ALL );
  RETURN( TRUE );
}

SKILL( cast_shocking_grasp )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         dam;

  PUSH_FUNCTION( "cast_shocking_grasp" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_shocking_grasp: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  dam = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_ONE );

  act( "$n�o�X�F�@�ӹp�q������M�F�V$N�T", ch, NULL, victim, TO_ALL );

  if ( saves_spell( level, victim ) ) dam /= 2;
  damage( ch, victim, dam, sn, ATTACK_RANDOM );
  RETURN( TRUE );
}

SKILL( cast_sleep )
{
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA   af;

  PUSH_FUNCTION( "cast_sleep" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_sleep: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( ch == victim )
  {
    send_to_char( "�A�����δN�i�H�F�a�M�p�G�u�������M���N�Ҽ{���v�a�T\n\r"
      , ch );
    RETURN( FALSE );
  }

  if ( !can_damage( victim ) )
  {
    act( "�A�ثe�S����k�ʯv$N�T", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  /* �Y��H�O���a�B���O�s��άO���H�ǫh���� */
  if ( is_pk( ch ) && is_pk( victim ) )
  {
    if ( fight_info->status != PK_FIGHT )
    {
      send_to_char( "�b�o�q�ɶ��M�٬O�n�n�a�𮧤@�U�a�T\n\r", ch );
      RETURN( FALSE );
    }
  }

  else if ( !IS_NPC( victim ) )
  {
    if ( !IS_SET( victim->act, PLR_KILLER )
      && !IS_SET( victim->act, PLR_THIEF  )
      && !IS_SET( victim->act, PLR_BOLTER )
      && ( victim->in_room
      && victim->in_room->area
      && !( victim->in_room->area->fight ) ) )
    {
      act( "�A�n����$N�M�Х� $2MURDER$0 �o�ӫ��O�C", ch, NULL, victim, TO_CHAR );
      RETURN( FALSE );
    }
  }

  if ( !can_fight( ch , victim ) ) RETURN( FALSE );

  if ( is_affected( victim, SLOT_SLEEP ) )
  {
    act( "$N�����A�ʯv�N���v�T�T", ch, NULL, victim, TO_CHAR );
    if ( IS_AWAKE( victim ) && !victim->fighting ) set_fighting( victim, ch );
    RETURN( FALSE );
  }

  if ( victim->fighting )
  {
    act( "$N�ԷN�����M�����A���ʯv���v�T�T", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  if ( victim->mount_by )
  {
    act( "�n�ʯv�]�o�ݥD�H$N�֤��֧a�T", ch, NULL, victim->mount_by, TO_CHAR );
    RETURN( FALSE );
  }

  if ( victim->boss )
  {
    act( "�A����ʯv$N���p���T", ch, NULL, victim->boss, TO_CHAR );
    RETURN( FALSE );
  }

  if ( number_range( 1, level ) <= victim->level )
  {
    act( "$N�����믫�M����$n���ʯv�T", ch, NULL, victim, TO_ALL );
    set_fighting( victim, ch );
    RETURN( FALSE );
  }

  af.type      = sn;
  af.duration  = 4 + number_fuzzy( level / 5 );
  af.location  = APPLY_NONE;
  af.modifier  = 0;
  af.level     = level;
  affect_join( victim, &af );

  if ( IS_AWAKE( victim ) )
  {
    if ( IS_NPC( victim ) && victim->talk_buf )
    {
      free_string( victim->talk_buf );
      victim->talk_buf  = NULL;
      victim->talk_mode = MODE_ABSENT;
    }

    send_to_char( "�@�}�w�t�M�A��M�P��ܷQ�ΡT\n\r", victim );
    act( "$N�Q�A�o�X���@�ζ®����N�εۤF�C", ch, NULL, victim, TO_CHAR );
    act( "$n��M�����˦b�a�W�I�I�j�ΡC", victim, NULL, NULL, TO_ROOM );
    victim->position = POS_SLEEPING;

    if ( victim->mount ) unmount_char( victim, victim->mount );
  }

  RETURN( TRUE );
}

SKILL( cast_stone_skin )
{
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA   af;

  PUSH_FUNCTION( "cast_stone_skin" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_stone_skin: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( is_affected( victim, sn ) ) RETURN( FALSE );

  af.type      = sn;
  af.duration  = level;
  af.location  = APPLY_AC;
  af.modifier  = -( UMAX( 1, number_range( level / 2, level ) ) );
  af.level     = level;
  affect_to_char( victim, &af );

  act( "$n�V���רF��ë�M��M����$e���ֽ��ܪ�����Y�@�ˡC"
    , victim, NULL, NULL, TO_ROOM );

  send_to_char( "�A���ֽ��w���M���Y�@�ˡT\n\r", victim );
  RETURN( TRUE );
}

SKILL( cast_teleport )
{
  CHAR_DATA       * victim = ( CHAR_DATA * ) vo;
  ROOM_INDEX_DATA * pRoomIndex;

  PUSH_FUNCTION( "cast_teleport" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_teleport: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( is_pk( victim ) )
  {
    send_to_char( "�藍�_�M�A���b�Z���j�|�̡M�S����k�I�i�o�Ӫk�N�T\n\r", ch );
    RETURN( FALSE );
  }

  act( "$n���_�@�i���ɶǰe�šC���M�|�P�j�_�F�@�}�D�B�B�������M���������ϩ�"
       "���\\�h�H�v�b�̰ʡM�M��@�}������M�z�_�T", ch, NULL, NULL, TO_CHAR );

  if ( !victim->in_room
    || victim->mount
    || victim->mount_by
    || victim->spirit
    || victim->boss
    || is_affected( victim, SLOT_CURSE )
    || victim->in_room->NoRecall
    || ( !IS_NPC( ch ) && victim->fighting != NULL )
    || ( victim != ch
      && ( saves_spell( level, victim ) || saves_spell( level, victim ) ) ) )
  {
    send_to_char( "���O����Ƥ]�S���o�͡C\n\r", ch );
    RETURN( FALSE );
  }

  for ( ; ; )
  {
    pRoomIndex = get_room_index( number_range( 0, 65535 ) );

    if ( pRoomIndex
      && pRoomIndex->NoQuit  == FALSE
      && pRoomIndex->Safe    == FALSE
      && pRoomIndex->Sail    == FALSE
      && pRoomIndex->Private == FALSE
      && !is_loge( pRoomIndex ) ) break;
  }

  act( "$n�uı�o���l���_�U�Y�M�@���U�Y�M�@���U�Y�M�@���U�Y�M�@���U�Y�C"
      , victim, NULL, NULL, TO_ALL );

  char_from_room( victim );
  char_to_room( victim, pRoomIndex );

  act( "�����D����ɭԡM$n�o�{���@�������b��w���a���W�M�i�O�|�P������"
       "�o���@�ˤF�C" , ch, NULL, NULL, TO_ALL );

  do_look( victim, "auto" );

  /* �M���l�ܬ����I */
  clear_trace( victim, TRUE );

  RETURN( TRUE );
}

SKILL( cast_weaken )
{
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA   af;

  PUSH_FUNCTION( "cast_weaken" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "cast_weaken: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( is_affected( victim, sn )
    || saves_spell( level, victim )
    || get_curr_str( victim ) <= 2 )
  {
    act( "�A���k�N��$N�èS���ġT", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  af.type      = sn;
  af.duration  = number_range( level / 22, level / 2 );
  af.location  = APPLY_STR;
  af.modifier  = -2;
  af.level     = level;
  affect_to_char( victim, &af );
  send_to_char( "�A�Pı������z�F�@�ǡM�n���s�������O�q���S�F�T\n\r", victim );
  if ( ch != victim ) send_to_char( "�n�F�T\n\r", ch );
  RETURN( TRUE );
}

SKILL( cast_word_of_recall )
{
  PUSH_FUNCTION( "cast_word_of_recall" );
  do_recall( ( CHAR_DATA * ) vo, "" );
  RETURN( TRUE );
}

SKILL( cast_acid_breath )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  OBJ_DATA  * obj_lose;
  OBJ_DATA  * obj_next;
  int         dam;

  PUSH_FUNCTION( "cast_acid_breath" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_acid_breath: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( level < 40 )
  {
    if ( saves_spell( level, victim ) ) dam /= 2;

    act( "$n��q�@�n�M�b�@�}�շϤ��u��$e�w�ۤƦ��@�䥨�s�C"
      , ch, NULL, NULL, TO_ALL );
  }
  else
  {
    if ( saves_spell( level, victim ) ) dam /= 2;
    act( "$n�����v�Z�a���������M�b$N�������ɭԡM�@�䥨�j�����s�q�Ѧӭ�"
         "�M�N$N�~�F�@���T", ch, NULL, victim, TO_ALL );
  }

  sact( ch, victim, NULL,
    "���s�ͻH�ĤO�@���M�u���з��j���M���ۤ@�}���j���R����$n�V"
    "$N�o�X�C\n\r�򱵵۶·t�@���M�A���򳣬ݤ���M�����P����@�ѻĨ�"
    "�M�A�P��@�}�w�t�C" );

  dam = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_MOB );

  if ( number_percent() < 2 * level && !saves_spell( level, victim ) )
  {
    for ( obj_lose = ch->carrying; obj_lose; obj_lose = obj_next )
    {
      int iWear;

      obj_next = obj_lose->next_content;
      if ( number_bits( 2 ) != 0 ) continue;

      switch ( obj_lose->item_type )
      {
      case ITEM_ARMOR:
        if ( obj_lose->value[0] > 0 )
        {
          act( "$p�o�X�}�}�շϡM���ۦb�ĲG�����a�F�T"
            , victim, obj_lose, NULL, TO_CHAR );

          if ( ( iWear = obj_lose->wear_loc ) != WEAR_NONE )
            victim->armor -= apply_ac( obj_lose, iWear );

          obj_lose->value[0] -= 1;
          obj_lose->cost      = 0;

          if ( iWear != WEAR_NONE )
            victim->armor += apply_ac( obj_lose, iWear );
        }
        break;

      case ITEM_CONTAINER:
        act( "$p�Q�ĲG�G�k�M�_�F�@�}�ϫK�����F�T"
          , victim, obj_lose, NULL, TO_CHAR );
        extract_obj( obj_lose );
        break;
      }
    }
  }

  damage( ch, victim, dam, sn, ATTACK_RANDOM );
  RETURN( TRUE );
}

SKILL( cast_fire_breath )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  OBJ_DATA  * obj_lose;
  OBJ_DATA  * obj_next;
  int         dam;

  PUSH_FUNCTION( "cast_fire_breath" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_fire_breath: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( number_percent() < 2 * level && !saves_spell( level, victim ) )
  {
    for ( obj_lose = victim->carrying; obj_lose; obj_lose = obj_next )
    {
      char * msg;

      obj_next = obj_lose->next_content;

      if ( number_bits( 2 ) != 0 ) continue;

      switch ( obj_lose->item_type )
      {
      default:             continue;
      case ITEM_CONTAINER: msg = "$p�_���U�N�F�T";         break;
      case ITEM_POTION:    msg = "$p�_�X��w�Ӫm�ˤF�T";   break;
      case ITEM_SCROLL:    msg = "$p�_���U�N�F�T";         break;
      case ITEM_STAFF:     msg = "$p�_�Ϧ��ܦ��쬴�F�T";   break;
      case ITEM_WAND:      msg = "$p�_�ϦӸH���F�T";       break;
      case ITEM_FOOD:      msg = "$p�ܦ��J�ҤF�T";         break;
      case ITEM_PILL:      msg = "$p���ƦӺw���a���T";     break;
      }

      act( smash_act_keyword( msg, "nesp" ) , victim, obj_lose, NULL, TO_CHAR );
      extract_obj( obj_lose );
    }
  }

  act( "$1$n$1���f�@�i�M�@�Ѥ��K�몺�R���Ѥf���Q�X�M���G$N�T$0"
    , ch, NULL, victim, TO_CHAR );

  dam  = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_MOB );
  if ( saves_spell( level, victim ) ) dam /= 2;
  damage( ch, victim, dam, sn, ATTACK_RANDOM );
  RETURN( TRUE );
}

SKILL( cast_frost_breath )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  OBJ_DATA  * obj_lose;
  OBJ_DATA  * obj_next;
  int         dam;

  PUSH_FUNCTION( "cast_frost_breath" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_frost_breath: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( number_percent() < 2 * level && !saves_spell( level, victim ) )
  {
    for ( obj_lose = victim->carrying; obj_lose; obj_lose = obj_next )
    {
      obj_next = obj_lose->next_content;

      if ( number_bits( 2 ) != 0 ) continue;

      switch ( obj_lose->item_type )
      {
      default:
        continue;

      case ITEM_CONTAINER:
      case ITEM_DRINK_CON:
      case ITEM_POTION:

        act( "$p�D�췴�a�F�T", victim, obj_lose, NULL, TO_CHAR );
        extract_obj( obj_lose );
        break;
      }
    }
  }

  sact( ch, victim, NULL,
    "$S$n$S�N�f����$w�B��$s�R���R�V$N�M�|�P���ū׫��ح���B�I�H�U�T$Z" );

  dam  = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_MOB );
  if ( saves_spell( level, victim ) ) dam /= 2;
  damage( ch, victim, dam, sn , ATTACK_RANDOM );
  RETURN( TRUE );
}

SKILL( cast_gas_breath )
{
  CHAR_DATA * vch;
  CHAR_DATA * vch_next;
  int         dam;

  PUSH_FUNCTION( "cast_gas_breath" );

  if ( !ch->in_room )
  {
    mudlog( LOG_DEBUG, "cast_gas_breath: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  for ( vch = ch->in_room->people; vch; vch = vch_next )
  {
    vch_next = vch->next_in_room;

    /* �Ǫ������Ǫ� */
    if ( IS_NPC( ch ) && IS_NPC( vch ) ) continue;

    if ( !check_can_kill( ch, vch ) ) continue;

    sact( ch, vch, NULL,
      "$G$n$G�i�f�Q�X�@�r��$b�˴�$R���ɡM�{���@���V�áT$Z" );

    if ( IS_NPC( ch ) ? !IS_NPC( vch ) : IS_NPC( vch ) )
    {
      dam  = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_MOB );
      if ( saves_spell( level, vch ) ) dam /= 2;
      damage( ch, vch, dam, sn , ATTACK_RANDOM );
    }
  }

  RETURN( TRUE );
}

SKILL( cast_lightning_breath )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         dam;

  PUSH_FUNCTION( "cast_lightning_breath" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_lightning_breath: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  sact( ch, victim, NULL,
    "$W$n$W�I�W�ͻH�@�}�@�󤧻ڧY�ѤW�ſE�g�X�ƹD"
    "$s�p�q$W�M�������e��$N������T$Z" );

  dam  = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_MOB );
  if ( saves_spell( level, victim ) ) dam /= 2;
  damage( ch, victim, dam, sn, ATTACK_RANDOM );
  RETURN( TRUE );
}

SKILL( cast_scan )
{
  int    dir;
  bool   found;
  char * pString = ( char * ) vo;

  PUSH_FUNCTION( "cast_scan" );

  if ( !check_blind( ch ) || IS_NPC( ch ) ) RETURN( FALSE );

  if ( !pString[0] )
  {
    act( "$n�V�|���K�軷���C", ch, NULL, NULL, TO_ROOM );

    for ( found = FALSE, dir = 0; dir < DIR_MAX; dir++ )
    {
      if ( ch->in_room->exit[dir] )
      {
        scan( ch, dir );
        found = TRUE;
      }
    }

    if ( !found ) act( "$r�o��S������X�f�T", ch, NULL, NULL, TO_CHAR );
  }

  else
  {
         if ( !str_prefix( pString, "north" ) ) dir = DIR_NORTH;
    else if ( !str_prefix( pString, "east"  ) ) dir = DIR_EAST;
    else if ( !str_prefix( pString, "south" ) ) dir = DIR_SOUTH;
    else if ( !str_prefix( pString, "west"  ) ) dir = DIR_WEST;
    else if ( !str_prefix( pString, "up"    ) ) dir = DIR_UP;
    else if ( !str_prefix( pString, "down"  ) ) dir = DIR_DOWN;
    else if ( !str_prefix( pString, "out"   ) ) dir = DIR_OUT;
    else
    {
      act( "$T ���O�Ӥ�V�T", ch, NULL, pString, TO_CHAR );
      RETURN( TRUE );
    }

    act( "$n�V$T�軷���C", ch, NULL, direction_name( dir ), TO_ROOM );

    if ( !ch->in_room->exit[dir] )
    {
      act( "$T��S���X���T", ch, NULL, direction_name( dir ), TO_CHAR );
      RETURN( TRUE );
    }

    scan( ch, dir );
  }

  RETURN( TRUE );
}

void scan( CHAR_DATA * ch , int door )
{
  int               distance;
  int               visibility = 6;
  bool              found;
  ROOM_INDEX_DATA * was_in_room;
  char * const dist_name [] =
    { "", "�ܪ�B", "�����B", "�y���B", "���B", "�����B", "�̻��B" };

  PUSH_FUNCTION( "scan" );

  if ( !ch || !ch->in_room )
  {
    mudlog( LOG_DEBUG, "scan: �ʥF�ӷ�" );
    RETURN_NULL();
  }

  if ( door < 0 || door >= DIR_MAX )
  {
    mudlog( LOG_DEBUG, "scan: �X�f %d ����.", door );
    RETURN_NULL();
  }

  if ( !IS_SET( ch->act, PLR_HOLYLIGHT ) )
  {
    switch( weather_info.sunlight )
    {
    case SUN_SET:   visibility = 4; break;
    case SUN_DARK:  visibility = 2; break;
    case SUN_RISE:  visibility = 4; break;
    case SUN_LIGHT: visibility = 6; break;
    }

    switch( weather_info.sky )
    {
    case SKY_CLOUDLESS:                  break;
    case SKY_CLOUDY:    visibility -= 1; break;
    case SKY_RAINING:   visibility -= 2; break;
    case SKY_LIGHTNING: visibility -= 3; break;
    }
  }

  was_in_room = ch->in_room;
  for ( found = FALSE ,distance = 1; distance <= DIR_MAX; distance++ )
  {
    EXIT_DATA * pExit;
    CHAR_DATA * list;
    CHAR_DATA * rch;

    if ( ( pExit = ch->in_room->exit[door] )
      && pExit->to_room
      && pExit->to_room != was_in_room )
    {
      if ( pExit->closed )
      {
        char door_name[MAX_STRING_LENGTH];

        one_argument( pExit->keyword, door_name );
        if ( !door_name[0] ) str_cat( door_name, "��" );

        print_to_char( ch , "�@�ӳQ���_�Ӫ�%s�b%s��%s�C\n\r"
          , door_name, direction_name( door ), dist_name[distance-1] );

        found = TRUE;
        break;
      }

      if ( pExit->to_room->Dark )
      {
        visibility--;
        continue;
      }

      ch->in_room = pExit->to_room;
      if ( IS_OUTSIDE( ch ) ? distance > visibility : distance > 4 ) break;

      list = ch->in_room->people;
      for ( rch = list; rch; rch = rch->next_in_room )
      {
        if ( can_see( ch, rch ) )
        {
          print_to_char( ch , "%s%s�N�b�A��%s��%s�C\n\r"
            ,mob_name( ch, rch )
            ,IS_NPC( rch ) ? "" : "�u���a�v"
            ,direction_name( door )
            ,dist_name[distance] );
          found = TRUE;
        }
      }
    }
  }

  ch->in_room = was_in_room;

  if ( !found ) act( "�A��$t��S������ȱo�`�N���H�C"
    , ch, direction_name( door ), NULL, TO_CHAR );

  RETURN_NULL();
}

SKILL( cast_seven_key_numen )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         dam;

  PUSH_FUNCTION( "cast_seven_key_numen" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_seven_key_numen: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  /* �e�m�T�� */
  sact( ch, victim, NULL,
    "$R        �Ӧۭ߬ɪ��C��̰ڡT$A        $Y�ΤC���_�ͥ��}�a�������T"
    "$A$A        $G�s���]�k�T�C�_�u�@���T$Z$A" );

  dam = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_MAG );
  dam = number_range( dam, dam * 3 );

  if ( ch->alignment < -( AlignRange / 2 ) )
  {
    sact( ch, victim, NULL, "$R�߬ɪ����K���������N��$N���W�T$Z" );
    dam = number_range( dam, dam * ( ( 0 - ch->alignment ) / 300 ) );
    dam = UMAX( 0, UMIN( MAG_ATTACK_DAM, dam ) );
  }

  if ( ch->skill[sn] > 85 && number_percent() > 70 )
  {
    sact( ch, victim, NULL,
      "$S�@�}�v�Ū����K�����M�j�Ѩ�$R����k$S�ѤѦӭ��T$A"
      "$R����k$S���۫K�ζ¦⪺�ͻH����$N�T$Z$A" );

    dam = number_range( 3 * dam / 2, dam * 3 );
    dam = UMAX( 0, UMIN( MAG_ATTACK_DAM, dam ) );
  }

  damage( ch, victim, dam, sn, ATTACK_RANDOM );

  RETURN( TRUE );
}

/* �t���j�k */
SKILL( cast_dormancy )
{
  int  old;
  int  value;
  bool change = FALSE;

  PUSH_FUNCTION( "cast_dormancy" );

  old      = ch->hit;
  value    = number_range( get_curr_con( ch ), get_curr_con( ch ) * 2 );
  ch->hit  = UMIN( get_curr_hit( ch ), level + value + ch->hit  );
  if ( ch->hit > old ) change = TRUE;

  old      = ch->mana;
  value    = number_range( get_curr_int( ch ), get_curr_int( ch ) * 2 );
  ch->mana = UMIN( get_curr_mana( ch ), level + value + ch->mana );
  if ( ch->mana > old ) change = TRUE;

  old      = ch->move;
  value    = number_range( get_curr_dex( ch ), get_curr_dex( ch ) * 3 );
  ch->move = UMIN( get_curr_move( ch ) , level + value + ch->move );
  if ( ch->move > old ) change = TRUE;

  if ( change )
  {
    /* ��o�ޯ઺���m�� */
    gain_skill_exp( ch , sn , TRUE );

    send_to_char( "�A���I�l�ͩ�w�C�M����]�u���o��Ѳ�C\n\r", ch );
  }

  else
  {
    send_to_char( "�A���I�l�S�^�_�쥿�`���ˤl�C\n\r", ch );
  }

  RETURN( TRUE );
}

/* �����g */
SKILL( cast_physique )
{
  int old;
  int value;

  PUSH_FUNCTION( "cast_physique" );

  old      = ch->move;
  value    = number_range( get_curr_dex( ch ), get_curr_dex( ch ) * 2 );
  ch->move = UMIN( get_curr_move( ch ) ,level + ch->move + value );

  if ( ch->move > old )
  {
    /* ��o�ޯ઺���m�� */
    gain_skill_exp( ch , sn , TRUE );

    switch( level / 20 )
    {
    default:
    case 0:
      send_to_char( "�A���G�P�즳�ƹD�u��C�����餺�C\n\r", ch );
      break;

    case 1:
      send_to_char( "�A�餺���u��V�ӶV�j�P�M�A�֭n�����C", ch );
      break;

    case 2:
      send_to_char( "�A�餺���u��S�p�s���ѡM�p��p�ۡC\n\r", ch );
      break;

    case 3:
      send_to_char( "�u���A���y�H�I�M�ƹD�u��S�p���s��_�F�X�ӡC\n\r", ch );
      break;

    case 4:
      send_to_char( "�A�y�W�@�}���M�@�}���M�餺�����X���֬��C\n\r", ch );
      break;
    }
  }

  else
  {
    send_to_char( "�A�餺���ƹD�u��C�C���h�����F�T\n\r", ch );
  }

  RETURN( TRUE );
}

/* ���I�C */
SKILL( cast_meditation )
{
  int old;
  int value;

  PUSH_FUNCTION( "cast_meditation" );

  old      = ch->mana;
  value   = number_range( get_curr_int( ch ), get_curr_int( ch ) * 2 );
  ch->mana = UMIN( get_curr_mana( ch ) ,level + ch->mana + value );

  if ( ch->mana > old )
  {
    /* ��o�ޯ઺���m�� */
    gain_skill_exp( ch , sn , TRUE );
    send_to_char( "�A�𮧽դáM�I�l���C�M�ߤ��o���p�d�x�U���b�ˡC\n\r", ch );
  }

  else
  {
    send_to_char( "�A�w�g�L�k�~��߷Q�F�C\n\r", ch );
  }

  RETURN( TRUE );
}

/* �g�߰f�� */
SKILL( cast_anti_physique )
{
  int old;
  int value;

  PUSH_FUNCTION( "cast_anti_physique" );

  old     = ch->hit;
  value   = number_range( get_curr_con( ch ) * 2, get_curr_con( ch ) * 4 );
  ch->hit = UMIN( get_curr_hit( ch ), ch->hit + value + level );

  if ( ch->hit > old )
  {
    /* ��o�ޯ઺���m�� */
    gain_skill_exp( ch , sn , TRUE );

    switch( level / 50 )
    {
    default:
    case 0:
      send_to_char( "���W���½��M�g�߰f��T\n\r", ch );
      break;

    case 1:
      send_to_char( "���W�_�g�K�߬Ұf��M�����ػ����X�Ӫ��h�֡C\n\r", ch );
      break;
    }
  }

  else
  {
    send_to_char( "�A�����S��_���`�C\n\r", ch );
  }

  RETURN( TRUE );
}

SKILL( cast_venom )
{
  OBJ_DATA * obj = ( OBJ_DATA * ) vo;
  int        timer;

  PUSH_FUNCTION( "cast_venom" );

  if ( !obj )
  {
    mudlog( LOG_DEBUG, "cast_venom: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( obj->venom > 0 )
  {
    act( "�藍�_�M$p�w�g�٬r�F�T", ch, obj, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  switch( obj->item_type )
  {
  default:
    break;

  case ITEM_MONEY:
  case ITEM_BOAT:
  case ITEM_CORPSE_NPC:
  case ITEM_CORPSE_PC:
  case ITEM_FOUNTAIN:
  case ITEM_LETTER:

    act( "�A�O�S����k�b$p�W���٬r���T", ch, obj, NULL, TO_CHAR );
    RETURN( TRUE );
  }

  obj->venom = timer = UMAX( 1, level /5 );

  act( "�p�ߡM$n�w�g�b$p�W�٤F�r�M�ɮĬO$I�p�ɡC", ch, obj, &timer, TO_ALL );
  RETURN( TRUE );
}

SKILL( cast_develop )
{
  OBJ_DATA * obj = ( OBJ_DATA * ) vo;
  int        max;
  int        diff;
  int        enforce;

  PUSH_FUNCTION( "cast_develop" );

  if ( !obj )
  {
    mudlog( LOG_DEBUG, "cast_develop: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( !is_armor( obj ) || obj->max_armor <= 0 )
  {
    act( "$p�O�L�k�W�j����T", ch, obj, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  if ( obj->armor < ( ( obj->max_armor * 80 ) / 100 ) )
  {
    act( "�A����$p�ײz�n�@�I�M�A�ӥ[�j�a�T", ch, obj, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  max = UMIN( MAX_ARMOR , ( ( level / 33 ) + 1 ) * obj->max_armor );

  if ( ( diff = max - obj->armor ) <= 0 )
  {
    act( "$p�w�g�W�j�F�쳻���F�T", ch, obj, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  enforce    = number_range( 1, level * 2 );
  obj->armor = UMIN( max, obj->armor + enforce );

  act( "$n��$p�����@�O�S���@�F���֡T", ch, obj, NULL, TO_ALL );
  RETURN( TRUE );
}

SKILL( cast_repair )
{
  OBJ_DATA * obj = ( OBJ_DATA * ) vo;
  int        max;
  int        enforce;

  PUSH_FUNCTION( "cast_repair" );

  if ( !obj )
  {
    mudlog( LOG_DEBUG, "cast_repair: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( !is_armor( obj ) || obj->max_armor <= 0 )
  {
    act( "$p�ثe�L�k�ײz����T", ch, obj, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  if ( ( max = obj->max_armor - obj->armor ) <= 0 )
  {
    act( "$p�n���ܡM���έײz�F�T", ch, obj, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  enforce = UMIN( number_range( 1, level * 20 ), max );
  obj->armor = UMIN( obj->armor + enforce, obj->max_armor );

  if ( obj->armor >= obj->max_armor )
    act( "$n�V�O�a�ײz$p�M$p�ݰ_�өM�s���@�ˤF�T", ch, obj, NULL, TO_ALL );
  else
    act( "$n�ɤO�a�ײz$p�M���޳N���������M�ٱo�V�O�T", ch, obj, NULL, TO_ALL );

  RETURN( TRUE );
}

SKILL( cast_preserve )
{
  OBJ_DATA * obj = ( OBJ_DATA * ) vo;

  PUSH_FUNCTION( "cast_preserve" );

  if ( !obj )
  {
    mudlog( LOG_DEBUG, "cast_preserve: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  switch( obj->item_type )
  {
  default:
    act( "$p�O�L�k�����G�B�z���T", ch, obj, NULL, TO_CHAR );
    RETURN( FALSE );

  case ITEM_FOOD:
    break;
  }

  if ( obj->timer >= level * 30 )
  {
    act( "$p���O�s�����w�g�W�X�A�઺�����F�T", ch, obj, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  obj->timer += number_range( 10, ( level / 2 ) * 10 );

  act( "$n�I�i���G�N�M�u��$p����Ţ�n�ۤ@�ѯS���y�C", ch, obj, NULL, TO_ALL );
  RETURN( TRUE );
}

SKILL( cast_cook )
{
  OBJ_DATA * obj = ( OBJ_DATA * ) vo;
  OBJ_DATA * meat;

  PUSH_FUNCTION( "cast_cook" );

  if ( !obj || !ch->in_room )
  {
    mudlog( LOG_DEBUG, "cast_cook: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( ch->in_room->nobject >= MaxRoomObject )
  {
    send_to_char( "�o�̪����~�Ӧh�F�M�Х��^�m���@�ǧa�T\n\r", ch );
    RETURN( FALSE );
  }

  switch( obj->item_type )
  {
  default:
    act( "$p�O�L�k���ӮƲz���T", ch, obj, NULL, TO_CHAR );
    RETURN( FALSE );

  case ITEM_CORPSE_NPC:
    break;
  }

  if ( !( meat = create_object( ObjMeat, 1 ) ) )
  {
    send_to_char( "�藍�_�M�t�Χ@���X�׶��X�ӡT\n\r", ch );
    mudlog( LOG_DEBUG, "cast_cook: �S���׶� %d.", ObjMeatVnum );
    RETURN( FALSE );
  }

  act( "$n�ΤM��$p���F���M�Ʋz��$P�C", ch, obj, meat, TO_ALL );

  extract_obj( obj );
  obj_to_room( meat, ch->in_room );
  RETURN( TRUE );
}

SKILL( cast_steal )
{
  char         buf[MAX_STRING_LENGTH];
  char         arg1[MAX_INPUT_LENGTH];
  char         arg2[MAX_INPUT_LENGTH];
  CHAR_DATA  * victim;
  OBJ_DATA   * obj;
  char       * argument = ( char * ) vo;
  int          percent;
  int          amount;

  PUSH_FUNCTION( "cast_steal" );

  if ( !ch || !ch->in_room )
  {
    mudlog( LOG_DEBUG, "cast_steal: �ӷ����~." );
    RETURN( FALSE );
  }

  if ( !argument || !*argument )
  {
    send_to_char( "�A�n�q�֨��W������F��S\n\r", ch );
    RETURN( FALSE );
  }

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );

  if ( arg1[0] == '\x0' || arg2[0] == '\x0' )
  {
    send_to_char( "�A�Q�q�֨��W������F��O�S\n\r", ch );
    RETURN( FALSE );
  }

  if ( ch->pcdata && ch->pcdata->steal_panity > 0 )
  {
    act( "��M���n�����Ӷԧֳ�T�|�Q�H�o�{��T", ch, NULL, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  if ( !( victim = get_char_room( ch, arg2 ) ) )
  {
    act( "�A�n�����H $2$T$0 ���b�o�̡T", ch, NULL, arg2, TO_CHAR );
    RETURN( FALSE );
  }

  if ( victim == ch )
  {
    send_to_char( "�A�n���A�ۤv���F��M�A���S���d���T\n\r", ch );
    RETURN( FALSE );
  }

  if ( IS_IMMORTAL( victim ) )
  {
    act( "�A���i�H���j��$N���˳ơT", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  if ( !IS_NPC( ch ) && excess_filequota( ch ) )
  {
    send_to_char( "�藍�_�M�A���ɮפӤj�M�L�k����o�өR�O�T\n\r", ch );
    RETURN( FALSE );
  }

  if ( !can_steal( ch, victim ) ) RETURN( FALSE );

  percent = number_percent() + ( IS_AWAKE( victim ) ? 20 : -30 );

  if ( ch->level + 5 < victim->level
    || victim->position == POS_FIGHTING
    || ( !IS_NPC( ch ) && percent > level ) )
  {
    /* ���� */
    send_to_char( "�u�O�V�|�M���M����աM���u�I�T\n\r", ch );
    act( "$n���ձq�A���W���I�F���@�����~�C", ch, NULL, victim, TO_VICT );
    act( "$n���ձq$N���W���I�F��ӷ�@�����~�C",  ch, NULL, victim, TO_NOTVICT );

    sprintf( buf, "%s�O�ӤU�y���p���T", mob_name( NULL, ch ) );

    ChatRecord = FALSE;
    do_yell( victim, buf );
    ChatRecord = TRUE;

    if ( !IS_NPC( ch ) )
    {
      if ( IS_NPC( victim )
        && !( victim->fighting )
        && !IS_SET( victim->act, ACT_NOKILL )
        && ch->in_room->Safe == FALSE )
      {
        striking( victim, ch, TYPE_UNDEFINED );
      }

      else
      {
        mudlog( LOG_INFO , "[STEAL] %s ���F�襢�ѡM��H�R %s�T"
          , ch->name, victim->name );

        if ( !IS_SET( ch->act, PLR_THIEF ) )
        {
          SET_BIT( ch->act, PLR_THIEF );
          send_to_char( "�A�{�b�O�@�Ӥp���T\n\r", ch );
          save_char_obj( ch , SAVE_FILE );
        }
      }
    }

    RETURN( TRUE );
  }

  if ( !str_prefix( arg1, "coins" ) || !str_cmp( arg1, "gold" ) )
  {
    amount = victim->gold * number_range( 1, 10 ) / 100;
    if ( amount <= 0 || over_scale( ch ) )
    {
      act( "�A�S��k�q$N���W�A�d����F�C", ch, NULL, victim, TO_CHAR );
      RETURN( TRUE );
    }

    mudlog( LOG_INFO, "[STEAL] %s �������\\�M��H�R%s."
      , ch->name, victim->name );
    gold_to_char( ch, amount );
    gold_from_char( victim, amount );
    act( "�u�O�����T�A����F$i������C", ch, &amount, NULL, TO_CHAR );
    if ( ch->pcdata ) ch->pcdata->steal_panity = 3;
    RETURN( TRUE );
  }

  if ( !( obj = get_obj_carry( victim, arg1 ) ) )
  {
    send_to_char( "½��½�h�M���M�䤣��A�n���F��M�z�����T\n\r", ch );
    RETURN( TRUE );
  }

  if ( !can_see_obj( ch , obj ) ) RETURN( TRUE );

  if ( !can_drop_obj( ch, obj )
    || obj->address
    || obj->Inventory
    || obj->level > ch->level )
  {
    act( "�A������$p�C�C", ch, obj, NULL, TO_CHAR );
    RETURN( TRUE );
  }

  if ( ch->carry_number + get_obj_number( obj ) > can_carry_n( ch ) )
  {
    send_to_char( "�A�ۤv���W�w�g��a�Ӧh���~�F�C\n\r", ch );
    RETURN( TRUE );
  }

  if ( ch->carry_weight + get_obj_weight( obj ) > can_carry_w( ch ) )
  {
    send_to_char( "�A�ۤv���W�w�g��a�ӭ������~�F�C\n\r", ch );
    RETURN( TRUE );
  }

  mudlog( LOG_INFO, "[STEAL] %s ���F�覨�\\�M��H�R%s ���~ %s ."
    , ch->name, victim->name , obj->name );

  obj_from_char( obj );
  obj_to_char( obj, ch );
  act( "$p���F�T", ch, obj, NULL, TO_CHAR );
  if ( ch->pcdata ) ch->pcdata->steal_panity = 5;

  RETURN( TRUE );
}

SKILL( cast_pick )
{
  extern const sh_int  rev_dir [];
  char                 arg[MAX_INPUT_LENGTH];
  CHAR_DATA          * gch;
  OBJ_DATA           * obj;
  int                  door;
  char               * argument = ( char * ) vo;
  ROOM_INDEX_DATA    * to_room;
  EXIT_DATA          * pexit;
  EXIT_DATA          * pexit_rev;

  PUSH_FUNCTION( "cast_pick" );

  if ( !ch || !ch->in_room )
  {
    mudlog( LOG_DEBUG, "cast_pick: �ӷ����~." );
    RETURN( FALSE );
  }

  if ( !argument || !*argument )
  {
    send_to_char( "�A�n���}���@����O�S\n\r", ch );
    RETURN( FALSE );
  }

  one_argument( argument, arg );

  for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
  {
    if ( IS_NPC( gch ) && IS_AWAKE( gch ) && ch->level + 5 < gch->level )
    {
      act( "$N�N�b�A����M�ҥH�L�k���}�C", ch, NULL, gch, TO_CHAR );
      RETURN( FALSE );
    }
  }

  if ( !IS_NPC( ch ) && number_percent() > level )
  {
    send_to_char( "�A���ѤF�T\n\r", ch);
    RETURN( TRUE );
  }

  if ( ( obj = get_obj_here( ch, arg ) ) )
  {
    if ( obj->item_type != ITEM_CONTAINER )
    {
      act( "$p���O�@�Ӯe���T", ch, obj, NULL, TO_CHAR );
      RETURN( FALSE );
    }

    if ( !IS_SET( obj->value[1], CONT_CLOSED ) )
    {
      act( "$p�èS�����_�ӰڡT", ch, obj, NULL, TO_CHAR );
      RETURN( FALSE );
    }

    if ( obj->value[2] < 0 )
    {
      act( "�A���ॴ�}$p�T", ch, obj, NULL, TO_CHAR );
      RETURN( FALSE );
    }

    if ( !IS_SET( obj->value[1], CONT_LOCKED ) )
    {
      act( "$p�w�g���}�F�T", ch, obj, NULL, TO_CHAR );
      RETURN( FALSE );
    }

    if ( IS_SET( obj->value[1], CONT_PICKPROOF ) )
    {
      send_to_char( "�A���ѤF�T\n\r", ch );
      RETURN( FALSE );
    }

    REMOVE_BIT(obj->value[1], CONT_LOCKED);
    send_to_char( "�ذդ@�n�M�A���ꥴ�}�F�T\n\r", ch );
    act( "$n��$p���ꥴ�}�F�C", ch, obj, NULL, TO_ROOM );
    RETURN( TRUE );
  }

  if ( ( door = find_door( ch, arg ) ) >= 0 )
  {
    pexit = ch->in_room->exit[door];

    if ( !pexit->closed )
    {
      send_to_char( "���èS�����_�ӡC\n\r", ch );
      RETURN( FALSE );
    }

    if ( pexit->key < 0 )
    {
      send_to_char( "�A���ॴ�}���C\n\r", ch );
      RETURN( FALSE );
    }

    if ( !pexit->locked )
    {
      send_to_char( "���èS����_�ӡC\n\r", ch );
      RETURN( FALSE );
    }

    if ( pexit->pick )
    {
      send_to_char( "�A���ѤF�C\n\r", ch );
      RETURN( TRUE );
    }

    pexit->locked = FALSE;
    send_to_char( "�ذդ@�n�M�A���ꥴ�}�F�C\n\r", ch );
    act( "$n��$d���ꥴ�}�F�C", ch, NULL, pexit->keyword, TO_ROOM );

    if ( ( to_room   = pexit->to_room               )
      && ( pexit_rev = to_room->exit[rev_dir[door]] )
      && pexit_rev->to_room == ch->in_room )
    {
      pexit_rev->locked = FALSE;
    }
  }
  RETURN( TRUE );
}

SKILL( cast_defloration )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         move;
  int         add;

  PUSH_FUNCTION( "cast_deloration" );

  if ( !ch || !victim )
  {
    mudlog( LOG_DEBUG, "cast_defloration" );
    RETURN( FALSE );
  }

  if ( victim->move <= 1 )
  {
    act( "$N����ؤw�g�Q�A�l���F�C", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  move = UMAX( 5, number_range( 1, level * 2 ) );
  victim->move = UMAX( 1, victim->move - move );

  if ( level > 20 )
  {
    add = move * UMIN( 30, level ) / 100;
    if ( add > 0 ) ch->move = UMIN( ch->max_move, ch->move + add );
  }

  act( "$n���X������$N�@�ġT�L�ƪ��O�q��J$n���餺�M��$n�κZ�L��T"
       "��$N�yı�������L�O�T�b�����M$Nı�o�ۤv�}�l���g�Ӯz�T"
       , ch, NULL, victim, TO_ALL );

  RETURN( TRUE );
}

SKILL( cast_cannibal )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         hit;
  int         add;

  PUSH_FUNCTION( "cast_cannibal" );

  if ( !ch || !victim )
  {
    mudlog( LOG_DEBUG, "cast_cannibal" );
    RETURN( FALSE );
  }

  if ( victim->hit <= 1 )
  {
    act( "$N�����w�g�Q�A�l���F�C", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  hit = UMAX( 5, number_range( 1, level * 2 ) );
  victim->hit = UMAX( 1, victim->hit - hit );

  if ( level > 20 )
  {
    add = hit * UMIN( 30, level ) / 100;
    if ( add > 0 ) ch->hit = UMIN( ch->max_hit, ch->hit + add );
  }

  act( "$n���$N�@���M�@�D�p���{�������u�V$N�g�h�M$N���ͩR�෽�Q$n�l���F�C"
       "$n�Ѥ⤤�o�X�@�D���������F$N�M$N�P��ۤv�餺���A�奿�������y���T"
       , ch, NULL, victim, TO_ALL );

  /* �Y���|���԰�, �h�}�Ҿ԰� */
  if ( !victim->fighting ) set_fighting( victim, ch );

  /* �Y�D���|���԰�, �h�}�Ҿ԰� */
  if ( !ch->fighting ) set_fighting( ch, victim );

  RETURN( TRUE );
}

SKILL( cast_energy_drain )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         mana;
  int         add;

  PUSH_FUNCTION( "cast_energy_drain" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_energy_drain: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( victim->mana <= 1 )
  {
    act( "$N�����w�g�Q�A�l���F�C", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  mana = UMAX( 5, number_range( 1, level * 2 ) );
  victim->mana = UMAX( 1, victim->mana - mana );

  if ( level > 20 )
  {
    add = mana * UMIN( 30, level ) / 100;
    if ( add > 0 ) ch->mana = UMIN( ch->max_mana, ch->mana + add );
  }

  act( "$n���$N�@���M�@�D�p���{�������u�V$N�g�h�M$N���ͩR�෽�Q$n�l���F�C"
       "$n�Ѥ⤤�o�X�@�D���������F$N�M$N�P��ۤv�餺���A�奿�������y���T"
       , ch, NULL, victim, TO_ALL );

  /* �Y���|���԰�, �h�}�Ҿ԰� */
  if ( !victim->fighting ) set_fighting( victim, ch );

  /* �Y�D���|���԰�, �h�}�Ҿ԰� */
  if ( !ch->fighting ) set_fighting( ch, victim );

  RETURN( TRUE );
}

SKILL( cast_wakeup )
{
  AFFECT_DATA * pAffect;
  CHAR_DATA   * victim = ( CHAR_DATA * ) vo;

  PUSH_FUNCTION( "cast_wakeup" );

    if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_wakeup: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( !is_affected( victim, SLOT_SLEEP ) )
  {
    act( "$N�èS���ε۰ڡT", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  if ( !is_affected( victim, SLOT_SLEEP ) )
  {
    act( "$N�èS������ʯv�N���v�T�T", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  for ( pAffect = victim->affected; pAffect; pAffect = pAffect->next )
  {
    if ( pAffect->type == SLOT_SLEEP ) break;
  }

  if ( !pAffect && is_affected( victim, SLOT_SLEEP ) )
  {
    act( "$N�Ϊ������`�H�M$n��$N�s���F�T", ch, NULL, victim, TO_ALL );
    RETURN( TRUE );
  }

  else if ( pAffect )
  {
    if ( pAffect->duration > ( level / 5 + 5 ) )
    {
      send_to_char( "�A���k�N���ĤF�C\n\r", ch );
      RETURN( FALSE );
    }

    act( "�@�}�������L���j�L�M�Ѱ��F$N���ʯv���A�C$A"
      "$n�{�b�i�H�N$N�n��($2wake$0)�T", ch, NULL, victim, TO_ALL );

    affect_release( victim, SLOT_SLEEP );
    RETURN( TRUE );
  }

  else
  {
    act( "$N�èS���b�ίv��T", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  RETURN( FALSE );
}

SKILL( cast_evil_power )
{
  AFFECT_DATA af;

  PUSH_FUNCTION( "cast_evil_power" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "cast_evil_power:�ӷ����~." );
    RETURN( FALSE );
  }

  if ( is_affected( ch, sn ) )
  {
    send_to_char( "�A�w�g�����c�F�T\n\r", ch );
    RETURN( FALSE );
  }

  act( "�ѤW�����]�ڡM�絹$n�û��L�a���O�q�a�T", ch, NULL, NULL, TO_ALL );

  af.type      = sn;
  af.duration  = number_range( level / 6, level / 5 );
  af.location  = APPLY_STR;
  af.modifier  = 1;
  af.level     = level;
  affect_join( ch, &af );

  RETURN( TRUE );
}

SKILL( cast_crash_weapon )
{
  CHAR_DATA  * victim = ( CHAR_DATA * ) vo;
  OBJ_DATA   * obj;
  OBJ_DATA   * crash;
  SITUS_DATA * pSitus;
  int          count;
  int          damage;

  PUSH_FUNCTION( "cast_crash_weapon" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_crash_weapon" );
    RETURN( FALSE );
  }

  crash = NULL;
  count = 0;

  for ( pSitus = situs_list; pSitus; pSitus = pSitus->next )
  {
    if ( !( obj = get_eq_char( victim, pSitus->location ) )
      || !can_see_obj( victim, obj ) ) continue;

    if ( number_range( 0, count++ ) == 0 ) crash = obj;
  }

  if ( crash == NULL || crash->max_armor <= 0 )
  {
    act( "$N���W�w�g�S���i�H�������˳ƤF�T", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  sact( ch, victim, crash,
    "$n�p���[��M�@�����|�N��$N��$O���������U�h�T" );

  damage = number_range( level * 2, level * 4 );
  crash->armor -= damage;

  if ( crash->armor <= 0 )
  {
    act( "$N���W��$p�L�k�Ө��o��Ӫ��r�P�@���M�H���F��b�T"
      , ch, crash, victim, TO_ALL );

    unequip_char( victim, crash, FALSE );
    extract_obj( crash );
  }

  /* �Y���|���԰�, �h�}�Ҿ԰� */
  if ( !victim->fighting ) set_fighting( victim, ch );

  /* �Y�D���|���԰�, �h�}�Ҿ԰� */
  if ( !ch->fighting ) set_fighting( ch, victim );

  RETURN( TRUE );
}

SKILL( cast_flying_stone )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         dam;

  PUSH_FUNCTION( "cast_flying_stone" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "cast_flying_stone: �ӷ����~." );
    RETURN( FALSE );
  }

  /* ��˱ԭz */
  act( "$n���⺴�ʡM���R�_�@�G���j���ۭ��M���a�۵L�ƪ����F�V��$N���h�T"
    , ch, NULL, victim, TO_ALL );

  /* �]�w��� */
  dam = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_ONE );

  damage( ch, victim, dam, sn, ATTACK_RANDOM );

  RETURN( TRUE );
}

SKILL( cast_slow )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  AFFECT_DATA af;

  PUSH_FUNCTION( "cast_slow" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "cast_slow: �ӷ����~." );
    RETURN( FALSE );
  }

  if ( saves_spell( level, victim ) && get_curr_dex( victim ) < 1 )
  {
    act( "�A��$N���𺢳N���G�S������ΡC", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  act( "$n�N�믫�����M�H���ⴧ�X�@�ζ¶��x��$N����ʡT"
    , ch, NULL, victim, TO_ALL );

  if ( ( number_percent() + victim->level / 2 )
    > ( number_percent() + level ) )
  {
    act( "���O$N�����ܻ������{�L�F�o�Ӷ¶��T", ch, NULL, victim, TO_ALL );
    RETURN( FALSE );
  }

  act( "�o�ζ¶��̦�F$N����ʡT", ch, NULL, victim, TO_ALL );

  af.type      = sn;
  af.duration  = number_range( level / 20, level / 15 );
  af.location  = APPLY_DEX;
  af.modifier  = -1;
  af.level     = level;
  affect_join( victim, &af );

  RETURN( TRUE );
}

SKILL( cast_mint )
{
  OBJ_DATA       * obj = ( OBJ_DATA * ) vo;
  OBJ_DATA       * target;
  OBJ_INDEX_DATA * pIndex;

  PUSH_FUNCTION( "cast_mint" );

  if ( !obj )
  {
    mudlog( LOG_DEBUG, "cast_repair: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( obj->item_type != ITEM_ORE
    || !( pIndex = get_obj_index( obj->value[0] ) )
    || obj->value[2] <= 0 )
  {
    act( "$p�O�L�k�i��ű�y���T", ch, obj, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  if ( !IS_NPC( ch ) && excess_filequota( ch ) )
  {
    send_to_char( "�藍�_�M�A���ɮפӤj�M�L�k����o�өR�O�T\n\r", ch );
    RETURN( FALSE );
  }

  if ( level < obj->value[1] )
  {
    act( "�ݨ�$p�D�A��O�ҤΡM�ЧO�H�ӸոլݡT", ch, obj, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  act( "$n���ۦ����M�@��V�O�a�����$p�T", ch, obj, NULL, TO_ALL );

  if ( number_range( 0, 10000 - level * 10 ) >= obj->value[2] )
  {
    act( "�ݨ�$p�٦����A���F�M�[�o�a�T", ch, obj, NULL, TO_CHAR );
  }

  else
  {
    target = create_object( pIndex, -1 );

    act( "�j�\\�i���M$n�ש�q$p���y�X$P�T", ch, obj, target, TO_ALL );

    if ( target->Takeable == FALSE
      || ( ch->carry_number + get_obj_number( target ) > can_carry_n( ch ) )
      || ( get_carry_weight( ch ) + get_obj_weight( target ) > can_carry_w( ch ) ) )
    {
      act( "�]��$n�L�k���_$p�M�ҥH����a�W�h�F�T", ch, target, NULL, TO_ALL );
      obj_to_room( target, ch->in_room );
    }

    else
    {
      act( "�A��M�o�{��A�����W�h�F�@��$p�T", ch, target, NULL, TO_CHAR );
      obj_to_char( target, ch );
    }

    obj_from_char( obj );
    extract_obj( obj );
  }

  RETURN( TRUE );
}

SKILL( cast_magic_power )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         chance;

  PUSH_FUNCTION( "cast_magic_power" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_magic_power: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( IS_NPC( victim ) )
  {
    chance = level + ( get_curr_dex( ch ) * 3 - get_curr_dex( victim ) * 2 );
  }

  else
  {
    chance = get_curr_dex( ch ) - get_curr_dex( victim );
  }

  RETURN( TRUE );
}

/* �ݬݬO�_�i�H���� ch �O�I�k��, victim �O��H.*/
bool can_steal( CHAR_DATA * ch , CHAR_DATA * victim )
{
  PUSH_FUNCTION( "can_fight" );

  if ( !ch || !victim || !ch->in_room )
  {
    mudlog( LOG_DEBUG, "can_fight: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  /* �p�G�ж����X�ЬO�w����, �@�w����o�;԰� */
  if ( ch->in_room->Safe )
  {
    act( "�藍�_�M$r�̬O���ఽ�Ѫ��C" , ch, NULL, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  if ( ( fight_info->status == PK_JOIN || fight_info->status == PK_BET )
    && is_pk( ch ) )
  {
    send_to_char( "�𮧤��ФŰ��ѡT\n\r", ch );
    RETURN( FALSE );
  }

  /* ���a�p�G�Q�g�b, ���ఽ�ۤv���D�H */
  if ( is_affected( ch, SLOT_CHARM_PERSON ) && ch->master == victim )
  {
    act( "�Ф��n�ϥD�H$N���F��C", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  /* �Y�O�D�H�@�w���ఽ, �n���ž� */
  if ( ch->master == victim || victim->master == ch )
  {
    send_to_char( "�A�S���ۥD�M�٥i�H�ð��ܡS\n\r" , ch );
    RETURN( FALSE );
  }

  if ( is_same_group( ch, victim ) && ch != victim )
  {
    act( "$N�O�A�˷R�����ͣ��M�~�M��$E�F��S", ch, NULL, victim, TO_CHAR );
    act( "�ְk�a�T$n�Q�n���A���F��T"        , ch, NULL, victim, TO_VICT );
    RETURN( FALSE );
  }

  if ( victim->mount_by )
  {
    act( "�n���F����ݥD�H$N�a�T", ch, NULL, victim->mount_by, TO_CHAR );
    RETURN( FALSE );
  }

  if ( victim->boss && victim->boss == ch )
  {
    act( "���S���d���M���ۤv$N���_���S", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  if ( victim->boss )
  {
    act( "$n�ܮ`�Ȧa����$N����M�]�����H�Q�n���L�F��T"
      , victim, NULL, victim->boss, TO_ALL );

     RETURN( FALSE );
  }

  if ( victim->hirer )
  {
    act( "�A���ఽ�O�H���ħL$N�C", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  /* �Ǫ����ݩʬO������` */
  if ( IS_NPC( victim ) && IS_SET( victim->act, ACT_NOKILL ) )
  {
    act( "�A�����$N�N�O����ʡC", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

   /* �p�G���F, �٨S�^�m�X����ɤ���I�i�k�N */
  if ( !IS_NPC( ch ) && ch->pcdata->corpse )
  {
    send_to_char( "�Aı�o�����Ϥ��W�O�M�ڥ��S��k���F��C\n\r" , ch );
    RETURN( FALSE );
  }

  if ( !can_damage( victim ) )
  {
    act( "�A�N��L$N�a�M�A�O���|�l���h�֪��T", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  RETURN( TRUE );
}

SKILL( cast_abstract )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  OBJ_DATA  * pUrn;
  int         base;
  int         chance;

  PUSH_FUNCTION( "cast_abstract" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_abstract: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( victim == ch )
  {
    send_to_char( "����M�A�n�ʦL�A�ۤv�ܡS\n\r", ch );
    RETURN( FALSE );
  }

  if ( !can_damage( victim ) )
  {
    act( "�A�ثe�S����k�ʦL$N�T", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  if ( !IS_NPC( victim ) || !victim->pIndexData )
  {
    act( "�A�u��ʦL�D���a�M$N�O���i�H�Q�ʦL���T", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  if ( !can_fight( ch , victim ) ) RETURN( FALSE );

  if ( victim->mount_by || victim->boss )
  {
    act( "�藍�_�M�A����ʦL�O�H�����M�Τp��$N�T", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  base   = UMAX( 100, number_range( 1, victim->level * victim->level ) );
  chance = ch->level + level * 2 + get_curr_wis( ch ) * 2;

  if ( base > chance )
  {
    act( "$n���D���٤����M���G�L�k�N$N�ʦL��T", ch, NULL, victim, TO_ALL );
  }

  else
  {
    pUrn = create_object( ObjUrn, -1 );
    pUrn->value[0] = victim->pIndexData->vnum;
    pUrn->value[1] = victim->hit;
    pUrn->value[2] = victim->mana;
    pUrn->value[3] = victim->move;

    obj_to_char( pUrn, ch );

    act( "��M�M$N�Q$n�ʦL��F�M��$N�����v�]�C�C�����b$p�����T"
      , ch, pUrn, victim, TO_ALL );

    raw_kill( ch, victim, FALSE );
  }

  RETURN( TRUE );
}

SKILL( cast_dark_hell )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         dam;

  PUSH_FUNCTION( "cast_dark_hell" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_dark_hell: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( ch->alignment > -( AlignRange / 5 ) && number_percent() > 50 )
  {
    act( "$n�������M��ť�ϳ�M�L�k�I�i�k�N�T", ch, NULL, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  /* �e�m�T�� */
  act( "$A$n���⹳�Ѥ@���M�Ⲵ�o�X�����M�f������ۤ����W���G�y�K$A"
    , ch, NULL, victim, TO_ALL );

  dam = spell_damage( ch, level, TYPE_INT, SPELL_CLASS_MAG );

  if ( ch->skill[sn] > 45 && number_percent() > 60 )
  {
    sact( ch, victim, NULL, "�a���U�ǨӤ@�}�n���R�u�l��ڪ��H�ڡT"
      "�ڨ̧A���޾ɫe�ӤF�T�v$A�@�ӥ��j���v��{���e�M��������"
      "���򳣬ݤ����M�u���@�}���ºC�C�X���M�]�ЩҦ�������T$A"
      "$A$A$A$b                    ���®��z���a��              $Z"
      "         $A$A$A�z�}�n�R���F�A���զ��M�Aı�o�D�`���L�T" );

    dam = number_range( dam * 4 / 3, dam * 2 );
    dam = UMAX( 0, UMIN( MAG_ATTACK_DAM, dam ) );

    if ( ch->skill[sn] > 80 )
    {
      sact( ch, victim, NULL, "$A�z�}���H�������S��$N�y���ĤG���ˮ`�T" );
      dam = number_range( dam, dam * 4 / 3 );
      dam = UMAX( 0, UMIN( MAG_ATTACK_DAM, dam ) );
    }
  }
  else
  {
    sact( ch, victim, NULL, "$n����o�X�@�}�¥��M�������j�L�k����M"
      "$A��O�ϤO�N��⤧�²y�X�Ӭ��@�M��V$N�T$A$A$A"
      "                    $b         ���®֦a��          "
      "               $Z$A$A$A�²y�ĦX�Ҳ��ͪ����׼��q�U�N��$N�T$Z" );
    dam = number_range( dam, dam * 4 / 3 );
    dam = UMAX( 0, UMIN( MAG_ATTACK_DAM, dam ) );
  }

  damage( ch, victim, dam, sn, ATTACK_RANDOM );

  RETURN( TRUE );
}

SKILL( cast_six_god )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         dam;
  int         choice[] = { 20, 40, 60, 80, 95, 100, -1 };

  PUSH_FUNCTION( "cast_six_god" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "cast_six_god: �ӷ����~." );
    RETURN( FALSE );
  }

  if ( IS_EVIL( ch ) )
  {
    act( "$n�Ө��c�F�M�L�k�l�꯫��T", ch, NULL, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  dam = spell_damage( ch, level, TYPE_WIS, SPELL_CLASS_FIV );
  dam = number_range( dam * 3 / 4, dam * 5 / 4 );
  dam = URANGE( 0, dam, LEVEL_FIV_DAM );

  switch( dimemsion_position( choice, level, TRUE ) )
  {
  case ERRORCODE:
    mudlog( LOG_DEBUG, "�ޯ� %d ���T���]�w�����D.", sn );
    RETURN( FALSE );

  case 0:
    sact( ch, victim, NULL, "$n�f����ᦳ���M�⵲$Y�y�K�j����L�z$Z�ϥX"
      "����l�M�����Q�D�M�l�O�p�@�e�p�V$N�C" );
    break;

  case 1:
    sact( ch, victim, NULL, "����$N��աM$n���W�����M���_$B�y"
      "���sù�~�L�z$Z�M�f����ᦳ���M�P���M���ͱj�b�X�l�M�|��"
      "���d��M�ɨ�$N��աT" );
    break;

  case 2:
    sact( ch, victim, NULL, "$n���_$S�y�[���üv�L�z$Z�ܹD�R$W�y�L�Фd��"
      "�[���{�u���z$Z�M�b�����M$N�ϩ��ݨ�$W�d���[��$Z�r�{�M$W�d���[��$Z��M"
      "�i�}�d��M�����ɶê��۴��ۦ��M������$N�F�X�d�����T" );
    break;

  case 3:
    sact( ch, victim, NULL, "$N�@�ۼA�ӡM$n���k�@�{�M�浲$P�y��p"
      "�_�ߦL�z$Z�M�p������M�K�ɧ֭Y�b�p�M��Y���q�M��¶��$N����"
      "�M���_�j�j�q�l�M��$N��߲r���T" );
    break;

  case 4:
    sact( ch, victim, NULL, "$n�ĦV$N�M�⵲$R�y���ĥ��k�L�z$Z�u��"
      "$n��M�Q�P���]��M$N�{�פ��ΡM�Q�k�L�F�ӥ��ۡM$N�uı��"
      "����`����$r���V��l$Z�b�餺«��M$N�s�s�@�n�M�n�ɤ����p�I"
      "�M����˦a�T" );
    break;

  case 5:
    sact( ch, victim, NULL, "$n�}�@�ޡM�@���ĤѡM�b�Ť��L�L��"
      "���M�⵲$Y�y�j�~�p�ӦL�z$Z�M�u���ѻڪ����U�D�M�m�������M$n"
      "�f����ᦳ���M��M�ɳ�$Y�y�H���X�@ �ѤW�a�U �ߧڿW�L�z$Z"
      "�M��Ѧp�ӻ��M�r�{�M�k�۲��Y�M$N�uı�o�߯��@�_�M�����"
      "�M�Q�@�ѥ��O�e��T" );
    break;
  }

  if ( ch->alignment > ( AlignRange * 3 / 4 ) )
  {
    sact( ch, victim, NULL, "$n��$W�j�P�t��$Z��$N�y���F�B�~���ˮ`�T" );
    dam = number_range( dam * 5 / 4, dam * 2 );
    dam = URANGE( 0, dam, LEVEL_FIV_DAM );
  }
  else if ( ch->alignment < ( AlignRange / 2 ) )
  {
    sact( ch, victim, NULL, "$n���ߤ����²b�M�¤O�j��T" );
    dam = number_range( dam / 2, dam * 2 / 3 );
    dam = URANGE( 0, dam, LEVEL_FIV_DAM );
  }

  damage( ch, victim, dam, sn, ATTACK_RANDOM );

  RETURN( TRUE );
}

SKILL( cast_water_cloud_blast )
{
  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         choice[] = { 9, 19, 29, 39, 49, 59, 69, 79, 89, 100, -1 };
  int         tt;
  int         multi;
  int         value;
  int         dam = 0;
  OBJ_DATA  * pObj;

  PUSH_FUNCTION( "cast_water_cloud_blast" );

  if ( !( pObj = get_wield( ch ) ) )
  {
    mudlog( LOG_DEBUG, "cast_water_cloud_blast: �Z�����~." );
    RETURN( FALSE );
  }

  level = number_range( level / 2, level );
  switch( dimemsion_position( choice, level, TRUE ) )
  {
  case ERRORCODE:
    mudlog( LOG_DEBUG, "cast_water_cloud_blast: �X�ɤF." );
    RETURN( FALSE );

  case 0:
    sact( ch, victim, pObj,
      "$n�����@���M�w�w�a�ਭ�M����C�C�a�f�W�F$O�}���M��M�����@�n�M�}��"
      "�e���M$S�����b�k$Z���u$R�X��$S����$Z�v�ϥX�M�@�b�g�V�F$N$Z�C" );
    dam = 300;
    break;

  case 1:
    sact( ch, victim, pObj,
      "$n���ίe���M�ʧ@���֥O�H�ؤ��v���M�ϩ����X�Q��$n�]���$N�M�d�o"
      "$N�u$S���Y����$Z�v�M�@�y��M�M$n�X���V$N�I��g�X�@�b�C$Z" );
    dam = 350;
    break;

  case 2:
    sact( ch, victim, pObj,
      "$n�������k�M�N�u�𩹤��ж����M�ߤ����N�y�ɮ���L�ܡM�y�W�S�X�O�d"
      "�Mħ�����e�M��$O�M�f�b�M�Ժ��}�M�Ҧ��ʧ@�@������M�p�u$S�涳�y��$Z�v"
      "��N�b�g�V$N�C" );
    dam = 400;
    break;

  case 3:
    sact( ch, victim, pObj,
      "$n�N�����C�C�פW�M���Ĥ߯��M�N�U�طP�x���@�췥���M�ߤ��u$S����p��"
      "$Z�v�����X$N���Ҧ��ʧ@�M�b�ݲM�@����M�@�b�g�V$N���n�`�C$Z" );
    dam = 450;
    break;

  case 4:
    sact( ch, victim, pObj,
      "$n�B��u��M�N�l�O�麡���L�M�a���@�D�M���W�b�Ť��M�ѤW�ӤU�D�E�a"
      "�W�U���M�L���@���M$S�����b�k$Z�p�u$W����$S�m�a$Z�v��g���ӤU�C" );
    dam = 500;
    break;

  case 5:
    sact( ch, victim, pObj,
      "$n���Ϋ���M�ۤƥX�����몺���v�M�|�P�y�ɳ��J�@���g�۳s�����"
      "�Ӥ��i�ӡM�@�D������$n���W��$O�z�X�M�@���u$S���P�}��$Z�v$Y��$Z���@"
      "�{�M��z�F$N�C$Z" );
    dam = 600;
    break;

  case 6:
    sact( ch, victim, pObj,
      "�b��i���԰���$n��M���F�U�ӡM�ǭY�L�H�a���X$R�������$Z�M�}�l��"
      "�F�_�ӡM�u$S�]���R��$Z�v�T������M$n�믫�j���M�����@�b�X�G�g��F"
      "$N$Z�C" );
    dam = 700;
    break;

  case 7:
    sact( ch, victim, pObj,
      "$n����@���M�¦⪺�����$O�����X�M�Φ��@�Υ��j���¶��M�B��"
      "�F�����~�M$S�����b�k$Z���ܨ��@�ۡu$B�Q������$Z�v�ϥX�M$N���T���F"
      "�ӧNŸ�C$Z" );
    dam = 800;
    break;

  case 8:
    sact( ch, victim, pObj,
      "$n����T���M��e�����g�ߡM�������۫���M���ĦӤW�M�j�j����l"
      "Ţ�n�j�a�M$n�ϩ��ܤj�F�ƤQ���M����$R���$Z�g�A���S�M�p�u$R�o��$S�ѭ�"
      "$Z�v���ĦӤU�M��$N$Z���Ƥ��ΡC" );
    dam = 900;
    break;

  case 9:
    sact( ch, victim, pObj,
      "$n�N��𯫤T�̶פ@�M�_���@����~�𮧡M�|�P�}�l�����ܤơM$G���_$B"
      "����$Z�M$y�j�a�_��$Z�M$Y���$b�L��$Z�M$N���B$n�ҨϥX���u$S����$P�]��$Z�v����"
      "�M�u��b�b�Q�ýb�g���F�C$Z" );
    dam = 1000;
    break;
  }

  for ( tt = 0; tt <= level / 33; tt++ )
  {
    if ( !verify_char( victim )
      || !can_damage( victim )
      || victim->in_room != ch->in_room ) break;

    /* �Z�����[�� */
    if ( !IS_NPC( ch )
      && pObj
      && pObj->item_type == ITEM_WEAPON
      && pObj->value[3] == WEAPON_BOW )
    {
      pObj->value[1] = UMIN( pObj->value[1], 1000 );
      pObj->value[2] = UMIN( pObj->value[2], 1000 );

      multi = number_range( pObj->value[1], pObj->value[2] );
    }
    else
    {
      multi = 100;
    }

    value = variation( ( dam * multi / 100 ), 10 );
    value = value + tt * level;
    damage( ch, victim, value, sn, ATTACK_RANDOM );
  }

  RETURN( TRUE );
}

SKILL( cast_fun_wu_blast )
{

  CHAR_DATA * victim = ( CHAR_DATA * ) vo;
  int         choice[] = { 9, 19, 29, 39, 49, 59, 69, 79, 89, 100, -1 };
  int         tt;
  int         multi;
  int         value;
  int         dam = 0;
  OBJ_DATA  * pObj;

  PUSH_FUNCTION( "cast_fun_wu_blast" );

  if ( !( pObj = get_wield( ch ) ) )
  {
    mudlog( LOG_DEBUG, "cast_water_cloud_blast: �Z�����~." );
    RETURN( FALSE );
  }

  level = number_range( level / 2, level );
  switch( dimemsion_position( choice, level, TRUE ) )
  {
  case ERRORCODE:
    mudlog( LOG_DEBUG, "cast_water_cloud_blast: �X�ɤF." );
    RETURN( FALSE );

  case 0:
    sact( ch, victim, pObj,
      "$A             $P ��R�b�k $Z��$R �@��R��$Z$A$A"
      "$n�����}�B�M�b$N���r�P�������F«��{�M�n���e�����@�Ӫ���"
      "�M$n�ߧY���X�A��$O�M�l�e�k��M�Ժ��}���M��i�}�X�G�Ԧ�"
      "�F��ΡM�r�M�@�b�g�V�ѪšM$n�g�X���b�ۤƦ��@��$R�����$Z���ĤW��"
      "�M�b�Ť��L�ۼƩP��M��$N�g���ӥh�C" );
    dam = 450;
    break;

  case 1:
    sact( ch, victim, pObj,
      "$A              $P ��R�b�k $Z�� $G�G����$Z$A$A"
      "$n�N�Ҧ��ʧ@��C�M���ۤv�����ߩӨ�$N�L�����r�P�����M��í�B"
      "��M�N���ߩ�b�U�L�Mí�p���s�M�H���X�G��$W�Ƚb$Z�M�f�b$O"
      "�W�M�u��@���M���}�J�g�T���M$W���b$Z�P�ɯe�g�ӥX�M�e�W���ۮ�l�M"
      "$W���b$Z�p�Ⱖ$R����$Z��������M�����٦ۦ�������ʡM��´���@�D�j�j"
      "��$R���$S���b$Z�M��$N���g�ӥh�C" );
    dam = 500;
    break;

  case 2:
    sact( ch, victim, pObj,
      "$A               $P��R�b�k $Z�� $Y�}��T�g$A$Z$A"
      "$n�s�}�f�b�M�g�X�Ĥ@�K�b�M�b�b��g�X���P�ɰ��W�A�g�X�ĤG�b�M"
      "���۲ĤT�b���H�ӥX�M�T�b�X�G�O�s�b�@�_���@��W�Ū��b�@�ˡM�S"
      "�p��۪������ڪ��}����ġM������$N������ĥh�M�@�b���ۤ@�b"
      "�M�y��$N���W$l�Q�����F�T���M�X�G�n�F$N�����R�C" );
    dam = 600;
    break;

  case 3:
    sact( ch, victim, pObj,
      "$A               $P��R�b�k$Z ��$B ��C�|��$Z$A$A"
      "��ۤѤW���ն��M$n�߱�����ı���P�F�_�ӡM���e��$N�ݰ_�Ӥ]ı�o"
      "�i�R�M�I�i�}$P��R���k$Z�{�}$N���P�R��������M$n�Y�]���^�a�ƶh"
      "�ӥh�M�ߤ��u�Q�y�����C�s�����M�A�]���Q�԰��M�N�b$n�@���P�Ӥ�"
      "�ɡM$N��������$n���I��M���j���hı�G��$n���o���ϥX���ۡM��"
      "�F�t�X�߱��M$P��R�b�k$Z�����u�����u$B��C�|��$Z�v�ϥX�M$N�Q��p��"
      "�Ӫ�$S�����$Z�]���F�C" );
    dam = 700;
    break;

  case 4:
    sact( ch, victim, pObj,
      "$P              $P��R�b�k $Z��$P �m�񤭼A$Z$A$A"
      "����$N���ФH�����M$n�u�४��k�סM�⤣�X�ɶ��ө�X�I�W��$O"
      "�����M�[�W�Q�_�F�Q�Ѹ�p���n�[�M�Q���W�䧮�a�ުP�M�̫��ٳQ��"
      "��M�@�ѫ��Ѥߤ��ɰ_�M������$R�d�ˡB����B�L�`$Z�������ȵ��b��"
      "���M$n�N�L�B�o�������q�q�[�Ѧb$N���W�M�s�b�]�i�o�ޤF�M��"
      "����$O�|�F�_�ӡM������$N��$l�W�A�F���U�M�A�學�q"
      "�ӥX�M�b�����Ӯg�U�M��´�����R��$R���m���$Z�C" );
    dam = 800;
    break;

  case 5:
    sact( ch, victim, pObj,
      "$P              $P��R�b�k $Z�� $R������$Z$A$A"
      "$n�ϥX$P��R���k$Z�ܷ��@���u$Y�ۻ�R��$Z�v�M���Τ@�������M����$N��"
      "�������M�Φ��@��$S���~�P���]�}$Z�M�N$N�ʦb�䤤�M����$n�P���s"
      "�}�f�b�M�g�V�ѪšM���b�b�Ť���ۤ����M�ۤƥX���j��$R�P�����$Z�M"
      "�e���l�@���M�������j��$R���y$Z�q�Ѧӭ��M�{�V���]�}���M$N"
      "�]���L�k�{���M�u�಴�C�C�Q�{�C" );
    dam = 900;
    break;

  case 6:
    sact( ch, victim, pObj,
      "$A$P              ��R�b�k$Z ��$S �g��C��$Z$A$A"
      "$n��������M�κA�Y�g�M���Y���v�M�����W�U���o�X�L�H�ۤ񪺤��r"
      "���N�M$n�N���W�쥻���b�U�q�q���M�q�h���ǥX�t�@�ӽb�U�M�U��"
      "���O�馨�C�I���u�b�M�N�b�@�ԡM�����@�K�K�f�ۼ@�r���C���r�b�M"
      "���Ŧ⪺�b�Y�{�{�o�G�M�M��$n�B���몺�N���M����ʳ諸�r�b���H"
      "ı�o$N�������j�������F�C" );
    dam = 1000;
    break;

  case 7:
    sact( ch, victim, pObj,
      "$A$P               ��R�b�k$Z ��$Y ����K��$Z$A$A"
      "$n���E�u��M�N$P��R���k$Z�o�����O�v�ɭP�M�b�Գ�������ۦp�M����"
      "���N$N��b���̡M�b�u��ֳt�y��M���ηU�ӷU�֤��U�M$N�w�g"
      "�Q�d�o���Y��V�F�M�g�ۤ�$n�����v�w�g�Ʀ��F$R�C$G�m$B��$Y��$Z�M�b���C��"
      "������դ���M�@��$S���l�ѵ�$Z�ϥX�M�۽b�p�B��q�|���K��g�V$N"
      "�M�u���w���@�ݡM$N���W���F�K�K�b�M�K�K�e��$N������C" );
    dam = 1100;
    break;

  case 8:
    sact( ch, victim, pObj,
      "$A$P              ��R�b�k$Z ��$R ��R�E��$A$Z$A"
      "$n�N��������¬����M���P�����ʳ��M�_���@���P�~�ɪ��pô�M����"
      "�a�M$S��$Z���y�ʡM$Y��$Z���ܤơM���������M�J�߲������M�~�ɪ��@��"
      "�ܤƳ���M���P����M$N���@�|�@�ʡB�٦��ܤơM���b�x����"
      "���M$n�r�M�C���@���M$N�Q��թ�����M�@�s�K�b�g�X�M�b�b"
      "����ǤF$N���h���M�K�b����M$N�w�g�׵L�i�סM�ܩR���ĤE�b"
      "�����g���F$N��$l�C" );
    dam = 1200;
    break;

  case 9:
    sact( ch, victim, pObj,
      "$A$P              ��R�b�k $Z�� $R�d��$S�P$Y�R$A$Z$A"
      "$n�B�_$P��R�b�k$Z���ߤ@���ߪk--$Y�u$G�@�L�Ҧ�$Z�M$B�L�Ҥ���$Y�v$Z�M�N�u����"
      "�i�}���M�W����l���}���y��$S���@�T$Z�M�����@���u��M��í�}�`�M"
      "����@���}���M�l��y�ɯe�ĦӥX�M�d�D�U�D��b���O�Ʀ��d�U����"
      "�P��$R���$Z�M�R�X�U�ΦU��޲��ҧ����R���M�ǻ������A�G���R��$N"
      "�ݱo���m���ɡM���������D�ۤv�w�g�Q��b�αo��L�����F�C" );
    dam = 1400;
    break;
  }

  for ( tt = 0; tt <= level / 20; tt++ )
  {
    if ( !verify_char( victim )
      || !can_damage( victim )
      || victim->in_room != ch->in_room ) break;

    /* �Z�����[�� */
    if ( !IS_NPC( ch )
      && pObj
      && pObj->item_type == ITEM_WEAPON
      && pObj->value[3] == WEAPON_BOW )
    {
      pObj->value[1] = UMIN( pObj->value[1], 1000 );
      pObj->value[2] = UMIN( pObj->value[2], 1000 );

      multi = number_range( pObj->value[1], pObj->value[2] );
    }
    else
    {
      multi = 100;
    }

    value = variation( ( dam * multi / 100 ), 10 );
    value = value + tt * level;
    damage( ch, victim, value, sn, ATTACK_RANDOM );
  }

  RETURN( TRUE );
}

SKILL( cast_make_sword )
{
  OBJ_INDEX_DATA * pObj;
  OBJ_DATA       * pSword;

  PUSH_FUNCTION( "cast_make_sword" );

  if ( !ch || !verify_char( ch ) || !( pObj = get_obj_index( 410 ) ) )
    RETURN( FALSE );

  if ( !( pSword = create_object( pObj, ch->level ) ) )
  {
    send_to_char( "�L�k�ХX�]�k�C�T\n\r", ch );
    RETURN( FALSE );
  }

  pSword->timer    = 3 + ( level / 10 );
  pSword->value[1] = UMAX( 100, 100 + level / 5 );
  pSword->value[2] = UMAX( 100, 100 + level / 2  );

  if ( ( ch->carry_number + 1 > can_carry_n( ch ) )
    || ( ch->carry_weight + get_obj_weight( pSword ) > can_carry_w( ch ) ) )
    obj_to_room( pSword, ch->in_room );
  else
    obj_to_char( pSword, ch );

  act( "�@�}�{���L��M$p�q$n����W���E�X�ӡT", ch, pSword, NULL, TO_ALL );

  RETURN( TRUE );
}

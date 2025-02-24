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
#include <string.h>
#include "merc.h"

/* �ϰ��� */
int     attr_gain       args( ( CHAR_DATA *, int ) );
void    mobile_update   args( ( void ) );
void    weather_update  args( ( void ) );
void    char_update     args( ( void ) );
void    obj_update      args( ( void ) );
void    aggr_update     args( ( void ) );
void    food_update     args( ( void ) );
void    time_update     args( ( void ) );
void    login_update    args( ( void ) );
void    bank_update     args( ( void ) );
void    donate_update   args( ( void ) );
void    jail_update     args( ( void ) );
void    tributal_update args( ( void ) );
void    steal_update    args( ( void ) );
void    fail_update     args( ( void ) );
void    talk_update     args( ( void ) );
void    check_train     args( ( CHAR_DATA * ) );
void    variable_update args( ( void ) );
void    dosage_update   args( ( void ) );
void    increase_update args( ( void ) );

#define HIT_GAIN        1
#define MANA_GAIN       2
#define MOVE_GAIN       3

/* ���@���a���� */
void advance_level( CHAR_DATA * ch )
{
  int  add_hp;
  int  add_mana;
  int  add_move;
  int  add_prac;
  int  mana_factor;
  int  move_factor;
  char buf1[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];
  CLASS_DATA * pClass;

  PUSH_FUNCTION( "advance_level" );

  /* ��F�^���L�k�~��ɯ� */
  if ( ch->level > LEVEL_HERO || !( pClass = ch->class ) ) RETURN_NULL();

  /* �W�[�k�O���B�z */
  add_mana     = ( pClass->fMana )
    ? add_mana = number_range( 2, ( 2 * get_curr_int( ch )
    + get_curr_wis( ch ) ) / 6 ) : 1;

  mana_factor  = ( pClass->factor[INT_ITEM] + pClass->factor[WIS_ITEM] );
  add_mana     = UMIN( 10000, ( add_mana * 200 ) ) / UMAX( 1, mana_factor );
  add_mana    += ch->level / 10;

  /* �W�[�ͩR�O���B�z */
  add_hp       = number_range( 1 , get_curr_con( ch ) / 10 );
  add_hp       = ( ch->level / 2 ) + ( 2 * get_curr_con( ch ) )
    + number_range( 10 * add_hp , 12 * add_hp );
  add_hp       = ( add_hp * 100 ) / UMAX( 1, pClass->factor[CON_ITEM] );

  /* �W�[��O���B�z */
  add_move     = number_range( 5,
               ( get_curr_con( ch ) + get_curr_dex( ch ) ) / 6 );
  move_factor  = pClass->factor[STR_ITEM] + pClass->factor[DEX_ITEM];

  add_move     = UMIN( 10000, ( add_move * 200 ) ) / UMAX( 1, move_factor );
  add_move    += ch->level / 10;

  /* �W�[�m���I�ƪ��B�z */
  add_prac     = number_range( ch->level * 9 , ch->level * 10 );

  add_hp       = UMAX(   1, add_hp    );
  add_mana     = UMAX(   0, add_mana  );
  add_move     = UMAX(   1, add_move  );
  add_move     = UMIN(   add_move, 30 );
  add_prac     = UMAX(   1, add_prac  );

  ch->max_hit   = UMIN( MaxHit     , ch->max_hit  + add_hp   );
  ch->max_mana  = UMIN( MaxMana    , ch->max_mana + add_mana );
  ch->max_move  = UMIN( MaxMove    , ch->max_move + add_move );
  ch->practice  = UMIN( MaxPractice, ch->practice + add_prac );

  if ( !IS_NPC(ch) ) REMOVE_BIT( ch->act, PLR_BOUGHT_PET );

  print_to_char( ch ,
    "\e[1;33m�A��o�F: �ͩR�O %d/%d �k�O %d/%d ��O %d/%d "
    "�ǲ��I�� %d/%d \e[0m\n\r"
      , add_hp, get_curr_hit( ch ), add_mana, get_curr_mana( ch ), add_move
      , get_curr_move( ch ), add_prac, ch->practice  );

  if ( ( ch->level % 10 ) == 0 )
  {
    if ( add_firman( ch, 1 ) > 0 )
      send_to_char( "\e[1;32m���~�A�e�A�@�i�K�����P�T\e[0m\n\r", ch );
  }

  /* �o�X��¾��ĵ�i */
  if ( ch->level > ForceLevel
    && ch->level < LEVEL_HERO
    && pClass == class_demos )
  {
    send_to_char(
      "�A���ֿ��y�a����¾�M���`�S���Ǧn�M�ݨӧA�u�O��s�骺�ơC\n\r" , ch );

    ch->class = class_force;
    set_title( ch, class_force->title );
  }

  else if ( ch->class && ch->level >= ch->class->warn )
    send_to_char( "���H�A�M�A�i�H�Ҽ{��¾�o�C\n\r" , ch );

  /* �O�_�u�@���n���} */
  if ( ch->level == AngelLevel )
  {
    send_to_char( "�A���u�@��(\e[1;32mangel\e[0m)�w�g���}�F�T\n\r", ch );
    REMOVE_BIT( ch->act, PLR_ANGEL );
  }

  check_train( ch );

  /* ��F�^�� */
  if ( ch->level == LEVEL_HERO )
  {
    chinese_number( hero_count() + 1, buf2 );

    sprintf( buf1, "\e[1;36m���ڭ̮��H%s����%s��%s��^���T\e[0m\n\r"
      , mob_name( NULL, ch ), mud_name, buf2 );

    send_to_all_char( buf1 );

    act( "���H�A����$t��$T��^���T", ch, mud_name, buf2, TO_CHAR );

    /* �W�[�B�~�ݩ� */
    add_hero_bonus( ch );

    /* �W�[�^�� */
    hero_add( ch );

    /* �g�J�ɮ� */
    update_hero();
  }

  RETURN_NULL();
}

void check_train( CHAR_DATA * ch )
{
  bool Train = FALSE;

  PUSH_FUNCTION( "check_train" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "check_train: �ʥF�ӷ�." );
    RETURN_NULL();
  }

  if ( IS_NPC( ch ) ) RETURN_NULL();

  if ( ch->practice >= get_str_train( ch ) ) Train = TRUE;
  if ( ch->practice >= get_wis_train( ch ) ) Train = TRUE;
  if ( ch->practice >= get_int_train( ch ) ) Train = TRUE;
  if ( ch->practice >= get_dex_train( ch ) ) Train = TRUE;
  if ( ch->practice >= get_con_train( ch ) ) Train = TRUE;

  if ( Train ) send_to_char( "\n\r\n\r\e[1;34m�A���@�ǰV�m�I�ơM�Ч�V�m�v�ɯ�"
    "(train)�A���ݩʧa�T\n\r\n\r\e[0m", ch );

  RETURN_NULL();
}

void gain_exp( CHAR_DATA * ch, int gain )
{
  PUSH_FUNCTION( "gain_exp" );

  if ( ( IS_NPC( ch ) && !ch->boss ) || ch->level >= LEVEL_HERO )
    RETURN_NULL();

  ch->exp = UMAX( 1, ch->exp + gain );

  while ( ch->exp >= kill_table[ URANGE( 1 , ch->level, MAX_LEVEL -1 )].exp )
  {
    send_to_char( "\a\a\a\e[1;31m�z�T���H�A�T�A�ɯ��o�T\e[0m\n\r", ch );
    act( "$n�ԾĪ��W�m��o�F�^���M���@�F���šC", ch, NULL, NULL, TO_ROOM );

    ch->exp -= kill_table[ URANGE( 1, ch->level, MAX_LEVEL -1 ) ].exp;
    if ( ++ch->level == LEVEL_HERO ) ch->exp = 0;

    if ( !IS_NPC( ch ) ) advance_level( ch );

    if ( ch->level >= LEVEL_HERO ) break;
  }

  if ( IS_NPC( ch ) ) RETURN_NULL();

  RETURN_NULL();
}

int attr_gain( CHAR_DATA * ch , int type )
{
  int         gain    = 0;
  int         current = 0;
  int         max     = 0;

  PUSH_FUNCTION( "attr_gain" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "attr_gain: �ӷ������T." );
    RETURN( 0 );
  }

  switch( type )
  {
  default:
    mudlog( LOG_DEBUG, "attr_gain: ���~���ﶵ %d.", type );
    RETURN( 0 );

  case HIT_GAIN:
    max = get_curr_hit( ch ) - ch->hit;
    break;

  case MANA_GAIN:
    max = get_curr_mana( ch ) - ch->mana;
    break;

  case MOVE_GAIN:
    max = get_curr_move( ch ) - ch->move;
    break;
  }

  if ( IS_NPC( ch ) )
  {
    gain = ch->level;
  }

  else
  {
    switch( type )
    {
    case HIT_GAIN:

      gain = URANGE( 10 , ch->level , 150 );
      current = get_curr_con( ch ) * 4;
      break;

    case MANA_GAIN:

      gain = URANGE( 5 , ch->level / 2 , 100 );
      current = get_curr_int( ch ) * 2;
      break;

    case MOVE_GAIN:

      gain = URANGE( 5 , ch->level, 100 );
      current = get_curr_dex( ch ) * 2;
      break;
    }

    switch ( ch->position )
    {
    case POS_SLEEPING: gain += current;     break;
    case POS_RESTING:  gain += current / 2; break;
    }

    if ( ch->pcdata->condition[COND_FULL]   == 0 ) gain /= 2;
    if ( ch->pcdata->condition[COND_THIRST] == 0 ) gain /= 2;
  }

  /* ���� recall �I�[�j�^�_ */
  if ( ch->in_room && ch->club && ch->club == ch->in_room->club )
    gain *= ClubAttrGain;

  if ( is_affected( ch, SLOT_POISON ) ) gain /= 4;

  RETURN( UMIN( gain, max ) );
}

void gain_condition( CHAR_DATA * ch, int iCond, int value )
{
  int condition;

  PUSH_FUNCTION( "gain_condition" );

  if ( value == 0
    || IS_NPC(ch)
    || !ch->pcdata
    || ch->level > LEVEL_HERO ) RETURN_NULL();

  condition                    = ch->pcdata->condition[iCond];
  ch->pcdata->condition[iCond] = URANGE( 0, condition + value, 100 );

  if ( ch->pcdata->condition[iCond] == 0 )
  {
    switch ( iCond )
    {
    default:
      mudlog( LOG_DEBUG, "gain_condition: ���~���κA %d.", iCond );
      break;

    case COND_FULL:
      send_to_char( "\e[33m�B�P�P�@�n�M�A���{�l�j�F�C\e[0m\n\r", ch );
      break;

    case COND_THIRST:
      send_to_char( "\e[33m�Aı�o�f������M���G�i�H�ܤU�@����C\e[0m\n\r"
        , ch );
      break;

    case COND_DRUNK:
      if ( condition ) send_to_char( "\e[1;37m�A�Y�W�P�P�����F�M"
        "�w�g�M���F�C\e[0m\n\r", ch );
      break;
    }
  }
  RETURN_NULL();
}

void mobile_update( void )
{
  CHAR_DATA * ch;
  CHAR_DATA * rch;
  CHAR_DATA * rider;
  EXIT_DATA * pexit;
  OBJ_DATA  * obj;
  OBJ_DATA  * obj_best;
  int         door;
  int         max;
  bool        found;

  PUSH_FUNCTION( "mobile_update" );

  for ( ch = char_list; ch; ch = ch->next )
  {
    if ( !verify_char( ch )
      || !IS_NPC( ch )
      || !ch->in_room
      || is_affected( ch, SLOT_CHARM_PERSON ) )
      continue;

    /* ���M */
    if ( ( rider = ch->mount_by )
      && ch->in_room
      && ch->in_room != rider->in_room )
    {
      if ( ch->fighting ) stop_fighting( ch , TRUE );

      act( "$n�l�H�D�H���F�C", ch , NULL, NULL , TO_ROOM );
      char_from_room( ch );
      char_to_room( ch, rider->in_room );

      act( "$N�l�H$n�Ө�o�̡C", rider, NULL, ch, TO_ALL );
    }

    if ( ( ch->spec_fun && ( ( * ch->spec_fun ) ( ch ) ) ) ) continue;

    if ( !verify_char( ch )
      || ch->position < POS_STANDING
      || !ch->in_room ) continue;

    if ( ch->in_room->area->nplayer > 0 )
    {
      mprog_random_trigger( ch );
      if ( !verify_char( ch ) || ch->position < POS_STANDING ) continue;
    }

    if ( IS_SET( ch->act, ACT_SCAVENGER )
      && ch->in_room->contents
      && !ch->mount_by
      && !ch->boss
      && number_bits( 2 ) == 0 )
    {
      max      = 1;
      obj_best = NULL;

      for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
      {
        if ( obj->Takeable && obj->cost > max )
        {
          obj_best = obj;
          max      = obj->cost;
        }
      }

      if ( obj_best )
      {
        obj_from_room( obj_best );
        obj_to_char( obj_best, ch );
        act( "$n����߰_$p���ۤv���W�C", ch, obj_best, NULL, TO_ROOM );
      }
    }

    if ( !IS_SET( ch->act, ACT_SENTINEL )
      && !ch->talk_buf
      && !ch->mount_by
      && !ch->boss
      && ( door = number_bits( 5 ) ) < DIR_MAX
      && ( pexit = ch->in_room->exit[door] )
      && pexit->to_room
      && !is_station( pexit->to_room )
      && !pexit->closed
      && pexit->to_room->NoMob == FALSE
      && ( !IS_SET(ch->act, ACT_STAY_AREA )
        || pexit->to_room->area == ch->in_room->area ) )
    {
      move_char( ch, door );
      if ( !verify_char( ch ) || ch->position < POS_STANDING ) continue;
    }

    if ( ch->hit < ( get_curr_hit( ch ) / 4 )
      && ( door = number_bits( 4 ) ) < DIR_MAX
      && ( pexit = ch->in_room->exit[door] )
      && pexit->to_room
      && ch->in_room != pexit->to_room
      && !is_station( pexit->to_room )
      && !ch->boss
      && !pexit->closed
      && pexit->to_room->NoMob == FALSE )
    {
      found = FALSE;
      for ( rch = pexit->to_room->people; rch; rch = rch->next_in_room )
      {
        if ( !IS_NPC( rch ) )
        {
          found = TRUE;
          break;
        }
      }

      if ( !found )
      {
        if ( ch->mount_by ) unmount_char( ch->mount_by, ch );

        if ( ch->talk_buf )
        {
          free_string( ch->talk_buf );
          ch->talk_buf  = NULL;
          ch->talk_mode = MODE_ABSENT;
        }

        move_char( ch, door );
      }
    }
  }
  RETURN_NULL();
}

void variable_update( void )
{
  VARIABLE_DATA * pVar;

  PUSH_FUNCTION( "variable_update" );

  for ( pVar = variable_list; pVar; pVar = pVar->next )
    if ( pVar->hours > 0 && --pVar->hours == 0 )
      pVar->value = !pVar->value;

  RETURN_NULL();
}

void talk_update( void )
{
  static int  timer = PULSE_PER_TALK;
  CHAR_DATA * ch;
  char      * pString;
  char        arg1[MAX_INPUT_LENGTH];
  char        arg2[MAX_INPUT_LENGTH];

  PUSH_FUNCTION( "talk_update" );

  if ( --timer > 0 ) RETURN_NULL();
  timer = PULSE_PER_TALK;

  for ( ch = char_list; ch; ch = ch->next )
  {
    /* �ˬd�O�_����X�w�İ� */
    if ( !verify_char( ch ) || !ch->talk_buf || !IS_NPC( ch ) ) continue;

    /* �Y�S���Ҧb�a�άO�b���� */
    if ( !ch->in_room || ch->position == POS_FIGHTING )
    {
      free_string( ch->talk_buf );
      ch->talk_buf  = NULL;
      ch->talk_mode = MODE_ABSENT;
      continue;
    }

    switch( ch->talk_mode )
    {
    default:
      mudlog( LOG_DEBUG, "talk_update: ��X����X %d���~.", ch->talk_mode );
      free_string( ch->talk_buf );
      ch->talk_mode = MODE_ABSENT;
      break;

    case MODE_SAY:

      pString = one_statement( ch->talk_buf, arg1 );

      if ( *pString )
      {
        str_cpy( arg2, pString );
        free_string( ch->talk_buf );
        ch->talk_buf = str_dup( arg2 );
      }
      else
      {
        free_string( ch->talk_buf );
        ch->talk_buf  = NULL;
        ch->talk_mode = MODE_ABSENT;
      }

      if ( arg1[0] ) do_say( ch, arg1 );
      break;

    case MODE_CHAT:

      pString = one_statement( ch->talk_buf, arg1 );

      if ( *pString )
      {
        str_cpy( arg2, pString );
        free_string( ch->talk_buf );
        ch->talk_buf = str_dup( arg2 );
      }
      else
      {
        free_string( ch->talk_buf );
        ch->talk_buf  = NULL;
        ch->talk_mode = MODE_ABSENT;
      }

      if ( arg1[0] )
      {
        ChatRecord = FALSE;
        do_chat( ch, arg1 );
        ChatRecord = TRUE;
      }
      break;
    }
  }

  RETURN_NULL();
}

void fail_update( void )
{
  ROOM_INDEX_DATA * location;
  DESCRIPTOR_DATA * man;
  CHAR_DATA       * ch;

  PUSH_FUNCTION( "fail_update" );

  for ( man = descriptor_list; man; man = man->next )
  {
    if ( verify_desc( man )
      && man->connected == CON_PLAYING
      && verify_char( ( ch = man->character ) )
      && ch->failed > 0
      && ch->jail <= 0
      && ch->in_room )
    {
      if ( --ch->failed <= 0 )
      {
        if ( ch->position == POS_STANDING )
        {
          if ( !( location = get_hometown( ch ) ) )
          {
            send_to_char( "�~���@�ɤӶ·t�F�M�A���ܫݦb�o�̧a�T\n\r", ch );
            continue;
          }

          if ( ch->fighting ) stop_fighting( ch, TRUE );

          if ( ch->mount    ) unmount_char( ch, ch->mount    );
          if ( ch->mount_by ) unmount_char( ch->mount_by, ch );

          /* �ץ��p�� */
          if ( ch->boss  ) spirit_from_char( ch );

          send_to_char( "�ޡM�A�i�H���Ϋ�L�o�M�Ʊ�[�o�a�T\n\r", ch );
          act( "����$n�i�H���Ϋ�L�o�C", ch, NULL, NULL, TO_ROOM );
          char_from_room( ch );
          char_to_room( ch, location );
          act( "$n�b�o�̶}�l���T����F�C", ch, NULL, NULL, TO_ROOM );
          do_look( ch, "auto" );

          /* �M���l�ܬ����I */
          clear_trace( ch, TRUE );
        }

        else
        {
          send_to_char( "�ޡT�A�i�H������L�o�T\n\r\a\a", ch );
        }
      }
    }
  }

  RETURN_NULL();
}

void tribunal_update( void )
{
  DESCRIPTOR_DATA * man;
  CHAR_DATA       * ch;

  PUSH_FUNCTION( "tribunal_update" );

  for ( man = descriptor_list; man; man = man->next )
  {
    if ( verify_desc( man )
    && man->connected == CON_PLAYING
    && verify_char( ( ch = man->character ) )
    && ch->tribunal_timer > 0
    && ch->in_room )
    {
      if ( --ch->tribunal_timer <= 0 )
      {
        if ( IS_SET( ch->act, PLR_NO_TELL ) )
        {
          send_to_char( "�A�S�i�H�~��ϥ� \e[1;32mtell\e[0m ���O�F�T\n\r", ch );
          REMOVE_BIT( ch->act, PLR_NO_TELL );
        }

        else if ( IS_SET( ch->act, PLR_SILENCE ) )
        {
          send_to_char( "�A�S�i�H�j�n���ܤF�T\n\r", ch );
          REMOVE_BIT( ch->act, PLR_SILENCE );
        }
      }
    }
  }

  RETURN_NULL();
}

void steal_update( void )
{
  DESCRIPTOR_DATA * man;
  CHAR_DATA       * ch;

  PUSH_FUNCTION( "steal_update" );

  for ( man = descriptor_list; man; man = man->next )
  {
    if ( verify_desc( man )
    && man->connected == CON_PLAYING
    && verify_char( ( ch = man->character ) )
    && ch->pcdata
    && ch->pcdata->steal_panity > 0
    && ch->in_room )
      if ( --ch->pcdata->steal_panity <= 0 )
        act( "$n�S�}�lı�o���o�M�Q���F��F�T", ch, NULL, NULL, TO_CHAR );
  }

  RETURN_NULL();
}
void jail_update( void )
{
  ROOM_INDEX_DATA * location;
  DESCRIPTOR_DATA * man;
  CHAR_DATA       * ch;

  PUSH_FUNCTION( "jail_update" );

  for ( man = descriptor_list; man; man = man->next )
  {
    if ( verify_desc( man )
      && man->connected == CON_PLAYING
      && verify_char( ( ch = man->character ) )
      && ch->jail > 0
      && ch->in_room )
    {
      if ( --ch->jail <= 0 )
      {
        if ( ch->position == POS_STANDING )
        {
          if ( !( location = get_hometown( ch ) ) )
          {
            send_to_char( "�~���@�ɤӶ·t�F�M�A���ܫݦb�o�̧a�T\n\r", ch );
            continue;
          }

          if ( ch->fighting ) stop_fighting( ch, TRUE );

          if ( ch->mount    ) unmount_char( ch, ch->mount    );
          if ( ch->mount_by ) unmount_char( ch->mount_by, ch );

          /* �ץ��p�� */
          if ( ch->boss  ) spirit_from_char( ch );

          send_to_char( "�ޡM�A�i�H�X���o�M�Ʊ�A�n�n���H�T\n\r", ch );
          act( "����$n�i�H�X���o�C", ch, NULL, NULL, TO_ROOM );
          char_from_room( ch );
          char_to_room( ch, location );
          act( "$n�b�o�̶}�l�q�Y���H�C", ch, NULL, NULL, TO_ROOM );
          do_look( ch, "auto" );

          /* �M���l�ܬ����I */
          clear_trace( ch, TRUE );
        }

        else
        {
          send_to_char( "�ޡT�A�i�H�X���o�M�ٷQ�Y�c���ڡT\n\r\a\a", ch );
        }
      }
    }
  }

  RETURN_NULL();
}

void foggy_update( int type )
{
  AREA_DATA        * pArea;
  AREA_DATA        * zArea;
  ROOM_INDEX_DATA  * pRoom;
  EXIT_DATA        * pExit;
  char               buf[MAX_STRING_LENGTH];
  int                door;
  int                total;
  int                chance;

  PUSH_FUNCTION( "foggy_update" );

  if ( FoggyRoom <= 0 ) RETURN_NULL();

  switch( type )
  {
  default:
    mudlog( LOG_DEBUG, "foggy_update: ���A���~ %d.", type );
    break;

  case FOGGY_SET:

    if ( FoggyArea ) foggy_update( FOGGY_CLEAR );

    for ( total = 0, pArea = area_first; pArea; pArea = pArea->next )
      if ( pArea->foggy > 0 ) total += pArea->foggy;

    if ( total <= 0 ) break;

    chance = number_range( 1, total );

    for ( zArea = NULL, pArea = area_first; pArea; pArea = pArea->next )
    {
      if ( pArea->foggy > 0 && ( chance -= pArea->foggy ) <= 0 )
      {
        zArea = pArea;
        break;
      }
    }

    if ( zArea )
    {
      FoggyArea = zArea;

      sprintf( buf, "%s\e[0m�����M�X�{�@�}�@���M�Цb���ϰ쪺���H�h�[�`�N�T"
        , FoggyArea->name );

      talk_channel_2( buf, CHANNEL_WEATHER, "" );

      for ( pRoom = zArea->list; pRoom; pRoom = pRoom->next_in_area )
      {
        for ( door = 0; door < DIR_MAX; door++ )
        {
          if ( ( pExit = pRoom->exit[door] )
            && number_range( 1, 100 ) <= FoggyRoom )
          {
            pExit->foggy = TRUE;
          }
        }
      }
    }

    break;

  case FOGGY_CLEAR:

    if ( FoggyArea )
    {
      for ( pRoom = FoggyArea->list; pRoom; pRoom = pRoom->next_in_area )
      {
        for ( door = 0; door < DIR_MAX; door++ )
        {
          if ( ( pExit = pRoom->exit[door] ) && pExit->foggy )
            pExit->foggy = FALSE;
        }
      }

      sprintf( buf, "%s\e[0m�X�{���@���w�g�C�C�h�h�F�T", FoggyArea->name );

      talk_channel_2( buf, CHANNEL_WEATHER, "" );

      FoggyArea = NULL;
    }

    break;
  }

  RETURN_NULL();
}

/* ���� */
void doante_update( void )
{
  CHAR_DATA * ch;

  PUSH_FUNCTION( "donate_update" );

  if ( DonateLock ) RETURN_NULL();

  for ( ch = char_list; ch; ch = ch->next )
  {
    if ( verify_char( ch ) && ch->donate > 0 && --ch->donate <= 0 )
    {
      act( "���H�A�M�A���A����$t����|�ʷ��A���]���T"
        , ch, mud_name, NULL, TO_CHAR );
    }
  }

  RETURN_NULL();
}

/* �p��Ȧ�s�ڧQ�� */
void bank_update( void )
{
  CHAR_DATA * ch;
  int         profit;

  PUSH_FUNCTION( "bank_update" );

  for ( ch = char_list; ch; ch = ch->next )
  {
    if ( verify_char( ch ) && ch->pcdata ) ch->pcdata->pills = 0;

    if ( !verify_char( ch )
      || IS_NPC( ch )
      || ( profit = ch->bank ) < BankProfit
      || ch->bank >= BankThreshold
      || over_scale( ch ) ) continue;

    if ( ( ch->bank += ch->bank / BankProfit ) < 0 ) ch->bank = profit;
    act( "�S�O�s���@�Ѷ}�l�M$t$0�Ȧ�o���A�Q���T", ch, mud_name, NULL, TO_CHAR );
  }

  RETURN_NULL();
}

/* �Ī����q */
void dosage_update( void )
{
  CHAR_DATA * ch;
  bool        poisoned;
  int         percent;
  int         damage;

  PUSH_FUNCTION( "dosage_update" );

  for ( ch = char_list; ch; ch = ch->next )
  {
    if ( verify_char( ch )
      && !IS_NPC( ch )
      && !is_dead( ch )
      && ch->pcdata
      && ( ( percent = ch->pcdata->dosage - DosageLimit ) > 0 ) )
    {
      if ( ( damage = get_curr_hit( ch ) * percent / 100 ) <= 0 ) continue;

      ch->hit = UMAX( 1, ch->hit - damage );
      send_to_char( "�ѩ��餺�Ī����r�ʵo�@�M�A�C�Cı�o�V�ӶV���٨ơT\n\r", ch );
    }
  }

  for ( ch = char_list; ch; ch = ch->next )
  {
    if ( verify_char( ch ) && ch->pcdata )
    {
      poisoned = ch->pcdata->dosage >= DosageLimit ? TRUE : FALSE;
      ch->pcdata->dosage = UMAX( 0, ch->pcdata->dosage - DosageUpdate );

      if ( poisoned && ch->pcdata->dosage < DosageLimit )
      {
        send_to_char( "�A�Ī����r�����εy�y�h�F�T\n\r", ch );
      }
    }
  }

  RETURN_NULL();
}

/* ���ܤѮ� */
void weather_update( void )
{
  char              buf[MAX_STRING_LENGTH];
  char              msg[MAX_STRING_LENGTH];
  CLUB_DATA       * pClub;
  CHAR_DATA       * ch;
  DESCRIPTOR_DATA * pDesc;
  int               diff;

  PUSH_FUNCTION( "weather_update" );

  buf[0] = '\x0';
  switch ( ++time_info.hour )
  {
  case  5:
    foggy_update( FOGGY_SET );
    weather_info.sunlight = SUN_LIGHT;
    str_cat( buf, "\e[1;32m�������ڥs�n���_�����M������աC\e[0m" );
    break;

  case  7:
    foggy_update( FOGGY_CLEAR );
    weather_info.sunlight = SUN_RISE;
    str_cat( buf, "\e[1;33m���B�H�a���a��͸ͫ_�_���ϡC\e[0m" );
    break;

  case 12:

    /* �ƥ�Ĳ�o */
    event_update();

    str_cat( buf, "\e[1;33m�w�g���Ȯɤ��F�M���ӧ�a�ȴ̦Y�Ӷ��C\e[0m" );
    break;

  case 18:
    weather_info.sunlight = SUN_SET;
    str_cat( buf, "\e[1;41m�Ӷ��C�C���q���丨�U�h�C\e[0m" );
    break;

  case 19:
    str_cat( buf, "\e[1;33m�Ѧ⺥���a�t�F�U�ӡM�O���h�G�F�_�ӡC\e[0m" );
    break;

  case 20:
    weather_info.sunlight = SUN_DARK;
    str_cat( buf, "\e[1;46m�·t���W����M�]�߶}�lŢ�n�j�a�C\e[0m" );
    break;

  case 22:
    str_cat( buf, "\e[1;33m�A�oı�H�̺��������ש��p�M�Τj�Yı�h�F�C\e[0m" );
    break;

  case 24:

    str_cat( buf, "\e[1;46m����̻��D�R�Ѱ�����M�p�ߤ���C\e[0m" );
    time_info.hour = 0;
    time_info.day++;

    /* �W�� */
    for ( pDesc = descriptor_list; pDesc; pDesc = pDesc->next )
    {
      if ( verify_desc( pDesc )
        && pDesc->connected == CON_PLAYING
        && verify_char( ( ch = pDesc->character ) )
        && ch->pcdata
        && ch->pcdata->rechristen <= 0 )
        ch->pcdata->rechristen = 1;
    }

    /* �m�� */
    if ( --ticketreset <= 0 )
    {
      str_cpy( msg, "�s�����q�m��S���X�F�M�ЦV�U�g�P���ʶR"
        "(\e[1;32mchance\e[0m)�T" );

      talk_channel_2( msg, CHANNEL_BULLETIN, "�m��" );
      dice_ticket();
    }

    /* �H�񪫫~�O�� */
    deposit_update();

    /* �Ѳ� */
    stock_update();

    /* �p��Ȧ�s�ڧQ�� */
    bank_update();

    /* �ֳz */
    lotto_update();

    /* �������Q�� */
    for ( pClub = club_list; pClub; pClub = pClub->next )
    {
      if ( pClub->status == CLUB_STATUS_UNIONIZE
        && pClub->money >= BankProfit
        && pClub->money <= BankThreshold
        && pClub->master
        && is_online( pClub->master ) )
          pClub->money += pClub->money / BankProfit;
    }

    save_club();

    /* �Ѩc */
    jail_update();

    /* �g�@ */
    tribunal_update();

    /* �p�����Ѯɶ� */
    steal_update();

    break;
  }

  if ( time_info.day >= DaysPerMonth )
  {
    time_info.day = 0;
    time_info.month++;
  }

  if ( time_info.month >= MonthsPerYear )
  {
    time_info.month = 0;
    time_info.year++;
  }

  /*
  if ( time_info.month >= 9 && time_info.month <= 16 )
      diff = weather_info.mmhg >  985 ? -2 : 2;
  else
      diff = weather_info.mmhg > 1015 ? -2 : 2;
  */

  diff = weather_info.mmhg > 1000 ? -2 : 2;

  weather_info.change += diff * dice(1, 4) + dice(2, 6) - dice(2, 6);
  weather_info.change  = UMAX(weather_info.change, -12);
  weather_info.change  = UMIN(weather_info.change,  12);

  weather_info.mmhg += weather_info.change;
  weather_info.mmhg  = UMAX(weather_info.mmhg,  960);
  weather_info.mmhg  = UMIN(weather_info.mmhg, 1040);

  switch ( weather_info.sky )
  {
  default:
    mudlog( LOG_DEBUG , "Weather_update: ���~���Ѯ� %d.", weather_info.sky );
    weather_info.sky = SKY_CLOUDLESS;
    break;

  case SKY_CLOUDLESS:
    if ( weather_info.mmhg <  990
      || ( weather_info.mmhg < 1010 && number_bits( 2 ) == 0 ) )
    {
      str_cat( buf, "\e[36m�ѪŤ��C�C�E���Q���M���@�I�������Pı�C\e[0m" );
      weather_info.sky = SKY_CLOUDY;
    }
    break;

  case SKY_CLOUDY:
    if ( weather_info.mmhg <  970
      || ( weather_info.mmhg <  990 && number_bits( 2 ) == 0 ) )
    {
      str_cat( buf, "\e[1;34m���j���B�I�V�U���M�}�l�U�_�B�F�C\e[0m" );
      weather_info.sky = SKY_RAINING;
    }

    if ( weather_info.mmhg > 1030 && number_bits( 2 ) == 0 )
    {
      str_cat( buf, "\e[1;36m�@�}�L���j�L�M���h�����F�C\e[0m" );
      weather_info.sky = SKY_CLOUDLESS;
    }
    break;

  case SKY_RAINING:
    if ( weather_info.mmhg <  970 && number_bits( 2 ) == 0 )
    {
      str_cat( buf, "\e[1;37m�F���@�n�M�{�q�ӫG�F�ѪšC\e[0m" );
      weather_info.sky = SKY_LIGHTNING;
    }

    if ( weather_info.mmhg > 1030
      || ( weather_info.mmhg > 1010 && number_bits( 2 ) == 0 ) )
    {
      str_cat( buf, "\e[1;44m�B�ն}�l�ܽw�M���������F�C\e[0m" );
      weather_info.sky = SKY_CLOUDY;
    }
    break;

  case SKY_LIGHTNING:
    if ( weather_info.mmhg > 1010
      || ( weather_info.mmhg > 990 && number_bits( 2 ) == 0 ) )
    {
      str_cat( buf, "\e[44m�I�S�������{�q�����������F�C\e[0m" );
      weather_info.sky = SKY_RAINING;
      break;
    }
    break;
  }

  if ( buf[0] ) talk_channel_2( buf, CHANNEL_WEATHER, "" );
  RETURN_NULL();
}

void char_update( void )
{
  CHAR_DATA       * ch;
  CHAR_DATA       * ch_save;
  CHAR_DATA       * ch_quit;
  SHOP_DATA       * pShop;
  AFFECT_DATA     * paf;
  AFFECT_DATA     * paf_next;
  SKILL_DATA      * pSkill;
  ENABLE_DATA     * pEnable;
  OBJ_DATA        * obj;
  ROOM_INDEX_DATA * room;
  time_t            save_time;
  int               sn;

  PUSH_FUNCTION( "char_update" );

  save_time = current_time;
  ch_save   = ch_quit = NULL;

  for ( ch = char_list; ch; ch = ch->next )
  {
    if ( !verify_char( ch ) ) continue;

    if ( !IS_NPC( ch )
      && ( ch->desc == NULL || ch->desc->connected == CON_PLAYING )
      && ch->save_time < save_time )
    {
      ch_save   = ch;
      save_time = ch->save_time;
    }

    /* �����ө� */
    if ( is_keeper( ch )
      && ch->position == POS_STANDING
      && ch->in_room
      && ch->pIndexData
      && ( pShop = ch->pIndexData->pShop )
      && pShop->sential
      && pShop->sential != ch->in_room )
    {
      act( "$n��M����ơM�^�^�����a�]�F�C", ch, NULL, NULL, TO_ROOM );

      char_from_room( ch );
      char_to_room( ch, pShop->sential );

      act( "$n�S�^��L���u���^��F�C", ch, NULL, NULL, TO_ROOM );
    }

    /* �����E�p */
    if ( IS_NPC( ch )
      && ch->migrate > 0
      && ch->in_room
      && !ch->mount_by
      && !ch->boss
      && !ch->talk_buf
      && ch->position == POS_STANDING )
    {
      /* �^�� Limbo */
      if ( number_range( 1, 100 ) > ch->migrate )
      {
        /* �p�G�����N�b Limbo */
        if ( ch->in_room == RoomLimbo ) continue;

        /* ���O�h��L�s�^ Limbo */
        act( "$n�P��X�{�@�ε���M���[�M���٦�$n�����v�O�S"
          , ch, NULL, NULL, TO_ROOM );

        char_from_room( ch );
        char_to_room( ch, RoomLimbo );

        act( "���n�N��M$n�ӭɦ�@�U�C", ch, NULL, NULL, TO_ROOM );
      }

      /* �X�{�b�Z�� */
      else
      {
        act( "$n�P��X�{�@�ε���M���[�M���٦�$n�����v�O�S"
          , ch, NULL, NULL, TO_ROOM );

        for ( ; ; )
        {
          room = get_room_index( number_range( 0, 65535 ) );

          if ( room
            && room->NoQuit  == FALSE
            && room->Safe    == FALSE
            && room->Sail    == FALSE
            && room->Private == FALSE
            && !is_loge( room ) ) break;
        }

        char_from_room( ch );
        char_to_room( ch, room );

        act( "��M�M�W��X�{�@�ε���M$n�N�ѳo�ε��𨫤F�U�ӡC"
          , ch, NULL, NULL, TO_ROOM );
      }
    }

    if ( ch->position > POS_DEAD )
    {
      if ( ch->hit  < get_curr_hit( ch ) )  ch->hit  += attr_gain(ch, HIT_GAIN  );
      if ( ch->mana < get_curr_mana( ch ) ) ch->mana += attr_gain(ch, MANA_GAIN );
      if ( ch->move < get_curr_move( ch ) ) ch->move += attr_gain(ch, MOVE_GAIN );
    }

    /* �B�~��_�򥻼ƭ� */
    if ( ch->position == POS_SLEEPING
      && !is_dead( ch )
      && ch->jail <= 0
      && ch->failed <= 0 )
    {
      for ( pEnable = ch->enable; pEnable; pEnable = pEnable->next )
      {
        if ( !( pSkill = pEnable->skill ) || pSkill->type != TAR_MEDITATION )
          continue;

        if ( !pSkill->function )
        {
          mudlog( LOG_DEBUG, "char_update: �ޯ� %s �S���ާ@���."
            ,pSkill->name );
          break;
        }

        sn = pSkill->slot;
        ( *pSkill->function ) ( sn, get_practice( ch, sn ), ch, NULL );

        break;
      }
    }

    if ( !IS_NPC( ch ) && ch->level < LEVEL_IMMORTAL )
    {
      if ( ( obj = get_eq_char( ch, WEAR_LIGHT ) )
        && obj->item_type == ITEM_LIGHT
        && obj->value[2] > 0 )
      {
        if ( --obj->value[2] == 0 && ch->in_room )
        {
          --ch->in_room->light;
          act( "$p \"��\" ���@�n�N���������F�C", ch, obj, NULL, TO_ROOM );
          act( "$p���M���h�������������F�C", ch, obj, NULL, TO_CHAR );
          extract_obj( obj );
        }
      }

      if ( ++ch->timer >= 12 )
      {
        if ( ch->was_in_room == NULL && ch->in_room )
        {
          /* �ץ��O�_���毸 */
          ch->was_in_room = is_station( ch->in_room )
            ? get_hometown( ch ) : ch->in_room;

          if ( ch->fighting ) stop_fighting( ch, TRUE );

          /* �ץ��y�M */
          if ( ch->mount    ) unmount_char( ch, ch->mount    );
          if ( ch->mount_by ) unmount_char( ch->mount_by, ch );

          act( "$n�����v�C�C�����M���i�F�@���������C"
            , ch, NULL, NULL, TO_ROOM );
          send_to_char( "�A�����v�C�C�����F�C\n\r", ch );
          save_char_obj( ch , SAVE_FILE );
          char_from_room( ch );
          char_to_room( ch, RoomLimbo );

          /* �M���l�ܬ����I */
          clear_trace( ch, FALSE );
        }
      }

      /* �����Ӥ[�S���H�{�⪺���� */
      if ( ch->timer > MaxCorpseIdle && ch->pcdata && ch->pcdata->corpse )
      {
        extract_obj( ch->pcdata->corpse );
        ch->pcdata->corpse = NULL;
        ch->position       = POS_STANDING;
        ch->was_in_room    = get_hometown( ch );
      }

      if ( ch->timer > MaxPlayingIdle && can_quit( ch, FALSE ) )
        ch_quit = ch;

      gain_condition( ch, COND_DRUNK,  -1 );
      gain_condition( ch, COND_FULL,   -1 );
      gain_condition( ch, COND_THIRST, -1 );
    }

    for ( paf = ch->affected; paf; paf = paf_next )
    {
      paf_next = paf->next;

           if ( paf->duration > 0 ) paf->duration--;
      else if ( paf->duration < 0 );
      else
      {
        if ( !paf_next
          || paf_next->type != paf->type
          || paf_next->duration > 0 )
        {
          if ( ( pSkill = get_skill( paf->type ) ) && pSkill->msg_off )
            act( "$t", ch, pSkill->msg_off, NULL, TO_CHAR );
        }

        affect_remove( ch, paf );
      }
    }

    if ( is_affected( ch, SLOT_POISON ) )
    {
      act( "$n�ܵh�W�M�����b�o�ݡM�i�༵���F�h�[�F", ch, NULL, NULL, TO_ROOM );
      send_to_char( "�A�ܵh�W�M�����b�o�ݡC\n\r", ch );
      damage( ch, ch, 2, SLOT_POISON, ATTACK_RANDOM );
    }
  }

  if ( ch_save || ch_quit )
  {
    for ( ch = char_list; ch; ch = ch->next )
    {
      if ( !verify_char( ch ) ) continue;
      if ( ch == ch_save ) save_char_obj( ch , SAVE_FILE );
      if ( ch == ch_quit ) do_quit( ch, "" );
    }
  }

  RETURN_NULL();
}

void area_update( void )
{
  AREA_DATA * pArea;
  CHAR_DATA * pch;

  PUSH_FUNCTION( "area_update" );

  for ( pArea = area_first; pArea; pArea = pArea->next )
  {
    if ( ++pArea->age < 3 ) continue;
    if ( pArea->nplayer > 0 && pArea->age == 15 - 1 )
    {
      for ( pch = char_list; pch; pch = pch->next )
      {
        if ( !verify_char( pch ) ) continue;

        if ( !IS_NPC( pch )
          && IS_AWAKE( pch )
          && pch->in_room
          && pch->in_room->area == pArea )
        {
          send_to_char( "\e[1;34m�Ať�����B�ǨӤ@�}���L���}�B�n�C\e[0m\n\r"
            , pch );
        }
      }
    }

    if ( pArea->nplayer == 0 || pArea->age >= 15 )
    {
      reset_area( pArea );
      mine_reset_area( pArea );
      pArea->age = number_range( 0, 3 );
    }
  }
  RETURN_NULL();
}

void obj_update( void )
{
  CHAR_DATA * rch;
  CHAR_DATA * victim;
  OBJ_DATA  * obj;
  char      * message;

  PUSH_FUNCTION( "obj_update" );

  for ( obj = object_list; obj; obj = obj->next )
  {
    if ( !verify_obj( obj ) ) continue;

    if ( ( victim = obj->carried_by )
      && victim->pIndexData
      && victim->pIndexData->pShop ) continue;

    /* �r�����ɮ� */
    if ( obj->venom >= 0 ) obj->venom--;

    /* �Z���W�j����z */
    if ( obj->max_armor > 0 && obj->armor > obj->max_armor )
      obj->armor = UMAX( obj->max_armor, obj->armor - 100 );

    if ( obj->item_type == ITEM_LETTER && obj->address )
    {
      if ( --obj->value[0] <= 0 )
      {
        if ( obj->carried_by )
          send_to_char( "�ܥi���M�A�S���b�S�w���ɶ����e�F�A���H��T\n\r"
          , obj->carried_by );

        free_string( obj->description );
        obj->description = str_dup( "�@�ʤw�g�S���Ϊ��H��(letter)�C" );
        obj->address     = NULL;
        obj->value[0]    = 0;
        obj->cost        = 1;
      }
    }

    /* ���~��� */
    if ( obj->timer == 2 && obj->item_type == ITEM_FOOD && obj->value[3] == 0 )
    {
      if ( obj->carried_by )
      {
        act( "$p�o�X�@�}�c��M�ݨӬO�a���F�C"
          , obj->carried_by, obj, NULL, TO_CHAR );
      }

      else if ( obj->in_room && ( rch = obj->in_room->people ) )
      {
        act( "$p�o�X�@�}����M�a���F�C", rch, obj, NULL, TO_ALL );
      }

      obj->value[3]++;
      continue;
    }

    if ( obj->timer <= 0 || --obj->timer > 0 ) continue;

    switch ( obj->item_type )
    {
    default:              message = "$p�L�n�L���a�����F�C";       break;
    case ITEM_FOUNTAIN:   message = "$p���h�����Ӱ��\\�F�C";      break;
    case ITEM_CORPSE_NPC: message = "$p�಴���N�Ƭ��@���Фg�C";   break;
    case ITEM_CORPSE_PC:  message = "$p�Ƭ��@���Фg���b�Ů𤤡C"; break;
    case ITEM_FOOD:       message = "$p���a�Ӥ��ѱ��F�C";         break;
    }

    if ( obj->carried_by )
    {
      act( smash_act_keyword( message, "nesp" ) , obj->carried_by, obj, NULL, TO_CHAR );
    }

    else if ( obj->in_room && ( rch = obj->in_room->people ) )
    {
      act( smash_act_keyword( message, "nesp" ) , rch, obj, NULL, TO_ALL );
    }

    extract_obj( obj );
  }

  RETURN_NULL();
}

void aggr_update( void )
{
  CHAR_DATA      * wch;
  CHAR_DATA      * wch_next;
  CHAR_DATA      * ch;
  CHAR_DATA      * ch_next;
  CHAR_DATA      * vch;
  CHAR_DATA      * vch_next;
  CHAR_DATA      * victim;
  MPROG_ACT_LIST * tmp_act;
  MPROG_ACT_LIST * tmp2_act;
  int              count;

  PUSH_FUNCTION( "aggr_update" );

  for ( wch = char_list; wch; wch = wch_next )
  {
    wch_next = wch->next;

    if ( !verify_char( wch ) ) continue;

    /* �]�w���a��������H�ξ԰��ƭ� */
    wch->situs = -1;
    wch->cease = FALSE;

    if ( !wch->in_room
      || !wch->in_room->area
      || wch->in_room->area->nplayer <= 0 ) continue;

    if ( wch->mpactnum > 0 && IS_NPC( wch ) )
    {
      for ( tmp_act = wch->mpact; tmp_act; tmp_act = tmp_act->next )
      {
        mprog_wordlist_check( tmp_act->buf, wch, tmp_act->ch
          , tmp_act->obj, tmp_act->vo, ACT_PROG );
        free_string( tmp_act->buf );
      }

      for ( tmp_act = wch->mpact; tmp_act; tmp_act = tmp2_act )
      {
        tmp2_act = tmp_act->next;
        free_struct( tmp_act , STRUCT_MPROG_ACT_LIST );
      }

      wch->mpactnum = 0;
      wch->mpact    = NULL;
    }

    if ( IS_NPC( wch ) || wch->level >= LEVEL_IMMORTAL || !wch->in_room )
      continue;

    for ( ch = wch->in_room->people; ch; ch = ch_next )
    {
      ch_next = ch->next_in_room;

      if ( !verify_char( ch )
        || !IS_NPC( ch )
        || !IS_SET( ch->act, ACT_AGGRESSIVE )
        || ch->fighting
        || is_affected( ch, SLOT_CHARM_PERSON )
        || !IS_AWAKE( ch )
        || ( IS_SET( ch->act, ACT_WIMPY ) && IS_AWAKE( wch ) )
        || !can_see( ch, wch )
        || !can_damage( wch ) )
          continue;

      victim = NULL;
      for ( count = 0, vch = wch->in_room->people; vch; vch = vch_next )
      {
        vch_next = vch->next_in_room;

        if ( verify_char( vch )
          && !IS_NPC( vch )
          && vch->level < LEVEL_IMMORTAL
          && ( !IS_SET( ch->act, ACT_WIMPY ) || !IS_AWAKE( vch ) )
          && can_see( ch, vch )
          && can_damage( vch ) )
        {
          if ( number_range( 0, count++ ) == 0 ) victim = vch;
        }
      }

      if ( !victim )
      {
        mudlog( LOG_DEBUG , "aggr_update: ��H�O�Ū�." );
        continue;
      }

      striking( ch, victim, TYPE_UNDEFINED );
    }
  }

  RETURN_NULL();
}

void time_update( void )
{
  char buf [MAX_STRING_LENGTH];
  char buf1[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "time_update" );

  if ( reboot_time > 0 )
  {
    shutdown_time = ( time_t ) 0;

    /* �Ĥ@���q�� */
    if ( current_time >= reboot_time - FIRST_WARN * 60 && first_warn )
    {
      str_cpy( buf, "\e[1;37m�͸ͪ����Ϧb����~�ɰ_�M�@�Ѷ·t���Pı"
        "Ţ�n�ۨC�@�ӤH�C\n\r�����|�B��_�M���G���L�ƸU���Y�����W"
        "����b���~�h��ۡC\n\r\n\r\e[0m" );

      send_to_all_char( buf );
      chinese_number( FIRST_WARN, buf1 );
      sprintf( buf , "�u�t�γq���v%s%s�����᭫�s�}���T\n\r", mud_name, buf1 );
      send_to_all_char( buf );

      first_warn = FALSE;
      wizlock    = TRUE;
      RETURN_NULL();
    }

    /* �ĤG���q�� */
    if ( current_time >= reboot_time - SECOND_WARN * 60 && second_warn )
    {
      str_cpy( buf, "\e[1m���~�������W�������n�V�ӶV��M�ϩ��N�b����"
        "\n\r����W�T�϶}�l�ɰ_�M�A�P��F�Ʊ����Y���ʡC\n\r\n\r\e[0m" );

      send_to_all_char( buf );
      chinese_number( SECOND_WARN, buf1 );
      sprintf( buf , "�u�t�γq���v%s%s�����᭫�s�}���T\n\r", mud_name, buf1 );
      send_to_all_char( buf );

      second_warn = FALSE;
      wizlock     = TRUE;
      RETURN_NULL();
    }

    /* �ĤT���q�� */
    if ( current_time >= reboot_time - THIRD_WARN * 60 && third_warn )
    {
      str_cpy( buf, "\e[1m�Z�a�����}�l�Q�i��M�q�o�n���ӬݡM���G"
        "���W�N�|�Q�𳴡C\n\r�o�ɭԦA�����N�Ӥ��ΤF�M�A�ݨ�\\�h�H"
        "��a�a�����k�`�C\n\r�A�]�����Ϋݪ��Q�n���}�o�ӦM�I���a��M"
        "�����d���C�s�b�M���ȨS���N�C\n\r\n\r\e[0m" );

      send_to_all_char( buf );
      chinese_number( THIRD_WARN, buf1 );
      sprintf( buf , "�u�t�γq���v%s%s�����᭫�s�}���T\n\r", mud_name, buf1 );
      send_to_all_char( buf );

      third_warn = FALSE;
      wizlock    = TRUE;
      RETURN_NULL();
    }

    /* ���s�}�� */
    if ( current_time >= reboot_time )
    {
      str_cpy( buf, "\e[1m�L�Ʃ_�Ǫ��ͪ��N�����𳴡M�o�Ӯa��O���F"
        "�C\n\r���L�M�H�٬��۴N���Ʊ�M�@�ѷs�����O�b�C�ӤH���W�o��"
        "�C\n\r���[��M�ڭ̤@�w�|���خa�骺�C\n\r\n\r\e[0m" );
      send_to_all_char( buf );

      sprintf( buf , "%s\e[0m�w�g�ҰʤF�ۧڷ����{�ǡM�{�����s�ҰʡT\n\r"
        , mud_name );
      send_to_all_char( buf );
      mudlog( LOG_INFO, "�Ұʦۧڷ����{��, �{�����s�Ұ�." );
      do_reboot( NULL, "" );
      RETURN_NULL();
    }
  }

  else if ( shutdown_time > 0 )
  {
    reboot_time = ( time_t ) 0;

    /* �Ĥ@���q�� */
    if ( current_time >= shutdown_time - FIRST_WARN * 60 && first_warn )
    {
      str_cpy( buf, "\a\a\e[1;37m�͸ͪ����Ϧb����~�ɰ_�M�@�Ѷ·t���Pı"
        "Ţ�n�ۨC�@�ӤH�C\n\r�����|�B��_�M���G���L�ƸU���Y�����W"
        "����b���~�h��ۡC\n\r\n\r\e[0m" );

      send_to_all_char( buf );

      chinese_number( FIRST_WARN, buf1 );
      sprintf( buf , "�u�t��ĵ�i�v%s%s�����������t�ΡT\n\r", mud_name, buf1 );
      send_to_all_char( buf );

      first_warn = FALSE;
      wizlock    = TRUE;
      RETURN_NULL();
    }

    /* �ĤG��ĵ�i */
    if ( current_time >= shutdown_time - SECOND_WARN * 60 && second_warn )
    {
      str_cpy( buf, "\a\a\e[1m���~�������W�������n�V�ӶV��M�ϩ��N�b����"
        "\n\r����W�T�϶}�l�ɰ_�M�A�P��F�Ʊ����Y���ʡC\n\r\n\r\e[0m" );

      send_to_all_char( buf );

      chinese_number( SECOND_WARN, buf1 );
      sprintf( buf , "�u�t��ĵ�i�v%s%s�����������t�ΡT\n\r", mud_name, buf1 );
      send_to_all_char( buf );

      second_warn = FALSE;
      wizlock     = TRUE;
      RETURN_NULL();
    }

    /* �ĤT��ĵ�i */
    if ( current_time >= shutdown_time - THIRD_WARN * 60 && third_warn )
    {
      str_cpy( buf, "\a\a\e[1m�Z�a�����}�l�Q�i��M�q�o�n���ӬݡM���G"
        "���W�N�|�Q�𳴡C\n\r�o�ɭԦA�����N�Ӥ��ΤF�M�A�ݨ�\\�h�H"
        "��a�a�����k�`�C\n\r�A�]�����Ϋݪ��Q�n���}�o�ӦM�I���a��M"
        "�����d���C�s�b�M���ȨS���N�C\n\r\n\r\e[0m" );

      send_to_all_char( buf );
      chinese_number( THIRD_WARN, buf1 );
      sprintf( buf , "�u�t��ĵ�i�v%s%s�����������t�ΡT\n\r", mud_name, buf1 );
      send_to_all_char( buf );

      third_warn = FALSE;
      wizlock    = TRUE;
      RETURN_NULL();
    }

    /* �����t�� */
    if ( current_time >= shutdown_time )
    {
      str_cpy( buf, "\a\a\e[1m�F���@�n�M�@�}���j�����p���b�A���e�M\n\r"
        "�Ҧ����a�賣�ܦ��F�@�������M�N���O���s�z�o�@�ˡC\n\r"
        "�o�Ӧa��w�g�P���M�Ҧ��ͪ��������F�M�j�a�{�b�n�k�]�Ӥ�"
        "�ΤF�C\n\r\n\r\e[0m" );

      send_to_all_char( buf );

      sprintf( buf , "%s�w�g�ҰʤF�ۧڷ����{�ǡM�{�������T\n\r" , mud_name );
      send_to_all_char( buf );
      mudlog( LOG_INFO, "�Ұʦۧڷ����{�ǡM�{�������C" );

      do_shutdown( NULL, "" );
      RETURN_NULL();
    }
  }

  RETURN_NULL();
}

void login_update( void )
{
  DESCRIPTOR_DATA * man;

  PUSH_FUNCTION( "login_update" );

  for ( man = descriptor_list; man; man = man->next )
  {
    if ( verify_desc( man )
      && man->connected != CON_PLAYING
      && --man->repeat <= MaxLoginIdle )
    {
      mudlog( LOG_INFO , "%s �b���նi�J�ɶ��m�Ӥ[", man->host );
      write_to_buffer( man, "���m�Ӥ[�M�t�αj�������C\n\r" , 0 );
      close_socket( man );
    }
  }

  RETURN_NULL();
}

void increase_update( void )
{
  DESCRIPTOR_DATA * man;
  static time_t     RecordTime = -1;
  CHAR_DATA       * ch;
  char              buf[BUFSIZ];
  int               played;
  int               increase;
  int               HitPlus;
  int               ManaPlus;
  int               MovePlus;
  int               OldHit;
  int               OldMana;
  int               OldMove;

  PUSH_FUNCTION( "increase_update" );

  if ( Increase <= 0 || RecordTime == current_time ) RETURN_NULL();

  RecordTime = current_time;
  for ( man = descriptor_list; man; man = man->next )
  {
    if ( !verify_desc( man )
      || !( ch = man->character )
      || man->connected != CON_PLAYING
      || !( ch = man->character )
      || ch->was_in_room
      || ch->jail > 0 ) continue;

    if ( IncreaseLimit > 0 && get_age( ch ) > IncreaseLimit ) continue;

    played = ch->played + ( int ) ( current_time - ch->logon );

    if ( ( played % 14400 ) == 0 )
    {
      increase = Increase;
      HitPlus  = ManaPlus = MovePlus = 0;

      while ( --increase >= 0 )
      {
        switch( number_range( 1, 3 ) )
        {
        default:
        case 1:
          HitPlus++;
          break;

        case 2:
          ManaPlus++;
          break;

        case 3:
          MovePlus++;
          break;
        }
      }

      if ( HitPlus > 0 )
      {
        OldHit      = ch->max_hit;
        ch->max_hit = UMIN( MaxHit, ch->max_hit + HitPlus );

        if ( ch->max_hit > OldHit )
        {
          chinese_number( ch->max_hit - OldHit, buf );
          act( "$6�H�۷��몺�W���H�ξ��m�M�A���ͩR�O�W�@$t�I�T$0"
            , ch, buf, NULL, TO_CHAR );
        }
      }

      if ( ManaPlus > 0 )
      {
        OldMana      = ch->max_mana;
        ch->max_mana = UMIN( MaxMana, ch->max_mana + ManaPlus );

        if ( ch->max_mana > OldMana )
        {
          chinese_number( ch->max_mana - OldMana, buf );
          act( "$6�H�۷��몺�W���H�ξ��m�M�A���k�O�W�@$t�I�T$0"
            , ch, buf, NULL, TO_CHAR );
        }
      }

      if ( MovePlus > 0 )
      {
        OldMove      = ch->max_move;
        ch->max_move = UMIN( MaxMove, ch->max_move + MovePlus );

        if ( ch->max_move > OldMove )
        {
          chinese_number( ch->max_move - OldMove, buf );
          act( "$6�H�۷��몺�W���H�ξ��m�M�A����O�W�@$t�I�T$0"
            , ch, buf, NULL, TO_CHAR );
        }
      }
    }
  }

  RETURN_NULL();
}

void update_handler( void )
{
  static int pulse_area;
  static int pulse_mobile;
  static int pulse_violence;
  static int pulse_point;
  static int pulse_angel;
  static int pulse_angel_coming;

  PUSH_FUNCTION( "update_handler" );

  if ( --pulse_area <= 0 )
  {
    pulse_area = number_range( PULSE_AREA / 2, 3 * PULSE_AREA / 2 );
    area_update();
  }

  /* ��s�̤j�W�u�H�� */
  top_update();

  if ( --pulse_violence <= 0 )
  {
    pulse_violence = PULSE_VIOLENCE;
    violence_update();
  }

  if ( --pulse_mobile <= 0 )
  {
    pulse_mobile = PULSE_MOBILE;
    mobile_update();
  }

  if ( --pulse_point <= 0 )
  {
    pulse_point = number_range( PULSE_TICK / 2, 3 * PULSE_TICK / 2 );

    /* �ˬd§���O�_�i�H�o�e */
    gift_update();

    dosage_update();
    weather_update();
    doante_update();
    fail_update();
    ship_update();
    char_update();
    obj_update();
    fight_update();
    variable_update();

    /* �a�� */
    bounty_update();

    /* ��s��檫�~ */
    auction_update();

    /* �𱼶��m�Ӥ[���H */
    login_update();

    /* �۰ʦs�� */
    autosave_update();

    room_update();
  }

  if ( angel_list && PlayerAngel && --pulse_angel <= 0 )
  {
    pulse_angel = PULSE_PER_SECOND;
    angel_update();
  }

  /* �ץ��u�@����� */
  if ( angel_list && PlayerAngel && --pulse_angel_coming <= 0 )
  {
    pulse_angel_coming = PULSE_PER_SECOND * AngelTimes;
    AngelComing = TRUE;
  }

  talk_update();
  bus_update();
  increase_update();
  question_update();

  /* ��s�䰨����� */
  gamble_update();

  /* ���s�}����s */
  time_update();

  aggr_update();

  tail_chain();
  RETURN_NULL();
}

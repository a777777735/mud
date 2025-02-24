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
#include <string.h>
#include "merc.h"

/* 區域函數 */
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

/* 提昇玩家等級 */
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

  /* 到達英雄無法繼續升級 */
  if ( ch->level > LEVEL_HERO || !( pClass = ch->class ) ) RETURN_NULL();

  /* 增加法力的處理 */
  add_mana     = ( pClass->fMana )
    ? add_mana = number_range( 2, ( 2 * get_curr_int( ch )
    + get_curr_wis( ch ) ) / 6 ) : 1;

  mana_factor  = ( pClass->factor[INT_ITEM] + pClass->factor[WIS_ITEM] );
  add_mana     = UMIN( 10000, ( add_mana * 200 ) ) / UMAX( 1, mana_factor );
  add_mana    += ch->level / 10;

  /* 增加生命力的處理 */
  add_hp       = number_range( 1 , get_curr_con( ch ) / 10 );
  add_hp       = ( ch->level / 2 ) + ( 2 * get_curr_con( ch ) )
    + number_range( 10 * add_hp , 12 * add_hp );
  add_hp       = ( add_hp * 100 ) / UMAX( 1, pClass->factor[CON_ITEM] );

  /* 增加體力的處理 */
  add_move     = number_range( 5,
               ( get_curr_con( ch ) + get_curr_dex( ch ) ) / 6 );
  move_factor  = pClass->factor[STR_ITEM] + pClass->factor[DEX_ITEM];

  add_move     = UMIN( 10000, ( add_move * 200 ) ) / UMAX( 1, move_factor );
  add_move    += ch->level / 10;

  /* 增加練習點數的處理 */
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
    "\e[1;33m你獲得了: 生命力 %d/%d 法力 %d/%d 體力 %d/%d "
    "學習點數 %d/%d \e[0m\n\r"
      , add_hp, get_curr_hit( ch ), add_mana, get_curr_mana( ch ), add_move
      , get_curr_move( ch ), add_prac, ch->practice  );

  if ( ( ch->level % 10 ) == 0 )
  {
    if ( add_firman( ch, 1 ) > 0 )
      send_to_char( "\e[1;32m此外再送你一張免死金牌﹗\e[0m\n\r", ch );
  }

  /* 發出轉職的警告 */
  if ( ch->level > ForceLevel
    && ch->level < LEVEL_HERO
    && pClass == class_demos )
  {
    send_to_char(
      "你死皮賴臉地不轉職﹐平常又不學好﹐看來你真是當盜賊的料。\n\r" , ch );

    ch->class = class_force;
    set_title( ch, class_force->title );
  }

  else if ( ch->class && ch->level >= ch->class->warn )
    send_to_char( "恭禧你﹐你可以考慮轉職囉。\n\r" , ch );

  /* 是否守護神要離開 */
  if ( ch->level == AngelLevel )
  {
    send_to_char( "你的守護神(\e[1;32mangel\e[0m)已經離開了﹗\n\r", ch );
    REMOVE_BIT( ch->act, PLR_ANGEL );
  }

  check_train( ch );

  /* 到達英雄 */
  if ( ch->level == LEVEL_HERO )
  {
    chinese_number( hero_count() + 1, buf2 );

    sprintf( buf1, "\e[1;36m讓我們恭禧%s成為%s第%s位英雄﹗\e[0m\n\r"
      , mob_name( NULL, ch ), mud_name, buf2 );

    send_to_all_char( buf1 );

    act( "恭禧你成為$t第$T位英雄﹗", ch, mud_name, buf2, TO_CHAR );

    /* 增加額外屬性 */
    add_hero_bonus( ch );

    /* 增加英雄 */
    hero_add( ch );

    /* 寫入檔案 */
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
    mudlog( LOG_DEBUG, "check_train: 缺乏來源." );
    RETURN_NULL();
  }

  if ( IS_NPC( ch ) ) RETURN_NULL();

  if ( ch->practice >= get_str_train( ch ) ) Train = TRUE;
  if ( ch->practice >= get_wis_train( ch ) ) Train = TRUE;
  if ( ch->practice >= get_int_train( ch ) ) Train = TRUE;
  if ( ch->practice >= get_dex_train( ch ) ) Train = TRUE;
  if ( ch->practice >= get_con_train( ch ) ) Train = TRUE;

  if ( Train ) send_to_char( "\n\r\n\r\e[1;34m你有一些訓練點數﹐請找訓練師升級"
    "(train)你的屬性吧﹗\n\r\n\r\e[0m", ch );

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
    send_to_char( "\a\a\a\e[1;31m哇﹗恭禧你﹗你升級囉﹗\e[0m\n\r", ch );
    act( "$n勤奮的苦練獲得了回報﹐提昇了等級。", ch, NULL, NULL, TO_ROOM );

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
    mudlog( LOG_DEBUG, "attr_gain: 來源不正確." );
    RETURN( 0 );
  }

  switch( type )
  {
  default:
    mudlog( LOG_DEBUG, "attr_gain: 錯誤的選項 %d.", type );
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

  /* 幫派 recall 點加強回復 */
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
      mudlog( LOG_DEBUG, "gain_condition: 錯誤的形態 %d.", iCond );
      break;

    case COND_FULL:
      send_to_char( "\e[33m咕嚕嚕一聲﹐你的肚子餓了。\e[0m\n\r", ch );
      break;

    case COND_THIRST:
      send_to_char( "\e[33m你覺得口乾舌燥﹐似乎可以喝下一桶水。\e[0m\n\r"
        , ch );
      break;

    case COND_DRUNK:
      if ( condition ) send_to_char( "\e[1;37m你頭上星星不見了﹐"
        "已經清醒了。\e[0m\n\r", ch );
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

    /* 坐騎 */
    if ( ( rider = ch->mount_by )
      && ch->in_room
      && ch->in_room != rider->in_room )
    {
      if ( ch->fighting ) stop_fighting( ch , TRUE );

      act( "$n追隨主人走了。", ch , NULL, NULL , TO_ROOM );
      char_from_room( ch );
      char_to_room( ch, rider->in_room );

      act( "$N追隨$n來到這裡。", rider, NULL, ch, TO_ALL );
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
        act( "$n順手撿起$p放到自己身上。", ch, obj_best, NULL, TO_ROOM );
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
    /* 檢查是否有輸出緩衝區 */
    if ( !verify_char( ch ) || !ch->talk_buf || !IS_NPC( ch ) ) continue;

    /* 若沒有所在地或是在打鬥 */
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
      mudlog( LOG_DEBUG, "talk_update: 輸出控制碼 %d錯誤.", ch->talk_mode );
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
            send_to_char( "外面世界太黑暗了﹐你乾脆待在這裡吧﹗\n\r", ch );
            continue;
          }

          if ( ch->fighting ) stop_fighting( ch, TRUE );

          if ( ch->mount    ) unmount_char( ch, ch->mount    );
          if ( ch->mount_by ) unmount_char( ch->mount_by, ch );

          /* 修正小鬼 */
          if ( ch->boss  ) spirit_from_char( ch );

          send_to_char( "喂﹐你可以不用思過囉﹐希望加油吧﹗\n\r", ch );
          act( "恭喜$n可以不用思過囉。", ch, NULL, NULL, TO_ROOM );
          char_from_room( ch );
          char_to_room( ch, location );
          act( "$n在這裡開始挖糞塗牆了。", ch, NULL, NULL, TO_ROOM );
          do_look( ch, "auto" );

          /* 清除追蹤紀錄點 */
          clear_trace( ch, TRUE );
        }

        else
        {
          send_to_char( "喂﹗你可以不必思過囉﹗\n\r\a\a", ch );
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
          send_to_char( "你又可以繼續使用 \e[1;32mtell\e[0m 指令了﹗\n\r", ch );
          REMOVE_BIT( ch->act, PLR_NO_TELL );
        }

        else if ( IS_SET( ch->act, PLR_SILENCE ) )
        {
          send_to_char( "你又可以大聲說話了﹗\n\r", ch );
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
        act( "$n又開始覺得手癢﹐想偷東西了﹗", ch, NULL, NULL, TO_CHAR );
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
            send_to_char( "外面世界太黑暗了﹐你乾脆待在這裡吧﹗\n\r", ch );
            continue;
          }

          if ( ch->fighting ) stop_fighting( ch, TRUE );

          if ( ch->mount    ) unmount_char( ch, ch->mount    );
          if ( ch->mount_by ) unmount_char( ch->mount_by, ch );

          /* 修正小鬼 */
          if ( ch->boss  ) spirit_from_char( ch );

          send_to_char( "喂﹐你可以出獄囉﹐希望你好好做人﹗\n\r", ch );
          act( "恭喜$n可以出獄囉。", ch, NULL, NULL, TO_ROOM );
          char_from_room( ch );
          char_to_room( ch, location );
          act( "$n在這裡開始從頭做人。", ch, NULL, NULL, TO_ROOM );
          do_look( ch, "auto" );

          /* 清除追蹤紀錄點 */
          clear_trace( ch, TRUE );
        }

        else
        {
          send_to_char( "喂﹗你可以出獄囉﹐還想吃牢飯啊﹗\n\r\a\a", ch );
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
    mudlog( LOG_DEBUG, "foggy_update: 型態錯誤 %d.", type );
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

      sprintf( buf, "%s\e[0m附近突然出現一陣濃霧﹐請在此區域的路人多加注意﹗"
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

      sprintf( buf, "%s\e[0m出現的濃霧已經慢慢退去了﹗", FoggyArea->name );

      talk_channel_2( buf, CHANNEL_WEATHER, "" );

      FoggyArea = NULL;
    }

    break;
  }

  RETURN_NULL();
}

/* 捐贈 */
void doante_update( void )
{
  CHAR_DATA * ch;

  PUSH_FUNCTION( "donate_update" );

  if ( DonateLock ) RETURN_NULL();

  for ( ch = char_list; ch; ch = ch->next )
  {
    if ( verify_char( ch ) && ch->donate > 0 && --ch->donate <= 0 )
    {
      act( "恭禧你﹐你不再受到$t基金會監督你的財產﹗"
        , ch, mud_name, NULL, TO_CHAR );
    }
  }

  RETURN_NULL();
}

/* 計算銀行存款利息 */
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
    act( "又是新的一天開始﹐$t$0銀行發給你利息﹗", ch, mud_name, NULL, TO_CHAR );
  }

  RETURN_NULL();
}

/* 藥物劑量 */
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
      send_to_char( "由於體內藥物的毒性發作﹐你慢慢覺得越來越不濟事﹗\n\r", ch );
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
        send_to_char( "你藥物中毒的情形稍稍退了﹗\n\r", ch );
      }
    }
  }

  RETURN_NULL();
}

/* 改變天氣 */
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
    str_cat( buf, "\e[1;32m公雞的啼叫聲此起彼落﹐不絕於耳。\e[0m" );
    break;

  case  7:
    foggy_update( FOGGY_CLEAR );
    weather_info.sunlight = SUN_RISE;
    str_cat( buf, "\e[1;33m遠處人家的地方裊裊冒起炊煙。\e[0m" );
    break;

  case 12:

    /* 事件觸發 */
    event_update();

    str_cat( buf, "\e[1;33m已經正午時分了﹐應該找家客棧吃個飯。\e[0m" );
    break;

  case 18:
    weather_info.sunlight = SUN_SET;
    str_cat( buf, "\e[1;41m太陽慢慢的從西邊落下去。\e[0m" );
    break;

  case 19:
    str_cat( buf, "\e[1;33m天色漸漸地暗了下來﹐燈火則亮了起來。\e[0m" );
    break;

  case 20:
    weather_info.sunlight = SUN_DARK;
    str_cat( buf, "\e[1;46m黑暗爬上天邊﹐夜晚開始籠罩大地。\e[0m" );
    break;

  case 22:
    str_cat( buf, "\e[1;33m你發覺人們漸漸都往臥房鑽﹐睡大頭覺去了。\e[0m" );
    break;

  case 24:

    str_cat( buf, "\e[1;46m打更者說道﹕天乾物燥﹐小心火燭。\e[0m" );
    time_info.hour = 0;
    time_info.day++;

    /* 名稱 */
    for ( pDesc = descriptor_list; pDesc; pDesc = pDesc->next )
    {
      if ( verify_desc( pDesc )
        && pDesc->connected == CON_PLAYING
        && verify_char( ( ch = pDesc->character ) )
        && ch->pcdata
        && ch->pcdata->rechristen <= 0 )
        ch->pcdata->rechristen = 1;
    }

    /* 彩券 */
    if ( --ticketreset <= 0 )
    {
      str_cpy( msg, "新的公益彩券又推出了﹐請向各經銷商購買"
        "(\e[1;32mchance\e[0m)﹗" );

      talk_channel_2( msg, CHANNEL_BULLETIN, "彩券" );
      dice_ticket();
    }

    /* 寄放物品費用 */
    deposit_update();

    /* 股票 */
    stock_update();

    /* 計算銀行存款利息 */
    bank_update();

    /* 樂透 */
    lotto_update();

    /* 算幫派利息 */
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

    /* 天牢 */
    jail_update();

    /* 懲罰 */
    tribunal_update();

    /* 小偷偷竊時間 */
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
    mudlog( LOG_DEBUG , "Weather_update: 錯誤的天氣 %d.", weather_info.sky );
    weather_info.sky = SKY_CLOUDLESS;
    break;

  case SKY_CLOUDLESS:
    if ( weather_info.mmhg <  990
      || ( weather_info.mmhg < 1010 && number_bits( 2 ) == 0 ) )
    {
      str_cat( buf, "\e[36m天空中慢慢聚集烏雲﹐有一點陰陰的感覺。\e[0m" );
      weather_info.sky = SKY_CLOUDY;
    }
    break;

  case SKY_CLOUDY:
    if ( weather_info.mmhg <  970
      || ( weather_info.mmhg <  990 && number_bits( 2 ) == 0 ) )
    {
      str_cat( buf, "\e[1;34m豆大的雨點向下落﹐開始下起雨了。\e[0m" );
      weather_info.sky = SKY_RAINING;
    }

    if ( weather_info.mmhg > 1030 && number_bits( 2 ) == 0 )
    {
      str_cat( buf, "\e[1;36m一陣微風吹過﹐雲層消失了。\e[0m" );
      weather_info.sky = SKY_CLOUDLESS;
    }
    break;

  case SKY_RAINING:
    if ( weather_info.mmhg <  970 && number_bits( 2 ) == 0 )
    {
      str_cat( buf, "\e[1;37m轟隆一聲﹐閃電照亮了天空。\e[0m" );
      weather_info.sky = SKY_LIGHTNING;
    }

    if ( weather_info.mmhg > 1030
      || ( weather_info.mmhg > 1010 && number_bits( 2 ) == 0 ) )
    {
      str_cat( buf, "\e[1;44m雨勢開始變緩﹐漸漸的停了。\e[0m" );
      weather_info.sky = SKY_CLOUDY;
    }
    break;

  case SKY_LIGHTNING:
    if ( weather_info.mmhg > 1010
      || ( weather_info.mmhg > 990 && number_bits( 2 ) == 0 ) )
    {
      str_cat( buf, "\e[44m呼嘯不停的閃電漸漸的平息了。\e[0m" );
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

    /* 有關商店 */
    if ( is_keeper( ch )
      && ch->position == POS_STANDING
      && ch->in_room
      && ch->pIndexData
      && ( pShop = ch->pIndexData->pShop )
      && pShop->sential
      && pShop->sential != ch->in_room )
    {
      act( "$n突然有急事﹐匆匆忙忙地跑了。", ch, NULL, NULL, TO_ROOM );

      char_from_room( ch );
      char_to_room( ch, pShop->sential );

      act( "$n又回到他的工做崗位了。", ch, NULL, NULL, TO_ROOM );
    }

    /* 有關遷徙 */
    if ( IS_NPC( ch )
      && ch->migrate > 0
      && ch->in_room
      && !ch->mount_by
      && !ch->boss
      && !ch->talk_buf
      && ch->position == POS_STANDING )
    {
      /* 回到 Limbo */
      if ( number_range( 1, 100 ) > ch->migrate )
      {
        /* 如果本身就在 Limbo */
        if ( ch->in_room == RoomLimbo ) continue;

        /* 不是則把他叫回 Limbo */
        act( "$n周圍出現一團紫氣﹐不久﹐哪還有$n的身影呢﹖"
          , ch, NULL, NULL, TO_ROOM );

        char_from_room( ch );
        char_to_room( ch, RoomLimbo );

        act( "不好意思﹐$n來借住一下。", ch, NULL, NULL, TO_ROOM );
      }

      /* 出現在凡間 */
      else
      {
        act( "$n周圍出現一團紫氣﹐不久﹐哪還有$n的身影呢﹖"
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

        act( "突然﹐上方出現一團紫氣﹐$n就由這團紫氣走了下來。"
          , ch, NULL, NULL, TO_ROOM );
      }
    }

    if ( ch->position > POS_DEAD )
    {
      if ( ch->hit  < get_curr_hit( ch ) )  ch->hit  += attr_gain(ch, HIT_GAIN  );
      if ( ch->mana < get_curr_mana( ch ) ) ch->mana += attr_gain(ch, MANA_GAIN );
      if ( ch->move < get_curr_move( ch ) ) ch->move += attr_gain(ch, MOVE_GAIN );
    }

    /* 額外恢復基本數值 */
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
          mudlog( LOG_DEBUG, "char_update: 技能 %s 沒有操作函數."
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
          act( "$p \"啪\" 的一聲就消失不見了。", ch, obj, NULL, TO_ROOM );
          act( "$p忽然失去光輝消失不見了。", ch, obj, NULL, TO_CHAR );
          extract_obj( obj );
        }
      }

      if ( ++ch->timer >= 12 )
      {
        if ( ch->was_in_room == NULL && ch->in_room )
        {
          /* 修正是否為驛站 */
          ch->was_in_room = is_station( ch->in_room )
            ? get_hometown( ch ) : ch->in_room;

          if ( ch->fighting ) stop_fighting( ch, TRUE );

          /* 修正座騎 */
          if ( ch->mount    ) unmount_char( ch, ch->mount    );
          if ( ch->mount_by ) unmount_char( ch->mount_by, ch );

          act( "$n的身影慢慢消失﹐走進了一片薄霧中。"
            , ch, NULL, NULL, TO_ROOM );
          send_to_char( "你的身影慢慢消失了。\n\r", ch );
          save_char_obj( ch , SAVE_FILE );
          char_from_room( ch );
          char_to_room( ch, RoomLimbo );

          /* 清除追蹤紀錄點 */
          clear_trace( ch, FALSE );
        }
      }

      /* 消掉太久沒有人認領的屍體 */
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
      act( "$n很痛苦﹐全身在發抖﹐可能撐不了多久了", ch, NULL, NULL, TO_ROOM );
      send_to_char( "你很痛苦﹐全身在發抖。\n\r", ch );
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
          send_to_char( "\e[1;34m你聽見遠處傳來一陣輕微的腳步聲。\e[0m\n\r"
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

    /* 毒物的時效 */
    if ( obj->venom >= 0 ) obj->venom--;

    /* 武器增強的減弱 */
    if ( obj->max_armor > 0 && obj->armor > obj->max_armor )
      obj->armor = UMAX( obj->max_armor, obj->armor - 100 );

    if ( obj->item_type == ITEM_LETTER && obj->address )
    {
      if ( --obj->value[0] <= 0 )
      {
        if ( obj->carried_by )
          send_to_char( "很可惜﹐你沒有在特定的時間內送達你的信件﹗\n\r"
          , obj->carried_by );

        free_string( obj->description );
        obj->description = str_dup( "一封已經沒有用的信件(letter)。" );
        obj->address     = NULL;
        obj->value[0]    = 0;
        obj->cost        = 1;
      }
    }

    /* 物品臭酸 */
    if ( obj->timer == 2 && obj->item_type == ITEM_FOOD && obj->value[3] == 0 )
    {
      if ( obj->carried_by )
      {
        act( "$p發出一陣惡臭﹐看來是壞掉了。"
          , obj->carried_by, obj, NULL, TO_CHAR );
      }

      else if ( obj->in_room && ( rch = obj->in_room->people ) )
      {
        act( "$p發出一陣臭味﹐壞掉了。", rch, obj, NULL, TO_ALL );
      }

      obj->value[3]++;
      continue;
    }

    if ( obj->timer <= 0 || --obj->timer > 0 ) continue;

    switch ( obj->item_type )
    {
    default:              message = "$p無聲無息地消失了。";       break;
    case ITEM_FOUNTAIN:   message = "$p失去水分而乾枯\了。";      break;
    case ITEM_CORPSE_NPC: message = "$p轉眼間就化為一片塵土。";   break;
    case ITEM_CORPSE_PC:  message = "$p化為一片塵土散在空氣中。"; break;
    case ITEM_FOOD:       message = "$p敗壞而分解掉了。";         break;
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

    /* 設定玩家攻擊部位以及戰鬥數值 */
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
        mudlog( LOG_DEBUG , "aggr_update: 對象是空的." );
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

    /* 第一次通知 */
    if ( current_time >= reboot_time - FIRST_WARN * 60 && first_warn )
    {
      str_cpy( buf, "\e[1;37m裊裊的炊煙在城牆外升起﹐一股黑暗的感覺"
        "籠罩著每一個人。\n\r飛鳥四處驚起﹐似乎有無數萬的某不知名"
        "物體在城外蟄伏著。\n\r\n\r\e[0m" );

      send_to_all_char( buf );
      chinese_number( FIRST_WARN, buf1 );
      sprintf( buf , "「系統通知」%s%s分鐘後重新開機﹗\n\r", mud_name, buf1 );
      send_to_all_char( buf );

      first_warn = FALSE;
      wizlock    = TRUE;
      RETURN_NULL();
    }

    /* 第二次通知 */
    if ( current_time >= reboot_time - SECOND_WARN * 60 && second_warn )
    {
      str_cpy( buf, "\e[1m城外那不知名的鼓噪聲越來越近﹐彷彿就在身邊"
        "\n\r城牆上狼煙開始升起﹐你感到了事情的嚴重性。\n\r\n\r\e[0m" );

      send_to_all_char( buf );
      chinese_number( SECOND_WARN, buf1 );
      sprintf( buf , "「系統通知」%s%s分鐘後重新開機﹗\n\r", mud_name, buf1 );
      send_to_all_char( buf );

      second_warn = FALSE;
      wizlock     = TRUE;
      RETURN_NULL();
    }

    /* 第三次通知 */
    if ( current_time >= reboot_time - THIRD_WARN * 60 && third_warn )
    {
      str_cpy( buf, "\e[1m驀地城門開始被進攻﹐從這聲音來看﹐似乎"
        "馬上就會被攻陷。\n\r這時候再不走就來不及了﹐你看到許\多人"
        "攜家帶眷的逃亡。\n\r你也迫不及待的想要離開這個危險的地方﹐"
        "畢竟留的青山在﹐不怕沒材燒。\n\r\n\r\e[0m" );

      send_to_all_char( buf );
      chinese_number( THIRD_WARN, buf1 );
      sprintf( buf , "「系統通知」%s%s分鐘後重新開機﹗\n\r", mud_name, buf1 );
      send_to_all_char( buf );

      third_warn = FALSE;
      wizlock    = TRUE;
      RETURN_NULL();
    }

    /* 重新開機 */
    if ( current_time >= reboot_time )
    {
      str_cpy( buf, "\e[1m無數奇怪的生物將城池攻陷﹐這個家算是毀了"
        "。\n\r不過﹐人還活著就有希望﹐一股新的活力在每個人身上發芽"
        "。\n\r不久後﹐我們一定會重建家園的。\n\r\n\r\e[0m" );
      send_to_all_char( buf );

      sprintf( buf , "%s\e[0m已經啟動了自我毀滅程序﹐程式重新啟動﹗\n\r"
        , mud_name );
      send_to_all_char( buf );
      mudlog( LOG_INFO, "啟動自我毀滅程序, 程式重新啟動." );
      do_reboot( NULL, "" );
      RETURN_NULL();
    }
  }

  else if ( shutdown_time > 0 )
  {
    reboot_time = ( time_t ) 0;

    /* 第一次通知 */
    if ( current_time >= shutdown_time - FIRST_WARN * 60 && first_warn )
    {
      str_cpy( buf, "\a\a\e[1;37m裊裊的炊煙在城牆外升起﹐一股黑暗的感覺"
        "籠罩著每一個人。\n\r飛鳥四處驚起﹐似乎有無數萬的某不知名"
        "物體在城外蟄伏著。\n\r\n\r\e[0m" );

      send_to_all_char( buf );

      chinese_number( FIRST_WARN, buf1 );
      sprintf( buf , "「系統警告」%s%s分鐘後關閉系統﹗\n\r", mud_name, buf1 );
      send_to_all_char( buf );

      first_warn = FALSE;
      wizlock    = TRUE;
      RETURN_NULL();
    }

    /* 第二次警告 */
    if ( current_time >= shutdown_time - SECOND_WARN * 60 && second_warn )
    {
      str_cpy( buf, "\a\a\e[1m城外那不知名的鼓噪聲越來越近﹐彷彿就在身邊"
        "\n\r城牆上狼煙開始升起﹐你感到了事情的嚴重性。\n\r\n\r\e[0m" );

      send_to_all_char( buf );

      chinese_number( SECOND_WARN, buf1 );
      sprintf( buf , "「系統警告」%s%s分鐘後關閉系統﹗\n\r", mud_name, buf1 );
      send_to_all_char( buf );

      second_warn = FALSE;
      wizlock     = TRUE;
      RETURN_NULL();
    }

    /* 第三次警告 */
    if ( current_time >= shutdown_time - THIRD_WARN * 60 && third_warn )
    {
      str_cpy( buf, "\a\a\e[1m驀地城門開始被進攻﹐從這聲音來看﹐似乎"
        "馬上就會被攻陷。\n\r這時候再不走就來不及了﹐你看到許\多人"
        "攜家帶眷的逃亡。\n\r你也迫不及待的想要離開這個危險的地方﹐"
        "畢竟留的青山在﹐不怕沒材燒。\n\r\n\r\e[0m" );

      send_to_all_char( buf );
      chinese_number( THIRD_WARN, buf1 );
      sprintf( buf , "「系統警告」%s%s分鐘後關閉系統﹗\n\r", mud_name, buf1 );
      send_to_all_char( buf );

      third_warn = FALSE;
      wizlock    = TRUE;
      RETURN_NULL();
    }

    /* 關閉系統 */
    if ( current_time >= shutdown_time )
    {
      str_cpy( buf, "\a\a\e[1m轟隆一聲﹐一陣巨大的落雷打在你眼前﹐\n\r"
        "所有的地方都變成了一片火海﹐就像是火山爆發一樣。\n\r"
        "這個地方已經銷毀﹐所有生物都消失了﹐大家現在要逃也來不"
        "及了。\n\r\n\r\e[0m" );

      send_to_all_char( buf );

      sprintf( buf , "%s已經啟動了自我毀滅程序﹐程式關閉﹗\n\r" , mud_name );
      send_to_all_char( buf );
      mudlog( LOG_INFO, "啟動自我毀滅程序﹐程式關閉。" );

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
      mudlog( LOG_INFO , "%s 在嘗試進入時閒置太久", man->host );
      write_to_buffer( man, "閒置太久﹐系統強制關閉。\n\r" , 0 );
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
          act( "$6隨著歲月的增長以及歷練﹐你的生命力上昇$t點﹗$0"
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
          act( "$6隨著歲月的增長以及歷練﹐你的法力上昇$t點﹗$0"
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
          act( "$6隨著歲月的增長以及歷練﹐你的體力上昇$t點﹗$0"
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

  /* 更新最大上線人數 */
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

    /* 檢查禮物是否可以發送 */
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

    /* 懸賞 */
    bounty_update();

    /* 更新拍賣物品 */
    auction_update();

    /* 踢掉閒置太久的人 */
    login_update();

    /* 自動存檔 */
    autosave_update();

    room_update();
  }

  if ( angel_list && PlayerAngel && --pulse_angel <= 0 )
  {
    pulse_angel = PULSE_PER_SECOND;
    angel_update();
  }

  /* 修正守護神到來 */
  if ( angel_list && PlayerAngel && --pulse_angel_coming <= 0 )
  {
    pulse_angel_coming = PULSE_PER_SECOND * AngelTimes;
    AngelComing = TRUE;
  }

  talk_update();
  bus_update();
  increase_update();
  question_update();

  /* 更新賭馬的資料 */
  gamble_update();

  /* 重新開機更新 */
  time_update();

  aggr_update();

  tail_chain();
  RETURN_NULL();
}

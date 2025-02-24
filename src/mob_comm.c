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
#include <stdlib.h>
#include "merc.h"

char * mprog_type_to_name      args( ( int ) );

char * mprog_type_to_name( int type )
{
  PUSH_FUNCTION( "mprog_type_to_name" );

  switch ( type )
  {
  case IN_FILE_PROG:   RETURN( "in_file_prog"   );
  case ACT_PROG:       RETURN( "act_prog"       );
  case SPEECH_PROG:    RETURN( "speech_prog"    );
  case RAND_PROG:      RETURN( "rand_prog"      );
  case FIGHT_PROG:     RETURN( "fight_prog"     );
  case HITPRCNT_PROG:  RETURN( "hitprcnt_prog"  );
  case DEATH_PROG:     RETURN( "death_prog"     );
  case ENTRY_PROG:     RETURN( "entry_prog"     );
  case GREET_PROG:     RETURN( "greet_prog"     );
  case ALL_GREET_PROG: RETURN( "all_greet_prog" );
  case GIVE_PROG:      RETURN( "give_prog"      );
  case BRIBE_PROG:     RETURN( "bribe_prog"     );
  default:             RETURN( "ERROR_PROG"     );
  }
}

FUNCTION( do_mpstat )
{
  char         arg[ MAX_INPUT_LENGTH  ];
  MPROG_DATA * mprg;
  CHAR_DATA  * victim;

  PUSH_FUNCTION( "do_mpstat" );

  one_argument( argument, arg );

  if ( !( victim = get_char_world( ch, arg ) ) )
  {
    act( "你要觀看的對象 $T 不在這裡.", ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  if ( !IS_NPC( victim ) || !victim->pIndexData )
  {
    act( "玩家$N是沒有 MOB 的程式的﹗", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( !( victim->pIndexData->progtypes ) )
  {
    act( "$N沒有任何 MOB 程式﹗", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( victim->disable_prog )
  {
    act( "$N的 MOB 程式被取消﹗", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  clear_buffer();
  send_to_buffer(
    "名    字﹕%s  "
    "虛擬號碼﹕%d \n\r"
    "短 描 述﹕%s\n\r"
    "長 描 述﹕%s"
    "生 命 力﹕%d/%d  "
    "法    力﹕%d/%d  "
    "移 動 力﹕%d/%d\n\r"
    "等    級﹕%d "
    "職    業﹕%s "
    "陣    營﹕%d\n\r"
    "防 護 力﹕%d "
    "金    錢﹕%d "
    "經 驗 值﹕%d\n\r"
    , victim->name
    , victim->pIndexData->vnum
    , victim->byname
    , victim->long_descr[0] ? victim->long_descr : "(從缺)\n\r"
    , victim->hit
    , get_curr_hit( victim )
    , victim->mana
    , get_curr_mana( victim )
    , victim->move
    , get_curr_move( victim )
    , victim->level
    , class_name( victim->class, TRUE )
    , victim->alignment
    , get_ac( victim )
    , victim->gold
    , victim->exp );

  for ( mprg = victim->pIndexData->mobprogs; mprg ; mprg = mprg->next )
  {
    send_to_buffer( ">%s %s\n\r%s\n\r"
      , mprog_type_to_name( mprg->type )
      , mprg->arglist
      , mprg->comlist );
  }

  print_buffer( ch );
  RETURN_NULL();
}

FUNCTION( do_mpasound )
{
  ROOM_INDEX_DATA * was_in_room;
  EXIT_DATA       * pexit;
  int               door;

  PUSH_FUNCTION( "do_mpasound" );

  if ( !ch->in_room )
  {
    mudlog( LOG_DEBUG, "do_mpasound: %d 缺乏來源.", ch->pIndexData->vnum );
    RETURN_NULL();
  }

  if ( !argument[0] )
  {
    mudlog( LOG_DEBUG , "Mpasound: MOB 號碼 %d 沒有引數."
      , ch->pIndexData->vnum );
    RETURN_NULL();
  }

  for ( was_in_room = ch->in_room, door = 0; door < DIR_MAX; door++ )
  {
    if ( ( pexit = was_in_room->exit[door] )
      && pexit->to_room
      && pexit->to_room != was_in_room )
    {
      ch->in_room = pexit->to_room;
      MOBtrigger  = FALSE;
      act( smash_act_keyword( argument, "nes" ), ch, NULL, NULL, TO_ROOM );
    }
  }

  ch->in_room = was_in_room;
  RETURN_NULL();
}

FUNCTION( do_mpkill )
{
  char        arg[ MAX_INPUT_LENGTH ];
  CHAR_DATA * victim;
  int         vnum;

  PUSH_FUNCTION( "do_mpkill" );

  vnum = ch->pIndexData->vnum;
  one_argument( argument, arg );

  if ( !arg[0] )
  {
    mudlog( LOG_DEBUG , "MpKill: MOB 號碼 %d 沒有任何的引數.", vnum );
    RETURN_NULL();
  }

  if ( !( victim = get_char_room( ch, arg ) ) )
  {
    mudlog( LOG_DEBUG , "MpKill: MOB 號碼 %d 要殺的對象 %s 不在房間裡."
      , vnum, arg );

    if ( ( victim = get_char_world( ch, arg ) ) )
    {
      mudlog( LOG_DEBUG, "MpKill: %d(房號:%d)要殺的對象%s在%d."
        , vnum
        , ch->in_room ? ch->in_room->vnum : -1
        , victim->name
        , victim->in_room ? victim->in_room->vnum : -1 );
    }

    RETURN_NULL();
  }

  if ( victim == ch )
  {
    mudlog( LOG_DEBUG , "MpKill: MOB 號碼 %d 要殺的對象是自己.", vnum );
    RETURN_NULL();
  }

  if ( is_affected( ch, SLOT_CHARM_PERSON ) && ch->master == victim )
  {
    mudlog( LOG_DEBUG , "MpKill: 被迷惑的 MOB 號碼 %d 殺自己的主人.", vnum );
    RETURN_NULL();
  }

  if ( ch->position == POS_FIGHTING )
  {
    mudlog( LOG_DEBUG , "MpKill: MOB 號碼 %d 已經開始戰鬥了.", vnum );
    RETURN_NULL();
  }

  striking( ch, victim, TYPE_UNDEFINED );
  RETURN_NULL();
}

FUNCTION( do_mpjunk )
{
  char       arg[ MAX_INPUT_LENGTH ];
  OBJ_DATA * obj;
  OBJ_DATA * obj_next;

  PUSH_FUNCTION( "do_mpjunk" );

  one_argument( argument, arg );

  if ( !arg[0] )
  {
    mudlog( LOG_DEBUG , "Mpjunk: MOB 號碼 %d 沒有任何的引數."
      , ch->pIndexData->vnum );
    RETURN_NULL();
  }

  if ( str_cmp( arg, "all" ) && str_prefix( "all.", arg ) )
  {
    if ( ( obj = get_obj_wear( ch, arg ) ) )
    {
      unequip_char( ch, obj, FALSE );
      extract_obj( obj );
      RETURN_NULL();
    }

    if ( !( obj = get_obj_carry( ch, arg ) ) ) RETURN_NULL();
    extract_obj( obj );
  }

  else
  {
    for ( obj = ch->carrying; obj; obj = obj_next )
    {
      obj_next = obj->next_content;
      if ( !arg[3] || is_name( &arg[4], obj->name ) )
      {
        if ( obj->wear_loc != WEAR_NONE ) unequip_char( ch, obj, FALSE );
        extract_obj( obj );
      }
    }
  }
  RETURN_NULL();
}

FUNCTION( do_mpsay )
{
  char arg[MAX_INPUT_LENGTH];

  PUSH_FUNCTION( "do_mpsay" );

  if ( !argument[0] )
  {
    mudlog( LOG_DEBUG , "Mpsay: MOB 號碼 %d 沒有任何的引數."
      , ch->pIndexData->vnum );
    RETURN_NULL();
  }

  /* 已經有輸出緩衝區了 */
  if ( ch->talk_buf ) RETURN_NULL();

  argument = one_statement( argument, arg );

  /* 若是第一句話有內容 */
  if ( arg[0] ) do_say( ch, arg );

  /* 若是後面還有的話 */
  if ( argument[0] )
  {
    ch->talk_buf  = str_dup( argument );
    ch->talk_mode = MODE_SAY;
  }

  RETURN_NULL();
}

FUNCTION( do_mpchat )
{
  char arg[MAX_INPUT_LENGTH];

  PUSH_FUNCTION( "do_mpchat" );

  if ( !argument[0] )
  {
    mudlog( LOG_DEBUG , "Mpchat: MOB 號碼 %d 沒有任何的引數."
      , ch->pIndexData->vnum );
    RETURN_NULL();
  }

  /* 已經有輸出緩衝區了 */
  if ( ch->talk_buf ) RETURN_NULL();

  argument = one_statement( argument, arg );

  /* 若是第一句話有內容 */
  if ( arg[0] )
  {
    ChatRecord = FALSE;
    do_chat( ch, arg );
    ChatRecord = TRUE;
  }

  /* 若是後面還有的話 */
  if ( argument[0] )
  {
    ch->talk_buf  = str_dup( argument );
    ch->talk_mode = MODE_CHAT;
  }

  RETURN_NULL();
}

FUNCTION( do_mpechoaround )
{
  char        arg[ MAX_INPUT_LENGTH ];
  CHAR_DATA * victim;

  PUSH_FUNCTION( "do_mpechoaround" );

  argument = one_argument( argument, arg );

  if ( !arg[0] )
  {
    mudlog( LOG_DEBUG , "Mpechoaround: MOB 號碼 %d 沒有任何的引數."
      , ch->pIndexData->vnum );
    RETURN_NULL();
  }

  if ( !( victim = get_char_room( ch, arg ) ) )
  {
    mudlog( LOG_DEBUG , "Mpechoaround: MOB 號碼 %d 的對象 %s 不存在."
      , ch->pIndexData->vnum, arg );

    if ( ( victim = get_char_world( ch, arg ) ) )
    {
      mudlog( LOG_DEBUG, "Mpechoaround: %d(房號:%d)的對象%s在%d."
        , ch->pIndexData->vnum
        , ch->in_room ? ch->in_room->vnum : -1
        , victim->name
        , victim->in_room ? victim->in_room->vnum : -1 );
    }

    RETURN_NULL();
  }

  act( smash_act_keyword( argument, "nesNES" ) , ch, NULL, victim, TO_NOTVICT );
  RETURN_NULL();
}

FUNCTION( do_mpechoat )
{
  char        arg[ MAX_INPUT_LENGTH ];
  CHAR_DATA * victim;

  PUSH_FUNCTION( "do_mpechoat" );

  argument = one_argument( argument, arg );

  if ( !arg[0] || !argument[0] )
  {
    mudlog( LOG_DEBUG , "Mpechoat: MOB 號碼 %d 沒有任何的引數."
       , ch->pIndexData->vnum );
    RETURN_NULL();
  }

  if ( !( victim = get_char_room( ch, arg ) ) )
  {
    mudlog( LOG_DEBUG , "Mpechoat: MOB 號碼 %d 的對象 %s 不存在."
      , ch->pIndexData->vnum, arg );

    if ( ( victim = get_char_world( ch, arg ) ) )
    {
      mudlog( LOG_DEBUG, "Mpechoat: %d(房號:%d)的對象%s在%d."
        , ch->pIndexData->vnum
        , ch->in_room ? ch->in_room->vnum : -1
        , victim->name
        , victim->in_room ? victim->in_room->vnum : -1 );
    }

    RETURN_NULL();
  }

  act( smash_act_keyword( argument, "nesNES" ) , ch, NULL, victim, TO_VICT );
  RETURN_NULL();
}

FUNCTION( do_mpecho )
{
  PUSH_FUNCTION( "do_mpecho" );

  if ( !argument[0] )
  {
    mudlog( LOG_DEBUG , "Mpecho: MOB 號碼 %d 沒有任何的引數."
      , ch->pIndexData->vnum );
    RETURN_NULL();
  }

  act( smash_act_keyword( argument, "nes" ), ch, NULL, NULL, TO_ROOM );
  RETURN_NULL();
}

FUNCTION( do_mplset )
{
  CHAR_DATA   * victim;
  SKILL_DATA  * pSkill;
  ENABLE_DATA * pEnable;
  char          arg1[MAX_INPUT_LENGTH];
  char          arg2[MAX_INPUT_LENGTH];
  char          arg3[MAX_INPUT_LENGTH];
  int           value;

  PUSH_FUNCTION( "do_mplset" );

  if ( !IS_NPC( ch ) ) RETURN_NULL();

  /* 先取出三個參數 */
  argument = one_argument( argument , arg1 );
  argument = one_argument( argument , arg2 );
  one_argument( argument , arg3 );

  /* 察看是否參數都有正確 */
  if ( !arg1[0] || !arg2[0] || !arg3[0] )
  {
    mudlog( LOG_DEBUG, "Mplset: 怪物 %d 裡有設定不正確技能."
      , ch->pIndexData->vnum );
    RETURN_NULL();
  }

  /* 先找找看是否有這個人 */
  if ( !( victim = get_char_room( ch , arg1 ) ) )
  {
    mudlog( LOG_DEBUG, "Mplset: 怪物 %d 裡找不到人 %s 設定技能."
      , ch->pIndexData->vnum, arg1 );

    if ( ( victim = get_char_world( ch, arg1 ) ) )
    {
      mudlog( LOG_DEBUG, "Mplset: %d(房號:%d)的對象%s在%d."
        , ch->pIndexData->vnum
        , ch->in_room ? ch->in_room->vnum : -1
        , victim->name
        , victim->in_room ? victim->in_room->vnum : -1 );
    }

    RETURN_NULL();
  }

  /* 不能作用在非玩家身上 */
  if ( IS_NPC( victim ) ) RETURN_NULL();

  /* 找尋技能名稱, 若是第二個參數為 all就不用找了 */
  if ( !( pSkill = skill_isname( arg2 ) ) )
  {
    mudlog( LOG_DEBUG, "Mplset: 怪物 %d 裡設定錯誤技能 %s."
      , ch->pIndexData->vnum, arg2 );
    RETURN_NULL();
  }

  /* 第三個數值不能為非數字 */
  if ( !is_number( arg3 ) )
  {
    mudlog( LOG_DEBUG, "Mplset: 怪物 %d 裡設定錯誤熟練度 %s."
      , ch->pIndexData->vnum , arg3 );
    RETURN_NULL();
  }

  if ( ( value = atoi( arg3 ) ) < 0 || value > 100 )
  {
    mudlog( LOG_DEBUG, "Mplset: 怪物 %d 所設定之技能熟練 %d 不合理.",
      ch->pIndexData->vnum, value );
    RETURN_NULL();
  }

  /* 檢查是否可以學到 */
  if ( value > 0 && !check_skill_restrict( victim, pSkill, FALSE ) )
    RETURN_NULL();

  if ( ( victim->skill[pSkill->slot] = value ) <= 0 )
  {
    /* 取消他的這項技能 */
    for ( pEnable = victim->enable; pEnable; pEnable = pEnable->next )
    {
      if ( pEnable->skill == pSkill )
      {
        extract_an_enable( victim, pSkill );
        break;
      }
    }
  }

  RETURN_NULL();
}

FUNCTION( do_mpmload )
{
  char             arg[ MAX_INPUT_LENGTH ];
  MOB_INDEX_DATA * pMobIndex;
  CHAR_DATA      * victim;

  PUSH_FUNCTION( "do_mpmload" );

  one_argument( argument, arg );

  if ( !arg[0] || !is_number( arg ) )
  {
    mudlog( LOG_DEBUG , "Mpmload - 號碼 %d 怪物的程式裡有錯誤的參數 %s."
      , ch->pIndexData->vnum, arg );
    RETURN_NULL();
  }

  if ( !( pMobIndex = get_mob_index( atoi( arg ) ) ) )
  {
    mudlog( LOG_DEBUG , "Mpmload - 號碼 %d 怪物的程式裡有錯誤的參數 %s."
      , ch->pIndexData->vnum, arg );
    RETURN_NULL();
  }

  if ( ch->in_room ) victim = shape_mobile( pMobIndex, ch->in_room );
  RETURN_NULL();
}

FUNCTION( do_mpoload )
{
  char             arg1[ MAX_INPUT_LENGTH ];
  char             arg2[ MAX_INPUT_LENGTH ];
  OBJ_INDEX_DATA * pObjIndex;
  OBJ_DATA       * obj;
  int              level;

  PUSH_FUNCTION( "do_mpoload" );

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );

  if ( !arg1[0] || !is_number( arg1 ) )
  {
    mudlog( LOG_DEBUG , "Mpoload - 號碼 %d 語法錯誤﹐參數為 %s."
      , ch->pIndexData->vnum, arg1 );
    RETURN_NULL();
  }

  if ( !arg2[0] )
  {
    level = get_trust( ch );
  }

  else
  {
    if ( !is_number( arg2 ) )
    {
      mudlog( LOG_DEBUG , "Mpoload - 號碼 %d 語法 %s 錯誤."
        , ch->pIndexData->vnum, arg2 );

      RETURN_NULL();
    }

    if ( ( level = atoi( arg2 ) ) < 0 )
    {
      mudlog( LOG_DEBUG , "Mpoload - 號碼 %d 等級 %s 錯誤."
        , ch->pIndexData->vnum, arg2 );
      RETURN_NULL();
    }
  }

  if ( level > get_trust( ch ) )
  {
    mudlog( LOG_DEBUG , "Mpoload - 號碼 %d 載入物品 %d 等級比自己高 %d."
      , ch->pIndexData->vnum, level, ch->level );

    RETURN_NULL();
  }

  if ( !( pObjIndex = get_obj_index( atoi( arg1 ) ) ) )
  {
    mudlog( LOG_DEBUG , "Mpoload - 號碼 %d 參數 %s 錯誤."
      , ch->pIndexData->vnum, arg1 );
    RETURN_NULL();
  }

  obj = create_object( pObjIndex, level );

  if ( obj->Takeable    ) obj_to_char( obj, ch );
  else if ( ch->in_room ) obj_to_room( obj, ch->in_room );

  RETURN_NULL();
}

FUNCTION( do_mppurge )
{
  char        arg[ MAX_INPUT_LENGTH ];
  CHAR_DATA * victim;
  CHAR_DATA * vnext;
  OBJ_DATA  * obj;
  OBJ_DATA  * obj_next;

  PUSH_FUNCTION( "do_mppurge" );

  if ( !ch->in_room )
  {
    mudlog( LOG_DEBUG, "do_mppurge: %d 來源不正確.", ch->pIndexData->vnum );
    RETURN_NULL();
  }

  one_argument( argument, arg );

  if ( !arg[0] )
  {
    for ( victim = ch->in_room->people; victim; victim = vnext )
    {
      vnext = victim->next_in_room;
      if ( IS_NPC( victim ) && victim != ch ) extract_char( victim, TRUE );
    }

    for ( obj = ch->in_room->contents; obj; obj = obj_next )
    {
      obj_next = obj->next_content;
      extract_obj( obj );
    }

    RETURN_NULL();
  }

  if ( !( victim = get_char_room( ch, arg ) ) )
  {
    if ( ( obj = get_obj_here( ch, arg ) ) ) extract_obj( obj );
    else mudlog( LOG_DEBUG , "Mppurge - 號碼 %d 參數 %s 錯誤."
      , ch->pIndexData->vnum, arg );
    RETURN_NULL();
  }

  if ( !IS_NPC( victim ) )
  {
    mudlog( LOG_DEBUG , "Mppurge - 號碼 %d 錯誤清除玩家 %s."
      , ch->pIndexData->vnum, victim->name );
    RETURN_NULL();
  }

  extract_char( victim, TRUE );
  RETURN_NULL();
}

FUNCTION( do_mpgoto )
{
  char             arg[ MAX_INPUT_LENGTH ];
  ROOM_INDEX_DATA *location;

  PUSH_FUNCTION( "do_mpgoto" );

  one_argument( argument, arg );
  if ( !arg[0] )
  {
    mudlog( LOG_DEBUG , "Mpgoto - 號碼 %d 沒有參數" , ch->pIndexData->vnum );
    RETURN_NULL();
  }

  if ( !( location = find_location( ch, arg ) ) )
  {
    mudlog( LOG_DEBUG , "Mpgoto - 號碼 %d 去的地方 %s 從缺."
      , ch->pIndexData->vnum, arg );
    RETURN_NULL();
  }

  if ( ch->fighting ) stop_fighting( ch, TRUE );

  char_from_room( ch );
  char_to_room( ch, location );

  RETURN_NULL();
}

FUNCTION( do_mpat )
{
  char              arg[ MAX_INPUT_LENGTH ];
  ROOM_INDEX_DATA * location;
  ROOM_INDEX_DATA * original;
  CHAR_DATA       * wch;

  PUSH_FUNCTION( "do_mpat" );

  argument = one_argument( argument, arg );

  if ( !arg[0] || !argument[0] )
  {
    mudlog( LOG_DEBUG , "Mpat - 號碼 %d 錯誤的參數." , ch->pIndexData->vnum );
    RETURN_NULL();
  }

  if ( !( location = find_location( ch, arg ) ) )
  {
    mudlog( LOG_DEBUG , "Mpat - 號碼 %d 操作的地方 %s 不存在."
      , ch->pIndexData->vnum, arg );
    RETURN_NULL();
  }

  original = ch->in_room;
  char_from_room( ch );
  char_to_room( ch, location );
  interpret( ch, argument );

  for ( wch = char_list; wch; wch = wch->next )
  {
    if ( !verify_char( wch ) ) continue;

    if ( wch == ch )
    {
      char_from_room( ch );
      char_to_room( ch, original );
      break;
    }
  }
  RETURN_NULL();
}

FUNCTION( do_mptransfer )
{
  char              arg1[ MAX_INPUT_LENGTH ];
  char              arg2[ MAX_INPUT_LENGTH ];
  ROOM_INDEX_DATA * location;
  DESCRIPTOR_DATA * d;
  CHAR_DATA       * victim;

  PUSH_FUNCTION( "do_mptransfer" );

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );

  if ( !arg1[0] )
  {
    mudlog( LOG_DEBUG , "Mptransfer - 號碼 %d 語法錯誤."
      , ch->pIndexData->vnum );
    RETURN_NULL();
  }

  if ( !str_cmp( arg1, "all" ) )
  {
    for ( d = descriptor_list; d; d = d->next )
    {
      if ( !verify_desc( d ) ) continue;

      if ( d->connected == CON_PLAYING
        && d->character != ch
        && d->character->in_room
        && can_see( ch, d->character ) )
      {
        char buf[MAX_STRING_LENGTH];

        sprintf( buf, "%s %s", d->character->name, arg2 );
        do_transfer( ch, buf );
      }
    }
    RETURN_NULL();
  }

  if ( !arg2[0] )
  {
    location = ch->in_room;
  }
  else
  {
    if ( !( location = find_location( ch, arg2 ) ) )
    {
      mudlog( LOG_DEBUG , "Mptransfer : 號碼 %d 操作的地方 %s 不存在."
        , ch->pIndexData->vnum, arg2 );
      RETURN_NULL();
    }

    if ( room_is_private( location ) )
    {
      mudlog( LOG_DEBUG , "Mptransfer : 號碼 %d 操作的地方是私人地方 %d."
        , ch->pIndexData->vnum, location->vnum );
      RETURN_NULL();
    }
  }

  if ( !( victim = get_char_world( ch, arg1 ) ) )
  {
    mudlog( LOG_DEBUG , "Mptransfer : 號碼 %d 找不到人 %s."
      , ch->pIndexData->vnum , arg1 );
    RETURN_NULL();
  }

  if ( !victim->in_room )
  {
    mudlog( LOG_DEBUG , "Mptransfer : 號碼 %d 的對象 %s 在 Limbo."
      , ch->pIndexData->vnum, victim->name );
    RETURN_NULL();
  }

  if ( victim->fighting ) stop_fighting( victim, TRUE );

  char_from_room( victim );
  char_to_room( victim, location );

  RETURN_NULL();
}

FUNCTION( do_mpforce )
{
  char        arg[ MAX_INPUT_LENGTH ];
  CHAR_DATA * vch;
  CHAR_DATA * vch_next;
  CHAR_DATA * victim;

  PUSH_FUNCTION( "do_mpforce" );

  argument = one_argument( argument, arg );

  if ( !arg[0] || !argument[0] )
  {
    mudlog( LOG_DEBUG , "Mpforce: 號碼 %d 語法錯誤.", ch->pIndexData->vnum );
    RETURN_NULL();
  }

  if ( !str_cmp( arg, "all" ) )
  {
    /* 取得有效人物 */
    for ( vch = char_list; vch; vch = vch_next )
    {
      vch_next = vch->next;

      if ( !verify_char( vch ) ) continue;

      if ( vch->in_room == ch->in_room
        && get_trust( vch ) < get_trust( ch )
        && can_see( ch, vch ) )
      {
        interpret( vch, argument );
      }
    }
  }
  else
  {
    if ( !( victim = get_char_room( ch, arg ) ) )
    {
      mudlog( LOG_DEBUG , "Mpforce : 號碼 %d 找不到對象 %s."
        , ch->pIndexData->vnum, arg );
      RETURN_NULL();
    }

    if ( victim == ch )
    {
      mudlog( LOG_DEBUG , "Mpforce : 號碼 %d 命令自己."
        , ch->pIndexData->vnum );
      RETURN_NULL();
    }

    interpret( victim, argument );
  }

  RETURN_NULL();
}

FUNCTION( do_mpadd )
{
  CHAR_DATA * victim;
  char        arg1[MAX_INPUT_LENGTH];
  char        arg2[MAX_INPUT_LENGTH];
  char        arg3[MAX_INPUT_LENGTH];
  int         location;
  int         value;

  PUSH_FUNCTION( "do_mpadd" );

  if ( !ch->in_room )
  {
    mudlog( LOG_DEBUG, "do_mpadd: 號碼 %d 房間來源有誤.", ch->pIndexData->vnum );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );
  argument = one_argument( argument, arg3 );

  if ( !arg1[0] || !arg2[0] || !arg3[0] )
  {
    mudlog( LOG_DEBUG , "do_mpadd: 號碼 %d 語法錯誤.", ch->pIndexData->vnum );
    RETURN_NULL();
  }

       if ( !str_cmp( arg2, "str"        ) ) location = LOC_STR;
  else if ( !str_cmp( arg2, "int"        ) ) location = LOC_INT;
  else if ( !str_cmp( arg2, "wis"        ) ) location = LOC_WIS;
  else if ( !str_cmp( arg2, "dex"        ) ) location = LOC_DEX;
  else if ( !str_cmp( arg2, "con"        ) ) location = LOC_CON;
  else if ( !str_cmp( arg2, "train"      ) ) location = LOC_TRAIN;
  else if ( !str_cmp( arg2, "nskill"     ) ) location = LOC_NSKILL;
  else if ( !str_cmp( arg2, "firman"     ) ) location = LOC_FIRMAN;
  else if ( !str_cmp( arg2, "level"      ) ) location = LOC_LEVEL;
  else if ( !str_cmp( arg2, "pills"      ) ) location = LOC_PILLS;
  else if ( !str_cmp( arg2, "dosage"     ) ) location = LOC_DOSAGE;
  else if ( !str_cmp( arg2, "rechristen" ) ) location = LOC_RECHR;
  else if ( !str_cmp( arg2, "maxhit"     ) ) location = LOC_MAXHIT;
  else if ( !str_cmp( arg2, "maxmana"    ) ) location = LOC_MAXMANA;
  else if ( !str_cmp( arg2, "maxmove"    ) ) location = LOC_MAXMOVE;
  else if ( !str_cmp( arg2, "hit"        ) ) location = LOC_HIT;
  else if ( !str_cmp( arg2, "mana"       ) ) location = LOC_MANA;
  else if ( !str_cmp( arg2, "move"       ) ) location = LOC_MOVE;
  else
  {
    mudlog( LOG_DEBUG , "do_mpadd: 號碼 %d 參數 2 (%s) 錯誤."
      , ch->pIndexData->vnum, arg2 );
    RETURN_NULL();
  }

  if ( !is_number( arg3 ) )
  {
    mudlog( LOG_DEBUG , "do_mpadd: 號碼 %d 參數3 錯誤.", ch->pIndexData->vnum );
    RETURN_NULL();
  }

  value = atoi( arg3 );

  if ( !( victim = get_char_room( ch, arg1 ) ) )
  {
    mudlog( LOG_DEBUG, "do_mpadd: 怪物號碼 %d 找不到人名 %s."
      , ch->pIndexData->vnum, arg1 );
    RETURN_NULL();
  }

  if ( IS_NPC( victim ) )
  {
    mudlog( LOG_DEBUG, "do_mpadd: %d 對象是非玩家.", ch->pIndexData->vnum );
    RETURN_NULL();
  }

  switch( location )
  {
  default:
    mudlog( LOG_DEBUG , "do_mpadd: 號碼 %d 參數 2 (%d) 錯誤."
      , ch->pIndexData->vnum, location );

    RETURN_NULL();

  case LOC_STR:
    victim->now_str = UMIN( MaxStr, UMAX( 1, victim->now_str + value ) );
    break;

  case LOC_WIS:
    victim->now_wis = UMIN( MaxWis, UMAX( 1, victim->now_wis + value ) );
    break;

  case LOC_INT:
    victim->now_int = UMIN( MaxInt, UMAX( 1, victim->now_int + value ) );
    break;

  case LOC_DEX:
    victim->now_dex = UMIN( MaxDex, UMAX( 1, victim->now_dex + value ) );
    break;

  case LOC_CON:
    victim->now_con = UMIN( MaxCon, UMAX( 1, victim->now_con + value ) );
    break;

  case LOC_TRAIN:
    victim->practice = UMIN( MaxPractice, UMAX( 0, victim->practice + value ) );
    break;

  case LOC_NSKILL:
    victim->nskill = UMIN( MaxNSkill, UMAX( 0, victim->nskill + value ) );
    break;

  case LOC_FIRMAN:
    victim->firman = UMIN( MaxFirman, UMAX( 0, victim->firman + value ) );
    break;

  case LOC_LEVEL:
    victim->level = UMIN( LEVEL_HERO, UMAX( 1, victim->level + value ) );
    break;

  case LOC_PILLS:

    if ( victim->pcdata ) victim->pcdata->pills =
      UMIN( 10000, UMAX( 0, victim->pcdata->pills + value ) );

    break;

  case LOC_DOSAGE:

    if ( victim->pcdata ) victim->pcdata->dosage =
      UMIN( 10000, UMAX( 0, victim->pcdata->dosage + value ) );

    break;

  case LOC_RECHR:

    if ( victim->pcdata ) victim->pcdata->rechristen =
      UMIN( 100, UMAX( 0, victim->pcdata->rechristen + value ) );

    break;

  case LOC_MAXHIT:
    victim->max_hit = UMIN( MaxHit, UMAX( 1, get_curr_hit( victim ) + value ) );
    break;

  case LOC_HIT:
    victim->hit = UMIN( victim->max_hit, UMAX( 1, victim->hit + value ) );
    break;

  case LOC_MAXMANA:
    victim->max_mana = UMIN( MaxMana, UMAX( 1, get_curr_mana( victim ) + value ) );
    break;

  case LOC_MANA:
    victim->mana = UMIN( victim->max_mana, UMAX( 0, victim->mana + value ) );
    break;

  case LOC_MAXMOVE:
    victim->max_move = UMIN( MaxMove, UMAX( 1, get_curr_move( victim ) + value ) );
    break;

  case LOC_MOVE:
    victim->move = UMIN( victim->max_move, UMAX( 0, victim->move + value ) );
    break;
  }

  RETURN_NULL();
}

FUNCTION( do_mpclean )
{
  char       arg1[MAX_INPUT_LENGTH];
  char       arg2[MAX_INPUT_LENGTH];
  bool       bExtract;
  OBJ_DATA * pObj;
  OBJ_DATA * zObj;

  PUSH_FUNCTION( "do_mpclean" );

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );

  if ( !arg1[0] )
  {
    mudlog( LOG_DEBUG , "do_mpclean: 號碼 %d 缺乏參數一."
      , ch->pIndexData->vnum );
    RETURN_NULL();
  }

  if ( !str_cmp( arg1, "gold" )
    || !str_cmp( arg1, "coin" )
    || !str_cmp( arg1, "coins" ) )
  {
    for ( bExtract = FALSE, pObj = ch->carrying; pObj; pObj = zObj )
    {
      zObj = pObj->next_content;

      if ( pObj->wear_loc == WEAR_NONE && pObj->item_type == ITEM_MONEY )
      {
        bExtract = TRUE;
        obj_from_char( pObj );
        extract_obj( pObj );
      }
    }

    if ( bExtract == FALSE )
      mudlog( LOG_DEBUG , "do_mpclean: 怪物號碼 %d 沒有消掉任何金錢."
      , ch->pIndexData->vnum, arg2 );

    RETURN_NULL();
  }

  else if ( !str_cmp( arg1, "obj" ) )
  {
    /* 沒有參數就是全部銷毀 */
    if ( arg2[0] == '\x0' )
    {
      for ( bExtract = FALSE, pObj = ch->carrying; pObj; pObj = zObj )
      {
        zObj = pObj->next_content;

        if ( pObj->wear_loc == WEAR_NONE )
        {
          bExtract = TRUE;
          obj_from_char( pObj );
          extract_obj( pObj );
        }
      }

      if ( bExtract == FALSE )
        mudlog( LOG_DEBUG , "do_mpclean: 怪物號碼 %d 沒有消掉任何物品."
        , ch->pIndexData->vnum, arg2 );

      RETURN_NULL();
    }

    for ( pObj = ch->carrying; pObj; pObj = pObj->next_content )
    {
      if ( !str_cmp( arg2, pObj->name ) && pObj->wear_loc == WEAR_NONE )
      {
        obj_from_char( pObj );
        extract_obj( pObj );
        RETURN_NULL();
      }
    }

    mudlog( LOG_DEBUG , "do_mpclean: 號碼 %d 參數(2) %s 錯誤."
      , ch->pIndexData->vnum, arg2 );

    RETURN_NULL();
  }

  else
  {
    mudlog( LOG_DEBUG , "do_mpclean: 號碼 %d 參數(1) %s 錯誤."
      , ch->pIndexData->vnum, arg1 );

    RETURN_NULL();
  }

  RETURN_NULL();
}

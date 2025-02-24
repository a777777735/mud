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

#define NEXT_COLUMN	2

FUNCTION( do_wizhelp )
{
  CMD_DATA * pCommand;
  int        turn;
  char       command[MAX_INPUT_LENGTH];

  PUSH_FUNCTION( "do_wizhelp" );

  one_argument( argument , command );
  clear_buffer();

  if ( !command[0] )
  {
    for ( turn = 0, pCommand = cmd_list; pCommand; pCommand = pCommand->next )
    {
      if ( pCommand->level > LEVEL_HERO
        && pCommand->level <= get_trust( ch )
        && !pCommand->mobonly )
      {
        send_to_buffer( "%-12s %-20s" , pCommand->name , pCommand->cname );
        if ( ( ++turn % NEXT_COLUMN ) == 0 ) send_to_buffer( "\n\r" );
        if ( buffer_full() ) break;
      }
    }
    send_to_buffer( "\n\r" );
  }

  else if ( !str_cmp( command , "-a" ) )
  {
    for ( pCommand = cmd_list; pCommand; pCommand = pCommand->next )
    {
      if ( pCommand->level > LEVEL_HERO
        && pCommand->level <= get_trust( ch ) )
        send_to_buffer( "指令 \e[1;32m%-12s\e[0m "
                        "說明 \e[1;33m%-14s\e[0m "
                        "狀態:\e[1;35m%-4s\e[0m "
                        "等級:[ \e[1;31m%3d\e[0m ] "
                        "標記狀態:\e[1;36m%-10s\e[0m\n\r"
          , pCommand->name
          , pCommand->cname
          , get_position( pCommand->position )
          , pCommand->level
          , get_log( pCommand->log ) );

       if ( buffer_full() ) break;
     }
   }

   else
   {
     send_to_char( "參數錯誤。\n\r", ch );
     RETURN_NULL();
   }

   print_buffer( ch );
   RETURN_NULL();
}

#undef NEXT_COLUMN

/* 更改神族現身串 bamfin */
FUNCTION( do_bamfin )
{
  char buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "do_bamfin" );

  if ( !IS_NPC( ch ) )
  {
    ansi_transcribe( argument, buf );
    smash_tilde( argument );

    free_string( ch->pcdata->bamfin );
    ch->pcdata->bamfin = str_dup( buf );

    if ( *argument )
      act( "你把神族現身字串改為$t。", ch, buf, NULL, TO_CHAR );

    else
      send_to_char( "你把你的神族現身字串刪除了。\n\r" , ch );
  }

  RETURN_NULL();
}

/* 更改神族隱身字串 bamfout */
FUNCTION( do_bamfout )
{
  char buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "do_bamfout" );

  if ( !IS_NPC( ch ) )
  {
    ansi_transcribe( argument, buf );
    smash_tilde( argument );

    free_string( ch->pcdata->bamfout );
    ch->pcdata->bamfout = str_dup( buf );

    if ( * argument )
      act( "你把神族隱形字串改為$t。", ch, buf, NULL, TO_CHAR );

    else
      send_to_char( "你把你的神族隱形字串刪除了。\n\r" , ch );
  }

  RETURN_NULL();
}

/* 把玩家列為拒絕往來戶 */
FUNCTION( do_deny )
{
  char        arg[MAX_INPUT_LENGTH];
  CHAR_DATA * victim;

  PUSH_FUNCTION( "do_deny" );

  one_argument( argument, arg );

  if ( !( victim = get_char_world( ch, arg ) ) )
  {
    send_to_char( "找不到你要拒絕的人。\n\r", ch );
    RETURN_NULL();
  }

  if ( IS_NPC( victim ) || !victim->pcdata )
  {
    act( "不能用在非玩家$N身上。", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( get_trust( victim ) >= get_trust( ch ) )
  {
    act( "你的等級並沒有比$N高。", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  victim->pcdata->deny = TRUE;
  send_to_char( "你已經被列入拒絕往來戶了。\n\r", victim );
  act( "好了﹐你把$N給列入拒絕往來使用者。", ch, NULL, victim, TO_CHAR );
  do_quit( victim, "" );
  RETURN_NULL();
}

FUNCTION( do_disconnect )
{
  char              arg[MAX_INPUT_LENGTH];
  DESCRIPTOR_DATA * d;
  CHAR_DATA       * victim;

  PUSH_FUNCTION( "do_disconnect" );

  one_argument( argument, arg );

  if ( !( victim = get_char_world( ch, arg ) ) )
  {
    send_to_char( "找不到你想切斷連線的人。\n\r", ch );
    RETURN_NULL();
  }

  if ( !victim->desc )
  {
    act( "$N並沒有描述表﹐無法切斷連線。", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  for ( d = descriptor_list; d ; d = d->next )
  {
    if ( !verify_desc( d ) ) continue;

    if ( d == victim->desc )
    {
      act( "好了﹐你把$N的連線給切斷了。", ch, NULL, victim, TO_CHAR );
      close_socket( d );
      RETURN_NULL();
    }
  }

  mudlog( LOG_DEBUG , "Do_disconnect: 描述表找不到." );
  act( "有這個玩家$N﹐卻找不到他的描述表。", ch, NULL, victim, TO_CHAR );
  RETURN_NULL();
}

/* 饒恕玩家 */
FUNCTION( do_pardon )
{
  char         arg1[MAX_INPUT_LENGTH];
  char         arg2[MAX_INPUT_LENGTH];
  int          count;
  CHAR_DATA  * victim;
  ENEMY_DATA * pEnemy;

  PUSH_FUNCTION( "do_pardon" );

  argument = one_argument( argument, arg1 );

  if ( arg1[0] == '!' )
  {
    if ( arg1[1] == '\x0' )
    {
      send_to_char( "你要查詢哪一個人呢﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( !( victim = get_char_world( ch, arg1 + 1 ) ) )
    {
      send_to_char( "找不到你要查詢的人。\n\r", ch );
      RETURN_NULL();
    }

    if ( IS_NPC( victim ) )
    {
      act( "非玩家$N沒有這一方面的資料。", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    clear_buffer();
    send_to_buffer(
      "%s的犯罪資料有﹕\n\r殺人犯 %s\n\r盜賊   %s\n\r叛幫者 %s\n\r"
      , mob_name( NULL, victim )
      , YESNO( IS_SET( victim->act, PLR_KILLER ) )
      , YESNO( IS_SET( victim->act, PLR_THIEF  ) )
      , YESNO( IS_SET( victim->act, PLR_BOLTER ) ) );

    if ( victim->enemy )
    {
      send_to_buffer( "公敵資料有﹕\n\r" );
      for ( count = 0, pEnemy = victim->enemy; pEnemy; pEnemy = pEnemy->next )
        send_to_buffer( "%2d. %s\n\r", ++count, pEnemy->city );
    }

    print_buffer( ch );

    RETURN_NULL();
  }

  one_argument( argument, arg2 );

  if ( !arg1[0] || !arg2[0] )
  {
    send_to_char( "你的語法錯誤﹐請查詢使用方法。\n\r", ch );
    RETURN_NULL();
  }

  if ( !( victim = get_char_world( ch, arg1 ) ) )
  {
    send_to_char( "找不到你要饒恕的人。\n\r", ch );
    RETURN_NULL();
  }

  if ( IS_NPC( victim ) )
  {
    act( "非玩家 $N 沒有這一方面的資料。", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "killer" ) )
  {
    if ( IS_SET( victim->act, PLR_KILLER ) )
    {
      REMOVE_BIT( victim->act, PLR_KILLER );
      act( "$N殺人犯的旗標已經清除完畢。", ch, NULL, victim, TO_CHAR );
      send_to_char( "感謝神﹐你不再是殺人犯了。\n\r", victim );
    }

    else
    {
      act( "$N本來就不是殺人犯﹐動作取消。" , ch, NULL, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "thief" ) )
  {
    if ( IS_SET( victim->act, PLR_THIEF ) )
    {
      REMOVE_BIT( victim->act, PLR_THIEF );
      act( "$N竊賊旗標已經移除完畢。", ch, NULL, victim, TO_CHAR );
      send_to_char( "感謝神﹐你不再是竊賊了。\n\r", victim );
    }

    else
    {
      act( "$N本來就不是竊賊﹐動作取消。" , ch, NULL, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "bolter" ) )
  {
    if ( IS_SET( victim->act, PLR_BOLTER ) )
    {
      REMOVE_BIT( victim->act, PLR_BOLTER );
      act( "$N叛幫者的旗標已經清除完畢。", ch, NULL, victim, TO_CHAR );
      send_to_char( "感謝神﹐你不再是叛幫者了。\n\r", victim );
    }

    else
    {
      act( "$N本來就不是叛幫者﹐動作取消。", ch, NULL, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  if ( check_enemy( victim, arg2 ) )
  {
    act( "你移除$N的$t公敵旗標。", ch, arg2, victim, TO_CHAR );

    if ( victim != ch )
      act( "感謝神﹐移除你$t的公敵旗標。", victim, arg2, NULL, TO_CHAR );

    rem_enemy( victim, arg2 );
    RETURN_NULL();
  }

  send_to_char( "沒有這個關鍵字﹐請查詢 pardon 使用方法。\n\r", ch );
  RETURN_NULL();
}

FUNCTION( do_aecho )
{
  char              arg[MAX_INPUT_LENGTH];
  char              buf[MAX_STRING_LENGTH];
  int               count;
  DESCRIPTOR_DATA * pDesc;

  PUSH_FUNCTION( "do_aecho" );

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' || argument[0] == '\x0' )
  {
    send_to_char( "你要對這個位址廣播什麼呢﹖\n\r", ch );
    RETURN_NULL();
  }

  ansi_transcribe( argument, buf );

  for ( count = 0, pDesc = descriptor_list; pDesc; pDesc = pDesc->next )
  {
    if ( verify_desc( pDesc )
      && pDesc->connected == CON_PLAYING
      && pDesc->character
      && pDesc->character != ch
      && ( !str_cmp( pDesc->host, arg ) || !str_cmp( pDesc->address, arg ) ) )
    {
      act( "「$2位址廣播$0」 $7$t$0", pDesc->character, buf, NULL, TO_CHAR );
      count++;
    }
  }

  if ( count <= 0 )
  {
    send_to_char( "對不起﹐找不到這個位址的任何玩家。\n\r", ch );
  }
  else
  {
    act( "你總共對$i位玩家廣播。", ch, &count, NULL, TO_CHAR );
  }

  RETURN_NULL();
}

/* 神族廣播 */
FUNCTION( do_echo )
{
  DESCRIPTOR_DATA * man;
  char              buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "do_echo" );

  if ( !argument || !*argument ) RETURN_NULL();

  ansi_transcribe( argument, buf );

  for ( man = descriptor_list; man; man = man->next )
  {
    if ( verify_desc( man )
      && man->connected == CON_PLAYING
      && man->character )
    {
      act( "「$2全體廣播$0」 $7$t$0", man->character, buf, NULL, TO_CHAR );
    }
  }

  RETURN_NULL();
}

/* 在某間房間做廣播 */
FUNCTION( do_recho )
{
  DESCRIPTOR_DATA * d;
  char              buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "do_recho" );

  if ( !ch || !ch->in_room )
  {
    mudlog( LOG_DEBUG, "do_recho: 參數錯誤." );
    RETURN_NULL();
  }

  ansi_transcribe( argument, buf );
  for ( d = descriptor_list; d; d = d->next )
  {
    if ( !verify_desc( d ) ) continue;

    if ( d->connected == CON_PLAYING
      && d->character
      && d->character->in_room == ch->in_room )
    {
      act( "「$2房間廣播$0」 $7$t$0", d->character, buf, NULL, TO_CHAR );
    }
  }
  RETURN_NULL();
}

ROOM_INDEX_DATA * find_location( CHAR_DATA * ch, char * arg )
{
  CHAR_DATA * victim;
  OBJ_DATA  * obj;

  PUSH_FUNCTION( "find_location" );

  if ( is_number(arg) ) RETURN( get_room_index( atoi( arg ) ) );
  if ( ( victim = get_char_world( ch, arg ) ) ) RETURN( victim->in_room );
  if ( ( obj = get_obj_world( ch, arg ) ) )     RETURN( obj->in_room );
  RETURN( NULL );
}

FUNCTION( do_transfer )
{
  char              arg1[MAX_INPUT_LENGTH];
  char              arg2[MAX_INPUT_LENGTH];
  char              arg3[ MAX_INPUT_LENGTH];
  ROOM_INDEX_DATA * location;
  DESCRIPTOR_DATA * d;
  CHAR_DATA       * victim;

  PUSH_FUNCTION( "do_transfer" );

  argument = one_argument( argument, arg1 );
             one_argument( argument, arg2 );

  if ( !str_cmp( arg1, "all" ) )
  {
    for ( d = descriptor_list; d; d = d->next )
    {
      if ( !verify_desc( d ) ) continue;

      if ( d->connected == CON_PLAYING
        && d->character
        && d->character != ch
        && d->character->in_room
        && can_see( ch, d->character ) )
      {
        sprintf( arg3, "%s %s", d->character->name, arg2 );
        do_transfer( ch, arg3 );
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
      send_to_char( "沒有這個地方。\n\r", ch );
      RETURN_NULL();
    }

    if ( room_is_private( location ) )
    {
      send_to_char( "那間房間是私人房間。\n\r", ch );
      RETURN_NULL();
    }
  }

  if ( !( victim = get_char_world( ch, arg1 ) ) )
  {
    send_to_char( "找不到你要傳送的人。\n\r", ch );
    RETURN_NULL();
  }

  if ( ch == victim )
  {
    send_to_char( "幹嗎傳送自己呢﹖\n\r", ch );
    RETURN_NULL();
  }

  if ( !victim->in_room )
  {
    act( "$N在虛無飄渺的Limbo﹐所以不能被傳送。", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( is_dead( victim ) )
  {
    act( "$N還在處理他的身後大事耶﹗", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( victim->fighting ) stop_fighting( victim, TRUE );

  act( "$n消失在一團紫霧之間。", victim, NULL, NULL, TO_ROOM );
  char_from_room( victim );
  char_to_room( victim, location );

  act( "$n在一團紫霧中走了出來。", victim, NULL, NULL, TO_ROOM );
  if ( ch != victim ) act( "$n正在傳送你。", ch, NULL, victim, TO_VICT );

  do_look( victim, "auto" );
  clear_trace( victim, TRUE );

  send_to_char( "好了﹐傳送完畢。\n\r", ch );
}

FUNCTION( do_at )
{
  char              arg[MAX_INPUT_LENGTH];
  ROOM_INDEX_DATA * location;
  ROOM_INDEX_DATA * original;
  CHAR_DATA       * wch;

  PUSH_FUNCTION( "do_at" );

  argument = one_argument( argument, arg );

  if ( !arg[0] || !argument[0] )
  {
    send_to_char( "語法﹕at <地點> <所要執行的動作>\n\r", ch );
    RETURN_NULL();
  }

  if ( !( location = find_location( ch, arg ) ) )
  {
    send_to_char( "沒有你要的地方。\n\r", ch );
    RETURN_NULL();
  }

  if ( room_is_private( location ) )
  {
    send_to_char( "這是一間私人房間。\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->in_room == location )
  {
    send_to_char( "這是相同的房間﹐不需要這樣做吧﹗\n\r", ch );
    RETURN_NULL();
  }

  original = ch->in_room;
  char_from_room( ch );
  char_to_room( ch, location );
  interpret( ch, argument );

  /* See if 'ch' still exists before continuing!
     Handles 'at XXXX quit' case. */

  for ( wch = char_list; wch; wch = wch->next )
  {
    if ( verify_char( wch ) && wch == ch )
    {
      char_from_room( ch );
      char_to_room( ch, original );
      break;
    }
  }

  RETURN_NULL();
}

FUNCTION( do_goto )
{
  CHAR_DATA       * victim;
  DESCRIPTOR_DATA * pDesc;
  char              arg[MAX_INPUT_LENGTH];
  ROOM_INDEX_DATA * location;

  PUSH_FUNCTION( "do_goto" );

  one_argument( argument, arg );

  if ( ch->position == POS_DEAD )
  {
    act( "請先復活再走吧﹗", ch, NULL, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( arg[0] == '@' )
  {
    if ( arg[1] == '\x0' || !ch->in_room )
    {
      send_to_char( "你要飛到哪位玩家身邊呢﹖\n\r", ch );
      RETURN_NULL();
    }

    location = NULL;
    for ( pDesc = descriptor_list; pDesc; pDesc = pDesc->next )
    {
      if ( verify_desc( pDesc )
        && ( victim = pDesc->character )
        && pDesc->connected == CON_PLAYING
        && !str_cmp( victim->name, arg + 1 )
        && can_see( ch, victim )
        && victim->in_room )
      {
        location = victim->in_room ;
        break;
      }
    }
  }

  else
  {
    location = find_location( ch, arg );
  }

  if ( !location )
  {
    send_to_char( "沒有這個地方﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( room_is_private( location ) )
  {
    send_to_char( "對不起﹐這個房間是私人房間。\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->in_room == location )
  {
    send_to_char( "你去的地方和你目前的所在是一樣的地方。\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->fighting ) stop_fighting( ch, TRUE );

  if ( !IS_NPC( ch ) && !IS_SET( ch->act, PLR_WIZINVIS ) )
  {
    act( "$n$T。", ch, NULL,
        ( ch->pcdata && *ch->pcdata->bamfout )
        ? ch->pcdata->bamfout
        : "在一陣狂風暴雨中逐漸消失他的身影", TO_ROOM );
  }

  char_from_room( ch );
  char_to_room( ch, location );

  if ( !IS_NPC( ch ) && !IS_SET( ch->act, PLR_WIZINVIS ) )
  {
    act( "$n$T。", ch, NULL,
        ( ch->pcdata && *ch->pcdata->bamfin )
        ? ch->pcdata->bamfin
        : "在一陣狂風暴雨中逐漸現出他高大的身影", TO_ROOM );
  }

  do_look( ch, "auto" );
  clear_trace( ch, TRUE );

  RETURN_NULL();
}

/* 觀看房間的狀態 */
FUNCTION( do_rstat )
{
  char              buf[MAX_STRING_LENGTH];
  char              arg[MAX_INPUT_LENGTH];
  ROOM_INDEX_DATA * location;
  OBJ_DATA        * obj;
  CHAR_DATA       * rch;
  CHAR_DATA       * owner;
  SHIP_DATA       * pShip;
  MINE_DATA       * pMine;
  ENQUIRE_DATA    * pEnquire;
  int               door;
  int               count;
  bool              found;

  PUSH_FUNCTION( "do_rstat" );

  one_argument( argument, arg );

  if ( !( location = arg[0] ? find_location( ch, arg ) : ch->in_room ) )
  {
    send_to_char( "沒有這個地方。\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->in_room != location && room_is_private( location ) )
  {
    send_to_char( "這間房間是私人房間。\n\r", ch );
    RETURN_NULL();
  }

  owner  = owner_name( location );

  clear_buffer();
  send_to_buffer(
    "所在名稱﹕\e[1;32m[\e[0m%s\e[1;32m]\e[0m\n\r"
    "所在區域﹕\e[1;32m[\e[0m%s\e[1;32m]\e[0m 區域屬性﹕%s\n\r"
    "檔案名稱﹕\e[1;32m[\e[0m%s\e[1;32m]\e[0m\n\r"
    "編號﹕(\e[1;36m%5d\e[0m)  "
    "區域屬性﹕%d 幫會﹕%s 版面﹕%s\n\r"
    "地形旗標﹕\e[1;34m%s\e[0m. "
    "照明與否﹕\e[1;35m%s\e[0m (%d)\n\r"
    "怪物重生﹕%s%d\e[0m\n\r"
    "房間旗幟﹕%s\n\r"
    "房間人數﹕%d 人 房間物品數﹕%d 個\n\r"
    "房間主人﹕%d-%d 名稱﹕%s\n\r"
    "是否可寫回﹕%s 土地價值﹕%d 土地累計金額﹕%d\n\r"
    "此地的完整描述﹕\n\r%s\e[0m\n\r"
    , location->name
    , location->area->name
    , area_bit_name( location->area )
    , location->filename
    , location->vnum
    , location->area->attribute
    , location->club ? location->club->cname : "沒有"
    , location->board ? location->board->name : "(沒有)"
    , location->sector->cname
    , YESNO( !room_is_dark( location ) )
    , location->light
    , location->mob_reborn > 0 ? "\e[1;32m" : "\e[0m"
    , location->mob_reborn
    , room_bit_name( location )
    , location->nplayer
    , location->nobject
    , location->owner.high
    , location->owner.low
    , location->owner.high > 0 && location->owner.low > 0
      ? fullname( &( location->owner ) ) : "沒有主人"
    , YESNO( location->writeable )
    , location->cost
    , location->gold
    , location->description );

  if ( location->job )
    send_to_buffer( "這個房間有特殊的函式。\n\r" );

  if ( location->extra_descr )
  {
    EXTRA_DESCR_DATA * ed;

    send_to_buffer( "附加描述關鍵字﹕'" );
    for ( ed = location->extra_descr; ed; ed = ed->next )
    {
      send_to_buffer( "%s", ed->keyword );
      if ( ed->next ) send_to_buffer( " " );
    }

    send_to_buffer( "'.\n\r" );
  }

  send_to_buffer( "此地的人物﹕" );
  for ( found = FALSE, rch = location->people; rch; rch = rch->next_in_room )
  {
    found = TRUE;
    send_to_buffer( " " );
    one_argument( rch->name, buf );
    send_to_buffer( "%s", buf );
    if ( buffer_full() ) break;
  }

  if ( !found ) send_to_buffer( "(無)" );
  send_to_buffer( "。\n\r此地的物品﹕" );
  found = FALSE;

  for ( obj = location->contents; obj; obj = obj->next_content )
  {
    found = TRUE;
    send_to_buffer( " " );
    one_argument( obj->name, buf );
    send_to_buffer( "%s", buf );
    if ( buffer_full() ) break;
  }

  if ( !found ) send_to_buffer( "(無)" );
  send_to_buffer( "。\n\r此地的礦物﹕" );
  found = FALSE;

  for ( pMine = location->mine; pMine; pMine = pMine->next )
  {
    if ( pMine->mineral && pMine->mineral->mineral )
    {
      send_to_buffer( " " );
      one_argument( pMine->mineral->mineral->name, buf );
      send_to_buffer( "%s", buf );
      found = TRUE;

      if ( buffer_full() ) break;
    }
  }

  if ( !found ) send_to_buffer( "(無)" );
  send_to_buffer( "。\n\r" );

  for ( door = 0; door < DIR_MAX; door++ )
  {
    EXIT_DATA * pexit;

    if ( ( pexit = location->exit[door] ) )
    {
      send_to_buffer(
        "往%s到房間 %d  "
        "鑰匙﹕%d.  "
        "出口狀態﹕%s\n\r"
        "關鍵字﹕%s  "
        "描述﹕%s\n\r",
        direction_name( door ),
        pexit->to_room ? pexit->to_room->vnum : 0,
        pexit->key,
        exit_status( pexit ),
        ( pexit->keyword && pexit->keyword[0] ) ? pexit->keyword : "(沒有)",
        ( pexit->description && pexit->description[0] )
         ? pexit->description : "(沒有)." );
    }
  }

  if ( location->enquire )
  {
    send_to_buffer( "問路關鍵字﹕" );

    for ( pEnquire = location->enquire; pEnquire; pEnquire = pEnquire->next )
      send_to_buffer( " %s", pEnquire->keyword );

    send_to_buffer( "\n\r" );
  }

  send_to_buffer( "渡船資料﹕" );
  count = 1;
  for ( found = FALSE, pShip = ship_list; pShip; pShip = pShip->next, count++ )
  {
    if ( pShip->starting == location )
    {
      found = TRUE;
      send_to_buffer( " 出發點(%d)", count );
    }

    if ( pShip->destination == location )
    {
      found = TRUE;
      send_to_buffer( " 目的地(%d)", count );
    }

    if ( pShip->cabin == location )
    {
      found = TRUE;
      send_to_buffer( " 船艙(%d)"  , count );
    }
  }

  if ( !found ) send_to_buffer( "(無)" );
  send_to_buffer( "\n\r" );
  print_buffer( ch );
  RETURN_NULL();
}

FUNCTION( do_olist )
{
  char             arg[ MAX_INPUT_LENGTH ];
  OBJ_INDEX_DATA * pObjIndex;
  AFFECT_DATA    * paf;
  RESTRICT_DATA  * pRest;
  MESSAGE_DATA   * pMessage;

  PUSH_FUNCTION( "do_olist" );

  one_argument( argument , arg );

  if ( ( pObjIndex = get_obj_protype( arg ) ) )
  {
    clear_buffer();
    send_to_buffer( "名字﹕%s  "
                    "編號﹕%d 單位﹕%s 消失﹕%s\n\r"
                    "短描述﹕%s\n\r"
                    "長描述﹕%s\n\r"
                    "種類﹕%s ( %d )\n\r"
                    "額外旗標﹕%s\n\r"
                    "穿戴旗標﹕%s ( %d ) 可拿﹕%s\n\r"
                    "數量﹕%d  "
                    "重量﹕%d  "
                    "等級﹕%d  "
                    "防護力﹕%d  "
                    "價值﹕%d "
                    "經驗值﹕萬分之 %d\n\r",
                    pObjIndex->name,
                    pObjIndex->vnum,
                    pObjIndex->unit ? pObjIndex->unit : "(沒有)",
                    YESNO( pObjIndex->disappear ),
                    pObjIndex->short_descr,
                    pObjIndex->description,
                    item_type_name( pObjIndex ),
                    pObjIndex->item_type,
                    extra_bit_name_2( pObjIndex ) ,
                    wear_bit_name( pObjIndex->wear_flags ),
                    pObjIndex->wear_flags,
                    YESNO( pObjIndex->Takeable ),
                    pObjIndex->count,
                    pObjIndex->weight,
                    pObjIndex->level,
                    pObjIndex->armor,
                    pObjIndex->cost,
                    pObjIndex->exp );

    send_to_buffer( "%s", get_obj_value_usage( pObjIndex->item_type ,
      pObjIndex->value ) );

    for ( pRest = pObjIndex->restrict; pRest; pRest = pRest->next )
      send_to_buffer( "限制型態﹕%s\n\r", restrict_value( pRest, ch ) );

    for ( pMessage = pObjIndex->message; pMessage; pMessage = pMessage->next )
      send_to_buffer(
        "\n\r敘述時機: %s\n\r輸出給自己字串:%s\n\r輸出給別人字串:%s\n\r"
        , message_type( pMessage->type ), pMessage->self, pMessage->others );

    for ( paf = pObjIndex->affected; paf; paf = paf->next )
    {
      send_to_buffer( "對%s造成 %d 的影響。\n\r"
                      , affect_loc_name( paf->location )
                      , paf->modifier );
    }

    for ( paf = pObjIndex->impact; paf; paf = paf->next )
    {
      send_to_buffer( "[魔石] 對%s造成 %d 的影響。\n\r"
                      , affect_loc_name( paf->location )
                      , paf->modifier );
    }

    print_buffer( ch );
  }

  else
  {
    send_to_char( "沒有找到你要找的物品原型。\n\r" , ch );
  }

  RETURN_NULL();
}

FUNCTION( do_ostat )
{
  char             arg[MAX_INPUT_LENGTH];
  AFFECT_DATA    * paf;
  OBJ_DATA       * obj;
  RESTRICT_DATA  * pRest;
  MESSAGE_DATA   * pMess;
  CACHET_DATA    * pCachet;
  OBJ_INDEX_DATA * pIndex;

  PUSH_FUNCTION( "do_ostat" );

  one_argument( argument, arg );

  if ( !( obj = get_obj_world( ch, arg ) ) )
  {
    send_to_char( "世界上沒有你要的東西。\n\r", ch );
    RETURN_NULL();
  }

  clear_buffer();
  send_to_buffer( "名稱﹕%s\n\r"
                  "號碼:%5d  "
                  "種類﹕%s 單位﹕%s%s 消失: %s 毒效﹕%d小時 \n\r"
                  "檔案名稱﹕%s\n\r"
                  "簡短的描述﹕%s\n\r"
                  "詳細的描述﹕%s\n\r"
                  "穿戴旗標﹕%s(%d) 可拿﹕%s\n\r"
                  "附加旗標﹕%s\n\r"
                  "總件數﹕%d  "
                  "重量﹕%d/%d "
                  "經驗值﹕萬分之 %d\n\r"
                  "價值﹕%d  "
                  "計時器﹕%d  "
                  "等級﹕%d\n\r"
                  "流水序號﹕%d-%d 擁有者序號﹕%d-%d\n\r"
                  "正確所在地﹕%s\n\r"
                  "裝備位置﹕%s ( %d )\n\r",
                  obj->name ,
                  obj->pIndexData->vnum ,
                  item_type_name( obj->pIndexData ) ,
                  obj->unit,
                  obj->pIndexData->unit ? "" : "<\e[1;32m預設值\e[0m>",
                  YESNO( obj->disappear ),
                  obj->venom,
                  obj->pIndexData->filename ,
                  obj->cname,
                  obj->description ,
                  wear_bit_name( obj->wear_flags ),
                  obj->wear_flags,
                  YESNO( obj->Takeable ),
                  extra_bit_name( obj ) ,
                  get_obj_number( obj ),
                  obj->weight,
                  get_obj_weight( obj ),
                  obj->pIndexData ? obj->pIndexData->exp : -1,
                  obj->cost,
                  obj->timer,
                  obj->level ,
                  obj->serial.high,
                  obj->serial.low,
                  obj->owner.high,
                  obj->owner.low,
                  object_locate( obj , ch ),
                  wear_location_string( obj->wear_loc ),
                  obj->wear_loc );

  /* 防具類有防護力 */
  if ( is_armor( obj ) && obj->max_armor > 0 )
  {
    send_to_buffer( "最大防護力﹕%d 目前防護力﹕%d。 (%d%%)\n\r"
      , obj->max_armor, obj->armor
      , obj->armor * 100 / obj->max_armor );
  }

  for ( pRest = obj->pIndexData->restrict; pRest; pRest = pRest->next )
    send_to_buffer( "限制型態﹕%s\n\r", restrict_value( pRest, ch ) );

  for ( pMess = obj->pIndexData->message; pMess; pMess = pMess->next )
    send_to_buffer(
      "\n\r敘述時機: %s\n\r輸出給自己字串:%s\n\r輸出給別人字串:%s\n\r"
        , message_type( pMess->type ), pMess->self, pMess->others );

  send_to_buffer( "%s", get_obj_value_usage( obj->item_type , obj->value ) );

  if ( obj->extra_descr || obj->pIndexData->extra_descr )
  {
    EXTRA_DESCR_DATA *ed;

    send_to_buffer( "額外描述關鍵字﹕'" );

    for ( ed = obj->extra_descr; ed; ed = ed->next )
    {
      send_to_buffer( "%s", ed->keyword );
      if ( ed->next ) send_to_buffer( " " );
    }

    for ( ed = obj->pIndexData->extra_descr; ed; ed = ed->next )
    {
      send_to_buffer( "%s", ed->keyword );
      if ( ed->next ) send_to_buffer( " " );
    }

    send_to_buffer( "'\n\r" );
  }

  for ( paf = obj->affected; paf; paf = paf->next )
  {
    send_to_buffer( "對%s造成 %d 的影響。\n\r"
                    , affect_loc_name( paf->location )
                    , paf->modifier );
  }

  for ( paf = obj->pIndexData->affected; paf; paf = paf->next )
  {
    send_to_buffer( "對%s造成 %d 的影響。\n\r"
      , affect_loc_name( paf->location )
      , paf->modifier );
  }

  for ( paf = obj->pIndexData->impact; paf; paf = paf->next )
  {
    send_to_buffer( "[魔石] 對%s造成 %d 的影響。\n\r"
      , affect_loc_name( paf->location )
      , paf->modifier );
  }

  for ( pCachet = obj->cachet; pCachet; pCachet = pCachet->next )
  {
    if ( !( pIndex = get_obj_index( pCachet->vnum ) ) ) continue;

    for ( paf = pIndex->impact; paf; paf = paf->next )
    {
      send_to_buffer( "[封印魔石] 對%s造成 %d 的影響。\n\r"
        , affect_loc_name( paf->location )
      , paf->modifier );
    }
  }

  print_buffer( ch );
  RETURN_NULL();
}

FUNCTION( do_mlist )
{
  char             arg[ MAX_INPUT_LENGTH ];
  MOB_INDEX_DATA * pMobIndex;
  TEACH_DATA     * teach;
  SKILL_DATA     * pSkill;
  ENABLE_DATA    * pEnable;
  AFFECT_DATA    * pAffect;
  int              count;

  PUSH_FUNCTION( "do_mlist" );

  one_argument( argument , arg );

  if ( ( pMobIndex = get_char_protype( argument ) ) )
  {
    clear_buffer();
    send_to_buffer( "名字﹕%s  "
                    "編號﹕%d\n\r"
                    "檔案名稱﹕%s\n\r"
                    "短敘述﹕%s\n\r"
                    "長敘述﹕%s\n\r"
                    "完整描述﹕%s\n\r"
                    "數量﹕%d  "
                    "被殺了 %d 隻 "
                    "性別﹕%s  "
                    "金錢﹕%d  "
                    "等級﹕%d  "
                    "陣營﹕%d\n\r"
                    "力量﹕%d  "
                    "智力﹕%d  "
                    "學識﹕%d  "
                    "敏捷﹕%d  "
                    "體格﹕%d\n\r"
                    "旗標﹕%s ( %d )\n\r"
                    "命中率﹕%d "
                    "防護力﹕%d "
                    "經驗值加成﹕%d "
                    "遷移機率﹕ %d%% "
                    "說話﹕ %s\n\r"
                    "攻擊加成﹕百分之 %d "
                    "閃躲加成﹕百分之 %d\n\r"
                    "Hitnodice﹕%d  "
                    "HitSizeDice﹕%d  "
                    "HitPlus﹕%d\n\r"
                    "DamNoDice﹕%d  "
                    "DamSizeDice﹕%d  "
                    "DamPlus﹕%d\n\r",
     pMobIndex->name,
     pMobIndex->vnum,
     pMobIndex->filename ,
     pMobIndex->short_descr,
     pMobIndex->long_descr,
     pMobIndex->description,
     pMobIndex->count,
     pMobIndex->killed,
     sex_name( pMobIndex->sex ),
     pMobIndex->gold,
     pMobIndex->level,
     pMobIndex->alignment,
     pMobIndex->now_str,
     pMobIndex->now_int,
     pMobIndex->now_wis,
     pMobIndex->now_dex,
     pMobIndex->now_con,
     act_bit_name( pMobIndex->act ),
     pMobIndex->act,
     pMobIndex->hitroll,
     pMobIndex->ac,
     pMobIndex->multipile,
     pMobIndex->migrate,
     YESNO( pMobIndex->speak ),
     pMobIndex->attack_ratio,
     pMobIndex->dodge_ratio,
     pMobIndex->hitnodice,
     pMobIndex->hitsizedice,
     pMobIndex->hitplus,
     pMobIndex->damnodice,
     pMobIndex->damsizedice,
     pMobIndex->damplus );

    send_to_buffer( "生命力﹕%d  法力﹕%d  體力﹕%d\n\r職業﹕%s\n\r"
      , pMobIndex->hit, pMobIndex->mana, pMobIndex->move
      , class_name( pMobIndex->class, TRUE ) );

    send_to_buffer( "程式型態﹕" );
    if ( pMobIndex->progtypes == ERROR_PROG )
    {
      send_to_buffer( "錯誤的型態. (%d) \n\r" , pMobIndex->progtypes );
    }
    else
    {
       send_to_buffer( "%s (%d)\n\r" ,
                        progtypes_bit_name( pMobIndex->progtypes ),
                        pMobIndex->progtypes );
    }

    send_to_buffer( "教導的技能﹕\n\r" );
    for ( teach = pMobIndex->teach, count = 0; teach ; teach = teach->next )
    {
      if ( ( pSkill = get_skill( teach->slot ) ) )
      {
        send_to_buffer( "( %2d ) 編號﹕%d 法術名稱﹕%s "
                        "熟練度﹕%d 教授金﹕%d 隱藏﹕%d%%\n\r"
          , ++count
          , pSkill->slot
          , pSkill->name
          , teach->adept
          , teach->cost
          , teach->inventory );
      }
    }

    if ( count == 0 ) send_to_buffer( "沒有任何的技能教導。\n\r" );
    send_to_buffer( "致能的技能﹕\n\r" );

    for ( count = 0, pEnable = pMobIndex->enable; pEnable; pEnable = pEnable->next )
    {
      if ( ( pSkill = pEnable->skill ) )
      {
        send_to_buffer( "( %2d ) 編號﹕%3d 法術名稱﹕%-30s 熟練度﹕%2d%%\n\r"
          , ++count, pSkill->slot, pSkill->name
          , pMobIndex->skill[pSkill->slot] );
      }
    }

    if ( count == 0 ) send_to_buffer( "沒有任何的技能致能。\n\r" );

    for ( pAffect = pMobIndex->affected; pAffect; pAffect = pAffect->next )
    {
      if ( !( pSkill = get_skill( pAffect->type ) ) )
      {
        mudlog( LOG_DEBUG, "do_mstat: 錯誤的型態 %d.", pAffect->type );
        continue;
      }

      send_to_buffer( "法術﹕%s對%s造成 %d 影響還有 %d 小時\n\r",
        pSkill->cname,
        affect_loc_name( pAffect->location ),
        pAffect->modifier,
        pAffect->duration );
    }

    print_buffer( ch );
  }

  else
  {
    send_to_char( "沒有找到你要找的怪物原型。\n\r" , ch );
  }

  RETURN_NULL();
}

FUNCTION( do_mstat )
{
  char              arg[MAX_INPUT_LENGTH];
  AFFECT_DATA     * paf;
  CHAR_DATA       * victim;
  SKILL_DATA      * pSkill;
  ENROLL_DATA     * pEnroll;
  DESCRIPTOR_DATA * pDesc;
  QUEST_DATA      * pQuest;
  int               loop;

  PUSH_FUNCTION( "do_mstat" );

  one_argument( argument, arg );

  if ( !( victim = get_char_world( ch, arg ) ) )
  {
    send_to_char( "你的目標不在這裡。\n\r", ch );
    RETURN_NULL();
  }

  clear_buffer();
  send_to_buffer( "名字﹕%s  "
                  "序號﹕%d-%d "
                  "中文名字﹕%s "
                  "幫派 %s\n\r"
                  "號碼﹕%d  "
                  "性別﹕%s  "
                  "所在房間﹕%d "
                  "所屬區域﹕%s\n\r"
                  "主人序號﹕%d-%d\n\r"
                  "力量﹕%d "
                  "智慧﹕%d "
                  "學識﹕%d "
                  "速度﹕%d "
                  "體格﹕%d\n\r"
                  "極限技﹕%d 免死金牌﹕%d "
                  "監獄天數﹕%d 思過﹕%d 不雅字﹕%d 改名﹕%d\n\r"
                  "生命﹕%d/%d "
                  "法力﹕%d/%d "
                  "體力﹕%d/%d "
                  "學習點數﹕%d\n\r"
                  "等級﹕%d "
                  "職業﹕%s "
                  "陣營﹕%d "
                  "防護力﹕%d "
                  "技能加成﹕ %d\n\r"
                  "金錢﹕%d "
                  "錢莊﹕%d "
                  "經驗值﹕%d \n\r"
                  "命中率﹕%d "
                  "傷害力﹕%d "
                  "逃跑點數﹕%d "
                  "偷竊時間﹕%d ",
      victim->name , victim->serial.high, victim->serial.low,
      IS_NPC( victim ) ? "(從缺)" : victim->cname ,
      club_name( victim ),
      IS_NPC( victim ) ? victim->pIndexData->vnum : 0 ,
      sex_name( victim->sex ),
      !victim->in_room ? -1 : victim->in_room->vnum ,
      ( victim->in_room ) ? victim->in_room->area->name : "" ,
      victim->jade.high, victim->jade.low,
      get_curr_str(victim),
      get_curr_int(victim),
      get_curr_wis(victim),
      get_curr_dex(victim),
      get_curr_con(victim),
      victim->limit, victim->firman, victim->jail, victim->failed,
      victim->pcdata ? victim->pcdata->xname : 0,
      victim->pcdata ? victim->pcdata->rechristen : 0,
      victim->hit,         get_curr_hit( victim ),
      victim->mana,        get_curr_mana( victim ),
      victim->move,        get_curr_move( victim ),
      victim->practice,
      victim->level,
      class_name( victim->class, TRUE ),
      victim->alignment,
      get_ac(victim),
      ch->nskill,
      victim->gold,
      victim->bank,
      victim->exp,
      get_hitroll( victim ),
      get_damroll( victim ),
      victim->wimpy,
      victim->pcdata ? victim->pcdata->steal_panity : 0 );

  send_to_buffer( "狀態﹕%s\n\r", get_position( victim->position ) );
  send_to_buffer( "釋放旗標﹕%s。\n\r", YESNO( !( verify_char( ch ) ) ) );

  if ( victim->spirit && !victim->boss )
  {
    CHAR_DATA * spirit;

    send_to_buffer( "身邊小鬼﹕" );

    for ( spirit = victim->spirit; spirit; spirit = spirit->spirit )
      send_to_buffer( " %s", mob_name( NULL, spirit ) );

    send_to_buffer( "\n\r" );
  }

  if ( victim->boss )
    send_to_buffer( "小鬼擁有者﹕%s\n\r", victim->boss->name );

  if ( !IS_NPC( victim ) && victim->pcdata )
  {
    send_to_buffer( "求婚者﹕%s 配偶﹕%s 是否在線上﹕%s\n\r"
      , victim->pcdata->wooer
        ? victim->pcdata->wooer->name : "(從缺)"
      , victim->pcdata->mater && *victim->pcdata->mater
        ? victim->pcdata->mater : "(從缺)"
      , YESNO( victim->pcdata->mate ) );

    send_to_buffer( "PK贏﹕%d PK輸﹕%d 對戰贏: %d 對戰輸: %d"
                    "殺死怪物 %d 隻﹐共 %d 級 平均 %d 級\n\r"
      , victim->pcdata->pkwin
      , victim->pcdata->pklost
      , victim->pcdata->fightwin
      , victim->pcdata->fightlost
      , victim->pcdata->kills
      , victim->pcdata->kill_level
      , ( victim->pcdata->kills == 0 ) ? 0 :
          victim->pcdata->kill_level / victim->pcdata->kills );

    send_to_buffer( "換頁行數﹕%d. "
                    "屍體狀態﹕%s. "
                    , victim->pcdata->pagelen
                    , victim->pcdata->corpse ? "有" : "沒有" );

    if ( victim->pcdata->corpse )
      send_to_buffer( "(屍體所在地﹕%d ) " ,
        victim->pcdata->corpse->in_room ?
        victim->pcdata->corpse->in_room->vnum : -1 );

    send_to_buffer(
      "巨集總個數﹕%d 預設技能總數﹕%d\n\r"
      "電子郵件信箱﹕%s\n\r"
      , get_alias_count( victim )
      , enable_count( victim )
      , victim->email && *victim->email ? victim->email : "(從缺)" );

    if ( ( pDesc = victim->desc ) )
    {
      check_multi_update();

      send_to_buffer( "計數狀態﹕%s 計數器時脈﹕%d 計數器命令﹕%s\n\r"
        , pDesc->tick_count >= 0 ? "打開" : "關閉"
        , pDesc->tick
        , pDesc->tick == -1 ? "從缺" : pDesc->tick_command );

      send_to_buffer( "連接埠﹕%5d 多重連線﹕%d 玩家序號 %d "
                      "連線位址﹕%s\n\r"
        , pDesc->port
        , pDesc->multi
        , pDesc->id_number
        , host_name( pDesc ) );
    }

    else
    {
      send_to_buffer( "斷線中﹐沒有描述表。\n\r" );
    }
  }

  send_to_buffer( "正在對打的對象﹕%s\n\r"
    , victim->fighting ? victim->fighting->name : "(沒有)" );

  if ( !IS_NPC(victim) )
  {
    send_to_buffer( "口渴程度﹕%d%% "
                    "肚子餓程度﹕%d%% "
                    "酒醉程度﹕%d%% "
                    "Saving throw﹕%d "
                    "被關次數﹕%d\n\r",
        victim->pcdata->condition[COND_THIRST],
        victim->pcdata->condition[COND_FULL],
        victim->pcdata->condition[COND_DRUNK],
        victim->saving_throw,
        victim->pcdata->imprison );
  }

  send_to_buffer( "正在攜帶物品的總件數﹕%d。 "
                  "正在攜帶物品的總重﹕%d。\n\r"
                  "年齡﹕%d  "
                  "上線時數﹕%d  "
                  "計時器﹕%d\n\r"
                  "神族代理權﹕%s ( %d ) 懲罰時間﹕%d\n\r"
                  "旗標﹕%s ( %s )\n\r"
                  "致能﹕%s ( %d )\n\r"
                  "主人﹕%s. "
                  "領導者﹕%s。\n\r"
                  "買東西﹕%s。 "
                  "賣東西﹕%s。\n\r" ,

      victim->carry_number,
      get_carry_weight( victim ),
      get_age( victim ),
      (int) victim->played,
      victim->timer ,
      IS_NPC( victim ) ? "非玩家" : wiz_bit_name( victim->wizflags ),
      victim->wizflags,
      victim->tribunal_timer,
      act_bit_name( victim->act ),
      binary_bit( victim->act ),
      IS_NPC( victim) ? "從缺" : turn_bit_name( victim->turn ) ,
      IS_NPC( victim) ? -1     : victim->turn,
      victim->master      ? victim->master->name   : "(沒有)",
      victim->leader      ? victim->leader->name   : "(沒有)",
      YESNO( auction_info->buyer == victim ),
      YESNO( auction_info->seller == victim ) );

  send_to_buffer( "簡短的描述﹕%s。\n\r"
                  "詳細的描述﹕%s",
      victim->byname,
      victim->long_descr[0] != '\x0' ? victim->long_descr : "(沒有)。\n\r" );

  if ( IS_NPC( victim ) && victim->spec_fun )
    send_to_buffer( "這隻怪物有特殊的函數。\n\r" );

  if ( IS_NPC( victim ) && victim->pIndexData )
    send_to_buffer(
      "怪物數目 %d 經驗值加成 %d%% 遷移機率 %d%% 講話 %s\n\r"
      "攻擊加成﹕百分之 %d 閃躲加成﹕百分之 %d\n\r"
      , get_mob_count( victim->pIndexData->vnum )
      , victim->pIndexData->multipile
      , victim->pIndexData->migrate
      , YESNO( victim->pIndexData->speak )
      , victim->pIndexData->attack_ratio
      , victim->pIndexData->dodge_ratio );

  /* 輸出土遁地方 */
  if ( victim->pcdata )
  {
    send_to_buffer( "土遁地方﹕" );

    for ( loop = 0; loop < MAX_RECALL; loop++ )
      if ( victim->pcdata->recall[loop] > 0 )
        send_to_buffer( " %d", victim->pcdata->recall[loop] );
  }

  send_to_buffer( "\n\r" );

  /* 輸出解迷的部份 */
  if ( !IS_NPC( victim ) )
  {
    send_to_buffer( "解迷﹕" );
    for ( pQuest = victim->quest; pQuest; pQuest = pQuest->next )
    {
      if ( !pQuest->link ) continue;
      send_to_buffer( " %s", pQuest->link->mark );
    }

    send_to_buffer( "\n\r" );
  }

  if ( victim->enroll )
  {
    send_to_buffer( "記仇對象﹕" );

    for ( pEnroll = victim->enroll; pEnroll; pEnroll = pEnroll->next )
      send_to_buffer( " %s", pEnroll->name );

    send_to_buffer( "\n\r" );
  }

  if ( IS_NPC( victim ) && victim->pIndexData )
  {
    /* 輸出緩衝區 */
    if ( victim->talk_buf )
    {
      send_to_buffer( "有輸出緩衝區 模式﹕" );

      switch( victim->talk_mode )
      {
      default:
      case MODE_ABSENT: send_to_buffer( "模式不詳" ); break;
      case MODE_SAY:    send_to_buffer( "同格說話" ); break;
      case MODE_CHAT:   send_to_buffer( "閒聊模式" ); break;
      }

      send_to_buffer( "\n\r" );
    }

    if ( victim->spiritjade > 0 )
      send_to_buffer( "鬼玉號碼: %d\n\r", victim->spiritjade );

    /* 教導 */
    if ( victim->pIndexData->teach )
    {
      TEACH_DATA * teach;
      SKILL_DATA * pSkill;
      int          count = 0;

      send_to_buffer( "教導的技能﹕\n\r" );
      for ( teach = victim->pIndexData->teach; teach ; teach = teach->next )
      {
        if ( ( pSkill = get_skill( teach->slot ) ) )
        {
          send_to_buffer( "( %2d ) 編號﹕%d 法術名稱﹕%s "
                          "熟練度﹕%d 教授金﹕%d 隱藏﹕%d%%\n\r"
            , ++count
            , pSkill->slot
            , pSkill->name
            , teach->adept
            , teach->cost
            , teach->inventory );
        }
      }
    }

    /* 重生 */
    if ( victim->pIndexData->reborn_vnum > 0 )
    {
      MOB_INDEX_DATA * pMobIndex;

      if ( ( pMobIndex = get_mob_index( victim->pIndexData->reborn_vnum ) ) )
      {
        send_to_buffer( "重生怪物號碼﹕%d 名稱 %s\n\r"
          , pMobIndex->vnum
          , mob_index_name( NULL, pMobIndex ) );
      }
    }

    /* 座騎 */
    if ( victim->tractable > 0 )
      send_to_buffer( "馴化程度﹕%d%%\n\r", victim->tractable );
  }

  send_to_buffer( "座騎﹕%s 被騎﹕%s\n\r"
    , victim->mount    ? mob_name( NULL, victim->mount    ) : "無"
    , victim->mount_by ? mob_name( NULL, victim->mount_by ) : "無" );

  for ( paf = victim->affected; paf; paf = paf->next )
  {
    if ( !( pSkill = get_skill( paf->type ) ) )
    {
      mudlog( LOG_DEBUG, "do_mstat: 錯誤的型態 %d.", paf->type );
      continue;
    }

    send_to_buffer( "法術﹕%s對%s造成 %d 影響還有 %d 小時\n\r",
      pSkill->cname,
      affect_loc_name( paf->location ),
      paf->modifier,
      paf->duration );
  }

  if ( victim->pIndexData )
  {
    for ( paf = victim->pIndexData->affected; paf; paf = paf->next )
    {
      if ( !( pSkill = get_skill( paf->type ) ) )
      {
        mudlog( LOG_DEBUG, "do_mstat: 錯誤的型態 %d.", paf->type );
        continue;
      }

      send_to_buffer( "法術﹕%s對%s造成 %d 影響還有 %d 小時\n\r",
        pSkill->cname,
        affect_loc_name( paf->location ),
        paf->modifier,
        paf->duration );
    }
  }

  print_buffer( ch );
  RETURN_NULL();
}

FUNCTION( do_mfind )
{
  extern int       top_mob_index;
  char             arg1[MAX_INPUT_LENGTH];
  char             arg2[MAX_INPUT_LENGTH];
  MOB_INDEX_DATA * pMobIndex;
  int              vnum;
  int              nMatch;
  bool             fAll;
  bool             found;
  int              up;
  int              down;
  int              number;

  PUSH_FUNCTION( "do_mfind" );

  argument = one_argument( argument, arg1 );
  one_argument( argument , arg2 );

  clear_buffer();

  fAll   = !str_cmp( arg1, "all" );
  found  = FALSE;
  nMatch = 0;

  if ( is_number( arg1 ) && is_number( arg2 ) )
  {
    up   = UMAX( atoi( arg1 ) , atoi( arg2 ) );
    down = UMIN( atoi( arg1 ) , atoi( arg2 ) );

    for ( vnum = 0; nMatch < top_mob_index; vnum++ )
    {
      if ( ( pMobIndex = get_mob_index( vnum ) ) )
      {
        nMatch++;
        if ( pMobIndex->vnum >= down && pMobIndex->vnum <= up )
        {
          found = TRUE;
          send_to_buffer( "\e[1;32m號碼\e[0m%6d "
                          "\e[1;33m流水號\e[0m%5d "
                          "\e[1;35m描述\e[0m %s\n\r",
            pMobIndex->vnum,
            nMatch,
            mob_index_name( NULL, pMobIndex ) );

          if ( buffer_full() ) break;
        }
      }
    }
  }

  else
  {
    number = atoi( arg1 );
    for ( vnum = 0; nMatch < top_mob_index; vnum++ )
    {
      if ( ( pMobIndex = get_mob_index( vnum ) ) )
      {
        nMatch++;
        if ( fAll || is_name( arg1, pMobIndex->name )
          || number == pMobIndex->vnum )
        {
          found = TRUE;
          send_to_buffer( "\e[1;32m號碼\e[0m%6d "
                          "\e[1;33m流水號\e[0m%5d "
                          "\e[1;35m描述\e[0m %s\n\r",
                          pMobIndex->vnum,
                          nMatch,
                          mob_index_name( NULL, pMobIndex ) );

          if ( buffer_full() ) break;
        }
      }
    }
  }

  if ( !found )
  {
    send_to_char( "找不到你指定的對象。\n\r", ch );
    RETURN_NULL();
  }

  print_buffer( ch );
  RETURN_NULL();
}

FUNCTION( do_ofind )
{
  extern int       top_obj_index;
  char             arg1[MAX_INPUT_LENGTH];
  char             arg2[MAX_INPUT_LENGTH];
  OBJ_INDEX_DATA * pObjIndex;
  int              vnum;
  int              nMatch;
  int              number;
  bool             fAll;
  bool             found;
  int              up;
  int              down;

  PUSH_FUNCTION( "do_ofind" );

  argument = one_argument( argument, arg1 );
  one_argument( argument , arg2 );

  fAll   = !str_cmp( arg1, "all" );
  found  = FALSE;
  nMatch = 0;

  clear_buffer();

  if ( is_number( arg1 ) && is_number( arg2 ) )
  {
    up   = UMAX( atoi( arg1 ) , atoi( arg2 ) );
    down = UMIN( atoi( arg1 ) , atoi( arg2 ) );
    for ( vnum = 0; nMatch < top_obj_index; vnum++ )
    {
      if ( ( pObjIndex = get_obj_index( vnum ) ) )
      {
        nMatch++;
        if ( pObjIndex->vnum >= down && pObjIndex->vnum <= up )
        {
          found = TRUE;
          send_to_buffer( "\e[1;32m號碼\e[0m%6d "
                          "\e[1;33m流水號\e[0m%5d "
                          "\e[1;36m等級\e[0m%4d "
                          "\e[1;35m描述\e[0m %s(%s)\n\r",
                          pObjIndex->vnum,
                          nMatch,
                          pObjIndex->level,
                          pObjIndex->short_descr,
                          pObjIndex->name );

          if ( buffer_full() ) break;
        }
      }
    }
  }
  else
  {
    number = atoi( arg1 );
    for ( vnum = 0; nMatch < top_obj_index; vnum++ )
    {
      if ( ( pObjIndex = get_obj_index( vnum ) ) )
      {
        nMatch++;
        if ( fAll || is_name( arg1, pObjIndex->name )
          || number == pObjIndex->vnum )
        {
          found = TRUE;
          send_to_buffer( "\e[1;32m號碼\e[0m%6d "
                          "\e[1;33m流水號\e[0m%5d "
                          "\e[1;36m等級\e[0m%4d "
                          "\e[1;34m數量\e[0m%3d "
                          "\e[1;35m描述\e[0m %s(%s)\n\r",
                          pObjIndex->vnum,
                          nMatch,
                          pObjIndex->level,
                          pObjIndex->count,
                          pObjIndex->short_descr,
                          pObjIndex->name );

          if ( buffer_full() ) break;
        }
      }
    }
  }

  if ( !found ) send_to_buffer( "找不到你所指定的東西。\n\r" );
  print_buffer( ch );

  RETURN_NULL();
}

FUNCTION( do_mwhere )
{
  char        arg[MAX_INPUT_LENGTH];
  CHAR_DATA * victim;
  int         counter;
  bool        fAll = FALSE;

  PUSH_FUNCTION( "do_mwhere" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  one_argument( argument, arg );

  if ( !str_cmp( arg , "all" ) ) fAll = TRUE;

  clear_buffer();
  counter = 1;

  for ( victim = char_list; victim; victim = victim->next )
  {
    if ( !verify_char( victim ) ) continue;

    if ( ( is_name( arg , victim->name ) || fAll )
       &&  IS_NPC( victim ) && victim->in_room )
    {
      send_to_buffer( "[ %3d ] %s [ %d ] "
                      "在 %s [ %d ]。\n\r"
                      , counter++
                      , mob_name( ch, victim )
                      , victim->pIndexData->vnum
                      , victim->in_room->name
                      , victim->in_room->vnum );

      if ( buffer_full() ) break;
    }
  }

  if ( counter == 1 ) send_to_buffer( "並沒有找到你要找的怪物。\n\r" );
  print_buffer( ch );
  RETURN_NULL();
}

/* 重新開機 */
FUNCTION( do_reboot )
{
  char        arg[ MAX_INPUT_LENGTH];
  char        buf[ MAX_STRING_LENGTH];
  int         minutes;
  time_t      future;
  extern bool merc_down;

  PUSH_FUNCTION( "do_reboot" );

  argument = one_argument( argument, arg );

  if ( !arg[0] || !str_cmp( arg, "now" ) )
  {
    if ( ch && !IS_NPC( ch ) && !IS_SET( ch->act, PLR_WIZINVIS ) )
    {
      sprintf( buf, "由%s為\e[1;32m%s\e[0m"
        "重新開機﹐請再等待幾秒鐘吧﹗", mob_name( NULL, ch ), mud_name );
    }

    else
    {
     str_cpy( buf,  "系統重新啟動﹐請稍後再光臨﹗" );
    }

    talk_channel_2( buf, CHANNEL_NOTICE, "" );
    mudlog( LOG_INFO , " " );
    close_all_socket();
    mudlog( LOG_INFO , " " );
    merc_down = TRUE;
  }

  else if ( !str_cmp( arg, "cancel" ) )
  {
    if ( reboot_time > 0 )
      talk_channel_2( "取消系統重置動作。" , CHANNEL_NOTICE, "" );

    else if ( shutdown_time > 0 )
      talk_channel_2( "取消系統關閉動作。" , CHANNEL_NOTICE, "" );

    else if ( ch )
    {
      send_to_char( "系統沒有任何重置或關閉動作﹐指令無效。\n\r", ch );
      RETURN_NULL();
    }

    str_cpy( buf,
       "\e[1m天降神兵﹐將所有的鬼怪消滅﹐大地又恢復和平。\n\r\e[0m" );

    send_to_all_char( buf );
    first_warn  = second_warn = third_warn = TRUE;
    reboot_time = shutdown_time = ( time_t ) 0;
    wizlock     = FALSE;
  }

  else if ( !str_cmp( arg, "at" ) )
  {
    one_argument( argument, arg );

    if ( ( future = time_at( arg ) ) <= current_time )
    {
      if ( ch ) send_to_char( "時間設定錯誤﹐請查詢使用方法。\n\r", ch );
      RETURN_NULL();
    }

    minutes       = ( ( reboot_time = future ) - current_time ) / 60;
    shutdown_time = ( time_t ) 0;

    first_warn    = minutes >= FIRST_WARN  ? TRUE : FALSE;
    second_warn   = minutes >= SECOND_WARN ? TRUE : FALSE;
    third_warn    = minutes >= THIRD_WARN  ? TRUE : FALSE;

    if ( ch && !IS_NPC( ch ) && !IS_SET( ch->act, PLR_WIZINVIS ) )
    {
      sprintf( buf, "\e[1m遠方傳來了%s\e[1m陰惻的奸笑﹐在這個權力倚重的"
        "時代﹐重新開始是最佳途徑。\n\r唯有破壞才有建設﹐讓我們"
        "盡情的享受這個破壞的感覺吧﹗\n\r\n\r\e[0m"
        , mob_name( NULL, ch ) );
    }
    else
    {
      sprintf( buf, "\e[1m遠方傳來了一陣陰惻的奸笑﹐在這個權力倚重的"
        "時代﹐重新開始是最佳途徑。\n\r唯有破壞才有建設﹐讓我們"
        "盡情的享受這個破壞的感覺吧﹗\n\r\n\r\e[0m" );
    }

    send_to_all_char( buf );
    sprintf( buf, "系統將在 %s 重置。\n\r"
      , time_format( future, "%r, %a-%d-%b-%y" ) );

    send_to_all_char( buf );
  }

  else if ( is_number( arg ) )
  {
    if ( ( minutes = atoi( arg ) ) <= 0 || minutes > 60 )
    {
      if ( ch )
        send_to_char( "系統重置時間不合法( 1 到 60 分鐘 )。\n\r", ch );

      RETURN_NULL();
    }

    if ( ch && !IS_NPC( ch ) && !IS_SET( ch->act, PLR_WIZINVIS ) )
    {
      sprintf( buf, "\e[1m遠方傳來了%s\e[1m陰惻的奸笑﹐在這個權力倚重的"
        "時代﹐重新開始是最佳途徑。\n\r唯有破壞才有建設﹐讓我們"
        "盡情的享受這個破壞的感覺吧﹗\n\r\n\r\e[0m"
        , mob_name( NULL, ch ) );
    }
    else
    {
      sprintf( buf, "\e[1m遠方傳來了一陣陰惻的奸笑﹐在這個權力倚重的"
        "時代﹐重新開始是最佳途徑。\n\r唯有破壞才有建設﹐讓我們"
        "盡情的享受這個破壞的感覺吧﹗\n\r\n\r\e[0m" );
    }

    send_to_all_char( buf );
    sprintf( buf, "系統將在 %d分鐘後重置﹗\n\r", minutes );
    send_to_all_char( buf );

    reboot_time   = current_time + minutes * 60;
    shutdown_time = ( time_t ) 0;
    wizlock       = TRUE;
    first_warn    = minutes > FIRST_WARN  ? TRUE : FALSE;
    second_warn   = minutes > SECOND_WARN ? TRUE : FALSE;
    third_warn    = minutes > THIRD_WARN  ? TRUE : FALSE;
  }

  else
  {
    if ( ch ) send_to_char( "語法錯誤﹐請查詢使用方法。\n\r", ch );
  }

  RETURN_NULL();
}

/* 關閉 MUD */
FUNCTION( do_shutdown )
{
  char        buf[MAX_STRING_LENGTH];
  char        arg[MAX_INPUT_LENGTH];
  time_t      future;
  int         minutes;
  extern bool merc_down;

  PUSH_FUNCTION( "do_shutdown" );

  argument = one_argument( argument, arg );

  if ( !arg[0] || !str_cmp( arg, "now" ) )
  {
    if ( ch )
      sprintf( buf, "由%s關閉\e[1;32m%s\e[0m﹗", mob_name( NULL, ch ), mud_name );

    else
      str_cpy( buf, "系統關閉﹐請下次再光臨﹐謝謝。" );

    mudlog( LOG_SHUTDOWN , "%s", buf );
    talk_channel_2( buf, CHANNEL_NOTICE, "" );

    mudlog( LOG_INFO , " " );
    close_all_socket();
    mudlog( LOG_INFO , " " );

    merc_down = TRUE;
  }

  else if ( !str_cmp( arg, "cancel" ) )
  {
    if ( reboot_time   > 0 )
      talk_channel_2( "取消系統重置動作。", CHANNEL_NOTICE, "" );

    else if ( shutdown_time > 0 )
      talk_channel_2( "取消系統關閉動作。", CHANNEL_NOTICE, "" );

    else if ( ch )
    {
      send_to_char( "系統沒有任何重置或關閉動作﹐指令無效。\n\r", ch );
      RETURN_NULL();
    }

    str_cpy( buf,
      "\e[1m天降神兵﹐將所有的鬼怪消滅﹐大地又恢復和平﹗\n\r\e[0m" );

    send_to_all_char( buf );
    first_warn  = second_warn = third_warn = TRUE;
    reboot_time = shutdown_time = ( time_t ) 0;
    wizlock     = FALSE;
  }

  else if ( !str_cmp( arg, "at" ) )
  {
    one_argument( argument, arg );

    if ( ( future = time_at( arg ) ) <= current_time )
    {
      if ( ch ) send_to_char( "時間設定錯誤﹐請查詢使用方法。\n\r", ch );
      RETURN_NULL();
    }

    minutes     = ( ( shutdown_time = future ) - current_time ) / 60;
    reboot_time = ( time_t ) 0;

    first_warn  = minutes >= FIRST_WARN  ? TRUE : FALSE;
    second_warn = minutes >= SECOND_WARN ? TRUE : FALSE;
    third_warn  = minutes >= THIRD_WARN  ? TRUE : FALSE;

    if ( ch && !IS_NPC( ch ) && !IS_SET( ch->act, PLR_WIZINVIS ) )
    {
      sprintf( buf, "\e[1m遠方傳來了%s\[1m陰惻的奸笑﹐在這個權力倚重的"
        "時代﹐重新開始是最佳途徑。\n\r唯有破壞才有建設﹐讓我們"
        "盡情的享受這個破壞的感覺吧﹗\n\r\n\r\e[0m"
        , mob_name( NULL, ch ) );
    }
    else
    {
      sprintf( buf, "\e[1m遠方傳來了一陣陰惻的奸笑﹐在這個權力倚重的"
        "時代﹐重新開始是最佳途徑。\n\r唯有破壞才有建設﹐讓我們"
        "盡情的享受這個破壞的感覺吧﹗\n\r\n\r\e[0m" );
    }

    send_to_all_char( buf );

    sprintf( buf, "系統將在 %s 關閉﹗\n\r"
      , time_format( future, "%r, %a-%d-%b-%y" ) );

    send_to_all_char( buf );
  }

  else if ( is_number( arg ) )
  {
    if ( ( minutes = atoi( arg ) ) <= 0 || minutes > 60 )
    {
      if ( ch )
        send_to_char( "系統關閉時間不合法( 1 到 60 分鐘 )。\n\r", ch );

      RETURN_NULL();
    }

    if ( ch && !IS_NPC( ch ) && !IS_SET( ch->act, PLR_WIZINVIS ) )
    {
      sprintf( buf, "\e[1m遠方傳來了%s陰惻的奸笑﹐在這個權力倚重的"
        "時代﹐重新開始是最佳途徑。\n\r唯有破壞才有建設﹐讓我們"
        "盡情的享受這個破壞的感覺吧﹗\n\r\n\r\e[0m"
        , mob_name( NULL, ch ) );
    }
    else
    {
      sprintf( buf, "\e[1m遠方傳來了一陣陰惻的奸笑﹐在這個權力倚重的"
        "時代﹐重新開始是最佳途徑。\n\r唯有破壞才有建設﹐讓我們"
        "盡情的享受這個破壞的感覺吧﹗\n\r\n\r\e[0m" );
    }

    send_to_all_char( buf );
    sprintf( buf, "系統將在 %d分鐘後關閉。\n\r", minutes );
    send_to_all_char( buf );

    shutdown_time = current_time + minutes * 60;
    reboot_time   = ( time_t ) 0;
    wizlock       = TRUE;
    first_warn    = minutes > FIRST_WARN  ? TRUE : FALSE;
    second_warn   = minutes > SECOND_WARN ? TRUE : FALSE;
    third_warn    = minutes > THIRD_WARN  ? TRUE : FALSE;
  }

  else
  {
    if ( ch ) send_to_char( "語法錯誤﹐請查詢使用方法。\n\r", ch );
  }

  RETURN_NULL();
}

/* 監視玩家的動作 */
FUNCTION( do_snoop )
{
  char              arg[MAX_INPUT_LENGTH];
  DESCRIPTOR_DATA * d;
  CHAR_DATA       * victim;
  int               count;

  PUSH_FUNCTION( "do_snoop" );

  one_argument( argument, arg );

  if ( !str_prefix( arg, "!list" ) )
  {
    clear_buffer();
    for ( count = 0, d = descriptor_list; d; d = d->next )
    {
      if ( verify_desc( d )
        && d->snoop_by == ch->desc
        && ( victim = d->character ) )
      {
        if ( count == 0 )
          send_to_buffer( "\e[1;33;44m順序 玩 家 名 稱  等級 "
            "中文名稱                                             \e[0m\n\r" );

        send_to_buffer( "%3d. %-12s %4d %-s\n\r"
          , ++count, victim->name, victim->level, victim->cname );

        if ( buffer_full() ) break;
      }
    }

    if ( count == 0 ) send_to_buffer( "你沒有監視任何人﹗\n\r", ch );
    print_buffer( ch );

    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "!clear" ) )
  {
    for ( count = 0, d = descriptor_list; d; d = d->next )
    {
      if ( verify_desc( d ) && d->snoop_by == ch->desc )
      {
        count++;
        d->snoop_by = NULL;
      }
    }

    if ( count == 0 ) send_to_char( "你原本就沒有監視誰﹗\n\r", ch );
    else send_to_char( "清除你所有監視人的輸出。\n\r", ch );

    RETURN_NULL();
  }

  if ( !( victim = get_char_world( ch, arg ) ) )
  {
    send_to_char( "找不到你想監視的人。\n\r", ch );
    RETURN_NULL();
  }

  if ( !victim->desc )
  {
    act( "$N沒有描述表﹐所以不能監視。", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( victim == ch )
  {
    send_to_char( "取消所有監視動作。\n\r", ch );

    for ( d = descriptor_list; d; d = d->next )
      if ( verify_desc( d ) && d->snoop_by == ch->desc ) d->snoop_by = NULL;

    RETURN_NULL();
  }

  if ( victim->desc->snoop_by )
  {
    act( "$N已經被其它的管理者監視了。", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( get_trust( victim ) >= get_trust( ch ) )
  {
    act( "你的等級沒有$N的高﹐所以失敗了。", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( ch->desc )
  {
    for ( d = ch->desc->snoop_by; d; d = d->snoop_by )
    {
      if ( d->character == victim )
      {
        send_to_char( "沒有任何監視行程。\n\r", ch );
        RETURN_NULL();
      }
    }
  }

  victim->desc->snoop_by = ch->desc;
  act( "監視設定﹐取消請輸入 $2snoop $t$0。",ch, ch->name, NULL, TO_CHAR );
  RETURN_NULL();
}

FUNCTION( do_mload )
{
  char             arg[MAX_INPUT_LENGTH];
  MOB_INDEX_DATA * pMobIndex;
  CHAR_DATA      * victim;

  PUSH_FUNCTION( "do_mload" );

  one_argument( argument, arg );

  if ( !arg[0] || !is_number(arg) )
  {
    send_to_char( "語法﹕ mload <號碼>。\n\r", ch );
    RETURN_NULL();
  }

  if ( !( pMobIndex = get_mob_index( atoi( arg ) ) ) )
  {
    send_to_char( "號碼錯誤﹐沒有一支怪物有那個號碼。\n\r", ch );
    RETURN_NULL();
  }

  if ( ( victim = shape_mobile( pMobIndex, ch->in_room ) ) )
  {
    smash_point( victim->name        );
    smash_point( victim->byname      );
    smash_point( victim->long_descr  );
    smash_point( victim->description );

    act( "$n創造出一隻$N﹗", ch, NULL, victim, TO_ROOM );
    act( "好了﹐你創造出一隻$N。", ch, NULL, victim, TO_CHAR );
  }
  else
  {
    send_to_char( "複製怪物失敗。\n\r" , ch );
  }
  RETURN_NULL();
}

FUNCTION( do_oload )
{
  char             arg1[MAX_INPUT_LENGTH];
  char             arg2[MAX_INPUT_LENGTH];
  OBJ_INDEX_DATA * pObjIndex;
  OBJ_DATA       * obj;
  int              level;

  PUSH_FUNCTION( "do_oload" );

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );

  if ( !arg1[0] || !is_number( arg1 ) )
  {
    send_to_char( "語法﹕oload <號碼> <等級>。\n\r", ch );
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
      send_to_char( "語法﹕oload <號碼> <等級>。\n\r", ch );
      RETURN_NULL();
    }

    if ( ( level = atoi( arg2 ) ) < 0 || level > get_trust( ch ) )
    {
      send_to_char( "你失敗了﹐因為創造出來的東西比你等級還高。\n\r", ch );
      RETURN_NULL();
    }
  }

  if ( !( pObjIndex = get_obj_index( atoi( arg1 ) ) ) )
  {
    send_to_char( "號碼錯誤﹐沒有一樣物品的號碼是你想要的號碼。\n\r", ch );
    RETURN_NULL();
  }

  if ( pObjIndex == ObjProtype )
  {
    send_to_char( "對不起﹐這個物品是不能創造的﹗\n\r", ch );
    RETURN_NULL();
  }

  obj = create_object( pObjIndex, level );
  smash_point( obj->name        );
  smash_point( obj->cname       );
  smash_point( obj->description );

  if ( obj->Takeable
    && obj->item_type != ITEM_CORPSE_NPC
    && obj->item_type != ITEM_CORPSE_PC )
  {
    obj_to_char( obj, ch );
  }
  else
  {
    obj_to_room( obj, ch->in_room );
    act( "$n創造出一件$p﹗", ch, obj, NULL, TO_ROOM );
  }

  act( "你創造出一$T$p。", ch, obj, obj->unit, TO_CHAR );
  RETURN_NULL();
}

FUNCTION( do_purge )
{
  char        arg[MAX_INPUT_LENGTH];
  CHAR_DATA * victim;
  OBJ_DATA  * obj;

  PUSH_FUNCTION( "do_purge" );

  one_argument( argument, arg );

  if ( !arg[0] )
  {
    CHAR_DATA * vnext;
    OBJ_DATA  * obj_next;

    for ( victim = ch->in_room->people; victim; victim = vnext )
    {
      vnext = victim->next_in_room;
      if ( IS_NPC( victim ) && victim != ch ) extract_char( victim, TRUE );
    }

    for ( obj = ch->in_room->contents; obj; obj = obj_next )
    {
      obj_next = obj->next_content;
      if ( obj->item_type != ITEM_CORPSE_PC ) extract_obj( obj );
    }

    act( "$n清除這間房間﹗", ch, NULL, NULL, TO_ROOM);
    send_to_char( "好了﹐清除本房間完畢。\n\r", ch );
    RETURN_NULL();
  }

  if ( !( victim = get_char_world( ch, arg ) ) )
  {
    send_to_char( "找不到你要清除的對象。\n\r", ch );
    RETURN_NULL();
  }

  if ( !IS_NPC( victim ) )
  {
    act( "不能對玩家$N下這個指令。", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  act( "$n清除了$N﹗", ch, NULL, victim, TO_NOTVICT );
  extract_char( victim, TRUE );
  RETURN_NULL();
}

FUNCTION( do_habilitate )
{
  char        arg1[MAX_INPUT_LENGTH];
  char        arg2[MAX_INPUT_LENGTH];
  CHAR_DATA * victim;
  CHAR_DATA * vch;
  int         number;

  PUSH_FUNCTION( "do_habilitate" );

  argument = one_argument( argument, arg1 );
             one_argument( argument, arg2 );

  if ( !arg1[0] )
  {
    send_to_char( "語法﹕habilitate <對象> <代理能力>。\n\r", ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg1, "!list" ) )
  {
    clear_buffer();

    send_to_buffer( "目前%s的神族代理玩家如下﹕\n\r", mud_name );
    for ( number = 0, vch = char_list; vch; vch = vch->next )
    {
      if ( !verify_char( vch )
        || IS_NPC( vch )
        || vch->wizflags <= 0 ) continue;

      send_to_buffer( "%2d. %-23s ( %-12s ) ﹕%s\n\r"
        , number++
        , vch->cname
        , vch->name
        , wiz_bit_name( vch->wizflags ) );

      if ( buffer_full() ) break;
    }

    send_to_buffer( "\n\r" );
    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !arg2[0] )
  {
    send_to_char( "語法﹕habilitate <玩家> <能力>。\n\r", ch );
    RETURN_NULL();
  }

  if ( !( victim = get_char_room( ch, arg1 ) ) )
  {
    send_to_char( "找不到你要給予能力的對象。\n\r", ch);
    RETURN_NULL();
  }

  if ( IS_NPC( victim ) )
  {
    act( "不能給予怪物$N這種能力。", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( !IS_HERO( victim ) || IS_IMMORTAL( victim ) )
  {
    act( "$N的資歷不足或過高﹐請不要賦予他能力﹗", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  switch( arg2[0] )
  {
  default:
    send_to_char( "沒有這種神族代理能力﹗\n\r", ch );
    RETURN_NULL();

  case '!':
    if ( !str_prefix( arg2 + 1, "delete" ) )
    {
      victim->wizflags = 0;
      send_to_char( "移除神族代理能力完成﹗\n\r", ch );
      act( "你的神族代理能力都被$N移除了﹗", victim, NULL, ch, TO_CHAR );

      mudlog( LOG_WIZFLAGS, "%s 移除 %s 全部代理能力。"
        , ch->name, victim->name );
    }

    break;

  case 'n':
    if ( !str_prefix( arg2, "notell" ) )
    {
      if ( IS_SET( victim->wizflags, WIZ_NOTELL ) )
      {
        REMOVE_BIT( victim->wizflags, WIZ_NOTELL );

        act( "移除$N神族代理能力$t完成﹗"
          , ch,  wiz_bit_name( WIZ_NOTELL ), victim, TO_CHAR );

        act( "你的神族代理能力$t被$N移除了﹗"
          , victim,  wiz_bit_name( WIZ_NOTELL ), ch, TO_CHAR );

        mudlog( LOG_WIZFLAGS, "%s 移除 %s 的 %s 代理能力。"
          , ch->name, victim->name, wiz_bit_name( WIZ_NOTELL ) );
      }

      else
      {
        SET_BIT( victim->wizflags, WIZ_NOTELL );

        act( "設定$N神族代理能力$t完成﹗"
          , ch,  wiz_bit_name( WIZ_NOTELL ), victim, TO_CHAR );

        act( "你被$N賦予$t的神族代理能力﹗"
          , victim,  wiz_bit_name( WIZ_NOTELL ), ch, TO_CHAR );

        send_to_char( "請用 help tribunal 來察看相關事項。\n\r", victim );

        mudlog( LOG_WIZFLAGS, "%s 設定 %s 的 %s 代理能力。"
          , ch->name, victim->name, wiz_bit_name( WIZ_NOTELL ) );
      }
    }
    break;

  case 's':
    if ( !str_prefix( arg2, "silence" ) )
    {
      if ( IS_SET( victim->wizflags, WIZ_SILENCE ) )
      {
        REMOVE_BIT( victim->wizflags, WIZ_SILENCE );

        act( "移除$N神族代理能力$t完成﹗"
          , ch, wiz_bit_name( WIZ_SILENCE ), victim, TO_CHAR );

        act( "你的神族代理能力$t被$N移除了﹗"
          , victim, wiz_bit_name( WIZ_SILENCE ), ch, TO_CHAR );

        mudlog( LOG_WIZFLAGS, "%s 移除 %s 的 %s 代理能力。"
          , ch->name, victim->name, wiz_bit_name( WIZ_SILENCE ) );
      }

      else
      {
        SET_BIT( victim->wizflags, WIZ_SILENCE );

        act( "設定$N神族代理能力$t完成﹗"
          , ch, wiz_bit_name( WIZ_SILENCE ), victim, TO_CHAR );

        act( "你被$N賦予$t的神族代理能力﹗"
          , victim, wiz_bit_name( WIZ_SILENCE ), ch, TO_CHAR );

        send_to_char( "請用 help tribunal 來察看相關事項。\n\r", victim );
        mudlog( LOG_WIZFLAGS, "%s 設定 %s 的 %s 代理能力。"
          , ch->name, victim->name, wiz_bit_name( WIZ_SILENCE ) );
      }
    }
    break;

  }

  RETURN_NULL();
}

FUNCTION( do_advance )
{
  char        arg1[MAX_INPUT_LENGTH];
  char        arg2[MAX_INPUT_LENGTH];
  char        buf[MAX_STRING_LENGTH];
  CHAR_DATA * victim;
  int         level;
  int         iLevel;

  PUSH_FUNCTION( "do_advance" );

  argument = one_argument( argument, arg1 );
             one_argument( argument, arg2 );

  if ( !arg1[0] || !arg2[0] || !is_number( arg2 ) )
  {
    send_to_char( "語法﹕advance 「對象」 「等級」\n\r", ch );
    RETURN_NULL();
  }

  if ( !( victim = get_char_room( ch, arg1 ) ) )
  {
    send_to_char( "找不到你要升級的對象。\n\r", ch);
    RETURN_NULL();
  }

  if ( IS_NPC( victim ) )
  {
    act( "不能用在非玩家$N身上。", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( ( level = atoi( arg2 ) ) < 1 || level > LEVEL_HERO )
  {
    chinese_number( LEVEL_HERO, buf );
    act( "最高只能升到$t級。", ch, buf, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( level > get_trust( ch ) )
  {
    send_to_char( "提昇的等級比你自己的等級還高﹐所以失敗。\n\r", ch );
    RETURN_NULL();
  }

  if ( level <= victim->level )
  {
    send_to_char( "降低玩家的等級。\n\r", ch );
    send_to_char( "你是不是幹了什麼壞事﹐不然怎麼被降等級。\n\r", victim );

    victim->level    = 1;
    victim->exp      = 0;
    victim->max_hit  = 10;
    victim->max_mana = 100;
    victim->max_move = 100;
    victim->practice = 0;
    victim->hit      = get_curr_hit( victim );
    victim->mana     = get_curr_mana( victim );
    victim->move     = get_curr_move( victim );
    advance_level( victim );
  }

  else
  {
    act( "提昇玩家$N的等級。", ch, NULL, victim, TO_CHAR );
    send_to_char(
      "喔喔﹐你是什麼大人物﹖不然創世紀神怎麼幫你提昇等級﹖\n\r", victim );
  }

  for ( iLevel = victim->level ; iLevel < level; iLevel++ )
  {
    send_to_char( "\e[1;32m你升級了﹗\e[0m", victim );
    victim->level++;
    advance_level( victim );
  }

  victim->exp   = 0;
  victim->trust = 0;
  RETURN_NULL();
}

FUNCTION( do_trust )
{
  char        arg1[MAX_INPUT_LENGTH];
  char        arg2[MAX_INPUT_LENGTH];
  char        buf[MAX_STRING_LENGTH];
  int         level;
  CHAR_DATA * victim;

  PUSH_FUNCTION( "do_trust" );

  argument = one_argument( argument, arg1 );
             one_argument( argument, arg2 );

  if ( !arg1[0] || !arg2[0] || !is_number( arg2 ) )
  {
    send_to_char( "語法﹕trust <對象> <等級>。\n\r", ch );
    RETURN_NULL();
  }

  if ( !( victim = get_char_room( ch, arg1 ) ) )
  {
    send_to_char( "你要提昇信賴度的玩家不在這裡。\n\r", ch );
    RETURN_NULL();
  }

  if ( IS_NPC( victim ) )
  {
    act( "你不能提昇非玩家$N的信賴度。", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( ( level = atoi( arg2 ) ) < 0 || level > MAX_LEVEL - 1 )
  {
    chinese_number( MAX_LEVEL - 1, buf );
    act( "等級最高只能到$t級。", ch, buf, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( level > get_trust( ch ) )
  {
    send_to_char( "你提昇的信賴度比你自己還高﹐所以失敗了。\n\r", ch );
    RETURN_NULL();
  }

  victim->trust = level;
  RETURN_NULL();
}

FUNCTION( do_restore )
{
  char        arg[MAX_INPUT_LENGTH];
  CHAR_DATA * victim;

  PUSH_FUNCTION( "do_restore" );

  one_argument( argument, arg );

  if ( arg[0] == '!' )
  {
    if ( !( victim = get_char_world( ch, arg + 1 ) ) )
    {
      send_to_char( "你想恢復的對象不在這裡。\n\r", ch );
      RETURN_NULL();
    }

    while ( victim->affected ) affect_remove( victim, victim->affected );

    act( "$n移除所有你身上的法術。", ch, NULL, victim, TO_VICT );
    act( "移除$N身上所有的法術。", ch, NULL, victim, TO_CHAR );

    RETURN_NULL();
  }

  if ( !( victim = get_char_world( ch, arg ) ) )
  {
    send_to_char( "你想恢復的對象不在這裡。\n\r", ch );
    RETURN_NULL();
  }

  victim->hit  = get_curr_hit( victim );
  victim->mana = get_curr_mana( victim );
  victim->move = get_curr_move( victim );
  update_pos( victim );

  act( "$n恢復你的狀態。", ch, NULL, victim, TO_VICT );
  act( "恢復$N狀態完成。", ch, NULL, victim, TO_CHAR );

  RETURN_NULL();
}

FUNCTION( do_freeze )
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  PUSH_FUNCTION( "do_freeze" );

  one_argument( argument, arg );

  if ( !( victim = get_char_world( ch, arg ) ) )
  {
    send_to_char( "你要冷凍的對象不在這裡。\n\r", ch );
    RETURN_NULL();
  }

  if ( IS_NPC( victim ) )
  {
    act( "不能對非玩家$N做這個動作。", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( get_trust( victim ) >= get_trust( ch ) )
  {
    act( "$N的信賴度比你還要高﹐所以你失敗了。", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( IS_SET( victim->act, PLR_FREEZE ) )
  {
    REMOVE_BIT(victim->act, PLR_FREEZE);
    send_to_char( "你可以繼續玩了。\n\r", victim );
    act( "$N的冷凍旗標移除完成。", ch, NULL, victim, TO_CHAR );
  }
  else
  {
    SET_BIT( victim->act, PLR_FREEZE );
    send_to_char( "對不起﹐你現在不能做任何動作。\n\r", victim );
    act( "$N的冷凍旗標設定完成。", ch, NULL, victim, TO_CHAR );
  }

  save_char_obj( victim , SAVE_FILE );
  RETURN_NULL();
}

FUNCTION( do_log )
{
  char        arg[MAX_INPUT_LENGTH];
  CMD_DATA  * pCommand;
  CHAR_DATA * victim;

  PUSH_FUNCTION( "do_log" );

  argument = one_argument( argument, arg );

  if ( !str_cmp( arg, "cmd" ) )
  {
    if ( argument[0] == '\x0' )
    {
      send_to_char( "你要追蹤那個指令﹖\n\r", ch );
      RETURN_NULL();
    }

    for ( pCommand = cmd_list; pCommand; pCommand = pCommand->next )
    {
      if ( !str_cmp( pCommand->name, argument ) )
      {
        if ( pCommand->level > ch->level )
        {
          act( "指令 $2$t$0 的執行等級比你等級高﹐無法追蹤﹗"
            , ch, pCommand->name, NULL, TO_CHAR );
          RETURN_NULL();
        }

        if ( pCommand->mobonly )
        {
          act( "只有怪物才會執行這個指令 $2$t$0﹐所以追蹤無效﹗"
            , ch, argument, NULL, TO_CHAR );
          RETURN_NULL();
        }

        if ( pCommand->trace )
        {
          act( "你取消指令$t追蹤狀態﹗", ch, argument, NULL, TO_CHAR );
          pCommand->trace = FALSE;
        }
        else
        {
          act( "你把指令$t設定為追蹤﹗", ch, argument, NULL, TO_CHAR );
          pCommand->trace = TRUE;
        }

        RETURN_NULL();
      }
    }

    act( "找不到你要追蹤的指令 $2$t$0。", ch, argument, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( !str_cmp( arg, "all" ) )
  {
    if ( fLogAll )
    {
      fLogAll = FALSE;
      send_to_char( "所有玩家的標記旗標都被移除。\n\r", ch );
    }

    else
    {
      fLogAll = TRUE;
      send_to_char( "所有玩家都被標記起來了。\n\r", ch );
    }
    RETURN_NULL();
  }

  if ( !( victim = get_char_world( ch, arg ) ) )
  {
    send_to_char( "你要標記的對象不在這裡。\n\r", ch );
    RETURN_NULL();
  }

  if ( IS_NPC( victim ) )
  {
    act( "不能對非玩家$N做這個動作。", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( IS_SET( victim->act, PLR_LOG ) )
  {
    REMOVE_BIT(victim->act, PLR_LOG );
    act( "$N的標記旗標被移除了。", ch, NULL, victim, TO_CHAR );
  }
  else
  {
    SET_BIT( victim->act, PLR_LOG );
    act( "$N的標記旗標被標記了。", ch, NULL, victim, TO_CHAR );
  }

  RETURN_NULL();
}

FUNCTION( do_noemote )
{
  char        arg[MAX_INPUT_LENGTH];
  CHAR_DATA * victim;

  PUSH_FUNCTION( "do_noemote" );

  one_argument( argument, arg );

  if ( !( victim = get_char_world( ch, arg ) ) )
  {
    send_to_char( "你的對象不在這裡。\n\r", ch );
    RETURN_NULL();
  }

  if ( IS_NPC( victim ) )
  {
    act( "不能用在非玩家$N身上。", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( get_trust( victim ) >= get_trust( ch ) )
  {
    act( "你的信賴度比$N低﹐所以失敗了。", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( IS_SET( victim->act, PLR_NO_EMOTE ) )
  {
    REMOVE_BIT(victim->act, PLR_NO_EMOTE);
    send_to_char( "你可以重新使用emote這個指令。\n\r", victim );
    act( "$N的 NO_EMOTE旗標移除完畢。", ch, NULL, victim, TO_CHAR );
  }
  else
  {
    SET_BIT( victim->act, PLR_NO_EMOTE );
    send_to_char( "對不起﹐你不能使用emote這個指令了。\n\r", victim );
    act( "$N的 NO_EMOTE旗標設定完畢。", ch, NULL, victim, TO_CHAR );
  }

  RETURN_NULL();
}

FUNCTION( do_notell )
{
  char        arg[MAX_INPUT_LENGTH];
  char        arg2[MAX_INPUT_LENGTH];
  CHAR_DATA * victim;
  char        buf[MAX_STRING_LENGTH];
  int         timer;

  PUSH_FUNCTION( "do_notell" );

  argument = one_argument( argument, arg );
             one_argument( argument, arg2 );

  if ( arg[0] == '\x0' || arg2[0] == '\x0' || !is_number( arg2 ) )
  {
    send_to_char( "輸入錯誤﹐請見 notell /? \n\r", ch );
    RETURN_NULL();
  }

  if ( get_trust( ch ) < LEVEL_IMMORTAL
    && !IS_SET( ch->wizflags, WIZ_NOTELL ) )
  {
    send_to_char( "你的權力不足﹐不能設定別人不許\告訴別人話語﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( !( victim = get_char_world( ch, arg ) ) )
  {
    send_to_char( "你的對象不在這裡。\n\r", ch );
    RETURN_NULL();
  }

  if ( IS_NPC( victim ) )
  {
    act( "不能對非玩家$N做這個動作。", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( get_trust( victim ) >= get_trust( ch ) )
  {
    act( "你的信賴度比$N低﹐所以你失敗了。", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( ( timer = atoi( arg2 ) ) > MAX_TRIBUNAL || timer <= 0 )
  {
    chinese_number( MAX_TRIBUNAL, buf );
    act( "你設定的時間不能超過$t或小於零﹗", ch, buf, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( get_trust( ch ) == LEVEL_HERO
    && IS_SET( ch->wizflags, WIZ_NOTELL )
    && timer > MAX_TRIBUNAL_PC )
  {
    chinese_number( MAX_TRIBUNAL_PC, buf );
    act( "你給人懲罰的時間不可以超過$t。", ch, buf, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( victim->tribunal_timer > 0
    && !IS_SET( victim->act, PLR_NO_TELL ) )
  {
    act( "$N已經在受懲罰了﹐不要再懲罰他了﹗", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( IS_SET( victim->act, PLR_NO_TELL ) )
  {
    REMOVE_BIT(victim->act, PLR_NO_TELL);
    victim->tribunal_timer = 0;

    send_to_char( "你可以繼續使用 tell 這個指令了。\n\r", victim );
    act( "$N的 NO_TELL 旗標移除完畢。", ch, NULL, victim, TO_CHAR );

    sprintf( buf, "%s被取消 %s 的處置了﹗"
      , mob_name( NULL, victim ), act_bit_name( PLR_NO_TELL ) );

    talk_channel_2( buf, CHANNEL_BULLETIN, "制裁" );

    if ( IS_IMMORTAL( ch ) ) mudlog( LOG_WIZFLAGS, "%s 移除 %s 的禁 TELL ."
        , ch->name, mob_name( NULL, victim ) );
  }
  else
  {
    SET_BIT( victim->act, PLR_NO_TELL );
    victim->tribunal_timer = timer;

    send_to_char( "你不能使用 tell 這個指令了。\n\r", victim );
    act( "$N的 NO_TELL 旗標設定完畢。", ch, NULL, victim, TO_CHAR );

    sprintf( buf, "%s遭到 %s 的處置了﹗"
      , mob_name( NULL, victim ), act_bit_name( PLR_NO_TELL ) );

    talk_channel_2( buf, CHANNEL_BULLETIN, "制裁" );

    if ( IS_IMMORTAL( ch ) )
      mudlog( LOG_WIZFLAGS, "%s 設定 %s 不能使用 TELL %d 分鐘."
        , ch->name, mob_name( NULL, victim ) , timer );
  }

  RETURN_NULL();
}

FUNCTION( do_silence )
{
  char        arg[MAX_INPUT_LENGTH];
  char        arg2[MAX_INPUT_LENGTH];
  char        buf[MAX_STRING_LENGTH];
  CHAR_DATA * victim;
  int         timer;

  PUSH_FUNCTION( "do_silence" );

  argument = one_argument( argument, arg );
             one_argument( argument, arg2 );

  if ( arg[0] == '\x0' || arg2[0] == '\x0' || !is_number( arg2 ) )
  {
    send_to_char( "你的輸入不對﹐請見 silence /? \n\r", ch );
    RETURN_NULL();
  }

  if ( get_trust( ch ) < LEVEL_IMMORTAL
    && !IS_SET( ch->wizflags, WIZ_SILENCE ) )
  {
    send_to_char( "你的權力不足﹐不能設定別人禁言﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( !( victim = get_char_world( ch, arg ) ) )
  {
    send_to_char( "你的對象不在這裡。\n\r", ch );
    RETURN_NULL();
  }

  if ( IS_NPC( victim ) )
  {
    act( "不能對非玩家$N做這個指令。", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( get_trust( victim ) >= get_trust( ch ) )
  {
    act( "$N的信賴度比你高﹐所以你失敗了。", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( ( timer = atoi( arg2 ) ) > MAX_TRIBUNAL || timer <= 0 )
  {
    chinese_number( MAX_TRIBUNAL, buf );
    act( "你設定的時間不能超過$t或小於零﹗", ch, buf, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( IS_IMMORTAL( ch )
    && IS_SET( ch->wizflags, WIZ_SILENCE )
    && timer > MAX_TRIBUNAL_PC )
  {
    chinese_number( MAX_TRIBUNAL_PC, buf );
    act( "你給人懲罰的時間不可以超過$t天。", ch, buf, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( victim->tribunal_timer > 0
    && !IS_SET( victim->act, PLR_SILENCE ) )
  {
    act( "$N已經在受懲罰了﹐不要再懲罰他了﹗", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( IS_SET( victim->act, PLR_SILENCE ) )
  {
    REMOVE_BIT( victim->act, PLR_SILENCE );
    victim->tribunal_timer = 0;

    send_to_char( "你可以繼續使用你的頻道。\n\r", victim );
    act( "$N的 SILENCE旗標移除完畢。", ch, NULL, victim, TO_CHAR );

    sprintf( buf, "%s被取消 %s 的處置了﹗"
      , mob_name( NULL, victim ), act_bit_name( PLR_SILENCE ) );

    talk_channel_2( buf, CHANNEL_BULLETIN, "制裁" );

    if ( IS_IMMORTAL( ch ) ) mudlog( LOG_WIZFLAGS, "%s 移除 %s 的禁言."
        , ch->name, mob_name( NULL, victim ) );
  }
  else
  {
    SET_BIT( victim->act, PLR_SILENCE );
    victim->tribunal_timer = timer;

    send_to_char( "你沒有你的頻道了。\n\r", victim );
    act( "$N的 SILENCE旗標設定完畢。", ch, NULL, victim, TO_CHAR );

    sprintf( buf, "%s被標記 %s 的處置了﹗"
      , mob_name( NULL, victim ), act_bit_name( PLR_SILENCE ) );

    talk_channel_2( buf, CHANNEL_BULLETIN, "制裁" );

    if ( IS_IMMORTAL( ch ) )
      mudlog( LOG_WIZFLAGS, "%s 設定 %s 禁言 %d 分鐘."
        , ch->name, mob_name( NULL, victim ) , timer );
  }

  RETURN_NULL();
}

FUNCTION( do_peace )
{
  CHAR_DATA * rch;

  PUSH_FUNCTION( "do_peace" );

  for ( rch = ch->in_room->people; rch; rch = rch->next_in_room )
    if ( rch->fighting ) stop_fighting( rch, TRUE );

  sendmsg_to_room( "對不起﹐戰鬥停止﹗\n\r", ch );
  RETURN_NULL();
}

FUNCTION( do_ban )
{
  char       arg [MAX_INPUT_LENGTH];
  char       type[MAX_INPUT_LENGTH];
  BAN_DATA * pban;
  int        format;

  PUSH_FUNCTION( "do_ban" );

  argument = one_argument( argument, arg );
             one_argument( argument , type );

  if ( !arg[0] )
  {
    int Found = FALSE;

    clear_buffer();
    send_to_buffer( "禁止連接的位址﹕\n\r" );

    for ( pban = ban_list; pban; pban = pban->next )
    {
      Found = TRUE;
      send_to_buffer( "%-20s 旗標﹕%s\n\r"
        , pban->name, ban_bit_name( pban->type ) );

      if ( buffer_full() ) break;
    }

    if ( !Found ) send_to_buffer( "沒有任何被禁止的位址。\n\r" );
    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !type[0] )
  {
    send_to_char( "你必須引入位址旗標。\n\r" , ch );
    RETURN_NULL();
  }

  switch( ( format = atoi( type ) ) )
  {
  default :
    send_to_char( "你的位址旗標不對。\n\r" , ch );
    RETURN_NULL();

  case BAN_LOGIN  :
  case BAN_FQDN   :
  case BAN_FINGER :
    break;
  }

  for ( pban = ban_list; pban ; pban = pban->next )
  {
    if ( !str_cmp( arg, pban->name ) )
    {
      act( "這個位址 $t 已經被禁止連線了﹗", ch, arg, NULL, TO_CHAR );
      RETURN_NULL();
    }
  }

  /* 配置記憶體 */
  pban = alloc_struct( STRUCT_BAN_DATA );

  pban->name = str_dup( arg );
  pban->type = format;
  pban->next = ban_list;
  ban_list   = pban;

  act( "禁止位址 $t 設定完畢。", ch, arg, NULL, TO_CHAR );
  RETURN_NULL();
}

FUNCTION( do_allow )
{
  char       arg[MAX_INPUT_LENGTH];
  BAN_DATA * prev;
  BAN_DATA * curr;

  PUSH_FUNCTION( "do_allow" );

  one_argument( argument, arg );

  for ( curr = ban_list, prev = NULL; curr; prev = curr, curr = curr->next )
  {
    if ( !str_cmp( arg, curr->name ) )
    {
      if ( curr->type == BAN_LOGIN )
      {
        if ( !prev ) ban_list   = ban_list->next;
        else         prev->next = curr->next;

        free_string( curr->name );
        curr->type  = 0;

        /* 釋放記憶體 */
        free_struct( curr , STRUCT_BAN_DATA );

        send_to_char( "這個位址不再被禁止連線進來。\n\r", ch );
        RETURN_NULL();
      }
      else
      {
        send_to_char( "這個位址的旗標並非禁止連線。\n\r" , ch );
        RETURN_NULL();
      }
    }
  }

  send_to_char( "未發現到你想要的位址。\n\r", ch );
  RETURN_NULL();
}

FUNCTION( do_wizlock )
{
  char arg[MAX_INPUT_LENGTH];

  PUSH_FUNCTION( "do_wizlock" );

  argument = one_argument( argument, arg );

  if ( arg[0] != '\x0' && !str_cmp( arg, "new" ) )
  {
    NewLock = !NewLock;

    if ( NewLock ) send_to_char( "新玩家被管理者鎖定進入﹗\n\r", ch );
    else           send_to_char( "新玩家被管理者解除鎖定﹗\n\r", ch );
  }

  else
  {
    wizlock = !wizlock;
    if ( wizlock ) send_to_char( "遊戲被創世紀神鎖定了。\n\r", ch );
    else           send_to_char( "創世紀神解除遊戲鎖定。\n\r", ch );
  }

  RETURN_NULL();
}

/* 查詢技能或是法術的狀態 */
FUNCTION( do_llookup )
{
  SKILL_DATA    * pSkill;
  SKILL_DATA    * aSkill;
  LIMIT_DATA    * pLimit;
  DAMAGE_DATA   * pDamage;
  RESTRICT_DATA * pRestrict;
  AFFECT_DATA   * pAffect;
  int             loop;
  int             count;
  char            arg[MAX_INPUT_LENGTH];
  char            chinese[MAX_INPUT_LENGTH];

  PUSH_FUNCTION( "do_llookup" );

  if ( !skill_list )
  {
    act( "$t裡沒有任何技能載入.", ch, mud_name, NULL, TO_CHAR );
    RETURN_NULL();
  }

  one_argument( argument , arg );

  if ( !str_prefix( arg , "!list" ) )
  {
    clear_buffer();
    send_to_buffer( "\e[1;33;44m%6s %20s %20s %-20s\e[0m\n\r"
      , "編號" , "技能英文名稱" , "技能中文名稱", "分類" );

    for ( pSkill = skill_list; pSkill; pSkill = pSkill->next )
    {
      send_to_buffer( "%5d  %20s %20s %-10s\n\r"
        , pSkill->slot
        , pSkill->name
        , pSkill->cname ? pSkill->cname : "從缺"
        , pSkill->rating ? skill_rating( pSkill->rating ) : "從缺" );

      if ( buffer_full() ) break;
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "!innate" ) )
  {
    clear_buffer();

    for ( loop = count = 0; loop < nInnate; loop++ )
    {
      if ( !( pSkill = get_skill( iInnate[loop] ) ) ) continue;

      if ( count == 0 ) send_to_buffer(
         "\e[1;33;44m%4s %6s %8s %6s %20s %20s %-10s\e[0m\n\r"
         , "順序" , "編號" , "選取人數", "熟練度" , "技能英文名稱"
         , "技能中文名稱", "分類" );

      send_to_buffer( "%4d %5d  %8d %6d %20s %20s %-10s\n\r"
        , ++count
        , pSkill->slot
        , pSkill->choosen
        , pSkill->adeptation
        , pSkill->name
        , pSkill->cname ? pSkill->cname : "從缺"
        , pSkill->rating ? skill_rating( pSkill->rating ) : "從缺" );

      if ( buffer_full() ) break;
    }

    if ( count == 0 ) send_to_buffer( "本系統沒有任何預設技能﹗\n\r" );
    print_buffer( ch );
    RETURN_NULL();
  }

  if ( is_number( arg ) ) pSkill = get_skill( atoi( arg ) );
  else                    pSkill = skill_isname( arg );

  if ( !pSkill )
  {
    send_to_char( "找不到你要的法術或是技能名稱。\n\r" , ch );
    RETURN_NULL();
  }

  for ( loop = 0, pDamage = pSkill->damage; pDamage; pDamage = pDamage->next )
    loop++;

  clear_buffer();
  send_to_buffer(
    "法術號碼[%4d]\n\r"
    "英文名稱 %s\n\r"
    "中文名稱 %s\n\r"
    "法術的分類 %s\n\r"
    "相剋系法術 %s\n\r"
    "法術消失的訊息 %s\n\r"
    "施法等待時間 %d\n\r"
    "施法經驗獲取率 萬分之 %d\n\r"
    "施法浪費的數值 %d\n\r"
    "預選選取人數   %d\n\r"
    "預選的熟練度   %d\n\r"
    "是否可以互相學習﹕%s\n\r"
    "是否可以被致能﹕%s\n\r"
    "是否有要唸出咒語﹕%s\n\r"
    "是否有意志不集中﹕%s\n\r"
    "是否可以施展﹕%s\n\r"
    "是否為預設技能﹕%s\n\r"
    "是否可以查詢﹕%s\n\r"
    "是否開放﹕%s\n\r"
    "技能限制性別﹕%s(%d)\n\r"
    "練習機率﹕%d\n\r"
    "練習限制商數﹕%d\n\r"
    "影響效應﹕%s\n\r"
    "物品施法﹕%s\n\r"
    "攻傷敘述﹕%d\n\r"
    "攻擊武器﹕%s\n\r"
    "機率總和﹕%d\n\r"
    "攻擊整體訊息﹕\n\r%s\n\r"
    , pSkill->slot
    , pSkill->name
    , pSkill->cname   ? pSkill->cname   : "從缺"
    , pSkill->rating  ? skill_rating( pSkill->rating ) : "從缺"
    , pSkill->antirating ? skill_rating( pSkill->antirating ) : "從缺"
    , pSkill->msg_off ? pSkill->msg_off : "從缺"
    , pSkill->wait
    , pSkill->exp
    , pSkill->cost
    , pSkill->innate ? pSkill->choosen    : -1
    , pSkill->innate ? pSkill->adeptation : -1
    , YESNO( pSkill->teach         )
    , YESNO( pSkill->enable        )
    , YESNO( pSkill->say_spell     )
    , YESNO( pSkill->concentration )
    , YESNO( pSkill->cast          )
    , YESNO( pSkill->innate        )
    , YESNO( pSkill->canask        )
    , YESNO( pSkill->valid         )
    , sex_name( pSkill->sex )
    , pSkill->sex
    , pSkill->ply
    , pSkill->qutoient
    , skill_affect( pSkill->affect )
    , obj_cast_location( pSkill->affect_id )
    , loop, weapon_type_name( pSkill->weapon )
    , pSkill->chance
    , pSkill->message ? pSkill->message : "從缺" );

  send_to_buffer( "施法浪費的數值﹕" );

  switch( pSkill->cost_type )
  {
  default        : send_to_buffer( "未知"   ); break;
  case COST_HIT  : send_to_buffer( "生命力" ); break;
  case COST_MANA : send_to_buffer( "法力"   ); break;
  case COST_MOVE : send_to_buffer( "移動力" ); break;
  case COST_GOLD : send_to_buffer( "金錢"   ); break;
  }

  send_to_buffer( "\n\r施法的最低狀態﹕%s\n\r", get_position( pSkill->position ) );
  send_to_buffer( "施法的攻擊型態﹕" );

  switch( pSkill->type )
  {
    default                 : send_to_buffer( "未明"         ); break;
    case TAR_IGNORE         : send_to_buffer( "忽略對方"     ); break;
    case TAR_CHAR_OFFENSIVE : send_to_buffer( "攻擊對手"     ); break;
    case TAR_CHAR_DEFENSIVE : send_to_buffer( "本身防禦"     ); break;
    case TAR_CHAR_SELF      : send_to_buffer( "作用本身"     ); break;
    case TAR_OBJ_INV        : send_to_buffer( "作用物體隱形" ); break;
    case TAR_DODGE          : send_to_buffer( "本身閃躲"     ); break;
    case TAR_OBJ_ROOM       : send_to_buffer( "作用房間物品" ); break;
  }

  send_to_buffer( "\n\r法術呼叫函數型態﹕ " );
  if ( !pSkill->function )
  {
    send_to_buffer( "從缺。\n\r" );
  }
  else if ( pSkill->function == cast_null )
  {
    send_to_buffer( "不呼叫函數。\n\r" );
  }
  else
  {
    send_to_buffer( "有呼叫函數來處理。\n\r" );
  }

  if ( pSkill->associate >= 0 && ( aSkill = get_skill( pSkill->associate ) ) )
  {
    send_to_buffer( "相關連的函數號碼﹕%d 相關連的程度﹕萬分之 %d\n\r"
                    "相關連函數的名稱 %s %s\n\r"
      , aSkill->slot, pSkill->degree
      , aSkill->name, aSkill->cname );
  }

  for ( pLimit = pSkill->limit; pLimit; pLimit = pLimit->next )
  {
    if ( pLimit->class )
    {
      send_to_buffer( "%s等級最低 %d 級﹐熟練度最高能到 %3d%%。\n\r"
      , class_name( pLimit->class, FALSE )
      , pLimit->level
      , pLimit->adept );
    }
  }

  for ( count = 0, pRestrict = pSkill->restrict;
    pRestrict;
    pRestrict = pRestrict->next, count++ )
  {
    chinese_number( count + 1, chinese );
    send_to_buffer( "%s.本技能%s\n\r"
      , chinese, restrict_value( pRestrict, ch ) );
  }

  if ( ( pAffect = pSkill->affected ) )
  {
    send_to_buffer( "法術﹕%s對%s造成 %d 影響還有 %d 小時\n\r",
      pSkill->cname,
      affect_loc_name( pAffect->location ),
      pAffect->modifier,
      pAffect->duration );
  }

  if ( count == 0 ) send_to_buffer( "本技能沒有任何的限制。\n\r" );
  print_buffer( ch );
  RETURN_NULL();
}

/* 查詢什麼技能有什麼怪物教授 */
FUNCTION( do_lfind )
{
  int              vnum;
  int              nMatch;
  int              count;
  char             arg[MAX_INPUT_LENGTH];
  TEACH_DATA     * teach;
  MOB_INDEX_DATA * pMobIndex;
  SKILL_DATA     * pSkill;
  CHAR_DATA      * man;

  PUSH_FUNCTION( "do_lfind" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  one_argument( argument , arg );

  clear_buffer();

  if ( arg[0] == '!' )
  {
    if ( !( pSkill = skill_isname( arg + 1 ) ) )
    {
      send_to_char( "沒有這種技能或是法術﹐請使用 llookup 查詢。\n\r" , ch );
      RETURN_NULL();
    }

    send_to_buffer( "有教導 %s 這個技能的怪物有﹕\n\r", pSkill->cname );

    count = 0;
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man )
        || !IS_NPC( man )
        || !man->pIndexData
        || !man->pIndexData->teach )
        continue;

      for ( teach = man->pIndexData->teach; teach; teach = teach->next )
      {
        if ( pSkill->slot == teach->slot )
        {
          send_to_buffer( "(%2d) %-30s (號碼﹕%5d 所在地﹕%5d) \n\r"
            , ++count
            , mob_name( NULL, man )
            , man->pIndexData->vnum
            , man->in_room ? man->in_room->vnum : -1 );
        }
      }

      if ( buffer_full() ) break;
    }

    if ( count == 0 ) send_to_buffer( "沒有一隻怪物教授此技能。\n\r" );
  }

  else
  {
    if ( !( pSkill = skill_isname( arg ) ) )
    {
      send_to_char( "沒有這種技能或是法術﹐請使用 llookup 查詢。\n\r" , ch );
      RETURN_NULL();
    }

    send_to_buffer( "有教導 %s 這個技能的怪物有﹕\n\r", pSkill->cname );

    for ( count = nMatch = vnum = 0; nMatch < top_mob_index; vnum ++ )
    {
      if ( ( pMobIndex = get_mob_index( vnum ) ) )
      {
        nMatch++;
        for ( teach = pMobIndex->teach; teach; teach = teach->next )
        {
          if ( pSkill->slot == teach->slot )
          {
            send_to_buffer( "(%2d) %s (號碼﹕%5d 數量﹕%3d )\n\r"
              , ++count
              , mob_index_name( NULL, pMobIndex )
              , pMobIndex->vnum
              , pMobIndex->count );
          }
        }

        if ( buffer_full() ) break;
      }
    }

    if ( count == 0 ) send_to_buffer( "沒有一隻怪物教授此技能。\n\r" );
  }

  print_buffer( ch );
  RETURN_NULL();
}

/* 設定技能的熟練度 */
FUNCTION( do_lset )
{
  CHAR_DATA   * victim;
  SKILL_DATA  * pSkill;
  ENABLE_DATA * pEnable;
  ENABLE_DATA * zEnable;
  char          arg1[MAX_INPUT_LENGTH];
  char          arg2[MAX_INPUT_LENGTH];
  char          arg3[MAX_INPUT_LENGTH];
  bool          fAll;
  int           value;

  PUSH_FUNCTION( "do_lset" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  /* 先取出三個參數 */
  argument = one_argument( argument , arg1 );
  argument = one_argument( argument , arg2 );
  one_argument( argument , arg3 );

  /* 察看是否參數都有正確 */
  if ( !arg1[0] || !arg2[0] || !arg3[0] )
  {
    send_to_char( "語法﹕lset <對象> <技能> <熟練度>\n\r"
                  "或是  lset <對象> all    <熟練度>\n\r" , ch );
    RETURN_NULL();
  }

  /* 先找找看是否有這個人 */
  if ( !( victim = get_char_world( ch , arg1 ) ) )
  {
    send_to_char( "找不到你要的對象。\n\r", ch );
    RETURN_NULL();
  }

  /* 不能作用在非玩家身上 */
  if ( IS_NPC( victim ) )
  {
    act( "不能對非玩家$N做這個動作。" , ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  fAll = !str_cmp( arg2 , "all" );

  /* 找尋技能名稱, 若是第二個參數為 all就不用找了 */
  if ( !( ( pSkill = skill_isname( arg2 ) ) ) && !fAll )
  {
    send_to_char( "找不到這項法術或是技能。\n\r" , ch );
    RETURN_NULL();
  }

  /* 第三個數值不能為非數字 */
  if ( !is_number( arg3 ) )
  {
    send_to_char( "熟練度這一項一定要是數字。\n\r" , ch );
    RETURN_NULL();
  }

  if ( ( value = atoi( arg3 ) ) < 0 || value > 100 )
  {
    send_to_char( "熟練度的數字不合理。\n\r", ch );
    RETURN_NULL();
  }

  if ( fAll )
  {
    for ( pSkill = skill_list; pSkill; pSkill = pSkill->next )
    {
      if ( pSkill->slot < 0 || pSkill->slot >= MAX_SKILL )
      {
        mudlog( LOG_DEBUG, "do_lset: 技能號碼 %d 不合理.", pSkill->slot );
        continue;
      }

      /* 檢查是否可以學到 */
      if ( value > 0 && !check_skill_restrict( victim, pSkill, FALSE ) )
        continue;

      if ( ( victim->skill[pSkill->slot] = value ) <= 0 )
      {
        /* 取消他的這項技能 */
        for ( pEnable = victim->enable; pEnable; pEnable = zEnable )
        {
          zEnable = pEnable->next;

          if ( pEnable->skill == pSkill )
            extract_an_enable( victim, pSkill );
        }
      }
    }
  }

  else
  {
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
  }

  send_to_char( "設定技能完畢。\n\r" , ch );
  RETURN_NULL();
}

FUNCTION( do_mset )
{
  char         arg1[MAX_INPUT_LENGTH];
  char         arg2[MAX_INPUT_LENGTH];
  char         arg3[MAX_INPUT_LENGTH];
  char         buf[MAX_STRING_LENGTH];
  char         buf1[MAX_STRING_LENGTH];
  CHAR_DATA  * victim;
  CLASS_DATA * pClass;
  int          value;

  PUSH_FUNCTION( "do_mset" );

  smash_tilde( argument );
  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );
  str_cpy( arg3, argument );

  if ( !arg1[0] || !arg2[0] || !arg3[0] )
  {
    send_to_char(
      "語法﹕mset <對象> <項目> <數值>\n\r"
      "或是  mset <對象> <字串> <數值>\n\r\n\r"
      "項目可以有下列選擇﹕\n\r"
      "  str int wis dex con sex class level nskill\n\r"
      "  gold hp mana limit move practice align xname rechristen\n\r"
      "  thirst drunk full recall bank firman murder steal dosage\n\r"
      "  donate flags migrate tractable affect\n\r"
      "字串可以有下列選擇﹕\n\r"
      "  name short long description title\n\r", ch );
    RETURN_NULL();
  }

  if ( !( victim = get_char_world( ch, arg1 ) ) )
  {
    send_to_char( "你的對象不在這裡。\n\r", ch );
    RETURN_NULL();
  }

  value = is_number( arg3 ) ? atoi( arg3 ) : -1;

  if ( !str_prefix( arg2, "flags" ) )
  {
    if ( !IS_NPC( victim ) )
    {
      act( "$N不是非玩家﹗", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( !str_prefix( arg3, "sentinel" ) )
    {
      if ( IS_SET( victim->act, ACT_SENTINEL ) )
      {
        REMOVE_BIT( victim->act, ACT_SENTINEL );
        act( "$N不再是不可移動﹗", ch, NULL, victim, TO_CHAR );
      }
      else
      {
        SET_BIT( victim->act, ACT_SENTINEL );
        act( "$N又恢復不可移動的性質﹗", ch, NULL, victim, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "scavenger" ) )
    {
      if ( IS_SET( victim->act, ACT_SCAVENGER ) )
      {
        REMOVE_BIT( victim->act, ACT_SCAVENGER );
        act( "$N不再是會自動撿物品﹗", ch, NULL, victim, TO_CHAR );
      }
      else
      {
        SET_BIT( victim->act, ACT_SCAVENGER );
        act( "$N又恢復會自動撿物品的性質﹗", ch, NULL, victim, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "aggressive" ) )
    {
      if ( IS_SET( victim->act, ACT_AGGRESSIVE ) )
      {
        REMOVE_BIT( victim->act, ACT_AGGRESSIVE );
        act( "$N不再是會自動攻擊﹗", ch, NULL, victim, TO_CHAR );
      }
      else
      {
        SET_BIT( victim->act, ACT_AGGRESSIVE );
        act( "$N又恢復會自動攻擊的性質﹗", ch, NULL, victim, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "stayarea" ) )
    {
      if ( IS_SET( victim->act, ACT_STAY_AREA ) )
      {
        REMOVE_BIT( victim->act, ACT_STAY_AREA );
        act( "$N不再是會停留在本區域﹗", ch, NULL, victim, TO_CHAR );
      }
      else
      {
        SET_BIT( victim->act, ACT_STAY_AREA );
        act( "$N又恢復會停留在本區域的性質﹗", ch, NULL, victim, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "wimpy" ) )
    {
      if ( IS_SET( victim->act, ACT_WIMPY ) )
      {
        REMOVE_BIT( victim->act, ACT_WIMPY );
        act( "$N不再是會受傷逃跑﹗", ch, NULL, victim, TO_CHAR );
      }
      else
      {
        SET_BIT( victim->act, ACT_WIMPY );
        act( "$N又恢復會受傷逃跑的性質﹗", ch, NULL, victim, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "train" ) )
    {
      if ( IS_SET( victim->act, ACT_TRAIN ) )
      {
        REMOVE_BIT( victim->act, ACT_TRAIN );
        act( "$N不再是訓練屬性者﹗", ch, NULL, victim, TO_CHAR );
      }
      else
      {
        SET_BIT( victim->act, ACT_TRAIN );
        act( "$N又恢復訓練屬性者的性質﹗", ch, NULL, victim, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "rebirth" ) )
    {
      if ( IS_SET( victim->act, ACT_REBIRTH ) )
      {
        REMOVE_BIT( victim->act, ACT_REBIRTH );
        act( "$N不再是轉職者﹗", ch, NULL, victim, TO_CHAR );
      }
      else
      {
        SET_BIT( victim->act, ACT_REBIRTH );
        act( "$N又恢復轉職者的性質﹗", ch, NULL, victim, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "fight" ) )
    {
      if ( IS_SET( victim->act, ACT_REBORN_FIGHT ) )
      {
        REMOVE_BIT( victim->act, ACT_REBORN_FIGHT );
        act( "$N不再是會再生成其他怪物﹗", ch, NULL, victim, TO_CHAR );
      }
      else
      {
        SET_BIT( victim->act, ACT_REBORN_FIGHT );
        act( "$N又恢復會再生成其他怪物的性質﹗", ch, NULL, victim, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "ask" ) )
    {
      if ( IS_SET( victim->act, ACT_ASK ) )
      {
        REMOVE_BIT( victim->act, ACT_ASK );
        act( "$N不再是會叫人打工﹗", ch, NULL, victim, TO_CHAR );
      }
      else
      {
        SET_BIT( victim->act, ACT_ASK );
        act( "$N又恢復會叫人打工的性質﹗", ch, NULL, victim, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "noreborn" ) )
    {
      if ( IS_SET( victim->act, ACT_NOREBORN ) )
      {
        REMOVE_BIT( victim->act, ACT_NOREBORN );
        act( "$N不再是不會重生﹗", ch, NULL, victim, TO_CHAR );
      }
      else
      {
        SET_BIT( victim->act, ACT_NOREBORN );
        act( "$N又恢復會不會重生的性質﹗", ch, NULL, victim, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "nokill" ) )
    {
      if ( IS_SET( victim->act, ACT_NOKILL ) )
      {
        REMOVE_BIT( victim->act, ACT_NOKILL );
        act( "$N不再是不可殺害﹗", ch, NULL, victim, TO_CHAR );
      }
      else
      {
        SET_BIT( victim->act, ACT_NOKILL );
        act( "$N又恢復不可殺害的性質﹗", ch, NULL, victim, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "enroll" ) )
    {
      if ( IS_SET( victim->act, ACT_ENROLL ) )
      {
        REMOVE_BIT( victim->act, ACT_ENROLL );
        act( "$N不再是會記恨﹗", ch, NULL, victim, TO_CHAR );
      }
      else
      {
        SET_BIT( victim->act, ACT_ENROLL );
        act( "$N又恢復記恨的性質﹗", ch, NULL, victim, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "good" ) )
    {
      if ( IS_SET( victim->act, ACT_ALIGN_GOOD ) )
      {
        REMOVE_BIT( victim->act, ACT_ALIGN_GOOD );
        act( "$N不再是善良之人﹗", ch, NULL, victim, TO_CHAR );
      }
      else
      {
        SET_BIT( victim->act, ACT_ALIGN_GOOD );
        act( "$N又恢復善良之人的性質﹗", ch, NULL, victim, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "evil" ) )
    {
      if ( IS_SET( victim->act, ACT_ALIGN_EVIL ) )
      {
        REMOVE_BIT( victim->act, ACT_ALIGN_EVIL );
        act( "$N不再是邪惡之人﹗", ch, NULL, victim, TO_CHAR );
      }
      else
      {
        SET_BIT( victim->act, ACT_ALIGN_EVIL );
        act( "$N又恢復邪惡之人的性質﹗", ch, NULL, victim, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "nosummon" ) )
    {
      if ( IS_SET( victim->act,ACT_NOSUMMON ) )
      {
        REMOVE_BIT( victim->act, ACT_NOSUMMON );
        act( "$N不再是不可召喚之人﹗", ch, NULL, victim, TO_CHAR );
      }
      else
      {
        SET_BIT( victim->act, ACT_NOSUMMON );
        act( "$N又恢復不可召喚之人的性質﹗", ch, NULL, victim, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "speak" ) )
    {
      if ( victim->speak )
      {
        victim->speak = FALSE;
        act( "$N不再是不會講話之人﹗", ch, NULL, victim, TO_CHAR );
      }
      else
      {
        victim->speak = TRUE;
        act( "$N又恢復會講話的性質﹗", ch, NULL, victim, TO_CHAR );
      }
    }

    else
    {
      send_to_char(
        "人物的旗標只有 sentinel scavenger aggressive stayarea wimpy\n\r"
        "               train rebirth fight ask noreborn nokill nosummon\n\r"
        "               enroll good evil speak\n\r"
        , ch );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "strength" ) )
  {
    if ( value < 0 || value > MaxStr )
    {
      act( "只能改變力量由 0 到 $x。" , ch, &MaxStr, NULL, TO_CHAR );
    }

    else
    {
      victim->now_str = value;
      act( "你設定$N力量為 $x。", ch, &value, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "intelligence" ) )
  {

    if ( value < 0 || value > MaxInt )
    {
      act( "只能智力由 0 到 $x。", ch , &MaxInt, NULL, TO_CHAR );
    }

    else
    {
      victim->now_int = value;
      act( "你設定$N智力為 $x。", ch, &value, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "wisdom" ) )
  {
    if ( value < 0 || value > MaxWis )
    {
      act( "只能改變學識由 0 到 $x。", ch , &MaxWis, NULL, TO_CHAR );
    }

    else
    {
      victim->now_wis = value;
      act( "你設定$N學識為 $x。", ch, &value, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "dexterity" ) )
  {
    if ( value < 0 || value > MaxDex )
    {
      act( "只能改變敏捷由 0 到 $x。" , ch, &MaxDex, NULL, TO_CHAR );
    }

    else
    {
      victim->now_dex = value;
      act( "你設定$N敏捷為 $x。", ch, &value, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "constitutin" ) )
  {
    if ( value < 0 || value > MaxCon )
    {
      act( "只能改變體格由 0 到 $x。" , ch, &MaxCon, NULL, TO_CHAR );
    }

    else
    {
      victim->now_con = value;
      act( "你設定$N體格為 $x。", ch, &value, victim, TO_CHAR );
    }

   RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "sex" ) )
  {
         if ( !str_prefix( arg3, "male"   ) ) value = SEX_MALE;
    else if ( !str_prefix( arg3, "female" ) ) value = SEX_FEMALE;

    switch( value )
    {
    default:
      print_to_char( ch, "性別只有%s(%d)和%s(%d)。\n\r"
        , sex_name( SEX_MALE )  , SEX_MALE
        , sex_name( SEX_FEMALE ), SEX_FEMALE );
      break;

    case SEX_FEMALE:
      victim->sex = SEX_FEMALE;
      act( "你把$N性別改成$t了。", ch, sex_name( victim->sex ), victim, TO_CHAR );
      break;

    case SEX_MALE:
      victim->sex = SEX_MALE;
      act( "你把$N性別改成$t了。", ch, sex_name( victim->sex ), victim, TO_CHAR );
      break;
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "class" ) )
  {
    if ( !( pClass = is_class( arg3, TRUE ) ) )
    {
      send_to_char( "職業名稱不合法。\n\r" , ch );
      RETURN_NULL();
    }

    victim->class = pClass;
    act( "你把$N職業改成$t了。", ch , class_name( pClass, TRUE ), victim, TO_CHAR );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "level" ) )
  {
    if ( !IS_NPC( victim ) )
    {
      act( "不能對玩家$N做這個動作。", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( value < 0 || value >= LEVEL_HERO )
    {
      chinese_number( LEVEL_HERO - 1, buf );
      act( "等級範圍由 0 到 $t。", ch ,buf, NULL, TO_CHAR );
    }

    else
    {
      victim->level = value;
      act( "你設定$N等級為$i。", ch, &value, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "gold" ) )
  {
    if ( value < 0 )
    {
      send_to_char( "不能改變成負值。\n\r" , ch );
    }
    else
    {
      victim->gold = value;
      act( "你設定$N金錢為 $i 兩黃金。", ch, &value, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "steal" ) )
  {
    if ( IS_NPC( victim ) || !victim->pcdata )
    {
      act( "不能對非玩家$N做這個動作。", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( value < 0 )
    {
      send_to_char( "不能改變成負值。\n\r" , ch );
    }

    else
    {
      victim->pcdata->steal = value;
      act( "你設定$N偷竊次數為 $i 次。", ch, &value, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "dosage" ) )
  {
    if ( IS_NPC( victim ) || !victim->pcdata )
    {
      act( "不能對非玩家$N做這個動作。", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( value < 0 )
    {
      send_to_char( "不能改變成負值。\n\r" , ch );
    }

    else
    {
      victim->pcdata->dosage = value;
      act( "你設定$N藥劑為$i。", ch, &value, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "donate" ) )
  {
    if ( IS_NPC( victim ) )
    {
      act( "不能對非玩家$N做這個動作。", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( value < 0 )
    {
      send_to_char( "不能改變成負值。\n\r" , ch );
    }

    else
    {
      victim->donate = value;
      act( "你設定$N接受救濟時間為$i。", ch, &value, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "murder" ) )
  {
    if ( IS_NPC( victim ) || !victim->pcdata )
    {
      act( "不能對非玩家$N做這個動作。", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( value < 0 )
    {
      send_to_char( "不能改變成負值。\n\r" , ch );
    }
    else
    {
      victim->pcdata->murder = value;
      act( "你設定$N謀殺次數為$i次。", ch, &value, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "bank" ) )
  {
    if ( IS_NPC( victim ) )
    {
      act( "不能對非玩家$N做這個動作。", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( value < 0 )
    {
      send_to_char( "不能改變成負值。\n\r" , ch );
    }

    else
    {
      victim->bank = value;
      act( "你設定$N銀行存款為$i兩銀子。", ch, &value, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "firman" ) )
  {
    if ( IS_NPC( victim ) )
    {
      act( "不能對非玩家$N做這個動作。", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( value < 0 )
    {
         send_to_char( "不能改變成負值。\n\r" , ch );
    }

    else if ( value > MaxFirman )
    {
      send_to_char( "數值太大了。\n\r", ch );
    }

    else
    {
      victim->firman = value;
      act( "你設定$N免死金牌數為$i張。", ch, &value, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "nskill" ) )
  {
    if ( IS_NPC( victim ) )
    {
      act( "不能對非玩家$N做這個動作。", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( value < 0 )
    {
      send_to_char( "不能改變成負值。\n\r" , ch );
    }

    else if ( value > MaxNSkill )
    {
      send_to_char( "數值太大了。\n\r", ch );
    }

    else
    {
      victim->nskill = value;
      act( "你設定$N額外技能數為$i。", ch, &value, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "rechristen" ) )
  {
    if ( IS_NPC( victim ) || !victim->pcdata )
    {
      act( "不能對非玩家$N做這個動作。", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( value < 0 )
    {
      send_to_char( "不能改變成負值。\n\r" , ch );
    }
    else
    {
      victim->pcdata->rechristen = value;
      act( "你設定$N改中文名稱次數為$i次。", ch, &value, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "xname" ) )
  {
    if ( IS_NPC( victim ) || !victim->pcdata )
    {
      act( "不能對非玩家$N做這個動作。", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( value < 0 )
    {
      send_to_char( "不能改變成負值。\n\r" , ch );
    }

    else
    {
      victim->pcdata->xname = value;
      act( "你設定$N免不雅字次數為$i次。", ch, &value, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "hp" ) )
  {
    if ( value < 0 || value > MaxHit )
    {
      act( "生命力範圍由 0 到 $x。", ch, &MaxHit, NULL, TO_CHAR );
    }

    else
    {
      victim->max_hit = value;
      act( "你設定$N生命力為 $x。", ch, &value, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "mana" ) )
  {
    if ( value < 0 || value > MaxMana )
    {
      act( "法力範圍由 0 到 $x。", ch, &MaxMana, NULL, TO_CHAR );
    }

    else
    {
      victim->max_mana = value;
      act( "你設定$N法力為 $x。", ch, &value, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "move" ) )
  {
    if ( value < 0 || value > MaxMove )
    {
      act( "移動力範圍由 0 到 $x。", ch, &MaxMove, NULL, TO_CHAR );
    }

    else
    {
      victim->max_move = value;
      act( "你設定$N移動力為 $x。", ch, &value, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "limit" ) )
  {
    if ( value < 0 || value > MAX_LIMIT_VALUE  )
    {
      chinese_number( MAX_LIMIT_VALUE, buf );
      act( "移動力範圍由 0 到 $t。", ch, buf, NULL, TO_CHAR );
    }

    else
    {
      victim->limit = value;
      act( "你設定$N極限技值為 $x。", ch, &value, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "practice" ) )
  {
    if ( value < 0 || value > MaxPractice )
    {
      act( "學習點數範圍由 0 到 $x 點。", ch, &MaxPractice, NULL, TO_CHAR );
    }

    else
    {
      victim->practice = value;
      act( "你設定$N學習點數為 $x。", ch, &value, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "alignment" ) )
  {
    if ( value < -AlignRange || value > AlignRange )
    {
      chinese_number( -AlignRange, buf  );
      chinese_number(  AlignRange, buf1 );

      act( "陣營範圍由 $t 到 $T。", ch, buf, buf1, TO_CHAR );
    }

    else
    {
      victim->alignment = value;
      act( "你設定$N陣營為 $x。", ch, &value, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "thirst" ) )
  {
    if ( IS_NPC( victim ) || !victim->pcdata )
    {
      act( "不能對非玩家$N做這個動作。", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( value < 0 || value > 100 )
    {
      send_to_char( "口渴程度範圍由 0 到 100。\n\r", ch );
    }

    else
    {
      victim->pcdata->condition[COND_THIRST] = value;
      act( "你設定$N口渴程度為 $x。", ch, &value, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "drunk" ) )
  {
    if ( IS_NPC( victim ) || !victim->pcdata )
    {
      act( "不能對非玩家$N做這個動作。", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( value < 0 || value > 100 )
    {
      send_to_char( "酒醉程度範圍由 0 到 100。\n\r", ch );
    }

    else
    {
      victim->pcdata->condition[COND_DRUNK] = value;
      act( "你設定$N酒醉程度為 $x。", ch, &value, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "full" ) )
  {
    if ( IS_NPC( victim ) || !victim->pcdata )
    {
      act( "不能對非玩家$N做這個動作。", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( value < 0 || value > 100 )
    {
      send_to_char( "肚子餓程度範圍由 0 到 100。\n\r", ch );
    }

    else
    {
      victim->pcdata->condition[COND_FULL] = value;
      act( "設定$N肚子餓程度為 $x。", ch, &value, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "name" ) )
  {
    if ( !IS_NPC( victim ) )
    {
      act( "不能對非玩家$N做這個動作。", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    free_string( victim->name );
    victim->name = str_dup( arg3 );
    act( "設定$N名稱為 $t。", ch, arg3, victim, TO_CHAR );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "short" ) )
  {
    if ( !IS_NPC( victim ) )
    {
      act( "不能對非玩家$N做這個動作。", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    free_string( victim->byname );
    victim->byname = str_dup( arg3 );
    act( "設定$N短敘述為 $t。", ch, arg3, victim, TO_CHAR );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "long" ) )
  {
    if ( !IS_NPC( victim ) )
    {
      act( "不能對非玩家$N做這個動作。", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    free_string( victim->long_descr );
    victim->long_descr = str_dup( arg3 );
    act( "設定$N長敘述為 $t。", ch, arg3, victim, TO_CHAR );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "title" ) )
  {
    if ( IS_NPC( victim ) )
    {
      act( "不能對非玩家$N做這個動作。", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    set_title( victim, arg3 );
    act( "設定$N抬頭為 $t。", ch, arg3, victim, TO_CHAR );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "migrate" ) )
  {
    if ( !IS_NPC( victim ) || !victim->pIndexData )
    {
      act( "不能對玩家$N做這個動作。", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( value < 0 || value > 100 )
    {
      send_to_char( "遷移機率只能由 0 到 100。\n\r", ch );
    }
    else
    {
      victim->migrate = value;
      act( "你設定$N遷移率為百分之 $i。", ch, &value, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "tractable" ) )
  {
    if ( !IS_NPC( victim ) || !victim->pIndexData )
    {
      act( "不能對玩家$N做這個動作。", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( value < 0 || value > 100 )
    {
      send_to_char( "馴服機率只能由 0 到 100。\n\r", ch );
    }
    else
    {
      victim->tractable = value;
      act( "你設定$N馴服率為百分之 $i。", ch, &value, victim, TO_CHAR );
    }

    RETURN_NULL();
  }

  /* 如果都不對, 則出現錯誤訊息 */
  do_mset( ch, "" );
  RETURN_NULL();
}

FUNCTION( do_oset )
{
  char         arg1[MAX_INPUT_LENGTH];
  char         arg2[MAX_INPUT_LENGTH];
  char         arg3[MAX_INPUT_LENGTH];
  char         buf[MAX_STRING_LENGTH];
  OBJ_DATA   * obj;
  SITUS_DATA * pSitus;
  SITUS_DATA * aSitus;
  int          correct;
  int          value;

  PUSH_FUNCTION( "do_oset" );

  smash_tilde( argument );
  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );
  str_cpy( arg3, argument );

  if ( !arg1[0] || !arg2[0] || !arg3[0] )
  {
    send_to_char(
      "語法 oset <物品> <項目> <字串>\n\r"
      "或是 oset <物品> <字串> <字串>\n\r\n\r"
      "項目可以有下列選擇﹕\n\r"
      "  v0 v1 v2 v3 .... v14\n\r"
      "  level weight cost timer\n\r"
      "字串可以有下列選擇﹕\n\r"
      "  name short long ed wear flags\n\r", ch );
    RETURN_NULL();
  }

  if ( !( obj = get_obj_world( ch, arg1 ) ) )
  {
    send_to_char( "找不到你所要改變的物品。\n\r", ch );
    RETURN_NULL();
  }

  /* 讀入數值, 但也不一定需要是數值. */
  value = atoi( arg3 );

  if ( arg2[0] == 'v' && is_number( arg2 + 1 ) )
  {
    int  count;
    char cNumber[MAX_INPUT_LENGTH];

    if ( ( count = atoi( arg2 + 1 ) ) < 0 || count >= MAX_OBJECT_VALUE )
    {
      chinese_number( MAX_OBJECT_VALUE, buf );
      act( "數值只能由 0 到 $t。", ch, buf, NULL, TO_CHAR );
      RETURN_NULL();
    }

    chinese_number( count, cNumber );

    str_cpy( buf, check_obj_value( obj->item_type, count, value, &correct, TRUE ) );

    if ( buf[0] )
    {
      act( "更改$p數值錯誤﹐原因﹕$T", ch, obj, buf, TO_CHAR );
      RETURN_NULL();
    }

    obj->value[count] = value;
    sprintf( buf, "你設定$p的數值%s為%d。", cNumber, value );
    act( smash_act_keyword( buf, "nesp" ), ch, obj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "wear" ) )
  {
    if ( obj->wear_loc != WEAR_NONE )
    {
      act( "$p還被穿在身上耶﹐所以無法更改。", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    for ( pSitus = situs_list; pSitus; pSitus = pSitus->next )
    {
      if ( !str_cmp( arg3, pSitus->name ) )
      {
        if ( CAN_WEAR( obj, pSitus->wear ) )
        {
          REMOVE_BIT( obj->wear_flags, pSitus->wear );
          act( "你移除$p的$T的旗標﹗", ch, obj, pSitus->situs_name, TO_CHAR );
          RETURN_NULL();
        }
        else
        {
          /* 先移除其他的旗標 */
          for ( aSitus = situs_list; aSitus; aSitus = aSitus->next )
            REMOVE_BIT( obj->wear_flags, aSitus->wear );

          SET_BIT( obj->wear_flags, pSitus->wear );
          act( "你設定$p的$T的旗標﹗", ch, obj, pSitus->situs_name, TO_CHAR );
          RETURN_NULL();
        }
      }
    }

    clear_buffer();
    send_to_buffer( "物品穿戴旗標只有如下﹕\n\r" );

    for ( value = 1, pSitus = situs_list; pSitus; pSitus = pSitus->next )
    {
      send_to_buffer( "%2d) %-12s %s\n\r"
        , value++, pSitus->name, pSitus->situs_name );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "flags" ) )
  {
    if ( !str_prefix( arg3, "glow" ) )
    {
      if ( obj->Glow )
      {
        obj->Glow = FALSE;
        act( "$p不再閃耀動人﹗", ch, obj, NULL, TO_CHAR );
      }
      else
      {
        obj->Glow = TRUE;
        act( "$p又恢復閃耀動人﹗", ch, obj, NULL, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "hum" ) )
    {
      if ( obj->Hum )
      {
        obj->Hum = FALSE;
        act( "$p不再嗡嗡作響﹗", ch, obj, NULL, TO_CHAR );
      }
      else
      {
        obj->Hum = TRUE;
        act( "$p又恢復嗡嗡作響﹗", ch, obj, NULL, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "dark" ) )
    {
      if ( obj->Dark )
      {
        obj->Dark = FALSE;
        act( "$p不再黑暗無光﹗", ch, obj, NULL, TO_CHAR );
      }
      else
      {
        obj->Dark = TRUE;
        act( "$p又恢復黑暗無光﹗", ch, obj, NULL, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "evil" ) )
    {
      if ( obj->Evil )
      {
        obj->Evil = FALSE;
        act( "$p不再邪惡﹗", ch, obj, NULL, TO_CHAR );
      }
      else
      {
        obj->Evil = TRUE;
        act( "$p又恢復邪惡﹗", ch, obj, NULL, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "invis" ) )
    {
      if ( obj->Invis )
      {
        obj->Invis = FALSE;
        act( "$p不再隱形﹗", ch, obj, NULL, TO_CHAR );
      }
      else
      {
        obj->Invis = TRUE;
        act( "$p又恢復隱形﹗", ch, obj, NULL, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "magic" ) )
    {
      if ( obj->Magic )
      {
        obj->Magic = FALSE;
        act( "$p不再具魔法性質﹗", ch, obj, NULL, TO_CHAR );
      }
      else
      {
        obj->Magic = TRUE;
        act( "$p又恢復魔法性質﹗", ch, obj, NULL, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "nodrop" ) )
    {
      if ( obj->NoDrop )
      {
        obj->NoDrop = FALSE;
        act( "$p不再不可丟棄﹗", ch, obj, NULL, TO_CHAR );
      }
      else
      {
        obj->NoDrop = TRUE;
        act( "$p又恢復不可丟棄性質﹗", ch, obj, NULL, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "bless" ) )
    {
      if ( obj->Bless )
      {
        obj->Bless = FALSE;
        act( "$p不再具祝福性質﹗", ch, obj, NULL, TO_CHAR );
      }
      else
      {
        obj->Bless = TRUE;
        act( "$p又恢復祝福性質﹗", ch, obj, NULL, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "antigood" ) )
    {
      if ( obj->AntiGood )
      {
        obj->AntiGood = FALSE;
        act( "$p不再具抗善良性質﹗", ch, obj, NULL, TO_CHAR );
      }
      else
      {
        obj->AntiGood = TRUE;
        act( "$p又恢復抗善良性質﹗", ch, obj, NULL, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "antievil" ) )
    {
      if ( obj->AntiEvil )
      {
        obj->AntiEvil = FALSE;
        act( "$p不再具抗邪惡性質﹗", ch, obj, NULL, TO_CHAR );
      }
      else
      {
        obj->AntiEvil = TRUE;
        act( "$p又恢復抗邪惡性質﹗", ch, obj, NULL, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "antineutral" ) )
    {
      if ( obj->AntiNeutral )
      {
        obj->AntiNeutral = FALSE;
        act( "$p不再具抗中性性質﹗", ch, obj, NULL, TO_CHAR );
      }
      else
      {
        obj->AntiNeutral = TRUE;
        act( "$p又恢復抗中性性質﹗", ch, obj, NULL, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "noremove" ) )
    {
      if ( obj->NoRemove )
      {
        obj->NoRemove = FALSE;
        act( "$p不再不可移除﹗", ch, obj, NULL, TO_CHAR );
      }
      else
      {
        obj->NoRemove = TRUE;
        act( "$p又恢復不可移除性質﹗", ch, obj, NULL, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "inventory" ) )
    {
      if ( obj->Inventory )
      {
        obj->Inventory = FALSE;
        act( "$p不再隱形(inventory)﹗", ch, obj, NULL, TO_CHAR );
      }
      else
      {
        obj->Inventory = TRUE;
        act( "$p又恢復隱形(inventory)性質﹗", ch, obj, NULL, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "nosave" ) )
    {
      if ( obj->NoSave )
      {
        obj->NoSave = FALSE;
        act( "$p不再不可儲存﹗", ch, obj, NULL, TO_CHAR );
      }
      else
      {
        obj->NoSave = TRUE;
        act( "$p又恢復不可儲存性質﹗", ch, obj, NULL, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "contraband" ) )
    {
      if ( obj->Contraband )
      {
        obj->Contraband = FALSE;
        act( "$p不再具有違禁品性質﹗", ch, obj, NULL, TO_CHAR );
      }
      else
      {
        obj->Contraband = TRUE;
        act( "$p又恢復違禁品性質﹗", ch, obj, NULL, TO_CHAR );
      }
    }

    else
    {
      send_to_char(
        "物品的旗標只有 glow hum dark evil invis magic nodrop\n\r"
        "               bless antigood antievil antineutral noremove\n\r"
        "               nosave contraband\n\r", ch );
    }
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "armor" ) )
  {
    obj->armor = value;
    act( "你設定$p的目前防護為 $X。", ch, obj, &value, TO_CHAR );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "maxarmor" ) )
  {
    obj->max_armor = value;
    act( "你設定$p的最大防護為 $X。", ch, obj, &value, TO_CHAR );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "level" ) )
  {
    obj->level = value;
    act( "你設定$p的等級為 $X。", ch, obj, &value, TO_CHAR );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "weight" ) )
  {
    obj->weight = value;

    act( "你設定$p的重量為 $X。", ch, obj, &value, TO_CHAR );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "cost" ) )
  {
    obj->cost = value;
    act( "設定$p的價值為 $X。", ch, obj, &value, TO_CHAR );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "timer" ) )
  {
    obj->timer = value;
    act( "你設定$p的時效為 $X。", ch, obj, &value, TO_CHAR );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "name" ) )
  {
    smash_tilde( arg3 );
    act( "你設定$p的名稱為 $T。", ch, obj, arg3, TO_CHAR );
    free_string( obj->name );
    obj->name = str_dup( arg3 );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "short" ) )
  {
    smash_tilde( arg3 );
    act( "設定$p的短敘述為 $T。", ch, obj, arg3, TO_CHAR );
    free_string( obj->cname );
    obj->cname = str_dup( arg3 );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "long" ) )
  {
    smash_tilde( arg3 );
    act( "設定$p的長敘述為 $T。", ch, obj, arg3, TO_CHAR );
    free_string( obj->description );
    obj->description = str_dup( arg3 );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "ed" ) )
  {
    EXTRA_DESCR_DATA * ed;

    argument = one_argument( argument, arg3 );

    if ( !argument )
    {
      send_to_char( "語法﹕oset <物品> ed <關鍵字> <字串>。\n\r" , ch );
      RETURN_NULL();
    }

    /* 配置記憶體 */
    ed = alloc_struct( STRUCT_EXTRA_DESCR_DATA );

    smash_tilde( arg3     );
    smash_tilde( argument );

    ed->keyword      = str_dup( arg3     );
    ed->description  = str_dup( argument );
    ed->next         = obj->extra_descr;
    obj->extra_descr = ed;

    sprintf( buf, "%s %s", arg3, argument );
    act( "你設定$p的額外描述為 $T。", ch, obj, buf, TO_CHAR );
    RETURN_NULL();
  }

  /* 如果選項錯誤, 輸出錯誤的畫面 */
  do_oset( ch, "" );
  RETURN_NULL();
}

FUNCTION( do_rset )
{
  ROOM_INDEX_DATA * location;
  char              arg1[MAX_INPUT_LENGTH];
  char              arg2[MAX_INPUT_LENGTH];
  char              arg3[MAX_INPUT_LENGTH];
  int               tick;

  PUSH_FUNCTION( "do_rset" );

  smash_tilde( argument );
  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );
  str_cpy( arg3, argument );

  if ( arg1[0] == '\x0' || arg2[0] == '\x0' )
  {
    send_to_char( "語法﹕rset <位置> <項目> <數值>。\n\r\n\r"
                  "項目可以是下列選擇﹕\n\r "
                  "writeable dark nomob indoors private safe\n\r"
                  "foreverlight norecall noquit nofight stock\n\r"
                  "depositmoney storeroom memorize nowhere fane\n\r"
                  "killer standard waterful foodful manacure\n\r"
                  "movecure hitcure mobreborn practice\n\r", ch );
    RETURN_NULL();
  }

  if ( !( location = find_location( ch, arg1 ) ) )
  {
    send_to_char( "找不到你要改變的位置。\n\r", ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "writeable" ) )
  {
    if ( location->writeable )
    {
      location->writeable = FALSE;
      send_to_char( "你把這間房間可寫入旗標清除﹗\n\r", ch );
    }

    else
    {
      location->writeable = TRUE;
      send_to_char( "你設定這間房間可寫入旗標﹗\n\r", ch );
    }

    if ( location->player ) write_new_room( location );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "dark" ) )
  {
    if ( location->Dark )
    {
      location->Dark = FALSE;
      send_to_char( "你把這間房間黑暗旗標清除﹗\n\r", ch );
    }

    else
    {
      location->Dark = TRUE;
      send_to_char( "你設定這間房間黑暗旗標﹗\n\r", ch );
    }

    if ( location->player ) write_new_room( location );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "nomob" ) )
  {
    if ( location->NoMob )
    {
      location->NoMob = FALSE;
      send_to_char( "你把這間房間無生物旗標清除﹗\n\r", ch );
    }

    else
    {
      location->NoMob = TRUE;
      send_to_char( "你設定這間房間無生物旗標﹗\n\r", ch );
    }

    if ( location->player ) write_new_room( location );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "waterful" ) )
  {
    if ( location->Waterful )
    {
      location->Waterful = FALSE;
      send_to_char( "你把這間房間多水旗標清除﹗\n\r", ch );
    }

    else
    {
      location->Waterful = TRUE;
      send_to_char( "你設定這間房間多水旗標﹗\n\r", ch );
    }

    if ( location->player ) write_new_room( location );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "manacure" ) )
  {
    if ( location->ManaCure )
    {
      location->ManaCure = FALSE;
      send_to_char( "你把這間房間法力診療所旗標清除﹗\n\r", ch );
    }

    else
    {
      location->ManaCure = TRUE;
      send_to_char( "你設定這間房間法力診療所旗標﹗\n\r", ch );
    }

    if ( location->player ) write_new_room( location );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "movecure" ) )
  {
    if ( location->MoveCure )
    {
      location->MoveCure = FALSE;
      send_to_char( "你把這間房間體力診療所旗標清除﹗\n\r", ch );
    }

    else
    {
      location->MoveCure = TRUE;
      send_to_char( "你設定這間房間體力診療所旗標﹗\n\r", ch );
    }

    if ( location->player ) write_new_room( location );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "hitcure" ) )
  {
    if ( location->HitCure )
    {
      location->HitCure = FALSE;
      send_to_char( "你把這間房間生命力診療所旗標清除﹗\n\r", ch );
    }

    else
    {
      location->HitCure = TRUE;
      send_to_char( "你設定這間房間生命力診療所旗標﹗\n\r", ch );
    }

    if ( location->player ) write_new_room( location );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "practice" ) )
  {
    if ( location->PracRoom )
    {
      location->PracRoom = FALSE;
      send_to_char( "你把這間房間練功\場旗標清除﹗\n\r", ch );
    }

    else
    {
      location->PracRoom = TRUE;
      send_to_char( "你設定這間房間練功\場旗標﹗\n\r", ch );
    }

    if ( location->player ) write_new_room( location );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "foodful" ) )
  {
    if ( location->Foodful )
    {
      location->Foodful = FALSE;
      send_to_char( "你把這間房間多食物旗標清除﹗\n\r", ch );
    }

    else
    {
      location->Foodful = TRUE;
      send_to_char( "你設定這間房間多食物旗標﹗\n\r", ch );
    }

    if ( location->player ) write_new_room( location );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "indoors" ) )
  {
    if ( location->Indoors )
    {
      location->Indoors = FALSE;
      send_to_char( "你把這間房間室內物旗標清除﹗\n\r", ch );
    }

    else
    {
      location->Indoors = TRUE;
      send_to_char( "你設定這間房間室內旗標﹗\n\r", ch );
    }

    if ( location->player ) write_new_room( location );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "private" ) )
  {
    if ( location->Private )
    {
      location->Private = FALSE;
      send_to_char( "你把這間房間私人旗標清除﹗\n\r", ch );
    }

    else
    {
      location->Private = TRUE;
      send_to_char( "你設定這間房間私人旗標﹗\n\r", ch );
    }

    if ( location->player ) write_new_room( location );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "safe" ) )
  {
    if ( location->Safe )
    {
      location->Safe = FALSE;
      send_to_char( "你把這間房間安全旗標清除﹗\n\r", ch );
    }

    else
    {
      location->Safe = TRUE;
      send_to_char( "你設定這間房間安全旗標﹗\n\r", ch );
    }

    if ( location->player ) write_new_room( location );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "foreverlight" ) )
  {
    if ( location->ForeverLight )
    {
      location->ForeverLight = FALSE;
      send_to_char( "你把這間房間永明旗標清除﹗\n\r", ch );
    }

    else
    {
      location->ForeverLight = TRUE;
      send_to_char( "你設定這間房間永明旗標﹗\n\r", ch );
    }

    if ( location->player ) write_new_room( location );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "norecall" ) )
  {
    if ( location->NoRecall )
    {
      location->NoRecall = FALSE;
      send_to_char( "你把這間房間不能傳送旗標清除﹗\n\r", ch );
    }

    else
    {
      location->NoRecall = TRUE;
      send_to_char( "你設定這間房間不能傳送旗標﹗\n\r", ch );
    }

    if ( location->player ) write_new_room( location );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "depositmoney" ) )
  {
    if ( location->DepositMoney )
    {
      location->DepositMoney = FALSE;
      send_to_char( "你把這間房間錢莊旗標清除﹗\n\r", ch );
    }

    else
    {
      location->DepositMoney = TRUE;
      send_to_char( "你設定這間房間錢莊旗標﹗\n\r", ch );
    }

    if ( location->player ) write_new_room( location );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "storeroom" ) )
  {
    if ( location->StoreRoom )
    {
      location->StoreRoom = FALSE;
      send_to_char( "你把這間房間鑣局旗標清除﹗\n\r", ch );
    }

    else
    {
      location->StoreRoom = TRUE;
      send_to_char( "你設定這間房間鑣局旗標﹗\n\r", ch );
    }

    if ( location->player ) write_new_room( location );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "nofight" ) )
  {
    if ( location->NoFight )
    {
      location->NoFight = FALSE;
      send_to_char( "你把這間房間不能戰鬥旗標清除﹗\n\r", ch );
    }

    else
    {
      location->NoFight = TRUE;
      send_to_char( "你設定這間房間不能戰鬥旗標﹗\n\r", ch );
    }

    if ( location->player ) write_new_room( location );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "noquit" ) )
  {
    if ( location->NoQuit )
    {
      location->NoQuit = FALSE;
      send_to_char( "你把這間房間不能離開旗標清除﹗\n\r", ch );
    }

    else
    {
      location->NoQuit = TRUE;
      send_to_char( "你設定這間房間不能離開旗標﹗\n\r", ch );
    }

    if ( location->player ) write_new_room( location );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "memorize" ) )
  {
    if ( location->Memorize )
    {
      location->Memorize = FALSE;
      send_to_char( "你把這間房間記憶點旗標清除﹗\n\r", ch );
    }

    else
    {
      location->Memorize = TRUE;
      send_to_char( "你設定這間房間記憶點旗標﹗\n\r", ch );
    }

    if ( location->player ) write_new_room( location );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "stock" ) )
  {
    if ( location->Stock )
    {
      location->Stock = FALSE;
      send_to_char( "你把這間房間票據所旗標清除﹗\n\r", ch );
    }

    else
    {
      location->Stock = TRUE;
      send_to_char( "你設定這間房間票據所旗標﹗\n\r", ch );
    }

    if ( location->player ) write_new_room( location );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "nowhere" ) )
  {
    if ( location->NoWhere )
    {
      location->NoWhere = FALSE;
      send_to_char( "你把這間房間不能打聽旗標清除﹗\n\r", ch );
    }

    else
    {
      location->NoWhere = TRUE;
      send_to_char( "你設定這間房間不能打聽旗標﹗\n\r", ch );
    }

    if ( location->player ) write_new_room( location );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "fane" ) )
  {
    if ( location->Fane )
    {
      location->Fane = FALSE;
      send_to_char( "你把這間房間寺廟旗標清除﹗\n\r", ch );
    }

    else
    {
      location->Fane = TRUE;
      send_to_char( "你設定這間房間寺廟旗標﹗\n\r", ch );
    }

    if ( location->player ) write_new_room( location );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "killer" ) )
  {
    if ( location->Killer )
    {
      location->Killer = FALSE;
      send_to_char( "你把這間房間八仙飯店旗標清除﹗\n\r", ch );
    }

    else
    {
      location->Killer = TRUE;
      send_to_char( "你設定這間房間八仙飯店旗標﹗\n\r", ch );
    }

    if ( location->player ) write_new_room( location );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "standard" ) )
  {
    if ( location->Standard )
    {
      location->Standard = FALSE;
      send_to_char( "你把這間房間標準旗標清除﹗\n\r", ch );
    }

    else
    {
      location->Standard = TRUE;
      send_to_char( "你設定這間房間標準旗標﹗\n\r", ch );
    }

    if ( location->player ) write_new_room( location );
    RETURN_NULL();
  }

  if ( !str_prefix( arg2, "mobreborn" ) )
  {
    if ( arg3[0] == '\x0' || !is_number( arg3 ) )
    {
      send_to_char( "你要把這間房間怪物重生時間改為多少﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( ( tick = atoi( arg3 ) ) < 0 )
    {
      send_to_char( "你輸入的時間必須大於零﹗\n\r", ch );
      RETURN_NULL();
    }

    location->mob_reborn = tick;
    act( "你把$t的怪物重生時間改為$X﹗", ch, location->name, &tick, TO_CHAR );
    RETURN_NULL();
  }

  /* 如果選項錯誤, 輸出求助畫面 */
  do_rset( ch, "" );

  RETURN_NULL();
}

FUNCTION( do_users )
{
  DESCRIPTOR_DATA * d;
  DESCRIPTOR_DATA * man = NULL;
  CHAR_DATA       * victim;
  char              arg[MAX_INPUT_LENGTH];
  char              buf[MAX_STRING_LENGTH];
  int               count;
  int               port = ERRORCODE;

  PUSH_FUNCTION( "do_users" );

  argument = one_argument( argument, arg );

  if ( arg[0] )
  {
    if ( !str_prefix( arg, "!port" ) )
    {
      one_argument( argument, arg );

      if ( !arg[0] || !is_number( arg ) )
      {
        send_to_char( "你要觀看你一個連接埠的狀況﹖\n\r", ch );
        RETURN_NULL();
      }

      for ( port = atoi( arg ), count = 0; count < MAX_PORT; count++ )
        if ( MudPort[count] != ERRORCODE && MudPort[count] == port ) break;

      if ( count == MAX_PORT )
      {
        act( "$t沒有那個連接埠 $X﹗", ch, mud_name, &port, TO_CHAR );
        RETURN_NULL();
      }
    }

    else
    {
      if ( !( victim = get_char_world( ch, arg ) ) )
      {
        send_to_char( "你的目標不在這裡。\n\r", ch );
        RETURN_NULL();
      }

      if ( IS_NPC( victim ) )
      {
        act( "你的目標$N必須是玩家才可以。", ch, NULL, victim, TO_CHAR );
        RETURN_NULL();
      }

      if ( !( man = victim->desc ) )
      {
        act( "$N離線﹐沒有描述表。", ch, NULL, victim, TO_CHAR );
        RETURN_NULL();
      }
    }
  }

  send_to_char( "\e[1;33;44m[行程( 遊戲狀態) ] 停留時間 閒置 位 址 工 "
    "玩 家 名 稱  連    線    位    址    \e[0m\n\r" , ch );

  for ( count = 0, clear_buffer(), d = descriptor_list; d; d = d->next )
  {
    if ( !verify_desc( d )
      || ( port != ERRORCODE && d->port != port )
      || ( man && d != man ) )
      continue;

    if ( d->character
      && d->connected == CON_PLAYING
      && IS_SET( d->character->act, PLR_WIZINVIS )
      && get_trust( d->character ) > get_trust( ch ) ) continue;

    count++;
    str_cpy( buf, host_name( d ) );
    buf[25] = '\x0';

    send_to_buffer( "\e[1;37m[%3d(%10s) ] \e[1;33m%s\e[0m"
                    " \e[1;32m%4d\e[0m \e[1;37m%5d\e[0m \e[1;36m%s\e[0m "
                    "\e[1;31m%-12.12s\e[0m \e[1;31m%s\e[0m\n\r"
      , d->descriptor , player_status( d->connected )
      , showtime( current_time - d->timekeeper )
      , d->character ? d->character->timer : -1
      , d->port
      , CheckServer ? YESNO( d->server ) : "﹖"
      , d->character ? d->character->name : "(未詳)"
      , buf );

    if ( buffer_full() ) break;
  }

  if ( !man || port != ERRORCODE )
  {
    send_to_buffer( "總共有 %d 位使用者在%s。\n\r", count, mud_name );
    for ( count = 0, victim = char_list; victim; victim = victim->next )
    {
      if ( verify_char( victim ) && !IS_NPC( victim ) && !victim->desc )
      {
        if ( count == 0 )
          send_to_buffer( "\n\r另外目前不正常斷線的玩家有﹕\n\r" );

        send_to_buffer( "%2d. 名字﹕%-12s %s\e[0m「斷線總時間﹕%d」\n\r"
         , ++count, victim->name, victim->cname, victim->timer );
      }
    }
  }

  print_buffer( ch );
  RETURN_NULL();
}

FUNCTION( do_force )
{
  CMD_DATA  * pCommand;
  CHAR_DATA * vch;
  char        arg[MAX_INPUT_LENGTH];
  int         trust;

  PUSH_FUNCTION( "do_force" );

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' || argument[0] == '\x0' )
  {
    send_to_char( "你想強制誰做什麼事﹖\n\r", ch );
    RETURN_NULL();
  }

  /* 在指令表格中找符合的指令, 200 表示是怪物的指令 */
  trust = get_trust( ch );
  for ( pCommand = cmd_list; pCommand; pCommand = pCommand->next )
  {
    if ( !str_prefix( argument , pCommand->name )
      && ( pCommand->level > trust && pCommand->level != 200 ) )
    {
      send_to_char( "這個指令連你自己都不會做耶﹗\n\r", ch );
      RETURN_NULL();
    }
  }

  if ( !str_cmp( arg, "all" ) )
  {
    for ( vch = char_list; vch; vch = vch->next )
    {
      if ( !verify_char( vch ) ) continue;

      if ( !IS_NPC( vch ) && get_trust( vch ) < get_trust( ch ) )
      {
        MOBtrigger = FALSE;
        act( "$n強制你 $t。", ch, argument, vch, TO_VICT );
        interpret( vch, argument );
      }
    }
  }
  else
  {
    if ( !( vch = get_char_world( ch, arg ) ) )
    {
      send_to_char( "你的對象不在這裡。\n\r", ch );
      RETURN_NULL();
    }

    if ( vch == ch )
    {
      send_to_char( "強制自己﹐不如趕快自己去做吧﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( get_trust( vch ) >= get_trust( ch ) )
    {
      act( "你的等級比$N低﹐所以失敗了﹗", ch, NULL, vch, TO_CHAR );
      RETURN_NULL();
    }

    MOBtrigger = FALSE;
    act( "$n強制你 $t。", ch, argument, vch, TO_VICT );
    interpret( vch, argument );
  }

  send_to_char( "FORCE 動作執行完畢。\n\r", ch );
  RETURN_NULL();
}

FUNCTION( do_invis )
{
  PUSH_FUNCTION( "do_invis" );

  if ( IS_NPC(ch) ) RETURN_NULL();

  if ( IS_SET(ch->act, PLR_WIZINVIS) )
  {
    REMOVE_BIT( ch->act, PLR_WIZINVIS );
    act( "在一陣強光包圍之下﹐$n逐漸現出他的身影。", ch, NULL, NULL, TO_ROOM );
    send_to_char( "在一陣強光包圍之下﹐你逐漸現出你的身影。\n\r", ch );
  }
  else
  {
    SET_BIT( ch->act, PLR_WIZINVIS );
    act( "在一片紫氣籠罩之下﹐$n身影逐漸消失了。" , ch, NULL, NULL, TO_ROOM );
    send_to_char( "在一片紫氣籠罩之下﹐你的身影逐漸消失了。\n\r", ch );
  }

  RETURN_NULL();
}

FUNCTION( do_nodeath )
{
  PUSH_FUNCTION( "do_nodeath" );

  if ( IS_NPC(ch) ) RETURN_NULL();

  if ( IS_SET( ch->act, PLR_NODEATH ) )
  {
    REMOVE_BIT( ch->act, PLR_NODEATH );
    send_to_char( "閻羅王又隨侍在你身旁了。\n\r", ch );
  }
  else
  {
    SET_BIT(ch->act, PLR_NODEATH );
    send_to_char( "閻羅王離你遠去了。\n\r", ch );
  }

  RETURN_NULL();
}

FUNCTION( do_holylight )
{
  PUSH_FUNCTION( "do_holylight" );

  if ( IS_NPC(ch) ) RETURN_NULL();

  if ( IS_SET(ch->act, PLR_HOLYLIGHT) )
  {
    REMOVE_BIT(ch->act, PLR_HOLYLIGHT);
    send_to_char( "你的神聖光芒逐漸消失不見了。\n\r", ch );
  }
  else
  {
    SET_BIT(ch->act, PLR_HOLYLIGHT);
    send_to_char( "你全身被神聖光芒所籠罩著。\n\r", ch );
  }

  RETURN_NULL();
}

FUNCTION( do_owhere )
{
  char       arg[ MAX_INPUT_LENGTH ];
  OBJ_DATA * obj;
  int        counter;
  bool       fAll = FALSE;

  PUSH_FUNCTION( "do_owhere" );

  if ( IS_NPC( ch ) ) RETURN_NULL();
  one_argument( argument, arg );

  if ( !str_cmp( arg , "all" ) ) fAll = TRUE;
  clear_buffer();

  for ( counter = 1, obj = object_list; obj ; obj = obj->next )
  {
    if ( !verify_obj( obj ) ) continue;

    if ( (!is_name( arg,obj->name ) || !can_see_obj( ch ,obj )) && !fAll )
      continue;

    send_to_buffer( "[%3d] %s\e[0m %s\e[0m\n\r"
                    , counter++
                    , obj_name( ch, obj )
                    , object_locate( obj , ch ) );

   if ( buffer_full() ) break;
  }

  if ( counter == 1 ) send_to_buffer( "找不到你要的東西。\n\r" );
  print_buffer( ch );
  RETURN_NULL();
}

FUNCTION( do_mlevel )
{
  MOB_INDEX_DATA * pMobIndex;
  extern int       top_mob_index;
  char             arg1[MAX_INPUT_LENGTH];
  char             arg2[MAX_INPUT_LENGTH];
  int              up;
  int              down;
  int              vnum;
  int              nMatch;
  bool             found;

  PUSH_FUNCTION( "do_mlevel" );

  argument = one_argument( argument, arg1 );
  one_argument( argument , arg2 );

  if ( arg1[0] == '\x0' || !is_number( arg1 ) )
  {
    send_to_char( "語法﹕mlevel <等級> <等級>。\n\r" , ch );
    RETURN_NULL();
  }

  down = atoi( arg1 );

  if ( arg2[0] == '\x0')
  {
    up = down;
  }
  else
  {
    if ( !is_number( arg2 ) )
    {
      do_mlevel( ch , "" );
      RETURN_NULL();
    }
    else
    {
      up = atoi( arg2 );
    }
  }

  if ( down > up )
  {
    down = down - up ;
    up   = down + up ;
    down = up - down ;
  }

  clear_buffer();
  for ( found = FALSE, nMatch = vnum = 0; nMatch < top_mob_index; vnum++ )
  {
    if ( ( pMobIndex = get_mob_index( vnum ) ) )
    {
      nMatch++;
      if ( pMobIndex->level >= down && pMobIndex->level <= up )
      {
        found = TRUE;
        send_to_buffer( "\e[1;32m號碼\e[0m%6d "
                        "\e[1;33m等級\e[0m%4d "
                         "\e[1;34m被殺\e[0m%4d "
                         "\e[1;36m數量\e[0m%4d "
                         "\e[1;35m描述\e[0m %s\n\r",
          pMobIndex->vnum,
          pMobIndex->level,
          pMobIndex->killed,
          pMobIndex->count,
          mob_index_name( NULL, pMobIndex ) );

        if ( buffer_full() ) break;
      }
    }
  }

  if ( !found ) send_to_buffer( "找不到你指定的對象。\n\r" );
  print_buffer( ch );
  RETURN_NULL();
}

FUNCTION( do_olevel )
{
  OBJ_INDEX_DATA * pObjIndex;
  extern int       top_obj_index;
  char             arg1[MAX_INPUT_LENGTH];
  char             arg2[MAX_INPUT_LENGTH];
  int              up;
  int              down;
  int              vnum;
  int              nMatch;
  bool             found;

  PUSH_FUNCTION( "do_olevel" );

  argument = one_argument( argument, arg1 );
  one_argument( argument , arg2 );

  if ( arg1[0] == '\x0' || !is_number( arg1 ) )
  {
    send_to_char( "語法﹕olevel <等級> <等級>。\n\r" , ch );
    RETURN_NULL();
  }

  down = atoi( arg1 );

  if ( arg2[0] == '\x0')
  {
    up = down;
  }

  else
  {
    if ( !is_number( arg2 ) )
    {
      do_olevel( ch , "" );
      RETURN_NULL();
    }
    else
    {
      up = atoi( arg2 );
    }
  }

  if ( down > up )
  {
    down = down - up ;
    up   = down + up ;
    down = up - down ;
  }

  clear_buffer();
  for ( found = FALSE, nMatch = vnum = 0; nMatch < top_obj_index; vnum++ )
  {
    if ( ( pObjIndex = get_obj_index( vnum ) ) )
    {
      nMatch++;
      if ( pObjIndex->level >= down && pObjIndex->level <= up )
      {
        found = TRUE;
        send_to_buffer( "\e[1;32m號碼\e[0m%6d "
                        "\e[1;33m等級\e[0m%4d "
                        "\e[1;34m數量\e[0m%4d "
                        "\e[1;35m描述\e[0m %s(%s)\n\r",
          pObjIndex->vnum,
          pObjIndex->level,
          pObjIndex->count,
          pObjIndex->short_descr,
          pObjIndex->name );

        if ( buffer_full() ) break;
      }
    }
  }

  if ( !found ) send_to_buffer( "找不到你指定的物品。\n\r" );
  print_buffer( ch );
  RETURN_NULL();
}

FUNCTION( do_oaf )
{
  OBJ_INDEX_DATA * pObjIndex;
  AFFECT_DATA    * paf;
  extern int       top_obj_index;
  char             arg[MAX_INPUT_LENGTH];
  int              vnum;
  int              nMatch;
  int              field;
  bool             found;

  PUSH_FUNCTION( "do_oaf" );

  one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    send_to_char( "語法﹕oaf <查詢字串>\n\r\n\r"
                  "<查詢字串>﹕str int wis dex con sex class level\n\r"
                  "            age height weight mana hp move gold\n\r"
                  "            exp ac hitroll damroll saving_para\n\r"
                  "            saving_rod saving_petri saving_breath\n\r"
                  "            saving_spell\n\r" , ch );

    RETURN_NULL();
  }

  field = 0;
  if ( !str_prefix( arg , "str"           ) ) field = APPLY_STR          ;
  if ( !str_prefix( arg , "int"           ) ) field = APPLY_INT          ;
  if ( !str_prefix( arg , "wis"           ) ) field = APPLY_WIS          ;
  if ( !str_prefix( arg , "dex"           ) ) field = APPLY_DEX          ;
  if ( !str_prefix( arg , "con"           ) ) field = APPLY_CON          ;
  if ( !str_prefix( arg , "sex"           ) ) field = APPLY_SEX          ;
  if ( !str_prefix( arg , "class"         ) ) field = APPLY_CLASS        ;
  if ( !str_prefix( arg , "level"         ) ) field = APPLY_LEVEL        ;
  if ( !str_prefix( arg , "age"           ) ) field = APPLY_AGE          ;
  if ( !str_prefix( arg , "height"        ) ) field = APPLY_HEIGHT       ;
  if ( !str_prefix( arg , "weight"        ) ) field = APPLY_WEIGHT       ;
  if ( !str_prefix( arg , "mana"          ) ) field = APPLY_MANA         ;
  if ( !str_prefix( arg , "hp"            ) ) field = APPLY_HIT          ;
  if ( !str_prefix( arg , "move"          ) ) field = APPLY_MOVE         ;
  if ( !str_prefix( arg , "gold"          ) ) field = APPLY_GOLD         ;
  if ( !str_prefix( arg , "exp"           ) ) field = APPLY_EXP          ;
  if ( !str_prefix( arg , "ac"            ) ) field = APPLY_AC           ;
  if ( !str_prefix( arg , "hitroll"       ) ) field = APPLY_HITROLL      ;
  if ( !str_prefix( arg , "damroll"       ) ) field = APPLY_DAMROLL      ;
  if ( !str_prefix( arg , "saving_para"   ) ) field = APPLY_SAVING_PARA  ;
  if ( !str_prefix( arg , "saving_rod"    ) ) field = APPLY_SAVING_ROD   ;
  if ( !str_prefix( arg , "saving_petri"  ) ) field = APPLY_SAVING_PETRI ;
  if ( !str_prefix( arg , "saving_breath" ) ) field = APPLY_SAVING_BREATH;
  if ( !str_prefix( arg , "saving_spell"  ) ) field = APPLY_SAVING_SPELL ;

  if ( field == 0 )
  {
    do_oaf( ch , "" );
    RETURN_NULL();
  }

  clear_buffer();
  for ( nMatch = vnum = 0, found = FALSE; nMatch < top_obj_index; vnum++ )
  {
    if ( ( pObjIndex = get_obj_index( vnum ) ) )
    {
      nMatch++;
      for ( paf = pObjIndex->affected; paf; paf = paf->next )
      {
        if ( paf->location == field )
        {
          found = TRUE;
          send_to_buffer( "[\e[1;32m%5d\e[0m] "
                          "(\e[1;34m影響\e[0m﹕\e[1;32m%5d\e[0m) "
                          "(\e[1;35m等級\e[0m﹕\e[1;32m%4d\e[0m) "
                          "%s(%s)。\n\r",
            pObjIndex->vnum,
            paf->modifier,
            pObjIndex->level,
            pObjIndex->short_descr,
            pObjIndex->name );

          if ( buffer_full() ) break;
        }
      }
    }
  }

  if ( !found ) send_to_buffer( "並未找到加這種屬性的物品。\n\r" );
  print_buffer( ch );
  RETURN_NULL();
}

FUNCTION( do_see )
{
  int         count;
  char        arg[ MAX_INPUT_LENGTH ];
  OBJ_DATA *  obj;
  OBJ_DATA  * in_obj;
  CHAR_DATA * victim;
  bool        see_carrying = TRUE;
  bool        see_deposit  = TRUE;

  PUSH_FUNCTION( "do_see" );

  if ( !ch->desc ) RETURN_NULL();
  argument = one_argument( argument, arg );

  /* 只觀看人物身上的物品, 而不觀看寄放的物品 */
  if ( arg[0] == '!' )
  {
    see_carrying = TRUE;
    see_deposit  = FALSE;
    str_cpy( arg , arg+1 );
  }

  /* 只觀看人物寄放的物品, 而不觀看身上的物品 */
  if ( arg[0] == '@' && see_deposit )
  {
    see_carrying = FALSE;
    see_deposit  = TRUE;
    str_cpy( arg , arg+1 );
  }

  if ( !( victim = get_char_world( ch, arg ) )  )
  {
    send_to_char( "找不到你所要觀看的對象。\n\r", ch );
    RETURN_NULL();
  }

  clear_buffer();

  if ( ch != victim )
    send_to_buffer( "你觀看所有屬於%s的物品﹕\n\r\n\r", mob_name( ch, victim ) );

  for ( count = 1, obj = object_list; obj ; obj = obj->next )
  {
    if ( !verify_obj( obj ) ) continue;

    for ( in_obj = obj; in_obj->in_obj ; in_obj = in_obj->in_obj);

    if ( in_obj->carried_by == victim && see_carrying )
    {
      send_to_buffer( "身上[%3d] %s 虛擬號碼 %d 流水號碼 %d-%d\n\r"
        , count++
        , obj_name( ch, obj )
        , obj->pIndexData->vnum
        , obj->serial.high
        , obj->serial.low );

      if ( buffer_full() ) break;
    }

    if ( in_obj->deposit_by == victim && see_deposit )
    {
      send_to_buffer( "寄放[%3d] %s 虛擬號碼 %d 流水號碼 %d-%d\n\r"
        , count++
        , obj_name( ch, obj )
        , obj->pIndexData->vnum
        , obj->serial.high
        , obj->serial.low );

      if ( buffer_full() ) break;
    }
  }

  if ( count == 1 ) send_to_buffer( "沒有找到任何物品。\n\r" );
  print_buffer( ch );
  RETURN_NULL();
}

FUNCTION( do_otype )
{
  extern int       top_obj_index;
  OBJ_INDEX_DATA * obj;
  int              count;
  int              vnum;
  int              nMatch;
  char             arg[MAX_INPUT_LENGTH];
  int              type = -1;

  PUSH_FUNCTION( "do_otype" );

  argument = one_argument( argument, arg );

       if ( arg[0] == '\x0'                )   type = -1;
  else if ( !str_prefix( arg, "light"      ) ) type = ITEM_LIGHT;
  else if ( !str_prefix( arg, "scroll"     ) ) type = ITEM_SCROLL;
  else if ( !str_prefix( arg, "wand"       ) ) type = ITEM_WAND;
  else if ( !str_prefix( arg, "staff"      ) ) type = ITEM_STAFF;
  else if ( !str_prefix( arg, "weapon"     ) ) type = ITEM_WEAPON;
  else if ( !str_prefix( arg, "treasure"   ) ) type = ITEM_TREASURE;
  else if ( !str_prefix( arg, "armor"      ) ) type = ITEM_ARMOR;
  else if ( !str_prefix( arg, "potion"     ) ) type = ITEM_POTION;
  else if ( !str_prefix( arg, "furniture"  ) ) type = ITEM_FURNITURE;
  else if ( !str_prefix( arg, "trash"      ) ) type = ITEM_TRASH;
  else if ( !str_prefix( arg, "container"  ) ) type = ITEM_CONTAINER;
  else if ( !str_prefix( arg, "drink_con"  ) ) type = ITEM_DRINK_CON;
  else if ( !str_prefix( arg, "key"        ) ) type = ITEM_KEY;
  else if ( !str_prefix( arg, "food"       ) ) type = ITEM_FOOD;
  else if ( !str_prefix( arg, "money"      ) ) type = ITEM_MONEY;
  else if ( !str_prefix( arg, "boat"       ) ) type = ITEM_BOAT;
  else if ( !str_prefix( arg, "corpse_npc" ) ) type = ITEM_CORPSE_NPC;
  else if ( !str_prefix( arg, "corpse_pc"  ) ) type = ITEM_CORPSE_PC;
  else if ( !str_prefix( arg, "fountain"   ) ) type = ITEM_FOUNTAIN;
  else if ( !str_prefix( arg, "pill"       ) ) type = ITEM_PILL;
  else if ( !str_prefix( arg, "magicstone" ) ) type = ITEM_MAGICSTONE;
  else if ( !str_prefix( arg, "spiritjade" ) ) type = ITEM_SPIRITJADE;
  else if ( !str_prefix( arg, "mystery"    ) ) type = ITEM_MYSTERY;
  else if ( !str_prefix( arg, "letter"     ) ) type = ITEM_LETTER;
  else if ( !str_prefix( arg, "ore"        ) ) type = ITEM_ORE;
  else if ( !str_prefix( arg, "goldmine"   ) ) type = ITEM_GOLDMINE;
  else if ( !str_prefix( arg, "sigil"      ) ) type = ITEM_SIGIL;
  else if ( !str_prefix( arg, "vellum"     ) ) type = ITEM_VELLUM;
  else if ( !str_prefix( arg, "firework"   ) ) type = ITEM_FIREWORK;
  else if ( !str_prefix( arg, "urn"        ) ) type = ITEM_URN;
  else if ( !str_prefix( arg, "backbag"    ) ) type = ITEM_BACKBAG;
  else if ( !str_prefix( arg, "arrow"      ) ) type = ITEM_ARROW;
  if ( type < 0 )
  {
    send_to_char( "你的關鍵字必須是下列的字串﹕\n\r\n\r"
      "    light       scroll      wand        staff       weapon\n\r"
      "    treasure    armor       potion      furniture   trash\n\r"
      "    container   drink_con   key         food        money\n\r"
      "    boat        corpse_npc  coprse_pc   fountain    pill\n\r"
      "    magicstone  spiritjade  mystery     letter      ore\n\r"
      "    goldmine    sigil       vellum      firework    urn\n\r"
      "    backbag     arrow\n\r\n\r", ch );

    RETURN_NULL();
  }

  clear_buffer();
  send_to_buffer( "所有有關%s型態的物品列表﹕\n\r", item_kind_name( type ) );

  for ( vnum = count = nMatch = 0; nMatch < top_obj_index; vnum++ )
  {
    if ( ( obj = get_obj_index( vnum ) ) )
    {
      nMatch++;
      if ( obj->item_type == type )
      {
        send_to_buffer( "%3d %5d %25s %s(%s)\n\r"
          , ++count, obj->vnum, obj->name
          , obj->short_descr, obj->name );

        if ( buffer_full() ) break;
      }
    }
  }

  if ( count == 0 ) send_to_buffer( "沒有發現有關上述型態的物品﹗\n\r" );
  print_buffer( ch );
  RETURN_NULL();
}

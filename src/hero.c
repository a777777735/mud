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
#include <time.h>
#include "merc.h"

void    hero_remove             args( ( HERO_DATA * ) );

FUNCTION( do_hero )
{
  HERO_DATA * pHero;
  CHAR_DATA * victim;
  time_t      timer;
  struct tm * sTime;
  char        arg[MAX_INPUT_LENGTH];
  char        timebuf[MAX_STRING_LENGTH];
  char        buf1[MAX_INPUT_LENGTH];
  char        buf2[MAX_INPUT_LENGTH];
  char        buf3[MAX_INPUT_LENGTH];
  char        buf4[MAX_INPUT_LENGTH];
  char        buf5[MAX_INPUT_LENGTH];
  int         loop;
  int         total;

  PUSH_FUNCTION( "do_hero" );

  argument = one_argument( argument , arg );

  if ( arg[0] == '\x0' || arg[0] == '*' )
  {
    if ( !hero_first )
    {
      act( "$t還沒有產生任何英雄耶﹗", ch, mud_name, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( arg[0] == '*' )
    {
      total = -MaxHeroShow;
    }
    else
    {
      for ( total = 0, pHero = hero_first; pHero; pHero = pHero->next ) total++;
    }

    clear_buffer();
    send_to_buffer( "\e[1;33;44m編號 英  文 名 稱 職  業 中    文    名    稱"
      " 成名時間                          \e[0m\n\r" );

    for ( loop = 0, pHero = hero_first; pHero; pHero = pHero->next, loop++ )
    {
      timer = pHero->timer;

      if ( loop < total - MaxHeroShow
        || !pHero->name
        || !*pHero->name
        || !pHero->cname
        || !*pHero->cname
        || !( sTime = localtime( &timer ) ) )
        continue;

      chinese_number( sTime->tm_year - 11 , buf1 );
      chinese_number( sTime->tm_mon  +  1 , buf2 );
      chinese_number( sTime->tm_mday      , buf3 );
      chinese_number( sTime->tm_min       , buf5 );
      chinese_number( sTime->tm_hour > 12
        ? sTime->tm_hour - 12 : sTime->tm_hour, buf4 );

      sprintf( timebuf, "%s年%s月%s號%s%s點%s分"
        , buf1, buf2, buf3, sTime->tm_hour < 12 ? "早上" : "下午", buf4, buf5 );

      timebuf[34] = '\x0';

      send_to_buffer( "%4d ", loop );
      send_to_buffer( "%-s "   , format_string( pHero->name , 12 ) );
      send_to_buffer( "%-s "   , format_string( pHero->class,  6 ) );
      send_to_buffer( "%-s "   , format_string( pHero->cname, 20 ) );
      send_to_buffer( "%s\n\r" , format_string( timebuf     , 34 ) );

      if ( buffer_full() ) break;
    }

    print_buffer( ch );
  }

  else if ( !str_prefix( arg, "!add" ) && IS_IMMORTAL( ch ) )
  {
    one_argument( argument, arg );

    if ( arg[0] == '\x0' )
    {
      send_to_char( "你要增加哪一位玩家為英雄呢﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( !( victim = get_char_world( ch, arg ) ) )
    {
      act( "找不到你要升格的人 $2$T$0。", ch, NULL, arg, TO_CHAR );
      RETURN_NULL();
    }

    if ( IS_NPC( victim ) )
    {
      act( "你不能把怪物$N升格為英雄。", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    for ( pHero = hero_first; pHero; pHero = pHero->next )
    {
      if ( !str_cmp( pHero->name, victim->name ) )
      {
        act( "$N已經在英雄榜裡了﹗", ch, NULL, victim, TO_CHAR );
        RETURN_NULL();
      }
    }

    chinese_number( hero_count() + 1, buf1 );
    sprintf( timebuf, "恭禧%s成為%s\e[0m第%s位英雄﹗\n\r"
      , mob_name( NULL, victim ), mud_name, buf1 );

    send_to_all_char( timebuf );

    act( "恭禧你成為$t$0第$T位英雄﹗", victim, mud_name, buf1, TO_CHAR );
    if ( ch != victim ) send_to_char( "增加英雄榜完成。\n\r", ch );

    /* 增加英雄 */
    hero_add( victim );

    /* 寫入檔案 */
    update_hero();
  }

  else if ( !str_prefix( arg, "!delete" ) && IS_IMMORTAL( ch ) )
  {
    one_argument( argument, arg );

    if ( arg[0] == '\x0' )
    {
      send_to_char( "你要把哪一位踢出英雄榜外﹖\n\r", ch );
      RETURN_NULL();
    }

    for ( pHero = hero_first; pHero; pHero = pHero->next )
      if ( !str_cmp( pHero->name, arg ) ) break;

    if ( !pHero )
    {
      act( "對不起﹐英雄榜內沒有 $2$T$0﹗", ch, NULL, arg, TO_CHAR );
      RETURN_NULL();
    }

    /* 刪除英雄 */
    hero_remove( pHero );

    /* 寫入檔案 */
    update_hero();

    send_to_char( "刪除英雄﹗\n\r", ch );

    for ( victim = char_list; victim; victim = victim->next )
    {
      if ( verify_char( victim )
        && !IS_NPC( victim )
        && !str_cmp( victim->name, arg ) )
      {
        send_to_char( "對不起﹐你被踢出英雄榜外了﹗\n\r", victim );
        break;
      }
    }
  }

  else
  {
    send_to_char( "語法錯誤﹐請查詢 \e[1;32mhero\e[0m 正確的用法。\n\r", ch );
  }
  RETURN_NULL();
}

size_t hero_count( void )
{
  HERO_DATA * pHero;
  size_t      count;

  PUSH_FUNCTION( "hero_count" );
  for ( count = 0, pHero = hero_first; pHero; pHero = pHero->next ) count++;
  RETURN( count );
}

/* 增加或是取代 */
void hero_add( CHAR_DATA * ch )
{
  HERO_DATA * pHero;

  PUSH_FUNCTION( "hero_add" );

  if ( !ch || IS_NPC( ch ) )
  {
    mudlog( LOG_DEBUG, "hero_add: 來源有錯誤." );
    RETURN_NULL();
  }

  /* 尋找是否已經有過了 */
  for ( pHero = hero_first; pHero; pHero = pHero->next )
    if ( !str_cmp( pHero->name, ch->name ) ) break;

  /* 有則進行取代 */
  if ( pHero )
  {
    free_string( pHero->name  );
    free_string( pHero->cname );
    free_string( pHero->class );

    pHero->name  = str_dup( ch->name  );
    pHero->cname = str_dup( ch->cname );
    pHero->class = str_dup( class_name( ch->class, FALSE ) );
    pHero->timer = time( NULL );
  }

  /* 不然配置一個新的 */
  else
  {
    /* 找出最後一個 */
    for ( pHero = hero_first; pHero; pHero = pHero->next ) hero_last = pHero;

    pHero        = alloc_struct( STRUCT_HERO_DATA );
    pHero->name  = str_dup( ch->name  );
    pHero->cname = str_dup( ch->cname );
    pHero->class = str_dup( class_name( ch->class, FALSE ) );
    pHero->timer = time( NULL );

    if ( !hero_first ) hero_first        = pHero;
    if (  hero_last  ) hero_last->next   = pHero;

    hero_last   = pHero;
    pHero->next = NULL;
  }

  RETURN_NULL();
}

/* 移除英雄資料 */
void hero_remove( HERO_DATA * pHero )
{
  HERO_DATA * aHero;
  HERO_DATA * zHero;

  PUSH_FUNCTION( "hero_remove" );

  if ( !pHero )
  {
    mudlog( LOG_DEBUG, "hero_remove: 來源錯誤." );
    RETURN_NULL();
  }

  for ( zHero = NULL, aHero = hero_first;
        aHero;
        zHero = aHero, aHero = aHero->next )
  {
    if ( aHero == pHero )
    {
      if ( !zHero ) hero_first  = aHero->next;
      else          zHero->next = aHero->next;

      free_string( pHero->name  );
      free_string( pHero->cname );
      free_string( pHero->class );

      free_struct( pHero , STRUCT_HERO_DATA );
      RETURN_NULL();
    }
  }

  mudlog( LOG_DEBUG, "hero_remove: 未發現到欲刪除的英雄." );
  RETURN_NULL();
}

/* 寫入檔案 */
void update_hero( void )
{
  HERO_DATA * pHero;
  FILE      * pFile;

  PUSH_FUNCTION( "update_hero" );

  if ( !( pFile = FOPEN( hero_file, "w" ) ) )
  {
    mudlog( LOG_DEBUG, "update_hero: 無法寫入英雄檔案%s.", hero_file );
    RETURN_NULL();
  }

  for ( pHero = hero_first; pHero; pHero = pHero->next )
  {
    fprintf( pFile, "Hero    %s~ %s~ %s~ %d\n"
      , pHero->name, pHero->cname, pHero->class, pHero->timer );
  }

  fprintf( pFile, "End" );
  FCLOSE( pFile );

  RETURN_NULL();
}

void check_hero( char * name )
{
  HERO_DATA * pHero;

  PUSH_FUNCTION( "check_hero" );

  if ( !name || !*name )
  {
    mudlog( LOG_DEBUG, "check_hero: 來源錯誤." );
    RETURN_NULL();
  }

  for ( pHero = hero_first; pHero; pHero = pHero->next )
  {
    if ( !str_cmp( pHero->name, name ) )
    {
      /* 刪除英雄 */
      hero_remove( pHero );

      /* 寫入檔案 */
      update_hero();

      RETURN_NULL();
    }
  }

  RETURN_NULL();
}

/* 增加屬性 */
void add_hero_bonus( CHAR_DATA * ch )
{
  int          loop;
  int          max;
  int          value;
  int          old;
  CLASS_DATA * pClass;
  char         buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "add_hero_bonus" );

  if ( !ch || !( pClass = ch->class ) )
  {
    mudlog( LOG_DEBUG, "add_hero_bonus: 來源錯誤." );
    RETURN_NULL();
  }

  if ( ch->level != LEVEL_HERO || IS_NPC( ch ) ) RETURN_NULL();

  if ( HeroFirman > 0 )
  {
    chinese_number( add_firman( ch, HeroFirman ), buf );
    act( "系統恭禧你升到英雄﹐所以送你$t張免死金牌。", ch, buf, NULL, TO_CHAR );
  }

  for ( loop = 0; loop < MAX_ATTR; loop++ )
  {
    if ( ( value = pClass->hero[loop] ) > 0 )
    {
      max = pClass->attr[loop];

      switch( loop )
      {
      case STR_ITEM:

        old         = ch->now_str;
        ch->now_str = UMIN( max, old + value );

        if ( ch->now_str > old )
          send_to_char( "恭禧你﹐你的力量變大了﹗\n\r", ch );

        break;

      case INT_ITEM:

        old         = ch->now_int;
        ch->now_int = UMIN( max, old + value );

        if ( ch->now_int > old )
          send_to_char( "恭禧你﹐你的智力增加了﹗\n\r", ch );

        break;

      case WIS_ITEM:

        old         = ch->now_wis;
        ch->now_wis = UMIN( max, old + value );

        if ( ch->now_wis > old )
          send_to_char( "恭禧你﹐你的學識增廣了﹗\n\r", ch );

        break;

      case DEX_ITEM:

        old         = ch->now_dex;
        ch->now_dex = UMIN( max, old + value );

        if ( ch->now_dex > old )
          send_to_char( "恭禧你﹐你的敏捷上揚了﹗\n\r", ch );

        break;

      case CON_ITEM:

        old         = ch->now_con;
        ch->now_con = UMIN( max, old + value );

        if ( ch->now_con > old )
          send_to_char( "恭禧你﹐你的體格變壯了﹗\n\r", ch );

        break;
      }
    }
  }

  RETURN_NULL();
}

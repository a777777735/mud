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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "merc.h"

/* 巨集最大可以有幾個變數 */
#define MAX_VARIABLE              4

/* 每一位玩家巨集最多可以有幾個 */
#define MAX_ALIAS_COUNT           30

/* 每一位玩家最多巨集可以重複幾次 */
#define MAX_ALIAS_REPEAT          15

int     max_alias       = MAX_ALIAS_COUNT;
int     alias_repeat    = MAX_ALIAS_REPEAT;

/* 區域函數 */
bool extract_an_alias           args( ( CHAR_DATA * , char * ) );
bool check_alias_legal          args( ( char * ) );
void alias_spooler_full         args( ( DESCRIPTOR_DATA * ) );
void expand_alias_spooler       args( ( DESCRIPTOR_DATA * ) );
extern bool write_to_descriptor args( ( DESCRIPTOR_DATA *, char * , int ) );

/* 巨集指令的核心 */
FUNCTION( do_alias )
{
  CHAR_DATA  * victim;
  ALIAS_DATA * pAlias;
  ALIAS_DATA * zAlias;
  char         arg[MAX_INPUT_LENGTH];
  int          count;

  PUSH_FUNCTION( "do_alias" );

  /* 如果不是玩家一定沒有巨集指令集 */
  if ( !ch || IS_NPC( ch ) ) RETURN_NULL();

  /* 把 ~ 這個符號給消除, 否則存檔之後讀檔會有問題 */
  smash_tilde( argument );

  /* 捉出引數 */
  argument = one_argument( argument, arg );
  for ( ; *argument == ' '; argument++ );

  /* 沒有引數就是觀看自己所有的巨集指令 */
  if ( !arg[0] || arg[0] == '!' )
  {
    if ( arg[0] == '\x0' )
    {
      victim = ch;
    }

    else if ( arg[1] == '\x0' )
    {
      send_to_char( "你要看誰的巨集指令呢﹖\n\r", ch );
      RETURN_NULL();
    }
    else
    {
      if ( !( victim = get_char_world( ch, arg + 1 ) ) )
      {
        act( "找不到這一號人物 $2$T$0 耶﹗", ch, NULL, arg+1, TO_CHAR );
        RETURN_NULL();
      }

      if ( IS_NPC( victim ) )
      {
        act( "$N是沒有巨集指令的﹗", ch, NULL, victim, TO_CHAR );
        RETURN_NULL();
      }

      if ( !biliwick( ch, victim ) )
      {
        act( "你的權力不夠看$N的巨集指令。", ch, NULL, victim, TO_CHAR );
        RETURN_NULL();
      }
    }

    clear_buffer();
    for ( count = 0, pAlias = victim->alias; pAlias; pAlias = pAlias->next )
    {
      count++;
      send_to_buffer( "%s把 {%s} 替代成 {%s}\n\r"
        , mob_name( ch, victim ), pAlias->text , pAlias->macro );

      if ( buffer_full() ) break;
    }

    if ( count == 0 )
    {
      send_to_buffer(
        "%s目前沒有任何的巨集指令\n\r如果想要知道如何使用巨集﹐"
        "請輸入 alias /?。\n\r", mob_name( victim, ch ) );
    }
    else
    {
      send_to_buffer( "\n\r%s總共有 %d 個巨集指令﹗\n\r"
        , mob_name( victim, ch ), count );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  /* 清除所有的巨集指令集 */
  if ( arg[0] == '-' )
  {
    extract_alias( ch );
    send_to_char( "你清除整個巨集指令定義。\n\r" , ch );
    RETURN_NULL();
  }

  /* 若只有引數一則是清除相同的巨集指令集 */
  if ( !argument[0] )
  {
    if ( extract_an_alias( ch , arg ) )
      act( "已經把 {$t} 移除了。", ch , arg, NULL, TO_CHAR );

    else
      act( "並未找到任何與 {$t} 相同的巨集。", ch, arg, NULL, TO_CHAR );

    RETURN_NULL();
  }

  /* 同時有引數一和引數二則是輸入巨集, 但是引數一和引數二不能相同 */
  if ( !str_cmp( arg , argument ) )
  {
    send_to_char( "指令和巨集相同會導致無窮回圈。\n\r" , ch );
    RETURN_NULL();
  }

  /* 檢查巨集指令集是否合法 */
  if ( !check_alias_legal( arg ) || arg[0] == '!' )
  {
    send_to_char( "不能使用指令當作來源命令巨集。\n\r" , ch );
    RETURN_NULL();
  }

  /* 檢查巨集是否有重複, 若重複則進行取代 */
  for ( pAlias = ch->alias ; pAlias; pAlias = pAlias->next )
  {
    if ( !str_cmp( pAlias->text, arg ) )
    {
      str_cpy( pAlias->macro , argument );
      act( "你把 {$t} 更改替代成 {$T}。"
        ,ch , pAlias->text , pAlias->macro, TO_CHAR );

      RETURN_NULL();
    }
  }

  /* 檢查玩家巨集指令集是否太多 */
  if ( ( get_alias_count( ch ) > max_alias ) && !IS_IMMORTAL( ch ) )
  {
    act( "你的巨集指令超過$i個﹐無法再增加。" , ch, &max_alias, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( str_len( arg      ) >= sizeof( arg ) - 1
    || str_len( argument ) >= sizeof( arg ) - 1 )
  {
    send_to_char( "你的巨集長度不合法。\n\r", ch );
    RETURN_NULL();
  }

  /* 配置巨集指令結構的記憶體 */
  pAlias = alloc_struct( STRUCT_ALIAS_DATA );

  str_cpy( pAlias->text , arg      );
  str_cpy( pAlias->macro, argument );
  pAlias->next = NULL;

  for ( zAlias = ch->alias; zAlias && zAlias->next; zAlias = zAlias->next );

  if ( !zAlias ) ch->alias    = pAlias;
  else           zAlias->next = pAlias;

  act( "你把 {$t} 替代成 {$T}。", ch, pAlias->text , pAlias->macro, TO_CHAR );
  RETURN_NULL();
}

/* 傳回某人物的巨集指令個數 */
int get_alias_count( CHAR_DATA * ch )
{
  ALIAS_DATA * pAlias;
  int          count;

  PUSH_FUNCTION( "get_alias_count" );

  if ( !ch || IS_NPC( ch ) ) RETURN( 0 );
  for ( pAlias = ch->alias, count = 0; pAlias; pAlias = pAlias->next ) count++;
  RETURN( count );
}

/* 銷毀所有的巨集 */
void extract_alias( CHAR_DATA * ch )
{
  ALIAS_DATA * pAlias;
  ALIAS_DATA * zAlias;

  PUSH_FUNCTION( "extract_alias" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG , "extract_alias: 銷毀巨集時, 人物是空的." );
    RETURN_NULL();
  }

  for ( pAlias = ch->alias; pAlias; pAlias = zAlias )
  {
    zAlias  = pAlias->next;
    free_struct( pAlias , STRUCT_ALIAS_DATA );
  }

  ch->alias = NULL;
  RETURN_NULL();
}

/* 銷毀單一的巨集 */
bool extract_an_alias( CHAR_DATA * ch , char * arg )
{
  ALIAS_DATA * temp;
  ALIAS_DATA * prev;

  PUSH_FUNCTION( "extact_an_alias" );

  if ( !ch || !arg || !*arg )
  {
    mudlog( LOG_DEBUG, "extract_an_alias: 缺乏來源." );
    RETURN( FALSE );
  }

  for ( prev = NULL, temp = ch->alias; temp; prev = temp, temp = temp->next )
  {
    if ( !str_cmp( temp->text , arg ) )
    {
      if ( !prev ) ch->alias  = ch->alias->next;
      else         prev->next = temp->next;

      free_struct( temp , STRUCT_ALIAS_DATA );
      RETURN( TRUE );
    }
  }

  RETURN( FALSE );
}

/* 把巨集指令翻譯成指令集 */
void translate_alias( DESCRIPTOR_DATA * d , char * input )
{
  ALIAS_DATA * pAlias;
  char         source[MAX_INPUT_LENGTH];
  char         source_command[ MAX_VARIABLE + 1 ][MAX_INPUT_LENGTH];
  char       * source_address;
  char       * target_address;
  char       * output_address;
  char       * really_address;
  int          loop;
  int          value;
  int          len_1;
  int          len_2;

  PUSH_FUNCTION( "translate_alias" );

  if ( !d || !d->character || d->connected != CON_PLAYING ) RETURN_NULL();

  output_address = really_address = input;
  input          = one_argument( input , source );

  for ( pAlias = d->character->alias; pAlias; pAlias = pAlias->next )
  {
    if ( !str_cmp( pAlias->text , source ) )
    {
      target_address = pAlias->macro;

      /* 捉出命令列中的引數 */
      for ( loop = 0; loop < ( MAX_VARIABLE + 1 ) ; loop++ )
      {
        source_address  = source_command[ loop ];
        *source_address = '\x0';
        input = one_argument( input , source_address );
      }

      while ( *target_address )
      {
        if ( *target_address == '%' )
        {
          value = * ( target_address + 1 ) - '0';

          if ( value > 0 && value <= MAX_VARIABLE )
          {
            len_1 = str_len( really_address );
            len_2 = str_len( source_command[ value - 1 ] );

            if ( len_1 + len_2 > sizeof( source ) - 10 )
            {
              alias_spooler_full( d );
              RETURN_NULL();
            }

            str_cat( output_address  , source_command[ value -1 ] );
            output_address += str_len( source_command[ value -1 ] );
            target_address += 2;
          }

          else
          {
            *output_address++ = *target_address++;
            *output_address   = '\x0';
            if ( str_len( really_address ) > sizeof( source ) - 10 )
            {
              alias_spooler_full( d );
              RETURN_NULL();
            }
          }
        }

        else
        {
          *output_address++ = *target_address++;
          *output_address   = '\x0';
          if ( str_len( really_address ) > sizeof( source ) - 10 )
          {
            alias_spooler_full( d );
            RETURN_NULL();
          }
        }
      }
      expand_alias_spooler( d );
      RETURN_NULL();
    }
  }
  RETURN_NULL();
}

/* 檢查輸入的巨集命令是否合法 */
bool check_alias_legal( char * input )
{
  SOCIAL_DATA * social;
  CMD_DATA    * pCommand;

  PUSH_FUNCTION( "check_alias_legal" );

  for ( pCommand = cmd_list; pCommand; pCommand = pCommand->next )
    if ( !str_cmp( input , pCommand->name ) ) RETURN( FALSE );

  for ( social = social_first; social; social = social->next )
    if ( !str_cmp( input , social->name ) )        RETURN( FALSE );

  RETURN( TRUE );
}

/* 巨集指令導致輸入緩衝區滿載 */
void alias_spooler_full( DESCRIPTOR_DATA * d )
{
  PUSH_FUNCTION( "alias_spooler_full" );

  if ( !d )
  {
    mudlog( LOG_DEBUG, "alias_spooler_full: 缺乏來源." );
    RETURN_NULL();
  }

  mudlog( LOG_INFO , "[WARN] %s 巨集指令太長導致輸入緩衝區已滿."
    , d->character->name );

  write_to_descriptor( d, "\n\r你巨集指令太長導致輸入緩衝區已滿。\n\r", 0 );

  str_cpy( d->incomm, "quit\0" );
  RETURN_NULL();
}

/* 擴展巨集指令集 */
void expand_alias_spooler( DESCRIPTOR_DATA * d )
{
  char   arg[ MAX_STRING_LENGTH * 2 ];
  char * source_address;
  char * target_address;

  PUSH_FUNCTION( "expand_alias_spooler" );

  arg[0]         = '\x0';
  target_address = arg;
  source_address = d->incomm;

  while ( *source_address )
  {
    if ( *source_address == ALIAS_SEPRATOR )
    {
      *target_address++ = '\n';
      *target_address++ = '\r';
      *target_address   = '\x0';
    }

    else
    {
      *target_address++ = * source_address;
      *target_address   = '\x0';
    }

    source_address++;

    if ( *source_address >= '\x81'
      && *source_address <= '\xff'
      && *( source_address + 1 ) != '\x0' )
    {
      *target_address++ = *source_address++;
      *target_address   = '\x0';
    }
  }

  str_cat( arg , "\n\r" );
  if ( str_len( arg ) + str_len( d->inbuf ) > sizeof( d->inbuf ) - 10 )
  {
    alias_spooler_full( d );
    RETURN_NULL();
  }

  if ( ++d->alias_repeat > alias_repeat )
  {
    mudlog( LOG_INFO , "[WARN] %s 巨集指令重複次數太多, 強制斷線.",
      d->character->name );

    write_to_descriptor( d,
      "\n\r你一直執行巨集﹐系統基於安全﹐強制你斷線﹗\n\r", 0 );

    str_cpy( d->incomm, "quit\0" );
    RETURN_NULL();
  }

  write_to_descriptor( d, "\e[1;37m你的巨集指令開始計算運作﹐"
    "若是陷入無窮迴圈趕快輸入 alias -。\e[0m\n\r" , 0 );
  str_cat( d->inbuf , arg );
  d->incomm[0] =   ' ';
  d->incomm[1] = '\x0';

  RETURN_NULL();
}

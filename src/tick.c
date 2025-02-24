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

#define MAX_TICK       1000
#define MIN_TICK          2
#define TICK_MULTIPLE     2

FUNCTION( do_tick )
{
  char              arg[MAX_INPUT_LENGTH];
  int               tick;
  DESCRIPTOR_DATA * man;
  DESCRIPTOR_DATA * pDesc;

  PUSH_FUNCTION( "do_tick" );

  if ( IS_NPC( ch ) || !( pDesc = ch->desc ) ) RETURN_NULL();

  if ( !tick_permit && !IS_IMMORTAL( ch ) )
  {
    send_to_char( "對不起﹐系統管理者把計數器關閉了。\n\r" , ch );
    RETURN_NULL();
  }

  argument = one_argument( argument , arg );

  /* 如果沒有參數則是查詢 */
  if ( !arg[0] )
  {
    /* 尚未設定 */
    if ( pDesc->tick < 0 )
    {
      send_to_char( "你尚未設定你的計數器﹐請查詢 tick 的使用方法。\n\r" , ch );
      RETURN_NULL();
    }

    /* 已經設定則是查詢 */
    else
    {
      print_to_char( ch , "你設定在每 %d 個系統時脈會自動輸入 {%s}。\n\r"
        , pDesc->tick / TICK_MULTIPLE
        , pDesc->tick_command );
      RETURN_NULL();
    }
  }

  /* 若第一個參數是數字 */
  if ( is_number( arg ) )
  {
    /* 檢查秒數是否合法 */
    if ( ( tick = atoi( arg ) ) < MIN_TICK || tick > MAX_TICK )
    {
      print_to_char( ch , "計數器的系統時脈必須介於 %d 和 %d 之間。\n\r"
        , MIN_TICK , MAX_TICK );
      RETURN_NULL();
    }

    /* 檢查命令列 */
    if ( !*argument )
    {
      send_to_char( "你沒有輸入計數器的命令列。\n\r" , ch );
      RETURN_NULL();
    }


    /* 設定計數器的時脈和命令 */
    argument[sizeof( pDesc->tick_command ) - 1 ] = '\x0';
    str_cpy( pDesc->tick_command, argument );
    pDesc->tick = tick * TICK_MULTIPLE;

    act( "你設定在每$i個系統時脈會自動輸入 $T。", ch, &tick, argument, TO_CHAR );

    RETURN_NULL();
  }

  /* 清除計數器 */
  else if ( !str_prefix( arg , "!clear" ) )
  {
    pDesc->tick            = ERRORCODE;
    pDesc->tick_count      = ERRORCODE;
    pDesc->tick_command[0] = '\x0';
    send_to_char( "你把計數器清除且關閉了。\n\r" , ch );
    RETURN_NULL();
  }

  /* 關閉計數器 */
  else if ( !str_prefix( arg , "!off" ) )
  {
    pDesc->tick_count = ERRORCODE;
    send_to_char( "你把計數器關閉了。\n\r" , ch );
    RETURN_NULL();
  }

  /* 打開計數器 */
  else if ( !str_prefix( arg , "!on" ) )
  {
    if ( pDesc->tick == ERRORCODE || !pDesc->tick_command[0] )
    {
      send_to_char( "你尚未設定你的計數器﹐請查詢 tick。\n\r" , ch );
      RETURN_NULL();
    }

    pDesc->tick_count = pDesc->tick;
    print_to_char( ch , "你將每 %d 個系統時脈自動輸入 %s。\n\r"
      , pDesc->tick / TICK_MULTIPLE
      , pDesc->tick_command );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg , "!set" ) && IS_GOD( ch ) )
  {
    if ( tick_permit )
    {
      for ( man = descriptor_list; man; man = man->next )
      {
        if ( !verify_desc( man ) ) continue;

        if ( man->connected == CON_PLAYING && !IS_IMMORTAL( man->character ) )
        {
          man->tick            = ERRORCODE;
          man->tick_count      = ERRORCODE;
          man->tick_command[0] = '\x0';
        }
      }

      tick_permit = FALSE;
      send_to_char( "你把全體的計數器關閉了。\n\r" , ch );
    }

    else
    {
      tick_permit = TRUE;
      send_to_char( "你把全體的計數器打開了。\n\r" , ch );
    }

    RETURN_NULL();
  }

  else
  {
    send_to_char( "你輸入的格式不對﹐請查詢 tick。\n\r" , ch );
    RETURN_NULL();
  }

  RETURN_NULL();
}

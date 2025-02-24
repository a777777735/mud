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
#include "merc.h"

int     PlyQuota        =       -1;
int     PlyPenalty      =       -1;
int     QuestionTimer   =       -1;
int     QuestionFalse   =        1;
int     QuestionAlarm   =       20;
int     QuestionLock    =        0;

void    set_answer_default      args( ( ANSWER_DATA * ) );
void    give_question           args( ( CHAR_DATA * ) );

void set_answer_default( ANSWER_DATA * pAnswer )
{
  int loop;

  PUSH_FUNCTION( "set_answer_default" );

  pAnswer->question = NULL;
  pAnswer->false    = 0;

  for ( loop = 0; loop < MAX_QUESTION; loop++ )
    pAnswer->random[loop] = -1;

  RETURN_NULL();
}

FUNCTION( do_query )
{
  char            arg[MAX_INPUT_LENGTH];
  int             loop;
  int             count;
  int             cycle;
  CHAR_DATA     * victim;
  QUESTION_DATA * pQuestion;

  PUSH_FUNCTION( "do_query" );

  if ( !question_list )
  {
    act( "對不起﹐$t沒有任何的問題集資料﹗", ch, mud_name, NULL, TO_CHAR );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    clear_buffer();
    send_to_buffer( "\e[1;33;44m順序 次數 答錯 標題                       "
              "                                  \e[0m\n\r" );

    loop = 0;
    for ( pQuestion = question_list; pQuestion; pQuestion = pQuestion->next )
      send_to_buffer( "%4d %4d %4d %s\n\r"
        , ++loop, pQuestion->count, pQuestion->fail, pQuestion->title );

    print_buffer( ch );
  }

  else if ( arg[0] == '!' )
  {
    if ( QuestionLock == TRUE )
    {
      send_to_char( "目前無法給予題目測試。\n\r", ch );
      RETURN_NULL();
    }

    if ( arg[1] == '\x0' )
    {
      send_to_char( "對不起﹐你要出題目給那個人回答呢﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( !( victim = get_char_world( ch, arg + 1 ) ) )
    {
      act( "找不到你的對象 $2$T$0。", ch, NULL, arg+1, TO_CHAR );
      RETURN_NULL();
    }

    if ( victim->answer )
    {
      act( "$N正在回答題目﹗", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( IS_NPC( victim )
      || !victim->desc
      || !victim->in_room
      || victim->was_in_room
      || victim->position == POS_FIGHTING )
    {
      act( "對不起﹐$N目前不適合回答題目﹗", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( victim == ch )
    {
      send_to_char( "你為什麼要出題目給自己答呢﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
      act( "$N的等級比你還要高﹗", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    give_question( victim );

    act( "$n出了一個題目給$N回答﹗", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "lock" ) )
  {
    if ( !IS_IMMORTAL( ch ) )
    {
      send_to_char( "你沒有更動鎖定的權利。\n\r", ch );
      RETURN_NULL();
    }

    if ( QuestionLock == FALSE )
    {
      send_to_char( "你把問題資料鎖定了。\n\r", ch );
      QuestionLock = TRUE;
      RETURN_NULL();
    }
    else
    {
      send_to_char( "你把問題資料鎖定取消了。\n\r", ch );
      QuestionLock = FALSE;
      RETURN_NULL();
    }
  }

  else if ( !str_prefix( arg, "look" ) )
  {
    argument = one_argument( argument, arg );

    if ( arg[0] == '\x0' )
    {
      send_to_char( "你要察看誰的問題﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( !( victim = get_char_world( ch, arg ) ) )
    {
      act( "對不起﹐找不到你的對象 $2$T$0﹗", ch, NULL, arg, TO_CHAR );
      RETURN_NULL();
    }

    if ( !victim->answer )
    {
      act( "對不起﹐$N並沒有回答問題﹗", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    send_answer( victim, ch );
  }

  else if ( !str_prefix( arg, "cancel" ) )
  {
    argument = one_argument( argument, arg );

    if ( arg[0] == '\x0' )
    {
      send_to_char( "你要解除誰的問題﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( !( victim = get_char_world( ch, arg ) ) )
    {
      act( "對不起﹐找不到你的對象 $2$T$0﹗", ch, NULL, arg, TO_CHAR );
      RETURN_NULL();
    }

    if ( !victim->answer )
    {
      act( "對不起﹐$N並沒有回答問題﹗", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    free_struct( victim->answer, STRUCT_ANSWER_DATA );

    victim->answer = NULL;
    if ( victim->pcdata ) victim->pcdata->ply = 0;

    send_to_char( "你可以不用回答問題了﹗\n\r", victim );
    if ( ch != victim ) act( "$N可以不用回答問題﹗", ch, NULL, victim, TO_CHAR );
  }

  else if ( !str_prefix( arg, "info" ) )
  {
    print_to_char( ch,
      "練功\次數限制﹕%d 次\n\r"
      "練 功\ 處 罰 ﹕%d 天\n\r"
      "作 答 時 間  ﹕%d 秒\n\r"
      "答 錯 次 數  ﹕%d 次\n\r"
      "提 醒 間 隔  ﹕%d 秒\n\r"
      , PlyQuota
      , PlyPenalty
      , QuestionTimer
      , QuestionFalse
      , QuestionAlarm );
  }

  else if ( !str_prefix( arg, "show" ) )
  {
    argument = one_argument( argument, arg );

    if ( arg[0] == '\x0' || !is_number( arg ) )
    {
      send_to_char( "請輸入欲查詢的號碼順序﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( ( count = atoi( arg ) ) <= 0 )
    {
      send_to_char( "你的順序號碼不合理﹗\n\r", ch );
      RETURN_NULL();
    }

    loop = 0;
    for ( pQuestion = question_list; pQuestion; pQuestion = pQuestion->next )
    {
      if ( ++loop == count )
      {
        clear_buffer();
        send_to_buffer( "序號﹕ %d\n\r題目﹕ %s\n\r"
          , count, pQuestion->title );

        for ( cycle = 0; cycle < MAX_QUESTION; cycle++ )
        {
          if ( !pQuestion->question[cycle] || !*pQuestion->question[cycle] )
            break;

          send_to_buffer( "答案﹕ %s %s\n\r"
            , YESNO( pQuestion->answer[cycle] ), pQuestion->question[cycle] );
        }

        print_buffer( ch );
        RETURN_NULL();
      }
    }

    send_to_char( "對不起﹐找不到K那個序號的問題資料﹗\n\r", ch );
    RETURN_NULL();
  }

  else
  {
    send_to_char( "你的參數錯誤﹐請查詢 query 的使用方法 ﹗\n\r", ch );
  }
  RETURN_NULL();
}

void question_update( void )
{
  QUESTION_DATA   * pQuestion;
  DESCRIPTOR_DATA * man;
  ANSWER_DATA     * pAnswer;
  CHAR_DATA       * ch;
  char              buf[MAX_STRING_LENGTH];
  static time_t     RecordTime = -1;

  PUSH_FUNCTION( "question_update" );

  if ( !question_list
    || PlyQuota <= 0
    || PlyPenalty <= 0
    || QuestionTimer <= 0
    || current_time == RecordTime
    || QuestionLock == FALSE ) RETURN_NULL();

  RecordTime = current_time;

  for ( man = descriptor_list; man; man = man->next )
  {
    if ( !verify_desc( man )
      || man->connected != CON_PLAYING
      || !( ch = man->character )
      || IS_IMMORTAL( ch )
      || !ch->in_room
      || ch->jail > 0
      || ch->failed > 0
      || ch->was_in_room
      || ( ch->pcdata && ch->pcdata->corpse ) ) continue;

    if ( !ch->answer
      && ch->position != POS_FIGHTING
      && ch->pcdata
      && ch->pcdata->ply >= PlyQuota )
    {
      give_question( ch );
      continue;
    }

    if ( ( pAnswer = ch->answer ) && ( pQuestion = pAnswer->question ) )
    {
      if ( pAnswer->timer <= 0 )
      {
        if ( ch->pcdata ) ch->pcdata->ply = 0;

        if ( jail_someone( NULL, ch, PlyPenalty, FALSE ) )
        {
          sprintf( buf, "%s為機器人嫌疑犯﹐被系統捉起來了﹗"
            , mob_name( NULL, ch ) );

          talk_channel_2( buf, CHANNEL_BULLETIN, "公告" );

          send_to_char( "因為你在限時內未能解答﹐"
            "所以系統認定你是機器人﹗\n\r", ch );

          pQuestion->fail++;
          free_struct( pAnswer, STRUCT_ANSWER_DATA );
          ch->answer = NULL;
        }

        else
        {
          send_to_char( "你為機器人嫌疑犯﹗\n\r", ch );
        }
      }

      else if ( --pAnswer->timer % ( UMAX( 1, QuestionAlarm ) ) == 0 )
      {
        send_answer( ch, ch );
      }
    }
  }

  RETURN_NULL();
}

void send_answer( CHAR_DATA * ch, CHAR_DATA * looker )
{
  ANSWER_DATA   * pAnswer;
  QUESTION_DATA * pQuestion;
  char            buf[BUFSIZ];
  int             loop;
  int             color;

  PUSH_FUNCTION( "send_answer" );

  if ( !ch
    || !looker
    || !( pAnswer = ch->answer )
    || !( pQuestion = pAnswer->question ) )
  {
    mudlog( LOG_DEBUG, "send_answer: 來源不正確." );
    RETURN_NULL();
  }

  if ( QuestionLock == TRUE ) RETURN_NULL();

  clear_buffer();
  send_to_buffer(
    "由於你有重覆練功\的情況﹐所以系統出個問題考考你﹕\n\r%s"
    "題目﹕\e[1;32m%s\e[0m\n\r"
    , VERTICAL_LINE, pQuestion->title );

  for ( loop = 0; loop < MAX_QUESTION; loop++ )
  {
    if ( pAnswer->random[loop] < 0 || pAnswer->random[loop] >= MAX_QUESTION )
      break;

    color = number_range( 31, 37 );
    send_to_buffer( "\e[1;%dm%2d\e[0m ) %s\e[0m\n\r"
      , color
      , loop + 1
      , pQuestion->question[pAnswer->random[loop]] );
  }

  chinese_number( pAnswer->timer, buf );
  send_to_buffer( "\n\r剩下%s%s\e[0m秒可以做答(answer)﹗"
    "如果答案為 1﹐則輸入 \e[1;32manswer 1\e[0m﹐以此類推﹗\n\r%s"
      , pAnswer->timer <= 30 ? "\e[1;31m\a\a" : ""
      , buf, VERTICAL_LINE );

  print_buffer( looker );
  RETURN_NULL();
}

void give_question( CHAR_DATA * ch )
{
  QUESTION_DATA * pQuestion;
  QUESTION_DATA * zQuestion;
  ANSWER_DATA   * pAnswer;
  int             loop;
  int             count;
  int             temp;
  int             random_1;
  int             random_2;

  PUSH_FUNCTION( "give_question" );

  if ( !ch || !verify_char( ch ) || IS_NPC( ch ) )
  {
    mudlog( LOG_DEBUG, "give_question: 來源不正確." );
    RETURN_NULL();
  }

  if ( ch->answer )
  {
    mudlog( LOG_DEBUG, "give_question: 來源已有問題." );
    RETURN_NULL();
  }

  if ( QuestionLock == TRUE ) RETURN_NULL();

  count     = 1;
  zQuestion = NULL;

  for ( pQuestion = question_list; pQuestion; pQuestion = pQuestion->next )
    if ( number_range( 1, count++ ) == 1 ) zQuestion = pQuestion;

  if ( zQuestion )
  {
    pAnswer = alloc_struct( STRUCT_ANSWER_DATA );
    set_answer_default( pAnswer );

    pAnswer->question = zQuestion;
    pAnswer->timer    = QuestionTimer;
    zQuestion->count++;

    for ( count = loop = 0; loop < MAX_QUESTION; loop++ )
      if ( zQuestion->question[loop] && *zQuestion->question[loop] )
        count++;

    for ( loop = 0; loop < count; loop++ ) pAnswer->random[loop] = loop;

    for ( loop = 0; loop < 1000; loop++ )
    {
      random_1 = number_range( 0, count - 1 );
      random_2 = number_range( 0, count - 1 );

      temp                      = pAnswer->random[random_1];
      pAnswer->random[random_1] = pAnswer->random[random_2];
      pAnswer->random[random_2] = temp;
    }

    ch->answer = pAnswer;
    send_answer( ch, ch );
  }

  RETURN_NULL();
}

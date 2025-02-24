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
    act( "�藍�_�M$t�S�����󪺰��D����ơT", ch, mud_name, NULL, TO_CHAR );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    clear_buffer();
    send_to_buffer( "\e[1;33;44m���� ���� ���� ���D                       "
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
      send_to_char( "�ثe�L�k�����D�ش��աC\n\r", ch );
      RETURN_NULL();
    }

    if ( arg[1] == '\x0' )
    {
      send_to_char( "�藍�_�M�A�n�X�D�ص����ӤH�^���O�S\n\r", ch );
      RETURN_NULL();
    }

    if ( !( victim = get_char_world( ch, arg + 1 ) ) )
    {
      act( "�䤣��A����H $2$T$0�C", ch, NULL, arg+1, TO_CHAR );
      RETURN_NULL();
    }

    if ( victim->answer )
    {
      act( "$N���b�^���D�ءT", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( IS_NPC( victim )
      || !victim->desc
      || !victim->in_room
      || victim->was_in_room
      || victim->position == POS_FIGHTING )
    {
      act( "�藍�_�M$N�ثe���A�X�^���D�ءT", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( victim == ch )
    {
      send_to_char( "�A������n�X�D�ص��ۤv���O�S\n\r", ch );
      RETURN_NULL();
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
      act( "$N�����Ť�A�٭n���T", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    give_question( victim );

    act( "$n�X�F�@���D�ص�$N�^���T", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "lock" ) )
  {
    if ( !IS_IMMORTAL( ch ) )
    {
      send_to_char( "�A�S�������w���v�Q�C\n\r", ch );
      RETURN_NULL();
    }

    if ( QuestionLock == FALSE )
    {
      send_to_char( "�A����D�����w�F�C\n\r", ch );
      QuestionLock = TRUE;
      RETURN_NULL();
    }
    else
    {
      send_to_char( "�A����D�����w�����F�C\n\r", ch );
      QuestionLock = FALSE;
      RETURN_NULL();
    }
  }

  else if ( !str_prefix( arg, "look" ) )
  {
    argument = one_argument( argument, arg );

    if ( arg[0] == '\x0' )
    {
      send_to_char( "�A�n��ݽ֪����D�S\n\r", ch );
      RETURN_NULL();
    }

    if ( !( victim = get_char_world( ch, arg ) ) )
    {
      act( "�藍�_�M�䤣��A����H $2$T$0�T", ch, NULL, arg, TO_CHAR );
      RETURN_NULL();
    }

    if ( !victim->answer )
    {
      act( "�藍�_�M$N�èS���^�����D�T", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    send_answer( victim, ch );
  }

  else if ( !str_prefix( arg, "cancel" ) )
  {
    argument = one_argument( argument, arg );

    if ( arg[0] == '\x0' )
    {
      send_to_char( "�A�n�Ѱ��֪����D�S\n\r", ch );
      RETURN_NULL();
    }

    if ( !( victim = get_char_world( ch, arg ) ) )
    {
      act( "�藍�_�M�䤣��A����H $2$T$0�T", ch, NULL, arg, TO_CHAR );
      RETURN_NULL();
    }

    if ( !victim->answer )
    {
      act( "�藍�_�M$N�èS���^�����D�T", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    free_struct( victim->answer, STRUCT_ANSWER_DATA );

    victim->answer = NULL;
    if ( victim->pcdata ) victim->pcdata->ply = 0;

    send_to_char( "�A�i�H���Φ^�����D�F�T\n\r", victim );
    if ( ch != victim ) act( "$N�i�H���Φ^�����D�T", ch, NULL, victim, TO_CHAR );
  }

  else if ( !str_prefix( arg, "info" ) )
  {
    print_to_char( ch,
      "�m�\\���ƭ���R%d ��\n\r"
      "�m �\\ �B �@ �R%d ��\n\r"
      "�@ �� �� ��  �R%d ��\n\r"
      "�� �� �� ��  �R%d ��\n\r"
      "�� �� �� �j  �R%d ��\n\r"
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
      send_to_char( "�п�J���d�ߪ����X���ǡT\n\r", ch );
      RETURN_NULL();
    }

    if ( ( count = atoi( arg ) ) <= 0 )
    {
      send_to_char( "�A�����Ǹ��X���X�z�T\n\r", ch );
      RETURN_NULL();
    }

    loop = 0;
    for ( pQuestion = question_list; pQuestion; pQuestion = pQuestion->next )
    {
      if ( ++loop == count )
      {
        clear_buffer();
        send_to_buffer( "�Ǹ��R %d\n\r�D�ءR %s\n\r"
          , count, pQuestion->title );

        for ( cycle = 0; cycle < MAX_QUESTION; cycle++ )
        {
          if ( !pQuestion->question[cycle] || !*pQuestion->question[cycle] )
            break;

          send_to_buffer( "���סR %s %s\n\r"
            , YESNO( pQuestion->answer[cycle] ), pQuestion->question[cycle] );
        }

        print_buffer( ch );
        RETURN_NULL();
      }
    }

    send_to_char( "�藍�_�M�䤣��K���ӧǸ������D��ơT\n\r", ch );
    RETURN_NULL();
  }

  else
  {
    send_to_char( "�A���Ѽƿ��~�M�Ьd�� query ���ϥΤ�k �T\n\r", ch );
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
          sprintf( buf, "%s�������H���åǡM�Q�t�ή��_�ӤF�T"
            , mob_name( NULL, ch ) );

          talk_channel_2( buf, CHANNEL_BULLETIN, "���i" );

          send_to_char( "�]���A�b���ɤ�����ѵ��M"
            "�ҥH�t�λ{�w�A�O�����H�T\n\r", ch );

          pQuestion->fail++;
          free_struct( pAnswer, STRUCT_ANSWER_DATA );
          ch->answer = NULL;
        }

        else
        {
          send_to_char( "�A�������H���åǡT\n\r", ch );
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
    mudlog( LOG_DEBUG, "send_answer: �ӷ������T." );
    RETURN_NULL();
  }

  if ( QuestionLock == TRUE ) RETURN_NULL();

  clear_buffer();
  send_to_buffer(
    "�ѩ�A�����нm�\\�����p�M�ҥH�t�ΥX�Ӱ��D�ҦҧA�R\n\r%s"
    "�D�ءR\e[1;32m%s\e[0m\n\r"
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
  send_to_buffer( "\n\r�ѤU%s%s\e[0m��i�H����(answer)�T"
    "�p�G���׬� 1�M�h��J \e[1;32manswer 1\e[0m�M�H�������T\n\r%s"
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
    mudlog( LOG_DEBUG, "give_question: �ӷ������T." );
    RETURN_NULL();
  }

  if ( ch->answer )
  {
    mudlog( LOG_DEBUG, "give_question: �ӷ��w�����D." );
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

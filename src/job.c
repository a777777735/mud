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

extern void       check_contraband args( ( CHAR_DATA * ) );

/* �w�q�i�H���v�޳������� */
#define LEVEL_CAN_PK               35

/* �w�q���v�޳��n�檺�� */
#define FIGHT_MONEY                10000

/* �w�q�̤p�i�H�h�s��Ϫ����� */
#define LEVEL_NO_NEW               15

DECLARE_JOB_FUN( job_recall_new    );
DECLARE_JOB_FUN( job_goto_pk_area   );

#if defined(FUNC_NAME)
#undef FUNC_NAME
#endif

#define FUNC_NAME( nFunction )                             \
  if ( !str_cmp( name, #nFunction ) ) RETURN( nFunction ); \

JOB_FUN * job_lookup( const char * name )
{
  PUSH_FUNCTION( "job_lookup" );
  FUNC_NAME( job_recall_new   );
  FUNC_NAME( job_goto_pk_area );
  RETURN( NULL );
}

JOB( job_recall_new )
{
  ROOM_INDEX_DATA * pRoom;

  PUSH_FUNCTION( "job_recall_new" );

  if ( !( pRoom = RoomSchool )
    || !ch->in_room
    || !can_char_from_room( ch, TRUE ) ) RETURN_NULL();

  if ( ch->level >= LEVEL_NO_NEW )
  {
    send_to_char( "�A���w�g����ѤF�ٷQ�h�s��ϰڡM�u���`��T\n\r", ch );
    RETURN_NULL();
  }

  act( "$n�V�ѯ���ë�^��s����~��V�m�C", ch, NULL, NULL, TO_ROOM );
  send_to_char( "\e[1;33m�A�T�V���k�^�s��ϡT\e[0m\n\r\n\r", ch );

  char_from_room( ch );
  char_to_room( ch, pRoom );

  act( "$n�T�V���k�F�^�ӡT", ch, NULL, NULL, TO_ROOM );
  do_look( ch, "auto" );

  /* �M���l�ܬ����I */
  clear_trace( ch, TRUE );

  RETURN_NULL();
}

JOB( job_goto_pk_area )
{
  char              buf[MAX_STRING_LENGTH];
  ROOM_INDEX_DATA * pIndex1;
  ROOM_INDEX_DATA * pIndex2;

  PUSH_FUNCTION( "job_goto_pk_area" );

  if ( !ch || !verify_char( ch ) ) RETURN_NULL();

  if ( !( pIndex1 = get_room_index( 7043 ) )
    || !( pIndex2 = get_room_index( 7044 ) ) )
  {
    mudlog( LOG_DEBUG, "job_goto_pk_area: �S����԰�." );
    send_to_char( "�藍�_, �ثe�t�ΨS����԰�.\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->level <= LEVEL_CAN_PK )
  {
    chinese_number( LEVEL_CAN_PK, buf );
    act( "�藍�_�M�A�����ť����W�L$t�Ť~��ѥ[�j�V�ԡT",
      ch, buf, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( get_age( ch ) <= pk_age )
  {
    chinese_number( pk_age, buf );
    act( "�藍�_�M�A���~�֥����W�L$t���~��ѥ[�j�V�ԡT",
      ch, buf, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( ch->master || ch->leader )
  {
    send_to_char( " �藍�_�M�A�����H�O�H�T\n\r", ch );
    RETURN_NULL();
  }

  if ( auction_info->seller && auction_info->seller == ch )
  {
    send_to_char( "�藍�_�M���A�槹�F��A�Ӽr���a�T\n\r", ch );
    RETURN_NULL();
  }

  if ( auction_info->buyer && auction_info->buyer == ch )
  {
    send_to_char( "�藍�_�M���A�R���F��A�Ӽr���a�T\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->jail > 0 )
  {
    send_to_char( "����A���D���A���A���a�T\n\r", ch );
    RETURN_NULL();
  }

  if ( IS_SET( ch->act, PLR_KILLER )
    || IS_SET( ch->act, PLR_BOLTER )
    || IS_SET( ch->act, PLR_THIEF ) )
  {
    send_to_char( "�A�p�G�ѥ[�|���j�a���D�A�]�������ܡT\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->mount )
  {
    act( "�A�٦b$N�W�M���U���a�T", ch, NULL, ch->mount, TO_CHAR );
    RETURN_NULL();
  }

  if ( ch->spirit )
  {
    act( "�藍�_�M�A����a��$N�ѥ[�j�V�Գ�T", ch, NULL, ch->spirit, TO_CHAR );
    RETURN_NULL();
  }

  if ( ch->gold < FIGHT_MONEY )
  {
    send_to_char( "�A���������I�����T\n\r", ch );
    RETURN_NULL();
  }

  check_contraband( ch );

  act( "�ѤW�ƨӤ@�����M��$n�ǰe��T���v�޳��o�T", ch, NULL, NULL, TO_ALL );

  sprintf( buf, "%s�������i�J�F�T���v�޳�, �֥h��L��!"
    , mob_name( NULL, ch ) );
  talk_channel_2( buf, CHANNEL_PK, "" );

  char_from_room( ch );

  if ( number_percent() > 50 )
    char_to_room( ch, pIndex1 );
  else
    char_to_room( ch, pIndex2 );

  do_look( ch, "auto" );

  /* �M���l�ܬ����I */
  clear_trace( ch, TRUE );

  act( "$n�Ӱe���o�T", ch, NULL, NULL, TO_ROOM );
  gold_from_char( ch, FIGHT_MONEY );

  RETURN_NULL();
}

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

extern void       check_contraband args( ( CHAR_DATA * ) );

/* 定義可以到競技場的等級 */
#define LEVEL_CAN_PK               35

/* 定義到競技場要交的錢 */
#define FIGHT_MONEY                10000

/* 定義最小可以去新手區的等級 */
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
    send_to_char( "你都已經那麼老了還想去新手區啊﹐真不害臊﹗\n\r", ch );
    RETURN_NULL();
  }

  act( "$n向天神祈禱回到新手區繼續訓練。", ch, NULL, NULL, TO_ROOM );
  send_to_char( "\e[1;33m你狼狽的逃回新手區﹗\e[0m\n\r\n\r", ch );

  char_from_room( ch );
  char_to_room( ch, pRoom );

  act( "$n狼狽的逃了回來﹗", ch, NULL, NULL, TO_ROOM );
  do_look( ch, "auto" );

  /* 清除追蹤紀錄點 */
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
    mudlog( LOG_DEBUG, "job_goto_pk_area: 沒有對戰區." );
    send_to_char( "對不起, 目前系統沒有對戰區.\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->level <= LEVEL_CAN_PK )
  {
    chinese_number( LEVEL_CAN_PK, buf );
    act( "對不起﹐你的等級必須超過$t級才能參加大混戰﹗",
      ch, buf, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( get_age( ch ) <= pk_age )
  {
    chinese_number( pk_age, buf );
    act( "對不起﹐你的年齡必須超過$t歲才能參加大混戰﹗",
      ch, buf, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( ch->master || ch->leader )
  {
    send_to_char( " 對不起﹐你正跟隨別人﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( auction_info->seller && auction_info->seller == ch )
  {
    send_to_char( "對不起﹐等你賣完東西再來廝殺吧﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( auction_info->buyer && auction_info->buyer == ch )
  {
    send_to_char( "對不起﹐等你買完東西再來廝殺吧﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->jail > 0 )
  {
    send_to_char( "先把你的刑期服完再說吧﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( IS_SET( ch->act, PLR_KILLER )
    || IS_SET( ch->act, PLR_BOLTER )
    || IS_SET( ch->act, PLR_THIEF ) )
  {
    send_to_char( "你如果參加會讓大家知道你跑路的行蹤﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->mount )
  {
    act( "你還在$N上﹐先下馬吧﹗", ch, NULL, ch->mount, TO_CHAR );
    RETURN_NULL();
  }

  if ( ch->spirit )
  {
    act( "對不起﹐你不能帶著$N參加大混戰喔﹗", ch, NULL, ch->spirit, TO_CHAR );
    RETURN_NULL();
  }

  if ( ch->gold < FIGHT_MONEY )
  {
    send_to_char( "你的錢不夠付門票﹗\n\r", ch );
    RETURN_NULL();
  }

  check_contraband( ch );

  act( "天上飄來一朵雲﹐把$n傳送到三國競技場囉﹗", ch, NULL, NULL, TO_ALL );

  sprintf( buf, "%s偷偷的進入了三國競技場, 快去扁他喔!"
    , mob_name( NULL, ch ) );
  talk_channel_2( buf, CHANNEL_PK, "" );

  char_from_room( ch );

  if ( number_percent() > 50 )
    char_to_room( ch, pIndex1 );
  else
    char_to_room( ch, pIndex2 );

  do_look( ch, "auto" );

  /* 清除追蹤紀錄點 */
  clear_trace( ch, TRUE );

  act( "$n來送死囉﹗", ch, NULL, NULL, TO_ROOM );
  gold_from_char( ch, FIGHT_MONEY );

  RETURN_NULL();
}

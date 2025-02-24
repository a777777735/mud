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
#include <time.h>
#include "merc.h"

int     serial_time = 0;
int     serial_loop = 1;

void    log_serial_duplicate    args( ( OBJ_DATA * , OBJ_DATA * ) );

/* 起始化時間序號變數 */
void initial_serial_time( void )
{
  PUSH_FUNCTION( "initial_serial_time" );
  serial_time = ( int ) ( time( NULL ) );
  serial_loop = 1;
  RETURN_NULL();
}

void set_serial( SERIAL_DATA * pSerial )
{
  PUSH_FUNCTION( "set_serial" );

  if ( !pSerial )
  {
    mudlog( LOG_DEBUG, "set_serial: 缺乏來源." );
    RETURN_NULL();
  }

  if ( pSerial->high != 0 || pSerial->low != 0 ) RETURN_NULL();

  if ( serial_time == 0 ) initial_serial_time();

  pSerial->high = serial_time;
  pSerial->low  = serial_loop;
  serial_loop  += number_range( 1, 3 );

  RETURN_NULL();
}

void check_serial_number( CHAR_DATA * ch )
{
  CHAR_DATA * owner;
  OBJ_DATA  * obj;
  OBJ_DATA  * temp;
  bool        marked = FALSE;

  PUSH_FUNCTION( "check_serial_number" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG , "check_serial_number: 檢查物品序號, 人物是空的." );
    RETURN_NULL();
  }

  /* 看看對象是否為虛擬怪物 */
  if ( IS_NPC( ch ) ) RETURN_NULL();

  /* 設定自己身上帶的物品序號 */
  for ( obj = ch->carrying ; obj; obj = obj->next_content )
    set_serial( &obj->serial );

  /* 設定自己寄放物品的序號 */
  for ( obj = ch->deposit ; obj; obj = obj->next_content )
    set_serial( &obj->serial );

  /* 搜尋全世界存在的物品 */
  for ( temp = object_list; temp; temp = temp->next )
  {
    if ( !verify_obj( temp ) ) continue;

    if ( temp->serial.high == 0 || temp->serial.low == 0 ) continue;

    /* 檢查所攜帶的物品 */
    for ( obj = ch->carrying; obj; obj = obj->next_content )
    {
      if ( obj->serial.low  == temp->serial.low
        && obj->serial.high == temp->serial.high
        && obj != temp )
      {
        if ( ( owner = object_owner( temp ) )
          && owner->desc
          && owner->desc->connected != CON_PLAYING )
          continue;

        log_serial_duplicate( obj , temp );
        SET_BIT( ch->pcdata->suspect , PLR_COPYEQ );
        marked = TRUE;
      }
    }

    /* 檢查所寄放的物品 */
    for ( obj = ch->deposit; obj; obj = obj->next_content )
    {
      if ( obj->serial.low  == temp->serial.low
        && obj->serial.high == temp->serial.high
        && obj != temp )
      {
        if ( ( owner = object_owner( temp ) )
          && owner->desc
          && owner->desc->connected != CON_PLAYING )
          continue;

        log_serial_duplicate( obj , temp );
        SET_BIT( ch->pcdata->suspect , PLR_COPYEQ );
        marked = TRUE;
      }
    }
  }

  if ( marked ) mudlog( LOG_SUSPECT, "%s 涉嫌拷貝裝備.", ch->name );

  RETURN_NULL();
}

/* 記錄序號相同者至檔案中 */
void log_serial_duplicate( OBJ_DATA * org , OBJ_DATA * des )
{
  FILE * pFile;

  PUSH_FUNCTION( "log_serial_duplicate" );

  if ( !org || !des || org == des ) RETURN_NULL();

  if ( ( pFile = FOPEN( check_file , "a" ) ) )
  {
    fprintf( pFile ,"日期         %s\n"
      , time_format( time( NULL ) , "%r, %a-%d-%b-%y" ) );

    fprintf( pFile, "原始物品     [%s] (%s)( 號碼﹕%d )\n"
      , org->cname
      , org->name
      , org->pIndexData->vnum );

    fprintf( pFile, "複製物品     [%s] (%s)( 號碼﹕%d )\n"
      , des->cname
      , des->name
      , des->pIndexData->vnum );

    if ( org->carried_by )
      fprintf( pFile, "原始擁有者   %s\n" , org->carried_by->name );

    else if ( org->deposit_by )
      fprintf( pFile, "原始寄放者   %s\n" , org->deposit_by->name );

    if ( des->carried_by )
      fprintf( pFile, "複製擁有者   %s\n" , des->carried_by->name );

    else if ( des->deposit_by )
      fprintf( pFile, "複製寄放者   %s\n" , des->deposit_by->name );

    fprintf( pFile, "複製序號     %d-%d\n"
      , des->serial.high , des->serial.low );

    fprintf( pFile , "-------------------------------------------\n" );
    FCLOSE( pFile );

    /* 改變檔案存取模式 */
    set_file_mode( check_file );

  }

  RETURN_NULL();
}

FUNCTION( do_serial )
{
  DATABASE_DATA * pData;
  char            arg[ MAX_INPUT_LENGTH];
  char            arg1[MAX_INPUT_LENGTH];
  char            buf[MAX_STRING_LENGTH];
  OBJ_DATA      * obj;
  int             high;
  int             low;

  PUSH_FUNCTION( "do_serial" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  if ( argument[0] == '@' )
  {
    argument = one_argument( argument + 1, arg  );
               one_argument( argument, arg1 );

    if ( !arg[0] || !arg1[0] || !is_number( arg ) || !is_number( arg1 ) )
    {
      send_to_char( "你必須輸入兩個數字來查詢。\n\r", ch );
      RETURN_NULL();
    }

    pData = database_lookup( high = atoi( arg ), low = atoi( arg1 ) );

    if ( !pData )
    {
      act( "沒有 $x-$X 這組序號的人﹗", ch, &high, &low, TO_CHAR );
    }
    else
    {
      if ( pData->exist )
      {
        print_to_char( ch, "目前有人叫 %s ﹐他的序號就是 %d-%d﹗\n\r"
          , pData->name, high, low );
      }
      else
      {
        print_to_char( ch, "之前有個人叫 %s﹐他序號就是 %d-%d﹐不過他已"
          "經不存在了﹗", pData->name, high, low );
      }
    }
  }

  else
  {
    argument = one_argument( argument, arg );

    if ( !( obj = get_obj_carry( ch , arg ) ) )
    {
      act( "抱歉﹐你身上沒有這樣物品 $2$T$0。" , ch, NULL, arg, TO_CHAR );
      RETURN_NULL();
    }

    if ( obj->serial.high == 0 && obj->serial.low == 0 )
    {
      act( "物品$p尚未有任何序號。", ch, obj, NULL, TO_CHAR );
    }

    else
    {
      sprintf( buf, "%d-%d", obj->serial.high, obj->serial.low );
      act( "$p的流水序號是 $2$T$0。", ch, obj, buf, TO_CHAR );
    }
  }

  RETURN_NULL();
}

/* 搜尋某序號的物品 */
FUNCTION( do_sefind )
{
  char       arg[ MAX_INPUT_LENGTH ];
  int        serial_number;
  int        count;
  OBJ_DATA * obj;

  PUSH_FUNCTION( "do_sefind" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  argument = one_argument( argument, arg );

  if ( !is_number( arg ) || ( serial_number = atoi( arg ) ) <= 0 )
  {
    send_to_char( "物品序號不能小於等於 0。\n\r" , ch );
    RETURN_NULL();
  }

  for ( clear_buffer(), count = 0, obj = object_list; obj; obj = obj->next )
  {
    if ( !verify_obj( obj ) ) continue;

    if ( obj->serial.low == serial_number )
    {
      send_to_buffer( "[%3d] %s 的序號為 %d-%d %s\n\r"
        , ++count , obj_name( ch, obj ), obj->serial.high
        , obj->serial.low, object_locate( obj , ch ) );

      if ( buffer_full() ) break;
    }
  }

  if ( !count )  send_to_buffer( "尚未發現到相同序號的物品。\n\r" );
  print_buffer( ch );
  RETURN_NULL();
}

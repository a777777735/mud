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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"

int     serial_time = 0;
int     serial_loop = 1;

void    log_serial_duplicate    args( ( OBJ_DATA * , OBJ_DATA * ) );

/* �_�l�Ʈɶ��Ǹ��ܼ� */
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
    mudlog( LOG_DEBUG, "set_serial: �ʥF�ӷ�." );
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
    mudlog( LOG_DEBUG , "check_serial_number: �ˬd���~�Ǹ�, �H���O�Ū�." );
    RETURN_NULL();
  }

  /* �ݬݹ�H�O�_�������Ǫ� */
  if ( IS_NPC( ch ) ) RETURN_NULL();

  /* �]�w�ۤv���W�a�����~�Ǹ� */
  for ( obj = ch->carrying ; obj; obj = obj->next_content )
    set_serial( &obj->serial );

  /* �]�w�ۤv�H�񪫫~���Ǹ� */
  for ( obj = ch->deposit ; obj; obj = obj->next_content )
    set_serial( &obj->serial );

  /* �j�M���@�ɦs�b�����~ */
  for ( temp = object_list; temp; temp = temp->next )
  {
    if ( !verify_obj( temp ) ) continue;

    if ( temp->serial.high == 0 || temp->serial.low == 0 ) continue;

    /* �ˬd����a�����~ */
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

    /* �ˬd�ұH�񪺪��~ */
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

  if ( marked ) mudlog( LOG_SUSPECT, "%s �A�������˳�.", ch->name );

  RETURN_NULL();
}

/* �O���Ǹ��ۦP�̦��ɮפ� */
void log_serial_duplicate( OBJ_DATA * org , OBJ_DATA * des )
{
  FILE * pFile;

  PUSH_FUNCTION( "log_serial_duplicate" );

  if ( !org || !des || org == des ) RETURN_NULL();

  if ( ( pFile = FOPEN( check_file , "a" ) ) )
  {
    fprintf( pFile ,"���         %s\n"
      , time_format( time( NULL ) , "%r, %a-%d-%b-%y" ) );

    fprintf( pFile, "��l���~     [%s] (%s)( ���X�R%d )\n"
      , org->cname
      , org->name
      , org->pIndexData->vnum );

    fprintf( pFile, "�ƻs���~     [%s] (%s)( ���X�R%d )\n"
      , des->cname
      , des->name
      , des->pIndexData->vnum );

    if ( org->carried_by )
      fprintf( pFile, "��l�֦���   %s\n" , org->carried_by->name );

    else if ( org->deposit_by )
      fprintf( pFile, "��l�H���   %s\n" , org->deposit_by->name );

    if ( des->carried_by )
      fprintf( pFile, "�ƻs�֦���   %s\n" , des->carried_by->name );

    else if ( des->deposit_by )
      fprintf( pFile, "�ƻs�H���   %s\n" , des->deposit_by->name );

    fprintf( pFile, "�ƻs�Ǹ�     %d-%d\n"
      , des->serial.high , des->serial.low );

    fprintf( pFile , "-------------------------------------------\n" );
    FCLOSE( pFile );

    /* �����ɮצs���Ҧ� */
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
      send_to_char( "�A������J��ӼƦr�Ӭd�ߡC\n\r", ch );
      RETURN_NULL();
    }

    pData = database_lookup( high = atoi( arg ), low = atoi( arg1 ) );

    if ( !pData )
    {
      act( "�S�� $x-$X �o�էǸ����H�T", ch, &high, &low, TO_CHAR );
    }
    else
    {
      if ( pData->exist )
      {
        print_to_char( ch, "�ثe���H�s %s �M�L���Ǹ��N�O %d-%d�T\n\r"
          , pData->name, high, low );
      }
      else
      {
        print_to_char( ch, "���e���ӤH�s %s�M�L�Ǹ��N�O %d-%d�M���L�L�w"
          "�g���s�b�F�T", pData->name, high, low );
      }
    }
  }

  else
  {
    argument = one_argument( argument, arg );

    if ( !( obj = get_obj_carry( ch , arg ) ) )
    {
      act( "��p�M�A���W�S���o�˪��~ $2$T$0�C" , ch, NULL, arg, TO_CHAR );
      RETURN_NULL();
    }

    if ( obj->serial.high == 0 && obj->serial.low == 0 )
    {
      act( "���~$p�|��������Ǹ��C", ch, obj, NULL, TO_CHAR );
    }

    else
    {
      sprintf( buf, "%d-%d", obj->serial.high, obj->serial.low );
      act( "$p���y���Ǹ��O $2$T$0�C", ch, obj, buf, TO_CHAR );
    }
  }

  RETURN_NULL();
}

/* �j�M�Y�Ǹ������~ */
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
    send_to_char( "���~�Ǹ�����p�󵥩� 0�C\n\r" , ch );
    RETURN_NULL();
  }

  for ( clear_buffer(), count = 0, obj = object_list; obj; obj = obj->next )
  {
    if ( !verify_obj( obj ) ) continue;

    if ( obj->serial.low == serial_number )
    {
      send_to_buffer( "[%3d] %s ���Ǹ��� %d-%d %s\n\r"
        , ++count , obj_name( ch, obj ), obj->serial.high
        , obj->serial.low, object_locate( obj , ch ) );

      if ( buffer_full() ) break;
    }
  }

  if ( !count )  send_to_buffer( "�|���o�{��ۦP�Ǹ������~�C\n\r" );
  print_buffer( ch );
  RETURN_NULL();
}

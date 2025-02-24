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
#include "merc.h"

/* �s�񪫫~�άO�������D�{�� */
FUNCTION( do_deposit )
{
  char        arg1[ MAX_INPUT_LENGTH ];
  char        arg2[ MAX_INPUT_LENGTH ];
  char        buf[MAX_STRING_LENGTH];
  OBJ_DATA  * obj;
  CHAR_DATA * victim;
  int         money;
  int         fee;

  PUSH_FUNCTION( "do_deposit" );

  if ( IS_NPC( ch ) || !ch->in_room ) RETURN_NULL();

  argument = one_argument( argument , arg1 );

  if ( !arg1[0] )
  {
    send_to_char( "�A�Q�s�کάO�H��F��M�Ьd�� \e[1;32mdeposit\e[0m�C\n\r" , ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg1, "!donate" ) )
  {
    if ( ch->in_room->DepositMoney == FALSE )
    {
      act( "�藍�_�M$r�ä��O������T" , ch, NULL, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( !IS_NPC( ch ) && ch->level < level_limit )
    {
      act( "�b�A���ɯŨ�$i�ūe�M�Ф��n����ַݡC"
        , ch, &level_limit, NULL, TO_CHAR );

      RETURN_NULL();
    }

    if ( ch->donate > 0 )
    {
      send_to_char( "�b�A����٪����o�q�ɶ��M�Ф��n�˴I���T\n\r", ch );
      RETURN_NULL();
    }

    argument = one_argument( argument, arg1 );

    if ( arg1[0] == '\x0' )
    {
      send_to_char( "�A�n��b���֩O�S\n\r", ch );
      RETURN_NULL();
    }

    if ( !( victim = get_char_world( ch, arg1 ) ) )
    {
      act( "�䤣��A����H $2$T$0 �C�T", ch, NULL, arg1, TO_CHAR );
      RETURN_NULL();
    }

    if ( victim == ch )
    {
      send_to_char( "���S���d���ڡM�A�n��b���ۤv�S\n\r", ch );
      RETURN_NULL();
    }

    argument = one_argument( argument, arg1 );

    if ( arg1[0] == '\x0' || !is_number( arg1 ) )
    {
      act( "�A�n��h�ֱb��$N�O�S", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( ( money = atoi( arg1 ) ) <= 0 )
    {
      act( "�A�n�e�ȿ���$N�ܡS", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( money > ch->bank )
    {
      send_to_char( "�A���Y�̨S������h�C�T\n\r", ch );
      RETURN_NULL();
    }

    if ( ( fee = money / 10 ) < 10 )
    {
      send_to_char( "��b���B�Ӥ֤F�աT����O�����աT\n\r", ch );
      RETURN_NULL();
    }

    if ( over_scale( victim ) )
    {
      act( "�藍�_�M$N���겣�Ӥj�F�M�L�k�~��s���T", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    ch->bank     -= money;
    victim->bank += ( money - fee );

    act( "�A��F$i��Ȥl��$N�C", ch, &money, victim, TO_CHAR );

    chinese_number( money - fee, buf );
    act( "$n��F$t��Ȥl���A�C", ch, buf, victim, TO_VICT );

    RETURN_NULL();
  }

  /* ����r�i�H�O gold coin �άO coins �䤤�@�� */
  if ( !str_cmp( arg1 , "gold"  )
    || !str_cmp( arg1 , "coin"  )
    || !str_cmp( arg1 , "coins" ) )
  {
    if ( ch->in_room->DepositMoney == FALSE )
    {
      act( "$r�ä��O������T" , ch, NULL, NULL, TO_CHAR );
      RETURN_NULL();
    }

    one_argument( argument , arg2 );

    if ( !arg2[0] || !is_number( arg2 ) )
    {
      do_deposit( ch , "" );
      RETURN_NULL();
    }

    if ( ( money = atoi( arg2 ) ) <= 0 || money > ch->gold )
    {
      send_to_char( "�A�s���O�ȿ��ܡS\n\r" , ch );
      RETURN_NULL();
    }

    if ( over_scale( ch ) )
    {
      send_to_char( "�藍�_�M�A���겣�Ӥj�F�M�L�k�~��s���C\n\r", ch );
      RETURN_NULL();
    }

    gold_from_char( ch, money );
    ch->bank += money;

    act( "�A�s�F $i ��M�{�b�`�@�� $I ��b�����̡M���§A�����{�C"
      , ch, &money , &ch->bank, TO_CHAR );

    RETURN_NULL();
  }

  /* �s�񪫫~ */
    if ( ch->in_room->StoreRoom == FALSE )
  {
    act( "$r�ä��O�𧽳�T" , ch, NULL, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( !( obj = get_obj_carry( ch, arg1 ) ) )
  {
    act( "�A�S������ $2$T$0 �i�H�H���C", ch, NULL, arg1, TO_CHAR );
    RETURN_NULL();
  }

  if ( obj->wear_loc != WEAR_NONE )
  {
    act( "�A�٧�$p��b���W�C�M���⥦��F�a�C", ch, obj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  /* ���~�O�_�����r */
  if ( poison_char( ch, obj ) ) RETURN_NULL();

  if ( excess_filequota( ch ) )
  {
    send_to_char( "�藍�_�M�A���ɮפӤj�M�L�k����o�өR�O�T\n\r", ch );
    RETURN_NULL();
  }

  if ( !can_store( obj ) )
  {
    act( "��p�M�ڭ̤������A�s��$p�C" , ch, obj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( obj->address )
  {
    act( "$p�O�n�浹�O�H���H�M���ۤv�s�_�ӡC", ch, obj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( !can_drop_obj( ch, obj ) )
  {
    act( "�A����s��$p�C", ch, obj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  act( "�A�N$p$0�H��b���𧽡M���¥��{�C", ch, obj, NULL, TO_CHAR );

  message_driver( ch, obj, ACT_WHEN_DEPOSIT );

  obj_from_char( obj );
  obj_to_char_deposit( obj , ch );

  RETURN_NULL();
}

/* ��^�����άO���~ */
FUNCTION( do_withdraw )
{
  OBJ_DATA * obj;
  char       arg1[ MAX_INPUT_LENGTH ];
  char       arg2[ MAX_INPUT_LENGTH ];
  int        money;

  PUSH_FUNCTION( "do_withdraw" );

  if ( IS_NPC( ch ) || !ch->in_room ) RETURN_NULL();

  argument = one_argument( argument , arg1 );

  if ( !arg1[0] )
  {
    send_to_char( "�A�Q���کάO��^�F��M�Ьd�� withdraw�T\n\r" , ch );
    RETURN_NULL();
  }

  if ( !str_cmp( arg1 , "gold"  )
    || !str_cmp( arg1 , "coin"  )
    || !str_cmp( arg1 , "coins" ) )
  {
    if ( ch->trade == FALSE )
    {
      send_to_char( "�A���`�겣�ӥi�ȤF�M�ҥH�L�k����o�ӫ��O�T\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->in_room->DepositMoney == FALSE )
    {
      act( "$r�ä��O������C", ch, NULL, NULL, TO_CHAR );
      RETURN_NULL();
    }

    one_argument( argument , arg2 );

    if ( !arg2[0] || !is_number( arg2 ) )
    {
      do_withdraw( ch , "" );
      RETURN_NULL();
    }

    if ( ( money = atoi( arg2 ) ) <= 0 || money > ch->bank )
    {
      send_to_char( "�A�⪺�O�߿��ܡS\n\r" , ch );
      RETURN_NULL();
    }

    if ( over_scale( ch ) )
    {
      send_to_char( "�A���`�겣�w�F�W���M�L�k��X���ӡT\n\r", ch );
      RETURN_NULL();
    }

    gold_to_char( ch, money );
    ch->bank -= money;

    act( "�A�{�b�@�@�s�F$i��b�����̡M���§A�����{�C"
      , ch, &ch->bank, NULL, TO_CHAR );

    RETURN_NULL();
  }

  /* ��^���~ */
    if ( ch->in_room->StoreRoom == FALSE )
  {
    act( "$r�ä��O�𧽳�C", ch, NULL, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( !( obj = get_obj_deposit( ch, arg1 ) ) )
  {
    act( "�A�S������ $2$T$0 ��b�𧽡C", ch, NULL, arg1, TO_CHAR );
    RETURN_NULL();
  }

  /* ���~�O�_�����r */
  if ( poison_char( ch, obj ) ) RETURN_NULL();

  act( "�A�w�g��$p$0��^�F�M���¥��{�C", ch, obj, NULL, TO_CHAR );
  obj_from_char_deposit( obj );
  message_driver( ch, obj, ACT_WHEN_WITHDRAW );

  if ( ch->carry_number + 1 > can_carry_n( ch )
    ||  get_carry_weight( ch ) + get_obj_weight( obj ) > can_carry_w( ch ) )
  {
    act( "�A�L�k�I�t$p�M�ҥH�⥦��b�a�W�T", ch, obj, NULL, TO_CHAR );
    obj_to_room( obj, ch->in_room );
  }
  else
  {
    obj_to_char( obj , ch );
  }

  RETURN_NULL();
}

/* �ˬd�𧽩άO�����H�񪺪����άO���~ */
FUNCTION( do_check )
{
  OBJ_DATA * obj;
  bool       found;

  PUSH_FUNCTION( "do_check" );

  if ( IS_NPC( ch ) || !ch->in_room ) RETURN_NULL();

  if ( ch->in_room->DepositMoney )
  {
    act( "�A�{�b�@�@�s�F $i ��b�����̡M���§A�����{�C"
      , ch, &ch->bank, NULL, TO_CHAR );

    RETURN_NULL();
  }

  if ( ch->in_room->StoreRoom )
  {
    clear_buffer();
    for ( found = FALSE, obj = ch->deposit; obj; obj = obj->next_content )
    {
      found = TRUE;
      send_to_buffer( "�A��%s�H��b�𧽸̡C\n\r" , obj_name( ch, obj ) );
    }

    if ( !found ) send_to_buffer( "�A�èS�����󪺪F��b�o�̱H��C\n\r" );
    print_buffer( ch );

    RETURN_NULL();
  }

  act( "�藍�_�M$r�ä��O�����άO�𧽡C" , ch, NULL, NULL, TO_CHAR );
  RETURN_NULL();
}

/* ��F����H���H�񪺦a�� */
void obj_to_char_deposit( OBJ_DATA * obj, CHAR_DATA * ch )
{
  PUSH_FUNCTION( "obj_to_char_deposit" );

  obj->next_content = ch->deposit;
  ch->deposit       = obj;
  obj->deposit_by   = ch;
  obj->in_room      = NULL;
  obj->in_obj       = NULL;
  RETURN_NULL();
}

/* ��F��q�H�����s���̭簣 */
void obj_from_char_deposit( OBJ_DATA * obj )
{
  CHAR_DATA * ch;
  OBJ_DATA  * prev;

  PUSH_FUNCTION( "obj_from_char_deposit" );

  if ( !( ch = obj->deposit_by ) )
  {
    mudlog( LOG_DEBUG , "Obj_from_char_deposit: �H���O�Ū�." );
    RETURN_NULL();
  }

  if ( ch->deposit == obj )
  {
    ch->deposit = obj->next_content;
  }

  else
  {
    for ( prev = ch->deposit; prev; prev = prev->next_content )
    {
      if ( prev->next_content == obj )
      {
        prev->next_content = obj->next_content;
        break;
      }
    }

    if ( !prev )
    {
      mudlog( LOG_DEBUG , "Obj_from_char_deposit: ���~�èS���b��." );
    }
  }

  obj->deposit_by   = NULL;
  obj->next_content = NULL;
  RETURN_NULL();
}

OBJ_DATA * get_obj_deposit( CHAR_DATA * ch, char * argument )
{
  char       arg[MAX_INPUT_LENGTH];
  OBJ_DATA * obj;
  int        number;
  int        count = 0;

  PUSH_FUNCTION( "get_obj_deposit" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG , "get_obj_deposit: �H���O�Ū�." );
    RETURN( NULL );
  }

  number = number_argument( argument, arg );

  for ( obj = ch->deposit; obj; obj = obj->next_content )
    if ( is_name( arg, obj->name ) && ++count == number ) RETURN( obj );

  RETURN( NULL );
}

void obj_to_obj_dep( OBJ_DATA * obj, OBJ_DATA * obj_to )
{
  PUSH_FUNCTION( "obj_to_obj_dep" );

  if ( !obj || !obj_to )
  {
    mudlog( LOG_DEBUG , "obj_to_obj_dep: obj �� obj_to �O�Ū�" );
    RETURN_NULL();
  }

  obj->next_content = obj_to->contains;
  obj_to->contains  = obj;
  obj->in_obj       = obj_to;
  obj->in_room      = NULL;
  obj->deposit_by   = NULL;
  RETURN_NULL();
}

size_t deposit_count( CHAR_DATA * ch )
{
  int        count = 0;
  OBJ_DATA * pObj;

  PUSH_FUNCTION( "deposit_count" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "deposit_count: �ӷ������T." );
    RETURN( 0 );
  }

  for ( pObj = ch->deposit; pObj; pObj = pObj->next_content ) count++;
  RETURN( count );
}

void deposit_update( void )
{
  CHAR_DATA * ch;
  int         count;
  int         money;

  PUSH_FUNCTION( "deposit_update" );

  if ( DepositMoney <= 0 ) RETURN_NULL();

  for ( ch = char_list; ch; ch = ch->next )
  {
    if ( !verify_char( ch )
      || IS_NPC( ch )
      || ( count = deposit_count( ch ) ) <= 0 ) continue;

    money = count * DepositMoney;

    if ( ch->bank >= money )
    {
      ch->bank -= money;
    }

    else
    {
      gold_from_char( ch, UMIN( ch->gold, money - ch->bank ) );
      ch->bank = 0;
    }
  }

  RETURN_NULL();
}

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

#define LOTTO_BASE              2
#define LOTTO_MAX               6
#define LOTTO_PAPER_GOLD        100

int  LottoFirst   = 10;
int  LottoSecond  =  5;
int  LottoThird   =  2;
int  LottoForth   =  1;
int  lotto_count  =  0;
int  lotto_paper  =  LOTTO_PAPER_GOLD;
char nLotto[LOTTO_MAX];

int  lotto_check        args( ( OBJ_DATA * ) );

FUNCTION( do_lotto )
{
  char arg[MAX_INPUT_LENGTH];
  char buf[MAX_INPUT_LENGTH];
  int  loop;

  PUSH_FUNCTION( "do_lotto" );

  argument = one_argument( argument , arg );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  if ( !arg[0] )
  {
    clear_buffer();
    chinese_number( lotto_count, buf );
    send_to_buffer( "�����u��%s���v���ֳz���X�O\e[1;32m " , buf );

    for ( loop = 0; loop < LOTTO_MAX; loop++ )
      send_to_buffer( "%c", ( nLotto[loop] + '0' ) );

    chinese_number( lotto_paper, buf );
    send_to_buffer( "\e[0m�M�C���ֳz�ֳ������� %s ��C\n\r", buf );
    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg, "check" ) )
  {
    OBJ_DATA * pObj;
    int        check;

    one_argument( argument, arg );

    if ( !arg[0] )
    {
      send_to_char( "�A�n�����Ӫ��~�I���S\n\r", ch );
      RETURN_NULL();
    }

    if ( !( pObj = get_obj_carry( ch, arg ) ) )
    {
      act( "�A���W�S���o�˪����~ $2$T$0 ��C" , ch, NULL, arg, TO_CHAR );
      RETURN_NULL();
    }

    /* ���~�O�_�����r */
    if ( poison_char( ch, pObj ) ) RETURN_NULL();

    if ( ( check = lotto_check( pObj ) ) == ERRORCODE )
    {
      act( "�A��$p�èS�����ֳz���C", ch, pObj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    chinese_number( check, buf );

    /* �ھڲĴX�S���ӵ����~ */
    switch( check )
    {
    default:
      break;

    case 1:

      if ( LottoFirst > 0 )
      {
        act( "���H�A�M�A��$p���F��$T�S���C", ch, pObj, buf, TO_CHAR );
        chinese_number( add_firman( ch, LottoFirst ), buf );
        act( "�ֳz���|�e�A$t�i�K�����P�C", ch, buf, NULL, TO_CHAR );
        mudlog( LOG_INFO, "[LOTTO] %s�o��Ĥ@�S��.", ch->name );
      }

      break;

    case 2:

      if ( LottoSecond > 0 )
      {
        act( "���H�A�M�A��$p���F��$T�S���C", ch, pObj, buf, TO_CHAR );
        chinese_number( add_firman( ch, LottoSecond ), buf );
        act( "�ֳz���|�e�A$t�i�K�����P�C", ch, buf, NULL, TO_CHAR );
        mudlog( LOG_INFO, "[LOTTO] %s�o��ĤG�S��.", ch->name );
      }

      break;

    case 3:

      if ( LottoThird > 0 )
      {
        act( "���H�A�M�A��$p���F��$T�S���C", ch, pObj, buf, TO_CHAR );
        chinese_number( add_firman( ch, LottoThird ), buf );
        act( "�ֳz���|�e�A$t�i�K�����P�C", ch, buf, NULL, TO_CHAR );
        mudlog( LOG_INFO, "[LOTTO] %s�o��ĤT�S��.", ch->name );
      }

      break;

    case 4:

      if ( LottoForth > 0 )
      {
        act( "���H�A�M�A��$p���F��$T�S���C", ch, pObj, buf, TO_CHAR );
        chinese_number( add_firman( ch, LottoForth ), buf );
        act( "�ֳz���|�e�A$t�i�K�����P�C", ch, buf, NULL, TO_CHAR );
        mudlog( LOG_INFO, "[LOTTO] %s�o��ĥ|�S��.", ch->name );
      }

      break;
    }

    /* ���P����Ӫ����~ */
    extract_obj( pObj );

    RETURN_NULL();
  }

  /* ���ظ��X, �u�����ڥi�H */
  if ( !str_prefix( arg , "!reborn" ) && IS_GOD( ch ) )
  {
    mudlog( LOG_INFO, "[LOTTO] %s ���m�ֳz���X.", ch->name );
    send_to_char( "�A���s�]�w�ֳz���X�C\n\r" , ch );
    lotto_generate( -1 );
    do_lotto( ch , "" );
    RETURN_NULL();
  }

  /* �]�w���X, �u�����ڥi�H */
  if ( !str_prefix( arg, "!set" ) && IS_GOD( ch ) )
  {
    int  number;

    one_argument( argument , arg );

    if ( !is_number( arg ) || ( number = atoi( arg ) ) <= 0 )
    {
      send_to_char( "�]�w���X���~�C\n\r" , ch );
      RETURN_NULL();
    }

    mudlog( LOG_INFO, "[LOTTO] %s �]�w�ֳz���X.", ch->name );
    lotto_generate( number );
    do_lotto( ch, "" );
    RETURN_NULL();
  }

  send_to_char( "�Ьd�� lotto ���T���ϥΤ�k�C\n\r" , ch );
  RETURN_NULL();
}

/* �C�ѧ󴫼ֳz���X */
void lotto_update( void )
{
  DESCRIPTOR_DATA * man;
  CHAR_DATA       * ch;
  OBJ_DATA        * pObj;
  int               check;

  PUSH_FUNCTION( "lotto_update" );

  /* �����ͼֳz���X */
  lotto_generate( -1 );

  /* �e�X�ֳz�ֳ� */
  for ( man = descriptor_list; man; man = man->next )
  {
    if ( !verify_desc( man )
      || man->connected != CON_PLAYING
      || !( ch = man->character )
      || !IS_SET( ch->turn, PLR_LOTTO )
      || ch->gold < lotto_paper
      || ch->position != POS_STANDING ) continue;

    /* �����ֳ����� */
    if ( lotto_paper > 0 ) gold_from_char( ch, lotto_paper );

    /* �ˬd���W�����~ */
    for ( pObj = ch->carrying; pObj; pObj = pObj->next_content )
    {
      if ( pObj->wear_loc == WEAR_NONE
        && can_see_obj( ch, pObj )
        && ( check = lotto_check( pObj ) ) != ERRORCODE )
      {
        act( "$B�u�ֳz�ֳ��v �A��$p���F��$I�S���C", ch, pObj, &check, TO_CHAR );
      }
    }
  }

  RETURN_NULL();
}

/* ���ͤ@�ռֳz���X */
void lotto_generate( int number )
{
  int loop;

  PUSH_FUNCTION( "lotto_generate" );

  for ( loop = LOTTO_MAX - 1; loop >= 0; loop-- )
  {
    if ( number < 0 )
    {
      nLotto[loop] = ( char ) number_range( 0, 9 );
    }
    else
    {
      nLotto[loop] = number % 10;
      number      /= 10;
    }
  }

  lotto_count++;
  RETURN_NULL();
}

int lotto_check( OBJ_DATA * obj )
{
  int  loop;
  int  serial;
  int  check;

  PUSH_FUNCTION( "lotto_check" );

  /* �p�G���~�O�Ū�, �@�w�����~ */
  if ( !obj || ( serial = obj->serial.low ) == 0 ) RETURN ( ERRORCODE );

  /* ���~���� */
  switch( obj->item_type )
  {
  default:
    mudlog( LOG_DEBUG, "lotto_check: ���~���� %d ���~.", obj->item_type );
    RETURN( ERRORCODE );

  case ITEM_POTION:
  case ITEM_FURNITURE:
  case ITEM_TRASH:
  case ITEM_KEY:
  case ITEM_FOOD:
  case ITEM_MONEY:
  case ITEM_BOAT:
  case ITEM_CORPSE_NPC:
  case ITEM_CORPSE_PC:
  case ITEM_FOUNTAIN:
  case ITEM_PILL:
  case ITEM_LIGHT:
  case ITEM_ARROW:
  case ITEM_CONTAINER:
  case ITEM_DRINK_CON:

    RETURN( ERRORCODE );

  case ITEM_SCROLL:
  case ITEM_WAND:
  case ITEM_STAFF:
  case ITEM_WEAPON:
  case ITEM_TREASURE:
  case ITEM_ARMOR:
  case ITEM_MAGICSTONE:
  case ITEM_SPIRITJADE:
  case ITEM_MYSTERY:
  case ITEM_LETTER:
  case ITEM_ORE:
  case ITEM_GOLDMINE:
  case ITEM_SIGIL:
  case ITEM_VELLUM:
  case ITEM_FIREWORK:
  case ITEM_URN:
  case ITEM_BACKBAG:
    break;
  }

  /* �︹ */
  for ( check = 0, loop = LOTTO_MAX -1; loop >= 0; loop-- )
  {
    if ( nLotto[loop] == ( serial % 10 ) ) check++;
    else                                   break;
    serial /= 10;
  }

  /* �p�G�S���Ǧ^ -1, �����ܨ̧� 1, 2, 3 �Ǧ^ */
  RETURN( ( ( check > LOTTO_BASE ) ? ( LOTTO_MAX + 1 - check ) : ERRORCODE ) );
}

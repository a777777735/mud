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

size_t  hire_count      args( ( CHAR_DATA * ) );

int hire_cost( MOB_INDEX_DATA * pIndex, SHOP_DATA * pShop, int type )
{
  int cost;

  PUSH_FUNCTION( "hire_cost" );

  if ( !pIndex || !pShop )
  {
    mudlog( LOG_DEBUG, "hire_cost: �ʥF�ӷ�" );
    RETURN( ( ( ( unsigned ) - 1 ) / 2 ) ) ;
  }

  switch( type )
  {
  default:
    mudlog( LOG_DEBUG, "hire_cost: ���~���κA %d.", type );
    cost = ( ( unsigned ) - 1 ) / 2;
    break;

  case SHOP_BUY:

    cost = UMAX( 100, pIndex->level * 100 * pShop->profit_buy / 100 );
    break;

  case SHOP_SELL:

    cost = UMAX( 100, pIndex->level * 100 * pShop->profit_sell / 100 );
    break;
  }

  RETURN( cost );
}

FUNCTION( do_hire )
{
  char             arg[MAX_INPUT_LENGTH];
  char             name[MAX_STRING_LENGTH];
  CHAR_DATA      * keeper;
  CHAR_DATA      * mercenary;
  CHAR_DATA      * victim;
  SHOP_DATA      * pShop;
  MOB_INDEX_DATA * pIndex;
  int              loop;
  int              slot;
  int              cost;
  int              count;

  PUSH_FUNCTION( "do_hire" );

  if ( IS_NPC( ch ) || ch->hirer )
  {
    send_to_char( "�A�S���v�O���жħL�C\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->leader || ch->master )
  {
    send_to_char( "�A����L�����͡M�ҥH���න�жħL�C\n\r", ch );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {

    if ( !ch->mercenary )
    {
      send_to_char( "�A�S�����󪺶ħL�C\n\r", ch );
      RETURN_NULL();
    }

    clear_buffer();
    send_to_buffer(
      "\e[1;33;44m���� ���� �P ��  �R �O �k     �O ��     �O ��  �� "
      "                        \e[0m\n\r" );

    count = 0;
    for ( victim = ch->mercenary; victim; victim = victim->mercenary )
    {
      send_to_buffer( "%3d. %4d %2s %4d/%4d %4d/%4d %4d/%4d %-20s\n\r"
        , ++count
        , victim->level
        , YESNO( ch->in_room == victim->in_room )
        , victim->hit, get_curr_hit( victim )
        , victim->mana, get_curr_mana( victim )
        , victim->move, get_curr_move( victim )
        , mob_name( ch, victim ) );
    }

    if ( count == 0 ) send_to_buffer( "�A�S�����󪺶ħL�C\n\r" );
    print_buffer( ch );
  }

  else if ( !str_prefix( arg, "!list" ) )
  {
    if ( !( keeper = find_keeper( ch, SHOP_MERCENARY ) )
      || !keeper->pIndexData
      || !( pShop = keeper->pIndexData->pShop ) ) RETURN_NULL();

    if ( ch->position == POS_FIGHTING )
    {
      send_to_char( "�A���b�԰��M�L�k���жħL�T\n\r", ch );
      RETURN_NULL();
    }

    clear_buffer();

    send_to_buffer( "%s�i�H���Ъ��ħL���R\n\r"
      "\e[1;33;44m���� ���� ��  �� �ٿ�                                   "
      "                        \e[0m\n\r"
      , mob_name( ch, keeper ) );

    for ( count = loop = 0; loop < MAX_TRADE; loop++ )
    {
      if ( pShop->buy_type[loop] <= 0
        || !( pIndex = get_mob_index( pShop->buy_type[loop] ) ) ) continue;

      send_to_buffer( " %2d.  %3d %6d %s(%s)\n\r"
        , ++count
        , pIndex->level
        , hire_cost( pIndex, pShop, SHOP_BUY )
        , pIndex->short_descr
        , pIndex->name );
    }
    print_buffer( ch );
  }

  else if ( !str_prefix( arg, "!dismiss" ) )
  {
    if ( !( keeper = find_keeper( ch, SHOP_MERCENARY ) )
      || !keeper->pIndexData
      || !( pShop = keeper->pIndexData->pShop ) ) RETURN_NULL();

    if ( ch->position == POS_FIGHTING )
    {
      send_to_char( "�A���b�԰��M�L�k�Ѷ��ħL�T\n\r", ch );
      RETURN_NULL();
    }

    one_argument( argument, arg );

    if ( arg[0] == '\x0' || !is_number( arg ) )
    {
      send_to_char( "�A�n�Ѷ����@��ħL�S\n\r", ch );
      RETURN_NULL();
    }

    count = atoi( arg );
    loop  = 0;

    for ( victim = ch->mercenary; victim; victim = victim->mercenary )
    {
      if ( ++loop == count )
      {
        if ( victim->in_room != ch->in_room )
        {
          send_to_char( "�A����ħL���b�o�̭C�C\n\r", ch );
          RETURN_NULL();
        }

        if ( victim->hirer != ch )
        {
          send_to_char( "���줣�O�A���ħL�C\n\r", ch );
          RETURN_NULL();
        }

        if ( victim->position != POS_STANDING )
        {
          act( "$N�����A�ثe���A�X�Ѷ��C", ch, NULL, victim, TO_CHAR );
          RETURN_NULL();
        }

        mercenary_from_char( victim, ch );
        act( "$N���A�O$n���ħL�F�C", ch, NULL, victim, TO_ALL );

        if ( !over_scale( ch ) )
          gold_to_char( ch, hire_cost( victim->pIndexData, pShop, SHOP_SELL ) );

        extract_char( victim, TRUE );
        RETURN_NULL();
      }
    }

    send_to_char( "�A�S������ħL�C\n\r", ch );
  }

  else if ( is_number( arg ) )
  {
    if ( !( keeper = find_keeper( ch, SHOP_MERCENARY ) )
      || !keeper->pIndexData
      || !( pShop = keeper->pIndexData->pShop ) ) RETURN_NULL();

    if ( ch->position == POS_FIGHTING )
    {
      send_to_char( "�A���b�԰��M�L�k���жħL�T\n\r", ch );
      RETURN_NULL();
    }

    if ( ( loop = atoi( arg ) - 1 ) < 0
      || loop >= MAX_TRADE
      || ( slot = pShop->buy_type[loop] ) <= 0
      || !( pIndex = get_mob_index( slot ) ) )
    {
      send_to_char( "�A�����Ǥ���M�L�k���жħL�C\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->gold <= ( cost = hire_cost( pIndex, pShop, SHOP_BUY ) ) )
    {
      act( "�A���W������������$t($T)�C"
        , ch, pIndex->short_descr, pIndex->name, TO_CHAR );
      RETURN_NULL();
    }

    if ( hire_count( ch ) >= max_hire )
    {
      send_to_char( "�A��a���ħL�w�g�Ӧh�F�C\n\r", ch );
      RETURN_NULL();
    }

    mercenary = shape_mobile( pIndex, ch->in_room );
    act( "$n��F$i��Ȥl����$N�C", ch, &cost, mercenary, TO_ALL );
    gold_from_char( ch, cost );

    sprintf( name, "%s\e[0m(%s)���Ъ�%s"
      , ch->cname, ch->name, mob_name( NULL, mercenary ) );

    free_string( mercenary->byname );
    mercenary->byname = str_dup( name );

    mercenary->hirer     = ch;
    mercenary->mercenary = ch->mercenary;
    ch->mercenary        = mercenary;
  }

  else
  {
    send_to_char( "���O���~�M�Ьd�� hire �����T�Ϊk�T\n\r", ch );
  }

  RETURN_NULL();
}

size_t hire_count( CHAR_DATA * ch )
{
  size_t      count = 0;
  CHAR_DATA * victim;

  PUSH_FUNCTION( "hire_count" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "hire_count: �ӷ������T." );
    RETURN( 0 );
  }

  for ( victim = ch->mercenary; victim; victim = victim->mercenary )
    count++;

  RETURN( count );
}

void all_mercenary_from_char( CHAR_DATA * ch )
{
  CHAR_DATA * victim;
  CHAR_DATA * zVictim;

  PUSH_FUNCTION( "all_mercenary_from_char" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "all_mercenary_from_char: �ӷ����~." );
    RETURN_NULL();
  }

  for ( victim = ch->mercenary; victim; victim = zVictim )
  {
    zVictim           = victim->mercenary;
    victim->hirer     = NULL;
    victim->mercenary = NULL;

    /* �ץ��W�� */
    if ( IS_NPC( victim ) && victim->pIndexData )
    {
      free_string( victim->byname );
      victim->byname = str_dup( victim->pIndexData->short_descr );
    }
  }

  RETURN_NULL();
}

void mercenary_from_char( CHAR_DATA * victim, CHAR_DATA * ch )
{
  CHAR_DATA * prev;

  PUSH_FUNCTION( "mercenary_from_char" );

  if ( !ch || !victim || !ch->mercenary )
  {
    mudlog( LOG_DEBUG, "mercenary_from_char: �ӷ����~." );
    RETURN_NULL();
  }

  if ( victim == ch->mercenary )
  {
    ch->mercenary = victim->mercenary;
  }

  else
  {
    for ( prev = ch->mercenary; prev; prev = prev->mercenary )
    {
      if ( prev->mercenary == victim )
      {
        prev->mercenary = victim->mercenary;
        break;
      }
    }

    if ( !prev ) mudlog( LOG_DEBUG , "mercenary_from_char: ���o�{��H��." );
  }

  victim->boss = NULL;

  /* �ץ��W�� */
  if ( IS_NPC( victim ) && victim->pIndexData )
  {
    free_string( victim->byname );
    victim->byname = str_dup( victim->pIndexData->short_descr );
  }

  RETURN_NULL();
}

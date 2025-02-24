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

#define TICKET_TOTAL    10
#define TICKET_COST     100
#define TICKET_RESET    2

TICKET_DATA *   ticket_list = NULL;
ORDER_DATA  *   order_list  = NULL;
int             TicketTotal = TICKET_TOTAL;
int             TicketCost  = TICKET_COST;
int             TicketReset = TICKET_RESET;
int             ticketreset = TICKET_RESET;

void            set_ticket_default      args( ( TICKET_DATA * ) );

void generate_ticket( int howmany )
{
  TICKET_DATA * pTicket;
  ORDER_DATA  * pOrder;
  int           loop;
  int           count;

  PUSH_FUNCTION( "generate_tick" );

  if ( howmany < 0 )
    mudlog( LOG_ERR, "generate_ticket: �ӷ����X�z." );

  if ( !order_list )
    mudlog( LOG_ERR, "generate_ticket: �S�����������." );

  for ( count = 0, pOrder = order_list; pOrder; pOrder = pOrder->next )
    count += pOrder->howmany;

  if ( count > howmany )
    mudlog( LOG_ERR, "generate_ticket: �����ռƤj������ռ�." );

  for ( loop = 0; loop < howmany; loop++ )
  {
    set_ticket_default( pTicket = alloc_struct( STRUCT_TICKET_DATA ) );
    pTicket->next = ticket_list;
    ticket_list   = pTicket;
  }

  /* ���m�m�� */
  dice_ticket();

  RETURN_NULL();
}

void dice_ticket( void )
{
  TICKET_DATA * pTicket;
  TICKET_DATA * zTicket;
  ORDER_DATA  * pOrder;
  int           loop;
  int           count;

  PUSH_FUNCTION( "dice_ticket" );

  if ( !ticket_list || !order_list ) RETURN_NULL();

  /* �M����� */
  for ( pTicket = ticket_list; pTicket; pTicket = pTicket->next )
    set_ticket_default( pTicket );

  for ( pOrder = order_list; pOrder; pOrder = pOrder->next )
  {
    for ( loop = 0; loop < pOrder->howmany; loop++ )
    {
      zTicket = NULL;
      count   = 0;

      for ( pTicket = ticket_list; pTicket; pTicket = pTicket->next )
      {
        if ( !pTicket->order && ( number_range( 0, count++ ) == 0 ) )
          zTicket = pTicket;
      }

      if ( zTicket ) zTicket->order = pOrder;
    }
  }

  ticketreset = TicketReset;

  RETURN_NULL();
}

void set_ticket_default( TICKET_DATA * pTicket )
{
  PUSH_FUNCTION( "set_ticket_default" );

  pTicket->order = NULL;
  pTicket->sold  = FALSE;
  RETURN_NULL();
}

FUNCTION( do_chance )
{
  TICKET_DATA * pTicket;
  TICKET_DATA * zTicket;
  ORDER_DATA  * pOrder;
  CHAR_DATA   * victim;
  char        * msg;
  char          arg[MAX_INPUT_LENGTH];
  int           sold;
  int           count;
  int           loop;

  PUSH_FUNCTION( "do_chance" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  if ( !ticket_list || !order_list )
  {
    act( "�藍�_�M$t�S������m��c��T", ch, mud_name, NULL, TO_CHAR );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    clear_buffer();

    send_to_buffer(
      "�C�����͡R %d �աC\n\r"
      "�C�ջ����R %d ��C\n\r"
      "���m�ɶ��R %d �ѡC\n\r"
      , TicketTotal, TicketCost, TicketReset );

    for ( sold = 0, pTicket = ticket_list; pTicket; pTicket = pTicket->next )
      if ( pTicket->sold ) sold++;

    send_to_buffer(
      "�w�g��X�R %d �աC\n\r"
      "�Ѿl�m��R %d �աC\n\r"
      , sold, TicketTotal - sold );

    for ( pOrder = order_list; pOrder; pOrder = pOrder->next )
    {
      send_to_buffer( "�����]�w�R �� %d �S���M�� %3d �աM�C�� %5d ��C\n\r"
        , pOrder->order, pOrder->howmany, pOrder->gold );

      if ( buffer_full() ) break;
    }

    print_buffer( ch );
  }

  else if ( !str_prefix( arg, "setting" ) && IS_IMMORTAL( ch ) )
  {
    clear_buffer();

    send_to_buffer(
      "�C�����͡R %d �աC\n\r"
      "�C�ջ����R %d ��C\n\r"
      "���m�ɶ��R %d �ѡC\n\r"
      , TicketTotal, TicketCost, TicketReset );

    for ( sold = 0, pTicket = ticket_list; pTicket; pTicket = pTicket->next )
      if ( pTicket->sold ) sold++;

    send_to_buffer(
      "�w�g��X�R %d �աC\n\r"
      "�Ѿl�m��R %d �աC\n\r"
      , sold, TicketTotal - sold );

    for ( count = 0, pOrder = order_list; pOrder; pOrder = pOrder->next )
      count += pOrder->gold * pOrder->howmany;

    send_to_buffer( "�m��Q�q�R �`���B�R%d �`�����R%d\n\r"
      , TicketCost * TicketTotal , count );

    for ( pOrder = order_list; pOrder; pOrder = pOrder->next )
    {
      send_to_buffer( "�����]�w�R �� %d �S���M�� %3d �աM�C�� %5d ��C\n\r"
        , pOrder->order, pOrder->howmany, pOrder->gold );
    }

    for ( pOrder = order_list; pOrder; pOrder = pOrder->next )
    {
      send_to_buffer( "\n\r�� %d �S������:\n\r", pOrder->order );

      count = 1;
      loop = 0;
      for ( pTicket = ticket_list; pTicket; pTicket = pTicket->next, count++ )
      {
        if ( pTicket->order == pOrder )
        {
          if ( loop > 0 && ( loop % 15 ) == 0 ) send_to_buffer( "\n\r" );
          send_to_buffer( " %s%4d\e[0m"
            , pTicket->sold ? "\e[1;32m" : ""
            , count );
          loop++;
        }
      }

      send_to_buffer( "\n\r" );
    }
    print_buffer( ch );
  }

  else if ( !str_prefix( arg, "buy" ) )
  {
    if ( !IS_NPC( ch ) && ch->level < level_limit )
    {
      act( "�b�A�ɯŨ�$i�Ť��e�M�Ъ`�N�A�������T"
        , ch, &level_limit, NULL, TO_CHAR );

      RETURN_NULL();
    }

    if ( ch->donate > 0 )
    {
      send_to_char( "���٪��i���O���A���ӶR�m�骺�T\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->trade == FALSE )
    {
      send_to_char( "�A���`�겣�ӥi�ȤF�M�ҥH�L�k����o�ӫ��O�T\n\r", ch );
      RETURN_NULL();
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\x0' )
    {
      send_to_char( "�藍�_�M�A�n��ֶR�m��O�S\n\r", ch );
      RETURN_NULL();
    }

    if ( !( victim = get_char_room( ch, arg ) ) )
    {
      act( "�o�̨S�� $2$T$0�M�ҥH�A�R����m��C", ch, NULL, arg, TO_CHAR );
      RETURN_NULL();
    }

    if ( !IS_NPC( victim ) )
    {
      act( "�藍�_�M$N�O����m�骺�C", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( victim->position != POS_STANDING )
    {
      act( "$N���b���O�M���@�U�n�F�T", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( !can_see( victim, ch ) )
    {
      act( "$N�ݤ���A�C�C", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch->gold < TicketCost )
    {
      act( "�o�O���q�m��M�@�i�n$i��Ȥl�T", ch, &TicketCost, NULL, TO_CHAR );
      RETURN_NULL();
    }

    zTicket = NULL;
    loop    = 0;

    for ( pTicket = ticket_list; pTicket; pTicket = pTicket->next )
    {
      if ( !pTicket->sold && ( number_range( 0, loop++ ) == 0 ) )
        zTicket = pTicket;
    }

    if ( !zTicket )
    {
      send_to_char( "�藍�_�M�Ҧ����m�鳣�槹�F�M�U���Ц��T\n\r", ch );
      RETURN_NULL();
    }

    gold_from_char( ch, TicketCost );
    zTicket->sold = TRUE;

    act( "$N��F�@�i���q�m�鵹$n�M�ï�$e���j���T", ch, NULL, victim, TO_ALL );

    if ( !( pOrder = zTicket->order ) )
    {
      switch( number_range( 0, 3 ) )
      {
      default:
      case 0:
        msg = "�n�ߦ��n���M���´f�U";
        break;

      case 1:
        msg = "���§A���ߤ��q�M���A�U�����j��";
        break;

      case 2:
        msg = "�m��b��M�Ʊ�L�a";
        break;
      }

      act( "�A��@�F��M�W���g�ۡR�y$t�z", ch, msg, NULL, TO_CHAR );
      RETURN_NULL();
    }

    act( "���H�A�M�o�i�m�餤�F�� $2$i$0 �S���M�����O $3$I$0 ��Ȥl�C"
      , ch, &pOrder->order, &pOrder->gold, TO_CHAR );

    if ( !over_scale( ch ) ) gold_to_char( ch, pOrder->gold );
    else send_to_char( "�藍�_�M�A���`�겣�w�F�W���M�ҥH�S������T\n\r", ch );
  }

  else
  {
    send_to_char( "�A���y�k���~�M�Ьd�� chance ���Ϊk�T\n\r", ch );
  }

  RETURN_NULL();
}

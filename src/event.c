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

void event_update( void )
{
  EVENT_DATA * pEvent;

  PUSH_FUNCTION( "event_update" );

  for ( pEvent = event_list; pEvent; pEvent = pEvent->next )
  {
    if ( pEvent->chance > 0
      && !pEvent->lock
      && number_range( 1, 10000 ) <= pEvent->chance )
    {
      ( *pEvent->function ) ();
      pEvent->count++;
    }
  }

  RETURN_NULL();
}

FUNCTION( do_event )
{
  char         arg[MAX_INPUT_LENGTH];
  int          loop;
  int          count;
  EVENT_DATA * pEvent;

  PUSH_FUNCTION( "do_event" );

  if ( !event_list )
  {
    act( "�藍�_�M$t�S������Ĳ�o�ƥ�T", ch, mud_name, NULL, TO_CHAR );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    clear_buffer();

    send_to_buffer( "\e[1;33;44m ���� �W      ��          "
      "��  �� ��w ��  �v                                    \e[0m\n\r" );

    for ( loop = 1, pEvent = event_list; pEvent; pEvent = pEvent->next )
    {
      send_to_buffer( "%4d %-20s %6d  %s  %6d\n\r"
        , loop++, pEvent->title
        , pEvent->count, YESNO( pEvent->lock ), pEvent->chance );
    }

    print_buffer( ch );
  }

  else if ( !str_prefix( arg, "lock" ) )
  {
    if ( argument[0] == '\x0' || !is_number( argument ) )
    {
      send_to_char( "�A�n��w�X����Ĳ�o�O�S\n\r", ch );
      RETURN_NULL();
    }

    count = atoi( argument );
    for ( loop = 0, pEvent = event_list; pEvent; pEvent = pEvent->next )
      if ( ++loop == count ) break;

    if ( !pEvent )
    {
      send_to_char( "�藍�_�M�S������Ĳ�o�M��w�����T\n\r", ch );
      RETURN_NULL();
    }

    if ( pEvent->lock )
    {
      act( "�A�]�w�F$t�o��Ĳ�o�ʧ@�T", ch, pEvent->title, NULL, TO_CHAR );
      pEvent->lock = FALSE;
    }

    else
    {
      act( "�A��w�F$t�o��Ĳ�o�ʧ@�T", ch, pEvent->title, NULL, TO_CHAR );
      pEvent->lock = TRUE;
    }
  }

  else if ( !str_prefix( arg, "go" ) )
  {
    if ( argument[0] == '\x0' || !is_number( argument ) )
    {
      send_to_char( "�藍�_�M�A�n���Ĳ�o���@���ƥ�O�S\n\r", ch );
      RETURN_NULL();
    }

    count = atoi( argument );
    for ( loop = 0, pEvent = event_list; pEvent; pEvent = pEvent->next )
    {
      if ( ++loop == count )
      {
        if ( pEvent->lock )
        {
          act( "$t�o�Өƥ�w�g�Q��w�L�k����C"
            , ch, pEvent->title, NULL, TO_CHAR );
          RETURN_NULL();
        }

        act( "�A���Ĳ�o�F$t�o�Өƥ�T", ch, pEvent->title, NULL, TO_CHAR );
        ( *pEvent->function ) ();
        pEvent->count++;

        RETURN_NULL();
      }
    }

    send_to_char( "�藍�_�M�S������Ĳ�o�M��������T\n\r", ch );
  }

  else
  {
    send_to_char( "�A���Ѽƿ��~�M�Ьd�� event ���ϥΤ�k�T\n\r", ch );
  }
  RETURN_NULL();
}

void event_thunder( void )
{
  DESCRIPTOR_DATA * pDesc;
  DESCRIPTOR_DATA * zDesc;
  CHAR_DATA       * ch;
  char              buf[MAX_STRING_LENGTH];
  int               count;
  int               loop;

  PUSH_FUNCTION( "event_thunder" );

  zDesc = NULL;
  for ( count = 1, pDesc = descriptor_list; pDesc; pDesc = pDesc->next )
  {
    if ( verify_desc( pDesc )
      && pDesc->connected == CON_PLAYING
      && pDesc->character
      && pDesc->character->alignment <= ( -AlignRange + 100 )
      && can_damage( pDesc->character )
      && ( loop = number_range( 1, count++ ) ) == 1 )
      zDesc = pDesc;
  }

  if ( zDesc && ( ch = zDesc->character ) )
  {
    sprintf( buf, "��M�ѤW�@�}�p�n�����M����@�c�h�ݪ�%s"
      "��n�N�Q�o�}�p����T", mob_name( NULL, ch ) );

    talk_channel_2( buf, CHANNEL_BULLETIN, "�Ѩa" );

    send_to_char( "�֥s�A����@�c�h�ݡM�A�ݡM�Q�p����F�a�T\n\r", ch );

    ch->hit  = 1;
    ch->mana = 1;
    ch->move = 1;
  }

  RETURN_NULL();
}

void event_solar_eclipse( void )
{
  DESCRIPTOR_DATA * pDesc;
  CHAR_DATA       * ch;
  char              buf[MAX_STRING_LENGTH];
  int               value;

  PUSH_FUNCTION( "event_solar_eclipse" );

  str_cpy( buf, "��M�X�{�F��k�M�@�ɤH�ߴq�q�M�Ѹ�|�_�M�ܦh�H�N�b"
    "�@�}�L�W���ä����M��F���ֿ��T" );

  talk_channel_2( buf, CHANNEL_BULLETIN, "�Ѩa" );

  for ( pDesc = descriptor_list; pDesc; pDesc = pDesc->next )
  {
    if ( verify_desc( pDesc )
      && pDesc->connected == CON_PLAYING
      && ( ch = pDesc->character )
      && ch->level > level_limit
      && ch->gold >= 1000 )
    {
      if ( ( value = ch->gold * 10 / 100 ) > 0 )
      {
        act( "�A�򥢤F$i������C", ch, &value, NULL, TO_CHAR );
        gold_from_char( ch, value );
      }
    }
  }

  RETURN_NULL();
}

void event_month_eclipse( void )
{
  DESCRIPTOR_DATA * pDesc;
  CHAR_DATA       * ch;
  char              buf[MAX_STRING_LENGTH];
  int               value;

  PUSH_FUNCTION( "event_month_eclipse" );

  str_cpy( buf, "�ѥͲ��H�M���X�{�F��k�M�j�a�H�ߴq�q�M���֤H�����ڦӰk�M"
    "�̭����M���A�̿���������T" );

  talk_channel_2( buf, CHANNEL_BULLETIN, "�Ѩa" );

  for ( pDesc = descriptor_list; pDesc; pDesc = pDesc->next )
  {
    if ( verify_desc( pDesc )
      && pDesc->connected == CON_PLAYING
      && ( ch = pDesc->character )
      && ch->level >= level_limit
      && ch->bank >= 1000 )
    {
      if ( ( value = ch->bank * 10 / 100 ) > 0 )
      {
        act( "�A���������Y�򥢤F$i������C", ch, &value, NULL, TO_CHAR );
        ch->bank = UMAX( 0, ch->bank - value );
      }
    }
  }

  RETURN_NULL();
}

void event_drought( void )
{
  CHAR_DATA * ch;
  OBJ_DATA  * pObj;
  OBJ_DATA  * zObj;
  char        buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "event_drought" );

  str_cpy( buf, "�]���[�[�����B���M�U�a��a�W�ǡT" );
  talk_channel_2( buf, CHANNEL_BULLETIN, "�Ѩa" );

  for ( pObj = object_list; pObj; pObj = zObj )
  {
    zObj = pObj->next;

    if ( verify_obj( pObj )
      && pObj->item_type == ITEM_FOUNTAIN
      && !pObj->in_obj
      && !pObj->deposit_by
      && !pObj->carried_by
      && pObj != auction_info->obj
      && pObj->in_room )
    {
      sprintf( buf, "%s�]����a�Ӱ��\\�F�T\n\r", obj_name( NULL, pObj ) );
      sendmsg_to_someroom( buf, pObj->in_room );
      extract_obj( pObj );
    }
  }

  for ( ch = char_list; ch; ch = ch->next )
  {
    if ( verify_char( ch )
      && ch->desc
      && ch->desc->connected == CON_PLAYING
      && ch->in_room
      && ch->pcdata )
    {
      ch->pcdata->condition[COND_THIRST] = 0;
      send_to_char( "\e[33m�Aı�o�f������M���G�i�H�ܤU�@����C\e[0m\n\r"
        , ch );
    }
  }

  RETURN_NULL();
}

void event_locust( void )
{
  CHAR_DATA * ch;
  OBJ_DATA  * pObj;
  OBJ_DATA  * zObj;
  char        buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "event_locust" );

  str_cpy( buf, "�κ|���{�s�]�B�M�A�@�����S�J����`�M�U�a�a�`�W�ǡT" );
  talk_channel_2( buf, CHANNEL_BULLETIN, "�Ѩa" );

  for ( pObj = object_list; pObj; pObj = zObj )
  {
    zObj = pObj->next;

    if ( verify_obj( pObj )
      && pObj->item_type == ITEM_FOOD
      && !pObj->in_obj
      && !pObj->deposit_by
      && !pObj->carried_by
      && pObj != auction_info->obj
      && pObj->in_room )
    {
      sprintf( buf, "%s�]�����`�Ӯ����F�T\n\r", obj_name( NULL, pObj ) );
      sendmsg_to_someroom( buf, pObj->in_room );
      extract_obj( pObj );
    }
  }

  for ( ch = char_list; ch; ch = ch->next )
  {
    if ( verify_char( ch )
      && ch->desc
      && ch->desc->connected == CON_PLAYING
      && ch->in_room
      && ch->pcdata )
    {
      ch->pcdata->condition[COND_FULL] = 0;
      send_to_char( "\e[33m�B�P�P�@�n�M�A���{�l�j�F�C\e[0m\n\r", ch );
    }
  }

  RETURN_NULL();
}

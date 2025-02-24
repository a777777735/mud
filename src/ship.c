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

FUNCTION( do_ship )
{
  char        arg[MAX_INPUT_LENGTH];
  char        arg1[MAX_INPUT_LENGTH];
  char        arg2[MAX_INPUT_LENGTH];
  char        buf[MAX_STRING_LENGTH];
  char        buf1[MAX_STRING_LENGTH];
  SHIP_DATA * pShip;
  AREA_DATA * pArea;
  int         count;
  int         slot;

  PUSH_FUNCTION( "do_ship" );

  if ( !ship_list )
  {
    act( "$t�S�����󪺥�q�u���ơT", ch, mud_name, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( IS_NPC( ch ) ) RETURN_NULL();

  argument = one_argument( argument, arg );

  if ( !arg[0] )
  {
    clear_buffer();
    send_to_buffer( "\e[1;33;44m�X �o �� �� �X   �o   �I "
      "�� �� �� �� ��   ��   �a �O�� �� �� ��   ��   ��    �A\e[0m\n\r" );

    for ( count = 1, pShip = ship_list; pShip; pShip = pShip->next, count++ )
    {
      if ( pShip->lock ) continue;

      send_to_buffer( "%-11s %-12s %-11s %-12s %4d %2d:%2d %-17s\n\r"
        , ( pShip->starting
          && ( pArea = pShip->starting->area )
          && pArea->name ) ? pArea->name : "�����W�ϰ�"
        , pShip->starting && pShip->starting->name
          ? pShip->starting->name : "�����W�a��"
        , ( pShip->destination
          && ( pArea = pShip->destination->area )
          && pArea->name ) ? pArea->name : "�����W�ϰ�"
        , pShip->starting && pShip->destination->name
          ? pShip->destination->name : "�����W�a��"
        , pShip->cost, pShip->sailing, pShip->waiting
        , ship_status( pShip ) );
    }

    for ( pShip = ship_list; pShip; pShip = pShip->next )
    {
      if ( pShip->lock ) continue;

      else if ( pShip->starting == ch->in_room )
      {
        chinese_number( pShip->sailing, buf );
        chinese_number( pShip->cost, buf1 );

        send_to_buffer(
          "\n\r�A���b%s���X�o�I�W�M�����ت��a�O%s��%s�M���ɶ����p"
          "����%s�p�ɡT\n\r�ӥB��O���Q���Q�M�u��A%s��Ȥl�C\n\r"
          , pShip->name
          , pShip->destination && pShip->destination->area
            ? pShip->destination->area->name : "�Y�����W�ϰ�"
          , pShip->destination ? pShip->destination->name : "�Y�����W�a��"
          , buf, buf1 );

        if ( pShip->waiting_tick > 0 )
        {
          chinese_number( pShip->waiting_tick, buf );

          send_to_buffer( "���L�ЧA�ɧֵn�W��q�u��M�]��%s�N�b%s�p�ɫ�X�o�C\n\r"
            , pShip->name, buf );
        }
        else
        {
          chinese_number( pShip->sailing_tick, buf );

          send_to_buffer( "���O%s�w�g�X�o�F�M�U�Z��q�u��N�b%s�p�ɫ�X�o�C\n\r"
            , pShip->name, buf );
        }

        if ( pShip->pirate >= 1000 )
          send_to_buffer( "���L�M�����A�o�@�a�j�s�W�ǡM�^�U�A�p�߳�T\n\r" );

        else if ( pShip->pirate >= 500 )
          send_to_buffer( "ť���M�j�s���b�o�@�a�X�S�M�ЧA�n�h�[�p�ߡT\n\r" );

        else if ( pShip->pirate >= 100 )
          send_to_buffer( "�ǻD�M�o�̼h�X�{�L�j�s�M�ЧA�n�h�h�p�ߡT\n\r" );

        else if ( pShip->pirate >= 30 )
          send_to_buffer( "���M�j�s�ܤ֦b���X�S�M���O�٬O�p�ߪ��n�T\n\r" );

        else
          send_to_buffer( "�j�s���֦b�o�@�a�X�S�M���ӬO�������R�~��a�T\n\r" );
      }

      else if ( pShip->cabin == ch->in_room )
      {
        chinese_number( pShip->sailing, buf );

        send_to_buffer( "\n\r�A���b%s���ҪO�̡M�����ت��a�O%s��%s�M"
          "���ɶ�����%s�p�ɡT\n\r"
          , pShip->name
          , pShip->destination && pShip->destination->area
            ? pShip->destination->area->name : "�Y�����W�ϰ�"
          , pShip->destination ? pShip->destination->name : "�Y�����W�a��"
          , buf );

        if ( pShip->waiting_tick > 0 )
        {
          chinese_number( pShip->waiting_tick, buf );
          send_to_buffer( "�ЧA�y���𮧡M��q�u��N�b%s�p�ɫ�X�o�T���¡T\n\r"
            , buf );
        }
        else
        {
          chinese_number( pShip->sailing_tick, buf );
          send_to_buffer( "�ЧA�@�ߵ��ԡM��q�u��N�b%s�p�ɫ��F�ت��a�T���¡T\n\r"
            , buf );
        }

        if ( pShip->pirate >= 1000 )
          send_to_buffer( "���L�M�����A�o�@�a�j�s�W�ǡM�^�U�A�p�߳�T\n\r" );

        else if ( pShip->pirate >= 500 )
          send_to_buffer( "ť���M�j�s���b�o�@�a�X�S�M�ЧA�n�h�[�p�ߡT\n\r" );

        else if ( pShip->pirate >= 100 )
          send_to_buffer( "�ǻD�M�o�̼h�X�{�L�j�s�M�ЧA�n�h�h�p�ߡT\n\r" );

        else if ( pShip->pirate >= 30 )
          send_to_buffer( "���M�j�s�ܤ֦b���X�S�M���O�٬O�p�ߪ��n�T\n\r" );

        else
          send_to_buffer( "�j�s���֦b�o�@�a�X�S�M���ӬO�������R�~��a�T\n\r" );
      }
    }

    print_buffer( ch );
  }

  else if ( !str_cmp( arg, "on" ) )
  {
    for ( pShip = ship_list; pShip; pShip = pShip->next )
      if ( pShip->starting == ch->in_room ) break;

    if ( !pShip || IS_NPC( ch ) )
    {
      act( "�b$r�O�f�����q�u�㪺�M�A�Q�F�F�a�T", ch, NULL, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( pShip->lock )
    {
      send_to_char( "��q�u��Ѥj�]���h�F�M�o����q�u��S����k���A�A�ȤF�T\n\r", ch );
      RETURN_NULL();
    }

    if ( pShip->waiting_tick <= 0 || pShip->sailing_tick > 0 )
    {
      send_to_char( "�藍�_�M�o����q�u���٨S���a���C�T\n\r", ch );
      RETURN_NULL();
    }

    if ( ( reboot_time   > 0 && ( ( reboot_time   - current_time ) < 600 ) )
      || ( shutdown_time > 0 && ( ( shutdown_time - current_time ) < 600 ) ) )
    {
      act( "$t�Y�N�����M�ҥH�S����k�f�W��q�u��C", ch, mud_name, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch->gold < pShip->cost )
    {
      send_to_char( "���A�����������A�ӷf�a�M�o�̥i���O�O���Ʒ~�T\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->position != POS_STANDING )
    {
      send_to_char( "�A�٦b�����M�ݷ|�A�f�a�T\n\r", ch );
      RETURN_NULL();
    }

    if ( get_mount( ch ) )
    {
      send_to_char( "�藍�_�M�����@���M�Y�T�ʪ��J���T\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->spirit )
    {
      act( "�A�a��$N�O�|�~���L�ȫȪ��T", ch, NULL, ch->spirit, TO_CHAR );
      RETURN_NULL();
    }

    if ( ( IS_SET( ch->act, PLR_KILLER ) || IS_SET( ch->act, PLR_THIEF ) )
      && !IS_IMMORTAL( ch ) )
    {
      send_to_char( "�ھڤW�ųW�w�M�ڭ̬O�������A���窺�T\n\r", ch );
      RETURN_NULL();
    }

    /* ���� */
    gold_from_char( ch, pShip->cost );
    pShip->count++;

    /* �h���H�� */
    act( "$n�I�F���M�n�W�F$t�C", ch, pShip->name, NULL, TO_ALL );

    char_from_room( ch );
    char_to_room( ch, pShip->cabin );
    act( "$n�I�F���M�w�w�a�q�ҪO���ݥX�{�C", ch, NULL, NULL, TO_ROOM );
    do_look( ch, "auto" );

    /* �M���l�ܬ����I */
    clear_trace( ch, TRUE );
  }

  else if ( !str_cmp( arg, "off" ) )
  {
    for ( pShip = ship_list; pShip; pShip = pShip->next )
      if ( pShip->cabin == ch->in_room ) break;

    if ( !pShip || IS_NPC( ch ) )
    {
      send_to_char( "�A�S�S���b��q�u��W�M������U�h�O�S\n\r", ch );
      RETURN_NULL();
    }

    if ( pShip->waiting_tick <= 0 )
    {
      send_to_char( "�藍�_�M��q�u��w�g�X�o�F�M�ҥH�S����k�U�h�F�T\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->position != POS_STANDING )
    {
      send_to_char( "�A�٦b�����M�ҥH�S����k�U�h��q�u����T\n\r", ch );
      RETURN_NULL();
    }

    if ( get_mount( ch ) )
    {
      send_to_char( "�ҪO���j�M�ҥH�ЧA���q�A�����M�W�U�ӧa�T\n\r", ch );
      RETURN_NULL();
    }

    /* �ٿ� */
    gold_to_char( ch, pShip->cost / 2 );
    pShip->count = UMAX( 0, pShip->count - 1 );

    /* �h���H�� */
    act( "��q�u��Ѥj�����ٵ�$n�M$n�w�w�a���U$t�F�T"
      , ch, pShip->name, NULL, TO_ALL );

    char_from_room( ch );
    char_to_room( ch, pShip->starting );
    act( "$n��ۥ]�]�M�q$t���F�U�ӡC", ch, pShip->name, NULL, TO_ROOM );
    do_look( ch, "auto" );

    /* �M���l�ܬ����I */
    clear_trace( ch, TRUE );
  }

  else if ( is_number( arg ) && IS_IMMORTAL( ch ) )
  {
    slot = atoi( arg );
    for ( count = 0, pShip = ship_list; pShip; pShip = pShip->next )
    {
      if ( ++count == slot )
      {
        print_to_char( ch,
          "�s��          �R%d\n\r"
          "��W��      �R%s\n\r"
          "�X�o�I�ж����X�R%d\n\r"
          "�ت��a�ж����X�R%d\n\r"
          "��ж����X  �R%d\n\r"
          "����ԭz    �R%s"
          "���O��      �R%d ��Ȥl\n\r"
          "�a���ԭz      �R%s"
          "�i���ԭz      �R%s"
          "���H��      �R%d\n\r"
          "�j�s���v      �R�U����%d\n\r"
          "�j�s�X�{����  �R%d\n\r"
          "�������v      �R�U����%d\n\r"
          "���ɶ�      �R%d\n\r"
          "�ݾ��ɶ�      �R%d\n\r"
          "�ثe���ɶ�  �R%d\n\r"
          "�ثe���ݮɶ�  �R%d\n\r"
          "�ثe���A      �R%s\n\r"
          "��w          �R%s\n\r"
          , count
          , pShip->name && *pShip->name ? pShip->name : "�q��"
          , pShip->starting    ? pShip->starting->vnum    : -1
          , pShip->destination ? pShip->destination->vnum : -1
          , pShip->cabin       ? pShip->cabin->vnum       : -1
          , pShip->description ? pShip->description : "�q��"
          , pShip->cost
          , pShip->msg_land     ? pShip->msg_land     : "�q��"
          , pShip->msg_entrance ? pShip->msg_entrance : "�q��"
          , pShip->count, pShip->pirate, pShip->pirate_count
          , pShip->delay, pShip->sailing, pShip->waiting
          , pShip->sailing_tick, pShip->waiting_tick
          , ship_status( pShip )
          , YESNO( pShip->lock ) );
        RETURN_NULL();
      }
    }

    send_to_char( "�S���o�ӽs������T\n\r", ch );
  }

  else if ( !str_prefix( arg, "info" ) && IS_IMMORTAL( ch ) )
  {
    clear_buffer();
    send_to_buffer( "\e[1;33;44m���� �W        �� �_�l�I �ت��a ��  �� "
       "��  �O �H  �� ��  �s ��  �� ��w\e[0m\n\r" );

    for ( count = 0, pShip = ship_list; pShip; pShip = pShip->next )
    {
      send_to_buffer( "%4d %12s %6d %6d %6d %6d %6d %6d %6d %s\n\r"
        , ++count
        , pShip->name
        , pShip->starting    ? pShip->starting->vnum    : -1
        , pShip->destination ? pShip->destination->vnum : -1
        , pShip->cabin       ? pShip->cabin->vnum       : -1
        , pShip->cost
        , pShip->count, pShip->pirate, pShip->pirate_count
        , YESNO( pShip->lock ) );
    }

    print_buffer( ch );
  }

  else if ( !str_prefix( arg, "lock" ) && IS_IMMORTAL( ch ) )
  {
    argument = one_argument( argument, arg );
    if ( !arg[0] || !is_number( arg ) )
    {
      send_to_char( "�A�n��w�άO�Ѱ���w���@����S\n\r", ch );
      RETURN_NULL();
    }

    slot = atoi( arg );

    for ( count = 0, pShip = ship_list; pShip; pShip = pShip->next )
    {
      if ( ++count == slot )
      {
        chinese_number( count, buf );
        pShip->lock = pShip->lock ? FALSE : TRUE;

        act( "�A$t��$T��������A�T"
          , ch, pShip->lock ? "��w" : "�Ѱ���w", buf, TO_CHAR );

        mudlog( LOG_WIZARD, "%s%s��%s������A."
          , ch->name, pShip->lock ? "��w" : "�Ѱ���w", buf );

        if ( !pShip->lock )
        {
          sendmsg_to_someroom( pShip->msg_entrance, pShip->starting );
          pShip->sailing_tick = -1;
          pShip->waiting_tick = pShip->waiting;
        }

        else
        {
          sendmsg_to_someroom( "�ڡT�D�F�M��Ѥj�]���h�F�M�o����Y�N�����T"
            , pShip->cabin );

          sendmsg_to_someroom( "��Ѥj�]���h�F�M�A�A�]�f����o����F�T"
            , pShip->starting );
        }

        RETURN_NULL();
      }
    }

    send_to_char( "�䤣��A�n������X�M�ʧ@�����T\n\r", ch );
  }

  else if ( !str_prefix( arg, "set" ) && IS_IMMORTAL( ch ) )
  {
    argument = one_argument( argument, arg  );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( !arg[0] || !arg1[0] || !arg2[0] )
    {
      send_to_char( "�A�n�����@��������ǳ]�w�S�Ьd�� ship �Ϊk\n\r", ch );
      RETURN_NULL();
    }

    if ( !is_number( arg ) )
    {
      send_to_char( "�藍�_�M������X�����O�Ʀr�T\n\r", ch );
      RETURN_NULL();
    }

    slot = atoi( arg );
    for ( count = 0, pShip = ship_list; pShip; pShip = pShip->next )
    {
      if ( ++count == slot )
      {
        chinese_number( slot, buf );
        if ( !str_prefix( arg1, "name" ) )
        {
          free_string( pShip->name );
          pShip->name = str_dup( arg2 );
          act( "�A����$t���W�٬�$T�C", ch, buf, arg2, TO_CHAR );
          mudlog( LOG_DEBUG, "%s����%s������W��", ch->name, buf );
          RETURN_NULL();
        }

        else if ( !str_prefix( arg1, "cost" ) )
        {
          if ( !is_number( arg2 ) )
          {
            send_to_char( "���O�Υ����O�Ʀr�T\n\r", ch );
            RETURN_NULL();
          }

          if ( ( slot = atoi( arg2 ) ) <= 0 || slot >= MAX_ASSET )
          {
            act( "���O�� $i ���X�z�T", ch, &slot, NULL, TO_CHAR );
            RETURN_NULL();
          }

          chinese_number( ( pShip->cost = slot ), buf1 );
          act( "�A����$t������O�ά�$T�C", ch, buf, buf1, TO_CHAR );
          mudlog( LOG_WIZARD, "%s����%s������O�ά�%s�C\n\r"
            , ch->name, buf, buf1 );

          RETURN_NULL();
        }

        else if ( !str_prefix( arg1, "pirate" ) )
        {
          if ( !is_number( arg2 ) )
          {
            send_to_char( "�J��j�s���v�����O�Ʀr�T\n\r", ch );
            RETURN_NULL();
          }

          if ( ( slot = atoi( arg2 ) ) < 0 || slot >= 10000 )
          {
            act( "�j�s�X�{���v $i ���X�z�T", ch, &slot, NULL, TO_CHAR );
            RETURN_NULL();
          }

          chinese_number( ( pShip->pirate = slot ), buf1 );
          act( "�A����$t����j�s���v���U����$T�C", ch, buf, buf1, TO_CHAR );
          mudlog( LOG_WIZARD, "%s����%s����j�s���v���U����%s�C\n\r"
            , ch->name, buf, buf1 );

          RETURN_NULL();
        }

        else if ( !str_prefix( arg1, "delay" ) )
        {
          if ( !is_number( arg2 ) )
          {
            send_to_char( "���������v�����O�Ʀr�T\n\r", ch );
            RETURN_NULL();
          }

          if ( ( slot = atoi( arg2 ) ) < 0 || slot >= 10000 )
          {
            act( "���������v $i ���X�z�T", ch, &slot, NULL, TO_CHAR );
            RETURN_NULL();
          }

          chinese_number( ( pShip->pirate = slot ), buf1 );
          act( "�A����$t������������v���U����$T�C", ch, buf, buf1, TO_CHAR );
          mudlog( LOG_WIZARD, "%s����%s������������v���U����%s�C\n\r"
            , ch->name, buf, buf1 );

          RETURN_NULL();
        }

        else if ( !str_prefix( arg1, "sailing" ) )
        {
          if ( !is_number( arg2 ) )
          {
            send_to_char( "���ɶ������O�Ʀr�T\n\r", ch );
            RETURN_NULL();
          }

          if ( ( slot = atoi( arg2 ) ) <= 0 )
          {
            act( "���ɶ� $i ���X�z�T", ch, &slot, NULL, TO_CHAR );
            RETURN_NULL();
          }

          chinese_number( ( pShip->sailing = slot ), buf1 );
          act( "�A����$t������ɶ���$T�p�ɡC", ch, buf, buf1, TO_CHAR );
          mudlog( LOG_WIZARD, "%s����%s������ɶ���%s�p�ɡC\n\r"
            , ch->name, buf, buf1 );

          RETURN_NULL();
        }

        else if ( !str_prefix( arg1, "waiting" ) )
        {
          if ( !is_number( arg2 ) )
          {
            send_to_char( "�ݾ��ɶ������O�Ʀr�T\n\r", ch );
            RETURN_NULL();
          }

          if ( ( slot = atoi( arg2 ) ) <= 0 )
          {
            act( "�ݾ��ɶ� $i ���X�z�T", ch, &slot, NULL, TO_CHAR );
            RETURN_NULL();
          }

          chinese_number( ( pShip->waiting = slot ), buf1 );
          act( "�A����$t����ݾ��ɶ���$T�p�ɡC", ch, buf, buf1, TO_CHAR );
          mudlog( LOG_WIZARD, "%s����%s����ݾ��ɶ���%s�p�ɡC\n\r"
            , ch->name, buf, buf1 );

          RETURN_NULL();
        }

        else if ( !str_prefix( arg1, "entrance" ) )
        {
          free_string( pShip->msg_entrance );
          pShip->msg_entrance = str_dup( arg2 );
          act( "�A����$t���i��ԭz��$T�C", ch, buf, arg2, TO_CHAR );
          mudlog( LOG_DEBUG, "%s����%s������i��ԭz", ch->name, buf );
          RETURN_NULL();
        }

        else if ( !str_prefix( arg1, "land" ) )
        {
          free_string( pShip->msg_land );
          pShip->msg_land = str_dup( arg2 );
          act( "�A����$t���a��ԭz��$T�C", ch, buf, arg2, TO_CHAR );
          mudlog( LOG_DEBUG, "%s����%s������a��ԭz", ch->name, buf );
          RETURN_NULL();
        }

        send_to_char( "�]�w���ﶵ���~�M�Ьd�� ship �Ϊk�T\n\r", ch );
        RETURN_NULL();
      }
    }

    send_to_char( "�䤣��A�n������X�M�ʧ@�����T\n\r", ch );
  }

  else
  {
    send_to_char( "�Ѽƿ��~�M�Ьd�� ship ���ϥΤ�k�T\n\r", ch );
  }

  RETURN_NULL();
}

void ship_update( void )
{
  SHIP_DATA * pShip;
  CHAR_DATA * ch;
  CHAR_DATA * ch_next;
  OBJ_DATA  * obj;
  OBJ_DATA  * obj_next;
  char        buf[MAX_STRING_LENGTH];
  char        buf1[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "ship_update" );

  if ( !ship_list ) RETURN_NULL();

  for ( pShip = ship_list; pShip; pShip = pShip->next )
  {
    if ( pShip->waiting <= 0 || pShip->sailing <= 0 )
    {
      mudlog( LOG_DEBUG, "ship_update: ���l�p�ɾ����~." );
      pShip->waiting = 2;
      pShip->sailing = 10;

      if ( pShip->lock )
      {
        pShip->sailing_tick = -1;
        pShip->waiting_tick = -1;
      }
      else
      {
        sendmsg_to_someroom( pShip->msg_entrance, pShip->starting );
        pShip->waiting_tick = pShip->waiting;
        pShip->sailing_tick = -1;
      }

      continue;
    }

    if ( pShip->lock )
    {
      if ( pShip->cabin )
      {
        mudlog( LOG_DEBUG, "ship_update: ��ж������T." );
      }

      else
      {
        for ( ch = pShip->cabin->people; ch; ch = ch_next )
        {
          ch_next = ch->next_in_room;

          if ( ch->fighting ) stop_fighting( ch, TRUE );

          if ( ch->position == POS_RESTING || ch->position == POS_SLEEPING )
          {
            send_to_char( "�ޡM���F�M�Ӱ_�ɤF�C\n\r" , ch );

            /* �p�G�O���� */
            if ( !is_affected( ch, SLOT_SLEEP ) ) ch->position = POS_STANDING;
          }

          send_to_char( "��Ѥj�����A���ͷN�F�M�u�a�T\n\r", ch );
          act( "$n�Q��Ѥj�q$t���F�U�h�C", ch, pShip->name, NULL, TO_ROOM );
          char_from_room( ch );
          char_to_room( ch, pShip->destination );

          act( "$n�Q��Ѥj�q$t�W���F�U�ӡC", ch, pShip->name, NULL, TO_ROOM );
          do_look( ch, "auto" );

          /* �M���l�ܬ����I */
          clear_trace( ch, TRUE );
        }

        pShip->waiting_tick = -1;
        pShip->sailing_tick = -1;
        continue;
      }
    }

    if ( ( pShip->waiting_tick < 0 && pShip->sailing_tick < 0 )
      || ( pShip->waiting_tick > 0 && pShip->sailing_tick > 0 ) )
    {
      mudlog( LOG_DEBUG, "ship_update: ��p�ɾ����~." );
      pShip->waiting_tick = pShip->waiting;
      pShip->sailing_tick = -1;
      continue;
    }

    if ( pShip->waiting_tick > 0 )
    {
      if ( --pShip->waiting_tick <= 0 )
      {
        sprintf( buf, "%s\e[0m�X�o�F�M�Юȫȧ�Ӧn��m���U�C\n\r���K�Y��u�~"
          "���A�������M%s\e[0m�ܸ۷P�§A�����{�T\n\r"
          , pShip->name, mud_name );

        sendmsg_to_someroom( buf, pShip->cabin );

        chinese_number( pShip->sailing, buf1 );
        sprintf( buf, "%s\e[0m�X�o�F�M�б��f�����Z��ȫȡM"
          "�A��%s�p�ɵ��U�@�Z�T\n\r", pShip->name, buf1 );

        sendmsg_to_someroom( buf, pShip->starting );

        pShip->sailing_tick = pShip->sailing;
        pShip->waiting_tick = -1;
      }
      continue;
    }

    if ( pShip->sailing_tick > 0 )
    {
      pShip->sailing_tick--;

      if ( pShip->delay > 0 && number_range( 1, 10000 ) <= pShip->delay )
      {
        sendmsg_to_someroom(
          "���B�Q���K�G�M���B���ӡM��i��S����k�p����F�ت��a�F�T\n\r"
          , pShip->cabin );

        pShip->sailing_tick++;
      }

      if ( pShip->sailing_tick <= 0 )
      {
        if ( !pShip->cabin )
        {
          mudlog( LOG_DEBUG, "ship_update: ��ж������T." );
        }

        else
        {
          if ( pShip->msg_land && *pShip->msg_land )
            sendmsg_to_someroom( pShip->msg_land, pShip->cabin );

          for ( ch = pShip->cabin->people; ch; ch = ch_next )
          {
            ch_next = ch->next_in_room;

            if ( ch->fighting ) stop_fighting( ch, TRUE );

            if ( ch->position == POS_RESTING || ch->position == POS_SLEEPING )
            {
              send_to_char( "�ޡM���F�M�Ӱ_�ɤF�C\n\r" , ch );

              /* �p�G�O���� */
              if ( !is_affected( ch, SLOT_SLEEP ) ) ch->position = POS_STANDING;
            }

            act( "$n�q$t���F�U�h�C", ch, pShip->name, NULL, TO_ROOM );
            char_from_room( ch );
            char_to_room( ch, pShip->destination );

            act( "$n�q$t�W���F�U�ӡC", ch, pShip->name, NULL, TO_ROOM );
            do_look( ch, "auto" );

            /* �M���l�ܬ����I */
            clear_trace( ch, TRUE );
          }
        }

        if ( pShip->msg_entrance && *pShip->msg_entrance )
          sendmsg_to_someroom( pShip->msg_entrance, pShip->starting );

        pShip->waiting_tick = pShip->waiting;
        pShip->sailing_tick = -1;
      }

      /* �j�s */
      else if ( pShip->sailing_tick == 1
        && pShip->pirate > 0
        && number_range( 1, 10000 ) <= pShip->pirate )
      {
        sendmsg_to_someroom(
          "���B�@�������K�߲�C�C�a�X�{�M�D�F�M�����N�O�ǻD���j�s�ܡS\n\r"
          "�u���@�L��ܯ�O����Ѥj�d�˦a�|�_�պX�M�V�L�����j�s�̧뭰�C\n\r"
          "���T�u�O�˷��M�X�~�ȹC���M�I��F�j�s�M��M���W�������O�Q�~�T�@�šC\n\r"
          "�������O�S�F�M�����n�ٯ�O��@���p�R�C���T�d���C�s�b�M���ȨS���N�C\n\r"
          "�g�L�@�f��h��M�@�������K�߲�S�C�C�a�����b�a���u�F�T\n\r"
          "�u�O���M�Ӧ����j�s�̡T�U���N���n�Q�ڵ��e��T\n\r\n\r", pShip->cabin );

        for ( ch = pShip->cabin->people; ch; ch = ch_next )
        {
          ch_next = ch->next_in_room;

          /* if ( IS_IMMORTAL( ch ) ) continue; */

          if ( ch->fighting )
          {
            stop_fighting( ch, TRUE );
            send_to_char( "�Ѥl�ӷm�T�M�A�٦b���[�S�䦺�ڡT\n\r", ch );
          }

          if ( ch->position == POS_RESTING || ch->position == POS_SLEEPING )
          {
            send_to_char( "�ޡM�j�s�m�T�M�_�Ө����a�T�������T\n\r" , ch );

            /* �p�G�O���� */
            if ( !is_affected( ch, SLOT_SLEEP ) ) ch->position = POS_STANDING;
          }

          /* �m�� */
          if ( ch->gold > 0 )
          {
            gold_from_char( ch, ch->gold );
            send_to_char( "�p�l�M�������X�ӧa�M�������M�Ѥl�n�w�F�T\n\r"
              , ch );
          }

          else
          {
            send_to_char( "��p�l�M��򨭤W�@������S���M�h�h�h�T"
              "��j�s����A�j�h�F\n\r", ch );
          }

          /* �m���W���F��, �����a�W */
          for ( obj = ch->carrying; obj; obj = obj_next )
          {
            obj_next = obj->next_content;

            if ( obj->wear_loc == WEAR_NONE
              && can_drop_obj( ch, obj )
              && !obj->address )
            {
              act( "�ޡM�A��$p�ڭ̥S�̭̭n�w�F�C", ch, obj, NULL, TO_CHAR );
              act( "$n��$p�Q�j�s�m���F�C", ch, obj, NULL, TO_ROOM );
              obj_from_char( obj );
              obj_to_room( obj, ch->in_room );
            }
          }
        }

        /* �M���ж��̭��Ҧ����F�� */
        for ( obj = pShip->cabin->contents; obj; obj = obj_next )
        {
          obj_next = obj->next_content;
          if ( obj->item_type != ITEM_CORPSE_PC ) extract_obj( obj );
        }

        sprintf( buf, "�}��%s%s��%s�D��j�s�~�T�@�šM��W���ȫȵL�@�ƧK�C"
          , pShip->destination && pShip->destination->area
            ? pShip->destination->area->name : "�Y�����W�ϰ�"
          , pShip->destination ? pShip->destination->name : "�Y�����W�a��"
          , pShip->name );

        talk_channel_2( buf, CHANNEL_BULLETIN, "�ֳ�" );
        pShip->pirate_count++;
      }

      continue;
    }
  }

  RETURN_NULL();
}

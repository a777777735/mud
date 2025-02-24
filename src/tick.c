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

#define MAX_TICK       1000
#define MIN_TICK          2
#define TICK_MULTIPLE     2

FUNCTION( do_tick )
{
  char              arg[MAX_INPUT_LENGTH];
  int               tick;
  DESCRIPTOR_DATA * man;
  DESCRIPTOR_DATA * pDesc;

  PUSH_FUNCTION( "do_tick" );

  if ( IS_NPC( ch ) || !( pDesc = ch->desc ) ) RETURN_NULL();

  if ( !tick_permit && !IS_IMMORTAL( ch ) )
  {
    send_to_char( "�藍�_�M�t�κ޲z�̧�p�ƾ������F�C\n\r" , ch );
    RETURN_NULL();
  }

  argument = one_argument( argument , arg );

  /* �p�G�S���Ѽƫh�O�d�� */
  if ( !arg[0] )
  {
    /* �|���]�w */
    if ( pDesc->tick < 0 )
    {
      send_to_char( "�A�|���]�w�A���p�ƾ��M�Ьd�� tick ���ϥΤ�k�C\n\r" , ch );
      RETURN_NULL();
    }

    /* �w�g�]�w�h�O�d�� */
    else
    {
      print_to_char( ch , "�A�]�w�b�C %d �Өt�ήɯ߷|�۰ʿ�J {%s}�C\n\r"
        , pDesc->tick / TICK_MULTIPLE
        , pDesc->tick_command );
      RETURN_NULL();
    }
  }

  /* �Y�Ĥ@�ӰѼƬO�Ʀr */
  if ( is_number( arg ) )
  {
    /* �ˬd��ƬO�_�X�k */
    if ( ( tick = atoi( arg ) ) < MIN_TICK || tick > MAX_TICK )
    {
      print_to_char( ch , "�p�ƾ����t�ήɯߥ������� %d �M %d �����C\n\r"
        , MIN_TICK , MAX_TICK );
      RETURN_NULL();
    }

    /* �ˬd�R�O�C */
    if ( !*argument )
    {
      send_to_char( "�A�S����J�p�ƾ����R�O�C�C\n\r" , ch );
      RETURN_NULL();
    }


    /* �]�w�p�ƾ����ɯߩM�R�O */
    argument[sizeof( pDesc->tick_command ) - 1 ] = '\x0';
    str_cpy( pDesc->tick_command, argument );
    pDesc->tick = tick * TICK_MULTIPLE;

    act( "�A�]�w�b�C$i�Өt�ήɯ߷|�۰ʿ�J $T�C", ch, &tick, argument, TO_CHAR );

    RETURN_NULL();
  }

  /* �M���p�ƾ� */
  else if ( !str_prefix( arg , "!clear" ) )
  {
    pDesc->tick            = ERRORCODE;
    pDesc->tick_count      = ERRORCODE;
    pDesc->tick_command[0] = '\x0';
    send_to_char( "�A��p�ƾ��M���B�����F�C\n\r" , ch );
    RETURN_NULL();
  }

  /* �����p�ƾ� */
  else if ( !str_prefix( arg , "!off" ) )
  {
    pDesc->tick_count = ERRORCODE;
    send_to_char( "�A��p�ƾ������F�C\n\r" , ch );
    RETURN_NULL();
  }

  /* ���}�p�ƾ� */
  else if ( !str_prefix( arg , "!on" ) )
  {
    if ( pDesc->tick == ERRORCODE || !pDesc->tick_command[0] )
    {
      send_to_char( "�A�|���]�w�A���p�ƾ��M�Ьd�� tick�C\n\r" , ch );
      RETURN_NULL();
    }

    pDesc->tick_count = pDesc->tick;
    print_to_char( ch , "�A�N�C %d �Өt�ήɯߦ۰ʿ�J %s�C\n\r"
      , pDesc->tick / TICK_MULTIPLE
      , pDesc->tick_command );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg , "!set" ) && IS_GOD( ch ) )
  {
    if ( tick_permit )
    {
      for ( man = descriptor_list; man; man = man->next )
      {
        if ( !verify_desc( man ) ) continue;

        if ( man->connected == CON_PLAYING && !IS_IMMORTAL( man->character ) )
        {
          man->tick            = ERRORCODE;
          man->tick_count      = ERRORCODE;
          man->tick_command[0] = '\x0';
        }
      }

      tick_permit = FALSE;
      send_to_char( "�A����骺�p�ƾ������F�C\n\r" , ch );
    }

    else
    {
      tick_permit = TRUE;
      send_to_char( "�A����骺�p�ƾ����}�F�C\n\r" , ch );
    }

    RETURN_NULL();
  }

  else
  {
    send_to_char( "�A��J���榡����M�Ьd�� tick�C\n\r" , ch );
    RETURN_NULL();
  }

  RETURN_NULL();
}

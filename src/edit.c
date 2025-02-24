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

void accept_edit( CHAR_DATA * ch, char * argument )
{
  int               loop;
  char              buf[MAX_EDITING_LENGTH + 10 ];
  char              buf1[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA * pDesc;

  PUSH_FUNCTION( "accept_edit" );

  if ( !argument[0] || ( !( pDesc = ch->desc ) ) ) RETURN_NULL();

  if ( !ch->stack ) ch->stack = str_dup( "" );
  smash_tilde( argument );

  if ( !str_cmp( argument, "." ) )
  {
    switch( pDesc->edit_mode )
    {
    default:
      break;

    case EDIT_NOTE:

      pDesc->edit_mode = EDIT_NONE;

      if ( ch->pnote )
      {
        free_string( ch->pnote->text );
        ch->pnote->text = str_dup( ch->stack );
      }

      process_stack( pDesc );

      free_string( ch->stack );
      ch->stack = str_dup( "" );
      break;

    case EDIT_VOTE:

      pDesc->edit_mode = EDIT_NONE;

      if ( ch->vote )
      {
        free_string( ch->vote->text );
        ch->vote->text = str_dup( ch->stack );
      }

      process_stack( pDesc );
      free_string( ch->stack );
      ch->stack = str_dup( "" );
      break;

    case EDIT_POST:

      pDesc->edit_mode = EDIT_NONE;

      free_string( ch->editing );
      ch->editing = str_dup( ch->stack );

      process_stack( pDesc );
      free_string( ch->stack );
      ch->stack = str_dup( "" );
      break;

    case EDIT_SELF_DESCRIPTION:

      pDesc->edit_mode = EDIT_NONE;

      free_string( ch->description );
      ch->description = str_dup( ch->stack );

      process_stack( pDesc );
      free_string( ch->stack );
      ch->stack = str_dup( "" );
      break;

    case EDIT_HOME_DESCR:

      pDesc->edit_mode = EDIT_NONE;

      if ( ch->in_room && owner_name( ch->in_room ) == ch )
      {
        if ( str_len( ch->stack ) <= 10 )
        {
          send_to_char( "�藍�_�M�A���ж��ԭz�ӵu�F�T\n\r", ch );
          ch->bank += HomeGoldDescr;
        }
        else
        {
          free_string( ch->in_room->description );
          ch->in_room->description = str_dup( ch->stack );

          if ( !write_new_room( ch->in_room ) )
          {
            mudlog( LOG_DEBUG, "create_new_room: �L�k�s�� %s."
              , ch->in_room->filename );
            send_to_char( "�藍�_�M�A���ж��L�k�s�ɡT\n\r", ch );
            RETURN_NULL();
          }
        }
      }

      process_stack( pDesc );
      free_string( ch->stack );
      ch->stack = str_dup( "" );
      break;

    case EDIT_NOTEPAD:

      pDesc->edit_mode = EDIT_NONE;

      if ( ch->pcdata )
      {
        for ( loop = 0; loop < MAX_NOTEPAD; loop++ )
        {
          if ( ch->pcdata->notepad[loop]
            && !*( ch->pcdata->notepad[loop] ) )
          {
            ch->pcdata->notepad[loop] = str_dup( ch->stack );
            break;
          }
        }
      }

      process_stack( pDesc );

      free_string( ch->stack );
      ch->stack = str_dup( "" );
      break;
    }
  }

  else if ( !str_cmp( argument, "@" ) )
  {
    pDesc->edit_mode = EDIT_NONE;

    process_stack( pDesc );

    free_string( ch->stack );
    ch->stack = str_dup( "" );

    send_to_char( "\n\r�s������T\n\r", ch );
  }

  else
  {
    ansi_transcribe( argument, buf1 );
    str_cpy( buf, ch->stack );

    if ( str_len( buf ) + str_len( buf1 ) <= MAX_EDITING_LENGTH - 10 )
    {
      str_cat( buf, buf1   );
      str_cat( buf, "\n\r" );

      free_string( ch->stack );
      ch->stack = str_dup( buf );
    }
  }

  RETURN_NULL();
}

void show_editor( CHAR_DATA * ch )
{
  char buf[MAX_STRING_LENGTH];
  int  mode;

  PUSH_FUNCTION( "show_editor" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "show_edit: �ʥF�ӷ�." );
    RETURN_NULL();
  }

  str_cpy( buf, "�i �d��²���s�边(v1.0)  �����s��п�J \e[1;32m.\e[0m"
    " �����s�� \e[1;32m@\e[0m �s�褺�e�R" );

  mode = ( ch->desc ) ? ch->desc->edit_mode : EDIT_NONE;

  switch( mode )
  {
  default:
  case EDIT_NONE:
    str_cat( buf, "\e[1;33m����\e[0m" );
    break;

  case EDIT_NOTE:
    str_cat( buf, "\e[1;33m�s��H�󤺮e\e[0m" );
    break;

  case EDIT_VOTE:
    str_cat( buf, "\e[1;33m�s��벼�y�z\e[0m" );
    break;

  case EDIT_POST:
    str_cat( buf, "\e[1;33m�s����o��峹\e[0m" );
    break;

  case EDIT_SELF_DESCRIPTION:
    str_cat( buf, "\e[1;33m�s��ۤv�y�z\e[0m" );
    break;

  case EDIT_NOTEPAD:
    str_cat( buf, "\e[1;33m�s��O�ƥ�\e[0m" );
    break;

  case EDIT_HOME_DESCR:
    str_cat( buf, "\e[1;33m�s��ж��ԭz\e[0m" );
    break;
  }

  str_cat( buf, " �j\n\r" );
  str_cat( buf, VERTICAL_LINE );

  if ( ch->desc ) write_to_descriptor( ch->desc, buf, 0 );
  RETURN_NULL();
}

bool is_edit( DESCRIPTOR_DATA * man )
{
  PUSH_FUNCTION( "is_edit" );

  if ( !man )
  {
    mudlog( LOG_DEBUG, "is_edit: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( man->connected != CON_PLAYING || man->edit_mode == EDIT_NONE )
    RETURN( FALSE );

  RETURN( TRUE );
}

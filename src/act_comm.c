/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

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
#include <unistd.h>
#include "merc.h"

/* �ϰ��� */
bool    is_note_to      args( ( CHAR_DATA * , NOTE_DATA * ) );
void    note_attach     args( ( CHAR_DATA * ) );
void    note_remove     args( ( CHAR_DATA * , NOTE_DATA * ) );
void    send_note_to_char args( ( CHAR_DATA *, NOTE_DATA * ) );
void    talk_channel    args( ( CHAR_DATA * , char * , int , const char * ) );
void    note_delete     args( ( NOTE_DATA * ) );
char *  transfer_messages args( ( char * ) );
void    show_joke       args(( CHAR_DATA *, JOKE_DATA * ) );
void    refresh_donate  args( ( void ) );
bool    can_donate      args( ( CHAR_DATA * ch, bool ) );

extern  int     name_in_club            args( ( char *, CLUB_DATA * ) );

#define MAX_SOCIAL_MESSAGE      256

struct   social_token
{
  char * token;
  char * message;
};

const struct social_token token [] =
{
  { "?"  , "�ðݦa"     },
  { ":)" , "�r�֦a"     },
  { "-)" , "�������P�a" },
  { ":(" , "�ˤߦa"     },
  { "-(" , "�d�˦a"     },
  { "?!" , "����a"     },
  { ":!" , "���Ħa"     },
  { ":>" , "����a"     },
  { "->" , "���I�a"     },
  { ";>" , "���c�a"     },
  { ":p" , "�}�����a"   },
  { ":P" , "�}�����a"   },
  { ":b" , "�}�����a"   },
  { ":~" , "�d�۲��\\"  },
  { ":O" , "��Y�a"     },
  { ":*" , "�L���a"     },
  { ":Q" , "�ե֦a"     },
  { ""   , ""           }
};

bool is_note_to( CHAR_DATA * ch, NOTE_DATA * pNote )
{
  PUSH_FUNCTION( "is_note_to" );

  if ( !ch || !pNote )
  {
    mudlog( LOG_DEBUG, "is_note_to: �ӷ��O�Ū�." );
    RETURN( FALSE );
  }

  if ( !ch->name || !pNote->sender || !pNote->to_list )
  {
    mudlog( LOG_DEBUG, "is_note_to: ����r��O�Ū�." );
    RETURN( FALSE );
  }

  /* ���q�H�� */
  if ( ( !str_cmp( ch->name , pNote->sender ) )
    || ( is_name( "all" , pNote->to_list    ) )
    || ( is_name( ch->name , pNote->to_list ) )
    || ( IS_IMMORTAL( ch ) && ( is_name( "immortal" , pNote->to_list ) ) ) )
    RETURN( TRUE );

  RETURN( FALSE );
}

void note_attach( CHAR_DATA * ch )
{
  NOTE_DATA * pNote;

  PUSH_FUNCTION( "note_attach" );

  if ( !ch || !ch->name )
  {
    mudlog( LOG_DEBUG, "note_attach: �ӷ������D." );
    RETURN_NULL();
  }

  if ( IS_NPC( ch ) || ch->pnote ) RETURN_NULL();

  pNote           = alloc_struct( STRUCT_NOTE_DATA );
  pNote->next     = NULL;
  pNote->sender   = str_dup( ch->name );
  pNote->date     = str_dup( "" );
  pNote->to_list  = str_dup( "" );
  pNote->subject  = str_dup( "" );
  pNote->text     = str_dup( "" );
  pNote->filename = str_dup( "" );
  ch->pnote       = pNote;
  RETURN_NULL();
}

void note_remove( CHAR_DATA * ch, NOTE_DATA * pNote )
{
  char        to_new[MAX_INPUT_LENGTH];
  char        to_one[MAX_INPUT_LENGTH];
  NOTE_DATA * prev;
  char      * to_list;

  PUSH_FUNCTION( "note_remove" );

  if ( !ch || !pNote || !pNote->to_list )
  {
    mudlog( LOG_DEBUG, "note_remove: �ӷ������D." );
    RETURN_NULL();
  }

  /* �إ߷s�� to_list */
  to_new[0] = '\x0';
  to_list   = pNote->to_list;

  while ( *to_list )
  {
    to_list = one_argument( to_list, to_one );

    if ( to_one[0] && str_cmp( ch->name, to_one ) )
    {
      if ( str_len( to_new ) + str_len( to_new ) > sizeof( to_new ) - 2 )
      {
        mudlog( LOG_DEBUG, "note_remove: �W�X�d��." );
        RETURN_NULL();
      }

      str_cat( to_new, " " );
      str_cat( to_new, to_one );
    }
  }

  if ( str_cmp( ch->name, pNote->sender ) && to_new[0] )
  {
    free_string( pNote->to_list );
    pNote->to_list = str_dup( to_new + 1 );
    RETURN_NULL();
  }

  if ( pNote == note_list )
  {
    note_list = pNote->next;
  }
  else
  {
    for ( prev = note_list; prev; prev = prev->next )
      if ( prev->next == pNote ) break;

    if ( !prev )
    {
      mudlog( LOG_DEBUG , "Note_remove: �S�o�{����R�������c." );
      RETURN_NULL();
    }
    prev->next = pNote->next;
  }

  if ( unlink( pNote->filename ) != 0 )
    mudlog( LOG_DEBUG, "remove_note: �L�k�����H�� %s.", pNote->filename );

  free_string( pNote->text     );
  free_string( pNote->subject  );
  free_string( pNote->to_list  );
  free_string( pNote->date     );
  free_string( pNote->sender   );
  free_string( pNote->filename );

  /* ����O���� */
  free_struct( pNote , STRUCT_NOTE_DATA );
  RETURN_NULL();
}

void send_note_to_char( CHAR_DATA * ch, NOTE_DATA * pNote )
{
  PUSH_FUNCTION( "send_note_to_char" );

  if ( !ch || !pNote )
  {
    mudlog( LOG_DEBUG, "send_note_to_char: �ӷ������T." );
    RETURN_NULL();
  }

  clear_buffer();

  send_to_buffer( "�H�H�̡R%s\n\r��  �D�R%s\n\r��  ���R%s\n\r"
    , pNote->sender, pNote->subject, pNote->date );

  if ( pNote->to_list && *pNote->to_list == '@' )
  {
    send_to_buffer( "�����H�R%s\n\r", pNote->to_list + 1 );
  }
  else
  {
    send_to_buffer( "���H�H�R%s\n\r", pNote->to_list );
  }

  send_to_buffer( "�O  �d�R%s\n\r", YESNO( pNote->mark ) );

  if ( IS_IMMORTAL( ch ) ) send_to_buffer( "��  �W�R%s\n\r"
    , pNote->filename );

  send_to_buffer( "��  �e�R\n\r%s%s%s\e[0m"
    , VERTICAL_LINE, pNote->text, VERTICAL_LINE );

  print_buffer( ch );
  RETURN_NULL();
}

bool write_note( NOTE_DATA * pNote )
{
  char   filename[MAX_FILE_LENGTH];
  FILE * pFile;

  PUSH_FUNCTION( "write_note" );

  if ( !pNote )
  {
    mudlog( LOG_DEBUG, "write_note: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( pNote->filename && *pNote->filename )
  {
    str_cpy( filename, pNote->filename );
  }
  else
  {
    sprintf( filename, "%s/%d.%s", note_dir, ( int ) current_time, note_ext );
    pNote->filename = str_dup( filename );
  }

  if ( ( pFile = FOPEN( filename, "w" ) ) )
  {
    fprintf( pFile, "Sender  %s~\n"     , pNote->sender           );
    fprintf( pFile, "Mark    %d\n"      , pNote->mark             );
    fprintf( pFile, "Date    %s~\n"     , pNote->date             );
    fprintf( pFile, "Stamp   %d\n"      ,  (int) pNote->date_stamp );
    fprintf( pFile, "To      %s~\n"     , pNote->to_list          );
    fprintf( pFile, "Subject %s~\n"     , pNote->subject          );
    fprintf( pFile, "Text\n%s~\n\nEnd\n", pNote->text             );

    FCLOSE( pFile );

    /* �����ɮצs���Ҧ� */
    set_file_mode( filename );
  }
  else
  {
    mudlog( LOG_DEBUG, "write_note: �L�k�}���ɮ� %s.", filename );
    RETURN( FALSE );
  }
  RETURN( TRUE );
}

FUNCTION( do_note )
{
  char        buf[MAX_STRING_LENGTH];
  char        arg[MAX_INPUT_LENGTH];
  NOTE_DATA * pnote;
  CHAR_DATA * man;
  int         vnum;
  int         anum;
  char      * strtime;
  bool        found;

  PUSH_FUNCTION( "do_note" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  argument = one_argument( argument, arg );
  smash_tilde( argument );

  if ( !arg[0] )
  {
    do_note( ch, "read" );
    RETURN_NULL();
  }

  if ( !str_cmp( arg , "new_note" ) )
  {
    for ( pnote = note_list; pnote; pnote = pnote->next )
    {
      if ( is_note_to( ch , pnote )
        && str_cmp( ch->name , pnote->sender )
        && ch->last_note < pnote->date_stamp )
      {
        send_to_char( "\e[1;32m�A����@�ʷs�H�M�Х� \e[1;32mnote\e[0m �Ӿ\\Ū�C"
                      "\e[0m\a\n\r" , ch );
        RETURN_NULL();
      }
    }
  }

  if ( !str_cmp( arg, "list" ) )
  {
    clear_buffer();

    for ( vnum = 0, pnote = note_list; pnote; pnote = pnote->next )
    {
      if ( is_note_to( ch, pnote ) )
      {
        send_to_buffer( "[%3d%s] %s�R%s\e[0m\n\r"
          , vnum++
          , ( pnote->date_stamp > ch->last_note
            && str_cmp( pnote->sender, ch->name ) )
              ? " \e[1;32mN\e[0m " : "   "
          , pnote->sender
          , pnote->subject );
      }

      if ( buffer_full() ) break;
    }

    if ( vnum == 0 ) send_to_buffer( "�t�Τ��ثe�S�����󵹧A���H��C\n\r" );
    print_buffer( ch );

    RETURN_NULL();
  }

  if ( !str_cmp( arg, "read" ) )
  {
    if ( !str_cmp( argument, "all" ) )
    {
      found = TRUE;
      anum  = 0;
    }

    else if ( !argument[0] || !str_prefix( argument, "next" ) )
    {
      for ( pnote = note_list; pnote; pnote = pnote->next )
      {
        if ( is_note_to( ch, pnote )
          && str_cmp( ch->name, pnote->sender )
          && ch->last_note < pnote->date_stamp )
        {
          send_note_to_char( ch, pnote );
          ch->last_note = UMAX( ch->last_note, pnote->date_stamp );
          RETURN_NULL();
        }
      }

      send_to_char( "�A�S����Ū���T���C\n\r", ch );
      RETURN_NULL();
    }

    else if ( is_number( argument ) )
    {
      found = FALSE;
      anum  = atoi( argument );
    }
    else
    {
      send_to_char( "�A�QŪ���g�T���S\n\r", ch );
      RETURN_NULL();
    }

    for ( vnum = 0, pnote = note_list; pnote; pnote = pnote->next )
    {
      if ( is_note_to( ch, pnote ) && ( vnum++ == anum || found ) )
      {
        send_note_to_char( ch, pnote );
        ch->last_note = UMAX( ch->last_note, pnote->date_stamp );
        RETURN_NULL();
      }
    }

    send_to_char( "�藍�_, �S���ʫH��C\n\r", ch );
    RETURN_NULL();
  }

  if ( !str_cmp( arg, "edit" ) )
  {
    if ( !ch->desc ) RETURN_NULL();

    note_attach( ch );

    ch->desc->edit_mode = EDIT_NOTE;
    show_editor( ch );

    free_string( ch->stack );
    ch->stack = str_dup( "" );
    RETURN_NULL();
  }

  if ( !str_cmp( arg, "subject" ) )
  {
    if ( !argument || !*argument )
    {
      send_to_char( "�A�����������D�C\n\r", ch );
      RETURN_NULL();
    }

    if ( check_chat_xname( ch, argument ) == TRUE ) RETURN_NULL();
    if ( str_len( argument ) > 40 )
    {
      send_to_char( "�藍�_�M�A�����D�Ӫ��F�M���Y�u�@�I�a�T\n\r", ch );
      RETURN_NULL();
    }

    note_attach( ch );

    ansi_transcribe( argument, buf );
    free_string( ch->pnote->subject );

    ch->pnote->subject = str_dup( buf );
    act( "�A�o�ʫH�����D���R�u$t�v", ch, buf, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( !str_cmp( arg, "to" ) )
  {
    if ( !argument || !*argument )
    {
      send_to_char( "�A������������H�C\n\r", ch );
      RETURN_NULL();
    }

    /* �Y���O�H���޲z�� */
    if ( str_cmp( argument, "immortal" ) && str_cmp( argument, "all" ) )
    {
      /* �O�_�H���ۤv */
      if ( !str_cmp( argument, ch->name ) )
      {
        send_to_char( "�A���M�H�H���ۤv�M�A�O���O�ӦY�ĤF�S\n\r", ch );
        RETURN_NULL();
      }

      /* �H������ */
      if ( argument[0] == '@' )
      {
        if ( argument[1] == '\x0' )
        {
          send_to_char( "�A���������������^��W�١T\n\r", ch );
          RETURN_NULL();
        }

        if ( clubname_lookup( argument + 1, CLUB_NAME ) == NULL )
        {
          act( "$t�̨S���o��������T", ch, mud_name, NULL, TO_CHAR );
          RETURN_NULL();
        }
      }

      /* �H���ӤH */
      else
      {
        if ( !is_exist( argument ) )
        {
          act( "$t�̨S���o�@���H����T", ch, mud_name, NULL, TO_CHAR );
          RETURN_NULL();
        }
      }
    }

    note_attach( ch );
    free_string( ch->pnote->to_list );
    ch->pnote->to_list = str_dup( argument );

    if ( !str_cmp( argument, "all" ) )
    {
      send_to_char( "�A�o�ʫH�N�|�H���Ҧ������a�T\n\r", ch );
    }

    else if ( !str_cmp( argument, "immortal" ) )
    {
      act( "�A�o�ʫH�N�|�H��$t�t�κ޲z���T", ch, mud_name, NULL, TO_CHAR );
    }

    else if ( argument[0] == '@' )
    {
      act( "�A�o�ʫH�N�|�H��$t�����T", ch, argument + 1, NULL, TO_CHAR );
    }

    else
    {
      act( "�A�o�ʫH�N�|�H�� $t �o�쪱�a�T", ch, argument, NULL, TO_CHAR );
    }

    RETURN_NULL();
  }

  if ( !str_cmp( arg, "clear" ) )
  {
    if ( !ch->pnote )
    {
      send_to_char( "�A��{�̡M���ӴN�S���H��M�ҥH�L�k�M���T\n\r", ch );
      RETURN_NULL();
    }

    free_string( ch->pnote->text     );
    free_string( ch->pnote->subject  );
    free_string( ch->pnote->to_list  );
    free_string( ch->pnote->date     );
    free_string( ch->pnote->sender   );
    free_string( ch->pnote->filename );

    /* ����p���� */
    free_struct( ch->pnote, STRUCT_NOTE_DATA );
    ch->pnote = NULL;

    send_to_char( "�����M���H���ơC\n\r", ch );
    RETURN_NULL();
  }

  if ( !str_cmp( arg, "show" ) )
  {
    if ( !ch->pnote )
    {
      send_to_char( "�A����{�̨S�����󪺫H��C\n\r", ch );
      RETURN_NULL();
    }

    send_note_to_char( ch, ch->pnote );
    RETURN_NULL();
  }

  if ( !str_cmp( arg, "post" ) || !str_prefix( arg, "send" ) )
  {
    if ( !ch->pnote )
    {
      send_to_char( "�A����{�̨èS�����󪺫H��C\n\r", ch );
      RETURN_NULL();
    }

    if ( !ch->pnote->to_list || !*ch->pnote->to_list )
    {
      send_to_char( "�A�������Ѧ��H�H���W�r�M\e[1;32mall\e[0m �O�H���Ҧ��H�C"
                    "\e[1;32mimmortal\e[0m �O�H���޲z�̡C\n\r" , ch );
      RETURN_NULL();
    }

    if ( !ch->pnote->subject || !*ch->pnote->subject )
    {
      send_to_char( "�A�ݭn���T���@�Ӽ��D�C\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->level < NoteLevel )
    {
      act( "�A�o���쵥��$i�ɤ~��g�H�T", ch, &NoteLevel, NULL, TO_CHAR );
      RETURN_NULL();
    }

    ch->pnote->next             = NULL;
    strtime                     = ctime( &current_time );
    strtime[str_len(strtime)-1] = '\x0';
    ch->pnote->date             = str_dup( strtime );
    ch->pnote->date_stamp       = current_time;

    if ( !note_list )
    {
      note_list = ch->pnote;
    }
    else
    {
      for ( pnote = note_list; pnote->next; pnote = pnote->next );
      pnote->next = ch->pnote;
    }

    pnote     = ch->pnote;
    ch->pnote = NULL;

    if ( !write_note( pnote ) )
    {
      send_to_char( "�藍�_�M�t�αH�H���ѡT\n\r", ch );
      RETURN_NULL();
    }

    send_to_char( "�����H�H����C\n\r", ch );

    /* �ߨ�e�T�������H�H */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) ) continue;

      if ( !IS_NPC( man ) && ch != man
        && ( !str_cmp( pnote->to_list , "all" )
          || !str_cmp( man->name , pnote->to_list ) ) )
         do_note( man, "new_note");
    }

    RETURN_NULL();
  }

  if ( !str_cmp( arg, "mark" ) && IS_IMMORTAL( ch ) )
  {
    if ( !argument || !*argument || !is_number( argument ) )
    {
      send_to_char( "�A�n�]�w���ʫH���O�d���A�O�S\n\r", ch );
      RETURN_NULL();
    }

    anum = atoi( argument );
    for ( vnum = 0, pnote = note_list; pnote; pnote = pnote->next )
    {
      if ( vnum++ == anum )
      {
        if ( pnote->mark )
        {
          act( "�A��$t�o�ʫH�Ѱ��O�d�T", ch, pnote->subject, NULL, TO_CHAR );
          pnote->mark = FALSE;
        }

        else
        {
          pnote->mark = TRUE;
          act( "�A��$t�o�ʫH�]�w���O�d�T", ch, pnote->subject, NULL, TO_CHAR );
        }

        write_note( pnote );
        RETURN_NULL();
      }
    }

    send_to_char( "�藍�_�M�S���o�ʫH�C\n\r", ch );
    RETURN_NULL();
  }

  if ( !str_cmp( arg, "remove" ) )
  {
    if ( !argument || !*argument || !is_number( argument ) )
    {
      send_to_char( "�藍�_�M�A�n�R�����ʫH�S\n\r", ch );
      RETURN_NULL();
    }

    anum = atoi( argument );
    for ( vnum = 0, pnote = note_list; pnote; pnote = pnote->next )
    {
      if ( is_note_to( ch, pnote ) && vnum++ == anum )
      {
        act( "�A�R���F$t�o�ʫH�C", ch, pnote->subject, NULL, TO_CHAR );
        note_remove( ch, pnote );
        RETURN_NULL();
      }
    }

    send_to_char( "�藍�_�M�S���o�ʫH�C\n\r", ch );
    RETURN_NULL();
  }

  send_to_char( "�п�J note /? �Ӭd�߱H�H���ϥΤ�k�C\n\r", ch );
  RETURN_NULL();
}

void notes_update( void )
{
  int         count;
  NOTE_DATA * pNote;
  NOTE_DATA * zNote;
  NOTE_DATA * prev;

  PUSH_FUNCTION( "notes_update" );

  for ( count = 0, pNote = note_list; pNote; pNote = pNote->next )
    if ( !pNote->mark ) count++;

  if ( ( count -= max_notes ) <= 0 ) RETURN_NULL();

  mudlog( LOG_INFO, "���էR���L�q���H��( %d ��).", count );

  for ( pNote = note_list; pNote && count > 0; pNote = zNote )
  {
    zNote = pNote->next;

    if ( pNote->mark ) continue;

    if ( pNote == note_list )
    {
      note_list = pNote->next;
    }

    else
    {
      for ( prev = note_list; prev; prev = prev->next )
        if ( prev->next == pNote ) break;

      if ( !prev )
      {
        mudlog( LOG_DEBUG , "Notes_update: �S�o�{����R�������c." );
        continue;
      }

      prev->next = pNote->next;
    }

    if ( unlink( pNote->filename ) != 0 )
      mudlog( LOG_DEBUG, "note_update: �L�k�����H�� %s.", pNote->filename );

    free_string( pNote->text     );
    free_string( pNote->subject  );
    free_string( pNote->to_list  );
    free_string( pNote->date     );
    free_string( pNote->sender   );
    free_string( pNote->filename );

    /* ����O���� */
    free_struct( pNote , STRUCT_NOTE_DATA );
  }

  RETURN_NULL();
}

void talk_channel( CHAR_DATA * ch, char * argument, int channel
                 , const char * verb )
{
  char              buf[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA * d;
  int               position;
  CHAR_DATA       * vch;
  char            * pStr;

  PUSH_FUNCTION( "talk_channel" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "talk_channel: �ʥF�ӷ�." );
    RETURN_NULL();
  }

  if ( !argument || !argument[0] ) RETURN_NULL();

  if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_SILENCE ) )
  {
    act( "�A�Q�T���F�M�ҥH���వ $t �o�Ӱʧ@�C", ch, verb, NULL, TO_CHAR );
    RETURN_NULL();
  }

  /* �����ť� */
  for ( pStr = argument + str_len( argument ) - 1; pStr > argument; pStr-- )
  {
    if ( *pStr != ' ' ) break;
    *pStr = '\x0';
  }

  REMOVE_BIT( ch->deaf, channel );

  position     = ch->position;
  ch->position = POS_STANDING;

  switch ( channel )
  {
  default:
    mudlog( LOG_DEBUG, "talk_channel: ���~���W�D %d.", channel );
    ch->position = position;
    RETURN_NULL();

  case CHANNEL_AUCTION:
    act( "$3�i���D�����j$0�A$t�R�u$T�v$0", ch, verb, argument, TO_CHAR );
    sprintf( buf, "$3�i���D�����j$n$0%s�R�u$t�v$0", verb );
    break;

  case CHANNEL_CHAT:
    act( "$6�i�K���W�D�j�A$t�R�u$3$T$6�v$0", ch, verb, argument, TO_CHAR );
    sprintf( buf, "$6�i�K���W�D�j$n$6%s�R�u$3$t$6�v$0", verb );
    break;

  case CHANNEL_IMMTALK:
    act( "$2�i�|ĳ�j�A$t�R�u$T$2�v$0", ch, verb, argument, TO_CHAR );
    sprintf( buf, "$2�i�|ĳ�j$n$2%s�R�u$2$t$2�v$0", verb );
    break;

  case CHANNEL_MUSIC:
    act( "$6�i��ţ�j�A$t�R�u$T$6�v$0", ch, verb, argument, TO_CHAR );
    sprintf( buf, "$6�i��ţ�j$n$6%s�R�u$t$6�v$0", verb );
    break;

  case CHANNEL_QUESTION:
    act( "$5�i�·~�Ѵb�j�A$t�R�u$T$5�v$0", ch, verb, argument, TO_CHAR );
    sprintf( buf, "$5�i�·~�Ѵb�j$n$5%s�R�u$t$5�v$0", verb );
    break;

  case CHANNEL_SHOUT:
    act( "$7�i���ԡj�A$t�R�u$T$7�v$0", ch, verb, argument, TO_CHAR );
    sprintf( buf, "$7�i���ԡj$n$7%s�R�u$t$7�v$0", verb );
    break;

  case CHANNEL_YELL:
    act( "$7�i���ԡj�A$t�R�u$T$7�v$0", ch, verb, argument, TO_CHAR );
    sprintf( buf, "$7�i���ԡj$n$7%s�R�u$t$7�v$0", verb );
    break;

  case CHANNEL_CLASS:
    act( "$3�i$0�P��$3�j�A$t�R�u$T$3�v$0", ch, verb, argument, TO_CHAR );
    sprintf( buf, "$3�i$0�P��$3�j$n$3%s�R�u$t$3�v$0", verb );
    break;

  case CHANNEL_CLUB:
    act( "$3�i�P�ӡj�A$t�R�u$T$3�v$0", ch, verb, argument, TO_CHAR );
    sprintf( buf, "$3�i�P�ӡj$n$3%s�R�u$t$3�v$0", verb );
    break;

  case CHANNEL_GROUP:
    act( "$3�i�ɯšj�A$t�R�u$T$3�v$0", ch, verb, argument, TO_CHAR );
    sprintf( buf, "$3�i�ɯšj$n$3%s�R�u$t$3�v$0", verb );
    break;


  case CHANNEL_SUICIDE:
    sprintf( buf, "$3�i�۵��j$n$3%s�R�u$t$3�v$0", verb );
    break;

  case CHANNEL_RUMOR:
    act( "$3�i�����j�A$t�R�u$T$3�v$0", ch, verb, argument, TO_CHAR );
    sprintf( buf, "$3�i�����j�Y�����W�H��$3%s�R�u$t$3�v$0", verb );
    break;
  }

  ch->position = position;

  for ( d = descriptor_list; d; d = d->next )
  {
    if ( verify_desc( d )
      && ( vch = d->character )
      && vch->in_room
      && d->connected == CON_PLAYING
      && vch != ch
      && !IS_SET( vch->deaf, channel ) )
    {
      if ( channel == CHANNEL_CLASS && !is_same_class( vch, ch ) ) continue;
      if ( channel == CHANNEL_CLUB  && !is_same_club( vch, ch )  ) continue;
      if ( channel == CHANNEL_IMMTALK && !IS_IMMORTAL( vch )     ) continue;
      if ( channel == CHANNEL_YELL && vch->in_room->area != ch->in_room->area )
        continue;

      if ( channel == CHANNEL_GROUP && abs( vch->level - ch->level ) > 5 )
        continue;

      position  = vch->position;
      if ( channel != CHANNEL_SHOUT && channel != CHANNEL_YELL )
        vch->position = POS_STANDING;

      act( smash_act_keyword( buf, "nesNESt" ), ch, argument, vch, TO_VICT );
      vch->position = position;
    }
  }
  RETURN_NULL();
}

void talk_channel_2( char * argument, int channel, const char * verb )
{
  DESCRIPTOR_DATA * d;
  CHAR_DATA       * vch;
  char            * pStr;
  char              buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "talk_channel_2" );

  if ( !argument || !argument[0] ) RETURN_NULL();

  /* �����ť� */
  for ( pStr = argument + str_len( argument ) - 1; pStr > argument; pStr-- )
  {
    if ( *pStr != ' ' ) break;
    *pStr = '\x0';
  }

  switch ( channel )
  {
  default:
    mudlog( LOG_DEBUG, "talk_channel: ���~���W�D %d.", channel );
    RETURN_NULL();

  case CHANNEL_WEATHER:
    sprintf( buf, "\e[1;33m�i�Ѯ�j�u%s\e[1;33m�v\e[0m\n\r", argument );
    break;

  case CHANNEL_GAMBLE:
    sprintf( buf, "\e[1;33m�i��դ��|�j�u%s\e[1;33m�v\e[0m\n\r", argument );
    break;

  /* �ֳ��W�D */
  case CHANNEL_BULLETIN:

    if ( !verb || !*verb )
    {
      mudlog( LOG_DEBUG, "talk_channel_2: �ӷ������T." );
      RETURN_NULL();
    }

    sprintf( buf, "\e[1;33m�i%s�j�u%s\e[1;33m�v\e[0m\n\r", verb, argument );
    break;

  case CHANNEL_PK:
    sprintf( buf, "\e[1;37m�i�ޢ١j�u%s\e[1;37m�v\e[0m\n\r", argument );
    break;

  case CHANNEL_NOTICE:
    sprintf( buf, "\e[1;32m�i�t�Τ��i�j�u%s\e[1;32m�v\e[0m\n\r", argument );
    break;
  }

  for ( d = descriptor_list; d; d = d->next )
  {
    if ( verify_desc( d )
      && ( vch = d->character )
      && vch->in_room
      && d->connected == CON_PLAYING
      && !IS_SET( vch->deaf, channel ) )
    {
      /* �Ѯ��W�D���������� */
      if ( channel == CHANNEL_WEATHER
        && ( !IS_AWAKE( vch ) || !IS_OUTSIDE( vch ) ) )
        continue;

      send_to_char( buf, vch );
    }
  }
  RETURN_NULL();
}

char * transfer_messages( char * input )
{
  char * location;
  int    loop;
  int    junk;

  PUSH_FUNCTION( "transfer_messages" );

  if ( !input || !*input ) RETURN( NULL );

  for ( loop = 0; token[loop].token[0]; loop++ )
  {
    if ( str_len( input ) > str_len( token[loop].token )
      && str_len( token[loop].message ) < MAX_SOCIAL_MESSAGE )
    {
      location = input + str_len( input ) - str_len( token[loop].token );

      if ( !str_cmp( token[loop].token, location ) )
      {
        *location = '\x0';
        junk = str_len( input ) - 1;

        while ( *(input + junk ) == ' ' && junk > 0 )
        {
          *( input + junk ) = '\x0';
          junk--;
        }
        RETURN( token[ loop ].message );
      }
    }
  }
  RETURN( NULL );
}

FUNCTION( do_semote )
{
  ROOM_INDEX_DATA * victim_org;
  CHAR_DATA       * victim;
  CHAR_DATA       * pChar;
  SOCIAL_DATA     * pSocial;
  char              soccmd[MAX_INPUT_LENGTH];
  char              arg[MAX_INPUT_LENGTH];
  char              buf[MAX_STRING_LENGTH];
  char              buf1[MAX_STRING_LENGTH];
  char              buf2[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "do_semote" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  if ( IS_SET( ch->act, PLR_SILENCE ) )
  {
    send_to_char( "�A�Q�T��ܤF�M�򯫥D�Ӷq�@�U�a�C\n\r" , ch );
    RETURN_NULL();
  }

  if ( IS_SET( ch->deaf, CHANNEL_SEMOTE ) )
  {
    send_to_char( "�A���ݥ����}�A���ʧ@�W�D�C\n\r", ch );
    RETURN_NULL();
  }

  if ( argument[0] == '\x0' )
  {
    send_to_char( "�A�n��ܤ���ʧ@�O�S\n\r", ch );
    RETURN_NULL();
  }

  if ( check_repeat( ch , argument ) == TRUE )
  {
    send_to_char( "�A���వ�ۦP���ʧ@�C\n\r" , ch );
    RETURN_NULL();
  }

  argument = one_argument( argument , soccmd );

  if ( !soccmd[0] )
  {
    send_to_char( "�A�n���������ʧ@�O�S\n\r" , ch );
    RETURN_NULL();
  }

  for ( pSocial = social_first; pSocial; pSocial = pSocial->next )
  {
    if ( soccmd[0] == pSocial->name[0]
      && !str_prefix( soccmd, pSocial->name ) ) break;
  }

  /* �Y���O����ʧ@ */
  if ( !pSocial )
  {
    ansi_transcribe( soccmd,   buf1 );
    ansi_transcribe( argument, buf2 );

    /* �ˬd����O�d�r */
    if ( check_chat_xname( ch, buf1 ) == TRUE ) RETURN_NULL();
    if ( check_chat_xname( ch, buf2 ) == TRUE ) RETURN_NULL();

    for ( victim = char_list; victim; victim = victim->next )
    {
      if ( !verify_char( victim )
        || IS_NPC( victim )
        || !victim->in_room
        || ( IS_SET( victim->deaf, CHANNEL_SEMOTE )
          && victim->in_room != ch->in_room ) ) continue;

      print_to_char( victim, "\e[1;32m�i�A�_�j %s%s %s\n\r\e[0m"
        , mob_name( victim, ch ), buf1, buf2 );
    }

    RETURN_NULL();
  }

  one_argument( argument , arg );

  if ( !arg[0] )
  {
    victim     = NULL;
    victim_org = NULL;
  }

  else
  {
    if ( !( victim = get_char_world( ch, arg ) )
      || !( victim_org = victim->in_room ) )
    {
      act( "�����H $2$T$0 ���b�o�@�ɩO�C", ch, NULL, arg, TO_CHAR );
      RETURN_NULL();
    }

    if ( IS_NPC( victim ) )
    {
      act( "���ள�D���a$N���H��C", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( IS_SET( victim->deaf, CHANNEL_SEMOTE ) )
    {
      act( "$N�S����ʧ@�W�D���}�C�T", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( is_affected( victim, SLOT_MASK ) )
    {
      send_to_char( "�A����H���G�䤣��M�_�ǡS\n\r", ch );
      RETURN_NULL();
    }

    if ( victim->in_room == ch->in_room && victim != ch )
    {
      act( "�A�M$N��b�P�@���Ф��M�@���ۤv�ݴN�n�F�T", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }
  }

  /* ��Ҧ����H�����X */
  for ( pChar = char_list; pChar; pChar = pChar->next )
  {
    if ( !verify_char( pChar )
      || IS_NPC( pChar )
      || pChar == ch
      || !pChar->in_room
      || IS_SET( pChar->deaf, CHANNEL_SEMOTE ) ) continue;

    pChar->social = pChar->in_room;
    char_from_room( pChar );
    char_to_room( pChar, ch->in_room );
  }

  /* �� victim �]���X */
  if ( victim && victim != ch )
  {
    char_from_room( victim );
    char_to_room( victim, ch->in_room );
  }

  /* �n�F, �{�b�Ҧ����H���b�P�@���ж��̤F */
  if ( !victim )
  {
    if ( pSocial->others_no_arg && *pSocial->others_no_arg )
    {
      sprintf( buf , "$2�i�A�_�j %s$0", pSocial->others_no_arg );
      act( smash_act_keyword( buf, "nesNES" ) , ch, NULL, victim, TO_ROOM );
    }

    if ( pSocial->char_no_arg && *pSocial->char_no_arg )
    {
      sprintf( buf , "$2�i�A�_�j %s$0", pSocial->char_no_arg );
      act( smash_act_keyword( buf, "nesNES" ) , ch, NULL, victim, TO_CHAR );
    }
  }

  else if ( victim == ch )
  {
    if ( pSocial->others_auto && *pSocial->others_auto )
    {
      sprintf( buf , "$2�i�A�_�j %s$0", pSocial->others_auto );
      act( smash_act_keyword( buf, "nesNES" ), ch, NULL, victim, TO_ROOM );
    }

    if ( pSocial->char_auto && *pSocial->char_auto )
    {
      sprintf( buf , "$2�i�A�_�j %s$0", pSocial->char_auto );
      act( smash_act_keyword( buf, "nesNES" ), ch, NULL, victim, TO_CHAR );
    }
  }

  else
  {
    if ( pSocial->others_found && *pSocial->others_found )
    {
      sprintf( buf , "$2�i�A�_�j %s$0", pSocial->others_found );
      act( smash_act_keyword( buf, "nesNES" ), ch, NULL, victim, TO_NOTVICT );
    }

    if ( pSocial->char_found && *pSocial->char_found )
    {
      sprintf( buf , "$2�i�A�_�j %s$0", pSocial->char_found );
      act( smash_act_keyword( buf, "nesNES" ), ch, NULL, victim, TO_CHAR );
    }

    if ( pSocial->vict_found && *pSocial->vict_found )
    {
      sprintf( buf , "$2�i�A�_�j %s$0", pSocial->vict_found );
      act( smash_act_keyword( buf, "nesNES" ), ch, NULL, victim, TO_VICT );
    }
  }

  /* ��Ҧ����H�k�� */
  for ( pChar = char_list; pChar; pChar = pChar->next )
  {
    if ( !verify_char( pChar )
      || IS_NPC( pChar )
      || pChar == ch
      || !pChar->social
      || IS_SET( pChar->deaf, CHANNEL_SEMOTE ) ) continue;

    char_from_room( pChar );
    char_to_room( pChar, pChar->social );
    pChar->social = NULL;
  }

  /* �]�� victim �k�� */
  if ( victim && victim != ch )
  {
    char_from_room( victim );
    char_to_room( victim , victim_org );
  }

  RETURN_NULL();
}

FUNCTION( do_chat )
{
  char   social_action[ MAX_SOCIAL_MESSAGE ];
  char   buf[MAX_STRING_LENGTH];
  char * messages;

  PUSH_FUNCTION( "do_chat" );

  /* �ˬd����O�d�r */
  if ( check_chat_xname( ch, argument ) == TRUE ) RETURN_NULL();

  if ( check_repeat( ch , argument ) == TRUE )
  {
    send_to_char( "�A�������ۦP���ܡC\n\r" , ch );
    RETURN_NULL();
  }

  messages = transfer_messages( argument );
  if ( messages ) str_cpy( social_action , messages );
  else            social_action[0] = '\x0';

  str_cat( social_action , "����" );

  filter_ansi( argument );
  ansi_transcribe( argument, buf );

  /* �O�_�M�w�İϪ��@�� */
  if ( IS_NPC( ch ) && !str_cmp( buf, LastMessage ) ) RETURN_NULL();

  talk_channel( ch, buf, CHANNEL_CHAT, social_action );

  /* ������ܦr���w�İ� */
  if ( IS_NPC( ch ) ) str_cpy( LastMessage, buf );

  RETURN_NULL();
}

FUNCTION( do_music )
{
  char   social_action[ MAX_SOCIAL_MESSAGE ];
  char   buf[MAX_STRING_LENGTH];
  char * messages;

  PUSH_FUNCTION( "do_music" );

  /* �ˬd����O�d�r */
  if ( check_chat_xname( ch, argument ) == TRUE ) RETURN_NULL();

  if ( check_repeat( ch , argument ) == TRUE )
  {
    send_to_char( "�A����۬ۦP���q�C\n\r" , ch );
    RETURN_NULL();
  }

  messages = transfer_messages( argument );
  if ( messages ) str_cpy( social_action , messages );
  else                    social_action[0] = '\x0';

  str_cat( social_action , "�H�ۭ��ְ_��۹D" );

  filter_ansi( argument );
  ansi_transcribe( argument, buf );

  /* �O�_�M�w�İϪ��@�� */
  if ( IS_NPC( ch ) && !str_cmp( buf, LastMessage ) ) RETURN_NULL();

  talk_channel( ch, buf, CHANNEL_MUSIC, social_action );

  /* ������ܦr���w�İ� */
  if ( IS_NPC( ch ) ) str_cpy( LastMessage, buf );

  RETURN_NULL();
}

/* �P¾�~���� */
FUNCTION( do_cchat )
{
  char   social_action[ MAX_SOCIAL_MESSAGE ];
  char   buf[MAX_STRING_LENGTH];
  char * messages;

  PUSH_FUNCTION( "do_cchat" );

  if ( !ch->class )
  {
    send_to_char( "�A�S��¾�~�M�n��ֻ��ܩO�S\n\r", ch );
    RETURN_NULL();
  }

  /* �ˬd����O�d�r */
  if ( check_chat_xname( ch, argument ) == TRUE ) RETURN_NULL();

  if ( check_repeat( ch , argument ) == TRUE )
  {
    send_to_char( "�A�������ۦP���ܡC\n\r" , ch );
    RETURN_NULL();
  }

  messages = transfer_messages( argument );
  if ( messages ) str_cpy( social_action , messages );
  else            social_action[0] = '\x0';

  str_cat( social_action , "���D" );

  filter_ansi( argument );
  ansi_transcribe( argument, buf );
  talk_channel( ch, buf, CHANNEL_CLASS, social_action );
  RETURN_NULL();
}

FUNCTION( do_gchat )
{
  char   social_action[ MAX_SOCIAL_MESSAGE ];
  char   buf[MAX_STRING_LENGTH];
  char * messages;

  PUSH_FUNCTION( "do_gchat" );

  /* �ˬd����O�d�r */
  if ( check_chat_xname( ch, argument ) == TRUE ) RETURN_NULL();

  if ( check_repeat( ch , argument ) == TRUE )
  {
    send_to_char( "�A�������ۦP���ܡC\n\r" , ch );
    RETURN_NULL();
  }

  messages = transfer_messages( argument );
  if ( messages ) str_cpy( social_action , messages );
  else            social_action[0] = '\x0';

  str_cat( social_action , "�I��" );

  filter_ansi( argument );
  ansi_transcribe( argument, buf );
  talk_channel( ch, buf, CHANNEL_GROUP, social_action );

  RETURN_NULL();
}

/* �P�������� */
FUNCTION( do_clubchat )
{
  char   social_action[ MAX_SOCIAL_MESSAGE ];
  char   buf[MAX_STRING_LENGTH];
  char * messages;

  PUSH_FUNCTION( "do_clubchat" );

  if ( !ch->club )
  {
    send_to_char( "�A�S�������M�n��ֻ��ܩO�S\n\r", ch );
    RETURN_NULL();
  }

  /* �ˬd����O�d�r */
  if ( check_chat_xname( ch, argument ) == TRUE ) RETURN_NULL();

  if ( check_repeat( ch , argument ) == TRUE )
  {
    send_to_char( "�A�������ۦP���ܡC\n\r" , ch );
    RETURN_NULL();
  }

  messages = transfer_messages( argument );
  if ( messages ) str_cpy( social_action , messages );
  else            social_action[0] = '\x0';

  str_cat( social_action , "���D" );

  filter_ansi( argument );
  ansi_transcribe( argument, buf );
  talk_channel( ch, buf, CHANNEL_CLUB, social_action );
  RETURN_NULL();
}

FUNCTION( do_question )
{
  char   social_action[ MAX_SOCIAL_MESSAGE ];
  char   buf[MAX_STRING_LENGTH];
  char * messages;

  PUSH_FUNCTION( "do_question" );

  /* �ˬd����O�d�r */
  if ( check_chat_xname( ch, argument ) == TRUE ) RETURN_NULL();

  if ( check_repeat( ch , argument ) == TRUE )
  {
    send_to_char( "�A����ݬۦP�����D�C\n\r" , ch );
    RETURN_NULL();
  }

  messages = transfer_messages( argument );
  if ( messages ) str_cpy( social_action , messages );
  else            social_action[0] = '\x0';

  str_cat( social_action , "�ôb�a�ݹD" );

  filter_ansi( argument );
  ansi_transcribe( argument, buf );
  talk_channel( ch, buf, CHANNEL_QUESTION, social_action );
  RETURN_NULL();
}

FUNCTION( do_answer )
{
  ANSWER_DATA   * pAnswer;
  QUESTION_DATA * pQuestion;
  char            arg[MAX_INPUT_LENGTH];
  char            social_action[ MAX_SOCIAL_MESSAGE ];
  char            buf[MAX_STRING_LENGTH];
  char          * messages;
  int             answer;
  int             random;

  PUSH_FUNCTION( "do_answer" );

  if ( !IS_NPC( ch )
    && ( pAnswer = ch->answer )
    && ( pQuestion = pAnswer->question ) )
  {
    argument = one_argument( argument, arg );

    if ( arg[0] == '\x0' || !is_number( arg ) )
    {
      send_answer( ch, ch );
      RETURN_NULL();
    }

    answer = atoi( arg ) - 1;
    if ( answer < 0
      || answer >= sizeof( pQuestion->question )
      || answer >= sizeof( pQuestion->answer   ) )
    {
      send_to_char( "�A�����פ��X�z�T\n\r", ch );
      RETURN_NULL();
    }

    random = pAnswer->random[answer];
    if ( !pQuestion->question[random]
      || !*pQuestion->question[random]
      || pQuestion->answer[random] == FALSE )
    {
      if ( ++pAnswer->false < QuestionFalse )
      {
        act( "�藍�_�M�A�����F�M�A����$i���M����$I���Y�{�w�������H�T"
          , ch, &pAnswer->false, &QuestionFalse, TO_CHAR );

        RETURN_NULL();
      }

      if ( jail_someone( NULL, ch, PlyPenalty, FALSE ) )
      {
        if ( ch->pcdata ) ch->pcdata->ply = 0;

        sprintf( buf, "%s�������H���åǡM�Q�t�ή��_�ӤF�T"
          , mob_name( NULL, ch ) );

        talk_channel_2( buf, CHANNEL_BULLETIN, "���i" );

        send_to_char( "�]���A�s�򵪿��M�ҥH�t�λ{�w�A�O�����H�C\n\r", ch );
        pQuestion->fail++;
        free_struct( pAnswer, STRUCT_ANSWER_DATA );
        ch->answer = NULL;
      }
      else
      {
        send_to_char( "�藍�_�M�t�λ{�w�A�������H���åǡT\n\r", ch );
      }
      RETURN_NULL();
    }

    send_to_char( "���ߧA�M�A����F�T\n\r", ch );

    if ( ch->pcdata ) ch->pcdata->ply = 0;

    free_struct( pAnswer, STRUCT_ANSWER_DATA );
    ch->answer = NULL;
    RETURN_NULL();
  }

  /* �ˬd����O�d�r */
  if ( check_chat_xname( ch, argument ) == TRUE ) RETURN_NULL();

  if ( check_repeat( ch , argument ) == TRUE )
  {
    send_to_char( "�A����^���ۦP�����סC\n\r" , ch );
    RETURN_NULL();
  }

  messages = transfer_messages( argument );
  if ( messages ) str_cpy( social_action , messages );
  else            social_action[0] = '\x0';

  str_cat( social_action , "�^����" );

  filter_ansi( argument );
  ansi_transcribe( argument, buf );
  talk_channel( ch, buf, CHANNEL_QUESTION, social_action );
  RETURN_NULL();
}

FUNCTION( do_shout )
{
  char   social_action[ MAX_SOCIAL_MESSAGE ];
  char   buf[MAX_STRING_LENGTH];
  char * messages;

  PUSH_FUNCTION( "do_shout" );

  /* �ˬd����O�d�r */
  if ( check_chat_xname( ch, argument ) == TRUE ) RETURN_NULL();

  if ( check_repeat( ch , argument ) == TRUE )
  {
    send_to_char( "�A����۬ۦP���ܡC\n\r" , ch );
    RETURN_NULL();
  }

  messages = transfer_messages( argument );
  if ( messages ) str_cpy( social_action , messages );
  else            social_action[0] = '\x0';

  str_cat( social_action , "�j��" );

  filter_ansi( argument );
  ansi_transcribe( argument, buf );

  /* �O�_�M�w�İϪ��@�� */
  if ( IS_NPC( ch ) && !str_cmp( buf, LastMessage ) ) RETURN_NULL();

  talk_channel( ch, buf, CHANNEL_SHOUT, social_action );
  WAIT_STATE( ch, 10 );

  /* ������ܦr���w�İ� */
  if ( IS_NPC( ch ) ) str_cpy( LastMessage, buf );

  RETURN_NULL();
}

FUNCTION( do_yell )
{
  char   social_action[ MAX_SOCIAL_MESSAGE ];
  char   buf[MAX_STRING_LENGTH];
  char * messages;

  PUSH_FUNCTION( "do_yell" );

  /* �ˬd����O�d�r */
  if ( check_chat_xname( ch, argument ) == TRUE ) RETURN_NULL();

  if ( check_repeat( ch , argument ) == TRUE )
  {
    send_to_char( "�A����۬ۦP���ܡC\n\r" , ch );
    RETURN_NULL();
  }

  messages = transfer_messages( argument );
  if ( messages ) str_cpy( social_action , messages );
  else            social_action[0] = '\x0';

  str_cat( social_action , "�y�s" );

  filter_ansi( argument );
  ansi_transcribe( argument, buf );

  /* �O�_�M�w�İϪ��@�� */
  if ( IS_NPC( ch ) && !str_cmp( buf, LastMessage ) ) RETURN_NULL();

  talk_channel( ch, buf, CHANNEL_YELL, social_action );

  /* ������ܦr���w�İ� */
  if ( IS_NPC( ch ) ) str_cpy( LastMessage, buf );

  RETURN_NULL();
}

FUNCTION( do_immtalk )
{
  char buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "do_immtalk" );

  /* �ˬd����O�d�r */
  if ( check_chat_xname( ch, argument ) == TRUE ) RETURN_NULL();

  if ( check_repeat( ch , argument ) == TRUE )
  {
    send_to_char( "�A���໡�ۦP���ܡC\n\r" , ch );
    RETURN_NULL();
  }

  filter_ansi( argument );
  ansi_transcribe( argument, buf );
  talk_channel( ch, buf, CHANNEL_IMMTALK, "���D" );
  RETURN_NULL();
}

FUNCTION( do_say )
{
  char buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "do_say" );

  /* �ˬd����O�d�r */
  if ( check_chat_xname( ch, argument ) == TRUE ) RETURN_NULL();

  if ( check_repeat( ch , argument ) == TRUE )
  {
    send_to_char( "�A���໡�ۦP���ܡC\n\r" , ch );
    RETURN_NULL();
  }

  filter_ansi( argument );

  ansi_transcribe( argument, buf );
  act( "$1$n$1���D�R$3$T$1$0", ch, NULL, buf, TO_ROOM );
  act( "$1�A���D�R$3$t$1$0"  , ch, buf, NULL, TO_CHAR );
  RETURN_NULL();
}

FUNCTION( do_phone )
{
  char        arg[MAX_INPUT_LENGTH];
  CHAR_DATA * victim;
  int         position;

  PUSH_FUNCTION( "do_phone" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  if ( IS_SET( ch->act , PLR_SILENCE ) )
  {
    send_to_char( "�A���L�ڳQ�ʦ�F�C\n\r" , ch );
    RETURN_NULL();
  }

  one_argument( argument , arg );

  if ( !( victim = get_char_world( ch , arg ) ) )
  {
    act( "�䤣��A�n�I�s���H�C $2$T$0�C" , ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  if ( IS_NPC( victim ) )
  {
    act( "$N�O�D���a�M�ҥH����I�s�T", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( ch == victim )
  {
    send_to_char( "�A�O���O�ƤF�M���M�I�s�ۤv�T\n\r", ch );
    RETURN_NULL();
  }

  if ( !IS_IMMORTAL( ch ) && is_badfriend( victim, ch->name ) )
  {
    act( "$N�������A������T���C", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( victim->desc && is_edit( victim->desc ) )
  {
    act( "$N���b�s���󤤡M�еy��A�I�s�T", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( victim->desc && set_descriptor_stack( victim->desc ) )
  {
    victim->desc->stack = FALSE;
    act( "$N���b�\\Ū��󤤡M�еy��A�I�s�T", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( IS_SET( victim->deaf, CHANNEL_PHONE ) )
  {
    act( "$N���I�s���S�����}�C�C", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  act( "�A�I�s$N�C", ch, NULL, victim, TO_CHAR );

  position         = victim->position;
  victim->position = POS_STANDING;
  act( "$n���b�I�s�A�C$B$B$B$B", ch, NULL, victim, TO_VICT );
  victim->position = position;

  RETURN_NULL();
}

FUNCTION( do_tell )
{
  char        arg[MAX_INPUT_LENGTH];
  char        buf[MAX_STRING_LENGTH];
  CHAR_DATA * victim;
  int         position;

  PUSH_FUNCTION( "do_tell" );

  if ( ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_SILENCE ) )
    || ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_NO_TELL ) ) )
  {
    send_to_char( "�L�{�b�S��kť��A���n���M�A�Q�ʤf�F�C\n\r", ch );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg );

  if ( !arg[0] || !argument[0] )
  {
    send_to_char( "�A�n�i�D�֤���S\n\r", ch );
    RETURN_NULL();
  }

  if ( !( victim = get_char_world( ch, arg ) )
    || ( IS_NPC( victim ) && victim->in_room != ch->in_room ) )
  {
    act( "�䤣��A����H $2$T$0�C", ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  if ( victim == ch )
  {
    send_to_char( "�A�b���ۻy�ܡS\n\r", ch );
    RETURN_NULL();
  }

  if ( !IS_IMMORTAL( ch ) && !IS_AWAKE( victim ) )
  {
    act( "$N�ثe�L�kť���A���n���C", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  /* �ˬd����O�d�r */
  if ( check_chat_xname( ch, argument ) == TRUE ) RETURN_NULL();

  if ( check_repeat( ch , argument ) == TRUE )
  {
    act( "�A����i�D$N�ۦP���ܡC" , ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( !IS_IMMORTAL( ch ) && is_badfriend( victim, ch->name ) )
  {
    act( "$N�������A������T���C", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  position         = victim->position;
  victim->position = POS_STANDING;

  if ( IS_NPC( victim ) ) delete_chat( ch );

  if ( victim->in_room && victim->in_room == ch->in_room )
  {
    ansi_transcribe( argument, buf );
    act( "$7�A���n���i�D$N$7�R�u$1$t$7�v$0", ch, buf, victim, TO_CHAR );
    act( "$7$n$7���n���i�D�A�R�u$1$t$7�v$0", ch, buf, victim, TO_VICT );
    act( "$4$n$4�M$N$4���b���Ѩp�y��T$0", ch, NULL, victim, TO_NOTVICT );

    if ( !IS_NPC( victim )
      && victim->desc
      && ( is_edit( victim->desc ) || set_descriptor_stack( victim->desc ) ) )
    {
      act( "$N�ثe���b�\\Ū�άO�s���󤤡T", ch, NULL, victim, TO_CHAR );
    }

  }

  else
  {
    if ( victim->in_room )
    {
      ansi_transcribe( argument, buf );
      if ( !IS_IMMORTAL( ch ) ) ch->move = UMAX( 0, ch->move - 1 );
      act( "$7�A�H�d���ǭ��i�D$N$7�R�u$1$t$7�v$0"
        , ch, buf, victim, TO_CHAR );
      act( "$7$n$7�ϥΤd���ǭ��i�D�A�R�u$1$t$7�v$0"
        , ch, buf, victim, TO_VICT );

      if ( !IS_NPC( victim )
        && victim->desc
        && ( is_edit( victim->desc ) || set_descriptor_stack( victim->desc ) ) )
      {
        act( "$N�ثe���b�\\Ū�άO�s���󤤡T", ch, NULL, victim, TO_CHAR );
      }

    }

    else
    {
      victim->position = position;
      victim->reply    = ch;
      act( "$N���b�����W���a��M�A�L�k��L�p���T", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }
  }

  victim->position = position;
  victim->reply    = ch;
  mprog_speech_trigger( argument , ch );

  RETURN_NULL();
}

FUNCTION( do_reply )
{
  CHAR_DATA * victim;
  char        buf[MAX_STRING_LENGTH];
  int         position;

  PUSH_FUNCTION( "do_reply" );

  if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_SILENCE ) )
  {
    send_to_char( "�L�{�b�S��kť��A���n���M�A�Q�ʤf�F�C\n\r", ch );
    RETURN_NULL();
  }

  if ( !( victim = ch->reply ) )
  {
    send_to_char( "�䤣��A�n�^�ܪ���H�C\n\r", ch );
    RETURN_NULL();
  }

  if ( !IS_IMMORTAL( ch ) && !IS_AWAKE( victim ) )
  {
    act( "$N�ثe�L�kť���A���n���C", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  /* �ˬd����O�d�r */
  if ( check_chat_xname( ch, argument ) == TRUE ) RETURN_NULL();

  if ( check_repeat( ch , argument ) == TRUE )
  {
    act( "�A����i�D$N�ۦP���ܡC", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( !IS_IMMORTAL( ch ) && is_badfriend( victim, ch->name ) )
  {
    act( "$N�������A������T���C", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  ansi_transcribe( argument, buf );

  act( "$7�A�^��$N$7���D�R�u$1$t$7�v$0", ch, buf, victim, TO_CHAR );
  position         = victim->position;
  victim->position = POS_STANDING;

  act( "$7$n$7�^���A�R�u$1$t$7�v$0", ch, buf, victim, TO_VICT );
  victim->position = position;
  victim->reply    = ch;

  RETURN_NULL();
}

FUNCTION( do_emote )
{
  char buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "do_emote" );

  if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_NO_EMOTE ) )
  {
    send_to_char( "�A�L�k�q�X�A���ʧ@�C\n\r", ch );
    RETURN_NULL();
  }

  /* �ˬd����O�d�r */
  if ( check_chat_xname( ch, argument ) == TRUE ) RETURN_NULL();

  if ( check_repeat( ch , argument ) == TRUE )
  {
    send_to_char( "�A���వ�ۦP���ʧ@�C\n\r" , ch );
    RETURN_NULL();
  }

  ansi_transcribe( argument, buf );
  act( "$n$T", ch, NULL, buf, TO_ALL );
  RETURN_NULL();
}

FUNCTION( do_bug )
{
  PUSH_FUNCTION( "do_bug" );

  if ( !IS_NPC( ch ) && argument[0] )
  {
    /* �ˬd����O�d�r */
    if ( check_chat_xname( ch, argument ) == TRUE ) RETURN_NULL();

    mudlog( LOG_DEBUG , "���a�^�����~ [%5d] %s: %s"
      , ch->in_room ? ch->in_room->vnum : 0, ch->name, argument );
    send_to_char( "���§A�Ҧ^�����t�ο��~�C\n\r", ch );
  }

  RETURN_NULL();
}

FUNCTION( do_idea )
{
  PUSH_FUNCTION( "do_idea" );

  if ( !IS_NPC( ch ) && argument[0] )
  {
    /* �ˬd����O�d�r */
    if ( check_chat_xname( ch, argument ) == TRUE ) RETURN_NULL();

    mudlog( LOG_IDEA , "[%5d] %s: %s"
      , ch->in_room ? ch->in_room->vnum : 0, ch->name, argument );
    send_to_char( "���§A���D�N�C\n\r", ch );
  }

  RETURN_NULL();
}

FUNCTION( do_typo )
{
  PUSH_FUNCTION( "do_typo" );

  if ( !IS_NPC( ch ) && argument[0] )
  {
    /* �ˬd����O�d�r */
    if ( check_chat_xname( ch, argument ) == TRUE ) RETURN_NULL();

    mudlog( LOG_TYPO , "[%5d] %s: %s"
      , ch->in_room ? ch->in_room->vnum : 0, ch->name, argument );
    send_to_char( "���§A�Ҧ^�������~�C\n\r", ch );
  }

  RETURN_NULL();
}

FUNCTION( do_retire )
{
  OBJ_DATA       * pObj;
  OBJ_DATA       * obj_next;
  OBJ_INDEX_DATA * pObjIndex;
  QUEST_DATA     * pQuest;
  QUEST_DATA     * quest_next;
  ENROLL_DATA    * pEnroll;
  ENROLL_DATA    * zEnroll;
  SKILL_DATA     * pSkill;
  AFFECT_DATA    * paf;
  AFFECT_DATA    * paf_next;
  ENEMY_DATA     * pEnemy;
  ENEMY_DATA     * zEnemy;
  char             arg[MAX_INPUT_LENGTH];
  char             buf[MAX_STRING_LENGTH];
  char           * pArg;
  char             cEnd;
  int              loop;

  PUSH_FUNCTION( "do_retire" );

  if ( IS_NPC( ch ) || !ch->pcdata ) RETURN_NULL();

  pArg = arg;
  while ( isSpace( ( int ) *argument ) ) argument++;

  cEnd = ' ';
  if ( *argument == '\'' || *argument == '"' ) cEnd = *argument++;

  while ( *argument )
  {
    if ( *argument == cEnd )
    {
      argument++;
      break;
    }
    *pArg++ = *argument++;
  }

  *pArg = '\x0';

  if ( !arg[0] )
  {
    send_to_char( "�A�������㥴�X retire <�K�X>�C\n\r", ch );
    RETURN_NULL();
  }

  if ( str_cmp( crypt( arg, ch->pcdata->pwd ), ch->pcdata->pwd ) )
  {
    send_to_char( "�K�X���X��M�A�A�Ҽ{�M���a�C\n\r", ch );
    RETURN_NULL();
  }

  if ( IS_IMMORTAL( ch ) )
  {
    send_to_char( "�A�O�t�κ޲z�̡M���୫�m�C\n\r", ch );
    RETURN_NULL();
  }

  if ( is_pk( ch ) )
  {
    send_to_char( "�A�������q�v�޳����}�~��T\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->position != POS_STANDING )
  {
    send_to_char( "�A����A���Ʊ������A���a�T\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->leader || ch->master )
  {
    send_to_char( "�A�٦���L���M�����}�L�̧a�T\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->donate > 0 )
  {
    send_to_char( "�קK���áM�b�A����٪����o�q�ɶ��M�Ф��n���m�T\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->jail > 0 )
  {
    send_to_char( "�A�٦b�Ѩc�̡M�����X�Ӥ���~�୫�m�T\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->failed > 0 )
  {
    send_to_char( "�A�٦b��L�V�M������L��~�୫�m�T\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->level <= 1 )
  {
    send_to_char( "�A�ثe�����ũM���m�S�����ˡT\n\r", ch );
    RETURN_NULL();
  }

  /* ���X���~ */
  for ( pObj = ch->carrying; pObj; pObj = obj_next )
  {
    obj_next = pObj->next_content;
    act( "�A���W��$p�û��a���}�o�ӥ@�ɤF�T", ch, pObj, NULL, TO_CHAR );
    obj_from_char( pObj );
    extract_obj( pObj );
  }

  ch->carrying = NULL;

  /* �����H�� */
  for ( pObj = ch->deposit; pObj; pObj = obj_next )
  {
    obj_next = pObj->next_content;
    act( "�A�H��$p�û��a���}�o�ӥ@�ɤF�T", ch, pObj, NULL, TO_CHAR );
    obj_from_char_deposit( pObj );
    extract_obj( pObj );
  }

  ch->deposit = NULL;

  /* �⨭�W���v�T������ */
  if ( ch->affected )
  {
    for ( paf = ch->affected; paf; paf = paf_next )
    {
      paf_next = paf->next;
      affect_remove( ch, paf );
    }

    send_to_char( "�����A���W�Ҧ��k�N�v�T�O���T\n\r", ch );
    ch->affected = NULL;
  }

  /* �ץ��P�� */
  extract_enable( ch );
  ch->enable = NULL;

  /* �ץ��������X�� */
  if ( ch->quest )
  {
    for ( pQuest = ch->quest; pQuest; pQuest = quest_next )
    {
      quest_next = pQuest->next;
      free_struct( pQuest, STRUCT_QUEST_DATA );
    }

    send_to_char( "�����A���W�Ҧ��������O���T\n\r", ch );
    ch->quest = NULL;
  }

  /* �M������ */
  if ( ch->enemy )
  {
    for ( pEnemy = ch->enemy; pEnemy; pEnemy = zEnemy )
    {
      zEnemy = pEnemy->next;

      free_string( pEnemy->city );
      free_struct( pEnemy, STRUCT_ENEMY_DATA );
    }

    send_to_char( "�����A���W�Ҧ����Ī��O���T\n\r", ch );
    ch->enemy = NULL;
  }

  /* �ץ��ޯ� */
  for ( loop = 0; loop < MAX_SKILL; loop++ )
  {
    if ( ch->skill[loop] > 0 )
    {
      if ( !( pSkill = get_skill( loop ) ) ) continue;

      if ( !pSkill->innate )
      {
        ch->skill[loop] = 0;
        act( "�A�ҾǪ�$w�w�g������ѤF�T", ch, pSkill, NULL, TO_CHAR );
      }
      else
      {
        ch->skill[loop] = UMIN( ch->skill[loop], pSkill->adeptation );
        act( "�A�ҾǪ�$w�w�g�S�����e������m�F�T", ch, pSkill, NULL, TO_CHAR );
      }
    }
  }

  /* �����O������ */
  if ( ch->enroll )
  {
    for ( pEnroll = ch->enroll; pEnroll; pEnroll = zEnroll )
    {
      zEnroll = pEnroll->next;
      free_struct( pEnroll, STRUCT_ENROLL_DATA );
    }

    ch->enroll = NULL;
    send_to_char( "�����A���W�Ҧ��O�����O���T\n\r", ch );
  }

  /* ���Ť@�����ݩ� */
  ch->level   = 1;
  ch->now_str = DefaultStr;
  ch->now_int = DefaultInt;
  ch->now_wis = DefaultWis;
  ch->now_dex = DefaultDex;
  ch->now_con = DefaultCon;

  ch->hit      = DefaultHit;
  ch->max_hit  = DefaultHit;
  ch->mod_hit  = 0;
  ch->mana     = DefaultMana;
  ch->max_mana = DefaultMana;
  ch->mod_mana = 0;
  ch->move     = DefaultMove;
  ch->max_move = DefaultMove;
  ch->mod_move = 0;

  /* �V�m�I�ƩM��L�򥻼ƭ� */
  ch->practice             = DefaultPractice;
  ch->limit                = 0;
  ch->nskill               = 0;
  ch->clublog              = FALSE;
  ch->hitroll              = 0;
  ch->damroll              = 0;
  ch->eq_capcity           = 50;
  ch->trust                = 0;
  ch->exp                  = 0;
  ch->alignment            = DefaultAlign;
  ch->wimpy                = 0;
  ch->armor                = 0;
  ch->firman               = 0;
  ch->pcdata->pills        = 0;
  ch->pcdata->dosage       = 0;
  ch->pcdata->steal_panity = 0;
  ch->pcdata->steal        = 0;
  ch->pcdata->murder       = 0;
  ch->pcdata->rechristen   = Rechristen;

  /* ���j���A */
  ch->pcdata->condition[COND_THIRST]  = 100;
  ch->pcdata->condition[COND_FULL]    = 100;

  /* �󥿭^���] */
  check_hero( ch->name );

  /* �]�w���Y */
  ch->class = class_demos;
  str_cpy( buf, ch->class->title );
  set_title( ch, buf );

  /* �M�� Recall */
  for ( loop = 0; loop < MAX_RECALL; loop++ )
  {
    if ( RecallPlace[loop] > 0 )
    ch->pcdata->recall[loop] = RecallPlace[loop];
  }

  /* �]�w Enable */
  for ( loop = 0; loop < MAX_SKILL; loop++ )
  {
    if ( ch->skill[loop] > 0
      && ( pSkill = get_skill( loop ) )
      && pSkill->enable )
    {
      set_enable( ch, pSkill );
    }
  }

  /* �˳ư򥻰t�� */
  for ( loop = 0; loop < MAX_DEFAULT_OBJECT; loop++ )
  {
    if ( DefaultObject[loop] <= 0
      || !( pObjIndex = get_obj_index( DefaultObject[loop] ) ) )
      break;

    if ( ( pObj = create_object( pObjIndex, -1 ) ) )
    {
      obj_to_char( pObj, ch );
      wear_obj( ch, pObj, FALSE, FALSE );
    }
  }

  save_char_obj( ch, SAVE_FILE   );
  save_char_obj( ch, BACKUP_FILE );

  sprintf( buf, "%s���ۤv����{�Q�������N�M�ҥH���s�}�l�L���t�~�@���T"
    , mob_name( NULL, ch ) );

  talk_channel_2( buf, CHANNEL_BULLETIN, "�p�D����" );

  send_to_char( "�A��ۤv����{���ƺ��N�M�ҥH���s�}�l�A���s�ͬ��T\n\r", ch );

  RETURN_NULL();
}

FUNCTION( do_suicide )
{
  DESCRIPTOR_DATA * d;
  CLUB_DATA       * pClub;
  CHAR_DATA       * pChar;
  OBJ_DATA        * pObj;
  char              arg1[MAX_INPUT_LENGTH];
  char              filename[MAX_INPUT_LENGTH];
  char              exec_cmd[MAX_STRING_LENGTH];
  char              talkbuf[MAX_STRING_LENGTH];
  char            * pArg;
  char              cEnd;
  int               level;

  PUSH_FUNCTION( "do_suicide" );

  if ( IS_NPC( ch ) || !ch->pcdata ) RETURN_NULL();

  pArg = arg1;
  while ( isSpace( ( int ) *argument ) ) argument++;

  cEnd = ' ';
  if ( *argument == '\'' || *argument == '"' ) cEnd = *argument++;

  while ( *argument )
  {
    if ( *argument == cEnd )
    {
      argument++;
      break;
    }
    *pArg++ = *argument++;
  }
  *pArg = '\x0';

  if ( !arg1[0] )
  {
    send_to_char( "�A�������㥴�X suicide <�K�X>�C\n\r", ch );
    RETURN_NULL();
  }

  if ( str_cmp( crypt( arg1, ch->pcdata->pwd ), ch->pcdata->pwd ) )
  {
    send_to_char( "�K�X���X��M�A�A�Ҽ{�M���a�C\n\r", ch );
    RETURN_NULL();
  }

  if ( IS_IMMORTAL( ch ) )
  {
    send_to_char( "�A���u�@�S�������M����۱��C\n\r", ch );
    RETURN_NULL();
  }

  if ( ( pClub = ch->club )
    && ( ( level = name_in_club( ch->name, pClub ) ) == CLUB_MASTER ) )
  {
    send_to_char( "�A�O�@�����D�M�۱����n�a�C\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->pcdata && ch->pcdata->mater && *ch->pcdata->mater )
  {
    send_to_char( "�u���ݯ}�Х@�F�ܡS�����B�a�T\n\r", ch );
    RETURN_NULL();
  }

  if ( is_pk( ch ) )
  {
    send_to_char( "�A�٦b�Z���j�|���M�N�R�@�I�M�������A���T\n\r", ch );
    RETURN_NULL();
  }

  /* �ץ��O�� */
  for ( pChar = char_list; pChar; pChar = pChar->next )
  {
    if ( verify_char( pChar )
      && IS_NPC( pChar )
      && IS_SET( pChar->act, ACT_ENROLL ) )
    {
      extract_enroll( pChar, ch->name );
    }
  }

  /* �P������ */
  for ( pObj = object_list; pObj; pObj = pObj->next )
  {
    if ( verify_obj( pObj )
      && pObj->item_type == ITEM_CORPSE_PC
      && pObj->corpse_owner == ch )
    {
      extract_obj( pObj );
    }
  }

  /* �󥿭^���] */
  check_hero( ch->name );

  /* ������ */
  if ( ch->club ) char_from_club( ch->name, ch->club, CLUB_ALL );

  if ( !IS_NPC( ch ) && ch->level >= level_limit )
  {
    sprintf( talkbuf, "\e[1;31m���䪺�������a���F�}�ӡM�b���Ū��ѪŹ��W�F�@"
      "�����ꪺ�i�K\n\r\e[0m\e[1;36m����������S���@���P���F�U�ӡM"
      "�s�Ѫų��U�_�B�ӡM���G�b�L�n�����_�ۡK\n\r\e[0m" );

    do_echo( NULL, talkbuf );

    sprintf( talkbuf, "���o�˪��å@�M�ڵL�ର�O�F�M�çO�F�M�U��T");
    talk_channel( ch, talkbuf , CHANNEL_SUICIDE , "���_�a���D" );

    sprintf( talkbuf, "%s�q�����J�F���v�M�A�H��A�]�ݤ���o�ӤH�F�T"
      , mob_name( NULL, ch ) );

    do_echo( NULL, talkbuf );
  }

  send_to_char(
    "\n\r�A���A��V���F��ӵ��ꪺ�ѪšC�o�O�å@������O�ۤv�O�S\n\r"
    "�w���a�M�çO�F�M�ڪ�����T\n\r\n\r ",ch );

  mudlog( LOG_SUICIDE, "%s �۵�.", mob_name( NULL, ch ) );
  delete_database( ch );

  str_cpy( filename, file_name( ch->name, SAVE_FILE ) );
  save_char_obj( ch , SAVE_FILE );
  d = ch->desc;
  extract_char( ch, TRUE );
  if ( d ) close_socket( d );

  sprintf( exec_cmd, "rm -rf %s/%s/%s"
    , player_dir, initial( ch->name ), normalize( ch->name ) );

  system( exec_cmd );
  RETURN_NULL();
}

FUNCTION( do_dormancy )
{
  char arg[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  int  day;

  PUSH_FUNCTION( "do_dormancy" );

  if ( IS_NPC( ch )
    || !ch->desc
    || !can_quit( ch, TRUE )
    || !ch->pcdata ) RETURN_NULL();

  one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    send_to_char( "�A����n�V�v�X�ѩO�S\n\r", ch );
    RETURN_NULL();
  }

  if ( !is_number( arg ) || ( day = atoi( arg ) ) <= 0 || day >= hold_day )
  {
    send_to_char( "�A�o�˥V�v�|���H���C�T\n\r", ch );
    RETURN_NULL();
  }

  if ( is_pk( ch ) )
  {
    send_to_char( "�A�٦b�Z���j�|���M�N�R�@�I�M�������A���T\n\r", ch );
    RETURN_NULL();
  }

  if ( IS_IMMORTAL( ch ) )
  {
    send_to_char( "�A�O�޲z�̡M�S���v�O�V�v��T\n\r", ch );
    RETURN_NULL();
  }

  ch->pcdata->dormancy = time( NULL ) + day * 60 * 60 * 24;
  act( "�A�N�b$i�ѫ���ӡM�A�C�C�a���ιL�h�F�T", ch, &day, NULL, TO_CHAR );

  sprintf( buf, "%s�i�J�F�V�v���A�M%s�ѫ�~�|���ӡM�A���F�M�U��T"
    , mob_name( NULL, ch ), arg );

  do_echo( ch, buf );
  do_quit( ch, "" );

  RETURN_NULL();
}

FUNCTION( do_quit )
{
  DESCRIPTOR_DATA * d;

  PUSH_FUNCTION( "do_quit" );

  if ( IS_NPC( ch ) || !can_quit( ch, TRUE ) ) RETURN_NULL();

  /* �ץ����M */
  if ( ch->mount    ) unmount_char( ch, ch->mount    );
  if ( ch->mount_by ) unmount_char( ch->mount_by, ch );

  send_to_char(
    "\n\r�^�Q�@�U�M���ꪺ�ڦ��G�ٯd�b�A���������C"
    "���o�ͪ��M�O�u���٬O�����O�S\n\r"
    "���M�u�൥�ݤU���A�򤤰ꤧ�ڧa�T"
    "�[�o�a�M�ڪ����ꤧ�ڡC\n\r\n\r", ch );

  act( "$n���}�C���F�C", ch, NULL, NULL, TO_ROOM );
  mudlog( LOG_INFO , "%s���}�C���F�C", ch->name );

  if ( IS_IMMORTAL( ch ) ) immortal_off_line( ch );

  save_char_obj( ch ,SAVE_FILE );
  d = ch->desc;
  extract_char( ch, TRUE );
  if ( d ) close_socket( d );

  RETURN_NULL();
}

FUNCTION( do_backup )
{
  char arg[MAX_INPUT_LENGTH];
  int  tick;

  PUSH_FUNCTION( "do_backup" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    save_char_obj( ch , BACKUP_FILE );
    send_to_char( "�ƥ���Ƨ����C\n\r", ch );

    if ( excess_filequota( ch ) )
      send_to_char( "\e[1;31m�A���ƥ��ɮ׶W�X�t�Ϊ��t�B�F�T\e[0m\n\r", ch );
  }

  else if ( !str_prefix( arg, "show" ) && ch->pcdata )
  {
    if ( ch->pcdata->autobackup <= 0 )
    {
      send_to_char( "�A�|���]�w�۰ʳƥ����ɶ��T\n\r", ch );
      RETURN_NULL();
    }

    act( "�A�]�w�C$i�Өt�ήɶ��N�۰ʳƥ��@���T"
      , ch, &ch->pcdata->autobackup, NULL, TO_CHAR );
  }

  else if ( !str_prefix( arg, "set" ) && ch->pcdata )
  {
    if ( argument[0] == '\x0' || !is_number( argument ) )
    {
      send_to_char( "�A�n�]�w�X�Өt�ήɶ��۰ʳƥ��@���O�S\n\r", ch );
      RETURN_NULL();
    }

    if ( ( tick = atoi( argument ) ) <= 0 )
    {
      send_to_char( "�A�����F�۰ʳƥ����\\��T\n\r", ch );

      ch->pcdata->autobackup      = -1;
      ch->pcdata->autobackup_tick = -1;

      RETURN_NULL();
    }

    if ( tick > 100 )
    {
      send_to_char( "�藍�_�M�A�]�w���ɶ����X�k�T\n\r", ch );
      RETURN_NULL();
    }

    ch->pcdata->autobackup      = tick;
    ch->pcdata->autobackup_tick = tick;

    act( "�A�]�w$i�Өt�ήɶ��۰ʳƥ��@���T", ch, &tick, NULL, TO_CHAR );
  }

  else
  {
    send_to_char( "�藍�_�M�A���y�k���~�M�Ьd�� backup ���ϥΤ�k�T\n\r", ch );
  }

  RETURN_NULL();
}

FUNCTION( do_save )
{
  char arg[MAX_INPUT_LENGTH];
  int  tick;

  PUSH_FUNCTION( "do_save" );

  if ( IS_NPC( ch ) || !ch->pcdata ) RETURN_NULL();

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    save_char_obj( ch , SAVE_FILE );

    act( "�x�s��Ƨ����M�ɮפj�p�� $3$t$0 �줸�աT"
      , ch, numberize( ch->pcdata->file_size, NULL ), NULL, TO_CHAR );

    if ( excess_filequota( ch ) )
      send_to_char( "\a\a\e[1;31m�A���ɮ׶W�X�t�Ϊ��t�B�F�T\e[0m\n\r", ch );
  }

  else if ( !str_prefix( arg, "show" ) && ch->pcdata )
  {
    if ( ch->pcdata->autosave <= 0 )
    {
      send_to_char( "�A�|���]�w�۰ʦs�ɪ��ɶ��T\n\r", ch );
      RETURN_NULL();
    }

    act( "�A�]�w�C$i�Өt�ήɶ��N�۰ʦs�ɤ@���T"
      , ch, &ch->pcdata->autosave, NULL, TO_CHAR );
  }

  else if ( !str_prefix( arg, "set" ) && ch->pcdata )
  {
    if ( argument[0] == '\x0' || !is_number( argument ) )
    {
      send_to_char( "�A�n�]�w�X�Өt�ήɶ��۰ʦs�ɤ@���O�S\n\r", ch );
      RETURN_NULL();
    }

    if ( ( tick = atoi( argument ) ) <= 0 )
    {
      ch->pcdata->autosave      = -1;
      ch->pcdata->autosave_tick = -1;

      send_to_char( "�A�����F�۰ʦs�ɪ��\\��T\n\r", ch );
      RETURN_NULL();
    }

    if ( tick > 100 )
    {
      send_to_char( "�藍�_�M�A�]�w���ɶ����X�k�T\n\r", ch );
      RETURN_NULL();
    }

    ch->pcdata->autosave      = tick;
    ch->pcdata->autosave_tick = tick;

    act( "�A�]�w$i�Өt�ήɶ��۰ʦs�ɤ@���T", ch, &tick, NULL, TO_CHAR );
  }

  else
  {
    send_to_char( "�藍�_�M�A���y�k���~�M�Ьd�� save ���ϥΤ�k�T\n\r", ch );
  }

  RETURN_NULL();
}

FUNCTION( do_follow )
{
  char        arg[MAX_INPUT_LENGTH];
  CHAR_DATA * victim;

  PUSH_FUNCTION( "do_follow" );

  one_argument( argument, arg );

  if ( ch->mercenary || ch->hirer )
  {
    send_to_char( "�A���ħL�M�ҥH������H�O�H�C\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->boss )
  {
    send_to_char( "�A�O�O�H�i���p���M������H�O�H��T\n\r", ch );
    RETURN_NULL();
  }

  if ( !IS_NPC( ch ) && is_pk( ch ) )
  {
    send_to_char( "�藍�_�M�A�M�ߧA���Z���j�|�a�T\n\r", ch );
    RETURN_NULL();
  }

  if ( !( victim = get_char_room( ch, arg ) ) )
  {
    act( "�A��ӧ�h�M�N�O�䤣�� $2$T$0�C", ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  if ( is_affected( ch, SLOT_CHARM_PERSON ) && ch->master )
  {
    act( "�A�w�g���H�F$N�T", ch, NULL, ch->master, TO_CHAR );
    RETURN_NULL();
  }

  if ( victim == ch )
  {
    if ( !ch->master )
    {
      send_to_char( "�A�w�g���A���H�O�H�F�C\n\r", ch );
      RETURN_NULL();
    }

    stop_follower( ch );
    RETURN_NULL();
  }

  if ( ( ch->level - victim->level < -( FollowLevel )
    || ch->level - victim->level > FollowLevel )
    && !IS_IMMORTAL(ch) )
  {
    send_to_char( "�A����[�J�o�Ӷ���C\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->master ) stop_follower( ch );
  add_follower( ch, victim );

  RETURN_NULL();
}

void add_follower( CHAR_DATA * ch, CHAR_DATA * master )
{
  PUSH_FUNCTION( "add_follower" );

  if ( !ch || !master )
  {
    mudlog( LOG_DEBUG, "add_follower: �ӷ����s�b." );
    RETURN_NULL();
  }

  if ( ch->master )
  {
    mudlog( LOG_DEBUG , "add_follower: �٦���L�D�H." );
    RETURN_NULL();
  }

  ch->master = master;
  ch->leader = NULL;

  if ( can_see( master, ch ) )
    act( "$n�{�b���H�A�F�C", ch, NULL, master, TO_VICT );

  act( "�A�}�l���H$N�C",  ch, NULL, master, TO_CHAR );

  RETURN_NULL();
}

void stop_follower( CHAR_DATA * ch )
{
  PUSH_FUNCTION( "stop_follower" );

  if ( !ch || !ch->master )
  {
    mudlog( LOG_DEBUG , "Stop_follower: �ӷ����s�b." );
    RETURN_NULL();
  }

  if ( is_affected( ch, SLOT_CHARM_PERSON ) )
    affect_release( ch, SLOT_CHARM_PERSON );

  if ( can_see( ch->master, ch ) )
    act( "$n������H�A�C", ch, NULL, ch->master, TO_VICT );

  act( "�A������H$N�C", ch, NULL, ch->master, TO_CHAR );

  ch->master = ch->leader = NULL;
  RETURN_NULL();
}

void die_follower( CHAR_DATA * ch )
{
  CHAR_DATA * fch;

  PUSH_FUNCTION( "die_follower" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "die_follower: �ӷ����s�b." );
    RETURN_NULL();
  }

  if ( ch->master ) stop_follower( ch );
  ch->leader = NULL;

  for ( fch = char_list; fch; fch = fch->next )
  {
    if ( !verify_char( fch ) ) continue;
    if ( fch->master == ch )   stop_follower( fch );
    if ( fch->leader == ch )   fch->leader = fch;
  }

  RETURN_NULL();
}

FUNCTION( do_order )
{
  char               arg[MAX_INPUT_LENGTH];
  CHAR_DATA        * victim;
  CMD_DATA         * pCommand;
  extern CMD_DATA  * LastCommand;
  extern CHAR_DATA * LastChar;
  extern char        LastArgument[];

  PUSH_FUNCTION( "do_order" );

  argument = one_argument( argument, arg );

  if ( !arg[0] || !argument[0] )
  {
    send_to_char( "�A�Q�R�O�֥h������ơS\n\r", ch );
    RETURN_NULL();
  }

  if ( is_affected( ch, SLOT_CHARM_PERSON ) )
  {
    send_to_char( "�A�w�g���h�ߴ��M�ٷQ�R�O�O�H�C\n\r", ch );
    RETURN_NULL();
  }

  if ( !( victim = get_char_room( ch, arg ) ) )
  {
    act( "$2$T$0 ���b�o�̡C", ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  if ( victim == ch )
  {
    send_to_char( "�A�O���O�ƤF�M���M�R�O�ۤv���Ʊ��T\n\r", ch );
    RETURN_NULL();
  }

  if ( ( !is_affected( victim, SLOT_CHARM_PERSON ) || victim->master != ch )
    && victim->boss != ch )
  {
    act( "�A�S���O$N���D�H�M�A�ۤv�h���a�T", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( !IS_NPC( victim ) )
  {
    act( "���M$N�w�g�Q�A����M���O�A����R�O$E�U���O�T", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    act( "�A�n�R�O$N������ƩO�S", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  for ( pCommand = cmd_list; pCommand; pCommand = pCommand->next )
  {
    if ( !pCommand->order )                                 continue;
    if ( get_trust( victim ) < pCommand->level )            continue;
    if ( pCommand->argument && !argument[0] )               continue;
    if ( victim->position < pCommand->position )            continue;
    if ( !pCommand->dead && is_dead( victim ) )             continue;
    if ( pCommand->lock )                                   continue;

    if ( ( !pCommand->exact && !str_prefix( arg, pCommand->name ) )
      || (  pCommand->exact && !str_cmp   ( arg, pCommand->name ) ) )
    {
      print_to_char( ch, "�A�R�O%s�����O%s�M�Ѽ�%s�C\n\r"
        , mob_name( ch, victim ), arg
        , argument[0] ? argument : "�q��" );

      /* �O���̪�ϥΪ��p */
      LastCommand = pCommand;
      LastChar    = ch;
      str_cpy( LastArgument , argument );

      /* ����R�O */
      ( *pCommand->function ) ( victim , argument );

      /* �����̪�ϥΪ��p */
      LastCommand     = NULL;
      LastChar        = NULL;
      LastArgument[0] = '\x0';

      RETURN_NULL();
    }
  }

  send_to_char( "�藍�_�M�A�R�O�����O�L�ġT\n\r", ch );
  RETURN_NULL();
}

FUNCTION( do_group )
{
  char        buf[MAX_STRING_LENGTH];
  char        arg[MAX_INPUT_LENGTH];
  CHAR_DATA * victim;
  CHAR_DATA * gch;
  CHAR_DATA * leader;

  PUSH_FUNCTION( "do_group" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  if ( is_pk( ch ) )
  {
    send_to_char( "�藍�_�M�A�M�ߧA���Z���j�|�a�T\n\r", ch );
    RETURN_NULL();
  }

  one_argument( argument, arg );

  if ( !arg[0] )
  {
    leader = ch->leader ? ch->leader : ch;
    sprintf( buf, "%s������R\n\r", mob_name( ch, leader ) );
    send_to_char( buf, ch );

    /* ���o���ĤH�� */
    for ( gch = char_list; gch; gch = gch->next )
    {
      if ( !verify_char( gch ) ) continue;

      if ( is_same_group( gch, ch ) )
      {
        sprintf( buf,
        "[%3d %6s] %-6s  �ͩR�O%5d/%5d �k�O%5d/%5d ��O %5d/%5d\n\r"
            , gch->level
            , class_name( gch->class, FALSE )
            , mob_name( ch, gch )
            , gch->hit,   get_curr_hit( gch )
            , gch->mana,  get_curr_mana( gch )
            , gch->move,  get_curr_move( gch ) );
        send_to_char( buf, ch );
      }
    }
    RETURN_NULL();
  }

  if ( !( victim = get_char_room( ch, arg ) ) )
  {
    act( "$2$T$0 ���b�o�̡C", ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  if ( ch->hirer || ch->mercenary )
  {
    send_to_char( "�A���Ǥw�g���ħL�M�����O�H�ն��C\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->master || ( ch->leader && ch->leader != ch ) )
  {
    send_to_char( "�A�w�g���H�O�H�F�C\n\r", ch );
    RETURN_NULL();
  }

  if ( ch == victim )
  {
    send_to_char( "�A�����N�O�A�ۤv�������F�C\n\r", ch );
    RETURN_NULL();
  }

  if ( victim->master != ch )
  {
    act( "$N�èS�����H�A�C", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( is_same_group( victim, ch ) && ch != victim )
  {
    victim->leader = NULL;
    act( "$n��$N�q��������簣�C", ch, NULL, victim, TO_NOTVICT );
    act( "$n��A�q$n������簣�C", ch, NULL, victim, TO_VICT    );
    act( "�A��$N�q�A������簣�C", ch, NULL, victim, TO_CHAR    );
    RETURN_NULL();
  }

  if ( ch->level - victim->level < -( FollowLevel )
    || ch->level - victim->level >  ( FollowLevel ) )
  {
    act( "$N����[�J$n������C", ch, NULL, victim, TO_NOTVICT );
    act( "�A����[�J$n������C", ch, NULL, victim, TO_VICT    );
    act( "$N����[�J�A������C", ch, NULL, victim, TO_CHAR    );
    RETURN_NULL();
  }

  victim->leader = ch;
  act( "$N�[�J$n������C", ch, NULL, victim, TO_NOTVICT );
  act( "�A�[�J$n������C", ch, NULL, victim, TO_VICT    );
  act( "$N�[�J�A������C", ch, NULL, victim, TO_CHAR    );
  RETURN_NULL();
}

FUNCTION( do_split )
{
  char        buf[MAX_STRING_LENGTH];
  char        arg[MAX_INPUT_LENGTH];
  CHAR_DATA * gch;
  int         members = 0;
  int         amount;
  int         share;
  int         extra;

  PUSH_FUNCTION( "do_split" );

  if ( ch->trade == FALSE )
  {
    send_to_char( "�A���`�겣�ӥi�ȤF�M�ҥH�L�k����o�ӫ��O�T\n\r", ch );
    RETURN_NULL();
  }

  if ( !IS_NPC( ch ) && ch->level < level_limit )
  {
    act( "�b�A�ɯŨ�$i�ūe�M�Ф��n�������O�H�C"
      , ch, &level_limit, NULL, TO_CHAR );

    RETURN_NULL();
  }

  if ( ch->donate > 0 )
  {
    send_to_char( "�b�A����٪����o�q�ɶ��M�Ф��n�˴I���T\n\r", ch );
    RETURN_NULL();
  }

  if ( !ch->in_room )
  {
    send_to_char( "�A�{�b�������D�b���̭C�T\n\r", ch );
    RETURN_NULL();
  }

  one_argument( argument, arg );

  if ( ( amount = atoi( arg ) ) < 0 )
  {
    send_to_char( "���I�۷N�a�T\n\r", ch );
    RETURN_NULL();
  }

  if ( amount == 0 )
  {
    send_to_char( "�A�}�X�@�i���Y�䲼�M���S�H�`�N��A�C\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->gold < amount )
  {
    send_to_char( "�A���W�S������h�����C\n\r", ch );
    RETURN_NULL();
  }

  for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    if ( is_same_group( gch, ch ) ) members++;

  if ( members < 2 )
  {
    send_to_char( "�A�ۤv�d�۴N�n�F�C\n\r", ch );
    RETURN_NULL();
  }

  share = amount / members;
  extra = amount % members;

  if ( share == 0 )
  {
    send_to_char( "�A���o�ǿ��������ӥ����C\n\r", ch );
    RETURN_NULL();
  }

  gold_from_char( ch, amount );
  gold_to_char( ch, share + extra );

  sprintf( buf, "�A���X %d ����l�����M�A�ۤv���� %d ����l�C\n\r",
      amount, share + extra );
  send_to_char( buf, ch );

  sprintf( buf, "$n���X %d ����l�����M�A����F %d ����l�C", amount, share );

  /* ���o�ж����ĤH�� */
  for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
  {
    if ( gch != ch && is_same_group( gch, ch ) )
    {
      act( smash_act_keyword( buf, "nesNES" ), ch, NULL, gch, TO_VICT );
      gold_to_char( gch, share );
    }
  }

  RETURN_NULL();
}

FUNCTION( do_gtell )
{
  char        buf[MAX_STRING_LENGTH];
  char        buf1[MAX_STRING_LENGTH];
  CHAR_DATA * gch;

  PUSH_FUNCTION( "do_gtell" );

  if ( IS_SET( ch->act, PLR_NO_TELL ) )
  {
    send_to_char( "�L�{�bť����A���ܡC\n\r", ch );
    RETURN_NULL();
  }

  /* �ˬd����O�d�r */
  if ( check_chat_xname( ch, argument ) == TRUE ) RETURN_NULL();

  if ( check_repeat( ch , argument ) == TRUE )
  {
    send_to_char( "�A����i�D�O�H�ۦP���ܡC\n\r" , ch );
    RETURN_NULL();
  }

  ansi_transcribe( argument, buf1 );
  sprintf( buf, "\e[1;33m%s\e[1;33m�i�D���W���H�R�u%s�v�C\e[0m\n\r"
    , mob_name( NULL, ch ), buf1 );

  for ( gch = char_list; gch; gch = gch->next )
  {
    if ( !verify_char( gch ) ) continue;
    if ( is_same_group( gch, ch ) ) send_to_char( buf, gch );
  }

  RETURN_NULL();
}

/* �B�e���~����L���a�άO�Ǫ� */
FUNCTION( do_send )
{
  char        arg[MAX_INPUT_LENGTH];
  OBJ_DATA  * pObj;
  CHAR_DATA * pVictim;
  int         money;

  PUSH_FUNCTION( "do_send" );

  /* �u�����a�i�H���o�˰ʧ@ */
  if ( IS_NPC( ch ) ) RETURN_NULL();

  if ( ch->position == POS_FIGHTING )
  {
    send_to_char( "���ѨM�A���p�H����a�T\n\r", ch );
    RETURN_NULL();
  }

  if ( !IS_NPC( ch ) && ch->level < level_limit )
  {
    act( "�b�A�ɯŨ�$i�ūe�M�Ф��n�ðe�F�赹�O�H�C"
      , ch, &level_limit, NULL, TO_CHAR );

    RETURN_NULL();
  }

  if ( ch->donate > 0 )
  {
    send_to_char( "�b�A����٪����o�q�ɶ��M�Ф��n�˴I���T\n\r", ch );
    RETURN_NULL();
  }

  /* �ˬd�O�_���o�˪��~ */
  argument = one_argument( argument, arg );

  if ( !( pObj = get_obj_carry( ch, arg ) ) )
  {
    send_to_char( "�藍�_�M�A���W�S���o�˪��~�C\n\r", ch );
    RETURN_NULL();
  }

  /* ���~�O�_�����r */
  if ( poison_char( ch, pObj ) ) RETURN_NULL();

  if ( pObj->address )
  {
    act( "$p�O�n�浹�O�H���H�M���i�H�e���O�H�C", ch, pObj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  /* �ݬݬO���O�i�H��� */
  if ( !can_drop_obj( ch, pObj ) )
  {
    act( "$p�O�A���Ǯa���_�M���i�H�e���O�H�C", ch, pObj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( pObj->item_type == ITEM_TRASH )
  {
    send_to_char( "�Ф��n�å�U���n�ܡS\n\r", ch );
    RETURN_NULL();
  }

  /* ��M�H�e����H */
  argument = one_argument( argument, arg );

  if ( !arg[0]
    || !( pVictim = get_char_world( ch, arg ) )
    || !pVictim->in_room
    || !can_see( ch, pVictim )
    || !can_see_mask( ch, pVictim ) )
  {
    send_to_char( "�藍�_�M�䤣��A�n�H�e����H�C\n\r" , ch );
    RETURN_NULL();
  }

  /* �ݬݬO�_�P�@�ӤH�άO�P�@���ж� */
  if ( pVictim->in_room == ch->in_room )
  {
    act( "$N�Ӫ�F�M�ۤv���h�N�n�F�C", ch, NULL, pVictim, TO_CHAR );
    RETURN_NULL();
  }

  if ( is_pk( pVictim ) )
  {
    act( "$N���b�Z���j�|���M�ФŤz�Z�T", ch, NULL, pVictim, TO_CHAR );
    RETURN_NULL();
  }

  if ( is_dead( pVictim ) )
  {
    act( "$N�w�g�r�b���k�o�M�i��e����T", ch, NULL, pVictim, TO_CHAR );
    RETURN_NULL();
  }

  /* �ˬd�O�_���B�O�o�� */
  one_argument( argument, arg );

  if ( !arg[0] || ( money = atoi( arg ) ) <= 0 )
  {
    send_to_char( "�藍�_�M�A���������B�O�C\n\r" , ch );
    RETURN_NULL();
  }

  /* �ˬd�O�_�B�O�O�_���� */
  if ( money > ch->gold )
  {
    send_to_char( "�A���W�S�����������ӥI�A���B�O�C\n\r" , ch );
    RETURN_NULL();
  }

  /* �����B�O */
  gold_from_char( ch, money );
  obj_from_char( pObj );

  /* �B�e���� */
  if ( number_range( 0, UMIN( 1000000, pObj->cost ) ) > money )
  {
    act( "�藍�_�M�]���̪�g��s�g�M�A��$p�Q���T�F�C", ch, pObj, NULL, TO_CHAR );
    extract_obj( pObj );
  }

  /* �B�e���\ */
  else
  {
    ROOM_INDEX_DATA * pRoom;

    act( "�A��$p�w�g�B�e�ܥت��a�F�C", ch, pObj, NULL, TO_CHAR );
    obj_to_room( pObj, pVictim->in_room );

    pRoom = ch->in_room;
    char_from_room( ch );
    char_to_room( ch, pVictim->in_room );

    act( "$7�m�N�K�K$A$3$n�e��$N���F��w�g��F�C$A�ѤW�@���j�P"
         "�����L�N��U�һε۪�$p�C$0", ch, pObj, pVictim, TO_ROOM );

    message_driver( ch, pObj, ACT_WHEN_SEND );

    char_from_room( ch );
    char_to_room( ch, pRoom );
  }

  RETURN_NULL();
}

bool is_same_group( CHAR_DATA * ach, CHAR_DATA * bch )
{
  PUSH_FUNCTION( "is_same_group" );
  if ( ach->leader ) ach = ach->leader;
  if ( bch->leader ) bch = bch->leader;
  RETURN( ach == bch );
}

/* �O�_������} */
bool can_quit( CHAR_DATA * ch, bool fPrint )
{
  PUSH_FUNCTION( "can_quit" );

  if ( IS_NPC( ch ) ) RETURN( FALSE );

  if ( !ch || !verify_char( ch ) )
  {
    mudlog( LOG_DEBUG, "can_quit: �ӷ������T." );
    RETURN( TRUE );
  }

  if ( ch->position == POS_FIGHTING )
  {
    if ( fPrint ) send_to_char( "�Q���S�����S���M�A�٦b�԰��O�C\n\r", ch );
    RETURN( FALSE );
  }

  if ( ch->in_room && ch->in_room->NoQuit && ch->timer < MaxPlayingIdle )
  {
    if ( fPrint ) send_to_char( "�����}�o�Ӱ��a��A���a�T\n\r", ch );
    RETURN( FALSE );
  }

  if ( is_pk( ch ) )
  {
    if ( fPrint ) send_to_char( "�A���b�Z���j�|�̡M�L�k�����C���T\n\r", ch );
    RETURN( FALSE );
  }

  if ( is_dead( ch ) )
  {
    if ( fPrint ) send_to_char( "�A�ثe�L�k���}�o�ӮɪšC\n\r", ch );
    RETURN( FALSE );
  }

  if ( auction_info->seller == ch )
  {
    if ( fPrint ) send_to_char( "�A�٦b��F��C�M���槹�o�ӦA���C\n\r" , ch );
    RETURN( FALSE );
  }

  if ( auction_info->buyer == ch )
  {
    if ( fPrint ) send_to_char( "�A�٦b�R�F��C�M���R���o�ӦA���C\n\r" , ch );
    RETURN( FALSE );
  }

  if ( is_station( ch->in_room ) )
  {
    if ( fPrint ) send_to_char( "�A�٦b�毸�̡M�Х��U���a�C\n\r", ch );
    RETURN( FALSE );
  }

  if ( ch->in_room->Sail )
  {
    if ( fPrint ) send_to_char( "�A�٦b��W���M�Х��U��a�T\n\r", ch );
    RETURN( FALSE );
  }

  RETURN( TRUE );
}

void extract_failcode( CHAR_DATA * ch )
{
  FAILCODE_DATA * zFailCode;
  FAILCODE_DATA * pFailCode;

  PUSH_FUNCTION( "extract_failcode" );

  if ( !ch || IS_NPC( ch ) )
  {
    mudlog( LOG_DEBUG, "extract_failcode: �ӷ������T." );
    RETURN_NULL();
  }

  for ( pFailCode = ch->failcode; pFailCode; pFailCode = zFailCode )
  {
    zFailCode = pFailCode->next;

    if ( pFailCode->address ) free_string( pFailCode->address );
    free_struct( pFailCode, STRUCT_FAILCODE_DATA );
  }

  ch->failcode = NULL;
  RETURN_NULL();
}

void show_failcode( CHAR_DATA * ch )
{
  FAILCODE_DATA * pFail;
  struct tm     * sTime;
  time_t          timer;
  char            timebuf[MAX_STRING_LENGTH];
  char            buf1[MAX_INPUT_LENGTH];
  char            buf2[MAX_INPUT_LENGTH];
  char            buf3[MAX_INPUT_LENGTH];
  char            buf4[MAX_INPUT_LENGTH];
  char            buf5[MAX_INPUT_LENGTH];

  PUSH_FUNCTION( "show_failcode" );

  if ( !ch || IS_NPC( ch ) )
  {
    mudlog( LOG_DEBUG, "show_failcode: �ӷ������T." );
    RETURN_NULL();
  }

  for ( clear_buffer(), pFail = ch->failcode; pFail; pFail = pFail->next )
  {
    timer = ( time_t ) pFail->timer;
    if ( !( sTime = localtime( &timer ) )
      || !pFail->address
      || !*pFail->address ) continue;

    chinese_number( sTime->tm_year - 11 , buf1 );
    chinese_number( sTime->tm_mon  +  1 , buf2 );
    chinese_number( sTime->tm_mday      , buf3 );
    chinese_number( sTime->tm_min       , buf5 );
    chinese_number( sTime->tm_hour > 12 ? sTime->tm_hour - 12 : sTime->tm_hour, buf4 );

    sprintf( timebuf, "����%s�~%s��%s��%s%s�I%s��"
      , buf1, buf2, buf3, sTime->tm_hour < 12 ? "���W" : "�U��", buf4, buf5 );

    send_to_buffer( "�A�W���b\e[1;32m%s\e[0m��\e[1;33m%s\e[0m�s�u�ɱK�X���~�T\n\r"
      , timebuf, pFail->address );
  }

  print_buffer( ch );
  RETURN_NULL();
}

FUNCTION( do_ask )
{
  int              count;
  int              cost;
  char             arg[MAX_INPUT_LENGTH];
  char             buf[MAX_STRING_LENGTH];
  CHAR_DATA      * victim;
  CHAR_DATA      * target;
  CHAR_DATA      * man;
  OBJ_DATA       * obj;
  AREA_DATA      * pArea;
  OBJ_INDEX_DATA * pIndex;

  PUSH_FUNCTION( "do_ask" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  if ( excess_filequota( ch ) )
  {
    send_to_char( "�藍�_�M�A���ɮפӤj�M�L�k����o�өR�O�T\n\r", ch );
    RETURN_NULL();
  }

  if ( !( pIndex = ObjLetter ) )
  {
    act( "�藍�_�M$t�S�����u���\\��T", ch, mud_name, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( ch->jail > 0 || ch->failed > 0 )
  {
    send_to_char( "�藍�_�M�ثe�A�L�k�e�H�T\n\r", ch );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    for ( obj = ch->carrying; obj; obj = obj->next_content )
    {
      if ( obj->item_type == ITEM_LETTER
        && ( target = obj->address )
        && target->in_room
        && target->in_room->area )
      {
        act( "�A���W���@�ʫH�M�o�ʫH�n�浹$N�T", ch, NULL, target, TO_CHAR );
        RETURN_NULL();
      }
    }

    send_to_char( "�A���W�S���n��檺�H��C\n\r", ch );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "!cancel" ) )
  {
    for ( obj = ch->carrying; obj; obj = obj->next_content )
      if ( obj->item_type == ITEM_LETTER
        && ( target = obj->address )
        && target->in_room
        && target->in_room->area ) break;

    if ( !obj )
    {
      send_to_char( "�A�èS���b���u��T\n\r", ch );
      RETURN_NULL();
    }

    else
    {
      act( "�A��n��$N���H�N�a�N���M�߱����n�M���e�F�T"
        , ch, NULL, obj->address, TO_CHAR );

      act( "$n�⨭�W��$p�Υ������N���M�n�����@�N�h�e�H���T"
        , ch, obj, NULL, TO_ROOM );

      extract_obj( obj );
      RETURN_NULL();
    }
  }

  else
  {
    if ( !ch->in_room || !( pArea = ch->in_room->area ) )
    {
      send_to_char( "�A�{�b�b���Ӱ��a��O�S\n\r", ch );
      RETURN_NULL();
    }

    if ( !IS_NPC( ch ) && ch->level < level_limit )
    {
      act( "�b�A�ɯŨ�$t�ūe�M�O�������H�e�H�C"
        , ch, &level_limit, NULL, TO_CHAR );

      RETURN_NULL();
    }

    if ( ch->donate > 0 )
    {
      send_to_char( "�b�A����٪����o�q�ɶ��M�Ф��n���H�e�H�F�T\n\r", ch );
      RETURN_NULL();
    }

    for ( obj = ch->carrying; obj; obj = obj->next_content )
    {
      if ( obj->item_type == ITEM_LETTER
        && ( target = obj->address )
        && target->in_room
        && target->in_room->area )
      {
        act( "�A���W�w�g���@�ʫH�M�o�ʫH�·���浹$N�T"
          , ch, NULL, target, TO_CHAR );

        RETURN_NULL();
      }
    }

    if ( ch->gold < ( cost = ch->level * AskCost ) )
    {
      act( "�A�������ꨬ$i��O�Ҫ��~��T", ch, &cost, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( !( victim = get_char_room( ch, arg ) ) )
    {
      act( "�䤣��A�n�ШD���Ȫ��H $2$T$0�T", ch, NULL, arg, TO_CHAR );
      RETURN_NULL();
    }

    if ( !can_see( victim, ch ) )
    {
      act( "$N�ݤ���A�C�C", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( !IS_NPC( victim ) )
    {
      act( "$N���O�D���a�M����ШD���ȡT\n\r", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( !IS_SET( victim->act, ACT_ASK ) )
    {
      act( "�A�n��$N�ШD���ȡM�A�O���O�ݿ��H�F�S", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( victim->position != POS_STANDING )
    {
      act( "$N���b���O�M���@�U�n�F�T", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( is_dead( ch ) || is_dead( victim ) )
    {
      send_to_char( "�{�b�i�ण�O�ШD���Ȫ��n�ɾ���T\n\r", ch );
      RETURN_NULL();
    }

    target = NULL;
    count  = 0;
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man )
        || !IS_NPC( man )
        || man->level < 50
        || man->fighting
        || man->mount_by
        || man->boss
        || !man->pIndexData
        || man->pIndexData->count > 1
        || !man->in_room
        || man->in_room->area == pArea ) continue;

      if ( number_range( 0, count++ ) == 0 ) target = man;
    }

    if ( !target )
    {
      send_to_char( "���M�ثe�ڨS���H�n�A���ڰe�C�T\n\r", ch );
      RETURN_NULL();
    }

    if ( !( obj = create_object( ObjLetter, 1 ) ) )
    {
      send_to_char( "�D�F�M�ڪ����ʫH�O�S�]�쨺�̥h�F�O�S\n\r", ch );
      RETURN_NULL();
    }

    obj->wear_loc = WEAR_NONE;
    obj->address  = target;
    obj->value[0] = 24 * 7;
    obj->cost     = cost;
    obj->NoSave   = TRUE;

    free_string( obj->description );
    sprintf( buf, "�o�O�@%s%s�M�ЧA��浹%s�H��M�л��ְe�F�a�T"
      , obj->unit
      , obj_name( ch, obj )
      , mob_name( ch, target ) );

    obj->description = str_dup( buf );

    sprintf( buf, "%s���D�R�u�o�̦��@%s%s�M�·ЧA��浹%s�M�������j�M"
      "�Ʀ�����M�L�|���A�@���S�Ҫ��T�v\n\r"
      , obj->unit
      , obj_name( ch, obj )
      , victim->byname
      , mob_name( ch, target ) );

    send_to_char( buf, ch );

    act( "$N��$n���Ѩp�y�M����$N�N�浹$n�@��$p�T", ch, obj, victim, TO_ALL );

    obj_to_char( obj, ch );
    gold_from_char( ch, cost );
  }

  RETURN_NULL();
}

FUNCTION( do_joke )
{
  JOKE_DATA  * pJoke;
  JOKE_DATA  * zJoke;
  char         arg[MAX_INPUT_LENGTH];
  char         buf[MAX_STRING_LENGTH];
  int          count;
  int          loop;
  unsigned int max_number;

  PUSH_FUNCTION( "do_joke" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  if ( !joke_list )
  {
    act( "�藍�_�M$t�����ѯ��ܡT", ch, mud_name, NULL, TO_CHAR );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' || !str_prefix( arg, "list" ) )
  {
    clear_buffer();
    send_to_buffer( "�Ҧ������ܦC��p�U�R" );

    for ( count = 0, pJoke = joke_list; pJoke; pJoke = pJoke->next )
    {
      if ( !pJoke->title ) continue;
      if ( ( count % 3 ) == 0 ) send_to_buffer( "\n\r" );

      str_cpy( buf, pJoke->title );
      buf[20] = '\x0';

      send_to_buffer( "\e[1;%dm%3d\e[0m. %-20s "
        , pJoke->stamp > ch->last_joke ? 36 : 32
        , ++count, pJoke->title );

      if ( buffer_full() ) break;
    }

    send_to_buffer( "\n\r" );
    print_buffer( ch );
  }

  else if ( !str_prefix( arg, "read" ) )
  {
    if ( !argument || !*argument || !is_number( argument ) )
    {
      send_to_char( "�A�n�\\Ū���@�g���ܩO�S\n\r", ch );
      RETURN_NULL();
    }

    count = atoi( argument );
    for ( loop = 0, pJoke = joke_list; pJoke; pJoke = pJoke->next )
    {
      if ( ++loop == count )
      {
        show_joke( ch, pJoke );
        ch->last_joke = UMAX( ch->last_joke, pJoke->stamp );
        RETURN_NULL();
      }
    }

    send_to_char( "�藍�_�M�䤣�쨺�g���ܭC�T\n\r", ch );
  }

  else if ( !str_prefix( arg, "next" ) )
  {
    zJoke      = NULL;
    max_number = ( unsigned int ) -1;

    for ( pJoke = joke_list; pJoke; pJoke = pJoke->next )
    {
      if ( pJoke->stamp > ch->last_joke && max_number > pJoke->stamp )
      {
        zJoke      = pJoke;
        max_number = pJoke->stamp;
      }
    }

    if ( !zJoke )
    {
      send_to_char( "�藍�_�M�A�w�g�\\Ū���o�̩Ҧ������ܤF�T\n\r", ch );
      RETURN_NULL();
    }

    show_joke( ch, zJoke );
    ch->last_joke = UMAX( ch->last_joke, zJoke->stamp );
  }

  else if ( !str_prefix( arg, "random" ) )
  {
    zJoke = NULL;
    count = 0;
    for ( pJoke = joke_list; pJoke; pJoke = pJoke->next )
      if ( number_range( 0, count++ ) == 0 ) zJoke = pJoke;

    if ( !zJoke )
    {
      send_to_char( "�藍�_�M�䤣��X�A�����ܵ��A�}�ߤ@�U�T\n\r", ch );
      RETURN_NULL();
    }

    show_joke( ch, zJoke );
  }

  else
  {
    send_to_char( "�藍�_�M�A���Ѽƿ��~�M�Ьd�� joke ���ϥΤ�k�T\n\r", ch );
  }

  RETURN_NULL();
}

void show_joke( CHAR_DATA * ch, JOKE_DATA * pJoke )
{
  PUSH_FUNCTION( "show_joke" );

  if ( !ch || !pJoke )
  {
    mudlog( LOG_DEBUG, "show_joke: �ʥF�ӷ�." );
    RETURN_NULL();
  }

  print_to_char( ch,
    "���D�R%s\n\r�X�B�R%s\n\r%s%s%s"
    , pJoke->title, pJoke->org, VERTICAL_LINE
    , pJoke->text, VERTICAL_LINE );

  RETURN_NULL();
}

FUNCTION( do_notepad )
{
  char arg[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  int  loop;

  PUSH_FUNCTION( "do_notepad" );

  if ( IS_NPC( ch ) || !ch->desc || !ch->pcdata ) RETURN_NULL();

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    for ( clear_buffer() , loop = 0; loop < MAX_NOTEPAD; loop++ )
    {
      chinese_number( loop + 1 , buf );
      send_to_buffer( "�A����%s�i�O�ƥ������A�u%s�v\n\r"
        , buf
        , ( !ch->pcdata->notepad[loop] || !*( ch->pcdata->notepad[loop] ) )
           ? "\e[1;34m�ťդ�\e[0m" : "\e[1;32m�O����\e[0m" );

       if ( buffer_full() ) break;
    }

    print_buffer( ch );
  }

  else if ( !str_prefix( arg, "show" ) )
  {
    if ( argument[0] == '\x0' || !is_number( argument ) )
    {
      send_to_char( "�A�n��ݨ��ӰO�ƥ��O�S\n\r", ch );
      RETURN_NULL();
    }

    loop = atoi( argument ) - 1;
    if ( loop < 0 || loop >= MAX_NOTEPAD )
    {
      send_to_char( "�A�S�����ӰO�ƥ��T\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->pcdata->notepad[loop] == NULL
      || !*( ch->pcdata->notepad[loop] ) )
    {
      send_to_char( "���i�O�ƥ��̬O�Ū��M����]�S���T\n\r", ch );
      RETURN_NULL();
    }

    chinese_number( loop + 1, buf );
    print_to_char( ch, "�A����%s�i�O�ƥ������e���R\n\r%s%s\e[0m\n\r%s"
      , buf, VERTICAL_LINE, ch->pcdata->notepad[loop], VERTICAL_LINE );
  }

  else if ( !str_prefix( arg, "edit" ) )
  {
    for ( loop = 0; loop < MAX_NOTEPAD; loop++ )
    {
      if ( ch->pcdata->notepad[loop] && !*( ch->pcdata->notepad[loop] ) )
      {
        ch->desc->edit_mode = EDIT_NOTEPAD;
        show_editor( ch );

        free_string( ch->stack );
        ch->stack = str_dup( "" );
        RETURN_NULL();
      }
    }

    send_to_char( "�藍�_�M�A���O�ƥ����O���F�F��T\n\r", ch );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "delete" ) )
  {
    if ( argument[0] == '\x0' || !is_number( argument ) )
    {
      send_to_char( "�A�n�R�����ӰO�ƥ��O�S\n\r", ch );
      RETURN_NULL();
    }

    loop = atoi( argument ) - 1;
    if ( loop < 0 || loop >= MAX_NOTEPAD )
    {
      send_to_char( "�A�S�����ӰO�ƥ��T\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->pcdata->notepad[loop] == NULL
      || !*( ch->pcdata->notepad[loop] ) )
    {
      send_to_char( "�A���i�O�ƥ����ӴN�O�Ū��T\n\r", ch );
      RETURN_NULL();
    }

    free_string( ch->pcdata->notepad[loop] );
    ch->pcdata->notepad[loop] = str_dup( "" );
    send_to_char( "�A�w�g�R�����i�O�ƥ��F�T\n\r", ch );
  }

  else
  {
    send_to_char( "�A���Ѽƿ��~�M�Ьd�� notepad ���Ϊk�T\n\r", ch );
  }

  RETURN_NULL();

}

FUNCTION( do_endow )
{
  char arg[MAX_INPUT_LENGTH];
  bool bEvil;
  int  gold;
  int  old;
  int  value;

  PUSH_FUNCTION( "endow" );

  if ( IS_NPC( ch ) || !ch->in_room ) RETURN_NULL();

  if ( ch->position != POS_STANDING )
  {
    send_to_char( "�A�������A���䪺�Ʊ��a�T\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->in_room->Fane )
  {
    bEvil = FALSE;
  }

  else if ( ch->in_room->Killer )
  {
    bEvil = TRUE;
  }

  else
  {
    send_to_char( "�藍�_�M�o�̤��O�x�q�άO�K�P�����T\n\r", ch );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' || !is_number( arg ) )
  {
    send_to_char( "�A�Q�n����h�ֿ��O�S\n\r", ch );
    RETURN_NULL();
  }

  if ( ( gold = atoi( arg ) ) <= 0 )
  {
    send_to_char( "�A�����O�ȿ��ܡS\n\r", ch );
    RETURN_NULL();
  }

  if ( gold > ch->gold )
  {
    send_to_char( "�藍�_�M�o�̤��������Y�䲼�T\n\r", ch );
    RETURN_NULL();
  }

  if ( EndowRate <= 0 || gold < EndowRate )
  {
    send_to_char( "���I�۷N�@�I�ܡM�o��@�I�����F����S\n\r", ch );
    RETURN_NULL();
  }

  old   = ch->alignment;
  value = number_range( 1, UMAX( 1, UMIN( 1000000, gold / EndowRate ) ) );
  gold_from_char( ch, gold );

  if ( bEvil )
  {
    ch->alignment = UMAX( -AlignRange, ch->alignment - value );

    act( "$n���F$i��Ȥl�����a���K�P�����M�L�̥�$n�������X��h���H�פe�N�]�T"
      , ch, &gold, NULL, TO_ALL );

    if ( old == ch->alignment ) send_to_char( "�A���}��S�����ܡT\n\r", ch );
    else send_to_char( "�A���}���ܪ�����F�T\n\r", ch );
  }

  else
  {
    ch->alignment = UMIN( AlignRange, ch->alignment + value );

    act( "$n���F$i��Ȥl�����a���x�q�M�L����$n���F�@���k�ơT"
      , ch, &gold, NULL, TO_ALL );

    if ( old == ch->alignment ) send_to_char( "�A���}��S�����ܡT\n\r", ch );
    else send_to_char( "�A���}���ܪ���n�F�T\n\r", ch );
  }

  RETURN_NULL();
}

FUNCTION( do_donate )
{
  char arg[MAX_INPUT_LENGTH];
  char arg1[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  int  gold;
  int  number;

  PUSH_FUNCTION( "do_donate" );

  if ( IS_NPC( ch ) || !ch->in_room || is_affected( ch, SLOT_CHARM_PERSON ) )
    RETURN_NULL();

  if ( DonateLock && !IS_IMMORTAL( ch ) )
  {
    send_to_char( "�藍�_�M�ثe�o�ӥ\\��Ȯɤ��}�ҡT\n\r", ch );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    if ( DonateMoney > 0 )
    {
      act( "�ثe$2$t$0����|�`���B��$3$I$0��Ȥl�T"
        , ch, mud_name, &DonateMoney, TO_CHAR );

      if ( !can_donate( ch, FALSE ) )
      {
        send_to_char( "�A�ثe���������٪��T\n\r", ch );
      }
      else
      {
        act( "�A�ثe�i�H��U�a������������٪�$i��Ȥl�T"
          , ch, &DonateBenifit, NULL, TO_CHAR );
      }
    }

    else
    {
      act( "�ثe$2$t$0����|�̨S�����󮽴ڡM�еo���A�����ߡM�Ю��I���a�T"
        , ch, mud_name, NULL, TO_CHAR );
    }

    if ( ch->donate > 0 )
      send_to_char( "�A�ثe�٦b���ٮɶ����M�е��ΧA�������T\n\r", ch );
  }

  else if ( !str_prefix( arg, "gold" ) )
  {
    if ( DonateLock )
    {
      send_to_char( "�o�ӥ\\��b��w�ɤ������T\n\r", ch );
      RETURN_NULL();
    }

    if ( IS_IMMORTAL( ch ) )
    {
      send_to_char( "�A�u��ʷ�����|�M�����஽�ڡT\n\r", ch );
      RETURN_NULL();
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\x0' || !is_number( arg ) )
    {
      act( "�A�n���h�ֿ���$2$t$0����|�O�S", ch, mud_name, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( !IS_NPC( ch ) && ch->level < level_limit )
    {
      act( "�b�A�ɯŨ�$i�ūe�M�Ф��n�����C", ch, &level_limit, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch->donate > 0 )
    {
      send_to_char( "���~��F���٪��M���M�{�b�N�^�X�F�ڡS\n\r", ch );
      RETURN_NULL();
    }

    if ( ( gold = atoi( arg ) ) <= 0 )
    {
      send_to_char( "�A�n�����O�ȿ��ܡS\n\r", ch );
      RETURN_NULL();
    }

    if ( gold > ch->gold )
    {
      send_to_char( "�����O�ܦn�M���]�n�ݬݦۤv�����]�T\n\r", ch );
      RETURN_NULL();
    }

    if ( DonateMoney + gold <= 0 )
    {
      send_to_char( "�ѩ�A�����ءM����|�i��|�˳��T\n\r", ch );
      RETURN_NULL();
    }

    DonateMoney += gold;
    gold_from_char( ch, gold );

    act( "���§A�M�A���F$2$i$0��Ȥl�M�ثe����|�̦�$2$I$0��Ȥl�T"
      , ch, &gold, &DonateMoney, TO_CHAR );

    sprintf( buf, "�P��%s���F %d ��Ȥl��%s����|\e[0m�T"
      , mob_name( NULL, ch ), gold, mud_name );

    talk_channel_2( buf, CHANNEL_BULLETIN, "����" );

    refresh_donate();
  }

  else if ( !str_prefix( arg, "withdraw" ) )
  {
    if ( DonateLock )
    {
      send_to_char( "�o�ӥ\\��b��w�ɤ������T\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->in_room->DepositMoney )
    {
      send_to_char( "�o�̨ä��O������T\n\r" , ch );
      RETURN_NULL();
    }

    if ( !can_donate( ch, TRUE ) ) RETURN_NULL();

    gold_to_char( ch, DonateBenifit );
    DonateMoney -= DonateBenifit;
    ch->donate   = DonateDuration;

    act( "$n�q$t����|��F$I�⪺���٪��M�Ʊ�$n��n�n�Q��"
      , ch, mud_name, &DonateBenifit, TO_ALL );

    refresh_donate();
  }

  else if ( !str_prefix( arg, "lock" ) && IS_IMMORTAL( ch ) )
  {
    DonateLock = !DonateLock;
    act( "�A$1$t$0$T����|���\\��T"
      , ch, DonateLock ? "����" : "�}��", mud_name, TO_CHAR );
  }

  else if ( !str_prefix( arg, "set" ) && IS_IMMORTAL( ch ) )
  {
    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg1 );

    if ( arg[0] == '\x0' || arg1[0] == '\x0' )
    {
      send_to_char( "�A���Ѽƿ��~�M�Ьd�� donate �����T�ϥΤ�k�T\n\r", ch );
      RETURN_NULL();
    }

    if ( !is_number( arg1 ) )
    {
      send_to_char( "�A���ĤG�ӰѼƥ����O�Ʀr�T\n\r", ch );
      RETURN_NULL();
    }

    number = atoi( arg1 );

    if ( !str_prefix( arg, "duration" ) )
    {
      if ( number < 0 )
      {
        send_to_char( "�A�����j�ɶ����X�k�T\n\r", ch );
        RETURN_NULL();
      }

      DonateDuration = number;
      refresh_donate();
      act( "�A�����|�����j�אּ$i�p�ɡT", ch, &DonateDuration, NULL, TO_CHAR );
    }

    else if ( !str_prefix( arg, "benifit" ) )
    {
      if ( number < 0 )
      {
        send_to_char( "�A�����٪����B���X�k�T\n\r", ch );
        RETURN_NULL();
      }

      DonateBenifit = number;
      refresh_donate();
      act( "�A�����|�����٪��אּ$i��T", ch, &DonateBenifit, NULL, TO_CHAR );
    }

    else if ( !str_prefix( arg, "level" ) )
    {
      if ( number < 0 || number > LEVEL_HERO )
      {
        send_to_char( "�A�����٪����Ť��X�k�T\n\r", ch );
        RETURN_NULL();
      }

      DonateLevel = number;
      refresh_donate();
      act( "�A�����|�����ŧאּ$i�šT", ch, &DonateLevel, NULL, TO_CHAR );
    }

   else if ( !str_prefix( arg, "limit" ) )
    {
      if ( number < 0 )
      {
        send_to_char( "�A�����٪��d�򤣦X�k�T\n\r", ch );
        RETURN_NULL();
      }

      DonateLimit = number;
      refresh_donate();
      act( "�A�����|�����ٽd��אּ$i��T", ch, &DonateLimit, NULL, TO_CHAR );
    }

    else
    {
      send_to_char( "�A���Ѽƿ��~�M�Ьd�� donate �����T�ϥΤ�k�T\n\r", ch );
    }
  }

  else if ( !str_prefix( arg, "setting" ) && IS_IMMORTAL( ch ) )
  {
    print_to_char( ch,
      "%s\e[0m����|���p�p�U�R\n\r%s"
      "����|�`���B  �R %d ��\n\r"
      "�C���o����B  �R %d ��Ȥl\n\r"
      "�o��̧C����  �R %d �ťH�U�C\n\r"
      "��H�`�겣����R %d ��H�U�C\n\r"
      "�o�񶡹j�ɶ�  �R %d �p�ɡC\n\r"
      "�Q�����`����  �R %d ���C\n\r"
      , mud_name
      , VERTICAL_LINE
      , DonateMoney
      , DonateBenifit
      , DonateLevel
      , DonateLimit
      , DonateDuration
      , DonateCount );
  }

  else
  {
    send_to_char( "�A���Ѽƿ��~�M�Ьd�� donate �����T�ϥΤ�k�T\n\r", ch );
  }
  RETURN_NULL();
}

void refresh_donate( void )
{
  FILE * pFile;

  PUSH_FUNCTION( "refresh_donate" );

  if ( ( pFile = FOPEN( donate_file, "w" ) ) )
  {
    fprintf( pFile, "Money           %d\n"
                    "Benifit         %d\n"
                    "Level           %d\n"
                    "Limit           %d\n"
                    "Duration        %d\n"
                    "Count           %d\n"
                    "End\n"
      , DonateMoney, DonateBenifit, DonateLevel
      , DonateLimit, DonateDuration, DonateCount );

    FCLOSE( pFile );
  }

  else
  {
    mudlog( LOG_DEBUG, "refresh_donate: ��s�����ɮץ���." );
  }

  RETURN_NULL();
}

bool can_donate( CHAR_DATA * ch, bool fShow )
{
  PUSH_FUNCTION( "can_donate" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "can_donate: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( IS_NPC( ch ) ) RETURN( FALSE );

  if ( IS_IMMORTAL( ch ) )
  {
    if ( fShow ) send_to_char( "�A�u��ʷ�����|�M���������٪��T\n\r", ch );
    RETURN( FALSE );
  }

  if ( ch->level > DonateLevel )
  {
    if ( fShow ) act( "�藍�_�M�A������$i�šM�L�k�~�������٪��T"
      , ch, &DonateLevel, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  if ( DonateMoney < DonateBenifit )
  {
    if ( fShow ) act( "�藍�_�M$t����|�̨S�����F�M�L�k�~����١T"
      , ch, mud_name, NULL, TO_CHAR );

      RETURN( FALSE );
  }

  if ( ch->donate > 0 )
  {
    if ( fShow ) send_to_char( "���~�⧹���٪��M���{�b�S�ӻ�F�O�S\n\r", ch );
    RETURN( FALSE );
  }

  if ( personal_asset( ch ) >= DonateLimit )
  {
    if ( fShow ) send_to_char( "�藍�_�M�A���A�Ω���٪��d�򤺡T\n\r", ch );
    RETURN( FALSE );
  }

  RETURN( TRUE );
}

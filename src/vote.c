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
#include <time.h>
#include <dirent.h>
#include "merc.h"

void vote_attach        args( ( CHAR_DATA * ) );
char *  show_vote_all   args( ( CHAR_DATA *, VOTE_DATA * ) );
void load_a_vote        args( ( VOTE_DATA *, FILE * ) );
bool write_vote_to_file args( ( VOTE_DATA * ) );
bool set_vote           args( ( CHAR_DATA *, VOTE_DATA *, int ) );
bool can_vote           args( ( CHAR_DATA *, VOTE_DATA * ) );
bool show_vote          args( ( CHAR_DATA *, VOTE_DATA * ) );
int  voted_char         args( ( VOTE_DATA * ) );

FUNCTION( do_vote )
{
  VOTE_DATA * pVote;
  CLUB_DATA * pClub;
  char      * strtime;
  char        arg[MAX_INPUT_LENGTH];
  char        buf[MAX_STRING_LENGTH];
  int         loop;
  int         count;

  PUSH_FUNCTION( "do_vote" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' || !str_prefix( arg, "list" ) )
  {
    clear_buffer();
    for ( loop = 0, pVote = vote_list; pVote; pVote = pVote->next )
    {
      str_cpy( buf, pVote->subject );
      buf[33] = '\x0';

      send_to_buffer( "\e[1;36m%3d\e[0m. ���šR%3d �|��̡R\e[1;32m%-12s\e[0m "
        "[%s] �D���R\e[1;34m%s\e[0m\n\r"
        , loop++, pVote->level, pVote->poster
        , YESNO( can_vote( ch, pVote ) )
        , buf );
    }

    if ( loop == 0 ) send_to_buffer( "�ثe�S���|�����벼�T\n\r" );
    print_buffer( ch );

    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "subject" ) )
  {
    if ( !argument || !*argument )
    {
      send_to_char( "�A���������D���T\n\r", ch );
      RETURN_NULL();
    }

    ansi_transcribe( argument, buf );
    smash_tilde( buf );
    vote_attach( ch );
    free_string( ch->vote->subject );
    ch->vote->subject = str_dup( buf );
    send_to_char( "�����]�w�벼�D���C\n\r", ch );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "text" ) )
  {
    if ( !ch->desc ) RETURN_NULL();

    vote_attach( ch );

    ch->desc->edit_mode = EDIT_VOTE;
    show_editor( ch );

    free_string( ch->stack );
    ch->stack = str_dup( "" );

    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "post" ) )
  {
    if ( !( pVote = ch->vote ) )
    {
      send_to_char( "�A�|���]�w�n�A���ﶵ�M�ҥH�L�k�|��T\n\r", ch );
      RETURN_NULL();
    }

    if ( !pVote->subject || !*pVote->subject )
    {
      send_to_char( "�A�o���벼���D���O����M�A�o�]�w�n�T\n\r", ch );
      RETURN_NULL();
    }

    if ( !pVote->text || !*pVote->text )
    {
      send_to_char( "�A�o���벼���N�q�O����M�A�o�]�w�n���T\n\r", ch );
      RETURN_NULL();
    }

    for ( loop = count = 0; loop < MAX_VOTES; loop++ )
      if ( pVote->message[loop] && *( pVote->message[loop] ) ) count++;

    if ( count <= 1 )
    {
      send_to_char( "�A�벼���ﶵ�|���]�w�n�M�L�k�|��T\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->gold < VoteGold )
    {
      act( "�藍�_�M�A���W������$i��Ȥl�M����|��벼�T"
        , ch, &VoteGold, NULL, TO_CHAR );

      RETURN_NULL();
    }

    ch->vote->next              = NULL;
    strtime                     = ctime( &current_time );
    strtime[str_len(strtime)-1] = '\x0';
    ch->vote->date              = str_dup( strtime );
    ch->vote->stamp             = (int) current_time;

    if ( !vote_list )
    {
      vote_list = ch->vote;
    }

    else
    {
      for ( pVote = vote_list; pVote->next; pVote = pVote->next );
      pVote->next = ch->vote;
    }

    pVote    = ch->vote;
    ch->vote = NULL;

    if ( !write_vote_to_file( pVote ) )
    {
      send_to_char( "�|��벼���ѡM�t�εL�k�}�ҡT\n\r", ch );
      RETURN_NULL();
    }

    send_to_char( "�����|��벼����C\n\r", ch );

    sprintf( buf, "%s�|��F�@������%s\e[0m���벼�M�Фj�a"
      "���D�ѥ[�M���¡T", mob_name( NULL, ch ), pVote->subject );

    talk_channel_2( buf, CHANNEL_BULLETIN, "�벼" );

    gold_from_char( ch, VoteGold );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "set" ) )
  {
    vote_attach( ch );

    argument = one_argument( argument, arg );

    if ( arg[0] == '\x0' || !is_number( arg ) )
    {
      send_to_char( "�A�n�]�w���@�ӿﶵ�O�S\n\r", ch );
      RETURN_NULL();
    }

    loop = atoi( arg ) - 1;
    if ( loop < 0 || loop >= MAX_VOTES )
    {
      print_to_char( ch, "�Ĥ@�ӰѼƥu��� 1 �� %d �Ӥw�T\n\r", MAX_VOTES );
      RETURN_NULL();
    }

    if ( !argument || !*argument )
    {
      send_to_char( "�A�n��諸���جO����O�S\n\r", ch );
      RETURN_NULL();
    }

    ansi_transcribe( argument, buf );
    smash_tilde( buf );

    free_string( ch->vote->message[loop] );
    ch->vote->message[loop] = str_dup( buf );
    send_to_char( "�����벼��ﶵ�ءT\n\r", ch );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "show" ) )
  {
    if ( !ch->vote )
    {
      send_to_char( "�A�S���|��L���󪺧벼�M�L�k��ݡT\n\r", ch );
      RETURN_NULL();
    }

    send_to_char( show_vote_all( ch, ch->vote ), ch );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "clear" ) )
  {
    if ( !( pVote = ch->vote ) )
    {
      send_to_char( "�A����{�̨S���|�����벼�M�ҥH�L�k�M���T\n\r", ch );
      RETURN_NULL();
    }

    free_string( pVote->poster  );
    free_string( pVote->subject );
    free_string( pVote->text    );
    free_string( pVote->date    );
    free_string( pVote->club    );

    for ( loop = 0; loop < MAX_VOTES; loop++ )
      free_string( pVote->message[loop] );

    for ( loop = 0; loop < MAX_POLL; loop++ )
      free_string( pVote->poller[loop] );

    /* ����p���� */
    free_struct( ch->vote, STRUCT_VOTE_DATA );
    ch->vote = NULL;

    send_to_char( "�w�g�M���벼�]�w�����e�T\n\r", ch );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "club" ) )
  {
    if ( !( pVote = ch->vote ) )
    {
      send_to_char( "�A����{�̨S���|�����벼�T\n\r", ch );
      RETURN_NULL();
    }

    if ( !( pClub = ch->club ) || !pClub->name || !*pClub->name )
    {
      send_to_char( "�藍�_�M�A�S���ѥ[�����M�L�k�]�w�T\n\r", ch );
      RETURN_NULL();
    }

    free_string( pVote->club );
    pVote->club = str_dup( pClub->name );

    send_to_char( "�A�]�w�o���벼�������벼�T", ch );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "days" ) )
  {
    if ( !( pVote = ch->vote ) )
    {
      send_to_char( "�A����{�̨S���|�����벼�T\n\r", ch );
      RETURN_NULL();
    }

    if ( !argument || !*argument || !is_number( argument ) )
    {
      send_to_char( "�A�Q�X�ѫ�}���O�S\n\r", ch );
      RETURN_NULL();
    }

    count = atoi( argument );

    if ( count < VOTE_MIN_DAYS || count > VOTE_DAYS )
    {
      send_to_char( "�A�]�w���i�벼������X�z�T\n\r", ch );
      RETURN_NULL();
    }

    pVote->days = count;
    act( "�A�]�w�o���벼$i�ѫ�}���T", ch, &count, NULL, TO_CHAR );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "limit" ) )
  {
    if ( !( pVote = ch->vote ) )
    {
      send_to_char( "�A����{�̨S���|�����벼�T\n\r", ch );
      RETURN_NULL();
    }

    if ( !argument || !*argument || !is_number( argument ) )
    {
      send_to_char( "�A����o�O���Ŧh�֪��벼�O�S\n\r", ch );
      RETURN_NULL();
    }

    count = atoi( argument );

    if ( count < VOTE_LEVEL  || count > LEVEL_HERO )
    {
      send_to_char( "�A������Ť��X�z��T\n\r", ch );
      RETURN_NULL();
    }

    pVote->level = count;
    act( "�A����A���벼��������$i�H�W��i���T", ch, &count, NULL, TO_CHAR );

    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "read" ) )
  {
    if ( !argument || !*argument || !is_number( argument ) )
    {
      send_to_char( "�A�nŪ���@�ӧ벼�����e�O�S\n\r", ch );
      RETURN_NULL();
    }

    count = atoi( argument );

    for ( loop = 0, pVote = vote_list; pVote; pVote = pVote->next, loop++ )
    {
      if ( loop == count )
      {
        if ( pVote->lock )
        {
          send_to_char( "�藍�_�M�o�Ӷ��سQ��w�F�M�L�kŪ����T\n\r", ch );
          RETURN_NULL();
        }

        send_to_char( show_vote_all( ch, pVote ), ch );
        RETURN_NULL();
      }
    }

    send_to_char( "�藍�_�M�S�������벼�T\n\r", ch );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "moninal" ) )
  {
    if ( !( pVote = ch->vote ) )
    {
      send_to_char( "�A����{�̨S���|�����벼�T\n\r", ch );
      RETURN_NULL();
    }

    if ( pVote->moninal )
    {
      pVote->moninal = FALSE;
      send_to_char( "�A��A���벼�令���O�W�벼�T\n\r", ch );
    }

    else
    {
      pVote->moninal = TRUE;
      send_to_char( "�A��A���벼�令�O�W�벼�T\n\r", ch );
    }
    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "lock" ) && IS_IMMORTAL( ch ) )
  {
    if ( !argument || !*argument || !is_number( argument ) )
    {
      send_to_char( "�A�n��w���@�ӧ벼�O�S\n\r", ch );
      RETURN_NULL();
    }

    count = atoi( argument );
    for ( loop = 0, pVote = vote_list; pVote; pVote = pVote->next, loop++ )
    {
      if ( loop == count )
      {
        if ( pVote->lock )
        {
          pVote->lock = FALSE;
          act( "�A�Ѱ��F$t���벼��w�T", ch, pVote->subject, NULL, TO_CHAR );
        }
        else
        {
          pVote->lock = TRUE;
          act( "�A�]�w�F$t���벼��w�T", ch, pVote->subject, NULL, TO_CHAR );
        }

        write_vote_to_file( pVote );
        RETURN_NULL();
      }
    }

    send_to_char( "�藍�_�M�S�������벼�T\n\r", ch );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "poll" ) )
  {
    argument = one_argument( argument, arg );

    if ( arg[0] == '\x0' || !is_number( arg ) )
    {
      send_to_char( "�A�n����@�������|�O�S\n\r", ch );
      RETURN_NULL();
    }

    count = atoi( arg );

    for ( loop = 0, pVote = vote_list; pVote; pVote = pVote->next, loop++ )
      if ( loop == count ) break;

    if ( !pVote )
    {
      send_to_char( "�藍�_�M�S�������벼�T\n\r", ch );
      RETURN_NULL();
    }

    if ( pVote->lock )
    {
      send_to_char( "�藍�_�M�o�ӧ벼�@�~�Q�ȮɸT��T\n\r", ch );
      RETURN_NULL();
    }

    if ( !can_vote( ch, pVote ) )
    {
      send_to_char( "�藍�_�M�A����ѻP�o�ӿ��|�F�T\n\r", ch );
      RETURN_NULL();
    }

    if ( !argument || !*argument || !is_number( argument ) )
    {
      send_to_char( "�A�n�����@�ӿﶵ�O\n\r", ch );
      RETURN_NULL();
    }

    count = atoi( argument ) - 1;
    if ( count < 0
      || count >= MAX_VOTES
      || pVote->message[count] == NULL
      || *( pVote->message[count] ) == '\x0' )
    {
      send_to_char( "�藍�_�M���O�L�Ī��ﶵ�T\n\r", ch );
      RETURN_NULL();
    }

    set_vote( ch, pVote, count );
    write_vote_to_file( pVote );

    print_to_char( ch, "�A��%s���|�쪺����%s\e[0m���|�M���F��%d�M"
      "������%s���ﶵ�T\n\r"
      , pVote->poster, pVote->subject, count + 1, pVote->message[count] );

    sprintf( buf, "%s�H���[�M�ש�b\e[1;32m%s\e[0m�W���\\�W"
      "�L���t���@���T"
      , mob_name( NULL, ch ), pVote->subject );

    talk_channel_2( buf, CHANNEL_BULLETIN, "�벼" );

    RETURN_NULL();
  }

  else
  {
    send_to_char( "�A���Ѽƿ��~�M�Ьd�� vote ���ϥΤ�k�T\n\r", ch );
    RETURN_NULL();
  }

  RETURN_NULL();
}

void vote_attach( CHAR_DATA * ch )
{
  VOTE_DATA * pVote;

  PUSH_FUNCTION( "vote_attach" );

  if ( !ch || !ch->name )
  {
    mudlog( LOG_DEBUG, "vote_attach: �ӷ������T." );
    RETURN_NULL();
  }

  if ( IS_NPC( ch ) || ch->vote ) RETURN_NULL();

  set_vote_default( pVote = alloc_struct( STRUCT_VOTE_DATA ) );

  pVote->poster  = str_dup( ch->name );
  ch->vote       = pVote;

  RETURN_NULL();
}

char * show_vote_all( CHAR_DATA * ch, VOTE_DATA * pVote )
{
  static char   buf[MAX_STRING_LENGTH];
  char        * strtime;
  int           loop;
  int           count;
  time_t        end_time;
  int           iLine;

  PUSH_FUNCTION( "show_vote_all" );

  if ( !pVote )
  {
    mudlog( LOG_DEBUG, "show_vote_all: �ӷ������T�T" );
    RETURN( "" );
  }

  end_time = pVote->stamp + pVote->days * 24 * 60 * 60;
  strtime = ctime( &end_time );
  strtime[str_len(strtime)-1] = '\x0';

  clear_stack();
  send_to_stack(
    "�|��벼��  �R%s\e[0m\n\r"
    "�벼�D��    �R%s\e[0m\n\r"
    "�|����    �R%s\n\r"
    "�I����    �R%s\n\r"
    "�|����    �R%d ��\n\r"
    "�����벼    �R%s\n\r"
    "�w�벼�H��  �R%d\n\r"
    "�벼�̧C���šR%d\n\r"
    "�벼�i�椤  �R%s\n\r"
    "�O�_���벼�v�R%s\n\r"
    "�O�W�벼    �R%s\n\r"
    "�벼�N�q    �R\n\r%s%s%s"
    , pVote->poster
    , pVote->subject && *pVote->subject ? pVote->subject : "�|���M�w"
    , pVote->date && *pVote->date ? pVote->date : "�|���M�w"
    , pVote->stamp > 0  ? strtime : "�|���M�w"
    , pVote->days
    , pVote->club && *pVote->club ? pVote->club : "�D�����벼"
    , voted_char( pVote )
    , pVote->level
    , YESNO( !vote_timeup( pVote ) )
    , ch ? YESNO( can_vote( ch, pVote ) ) : "�S"
    , YESNO( pVote->moninal )
    , VERTICAL_LINE, pVote->text, VERTICAL_LINE );

  for ( loop = 0; loop < MAX_VOTES; loop++ )
  {
    if ( !pVote->message[loop] || !*pVote->message[loop] ) continue;
    send_to_stack( "�ﶵ %2d�R%3d �� %s\e[0m\n\r"
      , loop + 1
      , ( !ch || show_vote( ch, pVote ) ) ? pVote->poll[loop] : 0
      , pVote->message[loop] );
  }

  if ( vote_timeup( pVote ) && pVote->moninal )
  {
    send_to_stack( "\n\r" );
    for ( count = 0; count < MAX_VOTES; count++ )
    {
      if ( !pVote->message[count] || !*pVote->message[count] ) continue;

      send_to_stack( "�뵹�ﶵ%d(%s\e[0m)���H���R"
        , count + 1, pVote->message[count] );

      for ( iLine = loop = 0; loop < MAX_POLL; loop++ )
      {
        if ( pVote->vote[loop] == count
          && pVote->poller[loop]
          && *( pVote->poller[loop] ) )
        {
          if ( ( iLine++ % 6 ) == 0 ) send_to_stack( "\n\r" );
          send_to_stack( "%-12s", pVote->poller[loop] );
        }
      }

      if ( iLine == 0 ) send_to_stack( "\n\r�S���H��o�ӿﶵ�T\n\r\n\r" );
      else              send_to_stack( "\n\r\n\r" );
    }
  }

  str_ncpy( buf, return_stack(), sizeof( buf ) - 10 );
  RETURN( buf );
}

bool write_vote_to_file( VOTE_DATA * pVote )
{
  char   filename[MAX_FILE_LENGTH];
  int    loop;
  FILE * pFile;

  PUSH_FUNCTION( "write_vote_to_file" );

  if ( !pVote )
  {
    mudlog( LOG_DEBUG, "write_vote_to_file: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  sprintf( filename, "%s/%d.vot", vote_dir, pVote->stamp );

  if ( ( pFile = FOPEN( filename , "w" ) ) )
  {
    fprintf( pFile, "Poster          %s~\n", pVote->poster  );
    fprintf( pFile, "Subject         %s~\n", pVote->subject );
    fprintf( pFile, "Club            %s~\n", pVote->club    );
    fprintf( pFile, "Text\n%s~\n"          , pVote->text    );
    fprintf( pFile, "Date            %s~\n", pVote->date    );
    fprintf( pFile, "Days            %d\n" , pVote->days    );
    fprintf( pFile, "Stamp           %d\n" , pVote->stamp   );
    fprintf( pFile, "Level           %d\n" , pVote->level   );
    fprintf( pFile, "Lock            %d\n" , pVote->lock    );
    fprintf( pFile, "Moninal         %d\n" , pVote->moninal );

    for ( loop = 0; loop < MAX_VOTES; loop++ )
    {
      if ( pVote->message[loop] && *( pVote->message[loop] ) )
      {
        fprintf( pFile, "Vote            %d %s~\n"
          , loop, pVote->message[loop] );
      }
    }

    for ( loop = 0; loop < MAX_POLL; loop++ )
    {
      if ( pVote->poller[loop] && *( pVote->poller[loop] ) )
      {
        fprintf( pFile, "Ballot          %d %s~\n"
          , pVote->vote[loop], pVote->poller[loop] );
      }
    }

    fprintf( pFile, "End\n" );
    FCLOSE( pFile );

    /* �����ɮצs���Ҧ� */
    set_file_mode( filename );
    RETURN( TRUE );
  }

  RETURN( FALSE );
}

bool can_vote( CHAR_DATA * ch, VOTE_DATA * pVote )
{
  CLUB_DATA * pClub;
  int         loop;
  int         hold_times;
  bool        bVote;

  PUSH_FUNCTION( "can_vote" );

  if ( !ch || !ch->name || !pVote )
  {
    mudlog( LOG_DEBUG, "can_vote: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  /* �Ǫ�����벼 */
  if ( IS_NPC( ch ) ) RETURN( FALSE );

  /* �ݬݬO�_�Q��w�F */
  if ( pVote->lock ) RETURN( FALSE );

  /* �ݬݬO�_�w�g�L�� */
  hold_times = pVote->stamp + pVote->days * 24 * 60 * 60;
  if ( current_time > hold_times ) RETURN( FALSE );

  /* �O�_�������벼 */
  if ( pVote->club && *pVote->club )
  {
    if ( !( pClub = ch->club ) || !pClub->name || !*pClub->name )
      RETURN( FALSE );

    if ( str_cmp( pClub->name, pVote->club ) ) RETURN( FALSE );
  }

  /* �ݬݬO�_�٦��Ŧ� */
  for ( bVote = FALSE, loop = 0; loop < MAX_POLL; loop++ )
  {
    if ( pVote->poller[loop] && *( pVote->poller[loop] ) == '\x0' )
    {
      bVote = TRUE;
      break;
    }
  }

  if ( !bVote ) RETURN( FALSE );

  /* ���ŧC���]����벼 */
  if ( pVote->level > ch->level ) RETURN( FALSE );

  /* ���୫�Ƨ벼 */
  for ( loop = 0; loop < MAX_POLL; loop++ )
    if ( pVote->poller[loop] && !str_cmp( pVote->poller[loop], ch->name ) )
      RETURN( FALSE );

  RETURN( TRUE );
}

bool set_vote( CHAR_DATA * ch, VOTE_DATA * pVote, int choice )
{
  int loop;

  PUSH_FUNCTION( "set_vote" );

  if ( !ch || !ch->name || !pVote )
  {
    mudlog( LOG_DEBUG, "set_vote: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  choice = UMAX( 0, UMIN( MAX_VOTES - 1, choice ) );

  for ( loop = 0; loop < MAX_POLL; loop++ )
  {
    if ( pVote->poller[loop] && *( pVote->poller[loop] ) == '\x0' )
    {
      pVote->poller[loop] = str_dup( ch->name );
      pVote->vote[loop]   = choice;
      pVote->poll[choice]++;
      RETURN( TRUE );
    }
  }

  RETURN( FALSE );
}

int vote_count( CHAR_DATA * ch )
{
  int         count;
  VOTE_DATA * pVote;

  PUSH_FUNCTION( "vote_count" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "vote_count: �ʥF�ӷ�." );
    RETURN( 0 );
  }

  for ( count = 0, pVote = vote_list; pVote; pVote = pVote->next )
    if ( can_vote( ch, pVote ) ) count++;

  RETURN( count );
}

bool show_vote( CHAR_DATA * ch, VOTE_DATA * pVote )
{

  PUSH_FUNCTION( "show_vote" );

  if ( !ch || !pVote )
  {
    mudlog( LOG_DEBUG, "show_vote: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  /* �Ǫ��@�w����ݧ벼���G */
  if ( IS_NPC( ch ) ) RETURN( FALSE );

  /* ���ڤ@�w�i�H */
  if ( IS_IMMORTAL( ch ) ) RETURN( TRUE );

  if ( vote_timeup( pVote ) == TRUE ) RETURN( TRUE );
  RETURN( FALSE );
}

bool vote_timeup( VOTE_DATA * pVote )
{
  int  loop;
  int  hold_times;
  bool bVote;

  PUSH_FUNCTION( "vote_timeup" );

  if ( !pVote )
  {
    mudlog( LOG_DEBUG, "vote_timeup: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  /* �ɶ�����F */
  hold_times = pVote->stamp + pVote->days * 24 * 60 * 60;
  if ( current_time > hold_times ) RETURN( TRUE );

  /* �H�ƺ��F */
  for ( bVote = TRUE, loop = 0; loop < MAX_POLL; loop++ )
  {
    if ( pVote->poller[loop] == NULL
      || ( pVote->poller[loop] && *( pVote->poller[loop] ) == '\x0' ) )
    {
      bVote = FALSE;
      break;
    }
  }

  if ( bVote ) RETURN( TRUE );

  RETURN( FALSE );
}

int voted_char( VOTE_DATA * pVote )
{
  int count;
  int loop;

  PUSH_FUNCTION( "voted_char" );

  if ( !pVote )
  {
    mudlog( LOG_DEBUG, "voted_char: �ʥF�ӷ�." );
    RETURN( 0 );
  }

  for ( loop = count = 0; loop < MAX_POLL; loop++ )
    if ( pVote->poller[loop] && *( pVote->poller[loop] ) ) count++;

  RETURN( count );
}

void vote_to_note( VOTE_DATA * pVote )
{
  NOTE_DATA   * pNote;
  static int    count = 0;
  char          buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "vote_to_note" );

  if ( !pVote )
  {
    mudlog( LOG_DEBUG, "vote_to_note: �ӷ������T." );
    RETURN_NULL();
  }

  pNote             = alloc_struct( STRUCT_NOTE_DATA );
  pNote->sender     = str_dup( pVote->poster  );
  pNote->date_stamp = time( NULL ) + count++;

  sprintf( buf, "%d", ( int ) pNote->date_stamp );
  pNote->date = str_dup( buf );

  if ( pVote->club && *pVote->club )
  {
    sprintf( buf, "@%s", pVote->club );
    pNote->to_list = str_dup( buf );
  }
  else
  {
    pNote->to_list = str_dup( "all" );
  }

  sprintf( buf, "%s/%d.%s", note_dir, ( int ) pNote->date_stamp, note_ext );
  pNote->filename = str_dup( buf );

  sprintf( buf, "(\e[1;32m�벼���G\e[0m) %s", pVote->subject );
  pNote->subject = str_dup( buf );
  pNote->text    = str_dup( show_vote_all( NULL, pVote ) );

  write_note( pNote );

  free_string( pNote->filename );
  free_string( pNote->sender   );
  free_string( pNote->date     );
  free_string( pNote->to_list  );
  free_string( pNote->subject  );
  free_string( pNote->text     );

  free_struct( pNote, STRUCT_NOTE_DATA );

  RETURN_NULL();
}

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
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
#include "merc.h"

BOARD_DATA *    load_board_index        args( ( const char * ) );
void            load_post               args( ( const char *, BOARD_DATA * ) );
POST_DATA  *    load_a_post             args( ( const char * ) );
void            set_board_default       args( ( BOARD_DATA * ) );
void            set_post_default        args( ( POST_DATA * ) );
void            show_a_post             args( ( POST_DATA *, CHAR_DATA * ) );
void            purge_post              args( ( BOARD_DATA * ) );
void            attach_post             args( ( POST_DATA *, BOARD_DATA *, bool ) );
void            remove_post             args( ( POST_DATA * ) );
void            write_post              args( ( POST_DATA * ) );
void            rewrite_board_list      args( ( BOARD_DATA * ) );
const char *    post_status             args( ( POST_DATA *, int ) );

#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value ) \
  if ( !str_cmp( word, literal ) )   \
  {                                  \
    field  = value;                  \
    fMatch = TRUE;                   \
    break;                           \
  }

#define POSTER_FIELD    30
#define TITLE_FIELD     30
#define MAX_RESPONSE    500

BOARD_DATA * board_lookup( int slot )
{
  BOARD_DATA * pBoard;

  PUSH_FUNCTION( "board_lookup" );

  for ( pBoard = board_list; pBoard; pBoard = pBoard->next )
    if ( pBoard->slot == slot ) RETURN( pBoard );

  RETURN( NULL );
}

FUNCTION( do_board )
{
  BOARD_DATA * pBoard;
  char         arg[MAX_INPUT_LENGTH];
  int          count;

  PUSH_FUNCTION( "do_board" );

  if ( !board_list )
  {
    act( "$t�̨S�����󪺪��T", ch, mud_name, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( IS_NPC( ch ) || !ch->pcdata ) RETURN_NULL();

  argument = one_argument( argument, arg );
  if ( arg[0] == '\x0' )
  {
    clear_buffer();
    send_to_buffer( "\e[1;33;44m�s�� ��      �W �ж����X Ū�� "
      "�g�J �ƶq ���� ��w                               \e[0m\n\r" );

    for ( count = 0, pBoard = board_list; pBoard; pBoard = pBoard->next )
    {
      send_to_buffer( "%4d %10s %8d %4d %4d %4d %4d  %2s\n\r"
        , pBoard->slot
        , pBoard->name
        , pBoard->location ? pBoard->location->vnum : -1
        , pBoard->read_level
        , pBoard->write_level
        , post_count( pBoard, POST_ALL )
        , pBoard->capcity
        , YESNO( pBoard->lock ) );

      if ( buffer_full() ) break;
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "!lock" ) && IS_IMMORTAL( ch ) )
  {

    if ( !ch->in_room || !( pBoard = ch->in_room->board ) )
    {
      send_to_char( "�A�o�Ӧa��S���d�����T\n\r", ch );
      RETURN_NULL();
    }

    pBoard->lock = pBoard->lock ? FALSE : TRUE;

    act( "$n$t$T�d�����T"
      , ch, pBoard->lock ? "����" : "�}��", pBoard->name, TO_ALL );

    RETURN_NULL();
  }

  else if ( is_number( arg ) )
  {
    if ( !( pBoard = board_lookup( atoi( arg ) ) ) )
    {
      send_to_char( "�S�����ӽs���������T\n\r", ch );
      RETURN_NULL();
    }

    print_to_char( ch,
      "�����s��  �R%d\n\r"
      "�����W��  �R%s\n\r"
      "�w��r��  �R%s\n\r"
      "�Ҧb�a���X�R%d\n\r"
      "�C���ɮ�  �R%s\n\r"
      "���|�W��  �R%s\n\r"
      "Ū������  �R%d\n\r"
      "�g�J����  �R%d\n\r"
      "�e�q����  �R%d\n\r"
      "�ثe�ƶq  �R%d\n\r"
      "�O�_�Q��w�R%s\n\r"
      , pBoard->slot
      , pBoard->name
      , pBoard->welcome
      , pBoard->location ? pBoard->location->vnum : -1
      , pBoard->listfile
      , pBoard->pathname
      , pBoard->read_level
      , pBoard->write_level
      , pBoard->capcity
      , post_count( pBoard, POST_ALL )
      , YESNO( pBoard->lock ) );

    RETURN_NULL();
  }

  send_to_char( "�Ѽƿ��~�M�Ьd�߫��O���Ϊk�T\n\r", ch );
  RETURN_NULL();
}

void open_board_directory( void )
{
  char         directory    [ MAXNAMLEN ];
  char         buf          [ MAXNAMLEN ];
  char         filename     [ MAXNAMLEN ];
  char         work_dir_name[ MAXNAMLEN ];
  char         list_file    [ MAXNAMLEN ];
  char         index        [ MAXNAMLEN ];
  char       * word;
  FILE_DATA  * pFile;
  BOARD_DATA * pBoard;

  PUSH_FUNCTION( "open_board_directory" );

  /* �����ؿ��åB��ݬO�_�n��J�ؿ����O�� */
  fill_path( str_cpy( directory, board_dir ) );

  /* ���ϰ��ɮץؿ��C���� */
  sprintf( filename , "%s%s" , directory , board_lst );

  /* �ݬݬO�_�i�H�}�� */
  if ( ( pFile = f_open( filename , "r" ) ) )
  {
    /* �@��Ū���ɮץ��ݤ~���� */
    while ( !if_eof( pFile ) )
    {
      word = fread_word( pFile );

      /* �@��Ū�� */
      fread_to_eol( pFile );

      if ( word[0] == '*' ) continue;

      /* Ū�J�Q�nŪ���������ؿ� */
      sprintf( buf , "%s%s" , directory , word );

      /* �ˬd�O�_���i�H�s�����l�ؿ� */
      if ( is_directory( buf ) )
      {
        /* �A��ݬO�_�������� */
        sprintf( work_dir_name , "%s%s" , directory , word );
        fill_path( work_dir_name );
        sprintf( index , "%s%s" , work_dir_name , board_index );

        /* �p�G�������ɫhŪ�J�Ҧ���� */
        if ( is_regular( index ) )
        {
          if ( !( pBoard = load_board_index( work_dir_name ) ) )
          {
            mudlog( LOG_ERR, "open_board_directory: �L�kŪ������%s."
              , work_dir_name );
          }

          /* �A��ݬO�_�������� */
          sprintf( list_file, "%s%s" , work_dir_name, board_sheet );

          /* �O�������� */
          pBoard->listfile = str_dup( list_file );
          pBoard->pathname = str_dup( work_dir_name );

          if ( is_regular( list_file ) )
          {
            load_post( work_dir_name, pBoard );
          }

          /* ���M�e�X���~���T�� */
          else
          {
            mudlog( LOG_ERR , "open_board_directory: �����ؿ� %s �S���M����"
              , work_dir_name );
          }
        }

        /* ���M�e�X���~���T�� */
        else
        {
          mudlog( LOG_ERR , "open_board_directory: �����ؿ� %s �S��������"
            , work_dir_name );
        }
      }

      /* �ϰ�ؿ��ڥ�����s�� */
      else
      {
        mudlog( LOG_ERR , "open_board_directory: �����ؿ� %s �L�k�s��" , buf );
      }
    }
    f_close( pFile );
  }

  /* �ڥ��S���ϰ�ؿ��C����, �e�X���~�T�� */
  else
  {
    mudlog( LOG_ERR , "�S�������l�ؿ��C����" );
  }

  RETURN_NULL();
}

BOARD_DATA * load_board_index( const char * pathname )
{
  char       * word;
  char         filename[ MAXNAMLEN ];
  bool         fMatch;
  FILE_DATA  * pFile;
  BOARD_DATA * pBoard;

  PUSH_FUNCTION( "load_board" );

  /* ���]�w�����ɪ��ɦW */
  sprintf( filename , "%s%s" , pathname , board_index );

  if ( !( pFile = f_open( filename , "r" ) ) )
  {
    mudlog( LOG_ERR, "load_board_index: �L�k�}�ү�����%s.", filename );
    RETURN( NULL );
  }

  set_board_default( ( pBoard = alloc_struct( STRUCT_BOARD_DATA ) ) );

  for ( ;; )
  {
    word   = if_eof( pFile ) ? "End" : fread_word( pFile );
    fMatch = FALSE;

    switch ( UPPER( word[0] ) )
    {
    /* ���� */
    case '*':
      fMatch = TRUE;
      fread_to_eol( pFile );
      break;

    case 'C':

      if ( !str_cmp( word, "Capcity" ) )
      {
        if ( ( pBoard->capcity = fread_number( pFile ) ) <= 0 )
          mudlog( LOG_DEBUG, "load_board_index: �����e�q�����T %d."
            , pBoard->capcity );

        fMatch = TRUE;
        break;
      }

      break;

    case 'E':

      if ( !str_cmp( word, "End" ) )
      {
        if ( !pBoard->location )
          mudlog( LOG_DEBUG, "load_board_index: �����S���ϰ�ж��C" );

        if ( !pBoard->name )
          mudlog( LOG_DEBUG, "load_board_index: �����S�������W�١C" );

        if ( !pBoard->welcome )
          mudlog( LOG_DEBUG, "load_board_index: �����S���w��T���C" );

        if ( IS_ERROR( pBoard->slot ) )
          mudlog( LOG_DEBUG, "load_board_index: �����S���s���C" );

        if ( IS_ERROR( pBoard->capcity ) )
          mudlog( LOG_DEBUG, "load_board_index: �����S���e�q����C" );

        if ( !board_list ) board_list      = pBoard;
        if ( board_last  ) board_last->next = pBoard;

        board_last              = pBoard;
        pBoard->next            = NULL;
        pBoard->location->board = pBoard;

        top_board++;
        f_close( pFile );
        RETURN( pBoard );
      }

      break;

    case 'L':

      if ( !str_cmp( word, "Location" ) )
      {
        int               iRoom;
        ROOM_INDEX_DATA * pRoom;

        iRoom = fread_number( pFile );

        if ( !( pRoom = get_room_index( iRoom ) ) )
          mudlog( LOG_DEBUG, "load_board_index: �S�� %d ���ж�.", iRoom );

        if ( pRoom->board )
          mudlog( LOG_DEBUG, "load_board_index: �ж� %d ��������.", iRoom );

        pBoard->location = pRoom;
        fMatch           = TRUE;
        break;
      }

      if ( !str_cmp( word, "Lock" ) )
      {
        pBoard->lock = fread_number( pFile ) == TRUE ? TRUE : FALSE;
        fMatch = TRUE;
        break;
      }
      break;

    case 'N':
      KEY( "Name", pBoard->name, fread_string( pFile ) );
      break;

    case 'R':

      if ( !str_cmp( word, "Read" ) )
      {
        int level;

        level = fread_number( pFile );
        if ( level < 0 || level > MAX_LEVEL )
          mudlog( LOG_DEBUG, "load_board_index: Ū������ %d ���X�z.", level );

        pBoard->read_level = level;
        fMatch             = TRUE;
        break;
      }
      break;

    case 'S':

      if ( !str_cmp( word, "Slot" ) )
      {
        int slot;

        if ( ( slot = fread_number( pFile ) ) < 0 || slot >= MAX_BOARD )
          mudlog( LOG_DEBUG, "load_board_index: �ж� %d �s�����X�z.", slot );

        if ( board_lookup( slot ) )
          mudlog( LOG_DEBUG, "load_board_index: �ж� %d �s������.", slot );

        pBoard->slot = slot;
        fMatch = TRUE;
        break;
      }
      break;

    case 'W':

      KEY( "Welcome", pBoard->welcome, fread_string( pFile ) );

      if ( !str_cmp( word, "Write" ) )
      {
        int level;

        level = fread_number( pFile );
        if ( level < 0 || level > MAX_LEVEL )
          mudlog( LOG_DEBUG, "load_board_index: �g�J���� %d ���X�z.", level );

        pBoard->write_level = level;
        fMatch              = TRUE;
        break;
      }
      break;
    }

    /* ���O���A���~ */
    if ( !fMatch )
      mudlog( LOG_DEBUG , "load_board_index: �R�O %s �����T." , word );
  }

  RETURN( NULL );
}

void load_post( const char * pathname, BOARD_DATA * pBoard )
{
  char      * word;
  char        filename[ MAXNAMLEN ];
  char        postfile[ MAXNAMLEN ];
  char        postname[ MAXNAMLEN ];
  bool        fMatch;
  FILE_DATA * pFile;
  POST_DATA * pPost;

  PUSH_FUNCTION( "load_post" );

  /* ���]�w�����ɪ��ɦW */
  sprintf( filename , "%s%s" , pathname , board_sheet );

  if ( !( pFile = f_open( filename , "r" ) ) )
  {
    mudlog( LOG_ERR, "load_post: �L�k�}�ҲM���� %s.", filename );
    RETURN_NULL();
  }

  for ( ;; )
  {
    word   = if_eof( pFile ) ? "End" : fread_word( pFile );
    fMatch = FALSE;

    switch ( UPPER( word[0] ) )
    {
    case EOF:
      f_close( pFile );
      RETURN_NULL();

    /* ���� */
    case '*':
      fMatch = TRUE;
      fread_to_eol( pFile );
      break;

    case 'E':

      if ( !str_cmp( word, "End" ) )
      {
        f_close( pFile );
        RETURN_NULL();
      }

      break;

    case 'F':

      if ( !str_cmp( word, "File" ) )
      {
        str_cpy( postname, fread_word( pFile ) );

        sprintf( postfile, "%s%s" , pathname, postname );

        if ( is_regular( postfile ) )
        {
          pPost           = load_a_post( postfile );
          pPost->filename = str_dup( postname );
        }

        else
        {
          mudlog( LOG_ERR , "load_post: �S�� %s �o�g�峹.", postfile );
          RETURN_NULL();
        }

        attach_post( pPost, pBoard, FALSE );
        fMatch = TRUE;
        break;
      }

      break;
    }

    /* ���O���A���~ */
    if ( !fMatch )
      mudlog( LOG_DEBUG , "load_post: �R�O %s �����T." , word );
  }

  RETURN_NULL();
}

POST_DATA * load_a_post( const char * filename )
{
  char      * word;
  bool        fMatch;
  FILE_DATA * pFile;
  POST_DATA * pPost;

  PUSH_FUNCTION( "load_a_post" );

  if ( !( pFile = f_open( filename , "r" ) ) )
  {
    mudlog( LOG_ERR, "load_a_post: �L�k�}�ү�����%s.", filename );
    RETURN( NULL );
  }

  set_post_default( ( pPost = alloc_struct( STRUCT_POST_DATA ) ) );

  for ( ;; )
  {
    word   = if_eof( pFile ) ? "End" : fread_word( pFile );
    fMatch = FALSE;

    switch ( UPPER( word[0] ) )
    {
    /* ���� */
    case '*':
      fMatch = TRUE;
      fread_to_eol( pFile );
      break;

    case 'A':
      KEY( "Attach", pPost->attach, fread_number( pFile ) );
      break;

    case 'E':

      if ( !str_cmp( word, "End" ) )
      {
        if ( !pPost->title )
          mudlog( LOG_DEBUG, "load_a_post: �峹�S�����D." );

        if ( !pPost->text )
          mudlog( LOG_DEBUG, "load_a_post: �峹�S������." );

        if ( !pPost->poster )
          mudlog( LOG_DEBUG, "load_a_post: �峹�S���o�H��." );

        if ( !pPost->owner )
          mudlog( LOG_DEBUG, "load_a_post: �峹�S���֦���." );

        if ( IS_ERROR( pPost->timer ) )
          mudlog( LOG_DEBUG, "load_a_post: �峹�S���ɶ�����." );

        f_close( pFile );
        RETURN( pPost );
      }

      break;

    case 'M':

      if ( !str_cmp( word, "Mark" ) )
      {
        pPost->mark = fread_number( pFile ) == TRUE ? TRUE : FALSE;
        fMatch      = TRUE;
        break;
      }

      break;

    case 'O':

      KEY( "Owner", pPost->owner, fread_string( pFile ) );
      break;

    case 'P':

      KEY( "Poster", pPost->poster, fread_string( pFile ) );
      break;

    case 'T':

      KEY( "Title", pPost->title, fread_string( pFile ) );
      KEY( "Timer", pPost->timer, fread_number( pFile ) );

      if ( !str_cmp( word, "Text" ) )
      {
        fread_to_eol( pFile );
        pPost->text = fread_string( pFile );
        fMatch = TRUE;
        break;
      }

      break;
    }

    /* ���O���A���~ */
    if ( !fMatch )
      mudlog( LOG_DEBUG , "load_a_post: �R�O %s �����T." , word );
  }

  RETURN( NULL );
}

void set_board_default( BOARD_DATA * pBoard )
{
  PUSH_FUNCTION( "set_board_default" );

  if ( pBoard )
  {
    pBoard->post      = NULL;
    pBoard->location     = NULL;
    pBoard->name         = NULL;
    pBoard->welcome      = NULL;
    pBoard->listfile     = NULL;
    pBoard->pathname     = NULL;
    pBoard->capcity      = ERRORCODE;
    pBoard->slot         = ERRORCODE;
    pBoard->read_level   = 1;
    pBoard->write_level  = level_limit;
    pBoard->lock         = FALSE;
  }

  RETURN_NULL();
}

void set_post_default( POST_DATA * pPost )
{
  PUSH_FUNCTION( "set_post_default" );

  if ( pPost )
  {
    pPost->board    = NULL;
    pPost->title    = NULL;
    pPost->text     = NULL;
    pPost->owner    = NULL;
    pPost->poster   = NULL;
    pPost->filename = NULL;
    pPost->timer    = ERRORCODE;
    pPost->attach   = 0;
    pPost->mark     = FALSE;
  }

  RETURN_NULL();
}

int post_count( BOARD_DATA * pBoard, int type )
{
  int         count = 0;
  POST_DATA * pPost;

  PUSH_FUNCTION( "post_count" );

  if ( !pBoard )
  {
    mudlog( LOG_DEBUG, "post_count: �ӷ������T." );
    RETURN( 0 );
  }

  switch( type )
  {
  case POST_ALL:
    for ( pPost = pBoard->post; pPost; pPost = pPost->next )
      count++;
    break;

  case POST_NOT_MARK:

    for ( pPost = pBoard->post; pPost; pPost = pPost->next )
      if ( !pPost->mark ) count++;
    break;

  case POST_MARK:
    for ( pPost = pBoard->post; pPost; pPost = pPost->next )
      if ( pPost->mark ) count++;
    break;
  }

  RETURN( count );
}

int unread_post( CHAR_DATA * ch, BOARD_DATA * pBoard )
{
  int         count;
  int         timer;
  POST_DATA * pPost;

  PUSH_FUNCTION( "unread_post" );

  if ( !ch || !pBoard )
  {
    mudlog( LOG_DEBUG, "unread_post: �ӷ������T." );
    RETURN( 0 );
  }

  if ( !ch->pcdata ) RETURN( 0 );

  timer = ch->pcdata->board[pBoard->slot];
  count = 0;

  for ( pPost = pBoard->post; pPost; pPost = pPost->next )
    if ( timer < pPost->timer ) count++;

  RETURN( count );
}

void show_board_title( CHAR_DATA * ch, BOARD_DATA * pBoard )
{
  POST_DATA * pPost;
  char      * timestr;
  int         count;
  time_t      timer;

  PUSH_FUNCTION( "show_board_title" );

  if ( !ch || !pBoard )
  {
    mudlog( LOG_DEBUG, "show_board_title: �ʥF�ӷ�." );
    RETURN_NULL();
  }

  if ( IS_NPC( ch ) || !ch->pcdata || !pBoard->post ) RETURN_NULL();

  clear_buffer();
  count = 0;

  send_to_buffer( "%s\n\r\e[1;33;44m ���� ��     �D                      "
    "�o     �H     ��               �o �H �� �� \e[0m\n\r",  pBoard->welcome );

  for ( pPost = pBoard->post; pPost; pPost = pPost->next )
  {
    timer   = ( time_t ) UMAX( 1, pPost->timer );
    timestr = ctime( &timer );
    timestr[ UMAX( 0, str_len( timestr ) - 9 ) ] = '\x0';

    send_to_buffer( "%s%4d "
      , post_status( pPost, ch->pcdata->board[pBoard->slot] ), ++count );

    send_to_buffer( "%s ", format_string(
      pPost->title && *pPost->title ? pPost->title : "(����)", TITLE_FIELD ) );

    send_to_buffer( "%s ", format_string(
      pPost->poster && *pPost->poster ? pPost->poster : "(����)"
      , POSTER_FIELD ) );

    send_to_buffer( "%s\n\r", timestr + 4 );
    if ( buffer_full() ) break;
  }

  print_buffer( ch );
  RETURN_NULL();
}

void show_a_post( POST_DATA * pPost, CHAR_DATA * ch )
{
  char   * timestr;
  time_t   timer;

  PUSH_FUNCTION( "show_a_post" );

  if ( !pPost || !ch )
  {
    mudlog( LOG_DEBUG, "show_a_post: �ʥF�ӷ�." );
    RETURN_NULL();
  }

  timer   = ( time_t ) UMAX( 1, pPost->timer );
  timestr = ctime( &timer );
  timestr[ UMAX( 0, str_len( timestr ) - 1 ) ] = '\x0';

  clear_buffer();

  send_to_buffer( "��  �D�R%s\n\r�o�H�̡R%s\e[0m\n\r��  ���R%s\n\r"
    , pPost->title, pPost->poster, timestr );

  if ( IS_IMMORTAL( ch ) ) send_to_buffer( "��  �סR%s\n\r�֦��̡R%s\n\r"
      , pPost->filename, pPost->owner );

  send_to_buffer( "��  �e�R\n\r%s%s\e[0m%s"
    , VERTICAL_LINE, pPost->text, VERTICAL_LINE );

  print_buffer( ch );

  RETURN_NULL();
}

FUNCTION( do_read )
{
  char         arg[MAX_INPUT_LENGTH];
  char         buf[MAX_STRING_LENGTH];
  int          slot;
  int          count;
  int          timer;
  BOARD_DATA * pBoard;
  POST_DATA  * pPost;

  PUSH_FUNCTION( "do_read" );

  if ( IS_NPC( ch ) || !ch->in_room || !ch->pcdata ) RETURN_NULL();

  argument = one_argument( argument, arg );

  if ( !( pBoard = ch->in_room->board ) )
  {
    send_to_char( "�藍�_�M�o�̨S���d���O�T\n\r", ch );
    RETURN_NULL();
  }

  if ( !pBoard->post )
  {
    send_to_char( "�藍�_�M�o�ӯd���O�S������d���T\n\r", ch );
    RETURN_NULL();
  }

  if ( ( pBoard->lock || ch->level < pBoard->read_level ) && !IS_IMMORTAL( ch ) )
  {
    send_to_char( "�藍�_�M�o�ӯd���O�A�O����Ū���T\n\r", ch );
    RETURN_NULL();
  }

  if ( arg[0] )
  {
    if ( !str_prefix( arg, "!all" ) )
    {
      for ( pPost = pBoard->post; pPost; pPost = pPost->next )
      {
        ch->pcdata->board[pBoard->slot]
          = UMAX( pPost->timer, ch->pcdata->board[pBoard->slot] );
      }

      send_to_char( "�A�N�o�̩Ҧ����d���]���w�gŪ���T\n\r", ch );
      RETURN_NULL();
    }

    else if ( !str_prefix( arg, "!mark" ) && IS_IMMORTAL( ch ) )
    {
      one_argument( argument, arg );
      if ( !arg[0] || !is_number( arg ) )
      {
        send_to_char( "�A�n�аO���@�g�峹�O�S\n\r", ch );
        RETURN_NULL();
      }

      slot = atoi( arg );
      chinese_number( slot, buf );

      for ( pPost = pBoard->post; pPost; pPost = pPost->next )
      {
        if ( --slot == 0 )
        {
          pPost->mark = pPost->mark ? FALSE : TRUE;

          act( "�A���$t�g�峹$T�аO�T", ch, buf
            , pPost->mark ? "�]�w" : "�Ѱ�", TO_CHAR );

          write_post( pPost );
          RETURN_NULL();
        }
      }

      send_to_char( "�S�����A�n�аO���峹�s���T\n\r", ch );
      RETURN_NULL();
    }

    else if ( !str_prefix( arg, "!delete" ) )
    {
      one_argument( argument, arg );
      if ( !arg[0] || !is_number( arg ) )
      {
        send_to_char( "�A�n�R�����@�g�峹�O�S\n\r", ch );
        RETURN_NULL();
      }

      slot = atoi( arg );
      chinese_number( slot, buf );

      for ( pPost = pBoard->post; pPost; pPost = pPost->next )
      {
        if ( --slot == 0 )
        {
          if ( pPost->mark )
          {
            send_to_char( "�o�@�g�峹�Q�аO�M����R���C\n\r", ch );
            RETURN_NULL();
          }

          if ( !IS_IMMORTAL( ch ) && str_cmp( pPost->owner, ch->name ) )
          {
            send_to_char( "�A�S���v�O�R���o�g�峹�T\n\r", ch );
            RETURN_NULL();
          }

          pBoard = pPost->board;
          remove_post( pPost );

          /* ���s��s�C�� */
          rewrite_board_list( pBoard );

          act( "�A�R����$t�g�峹�T", ch, buf, NULL, TO_CHAR );
          RETURN_NULL();
        }
      }

      send_to_char( "�S�����A�n�R�����峹�s���T\n\r", ch );
      RETURN_NULL();
    }

    else if ( !is_number( arg ) )
    {
      send_to_char( "�A������J�Ʀr�Ӫ�ܽs���T\n\r", ch );
      RETURN_NULL();
    }

    slot = atoi( arg );
  }

  else
  {
    timer = ch->pcdata->board[pBoard->slot];
    slot = 1;

    for ( pPost = pBoard->post; pPost; pPost = pPost->next, slot++ )
      if ( timer < pPost->timer ) break;

    if ( !pPost )
    {
      send_to_char( "�A�w�gŪ���o�̪��Ҧ��d���T\n\r", ch );
      RETURN_NULL();
    }
  }

  for ( count = 0, pPost = pBoard->post; pPost; pPost = pPost->next )
  {
    if ( ++count == slot )
    {
      show_a_post( pPost, ch );

      ch->pcdata->board[pBoard->slot] =
        UMAX( pPost->timer, ch->pcdata->board[pBoard->slot] );

      RETURN_NULL();
    }
  }

  send_to_char( "�藍�_�M�S�����g�d���C\n\r", ch );
  RETURN_NULL();
}

FUNCTION( do_post )
{
  char         arg[MAX_INPUT_LENGTH];
  char         buf[MAX_STRING_LENGTH];
  char         buf1[MAX_STRING_LENGTH];
  char         buf2[MAX_STRING_LENGTH];
  int          slot;
  char       * aString;
  char       * pString;
  BOARD_DATA * pBoard;
  POST_DATA  * pPost;
  POST_DATA  * aPost;

  PUSH_FUNCTION( "do_post" );

  if ( IS_NPC( ch ) || !ch->in_room || !ch->pcdata ) RETURN_NULL();

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    send_to_char( "�A�����������D�άO�^�ĴX�g�峹�T\n\r", ch );
    RETURN_NULL();
  }

  if ( !( pBoard = ch->in_room->board ) )
  {
    send_to_char( "�藍�_�M�o�̨S���d���O�T\n\r", ch );
    RETURN_NULL();
  }

  if ( ( pBoard->lock || ch->level < pBoard->write_level )
    && !IS_IMMORTAL( ch ) )
  {
    send_to_char( "�藍�_�M�o�ӯd���O�A�O����o��峹���T\n\r", ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg, "edit" ) )
  {
    if ( !ch->desc ) RETURN_NULL();

    ch->desc->edit_mode = EDIT_POST;
    show_editor( ch );

    free_string( ch->stack );
    ch->stack = str_dup( "" );

    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "show" ) )
  {
    if ( !ch->editing || !*ch->editing )
    {
      send_to_char( "�A���s��w�İϸ̨S������F��T\n\r", ch );
      RETURN_NULL();
    }

    print_to_char( ch, "�A���峹�����e���R\n\r%s%s"
      , VERTICAL_LINE, ch->editing );

    RETURN_NULL();
  }

  else if ( arg[0] == '!' )
  {
    if ( !ch->editing || !*ch->editing )
    {
      send_to_char( "�A���s��w�İϸ̨S������F��M�Ьd�� edit�T\n\r", ch );
      RETURN_NULL();
    }

    if ( arg[1] == '\x0' )
    {
      send_to_char( "�A���������^�ĴX�g�峹�T\n\r", ch );
      RETURN_NULL();
    }

    if ( !is_number( arg + 1 ) )
    {
      send_to_char( "�藍�_�M�峹�s���@�w�O�Ʀr�T\n\r", ch );
      RETURN_NULL();
    }

    if ( str_len( ch->editing ) > sizeof( buf1 ) - ( MAX_RESPONSE * 2 ) )
    {
      send_to_char( "�A���s��w�İϤӤj�F�M�ЧR���@�ǧa�T\n\r", ch );
      RETURN_NULL();
    }

    slot = atoi( arg + 1 );
    chinese_number( slot, buf2 );

    for ( aPost = pBoard->post; aPost; aPost = aPost->next )
    {
      if ( --slot == 0 )
      {
        /* �t�m�O����åB�M�� */
        set_post_default( ( pPost = alloc_struct( STRUCT_POST_DATA ) ) );

        /* �]�w�@�� */
        str_cpy( buf,  mob_name( NULL, ch ) );
        buf[POSTER_FIELD] = '\x0';
        smash_tilde( buf );
        pPost->poster = str_dup( buf );
        pPost->owner  = str_dup( ch->name );

        /* �]�w���D */
        if ( aPost->attach > 0 ) str_cpy( buf, aPost->title           );
        else                     sprintf( buf, "Re: %s", aPost->title );

        buf[TITLE_FIELD] = '\x0';
        smash_tilde( buf );
        pPost->title = str_dup( buf );

        /* �]�w���e */
        sprintf( buf, "\e[36m:�i �b %s ���j�@������ �j\n  :", aPost->poster );
        aString = buf + str_len( buf ) - 1;

        for ( pString = aPost->text; *pString; pString++ )
        {
          if ( *pString == '\r' ) continue;
          if ( *pString == '\n' )
          {
            *aString = '\x0';
            str_cat( buf, "\n\e[36m  :" );
            aString = buf + str_len( buf );
          }
          else
          {
            *aString++ = *pString;
          }
        }

        *aString = '\x0';
        str_cat( buf, "\e[0m\n\n" );

        if ( str_len( buf ) >= MAX_RESPONSE )
        {
          buf[MAX_RESPONSE] = '\x0';
          str_cat( buf, "\e[0m\n\n" );
        }

        str_cpy( buf1, ch->editing );
        buf1[sizeof(buf1) - str_len( buf ) - 200 ] = '\x0';
        str_cat( buf, buf1 );

        smash_tilde( buf );
        pPost->text = str_dup( buf );

        /* �]�w�ɦW */
        sprintf( buf, "%d", ( pPost->timer = current_time ) );
        pPost->filename = str_dup( buf );

        /* �]�w���� */
        pPost->mark   = FALSE;
        pPost->attach = aPost->timer;
        pPost->timer  = current_time;
        pPost->board  = pBoard;

        /* �g�^�ɮ� */
        attach_post( pPost, pBoard, TRUE );
        write_post( pPost );

        act( "$n�b$t�^�F��$T�g���峹�C", ch, pBoard->name, buf2, TO_ALL );

        /* ����s��w�İ� */
        free_string( ch->editing );
        ch->editing = str_dup( "" );

        /* �s�誺 Stamp �ץ� */
        ch->pcdata->board[pBoard->slot] = current_time;

        RETURN_NULL();
      }
    }

    send_to_char( "�䤣�쨺�g�峹�i�H�^���T\n\r", ch );
    RETURN_NULL();
  }

  else if ( arg[0] == '*' )
  {
    if ( !ch->editing || !*ch->editing )
    {
      send_to_char( "�A���s��w�İϸ̨S������F��M�Ьd�� edit�T\n\r", ch );
      RETURN_NULL();
    }

    if ( arg[1] == '\x0' )
    {
      send_to_char( "�A���������^�ĴX�g�峹�T\n\r", ch );
      RETURN_NULL();
    }

    if ( !is_number( arg + 1 ) )
    {
      send_to_char( "�藍�_�M�峹�s���@�w�O�Ʀr�T\n\r", ch );
      RETURN_NULL();
    }

    if ( str_len( ch->editing ) > sizeof( buf1 ) - 200 )
    {
      send_to_char( "�A���s��w�İϤӤj�F�M�ЧR���@�ǧa�T\n\r", ch );
      RETURN_NULL();
    }

    slot = atoi( arg + 1 );
    chinese_number( slot, buf2 );

    for ( aPost = pBoard->post; aPost; aPost = aPost->next )
    {
      if ( --slot == 0 )
      {
        /* �t�m�O����åB�M�� */
        set_post_default( ( pPost = alloc_struct( STRUCT_POST_DATA ) ) );

        /* �]�w�@�� */
        str_cpy( buf, mob_name( NULL, ch ) );
        buf[POSTER_FIELD] = '\x0';
        smash_tilde( buf );
        pPost->poster = str_dup( buf );
        pPost->owner  = str_dup( ch->name );

        /* �]�w���D */
        if ( aPost->attach > 0 ) str_cpy( buf, aPost->title           );
        else                     sprintf( buf, "Re: %s", aPost->title );

        buf[TITLE_FIELD] = '\x0';
        smash_tilde( buf );
        pPost->title = str_dup( buf );

        /* �]�w���e */
        str_cpy( buf, ch->editing );
        smash_tilde( buf );
        pPost->text = str_dup( buf );

        /* �]�w�ɦW */
        sprintf( buf, "%d", ( pPost->timer = current_time ) );
        pPost->filename = str_dup( buf );

        /* �]�w���� */
        pPost->mark   = FALSE;
        pPost->attach = aPost->timer;
        pPost->timer  = current_time;
        pPost->board  = pBoard;

        /* �g�^�ɮ� */
        attach_post( pPost, pBoard, TRUE );
        write_post( pPost );

        act( "$n�b$t�^�F��$T�g���峹�C", ch, pBoard->name, buf2, TO_ALL );

        /* ����s��w�İ� */
        free_string( ch->editing );
        ch->editing = str_dup( "" );

        /* �s�誺 Stamp �ץ� */
        ch->pcdata->board[pBoard->slot] = current_time;

        RETURN_NULL();
      }
    }

    send_to_char( "�䤣�쨺�g�峹�i�H�^���T\n\r", ch );
    RETURN_NULL();
  }

  else
  {
    if ( !ch->editing || !*ch->editing )
    {
      send_to_char( "�A���s��w�İϸ̨S������F��M�Ьd�� edit�T\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd���e�O�_�������r */
    if ( check_chat_xname( ch, arg ) == TRUE ) RETURN_NULL();

    if ( str_len( ch->editing ) > sizeof( buf ) - 200 )
    {
      send_to_char( " �A���s��w�İϤӤj�F�M�Х��R���@�ǧa�T\n\r", ch );
      RETURN_NULL();
    }

    /* �t�m�O����åB�M�� */
    set_post_default( ( pPost = alloc_struct( STRUCT_POST_DATA ) ) );

    /* �]�w�@�� */
    str_cpy( buf, mob_name( NULL, ch ) );
    buf[POSTER_FIELD] = '\x0';
    smash_tilde( buf );
    pPost->poster = str_dup( buf );
    pPost->owner  = str_dup( ch->name );

    /* �]�w���D */
    str_cpy( buf, arg );
    buf[TITLE_FIELD] = '\x0';
    smash_tilde( buf );
    pPost->title = str_dup( buf );

    /* �]�w���e */
    str_cpy( buf, ch->editing );
    smash_tilde( buf );
    pPost->text = str_dup( buf );

    /* �]�w�ɦW */
    sprintf( buf, "%d", ( pPost->timer = current_time ) );
    pPost->filename = str_dup( buf );

    /* �]�w���� */
    pPost->mark   = FALSE;
    pPost->attach = 0;
    pPost->timer  = current_time;
    pPost->board  = pBoard;

    /* �g�^�ɮ� */
    attach_post( pPost, pBoard, TRUE );
    write_post( pPost );

    act( "$n�b$t�o��F�@�g����$T���峹�C", ch, pBoard->name, arg, TO_ALL );

    /* ����s��w�İ� */
    free_string( ch->editing );
    ch->editing = str_dup( "" );

    /* �s�誺 Stamp �ץ� */
    ch->pcdata->board[pBoard->slot] = current_time;

    RETURN_NULL();
  }

  RETURN_NULL();
}

/* �M���L�q���峹 */
void board_update( void )
{
  BOARD_DATA * pBoard;

  PUSH_FUNCTION( "board_update" );

  for ( pBoard = board_list; pBoard; pBoard = pBoard->next )
    purge_post( pBoard );

  RETURN_NULL();
}

void purge_post( BOARD_DATA * pBoard )
{
  POST_DATA * pPost;
  POST_DATA * zPost;
  int         count;

  PUSH_FUNCTION( "purge_post" );

  if ( !pBoard )
  {
    mudlog( LOG_DEBUG, "purge_post: �ӷ������T." );
    RETURN_NULL();
  }

  /* �p�G�٤���e�q���� */
  if ( ( count = post_count( pBoard, POST_NOT_MARK ) - pBoard->capcity ) <= 0 )
    RETURN_NULL();

  for ( pPost = pBoard->post; pPost && count > 0; pPost = zPost )
  {
    zPost = pPost->next;
    if ( !pPost->mark )
    {
      count--;
      remove_post( pPost );
    }
  }

  /* ���s��s�C�� */
  rewrite_board_list( pBoard );
  RETURN_NULL();
}

void remove_post( POST_DATA * pPost )
{
  BOARD_DATA * pBoard;
  POST_DATA  * aPost;
  POST_DATA  * zPost;
  char         filename[MAX_FILE_LENGTH];

  PUSH_FUNCTION( "remove_post" );

  if ( !pPost || !( pBoard = pPost->board ) )
  {
    mudlog( LOG_DEBUG, "remove_post: �ӷ����~." );
    RETURN_NULL();
  }

  /* �R���ɮ� */
  sprintf( filename, "%s%s", pBoard->pathname, pPost->filename );

  if ( unlink( filename ) != 0 )
    mudlog( LOG_DEBUG, "remove_post: �L�k�R�� %s.", filename );

  for ( zPost = NULL, aPost = pBoard->post;
        aPost;
        zPost = aPost, aPost = aPost->next )
  {
    if ( aPost == pPost )
    {
      if ( !zPost ) pBoard->post  = aPost->next;
      else          zPost->next   = aPost->next;

      free_string( pPost->title    );
      free_string( pPost->text     );
      free_string( pPost->poster   );
      free_string( pPost->owner    );
      free_string( pPost->filename );

      free_struct( pPost , STRUCT_POST_DATA );
      RETURN_NULL();
    }
  }

  mudlog( LOG_DEBUG, "remove_post: ���o�{����R�����峹." );
  RETURN_NULL();
}

/* ���s��s�C�� */
void rewrite_board_list( BOARD_DATA * pBoard )
{
  POST_DATA * pPost;
  FILE      * pFile;

  PUSH_FUNCTION( "rewrite_board_list" );

  if ( !pBoard || !pBoard->listfile || !*pBoard->listfile )
  {
    mudlog( LOG_DEBUG, "rewrite_board_list: �ӷ����~." );
    RETURN_NULL();
  }

  if ( ( pFile = FOPEN( pBoard->listfile, "w" ) ) )
  {
    for ( pPost = pBoard->post; pPost; pPost = pPost->next )
      fprintf( pFile, "File            %s\n", pPost->filename );

    FCLOSE( pFile );
  }

  else
  {
    mudlog( LOG_DEBUG, "rewrite_board_list: �L�k�}���ɮ� %s."
      , pBoard->listfile );
  }

  RETURN_NULL();
}

void attach_post( POST_DATA * pPost, BOARD_DATA * pBoard, bool fWrite )
{
  POST_DATA  * aPost;

  PUSH_FUNCTION( "attach_post" );

  if ( !pPost || !pBoard )
  {
    mudlog( LOG_DEBUG, "attach_post: �ӷ����~." );
    RETURN_NULL();
  }

  pPost->next  = NULL;
  pPost->board = pBoard;

  if ( !pBoard->post )
  {
    pBoard->post = pPost;
  }

  else
  {
    for ( aPost = pBoard->post; aPost->next; aPost = aPost->next );
    aPost->next = pPost;
  }

  /* �p�G�n��s�ɮצC�� */
  if ( fWrite ) rewrite_board_list( pBoard );

  RETURN_NULL();
}

void write_post( POST_DATA * pPost )
{
  BOARD_DATA * pBoard;
  char         filename[MAX_FILE_LENGTH];
  FILE       * pFile;

  PUSH_FUNCTION( "write_post" );

  if ( !pPost || !( pBoard = pPost->board ) || !pBoard->pathname )
  {
    mudlog( LOG_DEBUG, "write_post: �ӷ������T." );
    RETURN_NULL();
  }

  sprintf( filename, "%s%s", pBoard->pathname, pPost->filename );

  if ( !( pFile = FOPEN( filename, "w" ) ) )
  {
    mudlog( LOG_DEBUG, "write_post: �L�k�}���ɮ� %s.", filename );
    RETURN_NULL();
  }

  fprintf( pFile, "%s%s~\n"
    , str_space( "Poster" , BLANK_LENGTH ), pPost->poster );

  fprintf( pFile, "%s%s~\n"
    , str_space( "Owner" , BLANK_LENGTH ), pPost->owner );

  fprintf( pFile, "%s%s~\n"
    , str_space( "Title"  , BLANK_LENGTH ), pPost->title  );

  fprintf( pFile, "%s%d\n"
    , str_space( "Mark"   , BLANK_LENGTH ), pPost->mark   );

  fprintf( pFile, "%s%d\n"
    , str_space( "Attach" , BLANK_LENGTH ), pPost->attach );

  fprintf( pFile, "%s%d\n"
    , str_space( "Timer"  , BLANK_LENGTH ), pPost->timer  );

  fprintf( pFile, "Text\n%s~\n", pPost->text );
  fprintf( pFile, "End" );
  FCLOSE( pFile );

  RETURN_NULL();
}

/* ��ܤ峹��Y�H�����A */
const char * post_status( POST_DATA * pPost, int timer )
{
  PUSH_FUNCTION( "post_status" );

  if ( pPost->timer > timer && pPost->mark ) RETURN( "\e[1;35mM\e[0m" );
  if ( pPost->mark ) RETURN( "\e[1;33mm\e[0m" );

  if ( pPost->timer > timer ) RETURN( "\e[1;32mN\e[0m" );

  RETURN( " " );
}

FUNCTION( do_edit )
{
  char   arg[MAX_INPUT_LENGTH];
  char   buf[MAX_STRING_LENGTH];
  char   buf1[MAX_STRING_LENGTH];
  char * pString;

  PUSH_FUNCTION( "do_edit" );

  if ( IS_NPC( ch ) || !ch->editing ) RETURN_NULL();

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    if ( !*ch->editing )
    {
      send_to_char( "�A�èS���s�����F��T\n\r", ch );
      RETURN_NULL();
    }

    print_to_char( ch, "�A�ثe�s�誺���e���R\n\r%s%s%s"
      , VERTICAL_LINE, ch->editing, VERTICAL_LINE );

    RETURN_NULL();
  }

  else if ( !str_cmp( arg, "!clear" ) )
  {
    free_string( ch->editing );
    ch->editing = str_dup( "" );
    send_to_char( "�A�w�g�M���A���s��w�İϪ����e�C\n\r", ch );
    RETURN_NULL();
  }

  else if ( !str_cmp( arg, "+" ) )
  {
    str_cpy( buf, ch->editing );

    if ( str_len( buf ) + str_len( argument ) >= ( sizeof( buf ) - 200 ) )
    {
      send_to_char( "�藍�_�M�s�褺�e�Ӫ��F�C\n\r", ch );
      RETURN_NULL();
    }

    /* ��������r */
    smash_tilde( argument );

    /* �ˬd���e�O�_�������r */
    if ( check_chat_xname( ch, argument ) == TRUE ) RETURN_NULL();

    ansi_transcribe( argument, buf1 );

    /* �H�קK�ťճQ�R�� */
    if ( buf1[0] == '.' ) buf1[0] = ' ';

    str_cat( buf, buf1 );
    str_cat( buf, "\n" );
    free_string( ch->editing );
    ch->editing = str_dup( buf );

    send_to_char( "�W�[�s�誺���e�C\n\r", ch );
    RETURN_NULL();
  }

  else if ( !str_cmp( arg, "-" ) )
  {
    if ( !*ch->editing )
    {
      send_to_char( "�A���s��w�İϨS�����e�M�L�k�R���T\n\r", ch );
      RETURN_NULL();
    }

    str_cpy( buf, ch->editing );

    for ( pString = buf + str_len( buf ) - 2; pString > buf; pString-- )
    {
      if ( *pString == '\n' )
      {
        *( pString + 1 ) = '\x0';
        free_string( ch->editing );
        ch->editing = str_dup( buf );

        send_to_char( "�R���s��w�İϤW�@��T\n\r", ch );
        RETURN_NULL();
      }
    }

    free_string( ch->editing );
    ch->editing = str_dup( "" );
    send_to_char( "�]���A���w�İϥu���@��M�ҥH��ӳQ�R���F�T\n\r", ch );
    RETURN_NULL();
  }

  else
  {
    send_to_char( "�y�k���~�M�Ьd�߬����Ϊk�T\n\r", ch );
    RETURN_NULL();
  }
  RETURN_NULL();
}

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
#include <sys/time.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include "merc.h"
#include "directory.h"

bool        SystemCrash     = FALSE;
static int  overscale       = 0;
int         FunctionPointer = MAX_DEBUG -1;
char      * FunctionName[MAX_DEBUG];

bool        signal_segv     = TRUE;
bool        signal_bus      = TRUE;
bool        signal_fpe      = TRUE;
bool        signal_ill      = TRUE;

static void sig_segv    args( ( int ) );
static void sig_bus     args( ( int ) );
static void sig_fpe     args( ( int ) );
static void sig_ill     args( ( int ) );

static void sig_hup     args( ( int ) );
static void sig_int     args( ( int ) );
static void sig_term    args( ( int ) );
static void sig_usr1    args( ( int ) );
static void sig_usr2    args( ( int ) );
static void sig_quit    args( ( int ) );
void   checkloop        args( ( int ) );
void   checkprof        args( ( int ) );

#define         MAX_PROF        200
#define         MAX_PROF_COUNT  500

char     ProfFunction    [26][MAX_PROF][50];
int      ProfCount       [26][MAX_PROF];

void signal_setup( void )
{
  struct itimerval itime;
  struct timeval   interval;

  if ( signal_segv ) signal( SIGSEGV, sig_segv );
  if ( signal_fpe  ) signal( SIGFPE , sig_fpe  );
  if ( signal_bus  ) signal( SIGBUS , sig_bus  );
  if ( signal_ill  ) signal( SIGILL , sig_ill  );

  signal( SIGHUP  , sig_hup  );
  signal( SIGINT  , sig_int  );
  signal( SIGTERM , sig_term );

  signal( SIGUSR1 , sig_usr1 );
  signal( SIGUSR2 , sig_usr2 );
  signal( SIGQUIT , sig_quit );
  signal( SIGPIPE , SIG_IGN  );
  signal( SIGCHLD , SIG_IGN  );

  interval.tv_sec  = MaxGameIdle;
  interval.tv_usec = 0;

  itime.it_interval = itime.it_value = interval;

  signal( SIGVTALRM , checkloop );
  setitimer( ITIMER_VIRTUAL , &itime , ( struct itimerval * ) NULL );

  if ( ProfCycle > 0 )
  {
    interval.tv_sec  = 0;
    interval.tv_usec = 1000000 / ProfCycle;

    itime.it_interval = itime.it_value = interval;

    signal( SIGPROF , checkprof );
    setitimer( ITIMER_PROF , &itime , ( struct itimerval * ) NULL );
  }

  return;
}

void sig_hup( int sig )
{
  mudlog( LOG_EMERG , "����׺ݾ����h�s�u, �ɭP��{���_" );
  return;
}

void sig_int( int sig )
{
  mudlog( LOG_EMERG , "�׺ݾ�����̫��U���_��( Crtl-C )�ɭP�{���פ�" );
  return;
}

void sig_term( int sig )
{
  mudlog( LOG_EMERG , "�D���i�������{��, �ɭP��{���_" );
  return;
}

void sig_ill( int sig )
{
  mudlog( LOG_EMERG , "����D�k�w����O��" );
  return;
}

void sig_fpe( int sig )
{
  mudlog( LOG_EMERG , "�������s�άO�B�I�Ʒ���" );
  return;
}

void sig_segv( int sig )
{
  mudlog( LOG_EMERG , "�s�����X�k����}, �{�������`�פ�" );
  return;
}

void sig_bus( int sig )
{
  mudlog( LOG_EMERG , "�s�����X�k����}, �{���ѦҤ�����ɦ�}" );
  return;
}

void sig_usr1( int sig )
{
  mudlog( LOG_EMERG, "�ϥΪ̤��_(1)" );
  abort();
  return;
}

void sig_usr2( int sig )
{
  mudlog( LOG_INFO, "�ϥΪ̤��_(2)" );
  do_reboot( NULL , "" );
  return;
}

void sig_quit( int sig )
{
  mudlog( LOG_EMERG, "�׺ݾ��ϥΪ̫��U���}��( Ctrl-\\ )�ɭP�{���פ�" );
  return;
}

void checkprof( int sig )
{
  static bool reenter = FALSE;
  int         loop;
  int         letter;
  char      * pString;

  signal( SIGPROF , checkprof );

  if ( reenter ) return;

  reenter = TRUE;
  pString = FunctionName[FunctionPointer+1];

  if ( ( letter = tolower( *pString ) - 'a' ) < 0 || letter >= 26 ) return;

  for ( loop = 0; loop < MAX_PROF; loop++ )
  {
    if ( ProfFunction[letter][loop][0] == '\x0' )
    {
      strcpy( ProfFunction[letter][loop], pString );;
      ProfCount[letter][loop] = 1;
      reenter = FALSE;
      return;
    }

    else if ( !strcmp( pString, ProfFunction[letter][loop] ) )
    {
      ProfCount[letter][loop]++;
      reenter = FALSE;
      return;
    }
  }

  reenter = FALSE;
  return;
}

void checkloop( int sig )
{
  extern int traceloop;

  signal( SIGVTALRM , checkloop );
  if ( !traceloop ) mudlog( LOG_EMERG , "�{�����J�L�a�^��, �{���۰�����." );
  traceloop = 0;
  return;
}

/* ���ƦW�r���J���| */
void push_function( char * str )
{
  if ( internal_debug && !SystemCrash )
  {
    if ( FunctionPointer >= 0 ) FunctionName[FunctionPointer--] = str;
    else overscale++;
  }

  return;
}

void pop_function( void )
{
  if ( internal_debug && !SystemCrash )
  {
    if ( overscale <= 0 )
    {
      if ( ++FunctionPointer > MAX_DEBUG - 1 )
      {
        SystemCrash = TRUE;
        mudlog( LOG_STACK , "�������|����(POP), �{���۰�����." );
      }
    }
    else
    {
      overscale--;
    }
  }
  return;
}

void close_all_socket( void )
{
  CHAR_DATA * ch;
  NET_DATA  * pNet;

  PUSH_FUNCTION( "close_all_socket" );

  /* �����Ҧ����ڳs�u */
  mudlog( LOG_INFO, "���������Ҧ����ڳs�u" );

  for ( pNet = net_list; pNet; pNet = pNet->next )
  {
    if ( valid_socket( pNet, NET_IMPORT ) )
      close_net_socket( pNet, NET_IMPORT );

    if ( valid_socket( pNet, NET_EXPORT ) )
      close_net_socket( pNet, NET_EXPORT );
  }

  /* ��������� */
  if ( auction_info && auction_info->seller )
  {
    if ( auction_info->buyer )
      gold_to_char( auction_info->buyer, auction_info->price );

    if ( !IS_NPC( auction_info->seller ) )
    {
      auction_info->obj->timer     = auction_info->timer;
      obj_to_char( auction_info->obj , auction_info->seller );
    }

    else
    {
      extract_obj( auction_info->obj );
    }

    init_auction();
    mudlog( LOG_INFO , "���`��������Ƶ��c." );
  }
  else
  {
    mudlog( LOG_INFO , "�S�������, ���ݭn����" );
  }

  /* ��}��T�ɮצs�� */
  mudlog( LOG_INFO , "�x�s�Ҧ���}���" );
  do_address( NULL , "!save" );

  /* �Ѳ��ɮצs�� */
  mudlog( LOG_INFO, "�x�s�Ѳ�����ɮ�" );
  save_stock();

  mudlog( LOG_INFO , "�����x�s�Ҧ������a���." );

  for ( ch = char_list; ch; ch = ch->next )
  {
    if ( !verify_char( ch ) || IS_NPC( ch ) ) continue;

    mudlog( LOG_INFO , "���զ^�s���a %s �����" , ch->name );
    save_char_obj( ch, SAVE_FILE );
    extract_char( ch, TRUE );
    if ( ch->desc ) close_socket( ch->desc );
  }

  mudlog( LOG_INFO , "�t�Φ��\\�^�s���a���." );

  /* �x�s���|��� */
  mudlog( LOG_INFO, "�����x�s���|���." );
  save_club();

  /* ��z���� */
  mudlog( LOG_INFO, "���էR���L�q���峹." );
  board_update();

  /* �M���L�q���H�� */
  notes_update();

  /* ���U�@�ɰO���� */
  delete_share_memory( shmid );

  /* �M���O���w�İ� */
  output_buffer();

  RETURN_NULL();
}

/* ����H���O�_�u���s�b */
bool verify_char( CHAR_DATA * ch )
{
  if ( !ch
    || ch->delete
    || ( ch->desc && verify_desc( ch->desc ) &&
      ch->desc->connected != CON_PLAYING ) )
    return( FALSE );

  return( TRUE );
}

/* ���窫�~�O�_�u���s�b */
bool verify_obj( OBJ_DATA * obj )
{
  PUSH_FUNCTION( "verify_obj" );

  if ( !obj || obj->delete ) RETURN( FALSE );
  RETURN( TRUE );
}

/* ����y�z��O�_�u���s�b */
bool verify_desc( DESCRIPTOR_DATA * desc )
{
  PUSH_FUNCTION( "verify_desc" );

  if ( !desc || desc->delete ) RETURN( FALSE );
  RETURN( TRUE );
}

FUNCTION( do_prof )
{
  char * name[MAX_PROF_COUNT];
  int    times[MAX_PROF_COUNT];
  char * Name;
  int    Times;
  int    total;
  int    letter;
  int    loop;
  int    count;
  int    i;
  int    j;

  PUSH_FUNCTION( "do_prof" );

  if ( ProfCycle <= 0 )
  {
    act( "$t�ثe�S���ϥζq�έp�\\��C", ch, mud_name, NULL, TO_CHAR );
    RETURN_NULL();
  }

  for ( count = total = letter = 0; letter < 26; letter++ )
  {
    if ( count >= MAX_PROF_COUNT ) break;

    for ( loop = 0; loop < MAX_PROF; loop++ )
    {
      if ( ProfFunction[letter][loop][0] == '\x0' || count >= MAX_PROF_COUNT )
        break;

      total += ProfCount[letter][loop];
      name[count]  = ProfFunction[letter][loop];
      times[count] = ProfCount[letter][loop];
      count++;
    }
  }


  for ( i = 0; i < count - 1; i++ )
  {
    for ( j = i+1; j < count; j++ )
    {
      if ( times[i] < times[j] )
      {
        Times    = times[i];
        times[i] = times[j];
        times[j] = Times;

        Name     = name[i];
        name[i]  = name[j];
        name[j]  = Name;
      }
    }
  }

  clear_buffer();
  for ( i = 0; i < count; i++ )
  {
    send_to_buffer( "%-4d %-20s %4d%%%% (%5d/%d)\n\r"
      , i, name[i]
      , times[i] * 1000 / total
      , times[i], total );
  }

  print_buffer( ch );
  RETURN_NULL();
}

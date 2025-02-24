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
#include <sys/time.h>
#include <sys/wait.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/telnet.h>
#include <arpa/inet.h>
#include "merc.h"

/* Signal �B�z. Apollo �b signal.h �� __attribute(atomic) �����D */
/* �b�o�Ӧa�褣�n�B�z */

#if defined(apollo)
#define __attribute(x)
#endif

#if defined(apollo)
#undef __attribute
#endif

#if defined(unix)
#include <signal.h>
#endif

const   char    echo_off_str    [] = { IAC, WILL, TELOPT_ECHO, '\x0' };
const   char    echo_on_str     [] = { IAC, WONT, TELOPT_ECHO, '\x0' };
const   char    go_ahead_str    [] = { IAC, GA, '\x0' };

/* �M�@�~�t�Φ������ŧi */
/* �o�̨S���ϥΨ� */
#if     defined(_AIX)
#include <sys/select.h>
int     accept          args( ( int, struct sockaddr *, int * ) );
int     bind            args( ( int, struct sockaddr *, int ) );
int     getpeername     args( ( int , struct sockaddr *, int * ) );
int     getsockname     args( ( int , struct sockaddr *, int * ) );
int     gettimeofday    args( ( struct timeval *, struct timezone * ) );
int     listen          args( ( int, int ) );
int     setsockopt      args( ( int, int, int, void *, int ) );
int     socket          args( ( int, int, int ) );
#endif

/* �M�@�~�t�Φ������ŧi, �o�̨S���ϥΨ� */

#if     defined(apollo)
#include <unistd.h>
#endif

/* �M�@�~�t�Φ������ŧi */
/* �o�̨S���ϥΨ� */

#if     defined(__hpux)
int     accept          args( ( int, void *, int * ) );
int     bind            args( ( int, const void *, int ) );
int     getpeername     args( ( int, void *, int * ) );
int     getsockname     args( ( int, void *, int * ) );
int     gettimeofday    args( ( struct timeval *, struct timezone * ) );
int     listen          args( ( int, int ) );
int     setsockopt      args( ( int, int, int, const void *, int ) );
int     socket          args( ( int, int, int ) );
#endif

/* �M�@�~�t�Φ������ŧi */
/* �ڭ̨S�ϥΨ�o�� */

#if     defined(interactive)
#include <net/errno.h>
#include <sys/fcntl.h>
#endif

/* �M�@�~�t�Φ������ŧi */
#if     defined(__linux__)
/*
int     accept          args( ( int, struct sockaddr *, int * ) );
int     bind            args( ( int, struct sockaddr *, int ) );
int     close           args( ( int ) );
int     getpeername     args( ( int, struct sockaddr *, int * ) );
int     getsockname     args( ( int, struct sockaddr *, int * ) );
int     gettimeofday    args( ( struct timeval *, struct timezone * ) );
int     listen          args( ( int, int ) );
int     read            args( ( int, char *, int ) );
int     select          args( ( int, fd_set *, fd_set *, fd_set *, struct timeval * ) );
int     socket          args( ( int, int, int ) );
int     write           args( ( int, char *, int ) );
*/
#endif

/* �o�̨S���ϥΨ� */
#if     defined(macintosh)
#include <console.h>
#include <fcntl.h>
#include <unix.h>

struct  timeval
{
  time_t  tv_sec;
  time_t  tv_usec;
};

#if     !defined(isascii)
#define isascii(c)              ( (c) < 0245 )
#endif
static  long                    theKeys [4];
int     gettimeofday            args( ( struct timeval *, void * ) );
#endif

/* �o�̨S���ϥΨ� */
#if     defined(MIPS_OS)
extern  int             errno;
#endif

/* �o�̨S���ϥΨ� */
#if     defined(MSDOS)
int     gettimeofday    args( ( struct timeval *, void * ) );
int     kbhit           args( ( void ) );
#endif

/* �o�̨S���ϥΨ� */
#if     defined(NeXT)
int     close           args( ( int ) );
int     fcntl           args( ( int, int, int ) );

#if     !defined(htons)
u_short htons           args( ( u_short ) );
#endif

#if     !defined(ntohl)
u_long  ntohl           args( ( u_long ) );
#endif

int     read            args( ( int, char *, int ) );
int     select          args( ( int, fd_set *, fd_set *, fd_set *, struct timeval * ) );
int     write           args( ( int, char *, int ) );
#endif

/* �o�̨S���ϥΨ� */
#if     defined(sequent)
int     accept          args( ( int, struct sockaddr *, int * ) );
int     bind            args( ( int, struct sockaddr *, int ) );
int     close           args( ( int ) );
int     fcntl           args( ( int, int, int ) );
int     getpeername     args( ( int, struct sockaddr *, int * ) );
int     getsockname     args( ( int, struct sockaddr *, int * ) );
int     gettimeofday    args( ( struct timeval *, struct timezone * ) );

#if     !defined(htons)
u_short htons           args( ( u_short ) );
#endif

int     listen          args( ( int, int ) );

#if     !defined(ntohl)
u_long  ntohl           args( ( u_long ) );
#endif

int     read            args( ( int, char *, int ) );
int     select          args( ( int, fd_set *, fd_set *, fd_set *, struct timeval * ) );
int     setsockopt      args( ( int, int, int, caddr_t, int ) );
int     socket          args( ( int, int, int ) );
int     write           args( ( int, char *, int ) );
#endif

/* �o�̨S���ϥΨ� */
#if defined(sun)
int     close           args( ( int ) );
int     listen          args( ( int, int ) );
int     select          args( ( int, fd_set *, fd_set *, fd_set *, struct timeval * ) );
int     socket          args( ( int, int, int ) );
int     sigmask         args( ( int ) );
int     sigsetmask      args( ( int ) );
void    (*signal        args( (int signum, void (*handler)(int))))(int);
#endif

/* �o�̨S���ϥΨ� */
#if defined(ultrix)
int     accept          args( ( int, struct sockaddr *, int * ) );
int     bind            args( ( int, struct sockaddr *, int ) );
int     close           args( ( int ) );
int     getpeername     args( ( int, struct sockaddr *, int * ) );
int     getsockname     args( ( int, struct sockaddr *, int * ) );
int     gettimeofday    args( ( struct timeval *, struct timezone * ) );
int     listen          args( ( int, int ) );
int     read            args( ( int, char *, int ) );
int     select          args( ( int, fd_set *, fd_set *, fd_set *, struct timeval * ) );
int     setsockopt      args( ( int, int, int, void *, int ) );
int     socket          args( ( int, int, int ) );
int     write           args( ( int, char *, int ) );
#endif

/* �����ܼ� */
DESCRIPTOR_DATA *   descriptor_list;         /* All open descriptors   */
bool                merc_down;               /* �t�� shutdown          */
bool                merc_exec       = FALSE; /* �O�_ merc �w�g����F   */
bool                wizlock         = FALSE; /* �C���O�_�Q��w         */
bool                NewLock         = FALSE;
bool                multilogin      = FALSE; /* �O�_����h���s�u       */
bool                strict_password = FALSE; /* ���Y�檺�K�X           */
bool                strict_email    = FALSE; /* ���Y�檺 email- box    */
bool                tick_permit     = TRUE;  /* �O�_�i�H�ϥέp�ƾ�     */
bool                ChatRecord      = TRUE;  /* �O�_�O���͸ܾ��v       */
int                 MudPort[MAX_PORT];       /* �w�q�̦h���X�� port    */
int                 MudConnect[MAX_PORT];    /* �C�ӳs���𪺳s�u�H��   */
int                 traceloop       = 0;     /* �O�_���J�L�a�^��ʱ��� */
int                 max_stock_buy   = 0;     /* �C�تѲ��̤j������     */
int                 max_hire        = 3;
int                 max_board       = MAX_BOARD; /* �t�γ̦h�i�H���X�Ӫ��� */
int                 max_notes       = MAX_NOTES; /* �t�γ̦h���X�ʫH       */
int                 Rechristen = MAX_RECHRISTEN; /* �C�ӤH�̦h�i�H��W�X�� */
bool                first_warn      = TRUE;  /* �Ĥ@�����s�}��ĵ�i     */
bool                second_warn     = TRUE;  /* �ĤG�����s�}��ĵ�i     */
bool                third_warn      = TRUE;  /* �ĤT�����s�}��ĵ�i     */
bool                AngelComing     = TRUE;  /* �O�_�u�@���i�H���     */
bool                AutoBackup      = TRUE;  /* �O�_�۰ʨ��ƥ�         */
bool                CheckServer     = TRUE;  /* �O�_�ˬd�O�_���u�@��   */
bool                GetFQDN         = TRUE;  /* �O�_���o FQDN          */
bool                RecordFile      = FALSE; /* �O�_�O���}�ɰO��       */
int                 AngelTimes      = 1;     /* �u�@���C��u�Ӥ@��     */
int                 AngelDefault    = 60;    /* ���a�u�@���g���w�]��   */
int                 MaxConnect      = 0;     /* �̦h�W�u�H��           */
int                 max_ip          = MAX_IP; /* �̦h����s�u��}      */
int                 Increase        = 5;
int                 IncreaseLimit   = 300;
int                 DepositMoney    = 1;
int                 EndowRate       = 1000;
int                 DonateMoney     = -1;
int                 DonateLevel     = -1;
int                 DonateBenifit   = -1;
int                 DonateLimit     = -1;
int                 DonateDuration  = -1;
int                 DonateCount     = -1;
bool                DonateLock      = FALSE;
int                 MaxNumber       = ( ( unsigned ) -1 ) / 2;
int                 FightRound      = 200;
int                 FightDifferent  = 20;
int                 FightRate       = 2;
int                 FoggyRoom       = 30;
AREA_DATA         * FoggyArea       = NULL;

int                 FreeChar        = 0;
int                 FreeObject      = 0;
int                 FreeDesc        = 0;
int                 BankThreshold   = MAX_ASSET / 4;
int                 BankProfit      = BANK_PROFIT;
int                 SacGold         = 3;

time_t              current_time;            /* �ثe���ɶ�             */
time_t              mud_boot_time;           /* �t�έ��m�ɶ�           */
time_t              reboot_time   = ( time_t ) 0;
time_t              shutdown_time = ( time_t ) 0;
long                mud_boot_tick;
char                HostName[ MAX_HOSTNAME + 1 ];
char                CurrentDir[MAX_FILE_LENGTH];

int                 WriteKiloBytes  = 0;
int                 WriteBytes      = 0;
int                 ReadKiloBytes   = 0;
int                 ReadBytes       = 0;

int                 SaleChance      = ERRORCODE;
int                 SaleLimit       = 10;
int                 PillsPerDay     = PILLS_PER_DAY;
int                 DosageUpdate    = DOSAGE_UPDATE;
int                 DosageLimit     = DOSAGE_LIMIT;
int                 VoteGold        = VOTE_GOLD;
int                 load_hit_plus   = LOAD_HIT_PLUS;
int                 load_mana_plus  = LOAD_MANA_PLUS;
int                 load_move_plus  = LOAD_MOVE_PLUS;
int                 MaxPractice     = MAX_PRACTICE;
int                 MaxNSkill       = MAX_NSKILL;
int                 MaxFirman       = MAX_FIRMAN;
int                 HeroFirman      = HERO_ADD_FIRMAN;
int                 FoodDegrade     = FOOD_DEGRADE;
int                 BornAge         = BORN_AGE;
int                 DefaultGold     = DEFAULT_GOLD;
int                 DefaultStr      = DEFAULT_STR;
int                 DefaultInt      = DEFAULT_INT;
int                 DefaultWis      = DEFAULT_WIS;
int                 DefaultDex      = DEFAULT_DEX;
int                 DefaultCon      = DEFAULT_CON;
int                 DefaultHit      = DEFAULT_HIT;
int                 DefaultMana     = DEFAULT_MANA;
int                 DefaultMove     = DEFAULT_MOVE;
int                 DefaultPractice = 21;
int                 MaxStr          = MAX_STR;
int                 MaxInt          = MAX_INT;
int                 MaxWis          = MAX_WIS;
int                 MaxDex          = MAX_DEX;
int                 MaxCon          = MAX_CON;
int                 MaxHit          = MAX_HIT;
int                 MaxMana         = MAX_MANA;
int                 MaxMove         = MAX_MOVE;

int                 DefaultObject[ MAX_DEFAULT_OBJECT];

#if defined(unix)
void    game_loop               args( ( void ) );
void    new_descriptor          args( ( int, int ) );
bool    read_from_descriptor    args( ( DESCRIPTOR_DATA * ) );
#endif

/* ��L�M�@�~�t�εL�����ϰ��ܼ� */
int     init_socket             args( ( int ) );
bool    check_reconnect         args( ( DESCRIPTOR_DATA *, bool ) );
bool    check_logon             args( ( char * ) );
bool    check_playing           args( ( DESCRIPTOR_DATA *, char * ) );
int     main                    args( ( int, char ** ) );
void    nanny                   args( ( DESCRIPTOR_DATA *, char * ) );
bool    process_output          args( ( DESCRIPTOR_DATA *, bool ) );
void    read_from_buffer        args( ( DESCRIPTOR_DATA * ) );
void    stop_idling             args( ( CHAR_DATA * ) );
void    bust_a_prompt           args( ( CHAR_DATA * ) );
void    bust_fight_message      args( ( CHAR_DATA *, CHAR_DATA * ) );
void    spilt_semicolon         args( ( DESCRIPTOR_DATA * ) );
void    translate_command       args( ( DESCRIPTOR_DATA * ) );
void    show_game_status        args( ( DESCRIPTOR_DATA * ) );
void    recover_file            args( ( DESCRIPTOR_DATA * ) );
void    struct_updates          args( ( void ) );
void    lock_file               args( ( DESCRIPTOR_DATA * ) );
bool    check_multi_login       args( ( DESCRIPTOR_DATA * ) );
void    set_innate              args( ( DESCRIPTOR_DATA * ) );
void    show_innate             args( ( DESCRIPTOR_DATA * ) );
void    show_home               args( ( DESCRIPTOR_DATA * ) );
int     get_innate              args( ( DESCRIPTOR_DATA * ) );
bool    check_email_address     args( ( char * ) );
void    save_to_stack           args( ( DESCRIPTOR_DATA *, const char *, int ) );
void    close_communication     args( ( void ) );

static char inflect[] =
{
  '|' , '\\' , '-' , '/'
};

char * const he_she  [] = { "��",   "�L",   "�o" };
char * const his_her [] = { "����", "�L��", "�o��" };

int    Control[MAX_PORT];
int    Internal;

int main( int argc, char ** argv )
{
  extern bool    signal_segv;
  extern bool    signal_fpe;
  extern bool    signal_bus;
  extern bool    signal_ill;
  extern int     WriteBytes;
  extern int     WriteKiloBytes;
  extern int     ReadBytes;
  extern int     ReadKiloBytes;
  struct timeval now_time;
  int            pos;
  int            maxdesc;
  int            loop;
  char           filename[ MAX_FILE_LENGTH ];

  PUSH_FUNCTION( "main" );

  for ( pos = 1, filename[0] = '\x0'; pos < argc ; pos++ )
  {
    if ( argv[pos][0] != '-' )
    {
      if ( str_len( argv[pos] ) > sizeof( filename ) )
        mudlog( LOG_ERR, "main: �_�l���ɮצW�ٹL��." );

      str_cpy( filename, argv[pos] );
      break;
    }
  }

  if ( !filename[0] )
  {
    char * enviroment;

    if ( ( enviroment = getenv( "merc" ) ) )
    {
      if ( str_len( enviroment ) >= sizeof( filename ) -1 )
        mudlog( LOG_ERR, "main: �_�l���ɮצW�ٹL��." );

      str_cpy( filename,  enviroment );
    }

    else
    {
      if ( str_len( INI_FILE ) >= sizeof( filename ) - 1 )
        mudlog( LOG_ERR, "main: �_�l���ɮצW�ٹL��." );

      str_cpy( filename, INI_FILE );
    }
  }

  /* �M���s�u���s���� */
  for ( loop = 0; loop < MAX_PORT; loop++ )
    Control[loop] = MudPort[loop] = ERRORCODE;

  /* �M���w�w�����a���~ */
  for ( loop = 0; loop < MAX_DEFAULT_OBJECT; loop++ )
    DefaultObject[loop] = -1;

  /* ���o�{������{���X */
  mudlog( LOG_INFO, "��{���X�R%d", merc_pid = getpid() );

  /* �_�l�ưO���� */
  memory_ini();

  /* Ū���ҩl�ɮ� */
  read_ini( filename );

  /* ���|���ɮ׹w�]�� */
  default_file();

  /* �ҩl���ɮ׸��| */
  adjust_filename();

  mudlog( LOG_INFO, "%s�s���D�k��}�H��", signal_segv ? "�}��" : "����" );
  mudlog( LOG_INFO, "%s�B�I�B����~�H��", signal_fpe  ? "�}��" : "����" );
  mudlog( LOG_INFO, "%s�ѦҤ����}�H��", signal_bus  ? "�}��" : "����" );
  mudlog( LOG_INFO, "%s�D�k���O���H��"  , signal_ill  ? "�}��" : "����" );

  /* �ҩl�ưT�� */
  signal_setup();

  /* ���o�ثe�ؿ� */
  getcwd( CurrentDir, sizeof( CurrentDir ) - 1 );

  /* �ҩl�Ʈɶ� */
  gettimeofday( &now_time, NULL );
  mud_boot_time = current_time  = ( time_t ) now_time.tv_sec;
  mud_boot_tick = get_uptime();

  if ( reboot_time > 0 ) reboot_time += current_time;

  /* �O�d�@���ɮץN�X�H�Ѩϥ� */
  if ( !( fpReserve = fopen( null_file , "r" ) ) )
  {
    mudlog( LOG_INFO, strerror( errno ) );
    mudlog( LOG_ERR , "�L�k�O�d�ܤ֤@���ɮץN�X" );
  }

  /* �����o�D�����W��, �� /etc/hosts */
  if ( gethostname( HostName, MAX_HOSTNAME ) != 0 )
   mudlog( LOG_ERR, "�L�k���o�D���W��." );

  mudlog( LOG_INFO, "�D���W�� %s", HostName );

  /* �_�Ϥ� socket */
  for ( loop = 0; loop < MAX_PORT; loop++ )
  {
    if ( MudPort[loop] != ERRORCODE )
    {
      mudlog( LOG_INFO , "�D��{�u�q�H��R%4d�v�޹D %d"
      , MudPort[loop], Control[loop] = init_socket( MudPort[loop] ) );
    }
    else
    {
      Control[loop] = ERRORCODE;
    }
  }

  /* �_�Ϥƺ��ڳs�u */
  mudlog( LOG_NET , "���ں޹D %d", Internal = init_socket( internal_port ) );

  /* ����̦h�W�u�H�� */
  if ( ( maxdesc = getdtablesize() ) <= 0 )
  {
    mudlog( LOG_INFO, strerror( errno ) );
    mudlog( LOG_ERR , "�L�k���o�y�z��̤j�Ӽ�" );
  }

  max_connect = UMIN( maxdesc - 8 , max_connect );
  mudlog( LOG_INFO , "�̦h�W�u�H�ƤW�u %d" , max_connect );

  /* �Ұ� merc �D�{��, �ø��J�ϰ��� */
  boot_db();

  /* �ҨϤƦ@�ɰO���� */
  init_share_memory( merc_ipc, merc_ipc_2 );

  mudlog( LOG_INFO , "%s�}�l���`�B�@.", mud_name );

  /* �u������ merc �D�{�� */
  merc_exec = TRUE;
  game_loop();
  merc_exec = FALSE;

  /* ���� mud �u�@ */
  for ( loop = 0; loop < MAX_PORT; loop++ )
  {
    if ( Control[loop] != ERRORCODE )
    {
      #ifdef __linux__
      shutdown( Control[loop], 2 );
      #endif

      close( Control[loop] );
      mudlog( LOG_INFO, "�����s���� %5d �s�u�C", MudPort[loop] );
    }
  }

  #ifdef __linux__
  shutdown( Internal, 2 );
  #endif

  close( Internal );
  mudlog( LOG_INFO, "�������ڳs�u %5d�C", internal_port );

  mudlog( LOG_INFO, "�`��X�줸�� %d,%d �줸��.", WriteKiloBytes, WriteBytes );
  mudlog( LOG_INFO, "�`��J�줸�� %d,%d �줸��.", ReadKiloBytes, ReadBytes );

  mudlog( LOG_INFO , "%s���`����. ���¨ϥ�." , mud_name );

  mudlog( LOG_INFO, "�M���L�������a�ɮ�." );
  system_cleanup();
  mudlog( LOG_INFO, "�M���L�����a�ɮק���." );

  return 0;
}

/* �_�Ϥ� socket */
int init_socket( int port )
{
  struct sockaddr_in sa;

#ifdef __linux__
  struct hostent   * hp;
  int                wildcard;
#endif

  int                optval = 1;
  int                fd;

  PUSH_FUNCTION( "init_socket" );

  /* �إߤ@�� socket */
  if ( ( fd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
  {
    mudlog( LOG_INFO, strerror( errno )  );
    mudlog( LOG_ERR , "�L�k�Ұ� socket (%d)", port );
  }

  /* �]�w socket �����A */
  if ( setsockopt( fd, SOL_SOCKET, SO_REUSEADDR
    ,( char * ) &optval, sizeof( optval ) ) < 0 )
  {
    mudlog( LOG_INFO, strerror( errno ) );
    mudlog( LOG_ERR , "�L�k�]�w socket (�q�T��: %d)", port );
  }

#ifdef __linux__

  /* ���Ʈw���W�٬� hostname ����Ƶ�Ū���i�� */
  if ( !( hp = gethostbyname( HostName ) ) )
  {
    mudlog( LOG_INFO, strerror( errno ) );
    mudlog( LOG_ERR , "�L�k�Ұ� %d gethostbyname", port );
  }

  memset( &sa , 0 , sizeof( struct sockaddr_in ) );
  sa.sin_family = hp->h_addrtype;
  sa.sin_port   = htons( ( u_short ) port );
  wildcard = INADDR_ANY;
  memcpy( &sa.sin_addr, &wildcard, sizeof( long ) );

#else

  memset( &sa, 0, sizeof( struct sockaddr_in ) );
  sa.sin_family = AF_INET;
  sa.sin_port   = htons( ( u_short ) port );
#endif

  if ( bind( fd, ( struct sockaddr * ) &sa, sizeof( sa ) ) < 0 )
  {
    #ifdef __linux__
    shutdown( fd, 2 );
    #endif

    close( fd );
    mudlog( LOG_INFO , strerror( errno ) );
    mudlog( LOG_ERR, "�L�k�Ұ� bind (%d)" , port );
  }

  if ( listen( fd, MAX_CONNECT ) < 0 )
  {
    #ifdef __linux__
    shutdown( fd, 2 );
    #endif

    close( fd );
    mudlog( LOG_INFO, strerror( errno ) );
    mudlog( LOG_ERR , "�L�k�Ұʵ��ݳs�u %d." , port );
  }

  RETURN( fd );
}

void game_loop( void )
{
  static struct timeval   timeout;
  struct        timeval   last_time;
  DESCRIPTOR_DATA       * man;
  DESCRIPTOR_DATA       * d;
  NET_DATA              * pNet;
  fd_set                  in_set;   /* �iŪ�������X */
  fd_set                  out_set;  /* �i�g�J�����X */
  fd_set                  exc_set;  /*   �ҥ~�����X */
  int                     maxdesc;
  int                     connect;
  int                     loop;
  #if !defined(sun)
  int                     mask;
  #endif
  char                    information[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "game_loop" );

  #if !defined(sun)
  mask = sigmask(SIGUSR1) | sigmask(SIGUSR2) | sigmask(SIGINT)  |
         sigmask(SIGPIPE) | sigmask(SIGALRM) | sigmask(SIGTERM) |
         sigmask(SIGURG)  | sigmask(SIGXCPU) | sigmask(SIGHUP);
  #endif

  /* �D�n���j�� */
  while ( !merc_down )
  {
    /* �O�d�@���ɮץN�X�H�Ѩϥ� */
    if ( !fpReserve ) fopen( null_file, "r" );
    clear_reload_status();

    /* �]�w�ɶ� */
    gettimeofday( &last_time, NULL );
    current_time = ( time_t ) last_time.tv_sec;

    /* �ˬd�@�ɭp���骺 timer, �B�R�� idle �Ӥ[���@�ɭp����*/
    inc_share_memory_timer();

    /* �M���ɮ׳B�z�N�X���X���Ҧ����줸 */
    FD_ZERO( &in_set  );
    FD_ZERO( &out_set );
    FD_ZERO( &exc_set );

    /* �]�w�ɮ׳B�z�N�X���X��������Y���ɮ׳B�z�N�X����W�줸 */
    FD_SET( Internal, &in_set );

    for ( maxdesc = loop = 0; loop < MAX_PORT; loop++ )
    {
      if ( Control[loop] != ERRORCODE )
      {
        FD_SET( Control[loop], &in_set );
        maxdesc = UMAX( Control[loop], Internal );
      }
    }

    /* ��X�ݭn���ˬd���̰���Ʀr */
    for ( man = descriptor_list; man; man = man->next )
    {
      if ( !verify_desc( man ) ) continue;

      maxdesc = UMAX( maxdesc, man->descriptor );
      FD_SET( man->descriptor, &in_set  );
      FD_SET( man->descriptor, &out_set );
      FD_SET( man->descriptor, &exc_set );
    }

    /* �[�J���ڳs�u, �åB�[�J�ɶ� */
    for ( pNet = net_list; pNet; pNet = pNet->next )
    {
      /* ��J�譱 */
      if ( valid_socket( pNet, NET_IMPORT ) )
      {
        pNet->import_duration++;
        maxdesc = UMAX( maxdesc, ( connect = pNet->import ) );
        FD_SET( connect, &in_set  );
        FD_SET( connect, &exc_set );
      }

      /* ��X�譱 */
      if ( valid_socket( pNet, NET_EXPORT ) )
      {
        pNet->export_duration++;
        maxdesc = UMAX( maxdesc, ( connect = pNet->export ) );
        FD_SET( connect, &out_set );
        FD_SET( connect, &exc_set );
      }
    }

    /* �]�w select ���ݪ��ɶ� */
    timeout.tv_sec  = 1;
    timeout.tv_usec = 0;

    #if !defined(sun)
    sigsetmask( mask );
    #endif

    if ( select( maxdesc+1, &in_set, &out_set, &exc_set, &timeout ) < 0 )
    {
      mudlog( LOG_INFO, strerror( errno ) );
      mudlog( LOG_CRIT, "select() ��Ʀ����D." );
    }

    #if !defined(sun)
    sigsetmask( 0 );
    #endif

    /* �@�ɭp����B�z */
    handle_share_memory();

    /* �O�_���s���s�u ? */
    for ( loop = 0; loop < MAX_PORT; loop++ )
    {
      if ( Control[loop] != ERRORCODE && FD_ISSET( Control[loop], &in_set ) )
        new_descriptor( Control[loop], loop );
    }

    /* �O�_�����ڳs�u */
    if ( FD_ISSET( Internal, &in_set ) ) launch_internal( Internal );

    /* �𱼤����`���s�u */
    for ( d = descriptor_list; d; d = d->next )
    {
      if ( !verify_desc( d ) ) continue;

      if ( FD_ISSET( d->descriptor, &exc_set ) )
      {
        FD_CLR( d->descriptor, &in_set  );
        FD_CLR( d->descriptor, &out_set );

        if ( d->character )
        {
          save_char_obj( d->character , SAVE_FILE );
          mudlog( LOG_INFO , "%s �s�u�����`." , d->character->name );
        }

        d->outtop = 0;
        close_socket( d );
      }
    }

    /* �𱼤����`�����ڳs�u */
    for ( pNet = net_list; pNet; pNet = pNet->next )
    {
      /* ��J���� */
      if ( valid_socket( pNet, NET_IMPORT ) )
      {
        if ( FD_ISSET( connect = pNet->import, &exc_set ) )
        {
          FD_CLR( connect, &in_set );
          mudlog( LOG_NET, "[���ڳs�u] %s ��J���~.", pNet->address );
          close_net_socket( pNet, NET_IMPORT );
        }
      }

      /* ��X���� */
      if ( valid_socket( pNet, NET_EXPORT ) )
      {
        if ( FD_ISSET( connect = pNet->export, &exc_set ) )
        {
          FD_CLR( connect, &out_set );
          mudlog( LOG_NET, "[���ڳs�u] %s ��X���~.", pNet->address );
          close_net_socket( pNet, NET_EXPORT );
        }
      }
    }

    /* ���ڳs�u��J��X���� */
    for ( pNet = net_list; pNet; pNet = pNet->next )
    {
      /* ��J���� */
      if ( valid_socket( pNet, NET_IMPORT ) )
      {
        if ( FD_ISSET( connect = pNet->import ,&in_set ) )
        {
          int    nRead;
          char   buf[MAX_STRING_LENGTH];
          char   arg1[MAX_INPUT_LENGTH * 2 ];
          char   arg2[MAX_INPUT_LENGTH * 2 ];
          char   arg3[MAX_INPUT_LENGTH * 2 ];
          char * pString;

          if ( ( nRead = read( connect, buf, sizeof( buf ) - 10 ) ) > 0 )
          {
            /* �������檺�r�� */
            while ( nRead > 2 )
            {
              if ( buf[nRead-1] != '\n' && buf[nRead-1] != '\r' ) break;
              nRead--;
            }

            if ( nRead < MAX_INPUT_LENGTH )
            {
              buf[nRead] = '\x0';

              pString = one_argument( buf, arg1 );

              /* �S��R�O */
              if ( arg1[0] == '!' )
              {
                /* �֥[�p�ƾ� */
                pNet->nRead++;
              }

              else
              {
                pString = one_argument( pString, arg2 );
                pString = one_argument( pString, arg3 );

                if ( arg1[0] && arg2[0] && arg3[0] )
                {
                  sprintf( information,
                    "�ѻ������ͯ�%s��%s(%s)�ǨӪ��T���R%s�C\n\r"
                    , pNet->name, arg1, arg2, arg3 );

                  /* �֥[�p�ƾ� */
                  pNet->nRead++;

                  /* �e���Ҧ������a */
                  send_to_all_char( information );

                 /* �O�_�n�O�� */
                 if ( pNet->log )
                   mudlog( LOG_NET, "%s:%s", pNet->address, buf );
                }
              }
            }
          }

          /* Ū�����~ */
          else if ( nRead == 0 )
          {
            mudlog( LOG_NET, "[���ڳs�u] %s Ū�����~.", pNet->address );
            close_net_socket( pNet, NET_IMPORT );
          }

          else if ( errno != EWOULDBLOCK )
          {
            mudlog( LOG_NET, "[���ڳs�u] %s Ū�����~.", pNet->address );
            close_net_socket( pNet, NET_IMPORT );
          }
        }
      }

      /* ��X���� */
      if ( valid_socket( pNet, NET_EXPORT ) )
      {
        int nWrite;
        int iLen;

        if ( ( iLen = str_len( pNet->message ) ) <= 0 ) continue;

        if ( ( nWrite = write( pNet->export, pNet->message, iLen ) ) < 0 )
        {
          mudlog( LOG_NET, "[���ڳs�u] %s �g�J���~.", pNet->address );
          close_net_socket( pNet, NET_EXPORT );
        }

        pNet->message[0] = '\x0';
        pNet->nWrite++;
      }
    }

    /* ��J */
    for ( d = descriptor_list; d; d = d->next )
    {
      if ( !verify_desc( d ) ) continue;

      d->fcommand = FALSE;

      if ( FD_ISSET( d->descriptor, &in_set ) )
      {
        if ( d->character ) d->character->timer = 0;

        if ( !read_from_descriptor( d ) )
        {
          FD_CLR( d->descriptor, &out_set );

          if ( d->character )
          {
            save_char_obj( d->character , SAVE_FILE );
            mudlog( LOG_INFO , "%s ����Ū�������`." , d->character->name );
          }

          d->outtop = 0;
          close_socket( d );
          continue;
        }
      }

      /* ����a���p�ƾ� */
      if ( d->tick_count > 0 ) d->tick_count--;

      /* ����a�� chat_error */
      if ( d->chat_error > 0 )
      {
        d->chat_error--;
        continue;
      }

      /* ����a�� wait_time */
      if ( d->character && d->character->wait > 0 )
      {
        d->character->wait--;
        continue;
      }

      /* �⪱�a��J�����Ū�J���a��Ƹ̭� */
      read_from_buffer( d );

      /* ���檱�a����J */
      switch( d->connected )
      {
      default :
        mudlog( LOG_DEBUG, "���~�����A %d.", d->connected );
        break;

      case CON_GET_FQDN   :
      case CON_GET_FINGER :
      case CON_VERIFY     :
      case CON_CHECK_FILE :
        nanny( d, "" );
        d->incomm[0] = '\x0';
        break;

      case CON_GET_NAME             :
      case CON_PLAYING              :
      case CON_GET_OLD_PASSWORD     :
      case CON_CONFIRM_NEW_NAME     :
      case CON_GET_NEW_PASSWORD     :
      case CON_CONFIRM_NEW_PASSWORD :
      case CON_GET_SEX              :
      case CON_READ_MOTD            :
      case CON_GET_CNAME            :
      case CON_GET_EMAIL_ADDRESS    :
      case CON_CHOOSE_SKILL         :
      case CON_CHOOSE_MODE          :
      case CON_CHOOSE_ANGEL         :
      case CON_CHOOSE_HOME          :
      case CON_CONTRACT             :

        if ( d->incomm[0] )
        {
          d->alias_repeat = 0;
          d->fcommand     = TRUE;
          stop_idling( d->character );

          if ( d->connected == CON_PLAYING )
          {
            /* �O�_������X�����r�� */
            if ( d->edit_mode != EDIT_NONE )
            {
              accept_edit( d->character, d->incomm );
            }

            else if ( d->showstr_point )
            {
              write_to_descriptor( d, "\e[20D\e[1A\e[0m                     "
              "                                                   \e[80D", 0 );
              show_string( d, d->incomm );
            }

            else
            {
              interpret( d->character , d->incomm );
            }
          }

          else
          {
            nanny( d, d->incomm );
          }

          d->incomm[0] = '\x0';
        }
        break;
      }
    }

    /* �۰�Ĳ�o�C���ʧ@ */
    update_handler();

    /* ��X */
    for ( d = descriptor_list; d; d = d->next )
    {
      if ( !verify_desc( d ) || d->edit_mode != EDIT_NONE ) continue;

      if ( ( d->fcommand || d->outtop > 0 ) )
      {
        if ( !process_output( d, TRUE ) )
        {
          if ( d->character )
          {
            save_char_obj( d->character , SAVE_FILE );
            mudlog( LOG_INFO , "%s ���ݿ�X�����`." , d->character->name );
          }

          d->outtop = 0;
          close_socket( d );
        }
      }
    }

    /* ��J�ݬݬO�_���~�Ӧh */
    for ( d = descriptor_list; d; d = d->next )
    {
      if ( !verify_desc( d ) || d->connected == CON_PLAYING ) continue;

      if ( d->fqdn_limit )
      {
        mudlog( LOG_INFO, "%s �S�� FQDN �Q�ڵ��s�u.", d->host );
        write_to_descriptor( d, "�A�S�� FQDN�M�ڵ��s�u�M�����s�u�T\n\r", 0 );
        d->outtop = 0;
        close_socket( d );
        continue;
      }

      /* ���~�Ӧh */
      if ( d->error >= MaxLoginError )
      {
        mudlog( LOG_INFO , "%s �i�J�ɿ��~�Ӧh.", d->host );
        write_to_descriptor( d,
          "\n\r\n\r�A�����~�Ӧh�F�M�ЧA�y��A�i�J�C\n\r" , 0 );
        d->outtop = 0;
        close_socket( d );
        continue;
      }
    }

    /* �P�����P�B */
    /* Sleep( last_time + 1/PULSE_PER_SECOND - now ). */
    /* Careful here of signed versus unsigned arithmetic. */
    {
      struct timeval now_time;
      long           secDelta;
      long           usecDelta;

      gettimeofday( &now_time, NULL );
      usecDelta   = ((int) last_time.tv_usec) - ((int) now_time.tv_usec)
                  + 1000000 / PULSE_PER_SECOND;
      secDelta    = ((int) last_time.tv_sec ) - ((int) now_time.tv_sec );
      while ( usecDelta < 0 )
      {
        usecDelta += 1000000;
        secDelta  -= 1;
      }

      while ( usecDelta >= 1000000 )
      {
        usecDelta -= 1000000;
        secDelta  += 1;
      }

      if ( secDelta > 0 || ( secDelta == 0 && usecDelta > 0 ) )
      {
        struct timeval stall_time;

        stall_time.tv_usec = usecDelta;
        stall_time.tv_sec  = secDelta;

        if ( select( 0 , (fd_set * ) 0
          , (fd_set * ) 0 , (fd_set * ) 0 , &stall_time ) < 0)
        {
          mudlog( LOG_INFO, strerror( errno ) );
          mudlog( LOG_CRIT, "�ɶ��P�B����� select() �����D" );
        }
      }
    }

    /* ��s���c */
    struct_updates();

    /* �W�[�t�έp�ƾ� */
    traceloop++;
  }

  RETURN_NULL();
}

/* �إ߷s���y�z�� */
void new_descriptor( int control, int slot )
{
  char                 buf[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA    * dnew;
  BAN_DATA           * pBan;
  struct sockaddr_in   sock;
  int                  desc;
  int                  size;
  int                  addr;
  int                  loop;

  PUSH_FUNCTION( "new_descriptor" );

  size = sizeof( sock );
  getsockname( control, ( struct sockaddr * ) &sock, &size );

  if ( ( desc = accept( control, (struct sockaddr *) &sock, &size) ) < 0 )
  {
    mudlog( LOG_INFO, strerror( errno ) );
    mudlog( LOG_INFO, "New_descriptor: accept ��Ʀ����D." );
    RETURN_NULL();
  }

  #if !defined(FNDELAY)
  #define FNDELAY O_NDELAY
  #endif

  if ( fcntl( desc, F_SETFL, FNDELAY ) < 0 )
  {
    mudlog( LOG_INFO, strerror( errno ) );
    mudlog( LOG_INFO, "New_descriptor: fcntl ��Ʀ����D." );
    RETURN_NULL();
  }

  /* �s����@�ӷs���y�z�� */
  dnew = alloc_struct( STRUCT_DESCRIPTOR_DATA );

  dnew->descriptor     = desc;
  dnew->connected      = CON_GET_FQDN;
  dnew->tick           = ERRORCODE;
  dnew->tick_count     = ERRORCODE;
  dnew->file           = ERRORCODE;
  dnew->id_number      = ++merc_id;
  dnew->check_fqdn     = FALSE;
  dnew->check_finger   = FALSE;
  dnew->check_file     = FALSE;
  dnew->lock_file      = FALSE;
  dnew->fqdn_limit     = FALSE;
  dnew->stack          = FALSE;
  dnew->server         = FALSE;
  dnew->nRead          = 0;
  dnew->nWrite         = 0;
  dnew->naddr          = 0;
  dnew->edit_mode      = EDIT_NONE;
  dnew->timekeeper     = current_time;
  dnew->outbuf         = alloc_string( ( dnew->outsize = 2000 ) );
  memset( dnew->outbuf, 0, dnew->outsize );
  dnew->host           = str_dup( "" );
  dnew->address        = str_dup( "" );
  dnew->remote         = str_dup( "" );
  dnew->buffer         = str_dup( "" );
  dnew->path           = str_dup( "" );

  for ( loop = 0; loop < MAX_INNATE; loop++ ) dnew->select[loop] = FALSE;

  for ( loop = 0; loop < CHAT_REPEAT; loop++ )
    dnew->last_chat[loop][0] = '\x0';

  dnew->port           = MudPort[ URANGE( 0, slot, MAX_PORT - 1 ) ];
  MudConnect[ URANGE( 0, slot, MAX_PORT - 1 ) ]++;
  top_update();

  size = sizeof(sock);
  if ( getpeername( desc, ( struct sockaddr * ) &sock, &size ) < 0 )
  {
    mudlog( LOG_INFO, strerror( errno ) );
    mudlog( LOG_INFO, "New_descriptor: getpeername ��Ʀ����D." );

    dnew->host    = str_dup( "(����)" );
    dnew->address = str_dup( "(����)" );
    dnew->naddr   = 0;
  }

  else
  {
    dnew->naddr = ntohl( sock.sin_addr.s_addr );
    addr        = dnew->naddr;

    sprintf( buf, "%d.%d.%d.%d",
        ( addr >> 24 ) & 0xFF, ( addr >> 16 ) & 0xFF,
        ( addr >>  8 ) & 0xFF, ( addr       ) & 0xFF );

    mudlog( LOG_INFO , "�q��} %s ���T���L��" , buf );

    dnew->host      = str_dup( buf );
    dnew->address   = str_dup( buf );
    dnew->inet_addr = inet_addr( buf );

    /* �ˬd�O�_�Q�T��s���i�� */
    for ( pBan = ban_list; pBan; pBan = pBan->next )
    {
      if ( !str_prefix( pBan->name , dnew->host ) )
      {
        write_to_descriptor( dnew, "�藍�_�M�A����}�Q�T��s���L�ӡC\n\r", 0 );
        mudlog( LOG_INFO, "%s �Q�T��s�u�C", dnew->host );

        #ifdef __linux__
        shutdown( desc ,2 );
        #endif

        close( desc );

        /* ���񵲺c���O���� */
        free_struct( dnew , STRUCT_DESCRIPTOR_DATA );
        RETURN_NULL();
      }
    }

    if ( GetFQDN )
    {
      slot = get_free_share_memory( dnew, INTERNAL_GETHOSTBYADDR );

      if ( slot < 0 )
      {
        write_to_descriptor( dnew, "�藍�_�M�O���餣���M�L�k�s�u�C\n\r", 0 );

        #ifdef __linux__
        shutdown( desc ,2 );
        #endif

        close( desc );

        /* ���񵲺c���O���� */
        free_struct( dnew , STRUCT_DESCRIPTOR_DATA );
        RETURN_NULL();
      }

      /* �Ѥl��{�Ӭd�ߪ��a��FDQN �M���ݨϥΪ̸��X */
      switch( fork() )
      {
      case -1:

        SystemCrash = TRUE;
        clean_share_memory_address( slot );
        mudlog( LOG_INFO , strerror( errno ) );
        mudlog( LOG_CRIT , "fork() ��Ʀ����D." );
        break;

       /* �l��{���� */
      case 0:

        SystemCrash = TRUE;

        signal( SIGPIPE, SIG_IGN );
        signal( SIGCHLD, SIG_IGN );

        /* �����Ҧ��q�T */
        close_communication();

        switch( fork() )
        {
        case -1 :

          SystemCrash = TRUE;
          clean_share_memory_address( slot );
          mudlog( LOG_INFO, strerror( errno ) );
          mudlog( LOG_CRIT, "fork() ��Ʀ����D." );
          exit( 0 );

        case 0:
          {
            char                 buffer[ MAX_STRING_LENGTH ];
            char               * user;
            char               * address;
            struct sockaddr_in * sin = ( struct sockaddr_in * ) ( &sock );
            struct hostent     * from;

            SystemCrash = TRUE;

            /* �����Ҧ��q�T */
            close_communication();

            signal( SIGPIPE, SIG_IGN );
            signal( SIGCHLD, SIG_IGN );

            user = get_user_name( ( int ) desc , sin );

            if ( !( address = address_lookup( buf ) ) )
            {
              from = gethostbyaddr( ( char * ) &sock.sin_addr
                       ,sizeof( sock.sin_addr ), AF_INET );
              address = from ? from->h_name : FromUnknown;
            }

            sprintf( buffer , "%s %s", address, user ? user : FromUnknown );
            set_share_memory_text( slot , buffer );
            exit( 0 );
          }
          break;

        default:
          exit( 0 );
        }
        break;

      /* ����{���� */
      default:
        break;
      }
    }
    else
    {
      set_address_data( dnew->host, dnew->host );
      dnew->check_fqdn = TRUE;
    }
  }


  write_to_descriptor( dnew, "�ˬd�s�u��}�M�еy�ݤ���..." , 0 );
  RETURN_NULL();
}

void close_communication( void )
{
  int loop;

  PUSH_FUNCTION( "close_communication" );

  for ( loop = 0; loop < MAX_PORT; loop++ )
  {
    if ( Control[loop] != ERRORCODE )
    {
      #ifdef __linux__
      shutdown( Control[loop], 2 );
      #endif

      close( Control[loop] );
    }
  }

  #ifdef __linux__
  shutdown( Internal, 2 );
  #endif

  close( Internal );

  RETURN_NULL();
}

/* �����@�� socket */
void close_socket( DESCRIPTOR_DATA * pDesc )
{
  CHAR_DATA       * ch;
  DESCRIPTOR_DATA * man;
  char              buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "close_socket" );

  if ( !pDesc || !verify_desc( pDesc ) )
  {
    mudlog( LOG_DEBUG, "close_socket: �ӷ������T." );
    RETURN_NULL();
  }

  /* �������p���@�ɭp���� */
  update_share_memory( pDesc );

  if ( pDesc->outtop > 0 ) process_output( pDesc, FALSE );

  if ( pDesc->snoop_by )
  {
    set_descriptor_stack( pDesc->snoop_by );
    write_to_buffer( pDesc->snoop_by, "�A���ؼФw�g���}�C���F�C\n\r", 0 );
    pDesc->snoop_by->stack = FALSE;
  }

  /* �N���H�Q�ʵ��M�� */
  for ( man = descriptor_list; man; man = man->next )
    if ( verify_desc( man ) && man->snoop_by == pDesc ) man->snoop_by = NULL;

  if ( ( ch = pDesc->character ) )
  {
    mudlog( LOG_INFO , "%s ��M���h�s�u.", ch->name );

    if ( ch->answer )
    {
      free_struct( ch->answer, STRUCT_ANSWER_DATA );
      ch->answer = NULL;
    }

    /* �����յ��c��� */
    extract_gamble( ch );
    ch->gamble = NULL;

    if ( pDesc->connected == CON_PLAYING )
    {
      act( "$n��M�Q�@�}�ѤW���U�Ӫ��p����M���h��ı�M���L���T"
        , ch, NULL, NULL, TO_ROOM );

      if ( is_pk( ch ) )
      {
        switch( fight_info->status )
        {
        case PK_JOIN:

          /* ���e�^ */
          if ( fight_info->fight_1 && fight_info->fight_1 != ch )
          {
            char_from_room( fight_info->fight_1 );
            char_to_room( fight_info->fight_1
              , get_hometown( fight_info->fight_1 ) );

            /* �M���l�ܬ����I */
            clear_trace( fight_info->fight_1, TRUE );
          }

          if ( fight_info->fight_2 && fight_info->fight_2 != ch )
          {
            char_from_room( fight_info->fight_2 );
            char_to_room( fight_info->fight_2
              , get_hometown( fight_info->fight_2 ) );

            /* �M���l�ܬ����I */
            clear_trace( fight_info->fight_2, TRUE );
          }

          sprintf( buf, "%s�������]�F�M�ҥH�۰ʱ��v�T", mob_name( NULL, ch ) );
          talk_channel_2( buf, CHANNEL_PK, "" );

          if ( fight_info->fight_1 == ch ) fight_info->fight_1 = NULL;
          if ( fight_info->fight_2 == ch ) fight_info->fight_2 = NULL;

          char_from_room( ch );
          char_to_room( ch, get_hometown( ch ) );
          break;

        case PK_BET:

          /* ���e�^ */
          if ( fight_info->fight_1 && fight_info->fight_1 != ch )
          {
            char_from_room( fight_info->fight_1 );
            char_to_room( fight_info->fight_1
              , get_hometown( fight_info->fight_1 ) );

            /* �M���l�ܬ����I */
            clear_trace( fight_info->fight_1, TRUE );
          }

          if ( fight_info->fight_2 && fight_info->fight_2 != ch )
          {
            char_from_room( fight_info->fight_2 );
            char_to_room( fight_info->fight_2
              , get_hometown( fight_info->fight_2 ) );

            /* �M���l�ܬ����I */
            clear_trace( fight_info->fight_2, TRUE );
          }

          sprintf( buf, "%s�������]�F�M�ҥH�o�����ɤ���T"
            , mob_name( NULL, ch ) );

          talk_channel_2( buf, CHANNEL_PK, "" );

          char_from_room( ch );
          char_to_room( ch, get_hometown( ch ) );

          init_fight( fight_info );
          fight_info->status = PK_DESERT;
          break;

        case PK_FIGHT:

          /* ��L���ͩR�O���C�M�۰��� damage �h�B�z */
          if ( ch->hit > 1 ) ch->hit = 1;
          break;
        }
      }
    }

    else
    {
      free_char( pDesc->character );
    }

    ch->desc = NULL;
  }

  #ifdef __linux__
  shutdown( pDesc->descriptor, 2 );
  #endif

  close( pDesc->descriptor );

  /* ��w���a�ɮ� */
  lock_file( pDesc );

  /* ���񵲺c�O���� */
  free_struct( pDesc , STRUCT_DESCRIPTOR_DATA );

  RETURN_NULL();
}

bool read_from_descriptor( DESCRIPTOR_DATA * d )
{
  int iStart;

  PUSH_FUNCTION( "read_from_descriptor" );

  /* �p�G��J�w�İϤ��O�Ū��h�^�h */
  if ( d->incomm[0] ) RETURN( TRUE );

  /* �ˬd�O�_�W�X�d�� */
  if ( ( iStart = str_len( d->inbuf ) ) >= ( int ) sizeof( d->inbuf ) - 10 )
  {
    mudlog( LOG_INFO , "%s ��J�w�İϤw��.", d->host );

    write_to_descriptor( d,
      "\n\r�A���w�İϤw���M�t�Υu�n��A�j���_�u�C\n\r", 0 );
    RETURN( FALSE );
  }

  /* Ū�J��J�w�İ� */
  for ( ;; )
  {
    int nRead;
    int iCount;

    if ( ( iCount = sizeof( d->inbuf ) - 10 - iStart ) <= 0 )
    {
      mudlog( LOG_INFO , "%s ��J�w�İϤw��.", d->host );

      write_to_descriptor( d,
        "\n\r�A���w�İϤw���M�t�Υu�n��A�j���_�u�C\n\r", 0 );
      RETURN( FALSE );
    }

    if ( ( nRead = read( d->descriptor, d->inbuf + iStart, iCount ) ) > 0 )
    {
      d->nRead      += nRead;
      ReadKiloBytes += nRead / 1000;
      ReadBytes     += nRead % 1000;
      if ( top_list ) top_list->nRead += nRead;

      while ( ReadBytes >= 1000 )
      {
        ReadKiloBytes++;
        ReadBytes -= 1000;
      }

      iStart += nRead;
      if ( d->inbuf[iStart-1] == '\n' || d->inbuf[iStart-1] == '\r' ) break;
    }

    else if ( nRead == 0 )
    {
      mudlog( LOG_INFO , "�q��J�w�İ�Ū�� EOF (End Of File)." );
      RETURN( FALSE );
    }

    else if ( errno == EWOULDBLOCK ) break;
    else
    {
      mudlog( LOG_INFO, "Read_from_descriptor: %s" , strerror( errno ) );
      RETURN( FALSE );
    }
  }

  d->inbuf[iStart] = '\x0';
  RETURN( TRUE );
}

/* �q��J�w�İ�Ū�i�Ӥ@���� */
void read_from_buffer( DESCRIPTOR_DATA * d )
{
  extern bool tick_permit;
  int         i;
  int         j;
  int         k;

  PUSH_FUNCTION( "read_from_buffer" );

  /* �p�G��J�w�İϤ��O�Ū��h�^�h, �]���|���B�z�W�������O. */
  if ( d->incomm[0] ) RETURN_NULL();

  /* �ˬd�O�_�n��J�p�ƾ����R�O */
  if ( d->connected  == CON_PLAYING
    && d->tick_count == 0
    && d->character
    && ( tick_permit || IS_IMMORTAL( d->character ) ) )
  {
    /* �p�G�j��t�Ϊ��w�]�� */
    if ( str_len( d->tick_command ) + str_len( d->inbuf )
      > sizeof( d->incomm ) - 10 )
    {
      write_to_descriptor( d, "\e[1;31m##BADTICK!!!\e[0m\n\r" , 0 );
      d->tick_count = d->tick + 1;
    }

    else
    {
      char buffer[ MAX_INPUT_LENGTH ];

      str_cpy( buffer, d->inbuf );
      if ( d->tick_command[0] ) str_cpy( d->inbuf , d->tick_command );
      str_cat( d->inbuf, "\n\r" );
      str_cat( d->inbuf, buffer );

      write_to_descriptor( d, "\e[1;32m##TICK!!!\e[0m" , 0 );
      d->tick_count = d->tick + 1;
    }
  }

  /* �j�M�ܤ֭n����J�@�C */
  for ( i = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++ )
  {
    if ( i >= ( sizeof( d->inbuf ) - 2 ) )
    {
      mudlog( LOG_DEBUG, "read_from_buffer: ��J�ݲ��`." );
      d->inbuf[0]  = '\x0';
      d->incomm[0] = '\x0';
      RETURN_NULL();
    }

    if ( d->inbuf[i] == '\x0' ) RETURN_NULL();
  }

  /* ���j���� */
  spilt_semicolon( d );

  /* �o�������`���r */
  for ( i = 0; d->inbuf[i]; i++ )
  {
    switch( d->inbuf[i] )
    {
    default:
      continue;

    case '\x0e': /* �|�Ϥ���ᱼ */
    case '\a'  : /* �a�n         */
    case '\x09': /* Tab ��       */

      d->inbuf[i] = '+';
      break;
    }
  }

  for ( i = k = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++ )
  {
    /* �ˬd�O�_�W�X�}�C���j�p */
    if ( k >= sizeof( d->incomm ) - 10 )
    {
      write_to_descriptor( d, "��J��Ӫ��C\n\r", 0 );

      /* ��Ѿl���r���� */
      for ( ; d->inbuf[i]; i++ )
      {
        if ( i >= sizeof( d->inbuf ) - 2 )
        {
          mudlog( LOG_DEBUG, "read_from_buffer: ��J�ݲ��`." );
          d->inbuf[0]  = '\x0';
          d->incomm[0] = '\x0';
          RETURN_NULL();
        }

        if ( d->inbuf[i] == '\n' || d->inbuf[i] == '\r' ) break;
      }

      d->inbuf[i]   = '\n';
      d->inbuf[i+1] = '\x0';
      break;
    }

    /* �O�_���˰h�� */
    if ( d->inbuf[i] == '\b' )
    {
      if ( k > 0 ) k--;
    }

    else
    {
      d->incomm[k++] = d->inbuf[i];
    }
  }

  /* ���J�w�İϰ��ӽվ� */
  while ( d->inbuf[i] == '\n' || d->inbuf[i] == '\r' )
  {
    if ( ++i >= sizeof( d->inbuf ) - 2 )
    {
      mudlog( LOG_DEBUG, "read_from_buffer: ��J�ݲ��`." );
      d->inbuf[0]  = '\x0';
      d->incomm[0] = '\x0';
      RETURN_NULL();
    }
  }

  for ( j = 0; ( d->inbuf[j] = d->inbuf[i+j] ); j++ )
  {
    if ( ( i+j ) >= sizeof( d->inbuf ) - 2 )
    {
      mudlog( LOG_DEBUG, "read_from_buffer: ��J�ݲ��`." );
      d->inbuf[0]  = '\x0';
      d->incomm[0] = '\x0';
      RETURN_NULL();
    }
  }

  if ( k == 0 ) d->incomm[k++] = ' ';

  /* ��Ū�J����J�r��[�J�����Ÿ� */
  d->incomm[k] = '\x0';

  /* ½Ķ�������O�����O */
  translate_alias( d , d->incomm );

  /* ½Ķ # �R�O */
  translate_command( d );

  /* �ݬݬO�_���Ʀ��ƤӦh */
  if ( k > 1 || d->incomm[0] == '!' )
  {
    if ( d->incomm[0] != '!'
      && str_cmp( d->incomm , d->inlast[d->substitution] ) )
    {
      d->repeat = 0;
    }

    else
    {
      if ( ++d->repeat > MaxRepeat )
      {
        mudlog( LOG_INFO , "%s ��J���ƤӦh.", d->host );

        write_to_descriptor( d, "\n\r�A��J���r�꭫�ƤӦh�C\n\r" , 0 );
        str_cpy( d->incomm, "quit\x0" );
      }
    }
  }

  if ( d->connected == CON_PLAYING )
  {
    /* ���N ! ���ʧ@ */
    if   ( d->incomm[0] == '!' )
    {
      int order;

      order = d->incomm[1] - '0';

      if ( order > 0 && order <= MAX_HISTORY ) order--;
      else                                     order = 0;

      if ( d->inlast[order][0] == '\x0' )
      {
        d->substitution = 0;
        str_cpy( d->incomm , "history\x0" );
      }

      else
      {
        str_cpy( d->incomm , d->inlast[ ( d->substitution = order ) ] );
      }
    }

    /* �_�h�վ��J���O�� */
    else
    {
      int loop;

      if ( str_cmp( d->incomm , " \x0" ) )
      {
        for ( loop = MAX_HISTORY - 2; loop >= 0; loop-- )
          str_cpy( d->inlast[loop+1] , d->inlast[loop] );
      }
    }

    if ( str_cmp( d->incomm , " \x0" ) ) str_cpy( d->inlast[0] , d->incomm );
  }
  RETURN_NULL();
}

/* �C����X��� */
bool process_output( DESCRIPTOR_DATA * d, bool fPrompt )
{
  extern bool   merc_down;
  char          buf[MAX_INPUT_LENGTH * 2];
  CHAR_DATA   * ch;
  CHAR_DATA   * victim;

  PUSH_FUNCTION( "process_output" );

  /* ��X���ܦC */
  if ( fPrompt && !merc_down && d->connected == CON_PLAYING )
  {
    /* �O�_������X������ */
    if ( d->showstr_point )
    {
      char buffer[ MAX_STRING_LENGTH ];

      sprintf( buffer, "\e[80D\e[0m--More--(%2d%%)"
         , 100 - 100 * ( d->showstr_point - d->showstr_head ) /
           UMAX( 1, str_len( d->showstr_head ) ) );

      write_to_buffer( d, buffer , 0 );
    }

    /* �w�g���e����X�����F */
    else
    {
      ch = d->character;

      if ( IS_SET( ch->act, PLR_BLANK ) )  write_to_buffer( d, "\n\r", 0 );

      if ( ( victim = ch->fighting ) && verify_char( victim ) )
        bust_fight_message( ch, victim );

      if ( IS_SET( ch->act, PLR_PROMPT ) )
      {
        bust_a_prompt( ch );
      }
      else if ( ch->desc && ch->desc->fcommand )
      {
        if ( IS_IMMORTAL( ch ) )
        {
          sprintf( buf, "\e[1;37m/%s\e[0m$ > "
            , ch->desc->path ? ch->desc->path : "" );
          write_to_buffer( d, buf, 0 );
        }
        else
        {
          write_to_buffer( d, "> ", 0 );
        }
      }

      if ( IS_SET( ch->act, PLR_TELNET_GA ) )
        write_to_buffer( d, go_ahead_str, 0 );
    }
  }

  /* �p�G��X�O�Ū��h�ֳt���} */
  if ( d->outtop == 0 ) RETURN( TRUE );

  /* �ʵ��{�Ǳ����X */
  if ( d->snoop_by )
  {
    set_descriptor_stack( d->snoop_by );
    write_to_buffer( d->snoop_by, "�ʵ��R", 0 );
    write_to_buffer( d->snoop_by, d->outbuf, d->outtop );
    d->snoop_by->stack = FALSE;
  }

  if ( !write_to_descriptor( d, d->outbuf, d->outtop ) )
  {
    d->outtop = 0;
    RETURN( FALSE );
  }

  else
  {
    d->outtop = 0;
    RETURN( TRUE );
  }
}

void bust_fight_message( CHAR_DATA * ch, CHAR_DATA * victim )
{
  char buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "bust_fight_message" );

  if ( !ch || !victim || !verify_char( ch ) || !verify_char( victim ) )
  {
    mudlog( LOG_DEBUG, "bust_fight_message: �ӷ������T." );
    RETURN_NULL();
  }

  if ( get_curr_hit( ch ) > 0 && get_curr_hit( victim ) > 0 )
  {
    /* �L�X�ۤv�����p */
    sprintf( buf, "(\e[1;36m�A\e[0m%s\e[0m)\n\r"
      , status_message( ch ) );

    write_to_buffer( ch->desc , buf, 0 );

    /* �L�X��⪺���p */
    sprintf( buf , "(\e[1;36m%s\e[0m%s\e[0m)\n\r\n\r"
      , mob_name( ch, victim ), status_message( victim ) );

    write_to_buffer( ch->desc , buf, 0 );
  }

  RETURN_NULL();
}

/* �e�X Prompt �����a */
void bust_a_prompt( CHAR_DATA * ch )
{
  char         buf[MAX_STRING_LENGTH];
  char         buf2[MAX_STRING_LENGTH];
  char         new_format[MAX_STRING_LENGTH];
  const char * str;
  const char * i;
  char       * point;
  int          len;

  PUSH_FUNCTION( "bust_a_prompt" );

  if ( !ch->prompt || !*ch->prompt )
  {
    send_to_char( "\n\r", ch );
    RETURN_NULL();
  }

  point = buf;
  str   = ch->prompt;
  len   = 0;

  while( *str != '\x0' )
  {
    if ( len > sizeof( buf ) - 10 )
    {
      mudlog( LOG_DEBUG, "bust_a_prompt: �W�X�d��." );
      RETURN_NULL();
    }

    if ( *str != '^' )
    {
      *point++ = *str++;
      len++;
      continue;
    }

    str_cpy( buf2 , "^" );
    str_ncpy( new_format, str + 1, 3 );

    /* �ͩR��( �H�u��������� ) */
    if ( !str_cmp( new_format , "hps" ) )
    {
      sprintf( buf2, "\e[1;32m%s\e[0m"
        , return_percentage( ch , ch->hit , get_curr_hit( ch ) , 5 ) );
      str += 3;
    }

    /* �ͩR��( �H����������� ) */
    else if ( !str_cmp( new_format , "hpl" ) )
    {
      sprintf( buf2, "\e[1;32m%s\e[0m"
        , return_percentage( ch , ch->hit , get_curr_hit( ch ) , 10 ) );
      str += 3;
    }

    /* �A�{�b���ͩR��( �H�Ʀr��� ) */
    else if ( !str_cmp( new_format , "hp1" ) )
    {
      if ( ch->hit >= get_curr_hit( ch ) )
        sprintf( buf2, "\e[1;31m%d\e[0m", ch->hit );
      else
        sprintf( buf2, "\e[31m%d\e[0m", ch->hit );
      str += 3;
    }

    /* �A�̤j���ͩR��( �H�Ʀr��� ) */
    else if ( !str_cmp( new_format , "hp2" ) )
    {
      sprintf( buf2, "\e[1;31m%d\e[0m" , get_curr_hit( ch ) );
      str += 3;
    }

    /* �k�O��( �H����������� ) */
    else if ( !str_cmp( new_format , "mps" ) )
    {
      sprintf( buf2, "\e[1;32m%s\e[0m"
        , return_percentage( ch , ch->mana , get_curr_mana( ch ) , 5 ) );
      str += 3;
    }

    /* �k�O��( �H�u��������� ) */
    else if ( !str_cmp( new_format , "mpl" ) )
    {
      sprintf( buf2, "\e[1;32m%s\e[0m"
        , return_percentage( ch , ch->mana , get_curr_mana( ch ) , 10 ) );
      str += 3;
    }

    /* �A�{�b���k�O��( �H�Ʀr��� ) */
    else if ( !str_cmp( new_format , "mp1" ) )
    {
      if ( ch->mana >= get_curr_mana( ch ) )
        sprintf( buf2, "\e[1;36m%d\e[0m" , ch->mana );
      else
        sprintf( buf2, "\e[1;34m%d\e[0m" , ch->mana );
      str += 3;
    }

    /* �A�̤j���k�O��( �H�Ʀr��� ) */
    else if ( !str_cmp( new_format , "mp2" ) )
    {
      sprintf( buf2, "\e[1;36m%d\e[0m" , get_curr_mana( ch ) );
      str += 3;
    }

    /* ���ʤO( �H�u��������� ) */
    else if ( !str_cmp( new_format , "mvs" ) )
    {
      sprintf( buf2, "\e[1;32m%s\e[0m"
        , return_percentage( ch , ch->move , get_curr_move( ch ) , 5 ) );
      str += 3;
    }

    /* ���ʤO( �H����������� ) */
    else if ( !str_cmp( new_format , "mvl" ) )
    {
      sprintf( buf2, "\e[1;32m%s\e[0m"
        , return_percentage( ch , ch->move , get_curr_move( ch ) , 10 ) );
      str += 3;
    }

    /* �A�{�b����O��( �H�Ʀr��� ) */
    else if ( !str_cmp( new_format , "mv1" ) )
    {
      if ( ch->move >= get_curr_move( ch ) )
        sprintf( buf2, "\e[1;33m%d\e[0m" , ch->move );
      else
        sprintf( buf2, "\e[33m%d\e[0m" , ch->move );
      str += 3;
    }

    /* �A�̤j����O��( �H�Ʀr��� ) */
    else if ( !str_cmp( new_format , "mv2" ) )
    {
      sprintf( buf2, "\e[1;33m%d\e[0m" , get_curr_move( ch ) );
      str += 3;
    }

    /* ���� */
    else if ( !str_cmp( new_format , "lvl" ) )
    {
      sprintf( buf2, "\e[1;37m%d\e[0m" , ch->level );
      str += 3;
    }

    /* �}�� */
    else if ( !str_cmp( new_format , "ali" ) )
    {
      sprintf( buf2, "%s\e[0m"
        , IS_GOOD(ch) ? "\e[32m���}" : IS_EVIL(ch) ?
         "\e[34m���c" : "\e[36m����" );
      str += 3;
    }

    /* ���� */
    else if ( !str_cmp( new_format , "gol" ) )
    {
      sprintf( buf2, "\e[1;33m%d\e[0m" , ch->gold );
      str += 3;
    }

    /* �~�� */
    else if ( !str_cmp( new_format , "age" ) )
    {
      sprintf( buf2, "\e[34m%d\e[0m" , get_age( ch ) );
      str += 3;
    }

    /* �O�q */
    else if ( !str_cmp( new_format , "str" ) )
    {
      sprintf( buf2, "\e[31m%d\e[0m" , get_curr_str( ch ) );
      str += 3;
    }

    /* ���O */
    else if ( !str_cmp( new_format , "int" ) )
    {
      sprintf( buf2, "\e[32m%d\e[0m" , get_curr_int( ch ) );
      str += 3;
    }

    /* ���� */
    else if ( !str_cmp( new_format , "wis" ) )
    {
      sprintf( buf2, "\e[33m%d\e[0m" , get_curr_wis( ch ) );
      str += 3;
    }

    /* �ӱ� */
    else if ( !str_cmp( new_format , "dex" ) )
    {
      sprintf( buf2, "\e[34m%d\e[0m" , get_curr_dex( ch ) );
      str += 3;
    }

    /* ��� */
    else if ( !str_cmp( new_format , "con" ) )
    {
      sprintf( buf2, "\e[35m%d\e[0m" , get_curr_con( ch ) );
      str += 3;
    }

    /* �A�ҳѤU���g��� */
    else if ( !str_cmp( new_format, "exl" ) )
    {
      sprintf( buf2, "\e[1;33m%d\e[0m",
        kill_table[ URANGE( 1, ch->level, MAX_LEVEL -1 )].exp - ch->exp );
      str += 3;
    }

    /* �A�Ҿ֦����g��� */
    else if ( !str_cmp( new_format , "exp" ) )
    {
      sprintf( buf2, "\e[36m%d\e[0m" , ch->exp );
      str += 3;
    }

    /* �A�Ҧb���ж��W�� */
    else if ( !str_cmp( new_format , "roo" ) )
    {
      if ( ch->in_room ) sprintf( buf2, "\e[1;35m%s\e[0m", ch->in_room->name );
      else               sprintf( buf2, "�����W���ж�" );
      str += 3;
    }

    /* ��ܧA�i��a�����~�˼� */
    else if ( !str_cmp( new_format , "thi" ) )
    {
      sprintf( buf2, "\e[36m%d\e[0m" , can_carry_n( ch ) );
      str += 3;
    }

    /* ��ܧA�i��a�����~���q */
    else if ( !str_cmp( new_format , "wei" ) )
    {
      sprintf( buf2, "\e[1;34m%d\e[0m" , can_carry_w( ch ) );
      str += 3;
    }

    /* ���� */
    else if ( !str_cmp( new_format , "nex" ) )
    {
      str_cpy( buf2, "\n\r" );
      str += 3;
    }

    /* �A���R���v */
    else if ( !str_cmp( new_format , "hit" ) )
    {
      sprintf( buf2, "\e[1;31m%d\e[0m" , get_hitroll( ch ) );
      str += 3;
    }

    /* �A�����@�O */
    else if ( !str_cmp( new_format , "arm" ) )
    {
      sprintf( buf2, "\e[1;32m%d\e[0m" , get_ac( ch ) );
      str += 3;
    }

    /* �A�������ˮ`�O */
    else if ( !str_cmp( new_format , "dam" ) )
    {
      sprintf( buf2, "\e[1;33m%d\e[0m" , get_damroll( ch ) );
      str += 3;
    }

    /* �A�Ҧb�ж����s��( ���ګ��O ) */
    else if ( !str_cmp( new_format , "rrr" ) )
    {
      if ( IS_IMMORTAL( ch ) && ch->in_room )
         sprintf( buf2, "\e[1;37m%d\e[0m", ch->in_room->vnum );
      else str_cpy( buf2, " " );
      str += 3;
    }

    /* �A�Ҧb�ϰ쪺�W��( ���ګ��O ) */
    else if ( !str_cmp( new_format , "zon" ) )
    {
      if ( IS_IMMORTAL( ch ) && ch->in_room )
         sprintf( buf2, "\e[1;32m%s\e[0m" , ch->in_room->area->name );
      else str_cpy( buf2, " " );
      str += 3;
    }

    else if ( !str_cmp( new_format , "tim" ) )
    {
      sprintf( buf2 , "\e[1;36m%s\e[0m" , time_format( current_time , "%r" ) );
      str += 3;
    }

    else if ( !str_cmp( new_format , "dat" ) )
    {
      sprintf( buf2 , "\e[33m%s\e[0m"
        , time_format( current_time, "%a-%d-%b-%y" ) );
      str += 3;
    }

    str++;
    if ( len + str_len( i = buf2 ) < sizeof( buf ) - 10 )
      while( ( *point = *i ) != '\x0' ) ++point, ++i, len++;

    else
      mudlog( LOG_DEBUG , "bust_a_prompt: ��X�r��W�L�t�Φr�����" );
  }

  *point = '\x0';
  write_to_buffer( ch->desc, buf, 0 );
  RETURN_NULL();
}

/* ���X���F��[���X�w�İϤ� */
void write_to_buffer( DESCRIPTOR_DATA * d, const char * txt, int length )
{
  CHAR_DATA * victim;
  char      * target;
  char      * outbuf;
  int         outsize;
  bool        ansi;
  const char  warning[] =
    "\n\r\n\r\e[1;31m**�A����X�w�İϤӤj.�t�Φ۰ʭ�"
    "�s�]�w�A���w�İ�**\e[0m\n\r\n\r\0";

  PUSH_FUNCTION( "write_to_buffer" );

  /* �p�G�I�s�̥����w����, �h�]�w���� */
  if ( length <= 0 ) length = str_len( txt );

  if ( d->stack
    || ( d->connected == CON_PLAYING && d->edit_mode != EDIT_NONE ) )
  {
    save_to_stack( d, txt, length );
    RETURN_NULL();
  }

  /* �p�G�S�� \n\r if �h�ҩl�ƥ�. */
  if ( d->outtop == 0 && !d->fcommand )
  {
    if ( ( victim = d->character )
      && !IS_NPC( victim )
      && !IS_SET( victim->act, PLR_PROMPT ) )
    {
      d->outbuf[0] = '\x0';
      d->outtop    = 0;
    }

    else
    {
      d->outbuf[0] = '\n';
      d->outbuf[1] = '\r';
      d->outbuf[2] = '\x0';
      d->outtop    = 2;
    }
  }

  /* �p�G���פ����h�t�m�j�@�I���� */
  while ( d->outtop + length >= d->outsize - 1 )
  {
    /* �ˬd�O�_�W�X�t�m�O���骺���� */
    if ( ( outsize = ( d->outsize * 2 ) ) < MAX_OUTPUT_LENGTH )
    {
      outbuf = ( char * ) alloc_string( d->outsize * 2 );
      strncpy( outbuf, d->outbuf, d->outtop );
      *( outbuf + d->outtop ) = '\x0';
      free_string( d->outbuf );
      d->outbuf   = outbuf;
      d->outsize *= 2;
    }

    /* �W�X�����h�k�s */
    else
    {
      outbuf  = ( char * ) alloc_string( ( outsize = 2000 ) );
      *outbuf = '\x0';
      length  = str_len( ( txt = warning ) );
      free_string( d->outbuf );

      d->outbuf  = outbuf;
      d->outtop  = 0;
      d->outsize = 2000;
      mudlog( LOG_INFO , "[WARN] %s ��X�w�İϤӤj, �t�Φ۰ʭץ�"
        , d->character->name );
      break;
    }
  }

  /* �L�o ANSI ��X�r�� */
  target = d->outbuf + d->outtop;
  ansi   = ( d->character && IS_SET( d->character->turn , PLR_ANSI ) );

  while ( *txt && length > 0 )
  {
    if ( ansi && *txt == '\e' && *( txt +1 ) == '[' )
    {
      for ( ; *txt && *txt != 'm'; txt++, length-- );

      if ( *txt == 'm' )
      {
        txt++;
        length--;
      }
    }

    else
    {
      *target++ = *txt++;
      length--;
      d->outtop++;
    }
  }

  *target    = '\x0';

  if ( d->outtop >= d->outsize )
    mudlog( LOG_DEBUG, "write_to_buffer: ��X���~." );

  RETURN_NULL();
}

/* �̧C����X�����, ��X�@�q��r���ɮת��y�z��. */
/* �p�G�o�̦��o�{���D����(�Ҧp ofind all), �h�ոէ� MAX_BLOCK_SIZE ��� */
/* �L�O�Q�Χ⻷�ݷQ�V���@���ɮ�, ����@�g�J�ɮת��ʧ@�����C����X */
/* desc �K�O�o�ӻ����ɮת� handler */
bool write_to_descriptor( DESCRIPTOR_DATA * desc, char * txt, int length )
{
  int iStart;
  int nWrite;
  int nBlock;

  PUSH_FUNCTION( "write_to_descriptor" );

  if ( !desc )
  {
    mudlog( LOG_DEBUG, "write_to_descriptor: �S���ӷ�." );
    RETURN( FALSE );
  }

  if ( length <= 0 ) length = str_len( txt );
  for ( iStart = 0; iStart < length; iStart += nWrite )
  {
    nBlock = UMIN( length - iStart, MAX_BLOCK_SIZE );
    if ( ( nWrite = write( desc->descriptor, txt + iStart, nBlock ) ) < 0 )
    {
      mudlog( LOG_INFO, strerror( errno ) );
      mudlog( LOG_INFO, "write_to_descriptor: �g�J���~" );
      RETURN( FALSE );
    }

    desc->nWrite   += nWrite;
    WriteKiloBytes += nWrite / 1000;
    WriteBytes     += nWrite % 1000;
    if ( top_list ) top_list->nWrite += nWrite;

    if ( WriteBytes >= 1000 )
    {
      WriteKiloBytes++;
      WriteBytes -= 1000;
    }
  }

  RETURN( TRUE );
}

/* �B�z�|���s�u������ socket */
void nanny( DESCRIPTOR_DATA * d, char * argument )
{
  char             buf[MAX_STRING_LENGTH];
  char             temp[MAX_STRING_LENGTH];
  char             selection[MAX_INPUT_LENGTH];
  CHAR_DATA      * ch;
  AREA_DATA      * pArea;
  NOTE_DATA      * pnote;
  IMMLIST_DATA   * pImmlist;
  SKILL_DATA     * pSkill;
  RESTRICT_DATA  * pRest;
  OBJ_INDEX_DATA * pObjIndex;
  char           * pwdnew;
  char           * p;
  int              lines;
  int              notes;
  int              fOld;
  int              slot;
  int              serv;
  int              loop;

  PUSH_FUNCTION( "nanny" );

  if ( d->connected == CON_GET_CNAME )
  {
    while ( *argument == ' ' ) argument++;
  }

  else
  {
    while ( ( isSpace( ( int ) *argument )
      || *argument < 0
      || !isascii( ( int ) *argument )
      || !isprint( ( int ) *argument ) ) && *argument )
      argument++;
  }

  ch = d->character;

  switch ( d->connected )
  {
  default:

    mudlog( LOG_DEBUG , "Nanny: ���w�q�� d->connected %d.", d->connected );
    close_socket( d );
    RETURN_NULL();

  /* ���o���誺 FQDN �H�ΦW�r */
  case CON_GET_FQDN:

    if ( d->check_fqdn == FALSE )
    {
      sprintf( temp , "\b%c" , inflect[ d->inflect++ % 4 ] );
      write_to_descriptor( d, temp , 0 );
    }

    else
    {
      write_to_descriptor( d, "\b \n\r�ˬd���ݳs�u��ơM�еy��..." , 0 );
      d->connected    = CON_GET_FINGER;
      d->inflect      = 0;
      d->check_finger = TRUE;

      /* �ʥF����ñ�J�W�� */
      if ( !d->remote || !d->remote[0] )
      {
        str_cpy( d->finger_data , "�ʥF����ñ�J�m�W�C" );
        d->check_finger = TRUE;
      }

      else
      {
        slot = get_free_share_memory( d, INTERNAL_FINGER    );

        if ( slot < 0 )
        {
          write_to_descriptor( d, "�藍�_�M�O���餣���M�L�k�s�u�C\n\r", 0 );
          close_socket( d );
          RETURN_NULL();
        }

        if ( CheckServer )
        {
          serv = get_free_share_memory( d, INTERNAL_CHECKSERV );

          if ( serv < 0 )
          {
            write_to_descriptor( d, "�藍�_�M�O���餣���M�L�k�s�u�C\n\r", 0 );
            close_socket( d );
            RETURN_NULL();
          }
        }

        else
        {
          serv = -1;
        }

        switch ( fork() )
        {
        case -1:

          SystemCrash = TRUE;
          mudlog( LOG_INFO, strerror( errno ) );
          mudlog( LOG_INFO, "fork() ��Ʀ����D." );
          str_cpy( d->finger_data , "���� fork ��Ʀ����D." );
          clean_share_memory_address( slot );
          d->check_finger = TRUE;
          break;

        /* �l��{���� */
        case 0 :

          SystemCrash = TRUE;

          /* �����Ҧ��q�T */
          close_communication();

          signal( SIGPIPE, SIG_IGN );
          signal( SIGCHLD, SIG_IGN );

          switch( fork() )
          {
          case -1:

            SystemCrash = TRUE;
            mudlog( LOG_INFO, strerror( errno ) );
            mudlog( LOG_INFO, "fork() ��Ʀ����D." );
            set_share_memory_text( slot , "�{���L�k�}�� fork." );
            exit( 0 );

          case 0:
            {
              char               line[ MAX_STRING_LENGTH ];
              struct sockaddr_in sin;
              int                cfd;
              FILE             * fp;

              SystemCrash = TRUE;

              /* �����Ҧ��q�T */
              close_communication();

              signal( SIGPIPE, SIG_IGN );
              signal( SIGCHLD, SIG_IGN );

              if ( CheckServer )
              {
                memset( ( char * ) &sin, 0, sizeof( sin ) );
                sin.sin_family      = AF_INET;
                sin.sin_addr.s_addr = d->inet_addr;
                sin.sin_port        = htons( TelnetPort );

                cfd = socket( AF_INET, SOCK_STREAM, 0 );

                if ( connect( cfd, ( struct sockaddr *) & sin, sizeof( sin ) ) )
                  set_share_memory_text( serv , "0" );
                else
                  set_share_memory_text( serv , "1" );

                close( cfd );
              }

              memset( ( char * ) &sin, 0, sizeof( sin ) );
              sin.sin_family      = AF_INET;
              sin.sin_addr.s_addr = d->inet_addr;
              sin.sin_port        = htons( FingerPort );

              cfd = socket( AF_INET , SOCK_STREAM , 0 );

              if ( !( fp = fdopen( cfd, "r+") ) )
              {
                close( cfd );
                set_share_memory_text( slot , "�{���L�k�}�� fdopen�T" );
                exit( 0 );
              }

              setbuf( fp, ( char * ) 0 );
              cfd = fileno( fp );

              if ( connect( cfd, ( struct sockaddr *) & sin, sizeof( sin ) ) )
              {
                close( cfd );
                set_share_memory_text( slot, "�d�ߺݨS���}��d�ߪA�ȡT" );
                exit( 0 );
              }

              fprintf( fp, "%s\r\n" , d->remote );
              fflush( fp );

              for ( temp[0] = '\x0';fgets( line, MAX_STRING_LENGTH , fp ); )
              {
                if ( str_len( line ) + str_len( temp )
                  < MAX_STRING_LENGTH - 1 ) str_cat( temp, line );
              }

              set_share_memory_text( slot , temp );
              exit( 0 );
            }

            break;

          default:
            exit( 0 );
          }

          break;

        /* ����{����, �u�ݭn�O����{���X�ӿ��� */
        default:
          break;
        }
      }
    }

    break;

  case CON_GET_FINGER:

    if ( d->check_finger == FALSE )
    {
      sprintf( temp , "\b%c" , inflect[ d->inflect++ % 4 ] );
      write_to_descriptor( d, temp , 0 );
      break;
    }

    else
    {
      /* �e�X greeting ���T��, �åB��X�C�������A */
      {
        GREETING_DATA * pGreet;
        GREETING_DATA * zGreet;
        int             count;

        write_to_descriptor( d, "\b \n\r" , 0 );

        count  = 0;
        zGreet = 0;

        for ( pGreet = greeting_list; pGreet; pGreet = pGreet->next )
          if ( number_range( 0, count++ ) == 0 ) zGreet = pGreet;

        if ( zGreet )
        {
          write_to_descriptor( d, zGreet->text, 0 );
        }
        else
        {
          mudlog( LOG_DEBUG, "nanny: �䤣��i���e��." );
          write_to_descriptor( d, "�S���i���e��\n\r", 0 );
        }

        /* �L�X�C�������A */
        show_game_status( d );

        /* �}�l��J�򥻸�ƤF */
        write_to_descriptor( d , "�п�J�A���^��W�r �R" , 0 );
        d->inbuf[0]  = '\x0';
        d->incomm[0] = '\x0';
      }
    }

    d->connected = CON_GET_NAME;
    break;

  case CON_VERIFY:

    switch( d->file )
    {
    case FILE_CORRECT:
      write_to_descriptor( d, "\b���T"  , 0 );
      break;

    case FILE_NEW:
      write_to_descriptor( d, "\b�s���a", 0 );

      if ( NewLock )
      {
        write_to_descriptor( d,
          "\n\r�ثe�C���S����k�s�W���a, �еy��A��!\n\r", 0 );
        close_socket( d );
        RETURN_NULL();
      }

      break;

    default:

      write_to_descriptor( d, "\b�����T", 0 );

      mudlog( LOG_DEBUG, "nanny: %s �ɮפ����T.", d->username );

      if ( AutoBackup )
      {
        recover_file( d );
      }

      else
      {
        sprintf( temp, "\n\r�A���ɮפw�g�l���M�Ц� \e[1;32m%s\e[0m �i��"
          "���޲z�̱o���T\a\a\a\n\r", bbs_name );

        write_to_descriptor( d, temp, 0 );
      }

      mudlog( LOG_BADFILE, "%s �ɮ׷l��.", d->username );
      close_socket( d );
      RETURN_NULL();
    }

    fOld = load_char_obj( d, d->username );
    ch   = d->character;

    if ( IS_SET( ch->pcdata->suspect , PLR_COPYEQ ) && !IS_IMMORTAL( ch ) )
    {
      sprintf( temp, "\n\r\a\a\a�A�Q�ڵ��i�J�M�]���A�A�������˳ơC"
                     "�Ш� \e[1;32m%s\e[0m �h�����@�U�C\n\r", bbs_name );

      write_to_descriptor( d, temp, 0 );
      mudlog( LOG_SUSPECT, "%s �]�A�������˳ơM�T��s�u�T", d->username );
      mudlog( LOG_INFO , "%s �q %s �i��, ���L�Q�ڵ��i�J, �]���A�������˳�."
         , ch->name , d->host );

      close_socket( d );
      RETURN_NULL();
    }

    if ( multilogin
      && check_multi_login( d )
      && !is_server( d->host )
      && !IS_IMMORTAL( ch ) )
    {
      mudlog( LOG_INFO, "%s �h���s�u.", d->host );

      write_to_descriptor( d,
        "\n\r�t�εo�{�A����}���h���s�u�M�бq�O����}�s�u�M���¡T\n\r", 0 );
      close_socket( d );
      RETURN_NULL();
    }

    if ( ch->pcdata->deny != 0 && !IS_IMMORTAL( ch ) )
    {
      mudlog( LOG_INFO , "%s �q %s �i��, ���L�Q�ڵ��i�J."
        , ch->name , d->host );

      sprintf( temp, "\n\r\a\a\a�A�Q�ڵ��i�J�M�ЩM�t�κ޲z�̰Ӷq�a�M"
               "�άO�� \e[1;32m%s\e[0m �h�߰ݤ@�U�C\n\r" , bbs_name );

      write_to_descriptor( d, temp, 0 );
      close_socket( d );
      RETURN_NULL();
    }

    if ( ch->pcdata && ch->pcdata->dormancy > 0 )
    {
      if ( ch->pcdata->dormancy > time( NULL ) )
      {
        char days[MAX_INPUT_LENGTH];
        char buf[MAX_STRING_LENGTH];

        chinese_number( ( ch->pcdata->dormancy - time( NULL ) ) / ( 60 * 60 * 24 )
          , days );

        sprintf( buf, "�A�٦b�V�v�C�M�A�n%s�ѫ�~����ӳ�T\n\r", days );
        write_to_descriptor( d, buf, 0 );
        close_socket( d );
        RETURN_NULL();
      }

      else
      {
        write_to_descriptor( d, "�A�q�V�v���A�����F�L�ӡM�n�t��T\n\r", 0 );
        ch->pcdata->dormancy = 0;
      }
    }

    if ( check_reconnect( d, FALSE ) )
    {
      fOld = FILE_CORRECT;
    }

    else
    {
      if ( wizlock && !IS_IMMORTAL( ch ) )
      {
        mudlog( LOG_INFO , "%s �]����w�ӵL�k�s�u." , ch->name );
        write_to_buffer( d, "�藍�_�M�C���Y�N���s�}���M�еy��A�ӡC\n\r", 0 );
        close_socket( d );
        RETURN_NULL();
      }
    }

    if ( fOld == FILE_CORRECT )
    {
      /* �Ѫ��a */
      sprintf( buf, "�١T\e[1;32m%s\e[0m�M�A�W���� \e[1;32m%s\e[0m �s���i��\n\r"
                    "�п�J�A���K�X�R" , ch->cname , ch->pcdata->lasthost );
      write_to_buffer( d, buf , 0 );
      write_to_buffer( d, echo_off_str, 0 );
      d->connected = CON_GET_OLD_PASSWORD;
      RETURN_NULL();
    }

    else
    {
      /* �s���a */
      if ( check_playing( d, ch->name ) ) RETURN_NULL();

      sprintf( buf, "�o�O�A�n���W�r�ܡS %s (\e[1;32mY\e[0m/\e[1;32mN\e[0m) "
        , d->username );

      write_to_buffer( d, buf, 0 );
      d->connected = CON_CONFIRM_NEW_NAME;
      RETURN_NULL();
    }
    break;

  /* �ˬd�ɮ� */
  case CON_CHECK_FILE :

    if ( d->check_file == FALSE )
    {
      sprintf( temp , "\b%c" , inflect[ d->inflect++ % 4 ] );
      write_to_descriptor( d, temp , 0 );
      break;
    }

    else
    {
      d->connected = CON_VERIFY;
    }

    break;

  /* ���o�m�W */
  case CON_GET_NAME :

    if ( !argument[0] )
    {
      write_to_buffer( d , "�п�J�A���^��W�r �R" , 0 );
      d->error++;
      RETURN_NULL();
    }

    argument[0] = UPPER( argument[0] );

    str_cpy( buf, file_name( argument, SAVE_FILE ) );
    fOld = access( buf, R_OK ) ? FALSE : TRUE;

    if ( !check_parse_name( argument ) )
    {
      if ( fOld )
      {
        write_to_buffer( d, "�A���­^��W�٥i��P�ثe�u�W���s�ϰ�۽Ĭ�"
                            "�M�����s�u�T\n\r", 0 );
        close_socket( d );
        RETURN_NULL();
      }

      else
      {
        write_to_buffer( d ,
          "\e[1;31m�A���W�r���Q�t�Ωұ����M�Э��s��J�T\n\r\e[0m"
          "�п�J�A���m�W�R" , 0 );
      }

      d->error++;
      RETURN_NULL();
    }

    if ( check_logon( argument ) )
    {
      write_to_buffer( d, "���ۦP���W�٪����a�b��J�K�X�M�Э��s��J�T\n\r"
        "�п�J�A���m�W�R" , 0 );
      d->error++;
      RETURN_NULL();
    }

    /* �⪱�a���^��W�٫�����y�z��, ����|�ϥΨ� */
    argument[sizeof( d->username ) - 1 ] = '\x0';
    str_cpy( d->username , argument );

    /* �e�X�r�� */
    sprintf( buf , "�ˬd %s ���ɮ�..." , argument );
    write_to_buffer( d, buf , 0 );

    slot = get_free_share_memory( d, INTERNAL_CHECKFILE );

    if ( slot < 0 )
    {
      write_to_descriptor( d, "�藍�_�M�O���餣���M�L�k�s�u�C\n\r", 0 );
      close_socket( d );
      RETURN_NULL();
    }

    /* �Ѥl��{�Ӭd�ߪ��a���ɮ� */
    switch( fork() )
    {
    case -1:
      SystemCrash = TRUE;
      clean_share_memory_address( slot );
      mudlog( LOG_INFO , strerror( errno ) );
      mudlog( LOG_CRIT , "fork() ��Ʀ����D." );
      break;

    /* �l��{���� */
    case 0:

      SystemCrash = TRUE;

      /* �����Ҧ��q�T */
      close_communication();

      signal( SIGPIPE, SIG_IGN );
      signal( SIGCHLD, SIG_IGN );

      switch( fork() )
      {
      case -1 :

        SystemCrash = TRUE;
        clean_share_memory_address( slot );
        mudlog( LOG_INFO, strerror( errno ) );
        mudlog( LOG_CRIT, "fork() ��Ʀ����D." );
        exit( 0 );

      case 0:
        {
          char   buffer[MAX_INPUT_LENGTH];

          SystemCrash = TRUE;

          /* �����Ҧ��q�T */
          close_communication();

          signal( SIGPIPE, SIG_IGN );
          signal( SIGCHLD, SIG_IGN );

          sprintf( buffer, "%d", load_char_obj( d, d->username ) );
          set_share_memory_text( slot, buffer );
          exit( 0 );
        }
        break;

      default:
        exit( 0 );
      }
      break;

    /* ����{���� */
    default:
      break;
    }

    d->connected = CON_CHECK_FILE;
    break;

  /* ���o�ʧO */
  case CON_GET_SEX :

    switch ( argument[0] )
    {
    case 'm': case 'M': case '1':
      d->character->sex = SEX_MALE;
      break;

    case 'f': case 'F': case '2':
      d->character->sex = SEX_FEMALE;
      break;

    default:
      d->error++;
      write_to_buffer( d, "�п�J \e[1;32m1\e[0m(�k��) �� "
        "\e[1;32m2\e[0m(�k��)�C", 0 );

      RETURN_NULL();
    }

    write_to_buffer( d, "�п����X�Ҧ��R(\e[1;32m1\e[0m) Merc �Ҧ�"
      " (\e[1;32m2\e[0m) LP �Ҧ��R", 0 );

    d->connected = CON_CHOOSE_MODE;
    break;

  case CON_CHOOSE_MODE:

    switch ( argument[0] )
    {
    case '1':
      SET_BIT( ch->act, PLR_PROMPT );
      SET_BIT( ch->act, PLR_BLANK  );
      break;

    case '2':
      REMOVE_BIT( ch->act, PLR_PROMPT );
      REMOVE_BIT( ch->act, PLR_BLANK  );
      break;

    default:
      d->error++;
      write_to_buffer( d, "�п����X�Ҧ��R(\e[1;32m1\e[0m) Merc �Ҧ� "
        "(\e[1;32m2\e[0m) LP �Ҧ��R", 0 );

      RETURN_NULL();
    }

    write_to_buffer( d, "�п�ܦu�@���X�{�W�v�R(\e[1;32m1\e[0m) �g�`"
      " (\e[1;32m2\e[0m) ���q (\e[1;32m3\e[0m) �ܤ�"
      "(\e[1;32m4\e[0m) �q���R ", 0 );

    d->connected = CON_CHOOSE_ANGEL;
    break;

  case CON_CHOOSE_HOME:

    if ( !argument[0] || !is_number( argument ) )
    {
      show_home( d );
      RETURN_NULL();
    }

    slot = atoi( argument );
    loop = 1;

    for ( pArea = area_first; pArea; pArea = pArea->next )
    {
      if ( pArea->capital && loop++ == slot )
      {
        ch->home = pArea->serial;

        if ( ch->level > 0 )
        {
          lines               = ch->pcdata->pagelen;
          ch->pcdata->pagelen = 20;
          if ( IS_IMMORTAL( ch ) ) send_to_char( welcome_immortal, ch );
          send_to_char( welcome_message, ch );
          send_to_char( "\e[1;32m�Ы� \e[1;33mENTER\e[1;32m �~��C��\e[0m", ch );
          ch->pcdata->pagelen = lines;
          d->connected        = CON_READ_MOTD;
        }

        else
        {
          show_innate( d );
          d->connected = CON_CHOOSE_SKILL;
        }
        RETURN_NULL();
      }
    }

    show_home( d );
    break;

  case CON_CHOOSE_ANGEL:

    switch ( argument[0] )
    {
    case '1':
      ch->pcdata->angel_set =  AngelDefault / 2;
      SET_BIT( ch->act, PLR_ANGEL );
      break;

    case '2':
      ch->pcdata->angel_set =  AngelDefault;
      SET_BIT( ch->act, PLR_ANGEL );
      break;

    case '3':
      ch->pcdata->angel_set =  AngelDefault * 2;
      SET_BIT( ch->act, PLR_ANGEL );
      break;

    case '4':
      ch->pcdata->angel_set = AngelDefault * 5;
      REMOVE_BIT( ch->act, PLR_ANGEL );
      break;

    default:
      d->error++;
      write_to_buffer( d, "�п�ܦu�@���X�{�W�v�R(\e[1;32m1\e[0m) �g�` "
        "(\e[1;32m2\e[0m) ���q (\e[1;32m3\e[0m) �ܤ�"
        "(\e[1;32m4\e[0m) �q���R ", 0 );

      RETURN_NULL();
    }

    write_to_buffer( d, "�п�J�A������W�r�R" , 0 );
    d->connected = CON_GET_CNAME;
    break;

  case CON_GET_OLD_PASSWORD:

    write_to_buffer( d, "\n\r", 0 );

    if ( str_cmp( crypt( argument, ch->pcdata->pwd ), ch->pcdata->pwd ) )
    {
      DESCRIPTOR_DATA * man;
      char              address[MAX_STRING_LENGTH];

      str_cpy( address, host_name( d ) );

      /* �q���u�W���H, �Y�O�b�u�W */
      for ( man = descriptor_list; man; man = man->next )
      {
        if ( verify_desc( man )
          && man != d
          && man->connected == CON_PLAYING
          && man->character
          && !str_cmp( man->character->name, ch->name ) )
        {
          act( "$B$B$2���H���ϥH�A���W�ٱq$1$t$2�a��s�i�ӡM���O�K�X���~�T$0"
            , man->character, address, NULL, TO_CHAR );
          break;
        }
      }

      mudlog( LOG_FAILPASS , "%s(%s) �K�X���~.", ch->name, address );
      write_to_buffer( d, "�K�X���~�M�Э��s�i�J�C\n\r", 0 );

      /* �O����K�X���~�O���� */
      fwrite_failcode( ch, address );

      close_socket( d );
      RETURN_NULL();
    }

    if ( !check_limit_ip( d ) )
    {
      mudlog( LOG_INFO, "[Limit] %s ����s�u.", ch->name );
      write_to_buffer( d, "�藍�_�M�o�쪱�a����s�u��}�M"
        "���o�Ӧ�}�O�D�k���T\n\r", 0 );

      close_socket( d );
      RETURN_NULL();
    }

    write_to_buffer( d, echo_on_str, 0 );

    if ( d->lock_file )
    {
      mudlog( LOG_INFO, "%s �ɮ��ܰ�, �j���_�u.", ch->name );
      write_to_buffer( d, "�藍�_�M�A���ɮצ��ܰʡM�Э��s�i�J�C\n\r", 0 );
      close_socket( d );
      RETURN_NULL();
    }

    /* �M����J�w�İϪ���� */
    d->incomm[0] = '\x0';
    d->inbuf [0] = '\x0';

    if ( check_reconnect( d, TRUE ) )   RETURN_NULL();
    if ( check_playing( d, ch->name ) ) RETURN_NULL();

    mudlog( LOG_INFO , "%s �q %s �s�i��.", ch->name, host_name( d ) );

    if ( imm_list && !IS_IMMORTAL( ch ) && get_total_user() > max_connect )
    {
      mudlog( LOG_INFO , "%s �s���ɶW�L�W�u�H��." , ch->name );
      write_to_buffer( d, "�藍�_�M�ثe�s�u�H�ƤӦh�M�еy��A�s���C\n\r", 0 );
      close_socket( d );
      RETURN_NULL();
    }

    /* �ˬd�ݩʹL��, �]�tŪ�J���ݩʭ׹��� */
    if ( !IS_IMMORTAL( ch )
      && ( get_curr_str( ch ) > HUGE_ATTR
      || get_curr_int( ch ) > HUGE_ATTR
      || get_curr_wis( ch ) > HUGE_ATTR
      || get_curr_dex( ch ) > HUGE_ATTR
      || get_curr_con( ch ) > HUGE_ATTR ) )
    {
      write_to_buffer( d, "�A�������ݩʦ����D, �и�t�κ޲z���p��, ��"
        "�w��g�H�� lc@mud.ch.fju.edu.tw, ����!.", 0 );
      close_socket( d );
      RETURN_NULL();
    }

    if ( get_home( ch ) )
    {
      lines               = ch->pcdata->pagelen;
      ch->pcdata->pagelen = 20;
      if ( IS_IMMORTAL( ch ) ) send_to_char( welcome_immortal, ch );

      send_to_char( welcome_message, ch );
      send_to_char( "\e[1;32m�Ы� \e[1;33mENTER\e[1;32m �~��C��\e[0m", ch );

      ch->pcdata->pagelen = lines;
      d->connected        = CON_READ_MOTD;
    }

    else
    {
      show_home( d );
      d->connected = CON_CHOOSE_HOME;
    }

    break;

  case CON_CONFIRM_NEW_NAME:

    switch ( *argument )
    {
    int now_connect;

    case 'y': case 'Y':

      if ( imm_list && ( now_connect = get_total_user() ) > max_connect )
      {
        mudlog( LOG_INFO , "�s���a %s �s���ɶW�L�W�u�H��." , ch->name );
        write_to_buffer( d, "�s�u�H�ƤӦh�M�еy��A�s���C\n\r", 0 );
        close_socket( d );
        RETURN_NULL();
      }

      if ( !imm_list )
      {
        sprintf( buf, "\a\a\a\e[1;32m�A�O�Ĥ@�ӷs���a�M�A�N�Q���R���W��"
          "�޲z�̡M�Фp�ߦa�� %s �@�ӱK�X�R\e[0m %s", ch->name, echo_off_str );
      }

      else
      {
        sprintf( buf, "�A�O�@�ӷs���a�M�е� %s �@�ӱK�X�R %s"
          , ch->name, echo_off_str );
      }

      write_to_buffer( d, buf, 0 );
      d->connected = CON_GET_NEW_PASSWORD;
      break;

    case 'n': case 'N':

      write_to_buffer( d, "�n�a�T�аݧA���W�r�O����S", 0 );
      free_char( d->character );
      d->character = NULL;
      d->connected = CON_GET_NAME;
      d->error++;
      break;

    default:
      write_to_buffer( d, "�п�J [\e[1;32mY\e[0m] �άO [\e[1;32mN\e[0m]�S", 0 );
      d->error++;
      break;
    }
    break;

  case CON_GET_NEW_PASSWORD:

    write_to_buffer( d, "\n\r", 0 );

    if ( !check_password( argument ) )
    {
      const char * pMessage;

      pMessage = ( strict_password )
        ? "�A���K�X��²��F�M�г]�w�]�[�Ʀr�M�r���B���Ӧr�H�W���K�X�T"
          "\n\r�п�J�K�X�R"
        : "�A���K�X��²��F�M�г]�w���Ӧr�H�W���K�X�T\n\r�п�J�K�X�R";

      write_to_buffer( d, pMessage, 0 );

      d->error++;
      RETURN_NULL();
    }

    pwdnew = crypt( argument, ch->name );
    for ( p = pwdnew; *p; p++ )
    {
      if ( *p == '~' )
      {
        write_to_buffer( d, "�A���K�X���X�W�w�C\n\r�п�J�K�X�R", 0 );
        RETURN_NULL();
      }
    }

    free_string( ch->pcdata->pwd );
    ch->pcdata->pwd = str_dup( pwdnew );
    write_to_buffer( d, "�Э��s��J�K�X�ӽT�{�R", 0 );
    d->connected = CON_CONFIRM_NEW_PASSWORD;
    break;

  case CON_GET_CNAME:

    if ( str_len( argument ) > MaxCNameLen )
    {
      write_to_buffer( d, "�A������W�r�Ӫ��F, �Э��s��J�A������W�r: ", 0 );
      d->error++;
      RETURN_NULL();
    }

    if ( str_len( argument ) < MinCNameLen )
    {
      write_to_buffer( d, "�A������W�r�ӵu�F�M�Э��s��J�A������W�r�R", 0 );
      d->error++;
      RETURN_NULL();
    }

    if ( ( *argument < '\x81' || *argument > '\xFF' ) && *argument != '\e' )
    {
      write_to_buffer( d, "�o���O�@�Ӥ���W�r�M�Э��s��J�A������W�r�R", 0 );
      d->error++;
      RETURN_NULL();
    }

    if ( check_cname_xname( argument ) )
    {
      write_to_buffer( d, "�A���ள�o�Ӱ��W�r�M�Э��s��J�A������W�r�R", 0 );
      d->error++;
      RETURN_NULL();
    }

    filter_ansi( argument );
    smash_tilde( argument );
    fix_color( argument );
    free_string( ch->cname );
    ch->cname = str_dup( argument );
    ch->pcdata->pagelen = 22;
    ch->prompt =
      str_dup( "< �ͩR�O^hp1 �k�O^mp1 ��O^mv1 ����^gol �g���^exp >" );

    if ( aInnate >= nInnate || aInnate <= 0 )
    {
      if ( aInnate > 0 )
      {
        for ( slot = 0; slot < MAX_INNATE; slot++ )
        {
          if ( iInnate[slot] > 0 ) d->select[slot] = TRUE;
          else break;
        }

        set_innate( d );
      }

      print_to_char( ch , "\n\r\n\r%s\e[1;32m�Ы� ENTER �~��C���R\e[0m"
        , welcome_message );

      d->connected = CON_READ_MOTD;
    }

    else
    {
      show_home( d );
      d->connected = CON_CHOOSE_HOME;
    }

    break;

  case CON_CONTRACT:
    print_to_char( ch , "\n\r%s\e[1;32m�Ы� ENTER �~��C���R\e[0m"
      , welcome_message );

    set_innate( d );
    d->connected = CON_READ_MOTD;
    break;

  case CON_CHOOSE_SKILL:

    argument = one_argument( argument, selection );

    if ( !selection[0] )
    {
      show_innate( d );
    }

    else if ( !str_prefix( selection, "select" ) )
    {
      one_argument( argument, selection );

      if ( !selection[0] )
      {
        write_to_buffer( d, "�A����������墨�@�ӡM�Ҧp�Rselect 1�C"
          "�Ы� \e[1;32mENTER\e[0m �~���� ", 0 );

        break;
      }

      if ( !is_number( selection )
        || ( slot = atoi( selection ) ) <= 0
        || slot > nInnate )
      {
        sprintf( buf, "�A���ﶵ�����O�Ʀr�M�B�b 1 �� %d �����C�Ҧp�Rselect 2"
          "�C�Ы� \e[1;32mENTER\e[0m �~���� ", nInnate );

        write_to_buffer( d, buf, 0 );
        break;
      }

      if ( d->select[--slot] == FALSE && get_innate( d ) >= aInnate )
      {
        write_to_buffer( d, "�A��������ޯ�W�X�A�i�H�諸�`�ơC"
          "�Ы� \e[1;32mENTER\e[0m �~���� ", 0 );

        break;
      }

      d->select[slot] = d->select[slot] ? FALSE : TRUE;
      show_innate( d );
    }

    else if ( !str_prefix( selection, "query" ) )
    {
      one_argument( argument, selection );

      if ( !selection[0] )
      {
        write_to_buffer( d, "�A���������d�ߨ��@�ӧޯ�M�Ҧp�Rquery 1�C"
          "�Ы� \e[1;32mENTER\e[0m �~���� ", 0 );
        break;
      }

      if ( !is_number( selection )
        || ( slot = atoi( selection ) ) <= 0
        || slot > nInnate )
      {
        sprintf( buf, "�A���ﶵ�����O�Ʀr�M�B�b 1 �� %d �����C�Ҧp�Rquery 1�C"
          "�Ы� \e[1;32mENTER\e[0m �~���� ", nInnate );

        write_to_buffer( d, buf, 0 );
        break;
      }

      if ( !( pSkill = get_skill( iInnate[slot-1] ) )
        || !pSkill->help
        || !*pSkill->help )
      {
        write_to_buffer( d, "�藍�_�M�o�ӧޯ�S���������T"
          "�Ы� \e[1;32mENTER\e[0m �~���� ", 0 );
        break;
      }

      write_to_buffer( d, pSkill->help, 0 );
      write_to_buffer( d, "�Ы� \e[1;32mENTER\e[0m �~���� ", 0 );
    }

    else if ( !str_prefix( selection, "help" ) )
    {
    write_to_descriptor( d,
      "\n\r    �ثe�A���b����@�ǰ򥻪��ޯ�M�]�N�O�@�}�l�i�J�C���e�N�|���򥻥\\�ҡC\n\r"
      "���ܦC�|��ܥثe�A�i�H���X�ӡM�٦��A�w�g��F�X�ӡT�@�}�l�A�i�H��J\n\r"
      "query 1 �Ӭd�ߤ@���ޯ઺������ơM�A�i�H�~��d�ߨ�L�ޯ઺���p�M�쩳�A��\n\r"
      "�A�X�ۤv�ǲߡC��A�w�g�M�w�n���ɡM���w�A�n���w�ĤG���ޯ�M�A�i�H��J\n\r"
      "select 2�C����M�ĤG���ޯ���䪺���ӫK�|���ӰO���M�N��A�w�g���L�F�M\n\r"
      "�p�G�A�ᮬ�M�A�i�H�A�@����J select 2 �M�h�o�ӰO���K�|�����C��A���n�F\n\r"
      "�M�A�h�i�H��J done �M�N��w�g�������M���O�A��諸�ƥؤ���֩����Ӱ��\n\r"
      "���ƥءM�_�h�t�η|�ЧA�b�Ҽ{����L�ޯ�C�H�U�O�o�ǫ��O��²�满���R\n\r\n\r"
      "  1�B\e[1;35mselect\e[0m (���X) ���ή����򥻧ޯ�\n\r"
      "  2�B\e[1;35mquery\e[0m  (���X) �d�߬Y�ӧޯ઺��T\n\r"
      "  3�B\e[1;35mclear\e[0m         �M�����e���]�w\n\r"
      "  4�B\e[1;35mhelp\e[0m          �ثe���D�U�e��\n\r"
      "  5�B\e[1;35mdone\e[0m          ������諸�ʧ@\n\r\n\r"
      "�Ы� \e[1;32mENTER\e[0m �~���� ", 0 );
    }

    else if ( !str_prefix( selection, "done" ) )
    {
      if ( get_innate( d ) < aInnate )
      {
        write_to_buffer( d, "�A�|���I�ƥi�H���M�Ф��n���T"
          "�Ы� \e[1;32mENTER\e[0m �~���� ", 0 );
        break;
      }

      write_to_buffer( d, "�Ы��������~��T", 0 );
      d->connected = CON_CONTRACT;
    }

    else if ( !str_prefix( selection, "clear" ) )
    {
      for ( slot = 0; slot < MAX_INNATE; slot++ ) d->select[slot] = FALSE;
      show_innate( d );
    }

    else
    {
      write_to_buffer( d, "�L�Ī����O�T�Ы� \e[1;32mENTER\e[0m �~���� ", 0 );
    }

    break;

  case CON_CONFIRM_NEW_PASSWORD:

    write_to_buffer( d, "\n\r", 0 );
    if ( str_cmp( crypt( argument, ch->pcdata->pwd ), ch->pcdata->pwd ) )
    {
      write_to_buffer( d, "�K�X���X�C\n\r�Э��s��J�A���K�X�R", 0 );
      d->connected = CON_GET_NEW_PASSWORD;
      d->error++;
      RETURN_NULL();
    }

    write_to_buffer( d, echo_on_str , 0 );
    write_to_buffer( d, "�п�J�A���q�l�l��H�c�R", 0 );

    d->connected = CON_GET_EMAIL_ADDRESS;
    d->incomm[0] = '\x0';
    d->inbuf [0] = '\x0';
    break;

  case CON_GET_EMAIL_ADDRESS:

    if ( str_len( argument ) > 50 )
    {
      write_to_buffer( d, "�A���H�c�W�٤Ӫ��F�M�Э��s��J�A���q�l�H�c�R", 0 );
      d->error++;
      RETURN_NULL();
    }

    if ( str_len( argument ) < 10 )
    {
      write_to_buffer( d, "�A���q�l�H�c�W�٤ӵu�F�M�Э��s��J�A���q�l�H�c�R", 0 );
      d->error++;
      RETURN_NULL();
    }

    if ( !check_email_address( argument ) )
    {
      write_to_buffer( d, "�A���q�l�H�c�W�٤��X�k�M�Э��s��J�A���q�l�H�c�R", 0 );
      d->error++;
      RETURN_NULL();
    }

    smash_tilde( argument );
    free_string( ch->email);
    ch->email = str_dup( argument );

    write_to_buffer( d, "�п�ܧA���ʧO \e[1;32m1\e[0m(�k��)"
      " \e[1;32m2\e[0m(�k��) ", 0 );

    d->connected = CON_GET_SEX;
    d->incomm[0] = '\x0';
    d->inbuf [0] = '\x0';
    break;

  case CON_READ_MOTD:

    if ( ch->level > MAX_LEVEL )
    {
      mudlog( LOG_INFO, "�ץ� %s ���� %d -> %d."
        , ch->name, ch->level, MAX_LEVEL );
      ch->level = MAX_LEVEL;
    }

    d->connected = CON_PLAYING;

    act( "$A$2�w��A�Ө�$5$t$2�M���§A������M�ýЦh�h���{�T$0$A"
      , ch, mud_name, NULL, TO_CHAR );

    /* �e�X������� */
    if ( motd_message && *motd_message )
    {
      send_to_char(
        "\n\r--------------------------------- \e[1;32m�������\e[0m "
        "---------------------------------\n\r", ch );

      send_to_char( motd_message, ch );

      send_to_char(
        "-----------------------------------------------"
        "-----------------------------\n\r\n\r", ch );
    }


    if ( ch->level <= 0 )
    {
      OBJ_DATA * obj;
      int        loop;

      if ( ch->pcdata )
      {
        for ( loop = 0; loop < MAX_RECALL; loop++ )
        {
          if ( RecallPlace[loop] > 0 )
            ch->pcdata->recall[loop] = RecallPlace[loop];
        }
      }

      for ( loop = 0; loop < MAX_SKILL; loop++ )
      {
        if ( ch->skill[loop] > 0
          && ( pSkill = get_skill( loop ) )
          && pSkill->enable )
        {
          set_enable( ch, pSkill );
        }
      }

      /* ���Ť@�����ݩ� */
      ch->level   = 1;
      ch->now_str = DefaultStr;
      ch->now_int = DefaultInt;
      ch->now_wis = DefaultWis;
      ch->now_dex = DefaultDex;
      ch->now_con = DefaultCon;

      /* ���Ť@�������ƥ� */
      gold_to_char( ch, DefaultGold );
      ch->hit     = get_curr_hit( ch );
      ch->mana    = get_curr_mana( ch );
      ch->move    = get_curr_move( ch );

      /* �]�w���Y */
      ch->class = class_demos;
      str_cpy( buf,  ch->class->title );
      set_title( ch, buf );

      if ( ch ) char_to_room( ch, RoomSchool );

      for ( loop = 0; loop < MAX_DEFAULT_OBJECT; loop++ )
      {
        if ( DefaultObject[loop] <= 0
          || !( pObjIndex = get_obj_index( DefaultObject[loop] ) ) )
          break;

        if ( ( obj = create_object( pObjIndex, -1 ) ) )
        {
          obj_to_char( obj, ch );
          wear_obj( ch, obj, FALSE, FALSE );
        }
      }

      mudlog( LOG_NEWPLAYER , "%s �q %s �s�i�Ӫ��s���a.", ch->name, d->host );

      /*  �]�w�U�W�D */
      if ( !ChannelAuction  ) SET_BIT( ch->deaf, CHANNEL_AUCTION  );
      if ( !ChannelChat     ) SET_BIT( ch->deaf, CHANNEL_CHAT     );
      if ( !ChannelBulletin ) SET_BIT( ch->deaf, CHANNEL_BULLETIN );
      if ( !ChannelImmtalk  ) SET_BIT( ch->deaf, CHANNEL_IMMTALK  );
      if ( !ChannelMusic    ) SET_BIT( ch->deaf, CHANNEL_MUSIC    );
      if ( !ChannelQuestion ) SET_BIT( ch->deaf, CHANNEL_QUESTION );
      if ( !ChannelShout    ) SET_BIT( ch->deaf, CHANNEL_SHOUT    );
      if ( !ChannelYell     ) SET_BIT( ch->deaf, CHANNEL_YELL     );
      if ( !ChannelGamble   ) SET_BIT( ch->deaf, CHANNEL_GAMBLE   );
      if ( !ChannelClass    ) SET_BIT( ch->deaf, CHANNEL_CLASS    );
      if ( !ChannelClub     ) SET_BIT( ch->deaf, CHANNEL_CLUB     );
      if ( !ChannelSemote   ) SET_BIT( ch->deaf, CHANNEL_SEMOTE   );
      if ( !ChannelWeather  ) SET_BIT( ch->deaf, CHANNEL_WEATHER  );
      if ( !ChannelPhone    ) SET_BIT( ch->deaf, CHANNEL_PHONE    );
      if ( !ChannelSuicide  ) SET_BIT( ch->deaf, CHANNEL_SUICIDE  );
      if ( !ChannelRumor    ) SET_BIT( ch->deaf, CHANNEL_RUMOR    );
      if ( !ChannelNotice   ) SET_BIT( ch->deaf, CHANNEL_NOTICE   );
      if ( !ChannelGroup    ) SET_BIT( ch->deaf, CHANNEL_GROUP    );
      if ( !ChannelPK       ) SET_BIT( ch->deaf, CHANNEL_PK       );

      /* �]�w�U�պA */
      ch->act    = 0;
      ch->turn   = 0;
      ch->notify = 0;

      if ( ConfigAutoExit          ) SET_BIT( ch->act , PLR_AUTOEXIT  );
      if ( ConfigAutoLoot          ) SET_BIT( ch->act , PLR_AUTOLOOT  );
      if ( ConfigAutoSac           ) SET_BIT( ch->act , PLR_AUTOSAC   );
      if ( ConfigBlank             ) SET_BIT( ch->act , PLR_BLANK     );
      if ( ConfigBrief             ) SET_BIT( ch->act , PLR_BRIEF     );
      if ( ConfigCombine           ) SET_BIT( ch->act , PLR_COMBINE   );
      if ( ConfigPrompt            ) SET_BIT( ch->act , PLR_PROMPT    );
      if ( ConfigExact             ) SET_BIT( ch->act , PLR_EXACT     );
      if ( ConfigMessage           ) SET_BIT( ch->act , PLR_MESSAGE   );
      if ( ConfigFlee              ) SET_BIT( ch->act , PLR_FLEE      );
      if ( ConfigAngel             ) SET_BIT( ch->act , PLR_ANGEL     );
      if ( ConfigAutoFood          ) SET_BIT( ch->act , PLR_AUTOFOOD  );
      if ( ConfigAutoDrink         ) SET_BIT( ch->act , PLR_AUTODRINK );
      if ( ConfigRebirth           ) SET_BIT( ch->turn, PLR_REBIRTH   );
      if ( ConfigTrain             ) SET_BIT( ch->turn, PLR_TRAIN     );
      if ( ConfigPractice          ) SET_BIT( ch->turn, PLR_PRACTICE  );
      if ( ConfigAnsi              ) SET_BIT( ch->turn, PLR_ANSI      );
      if ( ConfigLotto             ) SET_BIT( ch->turn, PLR_LOTTO     );

      if ( !NotifyDebug     ) SET_BIT( ch->notify, NOTIFY_DEBUG      );
      if ( !NotifyInfo      ) SET_BIT( ch->notify, NOTIFY_INFO       );
      if ( !NotifyWizard    ) SET_BIT( ch->notify, NOTIFY_WIZARD     );
      if ( !NotifyNewplayer ) SET_BIT( ch->notify, NOTIFY_NEWPLAYER  );
      if ( !NotifyFailpass  ) SET_BIT( ch->notify, NOTIFY_FAILPASS   );
      if ( !NotifyEmerg     ) SET_BIT( ch->notify, NOTIFY_EMERG      );
      if ( !NotifyCrit      ) SET_BIT( ch->notify, NOTIFY_CRIT       );
      if ( !NotifyErr       ) SET_BIT( ch->notify, NOTIFY_ERR        );
      if ( !NotifyXname     ) SET_BIT( ch->notify, NOTIFY_XNAME      );
      if ( !NotifyNet       ) SET_BIT( ch->notify, NOTIFY_NET        );

      /* �O�_���Ĥ@�Ӫ��a�M�O���ܵ��L��J���j�� */
      if ( !imm_list )
      {
        IMMLIST_DATA * pImm;
        FILE         * pFile;

        if ( ( pFile = FOPEN( immlist_file, "w" ) ) )
        {
          fprintf( pFile, "#Immortal\n"                      );
          fprintf( pFile, "Name            %s~\n", ch->name  );
          fprintf( pFile, "Level           %d\n" , MAX_LEVEL );
          fprintf( pFile, "Adviser         %s\n", symbol_string( FALSE, "�޿�`��" ) );
          fprintf( pFile, "Description\n~\nEnd\n\nEnd"       );
          FCLOSE( pFile );

          set_immlist_default( pImm = alloc_struct( STRUCT_IMMLIST_DATA ) );

          pImm->name        = str_dup( ch->name );
          pImm->description = str_dup( "" );
          pImm->level       = MAX_LEVEL;
          pImm->next        = NULL;
          imm_list          = pImm;

          ch->level = MAX_LEVEL;

          act( "$B$B$B$A$A$2�A�O�Ĥ@�Ӷi�J$t���H�M�ҥH�A�Q���R���j���C$A$0"
            , ch, mud_name, NULL, TO_CHAR );
        }

        else
        {
          mudlog( LOG_DEBUG, "�L�k�}�ү����ɮ�." );
        }
      }
    }

    else if ( ch && ch->in_room )
    {
      if ( ch->in_room->NoQuit )
      {
        send_to_char( "�A�W���h���a��n�������F�C\n\r", ch );
        char_to_room( ch, get_hometown( ch ) );
      }

      else
      {
        char_to_room( ch, ch->in_room );
      }
    }

    else if ( ch && IS_IMMORTAL( ch ) )
    {
      char_to_room( ch, RoomChat );
    }

    else
    {
      char_to_room( ch, get_hometown( ch ) );
    }

    if ( ch && ch->jail > 0 )
    {
      if ( ch->in_room != RoomJail )
      {
        char_from_room( ch );
        char_to_room( ch, RoomJail );

        /* �M���l�ܬ����I */
        clear_trace( ch, TRUE );
      }
    }

    if ( ch && ch->failed > 0 && ch->jail <= 0 )
    {
      if ( ch->in_room != RoomFail )
      {
        char_from_room( ch );
        char_to_room( ch, RoomFail );

        /* �M���l�ܬ����I */
        clear_trace( ch, TRUE );
      }
    }

    if ( ch && ch->jail <= 0 && ch->in_room == RoomJail )
    {
      char_from_room( ch );
      char_to_room( ch, get_hometown( ch ) );

      /* �M���l�ܬ����I */
      clear_trace( ch, TRUE );
    }

    /* �Y�w������L�M�٦b��L�V */
    if ( ch && ch->failed <= 0 && ch->in_room == RoomFail )
    {
      char_from_room( ch );
      char_to_room( ch, get_hometown( ch ) );

      /* �M���l�ܬ����I */
      clear_trace( ch, TRUE );
    }

    /* �ˬd���~�O�_���T */
    check_null_object( ch );

    /* �ˬd���~�Ǹ��O�_���� */
    check_serial_number( ch );

    /* ���o���� */
    char_attach_club( ch );

    /* �ץ����� */
    if ( ch->level >= LEVEL_IMMORTAL ) ch->level = LEVEL_HERO;
    if ( ch->trust >= LEVEL_IMMORTAL ) ch->trust = LEVEL_HERO;

    ch->adviser = FALSE;
    ch->trust   = 0;

    if ( ( pImmlist = imm_lookup( ch->name ) ) )
    {
      if ( pImmlist->level   > 0     ) ch->level   = pImmlist->level;
      if ( pImmlist->trust   > 0     ) ch->trust   = pImmlist->trust;
      if ( pImmlist->adviser == TRUE ) ch->adviser = TRUE;
    }

    ch->bank = UMAX( 0, ch->bank  );

    /* �ץ��S������M�����A���` */
    if ( ch->position == POS_DEAD && ch->pcdata && !ch->pcdata->corpse )
    {
      ch->position = POS_STANDING;
      char_from_room( ch );
      char_to_room( ch, get_hometown( ch ) );

      /* �M���l�ܬ����I */
      clear_trace( ch, TRUE );
    }

    /* �]�w���a�Ǹ� */
    set_serial( &ch->serial );

    /* �إ߸�Ʈw */
    set_database( ch );

    act( "$n �����r���q�Ŧӭ��M�{�p�ѯ��T", ch, NULL, NULL, TO_ROOM );
    do_look( ch, "auto" );

    /* �ˬd�X�ͦa */
    get_home( ch );

    /* �ץ��ݩ� */
    while ( get_curr_hit( ch  ) > MaxHit  ) ch->max_hit--;
    while ( get_curr_mana( ch ) > MaxMana ) ch->max_mana--;
    while ( get_curr_move( ch ) > MaxMove ) ch->max_move--;

    while ( get_curr_str( ch ) > MaxStr ) ch->now_str--;
    while ( get_curr_int( ch ) > MaxInt ) ch->now_int--;
    while ( get_curr_wis( ch ) > MaxWis ) ch->now_wis--;
    while ( get_curr_dex( ch ) > MaxDex ) ch->now_dex--;
    while ( get_curr_con( ch ) > MaxCon ) ch->now_con--;

    ch->firman      = UMIN( MaxFirman  , ch->firman      );
    ch->nskill      = UMIN( MaxNSkill  , ch->nskill      );
    ch->practice    = UMIN( MaxPractice, ch->practice    );
    ch->pcdata->ply = UMIN( PlyQuota   , ch->pcdata->ply );

    /* �ץ��ޯ� */
    for ( notes = 0; notes < MAX_SKILL; notes++ )
    {
      if ( ch->skill[notes] > 0 && ( pSkill = get_skill( notes ) ) )
      {
        for ( pRest = pSkill->restrict; pRest; pRest = pRest->next )
        {
          switch( pRest->type )
          {
          default:
            continue;

          case RES_CLASS:
          case RES_TRUST:
          case RES_LEVEL:
          case RES_SEX:
          case RES_NOSKILL:

            if ( !check_restrict( ch, pRest, FALSE ) && !IS_IMMORTAL( ch ) )
            {
              cancel_skill( ch, pSkill );

              act( "$B$B�A�w�g�����a���$1$w$0�o���ޯ�F�T"
                , ch, pSkill, NULL, TO_CHAR );
            }
          }
        }
      }
    }

    for ( notes = 0, pnote = note_list; pnote; pnote = pnote->next )
      if ( is_note_to( ch, pnote ) && str_cmp( ch->name, pnote->sender )
        && pnote->date_stamp > ch->last_note ) notes++;

    if ( notes > 0 )
      act( "$3�A��$i�ʫH�|��Ū���M�Х� �u$7note$3�v ���OŪ���C$0$B$B"
        , ch, &notes, NULL, TO_CHAR );

    if ( ( notes = vote_count( ch ) ) > 0 )
      act( "�A�|��$i�����S����M�ХΡu$6vote$0�v�Ӷi��벼�T"
        , ch, &notes, NULL, TO_CHAR );

    if ( IS_SET( ch->act, PLR_WIZINVIS ) )
    {
      send_to_char( "\e[1;32m�N�M�A����������T\e[0m\n\r", ch );
    }
    /* �e�X�n�ͦW�� */
    else
    {
      friend_msg( ch );
    }

    if ( IS_IMMORTAL( ch ) )
    {
      immortal_on_line( ch );
      loading_warning( ch );
    }

    if ( ch->jail > 0 )
      send_to_char( "\e[1;32m�A���b�A�D���T\e[0m\n\r", ch );

    else if ( ch->failed > 0 )
      send_to_char( "\e[1;32m�A���b��L���T\e[0m\n\r", ch );

    /* �]�w�۰ʦs�ɩM�ƥ��ɶ� */
    ch->pcdata->autobackup_tick = ch->pcdata->autobackup;
    ch->pcdata->autosave_tick   = ch->pcdata->autosave;

    /* �T�w�t�� */
    check_mate( ch );

    /* �ˬd�������q */
    fix_gold_weight( ch );

    if ( over_scale( ch ) )
      send_to_char( "\e[1;31m�A���`�겣�w�F�W���T�T�T\e[0m\n\r", ch );

    /* ���v�Ѳ� */
    if ( ch->pcdata )
    {
      if ( StockBote > 0 ) bote_stock( ch );
      stock_value( ch );
    }

    /* ���s�]�w���a�� armor */
    calculate_armor( ch );

    /* ��ܤ��e�i�J���~�K�X */
    if ( ch->failcode )
    {
      char filename[MAX_FILE_LENGTH];

      show_failcode( ch );
      extract_failcode( ch );

      /* �R���ɮ� */
      str_cpy( filename, file_name( ch->name, FAILCODE_FILE ) );
      unlink( filename );
    }

    if ( !ch->email || !*ch->email )
    {
      free_string( ch->email );
      ch->email = str_dup( DefaultEmail );
    }

    break;
  }

  RETURN_NULL();
}

/* �ˬd ID �O�_�ŦX�W�w */
bool check_parse_name( char * name )
{
  XNAMES_DATA    * xnames;
  char           * pc;
  MOB_INDEX_DATA * pMob;
  int              iHash;

  PUSH_FUNCTION( "check_parse_name" );

  /* �O�d�r */
  for ( xnames = xnames_first; xnames; xnames = xnames->next )
    if ( ( xnames->type == XNAMES_SYSTEM || xnames->type == XNAMES_CACONYM )
      &&  !str_cmp( xnames->word , name ) ) RETURN( FALSE );

  /* ���׭��� */
  if ( str_len(name) <  3 || str_len( name ) > NAME_LENGTH ) RETURN( FALSE );

  /* �u���^��M�Ʀr�i�H��@�W�r */
  for ( pc = name; *pc; pc++ ) if ( !isalpha( ( int ) *pc ) ) RETURN( FALSE );

  /* �w�����a�M MOB �ۦP ID */
  for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
  {
    for ( pMob = mob_index_hash[iHash]; pMob; pMob = pMob->next )
      if ( is_fullname( name, pMob->name ) ) RETURN( FALSE );
  }

  RETURN( TRUE );
}

/* �ˬd�O�_���s�s�u */
bool check_reconnect( DESCRIPTOR_DATA * d, bool fConn )
{
  CHAR_DATA * ch;

  PUSH_FUNCTION( "check_reconnect" );

  if ( !d )
  {
    mudlog( LOG_DEBUG, "check_reconnect: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  for ( ch = char_list; ch; ch = ch->next )
  {
    if ( !verify_char( ch ) ) continue;

    if ( !IS_NPC( ch )
      && ( !fConn || !ch->desc )
      && !str_cmp( d->character->name, ch->name ) )
    {
      if ( fConn == FALSE )
      {
        free_string( d->character->pcdata->pwd );
        d->character->pcdata->pwd = str_dup( ch->pcdata->pwd );
      }

      else
      {
        free_char( d->character );
        d->character = ch;
        ch->desc     = d;
        ch->timer    = 0;

        /* �ץ����`���O�S�����骺���p */
        if ( ch->position == POS_DEAD && ch->pcdata && !ch->pcdata->corpse )
        {
          ch->position = POS_STANDING;
          char_from_room( ch );
          char_to_room( ch, get_hometown( ch ) );

          /* �M���l�ܬ����I */
          clear_trace( ch, TRUE );
        }

        send_to_char( "�A�^�_�F��ı�T\n\r", ch );
        act( "$n�^�_�F��ı�T", ch, NULL, NULL, TO_ROOM );

        /* �ˬd�������q */
        fix_gold_weight( ch );

        mudlog( LOG_INFO , "%s �q %s ���s�s�u .", ch->name, d->host );

        d->connected = CON_PLAYING;
        d->inbuf[0]  = '\x0';
        d->incomm[0] = '\x0';

        /* ���s�ˬd�˳ƪk�N�ݩ� */
        affect_login_check( ch );

        /* �����յ��c��� */
        extract_gamble( ch );

        light_adjust( ch , get_eq_char( ch , WEAR_LIGHT ) , TRUE );
      }
      RETURN( TRUE );
    }
  }
  RETURN( FALSE );
}

bool check_logon( char * name )
{
  DESCRIPTOR_DATA * man;

  PUSH_FUNCTION( "check_logon" );

  for ( man = descriptor_list; man; man = man->next )
  {
    if ( !verify_desc( man ) ) continue;

    if ( man->connected == CON_GET_OLD_PASSWORD
      && !str_cmp( name, man->username ) )
      RETURN( TRUE );
  }
  RETURN( FALSE );
}

/* �ˬd�O�_�w�g�b�C���� */
bool check_playing( DESCRIPTOR_DATA * d, char * name )
{
  DESCRIPTOR_DATA * dold;

  PUSH_FUNCTION( "check_playing" );

  for ( dold = descriptor_list; dold; dold = dold->next )
  {
    if ( !verify_desc( dold ) ) continue;

    if ( dold != d
      && dold->character
      && dold->connected != CON_GET_NAME
      && dold->connected != CON_GET_OLD_PASSWORD
      && !str_cmp( name, dold->character->name ) )
    {
      write_to_buffer( d, "�w�g���ۦP�W�r�����a�b�C�����F�M"
                          "�Э��s��J�A���W�r\n\r�A���W�r�R", 0 );

      d->username[0] = '\x0';

      str_cpy( dold->incomm, "quit\0" );
      if ( dold->edit_mode != EDIT_NONE && dold->connected == CON_PLAYING )
      {
        dold->edit_mode = EDIT_NONE;
        free_string( dold->character->stack );
        dold->character->stack = str_dup( "" );
        process_stack( dold );
      }

      write_to_buffer( dold , "\n\r�ۦPID�i�J�M�j���_�u�C\n\r" , 0 );

      d->connected = CON_GET_NAME;
      if ( d->character )
      {
        free_char( d->character );
        d->character = NULL;
      }

      RETURN( TRUE );
    }
  }
  RETURN( FALSE );
}

/* ���� idle */
void stop_idling( CHAR_DATA * ch )
{
  PUSH_FUNCTION( "stop_idling" );

  if ( !ch
    || !ch->desc
    || ch->desc->connected != CON_PLAYING
    || !ch->was_in_room
    || ch->in_room != RoomLimbo )
    RETURN_NULL();

  ch->timer = 0;
  char_from_room( ch );
  char_to_room( ch, ch->was_in_room );
  ch->was_in_room = NULL;
  act( "$n�q��L���Ҧ^�_�L�ӡT", ch, NULL, NULL, TO_ROOM );

  /* �M���l�ܬ����I */
  clear_trace( ch, TRUE );

  RETURN_NULL();
}

void send_to_all_char( const char * txt )
{
  DESCRIPTOR_DATA * man;

  PUSH_FUNCTION( "send_to_all_char" );

  if ( !txt )
  {
    mudlog( LOG_DEBUG, "send_to_all_char: �ӷ������T." );
    RETURN_NULL();
  }

  /* �ֳt�ˬd */
  if ( !*txt ) RETURN_NULL();

  for ( man = descriptor_list; man; man = man->next )
  {
    if ( verify_desc( man ) && man->connected == CON_PLAYING )
      send_to_char( txt, man->character );
  }

  RETURN_NULL();
}

/* �e�@�q�T�����Y���� */
void send_to_club( const char * txt, CLUB_DATA * pClub )
{
  DESCRIPTOR_DATA * man;

  PUSH_FUNCTION( "send_to_club" );

  if ( !txt || !pClub )
  {
    mudlog( LOG_DEBUG, "send_to_club: �ӷ������T" );
    RETURN_NULL();
  }

  /* �ֳt�ˬd */
  if ( !*txt ) RETURN_NULL();

  for ( man = descriptor_list; man; man = man->next )
  {
    if ( verify_desc( man )
      && man->connected == CON_PLAYING
      && man->character
      && man->character->club == pClub )
      send_to_char( txt, man->character );
  }

  RETURN_NULL();
}

/* ��X���Y���ж����Ҧ����H�� */
void sendmsg_to_room( const char * txt, CHAR_DATA * ch )
{
  CHAR_DATA * victim;

  PUSH_FUNCTION( "sendmsg_to_room" );

  if ( !txt || !ch || !ch->in_room )
  {
    mudlog( LOG_DEBUG, "sendmsg_to_room: �ӷ������T" );
    RETURN_NULL();
  }

  /* �ֳt�ˬd */
  if ( !*txt ) RETURN_NULL();

  for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    if ( IS_AWAKE( victim ) && !IS_NPC( victim ) ) send_to_char( txt, victim );

  RETURN_NULL();
}

/* �e�T����Y���ж� */
void sendmsg_to_someroom( const char * txt , ROOM_INDEX_DATA * pRoom )
{
  CHAR_DATA * victim;

  PUSH_FUNCTION( "sendmsg_to_someroom" );

  if ( !txt || !pRoom )
  {
    mudlog( LOG_DEBUG, "sendmsg_to_someroom: �ӷ������T." );
    RETURN_NULL();
  }

  /* �ֳt�ˬd */
  if ( !*txt ) RETURN_NULL();

  for ( victim = pRoom->people; victim; victim = victim->next_in_room )
    if ( IS_AWAKE( victim ) && !IS_NPC( victim ) ) send_to_char( txt, victim );

  RETURN_NULL();
}

/* ��X�����a */
void send_to_char( const char * txt, CHAR_DATA * ch )
{
  DESCRIPTOR_DATA * d;

  PUSH_FUNCTION( "send_to_char" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "send_to_char: �ӷ������T." );
    RETURN_NULL();
  }

  if ( !txt || !( d = ch->desc ) ) RETURN_NULL();

  /* �p�G�r��ܤp�h������X */
  if ( d->showstr_point || d->edit_mode != EDIT_NONE )
  {
    d->stack = TRUE;
    write_to_buffer( d, txt, str_len( txt ) );
    d->stack = FALSE;
    RETURN_NULL();
  }

  /* �Y���|����X�������r��h��L������ */
  /*
  if ( d->showstr_point ) free_string( d->showstr_head );
  */

  if ( !( d->showstr_head = str_dup( txt ) ) ) RETURN_NULL();
  d->showstr_point = d->showstr_head;
  show_string( d, "" );
  RETURN_NULL();
}

/* ��X���D�n�֤� */
void show_string( struct descriptor_data * d, char * input )
{
  char   buffer[ MAX_SCREEN_BUFFER ];
  char   buf   [ MAX_INPUT_LENGTH ];
  char * scan;
  char * chk;
  int    lines = 0;
  int    toggle = 1;
  int    limit;

  PUSH_FUNCTION( "show_string" );

  one_argument( input , buf );

  switch( UPPER( buf[0] ) )
  {
  /* �U�@�� */
  case '\x0':
  case 'C'  :
    lines = 0;
    break;

  /* ���s��X�e�� */
  case 'R':
    lines = - 1 - ( d->character->pcdata->pagelen );
    break;

   /* �W�@�� */
  case 'B':
    lines = -( 2 * d->character->pcdata->pagelen );
    break;

  /* ��ܨD�U */
  case 'H':
    write_to_buffer( d,
      "C �� RETURN [�U�@��]�MR [���s��X�e��]�M\n\r"
      "B = [�W�@��]�MH = [�D�U]�MQ �Ψ�L�� [����]�C\n\r\n\r" , 0 );

    lines = - 1 - ( d->character->pcdata->pagelen );
    break;

  /* ���M�����ù���X */
  default:

    cease_stack( d );
    RETURN_NULL();
  }

  /* �p�G�n�h��W�@�i�e�� */
  if ( lines < 0 )
  {
    for ( scan = d->showstr_point; scan > d->showstr_head; scan-- )
    {
      if ( ( *scan == '\n' ) || ( *scan == '\r' ) )
      {
        toggle = -toggle;
        if ( toggle < 0 ) if ( !( ++lines ) ) break;
      }
    }
    d->showstr_point = scan;
  }

  toggle = 1;
  for ( lines = limit = 0, scan = buffer; ; scan++, d->showstr_point++ )
  {
    /* �t�ΦۧګO�@ */
    if ( limit++ > MAX_SCREEN_BUFFER - 2 )
    {
      *d->showstr_point = '\x0';
      mudlog( LOG_DEBUG , "show_string:�e����X�w�W�L MAX_SCREEN_BUFFER �j�p." );
      mudlog( LOG_DEBUG , "show_string:�Ш� merc.h �� MAX_SCREEN_BUFFER �[�j." );
    }

    /* �p�⴫�檺��� */
    if ( ( ( *scan = *d->showstr_point ) == '\n' || *scan == '\r' )
      && ( toggle = -toggle ) < 0 ) lines++;

    /* �O�_�i�H��X�����a�F */
    else if ( *scan == '\x0' || ( d->character && !IS_NPC( d->character )
           && lines >= d->character->pcdata->pagelen ) )
    {
      *scan = '\x0';
      write_to_buffer( d, buffer, 0 );

      /* �p�G�ѤU�����u�O�ť�, �h���L, ��O��X���� */
      for ( chk = d->showstr_point; *chk == ' ' ; chk++ );

      /* ��X���� */
      if ( *chk == '\x0' ) cease_stack( d );
      RETURN_NULL();
    }
  }
  RETURN_NULL();
}

/* �Ѽ�
1. �r��ӷ�.
2. �D�n�ާ@�H��
3. �i�H�O�r��@, ���~�@
4. �i�H�O�r��G, �Q�ާ@�H��.
5. ���A.
   a) TO_CHAR   ���G�u��X���D�n�ާ@�H��, �Y�ѼƤG.
   b) TO_VICT   ���G�u��X���Q�ާ@�H��, ���o�ӳQ�ާ@�H���i�H�O�D�n�ާ@�H��
   c) TO_ROOM   ���G��X���D�n�ާ@�H���ж��Ҧ����H, ���F�D�n�ާ@�H��.
   d) TO_NOVICT ���G��X�����F�D�n�ާ@�M�Q�ާ@�H���H�~�P�@�ж����H.

�Ѽ�
$t �����r��@�����e.
$T �����r��G�����e.
$n �D���H��(1)
$N �D���H��(2)
$e
$E
$s
$S
$p ���~�@
$P ���~�G
$d �ѼƤG���������ԭz
$A ����
$$ $
$C �C�ⱱ��X
$0 �٭��C��
$1 ���G�׬���
$2 ���G�׺��
$3 ���G�׶���
$4 ���G���Ŧ�
$5 ���G�׵���
$6 ���G�פѪŦ�
$7 ���G�ץզ�
$z �H�����^��W��
$Z �H�����^��W��
$B �a�n
$w �ޯ�W��(1)
$W �ޯ�W��(2)
$y �����W��(1)
$Y �����W��(2)
$i ����ƾ��(1)
$I ����ƾ��(2)
$x �Ʀr���A���(1)
$X �Ʀr���A���(2)
$r �ж��W��(1)
$R �ж��W��(2)
*/

/* �榡�ƿ�X���D�n�֤� */
void act( const char * format, CHAR_DATA * ch, const void * arg1,
         const void * arg2, int type )
{
  char             buf[MAX_STRING_LENGTH];
  char             fname[MAX_INPUT_LENGTH];
  char             buffer[MAX_STRING_LENGTH];
  CHAR_DATA      * to;
  CHAR_DATA      * vch    = ( CHAR_DATA *  ) arg2;
  OBJ_DATA       * obj1   = ( OBJ_DATA  *  ) arg1;
  OBJ_DATA       * obj2   = ( OBJ_DATA  *  ) arg2;
  CLUB_DATA      * club1  = ( CLUB_DATA *  ) arg1;
  CLUB_DATA      * club2  = ( CLUB_DATA *  ) arg2;
  SKILL_DATA     * skill1 = ( SKILL_DATA * ) arg1;
  SKILL_DATA     * skill2 = ( SKILL_DATA * ) arg2;
  MOB_INDEX_DATA * pMob;
  const char     * str;
  const char     * i;
  const int      * variable1 = arg1;
  const int      * variable2 = arg2;
  char           * point;
  int              len;

  PUSH_FUNCTION( "act" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "act: �ӷ������T, ��r�� '%s'", format );
    RETURN_NULL();
  }

  /* �����Ū��άO���׬��s���T�� */
  if ( !ch->in_room || !format || !*format ) RETURN_NULL();

  to = ch->in_room->people;

  if ( type == TO_VICT )
  {
    if ( !vch || !vch->in_room )
    {
      mudlog( LOG_DEBUG , "act: vch �ӷ������T�M��r�� '%s'", format );
      RETURN_NULL();
    }

    to = vch->in_room->people;
  }

  for ( ; to; to = to->next_in_room )
  {
    if ( ( !to->desc
      && ( IS_NPC( to ) && !( to->pIndexData->progtypes & ACT_PROG ) ) )
      || !IS_AWAKE( to ) )
        continue;

    if ( type != TO_ALL )
    {
      if ( type == TO_CHAR    && to != ch )                  continue;
      if ( type == TO_VICT    && ( to != vch || to == ch ) ) continue;
      if ( type == TO_ROOM    && to == ch )                  continue;
      if ( type == TO_NOTVICT && ( to == ch || to == vch ) ) continue;
    }

    point = buf;
    str   = format;
    len   = 0;

    while ( *str )
    {
      if ( len > sizeof( buf ) - 10 )
      {
        mudlog( LOG_DEBUG, "act: ��X�L���M��r�� '%s'", format );
        RETURN_NULL();
      }

      if ( *str != '$' )
      {
        *point++ = *str++;
        len++;
        continue;
      }

      switch( *( ++str ) )
      {
      default:

        mudlog( LOG_DEBUG , "act: ���~���R�O '%c'�M��r�� '%s'"
          , *str, format );

        i = " <@@@> ";
        break;

      case '$':
        i = "$";
        break;

      /* �٭��C�� */
      case '0':
        i = "\e[0m";
        break;

      /* ���� */
      case '1':
        i = "\e[1;31m";
        break;

      /* ��� */
      case '2':
        i = "\e[1;32m";
        break;

      /* ���� */
      case '3':
        i = "\e[1;33m";
        break;

      /* �Ŧ� */
      case '4':
        i = "\e[1;34m";
        break;

      /* ���� */
      case '5':
        i = "\e[1;35m";
        break;

      /* �ѪŦ� */
      case '6':
        i = "\e[1;36m";
        break;

      /* �զ� */
      case '7':
        i = "\e[1;37m";
        break;

      /* ���� */
      case 'A':
        i = "\n\r";
        break;

      /* �a�n */
      case 'B':
        i = "\a";
        break;

      /* �]�w�C�ⱱ��X */
      case 'C':
        i = "\e[";
        break;

      case 't':

        if ( !arg1 )
        {
          mudlog( LOG_DEBUG, "act: arg1 �ӷ������T(t), ��r�� '%s'", format );
          i = " <@@@> ";
          break;
        }

        i = ( char * ) arg1;
        break;

      case 'T':

        if ( !arg2 )
        {
          mudlog( LOG_DEBUG, "act: arg2 �ӷ������T(T), ��r�� '%s'", format );
          i = " <@@@> ";
          break;
        }

        i = ( char * ) arg2;
        break;

      case 'w':

        if ( !skill1 )
        {
          mudlog( LOG_DEBUG, "act: skill1 �ӷ������T(w), ��r�� '%s'", format );
          i = " <@@@> ";
          break;
        }

        if ( !IS_NPC( to ) && IS_SET( to->act, PLR_EXACT ) )
        {
          sprintf( buffer, "%s\e[0m(%s)", skill1->cname, skill1->name );
          i = buffer;
        }

        else
        {
          i = skill1->cname;
        }

        break;

      case 'W':

        if ( !skill2 )
        {
          mudlog( LOG_DEBUG, "act: skill2 �ӷ������T(W), ��r�� '%s'", format );
          i = " <@@@> ";
          break;
        }

        if ( !IS_NPC( to ) && IS_SET( to->act, PLR_EXACT ) )
        {
          sprintf( buffer, "%s\e[0m(%s)", skill2->cname, skill2->name );
          i = buffer;
        }

        else
        {
          i = skill2->cname;
        }

        break;

      case 'n':

        if ( to == ch )
        {
          i = "�A";
          break;
        }

        i = buffer;

        if ( can_see( to, ch ) )
        {
          if ( !can_see_mask( to, ch )
            && is_affected( ch, SLOT_MASK )
            && ( pMob = get_mob_index( ch->mask ) ) )
          {
            str_cpy( buffer, mob_index_name( NULL, pMob ) );
          }

          else
          {
            if ( !IS_NPC( to ) && !IS_SET( to->act, PLR_EXACT ) )
              str_cpy( buffer, IS_NPC( ch ) ? ch->byname : ch->cname );

            else
              str_cpy( buffer, mob_name( NULL, ch ) );
          }
        }
        else
        {
          str_cpy( buffer,  "�Y�����W�H��" );
        }

        break;

      case 'N':

        if ( !vch )
        {
          mudlog( LOG_DEBUG, "act: vch �ӷ������T(N), ��r�� '%s'", format );
          i = " <@@@> ";
          break;
        }

        if ( to == vch )
        {
          i = "�A";
          break;
        }

        i = buffer;

        if ( can_see( to, vch ) )
        {
          if ( !can_see_mask( to, vch )
            && vch->mask
            && ( pMob = get_mob_index( vch->mask ) ) )
          {
            str_cpy( buffer, mob_index_name( NULL, pMob ) );
          }

          else
          {
            if ( !IS_NPC( to ) && !IS_SET( to->act, PLR_EXACT ) )
              str_cpy( buffer, IS_NPC( vch ) ? vch->byname : vch->cname );
            else
              str_cpy( buffer, mob_name( to, vch ) );
          }
        }

        else
        {
          str_cpy( buffer, "�Y�����W�H��" );
        }

        break;

      case 'r':
        sprintf( buffer, "%s\e[0m", ch->in_room->name );
        i = buffer;
        break;

      case 'e':
        i = ( to == ch ) ? "�A" : he_she[ URANGE( SEX_NEUTRAL, ch->sex, SEX_FEMALE ) ];
        break;

      case 'E':

        if ( !vch )
        {
          mudlog( LOG_DEBUG, "act: vch �ӷ������T(E), ��r�� '%s'", format );
          i = " <@@@> ";
          break;
        }

        i = ( to == vch ) ? "�A" : he_she[ URANGE( SEX_NEUTRAL, vch->sex, SEX_FEMALE ) ];
        break;

      case 's':
        i = ( to == ch ) ? "�A��" : his_her[ URANGE( SEX_NEUTRAL, ch->sex, SEX_FEMALE ) ];
        break;

      case 'S':

        if ( !vch )
        {
          mudlog( LOG_DEBUG, "act: vch �ӷ������T(S), ��r�� '%s'", format );
          i = " <@@@> ";
          break;
        }

        i = ( to == vch ) ? "�A��" : his_her[ URANGE( SEX_NEUTRAL, vch->sex, SEX_FEMALE ) ];
        break;

      case 'p':

        if ( !obj1 )
        {
          mudlog( LOG_DEBUG, "act: obj1 �ӷ������T(p), ��r�� '%s'", format );
          i = " <@@@> ";
          break;
        }

        if ( obj1->corpse_owner && obj1->corpse_owner == to )
        {
          i = "�ۤv����(corpse)";
        }
        else
        {
          if ( can_see_obj( to, obj1 ) )
          {
            if ( !IS_NPC( to ) && !IS_SET( to->act, PLR_EXACT ) )
              str_cpy( buffer, obj1->cname );
            else
              str_cpy( buffer, obj_name( to, obj1 ) );

            i = buffer;
          }
          else
          {
            i = "�Y��";
          }
        }

        break;

      case 'P':

        if ( !obj2 )
        {
          mudlog( LOG_DEBUG, "act: obj2 �ӷ������T(P), ��r�� '%s'", format );
          i = " <@@@> ";
          break;
        }

        if ( obj2->corpse_owner && obj2->corpse_owner == to )
        {
          i = "�ۤv����(corpse)";
        }
        else
        {
          if ( can_see_obj( to, obj2 ) )
          {
            if ( !IS_NPC( to ) && !IS_SET( to->act, PLR_EXACT ) )
              sprintf( buffer, "%s\e[0m", obj2->cname );
            else
              str_cpy( buffer, obj_name( to, obj2 ) );

            i = buffer;
          }
          else
          {
            i = "�Y��";
          }
        }

        break;

      case 'x':

        if ( !arg1 )
        {
          mudlog( LOG_DEBUG, "act: ��ƨӷ������T(x), ��r�� '%s'", format );
          i = " <@@@> ";
        }

        else
        {
          sprintf( buffer, "%d", *variable1 );
          i = buffer;
        }

        break;

      case 'X':

        if ( !arg2 )
        {
          mudlog( LOG_DEBUG, "act: ��ƨӷ������T(X), ��r�� '%s'", format );
          i = " <@@@> ";
        }

        else
        {
          sprintf( buffer, "%d", *variable2 );
          i = buffer;
        }

        break;

      case 'i':

        if ( !arg1 )
        {
          mudlog( LOG_DEBUG, "act: ��ƨӷ������T(i), ��r�� '%s'", format );
          i = " <@@@> ";
        }

        else
        {
          chinese_number( *variable1, buffer );
          i = buffer;
        }

        break;

      case 'I':

        if ( !arg2 )
        {
          mudlog( LOG_DEBUG, "act: ��ƨӷ������T(I), ��r�� '%s'", format );
          i = " <@@@> ";
        }

        else
        {
          chinese_number( *variable2, buffer );
          i = buffer;
        }

        break;

      case 'y':

        if ( !club1 )
        {
          mudlog( LOG_DEBUG, "act: club1 �ӷ������T(y), ��r�� '%s'", format );
          i = " <@@@> ";
          break;
        }

        i = buffer;
        sprintf( buffer, "%s\e[0m(%s)", club1->cname, club1->name );
        break;

      case 'Y':

        if ( !club2 )
        {
          mudlog( LOG_DEBUG, "act: club2 �ӷ������T(Y), ��r�� '%s'", format );
          i = " <@@@> ";
          break;
        }

        i = buffer;
        sprintf( buffer, "%s\e[0m(%s)", club2->cname, club2->name );
        break;

      case 'd':

        if ( !arg2 || ( ( char * ) arg2)[0] == '\x0' )
        {
          i = "��";
        }

        else
        {
          one_argument( ( char * ) arg2, fname );
          i = fname;
        }
        break;

      case 'z':

        i = "";
        if ( !IS_NPC( ch ) && can_see( to, ch ) )
        {
          sprintf( fname, "(%s)", ch->name );
          i = fname;
        }

        break;

      case 'Z':

        if ( !vch )
        {
          mudlog( LOG_DEBUG, "act: vch �ӷ������T(Z), ��r�� '%s'", format );
          i = " <@@@> ";
          break;
        }

        i = "";
        if ( !IS_NPC( vch ) && can_see( to, vch ) )
        {
          sprintf( fname, "(%s)", vch->name );
          i = fname;
        }

        break;

      }

      ++str;
      while ( ( *point = *i ) != '\x0' ) ++point, ++i, len++;
    }

    *point++ = '\n';
    *point++ = '\r';
    *point   = '\x0';

    if ( to->desc )
    {
      set_descriptor_stack( to->desc );
      write_to_buffer( to->desc, buf, 0 );
      to->desc->stack = FALSE;
    }

    if ( MOBtrigger ) mprog_act_trigger( buf, to, ch, obj1, vch );
  }

  MOBtrigger = TRUE;
  RETURN_NULL();
}

/*
$$ $
$n �D��
$N ����
$O ���~
$Z �٭��C��
$C �]�w�C��
$A ����
$l ���˳���
$R ���G�ת�����   1;31m
$r �C�G�ת�����   0;31m
$G ���G�ת����   1;32m
$g �C�G�ת����   0;32m
$Y ���G�ת�����   1;33m
$y �C�G�ת�����   0;33m
$B ���G�ת��Ŧ�   1;34m
$b �C�G�ת��Ŧ�   0;34m
$P ���G�ת�����   1;35m
$p �C�G�ת�����   0;35m
$S ���G�ת��ѪŦ� 1;36m
$s �C�G�ת��ѪŦ� 0;36m
$W ���G�ת��զ�   1;37m
$w �C�G�ת��զ�   0;37m
*/

/* �ޯ���ܦr�� */
void sact( CHAR_DATA * ch, CHAR_DATA * victim, OBJ_DATA * obj, char * format )
{
  CHAR_DATA  * to;
  SITUS_DATA * pSitus;
  char         buf[MAX_STRING_LENGTH];
  char       * pBuf;
  char       * pStr;
  char       * pSource;
  int          len;

  PUSH_FUNCTION( "sact" );

  /* ���դH�� */
  if ( !ch )
  {
    mudlog( LOG_DEBUG, "sact: ��H�H���O�Ū�, ��r�� '%s'", format );
    RETURN_NULL();
  }

  /* �����Ū��άO���׬��s���T�� */
  if ( !format || !*format )
  {
    mudlog( LOG_DEBUG, "sact: �ӷ��r��O�Ū�, '%s'", format );
    RETURN_NULL();
  }

  /* �ˬd�ж��O�_�O�Ū�, �_�h�i��ɭP��� */
  if ( !ch->in_room )
  {
    mudlog( LOG_DEBUG, "sact: �H���ж��O�Ū�, '%s'", format );
    RETURN_NULL();
  }

  /* �e���ж����C�@�ӤH�� */
  for ( to = ch->in_room->people; to; to = to->next_in_room )
  {
    if ( !to->desc
      || !IS_AWAKE( to )
      || ( !IS_NPC( to ) && !IS_SET( to->act, PLR_MESSAGE ) ) ) continue;

    *( pBuf = buf ) = '\x0';
    for ( len = 0, pSource = format; *pSource; pSource++ )
    {
      /* �ˬd�r�ꪺ���� */
      if ( len > sizeof( buf ) - 10 )
      {
        mudlog( LOG_DEBUG, "sact: �r��W�X�d��, '%s'", format );
        RETURN_NULL();
      }

      /* �p�G���O�R�O�r�� */
      if ( *pSource != '$' )
      {
        *pBuf++ = *pSource;
        len++;
        continue;
      }

      switch( *(++pSource ) )
      {
      default :
        mudlog( LOG_DEBUG , "sact: ���~���R�O '%c', ��r�� '%s'"
          , *pSource, format );

        pStr = "<@@@>";
        break;

      case '$':
        pStr = "$";
        break;

      case 'A':
        pStr = "\n\r";
        break;

      /* ���G���Ŧ� */
      case 'B':
        pStr = "\e[1;34m";
        break;

      /* �C�G���Ŧ� */
      case 'b':
        pStr = "\e[0;34m";
        break;

      /* �]�w�C�� */
      case 'C':
        pStr = "\e[";
        break;

      case 'E':
             if ( !victim      ) pStr = "�����W�H��";
        else if ( to == victim ) pStr = "�A";
        else                     pStr = he_she[ URANGE( SEX_NEUTRAL, victim->sex, SEX_FEMALE ) ];
        break;

      case 'e':
        pStr = ( to == ch ) ? "�A" : he_she[ URANGE( SEX_NEUTRAL, ch->sex, SEX_FEMALE ) ];
        break;

      /* ���G�׺�� */
      case 'G':
        pStr = "\e[1;32m";
        break;

      /* �C�G�׺�� */
      case 'g':
        pStr = "\e[0;32m";
        break;

      case 'l':

        if ( !victim )
        {
          mudlog( LOG_DEBUG, "sact: ��H�O�Ū�, '%s'", format );
          RETURN_NULL();
        }

        while ( situs_lookup( victim->situs ) == NULL )
          random_situs( victim, ATTACK_RANDOM );

        if ( !( pSitus = situs_lookup( victim->situs ) ) ) pStr = "<@@@>";
        else pStr = pSitus->situs_name;
        break;

      /* ���� */
      case 'N':
             if ( !victim )      pStr = "�����W�H��";
        else if ( to == victim ) pStr = "�A";
        else                     pStr = mob_name( to, victim );
        break;

      /* �D�� */
      case 'n':
        pStr = ( to == ch ) ? "�A" : mob_name( to, ch );
        break;

      /* ���~ */
      case 'O':

        if ( !obj )
        {
          mudlog( LOG_DEBUG , "sact: ���~�O�Ū�, '%s'", format );
          RETURN_NULL();
        }

        pStr = can_see_obj( to, obj ) ? obj->cname : "�Y��";
        break;

      /* ���G�׵��� */
      case 'P':
        pStr = "\e[1;35m";
        break;

      /* �C�G�׵��� */
      case 'p':
        pStr = "\e[0;35m";
        break;

      /* ���G�׬��� */
      case 'R':
        pStr = "\e[1;31m";
        break;

      /* �C�G�׬��� */
      case 'r':
        pStr = "\e[0;31m";
        break;

      /* �]�w���G�פѪŦ� */
      case 'S':
        pStr = "\e[1;36m";
        break;

      /* �]�w�C�G�פѪŦ� */
      case 's':
        pStr = "\e[0;36m";
        break;

      /* �]�w���G�ץզ� */
      case 'W':
        pStr = "\e[1;37m";
        break;

      /* �]�w�C�G�ץզ� */
      case 'w':
        pStr = "\e[0;37m";
        break;

      /* �]�w���G�׶��� */
      case 'Y':
        pStr = "\e[1;33m";
        break;

      /* �]�w�C�G�׶��� */
      case 'y':
        pStr = "\e[0;33m";
        break;

      /* �٭��C��X */
      case 'Z':
        pStr = "\e[0m";
        break;
      }

      if ( !pStr || !*pStr ) continue;

      if ( len + str_len( pStr ) > sizeof( buf ) - 10 )
      {
        mudlog( LOG_DEBUG, "sact: �s���r��W�X�d��, '%s'", format );
        RETURN_NULL();
      }

      while ( ( *pBuf = *pStr ) != '\x0' ) ++pBuf, ++pStr, len++;
    }

    /* ���W����r��, �H�Φr�굲���r�� */
    *pBuf++ =  '\n';
    *pBuf++ =  '\r';
    *pBuf++ = '\x0';

    /* �ھڤ��P����H�e�X���P���r�� */
    set_descriptor_stack( to->desc );
    write_to_buffer( to->desc, buf, 0 );
    to->desc->stack = FALSE;
  }

  RETURN_NULL();
}

/* ���j���� */
void spilt_semicolon( DESCRIPTOR_DATA * d )
{
  char buffer[ MAX_INPUT_LENGTH * 2 ];
  int  source;
  int  target;

  PUSH_FUNCTION( "spilt_semicolon" );

  /* �Y���O�w�g�i�J�C���h���� */
  if ( !d || d->connected != CON_PLAYING ) RETURN_NULL();

  for ( buffer[0] = '\x0', source = target = 0; d->inbuf[source]; source++ )
  {
    if ( target >= sizeof( buffer ) - 10 ) break;

    if ( d->inbuf[source] == SYMBOL_SEMICOLON )
    {
      buffer[target++] = '\n';
      buffer[target++] = '\r';
    }

    else
    {
      buffer[target++] = d->inbuf[source];
    }
  }

  buffer[target] = '\x0';

  /* �ݬݬO�_�W�X�d�� */
  if ( target >= sizeof( d->inbuf )  - 10 )
  {
    write_to_buffer( d,
      "\n\r�A��J�w�W�X�t�Ϊ��d��M�ҥH�M���A����J�w�İϡC\n\r", 0 );

    str_cpy( d->inbuf, "\n\r" );
  }

  else
  {
    str_cpy( d->inbuf, buffer );
  }

  RETURN_NULL();
}

void translate_command( DESCRIPTOR_DATA * d )
{
  char * pBuf;
  char * pDest = NULL;
  char   buf1[ MAX_INPUT_LENGTH ];
  char   buffer[ 4 * MAX_INPUT_LENGTH ];
  int    dup;
  int    loop;

  PUSH_FUNCTION( "translate_command" );

  /* �Y���i�J�C���h���� */
  if ( !d
    || d->connected != CON_PLAYING
    || str_len( d->incomm ) >= sizeof( buf1 ) - 2 ) RETURN_NULL();

  /* ����r��B�z�@�U */
  str_cpy( ( pBuf = buf1 ) , d->incomm );
  while ( *pBuf == ' ' ) pBuf++;

  /* �Y���O�R�O�_�Y�h���� */
  if ( *pBuf != SYMBOL_COMMAND ) RETURN_NULL();

  /* �Ʀr���� */
  pDest = test_number( pBuf + 1 , &dup );

  if ( dup > 0 && dup < MaxRepeat - 1 )
  {
    /* �ˬd�O�_�W�X�d�� */
    if ( str_len( d->inbuf ) + ( dup * ( str_len( pDest ) + 2 ) )
      > sizeof( d->incomm ) - 10 )
    {
      write_to_buffer( d,
        "\n\r�A��J�w�W�X�t�Ϊ��d��M�ҥH�M���A����J�w�İϡC\n\r", 0 );
      str_cpy( d->incomm, " \x0" );
      RETURN_NULL();
    }

    str_cpy( buffer , d->inbuf );
    for ( d->inbuf[0] = '\x0', loop = 0; loop < dup; loop++ )
    {
      str_cat( d->inbuf ,  pDest );
      str_cat( d->inbuf , "\n\r" );
    }

    str_cat( d->inbuf  , buffer );
    str_cpy( d->incomm , " \x0" );
    RETURN_NULL();
  }

  /* �R�O #BEEP */
  if ( !str_cmp( pBuf + 1 , "beep" ) )
  {
    write_to_buffer( d, "##BEEP!!!\a\n\r" , 0 );
    str_cpy( d->incomm , " \x0" );
    RETURN_NULL();
  }

  RETURN_NULL();
}

/* ��ܥX�C�������A */
void show_game_status( DESCRIPTOR_DATA * man )
{
  char buffer[MAX_STRING_LENGTH];
  char buf1[MAX_INPUT_LENGTH];
  char buf2[MAX_INPUT_LENGTH];
  char buf3[MAX_INPUT_LENGTH];
  char buf4[MAX_INPUT_LENGTH];

  PUSH_FUNCTION( "show_game_status" );

  if ( !man )
  {
    mudlog( LOG_DEBUG, "show_game_status: �ӷ��O�Ū�." );
    RETURN_NULL();
  }

  chinese_number( merc_id         , buf1 );
  chinese_number( get_usernum()   , buf2 );
  chinese_number( get_attemptnum(), buf3 );
  chinese_number( MaxConnect      , buf4 );

  sprintf( buffer,
    "\n\r\e[1;33m�w��Ө�\e[1;37m�i\e[1;44;36m%s\e[0m\e[1;37m�j"
    "( ���t�Τw�g�ҰʤF\e[1;33m%s \e[1;37m)\n\r"
    "\e[1;33m�t�έt���q�� %s%% �`�W���H�Ʀ�%s���M�W�u�H�Ƴ̰��O��%s�H\n\r"
    "�ثe�@��%s�H�b��������%s�İ��M"
    "�t�~�٦�%s�ӤH���ճs�u���C\e[0m\n\r\n\r"
    , mud_name, get_worktime_string(), get_loading()
    , buf1, buf4, buf2, mud_name, buf3 );

  write_to_descriptor( man, buffer, 0 );
  RETURN_NULL();
}

/* ���^�ƥ��ɮ� */
void recover_file( DESCRIPTOR_DATA * man )
{
  char   source[MAX_FILE_LENGTH];
  char   target[MAX_FILE_LENGTH];
  char   failcode[MAX_FILE_LENGTH];
  char   buf[MAX_STRING_LENGTH];
  char   transfer;
  char * name;
  FILE * fp_source;
  FILE * fp_target;

  PUSH_FUNCTION( "recover_file" );

  if ( !man )
  {
    mudlog( LOG_DEBUG, "recover_file: �ʥF�ӷ�." );
    RETURN_NULL();
  }

  name = man->username;

  /* ���屼�K�X���~�� */
  str_cpy( failcode, file_name( name, FAILCODE_FILE ) );
  unlink( failcode );

  str_cpy( source, file_name( name, BACKUP_FILE ) );
  str_cpy( target, file_name( name, SAVE_FILE   ) );

  if ( ( fp_source = fopen( source, "r+" ) ) )
  {
    if ( ( fp_target = fopen( target, "w+" ) ) )
    {
      write_to_descriptor( man,
        "�A���ɮפw�g�l���M�t�Φ۰����A���^�ƥ��ɡT\n\r", 0 );
      mudlog( LOG_DEBUG , "[Bad_File] %s ���ɮ׷l���M�t�Φ۰ʨ��^�C", name );
      while ( ( transfer = fgetc( fp_source ) ) != EOF )
        fputc( transfer , fp_target );
      fclose( fp_target );

      /* �����ɮצs���Ҧ� */
      set_file_mode( target );
    }

    else
    {
      sprintf( buf,
        "\n\r\a\a\a�A���ɮפw�g�l���M���O�t�εL�k���A���^�ƥ��ɡM"
        "�Ц� \e[1;32m%s\e[0m ���i�C\n\r" , bbs_name );

      write_to_descriptor( man, buf, 0 );

      mudlog( LOG_DEBUG , "[Bad_File] %s ���ɮ׷l���M���t�εL�k���^.", name );
    }

    fclose( fp_source );
  }

  else
  {

    sprintf( buf,
      "\n\r\a\a\a�A���ɮפw�g�l���M���O�t�εL�k���A���^�ƥ��ɡM"
      "�Ц� \e[1;32m%s\e[0m ���i�C\n\r" , bbs_name );

    write_to_descriptor( man, buf, 0 );
    mudlog( LOG_DEBUG , "[Bad_File] %s ���ɮ׷l��, ���t�εL�k���^.", name );
  }

  RETURN_NULL();
}

void struct_updates( void )
{
  DESCRIPTOR_DATA * pDesc;
  CHAR_DATA       * pChar;
  OBJ_DATA        * pObj;

  PUSH_FUNCTION( "struct_update" );

  for ( pDesc = descriptor_list; pDesc; pDesc = pDesc->next )
  {
    if ( pDesc->delete && pDesc->counter > 0 && --pDesc->counter <= 0 )
      FreeDesc++;
  }

  for ( pChar = char_list; pChar; pChar = pChar->next )
  {
    if ( pChar->delete && pChar->counter > 0 && --pChar->counter <= 0 )
      FreeChar++;
  }

  for ( pObj = object_list; pObj; pObj = pObj->next )
  {
    if ( pObj->delete && pObj->counter > 0 && --pObj->counter <= 0 )
      FreeObject++;
  }

  RETURN_NULL();
}

bool check_password( const char * password )
{
  bool number = FALSE;
  bool alpha  = FALSE;

  PUSH_FUNCTION( "check_password" );

  if ( !password || str_len( password ) < MinPasswordLen ) RETURN( FALSE );
  if ( strict_password == FALSE ) RETURN( TRUE );

  for ( ; *password; password++ )
  {
    if ( isdigit( ( int ) *password ) ) number = TRUE;

    if ( islower( ( int ) *password ) || isupper( ( int ) *password ) )
      alpha = TRUE;
  }

  if ( !number || !alpha ) RETURN( FALSE );

  RETURN( TRUE );
}

void lock_file( DESCRIPTOR_DATA * pDesc )
{
  DESCRIPTOR_DATA * man;

  PUSH_FUNCTION( "lock_file" );

  if ( !pDesc )
  {
    mudlog( LOG_DEBUG, "lock_file: �ʥF�ӷ�." );
    RETURN_NULL();
  }

  if ( pDesc->username[0] == '\x0' ) RETURN_NULL();

  for ( man = descriptor_list; man; man = man->next )
  {
    if ( verify_desc( man )
      && man->username[0]
      && man != pDesc
      && !str_cmp( pDesc->username, man->username ) )
      man->lock_file = TRUE;
  }

  RETURN_NULL();
}

bool check_multi_login( DESCRIPTOR_DATA * man )
{
  DESCRIPTOR_DATA * d;

  PUSH_FUNCTION( "check_multi_login" );

  if ( !man || !man->host || !*man->host )
  {
    mudlog( LOG_DEBUG, "check_multi_login: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( man->naddr == 0 ) RETURN( FALSE );

  for ( d = descriptor_list; d; d = d->next )
  {
    if ( !verify_desc( d )
      || d == man
      || !d->host
      || !*d->host
      || d->naddr != man->naddr
      || !str_cmp( d->username, man->username )
      || imm_lookup( d->username ) ) continue;

    if ( man->remote
      && *man->remote
      && d->remote
      && *d->remote
      && str_cmp( d->remote, man->remote ) ) continue;

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

void set_innate( DESCRIPTOR_DATA * man )
{
  int          loop;
  int          slot;
  SKILL_DATA * pSkill;
  CHAR_DATA  * ch;

  PUSH_FUNCTION( "set_innate" );

  if ( !man || !( ch = man->character ) )
  {
    mudlog( LOG_DEBUG, "set_innate: �ʥF�ӷ�." );
    RETURN_NULL();
  }

  for ( loop = 0; loop < MAX_INNATE; loop++ )
  {
    if ( man->select[loop] == TRUE )
    {
      if ( ( slot = iInnate[loop] ) >= 0
        && slot < MAX_SKILL
        && ( pSkill = get_skill( slot ) ) )
      {
        ch->skill[slot] = UMIN( 100, UMAX( 0, pSkill->adeptation ) );
        pSkill->choosen++;
      }
    }
  }

  RETURN_NULL();
}

void show_home( DESCRIPTOR_DATA * man )
{
  AREA_DATA * pArea;
  char        buf1[MAX_STRING_LENGTH];
  char        buf2[MAX_STRING_LENGTH];
  int         loop;

  PUSH_FUNCTION( "show_home" );

  str_cpy( buf1, VERTICAL_LINE );

  for ( loop = 1, pArea = area_first; pArea; pArea = pArea->next )
  {
    if ( !pArea->capital ) continue;

    if ( loop > 5 && ( ( loop % 5 ) == 1 ) ) str_cat( buf1, "\n\r" );
    sprintf( buf2, "\e[1;32m%2d\e[0m. %-10s ", loop++, pArea->name );

    if ( str_len( buf1 ) + str_len( buf2 ) < sizeof( buf1 ) - 100 )
      str_cat( buf1, buf2 );
  }

  sprintf( buf2, "\n\r%s�п�ܧA���X�ͦa(\e[1;32m1\e[0m-\e[1;32m%d\e[0m)�R "
    , VERTICAL_LINE, loop -1 );
  str_cat( buf1, buf2 );

  write_to_buffer( man, buf1, 0 );
  RETURN_NULL();
}

void show_innate( DESCRIPTOR_DATA * man )
{
  SKILL_DATA * pSkill;
  int          count;
  int          loop;
  char         buf1[MAX_STRING_LENGTH];
  char         buf2[MAX_STRING_LENGTH];
  char         chinese1[MAX_INPUT_LENGTH];
  char         chinese2[MAX_INPUT_LENGTH];

  PUSH_FUNCTION( "show_innate" );

  if ( !man )
  {
    mudlog( LOG_DEBUG, "show_innate: �ʥF�ӷ�." );
    RETURN_NULL();
  }

  chinese_number( aInnate          , chinese1 );
  chinese_number( get_innate( man ), chinese2 );

  sprintf( buf1, "\n\r�п���A�Q�n���ޯ�u�A�i�H���%s�ӧޯ�M�A�w�g��F%s��"
    "�v\n\r%s", chinese1, chinese2, VERTICAL_LINE );

  for ( count = loop = 0; loop < MAX_INNATE; loop++ )
  {
    if ( iInnate[loop] >= 0 && ( pSkill = get_skill( iInnate[loop] ) ) )
    {
      sprintf( buf2, "%2d.[%s] \e[1;33m%-18s\e[0m "
        , ++count
        , man->select[loop] ? "\e[1;32m*\e[0m" : " "
        , pSkill->cname );

      if ( str_len( buf1 ) + str_len( buf2 ) < sizeof( buf1 ) - 10 )
        str_cat( buf1, buf2 );

      if ( ( count % 3 ) == 0 && str_len( buf1 ) < sizeof( buf1 ) - 12 )
        str_cat( buf1, "\n\r" );
    }
  }

  if ( count > 0
    && ( ( count % 3 ) != 0 )
    && str_len( buf1 ) < sizeof( buf1 ) - 12 )
    str_cat( buf1, "\n\r" );

  if ( str_len( buf1 ) + str_len( VERTICAL_LINE ) < sizeof( buf1 ) - 10 )
    str_cat( buf1, VERTICAL_LINE );

  sprintf( buf2, "�п�J \e[1;36mselect\e[0m(���) \e[1;36mhelp\e[0m(�D�U)"
    " \e[1;36mdone\e[0m(����) \e[1;36mclear\e[0m(�M��) "
    "\e[1;36mquery\e[0m(�d��) " );

  if ( str_len( buf1 ) + str_len( buf2 ) < sizeof( buf1 ) - 10 )
    str_cat( buf1, buf2 );

  write_to_buffer( man, buf1, 0 );

  RETURN_NULL();
}

int get_innate( DESCRIPTOR_DATA * man )
{
  int loop;
  int count;

  PUSH_FUNCTION( "get_innate" );

  if ( !man )
  {
    mudlog( LOG_DEBUG, "show_innate: �ʥF�ӷ�." );
    RETURN( 0 );
  }

  for ( loop = count = 0; loop < MAX_INNATE; loop++ )
    if ( man->select[loop] == TRUE ) count++;

  RETURN( count );
}

bool check_email_address( char * pString )
{
  bool mousetail;

  PUSH_FUNCTION( "check_email_address" );

  if ( !pString || str_len( pString ) < 10 || str_len( pString ) > 50 )
    RETURN( FALSE );

  if ( !strict_email ) RETURN( TRUE );

  for ( mousetail = FALSE; *pString; pString++ )
  {
    if ( *pString == '@' )
    {
      if ( mousetail ) RETURN( FALSE );
      mousetail = TRUE;
      continue;
    }

    if ( isalnum( ( int ) *pString ) ) continue;

    switch( *pString )
    {
    default:
      RETURN( FALSE );

    case '.':
    case '-':
    case '(':
    case ')':
    case '<':
    case '>':
    case '[':
    case ']':
      break;
    }
  }

  RETURN( mousetail );
}

bool set_descriptor_stack( DESCRIPTOR_DATA * man )
{
  PUSH_FUNCTION( "set_desccriptor_stack" );

  if ( !man )
  {
    mudlog( LOG_DEBUG, "set_descriptor_stack: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( man->connected == CON_PLAYING && man->showstr_point )
  {
    man->stack = TRUE;
    RETURN( TRUE );
  }

  RETURN( FALSE );
}

void save_to_stack( DESCRIPTOR_DATA * man, const char * txt, int len )
{
  char   buf[MAX_BUFFER_LENGTH + 10 ];
  char * pString;
  int    loop;

  PUSH_FUNCTION( "save_to_stack" );

  if ( !man )
  {
    mudlog( LOG_DEBUG, "save_to_stack: �ʥF�ӷ�." );
    RETURN_NULL();
  }

  if ( man->buffer && *man->buffer ) str_cpy( buf, man->buffer );
  else buf[0] = '\x0';

  if ( ( str_len( buf ) + len ) <= MAX_BUFFER_LENGTH )
  {
    for ( pString = buf; *pString; pString++ );
    for ( loop = 0; loop < len && *( txt + loop ); loop++ )
      *( pString + loop ) = *( txt + loop );

    *( pString + loop ) = '\x0';
    free_string( man->buffer );
    man->buffer = str_dup( buf );
  }

  RETURN_NULL();
}

void process_stack( DESCRIPTOR_DATA * man )
{
  PUSH_FUNCTION( "process_stack" );

  if ( !man )
  {
    mudlog( LOG_DEBUG, "process_stack: �ʥF�ӷ�." );
    RETURN_NULL();
  }

  if ( man->buffer && *man->buffer )
  {
    write_to_buffer( man, "\n\r                          ----"
      "�u\e[1;37m��X�Ȧs���e\e[0m�v----\n\r\n\r", 0 );

    write_to_buffer( man, man->buffer, 0 );
    free_string( man->buffer );
    man->buffer = str_dup( "" );
  }

  RETURN_NULL();
}

void cease_stack( DESCRIPTOR_DATA * man )
{
  CHAR_DATA * ch;

  PUSH_FUNCTION( "cease_stack" );

  if ( !man )
  {
    mudlog( LOG_DEBUG, "cease_stack: �ʥF�ӷ�." );
    RETURN_NULL();
  }

  if ( man->showstr_head )
  {
    free_string( man->showstr_head );
    man->showstr_head = NULL;
  }

  man->showstr_point = NULL;

  if ( man->edit_mode != EDIT_NONE )
  {
    if ( ( ch = man->character ) )
    {
      free_string( ch->stack );
      ch->stack = str_dup( "" );
    }

    man->edit_mode = EDIT_NONE;
    str_cpy( man->incomm, " \n\r" );
  }

  process_stack( man );

  RETURN_NULL();
}

void immortal_on_line( CHAR_DATA * ch )
{
  DESCRIPTOR_DATA * man;
  CHAR_DATA       * victim;

  PUSH_FUNCTION( "immortal_on_line" );

  if ( !ch || !verify_char( ch ) )
  {
    mudlog( LOG_DEBUG, "immortal_on_line: �ӷ����~." );
    RETURN_NULL();
  }

  for ( man = descriptor_list; man; man = man->next )
  {
    if ( verify_desc( man )
      && man->connected == CON_PLAYING
      && verify_char( ( victim = man->character ) )
      && IS_IMMORTAL( victim )
      && get_trust( victim ) >= get_trust( ch )
      && victim != ch )
      act( "$2�u�W�u�q���v$0 ���� $N (���šR$2$x$0 ) �W�u�ӤF�T"
        , victim, &ch->level, ch, TO_CHAR );
  }

  RETURN_NULL();
}

void immortal_off_line( CHAR_DATA * ch )
{
  DESCRIPTOR_DATA * man;
  CHAR_DATA       * victim;

  PUSH_FUNCTION( "immortal_off_line" );

  if ( !ch || !verify_char( ch ) )
  {
    mudlog( LOG_DEBUG, "immortal_off_line: �ӷ����~." );
    RETURN_NULL();
  }

  for ( man = descriptor_list; man; man = man->next )
  {
    if ( verify_desc( man )
      && man->connected == CON_PLAYING
      && verify_char( ( victim = man->character ) )
      && IS_IMMORTAL( victim )
      && get_trust( victim ) >= get_trust( ch )
      && victim != ch )
      act( "$2�u���u�q���v$0 ���� $N (���šR$2$x$0 ) ���u�F�T"
        , victim, &ch->level, ch, TO_CHAR );
  }

  RETURN_NULL();
}

void merc_exit( int exitcode )
{
  if ( CoreDump && merc_pid == getpid() ) abort();
  exit( exitcode );
}

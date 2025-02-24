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
*  這是由輔大化學系製作群所撰寫的遊戲﹐主體由 merc 改編而來﹐所有的版權    *
*  將會被保留﹐但歡迎大家修改﹐但我們也希望你們也能提供給大家﹐所有的商    *
*  業行為將不被允許。                                                      *
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

/* Signal 處理. Apollo 在 signal.h 有 __attribute(atomic) 的問題 */
/* 在這個地方不好處理 */

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

/* 和作業系統有關的宣告 */
/* 這裡沒有使用到 */
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

/* 和作業系統有關的宣告, 這裡沒有使用到 */

#if     defined(apollo)
#include <unistd.h>
#endif

/* 和作業系統有關的宣告 */
/* 這裡沒有使用到 */

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

/* 和作業系統有關的宣告 */
/* 我們沒使用到這裡 */

#if     defined(interactive)
#include <net/errno.h>
#include <sys/fcntl.h>
#endif

/* 和作業系統有關的宣告 */
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

/* 這裡沒有使用到 */
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

/* 這裡沒有使用到 */
#if     defined(MIPS_OS)
extern  int             errno;
#endif

/* 這裡沒有使用到 */
#if     defined(MSDOS)
int     gettimeofday    args( ( struct timeval *, void * ) );
int     kbhit           args( ( void ) );
#endif

/* 這裡沒有使用到 */
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

/* 這裡沒有使用到 */
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

/* 這裡沒有使用到 */
#if defined(sun)
int     close           args( ( int ) );
int     listen          args( ( int, int ) );
int     select          args( ( int, fd_set *, fd_set *, fd_set *, struct timeval * ) );
int     socket          args( ( int, int, int ) );
int     sigmask         args( ( int ) );
int     sigsetmask      args( ( int ) );
void    (*signal        args( (int signum, void (*handler)(int))))(int);
#endif

/* 這裡沒有使用到 */
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

/* 全域變數 */
DESCRIPTOR_DATA *   descriptor_list;         /* All open descriptors   */
bool                merc_down;               /* 系統 shutdown          */
bool                merc_exec       = FALSE; /* 是否 merc 已經執行了   */
bool                wizlock         = FALSE; /* 遊戲是否被鎖定         */
bool                NewLock         = FALSE;
bool                multilogin      = FALSE; /* 是否防止多重連線       */
bool                strict_password = FALSE; /* 更嚴格的密碼           */
bool                strict_email    = FALSE; /* 更嚴格的 email- box    */
bool                tick_permit     = TRUE;  /* 是否可以使用計數器     */
bool                ChatRecord      = TRUE;  /* 是否記錄談話歷史       */
int                 MudPort[MAX_PORT];       /* 定義最多有幾個 port    */
int                 MudConnect[MAX_PORT];    /* 每個連接埠的連線人數   */
int                 traceloop       = 0;     /* 是否陷入無窮回圈監控器 */
int                 max_stock_buy   = 0;     /* 每種股票最大持有數     */
int                 max_hire        = 3;
int                 max_board       = MAX_BOARD; /* 系統最多可以有幾個版面 */
int                 max_notes       = MAX_NOTES; /* 系統最多有幾封信       */
int                 Rechristen = MAX_RECHRISTEN; /* 每個人最多可以改名幾次 */
bool                first_warn      = TRUE;  /* 第一次重新開機警告     */
bool                second_warn     = TRUE;  /* 第二次重新開機警告     */
bool                third_warn      = TRUE;  /* 第三次重新開機警告     */
bool                AngelComing     = TRUE;  /* 是否守護神可以到來     */
bool                AutoBackup      = TRUE;  /* 是否自動取備份         */
bool                CheckServer     = TRUE;  /* 是否檢查是否為工作站   */
bool                GetFQDN         = TRUE;  /* 是否取得 FQDN          */
bool                RecordFile      = FALSE; /* 是否記錄開檔記錄       */
int                 AngelTimes      = 1;     /* 守護神每秒只來一次     */
int                 AngelDefault    = 60;    /* 玩家守護神週期預設值   */
int                 MaxConnect      = 0;     /* 最多上線人數           */
int                 max_ip          = MAX_IP; /* 最多限制連線位址      */
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

time_t              current_time;            /* 目前的時間             */
time_t              mud_boot_time;           /* 系統重置時間           */
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

/* 其他和作業系統無關的區域變數 */
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

char * const he_she  [] = { "它",   "他",   "她" };
char * const his_her [] = { "它的", "他的", "她的" };

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
        mudlog( LOG_ERR, "main: 起始化檔案名稱過長." );

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
        mudlog( LOG_ERR, "main: 起始化檔案名稱過長." );

      str_cpy( filename,  enviroment );
    }

    else
    {
      if ( str_len( INI_FILE ) >= sizeof( filename ) - 1 )
        mudlog( LOG_ERR, "main: 起始化檔案名稱過長." );

      str_cpy( filename, INI_FILE );
    }
  }

  /* 清除連線的連接埠 */
  for ( loop = 0; loop < MAX_PORT; loop++ )
    Control[loop] = MudPort[loop] = ERRORCODE;

  /* 清除預定的玩家物品 */
  for ( loop = 0; loop < MAX_DEFAULT_OBJECT; loop++ )
    DefaultObject[loop] = -1;

  /* 取得程式的行程號碼 */
  mudlog( LOG_INFO, "行程號碼﹕%d", merc_pid = getpid() );

  /* 起始化記憶體 */
  memory_ini();

  /* 讀取啟始檔案 */
  read_ini( filename );

  /* 路徑或檔案預設值 */
  default_file();

  /* 啟始化檔案路徑 */
  adjust_filename();

  mudlog( LOG_INFO, "%s存取非法位址信號", signal_segv ? "開啟" : "關閉" );
  mudlog( LOG_INFO, "%s浮點運算錯誤信號", signal_fpe  ? "開啟" : "關閉" );
  mudlog( LOG_INFO, "%s參考不當位址信號", signal_bus  ? "開啟" : "關閉" );
  mudlog( LOG_INFO, "%s非法指令集信號"  , signal_ill  ? "開啟" : "關閉" );

  /* 啟始化訊號 */
  signal_setup();

  /* 取得目前目錄 */
  getcwd( CurrentDir, sizeof( CurrentDir ) - 1 );

  /* 啟始化時間 */
  gettimeofday( &now_time, NULL );
  mud_boot_time = current_time  = ( time_t ) now_time.tv_sec;
  mud_boot_tick = get_uptime();

  if ( reboot_time > 0 ) reboot_time += current_time;

  /* 保留一個檔案代碼以供使用 */
  if ( !( fpReserve = fopen( null_file , "r" ) ) )
  {
    mudlog( LOG_INFO, strerror( errno ) );
    mudlog( LOG_ERR , "無法保留至少一個檔案代碼" );
  }

  /* 先取得主機的名稱, 由 /etc/hosts */
  if ( gethostname( HostName, MAX_HOSTNAME ) != 0 )
   mudlog( LOG_ERR, "無法取得主機名稱." );

  mudlog( LOG_INFO, "主機名稱 %s", HostName );

  /* 起使化 socket */
  for ( loop = 0; loop < MAX_PORT; loop++ )
  {
    if ( MudPort[loop] != ERRORCODE )
    {
      mudlog( LOG_INFO , "主行程「通信埠﹕%4d」管道 %d"
      , MudPort[loop], Control[loop] = init_socket( MudPort[loop] ) );
    }
    else
    {
      Control[loop] = ERRORCODE;
    }
  }

  /* 起使化網際連線 */
  mudlog( LOG_NET , "網際管道 %d", Internal = init_socket( internal_port ) );

  /* 限制最多上線人數 */
  if ( ( maxdesc = getdtablesize() ) <= 0 )
  {
    mudlog( LOG_INFO, strerror( errno ) );
    mudlog( LOG_ERR , "無法取得描述表最大個數" );
  }

  max_connect = UMIN( maxdesc - 8 , max_connect );
  mudlog( LOG_INFO , "最多上線人數上線 %d" , max_connect );

  /* 啟動 merc 主程式, 並載入區域檔 */
  boot_db();

  /* 啟使化共享記憶體 */
  init_share_memory( merc_ipc, merc_ipc_2 );

  mudlog( LOG_INFO , "%s開始正常運作.", mud_name );

  /* 真正執行 merc 主程式 */
  merc_exec = TRUE;
  game_loop();
  merc_exec = FALSE;

  /* 結束 mud 工作 */
  for ( loop = 0; loop < MAX_PORT; loop++ )
  {
    if ( Control[loop] != ERRORCODE )
    {
      #ifdef __linux__
      shutdown( Control[loop], 2 );
      #endif

      close( Control[loop] );
      mudlog( LOG_INFO, "關閉連接埠 %5d 連線。", MudPort[loop] );
    }
  }

  #ifdef __linux__
  shutdown( Internal, 2 );
  #endif

  close( Internal );
  mudlog( LOG_INFO, "關閉網際連線 %5d。", internal_port );

  mudlog( LOG_INFO, "總輸出位元組 %d,%d 位元組.", WriteKiloBytes, WriteBytes );
  mudlog( LOG_INFO, "總輸入位元組 %d,%d 位元組.", ReadKiloBytes, ReadBytes );

  mudlog( LOG_INFO , "%s正常關閉. 謝謝使用." , mud_name );

  mudlog( LOG_INFO, "清除過期的玩家檔案." );
  system_cleanup();
  mudlog( LOG_INFO, "清除過期玩家檔案完畢." );

  return 0;
}

/* 起使化 socket */
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

  /* 建立一個 socket */
  if ( ( fd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
  {
    mudlog( LOG_INFO, strerror( errno )  );
    mudlog( LOG_ERR , "無法啟動 socket (%d)", port );
  }

  /* 設定 socket 的狀態 */
  if ( setsockopt( fd, SOL_SOCKET, SO_REUSEADDR
    ,( char * ) &optval, sizeof( optval ) ) < 0 )
  {
    mudlog( LOG_INFO, strerror( errno ) );
    mudlog( LOG_ERR , "無法設定 socket (通訊埠: %d)", port );
  }

#ifdef __linux__

  /* 把資料庫中名稱為 hostname 的資料給讀取進來 */
  if ( !( hp = gethostbyname( HostName ) ) )
  {
    mudlog( LOG_INFO, strerror( errno ) );
    mudlog( LOG_ERR , "無法啟動 %d gethostbyname", port );
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
    mudlog( LOG_ERR, "無法啟動 bind (%d)" , port );
  }

  if ( listen( fd, MAX_CONNECT ) < 0 )
  {
    #ifdef __linux__
    shutdown( fd, 2 );
    #endif

    close( fd );
    mudlog( LOG_INFO, strerror( errno ) );
    mudlog( LOG_ERR , "無法啟動等待連線 %d." , port );
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
  fd_set                  in_set;   /* 可讀取的集合 */
  fd_set                  out_set;  /* 可寫入的集合 */
  fd_set                  exc_set;  /*   例外的集合 */
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

  /* 主要的迴圈 */
  while ( !merc_down )
  {
    /* 保留一個檔案代碼以供使用 */
    if ( !fpReserve ) fopen( null_file, "r" );
    clear_reload_status();

    /* 設定時間 */
    gettimeofday( &last_time, NULL );
    current_time = ( time_t ) last_time.tv_sec;

    /* 檢查共享計憶體的 timer, 且刪除 idle 太久的共享計憶體*/
    inc_share_memory_timer();

    /* 清除檔案處理代碼集合中所有的位元 */
    FD_ZERO( &in_set  );
    FD_ZERO( &out_set );
    FD_ZERO( &exc_set );

    /* 設定檔案處理代碼集合中對應於某些檔案處理代碼的單獨位元 */
    FD_SET( Internal, &in_set );

    for ( maxdesc = loop = 0; loop < MAX_PORT; loop++ )
    {
      if ( Control[loop] != ERRORCODE )
      {
        FD_SET( Control[loop], &in_set );
        maxdesc = UMAX( Control[loop], Internal );
      }
    }

    /* 算出需要做檢查的最高位數字 */
    for ( man = descriptor_list; man; man = man->next )
    {
      if ( !verify_desc( man ) ) continue;

      maxdesc = UMAX( maxdesc, man->descriptor );
      FD_SET( man->descriptor, &in_set  );
      FD_SET( man->descriptor, &out_set );
      FD_SET( man->descriptor, &exc_set );
    }

    /* 加入網際連線, 並且加入時間 */
    for ( pNet = net_list; pNet; pNet = pNet->next )
    {
      /* 輸入方面 */
      if ( valid_socket( pNet, NET_IMPORT ) )
      {
        pNet->import_duration++;
        maxdesc = UMAX( maxdesc, ( connect = pNet->import ) );
        FD_SET( connect, &in_set  );
        FD_SET( connect, &exc_set );
      }

      /* 輸出方面 */
      if ( valid_socket( pNet, NET_EXPORT ) )
      {
        pNet->export_duration++;
        maxdesc = UMAX( maxdesc, ( connect = pNet->export ) );
        FD_SET( connect, &out_set );
        FD_SET( connect, &exc_set );
      }
    }

    /* 設定 select 等待的時間 */
    timeout.tv_sec  = 1;
    timeout.tv_usec = 0;

    #if !defined(sun)
    sigsetmask( mask );
    #endif

    if ( select( maxdesc+1, &in_set, &out_set, &exc_set, &timeout ) < 0 )
    {
      mudlog( LOG_INFO, strerror( errno ) );
      mudlog( LOG_CRIT, "select() 函數有問題." );
    }

    #if !defined(sun)
    sigsetmask( 0 );
    #endif

    /* 共享計憶體處理 */
    handle_share_memory();

    /* 是否有新的連線 ? */
    for ( loop = 0; loop < MAX_PORT; loop++ )
    {
      if ( Control[loop] != ERRORCODE && FD_ISSET( Control[loop], &in_set ) )
        new_descriptor( Control[loop], loop );
    }

    /* 是否有網際連線 */
    if ( FD_ISSET( Internal, &in_set ) ) launch_internal( Internal );

    /* 踢掉不正常的連線 */
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
          mudlog( LOG_INFO , "%s 連線不正常." , d->character->name );
        }

        d->outtop = 0;
        close_socket( d );
      }
    }

    /* 踢掉不正常的網際連線 */
    for ( pNet = net_list; pNet; pNet = pNet->next )
    {
      /* 輸入部份 */
      if ( valid_socket( pNet, NET_IMPORT ) )
      {
        if ( FD_ISSET( connect = pNet->import, &exc_set ) )
        {
          FD_CLR( connect, &in_set );
          mudlog( LOG_NET, "[網際連線] %s 輸入錯誤.", pNet->address );
          close_net_socket( pNet, NET_IMPORT );
        }
      }

      /* 輸出部份 */
      if ( valid_socket( pNet, NET_EXPORT ) )
      {
        if ( FD_ISSET( connect = pNet->export, &exc_set ) )
        {
          FD_CLR( connect, &out_set );
          mudlog( LOG_NET, "[網際連線] %s 輸出錯誤.", pNet->address );
          close_net_socket( pNet, NET_EXPORT );
        }
      }
    }

    /* 網際連線輸入輸出部份 */
    for ( pNet = net_list; pNet; pNet = pNet->next )
    {
      /* 輸入部份 */
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
            /* 消掉換行的字元 */
            while ( nRead > 2 )
            {
              if ( buf[nRead-1] != '\n' && buf[nRead-1] != '\r' ) break;
              nRead--;
            }

            if ( nRead < MAX_INPUT_LENGTH )
            {
              buf[nRead] = '\x0';

              pString = one_argument( buf, arg1 );

              /* 特殊命令 */
              if ( arg1[0] == '!' )
              {
                /* 累加計數器 */
                pNet->nRead++;
              }

              else
              {
                pString = one_argument( pString, arg2 );
                pString = one_argument( pString, arg3 );

                if ( arg1[0] && arg2[0] && arg3[0] )
                {
                  sprintf( information,
                    "由遙遠的友站%s的%s(%s)傳來的訊息﹕%s。\n\r"
                    , pNet->name, arg1, arg2, arg3 );

                  /* 累加計數器 */
                  pNet->nRead++;

                  /* 送給所有的玩家 */
                  send_to_all_char( information );

                 /* 是否要記錄 */
                 if ( pNet->log )
                   mudlog( LOG_NET, "%s:%s", pNet->address, buf );
                }
              }
            }
          }

          /* 讀取錯誤 */
          else if ( nRead == 0 )
          {
            mudlog( LOG_NET, "[網際連線] %s 讀取錯誤.", pNet->address );
            close_net_socket( pNet, NET_IMPORT );
          }

          else if ( errno != EWOULDBLOCK )
          {
            mudlog( LOG_NET, "[網際連線] %s 讀取錯誤.", pNet->address );
            close_net_socket( pNet, NET_IMPORT );
          }
        }
      }

      /* 輸出部份 */
      if ( valid_socket( pNet, NET_EXPORT ) )
      {
        int nWrite;
        int iLen;

        if ( ( iLen = str_len( pNet->message ) ) <= 0 ) continue;

        if ( ( nWrite = write( pNet->export, pNet->message, iLen ) ) < 0 )
        {
          mudlog( LOG_NET, "[網際連線] %s 寫入錯誤.", pNet->address );
          close_net_socket( pNet, NET_EXPORT );
        }

        pNet->message[0] = '\x0';
        pNet->nWrite++;
      }
    }

    /* 輸入 */
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
            mudlog( LOG_INFO , "%s 遠端讀取不正常." , d->character->name );
          }

          d->outtop = 0;
          close_socket( d );
          continue;
        }
      }

      /* 遞減玩家的計數器 */
      if ( d->tick_count > 0 ) d->tick_count--;

      /* 遞減玩家的 chat_error */
      if ( d->chat_error > 0 )
      {
        d->chat_error--;
        continue;
      }

      /* 遞減玩家的 wait_time */
      if ( d->character && d->character->wait > 0 )
      {
        d->character->wait--;
        continue;
      }

      /* 把玩家輸入的資料讀入玩家資料裡面 */
      read_from_buffer( d );

      /* 執行玩家的輸入 */
      switch( d->connected )
      {
      default :
        mudlog( LOG_DEBUG, "錯誤的狀態 %d.", d->connected );
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
            /* 是否有未輸出完的字串 */
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

    /* 自動觸發遊戲動作 */
    update_handler();

    /* 輸出 */
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
            mudlog( LOG_INFO , "%s 遠端輸出不正常." , d->character->name );
          }

          d->outtop = 0;
          close_socket( d );
        }
      }
    }

    /* 輸入看看是否錯誤太多 */
    for ( d = descriptor_list; d; d = d->next )
    {
      if ( !verify_desc( d ) || d->connected == CON_PLAYING ) continue;

      if ( d->fqdn_limit )
      {
        mudlog( LOG_INFO, "%s 沒有 FQDN 被拒絕連線.", d->host );
        write_to_descriptor( d, "你沒有 FQDN﹐拒絕連線﹐關閉連線﹗\n\r", 0 );
        d->outtop = 0;
        close_socket( d );
        continue;
      }

      /* 錯誤太多 */
      if ( d->error >= MaxLoginError )
      {
        mudlog( LOG_INFO , "%s 進入時錯誤太多.", d->host );
        write_to_descriptor( d,
          "\n\r\n\r你的錯誤太多了﹐請你稍後再進入。\n\r" , 0 );
        d->outtop = 0;
        close_socket( d );
        continue;
      }
    }

    /* 與時鐘同步 */
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
          mudlog( LOG_CRIT, "時間同步的函數 select() 有問題" );
        }
      }
    }

    /* 更新結構 */
    struct_updates();

    /* 增加系統計數器 */
    traceloop++;
  }

  RETURN_NULL();
}

/* 建立新的描述表 */
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
    mudlog( LOG_INFO, "New_descriptor: accept 函數有問題." );
    RETURN_NULL();
  }

  #if !defined(FNDELAY)
  #define FNDELAY O_NDELAY
  #endif

  if ( fcntl( desc, F_SETFL, FNDELAY ) < 0 )
  {
    mudlog( LOG_INFO, strerror( errno ) );
    mudlog( LOG_INFO, "New_descriptor: fcntl 函數有問題." );
    RETURN_NULL();
  }

  /* 連接近一個新的描述表 */
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
    mudlog( LOG_INFO, "New_descriptor: getpeername 函數有問題." );

    dnew->host    = str_dup( "(未知)" );
    dnew->address = str_dup( "(未知)" );
    dnew->naddr   = 0;
  }

  else
  {
    dnew->naddr = ntohl( sock.sin_addr.s_addr );
    addr        = dnew->naddr;

    sprintf( buf, "%d.%d.%d.%d",
        ( addr >> 24 ) & 0xFF, ( addr >> 16 ) & 0xFF,
        ( addr >>  8 ) & 0xFF, ( addr       ) & 0xFF );

    mudlog( LOG_INFO , "從位址 %s 有訊號過來" , buf );

    dnew->host      = str_dup( buf );
    dnew->address   = str_dup( buf );
    dnew->inet_addr = inet_addr( buf );

    /* 檢查是否被禁止連接進來 */
    for ( pBan = ban_list; pBan; pBan = pBan->next )
    {
      if ( !str_prefix( pBan->name , dnew->host ) )
      {
        write_to_descriptor( dnew, "對不起﹐你的位址被禁止連接過來。\n\r", 0 );
        mudlog( LOG_INFO, "%s 被禁止連線。", dnew->host );

        #ifdef __linux__
        shutdown( desc ,2 );
        #endif

        close( desc );

        /* 釋放結構的記憶體 */
        free_struct( dnew , STRUCT_DESCRIPTOR_DATA );
        RETURN_NULL();
      }
    }

    if ( GetFQDN )
    {
      slot = get_free_share_memory( dnew, INTERNAL_GETHOSTBYADDR );

      if ( slot < 0 )
      {
        write_to_descriptor( dnew, "對不起﹐記憶體不足﹐無法連線。\n\r", 0 );

        #ifdef __linux__
        shutdown( desc ,2 );
        #endif

        close( desc );

        /* 釋放結構的記憶體 */
        free_struct( dnew , STRUCT_DESCRIPTOR_DATA );
        RETURN_NULL();
      }

      /* 由子行程來查詢玩家的FDQN 和遠端使用者號碼 */
      switch( fork() )
      {
      case -1:

        SystemCrash = TRUE;
        clean_share_memory_address( slot );
        mudlog( LOG_INFO , strerror( errno ) );
        mudlog( LOG_CRIT , "fork() 函數有問題." );
        break;

       /* 子行程部份 */
      case 0:

        SystemCrash = TRUE;

        signal( SIGPIPE, SIG_IGN );
        signal( SIGCHLD, SIG_IGN );

        /* 關閉所有通訊 */
        close_communication();

        switch( fork() )
        {
        case -1 :

          SystemCrash = TRUE;
          clean_share_memory_address( slot );
          mudlog( LOG_INFO, strerror( errno ) );
          mudlog( LOG_CRIT, "fork() 函數有問題." );
          exit( 0 );

        case 0:
          {
            char                 buffer[ MAX_STRING_LENGTH ];
            char               * user;
            char               * address;
            struct sockaddr_in * sin = ( struct sockaddr_in * ) ( &sock );
            struct hostent     * from;

            SystemCrash = TRUE;

            /* 關閉所有通訊 */
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

      /* 父行程部份 */
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


  write_to_descriptor( dnew, "檢查連線位址﹐請稍待片刻..." , 0 );
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

/* 關閉一個 socket */
void close_socket( DESCRIPTOR_DATA * pDesc )
{
  CHAR_DATA       * ch;
  DESCRIPTOR_DATA * man;
  char              buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "close_socket" );

  if ( !pDesc || !verify_desc( pDesc ) )
  {
    mudlog( LOG_DEBUG, "close_socket: 來源不正確." );
    RETURN_NULL();
  }

  /* 關閉關聯的共享計憶體 */
  update_share_memory( pDesc );

  if ( pDesc->outtop > 0 ) process_output( pDesc, FALSE );

  if ( pDesc->snoop_by )
  {
    set_descriptor_stack( pDesc->snoop_by );
    write_to_buffer( pDesc->snoop_by, "你的目標已經離開遊戲了。\n\r", 0 );
    pDesc->snoop_by->stack = FALSE;
  }

  /* 將此人被監視清除 */
  for ( man = descriptor_list; man; man = man->next )
    if ( verify_desc( man ) && man->snoop_by == pDesc ) man->snoop_by = NULL;

  if ( ( ch = pDesc->character ) )
  {
    mudlog( LOG_INFO , "%s 突然失去連線.", ch->name );

    if ( ch->answer )
    {
      free_struct( ch->answer, STRUCT_ANSWER_DATA );
      ch->answer = NULL;
    }

    /* 釋放賭博結構資料 */
    extract_gamble( ch );
    ch->gamble = NULL;

    if ( pDesc->connected == CON_PLAYING )
    {
      act( "$n突然被一陣天上落下來的雷打到﹐失去知覺﹐面無表情﹗"
        , ch, NULL, NULL, TO_ROOM );

      if ( is_pk( ch ) )
      {
        switch( fight_info->status )
        {
        case PK_JOIN:

          /* 對手送回 */
          if ( fight_info->fight_1 && fight_info->fight_1 != ch )
          {
            char_from_room( fight_info->fight_1 );
            char_to_room( fight_info->fight_1
              , get_hometown( fight_info->fight_1 ) );

            /* 清除追蹤紀錄點 */
            clear_trace( fight_info->fight_1, TRUE );
          }

          if ( fight_info->fight_2 && fight_info->fight_2 != ch )
          {
            char_from_room( fight_info->fight_2 );
            char_to_room( fight_info->fight_2
              , get_hometown( fight_info->fight_2 ) );

            /* 清除追蹤紀錄點 */
            clear_trace( fight_info->fight_2, TRUE );
          }

          sprintf( buf, "%s偷偷落跑了﹐所以自動棄權﹗", mob_name( NULL, ch ) );
          talk_channel_2( buf, CHANNEL_PK, "" );

          if ( fight_info->fight_1 == ch ) fight_info->fight_1 = NULL;
          if ( fight_info->fight_2 == ch ) fight_info->fight_2 = NULL;

          char_from_room( ch );
          char_to_room( ch, get_hometown( ch ) );
          break;

        case PK_BET:

          /* 對手送回 */
          if ( fight_info->fight_1 && fight_info->fight_1 != ch )
          {
            char_from_room( fight_info->fight_1 );
            char_to_room( fight_info->fight_1
              , get_hometown( fight_info->fight_1 ) );

            /* 清除追蹤紀錄點 */
            clear_trace( fight_info->fight_1, TRUE );
          }

          if ( fight_info->fight_2 && fight_info->fight_2 != ch )
          {
            char_from_room( fight_info->fight_2 );
            char_to_room( fight_info->fight_2
              , get_hometown( fight_info->fight_2 ) );

            /* 清除追蹤紀錄點 */
            clear_trace( fight_info->fight_2, TRUE );
          }

          sprintf( buf, "%s偷偷落跑了﹐所以這場比賽不算﹗"
            , mob_name( NULL, ch ) );

          talk_channel_2( buf, CHANNEL_PK, "" );

          char_from_room( ch );
          char_to_room( ch, get_hometown( ch ) );

          init_fight( fight_info );
          fight_info->status = PK_DESERT;
          break;

        case PK_FIGHT:

          /* 把他的生命力降低﹐自動讓 damage 去處理 */
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

  /* 鎖定玩家檔案 */
  lock_file( pDesc );

  /* 釋放結構記憶體 */
  free_struct( pDesc , STRUCT_DESCRIPTOR_DATA );

  RETURN_NULL();
}

bool read_from_descriptor( DESCRIPTOR_DATA * d )
{
  int iStart;

  PUSH_FUNCTION( "read_from_descriptor" );

  /* 如果輸入緩衝區不是空的則回去 */
  if ( d->incomm[0] ) RETURN( TRUE );

  /* 檢查是否超出範圍 */
  if ( ( iStart = str_len( d->inbuf ) ) >= ( int ) sizeof( d->inbuf ) - 10 )
  {
    mudlog( LOG_INFO , "%s 輸入緩衝區已滿.", d->host );

    write_to_descriptor( d,
      "\n\r你的緩衝區已滿﹐系統只好把你強制斷線。\n\r", 0 );
    RETURN( FALSE );
  }

  /* 讀入輸入緩衝區 */
  for ( ;; )
  {
    int nRead;
    int iCount;

    if ( ( iCount = sizeof( d->inbuf ) - 10 - iStart ) <= 0 )
    {
      mudlog( LOG_INFO , "%s 輸入緩衝區已滿.", d->host );

      write_to_descriptor( d,
        "\n\r你的緩衝區已滿﹐系統只好把你強制斷線。\n\r", 0 );
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
      mudlog( LOG_INFO , "從輸入緩衝區讀到 EOF (End Of File)." );
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

/* 從輸入緩衝區讀進來一行資料 */
void read_from_buffer( DESCRIPTOR_DATA * d )
{
  extern bool tick_permit;
  int         i;
  int         j;
  int         k;

  PUSH_FUNCTION( "read_from_buffer" );

  /* 如果輸入緩衝區不是空的則回去, 因為尚未處理上次的指令. */
  if ( d->incomm[0] ) RETURN_NULL();

  /* 檢查是否要鍵入計數器的命令 */
  if ( d->connected  == CON_PLAYING
    && d->tick_count == 0
    && d->character
    && ( tick_permit || IS_IMMORTAL( d->character ) ) )
  {
    /* 如果大於系統的預設值 */
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

  /* 搜尋至少要有輸入一列 */
  for ( i = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++ )
  {
    if ( i >= ( sizeof( d->inbuf ) - 2 ) )
    {
      mudlog( LOG_DEBUG, "read_from_buffer: 輸入端異常." );
      d->inbuf[0]  = '\x0';
      d->incomm[0] = '\x0';
      RETURN_NULL();
    }

    if ( d->inbuf[i] == '\x0' ) RETURN_NULL();
  }

  /* 分隔分號 */
  spilt_semicolon( d );

  /* 濾掉不正常的字 */
  for ( i = 0; d->inbuf[i]; i++ )
  {
    switch( d->inbuf[i] )
    {
    default:
      continue;

    case '\x0e': /* 會使中文花掉 */
    case '\a'  : /* 鈴聲         */
    case '\x09': /* Tab 鍵       */

      d->inbuf[i] = '+';
      break;
    }
  }

  for ( i = k = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++ )
  {
    /* 檢查是否超出陣列的大小 */
    if ( k >= sizeof( d->incomm ) - 10 )
    {
      write_to_descriptor( d, "輸入行太長。\n\r", 0 );

      /* 把剩餘的字消掉 */
      for ( ; d->inbuf[i]; i++ )
      {
        if ( i >= sizeof( d->inbuf ) - 2 )
        {
          mudlog( LOG_DEBUG, "read_from_buffer: 輸入端異常." );
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

    /* 是否為倒退鍵 */
    if ( d->inbuf[i] == '\b' )
    {
      if ( k > 0 ) k--;
    }

    else
    {
      d->incomm[k++] = d->inbuf[i];
    }
  }

  /* 把輸入緩衝區做個調整 */
  while ( d->inbuf[i] == '\n' || d->inbuf[i] == '\r' )
  {
    if ( ++i >= sizeof( d->inbuf ) - 2 )
    {
      mudlog( LOG_DEBUG, "read_from_buffer: 輸入端異常." );
      d->inbuf[0]  = '\x0';
      d->incomm[0] = '\x0';
      RETURN_NULL();
    }
  }

  for ( j = 0; ( d->inbuf[j] = d->inbuf[i+j] ); j++ )
  {
    if ( ( i+j ) >= sizeof( d->inbuf ) - 2 )
    {
      mudlog( LOG_DEBUG, "read_from_buffer: 輸入端異常." );
      d->inbuf[0]  = '\x0';
      d->incomm[0] = '\x0';
      RETURN_NULL();
    }
  }

  if ( k == 0 ) d->incomm[k++] = ' ';

  /* 把讀入的輸入字串加入結束符號 */
  d->incomm[k] = '\x0';

  /* 翻譯巨集指令成指令 */
  translate_alias( d , d->incomm );

  /* 翻譯 # 命令 */
  translate_command( d );

  /* 看看是否重複次數太多 */
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
        mudlog( LOG_INFO , "%s 輸入重複太多.", d->host );

        write_to_descriptor( d, "\n\r你輸入的字串重複太多。\n\r" , 0 );
        str_cpy( d->incomm, "quit\x0" );
      }
    }
  }

  if ( d->connected == CON_PLAYING )
  {
    /* 取代 ! 的動作 */
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

    /* 否則調整輸入的記錄 */
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

/* 低階輸出函數 */
bool process_output( DESCRIPTOR_DATA * d, bool fPrompt )
{
  extern bool   merc_down;
  char          buf[MAX_INPUT_LENGTH * 2];
  CHAR_DATA   * ch;
  CHAR_DATA   * victim;

  PUSH_FUNCTION( "process_output" );

  /* 輸出提示列 */
  if ( fPrompt && !merc_down && d->connected == CON_PLAYING )
  {
    /* 是否有未輸出完畢的 */
    if ( d->showstr_point )
    {
      char buffer[ MAX_STRING_LENGTH ];

      sprintf( buffer, "\e[80D\e[0m--More--(%2d%%)"
         , 100 - 100 * ( d->showstr_point - d->showstr_head ) /
           UMAX( 1, str_len( d->showstr_head ) ) );

      write_to_buffer( d, buffer , 0 );
    }

    /* 已經先前的輸出完畢了 */
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

  /* 如果輸出是空的則快速離開 */
  if ( d->outtop == 0 ) RETURN( TRUE );

  /* 監視程序控制輸出 */
  if ( d->snoop_by )
  {
    set_descriptor_stack( d->snoop_by );
    write_to_buffer( d->snoop_by, "監視﹕", 0 );
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
    mudlog( LOG_DEBUG, "bust_fight_message: 來源不正確." );
    RETURN_NULL();
  }

  if ( get_curr_hit( ch ) > 0 && get_curr_hit( victim ) > 0 )
  {
    /* 印出自己的狀況 */
    sprintf( buf, "(\e[1;36m你\e[0m%s\e[0m)\n\r"
      , status_message( ch ) );

    write_to_buffer( ch->desc , buf, 0 );

    /* 印出對手的狀況 */
    sprintf( buf , "(\e[1;36m%s\e[0m%s\e[0m)\n\r\n\r"
      , mob_name( ch, victim ), status_message( victim ) );

    write_to_buffer( ch->desc , buf, 0 );
  }

  RETURN_NULL();
}

/* 送出 Prompt 給玩家 */
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
      mudlog( LOG_DEBUG, "bust_a_prompt: 超出範圍." );
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

    /* 生命值( 以短長條狀顯示 ) */
    if ( !str_cmp( new_format , "hps" ) )
    {
      sprintf( buf2, "\e[1;32m%s\e[0m"
        , return_percentage( ch , ch->hit , get_curr_hit( ch ) , 5 ) );
      str += 3;
    }

    /* 生命值( 以長長條狀顯示 ) */
    else if ( !str_cmp( new_format , "hpl" ) )
    {
      sprintf( buf2, "\e[1;32m%s\e[0m"
        , return_percentage( ch , ch->hit , get_curr_hit( ch ) , 10 ) );
      str += 3;
    }

    /* 你現在的生命值( 以數字顯示 ) */
    else if ( !str_cmp( new_format , "hp1" ) )
    {
      if ( ch->hit >= get_curr_hit( ch ) )
        sprintf( buf2, "\e[1;31m%d\e[0m", ch->hit );
      else
        sprintf( buf2, "\e[31m%d\e[0m", ch->hit );
      str += 3;
    }

    /* 你最大的生命值( 以數字顯示 ) */
    else if ( !str_cmp( new_format , "hp2" ) )
    {
      sprintf( buf2, "\e[1;31m%d\e[0m" , get_curr_hit( ch ) );
      str += 3;
    }

    /* 法力值( 以長長條狀顯示 ) */
    else if ( !str_cmp( new_format , "mps" ) )
    {
      sprintf( buf2, "\e[1;32m%s\e[0m"
        , return_percentage( ch , ch->mana , get_curr_mana( ch ) , 5 ) );
      str += 3;
    }

    /* 法力值( 以短長條狀顯示 ) */
    else if ( !str_cmp( new_format , "mpl" ) )
    {
      sprintf( buf2, "\e[1;32m%s\e[0m"
        , return_percentage( ch , ch->mana , get_curr_mana( ch ) , 10 ) );
      str += 3;
    }

    /* 你現在的法力值( 以數字顯示 ) */
    else if ( !str_cmp( new_format , "mp1" ) )
    {
      if ( ch->mana >= get_curr_mana( ch ) )
        sprintf( buf2, "\e[1;36m%d\e[0m" , ch->mana );
      else
        sprintf( buf2, "\e[1;34m%d\e[0m" , ch->mana );
      str += 3;
    }

    /* 你最大的法力值( 以數字顯示 ) */
    else if ( !str_cmp( new_format , "mp2" ) )
    {
      sprintf( buf2, "\e[1;36m%d\e[0m" , get_curr_mana( ch ) );
      str += 3;
    }

    /* 移動力( 以短長條狀顯示 ) */
    else if ( !str_cmp( new_format , "mvs" ) )
    {
      sprintf( buf2, "\e[1;32m%s\e[0m"
        , return_percentage( ch , ch->move , get_curr_move( ch ) , 5 ) );
      str += 3;
    }

    /* 移動力( 以長長條狀顯示 ) */
    else if ( !str_cmp( new_format , "mvl" ) )
    {
      sprintf( buf2, "\e[1;32m%s\e[0m"
        , return_percentage( ch , ch->move , get_curr_move( ch ) , 10 ) );
      str += 3;
    }

    /* 你現在的體力值( 以數字顯示 ) */
    else if ( !str_cmp( new_format , "mv1" ) )
    {
      if ( ch->move >= get_curr_move( ch ) )
        sprintf( buf2, "\e[1;33m%d\e[0m" , ch->move );
      else
        sprintf( buf2, "\e[33m%d\e[0m" , ch->move );
      str += 3;
    }

    /* 你最大的體力值( 以數字顯示 ) */
    else if ( !str_cmp( new_format , "mv2" ) )
    {
      sprintf( buf2, "\e[1;33m%d\e[0m" , get_curr_move( ch ) );
      str += 3;
    }

    /* 等級 */
    else if ( !str_cmp( new_format , "lvl" ) )
    {
      sprintf( buf2, "\e[1;37m%d\e[0m" , ch->level );
      str += 3;
    }

    /* 陣營 */
    else if ( !str_cmp( new_format , "ali" ) )
    {
      sprintf( buf2, "%s\e[0m"
        , IS_GOOD(ch) ? "\e[32m善良" : IS_EVIL(ch) ?
         "\e[34m邪惡" : "\e[36m中立" );
      str += 3;
    }

    /* 金錢 */
    else if ( !str_cmp( new_format , "gol" ) )
    {
      sprintf( buf2, "\e[1;33m%d\e[0m" , ch->gold );
      str += 3;
    }

    /* 年齡 */
    else if ( !str_cmp( new_format , "age" ) )
    {
      sprintf( buf2, "\e[34m%d\e[0m" , get_age( ch ) );
      str += 3;
    }

    /* 力量 */
    else if ( !str_cmp( new_format , "str" ) )
    {
      sprintf( buf2, "\e[31m%d\e[0m" , get_curr_str( ch ) );
      str += 3;
    }

    /* 智力 */
    else if ( !str_cmp( new_format , "int" ) )
    {
      sprintf( buf2, "\e[32m%d\e[0m" , get_curr_int( ch ) );
      str += 3;
    }

    /* 學識 */
    else if ( !str_cmp( new_format , "wis" ) )
    {
      sprintf( buf2, "\e[33m%d\e[0m" , get_curr_wis( ch ) );
      str += 3;
    }

    /* 敏捷 */
    else if ( !str_cmp( new_format , "dex" ) )
    {
      sprintf( buf2, "\e[34m%d\e[0m" , get_curr_dex( ch ) );
      str += 3;
    }

    /* 體格 */
    else if ( !str_cmp( new_format , "con" ) )
    {
      sprintf( buf2, "\e[35m%d\e[0m" , get_curr_con( ch ) );
      str += 3;
    }

    /* 你所剩下的經驗值 */
    else if ( !str_cmp( new_format, "exl" ) )
    {
      sprintf( buf2, "\e[1;33m%d\e[0m",
        kill_table[ URANGE( 1, ch->level, MAX_LEVEL -1 )].exp - ch->exp );
      str += 3;
    }

    /* 你所擁有的經驗值 */
    else if ( !str_cmp( new_format , "exp" ) )
    {
      sprintf( buf2, "\e[36m%d\e[0m" , ch->exp );
      str += 3;
    }

    /* 你所在的房間名稱 */
    else if ( !str_cmp( new_format , "roo" ) )
    {
      if ( ch->in_room ) sprintf( buf2, "\e[1;35m%s\e[0m", ch->in_room->name );
      else               sprintf( buf2, "未知名的房間" );
      str += 3;
    }

    /* 顯示你可攜帶的物品樣數 */
    else if ( !str_cmp( new_format , "thi" ) )
    {
      sprintf( buf2, "\e[36m%d\e[0m" , can_carry_n( ch ) );
      str += 3;
    }

    /* 顯示你可攜帶的物品重量 */
    else if ( !str_cmp( new_format , "wei" ) )
    {
      sprintf( buf2, "\e[1;34m%d\e[0m" , can_carry_w( ch ) );
      str += 3;
    }

    /* 換行 */
    else if ( !str_cmp( new_format , "nex" ) )
    {
      str_cpy( buf2, "\n\r" );
      str += 3;
    }

    /* 你的命中率 */
    else if ( !str_cmp( new_format , "hit" ) )
    {
      sprintf( buf2, "\e[1;31m%d\e[0m" , get_hitroll( ch ) );
      str += 3;
    }

    /* 你的防護力 */
    else if ( !str_cmp( new_format , "arm" ) )
    {
      sprintf( buf2, "\e[1;32m%d\e[0m" , get_ac( ch ) );
      str += 3;
    }

    /* 你的攻擊傷害力 */
    else if ( !str_cmp( new_format , "dam" ) )
    {
      sprintf( buf2, "\e[1;33m%d\e[0m" , get_damroll( ch ) );
      str += 3;
    }

    /* 你所在房間的編號( 神族指令 ) */
    else if ( !str_cmp( new_format , "rrr" ) )
    {
      if ( IS_IMMORTAL( ch ) && ch->in_room )
         sprintf( buf2, "\e[1;37m%d\e[0m", ch->in_room->vnum );
      else str_cpy( buf2, " " );
      str += 3;
    }

    /* 你所在區域的名稱( 神族指令 ) */
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
      mudlog( LOG_DEBUG , "bust_a_prompt: 輸出字串超過系統字串長度" );
  }

  *point = '\x0';
  write_to_buffer( ch->desc, buf, 0 );
  RETURN_NULL();
}

/* 把輸出的東西加到輸出緩衝區中 */
void write_to_buffer( DESCRIPTOR_DATA * d, const char * txt, int length )
{
  CHAR_DATA * victim;
  char      * target;
  char      * outbuf;
  int         outsize;
  bool        ansi;
  const char  warning[] =
    "\n\r\n\r\e[1;31m**你的輸出緩衝區太大.系統自動重"
    "新設定你的緩衝區**\e[0m\n\r\n\r\0";

  PUSH_FUNCTION( "write_to_buffer" );

  /* 如果呼叫者未指定長度, 則設定長度 */
  if ( length <= 0 ) length = str_len( txt );

  if ( d->stack
    || ( d->connected == CON_PLAYING && d->edit_mode != EDIT_NONE ) )
  {
    save_to_stack( d, txt, length );
    RETURN_NULL();
  }

  /* 如果沒有 \n\r if 則啟始化它. */
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

  /* 如果長度不夠則配置大一點給它 */
  while ( d->outtop + length >= d->outsize - 1 )
  {
    /* 檢查是否超出配置記憶體的極限 */
    if ( ( outsize = ( d->outsize * 2 ) ) < MAX_OUTPUT_LENGTH )
    {
      outbuf = ( char * ) alloc_string( d->outsize * 2 );
      strncpy( outbuf, d->outbuf, d->outtop );
      *( outbuf + d->outtop ) = '\x0';
      free_string( d->outbuf );
      d->outbuf   = outbuf;
      d->outsize *= 2;
    }

    /* 超出極限則歸零 */
    else
    {
      outbuf  = ( char * ) alloc_string( ( outsize = 2000 ) );
      *outbuf = '\x0';
      length  = str_len( ( txt = warning ) );
      free_string( d->outbuf );

      d->outbuf  = outbuf;
      d->outtop  = 0;
      d->outsize = 2000;
      mudlog( LOG_INFO , "[WARN] %s 輸出緩衝區太大, 系統自動修正"
        , d->character->name );
      break;
    }
  }

  /* 過濾 ANSI 輸出字元 */
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
    mudlog( LOG_DEBUG, "write_to_buffer: 輸出錯誤." );

  RETURN_NULL();
}

/* 最低階輸出的函數, 輸出一段文字到檔案的描述者. */
/* 如果這裡有發現問題的話(例如 ofind all), 則試試把 MAX_BLOCK_SIZE 減少 */
/* 他是利用把遠端想向成一個檔案, 之後作寫入檔案的動作完成低階輸出 */
/* desc 便是這個遠端檔案的 handler */
bool write_to_descriptor( DESCRIPTOR_DATA * desc, char * txt, int length )
{
  int iStart;
  int nWrite;
  int nBlock;

  PUSH_FUNCTION( "write_to_descriptor" );

  if ( !desc )
  {
    mudlog( LOG_DEBUG, "write_to_descriptor: 沒有來源." );
    RETURN( FALSE );
  }

  if ( length <= 0 ) length = str_len( txt );
  for ( iStart = 0; iStart < length; iStart += nWrite )
  {
    nBlock = UMIN( length - iStart, MAX_BLOCK_SIZE );
    if ( ( nWrite = write( desc->descriptor, txt + iStart, nBlock ) ) < 0 )
    {
      mudlog( LOG_INFO, strerror( errno ) );
      mudlog( LOG_INFO, "write_to_descriptor: 寫入錯誤" );
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

/* 處理尚未連線完畢的 socket */
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

    mudlog( LOG_DEBUG , "Nanny: 未定義的 d->connected %d.", d->connected );
    close_socket( d );
    RETURN_NULL();

  /* 取得遠方的 FQDN 以及名字 */
  case CON_GET_FQDN:

    if ( d->check_fqdn == FALSE )
    {
      sprintf( temp , "\b%c" , inflect[ d->inflect++ % 4 ] );
      write_to_descriptor( d, temp , 0 );
    }

    else
    {
      write_to_descriptor( d, "\b \n\r檢查遠端連線資料﹐請稍後..." , 0 );
      d->connected    = CON_GET_FINGER;
      d->inflect      = 0;
      d->check_finger = TRUE;

      /* 缺乏遠端簽入名稱 */
      if ( !d->remote || !d->remote[0] )
      {
        str_cpy( d->finger_data , "缺乏遠端簽入姓名。" );
        d->check_finger = TRUE;
      }

      else
      {
        slot = get_free_share_memory( d, INTERNAL_FINGER    );

        if ( slot < 0 )
        {
          write_to_descriptor( d, "對不起﹐記憶體不足﹐無法連線。\n\r", 0 );
          close_socket( d );
          RETURN_NULL();
        }

        if ( CheckServer )
        {
          serv = get_free_share_memory( d, INTERNAL_CHECKSERV );

          if ( serv < 0 )
          {
            write_to_descriptor( d, "對不起﹐記憶體不足﹐無法連線。\n\r", 0 );
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
          mudlog( LOG_INFO, "fork() 函數有問題." );
          str_cpy( d->finger_data , "執行 fork 函數有問題." );
          clean_share_memory_address( slot );
          d->check_finger = TRUE;
          break;

        /* 子行程部份 */
        case 0 :

          SystemCrash = TRUE;

          /* 關閉所有通訊 */
          close_communication();

          signal( SIGPIPE, SIG_IGN );
          signal( SIGCHLD, SIG_IGN );

          switch( fork() )
          {
          case -1:

            SystemCrash = TRUE;
            mudlog( LOG_INFO, strerror( errno ) );
            mudlog( LOG_INFO, "fork() 函數有問題." );
            set_share_memory_text( slot , "程式無法開啟 fork." );
            exit( 0 );

          case 0:
            {
              char               line[ MAX_STRING_LENGTH ];
              struct sockaddr_in sin;
              int                cfd;
              FILE             * fp;

              SystemCrash = TRUE;

              /* 關閉所有通訊 */
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
                set_share_memory_text( slot , "程式無法開啟 fdopen﹗" );
                exit( 0 );
              }

              setbuf( fp, ( char * ) 0 );
              cfd = fileno( fp );

              if ( connect( cfd, ( struct sockaddr *) & sin, sizeof( sin ) ) )
              {
                close( cfd );
                set_share_memory_text( slot, "查詢端沒有開放查詢服務﹗" );
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

        /* 父行程部份, 只需要記錄行程號碼來辨識 */
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
      /* 送出 greeting 的訊息, 並且輸出遊戲的狀態 */
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
          mudlog( LOG_DEBUG, "nanny: 找不到進站畫面." );
          write_to_descriptor( d, "沒有進站畫面\n\r", 0 );
        }

        /* 印出遊戲的狀態 */
        show_game_status( d );

        /* 開始輸入基本資料了 */
        write_to_descriptor( d , "請輸入你的英文名字 ﹕" , 0 );
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
      write_to_descriptor( d, "\b正確"  , 0 );
      break;

    case FILE_NEW:
      write_to_descriptor( d, "\b新玩家", 0 );

      if ( NewLock )
      {
        write_to_descriptor( d,
          "\n\r目前遊戲沒有辦法新增玩家, 請稍後再試!\n\r", 0 );
        close_socket( d );
        RETURN_NULL();
      }

      break;

    default:

      write_to_descriptor( d, "\b不正確", 0 );

      mudlog( LOG_DEBUG, "nanny: %s 檔案不正確.", d->username );

      if ( AutoBackup )
      {
        recover_file( d );
      }

      else
      {
        sprintf( temp, "\n\r你的檔案已經損毀﹐請至 \e[1;32m%s\e[0m 告知"
          "給管理者得知﹗\a\a\a\n\r", bbs_name );

        write_to_descriptor( d, temp, 0 );
      }

      mudlog( LOG_BADFILE, "%s 檔案損毀.", d->username );
      close_socket( d );
      RETURN_NULL();
    }

    fOld = load_char_obj( d, d->username );
    ch   = d->character;

    if ( IS_SET( ch->pcdata->suspect , PLR_COPYEQ ) && !IS_IMMORTAL( ch ) )
    {
      sprintf( temp, "\n\r\a\a\a你被拒絕進入﹐因為你涉嫌拷貝裝備。"
                     "請到 \e[1;32m%s\e[0m 去說明一下。\n\r", bbs_name );

      write_to_descriptor( d, temp, 0 );
      mudlog( LOG_SUSPECT, "%s 因涉嫌拷貝裝備﹐禁止連線﹗", d->username );
      mudlog( LOG_INFO , "%s 從 %s 進來, 但他被拒絕進入, 因為涉嫌拷貝裝備."
         , ch->name , d->host );

      close_socket( d );
      RETURN_NULL();
    }

    if ( multilogin
      && check_multi_login( d )
      && !is_server( d->host )
      && !IS_IMMORTAL( ch ) )
    {
      mudlog( LOG_INFO, "%s 多重連線.", d->host );

      write_to_descriptor( d,
        "\n\r系統發現你的位址有多重連線﹐請從別的位址連線﹐謝謝﹗\n\r", 0 );
      close_socket( d );
      RETURN_NULL();
    }

    if ( ch->pcdata->deny != 0 && !IS_IMMORTAL( ch ) )
    {
      mudlog( LOG_INFO , "%s 從 %s 進來, 但他被拒絕進入."
        , ch->name , d->host );

      sprintf( temp, "\n\r\a\a\a你被拒絕進入﹐請和系統管理者商量吧﹐"
               "或是到 \e[1;32m%s\e[0m 去詢問一下。\n\r" , bbs_name );

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

        sprintf( buf, "你還在冬眠耶﹐你要%s天後才後醒來喔﹗\n\r", days );
        write_to_descriptor( d, buf, 0 );
        close_socket( d );
        RETURN_NULL();
      }

      else
      {
        write_to_descriptor( d, "你從冬眠狀態中醒了過來﹐好睏喔﹗\n\r", 0 );
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
        mudlog( LOG_INFO , "%s 因為鎖定而無法連線." , ch->name );
        write_to_buffer( d, "對不起﹐遊戲即將重新開機﹐請稍後再來。\n\r", 0 );
        close_socket( d );
        RETURN_NULL();
      }
    }

    if ( fOld == FILE_CORRECT )
    {
      /* 老玩家 */
      sprintf( buf, "嗨﹗\e[1;32m%s\e[0m﹐你上次由 \e[1;32m%s\e[0m 連接進來\n\r"
                    "請輸入你的密碼﹕" , ch->cname , ch->pcdata->lasthost );
      write_to_buffer( d, buf , 0 );
      write_to_buffer( d, echo_off_str, 0 );
      d->connected = CON_GET_OLD_PASSWORD;
      RETURN_NULL();
    }

    else
    {
      /* 新玩家 */
      if ( check_playing( d, ch->name ) ) RETURN_NULL();

      sprintf( buf, "這是你要的名字嗎﹖ %s (\e[1;32mY\e[0m/\e[1;32mN\e[0m) "
        , d->username );

      write_to_buffer( d, buf, 0 );
      d->connected = CON_CONFIRM_NEW_NAME;
      RETURN_NULL();
    }
    break;

  /* 檢查檔案 */
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

  /* 取得姓名 */
  case CON_GET_NAME :

    if ( !argument[0] )
    {
      write_to_buffer( d , "請輸入你的英文名字 ﹕" , 0 );
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
        write_to_buffer( d, "你的舊英文名稱可能與目前線上的新區域相衝突"
                            "﹐關閉連線﹗\n\r", 0 );
        close_socket( d );
        RETURN_NULL();
      }

      else
      {
        write_to_buffer( d ,
          "\e[1;31m你的名字不被系統所接受﹐請重新輸入﹗\n\r\e[0m"
          "請輸入你的姓名﹕" , 0 );
      }

      d->error++;
      RETURN_NULL();
    }

    if ( check_logon( argument ) )
    {
      write_to_buffer( d, "有相同的名稱的玩家在輸入密碼﹐請重新輸入﹗\n\r"
        "請輸入你的姓名﹕" , 0 );
      d->error++;
      RETURN_NULL();
    }

    /* 把玩家的英文名稱拷貝到描述表中, 之後會使用到 */
    argument[sizeof( d->username ) - 1 ] = '\x0';
    str_cpy( d->username , argument );

    /* 送出字串 */
    sprintf( buf , "檢查 %s 的檔案..." , argument );
    write_to_buffer( d, buf , 0 );

    slot = get_free_share_memory( d, INTERNAL_CHECKFILE );

    if ( slot < 0 )
    {
      write_to_descriptor( d, "對不起﹐記憶體不足﹐無法連線。\n\r", 0 );
      close_socket( d );
      RETURN_NULL();
    }

    /* 由子行程來查詢玩家的檔案 */
    switch( fork() )
    {
    case -1:
      SystemCrash = TRUE;
      clean_share_memory_address( slot );
      mudlog( LOG_INFO , strerror( errno ) );
      mudlog( LOG_CRIT , "fork() 函數有問題." );
      break;

    /* 子行程部份 */
    case 0:

      SystemCrash = TRUE;

      /* 關閉所有通訊 */
      close_communication();

      signal( SIGPIPE, SIG_IGN );
      signal( SIGCHLD, SIG_IGN );

      switch( fork() )
      {
      case -1 :

        SystemCrash = TRUE;
        clean_share_memory_address( slot );
        mudlog( LOG_INFO, strerror( errno ) );
        mudlog( LOG_CRIT, "fork() 函數有問題." );
        exit( 0 );

      case 0:
        {
          char   buffer[MAX_INPUT_LENGTH];

          SystemCrash = TRUE;

          /* 關閉所有通訊 */
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

    /* 父行程部份 */
    default:
      break;
    }

    d->connected = CON_CHECK_FILE;
    break;

  /* 取得性別 */
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
      write_to_buffer( d, "請輸入 \e[1;32m1\e[0m(男性) 或 "
        "\e[1;32m2\e[0m(女性)。", 0 );

      RETURN_NULL();
    }

    write_to_buffer( d, "請選取輸出模式﹕(\e[1;32m1\e[0m) Merc 模式"
      " (\e[1;32m2\e[0m) LP 模式﹕", 0 );

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
      write_to_buffer( d, "請選取輸出模式﹕(\e[1;32m1\e[0m) Merc 模式 "
        "(\e[1;32m2\e[0m) LP 模式﹕", 0 );

      RETURN_NULL();
    }

    write_to_buffer( d, "請選擇守護神出現頻率﹕(\e[1;32m1\e[0m) 經常"
      " (\e[1;32m2\e[0m) 普通 (\e[1;32m3\e[0m) 很少"
      "(\e[1;32m4\e[0m) 從不﹕ ", 0 );

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
          send_to_char( "\e[1;32m請按 \e[1;33mENTER\e[1;32m 繼續遊戲\e[0m", ch );
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
      write_to_buffer( d, "請選擇守護神出現頻率﹕(\e[1;32m1\e[0m) 經常 "
        "(\e[1;32m2\e[0m) 普通 (\e[1;32m3\e[0m) 很少"
        "(\e[1;32m4\e[0m) 從不﹕ ", 0 );

      RETURN_NULL();
    }

    write_to_buffer( d, "請輸入你的中文名字﹕" , 0 );
    d->connected = CON_GET_CNAME;
    break;

  case CON_GET_OLD_PASSWORD:

    write_to_buffer( d, "\n\r", 0 );

    if ( str_cmp( crypt( argument, ch->pcdata->pwd ), ch->pcdata->pwd ) )
    {
      DESCRIPTOR_DATA * man;
      char              address[MAX_STRING_LENGTH];

      str_cpy( address, host_name( d ) );

      /* 通知線上的人, 若是在線上 */
      for ( man = descriptor_list; man; man = man->next )
      {
        if ( verify_desc( man )
          && man != d
          && man->connected == CON_PLAYING
          && man->character
          && !str_cmp( man->character->name, ch->name ) )
        {
          act( "$B$B$2有人企圖以你的名稱從$1$t$2地方連進來﹐但是密碼錯誤﹗$0"
            , man->character, address, NULL, TO_CHAR );
          break;
        }
      }

      mudlog( LOG_FAILPASS , "%s(%s) 密碼錯誤.", ch->name, address );
      write_to_buffer( d, "密碼錯誤﹐請重新進入。\n\r", 0 );

      /* 記錄到密碼錯誤記錄檔 */
      fwrite_failcode( ch, address );

      close_socket( d );
      RETURN_NULL();
    }

    if ( !check_limit_ip( d ) )
    {
      mudlog( LOG_INFO, "[Limit] %s 限制連線.", ch->name );
      write_to_buffer( d, "對不起﹐這位玩家限制連線位址﹐"
        "但這個位址是非法的﹗\n\r", 0 );

      close_socket( d );
      RETURN_NULL();
    }

    write_to_buffer( d, echo_on_str, 0 );

    if ( d->lock_file )
    {
      mudlog( LOG_INFO, "%s 檔案變動, 強制斷線.", ch->name );
      write_to_buffer( d, "對不起﹐你的檔案有變動﹐請重新進入。\n\r", 0 );
      close_socket( d );
      RETURN_NULL();
    }

    /* 清除輸入緩衝區的資料 */
    d->incomm[0] = '\x0';
    d->inbuf [0] = '\x0';

    if ( check_reconnect( d, TRUE ) )   RETURN_NULL();
    if ( check_playing( d, ch->name ) ) RETURN_NULL();

    mudlog( LOG_INFO , "%s 從 %s 連進來.", ch->name, host_name( d ) );

    if ( imm_list && !IS_IMMORTAL( ch ) && get_total_user() > max_connect )
    {
      mudlog( LOG_INFO , "%s 連接時超過上線人數." , ch->name );
      write_to_buffer( d, "對不起﹐目前連線人數太多﹐請稍後再連接。\n\r", 0 );
      close_socket( d );
      RETURN_NULL();
    }

    /* 檢查屬性過高, 包含讀入的屬性修飾值 */
    if ( !IS_IMMORTAL( ch )
      && ( get_curr_str( ch ) > HUGE_ATTR
      || get_curr_int( ch ) > HUGE_ATTR
      || get_curr_wis( ch ) > HUGE_ATTR
      || get_curr_dex( ch ) > HUGE_ATTR
      || get_curr_con( ch ) > HUGE_ATTR ) )
    {
      write_to_buffer( d, "你的全體屬性有問題, 請跟系統管理者聯絡, 或"
        "逕行寫信到 lc@mud.ch.fju.edu.tw, 謝謝!.", 0 );
      close_socket( d );
      RETURN_NULL();
    }

    if ( get_home( ch ) )
    {
      lines               = ch->pcdata->pagelen;
      ch->pcdata->pagelen = 20;
      if ( IS_IMMORTAL( ch ) ) send_to_char( welcome_immortal, ch );

      send_to_char( welcome_message, ch );
      send_to_char( "\e[1;32m請按 \e[1;33mENTER\e[1;32m 繼續遊戲\e[0m", ch );

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
        mudlog( LOG_INFO , "新玩家 %s 連接時超過上線人數." , ch->name );
        write_to_buffer( d, "連線人數太多﹐請稍後再連接。\n\r", 0 );
        close_socket( d );
        RETURN_NULL();
      }

      if ( !imm_list )
      {
        sprintf( buf, "\a\a\a\e[1;32m你是第一個新玩家﹐你將被任命為超級"
          "管理者﹐請小心地給 %s 一個密碼﹕\e[0m %s", ch->name, echo_off_str );
      }

      else
      {
        sprintf( buf, "你是一個新玩家﹐請給 %s 一個密碼﹕ %s"
          , ch->name, echo_off_str );
      }

      write_to_buffer( d, buf, 0 );
      d->connected = CON_GET_NEW_PASSWORD;
      break;

    case 'n': case 'N':

      write_to_buffer( d, "好吧﹗請問你的名字是什麼﹖", 0 );
      free_char( d->character );
      d->character = NULL;
      d->connected = CON_GET_NAME;
      d->error++;
      break;

    default:
      write_to_buffer( d, "請輸入 [\e[1;32mY\e[0m] 或是 [\e[1;32mN\e[0m]﹖", 0 );
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
        ? "你的密碼太簡單了﹐請設定包涵數字和字母且五個字以上的密碼﹗"
          "\n\r請輸入密碼﹕"
        : "你的密碼太簡單了﹐請設定五個字以上的密碼﹗\n\r請輸入密碼﹕";

      write_to_buffer( d, pMessage, 0 );

      d->error++;
      RETURN_NULL();
    }

    pwdnew = crypt( argument, ch->name );
    for ( p = pwdnew; *p; p++ )
    {
      if ( *p == '~' )
      {
        write_to_buffer( d, "你的密碼不合規定。\n\r請輸入密碼﹕", 0 );
        RETURN_NULL();
      }
    }

    free_string( ch->pcdata->pwd );
    ch->pcdata->pwd = str_dup( pwdnew );
    write_to_buffer( d, "請重新輸入密碼來確認﹕", 0 );
    d->connected = CON_CONFIRM_NEW_PASSWORD;
    break;

  case CON_GET_CNAME:

    if ( str_len( argument ) > MaxCNameLen )
    {
      write_to_buffer( d, "你的中文名字太長了, 請重新輸入你的中文名字: ", 0 );
      d->error++;
      RETURN_NULL();
    }

    if ( str_len( argument ) < MinCNameLen )
    {
      write_to_buffer( d, "你的中文名字太短了﹐請重新輸入你的中文名字﹕", 0 );
      d->error++;
      RETURN_NULL();
    }

    if ( ( *argument < '\x81' || *argument > '\xFF' ) && *argument != '\e' )
    {
      write_to_buffer( d, "這不是一個中文名字﹐請重新輸入你的中文名字﹕", 0 );
      d->error++;
      RETURN_NULL();
    }

    if ( check_cname_xname( argument ) )
    {
      write_to_buffer( d, "你不能拿這個做名字﹐請重新輸入你的中文名字﹕", 0 );
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
      str_dup( "< 生命力^hp1 法力^mp1 體力^mv1 金錢^gol 經驗值^exp >" );

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

      print_to_char( ch , "\n\r\n\r%s\e[1;32m請按 ENTER 繼續遊戲﹕\e[0m"
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
    print_to_char( ch , "\n\r%s\e[1;32m請按 ENTER 繼續遊戲﹕\e[0m"
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
        write_to_buffer( d, "你必須註明圈選那一個﹐例如﹕select 1。"
          "請按 \e[1;32mENTER\e[0m 繼續圈選 ", 0 );

        break;
      }

      if ( !is_number( selection )
        || ( slot = atoi( selection ) ) <= 0
        || slot > nInnate )
      {
        sprintf( buf, "你的選項必須是數字﹐且在 1 到 %d 之間。例如﹕select 2"
          "。請按 \e[1;32mENTER\e[0m 繼續圈選 ", nInnate );

        write_to_buffer( d, buf, 0 );
        break;
      }

      if ( d->select[--slot] == FALSE && get_innate( d ) >= aInnate )
      {
        write_to_buffer( d, "你的選取的技能超出你可以選的總數。"
          "請按 \e[1;32mENTER\e[0m 繼續圈選 ", 0 );

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
        write_to_buffer( d, "你必須註明查詢那一個技能﹐例如﹕query 1。"
          "請按 \e[1;32mENTER\e[0m 繼續圈選 ", 0 );
        break;
      }

      if ( !is_number( selection )
        || ( slot = atoi( selection ) ) <= 0
        || slot > nInnate )
      {
        sprintf( buf, "你的選項必須是數字﹐且在 1 到 %d 之間。例如﹕query 1。"
          "請按 \e[1;32mENTER\e[0m 繼續圈選 ", nInnate );

        write_to_buffer( d, buf, 0 );
        break;
      }

      if ( !( pSkill = get_skill( iInnate[slot-1] ) )
        || !pSkill->help
        || !*pSkill->help )
      {
        write_to_buffer( d, "對不起﹐這個技能沒有說明文件﹗"
          "請按 \e[1;32mENTER\e[0m 繼續圈選 ", 0 );
        break;
      }

      write_to_buffer( d, pSkill->help, 0 );
      write_to_buffer( d, "請按 \e[1;32mENTER\e[0m 繼續圈選 ", 0 );
    }

    else if ( !str_prefix( selection, "help" ) )
    {
    write_to_descriptor( d,
      "\n\r    目前你正在選取一些基本的技能﹐也就是一開始進入遊戲前就會的基本功\夫。\n\r"
      "提示列會顯示目前你可以圈選幾個﹐還有你已經選了幾個﹗一開始你可以輸入\n\r"
      "query 1 來查詢一號技能的相關資料﹐你可以繼續查詢其他技能的狀況﹐到底適不\n\r"
      "適合自己學習。當你已經決定要圈選時﹐假定你要圈選定第二號技能﹐你可以輸入\n\r"
      "select 2。之後﹐第二號技能旁邊的圈選來便會有個記號﹐代表你已經圈選過了﹐\n\r"
      "如果你後悔﹐你可以再一次輸入 select 2 ﹐則這個記號便會消掉。當你圈選好了\n\r"
      "﹐你則可以輸入 done ﹐代表已經完成圈選﹐但是你圈選的數目不能少於應該圈選\n\r"
      "的數目﹐否則系統會請你在考慮圈選其他技能。以下是這些指令的簡單說明﹕\n\r\n\r"
      "  1、\e[1;35mselect\e[0m (號碼) 圈選或消除基本技能\n\r"
      "  2、\e[1;35mquery\e[0m  (號碼) 查詢某個技能的資訊\n\r"
      "  3、\e[1;35mclear\e[0m         清除之前的設定\n\r"
      "  4、\e[1;35mhelp\e[0m          目前的求助畫面\n\r"
      "  5、\e[1;35mdone\e[0m          完成圈選的動作\n\r\n\r"
      "請按 \e[1;32mENTER\e[0m 繼續圈選 ", 0 );
    }

    else if ( !str_prefix( selection, "done" ) )
    {
      if ( get_innate( d ) < aInnate )
      {
        write_to_buffer( d, "你尚有點數可以圈選﹐請不要放棄﹗"
          "請按 \e[1;32mENTER\e[0m 繼續圈選 ", 0 );
        break;
      }

      write_to_buffer( d, "請按任何鍵繼續﹗", 0 );
      d->connected = CON_CONTRACT;
    }

    else if ( !str_prefix( selection, "clear" ) )
    {
      for ( slot = 0; slot < MAX_INNATE; slot++ ) d->select[slot] = FALSE;
      show_innate( d );
    }

    else
    {
      write_to_buffer( d, "無效的指令﹗請按 \e[1;32mENTER\e[0m 繼續圈選 ", 0 );
    }

    break;

  case CON_CONFIRM_NEW_PASSWORD:

    write_to_buffer( d, "\n\r", 0 );
    if ( str_cmp( crypt( argument, ch->pcdata->pwd ), ch->pcdata->pwd ) )
    {
      write_to_buffer( d, "密碼不合。\n\r請重新輸入你的密碼﹕", 0 );
      d->connected = CON_GET_NEW_PASSWORD;
      d->error++;
      RETURN_NULL();
    }

    write_to_buffer( d, echo_on_str , 0 );
    write_to_buffer( d, "請輸入你的電子郵件信箱﹕", 0 );

    d->connected = CON_GET_EMAIL_ADDRESS;
    d->incomm[0] = '\x0';
    d->inbuf [0] = '\x0';
    break;

  case CON_GET_EMAIL_ADDRESS:

    if ( str_len( argument ) > 50 )
    {
      write_to_buffer( d, "你的信箱名稱太長了﹐請重新輸入你的電子信箱﹕", 0 );
      d->error++;
      RETURN_NULL();
    }

    if ( str_len( argument ) < 10 )
    {
      write_to_buffer( d, "你的電子信箱名稱太短了﹐請重新輸入你的電子信箱﹕", 0 );
      d->error++;
      RETURN_NULL();
    }

    if ( !check_email_address( argument ) )
    {
      write_to_buffer( d, "你的電子信箱名稱不合法﹐請重新輸入你的電子信箱﹕", 0 );
      d->error++;
      RETURN_NULL();
    }

    smash_tilde( argument );
    free_string( ch->email);
    ch->email = str_dup( argument );

    write_to_buffer( d, "請選擇你的性別 \e[1;32m1\e[0m(男性)"
      " \e[1;32m2\e[0m(女性) ", 0 );

    d->connected = CON_GET_SEX;
    d->incomm[0] = '\x0';
    d->inbuf [0] = '\x0';
    break;

  case CON_READ_MOTD:

    if ( ch->level > MAX_LEVEL )
    {
      mudlog( LOG_INFO, "修正 %s 等級 %d -> %d."
        , ch->name, ch->level, MAX_LEVEL );
      ch->level = MAX_LEVEL;
    }

    d->connected = CON_PLAYING;

    act( "$A$2歡迎你來到$5$t$2﹐謝謝你的支持﹐並請多多光臨﹗$0$A"
      , ch, mud_name, NULL, TO_CHAR );

    /* 送出今日消息 */
    if ( motd_message && *motd_message )
    {
      send_to_char(
        "\n\r--------------------------------- \e[1;32m今日消息\e[0m "
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

      /* 等級一的基本屬性 */
      ch->level   = 1;
      ch->now_str = DefaultStr;
      ch->now_int = DefaultInt;
      ch->now_wis = DefaultWis;
      ch->now_dex = DefaultDex;
      ch->now_con = DefaultCon;

      /* 等級一的金錢數目 */
      gold_to_char( ch, DefaultGold );
      ch->hit     = get_curr_hit( ch );
      ch->mana    = get_curr_mana( ch );
      ch->move    = get_curr_move( ch );

      /* 設定抬頭 */
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

      mudlog( LOG_NEWPLAYER , "%s 從 %s 連進來的新玩家.", ch->name, d->host );

      /*  設定各頻道 */
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

      /* 設定各組態 */
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

      /* 是否為第一個玩家﹐是的話給他輸入為大神 */
      if ( !imm_list )
      {
        IMMLIST_DATA * pImm;
        FILE         * pFile;

        if ( ( pFile = FOPEN( immlist_file, "w" ) ) )
        {
          fprintf( pFile, "#Immortal\n"                      );
          fprintf( pFile, "Name            %s~\n", ch->name  );
          fprintf( pFile, "Level           %d\n" , MAX_LEVEL );
          fprintf( pFile, "Adviser         %s\n", symbol_string( FALSE, "邏輯常數" ) );
          fprintf( pFile, "Description\n~\nEnd\n\nEnd"       );
          FCLOSE( pFile );

          set_immlist_default( pImm = alloc_struct( STRUCT_IMMLIST_DATA ) );

          pImm->name        = str_dup( ch->name );
          pImm->description = str_dup( "" );
          pImm->level       = MAX_LEVEL;
          pImm->next        = NULL;
          imm_list          = pImm;

          ch->level = MAX_LEVEL;

          act( "$B$B$B$A$A$2你是第一個進入$t的人﹐所以你被任命為大神。$A$0"
            , ch, mud_name, NULL, TO_CHAR );
        }

        else
        {
          mudlog( LOG_DEBUG, "無法開啟神族檔案." );
        }
      }
    }

    else if ( ch && ch->in_room )
    {
      if ( ch->in_room->NoQuit )
      {
        send_to_char( "你上次去的地方好像消失了。\n\r", ch );
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

        /* 清除追蹤紀錄點 */
        clear_trace( ch, TRUE );
      }
    }

    if ( ch && ch->failed > 0 && ch->jail <= 0 )
    {
      if ( ch->in_room != RoomFail )
      {
        char_from_room( ch );
        char_to_room( ch, RoomFail );

        /* 清除追蹤紀錄點 */
        clear_trace( ch, TRUE );
      }
    }

    if ( ch && ch->jail <= 0 && ch->in_room == RoomJail )
    {
      char_from_room( ch );
      char_to_room( ch, get_hometown( ch ) );

      /* 清除追蹤紀錄點 */
      clear_trace( ch, TRUE );
    }

    /* 若已不必思過﹐還在思過崖 */
    if ( ch && ch->failed <= 0 && ch->in_room == RoomFail )
    {
      char_from_room( ch );
      char_to_room( ch, get_hometown( ch ) );

      /* 清除追蹤紀錄點 */
      clear_trace( ch, TRUE );
    }

    /* 檢查物品是否正確 */
    check_null_object( ch );

    /* 檢查物品序號是否重複 */
    check_serial_number( ch );

    /* 取得幫派 */
    char_attach_club( ch );

    /* 修正等級 */
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

    /* 修正沒有屍體﹐但狀態死亡 */
    if ( ch->position == POS_DEAD && ch->pcdata && !ch->pcdata->corpse )
    {
      ch->position = POS_STANDING;
      char_from_room( ch );
      char_to_room( ch, get_hometown( ch ) );

      /* 清除追蹤紀錄點 */
      clear_trace( ch, TRUE );
    }

    /* 設定玩家序號 */
    set_serial( &ch->serial );

    /* 建立資料庫 */
    set_database( ch );

    act( "$n 乘雲駕霧從空而降﹐宛如天神﹗", ch, NULL, NULL, TO_ROOM );
    do_look( ch, "auto" );

    /* 檢查出生地 */
    get_home( ch );

    /* 修正屬性 */
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

    /* 修正技能 */
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

              act( "$B$B你已經漸漸地遺忘$1$w$0這項技能了﹗"
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
      act( "$3你有$i封信尚未讀取﹐請用 「$7note$3」 指令讀取。$0$B$B"
        , ch, &notes, NULL, TO_CHAR );

    if ( ( notes = vote_count( ch ) ) > 0 )
      act( "你尚有$i場票沒有投﹐請用「$6vote$0」來進行投票﹗"
        , ch, &notes, NULL, TO_CHAR );

    if ( IS_SET( ch->act, PLR_WIZINVIS ) )
    {
      send_to_char( "\e[1;32m噓﹐你正隱身中喔﹗\e[0m\n\r", ch );
    }
    /* 送出好友名單 */
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
      send_to_char( "\e[1;32m你正在服刑中﹗\e[0m\n\r", ch );

    else if ( ch->failed > 0 )
      send_to_char( "\e[1;32m你正在思過中﹗\e[0m\n\r", ch );

    /* 設定自動存檔和備份時間 */
    ch->pcdata->autobackup_tick = ch->pcdata->autobackup;
    ch->pcdata->autosave_tick   = ch->pcdata->autosave;

    /* 確定配偶 */
    check_mate( ch );

    /* 檢查金錢重量 */
    fix_gold_weight( ch );

    if ( over_scale( ch ) )
      send_to_char( "\e[1;31m你的總資產已達上限﹗﹗﹗\e[0m\n\r", ch );

    /* 賠償股票 */
    if ( ch->pcdata )
    {
      if ( StockBote > 0 ) bote_stock( ch );
      stock_value( ch );
    }

    /* 重新設定玩家的 armor */
    calculate_armor( ch );

    /* 顯示之前進入錯誤密碼 */
    if ( ch->failcode )
    {
      char filename[MAX_FILE_LENGTH];

      show_failcode( ch );
      extract_failcode( ch );

      /* 刪除檔案 */
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

/* 檢查 ID 是否符合規定 */
bool check_parse_name( char * name )
{
  XNAMES_DATA    * xnames;
  char           * pc;
  MOB_INDEX_DATA * pMob;
  int              iHash;

  PUSH_FUNCTION( "check_parse_name" );

  /* 保留字 */
  for ( xnames = xnames_first; xnames; xnames = xnames->next )
    if ( ( xnames->type == XNAMES_SYSTEM || xnames->type == XNAMES_CACONYM )
      &&  !str_cmp( xnames->word , name ) ) RETURN( FALSE );

  /* 長度限制 */
  if ( str_len(name) <  3 || str_len( name ) > NAME_LENGTH ) RETURN( FALSE );

  /* 只有英文和數字可以當作名字 */
  for ( pc = name; *pc; pc++ ) if ( !isalpha( ( int ) *pc ) ) RETURN( FALSE );

  /* 預防玩家和 MOB 相同 ID */
  for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
  {
    for ( pMob = mob_index_hash[iHash]; pMob; pMob = pMob->next )
      if ( is_fullname( name, pMob->name ) ) RETURN( FALSE );
  }

  RETURN( TRUE );
}

/* 檢查是否重新連線 */
bool check_reconnect( DESCRIPTOR_DATA * d, bool fConn )
{
  CHAR_DATA * ch;

  PUSH_FUNCTION( "check_reconnect" );

  if ( !d )
  {
    mudlog( LOG_DEBUG, "check_reconnect: 缺乏來源." );
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

        /* 修正死亡但是沒有屍體的狀況 */
        if ( ch->position == POS_DEAD && ch->pcdata && !ch->pcdata->corpse )
        {
          ch->position = POS_STANDING;
          char_from_room( ch );
          char_to_room( ch, get_hometown( ch ) );

          /* 清除追蹤紀錄點 */
          clear_trace( ch, TRUE );
        }

        send_to_char( "你回復了知覺﹗\n\r", ch );
        act( "$n回復了知覺﹗", ch, NULL, NULL, TO_ROOM );

        /* 檢查金錢重量 */
        fix_gold_weight( ch );

        mudlog( LOG_INFO , "%s 從 %s 重新連線 .", ch->name, d->host );

        d->connected = CON_PLAYING;
        d->inbuf[0]  = '\x0';
        d->incomm[0] = '\x0';

        /* 重新檢查裝備法術屬性 */
        affect_login_check( ch );

        /* 釋放賭博結構資料 */
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

/* 檢查是否已經在遊戲中 */
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
      write_to_buffer( d, "已經有相同名字的玩家在遊戲中了﹐"
                          "請重新輸入你的名字\n\r你的名字﹕", 0 );

      d->username[0] = '\x0';

      str_cpy( dold->incomm, "quit\0" );
      if ( dold->edit_mode != EDIT_NONE && dold->connected == CON_PLAYING )
      {
        dold->edit_mode = EDIT_NONE;
        free_string( dold->character->stack );
        dold->character->stack = str_dup( "" );
        process_stack( dold );
      }

      write_to_buffer( dold , "\n\r相同ID進入﹐強制斷線。\n\r" , 0 );

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

/* 停止 idle */
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
  act( "$n從虛無之境回復過來﹗", ch, NULL, NULL, TO_ROOM );

  /* 清除追蹤紀錄點 */
  clear_trace( ch, TRUE );

  RETURN_NULL();
}

void send_to_all_char( const char * txt )
{
  DESCRIPTOR_DATA * man;

  PUSH_FUNCTION( "send_to_all_char" );

  if ( !txt )
  {
    mudlog( LOG_DEBUG, "send_to_all_char: 來源不正確." );
    RETURN_NULL();
  }

  /* 快速檢查 */
  if ( !*txt ) RETURN_NULL();

  for ( man = descriptor_list; man; man = man->next )
  {
    if ( verify_desc( man ) && man->connected == CON_PLAYING )
      send_to_char( txt, man->character );
  }

  RETURN_NULL();
}

/* 送一段訊息給某幫派 */
void send_to_club( const char * txt, CLUB_DATA * pClub )
{
  DESCRIPTOR_DATA * man;

  PUSH_FUNCTION( "send_to_club" );

  if ( !txt || !pClub )
  {
    mudlog( LOG_DEBUG, "send_to_club: 來源不正確" );
    RETURN_NULL();
  }

  /* 快速檢查 */
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

/* 輸出給某間房間的所有的人物 */
void sendmsg_to_room( const char * txt, CHAR_DATA * ch )
{
  CHAR_DATA * victim;

  PUSH_FUNCTION( "sendmsg_to_room" );

  if ( !txt || !ch || !ch->in_room )
  {
    mudlog( LOG_DEBUG, "sendmsg_to_room: 來源不正確" );
    RETURN_NULL();
  }

  /* 快速檢查 */
  if ( !*txt ) RETURN_NULL();

  for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    if ( IS_AWAKE( victim ) && !IS_NPC( victim ) ) send_to_char( txt, victim );

  RETURN_NULL();
}

/* 送訊息到某間房間 */
void sendmsg_to_someroom( const char * txt , ROOM_INDEX_DATA * pRoom )
{
  CHAR_DATA * victim;

  PUSH_FUNCTION( "sendmsg_to_someroom" );

  if ( !txt || !pRoom )
  {
    mudlog( LOG_DEBUG, "sendmsg_to_someroom: 來源不正確." );
    RETURN_NULL();
  }

  /* 快速檢查 */
  if ( !*txt ) RETURN_NULL();

  for ( victim = pRoom->people; victim; victim = victim->next_in_room )
    if ( IS_AWAKE( victim ) && !IS_NPC( victim ) ) send_to_char( txt, victim );

  RETURN_NULL();
}

/* 輸出給玩家 */
void send_to_char( const char * txt, CHAR_DATA * ch )
{
  DESCRIPTOR_DATA * d;

  PUSH_FUNCTION( "send_to_char" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "send_to_char: 來源不正確." );
    RETURN_NULL();
  }

  if ( !txt || !( d = ch->desc ) ) RETURN_NULL();

  /* 如果字串很小則直接輸出 */
  if ( d->showstr_point || d->edit_mode != EDIT_NONE )
  {
    d->stack = TRUE;
    write_to_buffer( d, txt, str_len( txt ) );
    d->stack = FALSE;
    RETURN_NULL();
  }

  /* 若有尚未輸出完畢的字串則把他給釋放掉 */
  /*
  if ( d->showstr_point ) free_string( d->showstr_head );
  */

  if ( !( d->showstr_head = str_dup( txt ) ) ) RETURN_NULL();
  d->showstr_point = d->showstr_head;
  show_string( d, "" );
  RETURN_NULL();
}

/* 輸出的主要核心 */
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
  /* 下一頁 */
  case '\x0':
  case 'C'  :
    lines = 0;
    break;

  /* 重新輸出畫面 */
  case 'R':
    lines = - 1 - ( d->character->pcdata->pagelen );
    break;

   /* 上一頁 */
  case 'B':
    lines = -( 2 * d->character->pcdata->pagelen );
    break;

  /* 顯示求助 */
  case 'H':
    write_to_buffer( d,
      "C 或 RETURN [下一頁]﹐R [重新輸出畫面]﹐\n\r"
      "B = [上一頁]﹐H = [求助]﹐Q 或其他鍵 [結束]。\n\r\n\r" , 0 );

    lines = - 1 - ( d->character->pcdata->pagelen );
    break;

  /* 不然結束螢幕輸出 */
  default:

    cease_stack( d );
    RETURN_NULL();
  }

  /* 如果要退到上一張畫面 */
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
    /* 系統自我保護 */
    if ( limit++ > MAX_SCREEN_BUFFER - 2 )
    {
      *d->showstr_point = '\x0';
      mudlog( LOG_DEBUG , "show_string:畫面輸出已超過 MAX_SCREEN_BUFFER 大小." );
      mudlog( LOG_DEBUG , "show_string:請到 merc.h 把 MAX_SCREEN_BUFFER 加大." );
    }

    /* 計算換行的行數 */
    if ( ( ( *scan = *d->showstr_point ) == '\n' || *scan == '\r' )
      && ( toggle = -toggle ) < 0 ) lines++;

    /* 是否可以輸出給玩家了 */
    else if ( *scan == '\x0' || ( d->character && !IS_NPC( d->character )
           && lines >= d->character->pcdata->pagelen ) )
    {
      *scan = '\x0';
      write_to_buffer( d, buffer, 0 );

      /* 如果剩下部份只是空白, 則跳過, 算是輸出完畢 */
      for ( chk = d->showstr_point; *chk == ' ' ; chk++ );

      /* 輸出結束 */
      if ( *chk == '\x0' ) cease_stack( d );
      RETURN_NULL();
    }
  }
  RETURN_NULL();
}

/* 參數
1. 字串來源.
2. 主要操作人物
3. 可以是字串一, 物品一
4. 可以是字串二, 被操作人物.
5. 型態.
   a) TO_CHAR   結果只輸出給主要操作人物, 即參數二.
   b) TO_VICT   結果只輸出給被操作人物, 但這個被操作人不可以是主要操作人物
   c) TO_ROOM   結果輸出給主要操作人物房間所有的人, 除了主要操作人物.
   d) TO_NOVICT 結果輸出給除了主要操作和被操作人物以外同一房間的人.

參數
$t 拷貝字串一的內容.
$T 拷貝字串二的內容.
$n 主角人物(1)
$N 主角人物(2)
$e
$E
$s
$S
$p 物品一
$P 物品二
$d 參數二有關門的敘述
$A 換行
$$ $
$C 顏色控制碼
$0 還原顏色
$1 高亮度紅色
$2 高亮度綠色
$3 高亮度黃色
$4 高亮度藍色
$5 高亮度紫色
$6 高亮度天空色
$7 高亮度白色
$z 人物的英文名稱
$Z 人物的英文名稱
$B 鈴聲
$w 技能名稱(1)
$W 技能名稱(2)
$y 幫派名稱(1)
$Y 幫派名稱(2)
$i 中文化整數(1)
$I 中文化整數(2)
$x 數字型態整數(1)
$X 數字型態整數(2)
$r 房間名稱(1)
$R 房間名稱(2)
*/

/* 格式化輸出的主要核心 */
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
    mudlog( LOG_DEBUG, "act: 來源不正確, 原字串 '%s'", format );
    RETURN_NULL();
  }

  /* 忽略空的或是長度為零的訊息 */
  if ( !ch->in_room || !format || !*format ) RETURN_NULL();

  to = ch->in_room->people;

  if ( type == TO_VICT )
  {
    if ( !vch || !vch->in_room )
    {
      mudlog( LOG_DEBUG , "act: vch 來源不正確﹐原字串 '%s'", format );
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
        mudlog( LOG_DEBUG, "act: 輸出過長﹐原字串 '%s'", format );
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

        mudlog( LOG_DEBUG , "act: 錯誤的命令 '%c'﹐原字串 '%s'"
          , *str, format );

        i = " <@@@> ";
        break;

      case '$':
        i = "$";
        break;

      /* 還原顏色 */
      case '0':
        i = "\e[0m";
        break;

      /* 紅色 */
      case '1':
        i = "\e[1;31m";
        break;

      /* 綠色 */
      case '2':
        i = "\e[1;32m";
        break;

      /* 黃色 */
      case '3':
        i = "\e[1;33m";
        break;

      /* 藍色 */
      case '4':
        i = "\e[1;34m";
        break;

      /* 紫色 */
      case '5':
        i = "\e[1;35m";
        break;

      /* 天空色 */
      case '6':
        i = "\e[1;36m";
        break;

      /* 白色 */
      case '7':
        i = "\e[1;37m";
        break;

      /* 換行 */
      case 'A':
        i = "\n\r";
        break;

      /* 鈴聲 */
      case 'B':
        i = "\a";
        break;

      /* 設定顏色控制碼 */
      case 'C':
        i = "\e[";
        break;

      case 't':

        if ( !arg1 )
        {
          mudlog( LOG_DEBUG, "act: arg1 來源不正確(t), 原字串 '%s'", format );
          i = " <@@@> ";
          break;
        }

        i = ( char * ) arg1;
        break;

      case 'T':

        if ( !arg2 )
        {
          mudlog( LOG_DEBUG, "act: arg2 來源不正確(T), 原字串 '%s'", format );
          i = " <@@@> ";
          break;
        }

        i = ( char * ) arg2;
        break;

      case 'w':

        if ( !skill1 )
        {
          mudlog( LOG_DEBUG, "act: skill1 來源不正確(w), 原字串 '%s'", format );
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
          mudlog( LOG_DEBUG, "act: skill2 來源不正確(W), 原字串 '%s'", format );
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
          i = "你";
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
          str_cpy( buffer,  "某不知名人物" );
        }

        break;

      case 'N':

        if ( !vch )
        {
          mudlog( LOG_DEBUG, "act: vch 來源不正確(N), 原字串 '%s'", format );
          i = " <@@@> ";
          break;
        }

        if ( to == vch )
        {
          i = "你";
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
          str_cpy( buffer, "某不知名人物" );
        }

        break;

      case 'r':
        sprintf( buffer, "%s\e[0m", ch->in_room->name );
        i = buffer;
        break;

      case 'e':
        i = ( to == ch ) ? "你" : he_she[ URANGE( SEX_NEUTRAL, ch->sex, SEX_FEMALE ) ];
        break;

      case 'E':

        if ( !vch )
        {
          mudlog( LOG_DEBUG, "act: vch 來源不正確(E), 原字串 '%s'", format );
          i = " <@@@> ";
          break;
        }

        i = ( to == vch ) ? "你" : he_she[ URANGE( SEX_NEUTRAL, vch->sex, SEX_FEMALE ) ];
        break;

      case 's':
        i = ( to == ch ) ? "你的" : his_her[ URANGE( SEX_NEUTRAL, ch->sex, SEX_FEMALE ) ];
        break;

      case 'S':

        if ( !vch )
        {
          mudlog( LOG_DEBUG, "act: vch 來源不正確(S), 原字串 '%s'", format );
          i = " <@@@> ";
          break;
        }

        i = ( to == vch ) ? "你的" : his_her[ URANGE( SEX_NEUTRAL, vch->sex, SEX_FEMALE ) ];
        break;

      case 'p':

        if ( !obj1 )
        {
          mudlog( LOG_DEBUG, "act: obj1 來源不正確(p), 原字串 '%s'", format );
          i = " <@@@> ";
          break;
        }

        if ( obj1->corpse_owner && obj1->corpse_owner == to )
        {
          i = "自己屍體(corpse)";
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
            i = "某物";
          }
        }

        break;

      case 'P':

        if ( !obj2 )
        {
          mudlog( LOG_DEBUG, "act: obj2 來源不正確(P), 原字串 '%s'", format );
          i = " <@@@> ";
          break;
        }

        if ( obj2->corpse_owner && obj2->corpse_owner == to )
        {
          i = "自己屍體(corpse)";
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
            i = "某物";
          }
        }

        break;

      case 'x':

        if ( !arg1 )
        {
          mudlog( LOG_DEBUG, "act: 整數來源不正確(x), 原字串 '%s'", format );
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
          mudlog( LOG_DEBUG, "act: 整數來源不正確(X), 原字串 '%s'", format );
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
          mudlog( LOG_DEBUG, "act: 整數來源不正確(i), 原字串 '%s'", format );
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
          mudlog( LOG_DEBUG, "act: 整數來源不正確(I), 原字串 '%s'", format );
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
          mudlog( LOG_DEBUG, "act: club1 來源不正確(y), 原字串 '%s'", format );
          i = " <@@@> ";
          break;
        }

        i = buffer;
        sprintf( buffer, "%s\e[0m(%s)", club1->cname, club1->name );
        break;

      case 'Y':

        if ( !club2 )
        {
          mudlog( LOG_DEBUG, "act: club2 來源不正確(Y), 原字串 '%s'", format );
          i = " <@@@> ";
          break;
        }

        i = buffer;
        sprintf( buffer, "%s\e[0m(%s)", club2->cname, club2->name );
        break;

      case 'd':

        if ( !arg2 || ( ( char * ) arg2)[0] == '\x0' )
        {
          i = "門";
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
          mudlog( LOG_DEBUG, "act: vch 來源不正確(Z), 原字串 '%s'", format );
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
$n 主格
$N 受格
$O 物品
$Z 還原顏色
$C 設定顏色
$A 換行
$l 受傷部位
$R 高亮度的紅色   1;31m
$r 低亮度的紅色   0;31m
$G 高亮度的綠色   1;32m
$g 低亮度的綠色   0;32m
$Y 高亮度的黃色   1;33m
$y 低亮度的黃色   0;33m
$B 高亮度的藍色   1;34m
$b 低亮度的藍色   0;34m
$P 高亮度的紫色   1;35m
$p 低亮度的紫色   0;35m
$S 高亮度的天空色 1;36m
$s 低亮度的天空色 0;36m
$W 高亮度的白色   1;37m
$w 低亮度的白色   0;37m
*/

/* 技能顯示字串 */
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

  /* 測試人物 */
  if ( !ch )
  {
    mudlog( LOG_DEBUG, "sact: 對象人物是空的, 原字串 '%s'", format );
    RETURN_NULL();
  }

  /* 忽略空的或是長度為零的訊息 */
  if ( !format || !*format )
  {
    mudlog( LOG_DEBUG, "sact: 來源字串是空的, '%s'", format );
    RETURN_NULL();
  }

  /* 檢查房間是否是空的, 否則可能導致當機 */
  if ( !ch->in_room )
  {
    mudlog( LOG_DEBUG, "sact: 人物房間是空的, '%s'", format );
    RETURN_NULL();
  }

  /* 送給房間的每一個人物 */
  for ( to = ch->in_room->people; to; to = to->next_in_room )
  {
    if ( !to->desc
      || !IS_AWAKE( to )
      || ( !IS_NPC( to ) && !IS_SET( to->act, PLR_MESSAGE ) ) ) continue;

    *( pBuf = buf ) = '\x0';
    for ( len = 0, pSource = format; *pSource; pSource++ )
    {
      /* 檢查字串的長度 */
      if ( len > sizeof( buf ) - 10 )
      {
        mudlog( LOG_DEBUG, "sact: 字串超出範圍, '%s'", format );
        RETURN_NULL();
      }

      /* 如果不是命令字元 */
      if ( *pSource != '$' )
      {
        *pBuf++ = *pSource;
        len++;
        continue;
      }

      switch( *(++pSource ) )
      {
      default :
        mudlog( LOG_DEBUG , "sact: 錯誤的命令 '%c', 原字串 '%s'"
          , *pSource, format );

        pStr = "<@@@>";
        break;

      case '$':
        pStr = "$";
        break;

      case 'A':
        pStr = "\n\r";
        break;

      /* 高亮度藍色 */
      case 'B':
        pStr = "\e[1;34m";
        break;

      /* 低亮度藍色 */
      case 'b':
        pStr = "\e[0;34m";
        break;

      /* 設定顏色 */
      case 'C':
        pStr = "\e[";
        break;

      case 'E':
             if ( !victim      ) pStr = "不知名人物";
        else if ( to == victim ) pStr = "你";
        else                     pStr = he_she[ URANGE( SEX_NEUTRAL, victim->sex, SEX_FEMALE ) ];
        break;

      case 'e':
        pStr = ( to == ch ) ? "你" : he_she[ URANGE( SEX_NEUTRAL, ch->sex, SEX_FEMALE ) ];
        break;

      /* 高亮度綠色 */
      case 'G':
        pStr = "\e[1;32m";
        break;

      /* 低亮度綠色 */
      case 'g':
        pStr = "\e[0;32m";
        break;

      case 'l':

        if ( !victim )
        {
          mudlog( LOG_DEBUG, "sact: 對象是空的, '%s'", format );
          RETURN_NULL();
        }

        while ( situs_lookup( victim->situs ) == NULL )
          random_situs( victim, ATTACK_RANDOM );

        if ( !( pSitus = situs_lookup( victim->situs ) ) ) pStr = "<@@@>";
        else pStr = pSitus->situs_name;
        break;

      /* 受詞 */
      case 'N':
             if ( !victim )      pStr = "不知名人物";
        else if ( to == victim ) pStr = "你";
        else                     pStr = mob_name( to, victim );
        break;

      /* 主詞 */
      case 'n':
        pStr = ( to == ch ) ? "你" : mob_name( to, ch );
        break;

      /* 物品 */
      case 'O':

        if ( !obj )
        {
          mudlog( LOG_DEBUG , "sact: 物品是空的, '%s'", format );
          RETURN_NULL();
        }

        pStr = can_see_obj( to, obj ) ? obj->cname : "某物";
        break;

      /* 高亮度紫色 */
      case 'P':
        pStr = "\e[1;35m";
        break;

      /* 低亮度紫色 */
      case 'p':
        pStr = "\e[0;35m";
        break;

      /* 高亮度紅色 */
      case 'R':
        pStr = "\e[1;31m";
        break;

      /* 低亮度紅色 */
      case 'r':
        pStr = "\e[0;31m";
        break;

      /* 設定高亮度天空色 */
      case 'S':
        pStr = "\e[1;36m";
        break;

      /* 設定低亮度天空色 */
      case 's':
        pStr = "\e[0;36m";
        break;

      /* 設定高亮度白色 */
      case 'W':
        pStr = "\e[1;37m";
        break;

      /* 設定低亮度白色 */
      case 'w':
        pStr = "\e[0;37m";
        break;

      /* 設定高亮度黃色 */
      case 'Y':
        pStr = "\e[1;33m";
        break;

      /* 設定低亮度黃色 */
      case 'y':
        pStr = "\e[0;33m";
        break;

      /* 還原顏色碼 */
      case 'Z':
        pStr = "\e[0m";
        break;
      }

      if ( !pStr || !*pStr ) continue;

      if ( len + str_len( pStr ) > sizeof( buf ) - 10 )
      {
        mudlog( LOG_DEBUG, "sact: 連接字串超出範圍, '%s'", format );
        RETURN_NULL();
      }

      while ( ( *pBuf = *pStr ) != '\x0' ) ++pBuf, ++pStr, len++;
    }

    /* 接上換行字元, 以及字串結尾字元 */
    *pBuf++ =  '\n';
    *pBuf++ =  '\r';
    *pBuf++ = '\x0';

    /* 根據不同的對象送出不同的字串 */
    set_descriptor_stack( to->desc );
    write_to_buffer( to->desc, buf, 0 );
    to->desc->stack = FALSE;
  }

  RETURN_NULL();
}

/* 分隔分號 */
void spilt_semicolon( DESCRIPTOR_DATA * d )
{
  char buffer[ MAX_INPUT_LENGTH * 2 ];
  int  source;
  int  target;

  PUSH_FUNCTION( "spilt_semicolon" );

  /* 若不是已經進入遊戲則不行 */
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

  /* 看看是否超出範圍 */
  if ( target >= sizeof( d->inbuf )  - 10 )
  {
    write_to_buffer( d,
      "\n\r你輸入已超出系統的範圍﹐所以清除你的輸入緩衝區。\n\r", 0 );

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

  /* 若未進入遊戲則不行 */
  if ( !d
    || d->connected != CON_PLAYING
    || str_len( d->incomm ) >= sizeof( buf1 ) - 2 ) RETURN_NULL();

  /* 先把字串處理一下 */
  str_cpy( ( pBuf = buf1 ) , d->incomm );
  while ( *pBuf == ' ' ) pBuf++;

  /* 若不是命令起頭則不行 */
  if ( *pBuf != SYMBOL_COMMAND ) RETURN_NULL();

  /* 數字部份 */
  pDest = test_number( pBuf + 1 , &dup );

  if ( dup > 0 && dup < MaxRepeat - 1 )
  {
    /* 檢查是否超出範圍 */
    if ( str_len( d->inbuf ) + ( dup * ( str_len( pDest ) + 2 ) )
      > sizeof( d->incomm ) - 10 )
    {
      write_to_buffer( d,
        "\n\r你輸入已超出系統的範圍﹐所以清除你的輸入緩衝區。\n\r", 0 );
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

  /* 命令 #BEEP */
  if ( !str_cmp( pBuf + 1 , "beep" ) )
  {
    write_to_buffer( d, "##BEEP!!!\a\n\r" , 0 );
    str_cpy( d->incomm , " \x0" );
    RETURN_NULL();
  }

  RETURN_NULL();
}

/* 顯示出遊戲的狀態 */
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
    mudlog( LOG_DEBUG, "show_game_status: 來源是空的." );
    RETURN_NULL();
  }

  chinese_number( merc_id         , buf1 );
  chinese_number( get_usernum()   , buf2 );
  chinese_number( get_attemptnum(), buf3 );
  chinese_number( MaxConnect      , buf4 );

  sprintf( buffer,
    "\n\r\e[1;33m歡迎來到\e[1;37m【\e[1;44;36m%s\e[0m\e[1;37m】"
    "( 本系統已經啟動了\e[1;33m%s \e[1;37m)\n\r"
    "\e[1;33m系統負載量為 %s%% 總上站人數有%s次﹐上線人數最高記錄%s人\n\r"
    "目前共有%s人在此遙遠的%s奮鬥﹐"
    "另外還有%s個人嘗試連線中。\e[0m\n\r\n\r"
    , mud_name, get_worktime_string(), get_loading()
    , buf1, buf4, buf2, mud_name, buf3 );

  write_to_descriptor( man, buffer, 0 );
  RETURN_NULL();
}

/* 取回備份檔案 */
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
    mudlog( LOG_DEBUG, "recover_file: 缺乏來源." );
    RETURN_NULL();
  }

  name = man->username;

  /* 先砍掉密碼錯誤檔 */
  str_cpy( failcode, file_name( name, FAILCODE_FILE ) );
  unlink( failcode );

  str_cpy( source, file_name( name, BACKUP_FILE ) );
  str_cpy( target, file_name( name, SAVE_FILE   ) );

  if ( ( fp_source = fopen( source, "r+" ) ) )
  {
    if ( ( fp_target = fopen( target, "w+" ) ) )
    {
      write_to_descriptor( man,
        "你的檔案已經損毀﹐系統自動幫你取回備份檔﹗\n\r", 0 );
      mudlog( LOG_DEBUG , "[Bad_File] %s 的檔案損毀﹐系統自動取回。", name );
      while ( ( transfer = fgetc( fp_source ) ) != EOF )
        fputc( transfer , fp_target );
      fclose( fp_target );

      /* 改變檔案存取模式 */
      set_file_mode( target );
    }

    else
    {
      sprintf( buf,
        "\n\r\a\a\a你的檔案已經損毀﹐但是系統無法幫你取回備份檔﹐"
        "請至 \e[1;32m%s\e[0m 報告。\n\r" , bbs_name );

      write_to_descriptor( man, buf, 0 );

      mudlog( LOG_DEBUG , "[Bad_File] %s 的檔案損毀﹐但系統無法取回.", name );
    }

    fclose( fp_source );
  }

  else
  {

    sprintf( buf,
      "\n\r\a\a\a你的檔案已經損毀﹐但是系統無法幫你取回備份檔﹐"
      "請至 \e[1;32m%s\e[0m 報告。\n\r" , bbs_name );

    write_to_descriptor( man, buf, 0 );
    mudlog( LOG_DEBUG , "[Bad_File] %s 的檔案損毀, 但系統無法取回.", name );
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
    mudlog( LOG_DEBUG, "lock_file: 缺乏來源." );
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
    mudlog( LOG_DEBUG, "check_multi_login: 缺乏來源." );
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
    mudlog( LOG_DEBUG, "set_innate: 缺乏來源." );
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

  sprintf( buf2, "\n\r%s請選擇你的出生地(\e[1;32m1\e[0m-\e[1;32m%d\e[0m)﹕ "
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
    mudlog( LOG_DEBUG, "show_innate: 缺乏來源." );
    RETURN_NULL();
  }

  chinese_number( aInnate          , chinese1 );
  chinese_number( get_innate( man ), chinese2 );

  sprintf( buf1, "\n\r請選取你想要的技能「你可以圈選%s個技能﹐你已經圈了%s個"
    "」\n\r%s", chinese1, chinese2, VERTICAL_LINE );

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

  sprintf( buf2, "請輸入 \e[1;36mselect\e[0m(選擇) \e[1;36mhelp\e[0m(求助)"
    " \e[1;36mdone\e[0m(完成) \e[1;36mclear\e[0m(清除) "
    "\e[1;36mquery\e[0m(查詢) " );

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
    mudlog( LOG_DEBUG, "show_innate: 缺乏來源." );
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
    mudlog( LOG_DEBUG, "set_descriptor_stack: 缺乏來源." );
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
    mudlog( LOG_DEBUG, "save_to_stack: 缺乏來源." );
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
    mudlog( LOG_DEBUG, "process_stack: 缺乏來源." );
    RETURN_NULL();
  }

  if ( man->buffer && *man->buffer )
  {
    write_to_buffer( man, "\n\r                          ----"
      "「\e[1;37m輸出暫存內容\e[0m」----\n\r\n\r", 0 );

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
    mudlog( LOG_DEBUG, "cease_stack: 缺乏來源." );
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
    mudlog( LOG_DEBUG, "immortal_on_line: 來源錯誤." );
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
      act( "$2「上線通知」$0 神族 $N (等級﹕$2$x$0 ) 上線來了﹗"
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
    mudlog( LOG_DEBUG, "immortal_off_line: 來源錯誤." );
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
      act( "$2「離線通知」$0 神族 $N (等級﹕$2$x$0 ) 離線了﹗"
        , victim, &ch->level, ch, TO_CHAR );
  }

  RETURN_NULL();
}

void merc_exit( int exitcode )
{
  if ( CoreDump && merc_pid == getpid() ) abort();
  exit( exitcode );
}

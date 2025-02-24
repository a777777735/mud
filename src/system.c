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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/param.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/signal.h>
#include <sys/syscall.h>
#include <sys/procfs.h>
#include <sys/utsname.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/times.h>
#include "merc.h"

struct ProcFile
{
  char cmd[40];       /* 命令               Bsd */
  char ttyname[20];   /*                    Bsd, no Linux */
  char state;         /* 行程狀態           */
  int pid;            /* 行程 PID           Bsd */
  int ppid;           /* 父行程 PID         Bsd */
  int pgrp;           /* 群族行程號碼       Bsd */
  int sid;            /* Session ID         Bsd */
  int uid;            /* 使用者的ID號碼     */
  int gid;            /* 使用者的群族ID號碼 */
  int tty;            /* TTY                */
  int tpgid;          /* 終端機行程ID       */
  int flags;          /* 行程旗標           */
  int min_flt;        /* 填滿 PAGE 時不需要用到 I/O 動作的次數       Bsd */
  int maj_flt;        /* 子行程填滿 PAGE 時不需要用到 I/O 動作的次數 Bsd */
  int cmin_flt;       /* 填滿 PAGE 時需要用到 I/O 動作的次數         */
  int cmaj_flt;       /* 子行程填滿 PAGE 時需要用到 I/O 動作的次數   */
  int utime;          /* 行程所耗的時間                              */
  int stime;          /* 核心被行程所耗的時間                        */
  int cutime;         /* 行程的子行程所耗的時間                      */
  int cstime;         /* 核心被行程的子行程所耗的時間                */
  int priority;       /* 優先權                                      */
  int nice;           /* Nice                    */
  int timeout;        /* 未知用途                */
  int it_real_value;  /* 未知用途                */
  int start_time;     /* 啟動時間                Bsd */
  int start_utime;    /* 啟動時間                Bsd, no Linux */
  int vsize;          /* 虛擬記憶體的頁數        */
  int rss;            /* 佔存的記憶體            */
  int rss_rlim;       /* 未知用途                */
  int start_code;     /* Code Segment 起使位址   */
  int end_code;       /* Code Segment 結束位址   */
  int start_stack;    /* 堆疊的底部位址          */
  int kstack_esp;     /* 核心堆疊的ESP暫存器內容 */
  int kstack_eip;     /* 核心堆疊的EIP暫存器內容 */
  int signal;         /* pending 訊號的遮罩 */
  int blocked;        /* blocked 訊號的遮罩 */
  int sigignore;      /* ignored 訊號的遮罩 */
  int sigcatch;       /* catch   訊號的遮罩 */
  int wchan;          /* 核心等待行程的位址 */
};

bool    get_proc                args( ( pid_t, struct ProcFile * ) );
int     get_tick                args( ( void ) );
int     get_process_loading     args( ( struct ProcFile * sysinfo ) );
size_t  ip_count                args( ( CHAR_DATA * ) );
void    set_ip                  args( ( CHAR_DATA *, const char * ) );
char *  get_OS_loading          args( ( void ) );
char *  get_mud_loading         args( ( void ) );
char *  rlimit_str              args( ( int ) );

#define PROC_DIR        "/proc"
#define LOADING_FILE    "/proc/loadavg"

/* 讀取系統一般資訊 */
FUNCTION( do_sysinfo )
{
  char            arg[MAX_INPUT_LENGTH];
  char            buf[MAX_STRING_LENGTH];
  extern int      WriteBytes;
  extern int      WriteKiloBytes;
  extern int      ReadBytes;
  extern int      ReadKiloBytes;
  struct ProcFile sysinfo;
  struct timeval  sTime;
  struct rusage   usage_self;
  struct rusage   usage_chld;
  int             loop;
  int             freq;
  int             total;
  int             stime;
  int             ustime;
  long long       flow;

  PUSH_FUNCTION( "do_sysinfo" );

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    if ( get_proc( merc_pid, &sysinfo ) == FALSE )
    {
      send_to_char( "讀取系統檔案發生錯誤。\n\r", ch );
      RETURN_NULL();
    }

    freq = get_tick();
    clear_buffer();

    for ( loop = 0; loop < MAX_PORT; loop++ )
    {
      if ( MudPort[loop] == ERRORCODE ) break;
      send_to_buffer( "連接埠﹕%5d 連接總人次﹕%5d\n\r"
        , MudPort[loop], MudConnect[loop] );
    }

    #ifdef __linux__
    send_to_buffer( "虛擬系統記憶體﹕%d 千位元組。\n\r"
      , ( sysinfo.vsize >> 10 ) );

    send_to_buffer( "系統常駐記憶體﹕%d 千位元組。\n\r"
      , ( sysinfo.rss << 2 ) );
    #endif

    getrusage( RUSAGE_SELF    , &usage_self );
    getrusage( RUSAGE_CHILDREN, &usage_chld );

    sTime.tv_sec  = usage_self.ru_utime.tv_sec;
    sTime.tv_usec = usage_self.ru_utime.tv_usec;

    send_to_buffer( "主行程正常消耗時間﹕%s 秒。\n\r", time_string( &sTime ) );

    sTime.tv_sec  = usage_chld.ru_utime.tv_sec;
    sTime.tv_usec = usage_chld.ru_utime.tv_usec;
    send_to_buffer( "子行程正常消耗時間﹕%s 秒。\n\r", time_string( &sTime ) );

    sTime.tv_sec  = usage_self.ru_stime.tv_sec;
    sTime.tv_usec = usage_self.ru_stime.tv_usec;
    send_to_buffer( "主行程系統消耗時間﹕%s 秒。\n\r", time_string( &sTime ) );

    sTime.tv_sec  = usage_chld.ru_stime.tv_sec;
    sTime.tv_usec = usage_chld.ru_stime.tv_usec;
    send_to_buffer( "子行程系統消耗時間﹕%s 秒。\n\r", time_string( &sTime ) );

    sTime.tv_sec = usage_self.ru_stime.tv_sec
                 + usage_self.ru_utime.tv_sec
                 + usage_chld.ru_stime.tv_sec
                 + usage_chld.ru_stime.tv_sec;

    sTime.tv_usec = usage_self.ru_stime.tv_usec
                  + usage_self.ru_utime.tv_usec
                  + usage_chld.ru_stime.tv_usec
                  + usage_chld.ru_stime.tv_usec;

    send_to_buffer( "行程執行總和時間﹕%s 秒。\n\r", time_string( &sTime ) );

    total = get_uptime() - mud_boot_tick;
    sTime.tv_sec  = total / freq;
    sTime.tv_usec = total % freq;

    send_to_buffer( "行程累積總和時間﹕%s 秒。\n\r", time_string( &sTime ) );
    send_to_buffer( "行程平均負載量級﹕%s%%。\n\r" , get_loading() );

    send_to_buffer( "總寫出位元組  ﹕%d 千位元組 %d 位元組。\n\r"
      , WriteKiloBytes, WriteBytes );

    flow = ( WriteKiloBytes * 1000 + WriteBytes ) / UMAX( sTime.tv_sec , 1 );
    send_to_buffer( "平均輸出流量  ﹕每秒 %d 位元組。\n\r", (int) flow );

    send_to_buffer( "總讀入位元組  ﹕%d 千位元組 %d 位元組。\n\r"
      , ReadKiloBytes, ReadBytes );

    flow = ( ReadKiloBytes * 1000 + ReadBytes ) / UMAX( sTime.tv_sec , 1 );
    send_to_buffer( "平均輸入流量  ﹕每秒 %d 位元組。\n\r", (int) flow );

    send_to_buffer( "主機最近的負荷﹕%s。\n\r", get_OS_loading()  );
    send_to_buffer( "系統最近的負荷﹕%s。\n\r", get_mud_loading() );

    send_to_buffer( "系統總註冊人數﹕%s 人。\n\r"
      , numberize( database_count( DATABASE_EXIST ), NULL ) );

    print_buffer( ch );
  }
  else if ( !str_prefix( arg, "loading" ) )
  {
    if ( argument[0] == '\x0' || !is_number( argument ) )
    {
      send_to_char( "你要計算幾分鐘前至目前的負載量﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( ( loop = atoi( argument ) ) <= 0 )
    {
      send_to_char( "你的分鐘數必須大於零的整數﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !get_exec_time( loop, &stime, &ustime ) )
    {
      send_to_char( "沒有那麼久之前的資料可供計算﹗\n\r", ch );
      RETURN_NULL();
    }

    chinese_number( loop, buf );
    total = calc_loading( loop, stime, ustime );

    print_to_char( ch, "%s\e[0m系統﹐%s分鐘前至目前的平均負載量為"
      " %d.%d%d%%\n\r"
      , mud_name
      , buf, total / 100, ( total % 100 ) / 10 , ( total % 100 ) % 10);
  }
  else
  {
    send_to_char( "對不起﹐你的語法錯誤﹐請查詢 sysconf 的使用方法﹗\n\r", ch );
  }
  RETURN_NULL();
}

#ifdef __linux__
/* 執行閱讀系統低階資訊函數 */
FUNCTION( do_debug )
{
  struct ProcFile sysinfo;

  PUSH_FUNCTION( "do_debug" );

  /* 若是讀取有發生錯誤 */
  if ( get_proc( merc_pid, &sysinfo ) == FALSE )
  {
    send_to_char( "讀取 proc 檔案有錯誤發生。\n\r" , ch );
    RETURN_NULL();
  }

  clear_buffer();
  send_to_buffer( "目前行程號碼(PID)﹕                          %d\n\r"
                  "父程號碼(PPID)﹕                             %d\n\r"
                  "狀態﹕                                       "
    , sysinfo.pid, sysinfo.ppid );

  switch( sysinfo.state )
  {
    default  : send_to_buffer( "未明"            ); break;
    case 'I' : send_to_buffer( "閒置(I)"         ); break;
    case 'O' : send_to_buffer( "正在處理(O)"     ); break;
    case 'R' : send_to_buffer( "執行佇列中(R)"   ); break;
    case 'S' : send_to_buffer( "沈睡中(S)"       ); break;
    case 'T' : send_to_buffer( "停止中(T)"       ); break;
    case 'X' : send_to_buffer( "等待記憶體中(X)" ); break;
    case 'Z' : send_to_buffer( "呆滯中(Z)"       ); break;
  }

  send_to_buffer( "\n\r"
                  "群族行程號碼(PGRP)﹕                         %d\n\r"
                  "Session行程號碼(SID)﹕                       %d\n\r"
                  "控制台﹕                                     %d\n\r"
                  "終端機行程號碼(TPGID)﹕                      %d\n\r"
                  "行程旗標﹕                                   %d\n\r"
                  "填滿 PAGE 時不需要用到 I/O 動作的次數﹕      %d\n\r"
                  "子行程填滿 PAGE 時不需要用到 I/O 動作的次數﹕%d\n\r"
                  "填滿 PAGE 時需要用到 I/O 動作的次數﹕        %d\n\r"
                  "子行程填滿 PAGE 時需要用到 I/O 動作的次數﹕  %d\n\r"
                  "行程所耗的時間﹕                             %d\n\r"
                  "核心被行程所耗的時間﹕                       %d\n\r"
                  "行程的子行程所耗的時間﹕                     %d\n\r"
                  "核心被行程的子行程所耗的時間﹕               %d\n\r"
                  "優先權﹕                                     %d\n\r"
                  "Nice﹕                                       %d\n\r"
                  "啟動時間﹕                                   %d\n\r"
                  "虛擬記憶體的大小﹕                           %d\n\r"
                  "佔存的記憶體                                 %d\n\r"
                  "Code Segment 起使位址                        %d\n\r"
                  "Code Segment 結束位址                        %d\n\r"
                  "堆疊的底部位址                               %d\n\r"
                  "核心堆疊的ESP暫存器內容                      %d\n\r"
                  "核心堆疊的EIP暫存器內容                      %d\n\r"
                  "pending 訊號的遮罩                           %d\n\r"
                  "blocked 訊號的遮罩                           %d\n\r"
                  "ignored 訊號的遮罩                           %d\n\r"
                  "catch   訊號的遮罩                           %d\n\r"
                  "核心等待行程的位址                           %d\n\r"
                  "頻率(HZ)                                     %d\n\r"
                  "系統負載                                     %s%%\n\r"
                  , sysinfo.pgrp
                  , sysinfo.sid
                  , sysinfo.tty
                  , sysinfo.tpgid
                  , sysinfo.flags
                  , sysinfo.min_flt
                  , sysinfo.cmin_flt
                  , sysinfo.maj_flt
                  , sysinfo.cmaj_flt
                  , sysinfo.utime
                  , sysinfo.stime
                  , sysinfo.cutime
                  , sysinfo.cstime
                  , sysinfo.priority
                  , sysinfo.nice
                  , sysinfo.start_time
                  , sysinfo.vsize >> 10
                  , sysinfo.rss << 2
                  , sysinfo.start_code
                  , sysinfo.end_code
                  , sysinfo.start_stack
                  , sysinfo.kstack_esp
                  , sysinfo.kstack_eip
                  , sysinfo.signal
                  , sysinfo.blocked
                  , sysinfo.sigignore
                  , sysinfo.sigcatch
                  , sysinfo.wchan
                  , get_tick()
                  , get_loading() );

  print_buffer( ch );
  RETURN_NULL();
}

#else
FUNCTION( do_debug )
{
  PUSH_FUNCTION( "do_debug" );
  send_to_char( "此類作業系統不提供這類服務。\n\r", ch );
  RETURN_NULL();
}
#endif

/* 計算出系統的負載量 */
char * get_loading( void )
{
  int         used;
  int         elapsed;
  float       loading;
  struct tms  pTms;
  static char buffer[ MAX_INPUT_LENGTH ];

  PUSH_FUNCTION( "get_loading" );

  times( &pTms );
  used = pTms.tms_utime + pTms.tms_stime + pTms.tms_cutime + pTms.tms_cstime;

  elapsed = get_uptime() - mud_boot_tick;
  loading = ( ( float ) ( used * 100 ) ) / ( ( float ) elapsed );
  sprintf( buffer , "%.3f" , loading );
  RETURN( buffer );
}

int get_process_loading( struct ProcFile * sys )
{
  PUSH_FUNCTION( "get_process_loading" );
  if ( !sys ) RETURN( 0 );

  RETURN( sys->utime + sys->stime + sys->cutime + sys->cstime );
}

/* 列印出系統的資訊 */
FUNCTION( do_uname )
{
  struct utsname name;

  PUSH_FUNCTION( "do_uname" );

  if ( uname( &name ) < 0 )
  {
    send_to_char( "函數 uname() 有問題。" , ch );
    RETURN_NULL();
  }

  print_to_char( ch ,
    "函數 uname() 的資訊﹕\n\r"
    "作業系統名稱﹕%s\n\r"
    "作業系統版本﹕%s\n\r"
    "系統節點名稱﹕%s\n\r"
    "系統正式版本﹕%s\n\r"
    "系統機器型態﹕%s\n\r"
    "Domain name ﹕%s\n\r"
    , name.sysname
    , name.release
    , name.nodename
    , name.version
    , name.machine );

  #ifdef __linux__
  print_to_char( ch, "Domain name ﹕%s\n\r", name.domainname );
  #endif

  RETURN_NULL();
}

/* 取得系統的組態 */
FUNCTION( do_sysconf )
{
  PUSH_FUNCTION( "do_sysconf" );

  clear_buffer();

  send_to_buffer( "每個使用者最多行程數   ﹕%d\n\r"
    , sysconf( _SC_CHILD_MAX ) );

  send_to_buffer( "每秒的系統記數器次數   ﹕%d\n\r" , get_tick() );

  send_to_buffer( "行程最多可以開幾個匯流 ﹕%d\n\r"
    , sysconf( _SC_STREAM_MAX ) );

  send_to_buffer( "行程最多可以開幾個檔案 ﹕%d\n\r"
    , sysconf( _SC_OPEN_MAX ) );

  send_to_buffer( "是否支援 Job 控制      ﹕%s\n\r"
    , YESNO( sysconf( _SC_JOB_CONTROL ) ) );

  send_to_buffer( "是否支援記錄 IDS       ﹕%s\n\r"
    , YESNO( sysconf( _SC_SAVED_IDS ) ) );

  send_to_buffer( "系統所支援的 POSIX 版本﹕%d.%d\n\r"
    , sysconf( _SC_VERSION ) / 100
    , sysconf( _SC_VERSION ) % 100 );

  send_to_buffer( "系統所支援的POSIX2 版本﹕%d.%d\n\r"
    , sysconf( _SC_2_VERSION ) / 100
    , sysconf( _SC_2_VERSION ) % 100 );

  print_buffer( ch );
  RETURN_NULL();
}

/* 取得系統開機到目前為止的 Tick 數 */
unsigned long get_uptime( void )
{
  struct tms pTms;

  PUSH_FUNCTION( "getuptime" );
  RETURN( times( &pTms ) );
}

/* 取得系統的 Tick */
int get_tick( void )
{
  PUSH_FUNCTION( "get_tick" );
  RETURN( sysconf( _SC_CLK_TCK ) );
}

#if defined(READ_WORD)
#undef READ_WORD
#endif

#if defined(READ_NUMBER)
#undef READ_NUMBER
#endif

#if defined(READ_CHAR)
#undef READ_CHAR
#endif

#define READ_WORD( source, target )        \
  {                                        \
    char temp[256];                        \
    source = one_argument( source, temp ); \
    str_cpy( target, temp );               \
  }

#define READ_NUMBER( source, target )      \
  {                                        \
    char temp[256];                        \
    source = one_argument( source, temp ); \
    target = atoi( temp );                 \
  }

#define READ_CHAR( source, target )        \
  {                                        \
    char temp[256];                        \
    source = one_argument( source, temp ); \
    target = ( char ) temp[0];             \
  }

#ifdef __linux__
/* 取得 proc file 的檔案 */
bool get_proc( pid_t pid, struct ProcFile * proc )
{
  int    fd;
  char   filename[MAX_FILE_LENGTH];
  char   buffer[512];
  char * arg;

  PUSH_FUNCTION( "get_proc" );

  /* 檢查 PID 是否 合法 */
  if ( pid <= 1 )
  {
    mudlog( LOG_INFO, "get_proc: pid 號碼 %d 不合法" , pid );
    RETURN( FALSE );
  }

  /* 檢查結構是否合法 */
  if ( proc == NULL )
  {
    mudlog( LOG_INFO, "get_proc: 結構 proc 是空的" );
    RETURN( FALSE );
  }

  /* 清除結構的資料 */
  memset( proc , 0, sizeof( struct ProcFile ) );

  /* 設定檔名 */
  sprintf( filename, "%s/%d/stat" , PROC_DIR , pid );

  /* 開啟 proc file stat 的內容 */
  if ( ( fd = open( filename , O_RDONLY ,0 ) ) < 0 )
  {
    mudlog( LOG_INFO , "get_proc: open:%s" , strerror( errno ) );
    RETURN( FALSE );
  }

  /* 讀取 proc file stat 的內容 */
  if ( ( read( fd, buffer , sizeof( buffer ) - 1 ) ) <= 0 )
  {
    mudlog( LOG_INFO , "Get_proc: read:%s" , strerror( errno ) );
    close( fd );
    RETURN( FALSE );
  }

  /* 關閉檔案描述子 */
  close( fd );

  /* 從緩衝區中讀取資料 */
  arg = buffer;
  READ_NUMBER( arg, proc->pid           );
  READ_WORD  ( arg, proc->cmd           );
  READ_CHAR  ( arg, proc->state         );
  READ_NUMBER( arg, proc->ppid          );
  READ_NUMBER( arg, proc->pgrp          );
  READ_NUMBER( arg, proc->sid           );
  READ_NUMBER( arg, proc->tty           );
  READ_NUMBER( arg, proc->tpgid         );
  READ_NUMBER( arg, proc->flags         );
  READ_NUMBER( arg, proc->min_flt       );
  READ_NUMBER( arg, proc->cmin_flt      );
  READ_NUMBER( arg, proc->maj_flt       );
  READ_NUMBER( arg, proc->cmaj_flt      );
  READ_NUMBER( arg, proc->utime         );
  READ_NUMBER( arg, proc->stime         );
  READ_NUMBER( arg, proc->cutime        );
  READ_NUMBER( arg, proc->cstime        );
  READ_NUMBER( arg, proc->priority      );
  READ_NUMBER( arg, proc->nice          );
  READ_NUMBER( arg, proc->timeout       );
  READ_NUMBER( arg, proc->it_real_value );
  READ_NUMBER( arg, proc->start_time    );
  READ_NUMBER( arg, proc->vsize         );
  READ_NUMBER( arg, proc->rss           );
  READ_NUMBER( arg, proc->rss_rlim      );
  READ_NUMBER( arg, proc->start_code    );
  READ_NUMBER( arg, proc->end_code      );
  READ_NUMBER( arg, proc->start_code    );
  READ_NUMBER( arg, proc->kstack_esp    );
  READ_NUMBER( arg, proc->kstack_eip    );
  READ_NUMBER( arg, proc->signal        );
  READ_NUMBER( arg, proc->blocked       );
  READ_NUMBER( arg, proc->sigignore     );
  READ_NUMBER( arg, proc->sigcatch      );
  READ_NUMBER( arg, proc->wchan         );

  /* 設定檔名 */
  sprintf( filename, "%s/%d/status" , PROC_DIR , pid );

  /* 開啟 proc file status 的內容 */
  if ( ( fd = open( filename , O_RDONLY ,0 ) ) < 0 )
  {
    mudlog( LOG_INFO , "get_proc: open:%s" , strerror( errno ) );
    RETURN( FALSE );
  }

  /* 讀取 proc file status 的內容 */
  if ( ( read( fd, buffer , sizeof( buffer ) - 1 ) ) <= 0 )
  {
    mudlog( LOG_INFO , "Get_proc: read:%s" , strerror( errno ) );
    close( fd );
    RETURN( FALSE );
  }

  /* 關閉檔案描述子 */
  close( fd );

  proc->uid = -1;
  proc->gid = -1;
  if ( ( arg = strstr( buffer , "Uid:" ) ) ) proc->uid = atoi( arg + 4 );
  if ( ( arg = strstr( buffer , "Gid:" ) ) ) proc->gid = atoi( arg + 4 );

  RETURN( TRUE );
}

#else
bool get_proc( pid_t pid, struct ProcFile * proc )
{
  int    fd;
  char   filename[MAX_FILE_LENGTH];
  char   buffer[512];
  char * arg;

  PUSH_FUNCTION( "get_proc" );

  /* 檢查 PID 是否 合法 */
  if ( pid <= 1 )
  {
    mudlog( LOG_INFO, "get_proc: pid 號碼 %d 不合法" , pid );
    RETURN( FALSE );
  }

  /* 檢查結構是否合法 */
  if ( proc == NULL )
  {
    mudlog( LOG_INFO, "get_proc: 結構 proc 是空的" );
    RETURN( FALSE );
  }

  /* 清除結構的資料 */
  memset( proc , 0, sizeof( struct ProcFile ) );

  /* 設定檔名 */
  sprintf( filename, "%s/%d/status", PROC_DIR, ( int ) pid );

  /* 開啟 proc file stat 的內容 */
  if ( ( fd = open( filename , O_RDONLY ,0 ) ) < 0 )
  {
    mudlog( LOG_INFO , "get_proc: open:%s" , strerror( errno ) );
    RETURN( FALSE );
  }

  /* 讀取 proc file stat 的內容 */
  if ( ( read( fd, buffer , sizeof( buffer ) - 1 ) ) <= 0 )
  {
    mudlog( LOG_INFO , "Get_proc: read:%s" , strerror( errno ) );
    close( fd );
    RETURN( FALSE );
  }

  /* 關閉檔案描述子 */
  close( fd );

  arg = buffer;
  sscanf( buffer, "%s %d %d %d %d %d,%d %s %d,%d"
    , proc->cmd, &proc->pid, &proc->ppid, &proc->pgrp, &proc->sid
    , &proc->maj_flt, &proc->min_flt, proc->ttyname
    , &proc->start_time, &proc->start_utime );

  RETURN( TRUE );
}

#endif

#undef READ_NUMBER
#undef READ_WORD
#undef READ_CHAR

FUNCTION( do_ps )
{
  struct ProcFile   sysinfo;
  struct dirent   * next;
  struct stat       st;
  DIR             * reading;
  pid_t             sid;
  int               uid;
  int               gid;
  bool              Found  = FALSE;
  bool              fAll   = FALSE;
  bool              fMerc  = FALSE;
  bool              fBack  = FALSE;
  bool              fGrp   = FALSE;
  bool              fUser  = FALSE;
  bool              fPrint;

  PUSH_FUNCTION( "do_ps" );

  if ( get_proc( merc_pid, &sysinfo ) == FALSE )
  {
    send_to_char( "讀取系統檔案發生錯誤。\n\r", ch );
    RETURN_NULL();
  }

  uid = sysinfo.uid;
  gid = sysinfo.gid;
  sid = getsid( 0 );

  for ( ; *argument != '\x0'; argument++ )
  {
    switch( *argument )
    {
    case 'a': fAll  = TRUE; break;
    case 'x': fBack = TRUE; break;
    case 'm': fMerc = TRUE; break;
    case 'u': fUser = TRUE; break;
    case 'g': fGrp  = TRUE; break;
    default:
      print_to_char( ch, "錯誤的參數 %c。 請查詢使用方法。\n\r", *argument );
      RETURN_NULL();
    }
  }

  clear_buffer();
  if ( ( reading = opendir( PROC_DIR ) ) )
  {
    /* 讀取目錄的內容 */
    while ( ( next = readdir( reading ) ) )
    {
      char filename[MAX_FILE_LENGTH];

      /* 檔名或是目錄名稱 */
      sprintf( filename, "%s/%s", PROC_DIR, next->d_name );

      if ( ( atoi( next->d_name ) <= 1
        || ( lstat( filename, &st ) < 0 )
        || ( ( st.st_mode & S_IFMT ) != S_IFDIR )
        || ( get_proc( atoi( next->d_name ), &sysinfo ) != TRUE ) ) )
        continue;

      fPrint = FALSE;

      if ( fBack )
      {
        if ( fAll )                        fPrint = TRUE;
        if ( fUser && sysinfo.uid == uid ) fPrint = TRUE;
        if ( fGrp  && sysinfo.gid == gid ) fPrint = TRUE;
        if ( fMerc && sysinfo.sid == sid ) fPrint = TRUE;
      }

      else if ( sysinfo.tty > 0 )
      {
        if ( fAll                        ) fPrint = TRUE;
        if ( fUser && sysinfo.uid == uid ) fPrint = TRUE;
        if ( fGrp  && sysinfo.gid == gid ) fPrint = TRUE;
        if ( fMerc && sysinfo.sid == sid ) fPrint = TRUE;
      }

      if ( sysinfo.sid == sid && sysinfo.ppid == 1 ) fPrint = TRUE;

      if ( fPrint )
      {
        struct timeval sTime;
        int            diff_sec;
        int            freq;

        Found         = TRUE;
        freq          = get_tick();
        diff_sec      = get_process_loading( &sysinfo );
        sTime.tv_sec  = diff_sec / freq;
        sTime.tv_usec = diff_sec % freq;

        send_to_buffer( "%5d %13s %8s %8s %5d %5d %s "
          , sysinfo.pid, sysinfo.cmd
          , pw_lookup( sysinfo.uid ), gr_lookup( sysinfo.gid )
          , sysinfo.sid, sysinfo.ppid, time_string( &sTime ) );

        diff_sec      = get_uptime() - sysinfo.start_time;
        sTime.tv_sec  = diff_sec / freq;
        sTime.tv_usec = diff_sec % freq;
        send_to_buffer( "%s\n\r", time_string( &sTime ) );
      }
    }

    if ( !Found ) send_to_buffer( "目前沒有任何的子行程﹗\n\r" );
    else send_to_char( "\r\e[44;1m 行程    執行的指令      UID "
     "     GID   SID  PPID 耗損時間       執行時間       \n\r\e[0m" , ch );

    print_buffer( ch );
    closedir( reading );
  }
  else
  {
    send_to_char( "無法開啟系統行程目錄。\n\r" , ch );
  }
  RETURN_NULL();
}

FUNCTION( do_flow )
{
  DESCRIPTOR_DATA * man;
  CHAR_DATA       * victim;
  char              buf[MAX_STRING_LENGTH];
  int               nTimes;

  PUSH_FUNCTION( "do_flow" );

  clear_buffer();
  send_to_buffer( "\e[1;33;44m玩家英文名稱 停 留 輸    出 平均 輸  入 平均"
    " 位                               址\e[0m\n\r" );

  for ( man = descriptor_list; man; man = man->next )
  {
    if ( verify_desc( man )
      && man->connected == CON_PLAYING
      && verify_char( ( victim = man->character ) ) )
    {
      if ( IS_SET( victim->act, PLR_WIZINVIS )
        && get_trust( victim ) > get_trust( ch ) ) continue;

      nTimes = ( int ) current_time - ( int ) victim->logon;

      str_cpy( buf, host_name( man ) );
      buf[35] = '\x0';

      send_to_buffer( "%-12s %5d %8d %s%4d\e[0m %6d %s%4d\e[0m %s\n\r"
        , victim->name
        , nTimes
        , man->nWrite
        , ( ( man->nWrite / nTimes ) > 200 ) ? "\e[1;31m" : ""
        , man->nWrite / nTimes
        , man->nRead
        , ( ( man->nRead / nTimes ) > 10 ) ? "\e[1;34m" : ""
        , man->nRead / nTimes
        , buf );
    }
  }

  print_buffer( ch );
  RETURN_NULL();
}

FUNCTION( do_okip )
{
  char              arg[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA * pDesc;
  IP_DATA         * pIp;
  IP_DATA         * zIp;
  int               count;

  PUSH_FUNCTION( "do_okip" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    if ( !( pDesc = ch->desc ) || !pDesc->host || !*pDesc->host )
    {
      send_to_char( "你目前缺乏連線資料﹐無法顯示﹗\n\r", ch );
      RETURN_NULL();
    }

    act( "你目前的連線位址為 $6$t$0 數字型態為﹕$6$T$0"
      , ch, pDesc->host, pDesc->address, TO_CHAR );
  }

  else if ( !str_prefix( arg, "list" ) )
  {
    if ( !ch->connect )
    {
      send_to_char( "你目前沒有設定任何限制連線位址﹐請查詢 okip﹗\n\r", ch );
      RETURN_NULL();
    }

    clear_buffer();
    send_to_buffer( "你目前設定的限制連線位址有如下﹕\n\r" );
    for ( count = 1, pIp = ch->connect; pIp; pIp = pIp->next, count++ )
    {
      if ( !pIp->address || !*pIp->address ) continue;

      send_to_buffer( "順序 %2d 限制連線位址為 「%s\e[0m」\n\r"
        , count, pIp->address );
    }

    print_buffer( ch );
  }

  else if ( !str_prefix( arg, "remove" ) )
  {
    if ( !ch->connect )
    {
      send_to_char( "你目前沒有設定任何連線位址﹐所以無法刪除﹗\n\r", ch );
      RETURN_NULL();
    }

    argument = one_argument( argument, arg );
    if ( arg[0] == '\x0' )
    {
      send_to_char( "對不起﹐你必須註明你的連線位址才能刪除﹗\n\r", ch );
      RETURN_NULL();
    }

    for ( zIp = NULL, pIp = ch->connect; pIp; zIp = pIp, pIp = pIp->next )
    {
      if ( !strcmp( arg, pIp->address ) )
      {
        if ( zIp == NULL ) ch->connect = ch->connect->next;
        else               zIp->next  = pIp->next;

        act( "你移除了$1$t$0這個限制連線位址﹗"
          , ch, pIp->address, NULL, TO_CHAR );

        free_string( pIp->address );
        free_struct( pIp, STRUCT_IP_DATA );
        RETURN_NULL();
      }
    }

    send_to_char( "對不起﹐沒有找到相關的位址﹗\n\r", ch );
  }

  else if ( !str_prefix( arg, "clear" ) )
  {
    if ( !ch->connect )
    {
      send_to_char( "你目前沒有設定任何連線位址﹐所以無法清除﹗\n\r", ch );
      RETURN_NULL();
    }

    for ( pIp = ch->connect; pIp; pIp = zIp )
    {
      zIp = pIp->next;
      if ( pIp->address ) free_string( pIp->address );
      free_struct( pIp, STRUCT_IP_DATA );
    }

    ch->connect = NULL;
    send_to_char( "已經全部清除你的所有限制連線位址資料﹗\n\r", ch );
  }

  else if ( !str_prefix( arg, "add" ) || !str_prefix( arg, "set" ) )
  {
    argument = one_argument( argument, arg );
    if ( arg[0] == '\x0' )
    {
      send_to_char( "對不起﹐你必須註明你的連線位址﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( ip_count( ch ) >= max_ip )
    {
      send_to_char( "對不起﹐你設定的位址太多了﹐無法再增加了﹗\n\r", ch );
      RETURN_NULL();
    }

    smash_tilde( arg );
    set_ip( ch, arg );
    act( "你限制今後只能由$t連接上線﹗", ch, arg, NULL, TO_CHAR );
  }

  else
  {
    send_to_char( "你的參數錯誤﹐請查詢 okip 的用法﹗\n\r", ch );
  }

  RETURN_NULL();
}

void set_ip( CHAR_DATA * ch, const char * address )
{
  IP_DATA * pIp;

  PUSH_FUNCTION( "set_ip" );

  if ( !ch || !address )
  {
    mudlog( LOG_DEBUG, "set_ip: 來源錯誤。" );
    RETURN_NULL();
  }

  if ( !*address ) RETURN_NULL();

  pIp = alloc_struct( STRUCT_IP_DATA );
  pIp->address = str_dup( address );
  pIp->next    = ch->connect;
  ch->connect  = pIp;

  RETURN_NULL();
}

size_t ip_count( CHAR_DATA * ch )
{
  size_t    count;
  IP_DATA * pIp;

  PUSH_FUNCTION( "ip_count" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "ip_count: 缺乏來源。" );
    RETURN( 0 );
  }

  if ( IS_NPC( ch ) ) RETURN( 0 );

  for ( count = 0, pIp = ch->connect; pIp; pIp = pIp->next ) count++;
  RETURN( count );
}

bool check_limit_ip( DESCRIPTOR_DATA * pDesc )
{
  CHAR_DATA * ch;
  IP_DATA   * pIp;
  char      * address;

  PUSH_FUNCTION( "check_limit_ip" );

  if ( !pDesc || !pDesc->host || !*pDesc->host || !( ch = pDesc->character ) )
  {
    mudlog( LOG_DEBUG, "check_limit_ip: 缺乏來源。" );
    RETURN( FALSE );
  }

  /* 如果沒有設定﹐那一定是對的 */
  if ( !ch->connect ) RETURN( TRUE );

  for ( pIp = ch->connect; pIp; pIp = pIp->next )
  {
    if ( !( address = pIp->address ) || !*address ) continue;

    if ( !str_prefix( address, address_mode( pDesc->host, ADDRESS_ALPHA ) )
      || !str_prefix( address, address_mode( pDesc->host, ADDRESS_NUMBER ) ) )
      RETURN( TRUE );
  }

  RETURN( FALSE );
}

char * get_OS_loading( void )
{
  static char buf[MAX_STRING_LENGTH];
  float       load_1;
  float       load_10;
  float       load_15;

  PUSH_FUNCTION( "get_OS_loading" );

  if ( !open_loading_file( &load_1, &load_10, &load_15 ) )
    RETURN( "無法開啟檔案讀取資料" );

  sprintf( buf,
    "一分鐘﹕\e[1;%dm%.2f\e[0m "
    "十分鐘﹕\e[1;%dm%.2f\e[0m "
    "十五分鐘﹕\e[1;%dm%.2f\e[0m"
    , ( load_1  * 100 ) >= OverLoading ? 31 : 32
    , load_1
    , ( load_10 * 100 ) >= OverLoading ? 31 : 32
    , load_10
    , ( load_15 * 100 ) >= OverLoading ? 31 : 32
    , load_15 );

  RETURN( buf );
}

char * get_mud_loading( void )
{
  static char buf[MAX_STRING_LENGTH];
  int         stime;
  int         ustime;
  float       load_1;
  float       load_10;
  float       load_15;

  PUSH_FUNCTION( "get_mud_loading" );

  load_1  = 0;
  load_10 = 0;
  load_15 = 0;

  if ( get_exec_time( 1, &stime, &ustime ) )
    load_1 = ( ( float ) calc_loading( 1, stime, ustime ) ) / 10000;

  if ( get_exec_time( 10, &stime, &ustime ) )
    load_10 = ( ( float ) calc_loading( 10, stime, ustime ) ) / 10000;

  if ( get_exec_time( 15, &stime, &ustime ) )
    load_15 = ( ( float ) calc_loading( 15, stime, ustime ) ) / 10000;

  sprintf( buf,
    "一分鐘﹕\e[1;%dm%.2f\e[0m "
    "十分鐘﹕\e[1;%dm%.2f\e[0m "
    "十五分鐘﹕\e[1;%dm%.2f\e[0m"
    , ( load_1  * 100 ) >= OverLoading / 2 ? 31 : 32
    , load_1
    , ( load_10 * 100 ) >= OverLoading / 2 ? 31 : 32
    , load_10
    , ( load_15 * 100 ) >= OverLoading / 2 ? 31 : 32
    , load_15 );

  RETURN( buf );
}

bool open_loading_file( float * load_1, float * load_10, float * load_15 )
{
  FILE * pFile;

  PUSH_FUNCTION( "open_loading_file" );

  *load_1  = 0;
  *load_10 = 0;
  *load_15 = 0;

  if ( !( pFile = fopen( LOADING_FILE , "r" ) ) ) RETURN( FALSE );

  fscanf( pFile, "%f %f %f", load_1, load_10, load_15 );
  fclose( pFile );

  RETURN( TRUE );
}

void loading_warning( CHAR_DATA * ch )
{
  float load_1;
  float load_10;
  float load_15;

  PUSH_FUNCTION( "loading_warning" );

  if ( open_loading_file( &load_1, &load_10, &load_15 ) )
  {
    if ( ( load_1 * 100 ) >= OverLoading
      || ( load_10 * 100 ) >= OverLoading
      || ( load_15 * 100 ) >= OverLoading )
    {
      act( "$B$B$B$2「系統警告」$0主機系統的負載量超限﹐$t$0﹗"
        , ch, get_OS_loading(), NULL, TO_CHAR );
    }
  }

  RETURN_NULL();
}

FUNCTION( do_rlimit )
{
  struct rlimit pLim;

  PUSH_FUNCTION( "do_rlimit" );

  clear_buffer();

  send_to_buffer( "\e[1;33;44m項          目            軟性限制"
    "             硬性限制                         \e[0m\n\r" );

  if ( getrlimit( RLIMIT_CPU, &pLim ) == 0 )
  {
    send_to_buffer( "ＣＰＵ使用時間限制" );
    send_to_buffer( "%s     秒 "    , rlimit_str( pLim.rlim_cur ) );
    send_to_buffer( "%s     秒\n\r" , rlimit_str( pLim.rlim_max ) );
  }

  if ( getrlimit( RLIMIT_FSIZE, &pLim ) == 0 )
  {
    send_to_buffer( "開啟檔案最大大小  " );
    send_to_buffer( "%s 位元組 "    , rlimit_str( pLim.rlim_cur ) );
    send_to_buffer( "%s 位元組\n\r" , rlimit_str( pLim.rlim_max ) );
  }

  if ( getrlimit( RLIMIT_DATA, &pLim ) == 0 )
  {
    send_to_buffer( "最大資料記憶體大小" );
    send_to_buffer( "%s 位元組 "    , rlimit_str( pLim.rlim_cur ) );
    send_to_buffer( "%s 位元組\n\r" , rlimit_str( pLim.rlim_max ) );
  }

  if ( getrlimit( RLIMIT_STACK, &pLim ) == 0 )
  {
    send_to_buffer( "最大堆疊記憶體大小" );
    send_to_buffer( "%s 位元組 "    , rlimit_str( pLim.rlim_cur ) );
    send_to_buffer( "%s 位元組\n\r" , rlimit_str( pLim.rlim_max ) );
  }

  if ( getrlimit( RLIMIT_CORE, &pLim ) == 0 )
  {
    send_to_buffer( "最大核心映射檔大小" );
    send_to_buffer( "%s 位元組 "    , rlimit_str( pLim.rlim_cur ) );
    send_to_buffer( "%s 位元組\n\r" , rlimit_str( pLim.rlim_max ) );
  }

  #if defined(RLIMIT_RSS)
  if ( getrlimit( RLIMIT_RSS, &pLim ) == 0 )
  {
    send_to_buffer( "可使用的最大記憶體" );
    send_to_buffer( "%s 位元組 "    , rlimit_str( pLim.rlim_cur ) );
    send_to_buffer( "%s 位元組\n\r" , rlimit_str( pLim.rlim_max ) );
  }
  #endif

  #if defined(RLIMIT_NPROC)
  if ( getrlimit( RLIMIT_NPROC, &pLim ) == 0 )
  {
    send_to_buffer( "最多可產生的子程序" );
    send_to_buffer( "%s     個 "    , rlimit_str( pLim.rlim_cur ) );
    send_to_buffer( "%s     個\n\r" , rlimit_str( pLim.rlim_max ) );
  }
  #endif

  if ( getrlimit( RLIMIT_NOFILE, &pLim ) == 0 )
  {
    send_to_buffer( "開啟檔案數目上限  " );
    send_to_buffer( "%s     個 "    , rlimit_str( pLim.rlim_cur ) );
    send_to_buffer( "%s     個\n\r" , rlimit_str( pLim.rlim_max ) );
  }

  print_buffer( ch );
  RETURN_NULL();
}

char * rlimit_str( int value )
{
  static char buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "rlimit_string" );

  if ( value == ( int ) RLIM_INFINITY ) RETURN( "        無窮大" );

  sprintf( buf, "%14s", numberize( value, NULL ) );
  RETURN( buf );
}

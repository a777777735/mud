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
  char cmd[40];       /* �R�O               Bsd */
  char ttyname[20];   /*                    Bsd, no Linux */
  char state;         /* ��{���A           */
  int pid;            /* ��{ PID           Bsd */
  int ppid;           /* ����{ PID         Bsd */
  int pgrp;           /* �s�ڦ�{���X       Bsd */
  int sid;            /* Session ID         Bsd */
  int uid;            /* �ϥΪ̪�ID���X     */
  int gid;            /* �ϥΪ̪��s��ID���X */
  int tty;            /* TTY                */
  int tpgid;          /* �׺ݾ���{ID       */
  int flags;          /* ��{�X��           */
  int min_flt;        /* �� PAGE �ɤ��ݭn�Ψ� I/O �ʧ@������       Bsd */
  int maj_flt;        /* �l��{�� PAGE �ɤ��ݭn�Ψ� I/O �ʧ@������ Bsd */
  int cmin_flt;       /* �� PAGE �ɻݭn�Ψ� I/O �ʧ@������         */
  int cmaj_flt;       /* �l��{�� PAGE �ɻݭn�Ψ� I/O �ʧ@������   */
  int utime;          /* ��{�үӪ��ɶ�                              */
  int stime;          /* �֤߳Q��{�үӪ��ɶ�                        */
  int cutime;         /* ��{���l��{�үӪ��ɶ�                      */
  int cstime;         /* �֤߳Q��{���l��{�үӪ��ɶ�                */
  int priority;       /* �u���v                                      */
  int nice;           /* Nice                    */
  int timeout;        /* �����γ~                */
  int it_real_value;  /* �����γ~                */
  int start_time;     /* �Ұʮɶ�                Bsd */
  int start_utime;    /* �Ұʮɶ�                Bsd, no Linux */
  int vsize;          /* �����O���骺����        */
  int rss;            /* ���s���O����            */
  int rss_rlim;       /* �����γ~                */
  int start_code;     /* Code Segment �_�Ϧ�}   */
  int end_code;       /* Code Segment ������}   */
  int start_stack;    /* ���|��������}          */
  int kstack_esp;     /* �֤߰��|��ESP�Ȧs�����e */
  int kstack_eip;     /* �֤߰��|��EIP�Ȧs�����e */
  int signal;         /* pending �T�����B�n */
  int blocked;        /* blocked �T�����B�n */
  int sigignore;      /* ignored �T�����B�n */
  int sigcatch;       /* catch   �T�����B�n */
  int wchan;          /* �֤ߵ��ݦ�{����} */
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

/* Ū���t�Τ@���T */
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
      send_to_char( "Ū���t���ɮ׵o�Ϳ��~�C\n\r", ch );
      RETURN_NULL();
    }

    freq = get_tick();
    clear_buffer();

    for ( loop = 0; loop < MAX_PORT; loop++ )
    {
      if ( MudPort[loop] == ERRORCODE ) break;
      send_to_buffer( "�s����R%5d �s���`�H���R%5d\n\r"
        , MudPort[loop], MudConnect[loop] );
    }

    #ifdef __linux__
    send_to_buffer( "�����t�ΰO����R%d �d�줸�աC\n\r"
      , ( sysinfo.vsize >> 10 ) );

    send_to_buffer( "�t�α`�n�O����R%d �d�줸�աC\n\r"
      , ( sysinfo.rss << 2 ) );
    #endif

    getrusage( RUSAGE_SELF    , &usage_self );
    getrusage( RUSAGE_CHILDREN, &usage_chld );

    sTime.tv_sec  = usage_self.ru_utime.tv_sec;
    sTime.tv_usec = usage_self.ru_utime.tv_usec;

    send_to_buffer( "�D��{���`���Ӯɶ��R%s ��C\n\r", time_string( &sTime ) );

    sTime.tv_sec  = usage_chld.ru_utime.tv_sec;
    sTime.tv_usec = usage_chld.ru_utime.tv_usec;
    send_to_buffer( "�l��{���`���Ӯɶ��R%s ��C\n\r", time_string( &sTime ) );

    sTime.tv_sec  = usage_self.ru_stime.tv_sec;
    sTime.tv_usec = usage_self.ru_stime.tv_usec;
    send_to_buffer( "�D��{�t�ή��Ӯɶ��R%s ��C\n\r", time_string( &sTime ) );

    sTime.tv_sec  = usage_chld.ru_stime.tv_sec;
    sTime.tv_usec = usage_chld.ru_stime.tv_usec;
    send_to_buffer( "�l��{�t�ή��Ӯɶ��R%s ��C\n\r", time_string( &sTime ) );

    sTime.tv_sec = usage_self.ru_stime.tv_sec
                 + usage_self.ru_utime.tv_sec
                 + usage_chld.ru_stime.tv_sec
                 + usage_chld.ru_stime.tv_sec;

    sTime.tv_usec = usage_self.ru_stime.tv_usec
                  + usage_self.ru_utime.tv_usec
                  + usage_chld.ru_stime.tv_usec
                  + usage_chld.ru_stime.tv_usec;

    send_to_buffer( "��{�����`�M�ɶ��R%s ��C\n\r", time_string( &sTime ) );

    total = get_uptime() - mud_boot_tick;
    sTime.tv_sec  = total / freq;
    sTime.tv_usec = total % freq;

    send_to_buffer( "��{�ֿn�`�M�ɶ��R%s ��C\n\r", time_string( &sTime ) );
    send_to_buffer( "��{�����t���q�šR%s%%�C\n\r" , get_loading() );

    send_to_buffer( "�`�g�X�줸��  �R%d �d�줸�� %d �줸�աC\n\r"
      , WriteKiloBytes, WriteBytes );

    flow = ( WriteKiloBytes * 1000 + WriteBytes ) / UMAX( sTime.tv_sec , 1 );
    send_to_buffer( "������X�y�q  �R�C�� %d �줸�աC\n\r", (int) flow );

    send_to_buffer( "�`Ū�J�줸��  �R%d �d�줸�� %d �줸�աC\n\r"
      , ReadKiloBytes, ReadBytes );

    flow = ( ReadKiloBytes * 1000 + ReadBytes ) / UMAX( sTime.tv_sec , 1 );
    send_to_buffer( "������J�y�q  �R�C�� %d �줸�աC\n\r", (int) flow );

    send_to_buffer( "�D���̪񪺭t���R%s�C\n\r", get_OS_loading()  );
    send_to_buffer( "�t�γ̪񪺭t���R%s�C\n\r", get_mud_loading() );

    send_to_buffer( "�t���`���U�H�ơR%s �H�C\n\r"
      , numberize( database_count( DATABASE_EXIST ), NULL ) );

    print_buffer( ch );
  }
  else if ( !str_prefix( arg, "loading" ) )
  {
    if ( argument[0] == '\x0' || !is_number( argument ) )
    {
      send_to_char( "�A�n�p��X�����e�ܥثe���t���q�S\n\r", ch );
      RETURN_NULL();
    }

    if ( ( loop = atoi( argument ) ) <= 0 )
    {
      send_to_char( "�A�������ƥ����j��s����ơT\n\r", ch );
      RETURN_NULL();
    }

    if ( !get_exec_time( loop, &stime, &ustime ) )
    {
      send_to_char( "�S������[���e����ƥi�ѭp��T\n\r", ch );
      RETURN_NULL();
    }

    chinese_number( loop, buf );
    total = calc_loading( loop, stime, ustime );

    print_to_char( ch, "%s\e[0m�t�ΡM%s�����e�ܥثe�������t���q��"
      " %d.%d%d%%\n\r"
      , mud_name
      , buf, total / 100, ( total % 100 ) / 10 , ( total % 100 ) % 10);
  }
  else
  {
    send_to_char( "�藍�_�M�A���y�k���~�M�Ьd�� sysconf ���ϥΤ�k�T\n\r", ch );
  }
  RETURN_NULL();
}

#ifdef __linux__
/* ����\Ū�t�ΧC����T��� */
FUNCTION( do_debug )
{
  struct ProcFile sysinfo;

  PUSH_FUNCTION( "do_debug" );

  /* �Y�OŪ�����o�Ϳ��~ */
  if ( get_proc( merc_pid, &sysinfo ) == FALSE )
  {
    send_to_char( "Ū�� proc �ɮצ����~�o�͡C\n\r" , ch );
    RETURN_NULL();
  }

  clear_buffer();
  send_to_buffer( "�ثe��{���X(PID)�R                          %d\n\r"
                  "���{���X(PPID)�R                             %d\n\r"
                  "���A�R                                       "
    , sysinfo.pid, sysinfo.ppid );

  switch( sysinfo.state )
  {
    default  : send_to_buffer( "����"            ); break;
    case 'I' : send_to_buffer( "���m(I)"         ); break;
    case 'O' : send_to_buffer( "���b�B�z(O)"     ); break;
    case 'R' : send_to_buffer( "�����C��(R)"   ); break;
    case 'S' : send_to_buffer( "�H�Τ�(S)"       ); break;
    case 'T' : send_to_buffer( "���(T)"       ); break;
    case 'X' : send_to_buffer( "���ݰO���餤(X)" ); break;
    case 'Z' : send_to_buffer( "�b����(Z)"       ); break;
  }

  send_to_buffer( "\n\r"
                  "�s�ڦ�{���X(PGRP)�R                         %d\n\r"
                  "Session��{���X(SID)�R                       %d\n\r"
                  "����x�R                                     %d\n\r"
                  "�׺ݾ���{���X(TPGID)�R                      %d\n\r"
                  "��{�X�СR                                   %d\n\r"
                  "�� PAGE �ɤ��ݭn�Ψ� I/O �ʧ@�����ơR      %d\n\r"
                  "�l��{�� PAGE �ɤ��ݭn�Ψ� I/O �ʧ@�����ơR%d\n\r"
                  "�� PAGE �ɻݭn�Ψ� I/O �ʧ@�����ơR        %d\n\r"
                  "�l��{�� PAGE �ɻݭn�Ψ� I/O �ʧ@�����ơR  %d\n\r"
                  "��{�үӪ��ɶ��R                             %d\n\r"
                  "�֤߳Q��{�үӪ��ɶ��R                       %d\n\r"
                  "��{���l��{�үӪ��ɶ��R                     %d\n\r"
                  "�֤߳Q��{���l��{�үӪ��ɶ��R               %d\n\r"
                  "�u���v�R                                     %d\n\r"
                  "Nice�R                                       %d\n\r"
                  "�Ұʮɶ��R                                   %d\n\r"
                  "�����O���骺�j�p�R                           %d\n\r"
                  "���s���O����                                 %d\n\r"
                  "Code Segment �_�Ϧ�}                        %d\n\r"
                  "Code Segment ������}                        %d\n\r"
                  "���|��������}                               %d\n\r"
                  "�֤߰��|��ESP�Ȧs�����e                      %d\n\r"
                  "�֤߰��|��EIP�Ȧs�����e                      %d\n\r"
                  "pending �T�����B�n                           %d\n\r"
                  "blocked �T�����B�n                           %d\n\r"
                  "ignored �T�����B�n                           %d\n\r"
                  "catch   �T�����B�n                           %d\n\r"
                  "�֤ߵ��ݦ�{����}                           %d\n\r"
                  "�W�v(HZ)                                     %d\n\r"
                  "�t�έt��                                     %s%%\n\r"
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
  send_to_char( "�����@�~�t�Τ����ѳo���A�ȡC\n\r", ch );
  RETURN_NULL();
}
#endif

/* �p��X�t�Ϊ��t���q */
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

/* �C�L�X�t�Ϊ���T */
FUNCTION( do_uname )
{
  struct utsname name;

  PUSH_FUNCTION( "do_uname" );

  if ( uname( &name ) < 0 )
  {
    send_to_char( "��� uname() �����D�C" , ch );
    RETURN_NULL();
  }

  print_to_char( ch ,
    "��� uname() ����T�R\n\r"
    "�@�~�t�ΦW�١R%s\n\r"
    "�@�~�t�Ϊ����R%s\n\r"
    "�t�θ`�I�W�١R%s\n\r"
    "�t�Υ��������R%s\n\r"
    "�t�ξ������A�R%s\n\r"
    "Domain name �R%s\n\r"
    , name.sysname
    , name.release
    , name.nodename
    , name.version
    , name.machine );

  #ifdef __linux__
  print_to_char( ch, "Domain name �R%s\n\r", name.domainname );
  #endif

  RETURN_NULL();
}

/* ���o�t�Ϊ��պA */
FUNCTION( do_sysconf )
{
  PUSH_FUNCTION( "do_sysconf" );

  clear_buffer();

  send_to_buffer( "�C�ӨϥΪ̳̦h��{��   �R%d\n\r"
    , sysconf( _SC_CHILD_MAX ) );

  send_to_buffer( "�C���t�ΰO�ƾ�����   �R%d\n\r" , get_tick() );

  send_to_buffer( "��{�̦h�i�H�}�X�Ӷ׬y �R%d\n\r"
    , sysconf( _SC_STREAM_MAX ) );

  send_to_buffer( "��{�̦h�i�H�}�X���ɮ� �R%d\n\r"
    , sysconf( _SC_OPEN_MAX ) );

  send_to_buffer( "�O�_�䴩 Job ����      �R%s\n\r"
    , YESNO( sysconf( _SC_JOB_CONTROL ) ) );

  send_to_buffer( "�O�_�䴩�O�� IDS       �R%s\n\r"
    , YESNO( sysconf( _SC_SAVED_IDS ) ) );

  send_to_buffer( "�t�ΩҤ䴩�� POSIX �����R%d.%d\n\r"
    , sysconf( _SC_VERSION ) / 100
    , sysconf( _SC_VERSION ) % 100 );

  send_to_buffer( "�t�ΩҤ䴩��POSIX2 �����R%d.%d\n\r"
    , sysconf( _SC_2_VERSION ) / 100
    , sysconf( _SC_2_VERSION ) % 100 );

  print_buffer( ch );
  RETURN_NULL();
}

/* ���o�t�ζ}����ثe��� Tick �� */
unsigned long get_uptime( void )
{
  struct tms pTms;

  PUSH_FUNCTION( "getuptime" );
  RETURN( times( &pTms ) );
}

/* ���o�t�Ϊ� Tick */
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
/* ���o proc file ���ɮ� */
bool get_proc( pid_t pid, struct ProcFile * proc )
{
  int    fd;
  char   filename[MAX_FILE_LENGTH];
  char   buffer[512];
  char * arg;

  PUSH_FUNCTION( "get_proc" );

  /* �ˬd PID �O�_ �X�k */
  if ( pid <= 1 )
  {
    mudlog( LOG_INFO, "get_proc: pid ���X %d ���X�k" , pid );
    RETURN( FALSE );
  }

  /* �ˬd���c�O�_�X�k */
  if ( proc == NULL )
  {
    mudlog( LOG_INFO, "get_proc: ���c proc �O�Ū�" );
    RETURN( FALSE );
  }

  /* �M�����c����� */
  memset( proc , 0, sizeof( struct ProcFile ) );

  /* �]�w�ɦW */
  sprintf( filename, "%s/%d/stat" , PROC_DIR , pid );

  /* �}�� proc file stat �����e */
  if ( ( fd = open( filename , O_RDONLY ,0 ) ) < 0 )
  {
    mudlog( LOG_INFO , "get_proc: open:%s" , strerror( errno ) );
    RETURN( FALSE );
  }

  /* Ū�� proc file stat �����e */
  if ( ( read( fd, buffer , sizeof( buffer ) - 1 ) ) <= 0 )
  {
    mudlog( LOG_INFO , "Get_proc: read:%s" , strerror( errno ) );
    close( fd );
    RETURN( FALSE );
  }

  /* �����ɮ״y�z�l */
  close( fd );

  /* �q�w�İϤ�Ū����� */
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

  /* �]�w�ɦW */
  sprintf( filename, "%s/%d/status" , PROC_DIR , pid );

  /* �}�� proc file status �����e */
  if ( ( fd = open( filename , O_RDONLY ,0 ) ) < 0 )
  {
    mudlog( LOG_INFO , "get_proc: open:%s" , strerror( errno ) );
    RETURN( FALSE );
  }

  /* Ū�� proc file status �����e */
  if ( ( read( fd, buffer , sizeof( buffer ) - 1 ) ) <= 0 )
  {
    mudlog( LOG_INFO , "Get_proc: read:%s" , strerror( errno ) );
    close( fd );
    RETURN( FALSE );
  }

  /* �����ɮ״y�z�l */
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

  /* �ˬd PID �O�_ �X�k */
  if ( pid <= 1 )
  {
    mudlog( LOG_INFO, "get_proc: pid ���X %d ���X�k" , pid );
    RETURN( FALSE );
  }

  /* �ˬd���c�O�_�X�k */
  if ( proc == NULL )
  {
    mudlog( LOG_INFO, "get_proc: ���c proc �O�Ū�" );
    RETURN( FALSE );
  }

  /* �M�����c����� */
  memset( proc , 0, sizeof( struct ProcFile ) );

  /* �]�w�ɦW */
  sprintf( filename, "%s/%d/status", PROC_DIR, ( int ) pid );

  /* �}�� proc file stat �����e */
  if ( ( fd = open( filename , O_RDONLY ,0 ) ) < 0 )
  {
    mudlog( LOG_INFO , "get_proc: open:%s" , strerror( errno ) );
    RETURN( FALSE );
  }

  /* Ū�� proc file stat �����e */
  if ( ( read( fd, buffer , sizeof( buffer ) - 1 ) ) <= 0 )
  {
    mudlog( LOG_INFO , "Get_proc: read:%s" , strerror( errno ) );
    close( fd );
    RETURN( FALSE );
  }

  /* �����ɮ״y�z�l */
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
    send_to_char( "Ū���t���ɮ׵o�Ϳ��~�C\n\r", ch );
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
      print_to_char( ch, "���~���Ѽ� %c�C �Ьd�ߨϥΤ�k�C\n\r", *argument );
      RETURN_NULL();
    }
  }

  clear_buffer();
  if ( ( reading = opendir( PROC_DIR ) ) )
  {
    /* Ū���ؿ������e */
    while ( ( next = readdir( reading ) ) )
    {
      char filename[MAX_FILE_LENGTH];

      /* �ɦW�άO�ؿ��W�� */
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

    if ( !Found ) send_to_buffer( "�ثe�S�����󪺤l��{�T\n\r" );
    else send_to_char( "\r\e[44;1m ��{    ���檺���O      UID "
     "     GID   SID  PPID �ӷl�ɶ�       ����ɶ�       \n\r\e[0m" , ch );

    print_buffer( ch );
    closedir( reading );
  }
  else
  {
    send_to_char( "�L�k�}�Ҩt�Φ�{�ؿ��C\n\r" , ch );
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
  send_to_buffer( "\e[1;33;44m���a�^��W�� �� �d ��    �X ���� ��  �J ����"
    " ��                               �}\e[0m\n\r" );

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
      send_to_char( "�A�ثe�ʥF�s�u��ơM�L�k��ܡT\n\r", ch );
      RETURN_NULL();
    }

    act( "�A�ثe���s�u��}�� $6$t$0 �Ʀr���A���R$6$T$0"
      , ch, pDesc->host, pDesc->address, TO_CHAR );
  }

  else if ( !str_prefix( arg, "list" ) )
  {
    if ( !ch->connect )
    {
      send_to_char( "�A�ثe�S���]�w���󭭨�s�u��}�M�Ьd�� okip�T\n\r", ch );
      RETURN_NULL();
    }

    clear_buffer();
    send_to_buffer( "�A�ثe�]�w������s�u��}���p�U�R\n\r" );
    for ( count = 1, pIp = ch->connect; pIp; pIp = pIp->next, count++ )
    {
      if ( !pIp->address || !*pIp->address ) continue;

      send_to_buffer( "���� %2d ����s�u��}�� �u%s\e[0m�v\n\r"
        , count, pIp->address );
    }

    print_buffer( ch );
  }

  else if ( !str_prefix( arg, "remove" ) )
  {
    if ( !ch->connect )
    {
      send_to_char( "�A�ثe�S���]�w����s�u��}�M�ҥH�L�k�R���T\n\r", ch );
      RETURN_NULL();
    }

    argument = one_argument( argument, arg );
    if ( arg[0] == '\x0' )
    {
      send_to_char( "�藍�_�M�A���������A���s�u��}�~��R���T\n\r", ch );
      RETURN_NULL();
    }

    for ( zIp = NULL, pIp = ch->connect; pIp; zIp = pIp, pIp = pIp->next )
    {
      if ( !strcmp( arg, pIp->address ) )
      {
        if ( zIp == NULL ) ch->connect = ch->connect->next;
        else               zIp->next  = pIp->next;

        act( "�A�����F$1$t$0�o�ӭ���s�u��}�T"
          , ch, pIp->address, NULL, TO_CHAR );

        free_string( pIp->address );
        free_struct( pIp, STRUCT_IP_DATA );
        RETURN_NULL();
      }
    }

    send_to_char( "�藍�_�M�S������������}�T\n\r", ch );
  }

  else if ( !str_prefix( arg, "clear" ) )
  {
    if ( !ch->connect )
    {
      send_to_char( "�A�ثe�S���]�w����s�u��}�M�ҥH�L�k�M���T\n\r", ch );
      RETURN_NULL();
    }

    for ( pIp = ch->connect; pIp; pIp = zIp )
    {
      zIp = pIp->next;
      if ( pIp->address ) free_string( pIp->address );
      free_struct( pIp, STRUCT_IP_DATA );
    }

    ch->connect = NULL;
    send_to_char( "�w�g�����M���A���Ҧ�����s�u��}��ơT\n\r", ch );
  }

  else if ( !str_prefix( arg, "add" ) || !str_prefix( arg, "set" ) )
  {
    argument = one_argument( argument, arg );
    if ( arg[0] == '\x0' )
    {
      send_to_char( "�藍�_�M�A���������A���s�u��}�T\n\r", ch );
      RETURN_NULL();
    }

    if ( ip_count( ch ) >= max_ip )
    {
      send_to_char( "�藍�_�M�A�]�w����}�Ӧh�F�M�L�k�A�W�[�F�T\n\r", ch );
      RETURN_NULL();
    }

    smash_tilde( arg );
    set_ip( ch, arg );
    act( "�A�����u���$t�s���W�u�T", ch, arg, NULL, TO_CHAR );
  }

  else
  {
    send_to_char( "�A���Ѽƿ��~�M�Ьd�� okip ���Ϊk�T\n\r", ch );
  }

  RETURN_NULL();
}

void set_ip( CHAR_DATA * ch, const char * address )
{
  IP_DATA * pIp;

  PUSH_FUNCTION( "set_ip" );

  if ( !ch || !address )
  {
    mudlog( LOG_DEBUG, "set_ip: �ӷ����~�C" );
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
    mudlog( LOG_DEBUG, "ip_count: �ʥF�ӷ��C" );
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
    mudlog( LOG_DEBUG, "check_limit_ip: �ʥF�ӷ��C" );
    RETURN( FALSE );
  }

  /* �p�G�S���]�w�M���@�w�O�諸 */
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
    RETURN( "�L�k�}���ɮ�Ū�����" );

  sprintf( buf,
    "�@�����R\e[1;%dm%.2f\e[0m "
    "�Q�����R\e[1;%dm%.2f\e[0m "
    "�Q�������R\e[1;%dm%.2f\e[0m"
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
    "�@�����R\e[1;%dm%.2f\e[0m "
    "�Q�����R\e[1;%dm%.2f\e[0m "
    "�Q�������R\e[1;%dm%.2f\e[0m"
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
      act( "$B$B$B$2�u�t��ĵ�i�v$0�D���t�Ϊ��t���q�W���M$t$0�T"
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

  send_to_buffer( "\e[1;33;44m��          ��            �n�ʭ���"
    "             �w�ʭ���                         \e[0m\n\r" );

  if ( getrlimit( RLIMIT_CPU, &pLim ) == 0 )
  {
    send_to_buffer( "�Ѣޢ�ϥήɶ�����" );
    send_to_buffer( "%s     �� "    , rlimit_str( pLim.rlim_cur ) );
    send_to_buffer( "%s     ��\n\r" , rlimit_str( pLim.rlim_max ) );
  }

  if ( getrlimit( RLIMIT_FSIZE, &pLim ) == 0 )
  {
    send_to_buffer( "�}���ɮ׳̤j�j�p  " );
    send_to_buffer( "%s �줸�� "    , rlimit_str( pLim.rlim_cur ) );
    send_to_buffer( "%s �줸��\n\r" , rlimit_str( pLim.rlim_max ) );
  }

  if ( getrlimit( RLIMIT_DATA, &pLim ) == 0 )
  {
    send_to_buffer( "�̤j��ưO����j�p" );
    send_to_buffer( "%s �줸�� "    , rlimit_str( pLim.rlim_cur ) );
    send_to_buffer( "%s �줸��\n\r" , rlimit_str( pLim.rlim_max ) );
  }

  if ( getrlimit( RLIMIT_STACK, &pLim ) == 0 )
  {
    send_to_buffer( "�̤j���|�O����j�p" );
    send_to_buffer( "%s �줸�� "    , rlimit_str( pLim.rlim_cur ) );
    send_to_buffer( "%s �줸��\n\r" , rlimit_str( pLim.rlim_max ) );
  }

  if ( getrlimit( RLIMIT_CORE, &pLim ) == 0 )
  {
    send_to_buffer( "�̤j�֤߬M�g�ɤj�p" );
    send_to_buffer( "%s �줸�� "    , rlimit_str( pLim.rlim_cur ) );
    send_to_buffer( "%s �줸��\n\r" , rlimit_str( pLim.rlim_max ) );
  }

  #if defined(RLIMIT_RSS)
  if ( getrlimit( RLIMIT_RSS, &pLim ) == 0 )
  {
    send_to_buffer( "�i�ϥΪ��̤j�O����" );
    send_to_buffer( "%s �줸�� "    , rlimit_str( pLim.rlim_cur ) );
    send_to_buffer( "%s �줸��\n\r" , rlimit_str( pLim.rlim_max ) );
  }
  #endif

  #if defined(RLIMIT_NPROC)
  if ( getrlimit( RLIMIT_NPROC, &pLim ) == 0 )
  {
    send_to_buffer( "�̦h�i���ͪ��l�{��" );
    send_to_buffer( "%s     �� "    , rlimit_str( pLim.rlim_cur ) );
    send_to_buffer( "%s     ��\n\r" , rlimit_str( pLim.rlim_max ) );
  }
  #endif

  if ( getrlimit( RLIMIT_NOFILE, &pLim ) == 0 )
  {
    send_to_buffer( "�}���ɮ׼ƥؤW��  " );
    send_to_buffer( "%s     �� "    , rlimit_str( pLim.rlim_cur ) );
    send_to_buffer( "%s     ��\n\r" , rlimit_str( pLim.rlim_max ) );
  }

  print_buffer( ch );
  RETURN_NULL();
}

char * rlimit_str( int value )
{
  static char buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "rlimit_string" );

  if ( value == ( int ) RLIM_INFINITY ) RETURN( "        �L�a�j" );

  sprintf( buf, "%14s", numberize( value, NULL ) );
  RETURN( buf );
}

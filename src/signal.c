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
  mudlog( LOG_EMERG , "執行終端機失去連線, 導致行程中斷" );
  return;
}

void sig_int( int sig )
{
  mudlog( LOG_EMERG , "終端機執行者按下中斷鍵( Crtl-C )導致程式終止" );
  return;
}

void sig_term( int sig )
{
  mudlog( LOG_EMERG , "主機進行關機程序, 導致行程中斷" );
  return;
}

void sig_ill( int sig )
{
  mudlog( LOG_EMERG , "執行非法硬體指令集" );
  return;
}

void sig_fpe( int sig )
{
  mudlog( LOG_EMERG , "分母為零或是浮點數溢位" );
  return;
}

void sig_segv( int sig )
{
  mudlog( LOG_EMERG , "存取不合法的位址, 程式不正常終止" );
  return;
}

void sig_bus( int sig )
{
  mudlog( LOG_EMERG , "存取不合法的位址, 程式參考不當邊界位址" );
  return;
}

void sig_usr1( int sig )
{
  mudlog( LOG_EMERG, "使用者中斷(1)" );
  abort();
  return;
}

void sig_usr2( int sig )
{
  mudlog( LOG_INFO, "使用者中斷(2)" );
  do_reboot( NULL , "" );
  return;
}

void sig_quit( int sig )
{
  mudlog( LOG_EMERG, "終端機使用者按下離開鍵( Ctrl-\\ )導致程式終止" );
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
  if ( !traceloop ) mudlog( LOG_EMERG , "程式陷入無窮回圈, 程式自動關閉." );
  traceloop = 0;
  return;
}

/* 把函數名字推入堆疊 */
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
        mudlog( LOG_STACK , "除錯堆疊溢位(POP), 程式自動關閉." );
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

  /* 關閉所有網際連線 */
  mudlog( LOG_INFO, "嘗試關閉所有網際連線" );

  for ( pNet = net_list; pNet; pNet = pNet->next )
  {
    if ( valid_socket( pNet, NET_IMPORT ) )
      close_net_socket( pNet, NET_IMPORT );

    if ( valid_socket( pNet, NET_EXPORT ) )
      close_net_socket( pNet, NET_EXPORT );
  }

  /* 先取消拍賣 */
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
    mudlog( LOG_INFO , "正常關閉拍賣資料結構." );
  }
  else
  {
    mudlog( LOG_INFO , "沒有拍賣資料, 不需要關閉" );
  }

  /* 位址資訊檔案存檔 */
  mudlog( LOG_INFO , "儲存所有位址資料" );
  do_address( NULL , "!save" );

  /* 股票檔案存檔 */
  mudlog( LOG_INFO, "儲存股票資料檔案" );
  save_stock();

  mudlog( LOG_INFO , "嘗試儲存所有的玩家資料." );

  for ( ch = char_list; ch; ch = ch->next )
  {
    if ( !verify_char( ch ) || IS_NPC( ch ) ) continue;

    mudlog( LOG_INFO , "嘗試回存玩家 %s 的資料" , ch->name );
    save_char_obj( ch, SAVE_FILE );
    extract_char( ch, TRUE );
    if ( ch->desc ) close_socket( ch->desc );
  }

  mudlog( LOG_INFO , "系統成功\回存玩家資料." );

  /* 儲存幫會資料 */
  mudlog( LOG_INFO, "嘗試儲存幫會資料." );
  save_club();

  /* 整理版面 */
  mudlog( LOG_INFO, "嘗試刪除過量的文章." );
  board_update();

  /* 清除過量的信件 */
  notes_update();

  /* 卸下共享記憶體 */
  delete_share_memory( shmid );

  /* 清除記錄緩衝區 */
  output_buffer();

  RETURN_NULL();
}

/* 檢驗人物是否真的存在 */
bool verify_char( CHAR_DATA * ch )
{
  if ( !ch
    || ch->delete
    || ( ch->desc && verify_desc( ch->desc ) &&
      ch->desc->connected != CON_PLAYING ) )
    return( FALSE );

  return( TRUE );
}

/* 檢驗物品是否真的存在 */
bool verify_obj( OBJ_DATA * obj )
{
  PUSH_FUNCTION( "verify_obj" );

  if ( !obj || obj->delete ) RETURN( FALSE );
  RETURN( TRUE );
}

/* 檢驗描述表是否真的存在 */
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
    act( "$t目前沒有使用量統計功\能。", ch, mud_name, NULL, TO_CHAR );
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

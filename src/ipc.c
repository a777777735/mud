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
#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "merc.h"

#define  PERM           0600       /* 共享記憶體的讀取寫入屬性     */

int      merc_ipc;                 /* 要求的配置共享記憶體的 KEY   */
int      merc_ipc_2;               /* 備用共享記憶體的 KEY         */
int      shmid     = -1;           /* 系統配置共享記憶體的編號     */
char   * shm_ptr   = ( char * ) 0; /* 系統分配共享記憶體的位址     */
int      ipc_block = IPC_BLOCK;    /* 可以有幾塊記憶體區塊         */
int      ipc_idle  = IPC_IDLE;     /* 閒置多久系統時間則刪除此區塊 */

MEMORY_DATA * get_share_memory_address  args( ( int ) );

/* 啟始化共享記憶體 */
void init_share_memory( int key, int key2 )
{
  int           size;
  int           count;
  MEMORY_DATA * mem;

  PUSH_FUNCTION( "init_share_memory" );

  /* 計算出所需的共享記憶體的大小 */
  size = sizeof( MEMORY_DATA ) * ipc_block;

  /* 測試是否之前的區塊沒有被刪除, 有的話嘗試刪除看看 */
  if ( ( shmid = shmget( key , 0 , PERM ) != -1 ) )
  {
    mudlog( LOG_INFO, "shmget: IPC KEY %d 存在, 嘗試刪除.", key );
    delete_share_memory( shmget( key, 0, IPC_CREAT ) );
  }

  /* 建立共享記憶體區塊 */
  if ( ( shmid = shmget( key, size, IPC_EXCL|IPC_CREAT|PERM ) ) == -1 )
  {
    mudlog( LOG_INFO, "shmget: %s" , strerror( errno ) );

    /* 嘗試以第二個共享記憶體建立 */
    /*
    if ( ( shmid = shmget( key2, size, IPC_EXCL|IPC_CREAT|PERM ) ) == -1 )
      mudlog( LOG_INFO, "shmget: %s", strerror( errno ) );
    */
    mudlog( LOG_ERR,  "shmget: 無法取得 %d 的共享計憶體.", key );
  }

  mudlog( LOG_INFO, "shmget: 建立 IPC KEY %d 大小 %d 位元組.", shmid, size );

  /* 掛上共享記憶體, 系統會分配一個位址 */
  if ( ( shm_ptr = shmat( shmid, ( char * ) 0, 0 ) ) == ( char * ) -1 )
  {
    mudlog( LOG_INFO, "shmat: %s" , strerror( errno ) );
    mudlog( LOG_ERR,  "無法掛上共享計憶體." );
  }

  mudlog( LOG_INFO, "shmat: 掛上共享計憶體位址 %p.", shm_ptr );

  /* 清除共享計憶體, 且設定使用次數為 0 */
  for ( count = 0; count < ipc_block; count++ )
  {
    mem = get_share_memory_address( count );
    clean_share_memory_address( count );
    mem->used = 0;
  }

  RETURN_NULL();
}

/* 刪除共享記憶體 */
void delete_share_memory( int aShmid )
{
  /* 先卸下共享記憶體區塊 */
  if ( shmdt( shm_ptr ) == 0 )
    mudlog( LOG_INFO, "shmdt: 成功\卸下共享計憶體." );
  else
    mudlog( LOG_INFO, "shmdt: 卸下共享記憶體失敗." );


  /* 刪除共享記憶體 */
  if ( shmctl( aShmid, IPC_RMID, 0 ) == -1 )
  {
    mudlog( LOG_INFO, "shmctl: %s", strerror( errno ) );
    mudlog( LOG_ERR,  "shmctl: 無法刪除 IPC KEY %d.", aShmid );
  }
  else
  {
    mudlog( LOG_INFO, "shmctl: 成功\刪除 IPC KEY %d.", aShmid );
  }

  /* 標記已經清除, 卸下 */
  shm_ptr = ( char * ) 0;
  return;
}

/* 取得某編號的共享記憶體的位址 */
MEMORY_DATA * get_share_memory_address( int slot )
{
  PUSH_FUNCTION( "get_share_memory_address" );

  /* 判定是否共享記憶體是否已經卸下 */
  if ( shm_ptr == ( char * ) 0 )
    mudlog( LOG_ERR, "共享計憶體已經卸下, 還進行讀取." );

  /* 測試是否編號錯誤 */
  if ( slot < 0 || slot >= ipc_block )
    mudlog( LOG_CRIT, "get_share_memory_address: 編號錯誤" );

  /* 計算位址並且傳回 */
  RETURN( ( MEMORY_DATA * ) ( shm_ptr + ( slot * sizeof( MEMORY_DATA ) ) ) );
}

/* 取得閒置的共享記憶體區塊 */
int get_free_share_memory( DESCRIPTOR_DATA * man, int type )
{
  int           count;
  MEMORY_DATA * mem;

  PUSH_FUNCTION( "get_free_share_memory" );

  for ( count = 0; count < ipc_block; count++ )
  {
    mem = get_share_memory_address( count );

    /* 察看是否被鎖定 */
    if ( mem->lock == FALSE )
    {
      /* 清除並且設定數值 */
      clean_share_memory_address( count );
      mem->lock = TRUE;               /* 設定被鎖定   */
      mem->type = type;               /* 設定區塊型態 */
      mem->id   = man->id_number;     /* 設定玩家號碼 */
      mem->used++;                    /* 累加使用次數 */
      break;
    }
  }

  if ( count == ipc_block )
  {
    mudlog( LOG_DEBUG, "get_free_share_memory: 沒有空閒的區塊" );
    RETURN( -1 );
  }

  RETURN( count );
}

/* 監控是否有閒置太久的共享記憶體 */
void inc_share_memory_timer( void )
{
  int           count;
  MEMORY_DATA * mem;

  PUSH_FUNCTION( "inc_share_memory_timer" );

  for ( count = 0; count < ipc_block; count++ )
  {
    mem = get_share_memory_address( count );
    if ( mem->lock == TRUE && ++mem->timer > ipc_idle )
      clean_share_memory_address( count );
  }

  RETURN_NULL();
}

/* 刪除已經斷線的共享記憶體 */
void update_share_memory( DESCRIPTOR_DATA * man )
{
  int           count;
  MEMORY_DATA * mem;

  PUSH_FUNCTION( "update_share_memory" );

  if ( shm_ptr != ( char * ) 0 )
  {
    for ( count = 0; count < ipc_block; count++ )
    {
      mem = get_share_memory_address( count );
      if ( mem->id == man->id_number ) clean_share_memory_address( count );
    }
  }

  RETURN_NULL();
}

/* 清除某編號的共享記憶體區塊 */
void clean_share_memory_address( int slot )
{
  MEMORY_DATA * mem;

  PUSH_FUNCTION( "clean_share_memory_address" );
  mem = get_share_memory_address( slot );

  mem->done    = FALSE;  /* 區塊是否工作完成     */
  mem->lock    = FALSE;  /* 區塊是否被鎖定       */
  mem->timer   = 0;      /* 鎖定後的計數器       */
  mem->id      = -1;     /* 屬於哪一位玩家的區塊 */
  mem->type    = -1;     /* 區塊的型態           */
  mem->text[0] = '\x0';  /* 區塊的資料區         */

  RETURN_NULL();
}

/* 把字串設定給某編號的共享記憶體區塊 */
void set_share_memory_text( int slot, char * buffer )
{
  MEMORY_DATA * mem;

  PUSH_FUNCTION( "set_share_memory_text" );

  mem = get_share_memory_address( slot );

  /* 察看是否尚未被鎖定就設定資料 */
  if ( mem->lock == FALSE )
  {
    mudlog( LOG_INFO, "編號 %d 的共享計憶體尚未鎖定就設定資料.", slot );
    clean_share_memory_address( slot );
  }

  else
  {
    /* 先檢查資料來源是否會超出範圍 */
    if ( str_len( buffer ) < MAX_STRING_LENGTH - 1 )
      str_cpy( mem->text, buffer );

    else
      str_cpy( mem->text, "訊息太長。" );

    /* 設定區塊工作完成, 等待讀取 */
    mem->done = TRUE;
  }

  RETURN_NULL();
}

/* 察看共享記憶體的使用情況 */
FUNCTION( do_ipcs )
{
  int           count;
  MEMORY_DATA * mem;

  PUSH_FUNCTION( "do_ipcs" );

  for ( clear_buffer(), count = 0; count < ipc_block; count++ )
  {
    mem = get_share_memory_address( count );
    send_to_buffer("編號﹕%3d 號碼﹕%5d 型態﹕%2d 鎖定[%s] "
                   "完成[%s] 時間﹕%5d 次數﹕%5d\n\r"
      , count
      , mem->id, mem->type
      , YESNO( mem->lock )
      , YESNO( mem->done )
      , mem->timer , mem->used );
  }

  send_to_buffer( "\n\r共有 %3d 個共享計憶體區塊﹐共 %d 位元組﹐"
                  "shmid = %d KEY = %d KEY 2 = %d\n\r"
    , ipc_block, ipc_block * sizeof( MEMORY_DATA ), shmid
    , merc_ipc, merc_ipc_2 );

  print_buffer( ch );
  RETURN_NULL();
}

/* 讀取並處理已完成的共享記憶體區塊核心 */
void handle_share_memory( void )
{
  int               count;
  char            * buffer;
  char              buf1[MAX_STRING_LENGTH];
  char              buf2[MAX_STRING_LENGTH];
  MEMORY_DATA     * mem;
  DESCRIPTOR_DATA * man;

  PUSH_FUNCTION( "handle_share_memory" );

  /* 察看哪些是已經完成設定的, 沒有則跳過去 */
  for ( count = 0; count < ipc_block; count++ )
  {
    mem = get_share_memory_address( count );
    if ( mem->done != TRUE ) continue;

    /* 察看到底是給哪一位玩家的資料 */
    for ( man = descriptor_list; man; man = man->next )
    {
      if ( !verify_desc( man ) ) continue;
      if ( man->id_number == mem->id ) break;
    }

    /* 找不到目的的玩家 */
    if ( !man )
    {
      mudlog( LOG_INFO, "有訊息卻找不到玩家, 訊息被忽略." );
      clean_share_memory_address( count );
      continue;
    }

    buffer = mem->text;

    /* 到底是哪一種型態的訊息區塊 */
    switch( mem->type )
    {
    default:
      mudlog( LOG_INFO, "訊息型態錯誤 %d.", mem->type );
      break;

    /* 取得 FQDN 以及遠端使用者的簽入帳號 */
    case INTERNAL_GETHOSTBYADDR:

      buffer = one_argument( buffer , buf1 );
               one_argument( buffer , buf2 );

      if ( buf1[0] == '\x0' || !str_cmp( man->host, buf1 ) )
      {
        write_to_descriptor( man,
          "\n\r\e[1;33m經查詢後﹐發現你的機器沒有FQDN﹐請向你的網路"
          "管理員申請﹐謝謝合作。\e[0m", 0 );

        if ( fqdn_limit )
        {
          man->fqdn_limit = TRUE;

          write_to_descriptor( man,
            "\n\r\e[1;32m為維護網路秩序﹐在你玩此遊戲之前﹐先申請好 "
            "FQDN﹐謝謝合作。\e[0m", 0 );
        }
      }

      /* 位址名稱 */
      if ( !str_cmp( buf1 , FromUnknown ) )
      {
        set_address_data( man->host, man->host );
      }

      else
      {
        set_address_data( man->host , buf1 );
        free_string( man->host );
        man->host = str_dup( buf1 );
      }

      /* 遠端簽入者帳號 */
      if ( str_cmp( buf2 , FromUnknown ) )
      {
        free_string( man->remote );
        man->remote = str_dup( buf2 );
      }

      man->check_fqdn = TRUE;
      break;

    /* 查詢遠端使用者的資料 */
    case INTERNAL_FINGER:

      /* 檢查是否超出範圍, 否則則拷貝到資料區中 */
      buffer[ sizeof( man->finger_data ) - 1 ] = '\x0';
      str_cpy( man->finger_data, buffer );
      man->check_finger = TRUE;
      break;

    /* 檢查玩家檔案是否正確 */
    case INTERNAL_CHECKFILE:
      man->check_file = TRUE;
      man->file       = atoi( buffer );
      break;

    case INTERNAL_CHECKSERV:
      if ( buffer[0] == '1' ) man->server = TRUE;
      else                    man->server = FALSE;
      break;
    }

    /* 清除已讀取完畢的共享記憶體區塊 */
    clean_share_memory_address( count );
  }
  RETURN_NULL();
}

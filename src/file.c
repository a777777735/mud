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
#include <sys/stat.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <glob.h>
#include "file.h"
#include "merc.h"

AREA_DATA * load_zone   args( ( char * ) );
void check_capital      args( ( AREA_DATA * ) );
int  index_boot         args( ( char * , int , AREA_DATA * ) );
char LastFileName[MAX_FILE_LENGTH] = { '\x0' };

extern int      glob_file       args( ( const char *, const char *, glob_t * ) );

FILE * FOPEN( const char * path , const char * mode )
{
  FILE * pFile;

  PUSH_FUNCTION( "FOPEN" );

  if ( RecordFile ) mudlog( LOG_INFO, "開啟檔案﹕%s", path );

  if ( fpReserve ) fclose( fpReserve );

  if ( !( pFile = fopen( path , mode ) ) )
  {
    mudlog( LOG_INFO , "無法開啟檔案 %s %s", path, strerror( errno ) );
    fpReserve = fopen( null_file , "r" );
    RETURN( NULL );
  }

  fpReserve = NULL;
  RETURN( pFile );
}

void FCLOSE( FILE * stream )
{
  PUSH_FUNCTION( "FCLOSE" );

  fclose( stream );
  if ( !fpReserve ) fpReserve = fopen( null_file , "r" );

  RETURN_NULL();
}

/* 開啟所有區域的主要程式部份 */
void open_area_directory( void )
{
  char        directory    [ MAX_FILE_LENGTH ];
  char        buf          [ MAX_FILE_LENGTH ];
  char        filename     [ MAX_FILE_LENGTH ];
  char        work_dir_name[ MAX_FILE_LENGTH ];
  char        index        [ MAX_FILE_LENGTH ];
  char      * word;
  FILE_DATA * pFile;
  AREA_DATA * pArea;

  PUSH_FUNCTION( "open_area_directory" );

  /* 先找到目錄並且察看是否要填入目錄的記號 */
  fill_path( str_cpy( directory, area_dir ) );

  /* 找到區域檔案目錄列表檔 */
  sprintf( filename , "%s%s" , directory , dir_file );

  /* 看看是否可以開檔 */
  if ( ( pFile = f_open( filename , "r" ) ) )
  {
    /* 一直讀到檔案末端才停止 */
    while ( !if_eof( pFile ) )
    {
      /* 讀入想要讀取的區域目錄 */
      word = fread_word( pFile );
      if ( word[0] == '*' )
      {
        fread_to_eol( pFile );
        continue;
      }

      sprintf( buf , "%s%s" , directory , word );

      /* 檢查是否為可以存取的子目錄 */
      if ( is_directory( buf ) )
      {
        /* 再察看是否有索引檔 */
        sprintf( work_dir_name , "%s%s" , directory , word );
        fill_path( work_dir_name );
        sprintf( index , "%s%s" , work_dir_name , index_file );

        /* 如果有索引檔則讀入所有資料 */
        if ( is_regular( index ) )
        {
          pArea = load_zone( work_dir_name );
          if ( !DefaultArea ) DefaultArea = pArea;
        }

        /* 不然送出錯誤的訊息 */
        else
        {
          mudlog( LOG_ERR , "區域目錄 %s 沒有索引檔" , work_dir_name );
        }
      }

      /* 區域目錄根本不能存取 */
      else
      {
        mudlog( LOG_ERR , "區域目錄 %s 無法存取" , buf );
      }
    }
    f_close( pFile );
  }

  /* 根本沒有區域目錄列表檔, 送出錯誤訊息 */
  else
  {
    mudlog( LOG_ERR , "沒有區域子目錄列表檔" );
  }

  if ( !DefaultArea ) mudlog( LOG_ERR , "沒有預設區域." );

  /* 讀入玩家寫的區域 */
  if ( PlayerArea[0] )
  {
    /* 檢查是否為可以存取的子目錄 */
    if ( is_directory( PlayerArea ) )
    {
      /* 再察看是否有索引檔 */
      str_cpy( work_dir_name, PlayerArea );
      fill_path( work_dir_name );
      sprintf( index , "%s%s" , work_dir_name , index_file );

      /* 如果有索引檔則讀入所有資料 */
      if ( is_regular( index ) )
      {
        PlayerZone = load_zone( work_dir_name );
      }

      /* 不然送出錯誤的訊息 */
      else
      {
        mudlog( LOG_ERR , "玩家區域目錄 %s 沒有索引檔" , PlayerArea );
      }
    }

    /* 區域目錄根本不能存取 */
    else
    {
      mudlog( LOG_ERR , "玩家區域目錄 %s 無法存取" , PlayerArea );
    }
  }

  RETURN_NULL();
}

/* 看看是否有必要填入目錄結尾符號 '/' */
void fill_path( char * pathname )
{
  PUSH_FUNCTION( "fill_path" );

  /* 目錄名稱長度為零, 一定錯誤 */
  if ( !pathname || !*pathname ) mudlog( LOG_ERR , "載入目錄有錯誤" );

  /* 看看是否有必要填入目錄結尾符號 '/' */
  if ( pathname[ str_len( pathname ) - 1 ] != '/' ) str_cat( pathname , "/" );

  RETURN_NULL();
}

/* 查詢是否真的有這個檔案 */
bool is_regular( const char * filename )
{
  struct stat st;

  PUSH_FUNCTION( "is_regular" );

  if ( stat( filename, &st ) == 0 && S_ISREG( st.st_mode ) )
    RETURN( TRUE );

  RETURN( FALSE );
}

/* 查詢是否真的有這個目錄 */
bool is_directory( const char * path )
{
  struct stat st;

  PUSH_FUNCTION( "is_directory" );

  if ( stat( path, &st ) == 0 && S_ISDIR( st.st_mode ) ) RETURN( TRUE );
  RETURN( FALSE );
}

/* 載入整個目錄所有的資料 */
AREA_DATA * load_zone( char * pathname )
{
  AREA_DATA * pArea;
  char        index_file_name[ MAX_FILE_LENGTH ];

  PUSH_FUNCTION( "load_zone" );

  /* 先設定索引檔的檔名 */
  sprintf( index_file_name , "%s%s" , pathname , index_file );

  /* 載入索引檔案 */
  if ( !( pArea = load_area( index_file_name ) ) )
    mudlog( LOG_ERR , "載入目錄 %s 索引檔有問題" , pathname );

  if ( !area_first ) area_first      = pArea;
  if (  area_last  ) area_last->next = pArea;

  area_last   = pArea;
  pArea->next = NULL;
  top_area++;

  /* 載入房間 */
  pArea->room += index_boot( pathname , ROOM_MODE, pArea );

  /* 載入怪物 */
  pArea->mob += index_boot( pathname , MOB_MODE, pArea );

  /* 載入物品 */
  pArea->obj += index_boot( pathname , OBJ_MODE, pArea );

  /* 載入商店 */
  pArea->shop += index_boot( pathname , SHOP_MODE, pArea );

  /* 載入重置 */
  pArea->reset += index_boot( pathname , RESET_MODE, pArea );

  /* 載入礦物 */
  pArea->reset += index_boot( pathname , MINERAL_MODE, pArea );

  /* 檢查首都 */
  check_capital( pArea );
  RETURN( pArea );
}

/* 載入各種檔案格式的主程式 */
int index_boot( char * path , int mode , AREA_DATA * pArea )
{
  MOB_INDEX_DATA  * pMobIndex;
  OBJ_INDEX_DATA  * pObjIndex;
  ROOM_INDEX_DATA * pRoomIndex;
  SHOP_DATA       * pShop;
  MINERAL_DATA    * pMineral;
  glob_t            result;
  char              dir_name[ MAX_FILE_LENGTH ];
  char              filename[ MAX_FILE_LENGTH ];
  char              ext[ MAX_FILE_LENGTH];
  int               count;
  int               loop;
  int               rc;
  int               iHash;

  PUSH_FUNCTION( "index_boot" );

  /* 先看看需要哪一種的目錄名稱 */
  switch( mode )
  {
  /* 錯誤的模式 */
  default:
    mudlog( LOG_ERR , "Index_boot: 未定義的項目 %d." , mode );

  /* 載入房間 */
  case ROOM_MODE:
    sprintf( dir_name , "%s%s" , path , room_dir );
    strcpy( ext, room_ext );
    break;

  /* 載入怪物 */
  case MOB_MODE:
    sprintf( dir_name , "%s%s" , path , mob_dir );
    strcpy( ext, mob_ext );
    break;

  /* 載入物品 */
  case OBJ_MODE:
    sprintf( dir_name , "%s%s" , path , obj_dir );
    strcpy( ext, obj_ext );
    break;

  /* 載入商店 */
  case SHOP_MODE:
    sprintf( dir_name , "%s%s" , path , shop_dir );
    strcpy( ext, shop_ext );
    break;

  /* 載入重置 */
  case RESET_MODE:
    sprintf( dir_name , "%s%s" , path , reset_dir );
    strcpy( ext, reset_ext );
    break;

  /* 載入礦物 */
  case MINERAL_MODE:
    sprintf( dir_name , "%s%s" , path , mineral_dir );
    strcpy( ext, mineral_ext );
    break;
  }

  fill_path( dir_name );
  count = 0;

  if ( ( rc = glob_file( dir_name, ext, &result ) ) >= 0 )
  {
    for ( loop = 0; loop < rc; loop++ )
    {
      sprintf( filename, "%s%s", dir_name, result.gl_pathv[loop] );
      if ( !is_regular( filename ) ) continue;

      /* 再看看試試哪一種來決定載入的程式 */
      switch( mode )
      {
      /* 錯誤的模式 */
      default :
        mudlog( LOG_ERR , "Index_boot: 未定義的項目" );

      /* 載入房間 */
      case ROOM_MODE:

        /* 載入資料 */
        pRoomIndex = load_room( filename );

        pRoomIndex->area       = pArea;
        pRoomIndex->filename   = str_dup( filename );
        iHash                  = pRoomIndex->vnum % MAX_KEY_HASH;
        pRoomIndex->next       = room_index_hash[iHash];
        room_index_hash[iHash] = pRoomIndex;
        top_room++;
        count++;

        if ( pArea->list )
        {
          pRoomIndex->next_in_area = pArea->list;
          pArea->list              = pRoomIndex;
        }
        else
        {
          pArea->list = pRoomIndex;
        }

        break;

      /* 載入怪物 */
      case MOB_MODE:

        /* 開啟檔案並且載入資料 */
        if ( !( pMobIndex = load_mobiles( filename ) ) )
          mudlog( LOG_DEBUG, "index_boot: 載入怪物失敗." );

        if ( get_mob_index( pMobIndex->vnum ) )
          mudlog( LOG_DEBUG, "index_boot: 載入重複怪物 %s 號碼 %d."
            , filename, pMobIndex->vnum );

        pMobIndex->area       = pArea;
        iHash                 = pMobIndex->vnum % MAX_KEY_HASH;
        pMobIndex->next       = mob_index_hash[iHash];
        mob_index_hash[iHash] = pMobIndex;
        top_mob_index++;
        kill_table[URANGE(0, pMobIndex->level, MAX_LEVEL-1)].number++;

        count++;
        break;

      /* 載入物品 */
      case OBJ_MODE:

        if ( !( pObjIndex = load_object( filename ) ) )
          mudlog( LOG_DEBUG, "index_boot: 載入物品失敗." );

        if ( get_obj_index( pObjIndex->vnum ) )
          mudlog( LOG_DEBUG, "index_boot: 載入重複物品 %s 號碼 %d."
            , filename, pObjIndex->vnum );

        iHash                 = pObjIndex->vnum % MAX_KEY_HASH;
        pObjIndex->next       = obj_index_hash[iHash];
        obj_index_hash[iHash] = pObjIndex;
        top_obj_index++;

        count++;
        break;

      /* 載入商店 */
      case SHOP_MODE:

        /* 開啟檔案並且載入資料 */
        pShop = load_shop( filename );

        pShop->filename  = str_dup( filename );

        if ( !shop_first ) shop_first      = pShop;
        if (  shop_last  ) shop_last->next = pShop;

        shop_last   = pShop;
        pShop->next = NULL;
        top_shop++;
        count++;
        break;

      /* 載入礦物 */
      case MINERAL_MODE:

        /* 開啟檔案並且載入資料 */
        pMineral = load_mineral( filename );
        pMineral->next = pArea->mineral;
        pArea->mineral = pMineral;
        top_mineral++;
        count++;
        break;

      /* 載入重置 */
      case RESET_MODE:

        /* 開啟檔案並且載入資料 */
        load_resets( filename , pArea );
        count++;
        break;
      }
    }
    globfree( &result );
  }

  RETURN( count );
}

void check_capital( AREA_DATA * pArea )
{
  ROOM_INDEX_DATA * pRoom;
  int               capital;

  PUSH_FUNCTION( "check_capital" );

  if ( !pArea ) mudlog( LOG_ERR, "check_capital: 來源不正確." );

  if ( ( capital = pArea->capital_no ) <= 0 ) RETURN_NULL();

  for ( pRoom = pArea->list; pRoom; pRoom = pRoom->next_in_area )
  {
    if ( pRoom->vnum == capital )
    {
      pArea->capital = pRoom;
      RETURN_NULL();
    }
  }
  mudlog( LOG_ERR, "區域 %s 的首都 %d 不存在.", pArea->name, capital );
  RETURN_NULL();
}

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
#include "merc.h"

/* 表示記憶體區塊的魔術數字, 可以改變 */
#define MAGIC_NUMBER    'J'
#define KILO_BYTE       1024

/* 配置記憶體的大小 */
int rgSizeList [MAX_MEM_LIST]  =
{
  16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768
};

struct magic_block
{
  char magic_number;
  char type;
};

/* 一切配置的結構資料 */
struct  struct_data struct_block [] =
{
  { STRUCT_OBJ_DATA        ,sizeof( OBJ_DATA        ), "物品"    , 0, 0 },
  { STRUCT_CHAR_DATA       ,sizeof( CHAR_DATA       ), "人物"    , 0, 0 },
  { STRUCT_AFFECT_DATA     ,sizeof( AFFECT_DATA     ), "影響"    , 0, 0 },
  { STRUCT_FRIEND_DATA     ,sizeof( FRIEND_DATA     ), "好友名單", 0, 0 },
  { STRUCT_BASIC_DATA      ,sizeof( BASIC_DATA      ), "基本資料", 0, 0 },
  { STRUCT_ENABLE_DATA     ,sizeof( ENABLE_DATA     ), "致能"    , 0, 0 },
  { STRUCT_FAILCODE_DATA   ,sizeof( FAILCODE_DATA   ), "錯誤密碼", 0, 0 },
  { STRUCT_QUEST_DATA      ,sizeof( QUEST_DATA      ), "解謎"    , 0, 0 },
  { STRUCT_DATABASE_DATA   ,sizeof( DATABASE_DATA   ), "資料庫"  , 0, 0 },
  { STRUCT_FILE_DATA       ,sizeof( FILE_DATA       ), "檔案"    , 0, 0 },
  { STRUCT_EXIT_DATA       ,sizeof( EXIT_DATA       ), "出口"    , 0, 0 },
  { STRUCT_ALIAS_DATA      ,sizeof( ALIAS_DATA      ), "巨集"    , 0, 0 },
  { STRUCT_DESCRIPTOR_DATA ,sizeof( DESCRIPTOR_DATA ), "描述子"  , 0, 0 },
  { STRUCT_RESET_DATA      ,sizeof( RESET_DATA      ), "重置"    , 0, 0 },
  { STRUCT_ROOM_INDEX_DATA ,sizeof( ROOM_INDEX_DATA ), "房間原型", 0, 0 },
  { STRUCT_PC_DATA         ,sizeof( PC_DATA         ), "玩家特別", 0, 0 },
  { STRUCT_ENROLL_DATA     ,sizeof( ENROLL_DATA     ), "記仇"    , 0, 0 },
  { STRUCT_MPROG_DATA      ,sizeof( MPROG_DATA      ), "怪物程式", 0, 0 },
  { STRUCT_MPROG_ACT_LIST  ,sizeof( MPROG_ACT_LIST  ), "怪物動作", 0, 0 },
  { STRUCT_ENEMY_DATA      ,sizeof( ENEMY_DATA      ), "公敵"    , 0, 0 },
  { STRUCT_MOB_INDEX_DATA  ,sizeof( MOB_INDEX_DATA  ), "怪物原型", 0, 0 },
  { STRUCT_OBJ_INDEX_DATA  ,sizeof( OBJ_INDEX_DATA  ), "物品原型", 0, 0 },
  { STRUCT_LIMIT_DATA      ,sizeof( LIMIT_DATA      ), "限制資料", 0, 0 },
  { STRUCT_SYMBOL_DATA     ,sizeof( SYMBOL_DATA     ), "符號"    , 0, 0 },
  { STRUCT_NOTE_DATA       ,sizeof( NOTE_DATA       ), "信件"    , 0, 0 },
  { STRUCT_BAN_DATA        ,sizeof( BAN_DATA        ), "位址禁止", 0, 0 },
  { STRUCT_EXTRA_DESCR_DATA,sizeof( EXTRA_DESCR_DATA), "額外描述", 0, 0 },
  { STRUCT_CMD_DATA        ,sizeof( CMD_DATA        ), "命令"    , 0, 0 },
  { STRUCT_AREA_DATA       ,sizeof( AREA_DATA       ), "區域"    , 0, 0 },
  { STRUCT_HELP_DATA       ,sizeof( HELP_DATA       ), "求助"    , 0, 0 },
  { STRUCT_SHOP_DATA       ,sizeof( SHOP_DATA       ), "商店"    , 0, 0 },
  { STRUCT_SYMBOL_DATA     ,sizeof( SYMBOL_DATA     ), "符號"    , 0, 0 },
  { STRUCT_SKILL_DATA      ,sizeof( SKILL_DATA      ), "技能"    , 0, 0 },
  { STRUCT_SOCIAL_DATA     ,sizeof( SOCIAL_DATA     ), "社交"    , 0, 0 },
  { STRUCT_GAMBLE_DATA     ,sizeof( GAMBLE_DATA     ), "賭博"    , 0, 0 },
  { STRUCT_TEACH_DATA      ,sizeof( TEACH_DATA      ), "教導"    , 0, 0 },
  { STRUCT_XNAME_DATA      ,sizeof( XNAMES_DATA     ), "不雅字"  , 0, 0 },
  { STRUCT_SECTOR_DATA     ,sizeof( SECTOR_DATA     ), "地形資料", 0, 0 },
  { STRUCT_CLASS_DATA      ,sizeof( CLASS_DATA      ), "職業資料", 0, 0 },
  { STRUCT_ADDRESS_DATA    ,sizeof( ADDRESS_DATA    ), "位址資料", 0, 0 },
  { STRUCT_BUS_DATA        ,sizeof( BUS_DATA        ), "地鐵資料", 0, 0 },
  { STRUCT_NET_DATA        ,sizeof( NET_DATA        ), "網際連線", 0, 0 },
  { STRUCT_CLUB_DATA       ,sizeof( CLUB_DATA       ), "幫派資料", 0, 0 },
  { STRUCT_DAMAGE_DATA     ,sizeof( DAMAGE_DATA     ), "攻傷資料", 0, 0 },
  { STRUCT_PW_DATA         ,sizeof( PW_DATA         ), "使用者"  , 0, 0 },
  { STRUCT_GR_DATA         ,sizeof( GR_DATA         ), "族群"    , 0, 0 },
  { STRUCT_CACHET_DATA     ,sizeof( CACHET_DATA     ), "封印魔石", 0, 0 },
  { STRUCT_LIQ_DATA        ,sizeof( LIQ_DATA        ), "液體"    , 0, 0 },
  { STRUCT_RESTRICT_DATA   ,sizeof( RESTRICT_DATA   ), "裝備限制", 0, 0 },
  { STRUCT_JOB_DATA        ,sizeof( JOB_DATA        ), "特殊函數", 0, 0 },
  { STRUCT_QUEST_INFO      ,sizeof( QUEST_INFO      ), "解迷資料", 0, 0 },
  { STRUCT_IMMLIST_DATA    ,sizeof( IMMLIST_DATA    ), "神族資料", 0, 0 },
  { STRUCT_SHIP_DATA       ,sizeof( SHIP_DATA       ), "渡船名單", 0, 0 },
  { STRUCT_BOARD_DATA      ,sizeof( BOARD_DATA      ), "版面"    , 0, 0 },
  { STRUCT_POST_DATA       ,sizeof( POST_DATA       ), "文章"    , 0, 0 },
  { STRUCT_HERO_DATA       ,sizeof( HERO_DATA       ), "英雄"    , 0, 0 },
  { STRUCT_EFFECT_DATA     ,sizeof( EFFECT_DATA     ), "效應"    , 0, 0 },
  { STRUCT_MESSAGE_DATA    ,sizeof( MESSAGE_DATA    ), "敘述"    , 0, 0 },
  { STRUCT_ANGEL_DATA      ,sizeof( ANGEL_DATA      ), "守護神"  , 0, 0 },
  { STRUCT_SALE_DATA       ,sizeof( SALE_DATA       ), "拍賣品"  , 0, 0 },
  { STRUCT_FIGHT_DATA      ,sizeof( FIGHT_DATA      ), "ＰＫ"    , 0, 0 },
  { STRUCT_SERIAL_DATA     ,sizeof( SERIAL_DATA     ), "序號"    , 0, 0 },
  { STRUCT_TOP_DATA        ,sizeof( TOP_DATA        ), "統計"    , 0, 0 },
  { STRUCT_WANTED_DATA     ,sizeof( WANTED_DATA     ), "通緝"    , 0, 0 },
  { STRUCT_VOTE_DATA       ,sizeof( VOTE_DATA       ), "投票"    , 0, 0 },
  { STRUCT_IP_DATA         ,sizeof( IP_DATA         ), "連線"    , 0, 0 },
  { STRUCT_JOKE_DATA       ,sizeof( JOKE_DATA       ), "笑話"    , 0, 0 },
  { STRUCT_TRACE_DATA      ,sizeof( TRACE_DATA      ), "地圖"    , 0, 0 },
  { STRUCT_VARIABLE_DATA   ,sizeof( VARIABLE_DATA   ), "變數"    , 0, 0 },
  { STRUCT_TICKET_DATA     ,sizeof( TICKET_DATA     ), "彩券"    , 0, 0 },
  { STRUCT_ORDER_DATA      ,sizeof( ORDER_DATA      ), "中獎"    , 0, 0 },
  { STRUCT_GIFT_DATA       ,sizeof( GIFT_DATA       ), "禮物"    , 0, 0 },
  { STRUCT_STAMP_DATA      ,sizeof( STAMP_DATA      ), "標記"    , 0, 0 },
  { STRUCT_MINERAL_DATA    ,sizeof( MINERAL_DATA    ), "礦產"    , 0, 0 },
  { STRUCT_MINE_DATA       ,sizeof( MINE_DATA       ), "礦產"    , 0, 0 },
  { STRUCT_EVENT_DATA      ,sizeof( EVENT_DATA      ), "事件"    , 0, 0 },
  { STRUCT_BOUNTY_DATA     ,sizeof( BOUNTY_DATA     ), "懸賞資料", 0, 0 },
  { STRUCT_SERVER_DATA     ,sizeof( SERVER_DATA     ), "工作站"  , 0, 0 },
  { STRUCT_CACHET_DATA     ,sizeof( CACHET_DATA     ), "封印魔石", 0, 0 },
  { STRUCT_QUESTION_DATA   ,sizeof( QUESTION_DATA   ), "問題集"  , 0, 0 },
  { STRUCT_ANSWER_DATA     ,sizeof( ANSWER_DATA     ), "答案"    , 0, 0 },
  { STRUCT_ENQUIRE_DATA    ,sizeof( ENQUIRE_DATA    ), "問路"    , 0, 0 },
  { STRUCT_GREETING_DATA   ,sizeof( GREETING_DATA   ), "進站"    , 0, 0 },
  { STRUCT_SITUS_DATA      ,sizeof( SITUS_DATA      ), "部位"    , 0, 0 },
  { ERRORCODE              , -1                      , NULL     , -1, 0 }
};

/* 宣告區域函數 */
void * alloc_memory     args( ( int , bool ) );
void   free_mem         args( ( void * ) );
int    mem_block        args( ( int ) );

/* 區域變數 */
void *  rgFreeList [MAX_MEM_LIST];
int     rgAllocList[MAX_MEM_LIST];
int     nAllocPerm      = 0;
int     sAllocPerm      = 0;
int     nAllocPermBlock = 0;
int     iMemPerm        = 0;
char  * pMemPerm;

/* 起始化記憶體區塊 */
void memory_ini( void )
{
  int iList;

  PUSH_FUNCTION( "memory_ini" );

  for ( iList = 0; iList < MAX_MEM_LIST; iList++ )
  {
    /* Round off 以後要配置記憶體的大小 */
    while ( rgSizeList[iList] % sizeof( long ) ) rgSizeList[iList]++;

    /* 如果配置的大小比一整塊的區塊還大, 這是不行的 */
    if ( rgSizeList[iList] > MAX_PERM_BLOCK )
    {
      perror( "memory_ini: 記憶體設定錯誤" );
      merc_exit( -1 );
    }

    if ( rgSizeList[iList] <= sizeof( struct magic_block ) )
    {
      perror( "memory_ini: 記憶體設定錯誤" );
      merc_exit( -1 );
    }
  }

  RETURN_NULL();
}

/* 配置記憶體的核心程式部份 */
void * alloc_memory( int sMem , bool record )
{
  struct magic_block * magic;
  void               * pMem;
  int                  iList;
  int                  loop;

  PUSH_FUNCTION( "alloc_memory" );

  if ( ( ( iList = mem_block( sMem + sizeof( struct magic_block ) ) ) == ERRORCODE ) )
    mudlog( LOG_CRIT, "alloc_memory: 配置 %d 位元組記憶體太大" , sMem );

  sMem = rgSizeList[iList];

  /* 假如沒有備用的記憶體 */
  if ( !rgFreeList[iList] )
  {
    if ( !pMemPerm || iMemPerm + sMem > MAX_PERM_BLOCK )
    {
      /* 先看看是否還是切成其他更小的區塊給以後使用 */
      if ( pMemPerm )
      {
        for ( loop = MAX_MEM_LIST - 1; loop >= 0; loop-- )
        {
          /* 切給更小區塊以供以後使用 */
          while ( iMemPerm + rgSizeList[loop] <= MAX_PERM_BLOCK )
          {
            pMem        = pMemPerm + iMemPerm;
            iMemPerm   += sMem;
            sAllocPerm += sMem;
            nAllocPerm++;
            rgAllocList[loop]++;

            * ((void **) pMem) = rgFreeList[loop];
            rgFreeList[loop]   = pMem;
          }
        }
      }

      /* 向系統配置記憶體 */
      if ( !( pMemPerm = calloc( 1, MAX_PERM_BLOCK ) ) )
        mudlog( LOG_CRIT , "alloc_memory: 無法向系統要求記憶體" );

      nAllocPermBlock++;
      iMemPerm = 0;
    }

    pMem        = pMemPerm + iMemPerm;
    iMemPerm   += sMem;
    sAllocPerm += sMem;
    nAllocPerm++;

    if ( record == TRUE ) rgAllocList[iList]++;
  }

  /* 有備用的記憶體, 所以不用真正去切記憶體來使用 */
  else
  {
    pMem              = rgFreeList[iList];
    rgFreeList[iList] = * ( ( void ** ) rgFreeList[iList] );
  }

  /* 清除記憶體 */
  memset( pMem, 0, rgSizeList[iList] );

  /* 設定魔術數字以及記標這個區塊的大小型態 */
  magic = ( struct magic_block * ) pMem;
  magic->magic_number = ( char ) MAGIC_NUMBER;
  magic->type         = ( char ) iList;

  RETURN( pMem + sizeof( struct magic_block ) );
}

char * str_dup( const char * str )
{
  char * target;

  PUSH_FUNCTION( "str_dup" );

  if ( !str || !str[0] ) RETURN( &str_empty[0] );

  target = str_cpy( alloc_string( str_len( str ) + 1 ), str );

  RETURN( target );
}

/* 釋放記憶體, 將之放到被用區塊中 */
void free_mem( void * pMem )
{
  struct magic_block * magic;
  int                  iList;

  PUSH_FUNCTION( "free_mem" );

  pMem  -= sizeof( struct magic_block );
  magic  = ( struct magic_block * ) pMem;

  if ( magic->magic_number != ( char ) MAGIC_NUMBER )
  {
    int  loop;
    char buf[MAX_INPUT_LENGTH];

    for ( loop = 0; loop < rgSizeList[0] - sizeof( struct magic_block ); loop++ )
    {
      if ( loop >= sizeof( buf ) - 1 ) break;
      buf[loop] = *( ( char * ) pMem + loop + sizeof( struct magic_block ) );
    }

    buf[loop] = '\x0';

    mudlog( LOG_DEBUG, "free_mem: 不正常記憶體內容 [%s]", buf );
    mudlog( LOG_EMERG , "free_mem: 釋放的記憶體區塊沒有魔術數字" );
  }

  iList = ( int ) magic->type;

  /* 送入閒置的串列之中 */
  * ( ( void ** ) pMem ) = rgFreeList[iList];
  rgFreeList[iList]      = pMem;

  RETURN_NULL();
}

/* 配置字串型態記憶體 */
void * alloc_string( int sMem )
{
  void * target;

  PUSH_FUNCTION( "alloc_string" );

  target = alloc_memory( sMem, TRUE );
  RETURN( target );
}

/* 釋放字串型態的記憶體 */
void free_string( char * pstr )
{
  PUSH_FUNCTION( "free_string" );

  if ( pstr == NULL || pstr == &str_empty[0] ) RETURN_NULL();
  free_mem( pstr );

  RETURN_NULL();
}

/* 配置結構 */
void * alloc_struct( int type )
{
  CHAR_DATA       * pChar;
  OBJ_DATA        * pObj;
  DESCRIPTOR_DATA * pDesc;
  int               iList;
  int               size;

  PUSH_FUNCTION( "alloc_struct" );

  for ( iList = size = 0; ; iList++ )
  {
    if ( struct_block[iList].type == ERRORCODE )
      mudlog( LOG_ERR , "配置錯誤型態的結構 %d", type );

    if ( struct_block[iList].type == type )
    {
      size = struct_block[iList].size;
      struct_block[iList].ref++;
      break;
    }
  }

  switch( type )
  {
  case STRUCT_CHAR_DATA:

    /* 尋找閒置的結構 */
    if ( FreeChar > 0 )
    {
      for ( pChar = char_list; pChar; pChar = pChar->next )
      {
        if ( pChar->delete && pChar->counter <= 0 )
        {
          CHAR_DATA * pNext;

          /* 釋放上一次的內容 */
          free_string( pChar->name        );
          free_string( pChar->cname       );
          free_string( pChar->byname      );
          free_string( pChar->long_descr  );
          free_string( pChar->description );
          free_string( pChar->prompt      );
          free_string( pChar->email       );
          free_string( pChar->talk_buf    );
          free_string( pChar->editing     );
          free_string( pChar->stack       );

          /* 記錄本來的下一個結構, 並清除此記憶體內容為零 */
          pNext = pChar->next;
          memset( pChar, 0, sizeof( CHAR_DATA ) );

          /* 重新指向, 並設定為未清除 */
          pChar->delete  = FALSE;
          pChar->next    = pNext;

          FreeChar--;
          RETURN( ( void * ) pChar );
        }
      }
    }

    /* 沒有閒置的結構, 必須另行配置 */
    struct_block[iList].nAlloc++;
    pChar = alloc_memory( size, FALSE );

    /* 清除記憶體內容為零 */
    memset( pChar, 0, sizeof( CHAR_DATA ) );

    /* 調整串列的順序, 並且設定為未清除 */
    pChar->next    = char_list;
    char_list      = pChar;
    pChar->delete  = FALSE;

    RETURN( ( void * ) pChar );

  case STRUCT_OBJ_DATA:

    /* 尋找閒置的結構 */

    if ( FreeObject > 0 )
    {
      for ( pObj = object_list; pObj; pObj = pObj->next )
      {
        if ( pObj->delete && pObj->counter <= 0 )
        {
          OBJ_DATA * pNext;

          /* 釋放上一次的內容 */
          free_string( pObj->name        );
          free_string( pObj->cname       );
          free_string( pObj->description );

          /* 記錄本來的下一個結構, 並清除此記憶體內容為零 */
          pNext = pObj->next;
          memset( pObj, 0, sizeof( OBJ_DATA ) );

          /* 重新指向, 並設定為未清除 */
          pObj->delete  = FALSE;
          pObj->next    = pNext;

          FreeObject--;
          RETURN( ( void * ) pObj );
        }
      }
    }

    /* 沒有閒置的結構, 必須另行配置 */
    struct_block[iList].nAlloc++;
    pObj = alloc_memory( size, FALSE );

    /* 清除記憶體內容為零 */
    memset( pObj, 0, sizeof( OBJ_DATA ) );

    /* 調整串列的順序, 並且設定為未清除 */
    pObj->next    = object_list;
    object_list   = pObj;
    pObj->delete  = FALSE;

    RETURN( ( void * ) pObj );

  case STRUCT_DESCRIPTOR_DATA:

    /* 尋找閒置的結構 */
    if ( FreeDesc > 0 )
    {
      for ( pDesc = descriptor_list; pDesc; pDesc = pDesc->next )
      {
        if ( pDesc->delete && pDesc->counter <= 0 )
        {
          DESCRIPTOR_DATA * pNext;

          /* 釋放上一次的內容 */
          free_string( pDesc->address );
          free_string( pDesc->host    );
          free_string( pDesc->remote  );
          free_string( pDesc->outbuf  );
          free_string( pDesc->buffer  );
          free_string( pDesc->path    );

          /* 記錄本來的下一個結構, 並清除此記憶體內容為零 */
          pNext = pDesc->next;
          memset( pDesc, 0, sizeof( DESCRIPTOR_DATA ) );

          /* 重新指向, 並設定為未清除 */
          pDesc->next      = pNext;
          pDesc->delete    = FALSE;
          pDesc->edit_mode = EDIT_NONE;

          FreeDesc--;
          RETURN( ( void * ) pDesc );
        }
      }
    }

    /* 沒有閒置的結構, 必須另行配置 */
    struct_block[iList].nAlloc++;
    pDesc = alloc_memory( size, FALSE );

    /* 清除記憶體內容為零 */
    memset( pDesc, 0, sizeof( DESCRIPTOR_DATA ) );

    /* 調整串列的順序, 並且設定為未清除 */
    pDesc->next     = descriptor_list;
    descriptor_list = pDesc;
    pDesc->delete   = FALSE;

    /* 連線人數加一 */
    MaxConnect++;

    RETURN( ( void * ) pDesc );
  }

  /* 記錄到底配置了幾個 */
  struct_block[iList].nAlloc++;
  RETURN( alloc_memory( size , FALSE ) );
}

/* 釋放結構 */
void free_struct( void * aMem , int type )
{
  int iList;

  PUSH_FUNCTION( "free_struct" );

  if ( !aMem ) mudlog( LOG_ERR , "釋放結構型態 %d 指標是空的" , type );

  for ( iList = 0; ; iList++ )
  {
    if ( struct_block[iList].type == ERRORCODE )
      mudlog( LOG_ERR, "釋放結構型態錯誤 %d" , type );

    /* 看看這種結構能不能被釋放, 否則是錯誤的釋放 */
    if ( struct_block[iList].type == type ) break;
  }

  switch( type )
  {
  case STRUCT_CHAR_DATA:

    if ( ( ( ( CHAR_DATA * ) aMem )->delete ) == TRUE )
    {
      mudlog( LOG_DEBUG, "free_struct: 重複釋放人物結構." );
      RETURN_NULL();
    }

    ( ( CHAR_DATA * ) aMem )->delete  = TRUE;
    ( ( CHAR_DATA * ) aMem )->counter = 1;
    RETURN_NULL();

  case STRUCT_OBJ_DATA:

    if ( ( ( ( OBJ_DATA * ) aMem )->delete ) == TRUE )
    {
      mudlog( LOG_DEBUG, "free_struct: 重複釋放物品結構." );
      RETURN_NULL();
    }

    ( ( OBJ_DATA * ) aMem )->delete  = TRUE;
    ( ( OBJ_DATA * ) aMem )->counter = 1;
    RETURN_NULL();

  case STRUCT_DESCRIPTOR_DATA:

    if ( ( ( ( DESCRIPTOR_DATA * ) aMem )->delete ) == TRUE )
    {
      mudlog( LOG_DEBUG, "free_struct: 重複釋放描述子結構." );
      RETURN_NULL();
    }

    ( ( DESCRIPTOR_DATA * ) aMem )->delete      = TRUE;
    ( ( DESCRIPTOR_DATA * ) aMem )->counter     = 1;
    ( ( DESCRIPTOR_DATA * ) aMem )->username[0] = '\x0';
    RETURN_NULL();
  }

  /* 記錄到底配置了幾個 */
  struct_block[iList].nAlloc--;

  free_mem( aMem );
  RETURN_NULL();
}

/* 顯示系統的記憶體使用情形 */
FUNCTION( do_memory )
{
  char arg[MAX_INPUT_LENGTH];
  char buf1[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "do_memory" );

  one_argument( argument, arg );

  if ( !arg[0] )
  {
    print_to_char( ch ,
      "影響(affect)       %5d\n\r"
      "區域(area)         %5d\n\r"
      "額外描述(ExtDes)   %5d\n\r"
      "出口(exit)         %5d\n\r"
      "求助(help)         %5d\n\r"
      "怪物(mob)          %5d\n\r"
      "物品(obj)          %5d\n\r"
      "重置(reset)        %5d\n\r"
      "房間(room)         %5d\n\r"
      "商店(shop)         %5d\n\r"
      "教導(teach)        %5d\n\r"
      "進站畫面(greet)    %5d\n\r"
      "地形資料(sector)   %5d\n\r"
      "礦物資料(mineral)  %5d\n\r"
      "職業資料(class)    %5d\n\r"
      "液體資料(liq)      %5d\n\r"
      "限制資料(restrict) %5d\n\r"
      "解謎資料(quest)    %5d\n\r"
      "神族資料(immlist)  %5d\n\r"
      "渡船(ship)         %5d\n\r"
      "版面(board)        %5d\n\r"
      "守護神(angel)      %5d\n\r"
      "禮物(gift)         %5d\n\r"
      "\n\r■■■■ 有關配置的記憶體區塊 部份 ■■■■\n\r"
      "● 區塊配置 %d 格﹐每格配置 %d bytes﹐每格 %dK 共 %d bytes﹐%dK。\n\r"
      "● 總共分配給 %d 個結構﹐花去 %s 位元組﹐約 %dK。\n\r"
      "● 目前區塊指標 %d﹐剩下值 %d﹐已經使用 %d.%d%%。\n\r"
      "● 字串以及區塊共配置 %13s 位元組﹐約 %d.%dK\n\r"
      , top_affect, top_area, top_ed, top_exit, top_help, top_mob_index
      , top_obj_index, top_reset, top_room
      , top_shop, top_teach, top_greeting, top_sector, top_mineral
      , top_class, top_liq, top_restrict, top_quest, top_immlist, top_ship
      , top_board, top_angel, top_gift
      , nAllocPermBlock
      , MAX_PERM_BLOCK
      , ( MAX_PERM_BLOCK / KILO_BYTE )
      , nAllocPermBlock * MAX_PERM_BLOCK
      , nAllocPermBlock * MAX_PERM_BLOCK / KILO_BYTE
      , nAllocPerm
      , numberize( sAllocPerm, NULL )
      , ( sAllocPerm / KILO_BYTE )
      , iMemPerm
      , ( MAX_PERM_BLOCK - iMemPerm )
      , ( iMemPerm * 100  / MAX_PERM_BLOCK )
      , ( iMemPerm * 1000 / MAX_PERM_BLOCK ) % 10
      , numberize( ( nAllocPermBlock * MAX_PERM_BLOCK ), buf1 )
      , ( ( nAllocPermBlock * MAX_PERM_BLOCK ) / KILO_BYTE )
      , ( ( nAllocPermBlock * MAX_PERM_BLOCK * 10 ) / KILO_BYTE ) % 10 );
  }

  else
  {
    send_to_char( "錯誤的參數﹐請查詢正確使用方法﹗\n\r", ch );
  }

  RETURN_NULL();
}

/* 察看系統所要求記憶體的使用狀況 */
FUNCTION( do_perm )
{
  int  count;
  int  iList;
  int  size;

  PUSH_FUNCTION( "do_perm" );

  clear_buffer();
  send_to_buffer( "記憶體使用狀況列表﹕\n\r\n\r" );

  for ( count = iList = 0; iList < MAX_MEM_LIST; iList++ )
  {
    size = rgSizeList[iList] * rgAllocList[iList];

    send_to_buffer( "字串組       %5d 個﹐每個%6d﹐共 %12s "
                     "byte(%5dK)%4d.%1d%%\n\r"
      , rgAllocList[iList] , rgSizeList[iList]
      , numberize( size, NULL )
      , size / KILO_BYTE
      , size * 100 / sAllocPerm
      , ( size * 1000 / sAllocPerm ) % 10 );

    count += rgAllocList[iList];
  }

  for ( iList = 0; ; iList++ )
  {
    int size;
    int num;
    int ReallySize;

    if ( struct_block[iList].type == ERRORCODE ) break;

    num        = struct_block[iList].nAlloc;
    size       = struct_block[iList].size;
    ReallySize = rgSizeList[ mem_block(size) ];
    count += num;

    send_to_buffer(
      "%-12s %5d 個﹐每個%6d﹐共 %12s "
      "byte(%5dK)%4d.%1d%% %6d\n\r"
      , struct_block[iList].name , num , size
      , numberize( num * ReallySize, NULL )
      , num * ReallySize / KILO_BYTE
      , num * ReallySize * 100 / sAllocPerm
      , ( num * ReallySize * 1000 / sAllocPerm ) % 10
      , struct_block[iList].ref );
  }

  send_to_buffer( "\n\r總計﹕共 %d個﹐共花 %s 位元組( %dK )\n\r"
    , count , numberize( sAllocPerm, NULL )
    , sAllocPerm / KILO_BYTE );

  print_buffer( ch );
  RETURN_NULL();
}

/* 察看被用記憶體的情況 */
FUNCTION( do_free )
{
  CHAR_DATA       * pChar;
  OBJ_DATA        * pObj;
  DESCRIPTOR_DATA * pDesc;
  void            * temp_address;
  int               count;
  int               total;
  int               loop;

  PUSH_FUNCTION( "do_free" );
  clear_buffer();

  for ( loop = 0; loop < MAX_MEM_LIST; loop++ )
  {
    count = 0;
    temp_address = rgFreeList[loop];

    while ( rgFreeList[loop] )
    {
      count++;
      rgFreeList[loop] = * ( ( void ** ) rgFreeList[loop] );
    }

    rgFreeList[loop] = temp_address;
    send_to_buffer( "字串( %6d bytes) 有 %4d 個備用﹐已配置了 %5d 個\n\r"
      , rgSizeList[loop] , count , rgAllocList[loop] );
  }

  for ( total = count = 0, pChar = char_list; pChar; pChar = pChar->next )
    if ( pChar->delete ) count++;
    else                 total++;

  send_to_buffer( "\n\r人物結構閒置 %d個( 共 %d個)。\n\r"
    , count, count + total );

  for ( total = count = 0, pObj = object_list; pObj; pObj = pObj->next )
    if ( pObj->delete ) count++;
    else                total++;

  send_to_buffer( "物品結構閒置 %d個( 共 %d個)。\n\r", count, count + total );

  for ( total = count = 0, pObj = object_list; pObj; pObj = pObj->next )
    if ( pObj->delete ) count++;
    else                total++;

  total = count = 0;
  for ( pDesc = descriptor_list; pDesc; pDesc = pDesc->next )
    if ( pDesc->delete ) count++;
    else                 total++;

  send_to_buffer( "人物描述表閒置 %d個( 共 %d個)。\n\r", count, count + total );

  print_buffer( ch );
  RETURN_NULL();
}

/* 計算真正配置記憶體的大小 */
int mem_block( int sMem )
{
  int iList;

  PUSH_FUNCTION( "mem_block" );

  for ( iList = 0; iList < MAX_MEM_LIST; iList++ )
    if ( sMem <= rgSizeList[iList] ) RETURN( iList );

  RETURN( ERRORCODE );
}

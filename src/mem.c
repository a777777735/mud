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
#include "merc.h"

/* ��ܰO����϶����]�N�Ʀr, �i�H���� */
#define MAGIC_NUMBER    'J'
#define KILO_BYTE       1024

/* �t�m�O���骺�j�p */
int rgSizeList [MAX_MEM_LIST]  =
{
  16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768
};

struct magic_block
{
  char magic_number;
  char type;
};

/* �@���t�m�����c��� */
struct  struct_data struct_block [] =
{
  { STRUCT_OBJ_DATA        ,sizeof( OBJ_DATA        ), "���~"    , 0, 0 },
  { STRUCT_CHAR_DATA       ,sizeof( CHAR_DATA       ), "�H��"    , 0, 0 },
  { STRUCT_AFFECT_DATA     ,sizeof( AFFECT_DATA     ), "�v�T"    , 0, 0 },
  { STRUCT_FRIEND_DATA     ,sizeof( FRIEND_DATA     ), "�n�ͦW��", 0, 0 },
  { STRUCT_BASIC_DATA      ,sizeof( BASIC_DATA      ), "�򥻸��", 0, 0 },
  { STRUCT_ENABLE_DATA     ,sizeof( ENABLE_DATA     ), "�P��"    , 0, 0 },
  { STRUCT_FAILCODE_DATA   ,sizeof( FAILCODE_DATA   ), "���~�K�X", 0, 0 },
  { STRUCT_QUEST_DATA      ,sizeof( QUEST_DATA      ), "����"    , 0, 0 },
  { STRUCT_DATABASE_DATA   ,sizeof( DATABASE_DATA   ), "��Ʈw"  , 0, 0 },
  { STRUCT_FILE_DATA       ,sizeof( FILE_DATA       ), "�ɮ�"    , 0, 0 },
  { STRUCT_EXIT_DATA       ,sizeof( EXIT_DATA       ), "�X�f"    , 0, 0 },
  { STRUCT_ALIAS_DATA      ,sizeof( ALIAS_DATA      ), "����"    , 0, 0 },
  { STRUCT_DESCRIPTOR_DATA ,sizeof( DESCRIPTOR_DATA ), "�y�z�l"  , 0, 0 },
  { STRUCT_RESET_DATA      ,sizeof( RESET_DATA      ), "���m"    , 0, 0 },
  { STRUCT_ROOM_INDEX_DATA ,sizeof( ROOM_INDEX_DATA ), "�ж��쫬", 0, 0 },
  { STRUCT_PC_DATA         ,sizeof( PC_DATA         ), "���a�S�O", 0, 0 },
  { STRUCT_ENROLL_DATA     ,sizeof( ENROLL_DATA     ), "�O��"    , 0, 0 },
  { STRUCT_MPROG_DATA      ,sizeof( MPROG_DATA      ), "�Ǫ��{��", 0, 0 },
  { STRUCT_MPROG_ACT_LIST  ,sizeof( MPROG_ACT_LIST  ), "�Ǫ��ʧ@", 0, 0 },
  { STRUCT_ENEMY_DATA      ,sizeof( ENEMY_DATA      ), "����"    , 0, 0 },
  { STRUCT_MOB_INDEX_DATA  ,sizeof( MOB_INDEX_DATA  ), "�Ǫ��쫬", 0, 0 },
  { STRUCT_OBJ_INDEX_DATA  ,sizeof( OBJ_INDEX_DATA  ), "���~�쫬", 0, 0 },
  { STRUCT_LIMIT_DATA      ,sizeof( LIMIT_DATA      ), "������", 0, 0 },
  { STRUCT_SYMBOL_DATA     ,sizeof( SYMBOL_DATA     ), "�Ÿ�"    , 0, 0 },
  { STRUCT_NOTE_DATA       ,sizeof( NOTE_DATA       ), "�H��"    , 0, 0 },
  { STRUCT_BAN_DATA        ,sizeof( BAN_DATA        ), "��}�T��", 0, 0 },
  { STRUCT_EXTRA_DESCR_DATA,sizeof( EXTRA_DESCR_DATA), "�B�~�y�z", 0, 0 },
  { STRUCT_CMD_DATA        ,sizeof( CMD_DATA        ), "�R�O"    , 0, 0 },
  { STRUCT_AREA_DATA       ,sizeof( AREA_DATA       ), "�ϰ�"    , 0, 0 },
  { STRUCT_HELP_DATA       ,sizeof( HELP_DATA       ), "�D�U"    , 0, 0 },
  { STRUCT_SHOP_DATA       ,sizeof( SHOP_DATA       ), "�ө�"    , 0, 0 },
  { STRUCT_SYMBOL_DATA     ,sizeof( SYMBOL_DATA     ), "�Ÿ�"    , 0, 0 },
  { STRUCT_SKILL_DATA      ,sizeof( SKILL_DATA      ), "�ޯ�"    , 0, 0 },
  { STRUCT_SOCIAL_DATA     ,sizeof( SOCIAL_DATA     ), "����"    , 0, 0 },
  { STRUCT_GAMBLE_DATA     ,sizeof( GAMBLE_DATA     ), "���"    , 0, 0 },
  { STRUCT_TEACH_DATA      ,sizeof( TEACH_DATA      ), "�о�"    , 0, 0 },
  { STRUCT_XNAME_DATA      ,sizeof( XNAMES_DATA     ), "�����r"  , 0, 0 },
  { STRUCT_SECTOR_DATA     ,sizeof( SECTOR_DATA     ), "�a�θ��", 0, 0 },
  { STRUCT_CLASS_DATA      ,sizeof( CLASS_DATA      ), "¾�~���", 0, 0 },
  { STRUCT_ADDRESS_DATA    ,sizeof( ADDRESS_DATA    ), "��}���", 0, 0 },
  { STRUCT_BUS_DATA        ,sizeof( BUS_DATA        ), "�a�K���", 0, 0 },
  { STRUCT_NET_DATA        ,sizeof( NET_DATA        ), "���ڳs�u", 0, 0 },
  { STRUCT_CLUB_DATA       ,sizeof( CLUB_DATA       ), "�������", 0, 0 },
  { STRUCT_DAMAGE_DATA     ,sizeof( DAMAGE_DATA     ), "��˸��", 0, 0 },
  { STRUCT_PW_DATA         ,sizeof( PW_DATA         ), "�ϥΪ�"  , 0, 0 },
  { STRUCT_GR_DATA         ,sizeof( GR_DATA         ), "�ڸs"    , 0, 0 },
  { STRUCT_CACHET_DATA     ,sizeof( CACHET_DATA     ), "�ʦL�]��", 0, 0 },
  { STRUCT_LIQ_DATA        ,sizeof( LIQ_DATA        ), "�G��"    , 0, 0 },
  { STRUCT_RESTRICT_DATA   ,sizeof( RESTRICT_DATA   ), "�˳ƭ���", 0, 0 },
  { STRUCT_JOB_DATA        ,sizeof( JOB_DATA        ), "�S����", 0, 0 },
  { STRUCT_QUEST_INFO      ,sizeof( QUEST_INFO      ), "�Ѱg���", 0, 0 },
  { STRUCT_IMMLIST_DATA    ,sizeof( IMMLIST_DATA    ), "���ڸ��", 0, 0 },
  { STRUCT_SHIP_DATA       ,sizeof( SHIP_DATA       ), "���W��", 0, 0 },
  { STRUCT_BOARD_DATA      ,sizeof( BOARD_DATA      ), "����"    , 0, 0 },
  { STRUCT_POST_DATA       ,sizeof( POST_DATA       ), "�峹"    , 0, 0 },
  { STRUCT_HERO_DATA       ,sizeof( HERO_DATA       ), "�^��"    , 0, 0 },
  { STRUCT_EFFECT_DATA     ,sizeof( EFFECT_DATA     ), "����"    , 0, 0 },
  { STRUCT_MESSAGE_DATA    ,sizeof( MESSAGE_DATA    ), "�ԭz"    , 0, 0 },
  { STRUCT_ANGEL_DATA      ,sizeof( ANGEL_DATA      ), "�u�@��"  , 0, 0 },
  { STRUCT_SALE_DATA       ,sizeof( SALE_DATA       ), "���~"  , 0, 0 },
  { STRUCT_FIGHT_DATA      ,sizeof( FIGHT_DATA      ), "�ޢ�"    , 0, 0 },
  { STRUCT_SERIAL_DATA     ,sizeof( SERIAL_DATA     ), "�Ǹ�"    , 0, 0 },
  { STRUCT_TOP_DATA        ,sizeof( TOP_DATA        ), "�έp"    , 0, 0 },
  { STRUCT_WANTED_DATA     ,sizeof( WANTED_DATA     ), "�q�r"    , 0, 0 },
  { STRUCT_VOTE_DATA       ,sizeof( VOTE_DATA       ), "�벼"    , 0, 0 },
  { STRUCT_IP_DATA         ,sizeof( IP_DATA         ), "�s�u"    , 0, 0 },
  { STRUCT_JOKE_DATA       ,sizeof( JOKE_DATA       ), "����"    , 0, 0 },
  { STRUCT_TRACE_DATA      ,sizeof( TRACE_DATA      ), "�a��"    , 0, 0 },
  { STRUCT_VARIABLE_DATA   ,sizeof( VARIABLE_DATA   ), "�ܼ�"    , 0, 0 },
  { STRUCT_TICKET_DATA     ,sizeof( TICKET_DATA     ), "�m��"    , 0, 0 },
  { STRUCT_ORDER_DATA      ,sizeof( ORDER_DATA      ), "����"    , 0, 0 },
  { STRUCT_GIFT_DATA       ,sizeof( GIFT_DATA       ), "§��"    , 0, 0 },
  { STRUCT_STAMP_DATA      ,sizeof( STAMP_DATA      ), "�аO"    , 0, 0 },
  { STRUCT_MINERAL_DATA    ,sizeof( MINERAL_DATA    ), "�q��"    , 0, 0 },
  { STRUCT_MINE_DATA       ,sizeof( MINE_DATA       ), "�q��"    , 0, 0 },
  { STRUCT_EVENT_DATA      ,sizeof( EVENT_DATA      ), "�ƥ�"    , 0, 0 },
  { STRUCT_BOUNTY_DATA     ,sizeof( BOUNTY_DATA     ), "�a����", 0, 0 },
  { STRUCT_SERVER_DATA     ,sizeof( SERVER_DATA     ), "�u�@��"  , 0, 0 },
  { STRUCT_CACHET_DATA     ,sizeof( CACHET_DATA     ), "�ʦL�]��", 0, 0 },
  { STRUCT_QUESTION_DATA   ,sizeof( QUESTION_DATA   ), "���D��"  , 0, 0 },
  { STRUCT_ANSWER_DATA     ,sizeof( ANSWER_DATA     ), "����"    , 0, 0 },
  { STRUCT_ENQUIRE_DATA    ,sizeof( ENQUIRE_DATA    ), "�ݸ�"    , 0, 0 },
  { STRUCT_GREETING_DATA   ,sizeof( GREETING_DATA   ), "�i��"    , 0, 0 },
  { STRUCT_SITUS_DATA      ,sizeof( SITUS_DATA      ), "����"    , 0, 0 },
  { ERRORCODE              , -1                      , NULL     , -1, 0 }
};

/* �ŧi�ϰ��� */
void * alloc_memory     args( ( int , bool ) );
void   free_mem         args( ( void * ) );
int    mem_block        args( ( int ) );

/* �ϰ��ܼ� */
void *  rgFreeList [MAX_MEM_LIST];
int     rgAllocList[MAX_MEM_LIST];
int     nAllocPerm      = 0;
int     sAllocPerm      = 0;
int     nAllocPermBlock = 0;
int     iMemPerm        = 0;
char  * pMemPerm;

/* �_�l�ưO����϶� */
void memory_ini( void )
{
  int iList;

  PUSH_FUNCTION( "memory_ini" );

  for ( iList = 0; iList < MAX_MEM_LIST; iList++ )
  {
    /* Round off �H��n�t�m�O���骺�j�p */
    while ( rgSizeList[iList] % sizeof( long ) ) rgSizeList[iList]++;

    /* �p�G�t�m���j�p��@������϶��٤j, �o�O���檺 */
    if ( rgSizeList[iList] > MAX_PERM_BLOCK )
    {
      perror( "memory_ini: �O����]�w���~" );
      merc_exit( -1 );
    }

    if ( rgSizeList[iList] <= sizeof( struct magic_block ) )
    {
      perror( "memory_ini: �O����]�w���~" );
      merc_exit( -1 );
    }
  }

  RETURN_NULL();
}

/* �t�m�O���骺�֤ߵ{������ */
void * alloc_memory( int sMem , bool record )
{
  struct magic_block * magic;
  void               * pMem;
  int                  iList;
  int                  loop;

  PUSH_FUNCTION( "alloc_memory" );

  if ( ( ( iList = mem_block( sMem + sizeof( struct magic_block ) ) ) == ERRORCODE ) )
    mudlog( LOG_CRIT, "alloc_memory: �t�m %d �줸�հO����Ӥj" , sMem );

  sMem = rgSizeList[iList];

  /* ���p�S���ƥΪ��O���� */
  if ( !rgFreeList[iList] )
  {
    if ( !pMemPerm || iMemPerm + sMem > MAX_PERM_BLOCK )
    {
      /* ���ݬݬO�_�٬O������L��p���϶����H��ϥ� */
      if ( pMemPerm )
      {
        for ( loop = MAX_MEM_LIST - 1; loop >= 0; loop-- )
        {
          /* ������p�϶��H�ѥH��ϥ� */
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

      /* �V�t�ΰt�m�O���� */
      if ( !( pMemPerm = calloc( 1, MAX_PERM_BLOCK ) ) )
        mudlog( LOG_CRIT , "alloc_memory: �L�k�V�t�έn�D�O����" );

      nAllocPermBlock++;
      iMemPerm = 0;
    }

    pMem        = pMemPerm + iMemPerm;
    iMemPerm   += sMem;
    sAllocPerm += sMem;
    nAllocPerm++;

    if ( record == TRUE ) rgAllocList[iList]++;
  }

  /* ���ƥΪ��O����, �ҥH���ίu���h���O����Өϥ� */
  else
  {
    pMem              = rgFreeList[iList];
    rgFreeList[iList] = * ( ( void ** ) rgFreeList[iList] );
  }

  /* �M���O���� */
  memset( pMem, 0, rgSizeList[iList] );

  /* �]�w�]�N�Ʀr�H�ΰO�гo�Ӱ϶����j�p���A */
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

/* ����O����, �N�����Q�ΰ϶��� */
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

    mudlog( LOG_DEBUG, "free_mem: �����`�O���餺�e [%s]", buf );
    mudlog( LOG_EMERG , "free_mem: ���񪺰O����϶��S���]�N�Ʀr" );
  }

  iList = ( int ) magic->type;

  /* �e�J���m����C���� */
  * ( ( void ** ) pMem ) = rgFreeList[iList];
  rgFreeList[iList]      = pMem;

  RETURN_NULL();
}

/* �t�m�r�ꫬ�A�O���� */
void * alloc_string( int sMem )
{
  void * target;

  PUSH_FUNCTION( "alloc_string" );

  target = alloc_memory( sMem, TRUE );
  RETURN( target );
}

/* ����r�ꫬ�A���O���� */
void free_string( char * pstr )
{
  PUSH_FUNCTION( "free_string" );

  if ( pstr == NULL || pstr == &str_empty[0] ) RETURN_NULL();
  free_mem( pstr );

  RETURN_NULL();
}

/* �t�m���c */
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
      mudlog( LOG_ERR , "�t�m���~���A�����c %d", type );

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

    /* �M�䶢�m�����c */
    if ( FreeChar > 0 )
    {
      for ( pChar = char_list; pChar; pChar = pChar->next )
      {
        if ( pChar->delete && pChar->counter <= 0 )
        {
          CHAR_DATA * pNext;

          /* ����W�@�������e */
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

          /* �O�����Ӫ��U�@�ӵ��c, �òM�����O���餺�e���s */
          pNext = pChar->next;
          memset( pChar, 0, sizeof( CHAR_DATA ) );

          /* ���s���V, �ó]�w�����M�� */
          pChar->delete  = FALSE;
          pChar->next    = pNext;

          FreeChar--;
          RETURN( ( void * ) pChar );
        }
      }
    }

    /* �S�����m�����c, �����t��t�m */
    struct_block[iList].nAlloc++;
    pChar = alloc_memory( size, FALSE );

    /* �M���O���餺�e���s */
    memset( pChar, 0, sizeof( CHAR_DATA ) );

    /* �վ��C������, �åB�]�w�����M�� */
    pChar->next    = char_list;
    char_list      = pChar;
    pChar->delete  = FALSE;

    RETURN( ( void * ) pChar );

  case STRUCT_OBJ_DATA:

    /* �M�䶢�m�����c */

    if ( FreeObject > 0 )
    {
      for ( pObj = object_list; pObj; pObj = pObj->next )
      {
        if ( pObj->delete && pObj->counter <= 0 )
        {
          OBJ_DATA * pNext;

          /* ����W�@�������e */
          free_string( pObj->name        );
          free_string( pObj->cname       );
          free_string( pObj->description );

          /* �O�����Ӫ��U�@�ӵ��c, �òM�����O���餺�e���s */
          pNext = pObj->next;
          memset( pObj, 0, sizeof( OBJ_DATA ) );

          /* ���s���V, �ó]�w�����M�� */
          pObj->delete  = FALSE;
          pObj->next    = pNext;

          FreeObject--;
          RETURN( ( void * ) pObj );
        }
      }
    }

    /* �S�����m�����c, �����t��t�m */
    struct_block[iList].nAlloc++;
    pObj = alloc_memory( size, FALSE );

    /* �M���O���餺�e���s */
    memset( pObj, 0, sizeof( OBJ_DATA ) );

    /* �վ��C������, �åB�]�w�����M�� */
    pObj->next    = object_list;
    object_list   = pObj;
    pObj->delete  = FALSE;

    RETURN( ( void * ) pObj );

  case STRUCT_DESCRIPTOR_DATA:

    /* �M�䶢�m�����c */
    if ( FreeDesc > 0 )
    {
      for ( pDesc = descriptor_list; pDesc; pDesc = pDesc->next )
      {
        if ( pDesc->delete && pDesc->counter <= 0 )
        {
          DESCRIPTOR_DATA * pNext;

          /* ����W�@�������e */
          free_string( pDesc->address );
          free_string( pDesc->host    );
          free_string( pDesc->remote  );
          free_string( pDesc->outbuf  );
          free_string( pDesc->buffer  );
          free_string( pDesc->path    );

          /* �O�����Ӫ��U�@�ӵ��c, �òM�����O���餺�e���s */
          pNext = pDesc->next;
          memset( pDesc, 0, sizeof( DESCRIPTOR_DATA ) );

          /* ���s���V, �ó]�w�����M�� */
          pDesc->next      = pNext;
          pDesc->delete    = FALSE;
          pDesc->edit_mode = EDIT_NONE;

          FreeDesc--;
          RETURN( ( void * ) pDesc );
        }
      }
    }

    /* �S�����m�����c, �����t��t�m */
    struct_block[iList].nAlloc++;
    pDesc = alloc_memory( size, FALSE );

    /* �M���O���餺�e���s */
    memset( pDesc, 0, sizeof( DESCRIPTOR_DATA ) );

    /* �վ��C������, �åB�]�w�����M�� */
    pDesc->next     = descriptor_list;
    descriptor_list = pDesc;
    pDesc->delete   = FALSE;

    /* �s�u�H�ƥ[�@ */
    MaxConnect++;

    RETURN( ( void * ) pDesc );
  }

  /* �O���쩳�t�m�F�X�� */
  struct_block[iList].nAlloc++;
  RETURN( alloc_memory( size , FALSE ) );
}

/* ���񵲺c */
void free_struct( void * aMem , int type )
{
  int iList;

  PUSH_FUNCTION( "free_struct" );

  if ( !aMem ) mudlog( LOG_ERR , "���񵲺c���A %d ���ЬO�Ū�" , type );

  for ( iList = 0; ; iList++ )
  {
    if ( struct_block[iList].type == ERRORCODE )
      mudlog( LOG_ERR, "���񵲺c���A���~ %d" , type );

    /* �ݬݳo�ص��c�ण��Q����, �_�h�O���~������ */
    if ( struct_block[iList].type == type ) break;
  }

  switch( type )
  {
  case STRUCT_CHAR_DATA:

    if ( ( ( ( CHAR_DATA * ) aMem )->delete ) == TRUE )
    {
      mudlog( LOG_DEBUG, "free_struct: ��������H�����c." );
      RETURN_NULL();
    }

    ( ( CHAR_DATA * ) aMem )->delete  = TRUE;
    ( ( CHAR_DATA * ) aMem )->counter = 1;
    RETURN_NULL();

  case STRUCT_OBJ_DATA:

    if ( ( ( ( OBJ_DATA * ) aMem )->delete ) == TRUE )
    {
      mudlog( LOG_DEBUG, "free_struct: �������񪫫~���c." );
      RETURN_NULL();
    }

    ( ( OBJ_DATA * ) aMem )->delete  = TRUE;
    ( ( OBJ_DATA * ) aMem )->counter = 1;
    RETURN_NULL();

  case STRUCT_DESCRIPTOR_DATA:

    if ( ( ( ( DESCRIPTOR_DATA * ) aMem )->delete ) == TRUE )
    {
      mudlog( LOG_DEBUG, "free_struct: ��������y�z�l���c." );
      RETURN_NULL();
    }

    ( ( DESCRIPTOR_DATA * ) aMem )->delete      = TRUE;
    ( ( DESCRIPTOR_DATA * ) aMem )->counter     = 1;
    ( ( DESCRIPTOR_DATA * ) aMem )->username[0] = '\x0';
    RETURN_NULL();
  }

  /* �O���쩳�t�m�F�X�� */
  struct_block[iList].nAlloc--;

  free_mem( aMem );
  RETURN_NULL();
}

/* ��ܨt�Ϊ��O����ϥα��� */
FUNCTION( do_memory )
{
  char arg[MAX_INPUT_LENGTH];
  char buf1[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "do_memory" );

  one_argument( argument, arg );

  if ( !arg[0] )
  {
    print_to_char( ch ,
      "�v�T(affect)       %5d\n\r"
      "�ϰ�(area)         %5d\n\r"
      "�B�~�y�z(ExtDes)   %5d\n\r"
      "�X�f(exit)         %5d\n\r"
      "�D�U(help)         %5d\n\r"
      "�Ǫ�(mob)          %5d\n\r"
      "���~(obj)          %5d\n\r"
      "���m(reset)        %5d\n\r"
      "�ж�(room)         %5d\n\r"
      "�ө�(shop)         %5d\n\r"
      "�о�(teach)        %5d\n\r"
      "�i���e��(greet)    %5d\n\r"
      "�a�θ��(sector)   %5d\n\r"
      "�q�����(mineral)  %5d\n\r"
      "¾�~���(class)    %5d\n\r"
      "�G����(liq)      %5d\n\r"
      "������(restrict) %5d\n\r"
      "�������(quest)    %5d\n\r"
      "���ڸ��(immlist)  %5d\n\r"
      "���(ship)         %5d\n\r"
      "����(board)        %5d\n\r"
      "�u�@��(angel)      %5d\n\r"
      "§��(gift)         %5d\n\r"
      "\n\r�������� �����t�m���O����϶� ���� ��������\n\r"
      "�� �϶��t�m %d ��M�C��t�m %d bytes�M�C�� %dK �@ %d bytes�M%dK�C\n\r"
      "�� �`�@���t�� %d �ӵ��c�M��h %s �줸�աM�� %dK�C\n\r"
      "�� �ثe�϶����� %d�M�ѤU�� %d�M�w�g�ϥ� %d.%d%%�C\n\r"
      "�� �r��H�ΰ϶��@�t�m %13s �줸�աM�� %d.%dK\n\r"
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
    send_to_char( "���~���ѼơM�Ьd�ߥ��T�ϥΤ�k�T\n\r", ch );
  }

  RETURN_NULL();
}

/* ��ݨt�Ωҭn�D�O���骺�ϥΪ��p */
FUNCTION( do_perm )
{
  int  count;
  int  iList;
  int  size;

  PUSH_FUNCTION( "do_perm" );

  clear_buffer();
  send_to_buffer( "�O����ϥΪ��p�C��R\n\r\n\r" );

  for ( count = iList = 0; iList < MAX_MEM_LIST; iList++ )
  {
    size = rgSizeList[iList] * rgAllocList[iList];

    send_to_buffer( "�r���       %5d �ӡM�C��%6d�M�@ %12s "
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
      "%-12s %5d �ӡM�C��%6d�M�@ %12s "
      "byte(%5dK)%4d.%1d%% %6d\n\r"
      , struct_block[iList].name , num , size
      , numberize( num * ReallySize, NULL )
      , num * ReallySize / KILO_BYTE
      , num * ReallySize * 100 / sAllocPerm
      , ( num * ReallySize * 1000 / sAllocPerm ) % 10
      , struct_block[iList].ref );
  }

  send_to_buffer( "\n\r�`�p�R�@ %d�ӡM�@�� %s �줸��( %dK )\n\r"
    , count , numberize( sAllocPerm, NULL )
    , sAllocPerm / KILO_BYTE );

  print_buffer( ch );
  RETURN_NULL();
}

/* ��ݳQ�ΰO���骺���p */
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
    send_to_buffer( "�r��( %6d bytes) �� %4d �ӳƥΡM�w�t�m�F %5d ��\n\r"
      , rgSizeList[loop] , count , rgAllocList[loop] );
  }

  for ( total = count = 0, pChar = char_list; pChar; pChar = pChar->next )
    if ( pChar->delete ) count++;
    else                 total++;

  send_to_buffer( "\n\r�H�����c���m %d��( �@ %d��)�C\n\r"
    , count, count + total );

  for ( total = count = 0, pObj = object_list; pObj; pObj = pObj->next )
    if ( pObj->delete ) count++;
    else                total++;

  send_to_buffer( "���~���c���m %d��( �@ %d��)�C\n\r", count, count + total );

  for ( total = count = 0, pObj = object_list; pObj; pObj = pObj->next )
    if ( pObj->delete ) count++;
    else                total++;

  total = count = 0;
  for ( pDesc = descriptor_list; pDesc; pDesc = pDesc->next )
    if ( pDesc->delete ) count++;
    else                 total++;

  send_to_buffer( "�H���y�z���m %d��( �@ %d��)�C\n\r", count, count + total );

  print_buffer( ch );
  RETURN_NULL();
}

/* �p��u���t�m�O���骺�j�p */
int mem_block( int sMem )
{
  int iList;

  PUSH_FUNCTION( "mem_block" );

  for ( iList = 0; iList < MAX_MEM_LIST; iList++ )
    if ( sMem <= rgSizeList[iList] ) RETURN( iList );

  RETURN( ERRORCODE );
}

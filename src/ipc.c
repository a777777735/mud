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
#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "merc.h"

#define  PERM           0600       /* �@�ɰO���骺Ū���g�J�ݩ�     */

int      merc_ipc;                 /* �n�D���t�m�@�ɰO���骺 KEY   */
int      merc_ipc_2;               /* �ƥΦ@�ɰO���骺 KEY         */
int      shmid     = -1;           /* �t�ΰt�m�@�ɰO���骺�s��     */
char   * shm_ptr   = ( char * ) 0; /* �t�Τ��t�@�ɰO���骺��}     */
int      ipc_block = IPC_BLOCK;    /* �i�H���X���O����϶�         */
int      ipc_idle  = IPC_IDLE;     /* ���m�h�[�t�ήɶ��h�R�����϶� */

MEMORY_DATA * get_share_memory_address  args( ( int ) );

/* �ҩl�Ʀ@�ɰO���� */
void init_share_memory( int key, int key2 )
{
  int           size;
  int           count;
  MEMORY_DATA * mem;

  PUSH_FUNCTION( "init_share_memory" );

  /* �p��X�һݪ��@�ɰO���骺�j�p */
  size = sizeof( MEMORY_DATA ) * ipc_block;

  /* ���լO�_���e���϶��S���Q�R��, �����ܹ��էR���ݬ� */
  if ( ( shmid = shmget( key , 0 , PERM ) != -1 ) )
  {
    mudlog( LOG_INFO, "shmget: IPC KEY %d �s�b, ���էR��.", key );
    delete_share_memory( shmget( key, 0, IPC_CREAT ) );
  }

  /* �إߦ@�ɰO����϶� */
  if ( ( shmid = shmget( key, size, IPC_EXCL|IPC_CREAT|PERM ) ) == -1 )
  {
    mudlog( LOG_INFO, "shmget: %s" , strerror( errno ) );

    /* ���եH�ĤG�Ӧ@�ɰO����إ� */
    /*
    if ( ( shmid = shmget( key2, size, IPC_EXCL|IPC_CREAT|PERM ) ) == -1 )
      mudlog( LOG_INFO, "shmget: %s", strerror( errno ) );
    */
    mudlog( LOG_ERR,  "shmget: �L�k���o %d ���@�ɭp����.", key );
  }

  mudlog( LOG_INFO, "shmget: �إ� IPC KEY %d �j�p %d �줸��.", shmid, size );

  /* ���W�@�ɰO����, �t�η|���t�@�Ӧ�} */
  if ( ( shm_ptr = shmat( shmid, ( char * ) 0, 0 ) ) == ( char * ) -1 )
  {
    mudlog( LOG_INFO, "shmat: %s" , strerror( errno ) );
    mudlog( LOG_ERR,  "�L�k���W�@�ɭp����." );
  }

  mudlog( LOG_INFO, "shmat: ���W�@�ɭp�����} %p.", shm_ptr );

  /* �M���@�ɭp����, �B�]�w�ϥΦ��Ƭ� 0 */
  for ( count = 0; count < ipc_block; count++ )
  {
    mem = get_share_memory_address( count );
    clean_share_memory_address( count );
    mem->used = 0;
  }

  RETURN_NULL();
}

/* �R���@�ɰO���� */
void delete_share_memory( int aShmid )
{
  /* �����U�@�ɰO����϶� */
  if ( shmdt( shm_ptr ) == 0 )
    mudlog( LOG_INFO, "shmdt: ���\\���U�@�ɭp����." );
  else
    mudlog( LOG_INFO, "shmdt: ���U�@�ɰO���饢��." );


  /* �R���@�ɰO���� */
  if ( shmctl( aShmid, IPC_RMID, 0 ) == -1 )
  {
    mudlog( LOG_INFO, "shmctl: %s", strerror( errno ) );
    mudlog( LOG_ERR,  "shmctl: �L�k�R�� IPC KEY %d.", aShmid );
  }
  else
  {
    mudlog( LOG_INFO, "shmctl: ���\\�R�� IPC KEY %d.", aShmid );
  }

  /* �аO�w�g�M��, ���U */
  shm_ptr = ( char * ) 0;
  return;
}

/* ���o�Y�s�����@�ɰO���骺��} */
MEMORY_DATA * get_share_memory_address( int slot )
{
  PUSH_FUNCTION( "get_share_memory_address" );

  /* �P�w�O�_�@�ɰO����O�_�w�g���U */
  if ( shm_ptr == ( char * ) 0 )
    mudlog( LOG_ERR, "�@�ɭp����w�g���U, �ٶi��Ū��." );

  /* ���լO�_�s�����~ */
  if ( slot < 0 || slot >= ipc_block )
    mudlog( LOG_CRIT, "get_share_memory_address: �s�����~" );

  /* �p���}�åB�Ǧ^ */
  RETURN( ( MEMORY_DATA * ) ( shm_ptr + ( slot * sizeof( MEMORY_DATA ) ) ) );
}

/* ���o���m���@�ɰO����϶� */
int get_free_share_memory( DESCRIPTOR_DATA * man, int type )
{
  int           count;
  MEMORY_DATA * mem;

  PUSH_FUNCTION( "get_free_share_memory" );

  for ( count = 0; count < ipc_block; count++ )
  {
    mem = get_share_memory_address( count );

    /* ��ݬO�_�Q��w */
    if ( mem->lock == FALSE )
    {
      /* �M���åB�]�w�ƭ� */
      clean_share_memory_address( count );
      mem->lock = TRUE;               /* �]�w�Q��w   */
      mem->type = type;               /* �]�w�϶����A */
      mem->id   = man->id_number;     /* �]�w���a���X */
      mem->used++;                    /* �֥[�ϥΦ��� */
      break;
    }
  }

  if ( count == ipc_block )
  {
    mudlog( LOG_DEBUG, "get_free_share_memory: �S���Ŷ����϶�" );
    RETURN( -1 );
  }

  RETURN( count );
}

/* �ʱ��O�_�����m�Ӥ[���@�ɰO���� */
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

/* �R���w�g�_�u���@�ɰO���� */
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

/* �M���Y�s�����@�ɰO����϶� */
void clean_share_memory_address( int slot )
{
  MEMORY_DATA * mem;

  PUSH_FUNCTION( "clean_share_memory_address" );
  mem = get_share_memory_address( slot );

  mem->done    = FALSE;  /* �϶��O�_�u�@����     */
  mem->lock    = FALSE;  /* �϶��O�_�Q��w       */
  mem->timer   = 0;      /* ��w�᪺�p�ƾ�       */
  mem->id      = -1;     /* �ݩ���@�쪱�a���϶� */
  mem->type    = -1;     /* �϶������A           */
  mem->text[0] = '\x0';  /* �϶�����ư�         */

  RETURN_NULL();
}

/* ��r��]�w���Y�s�����@�ɰO����϶� */
void set_share_memory_text( int slot, char * buffer )
{
  MEMORY_DATA * mem;

  PUSH_FUNCTION( "set_share_memory_text" );

  mem = get_share_memory_address( slot );

  /* ��ݬO�_�|���Q��w�N�]�w��� */
  if ( mem->lock == FALSE )
  {
    mudlog( LOG_INFO, "�s�� %d ���@�ɭp����|����w�N�]�w���.", slot );
    clean_share_memory_address( slot );
  }

  else
  {
    /* ���ˬd��ƨӷ��O�_�|�W�X�d�� */
    if ( str_len( buffer ) < MAX_STRING_LENGTH - 1 )
      str_cpy( mem->text, buffer );

    else
      str_cpy( mem->text, "�T���Ӫ��C" );

    /* �]�w�϶��u�@����, ����Ū�� */
    mem->done = TRUE;
  }

  RETURN_NULL();
}

/* ��ݦ@�ɰO���骺�ϥα��p */
FUNCTION( do_ipcs )
{
  int           count;
  MEMORY_DATA * mem;

  PUSH_FUNCTION( "do_ipcs" );

  for ( clear_buffer(), count = 0; count < ipc_block; count++ )
  {
    mem = get_share_memory_address( count );
    send_to_buffer("�s���R%3d ���X�R%5d ���A�R%2d ��w[%s] "
                   "����[%s] �ɶ��R%5d ���ơR%5d\n\r"
      , count
      , mem->id, mem->type
      , YESNO( mem->lock )
      , YESNO( mem->done )
      , mem->timer , mem->used );
  }

  send_to_buffer( "\n\r�@�� %3d �Ӧ@�ɭp����϶��M�@ %d �줸�աM"
                  "shmid = %d KEY = %d KEY 2 = %d\n\r"
    , ipc_block, ipc_block * sizeof( MEMORY_DATA ), shmid
    , merc_ipc, merc_ipc_2 );

  print_buffer( ch );
  RETURN_NULL();
}

/* Ū���óB�z�w�������@�ɰO����϶��֤� */
void handle_share_memory( void )
{
  int               count;
  char            * buffer;
  char              buf1[MAX_STRING_LENGTH];
  char              buf2[MAX_STRING_LENGTH];
  MEMORY_DATA     * mem;
  DESCRIPTOR_DATA * man;

  PUSH_FUNCTION( "handle_share_memory" );

  /* ��ݭ��ǬO�w�g�����]�w��, �S���h���L�h */
  for ( count = 0; count < ipc_block; count++ )
  {
    mem = get_share_memory_address( count );
    if ( mem->done != TRUE ) continue;

    /* ��ݨ쩳�O�����@�쪱�a����� */
    for ( man = descriptor_list; man; man = man->next )
    {
      if ( !verify_desc( man ) ) continue;
      if ( man->id_number == mem->id ) break;
    }

    /* �䤣��ت������a */
    if ( !man )
    {
      mudlog( LOG_INFO, "���T���o�䤣�쪱�a, �T���Q����." );
      clean_share_memory_address( count );
      continue;
    }

    buffer = mem->text;

    /* �쩳�O���@�ث��A���T���϶� */
    switch( mem->type )
    {
    default:
      mudlog( LOG_INFO, "�T�����A���~ %d.", mem->type );
      break;

    /* ���o FQDN �H�λ��ݨϥΪ̪�ñ�J�b�� */
    case INTERNAL_GETHOSTBYADDR:

      buffer = one_argument( buffer , buf1 );
               one_argument( buffer , buf2 );

      if ( buf1[0] == '\x0' || !str_cmp( man->host, buf1 ) )
      {
        write_to_descriptor( man,
          "\n\r\e[1;33m�g�d�߫�M�o�{�A�������S��FQDN�M�ЦV�A������"
          "�޲z���ӽСM���¦X�@�C\e[0m", 0 );

        if ( fqdn_limit )
        {
          man->fqdn_limit = TRUE;

          write_to_descriptor( man,
            "\n\r\e[1;32m�����@�������ǡM�b�A�����C�����e�M���ӽЦn "
            "FQDN�M���¦X�@�C\e[0m", 0 );
        }
      }

      /* ��}�W�� */
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

      /* ����ñ�J�̱b�� */
      if ( str_cmp( buf2 , FromUnknown ) )
      {
        free_string( man->remote );
        man->remote = str_dup( buf2 );
      }

      man->check_fqdn = TRUE;
      break;

    /* �d�߻��ݨϥΪ̪���� */
    case INTERNAL_FINGER:

      /* �ˬd�O�_�W�X�d��, �_�h�h�������ưϤ� */
      buffer[ sizeof( man->finger_data ) - 1 ] = '\x0';
      str_cpy( man->finger_data, buffer );
      man->check_finger = TRUE;
      break;

    /* �ˬd���a�ɮ׬O�_���T */
    case INTERNAL_CHECKFILE:
      man->check_file = TRUE;
      man->file       = atoi( buffer );
      break;

    case INTERNAL_CHECKSERV:
      if ( buffer[0] == '1' ) man->server = TRUE;
      else                    man->server = FALSE;
      break;
    }

    /* �M���wŪ���������@�ɰO����϶� */
    clean_share_memory_address( count );
  }
  RETURN_NULL();
}

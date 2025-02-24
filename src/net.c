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

#ifdef __FreeBSD__
#include <sys/socket.h>
#endif

#if defined(sun)
#include <sys/socket.h>
#include <strings.h>
#endif

#include <netinet/in.h>
#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <netdb.h>
#include "merc.h"

int             internal_port;

/* ���ڳs�u��� */
FUNCTION( do_mud )
{
  NET_DATA * pNet;
  char       arg[MAX_INPUT_LENGTH];
  char       arg1[MAX_INPUT_LENGTH];
  char       buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "do_mud" );

  /* �D���a����s�u�Ǯ��� */
  if ( IS_NPC( ch ) || !ch->desc ) RETURN_NULL();

  argument = one_argument( argument, arg );

  /* �S���ѼƴN�O�d�� */
  if ( !arg[0] )
  {
    /* �I�s��L��ƨӤu�@ */
    do_netstat( ch, "" );
    RETURN_NULL();
  }

  /* ���_�Y�ͯ������V�s�u */
  else if ( !str_prefix( arg, "!disconnect" ) && IS_GOD( ch ) )
  {
    argument = one_argument( argument, arg1 );

    if ( !arg1[0] )
    {
      send_to_char( "�A�����������@�ӯ��W�C\n\r", ch );
      RETURN_NULL();
    }

    if ( !( pNet = get_net_data( arg1, NET_NAME ) ) )
    {
      act( "�䤣�� $t ���W�M�Х� mud �d�ߡC", ch, arg1, NULL, TO_CHAR );
      RETURN_NULL();
    }

    /* �ˬd��J�޹D�O�_�w�g�s�u */
    if ( valid_socket( pNet, NET_IMPORT ) )
    {
      close_net_socket( pNet, NET_IMPORT );
      act( "���_��s�u $t ��J�C", ch , pNet->name, NULL, TO_CHAR );
    }

    else
    {
      act( "���W $t ��J�s�u�쥻�N�����C", ch, pNet->name, NULL, TO_CHAR );
    }

    /* �ˬd��X�޹D�O�_�w�g�s�u */
    if ( valid_socket( pNet, NET_EXPORT ) )
    {
      close_net_socket( pNet, NET_EXPORT );
      act( "���_��s�u $t ��X�C", ch, pNet->name, NULL, TO_CHAR );
    }
    else
    {
      act( "���W $t ��X�s�u�쥻�N�����C", ch, pNet->name, NULL, TO_CHAR );
    }

    RETURN_NULL();
  }

  /* ��_�Y�ͯ����s�u */
  else if ( !str_prefix( arg, "!reconnect" ) && IS_GOD( ch ) )
  {
    argument = one_argument( argument, arg1 );

    if ( !arg1[0] )
    {
      send_to_char( "�A�������������C\n\r", ch );
      RETURN_NULL();
    }

    if ( !( pNet = get_net_data( arg1, NET_NAME ) ) )
    {
      act( "�䤣�� $t ���W�M�Х� mud �d�ߡC", ch, arg1, NULL, TO_CHAR );
      RETURN_NULL();
    }

    /* �ˬd�O�_�w�g�s�u�F */
    if ( valid_socket( pNet, NET_EXPORT ) )
      act( "���� $t �w�g���o��X�s�u�C", ch, pNet->name, NULL, TO_CHAR );

    /* �ݬݬO�_�T��s�u */
    else if ( pNet->export == -2 )
      act( "���� $t �T���X�s�u�C", ch, pNet->name, NULL, TO_CHAR );

    /* �s�u */
    else init_client( pNet );

    RETURN_NULL();
  }

  /* �M���s�u����� */
  else if ( !str_prefix( arg, "!clean" ) && IS_GOD( ch ) )
  {
    argument = one_argument( argument, arg1 );

    if ( !arg1[0] )
    {
      send_to_char( "�A�������������C\n\r", ch );
      RETURN_NULL();
    }

    if ( !( pNet = get_net_data( arg1, NET_NAME ) ) )
    {
      act( "�䤣�� $t ���W�M�Х� mud �d�ߡC", ch, arg1, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( !valid_socket( pNet, NET_EXPORT ) )
    {
      act( "���� $t �S�����o�s�u�C", ch, arg, NULL, TO_CHAR );
      RETURN_NULL();
    }

    pNet->message[0] = '\x0';
    act( "���� $t�w�g�M����X�w�İϡC", ch, pNet->name, NULL, TO_CHAR );
    RETURN_NULL();
  }

  /* �]�w�аO���p */
  else if ( !str_prefix( arg, "!log" ) && IS_GOD( ch ) )
  {
    argument = one_argument( argument, arg1 );

    if ( !arg1[0] )
    {
      send_to_char( "�A�������������C\n\r", ch );
      RETURN_NULL();
    }

    if ( !( pNet = get_net_data( arg1, NET_NAME ) ) )
    {
      act( "�䤣�� $t ���W�M�Х� mud �d�ߡC", ch, arg1, NULL, TO_CHAR );
      RETURN_NULL();
    }

    /* ���]���e�N�w�g�Q�аO, �h�M�� */
    if ( pNet->log )
    {
      pNet->log = FALSE;
      act( "���W $t �M���аO�C", ch, pNet->name, NULL, TO_CHAR );
    }
    else
    {
      pNet->log = TRUE;
      act( "���W $t �]�w�аO�C", ch, pNet->name, NULL, TO_CHAR );
    }

    RETURN_NULL();
  }

  /* �T�����V�s�u */
  else if ( !str_prefix( arg, "!banexternal" ) && IS_GOD( ch ) )
  {
    argument = one_argument( argument, arg1 );

    if ( !arg1[0] )
    {
      send_to_char( "�A�������������C\n\r", ch );
      RETURN_NULL();
    }

    if ( !( pNet = get_net_data( arg1, NET_NAME ) ) )
    {
      act( "�䤣�� $t ���W�M�Х� mud �d�ߡC", ch, arg1, NULL, TO_CHAR );
      RETURN_NULL();
    }

    /* ��J����, �B�쥻�N�T�� */
    if ( pNet->import == -2 )
    {
      pNet->import = -1;
      act( "$t ��_��J�s�u�C", ch, arg1, NULL, TO_CHAR );
    }

    else
    {
      /* �p�G�s�u��, ���_�L */
      if ( valid_socket( pNet, NET_IMPORT ) )
        close_net_socket( pNet, NET_IMPORT );

      /* �аO����s�u */
      pNet->import = -2;
      act( "$t �����J�s�u�C", ch, arg1, NULL, TO_CHAR );
    }

    /* ��X���, �B�쥻�N�T�� */
    if ( pNet->export == -2 )
    {
      pNet->export = -1;
      init_client( pNet );
      act( "$t ��_��X�s�u�C", ch, arg1, NULL, TO_CHAR );
    }
    else
    {
      /* �p�G�s�u��, ���_�L */
      if ( valid_socket( pNet, NET_EXPORT ) )
        close_net_socket( pNet, NET_EXPORT );

      /* �аO����s�u */
      pNet->export = -2;
      act( "$t �����X�s�u�C", ch, arg1, NULL, TO_CHAR );
    }
    RETURN_NULL();
  }

  else
  {
    if ( !( pNet = get_net_data( arg, NET_NAME ) ) )
    {
      act( "�䤣��A������ $t�C", ch, arg, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( !valid_socket( pNet, NET_EXPORT ) )
    {
      act( "���� $t �S�����o�s�u�C", ch, arg, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( !argument[0] )
    {
      send_to_char( "�A�n�j�Ŷǭ�����S\n\r", ch );
      RETURN_NULL();
    }

    if ( pNet->message[0] )
    {
      act( "���� $t ���u���C", ch, arg, NULL, TO_CHAR );
      RETURN_NULL();
    }

    sprintf( buf, "%s %s %s", ch->name, host_name( ch->desc ), argument );
    buf[sizeof( pNet->message ) - 1] = '\x0';
    str_cpy( pNet->message, buf );

    send_to_char( "�w�g���A�ǻ������F�C\n\r", ch );
  }

  RETURN_NULL();
}

/* �d�ߺ����s�u���p */
FUNCTION( do_netstat )
{
  NET_DATA * pNet;
  char       arg[MAX_INPUT_LENGTH];
  int        count;

  PUSH_FUNCTION( "do_netstat" );

  one_argument( argument, arg );

  /* �S���ѼƴN�O�d�� */
  if ( !arg[0] )
  {
    clear_buffer();

    send_to_buffer( "���� �����W��        ������}             �q�T�� "
      "��J ��X\n\r" );

    for ( count = 0, pNet = net_list; pNet; pNet = pNet->next )
    {
      send_to_buffer( "%-3d. %-15s %-20s %-6d %s   %s\n\r"
        , ++count , pNet->name, pNet->address, pNet->port
        , YESNO( pNet->import >= 0 )
        , YESNO( pNet->export >= 0 ) );

      if ( buffer_full() ) break;
    }

    if ( count == 0 ) send_to_char( "�S�����󪺺��ڳs�u���W���]�w�C\n\r", ch );
    else print_buffer( ch );

    RETURN_NULL();
  }

  if ( !( pNet = get_net_data( arg, NET_NAME ) )
    && !( pNet = get_net_data( arg, NET_ADDRESS ) ) )
  {
    act( "�S�o�{�A�����W $t�M�п�J netstat �d�ߡC", ch, arg, NULL, TO_CHAR );
    RETURN_NULL();
  }

  print_to_char( ch, "���W�R%s\n\r"
                     "���寸�W�R%s\n\r"
                     "��}�R%s ( �s���� = %5d )\n\r"
                     "��X�R[%s] ��J�R[%s] �ӻ{�R[%s] �O���R[%s]\n\r"
                     "Ū�g���� %5d�R%5d\n\r"
                     "�ɶ�����R%5d ��J�ɶ��R%5d ��X�ɶ��R%5d\n\r"
     , pNet->name, pNet->cname ? pNet->cname : "(�q��)"
     , pNet->address, pNet->port
     , YESNO( pNet->export >= 0 )
     , YESNO( pNet->import >= 0 )
     , YESNO( pNet->valid )
     , YESNO( pNet->log )
     , pNet->nRead, pNet->nWrite, pNet->timer
     , pNet->import_duration, pNet->export_duration );

  RETURN_NULL();
}

/* �O�_�����Ī��s�u */
bool valid_socket( NET_DATA * pNet, int type )
{
  PUSH_FUNCTION( "valid_socket" );

  if ( !pNet ) RETURN( FALSE );

  switch( type )
  {
  default:
    RETURN( FALSE );

  /* ��J�譱 */
  case NET_IMPORT:

    if ( pNet->import >= 0 ) RETURN( TRUE );
    break;

  /* ��X�譱 */
  case NET_EXPORT:

    if ( pNet->export >= 0 ) RETURN( TRUE );
    break;
  }

  RETURN( FALSE );
}

/* ���������s�u */
void close_net_socket( NET_DATA * pNet, int type )
{
  PUSH_FUNCTION( "close_net_socket" );

  if ( !pNet ) RETURN_NULL();

  switch( type )
  {
  default:
    mudlog( LOG_DEBUG, "close_net_socket: ���A %d ���~ .", type );
    RETURN_NULL();

  /* ������J�޹D */
  case NET_IMPORT:

    if ( pNet->import >= 0 )
    {
      close( pNet->import );
      pNet->import          = -1;
      pNet->import_duration = 0;
    }

    RETURN_NULL();

  /* ������X�޹D */
  case NET_EXPORT:

    if ( pNet->export >= 0 )
    {
      close( pNet->export );
      pNet->export          = -1;
      pNet->export_duration = 0;
      pNet->message[0]      = '\x0';
    }

    RETURN_NULL();
  }

  RETURN_NULL();
}

/* ���o�Y���ڸ�Ƶ��c */
NET_DATA * get_net_data( char * name, int type )
{
  NET_DATA * pNet;

  PUSH_FUNCTION( "get_net_data" );

  switch( type )
  {
  default:
    mudlog( LOG_DEBUG, "get_net_data: ���~���A %d.", type );
    RETURN( NULL );

  /* �ΦW�٬d�� */
  case NET_NAME:

    for ( pNet = net_list; pNet; pNet = pNet->next )
      if ( !str_cmp( name, pNet->name ) ) RETURN( pNet );

    break;

  /* �Φ�}�d�� */
  case NET_ADDRESS:

    for ( pNet = net_list; pNet; pNet = pNet->next )
      if ( !str_cmp( name, pNet->address ) ) RETURN( pNet );

    break;
  }

  RETURN( NULL );
}

/* ��l�ƺ��ڳs�u */
void launch_internal( int internal )
{
  struct sockaddr_in   sock;
  NET_DATA           * pNet;
  char                 buf[MAX_HOSTNAME+1];
  int                  size;
  int                  desc;
  int                  addr;

  PUSH_FUNCTION( "launch_internal" );

  size = sizeof( sock );
  getsockname( internal, ( struct sockaddr * ) &sock, &size );

  if ( ( desc = accept( internal, (struct sockaddr *) &sock, &size) ) < 0 )
  {
    mudlog( LOG_INFO, strerror( errno ) );
    mudlog( LOG_INFO, "launch_internal: accept �����D." );
    RETURN_NULL();
  }

  if ( fcntl( desc, F_SETFL, ( fcntl( desc , F_GETFL ) | O_NONBLOCK ) ) < 0 )
  {
    mudlog( LOG_INFO, strerror( errno ) );
    mudlog( LOG_INFO, "launch_internal: fcntl �����D" );
    RETURN_NULL();
  }

  size = sizeof(sock);
  if ( getpeername( desc, (struct sockaddr *) &sock, &size ) < 0 )
  {
    mudlog( LOG_NET, "[���ڳs�u] �d�ߤ�����}." );
    close( desc );
    RETURN_NULL();
  }

  addr = ntohl( sock.sin_addr.s_addr );
  sprintf( buf, "%d.%d.%d.%d"
    , ( addr >> 24 ) & 0xFF, ( addr >> 16 ) & 0xFF
    , ( addr >>  8 ) & 0xFF, ( addr       ) & 0xFF );

  /* �ˬd�O�_�b��椧��, ���O�̤���s�u */
  if ( !( pNet = get_net_data( buf, NET_ADDRESS ) ) )
  {
    mudlog( LOG_NET, "[���ڳs�u] %s ���b���w����.", buf );
    close( desc );
    RETURN_NULL();
  }

  /* �ˬd�O�_�аO����s�u */
  if ( pNet->import == -2 )
  {
    mudlog( LOG_NET, "[���ڳs�u] %s ����s�u.", buf );
    close( desc );
    RETURN_NULL();
  }

  if ( pNet->import >= 0 )
  {
    mudlog( LOG_NET, "[���ڳs�u] %s ���Ƴs�u.", buf );
    close_net_socket( pNet, NET_IMPORT );
  }

  pNet->import          = desc;
  pNet->import_duration = 0;

  mudlog( LOG_NET, "[���ڳs�u] %s�����s�u", buf );

  /* �ˬd���V */
  if ( pNet->export == -1 ) init_client( pNet );
  RETURN_NULL();
}

/* ��l�ƻ��ݳs�u */
bool init_client( NET_DATA * pNet )
{
  int                  sockfd;
  struct sockaddr_in   serv;
  struct hostent     * hp;

  PUSH_FUNCTION( "init_client" );

  if ( !pNet ) RETURN( FALSE );

  if ( ( sockfd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
  {
    mudlog( LOG_NET, "init_client: �L�k�}�� socket." );
    RETURN( FALSE );
  }

  serv.sin_family = AF_INET;
  if ( ( hp = gethostbyname( pNet->address ) ) == NULL )
  {
    mudlog( LOG_NET, "init_client: %s �L�k���o.", pNet->address );
    close( sockfd );
    RETURN( FALSE );
  }

  bcopy( ( char * ) hp->h_addr, ( char * ) &serv.sin_addr, hp->h_length );
  serv.sin_port = htons( pNet->port );

  if ( connect( sockfd, ( struct sockaddr * ) &serv, sizeof( serv ) ) < 0 )
  {
    mudlog( LOG_NET, "init_client: �L�k�s�� %s@%d."
      , pNet->address, pNet->port );

    close( sockfd );
    RETURN( FALSE );
  }

  pNet->export          = sockfd;
  pNet->export_duration = 0;
  pNet->message[0]      = '\x0';

  mudlog( LOG_NET, "���o�s�� %s@%d", pNet->address, pNet->port );

  RETURN( TRUE );
}

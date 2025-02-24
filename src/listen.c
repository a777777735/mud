/***************************************************************************
*  �o�O�ѻ��j�ƾǨt�s�@�s�Ҽ��g���C���M�D��� merc ��s�ӨӡM�Ҧ������v    *
*  �N�|�Q�O�d�M���w��j�a�ק�M���ڭ̤]�Ʊ�A�̤]�ണ�ѵ��j�a�M�Ҧ�����    *
*  �~�欰�N���Q���\�C                                                      *
*                                                                          *
*  paul@mud.ch.fju.edu.tw                                                  *
*  lc@mud.ch.fju.edu.tw                                                    *
*                                                                          *
***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/tcp.h>

#define MAX_PORT        5
#define FILENAME        "/proc/net/tcp"
#define TRUE            1
#define FALSE           0

int     hex_to_int      ( char * );

int     listen_port     [MAX_PORT];
int     sleep_time    = 120;
int     interval      = 5;

void main( int argc, char ** argv )
{
  char   buf[BUFSIZ];
  char   number[BUFSIZ];
  FILE * pFile;
  char * status_string_detail;
  char * status_string;
  int    loop;
  int    port;
  int    check;
  int    address;
  int    local;
  int    remote_port;
  int    control;
  int    status;

  for ( loop = 0; loop < MAX_PORT; loop++ ) listen_port[loop] = 0;

  for ( port = 0, loop = 1; loop < argc ; loop++ )
  {
    if ( argv[loop][0] == '-' )
    {
      switch( argv[loop][1] )
      {
      default:
        printf( "���X�z���ѼơC\n\r" );
        exit( -1 );

      case 'I':
      case 'i':
        if ( ( interval = atoi( argv[loop] + 2 ) ) <= 0 )
        {
          printf( "���j�ɶ����X�k�C\n\r" );
          exit( -1 );
        }
        break;

      case 'S':
      case 's':
        if ( ( sleep_time = atoi( argv[loop] + 2 ) ) <= 0 )
        {
          printf( "�ίv�ɶ����X�k�C\n\r" );
          exit( -1 );
        }
        break;

      }
    }

    else
    {
      if ( port >= MAX_PORT )
      {
        printf( "�̤j�u��ʵ� %d �� PORT�C\n\r", MAX_PORT );
        exit( -1 );
      }

      if ( ( address = atoi( argv[loop] ) ) <= 0 || address > 65535 )
      {
        printf( "��ť�� %d ���X�k�C\n\r", address );
        exit( -1 );
      }

      for ( check = 0; check < port; check++ )
      {
        if ( listen_port[check] == address )
        {
          printf( "��ť�� %d ���ơC\n\r", address );
          exit( -1 );
        }
      }

      listen_port[port++] = address;
    }

  }

  if ( port <= 0 )
  {
    printf( "�̤֭n���@�Ӻ�ť��C\n\r" );
    exit( -1 );
  }
  else
  {
    printf( "��ť�U�C����X��R" );
    for ( loop = 0; loop < port; loop++ ) printf( " %d", listen_port[loop] );
    printf( "\n\r" );

    printf( "�ίv�ɶ��R%d\n\r", sleep_time );
    printf( "���j�ɶ��R%d\n\r", interval   );
  }

  /* sleep( sleep_time ); */

  for ( ;; )
  {
    if ( !( pFile = fopen( FILENAME, "r" ) ) )
    {
      printf( "�L�k�}���ɮ� %s�C\n\r", FILENAME );
      exit( -1 );
    }

    fseek( pFile, 129, SEEK_SET );

    for ( ;; )
    {
      if ( feof( pFile ) ) break;
      fread( &buf, 128, 1, pFile );

      for ( local = TRUE, loop = 5; loop < 13; loop++ )
      {
        if ( buf[loop] != '0' )
        {
          local = FALSE;
          break;
        }
      }

      if ( local ) continue;

      sprintf( number, "%s", buf + 14 );
      number[4] = '\x0';
      remote_port = hex_to_int( number );

      for ( control = FALSE, loop = 0; loop < MAX_PORT; loop++ )
      {
        if ( listen_port[loop] > 0 && listen_port[loop] == remote_port )
        {
          control = TRUE;
          break;
        }
      }

      if ( !control ) continue;
      sprintf( number, "%s", buf + 33 );
      number[2] = '\x0';

      switch( status = hex_to_int( number ) )
      {
      default:
        status_string = "TCP_UNKOWN";
        status_string_detail = "Unknown";
        break;

      case TCP_ESTABLISHED:
        status_string = "ESTABLISHED";
        status_string_detail = "Established connection.";
        break;

      case TCP_SYN_SENT:
        status_string = "SYN_SENT";
        status_string_detail = "Attempting to establish a connection.";
        break;

      case TCP_SYN_RECV:
        status_string = "SYNC_RECV";
        status_string_detail = "Being initialized.";
        break;

      case TCP_FIN_WAIT1:
        status_string = "FIN_WAIT1";
        status_string_detail = "Closed, and the connection is shutting down.";
        break;

      case TCP_FIN_WAIT2:
        status_string = "WAIT2";
        status_string_detail = "Closed, and waiting for a shutdown from the remote end.";
        break;

      case TCP_TIME_WAIT:
        status_string = "WAIT";
        status_string_detail = "Waiting after close for remote shutdown retransmission.";
        break;

      case TCP_CLOSE:
        status_string = "CLOSE";
        status_string_detail = "The socket is not being used.";
        break;

      case TCP_CLOSE_WAIT:
        status_string = "CLOSE_WAIT";
        status_string_detail = "The remote end has shut down, waiting for the socket to close.";
        break;

      case TCP_LAST_ACK:
        status_string = "LAST_ACK";
        status_string_detail = "Shut down and the socket closed. Waiting for acknowledgement.";
        break;

      case TCP_LISTEN:
        status_string = "LISTEN";
        status_string_detail = "The socket is listening for incoming connections.";
        break;

      case TCP_CLOSING:
        status_string = "CLOSING";
        status_string_detail = "Both sockets are shut down but we still don't have all our data sent.";
        break;
      }

      printf( "%d %s\n\r", remote_port, status_string );
    }

    fclose( pFile );
    exit( 1 );
  }
}

int hex_to_int( char * buf )
{
  char * pString;
  char * pChar;
  char   mode[] = "0123456789ABCDEF";
  int    number = 0;
  int    shift  = 1;

  for ( pString = buf; *pString; pString++ );

  for ( --pString; pString >= buf; pString-- )
  {
    if ( ( pChar = strchr( mode, *pString ) ) == NULL ) return -1;
    number += ( pChar - mode ) * shift;
    shift *= 16;
  }

  return number;
}

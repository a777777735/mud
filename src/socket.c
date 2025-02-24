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
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <ctype.h>
#include "merc.h"
#include <arpa/inet.h>

#define TCP_FILENAME    "/proc/net/tcp"

int     hex_to_int      args( ( char *, int ) );

static  jmp_buf timebuf;

/* ���� */
static void timeout( int sig )
{
  longjmp( timebuf, sig );
}

char * get_user_name( int desc, struct sockaddr_in * there )
{
  struct sockaddr_in   here;          /* ���a�����c */
  struct sockaddr_in   sin;           /* ��誺���c*/
  int                  length;
  int                  s;
  unsigned int         remote;
  unsigned int         local;
  static char          user[256];
  FILE               * fp;
  char               * cp;
  static char        * result;

  PUSH_FUNCTION( "get_user_name" );

  result = FromUnknown;

  /* ��X���a����}�H�Ϊ� */
  length = sizeof( here );
  if ( getsockname( desc, (struct sockaddr *) & here, &length ) < 0 )
    RETURN( result );

  if ( ( s = socket( AF_INET , SOCK_STREAM , 0 ) ) < 0 ) RETURN( result );

  sin          = here;
  sin.sin_port = 0;
  if ( bind( s, (struct sockaddr *)&sin, sizeof(sin) ) < 0 ) RETURN( result );

  /* �]�w���� */
  signal( SIGALRM, timeout );

  /* �Ĥ@���I�s�@�w�� 0, �Y�O timeout ��~�|����o�� */
  if ( setjmp( timebuf ) != 0 )
  {
    close( s );
    RETURN( FromUnknown );
  }

  alarm( SocketTimeOut );

  sin          = *there;
  sin.sin_port = htons( AuthPort );

  if ( connect( s, (struct sockaddr *) &sin, sizeof(sin) ) < 0
    || ( fp = fdopen( s, "w+" ) ) == NULL )
  {
    close( s );
    alarm( 0 );
    RETURN( result );
  }

  fprintf( fp, "%u,%u\r\n", ntohs(there->sin_port), ntohs(here.sin_port) );
  fflush( fp );

  setbuf( fp , ( char * ) NULL );
  if ( fscanf( fp, "%u , %u : USERID :%*[^:]:%255s"
    , &remote , &local , user ) == 3
      && ferror( fp )           == 0
      && feof( fp )             == 0
      && ntohs(there->sin_port) == remote
      && ntohs(here.sin_port)   == local )
  {
    if ( ( cp = strchr( user, '\r' ) ) ) *cp = '\x0';
    result = user;
  }

  alarm( 0 );
  fclose( fp );
  RETURN( result );
}

/* �ˬd�h���s�u�����O */
FUNCTION( do_multi )
{
  char              arg[MAX_INPUT_LENGTH];
  char              buf[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA * d;
  CHAR_DATA       * victim;
  int               max;
  int               loop;
  int               count;

  PUSH_FUNCTION( "do_multi" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  check_multi_update();
  one_argument( argument , arg );

  if ( !arg[0] )
  {
    /* �ˬd�̤j�� */
    for ( max = 0 , d = descriptor_list; d; d = d->next )
    {
      if ( !verify_desc( d )
        || d->connected != CON_PLAYING
        || !d->character ) continue;
      max = UMAX( max , d->multi );
    }

    /* �ڥ��S���H�h���s�u */
    if ( max == 0 )
    {
      send_to_char( "�S���o�{���H�h���s�u�T\n\r" , ch );
      RETURN_NULL();
    }

    clear_buffer();
    send_to_buffer( "�o�{�� %d �զh���s�u�C���O�O�R" , max );

    for ( loop = 0; loop < max; loop++ )
    {
      send_to_buffer( "\n\r" );
      for ( count = 0 , d = descriptor_list; d; d = d->next )
      {
        if ( !verify_desc( d )
          || d->connected != CON_PLAYING
          || !d->character ) continue;

        if ( d->multi == loop + 1 )
        {
          send_to_buffer( "[%2d-%2d] �u�@��: %s %s\e[0m(%s)\e[80C\e[35D"
            , loop + 1
            , ++count
            , CheckServer ? YESNO( d->server ) : "�S"
            , d->character->cname
            , d->character->name );

          sprintf( buf, "%s(%s)", host_name( d ), d->address );
          buf[35] = '\x0';
          send_to_buffer( "%s\n\r", buf );
        }
      }
    }

    print_buffer( ch );
  }

  else
  {
    if ( ( victim = get_char_world( ch , arg ) ) == NULL )
    {
      act( "�S�����A����H $2$T$0�C" , ch, NULL, arg, TO_CHAR );
      RETURN_NULL();
    }

    if ( IS_NPC( victim ) )
    {
      act( "$N�O�����Ǫ��C", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( !victim->desc )
    {
      act( "$N���u�M�]���S���y�z��i�Ѭd�ߡC", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    check_multi_update();
    if ( victim->desc->multi == 0 )
    {
      act( "$N�S���h���s�u�����ΡC" , ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    clear_buffer();

    send_to_buffer( "�P%s �h���s�u�����R\n\r", mob_name( NULL, victim ) );

    for ( d = descriptor_list , count = 0; d; d = d->next )
    {
      if ( !verify_desc( d )
        || d->connected != CON_PLAYING
        || !d->character
        || d == victim->desc
        || d->multi != victim->desc->multi ) continue;

      send_to_buffer( "[%2d] �u�@��: %s %s\e[0m(%s)\e[80C\e[35D"
        , ++count
        , CheckServer ? YESNO( d->server ) : "�S"
        , d->character->cname
        , d->character->name );

      sprintf( buf, "%s(%s)", host_name( d ), d->address );
      buf[35] = '\x0';
      send_to_buffer( "%s\n\r", buf );
    }

    print_buffer( ch );
    RETURN_NULL();
  }
  RETURN_NULL();
}

/* �ˬd�O�_���h���s�u */
void check_multi_update( void )
{
  DESCRIPTOR_DATA * d;
  DESCRIPTOR_DATA * d_next;
  int               loop;
  bool              touch;

  PUSH_FUNCTION( "check_multi_update" );

  /* �M����� */
  for ( d = descriptor_list; d; d = d->next )
    if ( verify_desc( d ) ) d->multi = 0;

  /* �ˬd�O�_���ۦPñ�J��} */
  for ( loop = 1 , d = descriptor_list; d; d = d->next )
  {
    /* �Y���s�u������, �άO�Ϭd�ߥ��������C�J�p�� */
    if ( !verify_desc( d )
      || d->connected != CON_PLAYING
      || !d->character
      || d->naddr == 0 )
      continue;

    for ( touch = FALSE, d_next = d->next; d_next; d_next = d_next->next )
    {
      /* �Y���s�u������, �άO�Ϭd�ߥ��������C�J�p�� */
      if ( !verify_desc( d_next )
        || d_next->connected != CON_PLAYING
        || !d_next->character
        || d->naddr != d_next->naddr ) continue;

      if ( d->remote
        && d->remote[0]
        && d_next->remote
        && d_next->remote[0]
        && strcmp( d->remote, d_next->remote ) )
        continue;

      if ( d->multi == 0 )
      {
        d->multi = loop;
        touch    = TRUE;
      }

      d_next->multi = d->multi;
    }

    if ( touch ) loop++;
  }

  RETURN_NULL();
}

FUNCTION( do_finger )
{
  CHAR_DATA       * victim;
  DESCRIPTOR_DATA * pDesc;
  DATABASE_DATA   * pData;
  BASIC_DATA      * pBasic;
  CLUB_DATA       * pClub;
  CLASS_DATA      * pClass;
  char              arg[ MAX_INPUT_LENGTH ];
  char              buf[MAX_STRING_LENGTH];
  char              buf1[MAX_INPUT_LENGTH];
  char              buf2[MAX_INPUT_LENGTH];
  char              buf3[MAX_INPUT_LENGTH];
  char              buf4[MAX_INPUT_LENGTH];
  char              buf5[MAX_INPUT_LENGTH];
  char              timebuf[MAX_STRING_LENGTH];
  char              name[MAX_INPUT_LENGTH * 2];
  char              couple[MAX_INPUT_LENGTH];
  int               iTime;
  int               level;
  struct stat       sData;
  struct tm       * sTime;

  PUSH_FUNCTION( "do_finger" );

  one_argument( argument , arg );

  if ( !arg[0] )
  {
    send_to_char( "�A�Q�d�߽֪���ơS\n\r" , ch );
    RETURN_NULL();
  }

  if ( arg[0] == '@' )
  {
    if ( !IS_IMMORTAL( ch ) )
    {
      send_to_char( "�A�S���������v�O���[�ݨ�L�H����ơC\n\r", ch );
      RETURN_NULL();
    }

    if ( !arg[1] )
    {
      send_to_char( "�A�n�[�ݽ֪���ƩO�S\n\r", ch );
      RETURN_NULL();
    }

    if ( !( victim = get_char_world( ch, arg + 1 ) ) )
    {
      act( "�䤣��A���w���H�� $2$T$0�C" , ch, NULL, arg+1, TO_CHAR );
      RETURN_NULL();
    }

    if ( IS_NPC( victim ) )
    {
      act( "����d�߫D���a$N�C", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( !( pDesc = victim->desc ) )
    {
      act( "$N�S���y�z��M�L�k�d�߸�ơC" , ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( pDesc->finger_data && pDesc->finger_data[0] )
      act( "$t", ch, pDesc->finger_data, NULL, TO_CHAR );

    else
      act( "�y�z���S������$N����ơC", ch, NULL, victim, TO_CHAR );
  }

  else
  {
    if ( !( pData = name2database( arg ) ) )
    {
      act( "�藍�_�M$t�S�� $2$T$0 �o�@���H���T", ch, mud_name, arg, TO_CHAR );
      RETURN_NULL();
    }

    if ( !pData->exist )
    {
      act( "�藍�_�M$t�w�g�����L�h���F�T", ch, arg, NULL, TO_CHAR );
      RETURN_NULL();
    }

    str_cpy( name, file_name( arg, SAVE_FILE ) );
    if ( access( name , R_OK ) != 0 )
    {
      act( "�藍�_�M$t�S��$T�o�@���H���T", ch, mud_name, arg, TO_CHAR );
      RETURN_NULL();
    }

    if ( stat( name, &sData ) != 0 )
    {
      act( "Ū�� $2$T$0 ��Ƶo�Ϳ��~�T", ch, NULL, arg, TO_CHAR );
      RETURN_NULL();
    }

    if ( !( sTime = localtime( &sData.st_mtime ) ) )
    {
      act( "�p���� $2$T$0 ��Ʈɵo�Ϳ��~�T", ch, NULL, arg, TO_CHAR );
      RETURN_NULL();
    }

    chinese_number( sTime->tm_year - 11 , buf1 );
    chinese_number( sTime->tm_mon  +  1 , buf2 );
    chinese_number( sTime->tm_mday      , buf3 );
    chinese_number( sTime->tm_min       , buf5 );
    chinese_number( sTime->tm_hour > 12 ? sTime->tm_hour - 12 : sTime->tm_hour , buf4 );

    sprintf( timebuf, "����%s�~%s��%s��%s%s�I%s��"
      , buf1
      , buf2
      , buf3
      , sTime->tm_hour < 12 ? "���W" : "�U��"
      , buf4
      , buf5 );

    for ( victim = char_list; victim; victim = victim->next )
    {
      if ( verify_char( victim )
        && !str_cmp( victim->name, arg )
        && !IS_NPC( victim ) ) break;
    }

    buf[0] = '\x0';
    if ( victim && victim->desc ) str_cpy( buf, host_name( victim->desc ) );

    if ( !( pBasic = serial2basic( &( pData->serial ) ) ) )
    {
      act( "�藍�_�M�L�kŪ��$t����ơT", ch, arg, NULL, TO_CHAR );
      RETURN_NULL();
    }

    iTime = victim ? victim->played + current_time - victim->logon : pBasic->played;
    level = victim ? victim->level : pBasic->level;

    if ( level >= LEVEL_IMMORTAL && level > ch->level )
      str_cpy( timebuf, "-------------------" );

    if ( pBasic->couple.high > 0 && pBasic->couple.low > 0 )
    {
      BASIC_DATA * pCouple;

      if ( ( pCouple = serial2basic( &( pBasic->couple ) ) ) )
      {
        sprintf( couple, "%s\e[0m(%s)", pCouple->cname, pCouple->name );
        free_basic( pCouple );
      }
      else
      {
        str_cpy( couple, "�䤣��t�����" );
      }
    }
    else
    {
      str_cpy( couple, "�S���t��" );
    }

    if ( pBasic->club[0] ) pClub = clubname_lookup( pBasic->club, CLUB_NAME );
    else                   pClub = NULL;

    if ( pBasic->class[0] ) pClass = is_class( pBasic->class, TRUE );
    else                    pClass = NULL;

    print_to_char( ch,
      "�^��N��    �R%s\n\r"
      "����N��    �R%s\e[0m\n\r"
      "�q�l�H�c    �R%s\n\r"
      "���a�~��    �R%d ��\n\r"
      "�s�u�`��    �R%s\n\r"
      "��    ��    �R%d\n\r"
      "��    �O    �R%s\n\r"
      "�t    ��    �R%s\n\r"
      "��    ��    �R%s\e[0m\n\r"
      "¾    �~    �R%s\n\r"
      "�s�u��}    �R%s\n\r"
      "�̫�s�u�ɶ��R%s\n\r"
      "�ɮפj�p    �R%s �줸��\n\r"
      , arg
      , victim ? victim->cname : pBasic->cname
      , victim ? victim->email : pBasic->email
      , victim ? get_age( victim ) : ( pBasic->played / 14400 ) + BornAge
      , iTime > 0 ? date_string( iTime ) : "����"
      , victim ? victim->level : pBasic->level
      , sex_name( pBasic->sex )
      , couple
      , pClub ? pClub->cname : "�S������"
      , class_name( pClass, TRUE )
      , victim ? buf : pBasic->address
      , timebuf
      , numberize( sData.st_size, NULL ) );

    if ( IS_IMMORTAL( ch ) )
    {
      print_to_char( ch,
        "%s"
        "�ɮצ�m    �R%s\n\r"
        "�ɮ׾֦���  �R%s\n\r"
        "�ɮױڸs��  �R%s\n\r"
        "�ɮ��ݩ�    �R%s\n\r"
        , VERTICAL_LINE
        , name
        , pw_lookup( sData.st_uid )
        , gr_lookup( sData.st_gid )
        , perm_of_file( sData.st_mode ) );
    }

    free_basic( pBasic );
    RETURN_NULL();
  }

  RETURN_NULL();
}

char * address_mode( const char * address, int mode )
{
  ADDRESS_DATA * pAddress;

  PUSH_FUNCTION( "address_mode" );

  if ( !address || !*address )
  {
    mudlog( LOG_DEBUG, "address_mode: �ӷ����~." );
    RETURN( "" );
  }

  for ( pAddress = address_list; pAddress; pAddress = pAddress->next )
  {
    if ( !strcmp( pAddress->number, address )
      || !strcmp( pAddress->alpha, address ) )
    {
      switch( mode )
      {
      default:
        RETURN( "" );

      case ADDRESS_ALPHA:
        RETURN( pAddress->alpha );

      case ADDRESS_NUMBER:
        RETURN( pAddress->number );
      }
    }
  }

  RETURN( "" );
}

char * address_lookup( const char * number )
{
  ADDRESS_DATA * pAddress;

  PUSH_FUNCTION( "address_lookup" );

  for ( pAddress = address_list; pAddress; pAddress = pAddress->next )
    if ( !strcmp( pAddress->number , number ) )
      RETURN( pAddress->alpha );

  RETURN( NULL );
}

void set_address_data( const char * number , const char * alpha )
{
  ADDRESS_DATA * pAddress;

  PUSH_FUNCTION( "set_address_data" );

  for ( pAddress = address_list; pAddress; pAddress = pAddress->next )
  {
    if ( !strcmp( pAddress->number, number ) )
    {
      pAddress->count++;
      RETURN_NULL();
    }
  }

  pAddress = alloc_struct( STRUCT_ADDRESS_DATA );
  pAddress->count  = 1;
  pAddress->number = str_dup( number );
  pAddress->alpha  = str_dup( alpha  );
  pAddress->next   = address_list;
  address_list     = pAddress;
  RETURN_NULL();
}

FUNCTION( do_address )
{
  ADDRESS_DATA * pAddress;
  bool           bFound = FALSE;
  char           arg[MAX_INPUT_LENGTH];
  FILE         * pFile;

  PUSH_FUNCTION( "do_address" );

  one_argument( argument , arg );

  if ( !arg[0] )
  {
    clear_buffer();
    for ( pAddress = address_list; pAddress; pAddress = pAddress->next )
    {
      bFound = TRUE;
      send_to_buffer( "%-30s %-30s %5d\n\r"
        , pAddress->number, pAddress->alpha, pAddress->count );
    }

    if ( !bFound ) send_to_buffer( "��}���S�������ơC\n\r" );
    print_buffer( ch );
  }

  else if ( !str_prefix( arg , "!save" ) )
  {
    if ( ( pFile = FOPEN( address_file , "w" ) ) )
    {
      for ( pAddress = address_list; pAddress; pAddress = pAddress->next )
      {
        fprintf( pFile , "%-20s %-30s %5d\n"
          , pAddress->number , pAddress->alpha , pAddress->count );
      }

      if ( ch ) send_to_char( "��}��T�s�ɧ����T\n\r" , ch );
      FCLOSE( pFile );

      /* �����ɮצs���Ҧ� */
      set_file_mode( address_file );

    }
    else
    {
      mudlog( LOG_DEBUG, "do_address: �L�k�g�J��}�ɮ� %s.", address_file );
    }
  }

  else
  {
    clear_buffer();
    for ( pAddress = address_list; pAddress; pAddress = pAddress->next )
    {
      if ( !str_prefix( arg, pAddress->number )
        || !str_prefix( arg, pAddress->alpha  ) )
      {
        bFound = TRUE;
        send_to_buffer( "%-30s %-30s %5d\n\r"
          , pAddress->number, pAddress->alpha, pAddress->count );
      }
    }

    if ( !bFound ) send_to_buffer( "�S������������}�T\n\r" );
    print_buffer( ch );
  }

  RETURN_NULL();
}

int hex_to_int( char * buf, int len )
{
  char * pString;
  char * pChar;
  char   mode[] = "0123456789ABCDEF";
  int    number = 0;
  int    shift  = 1;

  PUSH_FUNCTION( "hex_to_int" );

  if ( !buf || len <= 0 )
  {
    mudlog( LOG_DEBUG, "hex_to_int: �ӷ������T." );
    RETURN( -1 );
  }

  for ( pString = buf; *pString; pString++ ) if ( len-- <= 0 ) break;

  for ( --pString; pString >= buf; pString-- )
  {
    if ( *pString != ' ' )
    {
      if ( ( pChar = strchr( mode, toupper( *pString ) ) ) == NULL )
      RETURN( -1 );

      number += ( pChar - mode ) * shift;
    }

    shift *= 16;
  }

  RETURN( number );
}

FUNCTION( do_tcp )
{
  FILE * pFile;
  char   arg[MAX_INPUT_LENGTH];
  char   buf[MAX_STRING_LENGTH];
  char   remote[BUFSIZ];
  bool   fAll = FALSE;
  bool   fOpen;
  int    port;
  int    loop;
  int    count;
  int    listen;
  int    LocalPort;

  PUSH_FUNCTION( "do_listen" )

  if ( IS_NPC( ch ) ) RETURN_NULL();

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    fAll   = TRUE;
    listen = ERRORCODE;
  }
  else
  {
    if ( !is_number( arg ) )
    {
      send_to_char( "�A������J�Ʀr�Ӫ�ܿ�X��T\n\r", ch );
      RETURN_NULL();
    }

    port = atoi( arg );

    for ( listen = ERRORCODE, loop = 0; loop < MAX_PORT; loop++ )
    {
      if ( MudPort[loop] == port )
      {
        listen = port;
        fAll   = FALSE;
        break;
      }
    }

    if ( listen == ERRORCODE )
    {
      send_to_char( "�A���w����X�𤣬O�X�k���T\n\r", ch );
      RETURN_NULL();
    }
  }

  if ( !( pFile = fopen( TCP_FILENAME, "r" ) ) )
  {
    act( "�L�k�}���[���ɮ� $t�T", ch, TCP_FILENAME, NULL, TO_CHAR );
    RETURN_NULL();
  }

  fseek( pFile, 128, SEEK_SET );

  for ( count = 0, clear_buffer(); ; )
  {
    if ( feof( pFile ) ) break;
    fread( &buf, 128, 1, pFile );

    buf[126] = '\x0';

    if ( hex_to_int( buf + 12, 2 ) == 0
      && hex_to_int( buf + 10, 2 ) == 0
      && hex_to_int( buf + 10, 2 ) == 0
      && hex_to_int( buf + 10, 2 ) == 0 ) continue;

    LocalPort = hex_to_int( buf + 15 , 4 );
    for ( fOpen = FALSE, loop = 0; loop < MAX_PORT; loop++ )
    {
      if ( ( MudPort[loop] == LocalPort && fAll ) || listen == LocalPort )
      {
        fOpen = TRUE;
        break;
      }
    }

    if ( !fOpen ) continue;

    sprintf( remote, "%d.%d.%d.%d"
      , hex_to_int( buf + 26, 2 )
      , hex_to_int( buf + 24, 2 )
      , hex_to_int( buf + 22, 2 )
      , hex_to_int( buf + 20, 2 ) );

    if ( count == 0 ) send_to_buffer( "\e[1;33;44m�s�� ���a�� "
      "��  ��  ��  �}  ���ݰ� �O  �� ���A              "
      "                    \e[0m\n\r" );

    send_to_buffer( "%4d %6d %-15s %6d %6d %-s\n\r"
      , ++count
      , hex_to_int( buf + 15, 4 )
      , remote
      , hex_to_int( buf + 29, 4 )
      , hex_to_int( buf + 83, 7 )
      , tcp_status( hex_to_int( buf + 34, 2 ) ) );
  }

  if ( count == 0 ) send_to_buffer( "�o�ӿ�X��S�������ơT\n\r" );

  print_buffer( ch );
  fclose( pFile );
  RETURN_NULL();
}

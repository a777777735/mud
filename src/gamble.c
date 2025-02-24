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

#define MAX_GAMBLE_MESSAGE      8
#define MIN_GAMBLE_GOLD         100
#define MAX_GAMBLE_GOLD         10000
#define LOTTO_CONGULATION       200000

#define GAMBLE_GOLD             1

/* �ϰ��ܼ� */
int     last_hour;
int     gamble_tick;
int     horse_step [ MAX_HORSES ];
int     horse_data [ MAX_HORSES ];
int     horse_order[ MAX_HORSES ];

char * const horse_name[ MAX_HORSES ] =
{
  "\e[1;31m���ѽ�\e[0m" ,
  "\e[1;32m�j�i��\e[0m" ,
  "\e[1;33m�x�ֽ�\e[0m" ,
  "\e[1;34m���F��\e[0m" ,
  "\e[1;35m�ɤO��\e[0m" ,
  "\e[1;36m�w���\e[0m"
};

char * gamble_message[ MAX_GAMBLE_MESSAGE ] =
{
  "\e[1;33m�p��i�H�w�a�M�j��i�H�����T\e[0m"   ,
  "\e[1;31m�R�w����M�R�w����T\e[0m"           ,
  "\e[1;35m�ݽ䪺�X�ʭӡM�U�`���S�X�ӡT\e[0m"   ,
  "\e[1;32m�Ѥj�a�j�M��ճ̤j�T\e[0m"           ,
  "\e[1;34m�����ӽ�աM�S����ѱC�T\e[0m"       ,
  "\e[1;36m�����Ĺ�M��䪺�|��줣��T\e[0m"   ,
  "\e[1;31m��������M�S������T\e[0m"           ,
  "\e[1;32m�����ФU�`�M�S���Ц^�a��ѱC�T\e[0m"
};

void   gamble_order       args( ( void ) );
char * return_horse_name  args( ( GAMBLE_DATA * gamble ) );
int    multipile          args( ( int number ) );
int    check_gamble       args( ( GAMBLE_DATA * gamble ) );
int    check_gamble_valid args( ( CHAR_DATA * ch , GAMBLE_DATA * gamble ) );

FUNCTION( do_gamble )
{
  GAMBLE_DATA * gamble;
  char          arg[MAX_INPUT_LENGTH];
  char          buf[MAX_STRING_LENGTH];
  int           loop;
  int           count;
  int           money;
  int           horse;
  bool          found;

  PUSH_FUNCTION( "do_gamble" );

  /* �D���a������ */
  if ( IS_NPC( ch ) ) RETURN_NULL();

  argument = one_argument( argument , arg );

  if ( !arg[0] )
  {
    send_to_char( "�p�G���t�ޤ��ɤ������M�п�J \e[1;32mgamble /?\e[0m\n\r"
      , ch );
    RETURN_NULL();
  }

  /* ��ݽ�ժ�������T */
  if ( !str_prefix( arg , "!information" ) && IS_GOD( ch ) )
  {
    clear_buffer();
    send_to_buffer( "��ժ��p�ƾ��R%d�C\n\r" , gamble_tick );
    for ( loop = 0; loop < MAX_HORSES; loop++ )
    {
      send_to_buffer( "[%2d] %s���}�B�� %5d�C\n\r"
        , loop+1 , horse_name[loop] , horse_step[loop] );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  /* �ק�C�@���ު��}�B�� */
  if ( !str_prefix( arg , "!set" ) && IS_GOD( ch ) )
  {
    argument = one_argument( argument , arg );

    if ( ( horse = atoi( arg ) ) < 1 || horse > MAX_HORSES )
    {
      chinese_number( MAX_HORSES, buf );
      act( "�ק諸�ޥu��q 1 �� $t�C" , ch, buf, NULL, TO_CHAR );
      RETURN_NULL();
    }

    argument = one_argument( argument , arg );
    if ( ( count = atoi( arg ) ) < 0 )
    {
      send_to_char( "�ק諸�}�B�Ƥ���p�� 0�C\n\r" , ch );
      RETURN_NULL();
    }

    horse_step[ horse - 1 ] = count;
    act( "�A�ק�F��$i���ު��}�B�Ƭ�$I�C", ch, &horse, &count, TO_CHAR );

    /* �аO */
    mudlog( LOG_INFO , "[GAMBLE] %s �ק�F�t�ު����." , ch->name );

    RETURN_NULL();
  }

  /* ��ݨC�@���ު����p */
  if ( !str_prefix( arg , "!condition" ) )
  {
    for ( clear_buffer(), loop = 0; loop < MAX_HORSES; loop++ )
    {
      send_to_buffer( "[%d] %sĹ�L %5d ���Ĥ@�W�C\n\r"
        , loop + 1 , horse_name[ loop ] , horse_data[ loop ] );
    }

    send_to_buffer( "\n\r�ثe�t�ު����p�O�R" );
    switch ( gamble_tick )
    {
    default : break;
    case -1 : send_to_buffer( "�w���������C\n\r" ); break;
    case  0 : send_to_buffer( "�o�e�������C\n\r" ); break;
    case  1 : send_to_buffer( "��z���a���C\n\r" ); break;
    case  2 :
    case  3 :
    case  4 : send_to_buffer( "�����U�`���C\n\r" ); break;
    case  5 :
    case  6 :
    case  7 :
    case  8 : send_to_buffer( "���ɶi�椤�C\n\r" ); break;
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  /* ���ɤ��ɪ����p */
  if ( !str_prefix( arg , "!report" ) )
  {
    if ( gamble_tick < 5 )
    {
      send_to_char( "���ɳ��٨S�}�l�M����[�ݤ��ɪ����ΡS\n\r" , ch );
      RETURN_NULL();
    }

    for ( gamble_order(), clear_buffer(), loop = 0; loop < MAX_HORSES; loop++ )
    {
      send_to_buffer( "�{�b�]�� %d ���ެO�R%s(%d)\n\r"
        , loop + 1
        , horse_name[horse_order[loop]]
        , horse_order[loop] + 1 );
    }

    send_to_buffer( "\n\r\n\r" );
    print_buffer( ch );
    RETURN_NULL();
  }

  /* �ˬd�U�`�����p */
  if ( !str_prefix( arg , "!check" ) )
  {
    clear_buffer();
    send_to_buffer( "�A�ثe�ҤU���`���R\n\r" );
    for ( found = FALSE, gamble = ch->gamble; gamble; gamble = gamble->next )
    {
      found = TRUE;
      send_to_buffer( "�A��F%d��Ȥl�R�F%s�C\n\r"
        , gamble->mount , return_horse_name( gamble ) );
    }

    if ( !found ) send_to_buffer( "�A�S���U���󪺪`�C\n\r" );
    print_buffer( ch );
    RETURN_NULL();
  }

  /* �U�`�������D�n�{�� */
  if ( !str_prefix( arg , "gold" ) || !str_prefix( arg , "coin" ) )
  {
    /* ���O�U�`���ɶ� */
    if ( gamble_tick < 2 || gamble_tick >=5 )
    {
      send_to_char( "�{�b����U�`�C�M�е��@�U�a�C\n\r" , ch );
      RETURN_NULL();
    }

    if ( ch->trade == FALSE )
    {
      send_to_char( "�A���`�겣�ӥi�ȤF�M�ҥH�L�k����o�ӫ��O�T\n\r", ch );
      RETURN_NULL();
    }

    if ( !IS_NPC( ch ) && ch->level < level_limit )
    {
      act( "�b�A�ɯŨ�$i�ūe�M�Ф��n�H�����������M�ӭt�C"
        , ch, &level_limit, NULL, TO_CHAR );

      RETURN_NULL();
    }

    if ( ch->donate > 0 )
    {
      send_to_char( "���٪��i���O���A���ӽ�եΪ��T\n\r", ch );
      RETURN_NULL();
    }

    argument = one_argument( argument , arg );

    /* �S���U�@�Ӥޭz */
    if ( !arg[0] )
    {
      send_to_char( "�ݨӧA���O��L����M�٬O���d�� gamble �a�C\n\r" , ch );
      RETURN_NULL();
    }

    /* ��`�Ӥ� */
    if ( ( money = atoi( arg ) ) < MIN_GAMBLE_GOLD )
    {
      send_to_char( "�o��p����`�M�������̭���F�C\n\r" , ch );
      RETURN_NULL();
    }

    /* ��`�Ӧh */
    if ( money > MAX_GAMBLE_GOLD )
    {
      send_to_char( "��M�o��j����`�M�کȷ|�˲��C�C\n\r" , ch );
      RETURN_NULL();
    }

    /* ���W���������ӽ� */
    if ( money > ch->gold )
    {
      send_to_char( "�u�O��ʰ�j�M���L�A���w�@�ǿ��ӽ�a�C\n\r" , ch );
      RETURN_NULL();
    }

    /* �t�m��յ��c���O���� */
    gamble = alloc_struct( STRUCT_GAMBLE_DATA );

    /* �M�����Ǹ�� */
    for ( count = 0; count < MAX_HORSES; count++ ) gamble->horse[ count ] = -1;

    count    = 0;
    argument = one_argument( argument , arg );

    while ( count < MAX_HORSES && arg[0] )
    {
      if ( ( horse = atoi( arg ) ) > MAX_HORSES || horse < 1 )
      {
        count = 0;
        break;
      }

      gamble->horse[ count ] = horse - 1;
      count++;
      argument = one_argument( argument , arg );
    }

    /* �ڥ��S����J�ް������X�άO��J�覡���� */
    if ( count == 0 )
    {
      for ( count = 0; count < MAX_HORSES; count++ ) gamble->horse[count] = -1;

      /* ���񵲺c���O���� */
      free_struct( gamble , STRUCT_GAMBLE_DATA );

      send_to_char( "�A�U�`���覡����M�Ьd�� gamble ���ϥΤ�k�C\n\r" , ch );
      RETURN_NULL();
    }

    /* �ˬd�O�_�����ƩάO���~ */
    if ( !( check_gamble_valid( ch , gamble ) ) )
    {
      for ( count = 0; count < MAX_HORSES; count++ ) gamble->horse[count] = -1;

      /* ���񵲺c���O���� */
      free_struct( gamble , STRUCT_GAMBLE_DATA );

      send_to_char( "�A�U�`���m���O�L�Ī��M�Ьd�� GAMBLE�C\n\r" , ch );
      RETURN_NULL();
    }

    act( "�A��F$i��Ȥl�R�F$T�C"
      , ch, &money, return_horse_name( gamble ), TO_CHAR );

    /* ���ƶ�J���c���� */
    gamble->format = GAMBLE_GOLD;
    gamble->mount  = money;

    /* ���J���a�� gamble �����c�� */
    gamble->next = ch->gamble;
    ch->gamble   = gamble;

    /* �����m�� */
    gold_from_char( ch, money );

    /* �j��}����W�D */
    if ( IS_SET( ch->deaf , CHANNEL_GAMBLE ) )
    {
      REMOVE_BIT( ch->deaf , CHANNEL_GAMBLE );
      send_to_char( "�޳��Ѥj���K���A�����W�D���}�F�C\n\r" , ch );
    }

    RETURN_NULL();
  }

  do_gamble( ch , "" );
  RETURN_NULL();
}

/* �ҩl�ƽ�ժ��Ѽ� */
void initial_gamble( void )
{
  int loop;

  PUSH_FUNCTION( "inital_gamble" );

  gamble_tick =  1;
  last_hour   = -1;

  /* �M���Ĥ@�W����� */
  for ( loop = 0 ; loop < MAX_HORSES; loop++ ) horse_data[loop] = 0;

  RETURN_NULL();
}

/* �t���t�ު��D�n�{�� */
void gamble_update( void )
{
  int               loop;
  int               lotto;
  char              buf[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA * d;
  GAMBLE_DATA     * gamble;
  CHAR_DATA       * ch;

  PUSH_FUNCTION( "gamble_update" );

  /* �üƨM�w�޶]���B�� */
  if ( gamble_tick >= 5 )
  {
    for ( loop = 0; loop < MAX_HORSES; loop++ )
      horse_step[loop] += number_range( 1 , 5 );
  }

  /* �C�@�Ӥp�ɧ@�@���ʧ@, ���M�S���ʧ@ */
  if ( time_info.hour == last_hour ) RETURN_NULL();

  /* �O���W�����ɶ� , �åB�� tick �W�[�@ */
  last_hour = time_info.hour;
  gamble_tick++;

  /* ����, �åB��� */
  if ( gamble_tick == 0 )
  {
    /* �M���Ҧ����a����ո�� */
    for ( d = descriptor_list; d; d = d->next )
    {
      if ( !verify_desc( d )
        || d->connected != CON_PLAYING
        || !( ch = d->character ) ) continue;

      for ( gamble = ch->gamble; gamble; gamble = gamble->next )
      {
        if ( gamble->format == GAMBLE_GOLD )
        {
          if ( ( lotto = check_gamble( gamble ) ) > 0 )
          {
            if ( number_range( 1 , GambleLost ) != GambleLost )
            {
              act( "���H�A�M�A��$tĹ�o$I��Ȥl�C"
                , ch, return_horse_name( gamble ) , &lotto, TO_CHAR );

              if ( !over_scale( ch ) ) gold_to_char( ch, lotto );

              /* �e�X���H���T�� */
              if ( lotto > LOTTO_CONGULATION )
              {
                sprintf( buf , "%s���˹B���F�j���M���ָ�L�����a�C"
                  , mob_name( NULL, ch ) );

                talk_channel_2( buf, CHANNEL_GAMBLE, "" );
              }
            }

            else
            {
              act( "�u��p�M�A��$t�W�S�����Ѥj�����r�аO�M�ҥH����ơC"
                , ch, return_horse_name( gamble ), NULL, TO_CHAR );

              if ( lotto > LOTTO_CONGULATION )
              {
                sprintf( buf , "%s���˹B���F�j���M���L�Q�޳�"
                  "�Ѥj���¦Y�¤F�C", mob_name( NULL, ch ) );

                talk_channel_2( buf, CHANNEL_GAMBLE, "" );
              }
            }
          }

          else
          {
            act( "�u��p�M�A��$t�Q��@�޷|�Ѥj�����ѱC���F�C"
              , ch, return_horse_name( gamble ), NULL, TO_CHAR );
          }
        }
      }
      extract_gamble( d->character );
    }
    RETURN_NULL();
  }

  /* �}�l�U�` */
  if ( gamble_tick == 2 )
  {
    talk_channel_2( "�t�ޤ��ɤS�}�l�F�M�ЦU��j�ݤU�`�a�C"
      , CHANNEL_GAMBLE, "" );
    RETURN_NULL();
  }

  /* �U�`�ɶ�, �åB�H���e�X�U�`���T�� */
  if ( gamble_tick < 5 && gamble_tick > 2 )
  {
    talk_channel_2(
      gamble_message[ number_range( 0, MAX_GAMBLE_MESSAGE- 1) ]
      , CHANNEL_GAMBLE, "" );

    RETURN_NULL();
  }

  /* �}�l�t�� */
  if ( gamble_tick == 5 )
  {
    /* ��C�@�������]���B�ƲM�����s */
    for ( loop = 0; loop < MAX_HORSES; loop++ ) horse_step[loop] = 0;

    talk_channel_2( "����A�U�`�F�M�}�l�t�ޤF�C", CHANNEL_GAMBLE, "" );
    RETURN_NULL();
  }

  /* �����t�� */
  if ( gamble_tick == 8 )
  {
    /* ���t�ު����G�Ƥ@�U���� */
    gamble_order();

    /* �ŧi���ɵ���, �åB���G�W�� */
    clear_stack();
    send_to_stack( "�t�ަW���O " );

    for ( loop = 0; loop < MAX_HORSES; loop++ )
    {
      send_to_stack( "%s(%d)"
        , horse_name[horse_order[loop]]
        , horse_order[loop] + 1 );
      if ( loop != MAX_HORSES - 1 ) send_to_stack( "-" );
    }

    talk_channel_2( return_stack(), CHANNEL_GAMBLE, "" );

    /* �Ĥ@�W���ާ�O���[�@ */
    horse_data[ horse_order[0] ]++;

    /* ��O�ƾ��k�s, �U�@���N�O�o����� */
    gamble_tick = -1;

    RETURN_NULL();
  }
  RETURN_NULL();
}

/* ��޶]�����ǱƦC�@�U */
void gamble_order( void )
{
  int horse_temp[ MAX_HORSES ];
  int loop1;
  int loop2;
  int temp;

  PUSH_FUNCTION( "gmable_order" );

  for ( loop1 = 0; loop1 < MAX_HORSES; loop1++ )
  {
    horse_temp[loop1]  = horse_step[loop1];
    horse_order[loop1] = loop1;
  }

  for ( loop1 = 0; loop1 < MAX_HORSES - 1 ; loop1++ )
  {
    for ( loop2 = loop1 + 1; loop2 < MAX_HORSES; loop2++ )
    {
      if ( horse_temp[loop1] < horse_temp[loop2] )
      {
        temp               = horse_temp[loop1];
        horse_temp[loop1]  = horse_temp[loop2];
        horse_temp[loop2]  = temp;

        temp               = horse_order[loop1];
        horse_order[loop1] = horse_order[loop2];
        horse_order[loop2] = temp;
      }
    }
  }
  RETURN_NULL();
}

/* �Ǧ^�R���m�����ް����X */
char * return_horse_name( GAMBLE_DATA * gamble )
{
  int loop;

  PUSH_FUNCTION( "return_horse_name" );

  clear_stack();

  for ( loop = 0; loop < MAX_HORSES; loop++ )
  {
    if ( gamble->horse[loop] == -1 ) break;

    send_to_stack( "%s(%d) "
      , horse_name[gamble->horse[loop]] , gamble->horse[loop] + 1 );
  }
  RETURN( return_stack() );
}

/* �R���Y���a����ո�Ƶ��c */
void extract_gamble( CHAR_DATA * ch )
{
  GAMBLE_DATA * temp;
  GAMBLE_DATA * temp_next;

  PUSH_FUNCTION( "extract_gamble" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG , "�P�����a��ո�ƮɤH���O�Ū�." );
    RETURN_NULL();
  }

  for ( temp = ch->gamble; temp; temp = temp_next )
  {
    temp_next    = temp->next;

    temp->format = -1;
    temp->mount  =  0;

    /* ���񵲺c���O���� */
    free_struct( temp , STRUCT_GAMBLE_DATA );
  }

  ch->gamble = NULL;
  RETURN_NULL();
}

/* �ˬd�O�_�M���G�@�� */
int check_gamble( GAMBLE_DATA * gamble )
{
  int loop;
  int right;

  PUSH_FUNCTION( "check_gamble" );

  for ( loop = 0, right = 0; loop < MAX_HORSES; loop++ )
  {
    if ( gamble->horse[loop] == -1 ) break;
    if ( gamble->horse[loop] != horse_order[loop] )
    {
      right = 0;
      break;
    }
    right++;
  }

  RETURN ( multipile( right ) * gamble->mount );
}

/* ��X�X���������� */
int multipile( int number )
{
  int loop;
  int count;

  PUSH_FUNCTION( "multipile" );

  /* �p�G�p�󵥩�0 , ���Ƭ� 0 */
  if ( number <= 0 ) RETURN( 0 );

  count = 1;
  loop  = MAX_HORSES;

  while ( number-- > 0 ) count *= loop--;

  RETURN( count );
}

/* �ˬd�R���m���O�_���� */
int check_gamble_valid( CHAR_DATA * ch , GAMBLE_DATA * gamble )
{
  int           loop;
  int           same;
  GAMBLE_DATA * temp;
  int           repeat_table[ MAX_HORSES ];

  PUSH_FUNCTION( "check_gamble_valid" );

  for ( temp = ch->gamble ; temp; temp = temp->next )
  {
    for ( same = TRUE, loop = 0; loop < MAX_HORSES; loop++ )
      if ( temp->horse[loop] != gamble->horse[loop] ) same = FALSE;

    if ( same == TRUE ) RETURN( FALSE );
  }

  for ( loop = 0; loop < MAX_HORSES; loop++ ) repeat_table[loop] = -1;

  for ( loop = 0; loop < MAX_HORSES; loop++ )
  {
    if ( gamble->horse[loop] == -1 ) break;
    if ( repeat_table[gamble->horse[loop]] != -1 ) RETURN( FALSE );
    repeat_table[gamble->horse[loop]]++;
  }
  RETURN( TRUE );
}

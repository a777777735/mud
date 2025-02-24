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
#include <stdio.h>
#include <stdlib.h>
#include "merc.h"

int     NowBounty       =       0;
int     BountyLimit     =      10;

FUNCTION( do_bounty )
{
  BOUNTY_DATA     * pBounty;
  ROOM_INDEX_DATA * pRoom;
  CHAR_DATA       * pMob;
  char              arg[MAX_INPUT_LENGTH];
  char              buf[MAX_STRING_LENGTH];
  char              buf1[MAX_STRING_LENGTH];
  int               loop;
  int               slot;

  PUSH_FUNCTION( "do_bounty" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  if ( !bounty_list )
  {
    act( "�藍�_�M$t�S�������a���ơT", ch, mud_name, NULL, TO_CHAR );
    RETURN_NULL();
  }

  argument = one_argument( argument ,arg );

  if ( arg[0] == '\x0' )
  {
    if ( NowBounty <= 0 )
    {
      act( "�ثe$t�S�������a�઺�W��M�Э@�ߦa���ԧa�T", ch, mud_name, NULL, TO_CHAR );
      RETURN_NULL();
    }

    clear_buffer();
    for ( loop = 0, pMob = char_list; pMob; pMob = pMob->next )
    {
      if ( !verify_char( pMob )
        || !( pBounty = pMob->bounty )
        || !pMob->in_room
        || !pMob->in_room->area
        || !pMob->in_room->area->name )
        continue;

      if ( loop == 0 )
        send_to_buffer( "\e[1;33;44m���� �Ҧb�ϰ�W�� �Ǫ��W��             "
          "      ����                              \e[0m\n\r" );

      chinese_number( pBounty->value, buf );
      send_to_buffer( "%4d %-12s %-26s %s%s%s\n\r"
        , ++loop
        , pMob->in_room->area->name
        , mob_name( ch, pMob )
        , bounty_type( pBounty->type )
        , buf
        , bounty_unit( pBounty->type ) );

      if ( buffer_full() ) break;
    }

    if ( loop == 0 )
      send_to_buffer( "�ثe�S�������a�઺�W��M�Э@�ߦa���ԧa�T\n\r" );

    if ( loop != NowBounty ) mudlog( LOG_DEBUG, "do_bounty: �`�Ƥ��ŦX." );

    print_buffer( ch );
  }

  else if ( !str_prefix( arg, "lock" ) && IS_IMMORTAL( ch ) )
  {
    one_argument( argument, arg );

    if ( arg[0] == '\x0' || !is_number( arg ) )
    {
      send_to_char( "�A�Q��w�ĴX���a��O�S\n\r", ch );
      RETURN_NULL();
    }

    slot = atoi( arg );
    loop = 1;
    for ( pBounty = bounty_list; pBounty; pBounty = pBounty->next, loop++ )
    {
      if ( slot == loop )
      {
        if ( pBounty->lock )
        {
          pBounty->lock = FALSE;
          act( "�A�Ѱ��F��$i�����a����w�T", ch, &slot, NULL, TO_CHAR );
        }

        else
        {
          pBounty->lock = TRUE;
          act( "�A�]�w�F��$i�����a����w�T", ch, &slot, NULL, TO_CHAR );

          for ( pMob = char_list; pMob; pMob = pMob->next )
          {
            if ( verify_char( pMob ) && pMob->bounty == pBounty )
            {
              pMob->bounty = NULL;
              pBounty->count--;
              pBounty->killed++;
              NowBounty--;

              act( "$n���A�O�Q�a�઺�H�F�T", pMob, NULL, NULL, TO_ROOM );
            }
          }
        }

        RETURN_NULL();
      }
    }

    send_to_char( "�藍�_�M�S�����Ӷ��Ǫ���ơT\n\r", ch );
  }

  else if ( !str_prefix( arg, "go" ) && IS_IMMORTAL( ch ) )
  {
    one_argument( argument, arg );
    if ( arg[0] == '\x0' || !is_number( arg ) )
    {
      send_to_char( "�A�Q���ĴX���a��ߨ�ҰʡS\n\r", ch );
      RETURN_NULL();
    }

    slot = atoi( arg );
    loop = 1;
    for ( pBounty = bounty_list; pBounty; pBounty = pBounty->next, loop++ )
    {
      if ( loop == slot )
      {
        if ( pBounty->lock )
        {
          send_to_char( "�o����ƳQ��w�M�ҥH�L�k�ҰʡT\n\r", ch );
          RETURN_NULL();
        }

        if ( !pBounty->mob || !pBounty->msg || !*pBounty->msg )
        {
          send_to_char( "��Ƥ������M�ҥH�L�k�ҰʡT\n\r", ch );
          RETURN_NULL();
        }

        if ( pBounty->count >= pBounty->max )
        {
          send_to_char( "�ƶq�w�F�W���M�ҥH�L�k�ҰʡC\n\r", ch );
          RETURN_NULL();
        }

        if ( !( pRoom = random_room( get_room_area( pBounty->room ) ) ) )
        {
          send_to_char( "�䤣��X�{���ж��M�Ұʥ��ѡT\n\r", ch );
          RETURN_NULL();
        }

        if ( !( pMob = shape_mobile( pBounty->mob, pRoom ) ) )
        {
          send_to_char( "�藍�_�M�L�k���ͩǪ��MĲ�o���ѡT\n\r", ch );
          RETURN_NULL();
        }

        NowBounty++;
        pBounty->count++;
        pBounty->occurred++;
        pMob->bounty = pBounty;
        pMob->gold   = 0;

        chinese_number( pBounty->value, buf1 );
        sprintf( buf, "%s�M�Z�O�i�H����%s\e[1;33m���H�M���i�H���%s%s%s�T"
          , pBounty->msg
          , mob_name( NULL, pMob )
          , bounty_type( pBounty->type )
          , buf1
          , bounty_unit( pBounty->type ) );

        talk_channel_2( buf, CHANNEL_BULLETIN, "�a��" );

        send_to_char( "�A���Ĳ�o�o�����a��ƥ�T\n\r", ch );
        RETURN_NULL();
      }
    }

    send_to_char( "�藍�_�M�S�����Ӷ��Ǫ���ơT\n\r", ch );
  }

  else if ( !str_prefix( arg, "status" ) && IS_IMMORTAL( ch ) )
  {
    one_argument( argument, arg );

    if ( arg[0] == '\x0' )
    {
      clear_buffer();
      send_to_buffer( "\e[1;33;44m���� �� �� �W ��          �� �X ���v"
        " �� �� �ƶq Max ��    �A �ƭ� �E�p �o�� �Q��\e[0m\n\r" );

      loop = 0;
      for ( pBounty = bounty_list; pBounty; pBounty = pBounty->next, loop++ )
      {
        if ( !pBounty->mob ) continue;
        send_to_buffer( "%4d %-20s %5d %4d %5d %4d %3d %-8s %4d %4d %4d %4d\n\r"
          , loop + 1
          , mob_index_name( NULL, pBounty->mob )
          , pBounty->mob->vnum
          , pBounty->chance
          , pBounty->room
          , pBounty->count
          , pBounty->max
          , bounty_type( pBounty->type )
          , pBounty->value
          , pBounty->migration
          , pBounty->occurred
          , pBounty->killed );

        if ( buffer_full() ) break;
      }

      send_to_buffer( "\n\r�ܤ֭n %d �H�H�W�~�|�}���a��ƥ�T\n\r"
        , BountyLimit );

      print_buffer( ch );
    }

    else if ( !is_number( arg ) )
    {
      send_to_char( "�A�����ǥ����O���X�T\n\r", ch );
      RETURN_NULL()
    }

    else
    {
      if ( ( slot = atoi( arg ) ) <= 0 )
      {
        send_to_char( "�A�����ǼƦr���X�z�T\n\r", ch );
        RETURN_NULL();
      }

      loop = 1;
      for ( pBounty = bounty_list; pBounty; pBounty = pBounty->next, loop++ )
      {
        if ( loop == slot && pBounty->mob && pBounty->msg )
        {
          print_to_char( ch,
            "��    �ǡR %d\n\r"
            "�Ǫ����X�R %d\n\r"
            "�Ǫ��W�١R %s\n\r"
            "�X�{�ж��R %d\n\r"
            "�X�{���v�R �d���� %d\n\r"
            "�ثe�ƶq�R %d\n\r"
            "�̤j�ƶq�R %d\n\r"
            "���૬�A�R %s\n\r"
            "����ƭȡR %d\n\r"
            "�E�p���v�R �ʤ��� %d\n\r"
            "���ͦ��ơR %d ��\n\r"
            "�Q �� ���R %d ��\n\r"
            "�Q �� �w�R %s\n\r"
            "��X�ԭz�R\n\r%s\n\r"
            , loop
            , pBounty->mob->vnum
            , mob_index_name( NULL, pBounty->mob )
            , pBounty->room
            , pBounty->chance
            , pBounty->count
            , pBounty->max
            , bounty_type( pBounty->type )
            , pBounty->value
            , pBounty->migration
            , pBounty->occurred
            , pBounty->killed
            , YESNO( pBounty->lock )
            , pBounty->msg );

          RETURN_NULL();
        }
      }

      send_to_char( "�藍�_�M�S�����Ӷ��Ǫ���ơT\n\r", ch );
    }

    RETURN_NULL();
  }

  else
  {
    send_to_char( "�A���y�k���~�M�Ьd�� bounty �����T�ϥΤ�k�T\n\r", ch );
  }

  RETURN_NULL();
}

void bounty_update( void )
{
  BOUNTY_DATA     * pBounty;
  ROOM_INDEX_DATA * pRoom;
  CHAR_DATA       * pMob;
  char              buf[MAX_STRING_LENGTH];
  char              buf1[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "bounty_update" );

  if ( get_usernum() < BountyLimit ) RETURN_NULL();

  /* �t�έ��m, ���� */
  if ( ( reboot_time   > 0 && ( ( reboot_time   - current_time ) < 300 ) )
    || ( shutdown_time > 0 && ( ( shutdown_time - current_time ) < 300 ) ) )
    RETURN_NULL();

  for ( pBounty = bounty_list; pBounty; pBounty = pBounty->next )
  {
    if ( pBounty->lock
      || pBounty->count >= pBounty->max
      || number_range( 1, 1000 ) > pBounty->chance
      || !( pRoom = random_room( get_room_area( pBounty->room ) ) )
      || !pBounty->mob
      || !pBounty->msg
      || !*pBounty->msg
      || !( pMob = shape_mobile( pBounty->mob, pRoom ) ) ) continue;

    NowBounty++;
    pBounty->count++;
    pBounty->occurred++;
    pMob->bounty = pBounty;
    pMob->gold   = 0;

    chinese_number( pBounty->value, buf1 );
    sprintf( buf, "%s�M�Z�O�i�H����%s\e[1;33m���H�M���i�H���%s%s%s�T"
      , pBounty->msg
      , mob_name( NULL, pMob )
      , bounty_type( pBounty->type )
      , buf1
      , bounty_unit( pBounty->type ) );

    talk_channel_2( buf, CHANNEL_BULLETIN, "�a��" );
  }

  if ( NowBounty <= 0 ) RETURN_NULL();

  for ( pMob = char_list; pMob; pMob = pMob->next )
  {
    if ( !verify_char( pMob )
      || !IS_NPC( pMob )
      || !( pBounty = pMob->bounty )
      || number_range( 1, 100 ) > pBounty->migration
      || !( pRoom = random_room( get_room_area( pBounty->room ) ) )
      || !pMob->in_room
      || pMob->mount_by
      || pMob->mount
      || pMob->boss
      || pMob->talk_buf
      || pMob->position != POS_STANDING ) continue;

    act( "$n�P��X�{�@�ε���M���[�M���٦�$n�����v�O�S"
       , pMob, NULL, NULL, TO_ROOM );

    char_from_room( pMob );
    char_to_room( pMob, pRoom );
    act( "���n�N��M$n�ӭɦ�@�U�C", pMob, NULL, NULL, TO_ROOM );
  }

  RETURN_NULL();
}

void check_bounty( CHAR_DATA * ch, CHAR_DATA * victim )
{
  BOUNTY_DATA * pBounty;
  char          buf[MAX_STRING_LENGTH];
  char          buf1[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "check_bounty" );

  if ( !ch || !victim || !verify_char( ch ) || !verify_char( victim ) )
  {
    mudlog( LOG_DEBUG, "check_bounty: �ӷ������T." );
    RETURN_NULL();
  }

  if ( IS_NPC( ch ) || !IS_NPC( victim ) || !( pBounty = victim->bounty ) )
    RETURN_NULL();

  pBounty->killed++;
  pBounty->count--;
  NowBounty--;
  victim->bounty = NULL;

  switch( pBounty->type )
  {
  default:
    mudlog( LOG_DEBUG, "check_bounty: ���~�����A %d.", pBounty->type );
    RETURN_NULL();

  case BOUNTY_GOLD:

    if ( pBounty->value <= 0 )
    {
      mudlog( LOG_DEBUG, "check_bounty: ���~���ƭ� %d.", pBounty->value );
      RETURN_NULL();
    }

    if ( !over_scale( ch ) ) gold_to_char( ch, pBounty->value );
    else send_to_char( "�藍�_�M�A���`�겣�w�F�W���M�ҥH�S������T\n\r", ch );
    break;

  case BOUNTY_FIRMAN:

    if ( pBounty->value <= 0 )
    {
      mudlog( LOG_DEBUG, "check_bounty: ���~���ƭ� %d.", pBounty->value );
      RETURN_NULL();
    }

    add_firman( ch, pBounty->value );
    break;
  }

  chinese_number( pBounty->value, buf1 );
  sprintf( buf, "�]��%s\e[1;33m(%s)�����F�a�त��%s�M"
    "�ҥH�S�a����%s%s%s�T\e[0m"
    , ch->cname
    , ch->name
    , mob_name( NULL, victim )
    , bounty_type( pBounty->type )
    , buf1
    , bounty_unit( pBounty->type ) );

  talk_channel_2( buf, CHANNEL_BULLETIN, "���i" );

  print_to_char( ch
    , "�]���A�����F�a�त��%s�M�ҥH�S�a����A%s%s%s�T\e[0m\n\r"
    , mob_name( NULL, victim )
    , bounty_type( pBounty->type )
    , buf1
    , bounty_unit( pBounty->type ) );

  RETURN_NULL();
}

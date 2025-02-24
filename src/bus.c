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
#include "merc.h"

int     bus_tick        =       0;
char *  company_name    =       "�T������毸���q";
char *  platform_descr  =
  "  �o�̬O�毸���Ԩ��B�M�Y�O�Q�����Щ󦹵��Աa��\n\r";

char *  loge_descr      =
  "�o�O�T��̤j�������氨���M���ݳo�W�ҴN���D���T��F�@�f�߫�ӧG�m�M" \
  "�J�ٵe�ɡM�]�_�ӬO�p�����ΪA�ֱ��M�O�H�Q�@���A���C\n\r";

char *  platform_short  =       "�Ԩ��B";
char *  loge_short      =       "���[";

BUS_DATA *      bus_next        args( ( BUS_DATA * ) );

FUNCTION( do_bus )
{
  char       arg[MAX_INPUT_LENGTH];
  char       buf[MAX_STRING_LENGTH];
  BUS_DATA * pBus;
  int        count;

  PUSH_FUNCTION( "do_bus" );

  if ( IS_NPC( ch ) || !ch->in_room ) RETURN_NULL();

  if ( !bus_first )
  {
    act( "�藍�_�M$t�S�������毸�C", ch, mud_name, NULL, TO_CHAR );
    RETURN_NULL();
  }

  one_argument( argument, arg );

  /* �S���h�O�d���毸 */
  if ( !arg[0] )
  {
    if ( ( pBus = is_platform( ch->in_room ) ) )
    {
      clear_buffer();

      send_to_buffer( "�����O%s�M�U�@���O%s�C "
        , pBus->name, bus_next( pBus )->name );

      if ( bus_tick > 0 && bus_tick < 100 )
      {
        chinese_number( 100 - bus_tick, buf );
        send_to_buffer( "���C���b%s����}�ʡC\n\r", buf );
      }

      else
      {
        chinese_number( 300 - bus_tick, buf );
        send_to_buffer( "�U�Z���b%s�����F�C\n\r", buf );
      }

      print_buffer( ch );
    }

    else if ( ( pBus = is_loge( ch->in_room ) ) )
    {
      chinese_number( 300 - bus_tick, buf );
      act( "�A���b��$t���C���W�M�w���b$T�����F�C"
        , ch, bus_next( pBus )->name, buf, TO_CHAR );
    }

    else
    {
      clear_buffer();
      send_to_buffer( "%s���C����Ʀp�U�R\n\r"
        "\e[1;33;44m�s    �� ��            �W ��      ��\e[0m\n\r", mud_name );

      for ( count = 0, pBus = bus_first; pBus; pBus = pBus->next )
      {
        send_to_buffer( "�� %2d �� �u\e[1;33m%-12s\e[0m�v "
          "�u$\e[1;32m%5d\e[0m�v\n\r"
          , ++count, pBus->name, pBus->cost );

        if ( buffer_full() ) break;
      }
      print_buffer( ch );
    }

    RETURN_NULL();
  }

  /* �d�߸��| */
  else if ( !str_prefix( arg, "!path" ) && IS_IMMORTAL( ch ) )
  {
    clear_buffer();
    send_to_buffer( "\e[1;33;44m���� ��              �W   �� �� �B   "
      "��    �x   ��    �[  ��    ��  �H    ��\e[0m\n\r" );

    for ( count = 0, pBus = bus_first; pBus; pBus = pBus->next )
    {
      send_to_buffer( " %3d %-16s    %6d     %6d     %6d     %6d   %6d\n\r"
        , ++count , pBus->name, pBus->station->vnum
        , pBus->platform->vnum, pBus->loge->vnum, pBus->cost, pBus->count );

      if ( buffer_full() ) break;
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  send_to_char( "�藍�_�M�Х� bus /? �d�� bus �����T�ϥΤ�k�C\n\r" , ch );
  RETURN_NULL();
}

/* �ˬd�O�_�i�H�O�a�K�W�U�� */
bool check_station( ROOM_INDEX_DATA * pRoom )
{
  PUSH_FUNCTION( "check_station" );
  if ( !pRoom || pRoom->exit[DIR_DOWN] ) RETURN( FALSE );
  RETURN( TRUE );
}

/* �إߦa�K����x */
ROOM_INDEX_DATA * create_platform( int vnum, char * name, AREA_DATA * pArea )
{
  ROOM_INDEX_DATA * pRoom;
  int               iHash;
  int               loop;
  char              buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "create_platform" );

  if ( vnum <= 0 || !pArea ) RETURN( NULL );

  /* ���إ߰_���W */
  sprintf( buf, "%s%s", name, platform_short );

  pRoom = alloc_struct( STRUCT_ROOM_INDEX_DATA );

  pRoom->people          = NULL;
  pRoom->contents        = NULL;
  pRoom->extra_descr     = NULL;
  pRoom->club            = NULL;
  pRoom->job             = NULL;
  pRoom->area            = pArea;
  pRoom->name            = str_dup( buf );
  pRoom->description     = str_dup( platform_descr );
  pRoom->filename        = str_dup( "�t�Ϋإ�" );
  pRoom->vnum            = vnum;
  pRoom->Safe            = TRUE;
  pRoom->NoRecall        = TRUE;
  pRoom->NoWhere         = TRUE;
  pRoom->NoQuit          = TRUE;
  pRoom->NoMob           = TRUE;
  pRoom->NoLink          = TRUE;
  pRoom->light           = 0;
  pRoom->sector          = DefaultSector;
  iHash                  = pRoom->vnum % MAX_KEY_HASH;
  pRoom->next            = room_index_hash[iHash];
  room_index_hash[iHash] = pRoom;
  top_room++;

  for ( loop = 0; loop < DIR_MAX; loop++ ) pRoom->exit[loop] = NULL;
  RETURN( pRoom );
}

/* �إߦa�K�����[ */
ROOM_INDEX_DATA * create_loge( int vnum , char * name , AREA_DATA * pArea )
{
  ROOM_INDEX_DATA * pRoom;
  int               iHash;
  int               loop;
  char              buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "create_loge" );

  if ( vnum <= 0 || !pArea ) RETURN( NULL );

  /* ���إ߰_�W�� */
  sprintf( buf, "%s%s", name, loge_short );

  pRoom = alloc_struct( STRUCT_ROOM_INDEX_DATA );

  pRoom->people          = NULL;
  pRoom->contents        = NULL;
  pRoom->extra_descr     = NULL;
  pRoom->area            = pArea;
  pRoom->name            = str_dup( buf );
  pRoom->description     = str_dup( loge_descr );
  pRoom->filename        = str_dup( "�t�Ϋإ�" );
  pRoom->vnum            = vnum;
  pRoom->Safe            = TRUE;
  pRoom->NoRecall        = TRUE;
  pRoom->NoWhere         = TRUE;
  pRoom->NoQuit          = TRUE;
  pRoom->NoMob           = TRUE;
  pRoom->NoLink          = TRUE;
  pRoom->light           = 0;
  pRoom->sector          = DefaultSector;
  iHash                  = pRoom->vnum % MAX_KEY_HASH;
  pRoom->next            = room_index_hash[iHash];
  room_index_hash[iHash] = pRoom;
  top_room++;

  for ( loop = 0; loop < DIR_MAX; loop++ ) pRoom->exit[loop] = NULL;
  RETURN( pRoom );
}

/* �O�_���a�K����x�M�άO�]�[�M�o�ǬO�t�Ϋإ߰_�Ӫ��M��ڤ��s�b�� */
BUS_DATA * is_station( ROOM_INDEX_DATA * pRoom )
{
  BUS_DATA * pBus;

  PUSH_FUNCTION( "is_station" );

  if ( !bus_first || !pRoom ) RETURN( NULL );

  for ( pBus = bus_first; pBus; pBus = pBus->next )
    if ( pBus->platform == pRoom || pBus->loge == pRoom ) RETURN( pBus );

  RETURN( NULL );
}

/* �O�_����x */
BUS_DATA * is_platform( ROOM_INDEX_DATA * pRoom )
{
  BUS_DATA * pBus;

  PUSH_FUNCTION( "is_platform" );

  if ( !bus_first || !pRoom ) RETURN( NULL );

  for ( pBus = bus_first; pBus; pBus = pBus->next )
    if ( pBus->platform == pRoom ) RETURN( pBus );

  RETURN( NULL );
}

/* �O�_�����[ */
BUS_DATA * is_loge( ROOM_INDEX_DATA * pRoom )
{
  BUS_DATA * pBus;

  PUSH_FUNCTION( "is_loge" );

  if ( !bus_first || !pRoom ) RETURN( NULL );

  for ( pBus = bus_first; pBus; pBus = pBus->next )
    if ( pBus->loge == pRoom ) RETURN( pBus );

  RETURN( NULL );
}

/* �B�z�O�_�b�a�K���s�� */
int save_room( ROOM_INDEX_DATA * pRoom )
{
  BUS_DATA        * pBus;

  PUSH_FUNCTION( "save_room" );

  if ( ( pBus = is_station( pRoom ) ) ) RETURN( pBus->station->vnum )
  RETURN( pRoom->vnum );
}

/* �W�U�s���������u */
bool link_path( ROOM_INDEX_DATA * aRoom, ROOM_INDEX_DATA * bRoom )
{
  EXIT_DATA * aExit;
  EXIT_DATA * bExit;

  PUSH_FUNCTION( "link_path" );

  /* �t�m�O���� */
  set_exit_default( aExit = alloc_struct( STRUCT_EXIT_DATA ) );
  set_exit_default( bExit = alloc_struct( STRUCT_EXIT_DATA ) );

  if ( !aRoom || !bRoom || !aExit || !bExit ) RETURN( FALSE );

  /* �M���򥻭� */
  aExit->description = bExit->description = str_dup( "" );
  aExit->keyword     = bExit->keyword     = str_dup( "" );
  aExit->closed      = bExit->closed      = FALSE;
  aExit->isdoor      = bExit->isdoor      = FALSE;
  aExit->locked      = bExit->locked      = FALSE;
  aExit->pick        = bExit->pick        = FALSE;
  aExit->key         = bExit->key         = 0;

  aExit->message     = str_dup( "�A���i�F�毸�̭��T" );
  bExit->message     = str_dup( "�A�q�毸���F�X�ӡT" );

  /* �إ߳s�� */
  aExit->vnum = bRoom->vnum;
  bExit->vnum = aRoom->vnum;

  aRoom->exit[DIR_DOWN] = aExit;
  bRoom->exit[DIR_UP  ] = bExit;

  RETURN( TRUE );
}

void bus_update( void )
{
  BUS_DATA        * pBus;
  CHAR_DATA       * ch;
  CHAR_DATA       * ch_next;
  ROOM_INDEX_DATA * aRoom;
  ROOM_INDEX_DATA * bRoom;
  char              buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "bus_update" );

  /* �p�G�t�ΨS�����J�����毸�t�� */
  if ( !bus_first ) RETURN_NULL();

  /* �p�� */
  if ( ++bus_tick > 300 ) bus_tick = 0;

  switch( bus_tick )
  {
  /* �e�X�쯸�T�� */
  case 0:

    for ( pBus = bus_first; pBus; pBus = pBus->next )
    {
      sprintf( buf,
        "\e[1;33m%s����F�M�Эn�U�����ȫȦb���U���M"
        "�X�f��V�b�W��\e[0m�C\n\r %s �ܸ��w��A���f���C\n\r"
        , bus_next( pBus )->name , company_name );
      sendmsg_to_someroom( buf, pBus->loge );
    }

    for ( pBus = bus_first; pBus; pBus = pBus->next )
    {
      str_cpy( buf,
        "\e[1;33m�C���i���F�M�Фj�a���n�a�b�K����M"
        "�H���M�I�C\e[0m\n\r" );

      sendmsg_to_someroom( buf, bus_next( pBus )->platform );
    }

    break;

  /* �h���ȫ� */
  case 1:

    for ( pBus = bus_first; pBus; pBus = pBus->next )
    {
      /* �p����e���M���᪺�ж� */
      aRoom = pBus->loge;
      bRoom = bus_next( pBus )->platform;

      for ( ch = aRoom->people; ch; ch = ch_next )
      {
        ch_next = ch->next_in_room;

        if ( ch->fighting ) stop_fighting( ch, TRUE );

        if ( ch->position == POS_RESTING || ch->position == POS_SLEEPING )
        {
          send_to_char( "�ޡM�C���쯸�F�M�Ӱ_�ɤF�C\n\r" , ch );

          /* �p�G�O���� */
          if ( !is_affected( ch, SLOT_SLEEP ) ) ch->position = POS_STANDING;
        }

        act( "$n�q�o�Z�C�����F�X�h�C", ch, NULL, NULL, TO_ALL );

        char_from_room( ch );
        char_to_room( ch, bRoom );
        act( "$n�q�o�Z�C���W���F�X�ӡC", ch, NULL, NULL, TO_ROOM );
        do_look( ch, "auto" );

        /* �M���l�ܬ����I */
        clear_trace( ch, TRUE );
      }
    }
    break;

  /* �e�X�Y�N�}�����T�� */
  case 20:
  case 80:

    for ( pBus = bus_first; pBus; pBus = pBus->next )
    {
      sprintf( buf,
        "���C���Y�N�}��%s�M�n�U�����ȫȽо��t�ѤW��X�f���}�C\n\r"
        "�Ԩ����ȫȽеy�����赥�Զ}���M%s�ܸ��w��A�����{�C\n\r"
        , bus_next( pBus)->name , company_name );

      sendmsg_to_someroom( buf, pBus->platform );
    }
    break;

  /* �}�����T�� */
  case 99:

    for ( pBus = bus_first; pBus; pBus = pBus->next )
    {
      sprintf( buf, "\e[1;33m���C�����W�n�X�o�M�Юȫȭ̵y�@�𮧡C\e[0m\n\r"
        "�Y�����W�����ȫȡM�кɳt�ѤW�����}�C\n\r"
        "���C���o����Y�N�}��%s�M%s�ܸ��w��A�����{�C\n\r"
        , bus_next( pBus )->name , company_name );

      sendmsg_to_someroom( buf, pBus->loge );
    }
    break;


  /* �h���ȫ� */
  case 100:

    /* �t�έ��m, �a�K���� */
    if ( ( reboot_time   > 0 && ( ( reboot_time   - current_time ) < 300 ) )
      || ( shutdown_time > 0 && ( ( shutdown_time - current_time ) < 300 ) ) )
      break;

    for ( pBus = bus_first; pBus; pBus = pBus->next )
    {
      /* �p����e���M���᪺�ж� */
      aRoom = pBus->platform;
      bRoom = pBus->loge;

      for ( ch = aRoom->people; ch; ch = ch_next )
      {
        ch_next = ch->next_in_room;

        /* �Y�O�Ǫ��άO�����������h */
        if ( IS_NPC( ch )
          || ch->gold < pBus->cost
          || ch->position != POS_STANDING )
        {
          if ( ch->gold < pBus->cost )
            send_to_char( "�A���������M�L�k�f�����C���C\n\r" , ch );
          continue;
        }

        /* ���� */
        gold_from_char( ch, pBus->cost );
        pBus->count++;

        /* �h���H�� */
        act( "$n�I�F���M�f�W�F�a�K�C", ch, NULL, NULL, TO_ALL );
        char_from_room( ch );
        char_to_room( ch, bRoom );
        act( "$n�I�F���M���W�F�o�Z�a�K�C", ch, NULL, NULL, TO_ROOM );

        do_look( ch, "auto" );

        /* �M���l�ܬ����I */
        clear_trace( ch, TRUE );
      }
    }
    break;

  /* �}�����T�� */
  case 101:

    for ( pBus = bus_first; pBus; pBus = pBus->next )
    {
      sprintf( buf, "\e[1;33m���C���w�g�X�o�M�Юȫȭ̵y�@�𮧡C\e[0m\n\r"
        "���C���Y�N�}��%s�C %s�ܸ��w��A�����{�C\n\r"
        , bus_next( pBus )->name , company_name );

      sendmsg_to_someroom( buf, pBus->loge );
      sendmsg_to_someroom( "\e[1;33m�C���w�g�X�o�M�������̡M��"
        "�y�ݤU�@�Z�C\e[0m\n\r", pBus->platform );
    }

    break;

  /* �e�X�Y�N�쯸���T�� */
  case 150:
  case 200:
  case 250:

    for ( pBus = bus_first; pBus; pBus = pBus->next )
    {
      sprintf( buf, "���C�����}���U�@��%s�M�Юȫȵy�@�𮧡M�@�ߵ��ԡC\n\r"
        "%s�ܸ��w��A�����{�C\n\r"
        , bus_next( pBus )->name , company_name );
      sendmsg_to_someroom( buf, pBus->loge );
    }

    break;
  }

  RETURN_NULL();
}

/* �U�@�� */
BUS_DATA * bus_next( BUS_DATA * pBus )
{
  PUSH_FUNCTION( "bus_next" );

  if ( !pBus )      RETURN( NULL );
  if ( pBus->next ) RETURN( pBus->next );
  RETURN( bus_first );
}

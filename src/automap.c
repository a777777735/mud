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

char *          show_a_trace    args( ( TRACE_DATA * ) );

FUNCTION( do_automap )
{
  char              arg[MAX_INPUT_LENGTH];
  char              buf[MAX_STRING_LENGTH];
  PC_DATA         * pc;
  ROOM_INDEX_DATA * pRoom;
  ROOM_INDEX_DATA * zRoom;
  ROOM_INDEX_DATA * aRoom;
  AREA_DATA       * aStart;
  AREA_DATA       * aEnd;
  TRACE_DATA      * pTrace;
  DESCRIPTOR_DATA * pDesc;
  int               loop;
  bool              HasExit;

  PUSH_FUNCTION( "do_automap" );

  if ( IS_NPC( ch ) || !( pc = ch->pcdata ) || !( pRoom = ch->in_room ) )
    RETURN_NULL();

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    if ( !( pTrace = pc->trace ) )
    {
      send_to_char( "�藍�_�M�A�ثe�S����w�������|�T\n\r", ch );
      RETURN_NULL();
    }

    print_to_char( ch, "�A�ثe��w�����|��Ƭ��R\n\r%s%s%s"
      , VERTICAL_LINE, show_a_trace( pTrace ) , VERTICAL_LINE );
  }

  else if ( arg[0] == '!' )
  {
    if ( arg[1] == '\x0' )
    {
      send_to_char( "�е����A���X�������S\n\r", ch );
      RETURN_NULL();
    }

    loop = atoi( arg + 1 ) - 1;

    if ( loop < 0 || loop >= MAX_TRACE )
    {
      send_to_char( "�藍�_�M�A�����X���X�k�T\n\r", ch );
      RETURN_NULL();
    }

    if ( !( pTrace = pc->map[loop] ) )
    {
      send_to_char( "�藍�_�M�A���Ӭ����O�Ū��T\n\r", ch );
      RETURN_NULL();
    }

    if ( pTrace->ending != pRoom->vnum )
    {
      send_to_char( "�藍�_�M�ثe���O�b���Ӳ��I�M�ҥH�L�k�ҰʡT\n\r", ch );
      RETURN_NULL();
    }

    if ( !( pDesc = ch->desc ) )
    {
      send_to_char( "���H�L�y�z��M�L�k�ҰʡT\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->position != POS_STANDING )
    {
      send_to_char( "�A�٬O�������A���䪺�F��a�T\n\r", ch );
      RETURN_NULL();
    }

    if ( is_pk( ch ) )
    {
      send_to_char( "�A�٬O�M�ߧA���Z���j�|�a�T\n\r", ch );
      RETURN_NULL();
    }

    for ( buf[0] = '\x0', loop = MAX_PATH - 1 ; loop >= 0; loop-- )
    {
      if ( pTrace->path[loop] == ERRORCODE ) continue;

      switch( pTrace->path[loop] )
      {
      case DIR_NORTH:
        str_cat( buf, "s\n\r" );
        break;

      case DIR_SOUTH:
        str_cat( buf, "n\n\r" );
        break;

      case DIR_EAST:
        str_cat( buf, "w\n\r" );
        break;

      case DIR_WEST:
        str_cat( buf, "e\n\r" );
        break;

      case DIR_DOWN:
        str_cat( buf, "u\n\r" );
        break;

      case DIR_UP:
        str_cat( buf, "d\n\r" );
        break;

      case DIR_ENTER:
        str_cat( buf, "ou\n\r" );
        break;

      case DIR_OUT:
        str_cat( buf, "en\n\r" );
        break;
      }
    }

    if ( buf[0] == '\x0' )
    {
      send_to_char( "�L�Ī������T\n\r", ch );
      RETURN_NULL();
    }

    buf[ UMAX( 0, sizeof( pDesc->inbuf ) - 100 ) ] = '\x0';

    send_to_char( "�Ұʬ������|�Ҧ��T\n\r", ch );
    str_cpy( pDesc->inbuf , buf );
    RETURN_NULL();
  }

  else if ( is_number( arg ) )
  {
    loop = atoi( arg ) - 1;

    if ( loop < 0 || loop >= MAX_TRACE )
    {
      send_to_char( "�藍�_�M�A�����X���X�k�T\n\r", ch );
      RETURN_NULL();
    }

    if ( !( pTrace = pc->map[loop] ) )
    {
      send_to_char( "�藍�_�M�A���Ӭ����O�Ū��T\n\r", ch );
      RETURN_NULL();
    }

    if ( pTrace->starting != pRoom->vnum )
    {
      send_to_char( "�藍�_�M�ثe���O�b���Ӱ_�l�I�M�ҥH�L�k�ҰʡT\n\r", ch );
      RETURN_NULL();
    }

    if ( !( pDesc = ch->desc ) )
    {
      send_to_char( "���H�L�y�z��M�L�k�ҰʡT\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->position != POS_STANDING )
    {
      send_to_char( "�A�٬O�������A���䪺�F��a�T\n\r", ch );
      RETURN_NULL();
    }

    if ( is_pk( ch ) )
    {
      send_to_char( "�A�٬O�M�ߧA���Z���j�|�a�T\n\r", ch );
      RETURN_NULL();
    }

    for ( buf[0] = '\x0', loop = 0; loop < MAX_PATH; loop++ )
    {
      if ( pTrace->path[loop] == ERRORCODE ) break;

      switch( pTrace->path[loop] )
      {
      case DIR_NORTH:
        str_cat( buf, "n\n\r" );
        break;

      case DIR_SOUTH:
        str_cat( buf, "s\n\r" );
        break;

      case DIR_EAST:
        str_cat( buf, "e\n\r" );
        break;

      case DIR_WEST:
        str_cat( buf, "w\n\r" );
        break;

      case DIR_DOWN:
        str_cat( buf, "e\n\r" );
        break;

      case DIR_UP:
        str_cat( buf, "u\n\r" );
        break;

      case DIR_ENTER:
        str_cat( buf, "en\n\r" );
        break;

      case DIR_OUT:
        str_cat( buf, "ou\n\r" );
        break;
      }
    }

    if ( buf[0] == '\x0' )
    {
      send_to_char( "�L�Ī������T\n\r", ch );
      RETURN_NULL();
    }

    buf[ UMAX( 0, sizeof( pDesc->inbuf ) - 100 ) ] = '\x0';

    send_to_char( "�Ұʬ������|�Ҧ��T\n\r", ch );
    str_cpy( pDesc->inbuf , buf );
  }

  else if ( !str_prefix( arg, "lock" ) )
  {
    if ( ch->position != POS_STANDING
      || is_dead( ch )
      || ch->failed > 0
      || ch->jail > 0 )
    {
      send_to_char( "�A�ثe�����A���A�X�l�ܸ��|�T\n\r", ch );
      RETURN_NULL();
    }

    if ( is_pk( ch ) )
    {
      send_to_char( "�A�٬O�M�ߪ��Z���j�|�a�T\n\r", ch );
      RETURN_NULL();
    }

    if ( is_station( pRoom ) || pRoom->Sail )
    {
      send_to_char( "�b�o�̬O�L�k�����l�ܸ��|����T\n\r", ch );
      RETURN_NULL();
    }

    for ( HasExit = FALSE, loop = 0; loop < DIR_MAX; loop++ )
    {
      if ( pRoom->exit[loop] )
      {
        HasExit = TRUE;
        break;
      }
    }

    if ( !HasExit )
    {
      send_to_char( "�藍�_�M�o�̨S���X�f�M�ҥH�L�k�l�ܸ��|�T\n\r", ch );
      RETURN_NULL();
    }

    if ( !( pTrace = pc->trace ) )
    {
      pTrace     = alloc_struct( STRUCT_TRACE_DATA );
      pc->trace  = pTrace;
    }

    pTrace->starting = pRoom->vnum;
    pTrace->ending   = ERRORCODE;
    for ( loop = 0; loop < MAX_PATH; loop++ ) pTrace->path[loop] = ERRORCODE;

    act( "�A��w�F$2$t$0���������_�I�T", ch, pRoom->name, NULL, TO_CHAR );
  }

  else if ( !str_prefix( arg, "clear" ) )
  {
    if ( !( pTrace = pc->trace ) )
    {
      send_to_char( "�藍�_�M�A�������o����w�~��M���T\n\r", ch );
      RETURN_NULL();
    }

    free_struct( pc->trace, STRUCT_TRACE_DATA );
    pc->trace = NULL;
    send_to_char( "�A�w�g�M�����e����w�l�ܡT\n\r", ch );
  }

  else if ( !str_prefix( arg, "delete" ) )
  {
    if ( argument[0] == '\x0' || !is_number( argument ) )
    {
      send_to_char( "�A�n�R�����@�Ӱl�ܸ��|�����S\n\r", ch );
      RETURN_NULL();
    }

    loop = atoi( argument ) - 1;

    if ( loop < 0 || loop >= MAX_TRACE )
    {
      send_to_char( "�藍�_�M�A�����X���X�k�T\n\r", ch );
      RETURN_NULL();
    }

    if ( !pc->map[loop] )
    {
      send_to_char( "�藍�_�M�A���Ӭ����O�Ū��T\n\r", ch );
      RETURN_NULL();
    }

    free_struct( pc->map[loop], STRUCT_TRACE_DATA );
    pc->map[loop] = NULL;

    chinese_number( loop + 1, buf );
    act( "�A�R���F��$t�������|�����T\n\r", ch, buf, NULL, TO_CHAR );
  }

  else if ( !str_prefix( arg, "show" ) )
  {
    if ( argument[0] == '\x0' || !is_number( argument ) )
    {
      send_to_char( "�A�n��ܭ��@�Ӱl�ܸ��|�����S\n\r", ch );
      RETURN_NULL();
    }

    loop = atoi( argument ) - 1;

    if ( loop < 0 || loop >= MAX_TRACE )
    {
      send_to_char( "�藍�_�M�A�����X���X�k�T\n\r", ch );
      RETURN_NULL();
    }

    if ( !pc->map[loop] )
    {
      send_to_char( "�藍�_�M�A���Ӭ����O�Ū��T\n\r", ch );
      RETURN_NULL();
    }

    chinese_number( loop + 1, buf );
    print_to_char( ch, "�A��%s�����|��Ƭ��R\n\r%s%s%s"
      , buf , VERTICAL_LINE, show_a_trace( pc->map[loop] ) , VERTICAL_LINE );
  }

  else if ( !str_prefix( arg, "list" ) )
  {
    clear_buffer();

    for ( loop = 0; loop < MAX_TRACE; loop++ )
    {
      chinese_number( loop + 1, buf );

      if ( pc->map[loop] )
      {
        aRoom = get_room_index( pc->map[loop]->starting );
        zRoom = get_room_index( pc->map[loop]->ending   );

        aStart = aRoom ? aRoom->area : NULL;
        aEnd   = zRoom ? zRoom->area : NULL;

        send_to_buffer( "%s�B%s��%s\e[0m �� %s��%s\e[0m\n\r"
          , buf
          , aStart ? aStart->name : "�Y�����W�ϰ�"
          , aRoom  ? aRoom->name  : "�Y�����W�a��"
          , aEnd   ? aEnd->name   : "�Y�����W�ϰ�"
          , zRoom  ? zRoom->name  : "�Y�����W�a��" );
      }

      else
      {
        send_to_buffer( "%s�B�S�������T\n\r", buf );
      }

      if ( buffer_full() ) break;
    }

    print_buffer( ch );
  }

  else if ( !str_prefix( arg, "dump" ) )
  {
    if ( !( pTrace = pc->trace ) )
    {
      send_to_char( "�藍�_�M�A�������o����w�~��g�J�T\n\r", ch );
      RETURN_NULL();
    }

    if ( !( zRoom = get_room_index( pTrace->starting ) ) )
    {
      send_to_char( "�A�������_�I�w�g�����F�T\n\r", ch );
      RETURN_NULL();
    }

    if ( zRoom == pRoom )
    {
      send_to_char( "�A���_�l�I�M���I�ڥ��N�@�ˡM���ݭn�����T\n\r", ch );
      RETURN_NULL();
    }

    if ( pTrace->path[0] == ERRORCODE )
    {
      send_to_char( "�A�����|�̮ڥ��S�������M�ʧ@�����T\n\r", ch );
      RETURN_NULL();
    }

    if ( argument[0] == '\x0' || !is_number( argument ) )
    {
      send_to_char( "�A�n��l�ܬ�������@�Ӹ��X�̡S\n\r", ch );
      RETURN_NULL();
    }

    loop = atoi( argument ) - 1;

    if ( loop < 0 || loop >= MAX_TRACE )
    {
      send_to_char( "�藍�_�M�A����X���X���X�k�T\n\r", ch );
      RETURN_NULL();
    }

    if ( pc->map[loop] )
    {
      free_struct( pc->map[loop], STRUCT_TRACE_DATA );
      pc->map[loop] = NULL;
    }

     pTrace->ending = pRoom->vnum;
     pc->map[loop]  = pTrace;
     pc->trace      = NULL;

     chinese_number( loop + 1, buf );
     print_to_char( ch, "�A��%s\e[0m��%s\e[0m�����|�������%s�������̡T\n\r"
       , zRoom->name, pRoom->name, buf );

     RETURN_NULL();
  }

  else
  {
    send_to_char( "�A���Ѽƿ��~�M�Ьd�� automap ���Ϊk�T\n\r", ch );
  }

  RETURN_NULL();
}

void clear_trace( CHAR_DATA * ch, bool bShow )
{
  TRACE_DATA * pTrace;

  PUSH_FUNCTION( "clear_trace" );

  if ( !ch || !ch->pcdata || !( pTrace = ch->pcdata->trace ) ) RETURN_NULL();

  free_struct( pTrace, STRUCT_TRACE_DATA );
  ch->pcdata->trace = NULL;

  if ( bShow ) send_to_char( "�A�������|���I�޲��M�t�εL�k�l�ܡT\n\r", ch );
  RETURN_NULL();
}

void set_map( CHAR_DATA * ch, int dir )
{
  TRACE_DATA * pTrace;
  int          loop;

  PUSH_FUNCTION( "set_map" );

  if ( !ch || !ch->pcdata || !( pTrace = ch->pcdata->trace ) ) RETURN_NULL();

  for ( loop = 0; loop < MAX_PATH; loop++ )
  {
    if ( pTrace->path[loop] == ERRORCODE )
    {
      switch( dir )
      {
      default:
        break;

      case DIR_NORTH:
      case DIR_EAST:
      case DIR_SOUTH:
      case DIR_WEST:
      case DIR_UP:
      case DIR_DOWN:
      case DIR_ENTER:
      case DIR_OUT:
        pTrace->path[loop] = dir;
        break;
      }

      RETURN_NULL();
    }
  }

  send_to_char( "�A���l�ܬ����I�w���M�L�k�~������T\n\r", ch );
  RETURN_NULL();
}

char * show_a_trace( TRACE_DATA * pTrace )
{
  int               loop;
  bool              bAttach;
  static char       buf[MAX_STRING_LENGTH];
  ROOM_INDEX_DATA * pStart;
  ROOM_INDEX_DATA * pEnd;
  AREA_DATA       * aStart;
  AREA_DATA       * aEnd;

  PUSH_FUNCTION( "show_a_trace" );

  if ( !pTrace )
  {
    mudlog( LOG_DEBUG, "show_a_trace: �ʥF�ӷ�." );
    RETURN( "" );
  }

  pStart = get_room_index( pTrace->starting );
  pEnd   = get_room_index( pTrace->ending   );

  aStart = pStart ? pStart->area : NULL;
  aEnd   = pEnd   ? pEnd->area   : NULL;

  sprintf( buf, "%s%s\e[0m��%s\e[0m��%s%s\e[0m��%s\e[0m\n\r���|�R"
    , ""
    , aStart ? aStart->name : "�����W�ϰ�"
    , pStart ? pStart->name : "�����W�a��"
    , ""
    , aEnd   ? aEnd->name   : "�����W�ϰ�"
    , pEnd   ? pEnd->name   : "�����W�a��" );

  for ( bAttach = FALSE, loop = 0; loop < MAX_PATH; loop++ )
  {
    if ( pTrace->path[loop] == ERRORCODE ) break;

    str_cat( buf, " " );
    str_cat( buf, direction_name( pTrace->path[loop] ) );

    bAttach = TRUE;
  }

  if ( !bAttach ) str_cat( buf, "�L" );
  str_cat( buf, "\n\r" );
  RETURN( buf );
}

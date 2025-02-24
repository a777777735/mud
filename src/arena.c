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

int             pk_age          = PK_AGE;
int             pk_level        = PK_LEVEL;
int             pk_limit        = 10;           /* �W�u�Q�H�H�W�~�|�}�� */
int             PkContrastLevel = PK_CONTRAST_LEVEL;
int             MinPKGold       = MIN_PK_GOLD;
int             MaxPKGold       = MAX_PK_GOLD;
int             MaxPKTotal      = MAX_PK_TOTAL;
int             PKBetTotal      = 0;

extern void       check_contraband args( ( CHAR_DATA * ) );

ROOM_INDEX_DATA * random_pk_room  args( ( CHAR_DATA * ch ) );
void              check_contraband args( ( CHAR_DATA * ch ) );
void              set_pk          args( ( CHAR_DATA *, CLUB_DATA * ) );
void              cancel_pk       args( ( void ) );
int               pk_ante_total   args( ( int ) );
void              clear_ante      args( ( void ) );

void init_fight( FIGHT_DATA * pFight )
{
  int         count;
  AREA_DATA * pArea;
  AREA_DATA * zArea;

  PUSH_FUNCTION( "init_fight" );

  count = 0;
  zArea = NULL;

  for ( pArea = area_first; pArea; pArea = pArea->next )
    if (  pArea->pk && number_range( 0, count++ ) == 0 ) zArea = pArea;

  pFight->fight_1 = NULL;
  pFight->fight_2 = NULL;
  pFight->club_1  = NULL;
  pFight->club_2  = NULL;
  pFight->area    = zArea;
  pFight->status  = PK_JOIN;
  pFight->type    = PK_NODEFINE;

  RETURN_NULL();
}

FUNCTION( do_fight )
{
  int               which;
  int               amount;
  int               total;
  char              arg[MAX_INPUT_LENGTH];
  char              buf[MAX_STRING_LENGTH];
  ROOM_INDEX_DATA * pRoom;
  CHAR_DATA       * victim;
  CHAR_DATA       * fight_1;
  CHAR_DATA       * fight_2;

  PUSH_FUNCTION( "do_fight" );

  if ( !ch || !ch->in_room || IS_NPC( ch ) || !ch->pcdata )
    RETURN_NULL();

  if ( !fight_info->area )
  {
    act( "$t���S�������v�޳��T", ch, mud_name, NULL, TO_CHAR );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    if ( fight_info->lock )
    {
      send_to_char( "�藍�_�M�ثe�Z���j�|�O�������T\n\r", ch );
      RETURN_NULL();
    }

    clear_buffer();

    send_to_buffer( "�ثe�Z���j�|�����p�O�R" );

    switch( fight_info->status )
    {
    case PK_JOIN:
      send_to_buffer( "�������W���T\n\r" );

      switch( fight_info->type )
      {
      case PK_PERSON:

        send_to_buffer( "�s���@�M�ӤH�ɥҤ�B" );

        if ( ( victim = fight_info->fight_1 ) )
        {
          send_to_buffer( "%s\n\r", mob_name( NULL, victim ) );
          send_to_buffer( "��Ĺ���ơR �uĹ�v%4d �u��v%4d\n\r"
            , victim->pcdata ? victim->pcdata->fight_win  : 0
            , victim->pcdata ? victim->pcdata->fight_lost : 0 );
          send_to_buffer( "�ثe���šR%3d ��\n\r", victim->level );
        }

        else
        {
          send_to_buffer( "�q��\n\r" );
        }

        send_to_buffer( "�s���G�M�ӤH�ɤA��B" );

        if ( ( victim = fight_info->fight_2 )  )
        {
          send_to_buffer( "%s\n\r", mob_name( NULL, victim ) );

          send_to_buffer( "��Ĺ���ơR �uĹ�v%4d �u��v%4d\n\r"
            , victim->pcdata ? victim->pcdata->fight_win  : 0
            , victim->pcdata ? victim->pcdata->fight_lost : 0 );

          send_to_buffer( "�ثe���šR%3d ��\n\r", victim->level );
        }

        else
        {
          send_to_buffer( "�q��\n\r" );
        }

        break;

      case PK_CLUB:
        break;

      default:
        send_to_buffer( "�|��������H���W�T\n\r" );
        break;
      }
      break;

    case PK_BET:

      send_to_buffer( "�����~���U�`���T\n\r" );

      if ( ( victim = fight_info->fight_1 ) )
      {
        send_to_buffer( "�s���@�B%s�`����R %d ������C\n\r"
          , mob_name( NULL, victim ), pk_ante_total( PK_FIRST ) );

        send_to_buffer( "�ۤv��%s�R %d ������C\n\r"
          , mob_name( NULL, victim ), ch->ante[PK_FIRST] );

        send_to_buffer( "��Ĺ�O���R �uĹ�v%4d �u��v%4d\n\r"
          , victim->pcdata ? victim->pcdata->fight_win  : 0
          , victim->pcdata ? victim->pcdata->fight_lost : 0 );

        send_to_buffer( "�ثe���šR%3d ��\n\r", victim->level );
      }

      if ( ( victim = fight_info->fight_2 ) )
      {
        send_to_buffer( "\n\r�s���G�B%s�`����R %d ������C\n\r"
          , mob_name( NULL, victim ), pk_ante_total( PK_SECOND ) );

        send_to_buffer( "�ۤv��%s�R %d ������C\n\r"
          , mob_name( NULL, victim ), ch->ante[PK_SECOND] );

        send_to_buffer( "��Ĺ�O���R �uĹ�v%4d �u��v%4d\n\r"
          , victim->pcdata ? victim->pcdata->fight_win  : 0
          , victim->pcdata ? victim->pcdata->fight_lost : 0 );

        send_to_buffer( "�ثe���šR%3d ��\n\r", victim->level );
      }

      break;

    case PK_FIGHT:
      send_to_buffer( "�Z���j�|�i�椤�M�ѤU�C����M���T\n\r" );

      if ( ( fight_1 = fight_info->fight_1 )
        && ( fight_2 = fight_info->fight_2 ) )
      {
        send_to_buffer(
          "�s���@���R%s\e[0m(%s)\n\r"
          "�ثe���A�R%s\n\r"
          "�`����B�R%d �����\n\r"
          "�ۤv��`�R%d �����\n\r"
          "�ӱѰO���R%4d�u�ӡv�R%4d�u�ѡv\n\r"
          "�s���G���R%s\e[0m(%s)\n\r"
          "�ثe���A�R%s\n\r"
          "�`����B�R%d �����\n\r"
          "�ۤv��`�R%d �����\n\r"
          "�ӱѰO���R%4d�u�ӡv�R%4d�u�ѡv\n\r"
          , fight_1->cname
          , fight_1->name
          , status_message( fight_1 )
          , pk_ante_total( PK_FIRST )
          , ch->ante[PK_FIRST]
          , fight_1->pcdata ? fight_1->pcdata->fight_win  : 0
          , fight_1->pcdata ? fight_1->pcdata->fight_lost : 0
          , fight_2->cname
          , fight_2->name
          , status_message( fight_2 )
          , pk_ante_total( PK_SECOND )
          , ch->ante[PK_SECOND]
          , fight_2->pcdata ? fight_2->pcdata->fight_win  : 0
          , fight_2->pcdata ? fight_2->pcdata->fight_lost : 0 );
      }

      break;

    case PK_STOP:
      send_to_buffer( "�M�z�Z���j�|�{�����T\n\r" );
      break;

    case PK_DESERT:
      send_to_buffer( "�Z���j�|���m���T\n\r" );
      break;
    }

    print_buffer( ch );
  }

  else if ( !str_prefix( arg, "join" ) )
  {
    if ( ( reboot_time   > 0 && ( ( reboot_time   - current_time ) < 900 ) )
      || ( shutdown_time > 0 && ( ( shutdown_time - current_time ) < 900 ) ) )
    {
      act( "$t�Y�N�����M�ҥH�S����k�}�ҪZ���j�|�C", ch, mud_name, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( IS_IMMORTAL( ch ) )
    {
      send_to_char( "�A�u��[�[�o�M����ѥ[���աT\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->level <= pk_level )
    {
      chinese_number( pk_level, buf );
      act( "�藍�_�M�A�����ť����W�L$t�Ť~��ѥ[�Z���j�|�T", ch, buf, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( get_age( ch ) <= pk_age )
    {
      chinese_number( pk_age, buf );
      act( "�藍�_�M�A���~�֥����W�L$t���~��ѥ[�Z���j�|�T", ch, buf, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( fight_info->lock )
    {
      send_to_char( "�ثe�Z���j�|�Q��w�F�M�ҥH�֤]�L�k�ѥ[�T\n\r", ch );
      RETURN_NULL();
    }

    if ( fight_info->status != PK_JOIN )
    {
      send_to_char( "�藍�_�M�ثe���O���W���ɭԡT\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->master || ch->leader )
    {
      send_to_char( " �藍�_�M�A�����H�O�H�T\n\r", ch );
      RETURN_NULL();
    }

    if ( auction_info->seller && auction_info->seller == ch )
    {
      send_to_char( "�藍�_�M���A�槹�F��A�ӳ��W�a�T\n\r", ch );
      RETURN_NULL();
    }

    if ( auction_info->buyer && auction_info->buyer == ch )
    {
      send_to_char( "�藍�_�M���A�R���F��A�ӳ��W�a�T\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->position != POS_STANDING )
    {
      send_to_char( "�藍�_�M���A�����F�A�ѥ[�Z���j�|�a�T\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->failed > 0 )
    {
      send_to_char( "�A�餣���ܡS���A�m�n�@�I�A�ӧa�T\n\r", ch );
      RETURN_NULL()
    }

    if ( PkContrastLevel > 0
      && fight_info->fight_1
      && ch->level - fight_info->fight_1->level > PkContrastLevel )
    {
      act( "�A����ۭt���Ť�A�C$i�Ū����a�C", ch
        , &PkContrastLevel, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch->jail > 0 )
    {
      send_to_char( "����A���D���A���A���a�T\n\r", ch );
      RETURN_NULL();
    }

    if ( IS_SET( ch->act, PLR_KILLER )
      || IS_SET( ch->act, PLR_BOLTER )
      || IS_SET( ch->act, PLR_THIEF ) )
    {
      send_to_char( "�A�p�G�ѥ[�|���j�a���D�A�]�������ܡT\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->mount )
    {
      act( "�A�٦b$N�W�M���U���a�T", ch, NULL, ch->mount, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch->spirit )
    {
      act( "�藍�_�M�A����a��$N�ѥ[�Z���j�|��T", ch, NULL, ch->spirit, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch->in_room && ch->in_room->NoQuit )
    {
      act( "�����}�o�Ӱ��a��$r�A���a�T", ch, NULL, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( is_station( ch->in_room ) )
    {
      act( "�A�٦b$r�̡M�Х��U���a�M�A�Ӱѥ[�Z���j�|�a�T", ch, NULL, NULL, TO_CHAR  );
      RETURN_NULL();
    }

    if ( ch->in_room->Sail )
    {
      act( "�A�٦b$r�M�Х��U��a�M���M�L�k�ѥ[�Z���j�|�T", ch, NULL, NULL, TO_CHAR  );
      RETURN_NULL();
    }

    if ( fight_info->type == PK_CLUB )
    {
      send_to_char( "�ثe�O���������M�A���A�X�ѥ[�T\n\r", ch );
      RETURN_NULL();
    }

    if ( is_pk( ch ) )
    {
      send_to_char( "�A�w�g���W�F�M���୫�Ƴ��W��T\n\r", ch );
      RETURN_NULL();
    }

    if ( fight_info->area == NULL || !( pRoom = random_pk_room( ch ) ) )
    {
      send_to_char( "�ثe�䤣��i�H�������a��M�o�Ӱ��D���I�Y���T\n\r", ch );
      RETURN_NULL();
    }

    check_contraband( ch );

    act( "�ѤW�ƨӤ@�����M��$n�ǰe���v�޳��o�T", ch, NULL, NULL, TO_ALL );

    char_from_room( ch );
    char_to_room( ch, pRoom );
    do_look( ch, "auto" );

    /* �M���l�ܬ����I */
    clear_trace( ch, TRUE );

    set_pk( ch, NULL );

    sprintf( buf, "%s�ѥ[�F�o���Z���j�|�M�j�a���L�[�o�a�T"
      , mob_name( NULL, ch ) );

    talk_channel_2( buf, CHANNEL_PK, "" );

    if ( fight_info->status == PK_BET )
    {
      str_cpy( buf, "�@�~�@�ת��Z���j�|�w�g�B���M�Фj�a�i�H"
        "�}�l�U�`(\e[1;32mfight bet\e[0m)�F�T" );

      talk_channel_2( buf, CHANNEL_PK, "" );

      if ( ( victim = fight_info->fight_1 ) )
      {
        /* �������W�Ҧ����k�N */
        while ( victim->affected ) affect_remove( victim, victim->affected );
        victim->affected    = NULL;
      }

      if ( ( victim = fight_info->fight_2 ) )
      {
        /* �������W�Ҧ����k�N */
        while ( victim->affected ) affect_remove( victim, victim->affected );
        victim->affected     = NULL;
      }
    }
  }

  else if ( !str_prefix( arg, "bet" ) )
  {
    if ( ch->trade == FALSE )
    {
      send_to_char( "�A���`�겣�ӥi�ȤF�M�ҥH�L�k����o�ӫ��O�T\n\r", ch );
      RETURN_NULL();
    }

    if ( fight_info->lock )
    {
      send_to_char( "�藍�_�M�ثe�Z���j�|�O�������T\n\r", ch );
      RETURN_NULL();
    }

    if ( fight_info->status != PK_BET )
    {
      send_to_char( "�藍�_�M�ثe�L�k�U�`�T\n\r", ch );
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
      send_to_char( "���٪��i���O���A���ӽ�ժ��T\n\r", ch );
      RETURN_NULL();
    }

    argument = one_argument( argument, arg );
    if ( arg[0] == '\x0' )
    {
      send_to_char( "�A�n����@��Ĺ�O�S\n\r", ch );
      RETURN_NULL();
    }

    if ( ( which = atoi( arg ) ) <= 0 || which > 2 )
    {
      send_to_char( "�A�u���ܤ@(\e[1;32m1\e[0m)�ΤG(\e[1;32m2\e[0m)"
                    "Ĺ�T\n\r", ch );

      RETURN_NULL();
    }

    if ( ( fight_info->fight_1 && ( fight_info->fight_1 == ch ) && ( which == 2 ) )
      || ( fight_info->fight_2 && ( fight_info->fight_2 == ch ) && ( which == 1 ) ) )
    {
      send_to_char( "�A��ۤv���S���H�ߤF�M���ٹ�A���H�ߩO�S\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->ante[--which] > 0 )
    {
      send_to_char( "�A�u���@������T\n\r", ch );
      RETURN_NULL();
    }

    argument = one_argument( argument, arg );
    if ( arg[0] == '\x0' )
    {
      send_to_char( "�A�쩳�n��h�ֿ���LĹ�O�S\n\r", ch );
      RETURN_NULL();
    }

    if ( ( amount = atoi( arg ) ) < MinPKGold )
    {
      send_to_char( "�A������]���K�Ӥp�F�a�T\n\r", ch );
      RETURN_NULL();
    }

    if ( ( amount = atoi( arg ) ) > MaxPKGold )
    {
      send_to_char( "�A������Ӥj�F�M�A��L���򦳫H�߶ܡS\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->gold < amount )
    {
      send_to_char( "�u�O��ʰ�j�M���O���ݬݧA�ۤv���f�U�a�T\n\r", ch );
      RETURN_NULL();
    }

    if ( ( ( total = pk_ante_total( which ) ) + amount ) > MaxPKTotal )
    {
      send_to_char( "���M�o�@�䪺����Ӥj�F�M�ҥH�L�k�~���`�T\n\r", ch );
      RETURN_NULL();
    }

    gold_from_char( ch, amount );
    ch->ante[which] = amount;
    PKBetTotal     += amount;

    switch( fight_info->type )
    {
    case PK_CLUB:
      break;

    case PK_PERSON:

      if ( which == PK_FIRST && ( victim = fight_info->fight_1 ) )
      {
        if ( ch == victim )
        {
          sprintf( buf, "%s��ۤv�Q�����H�ߡM��F%d������R�ۤvĹ�T"
            , mob_name( NULL, ch ), amount );
        }

        else
        {
          sprintf( buf, "%s\e[0m(%s)��%s\e[0m(%s)�Q�����H�ߡM"
            "��F%d������R�LĹ�T"
            , ch->cname, ch->name, victim->cname, victim->name, amount );
        }

        talk_channel_2( buf, CHANNEL_PK, "" );
      }

      else if ( which == PK_SECOND && ( victim = fight_info->fight_2 ) )
      {
        if ( ch == victim )
        {
          sprintf( buf, "%s��ۤv�Q�����H�ߡM��F%d������R�ۤvĹ�T"
            , mob_name( NULL, ch ), amount );
        }

        else
        {
          sprintf( buf, "%s\e[0m(%s)��%s\e[0m(%s)�Q�����H�ߡM"
            "��F%d������R�LĹ�T"
            , ch->cname, ch->name, victim->cname, victim->name, amount );
        }

        talk_channel_2( buf, CHANNEL_PK, "" );
      }

      break;
    }
  }

  else if ( !str_prefix( arg, "status" ) && IS_IMMORTAL( ch ) )
  {
    clear_buffer();
    send_to_buffer(
      "%s\e[0m�Z���j�|���p�p�U�R\n\r%s"
      "�W�u�H��  �R%d �H�H�W�~�|�|��\n\r"
      "�Z������  �R%d ��\n\r"
      "�}�Ҧ���  �R%d ��\n\r"
      "��w���p  �R%s\n\r"
      "�̤p���  �R%d �����\n\r"
      "�̤j���  �R%d �����\n\r"
      "�̤j�`����R%d �����\n\r"
      "�֭p�`����R%d �����\n\r"
      "�ثe���p  �R"
      , mud_name, VERTICAL_LINE
      , pk_limit
      , fight_info->time, fight_info->open
      , fight_info->lock ? "��w" : "���`"
      , MinPKGold, MaxPKGold, MaxPKTotal, PKBetTotal );

    switch( fight_info->status )
    {
    default:
      send_to_buffer( "���ԡT\n\r" );
      break;

    case PK_JOIN:
      send_to_buffer( "�������W���T\n\r" );
      break;

    case PK_BET:
      send_to_buffer( "�����~���U�`���T\n\r" );
      break;

    case PK_FIGHT:
      send_to_buffer( "�Z���j�|��M���T\n\r" );
      break;

    case PK_STOP:
      send_to_buffer( "�M�z�Z���j�|�{�����T\n\r" );
      break;

    case PK_DESERT:
      send_to_buffer( "�Z���j�|���m���T\n\r" );
      break;
    }

    print_buffer( ch );
  }

  else if ( !str_prefix( arg, "lock" ) && IS_IMMORTAL( ch ) )
  {
    switch( fight_info->lock )
    {
    case TRUE:
      fight_info->lock = FALSE;
      send_to_char( "�A��Z���j�|�Ѱ���w�F�T\n\r", ch );
      break;

    case FALSE:
      fight_info->lock = TRUE;
      send_to_char( "�A��Z���j�|��w�F�T\n\r", ch );
      break;
    }
  }

  else
  {
    send_to_char( "�A���y�k���~�M�Ьd�� fight ���ϥΤ�k�T\n\r", ch );
  }
  RETURN_NULL();
}

void fight_update( void )
{
  static int  Tick = 0;
  CHAR_DATA * ch;
  CHAR_DATA * fight_1;
  CHAR_DATA * fight_2;
  char        buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "fight_update" );

  /* �p�G�Z���j�|���m�ɡM�B�W�u�H�Ƥ֩�]�w�ȡM�h�����ʧ@ */
  if ( fight_info->status == PK_DESERT && get_usernum() < pk_limit )
     RETURN_NULL();

  switch( ++Tick )
  {
  case 1:
  case 24:

    if ( Tick == 24 )
    {
      init_fight( fight_info );
      Tick = 0;
      fight_info->time++;
    }

    if ( fight_info->status == PK_JOIN )
    {
      str_cpy( buf, "�@�~�@�ת��Z���j�|�S�}�l�F�M�Фj�a������D���W(fight join)�T" );
      talk_channel_2( buf, CHANNEL_PK, "" );
    }

    else if ( fight_info->status == PK_BET )
    {
      str_cpy( buf, "�Z���j�|�w�g�V�w�M�г��~�H�h�}�l�U�`"
        "(\e[1;32mfight bet\e[0m)�T" );

      talk_channel_2( buf, CHANNEL_PK, "" );
    }

    break;

  case 2:

    if ( fight_info->status == PK_JOIN )
    {
      str_cpy( buf, "�@�~�@�ת��Z���j�|���W�I��F�M�U���Ц��a�T" );
      talk_channel_2( buf, CHANNEL_PK, "" );

      ch = NULL;

           if ( fight_info->fight_1 ) ch = fight_info->fight_1;
      else if ( fight_info->fight_2 ) ch = fight_info->fight_2;

      if ( ch )
      {
        sprintf( buf, "�u��%s���b�t�B��_�M�ګ�򨺻�j�M���M�䤣����T"
          , mob_name( NULL, ch ) );

        talk_channel_2( buf, CHANNEL_PK, "" );
      }

      cancel_pk();
      init_fight( fight_info );
      fight_info->status = PK_DESERT;
      break;
    }

    else if ( fight_info->status == PK_BET )
    {
      str_cpy( buf, "�Z���j�|�w�g�V�w�M�г��~�H�h�ɱ��U�`"
        "(\e[1;32mfight bet\e[0m)�T" );

      talk_channel_2( buf, CHANNEL_PK, "" );
    }

    break;

  case 3:

    if ( fight_info->status == PK_BET )
    {
      str_cpy( buf, "�Z���j�|�w�g�V�w�M�г��~�H�h�V�O�U�`"
        "(\e[1;32mfight bet\e[0m)�T" );

      talk_channel_2( buf, CHANNEL_PK, "" );
    }

    break;

  case 4:

    if ( fight_info->status == PK_BET )
    {
      str_cpy( buf, "������~�U�`�M�Z���j�|�w�g�}���F�T" );
      talk_channel_2( buf, CHANNEL_PK, "" );
      fight_info->status = PK_FIGHT;
      fight_info->open++;
    }

    break;

  case 18:
  case 19:
  case 20:
    if ( fight_info->status == PK_FIGHT )
    {
      switch( fight_info->type )
      {
      default:
        break;

      case PK_PERSON:

        if ( !( fight_1 = fight_info->fight_1 ) || !fight_1->in_room ) break;
        if ( !( fight_2 = fight_info->fight_2 ) || !fight_2->in_room ) break;

        if ( !fight_1->fighting
          || fight_1->fighting != fight_2
          || !fight_2->fighting
          || fight_2->fighting != fight_1 )
        {

          if ( fight_1->in_room == fight_2->in_room )
          {
            send_to_char( "�٤����I���M���~���w�g�æ��@�ΤF�T\n\r", fight_1 );
            send_to_char( "�٤����I���M���~���w�g�æ��@�ΤF�T\n\r", fight_2 );
            break;
          }

          if ( fight_1->fighting ) stop_fighting( fight_1, FALSE );
          if ( fight_2->fighting ) stop_fighting( fight_2, FALSE );

          act( "$n�����D�Q�Z����쨺�̥h�F�T", fight_1, NULL, NULL, TO_ALL );

          char_from_room( fight_1 );
          char_to_room( fight_1, fight_2->in_room );

          /* �M���l�ܬ����I */
          clear_trace( fight_1, TRUE );

          send_to_char( "�t�αj��A�̦b�@�_�M���ڥ��T\n\r", fight_1 );
          send_to_char( "�t�αj��A�̦b�@�_�M���ڥ��T\n\r", fight_2 );
        }

        break;

      case PK_CLUB:
        break;
      }
    }
    break;

  case 22:

    if ( fight_info->status == PK_FIGHT )
    {
      stop_pk( NULL );
      str_cpy( buf, "�o���Z���j�|�S��Ĺ�a�M�ҥH���賣���T" );
      talk_channel_2( buf, CHANNEL_PK, "" );
    }

    break;
  }

  RETURN_NULL();
}

ROOM_INDEX_DATA * random_pk_room( CHAR_DATA * ch )
{
  int count;
  ROOM_INDEX_DATA * pRoom;
  ROOM_INDEX_DATA * zRoom;

  AREA_DATA * pArea;

  PUSH_FUNCTION( "random_pk_room" );

  if ( !ch || !( pArea = fight_info->area ) )
  {
    mudlog( LOG_DEBUG, "random_pk_room: �ʥF�ӷ�." );
    RETURN( NULL );
  }

  count = 0;
  zRoom = NULL;
  for ( count = 0, pRoom = pArea->list; pRoom; pRoom = pRoom->next_in_area )
    if ( number_range( 0, count++ ) == 0 ) zRoom = pRoom;

  RETURN( zRoom );
}

bool is_pk( CHAR_DATA * ch )
{
  PUSH_FUNCTION( "is_pk" );

  if ( !ch ) RETURN( FALSE );

  if ( ( fight_info->fight_1 && fight_info->fight_1 == ch )
    || ( fight_info->fight_2 && fight_info->fight_2 == ch ) )
    RETURN( TRUE );

  RETURN( FALSE );
}

void check_contraband( CHAR_DATA * ch )
{
  OBJ_DATA * pObj;
  OBJ_DATA * pObj_next;
  OBJ_DATA * zObj;
  OBJ_DATA * zObj_next;

  PUSH_FUNCTION( "check_contraband" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "check_contraband: �ʥF�ӷ�." );
    RETURN_NULL();
  }

  if ( IS_NPC( ch ) ) RETURN_NULL();

  act( "���ڨ��ˬd$n���W�O�_���a���ǹH�T�~�T", ch, NULL, NULL, TO_ALL );
  act( "$n���|����M�Q���X�@�a�����ˬd�T", ch, NULL, NULL, TO_ALL );

  for ( pObj = ch->carrying; pObj; pObj = pObj_next )
  {
    pObj_next = pObj->next_content;

    if ( pObj->Contraband )
    {
      act( "�A���W��$p�O�H�T�~�M�ڨS���F�T", ch, pObj, NULL, TO_CHAR );
      extract_obj( pObj );
      continue;
    }

    if ( pObj->contains )
    {
      for ( zObj = pObj->contains; zObj; zObj = zObj_next )
      {
        zObj_next = zObj->next_content;

        if ( zObj->Contraband )
        {
          act( "�A���W��$p�O�H�T�~�M�ڨS���F�T", ch, zObj, NULL, TO_CHAR );
          extract_obj( zObj );
          continue;
        }
      }
    }
  }

  RETURN_NULL();
}

void set_pk( CHAR_DATA * ch, CLUB_DATA * club )
{
  PUSH_FUNCTION( "set_pk" );

  switch( fight_info->type )
  {
  default:
    mudlog( LOG_DEBUG, "set_pk: ���A���~." );
    break;

  case PK_PERSON:

    if ( !fight_info->fight_1 )      fight_info->fight_1 = ch;
    else if ( !fight_info->fight_2 ) fight_info->fight_2 = ch;
    else mudlog( LOG_DEBUG, "set_pk: �ӤH���A�W�B�w��." );

    if ( fight_info->fight_1 && fight_info->fight_2 )
      fight_info->status = PK_BET;

    break;

  case PK_CLUB:

    if ( !fight_info->club_1 )       fight_info->club_1 = club;
    else if ( !fight_info->club_2 ) fight_info->club_2 = club;
    else mudlog( LOG_DEBUG, "set_pk: �������A�W�B�w��." );

    if ( fight_info->club_1 && fight_info->club_2 )
      fight_info->status = PK_BET;

    break;

  case PK_NODEFINE:

    if ( ch )
    {
      fight_info->type = PK_PERSON;
      set_pk( ch, NULL );
    }

    else if ( club )
    {
      fight_info->type = PK_CLUB;
      set_pk( NULL, club );
    }

    else
    {
      mudlog( LOG_DEBUG, "set_pk: ���A�J���O�����]���O�ӤH." );
    }

    break;
  }

  RETURN_NULL();
}

void cancel_pk( void )
{
  CHAR_DATA * ch;

  PUSH_FUNCTION( "cancel_pk" );

  switch( fight_info->type )
  {
  case PK_PERSON:

    if ( ( ch = fight_info->fight_1 ) && ch->in_room )
    {
      send_to_char( "�藍�_�M�A�ӱj�F�M�ҥH�䤣����T\n\r", ch );
      char_from_room( ch );
      char_to_room( ch, get_hometown( ch ) );

      /* �M���l�ܬ����I */
      clear_trace( ch, TRUE );
    }

    if ( ( ch = fight_info->fight_2 ) && ch->in_room )
    {
      send_to_char( "�藍�_�M�A�ӱj�F�M�ҥH�䤣����T\n\r", ch );
      char_from_room( ch );
      char_to_room( ch, get_hometown( ch ) );

      /* �M���l�ܬ����I */
      clear_trace( ch, TRUE );
    }

    break;

  case PK_CLUB:
    break;
  }

  RETURN_NULL();
}

void stop_pk( void * winner )
{
  CHAR_DATA * fighter = ( CHAR_DATA * ) winner;
  CHAR_DATA * fight_1 = fight_info->fight_1;
  CHAR_DATA * fight_2 = fight_info->fight_2;

  PUSH_FUNCTION( "stop_pk" );

  if ( fight_info->status != PK_FIGHT )
  {
    mudlog( LOG_DEBUG, "stop_pk: ���A����." );
    RETURN_NULL();
  }

  switch( fight_info->type )
  {
  default:
    mudlog( LOG_DEBUG, "stop_pk: ���~�����A." );
    RETURN_NULL();

  case PK_PERSON:

    if ( fight_1->fighting ) stop_fighting( fight_1, TRUE );
    if ( fight_2->fighting ) stop_fighting( fight_2, TRUE );

    fight_1->cease = TRUE;
    fight_2->cease = TRUE;

    /* �B�z�ԳӪ� */
    if ( fighter )
    {
      if ( fighter != fight_1 && fighter != fight_2 )
      {
        mudlog( LOG_DEBUG, "stop_pk: Ĺ��D�Z�����H." );
        RETURN_NULL();
      }

      char_from_room( fighter );
      char_to_room( fighter, get_hometown( fighter ) );

      act( "$n��Ĺ�F�Z���j�|�M���a�a�^��o�̡T", fighter, NULL, NULL, TO_ALL );

      /* �M���l�ܬ����I */
      clear_trace( fighter, TRUE );

      do_look( fighter, "auto" );

      /* �o�e���� */
      if ( fighter == fight_1 ) send_ante( PK_FIRST  );
      if ( fighter == fight_2 ) send_ante( PK_SECOND );

      if ( fighter->pcdata ) fighter->pcdata->fight_win++;
    }

    /* �B�z�ԱѪ� */
    if ( fighter != fight_1 )
    {
      char_from_room( fight_1 );
      char_to_room( fight_1, RoomFail );

      act( "$n����Z���j�|�M�u�n�ۦb�o�̫�L�F�T", fight_1, NULL, NULL, TO_ALL );

      /* �M���l�ܬ����I */
      clear_trace( fight_1, TRUE );

      fight_1->failed   = 20;
      fight_1->hit      = 1;
      fight_1->mana     = 1;
      fight_1->move     = 1;
      fight_1->position = POS_STANDING;

      do_look( fight_1, "auto" );
      if ( fight_1->pcdata ) fight_1->pcdata->fight_lost++;
    }

    if ( fighter != fight_2 )
    {
      char_from_room( fight_2 );
      char_to_room( fight_2, RoomFail );

      act( "$n����Z���j�|�M�u�n�ۦb�o�̫�L�F�T", fight_2, NULL, NULL, TO_ALL );

      /* �M���l�ܬ����I */
      clear_trace( fight_2, TRUE );

      fight_2->failed   = 20;
      fight_2->hit      = 1;
      fight_2->mana     = 1;
      fight_2->move     = 1;
      fight_2->position = POS_STANDING;

      do_look( fight_2, "auto" );

      if ( fight_2->pcdata ) fight_2->pcdata->fight_lost++;
    }

    break;

  case PK_CLUB:
    break;
  }

  init_fight( fight_info );
  fight_info->status = PK_STOP;

  /* �����m�� */
  clear_ante();
  RETURN_NULL();
}

int pk_ante_total( int which )
{
  int         total = 0;
  CHAR_DATA * ch;

  PUSH_FUNCTION( "pk_ante_total" );

  if ( which == PK_FIRST || which == PK_SECOND )
  {
    for ( ch = char_list; ch; ch = ch->next )
    {
      if ( verify_char( ch ) && !IS_NPC( ch ) && ch->ante[which] > 0 )
        total += ch->ante[which];
    }
  }

  RETURN( total );
}

void send_ante( int which )
{
  int         amount_1;
  int         amount_2;
  int         gold;
  int         bonus;
  CHAR_DATA * ch;

  PUSH_FUNCTION( "send_ante" );

  if ( which != PK_FIRST && which != PK_SECOND )
  {
    mudlog( LOG_DEBUG, "send_ante: �ӷ������T." );
    RETURN_NULL();
  }

  amount_1 = pk_ante_total( PK_FIRST );
  amount_2 = pk_ante_total( PK_SECOND );

  switch( which )
  {
  case PK_FIRST:
    bonus     = amount_2 * 2 / 10;
    amount_2 -= bonus;

    if ( fight_info->fight_1 && bonus > 0 )
    {
      act( "�]���A���ԳӡM�ҥH�A�i�H����m��$i������T"
        , fight_info->fight_1, &bonus, NULL, TO_CHAR );

      if ( !over_scale( fight_info->fight_1 ) )
      {
        gold_to_char( fight_info->fight_1, bonus );
      }
      else
      {
        send_to_char( "�]���A�`�겣�w�F�W���M�ҨS������T\n\r"
          , fight_info->fight_1 );
      }
    }

    break;

  case PK_SECOND:
    bonus     = amount_1 * 2 / 10;
    amount_1 -= bonus;

    if ( fight_info->fight_2 && bonus > 0 )
    {
      act( "�]���A���L�ġM�ҥH�A�i�H����m��$i������T"
        , fight_info->fight_2, &bonus, NULL, TO_CHAR );

      if ( !over_scale( fight_info->fight_2 ) )
      {
        gold_to_char( fight_info->fight_2, bonus );
      }
      else
      {
        send_to_char( "�]���A�`�겣�w�F�W���M�ҨS������T\n\r"
          , fight_info->fight_2 );
      }
    }

    break;
  }

  amount_1 /= 1000;
  amount_2 /= 1000;

  for ( ch = char_list; ch; ch = ch->next )
  {
    if ( verify_char( ch ) && !IS_NPC( ch ) && ch->ante[which] > 0 )
    {
      gold = 0;

      switch( which )
      {
      case PK_FIRST:

        if ( amount_1 > 0 ) gold = ch->ante[PK_FIRST] * amount_2 / amount_1;
        else                gold = 0;

        gold += ch->ante[PK_FIRST];
        break;

      case PK_SECOND:

        if ( amount_2 > 0 ) gold = ch->ante[PK_SECOND] * amount_1 / amount_2;
        else                gold = 0;

        gold += ch->ante[PK_SECOND];
        break;
      }

      if ( gold > 0 )
      {
        act( "�]���A�b�Z���j�|���F�H�M�A�o��m��$i������T"
          , ch, &gold, NULL, TO_CHAR );

        if ( !over_scale( ch ) ) gold_to_char( ch, gold );
        else send_to_char( "�]���A�`�겣�w�F�W���M�ҨS������T\n\r", ch );
      }
    }
  }

  RETURN_NULL();
}

void clear_ante( void )
{
  CHAR_DATA * ch;

  PUSH_FUNCTION( "clear_ante" );

  for ( ch = char_list; ch; ch = ch->next )
    if ( verify_char( ch ) ) ch->ante[PK_FIRST] = ch->ante[PK_SECOND] = 0;

  RETURN_NULL();
}

void return_ante( void )
{
  CHAR_DATA * ch;
  int         total;

  PUSH_FUNCTION( "return_ante" );

  for ( ch = char_list; ch; ch = ch->next )
  {
    if ( !verify_char( ch ) )
    {
      if ( ( total = ch->ante[PK_FIRST] + ch->ante[PK_SECOND] ) > 0 )
      {
        act( "�Z���j�|�h�ٵ��A�m��$i������T", ch, &total, NULL, TO_CHAR );
      }
    }
  }

  RETURN_NULL();
}

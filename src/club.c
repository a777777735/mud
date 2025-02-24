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
#include <sys/times.h>
#include <time.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "merc.h"

/* �ϰ��ƪ��ŧi */
size_t        count_club_member args( ( CLUB_DATA *, int ) );
int             name_in_club    args( ( char *, CLUB_DATA * ) );
bool            clubmember_full args( ( CLUB_DATA *, int ) );
bool           countersign_club args( ( CHAR_DATA *, CLUB_DATA * ) );
bool            right_ring      args( ( char *, CLUB_DATA *, int ) );
bool            appoint_member  args( ( char *, CLUB_DATA *, int ) );
bool            fordo_club      args( ( CLUB_DATA * ) );
bool            is_note_to_club args( ( CLUB_DATA *, NOTE_DATA * ) );
bool            set_club        args( ( CLUB_DATA *, int, char *, int ) );
CLUB_DATA *     is_inclub       args( ( char * ) );
CLUB_DATA *     char_club       args( ( CHAR_DATA * ) );
CLUB_DATA *     create_club     args( ( CHAR_DATA *, char *, char * ) );

extern  void    set_club_default        args( ( CLUB_DATA * ) );
extern  int     name_in_club            args( ( char *, CLUB_DATA * ) );

int             club_room               = 0;
int             max_club                = 0;

/* �]�w������ƪ����O */
FUNCTION( do_clubset )
{
  CLUB_DATA * pClub;
  CHAR_DATA * victim;
  char      * pString;
  char        buf[MAX_STRING_LENGTH];
  char        arg1[MAX_INPUT_LENGTH];
  char        arg2[MAX_INPUT_LENGTH];
  char        arg3[MAX_INPUT_LENGTH];
  int         option;
  int         loop;

  PUSH_FUNCTION( "do_clubset" );

  /* �^���Ѽ� */
  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );
  argument = one_argument( argument, arg3 );

  /* �ʥF�Ѽ� */
  if ( !arg1[0] || !arg2[0] )
  {
    send_to_char( "�A�n�����@�������������ơS�H�U�O�ﶵ�R\n\r"
                  " 1) name       ���^�������W��\n\r"
                  " 2) cname      �����������W��\n\r"
                  " 3) money      �������������\n\r"
                  " 4) master     ������������D\n\r"
                  " 5) vicemaster ��������������D\n\r"
                  " 6) doyen      �W�[����������\n\r"
                  " 7) member     �W�[����������\n\r"
                  " 8) strip      ���������H��\n\r"
                  " 9) verify     �{�������H��\n\r"
                  "10) status     ����������A\n\r"
                  "11) fordo      �Ѵ�����\n\r"
      , ch );
    RETURN_NULL();
  }

  /* �d�߬O�_���o������ */
  if ( !( pClub = clubname_lookup( arg1, CLUB_NAME ) ) )
  {
    act( "�䤣��A�n��諸�����W��$t�C", ch, arg1, NULL, TO_CHAR );
    RETURN_NULL();
  }

  /* ���^��W�� */
  if ( !str_prefix( arg2, "name" ) )
  {
    /* �ˬd�Ѽ� */
    if ( !arg3[0] )
    {
      send_to_char( "�аݧA�n�������^��W�٧令����O�S\n\r", ch );
      RETURN_NULL();
    }

    /* �O�_�ۦP */
    if ( pClub->name && !str_cmp( pClub->name, arg3 ) )
    {
      send_to_char( "�藍�_�M�M�쥻���W�٤@�ˡM�ʧ@�����C\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd�O�_���e�N�w�g�s�b�F */
    if ( clubname_lookup( arg3, CLUB_NAME ) )
    {
      send_to_char( "�藍�_�M�w�g���o�˪������^��W�١C\n\r", ch );
      RETURN_NULL();
    }

    /* �קK���ťզr */
    for ( pString = arg3; *pString; pString++ )
    {
      if ( !isalpha( ( int ) *pString ) )
      {
        send_to_char( "�o�ӭ^��W�٤��X�k�C\n\r", ch );
        RETURN_NULL();
      }
    }

    sprintf( buf, "�t�Χ������ %s �^��W�٬� %s�T\n\r", pClub->name, arg3 );

    /* �]�w������� */
    if ( !set_club( pClub, CLUB_NAME, arg3, 0 ) )
    {
      send_to_char( "�藍�_�M�]�w�����^��W�٥��ѡC\n\r", ch );
      RETURN_NULL();
    }

    send_to_all_char( buf );
  }

  /* �����������W�� */
  else if ( !str_prefix( arg2, "cname" ) )
  {
    /* �ˬd�Ѽ� */
    if ( !arg3[0] )
    {
      send_to_char( "�аݧA�n������������W�٧令����O�S\n\r", ch );
      RETURN_NULL();
    }

    /* �O�_�ۦP */
    if ( pClub->cname && !str_cmp( pClub->cname, arg3 ) )
    {
      send_to_char( "�藍�_�M�M�쥻���W�٤@�ˡM�ʧ@�����C\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd�O�_���e�N�w�g�s�b�F */
    if ( clubname_lookup( arg3, CLUB_CNAME ) )
    {
      send_to_char( "�藍�_�M�w�g���o�˪���������W�١C\n\r", ch );
      RETURN_NULL();
    }

    for ( pString = arg3; *pString; pString++ )
    {
      if ( *pString == '%' )
      {
        send_to_char( "�藍�_�M�o�Ӥ���W�٤��X�k�C\n\r", ch );
        RETURN_NULL();
      }
    }

    sprintf( buf, "�t�Χ������ %s\e[0m ����W�٬� %s�T\n\r"
      , pClub->cname, arg3 );

    /* �]�w������� */
    if ( !set_club( pClub, CLUB_CNAME, arg3, 0 ) )
    {
      send_to_char( "�藍�_�M�]�w��������W�٥��ѡC\n\r", ch );
      RETURN_NULL();
    }
    send_to_all_char( buf );
  }

  /* ����������A */
  else if ( !str_prefix( arg2, "status" ) )
  {
    /* �ˬd�Ѽ� */
    if ( !arg3[0] )
    {
      send_to_char( "����������A�ﶵ�i�H�O�R1. �s�p 2. ����\n\r", ch );
      RETURN_NULL();
    }

    sprintf( buf, "�t�Χ������ %s\e[0m(%s)���A��"
      , pClub->cname, pClub->name );

    switch( ( option = atoi( arg3 ) ) )
    {
    /* ���w�q */
    default:
      send_to_char( "�����T�����A�ѼơM������ 1) �s�p 2) ����\n\r", ch );
      RETURN_NULL();

    /* �s�p���A */
    case CLUB_STATUS_COUNTERSIGN:

      str_cat( buf, "�s�p�C\n\r" );
      break;

    /* �w���ߪ��A */
    case CLUB_STATUS_UNIONIZE:

      str_cat( buf, "�w���ߡC\n\r" );
      break;
    }

    /* �]�w������� */
    if ( !set_club( pClub, CLUB_STATUS, NULL, option ) )
    {
      send_to_char( "�藍�_�M�]�w�������A���ѡC\n\r", ch );
      RETURN_NULL();
    }

    send_to_all_char( buf );
  }

  /* ������w������ */
  else if ( !str_prefix( arg2, "money" ) )
  {
    if ( !arg3[0] || ( option = atoi( arg3 ) ) < 0 )
    {
      send_to_char( "�藍�_�M������������Ѽƿ��~�C\n\r", ch );
      RETURN_NULL();
    }

    /* �]�w������� */
    if ( !set_club( pClub, CLUB_MONEY, NULL, option ) )
    {
      send_to_char( "�]�w�����������ѡC\n\r", ch );
      RETURN_NULL();
    }

    act( "�A���$y������������$I�C", ch, pClub, &option, TO_CHAR );
  }

  /* ������������D */
  else if ( !str_prefix( arg2, "master" ) )
  {
    /* �ˬd�Ѽ� */
    if ( !arg3[0] )
    {
      send_to_char( "�аݧA�n�����D�令�֡S\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd�O�_���e���ѥ[�L���� */
    else if ( is_inclub( arg3 ) )
    {
      send_to_char( "�藍�_�M���H���e���ѥ[�����C\n\r", ch );
      RETURN_NULL();
    }

    else
    {
      /* �Y���D�s�b, �������D���� */
      if ( clubmember_full( pClub, CLUB_MASTER ) )
      {
        if ( !char_from_club( pClub->master, pClub, CLUB_MASTER ) )
        {
          send_to_char( "�t�β������D���~�C\n\r", ch );
          RETURN_NULL();
        }

        /* �p�G���H�W�u�e��T���L */
        if ( ( victim = is_online( pClub->master ) ) )
          act( "��p�M�A�Q�t�β���$T($t)�����D¾�ȡC"
            , victim, pClub->cname, pClub->name, TO_CHAR );
      }

      sprintf( buf, "�t�Χ� %s\e[0m(%s) ���D %s �令 %s�C\n\r"
        , pClub->cname
        , pClub->name
        , pClub->master ? pClub->master : "", arg3 );

      /* �]�w���D¾�� */
      if ( !char_to_club( arg3, pClub, CLUB_MASTER ) )
      {
        send_to_char( "�t�γ]�w���D���~�C\n\r", ch );
        RETURN_NULL();
      }

      /* �p�G���H�W�u�e��T���L */
      if ( ( victim = is_online( arg3 ) ) )
        act( "�t�νЧA���$T($t)�����D�C"
          , victim, pClub->cname, pClub->name, TO_CHAR );

      send_to_all_char( buf );
    }
  }

  /* �]�w���������D */
  else if ( !str_prefix( arg2, "vicemaster" ) )
  {
    /* �ˬd�Ѽ� */
    if ( !arg3[0] )
    {
      send_to_char( "�аݧA�n������D�令�֡S\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd�O�_���e���ѥ[�L���� */
    else if ( is_inclub( arg3 ) )
    {
      send_to_char( "�藍�_�M���H���e���ѥ[�����C\n\r", ch );
      RETURN_NULL();
    }

    else
    {
      /* �Y�����D�s�b, ��������D���� */
      if ( clubmember_full( pClub, CLUB_VICEMASTER ) )
      {
        if ( !char_from_club( pClub->vicemaster, pClub, CLUB_VICEMASTER ) )
        {
          send_to_char( "�t�β��������D���~�C\n\r", ch );
          RETURN_NULL();
        }

        /* �p�G���H�W�u�e��T���L */
        if ( ( victim = is_online( pClub->master ) ) )
          act( "��p�M�A�Q�t�β��� $y �������D¾�ȡC"
            , victim, pClub, NULL, TO_CHAR );
      }

      sprintf( buf, "�t�Χ� %s\e[0m(%s)�����D %s �令 %s�C\n\r"
        , pClub->cname
        , pClub->name
        , pClub->vicemaster ? pClub->vicemaster : "", arg3 );

      /* �]�w�����D¾�� */
      if ( !char_to_club( arg3, pClub, CLUB_VICEMASTER ) )
      {
        send_to_char( "�t�γ]�w�����D���~�C\n\r", ch );
        RETURN_NULL();
      }

      /* �p�G���H�W�u�e��T���L */
      if ( ( victim = is_online( arg3 ) ) )
        act( "�t�νЧA��� $y �������D�C", victim, pClub, NULL, TO_CHAR );

      send_to_all_char( buf );
    }
  }

  /* �]�w�������� */
  else if ( !str_prefix( arg2, "doyen" ) )
  {
    /* �ˬd�Ѽ� */
    if ( !arg3[0] )
    {
      send_to_char( "�аݧA�n�W�[���@����ѡS\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd�O�_���e���ѥ[�L���� */
    else if ( is_inclub( arg3 ) )
    {
      send_to_char( "�藍�_�M���H���e���ѥ[�����C\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd�����������ѦW�B�O�_�w�� */
    else if ( clubmember_full( pClub, CLUB_DOYEN ) )
    {
      send_to_char( "�藍�_�M�����������ѦW�B�w���C\n\r", ch );
      RETURN_NULL();
    }

    else
    {
      /* �]�w����¾�� */
      if ( !char_to_club( arg3, pClub, CLUB_DOYEN ) )
      {
        send_to_char( "�t�ΰ���]�w���ѥ��ѡC\n\r", ch );
        RETURN_NULL();
      }

      /* �p�G���H�W�u�e��T���L */
      if ( ( victim = is_online( arg3 ) ) )
        act( "�t�νЧA��� $y �����ѡC", victim, pClub, NULL, TO_CHAR );

      send_to_char( "�]�w�������ѧ����C\n\r", ch );
    }
  }

  /* �]�w�������� */
  else if ( !str_prefix( arg2, "member" ) )
  {
    /* �ˬd�Ѽ� */
    if ( !arg3[0] )
    {
      send_to_char( "�аݧA�n�W�[���@�������S\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd�O�_���e���ѥ[�L���� */
    else if ( is_inclub( arg3 ) )
    {
      send_to_char( "�藍�_�M���H���e���ѥ[�����C\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd�������������W�B�O�_�w�� */
    else if ( clubmember_full( pClub, CLUB_MEMBER ) )
    {
      send_to_char( "��p�M�������������W�B�w���C\n\r", ch );
      RETURN_NULL();
    }

    else
    {
      /* �]�w��������¾�� */
      if ( !char_to_club( arg3, pClub, CLUB_MEMBER ) )
      {
        send_to_char( "�t�ΰ���]�w�������ѡC\n\r", ch );
        RETURN_NULL();
      }

      /* �p�G���H�W�u�e��T���L */
      if ( ( victim = is_online( arg3 ) ) )
        act( "�t�νЧA�ѥ[ $y �������C", victim, pClub, NULL, TO_CHAR );

      send_to_char( "�]�w�������������C\n\r", ch );
    }
  }

  /* �]�w�������� */
  else if ( !str_prefix( arg2, "follower" ) )
  {
    /* �ˬd�Ѽ� */
    if ( !arg3[0] )
    {
      send_to_char( "�аݧA�n�W�[���@��̤l�S\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd�O�_���e���ѥ[�L���� */
    else if ( is_inclub( arg3 ) )
    {
      send_to_char( "�藍�_�M���H���e���ѥ[�����C\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd�������������W�B�O�_�w�� */
    else if ( clubmember_full( pClub, CLUB_FOLLOWER ) )
    {
      send_to_char( "��p�M���������̤l�W�B�w���C\n\r", ch );
      RETURN_NULL();
    }

    else
    {
      /* �]�w��������¾�� */
      if ( !char_to_club( arg3, pClub, CLUB_FOLLOWER ) )
      {
        send_to_char( "�t�ΰ���]�w�̤l���ѡC\n\r", ch );
        RETURN_NULL();
      }

      /* �p�G���H�W�u�e��T���L */
      if ( ( victim = is_online( arg3 ) ) )
        act( "�t�νЧA�ѥ[ $y ���̤l�C", victim, pClub, NULL, TO_CHAR );

      send_to_char( "�]�w�����̤l�����C\n\r", ch );
    }
  }

  /* �����Y�H������¾��, ���F���D�H�~ */
  else if ( !str_prefix( arg2, "strip" ) )
  {
    /* �ˬd�Ѽ� */
    if ( !arg3[0] )
    {
      send_to_char( "�аݧA�n�������@�������H���S\n\r", ch );
      RETURN_NULL();
    }

    /* �Ǧ^�쥻��¾�� */
    if ( ( option = name_in_club( arg3, pClub ) ) == CLUB_NO_RELATION )
    {
      send_to_char( "�藍�_�M�o�ӤH���b�o�����������C\n\r", ch );
      RETURN_NULL();
    }

    /* ���D���ಾ�� */
    if ( option == CLUB_MASTER )
    {
      send_to_char( "�藍�_�M���D�O���ಾ���M�u���ಾ�T\n\r", ch );
      RETURN_NULL();
    }

    /* �����L��¾�� */
    if ( !char_from_club( arg3, pClub, option ) )
    {
      send_to_char( "�t�ΰ��沾��¾�쥢�ѡC\n\r", ch );
      RETURN_NULL();
    }

    /* �p�G���H�W�u�e��T���L */
    if ( ( victim = is_online( arg3 ) ) )
      send_to_char( "��p�M�A�Q�t�λ��X�����F�C\n\r", victim );

    act( "�A�� $T ���X $y �����~�F�C", ch, pClub, arg3, TO_CHAR );
  }

  /* �{������ */
  else if ( !str_prefix( arg2, "verify" ) )
  {
    /* �ˬd�Ѽ� */
    if ( !arg3[0] )
    {
      send_to_char( "�аݧA�n�{�Ҩ��������S\n\r", ch );
      RETURN_NULL();
    }

    /* �쥻�N���b���������� */
    if ( ( option = name_in_club( arg3, pClub ) ) == CLUB_NO_RELATION )
    {
      send_to_char( "�藍�_�M�o�ӤH�٨S���[�J�o�������C\n\r", ch );
      RETURN_NULL();
    }

    /* �u�������ݭn�{�� */
    if ( option != CLUB_FOLLOWER )
    {
      send_to_char( "��p�M�A�u����̤l�{�ҡM��L�����ݭn�{�ҡC\n\r", ch );
      RETURN_NULL();
    }

    for ( loop = 0; loop < MAX_CLUB_MEMBER; loop++ )
    {
      if ( pClub->member[loop] && !str_cmp( arg3, pClub->member[loop] ) )
      {
        if ( pClub->verify[loop] )
        {
          send_to_char( "��p�M���H���e�N�Q�{�ҤF�C\n\r", ch );
          RETURN_NULL();
        }

        pClub->verify[loop] = TRUE;

        /* �p�G���H�W�u�e��T���L */
        if ( ( victim = is_online( pClub->member[loop] ) ) )
        {
          victim->club = pClub;
          send_to_char( "���H�A�M�q�L�t�Ϊ������{�ҡC\n\r", victim );
        }
        break;
      }
    }
  }

  /* �Ѵ����� */
  else if ( !str_prefix( arg2, "fordo" ) )
  {
    sprintf( buf, "�t�θѴ� %s\e[0m(%s)�����M�q��%s���i���v�T\n\r"
      , pClub->cname, pClub->name, pClub->cname );

    /* �Ѵ���������� */
    if ( fordo_club( pClub ) )
    {
      send_to_char( "�A�Ѵ��F�o�������C\n\r", ch );
      send_to_all_char( buf );
    }

    else
    {
      send_to_char( "�t�θѴ��������ѡC\n\r", ch );
    }
  }

  /* �ƫ��O���~ */
  else
  {
    send_to_char( "�ﶵ���~�M�Ьd�� clubset ���O���T���Ϊk�C\n\r", ch );
  }

  RETURN_NULL();
}

/* �������O */
FUNCTION( do_club )
{
  CLUB_DATA  * pClub;
  CHAR_DATA  * victim;
  NOTE_DATA  * pNote;
  BASIC_DATA * pBasic;
  CLASS_DATA * pClass;
  char       * pString;
  char         arg1[MAX_INPUT_LENGTH];
  char         arg2[MAX_INPUT_LENGTH];
  char         arg3[MAX_INPUT_LENGTH];
  char         buf[MAX_STRING_LENGTH];
  char         pellet[MAX_STRING_LENGTH];
  int          count;
  int          loop;
  int          level;

  PUSH_FUNCTION( "do_club" );

  /* �D���a���i�H�����������O */
  if ( IS_NPC( ch ) ) RETURN_NULL();

  if ( !club_list )
  {
    act( "$t�|��������������ơC", ch, mud_name, NULL, TO_CHAR );
    RETURN_NULL();
  }

  /* �^���Ѽ� */
  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );
  argument = one_argument( argument, arg3 );

  /* �S���ѼƴN�O�d�� */
  if ( !arg1[0] )
  {
    clear_buffer();
    send_to_buffer( "\e[1;32m%s��������Ʀp�U�R\e[0m\n\r\n\r"
      "\e[1;33;44m���� �����^��W��   ��������W��         ���D          "
      "��  �A ����     ���w\e[0m\n\r" , mud_name );

    for ( count = 0, pClub = club_list; pClub; pClub = pClub->next )
    {
      tablize( FORMAT_SET,
        "\r NU) NAME           CHIN                \e[0m "
        "MAS           STATUS MEMB CLUBGO  \e[0m\n\r"
        , 0, NULL, 0 );

      tablize( FORMAT_NUM,    NULL,                          ++count,     "NU", 2  );
      tablize( FORMAT_STRING, pClub->name,                         0,   "NAME", 15 );
      tablize( FORMAT_STRING, pClub->cname,                        0,   "CHIN", 20 );
      tablize( FORMAT_STRING, pClub->master ? pClub->master : "�q��", 0, "MAS", 12 );
      tablize( FORMAT_STRING, club_status( pClub ),                0, "STATUS", 6  );
      tablize( FORMAT_NUM,  NULL, count_club_member( pClub, CLUB_ALL ), "MEMB", 4  );
      tablize( FORMAT_NUM,    NULL,                     pClub->money, "CLUBGO", 8  );
      tablize( FORMAT_RETURN, pellet, 0, NULL, sizeof( pellet ) - 1 );

      send_to_buffer( "%s", pellet );
      if ( buffer_full() ) break;
    }

    if ( count == 0 ) send_to_buffer( "�S��������������ơC\n\r" );
    print_buffer( ch );
    RETURN_NULL();
  }

  /* ���� */
  else if ( !str_prefix( arg1, "!create" ) )
  {
    /* �ˬd���� */
    if ( !IS_HERO( ch ) )
    {
      chinese_number( LEVEL_HERO, buf );
      act( "�藍�_�M�A�����n�F��$t�Ť~�i�H�C", ch, buf, NULL, TO_CHAR );
      RETURN_NULL();
    }

    /* �ˬd���� */
    if ( ch->club )
    {
      send_to_char( "��p�M�A�w�g���[�J��L�����F�C\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd�Ѽ� */
    if ( !arg2[0] || !arg3[0] )
    {
      send_to_char( "�A�������w�������^��H�Τ���W�١C\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd�O�_�W�٭��� */
    if ( clubname_lookup( arg2, CLUB_NAME )
      || clubname_lookup( arg3, CLUB_CNAME ) )
    {
      send_to_char( "�o�ӦW�٦��H�ιL�F�C\n\r", ch );
      RETURN_NULL();
    }

    /* �קK���ťզr */
    for ( pString = arg2; *pString; pString++ )
    {
      if ( !isalpha( ( int ) *pString ) )
      {
        send_to_char( "�o�ӭ^��W�٤��X�k�C\n\r", ch );
        RETURN_NULL();
      }
    }

    for ( pString = arg3; *pString; pString++ )
    {
      if ( *pString == '%' )
      {
        send_to_char( "�o�Ӥ���W�٤��X�k�C\n\r", ch );
        RETURN_NULL();
      }
    }

    /* �ˬd�����g�O */
    if ( ch->bank < ClubCreateFund )
    {
      act( "���A�ꨬ�F$i������~������T", ch, &ClubCreateFund, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( !( pClub = create_club( ch, arg2, arg3 ) ) )
    {
      send_to_char( "��p�M�t�εL�k���A���ͤ@�������C\n\r", ch );
      RETURN_NULL();
    }

    sprintf( buf, "%s\e[0m �ФF %s\e[0m(%s)�����M���Фj�a�s�p�M���¡T\n\r"
      , mob_name( NULL, ch ), pClub->cname, pClub->name );

    send_to_all_char( buf );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg1, "!countersign" ) )
  {
    /* �ˬd���� */
    if ( !IS_HERO( ch ) )
    {
      chinese_number( LEVEL_HERO, buf );
      act( "�藍�_�M�A�����n�F��$t�Ť~�i�H�C", ch, buf, NULL, TO_CHAR );
      RETURN_NULL();
    }

    /* �ˬd���� */
    if ( ch->club )
    {
      send_to_char( "��p�M�A�w�g���[�J��L�����F�C\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd�s�p�g�O */
    if ( ch->gold < ClubCountersignFund )
    {
      act( "���A�ꨬ�F$x��Ȥl�~��s�p�T", ch, &ClubCountersignFund, NULL, TO_CHAR );
      RETURN_NULL();
    }

    /* �ˬd�s�p�X�� */
    if ( ch->clublog )
    {
      send_to_char( "�藍�_�M�A�w�g���s�p�L�F�T\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd�X�� */
    if ( !arg2[0] )
    {
      send_to_char( "�A���������s�p���@�������C\n\r", ch );
      RETURN_NULL();
    }

    /* �d�߬O�_���o�˪����� */
    if ( !( pClub = clubname_lookup( arg2, CLUB_NAME ) ) )
    {
      act( "��p�M�䤣��A�n�s�p�������W�� $2$T$0�C", ch, NULL, arg2, TO_CHAR );
      RETURN_NULL();
    }

    /* ��ݪ��A */
    if ( pClub->status != CLUB_STATUS_COUNTERSIGN )
    {
      send_to_char( "�������ثe�������s�p�C\n\r", ch );
      RETURN_NULL();
    }

    if ( !countersign_club( ch, pClub ) )
    {
      send_to_char( "��p�M�A�s�p���ѡC\n\r", ch );
      RETURN_NULL();
    }

    sprintf( buf, "%s �w�g�s�p�F %s\e[0m(%s) �����T\n\r"
      , mob_name( NULL, ch ), pClub->cname, pClub->name );

    send_to_all_char( buf );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg1, "!join" ) )
  {
    /* �ˬd���� */
    if ( ch->club )
    {
      send_to_char( "��p�M�A�w�g���[�J��L�����F�C\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd�Ѽ� */
    if ( !arg2[0] )
    {
      send_to_char( "�A���������ѥ[���@�������C\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd�O�_���o�˪����� */
    if ( !( pClub = clubname_lookup( arg2, CLUB_NAME ) ) )
    {
      act( "�䤣��A�n�ѥ[�������W�� $2$T$0�C", ch, NULL, arg2, TO_CHAR );
      RETURN_NULL();
    }

    /* �������A�O�_�i�H�[�J */
    if ( pClub->status != CLUB_STATUS_UNIONIZE )
    {
      send_to_char( "�{�b����[�J�������C\n\r", ch );
      RETURN_NULL();
    }

    /* �O�_����ú��W�O */
    if ( ch->gold < ClubExpense )
    {
      act( "���A�ꨬ�F$i��Ȥl�~��[�J�����T", ch, &ClubExpense, NULL, TO_CHAR );
      RETURN_NULL();
    }

    /* �ˬd�H�ƬO�_�w�� */
    if ( clubmember_full( pClub, CLUB_FOLLOWER ) )
    {
      send_to_char( "�������H�Ƥw���C\n\r", ch );
      RETURN_NULL();
    }

    /* �[�J���� */
    if ( char_to_club( ch->name, pClub, CLUB_FOLLOWER ) )
    {
      gold_from_char( ch, ClubExpense );

      if ( pClub->money + ClubExpense / 2 <= MAX_ASSET )
        pClub->money = UMAX( 0, pClub->money + ClubExpense / 2 );

      sprintf( buf, "%s �[�J�F�������C\n\r", mob_name( NULL, ch ) );
      send_to_club( buf, pClub );

      act( "�A�[�J�F $y �����C", ch, pClub, NULL, TO_CHAR );
    }

    else
    {
      send_to_char( "�藍�_�M�A����ѥ[�������C\n\r", ch );
    }

    RETURN_NULL();
  }

  /* �d�߬Y����, �S���U�@�ӰѼƴN�O�d�ߦۤv���� */
  else if ( !str_prefix( arg1, "!who" ) )
  {
    pClub = arg2[0] == '\x0' ? ch->club : clubname_lookup( arg2, CLUB_NAME );

    /* �d�������W��, �O�_�s�b */
    if ( !pClub )
    {
      send_to_char( "�䤣��A�n�[�ݪ������W�١C\n\r", ch );
      RETURN_NULL();
    }

    club_sort( pClub );

    clear_buffer();
    send_to_buffer( "%s\e[0m(%s) �������W��u�@ %d �H�M���w %d�M%s"
      , pClub->cname, pClub->name
      , count_club_member( pClub, CLUB_ALL )
      , pClub->money, club_status( pClub ) );

    /* �u�����ڥi�H�ݨ�ж����X */
    if ( IS_IMMORTAL( ch ) )
    {
      send_to_buffer( "�M�ж����X %d"
        , pClub->location ? pClub->location->vnum : -1 );
    }

    send_to_buffer( "�v\n\r"
      "\e[1;33;44m¾  �� �W�u �W�r         ����W�r            "
      " ���� �~�� �ʧO �T�{ ¾  �~       \e[0m\n\r" );

    /* �L�X���D */
    if ( pClub->master )
    {
      victim = is_online( pClub->master );
      if ( ( pBasic = name2basic( pClub->master ) ) )
      {
        pClass  = is_class( pBasic->class, TRUE );
        tablize( FORMAT_SET,
          "��  �D ONLI NAME         CNAME                \e[0mLEVV AGEE "
          "SEXX VERI CLASSS      \e[0m\n\r"
          , 0, NULL, 0 );

        tablize( FORMAT_STRING, pClub->master  , 0, "NAME" , 12 );
        tablize( FORMAT_STRING, pBasic->cname  , 0, "CNAME", 20 );
        tablize( FORMAT_STRING, YESNO( 1 )     , 0, "VERI" ,  4 );
        tablize( FORMAT_STRING, YESNO( victim ), 0, "ONLI" ,  4 );

        tablize( FORMAT_STRING, class_name( pClass, FALSE )
          , 0, "CLASSS" , 6 );

        tablize( FORMAT_NUM   , NULL         , pBasic->level, "LEVV" ,  4 );

        tablize( FORMAT_NUM   , NULL
          , ( pBasic->played / 14400 ) + BornAge, "AGEE",  4 );

        tablize( FORMAT_STRING, sex_name( pBasic->sex ), 0, "SEXX", 4 );
        tablize( FORMAT_RETURN, pellet, 0, NULL, sizeof( pellet ) - 1 );
        send_to_buffer( "%s", pellet );

        free_basic( pBasic );
      }
      else
      {
        send_to_buffer( "��  �D %2s   %-12s ????                "
          " ???? ???? ???? %2s   ??????\n\r"
          , YESNO( victim ), pClub->master, YESNO( 1 ) );
      }
    }
    else
    {
      send_to_buffer( "��  �D %2s   �q��\n\r", YESNO( 0 ) );
    }

    if ( pClub->vicemaster )
    {
      victim = is_online( pClub->vicemaster );

      if ( ( pBasic = name2basic( pClub->vicemaster ) ) )
      {
        pClass = is_class( pBasic->class, TRUE );

        tablize( FORMAT_SET,
          "�����D ONLI NAME         CNAME                \e[0mLEVV AGEE"
          " SEXX VERI CLASSS      \e[0m\n\r"
          , 0, NULL, 0 );

        tablize( FORMAT_STRING, pClub->vicemaster, 0, "NAME" , 12 );
        tablize( FORMAT_STRING, pBasic->cname    , 0, "CNAME", 20 );
        tablize( FORMAT_STRING, YESNO( 1 )       , 0, "VERI" ,  4 );
        tablize( FORMAT_STRING, YESNO( victim )  , 0, "ONLI" ,  4 );
        tablize( FORMAT_NUM   , NULL         , pBasic->level, "LEVV" ,  4 );

        tablize( FORMAT_STRING, class_name( pClass, FALSE )
          , 0, "CLASSS" , 6 );

        tablize( FORMAT_NUM   , NULL
          , ( pBasic->played / 14400 ) + BornAge, "AGEE",  4 );

        tablize( FORMAT_STRING, sex_name( pBasic->sex ), 0, "SEXX", 4 );
        tablize( FORMAT_RETURN, pellet, 0, NULL, sizeof( pellet ) - 1 );
        send_to_buffer( "%s", pellet );

        free_basic( pBasic );
      }
      else
      {
        send_to_buffer( "�����D %2s   %-12s ????                "
          " ???? ???? ???? %2s   ??????\n\r"
          , YESNO( victim ), pClub->vicemaster, YESNO( 1 ) );
      }
    }

    /* �L�X���� */
    for ( loop = 0; loop < MAX_CLUB_DOYEN; loop++ )
    {
      if ( pClub->doyen[loop] )
      {
        victim = is_online( pClub->doyen[loop] );

        if ( ( pBasic = name2basic( pClub->doyen[loop] ) ) )
        {
          pClass = is_class( pBasic->class, TRUE );

          tablize( FORMAT_SET,
            "��  �� ONLI NAME         CNAME                \e[0mLEVV AGEE "
            "SEXX VERI CLASSS      \e[0m\n\r"
            , 0, NULL, 0 );

          tablize( FORMAT_STRING, pBasic->name     , 0, "NAME" , 12 );
          tablize( FORMAT_STRING, pBasic->cname    , 0, "CNAME", 20 );
          tablize( FORMAT_STRING, YESNO( 1 )       , 0, "VERI" ,  4 );
          tablize( FORMAT_STRING, YESNO( victim )  , 0, "ONLI" ,  4 );
          tablize( FORMAT_NUM   , NULL , pBasic->level, "LEVV" ,  4 );

          tablize( FORMAT_STRING, class_name( pClass, FALSE )
            , 0, "CLASSS" , 6 );

          tablize( FORMAT_NUM   , NULL
            , ( pBasic->played / 14400 ) + BornAge, "AGEE",  4 );

          tablize( FORMAT_STRING, sex_name( pBasic->sex ), 0, "SEXX", 4 );
          tablize( FORMAT_RETURN, pellet, 0, NULL, sizeof( pellet ) - 1 );
          send_to_buffer( "%s", pellet );

          free_basic( pBasic );
        }
        else
        {
          send_to_buffer( "��  �� %2s   %-12s ????                "
            " ???? ???? ???? %2s   ??????\n\r"
            , YESNO( victim ), pClub->doyen[loop], YESNO( 1 ) );
        }
      }
    }

    /* �C�L���� */
    for ( loop = 0; loop < MAX_CLUB_MEMBER; loop++ )
    {
      if ( pClub->member[loop] && pClub->verify[loop] )
      {
        victim = is_online( pClub->member[loop] );

        if ( ( pBasic = name2basic( pClub->member[loop] ) ) )
        {
          pClass = is_class( pBasic->class, TRUE );

          tablize( FORMAT_SET,
            "��  �� ONLI NAME         CNAME                \e[0mLEVV AGEE "
            "SEXX VERI CLASSS      \e[0m\n\r"
            , 0, NULL, 0 );

          tablize( FORMAT_STRING, pBasic->name     , 0, "NAME" , 12 );
          tablize( FORMAT_STRING, pBasic->cname    , 0, "CNAME", 20 );
          tablize( FORMAT_STRING, YESNO( 1 )       , 0, "VERI" ,  4 );
          tablize( FORMAT_STRING, YESNO( victim )  , 0, "ONLI" ,  4 );
          tablize( FORMAT_NUM   , NULL , pBasic->level, "LEVV" ,  4 );

          tablize( FORMAT_STRING, class_name( pClass, FALSE )
            , 0, "CLASSS" , 6 );

          tablize( FORMAT_NUM   , NULL
            , ( pBasic->played / 14400 ) + BornAge, "AGEE",  4 );

          tablize( FORMAT_STRING, sex_name( pBasic->sex ), 0, "SEXX", 4 );
          tablize( FORMAT_RETURN, pellet, 0, NULL, sizeof( pellet ) - 1 );
          send_to_buffer( "%s", pellet );
          free_basic( pBasic );
          if ( buffer_full() ) break;
        }
        else
        {
          send_to_buffer( "��  �� %2s   %-12s ????                "
            " ???? ???? ???? %2s   ??????\n\r"
            , YESNO( victim ), pClub->member[loop], YESNO( 1 ) );
        }
      }
    }

    /* �C�L�̤l */
    for ( loop = 0; loop < MAX_CLUB_MEMBER; loop++ )
    {
      if ( pClub->member[loop] && !pClub->verify[loop] )
      {
        victim = is_online( pClub->member[loop] );

        if ( ( pBasic = name2basic( pClub->member[loop] ) ) )
        {
          pClass = is_class( pBasic->class, TRUE );

          tablize( FORMAT_SET,
            "��  �l ONLI NAME         CNAME                \e[0mLEVV AGEE "
            "SEXX VERI CLASSS      \e[0m\n\r"
            , 0, NULL, 0 );

          tablize( FORMAT_STRING, pBasic->name     , 0, "NAME" , 12 );
          tablize( FORMAT_STRING, pBasic->cname    , 0, "CNAME", 20 );
          tablize( FORMAT_STRING, YESNO( 0 )       , 0, "VERI" ,  4 );
          tablize( FORMAT_STRING, YESNO( victim )  , 0, "ONLI" ,  4 );
          tablize( FORMAT_NUM   , NULL , pBasic->level, "LEVV" ,  4 );

          tablize( FORMAT_STRING, class_name( pClass, FALSE )
            , 0, "CLASSS" , 6 );

          tablize( FORMAT_NUM   , NULL
            , ( pBasic->played / 14400 ) + BornAge, "AGEE",  4 );

          tablize( FORMAT_STRING, sex_name( pBasic->sex ), 0, "SEXX", 4 );
          tablize( FORMAT_RETURN, pellet, 0, NULL, sizeof( pellet ) - 1 );
          send_to_buffer( "%s", pellet );
          free_basic( pBasic );
          if ( buffer_full() ) break;
        }
        else
        {
          send_to_buffer( "��  �l %2s   %-12s ????                "
            " ???? ???? ???? %2s   ??????\n\r"
            , YESNO( victim ), pClub->member[loop], YESNO( 0 ) );
        }
      }
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  /* �{������ */
  else if ( !str_prefix( arg1, "!verify" ) )
  {
    /* �ˬd���� */
    if ( !( pClub = ch->club ) )
    {
      send_to_char( "�A�S���ѥ[�����C\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd�O�_��¾�v */
    if ( !right_ring( ch->name, ch->club, CLUB_DOYEN ) )
    {
      send_to_char( "�A�S���v�O�h�{�������̤l�C\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd���������A�O�_�i�H�{�������F */
    if ( pClub->status != CLUB_STATUS_UNIONIZE )
    {
      send_to_char( "�{�b�٤���{�ҧA���̤l�C\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd�Ѽ� */
    if ( !arg2[0] )
    {
      send_to_char( "�A�n�{�Ҩ��������̤l�C\n\r", ch );
      RETURN_NULL();
    }

    /* �ڥ����O�������H */
    if ( ( level = name_in_club( arg2, pClub ) ) == CLUB_NO_RELATION )
    {
      send_to_char( "�A�S���o�������C\n\r", ch );
      RETURN_NULL();
    }

    /* �u�������~�ݭn�{�� */
    if ( level != CLUB_FOLLOWER )
    {
      send_to_char( "�A�u��{�ҧA�������̤l�C\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd�O�_�u�����o������, �άO�w�g�q�L�{�� */
    for ( loop = 0; loop < MAX_CLUB_MEMBER; loop++ )
    {
      if ( pClub->member[loop] && !str_cmp( arg2, pClub->member[loop] ) )
      {
        if ( pClub->verify[loop] )
        {
          send_to_char( "�A���̤l�w�g�q�L�{�ҤF�C\n\r", ch );
          RETURN_NULL();
        }

        pClub->verify[loop] = TRUE;
        send_to_char( "���H�M�A���̤l�w�g�q�L�{�ҡC\n\r", ch );

        /* ��ݹ�H�O�_�W�u */
        if ( ( victim = is_online( arg2 ) ) )
        {
          victim->club = pClub;
          send_to_char( "���H�A�M�q�L���D���{�ҡC\n\r", victim );
        }

        RETURN_NULL();
      }
    }
  }

  /* ��b����L�̤l */
  else if ( !str_prefix( arg1, "!dump" ) )
  {
    /* �ˬd���� */
    if ( !( pClub = ch->club ) )
    {
      send_to_char( "�A�S���ѥ[�����C\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd�O�_��¾�v */
    if ( !right_ring( ch->name, pClub, CLUB_VICEMASTER ) )
    {
      send_to_char( "�A�S���v�O�h��b�������̤l�C\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd���������A�O�_�i�H��b�������F */
    if ( pClub->status != CLUB_STATUS_UNIONIZE )
    {
      send_to_char( "�{�b�٤�����b���A���̤l�C\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd�Ѽ� */
    if ( !arg2[0] )
    {
      send_to_char( "�A�n��b������̤l�S\n\r", ch );
      RETURN_NULL();
    }

    /* ��ݹ�H�O�_�W�u, �����W�u�~����b */
    if ( !( victim = is_online( arg2 ) ) )
    {
      send_to_char( "�A����̤l�ثe�S���W�u�C\n\r", ch );
      RETURN_NULL();
    }

    /* �O�_�P�@������, ���P����������b */
    if ( !is_same_club( ch, victim ) )
    {
      send_to_char( "���줣�O�A�������H�h�C\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd�Ѽ� */
    if ( !arg3[0] )
    {
      send_to_char( "�A�n��h�ֿ����L�O�S\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd�Ѽ� */
    if ( ( count = atoi( arg3 ) ) <= 0 || count > pClub->money )
    {
      send_to_char( "�A�n��ݼֲ����L�ܡS\n\r", ch );
      RETURN_NULL();
    }

    if ( over_scale( victim ) )
    {
      act( "$N���`�겣�w�F�W���T", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    act( "�A���F $i �����A���̤l$N�C", ch, &count, victim, TO_CHAR );
    act( "$N ���F $i �����A�C", victim, &count, ch, TO_CHAR );

    pClub->money -= count;
    gold_to_char( victim, count );

    RETURN_NULL();
  }

  /* �ܽ� */
  else if ( !str_prefix( arg1, "!invite" ) )
  {
    /* �ˬd���� */
    if ( !( pClub = ch->club ) )
    {
      send_to_char( "�A�٨S���[�J�����M����ܽСC\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd���������A�O�_�i�H�ܽ� */
    if ( pClub->status != CLUB_STATUS_UNIONIZE )
    {
      send_to_char( "�{�b�٤����ܽШ�L�H�ѥ[�����C\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd�O�_��¾�v */
    if ( !right_ring( ch->name, pClub, CLUB_MEMBER ) )
    {
      send_to_char( "�A�S���v�O�h�ܽЧO�H�J���C�C\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd�Ѽ� */
    if ( !arg2[0] )
    {
      send_to_char( "�A�n�ܽн֩O�S\n\r", ch );
      RETURN_NULL();
    }

    /* ��ݹ�H�O�_�W�u, �W�u�~���ܽ� */
    if ( !( victim = get_char_world( ch, arg2 ) ) )
    {
      act( "�䤣��A�n�ܽЪ��H $2$T$0�C", ch, NULL, arg2, TO_CHAR );
      RETURN_NULL();
    }

    /* �ˬd�O�_�i�H�ܽХL */
    if ( IS_NPC( victim ) || victim->club )
    {
      act( "�A�L�k�ܽ�$N�C", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    act( "�A�ܽ�$N�Ӱѥ[�A�������C", ch, NULL, victim, TO_CHAR );
    act( "$N�ܽЧA�ѥ[�L������$y�C", victim, pClub, ch, TO_CHAR );
    RETURN_NULL();
  }

  /* ���m */
  else if ( !str_prefix( arg1, "!donate" ) )
  {
    /* �ˬd���� */
    if ( !( pClub = ch->club ) )
    {
      send_to_char( "�A���S���[�J�����M�p�󮽿�C\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->trade == FALSE )
    {
      send_to_char( "�A���`�겣�ӥi�ȤF�M�ҥH�L�k����o�ӫ��O�T\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd�Ѽ� */
    if ( !arg2[0] )
    {
      send_to_char( "�A�n���h�ֿ��O�S\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd�ƥجO�_�X�z */
    if ( ( count = atoi( arg2 ) ) <= 0 || count > ch->gold )
    {
      send_to_char( "�A�����O�ȿ��ܡS\n\r", ch );
      RETURN_NULL();
    }

    if ( pClub->money + count <= MAX_ASSET )
      pClub->money = UMAX( 0, pClub->money + count );

    gold_from_char( ch, count );

    sprintf( buf, " %s���F���� %d ���C\n\r", mob_name( NULL, ch ), count );
    send_to_club( buf, pClub );

    RETURN_NULL();
  }

  /* ��� */
  else if ( !str_prefix( arg1, "!withdraw" ) )
  {
    /* �ˬd���� */
    if ( !( pClub = ch->club ) )
    {
      send_to_char( "�A�٨S���ѥ[�����M�p�����S\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd���������A�O�_�i�H��� */
    if ( pClub->status != CLUB_STATUS_UNIONIZE )
    {
      send_to_char( "�{�b�٤�������������C\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd�O�_��¾�v */
    if ( !right_ring( ch->name, pClub, CLUB_DOYEN ) )
    {
      send_to_char( "�A�S���v�O�h�����������C\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd�Ѽ� */
    if ( !arg2[0] )
    {
      send_to_char( "�A�n��h�����������S\n\r", ch );
      RETURN_NULL();
    }

    if ( over_scale( ch ) )
    {
      send_to_char( "�A���`�겣�w�F�W���M�L�k�~�����T\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd�ƥجO�_�X�z */
    if ( ( count = atoi( arg2 ) ) <= 0 || count > pClub->money )
    {
      send_to_char( "�A�⪺�O�ȿ��ܡS\n\r", ch );
      RETURN_NULL();
    }

    pClub->money -= count;
    gold_to_char( ch, count );

    sprintf( buf, " %s �q������F %d ���C\n\r", mob_name( NULL, ch ), count );
    send_to_club( buf, pClub );

    RETURN_NULL();
  }

  /* �X�v���� */
  else if ( !str_prefix( arg1, "!kick" ) )
  {
    /* �ˬd���� */
    if ( !( pClub = ch->club ) )
    {
      send_to_char( "�A�S���ѥ[�����M�L�k����C\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd���������A�O�_�i�H�X�v���� */
    if ( pClub->status != CLUB_STATUS_UNIONIZE )
    {
      send_to_char( "�{�b�٤����X�v�����H�h�C\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd�O�_��¾�v */
    if ( !right_ring( ch->name, pClub, CLUB_VICEMASTER ) )
    {
      send_to_char( "�A�S���v�O�h�X�v�����C\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd�Ѽ� */
    if ( !arg2[0] )
    {
      send_to_char( "�A�n�X�v���������C\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd�O�_�u�����o������ */
    if ( ( level = name_in_club( arg2, pClub ) ) == CLUB_NO_RELATION )
    {
      send_to_char( "�A�S�����������C\n\r", ch );
      RETURN_NULL();
    }

    /* �u���X�v����, ��L�u��έ��Ť���A�X�v */
    if ( level >= CLUB_DOYEN )
    {
      send_to_char( "�A�u���X�v�A�������C\n\r", ch );
      RETURN_NULL();
    }

    if ( !char_from_club( arg2, pClub, level ) )
    {
      send_to_char( "�L�k�X�v���������C\n\r", ch );
      RETURN_NULL();
    }

    if ( ( victim = is_online( arg2 ) ) )
      send_to_char( "�A�Q�����X�v�X���F�C\n\r", victim );

    sprintf( buf, "%s �Q�X�v�X���F�C\n\r", arg2 );
    send_to_club( buf, pClub );

    RETURN_NULL();
  }

  /* �q�� */
  else if ( !str_prefix( arg1, "!quit" ) )
  {
    if ( !arg2[0] || !( pClub = clubname_lookup( arg2, CLUB_NAME ) ) )
    {
      send_to_char( "�A�����n��A�������W�r����~�i�H��T\n\r", ch );
      RETURN_NULL();
    }

    if ( ( level = name_in_club( ch->name, pClub ) ) == CLUB_NO_RELATION )
    {
      send_to_char( "�A�S���ѥ[�o�������M�p��q���T\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd���������A�O�_�i�H�q�� */
    if ( pClub->status != CLUB_STATUS_UNIONIZE )
    {
      send_to_char( "�{�b�٤���q���C\n\r", ch );
      RETURN_NULL();
    }

    if ( level == CLUB_MASTER )
    {
      send_to_char( "�]���A��¾�ȡM�ҥH�A����q���C\n\r", ch );
      RETURN_NULL();
    }

    if ( char_from_club( ch->name, pClub, level ) )
    {
      if ( level == CLUB_FOLLOWER )
      {
        send_to_char( "�A�q���F�M���O�٨S�Q�����{�ҡM�ҥH�S�ơT\n\r", ch );
      }

      else
      {
        send_to_char( "�A�q���F�M���@�ɪ��H���i�H�l���A�F�T\n\r", ch );
        SET_BIT( ch->act, PLR_BOLTER );
      }

      sprintf( buf, "%s �w�g�q���X���F�T\n\r", mob_name( NULL, ch ) );
      send_to_club( buf, pClub );
    }

    else
    {
      send_to_char( "�A�q�����ѡT\n\r" , ch );
    }
  }

  /* �X�v */
  else if ( !str_prefix( arg1, "!dispel" ) )
  {
    /* �ˬd���� */
    if ( !( pClub = ch->club ) )
    {
      send_to_char( "�A�S���ѥ[�����M�L�k�����X�v�����H�h�C\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd���������A�O�_�i�H�X�v���� */
    if ( pClub->status != CLUB_STATUS_UNIONIZE )
    {
      send_to_char( "�{�b�٤����X�v�����H�h�C\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd�O�_��¾�v */
    if ( !right_ring( ch->name, pClub, CLUB_MASTER ) )
    {
      send_to_char( "�A�S���v�O�h�X�v�����C\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd�Ѽ� */
    if ( !arg2[0] )
    {
      send_to_char( "�A�n�X�v���������C\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd�O�_�u�����o������ */
    if ( ( level = name_in_club( arg2, pClub ) ) == CLUB_NO_RELATION )
    {
      send_to_char( "�A�S�����������C\n\r", ch );
      RETURN_NULL();
    }

    if ( level == CLUB_MASTER )
    {
      send_to_char( "�A�����X�v���D�C\n\r", ch );
      RETURN_NULL();
    }

    if ( !( victim = is_online( arg2 ) ) )
    {
      act( "$t�S���W�u�M�A�����X�v�L�C", ch, arg2, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( char_from_club( arg2, pClub, level ) )
    {
      SET_BIT( victim->act, PLR_BOLTER );
      send_to_char( "�A�Q���D�X�v�X���F�C\n\r", victim );
      sprintf( buf, "%s�Q���D�X�v�X���F�C\n\r", mob_name( NULL, victim ) );
      send_to_club( buf, pClub );
    }

    else
    {
      send_to_char( "�X�v�������ѡC\n\r", ch );
      RETURN_NULL();
    }

    RETURN_NULL();
  }

  /* ���R */
  else if ( !str_prefix( arg1, "!appoint" ) )
  {
    /* �ˬd���� */
    if ( !( pClub = ch->club ) )
    {
      send_to_char( "�A�S���ѥ[�����M�L�k������R¾��C\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd���������A�O�_�i�H���R���� */
    if ( pClub->status != CLUB_STATUS_UNIONIZE )
    {
      send_to_char( "�{�b�٤�����R�����H�h�C\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd�O�_��¾�v */
    if ( !right_ring( ch->name, pClub, CLUB_MASTER ) )
    {
      send_to_char( "��p�M�A�S���v�O�h���R�����C\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd�Ѽ� */
    if ( !arg2[0] )
    {
      send_to_char( "�A�n���R���������C\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd�Ѽ� */
    if ( !arg3[0] )
    {
      send_to_char( "�A�n���R�L�������D(vicemaster)�Ϊ���(doyen)�S\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd�O�_�u�����o������ */
    if ( ( level = name_in_club( arg2, pClub ) ) == CLUB_NO_RELATION )
    {
      send_to_char( "�A�S�����������C\n\r", ch );
      RETURN_NULL();
    }

    if ( level == CLUB_FOLLOWER )
    {
      send_to_char( "���������S���q�L�{�ҡC\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd¾�� */
    if ( level != CLUB_MEMBER )
    {
      send_to_char( "�A�u����R�����������D�Ϊ��ѡC\n\r", ch );
      RETURN_NULL();
    }

    victim = is_online( arg2 );

    /* ���R�������D */
    if ( !str_prefix( arg3, "vicemaster" ) )
    {
      /* �ˬd�O�_�w�g�������D */
      if ( clubmember_full( pClub, CLUB_VICEMASTER ) )
      {
        send_to_char( "���������w�g�������D�F�C\n\r", ch );
        RETURN_NULL();
      }

      if ( appoint_member( arg2, pClub, CLUB_VICEMASTER ) )
      {
        sprintf( buf, "%s���R%s�������������D�C\n\r"
          , mob_name( NULL, ch ), arg2 );

        send_to_club( buf, pClub );
        if ( victim ) send_to_char( "�A�Q���R�����������D�C\n\r", victim );
      }

      else
      {
        send_to_char( "���R�����D���ѡC\n\r", ch );
      }

      RETURN_NULL();
    }

    /* ���R������ */
    else if ( !str_prefix( arg3, "doyen" ) )
    {
      /* �ˬd�O�_�w�g������ */
      if ( clubmember_full( pClub, CLUB_DOYEN ) )
      {
        send_to_char( "�����������ѦW�B�w���C\n\r", ch );
        RETURN_NULL();
      }

      if ( appoint_member( arg2, pClub, CLUB_DOYEN ) )
      {
        sprintf( buf, "%s���R%s�����������ѡC\n\r"
          , mob_name( NULL, ch ), arg2 );

        send_to_club( buf, pClub );
        if ( victim ) send_to_char( "�A�Q���R���������ѡC\n\r", victim );
      }

      else
      {
        send_to_char( "���R���ѥ��ѡC\n\r", ch );
      }
    }

    else
    {
      send_to_char( "�A�n���R�L�������D(vicemaster)�Ϊ���(doyen)�S\n\r", ch );
    }
  }

  /* �Ѹu */
  else if ( !str_prefix( arg1, "!dismiss" ) )
  {
    /* �ˬd���� */
    if ( !( pClub = ch->club ) )
    {
      send_to_char( "�A�S���ѥ[�����M�L�k����C\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd���������A�O�_�i�H�Ѹu���� */
    if ( pClub->status != CLUB_STATUS_UNIONIZE )
    {
      send_to_char( "�{�b�٤���Ѹu�����H�h�C\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd�O�_��¾�v */
    if ( !right_ring( ch->name, pClub, CLUB_MASTER ) )
    {
      send_to_char( "�A�S���v�O�h�Ѹu�����C\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd�Ѽ� */
    if ( !arg2[0] )
    {
      send_to_char( "�A�n�Ѹu���������C\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd�O�_�٦����������B */
    if ( clubmember_full( pClub, CLUB_MEMBER ) )
    {
      send_to_char( "�A�������w�g���B�F�M�ҥH�L�k�����C\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd�O�_�u�����o������ */
    if ( ( level = name_in_club( arg2, pClub ) ) == CLUB_NO_RELATION )
    {
      send_to_char( "�A�S�����������C\n\r", ch );
      RETURN_NULL();
    }

    victim = is_online( arg2 );

    switch( level )
    {
    default:
      send_to_char( "�A�u��Ѹu�����D�άO���ѡC\n\r", ch );
      RETURN_NULL();

    /* �����D */
    case CLUB_VICEMASTER:

      if ( appoint_member( arg2, pClub, CLUB_MEMBER ) )
      {
        sprintf( buf, "%s ������D %s �Ѹu�F�C\n\r"
          , mob_name( NULL, ch ), arg2 );

        send_to_club( buf, pClub );
        if ( victim ) send_to_char( "�A���A�O�����D�F�C\n\r", victim );
      }

      else
      {
        send_to_char( "�Ѹu�����D���ѡC\n\r", ch );
      }
      break;

    /* ���� */
    case CLUB_DOYEN:

      if ( appoint_member( arg2, pClub, CLUB_MEMBER ) )
      {
        sprintf( buf, "%s ����� %s �Ѹu�F�C\n\r"
          , mob_name( NULL, ch ), arg2 );

        send_to_club( buf, pClub );
        if ( victim ) send_to_char( "�A���A�O���ѤF�C\n\r", victim );
      }

      else
      {
        send_to_char( "�Ѹu���ѥ��ѡC\n\r", ch );
      }

      break;
    }
  }

  /* �I�� */
  else if ( !str_prefix( arg1, "!abdicate" ) )
  {
    /* �ˬd���� */
    if ( !( pClub = ch->club ) )
    {
      send_to_char( "�A�S���ѥ[�����M�L�k����C\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd���������A�O�_�i�H�I�� */
    if ( pClub->status != CLUB_STATUS_UNIONIZE )
    {
      send_to_char( "�{�b�٤����I�쵹�����H�h�C\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd�O�_��¾�v */
    if ( !right_ring( ch->name, pClub, CLUB_MASTER ) )
    {
      send_to_char( "�A�S���v�O���I��C\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd�Ѽ� */
    if ( !arg2[0] )
    {
      send_to_char( "�A�n�I�쵹���������S\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd�O�_�u�����o������ */
    if ( ( level = name_in_club( arg2, pClub ) ) == CLUB_NO_RELATION )
    {
      send_to_char( "�A�S�����������C\n\r", ch );
      RETURN_NULL();
    }

    if ( level == CLUB_FOLLOWER )
    {
      send_to_char( "��p�M���������S���q�L�{�ҡC\n\r", ch );
      RETURN_NULL();
    }

    if ( clubmember_full( pClub , CLUB_MEMBER ) )
    {
      send_to_char( "�A�������w�g���B�F�M�ҥH�L�k�����C\n\r", ch );
      RETURN_NULL();
    }

    sprintf( buf, "���D%s�����D����l�I�쵹%s�C\n\r"
      , mob_name( NULL, ch ), arg2 );

    send_to_club( buf, pClub );

    appoint_member( ch->name, pClub, CLUB_MEMBER );
    appoint_member( arg2    , pClub, CLUB_MASTER );
  }

  else if ( !str_prefix( arg1, "!set" ) )
  {
    /* �ˬd���� */
    if ( !( pClub = ch->club ) )
    {
      send_to_char( "�A�S���ѥ[�����M�L�k����C\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd���������A�O�_�i�H���� */
    if ( pClub->status != CLUB_STATUS_UNIONIZE )
    {
      send_to_char( "�{�b�٤�����������]�w�C\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd�O�_��¾�v */
    if ( !right_ring( ch->name, pClub, CLUB_MASTER ) )
    {
      send_to_char( "�A�S���v�O��������]�w�C\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd�Ѽ� */
    if ( !arg2[0] )
    {
      send_to_char( "�A�n�擄��ﶵ�S\n\r", ch );
      RETURN_NULL();
    }

    if ( !str_prefix( arg2, "name" ) )
    {
      /* �ˬd�Ѽ� */
      if ( !arg3[0] )
      {
        send_to_char( "�аݧA�n��^��W�٧令����S\n\r", ch );
        RETURN_NULL();
      }

      if ( clubname_lookup( arg3, CLUB_NAME ) )
      {
        send_to_char( "��p�M�o�ӭ^��W�٦��H�ιL�F�C\n\r", ch );
        RETURN_NULL();
      }

      set_club( pClub, CLUB_NAME, arg3, 0 );

      sprintf( buf, "���D %s �����W�令 %s\e[0m(%s)�C\n\r"
        , mob_name( NULL, ch ), pClub->cname, pClub->name );

      send_to_club( buf, pClub );
    }

    else if ( !str_prefix( arg2, "cname" ) )
    {
      /* �ˬd�Ѽ� */
      if ( !arg3[0] )
      {
        send_to_char( "�аݧA�n�⤤��W�٧令����S\n\r", ch );
        RETURN_NULL();
      }

      if ( clubname_lookup( arg3, CLUB_CNAME ) )
      {
        send_to_char( "��p�M�o�Ӥ���W�٦��H�ιL�F�C\n\r", ch );
        RETURN_NULL();
      }

      set_club( pClub, CLUB_CNAME, arg3, 0 );

      sprintf( buf, "���D %s �����W�令 %s\e[0m(%s)�C\n\r"
        , mob_name( NULL, ch ), pClub->cname, pClub->name );
      send_to_club( buf, pClub );
    }

    else
    {
      send_to_char( "�ﶵ���~�M�Х� club /? �d�� club ���O�C\n\r", ch );
    }
  }

  else if ( !str_prefix( arg1, "!fordo" ) )
  {
    if ( !( pClub = ch->club ) )
    {
      send_to_char( "�A�٨S���ѥ[�����C\n\r", ch );
      RETURN_NULL();
    }

    if ( !arg2[0] || pClub != clubname_lookup( arg2, CLUB_NAME ) )
    {
      send_to_char( "�A�����n��A�������W�r����~�i�H��T\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd���������A�O�_�i�H�Ѵ� */
    if ( pClub->status != CLUB_STATUS_UNIONIZE )
    {
      send_to_char( "�藍�_�M�{�b�٤���Ѵ������C\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd�O�_��¾�v */
    if ( !right_ring( ch->name, pClub, CLUB_MASTER ) )
    {
      send_to_char( "��p�M�A�S���v�O�h�Ѵ������C\n\r", ch );
      RETURN_NULL();
    }

    sprintf( buf, "%s\e[0m(%s)�����D %s �Ѵ��F�����M�q�� %s\e[0m ���i���v�T\n\r"
      , pClub->cname, pClub->name, mob_name( NULL, ch ), pClub->cname );

    if ( fordo_club( pClub ) )
    {
      send_to_char( "�A�Ѵ������C\n\r", ch );
      send_to_all_char( buf );
    }

    else
    {
      send_to_char( "�t�θѴ��������ѡC\n\r", ch );
    }
  }

  /* ��H��C�� */
  else if ( !str_prefix( arg1, "!notelist" ) )
  {
    /* �ˬd���� */
    if ( !( pClub = ch->club ) )
    {
      send_to_char( "�A�S���ѥ[�����M�L�k����H��C��C\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd���������A�O�_�i�HŪ���H�� */
    if ( pClub->status != CLUB_STATUS_UNIONIZE || !pClub->location )
    {
      send_to_char( "�{�b�٤����[�������H��C\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd�O�_��¾�v */
    if ( !right_ring( ch->name, pClub, CLUB_MEMBER ) )
    {
      send_to_char( "�A�S���v�O�h�������H��C��C\n\r", ch );
      RETURN_NULL();
    }

    if ( !ch->in_room || ch->in_room != pClub->location )
    {
      send_to_char( "�A���������|�E���a�~��ݪ��������H��C\n\r", ch );
      RETURN_NULL();
    }

    clear_buffer();

    send_to_char( "\e[1;33;44m�s �� Ū�� �H�H�H�P���D"
                  "                                 \e[0m\n\r", ch );
    for ( count = 0, pNote = note_list; pNote; pNote = pNote->next )
    {
      if ( is_note_to_club( pClub, pNote ) )
      {
        send_to_buffer( "[%3d] [%s] %s: %s\n\r"
          , ++count
          , YESNO( !( pNote->date_stamp > ch->last_clubnote
              && str_cmp( pNote->sender, ch->name ) ) )
          , pNote->sender
          , pNote->subject );

        if ( buffer_full() ) break;
      }
    }

    if ( count == 0 ) send_to_char( "���|���èS�����󪺫H��C\n\r", ch );
    else print_buffer( ch );
  }

  /* Ū�������H�� */
  else if ( !str_prefix( arg1, "!read" ) )
  {
    /* �ˬd���� */
    if ( !( pClub = ch->club ) )
    {
      send_to_char( "�A�S���ѥ[�����M�L�kŪ�������H��C\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd���������A�O�_�i�HŪ���H�� */
    if ( pClub->status != CLUB_STATUS_UNIONIZE || !pClub->location )
    {
      send_to_char( "�{�b�٤����[�������H��C\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd�O�_��¾�v */
    if ( !right_ring( ch->name, pClub, CLUB_MEMBER ) )
    {
      send_to_char( "�A�S���v�O�h�������H��C\n\r", ch );
      RETURN_NULL();
    }

    if ( !ch->in_room || ch->in_room != pClub->location )
    {
      send_to_char( "�A���������|�E���a�~��ݪ��������H��C\n\r", ch );
      RETURN_NULL();
    }

    /* �ˬd�Ѽ� */
    if ( !arg2[0] || ( count = atoi( arg2 ) ) <= 0 )
    {
      send_to_char( "�A�nŪ�ĴX�������H�S\n\r", ch );
      RETURN_NULL();
    }

    for ( pNote = note_list, loop = 0; pNote; pNote = pNote->next )
    {
      if ( is_note_to_club( pClub, pNote ) && ++loop == count )
      {
        ch->last_clubnote = UMAX( ch->last_clubnote, pNote->date_stamp );
        clear_buffer();

        send_to_buffer( "[%3d] �H�H��: %s\n\r"
                        "��  �D: %s\n\r"
                        "��  ��: %s\n\r"
                        "���H��: %s �����H\n\r"
                        "��  �e:\n\r%s"
           , loop
           , pNote->sender
           , pNote->subject
           , pNote->date
           , pNote->to_list + 1
           , pNote->text );
        print_buffer( ch );
        RETURN_NULL();
      }
    }

    send_to_char( "�S�����������H�C\n\r", ch );
  }

  else
  {
    send_to_char( "���O���~, �Ьd�� club ���O�C\n\r", ch );
  }

  RETURN_NULL();
}

/* �d�������W�� */
CLUB_DATA * clubname_lookup( const char * name, int type )
{
  CLUB_DATA * pClub;

  PUSH_FUNCTION( "clubname_lookup" );

  /* �קK���~ */
  if ( !name || *name == '\x0' ) RETURN( NULL );

  switch( type )
  {
  default:
    mudlog( LOG_DEBUG, "clubname_lookup: ���~���ﶵ %d.", type );
    break;

  /* �^��W�� */
  case CLUB_NAME:

    for ( pClub = club_list; pClub; pClub = pClub->next )
      if ( pClub->name && !str_cmp( name, pClub->name ) ) RETURN( pClub );

    break;

  /* ����W�� */
  case CLUB_CNAME:

    for ( pClub = club_list; pClub; pClub = pClub->next )
      if ( pClub->cname && !str_cmp( name, pClub->cname ) ) RETURN( pClub );

    break;
  }

  RETURN( NULL );
}

/* �гy���� */
CLUB_DATA * create_club( CHAR_DATA * ch, char * name, char * cname )
{
  CLUB_DATA * pClub;
  CLUB_DATA * zClub;

  PUSH_FUNCTION( "create_club" );

  if ( !ch
    || IS_NPC( ch )
    || !name
    || !cname
    || *name  == '\x0'
    || *cname == '\x0'
    || club_count() >= max_club )
    RETURN( NULL );

  /* �����g�O */
  ch->bank = UMAX( 0, ch->bank - ClubCreateFund );
  set_club_default( pClub = alloc_struct( STRUCT_CLUB_DATA ) );

  set_club( pClub, CLUB_NAME  , name  , 0 );
  set_club( pClub, CLUB_CNAME , cname , 0 );
  set_club( pClub, CLUB_STATUS, NULL  , CLUB_STATUS_COUNTERSIGN );
  set_club( pClub, CLUB_TIMER , NULL  , time( NULL ) );

  /* �]�w���D��¾�� */
  char_to_club( ch->name, pClub, CLUB_MASTER );

  /* �]�w�s�� */
  for ( zClub = club_list;
        zClub && zClub->next;
        zClub = zClub->next );

  if ( !zClub ) club_list   = pClub;
  else          zClub->next = pClub;

  ch->club = pClub;

  RETURN( pClub );
}

/* �s�p���� */
bool countersign_club( CHAR_DATA * ch, CLUB_DATA * pClub )
{
  char buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "countersign_club" );

  if ( !ch || !pClub || !char_to_club( ch->name, pClub, CLUB_MEMBER ) )
    RETURN( FALSE );

  ch->club     = pClub;
  ch->clublog  = TRUE;
  gold_from_char( ch, UMIN( ch->gold, ClubCountersignFund ) );

  /* �s�p���\ */
  if ( count_club_member( pClub, CLUB_MEMBER ) >= ClubCountersignCount )
  {
    sprintf( buf, "%s\e[0m(%s)�s�p���\\�M�������ߡC\n\r"
      , pClub->cname, pClub->name );
    send_to_all_char( buf );

    /* �]�w���A������, �åB�s�y�X�@�������ж� */
    set_club( pClub, CLUB_STATUS, NULL, CLUB_STATUS_UNIONIZE );
    club_location( pClub );
  }

  RETURN( TRUE );
}

/* �ˬd�Y�H�O�_�b�������� */
CLUB_DATA * char_club( CHAR_DATA * ch )
{
  PUSH_FUNCTION( "char_club" );

  /* �קK���~ */
  if ( !ch || IS_NPC( ch ) ) RETURN( NULL );

  /* �Ǫ��O�S�������� */
  if ( IS_NPC( ch ) ) RETURN( ( ch->club = NULL ) );

  RETURN( ( ch->club = is_inclub( ch->name ) ) );
}

/* �W�٬O�_�b���������F��? */
CLUB_DATA * is_inclub( char * name )
{
  CLUB_DATA * pClub;

  PUSH_FUNCTION( "is_inclub" );

  /* �קK���~ */
  if ( !name || *name == '\x0' ) RETURN( NULL );

  for ( pClub = club_list; pClub; pClub = pClub->next )
    if ( name_in_club( name, pClub ) != CLUB_NO_RELATION ) RETURN( pClub );

  RETURN( NULL );
}

/* ���լO�_�Y�H�b�������� */
int name_in_club( char * name, CLUB_DATA * pClub )
{
  int loop;

  PUSH_FUNCTION( "name_in_club" );

  /* �קK���~ */
  if ( !name || *name == '\x0' || !pClub ) RETURN( CLUB_NO_RELATION );

  /* ���D */
  if ( pClub->master && !str_cmp( name, pClub->master ) )
    RETURN( CLUB_MASTER );

  /* �����D */
  if ( pClub->vicemaster && !str_cmp( name, pClub->vicemaster ) )
    RETURN( CLUB_VICEMASTER  );

  /* ���� */
  for ( loop = 0; loop < MAX_CLUB_DOYEN; loop++ )
  {
    if ( pClub->doyen[loop] && !str_cmp( name, pClub->doyen[loop] ) )
      RETURN( CLUB_DOYEN );
  }

  /* �����Χ̤l */
  for ( loop = 0; loop < MAX_CLUB_MEMBER; loop++ )
  {
    if ( pClub->member[loop] && !str_cmp( name, pClub->member[loop] ) )
    {
      if ( pClub->verify[loop] ) RETURN( CLUB_MEMBER );
      RETURN( CLUB_FOLLOWER )
    }
  }

  RETURN( CLUB_NO_RELATION );
}

/* ��Y�H�[�J��������椧�� */
bool char_to_club( char * name, CLUB_DATA * pClub, int type )
{
  int         loop;
  CHAR_DATA * victim;

  PUSH_FUNCTION( "char_to_club" );

  /* �קK���~ */
  if ( !name || *name == '\x0' || !pClub ) RETURN( FALSE );

  /* �������X�k���r�� */
  smash_tilde( name );

  victim = is_online( name );

  switch( type )
  {
  default:
    mudlog( LOG_DEBUG, "char_to_club: ���~���ﶵ %d.", type );
    break;

  /* ���D */
  case CLUB_MASTER:

    if ( clubmember_full( pClub, CLUB_MASTER ) )
    {
      mudlog( LOG_DEBUG, "char_to_club: �w�g�����D" );
      break;
    }

    pClub->master = str_dup( name );
    if ( victim ) victim->club = pClub;
    RETURN( TRUE );

  /* �����D */
  case CLUB_VICEMASTER:

    if ( clubmember_full( pClub, CLUB_VICEMASTER ) )
    {
      mudlog( LOG_DEBUG, "char_to_club: �w�g�������D" );
      break;
    }

    pClub->vicemaster = str_dup( name );
    if ( victim ) victim->club = pClub;
    RETURN( TRUE );

  /* ���� */
  case CLUB_DOYEN:

    if ( clubmember_full( pClub, CLUB_DOYEN ) )
    {
      mudlog( LOG_DEBUG, "char_to_club: ���ѦW�B�w��." );
      break;
    }

    for ( loop = 0; loop < MAX_CLUB_DOYEN; loop++ )
    {
      if ( !pClub->doyen[loop] )
      {
        pClub->doyen[loop] = str_dup( name );
        if ( victim ) victim->club = pClub;
        RETURN( TRUE );
      }
    }

    mudlog( LOG_DEBUG, "char_to_club: ���ѦW�B�w��." );
    break;

  /* ���� */
  case CLUB_MEMBER:

    if ( clubmember_full( pClub, CLUB_MEMBER ) )
    {
      mudlog( LOG_DEBUG, "char_to_club: �����W�B�w��." );
      break;
    }

    for ( loop = 0; loop < MAX_CLUB_MEMBER; loop++ )
    {
      if ( !pClub->member[loop] )
      {
        pClub->member[loop] = str_dup( name );
        pClub->verify[loop] = TRUE;
        if ( victim ) victim->club = pClub;
        RETURN( TRUE );
      }
    }

    mudlog( LOG_DEBUG, "char_to_club: �����W�B�w��." );
    break;

  /* �̤l */
  case CLUB_FOLLOWER:

    if ( clubmember_full( pClub, CLUB_FOLLOWER ) )
    {
      mudlog( LOG_DEBUG, "char_to_club: �����W�B�w��." );
      break;
    }

    for ( loop = 0; loop < MAX_CLUB_MEMBER; loop++ )
    {
      if ( !pClub->member[loop] )
      {
        pClub->member[loop] = str_dup( name );
        pClub->verify[loop] = FALSE;
        if ( victim ) victim->club = pClub;
        RETURN( TRUE );
      }
    }

    mudlog( LOG_DEBUG, "char_to_club: �����W�B�w��." );
    break;
  }

  RETURN( FALSE );
}

/* ��Y�ӤH�q���|�������� */
bool char_from_club( char * name, CLUB_DATA * pClub, int type )
{
  int         loop;
  CHAR_DATA * victim;

  PUSH_FUNCTION( "char_from_club" );

  /* �]�w CLUB_ALL �۰ʧ�M��¾�� */
  if ( type == CLUB_ALL ) type = name_in_club( name, pClub );

  victim = is_online( name );

  switch( type )
  {
  /* ���~���ﶵ */
  default:
    mudlog( LOG_DEBUG, "char_from_club: ���~���ﶵ %d.", type );
    break;

  /* ���D */
  case CLUB_MASTER:

    if ( !clubmember_full( pClub, CLUB_MASTER ) )
    {
      mudlog( LOG_DEBUG, "char_from_club: ���D�O�Ū�." );
      break;
    }

    if ( str_cmp( name, pClub->master ) )
    {
      mudlog( LOG_DEBUG, "char_from_club: %s ���O %s�����D."
        , name, pClub->name );
      break;
    }

    free_string( pClub->master );
    pClub->master = NULL;
    if ( victim ) victim->club = NULL;
    RETURN( TRUE );

  /* �����D */
  case CLUB_VICEMASTER:

    if ( !clubmember_full( pClub, CLUB_VICEMASTER ) )
    {
      mudlog( LOG_DEBUG, "char_from_club: �����D�O�Ū�." );
      break;
    }

    if ( str_cmp( name, pClub->vicemaster ) )
    {
      mudlog( LOG_DEBUG, "char_from_club: %s ���O %s �������D."
        , name, pClub->name );
      break;
    }

    free_string( pClub->vicemaster );
    pClub->vicemaster = NULL;
    if ( victim ) victim->club = NULL;
    RETURN( TRUE );

  /* ���� */
  case CLUB_DOYEN:

    for ( loop = 0; loop < MAX_CLUB_DOYEN; loop++ )
    {
      if ( pClub->doyen[loop] && !str_cmp( name, pClub->doyen[loop] ) )
      {
        free_string( pClub->doyen[loop] );
        pClub->doyen[loop] = NULL;
        if ( victim ) victim->club = NULL;
        RETURN( TRUE );
      }
    }

    mudlog( LOG_DEBUG, "char_from_club: %s ���O %s ������."
      , name, pClub->name );
    break;

  /* �����άO�̤l */
  case CLUB_MEMBER:
  case CLUB_FOLLOWER:

    for ( loop = 0; loop < MAX_CLUB_MEMBER; loop++ )
    {
      if ( pClub->member[loop] && !str_cmp( name, pClub->member[loop] ) )
      {
        free_string( pClub->member[loop] );
        pClub->member[loop] = NULL;
        pClub->verify[loop] = FALSE;
        if ( victim ) victim->club = NULL;
        RETURN( TRUE );
      }
    }

    mudlog( LOG_DEBUG, "char_from_club: %s ���O %s �������Χ̤l."
      , name, pClub->name );
    break;
  }

  RETURN( FALSE );
}

/* ���R����¾�� */
bool appoint_member( char * name, CLUB_DATA * pClub, int level )
{
  int orginal;

  PUSH_FUNCTION( "appoint_member" );

  /* �קK���~ */
  if ( !name
    || *name == '\x0'
    || !pClub
    || ( orginal = name_in_club( name, pClub ) ) == CLUB_NO_RELATION
    || orginal == level ) RETURN( FALSE );

  switch( level )
  {
  default:
    mudlog( LOG_DEBUG, "appoint_member: ���~��¾�� %d.", level );
    break;

  /* ���D */
  case CLUB_MASTER:

    /* �ݬݬO�_�W�B�w��, �H�Υi�H�[�J�s��¾�~��椧�� */
    if ( clubmember_full( pClub, CLUB_MASTER )
      || !char_to_club( name, pClub, CLUB_MASTER ) )
    {
      mudlog( LOG_DEBUG, "appoint_member: ���R���D����." );
      break;
    }

    RETURN( char_from_club( name, pClub, orginal ) );

  /* �����D */
  case CLUB_VICEMASTER:

    /* �ݬݬO�_�W�B�w��, �H�Υi�H�[�J�s��¾�~��椧�� */
    if ( clubmember_full( pClub, CLUB_VICEMASTER )
      || !char_to_club( name, pClub, CLUB_VICEMASTER ) )
    {
      mudlog( LOG_DEBUG, "appoint_member: ���R�����D����." );
      break;
    }

    RETURN( char_from_club( name, pClub, orginal ) );

  /* ���� */
  case CLUB_DOYEN:

    /* �ݬݬO�_�W�B�w��, �H�Υi�H�[�J�s��¾�~��椧�� */
    if ( clubmember_full( pClub, CLUB_DOYEN )
      || !char_to_club( name, pClub, CLUB_DOYEN ) )
    {
      mudlog( LOG_DEBUG, "appoint_member: ���R���ѥ���." );
      break;
    }

    RETURN( char_from_club( name, pClub, orginal ) );

  /* ���� */
  case CLUB_MEMBER:

    /* �ݬݬO�_�W�B�w��, �H�Υi�H�[�J�s��¾�~��椧�� */
    if ( clubmember_full( pClub, CLUB_MEMBER )
      || !char_to_club( name, pClub, CLUB_MEMBER ) )
    {
      mudlog( LOG_DEBUG, "appoint_member: ���R��������." );
      break;
    }

    RETURN( char_from_club( name, pClub, orginal ) );

  /* �̤l */
  case CLUB_FOLLOWER:

    /* �ݬݬO�_�W�B�w��, �H�Υi�H�[�J�s��¾�~��椧�� */
    if ( clubmember_full( pClub, CLUB_FOLLOWER )
      || !char_to_club( name, pClub, CLUB_FOLLOWER ) )
    {
      mudlog( LOG_DEBUG, "appoint_member: ���R�̤l����." );
      break;
    }

    RETURN( char_from_club( name, pClub, orginal ) );
  }

  RETURN( FALSE );
}

/* �p���������Y¾�쪺�H�� */
size_t count_club_member( CLUB_DATA * pClub, int type )
{
  size_t cMaster;
  size_t cVicemaster;
  size_t cDoyen;
  size_t cMember;
  size_t cFollower;
  int    loop;

  PUSH_FUNCTION( "count_club_member" );

  if ( !pClub ) RETURN( 0 );

  cMaster     = ( pClub->master     ) ? 1 : 0;
  cVicemaster = ( pClub->vicemaster ) ? 1 : 0;

  for ( cDoyen = loop = 0; loop < MAX_CLUB_DOYEN; loop++ )
    if ( pClub->doyen[loop] ) cDoyen++;

  for ( cMember = loop = 0; loop < MAX_CLUB_MEMBER; loop++ )
    if ( pClub->member[loop] && pClub->verify[loop] )
      cMember++;

  for ( cFollower = loop = 0; loop < MAX_CLUB_MEMBER; loop++ )
    if ( pClub->member[loop] && !pClub->verify[loop] )
      cFollower++;

  switch( type )
  {
  default:
    mudlog( LOG_DEBUG, "count_club_member: ���~��¾�� %d.", type );
    break;

  /* ���D */
  case CLUB_MASTER:
    RETURN( cMaster );

  /* �����D */
  case CLUB_VICEMASTER:
    RETURN( cVicemaster );

  /* ���� */
  case CLUB_DOYEN:
    RETURN( cDoyen );

  /* ���� */
  case CLUB_MEMBER:
    RETURN( cMember );

  /* �̤l */
  case CLUB_FOLLOWER:
    RETURN( cFollower );

  /* �����Ҧ����H */
  case CLUB_ALL:
    RETURN( cMaster + cVicemaster + cDoyen + cMember + cFollower);
  }

  RETURN( 0 );
}

/* �����������Y¾��O�_�w�g�B�� */
bool clubmember_full( CLUB_DATA * pClub, int type )
{
  int loop;

  PUSH_FUNCTION( "clubmember_full" );

  if ( !pClub ) RETURN( TRUE );

  switch( type )
  {
  default:
    mudlog( LOG_DEBUG, "clubmember_full: ���~��¾�� %d.", type );
    break;

  /* ���D */
  case CLUB_MASTER:
    RETURN( pClub->master != NULL );

  /* �����D */
  case CLUB_VICEMASTER:
    RETURN( pClub->vicemaster != NULL );

  /* ���� */
  case CLUB_DOYEN:

    for ( loop = 0; loop < MAX_CLUB_DOYEN; loop++ )
      if ( !pClub->doyen[loop] ) RETURN( FALSE );
    break;

  /* �����Χ̤l */
  case CLUB_MEMBER:
  case CLUB_FOLLOWER:

    for ( loop = 0; loop < MAX_CLUB_MEMBER; loop++ )
      if ( !pClub->member[loop] ) RETURN( FALSE );
    break;
  }

  RETURN( TRUE );
}

/* �O�_�b�u�W */
CHAR_DATA * is_online( const char * name )
{
  DESCRIPTOR_DATA * man;

  PUSH_FUNCTION( "is_online" );

  if ( name && *name )
  {
    for ( man = descriptor_list; man; man = man->next )
    {
      if ( verify_desc( man )
        && man->connected == CON_PLAYING
        && man->character
        && man->character->name
        && !str_cmp( name, man->character->name ) ) RETURN( man->character );
    }
  }

  RETURN( NULL );
}

/* �Y�H�������W�� */
char * club_name( CHAR_DATA * ch )
{
  CLUB_DATA * pClub;

  PUSH_FUNCTION( "club_name" );
  if ( !ch ) RETURN( "����" );
  if ( IS_NPC( ch ) || !( pClub = ch->club ) ) RETURN( "�S������" );
  RETURN( pClub->cname );
}

/* ���a�i�J�C���ɨ��o��������� */
void char_attach_club( CHAR_DATA * ch )
{
  PUSH_FUNCTION( "char_attach_club" );

  if ( !ch || IS_NPC( ch ) ) RETURN_NULL();
  ch->club = char_club( ch );

  RETURN_NULL();
}

/* �v�O���� */
bool right_ring( char * name, CLUB_DATA * pClub, int right )
{
  PUSH_FUNCTION( "right_ring" );

  if ( !name || *name == '\x0' || !pClub ) RETURN( FALSE );

  switch( right )
  {
  default:
    RETURN( FALSE );

  case CLUB_MASTER:
  case CLUB_VICEMASTER:
  case CLUB_DOYEN:
  case CLUB_MEMBER:
  case CLUB_FOLLOWER:
    break;
  }

  RETURN ( ( name_in_club( name, pClub ) >= right ) );
}

/* �O�_�P�@������ */
bool is_same_club( CHAR_DATA * ch, CHAR_DATA * victim )
{
  PUSH_FUNCTION( "is_same_club" );

  if ( !ch
     || !victim
     || IS_NPC( ch )
     || IS_NPC( victim )
     || !ch->club
     || !victim->club ) RETURN( FALSE );

  RETURN( ch->club == victim->club );
}

/* �Ѵ����� */
bool fordo_club( CLUB_DATA * pClub )
{
  CLUB_DATA * prev;
  CHAR_DATA * victim;
  int         loop;

  PUSH_FUNCTION( "fordo_club" );

  if ( !pClub ) RETURN( FALSE );

  if ( pClub == club_list )
  {
    club_list = pClub->next;
  }

  else
  {
    for ( prev = club_list; prev; prev = prev->next )
    {
      if ( prev->next == pClub )
      {
        prev->next = pClub->next;
        break;
      }
    }

    if ( !prev )
    {
      mudlog( LOG_DEBUG , "fordo_club: �å��o�{����P��������." );
      RETURN( FALSE );
    }
  }

  free_string( pClub->name  );
  free_string( pClub->cname );

  if ( ( victim = is_online( pClub->master ) ) ) victim->club = NULL;
  free_string( pClub->master );

  if ( ( victim = is_online( pClub->vicemaster ) ) ) victim->club = NULL;
  free_string( pClub->vicemaster );

  for ( loop = 0; loop < MAX_CLUB_DOYEN; loop++ )
  {
    if ( ( victim = is_online( pClub->doyen[loop] ) ) ) victim->club = NULL;
    free_string( pClub->doyen[loop] );
  }

  for ( loop = 0; loop < MAX_CLUB_MEMBER; loop++ )
  {
    if ( ( victim = is_online( pClub->member[loop] ) ) ) victim->club = NULL;
    free_string( pClub->member[loop] );
  }

  free_struct( pClub, STRUCT_CLUB_DATA );
  RETURN( TRUE );
}

/* �����ƥ� */
size_t club_count( void )
{
  CLUB_DATA * pClub;
  size_t      count;

  PUSH_FUNCTION( "club_count" );
  for ( count = 0, pClub = club_list; pClub; pClub = pClub->next ) count++;
  RETURN( count );
}

/* �Ǧ^�����ǰe�I */
ROOM_INDEX_DATA * club_recall( CHAR_DATA * ch )
{
  PUSH_FUNCTION( "club_recall" );

  if ( ch
    && !IS_NPC( ch )
    && ch->club
    && name_in_club( ch->name, ch->club ) > CLUB_FOLLOWER )
    RETURN( ch->club->location );

  RETURN( NULL );
}

/* �إ������E���I */
ROOM_INDEX_DATA * club_location( CLUB_DATA * pClub )
{
  ROOM_INDEX_DATA * pRoom;
  EXIT_DATA       * pExit;
  char              buf[MAX_STRING_LENGTH];
  int               iHash;
  int               loop;

  PUSH_FUNCTION( "club_location" );

  if ( !pClub || pClub->status != CLUB_STATUS_UNIONIZE ) RETURN( NULL );

  /* �إ߰_�ж� */
  set_room_default( pRoom = alloc_struct( STRUCT_ROOM_INDEX_DATA ) );

  pRoom->people          = NULL;
  pRoom->contents        = NULL;
  pRoom->extra_descr     = NULL;
  pRoom->club            = pClub;
  pRoom->area            = DefaultArea;

  sprintf( buf, "%s \e[0m(%s)�����E���I\e[0m", pClub->cname, pClub->name );
  pRoom->name            = str_dup( buf );

  sprintf( buf,
    "�o�̬O %s\e[0m (%s)���������E���I�M�A�ݨ�\\�h�H�Өӥh�h�C\n\r"
    "�᭱���@�u�j�c�l�M�̭��s��ۦU���U�˪��˳ơM�����������A�i�H�ϥ�\n\r"
    "\e[1;32mstore\e[0m �o�ӫ��O�Ӧs���T\n\r\n\r"
    , pClub->cname, pClub->name );

  pRoom->description     = str_dup( buf );

  pRoom->filename        = str_dup( "�t�Ϋإ�" );
  pRoom->vnum            = club_room++;
  pRoom->Safe            = TRUE;
  pRoom->NoRecall        = TRUE;
  pRoom->NoWhere         = TRUE;
  pRoom->NoQuit          = TRUE;
  pRoom->NoMob           = TRUE;
  pRoom->Club            = TRUE;
  pRoom->NoLink          = TRUE;
  pRoom->light           = 0;
  pRoom->sector          = DefaultSector;
  iHash                  = pRoom->vnum % MAX_KEY_HASH;
  pRoom->next            = room_index_hash[iHash];
  room_index_hash[iHash] = pRoom;
  pClub->location        = pRoom;

  top_room++;
  for ( loop = 0; loop < DIR_MAX; loop++ ) pRoom->exit[loop] = NULL;

  /* �إ߰_�V�U���X�f */
  set_exit_default( pExit = alloc_struct( STRUCT_EXIT_DATA ) );
  pExit->description    = str_dup( "" );
  pExit->keyword        = str_dup( "" );
  pExit->closed         = FALSE;
  pExit->pick           = FALSE;
  pExit->isdoor         = FALSE;
  pExit->locked         = FALSE;
  pExit->key            = 0;
  pExit->vnum           = RoomRecall->vnum;
  pRoom->exit[ DIR_DOWN ] = pExit;
  pExit->message        = str_dup( "�A���X�F�A�������E���I�T" );

  RETURN( pRoom );
}

/* ���|�H�� */
size_t club_notes( CHAR_DATA * ch )
{
  NOTE_DATA * pNote;
  CLUB_DATA * pClub;
  size_t      count;

  PUSH_FUNCTION( "club_notes" );

  if ( !ch || !( pClub = ch->club ) ) RETURN( 0 );

  for ( count = 0, pNote = note_list; pNote; pNote = pNote->next )
    if ( is_note_to_club( pClub, pNote ) ) count++;

  RETURN( count );
}

/* ���|��Ū�H�� */
size_t club_unread( CHAR_DATA * ch )
{
  NOTE_DATA * pNote;
  CLUB_DATA * pClub;
  size_t      count;

  PUSH_FUNCTION( "club_notes" );

  if ( !ch || !( pClub = ch->club ) ) RETURN( 0 );

  for ( count = 0, pNote = note_list; pNote; pNote = pNote->next )
    if ( is_note_to_club( pClub, pNote )
      && pNote->date_stamp > ch->last_clubnote ) count++;

  RETURN( count );
}

/* �ݬݳo�ʫH�쩳�O���O�o���������H�� */
bool is_note_to_club( CLUB_DATA * pClub, NOTE_DATA * pNote )
{
  PUSH_FUNCTION( "is_note_to_club" );

  if ( pNote
    && pClub
    && pClub->name
    && pNote->to_list[0] == '@'
    && pNote->to_list[1] )
    RETURN( !str_cmp( pClub->name, ( pNote->to_list ) + 1 ) );

  RETURN( FALSE );
}

/* �]�w������� */
bool set_club( CLUB_DATA * pClub, int type, char * pString, int iNumber )
{
  PUSH_FUNCTION( "set_club" );

  /* �קK���~ */
  if ( !pClub ) RETURN( FALSE );

  switch ( type )
  {
  default:
    mudlog( LOG_DEBUG, "set_club: ���~���ﶵ %d.", type );
    break;

  /* ��������^��W�� */
  case CLUB_NAME:

    /* �Y�O�Ŧr��h�O���~ */
    if ( !pString || *pString == '\x0' ) break;

    filter_ansi( pString );
    smash_tilde( pString );
    free_string( pClub->name );
    pClub->name = str_dup( pString );
    RETURN( TRUE );

  /* �����������W�� */
  case CLUB_CNAME:

    /* �Y�O�Ŧr��h�O���~ */
    if ( !pString || *pString == '\x0' ) break;

    filter_ansi( pString );
    smash_tilde( pString );
    free_string( pClub->cname );
    pClub->cname = str_dup( pString );
    RETURN( TRUE );

  /* ����������� */
  case CLUB_MONEY:

    if ( iNumber >= 0 )
    {
      pClub->money = iNumber;
      RETURN( TRUE );
    }

    mudlog( LOG_DEBUG, "set_club: �ƥؤ��X�z %d.", iNumber );
    break;

  /* ����������A */
  case CLUB_STATUS:

    if ( iNumber == CLUB_STATUS_COUNTERSIGN
      || iNumber == CLUB_STATUS_UNIONIZE )
    {
      pClub->status = iNumber;
      RETURN( TRUE );
    }

    mudlog( LOG_DEBUG, "set_club: ���~�����A %d.", iNumber );
    break;

  /* ��������ɶ� */
  case CLUB_TIMER:

    if ( iNumber >= 0 )
    {
      pClub->timer = iNumber;
      RETURN( TRUE );
    }

    mudlog( LOG_DEBUG, "set_club: �ɶ����X�z %d.", iNumber );
    break;
  }

  RETURN( FALSE );
}

/* �o����������¾�� */
char * is_club_class( CHAR_DATA * ch )
{
  CLUB_DATA * pClub;

  PUSH_FUNCTION( "is_club_class" );

  if ( !ch ) RETURN( "����" );
  if ( IS_NPC( ch ) || !( pClub = ch->club ) ) RETURN( "�C��" );

  if ( ch && pClub )
  {
    switch( name_in_club( ch->name, pClub ) )
    {
    default:              break;
    case CLUB_MASTER:     RETURN( "���D"   );
    case CLUB_VICEMASTER: RETURN( "�����D" );
    case CLUB_DOYEN:      RETURN( "����"   );
    case CLUB_MEMBER:     RETURN( "����"   );
    case CLUB_FOLLOWER:   RETURN( "�̤l"   );
    }
  }

  RETURN( "����" );
}

/* �o����������¾�� */
char * club_class( CHAR_DATA * ch, CLUB_DATA * pClub )
{
  PUSH_FUNCTION( "club_class" );

  if ( ch && pClub )
  {
    switch( name_in_club( ch->name, pClub ) )
    {
    default:              break;
    case CLUB_MASTER:     RETURN( "���D"   );
    case CLUB_VICEMASTER: RETURN( "�����D" );
    case CLUB_DOYEN:      RETURN( "����"   );
    case CLUB_MEMBER:     RETURN( "����"   );
    case CLUB_FOLLOWER:   RETURN( "�̤l"   );
    }
  }

  RETURN( "����" );
}

bool name_from_club( const char * name, bool fPrint )
{
  CLUB_DATA * pClub;
  CLUB_DATA * temp;
  CLUB_DATA * prev;
  CHAR_DATA * victim;
  CHAR_DATA * member;
  char        buf[MAX_STRING_LENGTH];
  int         loop;

  PUSH_FUNCTION( "name_from_club" );

  if ( !name || !*name ) RETURN( FALSE );

  victim = is_online( name );

  for ( pClub = club_list; pClub; pClub = pClub->next )
  {
    if ( pClub->master && !str_cmp( pClub->master, name ) )
    {
      if ( fPrint )
      {
        sprintf( buf, "�H�����D%s�������M%s\e[0m(%s)�����]���J�F���v�M"
          "�A�]�S���o�������F�T"
          , pClub->master, pClub->cname, pClub->name );

        talk_channel_2( buf, CHANNEL_BULLETIN, "���i" );
      }

      for ( member = char_list; member; member = member->next )
      {
        if ( verify_char( member ) && member->club == pClub )
          member->club = NULL;
      }

      prev = NULL;
      for ( temp = club_list; temp; prev = temp, temp = temp->next )
      {
        if ( temp == pClub )
        {
          if ( !prev ) club_list  = club_list->next;
          else         prev->next = pClub->next;

          free_string( pClub->name       );
          free_string( pClub->cname      );
          free_string( pClub->master     );
          free_string( pClub->vicemaster );

          for ( loop = 0; loop < MAX_CLUB_DOYEN; loop++ )
            free_string( pClub->doyen[loop] );

          for ( loop = 0; loop < MAX_CLUB_MEMBER; loop++ )
            free_string( pClub->member[loop] );

          free_struct( pClub , STRUCT_CLUB_DATA );
          break;
        }
      }

      RETURN( TRUE );
    }

    if ( pClub->vicemaster && !str_cmp( pClub->vicemaster, name ) )
    {
      if ( fPrint )
      {
        sprintf( buf, "���������D%s�w�ϭ��Ƿ��M�o�t����F�T\n\r"
          , pClub->vicemaster );

        send_to_club( buf, pClub );
      }

      free_string( pClub->vicemaster );
      pClub->vicemaster = NULL;
      if ( victim ) victim->club = NULL;

      RETURN( TRUE );
    }

    for ( loop = 0; loop < MAX_CLUB_DOYEN; loop++ )
    {
      if ( pClub->doyen[loop] && !str_cmp( pClub->doyen[loop], name ) )
      {
        if ( fPrint )
        {
          sprintf( buf, "��������%s�������}�H�@�M�L�w�����v�̨ѫ���q���T\n\r"
            , name );

          send_to_club( buf, pClub );
        }

        free_string( pClub->doyen[loop] );
        pClub->doyen[loop] = NULL;
        if ( victim ) victim->club = NULL;

        RETURN( TRUE );
      }
    }

    for ( loop = 0; loop < MAX_CLUB_MEMBER; loop++ )
    {
      if ( pClub->member[loop] && !str_cmp( pClub->member[loop], name ) )
      {
        if ( fPrint )
        {
          sprintf( buf, "%s%s�w���}�ڭ̪��v��U�M���M�h���^�~�T\n\r"
            , pClub->member[loop] ? "����" : "�̤l", name );

          send_to_club( buf, pClub );
        }

        free_string( pClub->member[loop] );
        pClub->member[loop] = NULL;
        pClub->verify[loop] = FALSE;
        if ( victim ) victim->club = NULL;

        RETURN( TRUE );
      }
    }
  }

  RETURN( FALSE );
}

void club_sort( CLUB_DATA * pClub )
{
  int          loop_1;
  int          loop_2;
  int          level;
  int          doyen[MAX_CLUB_DOYEN];
  int          member[MAX_CLUB_MEMBER];
  char       * temp;
  BASIC_DATA * pBasic;

  PUSH_FUNCTION( "club_sort" );

  for ( loop_1 = 0; loop_1 < MAX_CLUB_DOYEN; loop_1++ )
  {
    if ( pClub->doyen[loop_1]
      && *( pClub->doyen[loop_1] )
      && ( pBasic = name2basic( pClub->doyen[loop_1] ) ) )
    {
      doyen[loop_1] = pBasic->level;
      free_basic( pBasic );
    }
    else
    {
      doyen[loop_1] = -MAX_LEVEL;
    }
  }

  for ( loop_1 = 0; loop_1 < MAX_CLUB_DOYEN - 1; loop_1++ )
  {
    for ( loop_2 = loop_1 + 1; loop_2 < MAX_CLUB_DOYEN; loop_2++ )
    {
      if ( doyen[loop_1] < doyen[loop_2] )
      {
        temp                 = pClub->doyen[loop_2];
        pClub->doyen[loop_2] = pClub->doyen[loop_1];
        pClub->doyen[loop_1] = temp;

        level         = doyen[loop_2];
        doyen[loop_2] = doyen[loop_1];
        doyen[loop_1] = level;
      }
    }
  }

  for ( loop_1 = 0; loop_1 < MAX_CLUB_MEMBER; loop_1++ )
  {
    if ( pClub->member[loop_1]
      && *( pClub->member[loop_1] )
      && ( pBasic = name2basic( pClub->member[loop_1] ) ) )
    {
      member[loop_1] = pBasic->level
                     + ( pClub->verify[loop_1] ? MAX_LEVEL * 2 : 0 );

      free_basic( pBasic );
    }
    else
    {
      member[loop_1] = -MAX_LEVEL * 4;
    }
  }

  for ( loop_1 = 0; loop_1 < MAX_CLUB_MEMBER - 1; loop_1++ )
  {
    for ( loop_2 = loop_1 + 1; loop_2 < MAX_CLUB_MEMBER; loop_2++ )
    {
      if ( member[loop_1] < member[loop_2] )
      {
        temp                  = pClub->member[loop_2];
        pClub->member[loop_2] = pClub->member[loop_1];
        pClub->member[loop_1] = temp;

        level                 = pClub->verify[loop_2];
        pClub->verify[loop_2] = pClub->verify[loop_1];
        pClub->verify[loop_1] = level;

        level          = member[loop_2];
        member[loop_2] = member[loop_1];
        member[loop_1] = level;
      }
    }
  }

  RETURN_NULL();
}

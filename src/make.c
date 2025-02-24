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
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include "merc.h"

ROOM_INDEX_DATA * create_new_room args( ( void ) );
bool              write_new_room  args( ( ROOM_INDEX_DATA * ) );

bool              LockCreateHome        =       FALSE;

FUNCTION( do_home )
{
  extern const sh_int   rev_dir [];
  ROOM_INDEX_DATA     * pRoom;
  ROOM_INDEX_DATA     * pHome;
  CHAR_DATA           * pMob;
  CHAR_DATA           * vch;
  CHAR_DATA           * fch;
  CHAR_DATA           * fch_next;
  EXIT_DATA           * aExit;
  EXIT_DATA           * bExit;
  int                   count;
  int                   loop;
  int                   dir;
  char                  arg[MAX_INPUT_LENGTH];
  char                  buf[MAX_STRING_LENGTH];
  char                  pellet[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "do_home" );

  if ( IS_NPC( ch ) || !ch->in_room ) RETURN_NULL();

  if ( !PlayerZone )
  {
    act( "$t�̨S�����󪱮a�g���ϰ�T", ch, mud_name, NULL, TO_CHAR );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' || !str_prefix( arg, "list" ) )
  {
    count = 0;
    clear_buffer();

    for ( pRoom = PlayerZone->list; pRoom; pRoom = pRoom->next_in_area )
    {
      if ( is_owner( ch, pRoom, TRUE ) )
      {
        if ( count <= 0 )
          send_to_buffer( "\e[1;33;44m���� �ж����X �ж��W��                 "
           "                                         \e[0m\n\r" );

        send_to_buffer( "%4d %8d %s\n\r", ++count, pRoom->vnum, pRoom->name );
        if ( buffer_full() ) break;
      }
    }

    if ( count <= 0 )
      send_to_buffer( "�藍�_�M�A�S���{�ʥ��󪺩Фl�g�a�T\n\r" );

    print_buffer( ch );
  }

  else if ( !str_prefix( arg, "practice" ) )
  {
    if ( !MobPractice )
    {
      send_to_char( "�ثe�t�ΨS���m�ߩǪ��M�и򯫱��p���T\n\r", ch );
      RETURN_NULL();
    }

    if ( !IS_NPC( ch ) && ch->level < level_limit )
    {
      act( "�b�A�ɯŨ�$i�ūe�M�Ф��n�ê���T", ch, &level_limit, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch->donate > 0 )
    {
      send_to_char( "�b�A����٪����o�q�ɶ��M�Ф��n�ê���T\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->bank < HomeMobile )
    {
      act( "$n�Ȧ�̥�����$i������~�ಣ�ͽm�ߩǪ��C"
        , ch, &HomeMobile, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch->position != POS_STANDING )
    {
      send_to_char( "������䪺�Ʊ������F�A���a�T\n\r", ch );
      RETURN_NULL();
    }

    if ( !( fch = owner_name( pRoom = ch->in_room ) ) || fch != ch )
    {
      send_to_char( "�藍�_�M�o���O�A���p���]���M�S��k���A�èӡT\n\r", ch );
      RETURN_NULL();
    }

    if ( !pRoom->Upgrade || !pRoom->PracRoom )
    {
      send_to_char( "�o�ж��èS���A�Q�n���\\��T\n\r", ch );
      RETURN_NULL();
    }

    if ( pRoom->mob_reborn > 0 )
    {
      send_to_char( "�Ǫ����s���ΡM�Э@�ߵ��ԡT\n\r", ch );
      RETURN_NULL();
    }

    for ( vch = pRoom->people; vch; vch = vch->next_in_room )
    {
      if ( !vch || !verify_char( vch ) ) continue;
      if ( IS_NPC( vch ) ) break;
    }

    if ( vch )
    {
      send_to_char( "�֧Ⲵ�e���ĤH���F�a�T\n\r", ch );
      RETURN_NULL();
    }

    if ( !( pMob = create_mobile( MobPractice, ch->level ) ) )
    {
      send_to_char( "�L�k���ͩǪ��M�j�������a���F�T\n\r", ch );
      mudlog( LOG_DEBUG, "do_home: Practice �L�k���ͩǪ�." );
      RETURN_NULL();
    }

    char_to_room( pMob, pRoom );
    act( "$N��M�X�{�b�o�̡M���۳Q�����T", ch, NULL, pMob, TO_ALL );

    /* �����Ȧ檺�� */
    ch->bank -= ch->level * HomeMobile;

    /* �]�w�ж����ͮɶ� */
    pRoom->mob_reborn = ch->level / 10;

    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "upgrade" ) )
  {
    if ( !IS_NPC( ch ) && ch->level < level_limit )
    {
      act( "�b�A�ɯŨ�$i�ūe�M�Ф��n�ê���T"
        , ch, &level_limit, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch->donate > 0 )
    {
      send_to_char( "�b�A����٪����o�q�ɶ��M�Ф��n�ê���T\n\r", ch );
      RETURN_NULL();
    }

    argument = one_argument( argument, arg );

    if ( !arg[0] )
    {
      send_to_char( "�A�Q�n�ɯŬ�����ж��O�S\n\r", ch );
      RETURN_NULL();
    }

    if ( !str_prefix( arg, "waterful" ) )
    {
      if ( ch->bank < HomeObject )
      {
        act( "$n�Ȧ�̥�����$i������~��ɯŦh���\\��C"
          , ch, &HomeObject, NULL, TO_CHAR );
        RETURN_NULL();
      }

      if ( ch->position != POS_STANDING )
      {
        send_to_char( "������䪺�Ʊ������F�A���a�T\n\r", ch );
        RETURN_NULL();
      }

      if ( !( fch = owner_name( pRoom = ch->in_room ) ) || fch != ch )
      {
        send_to_char( "�藍�_�M�o���O�A���p���]���M�S��k���A��~�T\n\r", ch );
        RETURN_NULL();
      }

      if ( pRoom->Upgrade )
      {
        act( "$r�w�g����A��ˤF�T", ch, NULL, NULL, TO_CHAR );
        RETURN_NULL();
      }

      pRoom->Waterful = TRUE;
      pRoom->Upgrade  = TRUE;
      ch->bank = UMAX( 0, ch->bank - HomeObject );

      act( "�A��F�@������$r��ˤF�h�����\\��T", ch, NULL, NULL, TO_CHAR );
      act( "$r�Q$n��˦����h�����a��F�T", ch, NULL, NULL, TO_ROOM );
      write_new_room( pRoom );
    }

    else if ( !str_prefix( arg, "foodful" ) )
    {
      if ( ch->bank < HomeObject )
      {
        act( "$n�Ȧ�̥�����$i������~��ɯŦh�����\\��C"
          , ch, &HomeObject, NULL, TO_CHAR );
        RETURN_NULL();
      }

      if ( ch->position != POS_STANDING )
      {
        send_to_char( "������䪺�Ʊ������F�A���a�T\n\r", ch );
        RETURN_NULL();
      }

      if ( !( fch = owner_name( pRoom = ch->in_room ) ) || fch != ch )
      {
        send_to_char( "�藍�_�M�o���O�A���p���]���M�S��k���A��~�T\n\r", ch );
        RETURN_NULL();
      }

      if ( pRoom->Upgrade )
      {
        act( "$r�w�g����A��ˤF�T", ch, NULL, NULL, TO_CHAR );
        RETURN_NULL();
      }

      pRoom->Foodful = TRUE;
      pRoom->Upgrade  = TRUE;
      ch->bank = UMAX( 0, ch->bank - HomeObject );

      act( "�A��F�@������$r��ˤF�h�������\\��T", ch, NULL, NULL, TO_CHAR );
      act( "$r�Q$n��˦����h�������a��F�T", ch, NULL, NULL, TO_ROOM );
      write_new_room( pRoom );
    }

    else if ( !str_prefix( arg, "deposit" ) )
    {
      if ( ch->bank < HomeObject )
      {
        act( "$n�Ȧ�̥�����$i������~��ɯſ����\\��C"
          , ch, &HomeObject, NULL, TO_CHAR );
        RETURN_NULL();
      }

      if ( ch->position != POS_STANDING )
      {
        send_to_char( "������䪺�Ʊ������F�A���a�T\n\r", ch );
        RETURN_NULL();
      }

      if ( !( fch = owner_name( pRoom = ch->in_room ) ) || fch != ch )
      {
        send_to_char( "�藍�_�M�o���O�A���p���]���M�S��k���A��~�T\n\r", ch );
        RETURN_NULL();
      }

      if ( pRoom->Upgrade )
      {
        act( "$r�w�g����A��ˤF�T", ch, NULL, NULL, TO_CHAR );
        RETURN_NULL();
      }

      pRoom->DepositMoney  = TRUE;
      pRoom->Upgrade       = TRUE;
      ch->bank = UMAX( 0, ch->bank - HomeObject );

      act( "�A��F�@������$r��ˤF�������\\��T", ch, NULL, NULL, TO_CHAR );
      act( "$r�Q$n��˦����������a��F�T", ch, NULL, NULL, TO_ROOM );
      write_new_room( pRoom );
    }

    else if ( !str_prefix( arg, "store" ) )
    {
      if ( ch->bank < HomeObject )
      {
        act( "$n�Ȧ�̥�����$i������~��ɯ��𧽥\\��C"
          , ch, &HomeObject, NULL, TO_CHAR );
        RETURN_NULL();
      }

      if ( ch->position != POS_STANDING )
      {
        send_to_char( "������䪺�Ʊ������F�A���a�T\n\r", ch );
        RETURN_NULL();
      }

      if ( !( fch = owner_name( pRoom = ch->in_room ) ) || fch != ch )
      {
        send_to_char( "�藍�_�M�o���O�A���p���]���M�S��k���A��~�T\n\r", ch );
        RETURN_NULL();
      }

      if ( pRoom->Upgrade )
      {
        act( "$r�w�g����A��ˤF�T", ch, NULL, NULL, TO_CHAR );
        RETURN_NULL();
      }

      pRoom->StoreRoom  = TRUE;
      pRoom->Upgrade    = TRUE;
      ch->bank = UMAX( 0, ch->bank - HomeObject );

      act( "�A��F�@������$r��ˤF�𧽪��\\��T", ch, NULL, NULL, TO_CHAR );
      act( "$r�Q$n��˦����𧽪��a��F�T", ch, NULL, NULL, TO_ROOM );
      write_new_room( pRoom );
    }

    else if ( !str_prefix( arg, "hitcure" ) )
    {
      if ( ch->bank < HomeObject )
      {
        act( "$n�Ȧ�̥�����$i������~��ɯťͩR��_�ҥ\\��C"
          , ch, &HomeObject, NULL, TO_CHAR );
        RETURN_NULL();
      }

      if ( ch->position != POS_STANDING )
      {
        send_to_char( "������䪺�Ʊ������F�A���a�T\n\r", ch );
        RETURN_NULL();
      }

      if ( !( fch = owner_name( pRoom = ch->in_room ) ) || fch != ch )
      {
        send_to_char( "�藍�_�M�o���O�A���p���]���M�S��k���A��~�T\n\r", ch );
        RETURN_NULL();
      }

      if ( pRoom->Upgrade )
      {
        act( "$r�w�g����A��ˤF�T", ch, NULL, NULL, TO_CHAR );
        RETURN_NULL();
      }

      pRoom->HitCure  = TRUE;
      pRoom->Upgrade  = TRUE;
      ch->bank = UMAX( 0, ch->bank - HomeObject );

      act( "�A��F�@������$r��ˤF�ͩR��_�Ҫ��\\��T", ch, NULL, NULL, TO_CHAR );
      act( "$r�Q$n��˦����ͩR��_�Ҫ��a��F�T", ch, NULL, NULL, TO_ROOM );
      write_new_room( pRoom );
    }

    else if ( !str_prefix( arg, "practice" ) )
    {
      if ( ch->bank < HomeObject )
      {
        act( "$n�Ȧ�̥�����$i������~��ɯŽm�\\���\\��C"
          , ch, &HomeObject, NULL, TO_CHAR );
        RETURN_NULL();
      }

      if ( ch->position != POS_STANDING )
      {
        send_to_char( "������䪺�Ʊ������F�A���a�T\n\r", ch );
        RETURN_NULL();
      }

      if ( !( fch = owner_name( pRoom = ch->in_room ) ) || fch != ch )
      {
        send_to_char( "�藍�_�M�o���O�A���p���]���M�S��k���A��~�T\n\r", ch );
        RETURN_NULL();
      }

      if ( pRoom->Upgrade )
      {
        act( "$r�w�g����A��ˤF�T", ch, NULL, NULL, TO_CHAR );
        RETURN_NULL();
      }

      pRoom->PracRoom = TRUE;
      pRoom->Upgrade  = TRUE;
      pRoom->Safe     = FALSE;
      pRoom->NoFight  = FALSE;
      ch->bank = UMAX( 0, ch->bank - HomeObject );

      act( "�A��F�@������$r��ˤF�m�\\�����\\��T", ch, NULL, NULL, TO_CHAR );
      act( "$r�Q$n��˦����m�\\�����a��F�T", ch, NULL, NULL, TO_ROOM );
      write_new_room( pRoom );
    }

    else if ( !str_prefix( arg, "manacure" ) )
    {
      if ( ch->bank < HomeObject )
      {
        act( "$n�Ȧ�̥�����$i������~��ɯŪk�O��_�ҥ\\��C"
          , ch, &HomeObject, NULL, TO_CHAR );
        RETURN_NULL();
      }

      if ( ch->position != POS_STANDING )
      {
        send_to_char( "������䪺�Ʊ������F�A���a�T\n\r", ch );
        RETURN_NULL();
      }

      if ( !( fch = owner_name( pRoom = ch->in_room ) ) || fch != ch )
      {
        send_to_char( "�藍�_�M�o���O�A���p���]���M�S��k���A��~�T\n\r", ch );
        RETURN_NULL();
      }

      if ( pRoom->Upgrade )
      {
        act( "$r�w�g����A��ˤF�T", ch, NULL, NULL, TO_CHAR );
        RETURN_NULL();
      }

      pRoom->ManaCure = TRUE;
      pRoom->Upgrade  = TRUE;
      ch->bank = UMAX( 0, ch->bank - HomeObject );

      act( "�A��F�@������$r��ˤF�k�O��_�Ҫ��\\��T", ch, NULL, NULL, TO_CHAR );
      act( "$r�Q$n��˦����k�O��_�Ҫ��a��F�T", ch, NULL, NULL, TO_ROOM );
      write_new_room( pRoom );
    }

    else if ( !str_prefix( arg, "movecure" ) )
    {
      if ( ch->bank < HomeObject )
      {
        act( "$n�Ȧ�̥�����$i������~��ɯ���O��_�ҥ\\��C"
          , ch, &HomeObject, NULL, TO_CHAR );
        RETURN_NULL();
      }

      if ( ch->position != POS_STANDING )
      {
        send_to_char( "������䪺�Ʊ������F�A���a�T\n\r", ch );
        RETURN_NULL();
      }

      if ( !( fch = owner_name( pRoom = ch->in_room ) ) || fch != ch )
      {
        send_to_char( "�藍�_�M�o���O�A���p���]���M�S��k���A��~�T\n\r", ch );
        RETURN_NULL();
      }

      if ( pRoom->Upgrade )
      {
        act( "$r�w�g����A��ˤF�T", ch, NULL, NULL, TO_CHAR );
        RETURN_NULL();
      }

      pRoom->MoveCure = TRUE;
      pRoom->Upgrade  = TRUE;
      ch->bank = UMAX( 0, ch->bank - HomeObject );

      act( "�A��F�@������$r��ˤF��O��_�Ҫ��\\��T", ch, NULL, NULL, TO_CHAR );
      act( "$r�Q$n��˦�����O��_�Ҫ��a��F�T", ch, NULL, NULL, TO_ROOM );
      write_new_room( pRoom );
    }

    else
    {
      send_to_char( "�S���A�ҿ�ܪ��\\��T\n\r�i�H�ɯŦ���"
        "�ͩR��_(hitcure) �k�O��_(manacure) \n\r��O��_(movecure)"
        "�W�[����(foodful) �W�[����(waterful) ���ﶵ�C\n\r", ch );
      RETURN_NULL();
    }
  }

  else if ( !str_prefix( arg, "set" ) && IS_GOD( ch ) )
  {
    argument = one_argument( argument, arg );

    if ( !arg[0] || !argument[0] )
    {
      send_to_char( "�A�Q�n�]�w����F��S\n\r", ch );
      RETURN_NULL();
    }

    if ( !is_number( argument ) || ( count = atoi( argument ) ) <= 0 )
    {
      send_to_char( "�A���]�w�ȥ����O�Ʀr�T\n\r", ch );
      RETURN_NULL();
    }

    if ( !str_prefix( arg, "Cost" ) )
    {
      HomeGold = count;
      act( "�A��R�Фl������אּ$i������T", ch, &count, NULL, TO_CHAR );
    }

    else if ( !str_prefix( arg, "Title" ) )
    {
      HomeGoldTitle = count;
      act( "�A����ж��W�٪�����אּ$i������T", ch, &count, NULL, TO_CHAR );
    }

    else if ( !str_prefix( arg, "Link" ) )
    {
      HomeGoldLink = count;
      act( "�A���s��������אּ$i������T", ch, &count, NULL, TO_CHAR );
    }

    else if ( !str_prefix( arg, "Descr" ) )
    {
      HomeGoldDescr = count;
      act( "�A����ж��y�z�����欰$i������T", ch, &count, NULL, TO_CHAR );
    }

    else if ( !str_prefix( arg, "Upgrade" ) )
    {
      HomeObject = count;
      act( "�A��ɯũЫΰt�ƪ�����אּ$i������T", ch, &count, NULL, TO_CHAR );
    }

    else if ( !str_prefix( arg, "Practice" ) )
    {
      HomeMobile = count;
      act( "�A�ⲣ�ͽm�ߩǪ�������אּ$i������T", ch, &count, NULL, TO_CHAR );
    }

    else
    {
      send_to_char( "�X�k���ѼƬ� Cost Title Descr Link Upgrade Practice\n\r", ch );
    }
  }

  else if ( !str_prefix( arg, "lock" ) && IS_GOD( ch ) )
  {
    if ( LockCreateHome )
    {
      LockCreateHome = FALSE;
      send_to_char( "�A��إ߭ק�ж��\\�൹���}�F�T\n\r", ch );
    }
    else
    {
      LockCreateHome = TRUE;
      send_to_char( "�A��إ߭ק�ж��\\�൹��w�F�T\n\r", ch );
    }
  }

  else if ( !str_prefix( arg, "information" ) && IS_IMMORTAL( ch ) )
  {
    clear_buffer();
    send_to_buffer(
    "���a�ϰ��ɮץؿ��W�١R\e[1;32m[\e[0m%s\e[1;32m]\e[0m\n\r"
    "���a�ϰ�}�l�Ǹ�    �R%-9d\n\r"
    "���a�ϰ�̤j�ƶq    �R%-9d\n\r"
    "�R�Фl������        �R%-9d ��\n\r"
    "���ж��W�٪�����  �R%-9d ��\n\r"
    "���ж��y�z������  �R%-9d ��\n\r"
    "�s���X�f������      �R%-9d ��\n\r"
    "�ɯũЫΰt�ƪ�����  �R%-9d ��\n\r"
    "�ӤH�ж��̤j��      �R%-9d ��\n\r"
    "��w���ж�        �R%s\n\r\n\r"
    , PlayerArea
    , PlayerAreaNum
    , PlayerAreaCap
    , HomeGold
    , HomeGoldTitle
    , HomeGoldDescr
    , HomeGoldLink
    , HomeObject
    , HomeManMax
    , YESNO( LockCreateHome ) );

    count = 0;
    for ( pRoom = PlayerZone->list; pRoom; pRoom = pRoom->next_in_area )
    {
      if ( count <= 0 )
        send_to_buffer( "\e[1;33;44m���� �ж����X ��  ��  �W  ��      "
         " �D�H�W��                                    \e[0m\n\r" );

      tablize( FORMAT_SET,
        "NUBR VNUM     NAME                \e[0m MASTER         "
        "                             \e[0m\n\r"
        , 0, NULL, 0 );

      tablize( FORMAT_NUM,    NULL,                          ++count,   "NUBR", 4  );
      tablize( FORMAT_NUM,    NULL,                      pRoom->vnum,   "VNUM", 8  );
      tablize( FORMAT_STRING, pRoom->name,                         0,   "NAME", 20 );
      tablize( FORMAT_STRING, fullname( &(pRoom->owner) ),         0, "MASTER", 30 );
      tablize( FORMAT_RETURN, pellet, 0, NULL, sizeof( pellet ) - 1 );

      send_to_buffer( "%s", pellet );
      if ( buffer_full() ) break;
    }

    if ( count <= 0 )
      send_to_buffer( "�藍�_�M%s�S�����󪺻{�ʤg�a�T\n\r", mud_name );

    print_buffer( ch );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "status" ) )
  {
    if ( !( fch = owner_name( pRoom = ch->in_room ) ) || fch != ch )
    {
      send_to_char( "�藍�_�M�o���O�A���p���]���M�S��k���A���D������ơT\n\r"
        , ch );
      RETURN_NULL();
    }

    print_to_char( ch,
      "�Ҧb�W�١R\e[1;32m[\e[0m%s\e[1;32m]\e[0m\n\r"
      "�ж��s���R(%5d)  "
      "�ө��P�_�R%s  "
      "�O�_�i�H�g�J�R%s\n\r"
      "�ж��X�СR%s\n\r"
      "�ж��a�ΡR%s\n\r"
      "���a������y�z�R\n\r%s%s\n\r%s"
    , pRoom->name
    , pRoom->vnum
    , YESNO( !room_is_dark( pRoom ) )
    , YESNO( pRoom->writeable )
    , room_bit_name( pRoom )
    , pRoom->sector ? pRoom->sector->cname : "����"
    , VERTICAL_LINE
    , pRoom->description
    , VERTICAL_LINE );

    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "create" ) )
  {
    if ( !IS_NPC( ch ) && ch->level < level_limit )
    {
      act( "�b�A�ɯŨ�$i�ūe�M�Ф��n�ê���T"
        , ch, &level_limit, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch->donate > 0 )
    {
      send_to_char( "�b�A����٪����o�q�ɶ��M�Ф��n�ê���T\n\r", ch );
      RETURN_NULL();
    }

    if ( LockCreateHome )
    {
      send_to_char( "�藍�_�M�ثe�޲z�̧�o�ӥ\\�൹��w��F�T\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->position != POS_STANDING )
    {
      send_to_char( "�藍�_�M�Х��ѨM�A�ۤv���Ʊ��a�T\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->bank < HomeGold )
    {
      act( "$n�Ȧ�̥�����$i������~��R�Фl�C", ch, &HomeGold, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( count_home_number( ch ) > HomeManMax )
    {
      send_to_char( "�A�w�g�O���C�����j��F�M�Ф��n�A�R�Фl�F�T\n\r",ch );
      RETURN_NULL();
    }

    if ( !( pRoom = create_new_room() ) )
    {
      send_to_char( "�藍�_�M�S����k���ͩж��F�M�A�N���ߤF�a�T\n\r", ch );
      RETURN_NULL();
    }

    pRoom->owner.high = ch->serial.high;
    pRoom->owner.low  = ch->serial.low;
    pRoom->writeable  = TRUE;
    pRoom->player     = TRUE;
    pRoom->cost       = HomeGold;

    if ( !write_new_room( pRoom ) )
    {
      mudlog( LOG_DEBUG, "create_new_room: �L�k�s�� %s.", pRoom->filename );
      send_to_char( "�藍�_�M�A���ж��L�k�s�ɡT\n\r", ch );
      RETURN_NULL();
    }

    act( "$n��F$i������R�F�@�l���v�T", ch, &HomeGold, NULL, TO_ALL );

    count = 0;
    for ( pRoom = PlayerZone->list; pRoom; pRoom = pRoom->next_in_area )
      if ( is_owner( ch, pRoom, TRUE ) ) count++;

    sprintf( buf, "�A�u�n��J \e[1;35mhome goto %d\e[0m "
      "�N�i�H��A���s�a�o�T\n\r", count );

    send_to_char( buf, ch );

    ch->bank = UMAX( 0, ch->bank - HomeGold );
    sprintf( buf, "%s��F�@�����R�F�@�ɻ��v�M�j�a�i�H��L�a�h�@�ȳ�T"
      , mob_name( NULL, ch ) );

    talk_channel_2( buf, CHANNEL_BULLETIN, "���i" );
  }

  else if ( !str_prefix( arg, "goto" ) )
  {
    one_argument( argument, arg );

    if ( !arg[0] )
    {
      count = 1;
    }
    else if ( !is_number( arg ) || ( count = atoi( arg ) ) <= 0 )
    {
      send_to_char( "�A�n�^��A���ɩФl�O�S\n\r", ch );
      RETURN_NULL();
    }

    pHome = NULL;
    loop  = 0;

    for ( pRoom = PlayerZone->list; pRoom; pRoom = pRoom->next_in_area )
    {
      if ( is_owner( ch, pRoom, TRUE ) && ++loop == count )
      {
        pHome = pRoom;
        break;
      }
    }

    if ( loop == 0 )
    {
      send_to_char( "�藍�_�M�A�ڥ��S���{�ʥ��󪺩ж��T\n\r", ch );
      RETURN_NULL();
    }

    if ( !pHome )
    {
      send_to_char( "�藍�_�M�A�S�����ɩж���T\n\r", ch );
      RETURN_NULL();
    }

    if ( pHome == ch->in_room )
    {
      send_to_char( "�藍�_�M�A�ثe�N�O�b�o�ɩФl�̰ڡT\n\r", ch );
      RETURN_NULL();
    }

    if ( !can_leaveroom( ch, TRUE ) ) RETURN_NULL();

    pRoom = ch->in_room;

    /* ���e�D�H */
    act( "$n�^�^�����a���}���a�M�^��L�ۤv���Фl�h�F�T", ch, NULL, NULL, TO_ROOM );

    char_from_room( ch );
    char_to_room( ch, pHome );

    do_look( ch, "auto" );

    /* �M���l�ܬ����I */
    clear_trace( ch, TRUE );

    act( "$n�榣�a�^��F�o���ݩ�L�ۤv���a�o�T", ch, NULL, NULL, TO_ROOM );

    /* �A�e follow ���H */
    for ( fch = pRoom->people; fch; fch = fch_next )
    {
      fch_next = fch->next_in_room;

      if ( fch->master == ch
        && fch->position == POS_STANDING
        && can_leaveroom( fch, FALSE ) )
      {
        act( "�A���H��$N�^��L���a�̡C", fch, NULL, ch, TO_CHAR );

        char_from_room( fch );
        char_to_room( fch, pHome );
        do_look( fch, "auto" );

        /* �M���l�ܬ����I */
        clear_trace( fch, TRUE );

        act( "$n���H��$N�^��L���a�̡C", fch, NULL, ch, TO_ROOM );
      }
    }
  }

  else if ( !str_prefix( arg, "title" ) )
  {
    if ( !IS_NPC( ch ) && ch->level < level_limit )
    {
      act( "�b�A�ɯŨ�$i�ūe�M�Ф��n�ê���T"
        , ch, &level_limit, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch->donate > 0 )
    {
      send_to_char( "�b�A����٪����o�q�ɶ��M�Ф��n�ê���T\n\r", ch );
      RETURN_NULL();
    }

    if ( LockCreateHome )
    {
      send_to_char( "�藍�_�M�ثe�޲z�̧�o�ӥ\\�൹��w��F�T\n\r", ch );
      RETURN_NULL();
    }

    if ( owner_name( ( pRoom = ch->in_room ) ) != ch )
    {
      send_to_char( "�藍�_�M�o���O�A���p���]���M�Ф��n�b����~�T\n\r", ch );
      RETURN_NULL();
    }

    if ( !pRoom->writeable )
    {
      act( "�藍�_�M$r�w�g����A���F��T", ch, NULL, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( !argument[0] )
    {
      act( "�藍�_�M�A�n��$r��W������O�S", ch, NULL, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch->bank < HomeGoldTitle )
    {
      act( "�A�Ȧ�̥�����$i������~����Фl�W�١C"
        , ch, &HomeGoldTitle, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( MaxCNameLen < sizeof( argument ) && str_len( argument ) > MaxCNameLen )
      argument[ MaxCNameLen ] = '\x0';

    if ( str_len( argument ) < MinCNameLen )
    {
      send_to_char( "�A���W�٤]�����ӵu�F�a�T\n\r", ch );
      RETURN_NULL();
    }

    if ( check_cname_xname( argument ) )
    {
      send_to_char( "�o�ӦW�٤��n�a�M���ӦW�٬ݬݡC\n\r", ch );
      RETURN_NULL();
    }

    ansi_transcribe( argument, buf );
    filter_ansi( buf );
    smash_tilde( buf );
    smash_point( buf );
    fix_color( buf );

    free_string( pRoom->name );
    pRoom->name = str_dup( buf );

    if ( !write_new_room( pRoom ) )
    {
      mudlog( LOG_DEBUG, "create_new_room: �L�k�s�� %s.", pRoom->filename );
      send_to_char( "�藍�_�M�A���ж��L�k�s�ɡT\n\r", ch );
      RETURN_NULL();
    }

    act( "$n��o�̧�W��$t�T", ch, buf, NULL, TO_ALL );
    ch->bank = UMAX( 0, ch->bank - HomeGoldTitle );
  }

  else if ( !str_prefix( arg, "edit" ) )
  {
    if ( !IS_NPC( ch ) && ch->level < level_limit )
    {
      act( "�b�A�ɯŨ�$i�ūe�M�Ф��n�ê���T"
        , ch, &level_limit, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch->donate > 0 )
    {
      send_to_char( "�b�A����٪����o�q�ɶ��M�Ф��n�ê���T\n\r", ch );
      RETURN_NULL();
    }

    if ( LockCreateHome )
    {
      send_to_char( "�藍�_�M�ثe�޲z�̧�o�ӥ\\�൹��w��F�T\n\r", ch );
      RETURN_NULL();
    }

    if ( !ch->desc ) RETURN_NULL();

    if ( owner_name( ( pRoom = ch->in_room ) ) != ch )
    {
      send_to_char( "�藍�_�M�o���O�A���p���]���M�Ф��n�b����~�T\n\r", ch );
      RETURN_NULL();
    }

    if ( !pRoom->writeable )
    {
      act( "�藍�_�M$r�w�g����A���]�w�F��T", ch, NULL, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch->bank < HomeGoldDescr )
    {
      act( "$n�Ȧ�̥�����$i������~����Фl���ԭz�C"
        , ch, &HomeGoldDescr, NULL, TO_CHAR );
      RETURN_NULL();
    }

    ch->desc->edit_mode = EDIT_HOME_DESCR;
    show_editor( ch );

    free_string( ch->stack );
    ch->stack = str_dup( "" );

    ch->bank = UMAX( 0, ch->bank - HomeGoldDescr );
  }

  else if ( !str_prefix( arg, "unlink" ) )
  {
    if ( !IS_NPC( ch ) && ch->level < level_limit )
    {
      act( "�b�A�ɯŨ�$i�ūe�M�Ф��n�ê���T"
        , ch, &level_limit, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch->donate > 0 )
    {
      send_to_char( "�b�A����٪����o�q�ɶ��M�Ф��n�ê���T\n\r", ch );
      RETURN_NULL();
    }

    if ( LockCreateHome )
    {
      send_to_char( "�藍�_�M�ثe�޲z�̧�o�ӥ\\�൹��w��F�T\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->position != POS_STANDING )
    {
      send_to_char( "�藍�_�M�A����A�����ƥ��˧��a�T\n\r", ch );
      RETURN_NULL();
    }

    if ( owner_name( ( pHome = ch->in_room ) ) != ch )
    {
      send_to_char( "�藍�_�M�o���O�A���p���]���M�Ф��n�b����~�T\n\r", ch );
      RETURN_NULL();
    }

    if ( !pHome->writeable )
    {
      act( "�藍�_�M$r�w�g����A���]�w�F��T", ch, NULL, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch->bank < HomeGoldLink )
    {
      act( "$n�Ȧ�̥�����$i������~��s����L�ж��C"
        , ch, &HomeGoldLink, NULL, TO_CHAR );
      RETURN_NULL();
    }

    argument = one_argument( argument, arg );

         if ( !str_prefix( arg, "north" ) ) dir = DIR_NORTH;
    else if ( !str_prefix( arg, "east"  ) ) dir = DIR_EAST;
    else if ( !str_prefix( arg, "south" ) ) dir = DIR_SOUTH;
    else if ( !str_prefix( arg, "west"  ) ) dir = DIR_WEST;
    else if ( !str_prefix( arg, "up"    ) ) dir = DIR_UP;
    else if ( !str_prefix( arg, "down"  ) ) dir = DIR_DOWN;
    else if ( !str_prefix( arg, "enter" ) ) dir = DIR_ENTER;
    else if ( !str_prefix( arg, "out"   ) ) dir = DIR_OUT;
    else
    {
      send_to_char( "�藍�_�M�����O�@�ӥ��T����V�M�Ьd�� direction�T\n\r", ch );
      RETURN_NULL();
    }

    if ( !( aExit = pHome->exit[dir] ) )
    {
      send_to_char( "�藍�_�M�ڥ��S���o�Ӥ�V���q�D�T\n\r", ch );
      RETURN_NULL();
    }

    if ( !( pRoom = aExit->to_room ) )
    {
      send_to_char( "�藍�_�M�o���q�D���I���D�M���G�L�k�R���T\n\r", ch );
      RETURN_NULL();
    }

    if ( !pRoom->writeable )
    {
      send_to_char( "�藍�_�M���ж��w�g����A���]�w�F��T\n\r", ch );
      RETURN_NULL();
    }

    if ( !( bExit = pRoom->exit[rev_dir[dir]] ) )
    {
      send_to_char( "�藍�_�M��V���V�s�������~�M�L�k�R���T\n\r", ch );
      RETURN_NULL();
    }

    pHome->exit[dir]          = NULL;
    pRoom->exit[rev_dir[dir]] = NULL;

    write_new_room( pRoom );
    write_new_room( pHome );

    act( "$n�b�o�̪�$t�誺�q�D�������F�T"
      , ch, direction_name( dir ), NULL, TO_ALL );

    char_from_room( ch );
    char_to_room( ch, pRoom );

    act( "$n�b�j����F�@���j���Y�M�]��$t�誺�q�D�w�g���q�F�T"
      , ch, direction_name( rev_dir[dir] ), NULL, TO_ROOM );

    char_from_room( ch );
    char_to_room( ch, pHome );

    ch->bank = UMAX( 0, ch->bank - HomeGoldLink );

    if ( aExit->keyword     ) free_string( aExit->keyword     );
    if ( aExit->description ) free_string( aExit->description );
    if ( bExit->message     ) free_string( bExit->message     );

    if ( bExit->keyword     ) free_string( bExit->keyword     );
    if ( bExit->description ) free_string( bExit->description );
    if ( bExit->message     ) free_string( bExit->message     );

    free_struct( aExit, STRUCT_EXIT_DATA );
    free_struct( bExit, STRUCT_EXIT_DATA );
  }

  else if ( !str_prefix( arg, "link" ) )
  {
    if ( !IS_NPC( ch ) && ch->level < level_limit )
    {
      act( "�b�A�ɯŨ�$i�ūe�M�Ф��n�ê���T"
        , ch, &level_limit, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch->donate > 0 )
    {
      send_to_char( "�b�A����٪����o�q�ɶ��M�Ф��n�ê���T\n\r", ch );
      RETURN_NULL();
    }

    if ( LockCreateHome )
    {
      send_to_char( "�藍�_�M�ثe�޲z�̧�o�ӥ\\�൹��w��F�T\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->position != POS_STANDING )
    {
      send_to_char( "�藍�_�M�A����A�����ƥ��˧��a�T\n\r", ch );
      RETURN_NULL();
    }

    if ( owner_name( ( pHome = ch->in_room ) ) != ch )
    {
      send_to_char( "�藍�_�M�o���O�A���p���]���M�Ф��n�b����~�T\n\r", ch );
      RETURN_NULL();
    }

    if ( !pHome->writeable )
    {
      act( "�藍�_�M$r�w�g����A���F��T", ch, NULL, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch->bank < HomeGoldLink )
    {
      act( "$n�Ȧ�̥�����$i������~��s����L�ж��C"
        , ch, &HomeGoldLink, NULL, TO_CHAR );
      RETURN_NULL();
    }

    argument = one_argument( argument, arg );

    if ( !arg[0] || !is_number( arg ) || ( count = atoi( arg ) ) <= 0 )
    {
      send_to_char( "�A�n�s�����@���ж��O�S�е����������X�a�T\n\r", ch );
      RETURN_NULL();
    }

    if ( !( pRoom = get_room_index( count ) ) )
    {
      send_to_char( "�藍�_�M�S���o�@�����X���ж��T\n\r", ch );
      RETURN_NULL();
    }

    if ( pRoom == pHome )
    {
      send_to_char( "�ۦP���ж��O�L�k�i��s���T\n\r", ch );
      RETURN_NULL();
    }

    if ( !pRoom->area || pRoom->area != PlayerZone )
    {
      send_to_char( "�藍�_�M�o���ж��O�T�w�Φ����M����s���T\n\r", ch );
      RETURN_NULL();
    }

    if ( pRoom->NoLink )
    {
      send_to_char( "�藍�_�M�o���ж��O����s���T\n\r", ch );
      RETURN_NULL();
    }

    if ( !pRoom->writeable )
    {
      send_to_char( "�藍�_�M�����ж�����A���F�T\n\r", ch );
      RETURN_NULL();
    }

    if ( !( fch = owner_name( pRoom ) ) )
    {
      send_to_char( "�藍�_�M�o���ж����D�H�����D�O�֭C�T\n\r", ch );
      RETURN_NULL();
    }

    if ( fch != ch && !is_same_group( fch, ch ) )
    {
      send_to_char( "�藍�_�M�s�����ж��A�S���v�O�h�ʥ��T\n\r", ch );
      RETURN_NULL();
    }

    if ( !argument[0] )
    {
      send_to_char( "�藍�_�M�A�n�s�����Ӥ�V�O�S\n\r", ch );
      RETURN_NULL();
    }

         if ( !str_prefix( argument, "north" ) ) dir = DIR_NORTH;
    else if ( !str_prefix( argument, "east"  ) ) dir = DIR_EAST;
    else if ( !str_prefix( argument, "south" ) ) dir = DIR_SOUTH;
    else if ( !str_prefix( argument, "west"  ) ) dir = DIR_WEST;
    else if ( !str_prefix( argument, "up"    ) ) dir = DIR_UP;
    else if ( !str_prefix( argument, "down"  ) ) dir = DIR_DOWN;
    else if ( !str_prefix( argument, "enter" ) ) dir = DIR_ENTER;
    else if ( !str_prefix( argument, "out"   ) ) dir = DIR_OUT;
    else
    {
      send_to_char( "�藍�_�M�����O�@�ӥ��T����V�M�Ьd�� direction�T\n\r", ch );
      RETURN_NULL();
    }

    if ( pHome->exit[dir] || pRoom->exit[rev_dir[dir]] )
    {
      send_to_char( "�藍�_�M�o��V�w�g���s���F�T\n\r", ch );
      RETURN_NULL();
    }

    for ( loop = 0; loop < DIR_MAX; loop++ )
    {
      if ( pHome->exit[loop] && pHome->exit[loop]->to_room == pRoom )
      {
        send_to_char( "�o���ж��w����L��V���s���F�T\n\r", ch );
        RETURN_NULL();
      }
    }

    for ( loop = 0; loop < DIR_MAX; loop++ )
    {
      if ( pRoom->exit[loop] && pRoom->exit[loop]->to_room == pHome )
      {
        send_to_char( "�o���ж��w����L��V���s���F�T\n\r", ch );
        RETURN_NULL();
      }
    }

    /* �t�m�O���� */
    set_exit_default( aExit = alloc_struct( STRUCT_EXIT_DATA ) );
    set_exit_default( bExit = alloc_struct( STRUCT_EXIT_DATA ) );

    /* �M���򥻭� */
    aExit->description = bExit->description = str_dup( "" );
    aExit->keyword     = bExit->keyword     = str_dup( "" );
    aExit->closed      = bExit->closed      = FALSE;
    aExit->isdoor      = bExit->isdoor      = FALSE;
    aExit->locked      = bExit->locked      = FALSE;
    aExit->pick        = bExit->pick        = FALSE;
    aExit->key         = bExit->key         = 0;

    /* �إ߳s�� */
    aExit->to_room = pRoom;
    bExit->to_room = pHome;

    aExit->vnum = pRoom->vnum;
    bExit->vnum = pHome->vnum;

    pHome->exit[dir]          = aExit;
    pRoom->exit[rev_dir[dir]] = bExit;

    write_new_room( pRoom );
    write_new_room( pHome );

    act( "$n�b�o�̪�$t�襴�F�Ӥj�}�T"
      , ch, direction_name( dir ), NULL, TO_ALL );

    char_from_room( ch );
    char_to_room( ch, pRoom );

    act( "$n�b�j�����F�Ӥj�}�M�]��$t��i�H�q�쨺�̥h�F�T"
      , ch, direction_name( rev_dir[dir] ), NULL, TO_ROOM );

    char_from_room( ch );
    char_to_room( ch, pHome );

    ch->bank = UMAX( 0, ch->bank - HomeGoldLink );
  }

  else
  {
    send_to_char( "�A���y�k���~�M�Ьd�� home �����T��k�C\n\r", ch );
  }
  RETURN_NULL();
}

ROOM_INDEX_DATA * create_new_room( void )
{
  int               loop;
  int               vnum;
  int               iExit;
  int               iHash;
  int               nMatch;
  int               count;
  char              filename[MAX_STRING_LENGTH];
  char              dirname[MAX_STRING_LENGTH];
  ROOM_INDEX_DATA * pRoom;
  ROOM_INDEX_DATA * zStandard;

  PUSH_FUNCTION( "create_new_room" );

  /* �p�G�S�����a�ϰ쪺�ؿ� */
  if ( !PlayerZone ) RETURN( NULL );

  /* �p�G�ϰ���t���X�ήe�q���X�z */
  if ( PlayerAreaNum <= 0 || PlayerAreaCap <= 0 ) RETURN( NULL );

  /* ����@���˥��ж� */
  zStandard = NULL;
  for ( count = vnum = nMatch = 0; nMatch < top_room; vnum++ )
  {
    if ( ( pRoom = get_room_index( vnum ) ) )
    {
      nMatch++;

     if ( pRoom->Standard && number_range( 0, count++ ) == 0 )
        zStandard = pRoom;
    }
  }

  /* �䤣�����@���˥��ж� */
  if ( !zStandard ) RETURN( NULL );

  /* �ؿ��O�_�i�H�g�J */
  sprintf( dirname, "%s/%s/", PlayerArea, room_dir );
  if ( access( dirname, W_OK ) ) RETURN( NULL );

  for ( loop = 0; loop < PlayerAreaCap; loop++ )
  {
    if ( get_room_index( ( vnum = loop + PlayerAreaNum ) ) ) continue;

    /* �]�w�ж��ɮצW�� */
    sprintf( filename, "%s/%s/%d.%s"
      , PlayerArea, room_dir, vnum, room_ext );

    set_room_default( pRoom = alloc_struct( STRUCT_ROOM_INDEX_DATA ) );

    /* �]�w�ж��� */
    pRoom->area            = PlayerZone;
    pRoom->filename        = str_dup( filename );
    pRoom->vnum            = vnum;
    pRoom->light           = 0;
    pRoom->sector          = DefaultSector;

    /* �̷Ӽзǩж��]�w�ƭ� */
    pRoom->name            = str_dup( zStandard->name );
    pRoom->description     = str_dup( zStandard->description );

    pRoom->Dark            = zStandard->Dark;
    pRoom->NoMob           = zStandard->NoMob;
    pRoom->Indoors         = zStandard->Indoors;
    pRoom->Private         = zStandard->Private;
    pRoom->Safe            = zStandard->Safe;
    pRoom->ForeverLight    = zStandard->ForeverLight;
    pRoom->NoRecall        = zStandard->NoRecall;
    pRoom->DepositMoney    = zStandard->DepositMoney;
    pRoom->StoreRoom       = zStandard->StoreRoom;
    pRoom->NoFight         = zStandard->NoFight;
    pRoom->NoQuit          = zStandard->NoQuit;
    pRoom->Club            = zStandard->Club;
    pRoom->Memorize        = zStandard->Memorize;
    pRoom->Stock           = zStandard->Stock;
    pRoom->NoWhere         = zStandard->NoWhere;
    pRoom->Sail            = zStandard->Sail;
    pRoom->Fane            = zStandard->Fane;
    pRoom->Killer          = zStandard->Killer;
    pRoom->Standard        = FALSE;

    /* ���ϰ�C�� */
    pRoom->next_in_area    = PlayerZone->list;
    PlayerZone->list       = pRoom;
    PlayerZone->room++;

    iHash                  = pRoom->vnum % MAX_KEY_HASH;
    pRoom->next            = room_index_hash[iHash];
    room_index_hash[iHash] = pRoom;
    top_room++;

    /* �]�w�S���X�f */
    for ( iExit = 0; iExit < DIR_MAX; iExit++ ) pRoom->exit[iExit] = NULL;

    RETURN( pRoom );
  }

  mudlog( LOG_DEBUG, "create_new_room: ���a�ϰ��ɮפw��." );
  RETURN( NULL );
}

bool write_new_room( ROOM_INDEX_DATA * pRoom )
{
  ENQUIRE_DATA     * pEnquire;
  EXTRA_DESCR_DATA * pExtra;
  FILE             * pFile;
  EXIT_DATA        * pExit;
  char               buf[MAX_STRING_LENGTH];
  char               Str1[MAX_INPUT_LENGTH];
  int                iExit;

  PUSH_FUNCTION( "write_new_room" );

  if ( !pRoom )
  {
    mudlog( LOG_DEBUG, "write_new_room: �ӷ����s�b." );
    RETURN( FALSE );
  }

  if ( !pRoom->sector ) RETURN( FALSE );

  if ( !( pFile = FOPEN( pRoom->filename, "w" ) ) )
  {
    mudlog( LOG_DEBUG, "write_new_room: �L�k�}���ɮ� %s.", pRoom->filename );
    RETURN( FALSE );
  }

  fprint_number( pFile, "Vnum" , pRoom->vnum );
  fprint_string( pFile, "Name",  pRoom->name );

  buf[0] = '\x0';
  clear_return( pRoom->description, buf );
  fprintf( pFile, "Description\n%s~\n", buf );

  switch( pRoom->sector->vnum )
  {
  default: fprint_number( pFile, "SectorType", pRoom->sector->vnum ); break;
  case 0: fprintf( pFile, "SectorType      SECT_INSIDE\n"          ); break;
  case 1: fprintf( pFile, "SectorType      SECT_CITY\n"            ); break;
  case 2: fprintf( pFile, "SectorType      SECT_FIELD\n"           ); break;
  case 3: fprintf( pFile, "SectorType      SECT_FOREST\n"          ); break;
  case 4: fprintf( pFile, "SectorType      SECT_HILLS\n"           ); break;
  case 5: fprintf( pFile, "SectorType      SECT_MOUNTAIN\n"        ); break;
  case 6: fprintf( pFile, "SectorType      SECT_WATER_SWIM\n"      ); break;
  case 7: fprintf( pFile, "SectorType      SECT_WATER_NOSWIM\n"    ); break;
  case 8: fprintf( pFile, "SectorType      SECT_AIR\n"             ); break;
  case 9: fprintf( pFile, "SectorType      SECT_DESERT\n"          ); break;
  }

  str_cpy( Str1, symbol_string( TRUE, "�޿�`��" ) );

  if ( pRoom->player       ) fprintf( pFile, "Player          %s\n", Str1 );
  if ( pRoom->Dark         ) fprintf( pFile, "Dark            %s\n", Str1 );
  if ( pRoom->NoMob        ) fprintf( pFile, "NoMob           %s\n", Str1 );
  if ( pRoom->Indoors      ) fprintf( pFile, "InDoors         %s\n", Str1 );
  if ( pRoom->Private      ) fprintf( pFile, "Private         %s\n", Str1 );
  if ( pRoom->Safe         ) fprintf( pFile, "Safe            %s\n", Str1 );
  if ( pRoom->ForeverLight ) fprintf( pFile, "ForeverLight    %s\n", Str1 );
  if ( pRoom->NoRecall     ) fprintf( pFile, "NoRecall        %s\n", Str1 );
  if ( pRoom->DepositMoney ) fprintf( pFile, "DepositMoney    %s\n", Str1 );
  if ( pRoom->StoreRoom    ) fprintf( pFile, "StoreRoom       %s\n", Str1 );
  if ( pRoom->NoFight      ) fprintf( pFile, "NoFight         %s\n", Str1 );
  if ( pRoom->NoQuit       ) fprintf( pFile, "NoQuit          %s\n", Str1 );
  if ( pRoom->Club         ) fprintf( pFile, "Club            %s\n", Str1 );
  if ( pRoom->Memorize     ) fprintf( pFile, "Memorize        %s\n", Str1 );
  if ( pRoom->Stock        ) fprintf( pFile, "Stock           %s\n", Str1 );
  if ( pRoom->NoWhere      ) fprintf( pFile, "NoWhere         %s\n", Str1 );
  if ( pRoom->Sail         ) fprintf( pFile, "Sail            %s\n", Str1 );
  if ( pRoom->Fane         ) fprintf( pFile, "Fane            %s\n", Str1 );
  if ( pRoom->Killer       ) fprintf( pFile, "Killer          %s\n", Str1 );
  if ( pRoom->Standard     ) fprintf( pFile, "Standard        %s\n", Str1 );
  if ( pRoom->writeable    ) fprintf( pFile, "Writeable       %s\n", Str1 );
  if ( pRoom->Waterful     ) fprintf( pFile, "Waterful        %s\n", Str1 );
  if ( pRoom->Foodful      ) fprintf( pFile, "Foodful         %s\n", Str1 );
  if ( pRoom->ManaCure     ) fprintf( pFile, "ManaCure        %s\n", Str1 );
  if ( pRoom->MoveCure     ) fprintf( pFile, "MoveCure        %s\n", Str1 );
  if ( pRoom->HitCure      ) fprintf( pFile, "HitCure         %s\n", Str1 );
  if ( pRoom->PracRoom     ) fprintf( pFile, "PracRoom        %s\n", Str1 );
  if ( pRoom->Upgrade      ) fprintf( pFile, "Upgrade         %s\n", Str1 );

  /* �g�J�֦��� */
  if ( pRoom->owner.high > 0 && pRoom->owner.low > 0 )
  {
    fprintf( pFile, "Owner           %d %d\n"
      , pRoom->owner.high, pRoom->owner.low );
  }

  if ( pRoom->cost > 0 ) fprint_number( pFile, "Cost", pRoom->cost );
  if ( pRoom->gold > 0 ) fprint_number( pFile, "Gold", pRoom->gold );

  for ( iExit = 0; iExit < DIR_MAX; iExit++ )
  {
    if ( !( pExit = pRoom->exit[iExit] ) ) continue;
    fprintf( pFile, "\n#Exit\n" );

    switch( iExit )
    {
    case DIR_NORTH: fprintf( pFile, "Direction       North\n" ); break;
    case DIR_SOUTH: fprintf( pFile, "Direction       South\n" ); break;
    case DIR_EAST : fprintf( pFile, "Direction       East\n"  ); break;
    case DIR_WEST : fprintf( pFile, "Direction       West\n"  ); break;
    case DIR_UP   : fprintf( pFile, "Direction       Up\n"    ); break;
    case DIR_DOWN : fprintf( pFile, "Direction       Down\n"  ); break;
    case DIR_ENTER: fprintf( pFile, "Direction       Enter\n" ); break;
    case DIR_OUT  : fprintf( pFile, "Direction       Out\n"   ); break;
    }

    fprint_number( pFile, "ExitVnum"    , pExit->to_room->vnum );
    fprint_string( pFile, "ExitDesc"    , pExit->description   );
    fprint_string( pFile, "ExitKeyword" , pExit->keyword       );
    fprint_number( pFile, "ExitKey"     , pExit->key           );

    if ( pExit->isdoor ) fprintf( pFile, "IsDoor          %s\n", Str1 );
    if ( pExit->closed ) fprintf( pFile, "Closed          %s\n", Str1 );
    if ( pExit->locked ) fprintf( pFile, "Locked          %s\n", Str1 );
    if ( pExit->pick   ) fprintf( pFile, "Pick            %s\n", Str1 );

    fprintf( pFile, "End\n" );
  }

  for ( pExtra = pRoom->extra_descr; pExtra; pExtra = pExtra->next )
  {
    fprintf( pFile, "\n#Keyword\n" );
    fprint_string( pFile, "Keyword", pExtra->keyword );

    buf[0] = '\x0';
    clear_return( pExtra->description, buf );
    fprintf( pFile, "Description\n%s~\n", buf );

    fprintf( pFile, "End\n" );
  }

  for ( pEnquire = pRoom->enquire; pEnquire; pEnquire = pEnquire->next )
  {
    fprintf( pFile, "\n#Enquire\n" );
    fprint_string( pFile, "KeyWord", pEnquire->keyword );
    fprintf( pFile, "End\n" );
  }

  fprintf( pFile, "\nEnd" );

  FCLOSE( pFile );
  RETURN( TRUE );
}

bool is_owner( CHAR_DATA * ch, ROOM_INDEX_DATA * pRoom, bool couple )
{
  PUSH_FUNCTION( "is_owner" );

  if ( !ch || !pRoom )
  {
    mudlog( LOG_DEBUG, "is_owner: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( IS_NPC( ch ) ) RETURN( FALSE );

  if ( ch->serial.high == pRoom->owner.high
    && ch->serial.low == pRoom->owner.low ) RETURN( TRUE );

  if ( couple
    && ch->pcdata
    && ch->pcdata->couple.high > 0
    && ch->pcdata->couple.low > 0
    && ch->pcdata->couple.high == pRoom->owner.high
    && ch->pcdata->couple.low  == pRoom->owner.low ) RETURN( TRUE )

  RETURN( FALSE );
}

CHAR_DATA * owner_name( ROOM_INDEX_DATA * pRoom )
{
  CHAR_DATA       * ch;
  DESCRIPTOR_DATA * man;

  PUSH_FUNCTION( "owner_name" );

  if ( !pRoom )
  {
    mudlog( LOG_DEBUG, "owner_name: �ʥF�ӷ�." );
    RETURN( NULL );
  }

  for ( man = descriptor_list; man; man = man->next )
  {
    if ( verify_desc( man )
      && man->connected == CON_PLAYING
      && verify_char( ( ch = man->character ) )
      && ch->in_room
      && ch->serial.high == pRoom->owner.high
      && ch->serial.low  == pRoom->owner.low )
      RETURN( ch );
  }

  RETURN( NULL );
}

void room_update( void )
{
  AREA_DATA       * pArea;
  CHAR_DATA       * pChar;
  CHAR_DATA       * zChar;
  ROOM_INDEX_DATA * pRoom;
  OBJ_DATA        * pObj;
  OBJ_DATA        * zObj;
  int               count;
  bool              bFound;
  char              buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "room_update" );

  for ( pArea = area_first; pArea; pArea = pArea->next )
  {
    for ( pRoom = pArea->list; pRoom; pRoom = pRoom->next_in_area )
    {
      if ( pRoom->mob_reborn > 0 ) pRoom->mob_reborn--;

      if ( pRoom->nplayer > MaxRoomPlayer )
      {

      }

      if ( pRoom->nobject > MaxRoomObject )
      {
        zObj  = NULL;
        count = 1;

        for ( pObj = pRoom->contents; pObj; pObj = pObj->next_content )
        {
          if ( number_range( 1, ++count ) == 1
            && pObj->item_type != ITEM_CORPSE_PC
            && pObj->Takeable == TRUE )
            zObj = pObj;
        }

        if ( zObj )
        {
          sprintf( buf, "�Y�����W�H����%s���^�m���F�T\n\r"
            , obj_name( NULL, zObj ) );

          sendmsg_to_someroom( buf, pRoom );
          extract_obj( zObj );
        }
      }

      if ( !pRoom->people ) continue;

      if ( pRoom->ManaCure )
      {
        zChar = NULL;
        count = 0;

        for ( pChar = pRoom->people; pChar; pChar = pChar->next_in_room )
        {
          if ( !IS_NPC( pChar )
            && pChar->mana < get_curr_mana( pChar )
            && number_range( 1, ++count ) == 1 )
            zChar = pChar;
        }

        if ( zChar )
        {
          zChar->mana += get_curr_mana( zChar ) / 3;
          zChar->mana  = UMIN( zChar->mana, get_curr_mana( zChar ) );
          send_to_char( "\n\r\e[1;32m���ڨ����A�^�_�����k�O�a�T\e[0m\n\r", zChar );
          continue;
        }
      }

      if ( pRoom->MoveCure )
      {
        zChar = NULL;
        count = 0;

        for ( pChar = pRoom->people; pChar; pChar = pChar->next_in_room )
        {
          if ( !IS_NPC( pChar )
            && pChar->move < get_curr_move( pChar )
            && number_range( 1, ++count ) == 1 )
            zChar = pChar;
        }

        if ( zChar )
        {
          zChar->move += get_curr_move( zChar ) / 3;
          zChar->move  = UMIN( zChar->move, get_curr_move( zChar ) );
          send_to_char( "\n\r\e[1;32m���ڨ����A�^�_������O�a�T\e[0m\n\r", zChar );
          continue;
        }
      }

      if ( pRoom->HitCure )
      {
        zChar = NULL;
        count = 0;

        for ( pChar = pRoom->people; pChar; pChar = pChar->next_in_room )
        {
          if ( !IS_NPC( pChar )
            && pChar->hit < get_curr_hit( pChar )
            && number_range( 1, ++count ) == 1 )
            zChar = pChar;
        }

        if ( zChar )
        {
          zChar->hit += get_curr_hit( zChar ) / 3;
          zChar->hit  = UMIN( zChar->hit, get_curr_hit( zChar ) );
          send_to_char( "\n\r\e[1;32m���ڨ����A�^�_�����ͩR�O�a�T\e[0m\n\r", zChar );
          continue;
        }
      }

      if ( pRoom->Waterful )
      {
        bFound = FALSE;
        for ( pObj = pRoom->contents; pObj; pObj = pObj->next_content )
        {
          if ( pObj->pIndexData == ObjSpring )
          {
            bFound = TRUE;
            break;
          }
        }

        if ( !bFound && pRoom->nobject < MaxRoomObject )
        {
          pObj  = create_object( ObjSpring , 1 );
          pObj->timer = 10;
          obj_to_room( pObj, pRoom );

          sprintf( buf, "�U��j�ݡM�K�O�ذe��%s���A�̰e�ӤF�T\n\r"
            , obj_name( NULL, pObj ) );

          sendmsg_to_room( buf, pRoom->people );
          continue;
        }
      }

      if ( pRoom->Foodful )
      {
        bFound = FALSE;
        for ( pObj = pRoom->contents; pObj; pObj = pObj->next_content )
        {
          if ( pObj->pIndexData == ObjDumpling )
          {
            bFound = TRUE;
            break;
          }
        }

        if ( !bFound && pRoom->nobject < MaxRoomObject )
        {
          pObj  = create_object( ObjDumpling , 1 );
          pObj->timer = 10;
          obj_to_room( pObj, pRoom );

          sprintf( buf, "�U��j�ݡM�K�O�ذe��%s���A�̰e�ӤF�T\n\r"
            , obj_name( NULL, pObj ) );

          sendmsg_to_room( buf, pRoom->people );
          continue;
        }
      }
    }
  }
  RETURN_NULL();
}

int count_home_number( CHAR_DATA * ch )
{
  ROOM_INDEX_DATA * pRoom;
  int               count;

  PUSH_FUNCTION( "count_home_number" );

  if ( IS_NPC( ch ) || !ch->in_room || !PlayerZone ) RETURN( 0 );

  count = 0;
  for ( pRoom = PlayerZone->list; pRoom; pRoom = pRoom->next_in_area )
    if ( is_owner( ch, pRoom, FALSE ) ) count++;

  RETURN( count );
}

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
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include "merc.h"

void    system_cleanup  args( ( void ) );

IMMLIST_DATA * imm_lookup( const char * name )
{
  IMMLIST_DATA * pImmlist;

  PUSH_FUNCTION( "imm_lookup" );

  if ( !name || !*name ) RETURN( NULL );

  for ( pImmlist = imm_list; pImmlist; pImmlist = pImmlist->next )
    if ( !str_cmp( pImmlist->name, name ) ) RETURN( pImmlist );

  RETURN( NULL );
}

FUNCTION( do_data )
{
  CHAR_DATA  * man;
  char         arg[ MAX_STRING_LENGTH ];
  char         buf1[MAX_STRING_LENGTH];
  char         buf2[MAX_STRING_LENGTH];
  char         buf3[MAX_STRING_LENGTH];
  int          aNum;
  int          bNum;
  int          loop;
  int          count;
  int          total;

  PUSH_FUNCTION( "do_data" );

  argument = one_argument( argument , arg );

  if ( !arg[0] )
  {
    send_to_char( "�Ьd�� data �����T�ϥΤ�k�M���¡C\n\r" , ch );
    RETURN_NULL();
  }

  clear_buffer();

  if ( !str_prefix( arg , "money" ) || !str_prefix( arg , "gold" ) )
  {
    send_to_buffer( "%-15s %13s %13s %13s %13s\n\r"
      , "���a�m�W" , "���W�Ȩ�" , "�����Ȩ�", "�Ѳ��겣", "�`�겣�B" );

    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) || !man->pcdata ) continue;

      count = man->gold + man->bank + stock_value( man );
      total = personal_asset( man );

      send_to_buffer( "%-15s %13s %13s %13s %s%13s\e[0m\n\r"
        , man->name
        , numberize( man->gold, NULL )
        , numberize( man->bank, buf1 )
        , numberize( stock_value( man ), buf2 )
        , count != total ? "\e[1;31m(����)" : ""
        , numberize( total, buf3 ) );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg, "age" ) )
  {
    send_to_buffer( "%-20s %6s %6s\n\r", "���a�m�W", "����", "�~��" );

    /* ���o���ĤH�� */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) ) continue;

      send_to_buffer( "%-20s %6d %6d\n\r"
        , man->name, man->level, get_age( man ) );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg, "Imprison" ) )
  {
    send_to_buffer( "%-20s %6s %8s\n\r", "���a�m�W" , "����", "�Q���X��" );

    /* ���o���ĤH�� */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || !man->pcdata ) continue;

      send_to_buffer( "%-20s %6d %8d\n\r"
        , man->name
        , man->level
        , man->pcdata->imprison );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg , "attribute" ) )
  {
    send_to_buffer( "%-20s %6s %6s %6s %6s %6s %6s\n\r"
      , "���a�m�W" , "����", "�O�q" , "���O" , "����" , "�ӱ�" , "���" );

    /* ���o���ĤH�� */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) ) continue;

      send_to_buffer( "%-20s %6d %6d %6d %6d %6d %6d\n\r"
        , man->name
        , man->level
        , get_curr_str( man )
        , get_curr_int( man )
        , get_curr_wis( man )
        , get_curr_dex( man )
        , get_curr_con( man ) );
    }
    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg , "basic" ) )
  {
    send_to_buffer( "%-20s %8s %8s %8s %8s\n\r"
      , "���a�m�W" , "����", "�ͩR�O" , "�k�O" , "���ʤO" );

    /* ���o���ĤH�� */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) ) continue;

      send_to_buffer( "%-20s %8d %8d %8d %8d\n\r"
        , man->name
        , man->level
        , get_curr_hit( man )
        , get_curr_mana( man )
        , get_curr_move( man ) );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg, "hitroll" ) )
  {
    send_to_buffer( "%-20s %8s %8s\n\r", "���a�m�W" , "����", "�R���v" );

    /* ���o���ĤH�� */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) ) continue;

      send_to_buffer( "%-20s %8d %8d\n\r"
        , man->name
        , man->level
        , get_hitroll( man ) );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg, "damroll" ) )
  {
    send_to_buffer( "%-20s %8s %8s\n\r", "���a�m�W" , "����", "�����O" );

    /* ���o���ĤH�� */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) ) continue;

      send_to_buffer( "%-20s %8d %8d\n\r"
        , man->name
        , man->level
        , get_damroll( man ) );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg, "armor" ) )
  {
    send_to_buffer( "%-20s %8s %8s\n\r", "���a�m�W" , "����", "���@�O" );

    /* ���o���ĤH�� */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) ) continue;

      send_to_buffer( "%-20s %8d %8d\n\r"
        , man->name
        , man->level
        , get_ac( man ) );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg , "filesize" ) )
  {
    send_to_buffer( "%-20s %8s %8s\n\r", "���a�m�W" , "����", "�ɮפj�p" );

    /* ���o���ĤH�� */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) || !man->pcdata ) continue;

      send_to_buffer( "%-20s %8d %s%s\e[0m\n\r"
        , man->name
        , man->level
        , man->pcdata->file_size > file_quota ? "\e[1;33m" : ""
        , numberize( man->pcdata->file_size, NULL ) );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg, "weight" ) )
  {
    send_to_buffer( "%-20s %8s %8s %8s\n\r"
      , "���a�m�W" , "����", "�t���W��", "�t��" );

    /* ���o���ĤH�� */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) ) continue;

      aNum = can_carry_w( man );
      bNum = get_carry_weight( man );

      send_to_buffer( "%-20s %8d %8d %s%8d\e[0m\n\r"
        , man->name, man->level, aNum
        , bNum > aNum ? "\e[1;31m" : ""
        , bNum );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg, "number" ) )
  {
    send_to_buffer( "%-20s %8s %8s %8s\n\r"
      , "���a�m�W" , "����", "�ƶq�W��", "�ƶq" );

    /* ���o���ĤH�� */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) ) continue;

      aNum = can_carry_n( man );
      bNum = man->carry_number;

      send_to_buffer( "%-20s %8d %8d %s%8d\e[0m\n\r"
        , man->name, man->level
        , aNum
        , bNum > aNum ? "\e[1;31m" : ""
        , bNum );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg , "alignment" ) )
  {
    send_to_buffer( "%-20s %8s %8s\n\r", "���a�m�W" , "����", "�}��" );

    /* ���o���ĤH�� */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) ) continue;

      send_to_buffer( "%-20s %8d %8d\n\r"
        , man->name, man->level, man->alignment );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg , "exp" ) )
  {
    send_to_buffer( "%-20s %8s %8s\n\r", "���a�m�W" , "����", "�g���" );

    /* ���o���ĤH�� */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) ) continue;

      send_to_buffer( "%-20s %8d %8d\n\r"
        , man->name, man->level, man->exp );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg , "eq" ) )
  {
    send_to_buffer( "%-20s %8s %8s\n\r", "���a�m�W" , "����", "�˳Ƨl��" );

    /* ���o���ĤH�� */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) ) continue;

      send_to_buffer( "%-20s %8d %8d\n\r"
        , man->name, man->level, man->eq_capcity );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg , "limit" ) )
  {
    send_to_buffer( "%-20s %8s %8s\n\r", "���a�m�W" , "����", "������" );

    /* ���o���ĤH�� */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) ) continue;

      send_to_buffer( "%-20s %8d %8d\n\r"
        , man->name, man->level, man->limit );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg , "sex" ) )
  {
    send_to_buffer( "%-20s %8s %8s\n\r", "���a�m�W" , "����", "�ʧO" );

    /* ���o���ĤH�� */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) ) continue;

      send_to_buffer( "%-20s %8d %8s\n\r"
        , man->name, man->level, sex_name( man->sex ) );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg , "firman" ) )
  {
    send_to_buffer( "%-20s %8s %8s\n\r", "���a�m�W" , "����", "�K�����P" );

    /* ���o���ĤH�� */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) ) continue;

      send_to_buffer( "%-20s %8d %8d\n\r"
        , man->name, man->level, man->firman );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg , "jail" ) )
  {
    send_to_buffer( "%-20s %8s %8s\n\r", "���a�m�W" , "����", "���D�ɶ�" );

    /* ���o���ĤH�� */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) ) continue;

      send_to_buffer( "%-20s %8d %8d\n\r"
        , man->name, man->level, man->jail );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg , "friend" ) )
  {
    send_to_buffer( "%-20s %8s %8s\n\r", "���a�m�W" , "����", "�n�ͼƥ�" );

    /* ���o���ĤH�� */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) ) continue;

      send_to_buffer( "%-20s %8d %8d\n\r"
        , man->name, man->level, friend_count( man ) );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg , "alias" ) )
  {
    send_to_buffer( "%-20s %8s %8s\n\r", "���a�m�W" , "����", "�����ƥ�" );

    /* ���o���ĤH�� */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) ) continue;

      send_to_buffer( "%-20s %8d %8d\n\r"
        , man->name, man->level, get_alias_count( man ) );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg , "enable" ) )
  {
    send_to_buffer( "%-20s %8s %8s\n\r", "���a�m�W" , "����", "�P��ƥ�" );

    /* ���o���ĤH�� */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) ) continue;

      send_to_buffer( "%-20s %8d %8d\n\r"
        , man->name, man->level, enable_count( man ) );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg , "deposit" ) )
  {
    send_to_buffer( "%-20s %8s %8s\n\r", "���a�m�W" , "����", "�H��ƥ�" );

    /* ���o���ĤH�� */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) ) continue;

      send_to_buffer( "%-20s %8d %8d\n\r"
        , man->name, man->level, deposit_count( man ) );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg , "steal" ) )
  {
    send_to_buffer( "%-20s %8s %8s\n\r", "���a�m�W" , "����", "���Ѧ���" );

    /* ���o���ĤH�� */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) || !man->pcdata ) continue;

      send_to_buffer( "%-20s %8d %8d\n\r"
        , man->name, man->level, man->pcdata->steal );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg , "murder" ) )
  {
    send_to_buffer( "%-20s %8s %8s\n\r", "���a�m�W" , "����", "�ѱ�����" );

    /* ���o���ĤH�� */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) || !man->pcdata ) continue;

      send_to_buffer( "%-20s %8d %8d\n\r"
        , man->name, man->level, man->pcdata->murder );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg, "notepad" ) )
  {
    send_to_buffer( "%-20s %8s %8s\n\r", "���a�m�W" , "����", "�O�ƥ�" );

    /* ���o���ĤH�� */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) || !ch->pcdata ) continue;

      for ( count = loop = 0; loop < MAX_NOTEPAD; loop++ )
      {
        if ( ch->pcdata->notepad[loop] && *( ch->pcdata->notepad[loop] ) )
          count++;
      }

      send_to_buffer( "%-20s %8d %8d\n\r"
        , man->name, man->level, count );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg , "automap" ) )
  {
    send_to_buffer( "%-20s %8s %8s\n\r", "���a�m�W" , "����", "�۰ʦa��" );

    /* ���o���ĤH�� */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || !man->pcdata ) continue;

      for ( count = loop = 0; loop < MAX_TRACE; loop++ )
        if ( man->pcdata->map[loop] ) count++;

      send_to_buffer( "%-20s %8d %8d\n\r"
        , man->name, man->level, count );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg, "cradle" ) )
  {
    send_to_buffer( "%-20s %8s %8s\n\r", "���a�m�W" , "����", "�X�ͦa" );

    /* ���o���ĤH�� */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) ) continue;

      send_to_buffer( "%-20s %8d %8s\n\r"
        , man->name, man->level, get_homename( man->home ) );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg , "xname" ) )
  {
    send_to_buffer( "%-20s %8s %8s\n\r", "���a�m�W" , "����", "�����r" );

    /* ���o���ĤH�� */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) || !man->pcdata ) continue;

      send_to_buffer( "%-20s %8d %8d\n\r"
        , man->name, man->level, man->pcdata->xname );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg , "nskill" ) )
  {
    send_to_buffer( "%-20s %8s %8s %8s\n\r"
      , "���a�m�W" , "����", "�ޯ�Ӽ�", "�i���`��" );

    /* ���o���ĤH�� */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) ) continue;

      send_to_buffer( "%-20s %8d %8d %8d\n\r"
        , man->name, man->level, learn_count( man ), get_learnable( ch ) );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg, "club" ) )
  {
    send_to_buffer( "%-20s %8s %8s\n\r", "���a�m�W" , "����", "�����W��" );

    /* ���o���ĤH�� */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) ) continue;

      send_to_buffer( "%-20s %8d %s\n\r"
        , man->name, man->level, club_name( man ) );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg, "class" ) )
  {
    send_to_buffer( "%-20s %8s %8s\n\r", "���a�m�W" , "����", "¾�~�W��" );

    /* ���o���ĤH�� */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) ) continue;

      send_to_buffer( "%-20s %8d %s\n\r"
        , man->name, man->level, class_name( man->class, FALSE ) );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg, "marry" ) )
  {
    send_to_buffer( "%-20s %8s %8s\n\r", "���a�m�W" , "����", "�B�ê��A" );

    /* ���o���ĤH�� */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) || !man->pcdata ) continue;

      send_to_buffer( "%-20s %8d %s\n\r"
        , man->name, man->level
        , YESNO( man->pcdata->mater && *man->pcdata->mater ) );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg, "fight" ) )
  {
    send_to_buffer( "%-20s %8s %8s %8s %8s\n\r"
      , "���a�m�W" , "����", "�����`��", "���ĵ���", "��������" );

    /* ���o���ĤH�� */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) || !man->pcdata ) continue;

      send_to_buffer( "%-20s %8d %8d %8d %8d\n\r"
        , man->name, man->level
        , man->pcdata->kills
        , man->pcdata->kill_level
        , man->pcdata->kills <= 0
          ? 0 : man->pcdata->kill_level / man->pcdata->kills );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg, "stock" ) )
  {
    int sn;

    one_argument( argument, arg );

    if ( arg[0] == '\x0' )
    {
      send_to_char( "�A�n�ݭ��@�i�Ѳ��������ơS\n\r", ch );
      RETURN_NULL();
    }

    if ( !is_number( arg ) || ( sn = atoi( arg ) - 1 ) < 0 || sn >= MAX_STOCK )
    {
      send_to_char( "�d����~�T�Ьd�� stock \n\r", ch );
      RETURN_NULL();
    }

    if ( !stock_data[sn].name )
    {
      send_to_char( "�o�a���q�|���W�d�M�L�k�[��C\n\r", ch );
      RETURN_NULL();
    }

    send_to_buffer( "%-20s %8s %s\n\r", "���a�m�W" , "����"
      , stock_data[sn].name );

    /* ���o���ĤH�� */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) || !man->pcdata ) continue;

      send_to_buffer( "%-20s %8d %-4d\n\r"
        , man->name, man->level, man->pcdata->stock[sn] );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg, "gift" ) )
  {
    GIFT_DATA * pGift;
    int         slot;
    int         loop;

    if ( !gift_list )
    {
      act( "$t�S������§���M�L�k�d�ߡT", ch, mud_name, NULL, TO_CHAR );
      RETURN_NULL();
    }

    one_argument( argument, arg );

    if ( arg[0] == '\x0' || !is_number( arg ) )
    {
      send_to_char( "�藍�_�M�A�n�d�߭��@��§���o�e���ΡS\n\r", ch );
      RETURN_NULL();
    }

    slot = atoi( arg );
    for ( loop = 1, pGift = gift_list; pGift; pGift = pGift->next, loop++ )
      if ( loop == slot ) break;

    if ( !pGift )
    {
      send_to_char( "�藍�_�M�S�����ӽs����§���T\n\r", ch );
      RETURN_NULL();
    }

   clear_buffer();
    send_to_buffer( "%-20s %8s %s(%d)\n\r"
      , "���a�m�W" , "����", pGift->title, slot );

    /* ���o���ĤH�� */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) ) continue;

      send_to_buffer( "%-20s %8d %s\n\r"
        , man->name, man->level
        , YESNO( !check_gift_stamp( man, pGift->stamp ) ) );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg, "skill" ) )
  {
    SKILL_DATA * pSkill;

    one_argument( argument, arg );

    if ( arg[0] == '\x0' )
    {
      send_to_char( "�A�n�ݭ��@�اޯ઺���m�סS\n\r", ch );
      RETURN_NULL();
    }

    if ( !( pSkill = skill_isname( arg ) ) )
    {
      act( "�藍�_�M�S���A�n���ޯ� $2$T$0�C", ch, NULL, arg, TO_CHAR );
      RETURN_NULL();
    }

    send_to_buffer( "%-20s %s���m��\n\r", "���a�m�W", pSkill->cname );

    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) || IS_NPC( man ) ) continue;
      send_to_buffer( "%-20s %3d\n\r", man->name, man->skill[pSkill->slot] );
    }
    print_buffer( ch );
    RETURN_NULL();
  }

  do_data( ch , "" );
  RETURN_NULL();
}

FUNCTION( do_xnames )
{
  XNAMES_DATA * xnames;
  XNAMES_DATA * prev;
  XNAMES_DATA * curr;
  char          arg[ MAX_INPUT_LENGTH ];
  char          type[ MAX_INPUT_LENGTH ];
  char        * msg;
  int           count;
  int           format;

  PUSH_FUNCTION( "do_xnames" );

  /* �D���a�������o�ӫ��O */
  if ( IS_NPC( ch ) ) RETURN_NULL();

  argument = one_argument( argument , arg );

  if ( !arg[0] )
  {
    clear_buffer();
    send_to_buffer( "�t�θ��J���O�d�r���R\n\r" );
    count = 0;

    for ( xnames = xnames_first, count = 0; xnames; xnames = xnames->next )
    {
      send_to_buffer( "%-18s �X�� %s%s\n\r"
        , xnames->type == XNAMES_CHAT && xnames->word && *xnames->word == '@'
          ? xnames->word + 1 : xnames->word
        , xnames_bit_name( xnames->type )
        , xnames->type == XNAMES_CHAT && xnames->word && *xnames->word == '@'
          ? " \x1b[1;32m���Ҽ{�j�p�g\x1b[0m" : "" );
      count++;
    }

    if ( count == 0 ) send_to_buffer( "�S������t�ΫO�d�r�Q���J�C\n\r" );
    else              send_to_buffer( "\n\r�@�� %d �t�ΫO�d�r�C\n\r" , count );

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( arg[0] == '+' )
  {
    one_argument( argument , type );

    for ( xnames = xnames_first; xnames; xnames = xnames->next )
    {
      if ( !str_cmp( xnames->word , arg + 1 ) )
      {
        act( "�藍�_�M�o�ӫO�d�r $2$T$0 �w�g���F�C" , ch, NULL, arg+1, TO_CHAR );
        RETURN_NULL();
      }
    }

    if ( !type[0] || !is_number( type ) )
    {
      send_to_char( "�Ьd�� xnames ���T����k�C\n\r" , ch );
      RETURN_NULL();
    }

    switch( ( format = atoi( type ) ) )
    {
    default :
      send_to_char( "�A���O�d�r�X�Ф���C\n\r" , ch );
      RETURN_NULL();

    case XNAMES_SYSTEM:
      msg = "�t�ΫO�d�r";
      break;

    case XNAMES_CACONYM:
      msg = "�����r";
      break;

    case XNAMES_CHAT :
      msg = "����O�d�r";
      break;

    case XNAMES_CNAME:
      msg = "����W�r�O�d�r";
      break;
    }

    /* �t�m�O���� */
    xnames = alloc_struct( STRUCT_XNAME_DATA );

    xnames->word = str_dup( arg + 1 );
    xnames->type = format;
    xnames->next = xnames_first;
    xnames_first = xnames;

    act( "�A��$t�]�w��$T�C", ch, arg+1, msg, TO_CHAR );
    RETURN_NULL();
  }

  else if ( arg[0] == '-' )
  {
    prev = NULL;
    for ( curr = xnames_first; curr; prev = curr, curr = curr->next )
    {
      if ( !str_cmp( arg + 1 , curr->word ) )
      {
        if ( curr->type == XNAMES_SYSTEM )
        {
          act( "�藍�_�M$2$T$0 �O�t�ΫO�d�r�M����R���C" , ch, NULL, arg+1, TO_CHAR );
          RETURN_NULL();
        }

        else
        {
          if ( prev == NULL ) xnames_first = xnames_first->next;
          else                prev->next   = curr->next;

          free_string( curr->word );
          curr->type  = 0;

          /* ���񵲺c */
          free_struct( curr , STRUCT_XNAME_DATA );

          act( "$2$T$0 ���A�O�O�d�r�C", ch, NULL, arg+1, TO_CHAR );
          RETURN_NULL();
        }
      }
    }

    send_to_char( "�䤣��A�n�R�����O�d�r�C\n\r" , ch );
    RETURN_NULL();
  }

  else
  {
    send_to_char( "�Ьd�� xnames ���T����k�C\n\r" , ch );
    RETURN_NULL();
  }

  RETURN_NULL();
}

FUNCTION( do_lockcmd )
{
  CMD_DATA * pCommand;
  int        trust;

  PUSH_FUNCTION( "do_lockcmd" );

  trust = get_trust( ch );

  for ( pCommand = cmd_list; pCommand; pCommand = pCommand->next )
  {
    if ( str_cmp( pCommand->name , argument ) || pCommand->level > trust )
      continue;

    if ( pCommand->canlock == FALSE )
    {
      act( "�A������w�o�ӫ��O $2$T$0�C" , ch , NULL, argument, TO_CHAR );
      RETURN_NULL();
    }

    if ( pCommand->lock == TRUE )
    {
      pCommand->lock = FALSE;
      act( "�A�Ѷ}���O$t����w�C" , ch, pCommand->name, NULL, TO_CHAR );
    }

    else
    {
      pCommand->lock = TRUE;
      act( "�A���F$t�o�ӫ��O�C" , ch, pCommand->name, NULL, TO_CHAR );
    }

    RETURN_NULL();
  }

  send_to_char( "�S�����A�n������O�C\n\r" , ch );
  RETURN_NULL();
}

FUNCTION( do_sector )
{
  SECTOR_DATA * pSector;
  char          arg[MAX_INPUT_LENGTH];
  bool          Found   = FALSE;
  bool          fAll    = FALSE;
  int           iSector = 0;
  int           count;

  PUSH_FUNCTION( "do_sector" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  one_argument( argument , arg );

  if ( !arg[0] )
  {
    fAll = TRUE;
  }

  else
  {
    if ( !is_number( arg ) )
    {
      send_to_char( "�a�ΰѼƥ����O�Ʀr�M�Ьd�� sector ���Ϊk�C\n\r" , ch );
      RETURN_NULL();
    }

    iSector = atoi( arg );
  }

  clear_buffer();
  for ( count = 0, pSector = sector_list; pSector; pSector = pSector->next )
  {
    if ( fAll || pSector->vnum == iSector )
    {
      if ( !Found ) send_to_buffer(
           "\e[1;33;44m���� �s�� ����W��             ����  "
           "�ƶq �Ѷ� �X��\e[0m\n\r"
           VERTICAL_LINE );
      Found = TRUE;
      send_to_buffer( "%2d). %4d %-20s %4d %5d %4s %s\n\r"
        , ++count
        , pSector->vnum
        , pSector->cname
        , pSector->movement
        , pSector->count
        , YESNO( pSector->dark )
        , sector_flag_name( pSector ) );
    }
  }

  if ( !Found ) send_to_buffer( "�S�����ŦX�A�n�D����ơC\n\r" );
  else send_to_buffer( "%s", VERTICAL_LINE );

  print_buffer( ch );
  RETURN_NULL();
}

FUNCTION( do_shop )
{
  char              arg[MAX_INPUT_LENGTH];
  char              buf[MAX_STRING_LENGTH];
  char              buf1[MAX_STRING_LENGTH];
  char              buf2[MAX_STRING_LENGTH];
  SHOP_DATA       * pShop;
  CHAR_DATA       * victim;
  MOB_INDEX_DATA  * pMobIndex;
  ROOM_INDEX_DATA * pRoom;
  int               count;
  int               iTrade;

  PUSH_FUNCTION( "do_shop" );

  one_argument( argument, arg );

  if ( !arg[0] )
  {
    clear_buffer();
    for ( count = 0, pShop = shop_first; pShop; pShop = pShop->next )
    {
      if ( !( pMobIndex = get_mob_index( pShop->keeper ) ) ) continue;

      if ( count++ == 0 ) send_to_buffer(
        "\e[1;33;44m���� ���A �c���            �s�� ��~�ɶ� �R"
        "��Q�q  �a�I ����                   \e[0m\n\r" );

      str_cpy( buf, mob_index_name( NULL, pMobIndex ) );
      buf[16] = '\x0';

      send_to_buffer( "%4d %4s %-16s %5d  %2d - %2d  %s%3d\e[0m"
         "%s-%3d\e[0m %5d"
         , count, shop_type_name( pShop->type )
         , buf
         , pMobIndex->vnum
         , pShop->open_hour, pShop->close_hour
         , pShop->profit_buy <= 100 ? "\e[1;31m" : ""
         , pShop->profit_buy
         , pShop->profit_sell >= 100 ? "\e[1;31m" : ""
         , pShop->profit_sell
         , pShop->sential ? pShop->sential->vnum : -1 );

      buf1[0] = '\x0';
      switch( pShop->type )
      {
      default:
        break;

      case SHOP_COPER:

        for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
        {
          if ( pShop->buy_type[iTrade] <= 0
            || !( pMobIndex = get_mob_index( pShop->buy_type[iTrade] ) ) )
            break;

          sprintf( buf2, " %d", pShop->buy_type[iTrade] );
          str_cat( buf1, buf2 );
        }

        break;

      case SHOP_STORE:

        for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
        {
          if ( pShop->buy_type[iTrade] <= 0 ) break;
          str_cat( buf1, " " );
          str_cat( buf1, item_kind_name( pShop->buy_type[iTrade] ) );
        }

        break;

      case SHOP_SMITH:

        for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
        {
          if ( pShop->buy_type[iTrade] <= 0 ) break;
          str_cat( buf1, " " );
          str_cat( buf1, wear_bit_name( pShop->buy_type[iTrade] ) );
        }
        break;
      }

      buf1[24] = '\x0';
      send_to_buffer( "%-24s\n\r", buf1 );
    }

    if ( count == 0 ) send_to_buffer( "�S���o�{���󪺰ө����A�C\n\r" );
    print_buffer( ch );
  }

  else if ( is_number( arg ) )
  {
    if ( !( pRoom = get_room_index( atoi( arg ) ) ) )
    {
      act( "�S���o�Ӧa�� $2$T$0�T", ch, NULL, arg, TO_CHAR );
      RETURN_NULL();
    }

    clear_buffer();
    count = 0;
    for ( victim = pRoom->people; victim; victim = victim->next_in_room )
    {
      if ( verify_char( victim )
        && IS_NPC( victim )
        && ( pMobIndex = victim->pIndexData )
        && ( pShop = pMobIndex->pShop ) )
      {
        if ( count++ != 0 ) send_to_buffer( "\n\r" );
        send_to_buffer(
          "����    �R%d\n\r"
          "���A    �R%s\n\r"
          "�c���  �R%s\n\r"
          "�D�H�s���R%d\n\r"
          "�}���ɶ��R%d\n\r"
          "���L�ɶ��R%d\n\r"
          "�Q�q��  �R%d-%d\n\r"
          "��~�a�I�R%d\n\r"
          "����    :"
          , count, shop_type_name( pShop->type )
          , mob_index_name( NULL, pMobIndex )
          , pMobIndex->vnum, pShop->open_hour, pShop->close_hour
          , pShop->profit_buy, pShop->profit_sell
          , pShop->sential ? pShop->sential->vnum : -1 );

        switch( pShop->type )
        {
        case SHOP_STORE:

          for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
          {
            if ( pShop->buy_type[iTrade] <= 0 ) break;
            send_to_buffer( " %s", item_kind_name( pShop->buy_type[iTrade] ) );
          }
          break;

        case SHOP_SMITH:

          for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
          {
            if ( pShop->buy_type[iTrade] <= 0 ) break;
            send_to_buffer( " %s", wear_bit_name( pShop->buy_type[iTrade] ) );
          }
          break;
        }

        send_to_buffer( "\n\r" );
      }
    }

    if ( count == 0 ) send_to_buffer( "���̨S�����󪺰ө����A�T\n\r" );
    print_buffer( ch );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "here" ) )
  {
    if ( !ch->in_room )
    {
      mudlog( LOG_DEBUG, "do_shop: �ӷ����~." );
      RETURN_NULL();
    }

    clear_buffer();
    count = 0;
    for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
      if ( verify_char( victim )
        && IS_NPC( victim )
        && ( pMobIndex = victim->pIndexData )
        && ( pShop = pMobIndex->pShop ) )
      {
        if ( count++ != 0 ) send_to_buffer( "\n\r" );
        send_to_buffer(
          "����    �R%d\n\r"
          "���A    �R%s\n\r"
          "�c���  �R%s\n\r"
          "�D�H�s���R%d\n\r"
          "�}���ɶ��R%d\n\r"
          "���L�ɶ��R%d\n\r"
          "�Q�q��  �R%d-%d\n\r"
          "��~�a�I�R%d\n\r"
          "����    �R"
          , count, shop_type_name( pShop->type )
          , mob_index_name( NULL, pMobIndex )
          , pMobIndex->vnum, pShop->open_hour, pShop->close_hour
          , pShop->profit_buy, pShop->profit_sell
          , pShop->sential ? pShop->sential->vnum : -1 );

        switch( pShop->type )
        {
        case SHOP_STORE:

          for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
          {
            if ( pShop->buy_type[iTrade] <= 0 ) break;
            send_to_buffer( " %s", item_kind_name( pShop->buy_type[iTrade] ) );
          }
          break;

        case SHOP_SMITH:

          for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
          {
            if ( pShop->buy_type[iTrade] <= 0 ) break;
            send_to_buffer( " %s", wear_bit_name( pShop->buy_type[iTrade] ) );
          }
          break;
        }

        send_to_buffer( "\n\r" );
      }
    }

    if ( count == 0 ) send_to_buffer( "�o�̨S�����󪺰ө����A�C\n\r" );
    print_buffer( ch );
    RETURN_NULL();
  }

  else
  {
    send_to_char( "�A���Ѽƿ��~�M�Ьd�� shop ���ϥΤ�k�C\n\r", ch );
    RETURN_NULL();
  }

  RETURN_NULL();
}

FUNCTION( do_site )
{
  int               count = 0;
  DESCRIPTOR_DATA * man;
  CHAR_DATA       * victim;

  PUSH_FUNCTION( "do_site" );

  clear_buffer();

  for ( man = descriptor_list; man; man = man->next )
  {
    if ( !verify_desc( man )
      || !man->host
      || !( victim = man->character )
      || str_prefix( argument, man->host ) ) continue;

    send_to_buffer( "%3d. %12s %25s\n\r"
      , ++count, victim->name, man->host );
  }

  if ( count == 0 ) send_to_char( "�S�����ŦX�A�����}�����a�C\n\r", ch );
  else  print_buffer( ch );

  RETURN_NULL();
}

FUNCTION( do_damage )
{
  SKILL_DATA  * pSkill;
  DAMAGE_DATA * pDam;
  EFFECT_DATA * pEffect;
  int           count = 0;
  int           loop;
  int           Number;
  int           chance = 0;
  char          arg[MAX_INPUT_LENGTH];
  char          buf[MAX_INPUT_LENGTH];

  PUSH_FUNCTION( "do_damage" );

  argument = one_argument( argument, arg );

  if ( !arg[0] || !( pSkill = skill_isname( arg ) ) )
  {
    act( "�䤣��A�n���k�N�άO�ޯ�W�� $2$T$0�C" , ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  if ( !pSkill->damage )
  {
    act( "�藍�_�M$w�S����˱ԭz�C", ch, pSkill, NULL, TO_CHAR );
    RETURN_NULL();
  }

  for ( pDam = pSkill->damage; pDam; pDam = pDam->next )
  {
    chance += pDam->chance;
    count++;
  }

  one_argument( argument, arg );
  chinese_number( count, buf );

  if ( !arg[0] || !is_number( arg ) )
  {
    clear_buffer();
    send_to_buffer( "%s(%s)�@��%s�ۡC ���v�`�M�� %d�M���O���R\n\r"
      , pSkill->cname, pSkill->name , buf, pSkill->chance );

    for ( loop = 0, pDam = pSkill->damage; pDam; pDam = pDam->next )
      send_to_buffer( "%2d. %3d(%3d%%) �ƭ� %3d �Ƽƭ� %d\n\r"
        , ++loop, pDam->chance
        , chance <= 0 ? 0 : pDam->chance * 100 / chance
        , pDam->value, pDam->vicevalue );

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( ( Number = atoi( arg ) ) <= 0 || Number > count )
  {
    act( "$w�u��$T�ӧ�˱ԭz�C", ch, pSkill, buf, TO_CHAR );
    RETURN_NULL();
  }

  for ( loop = 1, pDam = pSkill->damage; pDam; pDam = pDam->next )
    if ( loop++ == Number ) break;

  if ( !pDam )
  {
    act( "�䤣��A�n��$w����$T����˱ԭz�C", ch, pSkill, buf, TO_CHAR );
    RETURN_NULL();
  }

  clear_buffer();

  send_to_buffer(
    "�k�N�W�١R %s(%s) ���v�`�M�R %d\n\r"
    "���ǡR%2d/%2d ���v %3d/%3d(%3d%%) �ƭ� %d �Ƽƭ� %d\n\r"
    "��סR%3d �S����R%s\n\r"
    , pSkill->cname, pSkill->name, pSkill->chance
    , Number, count, pDam->chance, chance
    , chance <= 0 ? 0 : pDam->chance * 100 / chance
    , pDam->value, pDam->vicevalue
    , pDam->parry, attack_situs( pDam->situs ) );

  /* �O�_���S��Z�� */
  if ( pDam->innate )
  {
    send_to_buffer( "�S��Z���R%s(%s)(%d) �S��Z�����ơR%d%%\n\r"
    , pDam->innate->short_descr
    , pDam->innate->name
    , pDam->innate->vnum
    , pDam->multiple );
  }

  if ( pDam->routine ) send_to_buffer( "�o�ӧ�˦��S��T\n\r" );

  for ( pEffect = pDam->effect; pEffect; pEffect = pEffect->next )
  {
    send_to_buffer( "�����R%s �ƭȡR" , effect_type_name( pEffect->type ) );

    for ( loop = 0; loop < MAX_EFFECT_VALUE; loop++ )
      send_to_buffer( " %d", pEffect->value[loop] );

    send_to_buffer( "\n\r" );
  }

  send_to_buffer( "�y�z�R\n\r%s\n\r", pDam->description );
  print_buffer( ch );
  RETURN_NULL();
}

FUNCTION( do_liquid )
{
  LIQ_DATA * pLiq;

  PUSH_FUNCTION( "do_liq" );

  if ( !liq_list )
  {
    act( "$t�S�����󪺲G�骺��ơC" , ch, mud_name, NULL, TO_CHAR );
    RETURN_NULL();
  }

  send_to_char(
    "\e[1;42m�s��     �G��W�� �G���C�� ��� ���j �s�K  ��               "
    "                   \e[0m\n\r", ch );
  for ( pLiq = liq_list; pLiq; pLiq = pLiq->next )
  {
    print_to_char( ch, "%4d %12s %8s %4d %4d %4d  %s\n\r"
      , pLiq->slot
      , pLiq->name
      , pLiq->color
      , pLiq->thirst
      , pLiq->full
      , pLiq->drunk
      , YESNO( pLiq->water ) );
  }
  RETURN_NULL();
}

#define MEN_PER_LINE    5

FUNCTION( do_wizlist )
{
  IMMLIST_DATA * pImmlist;
  char           arg[MAX_INPUT_LENGTH];
  char           buf[1024];
  char           buf1[1024] = { '\x0' };
  char           buf2[1024] = { '\x0' };
  char           buf3[1024] = { '\x0' };
  char           buf4[1024] = { '\x0' };
  char           buf5[1024] = { '\x0' };
  int            imm_god = 0;
  int            imm_sup = 0;
  int            imm_dei = 0;
  int            imm_ang = 0;
  int            imm_her = 0;
  int            imm_adv = 0;

  PUSH_FUNCTION( "do_wizlist" );

  if ( !imm_list )
  {
    act( "�藍�_�M$t�̨S�����󪺺޲z�̡T", ch, mud_name, NULL, TO_CHAR );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg );
  if ( !arg[0] )
  {
    for ( pImmlist = imm_list; pImmlist; pImmlist = pImmlist->next )
    {
      switch ( UMAX( pImmlist->level, pImmlist->trust ) )
      {
      case L_GOD:

        if ( imm_god++ > 1 && ( imm_god - 1 ) % MEN_PER_LINE == 0 )
          str_cat( buf1, "\n\r" );

        if ( on_line( pImmlist->name, ch ) )
        {
          sprintf( buf, "\e[1;32m*%-14s\e[0m", pImmlist->name );
        }

        else if ( pImmlist->adviser == TRUE )
        {
          imm_adv++;
          sprintf( buf, "\e[1;36m%-15s\e[0m", pImmlist->name );
        }

        else
        {
          sprintf( buf, "%-15s", pImmlist->name );
        }

        str_cat( buf1, buf );
        break;

      case L_SUP:

        if ( imm_sup++ > 1 && ( imm_sup - 1 ) % MEN_PER_LINE == 0 )
          str_cat( buf2, "\n\r" );

        if ( on_line( pImmlist->name, ch ) )
        {
          sprintf( buf, "\e[1;32m*%-14s\e[0m", pImmlist->name );
        }

        else if ( pImmlist->adviser == TRUE )
        {
          imm_adv++;
          sprintf( buf, "\e[1;36m%-15s\e[0m", pImmlist->name );
        }

        else
        {
          sprintf( buf, "%-15s", pImmlist->name );
        }

        str_cat( buf2, buf );
        break;

      case L_DEI:

        if ( imm_dei++ > 1 && ( imm_dei - 1 ) % MEN_PER_LINE == 0 )
          str_cat( buf3, "\n\r" );

        if ( on_line( pImmlist->name, ch ) )
        {
          sprintf( buf, "\e[1;32m*%-14s\e[0m", pImmlist->name );
        }

        else if ( pImmlist->adviser == TRUE )
        {
          imm_adv++;
          sprintf( buf, "\e[1;36m%-15s\e[0m", pImmlist->name );
        }

        else
        {
          sprintf( buf, "%-15s", pImmlist->name );
        }

        str_cat( buf3, buf );
        break;

      case L_ANG:

        if ( imm_ang++ > 1 && ( imm_ang - 1 ) % MEN_PER_LINE == 0 )
          str_cat( buf4, "\n\r" );

        if ( on_line( pImmlist->name, ch ) )
        {
          sprintf( buf, "\e[1;32m*%-14s\e[0m", pImmlist->name );
        }

        else if ( pImmlist->adviser == TRUE )
        {
          imm_adv++;
          sprintf( buf, "\e[1;36m%-15s\e[0m", pImmlist->name );
        }
        else
        {
          sprintf( buf, "%-15s", pImmlist->name );
        }

        str_cat( buf4, buf );
        break;

      case L_HER:

        if ( imm_her++ > 1 && ( imm_her - 1 ) % MEN_PER_LINE == 0 )
          str_cat( buf5, "\n\r" );

        if ( on_line( pImmlist->name, ch ) )
        {
          sprintf( buf, "\e[1;32m*%-14s\e[0m", pImmlist->name );
        }

        else if ( pImmlist->adviser == TRUE )
        {
          imm_adv++;
          sprintf( buf, "\e[1;36m%-15s\e[0m", pImmlist->name );
        }
        else
        {
          sprintf( buf, "%-15s", pImmlist->name );
        }

        str_cat( buf5, buf );
        break;
      }
    }

    clear_buffer();
    send_to_buffer( "\e[1;31m%s\e[0m���޲z�����R(�ثe�@���U�� %d ��)\n\r"
      , mud_name, imm_adv );

    if ( imm_god > 0 )
    {
      send_to_buffer( "----------------------------\e[1;37m    �ѫ�      "
        "  �a�� \e[0m   ----------------------------\n\r" );
      send_to_buffer( "%s\n\r", buf1 );
    }

    if ( imm_sup > 0 )
    {
      send_to_buffer( "----------------------------\e[1;35m  �V�@�]��    "
        "ù�٤k��\e[0m  ----------------------------\n\r" );
      send_to_buffer( "%s\n\r", buf2 );
    }

    if ( imm_dei > 0 )
    {
      send_to_buffer( "---------------------------- \e[1;36m   ����       "
        "�]�e��\e[0m   ----------------------------\n\r" );
      send_to_buffer( "%s\n\r", buf3 );
    }

    if ( imm_ang > 0 )
    {
      send_to_buffer( "---------------------------- \e[1;31m   �ѥP     "
        "   �P�x\e[0m    ----------------------------\n\r" );
      send_to_buffer( "%s\n\r", buf4 );
    }

    if ( imm_her > 0 )
    {
      send_to_buffer( "----------------------------    \e[1;32m�@�k      "
        "  �L�`    \e[0m----------------------------\n\r" );
      send_to_buffer( "%s\n\r", buf5 );
    }

    send_to_buffer( "------------------------------------------"
        "--------------------------------------\n\r" );
    print_buffer( ch );
    RETURN_NULL();
  }

  else
  {
    if ( !( pImmlist = imm_lookup( arg ) ) )
    {
      act( "�藍�_�M�S���o��޲z�� $2$T$0 ��T", ch, NULL, arg, TO_CHAR );
      RETURN_NULL();
    }

    print_to_char( ch, "�޲z���W�١R%s\n\r"
                       "����      �R%d\n\r"
                       "�O�_�b�u�W�R%s\n\r"
                       "�U��      �R%s\n\r"
      , pImmlist->name, UMAX( pImmlist->trust, pImmlist->level )
      , YESNO( on_line( pImmlist->name, ch ) )
      , YESNO( pImmlist->adviser ) );

   if ( pImmlist->description && *pImmlist->description )
   {
     char * pString;

     pString = pImmlist->description;

     act( "�ۧڤ���  �R$A$t"
       , ch, *pString == '.' ? pString + 1 : pString, NULL, TO_CHAR );
   }

   else
   {
     act( "��p�M$2$T$0 �S���ۧڤ��СC", ch, NULL, arg, TO_CHAR );
   }
  }

  RETURN_NULL();
}

#undef MEN_PER_LINE

FUNCTION( do_cleanup )
{
  PUSH_FUNCTION( "do_cleanup" );

  if ( !IS_NPC( ch ) )
  {
    system_cleanup();
    send_to_char( "�M���L�������a�ɮסT\n\r", ch );
  }
  RETURN_NULL();
}

void system_cleanup( void )
{
  char            directory[MAX_FILE_LENGTH];
  char            filename[MAX_FILE_LENGTH];
  char            exec_cmd[MAX_STRING_LENGTH];
  char            header;
  DIR           * reading;
  struct dirent * next;
  struct stat     Status;
  time_t          nTimes;
  time_t          nHold;

  PUSH_FUNCTION( "system_cleanup" );

  nTimes = time( NULL );
  nHold  = hold_day * 24 * 60 * 60;

  for ( header = 'a'; header <= 'z'; header++ )
  {
    sprintf( directory, "%s/%c/", player_dir, header );

    /* �}�Ҥl�ؿ� */
    if ( ( reading = opendir( directory ) ) )
    {
      /* Ū���ؿ������e */
      while ( ( next = readdir( reading ) ) )
      {
        if ( !strcmp( next->d_name, "." ) || !strcmp( next->d_name, ".." ) )
          continue;

        sprintf( filename, "%s%s", directory, next->d_name );
        if ( !is_directory( filename ) ) continue;

        sprintf( filename, "%s%s/data", directory, next->d_name );

        if ( stat( filename, &Status ) == 0 )
        {
          if ( S_ISREG( Status.st_mode ) )
          {
            if ( imm_lookup( next->d_name ) ) continue;

            if ( ( nTimes - Status.st_mtime ) > nHold || Status.st_size <= 0 )
            {
              sprintf( exec_cmd, "rm -rf %s%s", directory, next->d_name );
              system( exec_cmd );

              /* �󥿭^���] */
              check_hero( next->d_name );

              /* �󥿸�Ʈw */
              erase_database( next->d_name );

              /* ������ */
              name_from_club( next->d_name, FALSE );
            }
          }
        }

        else
        {
          mudlog( LOG_DEBUG, "do_cleanup: �L�k���o%s���ɮת��A.", filename );
        }
      }

      closedir( reading );
      save_club();
    }
    else
    {
      mudlog( LOG_DEBUG, "do_cleanup: �L�k�}�ҥؿ� %s.", directory );
    }
  }

  RETURN_NULL();
}

FUNCTION( do_jail )
{
  ROOM_INDEX_DATA * location;
  CHAR_DATA       * victim;
  DESCRIPTOR_DATA * pDesc;
  char              arg1[MAX_INPUT_LENGTH];
  char              arg2[MAX_INPUT_LENGTH];
  char              buf[MAX_STRING_LENGTH];
  int               duration;
  int               count;

  PUSH_FUNCTION( "do_jail" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  if ( !RoomJail )
  {
    act( "$t���S���Ѩc�M�Ҧ��ʧ@�����C", ch, mud_name, NULL, TO_CHAR );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );

  if ( arg1[0] == '\x0' || arg2[0] == '\x0' )
  {
    send_to_char( "�Ѽƿ��~�M�Ьd�ߥ��T���ϥΤ�k�T\n\r", ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg1, "!address" ) )
  {
    one_argument( argument, arg1 );

    if ( !is_number( arg1 ) || ( duration = atoi( arg1 ) ) <= 0 )
    {
      act( "�A�n�� $2$t$0 �h�[���S", ch, arg1, NULL, TO_CHAR );
      RETURN_NULL();
    }

    for ( count = 0, pDesc = descriptor_list; pDesc; pDesc = pDesc->next )
    {
      if ( !verify_desc( pDesc )
        || !( victim = pDesc->character )
        || pDesc->connected != CON_PLAYING
        || strcmp( arg2, pDesc->host )
        || victim == ch )
        continue;

      act( "$N�ŦX��} $t ������C", ch, arg2, victim, TO_CHAR );

      if ( jail_someone( ch, victim, duration, TRUE ) )
      {
        sprintf( buf, "%s�ǤF�ѱ��M�Q�޲z�̮��_�ӤF�C"
          , mob_name( NULL, victim ) );

        talk_channel_2( buf, CHANNEL_BULLETIN, "�ֳ�" );
        count++;
      }
    }

    if ( count <= 0 ) send_to_char( "�藍�_�M�䤣��o�Ӧ�}�����a�C\n\r", ch );
    else act( "�A�`�@���F $i �쪱�a�C", ch, &count, NULL, TO_CHAR );

    RETURN_NULL();
  }

  if ( !str_prefix( arg1, "!set" ) )
  {
    if ( !( victim = get_char_world( ch, arg2 ) ) )
    {
      act( "�䤣��A�n�����H $2$T$0 �C�C", ch, NULL, arg2, TO_CHAR );
      RETURN_NULL();
    }

    if ( IS_NPC( victim ) )
    {
      act( "$N���ӴN���|�Q���F�C", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( victim->jail <= 0 )
    {
      act( "$N���ӴN�S���Q���_�ӡC", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    one_argument( argument, arg2 );

    if ( arg2[0] == '\x0'
      || !is_number( arg2 )
      || ( duration = atoi( arg2 ) ) <= 0 )
    {
      send_to_char( "�A������ܥL���A�D�ɶ����h�֫��S\n\r", ch );
      RETURN_NULL();
    }

    act( "�޲z�̧�A���A�D�ɶ��令$i�ѡT", victim, &duration, NULL, TO_CHAR );
    act( "�A��$N���A�D�ɶ��令$i�ѡT", ch, &duration, victim, TO_CHAR );

    victim->jail = duration;
    RETURN_NULL();
  }

  else if ( !str_prefix( arg1, "!release" ) )
  {
    if ( !( victim = get_char_world( ch, arg2 ) ) )
    {
      act( "�䤣��A�n���񪺤H $2$T$0 �C�T", ch, NULL, arg2, TO_CHAR );
      RETURN_NULL();
    }

    if ( IS_NPC( victim ) )
    {
      act( "�D���a$N���ӴN���|�Q���F�T", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( victim->jail <= 0 )
    {
      act( "$N���ӴN�S���Q���_�ӡT", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( !victim->in_room || victim->in_room != RoomJail )
    {
      act( "$N�k���F�C", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( victim->position != POS_STANDING )
    {
      act( "����$N�s�_�ӧa�C", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( !( location = get_hometown( victim ) ) )
    {
      act( "�~���@�ɤӶ·t�F�M�z�ܥs$N�ݦb�o�̧a�T", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( victim->fighting ) stop_fighting( victim, TRUE );
    if ( victim->mount    ) unmount_char( victim, victim->mount    );
    if ( victim->mount_by ) unmount_char( victim->mount_by, victim );

    /* �ץ��p�� */
    if ( ch->spirit ) spirit_from_char( ch->spirit );

    send_to_char( "�ޡM�A�i�H�X���o�M�Ʊ�A�n�n���H�T\n\r", victim );
    act( "����$n�i�H�X���o�C", victim, NULL, NULL, TO_ROOM );
    char_from_room( victim );
    char_to_room( victim, location );
    act( "$n�b�o�̶}�l�q�Y���H�C", victim, NULL, NULL, TO_ROOM );
    do_look( victim, "auto" );

    /* �M���l�ܬ����I */
    clear_trace( victim, TRUE );

    victim->jail = 0;

    act( "�A��$N������F�C", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  else
  {
    if ( !is_number( arg2 ) || ( duration = atoi( arg2 ) ) <= 0 )
    {
      send_to_char( "�A�n���L�h�[���S\n\r", ch );
      RETURN_NULL();
    }

    if ( !( victim = get_char_world( ch, arg1 ) ) )
    {
      act( "�䤣��A�n������H $2$T$0�T", ch, NULL, arg1, TO_CHAR );
      RETURN_NULL();
    }

    if ( jail_someone( ch, victim, duration, TRUE ) )
    {
      sprintf( buf, "%s�ǤF�ѱ��M�Q�޲z�̮��_�ӤF�C"
        , mob_name( NULL, victim ) );

      talk_channel_2( buf, CHANNEL_BULLETIN, "�ֳ�" );
    }

    RETURN_NULL();
  }

  RETURN_NULL();
}

FUNCTION( do_sset )
{
  PUSH_FUNCTION( "do_sset" );
  RETURN_NULL();
}

FUNCTION( do_level )
{
  int  level;
  char arg[MAX_INPUT_LENGTH];

  PUSH_FUNCTION( "do_level" );

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    send_to_char( "�o�ӫ��O�ݭn�ѼơM�Ьd�߫��O�Ϊk�C\n\r", ch );
    RETURN_NULL();
  }

  if ( is_number( arg ) )
  {
    if ( ( level = atoi( arg ) ) < 0 || level >= MAX_LEVEL )
    {
      send_to_char( "�A�n�d�ߪ����Ť��X�z�T\n\r", ch );
      RETURN_NULL();
    }

    print_to_char( ch,
      "���a����       %-d\n\r"
      "�ɯŪ�����     %-d\n\r"
      "�ɯŪ��g���   %-d\n\r"
      "�Ǫ�������     %-d\n\r"
      "�Ǫ������`���� %-d\n\r"
      "�Ǫ����ˮ`�O   %-d\n\r"
      "�Ǫ����{���O   %-d\n\r"
      , level
      , kill_table[level].promotion
      , kill_table[level].exp
      , kill_table[level].number
      , kill_table[level].killed
      , kill_table[level].damage
      , kill_table[level].dodge );
  }

  else
  {
    send_to_char( "���~���ѼơM�Ьd�߫��O���Ϊk�C\n\r", ch );
  }
  RETURN_NULL();
}

FUNCTION( do_sale )
{
  SALE_DATA      * pSale;
  CHAR_DATA      * victim;
  OBJ_INDEX_DATA * obj;
  MOB_INDEX_DATA * mob;
  int              count;

  PUSH_FUNCTION( "do_sale" );

  if ( !sale_list )
  {
    act( "�藍�_�M$t�S�����󪺩��~�� �T", ch, mud_name, NULL, TO_CHAR );
    RETURN_NULL();
  }

  clear_buffer();
  send_to_buffer( "\e[1;33;44m���� �� �X ��   �� ���ұo ���� ���� ��X "
    "��檫�~�W��                         \e[0m\n\r" );

  for ( count = 1, pSale = sale_list; pSale; pSale = pSale->next, count++ )
  {
    if ( !( obj = pSale->obj ) ) continue;

    send_to_buffer( "%3d. %5d %7d %8d  %s  %4d %4d %s(%s)\n\r"
      , count, obj->vnum, pSale->cost, pSale->gold, YESNO( pSale->visible )
      , pSale->times, pSale->sold, obj->short_descr, obj->name );
  }

  for ( count = 0, victim = char_list; victim; victim = victim->next )
  {
    if ( verify_char( victim )
      && ( mob = victim->pIndexData )
      && mob->auction
      && *mob->auction )
    {
      if ( ++count == 1 )
        send_to_buffer( "\n\r\e[1;33;44m���� �� �X ���Ǫ��W��           "
          "                                              \e[0m\n\r" );
      send_to_buffer( "%3d. %5d %s\n\r"
        , count, mob->vnum, mob_index_name( NULL, mob ) );
    }
  }

  if ( count == 0 ) send_to_buffer( "�t�Υثe�S�����󪺩��̡T\n\r" );

  send_to_buffer( "\n\r�����v�R�d���� %d�C\n\r�W�u�H�ƤU���R%d �H\n\r"
    , SaleChance, SaleLimit );

  print_buffer( ch );
  RETURN_NULL();
}

bool jail_someone( CHAR_DATA * ch, CHAR_DATA * victim, int dur, bool fPrint )
{
  PUSH_FUNCTION( "jail_someone" );

  if ( !victim )
  {
    mudlog( LOG_DEBUG, "jail_someone: �ӷ����~." );
    RETURN( FALSE );
  }

  if ( dur <= 0 )
  {
    mudlog( LOG_DEBUG, "jail_someone: �ɶ����X�z %d.", dur );
    RETURN( FALSE );
  }

  if ( !RoomJail )
  {
    if ( ch && fPrint ) send_to_char( "�t�Τ��S���Ѩc�M�Ҧ��ʧ@�����C\n\r", ch );
    RETURN( FALSE );
  }

  if ( IS_NPC( victim ) )
  {
    if ( ch && fPrint )
      act( "$N����L�d�M�A�F�������O�S", ch, NULL, victim, TO_CHAR );

    RETURN( FALSE );
  }

  if ( IS_IMMORTAL( victim ) )
  {
    if ( ch && fPrint )
      act( "�A�n��$N�ܡS�L�O���C�M�n�}�|�q�L�a�T", ch, NULL, victim, TO_CHAR );

    RETURN( FALSE );
  }

  if ( victim->jail > 0 )
  {
    if ( ch && fPrint ) act( "$N�w�g�b���D�o�T", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  if ( !victim->in_room )
  {
    if ( ch && fPrint ) act( "$N���ж������~�T", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  if ( victim->pcdata && victim->pcdata->corpse )
  {
    if ( ch && fPrint )
      act( "���L�ߧ����^�A���a�T", ch, NULL, victim, TO_CHAR );

    RETURN( FALSE );
  }

  if ( victim->fighting ) stop_fighting( victim, TRUE );
  if ( victim->mount    ) unmount_char( victim, victim->mount    );
  if ( victim->mount_by ) unmount_char( victim->mount_by, victim );

  /* ��L���p���������� */
  if ( victim->spirit ) spirit_from_char( victim->spirit );

  /* ��L���ħL�������� */
  if ( victim->mercenary ) all_mercenary_from_char( victim );

  /* �p�G�b��L���M�h�������L��L */
  victim->failed = 0;

  victim->jail = dur;

  /* �M���l�ܬ����I */
  clear_trace( victim, TRUE );

  send_to_char( "�藍�_�M�A�ǤF�ѱ��M�A�Q�ʸT�F�T\n\r", victim );
  act( "$n�ǤF�ѱ��M�Q�O�w���h���F�T", victim, NULL, NULL, TO_ROOM );
  char_from_room( victim );
  char_to_room( victim, RoomJail );
  act( "$n�ǤF�ѱ��M�q���Ѱ_�N�O�A�̪��s���F�T", victim, NULL, NULL, TO_ROOM );
  do_look( victim, "auto" );

  if ( victim->pcdata ) victim->pcdata->imprison++;
  RETURN( TRUE );
}

FUNCTION( do_nuke )
{
  int                loop;
  char               arg[MAX_INPUT_LENGTH];
  char               exec_cmd[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA * man;
  CHAR_DATA       * victim;

  PUSH_FUNCTION( "do_nuke" );

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    send_to_char( "�藍�_�M�A�n�娺�ӤH���ɮשO�S\n\r", ch );
    RETURN_NULL();
  }

   /* �ഫ���p�g */
   for ( loop = 0; arg[loop]; loop++ )
     if ( arg[loop] >= 'A' && arg[loop] <= 'Z' ) arg[loop] += 'a' - 'A';

  if ( imm_lookup( arg ) )
  {
    act( "$T �O���ڦ������@�M����γo�ؤ覡�Ӭ��ɡT", ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  if ( !is_exist( arg ) )
  {
    act( "$t�̨S��$T�o�@���H����T", ch, mud_name, arg, TO_CHAR );
    RETURN_NULL();
  }

  if ( ( victim = is_online( arg ) ) )
  {
    act( "�藍�_�M$N���b�u�W�M������$E���u�~����ɡT", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  for ( man = descriptor_list; man; man = man->next )
  {
    if ( verify_desc( man ) && !str_cmp( man->username, arg ) )
    {
      act( "�藍�_�M$T ���b�i�J�t�ΡM�L�k���ɡT", ch, NULL, arg, TO_CHAR );
      RETURN_NULL();
    }
  }

  sprintf( exec_cmd, "rm -rf %s/%c/%s", player_dir, arg[0], arg );
  system( exec_cmd );

  /* �󥿭^���] */
  check_hero( arg );

  /* �󥿸�Ʈw */
  erase_database( arg );

  /* ������ */
  if ( name_from_club( arg, TRUE ) ) save_club();

  act( "�A�w�g�N $t �o�H�q$T�ä[�a�����F�T", ch, arg, mud_name, TO_CHAR );
  RETURN_NULL();
}

FUNCTION( do_miset )
{
  MOB_INDEX_DATA * pIndex;
  char             arg1[MAX_INPUT_LENGTH];
  char             arg2[MAX_INPUT_LENGTH];
  char             arg3[MAX_INPUT_LENGTH];
  char             buf[MAX_STRING_LENGTH];
  char             buf1[MAX_STRING_LENGTH];
  char             mobname[MAX_STRING_LENGTH];
  CLASS_DATA     * pClass;
  int              value;
  int              vnum;

  PUSH_FUNCTION( "do_miset" );

  smash_tilde( argument );
  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );
  str_cpy( arg3, argument );

  if ( !arg1[0] || !arg2[0] || !arg3[0] )
  {
    send_to_char(
      "�y�k�Rmiset <��H���X> <����> <�ƭ�>\n\r"
      "�άO  miset <��H���X> <�r��> <�ƭ�>\n\r\n\r"
      "���إi�H���U�C��ܡR\n\r"
      "  flags strength intelligence wisdom dexterity constitutin sex\n\r"
      "  class gold hp mana move aligment name short long dodge attack\n\r"
      "  tractable migrate multipile deadmsg auction descrption affect\n\r\n\r"
      , ch );

    RETURN_NULL();
  }

  if ( !is_number( arg1 ) )
  {
    send_to_char( "�A���������Ǫ������X�M�~����T\n\r", ch );
    RETURN_NULL();
  }

  vnum = atoi( arg1 );
  if ( !( pIndex = get_mob_index( vnum ) ) )
  {
    act( "�S�����X $i ���Ǫ��T", ch, &vnum, NULL, TO_CHAR );
    RETURN_NULL();
  }

  str_cpy( mobname, mob_index_name( NULL, pIndex ) );

  value = is_number( arg3 ) ? atoi( arg3 ) : -1;

  if ( !str_prefix( arg2, "flags" ) )
  {
    if ( !str_prefix( arg3, "sentinel" ) )
    {
      if ( IS_SET( pIndex->act, ACT_SENTINEL ) )
      {
        REMOVE_BIT( pIndex->act, ACT_SENTINEL );
        act( "$T���A�O���i���ʡT", ch, NULL, mobname, TO_CHAR );
      }
      else
      {
        SET_BIT( pIndex->act, ACT_SENTINEL );
        act( "$T�S��_���i���ʪ��ʽ�T", ch, NULL, mobname, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "scavenger" ) )
    {
      if ( IS_SET( pIndex->act, ACT_SCAVENGER ) )
      {
        REMOVE_BIT( pIndex->act, ACT_SCAVENGER );
        act( "$T���A�O�|�۰ʾߪ��~�T", ch, NULL, mobname, TO_CHAR );
      }
      else
      {
        SET_BIT( pIndex->act, ACT_SCAVENGER );
        act( "$T�S��_�|�۰ʾߪ��~���ʽ�T", ch, NULL, mobname, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "aggressive" ) )
    {
      if ( IS_SET( pIndex->act, ACT_AGGRESSIVE ) )
      {
        REMOVE_BIT( pIndex->act, ACT_AGGRESSIVE );
        act( "$T���A�O�|�۰ʧ����T", ch, NULL, mobname, TO_CHAR );
      }
      else
      {
        SET_BIT( pIndex->act, ACT_AGGRESSIVE );
        act( "$T�S��_�|�۰ʧ������ʽ�T", ch, NULL, mobname, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "stayarea" ) )
    {
      if ( IS_SET( pIndex->act, ACT_STAY_AREA ) )
      {
        REMOVE_BIT( pIndex->act, ACT_STAY_AREA );
        act( "$T���A�O�|���d�b���ϰ�T", ch, NULL, mobname, TO_CHAR );
      }
      else
      {
        SET_BIT( pIndex->act, ACT_STAY_AREA );
        act( "$T�S��_�|���d�b���ϰ쪺�ʽ�T", ch, NULL, mobname, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "wimpy" ) )
    {
      if ( IS_SET( pIndex->act, ACT_WIMPY ) )
      {
        REMOVE_BIT( pIndex->act, ACT_WIMPY );
        act( "$T���A�O�|���˰k�]�T", ch, NULL, mobname, TO_CHAR );
      }
      else
      {
        SET_BIT( pIndex->act, ACT_WIMPY );
        act( "$T�S��_�|���˰k�]���ʽ�T", ch, NULL, mobname, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "train" ) )
    {
      if ( IS_SET( pIndex->act, ACT_TRAIN ) )
      {
        REMOVE_BIT( pIndex->act, ACT_TRAIN );
        act( "$T���A�O�V�m�ݩʪ̡T", ch, NULL, mobname, TO_CHAR );
      }
      else
      {
        SET_BIT( pIndex->act, ACT_TRAIN );
        act( "$T�S��_�V�m�ݩʪ̪��ʽ�T", ch, NULL, mobname, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "rebirth" ) )
    {
      if ( IS_SET( pIndex->act, ACT_REBIRTH ) )
      {
        REMOVE_BIT( pIndex->act, ACT_REBIRTH );
        act( "$T���A�O��¾�̡T", ch, NULL, mobname, TO_CHAR );
      }
      else
      {
        SET_BIT( pIndex->act, ACT_REBIRTH );
        act( "$T�S��_��¾�̪��ʽ�T", ch, NULL, mobname, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "fight" ) )
    {
      if ( IS_SET( pIndex->act, ACT_REBORN_FIGHT ) )
      {
        REMOVE_BIT( pIndex->act, ACT_REBORN_FIGHT );
        act( "$T���A�O�|�A�ͦ���L�Ǫ��T", ch, NULL, mobname, TO_CHAR );
      }
      else
      {
        SET_BIT( pIndex->act, ACT_REBORN_FIGHT );
        act( "$T�S��_�|�A�ͦ���L�Ǫ����ʽ�T", ch, NULL, mobname, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "ask" ) )
    {
      if ( IS_SET( pIndex->act, ACT_ASK ) )
      {
        REMOVE_BIT( pIndex->act, ACT_ASK );
        act( "$T���A�O�|�s�H���u�T", ch, NULL, mobname, TO_CHAR );
      }
      else
      {
        SET_BIT( pIndex->act, ACT_ASK );
        act( "$T�S��_�|�s�H���u���ʽ�T", ch, NULL, mobname, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "noreborn" ) )
    {
      if ( IS_SET( pIndex->act, ACT_NOREBORN ) )
      {
        REMOVE_BIT( pIndex->act, ACT_NOREBORN );
        act( "$T���A�O���|���͡T", ch, NULL, mobname, TO_CHAR );
      }
      else
      {
        SET_BIT( pIndex->act, ACT_NOREBORN );
        act( "$T�S��_�|���|���ͪ��ʽ�T", ch, NULL, mobname, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "nokill" ) )
    {
      if ( IS_SET( pIndex->act, ACT_NOKILL ) )
      {
        REMOVE_BIT( pIndex->act, ACT_NOKILL );
        act( "$T���A�O���i���`�T", ch, NULL, mobname, TO_CHAR );
      }
      else
      {
        SET_BIT( pIndex->act, ACT_NOKILL );
        act( "$T�S��_���i���`���ʽ�T", ch, NULL, mobname, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "enroll" ) )
    {
      if ( IS_SET( pIndex->act, ACT_ENROLL ) )
      {
        REMOVE_BIT( pIndex->act, ACT_ENROLL );
        act( "$T���A�O�|�O��T", ch, NULL, mobname, TO_CHAR );
      }
      else
      {
        SET_BIT( pIndex->act, ACT_ENROLL );
        act( "$T�S��_�O�몺�ʽ�T", ch, NULL, mobname, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "good" ) )
    {
      if ( IS_SET( pIndex->act, ACT_ALIGN_GOOD ) )
      {
        REMOVE_BIT( pIndex->act, ACT_ALIGN_GOOD );
        act( "$T���A�O���}���H�T", ch, NULL, mobname, TO_CHAR );
      }
      else
      {
        SET_BIT( pIndex->act, ACT_ALIGN_GOOD );
        act( "$T�S��_���}���H���ʽ�T", ch, NULL, mobname, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "evil" ) )
    {
      if ( IS_SET( pIndex->act, ACT_ALIGN_EVIL ) )
      {
        REMOVE_BIT( pIndex->act, ACT_ALIGN_EVIL );
        act( "$T���A�O���c���H�T", ch, NULL, mobname, TO_CHAR );
      }
      else
      {
        SET_BIT( pIndex->act, ACT_ALIGN_EVIL );
        act( "$T�S��_���c���H���ʽ�T", ch, NULL, mobname, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "nosummon" ) )
    {
      if ( IS_SET( pIndex->act,ACT_NOSUMMON ) )
      {
        REMOVE_BIT( pIndex->act, ACT_NOSUMMON );
        act( "$T���A�O���i�l�ꤧ�H�T", ch, NULL, mobname, TO_CHAR );
      }
      else
      {
        SET_BIT( pIndex->act, ACT_NOSUMMON );
        act( "$T�S��_���i�l�ꤧ�H���ʽ�T", ch, NULL, mobname, TO_CHAR );
      }
    }

    else if ( !str_prefix( arg3, "speak" ) )
    {
      if ( pIndex->speak )
      {
        pIndex->speak = FALSE;
        act( "$T���A�O���|���ܤ��H�T", ch, NULL, mobname, TO_CHAR );
      }
      else
      {
        pIndex->speak = TRUE;
        act( "$T�S��_�|���ܪ��ʽ�T", ch, NULL, mobname, TO_CHAR );
      }
    }

    else
    {
      send_to_char(
        "�H�����X�Хu�� sentinel scavenger aggressive stayarea wimpy\n\r"
        "               train rebirth fight ask noreborn nokill nosummon\n\r"
        "               enroll good evil speak\n\r"
        , ch );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "strength" ) )
  {
    if ( value < 0 || value > MaxStr )
    {
      act( "�u����ܤO�q�� 0 �� $x�C" , ch, &MaxStr, NULL, TO_CHAR );
    }

    else
    {
      pIndex->now_str = value;
      act( "�A�]�w$T�O�q�� $x�C", ch, &value, mobname, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "intelligence" ) )
  {

    if ( value < 0 || value > MaxInt )
    {
      act( "�u�഼�O�� 0 �� $x�C", ch , &MaxInt, NULL, TO_CHAR );
    }

    else
    {
      pIndex->now_int = value;
      act( "�A�]�w$T���O�� $x�C", ch, &value, mobname, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "wisdom" ) )
  {
    if ( value < 0 || value > MaxWis )
    {
      act( "�u����ܾ��ѥ� 0 �� $x�C", ch , &MaxWis, NULL, TO_CHAR );
    }

    else
    {
      pIndex->now_wis = value;
      act( "�A�]�w$T���Ѭ� $x�C", ch, &value, mobname, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "dexterity" ) )
  {
    if ( value < 0 || value > MaxDex )
    {
      act( "�u����ܱӱ��� 0 �� $x�C" , ch, &MaxDex, NULL, TO_CHAR );
    }

    else
    {
      pIndex->now_dex = value;
      act( "�A�]�w$T�ӱ��� $x�C", ch, &value, mobname, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "constitutin" ) )
  {
    if ( value < 0 || value > MaxCon )
    {
      act( "�u��������� 0 �� $x�C" , ch, &MaxCon, NULL, TO_CHAR );
    }

    else
    {
      pIndex->now_con = value;
      act( "�A�]�w$T��欰 $x�C", ch, &value, mobname, TO_CHAR );
    }

   RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "sex" ) )
  {
         if ( !str_prefix( arg3, "male"   ) ) value = SEX_MALE;
    else if ( !str_prefix( arg3, "female" ) ) value = SEX_FEMALE;

    switch( value )
    {
    default:
      print_to_char( ch, "�ʧO�u�� %s(%d) �M%s(%d)�C\n\r"
        , sex_name( SEX_MALE )  , SEX_MALE
        , sex_name( SEX_FEMALE ), SEX_FEMALE );
      break;

    case SEX_FEMALE:
      pIndex->sex = SEX_FEMALE;
      act( "�A��$T�ʧO�令$t�F�C", ch, sex_name( pIndex->sex ), mobname, TO_CHAR );
      break;

    case SEX_MALE:
      pIndex->sex = SEX_MALE;
      act( "�A��$T�ʧO�令$t�F�C", ch, sex_name( pIndex->sex ), mobname, TO_CHAR );
      break;
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "class" ) )
  {
    if ( !( pClass = is_class( arg3, TRUE ) ) )
    {
      act( "¾�~ $2$t$0 �W�٤��X�k�C" , ch, arg3, NULL, TO_CHAR );
      RETURN_NULL();
    }

    pIndex->class = pClass;
    act( "�A��$T¾�~�令$t�F�C", ch , class_name( pClass, TRUE ), mobname, TO_CHAR );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "gold" ) )
  {
    if ( value < 0 )
    {
      send_to_char( "������ܪ������t�ȡC\n\r" , ch );
    }
    else
    {
      pIndex->gold = value;
      act( "�A�]�w$T������ $i ������C", ch, &value, mobname, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "hp" ) )
  {
    if ( value < 0 || value > MaxHit )
    {
      act( "�ͩR�O�d��� 0 �� $x�C", ch, &MaxHit, NULL, TO_CHAR );
    }

    else
    {
      pIndex->hit = value;
      act( "�A�]�w$T�ͩR�O�� $x�C", ch, &value, mobname, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "mana" ) )
  {
    if ( value < 0 || value > MaxMana )
    {
      act( "�k�O�d��� 0 �� $x�C", ch, &MaxMana, NULL, TO_CHAR );
    }

    else
    {
      pIndex->mana = value;
      act( "�A�]�w$T�k�O�� $x�C", ch, &value, mobname, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "move" ) )
  {
    if ( value < 0 || value > MaxMove )
    {
      act( "���ʤO�d��� 0 �� $x�C", ch, &MaxMove, NULL, TO_CHAR );
    }

    else
    {
      pIndex->move = value;
      act( "�A�]�w$T���ʤO�� $x�C", ch, &value, mobname, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "alignment" ) )
  {
    if ( value < -AlignRange || value > AlignRange )
    {
      chinese_number( -AlignRange, buf  );
      chinese_number(  AlignRange, buf1 );

      act( "�}��d��u��� $t �� $T�C", ch, buf, buf1, TO_CHAR );
    }

    else
    {
      pIndex->alignment = value;
      act( "�A�]�w$T�}�笰 $x�C", ch, &value, mobname, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "name" ) )
  {
    free_string( pIndex->name );
    pIndex->name = str_dup( arg3 );
    act( "�]�w$T�^��W�٬� $t�C", ch, arg3, mobname, TO_CHAR );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "short" ) )
  {
    free_string( pIndex->short_descr );
    pIndex->short_descr = str_dup( arg3 );
    act( "�]�w$T����W�٬� $t�C", ch, arg3, mobname, TO_CHAR );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "long" ) )
  {
    free_string( pIndex->long_descr );
    pIndex->long_descr = str_dup( arg3 );
    act( "�]�w$T���ԭz�� $t�C", ch, arg3, mobname, TO_CHAR );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "deadmsg" ) )
  {
    free_string( pIndex->deadmsg );
    pIndex->deadmsg = str_dup( arg3 );
    act( "�]�w$T���`�ԭz�� $t�C", ch, arg3, mobname, TO_CHAR );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "auction" ) )
  {
    free_string( pIndex->auction );
    pIndex->auction = str_dup( arg3 );
    act( "�]�w$T���ԭz�� $t�C", ch, arg3, mobname, TO_CHAR );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "description" ) )
  {
    free_string( pIndex->description );
    pIndex->description = str_dup( arg3 );
    act( "�]�w$T���ԭz�� $t�C", ch, arg3, mobname, TO_CHAR );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "dodge" ) )
  {
    if ( value < 0 )
    {
      send_to_char( "������ܰ{���v���t�ȡC\n\r" , ch );
    }
    else
    {
      pIndex->dodge_ratio = value;
      act( "�A�]�w$T�{���v���ʤ��� $i�C", ch, &value, mobname, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "attack" ) )
  {
    if ( value < 0 )
    {
      send_to_char( "������ܧ����v���t�ȡC\n\r" , ch );
    }
    else
    {
      pIndex->attack_ratio = value;
      act( "�A�]�w$T�����v���ʤ��� $i�C", ch, &value, mobname, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "migrate" ) )
  {
    if ( value < 0 || value > 100 )
    {
      send_to_char( "�E�����v�u��� 0 �� 100�C\n\r", ch );
    }
    else
    {
      pIndex->migrate = value;
      act( "�A�]�w$T�E���v���ʤ��� $i�C", ch, &value, mobname, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "tractable" ) )
  {
    if ( value < 0 || value > 100 )
    {
      send_to_char( "���A���v�u��� 0 �� 100�C\n\r", ch );
    }
    else
    {
      pIndex->tractable = value;
      act( "�A�]�w$T���A�v���ʤ��� $i�C", ch, &value, mobname, TO_CHAR );
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg2, "multipile" ) )
  {
    if ( value <= 0 || value > 1000 )
    {
      send_to_char( "�g��ȥ[���u��� 1 �� 1000�C\n\r", ch );
    }
    else
    {
      pIndex->multipile = value;
      act( "�A�]�w$T �g��ȥ[��������ʤ���$i�C", ch, &value, mobname, TO_CHAR );
    }

    RETURN_NULL();
  }

  /* �p�G������, �h�X�{���~�T�� */
  do_miset( ch, "" );
  RETURN_NULL();
}

FUNCTION( do_filename )
{
  char              arg[MAX_INPUT_LENGTH];
  OBJ_INDEX_DATA  * pObjIndex;
  ROOM_INDEX_DATA * pRoomIndex;
  MOB_INDEX_DATA  * pMobIndex;
  int               sn;
  int               len;

  PUSH_FUNCTION( "do_filename" );

  argument = one_argument( argument, arg );

  if ( !arg[0] || !argument[0] )
  {
    send_to_char( "�Ьd�� filename �����T�ϥΤ�k�T\n\r", ch );
    RETURN_NULL();
  }

  if ( !is_number( argument ) || ( sn = atoi( argument ) ) <= 0 )
  {
    send_to_char( "�藍�_�M�ĤG�ӰѼƥ����O�Ʀr�T\n\r", ch );
    RETURN_NULL();
  }

  len = str_len( home_dir );

  if ( !str_cmp( arg, "obj" ) )
  {
    if ( !( pObjIndex = get_obj_index( sn ) ) )
    {
      act( "�藍�_�M�䤣�쪫�~�s�� $x ���쫬�T", ch, &sn, NULL, TO_CHAR );
      RETURN_NULL();
    }

    act( "�s�� $x ���~���ɮ׸��|�� $2$T$0"
      , ch, &sn, pObjIndex->filename + len, TO_CHAR );
  }

  else if ( !str_cmp( arg, "roo" ) )
  {
    if ( !( pRoomIndex = get_room_index( sn ) ) )
    {
      act( "�藍�_�M�䤣��ж��s�� $x ���쫬�T", ch, &sn, NULL, TO_CHAR );
      RETURN_NULL();
    }

    act( "�s�� $x �ж����ɮ׸��|�� $2$T$0"
      , ch, &sn, pRoomIndex->filename + len, TO_CHAR );
  }

  else if ( !str_cmp( arg, "mob" ) )
  {
    if ( !( pMobIndex = get_mob_index( sn ) ) )
    {
      act( "�藍�_�M�䤣��Ǫ��s�� $x ���쫬�T", ch, &sn, NULL, TO_CHAR );
      RETURN_NULL();
    }

    act( "�s�� $x ���Ǫ��ɮ׸��|�� $2$T$0"
      , ch, &sn, pMobIndex->filename + len, TO_CHAR );
  }

  else
  {
    send_to_char( "�藍�_�M�u��d�� roo mob obj �T�خ榡�T\n\r", ch );
  }

  RETURN_NULL();
}

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

FUNCTION( do_angel )
{
  ANGEL_DATA * pAngel;
  ANGEL_DATA * zAngel;
  char arg[MAX_INPUT_LENGTH];
  int  count;

  PUSH_FUNCTION( "do_angel" );

  if ( IS_NPC( ch ) || !ch->pcdata ) RETURN_NULL();

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    if ( AngelComing == FALSE && !IS_IMMORTAL( ch ) )
    {
      send_to_char( "�藍�_�M�u�@�����b���M�y��A�ХL�X�Ӧn�F�T\n\r", ch );
      RETURN_NULL();
    }

    if ( !IS_SET( ch->act, PLR_ANGEL ) )
    {
      send_to_char( "�A�S����u�@���ﶵ���}�M�п�J config +angel �ӱҰʡT\n\r", ch );
      RETURN_NULL();
    }

    act( "�A�]�w�u�@���g����$i��M�A$I���u�@���|�A�Ӥ@���T"
      , ch, &ch->pcdata->angel_set, &ch->pcdata->angel_left, TO_CHAR );

    AngelComing = FALSE;
    count       = 0;
    zAngel      = NULL;

    for ( pAngel = angel_list; pAngel; pAngel = pAngel->next )
    {
      if ( ( pAngel->description && pAngel->description[0] )
        || ch->level > pAngel->higher
        || ch->level < pAngel->lower
        || ch->position != pAngel->position ) continue;

      if ( ( ( *pAngel->function ) ( ch, FALSE ) ) == TRUE
        && number_range( 0, count++ ) == 0 )
        zAngel = pAngel;
    }

    if ( zAngel )
    {
      ch->pcdata->angel_left = ch->pcdata->angel_set;

      print_to_char( ch, "\e[1;32m�A������S�ǨӦu�@�����m�{�R"
          "\e[0m%s\e[0m��{��������M�~��[�o�M�ڵ��A����ĳ�O\n\r%s"
          , ch->cname, VERTICAL_LINE );

      ( *zAngel->function ) ( ch, TRUE );

      print_to_char( ch, "%s", VERTICAL_LINE );
    }
    else
    {
      send_to_char( "���M�A�ثe�����p�ڤ]�����D�n����M�бЪ��a�a�T\n\r", ch );
    }
    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "!set" ) )
  {
    one_argument( argument, arg );
    if ( arg[0] == '\x0' || !is_number( arg ) )
    {
      send_to_char( "�A������J�Ʀr�ӥN��u�@���X�{���g���T\n\r", ch );
      RETURN_NULL();
    }

    if ( ( count = atoi( arg ) ) < 10 )
    {
      send_to_char( "�o�ضg���u�@���O�|�֦����T\n\r", ch );
      RETURN_NULL();
    }

    if ( count > 300 )
    {
      send_to_char( "�`�������u�@�����Ӥ[�a�T\n\r", ch );
      RETURN_NULL();
    }

    ch->pcdata->angel_left = ch->pcdata->angel_set = count;
    act( "�A�]�w�C$i��u�@���|�X�{�@���T", ch, &count, NULL, TO_CHAR );

    if ( !IS_SET( ch->act, PLR_ANGEL ) )
    {
      SET_BIT( ch->act, PLR_ANGEL );
      send_to_char( "��F�M���K���A��u�@���ﶵ���}�F�T\n\r", ch );
    }
  }

  else
  {
    send_to_char( "�A���y�k����M�Ьd�� angel�M���¡T\n\r", ch );
  }

  RETURN_NULL();
}

void angel_update( void )
{
  DESCRIPTOR_DATA * man;
  ANGEL_DATA      * pAngel;
  ANGEL_DATA      * zAngel;
  CHAR_DATA       * ch;
  int               count;

  PUSH_FUNCTION( "angel_update" );

  for ( man = descriptor_list; man; man = man->next )
  {
    if ( verify_desc( man )
      && man->connected == CON_PLAYING
      && verify_char( ( ch = man->character ) )
      && ch->pcdata
      && IS_SET( ch->act, PLR_ANGEL ) )
    {
      /* �]�w�ɶ� */
      if ( --ch->pcdata->angel_left > 0 ) continue;
      ch->pcdata->angel_left = ch->pcdata->angel_set;

      zAngel = NULL;
      count  = 0;

      for ( pAngel = angel_list; pAngel; pAngel = pAngel->next )
      {
        if ( pAngel->function
          || ch->level > pAngel->higher
          || ch->level < pAngel->lower
          || ch->position != pAngel->position ) continue;

        if ( number_range( 0, count++ ) == 0 ) zAngel = pAngel;
      }

      if ( zAngel )
      {
        print_to_char( ch, "\e[1;32m�A������S�ǨӦu�@�����m�{�R"
          "\e[0m%s\e[0m��{��������M�~��[�o�M�ڵ��A����ĳ�O\n\r%s"
          "\e[0m%s\e[0m%s"
          , ch->cname, VERTICAL_LINE, zAngel->description, VERTICAL_LINE );
      }
    }
  }
  RETURN_NULL();
}

bool angel_dead( CHAR_DATA * ch, bool fPrint )
{
  PUSH_FUNCTION( "angel_dead" );
  RETURN( FALSE );
}

ANGEL( angel_mount )
{
  PUSH_FUNCTION( "angel_mount" );

  if ( !ch->mount && !ch->mount_by )
  {
    if ( fPrint ) send_to_char(
       "�A�O���O�i�H�Ҽ{��@�ǰ����M�M(\e[1;32mmount\[0m)�S"
       "�Aı�o�p��O�S\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_unmount )
{
  PUSH_FUNCTION( "angel_unmount" );

  if ( ch->mount )
  {
    if ( fPrint ) send_to_char(
      "�A���M�ۤ@�ǰ��M�����M�U���п�J \e[1;32munmount\e[0m �C\n\r", ch );

    RETURN( TRUE );
  }
  RETURN( FALSE );
}

ANGEL( angel_sleep )
{
  PUSH_FUNCTION( "angel_sleep" );

  if ( ch->hit  < get_curr_hit( ch )  / 3
    || ch->mana < get_curr_mana( ch ) / 3
    || ch->move < get_curr_move( ch ) / 3 )
  {
    if ( fPrint ) send_to_char(
      "�ݧA���˫��Y���M���ۺ�ı(\e[1;32msleep\e[0m)�άO"
      "��(\e[1;32mrest\e[0m)�n�ܡS\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_drink )
{
  PUSH_FUNCTION( "angel_drink" );

  if ( ch->pcdata && ch->pcdata->condition[COND_THIRST] <= 0 )
  {
    if ( fPrint ) send_to_char(
      "�ݧA�n���ܤf�����ˤl�M�h�����(\e[1;32mwell\e[0m)�άO�R"
      "���U�ӳ�(\e[1;32mdrink\e[0m)�C\n\r", ch );
    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_eat )
{
  PUSH_FUNCTION( "angel_eat" );

  if ( ch->pcdata && ch->pcdata->condition[COND_FULL] <= 0 )
  {
    if ( fPrint ) send_to_char(
      "�ݧA�n���ܾj���ˤl�M�h�R�@�ǭ����ӦY(\e[1;32meat\e[0m)�a�C\n\r", ch );
    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_marry )
{
  PUSH_FUNCTION( "angel_marry" );

  if ( get_age( ch ) > 20 && ch->pcdata && ch->pcdata->wooer )
  {
    if ( fPrint ) send_to_char(
      "���F�l�D�̡M�A�i�H�нu�W���j�����A���ұB�a�T\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_wedding )
{
  PUSH_FUNCTION( "angel_wedding" );

  if ( get_age( ch ) > 20 && ch->pcdata && !ch->pcdata->wooer )
  {
    if ( fPrint ) send_to_char(
      "�A�~���]���p�F�M�O���O�i�H�Ҽ{���B(\e[1;32mmarry\e[0m)�F�O�S\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_alias )
{
  PUSH_FUNCTION( "angel_alias" );

  if ( get_alias_count( ch ) <= 2 )
  {
    if ( fPrint ) send_to_char(
      "�ݨӧA���ӷ|�ϥΥ������O(\e[1;32malias\e[0m)�M�����ոլݡC\n\r", ch );

    RETURN( TRUE );
  }
  RETURN( FALSE );
}

ANGEL( angel_auction )
{
  PUSH_FUNCTION( "angel_auction" );

  if ( ch->carry_number >= 20 )
  {
    if ( fPrint ) send_to_char(
      "�z�M���W����h�F��M���X��X�ө��(\e[1;32mauction\e[0m)"
      "�ݬݹ��T\n\r", ch );

    RETURN( TRUE );
  }
  RETURN( FALSE );
}

ANGEL( angel_bet )
{
  PUSH_FUNCTION( "angel_bet" );

  if ( auction_info->obj
    && ch->gold > auction_info->price
    && auction_info->obj->level <= ch->level )
  {
    if ( fPrint ) send_to_char(
      "�A�i�H�Ҽ{�R�U(\e[1;32mbet\e[0m)�{�b��檺�F��M"
      "�άO�ݬݤ]�n(\e[1;32mbet help\e[0m)�T\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_areas )
{
  PUSH_FUNCTION( "angel_areas" );

  if ( ch->played <= 60 * 30 )
  {
    if ( fPrint ) send_to_char(
      "�A�n�M�s��M�i�H�d�߰ϰ���(\e[1;32mareas\e[0m)�M"
      "���᧤����(\e[1;32mbus\e[0m)�ӳ}�}�a�T\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_xname )
{
  PUSH_FUNCTION( "angel_xname" );

  if ( ch->pcdata && ch->pcdata->xname >= 5 )
  {
    if ( fPrint ) send_to_char(
      "�ݨӧA�`�`�ϥΤ����r��M�����A�M�o�i�O���n���ߺD��T\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_immortal )
{
  PUSH_FUNCTION( "angel_immortal" );

  if ( IS_IMMORTAL( ch ) )
  {
    if ( fPrint ) send_to_char( "��F�j���F�M�٦b�V�M�h�g�ϰ�աT\n\r", ch );
    RETURN( TRUE );
  }
  RETURN( FALSE );
}

ANGEL( angel_deposit_gold )
{
  PUSH_FUNCTION( "angel_deposit_gold" );

  if ( ch->gold > 50000 )
  {
    if ( fPrint ) send_to_char(
      "���ҿװ]���S�աM����h����s(\e[1;32mdeposit\e[0m)�_�ӧa�T\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_deposit_obj )
{
  PUSH_FUNCTION( "angel_deposit_obj" );

  if ( ch->carry_number >= can_carry_n( ch ) - 2 )
  {
    if ( fPrint ) send_to_char(
      "�z�M�A���W���˳ƥi���ְڡM����h�s(\e[1;32mdeposit\e[0m)"
      "�_�ӧa�T\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_stock )
{
  PUSH_FUNCTION( "angel_stock" );

  if ( ch->bank > 1000000 )
  {
    if ( fPrint ) send_to_char(
      "�ݨӧA���Ȧ�s�ڤ��ֳ�M�Ҽ{���Ѳ�(\e[1;32mstock\e[0m)��ˡS"
      "\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_gamble )
{
  PUSH_FUNCTION( "angel_gamble" );

  if ( ch->gold > 1000000 )
  {
    if ( fPrint ) send_to_char(
      "�ݧA���J���J���ˤl�M�ӽ��(\e[1;32mgamble\e[0m)�ոդ����T\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_direction )
{
  EXIT_DATA * pexit;
  int         door;
  int         count;
  int         rem;

  PUSH_FUNCTION( "angel_direction" );

  if ( !ch->in_room ) RETURN( FALSE );

  for ( rem = ERRORCODE, count = door = 0; door < DIR_MAX; door++ )
  {
    if ( ( pexit = ch->in_room->exit[door] )
      && pexit->to_room
      && pexit->closed
      && number_range( 0, count++ ) == 0 )
      rem = door;
  }

  if ( rem != ERRORCODE )
  {
    if ( fPrint ) act( "�A��$t��n�����X�f�M�����h�_�I�ݬݡT"
      , ch, direction_name( rem ), NULL, TO_CHAR );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_shop )
{
  CHAR_DATA * keeper;

  PUSH_FUNCTION( "angel_shop" );

  if ( !ch->in_room ) RETURN( FALSE );

  for ( keeper = ch->in_room->people; keeper; keeper = keeper->next_in_room )
  {
    if ( IS_NPC( keeper )
      && keeper->pIndexData
      && keeper->pIndexData->pShop
      && keeper->pIndexData->pShop->type == SHOP_STORE )
    {
      if ( fPrint ) send_to_char(
        "�A�o�̦n�����ӤH�C�M�����R(\e[1;32mbuy\e[0m)�ӪF��"
        "�M�ݬ�(\e[1;32mlist\e[0m)�]�n�T\n\r", ch );

      RETURN( TRUE );
    }
  }
  RETURN( FALSE );
}

ANGEL( angel_smith )
{
  CHAR_DATA * keeper;

  PUSH_FUNCTION( "angel_smith" );

  if ( !ch->in_room ) RETURN( FALSE );

  for ( keeper = ch->in_room->people; keeper; keeper = keeper->next_in_room )
  {
    if ( IS_NPC( keeper )
      && keeper->pIndexData
      && keeper->pIndexData->pShop
      && keeper->pIndexData->pShop->type == SHOP_SMITH )
    {
      if ( fPrint ) send_to_char(
        "�A�o�̦n�����K�K�C�M�����ײz(\e[1;32mrepair\e[0m)"
        "�ӪF��ݬݡT\n\r", ch );

      RETURN( TRUE );
    }
  }
  RETURN( FALSE );
}

ANGEL( angel_mercenary )
{
  CHAR_DATA * keeper;

  PUSH_FUNCTION( "angel_mercenary" );

  if ( !ch->in_room ) RETURN( FALSE );

  for ( keeper = ch->in_room->people; keeper; keeper = keeper->next_in_room )
  {
    if ( IS_NPC( keeper )
      && keeper->pIndexData
      && keeper->pIndexData->pShop
      && keeper->pIndexData->pShop->type == SHOP_MERCENARY )
    {
      if ( fPrint ) send_to_char(
        "�o�̦��ӶħL�ҡM��������(\e[1;32mhire\e[0m)�ħL�������T\n\r", ch );

      RETURN( TRUE );
    }
  }
  RETURN( FALSE );
}

ANGEL( angel_coper )
{
  CHAR_DATA * keeper;

  PUSH_FUNCTION( "angel_coper" );

  if ( !ch->in_room ) RETURN( FALSE );

  for ( keeper = ch->in_room->people; keeper; keeper = keeper->next_in_room )
  {
    if ( IS_NPC( keeper )
      && keeper->pIndexData
      && keeper->pIndexData->pShop
      && keeper->pIndexData->pShop->type == SHOP_COPER )
    {
      if ( fPrint ) send_to_char(
        "�o�̦����c�M�A�i�H��L�R(\e[1;32mtrade\e[0m)�ǰ��M�M�ݡT\n\r", ch );

      RETURN( TRUE );
    }
  }
  RETURN( FALSE );
}

ANGEL( angel_kill )
{
  CHAR_DATA * victim;

  PUSH_FUNCTION( "angel_kill" );

  if ( !ch->in_room ) RETURN( FALSE );

  for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
  {
    if ( IS_NPC( victim )
      && abs( victim->level - ch->level ) <= 3 )
    {
      if ( fPrint ) act( "�A�o�̦���$N�M�_��($2kill$0)���ӥi�H�����ָg��ȳ�T"
        , ch, NULL, victim, TO_CHAR );

      RETURN( TRUE );
    }
  }

  RETURN( FALSE );
}

ANGEL( angel_bank )
{
  PUSH_FUNCTION( "angel_bank" );

  if ( ch->in_room && ch->in_room->DepositMoney )
  {
    if ( fPrint ) send_to_char(
      "�o�̭�n���ӿ����M�����s��(\e[1;32mdeposit\e[0m)"
      "�άO�d��(\e[1;32mcheck\e[0m)�s�ڡT\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_obj_bank )
{
  PUSH_FUNCTION( "angel_obj_bank" );

  if ( ch->in_room && ch->in_room->StoreRoom )
  {
    if ( fPrint ) send_to_char(
      "�o�̭�n�����𧽡M�����s(\e[1;32mdeposit\e[0m)���~�M"
      "�άO�d��(\e[1;32mcheck\e[0m)�ݬݡC\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_get )
{
  OBJ_DATA * obj;

  PUSH_FUNCTION( "angel_get" );

  if ( !ch->in_room ) RETURN( FALSE );

  for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
  {
    if ( can_see_obj( ch, obj )
      && obj->Takeable
      && obj->item_type != ITEM_CORPSE_PC
      && obj->item_type != ITEM_CORPSE_NPC
      && obj->item_type != ITEM_TRASH )
    {
      if ( fPrint ) send_to_char(
        "�a�W�n�����F��M�A�i�H�յۧ⥦����(\e[1;32mget\e[0m)�_�ӡT\n\r", ch );

      RETURN( TRUE );
    }
  }

  RETURN( FALSE );
}

ANGEL( angel_drop )
{
  OBJ_DATA * obj;

  PUSH_FUNCTION( "angel_drop" );

  if ( !ch->in_room ) RETURN( FALSE );

  for ( obj = ch->carrying; obj; obj = obj->next_content )
  {
    if ( can_see_obj( ch, obj ) && obj->item_type == ITEM_TRASH )
    {
      if ( fPrint ) send_to_char(
        "�A���W�n�����ǩU���M�̦n�⥦���ᱼ(\e[1;32mdrop\e[0m)�C\n\r", ch );

      RETURN( TRUE );
    }
  }

  RETURN( FALSE );
}

ANGEL( angel_withdraw )
{
  PUSH_FUNCTION( "angel_withdraw" );

  if ( ch->deposit )
  {
    if ( fPrint ) send_to_char(
      "�A�b�𧽸̦��s�ǪF��M�O�ѤF�n�⥦����(\e[1;32mwithdraw\e[0m)�X�ӳ�C\n\r", ch );

    RETURN( TRUE );
  }
  RETURN( FALSE );
}

ANGEL( angel_call )
{
  OBJ_DATA * obj;

  PUSH_FUNCTION( "angel_call" );

  if ( !ch->in_room ) RETURN( FALSE );

  for ( obj = ch->carrying; obj; obj = obj->next_content )
  {
    if ( can_see_obj( ch, obj ) && obj->item_type == ITEM_SPIRITJADE )
    {
      if ( fPrint ) send_to_char(
        "�A���W�������ɭC�M�����l��(\e[1;32mcall\e[0m)�X�ӬݬݡC\n\r", ch );

      RETURN( TRUE );
    }
  }

  RETURN( FALSE );
}

ANGEL( angel_bus )
{
  PUSH_FUNCTION( "angel_bus" );

  if ( ch->in_room && is_platform( ch->in_room ) )
  {
    if ( fPrint ) send_to_char(
      "�A�ثe���b�a�K���̡M�i�H�d��(\e[1;32mbus\e[0m)�ݬݧA�����ӤF�ܡS\n\r", ch );

    RETURN( TRUE );
  }
  RETURN( FALSE );
}

ANGEL( angel_ship )
{
  SHIP_DATA * pShip;

  PUSH_FUNCTION( "angel_shop" );

  if ( !ch->in_room ) RETURN( FALSE );

  for ( pShip = ship_list; pShip; pShip = pShip->next )
  {
    if ( pShip->starting == ch->in_room )
    {
      if ( fPrint ) send_to_char(
        "�A�ثe���b�f��_�l�I�W�M�i�H�d��(\e[1;32mship\e[0m)�ݬݧA"
        "����ӤF�S�S\n\r", ch );

      RETURN( TRUE );
    }
  }

  RETURN( FALSE );
}

ANGEL( angel_lore )
{
  CHAR_DATA * victim;

  PUSH_FUNCTION( "angel_lore" );

  if ( !ch->in_room ) RETURN( FALSE );

  for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
  {
    if ( IS_NPC( victim ) )
    {
      if ( fPrint ) send_to_char(
        "�o�̦��Ǫ��i�H���M���O���F�p�ߡM�٬O�����q"
        "(\e[1;32mlore\e[0m)�ݬݧa�T\n\r", ch );
      RETURN( TRUE );
    }
  }

  RETURN( FALSE );
}

ANGEL( angel_badman )
{
  CHAR_DATA * victim;

  PUSH_FUNCTION( "angel_badman" );

  if ( !ch->in_room ) RETURN( FALSE );

  for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
  {
    if ( IS_NPC( victim ) && victim->level - ch->level >= 10 )
    {
      if ( fPrint ) send_to_char(
        "�o�̦����ܱj���Ǫ��M�٬O���L���@�I�a�T\n\r", ch );

      RETURN( TRUE );
    }
  }


  RETURN( FALSE );
}

ANGEL( angel_wake )
{
  PUSH_FUNCTION( "angel_wake" );

  if ( fPrint ) send_to_char(
    "�A���b�𮧩άO��ı�M�����M�O�o�n�_��"
    "(\e[1;32mwake\e[0m �� \e[1;32mstand\e[0m)��T\n\r", ch );

  RETURN( TRUE );
}

ANGEL( angel_lotto )
{
  PUSH_FUNCTION( "angel_lotto" );

  if ( !IS_SET( ch->turn, PLR_LOTTO ) )
  {
    if ( fPrint ) send_to_char(
      "�ݧA�٨S���q(\e[1;32mturn\e[0m)�ֳz�ֳ���M�q�q�ݡM"
      "���F�i�O���K�����P��C\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_hero )
{
  PUSH_FUNCTION( "angel_hero_1" );

  if ( ch->level >= ( LEVEL_IMMORTAL - 3 ) )
  {
    if ( fPrint ) send_to_char(
      "�V�O�@�I��M�N�֤ɨ�^��(\e[1;32mhero\e[0m)�F�M�[�o�[�o�T\n\r", ch );
    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_autosac )
{
  PUSH_FUNCTION( "angel_autosac" );

  if ( !IS_SET( ch->act, PLR_AUTOSAC ) )
  {
    if ( fPrint ) send_to_char(
      "���F�קK�;�M���M�Х��} \e[1;32mautosac\e[0m �a�T\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_exact )
{
  PUSH_FUNCTION( "angel_exact" );

  if ( !IS_SET( ch->act, PLR_EXACT ) )
  {
    if ( fPrint ) send_to_char(
      "�A�`�`ı�o�u�ݨ줤��W�٦ӨS���^��W��"
      "�ӳ·жܡS�ո� \e[1;32mconfig +exact\e[0m\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_message )
{
  PUSH_FUNCTION( "angel_message" );

  if ( IS_SET( ch->act, PLR_MESSAGE ) )
  {
    if ( fPrint ) send_to_char(
      "�p�G�Aı�o�����ܺC�M�A�i�H�����԰�"
      "�ԭz(\e[1;32mconfig -message\e[0m)�C\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_config_flee )
{
  PUSH_FUNCTION( "angel_config_flee" );

  if ( !IS_SET( ch->act, PLR_FLEE ) )
  {
    if ( fPrint ) send_to_char(
      "�p�G�A�`�`�԰��_�u�M�г]�w \e[1;32mconfig +flee\e[0m�C\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_autoloot )
{
  PUSH_FUNCTION( "angel_autoloot" );

  if ( !IS_SET( ch->act, PLR_AUTOLOOT ) )
  {
    if ( fPrint ) send_to_char(
      "���F�o���h�ԧQ�~�M�Ҽ{���} \e[1;32mautoloot\e[0m �a�T\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_friend )
{
  PUSH_FUNCTION( "angel_friend" );

  if ( friend_count( ch ) <= 0 )
  {
    if ( fPrint ) send_to_char(
      "�A�i�H�]�w�A���n�ͦW��(\e[1;32mfriend\e[0m)�M�ӳq��"
      "�A�L�O�_�W�u�C\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_wimpy )
{
  PUSH_FUNCTION( "angel_wimpy" );

  if ( ch->wimpy * 5 <= get_curr_hit( ch ) )
  {
    if ( fPrint ) send_to_char(
      "���F�A�����a�w���M�Ч�k�]�I��(\e[1;32mwimpy\e[0m)�հ�"
      "�@�I�a�T\n\r", ch );

    RETURN( TRUE );
  }
  RETURN( FALSE );
}

ANGEL( angel_enable )
{
  PUSH_FUNCTION( "angel_enabel" );

  if ( enable_count( ch ) <= 3 )
  {
    if ( fPrint ) send_to_char(
      "�A���P��(\e[1;32menable\e[0m)�Ӥ֤F��M�`�N�@�U��M"
      "�n���P��(\e[1;32menable\e[0m)�M�t�Τ~�|���A�X�۳�C\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_flee )
{
  ENABLE_DATA * pEnable;
  SKILL_DATA  * pSkill;

  PUSH_FUNCTION( "angel_flee" );

  for ( pEnable = ch->enable; pEnable; pEnable = pEnable->next )
  {
    if ( ( pSkill = pEnable->skill ) && pSkill->type == TAR_FLEE )
      RETURN( FALSE );
  }

  if ( fPrint ) send_to_char(
    "�A�S���P��(\e[1;32menable\e[0m)�k�]���ޯ��M�o�˫ܦM�I�C�T\n\r", ch );

  RETURN( TRUE );
}

ANGEL( angel_attack )
{
  ENABLE_DATA * pEnable;
  SKILL_DATA  * pSkill;

  PUSH_FUNCTION( "angel_attack" );

  for ( pEnable = ch->enable; pEnable; pEnable = pEnable->next )
  {
    if ( ( pSkill = pEnable->skill ) && pSkill->type == TAR_CHAR_OFFENSIVE )
      RETURN( FALSE );
  }

  if ( fPrint ) send_to_char(
    "�A�S���P��(\e[1;32menable\e[0m)���������ޯ��M"
    "�o�ˤ��O�ܦM�I�ܡS\n\r", ch );

  RETURN( TRUE );
}

ANGEL( angel_meditation )
{
  ENABLE_DATA * pEnable;
  SKILL_DATA  * pSkill;

  PUSH_FUNCTION( "angel_meditation" );

  for ( pEnable = ch->enable; pEnable; pEnable = pEnable->next )
  {
    if ( ( pSkill = pEnable->skill ) && pSkill->type == TAR_MEDITATION )
      RETURN( FALSE );
  }

  if ( fPrint ) send_to_char(
    "�A�S���P��(\e[1;32menable\e[0m)�߷Q�����ޯ��M"
    "���֥h��(\e[1;32mlearn\e[0m)�@�Өӧa�T\n\r", ch );

  RETURN( TRUE );
}

ANGEL( angel_description )
{
  PUSH_FUNCTION( "angel_description" );

  if ( !ch->description || !*ch->description )
  {
    if ( fPrint ) send_to_char(
      "�A�S���y�z(\e[1;32mdescription\e[0m)�M"
      "���֥h�s��@�ӨӬݬݧa�T\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_club )
{
  PUSH_FUNCTION( "angel_club" );

  if ( !ch->club )
  {
    if ( fPrint ) send_to_char(
      "�A�n���٨S���[�J����(\e[1;32mclub\e[0m)�M�Ҽ{�ѥ[�ݬݧa�T\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_group )
{
  PUSH_FUNCTION( "angel_group" );

  if ( ch->master || ch->leader )
  {
    if ( fPrint ) send_to_char(
      "�ε��N�O�O�q�M�A�i�H�� \e[1;32mgtell\e[0m �Ӧb���餺���ܡC\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_split )
{
  PUSH_FUNCTION( "angel_split" );

  if ( ch->master || ch->leader )
  {
    if ( fPrint ) send_to_char(
      "�J�M�ն�(\e[1;32mgroup\e[0m)�F�M�����Ǫ����Ȥl��"
      "�n�७��(\e[1;32msplit\e[0m)�C\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_nogroup )
{
  PUSH_FUNCTION( "angel_nogroup" );

  if ( !ch->master || !ch->leader )
  {
    if ( fPrint ) send_to_char(
      "���ҿ׹ε��N�O�O�q�M�Ҽ{�ն�(\e[1;32mgroup\e[0m)�a�T\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_reply )
{
  PUSH_FUNCTION( "angel_reply" );

  if ( ch->reply )
  {
    if ( fPrint ) send_to_char(
      "�A���D�^���O�H���ܥu���� \e[1;32mreply\e[0m�M"
      "�����A�� \e[1;32mtell\e[0m �F�C\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_fountain )
{
  OBJ_DATA * obj;

  PUSH_FUNCTION( "angel_fountain" );

  if ( !ch->in_room ) RETURN( FALSE );

  for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
  {
    if ( obj->item_type == ITEM_FOUNTAIN )
    {
      if ( fPrint ) send_to_char(
        "�o�̭�n���Ӥ����M�ܳ�(\e[1;32mdrink\e[0m)�ݧa�T\n\r", ch );

      RETURN( TRUE );
    }
  }

  RETURN( FALSE  );
}

ANGEL( angel_food )
{
  OBJ_DATA * obj;

  PUSH_FUNCTION( "angel_fountain" );

  if ( !ch->in_room ) RETURN( FALSE );

  for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
  {
    if ( obj->item_type == ITEM_FOOD )
    {
      if ( fPrint ) send_to_char(
        "�o�̭�n���ǭ����M���־�(\e[1;32mget\e[0m)�_�ӡM"
        "�Y�Y(\e[1;32meat\e[0m)�ݧa�T\n\r", ch );

      RETURN( TRUE );
    }
  }

  RETURN( FALSE  );
}

ANGEL( angel_gambleset )
{
  PUSH_FUNCTION( "angel_gambleset" );

  if ( IS_SET( ch->deaf , CHANNEL_GAMBLE ) )
  {
    if ( fPrint ) send_to_char(
      "�A�O���Oı�o���ɭԽ���W�D(\e[1;32mchannel\e[0m)�٦��I�n�M"
      "�A�i�H�� \e[1;32mchannel -gamble\e[0m ��������C\n\r", ch );

    RETURN( TRUE );
  }
  RETURN( FALSE );
}

ANGEL( angel_weather )
{
  PUSH_FUNCTION( "angel_weather" );

  if ( IS_SET( ch->deaf , CHANNEL_WEATHER ) )
  {
    if ( fPrint ) send_to_char(
      "�A�O���Oı�o�Ѯ��W�D��A�S���ΩO�S�A�i�H�� "
      "\e[1;32mchannel -weather\e[0m ��������C\n\r", ch );

    RETURN( TRUE );
  }
  RETURN( FALSE );
}

ANGEL( angel_repair )
{
  OBJ_DATA   * obj;
  SITUS_DATA * pSitus;

  PUSH_FUNCTION( "angel_repair" );

  for ( pSitus = situs_list; pSitus; pSitus = pSitus->next )
  {
    if ( ( obj = get_eq_char( ch, pSitus->location ) )
      && obj->max_armor > 100
      && obj->armor * 2 <= obj->max_armor )
    {
      if ( fPrint ) send_to_char(
        "�A���W���Ǹ˳Ƨ֭n�a�F�M������K�K�άOű�C�v"
        "�ײz(\e[1;32mrepair\e[0m)�a�T\n\r", ch );

      RETURN( TRUE );
    }
  }

  RETURN( FALSE );
}

ANGEL( angel_crave )
{
  PUSH_FUNCTION( "angel_crave" );

  if ( ch->pcdata && ch->pcdata->corpse && ch->pcdata->corpse->in_room )
  {
    if ( fPrint ) send_to_char(
      "�ٵ����򵥡S���֨��^(\e[1;32mcrave\e[0m)����a�T\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_search )
{
  PUSH_FUNCTION( "angel_search" );

  if ( ch->pcdata && ch->pcdata->corpse && ch->pcdata->corpse->in_room )
  {
    if ( fPrint ) send_to_char(
      "�ٵ����򵥡S���ַj�M(\e[1;32msearch\e[0m)�A������a�T\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_sac )
{
  PUSH_FUNCTION( "angel_sac" );

  if ( ch->pcdata && ch->pcdata->corpse && ch->pcdata->corpse->in_room )
  {
    if ( fPrint ) send_to_char(
      "���ֱq�A������̾ߦ^�Ҧ��F��(\e[1;32mget all corpse\e[0m)�M"
      "�N�i�H�ٶ�(\e[1;32msac corpse\e[0m)�C\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_wear )
{
  SITUS_DATA * pSitus;

  PUSH_FUNCTION( "angel_wear" );

  for ( pSitus = situs_list; pSitus; pSitus = pSitus->next )
    if ( get_eq_char( ch, pSitus->location ) ) RETURN( FALSE );

  if ( fPrint ) send_to_char(
    "�A������(\e[1;32mwear\e[0m)�˳ƪ���M�p�ߵ۲D��T\n\r", ch );

  RETURN( TRUE );
}

ANGEL( angel_study )
{
  OBJ_DATA * obj;

  PUSH_FUNCTION( "angel_study" );

  for ( obj = ch->carrying; obj; obj = obj->next_content )
  {
    if ( can_see_obj( ch, obj ) && obj->item_type == ITEM_MYSTERY )
    {
      if ( fPrint ) send_to_char(
        "��M�A���W�������D�M���֮���ŪŪ��(\e[1;32mstudy\e[0m)�T\n\r", ch );

      RETURN( TRUE );
    }
  }

  RETURN( FALSE );
}

ANGEL( angel_learn )
{
  PUSH_FUNCTION( "angel_learn" );

  if ( learn_count( ch ) <= 5 )
  {
    if ( fPrint ) send_to_char(
      "�A�O���O���ӥX�h�_�I�h�h�ǲ�(\e[1;32mlearn\e[0m)�@��"
      "�ޯ�(\e[1;32mskill\e[0m)�O�S\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_recall )
{
  PUSH_FUNCTION( "angel_recall" );

  if ( ch->in_room && ch->in_room->Memorize )
  {
    if ( fPrint ) send_to_char(
      "�ݨӳo�̥i�H�����ǰe�I�M�]�w(\e[1;32mrecall\e[0m)�L�a�C\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_recallset )
{
  int loop;

  PUSH_FUNCTION( "angel_recallset" );

  if ( !ch->pcdata ) RETURN( FALSE );

  for ( loop = 0; loop < MAX_RECALL; loop++ )
    if ( ch->pcdata->recall[loop] > 0 ) RETURN( FALSE );

  if ( fPrint ) send_to_char(
    "�ݨӧA�@�Ӷǰe�I���S���]�w�M�]�w(\e[1;32mrecall\e[0m)�@�U�a�T\n\r", ch );

  RETURN( TRUE );
}

ANGEL( angel_train )
{
  PUSH_FUNCTION( "angel_train" );

  if ( ch->practice >= get_str_train( ch )
    || ch->practice >= get_int_train( ch )
    || ch->practice >= get_wis_train( ch )
    || ch->practice >= get_dex_train( ch )
    || ch->practice >= get_con_train( ch ) )
  {
    if ( fPrint ) send_to_char(
      "���F�V�m�I�ơM���n���O�M����h�V�m(\e[1;32mtrain\e[0m)�a�T\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_forsake )
{
  PUSH_FUNCTION( "angel_forsake" );

  if ( ch->class && learn_count( ch ) >= ( get_learnable( ch ) - 2 ) )
  {
    if ( fPrint ) send_to_char(
      "�ޯ�m�F����h�M�O���O�i�H�ۼo�@��"
      "�S�Ϊ��Z�\\(\e[1;32mforsake\e[0m)�O�S\n\r", ch );

    RETURN( TRUE );
  }
  RETURN( FALSE );
}

ANGEL( angel_enemy )
{
  PUSH_FUNCTION( "angel_enemy" );

  if ( ch->enemy )
  {
    if ( fPrint ) send_to_char(
      "�A�i�O�Y�ӫ������ġM�_�I�ɽЧA�h�h�p�ߡC\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_blind )
{
  PUSH_FUNCTION( "angel_blind" );

  if ( is_affected( ch, SLOT_BLINDNESS ) )
  {
    if ( fPrint ) send_to_char(
      "�A�������w�g�M�F�M�������ͥh��v��Tť����������ͤ�����T\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_poison )
{
  PUSH_FUNCTION( "angel_poison" );

  if ( is_affected( ch, SLOT_POISON ) )
  {
    if ( fPrint ) send_to_char(
      "�A�w�g���r�F�M�������ͥh��v��Tť����������ͤ�����T\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_board )
{
  PUSH_FUNCTION( "angel_board" );

  if ( ch->in_room && ch->in_room->board )
  {
    if ( fPrint ) send_to_char(
      "��M�o�̦��ӯd�����M�ոլݯण��\\Ū(\e[1;32mread\e[0m)"
      "�̭����峹�C\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_money )
{
  PUSH_FUNCTION( "angel_money" );

  if ( ch->gold <= 100 )
  {
    if ( fPrint ) send_to_char(
      "�X���b�~���i�H���a�I���O�M���֥h��(\e[1;32mdeposit\e[1;32m)"
      "�άO�h�ȰաT\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_firman )
{
  PUSH_FUNCTION( "angel_firman" );

  if ( ch->firman <= 0 )
  {
    if ( fPrint ) send_to_char(
      "�p�߳�M�A���K�����P�i�O�Υ��F��M�A���|���ޯ઺��T\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_fighting_1 )
{
  PUSH_FUNCTION( "angel_fighting" );

  if ( ch->hit * 2 > get_curr_hit( ch ) )
  {
    if ( fPrint ) send_to_char( "�Ȥ���M�����L���_���N�O�F�C\n\r", ch );
    RETURN( TRUE );
  }
  RETURN( FALSE );
}

ANGEL( angel_fighting_2 )
{
  PUSH_FUNCTION( "angel_fighting" );

  if ( ch->hit * 2 <= get_curr_hit( ch ) )
  {
    if ( fPrint ) send_to_char(
      "���ֱ��I�F�M�s���]�S�ΡM����k�](\e[1;32mflee\e[0m)�a�C\n\r", ch );

    RETURN( TRUE );
  }
  RETURN( FALSE );
}

ANGEL( angel_cname )
{
  PUSH_FUNCTION( "angel_cname" );

  if ( ch->pcdata && ch->pcdata->rechristen <= 2 )
  {
    if ( fPrint ) send_to_char(
      "�o�̧�W�r�����ƥi�O������M�O�Υ��F��T\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_nothing )
{
  PUSH_FUNCTION( "angel_nothing" );

  if ( ch->level == LEVEL_HERO )
  {
    if ( fPrint ) send_to_char(
      "���H�A�ɨ�^���M�A�i�H�Ҽ{�h����(\e[1;32mtask\e[0m)�o�T\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_reboot )
{
  PUSH_FUNCTION( "angel_reboot" );

  if ( wizlock )
  {
    if ( fPrint ) send_to_char(
      "�t�Χ֭n���s�}���F�M�л��֦��B���B���u(\e[1;32mquit\e[0m)�M"
      "�ݷ|�A�ӧa�T\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_mobile )
{
  CHAR_DATA * victim;
  CHAR_DATA * mob;
  int         count;

  PUSH_FUNCTION( "angel_mobile" );

  count = 0;
  mob   = NULL;

  for ( victim = char_list; victim; victim = victim->next )
  {
    if ( verify_char( victim )
      && IS_NPC( victim )
      && victim->in_room
      && !IS_SET( victim->act, ACT_NOKILL )
      && abs( victim->level - ch->level ) <= 3
      && number_range( 0, count++ ) == 0 )
    {
      mob = victim;
      if ( count >= 10 ) break;
    }
  }

  if ( mob )
  {
    if ( fPrint )
      act( "���$t��$N�ܾA�X�A�ɯŪ��Ǫ��M���֥h��L�a�T"
        , ch, mob->in_room->area->name, mob, TO_CHAR );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_note )
{
  int         notes;
  NOTE_DATA * pnote;

  PUSH_FUNCTION( "angel_note" );

  for ( notes = 0, pnote = note_list; pnote; pnote = pnote->next )
  {
    if ( is_note_to( ch, pnote ) && str_cmp( ch->name, pnote->sender )
      && pnote->date_stamp > ch->last_note )
    {
      if ( fPrint ) send_to_char(
        "�A���s���H��i�ӡM���֥� \e[1;32mnotes\e[0m "
        "�Ӿ\\Ū�o�ʫH�a�T\n\r", ch );

      RETURN( TRUE );
    }
  }

  RETURN( FALSE );
}

ANGEL( angel_leftskill )
{
  PUSH_FUNCTION( "agel_leftskill" );

  if ( !ch->class ) RETURN( FALSE );

  if ( learn_count( ch ) * 2 < get_learnable( ch ) )
  {
    if ( fPrint ) send_to_char(
      "�ݨӧA�|���ޯण�h��M�Υ� \e[1;32mleftskill\e[0m �Ӭݬ�"
      "�A�٥i�H�ǭ��ǧޯ�T\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_north )
{
  PUSH_FUNCTION( "angel_north" );

  if ( ch->in_room && ch->in_room->exit[DIR_NORTH] )
  {
    if ( fPrint ) send_to_char(
      "�A���_�観�X���M�A�i�H��J \e[1;32mnorth\e[0m ���_���T\n\r" , ch );
    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_south )
{
  PUSH_FUNCTION( "angel_south" );

  if ( ch->in_room && ch->in_room->exit[DIR_SOUTH] )
  {
    if ( fPrint ) send_to_char(
      "�A���n�観�X���M�A�i�H��J \e[1;32msouth\e[0m ���n���T\n\r" , ch );
    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_east )
{
  PUSH_FUNCTION( "angel_east" );

  if ( ch->in_room && ch->in_room->exit[DIR_EAST] )
  {
    if ( fPrint ) send_to_char(
      "�A���F�観�X���M�A�i�H��J \e[1;32meast\e[0m ���F���T\n\r" , ch );
    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_west )
{
  PUSH_FUNCTION( "angel_west" );

  if ( ch->in_room && ch->in_room->exit[DIR_WEST] )
  {
    if ( fPrint ) send_to_char(
      "�A����観�X���M�A�i�H��J \e[1;32mwest\e[0m ���訫�T\n\r" , ch );
    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_up )
{
  PUSH_FUNCTION( "angel_up" );

  if ( ch->in_room && ch->in_room->exit[DIR_UP] )
  {
    if ( fPrint ) send_to_char(
      "�A���W�観�X���M�A�i�H��J \e[1;32mup\e[0m ���W���T\n\r" , ch );
    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_down )
{
  PUSH_FUNCTION( "angel_down" );

  if ( ch->in_room && ch->in_room->exit[DIR_DOWN] )
  {
    if ( fPrint ) send_to_char(
      "�A���U�観�X���M�A�i�H��J \e[1;32mdown\e[0m ���U���T\n\r" , ch );
    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_enter )
{
  PUSH_FUNCTION( "angel_enter" );

  if ( ch->in_room && ch->in_room->exit[DIR_ENTER] )
  {
    if ( fPrint ) send_to_char(
      "�A�i�H��J \e[1;32menter\e[0m �Ӷi�J�Y���ж��T\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_out )
{
  PUSH_FUNCTION( "angel_out" );

  if ( ch->in_room && ch->in_room->exit[DIR_OUT] )
  {
    if ( fPrint ) send_to_char(
      "�A�i�H��J \e[1;32mout\e[0m �Ӷi�J�Y���ж��T\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_spy )
{
  int dir;

  PUSH_FUNCTION( "angel_spy" );

  if ( !ch->in_room ) RETURN( FALSE );

  for ( dir = 0; dir < DIR_MAX; dir++ )
  {
    if ( ch->in_room->exit[dir] )
    {
      if ( fPrint ) print_to_char( ch,
        "�A��%s�観�X�f�M�A�i�H�h�[��(\e[1;32mspy %s\e[0m)�ݬݳ�T"
        , direction_name( dir ), direction_ename( dir ) );
      RETURN( TRUE );
    }
  }
  RETURN( FALSE );
}

ANGEL( angel_exits )
{
  int dir;

  PUSH_FUNCTION( "angel_exits" );

  if ( !ch->in_room ) RETURN( FALSE );

  for ( dir = 0; dir < DIR_MAX; dir++ )
  {
    if ( ch->in_room->exit[dir] )
    {
      if ( fPrint ) print_to_char( ch,
        "�A�i�H��J \e[1;32mexits\e[0m ���[��A�|�P���X�f�T\n\r" );

      RETURN( TRUE );
    }
  }
  RETURN( FALSE );
}

ANGEL( angel_cast )
{
  int          loop;
  SKILL_DATA * pSkill;

  PUSH_FUNCTION( "angel_cast" );

  for ( loop = 0; loop < MAX_SKILL; loop++ )
  {
    if ( ch->skill[loop] > 0
      && ( pSkill = get_skill( loop ) )
      && pSkill->cast )
    {
      if ( fPrint ) print_to_char( ch,
        "�A�Ƿ|�F%s(%s)�o���ޯ�M�A�i�H�ϥ� \e[1;32mcast '%s'\e[0m "
        "�ӬI�i�X�ӡT\n\r"
        , pSkill->cname, pSkill->name, pSkill->name );

      RETURN( TRUE );
    }
  }

  RETURN( FALSE );

}

ANGEL( angel_ply )
{
  int          loop;
  SKILL_DATA * pSkill;

  PUSH_FUNCTION( "angel_ply" );

  for ( loop = 0; loop < MAX_SKILL; loop++ )
  {
    if ( ch->skill[loop] > 0
      && ( pSkill = get_skill( loop ) )
      && pSkill->ply
      && ( ch->skill[loop] < ( ch->level / UMAX( 1, pSkill->qutoient ) ) ) )
    {
      if ( fPrint ) print_to_char( ch,
        "�A�Ƿ|�F%s(%s)�o���ޯ�M�A�i�H�ϥ� \e[1;32mply '%s'\e[0m "
        "�ӽm�߬ݬݡT\n\r"
        , pSkill->cname, pSkill->name, pSkill->name );

      RETURN( TRUE );
    }
  }

  RETURN( FALSE );

}

ANGEL( angel_inventory )
{
  PUSH_FUNCTION( "angel_inventory" );

  if ( ch->carrying )
  {
    if ( fPrint ) send_to_char(
      "�A���W���Ǹ˳ơM�A�i�H��J \e[1;32minventory\e[0m �ӹ�ݡT\n\r", ch );

    RETURN( TRUE );
  }
  RETURN( FALSE );
}

ANGEL( angel_fight_join )
{
  PUSH_FUNCTION( "angel_fight_join" );

  if ( ch->level > PK_LEVEL
    && get_age( ch ) > pk_age
    && !fight_info->lock
    && fight_info->status == PK_JOIN
    && !ch->leader
    && !ch->master
    && auction_info->seller != ch
    && auction_info->buyer != ch
    && ch->position == POS_STANDING
    && ch->failed <= 0
    && ch->jail <= 0
    && !IS_SET( ch->act, PLR_KILLER )
    && !IS_SET( ch->act, PLR_BOLTER )
    && !IS_SET( ch->act, PLR_THIEF )
    && !ch->mount
    && !ch->spirit
    && ch->in_room
    && !ch->in_room->NoQuit
    && !is_station( ch->in_room )
    && !ch->in_room->Sail
    && !is_pk( ch ) )
  {
    if ( fPrint ) send_to_char(
      "�ثe����PK�j�ɡM�A�i�H��J \e[1;32mfight join\e[0m �Ӱѥ[�T\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

ANGEL( angel_fight_bet )
{
  PUSH_FUNCTION( "angel_fight_bet" );

  if ( fight_info->status == PK_BET
    && !fight_info->lock
    && ch->level >= level_limit
    && ch->donate <= 0 )
  {
    if ( fPrint ) send_to_char(
      "�ثe����PK�j�ɡM�A�i�H��J \e[1;32mfight bet <���a���X> <����"
      "\e[0m �ӤU�`�T\n\r", ch );

    RETURN( TRUE );
  }

  RETURN( FALSE );
}

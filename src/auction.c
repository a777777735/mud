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
#include <string.h>
#include <ctype.h>
#include "merc.h"

void            system_auction       args( ( void ) );
extern void     set_sale_default     args( ( SALE_DATA     * ) );

void init_auction( void )
{
  auction_info->price      =    0;
  auction_info->times      =    0;
  auction_info->timer      =    0;
  auction_info->visible    = TRUE;
  auction_info->obj        = NULL;
  auction_info->seller     = NULL;
  auction_info->buyer      = NULL;
  auction_info->sale       = NULL;
}

FUNCTION( do_auction )
{
  char       buf[MAX_STRING_LENGTH];
  char       buf1[MAX_STRING_LENGTH];
  char       arg[MAX_INPUT_LENGTH];
  OBJ_DATA * auc_obj = auction_info->obj;
  OBJ_DATA * pObj;
  int        amount;
  int        sn;

  PUSH_FUNCTION( "do_auction" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  if( argument[0] == '\x0' )
  {
    if( auction_info->times == 0 || !auction_info->obj )
    {
      send_to_char( "�藍�_�M�ثe�������W�S������f���b�y�q��T\n\r", ch );
      RETURN_NULL();
    }

    chinese_number( auction_info->price, buf );

    if ( auction_info->visible )
    {
      chinese_number( auc_obj->level, buf1 );

      print_to_char( ch , "�ثe�̬ݦn���ӫ~�O %s�M�{���w�g�ۨ�F%s"
                          "������M�O�A�ݤF�M���I�X���a�T���ŬO%s�C\n\r"
        , obj_name( ch, auction_info->obj ), buf, buf1 );
    }

    else
    {
      act( "�ثe�������w�g�ۨ�$t������M���ֳۤW�h�a�M�C�F�N�Ӥ��ΤF�T"
        , ch, buf, NULL, TO_CHAR );
    }

    RETURN_NULL();
  }

  if ( !str_prefix( argument , "!information" ) && IS_IMMORTAL( ch ) )
  {
    if ( ch->level < LEVEL_NODEATH )
    {
      act( "��p�M$n�٤���ݬy�q�f������ơT", ch, NULL, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( auction_info->seller )
    {
      print_to_char( ch , "��D           �R%s\n\r"
                          "�R�D           �R%s\n\r"
                          "����           �R%d\n\r"
                          "�ɶ�           �R%d\n\r"
                          "�쪫�~�p�ɾ�   �R%d\n\r"
                          "���~�W��       �R%s\n\r"
                          "����W��       �R%s\n\r"
                          "���~����       �R%d\n\r"
                          "���~�ɮ�       �R%d\n\r\n\r" ,
      auction_info->seller->name  ,
      auction_info->buyer ? auction_info->buyer->name : "(�q��) " ,
      auction_info->price ,
      auction_info->times ,
      auction_info->timer ,
      auction_info->obj->name ,
      obj_name( ch, auction_info->obj ) ,
      auction_info->obj->level ,
      auction_info->obj->timer );

      identify_obj( ch, auction_info->obj );
    }

    else
    {
      send_to_char( "�ثe�èS���b��檫�~�M�ҥH�L�k�d�ߡC\n\r" , ch );
    }
    RETURN_NULL();
  }

  if ( !str_prefix( argument , "!cancel" ) && IS_GOD( ch ) )
  {
    if ( auction_info->seller )
    {
      if ( auction_info->buyer && !over_scale( auction_info->buyer ) )
        gold_to_char( auction_info->buyer, auction_info->price );

      if ( !IS_NPC( auction_info->seller ) )
      {
        auction_info->obj->timer     = auction_info->timer;
        obj_to_char( auction_info->obj , auction_info->seller );

        act( "�藍�_�M�A��$p�椣�X�h�Q�h�F�^�ӤF�T"
          , auction_info->seller, auction_info->obj, NULL, TO_CHAR );
      }

      else
      {
        extract_obj( auction_info->obj );
      }

      init_auction();

      talk_channel_2( "�藍�_�M�����F�o�������C", CHANNEL_NOTICE, "" );
      send_to_char( "�A�����F�o�������C\n\r" , ch );
      RETURN_NULL();
    }

    else
    {
      send_to_char( "�藍�_�M�S�����~���b���M�ҥH�L�k�����C\n\r" , ch );
      RETURN_NULL();
    }
  }

  if ( !str_prefix( argument , "!set" ) && IS_GOD( ch ) )
  {
    if ( auction_info->seller && auction_info->buyer)
    {
      if ( !IS_NPC( auction_info->seller ) )
      {
        gold_to_char( auction_info->seller, auction_info->price );
        auction_info->obj->timer     = auction_info->timer;
        obj_to_char( auction_info->obj , auction_info->buyer );

        act( "�A�бo��$p���z�e�L�ӤF�T"
           , auction_info->buyer, auction_info->obj, NULL, TO_CHAR );
      }

      else
      {
        extract_obj( auction_info->obj );
      }

      init_auction();
      talk_channel_2( "�藍�_�M�[�֧����F�o�������C" , CHANNEL_NOTICE, "" );
      send_to_char( "�A�[�֧����F�o�������C\n\r" , ch );
      RETURN_NULL();
    }

    else
    {
      send_to_char( "�S�����~���b���M�άO�R�a�M�ҥH�L�k�]�w�C\n\r" , ch );
      RETURN_NULL();
    }
  }

  if ( auction_info->times )
  {
    if ( auction_info->seller == ch && !auction_info->buyer )
    {
      act( "�ݨӬO�S���H��$p������F�M�A�٬O���I�ǳ��I��L���f���a�T"
        , ch, auction_info->obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    send_to_char( "�Q�n��O���F�F�ڡS����o�ӶR���a�T\n\r", ch );
    RETURN_NULL();
  }

  if ( ( reboot_time   > 0 && ( ( reboot_time   - current_time ) < 300 ) )
    || ( shutdown_time > 0 && ( ( shutdown_time - current_time ) < 300 ) ) )
  {
    send_to_char( "�t�ΧY�N�����M�ҥH�L�k�i�����C\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->trade == FALSE )
  {
    send_to_char( "�A���`�겣�ӥi�ȤF�M�ҥH�L�k����o�ӫ��O�T\n\r", ch );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg );

  if ( !( pObj = get_obj_carry( ch, arg ) ) )
  {
    send_to_char( "�A�j�s�ڡS�S�����F��]�����X�ӽ�T\n\r", ch );
    RETURN_NULL();
  }

  /* ���~�O�_�����r */
  if ( poison_char( ch, pObj ) ) RETURN_NULL();

  if ( pObj->address )
  {
    act( "$p�O�n�浹�O�H���H�M���i�H���ө��C", ch, pObj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  /* �ݬݬO���O�i�H��� */
  if ( !can_drop_obj( ch, pObj ) )
  {
    act( "$p�O�A���Ǯa���_�M�ҥH���i�H���ө��C", ch, pObj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  one_argument( argument, arg );
  if ( ( amount = atoi( arg ) ) <= 0 )
  {
    act( "�A�Q�n�K�O�ذe�ڡS��M$p�Ӥ��|�O�B�~�a�T", ch, pObj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( ( amount = atoi( arg ) ) >= UMAX( 1, MaxAuctionGold ) )
  {
    send_to_char( "�Фů}�a��������T\n\r", ch );
    RETURN_NULL();
  }

  switch( pObj->item_type )
  {
  case ITEM_FOOD:
    if ( pObj->value[3] != 0 )
    {
      act( "���r��$p�����X�ӽ�T", ch, pObj, NULL, TO_CHAR );
      RETURN_NULL();
    }
    break;

  case ITEM_DRINK_CON:
    if ( pObj->value[3] != 0 )
    {
      act( "���r��$p�A�����X�ӽ�T", ch, pObj, NULL, TO_CHAR );
      RETURN_NULL();
    }
    break;

  case ITEM_TRASH:
  case ITEM_CORPSE_PC:
  case ITEM_CORPSE_NPC:
  case ITEM_CONTAINER:
  case ITEM_MONEY:

    act( "�藍�_�M���|���O�����$p�T", ch, pObj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( ( sn = pObj->cost ) < 20 )
  {
    act( "$p�o��K�y�M�٬O���n���X�ӽ�n�F�T", ch, pObj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( sn > amount )
  {
    act( "�������O����n�F��M���O�`��$i����������ȧa�S", ch, &sn, NULL, TO_CHAR );
    RETURN_NULL();
  }

  auction_info->seller     = ch;
  auction_info->obj        = pObj;
  auction_info->price      = amount;
  auction_info->buyer      = NULL;
  auction_info->times      = 1;
  auction_info->visible    = TRUE;
  auc_obj                  = auction_info->obj;
  auction_info->timer      = auc_obj->timer;
  auc_obj->timer           = 0;

  chinese_number( auction_info->price, buf1 );
  obj_from_char( auction_info->obj );

  sprintf( buf, "�Ǯa���_ %s �{�b�u�� %s ������T��֦��M"
                "��C�L�M�n���o�⤤�C�n�R�n�ֳ�T\e[0m"
    , obj_name( NULL, auction_info->obj ), buf1 );

  talk_channel( ch, buf , CHANNEL_AUCTION , "\e[1;33m�j�n�s���" );
  message_driver( ch, auction_info->obj, ACT_WHEN_AUCTION );
  RETURN_NULL();
}

FUNCTION( do_bet )
{
  char buf[MAX_STRING_LENGTH];
  char arg[MAX_INPUT_LENGTH];
  int  price;
  int  money;

  PUSH_FUNCTION( "do_bet" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  if ( !auction_info->times || !auction_info->obj )
  {
    send_to_char( "�{�b�S���H�b��F��M�A���Ѥ@�Өӽ�a�T\n\r", ch );
    RETURN_NULL();
  }

  stock_value( ch );

  if ( ch->trade == FALSE )
  {
    send_to_char( "�A���`�겣�ӥi�ȤF�M�ҥH�L�k����o�ӫ��O�T\n\r", ch );
    RETURN_NULL();
  }

  if ( argument[0] == '\x0' )
  {
    act( "�ثe���污������M�w�g����$i������F�C�A���R�N�R���_�F�T"
      , ch, &auction_info->price, NULL, TO_CHAR );

    if ( auction_info->visible )
    {
      money = UMAX( 20, auction_info->obj->cost / 20 );
      act( "�Q�n���D���~�����($2bet help$0)�M�A�����I$i��Ȥl��T"
        , ch, &money, NULL, TO_CHAR );
    }

    RETURN_NULL();
  }

  if ( auction_info->seller == ch )
  {
    send_to_char( "�o�O�A�ۤv���F��C�M�Q�n�������ڡS�j�s�T\n\r", ch );
    RETURN_NULL();
  }

  one_argument( argument, arg );

  if ( !str_prefix( arg, "information" )
    || !str_prefix( arg, "help" ) )
  {
    if ( auction_info->visible == FALSE )
    {
      send_to_char( "�藍�_�M�o�ӬO�˦b���l�̡M�ڤ]�����D�O����T\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->gold >= ( money = UMAX( 20, auction_info->obj->cost / 20 ) ) )
    {
      identify_obj( ch, auction_info->obj );
      gold_from_char( ch, money );
    }

    else
    {
      send_to_char( "�Q�n���D��Ժɪ��f�������M�N�ǳ��I���ӧa�T\n\r", ch );
    }

    RETURN_NULL();
  }

  if ( ch->donate > 0 )
  {
    send_to_char( "�b�A����٪����o�q�ɶ��M�N�u��ݤH���T\n\r", ch );
    RETURN_NULL();
  }

  if ( excess_filequota( ch ) )
  {
    send_to_char( "�藍�_�M�A���ɮפӤj�M�L�k����o�өR�O�T\n\r", ch );
    RETURN_NULL();
  }

  if ( is_pk( ch ) )
  {
    send_to_char( "�A�٬O�M�ߦb�Z���j�|�a�M�O�ީ��F�T\n\r", ch );
    RETURN_NULL();
  }

  if( ( price = atoi( arg ) ) <= 0 )
  {
    send_to_char( "�A�O�b�}�����ܡS\n\r", ch );
    RETURN_NULL();
  }

  if( price > ch->gold )
  {
    send_to_char( "�ڭ̤�������d��T\n\r", ch );
    RETURN_NULL();
  }

  if( price < auction_info->price * 12 / 10 )
  {
    act( "�{�b�w�g�ۨ�F$i��F�C�A�X�o�ػ������|�ܴH�ĶܡS\n\r�ܤ֤]�h�⦨���T"
      , ch, &auction_info->price, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if( price <= auction_info->price )
  {
    send_to_char( "�o��ȭ��M��檺����O�u�ɤ������M"
                  "�p�G�A�Q�n�o�󪫫~�M���N�X���I�a�T\n\r", ch );
    RETURN_NULL();
  }

  if ( auction_info->buyer && !over_scale( auction_info->buyer ) )
    gold_to_char( auction_info->buyer, auction_info->price );

  auction_info->buyer = ch;
  auction_info->price = price;
  auction_info->times = 1;
  gold_from_char( ch, price );

  if ( auction_info->visible )
  {
    sprintf( buf, "�ڥX�� %d ������R %s�T\e[0m"
      , price , obj_name( NULL, auction_info->obj ) );
  }

  else
  {
    sprintf( buf, "�ڥX�� %d ������R�������T\e[0m", price );
  }

  talk_channel( ch, buf, CHANNEL_AUCTION, "\e[1;33m�R�۵�" );
  message_driver( ch, auction_info->obj, ACT_WHEN_BET );
  RETURN_NULL();
}

void auction_update( void )
{
  char             buf[MAX_STRING_LENGTH];
  char             buf1[MAX_STRING_LENGTH];
  char             buf2[MAX_STRING_LENGTH];
  SALE_DATA      * pSale;
  OBJ_DATA       * auc_obj = NULL;
  OBJ_INDEX_DATA * pIndex;

  PUSH_FUNCTION( "auction_update" );

  if ( !auction_info->times < 0
    || !auction_info->seller
    || !auction_info->obj )
  {
    auction_info->times = 0;

    /* �O�_�Ѩt�Ω��F�� */
    system_auction();

    RETURN_NULL();
  }

  if ( auction_info->times++ < 3 )
  {
    if ( auction_info->buyer )
    {
      if ( auction_info->visible )
      {
        chinese_number( auction_info->times -1, buf1 );
        chinese_number( auction_info->price   , buf2 );
        sprintf( buf, "%s �ثe�� %s �s�� \e[1;32m%s\e[0m �������%s���T\e[0m"
          , obj_name( NULL, auction_info->obj ), auction_info->buyer->name
          , buf2, buf1 );
      }

      else
      {
        chinese_number( auction_info->times - 1, buf1 );
        chinese_number( auction_info->price   , buf2 );
        sprintf( buf, "�������ثe�� %s �ۻ� \e[1;32m%s\e[0m"
                      " ������M��%s���T\e[0m"
          , auction_info->buyer->name, buf2, buf1 );
      }

      talk_channel( auction_info->seller, buf, CHANNEL_AUCTION
        , "\e[1;33m�j�n�R�۵�" );
    }

    else
    {
      if ( auction_info->visible )
      {
        chinese_number( auction_info->price, buf1 );
        sprintf( buf, "%s �ثe���X���O%s������T���S���H�n�X��"
                      "�������ڡS\e[0m"
        , obj_name( NULL, auction_info->obj ), buf1 );
      }

      else
      {
        chinese_number( auction_info->price, buf1 );
        sprintf( buf, "�������ثe���X���O%s������T���S���H�n�X��"
          "�������ڡS\e[0m", buf1 );
      }

      talk_channel( auction_info->seller, buf, CHANNEL_AUCTION
        , "\e[1;33m�j�n�s���" );
    }
  }

  else
  {
    if ( !auction_info->buyer )
    {
      str_cpy( buf, "\e[1;33m���M�S���ѳf���H�ڡM�U���A���X�ӽ�a�T\e[0m" );

      if ( !IS_NPC( auction_info->seller ) )
      {
        auc_obj        = auction_info->obj;
        auc_obj->timer = auction_info->timer;
        obj_to_char( auc_obj, auction_info->seller );

        act( "�藍�_�M�A��$p�椣�X�h�Q�h�F�^�ӤF�T"
          , auction_info->seller, auction_info->obj, NULL, TO_CHAR );
      }

      else
      {
        extract_obj( auction_info->obj );
      }
    }

    else
    {
      if ( auction_info->visible )
      {
        chinese_number( auction_info->price, buf1 );
        sprintf( buf, "%s �H%s�����������浹�F %s�T\e[0m"
          , obj_name( NULL, auction_info->obj ), buf1
          , ( !is_affected( auction_info->buyer, SLOT_INVIS )
            && !is_affected( auction_info->buyer, SLOT_MASS_INVIS ) )
            ? auction_info->buyer->name : "�Y�����W�H��");

        pIndex = auction_info->obj->pIndexData;

        pIndex->auction_times++;
        pIndex->auction_gold += auction_info->price;

        if ( !pIndex->Sale
         && ( pIndex->auction_times > AuctionTimes
           || pIndex->auction_gold   > AuctionGold ) )
        {
          set_sale_default( pSale = alloc_struct( STRUCT_SALE_DATA ) );

          pSale->obj     = pIndex;
          pSale->visible = TRUE;
          pSale->cost    = pIndex->auction_gold / pIndex->auction_times;
          pIndex->Sale   = TRUE;

          pSale->next = sale_list;
          sale_list   = pSale;
        }
      }

      else
      {
        chinese_number( auction_info->price, buf1 );
        sprintf( buf, "�������H%s�����������浹�F %s�T\e[0m"
          , buf1
          , ( !is_affected( auction_info->buyer, SLOT_INVIS )
            && !is_affected( auction_info->buyer, SLOT_MASS_INVIS ) )
            ? auction_info->buyer->name : "�Y�����W�H��");
      }

      if ( !IS_NPC( auction_info->seller )
        && !over_scale( auction_info->seller ) )
      {
        gold_to_char( auction_info->seller, auction_info->price );
      }

      auc_obj                     = auction_info->obj;
      auc_obj->timer              = auction_info->timer;
      obj_to_char( auc_obj, auction_info->buyer );

      act( "�A�бo��$p���z�e�L�ӤF�T"
        , auction_info->buyer, auction_info->obj, NULL, TO_CHAR );


      if ( auction_info->sale )
      {
        auction_info->sale->gold += auction_info->price;
        auction_info->sale->sold++;
      }
    }

    talk_channel( auction_info->seller, buf, CHANNEL_AUCTION,
      "\e[1;33m�ť��D" );

    init_auction();
  }

  RETURN_NULL();
}

void system_auction( void )
{
  MOB_INDEX_DATA * mob;
  SALE_DATA      * pSale;
  SALE_DATA      * zSale;
  CHAR_DATA      * victim;
  CHAR_DATA      * seller;
  OBJ_DATA       * pObj;
  int              count = 0;
  char           * message;
  char             buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "system_auction" );

  if ( !sale_list
    || number_range( 1, 1000 ) > SaleChance
    || get_usernum() < SaleLimit )
    RETURN_NULL();

  if ( ( reboot_time   > 0 && ( ( reboot_time   - current_time ) < 300 ) )
    || ( shutdown_time > 0 && ( ( shutdown_time - current_time ) < 300 ) ) )
    RETURN_NULL();

  count = 0;
  zSale = NULL;

  for ( pSale = sale_list; pSale; pSale = pSale->next )
    if ( pSale->obj && ( number_range( 0, count++ ) == 0 ) ) zSale = pSale;

  if ( zSale == NULL ) RETURN_NULL();

  count  = 0;
  seller = NULL;

  for ( count = 0, victim = char_list; victim; victim = victim->next )
  {
    if ( verify_char( victim )
      && ( mob = victim->pIndexData )
      && mob->auction
      && *mob->auction
      && number_range( 0, count++ ) == 0 )
      seller = victim;
  }

  if ( seller == NULL ) RETURN_NULL();

  if ( !( pObj = create_object( zSale->obj, -1 ) ) ) RETURN_NULL();

  auction_info->seller     = seller;
  auction_info->obj        = pObj;
  auction_info->price      = zSale->cost;
  auction_info->buyer      = NULL;
  auction_info->times      = 1;
  auction_info->timer      = pObj->timer;
  auction_info->visible    = zSale->visible;
  auction_info->sale       = zSale;

  zSale->times++;
  pObj->timer = 0;

  message = seller->pIndexData->auction;
  talk_channel( seller, message , CHANNEL_AUCTION , "\e[1;33m���D" );

  if ( zSale->visible )
  {
    sprintf( buf, "�Ǯa���_ %s �{�b�u�� %d ������T��֦��M"
                  "��C�L�M�n���o�⤤�C�n�R�n�ֳ�T\e[0m"
      , obj_name( NULL, pObj ), zSale->cost );
  }

  else
  {
    sprintf( buf, "���l�̤����D�ˤF���򰭡M�ϥ��U�U��M�u�n %d ������T"
      "�M�ЦU�즳�����ӥͷN���M�S�������ӤH���a�T"
      , zSale->cost );
  }

  talk_channel( seller, buf , CHANNEL_AUCTION , "\e[1;33m�j�n�s���" );
  RETURN_NULL();
}

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

void    set_cachet_default      args( ( CACHET_DATA * ) );

/* �]�w��� */
void set_cachet_default( CACHET_DATA * pCachet )
{
  PUSH_FUNCTION( "set_cachet_default" );
  pCachet->vnum = ERRORCODE;
  RETURN_NULL();
}

FUNCTION( do_cachet )
{
  int              loop;
  char             arg1[MAX_INPUT_LENGTH];
  char             arg2[MAX_INPUT_LENGTH];
  char             buf[MAX_STRING_LENGTH];
  OBJ_DATA       * obj;
  OBJ_DATA       * stone_obj;
  OBJ_INDEX_DATA * pIndex;
  CACHET_DATA    * pCachet;
  CACHET_DATA    * pCachet_next;

  PUSH_FUNCTION( "do_cachet" );

  argument = one_argument( argument, arg1 );
  one_argument( argument, arg2 );

  if ( !arg1[0] || !arg2[0] )
  {
    send_to_char( "�Ѽƿ��~�M�Ьd�� cachet /?\n\r", ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg1, "!list" ) )
  {
    if ( !( obj = get_obj_carry( ch, arg2 ) ) )
    {
      send_to_char( "�A�èS���o�Ӹ˳ơT\n\r", ch );
      RETURN_NULL();
    }

    if ( !obj->cachet )
    {
      act( "$p�èS���ʦL�C", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    clear_buffer();
    send_to_buffer( "%s�ʦL���]�ۦ��R\n\r", obj_name( ch, obj ) );

    for ( loop = 0, pCachet = obj->cachet; pCachet; pCachet = pCachet->next )
    {
      if ( !( pIndex = get_obj_index( pCachet->vnum ) ) ) continue;
      send_to_buffer( "%3d. %s(%s)\n\r"
        , ++loop, pIndex->short_descr, pIndex->name );
    }

    if ( loop == 0 ) send_to_buffer( "%s�S���ʦL�����]�ۡC"
      , obj_name( ch, obj ) );

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg1, "!back" ) )
  {
    if ( !( obj = get_obj_carry( ch, arg2 ) ) )
    {
      send_to_char( "�A�èS���o�Ӹ˳ơT\n\r", ch );
      RETURN_NULL();
    }

    if ( !obj->cachet )
    {
      act( "$p�èS���ʦL�C", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( obj->wear_loc != WEAR_NONE )
    {
      act( "$p�ثe�L�k�Ѱ��ʦL�C", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    for ( pCachet = obj->cachet; pCachet; pCachet = pCachet_next )
    {
      pCachet_next = pCachet->next;

      if ( ( pIndex = get_obj_index( pCachet->vnum ) ) )
      {
        stone_obj = create_object( ObjMagicStone, -1 );

        if ( !stone_obj )
        {
          send_to_char( "�藍�_�M�t�εL�k�����]�ۡT\n\r", ch );
          mudlog( LOG_DEBUG, "do_cachet: �L�k�����]��." );
          continue;
        }

        /* �⪫�~�����H */
        obj_to_char( stone_obj, ch );

        act( "$n�N$p���ʦL���U�M$p���A����ģ���F�M���L$n���W���G�h�F�@��$P�T"
          , ch, obj, stone_obj, TO_ALL );
      }

      free_struct( pCachet, STRUCT_CACHET_DATA );
    }

    obj->cachet = NULL;

    save_char_obj( ch, SAVE_FILE );
    RETURN_NULL();
  }

  if ( !( stone_obj = get_obj_carry( ch, arg1 ) ) )
  {
    send_to_char( "�A���W�èS���o�@���]�ۡT\n\r", ch );
    RETURN_NULL();
  }

  if ( !is_magic_stone( stone_obj ) )
  {
    act( "$p�ä��O�@���]�ۡT", ch, stone_obj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( !( obj = get_obj_carry( ch, arg2 ) ) )
  {
    send_to_char( "�A���W�èS���o�@�Ӹ˳ơT\n\r", ch );
    RETURN_NULL();
  }

  if ( !can_cachet( obj ) || !stone_obj->pIndexData->impact )
  {
    act( "$p�ڥ��S��k�ʦL�]�ۡC", ch, obj, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( obj_stone( obj ) > MAX_CAN_CACHET )
  {
    chinese_number( MAX_CAN_CACHET, buf );
    act( "$p�w�g�ʦL�W�L�̤j�ʦL����� $T �F�T", ch, obj, buf, TO_CHAR );
    RETURN_NULL();
  }

  for ( pCachet = obj->cachet; pCachet; pCachet = pCachet->next )
  {
    if ( pCachet->vnum == stone_obj->pIndexData->vnum )
    {
      act( "$p�w�g�ʦL���ۦP�������]�ۤF�C", ch, obj, NULL, TO_CHAR );
      RETURN_NULL();
    }
  }

  /* �t�m�O���� */
  set_cachet_default( pCachet = alloc_struct( STRUCT_CACHET_DATA ) );
  pCachet->vnum = stone_obj->pIndexData->vnum;
  pCachet->next = obj->cachet;
  obj->cachet   = pCachet;

  act( "$n��$p�ʦL��$P�̭��M$p�o�X�@�}�{������N�����F�C$n"
    "�P��$P���@�Ǥ��@�˪��a��M�o�X�}�}�����~�C"
    , ch, stone_obj, obj, TO_ALL );

  message_driver( ch, stone_obj, ACT_WHEN_CACHET );

  /* ���񪫫~ */
  extract_obj( stone_obj );

  /* �x�s�˳� */
  save_char_obj( ch, SAVE_FILE );

  RETURN_NULL();
}

/* �ˬd���~�O�_���]�� */
bool is_magic_stone( OBJ_DATA * obj )
{
  PUSH_FUNCTION( "is_magic_stone" );

  if ( !obj )
  {
    mudlog( LOG_DEBUG, "is_magic_stone: �ǤJ���~���s�b." );
    RETURN( FALSE );
  }

  if ( obj->item_type != ITEM_MAGICSTONE || !obj->pIndexData->impact )
    RETURN( FALSE );

  RETURN( TRUE );
}

/* ��ݪ��~�O�_�i�H�ʦL */
bool can_cachet( OBJ_DATA * obj )
{
  PUSH_FUNCTION( "can_cachet" );

  if ( !obj )
  {
    mudlog( LOG_DEBUG, "can_cachet: �ǤJ�����~�����T." );
    RETURN( FALSE );
  }

  if ( !is_armor( obj ) || !obj->Cancachet ) RETURN( FALSE );

  RETURN( TRUE );
}

/* �Ǧ^���~�ʦL�]�۪��ƶq */
int obj_stone( OBJ_DATA * obj )
{
  int           number;
  CACHET_DATA * pCachet;

  PUSH_FUNCTION( "obj_stone" );

  if ( !obj ) RETURN( ERRORCODE );

  number = 0;
  for ( pCachet = obj->cachet; pCachet; pCachet = pCachet->next ) number++;

  RETURN( number );
}

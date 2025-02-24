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

CHAR_DATA * get_rebirth_char args( ( CHAR_DATA * , char * ) );

/* ��¾ */
FUNCTION( do_rebirth )
{
  char         arg[MAX_INPUT_LENGTH];
  CHAR_DATA  * mob;
  CLASS_DATA * pClass;
  AREA_DATA  * area;
  int          count;

  PUSH_FUNCTION( "do_rebirth" );

  /* �D���a������¾ */
  if ( IS_NPC( ch ) ) RETURN_NULL();

  if ( !ch->class )
  {
    send_to_char( "�A��¾�~�����M�L�k��¾�C\n\r", ch );
    RETURN_NULL();
  }

  argument = one_argument( argument , arg );

  /* ��J�r�ꬰ�Ŧr�� */
  if ( !arg[0] )
  {
    send_to_char( "�A�p�G����¾���A�ѡM�Ьd�� rebirth�C\n\r" , ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg, "!list" ) )
  {
    clear_buffer();
    send_to_buffer( "\e[1;33;44m���� �� ��W �� �P�� ¾�~�W ����W��"
      "                                            \e[0m\n\r" );

    for ( count = 0, mob = char_list; mob; mob = mob->next )
    {
      if ( verify_char( mob )
        && IS_NPC( mob )
        && IS_SET( mob->act,  ACT_REBIRTH ) )
      {
        area = mob->in_room && mob->in_room->area ? mob->in_room->area : NULL;
        send_to_buffer( "%3d. %10s  %2s  %6s %-s\n\r"
          , ++count
          , area ? area->name : "�q��"
          , YESNO( area && ch->in_room && ch->in_room->area == area )
          , class_name( mob->class, FALSE )
          , mob_name( ch, mob ) );

        if ( buffer_full() ) break;
      }
    }

    if ( count == 0 ) send_to_char( "�S���i�H��¾���H�T\n\r", ch );
    else print_buffer( ch );

    RETURN_NULL();
  }

  /* IMM ������¾ */
  if ( IS_IMMORTAL( ch ) )
  {
    send_to_char( "�A��¾�~�O�t�κ޲z���M�र��¾�ڡC\n\r" , ch );
    RETURN_NULL();
  }

  /* ���P�@�ӷQ�n�ন¾�~���H */
  if ( !( mob = get_rebirth_char( ch , arg ) ) )
  {
    act( "�o�̨S����¾�� $T�M�٬O�V�O�H��ť�ݬݧa�C" , ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  /* �O�_�@�N���O�H��¾, �D���a���� */
  if ( (  IS_NPC( mob ) && !IS_SET( mob->act,  ACT_REBIRTH ) )
    || ( !IS_NPC( mob ) && !IS_SET( mob->turn, PLR_REBIRTH ) ) )
  {
    act( "�藍�_�M$N�L�k���A��¾�T", ch, NULL, mob, TO_CHAR );
    RETURN_NULL();
  }

  /* ���୫����¾ */
  if ( ch->level >= ch->class->low_rebirth
    && ch->level <= ch->class->high_rebirth )
  {
    send_to_char( "�藍�_�M�A���୫����¾�C", ch );
    RETURN_NULL();
  }

  if ( !( pClass = mob->class ) )
  {
    send_to_char( "�t��¾�~�����D�C" , ch );
    mudlog( LOG_DEBUG , "do_rebirth: �Ǫ� %d ¾�~�����D"
      , mob->pIndexData ? mob->pIndexData->vnum : -1 );
    RETURN_NULL();
  }

  /* ���ũ|���F�� */
  if ( ch->level < pClass->low_rebirth || ch->level > pClass->high_rebirth )
  {
    act( "�A���A�X�{�b���$T�C", ch, NULL, class_name( pClass, TRUE ), TO_CHAR );
    RETURN_NULL();
  }

  /* �ݬݰ�������¾�� */
  if ( ch->gold < pClass->rebirth_gold )
  {
    send_to_char( "��p�M�A���W�������Ӱ�������¾�C�C" , ch );
    RETURN_NULL();
  }

  /* �Y��¾�~������¾�o�� */
  if ( pClass->associate != ch->class->vnum )
  {
    act( "��p�M�A����o����¾��$T�C" , ch, NULL, class_name( pClass, TRUE ), TO_CHAR );
    RETURN_NULL();
  }

  /* ���Ťw�g�F����¾�з� */
  ch->class = mob->class;
  gold_from_char( ch, pClass->rebirth_gold );
  set_title( ch, pClass->title );

  act( "���H�A�M�A�w�g��¾��$t�o��¾�~�F�C�A�I�F$I��Ȥl�C"
    , ch, class_name( pClass, TRUE ) , &pClass->rebirth_gold, TO_CHAR );

  RETURN_NULL();
}

/* �d��¾�~ */
FUNCTION( do_vocation )
{
  int           count;
  extern char * const attr_name[];
  char          arg[ MAX_INPUT_LENGTH ];
  CLASS_DATA *  pClass;
  CLASS_DATA *  pAssociate;

  PUSH_FUNCTION( "do_vocation" );

  if IS_NPC( ch ) RETURN_NULL();

  one_argument( argument , arg );

  /* �S���Ѽƫh�C�L�Ҧ���¾�~²�� */
  if ( !arg[0] )
  {
    for ( pClass = class_list; pClass; pClass = pClass->next )
    {
      send_to_buffer( "(%2d) %6s %20s %10d\n\r"
        , pClass->vnum, pClass->cname, pClass->name, pClass->mask );

      if ( buffer_full() ) break;
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  /* ����X�ݬݬO�_�ŦX¾�~���ﶵ */
  if ( ( pClass = is_class( arg, TRUE ) ) )
  {
    clear_buffer();
    send_to_buffer( "¾�~���X�R%d\n\r"
                    "�^��W�١R%s\n\r"
                    "����W�١R%s\n\r"
                    "�w�]���Y�R%s\n\r"
                    "��¾���ŤU���R%d\n\r"
                    "��¾���ŤW���R%d\n\r"
                    "��¾�һݪ����R%d\n\r"
                    "��¾ĵ�i���šR%d\n\r"
                    "�i�ǧޯ�ƥءR%d\n\r"
                    "�����I��    �R%d\n\r"
                    "�����    �R%d\n\r"
                    "����ԭz    �R%s\n\r"
                    "��_�o��k�O�R%s\n\r"
                    "�w�]¾�~�R    %s\n\r"
                    "�j����¾¾�~�R%s\n\r"
                    "�O�_�i�H��ܡR%s\n\r"
      , pClass->vnum
      , pClass->name
      , pClass->cname
      , pClass->title
      , pClass->low_rebirth
      , pClass->high_rebirth
      , pClass->rebirth_gold
      , pClass->warn
      , pClass->nskill
      , pClass->limit
      , pClass->multiplier
      , pClass->msg_limit ? pClass->msg_limit : "(�q��)"
      , YESNO( pClass->fMana    )
      , YESNO( pClass->rudiment )
      , YESNO( pClass->force    )
      , YESNO( pClass->select   ) );

      if ( ( pAssociate = class_lookup( pClass->associate ) ) )
        send_to_buffer( "�Ѥ���¾�~��¾�ӨӡR%s\n\r"
          , pAssociate->cname );

      for ( count = 0; count < MAX_ATTR; count++ )
      {
        send_to_buffer( "%2d.%s�ݩʪ��W���R%2d �ݩʪ��]�ơR%3d "
          "�w�]�W���R%2d �w�]�U���R%2d �^���[���R%2d\n\r"
          , count + 1
          , attr_name[count]
          , pClass->attr[count]
          , pClass->factor[count]
          , pClass->max_default[count]
          , pClass->min_default[count]
          , pClass->hero[count] );
      }
      print_buffer( ch );
      RETURN_NULL();
  }

  send_to_char( "�S�����A�n�d�ߪ�¾�~��ơM�Ьd�� vocation�C\n\r", ch );
  RETURN_NULL();
}

CHAR_DATA * get_rebirth_char( CHAR_DATA * ch , char * arg )
{
  CHAR_DATA * mob;

  PUSH_FUNCTION( "get_rebirth_char" );

  if ( !( mob = get_char_room( ch , arg ) ) ) RETURN( NULL );

  /* �ݬݬO���O�t�κ޲z��, �O���ܳ��i�H���O�H�� */
  if ( IS_IMMORTAL( mob ) ) RETURN( mob );

  RETURN( mob );
}

char * class_name( CLASS_DATA * pClass, bool bFull )
{
  static char buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "class_name" );

  if ( !pClass ) RETURN( "��  ��" );

  if ( bFull ) sprintf( buf, "%s(%s)", pClass->cname, pClass->name );
  else         str_cpy( buf, pClass->cname );

  RETURN( buf );
}

CLASS_DATA * class_lookup( int vnum )
{
  CLASS_DATA * pClass;

  PUSH_FUNCTION( "class_lookup" );

  for ( pClass = class_list; pClass; pClass = pClass->next )
    if ( pClass->vnum == vnum ) RETURN( pClass );

  RETURN( NULL );
}

CLASS_DATA * is_class( char * name, bool Exact )
{
  CLASS_DATA * pClass;

  PUSH_FUNCTION( "is_class" );

  if ( !name || *name == '\x0' ) RETURN( NULL );

  for ( pClass = class_list; pClass; pClass = pClass->next )
  {
    if ( Exact )
    {
      if ( !str_cmp( name, pClass->name ) ) break;
    }
    else
    {
      if ( !str_prefix( name, pClass->name ) ) break;
    }
  }

  RETURN( pClass );
}

/* �O�_���P�@��¾�~ */
bool is_same_class( CHAR_DATA * ach, CHAR_DATA * bch )
{
  PUSH_FUNCTION( "is_same_class" );

  if ( !ach || !bch || !ach->class || !bch->class || ach->class != bch->class )
    RETURN( FALSE );

  RETURN( TRUE );
}

int get_skill_class_level( SKILL_DATA * pSkill, CLASS_DATA * pClass )
{
  LIMIT_DATA * pLimit;

  PUSH_FUNCTION( "get_skill_class_level" );

  for ( pLimit = pSkill->limit; pLimit; pLimit = pLimit->next )
    if ( pLimit->class == pClass ) RETURN( pLimit->level );

  RETURN( MAX_LEVEL + 100 );
}

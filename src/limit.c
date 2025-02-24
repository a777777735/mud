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

bool    check_weapon      args( ( CHAR_DATA *, int, bool ) );

#define CHECK_CHAR( character )                   \
{                                                 \
  if ( !character )                               \
  {                                               \
    mudlog( LOG_DEBUG , "�ˬd�ޯ�, ch �O�Ū�." ); \
    RETURN( FALSE );                              \
  }                                               \
}

bool check_hand( CHAR_DATA * ch , int type )
{
  OBJ_DATA   * obj;
  SITUS_DATA * pSitus;
  int          kind;

  PUSH_FUNCTION( "check_hand" );

  for ( kind = 0 ,obj = ch->carrying; obj; obj = obj->next_content )
  {
    if ( ( obj->item_type != ITEM_WEAPON )
      || ( type != WEAPON_ALL && obj->value[3] != type ) ) continue;

    for ( pSitus = situs_list; pSitus; pSitus = pSitus->next )
      if ( pSitus->wield && pSitus->location == obj->wear_loc ) RETURN( TRUE );
  }

  RETURN( FALSE );
}

/* �T�w�O�_�i�H�A���� */
bool check_continue( CHAR_DATA * ch, CHAR_DATA * victim )
{
  PUSH_FUNCTION( "check_continue" );

  if ( !ch || !victim
    || ch->in_room      != victim->in_room
    || ch->position     != POS_FIGHTING
    || victim->position != POS_FIGHTING ) RETURN( FALSE );

  RETURN( TRUE );
}

bool check_same_weapon( CHAR_DATA * ch, bool fPrint )
{
  OBJ_DATA * objr;
  OBJ_DATA * objl;

  PUSH_FUNCTION( "check_same_weapon" );

  objr = get_eq_char( ch, WEAR_WIELD );
  objl = get_eq_char( ch, WEAR_HOLD  );

  if ( objr
    && objl
    && objr->item_type == ITEM_WEAPON
    && objl->item_type == ITEM_WEAPON
    && objr->value[3] != objl->value[3] )
  {
    if ( fPrint ) act( "$n�٤��|������Z���M�L�k�I�i�ۦ��T"
      , ch, NULL, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  RETURN( TRUE );
}

/* �ˬd�Z�����A�O�_���T */
bool check_weapon( CHAR_DATA * ch, int type, bool fPrint )
{
  PUSH_FUNCTION( "check_weapon" );

  /* �ˬd�H�� */
  CHECK_CHAR( ch );

  switch( type )
  {
  default:
    RETURN( FALSE );

  /* �Ť�ۦ� */
  case WEAPON_HAND:

    if ( check_hand( ch , WEAPON_ALL ) )
    {
      if ( fPrint )
        send_to_char( "�o�ۦ����ݪŤ�M�A�L�k�I�i�o�ӧޯ�T\n\r" , ch );
      RETURN( FALSE );
    }

    if ( check_same_weapon( ch, fPrint ) == FALSE ) RETURN( FALSE );
    RETURN( TRUE );

  /* �ˬd�O�_���C */
  case WEAPON_SWORD:

    if ( !check_hand( ch, WEAPON_SWORD ) )
    {
      if ( fPrint ) send_to_char( "�A�����n���C�~��I�i�C�N�T\n\r" , ch );
      RETURN( FALSE );
    }

    if ( check_same_weapon( ch, fPrint ) == FALSE ) RETURN( FALSE );
    RETURN( TRUE );

  /* �ˬd�O�_���M */
  case WEAPON_BLADE:

    if ( !check_hand( ch, WEAPON_BLADE ) )
    {
      if ( fPrint ) send_to_char( "�A�����n���M�~��I�i�M�N�T\n\r" , ch );
      RETURN( FALSE );
    }

    if ( check_same_weapon( ch, fPrint ) == FALSE ) RETURN( FALSE );
    RETURN( TRUE );

  /* �ˬd�O�_���P�� */
  case WEAPON_DAGGER:

    if ( !check_hand( ch, WEAPON_DAGGER ) )
    {
      if ( fPrint ) send_to_char( "�A�����n���P���~��I�i�P���N�T\n\r" , ch );
      RETURN( FALSE );
    }

    if ( check_same_weapon( ch, fPrint ) == FALSE ) RETURN( FALSE );
    RETURN( TRUE );

  /* �ˬd�O�_���� */
  case WEAPON_AXE:

    if ( !check_hand( ch, WEAPON_AXE ) )
    {
      if ( fPrint ) send_to_char( "�A�����n�����Y�~��I�i���Y�N�T\n\r" , ch );
      RETURN( FALSE );
    }

    if ( check_same_weapon( ch, fPrint ) == FALSE ) RETURN( FALSE );
    RETURN( TRUE );

  /* �ˬd�O�_���@ */
  case WEAPON_WHIP:

    if ( !check_hand( ch, WEAPON_WHIP ) )
    {
      if ( fPrint ) send_to_char( "�A�����n���@�~��I�i�@�N�T\n\r" , ch );
      RETURN( FALSE );
    }

    if ( check_same_weapon( ch, fPrint ) == FALSE ) RETURN( FALSE );
    RETURN( TRUE );

  /* �ˬd�O�_���j */
  case WEAPON_SPEAR:

    if ( !check_hand( ch, WEAPON_SPEAR ) )
    {
      if ( fPrint ) send_to_char( "�A�����n���j�~��I�i�j�N�T\n\r" , ch );
      RETURN( FALSE );
    }

    if ( check_same_weapon( ch, fPrint ) == FALSE ) RETURN( FALSE );
    RETURN( TRUE );

  /* �ˬd�O�_���� */
  case WEAPON_PEN:

    if ( !check_hand( ch, WEAPON_PEN ) )
    {
      if ( fPrint ) send_to_char( "�A�����n�����~��I�i���N�T\n\r" , ch );
      RETURN( FALSE );
    }

    if ( check_same_weapon( ch, fPrint ) == FALSE ) RETURN( FALSE );
    RETURN( TRUE );

  /* �ˬd�O�_���� */
  case WEAPON_HAMMER:

    if ( !check_hand( ch, WEAPON_HAMMER ) )
    {
      if ( fPrint ) send_to_char( "�A�����n����~��I�i��N�T\n\r" , ch );
      RETURN( FALSE );
    }

    if ( check_same_weapon( ch, fPrint ) == FALSE ) RETURN( FALSE );
    RETURN( TRUE );

  /* �ˬd�O�_���� */
  case WEAPON_CLUB:

    if ( !check_hand( ch, WEAPON_CLUB ) )
    {
      if ( fPrint ) send_to_char( "�A�����n���Τ~��I�i�γN�T\n\r" , ch );
      RETURN( FALSE );
    }

    if ( check_same_weapon( ch, fPrint ) == FALSE ) RETURN( FALSE );
    RETURN( TRUE );

  /* �ˬd�O�_���} */
  case WEAPON_BOW:

    if ( !check_hand( ch, WEAPON_BOW ) )
    {
      if ( fPrint ) send_to_char( "�A�����n���}�~��I�i�}�N�T\n\r" , ch );
      RETURN( FALSE );
    }

    if ( check_same_weapon( ch, fPrint ) == FALSE ) RETURN( FALSE );
    RETURN( TRUE );

  }
}

/* �ˬd�O�_���C */
bool check_sword_attack( CHAR_DATA * ch, bool fPrint )
{
  PUSH_FUNCTION( "check_sword_attack" );

  /* �ˬd�H�� */
  CHECK_CHAR( ch );

  /* �ˬd�O�_���C */
  if ( check_weapon( ch, WEAPON_SWORD, fPrint ) == FALSE ) RETURN( FALSE );
  RETURN( TRUE );
}

/* �ˬd�O�_�i�H���} */
bool check_unrigid_attack( CHAR_DATA * ch, bool fPrint )
{
  PUSH_FUNCTION( "check_unrigid_attack" );

  /* �ˬd�H�� */
  CHECK_CHAR( ch );

  /* ��W���Z���h����I�i */
  if ( check_weapon( ch, WEAPON_HAND, fPrint ) == FALSE ) RETURN( FALSE );
  RETURN( TRUE );
}

/* �ˬd�O�_�Ǫ��i�H�Ϊ����} */
bool check_pc_attack( CHAR_DATA * ch, bool fPrint )
{
  PUSH_FUNCTION( "check_pc_attack" );

  /* �ˬd�H�� */
  CHECK_CHAR( ch );

  /* ��W���Z���h����I�i */
  if ( check_weapon( ch, WEAPON_HAND, fPrint ) == FALSE ) RETURN( FALSE );

  /* �Ǫ��L�kŪ�� */
  if ( IS_NPC( ch ) ) RETURN( FALSE );

  RETURN( TRUE );
}

/* �ˬd�O�_���j */
bool check_spear_attack( CHAR_DATA * ch, bool fPrint )
{
  PUSH_FUNCTION( "check_spear_attack" );

  /* �ˬd�H�� */
  CHECK_CHAR( ch );

  /* �ˬd�O�_���j */
  if ( check_weapon( ch, WEAPON_SPEAR, fPrint ) == FALSE ) RETURN( FALSE );
  RETURN( TRUE );
}

/* �ˬd�O�_���M */
bool check_blade_attack( CHAR_DATA * ch, bool fPrint )
{
  PUSH_FUNCTION( "check_blade_attack" );

  /* �ˬd�H�� */
  CHECK_CHAR( ch );

  /* �ˬd�O�_���M */
  if ( check_weapon( ch, WEAPON_BLADE, fPrint ) == FALSE ) RETURN( FALSE );
  RETURN( TRUE );
}

/* �ˬd�O�_���@ */
bool check_whip_attack( CHAR_DATA * ch, bool fPrint )
{
  PUSH_FUNCTION( "check_whip_attack" );

  /* �ˬd�H�� */
  CHECK_CHAR( ch );

  /* �ˬd�O�_���@ */
  if ( check_weapon( ch, WEAPON_WHIP, fPrint ) == FALSE ) RETURN( FALSE );
  RETURN( TRUE );
}

/* �ˬd�O�_���P�� */
bool check_dagger_attack( CHAR_DATA * ch, bool fPrint )
{
  PUSH_FUNCTION( "check_dagger_attack" );

  /* �ˬd�H�� */
  CHECK_CHAR( ch );

  /* �ˬd�O�_���P�� */
  if ( check_weapon( ch, WEAPON_DAGGER, fPrint ) == FALSE ) RETURN( FALSE );
  RETURN( TRUE );
}

/* �ˬd�O�_���� */
bool check_axe_attack( CHAR_DATA * ch, bool fPrint )
{
  PUSH_FUNCTION( "check_axe_attack" );

  /* �ˬd�H�� */
  CHECK_CHAR( ch );

  /* �ˬd�O�_���� */
  if ( check_weapon( ch, WEAPON_AXE, fPrint ) == FALSE ) RETURN( FALSE );
  RETURN( TRUE );
}

/* �ˬd�O�_���Ҵ� */
bool check_club_attack( CHAR_DATA * ch, bool fPrint )
{
  PUSH_FUNCTION( "check_club_attack" );

  /* �ˬd�H�� */
  CHECK_CHAR( ch );

  /* �ˬd�O�_���Ҵ� */
  if ( check_weapon( ch, WEAPON_CLUB, fPrint ) == FALSE ) RETURN( FALSE );
  RETURN( TRUE );
}

/* �ˬd�O�_���� */
bool check_pen_attack( CHAR_DATA * ch, bool fPrint )
{
  PUSH_FUNCTION( "check_pen_attack" );

  /* �ˬd�H�� */
  CHECK_CHAR( ch );

  /* �ˬd�O�_���� */
  if ( check_weapon( ch, WEAPON_PEN, fPrint ) == FALSE ) RETURN( FALSE );
  RETURN( TRUE );
}

/* �ˬd�O�_���} */
bool check_bow_attack( CHAR_DATA * ch, bool fPrint )
{
  PUSH_FUNCTION( "check_bow_attack" );

  /* �ˬd�H�� */
  CHECK_CHAR( ch );

  /* �ˬd�O�_���} */
  if ( check_weapon( ch, WEAPON_BOW, fPrint ) == FALSE ) RETURN( FALSE );
  RETURN( TRUE );
}

/* �ˬd�O�_���l�l */
bool check_hammer_attack( CHAR_DATA * ch, bool fPrint )
{
  PUSH_FUNCTION( "check_hammer_attack" );

  /* �ˬd�H�� */
  CHECK_CHAR( ch );

  /* �ˬd�O�_���l�l */
  if ( check_weapon( ch, WEAPON_HAMMER, fPrint ) == FALSE ) RETURN( FALSE );
  RETURN( TRUE );
}

bool check_can_kill( CHAR_DATA * ch, CHAR_DATA * victim )
{
  PUSH_FUNCTION( "check_can_kill" );

  if ( !can_see( ch, victim )
    || !IS_NPC( victim )
    || IS_IMMORTAL( victim )
    || !can_damage( victim ) ) RETURN( FALSE );

  RETURN( TRUE );
}

int check_skill_rating( char * argument )
{
  PUSH_FUNCTION( "check_skill_rating" );

  switch( argument[0] )
  {
  default:
    RETURN( ERRORCODE );

  case 'c':
    if ( !str_prefix( argument, "create"    ) ) RETURN( RATING_CREATE );
    if ( !str_prefix( argument, "cure"      ) ) RETURN( RATING_CURE  );
    break;

  case 'd':
    if ( !str_prefix( argument, "darkness"  ) ) RETURN( RATING_DARKNESS );
    break;

  case 'e':
    if ( !str_prefix( argument, "earth"     ) ) RETURN( RATING_EARTH );
    if ( !str_prefix( argument, "evil"      ) ) RETURN( RATING_EVIL  );
    break;

  case 'f':
    if ( !str_prefix( argument, "fight"     ) )  RETURN( RATING_FIGHT  );
    if ( !str_prefix( argument, "fire"      ) )  RETURN( RATING_FIRE  );
    break;

  case 'l':
    if ( !str_prefix( argument, "light"     ) ) RETURN( RATING_LIGHT );
    if ( !str_prefix( argument, "lightning" ) ) RETURN( RATING_LIGHTNING );
    if ( !str_prefix( argument, "lost"      ) ) RETURN( RATING_LOST );
    break;

  case 'm':
    if ( !str_prefix( argument, "murder"    ) ) RETURN( RATING_MURDER );
    break;

  case 'p':
    if ( !str_prefix( argument, "poison"    ) ) RETURN( RATING_POISON );
    break;

  case 's':
    if ( !str_prefix( argument, "saint"     ) ) RETURN( RATING_SAINT );
    if ( !str_prefix( argument, "sing"      ) ) RETURN( RATING_SING  );
    if ( !str_prefix( argument, "skill"     ) ) RETURN( RATING_SKILL );
    break;

  case 't':
    if ( !str_prefix( argument, "thief"     ) ) RETURN( RATING_THIEF );
    break;

  case 'w':
    if ( !str_prefix( argument, "water"     ) ) RETURN( RATING_WATER );
    if ( !str_prefix( argument, "wind"      ) ) RETURN( RATING_WIND  );
    break;
  }

  RETURN( ERRORCODE );
}

bool check_bow_content( CHAR_DATA * ch )
{
  PUSH_FUNCTION( "check_bow_content" );
  RETURN( TRUE );
}

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

char * check_obj_value( int type, int loc, int value, int * ret, bool unused )
{
  int              flags;
  OBJ_INDEX_DATA * pObjIndex;

  PUSH_FUNCTION( "check_obj_value" );

  *ret = 0;
  if ( loc < 0 || loc >= MAX_OBJECT_VALUE ) RETURN( "�}�C�W�X�ɽu�T" );

  switch( type )
  {
  default:
    RETURN( "���������A�T" );

  case ITEM_LIGHT:

    switch( loc )
    {
    default:
      if ( unused && value != 0 ) RETURN( "�ƭȥ��ϥΡM�ӫo�]�w���ƭȡT" );
      RETURN( "" );

    case 2:

      if ( value == 0 )
      {
        *ret = 1;
        RETURN( "�OŢ�����~�ө��ɶ��������൥��s�T" );
      }
    }

    break;

  case ITEM_SCROLL:
  case ITEM_POTION:
  case ITEM_PILL:

    switch( loc )
    {
    default:
      if ( unused && value != 0 ) RETURN( "�ƭȥ��ϥΡM�ӫo�]�w���ƭȡT" );
      RETURN( "" );

    case 0:

      if ( value < -10000 || value > 10000 )
      {
        *ret = 0;
        RETURN( "�ī~�����~�v�T���ƭȥ������� -10000 �� 10000 �����T" );
      }
      break;

    case 1:

      if ( value > 0 )
      {
        switch( value )
        {
        default:
          *ret = 0;
          RETURN( "�ī~�����~�v�T���A�ƭȤ��b�w�q���T" );

        case OBJ_CAST_HIT:
        case OBJ_CAST_MANA:
        case OBJ_CAST_MOVE:
        case OBJ_CAST_IDENT:
        case OBJ_CURE_POISON:
        case OBJ_DETECT_INVS:
        case OBJ_CAST_FLY:
        case OBJ_CURE_BLIND:
        case OBJ_GIANT_STR:
        case OBJ_DETECT_HIDE:
        case OBJ_SNEAK:
        case OBJ_DETECT_EVIL:
        case OBJ_CHANGE_SEX:
        case OBJ_DETECT_MAGIC:
        case OBJ_DETECT_POISON:
        case OBJ_FAERIE_FOG:
        case OBJ_GATE:
        case OBJ_FIXITY:
        case OBJ_CAST_ALL:
        case OBJ_DETECT_MASK:
          break;
        }
      }
      break;

    case 2:

      if ( value > 0 && !get_skill( value ) )
      {
        *ret = 0;
        RETURN( "�ī~�����~�ҬI�i���k�N���X���X�z�T" );
      }
      break;

    case 3:

      if ( value < 0 || value > 10000 )
      {
        *ret = 0;
        RETURN( "�ī~�����ľ��q�������� 0 �� 10000 �����T" );
      }
      break;
    }

    break;

  case ITEM_WAND:
  case ITEM_STAFF:

    switch( loc )
    {
    default:
      if ( unused && value != 0 ) RETURN( "�ƭȥ��ϥΡM�ӫo�]�w���ƭȡT" );
      RETURN( "" );

    case 4:
      if ( value > 0 )
      {
        switch( value )
        {
        default:
          *ret = 0;
          RETURN( "�k�����[�j���������X���s�b�T" );

        case RATING_SKILL:
        case RATING_WIND:
        case RATING_EARTH:
        case RATING_LIGHTNING:
        case RATING_POISON:
        case RATING_FIRE:
        case RATING_WATER:
        case RATING_DARKNESS:
        case RATING_LIGHT:
        case RATING_SAINT:
        case RATING_EVIL:
        case RATING_LOST:
        case RATING_CURE:
        case RATING_SING:
        case RATING_FIGHT:
        case RATING_MURDER:
        case RATING_CREATE:
        case RATING_THIEF:
        case RATING_ALL:
          break;
        }
      }
      break;

    case 5:

      if ( value < 0 || value > 1000 )
      {
        *ret = 0;
        RETURN( "�k���[�j�j�ץ������� 0 �� 1000 �����T" );
      }
      break;

    case 6:

      if ( value < 0 || value > MaxMana )
      {
        *ret = 0;
        RETURN( "�k�����̤j�k�O�����j�󵥩�s�T" );
      }

      break;

    case 7:

      if ( value < 0 || value > MaxMana )
      {
        *ret = 0;
        RETURN( "�k�����ثe�k�O�����j�󵥩�s�T" );
      }

      break;
    }

    break;

  case ITEM_WEAPON:

    switch( loc )
    {
    default:
      if ( unused && value != 0 ) RETURN( "�ƭȥ��ϥΡM�ӫo�]�w���ƭȡT" );
      RETURN( "" );

    case 1:
      if ( value <= 0 || value > 1000 )
      {
        *ret = 100;
        RETURN( "�Z���̤p�ˮ`�ȥ����b 0 �� 1000 �����T" );
      }
      break;

    case 2:
      if ( value <= 0 || value > 1000 )
      {
        *ret = 100;
        RETURN( "�Z���̤j�ˮ`�ȥ����b 0 �� 1000 �����T" );
      }
      break;

    case 3:
      switch( value )
      {
      default:
        *ret = WEAPON_HAND;
        RETURN( "�Z���������b���w�q�d�򤺡T" );

      case WEAPON_HAND:
      case WEAPON_DAGGER:
      case WEAPON_SWORD:
      case WEAPON_BLADE:
      case WEAPON_AXE:
      case WEAPON_WHIP:
      case WEAPON_SPEAR:
      case WEAPON_PEN:
      case WEAPON_HAMMER:
      case WEAPON_CLUB:
      case WEAPON_BOW:
      case WEAPON_FORCE:
      case WEAPON_STEPS:
      case WEAPON_FLEE:
        break;
      }
      break;
    }

    break;

  case ITEM_TREASURE:
  case ITEM_FURNITURE:
  case ITEM_KEY:
  case ITEM_TRASH:
  case ITEM_BOAT:
  case ITEM_CORPSE_PC:
  case ITEM_CORPSE_NPC:
  case ITEM_FOUNTAIN:
  case ITEM_SIGIL:
  case ITEM_VELLUM:

    switch( loc )
    {
    default:
      if ( unused && value != 0 ) RETURN( "�ƭȥ��ϥΡM�ӫo�]�w���ƭȡT" );
      RETURN( "" );
    }

    break;

  case ITEM_ARMOR:

    switch( loc )
    {
    default:
      if ( unused && value != 0 ) RETURN( "�ƭȥ��ϥΡM�ӫo�]�w���ƭȡT" );
      RETURN( "" );

    case 0:
      if ( value < -1000 || value > 1000 )
      {
        *ret = 0;
        RETURN( "���㪺���@���ť����b -1000 �� 1000 ����" );
      }
    }

    break;

  case ITEM_CONTAINER:

    switch( loc )
    {
    default:
      if ( unused && value != 0 ) RETURN( "�ƭȥ��ϥΡM�ӫo�]�w���ƭȡT" );
      RETURN( "" );

    case 0:

      if ( value <= 0 || value > 1000 )
      {
        *ret = 1;
        RETURN( "�e�����̤j�e�q�����b 1 �� 1000 �����T" );
      }

      break;

    case 1:
      flags = CONT_CLOSEABLE | CONT_PICKPROOF | CONT_CLOSED | CONT_LOCKED;
      if ( ( value & ~flags ) != 0 )
      {
        *ret = 0;
        RETURN( "�e�������A�X�Ф���T" );
      }
      break;

    case 2:

      if ( value != 0 )
      {
        if ( !( pObjIndex = get_obj_index( value ) ) )
        {
          *ret = 0;
          RETURN( "�e�����_�͸��X���~���s�b�T" );
        }

        if ( pObjIndex->item_type != ITEM_KEY )
        {
          *ret = 0;
          RETURN( "�e���_�ͪ��~���A���O�_�͡T" );
        }
      }

      break;
    }

    break;

  case ITEM_DRINK_CON:

    switch( loc )
    {
    default:
      if ( unused && value != 0 ) RETURN( "�ƭȥ��ϥΡM�ӫo�]�w���ƭȡT" );
      RETURN( "" );

    case 0:

      if ( value <= 0 || value > 1000 )
      {
        *ret = 1;
        RETURN( "�G��e�����̤j�e�q���X�k�T" );
      }

      break;

    case 1:
      if ( value <= 0 || value > 1000 )
      {
        *ret = 1;
        RETURN( "�G��e�����ثe�e�q���X�k�T" );
      }

      break;

    case 2:

      if ( value != 0 && !liq_lookup( value ) )
      {
        *ret = liq_water->slot;
        RETURN( "�G��e�����G�鸹�X�����T�T" );
      }

      break;

    case 3:
      if ( value != TRUE && value != FALSE )
      {
        *ret = FALSE;
        RETURN( "�G��e���O�_���r�ƭȥu��O 0 �� 1�T" );
      }

      break;
    }

    break;

  case ITEM_FOOD:

    switch( loc )
    {
    default:
      if ( unused && value != 0 ) RETURN( "�ƭȥ��ϥΡM�ӫo�]�w���ƭȡT" );
      RETURN( "" );

    case 0:
      if ( value <= 0 || value > 1000 )
      {
        *ret = 1;
        RETURN( "�������G�a�ɶ��������� 1 �� 1000 �����T" );
      }

      break;

    case 1:

      if ( value < 0 || value > 1000 )
      {
        *ret = 1;
        RETURN( "�������{�����i���Φ��ƥ������� 0 �� 1000�T" );
      }

      break;

    case 2:

      if ( value <= 0 || value > 1000 )
      {
        *ret = 1;
        RETURN( "�������@�f���q�������� 1 �� 1000 �����T" );
      }
      break;

    case 3:
      if ( value != TRUE && value != FALSE )
      {
        *ret = FALSE;
        RETURN( "�����O�_���r�ƭȥu��O 0 �� 1�T" );
      }
      break;

    case 4:

      if ( value != 0 && !get_obj_index( value ) )
      {
        *ret = 0;
        RETURN( "�������ͪ����~���X���s�b�T" );
      }

      break;
    }

    break;

  case ITEM_MONEY:

    switch( loc )
    {
    default:
      if ( unused && value != 0 ) RETURN( "�ƭȥ��ϥΡM�ӫo�]�w���ƭȡT" );
      RETURN( "" );

    case 0:
      if ( value < 0 )
      {
        *ret = 0;
        RETURN( "�������ƶq�p��s�T" );
      }
      break;
    }

    break;

  case ITEM_MAGICSTONE:

    switch( loc )
    {
    default:
      if ( unused && value != 0 ) RETURN( "�ƭȥ��ϥΡM�ӫo�]�w���ƭȡT" );
      RETURN( "" );

    case 0:
      break;

    case 1:
      break;

    case 2:
      break;

    case 3:
      break;

    case 4:
      break;

    case 5:
      break;
    }

    break;

  case ITEM_SPIRITJADE:

    switch( loc )
    {
    default:
      if ( unused && value != 0 ) RETURN( "�ƭȥ��ϥΡM�ӫo�]�w���ƭȡT" );
      RETURN( "" );

    case 0:

      if ( value != 0 && !get_mob_index( value ) )
      {
        *ret = 0;
        RETURN( "���ɪ��p�����X���s�b�T" );
      }

      break;

    case 1:
      if ( value < 0 || value > LEVEL_HERO )
      {
        *ret = 0;
        RETURN( "���ɤ��p�����ť����b 0 �� 100 �����T" );
      }
      break;

    case 2:

      if ( value < 0 )
      {
        *ret = 0;
        RETURN( "���ɤ��p���g��ȥ����j�󵥩�s�T" );
      }
      break;

    case 3:

      if ( !get_skill( value ) )
      {
        *ret = 1;
        RETURN( "�p�����ޯู�X���s�b�T" );
      }
      break;

    case 4:

      if ( value <= 0 || value > 100 )
      {
        *ret = 1;
        RETURN( "�p���ޯ઺���m�ץ����b 1 �� 100 �����T" );
      }
      break;
    }

    break;

  case ITEM_MYSTERY:

    switch( loc )
    {
    default:
      if ( unused && value != 0 ) RETURN( "�ƭȥ��ϥΡM�ӫo�]�w���ƭȡT" );
      RETURN( "" );

    case 0:

      if ( !get_skill( value ) )
      {
        *ret = 1;
        RETURN( "���D���ޯู�X���s�b�T" );
      }
      break;

    case 1:

      if ( value <= 0 || value > 100 )
      {
        *ret = 1;
        RETURN( "���D�����m�ץ������� 1 �� 100 �����T" );
      }
      break;
    }

    break;

  case ITEM_LETTER:

    switch( loc )
    {
    default:
      if ( unused && value != 0 ) RETURN( "�ƭȥ��ϥΡM�ӫo�]�w���ƭȡT" );
      RETURN( "" );

    case 0:

      if ( value < 0 )
      {
        *ret = 0;
        RETURN( "�H�󪺳Ѿl�ɶ������j�󵥩�s�T" );
      }
      break;
    }

    break;

  case ITEM_ORE:

    switch( loc )
    {
    default:
      if ( unused && value != 0 ) RETURN( "�ƭȥ��ϥΡM�ӫo�]�w���ƭȡT" );
      RETURN( "" );

    case 0:

      if ( !get_obj_index( value ) )
      {
        *ret = ObjProtypeVnum;
        RETURN( "�q�����y�X���~���X���s�b�T" );
      }

      break;

    case 1:

      if ( value <= 0 || value > LEVEL_HERO )
      {
         *ret = 1;
         RETURN( "�q�����y�̵��ť������� 1 �� 100 �����T" );
      }

      break;

    case 2:

      if ( value <= 0 || value > 10000 )
      {
        *ret = 1;
        RETURN( "�q�����y���\\���v�������� 1 �� 10000�T" );
      }
      break;

    case 3:
      if ( value <= 0 )
      {
        *ret = 1;
        RETURN( "�q�����y���O�Υ����j��s�T" );
      }
      break;
    }

    break;

  case ITEM_GOLDMINE:

    switch( loc )
    {
    default:
      if ( unused && value != 0 ) RETURN( "�ƭȥ��ϥΡM�ӫo�]�w���ƭȡT" );
      RETURN( "" );

    case 0:

      if ( value <= 0 )
      {
        *ret = 1;
        RETURN( "�E�_�֪��̤j�e�q�����j��s�T" );
      }

      break;

    case 1:

      if ( value < 0 )
      {
        *ret = 0;
        RETURN( "�E�_�֪��ֿn�e�q�����j�󵥩�s�T" );
      }
      break;

    case 2:
      if ( value <= 0 || value > MAX_LEVEL )
      {
        *ret = 1;
        RETURN( "�E�_�֥�J���~�̧C���ť������� 1 �� 120 �����T" );
      }
      break;

    case  5:
    case  6:
    case  7:
    case  8:
    case  9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:

      if ( value > 0 && !get_obj_index( value ) )
      {
        *ret = ObjProtypeVnum;
        RETURN( "�E�_�ֲ��ͪ����~���s�b�T" );
      }

      break;
    }

    break;

  case ITEM_FIREWORK:

    switch( loc )
    {
    default:
      if ( unused && value != 0 ) RETURN( "�ƭȥ��ϥΡM�ӫo�]�w���ƭȡT" );
      RETURN( "" );

    case 0:

      if ( value <= 0 )
      {
        *ret = 1;
        RETURN( "�Ϥ��I�񪺶Z�������j��s�T" );
      }
      break;

    case 1:
      if ( value <= 0 || value > 10000 )
      {
        *ret = 1;
        RETURN( "�Ϥ��I�񦨥\\���v�������� 1 �� 10000 �����T" );
      }
      break;
    }

    break;

  case ITEM_URN:

    switch( loc )
    {
    default:
      if ( unused && value != 0 ) RETURN( "�ƭȥ��ϥΡM�ӫo�]�w���ƭȡT" );
      RETURN( "" );

    case 0:
      if ( value > 0 && !get_mob_index( value ) )
      {
        *ret = 0;
        RETURN( "�����̫ʦL���Ǫ����X���s�b�T" );
      }
      break;

    case 1:

      if ( value < 0 || value > MaxHit )
      {
        *ret = 0;
        RETURN( "�����̩Ǫ����ͩR�ȥ����j��s�T" );
      }

      break;

    case 2:

      if ( value < 0 || value > MaxMana )
      {
        *ret = 0;
        RETURN( "�����̩Ǫ����k�O�ƭȥ����j��s�T" );
      }

      break;

    case 3:

      if ( value < 0 || value > MaxMove )
      {
        *ret = 0;
        RETURN( "�����̩Ǫ�����O�ƭȥ����j��s�T" );
      }

      break;
    }

    break;

  case ITEM_BACKBAG:

    switch( loc )
    {
    default:
      if ( unused && value != 0 ) RETURN( "�ƭȥ��ϥΡM�ӫo�]�w���ƭȡT" );
      RETURN( "" );

    case 0:

      if ( value <= 0 )
      {
        *ret = 1;
        RETURN( "�b�U�̳̤j�˸��q�����j��s�T" );
      }
      break;

    case 1:

      if ( value < 0 )
      {
        *ret = 0;
        RETURN( "�b�U�̥ثe�˸��q�����j�󵥩�s�T" );
      }
      break;

    case 2:

      if ( value > 0 )
      {
        if ( !( pObjIndex = get_obj_index( value ) ) )
        {
          *ret = 0;
          RETURN( "�b�U�̪��Z�����X���s�b�T" );
        }
        else if ( pObjIndex->item_type != ITEM_WEAPON )
        {
          *ret = 0;
          RETURN( "�b�U�̸˪����~���O�Z�������F��T" );
        }
      }

      break;
    }

    break;

  case ITEM_ARROW:

    switch( loc )
    {
    default:
      if ( unused && value != 0 ) RETURN( "�ƭȥ��ϥΡM�ӫo�]�w���ƭȡT" );
      RETURN( "" );

    case 0:

      if ( value <= 0 || value > 1000 )
      {
        *ret = 100;
        RETURN( "�}�b�̤j�ˮ`�ȥ����b 0 �� 1000 �����T" );
      }
      break;

    case 1:

      if ( value <= 0 || value > 1000 )
      {
        *ret = 100;
        RETURN( "�}�b�̤p�ˮ`�ȥ����b 0 �� 1000 �����T" );
      }
      break;
    }

    break;
  }

  RETURN( "" );
}

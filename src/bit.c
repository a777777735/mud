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

#ifdef __linux__
#include <linux/tcp.h>
#endif

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "merc.h"

const char * direction_name( int direction )
{
  int    size;
  const char * dir_name [] =
    {  "�_", "�F", "�n", "��", "�W", "�U", "�i�J", "�X�h" };

  PUSH_FUNCTION( "direction_name" );

  size = sizeof( dir_name ) / sizeof( dir_name[0] );
  if ( direction < 0 || direction >= size ) RETURN( "����" );
  RETURN( dir_name[direction] );
}

const char * direction_ename( int direction )
{
  int    size;
  const char * dir_ename [] =
    { "north", "east", "south", "west", "up", "down", "enter", "out" };

  PUSH_FUNCTION( "direction_name" );

  size = sizeof( dir_ename ) / sizeof( dir_ename[0] );
  if ( direction < 0 || direction >= size ) RETURN( "����" );
  RETURN( dir_ename[direction] );
}

const char * get_homename( int sn )
{
  AREA_DATA * pArea;

  PUSH_FUNCTION( "get_homename" );

  for ( pArea = area_first; pArea; pArea = pArea->next )
    if ( pArea->serial == sn ) RETURN( pArea->name );

  RETURN( "����" );
}

const char * status_message( CHAR_DATA * ch )
{
  char * const StatusMessage [] =
  {
     "\e[1;31m�Ⲵ�@�C�M��L�@���M�۫H�ܧִN�i�H�M�Fù���w���F�T\e[0m",
     "\e[1;31m�w�g���˹L���M�a�a�@���M�R�b���i�T\e[0m",
     "\e[1;31m�B��b���g���A�M�H�ɳ��i��L�˩��h�M�����]�}�l�ҽk�T\e[0m",
     "\e[1;31m���˹L���M���p��������M�H�ɦ��i���_��T\e[0m",
     "\e[1;31m���F�۷����ˡM�u�Ȧ��ͩR�M�I�M�ݨӱ��p���ӧ��T\e[0m",

     "\e[1;32m�@���Y���}�����ҼˡM���b�j�j�伵���ˤU�T\e[0m",
     "\e[1;32m�𮧨H���M�ʧ@�}�l���áM�ݨөҨ����˵۹ꤣ���T\e[0m",
     "\e[1;32m�}�l��ݦp���M�U�L��í�M�X�ۤw�����㪺�}��T\e[0m",
     "\e[1;32m���G�Q���h�ΡM�ݨӻݭn�n�n�𮧡T\e[0m",
     "\e[1;32m��ݦS�S�M�ݨӪ��p���Ӧn�M�ݭn�ղz�ղz�T\e[0m",

     "\e[1;33m���ˤ����M�ݤ��ӲM�����p�O�p��T\e[0m",
     "\e[1;33m���F�X�B�p�ˡM�i�O���Ӥ�ê�ơT\e[0m",
     "\e[1;33m�ʧ@���G�}�l���I���F���M���O���M���������T\e[0m",
     "\e[1;33m�R���F���O�M�ݨӬO���X����ӤF�T\e[0m",
     "\e[1;33m���������M��幡���M���B��̨Ϊ��p�T\e[0m"
  };

  int size = sizeof( StatusMessage ) / sizeof( StatusMessage[0] );
  int choice;

  PUSH_FUNCTION( "status_message" );

  choice = UMAX( 0 , UMIN( get_curr_hit( ch ) , ch->hit ) )
              * ( size - 1 ) / ( UMAX( 1, get_curr_hit( ch ) ) );

  RETURN( StatusMessage[choice] );
}

#ifndef TCP_ESTABLISHED
#define TCP_ESTABLISHED         1
#endif

#ifndef TCP_SYN_SENT
#define TCP_SYN_SENT            2
#endif

#ifndef TCP_SYN_RECV
#define TCP_SYN_RECV            3
#endif

#ifndef TCP_FIN_WAIT1
#define TCP_FIN_WAIT1           4
#endif

#ifndef TCP_FIN_WAIT2
#define TCP_FIN_WAIT2           5
#endif

#ifndef TCP_TIME_WAIT
#define TCP_TIME_WAIT           6
#endif

#ifndef TCP_CLOSE
#define TCP_CLOSE               7
#endif

#ifndef TCP_CLOSE_WAIT
#define TCP_CLOSE_WAIT          8
#endif

#ifndef TCP_LAST_ACK
#define TCP_LAST_ACK            9
#endif

#ifndef TCP_LISTEN
#define TCP_LISTEN              10
#endif

#ifndef TCP_CLOSING
#define TCP_CLOSING             11
#endif

const char * tcp_status( int status )
{
  PUSH_FUNCTION( "tcp_status" );

  switch( status )
  {
  default:              RETURN( "TCP_UNKOWN"  );
  case TCP_ESTABLISHED: RETURN( "ESTABLISHED" );
  case TCP_SYN_SENT:    RETURN( "SYN_SENT"    );
  case TCP_SYN_RECV:    RETURN( "SYNC_RECV"   );
  case TCP_FIN_WAIT1:   RETURN( "FIN_WAIT1"   );
  case TCP_FIN_WAIT2:   RETURN( "FIN_WAIT2"   );
  case TCP_TIME_WAIT:   RETURN( "WAIT"        );
  case TCP_CLOSE:       RETURN( "CLOSE"       );
  case TCP_CLOSE_WAIT:  RETURN( "CLOSE_WAIT"  );
  case TCP_LAST_ACK:    RETURN( "LAST_ACK"    );
  case TCP_LISTEN:      RETURN( "LISTEN"      );
  case TCP_CLOSING:     RETURN( "CLOSING"     );
  }
  RETURN( "Unknown" );
}

const char * adeptation_name( int adept )
{
  int     size;
  int     count;
  char *  const adeptation [] =
  {
    "\e[0;37m�@�L�ڰ�\e[0m" ,
    "\e[0;31m��ǥE�m\e[0m" ,
    "\e[0;32m�ʳq�֤�\e[0m" ,
    "\e[0;33m��s���|\e[0m" ,
    "\e[0;34m�������\e[0m" ,
    "\e[0;35m�߯���|\e[0m" ,
    "\e[1;31m�X���޵�\e[0m" ,
    "\e[1;32m���G���\e[0m" ,
    "\e[1;33m�X���J��\e[0m" ,
    "\e[1;34m�n�p�y��\e[0m" ,
    "\e[1;36m�@�N�v�v\e[0m" ,
    "\e[1;37m�`���i��\e[0m"
  };

  PUSH_FUNCTION( "adeptation_name" );

  if ( adept <= 0 ) RETURN( adeptation[0] );

  size  = sizeof( adeptation ) / sizeof( adeptation[0] );
  count = ( adept / 10 ) + 1;

  if ( count < 0 || count >= size ) RETURN( "����" );

  RETURN( adeptation[count] );
}

char * effect_type_name( int type )
{
  PUSH_FUNCTION( "effect_type_name" );

  switch( type )
  {
  default:
  case EFFECT_NONE:                     RETURN( "�����q��" );
  case EFFECT_VICTIM_MANA:              RETURN( "���k�O" );
  case EFFECT_SELF_MANA:                RETURN( "�ۤv�k�O" );
  case EFFECT_VICTIM_MOVE:              RETURN( "�����O" );
  case EFFECT_SELF_MOVE:                RETURN( "�ۤv��O" );
  case EFFECT_VICTIM_BLINDNESS:         RETURN( "��貴��" );
  case EFFECT_VICTIM_CURSE:             RETURN( "���A�G" );
  case EFFECT_VICTIM_POISON:            RETURN( "��褤�r" );
  case EFFECT_VICTIM_SLEEP:             RETURN( "������" );
  case EFFECT_VICTIM_PARA:              RETURN( "���·�" );
  case EFFECT_VICTIM_FAERIE_FIRE:       RETURN( "�����F����" );
  case EFFECT_VICTIM_HIT:               RETURN( "���ͩR" );
  case EFFECT_SELF_HIT:                 RETURN( "�ۤv�ͩR" );
  case EFFECT_HIT_DIE:                  RETURN( "�@������" );
  }
}

char * get_position( int number )
{
  PUSH_FUNCTION( "get_position" );

  switch( number )
  {
  default:           RETURN( "����" );
  case POS_DEAD:     RETURN( "���`" );
  case POS_SLEEPING: RETURN( "��ı" );
  case POS_RESTING:  RETURN( "��" );
  case POS_FIGHTING: RETURN( "�԰�" );
  case POS_STANDING: RETURN( "����" );
  }
}

char * get_log( int number )
{
  PUSH_FUNCTION( "get_log" );

  switch( number )
  {
  default:          RETURN( "����"     );
  case LOG_NORMAL:  RETURN( "���`�аO" );
  case LOG_ALWAYS:  RETURN( "�û��аO" );
  case LOG_NEVER :  RETURN( "�ä��аO" );
  case LOG_WIZ   :  RETURN( "���ڼаO" );
  }
}

char * sex_name( int sex )
{
  PUSH_FUNCTION( "sex_name" );
  switch( sex )
  {
  case SEX_MALE:    RETURN( "�k��" );
  case SEX_FEMALE:  RETURN( "�k��" );
  case SEX_NEUTRAL: RETURN( "����" );
  default:          RETURN( "����" );
  }
}

char * obj_cast_location( int location )
{
  PUSH_FUNCTION( "obj_cast_location" );

  switch( location )
  {
  default:                RETURN( "�����q��" );
  case OBJ_CAST_HIT:      RETURN( "�v�T�ͩR" );
  case OBJ_CAST_MANA:     RETURN( "�v�T�k�O" );
  case OBJ_CAST_MOVE:     RETURN( "�v�T��O" );
  case OBJ_CAST_IDENT:    RETURN( "Ų�w�N"   );
  case OBJ_CURE_POISON:   RETURN( "�Ѭr�N"   );
  case OBJ_DETECT_INVS:   RETURN( "��������" );
  case OBJ_DETECT_MASK:   RETURN( "�������e" );
  case OBJ_CAST_FLY:      RETURN( "����N"   );
  case OBJ_CURE_BLIND:    RETURN( "�v������" );
  case OBJ_GIANT_STR:     RETURN( "���H���O" );
  case OBJ_DETECT_HIDE:   RETURN( "�����ð�" );
  case OBJ_SNEAK:         RETURN( "���"     );
  case OBJ_DETECT_EVIL:   RETURN( "�������c" );
  case OBJ_CHANGE_SEX:    RETURN( "�ܩʳN"   );
  case OBJ_DETECT_MAGIC:  RETURN( "�����]�k" );
  case OBJ_DETECT_POISON: RETURN( "�����r��" );
  case OBJ_FAERIE_FOG:    RETURN( "�{���N"   );
  case OBJ_GATE:          RETURN( "�۰��N"   );
  case OBJ_FIXITY:        RETURN( "�w���N"   );
  case OBJ_CAST_ALL:      RETURN( "�^�_�Ҧ�" );
  }
}

const char * bounty_type( int type )
{
  PUSH_FUNCTION( "bounty_type" );

  switch( type )
  {
  default:            RETURN( "����" );
  case BOUNTY_GOLD:   RETURN( "����" );
  case BOUNTY_FIRMAN: RETURN( "�K�����P" );
  }

  RETURN( "����" );
}

const char * bounty_unit( int type )
{
  PUSH_FUNCTION( "bounty_unit" );

  switch( type )
  {
  default:            RETURN( ""   );
  case BOUNTY_GOLD:   RETURN( "��" );
  case BOUNTY_FIRMAN: RETURN( "�i" );
  }

  RETURN( "" );
}
char * message_type( int type )
{
  PUSH_FUNCTION( "message_type" );

  switch( type )
  {
  default:                 RETURN( "(����)"         );
  case ACT_WHEN_EAT:       RETURN( "�Y���ʧ@"       );
  case ACT_WHEN_WEAR:      RETURN( "�諸�ʧ@"       );
  case ACT_WHEN_DROP:      RETURN( "�᪺�ʧ@"       );
  case ACT_WHEN_REMOVE:    RETURN( "�������ʧ@"     );
  case ACT_WHEN_SACRIFICE: RETURN( "�^�m���ʧ@"     );
  case ACT_WHEN_GET:       RETURN( "�߰_���ʧ@"     );
  case ACT_WHEN_PUT:       RETURN( "��J���ʧ@"     );
  case ACT_WHEN_GIVE:      RETURN( "�ػP���ʧ@"     );
  case ACT_WHEN_FILL:      RETURN( "�˶񪺰ʧ@"     );
  case ACT_WHEN_DRINK:     RETURN( "�ܪ��ʧ@"       );
  case ACT_WHEN_RECITE:    RETURN( "���w���ʧ@"     );
  case ACT_WHEN_BUY:       RETURN( "�R���ʧ@"       );
  case ACT_WHEN_SELL:      RETURN( "�檺�ʧ@"       );
  case ACT_WHEN_VALUE:     RETURN( "�������ʧ@"     );
  case ACT_WHEN_LOOK:      RETURN( "�[�ݪ��ʧ@"     );
  case ACT_WHEN_COMPARE:   RETURN( "������ʧ@"     );
  case ACT_WHEN_SEND:      RETURN( "�l�H���ʧ@"     );
  case ACT_WHEN_OPEN:      RETURN( "���}���ʧ@"     );
  case ACT_WHEN_CLOSE:     RETURN( "���W���ʧ@"     );
  case ACT_WHEN_LOCK:      RETURN( "��W���ʧ@"     );
  case ACT_WHEN_UNLOCK:    RETURN( "�}�ꪺ�ʧ@"     );
  case ACT_WHEN_AUCTION:   RETURN( "��檺�ʧ@"     );
  case ACT_WHEN_BET:       RETURN( "�ۻ����ʧ@"     );
  case ACT_WHEN_REPAIR:    RETURN( "�ײz���ʧ@"     );
  case ACT_WHEN_DEPOSIT:   RETURN( "�H�񪺰ʧ@"     );
  case ACT_WHEN_WITHDRAW:  RETURN( "��^���ʧ@"     );
  case ACT_WHEN_CACHET:    RETURN( "�ˤW�]�۪��ʧ@" );
  case ACT_WHEN_SLUP:      RETURN( "���U�]�۪��ʧ@" );
  }
}

char * restrict_value( RESTRICT_DATA * pRestrict, CHAR_DATA * ch )
{
  CLASS_DATA * pClass;
  SKILL_DATA * pSkill;
  static char  buf[512];
  char         chinese[512];

  PUSH_FUNCTION( "restrict_value" );

  if ( !pRestrict || !ch ) RETURN( "(�ŵ��c)" );

  switch( pRestrict->type )
  {
  case RES_STR:
    chinese_number( pRestrict->value, chinese );
    sprintf( buf, "����O�q����C��%s�C", chinese );
    break;

  case RES_INT:
    chinese_number( pRestrict->value, chinese );
    sprintf( buf, "����O����C��%s�C", chinese );
    break;

  case RES_WIS:
    chinese_number( pRestrict->value, chinese );
    sprintf( buf, "������Ѥ���C��%s�C", chinese );
    break;

  case RES_DEX:
    chinese_number( pRestrict->value, chinese );
    sprintf( buf, "����ӱ�����C��%s�C", chinese );
    break;

  case RES_CON:
    chinese_number( pRestrict->value, chinese );
    sprintf( buf, "������椣��C��%s�C", chinese );
    break;

  case RES_HP:
    chinese_number( pRestrict->value, chinese );
    sprintf( buf, "����ͩR�O����C��%s�C", chinese );
    break;

  case RES_MANA:
    chinese_number( pRestrict->value, chinese );
    sprintf( buf, "����k�O����C��%s�C", chinese );
    break;

  case RES_MOVE:
    chinese_number( pRestrict->value, chinese );
    sprintf( buf, "������O����C��%s�C", chinese );
    break;

  case RES_CLASS:
    str_cpy( buf, "����¾�~���" );

    for ( pClass = class_list; pClass; pClass = pClass->next )
    {
      if ( pRestrict->value & pClass->mask )
      {
        str_cat( buf, " (" );
        str_cat( buf, pClass->cname );
        str_cat( buf, ")" );
      }
    }

    break;

  case RES_TRUST:

    chinese_number( pRestrict->value, chinese );
    sprintf( buf, "����H���Ȥ���C��%s�C", chinese );
    break;

  case RES_SKILL:

    if ( !( pSkill = get_skill( pRestrict->value ) ) ) RETURN( "����" );

    chinese_number( pRestrict->vicevalue, chinese );
    sprintf( buf, "����ޯ�%s���m�פ���C��%s%s%s%s�C"
      , pSkill->cname, adeptation_name( pRestrict->vicevalue )
      , SkillValue || IS_IMMORTAL( ch ) ? "�u���m�סR" : ""
      , SkillValue || IS_IMMORTAL( ch ) ? chinese      : ""
      , SkillValue || IS_IMMORTAL( ch ) ? "�v"         : "" );

    break;

  case RES_SEX:

    switch( pRestrict->value )
    {
    default:          send_to_buffer( "����ʧO���ԡC"   ); break;
    case SEX_NEUTRAL: send_to_buffer( "����ʧO�����ʡC" ); break;
    case SEX_MALE:    send_to_buffer( "����ʧO���k�ʡC" ); break;
    case SEX_FEMALE:  send_to_buffer( "����ʧO���k�ʡC" ); break;
    }

    break;

  case RES_LEVEL:
    chinese_number( pRestrict->value, chinese );
    sprintf( buf, "���Ť���C��%s�C", chinese );
    break;

  case RES_NOSKILL:

    if ( !( pSkill = get_skill( pRestrict->value ) ) ) RETURN( "����" );

    chinese_number( pRestrict->vicevalue, chinese );
    sprintf( buf, "����۫g�ޯ�%s���m���ప��%s%s%s%s�C"
      , pSkill->cname, adeptation_name( pRestrict->vicevalue )
      , SkillValue || IS_IMMORTAL( ch ) ? "�u���m�סR" : ""
      , SkillValue || IS_IMMORTAL( ch ) ? chinese      : ""
      , SkillValue || IS_IMMORTAL( ch ) ? "�v"         : "" );

    break;

  case RES_ALIGN:

    chinese_number( pRestrict->value, chinese );
    sprintf( buf, "�}�祲��%s%s�C"
      , pRestrict->vicevalue ? "����" : "�C��", chinese );
    break;

  default:
    str_cpy( buf, "����" );
    break;
  }

  RETURN( buf );
}

char * ship_status( SHIP_DATA * pShip )
{
  static char buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "ship_status" );

  if ( !pShip )
  {
    mudlog( LOG_DEBUG, "ship_status: �ӷ������T." );
    RETURN( "����" );
  }

  if ( pShip->sailing_tick > 0 )
  {
    sprintf( buf, "\e[1;32m���\e[0m�M%2d�p�ɫ��F", pShip->sailing_tick );
    RETURN( buf );
  }

  if ( pShip->waiting_tick > 0 )
  {
    sprintf( buf, "\e[1;33m�n��\e[0m�M%2d�p�ɫ�X�o", pShip->waiting_tick );
    RETURN( buf );
  }

  RETURN( "���A����" );
}

char * player_status( int vector )
{
  PUSH_FUNCTION( "player_status" );

  switch( vector )
  {
  case CON_PLAYING              : RETURN( "���b�C����" );
  case CON_GET_FQDN             : RETURN( "���oFQDN��" );
  case CON_GET_FINGER           : RETURN( "�d���a���" );
  case CON_GET_NAME             : RETURN( "��J�m�W��" );
  case CON_CHECK_FILE           : RETURN( "�ˬd�ɮפ�" );
  case CON_VERIFY               : RETURN( "�T�{�ɮפ�" );
  case CON_GET_OLD_PASSWORD     : RETURN( "��J�±K�X" );
  case CON_CONFIRM_NEW_NAME     : RETURN( "�T�{�s�W��" );
  case CON_GET_NEW_PASSWORD     : RETURN( "��J�s�K�X" );
  case CON_CONFIRM_NEW_PASSWORD : RETURN( "�T�{�s�K�X" );
  case CON_GET_SEX              : RETURN( "��J�ʧO��" );
  case CON_READ_MOTD            : RETURN( "�i���e����" );
  case CON_GET_CNAME            : RETURN( "��J����W" );
  case CON_CHOOSE_SKILL         : RETURN( "����ޯत" );
  case CON_GET_EMAIL_ADDRESS    : RETURN( "�]�q�l�H�c" );
  case CON_CHOOSE_CLASS         : RETURN( "���¾�~��" );
  case CON_CHOOSE_MODE          : RETURN( "���X�Ҧ�" );
  case CON_CHOOSE_ANGEL         : RETURN( "��ܦu�@��" );
  case CON_CHOOSE_HOME          : RETURN( "��ܥX�ͦa" );
  case CON_CONTRACT             : RETURN( "ñ�X��"     );
  }

  RETURN( "���������O" );
}

char * skill_type( int vector )
{
  PUSH_FUNCTION( "skill_type" );

  switch( vector )
  {
  case TAR_IGNORE:         RETURN( "����"     );
  case TAR_CHAR_OFFENSIVE: RETURN( "������"   );
  case TAR_CHAR_DEFENSIVE: RETURN( "���m��"   );
  case TAR_CHAR_SELF:      RETURN( "����"     );
  case TAR_OBJ_INV:        RETURN( "���~��"   );
  case TAR_DODGE:          RETURN( "�{����"   );
  case TAR_STRING:         RETURN( "����"     );
  case TAR_ARGUMENT:       RETURN( "�ݭn�r��" );
  case TAR_FLEE:           RETURN( "�k�]��"   );
  case TAR_MEDITATION:     RETURN( "�߷Q��"   );
  case TAR_OBJ_ROOM:       RETURN( "�ж����~" );
  case TAR_MOUNT:          RETURN( "�M�N"     );
  case TAR_NO_CAST:        RETURN( "����"     );
  }

  RETURN( "����" );
}

char * skill_rating( int group )
{
  PUSH_FUNCTION( "skill_rating" );

  switch( group )
  {
  case RATING_SKILL:     RETURN( "�ޯ�"   );
  case RATING_WIND:      RETURN( "���t"   );
  case RATING_EARTH:     RETURN( "�g�t"   );
  case RATING_LIGHTNING: RETURN( "�p�q�t" );
  case RATING_POISON:    RETURN( "�r�t"   );
  case RATING_FIRE:      RETURN( "���t"   );
  case RATING_WATER:     RETURN( "���t"   );
  case RATING_DARKNESS:  RETURN( "�t�t"   );
  case RATING_LIGHT:     RETURN( "���t"   );
  case RATING_SAINT:     RETURN( "�t�t"   );
  case RATING_EVIL:      RETURN( "���t"   );
  case RATING_LOST:      RETURN( "�k�v�t" );
  case RATING_CURE:      RETURN( "�����t" );
  case RATING_SING:      RETURN( "�u�ۨt" );
  case RATING_FIGHT:     RETURN( "�氫�t" );
  case RATING_MURDER:    RETURN( "�t���t" );
  case RATING_CREATE:    RETURN( "ű�y�t" );
  case RATING_THIEF:     RETURN( "�s��t" );
  case RATING_ALL:       RETURN( "����"   );
  }

  RETURN( "����" );
}

/* �������A */
char * club_status( CLUB_DATA * pClub )
{
  int type;

  PUSH_FUNCTION( "club_status" );

  if ( pClub )
  {
    type = pClub->status;
    if ( type == CLUB_STATUS_COUNTERSIGN ) RETURN( "�s�p��"   );
    if ( type == CLUB_STATUS_UNIONIZE    ) RETURN( "�w����"   );
    if ( type == CLUB_STATUS_UNKNOW      ) RETURN( "���A����" );
  }

  RETURN( "����" );
}

char * shop_type_name( int iType )
{
  PUSH_FUNCTION( "shop_type_name" );

  switch( iType )
  {
  case SHOP_STORE:     RETURN( "�ө�"   );
  case SHOP_SMITH:     RETURN( "�K�Q"   );
  case SHOP_MERCENARY: RETURN( "�ħL��" );
  case SHOP_COPER:     RETURN( "����"   );
  default:             RETURN( "����"   );
  }
}

char * item_type_name( OBJ_INDEX_DATA * obj )
{
  PUSH_FUNCTION( "item_type_name" );
  RETURN( item_kind_name( obj->item_type ) );
}

char * item_kind_name( int iType )
{
  PUSH_FUNCTION( "item_kind_name" );

  switch ( iType )
  {
  case ITEM_LIGHT:      RETURN( "�ө���"     );
  case ITEM_SCROLL:     RETURN( "���b"       );
  case ITEM_WAND:       RETURN( "�k��"       );
  case ITEM_STAFF:      RETURN( "�k��"       );
  case ITEM_WEAPON:     RETURN( "�Z��"       );
  case ITEM_TREASURE:   RETURN( "�_��"       );
  case ITEM_ARMOR:      RETURN( "����"       );
  case ITEM_BACKBAG:    RETURN( "�I�U"       );
  case ITEM_POTION:     RETURN( "�Ĥ�"       );
  case ITEM_FURNITURE:  RETURN( "�a��"       );
  case ITEM_TRASH:      RETURN( "�U��"       );
  case ITEM_CONTAINER:  RETURN( "�e��"       );
  case ITEM_DRINK_CON:  RETURN( "�ˤ��e��"   );
  case ITEM_KEY:        RETURN( "�_��"       );
  case ITEM_FOOD:       RETURN( "����"       );
  case ITEM_MONEY:      RETURN( "����"       );
  case ITEM_BOAT:       RETURN( "��"         );
  case ITEM_CORPSE_NPC: RETURN( "�Ǫ�������" );
  case ITEM_CORPSE_PC:  RETURN( "���a������" );
  case ITEM_FOUNTAIN:   RETURN( "�u��"       );
  case ITEM_PILL:       RETURN( "�ĤY"       );
  case ITEM_MAGICSTONE: RETURN( "�]��"       );
  case ITEM_SPIRITJADE: RETURN( "����"       );
  case ITEM_MYSTERY:    RETURN( "���D"       );
  case ITEM_LETTER:     RETURN( "�ѫH"       );
  case ITEM_ORE:        RETURN( "�q��"       );
  case ITEM_GOLDMINE:   RETURN( "�E�_��"     );
  case ITEM_SIGIL:      RETURN( "�ũG"       );
  case ITEM_VELLUM:     RETURN( "�ϥ֯�"     );
  case ITEM_FIREWORK:   RETURN( "�Ϥ�"       );
  case ITEM_URN:        RETURN( "����"       );
  case ITEM_ARROW:      RETURN( "�b"         );
  }

  mudlog( LOG_DEBUG , "Item_type_name: ���������A %d.", iType );
  RETURN( "(����)");
}

char * affect_loc_name( int location )
{
  PUSH_FUNCTION( "affect_loc_name" );

  switch ( location )
  {
  case APPLY_NONE:          RETURN( "�L" );
  case APPLY_STR:           RETURN( "�O�q" );
  case APPLY_DEX:           RETURN( "�ӱ�" );
  case APPLY_INT:           RETURN( "���O" );
  case APPLY_WIS:           RETURN( "����" );
  case APPLY_CON:           RETURN( "���" );
  case APPLY_SEX:           RETURN( "�ʧO" );
  case APPLY_CLASS:         RETURN( "¾�~" );
  case APPLY_LEVEL:         RETURN( "����" );
  case APPLY_AGE:           RETURN( "�~��" );
  case APPLY_MANA:          RETURN( "�k�O" );
  case APPLY_HIT:           RETURN( "�ͩR�O" );
  case APPLY_MOVE:          RETURN( "��O" );
  case APPLY_GOLD:          RETURN( "����" );
  case APPLY_EXP:           RETURN( "�g���" );
  case APPLY_AC:            RETURN( "���@����" );
  case APPLY_HITROLL:       RETURN( "�R���v" );
  case APPLY_DAMROLL:       RETURN( "�ˮ`�I��" );
  case APPLY_SAVING_PARA:   RETURN( "save vs paralysis" );
  case APPLY_SAVING_ROD:    RETURN( "save vs rod" );
  case APPLY_SAVING_PETRI:  RETURN( "save vs petrification" );
  case APPLY_SAVING_BREATH: RETURN( "save vs breath" );
  case APPLY_SAVING_SPELL:  RETURN( "�k�N�ȯ�" );
  }

  mudlog( LOG_DEBUG , "Affect_location_name: �����a�� %d.", location );
  RETURN( "(����)" );
}

char * skill_affect( int type )
{
  PUSH_FUNCTION( "skill_affect" );

  switch( type )
  {
  default:                RETURN( "�S��"     );
  case SLOT_BLINDNESS:    RETURN( "����"     );
  case SLOT_CHARM_PERSON: RETURN( "�g�b"     );
  case SLOT_CURSE:        RETURN( "�A�G"     );
  case SLOT_INVIS:        RETURN( "����"     );
  case SLOT_MASS_INVIS:   RETURN( "��������" );
  case SLOT_POISON:       RETURN( "���r"     );
  case SLOT_SLEEP:        RETURN( "����"     );
  case SLOT_SNEAK:        RETURN( "���"     );
  }
}

const char * exit_status( EXIT_DATA * pExit )
{
  static char buf[512];

  PUSH_FUNCTION( "exit_status" );

  buf[0] = '\x0';
  if ( pExit->closed ) str_cat( buf, " ����"   );
  if ( pExit->locked ) str_cat( buf, " ���"   );
  if ( pExit->pick   ) str_cat( buf, " �i���}" );
  if ( pExit->isdoor ) str_cat( buf, " ����"   );

  RETURN( ( buf[0] != '\x0' )  ? buf+1 : "(�S��)" );
}

char * area_bit_name( AREA_DATA * pArea )
{
  static char buf[512];

  PUSH_FUNCTION( "area_bit_name" );

  buf[0] = '\x0';
  if ( pArea->pk      ) str_cat( buf , " �ޢ�"   );
  if ( pArea->newhand ) str_cat( buf , " �s���" );
  if ( pArea->fight   ) str_cat( buf , " �T�ϰ�" );

  RETURN( buf[0] != '\x0' ? buf+1 : "(�S��)" );
}

char * sector_flag_name( SECTOR_DATA * pSector )
{
  static char buf[512];

  PUSH_FUNCTION( "sector_flag_name" );

  buf[0] = '\x0';
  if ( pSector->fly  ) str_cat( buf , " ����" );
  if ( pSector->swim ) str_cat( buf , " ��a" );
  RETURN( buf[0] != '\x0' ? buf+1 : "(�S��)" );
}

/* �ж����A������W�� */
char * room_bit_name( ROOM_INDEX_DATA * pRoom )
{
  static char buf[512];

  PUSH_FUNCTION( "room_bit_name" );

  buf[0] = '\x0';
  if ( pRoom->player        ) str_cat( buf, " ���a�ۻs" );
  if ( pRoom->Dark          ) str_cat( buf, " �·t"     );
  if ( pRoom->NoMob         ) str_cat( buf, " �S���Ǫ�" );
  if ( pRoom->Indoors       ) str_cat( buf, " �Ǥ�"     );
  if ( pRoom->Private       ) str_cat( buf, " �p�H"     );
  if ( pRoom->Safe          ) str_cat( buf, " �w��"     );
  if ( pRoom->NoRecall      ) str_cat( buf, " ����ǰe" );
  if ( pRoom->ForeverLight  ) str_cat( buf, " �é�"     );
  if ( pRoom->DepositMoney  ) str_cat( buf, " ����"     );
  if ( pRoom->StoreRoom     ) str_cat( buf, " ��"     );
  if ( pRoom->Club          ) str_cat( buf, " ���|"     );
  if ( pRoom->Memorize      ) str_cat( buf, " �O��"     );
  if ( pRoom->Stock         ) str_cat( buf, " ���ک�"   );
  if ( pRoom->NoQuit        ) str_cat( buf, " �������u" );
  if ( pRoom->Sail          ) str_cat( buf, " �"     );
  if ( pRoom->Fane          ) str_cat( buf, " �x�q"     );
  if ( pRoom->Killer        ) str_cat( buf, " �K�P����" );
  if ( pRoom->Standard      ) str_cat( buf, " �зǩж�" );
  if ( pRoom->NoFight       ) str_cat( buf, " ����԰�" );
  if ( pRoom->NoLink        ) str_cat( buf, " ����s��" );
  if ( pRoom->Waterful      ) str_cat( buf, " �h��"     );
  if ( pRoom->Foodful       ) str_cat( buf, " �h����"   );
  if ( pRoom->ManaCure      ) str_cat( buf, " �k�O�E����"   );
  if ( pRoom->MoveCure      ) str_cat( buf, " ��O�E����"   );
  if ( pRoom->HitCure       ) str_cat( buf, " �ͩR�O�E����" );
  if ( pRoom->PracRoom      ) str_cat( buf, " �m�\\��"   );

  RETURN( ( buf[0] != '\x0' ) ? buf+1 : "(�S��)" );
}

char * attack_situs( int flag )
{
  static char   buf[512];
  SITUS_DATA  * pSitus;

  PUSH_FUNCTION( "attack_situs" );

  buf[0] = '\x0';

  for ( pSitus = situs_list; pSitus; pSitus = pSitus->next )
  {
    if ( flag & pSitus->attack )
    {
      str_cat( buf, " " );
      str_cat( buf, pSitus->situs_name );
    }
  }

  RETURN( ( buf[0] ) ? buf+1 : "�H��" );
}

/* �����a�誺�W�� */
char * wear_location_string( int type )
{
  SITUS_DATA * pSitus;

  PUSH_FUNCTION( "wear_location_string" );

  if ( type == WEAR_NONE ) RETURN( "�|���˳�" );

  for ( pSitus = situs_list; pSitus; pSitus = pSitus->next )
    if ( pSitus->location == type ) RETURN( pSitus->situs_name );

  RETURN( "����" );
}

/* �����X�Ъ��W�� */
char * wear_bit_name( int wear_flags )
{
  static char   buf[512];
  SITUS_DATA  * pSitus;

  PUSH_FUNCTION( "wear_bit_name" );

  buf[0] = '\x0';

  for ( pSitus = situs_list; pSitus; pSitus = pSitus->next )
  {
    if ( wear_flags & pSitus->wear )
    {
      str_cat( buf, " " );
      str_cat( buf, pSitus->situs_name );
    }
  }

  RETURN( ( buf[0] != '\x0' ) ? buf+1 : "(�S��)" );
}

char * xnames_bit_name( int type )
{
  PUSH_FUNCTION( "xnames_bit_name" );

  switch ( type )
  {
  case XNAMES_SYSTEM  : RETURN( "�t�ΫO�d�r"     );
  case XNAMES_CACONYM : RETURN( "�����r"         );
  case XNAMES_CHAT    : RETURN( "����O�d�r"     );
  case XNAMES_CNAME   : RETURN( "����W�٫O�d�r" );
  }

  RETURN( "����" );
}

char * ban_bit_name( int type )
{
  PUSH_FUNCTION( "ban_bit_name" );

  switch ( type )
  {
  default         : RETURN( "����"                   );
  case BAN_LOGIN  : RETURN( "�T��s�u"               );
  case BAN_FQDN   : RETURN( "������}�Ϭd��"         );
  case BAN_FINGER : RETURN( "�T��d�߳o��}���a���" );
  }
}

/* ���~�B�~�X�Ъ��W�� */
char * extra_bit_name( OBJ_DATA * pObj )
{
  static char buf[512];

  PUSH_FUNCTION( "extra_bit_name" );

  buf[0] = '\x0';

  if ( pObj->Glow         ) str_cat( buf, " �{�{�o��"   );
  if ( pObj->Hum          ) str_cat( buf, " ���@�T"   );
  if ( pObj->Dark         ) str_cat( buf, " �·t"       );
  if ( pObj->Evil         ) str_cat( buf, " ���c"       );
  if ( pObj->Invis        ) str_cat( buf, " ����"       );
  if ( pObj->Magic        ) str_cat( buf, " �]�k"       );
  if ( pObj->NoDrop       ) str_cat( buf, " ���i���"   );
  if ( pObj->Bless        ) str_cat( buf, " ����"       );
  if ( pObj->AntiGood     ) str_cat( buf, " �ܵ��}�}��" );
  if ( pObj->AntiEvil     ) str_cat( buf, " �ܨ��c�}��" );
  if ( pObj->AntiNeutral  ) str_cat( buf, " �ܤ��߰}��" );
  if ( pObj->NoRemove     ) str_cat( buf, " ���i����"   );
  if ( pObj->Inventory    ) str_cat( buf, " �ӫ~"       );
  if ( pObj->Cancachet    ) str_cat( buf, " �i�ʦL"     );
  if ( pObj->UserSet      ) str_cat( buf, " �۳]�ƭ�"   );
  if ( pObj->NoSave       ) str_cat( buf, " ���i�x�s"   );
  if ( pObj->Contraband   ) str_cat( buf, " �H�T�~"     );
  if ( pObj->Loadable     ) str_cat( buf, " �i�˶�"     );

  RETURN( ( buf[0] != '\x0' ) ? buf+1 : "(�S��)" );
}

/* ���~�B�~�X�Ъ��W�� */
char * extra_bit_name_2( OBJ_INDEX_DATA * pObjIndex )
{
  static char buf[512];

  PUSH_FUNCTION( "extra_bit_name_2" );

  buf[0] = '\x0';

  if ( pObjIndex->Glow         ) str_cat( buf, " �{�{�o��"   );
  if ( pObjIndex->Hum          ) str_cat( buf, " ���@�T"   );
  if ( pObjIndex->Dark         ) str_cat( buf, " �·t"       );
  if ( pObjIndex->Evil         ) str_cat( buf, " ���c"       );
  if ( pObjIndex->Invis        ) str_cat( buf, " ����"       );
  if ( pObjIndex->Magic        ) str_cat( buf, " �]�k"       );
  if ( pObjIndex->NoDrop       ) str_cat( buf, " ���i���"   );
  if ( pObjIndex->Bless        ) str_cat( buf, " ����"       );
  if ( pObjIndex->AntiGood     ) str_cat( buf, " �ܵ��}�}��" );
  if ( pObjIndex->AntiEvil     ) str_cat( buf, " �ܨ��c�}��" );
  if ( pObjIndex->AntiNeutral  ) str_cat( buf, " �ܤ��߰}��" );
  if ( pObjIndex->NoRemove     ) str_cat( buf, " ���i����"   );
  if ( pObjIndex->Inventory    ) str_cat( buf, " �ӫ~"       );
  if ( pObjIndex->Cancachet    ) str_cat( buf, " �i�ʦL"     );
  if ( pObjIndex->UserSet      ) str_cat( buf, " �۳]�ƭ�"   );
  if ( pObjIndex->NoSave       ) str_cat( buf, " ���i�x�s"   );
  if ( pObjIndex->Contraband   ) str_cat( buf, " �H�T�~"     );
  if ( pObjIndex->Loadable     ) str_cat( buf, " �i�˶�"     );

  RETURN( ( buf[0] != '\x0' ) ? buf+1 : "(�S��)" );
}

/* �Z�����A������W�� */
char * weapon_type_name( int value )
{
  PUSH_FUNCTION( "weapon_type_name" );

  switch ( value )
  {
  default : RETURN( "\e[1;31m�Ť�\e[0m" );
  case  0 : RETURN( "�L�]�w" );
  case  1 : RETURN( "���x"   );
  case  2 : RETURN( "�u�L"   );
  case  3 : RETURN( "���C"   );
  case  4 : RETURN( "�j�M"   );
  case  5 : RETURN( "��"     );
  case  6 : RETURN( "�@"     );
  case  7 : RETURN( "�j��"   );
  case  8 : RETURN( "����"   );
  case  9 : RETURN( "��"     );
  case 10 : RETURN( "�Ҵ�"   );
  case 11 : RETURN( "�}"     );
  case 12 : RETURN( "��"     );
  case 13 : RETURN( "�B�k"   );
  case 14 : RETURN( "�k�]"   );
  }
}

/* �e�����X�ЦW�� */
char * container_bit_name( int value )
{
  static char buf[512];

  PUSH_FUNCTION( "container_bit_name" );

  buf[0] = '\x0';
  if ( value & CONT_CLOSEABLE ) str_cat( buf , " �i�H�}��" );
  if ( value & CONT_PICKPROOF ) str_cat( buf , " �L�k���}" );
  if ( value & CONT_CLOSED    ) str_cat( buf , " ����"     );
  if ( value & CONT_LOCKED    ) str_cat( buf , " ���"     );
  RETURN( ( buf[0] != '\x0' ) ? buf+1 : "(�S��)" );
}

char * turn_bit_name( int turn )
{
  static char buf[512];

  PUSH_FUNCTION( "turn_bit_name" );
  buf[0] = '\x0';

  if ( turn & PLR_REBIRTH  ) str_cat( buf, " ��¾��"   );
  if ( turn & PLR_TRAIN    ) str_cat( buf, " �ɯ��ݩ�" );
  if ( turn & PLR_PRACTICE ) str_cat( buf, " �V�m�ޯ�" );
  if ( turn & PLR_ANSI     ) str_cat( buf, " �L�o��m" );
  if ( turn & PLR_LOTTO    ) str_cat( buf, " �ֳz�ֳ�" );
  RETURN( ( buf[0] ) ? buf+1 : "(�S��)" );
}


/* ���ڥN�z�v */
char * wiz_bit_name( int wizflags )
{
  static char buf[512];

  PUSH_FUNCTION( "wiz_bit_name" );
  buf[0] = '\x0';

  if ( wizflags == 0 )          str_cat( buf, " �S�����ڥN�z�v" );

  if ( wizflags & WIZ_SILENCE ) str_cat( buf, " �T��"       );
  if ( wizflags & WIZ_NOTELL  ) str_cat( buf, " ����i�D"   );
  if ( wizflags & WIZ_NOEMOTE ) str_cat( buf, " ����Ϊ���" );

  RETURN( ( buf[0] ) ? buf + 1 : "(�S��) " );
}

/* �H���ݩʦW�� */
char * act_bit_name( int act )
{
  PUSH_FUNCTION( "act_bit_name" );

  clear_stack();

  if ( ( act & PLR_IS_NPC ) == 0 )
  {
    send_to_stack( "���a" );
    if ( act & PLR_AUTOEXIT     ) send_to_stack( " �۰ʥX�f"     );
    if ( act & PLR_AUTOSAC      ) send_to_stack( " �۰ʩ^�m"     );
    if ( act & PLR_BLANK        ) send_to_stack( " ���ťզC"     );
    if ( act & PLR_BRIEF        ) send_to_stack( " ²�u�y�z"     );
    if ( act & PLR_COMBINE      ) send_to_stack( " �զX"         );
    if ( act & PLR_PROMPT       ) send_to_stack( " ���ܦC"       );
    if ( act & PLR_HOLYLIGHT    ) send_to_stack( " �t��"         );
    if ( act & PLR_WIZINVIS     ) send_to_stack( " ��������"     );
    if ( act & PLR_SILENCE      ) send_to_stack( " ����"         );
    if ( act & PLR_NO_EMOTE     ) send_to_stack( " ����emote"    );
    if ( act & PLR_NO_TELL      ) send_to_stack( " ����tell"     );
    if ( act & PLR_LOG          ) send_to_stack( " �Qlog"        );
    if ( act & PLR_FREEZE       ) send_to_stack( " �Qfreeze"     );
    if ( act & PLR_THIEF        ) send_to_stack( " �s��"         );
    if ( act & PLR_KILLER       ) send_to_stack( " ���H��"       );
    if ( act & PLR_BOLTER       ) send_to_stack( " �q����"       );
    if ( act & PLR_EXACT        ) send_to_stack( " ����W��"     );
    if ( act & PLR_MESSAGE      ) send_to_stack( " �����ԭz"     );
    if ( act & PLR_FLEE         ) send_to_stack( " �_�u�۰ʰk�]" );
    if ( act & PLR_ANGEL        ) send_to_stack( " �u�@��"       );
    if ( act & PLR_AUTOFOOD     ) send_to_stack( " �۰ʦY����"   );
    if ( act & PLR_AUTODRINK    ) send_to_stack( " �۰ʳܤ�"     );
    if ( act & PLR_NODEATH      ) send_to_stack( " ����"         );
  }

  else
  {
    send_to_stack( " �D���a" );
    if ( act & ACT_SENTINEL     ) send_to_stack( " ������"         );
    if ( act & ACT_SCAVENGER    ) send_to_stack( " �߫���"         );
    if ( act & ACT_AGGRESSIVE   ) send_to_stack( " �۰ʧ���"       );
    if ( act & ACT_STAY_AREA    ) send_to_stack( " ���|���}���ϰ�" );
    if ( act & ACT_WIMPY        ) send_to_stack( " �۰ʰk�]"       );
    if ( act & ACT_PET          ) send_to_stack( " �d��"           );
    if ( act & ACT_TRAIN        ) send_to_stack( " �V�m��"         );
    if ( act & ACT_PRACTICE     ) send_to_stack( " �m�ߪ�"         );
    if ( act & ACT_REBIRTH      ) send_to_stack( " ��¾��"         );
    if ( act & ACT_REBORN_FIGHT ) send_to_stack( " ���;԰�"       );
    if ( act & AUTOSET_VALUE    ) send_to_stack( " �۰ʳ]�w�ƭ�"   );
    if ( act & ACT_NOREBORN     ) send_to_stack( " ���|����"       );
    if ( act & ACT_NOKILL       ) send_to_stack( " ������`"       );
    if ( act & ACT_NOSUMMON     ) send_to_stack( " ����۳�"       );
    if ( act & ACT_ENROLL       ) send_to_stack( " �O��"           );
  }

  RETURN( return_stack() );
}

/* MOB �{�����W�� */
char * progtypes_bit_name( int progtypes )
{
  static char buf[512];

  PUSH_FUNCTION( "progtypes_bit_name" );

  buf[0] = '\x0';

  if ( progtypes & IN_FILE_PROG   ) str_cat( buf , " IN_FILE"   );
  if ( progtypes & ACT_PROG       ) str_cat( buf , " ACT"       );
  if ( progtypes & SPEECH_PROG    ) str_cat( buf , " SPEECH"    );
  if ( progtypes & RAND_PROG      ) str_cat( buf , " RAND"      );
  if ( progtypes & FIGHT_PROG     ) str_cat( buf , " FIGHT"     );
  if ( progtypes & DEATH_PROG     ) str_cat( buf , " DEATH"     );
  if ( progtypes & HITPRCNT_PROG  ) str_cat( buf , " HITPRCNT"  );
  if ( progtypes & ENTRY_PROG     ) str_cat( buf , " ENTRY"     );
  if ( progtypes & GREET_PROG     ) str_cat( buf , " GREET"     );
  if ( progtypes & ALL_GREET_PROG ) str_cat( buf , " ALL_GREET" );
  if ( progtypes & GIVE_PROG      ) str_cat( buf , " GIVE"      );
  if ( progtypes & BRIBE_PROG     ) str_cat( buf , " BRIBE"     );

  RETURN( ( buf[0] != '\x0' ) ? buf+1 : "(�S��)" );
}

char * get_obj_value_usage( int type, int * array )
{
  static char buf[ MAX_STRING_LENGTH ];

  PUSH_FUNCTION( "get_obj_value_usage" );

  switch ( type )
  {
  default :

    sprintf( buf,
      "V0  (�γ~����)=%6d "
      "V1  (�γ~����)=%6d "
      "V2  (�γ~����)=%6d\n\r"
      "V3  (�γ~����)=%6d "
      "V4  (�γ~����)=%6d "
      "V5  (�γ~����)=%6d\n\r"
      "V6  (�γ~����)=%6d "
      "V7  (�γ~����)=%6d "
      "V8  (�γ~����)=%6d\n\r"
      "V9  (�γ~����)=%6d "
      "V10 (�γ~����)=%6d "
      "V11 (�γ~����)=%6d\n\r"
      "V12 (�γ~����)=%6d "
      "V13 (�γ~����)=%6d "
      "V14 (�γ~����)=%6d\n\r"
      ,*array      ,*(array+1)  ,*(array+2)  ,*(array+3)  ,*(array+4)
      ,*(array+5)  ,*(array+6)  ,*(array+7)  ,*(array+8)  ,*(array+9)
      ,*(array+10) ,*(array+11) ,*(array+12) ,*(array+13) ,*(array+14) );

    break;

  case ITEM_LIGHT:

    sprintf( buf ,
      "V0  (�S���ϥ�)=%6d "
      "V1  (�S���ϥ�)=%6d "
      "V2  (�ϥήɶ�)=%6d\n\r"
      "V3  (�S���ϥ�)=%6d "
      "V4  (�S���ϥ�)=%6d "
      "V5  (�S���ϥ�)=%6d\n\r"
      "V6  (�S���ϥ�)=%6d "
      "V7  (�S���ϥ�)=%6d "
      "V8  (�S���ϥ�)=%6d\n\r"
      "V9  (�S���ϥ�)=%6d "
      "V10 (�S���ϥ�)=%6d "
      "V11 (�S���ϥ�)=%6d\n\r"
      "V12 (�S���ϥ�)=%6d "
      "V13 (�S���ϥ�)=%6d "
      "V14 (�S���ϥ�)=%6d\n\r"
      ,*array      ,*(array+1)  ,*(array+2)  ,*(array+3)  ,*(array+4)
      ,*(array+5)  ,*(array+6)  ,*(array+7)  ,*(array+8)  ,*(array+9)
      ,*(array+10) ,*(array+11) ,*(array+12) ,*(array+13) ,*(array+14) );

    break;

  case ITEM_MYSTERY:
    sprintf( buf ,
      "V0  (�k�N�s��)=%6d "
      "V1  ( ���m�� )=%6d "
      "V2  (�S���ϥ�)=%6d\n\r"
      "V3  (�S���ϥ�)=%6d "
      "V4  (�S���ϥ�)=%6d "
      "V5  (�S���ϥ�)=%6d\n\r"
      "V6  (�S���ϥ�)=%6d "
      "V7  (�S���ϥ�)=%6d "
      "V8  (�S���ϥ�)=%6d\n\r"
      "V9  (�S���ϥ�)=%6d "
      "V10 (�S���ϥ�)=%6d "
      "V11 (�S���ϥ�)=%6d\n\r"
      "V12 (�S���ϥ�)=%6d "
      "V13 (�S���ϥ�)=%6d "
      "V14 (�S���ϥ�)=%6d\n\r"
      ,*array      , *(array+1) ,*(array+2)
      ,*(array+3)  , *(array+4) ,*(array+5)
      ,*(array+6)  ,*(array+7)  ,*(array+8)  ,*(array+9)
      ,*(array+10) ,*(array+11) ,*(array+12) ,*(array+13) ,*(array+14) );
    break;

  case ITEM_SCROLL:
  case ITEM_POTION:
  case ITEM_PILL:

    sprintf( buf ,
      "V0  (�v�T�ƭ�)=%6d "
      "V1  (�k�N�W��)=%8s "
      "V2  (�k�N�s��)=%6d\n\r"
      "V3  (�Ī����q)=%6d "
      "V4  (�S���ϥ�)=%8d "
      "V5  (�S���ϥ�)=%6d\n\r"
      "V6  (�S���ϥ�)=%6d "
      "V7  (�S���ϥ�)=%8d "
      "V8  (�S���ϥ�)=%6d\n\r"
      "V9  (�S���ϥ�)=%6d "
      "V10 (�S���ϥ�)=%8d "
      "V11 (�S���ϥ�)=%6d\n\r"
      "V12 (�S���ϥ�)=%6d "
      "V13 (�S���ϥ�)=%8d "
      "V14 (�S���ϥ�)=%6d\n\r"
      ,*array      , obj_cast_location( *(array+1) ) ,*(array+2)
      ,*(array+3)  , *(array+4) ,*(array+5)
      ,*(array+6)  ,*(array+7)  ,*(array+8)  ,*(array+9)
      ,*(array+10) ,*(array+11) ,*(array+12) ,*(array+13) ,*(array+14) );

    break;

  case ITEM_STAFF:
  case ITEM_WAND:

    sprintf( buf ,
      "V0  (�S���ϥ�)=%6d "
      "V1  (�S���ϥ�)=%6d "
      "V2  (�S���ϥ�)=%6d\n\r"
      "V3  (�S���ϥ�)=%6d "
      "V4  (�[�j����)=%6d "
      "V5  (�[�j�{��)=%6d\n\r"
      "V6  (�ثe�k�O)=%6d "
      "V7  (���Ӫk�O)=%6d "
      "V8  (�S���ϥ�)=%6d\n\r"
      "V9  (�S���ϥ�)=%6d "
      "V10 (�S���ϥ�)=%6d "
      "V11 (�S���ϥ�)=%6d\n\r"
      "V12 (�S���ϥ�)=%6d "
      "V13 (�S���ϥ�)=%6d "
      "V14 (�S���ϥ�)=%6d\n\r"
      ,*array      ,*(array+1)  ,*(array+2)  ,*(array+3)  ,*(array+4)
      ,*(array+5)  ,*(array+6)  ,*(array+7)  ,*(array+8)  ,*(array+9)
      ,*(array+10) ,*(array+11) ,*(array+12) ,*(array+13) ,*(array+14) );

    break;

  case ITEM_WEAPON:

    sprintf( buf ,
      "V0  (�S���ϥ�)=%6d "
      "V1  (�̤p�ˮ`)=%6d "
      "V2  (�̤j�ˮ`)=%6d\n\r"
      "V3  (�Z������)= %s (%d)\n\r"
      "V4  (�S���ϥ�)=%6d "
      "V5  (�S���ϥ�)=%6d "
      "V6  (�S���ϥ�)=%6d\n\r"
      "V7  (�S���ϥ�)=%6d "
      "V8  (�S���ϥ�)=%6d "
      "V9  (�S���ϥ�)=%6d\n\r"
      "V10 (�S���ϥ�)=%6d "
      "V11 (�S���ϥ�)=%6d "
      "V12 (�S���ϥ�)=%6d\n\r"
      "V13 (�S���ϥ�)=%6d "
      "V14 (�S���ϥ�)=%6d\n\r"
      ,*array      ,*(array+1)  ,*(array+2)
      , weapon_type_name( *(array+3 ) ) , *( array+3 ), *(array+4)
      ,*(array+5)  ,*(array+6)  ,*(array+7)  ,*(array+8)  ,*(array+9)
      ,*(array+10) ,*(array+11) ,*(array+12) ,*(array+13) ,*(array+14) );

    break;

  case ITEM_ARROW:

    sprintf( buf ,
      "V0  (�̤j�ˮ`)=%6d "
      "V1  (�̤p�ˮ`)=%6d "
      "V2  (�S���ϥ�)=%6d\n\r"
      "V3  (�S���ϥ�)= %s (%d)\n\r"
      "V4  (�S���ϥ�)=%6d "
      "V5  (�S���ϥ�)=%6d "
      "V6  (�S���ϥ�)=%6d\n\r"
      "V7  (�S���ϥ�)=%6d "
      "V8  (�S���ϥ�)=%6d "
      "V9  (�S���ϥ�)=%6d\n\r"
      "V10 (�S���ϥ�)=%6d "
      "V11 (�S���ϥ�)=%6d "
      "V12 (�S���ϥ�)=%6d\n\r"
      "V13 (�S���ϥ�)=%6d "
      "V14 (�S���ϥ�)=%6d\n\r"
      ,*array      ,*(array+1)  ,*(array+2)
      , weapon_type_name( *(array+3 ) ) , *( array+3 ), *(array+4)
      ,*(array+5)  ,*(array+6)  ,*(array+7)  ,*(array+8)  ,*(array+9)
      ,*(array+10) ,*(array+11) ,*(array+12) ,*(array+13) ,*(array+14) );

    break;

  case ITEM_FURNITURE:
  case ITEM_TRASH:
  case ITEM_KEY:
  case ITEM_BOAT:
  case ITEM_CORPSE_PC:
  case ITEM_CORPSE_NPC:
  case ITEM_FOUNTAIN:
  case ITEM_TREASURE:
  case ITEM_SIGIL:
  case ITEM_VELLUM:

    sprintf( buf ,
      "V0  (�S���ϥ�)=%6d "
      "V1  (�S���ϥ�)=%6d "
      "V2  (�S���ϥ�)=%6d\n\r"
      "V3  (�S���ϥ�)=%6d "
      "V4  (�S���ϥ�)=%6d "
      "V5  (�S���ϥ�)=%6d\n\r"
      "V6  (�S���ϥ�)=%6d "
      "V7  (�S���ϥ�)=%6d "
      "V8  (�S���ϥ�)=%6d\n\r"
      "V9  (�S���ϥ�)=%6d "
      "V10 (�S���ϥ�)=%6d "
      "V11 (�S���ϥ�)=%6d\n\r"
      "V12 (�S���ϥ�)=%6d "
      "V13 (�S���ϥ�)=%6d "
      "V14 (�S���ϥ�)=%6d\n\r"
      ,*array      ,*(array+1)  ,*(array+2)  ,*(array+3)  ,*(array+4)
      ,*(array+5)  ,*(array+6)  ,*(array+7)  ,*(array+8)  ,*(array+9)
      ,*(array+10) ,*(array+11) ,*(array+12) ,*(array+13) ,*(array+14) );

    break;

  case ITEM_FIREWORK:

    sprintf( buf ,
      "V0  (�I��Z��)=%6d "
      "V1  (���Ѿ��v)=%6d "
      "V2  (�S���ϥ�)=%6d\n\r"
      "V3  (�S���ϥ�)=%6d "
      "V4  (�S���ϥ�)=%6d "
      "V5  (�S���ϥ�)=%6d\n\r"
      "V6  (�S���ϥ�)=%6d "
      "V7  (�S���ϥ�)=%6d "
      "V8  (�S���ϥ�)=%6d\n\r"
      "V9  (�S���ϥ�)=%6d "
      "V10 (�S���ϥ�)=%6d "
      "V11 (�S���ϥ�)=%6d\n\r"
      "V12 (�S���ϥ�)=%6d "
      "V13 (�S���ϥ�)=%6d "
      "V14 (�S���ϥ�)=%6d\n\r"
      ,*array      ,*(array+1)  ,*(array+2)  ,*(array+3)  ,*(array+4)
      ,*(array+5)  ,*(array+6)  ,*(array+7)  ,*(array+8)  ,*(array+9)
      ,*(array+10) ,*(array+11) ,*(array+12) ,*(array+13) ,*(array+14) );

    break;

  case ITEM_URN:

    sprintf( buf ,
      "V0  (�Ǫ����X)=%6d "
      "V1  (�� �R �O)=%6d "
      "V2  (�k�O�ƭ�)=%6d\n\r"
      "V3  (��O�ƭ�)=%6d "
      "V4  (�S���ϥ�)=%6d "
      "V5  (�S���ϥ�)=%6d\n\r"
      "V6  (�S���ϥ�)=%6d "
      "V7  (�S���ϥ�)=%6d "
      "V8  (�S���ϥ�)=%6d\n\r"
      "V9  (�S���ϥ�)=%6d "
      "V10 (�S���ϥ�)=%6d "
      "V11 (�S���ϥ�)=%6d\n\r"
      "V12 (�S���ϥ�)=%6d "
      "V13 (�S���ϥ�)=%6d "
      "V14 (�S���ϥ�)=%6d\n\r"
      ,*array      ,*(array+1)  ,*(array+2)  ,*(array+3)  ,*(array+4)
      ,*(array+5)  ,*(array+6)  ,*(array+7)  ,*(array+8)  ,*(array+9)
      ,*(array+10) ,*(array+11) ,*(array+12) ,*(array+13) ,*(array+14) );

    break;

  case ITEM_LETTER:

    sprintf( buf ,
      "V0  (�Ѿl�ɶ�)=%6d "
      "V1  (�S���ϥ�)=%6d "
      "V2  (�S���ϥ�)=%6d\n\r"
      "V3  (�S���ϥ�)=%6d "
      "V4  (�S���ϥ�)=%6d "
      "V5  (�S���ϥ�)=%6d\n\r"
      "V6  (�S���ϥ�)=%6d "
      "V7  (�S���ϥ�)=%6d "
      "V8  (�S���ϥ�)=%6d\n\r"
      "V9  (�S���ϥ�)=%6d "
      "V10 (�S���ϥ�)=%6d "
      "V11 (�S���ϥ�)=%6d\n\r"
      "V12 (�S���ϥ�)=%6d "
      "V13 (�S���ϥ�)=%6d "
      "V14 (�S���ϥ�)=%6d\n\r"
      ,*array      ,*(array+1)  ,*(array+2)  ,*(array+3)  ,*(array+4)
      ,*(array+5)  ,*(array+6)  ,*(array+7)  ,*(array+8)  ,*(array+9)
      ,*(array+10) ,*(array+11) ,*(array+12) ,*(array+13) ,*(array+14) );

    break;

  case ITEM_ORE:

    sprintf( buf ,
      "V0  (�X�{���~)=%6d "
      "V1  (�k�N����)=%6d "
      "V2  (���\\���|)=%6d\n\r"
      "V3  (���y����)=%6d "
      "V4  (�S���ϥ�)=%6d "
      "V5  (�S���ϥ�)=%6d\n\r"
      "V6  (�S���ϥ�)=%6d "
      "V7  (�S���ϥ�)=%6d "
      "V8  (�S���ϥ�)=%6d\n\r"
      "V9  (�S���ϥ�)=%6d "
      "V10 (�S���ϥ�)=%6d "
      "V11 (�S���ϥ�)=%6d\n\r"
      "V12 (�S���ϥ�)=%6d "
      "V13 (�S���ϥ�)=%6d "
      "V14 (�S���ϥ�)=%6d\n\r"
      ,*array      ,*(array+1)  ,*(array+2)  ,*(array+3)  ,*(array+4)
      ,*(array+5)  ,*(array+6)  ,*(array+7)  ,*(array+8)  ,*(array+9)
      ,*(array+10) ,*(array+11) ,*(array+12) ,*(array+13) ,*(array+14) );

    break;

  case ITEM_GOLDMINE:

    sprintf( buf ,
      "V0  (�`�@����)=%6d "
      "V1  (�ֿn����)=%6d "
      "V2  (�̤p����)=%6d\n\r"
      "V3  (�S���ϥ�)=%6d "
      "V4  (�S���ϥ�)=%6d "
      "V5  (���~���X)=%6d\n\r"
      "V6  (���~���X)=%6d "
      "V7  (���~���X)=%6d "
      "V8  (���~���X)=%6d\n\r"
      "V9  (���~���X)=%6d "
      "V10 (���~���X)=%6d "
      "V11 (���~���X)=%6d\n\r"
      "V12 (���~���X)=%6d "
      "V13 (���~���X)=%6d "
      "V14 (���~���X)=%6d\n\r"
      ,*array      ,*(array+1)  ,*(array+2)  ,*(array+3)  ,*(array+4)
      ,*(array+5)  ,*(array+6)  ,*(array+7)  ,*(array+8)  ,*(array+9)
      ,*(array+10) ,*(array+11) ,*(array+12) ,*(array+13) ,*(array+14) );

    break;

  case ITEM_ARMOR:
    sprintf( buf ,
      "V0  (���@����)=%6d "
      "V1  (�S���ϥ�)=%6d "
      "V2  (�S���ϥ�)=%6d\n\r"
      "V3  (�S���ϥ�)=%6d "
      "V4  (�S���ϥ�)=%6d "
      "V5  (�S���ϥ�)=%6d\n\r"
      "V6  (�S���ϥ�)=%6d "
      "V7  (�S���ϥ�)=%6d "
      "V8  (�S���ϥ�)=%6d\n\r"
      "V9  (�S���ϥ�)=%6d "
      "V10 (�S���ϥ�)=%6d "
      "V11 (�S���ϥ�)=%6d\n\r"
      "V12 (�S���ϥ�)=%6d "
      "V13 (�S���ϥ�)=%6d "
      "V14 (�S���ϥ�)=%6d\n\r"
      ,*array      ,*(array+1)  ,*(array+2)  ,*(array+3)  ,*(array+4)
      ,*(array+5)  ,*(array+6)  ,*(array+7)  ,*(array+8)  ,*(array+9)
      ,*(array+10) ,*(array+11) ,*(array+12) ,*(array+13) ,*(array+14) );

    break;

  case ITEM_BACKBAG:
    sprintf( buf ,
      "V0  (�̦h�ƶq)=%6d "
      "V1  (�ثe�ƶq)=%6d "
      "V2  (�Z�����X)=%6d\n\r"
      "V3  (�S���ϥ�)=%6d "
      "V4  (�S���ϥ�)=%6d "
      "V5  (�S���ϥ�)=%6d\n\r"
      "V6  (�S���ϥ�)=%6d "
      "V7  (�S���ϥ�)=%6d "
      "V8  (�S���ϥ�)=%6d\n\r"
      "V9  (�S���ϥ�)=%6d "
      "V10 (�S���ϥ�)=%6d "
      "V11 (�S���ϥ�)=%6d\n\r"
      "V12 (�S���ϥ�)=%6d "
      "V13 (�S���ϥ�)=%6d "
      "V14 (�S���ϥ�)=%6d\n\r"
      ,*array      ,*(array+1)  ,*(array+2)  ,*(array+3)  ,*(array+4)
      ,*(array+5)  ,*(array+6)  ,*(array+7)  ,*(array+8)  ,*(array+9)
      ,*(array+10) ,*(array+11) ,*(array+12) ,*(array+13) ,*(array+14) );

    break;

  case ITEM_CONTAINER:
    sprintf( buf ,
      "V0  (�s�񭫶q)=%6d "
      "V1  (�˸m�X��)= %s (%d)\n\r"
      "V2  (�_�͸��X)=%6d "
      "V3  (�S���ϥ�)=%6d "
      "V4  (�S���ϥ�)=%6d\n\r"
      "V5  (�S���ϥ�)=%6d "
      "V6  (�S���ϥ�)=%6d "
      "V7  (�S���ϥ�)=%6d\n\r"
      "V8  (�S���ϥ�)=%6d "
      "V9  (�S���ϥ�)=%6d "
      "V10 (�S���ϥ�)=%6d\n\r"
      "V11 (�S���ϥ�)=%6d "
      "V12 (�S���ϥ�)=%6d "
      "V13 (�S���ϥ�)=%6d\n\r"
      "V14 (�S���ϥ�)=%6d\n\r"
      ,*array      , container_bit_name( *(array+1) ), *(array+1)
      ,*(array+2)  ,*(array+3)  ,*(array+4)
      ,*(array+5)  ,*(array+6)  ,*(array+7)  ,*(array+8)  ,*(array+9)
      ,*(array+10) ,*(array+11) ,*(array+12) ,*(array+13) ,*(array+14) );

    break;

  case ITEM_DRINK_CON:
    sprintf( buf ,
       "V0  (�̤j�e�q)=%6d "
       "V1  (�ثe�e�q)=%6d "
       "V2  (�G�鸹�X)=%6d\n\r"
       "V3  (�O�_���r)= %s (%d)\n\r"
       "V4  (�S���ϥ�)=%6d "
       "V5  (�S���ϥ�)=%6d "
       "V6  (�S���ϥ�)=%6d\n\r"
       "V7  (�S���ϥ�)=%6d "
       "V8  (�S���ϥ�)=%6d "
       "V9  (�S���ϥ�)=%6d\n\r"
       "V10 (�S���ϥ�)=%6d "
       "V11 (�S���ϥ�)=%6d "
       "V12 (�S���ϥ�)=%6d\n\r"
       "V13 (�S���ϥ�)=%6d "
       "V14 (�S���ϥ�)=%6d\n\r"
      ,*array      ,*(array+1)  ,*(array+2)
      , *(array+3) == 0 ? "�L�r" : "���r" ,*(array+3)  ,*(array+4)
      ,*(array+5)  ,*(array+6)  ,*(array+7)  ,*(array+8)  ,*(array+9)
      ,*(array+10) ,*(array+11) ,*(array+12) ,*(array+13) ,*(array+14) );

    break;

  case ITEM_FOOD:
    sprintf( buf ,
      "V0  (���Ĵ���)=%6d "
      "V1  (�{������)=%6d "
      "V2  (�@�f���q)=%6d\n\r"
      "V3  (�O�_���r)=%s (%d)\n\r"
      "V4  (���ͪ��~)=%6d "
      "V5  (�S���ϥ�)=%6d "
      "V6  (�S���ϥ�)=%6d\n\r"
      "V7  (�S���ϥ�)=%6d "
      "V8  (�S���ϥ�)=%6d "
      "V9  (�S���ϥ�)=%6d\n\r"
      "V10 (�S���ϥ�)=%6d "
      "V11 (�S���ϥ�)=%6d "
      "V12 (�S���ϥ�)=%6d\n\r"
      "V13 (�S���ϥ�)=%6d "
      "V14 (�S���ϥ�)=%6d\n\r"
      ,*array      ,*(array+1)  ,*(array+2)
      , *(array+3) == 0 ? "�L�r" : "���r" ,*(array+3)  ,*(array+4)
      ,*(array+5)  ,*(array+6)  ,*(array+7)  ,*(array+8)  ,*(array+9)
      ,*(array+10) ,*(array+11) ,*(array+12) ,*(array+13) ,*(array+14) );

    break;

  case ITEM_MONEY:
    sprintf( buf ,
      "V0  (�����ƶq)=%6d "
      "V1  (�S���ϥ�)=%6d "
      "V2  (�S���ϥ�)=%6d\n\r"
      "V3  (�S���ϥ�)=%6d "
      "V4  (�S���ϥ�)=%6d "
      "V5  (�S���ϥ�)=%6d\n\r"
      "V6  (�S���ϥ�)=%6d "
      "V7  (�S���ϥ�)=%6d "
      "V8  (�S���ϥ�)=%6d\n\r"
      "V9  (�S���ϥ�)=%6d "
      "V10 (�S���ϥ�)=%6d "
      "V11 (�S���ϥ�)=%6d\n\r"
      "V12 (�S���ϥ�)=%6d "
      "V13 (�S���ϥ�)=%6d "
      "V14 (�S���ϥ�)=%6d\n\r"
      ,*array      ,*(array+1)  ,*(array+2)  ,*(array+3)  ,*(array+4)
      ,*(array+5)  ,*(array+6)  ,*(array+7)  ,*(array+8)  ,*(array+9)
      ,*(array+10) ,*(array+11) ,*(array+12) ,*(array+13) ,*(array+14) );

    break;

  case ITEM_MAGICSTONE:
    sprintf( buf ,
      "V0  (�]�ۺ���)=%6d "
      "V1  (�k�N�]�w)=%6d "
      "V2  (�g���  )=%6d\n\r"
      "V3  (�]�۪��A)=%6d "
      "V4  (�v�T�{��)=%6d "
      "V5  (�]�۵���)=%6d\n\r"
      "V6  (�S���ϥ�)=%6d "
      "V7  (�S���ϥ�)=%6d "
      "V8  (�S���ϥ�)=%6d\n\r"
      "V9  (�S���ϥ�)=%6d "
      "V10 (�S���ϥ�)=%6d "
      "V11 (�S���ϥ�)=%6d\n\r"
      "V12 (�S���ϥ�)=%6d "
      "V13 (�S���ϥ�)=%6d "
      "V14 (�S���ϥ�)=%6d\n\r"
      ,*array      ,*(array+1)  ,*(array+2)  ,*(array+3)  ,*(array+4)
      ,*(array+5)  ,*(array+6)  ,*(array+7)  ,*(array+8)  ,*(array+9)
      ,*(array+10) ,*(array+11) ,*(array+12) ,*(array+13) ,*(array+14) );

    break;

  /* ���� */
  case ITEM_SPIRITJADE:
    sprintf( buf ,
      "V0  (�p�����X)=%6d "
      "V1  (�p������)=%6d "
      "V2  (�p���g��)=%6d\n\r"
      "V3  (�ޯู�X)=%6d "
      "V4  (�ޯ൥��)=%6d "
      "V5  (�S���ϥ�)=%6d\n\r"
      "V6  (�S���ϥ�)=%6d "
      "V7  (�S���ϥ�)=%6d "
      "V8  (�S���ϥ�)=%6d\n\r"
      "V9  (�S���ϥ�)=%6d "
      "V10 (�S���ϥ�)=%6d "
      "V11 (�S���ϥ�)=%6d\n\r"
      "V12 (�S���ϥ�)=%6d "
      "V13 (�S���ϥ�)=%6d "
      "V14 (�S���ϥ�)=%6d\n\r"
      ,*array      ,*(array+1)  ,*(array+2)  ,*(array+3)  ,*(array+4)
      ,*(array+5)  ,*(array+6)  ,*(array+7)  ,*(array+8)  ,*(array+9)
      ,*(array+10) ,*(array+11) ,*(array+12) ,*(array+13) ,*(array+14) );

    break;
  }
  RETURN( buf );
}

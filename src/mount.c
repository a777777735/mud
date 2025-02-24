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
#include "merc.h"

bool    mount_char      args( ( CHAR_DATA *, CHAR_DATA * ) );

/* �M�W�� */
FUNCTION( do_mount )
{
  CHAR_DATA * mount;
  char        arg[MAX_INPUT_LENGTH];
  char        buf[MAX_STRING_LENGTH];
  int         adept;

  PUSH_FUNCTION( "do_mount" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  one_argument( argument, arg );

  /* �S���ѼƴN�O�d�ߧ��M */
  if ( !arg[0] )
  {
    if ( ( mount = ch->mount ) )
    {
      sprintf( buf, "%d/%d", mount->move, get_curr_move( mount ) );
      act( "�A���y�M�O$N(��O�R$t)", ch, buf, mount, TO_CHAR );
    }

    else
    {
      send_to_char( "�A�S�����󪺮y�M��C\n\r", ch );
    }

    RETURN_NULL();
  }

  if ( ( adept = ch->skill[SLOT_MOUNT] )  <= 0 )
  {
    send_to_char( "�藍�_�M�A�٤��|�M�N�o�@�ۭC�T\n\r", ch );
    RETURN_NULL();
  }

  /* ���୫�Ʀ����M */
  if ( ch->mount )
  {
    act( "�A�w�g�M��$N�F�C", ch, NULL, ch->mount, TO_CHAR );
    RETURN_NULL();
  }

  /* �ˬd�O�_�ۤv�O�O�H�����M */
  if ( ch->mount_by )
  {
    act( "�A�ۤv�w�g�Q$N�M�ۤF�C", ch, NULL, ch->mount_by, TO_CHAR );
    RETURN_NULL();
  }

  /* �ˬd�O�_�ۤv�����X�A�P�_ */
  if ( ch->tractable > 0 )
  {
    send_to_char( "�A���A�X���y�M�C\n\r", ch );
    RETURN_NULL();
  }

  /* ���঳����N */
  if ( is_affected( ch, SLOT_FLY ) )
  {
    send_to_char( "�A���}���a�M�L�k�M�W�A�����M�C\n\r", ch );
    RETURN_NULL();
  }

  /* ��M�ж��̭������M */
  if ( !( mount = get_char_room( ch, arg ) ) )
  {
    act( "�䤣��A���y�M $2$T$0�C", ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  /* �ݬݳo���ʪ��O�_�w�g�Q�M���F */
  if ( !IS_NPC( mount ) || mount->tractable <= 0 )
  {
    act( "$N���M�Q�M��A���Y�W�C", mount, NULL, ch, TO_CHAR );
    act( "�A�{���A���y�M�F$N�C", ch, NULL, mount, TO_CHAR );
    RETURN_NULL();
  }

  /* �ݬݬO�_�Q�g�b�άO���D�H�� */
  if ( mount->master || mount->leader )
  {
    act( "$N���䥦���D�H�F�C", ch, NULL, mount, TO_CHAR );
    RETURN_NULL();
  }

  /* �w�g�O�O�H�����M�� */
  if ( mount->mount_by )
  {
    act( "$N���M�Q�m�A���y�M�C�C", mount->mount_by, NULL, ch, TO_CHAR );
    act( "$N�w�g�O�䥦�H���y�M�F�C", ch, NULL, mount, TO_CHAR );
    RETURN_NULL();
  }

  /* �ݬݧ��M�����A�O�_���T */
  if ( mount->position != POS_STANDING )
  {
    act( "$N�ثe�ȮɨS��k�M�C", ch, NULL, mount, TO_CHAR );
    RETURN_NULL();
  }

  /* ���M�O�_�i�H�ݨ�A */
  if ( !can_see( mount, ch ) )
  {
    act( "$N�ݤ���A�C�C", ch, NULL, mount, TO_CHAR );
    RETURN_NULL();
  }

  /* �ݬݬO�_��O�O�_���� */
  if ( ch->move < MountCost )
  {
    send_to_char( "�A����O���١M���}�o�n�M�Ϥ��W�O�C\n\r", ch );
    ch->move = 0;
    RETURN_NULL();
  }

  /* ������O�ӷl */
  ch->move = UMAX( 0, ch->move - MountCost );

  /* �ˬd���v */
  if ( number_range( UMIN( 100, adept * 5 ) , 100 ) < mount->tractable )
  {
    act( "$N��M���ʤj�o�M$n�q$N���W�L�F�U�ӡC", ch, NULL, mount, TO_ALL );
    ch->move  = UMAX( 0, ch->move - MountCost );

    RETURN_NULL();
  }

  if ( !mount_char( ch, mount ) )
  {
    send_to_char( "�A���ѤF�C\n\r" , ch );
    RETURN_NULL();
  }

  if ( IS_NPC( mount ) && mount->talk_buf )
  {
    free_string( mount->talk_buf );
    mount->talk_buf  = NULL;
    mount->talk_mode = MODE_ABSENT;
  }

  /* ��X�ԭz */
  act( "$N����$n���y�M�F�C"  , ch, NULL, mount, TO_ALL );

  /* ��o�g��� */
  gain_skill_exp( ch, SLOT_MOUNT, TRUE );

  RETURN_NULL();
}

/* ���U���M */
FUNCTION( do_unmount )
{
  PUSH_FUNCTION( "do_unmount" );

  /* �ˬd�O�_�w�g�O�_�����M */
  if ( !ch->mount )
  {
    send_to_char( "�A�èS�����󪺮y�M��C\n\r", ch );
    RETURN_NULL();
  }

  if ( !unmount_char( ch, ch->mount ) )
  {
    send_to_char( "�A���ѤF�C\n\r", ch );
    RETURN_NULL();
  }

  adjust_carry( ch );
  RETURN_NULL();
}

/* �ˬd�P�M�W���M */
bool mount_char( CHAR_DATA * rider, CHAR_DATA * ridden )
{
  PUSH_FUNCTION( "mount_char" );

  /* �ˬd���� */
  if ( rider->mount
    || rider->mount_by
    || rider->tractable > 0
    || !IS_NPC( ridden )
    || ridden->tractable <= 0
    || ridden->mount_by
    || ridden->master
    || ridden->leader )
    RETURN( FALSE );

  rider->mount     = ridden;
  ridden->mount_by = rider;

  RETURN( TRUE );
}

/* ���M�ѮM */
bool unmount_char( CHAR_DATA * rider, CHAR_DATA * ridden )
{
  PUSH_FUNCTION( "unmount_char" );

  /* ���ˬd�ӷ� */
  if ( !rider || !ridden || !rider->mount ) RETURN( FALSE );

  /* �ˬd��P�Q�M�O�_�@�P */
  if ( rider->mount != ridden || ridden->mount_by != rider )
  {
    mudlog( LOG_DEBUG, "unmount_char: �M�P�Q�M���k�X" );
    RETURN( FALSE );
  }

  rider->mount     = NULL;
  ridden->mount_by = NULL;

  /* ��X�ԭz */
  if ( rider->in_room && rider->in_room == ridden->in_room )
    act( "$n�q$N���W�u�F�U�ӡC", rider, NULL, ridden, TO_ALL );

  RETURN( TRUE );
}

/* �Ǧ^�Y�H�����M */
CHAR_DATA * get_mount( CHAR_DATA * ch )
{
  PUSH_FUNCTION( "get_mount" );
  if ( !ch || !ch->mount || ch->in_room != ch->mount->in_room ) RETURN( NULL );
  RETURN( ch->mount );
}

/* �Ǧ^�Y���M���D�H */
CHAR_DATA * get_rider( CHAR_DATA * ch )
{
  PUSH_FUNCTION( "get_rider" );

  if ( !ch || !ch->mount_by || ch->in_room != ch->mount_by->in_room )
    RETURN( NULL );

  RETURN( ch->mount_by );
}

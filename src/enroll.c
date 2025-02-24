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
#include <string.h>
#include "merc.h"

void set_enroll( CHAR_DATA * ch, CHAR_DATA * victim )
{
  ENROLL_DATA * pEnroll;
  char        * name;

  PUSH_FUNCTION( "set_enroll" );

  if ( !ch || !victim )
  {
    mudlog( LOG_DEBUG, "set_enroll: �ʥF�ӷ�." );
    RETURN_NULL();
  }

  if ( !IS_NPC( ch )
    || IS_NPC( victim )
    || IS_IMMORTAL( victim )
    || !( name = victim->name )
    || !*name
    || str_len( name ) > NAME_LENGTH ) RETURN_NULL();

  for ( pEnroll = ch->enroll; pEnroll; pEnroll = pEnroll->next )
    if ( !str_cmp( pEnroll->name, name ) ) RETURN_NULL();

  pEnroll = alloc_struct( STRUCT_ENROLL_DATA );
  memset( pEnroll, 0, sizeof( ENROLL_DATA ) );

  str_cpy( pEnroll->name, name );
  pEnroll->next = ch->enroll;
  ch->enroll    = pEnroll;

  RETURN_NULL();
}

bool is_enroll( CHAR_DATA * ch, CHAR_DATA * victim )
{
  ENROLL_DATA * pEnroll;

  PUSH_FUNCTION( "is_enroll" );

  if ( !ch || !victim )
  {
    mudlog( LOG_DEBUG, "is_enroll: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( !IS_NPC( ch ) || IS_NPC( victim ) ) RETURN( FALSE );

  for ( pEnroll = ch->enroll; pEnroll; pEnroll = pEnroll->next )
    if ( !str_cmp( pEnroll->name, victim->name ) ) RETURN( TRUE );

  RETURN( FALSE );
}

void extract_enroll( CHAR_DATA * ch, const char * name )
{
  ENROLL_DATA * pEnroll;
  ENROLL_DATA * zEnroll;

  PUSH_FUNCTION( "extract_enroll" );

  if ( !ch || !name || !*name )
  {
    mudlog( LOG_DEBUG, "extract_enroll: �ʥF�ӷ�." );
    RETURN_NULL();
  }

  for ( zEnroll = NULL, pEnroll = ch->enroll;
        pEnroll;
        zEnroll = pEnroll, pEnroll = pEnroll->next )
  {
    if ( !str_cmp( pEnroll->name, name ) )
    {
      if ( !zEnroll ) ch->enroll    = ch->enroll->next;
      else            zEnroll->next = pEnroll->next;

      free_struct( pEnroll , STRUCT_ENROLL_DATA );
      RETURN_NULL();
    }
  }

  RETURN_NULL();
}

void mprog_enroll_trigger( CHAR_DATA * ch )
{
  CHAR_DATA * wch;
  CHAR_DATA * victim;
  int         count;
  char        buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "mprog_enroll_trigger" );

  if ( !ch || !ch->in_room )
  {
    mudlog( LOG_DEBUG, "mprog_enroll_trigger: �ʥF�ӷ�." );
    RETURN_NULL();
  }

  if ( !IS_NPC( ch )
    || !IS_SET( ch->act, ACT_ENROLL )
    || !ch->enroll
    || ch->position != POS_STANDING
    || ch->fighting
    || is_affected( ch, SLOT_CHARM_PERSON )
    || ch->mount_by
    || ch->boss
    || !ch->in_room
    || ch->in_room->Safe )
    RETURN_NULL();

  victim = NULL;
  for ( count = 0, wch = ch->in_room->people; wch; wch = wch->next_in_room )
  {
    if ( !verify_char( wch )
      || !can_damage( wch )
      || IS_NPC( wch )
      || IS_IMMORTAL( wch )
      || wch->fighting
      || !IS_AWAKE( wch )
      || !can_see( ch, wch )
      || !is_enroll( ch, wch ) ) continue;

    if ( number_range( 0, count ) == 0 ) victim = wch;
    count++;
  }

  if ( victim )
  {
    switch( number_range( 0 ,2 ) )
    {
    default:
    case 0:
      sprintf( buf, "%s\e[0m�M�A�o�Ӥp����J�M�S�Q�ڶe��F�a�C"
        , mob_name( ch, victim ) );
      break;

    case 1:
      sprintf( buf, "�A�o�Ӥѱ���%s\e[0m�M�ٯu�O�ޮa�����C"
        , mob_name( ch, victim ) );
      break;

    case 2:
      sprintf( buf, "��}�K�c�L�V�B�M�o�ӥ����O�\\�ҡC%s\e[0m�M�����a�C"
        , mob_name( ch, victim ) );
      break;
    }

    do_say( ch, buf );
    striking( ch, victim, TYPE_UNDEFINED );
  }

  RETURN_NULL();
}

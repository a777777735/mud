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

/* �C�ӤH�̦h�i�H���X�Ӧn�ͦW�� */
#define MAX_FRIEND      30

int     max_friend      = MAX_FRIEND;

void            extract_a_friend args( ( CHAR_DATA *, const char * ) );

FUNCTION( do_friend )
{
  FRIEND_DATA * pFriend;
  BASIC_DATA  * pBasic;
  CLASS_DATA  * pClass;
  CHAR_DATA   * victim;
  CHAR_DATA   * vch;
  char          arg[MAX_INPUT_LENGTH];
  char          buf[MAX_STRING_LENGTH];
  int           count;

  PUSH_FUNCTION( "do_friend" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  argument = one_argument( argument, arg );

  if ( arg[0] == '+' )
  {
    if ( arg[1] == '\x0' )
    {
      send_to_char( "�A�n�[�J���@�쪱�a���n�ͦW��S\n\r", ch );
      RETURN_NULL();
    }

    if ( is_friend( ch, arg + 1 ) )
    {
      act( "$t���ӴN�b�A���n�ͦW�椧���C", ch, arg + 1, NULL, TO_CHAR );
      RETURN_NULL();
    }

    smash_tilde( arg + 1 );

    if ( str_len( arg + 1 ) > sizeof( pFriend->name ) )
    {
      send_to_char( "�A���n�ͦW�٪��פ��X�k�T\n\r", ch );
      RETURN_NULL();
    }

    if ( !str_cmp( ch->name, arg + 1 ) )
    {
      send_to_char( "�A�n��ۤv��@�n�B�ͶܡS\n\r", ch );
      RETURN_NULL();
    }

    if ( friend_count( ch ) >= max_friend && !IS_IMMORTAL( ch ) )
    {
      act( "�A���n�ͦW��W�L$i�ӡM�Х��R���@�ǧa�T", ch, &max_friend, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( !is_exist( arg + 1 ) )
    {
      act( "��Ӧt�z�����M�䤣��o�� $t ��T", ch, arg + 1, NULL, TO_CHAR );
      RETURN_NULL();
    }

    pFriend = alloc_struct( STRUCT_FRIEND_DATA );
    str_cpy( pFriend->name, arg + 1 );
    pFriend->next = ch->friend;
    ch->friend    = pFriend;

    act( "�A��$t�]���A���n�ͦW�椧���C", ch, arg + 1, NULL, TO_CHAR );
    RETURN_NULL();
  }

  else if ( arg[0] == '-' )
  {
    if ( arg[1] == '\x0' )
    {
      send_to_char( "�A�n�R�����@�쬰�n�ͦW��S\n\r", ch );
      RETURN_NULL();
    }

    if ( !is_friend( ch, arg + 1 ) )
    {
      act( "$t ���ӴN���b�A���n�ͦW�椧���o�T", ch, arg + 1, NULL, TO_CHAR );
      RETURN_NULL();
    }

    act( "�A��$t�q�A���n�ͦW�椤�����F�T", ch, arg + 1, NULL, TO_CHAR );
    extract_a_friend( ch, arg + 1 );
    RETURN_NULL();
  }

  else
  {
    if ( arg[0] == '\x0' )
    {
      victim = ch;
    }
    else
    {
      if ( !IS_IMMORTAL( ch ) )
      {
        send_to_char( "�A���v�O�����H�[�ݨ�L�H���n�ͦW��T\n\r", ch );
        RETURN_NULL();
      }

      if ( !( victim = get_char_world( ch, arg ) ) )
      {
        act( "�䤣��A�n�[�ݪ���H $2$T$0�C", ch, NULL, arg, TO_CHAR );
        RETURN_NULL();
      }

      if ( IS_NPC( victim ) )
      {
        act( "�D���a$N�O�S���n�ͦW�檺�C", ch, NULL, victim, TO_CHAR );
        RETURN_NULL();
      }

      if ( get_trust( ch ) < get_trust( victim ) )
      {
        act( "�A���v�O������$N���n�ͦW��C", ch, NULL, victim, TO_CHAR );
        RETURN_NULL();
      }
    }

    if ( !victim->friend )
    {
      act( "$N�èS�����󪺦n�ͦW��C", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    clear_buffer();
    count = 0;

    send_to_buffer(
      "\e[1;33;44m���� �W�u �W�r         ����W�r                        "
      " ���� �~�� �ʧO ¾  �~  \e[0m\n\r" );

    for ( pFriend = victim->friend; pFriend; pFriend = pFriend->next )
    {
      if ( ( pBasic = name2basic( pFriend->name ) ) )
      {
        pClass = is_class( pBasic->class, TRUE );
        vch = is_online( pFriend->name );

        tablize( FORMAT_SET,
          "LOOP ONLI NAME         CNAME                            \e[0mLEVV "
          "AGEE SEXX CLASSS \e[0m\n\r"
          , 0, NULL, 0 );

        tablize( FORMAT_STRING, pFriend->name  , 0, "NAME" , 12 );
        tablize( FORMAT_STRING, pBasic->cname  , 0, "CNAME", 32 );
        tablize( FORMAT_STRING, YESNO( vch && can_see( ch, vch ) ), 0, "ONLI" ,  4 );
        tablize( FORMAT_STRING, class_name( pClass, FALSE )
          , 0, "CLASSS" , 6 );

        tablize( FORMAT_NUM, NULL, ++count      , "LOOP" ,  4 );
        tablize( FORMAT_NUM, NULL, pBasic->level, "LEVV" ,  4 );

        tablize( FORMAT_NUM   , NULL
          , ( pBasic->played / 14400 ) + BornAge, "AGEE",  4 );

        tablize( FORMAT_STRING, sex_name( pBasic->sex ), 0, "SEXX", 4 );
        tablize( FORMAT_RETURN, buf, 0, NULL, sizeof( buf ) - 1 );
        send_to_buffer( "%s", buf );

        free_basic( pBasic );

        if ( buffer_full() ) break;
      }
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  RETURN_NULL();
}

/* �P���n�ͦW�� */
void extract_friend( CHAR_DATA * ch )
{
  FRIEND_DATA * pFriend;
  FRIEND_DATA * zFriend;

  PUSH_FUNCTION( "extract_friend" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "extract_friend: �ӷ������T." );
    RETURN_NULL();
  }

  for ( pFriend = ch->friend; pFriend; pFriend = zFriend )
  {
    zFriend = pFriend->next;
    free_struct( pFriend, STRUCT_FRIEND_DATA );
  }

  ch->friend = NULL;
  RETURN_NULL();
}

void extract_a_friend( CHAR_DATA * ch, const char * name )
{
  FRIEND_DATA * pFriend;
  FRIEND_DATA * zFriend;

  PUSH_FUNCTION( "extract_a_friend" );

  if ( !ch || !name || !*name )
  {
    mudlog( LOG_DEBUG, "extract_a_friend: �ʥF�ӷ�." );
    RETURN_NULL();
  }

  for ( zFriend = NULL, pFriend = ch->friend;
        pFriend; zFriend = pFriend, pFriend = pFriend->next )
  {
    if ( !str_cmp( pFriend->name, name ) )
    {
      if ( !zFriend ) ch->friend    = ch->friend->next;
      else            zFriend->next = pFriend->next;
      free_struct( pFriend, STRUCT_FRIEND_DATA );
      RETURN_NULL();
    }
  }

  mudlog( LOG_DEBUG, "extract_a_friend: �S�o�{�n�P�������c." );
  RETURN_NULL();
}

size_t friend_count( CHAR_DATA * ch )
{
  FRIEND_DATA * pFriend;
  size_t        count = 0;

  PUSH_FUNCTION( "friend_count" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "friend_count: �ӷ������T." );
    RETURN( 0 );
  }

  for ( pFriend = ch->friend; pFriend; pFriend = pFriend->next ) count++;

  RETURN( count );
}

FRIEND_DATA * is_friend( CHAR_DATA * ch, const char * name )
{
  FRIEND_DATA * pFriend;

  PUSH_FUNCTION( "is_friend" );

  if ( !ch || !name || !*name )
  {
    mudlog( LOG_DEBUG, "is_friend: �ӷ������T." );
    RETURN( NULL );
  }

  for ( pFriend = ch->friend; pFriend; pFriend = pFriend->next )
    if ( !str_cmp( pFriend->name, name ) ) RETURN( pFriend );

  RETURN( NULL );
}

void friend_msg( CHAR_DATA * ch )
{
  DESCRIPTOR_DATA * man;
  FRIEND_DATA     * pFriend;
  CHAR_DATA       * victim;

  PUSH_FUNCTION( "friend_msg" );

  if ( !ch || IS_NPC( ch ) )
  {
    mudlog( LOG_DEBUG, "friend_msg: �ӷ������D." );
    RETURN_NULL();
  }

  for ( man = descriptor_list; man; man = man->next )
  {
    if ( verify_desc( man )
      && man->connected == CON_PLAYING
      && verify_char( ( victim = man->character ) ) )
    {
      for ( pFriend = victim->friend; pFriend; pFriend = pFriend->next )
      {
        if ( !str_cmp( pFriend->name, ch->name ) )
        {
          act( "�A���n��$N�W�u�o�T\a\a", victim, NULL, ch, TO_CHAR );
          RETURN_NULL();
        }
      }
    }
  }

  RETURN_NULL();
}

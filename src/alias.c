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

/* �����̤j�i�H���X���ܼ� */
#define MAX_VARIABLE              4

/* �C�@�쪱�a�����̦h�i�H���X�� */
#define MAX_ALIAS_COUNT           30

/* �C�@�쪱�a�̦h�����i�H���ƴX�� */
#define MAX_ALIAS_REPEAT          15

int     max_alias       = MAX_ALIAS_COUNT;
int     alias_repeat    = MAX_ALIAS_REPEAT;

/* �ϰ��� */
bool extract_an_alias           args( ( CHAR_DATA * , char * ) );
bool check_alias_legal          args( ( char * ) );
void alias_spooler_full         args( ( DESCRIPTOR_DATA * ) );
void expand_alias_spooler       args( ( DESCRIPTOR_DATA * ) );
extern bool write_to_descriptor args( ( DESCRIPTOR_DATA *, char * , int ) );

/* �������O���֤� */
FUNCTION( do_alias )
{
  CHAR_DATA  * victim;
  ALIAS_DATA * pAlias;
  ALIAS_DATA * zAlias;
  char         arg[MAX_INPUT_LENGTH];
  int          count;

  PUSH_FUNCTION( "do_alias" );

  /* �p�G���O���a�@�w�S���������O�� */
  if ( !ch || IS_NPC( ch ) ) RETURN_NULL();

  /* �� ~ �o�ӲŸ�������, �_�h�s�ɤ���Ū�ɷ|�����D */
  smash_tilde( argument );

  /* ���X�޼� */
  argument = one_argument( argument, arg );
  for ( ; *argument == ' '; argument++ );

  /* �S���޼ƴN�O�[�ݦۤv�Ҧ����������O */
  if ( !arg[0] || arg[0] == '!' )
  {
    if ( arg[0] == '\x0' )
    {
      victim = ch;
    }

    else if ( arg[1] == '\x0' )
    {
      send_to_char( "�A�n�ݽ֪��������O�O�S\n\r", ch );
      RETURN_NULL();
    }
    else
    {
      if ( !( victim = get_char_world( ch, arg + 1 ) ) )
      {
        act( "�䤣��o�@���H�� $2$T$0 �C�T", ch, NULL, arg+1, TO_CHAR );
        RETURN_NULL();
      }

      if ( IS_NPC( victim ) )
      {
        act( "$N�O�S���������O���T", ch, NULL, victim, TO_CHAR );
        RETURN_NULL();
      }

      if ( !biliwick( ch, victim ) )
      {
        act( "�A���v�O������$N���������O�C", ch, NULL, victim, TO_CHAR );
        RETURN_NULL();
      }
    }

    clear_buffer();
    for ( count = 0, pAlias = victim->alias; pAlias; pAlias = pAlias->next )
    {
      count++;
      send_to_buffer( "%s�� {%s} ���N�� {%s}\n\r"
        , mob_name( ch, victim ), pAlias->text , pAlias->macro );

      if ( buffer_full() ) break;
    }

    if ( count == 0 )
    {
      send_to_buffer(
        "%s�ثe�S�����󪺥������O\n\r�p�G�Q�n���D�p��ϥΥ����M"
        "�п�J alias /?�C\n\r", mob_name( victim, ch ) );
    }
    else
    {
      send_to_buffer( "\n\r%s�`�@�� %d �ӥ������O�T\n\r"
        , mob_name( victim, ch ), count );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  /* �M���Ҧ����������O�� */
  if ( arg[0] == '-' )
  {
    extract_alias( ch );
    send_to_char( "�A�M����ӥ������O�w�q�C\n\r" , ch );
    RETURN_NULL();
  }

  /* �Y�u���޼Ƥ@�h�O�M���ۦP���������O�� */
  if ( !argument[0] )
  {
    if ( extract_an_alias( ch , arg ) )
      act( "�w�g�� {$t} �����F�C", ch , arg, NULL, TO_CHAR );

    else
      act( "�å�������P {$t} �ۦP�������C", ch, arg, NULL, TO_CHAR );

    RETURN_NULL();
  }

  /* �P�ɦ��޼Ƥ@�M�޼ƤG�h�O��J����, ���O�޼Ƥ@�M�޼ƤG����ۦP */
  if ( !str_cmp( arg , argument ) )
  {
    send_to_char( "���O�M�����ۦP�|�ɭP�L�a�^��C\n\r" , ch );
    RETURN_NULL();
  }

  /* �ˬd�������O���O�_�X�k */
  if ( !check_alias_legal( arg ) || arg[0] == '!' )
  {
    send_to_char( "����ϥΫ��O��@�ӷ��R�O�����C\n\r" , ch );
    RETURN_NULL();
  }

  /* �ˬd�����O�_������, �Y���ƫh�i����N */
  for ( pAlias = ch->alias ; pAlias; pAlias = pAlias->next )
  {
    if ( !str_cmp( pAlias->text, arg ) )
    {
      str_cpy( pAlias->macro , argument );
      act( "�A�� {$t} �����N�� {$T}�C"
        ,ch , pAlias->text , pAlias->macro, TO_CHAR );

      RETURN_NULL();
    }
  }

  /* �ˬd���a�������O���O�_�Ӧh */
  if ( ( get_alias_count( ch ) > max_alias ) && !IS_IMMORTAL( ch ) )
  {
    act( "�A���������O�W�L$i�ӡM�L�k�A�W�[�C" , ch, &max_alias, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( str_len( arg      ) >= sizeof( arg ) - 1
    || str_len( argument ) >= sizeof( arg ) - 1 )
  {
    send_to_char( "�A���������פ��X�k�C\n\r", ch );
    RETURN_NULL();
  }

  /* �t�m�������O���c���O���� */
  pAlias = alloc_struct( STRUCT_ALIAS_DATA );

  str_cpy( pAlias->text , arg      );
  str_cpy( pAlias->macro, argument );
  pAlias->next = NULL;

  for ( zAlias = ch->alias; zAlias && zAlias->next; zAlias = zAlias->next );

  if ( !zAlias ) ch->alias    = pAlias;
  else           zAlias->next = pAlias;

  act( "�A�� {$t} ���N�� {$T}�C", ch, pAlias->text , pAlias->macro, TO_CHAR );
  RETURN_NULL();
}

/* �Ǧ^�Y�H�����������O�Ӽ� */
int get_alias_count( CHAR_DATA * ch )
{
  ALIAS_DATA * pAlias;
  int          count;

  PUSH_FUNCTION( "get_alias_count" );

  if ( !ch || IS_NPC( ch ) ) RETURN( 0 );
  for ( pAlias = ch->alias, count = 0; pAlias; pAlias = pAlias->next ) count++;
  RETURN( count );
}

/* �P���Ҧ������� */
void extract_alias( CHAR_DATA * ch )
{
  ALIAS_DATA * pAlias;
  ALIAS_DATA * zAlias;

  PUSH_FUNCTION( "extract_alias" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG , "extract_alias: �P��������, �H���O�Ū�." );
    RETURN_NULL();
  }

  for ( pAlias = ch->alias; pAlias; pAlias = zAlias )
  {
    zAlias  = pAlias->next;
    free_struct( pAlias , STRUCT_ALIAS_DATA );
  }

  ch->alias = NULL;
  RETURN_NULL();
}

/* �P����@������ */
bool extract_an_alias( CHAR_DATA * ch , char * arg )
{
  ALIAS_DATA * temp;
  ALIAS_DATA * prev;

  PUSH_FUNCTION( "extact_an_alias" );

  if ( !ch || !arg || !*arg )
  {
    mudlog( LOG_DEBUG, "extract_an_alias: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  for ( prev = NULL, temp = ch->alias; temp; prev = temp, temp = temp->next )
  {
    if ( !str_cmp( temp->text , arg ) )
    {
      if ( !prev ) ch->alias  = ch->alias->next;
      else         prev->next = temp->next;

      free_struct( temp , STRUCT_ALIAS_DATA );
      RETURN( TRUE );
    }
  }

  RETURN( FALSE );
}

/* �⥨�����O½Ķ�����O�� */
void translate_alias( DESCRIPTOR_DATA * d , char * input )
{
  ALIAS_DATA * pAlias;
  char         source[MAX_INPUT_LENGTH];
  char         source_command[ MAX_VARIABLE + 1 ][MAX_INPUT_LENGTH];
  char       * source_address;
  char       * target_address;
  char       * output_address;
  char       * really_address;
  int          loop;
  int          value;
  int          len_1;
  int          len_2;

  PUSH_FUNCTION( "translate_alias" );

  if ( !d || !d->character || d->connected != CON_PLAYING ) RETURN_NULL();

  output_address = really_address = input;
  input          = one_argument( input , source );

  for ( pAlias = d->character->alias; pAlias; pAlias = pAlias->next )
  {
    if ( !str_cmp( pAlias->text , source ) )
    {
      target_address = pAlias->macro;

      /* ���X�R�O�C�����޼� */
      for ( loop = 0; loop < ( MAX_VARIABLE + 1 ) ; loop++ )
      {
        source_address  = source_command[ loop ];
        *source_address = '\x0';
        input = one_argument( input , source_address );
      }

      while ( *target_address )
      {
        if ( *target_address == '%' )
        {
          value = * ( target_address + 1 ) - '0';

          if ( value > 0 && value <= MAX_VARIABLE )
          {
            len_1 = str_len( really_address );
            len_2 = str_len( source_command[ value - 1 ] );

            if ( len_1 + len_2 > sizeof( source ) - 10 )
            {
              alias_spooler_full( d );
              RETURN_NULL();
            }

            str_cat( output_address  , source_command[ value -1 ] );
            output_address += str_len( source_command[ value -1 ] );
            target_address += 2;
          }

          else
          {
            *output_address++ = *target_address++;
            *output_address   = '\x0';
            if ( str_len( really_address ) > sizeof( source ) - 10 )
            {
              alias_spooler_full( d );
              RETURN_NULL();
            }
          }
        }

        else
        {
          *output_address++ = *target_address++;
          *output_address   = '\x0';
          if ( str_len( really_address ) > sizeof( source ) - 10 )
          {
            alias_spooler_full( d );
            RETURN_NULL();
          }
        }
      }
      expand_alias_spooler( d );
      RETURN_NULL();
    }
  }
  RETURN_NULL();
}

/* �ˬd��J�������R�O�O�_�X�k */
bool check_alias_legal( char * input )
{
  SOCIAL_DATA * social;
  CMD_DATA    * pCommand;

  PUSH_FUNCTION( "check_alias_legal" );

  for ( pCommand = cmd_list; pCommand; pCommand = pCommand->next )
    if ( !str_cmp( input , pCommand->name ) ) RETURN( FALSE );

  for ( social = social_first; social; social = social->next )
    if ( !str_cmp( input , social->name ) )        RETURN( FALSE );

  RETURN( TRUE );
}

/* �������O�ɭP��J�w�İϺ��� */
void alias_spooler_full( DESCRIPTOR_DATA * d )
{
  PUSH_FUNCTION( "alias_spooler_full" );

  if ( !d )
  {
    mudlog( LOG_DEBUG, "alias_spooler_full: �ʥF�ӷ�." );
    RETURN_NULL();
  }

  mudlog( LOG_INFO , "[WARN] %s �������O�Ӫ��ɭP��J�w�İϤw��."
    , d->character->name );

  write_to_descriptor( d, "\n\r�A�������O�Ӫ��ɭP��J�w�İϤw���C\n\r", 0 );

  str_cpy( d->incomm, "quit\0" );
  RETURN_NULL();
}

/* �X�i�������O�� */
void expand_alias_spooler( DESCRIPTOR_DATA * d )
{
  char   arg[ MAX_STRING_LENGTH * 2 ];
  char * source_address;
  char * target_address;

  PUSH_FUNCTION( "expand_alias_spooler" );

  arg[0]         = '\x0';
  target_address = arg;
  source_address = d->incomm;

  while ( *source_address )
  {
    if ( *source_address == ALIAS_SEPRATOR )
    {
      *target_address++ = '\n';
      *target_address++ = '\r';
      *target_address   = '\x0';
    }

    else
    {
      *target_address++ = * source_address;
      *target_address   = '\x0';
    }

    source_address++;

    if ( *source_address >= '\x81'
      && *source_address <= '\xff'
      && *( source_address + 1 ) != '\x0' )
    {
      *target_address++ = *source_address++;
      *target_address   = '\x0';
    }
  }

  str_cat( arg , "\n\r" );
  if ( str_len( arg ) + str_len( d->inbuf ) > sizeof( d->inbuf ) - 10 )
  {
    alias_spooler_full( d );
    RETURN_NULL();
  }

  if ( ++d->alias_repeat > alias_repeat )
  {
    mudlog( LOG_INFO , "[WARN] %s �������O���Ʀ��ƤӦh, �j���_�u.",
      d->character->name );

    write_to_descriptor( d,
      "\n\r�A�@�����楨���M�t�ΰ��w���M�j��A�_�u�T\n\r", 0 );

    str_cpy( d->incomm, "quit\0" );
    RETURN_NULL();
  }

  write_to_descriptor( d, "\e[1;37m�A���������O�}�l�p��B�@�M"
    "�Y�O���J�L�a�j�黰�ֿ�J alias -�C\e[0m\n\r" , 0 );
  str_cat( d->inbuf , arg );
  d->incomm[0] =   ' ';
  d->incomm[1] = '\x0';

  RETURN_NULL();
}

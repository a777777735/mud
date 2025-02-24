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

void    extract_wanted          args( ( WANTED_DATA * ) );

int     WantedThreshold         = 100000;

FUNCTION( do_wanted )
{
  char          arg[MAX_INPUT_LENGTH];
  char          buf[MAX_STRING_LENGTH];
  char          buf1[MAX_STRING_LENGTH];
  CHAR_DATA   * victim;
  WANTED_DATA * pWanted;
  bool          bChange;
  int           reward;
  int           loop;

  PUSH_FUNCTION( "do_wanted" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' || !str_prefix( arg, "!list" ) )
  {
    if ( !wanted_list )
    {
      send_to_char( "�藍�_�M�ثe�S�������a��W��T\n\r", ch );
      RETURN_NULL();
    }

    clear_buffer();
    loop = 0;

    for ( pWanted = wanted_list; pWanted; pWanted = pWanted->next )
    {
      send_to_buffer( "%2d. \e[1;32m%-12s\e[0m �� %13s ��Ȥl"
        "�R \e[1;33m%-12s\e[0m ���W�H�Y\n\r"
        , ++loop
        , pWanted->bountier && *pWanted->bountier ? pWanted->bountier : "����"
        , numberize( pWanted->reward, NULL )
        , pWanted->wanted && *pWanted->wanted ? pWanted->wanted : "����" );

      if ( buffer_full() ) break;
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  else
  {
    if ( !IS_NPC( ch ) && ch->level < level_limit )
    {
      act( "�b�A�ɯŨ�$i�ūe�M�Ф��n�|�B�����T"
        , ch, &level_limit, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch->donate > 0 )
    {
      send_to_char( "�b�A����٪����o�q�ɶ��M�Чԭ@�@�U�a�T\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->trade == FALSE )
    {
      send_to_char( "�A���`�겣�ӥi�ȤF�M�ҥH�L�k����o�ӫ��O�T\n\r", ch );
      RETURN_NULL();
    }

    if ( !( victim = get_char_world( ch, arg ) ) )
    {
      act( "�藍�_�M�䤣��A�n�q�r���H $2$T$0�T", ch, NULL, arg, TO_CHAR );
      RETURN_NULL();
    }

    if ( IS_NPC( victim ) )
    {
      act( "$N���γq�r�M�����_���L�N��F�T", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch == victim )
    {
      send_to_char( "�A���S���d���M�n�R�ۤv���H�Y�S\n\r", ch );
      RETURN_NULL();
    }

    if ( IS_IMMORTAL( victim ) )
    {
      act( "�n��$N�]�O�޲z�̡M��L�@���n�F�T", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( !IS_SET( victim->act, PLR_KILLER )
      && !IS_SET( victim->act, PLR_THIEF  )
      && !IS_SET( victim->act, PLR_BOLTER ) )
    {
      act( "$N�S���ǿ��M�ҥH�A����q�r�L��T", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    one_argument( argument, arg );
    if ( arg[0] == '\x0' || !is_number( arg ) )
    {
      act( "�A�����h�֨�Ȥl�R$N�����W�H�Y�S", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( ( reward = atoi( arg ) ) < WantedThreshold )
    {
      act( "�A�X�o�ػ����M���|�ܴH�ĶܡS�ܤ֭n$i������աT"
        , ch, &WantedThreshold, NULL, TO_CHAR );

      RETURN_NULL();
    }

    if ( ch->gold < reward )
    {
      send_to_char( "�ڪ��D�A�����ߤ��M���o�]�����ӨϪ������i�T\n\r", ch );
      RETURN_NULL();
    }

    gold_from_char( ch, reward );

    bChange = FALSE;
    for ( pWanted = wanted_list; pWanted; pWanted = pWanted->next )
    {
      if ( !str_cmp( pWanted->wanted  , victim->name )
        && !str_cmp( pWanted->bountier, ch->name ) )
      {
        if ( pWanted->reward + reward > 0 ) pWanted->reward += reward;
        bChange = TRUE;
        break;
      }
    }

    if ( !bChange )
    {
      pWanted = alloc_struct( STRUCT_WANTED_DATA );
      pWanted->wanted   = str_dup( victim->name );
      pWanted->bountier = str_dup( ch->name );
      pWanted->reward   = reward;
      pWanted->next     = wanted_list;
      wanted_list       = pWanted;
    }

    chinese_number( reward, buf );
    act( "$n��F$t��Ȥl�R$N�����W�H�Y�T", ch, buf, victim, TO_CHAR );

    sprintf( buf1, "%s\e[0m(%s)��F%s��Ȥl�ӶR%s\e[0m(%s)�����W�H�Y�T"
      , ch->cname, ch->name, buf, victim->cname, victim->name );

    talk_channel_2( buf1, CHANNEL_BULLETIN, "�a��" );
    act( "�A�Q$N�a��F�M�ЧA�����p�ߤ@�I�a�T", victim, NULL, ch, TO_CHAR );

    RETURN_NULL();
  }

  RETURN_NULL();
}

void check_wanted( CHAR_DATA * ch, CHAR_DATA * victim )
{
  WANTED_DATA * pWanted;
  WANTED_DATA * zWanted;
  int           reward;

  PUSH_FUNCTION( "check_wanted" );

  if ( !ch || !victim )
  {
    mudlog( LOG_DEBUG, "check_wanted: �ʥF�ӷ�." );
    RETURN_NULL();
  }

  if ( IS_NPC( victim ) ) RETURN_NULL();

  if ( over_scale( ch ) )
  {
    send_to_char( "�]���A���`�겣�w�F�W���M�ҥH�S����b����T\n\r", ch );
    RETURN_NULL();
  }

  clear_buffer();
  for ( reward = 0, pWanted = wanted_list; pWanted; pWanted = zWanted )
  {
    zWanted = pWanted->next;
    if ( !str_cmp( pWanted->wanted, victim->name ) )
    {
      send_to_buffer( "�]���A�����F%s�M�ҥH%s���A"
        "�@�������M����%d��Ȥl�H���S�¡T\n\r"
        , mob_name( NULL, victim ), pWanted->bountier, pWanted->reward );

      reward += pWanted->reward;
      extract_wanted( pWanted );
    }
  }

  if ( reward <= 0 || IS_NPC( ch ) || ch == victim )
  {
    clear_buffer();
    RETURN_NULL();
  }

  gold_to_char( ch, reward );
  print_buffer( ch );
  RETURN_NULL();
}

void extract_wanted( WANTED_DATA * pWanted )
{
  WANTED_DATA * prev;
  WANTED_DATA * temp;

  PUSH_FUNCTION( "extract_wanted" );

  prev = NULL;
  for ( temp = wanted_list; temp; prev = temp, temp = temp->next )
  {
    if ( temp == pWanted )
    {
      if ( !prev ) wanted_list = wanted_list->next;
      else         prev->next  = temp->next;

      free_string( pWanted->bountier );
      free_string( pWanted->wanted   );
      free_struct( temp , STRUCT_WANTED_DATA );
      RETURN_NULL();
    }
  }

  mudlog( LOG_DEBUG, "extract_wanted: ���o�{����R�������c." );
  RETURN_NULL();
}

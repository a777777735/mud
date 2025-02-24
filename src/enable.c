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

#define MAX_ENABLE_COUNT        10

extern bool gain_skill_exp args( ( CHAR_DATA *, int, bool ) );

int  max_enable = MAX_ENABLE_COUNT;

void driver_dodge  args( ( SKILL_DATA *, CHAR_DATA *, CHAR_DATA * ) );

/* �]�w�w�]�ޯ� */
FUNCTION( do_enable )
{
  int              number;
  int              adept;
  char             arg[MAX_INPUT_LENGTH];
  char             buf[MAX_STRING_LENGTH];
  ENABLE_DATA    * pEnable;
  ENABLE_DATA    * zEnable;
  ENABLE_DATA    * pFight;
  ENABLE_DATA    * pDodge;
  ENABLE_DATA    * pFlee;
  ENABLE_DATA    * pMed;
  MOB_INDEX_DATA * pIndex;
  SKILL_DATA     * pSkill;
  CHAR_DATA      * victim;

  PUSH_FUNCTION( "do_enable" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  argument = one_argument( argument , arg );

  if ( !arg[0] || ( arg[0] == '@' && IS_IMMORTAL( ch ) ) )
  {
    if ( arg[0] == '@' && arg[1] == '\x0' )
    {
      send_to_char( "�A���������A�n�[�ݪ���H�T\n\r", ch );
      RETURN_NULL();
    }

    if ( !arg[0] )
    {
      victim = ch;
    }
    else
    {
      if ( !( victim = get_char_world( ch, arg + 1 ) ) )
      {
        act( "�S�����A����H$2$T$0�C", ch, NULL, arg+1, TO_CHAR );
        RETURN_NULL();
      }

      if ( get_trust( ch ) < get_trust( victim ) )
      {
        act( "�A���v�O������$N���P��C", ch, NULL, victim, TO_CHAR );
        RETURN_NULL();
      }
    }

    if ( !victim->enable )
    {
      act( "$N��������w�]���ޯ�C", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    clear_buffer();
    number = 0;

    pFight = exert_fight_enable( victim, NULL, FALSE );
    pDodge = exert_dodge_enable( victim, NULL, FALSE );

    pFlee = NULL;
    for ( pEnable = victim->enable; pEnable; pEnable = pEnable->next )
    {
      if ( ( pSkill = pEnable->skill )
        && pSkill->type == TAR_FLEE
        && check_skill_cast( victim, pSkill, FALSE ) )
      {
        pFlee = pEnable;
        break;
      }
    }

    pMed = NULL;
    for ( pEnable = victim->enable; pEnable; pEnable = pEnable->next )
    {
      if ( ( pSkill = pEnable->skill ) && pSkill->type == TAR_MEDITATION )
      {
        pMed = pEnable;
        break;
      }
    }

    send_to_buffer( "%s���w�]�ޯ঳ %d �ӡR\n\r%s"
      , mob_name( ch, victim ), enable_count( victim )
      , VERTICAL_LINE );

    for ( pEnable = victim->enable; pEnable; pEnable = pEnable->next )
    {
      if ( ( pSkill = pEnable->skill ) )
      {
        adept = victim->skill[pSkill->slot];

        send_to_buffer(
          "[%s%s%s%s  %3d ] \e[1;32m%-18s \e[1;33m%-19s "
          "\e[1;35m%-12s\e[0m %s"
          , pFight == pEnable ? "\e[1;32m*\e[0m" : " "
          , pDodge == pEnable ? "\e[1;34m*\e[0m" : " "
          , pFlee  == pEnable ? "\e[1;33m*\e[0m" : " "
          , pMed   == pEnable ? "\e[1;37m*\e[0m" : " "
          , ++number , pSkill->cname , pSkill->name
          , skill_type( pSkill->type )
          , adeptation_name( adept ) );

        if ( SkillValue || IS_IMMORTAL( ch ) )
          send_to_buffer( "  %3d%%"  , adept );

        send_to_buffer( "\n\r" );
      }
    }

    if ( number == 0 )
    {
      send_to_buffer( "��������w�]���ޯ�C\n\r" );
    }
    else
    {
      send_to_buffer( "%s", VERTICAL_LINE );

      send_to_buffer( "�����w�]�ޯ�R%-20s %-20s\n\r"
        , ( pFight && ( pSkill = pFight->skill ) )
          ? pSkill->cname : "\e[1;31m�q��\e[0m"
        , ( pFight && ( pSkill = pFight->skill ) )
          ? pSkill->name : "" );

      send_to_buffer( "�{���w�]�ޯ�R%-20s %-20s\n\r"
        , ( pDodge && ( pSkill = pDodge->skill ) )
          ? pSkill->cname : "\e[1;31m�q��\e[0m"
        , ( pDodge && ( pSkill = pDodge->skill ) )
          ? pSkill->name : "" );

      send_to_buffer( "�k�]�w�]�ޯ�R%-20s %-20s\n\r"
        , ( pFlee && ( pSkill = pFlee->skill ) )
          ? pSkill->cname : "\e[1;31m�q��\e[0m"
        , ( pFlee && ( pSkill = pFlee->skill ) )
          ? pSkill->name : "" );

      send_to_buffer( "�߷Q�w�]�ޯ�R%-20s %-20s\n\r"
        , ( pMed && ( pSkill = pMed->skill ) )
          ? pSkill->cname : "\e[1;31m�q��\e[0m"
        , ( pMed && ( pSkill = pMed->skill ) )
          ? pSkill->name : "" );
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  if ( arg[0] == '#' )
  {
    if ( arg[1] == '\x0' || !is_number( arg + 1 ) )
    {
      send_to_char( "�A�������������Ǫ������X�T\n\r", ch );
      RETURN_NULL();
    }

    number = atoi( arg + 1 );
    if ( !( pIndex = get_mob_index( number ) ) )
    {
      act( "�䤣��s�� $x ���Ǫ��T", ch, &number, NULL, TO_CHAR );
      RETURN_NULL();
    }

    argument = one_argument( argument, arg );
    if ( arg[0] == '\x0' )
    {
      send_to_char( "�A���������ޯ઺�^��W�١T\n\r", ch );
      RETURN_NULL();
    }

    if ( !( pSkill = skill_isname( arg ) ) )
    {
      act( "�䤣�� $t �o�اޯ�W�١T", ch, arg, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( !pSkill->enable )
    {
      act( "$w�O���i�H�Q�P�઺�T", ch, pSkill, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( argument[0] == '\x0' || !is_number( argument ) )
    {
      send_to_char( "�ޯ���m�ץ����O�@�ӼƦr�T\n\r", ch );
      RETURN_NULL();
    }

    adept = atoi( argument );
    if ( adept < 0 || adept > 100 )
    {
      send_to_char( "�ޯ઺���m�ץ����b 0 �� 100 �����T\n\r", ch );
      RETURN_NULL();
    }

    if ( adept == 0 )
    {
      for ( zEnable = NULL, pEnable = pIndex->enable; pEnable;
            zEnable = pEnable, pEnable = pEnable->next )
      {
        if ( pEnable->skill == pSkill )
        {
          if ( !zEnable ) pIndex->enable = pIndex->enable->next;
          else            zEnable->next   = pEnable->next;

          free_struct( pEnable , STRUCT_ENABLE_DATA );

          act( "�A�����F$t��$W�ޯ���m�סT"
            , ch, mob_index_name( NULL, pIndex ), pSkill, TO_CHAR );

          for ( victim = char_list; victim; victim = victim->next )
          {
            if ( verify_char( victim ) && victim->pIndexData == pIndex )
            {
              victim->skill[pSkill->slot] = 0;
              victim->enable = pIndex->enable;
            }
          }

          RETURN_NULL();
        }
      }
      act( "$w���ӴN�S�Q�P��M�ҥH�L�k�����T", ch, pSkill, NULL, TO_CHAR );
      RETURN_NULL();
    }

    else
    {
      for ( pEnable = pIndex->enable; pEnable; pEnable = pEnable->next )
      {
        if ( pEnable->skill == pSkill )
        {
          pIndex->skill[pSkill->slot] = adept;

          sprintf( buf, "�A��%s��$w�ޯ���m�ק��ܬ��ʤ���$I�T"
            , mob_index_name( NULL, pIndex ) );

          act( smash_act_keyword( buf, "neswIX" ), ch, pSkill, &adept, TO_CHAR );

          for ( victim = char_list; victim; victim = victim->next )
          {
            if ( verify_char( victim ) && victim->pIndexData == pIndex )
              victim->skill[pSkill->slot] = adept;
          }

          RETURN_NULL();
        }
      }

      /* �]�w���m�� */
      pIndex->skill[pSkill->slot] = adept;

      /* �p�G�s�b�~�t�m�O���鵹�L */
      pEnable        = alloc_struct( STRUCT_ENABLE_DATA );
      pEnable->skill = pSkill;
      pEnable->next  = NULL;

      /* ��X�̫�@�� */
      for ( zEnable = pIndex->enable;
            zEnable && zEnable->next;
            zEnable = zEnable->next );

      if ( !zEnable ) pIndex->enable = pEnable;
      else            zEnable->next  = pEnable;

      sprintf( buf, "�A��%s��$w�ޯ���m�׳]�w���ʤ���$I�T"
        , mob_index_name( NULL, pIndex ) );

      act( smash_act_keyword( buf, "neswIX" ), ch, pSkill, &adept, TO_CHAR );

      for ( victim = char_list; victim; victim = victim->next )
      {
        if ( verify_char( victim ) && victim->pIndexData == pIndex )
        {
          victim->skill[pSkill->slot] = adept;
          victim->enable = pIndex->enable;
        }
      }

      RETURN_NULL();
    }
  }

  /* �M���Ҧ��w�]���ޯ� */
  if ( !str_prefix( arg , "!clean" ) )
  {
    send_to_char( "�t�Τw�g���A�M���Ҧ����w�]�ޯ�C\n\r" , ch );
    extract_enable( ch );
    RETURN_NULL();
  }

  if ( !( pSkill = skill_isname( arg ) ) )
  {
    act( "�S���o�{�A�n���ޯ� $2$T$0�C" , ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  if ( ch->skill[pSkill->slot] <= 0 )
  {
    act( "�A�٤��|$w��M�ҥH�L�k�P��", ch, pSkill, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( ( pEnable = enable_repeat( ch->enable , pSkill ) ) )
  {
    act( "�A�����F$w�o�ӹw�]�ޯ�C", ch, pSkill, NULL, TO_CHAR );
    extract_an_enable( ch , pSkill );
    RETURN_NULL();
  }

  if ( !pSkill->enable )
  {
    act( "�藍�_�M$w�O����Q�P��C" , ch, pSkill, NULL, TO_CHAR );
    RETURN_NULL();
  }

  /* �ˬd�w�]�ޯ઺�ӼƬO�_�W�L�w�]�� */
  if ( !IS_IMMORTAL( ch ) && enable_count( ch ) > max_enable )
  {
    act( "�藍�_�M�C�ӤH�̦h�u�঳$i�ӹw�]�ޯ�C"
      , ch, &max_enable, NULL, TO_CHAR );

    RETURN_NULL();
  }

  set_enable( ch, pSkill );
  act( "�A�w�]���ޯ�]�w�� $w�C", ch, pSkill, NULL, TO_CHAR );

  RETURN_NULL();
}

/* �ˬd�Y�ӹw�]�ޯ�O�_���ƤF */
ENABLE_DATA * enable_repeat( ENABLE_DATA * pEnable , SKILL_DATA * pSkill )
{
  PUSH_FUNCTION( "enable_repeat" );

  for ( ; pEnable; pEnable = pEnable->next )
    if ( pEnable->skill == pSkill ) RETURN( pEnable );

  RETURN( NULL );
}

/* �p��Y�H���w�]�ޯ�Ӽ� */
size_t enable_count( CHAR_DATA * ch )
{
  ENABLE_DATA * enable;
  size_t        count = 0 ;

  PUSH_FUNCTION( "enable_count" );
  for ( enable = ch->enable; enable; enable = enable->next, count++);
  RETURN( count );
}

/* �P����@���w�]�ޯ� */
void extract_an_enable( CHAR_DATA * ch , SKILL_DATA * pSkill )
{
  ENABLE_DATA * enable;
  ENABLE_DATA * prev = NULL;

  PUSH_FUNCTION( "extract_an_enable" );

  for ( enable = ch->enable; enable; prev = enable, enable = enable->next )
  {
    if ( enable->skill == pSkill )
    {
      if ( !prev ) ch->enable = ch->enable->next;
      else         prev->next = enable->next;

      free_struct( enable , STRUCT_ENABLE_DATA );
      RETURN_NULL();
    }
  }

  mudlog( LOG_DEBUG , "extract_an_enable: ���o�{����P�����P����." );
  RETURN_NULL();
}

/* �P���Ҧ����w�]�ޯ� */
void extract_enable( CHAR_DATA * ch )
{
  ENABLE_DATA * enable;
  ENABLE_DATA * temp;

  PUSH_FUNCTION( "extract_enable" );

  for ( enable = ch->enable; enable; enable = temp )
  {
    temp = enable->next;

    /* ���񵲺c�O���� */
    free_struct( enable , STRUCT_ENABLE_DATA );
  }

  ch->enable = NULL;
  RETURN_NULL();
}

ENABLE_DATA * exert_fight_enable( CHAR_DATA * ch , CHAR_DATA * victim, bool bKill )
{
  ENABLE_DATA * pEnable;
  SKILL_DATA  * pSkill;
  char          buffer[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "exert_fight_enable" );

  if ( !ch || !verify_char( ch ) )
  {
    mudlog( LOG_DEBUG, "exert_fight_enable: �ӷ��O�Ū�" );
    RETURN( NULL );
  }

  if ( !can_damage( ch ) )
  {
    mudlog( LOG_DEBUG, "exert_fight_enable: �H���w���`." );
    RETURN( NULL );
  }

  /* ���a�����O���ݿ�J */
  if ( ch->desc && ch->desc->inbuf[0] ) RETURN( NULL );

  if ( !ch->in_room )
  {
    mudlog( LOG_DEBUG, "exert_fight_enable: �ʥF�H���ж�." );
    RETURN( NULL );
  }

  for ( pEnable = ch->enable; pEnable; pEnable = pEnable->next )
  {
    if ( ( pSkill = pEnable->skill )
      && pSkill->enable
      && pSkill->type == TAR_CHAR_OFFENSIVE
      && ( !pSkill->check || ( ( *pSkill->check ) ( ch, FALSE ) ) ) )
    {
      if ( !bKill ) RETURN( pEnable );

      if ( !victim || !verify_char( victim ) )
      {
        mudlog( LOG_DEBUG, "exert_fight_enable: ��H�O�Ū�" );
        RETURN( NULL );
      }

      if ( !can_damage( victim ) )
      {
        mudlog( LOG_DEBUG, "exert_fight_enable: ��H�H���w���`." );
        RETURN( NULL );
      }

      /* �w�g��������H */
      if ( ch->fighting == victim )
      {
        sprintf( buffer, "'%s'", pSkill->name );
      }

      else
      {
        char        name[MAX_INPUT_LENGTH];
        char        buf[MAX_STRING_LENGTH];
        CHAR_DATA * rch;
        int         number;

        one_argument( victim->name, name );
        str_cpy( buf, name );
        number = 0;

        /* ��M get_char_room ���H������ */
        for ( rch = ch->in_room->people; rch; rch = rch->next_in_room )
        {
          if ( can_see( ch, rch ) && is_name( name, rch->name ) ) number++;

          if ( rch == victim )
          {
            if ( number > 1 ) sprintf( buf, "%d.%s", number, name );
            break;
          }
        }

        sprintf( buffer, "'%s' %s" , pSkill->name , buf );
      }

      do_cast( ch, buffer );
      RETURN( pEnable );
    }
  }

  RETURN( NULL );
}

ENABLE_DATA * exert_dodge_enable( CHAR_DATA * ch , CHAR_DATA * victim, bool bDodge )
{
  ENABLE_DATA * pEnable;
  SKILL_DATA  * pSkill;
  int           adept;
  int           cost;
  int           sn;

  PUSH_FUNCTION( "exert_dodge_enable" );

  if ( !ch || !verify_char( ch ) )
  {
    mudlog( LOG_DEBUG, "exert_dodge_enable: �ӷ��O�Ū�" );
    RETURN( NULL );
  }

  if ( !can_damage( ch ) )
  {
    mudlog( LOG_DEBUG, "exert_dodge_enable: �H���w���`." );
    RETURN( NULL );
  }

  for ( pEnable = ch->enable; pEnable; pEnable = pEnable->next )
  {
    if ( ( pSkill = pEnable->skill )
      && pSkill->enable
      && pSkill->type == TAR_DODGE
      && ( IS_NPC( ch ) || check_skill_cast( ch, pSkill, FALSE ) ) )
    {
      /* �p����Ӫ��ƭȶq */
      cost   = pSkill->cost;
      adept  = UMIN( 100, UMAX( ch->skill[pSkill->slot], 1 ) );
      cost   = UMAX( 1, UMAX( cost / 4, cost * adept / 100 ) );

      switch( pSkill->cost_type )
      {
      default:
        mudlog( LOG_DEBUG, "exert_dodge_enable: ���~���A %d."
          , pSkill->cost_type );

        RETURN( NULL );

      /* ���ӥͩR�O */
      case COST_HIT:
        if ( ch->hit < cost ) RETURN( NULL );
        if ( bDodge ) ch->hit -= cost;
        break;

      /* ���Ӫk�O */
      case COST_MANA:

        if ( ch->mana < cost ) RETURN( NULL );
        if ( bDodge ) ch->mana -= cost;
        break;

      /* ������O */
      case COST_MOVE:

        if ( ch->move < cost ) RETURN( NULL );
        if ( bDodge ) ch->move -= cost;
        break;

      /* ���Ӫ��� */
      case COST_GOLD:

        if ( ch->gold < cost ) RETURN( NULL );
        if ( bDodge ) gold_from_char( ch, cost );
        break;
      }

       if ( !bDodge ) RETURN( pEnable );

       if ( !victim || !verify_char( victim ) )
       {
         mudlog( LOG_DEBUG, "exert_dodge_enable: ��H�O�Ū�" );
         RETURN( NULL );
       }

      if ( !can_damage( victim ) )
      {
        mudlog( LOG_DEBUG, "exert_dodge_enable: ��H�w���`." );
        RETURN( NULL );
      }

      sn = pSkill->slot;

      if ( pSkill->damage )
      {
        driver_dodge( pSkill, ch, victim );
      }

      else
      {
        if ( !pSkill->function )
          mudlog( LOG_DEBUG, "exert_dodge_enable: �ޯ� %s �S���ާ@���."
            , pSkill->name );
        else
          ( *pSkill->function ) ( sn, get_practice( ch, sn ), ch, victim );
      }

      /* ��o���m�� */
      if ( pSkill->slot ) gain_skill_exp( ch, pSkill->slot, TRUE );

      RETURN( pEnable );
    }
  }

  RETURN( NULL );
}

void driver_dodge( SKILL_DATA * pSkill, CHAR_DATA * ch, CHAR_DATA * victim )
{
  int           total;
  int           chance;
  int           sn;
  int           practice;
  int           min;
  DAMAGE_DATA * pDam;
  EFFECT_DATA * pEffect;

  PUSH_FUNCTION( "driver_dodge" );

  if ( !pSkill || !ch )
  {
    mudlog( LOG_DEBUG, "driver_dodge: �ӷ��O�Ū�." );
    RETURN_NULL();
  }

  if ( !pSkill->damage )
  {
    mudlog( LOG_DEBUG, "driver_dodge: �ޯ�%s�S����˱ԭz.", pSkill->name );
    RETURN_NULL();
  }

  if ( ( total = pSkill->chance ) <= 0 )
  {
    mudlog( LOG_DEBUG, "driver_dodge: �ޯ�%s����˾��v�`�M�p�󵥩�s."
      , pSkill->name );
    RETURN_NULL();
  }

  practice = UMIN( 100, UMAX( 0, get_practice( ch, sn = pSkill->slot ) ) );

  if ( IS_NPC( ch ) )
  {
    chance = UMIN( number_range( 1, practice * total ), total * 100 );
  }
  else
  {
    min = UMAX( 1, ( UMIN( practice, ch->level / 2 ) * total ) );
    chance = UMIN( number_range( min, practice * total ), total * 100 );
  }

  for ( pDam = pSkill->damage; pDam; pDam = pDam->next )
  {
    if ( ( chance -= pDam->chance * 100 ) <= 0 )
    {
      if ( pSkill->message && *pSkill->message )
        sact( ch, victim, NULL, pSkill->message );

      if ( pDam->description && *pDam->description )
        sact( ch , victim , NULL, pDam->description );

      /* �O�_���S�� */
      if ( pDam->routine )
      {
        ( *pDam->routine ) ( ch, victim, practice );

        /* �ݬݭ�誺�ʧ@�O�_�Ϫ����覺�` */
        if ( !verify_char( ch )
          || !can_damage( ch )
          || !verify_char( victim )
          || !can_damage( victim ) ) RETURN_NULL();
      }

      /* �O�_���S����� */
      for ( pEffect = pDam->effect; pEffect; pEffect = pEffect->next )
      {
        /* �I�s��ƾާ@ */
        effect_driver( ch, victim, pEffect );

        /* �ݬݭ�誺�ʧ@�O�_�Ϫ����覺�` */
        if ( !verify_char( ch )
          || !can_damage( ch )
          || !verify_char( victim )
          || !can_damage( victim ) ) RETURN_NULL();
      }

      ch->dodge += variation( pDam->value, 10 );
      RETURN_NULL();
    }
  }

  mudlog( LOG_DEBUG, "driver_dodge: �䤣��ԭz." );
  RETURN_NULL();
}

void set_enable( CHAR_DATA * ch, SKILL_DATA * pSkill )
{
  ENABLE_DATA * pEnable;
  ENABLE_DATA * zEnable;

  PUSH_FUNCTION( "set_enable" );

  if ( !ch || !pSkill )
  {
    mudlog( LOG_DEBUG, "set_enable: �ӷ����~." );
    RETURN_NULL();
  }

  if ( !pSkill->enable ) RETURN_NULL();

  for ( pEnable = ch->enable; pEnable; pEnable = pEnable->next )
    if ( pEnable->skill == pSkill ) RETURN_NULL();

  /* �t�m�������O���c���O���� */
  pEnable        = alloc_struct( STRUCT_ENABLE_DATA );
  pEnable->skill = pSkill;
  pEnable->next  = NULL;

  /* ��X�̫�@�� */
  for ( zEnable = ch->enable
    ; zEnable && zEnable->next
    ; zEnable = zEnable->next );

  if ( !zEnable ) ch->enable    = pEnable;
  else            zEnable->next = pEnable;

  RETURN_NULL();
}

bool check_skill_enable( CHAR_DATA * ch, SKILL_DATA * pSkill )
{
  ENABLE_DATA * pEnable;

  PUSH_FUNCTION( "check_skill_enable" );

  if ( !ch || !pSkill )
  {
    mudlog( LOG_DEBUG, "check_skill_enable: �ӷ������T." );
    RETURN( FALSE );
  }

  for ( pEnable = ch->enable; pEnable; pEnable = pEnable->next )
    if ( pEnable->skill == pSkill ) RETURN( TRUE );

  RETURN( FALSE );
}

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

void murmur_spell       args( ( CHAR_DATA * , SKILL_DATA * ) );
bool lost_concentration args( ( CHAR_DATA * , int ) );
void driver_kill        args( ( SKILL_DATA *, CHAR_DATA *, CHAR_DATA * ) );
bool learn_expert_skill args( ( CHAR_DATA *, SKILL_DATA * ) );
int  effect_wand        args( ( CHAR_DATA *, SKILL_DATA * ) );

int effect_wand( CHAR_DATA * ch, SKILL_DATA * pSkill )
{
  OBJ_DATA   * pObj;

  PUSH_FUNCTION( "effect_wand" );

  if ( !ch || !pSkill || !verify_char( ch ) ) RETURN( ERRORCODE );

  switch( pSkill->rating )
  {
  case RATING_SKILL:
  case RATING_LOST:
  case RATING_CURE:
  case RATING_SING:
  case RATING_FIGHT:
  case RATING_MURDER:
  case RATING_CREATE:
  case RATING_THIEF:
    RETURN( 100 );

  default:
    break;
  }

  for ( pObj = ch->carrying; pObj; pObj = pObj->next_content )
  {
    if ( pObj->wear_loc == WEAR_WIELD
      && ( pObj->item_type == ITEM_STAFF || pObj->item_type == ITEM_WAND ) )
      break;
  }

  if ( pObj )
  {
    if ( pSkill->rating == pObj->value[4] )
    {
      if ( pObj->value[6] < pObj->value[7] )
      {
        act( "$p���k�O���Ӭp�ɡM�L�k�A���U�A�F�T", ch, pObj, NULL, TO_CHAR );
        extract_obj( pObj );
        RETURN( 100 );
      }

      act( "$3$n��W��$p�o�X�F�@�}ģ�������~�M�H�Y�S��_���`�T$0"
        , ch, pObj, NULL, TO_ALL );

      pObj->value[6] = UMAX( 0, pObj->value[6] - pObj->value[7] );
      RETURN( pObj->value[5] );
    }
    else
    {
      if ( pObj->value[4] == RATING_ALL )
      {
        if ( pObj->value[6] < pObj->value[7] )
        {
          act( "$p���k�O���Ӭp�ɡM�L�k�A���U�A�F�T", ch, pObj, NULL, TO_CHAR );
          extract_obj( pObj );
          RETURN( 100 );
        }

        act( "$3$n��W��$p�o�X�F�@�}ģ�������~�M�H�Y�S��_���`�T$0"
          , ch, pObj, NULL, TO_ALL );

        pObj->value[6] = UMAX( 0, pObj->value[6] - pObj->value[7] );
        RETURN( pObj->value[5] );
      }

      RETURN( 100 );
    }
  }
  else
  {
    RETURN( 100 );
  }

  RETURN( ERRORCODE );
}

FUNCTION( do_llink )
{
  char         arg[MAX_INPUT_LENGTH];
  int          count = 0;
  SKILL_DATA * pSkill;

  PUSH_FUNCTION( "do_llink" );

  one_argument( argument, arg );

  if ( !arg[0] )
  {
    send_to_char( "�A�n�d�߭��@�ӧޯ઺�s���S\n\r", ch );
    RETURN_NULL();
  }

  if ( !( pSkill = skill_isname( arg ) ) )
  {
    act( "�䤣��A�n���k�N�άO�ޯ�W�� $2$T$0�C", ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  if ( !( get_skill( pSkill->associate ) ) )
  {
    act( "$w�S���������s���ޯ�C", ch, pSkill, NULL, TO_CHAR );
    RETURN_NULL();
  }

  for ( clear_buffer(); pSkill; pSkill = get_skill( pSkill->associate ) )
  {
    send_to_buffer( "���ǡR%2d ���v�R%4d%%%% %-20s %-20s\n\r"
      , ++count, pSkill->degree, pSkill->name, pSkill->cname );
  }

  print_buffer( ch );
  RETURN_NULL();
}

FUNCTION( do_cast )
{
  CHAR_DATA     * victim;
  OBJ_DATA      * obj;
  SKILL_DATA    * pSkill;
  RESTRICT_DATA * pRest;
  LIMIT_DATA    * pLimit;
  void          * vo;
  char          * pString;
  char            arg1[MAX_INPUT_LENGTH ];
  char            arg2[MAX_INPUT_LENGTH ];
  int             sn;
  int             cost;
  int             adept;
  int             prac;
  int             prac_point;
  bool            attack;
  bool            gain = TRUE;
  bool            pFind = FALSE;

  PUSH_FUNCTION( "do_cast" );

  /* �p�G�����Ǫ��Q�g�b�h����I�i�k�N�Χޯ� */
  if ( IS_NPC( ch ) && is_affected( ch, SLOT_CHARM_PERSON ) )
    RETURN_NULL();

  if ( !ch->in_room ) RETURN_NULL();

  pString = argument = one_argument( argument , arg1 );
  one_argument( argument , arg2 );

  /* �S�����󪺰Ѽ� */
  if ( !arg1[0] )
  {
    send_to_char( "�A�n�I�i����ޯ�O�S\n\r" , ch );
    RETURN_NULL();
  }

  /* ���ˬd�O�_�i�H�I�i�o�˪k�N */
  pSkill = skill_isname( arg1 );

  if ( !pSkill || ( !IS_NPC( ch ) && ch->skill[pSkill->slot] <= 0 ) )
  {
    send_to_char( "�A�٤��|�o���ޯ�a�T\n\r" , ch );
    RETURN_NULL();
  }

  if ( is_dead( ch ) )
  {
    act( "�A�w�g���F, �L�k�I�i$w.", ch, pSkill, NULL, TO_CHAR );
    RETURN_NULL();
  }

  /* �ޯ�ʧO������ */
  switch( pSkill->sex )
  {
  default:
    break;

  case SEX_FEMALE:
  case SEX_MALE:
  case SEX_NEUTRAL:

    if ( ch->sex != pSkill->sex )
    {
      send_to_char( "�A����ϥΤ��O�A�ʧO���ޯ�T\n\r", ch );
      RETURN_NULL();
    }
    break;
  }

  /* �ˬd�ޯ઺���� */
  for ( pRest = pSkill->restrict; pRest; pRest = pRest->next )
  {
    switch( pRest->type )
    {
    default:
      if ( !check_restrict( ch, pRest, TRUE )
        && !IS_IMMORTAL( ch )
        && !IS_NPC( ch ) )
        RETURN_NULL();
      break;

    case RES_SKILL:
      break;

    case RES_CLASS:
    case RES_TRUST:
    case RES_LEVEL:
    case RES_SEX:
    case RES_NOSKILL:

      if ( !check_restrict( ch, pRest, FALSE )
        && !IS_IMMORTAL( ch )
        && !IS_NPC( ch ) )
      {
        act( "$B$B�A�w�g�����a���$1$w$0�o���ޯ�F�T", ch, pSkill, NULL, TO_CHAR );
        cancel_skill( ch, pSkill );
        RETURN_NULL();
      }
    }
  }

  for ( pLimit = pSkill->limit; pLimit; pLimit = pLimit->next )
  {
    if ( pLimit->class == ch->class )
    {
      pFind = TRUE;

      if ( ( ch->level < pLimit->level
        || ch->skill[ pSkill->slot ] > pLimit->adept )
        && !IS_IMMORTAL( ch )
        && !IS_NPC( ch ) )
      {
        act( "�A�L�k�ϥΦp���}�`��$w�T", ch, pSkill, NULL, TO_CHAR );
        RETURN_NULL();
      }
      break;
    }
  }

  if ( pFind == FALSE && !IS_IMMORTAL( ch ) && !IS_NPC( ch ) )
  {
    act( "$w���A�X�A�ثe��¾�~�C", ch, pSkill, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( !check_valid_rating( ch, pSkill, TRUE ) ) RETURN_NULL();

  switch( pSkill->rating )
  {
  default: break;

  case RATING_SAINT:
    if ( !IS_GOOD( ch ) && !IS_IMMORTAL( ch ) && !IS_NPC( ch ) )
    {
      act( "�A�Ө��c�F�M�L�k�ϥX$w�T", ch, pSkill, NULL, TO_CHAR );
      RETURN_NULL();
    }
    break;

  case RATING_EVIL:
    if ( !IS_EVIL( ch ) && !IS_IMMORTAL( ch ) && !IS_NPC( ch ) )
    {
      act( "�A���ߦa�ӵ��}�M�L�k�ϥX$w�T", ch, pSkill, NULL, TO_CHAR );
      RETURN_NULL();
    }
    break;

  }

  if ( pSkill->cast == FALSE )
  {
    act( "$w�O�L�k�γo�ؤ�k�I�i�C", ch, pSkill, NULL, TO_CHAR );
    RETURN_NULL();
  }

  /* �������X�z�����A�~��I�i�k�N */
  if ( ch->position < pSkill->position )
  {
    act( "�A�L�k�M�߬I�i$w�C", ch, pSkill, NULL, TO_CHAR );
    RETURN_NULL();
  }

  /* �w��ؼ� */
  victim = NULL;
  obj    = NULL;
  vo     = NULL;
  sn     = pSkill->slot;

  /* �p����Ӫ��ƭȶq */
  cost   = pSkill->cost;
  adept  = UMIN( 100, UMAX( ch->skill[sn], 1 ) );
  cost   = UMAX( 1, UMAX( cost / 4, cost * adept / 100 ) );
  attack = TRUE;

  /* �ˬd�O�_�ݭn�ĤG�ӰѼ� */
  switch ( pSkill->type )
  {
  default :

    mudlog( LOG_DEBUG , "Do_cast: �k�N%s�����A���~", pSkill->name );
    RETURN_NULL();

  /* ���ݭn�ؼ� */
  case TAR_IGNORE:
    break;

  /* �����ʪk�N */
  case TAR_CHAR_OFFENSIVE:

    if ( !arg2[0] )
    {
      if ( !( victim = ch->fighting ) )
      {
        act( "�n��֬I�i$w�O�S", ch, pSkill, NULL, TO_CHAR );
        RETURN_NULL();
      }
    }

    else
    {
      if ( !( victim = get_char_room( ch, arg2 ) ) )
      {
        act( "�A����H $2$t$0 �ä��b�o�̡C", ch, arg2, NULL, TO_CHAR );
        RETURN_NULL();
      }
      else
      {
        if ( victim == ch )
        {
          send_to_char( "�H�͵u�u�ƤQ�~�M�󥲦۵��T\n\r" , ch );
          RETURN_NULL();
        }
      }
    }

    if ( !can_damage( victim ) )
    {
      act( "�A�L�k��$N�A�i������F.", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( is_pk( ch ) && is_pk( victim ) )
    {
      if ( fight_info->status != PK_FIGHT )
      {
        send_to_char( "�b�o�q�ɶ��M�٬O�n�n�a�𮧤@�U�a�T\n\r", ch );
        RETURN_NULL();
      }
    }

    /* �ˬd�O�_�i�H�o�;԰� */
    if ( !can_fight( ch , victim ) ) RETURN_NULL();

    vo = ( void * ) victim;
    break;

  /* ���m�ʪk�N */
  case TAR_CHAR_DEFENSIVE:

    /* �p�G�S���Ѽƹ�H�N�O�ۤv */
    if ( !arg2[0] )
    {
      victim = ch;
    }

    else
    {
      if ( !( victim = get_char_room( ch, arg2 ) ) )
      {
        act( "�䤣��A����H $2$T$0�T", ch, NULL, arg2, TO_CHAR );
        RETURN_NULL();
      }
    }

    if ( !can_damage( victim ) )
    {
      act( "�A�L�k��$N�I�i$w.", ch, pSkill, victim, TO_CHAR );
      RETURN_NULL();
    }

    vo = ( void * ) victim;
    break;

  /* �u��I�i�b�ۤv���W���k�N */
  case TAR_CHAR_SELF:

    if ( arg2[0] && !is_name( arg2, ch->name ) )
    {
      act( "$w�u��I�i�b�ۤv���W�C", ch, pSkill, NULL, TO_CHAR );
      RETURN_NULL();
    }

    vo = ( void * ) ch;
    break;

  /* �I�i�b�ۤv����a���Z���W */
  case TAR_OBJ_INV:

    /* �@�w�n���Ѽ�, �o�ӰѼƬO�˳ƪ��W�r */
    if ( !arg2[0] )
    {
      act( "�A�n�I�i$w��A������˳ƤW�O�S", ch, pSkill, NULL, TO_CHAR );
      RETURN_NULL();
    }

    /* �ݬݬO�_����a�o�˪��Z�� */
    if ( !( obj = get_obj_carry( ch, arg2 ) ) )
    {
      act( "�A�èS����a���~ $2$T$0�C", ch, NULL, arg2, TO_CHAR );
      RETURN_NULL();
    }

    vo = ( void * ) obj;
    break;

  /* �{���ޯ�άO���s�� */
  case TAR_DODGE:

    act( "$w�u��P��ɡM�۰ʬI�i�T", ch, pSkill, NULL, TO_CHAR );
    RETURN_NULL();

  case TAR_ARGUMENT:

    if ( !pString || !*pString )
    {
      act( "�藍�_�M$w�᭱�����[�J�ѼơT", ch, pSkill, NULL, TO_CHAR );
      RETURN_NULL();
    }

    vo = ( char * ) pString;
    break;

  case TAR_STRING:

    if ( arg2[0] == '\x0' )
    {
      act( "�藍�_�M$w�᭱�����[�J�ѼơC", ch, pSkill, NULL, TO_CHAR );
      RETURN_NULL();
    }

    vo = ( char * ) arg2;
    break;

  /* �k�]�ޯ� */
  case TAR_FLEE:

    if ( !ch->fighting )
    {
      send_to_char( "�A�S���b�԰�����n�k�]�S\n\r", ch );
      RETURN_NULL();
    }
    break;

  /* ���� */
  case TAR_NO_CAST:

    act( "$w�O�L�k�γo�ؤ覡�I�i�X�ӡC", ch, pSkill, NULL, TO_CHAR );
    RETURN_NULL();

  /* �M�N */
  case TAR_MOUNT:

    act( "$w�O��A�M���ɡM�~�|�۰ʨϥΡT", ch, pSkill, NULL, TO_CHAR );
    RETURN_NULL();

  /* �߷Q�� */
  case TAR_MEDITATION:

    act( "$w�O��b�A��ı�ɡM�|�۰ʬI�i�T", ch, pSkill, NULL, TO_CHAR );
    RETURN_NULL();

  case TAR_OBJ_ROOM:

    /* �@�w�n���Ѽ�, �o�ӰѼƬO�˳ƪ��W�r */
    if ( !arg2[0] || !ch->in_room )
    {
      act( "�A�n�I�i$w��a�W���򪫫~�W�O�S", ch, pSkill, NULL, TO_CHAR );
      RETURN_NULL();
    }

    /* �ݬݬO�_����a�o�˪��Z�� */
    if ( !( obj = get_obj_list( ch, arg2, ch->in_room->contents ) ) )
    {
      act( "�o�̨èS�� $2$T$0 �C�T", ch, NULL, arg2, TO_CHAR );
      RETURN_NULL();
    }

    vo = ( void * ) obj;
    break;
  }

  /* �ˬd�O�_���������k�O�άO��O�άO��L....*/
  switch( pSkill->cost_type )
  {
  default :

    mudlog( LOG_DEBUG , "do_cast: �k�N %s ���ӷl���A���~", pSkill->name );
    RETURN_NULL();

  /* ���ӥͩR�O */
  case COST_HIT:

    if ( ch->hit < cost )
    {
      act( "�A���ͩR�O�ӧC�M�L�k�I�i$w�C", ch, pSkill, NULL, TO_CHAR );
      RETURN_NULL();
    }

    break;

  /* ���Ӫk�O */
  case COST_MANA:

    if ( ch->mana < cost )
    {
      act( "�A�S���������k�O�ӬI�i$w�C", ch, pSkill, NULL, TO_CHAR );
      RETURN_NULL();
    }

    break;

  /* ������O */
  case COST_MOVE:

    if ( ch->move < cost )
    {
      act( "�A����O���١M�L�k�I�i$w�C", ch, pSkill, NULL, TO_CHAR );
      RETURN_NULL();
    }

    break;

  /* ���Ӫ��� */
  case COST_GOLD:

    if ( ch->gold < cost )
    {
      act( "�A�S���������Ȩ�M�L�k�I�i$w�C", ch, pSkill, NULL, TO_CHAR );
      RETURN_NULL();
    }

    break;
  }

  /* �ݬݬO�_�ݭn��X�G�y */
  if ( pSkill->say_spell ) murmur_spell( ch , pSkill );

  /* �k�N�άO�ޯ઺���ݮɶ� */
  WAIT_STATE( ch, pSkill->wait );

  /* �ˬd�O�_���N�Ӥ����������� */
  if ( pSkill->concentration == TRUE && lost_concentration( ch, sn ) == TRUE )
  {
    cost   = UMAX( 1, cost / 2 );
    attack = FALSE;
  }

  /* �����ƭ� */
  switch( pSkill->cost_type )
  {
  default :
    mudlog( LOG_DEBUG , "Do_cast: �k�N%s���ӷl���A���~.", pSkill->name );
    RETURN_NULL();

  /* ���ӥͩR�O */
  case COST_HIT:
    ch->hit = UMAX( 1, ch->hit - cost );
    break;

  /* ���Ӫk�O */
  case COST_MANA:
    ch->mana = UMAX( 0, ch->mana - cost );
    break;

  /* ������O */
  case COST_MOVE:
    ch->move = UMAX( 0, ch->move - cost );
    break;

  /* ���Ӫ��� */
  case COST_GOLD:
    gold_from_char( ch, cost );
    break;
  }

  /* �ˬd�O�_�i�H�I�k */
  if ( pSkill->check && ( ( *pSkill->check ) ( ch, TRUE ) ) == FALSE )
    RETURN_NULL();

  /* �N�Ӥ��������ɭ� */
  if ( attack == FALSE )
  {
    act( "�A���믫�N�Ӥ������M$w�L�k�I�i�C", ch, pSkill, NULL, TO_CHAR );
  }

  /* �u���n�I�i�k�N */
  else
  {
    /* �O�_���T�w�Ҧ� */
    if ( pSkill->damage )
    {
      switch ( pSkill->type )
      {
      default:
        mudlog( LOG_DEBUG, "%s ���S���T�w�Ҧ�(%d)."
          , pSkill->name, pSkill->type );
        break;

      /* �����ʪk�N */
      case TAR_CHAR_OFFENSIVE:
        driver_kill( pSkill, ch, ( CHAR_DATA * ) vo );
        break;

      /* �k�] */
      case TAR_FLEE:

        driver_flee( pSkill, ch );
        break;
      }
    }

    /* �S���h�I�s��ƨӹB�� */
    else
    {
      if ( !pSkill->function )
      {
        mudlog( LOG_DEBUG, "do_cast: �ޯ� %s �S���ާ@���.", pSkill->name );
        RETURN_NULL();
      }

      else
      {
        if ( ( prac = effect_wand( ch, pSkill ) ) == ERRORCODE )
        {
          mudlog( LOG_DEBUG, "do_cast: �L�k��o�k���[��." );
          prac = 100;
        }

        prac_point = ( get_practice( ch, sn ) * prac ) / 100;
        prac_point = number_range( get_practice( ch, sn ), prac_point );
        prac_point = UMAX( 1, UMIN( prac_point, MAX_LEVEL ) );
        gain = ( *pSkill->function ) ( sn, prac_point, ch, vo );
      }
    }
  }

  /* ��o�ޯ઺���m�� */
  if ( gain ) gain_skill_exp( ch , sn , attack );

  /* �Y�䤤���@�̦��`, �h�����U�C�ʧ@ */
  if ( !can_damage( ch ) || !can_damage( victim ) ) RETURN_NULL();

  /* ���n���D�԰��F, �i�H�X�ۤF, ���O��褣��O�ۤv�άO�ۤv������ */
  if ( pSkill->type == TAR_CHAR_OFFENSIVE
    && victim->master != ch
    && victim != ch
    && ch->in_room )
  {
    CHAR_DATA * vch;
    CHAR_DATA * vch_next;

    for ( vch = ch->in_room->people; vch; vch = vch_next )
    {
      vch_next = vch->next_in_room;

      if ( victim == vch && !victim->fighting && can_damage( victim ) )
      {
        striking( victim, ch, TYPE_UNDEFINED );
        break;
      }
    }
  }

  RETURN_NULL();
}

void driver_kill( SKILL_DATA * pSkill, CHAR_DATA * ch, CHAR_DATA * victim )
{
  int           total;
  int           chance;
  int           sn;
  int           practice;
  int           value;
  int           multi;
  int           min;
  DAMAGE_DATA * pDam;
  EFFECT_DATA * pEffect;
  OBJ_DATA    * pObj;

  PUSH_FUNCTION( "driver_kill" );

  if ( !pSkill || !ch || !victim || !verify_char( ch )
    || !verify_char( victim ) )
  {
    mudlog( LOG_DEBUG, "driver_kill: �ӷ��O�Ū�." );
    RETURN_NULL();
  }

  if ( !pSkill->damage )
  {
    mudlog( LOG_DEBUG, "driver_kill: �ޯ� %s �S����˱ԭz.", pSkill->name );
    RETURN_NULL();
  }

  pObj = get_wield( ch );

  if ( pSkill->weapon != ERRORCODE
    && ( !pObj
      || pObj->item_type != ITEM_WEAPON
      || pObj->value[3] != pSkill->weapon ) ) RETURN_NULL();

  if ( ( total = pSkill->chance ) <= 0 )
  {
    mudlog( LOG_DEBUG, "driver_kill: �ޯ�%s����˾��v�`�M�p�󵥩�s."
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
        sact( ch, victim, pObj, pSkill->message );

      if ( pDam->description && *pDam->description )
        sact( ch , victim , pObj, pDam->description );

      /* �O�_���S�� */
      if ( pDam->routine )
      {
        ( *pDam->routine ) ( ch, victim, practice );

        /* �ݬݭ�誺�ʧ@�O�_�Ϫ����覺�` */
        if ( !verify_char( ch )
          || !can_damage( ch )
          || !verify_char( victim )
          || !can_damage( victim )
          || ( pObj && !verify_obj( pObj ) ) )
          RETURN_NULL();
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
          || !can_damage( victim )
          || ( pObj && !verify_obj( pObj ) ) )
          RETURN_NULL();
      }

      value = pDam->value;

      /* �ޯ઺�k���[�� */
      if ( pObj
        && verify_obj( pObj )
        && pDam->obj_vnum == pObj->pIndexData->vnum )
        value = value * pDam->multiple / 100;

      /* �Z�����[�� */
      if ( !IS_NPC( ch )
        && pObj
        && pObj->item_type == ITEM_WEAPON
        && pSkill->weapon == pObj->value[3] )
      {
        pObj->value[1] = UMIN( 1000, pObj->value[1] );
        pObj->value[2] = UMIN( 1000, pObj->value[2] );

        multi = number_range( pObj->value[1], pObj->value[2] );
      }
      else
      {
        multi = 100;
      }

      value = variation( ( value * multi / 100 ), 10 );
      damage( ch, victim, value, sn, pDam->situs );

      RETURN_NULL();
    }
  }

  mudlog( LOG_DEBUG, "driver_kill: �䤣��ԭz." );
  RETURN_NULL();
}

void driver_flee( SKILL_DATA * pSkill, CHAR_DATA * ch )
{
  CHAR_DATA       * victim;
  ROOM_INDEX_DATA * was_in;
  ROOM_INDEX_DATA * now_in;
  EXIT_DATA       * pExit;
  DAMAGE_DATA     * pDam;
  EFFECT_DATA     * pEffect;
  char              buf[MAX_STRING_LENGTH];
  int               total;
  int               chance;
  int               sn;
  int               practice;
  int               door;
  int               attemp;

  PUSH_FUNCTION( "driver_flee" );

  if ( !pSkill || !ch || !( victim = ch->fighting ) )
  {
    mudlog( LOG_DEBUG, "driver_flee: �ӷ��O�Ū�." );
    RETURN_NULL();
  }

  if ( !pSkill->damage )
  {
    mudlog( LOG_DEBUG, "driver_flee: �ޯ�%s�S����˱ԭz.", pSkill->name );
    RETURN_NULL();
  }

  if ( ( total = pSkill->chance ) <= 0 )
  {
    mudlog( LOG_DEBUG, "driver_flee: �ޯ�%s����˾��v�`�M�p�󵥩�s."
      , pSkill->name );
    RETURN_NULL();
  }

  if ( !( was_in = ch->in_room ) )
  {
    mudlog( LOG_DEBUG, "driver_flee: �H���Ҧb�ж����s�b." );
    RETURN_NULL();
  }

  practice = UMIN( 100, UMAX( 0, get_practice( ch, sn = pSkill->slot ) ) );
  chance   = UMIN( number_range( 1, practice * total ), total * 100 );

  for ( pDam = pSkill->damage; pDam; pDam = pDam->next )
  {
    if ( ( chance -= pDam->chance * 100 ) <= 0 )
    {
      if ( pSkill->message ) sact( ch, victim, NULL, pSkill->message );

      if ( pDam->description && *pDam->description )
        sact( ch, victim, NULL, pDam->description );

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

      /* ����X�f */
      for ( attemp = pDam->value; attemp > 0; attemp-- )
      {
        if ( number_range( 0, victim->level - pDam->vicevalue ) > ch->level
          || !( pExit = was_in->exit[ ( door = number_door() ) ] )
          || !pExit->to_room
          || pExit->closed
          || ( IS_NPC( ch )
            && ( pExit->to_room->NoMob
              || ( IS_SET( ch->act, ACT_STAY_AREA )
              && pExit->to_room->area != ch->in_room->area ) ) ) )
          continue;

        move_char( ch, door );
        if ( ( now_in = ch->in_room ) == was_in ) continue;

        /* �y�M�|�g�� */
        if ( ch->mount ) unmount_char( ch, ch->mount );

        /* �ħL�]�|���� */
        if ( ch->mercenary ) all_mercenary_from_char( ch );

        if ( IS_NPC( ch ) && ch->talk_buf )
        {
          free_string( ch->talk_buf );
          ch->talk_buf  = NULL;
          ch->talk_mode = MODE_ABSENT;
        }

        ch->in_room = was_in;
        act( "$n���ġM���Y��«�k�h�T", ch, NULL, NULL, TO_ROOM );
        ch->in_room = now_in;

        if ( !IS_NPC( ch ) )
        {
          send_to_char("�A���Q�q�԰����k���M���L�A�ॢ�F25�I�g��ȡT\n\r", ch );
          gain_exp( ch, -25 );
        }

        if ( is_pk( ch ) && is_pk( victim ) )
        {
          sprintf( buf, "%s�ѪZ���j�|��M���M���M�k�]�T", mob_name( NULL, ch ) );
          talk_channel_2( buf, CHANNEL_PK, "" );
        }

        stop_fighting( ch, TRUE );
        RETURN_NULL();
      }

      send_to_char( "�A�k�]���ѡM�A�ॢ \e[1;33m10\e[0m �I�g��ȡT\n\r", ch );
      gain_exp( ch, -10 );
      RETURN_NULL();
    }
  }

  mudlog( LOG_DEBUG, "driver_flee: �䤣��ԭz." );
  RETURN_NULL();
}

/* ��X�G�y */
void murmur_spell( CHAR_DATA * ch , SKILL_DATA * pSkill )
{
  char         buf [MAX_STRING_LENGTH] = { 0 };
  char         buf1[MAX_STRING_LENGTH];
  CHAR_DATA  * rch;
  char       * pName;
  int          iSyl;
  int          length;

  struct syl_type
  {
    char *  old;
    char *  new;
  };

  static const struct syl_type syl_table[] =
  {
    { " ",          " "             },
    { "ar",         "abra"          },
    { "au",         "kada"          },
    { "bless",      "fido"          },
    { "blind",      "nose"          },
    { "bur",        "mosa"          },
    { "cu",         "judi"          },
    { "de",         "oculo"         },
    { "en",         "unso"          },
    { "light",      "dies"          },
    { "lo",         "hi"            },
    { "mor",        "zak"           },
    { "move",       "sido"          },
    { "ness",       "lacri"         },
    { "ning",       "illa"          },
    { "per",        "duda"          },
    { "ra",         "gru"           },
    { "re",         "candus"        },
    { "son",        "sabru"         },
    { "tect",       "infra"         },
    { "tri",        "cula"          },
    { "ven",        "nofo"          },
    { "a", "a" }, { "b", "b" }, { "c", "q" }, { "d", "e" },
    { "e", "z" }, { "f", "y" }, { "g", "o" }, { "h", "p" },
    { "i", "u" }, { "j", "y" }, { "k", "t" }, { "l", "r" },
    { "m", "w" }, { "n", "i" }, { "o", "a" }, { "p", "s" },
    { "q", "d" }, { "r", "f" }, { "s", "g" }, { "t", "h" },
    { "u", "j" }, { "v", "z" }, { "w", "x" }, { "x", "n" },
    { "y", "l" }, { "z", "k" },
    { "", "" }
  };

  PUSH_FUNCTION( "murmur_spell" );

  if ( !ch->in_room || !pSkill->name|| !*pSkill->name ) RETURN_NULL();

  for ( pName = pSkill->name; *pName; pName += length )
  {
    for ( iSyl = 0; (length = str_len(syl_table[iSyl].old)) != 0; iSyl++ )
    {
      if ( !str_prefix( syl_table[iSyl].old, pName ) )
      {
        str_cat( buf, syl_table[iSyl].new );
        break;
      }
    }
    if ( length == 0 ) length = 1;
  }

  sprintf( buf1, "$n�f����ᦳ�����D: '%s'", buf );
  sprintf( buf,  "$n�f����ᦳ�����D: '%s'", pSkill->name );

  /* �e���ж��̪��C�@�ӤH */
  for ( rch = ch->in_room->people; rch; rch = rch->next_in_room )
  {
    if ( rch != ch )
      act( is_same_class( rch, ch ) ? buf : buf1, ch, NULL, rch, TO_VICT );
  }
  RETURN_NULL();
}

/* �ˬd�O�_�N�Ӥ����� */
bool lost_concentration( CHAR_DATA * ch , int sn )
{
  sh_int     chance;

  PUSH_FUNCTION( "lost_concentration" );

  if ( !ch || sn < 0 || sn >= MAX_SKILL )
  {
    mudlog( LOG_DEBUG, "lost_concentration: �ӷ����X�z." );
    RETURN( TRUE );
  }

  /* �p����v */
  if ( !IS_NPC( ch ) )
    chance = 50 - ( ( ch->skill[sn] + get_curr_wis( ch ) ) / 2 );
  else chance = ( 50 - ( ch->level + get_curr_wis( ch ) ) ) / 2;

  chance = URANGE( 1, chance, 100 );

  /* �S�����h�M�� -- �i�H�o�X�ޯ� */
  if ( number_percent() >= chance ) RETURN( FALSE );

  /* ���M��, �L�k�I�X�ޯ� */
  RETURN( TRUE );
}

/* �ˬd�O�_�i�H���@���m�� */
bool gain_skill_exp( CHAR_DATA * ch , int sn , bool fTrue )
{
  SKILL_DATA * pSkill;
  LIMIT_DATA * pLimit;
  char         buf[MAX_STRING_LENGTH];
  int          chance_base = 10000;
  int          chance      = 100;
  bool         pFind       = FALSE;

  PUSH_FUNCTION( "gain_skill_exp" );

  if ( !ch || sn < 0 || sn >= MAX_SKILL )
  {
    mudlog( LOG_DEBUG, "gain_skill_exp: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  if ( !( pSkill = get_skill( sn ) )
    || ( IS_NPC( ch ) && !ch->boss )
    || fTrue == FALSE
    || !ch->class ) RETURN( FALSE );

  /* ��|��L�Z�\ */
  if ( pSkill->associate != ERRORCODE ) learn_expert_skill( ch, pSkill );

  /* �ޯ�w�� */
  if ( ch->skill[sn] >= 100 ) RETURN( FALSE );

  /* �����]�S�� */
  if ( ch->fighting && ch->fighting->tractable > 0 ) RETURN( FALSE );

  /* �ʧO���� */
  if ( pSkill->sex && pSkill->sex > ERRORCODE && pSkill->sex < MAX_SEX )
    if ( ch->sex != pSkill->sex ) RETURN( FALSE );

  /* �O�_�W�X¾�~������ */
  for ( pLimit = pSkill->limit; pLimit; pLimit = pLimit->next )
  {
    if ( pLimit->class == ch->class )
    {
      pFind = TRUE;
      if ( ch->level < pLimit->level && !IS_NPC( ch ) ) RETURN( FALSE );
      if ( ch->skill[sn] >= pLimit->adept && !IS_NPC( ch ) ) RETURN( FALSE );
      break;
    }
  }

  /* �Y�O�䤣��]����ɼ��m�� */
  if ( pFind == FALSE && !IS_NPC( ch ) ) RETURN( FALSE );

  /* �]�w���v */
  chance_base = 10000 + ( 200 * ( ch->skill[sn] + ch->level ) / 2 );
  chance      = get_curr_wis( ch ) + pSkill->exp;

  /* ���� */
  if ( number_range( 1, chance_base ) >= chance ) RETURN( FALSE );

  /* ��o�@�I���m�� */
  sprintf( buf , "\n\r\e[1;33m�A���V�O�W�m�ש���o�^���F�M�A��%s"
                 "�ޯ���m�׼W�[�F�T\e[0m\n\r\n\r", pSkill->cname );

  act( smash_act_keyword( buf, "nes" ), ch->boss ? ch->boss : ch , NULL, NULL, TO_CHAR );
  ch->skill[sn]++;

  RETURN( TRUE );
}

/* ��o���m�� */
int get_practice( CHAR_DATA * ch , int sn )
{
  PUSH_FUNCTION( "get_practice" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "get_practice: �H���O�Ū�." );
    RETURN( 0 );
  }

  /* �Ǫ����� */
  if ( IS_NPC( ch ) && ch->skill[sn] <= 0 ) RETURN( ch->level );

  /* ���a���� */
  RETURN( ch->skill[sn] );
}

SKILL( skill_sneak )
{
  AFFECT_DATA pAf;

  PUSH_FUNCTION( "skill_sneak" );

  send_to_char( "�A�յۻ����A���}�B�C\n\r", ch );
  affect_release( ch, SLOT_SNEAK );

  pAf.type      = sn;
  pAf.duration  = level;
  pAf.location  = APPLY_NONE;
  pAf.modifier  = 0;
  pAf.level     = ch->level;
  affect_to_char( ch, &pAf );

  RETURN( TRUE );
}

SKILL( skill_mask )
{
  AFFECT_DATA      pAf;
  MOB_INDEX_DATA * pMob;
  MOB_INDEX_DATA * zMob;
  int              vnum;
  int              nMatch;
  int              count;

  PUSH_FUNCTION( "skill_mask" );

  if ( is_affected( ch, sn ) )
  {
    send_to_char( "�A�w�g���e�F�T\n\r", ch );
    RETURN( FALSE );
  }

  if ( ch->position != POS_STANDING )
  {
    send_to_char( "�A�ثe�����A���A�X���e�T\n\r", ch );
    RETURN( FALSE );
  }

  if ( is_pk( ch ) )
  {
    send_to_char( "�藍�_�M�A�ثe����I�i�o�تk�N�T\n\r", ch );
    RETURN( FALSE );
  }

  for ( zMob = NULL, nMatch = count = vnum = 0; nMatch < top_mob_index; vnum++ )
  {
    if ( ( pMob = get_mob_index( vnum ) ) )
    {
      nMatch++;
      if ( pMob->level <= level
        && pMob->count > 0
        && pMob->speak
        && number_range( 0, count++ ) == 0 ) zMob = pMob;
    }
  }

  if ( !zMob )
  {
    send_to_char( "�藍�_�M�䤣��i�H���e����H�C\n\r", ch );
    RETURN( FALSE );
  }

  pAf.type      = sn;
  pAf.duration  = UMIN( 5, UMAX( 1, level / 20 ) );
  pAf.location  = APPLY_NONE;
  pAf.modifier  = 0;
  pAf.level     = ch->level;
  affect_to_char( ch, &pAf );

  act( "$n�ʤF�Ǥ�}�M���e��$t�F�M�o�S"
    , ch, mob_index_name( ch, zMob ), NULL, TO_ALL );

  ch->mask = zMob->vnum;

  RETURN( TRUE );
}

SKILL( skill_hide )
{
  AFFECT_DATA pAffect;

  PUSH_FUNCTION( "skill_hide" );

  send_to_char( "�A�յ����æۤv�C\n\r", ch );

  if ( is_affected( ch, sn ) ) RETURN( FALSE );

  pAffect.type      = sn;
  pAffect.duration  = -1;
  pAffect.location  = APPLY_NONE;
  pAffect.modifier  = 0;
  pAffect.level     = ch->level;
  affect_to_char( ch, &pAffect );

  RETURN( TRUE );
}

FUNCTION( do_forsake )
{
  char          arg[MAX_INPUT_LENGTH];
  int           sn;
  SKILL_DATA  * pSkill;
  ENABLE_DATA * pEnable;

  PUSH_FUNCTION( "do_forsake" );

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    send_to_char( "�A�n�������@�ӧޯ�M�Ьd�� forsake�C\n\r", ch );
    RETURN_NULL();
  }

  if ( !( pSkill = skill_isname( arg ) ) )
  {
    act( "�䤣��A�n�������ޯ� $2$T$0�T", ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  if ( ch->skill[ ( sn = pSkill->slot ) ] <= 0 )
  {
    act( "�A�ۤv�]���|$w�M�������S", ch, pSkill, NULL, TO_CHAR );
    RETURN_NULL();
  }

  for ( pEnable = ch->enable; pEnable; pEnable = pEnable->next )
  {
    if ( pEnable->skill == pSkill )
    {
      act( "$w�٦b�P��M���������a�T", ch, pSkill, NULL, TO_CHAR );
      RETURN_NULL();
    }
  }

  cancel_skill( ch, pSkill );
  act( "�A�ۼo�F$w�o���ޯ�F�C", ch, pSkill, NULL, TO_CHAR );
  RETURN_NULL();
}

bool learn_expert_skill( CHAR_DATA * ch, SKILL_DATA * pSkill )
{
  LIMIT_DATA * pLimit;
  SKILL_DATA * aSkill;
  int          slot;
  int          max;

  PUSH_FUNCTION( "learn_expert_skill" );

  if ( !ch || !pSkill )
  {
    mudlog( LOG_DEBUG, "learn_expert_skill: ���~���ӷ�." );
    RETURN( FALSE );
  }

  if ( pSkill->associate == ERRORCODE
    || pSkill->degree == ERRORCODE
    || !( aSkill = get_skill( pSkill->associate ) ) )
    RETURN( FALSE );

  /* �ޯ�ʧO���� */
  if ( aSkill->sex && aSkill->sex > ERRORCODE && aSkill->sex < MAX_SEX )
    RETURN( FALSE );

  if ( ( slot = aSkill->slot ) < 0 || slot >= MAX_SKILL )
  {
    mudlog( LOG_DEBUG, "learn_expert_skill: �W�X�d��." );
    RETURN( FALSE );
  }

  if ( ch->skill[slot] > 0
    || !ch->class
    || learn_count( ch ) >= get_learnable( ch )
    || !check_skill_restrict( ch, aSkill, FALSE )
    || !check_valid_rating( ch, aSkill, FALSE )
    || number_range( 1, 10000 - ( ch->skill[slot] * 20 ) ) > pSkill->degree )
    RETURN( FALSE );

  for ( max = 0, pLimit = aSkill->limit; pLimit; pLimit = pLimit->next )
  {
    if ( pLimit->class == ch->class )
    {
      if ( ch->level < pLimit->level ) RETURN( FALSE );
      max = UMIN( number_range( 1, 5 ), pLimit->adept );
      break;
    }
  }

  if ( max <= 0 ) RETURN( FALSE );
  ch->skill[slot] = max;

  act( "$A$A$A$7���H�A�q$w��|��$2$W$7�o�@���\\�ҡT$0$A$A$A"
    , ch, pSkill, aSkill, TO_CHAR );

  act( "$A$A$A$7$n$7�o�ѿW�p�M�Ƿ|�F$2$W$7�o�@���\\�ҡT$0$A$A$A"
    , ch, NULL, aSkill, TO_ROOM );

  RETURN( TRUE );
}

bool effect_driver( CHAR_DATA * ch, CHAR_DATA * victim, EFFECT_DATA * pEffect )
{
  int         oldvalue;
  AFFECT_DATA pAffect;

  PUSH_FUNCTION( "effect_driver" );

  if ( !ch || !victim || !pEffect )
  {
    mudlog( LOG_DEBUG, "effect_driver: �ӷ����~." );
    RETURN( FALSE );
  }

  if ( pEffect->value[2] > 0 )
    if ( number_percent() > pEffect->value[2] ) RETURN( FALSE );

  switch( pEffect->type )
  {
  default:
    mudlog( LOG_DEBUG, "effect_driver: �ﶵ %d ���~.", pEffect->type );
    RETURN( FALSE );

  /* �����k�O */
  case EFFECT_VICTIM_MANA:

    oldvalue = victim->mana;

    victim->mana =
       UMAX( 0, UMIN( victim->max_mana, victim->mana - pEffect->value[0] ) );

    if ( oldvalue < victim->mana )
    {
      act( "$3$n���k�O��M�����W�ɤF�T$0", victim, NULL, NULL, TO_ALL );
    }

    else if ( oldvalue > victim->mana )
    {
      act( "$1$n���k�O��M�����U���F�T$0", victim, NULL, NULL, TO_ALL );
    }

    if ( victim->mana <= 0 )
    {
      act( "$2$n���k�O�w�g�ӺɡM���ݬO����F�T$0", victim, NULL, NULL, TO_ALL );
    }

    break;

  /* ���ۤv�k�O */
  case EFFECT_SELF_MANA:

    oldvalue = ch->mana;

    ch->mana =
       UMAX( 0, UMIN( ch->max_mana, ch->mana - pEffect->value[0] ) );

    if ( oldvalue < ch->mana )
    {
      act( "$3$n���k�O��M�����W�ɤF�T$0", ch, NULL, NULL, TO_ALL );
    }

    else if ( oldvalue > ch->mana )
    {
      act( "$1$n���k�O��M�����U���F�T$0", ch, NULL, NULL, TO_ALL );
    }

    if ( ch->mana <= 0 )
    {
      act( "$2$n���k�O�w�g�ӺɡM���ݬO����F�T$0", ch, NULL, NULL, TO_ALL );
    }

    break;

  /* �������O */
  case EFFECT_VICTIM_MOVE:

    oldvalue = victim->move;

    victim->move =
       UMAX( 0, UMIN( victim->max_move, victim->move - pEffect->value[0] ) );

    if ( oldvalue < victim->move )
    {
      act( "$3$n����O��M�����W�ɤF�T$0", victim, NULL, NULL, TO_ALL );
    }

    else if ( oldvalue > victim->move )
    {
      act( "$1$n����O��M�����U���F�T$0", victim, NULL, NULL, TO_ALL );
    }

    if ( victim->move <= 0 )
    {
      act( "$2$n����O�w�g�ӺɡM���ݬO����F�T$0", victim, NULL, NULL, TO_ALL );
    }

    break;

  /* ���ۤv��O */
  case EFFECT_SELF_MOVE:
    oldvalue = ch->move;

    ch->move =
       UMAX( 0, UMIN( ch->max_move, ch->move - pEffect->value[0] ) );

    if ( oldvalue < ch->move )
    {
      act( "$3$n����O��M�����W�ɤF�T$0", ch, NULL, NULL, TO_ALL );
    }

    else if ( oldvalue > ch->move )
    {
      act( "$1$n����O��M�����U���F�T$0", ch, NULL, NULL, TO_ALL );
    }

    if ( ch->move <= 0 )
    {
      act( "$2$n����O�w�g�ӺɡM���ݬO����F�T$0", ch, NULL, NULL, TO_ALL );
    }

    break;

  /* ��貴�� */
  case EFFECT_VICTIM_BLINDNESS:

    /* �Y�w�g�����h���ʧ@ */
    if ( is_affected( victim, SLOT_BLINDNESS ) ) break;

    pAffect.type      = SLOT_BLINDNESS;
    pAffect.location  = APPLY_HITROLL;
    pAffect.modifier  = 0 - pEffect->value[0];
    pAffect.duration  = pEffect->value[1];
    pAffect.level     = ch->level;
    affect_to_char( victim, &pAffect );

    act( "�j���@�{�M$n�Ȯɥ����F�T", victim, NULL, NULL, TO_ALL );
    break;

  /* ���A�G */
  case EFFECT_VICTIM_CURSE:

    /* �Y�w�g�Q�A�G�h���ʧ@ */
    if ( is_affected( victim, SLOT_CURSE ) ) break;

    pAffect.type      = SLOT_CURSE;
    pAffect.duration  = pEffect->value[1];
    pAffect.location  = APPLY_HITROLL;
    pAffect.modifier  = 0 - pEffect->value[0];
    pAffect.level     = ch->level;
    affect_to_char( victim, &pAffect );

    pAffect.location  = APPLY_SAVING_SPELL;
    pAffect.modifier  = 1;
    affect_to_char( victim, &pAffect );

    act( "$n�P��@�}�t�������$n���O�q�C", victim, NULL, NULL, TO_ALL );

    break;

  /* ��褤�r */
  case EFFECT_VICTIM_POISON:

    if ( get_curr_str( victim ) > 2 )
    {
      /* �o�ӷ|�֭p */
      pAffect.type      = SLOT_POISON;
      pAffect.duration  = pEffect->value[1];
      pAffect.location  = APPLY_STR;
      pAffect.modifier  = -2;
      pAffect.level     = ch->level;
      affect_join( victim, &pAffect );

      act( "$n�y�W�@�}�C�@�}�աM���G�O���F�r�T", victim, NULL, NULL, TO_ALL );
    }

    break;

  /* ������ */
  case EFFECT_VICTIM_SLEEP:
    break;

  /* ���·� */
  case EFFECT_VICTIM_PARA:
    /* �Ǫ��S���·� */
    if ( IS_NPC( victim ) ) RETURN( FALSE );

    WAIT_STATE( victim, pEffect->value[0] );
    act( "$2$n�L�Q���@�U�M�Ȯɤ��ӷ|�ʡT$0", victim, NULL, NULL, TO_ALL );
    break;

  /* �����F���� */
  case EFFECT_VICTIM_FAERIE_FIRE:

    /* �Y�w�g�Q�I�k�h���ʧ@ */
    if ( is_affected( victim, SLOT_FAERIE_FIRE ) ) break;

    pAffect.type      = SLOT_FAERIE_FIRE;
    pAffect.duration  = pEffect->value[1];
    pAffect.location  = APPLY_AC;
    pAffect.modifier  = pEffect->value[0];
    pAffect.level     = ch->level;
    affect_to_char( victim, &pAffect );

    act( "$n�Q�@�ί��������]��ۡC", victim, NULL, NULL, TO_ALL );

    break;

  /* �����ͩR�O */
  case EFFECT_VICTIM_HIT:

    oldvalue = victim->hit;

    victim->hit =
       UMAX( 0, UMIN( victim->max_hit, victim->hit - pEffect->value[0] ) );

    if ( oldvalue < victim->hit )
    {
      act( "$3$n���ͩR�O��M�����W�ɤF�T$0", victim, NULL, NULL, TO_ALL );
    }

    else if ( oldvalue > victim->hit )
    {
      act( "$1$n���ͩR�O��M�����U���F�T$0", victim, NULL, NULL, TO_ALL );
    }

    if ( victim->hit <= 0 )
    {
      act( "$2$n���ͩR�O�w�g�ӺɡM���F���h�a�T$0", victim, NULL, NULL, TO_ALL );
    }
    break;

  /* ���ۤv�ͩR�O */
  case EFFECT_SELF_HIT:

    oldvalue = ch->hit;

    ch->hit =
       UMAX( 0, UMIN( ch->max_hit, ch->hit - pEffect->value[0] ) );

    if ( oldvalue < ch->hit )
    {
      act( "$3$n���ͩR�O��M�����W�ɤF�T$0", ch, NULL, NULL, TO_ALL );
    }

    else if ( oldvalue > ch->hit )
    {
      act( "$1$n���ͩR�O��M�����U���F�T$0", ch, NULL, NULL, TO_ALL );
    }

    if ( ch->hit <= 0 )
    {
      act( "$2$n���ͩR�O�w�g�ӺɡM�a�����T$0", ch, NULL, NULL, TO_ALL );
    }

    break;

  /* �@������ */
  case EFFECT_HIT_DIE:
    if ( victim->hit >= 0 ) victim->hit = 0;
    act( "\n\r$2$n�o�X�����@���M$N����z�����N�Q�����Ĩ��F�T$0\n\r"
      , ch, NULL, victim, TO_CHAR );
    act( "\n\r$2$n�o�X�����@���M$N����z�����N�Q�����Ĩ��F�T$0\n\r"
      , ch, NULL, victim, TO_VICT );
    act( "\n\r$2$n�o�X�����@���M$N����z�����N�Q�����Ĩ��F�T$0\n\r"
      , ch, NULL, victim, TO_NOTVICT );
    break;
  }

  RETURN( TRUE );
}

FUNCTION( do_leftskill )
{
  int          loop;
  int          count;
  int          cost;
  bool         CanInc;
  SKILL_DATA * pSkill;
  LIMIT_DATA * pLimit;

  PUSH_FUNCTION( "do_leftskill" );

  if ( IS_NPC( ch ) || !ch->class ) RETURN_NULL();

  cost = UMAX( ch->level * LeftSkillCost, ch->level );

  if ( ch->gold < cost )
  {
    act( "���A�ꨬ$i��Ȥl�M�ڴN�i�D�A�T", ch, &cost, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( learn_count( ch ) >= get_learnable( ch ) )
  {
    act( "�藍�_�M�A�w�g�W�L$T�i�H�ǲߧޯ઺�`�ơT"
      , ch, NULL, class_name( ch->class, TRUE ), TO_CHAR );

    RETURN_NULL();
  }

  clear_buffer();

  send_to_buffer( "\e[1;33;44m���� �� �� �� �� �W ��    �� �� �^ �� �W ��"
    "    �о� ��    ��  �̰����m �ثe���m \e[0m\n\r" );

  for ( count = loop = 0; loop < MAX_SKILL; loop++ )
  {
    if ( !( pSkill = get_skill( loop ) )
      || !pSkill->valid
      || !check_skill_restrict( ch, pSkill, FALSE )
      || !check_valid_rating( ch, pSkill, FALSE ) )
      continue;

    CanInc = FALSE;
    for ( pLimit = pSkill->limit; pLimit; pLimit = pLimit->next )
    {
      if ( pLimit->class == ch->class )
      {
        if ( ch->level >= pLimit->level && ch->skill[loop] < pLimit->adept )
          CanInc = TRUE;

        break;
      }
    }

    if ( !pLimit ) continue;

    send_to_buffer( "%4d %-20s %-20s %4s %8s %s%s %s\n\r"
      , ++count, pSkill->cname, pSkill->name
      , pSkill->teach ? "�i�H" : "���i"
      , skill_type( pSkill->type )
      , CanInc ? " " : "\e[1;32m*\e[0m"
      , adeptation_name( pLimit->adept )
      , adeptation_name( ch->skill[loop] ) );
  }

  if ( count == 0 ) send_to_buffer( "�A�w�g�S���ޯ�i�H�ǲߤF�T\n\r" );
  else gold_from_char( ch, cost );

  print_buffer( ch );

  RETURN_NULL();
}

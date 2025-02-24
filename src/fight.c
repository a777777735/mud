/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

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
#include <string.h>
#include "merc.h"

/* �ϰ��� */
void    make_corpse     args( ( CHAR_DATA * ) );
void    dead_penalty    args( ( CHAR_DATA * ) );
void    group_gain      args( ( CHAR_DATA * , CHAR_DATA * ) );
int     xp_compute      args( ( CHAR_DATA * , CHAR_DATA * ) );
void    trip            args( ( CHAR_DATA * , CHAR_DATA * ) );
void    record_kill     args( ( CHAR_DATA * , CHAR_DATA * ) );
void    dam_message     args( ( CHAR_DATA * , CHAR_DATA * , int , int ) );

static ARRAY_DATA DamageMessage[] =
{
  { 15000, "\e[1;31m*****�W-�G-�Q-��-��-��-��*****\e[0m" },
  { 10000, "\e[1;33m****��-�}-��-��-��-��-��****\e[0m"   },
  {  5000, "\e[1;32m***�L-�i-��-��-��-��-��***\e[0m"     },
  {  2500, "\e[1;36m***��-�H-��-��-��-��-��***\e[0m"     },
  {  1000, "\e[1;34m***�����i�Ȫ��ж�***\e[0m"           },
  {   520, "\e[1;35m**���Y�����ж�**\e[0m"               },
  {   480, "\e[1;37m**���Y�����ˮ`**\e[0m"               },
  {   440, "\e[31m*�Y�����ˮ`*\e[0m"                     },
  {   400, "\e[32m*���j������*\e[0m"                     },
  {   360, "\e[33m*���q���ˮ`*\e[0m"                     },
  {   320, "\e[34m*���q������*\e[0m"                     },
  {   250, "\e[35m*���L���ˮ`*\e[0m"                     },
  {   200, "\e[36m*���L������*\e[0m"                     },
  {   160, "�ܲ`���ˤf"                                  },
  {   120, "���q���ˤf"                                  },
  {    80, "�L�L���ˤf"                                  },
  {    40, "���q������"                                  },
  {     1, "���L������"                                  },
  {     0, "���O����F"                                  }
};

/* �԰����D�n�y�{�{��, update_hanlder �|�g���ʪ��I�s�L */
void violence_update( void )
{
  CHAR_DATA * ch;
  CHAR_DATA * ch_next;
  CHAR_DATA * victim;
  CHAR_DATA * rch;
  CHAR_DATA * rch_next;

  PUSH_FUNCTION( "violence_update" );

  for ( ch = char_list; ch; ch = ch_next )
  {
    ch_next = ch->next;

    /* �S���԰����S��, �άO���m�Ӥ[���]�S�� */
    if ( !verify_char( ch )
      || !can_damage( ch )
      || !( victim = ch->fighting )
      || !ch->in_room
      || !verify_char( victim )
      || !can_damage( victim ) )
    {
      ch->fight_tick = 0;
      continue;
    }

    if ( ch->answer )
    {
      send_to_char( "\n\r\n\r�]���A���b�԰����M�ҥH�����A�����D�T\n\r\n\r", ch );
      free_struct( ch->answer, STRUCT_ANSWER_DATA );
      ch->answer = NULL;
    }

    if ( IS_NPC( ch )
      && !ch->boss
      && !IS_NPC( victim )
      && FightRound > 0
      && FightDifferent > 0
      && FightRate > 0
      && ++ch->fight_tick >= FightRound
      && ch->hit * FightRate > ch->max_hit
      && abs( ch->level - victim->level ) >= FightDifferent
      && ch->pIndexData
      && !IS_SET( ch->pIndexData->act, ACT_NOREBORN ) )
    {
      act( "$n��M�������D�R�o���԰��S����N�q�M�����F�M�٤��p�^�a�ܯ��T"
        "\n\r�಴���M$n�w�g�����h�V�F�T"
        , ch, NULL, NULL, TO_ROOM );
      extract_char( ch, TRUE );
      continue;
    }

    /* �Y�O���a����, �B�ĤH�b�P�@���ж� */
    if ( IS_AWAKE( ch ) && ch->in_room == victim->in_room )
      striking( ch, victim, TYPE_UNDEFINED );

    else
      stop_fighting( ch, FALSE );

    /* ��誺�ʧ@�ϼĤH�����F�h�S�� */
    if ( !verify_char( ch )
      || !( victim = ch->fighting )
      || !can_damage( ch )
      || !verify_char( victim )
      || !can_damage( victim )
      || !ch->in_room ) continue;

    /* MOBPROGRAM */
    mprog_hitprcnt_trigger( ch, victim );
    mprog_fight_trigger( ch, victim );

    /* ��誺�ʧ@�ϼĤH�����F�h�S�� */
    if ( !verify_char( ch )
      || !can_damage( ch )
      || !( victim = ch->fighting )
      || !verify_char( victim )
      || !can_damage( victim )
      || !ch->in_room ) continue;

    /* �Y�O�P�@�� group �S�b�P�@�椤 */
    for ( rch = ch->in_room->people; rch; rch = rch_next )
    {
      rch_next = rch->next_in_room;

      /* ��誺�ʧ@�ϼĤH�����F�h�S�� */
      if ( !verify_char( ch )
        || !can_damage( ch )
        || !( victim = ch->fighting )
        || !verify_char( victim )
        || !can_damage( victim )
        || !ch->in_room ) break;

      if ( IS_AWAKE( rch ) && !rch->fighting )
      {
        /* �P�@�� group �����a, �|�۰������A, ���L�L����Q�g�b */
        if ( !IS_NPC( ch ) || is_affected( ch, SLOT_CHARM_PERSON ) )
        {
          if ( ( !IS_NPC( rch ) || is_affected( rch, SLOT_CHARM_PERSON ) )
            && is_same_group( ch, rch ) )
            striking( rch, victim, TYPE_UNDEFINED );

          continue;
        }

        /* �p�������� */
        if ( IS_NPC( rch ) && rch->boss == victim )
        {
          striking( rch, ch, TYPE_UNDEFINED );
          continue;
        }

        /* �P�@���� NPC �|���� �P�� NPC , �άO 10% ���|�������P�� */
        if ( IS_NPC( rch )
          && !is_affected( rch, SLOT_CHARM_PERSON )
          && !rch->mount_by
          && !rch->boss )
        {
          if ( rch->pIndexData == ch->pIndexData
            || number_range( 1, 10 ) == 1 )
          {
            CHAR_DATA * vch;
            CHAR_DATA * target = NULL;
            int         number = 0;

            for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
            {
              if ( can_see( rch, vch )
                && is_same_group( vch, victim )
                && !IS_NPC( vch )
                && number_range( 0, number ) == 0 )
              {
                target = vch;
                number++;
              }
            }

            if ( target )
            {
              if ( ( ( ( target->level - rch->level <= 4 )
                && ( target->level - rch->level >= -4 ) )
                && !( IS_GOOD ( rch ) && IS_GOOD ( target ) ) )
                || ( IS_EVIL ( rch ) || IS_EVIL ( target ) ) )
                striking( rch, target, TYPE_UNDEFINED );
            }
          }
        }
      }
    }
  }

  RETURN_NULL();
}

void striking( CHAR_DATA * ch, CHAR_DATA * victim, int dt )
{
  OBJ_DATA * wield;
  int        dam;

  PUSH_FUNCTION( "striking" );

  /* �ݬ�����O���O�w�g�� */
  if ( !verify_char( ch )
    || !can_damage( ch )
    || !verify_char( victim )
    || !can_damage( victim ) )
  {
    mudlog( LOG_DEBUG, "striking: �H���w�g���`" );
    RETURN_NULL();
  }

  if ( !ch->in_room || !victim->in_room )
  {
    mudlog( LOG_DEBUG, "striking: �ӷ������T." );
    RETURN_NULL();
  }

  /* �Ұ� Enable */
  if ( exert_fight_enable( ch, victim, TRUE ) == NULL
    && ch->in_room == victim->in_room )
  {
    if ( dt == TYPE_UNDEFINED )
    {
      wield = get_eq_char( ch, WEAR_WIELD );
      dt    = TYPE_HIT;

      if ( wield && wield->item_type == ITEM_WEAPON ) dt += wield->value[3];
    }

    dam = get_damroll( ch );
    if ( !IS_AWAKE( victim ) ) dam *= 2;
    damage( ch, victim, UMAX( 1, dam ), dt, ATTACK_RANDOM );
  }

  RETURN_NULL();
}

/* ��˪��D�n�{�� */
void damage( CHAR_DATA * ch, CHAR_DATA * victim, int dam, int dt, int situs )
{
  bool        Reborn;
  int         hit;
  char        buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "damage" );

  if ( !verify_char( ch )
    || !can_damage( ch )
    || !verify_char( victim )
    || !can_damage( victim ) ) RETURN_NULL();

  if ( victim != ch )
  {
    /* �ݬݬO�_�i�H�}�Ҿ԰� */
    if ( !can_fight( ch , victim ) ) RETURN_NULL();

    check_killer( ch, victim );

    if ( victim->position > POS_DEAD )
    {
      /* �Y���|���԰�, �h�}�Ҿ԰� */
      if ( !victim->fighting ) set_fighting( victim, ch );

      /* �Y�D���|���԰�, �h�}�Ҿ԰� */
      if ( !ch->fighting ) set_fighting( ch, victim );

      /* �p�G���Q�g�b�F, �D���i��|�h�����L���D�H */
      if ( IS_NPC( ch )
        && IS_NPC( victim )
        && is_affected( victim, SLOT_CHARM_PERSON )
        && victim->master
        && victim->master->in_room == ch->in_room
        && number_range( 0 , 7 ) == 0 )
      {
        stop_fighting( ch, FALSE );
        striking( ch, victim->master, TYPE_UNDEFINED );
        RETURN_NULL();
      }
    }

    /* �p�G�D�H�������U, �h����l�H */
    if ( victim->master == ch ) stop_follower( victim );

    /* �p�G�D�H�����p�� */
    if ( victim->boss == ch   ) spirit_from_char( victim );
    if ( victim->spirit == ch ) spirit_from_char( ch );

    /* �p�G�D���O���Ϊ�, �@�w�|�{�X��� */
    if ( is_affected( ch, SLOT_INVIS ) || is_affected( ch, SLOT_MASS_INVIS ) )
    {
      affect_release( ch, SLOT_INVIS      );
      affect_release( ch, SLOT_MASS_INVIS );

      act( "$n�����@�{�M�v���{�X��ΡC", ch, NULL, NULL, TO_ROOM );
    }

    /* �p�G�O���e�]�|�Q�ѯ} */
    if ( is_affected( ch, SLOT_MASK ) )
    {
      affect_release( ch, SLOT_MASK );

      act( "$n�����e�N�Q�����ѯ}�F�T", ch, NULL, NULL, TO_ALL );
    }

    /* �p�G�O�ðΤ]�����X�{ */

    /* �ץ��ˮ`�� */
    if ( is_affected( victim, SLOT_SANCTUARY ) ) dam /= 2;

    if ( is_affected( victim, SLOT_PROTECTION ) && IS_EVIL( ch ) )
      dam -= dam / 4;

    if ( dam < 0 )                                         dam  = 0;

    if ( dt >= TYPE_HIT )
    {
      /* �̭˹�� */
      if ( IS_NPC( ch ) && number_percent() < ch->level / 20 )
        trip( ch, victim );
    }

    /* �Y�O�{�⥴�h�b�o�̦����L����O */
    if ( dt == TYPE_HIT )
    {
      if ( ch->move >= TYPE_HIT_COST ) ch->move -= TYPE_HIT_COST;
      else                             dam = 0;
    }

    /* �R���v */
    hit = UMIN( MaxHitroll, UMAX( 0, get_hitroll( ch ) + 100 ) );
    dam = dam * hit / 100;

    /* ���m�O */
    dam += get_ac( victim );

    /* �{�� */
    victim->dodge = 0;

    if ( exert_dodge_enable( victim, ch, TRUE ) && victim->dodge > 0 )
      dam -= victim->dodge;
  }

  /* �����L���ͩR�� */
  dam = damage_situs( victim, UMAX( 0, dam ), situs );

  /* ��ܧ�˰T�� */
  if ( victim != ch )
  {
    dam = UMAX( 0, dam );
    dam_message( ch, victim, dam, dt );
  }

  /* ����(Immortal)�Y���]�w�����h���|�� */
  if ( !IS_NPC( victim )
    && victim->level >= LEVEL_NODEATH
    && IS_SET( victim->act, PLR_NODEATH )
    && victim->hit < 1 )
    victim->hit = 1;

  /* �p�G���F, �O����@���A�� */
  if ( victim->pcdata && victim->pcdata->corpse && victim->hit < 1 )
    victim->hit = 1;

  /* �Ѩc�̤��঺ */
  if ( !IS_NPC( victim ) && victim->jail > 0 && victim->hit < 1 )
    victim->hit = 1;

  /* ��L�V�̤]���঺ */
  if ( !IS_NPC( victim ) && victim->failed > 0 && victim->hit < 1 )
    victim->hit = 1;

  /* �b�Z���j�|�̱��I */
  if ( is_pk( victim ) && victim->hit < 1 )
  {
    victim->hit = 1;

    if ( is_pk( ch ) )
    {
      stop_pk( ch );
      sprintf( buf, "%s\e[0m(%s)�Q%s\e[0m(%s)�_�F�M"
        "�o���Z���j�|��%s\e[0m(%s)��ӡT"
        , victim->cname, victim->name, ch->cname, ch->name
        , ch->cname, ch->name );

      talk_channel_2( buf, CHANNEL_PK, "" );
      RETURN_NULL();
    }
  }

  /* �վ�L�ثe�����A */
  update_pos( victim );

  if ( victim->position == POS_DEAD )
    act( "$n�r�b���k�o�C", victim, NULL, NULL, TO_ROOM );

  /* �ݬݹ��O�_�ٲM��, ���M�N����԰��F */
  if ( !IS_AWAKE( victim ) ) stop_fighting( victim, FALSE );

  /* ���Q�F���F */
  if ( victim->position == POS_DEAD )
  {
    group_gain( ch, victim );

    /* ���`���ԭz */
    if ( !IS_NPC( victim ) )
    {
      AREA_DATA * pArea;

      mudlog( LOG_INFO , "%s �b�ж����X[ %d ]���F %s%s."
        , mob_name( NULL, ch )
        , (victim->in_room) ? victim->in_room->vnum : 0
        , IS_SET( victim->act, PLR_KILLER ) ? "(����)" :
          IS_SET( victim->act, PLR_THIEF )  ? "(�p��)" :
          IS_SET( victim->act, PLR_BOLTER ) ? "(�P��)" : ""
        , victim->name );

      if ( victim != ch )
      {
        if ( victim->in_room
          && ( pArea = ( victim->in_room->area ) )
          && pArea->fight )
        {
          sprintf( buf, "\e[1;36m%s �b��԰Ϥ� %s ��"
            , mob_name( NULL, ch )
            , (victim->in_room) ? victim->in_room->name : "�Y�a" );

          sprintf( buf, "%s %s ���X�h�F�T\e[0m"
            , buf, mob_name( NULL, victim ) );

          /* �����ӻP�� */
          if ( ch->pcdata )     ch->pcdata->fightwin++;
          if ( victim->pcdata ) victim->pcdata->fightlost++;

        }
        else
        {
          sprintf( buf, "\e[1;36m%s �b %s ���F"
            , mob_name( NULL, ch )
            , (victim->in_room) ? victim->in_room->name : "�Y�a" );

          sprintf( buf, "%s %s�T\e[0m", buf, mob_name( NULL, victim ) );
        }
      }
      else
      {
        if ( victim->in_room
          && ( pArea = ( victim->in_room->area ) )
          && pArea->fight )
        {
          sprintf( buf, "\e[1;36m%s�b��԰Ϥ�%s�]�ӵL��ӯO�]�F�T\e[0m"
            , mob_name( NULL, ch )
            , ( ch->in_room ) ? victim->in_room->name : "�Y�a" );
        }
        else
        {
          sprintf( buf, "\e[1;36m%s�b%s�W�Q�۱��F�T\e[0m"
            , mob_name( NULL, ch )
            , ( ch->in_room ) ? victim->in_room->name : "�Y�a" );
        }
      }

      talk_channel_2( buf, CHANNEL_BULLETIN, "�ֳ�" );

      /* ���F�缾�a���B�@ */
      dead_penalty( victim );
    }

    else if ( victim->pIndexData && victim->pIndexData->deadmsg )
    {
      char   statement[MAX_STRING_LENGTH];
      char   name1[MAX_STRING_LENGTH];
      char   name2[MAX_STRING_LENGTH];
      char * pString;

      pString = victim->pIndexData->deadmsg;
      if ( *pString == '.' ) pString++;

      str_cpy( name1, mob_name( NULL, victim ) );
      str_cpy( name2, mob_name( NULL, ch     ) );

      sprintf( statement, "%s�{���e�ϥX�̫᪺�u���%s�ۥX�R\n\r%s\e[0m\n\r"
        , name1, name2, pString );

      send_to_all_char( statement );
    }

    Reborn = raw_kill( ch , victim, TRUE );

    if ( ch->pcdata ) ch->pcdata->ply = 0;

    /* �O�����H���O�� */
    if ( !IS_NPC( ch ) || !IS_NPC( victim ) ) record_kill( ch , victim );

    if ( !IS_NPC( ch ) && IS_NPC( victim ) && !Reborn )
    {
      if ( IS_SET( ch->act, PLR_AUTOLOOT ) )
      {
        do_get ( ch, "all corpse" );
      }
      else
      {
        do_look( ch, "in corpse"    );
        do_get ( ch, "coins corpse" );
      }

      if ( IS_SET( ch->act, PLR_AUTOSAC ) || IS_IMMORTAL( ch ) )
      {
        do_sacrifice( ch, "corpse" );
      }
    }

    RETURN_NULL();
  }

  if ( victim == ch ) RETURN_NULL();

  if ( !IS_NPC( victim ) && !victim->desc )
  {
    if ( number_range( 0, victim->wait ) == 0 )
    {
      do_recall( victim, "" );
      RETURN_NULL();
    }
  }

  /* ���] */
  if ( IS_NPC(victim) && dam > 0 )
  {
    if ( ( IS_SET(victim->act, ACT_WIMPY ) && number_range( 0 , 1 ) == 0
      && victim->hit < get_curr_hit( victim ) / 2 )
      || ( is_affected( victim, SLOT_CHARM_PERSON ) && victim->master
      && victim->master->in_room != victim->in_room ) )
      do_flee( victim, "" );
  }

  /* �ݬݥͩR�O�O�_�C��ҳ]�w wimpy */
  if ( !IS_NPC( victim )
    && victim->hit > 0
    && victim->hit <= victim->wimpy
    && victim->wait == 0 )
    do_flee( victim, "" );

  /* �ݬݬO�_�]�w�_�u�۰ʰk�] */
  if ( !IS_NPC( victim ) && !victim->desc && IS_SET( victim->act, PLR_FLEE ) )
    do_recall( victim, "" );

  tail_chain();
  RETURN_NULL();
}

/* �ݬݬO�_�|�ܦ����H�� */
void check_killer( CHAR_DATA * ch, CHAR_DATA * victim )
{
  AREA_DATA * pArea;

  PUSH_FUNCTION( "check_killer" );

  /* ����X���᪺�D�H */
  while ( is_affected( victim, SLOT_CHARM_PERSON && victim->master ) )
    victim = victim->master;

  /* �ˬd�O�_�w�g�O���H��, �s��άO�q����, �άO�����O�����Ǫ� */
  if ( IS_NPC( victim )
    || IS_SET( victim->act, PLR_KILLER )
    || IS_SET( victim->act, PLR_THIEF  )
    || IS_SET( victim->act, PLR_BOLTER ) )
    RETURN_NULL();

  /* ��԰Ϧ��ŧK�v */
  if ( victim->in_room
    && ( pArea = ( victim->in_room->area ) )
    && pArea->fight )
    RETURN_NULL();

  /* �p�G��H�Q�g�b�F, �b�O���D�H�Y�W */
  if ( is_affected( ch, SLOT_CHARM_PERSON ) )
  {
    if ( !ch->master )
    {
      mudlog( LOG_DEBUG, "Check_killer: %s �g�b���~.", ch->name );
      affect_release( ch, SLOT_CHARM_PERSON );
      RETURN_NULL();
    }

    act( "�A�����U$N���H�o�M�A�{�b�O�@�ӱ��H�ǡT", ch->master, NULL, ch, TO_CHAR );
    SET_BIT( ch->master->act, PLR_KILLER );
    stop_follower( ch );
    RETURN_NULL();
  }

  if ( ch->boss && !IS_NPC( ch->boss ) )
  {
    act( "�A���p��$N���H�F�M�`�A�������H�ǤF�T", ch->boss, NULL, ch, TO_CHAR );
    SET_BIT( ch->boss->act, PLR_KILLER );
    RETURN_NULL();
  }

  /* �����Ǫ��άO�t�κ޲z��, �άO�۴�, �άO���w�g�O���H�� */
  if ( IS_NPC( ch )
    || ch == victim
    || IS_IMMORTAL( ch )
    || IS_SET( ch->act , PLR_KILLER ) )
    RETURN_NULL();

  /* �Z���j�| */
  if ( is_pk( ch ) && is_pk( victim ) )
    RETURN_NULL();

  /* �ѤU�ӴN�@�w�|�ܦ����H�� */
  send_to_char( "\e[1;31m�A�{�b�O���H����F�M�n�x�����T\e[0m\n\r", ch );
  SET_BIT( ch->act , PLR_KILLER );
  mudlog( LOG_INFO, "[PK] %s �ѱ��O�H %s."
    , ch->name, victim ? victim->name : "[�Y�H]" );
  if ( ch->pcdata ) ch->pcdata->murder++;
  save_char_obj( ch , SAVE_FILE );
  RETURN_NULL();
}

/* �]�w�ؼХثe�����A */
void update_pos( CHAR_DATA * victim )
{
  PUSH_FUNCTION( "update_pos" );

  if ( !victim || !verify_char( victim ) )
  {
    mudlog( LOG_DEBUG, "update_pos: �ӷ������D." );
    RETURN_NULL();
  }

  if ( victim->fighting && victim->hit > 0 )
  {
    victim->position = POS_FIGHTING;
    RETURN_NULL();
  }

  if ( victim->hit > 0 ) victim->position = POS_STANDING;
  else                   victim->position = POS_DEAD;

  RETURN_NULL();
}

/* �Ұʾ԰� */
void set_fighting( CHAR_DATA * ch, CHAR_DATA * victim )
{
  char * msg;

  PUSH_FUNCTION( "set_fighting" );

  if ( !ch || !victim || !verify_char( ch ) || !verify_char( victim ) )
  {
    mudlog( LOG_DEBUG, "set_fighting: �ʥF�ӷ�." );
    RETURN_NULL();
  }

  if ( ch->fighting )
  {
    mudlog( LOG_DEBUG , "set_fighting: %s�w�g�԰��F, �o�S�}�Ҿ԰�."
      , ch->name );

    RETURN_NULL();
  }

  /* �O�� */
  if ( IS_NPC( ch ) && IS_SET( ch->act, ACT_ENROLL ) && !IS_NPC( victim ) )
    set_enroll( ch, victim );

  if ( is_affected( ch, SLOT_SLEEP ) ) affect_release( ch, SLOT_SLEEP );

  /* �Ȧs���� */
  if ( !IS_NPC( ch ) && ch->desc ) cease_stack( ch->desc );

  ch->fighting = victim;
  ch->position = POS_FIGHTING;

  if ( victim->speak )
  {
    switch( number_range( 0,14 ) )
    {
    default:
    case  0: msg = "$Y$n�۹D�R�i�c��$N�M�ڭn�_�F�A�T$A$Z";               break;
    case  1: msg = "$Y$n�۹D�R�ѱ���$N�M�ݧڪ��F�`�T$A$Z";               break;
    case  2: msg = "$Y$n�۹D�R�i�c��$N�M�ǩR�ӡT$A$Z";                   break;
    case  3: msg = "$Y$n�۹D�R�z���ǡT$N�M�A�����a�T$A$Z";               break;
    case  4: msg = "$Y$n�۹D�R��T $N�T �ڭn�_�F�A�T$A$Z";               break;
    case  5: msg = "$Y$n���D�R$N�T �����A��������Ѫ��Ӷ��T$A$Z";        break;
    case  6: msg = "$Y$n�N�N�����R$N�M�G�L�b�a�T$A$Z";                   break;
    case  7: msg = "$Y$n���D: $N���Q���F�T�Q���ڴN�����A�T$A$Z";         break;
    case  8: msg = "$Y$n���D: �i����$N, ���~�����ѴN�O�A���Ҥ�F�T$A$Z"; break;
    case  9: msg = "$Y$n���D: $N�T�A����Q���ڴN�����A�T$A$Z";           break;
    case 10: msg = "$Y$n���D: $N���ѧA�Ȩ�, �e�A�K�O�a���@��C�T$A$Z";   break;
    case 11: msg = "$Y$n���D: $N��A�Ҿǥ����i�{�X�ӧa�T$A$Z";           break;
    case 12: msg = "$Y$n���D: $N���h�R�n�ç��a�T�������T$A$Z";           break;
    case 13: msg = "$Y$n���D: ���۶q�O�T$N�ç����s���F�ܡS$A$Z";         break;
    case 14: msg = "$Y$n�۹D: $N�T�A�����M�F�����ڡT$A$Z";               break;
    }

    sact( victim, ch, NULL, msg );
  }

  RETURN_NULL();
}

/* ����԰� */
void stop_fighting( CHAR_DATA * ch, bool fBoth )
{
  CHAR_DATA * fch;

  PUSH_FUNCTION( "stop_fighting" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "stop_fighting: �ʥF�ӷ�." );
    RETURN_NULL();
  }

  for ( fch = char_list; fch; fch = fch->next )
  {
    if ( !verify_char( fch ) ) continue;

    if ( fch == ch || ( fBoth && fch->fighting == ch ) )
    {
      fch->fighting = NULL;
      fch->position = POS_STANDING;
      update_pos( fch );
    }
  }

  RETURN_NULL();
}

/* �s�y���� */
void make_corpse( CHAR_DATA * ch )
{
  char       buf[MAX_STRING_LENGTH];
  OBJ_DATA * corpse;
  OBJ_DATA * obj;
  OBJ_DATA * obj_next;
  char     * name;
  bool       fKeeper;

  PUSH_FUNCTION( "make_corpse" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "make_corpse: �ӷ������T." );
    RETURN_NULL();
  }

  if ( !IS_NPC( ch ) && ch->pcdata && ch->pcdata->corpse )
  {
    mudlog( LOG_DEBUG, "make_corpse: �H���w�g���`" );
    RETURN_NULL();
  }

  if ( !IS_NPC( ch ) && ch->jail > 0 )
  {
    mudlog( LOG_DEBUG, "make_corpse: �H�����D��." );
    RETURN_NULL();
  }

  if ( !IS_NPC( ch ) && ch->failed > 0 )
  {
    mudlog( LOG_DEBUG, "make_corpse: �H����L��." );
    RETURN_NULL();
  }

  /* �Y�O�����Ǫ������� */
  if ( IS_NPC( ch ) )
  {
    corpse        = create_object( ObjCorpseNPC, -1 );
    corpse->timer = number_range( 2, 4 );

    if ( ch->gold > 0 )
    {
      obj_to_obj( create_money( ch->gold ), corpse );
      ch->gold = 0;
    }
  }

  else
  {
    corpse        = create_object( ObjCorpsePC, -1 );
    corpse->timer = -1;
  }

  name = mob_name( NULL, ch );
  sprintf( buf, corpse->cname, name );
  free_string( corpse->cname );
  corpse->cname = str_dup( buf );

  sprintf( buf, corpse->description, name );
  free_string( corpse->description );
  corpse->description = str_dup( buf );

  /* ���X���~ */
  fKeeper = is_keeper( ch );
  for ( obj = ch->carrying; obj; obj = obj_next )
  {
    obj_next = obj->next_content;
    obj_from_char( obj );

    if ( fKeeper || obj->Inventory ) extract_obj( obj );
    else                             obj_to_obj( obj, corpse );
  }

  /* ���a�M�����Ǫ�������񪺦�m���@�� */
  if ( IS_NPC( ch ) && !ch->pcdata )
  {
    obj_to_room( corpse, ch->in_room );
  }

  else
  {
    obj_to_room( corpse, RoomCorpse );
    ch->pcdata->corpse   = corpse;
    corpse->corpse_owner = ch;
  }

  RETURN_NULL();
}

/* �{���e���s�z */
void death_cry( CHAR_DATA * ch )
{
  ROOM_INDEX_DATA * was_in_room;
  char            * msg;
  int               door;
  int               vnum = 0;

  PUSH_FUNCTION( "death_cry" );

  /* �W�[���e�s�z�n���ܤƩ� */
  switch ( number_range( 0 , 12 ) )
  {
  default:
    msg  = "$n��͵��Y�M�������G�M�G���Ը@�C";
    break;

  case  0:
    msg  = "$n�I���@�n�M�˦b�a�W�M\e[32m���F\e[0m�T";
    break;

  case  1:
    msg  = "$n��\e[1;31m�A��\e[0m�b�A�����ҤW�d�U�F���e���A�G�C";
    break;

  case  2:
    msg  = "�A�D��$n�b���e\e[33m��z\e[0m�����D�C";
    vnum = ObjTurdVnum;
    break;

  case  3:
    msg  = "$n��\e[1;33m�Y\e[0m(head)�Q��b�a�W�M���C�C���ݵۧA�C";
    vnum = ObjHeadVnum;
    break;

  case  4:
    msg  = "$n��\e[1;31m��Ŧ\e[0m(heart)�q$s�ݤf���X�ӡC";
    vnum = ObjHeartVnum;
    break;

  case  5:
    msg  = "$n��\e[1;32m���u\e[0m(arm)�q$s���W�Q��F�U�ӡC";
    vnum = ObjArmVnum;
    break;

  case  6:
    msg  = "$n��\e[36m�L\e[0m(leg)���ͥͪ��q$s�ͭ������_�C";
    vnum = ObjLegVnum;
    break;

  case  8:
    msg  = "$n�˦b�a�W�M�ݨӤ@�ʤ]���ʤF�T";
    break;

  case  9:
    msg  = "$n�������B�M�����G���Ը@�T";
    break;

  case 10:
    msg  = "�A�Pı��$n���F��w�g���}�F�L������T";
    break;

  case 11:
    msg  = "�Ať��$n�{���e���G�s�n�M���n���ٯu�O�ݧԡT";
    break;

  case 12:
    msg = "$n������q���B�M�ݨӬO�ʤ��F���ˤl�T";
    break;
  }

  if ( IS_NPC( ch ) && ch->pIndexData && ch->pIndexData->reborn_vnum > 0 )
  {
    msg  = "$n��\e[1;34m���v\e[0m�C�C�����F�M���b�����T�S";
    vnum = 0;
  }

  act( smash_act_keyword( msg, "nes" ), ch, NULL, NULL, TO_ROOM );

  /* �s�y�X�Y�`, ���u�������F�� */
  if ( vnum != 0 )
  {
    char       buf[MAX_STRING_LENGTH];
    OBJ_DATA * obj;

    obj        = create_object( get_obj_index( vnum ), -1 );
    obj->timer = number_range( 4, 7 );

    sprintf( buf, obj->cname, mob_name( NULL, ch ) );
    free_string( obj->cname );
    obj->cname = str_dup( buf );

    sprintf( buf, obj->description, mob_name( NULL, ch ) );
    free_string( obj->description );
    obj->description = str_dup( buf );

    obj_to_room( obj, ch->in_room );
  }

  if ( IS_NPC(ch) ) msg = "�Ať�줣���B�ǨӤ@�n���e���d��C";
  else              msg = "�Ať�줣���B�@�n�Y�H�{���e���s�z�C";

  was_in_room = ch->in_room;
  for ( door = 0; door < DIR_MAX; door++ )
  {
    EXIT_DATA * pexit;

    if ( ( pexit = was_in_room->exit[door] )
      &&   pexit->to_room
      &&   pexit->to_room != was_in_room )
    {
      ch->in_room = pexit->to_room;
      act( smash_act_keyword( msg, "nes" ), ch, NULL, NULL, TO_ROOM );
    }
  }

  ch->in_room = was_in_room;
  RETURN_NULL();
}

/* �������F */
bool raw_kill( CHAR_DATA * ch , CHAR_DATA * victim, bool bCorpse )
{
  AREA_DATA * pArea;
  bool        fArea  = FALSE;
  bool        Reborn = FALSE;

  PUSH_FUNCTION( "raw_kill" );

  /* ����԰� */
  stop_fighting( victim, TRUE );

  /* �ץ����M */
  if ( victim->mount    ) unmount_char( victim,    victim->mount );
  if ( victim->mount_by ) unmount_char( victim->mount_by, victim );

  /* �ץ��p�� */

  /* ���ǵ����Ǫ����{���e���ʧ@ */
  if ( bCorpse ) mprog_death_trigger( victim, ( ch == victim ? NULL : ch ) );

  /* �p�G�i�H���� */
  if ( IS_NPC( victim )
    && victim->pIndexData
    && victim->pIndexData->reborn_vnum > 0
    && bCorpse )
  {
    MOB_INDEX_DATA  * pMobIndex;
    ROOM_INDEX_DATA * pRoom;
    CHAR_DATA       * pReborn;

    if ( ( pMobIndex = get_mob_index( victim->pIndexData->reborn_vnum ) )
      && get_mob_count( pMobIndex->vnum ) == 0 )
    {

      /* ��ݬO�_���]�w���ͪ��ж� */
      if ( victim->pIndexData->reborn_room == 0 )
      {
        pRoom = ch->in_room;
      }

      else if ( !( pRoom = get_room_index( victim->pIndexData->reborn_room )))
      {
        mudlog( LOG_DEBUG, "raw_kill: �Ǫ����X %d ���ͩж� %d ���s�b."
         , pMobIndex->vnum, victim->pIndexData->reborn_room );
        pRoom = ch->in_room;
      }

      /* �ƻs�Ǫ� */
      if ( ( pReborn = shape_mobile( pMobIndex, pRoom ) ) )
      {
        char buffer[MAX_STRING_LENGTH];

        /* �L�X�T�� */
        sprintf( buffer, "�y�ɡM�ѹp����M�����j���M�|�P��y�}�l�����C\n\r"
                         "�C�C�B�{�X�@�Ө��v�M�����N�O\e[1;32m%s\e[0m�S\n\r"
          , mob_name( NULL, pReborn ) );

        sendmsg_to_room( buffer, pReborn );
        Reborn = TRUE;

        /* �ݬݬO�_�|�۰ʧ���, ���ݦb�P�@���ж� */
        if ( IS_SET( victim->act, ACT_REBORN_FIGHT )
          && pReborn->in_room
          && ch->in_room == pReborn->in_room )
        {
          set_fighting( pReborn , ch );
        }
      }
    }
  }

  /* �s�y�X���� */
  if ( !Reborn && bCorpse ) make_corpse( victim );

  /* �Y���̬O�����Ǫ� */
  if ( IS_NPC( victim ) )
  {
    victim->pIndexData->killed++;
    kill_table[URANGE(0, victim->level, MAX_LEVEL-1)].killed++;

    if ( victim->bounty ) check_bounty( ch, victim );

    extract_char( victim, TRUE );
    RETURN( Reborn );
  }

  /* �ˬd�O�_����԰ϰ� */
  if ( victim->in_room
    && ( pArea = ( victim->in_room->area ) )
    && pArea->fight )
  fArea = TRUE;

  /* �ˬd�a�� */
  check_wanted( ch, victim );

  /* �P���H��, ���O�u���P�� */
  extract_char( victim, FALSE );

  /* �Ȧs���� */
  if ( !IS_NPC( victim ) && victim->desc ) cease_stack( victim->desc );

  /* �������W�Ҧ����k�N */
  while ( victim->affected ) affect_remove( victim, victim->affected );

  /* �M�����a���@�Ǹ�� */
  victim->armor       = 0;
  victim->hit         = UMAX( 1, victim->max_hit  );
  victim->mana        = UMAX( 1, victim->max_mana );
  victim->move        = UMAX( 1, victim->max_move );

  if ( victim->pcdata )
  {
                  victim->pcdata->pills        = 0;
                  victim->pcdata->dosage       = 0;
    if ( !fArea ) victim->pcdata->murder       = 0;
    if ( !fArea ) victim->pcdata->steal        = 0;
    if ( !fArea ) victim->pcdata->steal_panity = 0;
  }

  if ( !IS_NPC( victim ) && !fArea )
  {
    /* �M�����H�ǩM�s��, ��O�H���¸o */
    REMOVE_BIT( victim->act, PLR_KILLER );
    REMOVE_BIT( victim->act, PLR_THIEF  );
    REMOVE_BIT( victim->act, PLR_BOLTER );
  }

  victim->position = POS_STANDING;
  save_char_obj( victim , SAVE_FILE );
  do_help( victim , "dead_picture" );

  do_look( victim , "auto" );
  victim->position = POS_DEAD;

  RETURN( Reborn );
}

/* ��� group �C�@�ӤH�i�H����h�ָg��� */
void group_gain( CHAR_DATA * ch, CHAR_DATA * victim )
{
  CHAR_DATA * gch;
  CHAR_DATA * lch;
  int         xp;
  int         members;

  PUSH_FUNCTION( "group_gain" );

  /* MOB ���|�o��g���, �������a�]�S��, �ۺɤ]�S�� */
  if ( ( IS_NPC( ch ) && !ch->boss )
    || !IS_NPC(victim)
    || victim == ch ) RETURN_NULL();

  /* �P�@�� group , �B�b�P�@���ж��̭��~�঳�g��� */
  for ( members = 0, gch = ch->in_room->people; gch; gch = gch->next_in_room )
    if ( is_same_group( gch, ch )
      || gch->boss == ch
      || gch->spirit == ch ) members++;

  if ( members == 0 )
  {
    mudlog( LOG_DEBUG , "Group_gain: ������ 0." );
    members = 1;
  }

  lch = ( ch->leader ) ? ch->leader : ch;

  for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
  {
    OBJ_DATA * obj;
    OBJ_DATA * obj_next;

    if ( is_same_group( gch, ch ) )
    {
      if ( gch->level - lch->level >= 6 )
      {
        act( "�A���n��ۦ׸}$N��B�]���T", gch, NULL, ch, TO_CHAR );
        continue;
      }

      if ( gch->level - lch->level <= -6 )
      {
        send_to_char( "�o�Ӷ�����G���n���A�o�˪��׸}�C\n\r", gch );
        continue;
      }
    }

    else if ( gch->boss != lch ) continue;

    /* �O�_�n���g��� */
    if ( GroupSpiltExp ) xp = xp_compute( gch, victim ) / members;
    else                 xp = xp_compute( gch, victim );

    act( "�A�b�o���԰��o�� $3$x$0 �I�g��ȡC", gch , &xp, NULL, TO_CHAR );

    gain_exp( gch, xp );

    if ( IS_NPC( gch ) ) continue;

    /* �ݬݨ��W�諸�˳ƬO�_�ŦX�ثe���}�� */
    for ( obj = ch->carrying; obj; obj = obj_next )
    {
      obj_next = obj->next_content;

      /* �S���˳ƤW������ */
      if ( obj->wear_loc == WEAR_NONE ) continue;

      /* �ݬݰ}��O�_�۲ŦX */
      if ( ( obj->AntiEvil    && IS_EVIL( ch ) )
        || ( obj->AntiGood    && IS_GOOD( ch ) )
        || ( obj->AntiNeutral && IS_NEUTRAL( ch ) ) )
      {
        act( "�ڡM�A�Q$p�ҹ���F�C"          , ch, obj, NULL, TO_CHAR );
        act( "$n����F$p�M���֧⥦�߰_�ӧa�C", ch, obj, NULL, TO_ROOM );
        obj_from_char( obj );
        obj_to_room( obj, ch->in_room );
      }
    }
  }
  RETURN_NULL();
}

/* �p��g��ȥH�έp����ܪ��}�� */
int xp_compute( CHAR_DATA * ch, CHAR_DATA * victim )
{
  int align;
  int value;
  int count;
  int level;
  int factor;

  PUSH_FUNCTION( "xp_compute" );

  if ( !ch || !victim || !victim->pIndexData )
  {
    mudlog( LOG_DEBUG, "xp_compute: �ӷ������T." );
    RETURN( 0 );
  }

  /* �����}�窺�վ� */
  if ( ( align = ch->alignment - victim->alignment ) >  AlignmentDiff )
  {
    ch->alignment =
      UMIN( ch->alignment + ( align - AlignmentDiff ) / 4, AlignRange );
  }

  else if ( align < -AlignmentDiff )
  {
    ch->alignment =
      UMAX( ch->alignment + ( align + AlignmentDiff ) / 4, -AlignRange );
  }

  else
  {
    ch->alignment -= ch->alignment / 4;
  }

  if ( ch->level >= LEVEL_HERO ) RETURN( 0 );

  /* ��X�X�z������ */
  level = URANGE( 1, ch->level, MAX_LEVEL -1 );

  /* �k���S���g��� */
  if ( victim->pIndexData && victim->pIndexData == MobVampire )
    RETURN( 0 );

  /* ���Ǥ]�S�� */
  if ( victim->tractable > 0 ) RETURN( 0 );

  /* �p�G���Ƥp�� 0, �h�|�S���g��� */
  if ( ( count = kill_table[level].promotion ) <= 0 ) RETURN( 0 );

  /* �p�G�b�o�@�B�N�p�� 0, ���᭱�]�K�w���� 0 */
  if ( ( value = kill_table[level].exp / count ) <= 0 ) RETURN( 0 );

  /* ���Ŭۮt�V�h, �h�]�ƶV�� */
  factor = ( ( UMAX( 1, abs( ch->level - victim->level ) ) - 1 ) / 3 );

  /* �p�G factor �j�� 1, �]�N�O���Ŭۮt���t 6 �H�W�S���g��� */
  if ( factor > 1 ) RETURN( 0 );

  factor = factor * UMAX( 2, ( ch->level / 5 ) ) + 1;

  /* �Ҽ{�O�_���]�� */
  if ( MobDecrease > 0 ) factor += victim->pIndexData->killed / MobDecrease;

  /* �ݬݩǪ��g��ȬO�_���[�� */
  if ( victim->release )
  {
    value /= 2;
  }
  else if ( victim->pIndexData->multipile > 0 )
  {
     value = value * victim->pIndexData->multipile / 100;
  }

  /* �g��ȤW�U�ܰʦʤ����Q */
  value = variation( value / factor, 10 );

  RETURN( value );
}

char * const attack_table[] =
{
  "����",
  "�����a���F",        /* WEAPON_HAND    */
  "�����a�W�F",        /* WEAPON_DAGGER  */
  "���R�a��F",        /* WEAPON_SWORD   */
  "�ܻ����@��",        /* WEAPON_BLADE   */
  "�����a��F",        /* WEAPON_AXE     */
  "�����a���F",        /* WEAPON_WHIP    */
  "�r�P�a����",        /* WEAPON_SPEAR   */
  "�ΤO�a��F",        /* WEAPON_PEN     */
  "�V�O�aݴ�F",        /* WEAPON_HAMMER  */
  "�r�P�a���F",        /* WEAPON_CLUB    */
  "�Τ}�g���F",        /* WEAPON_BOW     */
  "�B�����"           /* WEAPON_FORCE   */
};

/* �\�˪��T����X��� */
void dam_message( CHAR_DATA * ch, CHAR_DATA * victim, int dam, int dt )
{
  const char * vs;
  const char * vp;
  const char * v1;
  const char * v2;
  const char * v3;
  const char * attack;
  char         buf1[BUFSIZ];
  char         buf2[BUFSIZ];
  char         buf3[BUFSIZ];
  char         buf4[BUFSIZ];
  char         buf5[BUFSIZ];
  char         buf6[BUFSIZ];

  PUSH_FUNCTION( "dam_message" );

  if ( !ch || !victim )
  {
    mudlog( LOG_DEBUG, "dam_message: �ʥF�ӷ�." );
    RETURN_NULL();
  }

  /* �W�[������ */
  if ( ch->limit < MAX_LIMIT_VALUE && ( !is_pk( ch ) || !is_pk( victim ) ) )
  {
    int         num;
    AREA_DATA * pArea;

    if ( !( ch->in_room
      && ( pArea = ch->in_room->area )
      && pArea->fight ) )
    {
      num = dam / ( UMAX( 1, ( get_curr_hit( victim ) / 100 ) ) );
      victim->limit = UMIN( MAX_LIMIT_VALUE, victim->limit + num );
    }
  }

  v3 = return_array( DamageMessage, dam
       , sizeof( DamageMessage ) / sizeof( DamageMessage[0] ) );

  if ( dam > 0 )
  {
    vp = "�åB��$E�y��";
    vs = "�åB��A�y��";
  }

  else
  {
    vp = "";
    vs = "";
  }

  /* �{����� */
  if ( dt == TYPE_HIT )
  {
    sprintf( buf1, "$n�{�����$N�M%s%s�C"    , vp, v3 );
    sprintf( buf2, "�A�{�����$N�M%s%s�C"    , vp, v3 );
    sprintf( buf3, "$n�{������A�M%s%s�C"    , vs, v3 );
    sprintf( buf4, "�A�{������A�ۤv�M%s%s�C", vs, v3 );
    sprintf( buf5, "$n�{�����$e�ۤv�M%s%s�C", vp, v3 );
  }

  /* �ϥΧޯ�άO�Z�������覡 */
  else
  {
    /* �ϥΧޯ���� */
    if ( dt >= 0 && dt < MAX_SKILL )
    {
      SKILL_DATA * pSkill;

      v1 = "�ϥ�";
      v2 = "����";

      if ( ( pSkill = get_skill( dt ) ) ) attack = pSkill->cname;
      else                                attack = "�������ޯ�";
    }

    /* �ϥΪZ�� */
    else
    {
      int table_size;

      /* �p��}�C���X�Ӥ��� */
      table_size = sizeof( attack_table ) / sizeof( attack_table[0] );
      v1 = v2 = "";

      /* �ˬd�O�_���W�X�}�C, �_�h�|�޵o��� */
      if ( dt < TYPE_HIT || dt >= TYPE_HIT + table_size )
      {
        mudlog( LOG_DEBUG, "Dam_message: %s �޵o���~���������A %d."
          , ch->name, dt );
        dt = TYPE_HIT;
      }

      attack = attack_table[ dt - TYPE_HIT ];
    }

    /* �榡�ƿ�X���r�� */
    /* �����ɡM���P�@���ж����H�ݪ� */
    sprintf( buf1, "$n%s%s%s$N�M%s%s�C"    , v1, attack, v2, vp, v3 );

    /* �����ɡM���D��ݪ� */
    sprintf( buf2, "�A%s%s%s$N�M%s%s�C"    , v1, attack, v2, vp, v3 );

    /* �����ɡM������ݪ� */
    sprintf( buf3, "$n%s%s%s�A�M%s%s�C"    , v1, attack, v2, vs, v3 );

    /* �۴ݮɡM���ۤv�ݪ� */
    sprintf( buf4, "�A%s%s%s�A�ۤv�M%s%s�C", v1, attack, v2, vs, v3 );

    /* �۴ݮɡM���P�@���ж����H�ݪ� */
    sprintf( buf5, "$n%s%s%s$e�ۤv�M%s%s�C", v1, attack, v2, vp, v3 );
  }

  sprintf( buf6 , "(%i)" , dam );

  if ( AttackValue )
  {
    str_cat( buf2, buf6 );
    str_cat( buf3, buf6 );
    str_cat( buf4, buf6 );
    str_cat( buf5, buf6 );
  }

  else if ( IS_IMMORTAL( ch ) )
  {
    str_cat( buf2, buf6 );
    str_cat( buf4, buf6 );
  }

  else if ( IS_IMMORTAL( victim ) )
  {
    str_cat( buf3, buf6 );
    str_cat( buf5, buf6 );
  }

  /* �p�G�O�۴ݪ���X�e�� */
  if ( victim == ch )
  {
    act( smash_act_keyword( buf4, "nesNES" ), ch, NULL, victim, TO_CHAR );
    act( smash_act_keyword( buf5, "nesNES" ), ch, NULL, victim, TO_ROOM );
  }

  /* �����O�H����X�覡 */
  else
  {
    act( smash_act_keyword( buf1, "nesNES" ), ch, NULL, victim, TO_NOTVICT );
    act( smash_act_keyword( buf2, "nesNES" ), ch, NULL, victim, TO_CHAR    );
    act( smash_act_keyword( buf3, "nesNES" ), ch, NULL, victim, TO_VICT    );
  }

  RETURN_NULL();
}

void trip( CHAR_DATA * ch, CHAR_DATA * victim )
{
  PUSH_FUNCTION( "trip" );

  if ( victim->wait == 0 )
  {
    act( "$n�A��ۧ�A�̭ˤF�C",   ch, NULL, victim, TO_VICT    );
    act( "�A�A��ۧ�$N���̭ˤF�C", ch, NULL, victim, TO_CHAR    );
    act( "$n�A��ۧ�$N���̭ˤF�C", ch, NULL, victim, TO_NOTVICT );
    WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
  }
  RETURN_NULL();
}

FUNCTION( do_kill )
{
  char        arg[MAX_INPUT_LENGTH];
  CHAR_DATA * victim;

  PUSH_FUNCTION( "do_kill" );

  one_argument( argument, arg );

  if ( !arg[0] )
  {
    send_to_char( "�A�Q�����֩O�S\n\r", ch );
    RETURN_NULL();
  }

  if ( !( victim = get_char_room( ch, arg ) ) )
  {
    act( "�A�n��������H $2$T$0 ���b�o�̡C", ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  if ( !can_damage( victim ) )
  {
    act( "�A�ثe�S����k����$N�T", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  /* �Y��H�O���a�B���O�s��άO���H�ǫh���� */
  if ( is_pk( ch ) && is_pk( victim ) )
  {
    if ( fight_info->status != PK_FIGHT )
    {
      send_to_char( "�b�o�q�ɶ��M�٬O�n�n�a�𮧤@�U�a�T\n\r", ch );
      RETURN_NULL();
    }
  }

  else if ( !IS_NPC( victim ) )
  {
    if ( !IS_SET( victim->act, PLR_KILLER )
      && !IS_SET( victim->act, PLR_THIEF  )
      && !IS_SET( victim->act, PLR_BOLTER )
      && ( victim->in_room
      && victim->in_room->area
      && !( victim->in_room->area->fight ) ) )
    {
      act( "�A�n�������a$N�M�Х� $2MURDER$0 �o�ӫ��O�C"
        , ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }
  }

  else
  {
    if ( is_affected( victim, SLOT_CHARM_PERSON ) && victim->master )
    {
      act( "�A������ $2MURDER $0�o�ӫ��O�~������Q�g�b��$N�C"
        , ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }
  }

  /* �۴� */
  if ( victim == ch )
  {
    send_to_char( "�z�T�A����ۤv�F�T�n�h�T\n\r", ch );
    striking( ch, ch, TYPE_UNDEFINED );
    RETURN_NULL();
  }

  /* �ݬݬO�_�u���i�H�}�ҾԪ� */
  if ( !can_fight( ch , victim ) ) RETURN_NULL();

  /* �p�G�w�g�԰����� */
  if ( ch->position == POS_FIGHTING )
  {
    const char * msg;

    switch( number_range( 0, 7 ) )
    {
    default:
    case 0: msg = "�A�����C���|��M���ϧ�V�O�M���⪺�}��C"     ; break;
    case 1: msg = "�A���n�۹�⪺�@�|�@�ʡM�H�ɷǳƵo�ʧ�աC"       ; break;
    case 2: msg = "�A�w�w�a���ʸ}�B�M�Q�n��X��⪺�}��C"           ; break;
    case 3: msg = "�A�ؤ�����a�n�۹�⪺�ʧ@�M�M��i�𪺳̨ήɾ��C" ; break;
    case 4: msg = "�A�������b��⨭�ǹC���ۡM�M�ߪ��M���⪺�}��C" ; break;
    case 5: msg = "�A�C�C�a���ʵ۸}�B�M�����X��C"                   ; break;
    case 6: msg = "�A���[�|���M��ť�K��M�����믫�P����񰫵ۡC"     ; break;
    case 7: msg = "�A�B�B����M���������۹��M�Q��X�L�����u�|�}�C" ; break;
    }

    act( "$t", ch, msg, NULL, TO_CHAR );
    RETURN_NULL();
  }

  WAIT_STATE( ch, PULSE_VIOLENCE );
  check_killer( ch, victim );
  striking( ch, victim, TYPE_UNDEFINED );
  RETURN_NULL();
}

/* �ѱ����a */
FUNCTION( do_murder )
{
  char        arg[MAX_INPUT_LENGTH];
  CHAR_DATA * victim;

  PUSH_FUNCTION( "do_murder" );

  one_argument( argument, arg );

  if ( !( victim = get_char_room( ch, arg ) ) )
  {
    act( "�䤣��A�n�ѱ�����H $2$T$0�C", ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  if ( victim == ch )
  {
    send_to_char( "�Ьd�� suicide �����Ч�h�T\n\r", ch );
    RETURN_NULL();
  }

  if ( IS_NPC( victim ) )
  {
    act( "�A���ӥ� $3kill$0 �ӱO��$N", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( !can_damage( victim ) )
  {
    act( "�A�ثe�S����k�ѱ�$N�T", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( !can_fight( ch , victim ) ) RETURN_NULL();

  /* �p�G�w�g�԰����� */
  if ( ch->position == POS_FIGHTING )
  {
    const char * msg;

    switch( number_range( 0, 7 ) )
    {
    default:
    case 0: msg = "�A�����C���|��M���ϧ�V�O�M���⪺�}��C"     ; break;
    case 1: msg = "�A���n�۹�⪺�@�|�@�ʡM�H�ɷǳƵo�ʧ�աC"       ; break;
    case 2: msg = "�A�w�w�a���ʸ}�B�M�Q�n��X��⪺�}��C"           ; break;
    case 3: msg = "�A�ؤ�����a�n�۹�⪺�ʧ@�M�M��i�𪺳̨ήɾ��C" ; break;
    case 4: msg = "�A�������b��⨭�ǹC���ۡM�M�ߪ��M���⪺�}��C" ; break;
    case 5: msg = "�A�C�C�a���ʵ۸}�B�M�����X��C"                   ; break;
    case 6: msg = "�A���[�|���M��ť�K��M�����믫�P����񰫵ۡC"     ; break;
    case 7: msg = "�A�B�B����M���������۹��M�Q��X�L�����u�|�}�C" ; break;
    }

    act( "$t", ch, msg, NULL, TO_CHAR );
    RETURN_NULL();
  }

  /* Delay �ɶ� */
  WAIT_STATE( ch, PULSE_VIOLENCE );
  striking( ch, victim, TYPE_UNDEFINED );
  RETURN_NULL();
}

FUNCTION( do_flee )
{
  ENABLE_DATA     * pEnable;
  SKILL_DATA      * pSkill;
  CHAR_DATA       * victim;

  PUSH_FUNCTION( "do_flee" );

  if ( !( victim = ch->fighting ) )
  {
    if ( ch->position == POS_FIGHTING ) ch->position = POS_STANDING;
    send_to_char( "�A�èS���԰��C�C\n\r", ch );
    RETURN_NULL();
  }

  for ( pEnable = ch->enable; pEnable; pEnable = pEnable->next )
  {
    if ( ( pSkill = pEnable->skill )
      && pSkill->type == TAR_FLEE
      && check_skill_cast( ch, pSkill, FALSE ) )
    {
      driver_flee( pSkill, ch );
      RETURN_NULL();
    }
  }

  for ( pSkill = skill_list; pSkill; pSkill = pSkill->next )
  {
    if ( pSkill->type == TAR_FLEE
      && ch->skill[pSkill->slot] > 0
      && check_skill_cast( ch, pSkill, FALSE ) )
    {
      driver_flee( pSkill, ch );
      RETURN_NULL();
    }
  }

  send_to_char( "�A�}�l���b�a�W�˦��M���o�ݰ_�Ӧ��G�S���ΡC\n\r", ch );
  act( "$n�}�l���b�a�W�˦��M���o�ݰ_�Ӧ��G�S���ΡC", ch, NULL, NULL, TO_ROOM );
  RETURN_NULL();
}

/* �@�ۥ��� */
FUNCTION( do_slay )
{
  CHAR_DATA * victim;
  char        arg[MAX_INPUT_LENGTH];

  PUSH_FUNCTION( "do_slay" );

  one_argument( argument, arg );

  if ( !arg[0] )
  {
    send_to_char( "�A�Q�N��ܨ��֪��ͩR�S\n\r", ch );
    RETURN_NULL();
  }

  if ( !( victim = get_char_room( ch, arg ) ) )
  {
    act( "�A���ؼ� $2$T$0 ���b�o�̡C", ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  if ( ch == victim )
  {
    send_to_char( "�A���O�Q�۱��a�S\n\r", ch );
    RETURN_NULL();
  }

  if ( !IS_NPC( victim ) && victim->level >= ch->level )
  {
    send_to_char( "�A���ѤF�C\n\r", ch );
    RETURN_NULL();
  }

  if ( !can_damage( victim ) )
  {
    act( "$N�w�g���F�M�A��������L�C", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( !IS_NPC( victim ) && victim->jail > 0 )
  {
    act( "$N���b���D���M�ҥH��������L�T", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( !IS_NPC( victim ) && victim->failed > 0 )
  {
    act( "$N���b��L�V���M�ҥH��������L�T", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  act( "�A�N��a�ܨ�$N���ͩR�C", ch, NULL, victim, TO_CHAR    );
  act( "$n�N��a�ܨ��A���ͩR�C", ch, NULL, victim, TO_VICT    );
  act( "$n�N��a�ܨ�$N���ͩR�C", ch, NULL, victim, TO_NOTVICT );

  raw_kill( ch , victim, TRUE );
  RETURN_NULL();
}

/* �ݬݬO�_�i�H�԰� ch �O�I�k��, victim �O��H.*/
bool can_fight( CHAR_DATA * ch , CHAR_DATA * victim )
{
  AREA_DATA * pArea;
  char        buf[ MAX_STRING_LENGTH ];

  PUSH_FUNCTION( "can_fight" );

  if ( !ch || !victim || !ch->in_room )
  {
    mudlog( LOG_DEBUG, "can_fight: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  /* ��観�@�̦��` */
  if ( !can_damage( ch ) || !can_damage( victim ) ) RETURN( FALSE );

  /* �p�G�ж����X�ЬO�w����, �@�w����o�;԰� */
  if ( ch->in_room->Safe || ch->in_room->NoFight )
  {
    act( "�藍�_�M$r�O����o�;԰����C" , ch, NULL, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  /* �p�G victim ���b�� ch, �h ch �@�w�i�H���� */
  if ( victim->fighting == ch ) RETURN( TRUE );

  /* �s��Ϲ�ⵥ�ŶW�L���a���Ť��ॴ */
  if ( ch->in_room
    && ( pArea = ( ch->in_room->area ) )
    && pArea->newhand
    && IS_NPC( victim ) && !IS_NPC( ch )
    && ( victim->level > ch->level + 6 ) )
  {
    act( "$N���G��A�ٱj���M���n�e���a�M�Х��� $2lore$0 ���q�@�U�T"
      , ch, NULL, victim, TO_CHAR );

    RETURN( FALSE );
  }

  /* ��԰Ϥ@�w�i�H�� */
  if ( ch->in_room
    && ( pArea = ( ch->in_room->area ) )
    && pArea->fight ) RETURN( TRUE );

  if ( ch->firman >= FULL_FIRMAN
    && IS_NPC( victim )
    && victim->bounty
    && victim->bounty->type == BOUNTY_FIRMAN )
  {
    act( "$n�w�g����h���P�F�M�Ф��n�h���ͤF�T", ch, NULL, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  if ( ( fight_info->status == PK_JOIN || fight_info->status == PK_BET )
    && is_pk( ch ) )
  {
    send_to_char( "�b�o�q�ɶ����M�٬O�n�n���𮧡M�ݷ|�b���T\n\r", ch );
    RETURN( FALSE );
  }

  /* ���a�p�G�Q�g�b, ��������ۤv���D�H */
  if ( is_affected( ch, SLOT_CHARM_PERSON ) && ch->master == victim )
  {
    act( "�A�ߴ��ɥ��M�L�k�I�i�ۼƧ����D�H$N�C", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  /* �Y�O�D�H�@�w���ॴ, �n���ž� */
  if ( ch->master == victim || victim->master == ch )
  {
    act( "�A��������ۤv���D�H�άO�U��$N�C" , ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  if ( is_same_group( ch, victim ) && ch != victim )
  {
    act( "$N�O�A�߷R�����ͣ��M$n���i�H�ѱ��L�O�S"
      , ch, NULL, victim, TO_CHAR );
    act( "�ְk�a�T$n���Q�n�ѱ��A���N�@��T"
      , ch, NULL, victim, TO_VICT );
    RETURN( FALSE );
  }

  if ( victim->mount_by )
  {
    act( "�A��������ۤv�άO�O�H���y�M$N�C", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  if ( victim->boss && victim->boss == ch )
  {
    act( "$N�O���O�����ƤF�M�F�ܭn����$E�O�S", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  if ( victim->boss )
  {
    act( "$n�ܮ`�Ȧa����$N����M�]�����H�Q�n�����L�T"
     , victim, NULL, victim->boss, TO_ALL );

     RETURN( FALSE );
  }

  if ( victim->hirer )
  {
    act( "�A��������ۤv�άO�O�H���ħL$N�C", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  /* �Ǫ����ݩʬO������` */
  if ( IS_NPC( victim ) && IS_SET( victim->act, ACT_NOKILL ) )
  {
    act( "�A�����$N�N�O����ʡC", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

   /* �p�G���F, �٨S�^�m�X����ɤ���I�i�k�N */
  if ( !IS_NPC( ch ) && ch->pcdata->corpse )
  {
    send_to_char( "�Aı�o�����Ϥ��W�O�C\n\r" , ch );
    RETURN( FALSE );
  }

  if ( !can_damage( victim ) )
  {
    act( "�A�N��L$N�a�M�A�O���|�l���h�֪��T", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  /* �Y�O���@�̥H�W�O�����Ǫ��h�i�H�}�� */
  if ( IS_NPC( ch ) || IS_NPC( victim ) ) RETURN( TRUE );

  /* �p�G�O�۴� */
  if ( ch == victim ) RETURN( TRUE );

  /* ���H��, �s��άO�q����, �C�@�ӤH���i�H�� */
  if ( IS_SET( victim->act, PLR_KILLER )
    || IS_SET( victim->act, PLR_THIEF  )
    || IS_SET( victim->act, PLR_BOLTER ) ) RETURN( TRUE );

  /* �~�֥����j�� PK_AGE */
  if ( get_age( ch ) < pk_age )
  {
    send_to_char( "�^�U�A�@�y�R�����M�٤��b���T\n\r", ch );
    RETURN( FALSE );
  }

  /* ���|����Z�����ܤ������ */
  if ( check_same_weapon( ch, TRUE ) == FALSE ) RETURN( FALSE );

  /* ���誺���ť����j�� PK_LEVEL */
  if ( ch->level < pk_level )
  {
    send_to_char( "�A�����ť���F�i�HPK���šM�Ьd��PK�C\n\r" , ch );
    RETURN( FALSE );
  }

  /* ���誺���ť����j�� PK_LEVEL */
  if ( victim->level < pk_level )
  {
    act( "$N�����ť���F�i�HPK���šM�Ьd��PK�C" , ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  if ( ch->level > victim->level )
  {
    if ( !is_pk( ch ) || !is_pk( victim ) )
    {
      act( "�ۭt$N�O���n���欰��T", ch, NULL, victim, TO_CHAR );
      RETURN( FALSE );
    }
  }

  /* �Y�O PK �h�o�X�T��, ���ˬd�O�_�O killer  */
  if ( is_pk( ch ) && is_pk( victim ) )
  {
    if ( !ch->fighting && !victim->fighting )
    {
      str_cpy( buf, "�}���F�M�}���F�T" );
      talk_channel_2( buf, CHANNEL_PK, "" );
    }
  }

  else
  {
    if ( !IS_SET( ch->act, PLR_KILLER ) )
    {
      sprintf( buf, "�ϩR�ڡT�ڳQ%s�����F�T", mob_name( NULL, ch ) );
      ChatRecord = FALSE;
      do_shout( victim , buf );
      ChatRecord = TRUE;
    }

    check_killer( ch , victim );
  }

  RETURN( TRUE );
}

/* ���a���`���B�@ */
void dead_penalty( CHAR_DATA * victim )
{
  AREA_DATA * pArea;
  int         loop;
  int         skill;

  PUSH_FUNCTION( "dead_penalty" );

  /* �D���a�@�w���B�@ */
  if ( IS_NPC( victim ) ) RETURN_NULL();

  /* ��԰Ϥ��B�@ */
  if ( victim->in_room
    && ( pArea = victim->in_room->area )
    && pArea->fight )
  {
    send_to_char( "\n\r\n\r\e[1;33m��A�n�B, �]���A�u�O�m��, �o�@��"
      "�N�ǤF�A!\n\r\n\r\e[0m", victim );
    RETURN_NULL();
  }

  if ( victim->firman > 0 )
  {
    send_to_char( "\n\r\n\r\e[1;32m�]���A���K�����P�M�o�@���N"
      "�ǤF�A�a�T���O���o�i�K�M���o���k�T\n\r\e[0m", victim );
    victim->firman--;
    RETURN_NULL();
  }

  /* �g���I���k�s */
  victim->practice /= 2;

  /* ��C�ޯ઺���m��, ��C�Q�����@ */
  for ( loop = 0; loop < MAX_SKILL; loop++ )
  {
    if ( ( skill = victim->skill[loop] ) > 0 )
    {
      int minus;

      minus = victim->pcdata ? victim->pcdata->xname : 0;
      minus = UMAX( 0, minus );

      victim->skill[loop] = ( skill -= ( skill / 10 ) + minus );

      /* �Y�O�w�g���|�o���ޯ�F */
      if ( victim->skill[loop] <= 0 )
      {
        SKILL_DATA  * pSkill;
        ENABLE_DATA * pEnable;
        ENABLE_DATA * zEnable;

        if ( ( pSkill = get_skill( loop ) ) )
        {
          act( "�A�w�g��ѤF$w�o���ޯ�F�C", victim, pSkill, NULL, TO_CHAR );

          /* �����L���o���ޯ� */
          for ( pEnable = victim->enable; pEnable; pEnable = zEnable )
          {
            zEnable = pEnable->next;
            if ( pEnable->skill == pSkill ) extract_an_enable( victim, pSkill );
          }
        }

        victim->skill[loop] = 0;
      }
    }
  }

  RETURN_NULL();
}

/* �O�����H���O�� */
void record_kill( CHAR_DATA * ch , CHAR_DATA * victim )
{
  PUSH_FUNCTION( "record_kill" );

  /* �O���������O�� */
  if ( !IS_NPC( ch )
    && !IS_NPC( victim )
    && ch->pcdata
    && victim->pcdata )
  {
    ch->pcdata->pkwin++;
    victim->pcdata->pklost++;
  }

  else if ( !IS_NPC( ch ) && ch->pcdata )
  {
    ch->pcdata->kills++;
    ch->pcdata->kill_level += victim->level;
  }

  RETURN_NULL();
}

int get_hitroll( CHAR_DATA * ch )
{
  PUSH_FUNCTION( "get_hitroll" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "get_hitroll: �ӷ��O�Ū�" );
    RETURN( 0 );
  }

  RETURN( ch->hitroll );
}

/* ������O */
int get_damroll( CHAR_DATA * ch )
{
  int        dam;
  OBJ_DATA * wield;

  PUSH_FUNCTION( "get_damroll" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "get_damroll: �ӷ��O�Ū�" );
    RETURN( 0 );
  }

  /* ���Z���B�Ť⪺�����O��Z������ */
  if ( ( wield = get_eq_char( ch, WEAR_WIELD ) )
    && get_curr_str( ch ) < wield->value[2] )
  {
    dam = number_range( wield->value[1], wield->value[2] );
  }

  /* �S���Z�� */
  else
  {
    dam = number_range( 1, get_curr_str( ch ) );
  }

  RETURN( UMAX( 1, dam ) );
}

int get_ac( CHAR_DATA * ch )
{
  PUSH_FUNCTION( "get_ac" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "get_ac: �ӷ��O�Ū�" );
    RETURN( 0 );
  }

  RETURN( ch->armor );
}

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
*  這是由輔大化學系製作群所撰寫的遊戲﹐主體由 merc 改編而來﹐所有的版權    *
*  將會被保留﹐但歡迎大家修改﹐但我們也希望你們也能提供給大家﹐所有的商    *
*  業行為將不被允許。                                                      *
*                                                                          *
*  paul@mud.ch.fju.edu.tw                                                  *
*  lc@mud.ch.fju.edu.tw                                                    *
*                                                                          *
***************************************************************************/

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include "merc.h"

/* 區域函數 */
void    make_corpse     args( ( CHAR_DATA * ) );
void    dead_penalty    args( ( CHAR_DATA * ) );
void    group_gain      args( ( CHAR_DATA * , CHAR_DATA * ) );
int     xp_compute      args( ( CHAR_DATA * , CHAR_DATA * ) );
void    trip            args( ( CHAR_DATA * , CHAR_DATA * ) );
void    record_kill     args( ( CHAR_DATA * , CHAR_DATA * ) );
void    dam_message     args( ( CHAR_DATA * , CHAR_DATA * , int , int ) );

static ARRAY_DATA DamageMessage[] =
{
  { 15000, "\e[1;31m*****超-乎-想-像-的-創-傷*****\e[0m" },
  { 10000, "\e[1;33m****石-破-天-驚-的-創-傷****\e[0m"   },
  {  5000, "\e[1;32m***無-可-比-擬-的-創-傷***\e[0m"     },
  {  2500, "\e[1;36m***難-以-彌-補-的-創-傷***\e[0m"     },
  {  1000, "\e[1;34m***極為可怕的創傷***\e[0m"           },
  {   520, "\e[1;35m**很嚴重的創傷**\e[0m"               },
  {   480, "\e[1;37m**很嚴重的傷害**\e[0m"               },
  {   440, "\e[31m*嚴重的傷害*\e[0m"                     },
  {   400, "\e[32m*重大的打擊*\e[0m"                     },
  {   360, "\e[33m*普通的傷害*\e[0m"                     },
  {   320, "\e[34m*普通的打擊*\e[0m"                     },
  {   250, "\e[35m*輕微的傷害*\e[0m"                     },
  {   200, "\e[36m*輕微的打擊*\e[0m"                     },
  {   160, "很深的傷口"                                  },
  {   120, "普通的傷口"                                  },
  {    80, "淺淺的傷口"                                  },
  {    40, "普通的擦傷"                                  },
  {     1, "輕微的擦傷"                                  },
  {     0, "但是失手了"                                  }
};

/* 戰鬥的主要流程程式, update_hanlder 會週期性的呼叫他 */
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

    /* 沒有戰鬥的沒事, 或是閒置太久的也沒事 */
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
      send_to_char( "\n\r\n\r因為你正在戰鬥中﹐所以取消你的問題﹗\n\r\n\r", ch );
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
      act( "$n突然之間說道﹕這場戰鬥沒什麼意義﹐不打了﹐還不如回家喝茶﹗"
        "\n\r轉眼間﹐$n已經不知去向了﹗"
        , ch, NULL, NULL, TO_ROOM );
      extract_char( ch, TRUE );
      continue;
    }

    /* 若是玩家醒著, 且敵人在同一間房間 */
    if ( IS_AWAKE( ch ) && ch->in_room == victim->in_room )
      striking( ch, victim, TYPE_UNDEFINED );

    else
      stop_fighting( ch, FALSE );

    /* 剛剛的動作使敵人不見了則沒事 */
    if ( !verify_char( ch )
      || !( victim = ch->fighting )
      || !can_damage( ch )
      || !verify_char( victim )
      || !can_damage( victim )
      || !ch->in_room ) continue;

    /* MOBPROGRAM */
    mprog_hitprcnt_trigger( ch, victim );
    mprog_fight_trigger( ch, victim );

    /* 剛剛的動作使敵人不見了則沒事 */
    if ( !verify_char( ch )
      || !can_damage( ch )
      || !( victim = ch->fighting )
      || !verify_char( victim )
      || !can_damage( victim )
      || !ch->in_room ) continue;

    /* 若是同一個 group 又在同一格中 */
    for ( rch = ch->in_room->people; rch; rch = rch_next )
    {
      rch_next = rch->next_in_room;

      /* 剛剛的動作使敵人不見了則沒事 */
      if ( !verify_char( ch )
        || !can_damage( ch )
        || !( victim = ch->fighting )
        || !verify_char( victim )
        || !can_damage( victim )
        || !ch->in_room ) break;

      if ( IS_AWAKE( rch ) && !rch->fighting )
      {
        /* 同一個 group 的玩家, 會自動幫忙你, 不過他不能被迷惑 */
        if ( !IS_NPC( ch ) || is_affected( ch, SLOT_CHARM_PERSON ) )
        {
          if ( ( !IS_NPC( rch ) || is_affected( rch, SLOT_CHARM_PERSON ) )
            && is_same_group( ch, rch ) )
            striking( rch, victim, TYPE_UNDEFINED );

          continue;
        }

        /* 小鬼幫忙打 */
        if ( IS_NPC( rch ) && rch->boss == victim )
        {
          striking( rch, ch, TYPE_UNDEFINED );
          continue;
        }

        /* 同一種類 NPC 會幫忙 同類 NPC , 或是 10% 機會幫忙不同種 */
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

  /* 看看雙方是不是已經死 */
  if ( !verify_char( ch )
    || !can_damage( ch )
    || !verify_char( victim )
    || !can_damage( victim ) )
  {
    mudlog( LOG_DEBUG, "striking: 人物已經死亡" );
    RETURN_NULL();
  }

  if ( !ch->in_room || !victim->in_room )
  {
    mudlog( LOG_DEBUG, "striking: 來源不正確." );
    RETURN_NULL();
  }

  /* 啟動 Enable */
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

/* 攻傷的主要程式 */
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
    /* 看看是否可以開啟戰鬥 */
    if ( !can_fight( ch , victim ) ) RETURN_NULL();

    check_killer( ch, victim );

    if ( victim->position > POS_DEAD )
    {
      /* 若對手尚未戰鬥, 則開啟戰鬥 */
      if ( !victim->fighting ) set_fighting( victim, ch );

      /* 若主角尚未戰鬥, 則開啟戰鬥 */
      if ( !ch->fighting ) set_fighting( ch, victim );

      /* 如果對手被迷惑了, 主角可能會去攻擊他的主人 */
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

    /* 如果主人攻擊部下, 則停止追隨 */
    if ( victim->master == ch ) stop_follower( victim );

    /* 如果主人攻擊小鬼 */
    if ( victim->boss == ch   ) spirit_from_char( victim );
    if ( victim->spirit == ch ) spirit_from_char( ch );

    /* 如果主角是隱形的, 一定會現出原形 */
    if ( is_affected( ch, SLOT_INVIS ) || is_affected( ch, SLOT_MASS_INVIS ) )
    {
      affect_release( ch, SLOT_INVIS      );
      affect_release( ch, SLOT_MASS_INVIS );

      act( "$n黃光一閃﹐逐漸現出原形。", ch, NULL, NULL, TO_ROOM );
    }

    /* 如果是易容也會被識破 */
    if ( is_affected( ch, SLOT_MASK ) )
    {
      affect_release( ch, SLOT_MASK );

      act( "$n的易容術被徹底識破了﹗", ch, NULL, NULL, TO_ALL );
    }

    /* 如果是藏匿也必須出現 */

    /* 修正傷害值 */
    if ( is_affected( victim, SLOT_SANCTUARY ) ) dam /= 2;

    if ( is_affected( victim, SLOT_PROTECTION ) && IS_EVIL( ch ) )
      dam -= dam / 4;

    if ( dam < 0 )                                         dam  = 0;

    if ( dt >= TYPE_HIT )
    {
      /* 絆倒對手 */
      if ( IS_NPC( ch ) && number_percent() < ch->level / 20 )
        trip( ch, victim );
    }

    /* 若是徒手打則在這裡扣除他的體力 */
    if ( dt == TYPE_HIT )
    {
      if ( ch->move >= TYPE_HIT_COST ) ch->move -= TYPE_HIT_COST;
      else                             dam = 0;
    }

    /* 命中率 */
    hit = UMIN( MaxHitroll, UMAX( 0, get_hitroll( ch ) + 100 ) );
    dam = dam * hit / 100;

    /* 防禦力 */
    dam += get_ac( victim );

    /* 閃躲 */
    victim->dodge = 0;

    if ( exert_dodge_enable( victim, ch, TRUE ) && victim->dodge > 0 )
      dam -= victim->dodge;
  }

  /* 扣除他的生命值 */
  dam = damage_situs( victim, UMAX( 0, dam ), situs );

  /* 顯示攻傷訊息 */
  if ( victim != ch )
  {
    dam = UMAX( 0, dam );
    dam_message( ch, victim, dam, dt );
  }

  /* 神族(Immortal)若有設定不死則不會死 */
  if ( !IS_NPC( victim )
    && victim->level >= LEVEL_NODEATH
    && IS_SET( victim->act, PLR_NODEATH )
    && victim->hit < 1 )
    victim->hit = 1;

  /* 如果死了, 是不能一死再死 */
  if ( victim->pcdata && victim->pcdata->corpse && victim->hit < 1 )
    victim->hit = 1;

  /* 天牢裡不能死 */
  if ( !IS_NPC( victim ) && victim->jail > 0 && victim->hit < 1 )
    victim->hit = 1;

  /* 思過崖裡也不能死 */
  if ( !IS_NPC( victim ) && victim->failed > 0 && victim->hit < 1 )
    victim->hit = 1;

  /* 在武鬥大會裡掛點 */
  if ( is_pk( victim ) && victim->hit < 1 )
  {
    victim->hit = 1;

    if ( is_pk( ch ) )
    {
      stop_pk( ch );
      sprintf( buf, "%s\e[0m(%s)被%s\e[0m(%s)宰了﹐"
        "這場武鬥大會由%s\e[0m(%s)獲勝﹗"
        , victim->cname, victim->name, ch->cname, ch->name
        , ch->cname, ch->name );

      talk_channel_2( buf, CHANNEL_PK, "" );
      RETURN_NULL();
    }
  }

  /* 調整他目前的狀態 */
  update_pos( victim );

  if ( victim->position == POS_DEAD )
    act( "$n駕鶴西歸囉。", victim, NULL, NULL, TO_ROOM );

  /* 看看對手是否還清醒, 不然就停止戰鬥了 */
  if ( !IS_AWAKE( victim ) ) stop_fighting( victim, FALSE );

  /* 對手被幹掉了 */
  if ( victim->position == POS_DEAD )
  {
    group_gain( ch, victim );

    /* 死亡的敘述 */
    if ( !IS_NPC( victim ) )
    {
      AREA_DATA * pArea;

      mudlog( LOG_INFO , "%s 在房間號碼[ %d ]殺了 %s%s."
        , mob_name( NULL, ch )
        , (victim->in_room) ? victim->in_room->vnum : 0
        , IS_SET( victim->act, PLR_KILLER ) ? "(兇手)" :
          IS_SET( victim->act, PLR_THIEF )  ? "(小偷)" :
          IS_SET( victim->act, PLR_BOLTER ) ? "(判幫)" : ""
        , victim->name );

      if ( victim != ch )
      {
        if ( victim->in_room
          && ( pArea = ( victim->in_room->area ) )
          && pArea->fight )
        {
          sprintf( buf, "\e[1;36m%s 在對戰區之 %s 把"
            , mob_name( NULL, ch )
            , (victim->in_room) ? victim->in_room->name : "某地" );

          sprintf( buf, "%s %s 趕出去了﹗\e[0m"
            , buf, mob_name( NULL, victim ) );

          /* 紀錄勝與敗 */
          if ( ch->pcdata )     ch->pcdata->fightwin++;
          if ( victim->pcdata ) victim->pcdata->fightlost++;

        }
        else
        {
          sprintf( buf, "\e[1;36m%s 在 %s 殺了"
            , mob_name( NULL, ch )
            , (victim->in_room) ? victim->in_room->name : "某地" );

          sprintf( buf, "%s %s﹗\e[0m", buf, mob_name( NULL, victim ) );
        }
      }
      else
      {
        if ( victim->in_room
          && ( pArea = ( victim->in_room->area ) )
          && pArea->fight )
        {
          sprintf( buf, "\e[1;36m%s在對戰區之%s因太無聊而烙跑了﹗\e[0m"
            , mob_name( NULL, ch )
            , ( ch->in_room ) ? victim->in_room->name : "某地" );
        }
        else
        {
          sprintf( buf, "\e[1;36m%s在%s上吊自殺了﹗\e[0m"
            , mob_name( NULL, ch )
            , ( ch->in_room ) ? victim->in_room->name : "某地" );
        }
      }

      talk_channel_2( buf, CHANNEL_BULLETIN, "快報" );

      /* 死了對玩家的處罰 */
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

      sprintf( statement, "%s臨死前使出最後的真氣對%s喊出﹕\n\r%s\e[0m\n\r"
        , name1, name2, pString );

      send_to_all_char( statement );
    }

    Reborn = raw_kill( ch , victim, TRUE );

    if ( ch->pcdata ) ch->pcdata->ply = 0;

    /* 記錄殺人的記錄 */
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

  /* 落跑 */
  if ( IS_NPC(victim) && dam > 0 )
  {
    if ( ( IS_SET(victim->act, ACT_WIMPY ) && number_range( 0 , 1 ) == 0
      && victim->hit < get_curr_hit( victim ) / 2 )
      || ( is_affected( victim, SLOT_CHARM_PERSON ) && victim->master
      && victim->master->in_room != victim->in_room ) )
      do_flee( victim, "" );
  }

  /* 看看生命力是否低於所設定 wimpy */
  if ( !IS_NPC( victim )
    && victim->hit > 0
    && victim->hit <= victim->wimpy
    && victim->wait == 0 )
    do_flee( victim, "" );

  /* 看看是否設定斷線自動逃跑 */
  if ( !IS_NPC( victim ) && !victim->desc && IS_SET( victim->act, PLR_FLEE ) )
    do_recall( victim, "" );

  tail_chain();
  RETURN_NULL();
}

/* 看看是否會變成殺人犯 */
void check_killer( CHAR_DATA * ch, CHAR_DATA * victim )
{
  AREA_DATA * pArea;

  PUSH_FUNCTION( "check_killer" );

  /* 先找出幕後的主人 */
  while ( is_affected( victim, SLOT_CHARM_PERSON && victim->master ) )
    victim = victim->master;

  /* 檢查是否已經是殺人犯, 盜賊或是叛幫者, 或是本身是虛擬怪物 */
  if ( IS_NPC( victim )
    || IS_SET( victim->act, PLR_KILLER )
    || IS_SET( victim->act, PLR_THIEF  )
    || IS_SET( victim->act, PLR_BOLTER ) )
    RETURN_NULL();

  /* 對戰區有豁免權 */
  if ( victim->in_room
    && ( pArea = ( victim->in_room->area ) )
    && pArea->fight )
    RETURN_NULL();

  /* 如果對象被迷惑了, 帳是算到主人頭上 */
  if ( is_affected( ch, SLOT_CHARM_PERSON ) )
  {
    if ( !ch->master )
    {
      mudlog( LOG_DEBUG, "Check_killer: %s 迷惑錯誤.", ch->name );
      affect_release( ch, SLOT_CHARM_PERSON );
      RETURN_NULL();
    }

    act( "你的部下$N殺人囉﹐你現在是一個殺人犯﹗", ch->master, NULL, ch, TO_CHAR );
    SET_BIT( ch->master->act, PLR_KILLER );
    stop_follower( ch );
    RETURN_NULL();
  }

  if ( ch->boss && !IS_NPC( ch->boss ) )
  {
    act( "你的小鬼$N殺人了﹐害你成為殺人犯了﹗", ch->boss, NULL, ch, TO_CHAR );
    SET_BIT( ch->boss->act, PLR_KILLER );
    RETURN_NULL();
  }

  /* 虛擬怪物或是系統管理者, 或是自殘, 或是早已經是殺人犯 */
  if ( IS_NPC( ch )
    || ch == victim
    || IS_IMMORTAL( ch )
    || IS_SET( ch->act , PLR_KILLER ) )
    RETURN_NULL();

  /* 武鬥大會 */
  if ( is_pk( ch ) && is_pk( victim ) )
    RETURN_NULL();

  /* 剩下來就一定會變成殺人犯 */
  send_to_char( "\e[1;31m你現在是殺人兇手了﹐好膽麥走﹗\e[0m\n\r", ch );
  SET_BIT( ch->act , PLR_KILLER );
  mudlog( LOG_INFO, "[PK] %s 謀殺別人 %s."
    , ch->name, victim ? victim->name : "[某人]" );
  if ( ch->pcdata ) ch->pcdata->murder++;
  save_char_obj( ch , SAVE_FILE );
  RETURN_NULL();
}

/* 設定目標目前的狀態 */
void update_pos( CHAR_DATA * victim )
{
  PUSH_FUNCTION( "update_pos" );

  if ( !victim || !verify_char( victim ) )
  {
    mudlog( LOG_DEBUG, "update_pos: 來源有問題." );
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

/* 啟動戰鬥 */
void set_fighting( CHAR_DATA * ch, CHAR_DATA * victim )
{
  char * msg;

  PUSH_FUNCTION( "set_fighting" );

  if ( !ch || !victim || !verify_char( ch ) || !verify_char( victim ) )
  {
    mudlog( LOG_DEBUG, "set_fighting: 缺乏來源." );
    RETURN_NULL();
  }

  if ( ch->fighting )
  {
    mudlog( LOG_DEBUG , "set_fighting: %s已經戰鬥了, 卻又開啟戰鬥."
      , ch->name );

    RETURN_NULL();
  }

  /* 記仇 */
  if ( IS_NPC( ch ) && IS_SET( ch->act, ACT_ENROLL ) && !IS_NPC( victim ) )
    set_enroll( ch, victim );

  if ( is_affected( ch, SLOT_SLEEP ) ) affect_release( ch, SLOT_SLEEP );

  /* 暫存取消 */
  if ( !IS_NPC( ch ) && ch->desc ) cease_stack( ch->desc );

  ch->fighting = victim;
  ch->position = POS_FIGHTING;

  if ( victim->speak )
  {
    switch( number_range( 0,14 ) )
    {
    default:
    case  0: msg = "$Y$n喊道﹕可惡的$N﹐我要宰了你﹗$A$Z";               break;
    case  1: msg = "$Y$n喊道﹕天殺的$N﹐看我的厲害﹗$A$Z";               break;
    case  2: msg = "$Y$n喊道﹕可惡的$N﹐納命來﹗$A$Z";                   break;
    case  3: msg = "$Y$n喊道﹕哇哩勒﹗$N﹐你受死吧﹗$A$Z";               break;
    case  4: msg = "$Y$n喊道﹕哼﹗ $N﹗ 我要宰了你﹗$A$Z";               break;
    case  5: msg = "$Y$n說道﹕$N﹗ 我讓你見不到明天的太陽﹗$A$Z";        break;
    case  6: msg = "$Y$n冷冷的說﹕$N﹐亮兵刃吧﹗$A$Z";                   break;
    case  7: msg = "$Y$n說道: $N不想活了﹗想死我就成全你﹗$A$Z";         break;
    case  8: msg = "$Y$n說道: 可憐的$N, 明年的今天就是你的忌日了﹗$A$Z"; break;
    case  9: msg = "$Y$n說道: $N﹗你那麼想死我就成全你﹗$A$Z";           break;
    case 10: msg = "$Y$n說道: $N今天你賺到, 送你免費地府一日遊﹗$A$Z";   break;
    case 11: msg = "$Y$n說道: $N把你所學全部展現出來吧﹗$A$Z";           break;
    case 12: msg = "$Y$n說道: $N先去買好棺材吧﹗哈哈哈﹗$A$Z";           break;
    case 13: msg = "$Y$n說道: 不自量力﹗$N棺材本存夠了嗎﹖$A$Z";         break;
    case 14: msg = "$Y$n喊道: $N﹗你眼睛瞎了敢打我﹗$A$Z";               break;
    }

    sact( victim, ch, NULL, msg );
  }

  RETURN_NULL();
}

/* 停止戰鬥 */
void stop_fighting( CHAR_DATA * ch, bool fBoth )
{
  CHAR_DATA * fch;

  PUSH_FUNCTION( "stop_fighting" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "stop_fighting: 缺乏來源." );
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

/* 製造屍體 */
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
    mudlog( LOG_DEBUG, "make_corpse: 來源不正確." );
    RETURN_NULL();
  }

  if ( !IS_NPC( ch ) && ch->pcdata && ch->pcdata->corpse )
  {
    mudlog( LOG_DEBUG, "make_corpse: 人物已經死亡" );
    RETURN_NULL();
  }

  if ( !IS_NPC( ch ) && ch->jail > 0 )
  {
    mudlog( LOG_DEBUG, "make_corpse: 人物受刑中." );
    RETURN_NULL();
  }

  if ( !IS_NPC( ch ) && ch->failed > 0 )
  {
    mudlog( LOG_DEBUG, "make_corpse: 人物思過中." );
    RETURN_NULL();
  }

  /* 若是虛擬怪物的屍體 */
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

  /* 移出物品 */
  fKeeper = is_keeper( ch );
  for ( obj = ch->carrying; obj; obj = obj_next )
  {
    obj_next = obj->next_content;
    obj_from_char( obj );

    if ( fKeeper || obj->Inventory ) extract_obj( obj );
    else                             obj_to_obj( obj, corpse );
  }

  /* 玩家和虛擬怪物的屍體放的位置不一樣 */
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

/* 臨死前的哀嚎 */
void death_cry( CHAR_DATA * ch )
{
  ROOM_INDEX_DATA * was_in_room;
  char            * msg;
  int               door;
  int               vnum = 0;

  PUSH_FUNCTION( "death_cry" );

  /* 增加死前哀嚎聲的變化性 */
  switch ( number_range( 0 , 12 ) )
  {
  default:
    msg  = "$n橫屍街頭﹐死狀極慘﹐慘不忍睹。";
    break;

  case  0:
    msg  = "$n碰的一聲﹐倒在地上﹐\e[32m掛了\e[0m﹗";
    break;

  case  1:
    msg  = "$n的\e[1;31m鮮血\e[0m在你的盔甲上留下了死前的詛咒。";
    break;

  case  2:
    msg  = "你聞到$n在死前\e[33m脫肛\e[0m的味道。";
    vnum = ObjTurdVnum;
    break;

  case  3:
    msg  = "$n的\e[1;33m頭\e[0m(head)被砍在地上﹐眼睜睜的看著你。";
    vnum = ObjHeadVnum;
    break;

  case  4:
    msg  = "$n的\e[1;31m心臟\e[0m(heart)從$s胸口跳出來。";
    vnum = ObjHeartVnum;
    break;

  case  5:
    msg  = "$n的\e[1;32m手臂\e[0m(arm)從$s身上被砍了下來。";
    vnum = ObjArmVnum;
    break;

  case  6:
    msg  = "$n的\e[36m腿\e[0m(leg)活生生的從$s屍首中砍斷。";
    vnum = ObjLegVnum;
    break;

  case  8:
    msg  = "$n倒在地上﹐看來一動也不動了﹗";
    break;

  case  9:
    msg  = "$n身首異處﹐死狀慘不忍睹﹗";
    break;

  case 10:
    msg  = "你感覺到$n的靈魂已經離開了他的肉體﹗";
    break;

  case 11:
    msg  = "你聽到$n臨死前的慘叫聲﹐那聲音還真是殘忍﹗";
    break;

  case 12:
    msg = "$n當場血濺五步﹐看來是動不了的樣子﹗";
    break;
  }

  if ( IS_NPC( ch ) && ch->pIndexData && ch->pIndexData->reborn_vnum > 0 )
  {
    msg  = "$n的\e[1;34m身影\e[0m慢慢消失了﹐但剎那間﹗﹖";
    vnum = 0;
  }

  act( smash_act_keyword( msg, "nes" ), ch, NULL, NULL, TO_ROOM );

  /* 製造出頭顱, 手臂等等的東西 */
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

  if ( IS_NPC(ch) ) msg = "你聽到不遠處傳來一聲死前的悲鳴。";
  else              msg = "你聽到不遠處一聲某人臨死前的哀嚎。";

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

/* 殺死對手了 */
bool raw_kill( CHAR_DATA * ch , CHAR_DATA * victim, bool bCorpse )
{
  AREA_DATA * pArea;
  bool        fArea  = FALSE;
  bool        Reborn = FALSE;

  PUSH_FUNCTION( "raw_kill" );

  /* 停止戰鬥 */
  stop_fighting( victim, TRUE );

  /* 修正坐騎 */
  if ( victim->mount    ) unmount_char( victim,    victim->mount );
  if ( victim->mount_by ) unmount_char( victim->mount_by, victim );

  /* 修正小鬼 */

  /* 有些虛擬怪物有臨死前的動作 */
  if ( bCorpse ) mprog_death_trigger( victim, ( ch == victim ? NULL : ch ) );

  /* 如果可以重生 */
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

      /* 察看是否有設定重生的房間 */
      if ( victim->pIndexData->reborn_room == 0 )
      {
        pRoom = ch->in_room;
      }

      else if ( !( pRoom = get_room_index( victim->pIndexData->reborn_room )))
      {
        mudlog( LOG_DEBUG, "raw_kill: 怪物號碼 %d 重生房間 %d 不存在."
         , pMobIndex->vnum, victim->pIndexData->reborn_room );
        pRoom = ch->in_room;
      }

      /* 複製怪物 */
      if ( ( pReborn = shape_mobile( pMobIndex, pRoom ) ) )
      {
        char buffer[MAX_STRING_LENGTH];

        /* 印出訊息 */
        sprintf( buffer, "頓時﹐天雷交錯﹐煙霧瀰漫﹐四周氣流開始凝結。\n\r"
                         "慢慢浮現出一個身影﹐那不就是\e[1;32m%s\e[0m﹖\n\r"
          , mob_name( NULL, pReborn ) );

        sendmsg_to_room( buffer, pReborn );
        Reborn = TRUE;

        /* 看看是否會自動攻擊, 必需在同一間房間 */
        if ( IS_SET( victim->act, ACT_REBORN_FIGHT )
          && pReborn->in_room
          && ch->in_room == pReborn->in_room )
        {
          set_fighting( pReborn , ch );
        }
      }
    }
  }

  /* 製造出屍體 */
  if ( !Reborn && bCorpse ) make_corpse( victim );

  /* 若死者是虛擬怪物 */
  if ( IS_NPC( victim ) )
  {
    victim->pIndexData->killed++;
    kill_table[URANGE(0, victim->level, MAX_LEVEL-1)].killed++;

    if ( victim->bounty ) check_bounty( ch, victim );

    extract_char( victim, TRUE );
    RETURN( Reborn );
  }

  /* 檢查是否為對戰區域 */
  if ( victim->in_room
    && ( pArea = ( victim->in_room->area ) )
    && pArea->fight )
  fArea = TRUE;

  /* 檢查懸賞 */
  check_wanted( ch, victim );

  /* 銷毀人物, 不是真的銷毀 */
  extract_char( victim, FALSE );

  /* 暫存取消 */
  if ( !IS_NPC( victim ) && victim->desc ) cease_stack( victim->desc );

  /* 移除身上所有的法術 */
  while ( victim->affected ) affect_remove( victim, victim->affected );

  /* 清除玩家的一些資料 */
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
    /* 清除殺人犯和盜賊, 算是以死謝罪 */
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

/* 算算 group 每一個人可以分到多少經驗值 */
void group_gain( CHAR_DATA * ch, CHAR_DATA * victim )
{
  CHAR_DATA * gch;
  CHAR_DATA * lch;
  int         xp;
  int         members;

  PUSH_FUNCTION( "group_gain" );

  /* MOB 不會得到經驗值, 殺死玩家也沒有, 自盡也沒有 */
  if ( ( IS_NPC( ch ) && !ch->boss )
    || !IS_NPC(victim)
    || victim == ch ) RETURN_NULL();

  /* 同一個 group , 且在同一間房間裡面才能有經驗值 */
  for ( members = 0, gch = ch->in_room->people; gch; gch = gch->next_in_room )
    if ( is_same_group( gch, ch )
      || gch->boss == ch
      || gch->spirit == ch ) members++;

  if ( members == 0 )
  {
    mudlog( LOG_DEBUG , "Group_gain: 隊員為 0." );
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
        act( "你不要跟著肉腳$N到處跑嘛﹗", gch, NULL, ch, TO_CHAR );
        continue;
      }

      if ( gch->level - lch->level <= -6 )
      {
        send_to_char( "這個隊伍似乎不要像你這樣的肉腳。\n\r", gch );
        continue;
      }
    }

    else if ( gch->boss != lch ) continue;

    /* 是否要分經驗值 */
    if ( GroupSpiltExp ) xp = xp_compute( gch, victim ) / members;
    else                 xp = xp_compute( gch, victim );

    act( "你在這次戰鬥得到 $3$x$0 點經驗值。", gch , &xp, NULL, TO_CHAR );

    gain_exp( gch, xp );

    if ( IS_NPC( gch ) ) continue;

    /* 看看身上穿的裝備是否符合目前的陣營 */
    for ( obj = ch->carrying; obj; obj = obj_next )
    {
      obj_next = obj->next_content;

      /* 沒有裝備上的不算 */
      if ( obj->wear_loc == WEAR_NONE ) continue;

      /* 看看陣營是否相符合 */
      if ( ( obj->AntiEvil    && IS_EVIL( ch ) )
        || ( obj->AntiGood    && IS_GOOD( ch ) )
        || ( obj->AntiNeutral && IS_NEUTRAL( ch ) ) )
      {
        act( "啊﹐你被$p所厭棄了。"          , ch, obj, NULL, TO_CHAR );
        act( "$n厭棄了$p﹐趕快把它撿起來吧。", ch, obj, NULL, TO_ROOM );
        obj_from_char( obj );
        obj_to_room( obj, ch->in_room );
      }
    }
  }
  RETURN_NULL();
}

/* 計算經驗值以及計算改變的陣營 */
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
    mudlog( LOG_DEBUG, "xp_compute: 來源不正確." );
    RETURN( 0 );
  }

  /* 有關陣營的調整 */
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

  /* 找出合理的等級 */
  level = URANGE( 1, ch->level, MAX_LEVEL -1 );

  /* 女鬼沒有經驗值 */
  if ( victim->pIndexData && victim->pIndexData == MobVampire )
    RETURN( 0 );

  /* 馬匹也沒有 */
  if ( victim->tractable > 0 ) RETURN( 0 );

  /* 如果隻數小於 0, 則會沒有經驗值 */
  if ( ( count = kill_table[level].promotion ) <= 0 ) RETURN( 0 );

  /* 如果在這一步就小於 0, 那後面也鐵定等於 0 */
  if ( ( value = kill_table[level].exp / count ) <= 0 ) RETURN( 0 );

  /* 等級相差越多, 則因數越高 */
  factor = ( ( UMAX( 1, abs( ch->level - victim->level ) ) - 1 ) / 3 );

  /* 如果 factor 大於 1, 也就是等級相差正負 6 以上沒有經驗值 */
  if ( factor > 1 ) RETURN( 0 );

  factor = factor * UMAX( 2, ( ch->level / 5 ) ) + 1;

  /* 考慮是否有因數 */
  if ( MobDecrease > 0 ) factor += victim->pIndexData->killed / MobDecrease;

  /* 看看怪物經驗值是否有加成 */
  if ( victim->release )
  {
    value /= 2;
  }
  else if ( victim->pIndexData->multipile > 0 )
  {
     value = value * victim->pIndexData->multipile / 100;
  }

  /* 經驗值上下變動百分之十 */
  value = variation( value / factor, 10 );

  RETURN( value );
}

char * const attack_table[] =
{
  "擊中",
  "狠狠地打了",        /* WEAPON_HAND    */
  "狠狠地戳了",        /* WEAPON_DAGGER  */
  "拼命地砍了",        /* WEAPON_SWORD   */
  "很輕輕一掃",        /* WEAPON_BLADE   */
  "狠狠地砍了",        /* WEAPON_AXE     */
  "重重地揮了",        /* WEAPON_WHIP    */
  "猛烈地重擊",        /* WEAPON_SPEAR   */
  "用力地刺了",        /* WEAPON_PEN     */
  "努力地搥了",        /* WEAPON_HAMMER  */
  "猛烈地揮了",        /* WEAPON_CLUB    */
  "用弓射中了",        /* WEAPON_BOW     */
  "運氣攻擊"           /* WEAPON_FORCE   */
};

/* 功傷的訊息輸出函數 */
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
    mudlog( LOG_DEBUG, "dam_message: 缺乏來源." );
    RETURN_NULL();
  }

  /* 增加極限值 */
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
    vp = "並且對$E造成";
    vs = "並且對你造成";
  }

  else
  {
    vp = "";
    vs = "";
  }

  /* 徒手攻擊 */
  if ( dt == TYPE_HIT )
  {
    sprintf( buf1, "$n徒手攻擊$N﹐%s%s。"    , vp, v3 );
    sprintf( buf2, "你徒手攻擊$N﹐%s%s。"    , vp, v3 );
    sprintf( buf3, "$n徒手攻擊你﹐%s%s。"    , vs, v3 );
    sprintf( buf4, "你徒手攻擊你自己﹐%s%s。", vs, v3 );
    sprintf( buf5, "$n徒手攻擊$e自己﹐%s%s。", vp, v3 );
  }

  /* 使用技能或是武器攻擊方式 */
  else
  {
    /* 使用技能攻擊 */
    if ( dt >= 0 && dt < MAX_SKILL )
    {
      SKILL_DATA * pSkill;

      v1 = "使用";
      v2 = "攻擊";

      if ( ( pSkill = get_skill( dt ) ) ) attack = pSkill->cname;
      else                                attack = "未知的技能";
    }

    /* 使用武器 */
    else
    {
      int table_size;

      /* 計算陣列有幾個元素 */
      table_size = sizeof( attack_table ) / sizeof( attack_table[0] );
      v1 = v2 = "";

      /* 檢查是否有超出陣列, 否則會引發當機 */
      if ( dt < TYPE_HIT || dt >= TYPE_HIT + table_size )
      {
        mudlog( LOG_DEBUG, "Dam_message: %s 引發錯誤的攻擊型態 %d."
          , ch->name, dt );
        dt = TYPE_HIT;
      }

      attack = attack_table[ dt - TYPE_HIT ];
    }

    /* 格式化輸出的字串 */
    /* 互打時﹐給同一間房間的人看的 */
    sprintf( buf1, "$n%s%s%s$N﹐%s%s。"    , v1, attack, v2, vp, v3 );

    /* 互打時﹐給主格看的 */
    sprintf( buf2, "你%s%s%s$N﹐%s%s。"    , v1, attack, v2, vp, v3 );

    /* 互打時﹐給受格看的 */
    sprintf( buf3, "$n%s%s%s你﹐%s%s。"    , v1, attack, v2, vs, v3 );

    /* 自殘時﹐給自己看的 */
    sprintf( buf4, "你%s%s%s你自己﹐%s%s。", v1, attack, v2, vs, v3 );

    /* 自殘時﹐給同一間房間的人看的 */
    sprintf( buf5, "$n%s%s%s$e自己﹐%s%s。", v1, attack, v2, vp, v3 );
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

  /* 如果是自殘的輸出畫面 */
  if ( victim == ch )
  {
    act( smash_act_keyword( buf4, "nesNES" ), ch, NULL, victim, TO_CHAR );
    act( smash_act_keyword( buf5, "nesNES" ), ch, NULL, victim, TO_ROOM );
  }

  /* 攻擊別人的輸出方式 */
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
    act( "$n耍賤招把你絆倒了。",   ch, NULL, victim, TO_VICT    );
    act( "你耍賤招把$N給絆倒了。", ch, NULL, victim, TO_CHAR    );
    act( "$n耍賤招把$N給絆倒了。", ch, NULL, victim, TO_NOTVICT );
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
    send_to_char( "你想攻擊誰呢﹖\n\r", ch );
    RETURN_NULL();
  }

  if ( !( victim = get_char_room( ch, arg ) ) )
  {
    act( "你要攻擊的對象 $2$T$0 不在這裡。", ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  if ( !can_damage( victim ) )
  {
    act( "你目前沒有辦法攻擊$N﹗", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  /* 若對象是玩家且不是盜賊或是殺人犯則不行 */
  if ( is_pk( ch ) && is_pk( victim ) )
  {
    if ( fight_info->status != PK_FIGHT )
    {
      send_to_char( "在這段時間﹐還是好好地休息一下吧﹗\n\r", ch );
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
      act( "你要攻擊玩家$N﹐請用 $2MURDER$0 這個指令。"
        , ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }
  }

  else
  {
    if ( is_affected( victim, SLOT_CHARM_PERSON ) && victim->master )
    {
      act( "你必須用 $2MURDER $0這個指令才能攻擊被迷惑的$N。"
        , ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }
  }

  /* 自殘 */
  if ( victim == ch )
  {
    send_to_char( "哇﹗你打到自己了﹗好痛﹗\n\r", ch );
    striking( ch, ch, TYPE_UNDEFINED );
    RETURN_NULL();
  }

  /* 看看是否真的可以開啟戰爭 */
  if ( !can_fight( ch , victim ) ) RETURN_NULL();

  /* 如果已經戰鬥的話 */
  if ( ch->position == POS_FIGHTING )
  {
    const char * msg;

    switch( number_range( 0, 7 ) )
    {
    default:
    case 0: msg = "你眼神遊走四方﹐企圖更努力尋找對手的破綻。"     ; break;
    case 1: msg = "你正盯著對手的一舉一動﹐隨時準備發動攻勢。"       ; break;
    case 2: msg = "你緩緩地移動腳步﹐想要找出對手的破綻。"           ; break;
    case 3: msg = "你目不轉睛地盯著對手的動作﹐尋找進攻的最佳時機。" ; break;
    case 4: msg = "你不停的在對手身旁遊移著﹐專心的尋找對手的破綻。" ; break;
    case 5: msg = "你慢慢地移動著腳步﹐伺機出手。"                   ; break;
    case 6: msg = "你眼觀四路﹐耳聽八方﹐集中精神與對手纏鬥著。"     ; break;
    case 7: msg = "你步步為營﹐雙眼直視著對手﹐想找出他的防守漏洞。" ; break;
    }

    act( "$t", ch, msg, NULL, TO_CHAR );
    RETURN_NULL();
  }

  WAIT_STATE( ch, PULSE_VIOLENCE );
  check_killer( ch, victim );
  striking( ch, victim, TYPE_UNDEFINED );
  RETURN_NULL();
}

/* 謀殺玩家 */
FUNCTION( do_murder )
{
  char        arg[MAX_INPUT_LENGTH];
  CHAR_DATA * victim;

  PUSH_FUNCTION( "do_murder" );

  one_argument( argument, arg );

  if ( !( victim = get_char_room( ch, arg ) ) )
  {
    act( "找不到你要謀殺的對象 $2$T$0。", ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  if ( victim == ch )
  {
    send_to_char( "請查詢 suicide 有介紹更多﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( IS_NPC( victim ) )
  {
    act( "你應該用 $3kill$0 來屠殺$N", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( !can_damage( victim ) )
  {
    act( "你目前沒有辦法謀殺$N﹗", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( !can_fight( ch , victim ) ) RETURN_NULL();

  /* 如果已經戰鬥的話 */
  if ( ch->position == POS_FIGHTING )
  {
    const char * msg;

    switch( number_range( 0, 7 ) )
    {
    default:
    case 0: msg = "你眼神遊走四方﹐企圖更努力尋找對手的破綻。"     ; break;
    case 1: msg = "你正盯著對手的一舉一動﹐隨時準備發動攻勢。"       ; break;
    case 2: msg = "你緩緩地移動腳步﹐想要找出對手的破綻。"           ; break;
    case 3: msg = "你目不轉睛地盯著對手的動作﹐尋找進攻的最佳時機。" ; break;
    case 4: msg = "你不停的在對手身旁遊移著﹐專心的尋找對手的破綻。" ; break;
    case 5: msg = "你慢慢地移動著腳步﹐伺機出手。"                   ; break;
    case 6: msg = "你眼觀四路﹐耳聽八方﹐集中精神與對手纏鬥著。"     ; break;
    case 7: msg = "你步步為營﹐雙眼直視著對手﹐想找出他的防守漏洞。" ; break;
    }

    act( "$t", ch, msg, NULL, TO_CHAR );
    RETURN_NULL();
  }

  /* Delay 時間 */
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
    send_to_char( "你並沒有戰鬥耶。\n\r", ch );
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

  send_to_char( "你開始躺在地上裝死﹐但這看起來似乎沒有用。\n\r", ch );
  act( "$n開始躺在地上裝死﹐但這看起來似乎沒有用。", ch, NULL, NULL, TO_ROOM );
  RETURN_NULL();
}

/* 一招必殺 */
FUNCTION( do_slay )
{
  CHAR_DATA * victim;
  char        arg[MAX_INPUT_LENGTH];

  PUSH_FUNCTION( "do_slay" );

  one_argument( argument, arg );

  if ( !arg[0] )
  {
    send_to_char( "你想冷血奪走誰的生命﹖\n\r", ch );
    RETURN_NULL();
  }

  if ( !( victim = get_char_room( ch, arg ) ) )
  {
    act( "你的目標 $2$T$0 不在這裡。", ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  if ( ch == victim )
  {
    send_to_char( "你不是想自殺吧﹖\n\r", ch );
    RETURN_NULL();
  }

  if ( !IS_NPC( victim ) && victim->level >= ch->level )
  {
    send_to_char( "你失敗了。\n\r", ch );
    RETURN_NULL();
  }

  if ( !can_damage( victim ) )
  {
    act( "$N已經死了﹐你不能殺死他。", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( !IS_NPC( victim ) && victim->jail > 0 )
  {
    act( "$N正在受刑中﹐所以不能殺死他﹗", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( !IS_NPC( victim ) && victim->failed > 0 )
  {
    act( "$N正在思過崖中﹐所以不能殺死他﹗", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  act( "你冷血地奪走$N的生命。", ch, NULL, victim, TO_CHAR    );
  act( "$n冷血地奪走你的生命。", ch, NULL, victim, TO_VICT    );
  act( "$n冷血地奪走$N的生命。", ch, NULL, victim, TO_NOTVICT );

  raw_kill( ch , victim, TRUE );
  RETURN_NULL();
}

/* 看看是否可以戰鬥 ch 是施法者, victim 是對象.*/
bool can_fight( CHAR_DATA * ch , CHAR_DATA * victim )
{
  AREA_DATA * pArea;
  char        buf[ MAX_STRING_LENGTH ];

  PUSH_FUNCTION( "can_fight" );

  if ( !ch || !victim || !ch->in_room )
  {
    mudlog( LOG_DEBUG, "can_fight: 缺乏來源." );
    RETURN( FALSE );
  }

  /* 兩方有一者死亡 */
  if ( !can_damage( ch ) || !can_damage( victim ) ) RETURN( FALSE );

  /* 如果房間的旗標是安全的, 一定不能發生戰鬥 */
  if ( ch->in_room->Safe || ch->in_room->NoFight )
  {
    act( "對不起﹐$r是不能發生戰鬥的。" , ch, NULL, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  /* 如果 victim 正在打 ch, 則 ch 一定可以反擊 */
  if ( victim->fighting == ch ) RETURN( TRUE );

  /* 新手區對手等級超過玩家六級不能打 */
  if ( ch->in_room
    && ( pArea = ( ch->in_room->area ) )
    && pArea->newhand
    && IS_NPC( victim ) && !IS_NPC( ch )
    && ( victim->level > ch->level + 6 ) )
  {
    act( "$N似乎比你還強壯﹐不要送死吧﹐請先用 $2lore$0 較量一下﹗"
      , ch, NULL, victim, TO_CHAR );

    RETURN( FALSE );
  }

  /* 對戰區一定可以打 */
  if ( ch->in_room
    && ( pArea = ( ch->in_room->area ) )
    && pArea->fight ) RETURN( TRUE );

  if ( ch->firman >= FULL_FIRMAN
    && IS_NPC( victim )
    && victim->bounty
    && victim->bounty->type == BOUNTY_FIRMAN )
  {
    act( "$n已經那麼多金牌了﹐請不要多殺生了﹗", ch, NULL, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  if ( ( fight_info->status == PK_JOIN || fight_info->status == PK_BET )
    && is_pk( ch ) )
  {
    send_to_char( "在這段時間中﹐還是好好的休息﹐待會在打﹗\n\r", ch );
    RETURN( FALSE );
  }

  /* 玩家如果被迷惑, 不能攻擊自己的主人 */
  if ( is_affected( ch, SLOT_CHARM_PERSON ) && ch->master == victim )
  {
    act( "你心智盡失﹐無法施展招數攻擊主人$N。", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  /* 若是主人一定不能打, 要先宣戰 */
  if ( ch->master == victim || victim->master == ch )
  {
    act( "你不能攻擊自己的主人或是下屬$N。" , ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  if ( is_same_group( ch, victim ) && ch != victim )
  {
    act( "$N是你心愛的隊友ㄟ﹐$n怎麼可以謀殺他呢﹖"
      , ch, NULL, victim, TO_CHAR );
    act( "快逃吧﹗$n有想要謀殺你的意願喔﹗"
      , ch, NULL, victim, TO_VICT );
    RETURN( FALSE );
  }

  if ( victim->mount_by )
  {
    act( "你不能殺死自己或是別人的座騎$N。", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  if ( victim->boss && victim->boss == ch )
  {
    act( "$N是不是做錯事了﹐幹嗎要殺死$E呢﹖", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  if ( victim->boss )
  {
    act( "$n很害怕地躲到$N身後﹐因為有人想要殺死他﹗"
     , victim, NULL, victim->boss, TO_ALL );

     RETURN( FALSE );
  }

  if ( victim->hirer )
  {
    act( "你不能殺死自己或是別人的傭兵$N。", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  /* 怪物的屬性是不能殺害 */
  if ( IS_NPC( victim ) && IS_SET( victim->act, ACT_NOKILL ) )
  {
    act( "你不能動$N就是不能動。", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

   /* 如果死了, 還沒奉獻出屍體時不能施展法術 */
  if ( !IS_NPC( ch ) && ch->pcdata->corpse )
  {
    send_to_char( "你覺得渾身使不上力。\n\r" , ch );
    RETURN( FALSE );
  }

  if ( !can_damage( victim ) )
  {
    act( "你就放過$N吧﹐你是不會損失多少的﹗", ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  /* 若是有一者以上是虛擬怪物則可以開打 */
  if ( IS_NPC( ch ) || IS_NPC( victim ) ) RETURN( TRUE );

  /* 如果是自殘 */
  if ( ch == victim ) RETURN( TRUE );

  /* 殺人犯, 盜賊或是叛幫者, 每一個人都可以打 */
  if ( IS_SET( victim->act, PLR_KILLER )
    || IS_SET( victim->act, PLR_THIEF  )
    || IS_SET( victim->act, PLR_BOLTER ) ) RETURN( TRUE );

  /* 年齡必須大於 PK_AGE */
  if ( get_age( ch ) < pk_age )
  {
    send_to_char( "奉勸你一句﹕血氣方剛﹐戒之在鬥﹗\n\r", ch );
    RETURN( FALSE );
  }

  /* 不會雙手武器的話不能攻擊 */
  if ( check_same_weapon( ch, TRUE ) == FALSE ) RETURN( FALSE );

  /* 雙方的等級必須大於 PK_LEVEL */
  if ( ch->level < pk_level )
  {
    send_to_char( "你的等級未到達可以PK等級﹐請查詢PK。\n\r" , ch );
    RETURN( FALSE );
  }

  /* 雙方的等級必須大於 PK_LEVEL */
  if ( victim->level < pk_level )
  {
    act( "$N的等級未到達可以PK等級﹐請查詢PK。" , ch, NULL, victim, TO_CHAR );
    RETURN( FALSE );
  }

  if ( ch->level > victim->level )
  {
    if ( !is_pk( ch ) || !is_pk( victim ) )
    {
      act( "欺負$N是不好的行為喔﹗", ch, NULL, victim, TO_CHAR );
      RETURN( FALSE );
    }
  }

  /* 若是 PK 則發出訊息, 並檢查是否是 killer  */
  if ( is_pk( ch ) && is_pk( victim ) )
  {
    if ( !ch->fighting && !victim->fighting )
    {
      str_cpy( buf, "開打了﹐開打了﹗" );
      talk_channel_2( buf, CHANNEL_PK, "" );
    }
  }

  else
  {
    if ( !IS_SET( ch->act, PLR_KILLER ) )
    {
      sprintf( buf, "救命啊﹗我被%s攻擊了﹗", mob_name( NULL, ch ) );
      ChatRecord = FALSE;
      do_shout( victim , buf );
      ChatRecord = TRUE;
    }

    check_killer( ch , victim );
  }

  RETURN( TRUE );
}

/* 玩家死亡的處罰 */
void dead_penalty( CHAR_DATA * victim )
{
  AREA_DATA * pArea;
  int         loop;
  int         skill;

  PUSH_FUNCTION( "dead_penalty" );

  /* 非玩家一定不處罰 */
  if ( IS_NPC( victim ) ) RETURN_NULL();

  /* 對戰區不處罰 */
  if ( victim->in_room
    && ( pArea = victim->in_room->area )
    && pArea->fight )
  {
    send_to_char( "\n\r\n\r\e[1;33m算你好運, 因為你只是練習, 這一次"
      "就饒了你!\n\r\n\r\e[0m", victim );
    RETURN_NULL();
  }

  if ( victim->firman > 0 )
  {
    send_to_char( "\n\r\n\r\e[1;32m因為你有免死金牌﹐這一次就"
      "饒了你吧﹗但是死罪可免﹐活罪難逃﹗\n\r\e[0m", victim );
    victim->firman--;
    RETURN_NULL();
  }

  /* 經驗點數歸零 */
  victim->practice /= 2;

  /* 減低技能的熟練度, 減低十分之一 */
  for ( loop = 0; loop < MAX_SKILL; loop++ )
  {
    if ( ( skill = victim->skill[loop] ) > 0 )
    {
      int minus;

      minus = victim->pcdata ? victim->pcdata->xname : 0;
      minus = UMAX( 0, minus );

      victim->skill[loop] = ( skill -= ( skill / 10 ) + minus );

      /* 若是已經不會這項技能了 */
      if ( victim->skill[loop] <= 0 )
      {
        SKILL_DATA  * pSkill;
        ENABLE_DATA * pEnable;
        ENABLE_DATA * zEnable;

        if ( ( pSkill = get_skill( loop ) ) )
        {
          act( "你已經遺忘了$w這項技能了。", victim, pSkill, NULL, TO_CHAR );

          /* 取消他的這項技能 */
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

/* 記錄殺人的記錄 */
void record_kill( CHAR_DATA * ch , CHAR_DATA * victim )
{
  PUSH_FUNCTION( "record_kill" );

  /* 記錄殺死的記錄 */
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
    mudlog( LOG_DEBUG, "get_hitroll: 來源是空的" );
    RETURN( 0 );
  }

  RETURN( ch->hitroll );
}

/* 攻擊能力 */
int get_damroll( CHAR_DATA * ch )
{
  int        dam;
  OBJ_DATA * wield;

  PUSH_FUNCTION( "get_damroll" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "get_damroll: 來源是空的" );
    RETURN( 0 );
  }

  /* 有武器且空手的攻擊力比武器的爛 */
  if ( ( wield = get_eq_char( ch, WEAR_WIELD ) )
    && get_curr_str( ch ) < wield->value[2] )
  {
    dam = number_range( wield->value[1], wield->value[2] );
  }

  /* 沒有武器 */
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
    mudlog( LOG_DEBUG, "get_ac: 來源是空的" );
    RETURN( 0 );
  }

  RETURN( ch->armor );
}

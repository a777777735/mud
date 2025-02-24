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
      send_to_char( "對不起﹐目前沒有任何的懸賞名單﹗\n\r", ch );
      RETURN_NULL();
    }

    clear_buffer();
    loop = 0;

    for ( pWanted = wanted_list; pWanted; pWanted = pWanted->next )
    {
      send_to_buffer( "%2d. \e[1;32m%-12s\e[0m 花 %13s 兩銀子"
        "買 \e[1;33m%-12s\e[0m 項上人頭\n\r"
        , ++loop
        , pWanted->bountier && *pWanted->bountier ? pWanted->bountier : "不詳"
        , numberize( pWanted->reward, NULL )
        , pWanted->wanted && *pWanted->wanted ? pWanted->wanted : "不詳" );

      if ( buffer_full() ) break;
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  else
  {
    if ( !IS_NPC( ch ) && ch->level < level_limit )
    {
      act( "在你升級到$i級前﹐請不要四處結仇﹗"
        , ch, &level_limit, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch->donate > 0 )
    {
      send_to_char( "在你領救濟金的這段時間﹐請忍耐一下吧﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->trade == FALSE )
    {
      send_to_char( "你的總資產太可怕了﹐所以無法執行這個指令﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !( victim = get_char_world( ch, arg ) ) )
    {
      act( "對不起﹐找不到你要通緝的人 $2$T$0﹗", ch, NULL, arg, TO_CHAR );
      RETURN_NULL();
    }

    if ( IS_NPC( victim ) )
    {
      act( "$N不用通緝﹐直接宰掉他就對了﹗", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch == victim )
    {
      send_to_char( "你有沒有搞錯﹐要買自己的人頭﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( IS_IMMORTAL( victim ) )
    {
      act( "好歹$N也是管理者﹐放他一馬好了﹗", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( !IS_SET( victim->act, PLR_KILLER )
      && !IS_SET( victim->act, PLR_THIEF  )
      && !IS_SET( victim->act, PLR_BOLTER ) )
    {
      act( "$N沒有犯錯﹐所以你不能通緝他喔﹗", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    one_argument( argument, arg );
    if ( arg[0] == '\x0' || !is_number( arg ) )
    {
      act( "你打算花多少兩銀子買$N的項上人頭﹖", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( ( reward = atoi( arg ) ) < WantedThreshold )
    {
      act( "你出這種價錢﹐不會很寒酸嗎﹖至少要$i兩黃金啦﹗"
        , ch, &WantedThreshold, NULL, TO_CHAR );

      RETURN_NULL();
    }

    if ( ch->gold < reward )
    {
      send_to_char( "我知道你報仇心切﹐但得也有錢來使的鬼推磨﹗\n\r", ch );
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
    act( "$n花了$t兩銀子買$N的項上人頭﹗", ch, buf, victim, TO_CHAR );

    sprintf( buf1, "%s\e[0m(%s)花了%s兩銀子來買%s\e[0m(%s)的項上人頭﹗"
      , ch->cname, ch->name, buf, victim->cname, victim->name );

    talk_channel_2( buf1, CHANNEL_BULLETIN, "懸賞" );
    act( "你被$N懸賞了﹐請你走路小心一點吧﹗", victim, NULL, ch, TO_CHAR );

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
    mudlog( LOG_DEBUG, "check_wanted: 缺乏來源." );
    RETURN_NULL();
  }

  if ( IS_NPC( victim ) ) RETURN_NULL();

  if ( over_scale( ch ) )
  {
    send_to_char( "因為你的總資產已達上限﹐所以沒拿到半毛錢﹗\n\r", ch );
    RETURN_NULL();
  }

  clear_buffer();
  for ( reward = 0, pWanted = wanted_list; pWanted; pWanted = zWanted )
  {
    zWanted = pWanted->next;
    if ( !str_cmp( pWanted->wanted, victim->name ) )
    {
      send_to_buffer( "因為你除掉了%s﹐所以%s給你"
        "一筆獎金﹐約有%d兩銀子以做酬謝﹗\n\r"
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

  mudlog( LOG_DEBUG, "extract_wanted: 未發現到欲刪除的結構." );
  RETURN_NULL();
}

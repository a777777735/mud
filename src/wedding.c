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

char *  mate_name       args( ( CHAR_DATA * ) );

void check_mate( CHAR_DATA * ch )
{
  CHAR_DATA * mate;

  PUSH_FUNCTION( "check_mate" );

  if ( !ch || !ch->pcdata )
  {
    mudlog( LOG_DEBUG, "check_mate: 來源不正確." );
    RETURN_NULL();
  }

  ch->pcdata->mate = NULL;

  if ( !ch->pcdata->mater || !*ch->pcdata->mater )
  {
    ch->pcdata->couple.high = 0;
    ch->pcdata->couple.low  = 0;
    RETURN_NULL();
  }

  if ( !is_exist( ch->pcdata->mater ) )
  {
     act( "你的$t$T已經離開塵世囉。"
       , ch, mate_name( ch ), ch->pcdata->mater, TO_CHAR );

    free_string( ch->pcdata->mater );

    ch->pcdata->mater       = str_dup( "" );
    ch->pcdata->couple.high = 0;
    ch->pcdata->couple.low  = 0;

    RETURN_NULL();
  }

  for ( mate = char_list; mate; mate = mate->next )
  {
    if ( verify_char( mate )
      && !IS_NPC( mate )
      && mate->desc
      && mate->desc->connected == CON_PLAYING
      && !str_cmp( mate->name, ch->pcdata->mater ) )
    {
      if ( !mate->pcdata
        || !mate->pcdata->mater
        || mate->pcdata->mate
        || str_cmp( mate->pcdata->mater, ch->name ) )
      {
        mudlog( LOG_DEBUG, "check_mate: 設定時對象 %s 錯誤.", ch->name );

        act( "你的$t$T神經錯亂了。"
          , ch , mate_name( ch ), ch->pcdata->mater, TO_CHAR );

        free_string( ch->pcdata->mater );
        ch->pcdata->mater = str_dup( "" );

        ch->pcdata->couple.high = 0;
        ch->pcdata->couple.low  = 0;

        RETURN_NULL();
      }

      ch->pcdata->mate   = mate;
      mate->pcdata->mate = ch;

      ch->pcdata->couple.high = mate->serial.high;
      ch->pcdata->couple.low  = mate->serial.low;

      if ( mate->pcdata )
      {
        mate->pcdata->couple.high = ch->serial.high;
        mate->pcdata->couple.low  = ch->serial.low;
      }

      act( "你的$t$N在線上喔。", ch, mate_name( ch ), mate, TO_CHAR );
      act( "你的$t$N上線囉。", mate, mate_name( mate ), ch, TO_CHAR );

      RETURN_NULL();
    }
  }

  RETURN_NULL();
}

char * mate_name( CHAR_DATA * ch )
{
  PUSH_FUNCTION( "mate_name" );

  switch( ch->sex )
  {
  default:
    RETURN( "配偶" );

  case SEX_MALE:
    RETURN( "妻子" );

  case SEX_FEMALE:
    RETURN( "丈夫" );
  }
}

FUNCTION( do_marry )
{
  char        arg1[MAX_INPUT_LENGTH];
  char        arg2[MAX_INPUT_LENGTH];
  char        arg3[MAX_INPUT_LENGTH];
  char        buf[MAX_STRING_LENGTH];
  CHAR_DATA * victim;
  CHAR_DATA * other;
  CHAR_DATA * husband = NULL;
  CHAR_DATA * wife    = NULL;
  OBJ_DATA  * pObj;
  bool        selection;

  PUSH_FUNCTION( "do_marry" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  if ( !ch->pcdata )
  {
    mudlog( LOG_DEBUG, "do_marry: 來源有錯誤." );
    RETURN_NULL();
  }

  if ( get_age( ch ) < MarryAge )
  {
    send_to_char( "你的年紀太小了﹐等長大以後再說吧﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( is_affected( ch, SLOT_CHANGE_SEX ) )
  {
    send_to_char( "看來你的性別有點不太對耶﹗\n\r", ch );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg1 );

  if ( arg1[0] == '\x0' )
  {
    send_to_char( "參數錯誤﹐請查詢正確的使用方法。\n\r", ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg1, "!woo" ) )
  {
    argument = one_argument( argument, arg2 );
               one_argument( argument, arg3 );

    if ( arg2[0] == '\x0' || arg3[0] == '\x0' )
    {
      send_to_char( "參數錯誤﹐請查詢正確的使用方法。\n\r", ch );
      RETURN_NULL();
    }

    if ( !( victim = get_char_room( ch, arg2 ) ) )
    {
      act( "找不到你要追求的對象 $2$T$0。", ch, NULL, arg2, TO_CHAR );
      RETURN_NULL();
    }

    if ( IS_NPC( victim ) || !victim->pcdata )
    {
      act( "$N不是你理想的伴侶吧。", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( is_affected( victim, SLOT_MASK ) )
    {
      act( "$N正施展易容術﹗", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( is_affected( victim, SLOT_CHANGE_SEX ) )
    {
      act( "看來$N性別有點不太對耶。", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( get_age( victim ) < MarryAge )
    {
      act( "$N的年紀太小了﹐等他長大以後再說吧﹗", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( victim == ch )
    {
      send_to_char( "你是不是太自戀了﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !can_see( victim, ch ) )
    {
      act( "$N看不到你耶。", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( victim->pcdata->mate )
    {
      act( "$N已經結婚了耶﹐看來你慢了一步了。", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( victim->pcdata->wooer )
    {
      act( "$N已經有其它追求者了﹐看來你慢了一步了。", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( victim->position != POS_STANDING )
    {
      act( "$N正在忙耶。", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( ( ch->sex ==   SEX_MALE && victim->sex == SEX_FEMALE )
      || ( ch->sex == SEX_FEMALE && victim->sex == SEX_MALE   ) )
      selection = TRUE;
    else
      selection = FALSE;

    if ( selection == FALSE )
    {
      act( "追求$N是很好﹐但也先把性別弄清楚。", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( !( pObj = get_obj_carry( ch, arg3 ) ) )
    {
      send_to_char( "拿出你的誠意來嘛﹗求婚總不能兩串蕉吧。\n\r", ch );
      RETURN_NULL();
    }

    if ( pObj->wear_loc != WEAR_NONE )
    {
      act( "你還把$p穿在身上耶﹐先把它脫了吧。", ch, pObj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( pObj->address )
    {
      act( "$p是要交給別人的信﹐怎麼可以送給別人。", ch, pObj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    /* 看看是不是可以丟棄 */
    if ( !can_drop_obj( ch, pObj ) )
    {
      act( "$p是你的傳家之寶﹐不可以送給別人。", ch, pObj, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( victim->carry_number + get_obj_number( pObj )
      > can_carry_n( victim ) )
    {
      act( "$N似乎拿不動那麼多東西。", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( get_carry_weight( victim ) + get_obj_weight( pObj )
      > can_carry_w( victim ) )
    {
      act( "$N似乎拿不動那麼重的東西。", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    obj_from_char( pObj );
    obj_to_char( pObj, victim );

    act( "$n拿著$p當作向$N求婚的禮物﹗", ch, pObj, victim, TO_NOTVICT );
    act( "$n拿著$p當作向你求婚的禮物﹗"  , ch, pObj, victim, TO_VICT    );
    act( "你拿著$p當作向$N求婚的禮物﹗"  , ch, pObj, victim, TO_CHAR    );

    victim->pcdata->wooer = ch;

    if ( ch->pcdata->wooer )
    {
      if ( ch->pcdata->wooer != victim )
      {
        send_to_char( "你的追求者跟別人跑了。\n\r", ch->pcdata->wooer );
        ch->pcdata->wooer = NULL;
      }
      else
      {
        act( "$n和$N看來是天生的一對﹗", ch, NULL, victim, TO_NOTVICT );
        act( "$n和你看來是天生的一對﹗"  , ch, NULL, victim, TO_VICT    );
        act( "你和$N看來是天生的一對﹗"  , ch, NULL, victim, TO_CHAR    );
      }
    }

    RETURN_NULL();
  }

  else if ( !str_prefix( arg1, "!check" ) )
  {
    if ( ch->pcdata->mate )
    {
      send_to_char( "都已經結婚了﹐還查什麼查。\n\r", ch );
      RETURN_NULL();
    }

    if ( !( victim = ch->pcdata->wooer ) )
    {
      send_to_char( "ㄟ﹐對不起﹐你沒有追求者。\n\r", ch );
      RETURN_NULL();
    }

    act( "你的追求者是等級$i的$N。", ch, &victim->level, victim, TO_CHAR );

    RETURN_NULL();
  }

  else if ( !str_prefix( arg1, "!nod" ) )
  {
    if ( ch->pcdata->mate )
    {
      send_to_char( "都已經結婚了﹐還答應﹐你不怕別人捉姦啊。\n\r", ch );
      RETURN_NULL();
    }

    if ( !( victim = ch->pcdata->wooer ) || !victim->pcdata )
    {
      send_to_char( "你是不是想結婚想瘋了﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( !victim->in_room || victim->in_room != ch->in_room )
    {
      send_to_char( "趕快把你的追求者叫回來吧。\n\r", ch );
      RETURN_NULL();
    }

    if ( is_affected( victim, SLOT_CHANGE_SEX ) )
    {
      act( "看來$N性別有點不太對耶。", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( !can_see( victim, ch ) )
    {
      act( "$N看不到你耶。", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( victim->pcdata->wooer && victim->pcdata->wooer != ch )
    {
      act( "$N已經有其它的追求者了。", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( victim->pcdata->wooer == ch )
    {
      act( "$N已經知道你答應了啊。", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    victim->pcdata->wooer = ch;

    act( "$n和$N看來是天生的一對﹗", ch, NULL, victim, TO_NOTVICT );
    act( "$n和你看來是天生的一對﹗", ch, NULL, victim, TO_VICT    );
    act( "你和$N看來是天生的一對﹗", ch, NULL, victim, TO_CHAR    );

    RETURN_NULL();
  }

  else if ( !str_prefix( arg1, "!witness" ) )
  {
    if ( !IS_IMMORTAL( ch ) )
    {
      send_to_char( "你不是管理者﹐怎麼幫別人證婚呢﹖\n\r", ch );
      RETURN_NULL();
    }

    argument = one_argument( argument, arg2 );
               one_argument( argument, arg3 );

    if ( arg2[0] == '\x0' || arg3[0] == '\x0' )
    {
      send_to_char( "連參數都錯了﹐怎麼幫人家證婚﹐認真一點啦﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !( victim = get_char_room( ch, arg2 ) )
      || !( other  = get_char_room( ch, arg3 ) ) )
    {
      send_to_char( "找不到你要證婚的小倆口耶﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( IS_NPC( victim )
      || IS_NPC( other )
      || !victim->pcdata
      || !other->pcdata )
    {
      send_to_char( "你真的要幫「它」們證婚嗎﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( victim == ch || other == ch )
    {
      send_to_char( "你不可以自己幫自己證婚啦。\n\r", ch );
      RETURN_NULL();
    }

    if ( is_affected( victim, SLOT_CHANGE_SEX )
      || is_affected( other,  SLOT_CHANGE_SEX )
      || is_affected( victim, SLOT_MASK )
      || is_affected( other,  SLOT_MASK ) )
    {
      send_to_char( "他們之中有人施展奇怪的法術﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( victim->sex == SEX_MALE   ) husband = victim;
    if ( other->sex  == SEX_MALE   ) husband = other;
    if ( victim->sex == SEX_FEMALE ) wife    = victim;
    if ( other->sex  == SEX_FEMALE ) wife    = other;

    if ( !husband || !wife )
    {
      send_to_char( "他們的性別錯亂了﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( husband->pcdata->wooer != wife || wife->pcdata->wooer  != husband )
    {
      send_to_char( "他們並沒有互相相愛﹐何必勉強。\n\r", ch );
      RETURN_NULL();
    }

    if ( husband->pcdata->mate || wife->pcdata->mate
      || ( husband->pcdata->mater && *husband->pcdata->mater )
      || ( wife->pcdata->mater  && *wife->pcdata->mater  ) )
    {
      mudlog( LOG_DEBUG, "do_marry: 來源重覆聯結." );
      husband->pcdata->wooer = NULL;
      wife->pcdata->wooer    = NULL;

      send_to_char( "他們其中有一位已經結過婚了。\n\r", ch );
      RETURN_NULL();
    }

    /* 送給全部的人都知道 */
    sprintf( buf, "現場一片鑼鼓聲響﹐正廳堂兩旁紅燭高掛﹐張燈結綵﹐喜氣洋洋。"
         "\n\r賓客們絡繹不絕﹐紛紛到現場祝賀﹐%s(%s)和%s(%s)緩緩走到證婚大神"
         "的前方站定。\n\r讓我們恭禧他們之後將結為連理﹐共同生活﹗\n\r"
         ,husband->cname, husband->name
         , wife->cname  , wife->name );

    send_to_all_char( buf );

    str_cpy( buf, "一拜天地﹐二拜高堂﹐夫妻交拜﹐送入洞房﹗" );

    ChatRecord = FALSE;
    do_chat( ch, buf );
    ChatRecord = TRUE;

    act( "從此之後﹐你就是$N的丈夫了﹗", husband, NULL, wife, TO_CHAR );
    act( "從此之後﹐你就是$N的妻子了﹗", wife, NULL, husband, TO_CHAR );

    husband->pcdata->mater = str_dup( wife->name    );
    wife->pcdata->mater    = str_dup( husband->name );

    husband->pcdata->mate  = wife;
    wife->pcdata->mate     = husband;

    husband->pcdata->wooer = NULL;
    wife->pcdata->wooer    = NULL;

    husband->pcdata->couple.high = wife->serial.high;
    husband->pcdata->couple.low  = wife->serial.low;

    wife->pcdata->couple.high    = husband->serial.high;
    wife->pcdata->couple.low     = husband->serial.low;

    RETURN_NULL();
  }

  else if ( !str_prefix( arg1, "!refuse" ) )
  {
    if ( !( victim = ch->pcdata->wooer ) )
    {
      send_to_char( "你又沒有求愛對象﹐怎麼拒絕﹖\n\r", ch );
      RETURN_NULL();
    }

    act( "你的求愛對象$N拒絕你了﹗", victim, NULL, ch, TO_CHAR );
    act( "你拒絕了$N的求愛動作﹗", ch, NULL, victim, TO_CHAR );

    ch->pcdata->wooer = NULL;
    RETURN_NULL();
  }

  else if ( !str_prefix( arg1, "!divorce" ) )
  {
    if ( !IS_IMMORTAL( ch ) )
    {
      send_to_char( "你不是管理者﹐怎麼幫別人離婚呢﹖\n\r", ch );
      RETURN_NULL();
    }

    argument = one_argument( argument, arg2 );
               one_argument( argument, arg3 );

    if ( arg2[0] == '\x0' || arg3[0] == '\x0' )
    {
      send_to_char( "連參數都錯了﹐怎麼幫人家離婚呢﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !( victim = get_char_room( ch, arg2 ) )
      || !( other  = get_char_room( ch, arg3 ) ) )
    {
      send_to_char( "找不到你要離婚的冤家耶﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( IS_NPC( victim )
      || IS_NPC( other )
      || !victim->pcdata
      || !other->pcdata )
    {
      send_to_char( "你真的要幫「它」們離婚嗎﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( victim == ch || other == ch )
    {
      send_to_char( "你不可以自己幫自己離婚啦。\n\r", ch );
      RETURN_NULL();
    }

    if ( victim->pcdata->mate != other || other->pcdata->mate != victim )
    {
      send_to_char( "他們本來就不是夫妻了啊﹐怎麼離婚。\n\r", ch );
      RETURN_NULL();
    }

    if ( !victim->pcdata->mater
      || !*victim->pcdata->mater
      || !other->pcdata->mater
      || !*other->pcdata->mater )
    {
      send_to_char( "他們的資料有問題﹐無法離婚﹗\n\r", ch );
      mudlog( LOG_DEBUG, "do_marry: 離婚時結構有問題." );
      RETURN_NULL();
    }

    free_string( victim->pcdata->mater );
    victim->pcdata->mater = str_dup( "" );
    victim->pcdata->mate = NULL;

    free_string( other->pcdata->mater );
    other->pcdata->mater  = str_dup( "" );
    other->pcdata->mate = NULL;

    victim->pcdata->couple.high = 0;
    victim->pcdata->couple.low  = 0;

    other->pcdata->couple.high  = 0;
    other->pcdata->couple.low   = 0;

    print_to_char( ch, "你幫%s(%s)和%s(%s)離婚了。\n\r"
      , victim->cname, victim->name, other->cname, other->name );

    act( "你從此和$N再也沒有任何關係了。", victim, NULL, other, TO_CHAR );
    act( "你從此和$N再也沒有任何關係了。", other, NULL, victim, TO_CHAR );

    RETURN_NULL();
  }

  else
  {
    send_to_char( "參數錯誤﹐請查詢正確的使用方法。\n\r", ch );
    RETURN_NULL();
  }

  RETURN_NULL();
}

bool is_couple( CHAR_DATA * ch, CHAR_DATA * victim )
{
  PUSH_FUNCTION( "is_couple" );

  if ( !ch || !victim )
  {
    mudlog( LOG_DEBUG, "is_couple: 來源不正確." );
    RETURN( FALSE );
  }

  if ( IS_NPC( ch )
    || IS_NPC( victim )
    || !ch->pcdata
    || !ch->pcdata->mate
    || !victim->pcdata
    || !victim->pcdata->mate
    || ch->pcdata->mate != victim
    || victim->pcdata->mate != ch ) RETURN( FALSE );

  RETURN( TRUE );
}

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

int             pk_age          = PK_AGE;
int             pk_level        = PK_LEVEL;
int             pk_limit        = 10;           /* 上線十人以上才會開啟 */
int             PkContrastLevel = PK_CONTRAST_LEVEL;
int             MinPKGold       = MIN_PK_GOLD;
int             MaxPKGold       = MAX_PK_GOLD;
int             MaxPKTotal      = MAX_PK_TOTAL;
int             PKBetTotal      = 0;

extern void       check_contraband args( ( CHAR_DATA * ) );

ROOM_INDEX_DATA * random_pk_room  args( ( CHAR_DATA * ch ) );
void              check_contraband args( ( CHAR_DATA * ch ) );
void              set_pk          args( ( CHAR_DATA *, CLUB_DATA * ) );
void              cancel_pk       args( ( void ) );
int               pk_ante_total   args( ( int ) );
void              clear_ante      args( ( void ) );

void init_fight( FIGHT_DATA * pFight )
{
  int         count;
  AREA_DATA * pArea;
  AREA_DATA * zArea;

  PUSH_FUNCTION( "init_fight" );

  count = 0;
  zArea = NULL;

  for ( pArea = area_first; pArea; pArea = pArea->next )
    if (  pArea->pk && number_range( 0, count++ ) == 0 ) zArea = pArea;

  pFight->fight_1 = NULL;
  pFight->fight_2 = NULL;
  pFight->club_1  = NULL;
  pFight->club_2  = NULL;
  pFight->area    = zArea;
  pFight->status  = PK_JOIN;
  pFight->type    = PK_NODEFINE;

  RETURN_NULL();
}

FUNCTION( do_fight )
{
  int               which;
  int               amount;
  int               total;
  char              arg[MAX_INPUT_LENGTH];
  char              buf[MAX_STRING_LENGTH];
  ROOM_INDEX_DATA * pRoom;
  CHAR_DATA       * victim;
  CHAR_DATA       * fight_1;
  CHAR_DATA       * fight_2;

  PUSH_FUNCTION( "do_fight" );

  if ( !ch || !ch->in_room || IS_NPC( ch ) || !ch->pcdata )
    RETURN_NULL();

  if ( !fight_info->area )
  {
    act( "$t中沒有任何的競技場﹗", ch, mud_name, NULL, TO_CHAR );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    if ( fight_info->lock )
    {
      send_to_char( "對不起﹐目前武鬥大會是關閉的﹗\n\r", ch );
      RETURN_NULL();
    }

    clear_buffer();

    send_to_buffer( "目前武鬥大會的狀況是﹕" );

    switch( fight_info->status )
    {
    case PK_JOIN:
      send_to_buffer( "接受報名中﹗\n\r" );

      switch( fight_info->type )
      {
      case PK_PERSON:

        send_to_buffer( "編號一﹐個人賽甲方、" );

        if ( ( victim = fight_info->fight_1 ) )
        {
          send_to_buffer( "%s\n\r", mob_name( NULL, victim ) );
          send_to_buffer( "輸贏次數﹕ 「贏」%4d 「輸」%4d\n\r"
            , victim->pcdata ? victim->pcdata->fight_win  : 0
            , victim->pcdata ? victim->pcdata->fight_lost : 0 );
          send_to_buffer( "目前等級﹕%3d 級\n\r", victim->level );
        }

        else
        {
          send_to_buffer( "從缺\n\r" );
        }

        send_to_buffer( "編號二﹐個人賽乙方、" );

        if ( ( victim = fight_info->fight_2 )  )
        {
          send_to_buffer( "%s\n\r", mob_name( NULL, victim ) );

          send_to_buffer( "輸贏次數﹕ 「贏」%4d 「輸」%4d\n\r"
            , victim->pcdata ? victim->pcdata->fight_win  : 0
            , victim->pcdata ? victim->pcdata->fight_lost : 0 );

          send_to_buffer( "目前等級﹕%3d 級\n\r", victim->level );
        }

        else
        {
          send_to_buffer( "從缺\n\r" );
        }

        break;

      case PK_CLUB:
        break;

      default:
        send_to_buffer( "尚未有任何人報名﹗\n\r" );
        break;
      }
      break;

    case PK_BET:

      send_to_buffer( "接受外場下注中﹗\n\r" );

      if ( ( victim = fight_info->fight_1 ) )
      {
        send_to_buffer( "編號一、%s總押金﹕ %d 兩黃金。\n\r"
          , mob_name( NULL, victim ), pk_ante_total( PK_FIRST ) );

        send_to_buffer( "自己押%s﹕ %d 兩黃金。\n\r"
          , mob_name( NULL, victim ), ch->ante[PK_FIRST] );

        send_to_buffer( "輸贏記錄﹕ 「贏」%4d 「輸」%4d\n\r"
          , victim->pcdata ? victim->pcdata->fight_win  : 0
          , victim->pcdata ? victim->pcdata->fight_lost : 0 );

        send_to_buffer( "目前等級﹕%3d 級\n\r", victim->level );
      }

      if ( ( victim = fight_info->fight_2 ) )
      {
        send_to_buffer( "\n\r編號二、%s總押金﹕ %d 兩黃金。\n\r"
          , mob_name( NULL, victim ), pk_ante_total( PK_SECOND ) );

        send_to_buffer( "自己押%s﹕ %d 兩黃金。\n\r"
          , mob_name( NULL, victim ), ch->ante[PK_SECOND] );

        send_to_buffer( "輸贏記錄﹕ 「贏」%4d 「輸」%4d\n\r"
          , victim->pcdata ? victim->pcdata->fight_win  : 0
          , victim->pcdata ? victim->pcdata->fight_lost : 0 );

        send_to_buffer( "目前等級﹕%3d 級\n\r", victim->level );
      }

      break;

    case PK_FIGHT:
      send_to_buffer( "武鬥大會進行中﹐由下列兩位對決中﹗\n\r" );

      if ( ( fight_1 = fight_info->fight_1 )
        && ( fight_2 = fight_info->fight_2 ) )
      {
        send_to_buffer(
          "編號一號﹕%s\e[0m(%s)\n\r"
          "目前狀態﹕%s\n\r"
          "總押金額﹕%d 兩黃金\n\r"
          "自己押注﹕%d 兩黃金\n\r"
          "勝敗記錄﹕%4d「勝」﹕%4d「敗」\n\r"
          "編號二號﹕%s\e[0m(%s)\n\r"
          "目前狀態﹕%s\n\r"
          "總押金額﹕%d 兩黃金\n\r"
          "自己押注﹕%d 兩黃金\n\r"
          "勝敗記錄﹕%4d「勝」﹕%4d「敗」\n\r"
          , fight_1->cname
          , fight_1->name
          , status_message( fight_1 )
          , pk_ante_total( PK_FIRST )
          , ch->ante[PK_FIRST]
          , fight_1->pcdata ? fight_1->pcdata->fight_win  : 0
          , fight_1->pcdata ? fight_1->pcdata->fight_lost : 0
          , fight_2->cname
          , fight_2->name
          , status_message( fight_2 )
          , pk_ante_total( PK_SECOND )
          , ch->ante[PK_SECOND]
          , fight_2->pcdata ? fight_2->pcdata->fight_win  : 0
          , fight_2->pcdata ? fight_2->pcdata->fight_lost : 0 );
      }

      break;

    case PK_STOP:
      send_to_buffer( "清理武鬥大會現場中﹗\n\r" );
      break;

    case PK_DESERT:
      send_to_buffer( "武鬥大會閒置中﹗\n\r" );
      break;
    }

    print_buffer( ch );
  }

  else if ( !str_prefix( arg, "join" ) )
  {
    if ( ( reboot_time   > 0 && ( ( reboot_time   - current_time ) < 900 ) )
      || ( shutdown_time > 0 && ( ( shutdown_time - current_time ) < 900 ) ) )
    {
      act( "$t即將關閉﹐所以沒有辦法開啟武鬥大會。", ch, mud_name, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( IS_IMMORTAL( ch ) )
    {
      send_to_char( "你只能加加油﹐不能參加的啦﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->level <= pk_level )
    {
      chinese_number( pk_level, buf );
      act( "對不起﹐你的等級必須超過$t級才能參加武鬥大會﹗", ch, buf, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( get_age( ch ) <= pk_age )
    {
      chinese_number( pk_age, buf );
      act( "對不起﹐你的年齡必須超過$t歲才能參加武鬥大會﹗", ch, buf, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( fight_info->lock )
    {
      send_to_char( "目前武鬥大會被鎖定了﹐所以誰也無法參加﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( fight_info->status != PK_JOIN )
    {
      send_to_char( "對不起﹐目前不是報名的時候﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->master || ch->leader )
    {
      send_to_char( " 對不起﹐你正跟隨別人﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( auction_info->seller && auction_info->seller == ch )
    {
      send_to_char( "對不起﹐等你賣完東西再來報名吧﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( auction_info->buyer && auction_info->buyer == ch )
    {
      send_to_char( "對不起﹐等你買完東西再來報名吧﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->position != POS_STANDING )
    {
      send_to_char( "對不起﹐等你忙完了再參加武鬥大會吧﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->failed > 0 )
    {
      send_to_char( "你輸不夠嗎﹖等你練好一點再來吧﹗\n\r", ch );
      RETURN_NULL()
    }

    if ( PkContrastLevel > 0
      && fight_info->fight_1
      && ch->level - fight_info->fight_1->level > PkContrastLevel )
    {
      act( "你不能欺負等級比你低$i級的玩家。", ch
        , &PkContrastLevel, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch->jail > 0 )
    {
      send_to_char( "先把你的刑期服完再說吧﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( IS_SET( ch->act, PLR_KILLER )
      || IS_SET( ch->act, PLR_BOLTER )
      || IS_SET( ch->act, PLR_THIEF ) )
    {
      send_to_char( "你如果參加會讓大家知道你跑路的行蹤﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->mount )
    {
      act( "你還在$N上﹐先下馬吧﹗", ch, NULL, ch->mount, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch->spirit )
    {
      act( "對不起﹐你不能帶著$N參加武鬥大會喔﹗", ch, NULL, ch->spirit, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch->in_room && ch->in_room->NoQuit )
    {
      act( "先離開這個鬼地方$r再說吧﹗", ch, NULL, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( is_station( ch->in_room ) )
    {
      act( "你還在$r裡﹐請先下車吧﹐再來參加武鬥大會吧﹗", ch, NULL, NULL, TO_CHAR  );
      RETURN_NULL();
    }

    if ( ch->in_room->Sail )
    {
      act( "你還在$r﹐請先下船吧﹐不然無法參加武鬥大會﹗", ch, NULL, NULL, TO_CHAR  );
      RETURN_NULL();
    }

    if ( fight_info->type == PK_CLUB )
    {
      send_to_char( "目前是幫派火拼﹐你不適合參加﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( is_pk( ch ) )
    {
      send_to_char( "你已經報名了﹐不能重複報名喔﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( fight_info->area == NULL || !( pRoom = random_pk_room( ch ) ) )
    {
      send_to_char( "目前找不到可以火拼的地方﹐這個問題有點嚴重﹗\n\r", ch );
      RETURN_NULL();
    }

    check_contraband( ch );

    act( "天上飄來一朵雲﹐把$n傳送到競技場囉﹗", ch, NULL, NULL, TO_ALL );

    char_from_room( ch );
    char_to_room( ch, pRoom );
    do_look( ch, "auto" );

    /* 清除追蹤紀錄點 */
    clear_trace( ch, TRUE );

    set_pk( ch, NULL );

    sprintf( buf, "%s參加了這場武鬥大會﹐大家替他加油吧﹗"
      , mob_name( NULL, ch ) );

    talk_channel_2( buf, CHANNEL_PK, "" );

    if ( fight_info->status == PK_BET )
    {
      str_cpy( buf, "一年一度的武鬥大會已經額滿﹐請大家可以"
        "開始下注(\e[1;32mfight bet\e[0m)了﹗" );

      talk_channel_2( buf, CHANNEL_PK, "" );

      if ( ( victim = fight_info->fight_1 ) )
      {
        /* 移除身上所有的法術 */
        while ( victim->affected ) affect_remove( victim, victim->affected );
        victim->affected    = NULL;
      }

      if ( ( victim = fight_info->fight_2 ) )
      {
        /* 移除身上所有的法術 */
        while ( victim->affected ) affect_remove( victim, victim->affected );
        victim->affected     = NULL;
      }
    }
  }

  else if ( !str_prefix( arg, "bet" ) )
  {
    if ( ch->trade == FALSE )
    {
      send_to_char( "你的總資產太可怕了﹐所以無法執行這個指令﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( fight_info->lock )
    {
      send_to_char( "對不起﹐目前武鬥大會是關閉的﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( fight_info->status != PK_BET )
    {
      send_to_char( "對不起﹐目前無法下注﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !IS_NPC( ch ) && ch->level < level_limit )
    {
      act( "在你升級到$i級前﹐請不要以父母的金錢決勝負。"
        , ch, &level_limit, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch->donate > 0 )
    {
      send_to_char( "救濟金可不是讓你拿來賭博的﹗\n\r", ch );
      RETURN_NULL();
    }

    argument = one_argument( argument, arg );
    if ( arg[0] == '\x0' )
    {
      send_to_char( "你要押哪一方贏呢﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( ( which = atoi( arg ) ) <= 0 || which > 2 )
    {
      send_to_char( "你只能選擇一(\e[1;32m1\e[0m)或二(\e[1;32m2\e[0m)"
                    "贏﹗\n\r", ch );

      RETURN_NULL();
    }

    if ( ( fight_info->fight_1 && ( fight_info->fight_1 == ch ) && ( which == 2 ) )
      || ( fight_info->fight_2 && ( fight_info->fight_2 == ch ) && ( which == 1 ) ) )
    {
      send_to_char( "你對自己都沒有信心了﹐誰還對你有信心呢﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->ante[--which] > 0 )
    {
      send_to_char( "你只能押一次賭金﹗\n\r", ch );
      RETURN_NULL();
    }

    argument = one_argument( argument, arg );
    if ( arg[0] == '\x0' )
    {
      send_to_char( "你到底要押多少錢賭他贏呢﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( ( amount = atoi( arg ) ) < MinPKGold )
    {
      send_to_char( "你的押金也未免太小了吧﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( ( amount = atoi( arg ) ) > MaxPKGold )
    {
      send_to_char( "你的押金太大了﹐你對他那麼有信心嗎﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->gold < amount )
    {
      send_to_char( "真是賭性堅強﹐但是先看看你自己的口袋吧﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( ( ( total = pk_ante_total( which ) ) + amount ) > MaxPKTotal )
    {
      send_to_char( "ㄟ﹐這一邊的賭金太大了﹐所以無法繼續押注﹗\n\r", ch );
      RETURN_NULL();
    }

    gold_from_char( ch, amount );
    ch->ante[which] = amount;
    PKBetTotal     += amount;

    switch( fight_info->type )
    {
    case PK_CLUB:
      break;

    case PK_PERSON:

      if ( which == PK_FIRST && ( victim = fight_info->fight_1 ) )
      {
        if ( ch == victim )
        {
          sprintf( buf, "%s對自己十分有信心﹐押了%d兩黃金買自己贏﹗"
            , mob_name( NULL, ch ), amount );
        }

        else
        {
          sprintf( buf, "%s\e[0m(%s)對%s\e[0m(%s)十分有信心﹐"
            "押了%d兩黃金買他贏﹗"
            , ch->cname, ch->name, victim->cname, victim->name, amount );
        }

        talk_channel_2( buf, CHANNEL_PK, "" );
      }

      else if ( which == PK_SECOND && ( victim = fight_info->fight_2 ) )
      {
        if ( ch == victim )
        {
          sprintf( buf, "%s對自己十分有信心﹐押了%d兩黃金買自己贏﹗"
            , mob_name( NULL, ch ), amount );
        }

        else
        {
          sprintf( buf, "%s\e[0m(%s)對%s\e[0m(%s)十分有信心﹐"
            "押了%d兩黃金買他贏﹗"
            , ch->cname, ch->name, victim->cname, victim->name, amount );
        }

        talk_channel_2( buf, CHANNEL_PK, "" );
      }

      break;
    }
  }

  else if ( !str_prefix( arg, "status" ) && IS_IMMORTAL( ch ) )
  {
    clear_buffer();
    send_to_buffer(
      "%s\e[0m武鬥大會狀況如下﹕\n\r%s"
      "上線人數  ﹕%d 人以上才會舉辦\n\r"
      "武鬥次數  ﹕%d 次\n\r"
      "開啟次數  ﹕%d 次\n\r"
      "鎖定狀況  ﹕%s\n\r"
      "最小押金  ﹕%d 兩黃金\n\r"
      "最大押金  ﹕%d 兩黃金\n\r"
      "最大總押金﹕%d 兩黃金\n\r"
      "累計總押金﹕%d 兩黃金\n\r"
      "目前狀況  ﹕"
      , mud_name, VERTICAL_LINE
      , pk_limit
      , fight_info->time, fight_info->open
      , fight_info->lock ? "鎖定" : "正常"
      , MinPKGold, MaxPKGold, MaxPKTotal, PKBetTotal );

    switch( fight_info->status )
    {
    default:
      send_to_buffer( "不詳﹗\n\r" );
      break;

    case PK_JOIN:
      send_to_buffer( "接受報名中﹗\n\r" );
      break;

    case PK_BET:
      send_to_buffer( "接受外場下注中﹗\n\r" );
      break;

    case PK_FIGHT:
      send_to_buffer( "武鬥大會對決中﹗\n\r" );
      break;

    case PK_STOP:
      send_to_buffer( "清理武鬥大會現場中﹗\n\r" );
      break;

    case PK_DESERT:
      send_to_buffer( "武鬥大會閒置中﹗\n\r" );
      break;
    }

    print_buffer( ch );
  }

  else if ( !str_prefix( arg, "lock" ) && IS_IMMORTAL( ch ) )
  {
    switch( fight_info->lock )
    {
    case TRUE:
      fight_info->lock = FALSE;
      send_to_char( "你把武鬥大會解除鎖定了﹗\n\r", ch );
      break;

    case FALSE:
      fight_info->lock = TRUE;
      send_to_char( "你把武鬥大會鎖定了﹗\n\r", ch );
      break;
    }
  }

  else
  {
    send_to_char( "你的語法錯誤﹐請查詢 fight 的使用方法﹗\n\r", ch );
  }
  RETURN_NULL();
}

void fight_update( void )
{
  static int  Tick = 0;
  CHAR_DATA * ch;
  CHAR_DATA * fight_1;
  CHAR_DATA * fight_2;
  char        buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "fight_update" );

  /* 如果武鬥大會閒置時﹐且上線人數少於設定值﹐則不做動作 */
  if ( fight_info->status == PK_DESERT && get_usernum() < pk_limit )
     RETURN_NULL();

  switch( ++Tick )
  {
  case 1:
  case 24:

    if ( Tick == 24 )
    {
      init_fight( fight_info );
      Tick = 0;
      fight_info->time++;
    }

    if ( fight_info->status == PK_JOIN )
    {
      str_cpy( buf, "一年一度的武鬥大會又開始了﹐請大家能夠踴躍報名(fight join)﹗" );
      talk_channel_2( buf, CHANNEL_PK, "" );
    }

    else if ( fight_info->status == PK_BET )
    {
      str_cpy( buf, "武鬥大會已經敲定﹐請場外人士開始下注"
        "(\e[1;32mfight bet\e[0m)﹗" );

      talk_channel_2( buf, CHANNEL_PK, "" );
    }

    break;

  case 2:

    if ( fight_info->status == PK_JOIN )
    {
      str_cpy( buf, "一年一度的武鬥大會報名截止了﹐下次請早吧﹗" );
      talk_channel_2( buf, CHANNEL_PK, "" );

      ch = NULL;

           if ( fight_info->fight_1 ) ch = fight_info->fight_1;
      else if ( fight_info->fight_2 ) ch = fight_info->fight_2;

      if ( ch )
      {
        sprintf( buf, "只見%s躲在暗處啜泣﹐我怎麼那麼強﹐竟然找不到對手﹗"
          , mob_name( NULL, ch ) );

        talk_channel_2( buf, CHANNEL_PK, "" );
      }

      cancel_pk();
      init_fight( fight_info );
      fight_info->status = PK_DESERT;
      break;
    }

    else if ( fight_info->status == PK_BET )
    {
      str_cpy( buf, "武鬥大會已經敲定﹐請場外人士盡情下注"
        "(\e[1;32mfight bet\e[0m)﹗" );

      talk_channel_2( buf, CHANNEL_PK, "" );
    }

    break;

  case 3:

    if ( fight_info->status == PK_BET )
    {
      str_cpy( buf, "武鬥大會已經敲定﹐請場外人士努力下注"
        "(\e[1;32mfight bet\e[0m)﹗" );

      talk_channel_2( buf, CHANNEL_PK, "" );
    }

    break;

  case 4:

    if ( fight_info->status == PK_BET )
    {
      str_cpy( buf, "停止場外下注﹐武鬥大會已經開打了﹗" );
      talk_channel_2( buf, CHANNEL_PK, "" );
      fight_info->status = PK_FIGHT;
      fight_info->open++;
    }

    break;

  case 18:
  case 19:
  case 20:
    if ( fight_info->status == PK_FIGHT )
    {
      switch( fight_info->type )
      {
      default:
        break;

      case PK_PERSON:

        if ( !( fight_1 = fight_info->fight_1 ) || !fight_1->in_room ) break;
        if ( !( fight_2 = fight_info->fight_2 ) || !fight_2->in_room ) break;

        if ( !fight_1->fighting
          || fight_1->fighting != fight_2
          || !fight_2->fighting
          || fight_2->fighting != fight_1 )
        {

          if ( fight_1->in_room == fight_2->in_room )
          {
            send_to_char( "還不快點打﹐場外都已經亂成一團了﹗\n\r", fight_1 );
            send_to_char( "還不快點打﹐場外都已經亂成一團了﹗\n\r", fight_2 );
            break;
          }

          if ( fight_1->fighting ) stop_fighting( fight_1, FALSE );
          if ( fight_2->fighting ) stop_fighting( fight_2, FALSE );

          act( "$n不知道被武神拖到那裡去了﹗", fight_1, NULL, NULL, TO_ALL );

          char_from_room( fight_1 );
          char_to_room( fight_1, fight_2->in_room );

          /* 清除追蹤紀錄點 */
          clear_trace( fight_1, TRUE );

          send_to_char( "系統強制你們在一起﹐給我打﹗\n\r", fight_1 );
          send_to_char( "系統強制你們在一起﹐給我打﹗\n\r", fight_2 );
        }

        break;

      case PK_CLUB:
        break;
      }
    }
    break;

  case 22:

    if ( fight_info->status == PK_FIGHT )
    {
      stop_pk( NULL );
      str_cpy( buf, "這場武鬥大會沒有贏家﹐所以雙方都算輸﹗" );
      talk_channel_2( buf, CHANNEL_PK, "" );
    }

    break;
  }

  RETURN_NULL();
}

ROOM_INDEX_DATA * random_pk_room( CHAR_DATA * ch )
{
  int count;
  ROOM_INDEX_DATA * pRoom;
  ROOM_INDEX_DATA * zRoom;

  AREA_DATA * pArea;

  PUSH_FUNCTION( "random_pk_room" );

  if ( !ch || !( pArea = fight_info->area ) )
  {
    mudlog( LOG_DEBUG, "random_pk_room: 缺乏來源." );
    RETURN( NULL );
  }

  count = 0;
  zRoom = NULL;
  for ( count = 0, pRoom = pArea->list; pRoom; pRoom = pRoom->next_in_area )
    if ( number_range( 0, count++ ) == 0 ) zRoom = pRoom;

  RETURN( zRoom );
}

bool is_pk( CHAR_DATA * ch )
{
  PUSH_FUNCTION( "is_pk" );

  if ( !ch ) RETURN( FALSE );

  if ( ( fight_info->fight_1 && fight_info->fight_1 == ch )
    || ( fight_info->fight_2 && fight_info->fight_2 == ch ) )
    RETURN( TRUE );

  RETURN( FALSE );
}

void check_contraband( CHAR_DATA * ch )
{
  OBJ_DATA * pObj;
  OBJ_DATA * pObj_next;
  OBJ_DATA * zObj;
  OBJ_DATA * zObj_next;

  PUSH_FUNCTION( "check_contraband" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "check_contraband: 缺乏來源." );
    RETURN_NULL();
  }

  if ( IS_NPC( ch ) ) RETURN_NULL();

  act( "讓我來檢查$n身上是否有帶哪些違禁品﹗", ch, NULL, NULL, TO_ALL );
  act( "$n高舉雙手﹐十分合作地接受檢查﹗", ch, NULL, NULL, TO_ALL );

  for ( pObj = ch->carrying; pObj; pObj = pObj_next )
  {
    pObj_next = pObj->next_content;

    if ( pObj->Contraband )
    {
      act( "你身上的$p是違禁品﹐我沒收了﹗", ch, pObj, NULL, TO_CHAR );
      extract_obj( pObj );
      continue;
    }

    if ( pObj->contains )
    {
      for ( zObj = pObj->contains; zObj; zObj = zObj_next )
      {
        zObj_next = zObj->next_content;

        if ( zObj->Contraband )
        {
          act( "你身上的$p是違禁品﹐我沒收了﹗", ch, zObj, NULL, TO_CHAR );
          extract_obj( zObj );
          continue;
        }
      }
    }
  }

  RETURN_NULL();
}

void set_pk( CHAR_DATA * ch, CLUB_DATA * club )
{
  PUSH_FUNCTION( "set_pk" );

  switch( fight_info->type )
  {
  default:
    mudlog( LOG_DEBUG, "set_pk: 型態錯誤." );
    break;

  case PK_PERSON:

    if ( !fight_info->fight_1 )      fight_info->fight_1 = ch;
    else if ( !fight_info->fight_2 ) fight_info->fight_2 = ch;
    else mudlog( LOG_DEBUG, "set_pk: 個人型態名額已滿." );

    if ( fight_info->fight_1 && fight_info->fight_2 )
      fight_info->status = PK_BET;

    break;

  case PK_CLUB:

    if ( !fight_info->club_1 )       fight_info->club_1 = club;
    else if ( !fight_info->club_2 ) fight_info->club_2 = club;
    else mudlog( LOG_DEBUG, "set_pk: 幫派型態名額已滿." );

    if ( fight_info->club_1 && fight_info->club_2 )
      fight_info->status = PK_BET;

    break;

  case PK_NODEFINE:

    if ( ch )
    {
      fight_info->type = PK_PERSON;
      set_pk( ch, NULL );
    }

    else if ( club )
    {
      fight_info->type = PK_CLUB;
      set_pk( NULL, club );
    }

    else
    {
      mudlog( LOG_DEBUG, "set_pk: 型態既不是幫派也不是個人." );
    }

    break;
  }

  RETURN_NULL();
}

void cancel_pk( void )
{
  CHAR_DATA * ch;

  PUSH_FUNCTION( "cancel_pk" );

  switch( fight_info->type )
  {
  case PK_PERSON:

    if ( ( ch = fight_info->fight_1 ) && ch->in_room )
    {
      send_to_char( "對不起﹐你太強了﹐所以找不到對手﹗\n\r", ch );
      char_from_room( ch );
      char_to_room( ch, get_hometown( ch ) );

      /* 清除追蹤紀錄點 */
      clear_trace( ch, TRUE );
    }

    if ( ( ch = fight_info->fight_2 ) && ch->in_room )
    {
      send_to_char( "對不起﹐你太強了﹐所以找不到對手﹗\n\r", ch );
      char_from_room( ch );
      char_to_room( ch, get_hometown( ch ) );

      /* 清除追蹤紀錄點 */
      clear_trace( ch, TRUE );
    }

    break;

  case PK_CLUB:
    break;
  }

  RETURN_NULL();
}

void stop_pk( void * winner )
{
  CHAR_DATA * fighter = ( CHAR_DATA * ) winner;
  CHAR_DATA * fight_1 = fight_info->fight_1;
  CHAR_DATA * fight_2 = fight_info->fight_2;

  PUSH_FUNCTION( "stop_pk" );

  if ( fight_info->status != PK_FIGHT )
  {
    mudlog( LOG_DEBUG, "stop_pk: 狀態不對." );
    RETURN_NULL();
  }

  switch( fight_info->type )
  {
  default:
    mudlog( LOG_DEBUG, "stop_pk: 錯誤的型態." );
    RETURN_NULL();

  case PK_PERSON:

    if ( fight_1->fighting ) stop_fighting( fight_1, TRUE );
    if ( fight_2->fighting ) stop_fighting( fight_2, TRUE );

    fight_1->cease = TRUE;
    fight_2->cease = TRUE;

    /* 處理戰勝者 */
    if ( fighter )
    {
      if ( fighter != fight_1 && fighter != fight_2 )
      {
        mudlog( LOG_DEBUG, "stop_pk: 贏方非武鬥之人." );
        RETURN_NULL();
      }

      char_from_room( fighter );
      char_to_room( fighter, get_hometown( fighter ) );

      act( "$n打贏了武鬥大會﹐光榮地回到這裡﹗", fighter, NULL, NULL, TO_ALL );

      /* 清除追蹤紀錄點 */
      clear_trace( fighter, TRUE );

      do_look( fighter, "auto" );

      /* 發送獎金 */
      if ( fighter == fight_1 ) send_ante( PK_FIRST  );
      if ( fighter == fight_2 ) send_ante( PK_SECOND );

      if ( fighter->pcdata ) fighter->pcdata->fight_win++;
    }

    /* 處理戰敗者 */
    if ( fighter != fight_1 )
    {
      char_from_room( fight_1 );
      char_to_room( fight_1, RoomFail );

      act( "$n打輸武鬥大會﹐只好窩在這裡思過了﹗", fight_1, NULL, NULL, TO_ALL );

      /* 清除追蹤紀錄點 */
      clear_trace( fight_1, TRUE );

      fight_1->failed   = 20;
      fight_1->hit      = 1;
      fight_1->mana     = 1;
      fight_1->move     = 1;
      fight_1->position = POS_STANDING;

      do_look( fight_1, "auto" );
      if ( fight_1->pcdata ) fight_1->pcdata->fight_lost++;
    }

    if ( fighter != fight_2 )
    {
      char_from_room( fight_2 );
      char_to_room( fight_2, RoomFail );

      act( "$n打輸武鬥大會﹐只好窩在這裡思過了﹗", fight_2, NULL, NULL, TO_ALL );

      /* 清除追蹤紀錄點 */
      clear_trace( fight_2, TRUE );

      fight_2->failed   = 20;
      fight_2->hit      = 1;
      fight_2->mana     = 1;
      fight_2->move     = 1;
      fight_2->position = POS_STANDING;

      do_look( fight_2, "auto" );

      if ( fight_2->pcdata ) fight_2->pcdata->fight_lost++;
    }

    break;

  case PK_CLUB:
    break;
  }

  init_fight( fight_info );
  fight_info->status = PK_STOP;

  /* 消除彩金 */
  clear_ante();
  RETURN_NULL();
}

int pk_ante_total( int which )
{
  int         total = 0;
  CHAR_DATA * ch;

  PUSH_FUNCTION( "pk_ante_total" );

  if ( which == PK_FIRST || which == PK_SECOND )
  {
    for ( ch = char_list; ch; ch = ch->next )
    {
      if ( verify_char( ch ) && !IS_NPC( ch ) && ch->ante[which] > 0 )
        total += ch->ante[which];
    }
  }

  RETURN( total );
}

void send_ante( int which )
{
  int         amount_1;
  int         amount_2;
  int         gold;
  int         bonus;
  CHAR_DATA * ch;

  PUSH_FUNCTION( "send_ante" );

  if ( which != PK_FIRST && which != PK_SECOND )
  {
    mudlog( LOG_DEBUG, "send_ante: 來源不正確." );
    RETURN_NULL();
  }

  amount_1 = pk_ante_total( PK_FIRST );
  amount_2 = pk_ante_total( PK_SECOND );

  switch( which )
  {
  case PK_FIRST:
    bonus     = amount_2 * 2 / 10;
    amount_2 -= bonus;

    if ( fight_info->fight_1 && bonus > 0 )
    {
      act( "因為你的戰勝﹐所以你可以抽取彩金$i兩黃金﹗"
        , fight_info->fight_1, &bonus, NULL, TO_CHAR );

      if ( !over_scale( fight_info->fight_1 ) )
      {
        gold_to_char( fight_info->fight_1, bonus );
      }
      else
      {
        send_to_char( "因為你總資產已達上限﹐所沒拿到錢﹗\n\r"
          , fight_info->fight_1 );
      }
    }

    break;

  case PK_SECOND:
    bonus     = amount_1 * 2 / 10;
    amount_1 -= bonus;

    if ( fight_info->fight_2 && bonus > 0 )
    {
      act( "因為你的無敵﹐所以你可以抽取彩金$i兩黃金﹗"
        , fight_info->fight_2, &bonus, NULL, TO_CHAR );

      if ( !over_scale( fight_info->fight_2 ) )
      {
        gold_to_char( fight_info->fight_2, bonus );
      }
      else
      {
        send_to_char( "因為你總資產已達上限﹐所沒拿到錢﹗\n\r"
          , fight_info->fight_2 );
      }
    }

    break;
  }

  amount_1 /= 1000;
  amount_2 /= 1000;

  for ( ch = char_list; ch; ch = ch->next )
  {
    if ( verify_char( ch ) && !IS_NPC( ch ) && ch->ante[which] > 0 )
    {
      gold = 0;

      switch( which )
      {
      case PK_FIRST:

        if ( amount_1 > 0 ) gold = ch->ante[PK_FIRST] * amount_2 / amount_1;
        else                gold = 0;

        gold += ch->ante[PK_FIRST];
        break;

      case PK_SECOND:

        if ( amount_2 > 0 ) gold = ch->ante[PK_SECOND] * amount_1 / amount_2;
        else                gold = 0;

        gold += ch->ante[PK_SECOND];
        break;
      }

      if ( gold > 0 )
      {
        act( "因為你在武鬥大會押對了人﹐你得到彩金$i兩黃金﹗"
          , ch, &gold, NULL, TO_CHAR );

        if ( !over_scale( ch ) ) gold_to_char( ch, gold );
        else send_to_char( "因為你總資產已達上限﹐所沒拿到錢﹗\n\r", ch );
      }
    }
  }

  RETURN_NULL();
}

void clear_ante( void )
{
  CHAR_DATA * ch;

  PUSH_FUNCTION( "clear_ante" );

  for ( ch = char_list; ch; ch = ch->next )
    if ( verify_char( ch ) ) ch->ante[PK_FIRST] = ch->ante[PK_SECOND] = 0;

  RETURN_NULL();
}

void return_ante( void )
{
  CHAR_DATA * ch;
  int         total;

  PUSH_FUNCTION( "return_ante" );

  for ( ch = char_list; ch; ch = ch->next )
  {
    if ( !verify_char( ch ) )
    {
      if ( ( total = ch->ante[PK_FIRST] + ch->ante[PK_SECOND] ) > 0 )
      {
        act( "武鬥大會退還給你彩金$i兩黃金﹗", ch, &total, NULL, TO_CHAR );
      }
    }
  }

  RETURN_NULL();
}

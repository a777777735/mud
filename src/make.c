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
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include "merc.h"

ROOM_INDEX_DATA * create_new_room args( ( void ) );
bool              write_new_room  args( ( ROOM_INDEX_DATA * ) );

bool              LockCreateHome        =       FALSE;

FUNCTION( do_home )
{
  extern const sh_int   rev_dir [];
  ROOM_INDEX_DATA     * pRoom;
  ROOM_INDEX_DATA     * pHome;
  CHAR_DATA           * pMob;
  CHAR_DATA           * vch;
  CHAR_DATA           * fch;
  CHAR_DATA           * fch_next;
  EXIT_DATA           * aExit;
  EXIT_DATA           * bExit;
  int                   count;
  int                   loop;
  int                   dir;
  char                  arg[MAX_INPUT_LENGTH];
  char                  buf[MAX_STRING_LENGTH];
  char                  pellet[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "do_home" );

  if ( IS_NPC( ch ) || !ch->in_room ) RETURN_NULL();

  if ( !PlayerZone )
  {
    act( "$t裡沒有任何玩家寫的區域﹗", ch, mud_name, NULL, TO_CHAR );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' || !str_prefix( arg, "list" ) )
  {
    count = 0;
    clear_buffer();

    for ( pRoom = PlayerZone->list; pRoom; pRoom = pRoom->next_in_area )
    {
      if ( is_owner( ch, pRoom, TRUE ) )
      {
        if ( count <= 0 )
          send_to_buffer( "\e[1;33;44m順序 房間號碼 房間名稱                 "
           "                                         \e[0m\n\r" );

        send_to_buffer( "%4d %8d %s\n\r", ++count, pRoom->vnum, pRoom->name );
        if ( buffer_full() ) break;
      }
    }

    if ( count <= 0 )
      send_to_buffer( "對不起﹐你沒有認購任何的房子土地﹗\n\r" );

    print_buffer( ch );
  }

  else if ( !str_prefix( arg, "practice" ) )
  {
    if ( !MobPractice )
    {
      send_to_char( "目前系統沒有練習怪物﹐請跟神族聯絡﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !IS_NPC( ch ) && ch->level < level_limit )
    {
      act( "在你升級到$i級前﹐請不要亂花錢﹗", ch, &level_limit, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch->donate > 0 )
    {
      send_to_char( "在你領救濟金的這段時間﹐請不要亂花錢﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->bank < HomeMobile )
    {
      act( "$n銀行裡必須有$i兩黃金才能產生練習怪物。"
        , ch, &HomeMobile, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch->position != POS_STANDING )
    {
      send_to_char( "先把手邊的事情忙完了再說吧﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !( fch = owner_name( pRoom = ch->in_room ) ) || fch != ch )
    {
      send_to_char( "對不起﹐這不是你的私有財產﹐沒辦法讓你亂來﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !pRoom->Upgrade || !pRoom->PracRoom )
    {
      send_to_char( "這房間並沒有你想要的功\能﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( pRoom->mob_reborn > 0 )
    {
      send_to_char( "怪物趕製不及﹐請耐心等候﹗\n\r", ch );
      RETURN_NULL();
    }

    for ( vch = pRoom->people; vch; vch = vch->next_in_room )
    {
      if ( !vch || !verify_char( vch ) ) continue;
      if ( IS_NPC( vch ) ) break;
    }

    if ( vch )
    {
      send_to_char( "快把眼前的敵人殺了吧﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !( pMob = create_mobile( MobPractice, ch->level ) ) )
    {
      send_to_char( "無法產生怪物﹐大概機器壞掉了﹗\n\r", ch );
      mudlog( LOG_DEBUG, "do_home: Practice 無法產生怪物." );
      RETURN_NULL();
    }

    char_to_room( pMob, pRoom );
    act( "$N突然出現在這裡﹐等著被挨打﹗", ch, NULL, pMob, TO_ALL );

    /* 扣掉銀行的錢 */
    ch->bank -= ch->level * HomeMobile;

    /* 設定房間重生時間 */
    pRoom->mob_reborn = ch->level / 10;

    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "upgrade" ) )
  {
    if ( !IS_NPC( ch ) && ch->level < level_limit )
    {
      act( "在你升級到$i級前﹐請不要亂花錢﹗"
        , ch, &level_limit, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch->donate > 0 )
    {
      send_to_char( "在你領救濟金的這段時間﹐請不要亂花錢﹗\n\r", ch );
      RETURN_NULL();
    }

    argument = one_argument( argument, arg );

    if ( !arg[0] )
    {
      send_to_char( "你想要升級為什麼房間呢﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( !str_prefix( arg, "waterful" ) )
    {
      if ( ch->bank < HomeObject )
      {
        act( "$n銀行裡必須有$i兩黃金才能升級多水功\能。"
          , ch, &HomeObject, NULL, TO_CHAR );
        RETURN_NULL();
      }

      if ( ch->position != POS_STANDING )
      {
        send_to_char( "先把手邊的事情忙完了再說吧﹗\n\r", ch );
        RETURN_NULL();
      }

      if ( !( fch = owner_name( pRoom = ch->in_room ) ) || fch != ch )
      {
        send_to_char( "對不起﹐這不是你的私有財產﹐沒辦法讓你塗鴉﹗\n\r", ch );
        RETURN_NULL();
      }

      if ( pRoom->Upgrade )
      {
        act( "$r已經不能再改裝了﹗", ch, NULL, NULL, TO_CHAR );
        RETURN_NULL();
      }

      pRoom->Waterful = TRUE;
      pRoom->Upgrade  = TRUE;
      ch->bank = UMAX( 0, ch->bank - HomeObject );

      act( "你花了一筆錢把$r改裝了多水的功\能﹗", ch, NULL, NULL, TO_CHAR );
      act( "$r被$n改裝成為多水的地方了﹗", ch, NULL, NULL, TO_ROOM );
      write_new_room( pRoom );
    }

    else if ( !str_prefix( arg, "foodful" ) )
    {
      if ( ch->bank < HomeObject )
      {
        act( "$n銀行裡必須有$i兩黃金才能升級多食物功\能。"
          , ch, &HomeObject, NULL, TO_CHAR );
        RETURN_NULL();
      }

      if ( ch->position != POS_STANDING )
      {
        send_to_char( "先把手邊的事情忙完了再說吧﹗\n\r", ch );
        RETURN_NULL();
      }

      if ( !( fch = owner_name( pRoom = ch->in_room ) ) || fch != ch )
      {
        send_to_char( "對不起﹐這不是你的私有財產﹐沒辦法讓你塗鴉﹗\n\r", ch );
        RETURN_NULL();
      }

      if ( pRoom->Upgrade )
      {
        act( "$r已經不能再改裝了﹗", ch, NULL, NULL, TO_CHAR );
        RETURN_NULL();
      }

      pRoom->Foodful = TRUE;
      pRoom->Upgrade  = TRUE;
      ch->bank = UMAX( 0, ch->bank - HomeObject );

      act( "你花了一筆錢把$r改裝了多食物的功\能﹗", ch, NULL, NULL, TO_CHAR );
      act( "$r被$n改裝成為多食物的地方了﹗", ch, NULL, NULL, TO_ROOM );
      write_new_room( pRoom );
    }

    else if ( !str_prefix( arg, "deposit" ) )
    {
      if ( ch->bank < HomeObject )
      {
        act( "$n銀行裡必須有$i兩黃金才能升級錢莊功\能。"
          , ch, &HomeObject, NULL, TO_CHAR );
        RETURN_NULL();
      }

      if ( ch->position != POS_STANDING )
      {
        send_to_char( "先把手邊的事情忙完了再說吧﹗\n\r", ch );
        RETURN_NULL();
      }

      if ( !( fch = owner_name( pRoom = ch->in_room ) ) || fch != ch )
      {
        send_to_char( "對不起﹐這不是你的私有財產﹐沒辦法讓你塗鴉﹗\n\r", ch );
        RETURN_NULL();
      }

      if ( pRoom->Upgrade )
      {
        act( "$r已經不能再改裝了﹗", ch, NULL, NULL, TO_CHAR );
        RETURN_NULL();
      }

      pRoom->DepositMoney  = TRUE;
      pRoom->Upgrade       = TRUE;
      ch->bank = UMAX( 0, ch->bank - HomeObject );

      act( "你花了一筆錢把$r改裝了錢莊的功\能﹗", ch, NULL, NULL, TO_CHAR );
      act( "$r被$n改裝成為錢莊的地方了﹗", ch, NULL, NULL, TO_ROOM );
      write_new_room( pRoom );
    }

    else if ( !str_prefix( arg, "store" ) )
    {
      if ( ch->bank < HomeObject )
      {
        act( "$n銀行裡必須有$i兩黃金才能升級鏢局功\能。"
          , ch, &HomeObject, NULL, TO_CHAR );
        RETURN_NULL();
      }

      if ( ch->position != POS_STANDING )
      {
        send_to_char( "先把手邊的事情忙完了再說吧﹗\n\r", ch );
        RETURN_NULL();
      }

      if ( !( fch = owner_name( pRoom = ch->in_room ) ) || fch != ch )
      {
        send_to_char( "對不起﹐這不是你的私有財產﹐沒辦法讓你塗鴉﹗\n\r", ch );
        RETURN_NULL();
      }

      if ( pRoom->Upgrade )
      {
        act( "$r已經不能再改裝了﹗", ch, NULL, NULL, TO_CHAR );
        RETURN_NULL();
      }

      pRoom->StoreRoom  = TRUE;
      pRoom->Upgrade    = TRUE;
      ch->bank = UMAX( 0, ch->bank - HomeObject );

      act( "你花了一筆錢把$r改裝了鏢局的功\能﹗", ch, NULL, NULL, TO_CHAR );
      act( "$r被$n改裝成為鏢局的地方了﹗", ch, NULL, NULL, TO_ROOM );
      write_new_room( pRoom );
    }

    else if ( !str_prefix( arg, "hitcure" ) )
    {
      if ( ch->bank < HomeObject )
      {
        act( "$n銀行裡必須有$i兩黃金才能升級生命恢復所功\能。"
          , ch, &HomeObject, NULL, TO_CHAR );
        RETURN_NULL();
      }

      if ( ch->position != POS_STANDING )
      {
        send_to_char( "先把手邊的事情忙完了再說吧﹗\n\r", ch );
        RETURN_NULL();
      }

      if ( !( fch = owner_name( pRoom = ch->in_room ) ) || fch != ch )
      {
        send_to_char( "對不起﹐這不是你的私有財產﹐沒辦法讓你塗鴉﹗\n\r", ch );
        RETURN_NULL();
      }

      if ( pRoom->Upgrade )
      {
        act( "$r已經不能再改裝了﹗", ch, NULL, NULL, TO_CHAR );
        RETURN_NULL();
      }

      pRoom->HitCure  = TRUE;
      pRoom->Upgrade  = TRUE;
      ch->bank = UMAX( 0, ch->bank - HomeObject );

      act( "你花了一筆錢把$r改裝了生命恢復所的功\能﹗", ch, NULL, NULL, TO_CHAR );
      act( "$r被$n改裝成為生命恢復所的地方了﹗", ch, NULL, NULL, TO_ROOM );
      write_new_room( pRoom );
    }

    else if ( !str_prefix( arg, "practice" ) )
    {
      if ( ch->bank < HomeObject )
      {
        act( "$n銀行裡必須有$i兩黃金才能升級練功\場功\能。"
          , ch, &HomeObject, NULL, TO_CHAR );
        RETURN_NULL();
      }

      if ( ch->position != POS_STANDING )
      {
        send_to_char( "先把手邊的事情忙完了再說吧﹗\n\r", ch );
        RETURN_NULL();
      }

      if ( !( fch = owner_name( pRoom = ch->in_room ) ) || fch != ch )
      {
        send_to_char( "對不起﹐這不是你的私有財產﹐沒辦法讓你塗鴉﹗\n\r", ch );
        RETURN_NULL();
      }

      if ( pRoom->Upgrade )
      {
        act( "$r已經不能再改裝了﹗", ch, NULL, NULL, TO_CHAR );
        RETURN_NULL();
      }

      pRoom->PracRoom = TRUE;
      pRoom->Upgrade  = TRUE;
      pRoom->Safe     = FALSE;
      pRoom->NoFight  = FALSE;
      ch->bank = UMAX( 0, ch->bank - HomeObject );

      act( "你花了一筆錢把$r改裝了練功\場的功\能﹗", ch, NULL, NULL, TO_CHAR );
      act( "$r被$n改裝成為練功\場的地方了﹗", ch, NULL, NULL, TO_ROOM );
      write_new_room( pRoom );
    }

    else if ( !str_prefix( arg, "manacure" ) )
    {
      if ( ch->bank < HomeObject )
      {
        act( "$n銀行裡必須有$i兩黃金才能升級法力恢復所功\能。"
          , ch, &HomeObject, NULL, TO_CHAR );
        RETURN_NULL();
      }

      if ( ch->position != POS_STANDING )
      {
        send_to_char( "先把手邊的事情忙完了再說吧﹗\n\r", ch );
        RETURN_NULL();
      }

      if ( !( fch = owner_name( pRoom = ch->in_room ) ) || fch != ch )
      {
        send_to_char( "對不起﹐這不是你的私有財產﹐沒辦法讓你塗鴉﹗\n\r", ch );
        RETURN_NULL();
      }

      if ( pRoom->Upgrade )
      {
        act( "$r已經不能再改裝了﹗", ch, NULL, NULL, TO_CHAR );
        RETURN_NULL();
      }

      pRoom->ManaCure = TRUE;
      pRoom->Upgrade  = TRUE;
      ch->bank = UMAX( 0, ch->bank - HomeObject );

      act( "你花了一筆錢把$r改裝了法力恢復所的功\能﹗", ch, NULL, NULL, TO_CHAR );
      act( "$r被$n改裝成為法力恢復所的地方了﹗", ch, NULL, NULL, TO_ROOM );
      write_new_room( pRoom );
    }

    else if ( !str_prefix( arg, "movecure" ) )
    {
      if ( ch->bank < HomeObject )
      {
        act( "$n銀行裡必須有$i兩黃金才能升級體力恢復所功\能。"
          , ch, &HomeObject, NULL, TO_CHAR );
        RETURN_NULL();
      }

      if ( ch->position != POS_STANDING )
      {
        send_to_char( "先把手邊的事情忙完了再說吧﹗\n\r", ch );
        RETURN_NULL();
      }

      if ( !( fch = owner_name( pRoom = ch->in_room ) ) || fch != ch )
      {
        send_to_char( "對不起﹐這不是你的私有財產﹐沒辦法讓你塗鴉﹗\n\r", ch );
        RETURN_NULL();
      }

      if ( pRoom->Upgrade )
      {
        act( "$r已經不能再改裝了﹗", ch, NULL, NULL, TO_CHAR );
        RETURN_NULL();
      }

      pRoom->MoveCure = TRUE;
      pRoom->Upgrade  = TRUE;
      ch->bank = UMAX( 0, ch->bank - HomeObject );

      act( "你花了一筆錢把$r改裝了體力恢復所的功\能﹗", ch, NULL, NULL, TO_CHAR );
      act( "$r被$n改裝成為體力恢復所的地方了﹗", ch, NULL, NULL, TO_ROOM );
      write_new_room( pRoom );
    }

    else
    {
      send_to_char( "沒有你所選擇的功\能﹗\n\r可以升級成為"
        "生命恢復(hitcure) 法力恢復(manacure) \n\r體力恢復(movecure)"
        "增加食物(foodful) 增加水源(waterful) 等選項。\n\r", ch );
      RETURN_NULL();
    }
  }

  else if ( !str_prefix( arg, "set" ) && IS_GOD( ch ) )
  {
    argument = one_argument( argument, arg );

    if ( !arg[0] || !argument[0] )
    {
      send_to_char( "你想要設定什麼東西﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( !is_number( argument ) || ( count = atoi( argument ) ) <= 0 )
    {
      send_to_char( "你的設定值必須是數字﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !str_prefix( arg, "Cost" ) )
    {
      HomeGold = count;
      act( "你把買房子的價格改為$i兩黃金﹗", ch, &count, NULL, TO_CHAR );
    }

    else if ( !str_prefix( arg, "Title" ) )
    {
      HomeGoldTitle = count;
      act( "你把更改房間名稱的價格改為$i兩黃金﹗", ch, &count, NULL, TO_CHAR );
    }

    else if ( !str_prefix( arg, "Link" ) )
    {
      HomeGoldLink = count;
      act( "你把更連結的價格改為$i兩黃金﹗", ch, &count, NULL, TO_CHAR );
    }

    else if ( !str_prefix( arg, "Descr" ) )
    {
      HomeGoldDescr = count;
      act( "你把更改房間描述的價格為$i兩黃金﹗", ch, &count, NULL, TO_CHAR );
    }

    else if ( !str_prefix( arg, "Upgrade" ) )
    {
      HomeObject = count;
      act( "你把升級房屋配備的價格改為$i兩黃金﹗", ch, &count, NULL, TO_CHAR );
    }

    else if ( !str_prefix( arg, "Practice" ) )
    {
      HomeMobile = count;
      act( "你把產生練習怪物的價格改為$i兩黃金﹗", ch, &count, NULL, TO_CHAR );
    }

    else
    {
      send_to_char( "合法的參數為 Cost Title Descr Link Upgrade Practice\n\r", ch );
    }
  }

  else if ( !str_prefix( arg, "lock" ) && IS_GOD( ch ) )
  {
    if ( LockCreateHome )
    {
      LockCreateHome = FALSE;
      send_to_char( "你把建立修改房間功\能給打開了﹗\n\r", ch );
    }
    else
    {
      LockCreateHome = TRUE;
      send_to_char( "你把建立修改房間功\能給鎖定了﹗\n\r", ch );
    }
  }

  else if ( !str_prefix( arg, "information" ) && IS_IMMORTAL( ch ) )
  {
    clear_buffer();
    send_to_buffer(
    "玩家區域檔案目錄名稱﹕\e[1;32m[\e[0m%s\e[1;32m]\e[0m\n\r"
    "玩家區域開始序號    ﹕%-9d\n\r"
    "玩家區域最大數量    ﹕%-9d\n\r"
    "買房子的價格        ﹕%-9d 元\n\r"
    "更改房間名稱的價格  ﹕%-9d 元\n\r"
    "更改房間描述的價格  ﹕%-9d 元\n\r"
    "連結出口的價格      ﹕%-9d 元\n\r"
    "升級房屋配備的價格  ﹕%-9d 元\n\r"
    "個人房間最大數      ﹕%-9d 個\n\r"
    "鎖定更改房間        ﹕%s\n\r\n\r"
    , PlayerArea
    , PlayerAreaNum
    , PlayerAreaCap
    , HomeGold
    , HomeGoldTitle
    , HomeGoldDescr
    , HomeGoldLink
    , HomeObject
    , HomeManMax
    , YESNO( LockCreateHome ) );

    count = 0;
    for ( pRoom = PlayerZone->list; pRoom; pRoom = pRoom->next_in_area )
    {
      if ( count <= 0 )
        send_to_buffer( "\e[1;33;44m順序 房間號碼 房  間  名  稱      "
         " 主人名稱                                    \e[0m\n\r" );

      tablize( FORMAT_SET,
        "NUBR VNUM     NAME                \e[0m MASTER         "
        "                             \e[0m\n\r"
        , 0, NULL, 0 );

      tablize( FORMAT_NUM,    NULL,                          ++count,   "NUBR", 4  );
      tablize( FORMAT_NUM,    NULL,                      pRoom->vnum,   "VNUM", 8  );
      tablize( FORMAT_STRING, pRoom->name,                         0,   "NAME", 20 );
      tablize( FORMAT_STRING, fullname( &(pRoom->owner) ),         0, "MASTER", 30 );
      tablize( FORMAT_RETURN, pellet, 0, NULL, sizeof( pellet ) - 1 );

      send_to_buffer( "%s", pellet );
      if ( buffer_full() ) break;
    }

    if ( count <= 0 )
      send_to_buffer( "對不起﹐%s沒有任何的認購土地﹗\n\r", mud_name );

    print_buffer( ch );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "status" ) )
  {
    if ( !( fch = owner_name( pRoom = ch->in_room ) ) || fch != ch )
    {
      send_to_char( "對不起﹐這不是你的私有財產﹐沒辦法讓你知道相關資料﹗\n\r"
        , ch );
      RETURN_NULL();
    }

    print_to_char( ch,
      "所在名稱﹕\e[1;32m[\e[0m%s\e[1;32m]\e[0m\n\r"
      "房間編號﹕(%5d)  "
      "照明與否﹕%s  "
      "是否可以寫入﹕%s\n\r"
      "房間旗標﹕%s\n\r"
      "房間地形﹕%s\n\r"
      "此地的完整描述﹕\n\r%s%s\n\r%s"
    , pRoom->name
    , pRoom->vnum
    , YESNO( !room_is_dark( pRoom ) )
    , YESNO( pRoom->writeable )
    , room_bit_name( pRoom )
    , pRoom->sector ? pRoom->sector->cname : "不詳"
    , VERTICAL_LINE
    , pRoom->description
    , VERTICAL_LINE );

    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "create" ) )
  {
    if ( !IS_NPC( ch ) && ch->level < level_limit )
    {
      act( "在你升級到$i級前﹐請不要亂花錢﹗"
        , ch, &level_limit, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch->donate > 0 )
    {
      send_to_char( "在你領救濟金的這段時間﹐請不要亂花錢﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( LockCreateHome )
    {
      send_to_char( "對不起﹐目前管理者把這個功\能給鎖定住了﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->position != POS_STANDING )
    {
      send_to_char( "對不起﹐請先解決你自己的事情吧﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->bank < HomeGold )
    {
      act( "$n銀行裡必須有$i兩黃金才能買房子。", ch, &HomeGold, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( count_home_number( ch ) > HomeManMax )
    {
      send_to_char( "你已經是本遊戲的大戶了﹐請不要再買房子了﹗\n\r",ch );
      RETURN_NULL();
    }

    if ( !( pRoom = create_new_room() ) )
    {
      send_to_char( "對不起﹐沒有辦法產生房間了﹐你就死心了吧﹗\n\r", ch );
      RETURN_NULL();
    }

    pRoom->owner.high = ch->serial.high;
    pRoom->owner.low  = ch->serial.low;
    pRoom->writeable  = TRUE;
    pRoom->player     = TRUE;
    pRoom->cost       = HomeGold;

    if ( !write_new_room( pRoom ) )
    {
      mudlog( LOG_DEBUG, "create_new_room: 無法存檔 %s.", pRoom->filename );
      send_to_char( "對不起﹐你的房間無法存檔﹗\n\r", ch );
      RETURN_NULL();
    }

    act( "$n花了$i兩黃金買了一幢豪宅﹗", ch, &HomeGold, NULL, TO_ALL );

    count = 0;
    for ( pRoom = PlayerZone->list; pRoom; pRoom = pRoom->next_in_area )
      if ( is_owner( ch, pRoom, TRUE ) ) count++;

    sprintf( buf, "你只要輸入 \e[1;35mhome goto %d\e[0m "
      "就可以到你的新家囉﹗\n\r", count );

    send_to_char( buf, ch );

    ch->bank = UMAX( 0, ch->bank - HomeGold );
    sprintf( buf, "%s花了一筆錢買了一棟豪宅﹐大家可以到他家去作客喔﹗"
      , mob_name( NULL, ch ) );

    talk_channel_2( buf, CHANNEL_BULLETIN, "公告" );
  }

  else if ( !str_prefix( arg, "goto" ) )
  {
    one_argument( argument, arg );

    if ( !arg[0] )
    {
      count = 1;
    }
    else if ( !is_number( arg ) || ( count = atoi( arg ) ) <= 0 )
    {
      send_to_char( "你要回到你哪棟房子呢﹖\n\r", ch );
      RETURN_NULL();
    }

    pHome = NULL;
    loop  = 0;

    for ( pRoom = PlayerZone->list; pRoom; pRoom = pRoom->next_in_area )
    {
      if ( is_owner( ch, pRoom, TRUE ) && ++loop == count )
      {
        pHome = pRoom;
        break;
      }
    }

    if ( loop == 0 )
    {
      send_to_char( "對不起﹐你根本沒有認購任何的房間﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !pHome )
    {
      send_to_char( "對不起﹐你沒有那棟房間喔﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( pHome == ch->in_room )
    {
      send_to_char( "對不起﹐你目前就是在這棟房子裡啊﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !can_leaveroom( ch, TRUE ) ) RETURN_NULL();

    pRoom = ch->in_room;

    /* 先送主人 */
    act( "$n匆匆忙忙地離開此地﹐回到他自己的房子去了﹗", ch, NULL, NULL, TO_ROOM );

    char_from_room( ch );
    char_to_room( ch, pHome );

    do_look( ch, "auto" );

    /* 清除追蹤紀錄點 */
    clear_trace( ch, TRUE );

    act( "$n急忙地回到了這個屬於他自己的家囉﹗", ch, NULL, NULL, TO_ROOM );

    /* 再送 follow 的人 */
    for ( fch = pRoom->people; fch; fch = fch_next )
    {
      fch_next = fch->next_in_room;

      if ( fch->master == ch
        && fch->position == POS_STANDING
        && can_leaveroom( fch, FALSE ) )
      {
        act( "你跟隨著$N回到他的家裡。", fch, NULL, ch, TO_CHAR );

        char_from_room( fch );
        char_to_room( fch, pHome );
        do_look( fch, "auto" );

        /* 清除追蹤紀錄點 */
        clear_trace( fch, TRUE );

        act( "$n跟隨著$N回到他的家裡。", fch, NULL, ch, TO_ROOM );
      }
    }
  }

  else if ( !str_prefix( arg, "title" ) )
  {
    if ( !IS_NPC( ch ) && ch->level < level_limit )
    {
      act( "在你升級到$i級前﹐請不要亂花錢﹗"
        , ch, &level_limit, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch->donate > 0 )
    {
      send_to_char( "在你領救濟金的這段時間﹐請不要亂花錢﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( LockCreateHome )
    {
      send_to_char( "對不起﹐目前管理者把這個功\能給鎖定住了﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( owner_name( ( pRoom = ch->in_room ) ) != ch )
    {
      send_to_char( "對不起﹐這不是你的私有財產﹐請不要在此塗鴉﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !pRoom->writeable )
    {
      act( "對不起﹐$r已經不能再塗改了喔﹗", ch, NULL, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( !argument[0] )
    {
      act( "對不起﹐你要把$r改名為什麼呢﹖", ch, NULL, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch->bank < HomeGoldTitle )
    {
      act( "你銀行裡必須有$i兩黃金才能更改房子名稱。"
        , ch, &HomeGoldTitle, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( MaxCNameLen < sizeof( argument ) && str_len( argument ) > MaxCNameLen )
      argument[ MaxCNameLen ] = '\x0';

    if ( str_len( argument ) < MinCNameLen )
    {
      send_to_char( "你的名稱也取的太短了吧﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( check_cname_xname( argument ) )
    {
      send_to_char( "這個名稱不好吧﹐換個名稱看看。\n\r", ch );
      RETURN_NULL();
    }

    ansi_transcribe( argument, buf );
    filter_ansi( buf );
    smash_tilde( buf );
    smash_point( buf );
    fix_color( buf );

    free_string( pRoom->name );
    pRoom->name = str_dup( buf );

    if ( !write_new_room( pRoom ) )
    {
      mudlog( LOG_DEBUG, "create_new_room: 無法存檔 %s.", pRoom->filename );
      send_to_char( "對不起﹐你的房間無法存檔﹗\n\r", ch );
      RETURN_NULL();
    }

    act( "$n把這裡改名為$t﹗", ch, buf, NULL, TO_ALL );
    ch->bank = UMAX( 0, ch->bank - HomeGoldTitle );
  }

  else if ( !str_prefix( arg, "edit" ) )
  {
    if ( !IS_NPC( ch ) && ch->level < level_limit )
    {
      act( "在你升級到$i級前﹐請不要亂花錢﹗"
        , ch, &level_limit, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch->donate > 0 )
    {
      send_to_char( "在你領救濟金的這段時間﹐請不要亂花錢﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( LockCreateHome )
    {
      send_to_char( "對不起﹐目前管理者把這個功\能給鎖定住了﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !ch->desc ) RETURN_NULL();

    if ( owner_name( ( pRoom = ch->in_room ) ) != ch )
    {
      send_to_char( "對不起﹐這不是你的私有財產﹐請不要在此塗鴉﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !pRoom->writeable )
    {
      act( "對不起﹐$r已經不能再更改設定了喔﹗", ch, NULL, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch->bank < HomeGoldDescr )
    {
      act( "$n銀行裡必須有$i兩黃金才能更改房子的敘述。"
        , ch, &HomeGoldDescr, NULL, TO_CHAR );
      RETURN_NULL();
    }

    ch->desc->edit_mode = EDIT_HOME_DESCR;
    show_editor( ch );

    free_string( ch->stack );
    ch->stack = str_dup( "" );

    ch->bank = UMAX( 0, ch->bank - HomeGoldDescr );
  }

  else if ( !str_prefix( arg, "unlink" ) )
  {
    if ( !IS_NPC( ch ) && ch->level < level_limit )
    {
      act( "在你升級到$i級前﹐請不要亂花錢﹗"
        , ch, &level_limit, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch->donate > 0 )
    {
      send_to_char( "在你領救濟金的這段時間﹐請不要亂花錢﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( LockCreateHome )
    {
      send_to_char( "對不起﹐目前管理者把這個功\能給鎖定住了﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->position != POS_STANDING )
    {
      send_to_char( "對不起﹐你先把你的雜事先弄完吧﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( owner_name( ( pHome = ch->in_room ) ) != ch )
    {
      send_to_char( "對不起﹐這不是你的私有財產﹐請不要在此塗鴉﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !pHome->writeable )
    {
      act( "對不起﹐$r已經不能再更改設定了喔﹗", ch, NULL, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch->bank < HomeGoldLink )
    {
      act( "$n銀行裡必須有$i兩黃金才能連結其他房間。"
        , ch, &HomeGoldLink, NULL, TO_CHAR );
      RETURN_NULL();
    }

    argument = one_argument( argument, arg );

         if ( !str_prefix( arg, "north" ) ) dir = DIR_NORTH;
    else if ( !str_prefix( arg, "east"  ) ) dir = DIR_EAST;
    else if ( !str_prefix( arg, "south" ) ) dir = DIR_SOUTH;
    else if ( !str_prefix( arg, "west"  ) ) dir = DIR_WEST;
    else if ( !str_prefix( arg, "up"    ) ) dir = DIR_UP;
    else if ( !str_prefix( arg, "down"  ) ) dir = DIR_DOWN;
    else if ( !str_prefix( arg, "enter" ) ) dir = DIR_ENTER;
    else if ( !str_prefix( arg, "out"   ) ) dir = DIR_OUT;
    else
    {
      send_to_char( "對不起﹐那不是一個正確的方向﹐請查詢 direction﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !( aExit = pHome->exit[dir] ) )
    {
      send_to_char( "對不起﹐根本沒有這個方向的通道﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !( pRoom = aExit->to_room ) )
    {
      send_to_char( "對不起﹐這條通道有點問題﹐似乎無法刪除﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !pRoom->writeable )
    {
      send_to_char( "對不起﹐對方房間已經不能再更改設定了喔﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !( bExit = pRoom->exit[rev_dir[dir]] ) )
    {
      send_to_char( "對不起﹐方向雙向連結有錯誤﹐無法刪除﹗\n\r", ch );
      RETURN_NULL();
    }

    pHome->exit[dir]          = NULL;
    pRoom->exit[rev_dir[dir]] = NULL;

    write_new_room( pRoom );
    write_new_room( pHome );

    act( "$n在這裡的$t方的通道給關閉了﹗"
      , ch, direction_name( dir ), NULL, TO_ALL );

    char_from_room( ch );
    char_to_room( ch, pRoom );

    act( "$n在隔壁堆了一塊大石頭﹐因此$t方的通道已經不通了﹗"
      , ch, direction_name( rev_dir[dir] ), NULL, TO_ROOM );

    char_from_room( ch );
    char_to_room( ch, pHome );

    ch->bank = UMAX( 0, ch->bank - HomeGoldLink );

    if ( aExit->keyword     ) free_string( aExit->keyword     );
    if ( aExit->description ) free_string( aExit->description );
    if ( bExit->message     ) free_string( bExit->message     );

    if ( bExit->keyword     ) free_string( bExit->keyword     );
    if ( bExit->description ) free_string( bExit->description );
    if ( bExit->message     ) free_string( bExit->message     );

    free_struct( aExit, STRUCT_EXIT_DATA );
    free_struct( bExit, STRUCT_EXIT_DATA );
  }

  else if ( !str_prefix( arg, "link" ) )
  {
    if ( !IS_NPC( ch ) && ch->level < level_limit )
    {
      act( "在你升級到$i級前﹐請不要亂花錢﹗"
        , ch, &level_limit, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch->donate > 0 )
    {
      send_to_char( "在你領救濟金的這段時間﹐請不要亂花錢﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( LockCreateHome )
    {
      send_to_char( "對不起﹐目前管理者把這個功\能給鎖定住了﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->position != POS_STANDING )
    {
      send_to_char( "對不起﹐你先把你的雜事先弄完吧﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( owner_name( ( pHome = ch->in_room ) ) != ch )
    {
      send_to_char( "對不起﹐這不是你的私有財產﹐請不要在此塗鴉﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !pHome->writeable )
    {
      act( "對不起﹐$r已經不能再塗改了喔﹗", ch, NULL, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch->bank < HomeGoldLink )
    {
      act( "$n銀行裡必須有$i兩黃金才能連結其他房間。"
        , ch, &HomeGoldLink, NULL, TO_CHAR );
      RETURN_NULL();
    }

    argument = one_argument( argument, arg );

    if ( !arg[0] || !is_number( arg ) || ( count = atoi( arg ) ) <= 0 )
    {
      send_to_char( "你要連結哪一間房間呢﹖請註明它的號碼吧﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !( pRoom = get_room_index( count ) ) )
    {
      send_to_char( "對不起﹐沒有這一間號碼的房間﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( pRoom == pHome )
    {
      send_to_char( "相同的房間是無法進行連結﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !pRoom->area || pRoom->area != PlayerZone )
    {
      send_to_char( "對不起﹐這間房間是固定形式的﹐不能連結﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( pRoom->NoLink )
    {
      send_to_char( "對不起﹐這間房間是不能連結﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !pRoom->writeable )
    {
      send_to_char( "對不起﹐那間房間不能再更改了﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !( fch = owner_name( pRoom ) ) )
    {
      send_to_char( "對不起﹐這間房間的主人不知道是誰耶﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( fch != ch && !is_same_group( fch, ch ) )
    {
      send_to_char( "對不起﹐連結的房間你沒有權力去動它﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !argument[0] )
    {
      send_to_char( "對不起﹐你要連結那個方向呢﹖\n\r", ch );
      RETURN_NULL();
    }

         if ( !str_prefix( argument, "north" ) ) dir = DIR_NORTH;
    else if ( !str_prefix( argument, "east"  ) ) dir = DIR_EAST;
    else if ( !str_prefix( argument, "south" ) ) dir = DIR_SOUTH;
    else if ( !str_prefix( argument, "west"  ) ) dir = DIR_WEST;
    else if ( !str_prefix( argument, "up"    ) ) dir = DIR_UP;
    else if ( !str_prefix( argument, "down"  ) ) dir = DIR_DOWN;
    else if ( !str_prefix( argument, "enter" ) ) dir = DIR_ENTER;
    else if ( !str_prefix( argument, "out"   ) ) dir = DIR_OUT;
    else
    {
      send_to_char( "對不起﹐那不是一個正確的方向﹐請查詢 direction﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( pHome->exit[dir] || pRoom->exit[rev_dir[dir]] )
    {
      send_to_char( "對不起﹐這方向已經有連結了﹗\n\r", ch );
      RETURN_NULL();
    }

    for ( loop = 0; loop < DIR_MAX; loop++ )
    {
      if ( pHome->exit[loop] && pHome->exit[loop]->to_room == pRoom )
      {
        send_to_char( "這間房間已有其他方向的連結了﹗\n\r", ch );
        RETURN_NULL();
      }
    }

    for ( loop = 0; loop < DIR_MAX; loop++ )
    {
      if ( pRoom->exit[loop] && pRoom->exit[loop]->to_room == pHome )
      {
        send_to_char( "這間房間已有其他方向的連結了﹗\n\r", ch );
        RETURN_NULL();
      }
    }

    /* 配置記憶體 */
    set_exit_default( aExit = alloc_struct( STRUCT_EXIT_DATA ) );
    set_exit_default( bExit = alloc_struct( STRUCT_EXIT_DATA ) );

    /* 清除基本值 */
    aExit->description = bExit->description = str_dup( "" );
    aExit->keyword     = bExit->keyword     = str_dup( "" );
    aExit->closed      = bExit->closed      = FALSE;
    aExit->isdoor      = bExit->isdoor      = FALSE;
    aExit->locked      = bExit->locked      = FALSE;
    aExit->pick        = bExit->pick        = FALSE;
    aExit->key         = bExit->key         = 0;

    /* 建立連結 */
    aExit->to_room = pRoom;
    bExit->to_room = pHome;

    aExit->vnum = pRoom->vnum;
    bExit->vnum = pHome->vnum;

    pHome->exit[dir]          = aExit;
    pRoom->exit[rev_dir[dir]] = bExit;

    write_new_room( pRoom );
    write_new_room( pHome );

    act( "$n在這裡的$t方打了個大洞﹗"
      , ch, direction_name( dir ), NULL, TO_ALL );

    char_from_room( ch );
    char_to_room( ch, pRoom );

    act( "$n在隔壁挖了個大洞﹐因此$t方可以通到那裡去了﹗"
      , ch, direction_name( rev_dir[dir] ), NULL, TO_ROOM );

    char_from_room( ch );
    char_to_room( ch, pHome );

    ch->bank = UMAX( 0, ch->bank - HomeGoldLink );
  }

  else
  {
    send_to_char( "你的語法錯誤﹐請查詢 home 的正確方法。\n\r", ch );
  }
  RETURN_NULL();
}

ROOM_INDEX_DATA * create_new_room( void )
{
  int               loop;
  int               vnum;
  int               iExit;
  int               iHash;
  int               nMatch;
  int               count;
  char              filename[MAX_STRING_LENGTH];
  char              dirname[MAX_STRING_LENGTH];
  ROOM_INDEX_DATA * pRoom;
  ROOM_INDEX_DATA * zStandard;

  PUSH_FUNCTION( "create_new_room" );

  /* 如果沒有玩家區域的目錄 */
  if ( !PlayerZone ) RETURN( NULL );

  /* 如果區域分配號碼及容量不合理 */
  if ( PlayerAreaNum <= 0 || PlayerAreaCap <= 0 ) RETURN( NULL );

  /* 先找一間樣本房間 */
  zStandard = NULL;
  for ( count = vnum = nMatch = 0; nMatch < top_room; vnum++ )
  {
    if ( ( pRoom = get_room_index( vnum ) ) )
    {
      nMatch++;

     if ( pRoom->Standard && number_range( 0, count++ ) == 0 )
        zStandard = pRoom;
    }
  }

  /* 找不到任何一間樣本房間 */
  if ( !zStandard ) RETURN( NULL );

  /* 目錄是否可以寫入 */
  sprintf( dirname, "%s/%s/", PlayerArea, room_dir );
  if ( access( dirname, W_OK ) ) RETURN( NULL );

  for ( loop = 0; loop < PlayerAreaCap; loop++ )
  {
    if ( get_room_index( ( vnum = loop + PlayerAreaNum ) ) ) continue;

    /* 設定房間檔案名稱 */
    sprintf( filename, "%s/%s/%d.%s"
      , PlayerArea, room_dir, vnum, room_ext );

    set_room_default( pRoom = alloc_struct( STRUCT_ROOM_INDEX_DATA ) );

    /* 設定房間基本 */
    pRoom->area            = PlayerZone;
    pRoom->filename        = str_dup( filename );
    pRoom->vnum            = vnum;
    pRoom->light           = 0;
    pRoom->sector          = DefaultSector;

    /* 依照標準房間設定數值 */
    pRoom->name            = str_dup( zStandard->name );
    pRoom->description     = str_dup( zStandard->description );

    pRoom->Dark            = zStandard->Dark;
    pRoom->NoMob           = zStandard->NoMob;
    pRoom->Indoors         = zStandard->Indoors;
    pRoom->Private         = zStandard->Private;
    pRoom->Safe            = zStandard->Safe;
    pRoom->ForeverLight    = zStandard->ForeverLight;
    pRoom->NoRecall        = zStandard->NoRecall;
    pRoom->DepositMoney    = zStandard->DepositMoney;
    pRoom->StoreRoom       = zStandard->StoreRoom;
    pRoom->NoFight         = zStandard->NoFight;
    pRoom->NoQuit          = zStandard->NoQuit;
    pRoom->Club            = zStandard->Club;
    pRoom->Memorize        = zStandard->Memorize;
    pRoom->Stock           = zStandard->Stock;
    pRoom->NoWhere         = zStandard->NoWhere;
    pRoom->Sail            = zStandard->Sail;
    pRoom->Fane            = zStandard->Fane;
    pRoom->Killer          = zStandard->Killer;
    pRoom->Standard        = FALSE;

    /* 放到區域列表 */
    pRoom->next_in_area    = PlayerZone->list;
    PlayerZone->list       = pRoom;
    PlayerZone->room++;

    iHash                  = pRoom->vnum % MAX_KEY_HASH;
    pRoom->next            = room_index_hash[iHash];
    room_index_hash[iHash] = pRoom;
    top_room++;

    /* 設定沒有出口 */
    for ( iExit = 0; iExit < DIR_MAX; iExit++ ) pRoom->exit[iExit] = NULL;

    RETURN( pRoom );
  }

  mudlog( LOG_DEBUG, "create_new_room: 玩家區域檔案已滿." );
  RETURN( NULL );
}

bool write_new_room( ROOM_INDEX_DATA * pRoom )
{
  ENQUIRE_DATA     * pEnquire;
  EXTRA_DESCR_DATA * pExtra;
  FILE             * pFile;
  EXIT_DATA        * pExit;
  char               buf[MAX_STRING_LENGTH];
  char               Str1[MAX_INPUT_LENGTH];
  int                iExit;

  PUSH_FUNCTION( "write_new_room" );

  if ( !pRoom )
  {
    mudlog( LOG_DEBUG, "write_new_room: 來源不存在." );
    RETURN( FALSE );
  }

  if ( !pRoom->sector ) RETURN( FALSE );

  if ( !( pFile = FOPEN( pRoom->filename, "w" ) ) )
  {
    mudlog( LOG_DEBUG, "write_new_room: 無法開啟檔案 %s.", pRoom->filename );
    RETURN( FALSE );
  }

  fprint_number( pFile, "Vnum" , pRoom->vnum );
  fprint_string( pFile, "Name",  pRoom->name );

  buf[0] = '\x0';
  clear_return( pRoom->description, buf );
  fprintf( pFile, "Description\n%s~\n", buf );

  switch( pRoom->sector->vnum )
  {
  default: fprint_number( pFile, "SectorType", pRoom->sector->vnum ); break;
  case 0: fprintf( pFile, "SectorType      SECT_INSIDE\n"          ); break;
  case 1: fprintf( pFile, "SectorType      SECT_CITY\n"            ); break;
  case 2: fprintf( pFile, "SectorType      SECT_FIELD\n"           ); break;
  case 3: fprintf( pFile, "SectorType      SECT_FOREST\n"          ); break;
  case 4: fprintf( pFile, "SectorType      SECT_HILLS\n"           ); break;
  case 5: fprintf( pFile, "SectorType      SECT_MOUNTAIN\n"        ); break;
  case 6: fprintf( pFile, "SectorType      SECT_WATER_SWIM\n"      ); break;
  case 7: fprintf( pFile, "SectorType      SECT_WATER_NOSWIM\n"    ); break;
  case 8: fprintf( pFile, "SectorType      SECT_AIR\n"             ); break;
  case 9: fprintf( pFile, "SectorType      SECT_DESERT\n"          ); break;
  }

  str_cpy( Str1, symbol_string( TRUE, "邏輯常數" ) );

  if ( pRoom->player       ) fprintf( pFile, "Player          %s\n", Str1 );
  if ( pRoom->Dark         ) fprintf( pFile, "Dark            %s\n", Str1 );
  if ( pRoom->NoMob        ) fprintf( pFile, "NoMob           %s\n", Str1 );
  if ( pRoom->Indoors      ) fprintf( pFile, "InDoors         %s\n", Str1 );
  if ( pRoom->Private      ) fprintf( pFile, "Private         %s\n", Str1 );
  if ( pRoom->Safe         ) fprintf( pFile, "Safe            %s\n", Str1 );
  if ( pRoom->ForeverLight ) fprintf( pFile, "ForeverLight    %s\n", Str1 );
  if ( pRoom->NoRecall     ) fprintf( pFile, "NoRecall        %s\n", Str1 );
  if ( pRoom->DepositMoney ) fprintf( pFile, "DepositMoney    %s\n", Str1 );
  if ( pRoom->StoreRoom    ) fprintf( pFile, "StoreRoom       %s\n", Str1 );
  if ( pRoom->NoFight      ) fprintf( pFile, "NoFight         %s\n", Str1 );
  if ( pRoom->NoQuit       ) fprintf( pFile, "NoQuit          %s\n", Str1 );
  if ( pRoom->Club         ) fprintf( pFile, "Club            %s\n", Str1 );
  if ( pRoom->Memorize     ) fprintf( pFile, "Memorize        %s\n", Str1 );
  if ( pRoom->Stock        ) fprintf( pFile, "Stock           %s\n", Str1 );
  if ( pRoom->NoWhere      ) fprintf( pFile, "NoWhere         %s\n", Str1 );
  if ( pRoom->Sail         ) fprintf( pFile, "Sail            %s\n", Str1 );
  if ( pRoom->Fane         ) fprintf( pFile, "Fane            %s\n", Str1 );
  if ( pRoom->Killer       ) fprintf( pFile, "Killer          %s\n", Str1 );
  if ( pRoom->Standard     ) fprintf( pFile, "Standard        %s\n", Str1 );
  if ( pRoom->writeable    ) fprintf( pFile, "Writeable       %s\n", Str1 );
  if ( pRoom->Waterful     ) fprintf( pFile, "Waterful        %s\n", Str1 );
  if ( pRoom->Foodful      ) fprintf( pFile, "Foodful         %s\n", Str1 );
  if ( pRoom->ManaCure     ) fprintf( pFile, "ManaCure        %s\n", Str1 );
  if ( pRoom->MoveCure     ) fprintf( pFile, "MoveCure        %s\n", Str1 );
  if ( pRoom->HitCure      ) fprintf( pFile, "HitCure         %s\n", Str1 );
  if ( pRoom->PracRoom     ) fprintf( pFile, "PracRoom        %s\n", Str1 );
  if ( pRoom->Upgrade      ) fprintf( pFile, "Upgrade         %s\n", Str1 );

  /* 寫入擁有者 */
  if ( pRoom->owner.high > 0 && pRoom->owner.low > 0 )
  {
    fprintf( pFile, "Owner           %d %d\n"
      , pRoom->owner.high, pRoom->owner.low );
  }

  if ( pRoom->cost > 0 ) fprint_number( pFile, "Cost", pRoom->cost );
  if ( pRoom->gold > 0 ) fprint_number( pFile, "Gold", pRoom->gold );

  for ( iExit = 0; iExit < DIR_MAX; iExit++ )
  {
    if ( !( pExit = pRoom->exit[iExit] ) ) continue;
    fprintf( pFile, "\n#Exit\n" );

    switch( iExit )
    {
    case DIR_NORTH: fprintf( pFile, "Direction       North\n" ); break;
    case DIR_SOUTH: fprintf( pFile, "Direction       South\n" ); break;
    case DIR_EAST : fprintf( pFile, "Direction       East\n"  ); break;
    case DIR_WEST : fprintf( pFile, "Direction       West\n"  ); break;
    case DIR_UP   : fprintf( pFile, "Direction       Up\n"    ); break;
    case DIR_DOWN : fprintf( pFile, "Direction       Down\n"  ); break;
    case DIR_ENTER: fprintf( pFile, "Direction       Enter\n" ); break;
    case DIR_OUT  : fprintf( pFile, "Direction       Out\n"   ); break;
    }

    fprint_number( pFile, "ExitVnum"    , pExit->to_room->vnum );
    fprint_string( pFile, "ExitDesc"    , pExit->description   );
    fprint_string( pFile, "ExitKeyword" , pExit->keyword       );
    fprint_number( pFile, "ExitKey"     , pExit->key           );

    if ( pExit->isdoor ) fprintf( pFile, "IsDoor          %s\n", Str1 );
    if ( pExit->closed ) fprintf( pFile, "Closed          %s\n", Str1 );
    if ( pExit->locked ) fprintf( pFile, "Locked          %s\n", Str1 );
    if ( pExit->pick   ) fprintf( pFile, "Pick            %s\n", Str1 );

    fprintf( pFile, "End\n" );
  }

  for ( pExtra = pRoom->extra_descr; pExtra; pExtra = pExtra->next )
  {
    fprintf( pFile, "\n#Keyword\n" );
    fprint_string( pFile, "Keyword", pExtra->keyword );

    buf[0] = '\x0';
    clear_return( pExtra->description, buf );
    fprintf( pFile, "Description\n%s~\n", buf );

    fprintf( pFile, "End\n" );
  }

  for ( pEnquire = pRoom->enquire; pEnquire; pEnquire = pEnquire->next )
  {
    fprintf( pFile, "\n#Enquire\n" );
    fprint_string( pFile, "KeyWord", pEnquire->keyword );
    fprintf( pFile, "End\n" );
  }

  fprintf( pFile, "\nEnd" );

  FCLOSE( pFile );
  RETURN( TRUE );
}

bool is_owner( CHAR_DATA * ch, ROOM_INDEX_DATA * pRoom, bool couple )
{
  PUSH_FUNCTION( "is_owner" );

  if ( !ch || !pRoom )
  {
    mudlog( LOG_DEBUG, "is_owner: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( IS_NPC( ch ) ) RETURN( FALSE );

  if ( ch->serial.high == pRoom->owner.high
    && ch->serial.low == pRoom->owner.low ) RETURN( TRUE );

  if ( couple
    && ch->pcdata
    && ch->pcdata->couple.high > 0
    && ch->pcdata->couple.low > 0
    && ch->pcdata->couple.high == pRoom->owner.high
    && ch->pcdata->couple.low  == pRoom->owner.low ) RETURN( TRUE )

  RETURN( FALSE );
}

CHAR_DATA * owner_name( ROOM_INDEX_DATA * pRoom )
{
  CHAR_DATA       * ch;
  DESCRIPTOR_DATA * man;

  PUSH_FUNCTION( "owner_name" );

  if ( !pRoom )
  {
    mudlog( LOG_DEBUG, "owner_name: 缺乏來源." );
    RETURN( NULL );
  }

  for ( man = descriptor_list; man; man = man->next )
  {
    if ( verify_desc( man )
      && man->connected == CON_PLAYING
      && verify_char( ( ch = man->character ) )
      && ch->in_room
      && ch->serial.high == pRoom->owner.high
      && ch->serial.low  == pRoom->owner.low )
      RETURN( ch );
  }

  RETURN( NULL );
}

void room_update( void )
{
  AREA_DATA       * pArea;
  CHAR_DATA       * pChar;
  CHAR_DATA       * zChar;
  ROOM_INDEX_DATA * pRoom;
  OBJ_DATA        * pObj;
  OBJ_DATA        * zObj;
  int               count;
  bool              bFound;
  char              buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "room_update" );

  for ( pArea = area_first; pArea; pArea = pArea->next )
  {
    for ( pRoom = pArea->list; pRoom; pRoom = pRoom->next_in_area )
    {
      if ( pRoom->mob_reborn > 0 ) pRoom->mob_reborn--;

      if ( pRoom->nplayer > MaxRoomPlayer )
      {

      }

      if ( pRoom->nobject > MaxRoomObject )
      {
        zObj  = NULL;
        count = 1;

        for ( pObj = pRoom->contents; pObj; pObj = pObj->next_content )
        {
          if ( number_range( 1, ++count ) == 1
            && pObj->item_type != ITEM_CORPSE_PC
            && pObj->Takeable == TRUE )
            zObj = pObj;
        }

        if ( zObj )
        {
          sprintf( buf, "某不知名人物把%s給奉獻掉了﹗\n\r"
            , obj_name( NULL, zObj ) );

          sendmsg_to_someroom( buf, pRoom );
          extract_obj( zObj );
        }
      }

      if ( !pRoom->people ) continue;

      if ( pRoom->ManaCure )
      {
        zChar = NULL;
        count = 0;

        for ( pChar = pRoom->people; pChar; pChar = pChar->next_in_room )
        {
          if ( !IS_NPC( pChar )
            && pChar->mana < get_curr_mana( pChar )
            && number_range( 1, ++count ) == 1 )
            zChar = pChar;
        }

        if ( zChar )
        {
          zChar->mana += get_curr_mana( zChar ) / 3;
          zChar->mana  = UMIN( zChar->mana, get_curr_mana( zChar ) );
          send_to_char( "\n\r\e[1;32m讓我來幫你回復部分法力吧﹗\e[0m\n\r", zChar );
          continue;
        }
      }

      if ( pRoom->MoveCure )
      {
        zChar = NULL;
        count = 0;

        for ( pChar = pRoom->people; pChar; pChar = pChar->next_in_room )
        {
          if ( !IS_NPC( pChar )
            && pChar->move < get_curr_move( pChar )
            && number_range( 1, ++count ) == 1 )
            zChar = pChar;
        }

        if ( zChar )
        {
          zChar->move += get_curr_move( zChar ) / 3;
          zChar->move  = UMIN( zChar->move, get_curr_move( zChar ) );
          send_to_char( "\n\r\e[1;32m讓我來幫你回復部分體力吧﹗\e[0m\n\r", zChar );
          continue;
        }
      }

      if ( pRoom->HitCure )
      {
        zChar = NULL;
        count = 0;

        for ( pChar = pRoom->people; pChar; pChar = pChar->next_in_room )
        {
          if ( !IS_NPC( pChar )
            && pChar->hit < get_curr_hit( pChar )
            && number_range( 1, ++count ) == 1 )
            zChar = pChar;
        }

        if ( zChar )
        {
          zChar->hit += get_curr_hit( zChar ) / 3;
          zChar->hit  = UMIN( zChar->hit, get_curr_hit( zChar ) );
          send_to_char( "\n\r\e[1;32m讓我來幫你回復部分生命力吧﹗\e[0m\n\r", zChar );
          continue;
        }
      }

      if ( pRoom->Waterful )
      {
        bFound = FALSE;
        for ( pObj = pRoom->contents; pObj; pObj = pObj->next_content )
        {
          if ( pObj->pIndexData == ObjSpring )
          {
            bFound = TRUE;
            break;
          }
        }

        if ( !bFound && pRoom->nobject < MaxRoomObject )
        {
          pObj  = create_object( ObjSpring , 1 );
          pObj->timer = 10;
          obj_to_room( pObj, pRoom );

          sprintf( buf, "各位大爺﹐免費贈送的%s給你們送來了﹗\n\r"
            , obj_name( NULL, pObj ) );

          sendmsg_to_room( buf, pRoom->people );
          continue;
        }
      }

      if ( pRoom->Foodful )
      {
        bFound = FALSE;
        for ( pObj = pRoom->contents; pObj; pObj = pObj->next_content )
        {
          if ( pObj->pIndexData == ObjDumpling )
          {
            bFound = TRUE;
            break;
          }
        }

        if ( !bFound && pRoom->nobject < MaxRoomObject )
        {
          pObj  = create_object( ObjDumpling , 1 );
          pObj->timer = 10;
          obj_to_room( pObj, pRoom );

          sprintf( buf, "各位大爺﹐免費贈送的%s給你們送來了﹗\n\r"
            , obj_name( NULL, pObj ) );

          sendmsg_to_room( buf, pRoom->people );
          continue;
        }
      }
    }
  }
  RETURN_NULL();
}

int count_home_number( CHAR_DATA * ch )
{
  ROOM_INDEX_DATA * pRoom;
  int               count;

  PUSH_FUNCTION( "count_home_number" );

  if ( IS_NPC( ch ) || !ch->in_room || !PlayerZone ) RETURN( 0 );

  count = 0;
  for ( pRoom = PlayerZone->list; pRoom; pRoom = pRoom->next_in_area )
    if ( is_owner( ch, pRoom, FALSE ) ) count++;

  RETURN( count );
}

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
#include <sys/times.h>
#include <time.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "merc.h"

/* 區域函數的宣告 */
size_t        count_club_member args( ( CLUB_DATA *, int ) );
int             name_in_club    args( ( char *, CLUB_DATA * ) );
bool            clubmember_full args( ( CLUB_DATA *, int ) );
bool           countersign_club args( ( CHAR_DATA *, CLUB_DATA * ) );
bool            right_ring      args( ( char *, CLUB_DATA *, int ) );
bool            appoint_member  args( ( char *, CLUB_DATA *, int ) );
bool            fordo_club      args( ( CLUB_DATA * ) );
bool            is_note_to_club args( ( CLUB_DATA *, NOTE_DATA * ) );
bool            set_club        args( ( CLUB_DATA *, int, char *, int ) );
CLUB_DATA *     is_inclub       args( ( char * ) );
CLUB_DATA *     char_club       args( ( CHAR_DATA * ) );
CLUB_DATA *     create_club     args( ( CHAR_DATA *, char *, char * ) );

extern  void    set_club_default        args( ( CLUB_DATA * ) );
extern  int     name_in_club            args( ( char *, CLUB_DATA * ) );

int             club_room               = 0;
int             max_club                = 0;

/* 設定幫派資料的指令 */
FUNCTION( do_clubset )
{
  CLUB_DATA * pClub;
  CHAR_DATA * victim;
  char      * pString;
  char        buf[MAX_STRING_LENGTH];
  char        arg1[MAX_INPUT_LENGTH];
  char        arg2[MAX_INPUT_LENGTH];
  char        arg3[MAX_INPUT_LENGTH];
  int         option;
  int         loop;

  PUSH_FUNCTION( "do_clubset" );

  /* 擷取參數 */
  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );
  argument = one_argument( argument, arg3 );

  /* 缺乏參數 */
  if ( !arg1[0] || !arg2[0] )
  {
    send_to_char( "你要更改哪一個幫派的什麼資料﹖以下是選項﹕\n\r"
                  " 1) name       更改英文幫派名稱\n\r"
                  " 2) cname      更改幫派中文名稱\n\r"
                  " 3) money      更改幫派的金錢\n\r"
                  " 4) master     更改幫派的幫主\n\r"
                  " 5) vicemaster 更改幫派的副幫主\n\r"
                  " 6) doyen      增加幫派的長老\n\r"
                  " 7) member     增加幫派的幫眾\n\r"
                  " 8) strip      移除幫派人物\n\r"
                  " 9) verify     認證幫派人物\n\r"
                  "10) status     更改幫派狀態\n\r"
                  "11) fordo      解散幫派\n\r"
      , ch );
    RETURN_NULL();
  }

  /* 查詢是否有這個幫派 */
  if ( !( pClub = clubname_lookup( arg1, CLUB_NAME ) ) )
  {
    act( "找不到你要更改的幫派名稱$t。", ch, arg1, NULL, TO_CHAR );
    RETURN_NULL();
  }

  /* 更改英文名稱 */
  if ( !str_prefix( arg2, "name" ) )
  {
    /* 檢查參數 */
    if ( !arg3[0] )
    {
      send_to_char( "請問你要把幫派英文名稱改成什麼呢﹖\n\r", ch );
      RETURN_NULL();
    }

    /* 是否相同 */
    if ( pClub->name && !str_cmp( pClub->name, arg3 ) )
    {
      send_to_char( "對不起﹐和原本的名稱一樣﹐動作取消。\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查是否之前就已經存在了 */
    if ( clubname_lookup( arg3, CLUB_NAME ) )
    {
      send_to_char( "對不起﹐已經有這樣的幫派英文名稱。\n\r", ch );
      RETURN_NULL();
    }

    /* 避免有空白字 */
    for ( pString = arg3; *pString; pString++ )
    {
      if ( !isalpha( ( int ) *pString ) )
      {
        send_to_char( "這個英文名稱不合法。\n\r", ch );
        RETURN_NULL();
      }
    }

    sprintf( buf, "系統更改幫派 %s 英文名稱為 %s﹗\n\r", pClub->name, arg3 );

    /* 設定幫派資料 */
    if ( !set_club( pClub, CLUB_NAME, arg3, 0 ) )
    {
      send_to_char( "對不起﹐設定幫派英文名稱失敗。\n\r", ch );
      RETURN_NULL();
    }

    send_to_all_char( buf );
  }

  /* 更改幫派中文名稱 */
  else if ( !str_prefix( arg2, "cname" ) )
  {
    /* 檢查參數 */
    if ( !arg3[0] )
    {
      send_to_char( "請問你要把幫派的中文名稱改成什麼呢﹖\n\r", ch );
      RETURN_NULL();
    }

    /* 是否相同 */
    if ( pClub->cname && !str_cmp( pClub->cname, arg3 ) )
    {
      send_to_char( "對不起﹐和原本的名稱一樣﹐動作取消。\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查是否之前就已經存在了 */
    if ( clubname_lookup( arg3, CLUB_CNAME ) )
    {
      send_to_char( "對不起﹐已經有這樣的幫派中文名稱。\n\r", ch );
      RETURN_NULL();
    }

    for ( pString = arg3; *pString; pString++ )
    {
      if ( *pString == '%' )
      {
        send_to_char( "對不起﹐這個中文名稱不合法。\n\r", ch );
        RETURN_NULL();
      }
    }

    sprintf( buf, "系統更改幫派 %s\e[0m 中文名稱為 %s﹗\n\r"
      , pClub->cname, arg3 );

    /* 設定幫派資料 */
    if ( !set_club( pClub, CLUB_CNAME, arg3, 0 ) )
    {
      send_to_char( "對不起﹐設定幫派中文名稱失敗。\n\r", ch );
      RETURN_NULL();
    }
    send_to_all_char( buf );
  }

  /* 更改幫派狀態 */
  else if ( !str_prefix( arg2, "status" ) )
  {
    /* 檢查參數 */
    if ( !arg3[0] )
    {
      send_to_char( "更改幫派狀態選項可以是﹕1. 連署 2. 成立\n\r", ch );
      RETURN_NULL();
    }

    sprintf( buf, "系統更改幫派 %s\e[0m(%s)狀態為"
      , pClub->cname, pClub->name );

    switch( ( option = atoi( arg3 ) ) )
    {
    /* 未定義 */
    default:
      send_to_char( "不正確的狀態參數﹐必須為 1) 連署 2) 成立\n\r", ch );
      RETURN_NULL();

    /* 連署狀態 */
    case CLUB_STATUS_COUNTERSIGN:

      str_cat( buf, "連署。\n\r" );
      break;

    /* 已成立狀態 */
    case CLUB_STATUS_UNIONIZE:

      str_cat( buf, "已成立。\n\r" );
      break;
    }

    /* 設定幫派資料 */
    if ( !set_club( pClub, CLUB_STATUS, NULL, option ) )
    {
      send_to_char( "對不起﹐設定幫派狀態失敗。\n\r", ch );
      RETURN_NULL();
    }

    send_to_all_char( buf );
  }

  /* 更改幫庫的金錢 */
  else if ( !str_prefix( arg2, "money" ) )
  {
    if ( !arg3[0] || ( option = atoi( arg3 ) ) < 0 )
    {
      send_to_char( "對不起﹐更改幫派金錢參數錯誤。\n\r", ch );
      RETURN_NULL();
    }

    /* 設定幫派資料 */
    if ( !set_club( pClub, CLUB_MONEY, NULL, option ) )
    {
      send_to_char( "設定幫派金錢失敗。\n\r", ch );
      RETURN_NULL();
    }

    act( "你更改$y的幫派金錢為$I。", ch, pClub, &option, TO_CHAR );
  }

  /* 更改幫派的幫主 */
  else if ( !str_prefix( arg2, "master" ) )
  {
    /* 檢查參數 */
    if ( !arg3[0] )
    {
      send_to_char( "請問你要把幫主改成誰﹖\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查是否之前有參加過幫派 */
    else if ( is_inclub( arg3 ) )
    {
      send_to_char( "對不起﹐此人之前有參加幫派。\n\r", ch );
      RETURN_NULL();
    }

    else
    {
      /* 若幫主存在, 先把幫主移除 */
      if ( clubmember_full( pClub, CLUB_MASTER ) )
      {
        if ( !char_from_club( pClub->master, pClub, CLUB_MASTER ) )
        {
          send_to_char( "系統移除幫主錯誤。\n\r", ch );
          RETURN_NULL();
        }

        /* 如果此人上線送資訊給他 */
        if ( ( victim = is_online( pClub->master ) ) )
          act( "抱歉﹐你被系統移除$T($t)幫幫主職務。"
            , victim, pClub->cname, pClub->name, TO_CHAR );
      }

      sprintf( buf, "系統把 %s\e[0m(%s) 幫主 %s 改成 %s。\n\r"
        , pClub->cname
        , pClub->name
        , pClub->master ? pClub->master : "", arg3 );

      /* 設定幫主職位 */
      if ( !char_to_club( arg3, pClub, CLUB_MASTER ) )
      {
        send_to_char( "系統設定幫主錯誤。\n\r", ch );
        RETURN_NULL();
      }

      /* 如果此人上線送資訊給他 */
      if ( ( victim = is_online( arg3 ) ) )
        act( "系統請你擔任$T($t)的幫主。"
          , victim, pClub->cname, pClub->name, TO_CHAR );

      send_to_all_char( buf );
    }
  }

  /* 設定幫派副幫主 */
  else if ( !str_prefix( arg2, "vicemaster" ) )
  {
    /* 檢查參數 */
    if ( !arg3[0] )
    {
      send_to_char( "請問你要把副幫主改成誰﹖\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查是否之前有參加過幫派 */
    else if ( is_inclub( arg3 ) )
    {
      send_to_char( "對不起﹐此人之前有參加幫派。\n\r", ch );
      RETURN_NULL();
    }

    else
    {
      /* 若副幫主存在, 先把副幫主移除 */
      if ( clubmember_full( pClub, CLUB_VICEMASTER ) )
      {
        if ( !char_from_club( pClub->vicemaster, pClub, CLUB_VICEMASTER ) )
        {
          send_to_char( "系統移除副幫主錯誤。\n\r", ch );
          RETURN_NULL();
        }

        /* 如果此人上線送資訊給他 */
        if ( ( victim = is_online( pClub->master ) ) )
          act( "抱歉﹐你被系統移除 $y 幫副幫主職務。"
            , victim, pClub, NULL, TO_CHAR );
      }

      sprintf( buf, "系統把 %s\e[0m(%s)副幫主 %s 改成 %s。\n\r"
        , pClub->cname
        , pClub->name
        , pClub->vicemaster ? pClub->vicemaster : "", arg3 );

      /* 設定副幫主職位 */
      if ( !char_to_club( arg3, pClub, CLUB_VICEMASTER ) )
      {
        send_to_char( "系統設定副幫主錯誤。\n\r", ch );
        RETURN_NULL();
      }

      /* 如果此人上線送資訊給他 */
      if ( ( victim = is_online( arg3 ) ) )
        act( "系統請你擔任 $y 的副幫主。", victim, pClub, NULL, TO_CHAR );

      send_to_all_char( buf );
    }
  }

  /* 設定幫派長老 */
  else if ( !str_prefix( arg2, "doyen" ) )
  {
    /* 檢查參數 */
    if ( !arg3[0] )
    {
      send_to_char( "請問你要增加哪一位長老﹖\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查是否之前有參加過幫派 */
    else if ( is_inclub( arg3 ) )
    {
      send_to_char( "對不起﹐此人之前有參加幫派。\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查此幫派的長老名額是否已滿 */
    else if ( clubmember_full( pClub, CLUB_DOYEN ) )
    {
      send_to_char( "對不起﹐此幫派的長老名額已滿。\n\r", ch );
      RETURN_NULL();
    }

    else
    {
      /* 設定長老職位 */
      if ( !char_to_club( arg3, pClub, CLUB_DOYEN ) )
      {
        send_to_char( "系統執行設定長老失敗。\n\r", ch );
        RETURN_NULL();
      }

      /* 如果此人上線送資訊給他 */
      if ( ( victim = is_online( arg3 ) ) )
        act( "系統請你擔任 $y 的長老。", victim, pClub, NULL, TO_CHAR );

      send_to_char( "設定幫派長老完成。\n\r", ch );
    }
  }

  /* 設定幫派幫眾 */
  else if ( !str_prefix( arg2, "member" ) )
  {
    /* 檢查參數 */
    if ( !arg3[0] )
    {
      send_to_char( "請問你要增加哪一位幫眾﹖\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查是否之前有參加過幫派 */
    else if ( is_inclub( arg3 ) )
    {
      send_to_char( "對不起﹐此人之前有參加幫派。\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查此幫派的幫眾名額是否已滿 */
    else if ( clubmember_full( pClub, CLUB_MEMBER ) )
    {
      send_to_char( "抱歉﹐此幫派的幫眾名額已滿。\n\r", ch );
      RETURN_NULL();
    }

    else
    {
      /* 設定幫派幫眾職位 */
      if ( !char_to_club( arg3, pClub, CLUB_MEMBER ) )
      {
        send_to_char( "系統執行設定幫眾失敗。\n\r", ch );
        RETURN_NULL();
      }

      /* 如果此人上線送資訊給他 */
      if ( ( victim = is_online( arg3 ) ) )
        act( "系統請你參加 $y 的幫眾。", victim, pClub, NULL, TO_CHAR );

      send_to_char( "設定幫派幫眾完成。\n\r", ch );
    }
  }

  /* 設定幫派幫眾 */
  else if ( !str_prefix( arg2, "follower" ) )
  {
    /* 檢查參數 */
    if ( !arg3[0] )
    {
      send_to_char( "請問你要增加哪一位弟子﹖\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查是否之前有參加過幫派 */
    else if ( is_inclub( arg3 ) )
    {
      send_to_char( "對不起﹐此人之前有參加幫派。\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查此幫派的幫眾名額是否已滿 */
    else if ( clubmember_full( pClub, CLUB_FOLLOWER ) )
    {
      send_to_char( "抱歉﹐此幫派的弟子名額已滿。\n\r", ch );
      RETURN_NULL();
    }

    else
    {
      /* 設定幫派幫眾職位 */
      if ( !char_to_club( arg3, pClub, CLUB_FOLLOWER ) )
      {
        send_to_char( "系統執行設定弟子失敗。\n\r", ch );
        RETURN_NULL();
      }

      /* 如果此人上線送資訊給他 */
      if ( ( victim = is_online( arg3 ) ) )
        act( "系統請你參加 $y 的弟子。", victim, pClub, NULL, TO_CHAR );

      send_to_char( "設定幫派弟子完成。\n\r", ch );
    }
  }

  /* 移除某人幫派的職位, 除了幫主以外 */
  else if ( !str_prefix( arg2, "strip" ) )
  {
    /* 檢查參數 */
    if ( !arg3[0] )
    {
      send_to_char( "請問你要移除哪一位幫派人物﹖\n\r", ch );
      RETURN_NULL();
    }

    /* 傳回原本的職位 */
    if ( ( option = name_in_club( arg3, pClub ) ) == CLUB_NO_RELATION )
    {
      send_to_char( "對不起﹐這個人不在這個幫派之中。\n\r", ch );
      RETURN_NULL();
    }

    /* 幫主不能移除 */
    if ( option == CLUB_MASTER )
    {
      send_to_char( "對不起﹐幫主是不能移除﹐只能轉移﹗\n\r", ch );
      RETURN_NULL();
    }

    /* 移除他的職位 */
    if ( !char_from_club( arg3, pClub, option ) )
    {
      send_to_char( "系統執行移除職位失敗。\n\r", ch );
      RETURN_NULL();
    }

    /* 如果此人上線送資訊給他 */
    if ( ( victim = is_online( arg3 ) ) )
      send_to_char( "抱歉﹐你被系統趕出幫派了。\n\r", victim );

    act( "你把 $T 趕出 $y 幫之外了。", ch, pClub, arg3, TO_CHAR );
  }

  /* 認證幫眾 */
  else if ( !str_prefix( arg2, "verify" ) )
  {
    /* 檢查參數 */
    if ( !arg3[0] )
    {
      send_to_char( "請問你要認證那位幫眾﹖\n\r", ch );
      RETURN_NULL();
    }

    /* 原本就不在此幫派之中 */
    if ( ( option = name_in_club( arg3, pClub ) ) == CLUB_NO_RELATION )
    {
      send_to_char( "對不起﹐這個人還沒有加入這個幫派。\n\r", ch );
      RETURN_NULL();
    }

    /* 只有幫眾需要認證 */
    if ( option != CLUB_FOLLOWER )
    {
      send_to_char( "抱歉﹐你只能替弟子認證﹐其他的不需要認證。\n\r", ch );
      RETURN_NULL();
    }

    for ( loop = 0; loop < MAX_CLUB_MEMBER; loop++ )
    {
      if ( pClub->member[loop] && !str_cmp( arg3, pClub->member[loop] ) )
      {
        if ( pClub->verify[loop] )
        {
          send_to_char( "抱歉﹐此人之前就被認證了。\n\r", ch );
          RETURN_NULL();
        }

        pClub->verify[loop] = TRUE;

        /* 如果此人上線送資訊給他 */
        if ( ( victim = is_online( pClub->member[loop] ) ) )
        {
          victim->club = pClub;
          send_to_char( "恭禧你﹐通過系統的幫派認證。\n\r", victim );
        }
        break;
      }
    }
  }

  /* 解散幫派 */
  else if ( !str_prefix( arg2, "fordo" ) )
  {
    sprintf( buf, "系統解散 %s\e[0m(%s)幫派﹐從此%s走進歷史﹗\n\r"
      , pClub->cname, pClub->name, pClub->cname );

    /* 解散幫派的函數 */
    if ( fordo_club( pClub ) )
    {
      send_to_char( "你解散了這個幫派。\n\r", ch );
      send_to_all_char( buf );
    }

    else
    {
      send_to_char( "系統解散幫派失敗。\n\r", ch );
    }
  }

  /* 副指令錯誤 */
  else
  {
    send_to_char( "選項錯誤﹐請查詢 clubset 指令正確的用法。\n\r", ch );
  }

  RETURN_NULL();
}

/* 幫派指令 */
FUNCTION( do_club )
{
  CLUB_DATA  * pClub;
  CHAR_DATA  * victim;
  NOTE_DATA  * pNote;
  BASIC_DATA * pBasic;
  CLASS_DATA * pClass;
  char       * pString;
  char         arg1[MAX_INPUT_LENGTH];
  char         arg2[MAX_INPUT_LENGTH];
  char         arg3[MAX_INPUT_LENGTH];
  char         buf[MAX_STRING_LENGTH];
  char         pellet[MAX_STRING_LENGTH];
  int          count;
  int          loop;
  int          level;

  PUSH_FUNCTION( "do_club" );

  /* 非玩家不可以有幫派的指令 */
  if ( IS_NPC( ch ) ) RETURN_NULL();

  if ( !club_list )
  {
    act( "$t尚未有任何幫派資料。", ch, mud_name, NULL, TO_CHAR );
    RETURN_NULL();
  }

  /* 擷取參數 */
  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );
  argument = one_argument( argument, arg3 );

  /* 沒有參數就是查詢 */
  if ( !arg1[0] )
  {
    clear_buffer();
    send_to_buffer( "\e[1;32m%s的幫派資料如下﹕\e[0m\n\r\n\r"
      "\e[1;33;44m順序 幫派英文名稱   幫派中文名稱         幫主          "
      "狀  態 成員     幫庫\e[0m\n\r" , mud_name );

    for ( count = 0, pClub = club_list; pClub; pClub = pClub->next )
    {
      tablize( FORMAT_SET,
        "\r NU) NAME           CHIN                \e[0m "
        "MAS           STATUS MEMB CLUBGO  \e[0m\n\r"
        , 0, NULL, 0 );

      tablize( FORMAT_NUM,    NULL,                          ++count,     "NU", 2  );
      tablize( FORMAT_STRING, pClub->name,                         0,   "NAME", 15 );
      tablize( FORMAT_STRING, pClub->cname,                        0,   "CHIN", 20 );
      tablize( FORMAT_STRING, pClub->master ? pClub->master : "從缺", 0, "MAS", 12 );
      tablize( FORMAT_STRING, club_status( pClub ),                0, "STATUS", 6  );
      tablize( FORMAT_NUM,  NULL, count_club_member( pClub, CLUB_ALL ), "MEMB", 4  );
      tablize( FORMAT_NUM,    NULL,                     pClub->money, "CLUBGO", 8  );
      tablize( FORMAT_RETURN, pellet, 0, NULL, sizeof( pellet ) - 1 );

      send_to_buffer( "%s", pellet );
      if ( buffer_full() ) break;
    }

    if ( count == 0 ) send_to_buffer( "沒有任何幫派的資料。\n\r" );
    print_buffer( ch );
    RETURN_NULL();
  }

  /* 創幫 */
  else if ( !str_prefix( arg1, "!create" ) )
  {
    /* 檢查等級 */
    if ( !IS_HERO( ch ) )
    {
      chinese_number( LEVEL_HERO, buf );
      act( "對不起﹐你必須要達到$t級才可以。", ch, buf, NULL, TO_CHAR );
      RETURN_NULL();
    }

    /* 檢查幫派 */
    if ( ch->club )
    {
      send_to_char( "抱歉﹐你已經有加入其他幫派了。\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查參數 */
    if ( !arg2[0] || !arg3[0] )
    {
      send_to_char( "你必須指定幫派的英文以及中文名稱。\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查是否名稱重複 */
    if ( clubname_lookup( arg2, CLUB_NAME )
      || clubname_lookup( arg3, CLUB_CNAME ) )
    {
      send_to_char( "這個名稱有人用過了。\n\r", ch );
      RETURN_NULL();
    }

    /* 避免有空白字 */
    for ( pString = arg2; *pString; pString++ )
    {
      if ( !isalpha( ( int ) *pString ) )
      {
        send_to_char( "這個英文名稱不合法。\n\r", ch );
        RETURN_NULL();
      }
    }

    for ( pString = arg3; *pString; pString++ )
    {
      if ( *pString == '%' )
      {
        send_to_char( "這個中文名稱不合法。\n\r", ch );
        RETURN_NULL();
      }
    }

    /* 檢查創幫經費 */
    if ( ch->bank < ClubCreateFund )
    {
      act( "等你湊足了$i兩黃金才能創幫﹗", ch, &ClubCreateFund, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( !( pClub = create_club( ch, arg2, arg3 ) ) )
    {
      send_to_char( "抱歉﹐系統無法幫你產生一個幫派。\n\r", ch );
      RETURN_NULL();
    }

    sprintf( buf, "%s\e[0m 創了 %s\e[0m(%s)幫派﹐懇請大家連署﹐謝謝﹗\n\r"
      , mob_name( NULL, ch ), pClub->cname, pClub->name );

    send_to_all_char( buf );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg1, "!countersign" ) )
  {
    /* 檢查等級 */
    if ( !IS_HERO( ch ) )
    {
      chinese_number( LEVEL_HERO, buf );
      act( "對不起﹐你必須要達到$t級才可以。", ch, buf, NULL, TO_CHAR );
      RETURN_NULL();
    }

    /* 檢查幫派 */
    if ( ch->club )
    {
      send_to_char( "抱歉﹐你已經有加入其他幫派了。\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查連署經費 */
    if ( ch->gold < ClubCountersignFund )
    {
      act( "等你湊足了$x兩銀子才能連署﹗", ch, &ClubCountersignFund, NULL, TO_CHAR );
      RETURN_NULL();
    }

    /* 檢查連署旗標 */
    if ( ch->clublog )
    {
      send_to_char( "對不起﹐你已經有連署過了﹗\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查旗標 */
    if ( !arg2[0] )
    {
      send_to_char( "你必須註明連署哪一個幫派。\n\r", ch );
      RETURN_NULL();
    }

    /* 查詢是否有這樣的幫派 */
    if ( !( pClub = clubname_lookup( arg2, CLUB_NAME ) ) )
    {
      act( "抱歉﹐找不到你要連署的幫派名稱 $2$T$0。", ch, NULL, arg2, TO_CHAR );
      RETURN_NULL();
    }

    /* 察看狀態 */
    if ( pClub->status != CLUB_STATUS_COUNTERSIGN )
    {
      send_to_char( "本幫派目前不接受連署。\n\r", ch );
      RETURN_NULL();
    }

    if ( !countersign_club( ch, pClub ) )
    {
      send_to_char( "抱歉﹐你連署失敗。\n\r", ch );
      RETURN_NULL();
    }

    sprintf( buf, "%s 已經連署了 %s\e[0m(%s) 幫派﹗\n\r"
      , mob_name( NULL, ch ), pClub->cname, pClub->name );

    send_to_all_char( buf );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg1, "!join" ) )
  {
    /* 檢查幫派 */
    if ( ch->club )
    {
      send_to_char( "抱歉﹐你已經有加入其他幫派了。\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查參數 */
    if ( !arg2[0] )
    {
      send_to_char( "你必須註明參加哪一個幫派。\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查是否有這樣的幫派 */
    if ( !( pClub = clubname_lookup( arg2, CLUB_NAME ) ) )
    {
      act( "找不到你要參加的幫派名稱 $2$T$0。", ch, NULL, arg2, TO_CHAR );
      RETURN_NULL();
    }

    /* 幫派狀態是否可以加入 */
    if ( pClub->status != CLUB_STATUS_UNIONIZE )
    {
      send_to_char( "現在不能加入本幫派。\n\r", ch );
      RETURN_NULL();
    }

    /* 是否有錢繳交規費 */
    if ( ch->gold < ClubExpense )
    {
      act( "等你湊足了$i兩銀子才能加入幫派﹗", ch, &ClubExpense, NULL, TO_CHAR );
      RETURN_NULL();
    }

    /* 檢查人數是否已滿 */
    if ( clubmember_full( pClub, CLUB_FOLLOWER ) )
    {
      send_to_char( "本幫派人數已滿。\n\r", ch );
      RETURN_NULL();
    }

    /* 加入到表格 */
    if ( char_to_club( ch->name, pClub, CLUB_FOLLOWER ) )
    {
      gold_from_char( ch, ClubExpense );

      if ( pClub->money + ClubExpense / 2 <= MAX_ASSET )
        pClub->money = UMAX( 0, pClub->money + ClubExpense / 2 );

      sprintf( buf, "%s 加入了本幫派。\n\r", mob_name( NULL, ch ) );
      send_to_club( buf, pClub );

      act( "你加入了 $y 幫派。", ch, pClub, NULL, TO_CHAR );
    }

    else
    {
      send_to_char( "對不起﹐你不能參加本幫派。\n\r", ch );
    }

    RETURN_NULL();
  }

  /* 查詢某幫派, 沒有下一個參數就是查詢自己幫派 */
  else if ( !str_prefix( arg1, "!who" ) )
  {
    pClub = arg2[0] == '\x0' ? ch->club : clubname_lookup( arg2, CLUB_NAME );

    /* 查詢幫派名稱, 是否存在 */
    if ( !pClub )
    {
      send_to_char( "找不到你要觀看的幫派名稱。\n\r", ch );
      RETURN_NULL();
    }

    club_sort( pClub );

    clear_buffer();
    send_to_buffer( "%s\e[0m(%s) 的成員名單「共 %d 人﹐幫庫 %d﹐%s"
      , pClub->cname, pClub->name
      , count_club_member( pClub, CLUB_ALL )
      , pClub->money, club_status( pClub ) );

    /* 只有神族可以看到房間號碼 */
    if ( IS_IMMORTAL( ch ) )
    {
      send_to_buffer( "﹐房間號碼 %d"
        , pClub->location ? pClub->location->vnum : -1 );
    }

    send_to_buffer( "」\n\r"
      "\e[1;33;44m職  位 上線 名字         中文名字            "
      " 等級 年齡 性別 確認 職  業       \e[0m\n\r" );

    /* 印出幫主 */
    if ( pClub->master )
    {
      victim = is_online( pClub->master );
      if ( ( pBasic = name2basic( pClub->master ) ) )
      {
        pClass  = is_class( pBasic->class, TRUE );
        tablize( FORMAT_SET,
          "幫  主 ONLI NAME         CNAME                \e[0mLEVV AGEE "
          "SEXX VERI CLASSS      \e[0m\n\r"
          , 0, NULL, 0 );

        tablize( FORMAT_STRING, pClub->master  , 0, "NAME" , 12 );
        tablize( FORMAT_STRING, pBasic->cname  , 0, "CNAME", 20 );
        tablize( FORMAT_STRING, YESNO( 1 )     , 0, "VERI" ,  4 );
        tablize( FORMAT_STRING, YESNO( victim ), 0, "ONLI" ,  4 );

        tablize( FORMAT_STRING, class_name( pClass, FALSE )
          , 0, "CLASSS" , 6 );

        tablize( FORMAT_NUM   , NULL         , pBasic->level, "LEVV" ,  4 );

        tablize( FORMAT_NUM   , NULL
          , ( pBasic->played / 14400 ) + BornAge, "AGEE",  4 );

        tablize( FORMAT_STRING, sex_name( pBasic->sex ), 0, "SEXX", 4 );
        tablize( FORMAT_RETURN, pellet, 0, NULL, sizeof( pellet ) - 1 );
        send_to_buffer( "%s", pellet );

        free_basic( pBasic );
      }
      else
      {
        send_to_buffer( "幫  主 %2s   %-12s ????                "
          " ???? ???? ???? %2s   ??????\n\r"
          , YESNO( victim ), pClub->master, YESNO( 1 ) );
      }
    }
    else
    {
      send_to_buffer( "幫  主 %2s   從缺\n\r", YESNO( 0 ) );
    }

    if ( pClub->vicemaster )
    {
      victim = is_online( pClub->vicemaster );

      if ( ( pBasic = name2basic( pClub->vicemaster ) ) )
      {
        pClass = is_class( pBasic->class, TRUE );

        tablize( FORMAT_SET,
          "副幫主 ONLI NAME         CNAME                \e[0mLEVV AGEE"
          " SEXX VERI CLASSS      \e[0m\n\r"
          , 0, NULL, 0 );

        tablize( FORMAT_STRING, pClub->vicemaster, 0, "NAME" , 12 );
        tablize( FORMAT_STRING, pBasic->cname    , 0, "CNAME", 20 );
        tablize( FORMAT_STRING, YESNO( 1 )       , 0, "VERI" ,  4 );
        tablize( FORMAT_STRING, YESNO( victim )  , 0, "ONLI" ,  4 );
        tablize( FORMAT_NUM   , NULL         , pBasic->level, "LEVV" ,  4 );

        tablize( FORMAT_STRING, class_name( pClass, FALSE )
          , 0, "CLASSS" , 6 );

        tablize( FORMAT_NUM   , NULL
          , ( pBasic->played / 14400 ) + BornAge, "AGEE",  4 );

        tablize( FORMAT_STRING, sex_name( pBasic->sex ), 0, "SEXX", 4 );
        tablize( FORMAT_RETURN, pellet, 0, NULL, sizeof( pellet ) - 1 );
        send_to_buffer( "%s", pellet );

        free_basic( pBasic );
      }
      else
      {
        send_to_buffer( "副幫主 %2s   %-12s ????                "
          " ???? ???? ???? %2s   ??????\n\r"
          , YESNO( victim ), pClub->vicemaster, YESNO( 1 ) );
      }
    }

    /* 印出長老 */
    for ( loop = 0; loop < MAX_CLUB_DOYEN; loop++ )
    {
      if ( pClub->doyen[loop] )
      {
        victim = is_online( pClub->doyen[loop] );

        if ( ( pBasic = name2basic( pClub->doyen[loop] ) ) )
        {
          pClass = is_class( pBasic->class, TRUE );

          tablize( FORMAT_SET,
            "長  老 ONLI NAME         CNAME                \e[0mLEVV AGEE "
            "SEXX VERI CLASSS      \e[0m\n\r"
            , 0, NULL, 0 );

          tablize( FORMAT_STRING, pBasic->name     , 0, "NAME" , 12 );
          tablize( FORMAT_STRING, pBasic->cname    , 0, "CNAME", 20 );
          tablize( FORMAT_STRING, YESNO( 1 )       , 0, "VERI" ,  4 );
          tablize( FORMAT_STRING, YESNO( victim )  , 0, "ONLI" ,  4 );
          tablize( FORMAT_NUM   , NULL , pBasic->level, "LEVV" ,  4 );

          tablize( FORMAT_STRING, class_name( pClass, FALSE )
            , 0, "CLASSS" , 6 );

          tablize( FORMAT_NUM   , NULL
            , ( pBasic->played / 14400 ) + BornAge, "AGEE",  4 );

          tablize( FORMAT_STRING, sex_name( pBasic->sex ), 0, "SEXX", 4 );
          tablize( FORMAT_RETURN, pellet, 0, NULL, sizeof( pellet ) - 1 );
          send_to_buffer( "%s", pellet );

          free_basic( pBasic );
        }
        else
        {
          send_to_buffer( "長  老 %2s   %-12s ????                "
            " ???? ???? ???? %2s   ??????\n\r"
            , YESNO( victim ), pClub->doyen[loop], YESNO( 1 ) );
        }
      }
    }

    /* 列印幫眾 */
    for ( loop = 0; loop < MAX_CLUB_MEMBER; loop++ )
    {
      if ( pClub->member[loop] && pClub->verify[loop] )
      {
        victim = is_online( pClub->member[loop] );

        if ( ( pBasic = name2basic( pClub->member[loop] ) ) )
        {
          pClass = is_class( pBasic->class, TRUE );

          tablize( FORMAT_SET,
            "幫  眾 ONLI NAME         CNAME                \e[0mLEVV AGEE "
            "SEXX VERI CLASSS      \e[0m\n\r"
            , 0, NULL, 0 );

          tablize( FORMAT_STRING, pBasic->name     , 0, "NAME" , 12 );
          tablize( FORMAT_STRING, pBasic->cname    , 0, "CNAME", 20 );
          tablize( FORMAT_STRING, YESNO( 1 )       , 0, "VERI" ,  4 );
          tablize( FORMAT_STRING, YESNO( victim )  , 0, "ONLI" ,  4 );
          tablize( FORMAT_NUM   , NULL , pBasic->level, "LEVV" ,  4 );

          tablize( FORMAT_STRING, class_name( pClass, FALSE )
            , 0, "CLASSS" , 6 );

          tablize( FORMAT_NUM   , NULL
            , ( pBasic->played / 14400 ) + BornAge, "AGEE",  4 );

          tablize( FORMAT_STRING, sex_name( pBasic->sex ), 0, "SEXX", 4 );
          tablize( FORMAT_RETURN, pellet, 0, NULL, sizeof( pellet ) - 1 );
          send_to_buffer( "%s", pellet );
          free_basic( pBasic );
          if ( buffer_full() ) break;
        }
        else
        {
          send_to_buffer( "幫  眾 %2s   %-12s ????                "
            " ???? ???? ???? %2s   ??????\n\r"
            , YESNO( victim ), pClub->member[loop], YESNO( 1 ) );
        }
      }
    }

    /* 列印弟子 */
    for ( loop = 0; loop < MAX_CLUB_MEMBER; loop++ )
    {
      if ( pClub->member[loop] && !pClub->verify[loop] )
      {
        victim = is_online( pClub->member[loop] );

        if ( ( pBasic = name2basic( pClub->member[loop] ) ) )
        {
          pClass = is_class( pBasic->class, TRUE );

          tablize( FORMAT_SET,
            "弟  子 ONLI NAME         CNAME                \e[0mLEVV AGEE "
            "SEXX VERI CLASSS      \e[0m\n\r"
            , 0, NULL, 0 );

          tablize( FORMAT_STRING, pBasic->name     , 0, "NAME" , 12 );
          tablize( FORMAT_STRING, pBasic->cname    , 0, "CNAME", 20 );
          tablize( FORMAT_STRING, YESNO( 0 )       , 0, "VERI" ,  4 );
          tablize( FORMAT_STRING, YESNO( victim )  , 0, "ONLI" ,  4 );
          tablize( FORMAT_NUM   , NULL , pBasic->level, "LEVV" ,  4 );

          tablize( FORMAT_STRING, class_name( pClass, FALSE )
            , 0, "CLASSS" , 6 );

          tablize( FORMAT_NUM   , NULL
            , ( pBasic->played / 14400 ) + BornAge, "AGEE",  4 );

          tablize( FORMAT_STRING, sex_name( pBasic->sex ), 0, "SEXX", 4 );
          tablize( FORMAT_RETURN, pellet, 0, NULL, sizeof( pellet ) - 1 );
          send_to_buffer( "%s", pellet );
          free_basic( pBasic );
          if ( buffer_full() ) break;
        }
        else
        {
          send_to_buffer( "弟  子 %2s   %-12s ????                "
            " ???? ???? ???? %2s   ??????\n\r"
            , YESNO( victim ), pClub->member[loop], YESNO( 0 ) );
        }
      }
    }

    print_buffer( ch );
    RETURN_NULL();
  }

  /* 認證幫眾 */
  else if ( !str_prefix( arg1, "!verify" ) )
  {
    /* 檢查幫派 */
    if ( !( pClub = ch->club ) )
    {
      send_to_char( "你沒有參加幫派。\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查是否有職權 */
    if ( !right_ring( ch->name, ch->club, CLUB_DOYEN ) )
    {
      send_to_char( "你沒有權力去認證幫內弟子。\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查幫派的狀態是否可以認證幫眾了 */
    if ( pClub->status != CLUB_STATUS_UNIONIZE )
    {
      send_to_char( "現在還不能認證你的弟子。\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查參數 */
    if ( !arg2[0] )
    {
      send_to_char( "你要認證那位幫內弟子。\n\r", ch );
      RETURN_NULL();
    }

    /* 根本不是幫內之人 */
    if ( ( level = name_in_club( arg2, pClub ) ) == CLUB_NO_RELATION )
    {
      send_to_char( "你沒有這位幫眾。\n\r", ch );
      RETURN_NULL();
    }

    /* 只有幫眾才需要認證 */
    if ( level != CLUB_FOLLOWER )
    {
      send_to_char( "你只能認證你的幫眾弟子。\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查是否真的有這位幫眾, 或是已經通過認證 */
    for ( loop = 0; loop < MAX_CLUB_MEMBER; loop++ )
    {
      if ( pClub->member[loop] && !str_cmp( arg2, pClub->member[loop] ) )
      {
        if ( pClub->verify[loop] )
        {
          send_to_char( "你的弟子已經通過認證了。\n\r", ch );
          RETURN_NULL();
        }

        pClub->verify[loop] = TRUE;
        send_to_char( "恭禧﹐你的弟子已經通過認證。\n\r", ch );

        /* 察看對象是否上線 */
        if ( ( victim = is_online( arg2 ) ) )
        {
          victim->club = pClub;
          send_to_char( "恭禧你﹐通過幫主的認證。\n\r", victim );
        }

        RETURN_NULL();
      }
    }
  }

  /* 轉帳給其他弟子 */
  else if ( !str_prefix( arg1, "!dump" ) )
  {
    /* 檢查幫派 */
    if ( !( pClub = ch->club ) )
    {
      send_to_char( "你沒有參加幫派。\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查是否有職權 */
    if ( !right_ring( ch->name, pClub, CLUB_VICEMASTER ) )
    {
      send_to_char( "你沒有權力去轉帳給幫內弟子。\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查幫派的狀態是否可以轉帳給幫眾了 */
    if ( pClub->status != CLUB_STATUS_UNIONIZE )
    {
      send_to_char( "現在還不能轉帳給你的弟子。\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查參數 */
    if ( !arg2[0] )
    {
      send_to_char( "你要轉帳給那位弟子﹖\n\r", ch );
      RETURN_NULL();
    }

    /* 察看對象是否上線, 必須上線才能轉帳 */
    if ( !( victim = is_online( arg2 ) ) )
    {
      send_to_char( "你那位弟子目前沒有上線。\n\r", ch );
      RETURN_NULL();
    }

    /* 是否同一個幫派, 不同幫派不能轉帳 */
    if ( !is_same_club( ch, victim ) )
    {
      send_to_char( "那位不是你幫內的人士。\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查參數 */
    if ( !arg3[0] )
    {
      send_to_char( "你要轉多少錢給他呢﹖\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查參數 */
    if ( ( count = atoi( arg3 ) ) <= 0 || count > pClub->money )
    {
      send_to_char( "你要轉芭樂票給他嗎﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( over_scale( victim ) )
    {
      act( "$N的總資產已達上限﹗", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    act( "你拿了 $i 元給你的弟子$N。", ch, &count, victim, TO_CHAR );
    act( "$N 拿了 $i 元給你。", victim, &count, ch, TO_CHAR );

    pClub->money -= count;
    gold_to_char( victim, count );

    RETURN_NULL();
  }

  /* 邀請 */
  else if ( !str_prefix( arg1, "!invite" ) )
  {
    /* 檢查幫派 */
    if ( !( pClub = ch->club ) )
    {
      send_to_char( "你還沒有加入幫派﹐怎麼邀請。\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查幫派的狀態是否可以邀請 */
    if ( pClub->status != CLUB_STATUS_UNIONIZE )
    {
      send_to_char( "現在還不能邀請其他人參加幫派。\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查是否有職權 */
    if ( !right_ring( ch->name, pClub, CLUB_MEMBER ) )
    {
      send_to_char( "你沒有權力去邀請別人入幫耶。\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查參數 */
    if ( !arg2[0] )
    {
      send_to_char( "你要邀請誰呢﹖\n\r", ch );
      RETURN_NULL();
    }

    /* 察看對象是否上線, 上線才能邀請 */
    if ( !( victim = get_char_world( ch, arg2 ) ) )
    {
      act( "找不到你要邀請的人 $2$T$0。", ch, NULL, arg2, TO_CHAR );
      RETURN_NULL();
    }

    /* 檢查是否可以邀請他 */
    if ( IS_NPC( victim ) || victim->club )
    {
      act( "你無法邀請$N。", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    act( "你邀請$N來參加你的幫派。", ch, NULL, victim, TO_CHAR );
    act( "$N邀請你參加他的幫派$y。", victim, pClub, ch, TO_CHAR );
    RETURN_NULL();
  }

  /* 捐獻 */
  else if ( !str_prefix( arg1, "!donate" ) )
  {
    /* 檢查幫派 */
    if ( !( pClub = ch->club ) )
    {
      send_to_char( "你都沒有加入幫派﹐如何捐輸。\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->trade == FALSE )
    {
      send_to_char( "你的總資產太可怕了﹐所以無法執行這個指令﹗\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查參數 */
    if ( !arg2[0] )
    {
      send_to_char( "你要捐多少錢呢﹖\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查數目是否合理 */
    if ( ( count = atoi( arg2 ) ) <= 0 || count > ch->gold )
    {
      send_to_char( "你捐的是紙錢嗎﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( pClub->money + count <= MAX_ASSET )
      pClub->money = UMAX( 0, pClub->money + count );

    gold_from_char( ch, count );

    sprintf( buf, " %s捐了本幫 %d 元。\n\r", mob_name( NULL, ch ), count );
    send_to_club( buf, pClub );

    RETURN_NULL();
  }

  /* 領錢 */
  else if ( !str_prefix( arg1, "!withdraw" ) )
  {
    /* 檢查幫派 */
    if ( !( pClub = ch->club ) )
    {
      send_to_char( "你還沒有參加幫派﹐如何領錢﹖\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查幫派的狀態是否可以領錢 */
    if ( pClub->status != CLUB_STATUS_UNIONIZE )
    {
      send_to_char( "現在還不能領幫內的錢。\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查是否有職權 */
    if ( !right_ring( ch->name, pClub, CLUB_DOYEN ) )
    {
      send_to_char( "你沒有權力去領幫內的錢。\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查參數 */
    if ( !arg2[0] )
    {
      send_to_char( "你要領多少幫內的錢﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( over_scale( ch ) )
    {
      send_to_char( "你的總資產已達上限﹐無法繼續領取﹗\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查數目是否合理 */
    if ( ( count = atoi( arg2 ) ) <= 0 || count > pClub->money )
    {
      send_to_char( "你領的是紙錢嗎﹖\n\r", ch );
      RETURN_NULL();
    }

    pClub->money -= count;
    gold_to_char( ch, count );

    sprintf( buf, " %s 從幫內領了 %d 元。\n\r", mob_name( NULL, ch ), count );
    send_to_club( buf, pClub );

    RETURN_NULL();
  }

  /* 驅逐幫眾 */
  else if ( !str_prefix( arg1, "!kick" ) )
  {
    /* 檢查幫派 */
    if ( !( pClub = ch->club ) )
    {
      send_to_char( "你沒有參加幫派﹐無法執行。\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查幫派的狀態是否可以驅逐幫眾 */
    if ( pClub->status != CLUB_STATUS_UNIONIZE )
    {
      send_to_char( "現在還不能驅逐幫內人士。\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查是否有職權 */
    if ( !right_ring( ch->name, pClub, CLUB_VICEMASTER ) )
    {
      send_to_char( "你沒有權力去驅逐幫眾。\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查參數 */
    if ( !arg2[0] )
    {
      send_to_char( "你要驅逐那位幫眾。\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查是否真的有這位幫眾 */
    if ( ( level = name_in_club( arg2, pClub ) ) == CLUB_NO_RELATION )
    {
      send_to_char( "你沒有那位幫眾。\n\r", ch );
      RETURN_NULL();
    }

    /* 只能驅逐幫眾, 其他只能用降級之後再驅逐 */
    if ( level >= CLUB_DOYEN )
    {
      send_to_char( "你只能驅逐你的幫眾。\n\r", ch );
      RETURN_NULL();
    }

    if ( !char_from_club( arg2, pClub, level ) )
    {
      send_to_char( "無法驅逐那位幫眾。\n\r", ch );
      RETURN_NULL();
    }

    if ( ( victim = is_online( arg2 ) ) )
      send_to_char( "你被幫內驅逐出幫了。\n\r", victim );

    sprintf( buf, "%s 被驅逐出幫了。\n\r", arg2 );
    send_to_club( buf, pClub );

    RETURN_NULL();
  }

  /* 叛幫 */
  else if ( !str_prefix( arg1, "!quit" ) )
  {
    if ( !arg2[0] || !( pClub = clubname_lookup( arg2, CLUB_NAME ) ) )
    {
      send_to_char( "你必須要把你的幫派名字拼對才可以喔﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( ( level = name_in_club( ch->name, pClub ) ) == CLUB_NO_RELATION )
    {
      send_to_char( "你沒有參加這個幫派﹐如何叛幫﹗\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查幫派的狀態是否可以叛幫 */
    if ( pClub->status != CLUB_STATUS_UNIONIZE )
    {
      send_to_char( "現在還不能叛幫。\n\r", ch );
      RETURN_NULL();
    }

    if ( level == CLUB_MASTER )
    {
      send_to_char( "因為你的職務﹐所以你不能叛幫。\n\r", ch );
      RETURN_NULL();
    }

    if ( char_from_club( ch->name, pClub, level ) )
    {
      if ( level == CLUB_FOLLOWER )
      {
        send_to_char( "你叛幫了﹐但是還沒被幫內認證﹐所以沒事﹗\n\r", ch );
      }

      else
      {
        send_to_char( "你叛幫了﹐全世界的人都可以追殺你了﹗\n\r", ch );
        SET_BIT( ch->act, PLR_BOLTER );
      }

      sprintf( buf, "%s 已經叛幫出走了﹗\n\r", mob_name( NULL, ch ) );
      send_to_club( buf, pClub );
    }

    else
    {
      send_to_char( "你叛幫失敗﹗\n\r" , ch );
    }
  }

  /* 驅逐 */
  else if ( !str_prefix( arg1, "!dispel" ) )
  {
    /* 檢查幫派 */
    if ( !( pClub = ch->club ) )
    {
      send_to_char( "你沒有參加幫派﹐無法執行驅逐幫派人士。\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查幫派的狀態是否可以驅逐幫眾 */
    if ( pClub->status != CLUB_STATUS_UNIONIZE )
    {
      send_to_char( "現在還不能驅逐幫內人士。\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查是否有職權 */
    if ( !right_ring( ch->name, pClub, CLUB_MASTER ) )
    {
      send_to_char( "你沒有權力去驅逐幫眾。\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查參數 */
    if ( !arg2[0] )
    {
      send_to_char( "你要驅逐那位幫眾。\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查是否真的有這位幫眾 */
    if ( ( level = name_in_club( arg2, pClub ) ) == CLUB_NO_RELATION )
    {
      send_to_char( "你沒有那位幫眾。\n\r", ch );
      RETURN_NULL();
    }

    if ( level == CLUB_MASTER )
    {
      send_to_char( "你不能驅逐幫主。\n\r", ch );
      RETURN_NULL();
    }

    if ( !( victim = is_online( arg2 ) ) )
    {
      act( "$t沒有上線﹐你不能驅逐他。", ch, arg2, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( char_from_club( arg2, pClub, level ) )
    {
      SET_BIT( victim->act, PLR_BOLTER );
      send_to_char( "你被幫主驅逐出幫了。\n\r", victim );
      sprintf( buf, "%s被幫主驅逐出幫了。\n\r", mob_name( NULL, victim ) );
      send_to_club( buf, pClub );
    }

    else
    {
      send_to_char( "驅逐幫眾失敗。\n\r", ch );
      RETURN_NULL();
    }

    RETURN_NULL();
  }

  /* 任命 */
  else if ( !str_prefix( arg1, "!appoint" ) )
  {
    /* 檢查幫派 */
    if ( !( pClub = ch->club ) )
    {
      send_to_char( "你沒有參加幫派﹐無法執行任命職位。\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查幫派的狀態是否可以任命幫眾 */
    if ( pClub->status != CLUB_STATUS_UNIONIZE )
    {
      send_to_char( "現在還不能任命幫內人士。\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查是否有職權 */
    if ( !right_ring( ch->name, pClub, CLUB_MASTER ) )
    {
      send_to_char( "抱歉﹐你沒有權力去任命幫眾。\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查參數 */
    if ( !arg2[0] )
    {
      send_to_char( "你要任命那位幫眾。\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查參數 */
    if ( !arg3[0] )
    {
      send_to_char( "你要任命他為副幫主(vicemaster)或長老(doyen)﹖\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查是否真的有這位幫眾 */
    if ( ( level = name_in_club( arg2, pClub ) ) == CLUB_NO_RELATION )
    {
      send_to_char( "你沒有那位幫眾。\n\r", ch );
      RETURN_NULL();
    }

    if ( level == CLUB_FOLLOWER )
    {
      send_to_char( "那位幫眾沒有通過認證。\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查職位 */
    if ( level != CLUB_MEMBER )
    {
      send_to_char( "你只能任命幫眾為副幫主或長老。\n\r", ch );
      RETURN_NULL();
    }

    victim = is_online( arg2 );

    /* 任命為副幫主 */
    if ( !str_prefix( arg3, "vicemaster" ) )
    {
      /* 檢查是否已經有副幫主 */
      if ( clubmember_full( pClub, CLUB_VICEMASTER ) )
      {
        send_to_char( "幫派之中已經有副幫主了。\n\r", ch );
        RETURN_NULL();
      }

      if ( appoint_member( arg2, pClub, CLUB_VICEMASTER ) )
      {
        sprintf( buf, "%s任命%s為本幫的副幫主。\n\r"
          , mob_name( NULL, ch ), arg2 );

        send_to_club( buf, pClub );
        if ( victim ) send_to_char( "你被任命為本幫副幫主。\n\r", victim );
      }

      else
      {
        send_to_char( "任命副幫主失敗。\n\r", ch );
      }

      RETURN_NULL();
    }

    /* 任命為長老 */
    else if ( !str_prefix( arg3, "doyen" ) )
    {
      /* 檢查是否已經有長老 */
      if ( clubmember_full( pClub, CLUB_DOYEN ) )
      {
        send_to_char( "幫派之中長老名額已滿。\n\r", ch );
        RETURN_NULL();
      }

      if ( appoint_member( arg2, pClub, CLUB_DOYEN ) )
      {
        sprintf( buf, "%s任命%s為本幫的長老。\n\r"
          , mob_name( NULL, ch ), arg2 );

        send_to_club( buf, pClub );
        if ( victim ) send_to_char( "你被任命為本幫長老。\n\r", victim );
      }

      else
      {
        send_to_char( "任命長老失敗。\n\r", ch );
      }
    }

    else
    {
      send_to_char( "你要任命他為副幫主(vicemaster)或長老(doyen)﹖\n\r", ch );
    }
  }

  /* 解聘 */
  else if ( !str_prefix( arg1, "!dismiss" ) )
  {
    /* 檢查幫派 */
    if ( !( pClub = ch->club ) )
    {
      send_to_char( "你沒有參加幫派﹐無法執行。\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查幫派的狀態是否可以解聘幫眾 */
    if ( pClub->status != CLUB_STATUS_UNIONIZE )
    {
      send_to_char( "現在還不能解聘幫內人士。\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查是否有職權 */
    if ( !right_ring( ch->name, pClub, CLUB_MASTER ) )
    {
      send_to_char( "你沒有權力去解聘幫眾。\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查參數 */
    if ( !arg2[0] )
    {
      send_to_char( "你要解聘那位幫眾。\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查是否還有幫眾的空額 */
    if ( clubmember_full( pClub, CLUB_MEMBER ) )
    {
      send_to_char( "你的幫眾已經滿額了﹐所以無法卸任。\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查是否真的有這位幫眾 */
    if ( ( level = name_in_club( arg2, pClub ) ) == CLUB_NO_RELATION )
    {
      send_to_char( "你沒有那位幫眾。\n\r", ch );
      RETURN_NULL();
    }

    victim = is_online( arg2 );

    switch( level )
    {
    default:
      send_to_char( "你只能解聘副幫主或是長老。\n\r", ch );
      RETURN_NULL();

    /* 副幫主 */
    case CLUB_VICEMASTER:

      if ( appoint_member( arg2, pClub, CLUB_MEMBER ) )
      {
        sprintf( buf, "%s 把副幫主 %s 解聘了。\n\r"
          , mob_name( NULL, ch ), arg2 );

        send_to_club( buf, pClub );
        if ( victim ) send_to_char( "你不再是副幫主了。\n\r", victim );
      }

      else
      {
        send_to_char( "解聘副幫主失敗。\n\r", ch );
      }
      break;

    /* 長老 */
    case CLUB_DOYEN:

      if ( appoint_member( arg2, pClub, CLUB_MEMBER ) )
      {
        sprintf( buf, "%s 把長老 %s 解聘了。\n\r"
          , mob_name( NULL, ch ), arg2 );

        send_to_club( buf, pClub );
        if ( victim ) send_to_char( "你不再是長老了。\n\r", victim );
      }

      else
      {
        send_to_char( "解聘長老失敗。\n\r", ch );
      }

      break;
    }
  }

  /* 禪位 */
  else if ( !str_prefix( arg1, "!abdicate" ) )
  {
    /* 檢查幫派 */
    if ( !( pClub = ch->club ) )
    {
      send_to_char( "你沒有參加幫派﹐無法執行。\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查幫派的狀態是否可以禪位 */
    if ( pClub->status != CLUB_STATUS_UNIONIZE )
    {
      send_to_char( "現在還不能禪位給幫內人士。\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查是否有職權 */
    if ( !right_ring( ch->name, pClub, CLUB_MASTER ) )
    {
      send_to_char( "你沒有權力來禪位。\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查參數 */
    if ( !arg2[0] )
    {
      send_to_char( "你要禪位給那位幫眾﹖\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查是否真的有這位幫眾 */
    if ( ( level = name_in_club( arg2, pClub ) ) == CLUB_NO_RELATION )
    {
      send_to_char( "你沒有那位幫眾。\n\r", ch );
      RETURN_NULL();
    }

    if ( level == CLUB_FOLLOWER )
    {
      send_to_char( "抱歉﹐那位幫眾沒有通過認證。\n\r", ch );
      RETURN_NULL();
    }

    if ( clubmember_full( pClub , CLUB_MEMBER ) )
    {
      send_to_char( "你的幫眾已經滿額了﹐所以無法卸任。\n\r", ch );
      RETURN_NULL();
    }

    sprintf( buf, "幫主%s把幫主的位子禪位給%s。\n\r"
      , mob_name( NULL, ch ), arg2 );

    send_to_club( buf, pClub );

    appoint_member( ch->name, pClub, CLUB_MEMBER );
    appoint_member( arg2    , pClub, CLUB_MASTER );
  }

  else if ( !str_prefix( arg1, "!set" ) )
  {
    /* 檢查幫派 */
    if ( !( pClub = ch->club ) )
    {
      send_to_char( "你沒有參加幫派﹐無法執行。\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查幫派的狀態是否可以改變 */
    if ( pClub->status != CLUB_STATUS_UNIONIZE )
    {
      send_to_char( "現在還不能改變幫內設定。\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查是否有職權 */
    if ( !right_ring( ch->name, pClub, CLUB_MASTER ) )
    {
      send_to_char( "你沒有權力更改幫內設定。\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查參數 */
    if ( !arg2[0] )
    {
      send_to_char( "你要改什麼選項﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( !str_prefix( arg2, "name" ) )
    {
      /* 檢查參數 */
      if ( !arg3[0] )
      {
        send_to_char( "請問你要把英文名稱改成什麼﹖\n\r", ch );
        RETURN_NULL();
      }

      if ( clubname_lookup( arg3, CLUB_NAME ) )
      {
        send_to_char( "抱歉﹐這個英文名稱有人用過了。\n\r", ch );
        RETURN_NULL();
      }

      set_club( pClub, CLUB_NAME, arg3, 0 );

      sprintf( buf, "幫主 %s 把幫名改成 %s\e[0m(%s)。\n\r"
        , mob_name( NULL, ch ), pClub->cname, pClub->name );

      send_to_club( buf, pClub );
    }

    else if ( !str_prefix( arg2, "cname" ) )
    {
      /* 檢查參數 */
      if ( !arg3[0] )
      {
        send_to_char( "請問你要把中文名稱改成什麼﹖\n\r", ch );
        RETURN_NULL();
      }

      if ( clubname_lookup( arg3, CLUB_CNAME ) )
      {
        send_to_char( "抱歉﹐這個中文名稱有人用過了。\n\r", ch );
        RETURN_NULL();
      }

      set_club( pClub, CLUB_CNAME, arg3, 0 );

      sprintf( buf, "幫主 %s 把幫名改成 %s\e[0m(%s)。\n\r"
        , mob_name( NULL, ch ), pClub->cname, pClub->name );
      send_to_club( buf, pClub );
    }

    else
    {
      send_to_char( "選項錯誤﹐請用 club /? 查詢 club 指令。\n\r", ch );
    }
  }

  else if ( !str_prefix( arg1, "!fordo" ) )
  {
    if ( !( pClub = ch->club ) )
    {
      send_to_char( "你還沒有參加幫派。\n\r", ch );
      RETURN_NULL();
    }

    if ( !arg2[0] || pClub != clubname_lookup( arg2, CLUB_NAME ) )
    {
      send_to_char( "你必須要把你的幫派名字拼對才可以喔﹗\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查幫派的狀態是否可以解散 */
    if ( pClub->status != CLUB_STATUS_UNIONIZE )
    {
      send_to_char( "對不起﹐現在還不能解散幫派。\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查是否有職權 */
    if ( !right_ring( ch->name, pClub, CLUB_MASTER ) )
    {
      send_to_char( "抱歉﹐你沒有權力去解散幫派。\n\r", ch );
      RETURN_NULL();
    }

    sprintf( buf, "%s\e[0m(%s)幫幫主 %s 解散了幫派﹐從此 %s\e[0m 走進歷史﹗\n\r"
      , pClub->cname, pClub->name, mob_name( NULL, ch ), pClub->cname );

    if ( fordo_club( pClub ) )
    {
      send_to_char( "你解散幫派。\n\r", ch );
      send_to_all_char( buf );
    }

    else
    {
      send_to_char( "系統解散幫派失敗。\n\r", ch );
    }
  }

  /* 把信件列表 */
  else if ( !str_prefix( arg1, "!notelist" ) )
  {
    /* 檢查幫派 */
    if ( !( pClub = ch->club ) )
    {
      send_to_char( "你沒有參加幫派﹐無法執行信件列表。\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查幫派的狀態是否可以讀取信件 */
    if ( pClub->status != CLUB_STATUS_UNIONIZE || !pClub->location )
    {
      send_to_char( "現在還不能觀看幫內信件。\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查是否有職權 */
    if ( !right_ring( ch->name, pClub, CLUB_MEMBER ) )
    {
      send_to_char( "你沒有權力去看幫內信件列表。\n\r", ch );
      RETURN_NULL();
    }

    if ( !ch->in_room || ch->in_room != pClub->location )
    {
      send_to_char( "你必須到幫會聚集地才能看的到幫內信件。\n\r", ch );
      RETURN_NULL();
    }

    clear_buffer();

    send_to_char( "\e[1;33;44m編 號 讀取 寄信人與標題"
                  "                                 \e[0m\n\r", ch );
    for ( count = 0, pNote = note_list; pNote; pNote = pNote->next )
    {
      if ( is_note_to_club( pClub, pNote ) )
      {
        send_to_buffer( "[%3d] [%s] %s: %s\n\r"
          , ++count
          , YESNO( !( pNote->date_stamp > ch->last_clubnote
              && str_cmp( pNote->sender, ch->name ) ) )
          , pNote->sender
          , pNote->subject );

        if ( buffer_full() ) break;
      }
    }

    if ( count == 0 ) send_to_char( "幫會內並沒有任何的信件。\n\r", ch );
    else print_buffer( ch );
  }

  /* 讀取幫內信件 */
  else if ( !str_prefix( arg1, "!read" ) )
  {
    /* 檢查幫派 */
    if ( !( pClub = ch->club ) )
    {
      send_to_char( "你沒有參加幫派﹐無法讀取幫內信件。\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查幫派的狀態是否可以讀取信件 */
    if ( pClub->status != CLUB_STATUS_UNIONIZE || !pClub->location )
    {
      send_to_char( "現在還不能觀看幫內信件。\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查是否有職權 */
    if ( !right_ring( ch->name, pClub, CLUB_MEMBER ) )
    {
      send_to_char( "你沒有權力去看幫內信件。\n\r", ch );
      RETURN_NULL();
    }

    if ( !ch->in_room || ch->in_room != pClub->location )
    {
      send_to_char( "你必須到幫會聚集地才能看的到幫內信件。\n\r", ch );
      RETURN_NULL();
    }

    /* 檢查參數 */
    if ( !arg2[0] || ( count = atoi( arg2 ) ) <= 0 )
    {
      send_to_char( "你要讀第幾封幫內信﹖\n\r", ch );
      RETURN_NULL();
    }

    for ( pNote = note_list, loop = 0; pNote; pNote = pNote->next )
    {
      if ( is_note_to_club( pClub, pNote ) && ++loop == count )
      {
        ch->last_clubnote = UMAX( ch->last_clubnote, pNote->date_stamp );
        clear_buffer();

        send_to_buffer( "[%3d] 寄信者: %s\n\r"
                        "標  題: %s\n\r"
                        "日  期: %s\n\r"
                        "收信者: %s 幫內信\n\r"
                        "內  容:\n\r%s"
           , loop
           , pNote->sender
           , pNote->subject
           , pNote->date
           , pNote->to_list + 1
           , pNote->text );
        print_buffer( ch );
        RETURN_NULL();
      }
    }

    send_to_char( "沒有那封幫內信。\n\r", ch );
  }

  else
  {
    send_to_char( "指令錯誤, 請查詢 club 指令。\n\r", ch );
  }

  RETURN_NULL();
}

/* 查詢幫派名稱 */
CLUB_DATA * clubname_lookup( const char * name, int type )
{
  CLUB_DATA * pClub;

  PUSH_FUNCTION( "clubname_lookup" );

  /* 避免錯誤 */
  if ( !name || *name == '\x0' ) RETURN( NULL );

  switch( type )
  {
  default:
    mudlog( LOG_DEBUG, "clubname_lookup: 錯誤的選項 %d.", type );
    break;

  /* 英文名稱 */
  case CLUB_NAME:

    for ( pClub = club_list; pClub; pClub = pClub->next )
      if ( pClub->name && !str_cmp( name, pClub->name ) ) RETURN( pClub );

    break;

  /* 中文名稱 */
  case CLUB_CNAME:

    for ( pClub = club_list; pClub; pClub = pClub->next )
      if ( pClub->cname && !str_cmp( name, pClub->cname ) ) RETURN( pClub );

    break;
  }

  RETURN( NULL );
}

/* 創造幫派 */
CLUB_DATA * create_club( CHAR_DATA * ch, char * name, char * cname )
{
  CLUB_DATA * pClub;
  CLUB_DATA * zClub;

  PUSH_FUNCTION( "create_club" );

  if ( !ch
    || IS_NPC( ch )
    || !name
    || !cname
    || *name  == '\x0'
    || *cname == '\x0'
    || club_count() >= max_club )
    RETURN( NULL );

  /* 扣除經費 */
  ch->bank = UMAX( 0, ch->bank - ClubCreateFund );
  set_club_default( pClub = alloc_struct( STRUCT_CLUB_DATA ) );

  set_club( pClub, CLUB_NAME  , name  , 0 );
  set_club( pClub, CLUB_CNAME , cname , 0 );
  set_club( pClub, CLUB_STATUS, NULL  , CLUB_STATUS_COUNTERSIGN );
  set_club( pClub, CLUB_TIMER , NULL  , time( NULL ) );

  /* 設定幫主的職位 */
  char_to_club( ch->name, pClub, CLUB_MASTER );

  /* 設定連結 */
  for ( zClub = club_list;
        zClub && zClub->next;
        zClub = zClub->next );

  if ( !zClub ) club_list   = pClub;
  else          zClub->next = pClub;

  ch->club = pClub;

  RETURN( pClub );
}

/* 連署幫派 */
bool countersign_club( CHAR_DATA * ch, CLUB_DATA * pClub )
{
  char buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "countersign_club" );

  if ( !ch || !pClub || !char_to_club( ch->name, pClub, CLUB_MEMBER ) )
    RETURN( FALSE );

  ch->club     = pClub;
  ch->clublog  = TRUE;
  gold_from_char( ch, UMIN( ch->gold, ClubCountersignFund ) );

  /* 連署成功 */
  if ( count_club_member( pClub, CLUB_MEMBER ) >= ClubCountersignCount )
  {
    sprintf( buf, "%s\e[0m(%s)連署成功\﹐幫派成立。\n\r"
      , pClub->cname, pClub->name );
    send_to_all_char( buf );

    /* 設定狀態為成立, 並且製造出一個幫派房間 */
    set_club( pClub, CLUB_STATUS, NULL, CLUB_STATUS_UNIONIZE );
    club_location( pClub );
  }

  RETURN( TRUE );
}

/* 檢查某人是否在幫派之中 */
CLUB_DATA * char_club( CHAR_DATA * ch )
{
  PUSH_FUNCTION( "char_club" );

  /* 避免錯誤 */
  if ( !ch || IS_NPC( ch ) ) RETURN( NULL );

  /* 怪物是沒有幫派的 */
  if ( IS_NPC( ch ) ) RETURN( ( ch->club = NULL ) );

  RETURN( ( ch->club = is_inclub( ch->name ) ) );
}

/* 名稱是否在幫派之中了嗎? */
CLUB_DATA * is_inclub( char * name )
{
  CLUB_DATA * pClub;

  PUSH_FUNCTION( "is_inclub" );

  /* 避免錯誤 */
  if ( !name || *name == '\x0' ) RETURN( NULL );

  for ( pClub = club_list; pClub; pClub = pClub->next )
    if ( name_in_club( name, pClub ) != CLUB_NO_RELATION ) RETURN( pClub );

  RETURN( NULL );
}

/* 測試是否某人在幫派之中 */
int name_in_club( char * name, CLUB_DATA * pClub )
{
  int loop;

  PUSH_FUNCTION( "name_in_club" );

  /* 避免錯誤 */
  if ( !name || *name == '\x0' || !pClub ) RETURN( CLUB_NO_RELATION );

  /* 幫主 */
  if ( pClub->master && !str_cmp( name, pClub->master ) )
    RETURN( CLUB_MASTER );

  /* 副幫主 */
  if ( pClub->vicemaster && !str_cmp( name, pClub->vicemaster ) )
    RETURN( CLUB_VICEMASTER  );

  /* 長老 */
  for ( loop = 0; loop < MAX_CLUB_DOYEN; loop++ )
  {
    if ( pClub->doyen[loop] && !str_cmp( name, pClub->doyen[loop] ) )
      RETURN( CLUB_DOYEN );
  }

  /* 幫眾或弟子 */
  for ( loop = 0; loop < MAX_CLUB_MEMBER; loop++ )
  {
    if ( pClub->member[loop] && !str_cmp( name, pClub->member[loop] ) )
    {
      if ( pClub->verify[loop] ) RETURN( CLUB_MEMBER );
      RETURN( CLUB_FOLLOWER )
    }
  }

  RETURN( CLUB_NO_RELATION );
}

/* 把某人加入幫派的表格之中 */
bool char_to_club( char * name, CLUB_DATA * pClub, int type )
{
  int         loop;
  CHAR_DATA * victim;

  PUSH_FUNCTION( "char_to_club" );

  /* 避免錯誤 */
  if ( !name || *name == '\x0' || !pClub ) RETURN( FALSE );

  /* 移除不合法的字元 */
  smash_tilde( name );

  victim = is_online( name );

  switch( type )
  {
  default:
    mudlog( LOG_DEBUG, "char_to_club: 錯誤的選項 %d.", type );
    break;

  /* 幫主 */
  case CLUB_MASTER:

    if ( clubmember_full( pClub, CLUB_MASTER ) )
    {
      mudlog( LOG_DEBUG, "char_to_club: 已經有幫主" );
      break;
    }

    pClub->master = str_dup( name );
    if ( victim ) victim->club = pClub;
    RETURN( TRUE );

  /* 副幫主 */
  case CLUB_VICEMASTER:

    if ( clubmember_full( pClub, CLUB_VICEMASTER ) )
    {
      mudlog( LOG_DEBUG, "char_to_club: 已經有副幫主" );
      break;
    }

    pClub->vicemaster = str_dup( name );
    if ( victim ) victim->club = pClub;
    RETURN( TRUE );

  /* 長老 */
  case CLUB_DOYEN:

    if ( clubmember_full( pClub, CLUB_DOYEN ) )
    {
      mudlog( LOG_DEBUG, "char_to_club: 長老名額已滿." );
      break;
    }

    for ( loop = 0; loop < MAX_CLUB_DOYEN; loop++ )
    {
      if ( !pClub->doyen[loop] )
      {
        pClub->doyen[loop] = str_dup( name );
        if ( victim ) victim->club = pClub;
        RETURN( TRUE );
      }
    }

    mudlog( LOG_DEBUG, "char_to_club: 長老名額已滿." );
    break;

  /* 幫眾 */
  case CLUB_MEMBER:

    if ( clubmember_full( pClub, CLUB_MEMBER ) )
    {
      mudlog( LOG_DEBUG, "char_to_club: 幫眾名額已滿." );
      break;
    }

    for ( loop = 0; loop < MAX_CLUB_MEMBER; loop++ )
    {
      if ( !pClub->member[loop] )
      {
        pClub->member[loop] = str_dup( name );
        pClub->verify[loop] = TRUE;
        if ( victim ) victim->club = pClub;
        RETURN( TRUE );
      }
    }

    mudlog( LOG_DEBUG, "char_to_club: 幫眾名額已滿." );
    break;

  /* 弟子 */
  case CLUB_FOLLOWER:

    if ( clubmember_full( pClub, CLUB_FOLLOWER ) )
    {
      mudlog( LOG_DEBUG, "char_to_club: 幫眾名額已滿." );
      break;
    }

    for ( loop = 0; loop < MAX_CLUB_MEMBER; loop++ )
    {
      if ( !pClub->member[loop] )
      {
        pClub->member[loop] = str_dup( name );
        pClub->verify[loop] = FALSE;
        if ( victim ) victim->club = pClub;
        RETURN( TRUE );
      }
    }

    mudlog( LOG_DEBUG, "char_to_club: 幫眾名額已滿." );
    break;
  }

  RETURN( FALSE );
}

/* 把某個人從幫會之中移除 */
bool char_from_club( char * name, CLUB_DATA * pClub, int type )
{
  int         loop;
  CHAR_DATA * victim;

  PUSH_FUNCTION( "char_from_club" );

  /* 設定 CLUB_ALL 自動找尋其職位 */
  if ( type == CLUB_ALL ) type = name_in_club( name, pClub );

  victim = is_online( name );

  switch( type )
  {
  /* 錯誤的選項 */
  default:
    mudlog( LOG_DEBUG, "char_from_club: 錯誤的選項 %d.", type );
    break;

  /* 幫主 */
  case CLUB_MASTER:

    if ( !clubmember_full( pClub, CLUB_MASTER ) )
    {
      mudlog( LOG_DEBUG, "char_from_club: 幫主是空的." );
      break;
    }

    if ( str_cmp( name, pClub->master ) )
    {
      mudlog( LOG_DEBUG, "char_from_club: %s 不是 %s的幫主."
        , name, pClub->name );
      break;
    }

    free_string( pClub->master );
    pClub->master = NULL;
    if ( victim ) victim->club = NULL;
    RETURN( TRUE );

  /* 副幫主 */
  case CLUB_VICEMASTER:

    if ( !clubmember_full( pClub, CLUB_VICEMASTER ) )
    {
      mudlog( LOG_DEBUG, "char_from_club: 副幫主是空的." );
      break;
    }

    if ( str_cmp( name, pClub->vicemaster ) )
    {
      mudlog( LOG_DEBUG, "char_from_club: %s 不是 %s 的副幫主."
        , name, pClub->name );
      break;
    }

    free_string( pClub->vicemaster );
    pClub->vicemaster = NULL;
    if ( victim ) victim->club = NULL;
    RETURN( TRUE );

  /* 長老 */
  case CLUB_DOYEN:

    for ( loop = 0; loop < MAX_CLUB_DOYEN; loop++ )
    {
      if ( pClub->doyen[loop] && !str_cmp( name, pClub->doyen[loop] ) )
      {
        free_string( pClub->doyen[loop] );
        pClub->doyen[loop] = NULL;
        if ( victim ) victim->club = NULL;
        RETURN( TRUE );
      }
    }

    mudlog( LOG_DEBUG, "char_from_club: %s 不是 %s 的長老."
      , name, pClub->name );
    break;

  /* 幫眾或是弟子 */
  case CLUB_MEMBER:
  case CLUB_FOLLOWER:

    for ( loop = 0; loop < MAX_CLUB_MEMBER; loop++ )
    {
      if ( pClub->member[loop] && !str_cmp( name, pClub->member[loop] ) )
      {
        free_string( pClub->member[loop] );
        pClub->member[loop] = NULL;
        pClub->verify[loop] = FALSE;
        if ( victim ) victim->club = NULL;
        RETURN( TRUE );
      }
    }

    mudlog( LOG_DEBUG, "char_from_club: %s 不是 %s 的幫眾或弟子."
      , name, pClub->name );
    break;
  }

  RETURN( FALSE );
}

/* 任命幫眾職務 */
bool appoint_member( char * name, CLUB_DATA * pClub, int level )
{
  int orginal;

  PUSH_FUNCTION( "appoint_member" );

  /* 避免錯誤 */
  if ( !name
    || *name == '\x0'
    || !pClub
    || ( orginal = name_in_club( name, pClub ) ) == CLUB_NO_RELATION
    || orginal == level ) RETURN( FALSE );

  switch( level )
  {
  default:
    mudlog( LOG_DEBUG, "appoint_member: 錯誤的職等 %d.", level );
    break;

  /* 幫主 */
  case CLUB_MASTER:

    /* 看看是否名額已滿, 以及可以加入新的職業表格之中 */
    if ( clubmember_full( pClub, CLUB_MASTER )
      || !char_to_club( name, pClub, CLUB_MASTER ) )
    {
      mudlog( LOG_DEBUG, "appoint_member: 任命幫主失敗." );
      break;
    }

    RETURN( char_from_club( name, pClub, orginal ) );

  /* 副幫主 */
  case CLUB_VICEMASTER:

    /* 看看是否名額已滿, 以及可以加入新的職業表格之中 */
    if ( clubmember_full( pClub, CLUB_VICEMASTER )
      || !char_to_club( name, pClub, CLUB_VICEMASTER ) )
    {
      mudlog( LOG_DEBUG, "appoint_member: 任命副幫主失敗." );
      break;
    }

    RETURN( char_from_club( name, pClub, orginal ) );

  /* 長老 */
  case CLUB_DOYEN:

    /* 看看是否名額已滿, 以及可以加入新的職業表格之中 */
    if ( clubmember_full( pClub, CLUB_DOYEN )
      || !char_to_club( name, pClub, CLUB_DOYEN ) )
    {
      mudlog( LOG_DEBUG, "appoint_member: 任命長老失敗." );
      break;
    }

    RETURN( char_from_club( name, pClub, orginal ) );

  /* 幫眾 */
  case CLUB_MEMBER:

    /* 看看是否名額已滿, 以及可以加入新的職業表格之中 */
    if ( clubmember_full( pClub, CLUB_MEMBER )
      || !char_to_club( name, pClub, CLUB_MEMBER ) )
    {
      mudlog( LOG_DEBUG, "appoint_member: 任命幫眾失敗." );
      break;
    }

    RETURN( char_from_club( name, pClub, orginal ) );

  /* 弟子 */
  case CLUB_FOLLOWER:

    /* 看看是否名額已滿, 以及可以加入新的職業表格之中 */
    if ( clubmember_full( pClub, CLUB_FOLLOWER )
      || !char_to_club( name, pClub, CLUB_FOLLOWER ) )
    {
      mudlog( LOG_DEBUG, "appoint_member: 任命弟子失敗." );
      break;
    }

    RETURN( char_from_club( name, pClub, orginal ) );
  }

  RETURN( FALSE );
}

/* 計算幫派中某職位的人數 */
size_t count_club_member( CLUB_DATA * pClub, int type )
{
  size_t cMaster;
  size_t cVicemaster;
  size_t cDoyen;
  size_t cMember;
  size_t cFollower;
  int    loop;

  PUSH_FUNCTION( "count_club_member" );

  if ( !pClub ) RETURN( 0 );

  cMaster     = ( pClub->master     ) ? 1 : 0;
  cVicemaster = ( pClub->vicemaster ) ? 1 : 0;

  for ( cDoyen = loop = 0; loop < MAX_CLUB_DOYEN; loop++ )
    if ( pClub->doyen[loop] ) cDoyen++;

  for ( cMember = loop = 0; loop < MAX_CLUB_MEMBER; loop++ )
    if ( pClub->member[loop] && pClub->verify[loop] )
      cMember++;

  for ( cFollower = loop = 0; loop < MAX_CLUB_MEMBER; loop++ )
    if ( pClub->member[loop] && !pClub->verify[loop] )
      cFollower++;

  switch( type )
  {
  default:
    mudlog( LOG_DEBUG, "count_club_member: 錯誤的職等 %d.", type );
    break;

  /* 幫主 */
  case CLUB_MASTER:
    RETURN( cMaster );

  /* 副幫主 */
  case CLUB_VICEMASTER:
    RETURN( cVicemaster );

  /* 長老 */
  case CLUB_DOYEN:
    RETURN( cDoyen );

  /* 幫眾 */
  case CLUB_MEMBER:
    RETURN( cMember );

  /* 弟子 */
  case CLUB_FOLLOWER:
    RETURN( cFollower );

  /* 幫內所有的人 */
  case CLUB_ALL:
    RETURN( cMaster + cVicemaster + cDoyen + cMember + cFollower);
  }

  RETURN( 0 );
}

/* 測試幫派中某職位是否已經額滿 */
bool clubmember_full( CLUB_DATA * pClub, int type )
{
  int loop;

  PUSH_FUNCTION( "clubmember_full" );

  if ( !pClub ) RETURN( TRUE );

  switch( type )
  {
  default:
    mudlog( LOG_DEBUG, "clubmember_full: 錯誤的職等 %d.", type );
    break;

  /* 幫主 */
  case CLUB_MASTER:
    RETURN( pClub->master != NULL );

  /* 副幫主 */
  case CLUB_VICEMASTER:
    RETURN( pClub->vicemaster != NULL );

  /* 長老 */
  case CLUB_DOYEN:

    for ( loop = 0; loop < MAX_CLUB_DOYEN; loop++ )
      if ( !pClub->doyen[loop] ) RETURN( FALSE );
    break;

  /* 幫眾或弟子 */
  case CLUB_MEMBER:
  case CLUB_FOLLOWER:

    for ( loop = 0; loop < MAX_CLUB_MEMBER; loop++ )
      if ( !pClub->member[loop] ) RETURN( FALSE );
    break;
  }

  RETURN( TRUE );
}

/* 是否在線上 */
CHAR_DATA * is_online( const char * name )
{
  DESCRIPTOR_DATA * man;

  PUSH_FUNCTION( "is_online" );

  if ( name && *name )
  {
    for ( man = descriptor_list; man; man = man->next )
    {
      if ( verify_desc( man )
        && man->connected == CON_PLAYING
        && man->character
        && man->character->name
        && !str_cmp( name, man->character->name ) ) RETURN( man->character );
    }
  }

  RETURN( NULL );
}

/* 某人的幫派名稱 */
char * club_name( CHAR_DATA * ch )
{
  CLUB_DATA * pClub;

  PUSH_FUNCTION( "club_name" );
  if ( !ch ) RETURN( "未知" );
  if ( IS_NPC( ch ) || !( pClub = ch->club ) ) RETURN( "沒有幫派" );
  RETURN( pClub->cname );
}

/* 玩家進入遊戲時取得其幫派資料 */
void char_attach_club( CHAR_DATA * ch )
{
  PUSH_FUNCTION( "char_attach_club" );

  if ( !ch || IS_NPC( ch ) ) RETURN_NULL();
  ch->club = char_club( ch );

  RETURN_NULL();
}

/* 權力等級 */
bool right_ring( char * name, CLUB_DATA * pClub, int right )
{
  PUSH_FUNCTION( "right_ring" );

  if ( !name || *name == '\x0' || !pClub ) RETURN( FALSE );

  switch( right )
  {
  default:
    RETURN( FALSE );

  case CLUB_MASTER:
  case CLUB_VICEMASTER:
  case CLUB_DOYEN:
  case CLUB_MEMBER:
  case CLUB_FOLLOWER:
    break;
  }

  RETURN ( ( name_in_club( name, pClub ) >= right ) );
}

/* 是否同一個幫派 */
bool is_same_club( CHAR_DATA * ch, CHAR_DATA * victim )
{
  PUSH_FUNCTION( "is_same_club" );

  if ( !ch
     || !victim
     || IS_NPC( ch )
     || IS_NPC( victim )
     || !ch->club
     || !victim->club ) RETURN( FALSE );

  RETURN( ch->club == victim->club );
}

/* 解散幫派 */
bool fordo_club( CLUB_DATA * pClub )
{
  CLUB_DATA * prev;
  CHAR_DATA * victim;
  int         loop;

  PUSH_FUNCTION( "fordo_club" );

  if ( !pClub ) RETURN( FALSE );

  if ( pClub == club_list )
  {
    club_list = pClub->next;
  }

  else
  {
    for ( prev = club_list; prev; prev = prev->next )
    {
      if ( prev->next == pClub )
      {
        prev->next = pClub->next;
        break;
      }
    }

    if ( !prev )
    {
      mudlog( LOG_DEBUG , "fordo_club: 並未發現到欲銷毀的幫派." );
      RETURN( FALSE );
    }
  }

  free_string( pClub->name  );
  free_string( pClub->cname );

  if ( ( victim = is_online( pClub->master ) ) ) victim->club = NULL;
  free_string( pClub->master );

  if ( ( victim = is_online( pClub->vicemaster ) ) ) victim->club = NULL;
  free_string( pClub->vicemaster );

  for ( loop = 0; loop < MAX_CLUB_DOYEN; loop++ )
  {
    if ( ( victim = is_online( pClub->doyen[loop] ) ) ) victim->club = NULL;
    free_string( pClub->doyen[loop] );
  }

  for ( loop = 0; loop < MAX_CLUB_MEMBER; loop++ )
  {
    if ( ( victim = is_online( pClub->member[loop] ) ) ) victim->club = NULL;
    free_string( pClub->member[loop] );
  }

  free_struct( pClub, STRUCT_CLUB_DATA );
  RETURN( TRUE );
}

/* 幫派數目 */
size_t club_count( void )
{
  CLUB_DATA * pClub;
  size_t      count;

  PUSH_FUNCTION( "club_count" );
  for ( count = 0, pClub = club_list; pClub; pClub = pClub->next ) count++;
  RETURN( count );
}

/* 傳回幫派傳送點 */
ROOM_INDEX_DATA * club_recall( CHAR_DATA * ch )
{
  PUSH_FUNCTION( "club_recall" );

  if ( ch
    && !IS_NPC( ch )
    && ch->club
    && name_in_club( ch->name, ch->club ) > CLUB_FOLLOWER )
    RETURN( ch->club->location );

  RETURN( NULL );
}

/* 建立幫派聚集點 */
ROOM_INDEX_DATA * club_location( CLUB_DATA * pClub )
{
  ROOM_INDEX_DATA * pRoom;
  EXIT_DATA       * pExit;
  char              buf[MAX_STRING_LENGTH];
  int               iHash;
  int               loop;

  PUSH_FUNCTION( "club_location" );

  if ( !pClub || pClub->status != CLUB_STATUS_UNIONIZE ) RETURN( NULL );

  /* 建立起房間 */
  set_room_default( pRoom = alloc_struct( STRUCT_ROOM_INDEX_DATA ) );

  pRoom->people          = NULL;
  pRoom->contents        = NULL;
  pRoom->extra_descr     = NULL;
  pRoom->club            = pClub;
  pRoom->area            = DefaultArea;

  sprintf( buf, "%s \e[0m(%s)幫派聚集點\e[0m", pClub->cname, pClub->name );
  pRoom->name            = str_dup( buf );

  sprintf( buf,
    "這裡是 %s\e[0m (%s)幫的幫派聚集點﹐你看到許\多人來來去去。\n\r"
    "後面有一只大箱子﹐裡面存放著各式各樣的裝備﹐身為幫眾的你可以使用\n\r"
    "\e[1;32mstore\e[0m 這個指令來存取﹗\n\r\n\r"
    , pClub->cname, pClub->name );

  pRoom->description     = str_dup( buf );

  pRoom->filename        = str_dup( "系統建立" );
  pRoom->vnum            = club_room++;
  pRoom->Safe            = TRUE;
  pRoom->NoRecall        = TRUE;
  pRoom->NoWhere         = TRUE;
  pRoom->NoQuit          = TRUE;
  pRoom->NoMob           = TRUE;
  pRoom->Club            = TRUE;
  pRoom->NoLink          = TRUE;
  pRoom->light           = 0;
  pRoom->sector          = DefaultSector;
  iHash                  = pRoom->vnum % MAX_KEY_HASH;
  pRoom->next            = room_index_hash[iHash];
  room_index_hash[iHash] = pRoom;
  pClub->location        = pRoom;

  top_room++;
  for ( loop = 0; loop < DIR_MAX; loop++ ) pRoom->exit[loop] = NULL;

  /* 建立起向下的出口 */
  set_exit_default( pExit = alloc_struct( STRUCT_EXIT_DATA ) );
  pExit->description    = str_dup( "" );
  pExit->keyword        = str_dup( "" );
  pExit->closed         = FALSE;
  pExit->pick           = FALSE;
  pExit->isdoor         = FALSE;
  pExit->locked         = FALSE;
  pExit->key            = 0;
  pExit->vnum           = RoomRecall->vnum;
  pRoom->exit[ DIR_DOWN ] = pExit;
  pExit->message        = str_dup( "你走出了你的幫派聚集點﹗" );

  RETURN( pRoom );
}

/* 幫會信件 */
size_t club_notes( CHAR_DATA * ch )
{
  NOTE_DATA * pNote;
  CLUB_DATA * pClub;
  size_t      count;

  PUSH_FUNCTION( "club_notes" );

  if ( !ch || !( pClub = ch->club ) ) RETURN( 0 );

  for ( count = 0, pNote = note_list; pNote; pNote = pNote->next )
    if ( is_note_to_club( pClub, pNote ) ) count++;

  RETURN( count );
}

/* 幫會未讀信件 */
size_t club_unread( CHAR_DATA * ch )
{
  NOTE_DATA * pNote;
  CLUB_DATA * pClub;
  size_t      count;

  PUSH_FUNCTION( "club_notes" );

  if ( !ch || !( pClub = ch->club ) ) RETURN( 0 );

  for ( count = 0, pNote = note_list; pNote; pNote = pNote->next )
    if ( is_note_to_club( pClub, pNote )
      && pNote->date_stamp > ch->last_clubnote ) count++;

  RETURN( count );
}

/* 看看這封信到底是不是這個幫派的信件 */
bool is_note_to_club( CLUB_DATA * pClub, NOTE_DATA * pNote )
{
  PUSH_FUNCTION( "is_note_to_club" );

  if ( pNote
    && pClub
    && pClub->name
    && pNote->to_list[0] == '@'
    && pNote->to_list[1] )
    RETURN( !str_cmp( pClub->name, ( pNote->to_list ) + 1 ) );

  RETURN( FALSE );
}

/* 設定幫派資料 */
bool set_club( CLUB_DATA * pClub, int type, char * pString, int iNumber )
{
  PUSH_FUNCTION( "set_club" );

  /* 避免錯誤 */
  if ( !pClub ) RETURN( FALSE );

  switch ( type )
  {
  default:
    mudlog( LOG_DEBUG, "set_club: 錯誤的選項 %d.", type );
    break;

  /* 更改幫派英文名稱 */
  case CLUB_NAME:

    /* 若是空字串則是錯誤 */
    if ( !pString || *pString == '\x0' ) break;

    filter_ansi( pString );
    smash_tilde( pString );
    free_string( pClub->name );
    pClub->name = str_dup( pString );
    RETURN( TRUE );

  /* 更改幫派中文名稱 */
  case CLUB_CNAME:

    /* 若是空字串則是錯誤 */
    if ( !pString || *pString == '\x0' ) break;

    filter_ansi( pString );
    smash_tilde( pString );
    free_string( pClub->cname );
    pClub->cname = str_dup( pString );
    RETURN( TRUE );

  /* 更改幫派金錢 */
  case CLUB_MONEY:

    if ( iNumber >= 0 )
    {
      pClub->money = iNumber;
      RETURN( TRUE );
    }

    mudlog( LOG_DEBUG, "set_club: 數目不合理 %d.", iNumber );
    break;

  /* 更改幫派狀態 */
  case CLUB_STATUS:

    if ( iNumber == CLUB_STATUS_COUNTERSIGN
      || iNumber == CLUB_STATUS_UNIONIZE )
    {
      pClub->status = iNumber;
      RETURN( TRUE );
    }

    mudlog( LOG_DEBUG, "set_club: 錯誤的狀態 %d.", iNumber );
    break;

  /* 更改幫派時間 */
  case CLUB_TIMER:

    if ( iNumber >= 0 )
    {
      pClub->timer = iNumber;
      RETURN( TRUE );
    }

    mudlog( LOG_DEBUG, "set_club: 時間不合理 %d.", iNumber );
    break;
  }

  RETURN( FALSE );
}

/* 得知幫派中的職位 */
char * is_club_class( CHAR_DATA * ch )
{
  CLUB_DATA * pClub;

  PUSH_FUNCTION( "is_club_class" );

  if ( !ch ) RETURN( "未知" );
  if ( IS_NPC( ch ) || !( pClub = ch->club ) ) RETURN( "遊民" );

  if ( ch && pClub )
  {
    switch( name_in_club( ch->name, pClub ) )
    {
    default:              break;
    case CLUB_MASTER:     RETURN( "幫主"   );
    case CLUB_VICEMASTER: RETURN( "副幫主" );
    case CLUB_DOYEN:      RETURN( "長老"   );
    case CLUB_MEMBER:     RETURN( "幫眾"   );
    case CLUB_FOLLOWER:   RETURN( "弟子"   );
    }
  }

  RETURN( "未知" );
}

/* 得知幫派中的職位 */
char * club_class( CHAR_DATA * ch, CLUB_DATA * pClub )
{
  PUSH_FUNCTION( "club_class" );

  if ( ch && pClub )
  {
    switch( name_in_club( ch->name, pClub ) )
    {
    default:              break;
    case CLUB_MASTER:     RETURN( "幫主"   );
    case CLUB_VICEMASTER: RETURN( "副幫主" );
    case CLUB_DOYEN:      RETURN( "長老"   );
    case CLUB_MEMBER:     RETURN( "幫眾"   );
    case CLUB_FOLLOWER:   RETURN( "弟子"   );
    }
  }

  RETURN( "未知" );
}

bool name_from_club( const char * name, bool fPrint )
{
  CLUB_DATA * pClub;
  CLUB_DATA * temp;
  CLUB_DATA * prev;
  CHAR_DATA * victim;
  CHAR_DATA * member;
  char        buf[MAX_STRING_LENGTH];
  int         loop;

  PUSH_FUNCTION( "name_from_club" );

  if ( !name || !*name ) RETURN( FALSE );

  victim = is_online( name );

  for ( pClub = club_list; pClub; pClub = pClub->next )
  {
    if ( pClub->master && !str_cmp( pClub->master, name ) )
    {
      if ( fPrint )
      {
        sprintf( buf, "隨著幫主%s的消失﹐%s\e[0m(%s)幫派也走入了歷史﹐"
          "再也沒有這個幫派了﹗"
          , pClub->master, pClub->cname, pClub->name );

        talk_channel_2( buf, CHANNEL_BULLETIN, "公告" );
      }

      for ( member = char_list; member; member = member->next )
      {
        if ( verify_char( member ) && member->club == pClub )
          member->club = NULL;
      }

      prev = NULL;
      for ( temp = club_list; temp; prev = temp, temp = temp->next )
      {
        if ( temp == pClub )
        {
          if ( !prev ) club_list  = club_list->next;
          else         prev->next = pClub->next;

          free_string( pClub->name       );
          free_string( pClub->cname      );
          free_string( pClub->master     );
          free_string( pClub->vicemaster );

          for ( loop = 0; loop < MAX_CLUB_DOYEN; loop++ )
            free_string( pClub->doyen[loop] );

          for ( loop = 0; loop < MAX_CLUB_MEMBER; loop++ )
            free_string( pClub->member[loop] );

          free_struct( pClub , STRUCT_CLUB_DATA );
          break;
        }
      }

      RETURN( TRUE );
    }

    if ( pClub->vicemaster && !str_cmp( pClub->vicemaster, name ) )
    {
      if ( fPrint )
      {
        sprintf( buf, "本幫副幫主%s已煙飛灰滅﹐得另選賢能了﹗\n\r"
          , pClub->vicemaster );

        send_to_club( buf, pClub );
      }

      free_string( pClub->vicemaster );
      pClub->vicemaster = NULL;
      if ( victim ) victim->club = NULL;

      RETURN( TRUE );
    }

    for ( loop = 0; loop < MAX_CLUB_DOYEN; loop++ )
    {
      if ( pClub->doyen[loop] && !str_cmp( pClub->doyen[loop], name ) )
      {
        if ( fPrint )
        {
          sprintf( buf, "幫內長老%s不幸離開人世﹐他已成歷史憑供後輩敬悼﹗\n\r"
            , name );

          send_to_club( buf, pClub );
        }

        free_string( pClub->doyen[loop] );
        pClub->doyen[loop] = NULL;
        if ( victim ) victim->club = NULL;

        RETURN( TRUE );
      }
    }

    for ( loop = 0; loop < MAX_CLUB_MEMBER; loop++ )
    {
      if ( pClub->member[loop] && !str_cmp( pClub->member[loop], name ) )
      {
        if ( fPrint )
        {
          sprintf( buf, "%s%s已離開我們長眠於下﹐唉﹐痛失英才﹗\n\r"
            , pClub->member[loop] ? "幫眾" : "弟子", name );

          send_to_club( buf, pClub );
        }

        free_string( pClub->member[loop] );
        pClub->member[loop] = NULL;
        pClub->verify[loop] = FALSE;
        if ( victim ) victim->club = NULL;

        RETURN( TRUE );
      }
    }
  }

  RETURN( FALSE );
}

void club_sort( CLUB_DATA * pClub )
{
  int          loop_1;
  int          loop_2;
  int          level;
  int          doyen[MAX_CLUB_DOYEN];
  int          member[MAX_CLUB_MEMBER];
  char       * temp;
  BASIC_DATA * pBasic;

  PUSH_FUNCTION( "club_sort" );

  for ( loop_1 = 0; loop_1 < MAX_CLUB_DOYEN; loop_1++ )
  {
    if ( pClub->doyen[loop_1]
      && *( pClub->doyen[loop_1] )
      && ( pBasic = name2basic( pClub->doyen[loop_1] ) ) )
    {
      doyen[loop_1] = pBasic->level;
      free_basic( pBasic );
    }
    else
    {
      doyen[loop_1] = -MAX_LEVEL;
    }
  }

  for ( loop_1 = 0; loop_1 < MAX_CLUB_DOYEN - 1; loop_1++ )
  {
    for ( loop_2 = loop_1 + 1; loop_2 < MAX_CLUB_DOYEN; loop_2++ )
    {
      if ( doyen[loop_1] < doyen[loop_2] )
      {
        temp                 = pClub->doyen[loop_2];
        pClub->doyen[loop_2] = pClub->doyen[loop_1];
        pClub->doyen[loop_1] = temp;

        level         = doyen[loop_2];
        doyen[loop_2] = doyen[loop_1];
        doyen[loop_1] = level;
      }
    }
  }

  for ( loop_1 = 0; loop_1 < MAX_CLUB_MEMBER; loop_1++ )
  {
    if ( pClub->member[loop_1]
      && *( pClub->member[loop_1] )
      && ( pBasic = name2basic( pClub->member[loop_1] ) ) )
    {
      member[loop_1] = pBasic->level
                     + ( pClub->verify[loop_1] ? MAX_LEVEL * 2 : 0 );

      free_basic( pBasic );
    }
    else
    {
      member[loop_1] = -MAX_LEVEL * 4;
    }
  }

  for ( loop_1 = 0; loop_1 < MAX_CLUB_MEMBER - 1; loop_1++ )
  {
    for ( loop_2 = loop_1 + 1; loop_2 < MAX_CLUB_MEMBER; loop_2++ )
    {
      if ( member[loop_1] < member[loop_2] )
      {
        temp                  = pClub->member[loop_2];
        pClub->member[loop_2] = pClub->member[loop_1];
        pClub->member[loop_1] = temp;

        level                 = pClub->verify[loop_2];
        pClub->verify[loop_2] = pClub->verify[loop_1];
        pClub->verify[loop_1] = level;

        level          = member[loop_2];
        member[loop_2] = member[loop_1];
        member[loop_1] = level;
      }
    }
  }

  RETURN_NULL();
}

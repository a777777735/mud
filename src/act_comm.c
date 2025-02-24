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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "merc.h"

/* 區域函數 */
bool    is_note_to      args( ( CHAR_DATA * , NOTE_DATA * ) );
void    note_attach     args( ( CHAR_DATA * ) );
void    note_remove     args( ( CHAR_DATA * , NOTE_DATA * ) );
void    send_note_to_char args( ( CHAR_DATA *, NOTE_DATA * ) );
void    talk_channel    args( ( CHAR_DATA * , char * , int , const char * ) );
void    note_delete     args( ( NOTE_DATA * ) );
char *  transfer_messages args( ( char * ) );
void    show_joke       args(( CHAR_DATA *, JOKE_DATA * ) );
void    refresh_donate  args( ( void ) );
bool    can_donate      args( ( CHAR_DATA * ch, bool ) );

extern  int     name_in_club            args( ( char *, CLUB_DATA * ) );

#define MAX_SOCIAL_MESSAGE      256

struct   social_token
{
  char * token;
  char * message;
};

const struct social_token token [] =
{
  { "?"  , "疑問地"     },
  { ":)" , "愉快地"     },
  { "-)" , "興高采烈地" },
  { ":(" , "傷心地"     },
  { "-(" , "悲傷地"     },
  { "?!" , "憤怒地"     },
  { ":!" , "興奮地"     },
  { ":>" , "狡猾地"     },
  { "->" , "陰險地"     },
  { ";>" , "邪惡地"     },
  { ":p" , "開玩笑地"   },
  { ":P" , "開玩笑地"   },
  { ":b" , "開玩笑地"   },
  { ":~" , "留著眼淚\"  },
  { ":O" , "驚訝地"     },
  { ":*" , "無知地"     },
  { ":Q" , "調皮地"     },
  { ""   , ""           }
};

bool is_note_to( CHAR_DATA * ch, NOTE_DATA * pNote )
{
  PUSH_FUNCTION( "is_note_to" );

  if ( !ch || !pNote )
  {
    mudlog( LOG_DEBUG, "is_note_to: 來源是空的." );
    RETURN( FALSE );
  }

  if ( !ch->name || !pNote->sender || !pNote->to_list )
  {
    mudlog( LOG_DEBUG, "is_note_to: 比較字串是空的." );
    RETURN( FALSE );
  }

  /* 普通信件 */
  if ( ( !str_cmp( ch->name , pNote->sender ) )
    || ( is_name( "all" , pNote->to_list    ) )
    || ( is_name( ch->name , pNote->to_list ) )
    || ( IS_IMMORTAL( ch ) && ( is_name( "immortal" , pNote->to_list ) ) ) )
    RETURN( TRUE );

  RETURN( FALSE );
}

void note_attach( CHAR_DATA * ch )
{
  NOTE_DATA * pNote;

  PUSH_FUNCTION( "note_attach" );

  if ( !ch || !ch->name )
  {
    mudlog( LOG_DEBUG, "note_attach: 來源有問題." );
    RETURN_NULL();
  }

  if ( IS_NPC( ch ) || ch->pnote ) RETURN_NULL();

  pNote           = alloc_struct( STRUCT_NOTE_DATA );
  pNote->next     = NULL;
  pNote->sender   = str_dup( ch->name );
  pNote->date     = str_dup( "" );
  pNote->to_list  = str_dup( "" );
  pNote->subject  = str_dup( "" );
  pNote->text     = str_dup( "" );
  pNote->filename = str_dup( "" );
  ch->pnote       = pNote;
  RETURN_NULL();
}

void note_remove( CHAR_DATA * ch, NOTE_DATA * pNote )
{
  char        to_new[MAX_INPUT_LENGTH];
  char        to_one[MAX_INPUT_LENGTH];
  NOTE_DATA * prev;
  char      * to_list;

  PUSH_FUNCTION( "note_remove" );

  if ( !ch || !pNote || !pNote->to_list )
  {
    mudlog( LOG_DEBUG, "note_remove: 來源有問題." );
    RETURN_NULL();
  }

  /* 建立新的 to_list */
  to_new[0] = '\x0';
  to_list   = pNote->to_list;

  while ( *to_list )
  {
    to_list = one_argument( to_list, to_one );

    if ( to_one[0] && str_cmp( ch->name, to_one ) )
    {
      if ( str_len( to_new ) + str_len( to_new ) > sizeof( to_new ) - 2 )
      {
        mudlog( LOG_DEBUG, "note_remove: 超出範圍." );
        RETURN_NULL();
      }

      str_cat( to_new, " " );
      str_cat( to_new, to_one );
    }
  }

  if ( str_cmp( ch->name, pNote->sender ) && to_new[0] )
  {
    free_string( pNote->to_list );
    pNote->to_list = str_dup( to_new + 1 );
    RETURN_NULL();
  }

  if ( pNote == note_list )
  {
    note_list = pNote->next;
  }
  else
  {
    for ( prev = note_list; prev; prev = prev->next )
      if ( prev->next == pNote ) break;

    if ( !prev )
    {
      mudlog( LOG_DEBUG , "Note_remove: 沒發現到欲刪除的結構." );
      RETURN_NULL();
    }
    prev->next = pNote->next;
  }

  if ( unlink( pNote->filename ) != 0 )
    mudlog( LOG_DEBUG, "remove_note: 無法移除信件 %s.", pNote->filename );

  free_string( pNote->text     );
  free_string( pNote->subject  );
  free_string( pNote->to_list  );
  free_string( pNote->date     );
  free_string( pNote->sender   );
  free_string( pNote->filename );

  /* 釋放記憶體 */
  free_struct( pNote , STRUCT_NOTE_DATA );
  RETURN_NULL();
}

void send_note_to_char( CHAR_DATA * ch, NOTE_DATA * pNote )
{
  PUSH_FUNCTION( "send_note_to_char" );

  if ( !ch || !pNote )
  {
    mudlog( LOG_DEBUG, "send_note_to_char: 來源不正確." );
    RETURN_NULL();
  }

  clear_buffer();

  send_to_buffer( "寄信者﹕%s\n\r標  題﹕%s\n\r日  期﹕%s\n\r"
    , pNote->sender, pNote->subject, pNote->date );

  if ( pNote->to_list && *pNote->to_list == '@' )
  {
    send_to_buffer( "幫派信﹕%s\n\r", pNote->to_list + 1 );
  }
  else
  {
    send_to_buffer( "收信人﹕%s\n\r", pNote->to_list );
  }

  send_to_buffer( "保  留﹕%s\n\r", YESNO( pNote->mark ) );

  if ( IS_IMMORTAL( ch ) ) send_to_buffer( "檔  名﹕%s\n\r"
    , pNote->filename );

  send_to_buffer( "內  容﹕\n\r%s%s%s\e[0m"
    , VERTICAL_LINE, pNote->text, VERTICAL_LINE );

  print_buffer( ch );
  RETURN_NULL();
}

bool write_note( NOTE_DATA * pNote )
{
  char   filename[MAX_FILE_LENGTH];
  FILE * pFile;

  PUSH_FUNCTION( "write_note" );

  if ( !pNote )
  {
    mudlog( LOG_DEBUG, "write_note: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( pNote->filename && *pNote->filename )
  {
    str_cpy( filename, pNote->filename );
  }
  else
  {
    sprintf( filename, "%s/%d.%s", note_dir, ( int ) current_time, note_ext );
    pNote->filename = str_dup( filename );
  }

  if ( ( pFile = FOPEN( filename, "w" ) ) )
  {
    fprintf( pFile, "Sender  %s~\n"     , pNote->sender           );
    fprintf( pFile, "Mark    %d\n"      , pNote->mark             );
    fprintf( pFile, "Date    %s~\n"     , pNote->date             );
    fprintf( pFile, "Stamp   %d\n"      ,  (int) pNote->date_stamp );
    fprintf( pFile, "To      %s~\n"     , pNote->to_list          );
    fprintf( pFile, "Subject %s~\n"     , pNote->subject          );
    fprintf( pFile, "Text\n%s~\n\nEnd\n", pNote->text             );

    FCLOSE( pFile );

    /* 改變檔案存取模式 */
    set_file_mode( filename );
  }
  else
  {
    mudlog( LOG_DEBUG, "write_note: 無法開啟檔案 %s.", filename );
    RETURN( FALSE );
  }
  RETURN( TRUE );
}

FUNCTION( do_note )
{
  char        buf[MAX_STRING_LENGTH];
  char        arg[MAX_INPUT_LENGTH];
  NOTE_DATA * pnote;
  CHAR_DATA * man;
  int         vnum;
  int         anum;
  char      * strtime;
  bool        found;

  PUSH_FUNCTION( "do_note" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  argument = one_argument( argument, arg );
  smash_tilde( argument );

  if ( !arg[0] )
  {
    do_note( ch, "read" );
    RETURN_NULL();
  }

  if ( !str_cmp( arg , "new_note" ) )
  {
    for ( pnote = note_list; pnote; pnote = pnote->next )
    {
      if ( is_note_to( ch , pnote )
        && str_cmp( ch->name , pnote->sender )
        && ch->last_note < pnote->date_stamp )
      {
        send_to_char( "\e[1;32m你收到一封新信﹐請用 \e[1;32mnote\e[0m 來閱\讀。"
                      "\e[0m\a\n\r" , ch );
        RETURN_NULL();
      }
    }
  }

  if ( !str_cmp( arg, "list" ) )
  {
    clear_buffer();

    for ( vnum = 0, pnote = note_list; pnote; pnote = pnote->next )
    {
      if ( is_note_to( ch, pnote ) )
      {
        send_to_buffer( "[%3d%s] %s﹕%s\e[0m\n\r"
          , vnum++
          , ( pnote->date_stamp > ch->last_note
            && str_cmp( pnote->sender, ch->name ) )
              ? " \e[1;32mN\e[0m " : "   "
          , pnote->sender
          , pnote->subject );
      }

      if ( buffer_full() ) break;
    }

    if ( vnum == 0 ) send_to_buffer( "系統內目前沒有任何給你的信件。\n\r" );
    print_buffer( ch );

    RETURN_NULL();
  }

  if ( !str_cmp( arg, "read" ) )
  {
    if ( !str_cmp( argument, "all" ) )
    {
      found = TRUE;
      anum  = 0;
    }

    else if ( !argument[0] || !str_prefix( argument, "next" ) )
    {
      for ( pnote = note_list; pnote; pnote = pnote->next )
      {
        if ( is_note_to( ch, pnote )
          && str_cmp( ch->name, pnote->sender )
          && ch->last_note < pnote->date_stamp )
        {
          send_note_to_char( ch, pnote );
          ch->last_note = UMAX( ch->last_note, pnote->date_stamp );
          RETURN_NULL();
        }
      }

      send_to_char( "你沒有未讀的訊息。\n\r", ch );
      RETURN_NULL();
    }

    else if ( is_number( argument ) )
    {
      found = FALSE;
      anum  = atoi( argument );
    }
    else
    {
      send_to_char( "你想讀哪篇訊息﹖\n\r", ch );
      RETURN_NULL();
    }

    for ( vnum = 0, pnote = note_list; pnote; pnote = pnote->next )
    {
      if ( is_note_to( ch, pnote ) && ( vnum++ == anum || found ) )
      {
        send_note_to_char( ch, pnote );
        ch->last_note = UMAX( ch->last_note, pnote->date_stamp );
        RETURN_NULL();
      }
    }

    send_to_char( "對不起, 沒那封信件。\n\r", ch );
    RETURN_NULL();
  }

  if ( !str_cmp( arg, "edit" ) )
  {
    if ( !ch->desc ) RETURN_NULL();

    note_attach( ch );

    ch->desc->edit_mode = EDIT_NOTE;
    show_editor( ch );

    free_string( ch->stack );
    ch->stack = str_dup( "" );
    RETURN_NULL();
  }

  if ( !str_cmp( arg, "subject" ) )
  {
    if ( !argument || !*argument )
    {
      send_to_char( "你必須註明標題。\n\r", ch );
      RETURN_NULL();
    }

    if ( check_chat_xname( ch, argument ) == TRUE ) RETURN_NULL();
    if ( str_len( argument ) > 40 )
    {
      send_to_char( "對不起﹐你的標題太長了﹐請縮短一點吧﹗\n\r", ch );
      RETURN_NULL();
    }

    note_attach( ch );

    ansi_transcribe( argument, buf );
    free_string( ch->pnote->subject );

    ch->pnote->subject = str_dup( buf );
    act( "你這封信的標題為﹕「$t」", ch, buf, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( !str_cmp( arg, "to" ) )
  {
    if ( !argument || !*argument )
    {
      send_to_char( "你必須註明收件人。\n\r", ch );
      RETURN_NULL();
    }

    /* 若不是寄給管理者 */
    if ( str_cmp( argument, "immortal" ) && str_cmp( argument, "all" ) )
    {
      /* 是否寄給自己 */
      if ( !str_cmp( argument, ch->name ) )
      {
        send_to_char( "你竟然寄信給自己﹐你是不是該吃藥了﹖\n\r", ch );
        RETURN_NULL();
      }

      /* 寄給幫派 */
      if ( argument[0] == '@' )
      {
        if ( argument[1] == '\x0' )
        {
          send_to_char( "你必須註明幫派的英文名稱﹗\n\r", ch );
          RETURN_NULL();
        }

        if ( clubname_lookup( argument + 1, CLUB_NAME ) == NULL )
        {
          act( "$t裡沒有這個幫派喔﹗", ch, mud_name, NULL, TO_CHAR );
          RETURN_NULL();
        }
      }

      /* 寄給個人 */
      else
      {
        if ( !is_exist( argument ) )
        {
          act( "$t裡沒有這一號人物喔﹗", ch, mud_name, NULL, TO_CHAR );
          RETURN_NULL();
        }
      }
    }

    note_attach( ch );
    free_string( ch->pnote->to_list );
    ch->pnote->to_list = str_dup( argument );

    if ( !str_cmp( argument, "all" ) )
    {
      send_to_char( "你這封信將會寄給所有的玩家﹗\n\r", ch );
    }

    else if ( !str_cmp( argument, "immortal" ) )
    {
      act( "你這封信將會寄給$t系統管理員﹗", ch, mud_name, NULL, TO_CHAR );
    }

    else if ( argument[0] == '@' )
    {
      act( "你這封信將會寄給$t幫派﹗", ch, argument + 1, NULL, TO_CHAR );
    }

    else
    {
      act( "你這封信將會寄給 $t 這位玩家﹗", ch, argument, NULL, TO_CHAR );
    }

    RETURN_NULL();
  }

  if ( !str_cmp( arg, "clear" ) )
  {
    if ( !ch->pnote )
    {
      send_to_char( "你行程裡﹐本來就沒有信件﹐所以無法清除﹗\n\r", ch );
      RETURN_NULL();
    }

    free_string( ch->pnote->text     );
    free_string( ch->pnote->subject  );
    free_string( ch->pnote->to_list  );
    free_string( ch->pnote->date     );
    free_string( ch->pnote->sender   );
    free_string( ch->pnote->filename );

    /* 釋放計憶體 */
    free_struct( ch->pnote, STRUCT_NOTE_DATA );
    ch->pnote = NULL;

    send_to_char( "完成清除信件資料。\n\r", ch );
    RETURN_NULL();
  }

  if ( !str_cmp( arg, "show" ) )
  {
    if ( !ch->pnote )
    {
      send_to_char( "你的行程裡沒有任何的信件。\n\r", ch );
      RETURN_NULL();
    }

    send_note_to_char( ch, ch->pnote );
    RETURN_NULL();
  }

  if ( !str_cmp( arg, "post" ) || !str_prefix( arg, "send" ) )
  {
    if ( !ch->pnote )
    {
      send_to_char( "你的行程裡並沒有任何的信件。\n\r", ch );
      RETURN_NULL();
    }

    if ( !ch->pnote->to_list || !*ch->pnote->to_list )
    {
      send_to_char( "你必須提供收信人的名字﹐\e[1;32mall\e[0m 是寄給所有人。"
                    "\e[1;32mimmortal\e[0m 是寄給管理者。\n\r" , ch );
      RETURN_NULL();
    }

    if ( !ch->pnote->subject || !*ch->pnote->subject )
    {
      send_to_char( "你需要給訊息一個標題。\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->level < NoteLevel )
    {
      act( "你得等到等級$i時才能寫信﹗", ch, &NoteLevel, NULL, TO_CHAR );
      RETURN_NULL();
    }

    ch->pnote->next             = NULL;
    strtime                     = ctime( &current_time );
    strtime[str_len(strtime)-1] = '\x0';
    ch->pnote->date             = str_dup( strtime );
    ch->pnote->date_stamp       = current_time;

    if ( !note_list )
    {
      note_list = ch->pnote;
    }
    else
    {
      for ( pnote = note_list; pnote->next; pnote = pnote->next );
      pnote->next = ch->pnote;
    }

    pnote     = ch->pnote;
    ch->pnote = NULL;

    if ( !write_note( pnote ) )
    {
      send_to_char( "對不起﹐系統寄信失敗﹗\n\r", ch );
      RETURN_NULL();
    }

    send_to_char( "完成寄信手續。\n\r", ch );

    /* 立刻送訊息給收信人 */
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man ) ) continue;

      if ( !IS_NPC( man ) && ch != man
        && ( !str_cmp( pnote->to_list , "all" )
          || !str_cmp( man->name , pnote->to_list ) ) )
         do_note( man, "new_note");
    }

    RETURN_NULL();
  }

  if ( !str_cmp( arg, "mark" ) && IS_IMMORTAL( ch ) )
  {
    if ( !argument || !*argument || !is_number( argument ) )
    {
      send_to_char( "你要設定哪封信的保留狀態呢﹖\n\r", ch );
      RETURN_NULL();
    }

    anum = atoi( argument );
    for ( vnum = 0, pnote = note_list; pnote; pnote = pnote->next )
    {
      if ( vnum++ == anum )
      {
        if ( pnote->mark )
        {
          act( "你把$t這封信解除保留﹗", ch, pnote->subject, NULL, TO_CHAR );
          pnote->mark = FALSE;
        }

        else
        {
          pnote->mark = TRUE;
          act( "你把$t這封信設定成保留﹗", ch, pnote->subject, NULL, TO_CHAR );
        }

        write_note( pnote );
        RETURN_NULL();
      }
    }

    send_to_char( "對不起﹐沒有這封信。\n\r", ch );
    RETURN_NULL();
  }

  if ( !str_cmp( arg, "remove" ) )
  {
    if ( !argument || !*argument || !is_number( argument ) )
    {
      send_to_char( "對不起﹐你要刪除哪封信﹖\n\r", ch );
      RETURN_NULL();
    }

    anum = atoi( argument );
    for ( vnum = 0, pnote = note_list; pnote; pnote = pnote->next )
    {
      if ( is_note_to( ch, pnote ) && vnum++ == anum )
      {
        act( "你刪除了$t這封信。", ch, pnote->subject, NULL, TO_CHAR );
        note_remove( ch, pnote );
        RETURN_NULL();
      }
    }

    send_to_char( "對不起﹐沒有這封信。\n\r", ch );
    RETURN_NULL();
  }

  send_to_char( "請輸入 note /? 來查詢寄信的使用方法。\n\r", ch );
  RETURN_NULL();
}

void notes_update( void )
{
  int         count;
  NOTE_DATA * pNote;
  NOTE_DATA * zNote;
  NOTE_DATA * prev;

  PUSH_FUNCTION( "notes_update" );

  for ( count = 0, pNote = note_list; pNote; pNote = pNote->next )
    if ( !pNote->mark ) count++;

  if ( ( count -= max_notes ) <= 0 ) RETURN_NULL();

  mudlog( LOG_INFO, "嘗試刪除過量的信件( %d 封).", count );

  for ( pNote = note_list; pNote && count > 0; pNote = zNote )
  {
    zNote = pNote->next;

    if ( pNote->mark ) continue;

    if ( pNote == note_list )
    {
      note_list = pNote->next;
    }

    else
    {
      for ( prev = note_list; prev; prev = prev->next )
        if ( prev->next == pNote ) break;

      if ( !prev )
      {
        mudlog( LOG_DEBUG , "Notes_update: 沒發現到欲刪除的結構." );
        continue;
      }

      prev->next = pNote->next;
    }

    if ( unlink( pNote->filename ) != 0 )
      mudlog( LOG_DEBUG, "note_update: 無法移除信件 %s.", pNote->filename );

    free_string( pNote->text     );
    free_string( pNote->subject  );
    free_string( pNote->to_list  );
    free_string( pNote->date     );
    free_string( pNote->sender   );
    free_string( pNote->filename );

    /* 釋放記憶體 */
    free_struct( pNote , STRUCT_NOTE_DATA );
  }

  RETURN_NULL();
}

void talk_channel( CHAR_DATA * ch, char * argument, int channel
                 , const char * verb )
{
  char              buf[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA * d;
  int               position;
  CHAR_DATA       * vch;
  char            * pStr;

  PUSH_FUNCTION( "talk_channel" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "talk_channel: 缺乏來源." );
    RETURN_NULL();
  }

  if ( !argument || !argument[0] ) RETURN_NULL();

  if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_SILENCE ) )
  {
    act( "你被禁言了﹐所以不能做 $t 這個動作。", ch, verb, NULL, TO_CHAR );
    RETURN_NULL();
  }

  /* 消掉空白 */
  for ( pStr = argument + str_len( argument ) - 1; pStr > argument; pStr-- )
  {
    if ( *pStr != ' ' ) break;
    *pStr = '\x0';
  }

  REMOVE_BIT( ch->deaf, channel );

  position     = ch->position;
  ch->position = POS_STANDING;

  switch ( channel )
  {
  default:
    mudlog( LOG_DEBUG, "talk_channel: 錯誤的頻道 %d.", channel );
    ch->position = position;
    RETURN_NULL();

  case CHANNEL_AUCTION:
    act( "$3【跳蚤市場】$0你$t﹕「$T」$0", ch, verb, argument, TO_CHAR );
    sprintf( buf, "$3【跳蚤市場】$n$0%s﹕「$t」$0", verb );
    break;

  case CHANNEL_CHAT:
    act( "$6【八卦頻道】你$t﹕「$3$T$6」$0", ch, verb, argument, TO_CHAR );
    sprintf( buf, "$6【八卦頻道】$n$6%s﹕「$3$t$6」$0", verb );
    break;

  case CHANNEL_IMMTALK:
    act( "$2【會議】你$t﹕「$T$2」$0", ch, verb, argument, TO_CHAR );
    sprintf( buf, "$2【會議】$n$2%s﹕「$2$t$2」$0", verb );
    break;

  case CHANNEL_MUSIC:
    act( "$6【天籟】你$t﹕「$T$6」$0", ch, verb, argument, TO_CHAR );
    sprintf( buf, "$6【天籟】$n$6%s﹕「$t$6」$0", verb );
    break;

  case CHANNEL_QUESTION:
    act( "$5【授業解惑】你$t﹕「$T$5」$0", ch, verb, argument, TO_CHAR );
    sprintf( buf, "$5【授業解惑】$n$5%s﹕「$t$5」$0", verb );
    break;

  case CHANNEL_SHOUT:
    act( "$7【哭夭】你$t﹕「$T$7」$0", ch, verb, argument, TO_CHAR );
    sprintf( buf, "$7【哭夭】$n$7%s﹕「$t$7」$0", verb );
    break;

  case CHANNEL_YELL:
    act( "$7【哭夭】你$t﹕「$T$7」$0", ch, verb, argument, TO_CHAR );
    sprintf( buf, "$7【哭夭】$n$7%s﹕「$t$7」$0", verb );
    break;

  case CHANNEL_CLASS:
    act( "$3【$0同窗$3】你$t﹕「$T$3」$0", ch, verb, argument, TO_CHAR );
    sprintf( buf, "$3【$0同窗$3】$n$3%s﹕「$t$3」$0", verb );
    break;

  case CHANNEL_CLUB:
    act( "$3【同志】你$t﹕「$T$3」$0", ch, verb, argument, TO_CHAR );
    sprintf( buf, "$3【同志】$n$3%s﹕「$t$3」$0", verb );
    break;

  case CHANNEL_GROUP:
    act( "$3【升級】你$t﹕「$T$3」$0", ch, verb, argument, TO_CHAR );
    sprintf( buf, "$3【升級】$n$3%s﹕「$t$3」$0", verb );
    break;


  case CHANNEL_SUICIDE:
    sprintf( buf, "$3【自裁】$n$3%s﹕「$t$3」$0", verb );
    break;

  case CHANNEL_RUMOR:
    act( "$3【謠言】你$t﹕「$T$3」$0", ch, verb, argument, TO_CHAR );
    sprintf( buf, "$3【謠言】某不知名人物$3%s﹕「$t$3」$0", verb );
    break;
  }

  ch->position = position;

  for ( d = descriptor_list; d; d = d->next )
  {
    if ( verify_desc( d )
      && ( vch = d->character )
      && vch->in_room
      && d->connected == CON_PLAYING
      && vch != ch
      && !IS_SET( vch->deaf, channel ) )
    {
      if ( channel == CHANNEL_CLASS && !is_same_class( vch, ch ) ) continue;
      if ( channel == CHANNEL_CLUB  && !is_same_club( vch, ch )  ) continue;
      if ( channel == CHANNEL_IMMTALK && !IS_IMMORTAL( vch )     ) continue;
      if ( channel == CHANNEL_YELL && vch->in_room->area != ch->in_room->area )
        continue;

      if ( channel == CHANNEL_GROUP && abs( vch->level - ch->level ) > 5 )
        continue;

      position  = vch->position;
      if ( channel != CHANNEL_SHOUT && channel != CHANNEL_YELL )
        vch->position = POS_STANDING;

      act( smash_act_keyword( buf, "nesNESt" ), ch, argument, vch, TO_VICT );
      vch->position = position;
    }
  }
  RETURN_NULL();
}

void talk_channel_2( char * argument, int channel, const char * verb )
{
  DESCRIPTOR_DATA * d;
  CHAR_DATA       * vch;
  char            * pStr;
  char              buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "talk_channel_2" );

  if ( !argument || !argument[0] ) RETURN_NULL();

  /* 消掉空白 */
  for ( pStr = argument + str_len( argument ) - 1; pStr > argument; pStr-- )
  {
    if ( *pStr != ' ' ) break;
    *pStr = '\x0';
  }

  switch ( channel )
  {
  default:
    mudlog( LOG_DEBUG, "talk_channel: 錯誤的頻道 %d.", channel );
    RETURN_NULL();

  case CHANNEL_WEATHER:
    sprintf( buf, "\e[1;33m【天氣】「%s\e[1;33m」\e[0m\n\r", argument );
    break;

  case CHANNEL_GAMBLE:
    sprintf( buf, "\e[1;33m【賭博公會】「%s\e[1;33m」\e[0m\n\r", argument );
    break;

  /* 快報頻道 */
  case CHANNEL_BULLETIN:

    if ( !verb || !*verb )
    {
      mudlog( LOG_DEBUG, "talk_channel_2: 來源不正確." );
      RETURN_NULL();
    }

    sprintf( buf, "\e[1;33m【%s】「%s\e[1;33m」\e[0m\n\r", verb, argument );
    break;

  case CHANNEL_PK:
    sprintf( buf, "\e[1;37m【ＰＫ】「%s\e[1;37m」\e[0m\n\r", argument );
    break;

  case CHANNEL_NOTICE:
    sprintf( buf, "\e[1;32m【系統公告】「%s\e[1;32m」\e[0m\n\r", argument );
    break;
  }

  for ( d = descriptor_list; d; d = d->next )
  {
    if ( verify_desc( d )
      && ( vch = d->character )
      && vch->in_room
      && d->connected == CON_PLAYING
      && !IS_SET( vch->deaf, channel ) )
    {
      /* 天氣頻道必須的條件 */
      if ( channel == CHANNEL_WEATHER
        && ( !IS_AWAKE( vch ) || !IS_OUTSIDE( vch ) ) )
        continue;

      send_to_char( buf, vch );
    }
  }
  RETURN_NULL();
}

char * transfer_messages( char * input )
{
  char * location;
  int    loop;
  int    junk;

  PUSH_FUNCTION( "transfer_messages" );

  if ( !input || !*input ) RETURN( NULL );

  for ( loop = 0; token[loop].token[0]; loop++ )
  {
    if ( str_len( input ) > str_len( token[loop].token )
      && str_len( token[loop].message ) < MAX_SOCIAL_MESSAGE )
    {
      location = input + str_len( input ) - str_len( token[loop].token );

      if ( !str_cmp( token[loop].token, location ) )
      {
        *location = '\x0';
        junk = str_len( input ) - 1;

        while ( *(input + junk ) == ' ' && junk > 0 )
        {
          *( input + junk ) = '\x0';
          junk--;
        }
        RETURN( token[ loop ].message );
      }
    }
  }
  RETURN( NULL );
}

FUNCTION( do_semote )
{
  ROOM_INDEX_DATA * victim_org;
  CHAR_DATA       * victim;
  CHAR_DATA       * pChar;
  SOCIAL_DATA     * pSocial;
  char              soccmd[MAX_INPUT_LENGTH];
  char              arg[MAX_INPUT_LENGTH];
  char              buf[MAX_STRING_LENGTH];
  char              buf1[MAX_STRING_LENGTH];
  char              buf2[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "do_semote" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  if ( IS_SET( ch->act, PLR_SILENCE ) )
  {
    send_to_char( "你被禁止說話了﹐跟神主商量一下吧。\n\r" , ch );
    RETURN_NULL();
  }

  if ( IS_SET( ch->deaf, CHANNEL_SEMOTE ) )
  {
    send_to_char( "你必需先打開你的動作頻道。\n\r", ch );
    RETURN_NULL();
  }

  if ( argument[0] == '\x0' )
  {
    send_to_char( "你要表示什麼動作呢﹖\n\r", ch );
    RETURN_NULL();
  }

  if ( check_repeat( ch , argument ) == TRUE )
  {
    send_to_char( "你不能做相同的動作。\n\r" , ch );
    RETURN_NULL();
  }

  argument = one_argument( argument , soccmd );

  if ( !soccmd[0] )
  {
    send_to_char( "你要做什麼社交動作呢﹖\n\r" , ch );
    RETURN_NULL();
  }

  for ( pSocial = social_first; pSocial; pSocial = pSocial->next )
  {
    if ( soccmd[0] == pSocial->name[0]
      && !str_prefix( soccmd, pSocial->name ) ) break;
  }

  /* 若不是社交動作 */
  if ( !pSocial )
  {
    ansi_transcribe( soccmd,   buf1 );
    ansi_transcribe( argument, buf2 );

    /* 檢查閒聊保留字 */
    if ( check_chat_xname( ch, buf1 ) == TRUE ) RETURN_NULL();
    if ( check_chat_xname( ch, buf2 ) == TRUE ) RETURN_NULL();

    for ( victim = char_list; victim; victim = victim->next )
    {
      if ( !verify_char( victim )
        || IS_NPC( victim )
        || !victim->in_room
        || ( IS_SET( victim->deaf, CHANNEL_SEMOTE )
          && victim->in_room != ch->in_room ) ) continue;

      print_to_char( victim, "\e[1;32m【耍寶】 %s%s %s\n\r\e[0m"
        , mob_name( victim, ch ), buf1, buf2 );
    }

    RETURN_NULL();
  }

  one_argument( argument , arg );

  if ( !arg[0] )
  {
    victim     = NULL;
    victim_org = NULL;
  }

  else
  {
    if ( !( victim = get_char_world( ch, arg ) )
      || !( victim_org = victim->in_room ) )
    {
      act( "社交對象 $2$T$0 不在這世界呢。", ch, NULL, arg, TO_CHAR );
      RETURN_NULL();
    }

    if ( IS_NPC( victim ) )
    {
      act( "不能拿非玩家$N當對象喔。", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( IS_SET( victim->deaf, CHANNEL_SEMOTE ) )
    {
      act( "$N沒有把動作頻道打開耶﹗", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( is_affected( victim, SLOT_MASK ) )
    {
      send_to_char( "你的對象似乎找不到﹐奇怪﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( victim->in_room == ch->in_room && victim != ch )
    {
      act( "你和$N兩在同一間房內﹐作給自己看就好了﹗", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }
  }

  /* 把所有的人都移出 */
  for ( pChar = char_list; pChar; pChar = pChar->next )
  {
    if ( !verify_char( pChar )
      || IS_NPC( pChar )
      || pChar == ch
      || !pChar->in_room
      || IS_SET( pChar->deaf, CHANNEL_SEMOTE ) ) continue;

    pChar->social = pChar->in_room;
    char_from_room( pChar );
    char_to_room( pChar, ch->in_room );
  }

  /* 把 victim 也移出 */
  if ( victim && victim != ch )
  {
    char_from_room( victim );
    char_to_room( victim, ch->in_room );
  }

  /* 好了, 現在所有的人都在同一間房間裡了 */
  if ( !victim )
  {
    if ( pSocial->others_no_arg && *pSocial->others_no_arg )
    {
      sprintf( buf , "$2【耍寶】 %s$0", pSocial->others_no_arg );
      act( smash_act_keyword( buf, "nesNES" ) , ch, NULL, victim, TO_ROOM );
    }

    if ( pSocial->char_no_arg && *pSocial->char_no_arg )
    {
      sprintf( buf , "$2【耍寶】 %s$0", pSocial->char_no_arg );
      act( smash_act_keyword( buf, "nesNES" ) , ch, NULL, victim, TO_CHAR );
    }
  }

  else if ( victim == ch )
  {
    if ( pSocial->others_auto && *pSocial->others_auto )
    {
      sprintf( buf , "$2【耍寶】 %s$0", pSocial->others_auto );
      act( smash_act_keyword( buf, "nesNES" ), ch, NULL, victim, TO_ROOM );
    }

    if ( pSocial->char_auto && *pSocial->char_auto )
    {
      sprintf( buf , "$2【耍寶】 %s$0", pSocial->char_auto );
      act( smash_act_keyword( buf, "nesNES" ), ch, NULL, victim, TO_CHAR );
    }
  }

  else
  {
    if ( pSocial->others_found && *pSocial->others_found )
    {
      sprintf( buf , "$2【耍寶】 %s$0", pSocial->others_found );
      act( smash_act_keyword( buf, "nesNES" ), ch, NULL, victim, TO_NOTVICT );
    }

    if ( pSocial->char_found && *pSocial->char_found )
    {
      sprintf( buf , "$2【耍寶】 %s$0", pSocial->char_found );
      act( smash_act_keyword( buf, "nesNES" ), ch, NULL, victim, TO_CHAR );
    }

    if ( pSocial->vict_found && *pSocial->vict_found )
    {
      sprintf( buf , "$2【耍寶】 %s$0", pSocial->vict_found );
      act( smash_act_keyword( buf, "nesNES" ), ch, NULL, victim, TO_VICT );
    }
  }

  /* 把所有的人歸位 */
  for ( pChar = char_list; pChar; pChar = pChar->next )
  {
    if ( !verify_char( pChar )
      || IS_NPC( pChar )
      || pChar == ch
      || !pChar->social
      || IS_SET( pChar->deaf, CHANNEL_SEMOTE ) ) continue;

    char_from_room( pChar );
    char_to_room( pChar, pChar->social );
    pChar->social = NULL;
  }

  /* 也把 victim 歸位 */
  if ( victim && victim != ch )
  {
    char_from_room( victim );
    char_to_room( victim , victim_org );
  }

  RETURN_NULL();
}

FUNCTION( do_chat )
{
  char   social_action[ MAX_SOCIAL_MESSAGE ];
  char   buf[MAX_STRING_LENGTH];
  char * messages;

  PUSH_FUNCTION( "do_chat" );

  /* 檢查閒聊保留字 */
  if ( check_chat_xname( ch, argument ) == TRUE ) RETURN_NULL();

  if ( check_repeat( ch , argument ) == TRUE )
  {
    send_to_char( "你不能講相同的話。\n\r" , ch );
    RETURN_NULL();
  }

  messages = transfer_messages( argument );
  if ( messages ) str_cpy( social_action , messages );
  else            social_action[0] = '\x0';

  str_cat( social_action , "閒聊" );

  filter_ansi( argument );
  ansi_transcribe( argument, buf );

  /* 是否和緩衝區的一樣 */
  if ( IS_NPC( ch ) && !str_cmp( buf, LastMessage ) ) RETURN_NULL();

  talk_channel( ch, buf, CHANNEL_CHAT, social_action );

  /* 拷貝顯示字串到緩衝區 */
  if ( IS_NPC( ch ) ) str_cpy( LastMessage, buf );

  RETURN_NULL();
}

FUNCTION( do_music )
{
  char   social_action[ MAX_SOCIAL_MESSAGE ];
  char   buf[MAX_STRING_LENGTH];
  char * messages;

  PUSH_FUNCTION( "do_music" );

  /* 檢查閒聊保留字 */
  if ( check_chat_xname( ch, argument ) == TRUE ) RETURN_NULL();

  if ( check_repeat( ch , argument ) == TRUE )
  {
    send_to_char( "你不能唱相同的歌。\n\r" , ch );
    RETURN_NULL();
  }

  messages = transfer_messages( argument );
  if ( messages ) str_cpy( social_action , messages );
  else                    social_action[0] = '\x0';

  str_cat( social_action , "隨著音樂起伏唱道" );

  filter_ansi( argument );
  ansi_transcribe( argument, buf );

  /* 是否和緩衝區的一樣 */
  if ( IS_NPC( ch ) && !str_cmp( buf, LastMessage ) ) RETURN_NULL();

  talk_channel( ch, buf, CHANNEL_MUSIC, social_action );

  /* 拷貝顯示字串到緩衝區 */
  if ( IS_NPC( ch ) ) str_cpy( LastMessage, buf );

  RETURN_NULL();
}

/* 同職業漫談 */
FUNCTION( do_cchat )
{
  char   social_action[ MAX_SOCIAL_MESSAGE ];
  char   buf[MAX_STRING_LENGTH];
  char * messages;

  PUSH_FUNCTION( "do_cchat" );

  if ( !ch->class )
  {
    send_to_char( "你沒有職業﹐要跟誰說話呢﹖\n\r", ch );
    RETURN_NULL();
  }

  /* 檢查閒聊保留字 */
  if ( check_chat_xname( ch, argument ) == TRUE ) RETURN_NULL();

  if ( check_repeat( ch , argument ) == TRUE )
  {
    send_to_char( "你不能講相同的話。\n\r" , ch );
    RETURN_NULL();
  }

  messages = transfer_messages( argument );
  if ( messages ) str_cpy( social_action , messages );
  else            social_action[0] = '\x0';

  str_cat( social_action , "說道" );

  filter_ansi( argument );
  ansi_transcribe( argument, buf );
  talk_channel( ch, buf, CHANNEL_CLASS, social_action );
  RETURN_NULL();
}

FUNCTION( do_gchat )
{
  char   social_action[ MAX_SOCIAL_MESSAGE ];
  char   buf[MAX_STRING_LENGTH];
  char * messages;

  PUSH_FUNCTION( "do_gchat" );

  /* 檢查閒聊保留字 */
  if ( check_chat_xname( ch, argument ) == TRUE ) RETURN_NULL();

  if ( check_repeat( ch , argument ) == TRUE )
  {
    send_to_char( "你不能講相同的話。\n\r" , ch );
    RETURN_NULL();
  }

  messages = transfer_messages( argument );
  if ( messages ) str_cpy( social_action , messages );
  else            social_action[0] = '\x0';

  str_cat( social_action , "呼喚" );

  filter_ansi( argument );
  ansi_transcribe( argument, buf );
  talk_channel( ch, buf, CHANNEL_GROUP, social_action );

  RETURN_NULL();
}

/* 同幫派漫談 */
FUNCTION( do_clubchat )
{
  char   social_action[ MAX_SOCIAL_MESSAGE ];
  char   buf[MAX_STRING_LENGTH];
  char * messages;

  PUSH_FUNCTION( "do_clubchat" );

  if ( !ch->club )
  {
    send_to_char( "你沒有幫派﹐要跟誰說話呢﹖\n\r", ch );
    RETURN_NULL();
  }

  /* 檢查閒聊保留字 */
  if ( check_chat_xname( ch, argument ) == TRUE ) RETURN_NULL();

  if ( check_repeat( ch , argument ) == TRUE )
  {
    send_to_char( "你不能講相同的話。\n\r" , ch );
    RETURN_NULL();
  }

  messages = transfer_messages( argument );
  if ( messages ) str_cpy( social_action , messages );
  else            social_action[0] = '\x0';

  str_cat( social_action , "說道" );

  filter_ansi( argument );
  ansi_transcribe( argument, buf );
  talk_channel( ch, buf, CHANNEL_CLUB, social_action );
  RETURN_NULL();
}

FUNCTION( do_question )
{
  char   social_action[ MAX_SOCIAL_MESSAGE ];
  char   buf[MAX_STRING_LENGTH];
  char * messages;

  PUSH_FUNCTION( "do_question" );

  /* 檢查閒聊保留字 */
  if ( check_chat_xname( ch, argument ) == TRUE ) RETURN_NULL();

  if ( check_repeat( ch , argument ) == TRUE )
  {
    send_to_char( "你不能問相同的問題。\n\r" , ch );
    RETURN_NULL();
  }

  messages = transfer_messages( argument );
  if ( messages ) str_cpy( social_action , messages );
  else            social_action[0] = '\x0';

  str_cat( social_action , "疑惑地問道" );

  filter_ansi( argument );
  ansi_transcribe( argument, buf );
  talk_channel( ch, buf, CHANNEL_QUESTION, social_action );
  RETURN_NULL();
}

FUNCTION( do_answer )
{
  ANSWER_DATA   * pAnswer;
  QUESTION_DATA * pQuestion;
  char            arg[MAX_INPUT_LENGTH];
  char            social_action[ MAX_SOCIAL_MESSAGE ];
  char            buf[MAX_STRING_LENGTH];
  char          * messages;
  int             answer;
  int             random;

  PUSH_FUNCTION( "do_answer" );

  if ( !IS_NPC( ch )
    && ( pAnswer = ch->answer )
    && ( pQuestion = pAnswer->question ) )
  {
    argument = one_argument( argument, arg );

    if ( arg[0] == '\x0' || !is_number( arg ) )
    {
      send_answer( ch, ch );
      RETURN_NULL();
    }

    answer = atoi( arg ) - 1;
    if ( answer < 0
      || answer >= sizeof( pQuestion->question )
      || answer >= sizeof( pQuestion->answer   ) )
    {
      send_to_char( "你的答案不合理﹗\n\r", ch );
      RETURN_NULL();
    }

    random = pAnswer->random[answer];
    if ( !pQuestion->question[random]
      || !*pQuestion->question[random]
      || pQuestion->answer[random] == FALSE )
    {
      if ( ++pAnswer->false < QuestionFalse )
      {
        act( "對不起﹐你答錯了﹐你答錯$i次﹐答錯$I次即認定為機器人﹗"
          , ch, &pAnswer->false, &QuestionFalse, TO_CHAR );

        RETURN_NULL();
      }

      if ( jail_someone( NULL, ch, PlyPenalty, FALSE ) )
      {
        if ( ch->pcdata ) ch->pcdata->ply = 0;

        sprintf( buf, "%s為機器人嫌疑犯﹐被系統捉起來了﹗"
          , mob_name( NULL, ch ) );

        talk_channel_2( buf, CHANNEL_BULLETIN, "公告" );

        send_to_char( "因為你連續答錯﹐所以系統認定你是機器人。\n\r", ch );
        pQuestion->fail++;
        free_struct( pAnswer, STRUCT_ANSWER_DATA );
        ch->answer = NULL;
      }
      else
      {
        send_to_char( "對不起﹐系統認定你為機器人嫌疑犯﹗\n\r", ch );
      }
      RETURN_NULL();
    }

    send_to_char( "恭喜你﹐你答對了﹗\n\r", ch );

    if ( ch->pcdata ) ch->pcdata->ply = 0;

    free_struct( pAnswer, STRUCT_ANSWER_DATA );
    ch->answer = NULL;
    RETURN_NULL();
  }

  /* 檢查閒聊保留字 */
  if ( check_chat_xname( ch, argument ) == TRUE ) RETURN_NULL();

  if ( check_repeat( ch , argument ) == TRUE )
  {
    send_to_char( "你不能回答相同的答案。\n\r" , ch );
    RETURN_NULL();
  }

  messages = transfer_messages( argument );
  if ( messages ) str_cpy( social_action , messages );
  else            social_action[0] = '\x0';

  str_cat( social_action , "回答說" );

  filter_ansi( argument );
  ansi_transcribe( argument, buf );
  talk_channel( ch, buf, CHANNEL_QUESTION, social_action );
  RETURN_NULL();
}

FUNCTION( do_shout )
{
  char   social_action[ MAX_SOCIAL_MESSAGE ];
  char   buf[MAX_STRING_LENGTH];
  char * messages;

  PUSH_FUNCTION( "do_shout" );

  /* 檢查閒聊保留字 */
  if ( check_chat_xname( ch, argument ) == TRUE ) RETURN_NULL();

  if ( check_repeat( ch , argument ) == TRUE )
  {
    send_to_char( "你不能喊相同的話。\n\r" , ch );
    RETURN_NULL();
  }

  messages = transfer_messages( argument );
  if ( messages ) str_cpy( social_action , messages );
  else            social_action[0] = '\x0';

  str_cat( social_action , "大喊" );

  filter_ansi( argument );
  ansi_transcribe( argument, buf );

  /* 是否和緩衝區的一樣 */
  if ( IS_NPC( ch ) && !str_cmp( buf, LastMessage ) ) RETURN_NULL();

  talk_channel( ch, buf, CHANNEL_SHOUT, social_action );
  WAIT_STATE( ch, 10 );

  /* 拷貝顯示字串到緩衝區 */
  if ( IS_NPC( ch ) ) str_cpy( LastMessage, buf );

  RETURN_NULL();
}

FUNCTION( do_yell )
{
  char   social_action[ MAX_SOCIAL_MESSAGE ];
  char   buf[MAX_STRING_LENGTH];
  char * messages;

  PUSH_FUNCTION( "do_yell" );

  /* 檢查閒聊保留字 */
  if ( check_chat_xname( ch, argument ) == TRUE ) RETURN_NULL();

  if ( check_repeat( ch , argument ) == TRUE )
  {
    send_to_char( "你不能喊相同的話。\n\r" , ch );
    RETURN_NULL();
  }

  messages = transfer_messages( argument );
  if ( messages ) str_cpy( social_action , messages );
  else            social_action[0] = '\x0';

  str_cat( social_action , "尖叫" );

  filter_ansi( argument );
  ansi_transcribe( argument, buf );

  /* 是否和緩衝區的一樣 */
  if ( IS_NPC( ch ) && !str_cmp( buf, LastMessage ) ) RETURN_NULL();

  talk_channel( ch, buf, CHANNEL_YELL, social_action );

  /* 拷貝顯示字串到緩衝區 */
  if ( IS_NPC( ch ) ) str_cpy( LastMessage, buf );

  RETURN_NULL();
}

FUNCTION( do_immtalk )
{
  char buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "do_immtalk" );

  /* 檢查閒聊保留字 */
  if ( check_chat_xname( ch, argument ) == TRUE ) RETURN_NULL();

  if ( check_repeat( ch , argument ) == TRUE )
  {
    send_to_char( "你不能說相同的話。\n\r" , ch );
    RETURN_NULL();
  }

  filter_ansi( argument );
  ansi_transcribe( argument, buf );
  talk_channel( ch, buf, CHANNEL_IMMTALK, "說道" );
  RETURN_NULL();
}

FUNCTION( do_say )
{
  char buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "do_say" );

  /* 檢查閒聊保留字 */
  if ( check_chat_xname( ch, argument ) == TRUE ) RETURN_NULL();

  if ( check_repeat( ch , argument ) == TRUE )
  {
    send_to_char( "你不能說相同的話。\n\r" , ch );
    RETURN_NULL();
  }

  filter_ansi( argument );

  ansi_transcribe( argument, buf );
  act( "$1$n$1說道﹕$3$T$1$0", ch, NULL, buf, TO_ROOM );
  act( "$1你說道﹕$3$t$1$0"  , ch, buf, NULL, TO_CHAR );
  RETURN_NULL();
}

FUNCTION( do_phone )
{
  char        arg[MAX_INPUT_LENGTH];
  CHAR_DATA * victim;
  int         position;

  PUSH_FUNCTION( "do_phone" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  if ( IS_SET( ch->act , PLR_SILENCE ) )
  {
    send_to_char( "你的嘴巴被封住了。\n\r" , ch );
    RETURN_NULL();
  }

  one_argument( argument , arg );

  if ( !( victim = get_char_world( ch , arg ) ) )
  {
    act( "找不到你要呼叫的人耶 $2$T$0。" , ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  if ( IS_NPC( victim ) )
  {
    act( "$N是非玩家﹐所以不能呼叫﹗", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( ch == victim )
  {
    send_to_char( "你是不是瘋了﹐竟然呼叫自己﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( !IS_IMMORTAL( ch ) && is_badfriend( victim, ch->name ) )
  {
    act( "$N不接受你的任何訊息。", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( victim->desc && is_edit( victim->desc ) )
  {
    act( "$N正在編輯文件中﹐請稍後再呼叫﹗", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( victim->desc && set_descriptor_stack( victim->desc ) )
  {
    victim->desc->stack = FALSE;
    act( "$N正在閱\讀文件中﹐請稍後再呼叫﹗", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( IS_SET( victim->deaf, CHANNEL_PHONE ) )
  {
    act( "$N的呼叫器沒有打開耶。", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  act( "你呼叫$N。", ch, NULL, victim, TO_CHAR );

  position         = victim->position;
  victim->position = POS_STANDING;
  act( "$n正在呼叫你。$B$B$B$B", ch, NULL, victim, TO_VICT );
  victim->position = position;

  RETURN_NULL();
}

FUNCTION( do_tell )
{
  char        arg[MAX_INPUT_LENGTH];
  char        buf[MAX_STRING_LENGTH];
  CHAR_DATA * victim;
  int         position;

  PUSH_FUNCTION( "do_tell" );

  if ( ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_SILENCE ) )
    || ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_NO_TELL ) ) )
  {
    send_to_char( "他現在沒辦法聽到你的聲音﹐你被封口了。\n\r", ch );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg );

  if ( !arg[0] || !argument[0] )
  {
    send_to_char( "你要告訴誰什麼﹖\n\r", ch );
    RETURN_NULL();
  }

  if ( !( victim = get_char_world( ch, arg ) )
    || ( IS_NPC( victim ) && victim->in_room != ch->in_room ) )
  {
    act( "找不到你的對象 $2$T$0。", ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  if ( victim == ch )
  {
    send_to_char( "你在喃喃自語嗎﹖\n\r", ch );
    RETURN_NULL();
  }

  if ( !IS_IMMORTAL( ch ) && !IS_AWAKE( victim ) )
  {
    act( "$N目前無法聽見你的聲音。", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  /* 檢查閒聊保留字 */
  if ( check_chat_xname( ch, argument ) == TRUE ) RETURN_NULL();

  if ( check_repeat( ch , argument ) == TRUE )
  {
    act( "你不能告訴$N相同的話。" , ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( !IS_IMMORTAL( ch ) && is_badfriend( victim, ch->name ) )
  {
    act( "$N不接受你的任何訊息。", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  position         = victim->position;
  victim->position = POS_STANDING;

  if ( IS_NPC( victim ) ) delete_chat( ch );

  if ( victim->in_room && victim->in_room == ch->in_room )
  {
    ansi_transcribe( argument, buf );
    act( "$7你輕聲的告訴$N$7﹕「$1$t$7」$0", ch, buf, victim, TO_CHAR );
    act( "$7$n$7輕聲的告訴你﹕「$1$t$7」$0", ch, buf, victim, TO_VICT );
    act( "$4$n$4和$N$4正在竊竊私語喔﹗$0", ch, NULL, victim, TO_NOTVICT );

    if ( !IS_NPC( victim )
      && victim->desc
      && ( is_edit( victim->desc ) || set_descriptor_stack( victim->desc ) ) )
    {
      act( "$N目前正在閱\讀或是編輯文件中﹗", ch, NULL, victim, TO_CHAR );
    }

  }

  else
  {
    if ( victim->in_room )
    {
      ansi_transcribe( argument, buf );
      if ( !IS_IMMORTAL( ch ) ) ch->move = UMAX( 0, ch->move - 1 );
      act( "$7你以千里傳音告訴$N$7﹕「$1$t$7」$0"
        , ch, buf, victim, TO_CHAR );
      act( "$7$n$7使用千里傳音告訴你﹕「$1$t$7」$0"
        , ch, buf, victim, TO_VICT );

      if ( !IS_NPC( victim )
        && victim->desc
        && ( is_edit( victim->desc ) || set_descriptor_stack( victim->desc ) ) )
      {
        act( "$N目前正在閱\讀或是編輯文件中﹗", ch, NULL, victim, TO_CHAR );
      }

    }

    else
    {
      victim->position = position;
      victim->reply    = ch;
      act( "$N正在不知名的地方﹐你無法跟他聯絡﹗", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }
  }

  victim->position = position;
  victim->reply    = ch;
  mprog_speech_trigger( argument , ch );

  RETURN_NULL();
}

FUNCTION( do_reply )
{
  CHAR_DATA * victim;
  char        buf[MAX_STRING_LENGTH];
  int         position;

  PUSH_FUNCTION( "do_reply" );

  if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_SILENCE ) )
  {
    send_to_char( "他現在沒辦法聽到你的聲音﹐你被封口了。\n\r", ch );
    RETURN_NULL();
  }

  if ( !( victim = ch->reply ) )
  {
    send_to_char( "找不到你要回話的對象。\n\r", ch );
    RETURN_NULL();
  }

  if ( !IS_IMMORTAL( ch ) && !IS_AWAKE( victim ) )
  {
    act( "$N目前無法聽見你的聲音。", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  /* 檢查閒聊保留字 */
  if ( check_chat_xname( ch, argument ) == TRUE ) RETURN_NULL();

  if ( check_repeat( ch , argument ) == TRUE )
  {
    act( "你不能告訴$N相同的話。", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( !IS_IMMORTAL( ch ) && is_badfriend( victim, ch->name ) )
  {
    act( "$N不接受你的任何訊息。", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  ansi_transcribe( argument, buf );

  act( "$7你回答$N$7說道﹕「$1$t$7」$0", ch, buf, victim, TO_CHAR );
  position         = victim->position;
  victim->position = POS_STANDING;

  act( "$7$n$7回答你﹕「$1$t$7」$0", ch, buf, victim, TO_VICT );
  victim->position = position;
  victim->reply    = ch;

  RETURN_NULL();
}

FUNCTION( do_emote )
{
  char buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "do_emote" );

  if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_NO_EMOTE ) )
  {
    send_to_char( "你無法秀出你的動作。\n\r", ch );
    RETURN_NULL();
  }

  /* 檢查閒聊保留字 */
  if ( check_chat_xname( ch, argument ) == TRUE ) RETURN_NULL();

  if ( check_repeat( ch , argument ) == TRUE )
  {
    send_to_char( "你不能做相同的動作。\n\r" , ch );
    RETURN_NULL();
  }

  ansi_transcribe( argument, buf );
  act( "$n$T", ch, NULL, buf, TO_ALL );
  RETURN_NULL();
}

FUNCTION( do_bug )
{
  PUSH_FUNCTION( "do_bug" );

  if ( !IS_NPC( ch ) && argument[0] )
  {
    /* 檢查閒聊保留字 */
    if ( check_chat_xname( ch, argument ) == TRUE ) RETURN_NULL();

    mudlog( LOG_DEBUG , "玩家回報錯誤 [%5d] %s: %s"
      , ch->in_room ? ch->in_room->vnum : 0, ch->name, argument );
    send_to_char( "謝謝你所回報的系統錯誤。\n\r", ch );
  }

  RETURN_NULL();
}

FUNCTION( do_idea )
{
  PUSH_FUNCTION( "do_idea" );

  if ( !IS_NPC( ch ) && argument[0] )
  {
    /* 檢查閒聊保留字 */
    if ( check_chat_xname( ch, argument ) == TRUE ) RETURN_NULL();

    mudlog( LOG_IDEA , "[%5d] %s: %s"
      , ch->in_room ? ch->in_room->vnum : 0, ch->name, argument );
    send_to_char( "謝謝你的主意。\n\r", ch );
  }

  RETURN_NULL();
}

FUNCTION( do_typo )
{
  PUSH_FUNCTION( "do_typo" );

  if ( !IS_NPC( ch ) && argument[0] )
  {
    /* 檢查閒聊保留字 */
    if ( check_chat_xname( ch, argument ) == TRUE ) RETURN_NULL();

    mudlog( LOG_TYPO , "[%5d] %s: %s"
      , ch->in_room ? ch->in_room->vnum : 0, ch->name, argument );
    send_to_char( "謝謝你所回報的筆誤。\n\r", ch );
  }

  RETURN_NULL();
}

FUNCTION( do_retire )
{
  OBJ_DATA       * pObj;
  OBJ_DATA       * obj_next;
  OBJ_INDEX_DATA * pObjIndex;
  QUEST_DATA     * pQuest;
  QUEST_DATA     * quest_next;
  ENROLL_DATA    * pEnroll;
  ENROLL_DATA    * zEnroll;
  SKILL_DATA     * pSkill;
  AFFECT_DATA    * paf;
  AFFECT_DATA    * paf_next;
  ENEMY_DATA     * pEnemy;
  ENEMY_DATA     * zEnemy;
  char             arg[MAX_INPUT_LENGTH];
  char             buf[MAX_STRING_LENGTH];
  char           * pArg;
  char             cEnd;
  int              loop;

  PUSH_FUNCTION( "do_retire" );

  if ( IS_NPC( ch ) || !ch->pcdata ) RETURN_NULL();

  pArg = arg;
  while ( isSpace( ( int ) *argument ) ) argument++;

  cEnd = ' ';
  if ( *argument == '\'' || *argument == '"' ) cEnd = *argument++;

  while ( *argument )
  {
    if ( *argument == cEnd )
    {
      argument++;
      break;
    }
    *pArg++ = *argument++;
  }

  *pArg = '\x0';

  if ( !arg[0] )
  {
    send_to_char( "你必須完整打出 retire <密碼>。\n\r", ch );
    RETURN_NULL();
  }

  if ( str_cmp( crypt( arg, ch->pcdata->pwd ), ch->pcdata->pwd ) )
  {
    send_to_char( "密碼不合喔﹐你再考慮清楚吧。\n\r", ch );
    RETURN_NULL();
  }

  if ( IS_IMMORTAL( ch ) )
  {
    send_to_char( "你是系統管理者﹐不能重練。\n\r", ch );
    RETURN_NULL();
  }

  if ( is_pk( ch ) )
  {
    send_to_char( "你必須先從競技場離開才行﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->position != POS_STANDING )
  {
    send_to_char( "你先把你的事情忙完再說吧﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->leader || ch->master )
  {
    send_to_char( "你還有其他伙伴﹐先離開他們吧﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->donate > 0 )
  {
    send_to_char( "避免嫌疑﹐在你領救濟金的這段時間﹐請不要重練﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->jail > 0 )
  {
    send_to_char( "你還在天牢裡﹐必須出來之後才能重練﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->failed > 0 )
  {
    send_to_char( "你還在思過崖﹐必須思過後才能重練﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->level <= 1 )
  {
    send_to_char( "你目前的等級和重練沒什麼兩樣﹗\n\r", ch );
    RETURN_NULL();
  }

  /* 移出物品 */
  for ( pObj = ch->carrying; pObj; pObj = obj_next )
  {
    obj_next = pObj->next_content;
    act( "你身上的$p永遠地離開這個世界了﹗", ch, pObj, NULL, TO_CHAR );
    obj_from_char( pObj );
    extract_obj( pObj );
  }

  ch->carrying = NULL;

  /* 移除寄放 */
  for ( pObj = ch->deposit; pObj; pObj = obj_next )
  {
    obj_next = pObj->next_content;
    act( "你寄放的$p永遠地離開這個世界了﹗", ch, pObj, NULL, TO_CHAR );
    obj_from_char_deposit( pObj );
    extract_obj( pObj );
  }

  ch->deposit = NULL;

  /* 把身上的影響給釋放 */
  if ( ch->affected )
  {
    for ( paf = ch->affected; paf; paf = paf_next )
    {
      paf_next = paf->next;
      affect_remove( ch, paf );
    }

    send_to_char( "移除你身上所有法術影響記錄﹗\n\r", ch );
    ch->affected = NULL;
  }

  /* 修正致能 */
  extract_enable( ch );
  ch->enable = NULL;

  /* 修正解謎的旗標 */
  if ( ch->quest )
  {
    for ( pQuest = ch->quest; pQuest; pQuest = quest_next )
    {
      quest_next = pQuest->next;
      free_struct( pQuest, STRUCT_QUEST_DATA );
    }

    send_to_char( "移除你身上所有解謎的記錄﹗\n\r", ch );
    ch->quest = NULL;
  }

  /* 清除公敵 */
  if ( ch->enemy )
  {
    for ( pEnemy = ch->enemy; pEnemy; pEnemy = zEnemy )
    {
      zEnemy = pEnemy->next;

      free_string( pEnemy->city );
      free_struct( pEnemy, STRUCT_ENEMY_DATA );
    }

    send_to_char( "移除你身上所有公敵的記錄﹗\n\r", ch );
    ch->enemy = NULL;
  }

  /* 修正技能 */
  for ( loop = 0; loop < MAX_SKILL; loop++ )
  {
    if ( ch->skill[loop] > 0 )
    {
      if ( !( pSkill = get_skill( loop ) ) ) continue;

      if ( !pSkill->innate )
      {
        ch->skill[loop] = 0;
        act( "你所學的$w已經漸漸遺忘了﹗", ch, pSkill, NULL, TO_CHAR );
      }
      else
      {
        ch->skill[loop] = UMIN( ch->skill[loop], pSkill->adeptation );
        act( "你所學的$w已經沒有之前那麼熟練了﹗", ch, pSkill, NULL, TO_CHAR );
      }
    }
  }

  /* 取消記仇部份 */
  if ( ch->enroll )
  {
    for ( pEnroll = ch->enroll; pEnroll; pEnroll = zEnroll )
    {
      zEnroll = pEnroll->next;
      free_struct( pEnroll, STRUCT_ENROLL_DATA );
    }

    ch->enroll = NULL;
    send_to_char( "移除你身上所有記仇的記錄﹗\n\r", ch );
  }

  /* 等級一的基本屬性 */
  ch->level   = 1;
  ch->now_str = DefaultStr;
  ch->now_int = DefaultInt;
  ch->now_wis = DefaultWis;
  ch->now_dex = DefaultDex;
  ch->now_con = DefaultCon;

  ch->hit      = DefaultHit;
  ch->max_hit  = DefaultHit;
  ch->mod_hit  = 0;
  ch->mana     = DefaultMana;
  ch->max_mana = DefaultMana;
  ch->mod_mana = 0;
  ch->move     = DefaultMove;
  ch->max_move = DefaultMove;
  ch->mod_move = 0;

  /* 訓練點數和其他基本數值 */
  ch->practice             = DefaultPractice;
  ch->limit                = 0;
  ch->nskill               = 0;
  ch->clublog              = FALSE;
  ch->hitroll              = 0;
  ch->damroll              = 0;
  ch->eq_capcity           = 50;
  ch->trust                = 0;
  ch->exp                  = 0;
  ch->alignment            = DefaultAlign;
  ch->wimpy                = 0;
  ch->armor                = 0;
  ch->firman               = 0;
  ch->pcdata->pills        = 0;
  ch->pcdata->dosage       = 0;
  ch->pcdata->steal_panity = 0;
  ch->pcdata->steal        = 0;
  ch->pcdata->murder       = 0;
  ch->pcdata->rechristen   = Rechristen;

  /* 飢餓狀態 */
  ch->pcdata->condition[COND_THIRST]  = 100;
  ch->pcdata->condition[COND_FULL]    = 100;

  /* 更正英雄榜 */
  check_hero( ch->name );

  /* 設定抬頭 */
  ch->class = class_demos;
  str_cpy( buf, ch->class->title );
  set_title( ch, buf );

  /* 清除 Recall */
  for ( loop = 0; loop < MAX_RECALL; loop++ )
  {
    if ( RecallPlace[loop] > 0 )
    ch->pcdata->recall[loop] = RecallPlace[loop];
  }

  /* 設定 Enable */
  for ( loop = 0; loop < MAX_SKILL; loop++ )
  {
    if ( ch->skill[loop] > 0
      && ( pSkill = get_skill( loop ) )
      && pSkill->enable )
    {
      set_enable( ch, pSkill );
    }
  }

  /* 裝備基本配備 */
  for ( loop = 0; loop < MAX_DEFAULT_OBJECT; loop++ )
  {
    if ( DefaultObject[loop] <= 0
      || !( pObjIndex = get_obj_index( DefaultObject[loop] ) ) )
      break;

    if ( ( pObj = create_object( pObjIndex, -1 ) ) )
    {
      obj_to_char( pObj, ch );
      wear_obj( ch, pObj, FALSE, FALSE );
    }
  }

  save_char_obj( ch, SAVE_FILE   );
  save_char_obj( ch, BACKUP_FILE );

  sprintf( buf, "%s對於自己的表現十分不滿意﹐所以重新開始他的另外一頁﹗"
    , mob_name( NULL, ch ) );

  talk_channel_2( buf, CHANNEL_BULLETIN, "小道消息" );

  send_to_char( "你對自己的表現不甚滿意﹐所以重新開始你的新生活﹗\n\r", ch );

  RETURN_NULL();
}

FUNCTION( do_suicide )
{
  DESCRIPTOR_DATA * d;
  CLUB_DATA       * pClub;
  CHAR_DATA       * pChar;
  OBJ_DATA        * pObj;
  char              arg1[MAX_INPUT_LENGTH];
  char              filename[MAX_INPUT_LENGTH];
  char              exec_cmd[MAX_STRING_LENGTH];
  char              talkbuf[MAX_STRING_LENGTH];
  char            * pArg;
  char              cEnd;
  int               level;

  PUSH_FUNCTION( "do_suicide" );

  if ( IS_NPC( ch ) || !ch->pcdata ) RETURN_NULL();

  pArg = arg1;
  while ( isSpace( ( int ) *argument ) ) argument++;

  cEnd = ' ';
  if ( *argument == '\'' || *argument == '"' ) cEnd = *argument++;

  while ( *argument )
  {
    if ( *argument == cEnd )
    {
      argument++;
      break;
    }
    *pArg++ = *argument++;
  }
  *pArg = '\x0';

  if ( !arg1[0] )
  {
    send_to_char( "你必須完整打出 suicide <密碼>。\n\r", ch );
    RETURN_NULL();
  }

  if ( str_cmp( crypt( arg1, ch->pcdata->pwd ), ch->pcdata->pwd ) )
  {
    send_to_char( "密碼不合喔﹐你再考慮清楚吧。\n\r", ch );
    RETURN_NULL();
  }

  if ( IS_IMMORTAL( ch ) )
  {
    send_to_char( "你的工作沒有做完﹐不准自殺。\n\r", ch );
    RETURN_NULL();
  }

  if ( ( pClub = ch->club )
    && ( ( level = name_in_club( ch->name, pClub ) ) == CLUB_MASTER ) )
  {
    send_to_char( "你是一幫之主﹐自殺不好吧。\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->pcdata && ch->pcdata->mater && *ch->pcdata->mater )
  {
    send_to_char( "真的看破塵世了嗎﹖先離婚吧﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( is_pk( ch ) )
  {
    send_to_char( "你還在武鬥大會中﹐冷靜一點﹐等結束再說﹗\n\r", ch );
    RETURN_NULL();
  }

  /* 修正記仇 */
  for ( pChar = char_list; pChar; pChar = pChar->next )
  {
    if ( verify_char( pChar )
      && IS_NPC( pChar )
      && IS_SET( pChar->act, ACT_ENROLL ) )
    {
      extract_enroll( pChar, ch->name );
    }
  }

  /* 銷毀屍體 */
  for ( pObj = object_list; pObj; pObj = pObj->next )
  {
    if ( verify_obj( pObj )
      && pObj->item_type == ITEM_CORPSE_PC
      && pObj->corpse_owner == ch )
    {
      extract_obj( pObj );
    }
  }

  /* 更正英雄榜 */
  check_hero( ch->name );

  /* 更正幫派 */
  if ( ch->club ) char_from_club( ch->name, ch->club, CLUB_ALL );

  if ( !IS_NPC( ch ) && ch->level >= level_limit )
  {
    sprintf( talkbuf, "\e[1;31m天邊的晚霞倏地散了開來﹐在蔚藍的天空劃上了一"
      "片絢爛的虹…\n\r\e[0m\e[1;36m遙遠的天邊又有一顆星降了下來﹐"
      "連天空都下起雨來﹐似乎在無聲的哭泣著…\n\r\e[0m" );

    do_echo( NULL, talkbuf );

    sprintf( talkbuf, "對於這樣的亂世﹐我無能為力了﹐永別了﹐各位﹗");
    talk_channel( ch, talkbuf , CHANNEL_SUICIDE , "哭泣地說道" );

    sprintf( talkbuf, "%s從此走入了歷史﹐你以後再也看不到這個人了﹗"
      , mob_name( NULL, ch ) );

    do_echo( NULL, talkbuf );
  }

  send_to_char(
    "\n\r你的鮮血染紅了整個絢爛的天空。這是亂世的錯抑是自己呢﹖\n\r"
    "安息吧﹐永別了﹐我的中國﹗\n\r\n\r ",ch );

  mudlog( LOG_SUICIDE, "%s 自裁.", mob_name( NULL, ch ) );
  delete_database( ch );

  str_cpy( filename, file_name( ch->name, SAVE_FILE ) );
  save_char_obj( ch , SAVE_FILE );
  d = ch->desc;
  extract_char( ch, TRUE );
  if ( d ) close_socket( d );

  sprintf( exec_cmd, "rm -rf %s/%s/%s"
    , player_dir, initial( ch->name ), normalize( ch->name ) );

  system( exec_cmd );
  RETURN_NULL();
}

FUNCTION( do_dormancy )
{
  char arg[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  int  day;

  PUSH_FUNCTION( "do_dormancy" );

  if ( IS_NPC( ch )
    || !ch->desc
    || !can_quit( ch, TRUE )
    || !ch->pcdata ) RETURN_NULL();

  one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    send_to_char( "你打算要冬眠幾天呢﹖\n\r", ch );
    RETURN_NULL();
  }

  if ( !is_number( arg ) || ( day = atoi( arg ) ) <= 0 || day >= hold_day )
  {
    send_to_char( "你這樣冬眠會死人的耶﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( is_pk( ch ) )
  {
    send_to_char( "你還在武鬥大會中﹐冷靜一點﹐等結束再說﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( IS_IMMORTAL( ch ) )
  {
    send_to_char( "你是管理者﹐沒有權力冬眠喔﹗\n\r", ch );
    RETURN_NULL();
  }

  ch->pcdata->dormancy = time( NULL ) + day * 60 * 60 * 24;
  act( "你將在$i天後醒來﹐你慢慢地昏睡過去了﹗", ch, &day, NULL, TO_CHAR );

  sprintf( buf, "%s進入了冬眠狀態﹐%s天後才會醒來﹐再見了﹐各位﹗"
    , mob_name( NULL, ch ), arg );

  do_echo( ch, buf );
  do_quit( ch, "" );

  RETURN_NULL();
}

FUNCTION( do_quit )
{
  DESCRIPTOR_DATA * d;

  PUSH_FUNCTION( "do_quit" );

  if ( IS_NPC( ch ) || !can_quit( ch, TRUE ) ) RETURN_NULL();

  /* 修正坐騎 */
  if ( ch->mount    ) unmount_char( ch, ch->mount    );
  if ( ch->mount_by ) unmount_char( ch->mount_by, ch );

  send_to_char(
    "\n\r回想一下﹐中國的夢似乎還留在你的腦海中。"
    "剛剛發生的﹐是真的還是假的呢﹖\n\r"
    "唉﹐只能等待下次再續中國之夢吧﹗"
    "加油吧﹐我的中國之夢。\n\r\n\r", ch );

  act( "$n離開遊戲了。", ch, NULL, NULL, TO_ROOM );
  mudlog( LOG_INFO , "%s離開遊戲了。", ch->name );

  if ( IS_IMMORTAL( ch ) ) immortal_off_line( ch );

  save_char_obj( ch ,SAVE_FILE );
  d = ch->desc;
  extract_char( ch, TRUE );
  if ( d ) close_socket( d );

  RETURN_NULL();
}

FUNCTION( do_backup )
{
  char arg[MAX_INPUT_LENGTH];
  int  tick;

  PUSH_FUNCTION( "do_backup" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    save_char_obj( ch , BACKUP_FILE );
    send_to_char( "備份資料完畢。\n\r", ch );

    if ( excess_filequota( ch ) )
      send_to_char( "\e[1;31m你的備份檔案超出系統的配額了﹗\e[0m\n\r", ch );
  }

  else if ( !str_prefix( arg, "show" ) && ch->pcdata )
  {
    if ( ch->pcdata->autobackup <= 0 )
    {
      send_to_char( "你尚未設定自動備份的時間﹗\n\r", ch );
      RETURN_NULL();
    }

    act( "你設定每$i個系統時間就自動備份一次﹗"
      , ch, &ch->pcdata->autobackup, NULL, TO_CHAR );
  }

  else if ( !str_prefix( arg, "set" ) && ch->pcdata )
  {
    if ( argument[0] == '\x0' || !is_number( argument ) )
    {
      send_to_char( "你要設定幾個系統時間自動備份一次呢﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( ( tick = atoi( argument ) ) <= 0 )
    {
      send_to_char( "你取消了自動備份的功\能﹗\n\r", ch );

      ch->pcdata->autobackup      = -1;
      ch->pcdata->autobackup_tick = -1;

      RETURN_NULL();
    }

    if ( tick > 100 )
    {
      send_to_char( "對不起﹐你設定的時間不合法﹗\n\r", ch );
      RETURN_NULL();
    }

    ch->pcdata->autobackup      = tick;
    ch->pcdata->autobackup_tick = tick;

    act( "你設定$i個系統時間自動備份一次﹗", ch, &tick, NULL, TO_CHAR );
  }

  else
  {
    send_to_char( "對不起﹐你的語法錯誤﹐請查詢 backup 的使用方法﹗\n\r", ch );
  }

  RETURN_NULL();
}

FUNCTION( do_save )
{
  char arg[MAX_INPUT_LENGTH];
  int  tick;

  PUSH_FUNCTION( "do_save" );

  if ( IS_NPC( ch ) || !ch->pcdata ) RETURN_NULL();

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    save_char_obj( ch , SAVE_FILE );

    act( "儲存資料完畢﹐檔案大小為 $3$t$0 位元組﹗"
      , ch, numberize( ch->pcdata->file_size, NULL ), NULL, TO_CHAR );

    if ( excess_filequota( ch ) )
      send_to_char( "\a\a\e[1;31m你的檔案超出系統的配額了﹗\e[0m\n\r", ch );
  }

  else if ( !str_prefix( arg, "show" ) && ch->pcdata )
  {
    if ( ch->pcdata->autosave <= 0 )
    {
      send_to_char( "你尚未設定自動存檔的時間﹗\n\r", ch );
      RETURN_NULL();
    }

    act( "你設定每$i個系統時間就自動存檔一次﹗"
      , ch, &ch->pcdata->autosave, NULL, TO_CHAR );
  }

  else if ( !str_prefix( arg, "set" ) && ch->pcdata )
  {
    if ( argument[0] == '\x0' || !is_number( argument ) )
    {
      send_to_char( "你要設定幾個系統時間自動存檔一次呢﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( ( tick = atoi( argument ) ) <= 0 )
    {
      ch->pcdata->autosave      = -1;
      ch->pcdata->autosave_tick = -1;

      send_to_char( "你取消了自動存檔的功\能﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( tick > 100 )
    {
      send_to_char( "對不起﹐你設定的時間不合法﹗\n\r", ch );
      RETURN_NULL();
    }

    ch->pcdata->autosave      = tick;
    ch->pcdata->autosave_tick = tick;

    act( "你設定$i個系統時間自動存檔一次﹗", ch, &tick, NULL, TO_CHAR );
  }

  else
  {
    send_to_char( "對不起﹐你的語法錯誤﹐請查詢 save 的使用方法﹗\n\r", ch );
  }

  RETURN_NULL();
}

FUNCTION( do_follow )
{
  char        arg[MAX_INPUT_LENGTH];
  CHAR_DATA * victim;

  PUSH_FUNCTION( "do_follow" );

  one_argument( argument, arg );

  if ( ch->mercenary || ch->hirer )
  {
    send_to_char( "你有傭兵﹐所以不能跟隨別人。\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->boss )
  {
    send_to_char( "你是別人養的小鬼﹐不能跟隨別人喔﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( !IS_NPC( ch ) && is_pk( ch ) )
  {
    send_to_char( "對不起﹐你專心你的武鬥大會吧﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( !( victim = get_char_room( ch, arg ) ) )
  {
    act( "你找來找去﹐就是找不到 $2$T$0。", ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  if ( is_affected( ch, SLOT_CHARM_PERSON ) && ch->master )
  {
    act( "你已經跟隨了$N﹗", ch, NULL, ch->master, TO_CHAR );
    RETURN_NULL();
  }

  if ( victim == ch )
  {
    if ( !ch->master )
    {
      send_to_char( "你已經不再跟隨別人了。\n\r", ch );
      RETURN_NULL();
    }

    stop_follower( ch );
    RETURN_NULL();
  }

  if ( ( ch->level - victim->level < -( FollowLevel )
    || ch->level - victim->level > FollowLevel )
    && !IS_IMMORTAL(ch) )
  {
    send_to_char( "你不能加入這個隊伍。\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->master ) stop_follower( ch );
  add_follower( ch, victim );

  RETURN_NULL();
}

void add_follower( CHAR_DATA * ch, CHAR_DATA * master )
{
  PUSH_FUNCTION( "add_follower" );

  if ( !ch || !master )
  {
    mudlog( LOG_DEBUG, "add_follower: 來源不存在." );
    RETURN_NULL();
  }

  if ( ch->master )
  {
    mudlog( LOG_DEBUG , "add_follower: 還有其他主人." );
    RETURN_NULL();
  }

  ch->master = master;
  ch->leader = NULL;

  if ( can_see( master, ch ) )
    act( "$n現在跟隨你了。", ch, NULL, master, TO_VICT );

  act( "你開始跟隨$N。",  ch, NULL, master, TO_CHAR );

  RETURN_NULL();
}

void stop_follower( CHAR_DATA * ch )
{
  PUSH_FUNCTION( "stop_follower" );

  if ( !ch || !ch->master )
  {
    mudlog( LOG_DEBUG , "Stop_follower: 來源不存在." );
    RETURN_NULL();
  }

  if ( is_affected( ch, SLOT_CHARM_PERSON ) )
    affect_release( ch, SLOT_CHARM_PERSON );

  if ( can_see( ch->master, ch ) )
    act( "$n停止跟隨你。", ch, NULL, ch->master, TO_VICT );

  act( "你停止跟隨$N。", ch, NULL, ch->master, TO_CHAR );

  ch->master = ch->leader = NULL;
  RETURN_NULL();
}

void die_follower( CHAR_DATA * ch )
{
  CHAR_DATA * fch;

  PUSH_FUNCTION( "die_follower" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "die_follower: 來源不存在." );
    RETURN_NULL();
  }

  if ( ch->master ) stop_follower( ch );
  ch->leader = NULL;

  for ( fch = char_list; fch; fch = fch->next )
  {
    if ( !verify_char( fch ) ) continue;
    if ( fch->master == ch )   stop_follower( fch );
    if ( fch->leader == ch )   fch->leader = fch;
  }

  RETURN_NULL();
}

FUNCTION( do_order )
{
  char               arg[MAX_INPUT_LENGTH];
  CHAR_DATA        * victim;
  CMD_DATA         * pCommand;
  extern CMD_DATA  * LastCommand;
  extern CHAR_DATA * LastChar;
  extern char        LastArgument[];

  PUSH_FUNCTION( "do_order" );

  argument = one_argument( argument, arg );

  if ( !arg[0] || !argument[0] )
  {
    send_to_char( "你想命令誰去做什麼事﹖\n\r", ch );
    RETURN_NULL();
  }

  if ( is_affected( ch, SLOT_CHARM_PERSON ) )
  {
    send_to_char( "你已經失去心智﹐還想命令別人。\n\r", ch );
    RETURN_NULL();
  }

  if ( !( victim = get_char_room( ch, arg ) ) )
  {
    act( "$2$T$0 不在這裡。", ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  if ( victim == ch )
  {
    send_to_char( "你是不是瘋了﹐竟然命令自己做事情﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( ( !is_affected( victim, SLOT_CHARM_PERSON ) || victim->master != ch )
    && victim->boss != ch )
  {
    act( "你又不是$N的主人﹐你自己去做吧﹗", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( !IS_NPC( victim ) )
  {
    act( "雖然$N已經被你控制﹐但是你不能命令$E下指令﹗", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    act( "你要命令$N做什麼事呢﹖", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  for ( pCommand = cmd_list; pCommand; pCommand = pCommand->next )
  {
    if ( !pCommand->order )                                 continue;
    if ( get_trust( victim ) < pCommand->level )            continue;
    if ( pCommand->argument && !argument[0] )               continue;
    if ( victim->position < pCommand->position )            continue;
    if ( !pCommand->dead && is_dead( victim ) )             continue;
    if ( pCommand->lock )                                   continue;

    if ( ( !pCommand->exact && !str_prefix( arg, pCommand->name ) )
      || (  pCommand->exact && !str_cmp   ( arg, pCommand->name ) ) )
    {
      print_to_char( ch, "你命令%s做指令%s﹐參數%s。\n\r"
        , mob_name( ch, victim ), arg
        , argument[0] ? argument : "從缺" );

      /* 記錄最近使用狀況 */
      LastCommand = pCommand;
      LastChar    = ch;
      str_cpy( LastArgument , argument );

      /* 執行命令 */
      ( *pCommand->function ) ( victim , argument );

      /* 消除最近使用狀況 */
      LastCommand     = NULL;
      LastChar        = NULL;
      LastArgument[0] = '\x0';

      RETURN_NULL();
    }
  }

  send_to_char( "對不起﹐你命令的指令無效﹗\n\r", ch );
  RETURN_NULL();
}

FUNCTION( do_group )
{
  char        buf[MAX_STRING_LENGTH];
  char        arg[MAX_INPUT_LENGTH];
  CHAR_DATA * victim;
  CHAR_DATA * gch;
  CHAR_DATA * leader;

  PUSH_FUNCTION( "do_group" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  if ( is_pk( ch ) )
  {
    send_to_char( "對不起﹐你專心你的武鬥大會吧﹗\n\r", ch );
    RETURN_NULL();
  }

  one_argument( argument, arg );

  if ( !arg[0] )
  {
    leader = ch->leader ? ch->leader : ch;
    sprintf( buf, "%s的隊伍﹕\n\r", mob_name( ch, leader ) );
    send_to_char( buf, ch );

    /* 取得有效人物 */
    for ( gch = char_list; gch; gch = gch->next )
    {
      if ( !verify_char( gch ) ) continue;

      if ( is_same_group( gch, ch ) )
      {
        sprintf( buf,
        "[%3d %6s] %-6s  生命力%5d/%5d 法力%5d/%5d 體力 %5d/%5d\n\r"
            , gch->level
            , class_name( gch->class, FALSE )
            , mob_name( ch, gch )
            , gch->hit,   get_curr_hit( gch )
            , gch->mana,  get_curr_mana( gch )
            , gch->move,  get_curr_move( gch ) );
        send_to_char( buf, ch );
      }
    }
    RETURN_NULL();
  }

  if ( !( victim = get_char_room( ch, arg ) ) )
  {
    act( "$2$T$0 不在這裡。", ch, NULL, arg, TO_CHAR );
    RETURN_NULL();
  }

  if ( ch->hirer || ch->mercenary )
  {
    send_to_char( "你身旁已經有傭兵﹐不能跟別人組隊。\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->master || ( ch->leader && ch->leader != ch ) )
  {
    send_to_char( "你已經跟隨別人了。\n\r", ch );
    RETURN_NULL();
  }

  if ( ch == victim )
  {
    send_to_char( "你本身就是你自己的隊員了。\n\r", ch );
    RETURN_NULL();
  }

  if ( victim->master != ch )
  {
    act( "$N並沒有跟隨你。", ch, NULL, victim, TO_CHAR );
    RETURN_NULL();
  }

  if ( is_same_group( victim, ch ) && ch != victim )
  {
    victim->leader = NULL;
    act( "$n把$N從它的隊伍中剔除。", ch, NULL, victim, TO_NOTVICT );
    act( "$n把你從$n的隊伍中剔除。", ch, NULL, victim, TO_VICT    );
    act( "你把$N從你的隊伍中剔除。", ch, NULL, victim, TO_CHAR    );
    RETURN_NULL();
  }

  if ( ch->level - victim->level < -( FollowLevel )
    || ch->level - victim->level >  ( FollowLevel ) )
  {
    act( "$N不能加入$n的隊伍。", ch, NULL, victim, TO_NOTVICT );
    act( "你不能加入$n的隊伍。", ch, NULL, victim, TO_VICT    );
    act( "$N不能加入你的隊伍。", ch, NULL, victim, TO_CHAR    );
    RETURN_NULL();
  }

  victim->leader = ch;
  act( "$N加入$n的隊伍。", ch, NULL, victim, TO_NOTVICT );
  act( "你加入$n的隊伍。", ch, NULL, victim, TO_VICT    );
  act( "$N加入你的隊伍。", ch, NULL, victim, TO_CHAR    );
  RETURN_NULL();
}

FUNCTION( do_split )
{
  char        buf[MAX_STRING_LENGTH];
  char        arg[MAX_INPUT_LENGTH];
  CHAR_DATA * gch;
  int         members = 0;
  int         amount;
  int         share;
  int         extra;

  PUSH_FUNCTION( "do_split" );

  if ( ch->trade == FALSE )
  {
    send_to_char( "你的總資產太可怕了﹐所以無法執行這個指令﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( !IS_NPC( ch ) && ch->level < level_limit )
  {
    act( "在你升級到$i級前﹐請不要分錢給別人。"
      , ch, &level_limit, NULL, TO_CHAR );

    RETURN_NULL();
  }

  if ( ch->donate > 0 )
  {
    send_to_char( "在你領救濟金的這段時間﹐請不要裝富有﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( !ch->in_room )
  {
    send_to_char( "你現在都不知道在那裡耶﹗\n\r", ch );
    RETURN_NULL();
  }

  one_argument( argument, arg );

  if ( ( amount = atoi( arg ) ) < 0 )
  {
    send_to_char( "有點誠意吧﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( amount == 0 )
  {
    send_to_char( "你開出一張空頭支票﹐但沒人注意到你。\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->gold < amount )
  {
    send_to_char( "你身上沒有那麼多黃金。\n\r", ch );
    RETURN_NULL();
  }

  for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    if ( is_same_group( gch, ch ) ) members++;

  if ( members < 2 )
  {
    send_to_char( "你自己留著就好了。\n\r", ch );
    RETURN_NULL();
  }

  share = amount / members;
  extra = amount % members;

  if ( share == 0 )
  {
    send_to_char( "你的這些錢不夠拿來平分。\n\r", ch );
    RETURN_NULL();
  }

  gold_from_char( ch, amount );
  gold_to_char( ch, share + extra );

  sprintf( buf, "你拿出 %d 兩金子平分﹐你自己分到 %d 兩金子。\n\r",
      amount, share + extra );
  send_to_char( buf, ch );

  sprintf( buf, "$n拿出 %d 兩金子平分﹐你分到了 %d 兩金子。", amount, share );

  /* 取得房間有效人物 */
  for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
  {
    if ( gch != ch && is_same_group( gch, ch ) )
    {
      act( smash_act_keyword( buf, "nesNES" ), ch, NULL, gch, TO_VICT );
      gold_to_char( gch, share );
    }
  }

  RETURN_NULL();
}

FUNCTION( do_gtell )
{
  char        buf[MAX_STRING_LENGTH];
  char        buf1[MAX_STRING_LENGTH];
  CHAR_DATA * gch;

  PUSH_FUNCTION( "do_gtell" );

  if ( IS_SET( ch->act, PLR_NO_TELL ) )
  {
    send_to_char( "他現在聽不到你的話。\n\r", ch );
    RETURN_NULL();
  }

  /* 檢查閒聊保留字 */
  if ( check_chat_xname( ch, argument ) == TRUE ) RETURN_NULL();

  if ( check_repeat( ch , argument ) == TRUE )
  {
    send_to_char( "你不能告訴別人相同的話。\n\r" , ch );
    RETURN_NULL();
  }

  ansi_transcribe( argument, buf1 );
  sprintf( buf, "\e[1;33m%s\e[1;33m告訴隊上的人﹕「%s」。\e[0m\n\r"
    , mob_name( NULL, ch ), buf1 );

  for ( gch = char_list; gch; gch = gch->next )
  {
    if ( !verify_char( gch ) ) continue;
    if ( is_same_group( gch, ch ) ) send_to_char( buf, gch );
  }

  RETURN_NULL();
}

/* 運送物品給其他玩家或是怪物 */
FUNCTION( do_send )
{
  char        arg[MAX_INPUT_LENGTH];
  OBJ_DATA  * pObj;
  CHAR_DATA * pVictim;
  int         money;

  PUSH_FUNCTION( "do_send" );

  /* 只有玩家可以做這樣動作 */
  if ( IS_NPC( ch ) ) RETURN_NULL();

  if ( ch->position == POS_FIGHTING )
  {
    send_to_char( "先解決你的私人恩怨吧﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( !IS_NPC( ch ) && ch->level < level_limit )
  {
    act( "在你升級到$i級前﹐請不要亂送東西給別人。"
      , ch, &level_limit, NULL, TO_CHAR );

    RETURN_NULL();
  }

  if ( ch->donate > 0 )
  {
    send_to_char( "在你領救濟金的這段時間﹐請不要裝富有﹗\n\r", ch );
    RETURN_NULL();
  }

  /* 檢查是否有這樣物品 */
  argument = one_argument( argument, arg );

  if ( !( pObj = get_obj_carry( ch, arg ) ) )
  {
    send_to_char( "對不起﹐你身上沒有這樣物品。\n\r", ch );
    RETURN_NULL();
  }

  /* 物品是否有餵毒 */
  if ( poison_char( ch, pObj ) ) RETURN_NULL();

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

  if ( pObj->item_type == ITEM_TRASH )
  {
    send_to_char( "請不要亂丟垃圾好嗎﹖\n\r", ch );
    RETURN_NULL();
  }

  /* 找尋寄送的對象 */
  argument = one_argument( argument, arg );

  if ( !arg[0]
    || !( pVictim = get_char_world( ch, arg ) )
    || !pVictim->in_room
    || !can_see( ch, pVictim )
    || !can_see_mask( ch, pVictim ) )
  {
    send_to_char( "對不起﹐找不到你要寄送的對象。\n\r" , ch );
    RETURN_NULL();
  }

  /* 看看是否同一個人或是同一間房間 */
  if ( pVictim->in_room == ch->in_room )
  {
    act( "$N太近了﹐自己拿去就好了。", ch, NULL, pVictim, TO_CHAR );
    RETURN_NULL();
  }

  if ( is_pk( pVictim ) )
  {
    act( "$N正在武鬥大會中﹐請勿干擾﹗", ch, NULL, pVictim, TO_CHAR );
    RETURN_NULL();
  }

  if ( is_dead( pVictim ) )
  {
    act( "$N已經駕鶴西歸囉﹐可能送不到﹗", ch, NULL, pVictim, TO_CHAR );
    RETURN_NULL();
  }

  /* 檢查是否有運費這項 */
  one_argument( argument, arg );

  if ( !arg[0] || ( money = atoi( arg ) ) <= 0 )
  {
    send_to_char( "對不起﹐你必須註明運費。\n\r" , ch );
    RETURN_NULL();
  }

  /* 檢查是否運費是否足夠 */
  if ( money > ch->gold )
  {
    send_to_char( "你身上沒有足夠的錢來付你的運費。\n\r" , ch );
    RETURN_NULL();
  }

  /* 扣除運費 */
  gold_from_char( ch, money );
  obj_from_char( pObj );

  /* 運送失敗 */
  if ( number_range( 0, UMIN( 1000000, pObj->cost ) ) > money )
  {
    act( "對不起﹐因為最近土匪猖狂﹐你的$p被打劫了。", ch, pObj, NULL, TO_CHAR );
    extract_obj( pObj );
  }

  /* 運送成功 */
  else
  {
    ROOM_INDEX_DATA * pRoom;

    act( "你的$p已經運送至目的地了。", ch, pObj, NULL, TO_CHAR );
    obj_to_room( pObj, pVictim->in_room );

    pRoom = ch->in_room;
    char_from_room( ch );
    char_to_room( ch, pVictim->in_room );

    act( "$7叮咚……$A$3$n送給$N的東西已經到了。$A天上一隻大鵬"
         "鳥飛過就丟下所銜著的$p。$0", ch, pObj, pVictim, TO_ROOM );

    message_driver( ch, pObj, ACT_WHEN_SEND );

    char_from_room( ch );
    char_to_room( ch, pRoom );
  }

  RETURN_NULL();
}

bool is_same_group( CHAR_DATA * ach, CHAR_DATA * bch )
{
  PUSH_FUNCTION( "is_same_group" );
  if ( ach->leader ) ach = ach->leader;
  if ( bch->leader ) bch = bch->leader;
  RETURN( ach == bch );
}

/* 是否能夠離開 */
bool can_quit( CHAR_DATA * ch, bool fPrint )
{
  PUSH_FUNCTION( "can_quit" );

  if ( IS_NPC( ch ) ) RETURN( FALSE );

  if ( !ch || !verify_char( ch ) )
  {
    mudlog( LOG_DEBUG, "can_quit: 來源不正確." );
    RETURN( TRUE );
  }

  if ( ch->position == POS_FIGHTING )
  {
    if ( fPrint ) send_to_char( "想走﹖門都沒有﹐你還在戰鬥呢。\n\r", ch );
    RETURN( FALSE );
  }

  if ( ch->in_room && ch->in_room->NoQuit && ch->timer < MaxPlayingIdle )
  {
    if ( fPrint ) send_to_char( "先離開這個鬼地方再說吧﹗\n\r", ch );
    RETURN( FALSE );
  }

  if ( is_pk( ch ) )
  {
    if ( fPrint ) send_to_char( "你正在武鬥大會裡﹐無法脫離遊戲﹗\n\r", ch );
    RETURN( FALSE );
  }

  if ( is_dead( ch ) )
  {
    if ( fPrint ) send_to_char( "你目前無法離開這個時空。\n\r", ch );
    RETURN( FALSE );
  }

  if ( auction_info->seller == ch )
  {
    if ( fPrint ) send_to_char( "你還在賣東西耶﹐等賣完這個再走。\n\r" , ch );
    RETURN( FALSE );
  }

  if ( auction_info->buyer == ch )
  {
    if ( fPrint ) send_to_char( "你還在買東西耶﹐等買完這個再走。\n\r" , ch );
    RETURN( FALSE );
  }

  if ( is_station( ch->in_room ) )
  {
    if ( fPrint ) send_to_char( "你還在驛站裡﹐請先下車吧。\n\r", ch );
    RETURN( FALSE );
  }

  if ( ch->in_room->Sail )
  {
    if ( fPrint ) send_to_char( "你還在船上咧﹐請先下船吧﹗\n\r", ch );
    RETURN( FALSE );
  }

  RETURN( TRUE );
}

void extract_failcode( CHAR_DATA * ch )
{
  FAILCODE_DATA * zFailCode;
  FAILCODE_DATA * pFailCode;

  PUSH_FUNCTION( "extract_failcode" );

  if ( !ch || IS_NPC( ch ) )
  {
    mudlog( LOG_DEBUG, "extract_failcode: 來源不正確." );
    RETURN_NULL();
  }

  for ( pFailCode = ch->failcode; pFailCode; pFailCode = zFailCode )
  {
    zFailCode = pFailCode->next;

    if ( pFailCode->address ) free_string( pFailCode->address );
    free_struct( pFailCode, STRUCT_FAILCODE_DATA );
  }

  ch->failcode = NULL;
  RETURN_NULL();
}

void show_failcode( CHAR_DATA * ch )
{
  FAILCODE_DATA * pFail;
  struct tm     * sTime;
  time_t          timer;
  char            timebuf[MAX_STRING_LENGTH];
  char            buf1[MAX_INPUT_LENGTH];
  char            buf2[MAX_INPUT_LENGTH];
  char            buf3[MAX_INPUT_LENGTH];
  char            buf4[MAX_INPUT_LENGTH];
  char            buf5[MAX_INPUT_LENGTH];

  PUSH_FUNCTION( "show_failcode" );

  if ( !ch || IS_NPC( ch ) )
  {
    mudlog( LOG_DEBUG, "show_failcode: 來源不正確." );
    RETURN_NULL();
  }

  for ( clear_buffer(), pFail = ch->failcode; pFail; pFail = pFail->next )
  {
    timer = ( time_t ) pFail->timer;
    if ( !( sTime = localtime( &timer ) )
      || !pFail->address
      || !*pFail->address ) continue;

    chinese_number( sTime->tm_year - 11 , buf1 );
    chinese_number( sTime->tm_mon  +  1 , buf2 );
    chinese_number( sTime->tm_mday      , buf3 );
    chinese_number( sTime->tm_min       , buf5 );
    chinese_number( sTime->tm_hour > 12 ? sTime->tm_hour - 12 : sTime->tm_hour, buf4 );

    sprintf( timebuf, "民國%s年%s月%s號%s%s點%s分"
      , buf1, buf2, buf3, sTime->tm_hour < 12 ? "早上" : "下午", buf4, buf5 );

    send_to_buffer( "你上次在\e[1;32m%s\e[0m由\e[1;33m%s\e[0m連線時密碼錯誤﹗\n\r"
      , timebuf, pFail->address );
  }

  print_buffer( ch );
  RETURN_NULL();
}

FUNCTION( do_ask )
{
  int              count;
  int              cost;
  char             arg[MAX_INPUT_LENGTH];
  char             buf[MAX_STRING_LENGTH];
  CHAR_DATA      * victim;
  CHAR_DATA      * target;
  CHAR_DATA      * man;
  OBJ_DATA       * obj;
  AREA_DATA      * pArea;
  OBJ_INDEX_DATA * pIndex;

  PUSH_FUNCTION( "do_ask" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  if ( excess_filequota( ch ) )
  {
    send_to_char( "對不起﹐你的檔案太大﹐無法執行這個命令﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( !( pIndex = ObjLetter ) )
  {
    act( "對不起﹐$t沒有打工的功\能﹗", ch, mud_name, NULL, TO_CHAR );
    RETURN_NULL();
  }

  if ( ch->jail > 0 || ch->failed > 0 )
  {
    send_to_char( "對不起﹐目前你無法送信﹗\n\r", ch );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    for ( obj = ch->carrying; obj; obj = obj->next_content )
    {
      if ( obj->item_type == ITEM_LETTER
        && ( target = obj->address )
        && target->in_room
        && target->in_room->area )
      {
        act( "你身上有一封信﹐這封信要交給$N﹗", ch, NULL, target, TO_CHAR );
        RETURN_NULL();
      }
    }

    send_to_char( "你身上沒有要轉交的信件。\n\r", ch );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "!cancel" ) )
  {
    for ( obj = ch->carrying; obj; obj = obj->next_content )
      if ( obj->item_type == ITEM_LETTER
        && ( target = obj->address )
        && target->in_room
        && target->in_room->area ) break;

    if ( !obj )
    {
      send_to_char( "你並沒有在打工喔﹗\n\r", ch );
      RETURN_NULL();
    }

    else
    {
      act( "你把要給$N的信就地燒毀﹐心情不好﹐不送了﹗"
        , ch, NULL, obj->address, TO_CHAR );

      act( "$n把身上的$p用打火機燒毀﹐好像不願意去送信說﹗"
        , ch, obj, NULL, TO_ROOM );

      extract_obj( obj );
      RETURN_NULL();
    }
  }

  else
  {
    if ( !ch->in_room || !( pArea = ch->in_room->area ) )
    {
      send_to_char( "你現在在那個鬼地方呢﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( !IS_NPC( ch ) && ch->level < level_limit )
    {
      act( "在你升級到$t級前﹐是不能幫人送信。"
        , ch, &level_limit, NULL, TO_CHAR );

      RETURN_NULL();
    }

    if ( ch->donate > 0 )
    {
      send_to_char( "在你領救濟金的這段時間﹐請不要幫人送信了﹗\n\r", ch );
      RETURN_NULL();
    }

    for ( obj = ch->carrying; obj; obj = obj->next_content )
    {
      if ( obj->item_type == ITEM_LETTER
        && ( target = obj->address )
        && target->in_room
        && target->in_room->area )
      {
        act( "你身上已經有一封信﹐這封信麻煩轉交給$N﹗"
          , ch, NULL, target, TO_CHAR );

        RETURN_NULL();
      }
    }

    if ( ch->gold < ( cost = ch->level * AskCost ) )
    {
      act( "你必須先湊足$i兩保證金才能﹗", ch, &cost, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( !( victim = get_char_room( ch, arg ) ) )
    {
      act( "找不到你要請求任務的人 $2$T$0﹗", ch, NULL, arg, TO_CHAR );
      RETURN_NULL();
    }

    if ( !can_see( victim, ch ) )
    {
      act( "$N看不到你耶。", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( !IS_NPC( victim ) )
    {
      act( "$N不是非玩家﹐不能請求任務﹗\n\r", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( !IS_SET( victim->act, ACT_ASK ) )
    {
      act( "你要找$N請求任務﹐你是不是問錯人了﹖", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( victim->position != POS_STANDING )
    {
      act( "$N正在忙呢﹐等一下好了﹗", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    if ( is_dead( ch ) || is_dead( victim ) )
    {
      send_to_char( "現在可能不是請求任務的好時機喔﹗\n\r", ch );
      RETURN_NULL();
    }

    target = NULL;
    count  = 0;
    for ( man = char_list; man; man = man->next )
    {
      if ( !verify_char( man )
        || !IS_NPC( man )
        || man->level < 50
        || man->fighting
        || man->mount_by
        || man->boss
        || !man->pIndexData
        || man->pIndexData->count > 1
        || !man->in_room
        || man->in_room->area == pArea ) continue;

      if ( number_range( 0, count++ ) == 0 ) target = man;
    }

    if ( !target )
    {
      send_to_char( "ㄟ﹐目前我沒有信要你幫我送耶﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !( obj = create_object( ObjLetter, 1 ) ) )
    {
      send_to_char( "遭了﹐我的那封信呢﹖跑到那裡去了呢﹖\n\r", ch );
      RETURN_NULL();
    }

    obj->wear_loc = WEAR_NONE;
    obj->address  = target;
    obj->value[0] = 24 * 7;
    obj->cost     = cost;
    obj->NoSave   = TRUE;

    free_string( obj->description );
    sprintf( buf, "這是一%s%s﹐請你轉交給%s信件﹐請趕快送達吧﹗"
      , obj->unit
      , obj_name( ch, obj )
      , mob_name( ch, target ) );

    obj->description = str_dup( buf );

    sprintf( buf, "%s說道﹕「這裡有一%s%s﹐麻煩你轉交給%s﹐事關重大﹐"
      "事成之後﹐他會給你一筆酬勞的﹗」\n\r"
      , obj->unit
      , obj_name( ch, obj )
      , victim->byname
      , mob_name( ch, target ) );

    send_to_char( buf, ch );

    act( "$N跟$n竊竊私語﹐之後$N就交給$n一封$p﹗", ch, obj, victim, TO_ALL );

    obj_to_char( obj, ch );
    gold_from_char( ch, cost );
  }

  RETURN_NULL();
}

FUNCTION( do_joke )
{
  JOKE_DATA  * pJoke;
  JOKE_DATA  * zJoke;
  char         arg[MAX_INPUT_LENGTH];
  char         buf[MAX_STRING_LENGTH];
  int          count;
  int          loop;
  unsigned int max_number;

  PUSH_FUNCTION( "do_joke" );

  if ( IS_NPC( ch ) ) RETURN_NULL();

  if ( !joke_list )
  {
    act( "對不起﹐$t不提供笑話﹗", ch, mud_name, NULL, TO_CHAR );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' || !str_prefix( arg, "list" ) )
  {
    clear_buffer();
    send_to_buffer( "所有的笑話列表如下﹕" );

    for ( count = 0, pJoke = joke_list; pJoke; pJoke = pJoke->next )
    {
      if ( !pJoke->title ) continue;
      if ( ( count % 3 ) == 0 ) send_to_buffer( "\n\r" );

      str_cpy( buf, pJoke->title );
      buf[20] = '\x0';

      send_to_buffer( "\e[1;%dm%3d\e[0m. %-20s "
        , pJoke->stamp > ch->last_joke ? 36 : 32
        , ++count, pJoke->title );

      if ( buffer_full() ) break;
    }

    send_to_buffer( "\n\r" );
    print_buffer( ch );
  }

  else if ( !str_prefix( arg, "read" ) )
  {
    if ( !argument || !*argument || !is_number( argument ) )
    {
      send_to_char( "你要閱\讀哪一篇笑話呢﹖\n\r", ch );
      RETURN_NULL();
    }

    count = atoi( argument );
    for ( loop = 0, pJoke = joke_list; pJoke; pJoke = pJoke->next )
    {
      if ( ++loop == count )
      {
        show_joke( ch, pJoke );
        ch->last_joke = UMAX( ch->last_joke, pJoke->stamp );
        RETURN_NULL();
      }
    }

    send_to_char( "對不起﹐找不到那篇笑話耶﹗\n\r", ch );
  }

  else if ( !str_prefix( arg, "next" ) )
  {
    zJoke      = NULL;
    max_number = ( unsigned int ) -1;

    for ( pJoke = joke_list; pJoke; pJoke = pJoke->next )
    {
      if ( pJoke->stamp > ch->last_joke && max_number > pJoke->stamp )
      {
        zJoke      = pJoke;
        max_number = pJoke->stamp;
      }
    }

    if ( !zJoke )
    {
      send_to_char( "對不起﹐你已經閱\讀完這裡所有的笑話了﹗\n\r", ch );
      RETURN_NULL();
    }

    show_joke( ch, zJoke );
    ch->last_joke = UMAX( ch->last_joke, zJoke->stamp );
  }

  else if ( !str_prefix( arg, "random" ) )
  {
    zJoke = NULL;
    count = 0;
    for ( pJoke = joke_list; pJoke; pJoke = pJoke->next )
      if ( number_range( 0, count++ ) == 0 ) zJoke = pJoke;

    if ( !zJoke )
    {
      send_to_char( "對不起﹐找不到合適的笑話給你開心一下﹗\n\r", ch );
      RETURN_NULL();
    }

    show_joke( ch, zJoke );
  }

  else
  {
    send_to_char( "對不起﹐你的參數錯誤﹐請查詢 joke 的使用方法﹗\n\r", ch );
  }

  RETURN_NULL();
}

void show_joke( CHAR_DATA * ch, JOKE_DATA * pJoke )
{
  PUSH_FUNCTION( "show_joke" );

  if ( !ch || !pJoke )
  {
    mudlog( LOG_DEBUG, "show_joke: 缺乏來源." );
    RETURN_NULL();
  }

  print_to_char( ch,
    "標題﹕%s\n\r出處﹕%s\n\r%s%s%s"
    , pJoke->title, pJoke->org, VERTICAL_LINE
    , pJoke->text, VERTICAL_LINE );

  RETURN_NULL();
}

FUNCTION( do_notepad )
{
  char arg[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  int  loop;

  PUSH_FUNCTION( "do_notepad" );

  if ( IS_NPC( ch ) || !ch->desc || !ch->pcdata ) RETURN_NULL();

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    for ( clear_buffer() , loop = 0; loop < MAX_NOTEPAD; loop++ )
    {
      chinese_number( loop + 1 , buf );
      send_to_buffer( "你的第%s張記事本的狀態「%s」\n\r"
        , buf
        , ( !ch->pcdata->notepad[loop] || !*( ch->pcdata->notepad[loop] ) )
           ? "\e[1;34m空白中\e[0m" : "\e[1;32m記錄中\e[0m" );

       if ( buffer_full() ) break;
    }

    print_buffer( ch );
  }

  else if ( !str_prefix( arg, "show" ) )
  {
    if ( argument[0] == '\x0' || !is_number( argument ) )
    {
      send_to_char( "你要察看那個記事本呢﹖\n\r", ch );
      RETURN_NULL();
    }

    loop = atoi( argument ) - 1;
    if ( loop < 0 || loop >= MAX_NOTEPAD )
    {
      send_to_char( "你沒有那個記事本﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->pcdata->notepad[loop] == NULL
      || !*( ch->pcdata->notepad[loop] ) )
    {
      send_to_char( "那張記事本裡是空的﹐什麼也沒有﹗\n\r", ch );
      RETURN_NULL();
    }

    chinese_number( loop + 1, buf );
    print_to_char( ch, "你的第%s張記事本的內容為﹕\n\r%s%s\e[0m\n\r%s"
      , buf, VERTICAL_LINE, ch->pcdata->notepad[loop], VERTICAL_LINE );
  }

  else if ( !str_prefix( arg, "edit" ) )
  {
    for ( loop = 0; loop < MAX_NOTEPAD; loop++ )
    {
      if ( ch->pcdata->notepad[loop] && !*( ch->pcdata->notepad[loop] ) )
      {
        ch->desc->edit_mode = EDIT_NOTEPAD;
        show_editor( ch );

        free_string( ch->stack );
        ch->stack = str_dup( "" );
        RETURN_NULL();
      }
    }

    send_to_char( "對不起﹐你的記事本都記滿了東西﹗\n\r", ch );
    RETURN_NULL();
  }

  else if ( !str_prefix( arg, "delete" ) )
  {
    if ( argument[0] == '\x0' || !is_number( argument ) )
    {
      send_to_char( "你要刪除那個記事本呢﹖\n\r", ch );
      RETURN_NULL();
    }

    loop = atoi( argument ) - 1;
    if ( loop < 0 || loop >= MAX_NOTEPAD )
    {
      send_to_char( "你沒有那個記事本﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->pcdata->notepad[loop] == NULL
      || !*( ch->pcdata->notepad[loop] ) )
    {
      send_to_char( "你那張記事本本來就是空的﹗\n\r", ch );
      RETURN_NULL();
    }

    free_string( ch->pcdata->notepad[loop] );
    ch->pcdata->notepad[loop] = str_dup( "" );
    send_to_char( "你已經刪除那張記事本了﹗\n\r", ch );
  }

  else
  {
    send_to_char( "你的參數錯誤﹐請查詢 notepad 的用法﹗\n\r", ch );
  }

  RETURN_NULL();

}

FUNCTION( do_endow )
{
  char arg[MAX_INPUT_LENGTH];
  bool bEvil;
  int  gold;
  int  old;
  int  value;

  PUSH_FUNCTION( "endow" );

  if ( IS_NPC( ch ) || !ch->in_room ) RETURN_NULL();

  if ( ch->position != POS_STANDING )
  {
    send_to_char( "你先忙完你手邊的事情吧﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( ch->in_room->Fane )
  {
    bEvil = FALSE;
  }

  else if ( ch->in_room->Killer )
  {
    bEvil = TRUE;
  }

  else
  {
    send_to_char( "對不起﹐這裡不是寺廟或是八仙飯店﹗\n\r", ch );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' || !is_number( arg ) )
  {
    send_to_char( "你想要捐輸多少錢呢﹖\n\r", ch );
    RETURN_NULL();
  }

  if ( ( gold = atoi( arg ) ) <= 0 )
  {
    send_to_char( "你捐的是紙錢嗎﹖\n\r", ch );
    RETURN_NULL();
  }

  if ( gold > ch->gold )
  {
    send_to_char( "對不起﹐這裡不接受空頭支票﹗\n\r", ch );
    RETURN_NULL();
  }

  if ( EndowRate <= 0 || gold < EndowRate )
  {
    send_to_char( "有點誠意一點嗎﹐這麼一點錢拿幹什麼﹖\n\r", ch );
    RETURN_NULL();
  }

  old   = ch->alignment;
  value = number_range( 1, UMAX( 1, UMIN( 1000000, gold / EndowRate ) ) );
  gold_from_char( ch, gold );

  if ( bEvil )
  {
    ch->alignment = UMAX( -AlignRange, ch->alignment - value );

    act( "$n拿了$i兩銀子給此地的八仙飯店﹐他們用$n的錢做出更多的人肉叉燒包﹗"
      , ch, &gold, NULL, TO_ALL );

    if ( old == ch->alignment ) send_to_char( "你的陣營沒有改變﹗\n\r", ch );
    else send_to_char( "你的陣營變的更爛了﹗\n\r", ch );
  }

  else
  {
    ch->alignment = UMIN( AlignRange, ch->alignment + value );

    act( "$n捐了$i兩銀子給此地的寺廟﹐他們幫$n做了一場法事﹗"
      , ch, &gold, NULL, TO_ALL );

    if ( old == ch->alignment ) send_to_char( "你的陣營沒有改變﹗\n\r", ch );
    else send_to_char( "你的陣營變的更好了﹗\n\r", ch );
  }

  RETURN_NULL();
}

FUNCTION( do_donate )
{
  char arg[MAX_INPUT_LENGTH];
  char arg1[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  int  gold;
  int  number;

  PUSH_FUNCTION( "do_donate" );

  if ( IS_NPC( ch ) || !ch->in_room || is_affected( ch, SLOT_CHARM_PERSON ) )
    RETURN_NULL();

  if ( DonateLock && !IS_IMMORTAL( ch ) )
  {
    send_to_char( "對不起﹐目前這個功\能暫時不開啟﹗\n\r", ch );
    RETURN_NULL();
  }

  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    if ( DonateMoney > 0 )
    {
      act( "目前$2$t$0基金會總金額有$3$I$0兩銀子﹗"
        , ch, mud_name, &DonateMoney, TO_CHAR );

      if ( !can_donate( ch, FALSE ) )
      {
        send_to_char( "你目前不能領取救濟金﹗\n\r", ch );
      }
      else
      {
        act( "你目前可以到各地的錢莊領取救濟金$i兩銀子﹗"
          , ch, &DonateBenifit, NULL, TO_CHAR );
      }
    }

    else
    {
      act( "目前$2$t$0基金會裡沒有任何捐款﹐請發揮你的善心﹐請捐點錢吧﹗"
        , ch, mud_name, NULL, TO_CHAR );
    }

    if ( ch->donate > 0 )
      send_to_char( "你目前還在救濟時間內﹐請善用你的金錢﹗\n\r", ch );
  }

  else if ( !str_prefix( arg, "gold" ) )
  {
    if ( DonateLock )
    {
      send_to_char( "這個功\能在鎖定時不能執行﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( IS_IMMORTAL( ch ) )
    {
      send_to_char( "你只能監督基金會﹐但不能捐款﹗\n\r", ch );
      RETURN_NULL();
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\x0' || !is_number( arg ) )
    {
      act( "你要捐多少錢給$2$t$0基金會呢﹖", ch, mud_name, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( !IS_NPC( ch ) && ch->level < level_limit )
    {
      act( "在你升級到$i級前﹐請不要捐錢。", ch, &level_limit, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( ch->donate > 0 )
    {
      send_to_char( "剛剛才領了救濟金﹐怎麼﹐現在就回饋了啊﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( ( gold = atoi( arg ) ) <= 0 )
    {
      send_to_char( "你要捐的是紙錢嗎﹖\n\r", ch );
      RETURN_NULL();
    }

    if ( gold > ch->gold )
    {
      send_to_char( "捐錢是很好﹐但也要看看自己的荷包﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( DonateMoney + gold <= 0 )
    {
      send_to_char( "由於你的捐贈﹐基金會可能會倒閉﹗\n\r", ch );
      RETURN_NULL();
    }

    DonateMoney += gold;
    gold_from_char( ch, gold );

    act( "謝謝你﹐你捐了$2$i$0兩銀子﹐目前基金會裡有$2$I$0兩銀子﹗"
      , ch, &gold, &DonateMoney, TO_CHAR );

    sprintf( buf, "感謝%s捐了 %d 兩銀子給%s基金會\e[0m﹗"
      , mob_name( NULL, ch ), gold, mud_name );

    talk_channel_2( buf, CHANNEL_BULLETIN, "捐錢" );

    refresh_donate();
  }

  else if ( !str_prefix( arg, "withdraw" ) )
  {
    if ( DonateLock )
    {
      send_to_char( "這個功\能在鎖定時不能執行﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( ch->in_room->DepositMoney )
    {
      send_to_char( "這裡並不是錢莊喔﹗\n\r" , ch );
      RETURN_NULL();
    }

    if ( !can_donate( ch, TRUE ) ) RETURN_NULL();

    gold_to_char( ch, DonateBenifit );
    DonateMoney -= DonateBenifit;
    ch->donate   = DonateDuration;

    act( "$n從$t基金會領了$I兩的救濟金﹐希望$n能好好利用"
      , ch, mud_name, &DonateBenifit, TO_ALL );

    refresh_donate();
  }

  else if ( !str_prefix( arg, "lock" ) && IS_IMMORTAL( ch ) )
  {
    DonateLock = !DonateLock;
    act( "你$1$t$0$T基金會的功\能﹗"
      , ch, DonateLock ? "關閉" : "開啟", mud_name, TO_CHAR );
  }

  else if ( !str_prefix( arg, "set" ) && IS_IMMORTAL( ch ) )
  {
    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg1 );

    if ( arg[0] == '\x0' || arg1[0] == '\x0' )
    {
      send_to_char( "你的參數錯誤﹐請查詢 donate 的正確使用方法﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !is_number( arg1 ) )
    {
      send_to_char( "你的第二個參數必須是數字﹗\n\r", ch );
      RETURN_NULL();
    }

    number = atoi( arg1 );

    if ( !str_prefix( arg, "duration" ) )
    {
      if ( number < 0 )
      {
        send_to_char( "你的間隔時間不合法﹗\n\r", ch );
        RETURN_NULL();
      }

      DonateDuration = number;
      refresh_donate();
      act( "你把基金會的間隔改為$i小時﹗", ch, &DonateDuration, NULL, TO_CHAR );
    }

    else if ( !str_prefix( arg, "benifit" ) )
    {
      if ( number < 0 )
      {
        send_to_char( "你的救濟金金額不合法﹗\n\r", ch );
        RETURN_NULL();
      }

      DonateBenifit = number;
      refresh_donate();
      act( "你把基金會的救濟金改為$i兩﹗", ch, &DonateBenifit, NULL, TO_CHAR );
    }

    else if ( !str_prefix( arg, "level" ) )
    {
      if ( number < 0 || number > LEVEL_HERO )
      {
        send_to_char( "你的救濟金等級不合法﹗\n\r", ch );
        RETURN_NULL();
      }

      DonateLevel = number;
      refresh_donate();
      act( "你把基金會的等級改為$i級﹗", ch, &DonateLevel, NULL, TO_CHAR );
    }

   else if ( !str_prefix( arg, "limit" ) )
    {
      if ( number < 0 )
      {
        send_to_char( "你的救濟金範圍不合法﹗\n\r", ch );
        RETURN_NULL();
      }

      DonateLimit = number;
      refresh_donate();
      act( "你把基金會的救濟範圍改為$i兩﹗", ch, &DonateLimit, NULL, TO_CHAR );
    }

    else
    {
      send_to_char( "你的參數錯誤﹐請查詢 donate 的正確使用方法﹗\n\r", ch );
    }
  }

  else if ( !str_prefix( arg, "setting" ) && IS_IMMORTAL( ch ) )
  {
    print_to_char( ch,
      "%s\e[0m基金會狀況如下﹕\n\r%s"
      "基金會總金額  ﹕ %d 兩\n\r"
      "每次發放金額  ﹕ %d 兩銀子\n\r"
      "發放最低等級  ﹕ %d 級以下。\n\r"
      "對象總資產限制﹕ %d 兩以下。\n\r"
      "發放間隔時間  ﹕ %d 小時。\n\r"
      "被救濟總次數  ﹕ %d 次。\n\r"
      , mud_name
      , VERTICAL_LINE
      , DonateMoney
      , DonateBenifit
      , DonateLevel
      , DonateLimit
      , DonateDuration
      , DonateCount );
  }

  else
  {
    send_to_char( "你的參數錯誤﹐請查詢 donate 的正確使用方法﹗\n\r", ch );
  }
  RETURN_NULL();
}

void refresh_donate( void )
{
  FILE * pFile;

  PUSH_FUNCTION( "refresh_donate" );

  if ( ( pFile = FOPEN( donate_file, "w" ) ) )
  {
    fprintf( pFile, "Money           %d\n"
                    "Benifit         %d\n"
                    "Level           %d\n"
                    "Limit           %d\n"
                    "Duration        %d\n"
                    "Count           %d\n"
                    "End\n"
      , DonateMoney, DonateBenifit, DonateLevel
      , DonateLimit, DonateDuration, DonateCount );

    FCLOSE( pFile );
  }

  else
  {
    mudlog( LOG_DEBUG, "refresh_donate: 更新捐贈檔案失敗." );
  }

  RETURN_NULL();
}

bool can_donate( CHAR_DATA * ch, bool fShow )
{
  PUSH_FUNCTION( "can_donate" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "can_donate: 缺乏來源." );
    RETURN( FALSE );
  }

  if ( IS_NPC( ch ) ) RETURN( FALSE );

  if ( IS_IMMORTAL( ch ) )
  {
    if ( fShow ) send_to_char( "你只能監督基金會﹐但不能領救濟金﹗\n\r", ch );
    RETURN( FALSE );
  }

  if ( ch->level > DonateLevel )
  {
    if ( fShow ) act( "對不起﹐你的高於$i級﹐無法繼續領取救濟金﹗"
      , ch, &DonateLevel, NULL, TO_CHAR );
    RETURN( FALSE );
  }

  if ( DonateMoney < DonateBenifit )
  {
    if ( fShow ) act( "對不起﹐$t基金會裡沒有錢了﹐無法繼續救濟﹗"
      , ch, mud_name, NULL, TO_CHAR );

      RETURN( FALSE );
  }

  if ( ch->donate > 0 )
  {
    if ( fShow ) send_to_char( "剛剛才領完救濟金﹐怎麼現在又來領了呢﹖\n\r", ch );
    RETURN( FALSE );
  }

  if ( personal_asset( ch ) >= DonateLimit )
  {
    if ( fShow ) send_to_char( "對不起﹐你不適用於救濟的範圍內﹗\n\r", ch );
    RETURN( FALSE );
  }

  RETURN( TRUE );
}

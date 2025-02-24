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
#include <sys/stat.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "merc.h"
#include "directory.h"

#if !defined(macintosh)
extern  int     _filbuf         args( ( FILE * ) );
#endif

#define MAX_NEST        100
static  OBJ_DATA *      rgObjNest       [MAX_NEST];

#define SAVE_OBJ        1
#define DEPOSIT_OBJ     2
#define SAVE_CORPSE     3

/* 區域函數 */
void    fwrite_char     args( ( CHAR_DATA *, FILE * ) );
void    fwrite_obj      args( ( CHAR_DATA *, OBJ_DATA *, FILE *, int , int ) );
bool    fread_char      args( ( CHAR_DATA *, FILE_DATA * ) );
bool    fread_failcode     args( ( CHAR_DATA *, FILE_DATA * ) );
bool    fread_obj       args( ( CHAR_DATA *, FILE_DATA *, int ) );
void    fprint_string   args( ( FILE *, char *, const char * ) );
void    fprint_word     args( ( FILE *, char *, const char * ) );
void    fprint_const    args( ( FILE *, char *, const char * ) );
void    fprint_number   args( ( FILE *, char *, int ) );
void    create_dir      args( ( const char * ) );
void    save_basic_file args( ( CHAR_DATA * ) );

char * initial( const char * str )
{
  static char strint[ MAX_STRING_LENGTH ];

  PUSH_FUNCTION( "initial" );
  strint[0] = LOWER( str[ 0 ] );
  RETURN( strint );
}

void create_dir( const char * name )
{
  struct stat file_stat;

  char pathname[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "create_dir" );

  if ( !name || !*name )
  {
    mudlog( LOG_DEBUG, "create_name: 來源不正確." );
    RETURN_NULL();
  }

  sprintf( pathname, "%s/%s/%s"
    , player_dir, initial( name ), normalize( name ) );

  if ( stat( pathname, &file_stat ) != 0 )
  {
    if ( mkdir( pathname, S_IRWXU | S_IRWXG ) != 0 )
      mudlog( LOG_DEBUG, "create_dir: 無法建立目錄 %s.", name );

    RETURN_NULL();
  }

  if ( !S_ISDIR( file_stat.st_mode ) )
    mudlog( LOG_DEBUG, "create_dir: %s 已經建立, 但不是目錄.", name );

  RETURN_NULL();
}

char * file_name( const char * name, int type )
{
  static char filename[ MAX_STRING_LENGTH ];

  PUSH_FUNCTION( "file_name" );

  if ( !name || !*name )
  {
    mudlog( LOG_DEBUG, "file_name: 來源不正確." );
    filename[0] = '\x0';
    RETURN( filename );
  }

  switch( type )
  {
  default :
    filename[0] = '\x0';
    mudlog( LOG_DEBUG, "file_name: 錯誤的模式 %d.", type );
    break;

  case SAVE_FILE:
    sprintf( filename, "%s/%s/%s/data"
      , player_dir, initial( name ), normalize( name ) );
    break;

  case BACKUP_FILE:
    sprintf( filename, "%s/%s/%s/backup"
      , player_dir, initial( name ), normalize( name ) );
    break;

  case FAILCODE_FILE:
    sprintf( filename, "%s/%s/%s/failcode"
      , player_dir, initial( name ), normalize( name ) );
    break;

  case BASIC_FILE:
    sprintf( filename, "%s/%s/%s/basic"
      , player_dir, initial( name ), normalize( name ) );
    break;

  case LOG_FILE:
    sprintf( filename, "%s/%s/%s/log"
      , player_dir, initial( name ), normalize( name ) );
    break;

  }

  RETURN( filename );
}

void save_char_obj( CHAR_DATA * ch , int type )
{
  char   strsave[MAX_FILE_LENGTH];
  FILE * pFile;

  PUSH_FUNCTION( "save_char_obj" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "save_char_obj: 來源不正確." );
    RETURN_NULL();
  }

  if ( IS_NPC( ch )
    || ( ch->desc && ch->desc->connected != CON_PLAYING )
    || ch->level <= 0 )
      RETURN_NULL();

  ch->save_time = current_time;
  str_cpy( strsave, file_name( ch->name , type ) );

  create_dir( ch->name );

  if ( !( pFile = FOPEN( strsave, "w" ) ) )
  {
    mudlog( LOG_DEBUG , "Save_char_obj: 開啟檔案 %s 錯誤.", ch->name );
  }

  else
  {
    fwrite_char( ch, pFile );
    if ( ch->carrying ) fwrite_obj( ch, ch->carrying, pFile, 0, SAVE_OBJ    );
    if ( ch->deposit  ) fwrite_obj( ch, ch->deposit , pFile, 0, DEPOSIT_OBJ );
    fprintf( pFile , "#END\n" );
    FCLOSE( pFile );

    /* 改變檔案存取模式 */
    set_file_mode( strsave );

    /* 讀取檔案大小 */
    char_filesize( ch );

    save_basic_file( ch );
  }

  RETURN_NULL();
}

void fwrite_char( CHAR_DATA * ch, FILE * fp )
{
  AFFECT_DATA * paf;
  ALIAS_DATA  * temp;
  ENABLE_DATA * enable;
  SKILL_DATA  * pSkill;
  QUEST_DATA  * pQuest;
  ENEMY_DATA  * pEnemy;
  FRIEND_DATA * pFriend;
  TRACE_DATA  * pTrace;
  STAMP_DATA  * pStamp;
  IP_DATA     * pIp;
  int           sn;
  int           loop;

  PUSH_FUNCTION( "fwrite_char" );

  fprintf( fp, "#PLAYER\n" );
  fprint_string( fp, "Name" , ch->name  );
  fprint_string( fp, "Cname", ch->cname );
  fprint_number( fp, "Room"
    , save_room( ( ch->in_room == RoomLimbo && ch->was_in_room )
      ? ch->was_in_room : ch->in_room ) );

  /* 若不正常斷線 */
  fprint_string( fp, "LastHost"
    , ( ch->desc ) ? ch->desc->host : "不正常斷線" );

  fprint_string( fp, "ShortDescr" , ch->byname      );
  fprint_string( fp, "LongDescr"  , ch->long_descr  );
  fprint_string( fp, "Description", ch->description );
  fprint_string( fp, "Prompt"     , ch->prompt      );
  fprint_string( fp, "Email"      , ch->email       );
  fprint_string( fp, "Class"      ,
    ch->class ? ch->class->name : class_demos->name );

  fprint_string( fp, "Password"   , ch->pcdata->pwd        );
  fprint_string( fp, "Bamfin"     , ch->pcdata->bamfin     );
  fprint_string( fp, "Bamfout"    , ch->pcdata->bamfout    );
  fprint_number( fp, "Contract"   , ch->pcdata->contract   );
  fprint_number( fp, "SerialHigh" , ch->serial.high        );
  fprint_number( fp, "SerialLow"  , ch->serial.low         );

  fprint_string( fp, "Mate"       , ch->pcdata->mater      );

  if ( ch->pcdata->couple.high > 0 && ch->pcdata->couple.low > 0 )
  {
    fprint_number( fp, "CoupleHigh", ch->pcdata->couple.high );
    fprint_number( fp, "CoupleLow" , ch->pcdata->couple.low  );
  }

  fprint_string( fp, "Title"      , ch->pcdata->title       );
  fprint_number( fp, "Deny"       , ch->pcdata->deny        );
  fprint_number( fp, "Suspect"    , ch->pcdata->suspect     );
  fprint_number( fp, "PKWin"      , ch->pcdata->pkwin       );
  fprint_number( fp, "PKLost"     , ch->pcdata->pklost      );
  fprint_number( fp, "PlayWin"    , ch->pcdata->fightwin    );
  fprint_number( fp, "PlayLost"   , ch->pcdata->fightlost   );
  fprint_number( fp, "Kills"      , ch->pcdata->kills       );
  fprint_number( fp, "KillLevel"  , ch->pcdata->kill_level  );
  fprint_number( fp, "Imprison"   , ch->pcdata->imprison    );
  fprint_number( fp, "FightWin"   , ch->pcdata->fight_win   );
  fprint_number( fp, "FightLost"  , ch->pcdata->fight_lost  );
  fprint_number( fp, "Rechristen" , ch->pcdata->rechristen  );
  fprint_number( fp, "Xname"      , ch->pcdata->xname       );
  fprint_number( fp, "Steal"      , ch->pcdata->steal       );
  fprint_number( fp, "Murder"     , ch->pcdata->murder      );
  fprint_number( fp, "Pills"      , ch->pcdata->pills       );
  fprint_number( fp, "Dosage"     , ch->pcdata->dosage      );
  fprint_number( fp, "Ply"        , ch->pcdata->ply         );
  fprint_number( fp, "StPanity"  , ch->pcdata->steal_panity );
  fprint_number( fp, "Dormancy"   , ch->pcdata->dormancy    );

  fprint_number( fp, "ClubLog"    , ch->clublog            );
  fprint_number( fp, "Firman"     , ch->firman             );
  fprint_number( fp, "Jail"       , ch->jail               );
  fprint_number( fp, "Fail"       , ch->failed             );
  fprint_number( fp, "Sex"        , ch->sex                );
  fprint_number( fp, "Level"      , ch->level              );
  fprint_number( fp, "Trust"      , ch->trust              );
  fprint_number( fp, "Adviser"    , ch->adviser            );

  /* 紀錄追蹤路徑 */
  for ( loop = 0; loop < MAX_TRACE; loop++ )
  {
    if ( !( pTrace = ch->pcdata->map[loop] ) ) continue;

    fprintf( fp, "AutoMap         %d %d %d"
      , loop, pTrace->starting, pTrace->ending );

    for ( sn = 0; sn < MAX_PATH; sn++ )
    {
      if ( pTrace->path[sn] == ERRORCODE ) break;
      fprintf( fp, " %d", pTrace->path[sn] );
    }

    fprintf( fp, "\n" );
  }

  fprint_number( fp, "Played",
    (int) ( ch->played ) + ( (int) current_time - (int) ch->logon) );

  if ( ch->home >= 0 ) fprint_number( fp, "Home", ch->home );

  fprint_number( fp, "EqCapcity", ch->eq_capcity );

  fprint_number( fp, "Note"      , ( int ) ch->last_note     );
  fprint_number( fp, "Joke"      , ( int ) ch->last_joke     );
  fprint_number( fp, "ClubNote"  , ( int ) ch->last_clubnote );

  fprint_number( fp, "Hit"       , ch->hit               );
  fprint_number( fp, "HitMax"    , ch->max_hit           );
  fprint_number( fp, "HItMod"    , ch->mod_hit           );
  fprint_number( fp, "Mana"      , ch->mana              );
  fprint_number( fp, "ManaMax"   , ch->max_mana          );
  fprint_number( fp, "ManaMod"   , ch->mod_mana          );
  fprint_number( fp, "Move"      , ch->move              );
  fprint_number( fp, "MoveMax"   , ch->max_move          );
  fprint_number( fp, "MoveMod"   , ch->mod_move          );
  fprint_number( fp, "Limit"     , ch->limit             );
  fprint_number( fp, "Gold"      , ch->gold              );
  fprint_number( fp, "Bank"      , ch->bank              );
  fprint_number( fp, "Donate"    , ch->donate            );
  fprint_number( fp, "Exp"       , ch->exp               );
  fprint_number( fp, "Act"       , ch->act               );
  fprint_number( fp, "WizFlags"  , ch->wizflags          );
  fprint_number( fp, "Tribunal"  , ch->tribunal_timer    );
  fprint_number( fp ,"Turn"      , ch->turn              );
  fprint_number( fp ,"Notify"    , ch->notify            );

  fprint_number( fp, "Position"
    , ch->position == POS_FIGHTING ? POS_STANDING : ch->position );

  if ( is_affected( ch, SLOT_MASK ) && get_mob_index( ch->mask ) )
  {
    fprint_number( fp, "Mask", ch->mask );
  }

  fprint_number( fp, "Practice"   , ch->practice        );
  fprint_number( fp, "SavingThrow", ch->saving_throw    );
  fprint_number( fp, "Alignment"  , ch->alignment       );
  fprint_number( fp, "Hitroll"    , ch->hitroll         );
  fprint_number( fp, "Damroll"    , ch->damroll         );
  fprint_number( fp, "Armor"      , ch->armor           );
  fprint_number( fp, "Wimpy"      , ch->wimpy           );
  fprint_number( fp, "Deaf"       , ch->deaf            );

  /* 將修飾值歸零 */
  if ( ch->pcdata )
  {
    ch->pcdata->mod_str = 0;
    ch->pcdata->mod_int = 0;
    ch->pcdata->mod_wis = 0;
    ch->pcdata->mod_dex = 0;
    ch->pcdata->mod_con = 0;
  }

  fprint_number( fp, "Str"        , ch->now_str         );
  fprint_number( fp, "StrMod"     , ch->pcdata->mod_str );
  fprint_number( fp, "Int"        , ch->now_int         );
  fprint_number( fp, "IntMod"     , ch->pcdata->mod_int );
  fprint_number( fp, "Wis"        , ch->now_wis         );
  fprint_number( fp, "WisMod"     , ch->pcdata->mod_wis );
  fprint_number( fp, "Dex"        , ch->now_dex         );
  fprint_number( fp, "DexMod"     , ch->pcdata->mod_dex );
  fprint_number( fp, "Con"        , ch->now_con         );
  fprint_number( fp, "ConMod"     , ch->pcdata->mod_con );

  /* 再重新計算 */
  affect_recount( ch );

  fprint_number( fp, "Drunk"      , ch->pcdata->condition[0] );
  fprint_number( fp, "Full"       , ch->pcdata->condition[1] );
  fprint_number( fp, "Thirst"     , ch->pcdata->condition[2] );
  fprint_number( fp, "Pagelen"    , ch->pcdata->pagelen );
  fprint_number( fp, "Angel"      , ch->pcdata->angel_set);
  fprint_number( fp, "NSkill"     , ch->nskill          );

  /* 記錄傳送點 */
  if ( ch->pcdata )
  {
    fprintf( fp, str_space( "Recall" , BLANK_LENGTH ) );

    for ( loop = 0; loop < MAX_RECALL; loop++ )
      fprintf( fp ,"%-5d ", ch->pcdata->recall[loop] );

    fprintf( fp ,"\n" );
  }

  for ( pEnemy = ch->enemy; pEnemy; pEnemy = pEnemy->next )
    fprintf( fp, "%s%s~\n", str_space( "Enemy", BLANK_LENGTH ), pEnemy->city );

  for ( pStamp = ch->stamp; pStamp; pStamp = pStamp->next )
    fprintf( fp, "%s%d\n", str_space( "Stamp", BLANK_LENGTH ), pStamp->number );

  for ( pQuest = ch->quest; pQuest; pQuest = pQuest->next )
  {
    if ( !pQuest->link ) continue;
    fprintf( fp, "%s%s~\n", str_space( "Quest", BLANK_LENGTH )
     , pQuest->link->mark );
  }

  for ( pFriend = ch->friend; pFriend; pFriend = pFriend->next )
  {
    if ( is_exist( pFriend->name ) )
      fprintf( fp, "%s%s~\n", str_space( "Friend", BLANK_LENGTH )
        , pFriend->name );
  }

  for ( pFriend = ch->badfriend; pFriend; pFriend = pFriend->next )
  {
    if ( is_exist( pFriend->name ) )
      fprintf( fp, "%s%s~\n", str_space( "BadFriend", BLANK_LENGTH )
        , pFriend->name );
  }

  for ( temp = ch->alias; temp; temp = temp->next )
  {
    fprintf( fp, "%s%s~ %s~\n"
      , str_space( "Alias", BLANK_LENGTH ), temp->text, temp->macro );
  }

  for ( loop = 0; loop < MAX_NOTEPAD; loop++ )
  {
    if ( ch->pcdata->notepad[loop] == NULL
      || ch->pcdata->notepad[loop][0] == '\x0' )
      continue;

    fprintf( fp, "%s %2d %s~\n"
      , str_space( "NotePad", BLANK_LENGTH )
      , loop, ch->pcdata->notepad[loop] );
  }

  for ( pIp = ch->connect; pIp; pIp = pIp->next )
  {
    fprintf( fp, "%s%s~\n"
      , str_space( "LimitIP", BLANK_LENGTH ), pIp->address );
  }

  for ( enable = ch->enable; enable; enable = enable->next )
  {
    if ( enable->skill )
      fprint_word( fp, "Enable" , enable->skill->name );
  }


  if ( ch->pcdata )
  {
    /* 把股票寫入玩家檔案 */
    for ( sn = 0; sn < MAX_STOCK; sn++ )
    {
      if ( ch->pcdata->stock[sn] > 0 && stock_data[sn].name )
      {
        fprintf( fp, "%s%d %d %d\n"
          , str_space( "Stocks", BLANK_LENGTH )
          , sn + 1, ch->pcdata->stock[sn], ch->pcdata->asset[sn] );
      }
    }

    /* 把版面讀取記錄寫到檔案 */
    for ( sn = 0; sn < max_board; sn++ )
    {
      if ( ch->pcdata->board[sn] > 0 && board_lookup( sn ) )
        fprintf( fp, "%s%d %d\n", str_space( "Board", BLANK_LENGTH )
          , sn, ch->pcdata->board[sn] );
    }

    if ( ch->pcdata->autosave > 0 )
      fprint_number( fp, "AutoSave"  , ch->pcdata->autosave   );

    if ( ch->pcdata->autobackup > 0 )
      fprint_number( fp, "AutoBackup", ch->pcdata->autobackup );
  }

  /* 把技能寫入玩家的檔案之中 */
  for ( sn = 0; sn < MAX_SKILL; sn++ )
  {
    if ( ch->skill[sn] > 0 && ( ( pSkill = get_skill(sn) ) != NULL ) )
    {
      fprintf( fp, "%s%d '%s'\n"
        , str_space( "Learn" , BLANK_LENGTH )
        , ch->skill[sn], pSkill->name );
    }
  }

  /*
  for ( paf = ch->affected; paf; paf = paf->next )
  {
    fprintf( fp, "%s%3d %3d %3d %3d %10d\n"
      , str_space( "Affect" , BLANK_LENGTH )
      , paf->type, paf->duration, paf->modifier
      , paf->location, paf->bitvector );
  }
  */

  for ( paf = ch->affected; paf; paf = paf->next )
  {
    if ( !( pSkill = get_skill( paf->type ) ) ) continue;
    fprintf( fp, "%s'%s' %4d %4d %4d\n"
      , str_space( "Affected", BLANK_LENGTH )
      , pSkill->name
      , paf->duration
      , paf->modifier
      , paf->location );
  }

  fprintf( fp, "End\n\n" );
  RETURN_NULL();
}

/* 寫入物品 */
void fwrite_obj( CHAR_DATA * ch, OBJ_DATA * obj, FILE * fp
                , int iNest, int type )
{
  int                loop;
  EXTRA_DESCR_DATA * ed;
  AFFECT_DATA      * paf;
  CACHET_DATA      * pCachet;
  SKILL_DATA       * pSkill;
  char               Str1[MAX_INPUT_LENGTH];

  PUSH_FUNCTION( "fwrite_obj" );

  if ( obj->next_content )
    fwrite_obj( ch, obj->next_content, fp, iNest , type );

  if ( ( ch->level + 10 ) < obj->level
     || obj->item_type == ITEM_KEY
     || obj->item_type == ITEM_POTION
     || obj->NoSave )
     RETURN_NULL();

  set_serial( &obj->serial );

  switch ( type )
  {
  default :
    fprintf( fp, "#UNKNOWN\n" );
    break;

  case SAVE_OBJ:
    fprintf( fp , "#OBJECT\n" );
    break;

  case DEPOSIT_OBJ:
    fprintf( fp , "#DEPOSIT\n" );
    break;

  case SAVE_CORPSE:
    fprintf( fp , "#CORPSE\n" );
    break;
  }

  fprint_number( fp, "Nest"       , iNest                 );
  fprint_number( fp, "Vnum"       , obj->pIndexData->vnum );
  fprint_string( fp, "Name"       , obj->name             );
  fprint_string( fp, "ShortDescr" , obj->cname            );
  fprint_string( fp, "Description", obj->description      );

  str_cpy( Str1, symbol_string( TRUE, "邏輯常數" ) );

  if ( obj->Takeable    ) fprintf( fp, "Takeable        %s\n", Str1 );
  if ( obj->Glow        ) fprintf( fp, "Glow            %s\n", Str1 );
  if ( obj->Hum         ) fprintf( fp, "Hum             %s\n", Str1 );
  if ( obj->Dark        ) fprintf( fp, "Dark            %s\n", Str1 );
  if ( obj->Lock        ) fprintf( fp, "Lock            %s\n", Str1 );
  if ( obj->Evil        ) fprintf( fp, "Evil            %s\n", Str1 );
  if ( obj->Invis       ) fprintf( fp, "Invis           %s\n", Str1 );
  if ( obj->Magic       ) fprintf( fp, "Magic           %s\n", Str1 );
  if ( obj->NoDrop      ) fprintf( fp, "NoDrop          %s\n", Str1 );
  if ( obj->Bless       ) fprintf( fp, "Bless           %s\n", Str1 );
  if ( obj->AntiGood    ) fprintf( fp, "AntiGood        %s\n", Str1 );
  if ( obj->AntiEvil    ) fprintf( fp, "AntiEvil        %s\n", Str1 );
  if ( obj->AntiNeutral ) fprintf( fp, "AntiNeutral     %s\n", Str1 );
  if ( obj->NoRemove    ) fprintf( fp, "NoRemove        %s\n", Str1 );
  if ( obj->Inventory   ) fprintf( fp, "Inventory       %s\n", Str1 );
  if ( obj->Cancachet   ) fprintf( fp, "Cancachet       %s\n", Str1 );
  if ( obj->NoSave      ) fprintf( fp, "NoSave          %s\n", Str1 );
  if ( obj->Contraband  ) fprintf( fp, "Contraband      %s\n", Str1 );
  if ( obj->Loadable    ) fprintf( fp, "Loadable        %s\n", Str1 );

  fprint_number( fp, "WearFlags"  , obj->wear_flags       );
  fprint_number( fp, "WearLoc"    , obj->wear_loc         );
  fprint_number( fp, "ItemType"   , obj->item_type        );
  fprint_number( fp, "Weight"     , obj->weight           );
  fprint_number( fp, "Level"      , obj->level            );
  fprint_number( fp, "SerialHigh" , obj->serial.high      );
  fprint_number( fp, "SerialLow"  , obj->serial.low       );
  fprint_number( fp, "Timer"      , obj->timer            );
  fprint_number( fp, "Cost"       , obj->cost             );

  if ( obj->owner.high > 0 && obj->owner.low > 0 )
  {
    fprint_number( fp, "OwnerHigh" , obj->owner.high );
    fprint_number( fp, "OwnerLow"  , obj->owner.low  );
  }

  /* 是否有毒 */
  if ( obj->venom > 0 ) fprint_number( fp, "Venom", obj->venom );

  if ( obj->max_armor > 0 )
  {
    fprint_number( fp, "MaxArmor"   , obj->max_armor        );
    fprint_number( fp, "Armor"      , obj->armor            );
  }

  fprintf( fp, str_space( "Values" , BLANK_LENGTH ) );
  for ( loop = 0; loop < MAX_OBJECT_VALUE; loop++ )
      fprintf( fp ,"%d ", obj->value[loop] );
  fprintf( fp ,"\n" );

  for ( pCachet = obj->cachet; pCachet; pCachet = pCachet->next )
  {
    fprintf( fp, "%s%d\n"
      , str_space( "Cachet", BLANK_LENGTH ), pCachet->vnum );
  }

  for ( paf = obj->affected; paf; paf = paf->next )
  {
    if ( !( pSkill = get_skill( paf->type ) ) ) continue;
    fprintf( fp, "%s'%s' %4d %4d %4d\n"
      , str_space( "Affected", BLANK_LENGTH )
      , pSkill->name
      , paf->duration
      , paf->modifier
      , paf->location );
  }

  for ( ed = obj->extra_descr; ed; ed = ed->next )
    fprintf( fp, "%s%s~ %s~\n", str_space( "ExtraDescr" , BLANK_LENGTH )
      , ed->keyword, ed->description );

  fprintf( fp, "End\n\n" );
  if ( obj->contains ) fwrite_obj( ch, obj->contains, fp, iNest + 1 , type );

  RETURN_NULL();
}

int load_char_obj( DESCRIPTOR_DATA * d, char * name )
{
  char        strsave[MAX_INPUT_LENGTH];
  CHAR_DATA * ch;
  FILE_DATA * aFile;
  int         loop;
  int         iNest;

  PUSH_FUNCTION( "load_char_obj" );

  /* 配置記憶體 */
  clear_char( ch = alloc_struct( STRUCT_CHAR_DATA ) );

  /* 配置記憶體 */
  ch->pcdata = alloc_struct( STRUCT_PC_DATA );

  memset( ch->pcdata, 0, sizeof( PC_DATA ) );

  d->character           = ch;
  ch->desc               = d;
  ch->name               = str_dup( name );
  ch->cname              = str_dup( "" );
  ch->prompt             = str_dup( "" );
  ch->email              = str_dup( "" );
  ch->wizflags           = 0;
  ch->tribunal_timer     = 0;
  ch->act                = 0;
  ch->now_str            = DefaultStr;
  ch->now_int            = DefaultInt;
  ch->now_wis            = DefaultWis;
  ch->now_dex            = DefaultDex;
  ch->now_con            = DefaultCon;
  ch->pcdata->pwd        = str_dup( "" );
  ch->pcdata->bamfin     = str_dup( "" );
  ch->pcdata->bamfout    = str_dup( "" );
  ch->pcdata->title      = str_dup( "" );
  ch->pcdata->lasthost   = str_dup( "" );
  ch->pcdata->mater      = str_dup( "" );
  ch->pcdata->contract   = FALSE;
  ch->pcdata->pagelen    = 20;
  ch->pcdata->autosave   = -1;
  ch->pcdata->autobackup = -1;
  ch->pcdata->angel_set  = AngelDefault;
  ch->pcdata->angel_left = AngelDefault;

  ch->pcdata->condition[COND_THIRST]  = 100;
  ch->pcdata->condition[COND_FULL]    = 100;

  ch->pcdata->rechristen    = Rechristen;
  ch->pcdata->couple.high   = 0;
  ch->pcdata->couple.low    = 0;

  for ( loop = 0; loop < MAX_NOTEPAD; loop++ )
    ch->pcdata->notepad[loop] = str_dup( "" );

  ch->pcdata->trace = NULL;
  for ( loop = 0; loop < MAX_TRACE; loop++ ) ch->pcdata->map[loop] = NULL;

  str_cpy( strsave, file_name( name, SAVE_FILE ) );
  if ( access( strsave , R_OK ) != 0 ) RETURN( FILE_NEW );

  if ( ( aFile = f_open( strsave, "r" ) ) )
  {
    for ( iNest = 0; iNest < MAX_NEST; iNest++ ) rgObjNest[iNest] = NULL;

    for ( ;; )
    {
      char   letter;
      char * word;

      letter = fread_letter( aFile );

      if ( letter == '*' )
      {
        fread_to_eol( aFile );
        continue;
      }

      if ( letter != '#' )
      {
        mudlog( LOG_DEBUG , "Load_char_obj: 未發現到 # 符號 ." );
        f_close( aFile );
        RETURN( FILE_ERROR );
      }

      word = fread_word( aFile );
      if ( !str_cmp( word, "PLAYER" ) )
      {
        if ( !fread_char( ch, aFile ) )
        {
          f_close( aFile );
          RETURN( FILE_ERROR );
        }
      }

      else if ( !str_cmp( word, "OBJECT" ) )
      {
        if ( !fread_obj( ch, aFile , LOAD_OBJECT ) )
        {
          f_close( aFile );
          RETURN( FILE_ERROR );
        }
      }

      else if ( !str_cmp( word, "DEPOSIT" ) )
      {
        if ( !fread_obj( ch, aFile , LOAD_DEPOSIT ) )
        {
          f_close( aFile );
          RETURN( FILE_ERROR );
        }
      }

      else if ( !str_cmp( word, "END" ) ) break;

      else
      {
        mudlog( LOG_DEBUG , "Load_char_obj: 玩家檔案不正確的型態." );
        f_close( aFile );
        RETURN( FILE_ERROR );
      }
    }

    f_close( aFile );

    /* 讀取檔案大小 */
    char_filesize( ch );
  }

  str_cpy( strsave, file_name( name, FAILCODE_FILE ) );
  if ( access( strsave, R_OK ) == 0 && ( aFile = f_open( strsave, "r" ) ) )
  {
    if ( !fread_failcode( ch, aFile ) )
    {
      f_close( aFile );
      RETURN( FILE_ERROR );
    }

    f_close( aFile );
  }

  RETURN( FILE_CORRECT );
}

void fwrite_failcode( CHAR_DATA * ch, const char * address )
{
  char   failcode[MAX_FILE_LENGTH];
  char   pAddress[MAX_STRING_LENGTH];
  FILE * pFile;

  PUSH_FUNCTION( "fwrite_failcode" );

  if ( !ch || IS_NPC( ch ) || !address || !*address )
  {
    mudlog( LOG_DEBUG, "fwrite_failcode: 來源不正確." );
    RETURN_NULL();
  }

  str_cpy( failcode, file_name( ch->name, FAILCODE_FILE ) );

  if ( ( pFile = FOPEN( failcode, "a" ) ) )
  {
    str_cpy( pAddress, address );
    smash_tilde( pAddress );
    fprintf( pFile, "%s%s~ %d\n"
      , str_space( "FailCode", BLANK_LENGTH ), pAddress, ( int ) current_time );

    FCLOSE( pFile );

    /* 改變檔案存取模式 */
    set_file_mode( failcode );
  }

  else
  {
    mudlog( LOG_DEBUG, "fwrite_failcode: 無法寫入錯誤密碼記錄檔%s.", failcode );
  }
  RETURN_NULL();
}

#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )                                    \
                                if ( !str_cmp( word, literal ) )        \
                                {                                       \
                                    field  = value;                     \
                                    fMatch = TRUE;                      \
                                    break;                              \
                                }

bool fread_char( CHAR_DATA * ch, FILE_DATA * fp )
{
  char        * word;
  AFFECT_DATA * pAffect;
  SKILL_DATA  * pSkill;
  bool          fMatch;
  int           useless;

  PUSH_FUNCTION( "fread_char" );

  for ( ; ; )
  {
    word   = if_eof( fp ) ? "End" : fread_word( fp );
    fMatch = FALSE;

    switch ( UPPER(word[0]) )
    {
    case '*':
      fMatch = TRUE;
      fread_to_eol( fp );
      break;

    case 'A':
      KEY( "AutoSave",   ch->pcdata->autosave,   fread_number( fp ) );
      KEY( "AutoBackup", ch->pcdata->autobackup, fread_number( fp ) );
      KEY( "Act",        ch->act,                fread_number( fp ) );
      KEY( "Alignment",  ch->alignment,          fread_number( fp ) );
      KEY( "Armor",      ch->armor,              fread_number( fp ) );
      KEY( "Adviser",    ch->adviser,            fread_number( fp ) );

      if ( !str_cmp( word, "Angel" ) )
      {
        ch->pcdata->angel_set = ch->pcdata->angel_left = fread_number( fp );
        fMatch = TRUE;
        break;
      }

      if ( !str_cmp( word, "Alias" ) )
      {
        ALIAS_DATA * pAlias;
        ALIAS_DATA * zAlias;
        char       * point;

        pAlias = alloc_struct( STRUCT_ALIAS_DATA );

        point  = ( char * ) fread_string( fp );

        if ( str_len( point ) >= sizeof( pAlias->text ) )
        {
          str_cpy( pAlias->text, "???" );
        }
        else
        {
          str_cpy( pAlias->text , point );
        }

        free_string( point );

        point = ( char * ) fread_string( fp );

        if ( str_len( point ) >= sizeof( pAlias->macro ) )
        {
          str_cpy( pAlias->macro, "???" );
        }
        else
        {
          str_cpy( pAlias->macro , point );
        }

        free_string( point );

        pAlias->next = NULL;

        for ( zAlias = ch->alias;
          zAlias && zAlias->next;
          zAlias = zAlias->next );

        if ( !zAlias ) ch->alias    = pAlias;
        else           zAlias->next = pAlias;
        fMatch       = TRUE;
        break;
      }

      if ( !str_cmp( word, "Affected" ) )
      {
        char * name;
        int    duration;
        int    modifier;
        int    location;

        name     = fread_word( fp );
        duration = fread_number( fp );
        modifier = fread_number( fp );
        location = fread_number( fp );

        if ( ( pSkill = skill_isname( name ) ) )
        {
          /* 配置記憶體 */
          pAffect = alloc_struct( STRUCT_AFFECT_DATA );

          pAffect->type      = pSkill->slot;
          pAffect->duration  = duration;
          pAffect->modifier  = modifier;
          pAffect->location  = location;

          pAffect->next      = ch->affected;
          ch->affected       = pAffect;
        }

        fMatch = TRUE;
        break;
      }

      if ( !str_cmp( word, "AutoMap" ) )
      {
        TRACE_DATA * pTrace;
        int          sn;
        int          loop = 0;

        pTrace = alloc_struct( STRUCT_TRACE_DATA );

        sn               = fread_number( fp );
        sn               = UMIN( MAX_TRACE -1 , UMAX( 0, sn ) );
        pTrace->starting = fread_number( fp );
        pTrace->ending   = fread_number( fp );

        for ( loop = 0; loop < MAX_PATH; loop++ )
          pTrace->path[loop] = ERRORCODE;

        loop = 0;

        for ( loop = 0; loop < MAX_PATH; loop++ )
        {
          if ( fread_if_eol( fp ) ) break;
          pTrace->path[loop] = fread_number( fp );
        }

        if ( !get_room_index( pTrace->starting )
          || !get_room_index( pTrace->ending )
          || ch->pcdata->map[sn] )
        {
          free_struct( pTrace, STRUCT_TRACE_DATA );
        }

        else
        {
          ch->pcdata->map[sn] = pTrace;
        }

        fread_to_eol( fp );

        fMatch = TRUE;
        break;
      }
      break;

    case 'B':
      KEY( "Blind" ,  useless            , fread_number( fp ) );
      KEY( "Bamfin",  ch->pcdata->bamfin , fread_string( fp ) );
      KEY( "Bamfout", ch->pcdata->bamfout, fread_string( fp ) );
      KEY( "Bank",    ch->bank           , fread_number( fp ) );

      if ( !str_cmp( word, "Board" ) )
      {
        int sn;

        sn = fread_number( fp );

        if ( sn >= 0 && sn < max_board && board_lookup( sn ) )
          ch->pcdata->board[sn] = fread_number( fp );

        fMatch = TRUE;
        break;
      }

      if ( !str_cmp( word, "BadFriend" ) )
      {
        FRIEND_DATA * pFriend;
        char        * name;

        name  = ( char * ) fread_string( fp );

        if ( is_exist( name ) && str_len( name ) < sizeof( pFriend->name ) )
        {
          pFriend = alloc_struct( STRUCT_FRIEND_DATA );
          str_cpy( pFriend->name, name );
          pFriend->next = ch->badfriend;
          ch->badfriend = pFriend;
        }

        free_string( name );

        fMatch = TRUE;
        break;
      }

      break;

    case 'C':

      if ( !str_cmp( word , "Class" ) )
      {
        char       * pName;
        CLASS_DATA * pClass;

        if ( !( pClass = is_class( ( pName = fread_string( fp ) ), TRUE ) ) )
        {
          mudlog( LOG_DEBUG , "%s 的職業 %s 從缺.", ch->name , pName );
          pClass  = class_demos;
        }

        free_string( pName );
        fMatch    = TRUE;
        ch->class = pClass;
        break;
      }

      KEY( "CoupleHigh", ch->pcdata->couple.high, fread_number( fp ) );
      KEY( "CoupleLow" , ch->pcdata->couple.low , fread_number( fp ) );
      KEY( "Clublog"   , ch->clublog            , fread_number( fp ) );
      KEY( "ClubNote"  , ch->last_clubnote      , fread_number( fp ) );
      KEY( "Cname"     , ch->cname              , fread_string( fp ) );
      KEY( "Con"       , ch->now_con            , fread_number( fp ) );
      KEY( "ConMod"    , ch->pcdata->mod_con    , fread_number( fp ) );
      KEY( "Contract"  , ch->pcdata->contract   , fread_number( fp ) );
      KEY( "Curse"     , useless                , fread_number( fp ) );
      KEY( "Charm"     , useless                , fread_number( fp ) );

      break;

    case 'D':
      KEY( "Damroll"     , ch->damroll              , fread_number( fp ) );
      KEY( "Deaf"        , ch->deaf                 , fread_number( fp ) );
      KEY( "Deny"        , ch->pcdata->deny         , fread_number( fp ) );
      KEY( "Description" , ch->description          , fread_string( fp ) );
      KEY( "Dex"         , ch->now_dex              , fread_number( fp ) );
      KEY( "DexMod"      , ch->pcdata->mod_dex      , fread_number( fp ) );
      KEY( "Dormancy"    , ch->pcdata->dormancy     , fread_number( fp ) );
      KEY( "Dosage"      , ch->pcdata->dosage       , fread_number( fp ) );
      KEY( "Donate"      , ch->donate               , fread_number( fp ) );
      KEY( "Drunk"       , ch->pcdata->condition[0] , fread_number( fp ) );
      KEY( "DetectInvis" , useless                  , fread_number( fp ) );
      KEY( "DetectEvil"  , useless                  , fread_number( fp ) );
      KEY( "DetectMagic" , useless                  , fread_number( fp ) );
      KEY( "DetectMask"  , useless                  , fread_number( fp ) );
      KEY( "DetectHidden", useless                  , fread_number( fp ) );

      break;

    case 'E':

      if ( !str_cmp( word, "End" ) )
      {
        if ( ch->pcdata )
        {
          ch->pcdata->mod_str = 0;
          ch->pcdata->mod_int = 0;
          ch->pcdata->mod_wis = 0;
          ch->pcdata->mod_dex = 0;
          ch->pcdata->mod_con = 0;
        }

        RETURN( TRUE );
      }

      KEY( "Exp"      , ch->exp       , fread_number( fp ) );
      KEY( "EqCapcity", ch->eq_capcity, fread_number( fp ) );

      if ( !str_cmp( word, "Enable" ) )
      {
        SKILL_DATA * pSkill;

        /* 如果技能已經不存在 */
        if ( !( pSkill = skill_isname( fread_word( fp ) ) ) )
        {
          fMatch = TRUE;
          break;
        }

        /* 如果重複 */
        if ( ( enable_repeat( ch->enable , pSkill ) ) )
        {
          fMatch = TRUE;
          break;
        }

        set_enable( ch, pSkill );
        fMatch = TRUE;
        break;
      }

      if ( !str_cmp( word, "Enemy" ) )
      {
        char * mark;

        mark = fread_string( fp );
        set_enemy( ch, mark );
        free_string( mark );
        fMatch = TRUE;
        break;
      }

      if ( !str_cmp( word , "Email" ) )
      {
         free_string( ch->email );
         ch->email = fread_string( fp );
         fMatch    = TRUE;
         break;
      }

      break;

    case 'F':

      KEY( "Full"       , ch->pcdata->condition[1] , fread_number( fp ) );
      KEY( "Firman"     , ch->firman               , fread_number( fp ) );
      KEY( "Fail"       , ch->failed               , fread_number( fp ) );
      KEY( "FightWin"   , ch->pcdata->fight_win    , fread_number( fp ) );
      KEY( "FightLost"  , ch->pcdata->fight_lost   , fread_number( fp ) );
      KEY( "FaerieFire" , useless                  , fread_number( fp ) );
      KEY( "Flying"     , useless                  , fread_number( fp ) );
      KEY( "Fixity"     , useless                  , fread_number( fp ) );

      if ( !str_cmp( word, "Friend" ) )
      {
        FRIEND_DATA * pFriend;
        char        * name;

        name  = ( char * ) fread_string( fp );

        if ( is_exist( name ) && str_len( name ) < sizeof( pFriend->name ) )
        {
          pFriend = alloc_struct( STRUCT_FRIEND_DATA );
          str_cpy( pFriend->name, name );
          pFriend->next = ch->friend;
          ch->friend    = pFriend;
        }

        free_string( name );

        fMatch = TRUE;
        break;
      }

      break;

    case 'G':
      KEY( "Gold", ch->gold, fread_number( fp ) );
      break;

    case 'H':
      KEY( "Hit"    , ch->hit    , fread_number( fp ) );
      KEY( "HitMax" , ch->max_hit, fread_number( fp ) );
      KEY( "HitMod" , ch->mod_hit, fread_number( fp ) );
      KEY( "Hitroll", ch->hitroll, fread_number( fp ) );
      KEY( "Home"   , ch->home   , fread_number( fp ) );
      KEY( "Hide"   , useless    , fread_number( fp ) );

      break;

    case 'I':
      KEY( "Int"       , ch->now_int         , fread_number( fp ) );
      KEY( "IntMod"    , ch->pcdata->mod_int , fread_number( fp ) );
      KEY( "Imprison"  , ch->pcdata->imprison, fread_number( fp ) );
      KEY( "Invisible" , useless             , fread_number( fp ) );
      KEY( "Infrared"  , useless             , fread_number( fp ) );

      break;

    case 'J':
      KEY( "Jail", ch->jail     , fread_number( fp ) );
      KEY( "Joke", ch->last_joke, fread_number( fp ) );
      break;

    case 'K':
      KEY( "Kills" ,    ch->pcdata->kills ,      fread_number( fp ) );
      KEY( "KillLevel", ch->pcdata->kill_level , fread_number( fp ) );
      break;

    case 'L':
      KEY( "Level",     ch->level,            fread_number( fp ) );
      KEY( "Limit",     ch->limit,            fread_number( fp ) );
      KEY( "LongDescr", ch->long_descr,       fread_string( fp ) );
      KEY( "LastHost",  ch->pcdata->lasthost, fread_string( fp ) );

      if ( !str_cmp( word, "LimitIP" ) )
      {
        IP_DATA    * pIp;

        pIp = alloc_struct( STRUCT_IP_DATA );
        pIp->address = fread_string( fp );
        pIp->next    = ch->connect;
        ch->connect  = pIp;

        fMatch       = TRUE;
        break;
      }

      if ( !str_cmp( word, "Learn" ) )
      {
        SKILL_DATA * pSkill;
        int          value;

        value  = fread_number( fp );

        if ( ( pSkill = skill_isname( fread_word( fp ) ) ) == NULL )
        {
          mudlog( LOG_DEBUG , "Fread_char: 未知的技能." );
        }
        else
        {
          ch->skill[pSkill->slot] = value;
        }

        fMatch = TRUE;
        break;
      }

      break;

    case 'M':

      KEY( "Mana"   , ch->mana           , fread_number( fp ) );
      KEY( "Mate"   , ch->pcdata->mater  , fread_string( fp ) );
      KEY( "ManaMax", ch->max_mana       , fread_number( fp ) );
      KEY( "ManaMod", ch->mod_mana       , fread_number( fp ) );
      KEY( "Move"   , ch->move           , fread_number( fp ) );
      KEY( "MoveMax", ch->max_move       , fread_number( fp ) );
      KEY( "MoveMod", ch->mod_move       , fread_number( fp ) );
      KEY( "Murder" , ch->pcdata->murder , fread_number( fp ) );
      KEY( "Masked" , useless            , fread_number( fp ) );

      if ( !str_cmp( word, "Mask" ) )
      {
        int mask;

        if ( ( get_mob_index( mask = fread_number( fp ) ) ) )
          ch->mask = mask;

        fMatch = TRUE;
        break;
      }

      break;

    case 'N':

      KEY( "Note"  , ch->last_note, fread_number( fp ) );
      KEY( "Notify", ch->notify   , fread_number( fp ) );
      KEY( "NSkill", ch->nskill   , fread_number( fp ) );

      if ( !str_cmp( word, "Name" ) )
      {
        fread_to_eol( fp );
        fMatch = TRUE;
        break;
      }

      if ( !str_cmp( word, "NotePad" ) )
      {
        int loop;

        loop = fread_number( fp );
        loop = UMIN( MAX_NOTEPAD - 1, UMAX( loop, 0 ) );
        free_string( ch->pcdata->notepad[loop] );
        ch->pcdata->notepad[loop] = fread_string( fp );

        fMatch = TRUE;
        break;
      }
      break;

    case 'P':
      KEY( "Pagelen"  , ch->pcdata->pagelen,   fread_number( fp ) );
      KEY( "Password" , ch->pcdata->pwd,       fread_string( fp ) );
      KEY( "PKLost"   , ch->pcdata->pklost,    fread_number( fp ) );
      KEY( "PKWin"    , ch->pcdata->pkwin,     fread_number( fp ) );
      KEY( "PlayLost" , ch->pcdata->fightlost, fread_number( fp ) );
      KEY( "PlayWin"  , ch->pcdata->fightwin,  fread_number( fp ) );
      KEY( "Played"   , ch->played,            fread_number( fp ) );
      KEY( "Position" , ch->position,          fread_number( fp ) );
      KEY( "Practice" , ch->practice,          fread_number( fp ) );
      KEY( "Pills"    , ch->pcdata->pills,     fread_number( fp ) );
      KEY( "Ply"      , ch->pcdata->ply,       fread_number( fp ) );
      KEY( "Poison"   , useless   ,            fread_number( fp ) );
      KEY( "Protect"  , useless    ,           fread_number( fp ) );
      KEY( "PassDoor" , useless     ,          fread_number( fp ) );

      if ( !str_cmp( word , "Prompt" ) )
      {
         free_string( ch->prompt );
         ch->prompt = fread_string( fp );

         if ( ch->prompt && ch->prompt[0] != '\x0' )
           smash_point( ch->prompt );

         fMatch = TRUE;
         break;
      }

      break;

    case 'Q':

      if ( !str_cmp( word, "Quest" ) )
      {
        char * mark;

        if ( quest_lookup( mark = fread_string( fp ) ) ) set_quest( ch, mark );
        else          mudlog( LOG_DEBUG , "Fread_char: 未知的解謎 %s.", mark );

        fMatch = TRUE;
        break;
      }

      break;

    case 'R':

      if ( !str_cmp( word, "Recall" ) )
      {
        int               loop;
        ROOM_INDEX_DATA * pRoom;

        for ( loop = 0; loop < MAX_RECALL; loop++ )
        {
          ch->pcdata->recall[loop] = fread_number( fp );

          if ( ch->pcdata->recall[loop] > 0 )
          {
            if ( !( pRoom = get_room_index( ch->pcdata->recall[loop] ) )
              || pRoom->Memorize == FALSE )
              ch->pcdata->recall[loop] = 0;
          }
        }

        fMatch = TRUE;
        break;
      }

      if ( !str_cmp( word, "Room" ) )
      {
        if ( !( ch->in_room = get_room_index( fread_number( fp ) ) ) )
        {
          send_to_char( "找不到你原來的房間﹐只好把你傳送到起始點﹗\n\r" , ch );
          ch->in_room = get_hometown( ch );
        }

        fMatch = TRUE;
        break;
      }

      KEY( "Rechristen", ch->pcdata->rechristen, fread_number( fp ) );
      break;

    case 'S':

      KEY( "SerialHigh" , ch->serial.high         , fread_number( fp ) );
      KEY( "SerialLow"  , ch->serial.low          , fread_number( fp ) );
      KEY( "Str"        , ch->now_str             , fread_number( fp ) );
      KEY( "StrMod"     , ch->pcdata->mod_str     , fread_number( fp ) );
      KEY( "SavingThrow", ch->saving_throw        , fread_number( fp ) );
      KEY( "Suspect"    , ch->pcdata->suspect     , fread_number( fp ) );
      KEY( "Sex"        , ch->sex                 , fread_number( fp ) );
      KEY( "ShortDescr" , ch->byname              , fread_string( fp ) );
      KEY( "Steal"      , ch->pcdata->steal       , fread_number( fp ) );
      KEY( "StPanity"   , ch->pcdata->steal_panity, fread_number( fp ) );
      KEY( "Sanctuary"  , useless                 , fread_number( fp ) );
      KEY( "Sneak"      , useless                 , fread_number( fp ) );
      KEY( "Sleep"      , useless                 , fread_number( fp ) );

      if ( !str_cmp( word, "Stocks" ) )
      {
        int sn;
        int number;
        int gold;

        sn     = fread_number( fp ) - 1;
        number = fread_number( fp );
        gold   = fread_number( fp );

        if ( sn < 0 || sn >= MAX_STOCK || !stock_data[sn].name )
          mudlog( LOG_DEBUG, "fread_char: 未知的股票數值 %d.", sn );

        ch->pcdata->stock[sn] = UMIN( max_stock_buy, UMAX( 0, number ) );

        if ( ( ch->pcdata->asset[sn] = gold ) <= 0 )
          ch->pcdata->asset[sn] = stock_data[sn].cost;

        ch->pcdata->asset[sn] = UMIN( MAX_STOCK_COST, ch->pcdata->asset[sn] );
        fMatch = TRUE;
        break;
      }

      if ( !str_cmp( word, "Stamp" ) )
      {
        int number;

        if ( is_gift_stamp( number = fread_number( fp ) ) )
          set_gift_stamp( ch, number );

        fMatch = TRUE;
        break;
      }

      break;

    case 'T':
      KEY( "Thirst"  , ch->pcdata->condition[2], fread_number( fp ) );
      KEY( "Trust"   , ch->trust               , fread_number( fp ) );
      KEY( "Turn"    , ch->turn                , fread_number( fp ) );
      KEY( "Title"   , ch->pcdata->title       , fread_string( fp ) );
      KEY( "Tribunal", ch->tribunal_timer      , fread_number( fp ) );
      break;

    case 'V':
      if ( !str_cmp( word, "Vnum" ) )
      {
        ch->pIndexData = get_mob_index( fread_number( fp ) );
        fMatch = TRUE;
        break;
      }
      break;

    case 'W':
      KEY( "WizFlags", ch->wizflags       , fread_number( fp ) );
      KEY( "Wimpy"   , ch->wimpy          , fread_number( fp ) );
      KEY( "Wis"     , ch->now_wis        , fread_number( fp ) );
      KEY( "WisMod"  , ch->pcdata->mod_wis, fread_number( fp ) );
      break;

    case 'X':
      KEY( "Xname", ch->pcdata->xname, fread_number( fp ) );
      break;
    }

    if ( ReadNumberError )
    {
      mudlog( LOG_DEBUG, "Fread_char: 型態%s數字讀取錯誤.", word );
      RETURN( FALSE );
    }

    if ( !fMatch )
    {
      mudlog( LOG_DEBUG , "Fread_char: 找不到正確的型態 %s.", word );
      RETURN( FALSE );
    }
  }
}

bool fread_obj( CHAR_DATA * ch, FILE_DATA * fp , int type )
{
  OBJ_DATA    * obj;
  AFFECT_DATA * pAffect;
  SKILL_DATA  * pSkill;
  char        * word;
  int           iNest;
  bool          fMatch;
  bool          fNest;
  bool          fVnum;

  PUSH_FUNCTION( "fread_obj" );

  /* 配置記憶體 */
  obj = alloc_struct( STRUCT_OBJ_DATA );

  /* 設定預設值 */
  obj->name        = str_dup( "" );
  obj->cname       = str_dup( "" );
  obj->description = str_dup( "" );

  for ( fNest = FALSE, fVnum = TRUE, iNest = 0; ; )
  {
    word   = if_eof( fp ) ? "End" : fread_word( fp );
    fMatch = FALSE;

    switch ( UPPER(word[0]) )
    {
    case '*':
      fMatch = TRUE;
      fread_to_eol( fp );
      break;

    case 'A':

      KEY( "AntiGood"   , obj->AntiGood   , fread_number( fp ) );
      KEY( "AntiEvil"   , obj->AntiEvil   , fread_number( fp ) );
      KEY( "AntiNeutral", obj->AntiNeutral, fread_number( fp ) );

      if ( !str_cmp( word, "Affected" ) )
      {
        char * name;
        int    duration;
        int    modifier;
        int    location;

        name     = fread_word( fp );
        duration = fread_number( fp );
        modifier = fread_number( fp );
        location = fread_number( fp );

        if ( ( pSkill = skill_isname( name ) ) )
        {
          /* 配置記憶體 */
          pAffect = alloc_struct( STRUCT_AFFECT_DATA );

          pAffect->type      = pSkill->slot;
          pAffect->duration  = duration;
          pAffect->modifier  = modifier;
          pAffect->location  = location;

          pAffect->next      = obj->affected;
          obj->affected      = pAffect;
        }

        fMatch = TRUE;
        break;
      }

      KEY( "Armor", obj->armor, fread_number( fp ) );
      break;

    case 'B':
      KEY( "Bless", obj->Bless, fread_number( fp ) );
      break;

    case 'C':

      KEY( "Cost"      , obj->cost      , fread_number( fp ) );
      KEY( "Cancachet" , obj->Cancachet , fread_number( fp ) );
      KEY( "Contraband", obj->Contraband, fread_number( fp ) );

      if ( !str_cmp( word, "Cachet" ) )
      {
        CACHET_DATA * pCachet;
        int           vnum;

        if ( get_obj_index( ( vnum = fread_number( fp ) ) ) )
        {
          pCachet = alloc_struct( STRUCT_CACHET_DATA );
          pCachet->vnum = vnum;
          pCachet->next = obj->cachet;
          obj->cachet   = pCachet;
        }

        fMatch = TRUE;
        break;
      }

      break;

    case 'D':

      KEY( "Dark", obj->Dark, fread_number( fp ) );

      if ( !str_cmp( word, "Description" ) )
      {
        obj->description = fread_string( fp );

        if ( obj->pIndexData
          && obj->pIndexData->description
          && !str_cmp( obj->name, obj->pIndexData->description ) )
        {
          free_string( obj->description );
          obj->description = str_dup( obj->pIndexData->description );
        }

        fMatch = TRUE;
        break;
      }

      break;

    case 'E':

      KEY( "Evil"      , obj->Evil       , fread_number( fp ) );

      if ( !str_cmp( word, "ExtraDescr" ) )
      {
        EXTRA_DESCR_DATA * ed;

        /* 配置記憶體 */
        ed = alloc_struct( STRUCT_EXTRA_DESCR_DATA );

        ed->keyword      = fread_string( fp );
        ed->description  = fread_string( fp );
        ed->next         = obj->extra_descr;
        obj->extra_descr = ed;
        fMatch           = TRUE;
      }

      if ( !str_cmp( word, "End" ) )
      {
        if ( !fNest || !fVnum )
        {
          mudlog( LOG_DEBUG , "Fread_obj: 物品資料不完全." );
          free_string( obj->name        );
          free_string( obj->description );
          free_string( obj->cname       );

          /* 釋放結構記憶體 */
          free_struct( obj , STRUCT_OBJ_DATA );
          RETURN( FALSE );
        }

        obj->pIndexData->count++;

        /* 物品單位 */
        obj->unit = ( obj->pIndexData && obj->pIndexData->unit )
                  ? obj->pIndexData->unit : DefaultUnit;

        switch( type )
        {
        case LOAD_OBJECT:

          if ( iNest == 0 || rgObjNest[iNest] == NULL )
            obj_to_char( obj, ch );
          else
            obj_to_obj( obj, rgObjNest[iNest-1] );
          break;

        case LOAD_DEPOSIT:

          if ( iNest == 0 || rgObjNest[iNest] == NULL )
            obj_to_char_deposit( obj, ch );
          else
            obj_to_obj_dep( obj, rgObjNest[iNest-1] );
          break;

        default:
          break;
        }

        /* 修正數值 4.24 1999 */
        switch( obj->item_type )
        {
        case ITEM_WEAPON:
          obj->value[1] = obj->pIndexData->value[1];
          obj->value[2] = obj->pIndexData->value[2];
          break;

        case ITEM_ARMOR:
          obj->value[0] = obj->pIndexData->value[0];
          break;
        }

        /* 如果是防具類的 */
        if ( is_armor( obj ) && obj->max_armor <= 0 )
        {
          obj->max_armor = obj->level * 100;
          obj->armor     = obj->level * 100;
        }

        RETURN( TRUE );
      }
      break;

    case 'G':
      KEY( "Glow", obj->Glow, fread_number( fp ) );
      break;

    case 'H':
      KEY( "Hum", obj->Hum, fread_number( fp ) );
      break;

    case 'I':
      KEY( "ItemType" , obj->item_type, fread_number( fp ) );
      KEY( "Invis"    , obj->Invis    , fread_number( fp ) );
      KEY( "Inventory", obj->Inventory, fread_number( fp ) );
      break;

    case 'L':
      KEY( "Level"   , obj->level   , fread_number( fp ) );
      KEY( "Lock"    , obj->Lock    , fread_number( fp ) );
      KEY( "Loadable", obj->Loadable, fread_number( fp ) );
      break;

    case 'M':
      KEY( "MaxArmor", obj->max_armor, fread_number( fp ) );
      KEY( "Magic"   , obj->Magic    , fread_number( fp ) );
      break;

    case 'N':

      KEY( "NoDrop"  , obj->NoDrop  , fread_number( fp ) );
      KEY( "NoRemove", obj->NoRemove, fread_number( fp ) );
      KEY( "NoSave"  , obj->NoSave  , fread_number( fp ) );
      KEY( "Name"    , obj->name    , fread_string( fp ) );

      if ( !str_cmp( word, "Nest" ) )
      {
        iNest = fread_number( fp );
        if ( iNest < 0 || iNest >= MAX_NEST )
        {
          mudlog( LOG_DEBUG , "Fread_obj: 錯誤的巢狀 %d.", iNest );
        }
        else
        {
          rgObjNest[iNest] = obj;
          fNest            = TRUE;
        }
        fMatch = TRUE;
      }
      break;

    case 'O':

      KEY( "OwnerHigh", obj->owner.high, fread_number( fp ) );
      KEY( "OwnerLow" , obj->owner.low , fread_number( fp ) );

      break;

    case 'S':

      KEY( "SerialHigh", obj->serial.high, fread_number( fp ) );
      KEY( "SerialLow",  obj->serial.low , fread_number( fp ) );
      KEY( "ShortDescr", obj->cname      , fread_string( fp ) );

      if ( !str_cmp( word, "Spell" ) )
      {
        SKILL_DATA * pSkill;
        char       * SkillName;
        int          iValue;

        iValue    = fread_number( fp );
        SkillName = fread_word( fp );

        if ( !( pSkill = skill_isname( SkillName ) ) )
        {
          mudlog( LOG_DEBUG , "Fread_obj: 未知的技能 %s.", SkillName );
        }

        else if ( iValue < 0 || iValue > 3 )
        {
          mudlog( LOG_DEBUG , "Fread_obj: 錯誤的數值 %d.", iValue );
        }

        else
        {
          obj->value[iValue] = pSkill->slot;
        }

        fMatch = TRUE;
        break;
      }

      break;

    case 'T':
      KEY( "Timer"   , obj->timer   , fread_number( fp ) );
      KEY( "Takeable", obj->Takeable, fread_number( fp ) );
      break;

    case 'V':

      if ( !str_cmp( word, "Values" ) )
      {
        int loop;

        for ( loop = 0; loop < MAX_OBJECT_VALUE; loop++ )
          obj->value[loop] = fread_number( fp );

        fMatch = TRUE;
        break;
      }

      if ( !str_cmp( word, "Vnum" ) )
      {
        if ( !( obj->pIndexData = get_obj_index( fread_number( fp ) ) ) )
          obj->pIndexData = ObjProtype;

        fVnum  = TRUE;
        fMatch = TRUE;
        break;
      }

      KEY( "Venom", obj->venom, fread_number( fp ) );
      break;

    case 'W':
      KEY( "WearFlags", obj->wear_flags, fread_number( fp ) );
      KEY( "WearLoc",   obj->wear_loc,   fread_number( fp ) );
      KEY( "Weight",    obj->weight,     fread_number( fp ) );
      break;
    }

    if ( ReadNumberError )
    {
      mudlog( LOG_DEBUG, "Fread_obj: 型態 %s 數字讀取錯誤.", word );
      RETURN( FALSE );
    }

    if ( !fMatch )
    {
      mudlog( LOG_DEBUG , "Fread_obj: 物品型態 %s 不正確.", word );
      RETURN( FALSE );
    }
  }
}

bool fread_failcode( CHAR_DATA * ch, FILE_DATA * pFile )
{
  FAILCODE_DATA * pFailCode;
  char * word;
  bool   fMatch;

  PUSH_FUNCTION( "fread_failcode" );

  for ( ; ; )
  {
    word   = if_eof( pFile ) ? "End" : fread_word( pFile );
    fMatch = FALSE;

    switch ( UPPER( word[0] ) )
    {
    case EOF:
      RETURN( TRUE );

    case '*':
      fMatch = TRUE;
      fread_to_eol( pFile );
      break;

    case 'E':

      if ( !str_cmp( word, "End" ) ) RETURN( TRUE );
      break;

    case 'F':

      if ( !str_cmp( word, "FailCode" ) )
      {
        pFailCode = alloc_struct( STRUCT_FAILCODE_DATA );

        pFailCode->address = fread_string( pFile );
        pFailCode->timer   = fread_number( pFile );

        /* 一定讀完一行 */
        fread_to_eol( pFile );

        pFailCode->next = ch->failcode;
        ch->failcode    = pFailCode;

        fMatch = TRUE;
        break;
      }

      break;
    }

    if ( ReadNumberError )
    {
      mudlog( LOG_DEBUG, "Fread_failcode: 型態 %s 數字讀取錯誤.", word );
      RETURN( FALSE );
    }

    if ( !fMatch )
    {
      mudlog( LOG_DEBUG , "fread_failcode: 找不到正確的型態 %s.", word );
      RETURN( FALSE );
    }
  }
}

void check_null_object( CHAR_DATA * ch )
{
  OBJ_DATA * obj;
  OBJ_DATA * obj_next;
  bool       found;

  PUSH_FUNCTION( "check_null_object" );

  for ( found = FALSE, obj = ch->carrying; obj; obj = obj_next )
  {
    obj_next = obj->next_content;
    if ( verify_obj( obj ) && obj->pIndexData == ObjProtype )
    {
      found = TRUE;

      act( "$1你的物品$2$p$1找不到索引﹐所以系統自動將它損毀﹗$0"
        , ch, obj, NULL, TO_CHAR );

      extract_obj( obj );
    }
  }

  if ( found ) send_to_char( "\n\r\n\r" , ch );
  RETURN_NULL();
}

/* 回存幫派資料 */
void save_club( void )
{
  CLUB_DATA * pClub;
  FILE      * pFile;
  bool        bStart = TRUE;
  int         loop;

  PUSH_FUNCTION( "save_club" );

  if ( !( pFile = FOPEN( club_file, "w" ) ) )
  {
    mudlog( LOG_DEBUG , "Save_club: 開啟幫派檔案 %s 錯誤.", club_file );
    RETURN_NULL();
  }

  for ( pClub = club_list; pClub; pClub = pClub->next )
  {
    if ( !pClub->master ) continue;

    /* 若是不是第一個 */
    if ( !bStart ) fprintf( pFile, "\n" );
    bStart = FALSE;

    fprintf( pFile, "#CLUB\n" );

    fprint_string( pFile, "Name"   , pClub->name   );
    fprint_string( pFile, "Cname"  , pClub->cname  );
    fprint_string( pFile, "Master" , pClub->master );

    if ( pClub->vicemaster )
      fprint_string( pFile, "Vicemaster" , pClub->vicemaster );

    fprint_number( pFile, "Timer"  , pClub->timer  );
    fprint_number( pFile, "Status" , pClub->status );
    fprint_number( pFile, "Money"  , pClub->money  );

    for ( loop = 0; loop < MAX_CLUB_DOYEN; loop++ )
    {
      if ( pClub->doyen[loop] )
        fprint_string( pFile, "Doyen" , pClub->doyen[loop] );
    }

    for ( loop = 0; loop < MAX_CLUB_MEMBER; loop++ )
    {
      if ( pClub->member[loop] && pClub->verify[loop] )
        fprint_string( pFile, "Member", pClub->member[loop] );
    }

    for ( loop = 0; loop < MAX_CLUB_MEMBER; loop++ )
    {
      if ( pClub->member[loop] && !pClub->verify[loop] )
        fprint_string( pFile, "Follower", pClub->member[loop] );
    }

    fprintf( pFile, "End\n" );
  }

  FCLOSE( pFile );

  /* 改變檔案存取模式 */
  set_file_mode( club_file );

  RETURN_NULL();
}

void fprint_string( FILE * pFile, char * title, const char * string )
{
  PUSH_FUNCTION( "fprint_string" );
  fprintf( pFile , "%s%s~\n", str_space( title, BLANK_LENGTH ), string );
  RETURN_NULL();
}

void fprint_word( FILE * pFile, char * title, const char * string )
{
  PUSH_FUNCTION( "fprint_word" );
  fprintf( pFile , "%s'%s'\n", str_space( title, BLANK_LENGTH ), string );
  RETURN_NULL();
}

void fprint_const( FILE * pFile, char * title, const char * string )
{
  PUSH_FUNCTION( "fprint_const" );
  fprintf( pFile , "%s%s\n", str_space( title, BLANK_LENGTH ), string );
  RETURN_NULL();
}

void fprint_number( FILE * pFile, char * title, int number )
{
  PUSH_FUNCTION( "fprint_number" );
  fprintf( pFile, "%s%d\n" , str_space( title, BLANK_LENGTH ), number );
  RETURN_NULL();
}

void set_file_mode( const char * name )
{
  PUSH_FUNCTION( "set_file_mode" );

  /* 若是不開啟 */
  if ( !SetFileMode ) RETURN_NULL();

  if ( !name || !*name )
  {
    mudlog( LOG_DEBUG, "set_file_mode: 來源不正確." );
    RETURN_NULL();
  }

  if ( chmod( name, FileMode ) != 0 )
  {
    mudlog( LOG_DEBUG, "set_file_mode: 設定檔案 %s 模式錯誤.", name );
    RETURN_NULL();
  }

  RETURN_NULL();
}

FUNCTION( do_filestat )
{
  PUSH_FUNCTION( "do_filestat" );

  if ( !SetFileMode )
  {
    send_to_char( "目前系統更改模式不開啟﹗\n\r", ch );
    RETURN_NULL();
  }

  print_to_char( ch,
    "目前%s的檔案存入模式是﹕\n\r"
    "使用者 可讀﹕%s 可寫﹕%s 可執行﹕%s\n\r"
    "同族群 可讀﹕%s 可寫﹕%s 可執行﹕%s\n\r"
    "其他者 可讀﹕%s 可寫﹕%s 可執行﹕%s\n\r"
    , mud_name
    , YESNO( FileMode & S_IRUSR )
    , YESNO( FileMode & S_IWUSR )
    , YESNO( FileMode & S_IXUSR )
    , YESNO( FileMode & S_IRGRP )
    , YESNO( FileMode & S_IWGRP )
    , YESNO( FileMode & S_IXGRP )
    , YESNO( FileMode & S_IROTH )
    , YESNO( FileMode & S_IWOTH )
    , YESNO( FileMode & S_IXOTH ) );

  RETURN_NULL();
}

void char_filesize( CHAR_DATA * ch )
{
  struct stat sData;
  char        strsave[MAX_FILE_LENGTH];

  PUSH_FUNCTION( "char_filesize" );

  if ( IS_NPC( ch ) || !ch->pcdata ) RETURN_NULL();

  str_cpy( strsave, file_name( ch->name , SAVE_FILE ) );

  if ( stat( strsave, &sData ) != 0 )
  {
    mudlog( LOG_DEBUG, "char_filesize: 讀取檔案 %s 錯誤.", strsave );
    RETURN_NULL();
  }

  ch->pcdata->file_size = sData.st_size;
  RETURN_NULL();
}

bool excess_filequota( CHAR_DATA * ch )
{
  PUSH_FUNCTION( "excess_filequota" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "excess_filequota: 來源不正確." );
    RETURN( TRUE );
  }

  if ( IS_NPC( ch ) || !ch->pcdata        ) RETURN( FALSE );
  if ( ch->pcdata->file_size > file_quota ) RETURN( TRUE );

  RETURN( FALSE );
}

void autosave_update( void )
{
  DESCRIPTOR_DATA * man;
  CHAR_DATA       * ch;

  PUSH_FUNCTION( "autosave_update" );

  for ( man = descriptor_list; man; man = man->next )
  {
    if ( verify_desc( man )
      && man->connected == CON_PLAYING
      && ( ch = man->character )
      && ch->pcdata )
    {
      if ( ch->pcdata->autosave_tick > 0
        && --ch->pcdata->autosave_tick <= 0 )
      {
        do_save( ch, "" );
        ch->pcdata->autosave_tick = ch->pcdata->autosave;
      }

      if ( ch->pcdata->autobackup_tick > 0
        && --ch->pcdata->autobackup_tick <= 0 )
      {
        do_backup( ch, "" );
        ch->pcdata->autobackup_tick = ch->pcdata->autobackup;
      }
    }
  }

  RETURN_NULL();
}

void save_basic_file( CHAR_DATA * ch )
{
  char   strsave[MAX_FILE_LENGTH];
  FILE * pFile;

  PUSH_FUNCTION( "save_basic_file" );

  if ( !ch )
  {
    mudlog( LOG_DEBUG, "save_basic_file: 來源不正確." );
    RETURN_NULL();
  }

  if ( IS_NPC( ch ) ) RETURN_NULL();

  str_cpy( strsave, file_name( ch->name , BASIC_FILE ) );

  if ( !( pFile = FOPEN( strsave, "w" ) ) )
  {
    mudlog( LOG_DEBUG , "Save_basic_obj: 開啟檔案 %s 錯誤.", ch->name );
  }

  else
  {
    fprintf( pFile, "Name            %s\n", ch->name  );
    fprintf( pFile, "Cname           %s\n", ch->cname );
    fprintf( pFile, "Email           %s\n", ch->email );
    fprintf( pFile, "LastHost        %s\n"
      , ( ch->desc ) ? ch->desc->host : "不正常斷線" );

    fprintf( pFile, "Level           %d\n", ch->level );
    fprintf( pFile, "Sex             %d\n", ch->sex   );
    fprintf( pFile, "Played          %d\n"
      , (int) ( ch->played ) + ( (int) current_time - (int) ch->logon) );

    fprintf( pFile, "SerialHigh      %d\n", ch->serial.high );
    fprintf( pFile, "SerialLow       %d\n", ch->serial.low  );

    if ( ch->club && ch->club->name )
      fprintf( pFile, "Club            %s\n", ch->club->name );

    if ( ch->class && ch->class->name )
      fprintf( pFile, "Class           %s\n", ch->class->name );

    if ( ch->pcdata->couple.high > 0 && ch->pcdata->couple.low > 0 )
    {
      fprintf( pFile, "CoupleHigh      %d\n", ch->pcdata->couple.high );
      fprintf( pFile, "CoupleLow       %d\n", ch->pcdata->couple.low  );
    }

    fprintf( pFile, "End\n" );

    FCLOSE( pFile );
  }

  RETURN_NULL();
}

void player_log( CHAR_DATA * ch, const char * string )
{
  char   strsave[MAX_FILE_LENGTH];
  FILE * pFile;

  PUSH_FUNCTION( "player_log" );

  if ( !ch || !string )
  {
    mudlog( LOG_DEBUG, "player_log: 來源不正確." );
    RETURN_NULL();
  }

  if ( IS_NPC( ch ) || !*string ) RETURN_NULL();

  str_cpy( strsave, file_name( ch->name , LOG_FILE ) );

  if ( ( pFile = FOPEN( strsave, "a" ) ) )
  {
    fprintf( pFile ,"(%s) %s\n"
      , time_format( time( NULL ) , "%r, %a-%d-%b-%y" ), string );

    FCLOSE( pFile );
  }

  RETURN_NULL();
}

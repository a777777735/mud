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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "directory.h"
#include "file.h"
#include "merc.h"

#define MAX_LENGTH      255

/* 區域函數 */
int     fread_line      args( ( FILE * , char * , char * ) );
void    exec_ini        args( ( char * , char * ) );

/* 全域變數 */
int     max_connect;
int     chat_penalty    = CHAT_PENALTY;
int     level_limit     = 0;
bool    internal_debug  = TRUE;
char    mud_name        [MAX_FILE_LENGTH] = { '\x0' };
char    bbs_name        [MAX_FILE_LENGTH] = { '\x0' };
char    home_dir        [MAX_FILE_LENGTH] = { '\x0' };
char    help_dir        [MAX_FILE_LENGTH] = { '\x0' };
char    social_dir      [MAX_FILE_LENGTH] = { '\x0' };
char    angel_dir       [MAX_FILE_LENGTH] = { '\x0' };
char    social_file     [MAX_FILE_LENGTH] = { '\x0' };
char    area_dir        [MAX_FILE_LENGTH] = { '\x0' };
char    board_dir       [MAX_FILE_LENGTH] = { '\x0' };
char    greet_dir       [MAX_FILE_LENGTH] = { '\x0' };
char    vote_dir        [MAX_FILE_LENGTH] = { '\x0' };
char    joke_dir        [MAX_FILE_LENGTH] = { '\x0' };
char    note_dir        [MAX_FILE_LENGTH] = { '\x0' };
char    sector_dir      [MAX_FILE_LENGTH] = { '\x0' };
char    class_dir       [MAX_FILE_LENGTH] = { '\x0' };
char    liq_dir         [MAX_FILE_LENGTH] = { '\x0' };
char    mob_dir         [MAX_FILE_LENGTH] = { '\x0' };
char    mob_ext         [MAX_FILE_LENGTH] = { '\x0' };
char    room_dir        [MAX_FILE_LENGTH] = { '\x0' };
char    room_ext        [MAX_FILE_LENGTH] = { '\x0' };
char    joke_ext        [MAX_FILE_LENGTH] = { '\x0' };
char    note_ext        [MAX_FILE_LENGTH] = { '\x0' };
char    vote_ext        [MAX_FILE_LENGTH] = { '\x0' };
char    help_ext        [MAX_FILE_LENGTH] = { '\x0' };
char    angel_ext       [MAX_FILE_LENGTH] = { '\x0' };
char    greet_ext       [MAX_FILE_LENGTH] = { '\x0' };
char    sector_ext      [MAX_FILE_LENGTH] = { '\x0' };
char    class_ext       [MAX_FILE_LENGTH] = { '\x0' };
char    liquid_ext      [MAX_FILE_LENGTH] = { '\x0' };
char    social_ext      [MAX_FILE_LENGTH] = { '\x0' };
char    command_ext     [MAX_FILE_LENGTH] = { '\x0' };
char    skill_ext       [MAX_FILE_LENGTH] = { '\x0' };
char    obj_dir         [MAX_FILE_LENGTH] = { '\x0' };
char    obj_ext         [MAX_FILE_LENGTH] = { '\x0' };
char    reset_dir       [MAX_FILE_LENGTH] = { '\x0' };
char    reset_ext       [MAX_FILE_LENGTH] = { '\x0' };
char    shop_dir        [MAX_FILE_LENGTH] = { '\x0' };
char    shop_ext        [MAX_FILE_LENGTH] = { '\x0' };
char    mineral_dir     [MAX_FILE_LENGTH] = { '\x0' };
char    mineral_ext     [MAX_FILE_LENGTH] = { '\x0' };
char    dir_file        [MAX_FILE_LENGTH] = { '\x0' };
char    board_lst       [MAX_FILE_LENGTH] = { '\x0' };
char    board_sheet     [MAX_FILE_LENGTH] = { '\x0' };
char    player_dir      [MAX_FILE_LENGTH] = { '\x0' };
char    index_file      [MAX_FILE_LENGTH] = { '\x0' };
char    board_index     [MAX_FILE_LENGTH] = { '\x0' };
char    null_file       [MAX_FILE_LENGTH] = { '\x0' };
char    ideas_file      [MAX_FILE_LENGTH] = { '\x0' };
char    typo_file       [MAX_FILE_LENGTH] = { '\x0' };
char    newplayer_file  [MAX_FILE_LENGTH] = { '\x0' };
char    symbol_file     [MAX_FILE_LENGTH] = { '\x0' };
char    check_file      [MAX_FILE_LENGTH] = { '\x0' };
char    wizard_file     [MAX_FILE_LENGTH] = { '\x0' };
char    net_file        [MAX_FILE_LENGTH] = { '\x0' };
char    bus_file        [MAX_FILE_LENGTH] = { '\x0' };
char    donate_file     [MAX_FILE_LENGTH] = { '\x0' };
char    gift_file       [MAX_FILE_LENGTH] = { '\x0' };
char    date_file       [MAX_FILE_LENGTH] = { '\x0' };
char    bounty_file     [MAX_FILE_LENGTH] = { '\x0' };
char    event_file      [MAX_FILE_LENGTH] = { '\x0' };
char    ship_file       [MAX_FILE_LENGTH] = { '\x0' };
char    promotion_file  [MAX_FILE_LENGTH] = { '\x0' };
char    purge_file      [MAX_FILE_LENGTH] = { '\x0' };
char    quest_file      [MAX_FILE_LENGTH] = { '\x0' };
char    immlist_file    [MAX_FILE_LENGTH] = { '\x0' };
char    sale_file       [MAX_FILE_LENGTH] = { '\x0' };
char    situs_file      [MAX_FILE_LENGTH] = { '\x0' };
char    club_file       [MAX_FILE_LENGTH] = { '\x0' };
char    internal_file   [MAX_FILE_LENGTH] = { '\x0' };
char    site_file       [MAX_FILE_LENGTH] = { '\x0' };
char    station_file    [MAX_FILE_LENGTH] = { '\x0' };
char    question_file   [MAX_FILE_LENGTH] = { '\x0' };
char    xname_file      [MAX_FILE_LENGTH] = { '\x0' };
char    stock_file      [MAX_FILE_LENGTH] = { '\x0' };
char    xname_log       [MAX_FILE_LENGTH] = { '\x0' };
char    chat_log        [MAX_FILE_LENGTH] = { '\x0' };
char    suicide_log     [MAX_FILE_LENGTH] = { '\x0' };
char    address_file    [MAX_FILE_LENGTH] = { '\x0' };
char    error_file      [MAX_FILE_LENGTH] = { '\x0' };
char    bugs_file       [MAX_FILE_LENGTH] = { '\x0' };
char    failload_file   [MAX_FILE_LENGTH] = { '\x0' };
char    failexit_file   [MAX_FILE_LENGTH] = { '\x0' };
char    badobject_file  [MAX_FILE_LENGTH] = { '\x0' };
char    failpass_file   [MAX_FILE_LENGTH] = { '\x0' };
char    database_file   [MAX_FILE_LENGTH] = { '\x0' };
char    failenable_file [MAX_FILE_LENGTH] = { '\x0' };
char    bad_file        [MAX_FILE_LENGTH] = { '\x0' };
char    suspect_file    [MAX_FILE_LENGTH] = { '\x0' };
char    MOBProgs_dir    [MAX_FILE_LENGTH] = { '\x0' };
char    skill_dir       [MAX_FILE_LENGTH] = { '\x0' };
char    skill_file      [MAX_FILE_LENGTH] = { '\x0' };
char    ins_dir         [MAX_FILE_LENGTH] = { '\x0' };
char    ins_file        [MAX_FILE_LENGTH] = { '\x0' };
char    welcome_file    [MAX_FILE_LENGTH] = { '\x0' };
char    welcome_imm     [MAX_FILE_LENGTH] = { '\x0' };
char    hero_file       [MAX_FILE_LENGTH] = { '\x0' };
char    wizflags_log    [MAX_FILE_LENGTH] = { '\x0' };
char    motd_file       [MAX_FILE_LENGTH] = { '\x0' };

#if defined( DEFAULT_FILE )
#undef DEFAULT_FILE
#endif

#define DEFAULT_FILE( target, default )              \
   if ( !target[0] ) str_cpy( target, default );     \

/* 啟始化檔案或目錄名稱 */
void default_file( void )
{
  PUSH_FUNCTION( "default_file" );

  DEFAULT_FILE( home_dir       , HOME_DIRECTORY   );
  DEFAULT_FILE( help_dir       , HELP_DIRECTORY   );
  DEFAULT_FILE( social_dir     , SOCIAL_DIRECTORY );
  DEFAULT_FILE( angel_dir      , ANGEL_DIRECTORY  );
  DEFAULT_FILE( social_file    , SOCIAL_FILE      );
  DEFAULT_FILE( area_dir       , AREA_DIRECTORY   );
  DEFAULT_FILE( board_dir      , BOARD_DIRECTORY  );
  DEFAULT_FILE( greet_dir      , GREET_DIRECTORY  );
  DEFAULT_FILE( vote_dir       , VOTE_DIRECTORY   );
  DEFAULT_FILE( joke_dir       , JOKE_DIRECTORY   );
  DEFAULT_FILE( note_dir       , NOTE_DIRECTORY   );
  DEFAULT_FILE( sector_dir     , SECTOR_DIRECTORY );
  DEFAULT_FILE( class_dir      , CLASS_DIRECTORY  );
  DEFAULT_FILE( liq_dir        , LIQ_DIRECTORY    );
  DEFAULT_FILE( mob_dir        , MOB_DIRECTORY    );
  DEFAULT_FILE( mob_ext        , MOB_FILE_EXT     );
  DEFAULT_FILE( room_dir       , ROOM_DIRECTORY   );
  DEFAULT_FILE( room_ext       , ROOM_FILE_EXT    );
  DEFAULT_FILE( obj_dir        , OBJ_DIRECTORY    );
  DEFAULT_FILE( obj_ext        , OBJ_FILE_EXT     );
  DEFAULT_FILE( reset_dir      , RESET_DIRECTORY  );
  DEFAULT_FILE( reset_ext      , RESET_FILE_EXT   );
  DEFAULT_FILE( shop_dir       , SHOP_DIRECTORY   );
  DEFAULT_FILE( shop_ext       , SHOP_FILE_EXT    );
  DEFAULT_FILE( mineral_dir    , MINERAL_DIRECTORY);
  DEFAULT_FILE( mineral_ext    , MINERAL_FILE_EXT );
  DEFAULT_FILE( joke_ext       , JOKE_FILE_EXT    );
  DEFAULT_FILE( note_ext       , NOTE_FILE_EXT    );
  DEFAULT_FILE( vote_ext       , VOTE_FILE_EXT    );
  DEFAULT_FILE( help_ext       , HELP_FILE_EXT    );
  DEFAULT_FILE( angel_ext      , ANGEL_FILE_EXT   );
  DEFAULT_FILE( greet_ext      , GREET_FILE_EXT   );
  DEFAULT_FILE( sector_ext     , SECTOR_FILE_EXT  );
  DEFAULT_FILE( class_ext      , CLASS_FILE_EXT   );
  DEFAULT_FILE( liquid_ext     , LIQUID_FILE_EXT  );
  DEFAULT_FILE( social_ext     , SOCIAL_FILE_EXT  );
  DEFAULT_FILE( command_ext    , COMMAND_FILE_EXT );
  DEFAULT_FILE( skill_ext      , SKILL_FILE_EXT   );
  DEFAULT_FILE( dir_file       , DIRECTORY_FILE   );
  DEFAULT_FILE( board_lst      , BOARD_LIST       );
  DEFAULT_FILE( board_sheet    , BOARD_SHEET      );
  DEFAULT_FILE( player_dir     , PLAYER_DIR       );
  DEFAULT_FILE( index_file     , INDEX_FILE       );
  DEFAULT_FILE( board_index    , BOARD_INDEX      );
  DEFAULT_FILE( null_file      , NULL_FILE        );
  DEFAULT_FILE( ideas_file     , IDEAS_FILE       );
  DEFAULT_FILE( typo_file      , TYPO_FILE        );
  DEFAULT_FILE( newplayer_file , NEWPLAYER_FILE   );
  DEFAULT_FILE( symbol_file    , SYMBOL_FILE      );
  DEFAULT_FILE( check_file     , CHECK_FILE       );
  DEFAULT_FILE( wizard_file    , WIZARD_FILE      );
  DEFAULT_FILE( net_file       , NET_FILE         );
  DEFAULT_FILE( bus_file       , BUS_FILE         );
  DEFAULT_FILE( donate_file    , DONATE_FILE      );
  DEFAULT_FILE( gift_file      , GIFT_FILE        );
  DEFAULT_FILE( date_file      , DATE_FILE        );
  DEFAULT_FILE( bounty_file    , BOUNTY_FILE      );
  DEFAULT_FILE( event_file     , EVENT_FILE       );
  DEFAULT_FILE( ship_file      , SHIP_FILE        );
  DEFAULT_FILE( promotion_file , PROMOTION_FILE   );
  DEFAULT_FILE( purge_file     , PURGE_FILE       );
  DEFAULT_FILE( quest_file     , QUEST_FILE       );
  DEFAULT_FILE( immlist_file   , IMMLIST_FILE     );
  DEFAULT_FILE( sale_file      , SALE_FILE        );
  DEFAULT_FILE( situs_file     , SITUS_FILE       );
  DEFAULT_FILE( club_file      , CLUB_FILE        );
  DEFAULT_FILE( internal_file  , INTERNAL_FILE    );
  DEFAULT_FILE( site_file      , SITE_FILE        );
  DEFAULT_FILE( station_file   , STATION_FILE     );
  DEFAULT_FILE( question_file  , QUESTION_FILE    );
  DEFAULT_FILE( xname_file     , XNAMES_FILE      );
  DEFAULT_FILE( stock_file     , STOCK_FILE       );
  DEFAULT_FILE( xname_log      , XNAME_LOG_FILE   );
  DEFAULT_FILE( chat_log       , CHAT_LOG_FILE    );
  DEFAULT_FILE( suicide_log    , SUICIDE_LOG_FILE );
  DEFAULT_FILE( welcome_file   , WELCOME_FILE     );
  DEFAULT_FILE( welcome_imm    , WELCOME_IMMORTAL );
  DEFAULT_FILE( address_file   , ADDRESS_FILE     );
  DEFAULT_FILE( error_file     , ERROR_FILE       );
  DEFAULT_FILE( hero_file      , HERO_FILE        );
  DEFAULT_FILE( bugs_file      , BUG_FILE         );
  DEFAULT_FILE( failload_file  , FAILLOAD_FILE    );
  DEFAULT_FILE( failexit_file  , FAILEXIT_FILE    );
  DEFAULT_FILE( badobject_file , BADOBJECT_FILE   );
  DEFAULT_FILE( failpass_file  , FAILPASS_FILE    );
  DEFAULT_FILE( database_file  , DATABASE_FILE    );
  DEFAULT_FILE( failenable_file, FAILENABLE_FILE  );
  DEFAULT_FILE( bad_file       , BAD_FILE         );
  DEFAULT_FILE( suspect_file   , SUSPECT_FILE     );
  DEFAULT_FILE( MOBProgs_dir   , MOBPROGS_DIR     );
  DEFAULT_FILE( skill_dir      , SKILL_DIR        );
  DEFAULT_FILE( skill_file     , SKILL_FILE       );
  DEFAULT_FILE( ins_dir        , INSTRUMENT_DIR   );
  DEFAULT_FILE( ins_file       , INSTRUMENT_FILE  );
  DEFAULT_FILE( wizflags_log   , WIZFLAGS_LOG_FILE);
  DEFAULT_FILE( motd_file      , MOTD_FILE        );
  RETURN_NULL();
}

#undef DEFAULT_FILE

#if defined( ADJUST )
#undef ADJUST
#endif

#define ADJUST( target )                             \
   sprintf( tempfile , "%s%s" , home_dir , target ); \
   str_cpy( target , tempfile );                     \

/* 修正檔案名稱並加上絕對路徑給他 */
void adjust_filename( void )
{
  char tempfile[MAX_FILE_LENGTH];

  PUSH_FUNCTION( "adjust_filename" );

  ADJUST( help_dir        );
  ADJUST( social_dir      );
  ADJUST( angel_dir       );
  ADJUST( area_dir        );
  ADJUST( board_dir       );
  ADJUST( greet_dir       );
  ADJUST( vote_dir        );
  ADJUST( joke_dir        );
  ADJUST( note_dir        );
  ADJUST( sector_dir      );
  ADJUST( class_dir       );
  ADJUST( liq_dir         );
  ADJUST( player_dir      );
  ADJUST( ideas_file      );
  ADJUST( typo_file       );
  ADJUST( newplayer_file  );
  ADJUST( symbol_file     );
  ADJUST( check_file      );
  ADJUST( wizard_file     );
  ADJUST( net_file        );
  ADJUST( bus_file        );
  ADJUST( donate_file     );
  ADJUST( gift_file       );
  ADJUST( date_file       );
  ADJUST( bounty_file     );
  ADJUST( event_file      );
  ADJUST( ship_file       );
  ADJUST( promotion_file  );
  ADJUST( purge_file      );
  ADJUST( quest_file      );
  ADJUST( immlist_file    );
  ADJUST( sale_file       );
  ADJUST( situs_file      );
  ADJUST( club_file       );
  ADJUST( internal_file   );
  ADJUST( site_file       );
  ADJUST( station_file    );
  ADJUST( question_file   );
  ADJUST( xname_file      );
  ADJUST( stock_file      );
  ADJUST( welcome_file    );
  ADJUST( welcome_imm     );
  ADJUST( address_file    );
  ADJUST( error_file      );
  ADJUST( hero_file       );
  ADJUST( bugs_file       );
  ADJUST( failload_file   );
  ADJUST( failexit_file   );
  ADJUST( badobject_file  );
  ADJUST( failpass_file   );
  ADJUST( database_file   );
  ADJUST( failenable_file );
  ADJUST( bad_file        );
  ADJUST( suspect_file    );
  ADJUST( xname_log       );
  ADJUST( chat_log        );
  ADJUST( suicide_log     );
  ADJUST( MOBProgs_dir    );
  ADJUST( skill_dir       );
  ADJUST( ins_dir         );
  ADJUST( wizflags_log    );
  ADJUST( motd_file       );

  if ( PlayerArea[0] ) ADJUST( PlayerArea      );
  RETURN_NULL();
}

#undef ADJUST

/* 讀入參考檔案 */
void read_ini( const char * filename )
{
  int    flag;
  int    loop;
  char   command[ MAX_LENGTH ];
  char   argument[ MAX_LENGTH ];
  FILE * pFile;

  PUSH_FUNCTION( "read_ini" );

  if ( access( filename , R_OK ) != 0 )
    mudlog( LOG_ERR , "無法開啟啟始檔 %s。\n" , filename );

  if ( !( pFile = fopen( filename , "r" ) ) )
    mudlog( LOG_ERR , "無法讀取啟始檔 %s。\n" , filename );

  while ( !feof( pFile ) )
  {
    flag = fread_line( pFile , command , argument );

    if ( command[0] == '#' || command[0] == '\x0' ) continue;
    else if ( flag == FALSE ) mudlog( LOG_ERR , "啟始檔格式錯誤。\n" );

    smash_char( command  , ' ' );
    smash_char( argument , ' ' );
    exec_ini( command , argument );
  }

  fclose( pFile );

  if ( merc_ipc == 0 ) mudlog( LOG_ERR, "你必須註明共享記憶體."  );
  if ( !mud_name[0]  ) mudlog( LOG_ERR, "你必須註明本MUD的名稱." );

  for ( loop = 0; loop < MAX_PORT; loop++ )
    if ( MudPort[loop] != ERRORCODE ) break;

  if ( loop == MAX_PORT ) mudlog( LOG_ERR, "你必須註明通訊埠位址." );

  RETURN_NULL();
}

/* 從檔案中讀入一行 */
int fread_line( FILE * pFile , char * command , char * argument )
{
  char   Char;
  int    length;
  bool   turn;
  char * target;

  PUSH_FUNCTION( "fread_line" );

  *command = *argument = '\x0';

  do
  {
    Char = getc( pFile );
  } while ( isspace( ( int ) Char ) && Char != EOF );

  if ( Char != EOF ) ungetc( Char , pFile );

  for ( length = 0, target = command, turn = FALSE; length < MAX_LENGTH - 2; )
  {
    switch( ( Char = getc( pFile ) ) )
    {
    case EOF :
    case '\n':
    case '\r':

      *target = '\x0';
      RETURN( turn );

    case '\b':
    case '\a':
    case '\v':
    case '\f':
      break;

    case '=' :

      *target = '\x0';
      if ( turn == TRUE ) RETURN( FALSE );

      length =        0;
      turn   =     TRUE;
      target = argument;
      break;

    case ' ' :
    case '\t':

      if ( length > 0 && *(command - 1 ) != ' ' )
      {
        *target++ = ' ';
        length++;
      }
      break;

    default:

      *target++ = Char;
      length++;
      break;
    }
  }

  mudlog( LOG_ERR , "啟始檔長度超過系統內定值﹐可能是非文件檔。\n" );
  exit( -1 );
}

#if defined( CMP )
#undef CMP
#endif

#if defined ( READNUM )
#undef READNUM
#endif

#define CMP( literal, field, value )   \
  if ( !str_cmp( command, literal ) )  \
  {                                    \
    str_cpy( field , value );          \
    fMatch = TRUE;                     \
  }

#define READNUM( literal, field, value )   \
  if ( !str_cmp( command, literal ) )      \
  {                                        \
    field = atoi( value );                 \
    fMatch = TRUE;                         \
  }

/* 讀入資料 */
void exec_ini( char * command , char * argument )
{
  extern  bool    signal_segv;
  extern  bool    signal_bus;
  extern  bool    signal_fpe;
  extern  bool    signal_ill;
  extern  int     stock_win;
  extern  int     stock_lost;
  char            arg[MAX_INPUT_LENGTH];
  int             loop;
  int             port;
  bool            fMatch = FALSE;

  PUSH_FUNCTION( "exec_ini" );

  if ( *command == '\x0' || *argument == '\x0' )
    mudlog( LOG_ERR , "你的啟始檔案格式有問題﹐請檢查你的啟始檔。\n" );

  switch( UPPER( command[0] ) )
  {
  case '*' :

    fMatch = TRUE;
    break;

  case 'A':

    CMP( "Area directory" , area_dir     , argument );
    CMP( "Address file"   , address_file , argument );
    CMP( "Angel directory", angel_dir    , argument );
    CMP( "Angel Extension", angel_ext    , argument );

    if ( !str_cmp( command, "Auction Gold" ) )
    {
      if ( ( AuctionGold = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "拍賣金額 %d 太少.", AuctionGold );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Alignment Different" ) )
    {
      if ( ( AlignmentDiff = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "陣營差異 %d 太少.", AlignmentDiff );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Alignment Range" ) )
    {
      if ( ( AlignRange = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "陣營差異 %d 太少.", AlignRange );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Auction Times" ) )
    {
      if ( ( AuctionTimes = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "拍賣次數 %d 太少.", AuctionTimes );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Angel Times" ) )
    {
      if ( ( AngelTimes = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "守護神來的週期 %d 不合理.", AngelTimes );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Angel Default" ) )
    {
      if ( ( AngelDefault = atoi( argument ) ) <= 10 )
        mudlog( LOG_ERR, "玩家預設守護神來的週期 %d 不合理.", AngelDefault );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Angel Level" ) )
    {
      if ( ( AngelLevel = atoi( argument ) ) <= 0 || AngelLevel > MAX_LEVEL )
        mudlog( LOG_ERR, "守護神等級 %d 不合理.", AngelLevel );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Ask Cost" ) )
    {
      if ( ( AskCost = atoi( argument ) ) <= 0 || AskCost > 200 )
        mudlog( LOG_ERR, "打工保證金倍數 %d 不合法.", AskCost );

      fMatch = TRUE;
      break;
    }
    if ( !str_cmp( command, "Attack Value" ) )
    {
      AttackValue = ( atoi( argument ) == FALSE ) ? FALSE : TRUE;
      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Auto Backup" ) )
    {
      AutoBackup = ( atoi( argument ) == FALSE ) ? FALSE : TRUE;
      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Auth Port" ) )
    {
      if ( ( AuthPort = atoi( argument ) ) <= 0 || AuthPort > 1024 )
        mudlog( LOG_ERR, "你的查詢埠 %d 不合法." , AuthPort );

      fMatch = TRUE;
      break;
    }

    break;

  case 'B':

    CMP( "Bad File"         , bad_file      , argument );
    CMP( "Bad Object File"  , badobject_file, argument );
    CMP( "BBS Name"         , bbs_name      , argument );
    CMP( "Board directory"  , board_dir     , argument );
    CMP( "Board List"       , board_lst     , argument );
    CMP( "Board Sheet"      , board_sheet   , argument );
    CMP( "Board Index"      , board_index   , argument );
    CMP( "Bounty File"      , bounty_file   , argument );
    CMP( "Bugs file"        , bugs_file     , argument );
    CMP( "Bus file"         , bus_file      , argument );

    if ( !str_cmp( command, "Born Age" ) )
    {
      if ( ( BornAge = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "預設年齡 %d 不合理.", BornAge );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Bank Threshold" ) )
    {
      if ( ( BankThreshold = atoi( argument ) ) <= 0
        || BankThreshold > MaxNumber )
        mudlog( LOG_ERR, "存款上限 %d 不合理.", BankThreshold );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Bank Profit" ) )
    {
      if ( ( BankProfit = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "存款利息 %d 不合理.", BankProfit );

      fMatch = TRUE;
      break;
    }

    break;

  case 'C':

    if ( !str_cmp( command, "Club Create Fund" ) )
    {
      if ( ( ClubCreateFund = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "創建幫派費用 %d 不合法.", ClubCreateFund );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Club Attr Gain" ) )
    {
      if ( ( ClubAttrGain = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "幫派屬性倍數 %d 不合法.", ClubAttrGain );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Club Countersign Fund" ) )
    {
      if ( ( ClubCountersignFund = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "連署幫派費用 %d 不合法.", ClubCountersignFund );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Club Countersign Count" ) )
    {
      if ( ( ClubCountersignCount = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "連署人數費用 %d 不合法.", ClubCountersignCount );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Club Expense" ) )
    {
      if ( ( ClubExpense = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "參加幫派費用 %d 不合法.", ClubExpense );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command , "Chat penalty" ) )
    {
      if ( ( chat_penalty = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "你的重複對話懲罰 %d 不合法." , chat_penalty );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command , "Club Countersign Day" ) )
    {
      if ( ( ClubCountersignDay = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "你的幫派連署天數 %d 不合法." , ClubCountersignDay );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Chat Log" ) )
    {
      ChatLog = ( atoi( argument ) == FALSE ) ? FALSE : TRUE;
      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Club room" ) )
    {
      if ( ( club_room = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "系統分配給幫派房間 %d 不合理.", club_room );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Check Server" ) )
    {
      CheckServer = ( atoi( argument ) == FALSE ) ? FALSE : TRUE;
      fMatch      = TRUE;
      break;
    }

    if ( !str_cmp( command, "Core Dump" ) )
    {
      CoreDump = ( atoi( argument ) == FALSE ) ? FALSE : TRUE;
      fMatch   = TRUE;
      break;
    }

    CMP( "Check file"      , check_file , argument );
    CMP( "Class directory" , class_dir  , argument );
    CMP( "Class Extension" , class_ext  , argument );
    CMP( "Club file"       , club_file  , argument );
    CMP( "Chat log"        , chat_log   , argument );

    READNUM( "Channel Auction"  , ChannelAuction  , argument );
    READNUM( "Channel Chat"     , ChannelChat     , argument );
    READNUM( "Channel Bulletin" , ChannelBulletin , argument );
    READNUM( "Channel Immtalk"  , ChannelImmtalk  , argument );
    READNUM( "Channel Music"    , ChannelMusic    , argument );
    READNUM( "Channel Question" , ChannelQuestion , argument );
    READNUM( "Channel Shout"    , ChannelShout    , argument );
    READNUM( "Channel Yell"     , ChannelYell     , argument );
    READNUM( "Channel Gamble"   , ChannelGamble   , argument );
    READNUM( "Channel Class"    , ChannelClass    , argument );
    READNUM( "Channel Club"     , ChannelClub     , argument );
    READNUM( "Channel Semote"   , ChannelSemote   , argument );
    READNUM( "Channel Weather"  , ChannelWeather  , argument );
    READNUM( "Channel Phone"    , ChannelPhone    , argument );
    READNUM( "Channel Suicide"  , ChannelSuicide  , argument );
    READNUM( "Channel Rumor"    , ChannelRumor    , argument );
    READNUM( "Channel Notice"   , ChannelNotice   , argument );
    READNUM( "Channel Group"    , ChannelGroup    , argument );
    READNUM( "Channel PK"       , ChannelPK       , argument );

    READNUM( "Config AutoExit"  , ConfigAutoExit  , argument );
    READNUM( "Config AutoLoot"  , ConfigAutoLoot  , argument );
    READNUM( "Config AutoSac"   , ConfigAutoSac   , argument );
    READNUM( "Config Blank"     , ConfigBlank     , argument );
    READNUM( "Config Brief"     , ConfigBrief     , argument );
    READNUM( "Config Combine"   , ConfigCombine   , argument );
    READNUM( "Config Prompt"    , ConfigPrompt    , argument );
    READNUM( "Config Exact"     , ConfigExact     , argument );
    READNUM( "Config Message"   , ConfigMessage   , argument );
    READNUM( "Config Flee"      , ConfigFlee      , argument );
    READNUM( "Config Angel"     , ConfigAngel     , argument );
    READNUM( "Config AutoFood"  , ConfigAutoFood  , argument );
    READNUM( "Config AutoDrink" , ConfigAutoDrink , argument );
    READNUM( "Config Rebirth"   , ConfigRebirth   , argument );
    READNUM( "Config Train"     , ConfigTrain     , argument );
    READNUM( "Config Practice"  , ConfigPractice  , argument );
    READNUM( "Config Ansi"      , ConfigAnsi      , argument );
    READNUM( "Config Lotto"     , ConfigLotto     , argument );

    break;

  case 'D' :

    CMP( "Directory file" , dir_file     , argument );
    CMP( "Donate file"    , donate_file  , argument );
    CMP( "Default Email"  , DefaultEmail , argument );
    CMP( "Default Unit"   , DefaultUnit  , argument );
    CMP( "Date File"      , date_file    , argument );
    CMP( "Database file"  , database_file, argument );

    READNUM( "Deposit Money"    , DepositMoney, argument );
    READNUM( "Default Alignment", DefaultAlign, argument );

    if ( !str_cmp( command, "Dosage Update" ) )
    {
      if ( ( DosageUpdate = atoi( argument ) ) < 0 )
        mudlog( LOG_ERR, "劑量 %d 不合法." , DosageUpdate );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Dosage Limit" ) )
    {
      if ( ( DosageLimit = atoi( argument ) ) < 0 )
        mudlog( LOG_ERR, "劑量 %d 不合法." , DosageLimit );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Default Gold" ) )
    {
      if ( ( DefaultGold = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "預設金錢 %d 不合理.", DefaultGold );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Default Hit" ) )
    {
      if ( ( DefaultHit = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "預設生命力 %d 不合理.", DefaultHit );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Default Mana" ) )
    {
      if ( ( DefaultMana = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "預設法力 %d 不合理.", DefaultMana );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Default Move" ) )
    {
      if ( ( DefaultMove = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "預設體力 %d 不合理.", DefaultMove );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Default Practice" ) )
    {
      if ( ( DefaultPractice = atoi( argument ) ) < 0 )
        mudlog( LOG_ERR, "預設訓練點數 %d 不合理.", DefaultPractice );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Default Str" ) )
    {
      if ( ( DefaultStr = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "預設力量 %d 不合理.", DefaultStr );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Default Int" ) )
    {
      if ( ( DefaultInt = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "預設智慧 %d 不合理.", DefaultInt );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Default Wis" ) )
    {
      if ( ( DefaultWis = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "預設學識 %d 不合理.", DefaultWis );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Default Dex" ) )
    {
      if ( ( DefaultDex = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "預設敏捷 %d 不合理.", DefaultDex );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Default Con" ) )
    {
      if ( ( DefaultCon = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "預設體格 %d 不合理.", DefaultCon );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Default Object" ) )
    {
      for ( loop = 0; loop < MAX_DEFAULT_OBJECT; loop++ )
        if ( DefaultObject[loop] < 0 ) break;

      if ( loop >= MAX_DEFAULT_OBJECT )
        mudlog( LOG_ERR, "預設物品已經超出系統預定值." );

      if ( ( DefaultObject[loop] = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "預設物品序號 %d 不合理.", DefaultObject[loop] );

      fMatch = TRUE;
      break;
    }

    break;

  case 'E' :

    CMP( "Error file", error_file, argument );
    CMP( "Event File", event_file, argument );

    if ( !str_cmp( command, "Eq Search Cost" ) )
    {
      if ( ( EQSearchCost = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "打聽裝備的收費 %d 不合法.", EQSearchCost );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Endow Rate" ) )
    {
      if ( ( EndowRate = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "捐輸 %d 的比例不合理.", EndowRate );

      fMatch = TRUE;
      break;
    }

    break;

  case 'F':

    CMP( "Failload file"     , failload_file  , argument );
    CMP( "Failexit file"     , failexit_file  , argument );
    CMP( "Failpass file"     , failpass_file  , argument );
    CMP( "FailEnable file"   , failenable_file, argument );
    CMP( "From Unknown"      , FromUnknown    , argument );

    READNUM( "Find Cost", FindCost, argument );

    if ( !str_cmp( command, "FQDN Limit" ) )
    {
      fqdn_limit = ( atoi( argument ) == FALSE ) ? FALSE : TRUE;
      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Fight Round" ) )
    {
      if ( ( FightRound = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "對戰回合 %d 不合理.", FightRound );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Force Level" ) )
    {
      if ( ( ForceLevel = atoi( argument ) ) <= 0
        || level_limit >= MAX_LEVEL )
        mudlog( LOG_ERR, "強制轉職等級不合理 %d.", ForceLevel );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Follow Level" ) )
    {
      if ( ( FollowLevel = atoi( argument ) ) <= 1
        || level_limit >= MAX_LEVEL )
        mudlog( LOG_ERR, "跟隨等級不合理 %d.", FollowLevel );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Foggy Room" ) )
    {
      if ( FoggyRoom > 100 )
        mudlog( LOG_ERR, "霧的機率 %d 不合理.", FoggyRoom );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Fight Different" ) )
    {
      if ( ( FightDifferent = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "對戰相差等級 %d 不合理.", FightDifferent );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Fight Rate" ) )
    {
      if ( ( FightRate = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "對戰剩餘生命力比率 %d 不合理.", FightRate );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Food Degrade" ) )
    {
      if ( ( FoodDegrade = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "食物腐敗時間 %d 不合理.", FoodDegrade );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "File Quota" ) )
    {
      if ( ( file_quota = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "玩家檔案大小限制不合法.", file_quota );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Finger Port" ) )
    {
      if ( ( FingerPort = atoi( argument ) ) <= 0 || FingerPort > 1024 )
        mudlog( LOG_ERR, "你的查詢埠 %d 不合法." , FingerPort );

      fMatch = TRUE;
      break;
    }

    break;

  case 'G':

    if ( !str_cmp( command, "Game lock" ) )
    {
      wizlock = ( atoi( argument ) == FALSE ) ? FALSE : TRUE;
      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Get FQDN" ) )
    {
      GetFQDN = ( atoi( argument ) == FALSE ) ? FALSE : TRUE;
      fMatch  = TRUE;
      break;
    }

    CMP( "Greet directory" , greet_dir , argument );
    CMP( "Greet Extension" , greet_ext , argument );

    if ( !str_cmp( command, "Group Read" ) )
    {
      if ( atoi( argument ) == TRUE ) FileMode |= S_IRGRP;

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "GroupSpiltExp" ) )
    {
      GroupSpiltExp = ( atoi( argument ) == FALSE ) ? FALSE : TRUE;
      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Group Write" ) )
    {
      if ( atoi( argument ) == TRUE ) FileMode |= S_IWGRP;

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command , "Gold Weight" ) )
    {
      if ( ( GoldWeight = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "黃金重量 %d 不合理.", GoldWeight );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command , "Gamble Lost" ) )
    {
      if ( ( GambleLost = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "賭博賠率 %d 不合理.", GambleLost );

      fMatch = TRUE;
      break;
    }

    CMP( "Gift File", gift_file, argument );
    break;

  case 'H':

    CMP(     "Home directory" , home_dir ,     argument );
    CMP(     "Hero File"      , hero_file,     argument );
    CMP(     "Help directory" , help_dir ,     argument );
    CMP(     "Help Extension" , help_ext ,     argument );
    READNUM( "Home Gold",       HomeGold,      argument );
    READNUM( "Home Object",     HomeObject,    argument );
    READNUM( "Home Mobile",     HomeMobile,    argument );
    READNUM( "Home Gold Title", HomeGoldTitle, argument );
    READNUM( "Home Gold Descr", HomeGoldDescr, argument );
    READNUM( "Home Gold Link",  HomeGoldLink,  argument );
    READNUM( "Home Man Max",    HomeManMax,    argument );

    if ( !str_cmp( command , "Hold day" ) )
    {
      if ( ( hold_day = atoi( argument ) ) <= 10 )
        mudlog( LOG_ERR, "玩家保留檔案天數 %d 不合法." , hold_day );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command , "Hero Firman" ) )
    {
      if ( ( HeroFirman = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "英雄增加免死金牌數 %d 不合理" , HeroFirman );

      fMatch = TRUE;
      break;
    }

    break;

  case 'I':

    READNUM( "Increase Limit", IncreaseLimit, argument );

    if ( !str_cmp( command , "Ipc key" ) )
    {
      if ( ( merc_ipc = atoi( argument ) ) <= 1 )
        mudlog( LOG_ERR, "你的 merc ipc 不合法." , merc_ipc );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command , "Ipc Two" ) )
    {
      if ( ( merc_ipc_2 = atoi( argument ) ) <= 1 )
        mudlog( LOG_ERR, "你的 merc ipc 2 不合法." , merc_ipc_2 );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Increase" ) )
    {
      if ( ( Increase = atoi( argument ) ) < 0 )
        mudlog( LOG_ERR, "增值 %d 不合法.", Increase );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command , "IPC block" ) )
    {
      if ( ( ipc_block = atoi( argument ) ) <= 1 )
        mudlog( LOG_ERR, "你的 IPC 區塊數目 %d 不合法." , ipc_block );
      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command , "IPC idle" ) )
    {
      if ( ( ipc_idle = atoi( argument ) ) <= 1 )
        mudlog( LOG_ERR, "你的共享記憶體閒置秒數 %d 不合法." , ipc_idle );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Internal Debug" ) )
    {
      internal_debug = ( atoi( argument ) == FALSE ) ? FALSE : TRUE;
      fMatch         = TRUE;
      break;
    }

    CMP( "Immlist file"         , immlist_file , argument );
    CMP( "Index file"           , index_file   , argument );
    CMP( "Internal file"        , internal_file, argument );
    CMP( "Instrument directory" , ins_dir      , argument );
    CMP( "Instrument file"      , ins_file     , argument );
    CMP( "Instrument Extension" , command_ext  , argument );
    CMP( "Ideas file"           , ideas_file   , argument );
    break;

  case 'J':
    CMP( "Joke directory" , joke_dir , argument );
    CMP( "Joke Extension" , joke_ext , argument );
    break;

  case 'L':

    CMP( "Liquid directory", liq_dir    , argument );
    CMP( "Liquid Extension", liquid_ext , argument );

    if ( !str_cmp( command, "LeftSkill Cost" ) )
    {
      if ( ( LeftSkillCost = atoi( argument ) ) <= 0 || LeftSkillCost > 100 )
        mudlog( LOG_ERR, "打聽剩餘技能花費倍數 %d 不合法.", LeftSkillCost );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Lotto Paper" ) )
    {
      if ( ( lotto_paper = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "樂透快報價格 %d 不合理.", lotto_paper );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Lotto First" ) )
    {
      if ( ( LottoFirst = atoi( argument ) ) <= 0
        || LottoFirst > MAX_FIRMAN )
        mudlog( LOG_ERR, "樂透第一特獎 %d 不合理.", LottoFirst );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Lotto Second" ) )
    {
      if ( ( LottoSecond = atoi( argument ) ) <= 0
        || LottoSecond > MAX_FIRMAN )
        mudlog( LOG_ERR, "樂透第二特獎 %d 不合理.", LottoSecond );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Lotto Third" ) )
    {
      if ( ( LottoThird = atoi( argument ) ) <= 0
        || LottoThird > MAX_FIRMAN )
        mudlog( LOG_ERR, "樂透第三特獎 %d 不合理.", LottoThird );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Lotto Forth" ) )
    {
      if ( ( LottoForth = atoi( argument ) ) <= 0
        || LottoForth > MAX_FIRMAN )
        mudlog( LOG_ERR, "樂透第四特獎 %d 不合理.", LottoForth );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Load HitPlus" ) )
    {
      if ( ( load_hit_plus = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "生命力倍數 %d 不合理.", load_hit_plus );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Load ManaPlus" ) )
    {
      if ( ( load_mana_plus = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "法力倍數 %d 不合理.", load_mana_plus );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Load MovePlus" ) )
    {
      if ( ( load_move_plus = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "移動力倍數 %d 不合理.", load_move_plus );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Level limit" ) )
    {
      if ( ( level_limit = atoi( argument ) ) <= 0
        || level_limit > MAX_LEVEL )
        mudlog( LOG_ERR, "限制等級不合理 %d.", level_limit );

      fMatch = TRUE;
      break;
    }
    break;

  case 'M':

    if ( !str_cmp( command, "Mount Cost" ) )
    {
      if ( ( MountCost = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "騎馬費體力 %d 不合法.", MountCost );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command , "Marry Age" ) )
    {
      if ( ( MarryAge = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "你的結婚最低年齡 %d 不合法." , MarryAge );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command , "Maxdesc" ) )
    {
      if ( ( max_connect = atoi( argument ) ) <= 1 )
        mudlog( LOG_ERR, "你的最大連線人數 %d 不合法." , max_connect );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command , "Max Login Error" ) )
    {
      if ( ( MaxLoginError = atoi( argument ) ) <= 3 )
        mudlog( LOG_ERR, "你的連線錯誤 %d 不合法." , MaxLoginError );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command , "Max Hero Show" ) )
    {
      if ( ( MaxHeroShow = atoi( argument ) ) <= 10 )
        mudlog( LOG_ERR, "你的顯示英雄數 %d 不合法." , MaxHeroShow );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command , "Max Login Idle" ) )
    {
      MaxLoginIdle = -atoi( argument );
      fMatch       = TRUE;
      break;
    }

    if ( !str_cmp( command , "Max Playing Idle" ) )
    {
      if ( ( MaxPlayingIdle = atoi( argument ) ) <= 10 )
        mudlog( LOG_ERR, "你的最大連線閒置 %d 不合法." , MaxPlayingIdle );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command , "Max Corpse Idle" ) )
    {
      if ( ( MaxCorpseIdle = atoi( argument ) ) <= 10 )
        mudlog( LOG_ERR, "你的最大屍體閒置 %d 不合法." , MaxCorpseIdle );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command , "Max Game Idle" ) )
    {
      if ( ( MaxGameIdle = atoi( argument ) ) <= 10 )
        mudlog( LOG_ERR, "你的最大遊戲閒置 %d 不合法." , MaxGameIdle );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command , "Max Repeat" ) )
    {
      if ( ( MaxRepeat = atoi( argument ) ) <= 10 )
        mudlog( LOG_ERR, "你的重複次數 %d 不合理.", MaxRepeat );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Max Alias" ) )
    {
      if ( ( max_alias = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "最多巨集數目 %d 不合理.", max_alias );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Max Auction Gold" ) )
    {
      if ( ( MaxAuctionGold = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "最大拍賣金 %d 不合理.", MaxAuctionGold );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Max Hitroll" ) )
    {
      if ( ( MaxHitroll = atoi( argument ) ) <= 100 )
        mudlog( LOG_ERR, "最大攻擊加成 %d 不合理.", MaxHitroll );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Max IP" ) )
    {
      if ( ( max_ip = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "最多限制連線位址數目 %d 不合理.", max_ip );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Max PK Gold" ) )
    {
      if ( ( MaxPKGold = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "PK 最大押金 %d 不合理.", MaxPKGold );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Max PK Total" ) )
    {
      if ( ( MaxPKTotal = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "PK 最大總押金 %d 不合理.", MaxPKTotal );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Max Friend" ) )
    {
      if ( ( max_friend = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "最多好友名單數目 %d 不合理.", max_friend );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Max AliasRepeat" ) )
    {
      if ( ( alias_repeat = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "最多巨集數目 %d 不合理.", alias_repeat );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Max Club" ) )
    {
      if ( ( max_club = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "最多幫派數目 %d 不合理.", max_club );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Max Notes" ) )
    {
      if ( ( max_notes = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "最多信件數目數目 %d 不合理.", max_notes );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Max Board" ) )
    {
      if ( ( max_board = atoi( argument ) ) <= 0
        || max_board > MAX_BOARD )
        mudlog( LOG_ERR, "最多版面數目 %d 不合理.", max_board );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Max Enable" ) )
    {
      if ( ( max_enable = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "最多致能數目 %d 不合理.", max_enable );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Max Hire" ) )
    {
      if ( ( max_hire = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "最多傭兵數目 %d 不合理.", max_hire );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command , "Max Innate" ) )
    {
      if ( ( aInnate = atoi( argument ) ) < 0 || aInnate > MAX_INNATE )
        mudlog( LOG_ERR, "你的預設技能數目 %d 不合法." , aInnate );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command , "Max Firman" ) )
    {
      if ( ( MaxFirman = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "最大金牌數目 %d 不合法." , MaxFirman );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command , "Max Practice" ) )
    {
      if ( ( MaxPractice = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "最大學習點數 %d 不合法." , MaxPractice );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command , "Max NSkill" ) )
    {
      if ( ( MaxNSkill = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "最大額外技能總數 %d 不合法." , MaxNSkill );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Max Hit" ) )
    {
      if ( ( MaxHit = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "你的最大生命力 %d 不合法." , MaxHit );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Max Mana" ) )
    {
      if ( ( MaxMana = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "你的最大法力 %d 不合法." , MaxMana );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Max Move" ) )
    {
      if ( ( MaxMove = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "你的最大體力 %d 不合法." , MaxMove );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Max Str" ) )
    {
      if ( ( MaxStr = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "你的最大力量 %d 不合法." , MaxStr );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Max Int" ) )
    {
      if ( ( MaxInt = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "你的最大智慧 %d 不合法." , MaxInt );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Max Wis" ) )
    {
      if ( ( MaxWis = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "你的最大學識 %d 不合法." , MaxWis );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Max Dex" ) )
    {
      if ( ( MaxDex = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "你的最大敏捷 %d 不合法." , MaxDex );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Max Con" ) )
    {
      if ( ( MaxCon = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "你的最大體格 %d 不合法." , MaxCon );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Max Cname Len" ) )
    {
      if ( ( MaxCNameLen = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "玩家中文名字最大長度 %d 不合理.", MaxCNameLen );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command , "Max EqCapcity" ) )
    {
      if ( ( eq_max_capcity = atoi( argument ) ) < 0
        || eq_max_capcity > 100 )
        mudlog( LOG_ERR, "你的裝備最大吸收度 %d 不合法." , eq_max_capcity );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command , "Max Room Player" ) )
    {
      if ( ( MaxRoomPlayer = atoi( argument ) ) < 0 )
        mudlog( LOG_ERR, "房間最多人數 %d 不合法." , MaxRoomPlayer );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command , "Max Room Object" ) )
    {
      if ( ( MaxRoomObject = atoi( argument ) ) < 0 )
        mudlog( LOG_ERR, "房間最多物品數 %d 不合法." , MaxRoomObject );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command , "Max Where Gold" ) )
    {
      if ( ( MaxWhereGold = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "打聽怪物的最大金額 %d 不合法.", MaxWhereGold );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command , "Min Where Gold" ) )
    {
      if ( ( MinWhereGold = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "打聽怪物的最少金額 %d 不合法.", MinWhereGold );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Min PK Gold" ) )
    {
      if ( ( MinPKGold = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "PK 最小押金 %d 不合理.", MinPKGold );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Min Cname Len" ) )
    {
      if ( ( MinCNameLen = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "玩家中文名字最小長度 %d 不合理.", MinCNameLen );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Min PassWord Len" ) )
    {
      if ( ( MinPasswordLen = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "玩家密碼最大長度 %d 不合理.", MinPasswordLen );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command , "Min EqCapcity" ) )
    {
      if ( ( eq_min_capcity = atoi( argument ) ) < 0
        || eq_min_capcity > 100 )
        mudlog( LOG_ERR, "你的裝備最小吸收度 %d 不合法." , eq_min_capcity );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Mud Port" ) )
    {
      port = atoi( argument );

      for ( loop = 0; loop < MAX_PORT; loop++ )
      {
        if ( MudPort[loop] == port )
          mudlog( LOG_ERR, "你的通訊埠 %d 重複" , port );

        if ( MudPort[loop] == ERRORCODE ) break;
      }

      if ( loop == MAX_PORT )
        mudlog( LOG_ERR, "超過最大連接數目 %d.", MAX_PORT );

      port = atoi( argument );

      if ( ( port = atoi( argument ) ) <= 1024 || port > 65000 )
        mudlog( LOG_ERR, "你的通訊埠 %d 不合法." , port );

      MudPort[loop] = port;
      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Multi login" ) )
    {
      multilogin = ( atoi( argument ) == FALSE ) ? FALSE : TRUE;
      fMatch = TRUE;
      break;
    }

    READNUM( "Mob Decrease", MobDecrease    , argument );
    READNUM( "Mob Vampire" , MobVampireVnum , argument );
    READNUM( "Mob Practice", MobPracticeVnum, argument );

    CMP( "Mobprogs directory" , MOBProgs_dir , argument );
    CMP( "Mob directory"      , mob_dir      , argument );
    CMP( "Mob Extension"      , mob_ext      , argument );
    CMP( "Mineral directory"  , mineral_dir  , argument );
    CMP( "Mineral Extension"  , mineral_ext  , argument );
    CMP( "Motd File"          , motd_file    , argument );
    break;

  case 'N':

    CMP( "Name"           , mud_name       , argument );
    CMP( "Net file"       , net_file       , argument );
    CMP( "Newplayer file" , newplayer_file , argument );
    CMP( "Note directory" , note_dir       , argument );
    CMP( "Note Extension" , note_ext       , argument );
    CMP( "Null file"      , null_file      , argument );

    if ( !str_cmp( command, "New Lock" ) )
    {
      NewLock = ( atoi( argument ) == FALSE ) ? FALSE : TRUE;
      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Note Level" ) )
    {
      if ( ( NoteLevel = atoi( argument ) ) <= 0 || NoteLevel > MAX_LEVEL )
        mudlog( LOG_ERR, "寫信最低等級 %d 不合理.", NoteLevel );

      fMatch = TRUE;
      break;
    }

    READNUM( "Notify Debug"    , NotifyDebug     , argument );
    READNUM( "Notify Info"     , NotifyInfo      , argument );
    READNUM( "Notify Wizard"   , NotifyWizard    , argument );
    READNUM( "Notify Newplayer", NotifyNewplayer , argument );
    READNUM( "Notify Failpass" , NotifyFailpass  , argument );
    READNUM( "Notify Emerg"    , NotifyEmerg     , argument );
    READNUM( "Notify Crit"     , NotifyCrit      , argument );
    READNUM( "Notify Err"      , NotifyErr       , argument );
    READNUM( "Notify Xname"    , NotifyXname     , argument );
    READNUM( "Notify Net"      , NotifyNet       , argument );

    break;

  case 'O':

    CMP( "Object directory"     , obj_dir     , argument );
    CMP( "Object Extension"     , obj_ext     , argument );

    READNUM( "Object Protype"    , ObjProtypeVnum    , argument );
    READNUM( "Object Money"      , ObjMoneyVnum      , argument );
    READNUM( "Object MoneySome"  , ObjMoneySomeVnum  , argument );
    READNUM( "Object Corpse NPC" , ObjCorpseNPCVnum  , argument );
    READNUM( "Object Corpse PC"  , ObjCorpsePCVnum   , argument );
    READNUM( "Object Head"       , ObjHeadVnum       , argument );
    READNUM( "Object Heart"      , ObjHeartVnum      , argument );
    READNUM( "Object Arm"        , ObjArmVnum        , argument );
    READNUM( "Object Leg"        , ObjLegVnum        , argument );
    READNUM( "Object Turd"       , ObjTurdVnum       , argument );
    READNUM( "Object Mushroom"   , ObjMushroomVnum   , argument );
    READNUM( "Object Light"      , ObjLightVnum      , argument );
    READNUM( "Object Spring"     , ObjSpringVnum     , argument );
    READNUM( "Object Dumpling"   , ObjDumplingVnum   , argument );
    READNUM( "Object Bougi"      , ObjBougiVnum      , argument );
    READNUM( "Object Pon"        , ObjPonVnum        , argument );
    READNUM( "Object Chicken"    , ObjChickenVnum    , argument );
    READNUM( "Object MagicStone" , ObjMagicStoneVnum , argument );
    READNUM( "Object Meat"       , ObjMeatVnum       , argument );
    READNUM( "Object Letter"     , ObjLetterVnum     , argument );
    READNUM( "Object Urn"        , ObjUrnVnum        , argument );
    READNUM( "Over Loading"      , OverLoading       , argument );

    if ( !str_cmp( command, "Other Read" ) )
    {
      if ( atoi( argument ) == TRUE ) FileMode |= S_IROTH;

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Other Write" ) )
    {
      if ( atoi( argument ) == TRUE ) FileMode |= S_IWOTH;

      fMatch = TRUE;
      break;
    }

    break;

  case 'P':

    CMP( "Player directory" , player_dir    , argument );
    CMP( "Promotion file"   , promotion_file, argument );
    CMP( "Purge file"       , purge_file    , argument );
    CMP( "Perm File"        , PermFile      , argument );

    READNUM( "Ply Quota"  , PlyQuota  , argument );
    READNUM( "Ply Penalty", PlyPenalty, argument );
    READNUM( "Prof Cycle" , ProfCycle , argument );

    if ( !str_cmp( command, "Player Angel" ) )
    {
      PlayerAngel = ( atoi( argument ) == FALSE ) ? FALSE : TRUE;
      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Player Area" ) )
    {
      if ( area_dir[0] == '\x0' )
        mudlog( LOG_ERR, "有玩家區域目錄之前必須有區域目錄." );

      sprintf( PlayerArea, "%s%s", area_dir, argument );
      fMatch = TRUE;
      break;
    }

    READNUM( "Player Area Num", PlayerAreaNum, argument );
    READNUM( "Player Area Cap", PlayerAreaCap, argument );

    if ( !str_cmp( command, "Pills Per Day" ) )
    {
      if ( ( PillsPerDay = atoi( argument ) ) < 0 )
        mudlog( LOG_ERR, "每天藥量 %d 不合法." , PillsPerDay );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Pry Cost" ) )
    {
      if ( ( PryCost = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "打聽技能的錢 %d 不合理.", PryCost );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "PK Level" ) )
    {
      if ( ( pk_level = atoi( argument ) ) <= 0 || pk_level > LEVEL_HERO )
        mudlog( LOG_ERR, "你的PK等級 %d 不合法." , pk_level );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "PK Limit" ) )
    {
      if ( ( pk_limit = atoi( argument ) ) <= 0 || pk_limit > LEVEL_HERO )
        mudlog( LOG_ERR, "PK時場外人數 %d 不合法." , pk_limit );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "PK Contrast" ) )
    {
      if ( ( PkContrastLevel = atoi( argument ) ) <= 0
        || PkContrastLevel > LEVEL_HERO )
        mudlog( LOG_ERR, "PK時等級差 %d 不合法." , PkContrastLevel );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "PK Age" ) )
    {
      if ( ( pk_age = atoi( argument ) ) < BornAge )
        mudlog( LOG_ERR, "你的PK年齡 %d 不合法." , pk_age );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "PK Lock" ) )
    {
      fight_info->lock = ( atoi( argument ) == FALSE ) ? FALSE : TRUE;
      fMatch = TRUE;
      break;
    }

    break;

  case 'Q':

    CMP( "Quest file"   , quest_file   , argument );
    CMP( "Question file", question_file, argument );

    READNUM( "Question Timer", QuestionTimer, argument );

    if ( !str_cmp( command, "Question Lock" ) )
    {
      QuestionLock = ( atoi( argument ) == FALSE ) ? FALSE : TRUE;
      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Question False" ) )
    {
      if ( ( QuestionFalse = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "問題答錯次數 %d 不合法." , QuestionFalse );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Question Alarm" ) )
    {
      if ( ( QuestionAlarm = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "問題提醒間隔 %d 不合法." , QuestionAlarm );

      fMatch = TRUE;
      break;
    }

    break;

  case 'R' :

    CMP( "Room directory"  , room_dir  , argument );
    CMP( "Room Extension"  , room_ext  , argument );
    CMP( "Reset directory" , reset_dir , argument );
    CMP( "Reset Extension" , reset_ext , argument );

    READNUM( "Recall0", RecallPlace[ UMIN( 0, MAX_RECALL ) ], argument );
    READNUM( "Recall1", RecallPlace[ UMIN( 1, MAX_RECALL ) ], argument );
    READNUM( "Recall2", RecallPlace[ UMIN( 2, MAX_RECALL ) ], argument );
    READNUM( "Recall3", RecallPlace[ UMIN( 3, MAX_RECALL ) ], argument );
    READNUM( "Recall4", RecallPlace[ UMIN( 4, MAX_RECALL ) ], argument );
    READNUM( "Recall5", RecallPlace[ UMIN( 5, MAX_RECALL ) ], argument );
    READNUM( "Recall6", RecallPlace[ UMIN( 6, MAX_RECALL ) ], argument );
    READNUM( "Recall7", RecallPlace[ UMIN( 7, MAX_RECALL ) ], argument );
    READNUM( "Recall8", RecallPlace[ UMIN( 8, MAX_RECALL ) ], argument );
    READNUM( "Recall9", RecallPlace[ UMIN( 9, MAX_RECALL ) ], argument );

    READNUM( "Room Jail"  , RoomJailVnum  , argument );
    READNUM( "Room Limbo" , RoomLimboVnum , argument );
    READNUM( "Room Dead"  , RoomDeadVnum  , argument );
    READNUM( "Room Corpse", RoomCorpseVnum, argument );
    READNUM( "Room Recall", RoomRecallVnum, argument );
    READNUM( "Room School", RoomSchoolVnum, argument );
    READNUM( "Room Fail"  , RoomFailVnum  , argument );
    READNUM( "Room Chat"  , RoomChatVnum  , argument );
    READNUM( "Record File", RecordFile    , argument );

    if ( !str_cmp( command , "Reboot Time" ) )
    {
      if ( ( reboot_time = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "系統重置時間 %d 不合法." , reboot_time );

      reboot_time *= 60 * 60;
      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command , "Rechristen" ) )
    {
      if ( ( Rechristen = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "改名次數 %d 不合法." , Rechristen );

      fMatch = TRUE;
      break;
    }

    break;

  case 'S' :

    CMP( "Sale file"        , sale_file   , argument );
    CMP( "Situs file"       , situs_file  , argument );
    CMP( "Sector directory" , sector_dir  , argument );
    CMP( "Sector Extension" , sector_ext  , argument );
    CMP( "Site file"        , site_file   , argument );
    CMP( "Station file"     , station_file, argument );
    CMP( "Ship file"        , ship_file   , argument );
    CMP( "Shop directory"   , shop_dir    , argument );
    CMP( "Shop Extension"   , shop_ext    , argument );
    CMP( "Skill directory"  , skill_dir   , argument );
    CMP( "Skill file"       , skill_file  , argument );
    CMP( "Skill Extension"  , skill_ext   , argument );
    CMP( "Social directory" , social_dir  , argument );
    CMP( "Social file"      , social_file , argument );
    CMP( "Social Extension" , social_ext  , argument );
    CMP( "Stock file"       , stock_file  , argument );
    CMP( "Symbol file"      , symbol_file , argument );
    CMP( "Suspect File"     , suspect_file, argument );
    CMP( "Suicide Log"      , suicide_log , argument );

    if ( !str_cmp( command, "Socket TimeOut" ) )
    {
      if ( ( SocketTimeOut = atoi( argument ) ) <= 30 || SocketTimeOut > 300 )
        mudlog( LOG_ERR, "網路連線逾時 %d 不合法.", SocketTimeOut );
      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Sacrifice Gold" ) )
    {
      if ( ( SacGold = atoi( argument ) ) <= 0 )
         mudlog( LOG_ERR, "你的奉獻金 %d 不合法.", SacGold );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "SetFileMode" ) )
    {
      SetFileMode =( atoi( argument ) == FALSE ) ? FALSE : TRUE;
      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Skill Value" ) )
    {
      SkillValue = ( atoi( argument ) == FALSE ) ? FALSE : TRUE;
      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Signal Segv" ) )
    {
      signal_segv =( atoi( argument ) == FALSE ) ? FALSE : TRUE;
      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Signal Bus" ) )
    {
      signal_bus = ( atoi( argument ) == FALSE ) ? FALSE : TRUE;
      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Signal Fpe" ) )
    {
      signal_fpe = ( atoi( argument ) == FALSE ) ? FALSE : TRUE;
      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Signal ILL" ) )
    {
      signal_ill = ( atoi( argument ) == FALSE ) ? FALSE : TRUE;
      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Strict Email" ) )
    {
      strict_email = ( atoi( argument ) == FALSE ) ? FALSE : TRUE;
      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Strict Password" ) )
    {
      strict_password = ( atoi( argument ) == FALSE ) ? FALSE : TRUE;
      fMatch          = TRUE;
      break;
    }

    if ( !str_cmp( command, "Stock Shock" ) )
    {
      StockShock = atoi( argument );
      fMatch     = TRUE;
      break;
    }

    if ( !str_cmp( command, "Stock Quota" ) )
    {
      StockQuota = atoi( argument );
      fMatch     = TRUE;
      break;
    }

    if ( !str_cmp( command, "Stock Release" ) )
    {
      StockRelease = atoi( argument );
      fMatch       = TRUE;
      break;
    }

    if ( !str_cmp( command, "Stock Bote" ) )
    {
      StockBote = atoi( argument );
      fMatch    = TRUE;
      break;
    }

    if ( !str_cmp( command, "Stock Tax" ) )
    {
      if ( ( StockTax = atoi( argument ) ) <= 0 || StockTax > 999 )
        mudlog( LOG_ERR, "你的股市證交稅 %d 不合法." , StockTax );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Stock Lost" ) )
    {
      if ( ( stock_lost = atoi( argument ) ) < 0
        || stock_lost >= MAX_STOCK_LOST / 2 )
         mudlog( LOG_ERR, "你的股市跌幅 %d 不合法." , stock_lost );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Stock Win" ) )
    {
      if ( ( stock_win = atoi( argument ) ) < 0
        || stock_win >= MAX_STOCK_WIN /2 )
         mudlog( LOG_ERR, "你的股市漲幅 %d 不合法." , stock_win );

      fMatch = TRUE;
      break;
    }

    break;

  case 'T':

    if ( !str_cmp( command, "Tick permit" ) )
    {
      tick_permit = ( atoi( argument ) == FALSE ) ? FALSE : TRUE;
      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Ticket Total" ) )
    {
      if ( ( TicketTotal = atoi( argument ) ) < 0 )
        mudlog( LOG_ERR, "你的彩券總數目 %d 不合理.", TicketTotal );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Ticket Reset" ) )
    {
      if ( ( TicketReset = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "你的彩券重置時間 %d 不合理.", TicketReset );

      ticketreset = TicketReset;
      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Ticket Set" ) )
    {
      ORDER_DATA * pOrder;
      ORDER_DATA * zOrder;
      int          order;
      int          howmany = 0;
      int          gold    = 0;

      argument = one_argument( argument, arg );
      if ( arg[0] == '\x0' || ( ( howmany = atoi( arg ) ) <= 0 ) )
        mudlog( LOG_ERR, "你的彩券設定錯誤(參數1)." );

      argument = one_argument( argument, arg );
      if ( arg[0] == '\x0' || ( ( gold = atoi( arg ) ) <= 0 ) )
        mudlog( LOG_ERR, "你的彩券設定錯誤(參數2)." );

      for ( order = 1, zOrder = order_list; zOrder; zOrder = zOrder->next )
        order++;

      pOrder          = alloc_struct( STRUCT_ORDER_DATA );
      pOrder->order   = order;
      pOrder->howmany = howmany;
      pOrder->gold    = gold;
      pOrder->next    = NULL;

      for ( zOrder = order_list; zOrder && zOrder->next; zOrder = zOrder->next )
        order++;

      if ( !zOrder ) order_list   = pOrder;
      else           zOrder->next = pOrder;

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Ticket Cost" ) )
    {
      if ( ( TicketCost = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "你的彩券價錢 %d 不合理.", TicketCost );

      fMatch = TRUE;
      break;
    }

    if ( !str_cmp( command, "Telnet Port" ) )
    {
      if ( ( TelnetPort = atoi( argument ) ) <= 0 || TelnetPort > 1024 )
        mudlog( LOG_ERR, "你的查詢埠 %d 不合法." , TelnetPort );

      fMatch = TRUE;
      break;
    }

    CMP( "Typo file" , typo_file , argument );
    break;

  case 'V':
    CMP( "Vote directory" , vote_dir , argument );
    CMP( "Vote Extension" , vote_ext , argument );

    if ( !str_cmp( command, "Vote Gold" ) )
    {
      if ( ( VoteGold = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "你的投票保證金 %d 不合理.", VoteGold );

      fMatch = TRUE;
      break;
    }

    break;

  case 'W':
    CMP( "Wizard file"     , wizard_file  , argument );
    CMP( "WIZFLAGS FILE"   , wizflags_log , argument );
    CMP( "Welcome file"    , welcome_file , argument );
    CMP( "Welcome immortal", welcome_imm  , argument );

    if ( !str_cmp( command, "Wanted Threshold" ) )
    {
      if ( ( WantedThreshold = atoi( argument ) ) <= 0 )
        mudlog( LOG_ERR, "你的懸賞下限 %d 不合理.", WantedThreshold );

      fMatch = TRUE;
      break;
    }

    break;

  case 'X':
    CMP( "Xname file" , xname_file , argument );
    CMP( "Xname log"  , xname_log  , argument );
    break;
  }

  if ( !fMatch )
    mudlog( LOG_ERR , "啟始檔案指令 %s 參數 %s 有問題.", command, argument );

  RETURN_NULL();
}

#undef CMP

FUNCTION( do_directory )
{
  int len;

  PUSH_FUNCTION( "do_directory" );

  len = str_len( home_dir );

  print_to_char( ch , "所有目錄名稱﹕\n\r"
    "主目錄路徑   %s\n\r"
    "求助目錄     %s\n\r"
    "社交目錄     %s\n\r"
    "守護神目錄   %s\n\r"
    "區域目錄     %s\n\r"
    "進站畫面目錄 %s\n\r"
    "投票目錄     %s\n\r"
    "笑話目錄     %s\n\r"
    "區域地形目錄 %s\n\r"
    "版面目錄     %s\n\r"
    "職業目錄     %s\n\r"
    "區域液體目錄 %s\n\r"
    "玩家檔案目錄 %s\n\r"
    "怪物程式目錄 %s\n\r"
    "技能目錄     %s\n\r"
    "指令目錄     %s\n\r"
    "\n\r所有檔案名稱:\n\r"
    "留言板列表   %s\n\r"
    "留言板索引檔 %s\n\r"
    "留言板清單檔 %s\n\r"
    "社交列表檔案 %s\n\r"
    "區域索引檔案 %s\n\r"
    "虛擬空檔案   %s\n\r"
    "玩家意見檔案 %s\n\r"
    "筆誤意見檔案 %s\n\r"
    "新玩家列表   %s\n\r"
    "符號常數檔案 %s\n\r"
    "複製裝備檔案 %s\n\r"
    "監視神族檔案 %s\n\r"
    "網路連線記錄 %s\n\r"
    "馬車資料檔案 %s\n\r"
    "捐贈資料檔案 %s\n\r"
    "禮物資料檔案 %s\n\r"
    "懸賞資料檔案 %s\n\r"
    "渡船資料檔案 %s\n\r"
    "升級資料檔案 %s\n\r"
    "清除資料檔案 %s\n\r"
    "解迷資料檔案 %s\n\r"
    "神族資料檔案 %s\n\r"
    "拍賣品資料   %s\n\r"
    "部位資料檔案 %s\n\r"
    "幫派資料檔案 %s\n\r"
    "網路連線資料 %s\n\r"
    "禁止連線資料 %s\n\r"
    "工作站資料   %s\n\r"
    "問題集資料   %s\n\r"
    "不雅字檔案   %s\n\r"
    "不雅字記錄   %s\n\r"
    "股票資料檔案 %s\n\r"
    "說話記錄檔   %s\n\r"
    "自殺記錄檔   %s\n\r"
    "FQDN查詢     %s\n\r"
    "錯誤記錄檔案 %s\n\r"
    "錯誤記錄檔案 %s\n\r"
    "英雄檔案     %s\n\r"
    "載入錯誤記錄 %s\n\r"
    "出口錯誤記錄 %s\n\r"
    "密碼錯誤記錄 %s\n\r"
    "資料庫記錄檔 %s\n\r"
    "致能錯誤記錄 %s\n\r"
    "技能列表檔案 %s\n\r"
    "指令列表檔案 %s\n\r"
    "進站畫面     %s\n\r"
    "神族進站畫面 %s\n\r"
    "神族代理檔案 %s\n\r"
    "今日消息檔案 %s\n\r"
    , home_dir
    , help_dir        + len
    , social_dir      + len
    , angel_dir       + len
    , area_dir        + len
    , board_dir       + len
    , greet_dir       + len
    , vote_dir        + len
    , joke_dir        + len
    , sector_dir      + len
    , class_dir       + len
    , liq_dir         + len
    , player_dir      + len
    , MOBProgs_dir    + len
    , skill_dir       + len
    , ins_dir         + len
    , board_lst
    , board_index
    , board_sheet
    , social_file
    , index_file
    , null_file
    , ideas_file      + len
    , typo_file       + len
    , newplayer_file  + len
    , symbol_file     + len
    , check_file      + len
    , wizard_file     + len
    , net_file        + len
    , bus_file        + len
    , donate_file     + len
    , gift_file       + len
    , bounty_file     + len
    , ship_file       + len
    , promotion_file  + len
    , purge_file      + len
    , quest_file      + len
    , immlist_file    + len
    , sale_file       + len
    , situs_file      + len
    , club_file       + len
    , internal_file   + len
    , site_file       + len
    , station_file    + len
    , question_file   + len
    , xname_file      + len
    , xname_log       + len
    , stock_file      + len
    , chat_log        + len
    , suicide_log     + len
    , address_file    + len
    , error_file      + len
    , bugs_file       + len
    , hero_file       + len
    , failload_file   + len
    , failexit_file   + len
    , failpass_file   + len
    , database_file   + len
    , failenable_file + len
    , skill_file
    , ins_file
    , welcome_file    + len
    , welcome_imm     + len
    , wizflags_log    + len
    , motd_file       + len );
  RETURN_NULL();
}

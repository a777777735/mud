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
*                                                                          *
***************************************************************************/

/* 請勿自行更改 */
#define MUD_VERSION             "89-6-20"
#define STRUCT_NOTE_DATA        20
#define STRUCT_BAN_DATA         21
#define STRUCT_EXTRA_DESCR_DATA 22
#define STRUCT_DESCRIPTOR_DATA  23
#define STRUCT_CMD_DATA         24
#define STRUCT_AREA_DATA        25
#define STRUCT_HELP_DATA        26
#define STRUCT_MOB_INDEX_DATA   27
#define STRUCT_OBJ_INDEX_DATA   28
#define STRUCT_AFFECT_DATA      29
#define STRUCT_RESET_DATA       30
#define STRUCT_ROOM_INDEX_DATA  31
#define STRUCT_EXIT_DATA        32
#define STRUCT_SHOP_DATA        33
#define STRUCT_CHAR_DATA        34
#define STRUCT_OBJ_DATA         35
#define STRUCT_MPROG_DATA       36
#define STRUCT_SYMBOL_DATA      37
#define STRUCT_PC_DATA          38
#define STRUCT_SKILL_DATA       39
#define STRUCT_ALIAS_DATA       40
#define STRUCT_SOCIAL_DATA      41
#define STRUCT_GAMBLE_DATA      42
#define STRUCT_TEACH_DATA       43
#define STRUCT_ENABLE_DATA      44
#define STRUCT_XNAME_DATA       45
#define STRUCT_MPROG_ACT_LIST   46
#define STRUCT_SECTOR_DATA      47
#define STRUCT_CLASS_DATA       48
#define STRUCT_LIMIT_DATA       49
#define STRUCT_ADDRESS_DATA     50
#define STRUCT_BUS_DATA         51
#define STRUCT_NET_DATA         52
#define STRUCT_CLUB_DATA        53
#define STRUCT_DAMAGE_DATA      54
#define STRUCT_PW_DATA          55
#define STRUCT_GR_DATA          56
#define STRUCT_CACHET_DATA      57
#define STRUCT_LIQ_DATA         58
#define STRUCT_RESTRICT_DATA    59
#define STRUCT_JOB_DATA         60
#define STRUCT_ENROLL_DATA      61
#define STRUCT_QUEST_DATA       62
#define STRUCT_ENEMY_DATA       63
#define STRUCT_QUEST_INFO       64
#define STRUCT_IMMLIST_DATA     65
#define STRUCT_FRIEND_DATA      66
#define STRUCT_SHIP_DATA        67
#define STRUCT_FAILCODE_DATA    68
#define STRUCT_BOARD_DATA       69
#define STRUCT_POST_DATA        70
#define STRUCT_HERO_DATA        71
#define STRUCT_EFFECT_DATA      72
#define STRUCT_MESSAGE_DATA     73
#define STRUCT_ANGEL_DATA       74
#define STRUCT_SALE_DATA        75
#define STRUCT_FIGHT_DATA       76
#define STRUCT_SERIAL_DATA      77
#define STRUCT_TOP_DATA         78
#define STRUCT_WANTED_DATA      79
#define STRUCT_VOTE_DATA        80
#define STRUCT_IP_DATA          81
#define STRUCT_JOKE_DATA        82
#define STRUCT_TRACE_DATA       83
#define STRUCT_VARIABLE_DATA    84
#define STRUCT_TICKET_DATA      85
#define STRUCT_ORDER_DATA       86
#define STRUCT_GIFT_DATA        87
#define STRUCT_STAMP_DATA       88
#define STRUCT_MINERAL_DATA     89
#define STRUCT_MINE_DATA        90
#define STRUCT_EVENT_DATA       91
#define STRUCT_BASIC_DATA       92
#define STRUCT_BOUNTY_DATA      93
#define STRUCT_SERVER_DATA      94
#define STRUCT_QUESTION_DATA    95
#define STRUCT_ANSWER_DATA      96
#define STRUCT_ENQUIRE_DATA     97
#define STRUCT_FILE_DATA        98
#define STRUCT_GREETING_DATA    99
#define STRUCT_DATABASE_DATA    100
#define STRUCT_SITUS_DATA       101

#define MAX_QUESTION             5
#define MAX_PARAMETER            5
#define MAX_OBJECT_VALUE        15
#define MAX_EFFECT_VALUE         3
#define MAX_NAME_LENGTH         30
#define MAX_HORSES               6
#define MAX_DEBUG               15
#define MAX_INTERNAL             5
#define MAX_VELLUM_LENGTH       128
#define MAX_STAFF               20

#define isSpace(c)              ( isspace( c ) && !IsChinese( c ) )
#define IsChinese(c)            IS_SET( c, 0x80 )

#define MAX_PORT                5
#define MAX_MONTH               100
#define MAX_DAY                 100
#define MAX_HOUR                100

#define NO_ERROR                0
#define FILE_ERROR              -1
#define FILE_CORRECT            1
#define FILE_NEW                0
#define LOAD_OBJECT             1
#define LOAD_DEPOSIT            2

#define FOGGY_SET               1
#define FOGGY_CLEAR             2

#define EDIT_NONE               0
#define EDIT_NOTE               1
#define EDIT_VOTE               2
#define EDIT_POST               3
#define EDIT_SELF_DESCRIPTION   4
#define EDIT_NOTEPAD            5
#define EDIT_HOME_DESCR         6

#define DATABASE_ALL            1
#define DATABASE_EXIST          2
#define DATABASE_NO_EXIST       3

#define ERROR_LETTER            0x00000001
#define ERROR_NUMBER            0x00000002
#define ERROR_STRING            0x00000004
#define ERROR_ALPHA             0x00000008
#define ERROR_WORD              0x00000010
#define ERROR_OTHER             0x00000020

/* 符號定義 */
#define ALIAS_SEPRATOR          ':'
#define SYMBOL_SEMICOLON        '&'
#define SYMBOL_COMMAND          '#'
#define VERTICAL_LINE           "--------------------------------------" \
                                "-------------------------------------\n\r"
/* 版面設定 */
#define MAX_BOARD               50
#define MAX_NOTES               100
#define POST_ALL                1
#define POST_NOT_MARK           2
#define POST_MARK               3
#define BLANK_LENGTH            17

/* 存檔模式 */
#define SAVE_FILE               1
#define BACKUP_FILE             2
#define FAILCODE_FILE           3
#define BASIC_FILE              4
#define LOG_FILE                5

/* Automap 的參數 */
#define MAX_PATH                30
#define MAX_TRACE               10
#define TRACE_NONE              -1

#define SHOP_SELL               1
#define SHOP_BUY                2

/* 位址旗標 */
#define BAN_LOGIN               1
#define BAN_FQDN                2
#define BAN_FINGER              3

/* 輸出緩衝區的控制碼 */
#define MODE_ABSENT             0
#define MODE_SAY                1
#define MODE_CHAT               2

/* 警告時間, 分鐘為單位 */
#define FIRST_WARN              5
#define SECOND_WARN             3
#define THIRD_WARN              1

/* 系統保留字的旗標 */
#define XNAMES_SYSTEM           1
#define XNAMES_CACONYM          2
#define XNAMES_CHAT             3
#define XNAMES_CNAME            4

#define VARIABLE_ILLEGAL        1
#define VARIABLE_SAME           2
#define VARIABLE_DIFFERENT      3
#define VARIABLE_NOT_DEFINE     4

#define MAX_RECHRISTEN          5    /* 每個玩家最多可以改名幾次     */
#define NAME_LENGTH             12   /* 玩家英文名稱的長度           */
#define CHAT_REPEAT             3    /* 記錄幾個對話的次數           */
#define CHAT_PENALTY            15   /* 講重複話的懲罰               */
#define IPC_BLOCK               100  /* 可以有幾塊記憶體區塊         */
#define IPC_IDLE                1000 /* 閒置多久系統時間則刪除此區塊 */
#define EQ_MAX_CAPCITY          80
#define EQ_MIN_CAPCITY          20
#define PILLS_PER_DAY           5
#define DOSAGE_LIMIT            60
#define DOSAGE_UPDATE           2
#define MAX_HITROLL             150  /* 最大命中率                   */

/* 股票的定義常數 */
#define MAX_STOCK_COST  1000000      /* 股票的最大面值         */
#define MIN_STOCK_COST  3000         /* 股票的最低面值         */
#define STOCK_SHOCK     100          /* 每買賣幾張漲跌幅會波動 */
#define STOCK_TAX       5            /* 證交稅                 */
#define MAX_STOCK_WIN   200          /* 股票的最大漲幅         */
#define MAX_STOCK_LOST  200          /* 股票的最大跌幅         */
#define MAX_ASSET       1000000000   /* 玩家最多可以有的總資產 */
#define MAX_STOCK_TRADE 100000000
#define STOCK_BOTE      200
#define STOCK_BUY       1
#define STOCK_BUY_TAX   2
#define STOCK_SELL      3
#define STOCK_SELL_TAX  4
#define STOCK_QUOTA     100
#define STOCK_RELEASE   10000

/* 遊戲本身的一些參數的宣告 */
#define MAX_SKILL                  270               /* 最多技能數量   */
#define MAX_RECALL                 10                /* 最多回歸位置   */
#define MAX_INNATE                 10
#define MAX_STOCK                  100               /* 最多股票公司   */
#define MAX_FIRMAN                 100               /* 最多免死金牌   */
#define FULL_FIRMAN                10                /* 足夠的免死金牌 */
#define MAX_LEVEL                  120               /* 遊戲最高等級   */
#define LEVEL_NODEATH              ( MAX_LEVEL - 1 ) /* 不死的等級     */
#define LEVEL_HERO                 ( MAX_LEVEL - 20 )
#define LEVEL_IMMORTAL             ( MAX_LEVEL - 19 )
#define LEV_IMM                    LEVEL_IMMORTAL

/* 設定 log 的層級 */
#define LOG_EMERG               1
#define LOG_CRIT                2
#define LOG_ERR                 3
#define LOG_DEBUG               4
#define LOG_STACK               5
#define LOG_INFO                6
#define LOG_WIZARD              7
#define LOG_NEWPLAYER           8
#define LOG_FAILPASS            9
#define LOG_FAILEXIT            10
#define LOG_FAILLOAD            11
#define LOG_SHUTDOWN            12
#define LOG_IDEA                13
#define LOG_TYPO                14
#define LOG_LOWLIFE             15
#define LOG_NET                 16
#define LOG_CHAT                17
#define LOG_SUICIDE             18
#define LOG_PURGE               19
#define LOG_FAILENABLE          20
#define LOG_WIZFLAGS            21
#define LOG_BADFILE             22
#define LOG_SUSPECT             23
#define LOG_BADOBJECT           24

/* notify 的旗標 */
#define NOTIFY_DEBUG            0x00000001
#define NOTIFY_INFO             0x00000002
#define NOTIFY_WIZARD           0x00000004
#define NOTIFY_NEWPLAYER        0x00000008
#define NOTIFY_FAILPASS         0x00000010
#define NOTIFY_EMERG            0x00000020
#define NOTIFY_CRIT             0x00000040
#define NOTIFY_ERR              0x00000080
#define NOTIFY_XNAME            0x00000100
#define NOTIFY_NET              0x00000200

#define INTERNAL_GETHOSTBYADDR  1
#define INTERNAL_FINGER         2
#define INTERNAL_ENDFINGER      3
#define INTERNAL_CHECKFILE      4
#define INTERNAL_CHECKSERV      5

/* 最大可以同時有幾個人連線 */
#define MAX_CONNECT             5
#define MAX_HOSTNAME            256
#define MAX_IP                  10

/* 定義網路連線常數 */
#define NET_NAME                1
#define NET_ADDRESS             2
#define NET_IMPORT              1
#define NET_EXPORT              2

#define ADDRESS_ALPHA           1
#define ADDRESS_NUMBER          2

/* 定義物品影響的型態 */
#define OBJ_CAST_HIT            1
#define OBJ_CAST_MANA           2
#define OBJ_CAST_MOVE           3
#define OBJ_CAST_IDENT          4
#define OBJ_CURE_POISON         5
#define OBJ_DETECT_INVS         6
#define OBJ_CAST_FLY            7
#define OBJ_CURE_BLIND          8
#define OBJ_GIANT_STR           9
#define OBJ_DETECT_HIDE         10
#define OBJ_SNEAK               11
#define OBJ_DETECT_EVIL         12
#define OBJ_CHANGE_SEX          13
#define OBJ_DETECT_MAGIC        14
#define OBJ_DETECT_POISON       15
#define OBJ_FAERIE_FOG          16
#define OBJ_GATE                17
#define OBJ_FIXITY              18
#define OBJ_CAST_ALL            19
#define OBJ_DETECT_MASK         20

/* 定義效應 */
#define EFFECT_NONE                     0
#define EFFECT_VICTIM_MANA              1
#define EFFECT_SELF_MANA                2
#define EFFECT_VICTIM_MOVE              3
#define EFFECT_SELF_MOVE                4
#define EFFECT_VICTIM_BLINDNESS         5
#define EFFECT_VICTIM_CURSE             6
#define EFFECT_VICTIM_POISON            7
#define EFFECT_VICTIM_SLEEP             8
#define EFFECT_VICTIM_PARA              9
#define EFFECT_VICTIM_FAERIE_FIRE       10
#define EFFECT_VICTIM_HIT               11
#define EFFECT_SELF_HIT                 12
#define EFFECT_HIT_DIE                  13

/* 限制的時機 */
#define OCCASION_NONE                   0

/* 定義敘述的形態 */
#define ACT_WHEN_EAT                    1
#define ACT_WHEN_WEAR                   2
#define ACT_WHEN_DROP                   3
#define ACT_WHEN_REMOVE                 4
#define ACT_WHEN_SACRIFICE              5
#define ACT_WHEN_GET                    6
#define ACT_WHEN_PUT                    7
#define ACT_WHEN_GIVE                   8
#define ACT_WHEN_FILL                   9
#define ACT_WHEN_DRINK                  10
#define ACT_WHEN_RECITE                 11
#define ACT_WHEN_BUY                    12
#define ACT_WHEN_SELL                   13
#define ACT_WHEN_VALUE                  14
#define ACT_WHEN_LOOK                   15
#define ACT_WHEN_COMPARE                16
#define ACT_WHEN_SEND                   17
#define ACT_WHEN_OPEN                   18
#define ACT_WHEN_CLOSE                  19
#define ACT_WHEN_LOCK                   20
#define ACT_WHEN_UNLOCK                 21
#define ACT_WHEN_AUCTION                22
#define ACT_WHEN_BET                    23
#define ACT_WHEN_REPAIR                 24
#define ACT_WHEN_DEPOSIT                25
#define ACT_WHEN_WITHDRAW               26
#define ACT_WHEN_CACHET                 27
#define ACT_WHEN_SLUP                   28
#define ACT_WHEN_STUDY                  29

/* 定義人物的基本屬性 */
#define DEFAULT_HIT             100
#define DEFAULT_MOVE            100
#define DEFAULT_MANA            100
#define DEFAULT_STR             1
#define DEFAULT_INT             1
#define DEFAULT_WIS             1
#define DEFAULT_DEX             1
#define DEFAULT_CON             1

/* 定義屬性名稱 */
#define TYPE_STR                1
#define TYPE_INT                2
#define TYPE_WIS                3
#define TYPE_DEX                4
#define TYPE_CON                5
#define TYPE_MOB                6
#define SPELL_CLASS_ONE         1
#define SPELL_CLASS_TWO         2
#define SPELL_CLASS_THR         3
#define SPELL_CLASS_FOU         4
#define SPELL_CLASS_FIV         5
#define SPELL_CLASS_MOB         6
#define SPELL_CLASS_MAG         7

/* 定義系統設定怪物各項數值加成 */
#define LOAD_ATTRIB_PLUS        1
#define LOAD_ARMOR_PLUS         1
#define LOAD_GOLD_PLUS         10
#define LOAD_DAMROLL_PLUS       5
#define LOAD_HIT_PLUS          50
#define LOAD_MANA_PLUS         50
#define LOAD_MOVE_PLUS         50

/* 定義神族最大負載數以及負載量 */
#define MAX_CARRY_N             1000
#define MAX_CARRY_W             1000000

/* 定義人物一開始的金錢數目 */
#define DEFAULT_GOLD            1000

/* 定義最大的極限值 */
#define MAX_LIMIT_VALUE         30000

/* 定義最大可封印數 */
#define MAX_CAN_CACHET            3

/* 定義玩家可以達到的最大基本屬性, 超過視為作弊 */
#define HUGE_ATTR                80

/* 定義食物壞掉的時間 */
#define FOOD_DEGRADE             30

/* 定義最大魔石等級 */
#define MAX_CACHET_LEVEL        100

/* 定義英雄增加的免死金牌 */
#define HERO_ADD_FIRMAN          10

/* 定義法術技能最大攻傷 */
#define LEVEL_ONE_DAM           350
#define LEVEL_TWO_DAM           700
#define LEVEL_THR_DAM          1300
#define LEVEL_FOU_DAM          1800
#define LEVEL_FIV_DAM          2300
#define MOB_ATTACK_DAM        20000
#define MAG_ATTACK_DAM         4000

/* 定義職業的常數 */
#define MAX_DEFAULT_CLASS       11
#define FORCE_LEVEL             15

/* 定義最小最大換頁行數 */
#define MAX_PAGELEN             300
#define MIN_PAGELEN             5

/* 定義最多幾項屬性 */
#define MAX_ATTR                5
#define STR_UPGRADE             2
#define STR_ITEM                0
#define INT_UPGRADE             2
#define INT_ITEM                1
#define WIS_UPGRADE             2
#define WIS_ITEM                2
#define DEX_UPGRADE             2
#define DEX_ITEM                3
#define CON_UPGRADE             2
#define CON_ITEM                4
#define MAX_ARMOR               200000
#define MAX_PRACTICE            20000
#define MAX_NSKILL              100

/* 定義門重置的狀態 */
#define DOOR_OPEN               0
#define DOOR_CLOSED             1
#define DOOR_LOCKED             2

/* 定義最大的生命力法力以及移動力 */
#define MAX_HIT                 20000
#define MAX_MANA                20000
#define MAX_MOVE                20000
#define MAX_STR                 100
#define MAX_INT                 100
#define MAX_WIS                 100
#define MAX_DEX                 100
#define MAX_CON                 100
#define MAX_DEFAULT_OBJECT      10

/* Command logging types. */
#define LOG_NORMAL              0
#define LOG_ALWAYS              1
#define LOG_NEVER               2
#define LOG_WIZ                 3

#define BOUNTY_GOLD        1
#define BOUNTY_FIRMAN      2

/* mpadd 的參數 */
#define LOC_STR            1
#define LOC_INT            2
#define LOC_WIS            3
#define LOC_DEX            4
#define LOC_CON            5
#define LOC_TRAIN          6
#define LOC_NSKILL         7
#define LOC_FIRMAN         8
#define LOC_LEVEL          9
#define LOC_PILLS          10
#define LOC_DOSAGE         11
#define LOC_RECHR          12
#define LOC_MAXHIT         13
#define LOC_MAXMANA        14
#define LOC_MAXMOVE        15
#define LOC_HIT            16
#define LOC_MANA           17
#define LOC_MOVE           18

/* God Levels */
#define L_GOD                   MAX_LEVEL
#define L_SUP                   L_GOD - 1
#define L_DEI                   L_SUP - 1
#define L_ANG                   L_DEI - 1
#define L_HER                   L_ANG - 1

/* 格式化字串的選項 */
#define FORMAT_CLEAN            1
#define FORMAT_SET              2
#define FORMAT_RETURN           3
#define FORMAT_STRING           4
#define FORMAT_NUM              5
#define FORMAT_LOCATE_STRING    6
#define FORMAT_LOCATE_NUM       7

/* 練習技能所耗的體力 */
#define PRACTICE_MOVE           5

/* 有關 PK */
#define BORN_AGE                17            /* 出生的年紀     */
#define PK_AGE                  21            /* 可以對戰的年紀 */
#define PK_LEVEL                50            /* 可以對戰的等級 */
#define PK_JOIN                 1             /* 對戰加入中     */
#define PK_BET                  2             /* 對戰下注中     */
#define PK_FIGHT                3             /* 對戰中         */
#define PK_STOP                 4             /* 對戰結束       */
#define PK_DESERT               5             /* 對戰閒置中     */
#define PK_CONTRAST_LEVEL       20            /* 對戰相差等級   */
#define MIN_PK_GOLD             100
#define MAX_PK_GOLD             100000
#define MAX_PK_TOTAL            10000000

#define PK_FIRST                0
#define PK_SECOND               1

#define PK_NODEFINE             0
#define PK_PERSON               1
#define PK_CLUB                 2

#define FUNCTION(c)     void (c)( CHAR_DATA * ch , char * argument )
#define JOB(c)          void (c)( CHAR_DATA * ch, char * argument, JOB_MESSAGE * job )
#define SKILL(c)        bool (c)( int sn, int level, CHAR_DATA * ch, void * vo )
#define ANGEL(c)        bool (c)( CHAR_DATA * ch, bool fPrint )

/* 以符合舊型的 ANSI 的編譯器 */
#if defined(TRADITIONAL)
#define const
#define args( list )                    ( )
#define DECLARE_DO_FUN( fun )           void fun( )
#define DECLARE_SPEC_FUN( fun )         bool fun( )
#define DECLARE_SPELL_FUN( fun )        bool fun( )
#define DECLARE_CHECK_FUN( fun )        bool fun( )
#define DECLARE_JOB_FUN( fun )          bool fun( )
#define DECLARE_ANGEL_FUN( fun )        bool fun( )
#define DECLARE_ROUTINE_FUN( fun )      bool fun( )
#define DECLARE_EVENT_FUN( fun )        void fun( )

#else
#define args( list )                    list
#define DECLARE_DO_FUN( fun )           DO_FUN      fun
#define DECLARE_SPEC_FUN( fun )         SPEC_FUN    fun
#define DECLARE_SPELL_FUN( fun )        SPELL_FUN   fun
#define DECLARE_CHECK_FUN( fun )        CHECK_FUN   fun
#define DECLARE_JOB_FUN( fun )          JOB_FUN     fun
#define DECLARE_ANGEL_FUN( fun )        ANGEL_FUN   fun
#define DECLARE_ROUTINE_FUN( fun )      ROUTINE_FUN fun
#define DECLARE_EVENT_FUN( fun )        EVENT_FUN   fun
#endif

#if     !defined( FALSE )
#define FALSE    0
#endif

#if     !defined( TRUE )
#define TRUE     1
#endif

#if     !defined( ERRORCODE )
#define ERRORCODE       -1
#endif

#if     defined(_AIX)
#if     !defined(const)
#define const
#endif
typedef int sh_int;
typedef int bool;
#define unix
#else
typedef short    int                    sh_int;
typedef unsigned char                   bool;
#endif

/* 定義幫派名稱查詢方式 */
#define CLUB_NAME               1               /* 用英文名稱查詢 */
#define CLUB_CNAME              2               /* 用中文名稱查詢 */
#define CLUB_MONEY              3
#define CLUB_STATUS             4
#define CLUB_TIMER              5
#define MAX_CLUB_MEMBER         100              /* 幫派最多幾個人 */
#define MAX_CLUB_DOYEN          10               /* 幫派長老數目   */
#define CLUB_STATUS_UNKNOW      0               /* 未知的幫派狀態 */
#define CLUB_STATUS_COUNTERSIGN 1               /* 幫派連署中     */
#define CLUB_STATUS_UNIONIZE    2               /* 幫派已經成立   */
#define MEMBER_VERIFY           TRUE            /* 需要認證幫眾   */
#define MEMBER_NO_VERIFY        FALSE           /* 不需要認證幫眾 */
#define CLUB_MASTER             6               /* 職位->幫主     */
#define CLUB_VICEMASTER         5               /* 職位->副幫主   */
#define CLUB_DOYEN              4               /* 職位->長老     */
#define CLUB_MEMBER             3               /* 職位->幫眾     */
#define CLUB_FOLLOWER           2               /* 職位->弟子     */
#define CLUB_ALL                1               /* 職位->不明     */
#define CLUB_NO_RELATION        -1              /* 沒有任何關係   */
#define BANK_PROFIT             100000          /* 幫庫的利息     */
#define PLAYER_HOLD_DAY         90              /* 玩家保留天數   */
#define PLAYER_FILE_QUOTA       200000          /* 玩家檔案大小   */

#define MAX_VOTES               10
#define MAX_POLL                100
#define VOTE_GOLD               1000000
#define VOTE_LEVEL              10
#define VOTE_DAYS               10
#define VOTE_MIN_DAYS           5

/* 結構的型態宣告 */
typedef struct  alias_data              ALIAS_DATA;
typedef struct  teach_data              TEACH_DATA;
typedef struct  cmd_data                CMD_DATA;
typedef struct  affect_data             AFFECT_DATA;
typedef struct  area_data               AREA_DATA;
typedef struct  ban_data                BAN_DATA;
typedef struct  xnames_data             XNAMES_DATA;
typedef struct  char_data               CHAR_DATA;
typedef struct  descriptor_data         DESCRIPTOR_DATA;
typedef struct  exit_data               EXIT_DATA;
typedef struct  extra_descr_data        EXTRA_DESCR_DATA;
typedef struct  help_data               HELP_DATA;
typedef struct  kill_data               KILL_DATA;
typedef struct  mob_index_data          MOB_INDEX_DATA;
typedef struct  note_data               NOTE_DATA;
typedef struct  obj_data                OBJ_DATA;
typedef struct  obj_index_data          OBJ_INDEX_DATA;
typedef struct  pc_data                 PC_DATA;
typedef struct  reset_data              RESET_DATA;
typedef struct  room_index_data         ROOM_INDEX_DATA;
typedef struct  shop_data               SHOP_DATA;
typedef struct  time_info_data          TIME_INFO_DATA;
typedef struct  weather_data            WEATHER_DATA;
typedef struct  symbol_data             SYMBOL_DATA;
typedef struct  mob_prog_data           MPROG_DATA;
typedef struct  mob_prog_act_list       MPROG_ACT_LIST;
typedef struct  auction_data            AUCTION_DATA;
typedef struct  sale_data               SALE_DATA;
typedef struct  fight_data              FIGHT_DATA;
typedef struct  serial_data             SERIAL_DATA;
typedef struct  top_data                TOP_DATA;
typedef struct  wanted_data             WANTED_DATA;
typedef struct  vote_data               VOTE_DATA;
typedef struct  gamble_data             GAMBLE_DATA;
typedef struct  sector_data             SECTOR_DATA;
typedef struct  skill_data              SKILL_DATA;
typedef struct  class_data              CLASS_DATA;
typedef struct  memory_data             MEMORY_DATA;
typedef struct  enable_data             ENABLE_DATA;
typedef struct  bus_data                BUS_DATA;
typedef struct  ship_data               SHIP_DATA;
typedef struct  joke_data               JOKE_DATA;
typedef struct  variable_data           VARIABLE_DATA;
typedef struct  ticket_data             TICKET_DATA;
typedef struct  order_data              ORDER_DATA;
typedef struct  gift_data               GIFT_DATA;
typedef struct  stamp_data              STAMP_DATA;
typedef struct  mineral_data            MINERAL_DATA;
typedef struct  mine_data               MINE_DATA;
typedef struct  greeting_data           GREETING_DATA;
typedef struct  array_data              ARRAY_DATA;
typedef struct  event_data              EVENT_DATA;
typedef struct  basic_data              BASIC_DATA;
typedef struct  bounty_data             BOUNTY_DATA;
typedef struct  server_data             SERVER_DATA;
typedef struct  cachet_data             CACHET_DATA;
typedef struct  trace_data              TRACE_DATA;
typedef struct  club_data               CLUB_DATA;
typedef struct  net_data                NET_DATA;
typedef struct  internal_socket         INTERNAL_SOCKET;
typedef struct  limit_data              LIMIT_DATA;
typedef struct  address_data            ADDRESS_DATA;
typedef struct  ip_data                 IP_DATA;
typedef struct  social_data             SOCIAL_DATA;
typedef struct  sockaddr_in             SOCKET_DATA;
typedef struct  timeval                 TIMEVAL;
typedef struct  ansi_data               ANSI_DATA;
typedef struct  palette_data            PALETTE_DATA;
typedef struct  damage_data             DAMAGE_DATA;
typedef struct  pw_data                 PW_DATA;
typedef struct  gr_data                 GR_DATA;
typedef struct  failcode_data           FAILCODE_DATA;
typedef struct  post_data               POST_DATA;
typedef struct  board_data              BOARD_DATA;
typedef struct  hero_data               HERO_DATA;
typedef struct  effect_data             EFFECT_DATA;
typedef struct  message_data            MESSAGE_DATA;
typedef struct  angel_data              ANGEL_DATA;
typedef struct  battle_data             BATTLE_DATA;
typedef struct  liq_data                LIQ_DATA;
typedef struct  restrict_data           RESTRICT_DATA;
typedef struct  job_data                JOB_DATA;
typedef struct  job_message             JOB_MESSAGE;
typedef struct  enroll_data             ENROLL_DATA;
typedef struct  stock_data              STOCK_DATA;
typedef struct  quest_data              QUEST_DATA;
typedef struct  question_data           QUESTION_DATA;
typedef struct  answer_data             ANSWER_DATA;
typedef struct  file_data               FILE_DATA;
typedef struct  enquire_data            ENQUIRE_DATA;
typedef struct  scan_data               SCAN_DATA;
typedef struct  quest_info              QUEST_INFO;
typedef struct  enemy_data              ENEMY_DATA;
typedef struct  immlist_data            IMMLIST_DATA;
typedef struct  friend_data             FRIEND_DATA;
typedef struct  database_data           DATABASE_DATA;
typedef struct  situs_data              SITUS_DATA;

/* 宣告一些函數的原型型態 */
typedef void DO_FUN      args( ( CHAR_DATA * , char * ) );
typedef bool SPEC_FUN    args( ( CHAR_DATA * ) );
typedef bool SPELL_FUN   args( ( int, int, CHAR_DATA *, void * ) );
typedef bool CHECK_FUN   args( ( CHAR_DATA *, bool ) );
typedef void JOB_FUN     args( ( CHAR_DATA *, char *, JOB_MESSAGE * ) );
typedef void EVENT_FUN   args( ( void ) );
typedef bool ANGEL_FUN   args( ( CHAR_DATA *, bool ) );
typedef bool ROUTINE_FUN args( ( CHAR_DATA *, CHAR_DATA *, int level ) );

/* 字串以及記憶體管理的參數宣告 */
#define MAX_KEY_HASH             1024
#define MAX_STRING_LENGTH        4096
#define MAX_BUFFER_LENGTH        4000
#define MAX_EDITING_LENGTH       4000
#define MAX_OUTPUT_LENGTH       30000
#define MAX_BLOCK_SIZE           4096
#define MAX_INPUT_LENGTH          160
#define MAX_FILE_LENGTH           128
#define MAX_PERM_BLOCK         131072
#define MAX_MEM_LIST               12
#define MAX_REPEAT                 20
#define MAX_HISTORY                 9
#define MAX_NOTEPAD                10
#define MAX_SCREEN_BUFFER       32768
#define MAX_STOCK_HISTORY          20

/* 有關記憶體管理參數宣告 */
extern  int     top_restrict;
extern  int     top_board;
extern  int     top_affect;
extern  int     top_area;
extern  int     top_ed;
extern  int     top_exit;
extern  int     top_help;
extern  int     top_vote;
extern  int     top_joke;
extern  int     top_angel;
extern  int     top_sector;
extern  int     top_class;
extern  int     top_liq;
extern  int     top_social;
extern  int     top_mob_index;
extern  int     top_obj_index;
extern  int     top_reset;
extern  int     top_room;
extern  int     top_shop;
extern  int     top_mineral;
extern  int     top_teach;
extern  int     top_greeting;
extern  int     top_skill;
extern  int     top_cmd;
extern  int     top_quest;
extern  int     top_question;
extern  int     top_event;
extern  int     top_gift;
extern  int     top_bounty;
extern  int     top_immlist;
extern  int     top_sale;
extern  int     top_situs;
extern  int     top_ship;
extern  FILE_DATA * Now_File;
extern  bool    ReadNumberError;
extern  int     RecallPlace[MAX_RECALL];
extern  int     iInnate[MAX_INNATE];
extern  int     nInnate;
extern  int     aInnate;
extern  int     merc_id;
extern  pid_t   merc_pid;
extern  bool    arena_busy;
extern  bool    fqdn_limit;
extern  bool    SystemCrash;
extern  bool    SetFileMode;
extern  bool    GroupSpiltExp;
extern  bool    AttackValue;
extern  bool    SkillValue;
extern  bool    PlayerAngel;
extern  bool    ChatRecord;
extern  int     MobDecrease;
extern  int     FileMode;
extern  int     PillsPerDay;
extern  int     DosageUpdate;
extern  int     DosageLimit;
extern  int     VoteGold;
extern  bool    NewLock;
extern  CLASS_DATA * iClass[];

/* 有關字串控制變數 */
extern  char      str_empty       [1];
extern  void *    rgFreeList      [MAX_MEM_LIST];
extern  int       rgAllocList     [MAX_MEM_LIST];
extern  int rgSizeList            [MAX_MEM_LIST];

#define PULSE_PER_SECOND          4
#define PULSE_PER_TALK            PULSE_PER_SECOND
#define PULSE_VIOLENCE            ( 3 * PULSE_PER_SECOND)
#define PULSE_MOBILE              ( 4 * PULSE_PER_SECOND)
#define PULSE_TICK                (30 * PULSE_PER_SECOND)
#define PULSE_AREA                (60 * PULSE_PER_SECOND)

struct   alias_data
{
  ALIAS_DATA *        next;
  char                text            [MAX_INPUT_LENGTH];
  char                macro           [MAX_INPUT_LENGTH];
};

struct process_time
{
  int stime;
  int ustime;
};

struct teach_data
{
  TEACH_DATA * next;
  int          slot;
  int          cost;
  int          adept;
  int          inventory;
};

struct auction_data
{
  int         price;
  int         times;
  OBJ_DATA  * obj;
  CHAR_DATA * seller;
  CHAR_DATA * buyer;
  SALE_DATA * sale;
  int         start_time;
  int         timer;
  bool        visible;
};

struct sale_data
{
  SALE_DATA      * next;
  OBJ_INDEX_DATA * obj;
  bool             visible;
  int              cost;
  int              times;
  int              sold;
  int              gold;
};

struct fight_data
{
  CHAR_DATA * fight_1;
  CHAR_DATA * fight_2;
  CLUB_DATA * club_1;
  CLUB_DATA * club_2;
  AREA_DATA * area;
  int         status;
  int         type;
  int         lock;
  int         time;
  int         open;
};

struct serial_data
{
  int high;
  int low;
};

struct top_data
{
  TOP_DATA            * next;
  struct process_time   time[60];
  int                   day;
  int                   hour;
  int                   number;
  int                   nRead;
  int                   nWrite;
  int                   loading;
};

struct wanted_data
{
  WANTED_DATA * next;
  char        * wanted;
  char        * bountier;
  int           reward;
};

/* 技能致能的結構宣告 */
struct enable_data
{
  ENABLE_DATA * next;
  SKILL_DATA  * skill;
};

struct net_data
{
  NET_DATA * next;
  char     * name;
  char     * cname;
  char     * address;
  int        port;
  int        import;
  int        export;
  int        nRead;
  int        nWrite;
  int        timer;
  int        import_duration;
  int        export_duration;
  bool       log;
  bool       valid;
  char       message[MAX_INPUT_LENGTH];
};

/* 幫派資料結構 */
struct  club_data
{
  CLUB_DATA       * next;
  ROOM_INDEX_DATA * location;
  OBJ_DATA        * staff[MAX_LEVEL][MAX_STAFF];
  char            * name;
  char            * cname;
  char            * master;
  char            * vicemaster;
  char            * doyen[MAX_CLUB_DOYEN];
  char            * member[MAX_CLUB_MEMBER];
  int               verify[MAX_CLUB_MEMBER];
  int               timer;
  int               status;
  int               money;
};

/* 地鐵資料 */
struct bus_data
{
  BUS_DATA        * next;
  ROOM_INDEX_DATA * station;
  ROOM_INDEX_DATA * platform;
  ROOM_INDEX_DATA * loge;
  char            * name;
  int               cost;
  int               count;
};

/* 船隻的資料 */
struct ship_data
{
  SHIP_DATA       * next;
  char            * name;               /* 船隻的中文名稱         */
  char            * msg_entrance;       /* 船隻入港的訊息         */
  char            * msg_land;           /* 船隻靠港的訊息         */
  char            * description;        /* 船艙的敘述             */
  ROOM_INDEX_DATA * starting;           /* 船隻的出發點           */
  ROOM_INDEX_DATA * cabin;              /* 船隻的船艙             */
  ROOM_INDEX_DATA * destination;        /* 船隻的目的地           */
  int               cost;               /* 上船的費用             */
  int               sailing;            /* 船隻的航行時間         */
  int               waiting;            /* 船隻等待上船的時間     */
  int               sailing_tick;       /* 船隻的目前航行時間     */
  int               waiting_tick;       /* 船隻目前等待上船的時間 */
  int               count;              /* 船隻的上船人數         */
  int               pirate;             /* 船隻遇到海盜的機率     */
  int               pirate_count;       /* 船隻遇到海盜的次數     */
  int               delay;              /* 船隻延期的機率         */
  bool              lock;               /* 船隻是否遭到鎖定       */
};

/* 賭博資料的宣告 */
struct  gamble_data
{
  GAMBLE_DATA * next;
  int           format;
  int           mount;
  int           horse[ MAX_HORSES ];
};

/* 地形結構的宣告 */
struct sector_data
{
  SECTOR_DATA * next;
  char        * cname;
  int           vnum;
  int           movement;
  int           count;
  bool          dark    : 1;
  bool          fly     : 1;
  bool          swim    : 1;
};

/* 常數符號的結構宣告 */
struct  symbol_data
{
  SYMBOL_DATA * next;
  SYMBOL_DATA * next_content;
  char        * str;
  char        * field;
  int           num;
};

struct  ban_data
{
  BAN_DATA * next;
  char     * name;
  int        type;
};

/* 不雅字的結構 */
struct xnames_data
{
  XNAMES_DATA * next;
  char        * word;
  int           type;
};

/* 問題集結構 */
struct question_data
{
  QUESTION_DATA * next;
  char          * title;
  char          * question[MAX_QUESTION];
  bool            answer[MAX_QUESTION];
  int             count;
  int             fail;
};

struct enquire_data
{
  ENQUIRE_DATA * next;
  char         * keyword;
};

struct answer_data
{
  QUESTION_DATA * question;
  int             random[MAX_QUESTION];
  int             timer;
  int             false;
};

struct file_data
{
  FILE_DATA * next;
  FILE      * fp;
  char      * filename;
  char      * address;
  char      * pointer;
  char      * boundary;
  int         error;
  int         fd;
  int         timer;
  int         size;
};

struct database_data
{
  DATABASE_DATA * next;
  DATABASE_DATA * hash;
  SERIAL_DATA     serial;
  char          * name;
  bool            exist;
};

struct situs_data
{
  SITUS_DATA * next;
  SITUS_DATA * associate;
  char       * name;
  char       * type;
  char       * eq_name;
  char       * situs_name;
  char       * short_descr;
  char       * msg1;
  char       * msg2;
  int          item_type;
  int          attack;
  int          location;
  int          chance;
  int          wear;
  int          associate_vnum;
  int          apply_ac;
  bool         wield  : 1;
};

/* 解謎的結構 */
struct quest_data
{
  QUEST_DATA * next;
  QUEST_INFO * link;
};

/* 解迷的資料 */
struct quest_info
{
  QUEST_INFO * next;
  char       * mark;
  char       * info;
  char       * help;
  bool         show;
  bool         title;
};

/* 神族資料 */
struct immlist_data
{
  IMMLIST_DATA * next;
  char         * name;
  char         * description;
  int            level;
  int            trust;
  bool           adviser : 1;
};

/* 好友名單結構 */
struct friend_data
{
  FRIEND_DATA * next;
  char          name[NAME_LENGTH+1];
};

/* 公敵的結構 */
struct enemy_data
{
  ENEMY_DATA * next;
  char       * city;
};

/* 時間以及天氣的參數宣告 */
#define SUN_DARK                0
#define SUN_RISE                1
#define SUN_LIGHT               2
#define SUN_SET                 3

#define SKY_CLOUDLESS           0
#define SKY_CLOUDY              1
#define SKY_RAINING             2
#define SKY_LIGHTNING           3

struct  time_info_data
{
  int hour;
  int day;
  int week;
  int month;
  int year;
};

struct  weather_data
{
  int mmhg;
  int change;
  int sky;
  int sunlight;
};

/* 連線目前的狀態宣告 */
#define CON_PLAYING                     0
#define CON_GET_FQDN                    1
#define CON_GET_FINGER                  2
#define CON_GET_NAME                    3
#define CON_CHECK_FILE                  4
#define CON_VERIFY                      5
#define CON_GET_OLD_PASSWORD            6
#define CON_CONFIRM_NEW_NAME            7
#define CON_GET_NEW_PASSWORD            8
#define CON_CONFIRM_NEW_PASSWORD        9
#define CON_GET_SEX                    10
#define CON_READ_MOTD                  11
#define CON_GET_CNAME                  12
#define CON_CHOOSE_SKILL               13
#define CON_GET_EMAIL_ADDRESS          14
#define CON_CHOOSE_CLASS               15
#define CON_CHOOSE_MODE                16
#define CON_CHOOSE_ANGEL               17
#define CON_CHOOSE_HOME                18
#define CON_CONTRACT                   19

/* 技能的分類 */
#define RATING_SKILL                    1
#define RATING_WIND                     2
#define RATING_EARTH                    3
#define RATING_LIGHTNING                4
#define RATING_POISON                   5
#define RATING_FIRE                     6
#define RATING_WATER                    7
#define RATING_DARKNESS                 8
#define RATING_LIGHT                    9
#define RATING_SAINT                   10
#define RATING_EVIL                    11
#define RATING_LOST                    12
#define RATING_CURE                    13
#define RATING_SING                    14
#define RATING_FIGHT                   15
#define RATING_MURDER                  16
#define RATING_CREATE                  17
#define RATING_THIEF                   18
#define RATING_ALL                     19 /* 所有系都包含 */

struct  descriptor_data
{
  DESCRIPTOR_DATA * next;
  bool              delete;
  int               counter;
  DESCRIPTOR_DATA * snoop_by;
  CHAR_DATA       * character;
  sh_int            descriptor;
  sh_int            connected;
  int               file;
  int               id_number;
  char              inbuf             [4 * MAX_INPUT_LENGTH];
  char              incomm                [MAX_INPUT_LENGTH];
  char              inlast  [MAX_HISTORY] [MAX_INPUT_LENGTH];
  char              tick_command          [MAX_INPUT_LENGTH];
  char              finger_data           [MAX_STRING_LENGTH];
  char              last_chat             [CHAT_REPEAT] [MAX_INPUT_LENGTH];
  char              username              [ NAME_LENGTH + 1 ];
  char            * showstr_head;
  char            * showstr_point;
  char            * outbuf;
  char            * buffer;
  char            * address;
  char            * host;
  char            * remote;
  char            * path;
  bool              select[MAX_INNATE];
  unsigned int      naddr;
  int               chat_error;
  int               inflect;
  int               outsize;
  int               outtop;
  int               alias_repeat;
  int               repeat;
  int               substitution;
  int               error;
  int               tick;
  int               tick_count;
  int               multi;
  int               port;
  int               nWrite;
  int               nRead;
  int               edit_mode;
  time_t            timekeeper;
  unsigned long int inet_addr;
  bool              fcommand     : 1;
  bool              check_fqdn   : 1;
  bool              check_finger : 1;
  bool              check_file   : 1;
  bool              lock_file    : 1;
  bool              fqdn_limit   : 1;
  bool              stack        : 1;
  bool              server       : 1;
};

struct  address_data
{
  ADDRESS_DATA * next;
  char         * number;
  char         * alpha;
  int            count;
};

struct ip_data
{
  IP_DATA * next;
  char    * address;
};

struct variable_data
{
  VARIABLE_DATA * next;
  char          * keyword;
  int             value;
  int             hours;
};

struct ticket_data
{
  TICKET_DATA * next;
  ORDER_DATA  * order;
  bool          sold;
};

struct order_data
{
  ORDER_DATA * next;
  int          order;
  int          howmany;
  int          gold;
};

struct gift_data
{
  GIFT_DATA      * next;
  OBJ_INDEX_DATA * gift;
  char           * title;
  char           * message;
  int              month;
  int              day;
  int              high;
  int              low;
  int              count;
  int              stamp;
  int              sender;
  int              duration;
  int              tick;
  int              gold;
  int              days;
  time_t           starting;
  time_t           ending;
  bool             send;
};

struct mineral_data
{
  MINERAL_DATA   * next;
  OBJ_INDEX_DATA * mineral;
  char           * message;
  int              count;
  int              flags;
};

struct mine_data
{
  MINE_DATA    * next;
  MINERAL_DATA * mineral;
};

struct greeting_data
{
  GREETING_DATA * next;
  char          * text;
};

struct array_data
{
  int    number;
  char * message;
};

struct stamp_data
{
  STAMP_DATA * next;
  int          number;
};

struct event_data
{
  EVENT_DATA * next;
  char       * title;
  char       * keyword;
  int          count;
  int          chance;
  bool         lock;
  EVENT_FUN  * function;
};

struct basic_data
{
  char        * name;
  char        * cname;
  char        * address;
  char        * email;
  char        * club;
  char        * class;
  SERIAL_DATA   serial;
  SERIAL_DATA   couple;
  int           sex;
  int           level;
  int           played;
};

struct cachet_data
{
  CACHET_DATA * next;
  int           vnum;
};

struct server_data
{
  SERVER_DATA * next;
  char        * address;
};

struct bounty_data
{
  BOUNTY_DATA    * next;
  MOB_INDEX_DATA * mob;
  char           * msg;
  int              room;
  int              chance;
  int              count;
  int              max;
  int              type;
  int              value;
  int              migration;
  int              occurred;
  int              killed;
  bool             lock;
};

struct joke_data
{
  JOKE_DATA * next;
  int         stamp;
  char      * title;
  char      * org;
  char      * text;
};

struct trace_data
{
  int starting;
  int ending;
  int path[MAX_PATH];
};

struct  limit_data
{
  LIMIT_DATA * next;
  CLASS_DATA * class;
  int          level;
  int          adept;
};

struct  struct_data
{
  int    type;
  int    size;
  char * name;
  int    nAlloc;
  int    ref;
};

/* act 函數所需要的參數 */
#define TO_ROOM                 0
#define TO_NOTVICT              1
#define TO_VICT                 2
#define TO_CHAR                 3
#define TO_ALL                  4

/* 求助結構的宣告 */
struct  help_data
{
  HELP_DATA * next;
  char      * keyword;
  char      * text;
  sh_int      level;
};

/* 有關商店的宣告 */
#define MAX_TRADE               5

struct  shop_data
{
  SHOP_DATA       * next;                   /* Next shop in list            */
  ROOM_INDEX_DATA * sential;
  char            * filename;
  int               type;
  int               keeper;                 /* Vnum of shop keeper mob      */
  int               buy_type [MAX_TRADE];   /* Item types shop will buy     */
  int               profit_buy;             /* Cost multiplier for buying   */
  int               profit_sell;            /* Cost multiplier for selling  */
  int               open_hour;              /* First opening hour           */
  int               close_hour;             /* First closing hour           */
};

/* 職業結構的定義 */
struct  class_data
{
  CLASS_DATA * next;
  char       * name;                 /* 英文名稱       */
  char       * cname;                /* 中文名稱       */
  char       * title;                /* 抬頭名稱       */
  char       * rank[MAX_LEVEL+1];    /* 抬頭           */
  char       * msg_limit;            /* 極限訊息       */
  int          vnum;                 /* 職業編號       */
  int          low_rebirth;          /* 轉職等級下限   */
  int          high_rebirth;         /* 轉職等級上限   */
  int          rebirth_gold;         /* 轉職金         */
  int          warn;                 /* 轉職警告       */
  int          associate;            /* 能不能轉職     */
  int          limit;                /* 極限值         */
  int          nskill;               /* 可學技能數目   */
  int          multiplier;           /* 極限倍數       */
  int          mask;                 /* 旗標           */
  int          attr[ MAX_ATTR ];     /* 所有屬性的上限 */
  int          factor[ MAX_ATTR ];   /* 所有屬性的因數 */
  int          max_default[MAX_ATTR];/* 屬性預設最大值 */
  int          min_default[MAX_ATTR];/* 屬性預設最小值 */
  int          hero[MAX_ATTR];       /* 升級為英雄加的數值 */
  bool         fMana     : 1;        /* 能不能得到法力 */
  bool         rudiment  : 1;        /* 是否為預設職業 */
  bool         force     : 1;        /* 是否為強制職業 */
  bool         innate    : 1;        /* 是否為可選職業 */
  bool         select    : 1;        /* 是否一開始可以選擇 */
};

/* NOTE 結構的宣告 */
struct note_data
{
  NOTE_DATA * next;
  char      * filename;
  char      * sender;
  char      * date;
  char      * to_list;
  char      * subject;
  char      * text;
  time_t      date_stamp;
  bool        mark;
};

struct vote_data
{
  VOTE_DATA * next;
  char      * poster;
  char      * subject;
  char      * text;
  char      * date;
  char      * club;
  int         days;
  int         stamp;
  int         level;
  char      * message[MAX_VOTES];
  int         poll[MAX_VOTES];
  char      * poller[MAX_POLL];
  int         vote[MAX_POLL];
  int         lock;
  int         moninal;
};

/* AFFECT 結構的宣告 */
struct  affect_data
{
  AFFECT_DATA * next;
  int           type;
  int           duration;
  int           location;
  int           modifier;
  int           level;
};

/* 被殺的結構宣告 */
struct  kill_data
{
  int number;
  int killed;
  int promotion;
  int exp;
  int damage;
  int dodge;
};

struct memory_data
{
  bool done;
  bool lock;
  int  timer;
  int  id;
  int  type;
  int  used;
  char text[MAX_STRING_LENGTH];
};

struct pw_data
{
  PW_DATA * next;
  char    * name;
  int       slot;
};

struct gr_data
{
  GR_DATA * next;
  char    * name;
  int       slot;
};

struct failcode_data
{
  FAILCODE_DATA * next;
  char          * address;
  int             timer;
};

struct board_data
{
  BOARD_DATA      * next;
  POST_DATA       * post;
  ROOM_INDEX_DATA * location;
  char            * name;
  char            * welcome;
  char            * listfile;
  char            * pathname;
  int               slot;
  int               capcity;
  int               read_level;
  int               write_level;
  bool              lock;
};

struct post_data
{
  POST_DATA  * next;
  BOARD_DATA * board;
  char       * title;
  char       * text;
  char       * poster;
  char       * owner;
  char       * filename;
  int          timer;
  int          attach;
  bool         mark : 1;
};

struct battle_data
{
  CHAR_DATA * fighter_1;
  CHAR_DATA * fighter_2;
  CLUB_DATA * club_1;
  CLUB_DATA * club_2;
  int         timer;
  bool        lock;
};

struct hero_data
{
  HERO_DATA * next;
  char      * name;
  char      * cname;
  char      * class;
  int         timer;
};

struct effect_data
{
  EFFECT_DATA * next;
  int           type;
  int           value[MAX_EFFECT_VALUE];
};

struct message_data
{
  MESSAGE_DATA * next;
  int            type;
  char         * self;
  char         * others;
};

struct angel_data
{
  ANGEL_DATA * next;
  ANGEL_FUN  * function;
  int          lower;
  int          higher;
  int          position;
  char       * description;
};

/***************************************************************************
 *                   有關區域檔的宣告部份                                  *
 ***************************************************************************/

/* MOB 的 ACT 的數值表 */
#define ACT_IS_NPC              0x00000001 /* 怪物的設定           */
#define ACT_SENTINEL            0x00000002 /* 停在原位﹐不會走動   */
#define ACT_SCAVENGER           0x00000004 /* 會自動撿起東西       */
#define ACT_AGGRESSIVE          0x00000020 /* 會自動攻擊           */
#define ACT_STAY_AREA           0x00000040 /* 不會離開區域         */
#define ACT_WIMPY               0x00000080 /* Flees when hurt      */
#define ACT_PET                 0x00000100 /* Auto set for pets    */
#define ACT_TRAIN               0x00000200 /* Can train PC's       */
#define ACT_PRACTICE            0x00000400 /* Can practice PC's    */
#define ACT_REBIRTH             0x00000800 /* 可以替玩家轉職       */
#define ACT_REBORN_FIGHT        0x00001000 /* 會再生成其他怪物     */
#define ACT_ASK                 0x00002000 /* 會叫人打工           */
#define AUTOSET_VALUE           0x00004000 /* 由系統設定一些數值   */
#define ACT_NOREBORN            0x00008000 /* 不會重生             */
#define ACT_NOKILL              0x00010000 /* 不能殺害             */
#define ACT_ENROLL              0x00020000 /* 記恨                 */
#define ACT_ALIGN_GOOD          0x00040000 /* 陣營善良的怪物       */
#define ACT_ALIGN_EVIL          0x00080000 /* 陣營邪惡的怪物       */
#define ACT_NOSUMMON            0x00100000 /* 不能被召喚           */

/* 性別 */
#define SEX_NEUTRAL             0
#define SEX_MALE                1
#define SEX_FEMALE              2
#define MAX_SEX                 3

/* 常用的物品的虛擬號碼 */
#define OBJ_VNUM_PROTYPE        1
#define OBJ_VNUM_MONEY_ONE      2
#define OBJ_VNUM_MONEY_SOME     3
#define OBJ_VNUM_CORPSE_NPC     4
#define OBJ_VNUM_CORPSE_PC      5
#define OBJ_VNUM_SEVERED_HEAD   6
#define OBJ_VNUM_TORN_HEART     7
#define OBJ_VNUM_SLICED_ARM     8
#define OBJ_VNUM_SLICED_LEG     9
#define OBJ_VNUM_FINAL_TURD     10
#define OBJ_VNUM_MUSHROOM       11
#define OBJ_VNUM_LIGHT_BALL     12
#define OBJ_VNUM_SPRING         13
#define OBJ_VNUM_DUMPLING       62
#define OBJ_VNUM_BOUGI          120
#define OBJ_VNUM_PON            121
#define OBJ_VNUM_CHICKEN        119
#define OBJ_VNUM_SCHOOL_MACE    50
#define OBJ_VNUM_SCHOOL_DAGGER  51
#define OBJ_VNUM_SCHOOL_SWORD   52
#define OBJ_VNUM_SCHOOL_ROBE    53
#define OBJ_VNUM_SCHOOL_HAT     54
#define OBJ_VNUM_MAGICSTONE     122
#define OBJ_VNUM_MEAT           169
#define OBJ_VNUM_LETTER         170
#define OBJ_VNUM_URN            171

/* 物品的型態常數 */
#define ITEM_LIGHT              1
#define ITEM_SCROLL             2
#define ITEM_WAND               3
#define ITEM_STAFF              4
#define ITEM_WEAPON             5
#define ITEM_TREASURE           6
#define ITEM_ARMOR              7
#define ITEM_POTION             8
#define ITEM_FURNITURE          9
#define ITEM_TRASH              10
#define ITEM_CONTAINER          11
#define ITEM_DRINK_CON          12
#define ITEM_KEY                13
#define ITEM_FOOD               14
#define ITEM_MONEY              15
#define ITEM_BOAT               16
#define ITEM_CORPSE_NPC         17
#define ITEM_CORPSE_PC          18
#define ITEM_FOUNTAIN           19
#define ITEM_PILL               20
#define ITEM_MAGICSTONE         21
#define ITEM_SPIRITJADE         22
#define ITEM_MYSTERY            23
#define ITEM_LETTER             24
#define ITEM_ORE                25
#define ITEM_GOLDMINE           26
#define ITEM_SIGIL              27
#define ITEM_VELLUM             28
#define ITEM_FIREWORK           29
#define ITEM_URN                30
#define ITEM_BACKBAG            31
#define ITEM_ARROW              32

/* 武器的形態 */
#define WEAPON_ALL              65535
#define WEAPON_HAND             1        /* 空手攻擊 */
#define WEAPON_DAGGER           2        /* 匕首攻擊 */
#define WEAPON_SWORD            3        /* 長劍攻擊 */
#define WEAPON_BLADE            4        /* 大刀攻擊 */
#define WEAPON_AXE              5        /* 斧頭攻擊 */
#define WEAPON_WHIP             6        /* 長鞭攻擊 */
#define WEAPON_SPEAR            7        /* 長槍攻擊 */
#define WEAPON_PEN              8        /* 筆扇攻擊 */
#define WEAPON_HAMMER           9        /* 鎚類攻擊 */
#define WEAPON_CLUB             10       /* 棍棒攻擊 */
#define WEAPON_BOW              11       /* 弓箭攻擊 */
#define WEAPON_FORCE            12       /* 氣功攻擊 */
#define WEAPON_STEPS            13       /* 步法技能 */
#define WEAPON_FLEE             14       /* 逃跑技能 */

#define SHOP_STORE              1
#define SHOP_SMITH              2
#define SHOP_MERCENARY          3
#define SHOP_COPER              4

/* 物品的額外旗標數值表 */
#define ITEM_GLOW               0x00000001
#define ITEM_HUM                0x00000002
#define ITEM_DARK               0x00000004
#define ITEM_LOCK               0x00000008
#define ITEM_EVIL               0x00000010
#define ITEM_INVIS              0x00000020
#define ITEM_MAGIC              0x00000040
#define ITEM_NODROP             0x00000080
#define ITEM_BLESS              0x00000100
#define ITEM_ANTI_GOOD          0x00000200
#define ITEM_ANTI_EVIL          0x00000400
#define ITEM_ANTI_NEUTRAL       0x00000800
#define ITEM_NOREMOVE           0x00001000
#define ITEM_INVENTORY          0x00002000
#define ITEM_CANCACHET          0x00004000
#define ITEM_USERSET            0x00008000
#define ITEM_NOSAVE             0x00010000
#define ITEM_CONTRABAND         0x00020000
#define ITEM_LOADABLE           0x00040000

/* 物品的穿戴旗標的數值表 */
#define ITEM_NOT_TAKE           0x00000000
#define ITEM_WEAR_LIGHT         131072          /* 6.26.2000 移除 */

/* 物品的影響型態的數值表 */
#define APPLY_NONE              0
#define APPLY_STR               1
#define APPLY_DEX               2
#define APPLY_INT               3
#define APPLY_WIS               4
#define APPLY_CON               5
#define APPLY_SEX               6
#define APPLY_CLASS             7
#define APPLY_LEVEL             8
#define APPLY_AGE               9
#define APPLY_HEIGHT            10
#define APPLY_WEIGHT            11
#define APPLY_MANA              12
#define APPLY_HIT               13
#define APPLY_MOVE              14
#define APPLY_GOLD              15
#define APPLY_EXP               16
#define APPLY_AC                17
#define APPLY_HITROLL           18
#define APPLY_DAMROLL           19
#define APPLY_SAVING_PARA       20
#define APPLY_SAVING_ROD        21
#define APPLY_SAVING_PETRI      22
#define APPLY_SAVING_BREATH     23
#define APPLY_SAVING_SPELL      24

/* 容器類物品中的 Value[1] 中的種類數值表 */
#define CONT_CLOSEABLE          0x00000001
#define CONT_PICKPROOF          0x00000002
#define CONT_CLOSED             0x00000004
#define CONT_LOCKED             0x00000008

/* 房間的方向數值表 */
#define DIR_NORTH               0
#define DIR_EAST                1
#define DIR_SOUTH               2
#define DIR_WEST                3
#define DIR_UP                  4
#define DIR_DOWN                5
#define DIR_ENTER               6
#define DIR_OUT                 7
#define DIR_MAX                 8

/* 物品裝備裝戴位置數值表, -1 代表沒有 */
#define WEAR_NONE               -1
#define WEAR_LIGHT              0
#define WEAR_WIELD              16
#define WEAR_HOLD               17

/* 技能攻擊部位 */
#define ATTACK_RANDOM           0x00000000

/* 人物狀態 */
#define COND_DRUNK              0
#define COND_FULL               1
#define COND_THIRST             2

/* 人物目前的情形 */
#define POS_DEAD                0
#define POS_SLEEPING            1
#define POS_RESTING             2
#define POS_FIGHTING            3
#define POS_STANDING            4

/* 代理神族功能 */
#define WIZ_SILENCE             0x00000001
#define WIZ_NOTELL              0x00000002
#define WIZ_NOEMOTE             0x00000004

#define MAX_TRIBUNAL            100                 /* 最大懲罰時間     */
#define MAX_TRIBUNAL_PC         5                   /* 神族權力最大懲罰 */

/* 人物的 ACT 的數值對照表 */
#define PLR_IS_NPC              0x00000001          /* 不能亂設定 */
#define PLR_BOUGHT_PET          0x00000002
#define PLR_AUCTION             0x00000004
#define PLR_AUTOEXIT            0x00000008
#define PLR_AUTOLOOT            0x00000010
#define PLR_AUTOSAC             0x00000020
#define PLR_BLANK               0x00000040
#define PLR_BRIEF               0x00000080
#define PLR_CHAT                0x00000100
#define PLR_COMBINE             0x00000200
#define PLR_PROMPT              0x00000400
#define PLR_TELNET_GA           0x00000800
#define PLR_HOLYLIGHT           0x00001000
#define PLR_WIZINVIS            0x00002000
#define PLR_SILENCE             0x00004000
#define PLR_NO_EMOTE            0x00008000
#define PLR_NO_SHOUT            0x00010000
#define PLR_NO_TELL             0x00020000
#define PLR_LOG                 0x00040000
#define PLR_DENY                0x00080000
#define PLR_FREEZE              0x00100000
#define PLR_THIEF               0x00200000
#define PLR_KILLER              0x00400000
#define PLR_BOLTER              0x00800000
#define PLR_EXACT               0x01000000
#define PLR_MESSAGE             0x02000000
#define PLR_FLEE                0x04000000
#define PLR_ANGEL               0x08000000
#define PLR_AUTOFOOD            0x10000000
#define PLR_AUTODRINK           0x20000000
#define PLR_NODEATH             0x40000000

#define PLR_COPYEQ              0x00000001

/* 定義給 turn 這個變數用的 */
#define PLR_REBIRTH             0x00000001
#define PLR_TRAIN               0x00000002
#define PLR_PRACTICE            0x00000004
#define PLR_ANSI                0x00000008
#define PLR_LOTTO               0x00000010

/* 頻道數值對照表 */
#define CHANNEL_AUCTION         0x00000001
#define CHANNEL_CHAT            0x00000002
#define CHANNEL_BULLETIN        0x00000004
#define CHANNEL_IMMTALK         0x00000008
#define CHANNEL_MUSIC           0x00000010
#define CHANNEL_QUESTION        0x00000020
#define CHANNEL_SHOUT           0x00000040
#define CHANNEL_YELL            0x00000080
#define CHANNEL_GAMBLE          0x00000100
#define CHANNEL_CLASS           0x00000200
#define CHANNEL_CLUB            0x00000400
#define CHANNEL_SEMOTE          0x00000800
#define CHANNEL_WEATHER         0x00001000
#define CHANNEL_PHONE           0x00002000
#define CHANNEL_SUICIDE         0x00004000
#define CHANNEL_RUMOR           0x00008000
#define CHANNEL_NOTICE          0x00010000
#define CHANNEL_GROUP           0x00020000
#define CHANNEL_PK              0x00040000

/* 獨立事件魔石 */
#define STONE_SPEED              1                   /* 加速度   */
#define STONE_STRENGTH           2                   /* 加力量   */
#define STONE_CON                3                   /* 加體格   */
#define STONE_WIS                4                   /* 加學識   */
#define STONE_LUCKY              5                   /* 幸運     */
#define STONE_PARRY              6                   /* 抵擋     */
#define STONE_BLOCK              7                   /* 武器抵擋 */
#define STONE_REVERSEHPMP        8                   /* 數值互換 */
#define STONE_REVERSEHPMOVE      9                   /* 數值互換 */
#define STONE_MAGIC             10                   /* 加魔力   */
#define STONE_HPUP              11                   /* 加生命力 */
#define STONE_MANAUP            12                   /* 加法力   */
#define STONE_MOVEUP            13                   /* 加體力   */
#define STONE_LIMITGAINUP       14                   /* 增加極限 */
#define STONE_REKICK            15                   /* 反擊     */
#define STONE_DEFEND            16                   /* 防禦     */
#define STONE_MONEYUP           17                   /* 金錢增加 */
#define STONE_EXPUP             18                   /* 經驗增加 */
#define STONE_UNDEATH           19                   /* 不老     */
#define STONE_STEAL             20                   /* 偷竊     */
#define STONE_KNOW              21                   /* 窺看對方 */
#define STONE_POWER             22                   /* 奮力一擊 */
#define STONE_CHANGE            23                   /* 物化對方 */
#define STONE_TWOMAGIC          24                   /* 耗損減半 */
#define STONE_TWOATTACK         25                   /* 耗損減半 */
#define STONE_TWOSUMMON         26                   /* 耗損減半 */
#define STONE_THROW             27                   /* 投擲     */
#define STONE_ARMOR             28                   /* 防禦上昇 */
#define STONE_STEALMP           29                   /* 偷竊法力 */
#define STONE_STEALHP           30                   /* 偷竊生命 */
#define STONE_STEALMOVE         31                   /* 偷竊體力 */
#define STONE_MPPOWERUP         32                   /* 威力上昇 */
#define STONE_MOVEPOWERUP       33                   /* 威力上昇 */
#define STONE_DEATH             34                   /* 自爆     */
#define STONE_WITH              35                   /* 配合威力 */
#define STONE_HITROLL           36                   /* 加強命中 */

/* 攻擊魔石 */
#define STONE_FIRE              37                   /* 火系魔法 */
#define STONE_LIGHTNING         38                   /* 雷系魔法 */
#define STONE_ICE               39                   /* 冰系魔法 */
#define STONE_EARTH             40                   /* 土系魔法 */
#define STONE_POISON            41                   /* 毒系魔法 */
#define STONE_LIGHT             42                   /* 光系魔法 */
#define STONE_DARKNESS          43                   /* 闇系魔法 */
#define STONE_SAINT             44                   /* 聖系魔法 */
#define STONE_EVIL              45                   /* 邪系魔法 */
#define STONE_VOID              46                   /* 虛無魔法 */
#define STONE_WIND              47                   /* 風系魔法 */
#define STONE_BEST              48                   /* 究極魔法 */
#define STONE_WATER             49                   /* 水系魔法 */
#define STONE_NOATTR            50                   /* 無屬性   */

/* 恢復或輔助魔石 */
#define STONE_HEAL              51                   /* 回復生命 */
#define STONE_RECOVER           52                   /* 回復體力 */
#define STONE_MEDITATION        53                   /* 回復法力 */
#define STONE_CURE_POISON       54                   /* 療毒魔法 */
#define STONE_PRAY              55                   /* 祈禱魔法 */
#define STONE_BESTHEAL          56                   /* 究極回復 */
#define STONE_CURE_POSITION     57                   /* 回復狀態 */
#define STONE_DISPALMAGIC       58                   /* 去除魔法 */
#define STONE_ARMORSPELL        59                   /* 防禦魔法 */
#define STONE_CALLDEATH         60                   /* 必殺魔法 */
#define STONE_BESTARMOR         61                   /* 究極防禦 */
#define STONE_REBIRTH           62                   /* 復活魔法 */
#define STONE_MINIMIZE          63                   /* 縮小魔法 */
#define STONE_ESCAPE            64                   /* 逃跑魔法 */
#define STONE_SILENCE           65                   /* 沈默魔法 */

/* 召喚魔石 */
#define STONE_SUMMONEAGLE       66                   /* 召喚老鷹 */
#define STONE_SUMMONLION        67                   /* 召喚獅子 */
#define STONE_SUMMONCOLD        68                   /* 召喚雪女 */
#define STONE_SUMMONFIRE        69                   /* 依弗里特 */
#define STONE_SUMMONLAM         70                   /* 召喚拉姆 */
#define STONE_SUMMONTAITAN      71                   /* 召喚泰坦 */
#define STONE_SUMMONOODIN       72                   /* 召喚歐丁 */
#define STONE_SUMMONSNAKE       73                   /* 召喚水蛇 */
#define STONE_SUMMONBAHAMUT     74                   /* 巴哈姆特 */
#define STONE_SUMMONCOW         75                   /* 召喚神牛 */
#define STONE_SUMMONSAINT       76                   /* 聖之審判 */
#define STONE_SUMMONFIREBIRD    77                   /* 轉生之炎 */
#define STONE_SUMMONBAHAMUT2    78                   /* 巴哈姆特II */
#define STONE_SUMMONEVILKING    79                   /* 召喚冥王 */
#define STONE_SUMMONTWOFACE     80                   /* 天地崩壞 */
#define STONE_SUMMONBAHAMUTZERO 81                   /* 巴哈姆特零式 */
#define STONE_SUMMONRIDER       82                   /* 圓桌武士 */
#define STONE_BESTSUMMON        83                   /* 究極召喚 */
#define STONE_SUMMONGOLEM       84                   /* 召喚石人 */

/* 武器技能裝備使用限制結構宣告 */
struct restrict_data
{
  RESTRICT_DATA * next;
  int             occasion;     /* 限制的時機     */
  int             type;         /* 限制型態       */
  int             value;        /* 限制數值       */
  int             vicevalue;    /* 限制的副數值   */
};

struct job_data
{
  JOB_DATA * next;
  char     * keyword;
  int        position;
  JOB_FUN  * function;
};

struct job_message
{
  ROOM_INDEX_DATA * room;
  CHAR_DATA       * mob;
  OBJ_DATA        * obj;
};

struct enroll_data
{
  ENROLL_DATA * next;
  char          name[NAME_LENGTH+1];
};

struct stock_data
{
  char * name;
  int    cost;
  int    sell;
  int    buy;
  int    today_sell;
  int    today_buy;
  int    win;
  int    lost;
  int    history[MAX_STOCK_HISTORY];
  bool   lock;
};

/* 限制種類 */
#define RES_STR                 1                    /* 對力量之限制   */
#define RES_INT                 2                    /* 對智力之限制   */
#define RES_WIS                 3                    /* 對學識之限制   */
#define RES_DEX                 4                    /* 對敏捷之限制   */
#define RES_CON                 5                    /* 對體格之限制   */
#define RES_HP                  6                    /* 對生命之限制   */
#define RES_MANA                7                    /* 對法力之限制   */
#define RES_MOVE                8                    /* 對體力之限制   */
#define RES_CLASS               9                    /* 對職業之限制   */
#define RES_TRUST              10                    /* 對信任值之限制 */
#define RES_SKILL              11                    /* 對技能之限制   */
#define RES_SEX                12                    /* 對性別之限制   */
#define RES_LEVEL              13                    /* 對等級之限制   */
#define RES_NOSKILL            14                    /* 相剋技能限制   */
#define RES_ALIGN              15                    /* 陣營的限制     */

/* MOB 原型結構表宣告 */
struct  mob_index_data
{
  MOB_INDEX_DATA * next;
  SPEC_FUN       * spec_fun;
  SHOP_DATA      * pShop;
  TEACH_DATA     * teach;
  ENABLE_DATA    * enable;
  JOB_DATA       * job;
  CLASS_DATA     * class;
  AREA_DATA      * area;
  AFFECT_DATA    * affected;
  char           * name;
  char           * short_descr;
  char           * long_descr;
  char           * description;
  char           * filename;
  char           * deadmsg;
  char           * auction;
  sh_int           count;
  sh_int           killed;
  sh_int           sex;
  sh_int           now_str;
  sh_int           now_int;
  sh_int           now_wis;
  sh_int           now_dex;
  sh_int           now_con;
  sh_int           level;
  sh_int           damroll;
  int              vnum;
  int              reborn_vnum;
  int              reborn_room;
  int              hit;
  int              mana;
  int              move;
  int              act;
  int              tractable;
  int              multipile;
  int              migrate;
  int              attack_ratio;
  int              dodge_ratio;
  sh_int           skill[MAX_SKILL];
  sh_int           alignment;
  sh_int           hitroll;                /* Unused */
  sh_int           ac;                     /* Unused */
  sh_int           hitnodice;              /* Unused */
  sh_int           hitsizedice;            /* Unused */
  sh_int           hitplus;                /* Unused */
  sh_int           damnodice;              /* Unused */
  sh_int           damsizedice;            /* Unused */
  sh_int           damplus;                /* Unused */
  int              gold;                   /* Unused */
  MPROG_DATA *     mobprogs;               /* Used by MOBprogram */
  char       *     temp;
  int              progtypes;              /* Used by MOBprogram */
  bool             speak        : 1;
  bool             Sentinel     : 1;
  bool             Scavenger    : 1;
  bool             Aggressive   : 1;
  bool             StayArea     : 1;
  bool             Wimpy        : 1;
  bool             Pet          : 1;
  bool             Train        : 1;
  bool             Practice     : 1;
  bool             Rebirth      : 1;
  bool             RebornFight  : 1;
  bool             Ask          : 1;
  bool             AutoSetValue : 1;
  bool             NoReborn     : 1;
  bool             NoKill       : 1;
  bool             Enroll       : 1;
  bool             AlignGood    : 1;
  bool             AlignEvil    : 1;
  bool             NoSummon     : 1;
};

/* 玩家或是虛擬怪物都有的結構 */
struct  char_data
{
  CHAR_DATA       * next;
  bool              delete;
  int               counter;
  CHAR_DATA       * next_in_room;
  CHAR_DATA       * master;
  CHAR_DATA       * leader;
  CHAR_DATA       * fighting;
  CHAR_DATA       * reply;
  CHAR_DATA       * mount;
  CHAR_DATA       * mount_by;
  CHAR_DATA       * spirit;
  CHAR_DATA       * boss;
  CHAR_DATA       * mercenary;
  CHAR_DATA       * hirer;
  SPEC_FUN        * spec_fun;
  MOB_INDEX_DATA  * pIndexData;
  DESCRIPTOR_DATA * desc;
  AFFECT_DATA     * affected;
  NOTE_DATA       * pnote;
  VOTE_DATA       * vote;
  OBJ_DATA        * carrying;
  OBJ_DATA        * deposit;
  ROOM_INDEX_DATA * in_room;
  ROOM_INDEX_DATA * was_in_room;
  ROOM_INDEX_DATA * social;
  PC_DATA         * pcdata;
  ALIAS_DATA      * alias;
  IP_DATA         * connect;
  ENABLE_DATA     * enable;
  GAMBLE_DATA     * gamble;
  CLUB_DATA       * club;
  CLASS_DATA      * class;
  ENROLL_DATA     * enroll;
  QUEST_DATA      * quest;
  ENEMY_DATA      * enemy;
  FRIEND_DATA     * friend;
  FRIEND_DATA     * badfriend;
  BOUNTY_DATA     * bounty;
  FAILCODE_DATA   * failcode;
  STAMP_DATA      * stamp;
  BASIC_DATA      * basic;
  ANSWER_DATA     * answer;
  SERIAL_DATA       serial;
  SERIAL_DATA       jade;
  char            * name;
  char            * cname;
  char            * byname;
  char            * long_descr;
  char            * description;
  char            * prompt;
  char            * email;
  char            * talk_buf;
  char            * editing;
  char            * stack;
  char              talk_mode;
  sh_int            now_str;
  sh_int            now_int;
  sh_int            now_wis;
  sh_int            now_dex;
  sh_int            now_con;
  sh_int            sex;
  sh_int            level;
  sh_int            trust;
  int               wizflags;
  int               tribunal_timer;
  int               played;
  int               home;
  int               mask;
  time_t            logon;
  time_t            save_time;
  time_t            last_note;
  time_t            last_joke;
  time_t            last_clubnote;
  sh_int            timer;
  sh_int            wait;
  sh_int            eq_capcity;
  int               fight_tick;
  int               hit;
  int               mod_hit;
  int               max_hit;
  int               mana;
  int               mod_mana;
  int               max_mana;
  int               move;
  int               mod_move;
  int               max_move;
  int               limit;
  int               gold;
  int               bank;
  int               exp;
  int               act;
  int               practice;
  int               notify;
  int               dodge;
  int               tractable;
  int               situs;
  int               firman;
  int               jail;
  int               failed;
  int               spiritjade;
  int               migrate;
  sh_int            skill[MAX_SKILL];
  int               ante[2];
  int               nskill;
  int               donate;
  sh_int            position;
  sh_int            carry_weight;
  sh_int            carry_number;
  sh_int            saving_throw;
  sh_int            turn;
  int               alignment;
  int               hitroll;
  int               damroll;
  int               armor;
  int               wimpy;
  int               deaf;
  MPROG_ACT_LIST  * mpact;                  /* Used by MOBprogram */
  int               mpactnum;               /* Used by MOBprogram */
  bool              speak        : 1;
  bool              cease        : 1;
  bool              trade        : 1;
  bool              disable_prog : 1;
  bool              release      : 1;
  bool              clublog      : 1;
  bool              inprocess    : 1;
  bool              adviser      : 1;
};

/* 玩家專有的資料結構 */
struct  pc_data
{
  PC_DATA      * next;
  TRACE_DATA   * trace;
  TRACE_DATA   * map[MAX_TRACE];
  OBJ_DATA     * corpse;
  CHAR_DATA    * mate;
  CHAR_DATA    * wooer;
  SERIAL_DATA    couple;
  char         * pwd;
  char         * bamfin;
  char         * bamfout;
  char         * title;
  char         * lasthost;
  char         * mater;
  char         * notepad[MAX_NOTEPAD];
  int            pkwin;
  int            pklost;
  int            fightwin;
  int            fightlost;
  int            kills;
  int            kill_level;
  int            ply;
  int            imprison;
  int            file_size;
  int            autosave;
  int            autosave_tick;
  int            autobackup;
  int            autobackup_tick;
  int            dormancy;
  int            rechristen;
  int            pills;
  int            dosage;
  int            steal;
  int            steal_panity;
  int            murder;
  int            fight_win;
  int            fight_lost;
  int            xname;
  int            angel_set;
  int            angel_left;
  int            stock[MAX_STOCK];
  int            asset[MAX_STOCK];
  int            recall[MAX_RECALL];
  int            board[MAX_BOARD];
  sh_int         deny;
  sh_int         suspect;
  sh_int         mod_str;
  sh_int         mod_int;
  sh_int         mod_wis;
  sh_int         mod_dex;
  sh_int         mod_con;
  sh_int         condition[3];
  sh_int         pagelen;
  bool           contract;
};

/* 怪物程式的結構 */
struct  mob_prog_act_list
{
  MPROG_ACT_LIST * next;
  char           * buf;
  CHAR_DATA      * ch;
  OBJ_DATA       * obj;
  void           * vo;
};

struct  mob_prog_data
{
  MPROG_DATA * next;
  char       * arglist;
  char       * comlist;
  int          type;
};

bool    MOBtrigger;

#define ERROR_PROG              -1
#define IN_FILE_PROG            0
#define ACT_PROG                0x00000001
#define SPEECH_PROG             0x00000002
#define RAND_PROG               0x00000004
#define FIGHT_PROG              0x00000008
#define DEATH_PROG              0x00000010
#define HITPRCNT_PROG           0x00000020
#define ENTRY_PROG              0x00000040
#define GREET_PROG              0x00000080
#define ALL_GREET_PROG          0x00000100
#define GIVE_PROG               0x00000200
#define BRIBE_PROG              0x00000400

/* 液體 */
struct liq_data
{
  LIQ_DATA  * next;
  char      * name;
  char      * color;
  int         slot;
  int         drunk;
  int         full;
  int         thirst;
  bool        water;
};

struct body_type
{
  char * eq_name;
  char * situs_name;
  int    location;
  int    attack;
  int    chance;
};

/* 房間或是物品的額外描述 */
struct  extra_descr_data
{
  EXTRA_DESCR_DATA * next;
  char             * keyword;
  char             * description;
};

/* 物品的原型結構 */
struct  obj_index_data
{
  OBJ_INDEX_DATA   * next;
  EXTRA_DESCR_DATA * extra_descr;
  AFFECT_DATA      * affected;
  AFFECT_DATA      * impact;
  JOB_DATA         * job;
  RESTRICT_DATA    * restrict;
  MESSAGE_DATA     * message;
  char             * name;
  char             * short_descr;
  char             * description;
  char             * filename;
  char             * wearmsg;
  char             * remmsg;
  char             * unit;
  sh_int             vnum;
  sh_int             item_type;
  sh_int             count;
  sh_int             weight;
  sh_int             level;
  int                wear_flags;
  int                exp;
  int                cost;
  int                armor;
  int                value[ MAX_OBJECT_VALUE ];
  int                auction_times;
  int                auction_gold;
  bool               disappear    : 1;
  bool               Glow         : 1;
  bool               Hum          : 1;
  bool               Dark         : 1;
  bool               Lock         : 1;
  bool               Evil         : 1;
  bool               Invis        : 1;
  bool               Magic        : 1;
  bool               NoDrop       : 1;
  bool               Bless        : 1;
  bool               AntiGood     : 1;
  bool               AntiEvil     : 1;
  bool               AntiNeutral  : 1;
  bool               NoRemove     : 1;
  bool               Inventory    : 1;
  bool               Cancachet    : 1;
  bool               UserSet      : 1;
  bool               NoSave       : 1;
  bool               Contraband   : 1;
  bool               Loadable     : 1;
  bool               Takeable     : 1;
  bool               Sale         : 1;
};

/* 真正物品的結構 */
struct  obj_data
{
  OBJ_DATA         * next;
  bool               delete;
  int                counter;
  OBJ_DATA         * next_content;
  OBJ_DATA         * contains;
  OBJ_DATA         * in_obj;
  CHAR_DATA        * carried_by;
  CHAR_DATA        * deposit_by;
  CHAR_DATA        * corpse_owner;
  CHAR_DATA        * address;
  EXTRA_DESCR_DATA * extra_descr;
  AFFECT_DATA      * affected;
  OBJ_INDEX_DATA   * pIndexData;
  ROOM_INDEX_DATA  * in_room;
  CLUB_DATA        * club;
  SERIAL_DATA        serial;
  SERIAL_DATA        owner;
  CACHET_DATA      * cachet;
  char             * name;
  char             * cname;
  char             * description;
  char             * unit;
  sh_int             item_type;
  sh_int             weight;
  sh_int             level;
  sh_int             timer;
  int                wear_loc;
  int                wear_flags;
  int                cost;
  int                max_armor;
  int                armor;
  int                venom;
  int                value   [ MAX_OBJECT_VALUE ];
  bool               disappear    : 1;
  bool               Glow         : 1;
  bool               Hum          : 1;
  bool               Dark         : 1;
  bool               Lock         : 1;
  bool               Evil         : 1;
  bool               Invis        : 1;
  bool               Magic        : 1;
  bool               NoDrop       : 1;
  bool               Bless        : 1;
  bool               AntiGood     : 1;
  bool               AntiEvil     : 1;
  bool               AntiNeutral  : 1;
  bool               NoRemove     : 1;
  bool               Inventory    : 1;
  bool               Cancachet    : 1;
  bool               UserSet      : 1;
  bool               NoSave       : 1;
  bool               Contraband   : 1;
  bool               Loadable     : 1;
  bool               Takeable     : 1;
};

/* 出口資料結構 */
struct  exit_data
{
  ROOM_INDEX_DATA * to_room;
  int               vnum;
  int               key;
  int               direction;
  char            * keyword;
  char            * description;
  char            * message;
  bool              foggy  : 1;
  bool              isdoor : 1;
  bool              closed : 1;
  bool              locked : 1;
  bool              pick   : 1;
};

/*
 * Reset commands:
 *   '*': comment
 *   'M': read a mobile
 *   'O': read an object
 *   'P': put object in object
 *   'G': give object to mobile
 *   'E': equip object to mobile
 *   'D': set state of door
 *   'R': randomize room exits
 *   'S': stop (end of list)
 */

/* 區域 reset 命令的定義 */
struct  reset_data
{
  RESET_DATA * next;
  char         command;
  char       * filename;
  int          arg1;
  int          arg2;
  int          arg3;
};

/* 區域定義 */
struct  area_data
{
  AREA_DATA       * next;
  RESET_DATA      * reset_first;
  RESET_DATA      * reset_last;
  ROOM_INDEX_DATA * list;
  ROOM_INDEX_DATA * capital;
  MINERAL_DATA    * mineral;
  char            * name;
  char            * filename;
  char            * editor;
  char            * description;
  int               serial;
  int               capital_no;
  int               upper;
  int               lower;
  int               age;
  int               nplayer;
  int               attribute;
  int               room;
  int               mob;
  int               obj;
  int               shop;
  int               reset;
  int               mine;
  int               foggy;
  bool              newhand : 1;
  bool              pk      : 1;
  bool              fight   : 1;
};

/* 房間原型資料結構 */
struct  room_index_data
{
  ROOM_INDEX_DATA  * next;
  ROOM_INDEX_DATA  * next_in_area;
  CHAR_DATA        * people;
  OBJ_DATA         * contents;
  EXTRA_DESCR_DATA * extra_descr;
  AREA_DATA        * area;
  EXIT_DATA        * exit[8];
  JOB_DATA         * job;
  CLUB_DATA        * club;
  BOARD_DATA       * board;
  MINE_DATA        * mine;
  SECTOR_DATA      * sector;
  ENQUIRE_DATA     * enquire;
  SERIAL_DATA        owner;
  char             * name;
  char             * description;
  char             * filename;
  int                vnum;
  int                gold;
  int                cost;
  int                light;
  int                mob_reborn;
  int                nplayer;
  int                nobject;
  bool               Upgrade      : 1;
  bool               NoLink       : 1;
  bool               player       : 1;
  bool               writeable    : 1;
  bool               Dark         : 1;
  bool               NoMob        : 1;
  bool               Indoors      : 1;
  bool               Private      : 1;
  bool               Safe         : 1;
  bool               ForeverLight : 1;
  bool               NoRecall     : 1;
  bool               DepositMoney : 1;
  bool               StoreRoom    : 1;
  bool               NoFight      : 1;
  bool               NoQuit       : 1;
  bool               Club         : 1;
  bool               Memorize     : 1;
  bool               Stock        : 1;
  bool               NoWhere      : 1;
  bool               Sail         : 1;
  bool               Fane         : 1;
  bool               Killer       : 1;
  bool               Standard     : 1;
  bool               Waterful     : 1;
  bool               Foodful      : 1;
  bool               ManaCure     : 1;
  bool               MoveCure     : 1;
  bool               HitCure      : 1;
  bool               PracRoom     : 1;
};

/* ansi code 的資料結構 */
struct ansi_data
{
  char * code;
};

/* 調色盤的資料結構 */
struct palette_data
{
  char character;
  int  color;
};

/* 技能傷害資料結構 */
struct damage_data
{
  DAMAGE_DATA    * next;
  OBJ_INDEX_DATA * innate;
  ROUTINE_FUN    * routine;
  EFFECT_DATA    * effect;
  char           * description;
  int              chance;
  int              value;
  int              vicevalue;
  int              situs;
  int              parry;
  int              multiple;
  int              obj_vnum;
};

#define MAX_SCAN_COUNT          200
#define MAX_SCAN_DEPTH          20

struct scan_data
{
  int vnum;
  int depth;
  int dir[MAX_SCAN_DEPTH];
};

/*
 * Types of attacks.
 * Must be non-overlapping with spell/skill types,
 * but may be arbitrary beyond that.
 */
#define TYPE_UNDEFINED          -1
#define TYPE_HIT                1000
#define TYPE_HIT_COST           3

/* 法術施法的形態 */
#define TAR_IGNORE              0
#define TAR_CHAR_OFFENSIVE      1
#define TAR_CHAR_DEFENSIVE      2
#define TAR_CHAR_SELF           3
#define TAR_OBJ_INV             4
#define TAR_DODGE               5
#define TAR_STRING              6
#define TAR_FLEE                7
#define TAR_ARGUMENT            8
#define TAR_MEDITATION          9
#define TAR_OBJ_ROOM            10
#define TAR_MOUNT               11
#define TAR_NO_CAST             12

/* 法術消耗的形態 */
#define COST_HIT                0
#define COST_MANA               1
#define COST_MOVE               2
#define COST_GOLD               3

/* 技能資料結構 */
struct  skill_data
{
  SKILL_DATA    * next;
  SPELL_FUN     * function;           /* 法術施法的函數         */
  CHECK_FUN     * check;              /* 施法確認函數           */
  LIMIT_DATA    * limit;              /* 各職業的限制           */
  DAMAGE_DATA   * damage;             /* 傷害訊息               */
  RESTRICT_DATA * restrict;           /* 限制資料               */
  AFFECT_DATA   * affected;           /* 影響                   */
  char          * name;               /* 英文名稱               */
  char          * cname;              /* 中文名稱               */
  char          * msg_off;            /* 法術消失的訊息         */
  char          * help;               /* 求助資訊               */
  char          * message;            /* 攻擊訊息               */
  int             slot;               /* 法術的編號             */
  int             position;           /* 法術所需最低狀態       */
  int             type;               /* 法術的類別             */
  int             cost_type;          /* 施法所耗的型態         */
  int             cost;               /* 施法所耗的點數         */
  int             wait;               /* 施法後的延遲時間       */
  int             exp;                /* 施法後獲得經驗值的機率 */
  int             associate;          /* 關連的法術編號         */
  int             degree;             /* 關連的程度, 千分之一   */
  int             weapon;             /* 武器                   */
  int             sex;                /* 性別                   */
  int             chance;             /* 機率的總和             */
  int             affect;             /* 法術效應號碼           */
  int             affect_id;          /* 施法效應編號, 物品對照 */
  int             rating;             /* 識別號碼               */
  int             antirating;         /* 相剋技能               */
  int             choosen;            /* 選取的人數             */
  int             adeptation;         /* 選取後預設熟練度       */
  int             ply;                /* 練習的機率             */
  int             qutoient;           /* 練習等級的商數         */
  bool            teach         : 1;  /* 是否可以互相教導       */
  bool            enable        : 1;  /* 是否可以被致能         */
  bool            say_spell     : 1;  /* 是否要唸出咒語         */
  bool            concentration : 1;  /* 是否會意志不集中       */
  bool            cast          : 1;  /* 是否可以用 cast 施展   */
  bool            innate        : 1;  /* 是否為預設技能         */
  bool            canask        : 1;  /* 是否可以查詢           */
  bool            valid         : 1;  /* 是否開放               */
};

/* 一些常用的巨集 */
#define UMIN(a, b)              ((int) (a) < (int) (b) ? (a) : (b))
#define UMAX(a, b)              ((int) (a) > (int) (b) ? (a) : (b))
#define URANGE(a, b, c)         ((b) < (a) ? (a) : ((b) > (c) ? (c) : (b)))
#define LOWER(c)                ((c) >= 'A' && (c) <= 'Z' ? (c)+'a'-'A' : (c))
#define UPPER(c)                ((c) >= 'a' && (c) <= 'z' ? (c)+'A'-'a' : (c))
#define IS_SET(flag, bit)       ((flag) & (bit))
#define IS_ERROR( num )         ( ( num ) == ERRORCODE )
#define SET_BIT(var, bit)       ((var) |= (bit))
#define REMOVE_BIT(var, bit)    ((var) &= ~(bit))
#define SWAP( x , y )           { ( (x) = ( (x) - (y) ) ); \
                                  ( (y) = ( (x) + (y) ) ); \
                                  ( (x) = ( (y) - (x) ) ); }
#define SQUARE(x)               ( (x) * (x) )
#define NOSPACE( c )            while ( *(c) == ' ' || *(c) == '\t' ) (c)++
#define YESNO( comp )           ( ( comp ) ? "○" : "×" )

/* 有關人物巨集 */
#define IS_NPC(ch)              (IS_SET((ch)->act, ACT_IS_NPC))
#define IS_IMMORTAL(ch)         (get_trust(ch) >= LEVEL_IMMORTAL)
#define IS_GOD(ch)              ( (ch)->level >= MAX_LEVEL )
#define IS_HERO(ch)             (get_trust(ch) >= LEVEL_HERO)
#define IS_MALE(ch)             (ch->sex == SEX_MALE )
#define IS_FEMALE(ch)           (ch->sex == SEX_FEMALE )

#define IS_GOOD(ch)             (ch->alignment >= 350)
#define IS_EVIL(ch)             (ch->alignment <= -350)
#define IS_NEUTRAL(ch)          (!IS_GOOD(ch) && !IS_EVIL(ch))

#define IS_AWAKE(ch)            (ch->position > POS_SLEEPING)
#define IS_REST(ch)             ( (ch)->position == POS_SLEEPING ||         \
                                  (ch)->position == POS_RESTING )
#define IS_OUTSIDE(ch)          ( (ch)->in_room->Indoors == FALSE )
#define WAIT_STATE(ch, npulse)  ((ch)->wait = UMAX((ch)->wait, (npulse)))

/* 物品的巨集 */
#define CAN_WEAR(obj, part)     (IS_SET((obj)->wear_flags,  (part)))

struct cmd_data
{
  CMD_DATA * next;
  char     * name;
  char     * cname;
  char     * help;
  char     * filename;
  DO_FUN   * function;
  int        position;
  int        level;
  int        log;
  int        exec;
  bool       canlock  : 1;
  bool       lock     : 1;
  bool       exact    : 1;
  bool       argument : 1;
  bool       mobonly  : 1;
  bool       chat     : 1;
  bool       wizlog   : 1;
  bool       jail     : 1;
  bool       dead     : 1;
  bool       order    : 1;
  bool       lost     : 1;
  bool       limit    : 1;
  bool       trace    : 1;
};

/* 社交指令的結構 */
struct  social_data
{
  SOCIAL_DATA * next;
  char        * name;
  char        * chinese_name;
  char        * char_no_arg;
  char        * others_no_arg;
  char        * char_found;
  char        * others_found;
  char        * vict_found;
  char        * char_auto;
  char        * others_auto;
};

/* 區域常數 */
extern  const   struct  social_data     social_table    [];
extern  char *  const                   attr_name       [MAX_ATTR];

/* 全域變數 */
extern          HELP_DATA         *     help_first;
extern          HELP_DATA         *     help_last;
extern          BAN_DATA          *     ban_list;
extern          GREETING_DATA     *     greeting_list;
extern          SYMBOL_DATA       *     symbol_list;
extern          SERVER_DATA       *     server_list;
extern          TOP_DATA          *     top_list;
extern          XNAMES_DATA       *     xnames_first;
extern          CHAR_DATA         *     char_list;
extern          SKILL_DATA        *     skill_list;
extern          SKILL_DATA        *     skill_last;
extern          CMD_DATA          *     cmd_list;
extern          CMD_DATA          *     cmd_last;
extern          DESCRIPTOR_DATA   *     descriptor_list;
extern          NOTE_DATA         *     note_list;
extern          VOTE_DATA         *     vote_list;
extern          JOKE_DATA         *     joke_list;
extern          BOARD_DATA        *     board_list;
extern          BOARD_DATA        *     board_last;
extern          OBJ_DATA          *     object_list;
extern          SOCIAL_DATA       *     social_first;
extern          SOCIAL_DATA       *     social_last;
extern          ANGEL_DATA        *     angel_list;
extern          BUS_DATA          *     bus_first;
extern          BUS_DATA          *     bus_last;
extern          CLUB_DATA         *     club_list;
extern          QUEST_INFO        *     quest_list;
extern          QUESTION_DATA     *     question_list;
extern          GIFT_DATA         *     gift_list;
extern          BOUNTY_DATA       *     bounty_list;
extern          EVENT_DATA        *     event_list;
extern          IMMLIST_DATA      *     imm_list;
extern          IMMLIST_DATA      *     imm_last;
extern          SALE_DATA         *     sale_list;
extern          SITUS_DATA        *     situs_list;
extern          VARIABLE_DATA     *     variable_list;
extern          WANTED_DATA       *     wanted_list;
extern          SHIP_DATA         *     ship_list;
extern          NET_DATA          *     net_list;
extern          SECTOR_DATA       *     sector_list;
extern          SECTOR_DATA       *     DefaultSector;
extern          CLASS_DATA        *     class_list;
extern          CLASS_DATA        *     class_demos;
extern          CLASS_DATA        *     class_force;
extern          LIQ_DATA          *     liq_list;
extern          LIQ_DATA          *     liq_water;
extern          ADDRESS_DATA      *     address_list;
extern          DATABASE_DATA     *     database_list;
extern          DATABASE_DATA     *     database_last;
extern          time_t                  current_time;
extern          time_t                  mud_boot_time;
extern          time_t                  reboot_time;
extern          time_t                  shutdown_time;
extern          int                     SaleChance;
extern          int                     SaleLimit;
extern          bool                    first_warn;
extern          bool                    second_warn;
extern          bool                    third_warn;
extern          bool                    AngelComing;
extern          bool                    AutoBackup;
extern          bool                    CheckServer;
extern          bool                    GetFQDN;
extern          bool                    RecordFile;
extern          int                     AngelTimes;
extern          int                     AngelDefault;
extern          int                     MaxConnect;
extern          long                    mud_boot_tick;
extern          bool                    fLogAll;
extern          KILL_DATA               kill_table      [];
extern          TIME_INFO_DATA          time_info;
extern          WEATHER_DATA            weather_info;
extern          AUCTION_DATA          * auction_info;
extern          FIGHT_DATA            * fight_info;
extern          AREA_DATA *             area_first;
extern          AREA_DATA *             area_last;
extern          HERO_DATA *             hero_first;
extern          HERO_DATA *             hero_last;
extern          SHOP_DATA *             shop_first;
extern          SHOP_DATA *             shop_last;
extern          PW_DATA   *             pw_list;
extern          GR_DATA   *             gr_list;
extern          bool                    merc_exec;
extern          STOCK_DATA              stock_data[MAX_STOCK];
extern          char                  * welcome_message;
extern          char                  * welcome_immortal;
extern          char                  * motd_message;
extern          size_t                  hold_day;
extern          size_t                  file_quota;
extern          int                     max_notes;
extern          int                     Rechristen;
extern          int                     load_hit_plus;
extern          int                     load_mana_plus;
extern          int                     load_move_plus;
extern          int                     MaxPractice;
extern          int                     MaxNSkill;
extern          int                     MaxFirman;
extern          int                     HeroFirman;
extern          int                     FoodDegrade;
extern          int                     BornAge;
extern          int                     DefaultHit;
extern          int                     DefaultMana;
extern          int                     DefaultMove;
extern          int                     DefaultGold;
extern          int                     DefaultStr;
extern          int                     DefaultInt;
extern          int                     DefaultWis;
extern          int                     DefaultDex;
extern          int                     DefaultCon;
extern          int                     DefaultPractice;
extern          int                     MaxStr;
extern          int                     MaxInt;
extern          int                     MaxWis;
extern          int                     MaxDex;
extern          int                     MaxCon;
extern          int                     MaxHit;
extern          int                     MaxMana;
extern          int                     MaxMove;
extern          int                     DefaultObject[MAX_DEFAULT_OBJECT];

/* ini.c */
extern          int                     MudPort[MAX_PORT];
extern          int                     MudConnect[MAX_PORT];
extern          int                     max_connect;
extern          int                     max_ip;
extern          int                     Increase;
extern          int                     IncreaseLimit;
extern          int                     DepositMoney;
extern          int                     EndowRate;
extern          int                     DonateMoney;
extern          int                     DonateBenifit;
extern          int                     DonateLevel;
extern          int                     DonateLimit;
extern          int                     DonateDuration;
extern          int                     DonateCount;
extern          bool                    DonateLock;
extern          int                     BankThreshold;
extern          int                     BankProfit;
extern          int                     SacGold;
extern          int                     MaxNumber;
extern          int                     FightRound;
extern          int                     FightDifferent;
extern          int                     FightRate;
extern          int                     FreeChar;
extern          int                     FreeObject;
extern          int                     FreeDesc;
extern          int                     FoggyRoom;
extern          AREA_DATA             * FoggyArea;
extern          int                     chat_penalty;
extern          int                     level_limit;
extern          int                     ForceLevel;
extern          int                     ipc_block;
extern          int                     ipc_idle;
extern          int                     max_stock_buy;
extern          int                     max_hire;
extern          bool                    internal_debug;
extern          bool                    wizlock;
extern          bool                    multilogin;
extern          bool                    strict_password;
extern          bool                    strict_email;
extern          bool                    tick_permit;
extern          char                    CurrentDir      [];
extern          char                    mud_name        [];
extern          char                    bbs_name        [];
extern          char                    home_dir        [];
extern          char                    help_dir        [];
extern          char                    sector_dir      [];
extern          char                    class_dir       [];
extern          char                    liq_dir         [];
extern          char                    social_dir      [];
extern          char                    angel_dir       [];
extern          char                    social_file     [];
extern          char                    area_dir        [];
extern          char                    board_dir       [];
extern          char                    board_lst       [];
extern          char                    board_sheet     [];
extern          char                    greet_dir       [];
extern          char                    vote_dir        [];
extern          char                    joke_dir        [];
extern          char                    note_dir        [];
extern          char                    mob_dir         [];
extern          char                    mob_ext         [];
extern          char                    room_dir        [];
extern          char                    room_ext        [];
extern          char                    obj_dir         [];
extern          char                    obj_ext         [];
extern          char                    joke_ext        [];
extern          char                    note_ext        [];
extern          char                    vote_ext        [];
extern          char                    help_ext        [];
extern          char                    angel_ext       [];
extern          char                    greet_ext       [];
extern          char                    sector_ext      [];
extern          char                    class_ext       [];
extern          char                    liquid_ext      [];
extern          char                    social_ext      [];
extern          char                    command_ext     [];
extern          char                    skill_ext       [];
extern          char                    reset_dir       [];
extern          char                    reset_ext       [];
extern          char                    shop_dir        [];
extern          char                    shop_ext        [];
extern          char                    mineral_dir     [];
extern          char                    mineral_ext     [];
extern          char                    dir_file        [];
extern          char                    player_dir      [];
extern          char                    index_file      [];
extern          char                    board_index     [];
extern          char                    null_file       [];
extern          char                    ideas_file      [];
extern          char                    typo_file       [];
extern          char                    newplayer_file  [];
extern          char                    symbol_file     [];
extern          char                    note_file       [];
extern          char                    check_file      [];
extern          char                    wizard_file     [];
extern          char                    bus_file        [];
extern          char                    gift_file       [];
extern          char                    date_file       [];
extern          char                    bounty_file     [];
extern          char                    event_file      [];
extern          char                    ship_file       [];
extern          char                    purge_file      [];
extern          char                    quest_file      [];
extern          char                    question_file   [];
extern          char                    immlist_file    [];
extern          char                    sale_file       [];
extern          char                    situs_file      [];
extern          char                    club_file       [];
extern          char                    donate_file     [];
extern          char                    internal_file   [];
extern          char                    site_file       [];
extern          char                    station_file    [];
extern          char                    xname_file      [];
extern          char                    stock_file      [];
extern          char                    promotion_file  [];
extern          char                    net_file        [];
extern          char                    welcome_file    [];
extern          char                    welcome_imm     [];
extern          char                    address_file    [];
extern          char                    error_file      [];
extern          char                    hero_file       [];
extern          char                    bugs_file       [];
extern          char                    failload_file   [];
extern          char                    failexit_file   [];
extern          char                    badobject_file  [];
extern          char                    failpass_file   [];
extern          char                    database_file   [];
extern          char                    failenable_file [];
extern          char                    bad_file        [];
extern          char                    suspect_file    [];
extern          char                    MOBProgs_dir    [];
extern          char                    skill_dir       [];
extern          char                    skill_file      [];
extern          char                    ins_dir         [];
extern          char                    ins_file        [];
extern          char                    xname_log       [];
extern          char                    chat_log        [];
extern          char                    suicide_log     [];
extern          char                    wizflags_log    [];
extern          char                    motd_file       [];

/* load.c */
extern          ROOM_INDEX_DATA *       room_index_hash [MAX_KEY_HASH];
extern          OBJ_INDEX_DATA  *       obj_index_hash  [MAX_KEY_HASH];
extern          MOB_INDEX_DATA  *       mob_index_hash  [MAX_KEY_HASH];
extern          DATABASE_DATA   *       data_index_hash [MAX_KEY_HASH];

extern void     set_exit_default        args( ( EXIT_DATA       * ) );
extern void     set_room_default        args( ( ROOM_INDEX_DATA * ) );
extern void     set_database_default    args( ( DATABASE_DATA   * ) );
extern void     set_immlist_default     args( ( IMMLIST_DATA    * ) );

#define SLOT_TWOSWORD                   1
#define SLOT_EVILFIST                   2
#define SLOT_FLOWER                     3
#define SLOT_FOUEN                      4
#define SLOT_DRAGON_BLADE               5
#define SLOT_SEVENCOLOR                 6
#define SLOT_NINE_STEP                  7
#define SLOT_LUNGSHAN                   8
#define SLOT_MOON_STEP                  9
#define SLOT_LINPO_STEPS               10
#define SLOT_GWHIP                     11
#define SLOT_G_STEPS                   12
#define SLOT_GDRAGON_STEPS             13
#define SLOT_RYO                       14
#define SLOT_NONAME                    15
#define SLOT_YOUNGGUN                  16
#define SLOT_CLOUD_STEPS               17
#define SLOT_LONGFIST                  18
#define SLOT_WINDLEG                   19
#define SLOT_TIGERBLADE                20
#define SLOT_MIRAGE_STEPS              21
#define SLOT_LOST_STEPS                22
#define SLOT_EIGHT_STEPS               23
#define SLOT_RULAI                     24
#define SLOT_NIGHT_STEPS               25
#define SLOT_PURPLE                    26
#define SLOT_B_A_1                     27
#define SLOT_CLOUD_GHOST               28
#define SLOT_BA_BLADE                  29
#define SLOT_SHOUTSKY                  30
#define SLOT_FLY_BLADE                 31
#define SLOT_BADSWORD                  32
#define SLOT_BE_NEEDLE                 33
#define SLOT_WIND_STEPS                34
#define SLOT_WINDCOLOR_STEPS           35
#define SLOT_COLOR_STEPS               36
#define SLOT_KING_FIST                 37
#define SLOT_KNOW_PEN                  38
#define SLOT_UNIVERSE                  39
#define SLOT_TIGER_AXE                 40
#define SLOT_TENDO_SLASH               41
#define SLOT_TAO_SPELL                 42
#define SLOT_MONKEY_STICK              43
#define SLOT_MING_SNAKE                44
#define SLOT_DRAGONLEG                 45
#define SLOT_DARK_WORD                 46
#define SLOT_DAYSTICK                  47
#define SLOT_DRAGON_DAGGER             48
#define SLOT_SKYDRAGON                 49
#define SLOT_SKYDRAGON_FORCE           50
#define SLOT_SKYDRAGON_SWORD           51
#define SLOT_DRAGONFIST                52
#define SLOT_DRAGONSLEEVE              53
#define SLOT_DREAM_BLADE               54
#define SLOT_DREAMDANCE                55
#define SLOT_DRUNK                     56
#define SLOT_DUANSUN_SWORD             57
#define SLOT_DUBAFIST                  58
#define SLOT_SIXFIRE                   59
#define SLOT_FAST_BLADE                60
#define SLOT_FIRE_LANCE                61
#define SLOT_FIRE_ICE                  62
#define SLOT_CLOUD_FIST                63
#define SLOT_SEVEN_DAGGER              64
#define SLOT_FLY_STEPS                 65
#define SLOT_FOLE_KEN                  66
#define SLOT_FONXANSWORD               67
#define SLOT_FROST_STEPS               68
#define SLOT_FUSWORDS                  69
#define SLOT_G_FINGER                  70
#define SLOT_GHOST_AXE                 71
#define SLOT_GHOST_MARIAL              72
#define SLOT_GHOST_STEPS               73
#define SLOT_GHOSTSTRIKE               74
#define SLOT_GODEAGLE_CLAW             75
#define SLOT_GOLD_BLADE                76
#define SLOT_GSWORD                    77
#define SLOT_HANZO_BLADE               78
#define SLOT_HATE_SWORD                79
#define SLOT_HELL_EVIL                 80
#define SLOT_HENMA_STEPS               81
#define SLOT_HER_BLADE                 82
#define SLOT_HUA_SWORD                 83
#define SLOT_HUNDRED_STEPS             84
#define SLOT_ICEBLADE                  85
#define SLOT_ICEFIST                   86
#define SLOT_YU_STEPS                  87
#define SLOT_YU_NEEDLE                 88
#define SLOT_SEVEN_MIND                89
#define SLOT_ICEFREEZE_STRIKE          90
#define SLOT_WUMIN_SWORD               91
#define SLOT_SEAFLOW_BLADE             92
#define SLOT_RAIN_THROWING             93
#define SLOT_SHA_STEPS                 94
#define SLOT_SHADOW_KILL               95
#define SLOT_SHADE_STEPS               96
#define SLOT_SKY_STEPS                 97
#define SLOT_SUN_BLADE                 98
#define SLOT_SNOW_MARTIAL              99
#define SLOT_SIX_FINGERS              100
#define SLOT_SLEEV_STEPS              101
#define SLOT_SHANSWORD                102
#define SLOT_MAKE_SPRING              103
#define SLOT_MAKE_LIGHT               104
#define SLOT_CAST_ARMOR               105
#define SLOT_CAST_BLESS               106
#define SLOT_CAST_FIRE                107
#define SLOT_CAST_LIGHTNING           108
#define SLOT_CONTROL_WEATHER          109
#define SLOT_LOCATE_OBJECT            110
#define SLOT_SUMMON                   111
#define SLOT_VENTRILOQUATE            112
#define SLOT_ACID_BLAST               113
#define SLOT_ACID_BREATH              114
#define SLOT_BLINDNESS                115
#define SLOT_BURNING_HANDS            116
#define SLOT_CALL_LIGHTNING           117
#define SLOT_CAUSE_CRITICAL           118
#define SLOT_CAUSE_LIGHT              119
#define SLOT_CAUSE_SERIOUS            120
#define SLOT_CHANGE_SEX               121
#define SLOT_CHARM_PERSON             122
#define SLOT_CHILL_TOUCH              123
#define SLOT_COLOUR_SPRAY             124
#define SLOT_CREATE_FOOD              125
#define SLOT_CREATE_WATER             126
#define SLOT_CURE_BLINDNESS           127
#define SLOT_CURE_CRITICAL            128
#define SLOT_CURE_LIGHT               129
#define SLOT_CURE_POISON              130
#define SLOT_CURE_SERIOUS             131
#define SLOT_CURSE                    132
#define SLOT_DETECT_EVIL              133
#define SLOT_DETECT_HIDDEN            134
#define SLOT_DETECT_INVIS             135
#define SLOT_DETECT_MAGIC             136
#define SLOT_DETECT_POISON            137
#define SLOT_DISPEL_EVIL              138
#define SLOT_DISPEL_MAGIC             139
#define SLOT_EARTHQUAKE               140
#define SLOT_ENCHANT_WEAPON           141
#define SLOT_ENERGY_DRAIN             142
#define SLOT_FAERIE_FIRE              143
#define SLOT_FAERIE_FOG               144
#define SLOT_FIRE_BREATH              145
#define SLOT_FIREBALL                 146
#define SLOT_FLAMESTRIKE              147
#define SLOT_FLY                      148
#define SLOT_FROST_BREATH             149
#define SLOT_GAS_BREATH               150
#define SLOT_GATE                     151
#define SLOT_GENERAL_PURPOSE          152
#define SLOT_GIANT_STRENGTH           153
#define SLOT_HARM                     154
#define SLOT_HEAL                     155
#define SLOT_HIGH_EXPLOSIVE           156
#define SLOT_IDENTIFY                 157
#define SLOT_INFRAVISION              158
#define SLOT_INVIS                    159
#define SLOT_KNOW_ALIGNMENT           160
#define SLOT_LIGHTNING_BOLT           161
#define SLOT_LIGHTNING_BREATH         162
#define SLOT_MAGIC_MISSILE            163
#define SLOT_MASS_INVIS               164
#define SLOT_PASS_DOOR                165
#define SLOT_POISON                   166
#define SLOT_PROTECTION               167
#define SLOT_REFRESH                  168
#define SLOT_REMOVE_CURSE             169
#define SLOT_SANCTUARY                170
#define SLOT_SHIELD                   171
#define SLOT_SHOCKING_GRASP           172
#define SLOT_SLEEP                    173
#define SLOT_STONE_SKIN               174
#define SLOT_TELEPORT                 175
#define SLOT_WEAKEN                   176
#define SLOT_WORD_OF_RECALL           177
#define SLOT_FLEE                     178
#define SLOT_SNEAK                    179
#define SLOT_HIDE                     180
#define SLOT_KING_ROAD                181
#define SLOT_LUBUREVENGE              182
#define SLOT_PRY                      183
#define SLOT_JUDGE                    184
#define SLOT_HASHIN                   185
#define SLOT_EIGHT_GUN                186
#define SLOT_MODO_SLASH               187
#define SLOT_SLASH_LIGHT              188
#define SLOT_SEVEN_KEY_NUMEN          189
#define SLOT_BLOOD_TEN                190
#define SLOT_EVIL_KING                191
#define SLOT_SHOOT_SUN                192
#define SLOT_WRITE_PEN                193
#define SLOT_FLY_NINE                 194
#define SLOT_FIRE_ELF                 195
#define SLOT_DORMANCY                 196
#define SLOT_PHYSIQUE                 197
#define SLOT_MEDITATION               198
#define SLOT_ANTI_PHYSIQUE            199
#define SLOT_FIXITY                   200
#define SLOT_VENOM                    201
#define SLOT_DEVELOP                  202
#define SLOT_REPAIR                   203
#define SLOT_COOK                     204
#define SLOT_STEAL                    205
#define SLOT_PICK                     206
#define SLOT_DEFLORATION              207
#define SLOT_CANNIBAL                 208
#define SLOT_WAKEUP                   209
#define SLOT_EVIL_POWER               210
#define SLOT_CRASH_WEAPON             211
#define SLOT_BYSTANDER_FIST           212
#define SLOT_FLYING_STONE             213
#define SLOT_SLOW                     214
#define SLOT_MOUNT                    215
#define SLOT_MOBILE_KILL              216
#define SLOT_MINT                     217
#define SLOT_MOBILE_STEPS             218
#define SLOT_CITIZEN                  219
#define SLOT_FAN_HAMMER               220
#define SLOT_HOLY_FIST                221
#define SLOT_FIRE_DRAGON              222
#define SLOT_CRY_GHOST                223
#define SLOT_ROCK_SLASH               224
#define SLOT_DRAGON_SHOUT             225
#define SLOT_SIX_SWORD                226
#define SLOT_THUNDER_HAMMER           227
#define SLOT_JADE_EIGHT               228
#define SLOT_DRAGON_PHOE              229
#define SLOT_DRAGON_HEROISM           230
#define SLOT_ETEN                     231
#define SLOT_POWER_WORD               232
#define SLOT_PRESERVE                 233
#define SLOT_CURE_OVERDOSE            234
#define SLOT_GOLD_SHIELD              235
#define SLOT_IRON_SHIELD              236
#define SLOT_DEVA_SHIELD              237
#define SLOT_REMEDAY                  238
#define SLOT_MAGIC_POWER              239
#define SLOT_TAICHI_FIST              240
#define SLOT_RESTORE_STAFF            241
#define SLOT_MASK                     242
#define SLOT_DETECT_MASK              243
#define SLOT_SPARKLER                 244
#define SLOT_SEARCH                   245
#define SLOT_SWIM                     246
#define SLOT_WIND_BLADE               247
#define SLOT_VACUUM_WIND              248
#define SLOT_DREAM_SOUL               249
#define SLOT_ABSTRACT                 250
#define SLOT_HASTE                    251
#define SLOT_SUPER_REFRESH            252
#define SLOT_DARK_HELL                253
#define SLOT_SIX_GOD                  254
#define SLOT_BACKSTAB                 255
#define SLOT_WATER_CLOUD_BLAST        256
#define SLOT_FUN_WU_BLAST             257
#define SLOT_MAGIC_BLAST              258
#define SLOT_SKY_BLADE                259
#define SLOT_DREAM_TEAR               260
#define SLOT_FREE_STEPS               261
#define SLOT_SEVEN_STAR               262
#define SLOT_TREE_TOUCH               263
#define SLOT_MAKE_SWORD               264

DECLARE_DO_FUN( do_advance      );
DECLARE_DO_FUN( do_aecho        );
DECLARE_DO_FUN( do_habilitate   );
DECLARE_DO_FUN( do_alias        );
DECLARE_DO_FUN( do_allow        );
DECLARE_DO_FUN( do_angel        );
DECLARE_DO_FUN( do_answer       );
DECLARE_DO_FUN( do_areas        );
DECLARE_DO_FUN( do_ask          );
DECLARE_DO_FUN( do_at           );
DECLARE_DO_FUN( do_auction      );
DECLARE_DO_FUN( do_auto         );
DECLARE_DO_FUN( do_autoexit     );
DECLARE_DO_FUN( do_autoloot     );
DECLARE_DO_FUN( do_automap      );
DECLARE_DO_FUN( do_autosac      );
DECLARE_DO_FUN( do_address      );
DECLARE_DO_FUN( do_backup       );
DECLARE_DO_FUN( do_badfriend    );
DECLARE_DO_FUN( do_bamfin       );
DECLARE_DO_FUN( do_bamfout      );
DECLARE_DO_FUN( do_ban          );
DECLARE_DO_FUN( do_bet          );
DECLARE_DO_FUN( do_blank        );
DECLARE_DO_FUN( do_board        );
DECLARE_DO_FUN( do_bounty       );
DECLARE_DO_FUN( do_brief        );
DECLARE_DO_FUN( do_bug          );
DECLARE_DO_FUN( do_buy          );
DECLARE_DO_FUN( do_bus          );
DECLARE_DO_FUN( do_call         );
DECLARE_DO_FUN( do_cast         );
DECLARE_DO_FUN( do_cat          );
DECLARE_DO_FUN( do_cchat        );
DECLARE_DO_FUN( do_cd           );
DECLARE_DO_FUN( do_chance       );
DECLARE_DO_FUN( do_channels     );
DECLARE_DO_FUN( do_chat         );
DECLARE_DO_FUN( do_check        );
DECLARE_DO_FUN( do_cleanup      );
DECLARE_DO_FUN( do_close        );
DECLARE_DO_FUN( do_cls          );
DECLARE_DO_FUN( do_cname        );
DECLARE_DO_FUN( do_combine      );
DECLARE_DO_FUN( do_commands     );
DECLARE_DO_FUN( do_compare      );
DECLARE_DO_FUN( do_config       );
DECLARE_DO_FUN( do_consider     );
DECLARE_DO_FUN( do_crave        );
DECLARE_DO_FUN( do_credit       );
DECLARE_DO_FUN( do_club         );
DECLARE_DO_FUN( do_clubset      );
DECLARE_DO_FUN( do_clubchat     );
DECLARE_DO_FUN( do_cp           );
DECLARE_DO_FUN( do_damage       );
DECLARE_DO_FUN( do_data         );
DECLARE_DO_FUN( do_debug        );
DECLARE_DO_FUN( do_deny         );
DECLARE_DO_FUN( do_deposit      );
DECLARE_DO_FUN( do_description  );
DECLARE_DO_FUN( do_directory    );
DECLARE_DO_FUN( do_disconnect   );
DECLARE_DO_FUN( do_dormancy     );
DECLARE_DO_FUN( do_donate       );
DECLARE_DO_FUN( do_down         );
DECLARE_DO_FUN( do_edit         );
DECLARE_DO_FUN( do_enter        );
DECLARE_DO_FUN( do_endow        );
DECLARE_DO_FUN( do_enquire      );
DECLARE_DO_FUN( do_email        );
DECLARE_DO_FUN( do_event        );
DECLARE_DO_FUN( do_drink        );
DECLARE_DO_FUN( do_drop         );
DECLARE_DO_FUN( do_east         );
DECLARE_DO_FUN( do_eat          );
DECLARE_DO_FUN( do_echo         );
DECLARE_DO_FUN( do_emote        );
DECLARE_DO_FUN( do_enable       );
DECLARE_DO_FUN( do_equipment    );
DECLARE_DO_FUN( do_examine      );
DECLARE_DO_FUN( do_exits        );
DECLARE_DO_FUN( do_fcntl        );
DECLARE_DO_FUN( do_feed         );
DECLARE_DO_FUN( do_fight        );
DECLARE_DO_FUN( do_find         );
DECLARE_DO_FUN( do_filename     );
DECLARE_DO_FUN( do_filestat     );
DECLARE_DO_FUN( do_fill         );
DECLARE_DO_FUN( do_finger       );
DECLARE_DO_FUN( do_fire         );
DECLARE_DO_FUN( do_flee         );
DECLARE_DO_FUN( do_flow         );
DECLARE_DO_FUN( do_follow       );
DECLARE_DO_FUN( do_force        );
DECLARE_DO_FUN( do_forsake      );
DECLARE_DO_FUN( do_free         );
DECLARE_DO_FUN( do_freeze       );
DECLARE_DO_FUN( do_friend       );
DECLARE_DO_FUN( do_gamble       );
DECLARE_DO_FUN( do_get          );
DECLARE_DO_FUN( do_give         );
DECLARE_DO_FUN( do_gift         );
DECLARE_DO_FUN( do_goto         );
DECLARE_DO_FUN( do_group        );
DECLARE_DO_FUN( do_grep         );
DECLARE_DO_FUN( do_gtell        );
DECLARE_DO_FUN( do_help         );
DECLARE_DO_FUN( do_hero         );
DECLARE_DO_FUN( do_hire         );
DECLARE_DO_FUN( do_history      );
DECLARE_DO_FUN( do_holylight    );
DECLARE_DO_FUN( do_nodeath      );
DECLARE_DO_FUN( do_nuke         );
DECLARE_DO_FUN( do_hp           );
DECLARE_DO_FUN( do_home         );
DECLARE_DO_FUN( do_idea         );
DECLARE_DO_FUN( do_immtalk      );
DECLARE_DO_FUN( do_inventory    );
DECLARE_DO_FUN( do_invis        );
DECLARE_DO_FUN( do_ipcs         );
DECLARE_DO_FUN( do_jail         );
DECLARE_DO_FUN( do_joke         );
DECLARE_DO_FUN( do_kill         );
DECLARE_DO_FUN( do_cachet       );
DECLARE_DO_FUN( do_gchat        );
DECLARE_DO_FUN( do_learn        );
DECLARE_DO_FUN( do_leftskill    );
DECLARE_DO_FUN( do_level        );
DECLARE_DO_FUN( do_lfind        );
DECLARE_DO_FUN( do_liquid       );
DECLARE_DO_FUN( do_list         );
DECLARE_DO_FUN( do_llink        );
DECLARE_DO_FUN( do_llookup      );
DECLARE_DO_FUN( do_load         );
DECLARE_DO_FUN( do_lock         );
DECLARE_DO_FUN( do_log          );
DECLARE_DO_FUN( do_look         );
DECLARE_DO_FUN( do_lore         );
DECLARE_DO_FUN( do_lotto        );
DECLARE_DO_FUN( do_ls           );
DECLARE_DO_FUN( do_lset         );
DECLARE_DO_FUN( do_lockcmd      );
DECLARE_DO_FUN( do_map          );
DECLARE_DO_FUN( do_marry        );
DECLARE_DO_FUN( do_memory       );
DECLARE_DO_FUN( do_md           );
DECLARE_DO_FUN( do_mfind        );
DECLARE_DO_FUN( do_miset        );
DECLARE_DO_FUN( do_mlevel       );
DECLARE_DO_FUN( do_mlist        );
DECLARE_DO_FUN( do_mload        );
DECLARE_DO_FUN( do_mode         );
DECLARE_DO_FUN( do_mpadd        );
DECLARE_DO_FUN( do_mpasound     );
DECLARE_DO_FUN( do_mpat         );
DECLARE_DO_FUN( do_mpchat       );
DECLARE_DO_FUN( do_mpecho       );
DECLARE_DO_FUN( do_mpechoaround );
DECLARE_DO_FUN( do_mpechoat     );
DECLARE_DO_FUN( do_mpclean      );
DECLARE_DO_FUN( do_mpforce      );
DECLARE_DO_FUN( do_mpgoto       );
DECLARE_DO_FUN( do_mpjunk       );
DECLARE_DO_FUN( do_mpkill       );
DECLARE_DO_FUN( do_mplset       );
DECLARE_DO_FUN( do_mpmload      );
DECLARE_DO_FUN( do_mpoload      );
DECLARE_DO_FUN( do_mppurge      );
DECLARE_DO_FUN( do_mpremenemy   );
DECLARE_DO_FUN( do_mpremquest   );
DECLARE_DO_FUN( do_mpsetenemy   );
DECLARE_DO_FUN( do_mpsetquest   );
DECLARE_DO_FUN( do_mpstat       );
DECLARE_DO_FUN( do_mptransfer   );
DECLARE_DO_FUN( do_mpsay        );
DECLARE_DO_FUN( do_mset         );
DECLARE_DO_FUN( do_mstat        );
DECLARE_DO_FUN( do_multi        );
DECLARE_DO_FUN( do_murder       );
DECLARE_DO_FUN( do_music        );
DECLARE_DO_FUN( do_mwhere       );
DECLARE_DO_FUN( do_mount        );
DECLARE_DO_FUN( do_mud          );
DECLARE_DO_FUN( do_noemote      );
DECLARE_DO_FUN( do_north        );
DECLARE_DO_FUN( do_note         );
DECLARE_DO_FUN( do_notell       );
DECLARE_DO_FUN( do_notepad      );
DECLARE_DO_FUN( do_notify       );
DECLARE_DO_FUN( do_netstat      );
DECLARE_DO_FUN( do_oaf          );
DECLARE_DO_FUN( do_ofind        );
DECLARE_DO_FUN( do_okip         );
DECLARE_DO_FUN( do_olevel       );
DECLARE_DO_FUN( do_olist        );
DECLARE_DO_FUN( do_oload        );
DECLARE_DO_FUN( do_open         );
DECLARE_DO_FUN( do_order        );
DECLARE_DO_FUN( do_oset         );
DECLARE_DO_FUN( do_ostat        );
DECLARE_DO_FUN( do_otype        );
DECLARE_DO_FUN( do_owhere       );
DECLARE_DO_FUN( do_out          );
DECLARE_DO_FUN( do_pagelen      );
DECLARE_DO_FUN( do_paint        );
DECLARE_DO_FUN( do_pardon       );
DECLARE_DO_FUN( do_password     );
DECLARE_DO_FUN( do_peace        );
DECLARE_DO_FUN( do_perm         );
DECLARE_DO_FUN( do_pick         );
DECLARE_DO_FUN( do_phone        );
DECLARE_DO_FUN( do_ply          );
DECLARE_DO_FUN( do_post         );
DECLARE_DO_FUN( do_prompt       );
DECLARE_DO_FUN( do_pry          );
DECLARE_DO_FUN( do_prof         );
DECLARE_DO_FUN( do_purge        );
DECLARE_DO_FUN( do_put          );
DECLARE_DO_FUN( do_ps           );
DECLARE_DO_FUN( do_pwd          );
DECLARE_DO_FUN( do_qstat        );
DECLARE_DO_FUN( do_qset         );
DECLARE_DO_FUN( do_query        );
DECLARE_DO_FUN( do_question     );
DECLARE_DO_FUN( do_quit         );
DECLARE_DO_FUN( do_read         );
DECLARE_DO_FUN( do_release      );
DECLARE_DO_FUN( do_rebirth      );
DECLARE_DO_FUN( do_reboot       );
DECLARE_DO_FUN( do_recall       );
DECLARE_DO_FUN( do_recho        );
DECLARE_DO_FUN( do_recite       );
DECLARE_DO_FUN( do_reload       );
DECLARE_DO_FUN( do_remove       );
DECLARE_DO_FUN( do_rename       );
DECLARE_DO_FUN( do_reply        );
DECLARE_DO_FUN( do_report       );
DECLARE_DO_FUN( do_rest         );
DECLARE_DO_FUN( do_restore      );
DECLARE_DO_FUN( do_retire       );
DECLARE_DO_FUN( do_rd           );
DECLARE_DO_FUN( do_rlimit       );
DECLARE_DO_FUN( do_rm           );
DECLARE_DO_FUN( do_rset         );
DECLARE_DO_FUN( do_rstat        );
DECLARE_DO_FUN( do_repair       );
DECLARE_DO_FUN( do_sacrifice    );
DECLARE_DO_FUN( do_sale         );
DECLARE_DO_FUN( do_save         );
DECLARE_DO_FUN( do_say          );
DECLARE_DO_FUN( do_score        );
DECLARE_DO_FUN( do_search       );
DECLARE_DO_FUN( do_see          );
DECLARE_DO_FUN( do_sefind       );
DECLARE_DO_FUN( do_sell         );
DECLARE_DO_FUN( do_semote       );
DECLARE_DO_FUN( do_serial       );
DECLARE_DO_FUN( do_shout        );
DECLARE_DO_FUN( do_ship         );
DECLARE_DO_FUN( do_shutdown     );
DECLARE_DO_FUN( do_silence      );
DECLARE_DO_FUN( do_slay         );
DECLARE_DO_FUN( do_sleep        );
DECLARE_DO_FUN( do_snoop        );
DECLARE_DO_FUN( do_socials      );
DECLARE_DO_FUN( do_south        );
DECLARE_DO_FUN( do_split        );
DECLARE_DO_FUN( do_spy          );
DECLARE_DO_FUN( do_sset         );
DECLARE_DO_FUN( do_stand        );
DECLARE_DO_FUN( do_status       );
DECLARE_DO_FUN( do_stock        );
DECLARE_DO_FUN( do_store        );
DECLARE_DO_FUN( do_study        );
DECLARE_DO_FUN( do_suicide      );
DECLARE_DO_FUN( do_sysconf      );
DECLARE_DO_FUN( do_sysinfo      );
DECLARE_DO_FUN( do_system       );
DECLARE_DO_FUN( do_sector       );
DECLARE_DO_FUN( do_send         );
DECLARE_DO_FUN( do_shop         );
DECLARE_DO_FUN( do_site         );
DECLARE_DO_FUN( do_task         );
DECLARE_DO_FUN( do_tcp          );
DECLARE_DO_FUN( do_tell         );
DECLARE_DO_FUN( do_test         );
DECLARE_DO_FUN( do_tick         );
DECLARE_DO_FUN( do_time         );
DECLARE_DO_FUN( do_title        );
DECLARE_DO_FUN( do_toss         );
DECLARE_DO_FUN( do_touch        );
DECLARE_DO_FUN( do_top          );
DECLARE_DO_FUN( do_trade        );
DECLARE_DO_FUN( do_train        );
DECLARE_DO_FUN( do_transfer     );
DECLARE_DO_FUN( do_trust        );
DECLARE_DO_FUN( do_turn         );
DECLARE_DO_FUN( do_typo         );
DECLARE_DO_FUN( do_uname        );
DECLARE_DO_FUN( do_unlock       );
DECLARE_DO_FUN( do_up           );
DECLARE_DO_FUN( do_users        );
DECLARE_DO_FUN( do_unmount      );
DECLARE_DO_FUN( do_value        );
DECLARE_DO_FUN( do_visible      );
DECLARE_DO_FUN( do_vocation     );
DECLARE_DO_FUN( do_vote         );
DECLARE_DO_FUN( do_wake         );
DECLARE_DO_FUN( do_wanted       );
DECLARE_DO_FUN( do_wear         );
DECLARE_DO_FUN( do_weather      );
DECLARE_DO_FUN( do_west         );
DECLARE_DO_FUN( do_where        );
DECLARE_DO_FUN( do_whereis      );
DECLARE_DO_FUN( do_who          );
DECLARE_DO_FUN( do_wimpy        );
DECLARE_DO_FUN( do_withdraw     );
DECLARE_DO_FUN( do_wizhelp      );
DECLARE_DO_FUN( do_wizlist      );
DECLARE_DO_FUN( do_wizlock      );
DECLARE_DO_FUN( do_xnames       );
DECLARE_DO_FUN( do_yell         );

DECLARE_SPELL_FUN( skill_sneak              );
DECLARE_SPELL_FUN( skill_hide               );
DECLARE_SPELL_FUN( skill_mask               );

DECLARE_SPELL_FUN( spell_make_spring        );
DECLARE_SPELL_FUN( spell_make_light         );
DECLARE_SPELL_FUN( spell_cast_armor         );
DECLARE_SPELL_FUN( spell_cast_bless         );
DECLARE_SPELL_FUN( spell_cast_fire          );
DECLARE_SPELL_FUN( spell_cast_lightning     );

DECLARE_SPELL_FUN( cast_control_weather     );
DECLARE_SPELL_FUN( cast_locate_object       );
DECLARE_SPELL_FUN( cast_summon              );
DECLARE_SPELL_FUN( cast_scan                );
DECLARE_SPELL_FUN( cast_sparkler            );
DECLARE_SPELL_FUN( cast_search              );
DECLARE_SPELL_FUN( cast_ventriloquate       );
DECLARE_SPELL_FUN( cast_abstract            );
DECLARE_SPELL_FUN( cast_acid_blast          );
DECLARE_SPELL_FUN( cast_acid_breath         );
DECLARE_SPELL_FUN( cast_blindness           );
DECLARE_SPELL_FUN( cast_burning_hands       );
DECLARE_SPELL_FUN( cast_call_lightning      );
DECLARE_SPELL_FUN( cast_cause_critical      );
DECLARE_SPELL_FUN( cast_cause_light         );
DECLARE_SPELL_FUN( cast_cause_serious       );
DECLARE_SPELL_FUN( cast_change_sex          );
DECLARE_SPELL_FUN( cast_charm_person        );
DECLARE_SPELL_FUN( cast_chill_touch         );
DECLARE_SPELL_FUN( cast_colour_spray        );
DECLARE_SPELL_FUN( cast_create_food         );
DECLARE_SPELL_FUN( cast_create_water        );
DECLARE_SPELL_FUN( cast_cure_blindness      );
DECLARE_SPELL_FUN( cast_cure_critical       );
DECLARE_SPELL_FUN( cast_cure_light          );
DECLARE_SPELL_FUN( cast_cure_poison         );
DECLARE_SPELL_FUN( cast_cure_serious        );
DECLARE_SPELL_FUN( cast_curse               );
DECLARE_SPELL_FUN( cast_detect_evil         );
DECLARE_SPELL_FUN( cast_detect_hidden       );
DECLARE_SPELL_FUN( cast_detect_invis        );
DECLARE_SPELL_FUN( cast_detect_mask         );
DECLARE_SPELL_FUN( cast_detect_magic        );
DECLARE_SPELL_FUN( cast_detect_poison       );
DECLARE_SPELL_FUN( cast_dispel_evil         );
DECLARE_SPELL_FUN( cast_dispel_magic        );
DECLARE_SPELL_FUN( cast_earthquake          );
DECLARE_SPELL_FUN( cast_enchant_weapon      );
DECLARE_SPELL_FUN( cast_energy_drain        );
DECLARE_SPELL_FUN( cast_faerie_fire         );
DECLARE_SPELL_FUN( cast_faerie_fog          );
DECLARE_SPELL_FUN( cast_fire_breath         );
DECLARE_SPELL_FUN( cast_fireball            );
DECLARE_SPELL_FUN( cast_flamestrike         );
DECLARE_SPELL_FUN( cast_fly                 );
DECLARE_SPELL_FUN( cast_frost_breath        );
DECLARE_SPELL_FUN( cast_gas_breath          );
DECLARE_SPELL_FUN( cast_gate                );
DECLARE_SPELL_FUN( cast_general_purpose     );
DECLARE_SPELL_FUN( cast_giant_strength      );
DECLARE_SPELL_FUN( cast_harm                );
DECLARE_SPELL_FUN( cast_heal                );
DECLARE_SPELL_FUN( cast_high_explosive      );
DECLARE_SPELL_FUN( cast_identify            );
DECLARE_SPELL_FUN( cast_infravision         );
DECLARE_SPELL_FUN( cast_invis               );
DECLARE_SPELL_FUN( cast_judge               );
DECLARE_SPELL_FUN( cast_know_alignment      );
DECLARE_SPELL_FUN( cast_lightning_bolt      );
DECLARE_SPELL_FUN( cast_lightning_breath    );
DECLARE_SPELL_FUN( cast_magic_missile       );
DECLARE_SPELL_FUN( cast_mass_invis          );
DECLARE_SPELL_FUN( cast_null                );
DECLARE_SPELL_FUN( cast_pass_door           );
DECLARE_SPELL_FUN( cast_poison              );
DECLARE_SPELL_FUN( cast_protection          );
DECLARE_SPELL_FUN( cast_pry                 );
DECLARE_SPELL_FUN( cast_refresh             );
DECLARE_SPELL_FUN( cast_remove_curse        );
DECLARE_SPELL_FUN( cast_sanctuary           );
DECLARE_SPELL_FUN( cast_shield              );
DECLARE_SPELL_FUN( cast_shocking_grasp      );
DECLARE_SPELL_FUN( cast_sleep               );
DECLARE_SPELL_FUN( cast_stone_skin          );
DECLARE_SPELL_FUN( cast_teleport            );
DECLARE_SPELL_FUN( cast_weaken              );
DECLARE_SPELL_FUN( cast_word_of_recall      );
DECLARE_SPELL_FUN( cast_seven_key_numen     );
DECLARE_SPELL_FUN( cast_fire_elf            );
DECLARE_SPELL_FUN( cast_dormancy            );
DECLARE_SPELL_FUN( cast_physique            );
DECLARE_SPELL_FUN( cast_meditation          );
DECLARE_SPELL_FUN( cast_anti_physique       );
DECLARE_SPELL_FUN( cast_fixity              );
DECLARE_SPELL_FUN( cast_venom               );
DECLARE_SPELL_FUN( cast_develop             );
DECLARE_SPELL_FUN( cast_repair              );
DECLARE_SPELL_FUN( cast_cook                );
DECLARE_SPELL_FUN( cast_steal               );
DECLARE_SPELL_FUN( cast_pick                );
DECLARE_SPELL_FUN( cast_defloration         );
DECLARE_SPELL_FUN( cast_cannibal            );
DECLARE_SPELL_FUN( cast_wakeup              );
DECLARE_SPELL_FUN( cast_evil_power          );
DECLARE_SPELL_FUN( cast_crash_weapon        );
DECLARE_SPELL_FUN( cast_bystander_fist      );
DECLARE_SPELL_FUN( cast_flying_stone        );
DECLARE_SPELL_FUN( cast_slow                );
DECLARE_SPELL_FUN( cast_mint                );
DECLARE_SPELL_FUN( cast_power_word          );
DECLARE_SPELL_FUN( cast_preserve            );
DECLARE_SPELL_FUN( cast_cure_overdose       );
DECLARE_SPELL_FUN( cast_gold_shield         );
DECLARE_SPELL_FUN( cast_iron_shield         );
DECLARE_SPELL_FUN( cast_deva_shield         );
DECLARE_SPELL_FUN( cast_remeday             );
DECLARE_SPELL_FUN( cast_magic_power         );
DECLARE_SPELL_FUN( cast_taichi_fist         );
DECLARE_SPELL_FUN( cast_restore_staff       );
DECLARE_SPELL_FUN( cast_wind_blade          );
DECLARE_SPELL_FUN( cast_vacuum_wind         );
DECLARE_SPELL_FUN( cast_haste               );
DECLARE_SPELL_FUN( cast_super_refresh       );
DECLARE_SPELL_FUN( cast_dark_hell           );
DECLARE_SPELL_FUN( cast_six_god             );
DECLARE_SPELL_FUN( cast_backstab            );
DECLARE_SPELL_FUN( cast_water_cloud_blast   );
DECLARE_SPELL_FUN( cast_fun_wu_blast        );
DECLARE_SPELL_FUN( cast_make_sword          );

/* 確認技能的函數 */
DECLARE_CHECK_FUN( check_sword_attack       );
DECLARE_CHECK_FUN( check_unrigid_attack     );
DECLARE_CHECK_FUN( check_pc_attack          );
DECLARE_CHECK_FUN( check_spear_attack       );
DECLARE_CHECK_FUN( check_blade_attack       );
DECLARE_CHECK_FUN( check_whip_attack        );
DECLARE_CHECK_FUN( check_dagger_attack      );
DECLARE_CHECK_FUN( check_axe_attack         );
DECLARE_CHECK_FUN( check_club_attack        );
DECLARE_CHECK_FUN( check_pen_attack         );
DECLARE_CHECK_FUN( check_bow_attack         );
DECLARE_CHECK_FUN( check_hammer_attack      );

/* 特攻函數 */
DECLARE_ROUTINE_FUN( routine_none           );

/* 觸發事件函數 */
DECLARE_EVENT_FUN( event_thunder       );
DECLARE_EVENT_FUN( event_solar_eclipse );
DECLARE_EVENT_FUN( event_month_eclipse );
DECLARE_EVENT_FUN( event_drought       );
DECLARE_EVENT_FUN( event_locust        );

/* 守護神的函數 */
DECLARE_ANGEL_FUN( angel_mount        );
DECLARE_ANGEL_FUN( angel_sleep        );
DECLARE_ANGEL_FUN( angel_drink        );
DECLARE_ANGEL_FUN( angel_eat          );
DECLARE_ANGEL_FUN( angel_marry        );
DECLARE_ANGEL_FUN( angel_wedding      );
DECLARE_ANGEL_FUN( angel_alias        );
DECLARE_ANGEL_FUN( angel_auction      );
DECLARE_ANGEL_FUN( angel_bet          );
DECLARE_ANGEL_FUN( angel_areas        );
DECLARE_ANGEL_FUN( angel_xname        );
DECLARE_ANGEL_FUN( angel_immortal     );
DECLARE_ANGEL_FUN( angel_deposit_gold );
DECLARE_ANGEL_FUN( angel_deposit_obj  );
DECLARE_ANGEL_FUN( angel_stock        );
DECLARE_ANGEL_FUN( angel_gamble       );
DECLARE_ANGEL_FUN( angel_direction    );
DECLARE_ANGEL_FUN( angel_shop         );
DECLARE_ANGEL_FUN( angel_smith        );
DECLARE_ANGEL_FUN( angel_mercenary    );
DECLARE_ANGEL_FUN( angel_coper        );
DECLARE_ANGEL_FUN( angel_kill         );
DECLARE_ANGEL_FUN( angel_bank         );
DECLARE_ANGEL_FUN( angel_obj_bank     );
DECLARE_ANGEL_FUN( angel_get          );
DECLARE_ANGEL_FUN( angel_drop         );
DECLARE_ANGEL_FUN( angel_withdraw     );
DECLARE_ANGEL_FUN( angel_unmount      );
DECLARE_ANGEL_FUN( angel_call         );
DECLARE_ANGEL_FUN( angel_bus          );
DECLARE_ANGEL_FUN( angel_ship         );
DECLARE_ANGEL_FUN( angel_lore         );
DECLARE_ANGEL_FUN( angel_badman       );
DECLARE_ANGEL_FUN( angel_wake         );
DECLARE_ANGEL_FUN( angel_lotto        );
DECLARE_ANGEL_FUN( angel_hero         );
DECLARE_ANGEL_FUN( angel_autosac      );
DECLARE_ANGEL_FUN( angel_autoloot     );
DECLARE_ANGEL_FUN( angel_friend       );
DECLARE_ANGEL_FUN( angel_wimpy        );
DECLARE_ANGEL_FUN( angel_enable       );
DECLARE_ANGEL_FUN( angel_flee         );
DECLARE_ANGEL_FUN( angel_attack       );
DECLARE_ANGEL_FUN( angel_meditation   );
DECLARE_ANGEL_FUN( angel_description  );
DECLARE_ANGEL_FUN( angel_club         );
DECLARE_ANGEL_FUN( angel_group        );
DECLARE_ANGEL_FUN( angel_nogroup      );
DECLARE_ANGEL_FUN( angel_reply        );
DECLARE_ANGEL_FUN( angel_fountain     );
DECLARE_ANGEL_FUN( angel_food         );
DECLARE_ANGEL_FUN( angel_gambleset    );
DECLARE_ANGEL_FUN( angel_weather      );
DECLARE_ANGEL_FUN( angel_repair       );
DECLARE_ANGEL_FUN( angel_crave        );
DECLARE_ANGEL_FUN( angel_sac          );
DECLARE_ANGEL_FUN( angel_wear         );
DECLARE_ANGEL_FUN( angel_study        );
DECLARE_ANGEL_FUN( angel_learn        );
DECLARE_ANGEL_FUN( angel_recall       );
DECLARE_ANGEL_FUN( angel_recallset    );
DECLARE_ANGEL_FUN( angel_split        );
DECLARE_ANGEL_FUN( angel_train        );
DECLARE_ANGEL_FUN( angel_forsake      );
DECLARE_ANGEL_FUN( angel_enemy        );
DECLARE_ANGEL_FUN( angel_blind        );
DECLARE_ANGEL_FUN( angel_poison       );
DECLARE_ANGEL_FUN( angel_board        );
DECLARE_ANGEL_FUN( angel_firman       );
DECLARE_ANGEL_FUN( angel_money        );
DECLARE_ANGEL_FUN( angel_fighting_1   );
DECLARE_ANGEL_FUN( angel_fighting_2   );
DECLARE_ANGEL_FUN( angel_cname        );
DECLARE_ANGEL_FUN( angel_nothing      );
DECLARE_ANGEL_FUN( angel_reboot       );
DECLARE_ANGEL_FUN( angel_message      );
DECLARE_ANGEL_FUN( angel_config_flee  );
DECLARE_ANGEL_FUN( angel_exact        );
DECLARE_ANGEL_FUN( angel_mobile       );
DECLARE_ANGEL_FUN( angel_note         );
DECLARE_ANGEL_FUN( angel_leftskill    );
DECLARE_ANGEL_FUN( angel_north        );
DECLARE_ANGEL_FUN( angel_south        );
DECLARE_ANGEL_FUN( angel_east         );
DECLARE_ANGEL_FUN( angel_west         );
DECLARE_ANGEL_FUN( angel_up           );
DECLARE_ANGEL_FUN( angel_down         );
DECLARE_ANGEL_FUN( angel_enter        );
DECLARE_ANGEL_FUN( angel_out          );
DECLARE_ANGEL_FUN( angel_spy          );
DECLARE_ANGEL_FUN( angel_cast         );
DECLARE_ANGEL_FUN( angel_ply          );
DECLARE_ANGEL_FUN( angel_exits        );
DECLARE_ANGEL_FUN( angel_search       );
DECLARE_ANGEL_FUN( angel_inventory    );
DECLARE_ANGEL_FUN( angel_fight_join   );
DECLARE_ANGEL_FUN( angel_fight_bet    );

#if     defined(_AIX)
char *  crypt           args( ( const char *, const char * ) );
#endif

#if     defined(apollo)
int     atoi            args( ( const char * ) );
void *  calloc          args( ( unsigned, size_t ) );
char *  crypt           args( ( const char *, const char * ) );
#endif

#if     defined(hpux)
char *  crypt           args( ( const char *, const char * ) );
#endif

#if     defined(linux)
char *  crypt           args( ( const char *, const char * ) );
#endif

#if     defined(macintosh)
#define NOCRYPT
#if     defined(unix)
#undef  unix
#endif
#endif

#if     defined(MIPS_OS)
char *  crypt           args( ( const char *, const char * ) );
#endif

#if     defined(MSDOS)
#define NOCRYPT
#if     defined(unix)
#undef  unix
#endif
#endif

#if     defined(NeXT)
char *  crypt           args( ( const char *, const char * ) );
#endif

#if     defined(sequent)
char *  crypt           args( ( const char *, const char * ) );
int     fclose          args( ( FILE * ) );
int     fprintf         args( ( FILE *, const char *, ... ) );
int     fread           args( ( void *, int, int, FILE * ) );
int     fseek           args( ( FILE *, long, int ) );
void    perror          args( ( const char * ) );
int     ungetc          args( ( int, FILE * ) );
#endif

#if     defined(sun)
char *  crypt           args( ( const char *, const char * ) );
int     fclose          args( ( FILE * ) );
int     fprintf         args( ( FILE *, const char *, ... ) );
int     fseek           args( ( FILE *, long, int ) );
void    perror          args( ( const char * ) );
int     ungetc          args( ( int , FILE * ) );
#endif

#if     defined(ultrix)
char *  crypt           args( ( const char *, const char * ) );
#endif

#if     defined(NOCRYPT)
#define crypt(s1, s2)   (s1)
#endif

#define INI_FILE        "merc.ini"
#define SHUTDOWN_FILE   "shutdown.txt"

#define CD      CHAR_DATA
#define MID     MOB_INDEX_DATA
#define OD      OBJ_DATA
#define OID     OBJ_INDEX_DATA
#define RID     ROOM_INDEX_DATA
#define SF      SPEC_FUN
#define DD      DESCRIPTOR_DATA
#define SD      SECTOR_DATA
#define OTD     OBJ_TYPE_DATA

/* act_comm.c */
void    add_follower    args( ( CD *, CD * ) );
void    stop_follower   args( ( CD * ) );
void    die_follower    args( ( CD * ) );
bool    is_same_group   args( ( CD *, CD * ) );
bool    is_note_to      args( ( CD *, NOTE_DATA * ) );
bool    can_quit        args( ( CD *, bool ) );
bool    check_password  args( ( const char * ) );
bool    check_email_address args( ( char * ) );
bool    write_to_descriptor args( ( DD *, char *, int ) );
void    talk_channel_2  args( ( char * , int , const char * ) );
void    extract_failcode args( ( CD * ) );
void    show_failcode    args( ( CD * ) );
void    notes_update     args( ( void ) );
void    immortal_on_line args( ( CD * ) );
void    immortal_off_line args( ( CD * ) );
bool    write_note       args( ( NOTE_DATA * ) );
extern  void talk_channel args( ( CD *, char *, int, const char * ) );
void    merc_exit        args( ( int ) );

/* act_info.c */
char *  check_damage            args( ( OD * ) );
bool    check_blind             args( ( CD * ) );
void    set_title               args( ( CD *, char * ) );
char *  return_percentage       args( ( CD *, int, int, int ) );
size_t  get_usernum             args( ( void ) );
size_t  get_attemptnum          args( ( void ) );
size_t  get_total_user          args( ( void ) );
char *  get_worktime_string     args( ( void ) );
char *  date_string             args( ( int ) );
char *  time_format             args( ( time_t , const char * ) );
char *  time_string             args( ( TIMEVAL * ) );
char *  showtime                args( ( int ) );

/* act_move.c */
bool            move_char          args( ( CD *, int ) );
size_t          learn_count        args( ( CD * ) );
size_t          learn_rating_count args( ( CD *, int ) );
int             find_door          args( ( CD * , char * ) );
EXIT_DATA *     create_exit        args( ( int, int, int ) );
void            close_exit         args( ( int, int ) );

/* act_obj.c */
CD *    find_keeper     args( ( CD * , int ) );
void    message_driver  args( ( CD *, OD *, int ) );
size_t  add_firman      args( ( CD *, int ) );
bool    can_store       args( ( OD * ) );
bool    wear_obj        args( ( CD * , OD * , bool, bool ) );

/* act_wiz.c */
RID *   find_location   args( ( CD *, char * ) );

/* comm.c */
bool    check_class_attr args( ( CD * ) );
void    close_socket    args( ( DD * ) );
void    write_to_buffer args( ( DD *, const char *, int ) );
void    send_to_all_char args( ( const char * ) );
void    send_to_club    args( ( const char *, CLUB_DATA * ) );
void    send_to_char    args( ( const char *, CD * ) );
void    sendmsg_to_room args( ( const char *, CD * ) );
void    sendmsg_to_someroom args( ( const char *, RID * ) );
void    show_string     args( ( DD *, char * ) );
void    act             args( ( const char *, CD *,
                            const void *, const void *, int ) );

void    sact            args( ( CD *, CD *, OD *, char * ) );
bool    check_parse_name        args( ( char * ) );
void    cease_stack     args( ( DD * ) );
bool    set_descriptor_stack args( ( DD * ) );
void    process_stack   args( ( DD * ) );

/* db.c */
int     mprog_name_to_type      args( ( char * ) );
void    boot_db         args( ( void ) );
CD *    create_mobile   args( ( MID *, int ) );
OD *    create_object   args( ( OID *, int ) );
void    clear_char      args( ( CD * ) );
void    free_char       args( ( CD * ) );
bool    symbol_lookup   args( ( char *, int * ) );
char *  symbol_string   args( ( int, char * ) );
char *  get_extra_descr args( ( const char *, EXTRA_DESCR_DATA * ) );
MID  *  get_mob_index   args( ( int ) );
int     get_mob_count   args( ( int ) );
SD   *  get_sector_index args( ( int ) );

OID  *  get_obj_index   args( ( int ) );
RID  *  get_room_index  args( ( int ) );
AREA_DATA * get_room_area args( ( int ) );
RID  *  random_room     args( ( AREA_DATA * ) );
void    smash_tilde     args( ( char * ) );
void    smash_point     args( ( char * ) );
void    tail_chain      args( ( void ) );
void    reset_area      args( ( AREA_DATA * ) );
void    mine_reset_area args( ( AREA_DATA * ) );

extern FILE *           fpReserve;

/* mem.c */
char *  str_dup         args( ( const char * ) );
void    memory_ini      args( ( void ) );
void *  alloc_string    args( ( int ) );
void *  alloc_struct    args( ( int ) );
void    free_struct     args( ( void *, int ) );
void    free_string     args( ( char * ) );

/* random.c */
void    init_mm         args( ( void ) );
int     number_door     args( ( void ) );
int     number_fuzzy    args( ( int ) );
int     number_range    args( ( int, int ) );
int     number_percent  args( ( void ) );
int     number_bits     args( ( int ) );
int     dice            args( ( int, int ) );
int     variation       args( ( int, int ) );

/* fight.c */
void    violence_update args( ( void ) );
void    striking        args( ( CD *, CD *, int ) );
void    damage          args( ( CD *, CD *, int, int, int ) );
void    update_pos      args( ( CD * ) );
void    stop_fighting   args( ( CD *, bool ) );
void    death_cry       args( ( CD * ) );
bool    can_fight       args( ( CD *, CD * ) );
int     get_hitroll     args( ( CD * ) );
int     get_damroll     args( ( CD * ) );
int     get_ac          args( ( CD * ) );
void    check_killer    args( ( CD * , CD * ) );
void    set_fighting    args( ( CD * , CD * ) );
bool    raw_kill        args( ( CD * , CD *, bool ) );

/* bit.c */
const char * adeptation_name        args( ( int ) );
const char * direction_name         args( ( int ) );
const char * direction_ename        args( ( int ) );
const char * get_homename           args( ( int ) );
const char * tcp_status             args( ( int ) );
const char * bounty_type            args( ( int ) );
const char * bounty_unit            args( ( int ) );
const char * status_message         args( ( CD * ) );
const char * exit_status            args( ( EXIT_DATA * ) );
char     *  sex_name                args( ( int ) );
char     *  obj_cast_location       args( ( int ) );
char     *  effect_type_name        args( ( int ) );
char     *  ship_status             args( ( SHIP_DATA * ) );
char     *  restrict_value          args( ( RESTRICT_DATA *, CD * ) );
char     *  message_type            args( ( int ) );
char     *  player_status           args( ( int ) );
char     *  skill_type              args( ( int ) );
char     *  club_status             args( ( CLUB_DATA * ) );
char     *  item_type_name          args( ( OID * ) );
char     *  skill_rating            args( ( int ) );
char     *  item_kind_name          args( ( int ) );
char     *  affect_loc_name         args( ( int ) );
char     *  area_bit_name           args( ( AREA_DATA * ) );
char     *  sector_flag_name        args( ( SECTOR_DATA * ) );
char     *  shop_type_name          args( ( int ) );
char     *  skill_affect            args( ( int ) );
char     *  extra_bit_name          args( ( OD *  ) );
char     *  extra_bit_name_2        args( ( OID * ) );
char     *  wear_bit_name           args( ( int ) );
char     *  ban_bit_name            args( ( int ) );
char     *  xnames_bit_name         args( ( int ) );
char     *  attack_situs            args( ( int ) );
char     *  wear_location_string    args( ( int ) );
char     *  room_bit_name           args( ( RID * ) );
char     *  weapon_type_name        args( ( int ) );
char     *  container_bit_name      args( ( int ) );
char     *  act_bit_name            args( ( int ) );
char     *  wiz_bit_name            args( ( int ) );
char     *  turn_bit_name           args( ( int ) );
char     *  progtypes_bit_name      args( ( int ) );
char     *  get_obj_value_usage     args( ( int , int * ) );
char     *  object_locate           args( ( OD * , CD * ) );
CD       *  object_owner            args( ( OD * ) );
char *  get_position                args( ( int ) );
char *  get_log                     args( ( int ) );

/* handler.c */
int         get_trust       args( ( CD * ) );
int         get_age         args( ( CD * ) );
int         get_curr_hit    args( ( CD * ) );
int         get_curr_mana   args( ( CD * ) );
int         get_curr_move   args( ( CD * ) );
int         get_curr_str    args( ( CD * ) );
int         get_str_train   args( ( CD * ) );
int         get_curr_int    args( ( CD * ) );
int         get_int_train   args( ( CD * ) );
int         get_curr_wis    args( ( CD * ) );
int         get_wis_train   args( ( CD * ) );
int         get_curr_dex    args( ( CD * ) );
int         get_dex_train   args( ( CD * ) );
int         get_curr_con    args( ( CD * ) );
int         get_con_train   args( ( CD * ) );
int         can_carry_n     args( ( CD * ) );
int         can_carry_w     args( ( CD * ) );
int         get_carry_weight args( ( CD * ) );
void        affect_to_char  args( ( CD *, AFFECT_DATA * ) );
void        affect_remove   args( ( CD *, AFFECT_DATA * ) );
void        affect_release  args( ( CD *, int sn ) );
bool        is_affected     args( ( CD *, int ) );
void        affect_join     args( ( CD *, AFFECT_DATA * ) );
void        char_from_room  args( ( CD * ) );
void        char_to_room    args( ( CD *, RID * ) );
void        gold_to_room    args( ( RID *, int ) );
void        gold_to_char    args( ( CD *, int ) );
void        gold_from_char  args( ( CD *, int ) );
void        fix_gold_weight args( ( CD * ) );
void        obj_to_char     args( ( OD *, CD * ) );
void        obj_from_char   args( ( OD * ) );
int         apply_ac        args( ( OD *, int ) );
OD *        get_eq_char     args( ( CD *, int ) );
void        equip_char      args( ( CD *, OD *, int ) );
void        unequip_char    args( ( CD *, OD *, bool ) );
int         count_obj_list  args( ( OID *, OD * ) );
int         obj_type_room   args( ( CD *, int ) );
int         obj_type_char   args( ( CD *, int ) );
void        obj_from_room   args( ( OD * ) );
void        obj_to_room     args( ( OD *, RID * ) );
void        obj_to_obj      args( ( OD *, OD * ) );
void        obj_from_obj    args( ( OD * ) );
void        extract_obj     args( ( OD * ) );
void        extract_char    args( ( CD *, bool ) );
CD *        get_pc_room     args( ( CD *, const char * ) );
CD *        get_pc_world    args( ( CD *, const char * ) );
CD *        get_char_room   args( ( CD *, char * ) );
CD *        get_char_world  args( ( CD *, char * ) );
int         get_char_order  args( ( CD *, CD * ) );
CD *        get_char_area   args( ( CD *, char * ) );
MID *       get_char_protype args( ( char * ) );
OD *        get_obj_type    args( ( OID * ) );
OD *        get_obj_list    args( ( CD *, char *, OD * ) );
OD *        get_obj_carry   args( ( CD *, char * ) );
OD *        get_obj_wear    args( ( CD *, char * ) );
OD *        get_obj_here    args( ( CD *, char * ) );
OD *        get_obj_world   args( ( CD *, char * ) );
OID *       get_obj_protype args( ( char * ) );
RID *       get_obj_room    args( ( OD * ) );
OD *        create_money    args( ( int ) );
int         get_obj_number  args( ( OD * ) );
int         get_obj_weight  args( ( OD * ) );
bool        room_is_dark    args( ( RID * ) );
bool        room_is_private args( ( RID * ) );
bool        can_see         args( ( CD *, CD * ) );
bool        can_see_mask    args( ( CD *, CD * ) );
bool        can_see_obj     args( ( CD *, OD * ) );
bool        can_drop_obj    args( ( CD *, OD * ) );
bool        can_see_room    args( ( CD * ) );
int         search_cost     args( ( CD * ) );
OD *        get_wield       args( ( CD * ) );
void        light_adjust    args( ( CD *, OD *, bool ) );
int         dimemsion_position  args( ( int *, int , bool ) );
bool        check_repeat    args( ( CD *, const char * ) );
void        delete_chat     args( ( CD * ) );
bool        check_chat_xname args( ( CD *, char * ) );
bool        check_cname_xname args( ( char * ) );
CD *        shape_mobile    args( ( MID *, RID * ) );
void        adjust_carry    args( ( CD * ) );
void        identify_obj    args( ( CD *, OD * ) );
LIQ_DATA  * liq_lookup      args( ( int ) );
bool        check_obj_restrict args( ( CD *, OD *, bool ) );
bool        check_skill_restrict args( ( CD *, SKILL_DATA *, bool ) );
bool        check_skill_cast args( ( CD *, SKILL_DATA *, bool ) );
bool        check_valid_rating args( ( CD *, SKILL_DATA *, bool ) );
time_t      time_at         args( ( char * ) );
bool        on_line         args( ( const char *, CD * ) );
bool        is_keeper       args( ( CD * ) );
bool        biliwick        args( ( CD *, CD * ) );
void        fix_color       args( ( char * ) );
bool        is_dead         args( ( CD * ) );
bool        can_damage      args( ( CD * ) );
bool        can_char_from_room args( ( CD *, bool ) );
bool        check_restrict  args( ( CD *, RESTRICT_DATA *, bool ) );
void        cancel_skill    args( ( CD *, SKILL_DATA * ) );
char      * host_name       args( ( DESCRIPTOR_DATA * ) );
bool        poison_char     args( ( CD *, OD * ) );
int         get_adeptation  args( ( SKILL_DATA *, MID * ) );
void        calculate_armor args( ( CD * ) );
void        check_home      args( ( CD * ) );
AREA_DATA * get_home        args( ( CD * ) );
RID       * get_hometown    args( ( CD * ) );
void        set_variable    args( ( const char *, int, int ) );
int         check_variable  args( ( const char *, int ) );
bool        char_autofood   args( ( CD * ) );
bool        char_autodrink  args( ( CD * ) );
size_t      get_learnable   args( ( CD * ) );
const char * numberize      args( ( int, char * ) );
bool        is_server       args( ( const char * ) );
int         personal_asset  args( ( CD * ) );
bool        over_scale      args( ( CD * ) );
char *      return_array    args( ( ARRAY_DATA [], int, int ) );
char *      obj_name        args( ( CD *, OD * ) );
char *      mob_name        args( ( CD *, CD * ) );
char *      mob_index_name  args( ( CD *, MID * ) );
char *      binary_bit      args( ( int ) );
void        clear_return    args( ( char *, char * ) );
bool        can_leaveroom   args( ( CD *, bool ) );
bool        is_exist        args( ( const char * ) );
SITUS_DATA * situs_lookup   args( ( int ) );
void        affect_modify   args( ( CHAR_DATA * , AFFECT_DATA * , bool ) );
char       * smash_act_keyword args( ( char * buf, char * key ) );

/* interp.c */
void    interpret       args( ( CD *, char * ) );
int     number_argument args( ( char *, char * ) );
char *  one_argument    args( ( char *, char * ) );
char *  one_statement   args( ( char *, char * ) );

/* magic.c */
SKILL_DATA      * get_skill     args( ( int ) );
SKILL_DATA      * skill_isname  args( ( const char * ) );
bool    saves_spell     args( ( int, CD * ) );
void    mprog_wordlist_check    args( ( char *, CD *, CD *, OD *, void *, int ) );
void    mprog_percent_check     args( ( CD *, CD *, OD *, void *, int ) );
void    mprog_act_trigger       args( ( char *, CD *, CD *, OD *, void * ) );
void    mprog_bribe_trigger     args( ( CD *, CD *, int ) );
void    mprog_entry_trigger     args( ( CD * ) );
void    mprog_give_trigger args( ( CD *, CD *, OD * ) );
void    mprog_greet_trigger     args( ( CD * ) );
void    mprog_fight_trigger     args( ( CD *, CD * ) );
void    mprog_hitprcnt_trigger  args( ( CD *, CD * ) );
void    mprog_death_trigger     args( ( CD *, CD * ) );
void    mprog_random_trigger    args( ( CD * ) );
void    mprog_speech_trigger    args( ( char * , CD * ) );

/* save.c */
void    save_char_obj      args( ( CD * , int ) );
int     load_char_obj      args( ( DD *, char * ) );
void    check_null_object  args( ( CD * ) );
char *  initial            args( ( const char * ) );
char *  file_name          args( ( const char *, int ) );
void    save_club          args( ( void ) );
void    set_file_mode      args( ( const char * ) );
void    fwrite_failcode    args( ( CD *, const char * ) );
void    char_filesize      args( ( CD * ) );
void    autosave_update    args( ( void ) );
bool    excess_filequota   args( ( CD * ) );
void    player_log         args( ( CD *, const char * ) );
void    fprint_string      args( ( FILE *, char *, const char * ) );
void    fprint_word        args( ( FILE *, char *, const char * ) );
void    fprint_number      args( ( FILE *, char *, int ) );
bool    affect_login_check args( ( CD * ) );
void    affect_recount     args( ( CHAR_DATA * ) );

/* special.c */
SF *    spec_lookup     args( ( const char * ) );

/* update.c */
void    advance_level   args( ( CD * ) );
void    gain_exp        args( ( CD *, int ) );
void    gain_condition  args( ( CD *, int, int ) );
void    update_handler  args( ( void ) );
void    area_update     args( ( void ) );
void    ship_update     args( ( void ) );
void    foggy_update    args( ( int ) );

/* auction.c */
void    init_auction    args( ( void ) );
void    auction_update  args( ( void ) );

/* alias.c */
extern  int     max_alias;
extern  int     alias_repeat;

void    extract_alias   args( ( CD * ) );
int     get_alias_count args( ( CD * ) );
void    translate_alias args( ( DD * , char * ) );

/* deposit.c */
void    obj_to_char_deposit   args( ( OD * , CD * ) );
void    obj_from_char_deposit args( ( OD * ) );
OD *    get_obj_deposit       args( ( CD * , char * ) );
void    obj_to_obj_dep        args( ( OD * , OD * ) );
size_t  deposit_count         args( ( CD * ) );
void    deposit_update        args( ( void ) );

/* string.c */
bool    is_name                 args( ( const char *, char * ) );
bool    is_fullname             args( ( const char *, char * ) );
bool    is_number               args( ( char *arg ) );
char *  test_number             args( ( char * , int * ) );
size_t  str_len                 args( ( const char * ) );
size_t  ansi_str_len            args( ( char * ) );
char *  str_cat                 args( ( char * , const char * ) );
char *  str_cpy                 args( ( char * , const char * ) );
char *  str_ncpy                args( ( char * , const char *, size_t ) );
char *  str_str                 args( ( char *, char * ) );
bool    str_cmp                 args( ( const char * , const char * ) );
bool    str_scmp                args( ( const char * , const char * , bool * ) );
bool    str_prefix              args( ( const char * , const char * ) );
bool    str_infix               args( ( const char * , const char * ) );
bool    str_suffix              args( ( const char * , const char * ) );
char *  normalize               args( ( const char * ) );
void    smash_char              args( ( char *, char ) );
char *  str_space               args( ( char *, size_t ) );
char *  chinese_number          args( ( int , char * ) );

/* load.c */
AREA_DATA       *       load_area        args( ( char * ) );
MOB_INDEX_DATA  *       load_mobiles     args( ( const char * ) );
ROOM_INDEX_DATA *       load_room        args( ( const char * ) );
OBJ_INDEX_DATA  *       load_object      args( ( const char * ) );
SHOP_DATA       *       load_shop        args( ( const char * ) );
void                    load_resets      args( ( char * , AREA_DATA * ) );
MINERAL_DATA    *       load_mineral     args( ( const char * ) );
void                    load_mail        args( ( const char * ) );
void                    load_specials    args( ( FILE * ) );
void                    load_help        args( ( const char * ) );
void                    load_sector      args( ( const char * ) );
void                    load_class       args( ( const char * ) );
void                    load_liq         args( ( const char * ) );
void                    load_joke        args( ( const char * ) );
void                    load_social      args( ( const char * , const char * ) );
void                    load_angel       args( ( const char * ) );
void                    load_symbol      args( ( const char * ) );
void                    load_greeting    args( ( const char * ) );
void                    load_motd        args( ( const char * ) );
void                    load_welcome     args( ( const char * ) );
void                    load_welcome_immortal args( ( const char * ) );
void                    load_site        args( ( const char * ) );
void                    load_server      args( ( const char * ) );
void                    load_xnames      args( ( const char * ) );
void                    load_stock       args( ( const char * ) );
void                    load_database    args( ( const char * ) );
void                    load_promotion   args( ( const char * ) );
void                    load_hero        args( ( const char * ) );
void                    load_skill       args( ( const char * , const char * ) );
void                    load_instrument  args( ( const char * , const char * ) );
void                    load_bus         args( ( const char * ) );
void                    load_ship        args( ( const char * ) );
void                    load_quest       args( ( const char * ) );
void                    load_question    args( ( const char * ) );
void                    load_event       args( ( const char * ) );
void                    load_gift        args( ( const char * ) );
void                    load_date        args( ( const char * ) );
void                    load_bounty      args( ( const char * ) );
void                    load_immlist     args( ( const char * ) );
void                    load_donate      args( ( const char * ) );
void                    load_club        args( ( const char * ) );
void                    load_internal    args( ( const char * ) );
void                    load_sale        args( ( const char * ) );
void                    load_situs       args( ( const char * ) );
void                    set_vote_default args( ( VOTE_DATA * ) );

/* ansi.c */
void    filter_ansi     args( ( char * ) );
void    tablize         args( ( int, char *, int , char *, int ) );
char *  ansi_code       args( ( char *, char *, int * ) );
char *  format_string   args( ( const char *, int ) );
void    ansi_transcribe args( ( char *, char * ) );

/* check.c */
extern  int     serial_time;
extern  int     serial_loop;

void initial_serial_time args( ( void ) );
void set_serial          args( ( SERIAL_DATA * ) );
void check_serial_number args( ( CD * ) );

/* buffer.c */
bool   buffer_full     args( ( void ) );
bool   stack_full      args( ( void ) );
void   notify          args( ( int, int, const char *, ... ) );
void   clear_buffer    args( ( void ) );
void   clear_stack     args( ( void ) );
void   send_to_buffer  args( ( const char *, ... ) );
void   send_to_stack   args( ( const char *, ... ) );
void   print_buffer    args( ( CD * ) );
void   print_stack     args( ( CD * ) );
char * return_stack    args( ( void ) );
void   print_to_char   args( ( CD * , const char *, ... ) );
void   mudlog          args( ( int , const char *, ... ) );
void   output_buffer   args( ( void ) );

/* gamble.c */
void    initial_gamble args( ( void ) );
void    gamble_update  args( ( void ) );
void    extract_gamble args( ( CD * ) );

/* system.c */
char *          get_loading       args( ( void ) );
unsigned long   get_uptime        args( ( void ) );
bool            check_limit_ip    args( ( DD * ) );
bool            open_loading_file args( ( float *, float *, float * ) );
void            loading_warning   args( ( CD * ) );

/* enable.c */
extern          int     max_enable;
void            extract_an_enable  args( ( CD *, SKILL_DATA * ) );
void            set_enable         args( ( CD *, SKILL_DATA * ) );
ENABLE_DATA *   enable_repeat      args( ( ENABLE_DATA * , SKILL_DATA * ) );
void            extract_enable     args( ( CD * ) );
size_t          enable_count       args( ( CD * ) );
ENABLE_DATA *   exert_fight_enable args( ( CD *, CD *, bool ) );
ENABLE_DATA *   exert_dodge_enable args( ( CD *, CD *, bool ) );
bool            check_skill_enable args( ( CD *, SKILL_DATA * ) );

/* socket.c */
char  * get_user_name           args( ( int , SOCKET_DATA * ) );
void    check_multi_update      args( ( void ) );
char *  address_lookup          args( ( const char * ) );
void    set_address_data        args( ( const char * , const char * ) );
char *  address_mode            args( ( const char *, int mode ) );

/* signal.c */
void    signal_setup            args( ( void ) );
void    push_function           args( ( char * ) );
void    pop_function            args( ( void ) );
void    close_all_socket        args( ( void ) );
bool    verify_char             args( ( CD * ) );
bool    verify_obj              args( ( OD * ) );
bool    verify_desc             args( ( DD * ) );

/* file.c */
FILE  * FOPEN                   args( ( const char *, const char * ) );
void    FCLOSE                  args( ( FILE * ) );
void    open_area_directory     args( ( void ) );
void    fill_path               args( ( char * ) );
bool    file_ext                args( ( const char * , const char * ) );
bool    is_regular              args( ( const char *  ) );
bool    is_directory            args( ( const char * ) );

/* limit.c */
bool check_hand                 args( ( CD *, int ) );
bool check_continue             args( ( CD *, CD * ) );
int  check_skill_rating         args( ( char * ) );
bool check_can_kill             args( ( CD *, CD * ) );
bool check_same_weapon          args( ( CD *, bool ) );
bool check_bow_content          args( ( CD * ) );

/* ini.c */
void    read_ini                 args( ( const char * ) );
void    default_file             args( ( void ) );
void    adjust_filename          args( ( void ) );

/* lotto.c */
extern  int     lotto_paper;
extern  int     LottoFirst;
extern  int     LottoSecond;
extern  int     LottoThird;
extern  int     LottoForth;
void    lotto_generate           args( ( int ) );

void    lotto_update             args( ( void ) );

/* function.c */
void *  command_function_name    args( ( const char * ) );
void *  skill_function_name      args( ( const char * ) );
void *  check_function_name      args( ( const char * ) );
void *  check_routine_name       args( ( const char * ) );
void *  angel_function           args( ( const char * ) );
EVENT_FUN *     event_function   args( ( const char * ) );

/* verify.c */
int     verify_file              args( ( DD * ) );

/* ipc.c */
extern  int                     merc_ipc;
extern  int                     merc_ipc_2;
extern  int                     shmid;

void    init_share_memory       args( ( int, int ) );
void    delete_share_memory     args( ( int ) );
void    inc_share_memory_timer  args( ( void ) );
void    update_share_memory     args( ( DD * ) );
void    handle_share_memory     args( ( void ) );
int     get_free_share_memory   args( ( DD *, int ) );
void    set_share_memory_text   args( ( int , char * ) );
void    clean_share_memory_address args( ( int ) );

/* mount.c */
bool    unmount_char            args( ( CD *, CD * ) );
CD    * get_mount               args( ( CD * ) );
CD    * get_rider               args( ( CD * ) );

/* bus.c */
extern  char *                  company_name;
extern  char *                  platform_descr;
extern  char *                  loge_descr;
extern  char *                  platform_short;
extern  char *                  loge_short;

bool    check_station           args( ( RID * ) );
RID   * create_platform         args( ( int, char *, AREA_DATA * ) );
RID   * create_loge             args( ( int, char *, AREA_DATA * ) );
int     save_room               args( ( RID * ) );
bool    link_path               args( ( RID *, RID * ) );
void    bus_update              args( ( void ) );
BUS_DATA * is_station           args( ( RID * ) );
BUS_DATA * is_platform          args( ( RID * ) );
BUS_DATA * is_loge              args( ( RID * ) );

/* body.c */
int     random_situs            args( ( CD *, int ) );
void    init_max_situs          args( ( void ) );
int     damage_situs            args( ( CD *, int, int ) );
bool    is_armor                args( ( OD * ) );

extern int eq_max_capcity;
extern int eq_min_capcity;

/* mob_prog.c */
void    mprog_driver            args( ( char *, CD *, CD *, OD *, void * ) );

/* net.c */
extern  int                     internal_port;

NET_DATA * get_net_data         args( ( char * , int ) );
bool       valid_socket         args( ( NET_DATA *, int ) );
void       launch_internal      args( ( int ) );
void       close_net_socket     args( ( NET_DATA *, int ) );
bool       init_client          args( ( NET_DATA * ) );

/* ff7.c */
bool       is_magic_stone       args( ( OD * ) );
bool       can_cachet           args( ( OD * ) );
int        have_stone           args( ( CD * ) );
int        obj_stone            args( ( OD * ) );
void       check_wear_stone     args( ( CD *, int ) );
void       check_remove_stone   args( ( CD *, int ) );
void       gain_stone_exp       args( ( CD *, int ) );

/* club.c */
bool       char_from_club       args( ( char *, CLUB_DATA *, int ) );
bool       name_from_club       args( ( const char *, bool ) );
void       club_sort            args( ( CLUB_DATA * ) );
extern int club_room;
extern int max_club;

bool        char_to_club        args( ( char *, CLUB_DATA *, int ) );
bool        is_same_club        args( ( CD *, CD * ) );
CD *        is_online           args( ( const char * ) );
char *      club_name           args( ( CD * ) );
char *      is_club_class       args( ( CD * ) );
char *      club_class          args( ( CD *, CLUB_DATA * ) );
void        char_attach_club    args( ( CD * ) );
RID  *      club_recall         args( ( CD * ) );
RID *       club_location       args( ( CLUB_DATA * ) );
size_t      club_count          args( ( void ) );
size_t      club_notes          args( ( CD * ) );
size_t      club_unread         args( ( CD * ) );
CLUB_DATA * clubname_lookup args( ( const char *, int ) );

/* skill.c */
void        driver_flee         args( ( SKILL_DATA *, CD * ) );
int         get_practice        args( ( CD *, int ) );
bool        effect_driver       args( ( CD *, CD *, EFFECT_DATA * ) );
bool        gain_skill_exp      args( ( CD * , int , bool ) );

/* class.c */
char *      class_name          args( ( CLASS_DATA *, bool ) );
bool        is_same_class       args( ( CD *, CD * ) );
int         get_skill_class_level args( ( SKILL_DATA *, CLASS_DATA * ) );
CLASS_DATA * is_class           args( ( char *, bool ) );
CLASS_DATA * class_lookup       args( ( int ) );

/* job.c */
JOB_FUN * job_lookup   args( ( const char * ) );

/* fs.c */
char * pw_lookup        args( ( int ) );
char * gr_lookup        args( ( int ) );
char * perm_of_file     args( ( mode_t ) );

/* enroll.c */
void set_enroll        args( ( CD *, CD * ) );
bool is_enroll         args( ( CD *, CD * ) );
void extract_enroll    args( ( CD *, const char * ) );
void mprog_enroll_trigger args( ( CD * ) );

/* stock.c */
void stock_update      args( ( void ) );
void save_stock        args( ( void ) );
int  stock_value       args( ( CD * ) );
void bote_stock        args( ( CD * ) );
void set_stock_value   args( ( int, int ) );

extern int	       StockShock;
extern int             StockTax;
extern int             StockBote;
extern int             StockQuota;
extern int             StockRelease;

/* quest.c */
void set_quest         args( ( CD *, char * ) );
void rem_quest         args( ( CD *, const char * ) );
bool is_quest          args( ( CD *, char *, char * ) );

/* enemy.c */
void set_enemy         args( ( CD *, char * ) );
void rem_enemy         args( ( CD *, const char * ) );
bool is_enemy          args( ( CD *, char *, char * ) );
bool check_enemy       args( ( CD *, const char * ) );
QUEST_INFO * quest_lookup args( ( const char * ) );

/* wizard.c */
IMMLIST_DATA *  imm_lookup      args( ( const char * ) );
void            system_cleanup  args( ( void ) );
bool            jail_someone    args( ( CD *, CD *, int, bool ) );

/* friend.c */
extern  int             max_friend;
void    extract_friend  args( ( CD * ) );
void    friend_msg      args( ( CD * ) );
size_t  friend_count    args( ( CD * ) );
FRIEND_DATA * is_friend args( ( CD *, const char * ) );

/* badfriend.c */
void    extract_badfriend       args( ( CD * ) );
size_t  badfriend_count         args( ( CD * ) );
FRIEND_DATA * is_badfriend      args( ( CD *, const char * ) );

/* wedding.c */
void    check_mate      args( ( CD * ) );
char *  mate_name       args( ( CD * ) );
bool    is_couple       args( ( CD *, CD * ) );

/* spirit.c */
CD *    jade_to_spirit  args( ( CD *, OD * ) );
void    spirit_from_char args( ( CD * ) );

/* board.c */
extern  int     max_board;

void            board_update            args( ( void ) );
void            open_board_directory    args( ( void ) );
int             post_count              args( ( BOARD_DATA *, int type ) );
void            show_board_title        args( ( CD *, BOARD_DATA * ) );
BOARD_DATA *    board_lookup            args( ( int ) );
int             unread_post             args( ( CD *, BOARD_DATA * ) );

/* hero.c */
size_t          hero_count              args( ( void ) );
void            hero_add                args( ( CD * ) );
void            add_hero_bonus          args( ( CD * ) );
void            update_hero             args( ( void ) );
void            check_hero              args( ( char * ) );

/* hire.c */
void            all_mercenary_from_char args( ( CD * ) );
void            mercenary_from_char     args( ( CD *, CD * ) );
int             hire_cost               args( ( MID *, SHOP_DATA *, int ) );

/* angel.c */
void            angel_update            args( ( void ) );

/* arena.c */
extern int      pk_level;
extern int      pk_age;
extern int      pk_limit;
extern int      PkContrastLevel;
extern int      MinPKGold;
extern int      MaxPKGold;
extern int      MaxPKTotal;

void            fight_update            args( ( void ) );
void            init_fight              args( ( FIGHT_DATA * ) );
bool            is_pk                   args( ( CD * ) );
void            send_ante               args( ( int ) );
void            stop_pk                 args( ( void * ) );
void            return_ante             args( ( void ) );

/* top.c */
void            top_update              args( ( void ) );
bool            get_exec_time           args( ( int, int *, int * ) );
int             calc_loading            args( ( int, int, int ) );

extern TOP_DATA * NowTop;

/* wanted.c */
void            check_wanted            args( ( CD *, CD * ) );
extern int      WantedThreshold;

/* vote.c */
void            load_vote               args( ( const char * ) );
int             vote_count              args( ( CD * ) );
bool            vote_timeup             args( ( VOTE_DATA * ) );
void            vote_to_note            args( ( VOTE_DATA * ) );

/* edit.c */
void            accept_edit             args( ( CD *, char * ) );
void            show_editor             args( ( CD * ) );
bool            is_edit                 args( ( DD * ) );

/* automap.c */
void            set_map                 args( ( CD *, int ) );
void            clear_trace             args( ( CD *, bool ) );
void            stop_automap            args( ( DD * ) );

/* chance.c */
void            generate_ticket         args( ( int ) );
void            dice_ticket             args( ( void ) );

extern int              TicketTotal;
extern int              TicketCost;
extern int              TicketReset;
extern int              ticketreset;
extern ORDER_DATA *     order_list;

/* gift.c */
void            set_gift_time           args( ( GIFT_DATA * ) );
void            check_gift              args( ( void ) );
void            gift_update             args( ( void ) );
bool            is_gift_stamp           args( ( int ) );
void            set_gift_stamp          args( ( CD *, int ) );
bool            check_gift_stamp        args( ( CD *, int ) );

/* event.c */
void            event_update            args( ( void ) );

/* bounty.c */
void            bounty_update           args( ( void ) );
void            check_bounty            args( ( CD *, CD * ) );
extern int      NowBounty;
extern int      BountyLimit;

/* question.c */
void            question_update         args( ( void ) );
void            send_answer             args( ( CD *, CD * ) );
extern int      PlyQuota;
extern int      PlyPenalty;
extern int      QuestionTimer;
extern int      QuestionFalse;
extern int      QuestionAlarm;
extern int      QuestionLock;

/* scan.c */
void            clean_scan              args( ( void ) );
void            scan_room               args( ( int, int, int ) );
void            print_scan              args( ( CD *, OD * ) );
extern SCAN_DATA                        ScanData[];
extern int                              ScanPointer;

/* make.c */
bool            write_new_room          args( ( RID * ) );
RID           * create_new_room         args( ( void ) );
bool            is_owner                args( ( CD *, RID *, bool ) );
CD            * owner_name              args( ( RID * ) );
void            room_update             args( ( void ) );
int             count_home_number       args( ( CD * ) );

/* database.c */
void            set_database            args( ( CD * ) );
void            delete_database         args( ( CD * ) );
void            erase_database          args( ( const char * ) );
DATABASE_DATA * database_lookup         args( ( int, int ) );
DATABASE_DATA * name2database           args( ( const char * ) );
BASIC_DATA    * load_basic              args( ( const char * ) );
BASIC_DATA    * serial2basic            args( ( SERIAL_DATA * ) );
BASIC_DATA    * name2basic              args( ( const char * ) );
char          * fullname                args( ( SERIAL_DATA * ) );
size_t          database_count          args( ( int ) );

/* object.c */
char *          check_obj_value args( ( int, int, int, int *, bool ) );

/* fcntl.c */
FILE_DATA  *    f_open          args( ( const char *, const char * ) );
bool            f_close         args( ( FILE_DATA * ) );
const char *    error_string    args( ( FILE_DATA * ) );
char            fread_letter    args( ( FILE_DATA * ) );
int             fread_number    args( ( FILE_DATA * ) );
char       *    fread_string    args( ( FILE_DATA * ) );
bool            if_eof          args( ( FILE_DATA * ) );
bool            fread_if_eol    args( ( FILE_DATA * ) );
void            fread_to_eol    args( ( FILE_DATA * ) );
char       *    fread_alpha     args( ( FILE_DATA * ) );
char       *    fread_word      args( ( FILE_DATA * ) );
char       *    map_file        args( ( FILE_DATA * ) );
void            fw_string       args( ( FILE_DATA *, char *, const char * ) );
void            fw_word         args( ( FILE_DATA *, char *, const char * ) );
void            fw_number       args( ( FILE_DATA *, char *, int ) );
void            fw_out          args( ( FILE_DATA *, const char * ) );
extern unsigned int             FileError;

/* reload.c */
void            clear_reload_status     args( ( void ) );
extern int      FileLine;
extern int      FileWord;

/* free.c */
void            free_obj_index   args( ( OID              * ) );
void            free_mob_index   args( ( MID              * ) );
void            free_affect      args( ( AFFECT_DATA      * ) );
void            free_restrict    args( ( RESTRICT_DATA    * ) );
void            free_message     args( ( MESSAGE_DATA     * ) );
void            free_extra_descr args( ( EXTRA_DESCR_DATA * ) );
void            free_job         args( ( JOB_DATA         * ) );
void            free_teach       args( ( TEACH_DATA       * ) );
void            free_enable      args( ( ENABLE_DATA      * ) );
void            free_mob_program args( ( MPROG_DATA       * ) );
void            free_basic       args( ( BASIC_DATA       * ) );

/* variable.c */
extern  int                     RoomJailVnum;
extern  ROOM_INDEX_DATA *       RoomJail;
extern  int                     RoomLimboVnum;
extern  ROOM_INDEX_DATA *       RoomLimbo;
extern  int                     RoomDeadVnum;
extern  ROOM_INDEX_DATA *       RoomDead;
extern  int                     RoomRecallVnum;
extern  ROOM_INDEX_DATA *       RoomRecall;
extern  int                     RoomSchoolVnum;
extern  ROOM_INDEX_DATA *       RoomSchool;
extern  int                     RoomFailVnum;
extern  ROOM_INDEX_DATA *       RoomFail;
extern  int                     RoomChatVnum;
extern  ROOM_INDEX_DATA *       RoomChat;
extern  int                     RoomCorpseVnum;
extern  ROOM_INDEX_DATA *       RoomCorpse;
extern  AREA_DATA *             DefaultArea;
extern  int                     MobVampireVnum;
extern  MOB_INDEX_DATA *        MobVampire;
extern  int                     MobPracticeVnum;
extern  MOB_INDEX_DATA *        MobPractice;
extern  int                     ObjProtypeVnum;
extern  OBJ_INDEX_DATA *        ObjProtype;
extern  int                     ObjMoneyVnum;
extern  OBJ_INDEX_DATA *        ObjMoney;
extern  int                     ObjMoneySomeVnum;
extern  OBJ_INDEX_DATA *        ObjMoneySome;
extern  int                     ObjCorpseNPCVnum;
extern  OBJ_INDEX_DATA *        ObjCorpseNPC;
extern  int                     ObjCorpsePCVnum;
extern  OBJ_INDEX_DATA *        ObjCorpsePC;
extern  int                     ObjHeadVnum;
extern  OBJ_INDEX_DATA *        ObjHead;
extern  int                     ObjHeartVnum;
extern  OBJ_INDEX_DATA *        ObjHeart;
extern  int                     ObjArmVnum;
extern  OBJ_INDEX_DATA *        ObjArm;
extern  int                     ObjUrnVnum;
extern  OBJ_INDEX_DATA *        ObjUrn;
extern  int                     ObjLegVnum;
extern  OBJ_INDEX_DATA *        ObjLeg;
extern  int                     ObjTurdVnum;
extern  OBJ_INDEX_DATA *        ObjTurd;
extern  int                     ObjMushroomVnum;
extern  OBJ_INDEX_DATA *        ObjMushroom;
extern  int                     ObjLightVnum;
extern  OBJ_INDEX_DATA *        ObjLight;
extern  int                     ObjSpringVnum;
extern  OBJ_INDEX_DATA *        ObjSpring;
extern  int                     ObjDumplingVnum;
extern  OBJ_INDEX_DATA *        ObjDumpling;
extern  int                     ObjBougiVnum;
extern  OBJ_INDEX_DATA *        ObjBougi;
extern  int                     ObjPonVnum;
extern  OBJ_INDEX_DATA *        ObjPon;
extern  int                     ObjChickenVnum;
extern  OBJ_INDEX_DATA *        ObjChicken;
extern  int                     ObjMagicStoneVnum;
extern  OBJ_INDEX_DATA *        ObjMagicStone;
extern  int                     ObjMeatVnum;
extern  OBJ_INDEX_DATA *        ObjMeat;
extern  int                     ObjLetterVnum;
extern  OBJ_INDEX_DATA *        ObjLetter;
extern  bool                    ChatLog;
extern  bool                    ChannelAuction;
extern  bool                    ChannelChat;
extern  bool                    ChannelBulletin;
extern  bool                    ChannelImmtalk;
extern  bool                    ChannelMusic;
extern  bool                    ChannelQuestion;
extern  bool                    ChannelShout;
extern  bool                    ChannelYell;
extern  bool                    ChannelGamble;
extern  bool                    ChannelClass;
extern  bool                    ChannelClub;
extern  bool                    ChannelSemote;
extern  bool                    ChannelWeather;
extern  bool                    ChannelPhone;
extern  bool                    ChannelSuicide;
extern  bool                    ChannelRumor;
extern  bool                    ChannelNotice;
extern  bool                    ChannelGroup;
extern  bool                    ChannelPK;
extern  bool                    ConfigAutoExit;
extern  bool                    ConfigAutoLoot;
extern  bool                    ConfigAutoSac;
extern  bool                    ConfigBlank;
extern  bool                    ConfigBrief;
extern  bool                    ConfigCombine;
extern  bool                    ConfigPrompt;
extern  bool                    ConfigExact;
extern  bool                    ConfigMessage;
extern  bool                    ConfigFlee;
extern  bool                    ConfigAngel;
extern  bool                    ConfigAutoFood;
extern  bool                    ConfigAutoDrink;
extern  bool                    ConfigRebirth;
extern  bool                    ConfigTrain;
extern  bool                    ConfigPractice;
extern  bool                    ConfigAnsi;
extern  bool                    ConfigLotto;
extern  int                     MaxRepeat;
extern  int                     MinCNameLen;
extern  int                     MaxCNameLen;
extern  int                     MinPasswordLen;
extern  int                     AuthPort;
extern  int                     FingerPort;
extern  int                     TelnetPort;
extern  bool                    NotifyDebug;
extern  bool                    NotifyInfo;
extern  bool                    NotifyWizard;
extern  bool                    NotifyNewplayer;
extern  bool                    NotifyFailpass;
extern  bool                    NotifyEmerg;
extern  bool                    NotifyCrit;
extern  bool                    NotifyErr;
extern  bool                    NotifyXname;
extern  bool                    NotifyNet;
extern  int                     ClubCountersignDay;
extern  int                     MaxLoginError;
extern  int                     MaxLoginIdle;
extern  int                     MaxPlayingIdle;
extern  int                     MaxCorpseIdle;
extern  int                     MaxGameIdle;
extern  int                     GoldWeight;
extern  int                     PryCost;
extern  int                     MaxAuctionGold;
extern  int                     AngelLevel;
extern  int                     MaxWhereGold;
extern  int                     MinWhereGold;
extern  int                     FindCost;
extern  int                     NoteLevel;
extern  int                     MaxHitroll;
extern  int                     HoursPerDay;
extern  int                     DaysPerMonth;
extern  int                     MonthsPerYear;
extern  char                    DefaultEmail[];
extern  char                    DefaultUnit[];
extern  char                    FromUnknown[];
extern  char *                  DayName[MAX_DAY];
extern  char *                  MonthName[MAX_MONTH];
extern  char *                  HourName[MAX_HOUR];
extern  char                    PlayerArea[];
extern  int                     PlayerAreaNum;
extern  int                     PlayerAreaCap;
extern  AREA_DATA *             PlayerZone;
extern  int                     HomeGold;
extern  int                     HomeMobile;
extern  int                     HomeObject;
extern  int                     HomeGoldTitle;
extern  int                     HomeGoldDescr;
extern  int                     HomeGoldLink;
extern  int                     HomeManMax;
extern  int                     ObjWearMask;
extern  int                     AttackMask;
extern  int                     MarryAge;
extern  char                    PermFile[];
extern  char                    LastMessage[];
extern  int                     FollowLevel;
extern  int                     OverLoading;
extern  int                     SocketTimeOut;
extern  int                     MountCost;
extern  int                     LeftSkillCost;
extern  int                     AskCost;
extern  int                     EQSearchCost;
extern  int                     ClubCreateFund;
extern  int                     ClubCountersignFund;
extern  int                     ClubCountersignCount;
extern  int                     ClubExpense;
extern  int                     ClubAttrGain;
extern  int                     MaxHeroShow;
extern  bool                    CoreDump;
extern  int                     AuctionTimes;
extern  int                     AuctionGold;
extern  int                     AlignmentDiff;
extern  int                     AlignRange;
extern  int                     DefaultAlign;
extern  int                     ProfCycle;
extern  int                     GambleLost;
extern  int                     MaxRoomPlayer;
extern  int                     MaxRoomObject;

#define PUSH_FUNCTION( a )       push_function( (a) );
#define POP_FUNCTION()           pop_function();
#define RETURN( a )              { POP_FUNCTION(); return (a); }
#define RETURN_NULL()            { POP_FUNCTION(); return;     }

#undef  CD
#undef  MID
#undef  OD
#undef  OID
#undef  RID
#undef  SF
#undef  DD
#undef  SD
#undef  OTD


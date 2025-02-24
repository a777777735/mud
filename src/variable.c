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

/* 常用的房間的虛擬號碼 */
#define ROOM_VNUM_JAIL                  1
#define ROOM_VNUM_LIMBO                 2
#define ROOM_VNUM_CHAT                  3
#define ROOM_VNUM_DEADROOM              10
#define ROOM_VNUM_DEADBODY              20
#define ROOM_VNUM_RECALL                501
#define ROOM_VNUM_SCHOOL                500
#define ROOM_VNUM_FAIL                  50

/* 著名的MOB的虛擬號碼 */
#define MOB_VNUM_VAMPIRE                2
#define MOB_VNUM_PRACTICE               22

int                     RoomJailVnum    =       ROOM_VNUM_JAIL;
ROOM_INDEX_DATA *       RoomJail        =       NULL;

int                     RoomLimboVnum   =       ROOM_VNUM_LIMBO;
ROOM_INDEX_DATA *       RoomLimbo       =       NULL;

int                     RoomDeadVnum    =       ROOM_VNUM_DEADROOM;
ROOM_INDEX_DATA *       RoomDead        =       NULL;

int                     RoomRecallVnum  =       ROOM_VNUM_RECALL;
ROOM_INDEX_DATA *       RoomRecall      =       NULL;

int                     RoomSchoolVnum  =       ROOM_VNUM_SCHOOL;
ROOM_INDEX_DATA *       RoomSchool      =       NULL;

int                     RoomFailVnum    =       ROOM_VNUM_FAIL;
ROOM_INDEX_DATA *       RoomFail        =       NULL;

int                     RoomChatVnum    =       ROOM_VNUM_CHAT;
ROOM_INDEX_DATA *       RoomChat        =       NULL;

int                     RoomCorpseVnum  =       ROOM_VNUM_DEADBODY;
ROOM_INDEX_DATA *       RoomCorpse      =       NULL;

AREA_DATA       *       DefaultArea     =       NULL;

int                     MobVampireVnum  =       MOB_VNUM_VAMPIRE;
MOB_INDEX_DATA *        MobVampire      =       NULL;

int                     MobPracticeVnum =       MOB_VNUM_PRACTICE;
MOB_INDEX_DATA *        MobPractice     =       NULL;

int                     ObjProtypeVnum  =       OBJ_VNUM_PROTYPE;
OBJ_INDEX_DATA *        ObjProtype      =       NULL;

int                     ObjMoneyVnum    =       OBJ_VNUM_MONEY_ONE;
OBJ_INDEX_DATA *        ObjMoney        =       NULL;

int                     ObjMoneySomeVnum=       OBJ_VNUM_MONEY_SOME;
OBJ_INDEX_DATA *        ObjMoneySome    =       NULL;

int                     ObjCorpseNPCVnum=       OBJ_VNUM_CORPSE_NPC;
OBJ_INDEX_DATA *        ObjCorpseNPC    =       NULL;

int                     ObjCorpsePCVnum =       OBJ_VNUM_CORPSE_PC;
OBJ_INDEX_DATA *        ObjCorpsePC     =       NULL;

int                     ObjHeadVnum     =       OBJ_VNUM_SEVERED_HEAD;
OBJ_INDEX_DATA *        ObjHead         =       NULL;

int                     ObjHeartVnum    =       OBJ_VNUM_TORN_HEART;
OBJ_INDEX_DATA *        ObjHeart        =       NULL;

int                     ObjArmVnum      =       OBJ_VNUM_SLICED_ARM;
OBJ_INDEX_DATA *        ObjArm          =       NULL;

int                     ObjLegVnum      =       OBJ_VNUM_SLICED_LEG;
OBJ_INDEX_DATA *        ObjLeg          =       NULL;

int                     ObjTurdVnum     =       OBJ_VNUM_FINAL_TURD;
OBJ_INDEX_DATA *        ObjTurd         =       NULL;

int                     ObjMushroomVnum =       OBJ_VNUM_MUSHROOM;
OBJ_INDEX_DATA *        ObjMushroom     =       NULL;

int                     ObjLightVnum    =       OBJ_VNUM_LIGHT_BALL;
OBJ_INDEX_DATA *        ObjLight        =       NULL;

int                     ObjSpringVnum   =       OBJ_VNUM_SPRING;
OBJ_INDEX_DATA *        ObjSpring       =       NULL;

int                     ObjDumplingVnum =       OBJ_VNUM_DUMPLING;
OBJ_INDEX_DATA *        ObjDumpling     =       NULL;

int                     ObjBougiVnum    =       OBJ_VNUM_BOUGI;
OBJ_INDEX_DATA *        ObjBougi        =       NULL;

int                     ObjPonVnum      =       OBJ_VNUM_PON;
OBJ_INDEX_DATA *        ObjPon          =       NULL;

int                     ObjChickenVnum  =       OBJ_VNUM_CHICKEN;
OBJ_INDEX_DATA *        ObjChicken      =       NULL;

int                     ObjMagicStoneVnum=      OBJ_VNUM_MAGICSTONE;
OBJ_INDEX_DATA *        ObjMagicStone   =       NULL;

int                     ObjMeatVnum     =       OBJ_VNUM_MEAT;
OBJ_INDEX_DATA *        ObjMeat         =       NULL;

int                     ObjLetterVnum   =       OBJ_VNUM_LETTER;
OBJ_INDEX_DATA *        ObjLetter       =       NULL;

int                     ObjUrnVnum      =       OBJ_VNUM_URN;
OBJ_INDEX_DATA *        ObjUrn          =       NULL;

bool                    ChatLog         =       TRUE;

bool                    ChannelAuction  =        TRUE;
bool                    ChannelChat     =        TRUE;
bool                    ChannelBulletin =        TRUE;
bool                    ChannelImmtalk  =        TRUE;
bool                    ChannelMusic    =        TRUE;
bool                    ChannelQuestion =        TRUE;
bool                    ChannelShout    =        TRUE;
bool                    ChannelYell     =        TRUE;
bool                    ChannelGamble   =        TRUE;
bool                    ChannelClass    =        TRUE;
bool                    ChannelClub     =        TRUE;
bool                    ChannelSemote   =        TRUE;
bool                    ChannelWeather  =        TRUE;
bool                    ChannelPhone    =        TRUE;
bool                    ChannelSuicide  =        TRUE;
bool                    ChannelRumor    =        TRUE;
bool                    ChannelNotice   =        TRUE;
bool                    ChannelGroup    =        TRUE;
bool                    ChannelPK       =        TRUE;

bool                    ConfigAutoExit  =       TRUE;
bool                    ConfigAutoLoot  =       TRUE;
bool                    ConfigAutoSac   =       FALSE;
bool                    ConfigBlank     =       TRUE;
bool                    ConfigBrief     =       FALSE;
bool                    ConfigCombine   =       TRUE;
bool                    ConfigPrompt    =       TRUE;
bool                    ConfigExact     =       TRUE;
bool                    ConfigMessage   =       TRUE;
bool                    ConfigFlee      =       FALSE;
bool                    ConfigAngel     =       TRUE;
bool                    ConfigAutoFood  =       FALSE;
bool                    ConfigAutoDrink =       FALSE;
bool                    ConfigRebirth   =       FALSE;
bool                    ConfigTrain     =       FALSE;
bool                    ConfigPractice  =       FALSE;
bool                    ConfigAnsi      =       FALSE;
bool                    ConfigLotto     =       FALSE;

bool                    NotifyDebug     =       TRUE;
bool                    NotifyInfo      =       TRUE;
bool                    NotifyWizard    =       TRUE;
bool                    NotifyNewplayer =       TRUE;
bool                    NotifyFailpass  =       TRUE;
bool                    NotifyEmerg     =       TRUE;
bool                    NotifyCrit      =       TRUE;
bool                    NotifyErr       =       TRUE;
bool                    NotifyXname     =       TRUE;
bool                    NotifyNet       =       TRUE;

int                     MaxRepeat       =       MAX_REPEAT;
int                     MinCNameLen     =       2;
int                     MaxCNameLen     =       MAX_NAME_LENGTH;
int                     MinPasswordLen  =       5;

int                     AuthPort        =       113;
int                     FingerPort      =       79;
int                     TelnetPort      =       23;

int                     HomeGold        =       10000000;
int                     HomeGoldTitle   =       100000;
int                     HomeGoldDescr   =       200000;
int                     HomeGoldLink    =       1000000;
int                     HomeObject      =       2000000;
int                     HomeMobile      =       1000;
int                     HomeManMax      =       200;

int                     ForceLevel      =       15;
int                     MaxLoginError   =       5;
int                     MaxLoginIdle    =       -5;
int                     MaxPlayingIdle  =       30;
int                     MaxCorpseIdle   =       100;
int                     MaxGameIdle     =       300;
int                     GoldWeight      =       10000;
int                     PryCost         =       1500;
int                     MaxAuctionGold  =       100000000;
int                     AuctionTimes    =       10;
int                     AuctionGold     =       2000000;
int                     AngelLevel      =       10;
int                     MaxWhereGold    =       20;
int                     MinWhereGold    =       10;
int                     FindCost        =       100;
int                     NoteLevel       =       2;
int                     MaxHitroll      =       150;

char                    DefaultEmail[BUFSIZ] = { "anonymous@unknown.net" };
char                    DefaultUnit[BUFSIZ]  = { "個" };
char                    FromUnknown[BUFSIZ]  = { "unknown" };

char *                  HourName[MAX_HOUR];
char *                  DayName[MAX_DAY];
char *                  MonthName[MAX_MONTH];

int                     HoursPerDay     =       24;
int                     DaysPerMonth    =       35;
int                     MonthsPerYear   =       17;

int                     MarryAge        =       20;

AREA_DATA             * PlayerZone           = NULL;
char                    PlayerArea[BUFSIZ]   = { "" };
int                     PlayerAreaNum        = -1;
int                     PlayerAreaCap        = -1;

char                    PermFile[MAX_FILE_LENGTH] = ".mud";
int                     ClubCountersignDay   = 20;
int                     ObjWearFlag          = 0;
int                     ObjWearMask          = 0;
int                     SectorMask           = 0;
int                     AttackMask           = 0;
int                     FollowLevel          = 5;
int                     OverLoading          = 80;
int                     SocketTimeOut        = 60;
int                     MountCost            = 10;
int                     LeftSkillCost        = 20;

int                     AlignmentDiff        = 500;
int                     AlignRange           = 1000;
int                     DefaultAlign         = 0;
int                     AskCost              = 100;
int                     EQSearchCost         = 100;
int                     ClubCreateFund       = 10000000;
int                     ClubCountersignFund  = 100000;
int                     ClubCountersignCount = 5;
int                     ClubExpense          = 10000;
int                     ClubAttrGain         = 4;
int                     MaxHeroShow          = 200;
char                    LastMessage[MAX_INPUT_LENGTH] = "";
bool                    CoreDump             = FALSE;
int                     ProfCycle            = 20;
int                     GambleLost           = 20;
int                     MaxRoomPlayer        = 20;
int                     MaxRoomObject        = 20;

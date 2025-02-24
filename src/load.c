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
#include <glob.h>
#include <time.h>
#include <unistd.h>
#include "merc.h"

#if defined(KEY)
#undef KEY
#endif


/* 外掛變數 */
extern SYMBOL_DATA *      symbol_hash[26][20];
extern bool               fReload;
extern unsigned int       FileError;

/* 外掛函數 */
extern void    mprog_read_programs     args( ( FILE_DATA * , MOB_INDEX_DATA * ) );
extern void    set_exit_default        args( ( EXIT_DATA * ) );
extern int     glob_file        args( ( const char *, const char *, glob_t * ) );

/* 區域函數 */
void set_area_default     args( ( AREA_DATA       * ) );
void set_room_default     args( ( ROOM_INDEX_DATA * ) );
void set_exit_default     args( ( EXIT_DATA       * ) );
void set_shop_default     args( ( SHOP_DATA       * ) );
void set_mineral_default  args( ( MINERAL_DATA    * ) );
void set_object_default   args( ( OBJ_INDEX_DATA  * ) );
void set_affect_default   args( ( AFFECT_DATA   * ) );
void set_mobile_default   args( ( MOB_INDEX_DATA  * ) );
void set_teach_default    args( ( TEACH_DATA      * ) );
void set_skill_default    args( ( SKILL_DATA      * ) );
void set_damage_default   args( ( DAMAGE_DATA     * ) );
void set_command_default  args( ( CMD_DATA        * ) );
void set_sector_default   args( ( SECTOR_DATA     * ) );
void set_class_default    args( ( CLASS_DATA      * ) );
void set_liq_default      args( ( LIQ_DATA        * ) );
void set_bus_default      args( ( BUS_DATA        * ) );
void set_job_default      args( ( JOB_DATA        * ) );
void set_mprog_default    args( ( MPROG_DATA      * ) );
void set_enquire_default  args( ( ENQUIRE_DATA    * ) );
void set_station_default  args( ( NET_DATA        * ) );
void set_club_default     args( ( CLUB_DATA       * ) );
void set_restrict_default args( ( RESTRICT_DATA * ) );
void set_quest_default    args( ( QUEST_INFO    * ) );
void set_question_default args( ( QUESTION_DATA * ) );
void set_event_default    args( ( EVENT_DATA    * ) );
void set_gift_default     args( ( GIFT_DATA     * ) );
void set_bounty_default   args( ( BOUNTY_DATA   * ) );
void set_immlist_default  args( ( IMMLIST_DATA  * ) );
void set_sale_default     args( ( SALE_DATA     * ) );
void set_situs_default    args( ( SITUS_DATA    * ) );
void set_ship_default     args( ( SHIP_DATA     * ) );
void set_effect_default   args( ( EFFECT_DATA   * ) );
void set_message_default  args( ( MESSAGE_DATA  * ) );
void set_angel_default    args( ( ANGEL_DATA    * ) );
void set_joke_default     args( ( JOKE_DATA     * ) );
void set_server_default   args( ( SERVER_DATA  * ) );
void set_note_default     args( ( NOTE_DATA    * ) );

EXTRA_DESCR_DATA * load_extra_descr     args( ( FILE_DATA * ) );
EXIT_DATA     * load_exit               args( ( FILE_DATA * ) );
DAMAGE_DATA   * load_damage             args( ( FILE_DATA * ) );
QUEST_INFO    * load_a_quest            args( ( FILE_DATA * ) );
QUESTION_DATA * load_a_question         args( ( FILE_DATA * ) );
BOUNTY_DATA   * load_a_bounty           args( ( FILE_DATA * ) );
CLUB_DATA     * load_a_club             args( ( FILE_DATA * ) );
NET_DATA      * load_station            args( ( FILE_DATA * ) );
EVENT_DATA    * load_a_event            args( ( FILE_DATA * ) );
IMMLIST_DATA  * load_a_immlist          args( ( FILE_DATA * ) );
GIFT_DATA     * load_a_gift             args( ( FILE_DATA * ) );
SALE_DATA     * load_a_sale             args( ( FILE_DATA * ) );
NOTE_DATA     * load_a_note             args( ( FILE_DATA * ) );
SHIP_DATA     * load_a_ship             args( ( FILE_DATA * ) );
VOTE_DATA     * load_a_vote             args( ( FILE_DATA * ) );
JOKE_DATA     * load_a_joke             args( ( FILE_DATA * ) );
TEACH_DATA    * load_mob_teach          args( ( FILE_DATA * ) );
AFFECT_DATA   * load_affect             args( ( FILE_DATA * ) );
RESTRICT_DATA * load_restrict           args( ( FILE_DATA * ) );
MESSAGE_DATA  * load_message            args( ( FILE_DATA * ) );
JOB_DATA      * load_job                args( ( FILE_DATA * ) );
MPROG_DATA    * load_program            args( ( FILE_DATA * ) );
ENQUIRE_DATA  * load_enquire            args( ( FILE_DATA * ) );
ANGEL_DATA    * load_a_angel            args( ( FILE_DATA * ) );
SITUS_DATA    * load_a_situs            args( ( FILE_DATA * ) );

#define KEY( literal, field, value ) \
  if ( !str_cmp( word, literal ) )   \
  {                                  \
    fMatch = TRUE;                   \
    field  = value;                  \
    break;                           \
  }

#define TEST_READ_ERROR         { if ( FileError ) break; }

/* 設定區域檔中的區域的預設值 */
void set_area_default( AREA_DATA * pArea )
{
  PUSH_FUNCTION( "set_area_default" );
  pArea->reset_first = NULL;
  pArea->reset_last  = NULL;
  pArea->list        = NULL;
  pArea->attribute   = 1;
  pArea->name        = NULL;
  pArea->editor      = NULL;
  pArea->description = NULL;
  pArea->filename    = NULL;
  pArea->mineral     = NULL;
  pArea->age         = 15;
  pArea->serial      = ERRORCODE;
  pArea->upper       = ERRORCODE;
  pArea->lower       = ERRORCODE;
  pArea->foggy       = ERRORCODE;
  pArea->nplayer     = 0;
  pArea->room        = 0;
  pArea->obj         = 0;
  pArea->mob         = 0;
  pArea->shop        = 0;
  pArea->reset       = 0;
  pArea->mine        = 0;
  pArea->capital_no  = 0;
  pArea->capital     = NULL;
  pArea->newhand     = FALSE;
  pArea->pk          = FALSE;
  pArea->fight       = FALSE;
  RETURN_NULL();
}

/* 設定區域檔中的怪物的預設值 */
void set_mobile_default( MOB_INDEX_DATA * pMobIndex )
{
  int loop;

  PUSH_FUNCTION( "set_mobile_default" );

  pMobIndex->name              = NULL;
  pMobIndex->short_descr       = NULL;
  pMobIndex->long_descr        = str_dup( "" );
  pMobIndex->description       = NULL;
  pMobIndex->filename          = NULL;
  pMobIndex->deadmsg           = NULL;
  pMobIndex->auction           = NULL;
  pMobIndex->enable            = NULL;
  pMobIndex->job               = NULL;
  pMobIndex->area              = NULL;
  pMobIndex->affected          = NULL;
  pMobIndex->vnum              = ERRORCODE;
  pMobIndex->act               = ACT_IS_NPC;
  pMobIndex->pShop             = NULL;
  pMobIndex->alignment         = DefaultAlign;
  pMobIndex->level             = 1;
  pMobIndex->hitroll           = 0;
  pMobIndex->ac                = 0;
  pMobIndex->damroll           = 0;
  pMobIndex->reborn_vnum       = 0;
  pMobIndex->reborn_room       = 0;
  pMobIndex->migrate           = 0;
  pMobIndex->now_str           = DefaultStr;
  pMobIndex->now_int           = DefaultInt;
  pMobIndex->now_wis           = DefaultWis;
  pMobIndex->now_dex           = DefaultDex;
  pMobIndex->now_con           = DefaultCon;
  pMobIndex->hit               = 0;
  pMobIndex->mana              = 0;
  pMobIndex->move              = 0;
  pMobIndex->class             = class_demos;
  pMobIndex->gold              = 0;
  pMobIndex->sex               = SEX_MALE;
  pMobIndex->tractable         = -1;
  pMobIndex->multipile         = 100;
  pMobIndex->hitnodice         = 100;
  pMobIndex->hitsizedice       = 100;
  pMobIndex->hitplus           = 100;
  pMobIndex->damnodice         = 100;
  pMobIndex->damsizedice       = 100;
  pMobIndex->damplus           = 100;
  pMobIndex->attack_ratio      = 100;
  pMobIndex->dodge_ratio       = 100;
  pMobIndex->speak             = TRUE;
  pMobIndex->Sentinel          = FALSE;
  pMobIndex->Scavenger         = FALSE;
  pMobIndex->Aggressive        = FALSE;
  pMobIndex->StayArea          = FALSE;
  pMobIndex->Wimpy             = FALSE;
  pMobIndex->Pet               = FALSE;
  pMobIndex->Train             = FALSE;
  pMobIndex->Practice          = FALSE;
  pMobIndex->Rebirth           = FALSE;
  pMobIndex->RebornFight       = FALSE;
  pMobIndex->Ask               = FALSE;
  pMobIndex->AutoSetValue      = FALSE;
  pMobIndex->NoReborn          = FALSE;
  pMobIndex->NoKill            = FALSE;
  pMobIndex->Enroll            = FALSE;
  pMobIndex->AlignGood         = FALSE;
  pMobIndex->AlignEvil         = FALSE;
  pMobIndex->NoSummon          = FALSE;
  pMobIndex->temp              = str_dup( "" );

  for ( loop = 0; loop < MAX_SKILL; loop++ ) pMobIndex->skill[loop] = 0;
  RETURN_NULL();
}

/* 設定區域檔中的怪物教導的預設值 */
void set_teach_default( TEACH_DATA * teach )
{
  PUSH_FUNCTION( "set_teach_default" );
  teach->slot      = ERRORCODE;
  teach->cost      =  0;
  teach->adept     = 20;
  teach->inventory =  0;
  RETURN_NULL();
}

/* 設定區域檔中的房間的預設值 */
void set_room_default( ROOM_INDEX_DATA * pRoomIndex )
{
  PUSH_FUNCTION( "set_room_default" );
  pRoomIndex->people        = NULL;
  pRoomIndex->next_in_area  = NULL;
  pRoomIndex->contents      = NULL;
  pRoomIndex->extra_descr   = NULL;
  pRoomIndex->job           = NULL;
  pRoomIndex->club          = NULL;
  pRoomIndex->board         = NULL;
  pRoomIndex->mine          = NULL;
  pRoomIndex->enquire       = NULL;
  pRoomIndex->vnum          = ERRORCODE;
  pRoomIndex->name          = NULL;
  pRoomIndex->description   = NULL;
  pRoomIndex->filename      = NULL;
  pRoomIndex->sector        = NULL;
  pRoomIndex->light         = 0;
  pRoomIndex->gold          = 0;
  pRoomIndex->cost          = 0;
  pRoomIndex->mob_reborn    = 0;
  pRoomIndex->nplayer       = 0;
  pRoomIndex->nobject       = 0;
  pRoomIndex->NoLink        = FALSE;
  pRoomIndex->player        = FALSE;
  pRoomIndex->writeable     = FALSE;
  pRoomIndex->Dark          = FALSE;
  pRoomIndex->NoMob         = FALSE;
  pRoomIndex->Indoors       = FALSE;
  pRoomIndex->Private       = FALSE;
  pRoomIndex->Safe          = FALSE;
  pRoomIndex->ForeverLight  = FALSE;
  pRoomIndex->NoRecall      = FALSE;
  pRoomIndex->DepositMoney  = FALSE;
  pRoomIndex->StoreRoom     = FALSE;
  pRoomIndex->NoFight       = FALSE;
  pRoomIndex->NoQuit        = FALSE;
  pRoomIndex->Club          = FALSE;
  pRoomIndex->Memorize      = FALSE;
  pRoomIndex->Stock         = FALSE;
  pRoomIndex->NoWhere       = FALSE;
  pRoomIndex->Sail          = FALSE;
  pRoomIndex->Fane          = FALSE;
  pRoomIndex->Killer        = FALSE;
  pRoomIndex->Standard      = FALSE;
  pRoomIndex->Waterful      = FALSE;
  pRoomIndex->Foodful       = FALSE;
  pRoomIndex->ManaCure      = FALSE;
  pRoomIndex->MoveCure      = FALSE;
  pRoomIndex->HitCure       = FALSE;
  pRoomIndex->Upgrade       = FALSE;
  pRoomIndex->PracRoom      = FALSE;
  pRoomIndex->owner.high = 0;
  pRoomIndex->owner.low  = 0;

  RETURN_NULL();
}

/* 數定房間出口的預設值 */
void set_exit_default( EXIT_DATA * pExit )
{
  PUSH_FUNCTION( "set_exit_default" );
  pExit->description = NULL;
  pExit->keyword     = NULL;
  pExit->message     = NULL;
  pExit->direction   = ERRORCODE;
  pExit->key         = 0;
  pExit->foggy       = FALSE;
  pExit->isdoor      = FALSE;
  pExit->closed      = FALSE;
  pExit->locked      = FALSE;
  pExit->pick        = FALSE;
  pExit->vnum        = ERRORCODE;
  RETURN_NULL();
}

/* 設定物品的預設值 */
void set_object_default( OBJ_INDEX_DATA * pObjIndex )
{
  int loop;

  PUSH_FUNCTION( "set_obj_default" );
  pObjIndex->vnum        = ERRORCODE;
  pObjIndex->item_type   = ERRORCODE;
  pObjIndex->wear_flags  = ITEM_NOT_TAKE;
  pObjIndex->level       = 1;
  pObjIndex->weight      = 1;
  pObjIndex->cost        = ERRORCODE;
  pObjIndex->armor       = ERRORCODE;
  pObjIndex->name        = NULL;
  pObjIndex->short_descr = NULL;
  pObjIndex->description = NULL;
  pObjIndex->wearmsg     = NULL;
  pObjIndex->remmsg      = NULL;
  pObjIndex->filename    = NULL;
  pObjIndex->unit        = DefaultUnit;
  pObjIndex->job         = NULL;
  pObjIndex->restrict    = NULL;
  pObjIndex->message     = NULL;
  pObjIndex->affected    = NULL;
  pObjIndex->impact      = NULL;
  pObjIndex->disappear   = FALSE;
  pObjIndex->exp         = 0;

  pObjIndex->auction_times = 0;
  pObjIndex->auction_gold  = 0;

  pObjIndex->Glow        = FALSE;
  pObjIndex->Hum         = FALSE;
  pObjIndex->Dark        = FALSE;
  pObjIndex->Lock        = FALSE;
  pObjIndex->Evil        = FALSE;
  pObjIndex->Invis       = FALSE;
  pObjIndex->Magic       = FALSE;
  pObjIndex->NoDrop      = FALSE;
  pObjIndex->Bless       = FALSE;
  pObjIndex->AntiGood    = FALSE;
  pObjIndex->AntiEvil    = FALSE;
  pObjIndex->AntiNeutral = FALSE;
  pObjIndex->NoRemove    = FALSE;
  pObjIndex->Inventory   = FALSE;
  pObjIndex->Cancachet   = FALSE;
  pObjIndex->UserSet     = FALSE;
  pObjIndex->NoSave      = FALSE;
  pObjIndex->Contraband  = FALSE;
  pObjIndex->Loadable    = FALSE;
  pObjIndex->Takeable    = FALSE;
  pObjIndex->Sale        = FALSE;

  for ( loop = 0; loop < MAX_OBJECT_VALUE; loop++ )
    pObjIndex->value[ loop ] = 0;

  RETURN_NULL();
}

/* 設定物品屬性的預設值 */
void set_affect_default( AFFECT_DATA * pAffect )
{
  PUSH_FUNCTION( "set_affect_default" );

  pAffect->type        = ERRORCODE;
  pAffect->duration    = ERRORCODE;
  pAffect->location    = ERRORCODE;
  pAffect->modifier    = 0;
  RETURN_NULL();
}

/* 設定商店的預定值 */
void set_shop_default( SHOP_DATA * pShop )
{
  int Trade;

  PUSH_FUNCTION( "set_shop_default" );

  pShop->keeper      = ERRORCODE;
  pShop->type        = ERRORCODE;
  pShop->filename    = NULL;
  pShop->sential     = NULL;
  pShop->profit_buy  = 100;
  pShop->profit_sell = 100;
  pShop->open_hour   =   0;
  pShop->close_hour  =  23;

  for ( Trade = 0; Trade < MAX_TRADE; Trade++ ) pShop->buy_type[Trade] = 0;
  RETURN_NULL();
}

void set_mineral_default( MINERAL_DATA * pMineral )
{
  PUSH_FUNCTION( "set_mineral_default" );

  pMineral->mineral = NULL;
  pMineral->message = NULL;
  pMineral->count   = ERRORCODE;
  pMineral->flags   = 0;
  RETURN_NULL();
}

void set_skill_default( SKILL_DATA * pSkill )
{
  PUSH_FUNCTION( "set_skill_default" );
  pSkill->position        = POS_FIGHTING;
  pSkill->name            = NULL;
  pSkill->cname           = NULL;
  pSkill->msg_off         = NULL;
  pSkill->help            = NULL;
  pSkill->message         = NULL;
  pSkill->limit           = NULL;
  pSkill->check           = NULL;
  pSkill->damage          = NULL;
  pSkill->restrict        = NULL;
  pSkill->function        = NULL;
  pSkill->affected        = NULL;
  pSkill->type            = TAR_IGNORE;
  pSkill->cost_type       = COST_MANA;
  pSkill->cost            = 0;
  pSkill->wait            = 0;
  pSkill->exp             = 0;
  pSkill->associate       = ERRORCODE;
  pSkill->sex             = ERRORCODE;
  pSkill->degree          = 1;
  pSkill->chance          = 0;
  pSkill->affect          = 0;
  pSkill->affect_id       = 0;
  pSkill->rating          = ERRORCODE;
  pSkill->antirating      = ERRORCODE;
  pSkill->choosen         = 0;
  pSkill->adeptation      = 0;
  pSkill->weapon          = ERRORCODE;
  pSkill->slot            = ERRORCODE;
  pSkill->enable          = FALSE;
  pSkill->say_spell       = FALSE;
  pSkill->teach           = FALSE;
  pSkill->concentration   = FALSE;
  pSkill->cast            = TRUE;
  pSkill->innate          = FALSE;
  pSkill->canask          = FALSE;
  pSkill->valid           = TRUE;
  pSkill->ply             = 0;
  pSkill->qutoient        = 1;
  RETURN_NULL();
}

void set_damage_default( DAMAGE_DATA * pDamage )
{
  PUSH_FUNCTION( "set_damage_default" );

  pDamage->description = NULL;
  pDamage->innate      = NULL;
  pDamage->routine     = NULL;
  pDamage->effect      = NULL;
  pDamage->chance      = ERRORCODE;
  pDamage->value       = ERRORCODE;
  pDamage->vicevalue   = 0;
  pDamage->situs       = ATTACK_RANDOM;
  pDamage->obj_vnum    = ERRORCODE;
  pDamage->parry       = 0;
  pDamage->multiple    = 0;
  RETURN_NULL();
}

void set_command_default( CMD_DATA * pCommand )
{
  PUSH_FUNCTION( "set_command_default" );
  pCommand->name     = NULL;
  pCommand->cname    = NULL;
  pCommand->help     = NULL;
  pCommand->function = NULL;
  pCommand->filename = NULL;
  pCommand->position = POS_STANDING;
  pCommand->level    = 0;
  pCommand->exec     = 0;
  pCommand->log      = LOG_NORMAL;
  pCommand->lock     = FALSE;
  pCommand->canlock  = TRUE;
  pCommand->mobonly  = FALSE;
  pCommand->chat     = FALSE;
  pCommand->wizlog   = FALSE;
  pCommand->jail     = FALSE;
  pCommand->dead     = FALSE;
  pCommand->order    = FALSE;
  pCommand->lost     = FALSE;
  pCommand->limit    = FALSE;
  pCommand->trace    = FALSE;
  RETURN_NULL();
}

void set_sector_default( SECTOR_DATA * pSector )
{
  PUSH_FUNCTION( "set_sector_default" );

  pSector->vnum     = ERRORCODE;
  pSector->movement = ERRORCODE;
  pSector->cname    = NULL;
  pSector->dark     = TRUE;
  pSector->fly      = FALSE;
  pSector->swim     = FALSE;
  RETURN_NULL();
}

void set_class_default( CLASS_DATA * pClass )
{
  int loop;

  PUSH_FUNCTION( "set_class_default" );

  pClass->name         = NULL;
  pClass->cname        = NULL;
  pClass->title        = NULL;
  pClass->msg_limit    = NULL;
  pClass->vnum         = ERRORCODE;
  pClass->low_rebirth  = 0;
  pClass->high_rebirth = 0;
  pClass->rebirth_gold = 0;
  pClass->warn         = 0;
  pClass->associate    = ERRORCODE;
  pClass->limit        = -1;
  pClass->nskill       = ERRORCODE;
  pClass->mask         = 0;
  pClass->multiplier   = 2;
  pClass->fMana        = FALSE;
  pClass->rudiment     = FALSE;
  pClass->force        = FALSE;
  pClass->innate       = FALSE;
  pClass->select       = FALSE;

  for ( loop = 0; loop < MAX_ATTR; loop++ )
  {
    pClass->max_default[loop] = 0;
    pClass->min_default[loop] = 0;
    pClass->hero[loop]        = 0;
    pClass->attr[loop]        = 0;
    pClass->factor[loop]      = 0;
  }

  for ( loop = 0; loop < MAX_LEVEL; loop++ )
    pClass->rank[loop] = NULL;

  RETURN_NULL();
}

void set_liq_default( LIQ_DATA * pLiq )
{
  PUSH_FUNCTION( "set_liq_default" );

  pLiq->name   = NULL;
  pLiq->color  = NULL;
  pLiq->slot   = ERRORCODE;
  pLiq->full   = 0;
  pLiq->thirst = 0;
  pLiq->drunk  = 0;
  pLiq->water  = FALSE;
  RETURN_NULL();
}

/* 設定地鐵資料的預設值 */
void set_bus_default( BUS_DATA * pBus )
{
  PUSH_FUNCTION( "set_bus_default" );

  pBus->next      = NULL;
  pBus->station   = NULL;
  pBus->platform  = NULL;
  pBus->loge      = NULL;
  pBus->name      = NULL;
  pBus->cost      = 100;
  pBus->count     = 0;
  RETURN_NULL();
}

void set_job_default( JOB_DATA * pJob )
{
  PUSH_FUNCTION( "set_job_default" );

  pJob->keyword  = NULL;
  pJob->function = NULL;
  pJob->position = ERRORCODE;

  RETURN_NULL();
}

void set_mprog_default( MPROG_DATA * pProg )
{
  PUSH_FUNCTION( "set_mprog_default" );

  pProg->arglist = NULL;
  pProg->comlist = NULL;
  pProg->type    = ERRORCODE;

  RETURN_NULL();
}
void set_enquire_default( ENQUIRE_DATA * pEnquire )
{
  PUSH_FUNCTION( "set_enquire_default" );

  pEnquire->keyword = NULL;

  RETURN_NULL();
}
/* 設定網際連線的預設值 */
void set_station_default( NET_DATA * pNet )
{
  PUSH_FUNCTION( "set_station_default" );

  pNet->name            = NULL;
  pNet->cname           = NULL;
  pNet->address         = NULL;
  pNet->port            = -1;
  pNet->import          = -1;
  pNet->export          = -1;
  pNet->nRead           = 0;
  pNet->nWrite          = 0;
  pNet->timer           = 0;
  pNet->import_duration = 0;
  pNet->export_duration = 0;
  pNet->log             = FALSE;
  pNet->valid           = TRUE;
  pNet->message[0]      = '\x0';
  RETURN_NULL();
}

/* 清除幫派的資料 */
void set_club_default( CLUB_DATA * pClub )
{
  int loop;
  int loop_2;

  PUSH_FUNCTION( "set_club_default" )

  pClub->name        = NULL;
  pClub->cname       = NULL;
  pClub->master      = NULL;
  pClub->vicemaster  = NULL;
  pClub->location    = NULL;
  pClub->timer       = -1;
  pClub->money       = 0;
  pClub->status      = CLUB_STATUS_UNKNOW;

  /* 清除幫眾的資料 */
  for ( loop = 0; loop < MAX_CLUB_MEMBER; loop++ )
  {
    pClub->verify[loop] = MEMBER_NO_VERIFY;
    pClub->member[loop] = NULL;
  }

  /* 清除長老的資料 */
  for ( loop = 0; loop < MAX_CLUB_DOYEN; loop++ )
    pClub->doyen[loop] = NULL;

  /* 清除幫庫的資料 */
  for ( loop = 0; loop < MAX_LEVEL; loop++ )
  {
    for ( loop_2 = 0; loop_2 < MAX_STAFF; loop_2++ )
    {
      pClub->staff[loop][loop_2] = NULL;
    }
  }

  RETURN_NULL();
}

/* 設定物品限制結構初值 */
void set_restrict_default( RESTRICT_DATA * pRestrict )
{
  PUSH_FUNCTION( "set_restrict_default" );

  if ( pRestrict )
  {
    pRestrict->occasion  = OCCASION_NONE;
    pRestrict->type      = ERRORCODE;
    pRestrict->value     = ERRORCODE;
    pRestrict->vicevalue = 0;
  }

  else mudlog( LOG_DEBUG, "set_restrict_default﹕傳入結構不存在。" );

  RETURN_NULL();
}

void set_quest_default( QUEST_INFO * pQuest )
{
  PUSH_FUNCTION( "Set_qeuest_default" );

  pQuest->mark  = NULL;
  pQuest->info  = NULL;
  pQuest->help  = str_dup( "" );
  pQuest->show  = FALSE;
  pQuest->title = FALSE;

  RETURN_NULL();
}

void set_question_default( QUESTION_DATA * pQuestion )
{
  int loop;

  PUSH_FUNCTION( "set_question_default" );

  pQuestion->title = NULL;
  pQuestion->count = 0;
  pQuestion->fail  = 0;

  for ( loop = 0; loop < MAX_QUESTION; loop++ )
  {
    pQuestion->question[loop] = NULL;
    pQuestion->answer[loop]   = FALSE;
  }

  RETURN_NULL();
}
void set_event_default( EVENT_DATA * pEvent )
{
  PUSH_FUNCTION( "set_event_default" );

  pEvent->title    = NULL;
  pEvent->chance   = ERRORCODE;
  pEvent->count    = 0;
  pEvent->lock     = FALSE;
  pEvent->function = NULL;
  pEvent->keyword  = NULL;
  RETURN_NULL();
}

void set_gift_default( GIFT_DATA * pGift )
{
  PUSH_FUNCTION( "set_gift_default" );

  if ( pGift )
  {
    pGift->gift     = NULL;
    pGift->title    = NULL;
    pGift->message  = NULL;
    pGift->month    = ERRORCODE;
    pGift->day      = ERRORCODE;
    pGift->high     = ERRORCODE;
    pGift->low      = ERRORCODE;
    pGift->sender   = ERRORCODE;
    pGift->count    = 0;
    pGift->gold     = 0;
    pGift->stamp    = ERRORCODE;
    pGift->duration = ERRORCODE;
    pGift->tick     = ERRORCODE;
    pGift->days     = ERRORCODE;
    pGift->send     = FALSE;
    pGift->starting = 0;
    pGift->ending   = 0;
  }

  RETURN_NULL();
}

void set_bounty_default( BOUNTY_DATA * pBounty )
{
  PUSH_FUNCTION( "set_bounty_default" );

  pBounty->mob       = NULL;
  pBounty->msg       = NULL;
  pBounty->chance    = ERRORCODE;
  pBounty->max       = ERRORCODE;
  pBounty->type      = ERRORCODE;
  pBounty->migration = ERRORCODE;
  pBounty->room      = ERRORCODE;
  pBounty->value     = 0;
  pBounty->occurred  = 0;
  pBounty->killed    = 0;
  pBounty->count     = 0;
  pBounty->lock      = FALSE;
  RETURN_NULL();
}

void set_immlist_default( IMMLIST_DATA * pImmlist )
{
  PUSH_FUNCTION( "set_immlist_default" );
  pImmlist->name        = NULL;
  pImmlist->description = str_dup( "" );
  pImmlist->level       = ERRORCODE;
  pImmlist->trust       = ERRORCODE;
  pImmlist->adviser     = FALSE;
  RETURN_NULL();
}

void set_sale_default( SALE_DATA * pSale )
{
  PUSH_FUNCTION( "set_sale_default" );
  pSale->obj     = NULL;
  pSale->visible = TRUE;
  pSale->cost    = ERRORCODE;
  pSale->times   = 0;
  pSale->gold    = 0;
  pSale->sold    = 0;
  RETURN_NULL();
}

void set_situs_default( SITUS_DATA * pSitus )
{
  PUSH_FUNCTION( "set_situs_default" );

  pSitus->name           = str_dup( "" );
  pSitus->eq_name        = str_dup( "" );
  pSitus->type           = str_dup( "" );
  pSitus->situs_name     = str_dup( "" );
  pSitus->short_descr    = str_dup( "" );
  pSitus->msg1           = str_dup( "" );
  pSitus->msg2           = str_dup( "" );
  pSitus->associate      = NULL;
  pSitus->attack         = ERRORCODE;
  pSitus->location       = ERRORCODE;
  pSitus->chance         = ERRORCODE;
  pSitus->associate_vnum = ERRORCODE;
  pSitus->item_type      = ERRORCODE;
  pSitus->wear           = ERRORCODE;
  pSitus->apply_ac       = ERRORCODE;
  pSitus->wield          = FALSE;
  RETURN_NULL();
}

void set_ship_default( SHIP_DATA * pShip )
{
  PUSH_FUNCTION( "set_ship_default" );
  pShip->name         = NULL;
  pShip->msg_land     = NULL;
  pShip->msg_entrance = NULL;
  pShip->description  = NULL;
  pShip->starting     = NULL;
  pShip->destination  = NULL;
  pShip->cabin        = NULL;
  pShip->cost         = ERRORCODE;
  pShip->sailing      = ERRORCODE;
  pShip->waiting      = ERRORCODE;
  pShip->sailing_tick = ERRORCODE;
  pShip->waiting_tick = ERRORCODE;
  pShip->count        = 0;
  pShip->pirate       = 0;
  pShip->pirate_count = 0;
  pShip->delay        = 0;
  pShip->lock         = FALSE;
  RETURN_NULL();
}

void set_effect_default( EFFECT_DATA * pEffect )
{
  int loop;

  PUSH_FUNCTION( "set_effect_default" );

  pEffect->next      = NULL;
  pEffect->type      = EFFECT_NONE;

  for ( loop = 0; loop < MAX_EFFECT_VALUE; loop++ ) pEffect->value[loop] = 0;
  RETURN_NULL();
}

void set_message_default( MESSAGE_DATA * pMessage )
{
  PUSH_FUNCTION( "set_message_default" );

  pMessage->type   = ERRORCODE;
  pMessage->self   = NULL;
  pMessage->others = NULL;

  RETURN_NULL();
}

void set_angel_default( ANGEL_DATA * pAngel )
{
  PUSH_FUNCTION( "set_angel_default" );

  pAngel->lower       = 0;
  pAngel->higher      = AngelLevel;
  pAngel->position    = POS_STANDING;
  pAngel->description = NULL;
  pAngel->function    = NULL;
  RETURN_NULL();
}

void set_vote_default( VOTE_DATA * pVote )
{
  int loop;

  PUSH_FUNCTION( "set_vote_default" );

  if ( !pVote )
  {
    mudlog( LOG_DEBUG, "set_vote_default﹕缺乏來源。" );
    RETURN_NULL();
  }

  pVote->next    = NULL;
  pVote->poster  = str_dup( "" );
  pVote->subject = str_dup( "" );
  pVote->text    = str_dup( "" );
  pVote->date    = str_dup( "" );
  pVote->club    = str_dup( "" );
  pVote->days    = VOTE_DAYS;
  pVote->stamp   = 0;
  pVote->lock    = FALSE;
  pVote->level   = VOTE_LEVEL;
  pVote->moninal = TRUE;

  for ( loop = 0; loop < MAX_VOTES; loop++ )
  {
    pVote->poll[loop]    = 0;
    pVote->message[loop] = str_dup( "" );
  }

  for ( loop = 0; loop < MAX_POLL; loop++ )
  {
    pVote->poller[loop] = str_dup( "" );
    pVote->vote[loop]   = -1;
  }

  RETURN_NULL();
}

void set_joke_default( JOKE_DATA * pJoke )
{
  PUSH_FUNCTION( "set_joke_default" );

  pJoke->next  = NULL;
  pJoke->stamp = ERRORCODE;
  pJoke->title = NULL;
  pJoke->org   = NULL;
  pJoke->text  = NULL;

  RETURN_NULL();
}

void set_server_default( SERVER_DATA * pServer )
{
  PUSH_FUNCTION( "set_server_default" );

  pServer->address = NULL;
  RETURN_NULL();
}

void set_note_default( NOTE_DATA * pNote )
{
  PUSH_FUNCTION( "set_note_default" );

  pNote->filename   = str_dup( "" );
  pNote->sender     = NULL;
  pNote->date       = NULL;
  pNote->to_list    = NULL;
  pNote->subject    = NULL;
  pNote->text       = NULL;
  pNote->date_stamp = ERRORCODE;
  pNote->mark       = FALSE;
  RETURN_NULL();
}

void set_database_default( DATABASE_DATA * pData )
{
  PUSH_FUNCTION( "set_database_default" );

  pData->serial.high = 0;
  pData->serial.low  = 0;
  pData->name        = str_dup( "" );
  pData->exist       = FALSE;
  RETURN_NULL();
}

/* 載入新格式的區域檔的區域格式 */
AREA_DATA * load_area( char * filename )
{
  AREA_DATA * pArea;
  AREA_DATA * zArea;
  FILE_DATA * pFile;
  char      * word;
  bool        fMatch;

  PUSH_FUNCTION( "load_area" );

  if ( !( pFile = f_open( filename , "r" ) ) ) RETURN( NULL );

  /* 配置記憶體以及設定預設值 */
  set_area_default( pArea = alloc_struct( STRUCT_AREA_DATA ) );

  /* 載入區域的主程式部份 */
  for ( ;; )
  {
    word   = if_eof( pFile ) ? "End" : fread_word( pFile );
    fMatch = FALSE;

    switch ( UPPER(word[0]) )
    {
    /* 註解 */
    case '*':

      fMatch = TRUE;
      fread_to_eol( pFile );
      break;

    case 'C':
      KEY( "Capital", pArea->capital_no, fread_number( pFile ) );
      break;

    case 'D':

      if ( !str_scmp( word, "Description", &fMatch ) )
      {
        fread_to_eol( pFile ); /* 一定先讀完一行 */
        pArea->description = fread_string( pFile );
        break;
      }

      break;

    case 'E':

      if ( !str_scmp( word, "End", &fMatch ) )
      {
        /* 檢查是否有錯誤 */
        if ( !pArea->name )
        {
          mudlog( LOG_DEBUG , "Load_area﹕檔案 %s 缺乏區域名字。", filename );
          break;
        }

        if ( !pArea->editor )
        {
          mudlog( LOG_DEBUG , "Load_area﹕檔案 %s 缺乏區域撰寫者。", filename );
          break;
        }

        if ( IS_ERROR( pArea->serial ) )
        {
          mudlog( LOG_DEBUG, "Load_area﹕區域 %s 缺乏序號。", pArea->name );
          break;
        }

        if ( !pArea->description ) pArea->description = str_dup( "" );

        f_close( pFile );
        RETURN( pArea );
      }

      KEY( "Echo",   pArea->attribute , fread_number( pFile ) );
      KEY( "Editor", pArea->editor    , fread_string( pFile ) );
      break;

    case 'F':
      KEY( "Fight",   pArea->fight,   fread_number( pFile ) );
      KEY( "Fog",     pArea->foggy,   fread_number( pFile ) );
      break;

    case 'L':

      KEY( "Lower",   pArea->lower , fread_number( pFile ) );
      break;

    case 'N':

      KEY( "Name"   , pArea->name   , fread_string( pFile ) );
      KEY( "NewHand", pArea->newhand, fread_number( pFile ) );
      break;

    case 'P':
      KEY( "PK", pArea->pk, fread_number( pFile ) );
      break;

    case 'S':
      if ( !str_scmp( word, "Serial", &fMatch ) )
      {
        int serial;

        serial = fread_number( pFile );

        for ( zArea = area_first; zArea; zArea = zArea->next )
        {
          if ( zArea->serial == serial )
             mudlog( LOG_DEBUG, "Load_area﹕區域序號 %d 重複。", serial );
        }

        pArea->serial = serial;
        break;
      }
      break;

    case 'U':

      KEY( "Upper",   pArea->upper , fread_number( pFile ) );
      break;
    }

    /* 指令型態錯誤 */
    if ( !fMatch )
      mudlog( LOG_DEBUG , "Load_Area﹕檔案命令 %s 錯誤。" , word );
  }
  RETURN( NULL );
}

/* 載入新格式的區域檔的怪物格式 */
MOB_INDEX_DATA * load_mobiles( const char * filename )
{
  MOB_INDEX_DATA * pMobIndex;
  MPROG_DATA     * pProg;
  SKILL_DATA     * pSkill;
  AFFECT_DATA    * pAffect;
  AFFECT_DATA    * zAffect;
  FILE_DATA      * pFile;
  TEACH_DATA     * teach;
  char           * word;
  char           * name;
  bool             fMatch;

  PUSH_FUNCTION( "load_mobiles" );

  if ( !( pFile = f_open( filename , "r" ) ) )
  {
    mudlog( LOG_FAILLOAD , "Load_mobiles﹕載入怪物檔案 %s 失敗。\n" , filename );
    RETURN( NULL );
  }

  /* 預設一些數值 */
  set_mobile_default( pMobIndex = alloc_struct( STRUCT_MOB_INDEX_DATA ) );

  for ( ;; )
  {
    word   = if_eof( pFile ) ? "End" : fread_word( pFile );
    fMatch = FALSE;

    switch ( UPPER(word[0]) )
    {
    /* 註解 */
    case '*':

      fMatch = TRUE;
      fread_to_eol( pFile );
      break;

    case '#':

      if ( !str_scmp( word, "#Job", &fMatch ) )
      {
        JOB_DATA * pJob;

        if ( !( pJob = load_job( pFile ) ) )
        {
          mudlog( LOG_DEBUG, "Load_mobile﹕載入怪物特殊函數失敗。" );
          break;
        }

        pJob->next     = pMobIndex->job;
        pMobIndex->job = pJob;
        break;
      }

      if ( !str_scmp( word , "#Learn", &fMatch ) )
      {
        if ( !( teach = load_mob_teach( pFile ) ) )
        {
          mudlog( LOG_DEBUG, "Load_mobile﹕載入怪物教導失敗。" );
          break;
        }

        teach->next      = pMobIndex->teach;
        pMobIndex->teach = teach;
        if ( !fReload ) top_teach++;
        break;
      }

      break;

    case 'A':

      KEY( "Alignment"   , pMobIndex->alignment   , fread_number( pFile ) );
      KEY( "AttackRatio" , pMobIndex->attack_ratio, fread_number( pFile ) );
      KEY( "Aggressive"  , pMobIndex->Aggressive  , fread_number( pFile ) );
      KEY( "Ask"         , pMobIndex->Ask         , fread_number( pFile ) );
      KEY( "AutoSetValue", pMobIndex->AutoSetValue, fread_number( pFile ) );
      KEY( "AlignGood"   , pMobIndex->AlignGood   , fread_number( pFile ) );
      KEY( "AlignEvil"   , pMobIndex->AlignEvil   , fread_number( pFile ) );

      if ( !str_scmp( word , "Auction", &fMatch ) )
      {
        fread_to_eol( pFile ); /* 一定先讀完一行 */
        pMobIndex->auction = fread_string( pFile );
        break;
      }

      if ( !str_scmp( word, "AutoEnable", &fMatch ) )
      {
        int           practice;
        char        * word;
        ENABLE_DATA * pEnable;
        ENABLE_DATA * zEnable;
        SKILL_DATA  * pSkill;

        /* 如果技能已經不存在 */
        word = fread_word( pFile );
        TEST_READ_ERROR

        if ( !( pSkill = skill_isname( word ) ) )
        {
          mudlog( LOG_DEBUG , "Load_mobiles﹕怪物技能致能錯誤。" );
          break;
        }

        /* 看看這個技能是否可以被致能 */
        if ( !pSkill->enable )
        {
          mudlog( LOG_DEBUG, "Load_mobiles﹕怪物致能 %s 是不能致能的。"
            , pSkill->cname );
          break;
        }

        /* 如果重複 */
        if ( ( enable_repeat( pMobIndex->enable , pSkill ) ) )
        {
          mudlog( LOG_DEBUG , "Load_mobiles﹕怪物技能致能重複。" );
          break;
        }

        if ( ( practice = get_adeptation( pSkill, pMobIndex ) ) < 0 )
        {
          mudlog( LOG_DEBUG , "Load_mobiles﹕技能 %s 無法自動致能。"
            , pSkill->name );
          break;
        }

        if ( practice >= 100 )
        {
          mudlog( LOG_FAILENABLE, "怪物編號 %d 技能 %s 太差﹐建議換掉。"
            , pMobIndex->vnum, pSkill->name );

          practice = 100;
        }

        if ( practice <= 1 )
        {
          mudlog( LOG_FAILENABLE, "怪物編號 %d 技能 %s 太高﹐建議換掉。"
            , pMobIndex->vnum, pSkill->name );

          practice = 1;
        }

        /* 設定熟練度 */
        pMobIndex->skill[pSkill->slot] = practice;

        /* 如果存在才配置記憶體給他 */
        pEnable        = alloc_struct( STRUCT_ENABLE_DATA );
        pEnable->skill = pSkill;
        pEnable->next  = NULL;

        /* 找出最後一個 */
        for ( zEnable = pMobIndex->enable;
              zEnable && zEnable->next;
              zEnable = zEnable->next );

        if ( !zEnable ) pMobIndex->enable = pEnable;
        else            zEnable->next     = pEnable;

        break;
      }

      break;

    case 'C':

      if ( !str_scmp( word , "Class", &fMatch ) )
      {
        CLASS_DATA * pClass;
        int          vnum;

        vnum = fread_number( pFile );
        TEST_READ_ERROR

        if ( !( pClass = class_lookup( vnum ) ) )
        {
          mudlog( LOG_DEBUG, "Load_mobiles﹕號碼 %d 的怪物職業 %d 從缺。"
            , pMobIndex->vnum , vnum );

          break;
        }

        pMobIndex->class = pClass;
        break;
      }

      KEY( "Con", pMobIndex->now_con , fread_number( pFile ) );
      break;

    case 'D':

      KEY( "DodgeRatio", pMobIndex->dodge_ratio, fread_number( pFile ) );
      KEY( "Damroll"   , pMobIndex->damroll    , fread_number( pFile ) );
      KEY( "Dex"       , pMobIndex->now_dex    , fread_number( pFile ) );

      if ( !str_scmp( word , "Description", &fMatch ) )
      {
        fread_to_eol( pFile ); /* 一定先讀完一行 */
        pMobIndex->description = fread_string( pFile );
        break;
      }

      if ( !str_scmp( word , "Deadmsg", &fMatch ) )
      {
        fread_to_eol( pFile ); /* 一定先讀完一行 */
        pMobIndex->deadmsg = fread_string( pFile );
        break;
      }

      break;

    case 'E':

      if ( !str_scmp( word, "Enable", &fMatch ) )
      {
        int           practice;
        char        * word;
        ENABLE_DATA * pEnable;
        ENABLE_DATA * zEnable;
        SKILL_DATA  * pSkill;

        /* 先讀入熟練度 */
        practice = fread_number( pFile );
        TEST_READ_ERROR

        if ( practice > 100 || practice <= 0 )
        {
          mudlog( LOG_DEBUG, "Load_mobiles﹕怪物技能熟練度有問題。" );
          break;
        }

        /* 如果技能已經不存在 */
        word = fread_word( pFile );
        TEST_READ_ERROR

        if ( !( pSkill = skill_isname( word ) ) )
        {
          mudlog( LOG_DEBUG , "Load_mobiles﹕怪物技能致能錯誤。" );
          break;
        }

        /* 看看這個技能是否可以被致能 */
        if ( !pSkill->enable )
        {
          mudlog( LOG_DEBUG, "Load_mobiles﹕怪物致能 %s 是不能致能的。"
            , pSkill->cname );
          break;
        }

        /* 如果重複 */
        if ( ( enable_repeat( pMobIndex->enable , pSkill ) ) )
        {
          mudlog( LOG_DEBUG , "Load_mobiles﹕怪物技能致能重複。" );
          break;
        }

        /* 設定熟練度 */
        pMobIndex->skill[pSkill->slot] = practice;

        /* 如果存在才配置記憶體給他 */
        pEnable        = alloc_struct( STRUCT_ENABLE_DATA );
        pEnable->skill = pSkill;
        pEnable->next  = NULL;

        /* 找出最後一個 */
        for ( zEnable = pMobIndex->enable;
              zEnable && zEnable->next;
              zEnable = zEnable->next );

        if ( !zEnable ) pMobIndex->enable = pEnable;
        else            zEnable->next     = pEnable;

        break;
      }

      if ( !str_scmp( word, "Effect", &fMatch ) )
      {
        name = fread_word( pFile );
        TEST_READ_ERROR

        if ( !( pSkill = skill_isname( name ) ) )
        {
          mudlog( LOG_DEBUG, "Load_mobiles﹕怪物 %d 沒有技能 %s。"
            , pMobIndex->vnum, name );
          break;
        }

        if ( !pSkill->affected )
        {
          mudlog( LOG_DEBUG, "Load_mobiles﹕怪物 %d 技能 %s 不能成效應。"
            , pMobIndex->vnum, pSkill->name );
          break;
        }

        for ( pAffect = pMobIndex->affected; pAffect; pAffect = pAffect->next )
        {
          if ( pAffect->type == pSkill->slot )
          {
            mudlog( LOG_DEBUG, "Load_mobiles﹕怪物 %d 效應 %s 重覆。"
              , pMobIndex->vnum, pSkill->name );
            break;
          }
        }

        TEST_READ_ERROR

        pAffect            = alloc_struct( STRUCT_AFFECT_DATA );
        pAffect->type      = pSkill->slot;
        pAffect->level     = MAX_LEVEL;
        pAffect->location  = 0;
        pAffect->modifier  = 0;
        pAffect->duration  = -1;

        /* 找出最後一個 */
        for ( zAffect = pMobIndex->affected;
              zAffect && zAffect->next;
              zAffect = zAffect->next );

        if ( !zAffect ) pMobIndex->affected = pAffect;
        else            zAffect->next       = pAffect;

        break;
      }

      if ( !str_scmp( word, "End", &fMatch ) )
      {
        int vnum = pMobIndex->vnum;

        if ( IS_ERROR( vnum ) )
        {
          mudlog( LOG_DEBUG , "Load_mobiles﹕怪物沒有號碼。" );
          break;
        }

        if ( !pMobIndex->name )
        {
          mudlog( LOG_DEBUG , "Load_mobiles﹕怪物 %d 沒有名字。", vnum );
          break;
        }

        if ( !pMobIndex->short_descr )
        {
          mudlog( LOG_DEBUG , "Load_mobiles﹕怪物 %d 沒有短描述。", vnum );
          break;
        }

        if ( !pMobIndex->description )
        {
          mudlog( LOG_DEBUG , "Load_mobiles﹕怪物 %d 沒有描述。", vnum );
          break;
        }

        pMobIndex->act = ACT_IS_NPC;
        if ( pMobIndex->Sentinel     ) pMobIndex->act |= ACT_SENTINEL;
        if ( pMobIndex->Scavenger    ) pMobIndex->act |= ACT_SCAVENGER;
        if ( pMobIndex->Aggressive   ) pMobIndex->act |= ACT_AGGRESSIVE;
        if ( pMobIndex->StayArea     ) pMobIndex->act |= ACT_STAY_AREA;
        if ( pMobIndex->Wimpy        ) pMobIndex->act |= ACT_WIMPY;
        if ( pMobIndex->Pet          ) pMobIndex->act |= ACT_PET;
        if ( pMobIndex->Train        ) pMobIndex->act |= ACT_TRAIN;
        if ( pMobIndex->Practice     ) pMobIndex->act |= ACT_PRACTICE;
        if ( pMobIndex->Rebirth      ) pMobIndex->act |= ACT_REBIRTH;
        if ( pMobIndex->RebornFight  ) pMobIndex->act |= ACT_REBORN_FIGHT;
        if ( pMobIndex->Ask          ) pMobIndex->act |= ACT_ASK;
        if ( pMobIndex->AutoSetValue ) pMobIndex->act |= AUTOSET_VALUE;
        if ( pMobIndex->NoReborn     ) pMobIndex->act |= ACT_NOREBORN;
        if ( pMobIndex->NoKill       ) pMobIndex->act |= ACT_NOKILL;
        if ( pMobIndex->Enroll       ) pMobIndex->act |= ACT_ENROLL;
        if ( pMobIndex->AlignGood    ) pMobIndex->act |= ACT_ALIGN_GOOD;
        if ( pMobIndex->AlignEvil    ) pMobIndex->act |= ACT_ALIGN_EVIL;
        if ( pMobIndex->NoSummon     ) pMobIndex->act |= ACT_NOSUMMON;

        pMobIndex->filename = str_dup( filename );
        f_close( pFile );
        RETURN( pMobIndex );
      }

      KEY( "Enroll", pMobIndex->Enroll, fread_number( pFile ) );

      break;

    case 'G':

      KEY( "Gold" , pMobIndex->gold, fread_number( pFile ) );
      break;

    case 'H':

      KEY( "Hitroll", pMobIndex->hitroll, fread_number( pFile ) );
      KEY( "Hp"     , pMobIndex->hit    , fread_number( pFile ) );
      break;

    case 'I':

      KEY( "Int", pMobIndex->now_int   , fread_number( pFile ) );
      break;

    case 'L':

      if ( !str_scmp( word, "Level", &fMatch ) )
      {
        int level;

        level = fread_number( pFile );
        TEST_READ_ERROR

        if ( level <= 0 || level > MAX_LEVEL )
        {
          mudlog( LOG_DEBUG , "Load_mobiles﹕怪物 %d 等級 %d 不合理。"
            , pMobIndex->vnum, level );

          break;
        }

        pMobIndex->level = level;
        break;
      }

      if ( !str_scmp( word, "LongDesc", &fMatch ) )
      {
        fread_string( pFile );
        break;
      }

      break;

    case 'M':

      KEY( "Mana"     , pMobIndex->mana     , fread_number( pFile ) );
      KEY( "Move"     , pMobIndex->move     , fread_number( pFile ) );
      KEY( "Multipile", pMobIndex->multipile, fread_number( pFile ) );
      KEY( "Migrate"  , pMobIndex->migrate  , fread_number( pFile ) );
      break;

    case 'N':

      KEY( "Name"    , pMobIndex->name    , fread_string( pFile ) );
      KEY( "NoReborn", pMobIndex->NoReborn, fread_number( pFile ) );
      KEY( "NoKill"  , pMobIndex->NoKill  , fread_number( pFile ) );
      KEY( "NoSummon", pMobIndex->NoSummon, fread_number( pFile ) );
      break;

    case 'P':

      KEY( "Protect" , pMobIndex->ac      , fread_number( pFile ) );
      KEY( "Pet"     , pMobIndex->Pet     , fread_number( pFile ) );
      KEY( "Practice", pMobIndex->Practice, fread_number( pFile ) );

      if ( !str_scmp( word, "Process", &fMatch ) )
      {
        fread_to_eol( pFile );

        if ( !( pProg = load_program( pFile ) ) )
        {
          mudlog( LOG_DEBUG, "Load_mobile﹕載入怪物程式錯誤。" );
          break;
        }

        pProg->next           = pMobIndex->mobprogs;
        pMobIndex->mobprogs   = pProg;
        pMobIndex->progtypes |= pProg->type;
        break;
      }

      break;

    case 'R':

      KEY( "Reborn"     , pMobIndex->reborn_vnum, fread_number( pFile ) );
      KEY( "Rebornroom" , pMobIndex->reborn_room, fread_number( pFile ) );
      KEY( "Rebirth"    , pMobIndex->Rebirth    , fread_number( pFile ) );
      KEY( "RebornFight", pMobIndex->RebornFight, fread_number( pFile ) );
      break;

    case 'S':

      KEY( "ShortDesc" , pMobIndex->short_descr , fread_string( pFile ) );
      KEY( "Str"       , pMobIndex->now_str     , fread_number( pFile ) );
      KEY( "Sentinel"  , pMobIndex->Sentinel    , fread_number( pFile ) );
      KEY( "Scavenger" , pMobIndex->Scavenger   , fread_number( pFile ) );
      KEY( "StayArea"  , pMobIndex->StayArea    , fread_number( pFile ) );

      if ( !str_scmp( word, "Sex", &fMatch ) )
      {
        switch( pMobIndex->sex = fread_number( pFile ) )
        {
        default:
          mudlog( LOG_DEBUG, "Load_mobiles﹕怪物性別 %d 不合理。", pMobIndex->sex );
          break;

        case SEX_MALE:
        case SEX_FEMALE:
        case SEX_NEUTRAL:
          break;
        }
        break;

      }

      if ( !str_scmp( word, "Special", &fMatch ) )
      {
        char * word;

        word = fread_word( pFile );
        TEST_READ_ERROR

        if ( !( pMobIndex->spec_fun = spec_lookup( word ) ) )
        {
          mudlog( LOG_DEBUG , "Load_mobiles﹕特殊函數 %s 不存在。", word );
          break;
        }

        break;
      }

      if ( !str_scmp( word , "Speak", &fMatch ) )
      {
        pMobIndex->speak = ( fread_number( pFile ) == TRUE ) ? TRUE : FALSE;
        break;
      }

      break;

    case 'T':

      KEY( "Tractable", pMobIndex->tractable, fread_number( pFile ) );
      KEY( "Train"    , pMobIndex->Train    , fread_number( pFile ) );
      break;

    case 'V':

      KEY( "Vnum", pMobIndex->vnum, fread_number( pFile ) );
      break;

    case 'W':

      KEY( "Wis"  , pMobIndex->now_wis , fread_number( pFile ) );
      KEY( "Wimpy", pMobIndex->Wimpy   , fread_number( pFile ) );
      break;
    }

    /* 指令型態錯誤 */
    if ( !fMatch || FileError )
    {
      if ( !fMatch ) mudlog( LOG_DEBUG , "Load_mobile﹕命令 %s 不正確。" , word );
      if ( fReload )
      {
        free_mob_index( pMobIndex );
        f_close( pFile );
        RETURN( NULL );
      }
    }
  }

  RETURN( NULL );
}

/* 載入怪物教導結構 */
TEACH_DATA * load_mob_teach( FILE_DATA * pFile )
{
  TEACH_DATA * teach;
  char       * word;
  bool         fMatch;

  PUSH_FUNCTION( "load_mob_teach" );

  /* 配置教導所需要的記憶體以及設定預設值 */
  set_teach_default( teach = alloc_struct( STRUCT_TEACH_DATA ) );

  for ( ;; )
  {
    word   = if_eof( pFile ) ? "End" : fread_word( pFile );
    fMatch = FALSE;

    switch ( UPPER(word[0]) )
    {
    case '*':
      fMatch = TRUE;
      fread_to_eol( pFile );
      break;

    case 'A':

      if ( !str_scmp( word, "Adept", &fMatch ) )
      {
        teach->adept = fread_number( pFile );
        TEST_READ_ERROR

        if ( teach->adept < 0 || teach->adept > 100 )
        {
          mudlog( LOG_DEBUG, "Load_mob_teach﹕法術熟練度 %d 不合理。"
            , teach->adept );
          break;
        }

        break;
      }

      break;

    case 'C':

      if ( !str_scmp( word , "Cost", &fMatch ) )
      {
        teach->cost = fread_number( pFile );
        TEST_READ_ERROR

        if ( teach->cost < 0 )
        {
          mudlog( LOG_DEBUG, "Load_mob_teach﹕法術教授金 %d 不合理。"
            , teach->cost );
          break;
        }

        break;
      }

      break;

    case 'E':

      if ( !str_scmp( word, "End", &fMatch ) )
      {
        if ( IS_ERROR( teach->slot ) )
        {
          mudlog( LOG_DEBUG, "Load_mob_teach﹕沒有法術號碼。" );
          break;
        }

        RETURN( teach );
      }

      break;

    case 'I':

      if ( !str_scmp( word , "Inventory", &fMatch ) )
      {
        teach->inventory = fread_number( pFile );
        TEST_READ_ERROR

        if ( teach->inventory < 0 || teach->inventory > 100 )
        {
          mudlog( LOG_DEBUG, "Load_mob_teach﹕法術隱藏旗標不對。" );
          break;
        }

        break;
      }

      break;

    case 'N':

      if ( !str_scmp( word, "Name", &fMatch ) )
      {
        char       * name_string;
        SKILL_DATA * pSkill;

        name_string = fread_word( pFile );
        TEST_READ_ERROR

        if ( !( pSkill = skill_isname( name_string ) ) )
        {
          mudlog( LOG_DEBUG , "Load_mob_teach﹕法術 %s 不對。", name_string );
          break;
        }

        teach->slot = pSkill->slot;
        break;
      }
      break;
    }

    /* 指令型態錯誤 */
    if ( !fMatch || FileError )
    {
      if ( !fMatch ) mudlog( LOG_DEBUG , "Load_mob_teach﹕命令 %s 不正確。" , word );
      if ( fReload )
      {
        free_teach( teach );
        RETURN( NULL );
      }
    }
  }

  RETURN( NULL );
}

/* 載入新格式的區域檔的房間格式 */
ROOM_INDEX_DATA * load_room( const char * filename )
{
  ROOM_INDEX_DATA  * pRoomIndex;
  EXTRA_DESCR_DATA * pExtra;
  EXIT_DATA        * pExit;
  FILE_DATA        * pFile;
  char             * word;
  bool               fMatch;

  PUSH_FUNCTION( "load_room" );

  if ( !( pFile = f_open( filename , "r" ) ) )
  {
    mudlog( LOG_FAILLOAD , "載入房間檔案 %s 失敗。\n" , filename );
    RETURN( NULL );
  }

  /* 配置房間空間以及設定預設值 */
  set_room_default( pRoomIndex = alloc_struct( STRUCT_ROOM_INDEX_DATA ) );

  /* 正式載入區域檔中的房間 */
  for ( ;; )
  {
    word   = if_eof( pFile ) ? "End" : fread_word( pFile );
    fMatch = FALSE;

    switch ( UPPER(word[0]) )
    {
    case '*':
      fMatch = TRUE;
      fread_to_eol( pFile );
      break;

    case '#':

      if ( !str_scmp( word , "#Exit", &fMatch ) )
      {
        /* 避免沒有房間號碼就載入出口 */
        if ( IS_ERROR( pRoomIndex->vnum ) )
          mudlog( LOG_DEBUG , "Load_room﹕房間號碼還沒設定, 無法設定出口。" );

        /* 載入房間出口格式 */
        pExit = load_exit( pFile );

        pRoomIndex->exit[pExit->direction] = pExit;
        if ( !fReload ) top_exit++;

        break;
      }

      /* 載入房間的關鍵字部份 */
      if ( !str_scmp( word , "#Keyword", &fMatch ) )
      {
        /* 避免沒有房間號碼就載入關鍵字 */
        if ( IS_ERROR( pRoomIndex->vnum ) )
          mudlog( LOG_DEBUG , "Load_room﹕房間號碼還沒設定, 無法設定關鍵字。" );

        if ( !( pExtra = load_extra_descr( pFile ) ) )
        {
          mudlog( LOG_DEBUG, "Load_room﹕房間額外描述載入失敗。" );
          break;
        }

        pExtra->next            = pRoomIndex->extra_descr;
        pRoomIndex->extra_descr = pExtra;
        if ( !fReload ) top_ed++;
        break;
      }

      if ( !str_scmp( word, "#Job", &fMatch ) )
      {
        JOB_DATA * pJob;

        if ( !( pJob = load_job( pFile ) ) )
        {
          mudlog( LOG_DEBUG, "Load_room﹕載入房間特殊函數失敗。" );
          break;
        }

        pJob->next      = pRoomIndex->job;
        pRoomIndex->job = pJob;
        break;
      }

      if ( !str_scmp( word, "#Enquire", &fMatch ) )
      {
        ENQUIRE_DATA * pEnquire;

        pEnquire            = load_enquire( pFile );
        pEnquire->next      = pRoomIndex->enquire;
        pRoomIndex->enquire = pEnquire;
        break;
      }

      break;

    case 'C':

      KEY( "Club", pRoomIndex->Club, fread_number( pFile ) );
      KEY( "Cost", pRoomIndex->cost, fread_number( pFile ) );
      break;

    case 'D':

      KEY( "Dark"        , pRoomIndex->Dark        , fread_number( pFile ) );
      KEY( "DepositMoney", pRoomIndex->DepositMoney, fread_number( pFile ) );

      if ( !str_scmp( word , "Description", &fMatch ) )
      {
        fread_to_eol( pFile ); /* 一定先讀完一行 */
        pRoomIndex->description = fread_string( pFile );
        break;
      }
      break;

    case 'E':

      if ( !str_scmp( word, "End", &fMatch ) )
      {
        /* 避免有些重要值尚未載入就結束 */
        if ( !pRoomIndex->name )
          mudlog( LOG_DEBUG , "Load_room﹕房間沒有名稱。" );

        if ( !pRoomIndex->description )
          mudlog( LOG_DEBUG , "Load_room﹕房間沒有描述。" );

        if ( IS_ERROR( pRoomIndex->vnum ) )
          mudlog( LOG_DEBUG , "Load_room﹕房間沒有號碼。" );

        if ( !pRoomIndex->sector )
          mudlog( LOG_DEBUG , "Load_room﹕房間沒有地形。" );

        f_close( pFile );
        RETURN( pRoomIndex );
      }

      break;

    case 'F':

      KEY( "ForeverLight", pRoomIndex->ForeverLight, fread_number( pFile ) );
      KEY( "Fane"        , pRoomIndex->Fane        , fread_number( pFile ) );
      KEY( "Foodful"     , pRoomIndex->Foodful     , fread_number( pFile ) );
      break;

    case 'H':
      KEY( "HitCure", pRoomIndex->HitCure, fread_number( pFile ) );
      break;

    case 'I':

      KEY( "Indoors", pRoomIndex->Indoors, fread_number( pFile ) );
      break;

    case 'K':

      KEY( "Killer", pRoomIndex->Killer, fread_number( pFile ) );
      break;

    case 'M':

      KEY( "Memorize", pRoomIndex->Memorize, fread_number( pFile ) );
      KEY( "ManaCure", pRoomIndex->ManaCure, fread_number( pFile ) );
      KEY( "MoveCure", pRoomIndex->MoveCure, fread_number( pFile ) );
      break;

    case 'N':

      KEY( "Name"    , pRoomIndex->name    , fread_string( pFile ) );
      KEY( "NoMob"   , pRoomIndex->NoMob   , fread_number( pFile ) );
      KEY( "NoRecall", pRoomIndex->NoRecall, fread_number( pFile ) );
      KEY( "NoFight" , pRoomIndex->NoFight , fread_number( pFile ) );
      KEY( "NoQuit"  , pRoomIndex->NoQuit  , fread_number( pFile ) );
      KEY( "NoWhere" , pRoomIndex->NoWhere , fread_number( pFile ) );
      break;

    case 'O':

      if ( !str_scmp( word, "Owner", &fMatch ) )
      {
        pRoomIndex->owner.high = fread_number( pFile );
        pRoomIndex->owner.low  = fread_number( pFile );
        break;
      }

      break;

    case 'P':

      KEY( "Private",  pRoomIndex->Private,  fread_number( pFile ) );
      KEY( "Player" ,  pRoomIndex->player ,  fread_number( pFile ) );
      KEY( "PracRoom", pRoomIndex->PracRoom, fread_number( pFile ) );
      break;

    case 'S':

      KEY( "Sail"     , pRoomIndex->Sail     , fread_number( pFile ) );
      KEY( "Standard" , pRoomIndex->Standard , fread_number( pFile ) );
      KEY( "Stock"    , pRoomIndex->Stock    , fread_number( pFile ) );
      KEY( "StoreRoom", pRoomIndex->StoreRoom, fread_number( pFile ) );
      KEY( "Safe"     , pRoomIndex->Safe     , fread_number( pFile ) );

      if ( !str_scmp( word , "SectorType", &fMatch ) )
      {
        int iSector;
        SECTOR_DATA * pSector;

        iSector = fread_number( pFile );
        if ( !( pSector = get_sector_index( iSector ) ) )
          mudlog( LOG_DEBUG , "Load_room﹕房間地型編號 %d 不存在。" , iSector );

        pRoomIndex->sector = pSector;
        pSector->count++;
        break;
      }

      break;

    case 'U':
      KEY( "Upgrade", pRoomIndex->Upgrade, fread_number( pFile ) );
      break;


    case 'W':

      KEY( "Writeable", pRoomIndex->writeable, fread_number( pFile ) );
      KEY( "Waterful" , pRoomIndex->Waterful,  fread_number( pFile ) );
      break;

    case 'V':

      if ( !str_scmp( word , "Vnum", &fMatch ) )
      {
        pRoomIndex->vnum = fread_number( pFile ) ;

        /* 避免房間號碼會有重複 */
        if ( get_room_index( pRoomIndex->vnum ) )
          mudlog( LOG_DEBUG , "Load_room﹕房間號碼 %d 重複。"
            , pRoomIndex->vnum );

        break;
      }

      break;
    }

    /* 指令型態錯誤 */

    if ( !fMatch )
      mudlog( LOG_DEBUG , "Load_room﹕命令 %s 不正確。" , word );
  }
}

/* 載入房間的出口 */
EXIT_DATA * load_exit( FILE_DATA * pFile )
{
  EXIT_DATA * pExit;
  char      * word;
  bool        fMatch;
  int         direction;

  PUSH_FUNCTION( "load_exit" );

  /* 配置記憶體以及設定預設值 */
  set_exit_default( pExit = alloc_struct( STRUCT_EXIT_DATA ) );

  /* 載入區域檔房間出口部份 */
  for ( direction = -1;; )
  {
    word   = if_eof( pFile ) ? "End" : fread_word( pFile );
    fMatch = FALSE;

    switch ( UPPER( word[0] ) )
    {
    case '*':
      fMatch = TRUE;
      fread_to_eol( pFile );
      break;

    case 'C':

      if ( !str_scmp( word, "Closed", &fMatch ) )
      {
        pExit->closed = fread_number( pFile );
        if ( pExit->closed ) pExit->isdoor = TRUE;
        break;
      }
      break;

    case 'D':

      if ( !str_scmp( word, "Direction", &fMatch ) )
      {
        switch( direction = fread_number( pFile ) )
        {
        default:
          mudlog( LOG_DEBUG, "Load_exit﹕錯誤的方向 %d。", direction );
          break;

        case DIR_NORTH:
        case DIR_EAST:
        case DIR_SOUTH:
        case DIR_WEST:
        case DIR_UP:
        case DIR_DOWN:
        case DIR_ENTER:
        case DIR_OUT:
          pExit->direction = direction;
          break;
        }

        break;
      }

      break;

    case 'E':

      if ( !str_scmp( word, "End" , &fMatch ) )
      {
        /* 避免有些重要值尚為讀取就離開 */
        if ( IS_ERROR( direction ) )
          mudlog( LOG_DEBUG , "Load_exit﹕出口方向沒有設定。" );

        if ( IS_ERROR( pExit->vnum ) )
          mudlog( LOG_DEBUG , "Load_exit﹕房間出口沒有號碼。" );

        if ( !pExit->description )
          mudlog( LOG_DEBUG , "Load_exit﹕房間出口沒有描述。" );

        if ( !pExit->keyword )
          mudlog( LOG_DEBUG , "Load_exit﹕房間出口沒有關鍵字。" );

        RETURN( pExit );
      }

      KEY( "ExitKey",  pExit->key  , fread_number( pFile ) );
      KEY( "ExitVnum", pExit->vnum , fread_number( pFile ) );

      if ( !str_scmp( word , "ExitKeyword", &fMatch ) )
      {
        pExit->keyword = fread_string( pFile );
        break;
      }

      if ( !str_scmp( word , "ExitDesc", &fMatch ) )
      {
        pExit->description = fread_string( pFile );
        break;
      }

      break;

    case 'I':
      KEY( "IsDoor", pExit->isdoor, fread_number( pFile ) );
      break;

    case 'L':
      if ( !str_scmp( word, "Locked", &fMatch ) )
      {
        pExit->locked = fread_number( pFile );
        if ( pExit->locked )
        {
          pExit->isdoor = TRUE;
          pExit->closed = TRUE;
        }

        break;
      }
      break;

    case 'M':
      KEY( "Message", pExit->message, fread_string( pFile ) );
      break;

    case 'P':

      if ( !str_scmp( word, "Pick", &fMatch ) )
      {
        pExit->pick = fread_number( pFile );
        if ( pExit->pick )
        {
          pExit->isdoor = TRUE;
          pExit->locked = TRUE;
          pExit->closed = TRUE;
        }

        break;
      }
      break;
    }

    /* 指令型態錯誤 */
    if ( !fMatch )
      mudlog( LOG_DEBUG , "Load_exit﹕命令 %s 不正確。" , word );
  }

  RETURN( NULL );
}

/* 載入房間的關鍵字 */
EXTRA_DESCR_DATA * load_extra_descr( FILE_DATA * pFile )
{
  EXTRA_DESCR_DATA * ed;
  char             * word;
  bool               fMatch;

  PUSH_FUNCTION( "load_extra_descr" );

  /* 配置記憶體 */
  ed = alloc_struct( STRUCT_EXTRA_DESCR_DATA );

  /* 設定預設值 */
  ed->keyword     = NULL;
  ed->description = NULL;

  /*載入區域檔房間的關鍵字部份 */

  for ( ;; )
  {
    word   = if_eof( pFile ) ? "End" : fread_word( pFile );
    fMatch = FALSE;

    switch ( UPPER(word[0]) )
    {
    case '*':

      fMatch = TRUE;
      fread_to_eol( pFile );
      break;

    case 'D':

      if ( !str_scmp( word , "Description", &fMatch ) )
      {
        fread_to_eol( pFile ); /* 一定先讀完一行 */
        ed->description = fread_string( pFile );
        break;
      }

      break;

    case 'E':

      if ( !str_scmp( word, "End", &fMatch ) )
      {
        if ( !ed->description )
        {
          mudlog( LOG_DEBUG , "Load_extra_descr﹕沒有描述。" );
          break;
        }

        if ( !ed->keyword )
        {
          mudlog( LOG_DEBUG , "Load_extra_descr﹕描述沒有關鍵字。" );
          break;
        }

        RETURN( ed );
      }

      break;

    case 'K':

      KEY( "Keyword", ed->keyword , fread_string( pFile ) );
      break;
    }

    /* 指令型態錯誤 */
    if ( !fMatch || FileError )
    {
      if ( !fMatch ) mudlog( LOG_DEBUG , "Load_extra_descr﹕命令 %s 不正確。" , word );
      if ( FileError )
      {
        free_extra_descr( ed );
        RETURN( NULL );
      }
    }
  }

  RETURN( NULL );
}

/* 載入新格式的商店格式 */
SHOP_DATA * load_shop( const char * filename )
{
  MOB_INDEX_DATA  * pMobIndex;
  MOB_INDEX_DATA  * pVictim;
  SHOP_DATA       * pShop;
  FILE_DATA       * pFile;
  char            * word;
  int               iTrade;
  bool              fMatch;

  PUSH_FUNCTION( "load_shop" );

  if ( !( pFile = f_open( filename , "r" ) ) )
  {
    mudlog( LOG_FAILLOAD , "載入商店檔案 %s 失敗。\n" , filename );
    RETURN( NULL );
  }

  /* 配置商店所需要的記憶體以及設定預設值 */
  set_shop_default( pShop = alloc_struct( STRUCT_SHOP_DATA ) );

  /* 載入區域檔裡面的商店部份 */
  for ( ;; )
  {
    word   = if_eof( pFile ) ? "End" : fread_word( pFile );
    fMatch = FALSE;

    switch ( UPPER(word[0]) )
    {
    case '*':
      fMatch = TRUE;
      fread_to_eol( pFile );
      break;

    case 'B':
      KEY( "BuyProfit", pShop->profit_buy , fread_number( pFile ) );
      break;

    case 'C':
      KEY( "CloseHour", pShop->close_hour , fread_number( pFile ) );
      break;

    case 'E':

      if ( !str_scmp( word, "End", &fMatch ) )
      {
        /* 避免沒有店長的錯誤 */
        if ( IS_ERROR( pShop->keeper ) )
          mudlog( LOG_DEBUG , "Load_shop﹕沒有載入店主。" );

        if ( IS_ERROR( pShop->type ) )
          mudlog( LOG_DEBUG, "Load_shop﹕沒有載入店家型態。" );

        if ( !( pMobIndex = get_mob_index( pShop->keeper ) ) )
          mudlog( LOG_DEBUG, "Load_shop﹕沒有店主號碼 %d。", pShop->keeper );

        if ( pMobIndex->pShop )
          mudlog( LOG_DEBUG, "Load_shop﹕怪物 %d 已有商店結構。"
            , pMobIndex->vnum );

        switch( pShop->type )
        {
        default:
          break;

        case SHOP_MERCENARY:

          for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
          {
            int slot;

            if ( ( slot = pShop->buy_type[iTrade] ) == 0 ) continue;

            if ( !( get_mob_index( slot ) ) )
              mudlog( LOG_DEBUG, "Load_shop﹕傭兵號碼 %d 不存在。", slot );
          }

          break;

        case SHOP_COPER:

          for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
          {
            int slot;

            if ( ( slot = pShop->buy_type[iTrade] ) == 0 ) continue;

            if ( !( pVictim = get_mob_index( slot ) ) )
              mudlog( LOG_DEBUG, "Load_shop﹕座騎號碼 %d 不存在。", slot );

            if ( pVictim->tractable <= 0 )
              mudlog( LOG_DEBUG, "Load_shop﹕號碼 %d 不是座騎。", slot );
          }

          break;
        }

        pMobIndex->pShop = pShop;
        f_close( pFile );
        RETURN( pShop );
      }

      break;

    case 'H':

      if ( !str_scmp( word , "Horse", &fMatch ) )
      {
        for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
        {
          if ( fread_if_eol( pFile ) ) break;
          pShop->buy_type[iTrade] = fread_number( pFile );
        }

        fread_to_eol( pFile );
        break;
      }

      break;

    case 'K':

      KEY( "Keeper", pShop->keeper , fread_number( pFile ) );
      break;

    case 'M':

      if ( !str_scmp( word , "Mercenary", &fMatch ) )
      {
        for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
        {
          if ( fread_if_eol( pFile ) ) break;
          pShop->buy_type[iTrade] = fread_number( pFile );
        }

        fread_to_eol( pFile );
        break;
      }

      break;

    case 'O':

      KEY( "OpenHour", pShop->open_hour   , fread_number( pFile ) );
      KEY( "Object1",  pShop->buy_type[0] , fread_number( pFile ) );
      KEY( "Object2",  pShop->buy_type[1] , fread_number( pFile ) );
      KEY( "Object3",  pShop->buy_type[2] , fread_number( pFile ) );
      KEY( "Object4",  pShop->buy_type[3] , fread_number( pFile ) );
      KEY( "Object5",  pShop->buy_type[4] , fread_number( pFile ) );

      if ( !str_scmp( word , "Object", &fMatch ) )
      {
        for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
        {
          if ( fread_if_eol( pFile ) ) break;
          pShop->buy_type[iTrade] = fread_number( pFile );
        }

        fread_to_eol( pFile );
        break;
      }

      break;

    case 'S':
      KEY( "SellProfit", pShop->profit_sell , fread_number( pFile ) );

      if ( !str_scmp( word, "Sential", &fMatch ) )
      {
        int               vnum;
        ROOM_INDEX_DATA * pRoomIndex;

        vnum = fread_number( pFile );
        if ( !( pRoomIndex = get_room_index( vnum ) ) )
          mudlog( LOG_DEBUG, "Load_shop﹕找不到店面房間號碼 %d。", vnum );

        pShop->sential = pRoomIndex;
        break;
      }
      break;

    case 'T':

      if ( !str_scmp( word , "Type", &fMatch ) )
      {
        switch( ( iTrade = fread_number( pFile ) ) )
        {
        default:
          mudlog( LOG_DEBUG, "Load_shop﹕型態 %d 錯誤。", iTrade );
          break;

        case SHOP_STORE:
        case SHOP_SMITH:
        case SHOP_MERCENARY:
        case SHOP_COPER:

          pShop->type = iTrade;
          break;
        }

        break;
      }
      break;
    }

    /* 指令型態錯誤 */
    if ( !fMatch )
      mudlog( LOG_DEBUG , "Load_shop﹕命令 %s 不正確。" , word );
  }

  RETURN( NULL );
}

/* 載入新格式的礦物格式 */
MINERAL_DATA * load_mineral( const char * filename )
{
  MINERAL_DATA * pMineral;
  FILE_DATA    * pFile;
  char         * word;
  bool           fMatch;

  PUSH_FUNCTION( "load_mineral" );

  if ( !( pFile = f_open( filename , "r" ) ) )
  {
    mudlog( LOG_FAILLOAD , "載入礦物檔案 %s 失敗。\n" , filename );
    RETURN( NULL );
  }

  /* 配置商店所需要的記憶體以及設定預設值 */
  pMineral = alloc_struct( STRUCT_MINERAL_DATA );
  set_mineral_default( pMineral );

  /* 載入區域檔裡面的商店部份 */
  for ( ;; )
  {
    word   = if_eof( pFile ) ? "End" : fread_word( pFile );
    fMatch = FALSE;

    switch ( UPPER(word[0]) )
    {
    case '*':
      fMatch = TRUE;
      fread_to_eol( pFile );
      break;

    case 'C':
      KEY( "Count", pMineral->count, fread_number( pFile ) );
      break;

    case 'E':

      if ( !str_scmp( word, "End", &fMatch ) )
      {
        if ( IS_ERROR( pMineral->count ) )
          mudlog( LOG_DEBUG , "Load_mineral﹕沒有數量。" );

        if ( !pMineral->mineral )
          mudlog( LOG_DEBUG , "Load_mineral﹕沒有礦物。" );

        f_close( pFile );
        RETURN( pMineral );
      }

      break;

    case 'F':
      KEY( "Flags", pMineral->flags, fread_number( pFile ) );
      break;

    case 'M':
      KEY( "Message", pMineral->message, fread_string( pFile ) );
      break;

    case 'V':

      if ( !str_scmp( word , "Vnum", &fMatch ) )
      {
        int slot;

        slot = fread_number( pFile );

        if ( !( pMineral->mineral = get_obj_index( slot ) ) )
          mudlog( LOG_DEBUG , "Load_mineral﹕礦物號碼 %d 不存在。", slot );

        break;
      }

      break;
    }

    /* 指令型態錯誤 */
    if ( !fMatch )
      mudlog( LOG_DEBUG , "Load_mineral﹕命令 %s 不正確。" , word );
  }

  RETURN( NULL );
}

/* 載入物品 */
OBJ_INDEX_DATA * load_object( const char * filename )
{
  OBJ_INDEX_DATA * pObjIndex;
  RESTRICT_DATA  * pRestrict;
  RESTRICT_DATA  * zRestrict;
  MESSAGE_DATA   * pMessage;
  SKILL_DATA     * pSkill;
  AFFECT_DATA    * pAffect;
  EXTRA_DESCR_DATA * pExtra;
  FILE_DATA      * pFile;
  char           * word;
  bool             fMatch;
  int              vnum;

  PUSH_FUNCTION( "load_object" );

  if ( !( pFile = f_open( filename , "r" ) ) )
  {
    mudlog( LOG_FAILLOAD , "載入物品檔案 %s 失敗。\n" , filename );
    RETURN( NULL );
  }

  /* 配置物品所需要的記憶體以及設定預設值 */
  set_object_default( pObjIndex = alloc_struct( STRUCT_OBJ_INDEX_DATA ) );

  for ( ;; )
  {
    word   = if_eof( pFile ) ? "End" : fread_word( pFile );
    fMatch = FALSE;

    switch ( UPPER( word[0] ) )
    {
    case '*':
      fMatch = TRUE;
      fread_to_eol( pFile );
      break;

    case '#':

      if ( !str_scmp( word , "#Affect", &fMatch ) )
      {
        /* 避免沒有物品號碼就載入屬性 */
        if ( IS_ERROR( pObjIndex->vnum ) )
        {
          mudlog( LOG_DEBUG, "Load_object﹕物品號碼還沒設定﹐無法設定屬性。" );
          break;
        }

        /* 載入物品屬性格式 */
        if ( !( pAffect = load_affect( pFile ) ) )
        {
          mudlog( LOG_DEBUG, "Load_object﹕載入物品影響失敗﹗" );
          break;
        }

        pAffect->next       = pObjIndex->affected;
        pObjIndex->affected = pAffect;
        break;
      }

      if ( !str_scmp( word , "#Impact", &fMatch ) )
      {
        /* 避免沒有物品號碼就載入屬性 */
        if ( IS_ERROR( pObjIndex->vnum ) )
        {
          mudlog( LOG_DEBUG, "Load_object﹕物品號碼還沒設定﹐無法設定屬性。" );
          break;
        }

        /* 載入物品屬性格式 */
        if ( !( pAffect = load_affect( pFile ) ) )
        {
          mudlog( LOG_DEBUG, "Load_object﹕載入物品魔石效應失敗﹗" );
          break;
        }

        pAffect->next     = pObjIndex->impact;
        pObjIndex->impact = pAffect;
        break;
      }

      if ( !str_scmp( word, "#Restrict", &fMatch ) )
      {
        /* 載入物品裝備限制 */
        if ( !( pRestrict = load_restrict( pFile ) ) )
        {
          mudlog( LOG_DEBUG, "Load_object﹕載入物品限制型態失敗﹗" );
          break;
        }

        for ( zRestrict = pObjIndex->restrict; zRestrict; zRestrict = zRestrict->next )
        {
          if ( zRestrict->type == pRestrict->type
            && zRestrict->type != RES_SKILL )
          {
            mudlog( LOG_DEBUG , "Load_object﹕物品限制型態 %d 重複。"
              , pRestrict->type );
            break;
          }
        }

        TEST_READ_ERROR
        pRestrict->next     = pObjIndex->restrict;
        pObjIndex->restrict = pRestrict;
        break;
      }

      if ( !str_scmp( word, "#Message", &fMatch ) )
      {
        /* 載入物品裝備敘述 */
        if ( !( pMessage = load_message( pFile ) ) )
        {
          mudlog( LOG_DEBUG, "Load_object﹕載入物品敘述失敗﹗" );
          break;
        }

        pMessage->next     = pObjIndex->message;
        pObjIndex->message = pMessage;
        break;
      }

      if ( !str_scmp( word , "#Extra", &fMatch ) )
      {
        /* 避免沒有物品號碼就載入額外描述 */
        if ( IS_ERROR( pObjIndex->vnum ) )
        {
          mudlog( LOG_DEBUG ,
            "Load_object﹕物品號碼還沒設定﹐無法設定額外描述。" );

          break;
        }

        /* 載入物品屬性格式 */
        if ( !( pExtra = load_extra_descr( pFile ) ) )
        {
          mudlog( LOG_DEBUG, "Load_object﹕載入物品額外描述失敗﹗" );
          break;
        }

        pExtra->next           = pObjIndex->extra_descr;
        pObjIndex->extra_descr = pExtra;
        if ( !fReload ) top_ed++;
        break;
      }

      if ( !str_scmp( word, "#Job", &fMatch ) )
      {
        JOB_DATA * pJob;

        if ( !( pJob = load_job( pFile ) ) )
        {
          mudlog( LOG_DEBUG, "Load_object﹕載入物品特殊函數失敗﹗" );
          break;
        }

        pJob->next     = pObjIndex->job;
        pObjIndex->job = pJob;
        break;
      }

      break;

    case 'A':

      KEY( "AntiGood"   , pObjIndex->AntiGood   , fread_number( pFile ) );
      KEY( "AntiEvil"   , pObjIndex->AntiEvil   , fread_number( pFile ) );
      KEY( "AntiNeutral", pObjIndex->AntiNeutral, fread_number( pFile ) );

      if ( !str_scmp( word, "Armor", &fMatch ) )
      {
        pObjIndex->armor = UMIN( MAX_ARMOR, fread_number( pFile ) );
        break;
      }

      break;

    case 'B':
      KEY( "Bless", pObjIndex->Bless      , fread_number( pFile ) );
      break;

    case 'C':

      KEY( "Cost"      , pObjIndex->cost       , fread_number( pFile ) );
      KEY( "Contraband", pObjIndex->Contraband , fread_number( pFile ) );
      KEY( "Cancachet" , pObjIndex->Cancachet  , fread_number( pFile ) );
      break;

    case 'D':

      KEY( "Dark", pObjIndex->Dark, fread_number( pFile ) );

      if ( !str_scmp( word , "Description", &fMatch ) )
      {
        fread_to_eol( pFile ); /* 一定先讀完一行 */
        pObjIndex->description = fread_string( pFile );
        break;
      }

      if ( !str_scmp( word , "Disappear", &fMatch ) )
      {
        pObjIndex->disappear = ( fread_number( pFile ) == TRUE ) ? TRUE : FALSE;
        break;
      }

      break;

    case 'E':

      KEY( "Exp"       , pObjIndex->exp        , fread_number( pFile ) );
      KEY( "Evil"      , pObjIndex->Evil       , fread_number( pFile ) );

      if ( !str_scmp( word, "End", &fMatch ) )
      {
        AFFECT_DATA * paf;
        int           count;

        if ( IS_ERROR( ( vnum = pObjIndex->vnum ) ) )
        {
          mudlog( LOG_DEBUG , "Load_object﹕物品沒有號碼。" );
          break;
        }

        if ( !pObjIndex->name )
        {
          mudlog( LOG_DEBUG , "Load_object﹕物品 %d 沒有名字。", vnum );
          break;
        }

        if ( !pObjIndex->short_descr )
        {
          mudlog( LOG_DEBUG , "Load_object﹕物品 %d 沒有短敘述。", vnum );
          break;
        }

        if ( !pObjIndex->description )
        {
          mudlog( LOG_DEBUG , "Load_object﹕物品 %d 沒有長敘述。", vnum );
          break;
        }

        if ( IS_ERROR( pObjIndex->item_type ) )
        {
          mudlog( LOG_DEBUG , "Load_object﹕物品 %d 沒有物品型態。", vnum );
          break;
        }

        if ( pObjIndex->weight < 0 )
        {
          mudlog( LOG_DEBUG, "Load_object﹕物品 %d 沒有重量。", vnum );
          break;
        }

        switch( pObjIndex->item_type )
        {
        default:

          if ( pObjIndex->cost > pObjIndex->level * 20 )
            mudlog( LOG_INFO, "Load_object﹕[物品太貴] 物品 %d 價錢太高。", vnum );

          if ( pObjIndex->item_type == ITEM_MAGICSTONE && !pObjIndex->impact )
            mudlog( LOG_INFO, "Load_obj﹕魔石 %d 沒有指定效應。", vnum );

          if ( pObjIndex->item_type != ITEM_MAGICSTONE && pObjIndex->impact )
            mudlog( LOG_INFO, "Load_object﹕非魔石 %d 有指定效應。", vnum );

          for ( count = 0, paf = pObjIndex->affected; paf; paf = paf->next )
          {
            switch( paf->location )
            {
            default:
              break;

            case APPLY_HIT:
            case APPLY_MANA:
            case APPLY_MOVE:
              if ( paf->modifier > pObjIndex->level * 10 )
                mudlog( LOG_DEBUG, "Load_object﹕物品 %-5d 等級 %-3d 增加"
                  "點數 %4d 太多。", vnum, pObjIndex->level, paf->modifier );
              break;

            case APPLY_STR:
            case APPLY_INT:
            case APPLY_WIS:
            case APPLY_DEX:
            case APPLY_CON:
              if ( paf->modifier > 3 )
                mudlog( LOG_DEBUG, "Load_object﹕物品 %-5d 等級 %-3d 增加屬"
                  "性 %2d 太多。", vnum, pObjIndex->level, paf->modifier );
              count += paf->modifier;
              break;
            }
          }

          if ( count > 2 )
          {
            mudlog( LOG_DEBUG,
              "Load_object﹕物品 %-5d 等級 %-3d 增加屬性 %2d 太多。"
              , vnum, pObjIndex->level, count );
          }

          break;

        case ITEM_PILL:
        case ITEM_POTION:
        case ITEM_MYSTERY:
        case ITEM_SCROLL:
        case ITEM_SIGIL:
        case ITEM_VELLUM:
        case ITEM_BOAT:
        case ITEM_FIREWORK:
        case ITEM_URN:
          break;
        }


        if ( IS_ERROR( pObjIndex->cost ) )
          pObjIndex->cost = pObjIndex->level * 10;

        if ( IS_ERROR( pObjIndex->armor ) )
          pObjIndex->armor = pObjIndex->level * 100;

        if ( pObjIndex->item_type == ITEM_POTION ) pObjIndex->NoDrop = TRUE;

        /* 檢查物品數值 */
        switch( pObjIndex->item_type )
        {
        default:
          break;

        case ITEM_ARROW:
          pObjIndex->value[0] = UMAX( 1, pObjIndex->value[0] );
          pObjIndex->value[1] = UMAX( 1, pObjIndex->value[1] );

         break;

        case ITEM_WEAPON:

          /* 當武器攻傷小於等於零時的處理 */
          if ( pObjIndex->value[1] <= 0 || pObjIndex->value[2] <= 0 )
          {
            int max;
            int min;

            max = 100 + ( pObjIndex->level / 2 );
            min = 100;

            switch( pObjIndex->value[3] )
            {
            case WEAPON_AXE:
              max += 7;
              min += 4;
              break;

            case WEAPON_HAMMER:
            case WEAPON_CLUB:
              max += 6;
              min += 4;
              break;

            case WEAPON_BLADE:
              max += 5;
              min += 3;
              break;

            case WEAPON_SWORD:
              max += 4;
              min += 3;
              break;

            case WEAPON_SPEAR:
              max += 3;
              min += 2;
              break;

            case WEAPON_WHIP:
              max += 2;
              min += 2;
              break;

            case WEAPON_BOW:
              max += 1;
              min += 1;
              break;

            case WEAPON_DAGGER:
            case WEAPON_PEN:
              break;
            }

            pObjIndex->value[1] = min;
            pObjIndex->value[2] = max;
          }

          break;

        case ITEM_ARMOR:

          if ( pObjIndex->value[0] <= 0 )
            pObjIndex->value[0]  = UMAX( 1, ( pObjIndex->level / 20 ) );

          break;

        case ITEM_BACKBAG:
          pObjIndex->value[0] = UMAX( 1, pObjIndex->value[0] );
          pObjIndex->value[1] = 0;
          pObjIndex->value[2] = 0;
          break;

        case ITEM_SCROLL:
        case ITEM_POTION:
        case ITEM_PILL:

          switch( pObjIndex->value[1] )
          {
          default:
            mudlog( LOG_DEBUG , "Load_object﹕施法物品 %d 數值錯誤。", vnum );
            break;

          /* 不需要設定技能號碼 */
          case OBJ_CAST_HIT:
          case OBJ_CAST_MANA:
          case OBJ_CAST_MOVE:
          case OBJ_CAST_ALL:
            break;

          /* 不需要設定技能號碼 */
          case OBJ_CAST_IDENT:
          case OBJ_CURE_POISON:
          case OBJ_DETECT_INVS:
          case OBJ_DETECT_MASK:
          case OBJ_CAST_FLY:
          case OBJ_CURE_BLIND:
          case OBJ_GIANT_STR:
          case OBJ_DETECT_HIDE:
          case OBJ_SNEAK:
          case OBJ_DETECT_EVIL:
          case OBJ_CHANGE_SEX:
          case OBJ_DETECT_MAGIC:
          case OBJ_DETECT_POISON:
          case OBJ_FAERIE_FOG:
          case OBJ_GATE:
          case OBJ_FIXITY:

            for ( pSkill = skill_list; pSkill; pSkill = pSkill->next )
              if ( pSkill->affect_id == pObjIndex->value[1] ) break;

            if ( !pSkill )
              mudlog( LOG_DEBUG , "Load_object﹕施法物品 %d 技能錯誤。", vnum );

            pObjIndex->value[2] = pSkill->slot;
            break;
          }

          break;
        }

        pObjIndex->wear_flags &= ObjWearMask;
        pObjIndex->filename    = str_dup( filename );

        TEST_READ_ERROR

        f_close( pFile );
        RETURN( pObjIndex );
      }

      break;

    case 'G':

      KEY( "Glow", pObjIndex->Glow, fread_number( pFile ) );
      break;

    case 'H':

      KEY( "Hum", pObjIndex->Hum , fread_number( pFile ) );
      break;

    case 'I':

      KEY( "ItemType" , pObjIndex->item_type  , fread_number( pFile ) );
      KEY( "Invis"    , pObjIndex->Invis      , fread_number( pFile ) );
      KEY( "Inventory", pObjIndex->Inventory  , fread_number( pFile ) );

      break;

    case 'L':

      if ( !str_scmp( word, "Level", &fMatch ) )
      {
        pObjIndex->level = fread_number( pFile );
        TEST_READ_ERROR

        if ( pObjIndex->level < 0 || pObjIndex->level > MAX_LEVEL )
        {
          mudlog( LOG_DEBUG , "Load_object﹕物品等級 %d 錯誤。", pObjIndex->level );
          break;
        }

        break;
      }

      KEY( "Lock"    , pObjIndex->Lock       , fread_number( pFile ) );
      KEY( "Loadable", pObjIndex->Loadable   , fread_number( pFile ) );
      break;

    case 'N':

      KEY( "Name"    , pObjIndex->name       , fread_string( pFile ) );
      KEY( "NoDrop"  , pObjIndex->NoDrop     , fread_number( pFile ) );
      KEY( "NoRemove", pObjIndex->NoRemove   , fread_number( pFile ) );
      KEY( "NoSave"  , pObjIndex->NoSave     , fread_number( pFile ) );

      break;

    case 'M':

      KEY( "Magic", pObjIndex->Magic      , fread_number( pFile ) );
      break;

    case 'R':

      if ( !str_scmp( word , "Remmsg", &fMatch ) )
      {
        fread_to_eol( pFile ); /* 一定先讀完一行 */
        pObjIndex->remmsg = fread_string( pFile );
        break;
      }

      break;

    case 'S':

      KEY( "ShortDesc" , pObjIndex->short_descr , fread_string( pFile ) );
      break;

    case 'T':
      KEY( "Takeable", pObjIndex->Takeable, fread_number( pFile ) );
      break;

    case 'U':

      KEY( "Unit"   , pObjIndex->unit   , fread_string( pFile ) );
      KEY( "UserSet", pObjIndex->UserSet, fread_number( pFile ) );
      break;

    case 'V':

      KEY( "Value1"  , pObjIndex->value[ 0] , fread_number( pFile ) );
      KEY( "Value2"  , pObjIndex->value[ 1] , fread_number( pFile ) );
      KEY( "Value3"  , pObjIndex->value[ 2] , fread_number( pFile ) );
      KEY( "Value4"  , pObjIndex->value[ 3] , fread_number( pFile ) );
      KEY( "Value5"  , pObjIndex->value[ 4] , fread_number( pFile ) );
      KEY( "Value6"  , pObjIndex->value[ 5] , fread_number( pFile ) );
      KEY( "Value7"  , pObjIndex->value[ 6] , fread_number( pFile ) );
      KEY( "Value8"  , pObjIndex->value[ 7] , fread_number( pFile ) );
      KEY( "Value9"  , pObjIndex->value[ 8] , fread_number( pFile ) );
      KEY( "Value10" , pObjIndex->value[ 9] , fread_number( pFile ) );
      KEY( "Value11" , pObjIndex->value[10] , fread_number( pFile ) );
      KEY( "Value12" , pObjIndex->value[11] , fread_number( pFile ) );
      KEY( "Value13" , pObjIndex->value[12] , fread_number( pFile ) );
      KEY( "Value14" , pObjIndex->value[13] , fread_number( pFile ) );
      KEY( "Value15" , pObjIndex->value[14] , fread_number( pFile ) );
      KEY( "Vnum"    , pObjIndex->vnum      , fread_number( pFile ) );

      if ( !str_scmp( word , "Value", &fMatch ) )
      {
        int loop = 0;

        while ( loop < MAX_OBJECT_VALUE )
        {
          TEST_READ_ERROR
          if ( fread_if_eol( pFile ) ) break;
          pObjIndex->value[ loop++ ] = fread_number( pFile );
        }

        TEST_READ_ERROR
        fread_to_eol( pFile );
        break;
      }

      break;

    case 'W':

      KEY( "WearLoc" , pObjIndex->wear_flags , fread_number( pFile ) );
      KEY( "Weight"  , pObjIndex->weight     , fread_number( pFile ) );

      if ( !str_scmp( word , "Wearmsg", &fMatch ) )
      {
        fread_to_eol( pFile ); /* 一定先讀完一行 */
        pObjIndex->wearmsg = fread_string( pFile );
        break;
      }

      break;

    }

    /* 指令型態錯誤 */
    if ( !fMatch || FileError )
    {
      if ( !fMatch ) mudlog( LOG_DEBUG , "Load_object﹕命令 %s 不正確。" , word );
      if ( fReload )
      {
        free_obj_index( pObjIndex );
        f_close( pFile );
        RETURN( NULL );
      }
    }
  }

  RETURN( NULL );
}

/* 載入物品的影響 */
AFFECT_DATA * load_affect( FILE_DATA * pFile )
{
  AFFECT_DATA * pAffect;
  char        * word;
  bool          fMatch;

  PUSH_FUNCTION( "load_affect" );

  /* 配置物品屬性的記憶體和設定預設值 */
  set_affect_default( pAffect = alloc_struct( STRUCT_AFFECT_DATA ) );

  for ( ;; )
  {
    word   = if_eof( pFile ) ? "End" : fread_word( pFile );
    fMatch = FALSE;

    switch ( UPPER( word[0] ) )
    {
    case '*':
      fMatch = TRUE;
      fread_to_eol( pFile );
      break;

    case 'E':

      if ( !str_scmp( word, "End", &fMatch ) )
      {
        if ( IS_ERROR( pAffect->location ) )
          mudlog( LOG_DEBUG , "Load_affect﹕沒有設定影響屬性的型態。" );

        TEST_READ_ERROR
        if ( !fReload )  top_affect++;
        RETURN( pAffect );
      }

      break;

    case 'L':
      KEY( "Location" , pAffect->location , fread_number( pFile ) );
      break;

    case 'M':
      KEY( "Modifier" , pAffect->modifier , fread_number( pFile ) );
      break;

    case 'T':
      KEY( "Type", pAffect->type, fread_number( pFile ) );
      break;

    case 'V':
      KEY( "Value" , pAffect->modifier , fread_number( pFile ) );
      break;
    }

    /* 指令型態錯誤 */
    if ( !fMatch || FileError )
    {
      if ( !fMatch ) mudlog( LOG_DEBUG , "Load_affect﹕命令 %s 不正確。", word );
      if ( fReload )
      {
        free_affect( pAffect );
        RETURN( NULL );
      }
    }
  }

  RETURN( NULL );
}

/* 載入物品裝備、技能之限制 */
RESTRICT_DATA * load_restrict( FILE_DATA * pFile )
{
  RESTRICT_DATA * pRestrict;
  char          * word;
  bool            fMatch;

  PUSH_FUNCTION( "load_restrict" );

  /* 配置物品屬性的記憶體和設定預設值 */
  set_restrict_default( pRestrict = alloc_struct( STRUCT_RESTRICT_DATA ) );

  for ( ;; )
  {
    word   = if_eof( pFile ) ? "End" : fread_word( pFile );
    fMatch = FALSE;

    switch ( UPPER( word[0] ) )
    {
    case '*':
      fMatch = TRUE;
      fread_to_eol( pFile );
      break;

    case 'E':

      if ( !str_scmp( word, "End", &fMatch ) )
      {
        if ( IS_ERROR( pRestrict->type ) )
        {
          mudlog( LOG_DEBUG, "Load_restrict﹕限制缺乏型態。" );
          break;
        }

        if ( IS_ERROR( pRestrict->value ) )
        {
          mudlog( LOG_DEBUG, "Load_restrict﹕限制沒有數值。" );
          break;
        }

        switch( pRestrict->type )
        {
        case RES_STR:
          if ( pRestrict->value > MaxStr || pRestrict->value < 0 )
            mudlog( LOG_DEBUG, "Load_restrict﹕力量數值 %d 不合理。"
              , pRestrict->value );
           break;

        case RES_INT:
          if ( pRestrict->value > MaxInt || pRestrict->value < 0 )
            mudlog( LOG_DEBUG, "Load_restrict﹕智力數值 %d 不合理。"
              , pRestrict->value );
           break;

        case RES_WIS:
          if ( pRestrict->value > MaxWis || pRestrict->value < 0 )
            mudlog( LOG_DEBUG, "Load_restrict﹕學識數值 %d 不合理。"
              , pRestrict->value );
           break;

        case RES_DEX:
          if ( pRestrict->value > MaxDex || pRestrict->value < 0 )
            mudlog( LOG_DEBUG, "Load_restrict﹕敏捷數值 %d 不合理。"
              , pRestrict->value );
           break;

        case RES_CON:
          if ( pRestrict->value > MaxCon || pRestrict->value < 0 )
            mudlog( LOG_DEBUG, "Load_restrict﹕體格數值 %d 不合理。"
              , pRestrict->value );
           break;

        case RES_TRUST:
          if ( pRestrict->value > MAX_LEVEL || pRestrict->value < 0 )
            mudlog( LOG_DEBUG, "Load_restrict﹕信任數值 %d 不合理。"
              , pRestrict->value );
           break;

        case RES_SEX:

          switch( pRestrict->value )
          {
          default:
            mudlog( LOG_DEBUG, "Load_restrict﹕性別 %d 錯誤。"
              , pRestrict->value );
            break;

          case SEX_FEMALE:
          case SEX_MALE:
          case SEX_NEUTRAL:
            break;
          }

        case RES_LEVEL:
          if ( pRestrict->value > MAX_LEVEL || pRestrict->value < 0 )
            mudlog( LOG_DEBUG, "Load_restrict﹕等級數值 %d 不合理。"
              , pRestrict->value );

          break;

        case RES_ALIGN:

          if ( pRestrict->value < -1000 || pRestrict->value > 1000 )
            mudlog( LOG_DEBUG, "Load_restrict﹕陣營數值 %d 不合理。"
              , pRestrict->value );
           break;
        }

        TEST_READ_ERROR
        if ( !fReload ) top_restrict++;
        RETURN( pRestrict );
      }

      break;

    case 'O':

      if ( !str_scmp( word, "Occassion", &fMatch ) )
      {
        pRestrict->occasion = fread_number( pFile );
        TEST_READ_ERROR

        switch( pRestrict->occasion )
        {
        default:
          mudlog( LOG_DEBUG, "Load_restrict﹕時機 %d 錯誤。"
            , pRestrict->occasion );
          break;

        case OCCASION_NONE:
          break;
        }

        break;
      }
      break;

    case 'S':

      if ( !str_scmp( word, "Skill", &fMatch ) )
      {
        char       * pWord;
        SKILL_DATA * pSkill;

        pWord = fread_word( pFile );
        TEST_READ_ERROR

        if ( !( pSkill = skill_isname( pWord ) ) )
        {
          mudlog( LOG_DEBUG, "Load_restrict﹕沒有這種技能 %s。", pWord );
          break;
        }

        pRestrict->value = pSkill->slot;
        break;
      }
      break;

    case 'T':

      if ( !str_scmp( word, "Type", &fMatch ) )
      {
        pRestrict->type = fread_number( pFile );
        TEST_READ_ERROR

        switch( pRestrict->type )
        {
        default:
          mudlog( LOG_DEBUG, "Load_restrict﹕限制型態 %d 錯誤。"
            , pRestrict->type );
          break;

        case RES_STR:
        case RES_INT:
        case RES_WIS:
        case RES_DEX:
        case RES_CON:
        case RES_HP:
        case RES_MANA:
        case RES_MOVE:
        case RES_CLASS:
        case RES_TRUST:
        case RES_SKILL:
        case RES_NOSKILL:
        case RES_SEX:
        case RES_LEVEL:
        case RES_ALIGN:
          break;
        }

        break;
      }
      break;

    case 'V':
      KEY( "Value"    , pRestrict->value    , fread_number( pFile ) );
      KEY( "ViceValue", pRestrict->vicevalue, fread_number( pFile ) );
      break;
    }

    /* 指令型態錯誤 */
    if ( !fMatch || FileError )
    {
      if ( !fMatch ) mudlog( LOG_DEBUG , "Load_restrict﹕命令 %s 不正確。", word );
      if ( fReload )
      {
        free_restrict( pRestrict );
        RETURN( NULL );
      }
    }
  }

  RETURN( NULL );
}

MESSAGE_DATA * load_message( FILE_DATA * pFile )
{
  MESSAGE_DATA  * pMessage;
  char          * word;
  bool            fMatch;

  PUSH_FUNCTION( "load_message" );

  /* 配置物品屬性的記憶體和設定預設值 */
  set_message_default( pMessage= alloc_struct( STRUCT_MESSAGE_DATA ) );

  for ( ;; )
  {
    word   = if_eof( pFile ) ? "End" : fread_word( pFile );
    fMatch = FALSE;

    switch ( UPPER( word[0] ) )
    {
    case '*':
      fMatch = TRUE;
      fread_to_eol( pFile );
      break;

    case 'E':

      if ( !str_scmp( word, "End", &fMatch ) )
      {
        if ( IS_ERROR( pMessage->type ) )
        {
          mudlog( LOG_DEBUG, "Load_message﹕敘述沒有形態。" );
          break;
        }

        if ( !pMessage->self )
        {
          mudlog( LOG_DEBUG, "Load_message﹕敘述沒有敘述字串。" );
          break;
        }

        if ( !pMessage->others )
        {
          mudlog( LOG_DEBUG, "Load_message﹕敘述沒有敘述字串。" );
          break;
        }

        TEST_READ_ERROR
        RETURN( pMessage );
      }

      break;

    case 'O':

      if ( !str_scmp( word , "Others", &fMatch ) )
      {
        if ( pMessage->others )
        {
          mudlog( LOG_DEBUG, "Load_message﹕敘述重覆定義。" );
          break;
        }

        fread_to_eol( pFile ); /* 一定先讀完一行 */
        pMessage->others = fread_string( pFile );
        break;
      }

      break;

    case 'S':

      if ( !str_scmp( word , "Self", &fMatch ) )
      {
        if ( pMessage->self )
        {
          mudlog( LOG_DEBUG, "Load_message﹕敘述重覆定義。" );
          break;
        }

        fread_to_eol( pFile ); /* 一定先讀完一行 */
        pMessage->self = fread_string( pFile );
        break;
      }

      if ( !str_scmp( word , "String", &fMatch ) )
      {
        if ( pMessage->self )
        {
          mudlog( LOG_DEBUG, "Load_message﹕敘述重覆定義。" );
          break;
        }

        if ( pMessage->others )
        {
          mudlog( LOG_DEBUG, "Load_message﹕敘述重覆定義。" );
          break;
        }

        fread_to_eol( pFile ); /* 一定先讀完一行 */
        pMessage->self   = fread_string( pFile );
        TEST_READ_ERROR
        pMessage->others = str_dup( pMessage->self );
        break;
      }

      break;

    case 'T':

      if ( !str_scmp( word, "Type", &fMatch ) )
      {
        pMessage->type = fread_number( pFile );
        TEST_READ_ERROR

        switch( pMessage->type )
        {
        default:
          mudlog( LOG_DEBUG, "Load_message﹕敘述型態 %d 錯誤。"
            , pMessage->type );
          break;

        case ACT_WHEN_EAT:
        case ACT_WHEN_WEAR:
        case ACT_WHEN_DROP:
        case ACT_WHEN_REMOVE:
        case ACT_WHEN_SACRIFICE:
        case ACT_WHEN_GET:
        case ACT_WHEN_PUT:
        case ACT_WHEN_GIVE:
        case ACT_WHEN_FILL:
        case ACT_WHEN_DRINK:
        case ACT_WHEN_RECITE:
        case ACT_WHEN_BUY:
        case ACT_WHEN_SELL:
        case ACT_WHEN_VALUE:
        case ACT_WHEN_LOOK:
        case ACT_WHEN_COMPARE:
        case ACT_WHEN_SEND:
        case ACT_WHEN_OPEN:
        case ACT_WHEN_CLOSE:
        case ACT_WHEN_LOCK:
        case ACT_WHEN_UNLOCK:
        case ACT_WHEN_AUCTION:
        case ACT_WHEN_BET:
        case ACT_WHEN_REPAIR:
        case ACT_WHEN_DEPOSIT:
        case ACT_WHEN_WITHDRAW:
        case ACT_WHEN_CACHET:
        case ACT_WHEN_SLUP:
          break;
        }

        break;
      }
      break;
    }

    /* 指令型態錯誤 */
    if ( !fMatch || FileError )
    {
      if ( !fMatch ) mudlog( LOG_DEBUG , "Load_message﹕命令 %s 不正確。", word );
      if ( FileError )
      {
        free_message( pMessage );
        RETURN( NULL );
      }
    }
  }

  RETURN( NULL );
}

/* 讀入區域檔的 reset */
void load_resets( char * filename , AREA_DATA * pArea )
{
  ROOM_INDEX_DATA * pRoomIndex;
  RESET_DATA      * pReset;
  EXIT_DATA       * pExit;
  FILE_DATA       * pFile;
  char              letter;

  PUSH_FUNCTION( "load_reset" );

  if ( !( pFile = f_open( filename , "r" ) ) )
  {
    mudlog( LOG_FAILLOAD , "載入檔案 %s 失敗。\n" , filename );
    RETURN_NULL();
  }

  if ( !pArea )
    mudlog( LOG_DEBUG , "Load_resets﹕沒有任何的 #AREA 所以不能做 reset 。" );

  for ( ;; )
  {
    if ( if_eof( pFile ) ) break;
    letter = fread_letter( pFile );

    if ( letter == '*' )
    {
      fread_to_eol( pFile );
      continue;
    }

    /* 配置記憶體 */
    pReset          = alloc_struct( STRUCT_RESET_DATA );
    pReset->command = letter;
    pReset->arg1    = fread_number( pFile );
    pReset->arg2    = fread_number( pFile );
    pReset->arg3    = ( letter == 'G' || letter == 'R')
                        ? 0 : fread_number( pFile );

    fread_to_eol( pFile );

    switch ( letter )
    {
    default:
      mudlog( LOG_DEBUG , "Load_resets﹕未定義的 RESET 命令 '%c'。", letter );
      break;

    case 'M':

      if ( !get_mob_index ( pReset->arg1 ) )
        mudlog( LOG_DEBUG, "Load_reset﹕(M) 沒有怪物編號 %d。", pReset->arg1 );

      if ( !get_room_index( pReset->arg3 ) )
        mudlog( LOG_DEBUG, "Load_reset﹕(M) 沒有房間編號 %d。", pReset->arg3 );

      break;

    case 'O':

      if ( !get_obj_index ( pReset->arg1 ) )
        mudlog( LOG_DEBUG, "Load_reset﹕(O) 沒有物品號碼 %d。", pReset->arg1 );

      if ( !get_room_index( pReset->arg3 ) )
        mudlog( LOG_DEBUG, "Load_reset﹕(O)  沒有房間編號 %d。", pReset->arg3 );

      break;

    case 'P':

      if ( !get_obj_index( pReset->arg1 ) )
        mudlog( LOG_DEBUG, "Load_reset﹕(P) 沒有物品一號碼 %d。", pReset->arg1 );

      if ( !get_obj_index( pReset->arg3 ) )
        mudlog( LOG_DEBUG, "Load_reset﹕(P) 沒有物品二號碼 %d。", pReset->arg1 );

      break;

    case 'E':
    case 'G':

      if ( !get_obj_index( pReset->arg1 ) )
        mudlog( LOG_DEBUG, "Load_reset﹕(EG) 沒有物品號碼 %d。", pReset->arg1 );

      break;

    case 'A':

      if ( !get_mob_index( pReset->arg1 ) )
        mudlog( LOG_DEBUG, "Load_reset﹕(A) 沒有怪物號碼 %d。", pReset->arg1 );

      break;

    case 'D':

      if ( !( pRoomIndex = get_room_index( pReset->arg1 ) ) )
        mudlog( LOG_DEBUG, "Load_reset﹕(D) 沒有房間號碼 %d。", pReset->arg1 );

      if ( pReset->arg2 < 0
        || pReset->arg2 > ( DIR_MAX - 1 )
        || ( pExit = pRoomIndex->exit[pReset->arg2] ) == NULL
        || !pExit->isdoor )
      {
        mudlog( LOG_DEBUG , "Load_resets﹕'D'﹕出口 %d 不是一個門。"
          , pReset->arg2 );
      }

      if ( pReset->arg3 < 0 || pReset->arg3 > 2 )
        mudlog( LOG_DEBUG , "Load_resets﹕'D'﹕錯誤的鎖﹕%d。", pReset->arg3 );

      break;

    case 'R':

      if ( !( pRoomIndex = get_room_index( pReset->arg1 ) ) )
        mudlog( LOG_DEBUG, "Load_reset﹕(R) 沒有房間號碼 %d。", pReset->arg1 );


      if ( pReset->arg2 < 0 || pReset->arg2 > 6 )
        mudlog( LOG_DEBUG , "Load_resets﹕'R'﹕錯誤出口 %d。", pReset->arg2 );

      break;
    }

    if ( !pArea->reset_first ) pArea->reset_first      = pReset;
    if (  pArea->reset_last  ) pArea->reset_last->next = pReset;

    pArea->reset_last = pReset;
    pReset->next      = NULL;
    if ( !fReload ) top_reset++;
  }

  f_close( pFile );
  RETURN_NULL();
}

void load_mail( const char * path )
{
  glob_t      result;
  int         rc;
  int         count;
  FILE_DATA * pFile;
  NOTE_DATA * pNote;
  char        directory[ MAX_FILE_LENGTH ];
  char        buf[ MAX_FILE_LENGTH ];

  PUSH_FUNCTION( "load_mail" );

  /* 處理目錄 */
  fill_path( strcpy( directory, path ) );

  if ( ( rc = glob_file( directory, note_ext, &result ) ) >= 0 )
  {
    for ( count = 0; count < rc; count++ )
    {
      sprintf( buf, "%s%s", directory, result.gl_pathv[count] );

      if ( is_regular( buf ) )
      {
        if ( !( pFile = f_open( buf , "r" ) ) )
          mudlog( LOG_ERR , "Load_mail﹕開啟信件檔 %s 有問題。" , buf );

        pNote = load_a_note( pFile );

        pNote->next = note_list;
        note_list   = pNote;
        pNote->filename = str_dup( buf );

        f_close( pFile );
      }

    }
    globfree( &result );
  }

  else
  {
    mudlog( LOG_ERR , "Load_mail﹕沒有郵件子目錄 %s。", path );
  }

  RETURN_NULL();
}

NOTE_DATA * load_a_note( FILE_DATA * pFile )
{
  NOTE_DATA * pNote;
  char      * word;
  bool        fMatch;

  PUSH_FUNCTION( "load_a_note" );

  set_note_default( pNote = alloc_struct( STRUCT_NOTE_DATA ) );

  for ( ;; )
  {
    word   = if_eof( pFile ) ? "End" : fread_word( pFile );
    fMatch = FALSE;

    switch ( UPPER( word[0] ) )
    {
    case '*':

      fMatch = TRUE;
      fread_to_eol( pFile );
      break;

    case 'D':
      KEY( "Date", pNote->date, fread_string( pFile ) );
      break;

    case 'E':

      if ( !str_scmp( word, "End", &fMatch ) )
      {
        if ( !pNote->sender  ) mudlog( LOG_DEBUG , "Load_a_note﹕缺乏寄信者。" );
        if ( !pNote->date    ) mudlog( LOG_DEBUG , "Load_a_note﹕缺乏時間。"   );
        if ( !pNote->to_list ) mudlog( LOG_DEBUG , "Load_a_note﹕缺乏收信者。" );
        if ( !pNote->subject ) mudlog( LOG_DEBUG , "Load_a_note﹕缺乏主題。"   );
        if ( !pNote->text    ) mudlog( LOG_DEBUG , "Load_a_note﹕缺乏內容。"   );

        if ( IS_ERROR( pNote->date_stamp ) )
          mudlog( LOG_DEBUG , "Load_a_note﹕缺乏郵戳。" );

        RETURN( pNote );
      }

      break;

    case 'M':

      if ( !str_scmp( word, "Mark", &fMatch ) )
      {
        pNote->mark = ( fread_number( pFile ) == TRUE ) ? TRUE : FALSE;
        break;
      }
      break;

    case 'S':
      KEY( "Sender" , pNote->sender      , fread_string( pFile ) );
      KEY( "Subject", pNote->subject     , fread_string( pFile ) );
      KEY( "Stamp"  , pNote->date_stamp  , fread_number( pFile ) );
      break;

    case 'T':

      KEY( "To", pNote->to_list, fread_string( pFile ) );

      if ( !str_scmp( word, "Text", &fMatch ) )
      {
        fread_to_eol( pFile ); /* 一定先讀完一行 */
        pNote->text = fread_string( pFile );
        break;
      }

      break;
    }

    /* 指令型態錯誤 */
    if ( !fMatch )
      mudlog( LOG_ERR , "Load_a_note﹕命令 %s 不正確。" , word );
  }

  RETURN( NULL );
}

void load_joke( const char * path )
{
  glob_t      result;
  int         rc;
  int         count;
  FILE_DATA * pFile;
  JOKE_DATA * pJoke;
  JOKE_DATA * zJoke;
  char        directory[ MAX_FILE_LENGTH ];
  char        buf[ MAX_FILE_LENGTH ];

  PUSH_FUNCTION( "load_joke" );

  /* 處理目錄 */
  fill_path( strcpy( directory, path ) );

  if ( ( rc = glob_file( directory, joke_ext, &result ) ) >= 0 )
  {
    for ( count = 0; count < rc; count++ )
    {
      sprintf( buf, "%s%s", directory, result.gl_pathv[count] );

      if ( is_regular( buf ) )
      {
        if ( !( pFile = f_open( buf , "r" ) ) )
          mudlog( LOG_DEBUG , "Load_joke﹕開啟笑話檔 %s 有問題。" , buf );

        pJoke = load_a_joke( pFile );
        pJoke->stamp = ++top_joke;

        if ( !joke_list )
        {
          joke_list = pJoke;
        }

        else
        {
          for ( zJoke = joke_list; zJoke->next; zJoke = zJoke->next );
          zJoke->next = pJoke;
        }

        f_close( pFile );
      }

    }
    globfree( &result );
  }

  else
  {
    mudlog( LOG_DEBUG , "Load_joke﹕沒有笑話子目錄 %s。", path );
  }

  mudlog( LOG_INFO , "系統載入 %d 個笑話檔。" , top_joke );
  RETURN_NULL();
}

JOKE_DATA * load_a_joke( FILE_DATA * pFile )
{
  char      * word;
  bool        fMatch;
  JOKE_DATA * pJoke;

  PUSH_FUNCTION( "load_a_joke" );

  set_joke_default( pJoke = alloc_struct( STRUCT_JOKE_DATA ) );

  for ( ;; )
  {
    word   = if_eof( pFile ) ? "End" : fread_word( pFile );
    fMatch = FALSE;

    switch ( UPPER( word[0] ) )
    {
    case '*':

      fMatch = TRUE;
      fread_to_eol( pFile );
      break;

    case 'E':

      if ( !str_scmp( word, "End", &fMatch ) )
      {
        if ( !pJoke->title || !*pJoke->title )
          mudlog( LOG_DEBUG , "Load_a_joke﹕缺乏標題。" );

        if ( !pJoke->org )
          mudlog( LOG_DEBUG, "Load_a_joke﹕缺乏出處。" );

        if ( !pJoke->text || !*pJoke->text )
          mudlog( LOG_DEBUG , "Load_a_joke﹕缺乏本文。" );

        RETURN( pJoke );
      }

      break;

    case 'O':
      KEY( "Org", pJoke->org, fread_string( pFile ) );
      break;

    case 'T':

      KEY( "Title", pJoke->title, fread_string( pFile ) );

      if ( !str_scmp( word, "Text", &fMatch ) )
      {
        fread_to_eol( pFile ); /* 一定先讀完一行 */
        pJoke->text = fread_string( pFile );

        break;
      }
      break;

    }

    /* 指令型態錯誤 */
    if ( !fMatch )
      mudlog( LOG_DEBUG , "Load_a_joke﹕命令 %s 不正確。" , word );
  }

  RETURN( NULL );
}

void load_vote( const char * path )
{
  glob_t      result;
  int         rc;
  int         count;
  int         loop;
  FILE_DATA * pFile;
  VOTE_DATA * pVote;
  VOTE_DATA * zVote;
  char        directory[ MAX_FILE_LENGTH ];
  char        buf[ MAX_FILE_LENGTH ];

  PUSH_FUNCTION( "load_vote" );

  /* 處理目錄 */
  fill_path( strcpy( directory, path ) );

  if ( ( rc = glob_file( directory, vote_ext, &result ) ) >= 0 )
  {
    for ( count = 0; count < rc; count++ )
    {
      sprintf( buf, "%s%s", directory, result.gl_pathv[count] );
      if ( is_regular( buf ) )
      {
        if ( !( pFile = f_open( buf , "r" ) ) )
          mudlog( LOG_DEBUG , "Load_vote﹕開啟投票檔 %s 有問題。" , buf );

        pVote = load_a_vote( pFile );
        f_close( pFile );

        /* 如果已經開票則以信件方式公佈 */
        if ( vote_timeup( pVote ) )
        {
          vote_to_note( pVote );

          if ( unlink( buf ) != 0 )
            mudlog( LOG_INFO, "Load_vote﹕移除投票檔 %s 失敗。", buf );

          free_string( pVote->poster  );
          free_string( pVote->subject );
          free_string( pVote->text    );
          free_string( pVote->date    );
          free_string( pVote->club    );

          for ( loop = 0; loop < MAX_VOTES; loop++ )
            free_string( pVote->message[loop] );

          for ( loop = 0; loop < MAX_POLL; loop++ )
            free_string( pVote->poller[loop] );

          free_struct( pVote, STRUCT_VOTE_DATA );
        }

        else
        {
          if ( !vote_list )
          {
            vote_list = pVote;
          }

          else
          {
            for ( zVote = vote_list; zVote->next; zVote = zVote->next );
            zVote->next = pVote;
          }

          if ( !fReload ) top_vote++;
        }
      }
    }
    globfree( &result );
  }
  else
  {
    mudlog( LOG_DEBUG , "Load_vote﹕沒有投票子目錄 %s。", path );
  }

  mudlog( LOG_INFO , "系統載入 %d 個投票檔。" , top_vote );
  RETURN_NULL();
}

VOTE_DATA * load_a_vote( FILE_DATA * pFile )
{
  VOTE_DATA * pVote;
  char      * word;
  bool        fMatch;

  PUSH_FUNCTION( "load_a_vote" );

  set_vote_default( pVote = alloc_struct( STRUCT_VOTE_DATA ) );

  for ( ;; )
  {
    word   = if_eof( pFile ) ? "End" : fread_word( pFile );
    fMatch = FALSE;

    switch ( UPPER( word[0] ) )
    {
    case '*':

      fMatch = TRUE;
      fread_to_eol( pFile );
      break;

    case 'B':

      if ( !str_scmp( word, "Ballot", &fMatch ) )
      {
        int    loop;
        int    choice;
        char * name;

        choice = fread_number( pFile );
        name   = fread_string( pFile );
        choice = UMAX( 0, UMIN( MAX_VOTES - 1, choice ) );

        for ( loop = 0; loop < MAX_POLL; loop++ )
        {
          if ( !pVote->poller[loop] || *( pVote->poller[loop] ) == '\x0' )
          {
            pVote->poller[loop] = str_dup( name );
            pVote->vote[loop]   = choice;
            pVote->poll[choice]++;
            break;
          }
        }

        free_string( name );
        break;
      }
      break;

    case 'C':
      KEY( "Club", pVote->club, fread_string( pFile ) );
      break;

    case 'D':
      KEY( "Date", pVote->date, fread_string( pFile ) );
      KEY( "Days", pVote->days, fread_number( pFile ) );
      break;

    case 'E':

      if ( !str_scmp( word, "End", &fMatch ) ) RETURN( pVote );
      break;

    case 'L':
      KEY( "Level", pVote->level, fread_number( pFile ) );
      KEY( "Lock",  pVote->lock , fread_number( pFile ) );
      break;

    case 'M':
      KEY( "Moninal", pVote->moninal, fread_number( pFile ) );
      break;

    case 'P':
      KEY( "Poster", pVote->poster, fread_string( pFile ) );
      break;

    case 'S':
      KEY( "Subject", pVote->subject, fread_string( pFile ) );
      KEY( "Stamp"  , pVote->stamp,   fread_number( pFile ) );
      break;

    case 'T':

      if ( !str_scmp( word, "Text", &fMatch ) )
      {
        fread_to_eol( pFile ); /* 一定先讀完一行 */
        pVote->text = fread_string( pFile );

        break;
      }
      break;

    case 'V':

      if ( !str_scmp( word, "Vote", &fMatch ) )
      {
        int choice;

        choice = fread_number( pFile );
        choice = UMAX( 0, UMIN( MAX_VOTES - 1, choice ) );
        pVote->message[choice] = fread_string( pFile );
        break;
      }
      break;
    }

    /* 指令型態錯誤 */
    if ( !fMatch )
      mudlog( LOG_ERR , "Load_a_vote﹕命令 %s 不正確。" , word );
  }

  RETURN( NULL );
}

void load_symbol( const char * filename )
{
  SYMBOL_DATA * pSymbol;
  FILE_DATA   * fp;
  char        * word;
  char        * field;
  int           letter;
  int           count = 0;
  int           iLen;

  PUSH_FUNCTION( "load_symbol" );

  if ( !( fp = f_open( filename , "r" ) ) )
    mudlog( LOG_ERR , "無法開啟常數轉換檔 %s。" , filename );

  for ( ; ; )
  {
    word = fread_word( fp );
    if ( if_eof( fp ) ) break;

    /* symbol.def 每行以*為開頭的都當作是標記不會讀取 */
    if ( word[0] == '*' )
    {
      fread_to_eol( fp );
      continue;
    }

    /* symbol.def 若是出現 #END 則代表檔案結尾﹐之後的不會再讀取 */
    if ( !str_cmp( word , "#END" ) ) break;

    /* 配置記憶體 */
    pSymbol = alloc_struct( STRUCT_SYMBOL_DATA );

    pSymbol->str   = str_dup( word );
    pSymbol->num   = fread_number( fp );

    field          = fread_word( fp );
    pSymbol->field = str_dup( field );

    count++;

    if ( !isalpha( ( int ) pSymbol->str[0] ) )
      mudlog( LOG_ERR , "Load_symbol﹕%s 不是英文符號。" , pSymbol->str );

    letter                    = LOWER( pSymbol->str[0] ) - 'a';
    iLen                      = UMIN( 19, str_len( pSymbol->str ) - 1 );
    pSymbol->next             = symbol_hash[letter][iLen];
    symbol_hash[letter][iLen] = pSymbol;

    pSymbol->next_content = symbol_list;
    symbol_list           = pSymbol;
  }

  f_close( fp );
  mudlog( LOG_INFO , "系統載入 %d 個常數符號。" , count );
  RETURN_NULL();
}

void load_help( const char * path )
{
  glob_t      result;
  int         rc;
  int         count;
  HELP_DATA * pHelp;
  FILE_DATA * pFile;
  char        directory[ MAX_FILE_LENGTH ];
  char        buf[ MAX_FILE_LENGTH ];

  PUSH_FUNCTION( "load_help" );

  /* 處理目錄 */
  fill_path( strcpy( directory, path ) );

  if ( ( rc = glob_file( directory, help_ext, &result ) ) >= 0 )
  {
    for ( count = 0; count < rc; count++ )
    {
      sprintf( buf, "%s%s", directory, result.gl_pathv[count] );

      /* 附檔名為 hlp 的檔案 */
      if ( is_regular( buf ) )
      {
        if ( !( pFile = f_open( buf , "r" ) ) )
          mudlog( LOG_ERR , "Load_help﹕開啟求助檔 %s 有問題" , buf );

        pHelp          = alloc_struct( STRUCT_HELP_DATA );
        pHelp->level   = fread_number( pFile );
        pHelp->keyword = fread_string( pFile );
        pHelp->text    = fread_string( pFile );

        if ( !help_first ) help_first      = pHelp;
        if (  help_last  ) help_last->next = pHelp;

        help_last   = pHelp;
        pHelp->next = NULL;

        f_close( pFile );
        if ( !fReload ) top_help++;
      }
    }
    globfree( &result );
  }

  else
  {
    mudlog( LOG_ERR , "Load_help﹕沒有求助子目錄 %s。", path );
  }

  mudlog( LOG_INFO , "系統載入 %d 個求助檔。" , top_help );
  RETURN_NULL();
}

void load_social( const char * path , const char * index )
{
  SOCIAL_DATA   * pSocial;
  FILE_DATA     * pFile;
  FILE_DATA     * aFile;
  char          * word;
  char            directory[ MAX_FILE_LENGTH ];
  char            indexfile[ MAX_FILE_LENGTH ];
  char            buf[ MAX_FILE_LENGTH ];

  PUSH_FUNCTION( "load_social" );

  /* 處理目錄 */
  str_cpy( directory, path );
  fill_path( directory );

  sprintf( indexfile , "%s%s" , directory , index );

  if ( ( aFile = f_open( indexfile , "r" ) ) )
  {
    while ( !if_eof( aFile ) )
    {
      word = fread_word( aFile );
      fread_to_eol( aFile );

      if ( word[0] == '*' ) continue;

      sprintf( buf , "%s%c" , directory , LOWER( word[0] ) );
      fill_path( buf );
      str_cat( buf , word      );
      str_cat( buf, "."        );
      str_cat( buf, social_ext );

      if ( ( pFile = f_open( buf , "r" ) ) )
      {
        /* 配置記憶體 */
        pSocial = alloc_struct( STRUCT_SOCIAL_DATA );

        pSocial->name          = fread_string( pFile );
        pSocial->chinese_name  = fread_string( pFile );
        pSocial->char_no_arg   = fread_string( pFile );
        pSocial->others_no_arg = fread_string( pFile );
        pSocial->char_found    = fread_string( pFile );
        pSocial->others_found  = fread_string( pFile );
        pSocial->vict_found    = fread_string( pFile );
        pSocial->char_auto     = fread_string( pFile );
        pSocial->others_auto   = fread_string( pFile );

        if ( !social_first ) social_first      = pSocial;
        if (  social_last  ) social_last->next = pSocial;

        social_last   = pSocial;
        pSocial->next = NULL;
        if ( !fReload ) top_social++;
        f_close( pFile );
      }

      else
      {
        mudlog( LOG_ERR , "Load_social﹕無法開起社交動做檔 %s。" , buf );
      }
    }
  }
  else
  {
    mudlog( LOG_ERR , "Load_social﹕無法開啟社交索引檔 %s。" , indexfile );
  }

  f_close( aFile );
  mudlog( LOG_INFO , "系統載入 %d 個社交動作檔。" , top_social );
  RETURN_NULL();
}

void load_angel( const char * path )
{
  glob_t       result;
  int          rc;
  int          count;
  ANGEL_DATA * pAngel;
  FILE_DATA  * pFile;
  char         directory[ MAX_FILE_LENGTH ];
  char         buf[ MAX_FILE_LENGTH ];

  PUSH_FUNCTION( "load_angel" );

  /* 處理目錄 */
  fill_path( strcpy( directory, path ) );

  if ( ( rc = glob_file( directory, angel_ext, &result ) ) >= 0 )
  {
    for ( count = 0; count < rc; count++ )
    {
      sprintf( buf, "%s%s", directory, result.gl_pathv[count] );
      if ( is_regular( buf ) )
      {
        if ( !( pFile = f_open( buf , "r" ) ) )
          mudlog( LOG_ERR , "Load_angel﹕開啟守護神檔 %s 有問題。" , buf );

        pAngel       = load_a_angel( pFile );
        pAngel->next = angel_list;
        angel_list   = pAngel;

        f_close( pFile );
        if ( !fReload ) top_angel++;
      }
    }
    globfree( &result );
  }

  else
  {
    mudlog( LOG_ERR , "Load_angel﹕沒有守護神子目錄 %s。", path );
  }

  mudlog( LOG_INFO , "系統載入 %d 個守護神資料檔。" , top_angel );
  RETURN_NULL();
}

/* 讀取一個守護神資料 */
ANGEL_DATA * load_a_angel( FILE_DATA * pFile )
{
  ANGEL_DATA * pAngel;
  char       * word;
  bool         fMatch;

  PUSH_FUNCTION( "load_a_angel" );

  /* 配置記憶體以及設定預設值 */
  set_angel_default( pAngel = alloc_struct( STRUCT_ANGEL_DATA ) );

  for ( ;; )
  {
    word   = if_eof( pFile ) ? "End" : fread_word( pFile );
    fMatch = FALSE;

    switch ( UPPER( word[0] ) )
    {
    case '*':

      fMatch = TRUE;
      fread_to_eol( pFile );
      break;

    case 'D':

      if ( !str_scmp( word , "Description", &fMatch ) )
      {
        fread_to_eol( pFile ); /* 一定先讀完一行 */
        pAngel->description = fread_string( pFile );
        break;
      }

      break;

    case 'E':

      if ( !str_scmp( word, "End", &fMatch ) )
      {
        if ( pAngel->description && pAngel->function )
          mudlog( LOG_DEBUG, "Load_a_angel﹕守護神有函數又有描述。" );

        if ( !pAngel->description && !pAngel->function )
          mudlog( LOG_DEBUG, "Load_a_angel﹕守護神沒有函數又沒有描述。" );

        RETURN( pAngel );
      }

      break;

    case 'F':

      if ( !str_scmp( word , "Function", &fMatch ) )
      {
        char      * name;
        ANGEL_FUN * function;

        name = fread_string( pFile );

        if ( name[0] )
        {
          if ( !( function = angel_function( name ) ) )
            mudlog( LOG_DEBUG , "Load_a_angel﹕守護神的函數 %s 找不到。" , name );

          free_string( name );
          pAngel->function = function;
        }

        break;
      }
      break;

    case 'H':

      if ( !str_scmp( word, "Higher", &fMatch ) )
      {
        int level;

        if ( ( level = fread_number( pFile ) ) < 0 || level > MAX_LEVEL )
          mudlog( LOG_DEBUG, "Load_a_angel﹕守護最高等級 %d 不對。", level );

        pAngel->higher = level;
        break;
      }

      break;

    case 'L':

      if ( !str_scmp( word, "Lower", &fMatch ) )
      {
        int level;

        if ( ( level = fread_number( pFile ) ) < 0 || level > MAX_LEVEL )
          mudlog( LOG_DEBUG, "Load_a_angel﹕守護最低等級 %d 不對。", level );

        pAngel->lower = level;
        break;
      }

      break;

    case 'P':

      if ( !str_scmp( word, "Position", &fMatch ) )
      {
        int position;

        switch( ( position = fread_number( pFile ) ) )
        {
        default:
           mudlog( LOG_DEBUG, "Load_a_angel﹕守護狀態不對。", position );
           break;

        case POS_DEAD:
        case POS_SLEEPING:
        case POS_RESTING:
        case POS_FIGHTING:
        case POS_STANDING:
          break;
        }

        pAngel->position = position;
        break;
      }

      break;
    }

    /* 指令型態錯誤 */
    if ( !fMatch )
      mudlog( LOG_DEBUG , "Load_a_angel﹕命令 %s 不正確。" , word );
  }

  RETURN( NULL );
}

/* 載入進站畫面 */
void load_welcome_immortal( const char * filename )
{
  FILE_DATA * pFile;

  PUSH_FUNCTION( "load_welcome_immortal" );

  if ( !( pFile = f_open( filename , "r" ) ) )
    mudlog( LOG_ERR , "Load_welcome_immortal﹕開啟歡迎神族畫面檔 %s 有問題。"
    , filename );

  welcome_immortal = str_dup( map_file( pFile ) );

  f_close( pFile );
  RETURN_NULL();
}

/* 載入進站畫面 */
void load_welcome( const char * filename )
{
  FILE_DATA * pFile;

  PUSH_FUNCTION( "load_welcome" );

  if ( !( pFile = f_open( filename , "r" ) ) )
    mudlog( LOG_ERR , "Load_welcome﹕開啟歡迎畫面檔 %s 有問題。" , filename );

  welcome_message = str_dup( map_file( pFile ) );
  f_close( pFile );
  RETURN_NULL();
}

void load_motd( const char * filename )
{
  FILE_DATA * pFile;

  PUSH_FUNCTION( "load_motd" );

  if ( !( pFile = f_open( filename , "r" ) ) )
    mudlog( LOG_ERR , "Load_motd﹕開啟今日消息檔 %s 有問題。" , filename );

  motd_message = str_dup( map_file( pFile ) );
  f_close( pFile );

  RETURN_NULL();
}

/* 載入進站畫面 */
void load_greeting( const char * path )
{
  GREETING_DATA * pGreet;
  glob_t          result;
  int             rc;
  int             count;
  FILE_DATA     * pFile;
  char            directory[ MAX_FILE_LENGTH ];
  char            buf[ MAX_FILE_LENGTH ];

  PUSH_FUNCTION( "load_greeting" );

  /* 處理目錄 */
  fill_path( strcpy( directory, path ) );

  if ( ( rc = glob_file( directory, greet_ext, &result ) ) >= 0 )
  {
    for ( count = 0; count < rc; count++ )
    {
      sprintf( buf, "%s%s", directory, result.gl_pathv[count] );

      if ( is_regular( buf ) )
      {
        if ( !( pFile = f_open( buf , "r" ) ) )
          mudlog( LOG_ERR , "Load_greeting﹕開啟進站畫面檔 %s 有問題。" , buf );

        pGreet        = alloc_struct( STRUCT_GREETING_DATA );
        pGreet->text  = str_dup( map_file( pFile ) );
        pGreet->next  = greeting_list;
        greeting_list = pGreet;
        if ( !fReload ) top_greeting++;
        f_close( pFile );
      }
    }
    globfree( &result );
  }

  else
  {
    mudlog( LOG_ERR , "Load_greeting﹕沒有進站畫面子目錄 %s。", path );
  }

  if ( top_greeting <= 0 )
    mudlog( LOG_ERR, "Load_greeting﹕至少要有一個進站畫面。" );

  mudlog( LOG_INFO , "系統載入 %d 個進站畫面檔。" , top_greeting );
  RETURN_NULL();
}

/* 載入地形資料檔案, 副檔名為 def */
void load_sector( const char * path )
{
  glob_t        result;
  int           rc;
  int           count;
  FILE_DATA   * pFile;
  char        * word;
  SECTOR_DATA * pSector;
  char          directory[ MAX_FILE_LENGTH ];
  char          buf[ MAX_FILE_LENGTH ];
  bool          fMatch;
  bool          finish;
  int           vnum;

  PUSH_FUNCTION( "load_sector" );

  /* 處理目錄 */
  fill_path( strcpy( directory, path ) );

  if ( ( rc = glob_file( directory, sector_ext, &result ) ) >= 0 )
  {
    for ( count = 0; count < rc; count++ )
    {
      sprintf( buf, "%s%s", directory, result.gl_pathv[count] );

      if ( is_regular( buf ) )
      {
        if ( !( pFile = f_open( buf , "r" ) ) )
          mudlog( LOG_ERR , "Load_sector﹕開啟地形檔案 %s 有問題。" , buf );

        pSector = alloc_struct( STRUCT_SECTOR_DATA );
        set_sector_default( pSector );

        for ( finish = FALSE; finish != TRUE; )
        {
          word   = if_eof( pFile ) ? "End" : fread_word( pFile );
          fMatch = FALSE;

          switch ( UPPER( word[0] ) )
          {
          /* 註解 */
          case '*':
            fMatch = TRUE;
            fread_to_eol( pFile );
            break;

          case 'D':

            if ( !str_scmp( word , "Dark", &fMatch ) )
            {
              pSector->dark = ( fread_number( pFile ) == TRUE )
                              ? TRUE : FALSE;
              break;
            }

            if ( !str_scmp( word, "Default", &fMatch ) )
            {

              if ( fread_number( pFile ) == TRUE )
              {
                if ( DefaultSector )
                  mudlog( LOG_DEBUG, "Load_sector﹕預設地形重覆。" );

                DefaultSector = pSector;
              }

              break;
            }
            break;

          case 'E':

            if ( !str_scmp( word, "End", &fMatch ) )
            {
              if ( IS_ERROR( pSector->vnum ) )
                mudlog( LOG_DEBUG , "Load_sector﹕地形檔案沒有編號。" );

              if ( IS_ERROR( pSector->movement ) )
                mudlog( LOG_DEBUG , "Load_sector﹕地形檔案沒有移動消耗數值。" );

              if ( !pSector->cname )
                mudlog( LOG_DEBUG , "Load_sector﹕地形檔案沒有中文名稱。" );

              pSector->next   = sector_list;
              sector_list     = pSector;

              if ( !fReload ) top_sector++;
              finish = TRUE;
              break;
            }
            break;

          case 'F':

            KEY( "Fly"  , pSector->fly  , fread_number( pFile ) );
            break;

          case 'M':

            KEY( "Movement", pSector->movement, fread_number( pFile ) );
            break;

          case 'N':

            KEY( "Name" , pSector->cname , fread_string( pFile ) );
            break;

          case 'S':
            KEY( "Swim", pSector->swim, fread_number( pFile ) );
            break;

          case 'V':

            if ( !str_scmp( word , "Vnum", &fMatch ) )
            {
              if ( get_sector_index( ( vnum = fread_number( pFile ) ) ) )
                mudlog( LOG_DEBUG , "Load_sector﹕編號 %d 的地型號碼重覆。"
                  , vnum );

              if ( vnum < 0 )
                mudlog( LOG_DEBUG , "Load_sector﹕編號 %d 的地型檔案的編號不合法。"
                  , vnum );

              pSector->vnum = vnum;
              break;
            }

            break;
          }

          /* 指令型態錯誤 */
          if ( !fMatch )
            mudlog( LOG_DEBUG , "Load_sector﹕命令 %s 不正確。" , word );
        }

        f_close( pFile );
      }
    }
    globfree( &result );
  }

  else
  {
    mudlog( LOG_ERR , "Load_sector﹕沒有地形資料的子目錄 %s。", path );
  }

  if ( !DefaultSector ) mudlog( LOG_ERR, "Load_sector﹕沒有預設地形。" );

  mudlog( LOG_INFO , "系統載入 %d 個地形資料檔。" , top_sector );
  RETURN_NULL();
}

/* 載入職業資料檔案, 副檔名為 def */
void load_class( const char * path )
{
  glob_t       result;
  int          rc;
  int          count;
  CLASS_DATA * pClass;
  CLASS_DATA * zClass;
  FILE_DATA  * pFile;
  char       * word;
  char         directory[ MAX_FILE_LENGTH ];
  char         buf[ MAX_FILE_LENGTH ];
  int          amount;
  bool         fMatch;
  bool         finish;

  PUSH_FUNCTION( "load_class" );

  /* 處理目錄 */
  fill_path( strcpy( directory, path ) );

  if ( ( rc = glob_file( directory, class_ext, &result ) ) >= 0 )
  {
    for ( count = 0; count < rc; count++ )
    {
      sprintf( buf, "%s%s", directory, result.gl_pathv[count] );

      if ( is_regular( buf ) )
      {
        if ( !( pFile = f_open( buf , "r" ) ) )
          mudlog( LOG_ERR , "Load_class﹕開啟職業檔案 %s 有問題。" , buf );

        pClass = alloc_struct( STRUCT_CLASS_DATA );
        set_class_default( pClass );

        for ( finish = FALSE; finish != TRUE; )
        {
          word   = if_eof( pFile ) ? "End" : fread_word( pFile );
          fMatch = FALSE;

          switch ( UPPER( word[0] ) )
          {
          /* 註解 */
          case '*':
            fMatch = TRUE;
            fread_to_eol( pFile );
            break;

          case 'A':
            KEY( "Associate", pClass->associate, fread_number( pFile ) );
            break;

          case 'C':
            KEY( "Cname", pClass->cname , fread_string( pFile ) );

            if ( !str_scmp( word , "Conmax", &fMatch ) )
            {
              amount = fread_number( pFile );
              amount = UMAX(       0, amount );
              amount = UMIN( MaxCon, amount );
              pClass->attr[CON_ITEM] = amount;
              break;
            }

            if ( !str_scmp( word , "Confactor", &fMatch ) )
            {
              amount = fread_number( pFile );
              amount = UMAX(   0, amount );
              amount = UMIN( 100, amount );
              pClass->factor[CON_ITEM] = amount;
              break;
            }

            if ( !str_scmp( word , "ConDefaultMax", &fMatch ) )
            {
              amount = fread_number( pFile );
              amount = UMAX(   0, amount );
              amount = UMIN( 100, amount );
              pClass->max_default[CON_ITEM] = amount;
              break;
            }

            if ( !str_scmp( word , "ConDefaultMin", &fMatch ) )
            {
              amount = fread_number( pFile );
              amount = UMAX(   0, amount );
              amount = UMIN( 100, amount );
              pClass->min_default[CON_ITEM] = amount;
              break;
            }

            break;

          case 'D':

            if ( !str_scmp( word , "Dexmax", &fMatch ) )
            {
              amount = fread_number( pFile );
              amount = UMAX(       0, amount );
              amount = UMIN( MaxDex, amount );
              pClass->attr[DEX_ITEM] = amount;
              break;
            }

            if ( !str_scmp( word , "Dexfactor", &fMatch ) )
            {
              amount = fread_number( pFile );
              amount = UMAX(   0, amount );
              amount = UMIN( 100, amount );
              pClass->factor[DEX_ITEM] = amount;
              break;
            }

            if ( !str_scmp( word , "DexDefaultMax", &fMatch ) )
            {
              amount = fread_number( pFile );
              amount = UMAX(   0, amount );
              amount = UMIN( 100, amount );
              pClass->max_default[DEX_ITEM] = amount;
              break;
            }

            if ( !str_scmp( word , "DexDefaultMin", &fMatch ) )
            {
              amount = fread_number( pFile );
              amount = UMAX(   0, amount );
              amount = UMIN( 100, amount );
              pClass->min_default[DEX_ITEM] = amount;
              break;
            }

            break;

          case 'E':
            if ( !str_scmp( word , "End", &fMatch ) )
            {
              if ( IS_ERROR( pClass->vnum ) )
                mudlog( LOG_DEBUG , "Load_class﹕職業檔沒有編號。" );

              if ( pClass->mask == 0 )
                mudlog( LOG_DEBUG , "Load_class﹕職業檔沒有遮罩旗標。" );

              if ( !pClass->name )
                mudlog( LOG_DEBUG, "Load_class﹕職業檔沒有英文名稱。" );

              if ( !pClass->cname )
                mudlog( LOG_DEBUG, "Load_class﹕職業檔沒有中文名稱。" );

              if ( !pClass->title )
                mudlog( LOG_DEBUG, "Load_class﹕職業檔沒有抬頭。" );

              if ( pClass->limit > 0 && !pClass->msg_limit )
                mudlog( LOG_DEBUG, "Load_class﹕職業檔沒有限制敘述。" );

              if ( pClass->multiplier <= 1 )
                mudlog( LOG_DEBUG, "Load_class﹕職業檔極限倍數不合理。" );

              if ( pClass->nskill <= 0 )
                mudlog( LOG_DEBUG, "Load_class﹕職業檔的技能總數不合理。" );

              pClass->next = class_list;
              class_list   = pClass;

              if ( !fReload ) top_class++;
              finish = TRUE;
              break;
            }
            break;

          case 'F':

            if ( !str_scmp( word, "Force", &fMatch ) )
            {
              pClass->force = fread_number( pFile ) == FALSE ? FALSE : TRUE;
              break;
            }

            break;

          case 'H':

            if ( !str_scmp( word , "HeroCon", &fMatch ) )
            {
              amount = fread_number( pFile );
              amount = UMAX(   0, amount );
              amount = UMIN( 100, amount );
              pClass->hero[CON_ITEM] = amount;
              break;
            }

            if ( !str_scmp( word , "HeroStr", &fMatch ) )
            {
              amount = fread_number( pFile );
              amount = UMAX(   0, amount );
              amount = UMIN( 100, amount );
              pClass->hero[STR_ITEM] = amount;
              break;
            }

            if ( !str_scmp( word , "HeroInt", &fMatch ) )
            {
              amount = fread_number( pFile );
              amount = UMAX(   0, amount );
              amount = UMIN( 100, amount );
              pClass->hero[INT_ITEM] = amount;
              break;
            }

            if ( !str_scmp( word , "HeroWis", &fMatch ) )
            {
              amount = fread_number( pFile );
              amount = UMAX(   0, amount );
              amount = UMIN( 100, amount );
              pClass->hero[WIS_ITEM] = amount;
              break;
            }

            if ( !str_scmp( word , "HeroDex", &fMatch ) )
            {
              amount = fread_number( pFile );
              amount = UMAX(   0, amount );
              amount = UMIN( 100, amount );
              pClass->hero[DEX_ITEM] = amount;
              break;
            }

            break;

          case 'I':

            if ( !str_scmp( word , "Intmax", &fMatch ) )
            {
              amount = fread_number( pFile );
              amount = UMAX(       0, amount );
              amount = UMIN( MaxInt, amount );
              pClass->attr[INT_ITEM] = amount;
              break;
            }

            if ( !str_scmp( word , "Intfactor", &fMatch ) )
            {
              amount = fread_number( pFile );
              amount = UMAX(   0, amount );
              amount = UMIN( 100, amount );
              pClass->factor[INT_ITEM] = amount;
              break;
            }

            if ( !str_scmp( word , "IntDefaultMax", &fMatch ) )
            {
              amount = fread_number( pFile );
              amount = UMAX(   0, amount );
              amount = UMIN( 100, amount );
              pClass->max_default[INT_ITEM] = amount;
              break;
            }

            if ( !str_scmp( word , "IntDefaultMin", &fMatch ) )
            {
              amount = fread_number( pFile );
              amount = UMAX(   0, amount );
              amount = UMIN( 100, amount );
              pClass->min_default[INT_ITEM] = amount;
              break;
            }

            break;

          case 'L':

            KEY( "Lowerlimit", pClass->low_rebirth, fread_number( pFile ) );
            KEY( "LimitMsg"  , pClass->msg_limit  , fread_string( pFile ) );
            KEY( "Limit"     , pClass->limit      , fread_number( pFile ) );
            break;

          case 'M':

            KEY( "Multiplier", pClass->multiplier, fread_number( pFile ) );

            if ( !str_scmp( word , "Mana", &fMatch ) )
            {
              pClass->fMana = fread_number( pFile ) == TRUE ? TRUE : FALSE;
              break;
            }
            break;

          case 'N':
            KEY( "Name"  , pClass->name  , fread_string( pFile ) );
            KEY( "NSkill", pClass->nskill, fread_number( pFile ) );
            break;

          case 'R':

            KEY( "Rebirthwarn", pClass->warn , fread_number( pFile ) );

            if ( !str_scmp( word, "Rudiment", &fMatch ) )
            {
              pClass->rudiment = fread_number( pFile ) == FALSE ? FALSE : TRUE;
              break;
            }

            if ( !str_scmp( word , "Rebirthgold", &fMatch ) )
            {
              if ( ( amount = fread_number( pFile ) ) < 0 )
                mudlog( LOG_DEBUG , "Load_class﹕編號 %d 的職業檔案的轉職金不合法。"
                  , amount );

              pClass->rebirth_gold = amount;
              break;
            }

            if ( !str_scmp( word, "Rank", &fMatch ) )
            {
              if ( ( amount = fread_number( pFile ) ) < 0
                || amount > MAX_LEVEL )
                mudlog( LOG_DEBUG, "Load_class﹕編號 %d 的抬頭不合法。", amount );

              if ( pClass->rank[amount] )
                mudlog( LOG_DEBUG, "Load_class﹕編號 %d 的抬頭重複定義。", amount );

              pClass->rank[amount] = str_dup( fread_word( pFile ) );
              break;
            }
            break;

          case 'S':

            if ( !str_scmp( word, "Select", &fMatch ) )
            {
              pClass->select = fread_number( pFile ) == FALSE ? FALSE : TRUE;
              break;
            }

            if ( !str_scmp( word , "Strmax", &fMatch ) )
            {
              amount = fread_number( pFile );
              amount = UMAX(       0, amount );
              amount = UMIN( MaxStr, amount );
              pClass->attr[STR_ITEM] = amount;
              break;
            }

            if ( !str_scmp( word , "Strfactor", &fMatch ) )
            {
              amount = fread_number( pFile );
              amount = UMAX(   0, amount );
              amount = UMIN( 100, amount );
              pClass->factor[STR_ITEM] = amount;
              break;
            }

            if ( !str_scmp( word , "StrDefaultMax", &fMatch ) )
            {
              amount = fread_number( pFile );
              amount = UMAX(   0, amount );
              amount = UMIN( 100, amount );
              pClass->max_default[STR_ITEM] = amount;
              break;
            }

            if ( !str_scmp( word , "StrDefaultMin", &fMatch ) )
            {
              amount = fread_number( pFile );
              amount = UMAX(   0, amount );
              amount = UMIN( 100, amount );
              pClass->min_default[STR_ITEM] = amount;
              break;
            }

            break;

          case 'T':
            KEY( "Title", pClass->title, fread_string( pFile ) );
            break;

          case 'U':

            if ( !str_scmp( word, "UpperLimit", &fMatch ) )
            {
              if ( ( amount = fread_number( pFile ) ) > MAX_LEVEL )
                mudlog( LOG_DEBUG , "Load_class﹕編號 %d 的職業檔案的上限等級不合法。"
                 , amount );

              pClass->high_rebirth = amount;
              break;
            }
            break;

          case 'V':

            if ( !str_scmp( word , "Vnum", &fMatch ) )
            {
              int mask;

              if ( class_lookup( amount = fread_number( pFile ) ) )
                mudlog( LOG_DEBUG , "Load_class﹕編號 %d 的職業號碼重覆。" , amount );

              if ( amount < 0 || amount >= 32 )
                mudlog( LOG_DEBUG , "Load_class﹕編號 %d 的職業檔案的編號不合法。"
                  , amount );

              if ( amount < 0 || amount >= 32 )
                mudlog( LOG_DEBUG, "Load_class﹕職業編號不合理。" );

              mask = 1 << amount;

              for ( zClass = class_list; zClass; zClass = zClass->next )
              {
                if ( mask == zClass->mask )
                  mudlog( LOG_DEBUG, "Load_class﹕職業遮罩旗標重覆。" );
              }

              pClass->mask = mask;
              pClass->vnum = amount;

              break;
            }

          case 'W':

            if ( !str_scmp( word , "Wismax", &fMatch ) )
            {
              amount = fread_number( pFile );
              amount = UMAX(       0, amount );
              amount = UMIN( MaxWis, amount );
              pClass->attr[WIS_ITEM] = amount;
              break;
            }

            if ( !str_scmp( word , "Wisfactor", &fMatch ) )
            {
              amount = fread_number( pFile );
              amount = UMAX(   0, amount );
              amount = UMIN( 100, amount );
              pClass->factor[WIS_ITEM] = amount;
              break;
            }

            if ( !str_scmp( word , "WisDefaultMax", &fMatch ) )
            {
              amount = fread_number( pFile );
              amount = UMAX(   0, amount );
              amount = UMIN( 100, amount );
              pClass->max_default[WIS_ITEM] = amount;
              break;
            }

            if ( !str_scmp( word , "WisDefaultMin", &fMatch ) )
            {
              amount = fread_number( pFile );
              amount = UMAX(   0, amount );
              amount = UMIN( 100, amount );
              pClass->min_default[WIS_ITEM] = amount;
              break;
            }

            break;

          }

          /* 指令型態錯誤 */
          if ( !fMatch )
            mudlog( LOG_DEBUG , "Load_class﹕命令 %s 不正確。" , word );
        }

        f_close( pFile );
      }
    }
    globfree( &result );

    /* 找尋預設的職業 */
    for ( amount = 0, class_force = class_demos = NULL, pClass = class_list;
      pClass;
      pClass = pClass->next )
    {
      if ( amount >= MAX_DEFAULT_CLASS )
        mudlog( LOG_ERR, "Load_class﹕預設職業太多。" );

      if ( pClass->select ) iClass[amount++] = pClass;

      if ( pClass->rudiment )
      {
        if ( class_demos ) mudlog( LOG_ERR, "Load_class﹕預設職業 %d 重複。"
          , pClass->vnum );
        class_demos = pClass;
      }

      if ( pClass->force )
      {
        if ( class_force ) mudlog( LOG_ERR, "Load_class﹕強制職業 %d 重複。"
          , pClass->vnum );
        class_force = pClass;
      }
    }

    if ( !class_demos ) mudlog( LOG_ERR, "Load_class﹕沒有預設的職業。" );
    if ( !class_force ) mudlog( LOG_ERR, "Load_class﹕沒有強制的職業。" );
    if ( amount <= 0  ) mudlog( LOG_ERR, "Load_class﹕沒有選擇的職業。" );
  }

  else
  {
    mudlog( LOG_ERR , "Load_class﹕沒有職業資料的子目錄 %s。", path );
  }

  mudlog( LOG_INFO , "系統載入 %d 個職業資料檔。" , top_class );
  RETURN_NULL();
}

/* 載入液體資料檔案, 副檔名為 def */
void load_liq( const char * path )
{
  glob_t      result;
  int         rc;
  int         count;
  LIQ_DATA  * pLiq;
  FILE_DATA * pFile;
  char      * word;
  char        directory[ MAX_FILE_LENGTH ];
  char        buf[ MAX_FILE_LENGTH ];
  bool        fMatch;
  bool        finish;

  PUSH_FUNCTION( "load_liq" );

  /* 處理目錄 */
  fill_path( strcpy( directory, path ) );

  if ( ( rc = glob_file( directory, liquid_ext, &result ) ) >= 0 )
  {
    for ( count = 0; count < rc; count++ )
    {
      sprintf( buf, "%s%s", directory, result.gl_pathv[count] );

      if ( is_regular( buf ) )
      {
        if ( !( pFile = f_open( buf , "r" ) ) )
          mudlog( LOG_ERR , "Load_liq﹕開啟液體檔案 %s 有問題。" , buf );

        set_liq_default( pLiq = alloc_struct( STRUCT_LIQ_DATA ) );

        for ( finish = FALSE; finish != TRUE; )
        {
          word   = if_eof( pFile ) ? "End" : fread_word( pFile );
          fMatch = FALSE;

          switch ( UPPER( word[0] ) )
          {
          /* 註解 */
          case '*':
            fMatch = TRUE;
            fread_to_eol( pFile );
            break;

          case 'C':
            KEY( "Color" , pLiq->color , fread_string( pFile ) );
            break;

          case 'D':
            KEY( "Drunk" , pLiq->drunk , fread_number( pFile ) );
            break;

          case 'E':
            if ( !str_scmp( word , "End", &fMatch ) )
            {
              if ( !pLiq->name     ) mudlog( LOG_DEBUG, "Load_liq﹕液體檔沒有名稱。" );
              if ( !pLiq->color    ) mudlog( LOG_DEBUG, "Load_liq﹕液體檔沒有顏色。" );
              if ( pLiq->slot <= 0 ) mudlog( LOG_DEBUG, "Load_liq﹕液體沒有編號。"  );

              pLiq->next = liq_list;
              liq_list   = pLiq;
              if ( !fReload ) top_liq++;

              finish = TRUE;
              break;
            }
            break;

          case 'F':
            KEY( "Full" , pLiq->full , fread_number( pFile ) );
            break;

          case 'N':
            KEY( "Name" , pLiq->name , fread_string( pFile ) );
            break;

          case 'S':

            if ( !str_scmp( word, "Slot", &fMatch ) )
            {
              if ( liq_lookup(  pLiq->slot = fread_number( pFile ) ) )
                mudlog( LOG_DEBUG, "Load_liq﹕液體編號 %d 重複。", pLiq->slot );

              break;
            }
            break;

          case 'T':
            KEY( "Thirst" , pLiq->thirst , fread_number( pFile ) );
            break;

          case 'W':

            if ( !str_scmp( word, "Water", &fMatch ) )
            {
              pLiq->water = fread_number( pFile ) == FALSE ? FALSE : TRUE;
              break;
            }
            break;
          }

          /* 指令型態錯誤 */
          if ( !fMatch )
             mudlog( LOG_DEBUG , "Load_liq﹕命令 %s 不正確。" , word );
        }

        f_close( pFile );
      }
    }
    globfree( &result );

    /* 找尋為水的液體 */
    for ( liq_water = NULL, pLiq = liq_list; pLiq; pLiq = pLiq->next )
    {
      if ( pLiq->water )
      {
        if ( liq_water ) mudlog( LOG_ERR, "Load_liq﹕液體水 %d 重複。", pLiq->slot );
        liq_water = pLiq;
      }
    }

    if ( !liq_water ) mudlog( LOG_ERR, "Load_liq﹕沒有水的液體。" );
  }

  else
  {
    mudlog( LOG_ERR , "Load_liq﹕沒有液體資料的子目錄 %s。", path );
  }

  mudlog( LOG_INFO , "系統載入 %d 個液體資料檔。" , top_liq );
  RETURN_NULL();
}

void load_site( const char * filename )
{
  BAN_DATA  * pBan;
  FILE_DATA * File;
  char      * word;
  int         ban_type;

  PUSH_FUNCTION( "load_site" );

  if ( !( File = f_open( filename , "r" ) ) )
    mudlog( LOG_ERR , "Load_site﹕無法開啟禁止連線檔案 %s。" , filename );

  for ( ;; )
  {
    if ( if_eof( File ) ) break;

    word = fread_word( File );

    /* 每行以 * 為開頭的都當作是標記不會讀取 */
    if ( word[0] == '*' )
    {
      fread_to_eol( File );
      continue;
    }

    /* 若是出現 #END 則代表檔案結尾﹐之後的不會再讀取 */
    if ( !str_cmp( word , "#END" ) ) break;

    ban_type = fread_number( File );

    switch( ban_type )
    {
    default :
      mudlog( LOG_DEBUG , "Load_site﹕載入位址 %s 資訊錯誤 %d。" , word , ban_type );
      break;

    case BAN_LOGIN  :
    case BAN_FQDN   :
    case BAN_FINGER :
      break;
    }

    /* 配置記憶體 */
    pBan = alloc_struct( STRUCT_BAN_DATA );

    pBan->name = str_dup( word );
    pBan->type = ban_type;
    pBan->next = ban_list;
    ban_list   = pBan;
    mudlog( LOG_INFO , "系統設定 %s 的位址旗標 %d。" , word , ban_type );
  }

  f_close( File );
  RETURN_NULL();
}

void load_server( const char * filename )
{
  SERVER_DATA  * pServer;
  FILE_DATA    * File;
  char         * word;
  int            count;

  PUSH_FUNCTION( "load_server" );

  if ( !( File = f_open( filename , "r" ) ) )
    mudlog( LOG_ERR , "Load_server﹕無法開啟工作站檔案 %s。", filename );

  for ( count = 0; ; )
  {
    if ( if_eof( File ) ) break;
    word = fread_word( File );

    /* 每行以 * 為開頭的都當作是標記不會讀取 */
    if ( word[0] == '*' )
    {
      fread_to_eol( File );
      continue;
    }

    /* 若是出現 #END 則代表檔案結尾﹐之後的不會再讀取 */
    if ( !str_cmp( word , "#END" ) ) break;

    /* 配置記憶體 */
    set_server_default( pServer = alloc_struct( STRUCT_SERVER_DATA ) );
    pServer->address = str_dup( word );
    pServer->next    = server_list;
    server_list      = pServer;
    count++;

  }

  f_close( File );
  mudlog( LOG_INFO, "系統設定 %d 個工作站位址。", count );

  RETURN_NULL();
}

void load_xnames( const char * filename )
{
  XNAMES_DATA * pXnames;
  FILE_DATA   * File;
  char        * word;
  int           count = 0;
  int           xnames_type;

  PUSH_FUNCTION( "load_xname" );

  if ( !( File = f_open( filename , "r" ) ) )
    mudlog( LOG_ERR , "Load_xnames﹕無法開啟保留字檔案 %s。" , filename );

  for ( ;; )
  {
    if ( if_eof( File ) ) break;
    word = fread_word( File );

    /* symbol.def 每行以*為開頭的都當作是標記不會讀取 */
    if ( word[0] == '*' )
    {
      fread_to_eol( File );
      continue;
    }

    /* 若是出現 #END 則代表檔案結尾﹐之後的不會再讀取 */
    if ( !str_cmp( word , "#END" ) ) break;

    switch( ( xnames_type = fread_number( File ) ) )
    {
    default :
      mudlog( LOG_DEBUG , "Load_xnames﹕載入保留字 %s 錯誤旗標 %d。"
        , word , xnames_type );

      break;

    case XNAMES_SYSTEM  :
    case XNAMES_CACONYM :
    case XNAMES_CHAT    :
    case XNAMES_CNAME   :
      break;
    }

    /* 配置記憶體 */
    pXnames = alloc_struct( STRUCT_XNAME_DATA );

    pXnames->word = str_dup( word );
    pXnames->type = xnames_type;
    pXnames->next = xnames_first;
    xnames_first  = pXnames;
    count++;
  }

  mudlog( LOG_INFO , "系統載入 %d 個保留字。" , count );
  f_close( File );
  RETURN_NULL();
}

void load_skill( const char * path , const char * index )
{
  RESTRICT_DATA * pRestrict;
  RESTRICT_DATA * zRestrict;
  SKILL_DATA    * pSkill;
  DAMAGE_DATA   * pDamage;
  DAMAGE_DATA   * zDamage;
  FILE_DATA     * pFile;
  FILE_DATA     * aFile;
  char          * word;
  char            indexfile[ MAX_FILE_LENGTH ];
  char            buf[MAX_FILE_LENGTH];
  char            directory[ MAX_FILE_LENGTH ];
  bool            fMatch;
  bool            finish;

  PUSH_FUNCTION( "load_skill" );

  /* 處理目錄 */
  str_cpy( directory , path );
  fill_path( directory );

  /* 處理索引檔案名稱 */
  sprintf( indexfile , "%s%s" , directory , index );

  /* 開啟索引檔案, 不能沒有這個檔案 */
  if ( ( pFile = f_open( indexfile , "r" ) ) )
  {
    /* 一直讀取到檔案末端 */
    while ( !if_eof( pFile ) )
    {
      /* 讀取命令檔案的名稱 */
      word = fread_word( pFile );
      fread_to_eol( pFile );

      if ( word[0] == '*' ) continue;

      /* 處理欲開啟命令檔案的名稱 */
      sprintf( buf , "%s%c/%s.%s"
        , directory , LOWER( word[0] ) , word, skill_ext );

      if ( ( aFile = f_open( buf , "r" ) ) )
      {
        /* 配置記憶體 */
        set_skill_default( pSkill = alloc_struct( STRUCT_SKILL_DATA ) );

        /* 載入技能的主程式部份 */
        for ( finish = FALSE; finish != TRUE; )
        {
          word   = if_eof( aFile ) ? "End" : fread_word( aFile );
          fMatch = FALSE;

          switch ( UPPER( word[0] ) )
          {
          /* 註解 */
          case '*':
            fMatch = TRUE;
            fread_to_eol( aFile );
            break;

          case '#':

            if ( !str_scmp( word, "#DAMAGE", &fMatch ) )
            {
              pDamage = load_damage( aFile );

              /* 找尋最後一個 */
              for ( zDamage = pSkill->damage;
                    zDamage && zDamage->next;
                    zDamage = zDamage->next );

              if ( !zDamage ) pSkill->damage = pDamage;
              else            zDamage->next  = pDamage;

              break;
            }

            if ( !str_scmp( word, "#Restrict", &fMatch ) )
            {
              /* 載入技能限制 */
              if ( !( pRestrict = load_restrict( aFile ) ) )
              {
                mudlog( LOG_DEBUG, "Load_skill﹕載入技能限制失敗。" );
                break;
              }

              for ( zRestrict = pSkill->restrict; zRestrict; zRestrict = zRestrict->next )
              {
                if ( zRestrict->type == pRestrict->type
                  && zRestrict->type != RES_SKILL )
                {
                  mudlog( LOG_DEBUG , "Load_skill﹕技能限制型態 %d 重複。"
                    , pRestrict->type );
                }
              }

              pRestrict->next  = pSkill->restrict;
              pSkill->restrict = pRestrict;
              break;
            }

            if ( !str_scmp( word, "#Affect", &fMatch ) )
            {
              if ( pSkill->affected )
                mudlog( LOG_ERR, "Load_skill﹕%s 已有影響效應。", pSkill->name );

              if ( !( pSkill->affected = load_affect( aFile ) ) )
              {
                mudlog( LOG_DEBUG, "Load_skill﹕載入法術影響失敗。" );
                break;
              }

              pSkill->affected->duration = -1;
              pSkill->affected->location = 0;
              pSkill->affected->modifier = 0;
              pSkill->affected->level    = MAX_LEVEL;

              if ( !fReload ) top_affect++;
              break;
            }

            break;

          case 'A':

            KEY( "Associate" , pSkill->associate , fread_number( aFile ) );
            KEY( "Affect"    , pSkill->affect    , fread_number( aFile ) );
            KEY( "AffectID"  , pSkill->affect_id , fread_number( aFile ) );

            if ( !str_scmp( word, "AntiRating", &fMatch ) )
            {
              switch( pSkill->antirating = fread_number( aFile ) )
              {
              default:
                mudlog( LOG_DEBUG ,
                  "Load_skill﹕技能相剋種類 %d 不合理。" ,pSkill->antirating );
                break;

              case RATING_SKILL:
              case RATING_WIND:
              case RATING_EARTH:
              case RATING_LIGHTNING:
              case RATING_POISON:
              case RATING_FIRE:
              case RATING_WATER:
              case RATING_DARKNESS:
              case RATING_LIGHT:
              case RATING_SAINT:
              case RATING_EVIL:
              case RATING_LOST:
              case RATING_CURE:
              case RATING_SING:
              case RATING_FIGHT:
              case RATING_MURDER:
              case RATING_CREATE:
              case RATING_THIEF:
                break;

              }

              break;
            }

            if ( !str_scmp( word , "Adeptation", &fMatch ) )
            {
              if ( ( pSkill->adeptation = fread_number( aFile ) ) < 0
                || pSkill->adeptation > 100 )
                mudlog( LOG_DEBUG , "Load_skill﹕選取熟練度%d不合理。"
                  , pSkill->adeptation );

              break;
            }

            break;

          case 'C':

            if ( !str_scmp( word , "Check", &fMatch ) )
            {
              char      * name;
              CHECK_FUN * check;

              name = fread_string( aFile );

              if ( !( check = check_function_name( name ) ) )
                mudlog( LOG_DEBUG , "Load_skill﹕技能檔的限制函數 %s 找不到。" , name );

              free_string( name );
              pSkill->check = check;
              break;
            }

            if ( !str_scmp( word , "Cname", &fMatch ) )
            {
              SKILL_DATA * aSkill;
              char       * name;

              name = fread_string( aFile );
              for ( aSkill = skill_list; aSkill; aSkill = aSkill->next )
              {
                if ( !str_cmp( aSkill->cname , name ) )
                {
                  mudlog( LOG_DEBUG ,
                    "Load_skill﹕技能中文名稱 %s 重覆。" , name );
                }
              }

              pSkill->cname = name;
              break;
            }

            if ( !str_scmp( word , "Concentration", &fMatch ) )
            {
              pSkill->concentration = fread_number( aFile ) == TRUE
                                       ? TRUE : FALSE;
              break;
            }

            if ( !str_scmp( word , "CanAsk", &fMatch ) )
            {
              pSkill->canask = fread_number( aFile ) == TRUE ? TRUE : FALSE;
              break;
            }

            if ( !str_scmp( word , "Cast", &fMatch ) )
            {
              pSkill->cast = fread_number( aFile ) == TRUE ? TRUE : FALSE;
              break;
            }

            if ( !str_scmp( word , "Costtype", &fMatch ) )
            {
              int type;

              switch ( type = fread_number( aFile ) )
              {
              default:
                mudlog( LOG_DEBUG , "Load_skill﹕消耗型態 %d 錯誤。", type );
                break;

              case COST_HIT:
              case COST_MANA:
              case COST_MOVE:
              case COST_GOLD:
                break;
              }

              pSkill->cost_type = type;
              break;
            }

            KEY( "Cost" , pSkill->cost , fread_number( aFile ) );
            break;

          case 'D':

            if ( !str_scmp( word , "Degree" , &fMatch ) )
            {
              int degree;

              degree = fread_number( aFile );
              if ( degree < 0 || degree > 1000 )
                mudlog( LOG_DEBUG , "Load_skill﹕程度 %d 不合法。" , degree );

              pSkill->degree = degree;
              break;
            }

            break;

          case 'E':

            if ( !str_scmp( word , "Enable", &fMatch ) )
            {
              pSkill->enable = fread_number( aFile ) == 1 ? TRUE : FALSE;
              break;
            }

            if ( !str_scmp( word, "End", &fMatch ) )
            {
              if ( IS_ERROR( pSkill->slot ) )
                mudlog( LOG_DEBUG, "Load_skill﹕技能沒有編號。" );

              if ( !pSkill->name )
                mudlog( LOG_DEBUG , "Load_skill﹕技能沒有名稱。" );

              if ( !pSkill->cname )
                mudlog( LOG_DEBUG , "Load_skill﹕技能沒有中文名稱。" );

              if ( pSkill->function && pSkill->damage )
                mudlog( LOG_DEBUG, "Load_skill﹕有函數又有攻傷敘述。" );

              if ( !pSkill->function && !pSkill->damage )
                mudlog( LOG_DEBUG , "Load_skill﹕技能沒有操作函數。" );

              if ( !fReload ) top_skill++;

              if ( top_skill >= MAX_SKILL )
                mudlog( LOG_DEBUG, "Load_skill﹕技能數目超過設定。" );

              if ( pSkill->innate && pSkill->adeptation <= 0 )
                mudlog( LOG_DEBUG, "Load_skill﹕有選取卻沒熟練度。" );

              /* 計算機率總和 */
              for ( pDamage = pSkill->damage; pDamage; pDamage = pDamage->next )
                pSkill->chance += pDamage->chance;

              if ( !skill_list ) skill_list       = pSkill;
              if (  skill_last ) skill_last->next = pSkill;

              skill_last   = pSkill;
              pSkill->next = NULL;

              finish = TRUE;
              break;
            }

            KEY( "Exp" , pSkill->exp , fread_number( aFile ) );
            break;

          case 'F':

            if ( !str_scmp( word , "Function", &fMatch ) )
            {
              char      * name;
              SPELL_FUN * function;

              name = fread_string( aFile );

              if ( name[0] )
              {
                if ( !( function = skill_function_name( name ) ) )
                  mudlog( LOG_DEBUG , "Load_skill﹕技能檔的函數 %s 找不到。" , name );

                free_string( name );
                pSkill->function = function;
              }

              break;
            }
            break;

          case 'H':

            KEY( "Help" , pSkill->help , fread_string( aFile ) );
            break;

          case 'I':

            if ( !str_scmp( word , "Innate", &fMatch ) )
            {
              pSkill->innate = fread_number( aFile ) == TRUE ? TRUE : FALSE;
              break;
            }

          case 'L':

            if ( !str_scmp( word , "Limit", &fMatch ) )
            {
              int          iClass;
              LIMIT_DATA * pLimit;
              CLASS_DATA * pClass;

              pClass = class_lookup( iClass =fread_number( aFile ) );

              if ( !pClass )
                mudlog( LOG_DEBUG, "Load_skill﹕技能檔中職業 %d 不存在。", iClass );

              pLimit = alloc_struct( STRUCT_LIMIT_DATA );

              pLimit->class = pClass;
              pLimit->level = fread_number( aFile );
              pLimit->adept = fread_number( aFile );
              pLimit->next  = pSkill->limit;
              pSkill->limit = pLimit;

              fread_to_eol( aFile );
              break;
            }

            break;

          case 'M':

            KEY( "Msgoff", pSkill->msg_off, fread_string( aFile ) );

            if ( !str_scmp( word , "Message", &fMatch ) )
            {
              char   buf[MAX_STRING_LENGTH];
              char * temp;
              char * pSource;
              char * pString;

              /* 一定先讀完一行 */
              fread_to_eol( aFile );
              pSource = temp = fread_string( aFile );

              if ( str_len( temp ) > sizeof( buf ) - 10 )
                mudlog( LOG_DEBUG, "Load_skill﹕訊息太長。" );

              /* 消掉換行字元 */
              for ( pString = buf; *temp; temp++ )
              {
                switch( *temp )
                {
                default:
                  *pString++ = *temp;
                  break;

                case '\n': case '\r':
                  break;
                }
              }

              *pString = '\x0';
              pSkill->message = str_dup( buf );
              free_string( pSource );

              break;
            }

            break;

          case 'N':

            if ( !str_scmp( word , "Name", &fMatch ) )
            {
              SKILL_DATA * aSkill;
              char       * name;

              name = fread_string( aFile );
              for ( aSkill = skill_list; aSkill; aSkill = aSkill->next )
              {
                if ( !str_cmp( aSkill->name , name ) )
                  mudlog( LOG_DEBUG, "Load_skill﹕技能名稱 %s 重覆。" , name );
              }

              pSkill->name = name;
              break;
            }

            break;

          case 'P':

            KEY( "Ply", pSkill->ply, fread_number( aFile ) );

            if ( !str_scmp( word , "Position", &fMatch ) )
            {
              int position;

              switch( position = fread_number( aFile ) )
              {
              default:
                mudlog( LOG_DEBUG ,
                  "Load_skill﹕技能最低狀態 %d 不合理。" , position );
                break;

              case POS_DEAD:
              case POS_SLEEPING:
              case POS_RESTING:
              case POS_FIGHTING:
              case POS_STANDING:
                break;
              }

              pSkill->position = position;
              break;
            }

            break;

          case 'Q':
            KEY( "Qutoient", pSkill->qutoient, fread_number( aFile ) );
            break;

          case 'R':

            if ( !str_scmp( word, "Rating", &fMatch ) )
            {
              switch( pSkill->rating = fread_number( aFile ) )
              {
              default:
                mudlog( LOG_DEBUG ,
                  "Load_skill﹕技能識別種類 %d 不合理。" ,pSkill->rating );
                break;

              case RATING_SKILL:
              case RATING_WIND:
              case RATING_EARTH:
              case RATING_LIGHTNING:
              case RATING_POISON:
              case RATING_FIRE:
              case RATING_WATER:
              case RATING_DARKNESS:
              case RATING_LIGHT:
              case RATING_SAINT:
              case RATING_EVIL:
              case RATING_LOST:
              case RATING_CURE:
              case RATING_SING:
              case RATING_FIGHT:
              case RATING_MURDER:
              case RATING_CREATE:
              case RATING_THIEF:
                break;

              }

              break;
            }

            break;

          case 'S':

            if ( !str_scmp( word , "Sayspell", &fMatch ) )
            {
              pSkill->say_spell = fread_number( aFile ) == 1 ? TRUE : FALSE;
              break;
            }

            if ( !str_scmp( word, "Sex", &fMatch ) )
            {
              switch( pSkill->sex = fread_number( aFile ) )
              {
              default:
                mudlog( LOG_DEBUG, "Load_skill﹕技能性別 %d 不合理。", pSkill->sex );
                break;

              case ERRORCODE:
              case SEX_MALE:
              case SEX_FEMALE:
              case SEX_NEUTRAL:
                break;
              }
              break;
            }

            if ( !str_scmp( word, "Slot", &fMatch ) )
            {
              if ( ( pSkill->slot = fread_number( aFile ) ) >= MAX_SKILL
                || pSkill->slot < 0 )
                mudlog( LOG_DEBUG, "Load_skill﹕技能編號 %d 不合理。"
                  , pSkill->slot );

              if ( get_skill( pSkill->slot ) )
                mudlog( LOG_DEBUG , "Load_skill﹕技能編號 %d 重複。"
                  , pSkill->slot );

              break;
            }
            break;

          case 'T':

            if ( !str_scmp( word , "Teach", &fMatch ) )
            {
              pSkill->teach = fread_number( aFile ) == 1 ? TRUE : FALSE;
              break;
            }

            if ( !str_scmp( word , "Type", &fMatch ) )
            {
              int type;

              switch( type = fread_number( aFile ) )
              {
              default :
                mudlog( LOG_DEBUG , "Load_skill﹕技能形態 %d 不對。" , type );
                break;

              case TAR_IGNORE:
              case TAR_CHAR_OFFENSIVE:
              case TAR_CHAR_DEFENSIVE:
              case TAR_CHAR_SELF:
              case TAR_OBJ_INV:
              case TAR_DODGE:
              case TAR_STRING:
              case TAR_FLEE:
              case TAR_ARGUMENT:
              case TAR_MEDITATION:
              case TAR_OBJ_ROOM:
              case TAR_MOUNT:
              case TAR_NO_CAST:
                break;
              }

              pSkill->type = type;
              break;
            }

            break;

          case 'V':

            if ( !str_scmp( word , "Valid", &fMatch ) )
            {
              pSkill->valid = fread_number( aFile ) == TRUE ? TRUE : FALSE;
              break;
            }

            break;

          case 'W':

            if ( !str_scmp( word , "Weapon", &fMatch ) )
            {
              int weapon;

              switch( ( weapon = fread_number( aFile ) ) )
              {
              default:
                mudlog( LOG_DEBUG ,"Load_skill﹕武器形態 %d 不對。", weapon );
                break;

              case WEAPON_ALL:
              case WEAPON_HAND:
              case WEAPON_DAGGER:
              case WEAPON_SWORD:
              case WEAPON_BLADE:
              case WEAPON_AXE:
              case WEAPON_WHIP:
              case WEAPON_SPEAR:
              case WEAPON_PEN:
              case WEAPON_HAMMER:
              case WEAPON_CLUB:
              case WEAPON_BOW:
              case WEAPON_FORCE:
                pSkill->weapon = weapon;
                break;
              }

              break;
            }

            KEY( "Wait" , pSkill->wait , fread_number( aFile ) );
            break;
          }

          /* 指令型態錯誤 */
          if ( !fMatch )
            mudlog( LOG_DEBUG , "Load_skill﹕命令 %s 不正確。" , word );
        }

        f_close( aFile );
      }

      else
      {
        mudlog( LOG_ERR , "Load_skill﹕無法開啟技能檔 %s。" , buf );
      }
    }
  }

  else
  {
    mudlog( LOG_ERR , "Load_skill﹕沒有技能目錄列表 %s。", buf );
  }

  f_close( pFile );
  mudlog( LOG_INFO , "系統載入 %d 個技能檔。" , top_skill );
  RETURN_NULL();
}


DAMAGE_DATA * load_damage( FILE_DATA * pFile )
{
  DAMAGE_DATA * pDamage;
  char        * word;
  bool          fMatch;

  PUSH_FUNCTION( "load_damage" );

  /* 配置教導所需要的記憶體以及設定預設值 */
  set_damage_default( pDamage = alloc_struct( STRUCT_DAMAGE_DATA ) );

  for ( ;; )
  {
    word   = if_eof( pFile ) ? "End" : fread_word( pFile );
    fMatch = FALSE;

    switch ( UPPER( word[0]) )
    {
    case '*':
      fMatch = TRUE;
      fread_to_eol( pFile );
      break;

    case 'C':

      KEY( "Chance", pDamage->chance, fread_number( pFile ) );
      break;

    case 'D':

      if ( !str_scmp( word , "Description", &fMatch ) )
      {
        char   buf[MAX_STRING_LENGTH];
        char * temp;
        char * pSource;
        char * pString;

        /* 一定先讀完一行 */
        fread_to_eol( pFile );
        pSource = temp = fread_string( pFile );

        if ( str_len( temp ) > sizeof( buf ) - 10 )
          mudlog( LOG_DEBUG, "Loa_damage﹕攻傷描述太長。" );

        /* 消掉換行字元 */
        for ( pString = buf; *temp; temp++ )
        {
          switch( *temp )
          {
          default:
            *pString++ = *temp;
            break;

          case '\n': case '\r':
            break;
          }
        }

        *pString = '\x0';
        pDamage->description = str_dup( buf );
        free_string( pSource );

        break;
      }

      break;

    case 'E':

      if ( !str_scmp( word, "End", &fMatch ) )
      {
        if ( !pDamage->description )
          mudlog( LOG_DEBUG, "Load_damage﹕攻傷沒有敘述。" );

        if ( IS_ERROR( pDamage->chance ) || pDamage->chance <= 0 )
          mudlog( LOG_DEBUG, "Load_damage﹕攻傷沒有機率或不正確。" );

        if ( IS_ERROR( pDamage->value )  || pDamage->value < 0 )
          mudlog( LOG_DEBUG, "Loa_damage﹕攻傷沒有傷害值或不正確。" );

        pDamage->situs &= AttackMask;
        RETURN( pDamage );
      }

      if ( !str_scmp( word, "Effect", &fMatch ) )
      {
        EFFECT_DATA * pEffect;
        int           loop = 0;

        pEffect = alloc_struct( STRUCT_EFFECT_DATA );
        set_effect_default( pEffect );

        switch( ( pEffect->type = fread_number( pFile ) ) )
        {
        default:
          mudlog( LOG_DEBUG, "Load_damage﹕效應型態錯誤。" );
          break;

        case EFFECT_VICTIM_MANA:
        case EFFECT_SELF_MANA:
        case EFFECT_VICTIM_MOVE:
        case EFFECT_SELF_MOVE:
        case EFFECT_VICTIM_BLINDNESS:
        case EFFECT_VICTIM_CURSE:
        case EFFECT_VICTIM_POISON:
        case EFFECT_VICTIM_SLEEP:
        case EFFECT_VICTIM_PARA:
        case EFFECT_VICTIM_FAERIE_FIRE:
        case EFFECT_VICTIM_HIT:
        case EFFECT_SELF_HIT:
        case EFFECT_HIT_DIE:
          break;
        }

        while( loop < MAX_EFFECT_VALUE )
        {
          if ( fread_if_eol( pFile ) ) break;
          pEffect->value[loop++] = fread_number( pFile );
        }

        if ( pEffect->value[2] > 100 || pEffect->value[2] < 0 )
        {
          mudlog( LOG_DEBUG, "Load_damage﹕影響機率 %d 過大或過小。", pEffect->value );
          RETURN( NULL );
        }

        fread_to_eol( pFile );

        pEffect->next   = pDamage->effect;
        pDamage->effect = pEffect;
        break;
      }
      break;

    case 'I':

      if ( !str_scmp( word, "Innate", &fMatch ) )
      {
        pDamage->obj_vnum = fread_number( pFile );
        pDamage->multiple = fread_number( pFile );
        break;
      }

      break;

    case 'P':
      KEY( "Parry", pDamage->parry, fread_number( pFile ) );
      break;

    case 'R':

      if ( !str_scmp( word , "Routine", &fMatch ) )
      {
        char        * name;
        ROUTINE_FUN * routine;

        name = fread_string( pFile );

        if ( name[0] )
        {
          if ( !( routine = check_routine_name( name ) ) )
            mudlog( LOG_DEBUG , "Load_damage﹕技能檔的函數 %s 找不到。" , name );

          free_string( name );
          pDamage->routine = routine;
        }

        break;
      }
      break;

    case 'S':
      KEY( "Situs", pDamage->situs, fread_number( pFile ) );
      break;

    case 'V':
      KEY( "Value"    , pDamage->value    , fread_number( pFile ) );
      KEY( "ViceValue", pDamage->vicevalue, fread_number( pFile ) );
      break;
    }

    /* 指令型態錯誤 */
    if ( !fMatch ) mudlog( LOG_DEBUG , "Load_damage﹕命令 %s 不正確。" , word );
  }

  RETURN( NULL );
}

void load_instrument( const char * path , const char * index )
{
  CMD_DATA  * pCommand;
  FILE_DATA * pFile;
  FILE_DATA * aFile;
  char      * word;
  char        indexfile[ MAX_FILE_LENGTH ];
  char        buf[MAX_FILE_LENGTH];
  char        directory[ MAX_FILE_LENGTH ];
  bool        fMatch;
  bool        finish;

  PUSH_FUNCTION( "load_instrument" );

  /* 處理目錄 */
  str_cpy( directory , path );
  fill_path( directory );

  /* 處理索引檔案名稱 */
  sprintf( indexfile , "%s%s" , directory , index );

  /* 開啟索引檔案, 不能沒有這個檔案 */
  if ( ( pFile = f_open( indexfile , "r" ) ) )
  {
    /* 一直讀取到檔案末端 */
    while ( !if_eof( pFile ) )
    {
      /* 讀取命令檔案的名稱 */
      word = fread_word( pFile );
      fread_to_eol( pFile );

      if ( word[0] == '*' ) continue;

      /* 處理欲開啟命令檔案的名稱 */
      sprintf( buf , "%s%c" , directory , LOWER( word[0] ) );

      fill_path( buf            );
      str_cat( buf, word        );
      str_cat( buf, "."         );
      str_cat( buf, command_ext );

      if ( ( aFile = f_open( buf , "r" ) ) )
      {
        /* 配置記憶體 */
        pCommand = alloc_struct( STRUCT_CMD_DATA );

        /* 重置預設值 */
        set_command_default( pCommand );

        /* 載入指令的主程式部份 */
        for ( finish = FALSE; finish != TRUE; )
        {
          word   = if_eof( aFile ) ? "End" : fread_word( aFile );
          fMatch = FALSE;

          switch ( UPPER( word[0] ) )
          {
          /* 註解 */
          case '*':
            fMatch = TRUE;
            fread_to_eol( aFile );
            break;

          case 'A':

            if ( !str_scmp( word , "Argument", &fMatch ) )
            {
              pCommand->argument = fread_number( aFile ) == 1 ? TRUE : FALSE;
              break;
            }

            break;

          case 'C':

            if ( !str_scmp( word , "Canlock", &fMatch ) )
            {
              pCommand->canlock = fread_number( aFile ) == TRUE ? TRUE : FALSE;
              break;
            }

            if ( !str_scmp( word , "Chat", &fMatch ) )
            {
              pCommand->chat = fread_number( aFile ) == TRUE ? TRUE : FALSE;
              break;
            }

            KEY( "Cname" , pCommand->cname , fread_string( aFile ) );
            break;

          case 'D':

            if ( !str_scmp( word , "Dead", &fMatch ) )
            {
              pCommand->dead = fread_number( aFile ) == TRUE ? TRUE : FALSE;
              break;
            }

            break;

          case 'E':

            if ( !str_scmp( word, "End", &fMatch ) )
            {
              if ( !pCommand->name )
                mudlog( LOG_DEBUG , "Load_instrument﹕指令檔沒有名稱。" );

              if ( !pCommand->cname )
                mudlog( LOG_DEBUG , "Load_instrument﹕指令檔沒有中文名稱。" );

              if ( !pCommand->function )
                mudlog( LOG_DEBUG , "Load_instrument﹕指令檔沒有執行函數。" );

              if ( pCommand->lock ) pCommand->canlock = FALSE;
              pCommand->filename = str_dup( buf );

              if ( !cmd_list ) cmd_list       = pCommand;
              if (  cmd_last ) cmd_last->next = pCommand;

              cmd_last       = pCommand;
              pCommand->next = NULL;
              if ( !fReload ) top_cmd++;
              finish = TRUE;
              break;
            }

            if ( !str_scmp( word , "Exact", &fMatch ) )
            {
              pCommand->exact = fread_number( aFile ) == 1 ? TRUE : FALSE;
              break;
            }

            break;

          case 'F':

            if ( !str_scmp( word , "Function", &fMatch ) )
            {
              char   * name;
              DO_FUN * function;

              name = fread_string( aFile );

              if ( !( function = command_function_name( name ) ) )
                mudlog( LOG_DEBUG , "Load_instrument﹕指令檔的函數 %s 找不到。"
                  , name );

              free_string( name );
              pCommand->function = function;
              break;
            }
            break;

          case 'H':

            KEY( "Help" , pCommand->help , fread_string( aFile ) );
            break;

          case 'J':

            if ( !str_scmp( word , "Jail", &fMatch ) )
            {
              pCommand->jail = fread_number( aFile ) == TRUE ? TRUE : FALSE;
              break;
            }

            break;

          case 'L':

            if ( !str_scmp( word , "Limit", &fMatch ) )
            {
              pCommand->limit = fread_number( aFile ) == TRUE ? TRUE : FALSE;
              break;
            }

            if ( !str_scmp( word , "Lock", &fMatch ) )
            {
              pCommand->lock = fread_number( aFile ) == TRUE ? TRUE : FALSE;
              break;
            }

            KEY( "Level" , pCommand->level , fread_number( aFile ) );

            if ( !str_scmp( word , "Lost", &fMatch ) )
            {
              pCommand->lost = fread_number( aFile ) == TRUE ? TRUE : FALSE;
              break;
            }

            if ( !str_scmp( word , "Log", &fMatch ) )
            {
              int log;

              switch( log = fread_number( aFile ) )
              {
              default:
                mudlog( LOG_DEBUG , "Load_instrument﹕指令檔的標記變數 %d 不合理。" , log );
                break;

              case LOG_NORMAL:
              case LOG_ALWAYS:
              case LOG_NEVER:
              case LOG_WIZ:
                break;
              }

              pCommand->log = log;
              break;
            }

            break;

          case 'M':

            if ( !str_scmp( word , "Mobonly" , &fMatch ) )
            {
              pCommand->mobonly = fread_number( aFile ) == 1 ? TRUE : FALSE;
              break;
            }

            break;

          case 'N':

            if ( !str_scmp( word , "Name", &fMatch ) )
            {
              CMD_DATA * aCommand;

              pCommand->name = fread_string( aFile );

              for ( aCommand = cmd_list; aCommand; aCommand = aCommand->next )
              {
                if ( !str_cmp( aCommand->name , pCommand->name ) )
                  mudlog( LOG_DEBUG , "Load_instrument﹕指令檔指令 %s 重覆。"
                    , pCommand->name );
              }

              break;
            }

          case 'O':

            if ( !str_scmp( word , "Order", &fMatch ) )
            {
              pCommand->order = fread_number( aFile ) == TRUE ? TRUE : FALSE;
              break;
            }

            break;

          case 'P':

            if ( !str_scmp( word , "Position", &fMatch ) )
            {
              int pos;

              switch( pos = fread_number( aFile ) )
              {
              default:
                mudlog( LOG_DEBUG ,"Load_instrument﹕技能最低狀態 %d 不合理。"
                  , pos );
                break;

              case POS_DEAD:
              case POS_SLEEPING:
              case POS_RESTING:
              case POS_FIGHTING:
              case POS_STANDING:
                break;
              }

              pCommand->position = pos;
              break;
            }

            break;

          case 'W':

            if ( !str_scmp( word , "Wizlog", &fMatch ) )
            {
              pCommand->wizlog = fread_number( aFile ) == 1 ? TRUE : FALSE;
              break;
            }

            break;
          }

          /* 指令型態錯誤 */
          if ( !fMatch )
            mudlog( LOG_DEBUG , "Load_instrument﹕命令 %s 不正確。" , word );
        }

        f_close( aFile );
      }

      else
      {
        mudlog( LOG_ERR , "Load_instrument﹕無法開啟命令檔 %s。" , buf );
      }
    }
  }

  else
  {
    mudlog( LOG_ERR , "Load_instrument﹕沒有命令目錄列表 %s。", path );
  }

  f_close( pFile );
  mudlog( LOG_INFO , "系統載入 %d 個指令檔。" , top_cmd );
  RETURN_NULL();
}

void load_internal( const char * filename )
{
  NET_DATA  * pNet;
  FILE_DATA * pFile;
  char      * word;
  bool        fMatch;
  bool        Finish = FALSE;

  PUSH_FUNCTION( "load_internal" );

  if ( ( pFile = f_open( filename , "r" ) ) )
  {
    while ( !if_eof( pFile ) )
    {
      word   = if_eof( pFile ) ? "End" : fread_word( pFile );
      fMatch = FALSE;

      switch ( UPPER( word[0] ) )
      {
      /* 註解 */
      case '*':
        fMatch = TRUE;
        fread_to_eol( pFile );
        break;

      case '#':

        if ( !str_scmp( word, "#Station", &fMatch ) )
        {
          /* 載入檔案 */
          pNet = load_station( pFile );

          pNet->next = net_list;
          net_list   = pNet;

          /* 如果可以載入 */
          if ( pNet->valid ) init_client( pNet );
          else pNet->import = pNet->export = -2;

          break;
        }

        break;

      case 'E':

        if ( !str_scmp( word, "End", &fMatch ) )
        {
          Finish = TRUE;
          break;
        }
        break;

      case 'P':

        if ( !str_scmp( word , "Port", &fMatch ) )
        {
          if ( ( internal_port = fread_number( pFile ) ) <= 1024
            || internal_port > 65000 )
          mudlog( LOG_ERR, "Load_internal﹕你的通訊埠 %d 不合法。\n" , internal_port );

          break;
        }
        break;
      }

      /* 指令型態錯誤 */
      if ( !fMatch )
        mudlog( LOG_DEBUG , "Load_internal﹕命令 %s 不正確。" , word );

      if ( Finish ) break;
    }

    f_close( pFile );
  }

  else
  {
    mudlog( LOG_INFO, "沒有載入任何網際網路的資料。" );
  }

  RETURN_NULL();
}

NET_DATA * load_station( FILE_DATA * pFile )
{
  NET_DATA * pNet;
  char     * word;
  bool       fMatch;

  PUSH_FUNCTION( "load_station" );

  set_station_default( pNet = alloc_struct( STRUCT_NET_DATA ) );

  for ( ;; )
  {
    word   = if_eof( pFile ) ? "End" : fread_word( pFile );
    fMatch = FALSE;

    switch ( UPPER( word[0] ) )
    {
    case '*':

      fMatch = TRUE;
      fread_to_eol( pFile );
      break;

    case 'A':

      if ( !str_scmp( word, "Address", &fMatch ) )
      {
        pNet->address = str_dup( fread_word( pFile ) );

        /* 避免重複的位址 */
        if ( get_net_data( pNet->address, NET_ADDRESS ) )
          mudlog( LOG_ERR, "Load_station﹕站址 %s 重複。", pNet->address );

        break;
      }

      break;

    case 'C':

      if ( !str_scmp( word, "Cname", &fMatch ) )
      {
        pNet->cname = str_dup( fread_word( pFile ) );
        break;
      }
      break;

    case 'E':

      if ( !str_scmp( word, "End", &fMatch ) )
      {
        if ( !pNet->name    ) mudlog( LOG_ERR, "Load_station﹕缺乏站名。"   );
        if ( !pNet->address ) mudlog( LOG_ERR, "Load_station﹕缺乏位址。"   );
        if ( pNet->port < 0 ) mudlog( LOG_ERR, "Load_station﹕缺乏通訊埠。" );

        RETURN( pNet );
      }

      break;

    case 'L':

      if ( !str_scmp( word, "Log", &fMatch ) )
      {
        pNet->log = ( fread_number( pFile ) == FALSE ) ? FALSE : TRUE;
        break;
      }

      break;

    case 'N':

      if ( !str_scmp( word, "Name", &fMatch ) )
      {
        pNet->name = str_dup( fread_word( pFile ) );

        /* 避免重複的站名 */
        if ( get_net_data( pNet->name, NET_NAME ) )
          mudlog( LOG_ERR, "Load_station﹕站名 %s 重複。", pNet->name );

        break;
      }

      break;

    case 'P':

      KEY( "Port", pNet->port, fread_number( pFile ) );
      break;

    case 'V':

      if ( !str_scmp( word, "Valid", &fMatch ) )
      {
        pNet->valid = ( fread_number( pFile ) == FALSE ) ? FALSE : TRUE;
        break;
      }

      break;
    }

    /* 指令型態錯誤 */
    if ( !fMatch )
      mudlog( LOG_DEBUG , "Load_station﹕命令 %s 不正確。" , word );
  }

  RETURN( NULL );
}

void load_bus( const char * filename )
{
  ROOM_INDEX_DATA * platform;
  ROOM_INDEX_DATA * loge;
  BUS_DATA        * pBus;
  FILE_DATA       * pFile;
  char            * word;
  bool              fMatch;
  bool              Finish = FALSE;
  int               count = 0;
  int               location;

  PUSH_FUNCTION( "load_bus" );

  if ( ( pFile = f_open( filename , "r" ) ) )
  {
    while ( !if_eof( pFile ) )
    {
      word   = if_eof( pFile ) ? "End" : fread_word( pFile );
      fMatch = FALSE;

      switch ( UPPER( word[0] ) )
      {
      /* 註解 */
      case '*':
        fMatch = TRUE;
        fread_to_eol( pFile );
        break;

      case 'B':

        if ( !str_scmp( word, "Bus", &fMatch ) )
        {
          /* 配置記憶體以及設定預設值 */
          set_bus_default( ( pBus = alloc_struct( STRUCT_BUS_DATA ) ) );

          /* 讀入站名 */
          pBus->name = str_dup( fread_word( pFile ) );

          /* 讀入上下站處, 此處必需先存在, 且不能有向下出口 */
          location = fread_number( pFile );

          if ( !( pBus->station = get_room_index( location ) ) )
            mudlog( LOG_DEBUG, "Load_bus﹕找不到地鐵站 %d。" , location );

          /* 檢查是否有向下出口, 有則不行 */
          if ( !check_station( pBus->station ) )
            mudlog( LOG_DEBUG, "Load_bus﹕地鐵站 %d 不合法。", location );

          /* 讀取月台, 不能存在, 系統建立 */
          platform = get_room_index( location = fread_number( pFile ) );

          if ( platform )
            mudlog( LOG_DEBUG, "Load_bus﹕地鐵站月台 %d 存在。", location );

          if ( !( pBus->platform = create_platform( location
            , pBus->name, pBus->station->area ) ) )
          {
            mudlog( LOG_DEBUG, "Load_bus﹕建立地鐵站 %d 失敗。", location );
          }

          /* 上下連結車站 */
          if ( !link_path( pBus->station, pBus->platform ) )
            mudlog( LOG_DEBUG, "Load_bus﹕連結地鐵站失敗。" );

          /* 讀取車廂, 不能存在, 系統建立 */
          loge    = get_room_index( location = fread_number( pFile ) );

          if ( loge )
            mudlog( LOG_DEBUG, "Load_bus﹕地鐵站車廂 %d 存在。", location );

          if ( !( pBus->loge = create_loge( location
            , pBus->name, pBus->station->area ) ) )
          {
            mudlog( LOG_DEBUG, "Load_bus﹕建立地鐵站 %d 失敗。", location );
          }

          /* 讀取費用 */
          if ( ( pBus->cost = fread_number( pFile ) ) <= 0 )
            mudlog( LOG_DEBUG, "Load_bus﹕地鐵收費 %d 不合理。", pBus->cost );

          fread_to_eol( pFile );

          /* 建立串列 */
          if ( !bus_first ) bus_first      = pBus;
          if (  bus_last  ) bus_last->next = pBus;

          bus_last   = pBus;
          pBus->next = NULL;

          count++;
          break;
        }
        break;

      case 'C':

        KEY( "Company" , company_name, fread_string( pFile ) );
        break;

      case 'E':

        /* 結束 */
        if ( !str_scmp( word, "End", &fMatch ) )
        {
          Finish = TRUE;
          break;
        }
        break;

      case 'L':

        if ( !str_scmp( word , "Loge", &fMatch ) )
        {
          fread_to_eol( pFile ); /* 一定先讀完一行 */
          loge_descr = fread_string( pFile );
          break;
        }

        KEY( "LogeShort", loge_short, fread_string( pFile ) );
        break;

      case 'P':

        if ( !str_scmp( word , "Platform", &fMatch ) )
        {
          fread_to_eol( pFile ); /* 一定先讀完一行 */
          platform_descr = fread_string( pFile );
          break;
        }

        KEY( "PlatformShort", platform_short, fread_string( pFile ) );
        break;
      }

      /* 指令型態錯誤 */
      if ( !fMatch )
        mudlog( LOG_DEBUG , "Load_bus﹕命令 %s 不正確。" , word );

      if ( Finish ) break;
    }

    mudlog( LOG_INFO, "系統載入 %d 個地鐵資料。" , count );
    f_close( pFile );
  }

  else
  {
    mudlog( LOG_INFO, "沒有載入任何地鐵的資料。" );
  }

  RETURN_NULL();
}

/* 載入禮物資料 */
void load_bounty( const char * filename )
{
  BOUNTY_DATA * pBounty;
  FILE_DATA   * pFile;
  char        * word;
  bool          fMatch;
  bool          Finish = FALSE;

  PUSH_FUNCTION( "load_bounty" );

  if ( ( pFile = f_open( filename , "r" ) ) )
  {
    while ( !if_eof( pFile ) )
    {
      word   = if_eof( pFile ) ? "End" : fread_word( pFile );
      fMatch = FALSE;

      switch ( UPPER( word[0] ) )
      {
      /* 註解 */
      case '*':
        fMatch = TRUE;
        fread_to_eol( pFile );
        break;

      case '#':

        if ( !str_scmp( word, "#Bounty", &fMatch ) )
        {
          pBounty = load_a_bounty( pFile );

          pBounty->next = bounty_list;
          bounty_list   = pBounty;

          if ( !fReload ) top_bounty++;
          break;
        }

        break;

      case 'E':

        /* 結束 */
        if ( !str_scmp( word, "End", &fMatch ) )
        {
          Finish = TRUE;
          break;
        }

        break;

      case 'L':

        KEY( "Limit", BountyLimit, fread_number( pFile ) );
        break;
      }

      /* 指令型態錯誤 */
      if ( !fMatch )
        mudlog( LOG_DEBUG , "Load_bounty﹕命令 %s 不正確。" , word );

      if ( Finish ) break;
    }

    mudlog( LOG_INFO, "系統載入 %d 個懸賞資料。" , top_bounty );
    f_close( pFile );
  }

  else
  {
    mudlog( LOG_INFO, "沒有載入任何懸賞的資料。" );
  }

  RETURN_NULL();
}

/* 讀取一個禮物資料 */
BOUNTY_DATA * load_a_bounty( FILE_DATA * pFile )
{
  MOB_INDEX_DATA * pMob;
  BOUNTY_DATA    * pBounty;
  char           * word;
  bool             fMatch;
  int              slot;

  PUSH_FUNCTION( "load_a_bounty" );

  set_bounty_default( pBounty = alloc_struct( STRUCT_BOUNTY_DATA ) );

  for ( ;; )
  {
    word   = if_eof( pFile ) ? "End" : fread_word( pFile );
    fMatch = FALSE;

    switch ( UPPER( word[0] ) )
    {
    case '*':

      fMatch = TRUE;
      fread_to_eol( pFile );
      break;

    case 'C':

      if ( !str_scmp( word, "Chance", &fMatch ) )
      {
        if ( ( slot = fread_number( pFile ) ) <= 0 || slot > 1000 )
          mudlog( LOG_DEBUG, "Load_a_bounty﹕出現機率 %d 不合理。", slot );

        pBounty->chance = slot;
        break;
      }

      break;

    case 'E':

      if ( !str_scmp( word, "End", &fMatch ) )
      {
        if ( !pBounty->mob )
          mudlog( LOG_DEBUG, "Load_a_bounty﹕缺乏怪物的序號。" );

        if ( !pBounty->msg || !*pBounty->msg )
          mudlog( LOG_DEBUG, "Load_a_bounty﹕缺乏輸出敘述。" );

        if ( IS_ERROR( pBounty->chance ) )
          mudlog( LOG_DEBUG, "Load_a_bounty﹕缺乏出現機率。" );

        if ( IS_ERROR( pBounty->migration ) )
          mudlog( LOG_DEBUG, "Load_a_bounty﹕缺乏遷徙機率。" );

        if ( IS_ERROR( pBounty->type ) )
          mudlog( LOG_DEBUG, "Load_a_bounty﹕缺乏獎賞型態。" );

        if ( IS_ERROR( pBounty->max ) )
          mudlog( LOG_DEBUG, "Load_a_bounty﹕缺乏最大出現量。" );

        RETURN( pBounty );
      }

      break;

    case 'L':

      if ( !str_scmp( word , "Lock", &fMatch ) )
      {
        pBounty->lock = fread_number( pFile ) == TRUE ? TRUE : FALSE;
        break;
      }
      break;

    case 'M':

      if ( !str_scmp( word, "Migration", &fMatch ) )
      {
        if ( ( slot = fread_number( pFile ) ) < 0 || slot > 100 )
          mudlog( LOG_DEBUG, "Load_a_bounty﹕遷徙機率 %d 不合理。", slot );

        pBounty->migration = slot;
        break;
      }

      if ( !str_scmp( word, "Mobile", &fMatch ) )
      {
        if ( !( pMob = get_mob_index( slot = fread_number( pFile ) ) ) )
          mudlog( LOG_DEBUG, "Load_a_bounty﹕懸賞者 %d 不存在。", slot );

        pBounty->mob = pMob;
        break;
      }

      if ( !str_scmp( word, "Message", &fMatch ) )
      {
        fread_to_eol( pFile ); /* 一定先讀完一行 */
        pBounty->msg = fread_string( pFile );
        break;
      }

      if ( !str_scmp( word, "MaxNumber", &fMatch ) )
      {
        if ( ( slot = fread_number( pFile ) ) <= 0 )
          mudlog( LOG_DEBUG, "Load_a_bounty﹕最大數目 %d 不合理。", slot );

        pBounty->max = slot;
        break;
      }

      break;

    case 'R':

      if ( !str_scmp( word, "Room", &fMatch ) )
      {
        if ( ( slot = fread_number( pFile ) ) > 0 )
        {
          if ( !get_room_index( slot ) )
            mudlog( LOG_DEBUG, "Load_a_bounty﹕房間 %d 不存在。", slot );

          pBounty->room = slot;
        }

        break;
      }
      break;

    case 'T':

      if ( !str_scmp( word, "Type", &fMatch ) )
      {
        switch( ( slot = fread_number( pFile ) ) )
        {
        default:
          mudlog( LOG_DEBUG, "Load_a_bounty﹕獎賞型態 %d 不合理。", slot );
          break;

        case BOUNTY_GOLD:
        case BOUNTY_FIRMAN:
          break;
        }

        pBounty->type = slot;
        break;
      }
      break;

    case 'V':
      KEY( "Value", pBounty->value, fread_number( pFile ) );
      break;
    }

    /* 指令型態錯誤 */
    if ( !fMatch )
      mudlog( LOG_DEBUG , "Load_a_bounty﹕命令 %s 不正確。" , word );
  }

  RETURN( NULL );
}

/* 載入禮物資料 */
void load_gift( const char * filename )
{
  GIFT_DATA * pGift;
  GIFT_DATA * aGift;
  FILE_DATA * pFile;
  char      * word;
  bool        fMatch;
  bool        Finish = FALSE;

  PUSH_FUNCTION( "load_gift" );

  if ( ( pFile = f_open( filename , "r" ) ) )
  {
    while ( !if_eof( pFile ) )
    {
      word   = if_eof( pFile ) ? "End" : fread_word( pFile );
      fMatch = FALSE;

      switch ( UPPER( word[0] ) )
      {
      /* 註解 */
      case '*':
        fMatch = TRUE;
        fread_to_eol( pFile );
        break;

      case '#':

        if ( !str_scmp( word, "#Gift", &fMatch ) )
        {
          pGift = load_a_gift( pFile );

          set_gift_time( pGift );

          pGift->next = NULL;
          for ( aGift = gift_list; aGift && aGift->next; aGift = aGift->next );

          if ( !aGift ) gift_list   = pGift;
          else          aGift->next = pGift;

          if ( !fReload ) top_gift++;
          break;
        }

        break;

      case 'E':

        /* 結束 */
        if ( !str_scmp( word, "End" , &fMatch ) )
        {
          Finish = TRUE;
          break;
        }

        break;
      }

      /* 指令型態錯誤 */
      if ( !fMatch )
        mudlog( LOG_DEBUG , "Load_gift﹕命令 %s 不正確。" , word );

      if ( Finish ) break;
    }

    mudlog( LOG_INFO, "系統載入 %d 個禮物資料。" , top_gift );
    f_close( pFile );
  }

  else
  {
    mudlog( LOG_INFO, "沒有載入任何禮物的資料。" );
  }

  RETURN_NULL();
}

/* 讀取一個禮物資料 */
GIFT_DATA * load_a_gift( FILE_DATA * pFile )
{
  GIFT_DATA * pGift;
  GIFT_DATA * aGift;
  char      * word;
  int         level;
  bool        fMatch;

  PUSH_FUNCTION( "load_a_gift" );

  set_gift_default( pGift = alloc_struct( STRUCT_GIFT_DATA ) );

  for ( ;; )
  {
    word   = if_eof( pFile ) ? "End" : fread_word( pFile );
    fMatch = FALSE;

    switch ( UPPER( word[0] ) )
    {
    case '*':

      fMatch = TRUE;
      fread_to_eol( pFile );
      break;

    case 'D':

      if ( !str_scmp( word, "Duration", &fMatch ) )
      {
        if ( ( level = fread_number( pFile ) ) <= 0 )
          mudlog( LOG_DEBUG, "Load_a_gift﹕間隔時間 %d 不合理。", level );

        pGift->duration = level;
        break;
      }

      if ( !str_scmp( word, "Days", &fMatch ) )
      {
        if ( ( level = fread_number( pFile ) ) <= 0 )
          mudlog( LOG_DEBUG, "Load_a_gift﹕舉辦天數 %d 不合理。", level );

        pGift->days = level;
        break;
      }

      if ( !str_scmp( word, "Date", &fMatch ) )
      {
        int day;
        int month;

        month = atoi( fread_word( pFile ) );
        day   = atoi( fread_word( pFile ) );

        if ( month <= 0 || day <= 0 )
          mudlog( LOG_DEBUG, "Load_a_gift﹕禮物寄送日期不合理。" );

        switch( month )
        {
        default:
          mudlog( LOG_DEBUG, "Load_a_gift﹕禮物寄送日期不合理。" );
          break;

        case  1: case  3: case 5: case 7: case 8:
        case 10: case 12:

          if ( day <= 0 || day > 31 )
            mudlog( LOG_DEBUG, "Load_a_gift﹕禮物寄送日期不合理。" );

          break;

        case 2:

          if ( day <= 0 || day > 29 )
            mudlog( LOG_DEBUG, "Load_a_gift﹕禮物寄送日期不合理。" );

          break;

        case 4: case 6: case 9: case 11:

          if ( day <= 0 || day > 30 )
            mudlog( LOG_DEBUG, "Load_a_gift﹕禮物寄送日期不合理。" );

          break;
        }

        pGift->day   = day;
        pGift->month = month;
        break;
      }

      break;

    case 'E':

      if ( !str_scmp( word, "End", &fMatch ) )
      {
        if ( IS_ERROR( pGift->high ) )
          mudlog( LOG_DEBUG, "Load_a_gift﹕缺乏最高等級。" );

        if ( IS_ERROR( pGift->low ) )
          mudlog( LOG_DEBUG, "Load_a_gift﹕缺乏最低等級。" );

        if ( IS_ERROR( pGift->stamp ) )
          mudlog( LOG_DEBUG, "Load_a_gift﹕缺乏禮物序號。" );

        if ( IS_ERROR( pGift->sender ) )
          mudlog( LOG_DEBUG, "Load_a_gift﹕缺乏禮物派送者。" );

        if ( IS_ERROR( pGift->duration ) )
          mudlog( LOG_DEBUG, "Load_a_gift﹕缺乏禮物間隔時間。" );

        if ( IS_ERROR( pGift->days ) )
          mudlog( LOG_DEBUG, "Load_a_gift﹕缺乏禮物舉辦天數。" );

        if ( IS_ERROR( pGift->day ) || IS_ERROR( pGift->month ) )
          mudlog( LOG_DEBUG, "Load_a_gift﹕缺乏禮物派送日期。" );

        if ( !pGift->gift )
          mudlog( LOG_DEBUG, "Load_a_gift﹕缺乏禮物。" );

        if ( !pGift->title || !*pGift->title )
          mudlog( LOG_DEBUG, "Load_a_gift﹕缺乏禮物標題。" );

        if ( !pGift->message || !*pGift->message )
          mudlog( LOG_DEBUG, "Load_a_gift﹕缺乏禮物訊息。" );

        if ( pGift->high < pGift->low ) SWAP( pGift->high, pGift->low );

        RETURN( pGift );
      }

      break;

    case 'G':

      if ( !str_scmp( word, "Gold", &fMatch ) )
      {
        if ( ( level = fread_number( pFile ) ) < 0 )
          mudlog( LOG_DEBUG, "Load_a_gift﹕禮金 %d 不合理。", level );

        pGift->gold = level;
        break;
      }

      if ( !str_scmp( word, "Gift", &fMatch ) )
      {
        OBJ_INDEX_DATA * pIndex;

        level = fread_number( pFile );

        if ( !( pIndex = get_obj_index( level ) ) )
          mudlog( LOG_DEBUG, "Load_a_gift﹕禮物號碼 %d 不存在。", level );

        pGift->gift = pIndex;
        break;
      }

      break;

    case 'H':

      if ( !str_scmp( word, "High", &fMatch ) )
      {
        if ( ( level = fread_number( pFile ) ) <= 0 || level > MAX_LEVEL )
          mudlog( LOG_DEBUG, "Load_a_gift﹕最高等級 %d 不合理。", level );

        pGift->high = level;
        break;
      }
      break;

    case 'L':

      if ( !str_scmp( word, "Low", &fMatch ) )
      {
        if ( ( level = fread_number( pFile ) ) <= 0 || level > MAX_LEVEL )
          mudlog( LOG_DEBUG, "Load_a_gift﹕最低等級 %d 不合理。", level );

        pGift->low = level;
        break;
      }

      break;

    case 'M':

      if ( !str_scmp( word, "Message", &fMatch ) )
      {
        fread_to_eol( pFile ); /* 一定先讀完一行 */
        pGift->message = fread_string( pFile );
        break;
      }

      break;

    case 'S':

      if ( !str_scmp( word, "Sender", &fMatch ) )
      {
        if ( !get_mob_index( level = fread_number( pFile ) ) )
          mudlog( LOG_DEBUG, "Load_a_gift﹕禮物派送者 %d 不存在。", level );

        pGift->sender = level;
        break;
      }

      if ( !str_scmp( word, "Stamp", &fMatch ) )
      {
        level = fread_number( pFile );

        for ( aGift = gift_list; aGift; aGift = aGift->next )
        {
          if ( aGift->stamp == level )
            mudlog( LOG_DEBUG, "Load_a_gift﹕序號 %d 重複。", level );
        }

        pGift->stamp = level;
        break;
      }
      break;

    case 'T':
      KEY( "Title", pGift->title, fread_string( pFile ) );
      break;
    }

    /* 指令型態錯誤 */
    if ( !fMatch )
      mudlog( LOG_DEBUG , "Load_a_gift﹕命令 %s 不正確。" , word );
  }

  RETURN( NULL );
}

/* 載入問題資料 */
void load_question( const char * filename )
{
  QUESTION_DATA * pQuestion;
  FILE_DATA     * pFile;
  char          * word;
  bool            fMatch;
  bool            Finish = FALSE;

  PUSH_FUNCTION( "load_question" );

  if ( ( pFile = f_open( filename , "r" ) ) )
  {
    while ( !if_eof( pFile ) )
    {
      word   = if_eof( pFile ) ? "End" : fread_word( pFile );
      fMatch = FALSE;

      switch ( UPPER( word[0] ) )
      {
      /* 註解 */
      case '*':
        fMatch = TRUE;
        fread_to_eol( pFile );
        break;

      case '#':

        if ( !str_scmp( word, "#Question", &fMatch ) )
        {
          pQuestion = load_a_question( pFile );

          pQuestion->next = question_list;
          question_list   = pQuestion;

          if ( !fReload ) top_question++;
          break;
        }

        break;

      case 'E':

        /* 結束 */
        if ( !str_scmp( word, "End", &fMatch ) )
        {
          Finish = TRUE;
          break;
        }

        break;
      }

      /* 指令型態錯誤 */
      if ( !fMatch )
        mudlog( LOG_DEBUG , "Load_question﹕命令 %s 不正確。" , word );

      if ( Finish ) break;
    }

    mudlog( LOG_INFO, "系統載入 %d 個問題資料。" , top_question );
    f_close( pFile );
  }

  else
  {
    mudlog( LOG_INFO, "沒有載入任何問題的資料。" );
  }

  RETURN_NULL();
}

/* 讀取一個解謎資料 */
QUESTION_DATA * load_a_question( FILE_DATA * pFile )
{
  QUESTION_DATA * pQuestion;
  char          * word;
  bool            fMatch;
  int             loop;
  int             count;
  int             ans;

  PUSH_FUNCTION( "load_a_question" );

  set_question_default( pQuestion = alloc_struct( STRUCT_QUESTION_DATA ) );

  for ( ;; )
  {
    word   = if_eof( pFile ) ? "End" : fread_word( pFile );
    fMatch = FALSE;

    switch ( UPPER( word[0] ) )
    {
    case '*':

      fMatch = TRUE;
      fread_to_eol( pFile );
      break;

    case 'E':

      if ( !str_scmp( word, "End", &fMatch ) )
      {
        if ( !pQuestion->title || !*pQuestion->title )
          mudlog( LOG_DEBUG, "Load_a_question﹕沒有問題題目。" );

        for ( ans = loop = count = 0; loop < MAX_QUESTION; loop++ )
        {
          if ( pQuestion->question[loop] && *pQuestion->question[loop] )
          {
            if ( pQuestion->answer[loop] == TRUE ) ans++;
            count++;
          }
        }

        if ( count <= 1 )
          mudlog( LOG_DEBUG, "Load_a_question﹕問題太少 %d。", count );

        if ( ans <= 0 )
          mudlog( LOG_DEBUG, "Load_a_question﹕至少要有一個以上的答案。" );

        RETURN( pQuestion );
      }

      break;

    case 'Q':

      if ( !str_scmp( word, "Question", &fMatch ) )
      {
        for ( loop = 0; loop < MAX_QUESTION; loop++ )
          if ( !pQuestion->question[loop] ) break;

        if ( loop >= MAX_QUESTION )
          mudlog( LOG_DEBUG, "Load_a_question﹕太多題目﹐無法儲存。" );

        pQuestion->question[loop] = fread_string( pFile );
        pQuestion->answer[loop]   = fread_number( pFile );

        if ( !pQuestion->question[loop] || !*pQuestion->question[loop] )
          mudlog( LOG_DEBUG, "Load_a_question﹕題目太簡短。" );

        break;
      }

      break;

    case 'T':

      if ( !str_scmp( word, "Title", &fMatch ) )
      {
        fread_to_eol( pFile ); /* 一定先讀完一行 */
        pQuestion->title = fread_string( pFile );
        break;
      }

      break;
    }

    /* 指令型態錯誤 */
    if ( !fMatch )
      mudlog( LOG_DEBUG , "Load_a_question﹕命令 %s 不正確。" , word );
  }

  RETURN( NULL );
}

/* 載入解謎資料 */
void load_quest( const char * filename )
{
  QUEST_INFO * pQuest;
  FILE_DATA  * pFile;
  char       * word;
  bool         fMatch;
  bool         Finish = FALSE;

  PUSH_FUNCTION( "load_quest" );

  if ( ( pFile = f_open( filename , "r" ) ) )
  {
    while ( !if_eof( pFile ) )
    {
      word   = if_eof( pFile ) ? "End" : fread_word( pFile );
      fMatch = FALSE;

      switch ( UPPER( word[0] ) )
      {
      /* 註解 */
      case '*':
        fMatch = TRUE;
        fread_to_eol( pFile );
        break;

      case '#':

        if ( !str_scmp( word, "#Quest", &fMatch ) )
        {
          pQuest = load_a_quest( pFile );

          pQuest->next = quest_list;
          quest_list   = pQuest;

          if ( !fReload ) top_quest++;
          break;
        }

        break;

      case 'E':

        /* 結束 */
        if ( !str_scmp( word, "End", &fMatch ) )
        {
          Finish = TRUE;
          break;
        }

        break;
      }

      /* 指令型態錯誤 */
      if ( !fMatch )
        mudlog( LOG_DEBUG , "Load_quest﹕命令 %s 不正確。" , word );

      if ( Finish ) break;
    }

    mudlog( LOG_INFO, "系統載入 %d 個解謎資料。" , top_quest );
    f_close( pFile );
  }

  else
  {
    mudlog( LOG_INFO, "沒有載入任何解謎的資料。" );
  }

  RETURN_NULL();
}

/* 讀取一個解謎資料 */
QUEST_INFO * load_a_quest( FILE_DATA * pFile )
{
  QUEST_INFO * pQuest;
  char       * word;
  bool         fMatch;

  PUSH_FUNCTION( "load_a_quest" );

  set_quest_default( pQuest = alloc_struct( STRUCT_QUEST_INFO ) );

  for ( ;; )
  {
    word   = if_eof( pFile ) ? "End" : fread_word( pFile );
    fMatch = FALSE;

    switch ( UPPER( word[0] ) )
    {
    case '*':

      fMatch = TRUE;
      fread_to_eol( pFile );
      break;

    case 'E':

      if ( !str_scmp( word, "End", &fMatch ) )
      {
        if ( !pQuest->mark )
          mudlog( LOG_DEBUG, "Load_a_quest﹕沒有解謎關鍵字。" );

        if ( !pQuest->info )
          mudlog( LOG_DEBUG, "Load_a_quest﹕沒有解謎解說資訊。" );

        if ( pQuest->show && !pQuest->help[0] )
          mudlog( LOG_DEBUG, "Load_a_quest﹕顯示的任務必須要有提示。" );

        RETURN( pQuest );
      }

      break;

    case 'H':

      if ( !str_scmp( word , "Help" , &fMatch ) )
      {
        fread_to_eol( pFile ); /* 一定先讀完一行 */
        pQuest->help = fread_string( pFile );
        break;
      }

      break;

    case 'I':
      KEY( "Information", pQuest->info, fread_string( pFile ) );
      break;

    case 'K':

      if ( !str_scmp( word, "Keyword", &fMatch ) )
      {
        if ( quest_lookup( pQuest->mark = fread_string( pFile ) ) )
          mudlog( LOG_ERR, "Load_a_quest﹕解謎關鍵字%s重覆。", pQuest->mark );

        break;
      }
      break;

    case 'S':

      if ( !str_scmp( word , "Show", &fMatch ) )
      {
        pQuest->show = ( fread_number( pFile ) == TRUE ) ? TRUE : FALSE;
        break;
      }
      break;

    case 'T':

      if ( !str_scmp( word, "Title", &fMatch ) )
      {
        pQuest->title = ( fread_number( pFile ) == TRUE ) ? TRUE : FALSE;
        break;
      }
      break;

    }

    /* 指令型態錯誤 */
    if ( !fMatch )
      mudlog( LOG_DEBUG , "Load_a_quest﹕命令 %s 不正確。" , word );
  }

  RETURN( NULL );
}

/* 載入解謎資料 */
void load_event( const char * filename )
{
  EVENT_DATA * pEvent;
  FILE_DATA  * pFile;
  char       * word;
  bool         fMatch;
  bool         Finish = FALSE;

  PUSH_FUNCTION( "load_event" );

  if ( ( pFile = f_open( filename , "r" ) ) )
  {
    while ( !if_eof( pFile ) )
    {
      word   = if_eof( pFile ) ? "End" : fread_word( pFile );
      fMatch = FALSE;

      switch ( UPPER( word[0] ) )
      {
      /* 註解 */
      case '*':
        fMatch = TRUE;
        fread_to_eol( pFile );
        break;

      case '#':

        if ( !str_scmp( word, "#Event", &fMatch ) )
        {
          pEvent = load_a_event( pFile );

          pEvent->next = event_list;
          event_list   = pEvent;

          if ( !fReload ) top_event++;
          break;
        }

        break;

      case 'E':

        /* 結束 */
        if ( !str_scmp( word, "End", &fMatch ) )
        {
          Finish = TRUE;
          break;
        }

        break;
      }

      /* 指令型態錯誤 */
      if ( !fMatch )
        mudlog( LOG_DEBUG , "Load_event﹕命令 %s 不正確。" , word );

      if ( Finish ) break;
    }

    mudlog( LOG_INFO, "系統載入 %d 個觸發事件。" , top_event );
    f_close( pFile );
  }

  else
  {
    mudlog( LOG_INFO, "沒有載入任何觸發事件。" );
  }

  RETURN_NULL();
}

/* 讀取一個觸發事件 */
EVENT_DATA * load_a_event( FILE_DATA * pFile )
{
  EVENT_DATA * zEvent;
  EVENT_DATA * pEvent;
  char       * word;
  char       * keyword;
  bool         fMatch;
  int          chance;

  PUSH_FUNCTION( "load_a_event" );

  set_event_default( pEvent = alloc_struct( STRUCT_EVENT_DATA ) );

  for ( ;; )
  {
    word   = if_eof( pFile ) ? "End" : fread_word( pFile );
    fMatch = FALSE;

    switch ( UPPER( word[0] ) )
    {
    case '*':

      fMatch = TRUE;
      fread_to_eol( pFile );
      break;

    case 'C':

      if ( !str_scmp( word, "Chance", &fMatch ) )
      {
        if ( ( chance = fread_number( pFile ) ) <= 0 || chance > 1000 )
          mudlog( LOG_DEBUG, "Load_a_event﹕出現機率 %d 不合理。", chance );

        pEvent->chance = chance;
        break;
      }

      break;

    case 'E':

      if ( !str_scmp( word, "End", &fMatch ) )
      {
        if ( IS_ERROR( pEvent->chance ) )
          mudlog( LOG_DEBUG, "Load_a_event﹕沒有設定觸發機率。" );

        if ( !pEvent->function )
          mudlog( LOG_DEBUG, "Load_a_event﹕沒有觸發處理函數。" );

        if ( !pEvent->title )
          mudlog( LOG_DEBUG, "Load_a_event﹕沒有觸發名稱。" );

        if ( !pEvent->keyword )
          mudlog( LOG_DEBUG, "Load_a_event﹕缺乏觸發關鍵字。" );

        RETURN( pEvent );
      }

      break;

    case 'F':

      if ( !str_scmp( word , "Function", &fMatch ) )
      {
        EVENT_FUN * function;

        keyword = fread_string( pFile );

        if ( keyword[0] )
        {
          if ( !( function = event_function( keyword ) ) )
            mudlog( LOG_DEBUG , "Load_a_event﹕觸發事件的函數 %s 找不到。"
              , keyword );

          pEvent->function = function;
          free_string( keyword );
        }

        break;
      }

      break;

    case 'K':
      if ( !str_scmp( word, "Keyword", &fMatch ) )
      {
        keyword = fread_string( pFile );

        for ( zEvent = event_list; zEvent; zEvent = zEvent->next )
        {
          if ( !str_cmp( keyword, zEvent->keyword ) )
            mudlog( LOG_DEBUG, "Load_a_event﹕關鍵字 %s 重複。", keyword );
        }

        pEvent->keyword = str_dup( keyword );
        free_string( keyword );
        break;
      }

      break;

    case 'L':

      if ( !str_scmp( word , "Lock", &fMatch ) )
      {
        pEvent->lock = fread_number( pFile ) == TRUE ? TRUE : FALSE;
        break;
      }
      break;

    case 'T':
      KEY( "Title", pEvent->title, fread_string( pFile ) );
      break;
    }

    /* 指令型態錯誤 */
    if ( !fMatch )
      mudlog( LOG_DEBUG , "Load_a_event﹕命令 %s 不正確。" , word );
  }

  RETURN( NULL );
}

/* 載入渡船資料 */
void load_ship( const char * filename )
{
  SHIP_DATA    * pShip;
  FILE_DATA    * pFile;
  char         * word;
  bool           fMatch;
  bool           Finish = FALSE;

  PUSH_FUNCTION( "load_ship" );

  if ( ( pFile = f_open( filename , "r" ) ) )
  {
    while ( !if_eof( pFile ) )
    {
      word   = if_eof( pFile ) ? "End" : fread_word( pFile );
      fMatch = FALSE;

      switch ( UPPER( word[0] ) )
      {
      /* 註解 */
      case '*':
        fMatch = TRUE;
        fread_to_eol( pFile );
        break;

      case '#':

        if ( !str_scmp( word, "#Ship", &fMatch ) )
        {
          /* 配置記憶體以及設定預設值 */
          pShip = load_a_ship( pFile );

          pShip->next = ship_list;
          ship_list   = pShip;

          if ( !fReload ) top_ship++;
          pShip->waiting_tick = pShip->waiting;
          break;
        }

        break;

      case 'E':

        /* 結束 */
        if ( !str_scmp( word, "End", &fMatch ) )
        {
          Finish = TRUE;
          break;
        }

        break;
      }

      /* 指令型態錯誤 */
      if ( !fMatch )
        mudlog( LOG_DEBUG , "Load_ship﹕命令 %s 不正確。", word );

      if ( Finish ) break;
    }

    mudlog( LOG_INFO, "系統載入 %d 個渡船資料。" , top_ship );
    f_close( pFile );
  }

  else
  {
    mudlog( LOG_INFO, "沒有載入任何渡船的資料。" );
  }

  RETURN_NULL();
}

/* 讀取一艘渡船資料 */
SHIP_DATA * load_a_ship( FILE_DATA * pFile )
{
  char            * word;
  ROOM_INDEX_DATA * pRoom;
  SHIP_DATA       * aShip;
  SHIP_DATA       * pShip;
  int               slot;
  int               loop;
  int               iHash;
  bool              fMatch;

  PUSH_FUNCTION( "load_a_ship" );

  set_ship_default( pShip = alloc_struct( STRUCT_SHIP_DATA ) );

  for ( ;; )
  {
    word   = if_eof( pFile ) ? "End" : fread_word( pFile );
    fMatch = FALSE;

    switch ( UPPER( word[0] ) )
    {
    case '*':

      fMatch = TRUE;
      fread_to_eol( pFile );
      break;

    case 'C':

      if ( !str_scmp( word, "Cabin", &fMatch ) )
      {
        if ( pShip->cabin )
          mudlog( LOG_DEBUG, "Load_a_ship﹕船艙重複。" );

        if ( get_room_index( slot = fread_number( pFile ) ) )
          mudlog( LOG_DEBUG, "Load_a_ship﹕船艙本身存在。", slot );

        if ( !pShip->description )
          mudlog( LOG_DEBUG, "Load_a_ship﹕沒有船艙名稱。" );

        if ( !pShip->description )
          mudlog( LOG_DEBUG, "Load_a_ship﹕沒有船艙敘述。" );

        set_room_default( pRoom = alloc_struct( STRUCT_ROOM_INDEX_DATA ) );

        pRoom->people          = NULL;
        pRoom->contents        = NULL;
        pRoom->extra_descr     = NULL;
        pRoom->club            = NULL;
        pRoom->job             = NULL;
        pRoom->area            = DefaultArea;
        pRoom->name            = str_dup( pShip->name );
        pRoom->description     = str_dup( pShip->description );
        pRoom->filename        = str_dup( "系統建立" );
        pRoom->vnum            = slot;
        pRoom->Safe            = TRUE;
        pRoom->NoRecall        = TRUE;
        pRoom->NoWhere         = TRUE;
        pRoom->NoQuit          = TRUE;
        pRoom->Sail            = TRUE;
        pRoom->NoMob           = TRUE;
        pRoom->light           = 0;
        pRoom->sector          = DefaultSector;
        iHash                  = pRoom->vnum % MAX_KEY_HASH;
        pRoom->next            = room_index_hash[iHash];
        room_index_hash[iHash] = pRoom;
        if ( !fReload ) top_room++;

        for ( loop = 0; loop < DIR_MAX; loop++ ) pRoom->exit[loop] = NULL;

        pShip->cabin       = pRoom;
        break;
      }

      if ( !str_scmp( word, "Cost", &fMatch ) )
      {
        if ( ( slot = fread_number( pFile ) ) <= 0 || slot >= MAX_ASSET )
          mudlog( LOG_DEBUG, "Load_a_ship﹕渡費 %d 不合理。", slot );

        pShip->cost = slot;
        break;
      }

      break;

    case 'D':

      if ( !str_scmp( word , "Description", &fMatch ) )
      {
        if ( pShip->description )
          mudlog( LOG_DEBUG, "Load_a_ship﹕船艙敘述重複。" );

        fread_to_eol( pFile ); /* 一定先讀完一行 */
        pShip->description = fread_string( pFile );
        break;
      }

      if ( !str_scmp( word, "Destination" , &fMatch ) )
      {
        if ( pShip->destination )
          mudlog( LOG_DEBUG, "Load_a_ship﹕目的地重複。" );

        if ( !( pRoom = get_room_index( slot = fread_number( pFile ) ) ) )
          mudlog( LOG_DEBUG, "Load_a_ship﹕找不到目的地點 %d。", slot );

        for ( aShip = ship_list; aShip; aShip = aShip->next )
        {
          if ( aShip->destination == pRoom )
            mudlog( LOG_DEBUG, "Load_a_ship﹕目的地 %d 重複。", slot );
        }

        pShip->destination = pRoom;
        break;
      }

      if ( !str_scmp( word, "Delay", &fMatch ) )
      {
        if ( ( slot = fread_number( pFile ) ) < 0 || slot >= 10000 )
          mudlog( LOG_DEBUG, "Load_a_ship﹕延期機率 %d 不合理。", slot );

        pShip->delay = slot;
        break;
      }

      break;

    case 'E':

      if ( !str_scmp( word, "End", &fMatch ) )
      {
        if ( !pShip->name )
          mudlog( LOG_DEBUG, "Load_a_ship﹕沒有渡船名稱。" );

        if ( !pShip->msg_entrance )
          mudlog( LOG_DEBUG, "Load_a_ship﹕沒有靠岸敘述。" );

        if ( !pShip->msg_land )
          mudlog( LOG_DEBUG, "Load_a_ship﹕沒有入港敘述。" );

        if ( !pShip->description )
          mudlog( LOG_DEBUG, "Load_a_ship﹕沒有船艙敘述。" );

        if ( !pShip->starting )
          mudlog( LOG_DEBUG, "Load_a_ship﹕沒有出發點。" );

        if ( !pShip->destination )
          mudlog( LOG_DEBUG, "Load_a_ship﹕沒有目的地。" );

        if ( !pShip->cabin )
          mudlog( LOG_DEBUG, "Load_a_ship﹕沒有船艙。" );

        if ( IS_ERROR( pShip->cost ) )
          mudlog( LOG_DEBUG, "Load_a_ship﹕沒有費用。" );

        if ( IS_ERROR( pShip->sailing ) )
          mudlog( LOG_DEBUG, "Load_a_ship﹕沒有航行時間。" );

        if ( IS_ERROR( pShip->waiting ) )
          mudlog( LOG_DEBUG, "Load_a_ship﹕沒有等待時間。" );

        RETURN( pShip );
      }

      if ( !str_scmp( word , "Entrance", &fMatch ) )
      {
        if ( pShip->msg_entrance )
          mudlog( LOG_DEBUG, "Load_a_ship﹕靠港敘述重複。" );

        fread_to_eol( pFile ); /* 一定先讀完一行 */
        pShip->msg_entrance = fread_string( pFile );
        break;
      }

      break;

    case 'L':

      if ( !str_scmp( word , "Land", &fMatch ) )
      {
        if ( pShip->msg_land )
          mudlog( LOG_DEBUG, "Load_a_ship﹕入港敘述重複。" );

        fread_to_eol( pFile ); /* 一定先讀完一行 */
        pShip->msg_land = fread_string( pFile );
        break;
      }

      break;

    case 'N':
      KEY( "Name", pShip->name, fread_string( pFile ) );
      break;

    case 'P':

      if ( !str_scmp( word, "Pirate", &fMatch ) )
      {
        if ( ( slot = fread_number( pFile ) ) < 0 || slot >= 10000 )
          mudlog( LOG_DEBUG, "Load_a_ship﹕海盜出現機率 %d 不合理。", slot );

        pShip->pirate = slot;
        break;
      }

      break;

    case 'S':

      if ( !str_scmp( word, "Sailing", &fMatch ) )
      {
        if ( ( slot = fread_number( pFile ) ) <= 0 )
          mudlog( LOG_DEBUG, "Load_a_ship﹕航行時間 %d 不合理。", slot );

        pShip->sailing = slot;
        break;
      }

      if ( !str_scmp( word, "Starting", &fMatch ) )
      {
        if ( pShip->starting )
          mudlog( LOG_DEBUG, "Load_a_ship﹕出發點重複。" );

        if ( !( pRoom = get_room_index( slot = fread_number( pFile ) ) ) )
          mudlog( LOG_DEBUG, "Load_a_ship﹕找不到出發點 %d。", slot );

        for ( aShip = ship_list; aShip; aShip = aShip->next )
        {
          if ( aShip->starting == pRoom )
            mudlog( LOG_DEBUG, "Load_a_ship﹕出發點 %d 重複。", slot );
        }

        pShip->starting = pRoom;
        break;
      }

      break;

    case 'W':

      if ( !str_scmp( word, "Waiting", &fMatch ) )
      {
        if ( ( slot = fread_number( pFile ) ) <= 0 )
          mudlog( LOG_DEBUG, "Load_a_ship﹕等待渡船時間 %d 不合理。", slot );

        pShip->waiting = slot;
        break;
      }
      break;
    }

    /* 指令型態錯誤 */
    if ( !fMatch )
      mudlog( LOG_DEBUG , "Load_a_ship﹕命令 %s 不正確。" , word );
  }

  RETURN( NULL );
}

/* 讀取拍賣品檔案資料 */
void load_situs( const char * filename )
{
  SITUS_DATA * pSitus;
  SITUS_DATA * zSitus;
  FILE_DATA  * pFile;
  char       * word;
  bool         fMatch;
  bool         Finish = FALSE;

  PUSH_FUNCTION( "load_situs" );

  if ( ( pFile = f_open( filename , "r" ) ) )
  {
    while ( !if_eof( pFile ) )
    {
      word   = if_eof( pFile ) ? "End" : fread_word( pFile );
      fMatch = FALSE;

      switch ( UPPER( word[0] ) )
      {
      /* 註解 */
      case '*':
        fMatch = TRUE;
        fread_to_eol( pFile );
        break;

      case '#':

        if ( !str_scmp( word, "#Situs", &fMatch ) )
        {
          pSitus = load_a_situs( pFile );

          for ( zSitus = situs_list;
                zSitus && zSitus->next;
                zSitus = zSitus->next );


          if ( !zSitus ) situs_list   = pSitus;
          else           zSitus->next = pSitus;

          if ( !fReload ) top_situs++;
          break;
        }

        break;

      case 'E':

        /* 結束 */
        if ( !str_scmp( word, "End", &fMatch ) )
        {
          Finish = TRUE;
          break;
        }

        break;
      }

      /* 指令型態錯誤 */
      if ( !fMatch )
        mudlog( LOG_DEBUG , "Load_situs﹕命令 %s 不正確。" , word );

      if ( Finish ) break;
    }

    mudlog( LOG_INFO, "系統載入 %d 個部位資料。" , top_situs );
    f_close( pFile );

    for ( pSitus = situs_list; pSitus; pSitus = pSitus->next )
    {
      if ( pSitus->associate_vnum != ERRORCODE )
      {
        for ( zSitus = situs_list; zSitus; zSitus = zSitus->next )
          if ( zSitus->location == pSitus->associate_vnum ) break;

        if ( !zSitus || zSitus == pSitus )
          mudlog( LOG_DEBUG, "Load_situs﹕%d 部位聯集錯誤。", pSitus->location );

        else
          pSitus->associate = zSitus;
      }

      else
      {
        pSitus->associate = NULL;
      }
    }
  }

  else
  {
    mudlog( LOG_INFO, "沒有載入任何部位的資料。" );
  }

  RETURN_NULL();
}

/* 讀取一個部位資料 */
SITUS_DATA * load_a_situs( FILE_DATA * pFile )
{
  SITUS_DATA * pSitus;
  char       * word;
  bool         fMatch;

  PUSH_FUNCTION( "load_a_situs" );

  set_situs_default( pSitus = alloc_struct( STRUCT_SITUS_DATA ) );

  for ( ;; )
  {
    word   = if_eof( pFile ) ? "End" : fread_word( pFile );
    fMatch = FALSE;

    switch ( UPPER( word[0] ) )
    {
    case '*':

      fMatch = TRUE;
      fread_to_eol( pFile );
      break;

    case 'A':
      KEY( "Attack"   , pSitus->attack        , fread_number( pFile ) );
      KEY( "Associate", pSitus->associate_vnum, fread_number( pFile ) );
      KEY( "ApplyAc"  , pSitus->apply_ac      , fread_number( pFile ) );
      break;

    case 'C':
      KEY( "Chance", pSitus->chance, fread_number( pFile ) );
      break;

    case 'E':

      KEY( "EqName", pSitus->eq_name, fread_string( pFile ) );

      if ( !str_scmp( word, "End", &fMatch ) )
      {
        if ( IS_ERROR( pSitus->location ) || pSitus->location == WEAR_NONE )
          mudlog( LOG_DEBUG, "Load_a_situs﹕部位位置未設定。" );

        if ( IS_ERROR( pSitus->chance ) || pSitus->chance < 0 )
          mudlog( LOG_DEBUG, "Load_a_situs﹕部位機率未設定。" );

        if ( IS_ERROR( pSitus->wear ) )
          mudlog( LOG_DEBUG, "Load_a_situs﹕部位穿著旗標未設定。" );

        if ( IS_ERROR( pSitus->attack ) )
          mudlog( LOG_DEBUG, "Load_a_situs﹕部位攻擊旗標未設定。" );

        if ( !pSitus->eq_name || !*pSitus->eq_name )
          mudlog( LOG_DEBUG, "Load_a_situs﹕部位裝備名稱未設定。" );

        if ( !pSitus->situs_name || !*pSitus->situs_name )
          mudlog( LOG_DEBUG, "Load_a_situs﹕部位名稱未設定。" );

        if ( !pSitus->short_descr || !*pSitus->short_descr )
          mudlog( LOG_DEBUG, "Load_a_situs﹕部位名稱未設定。" );

        if ( !pSitus->name || !*pSitus->name )
          mudlog( LOG_DEBUG, "Load_a_situs﹕部位名稱未設定。" );

        if ( !pSitus->msg1 || !*pSitus->msg1 )
          mudlog( LOG_DEBUG, "Load_a_situs﹕部位穿著敘述一未設定。" );

        if ( !pSitus->msg2 || !*pSitus->msg2 )
          mudlog( LOG_DEBUG, "Load_a_situs﹕部位穿著敘述二未設定。" );

        if ( pSitus->apply_ac < 0 )
          mudlog( LOG_DEBUG, "Load_a_situs﹕部位防禦加強倍數(%d)不合法。"
             , pSitus->apply_ac );

        if ( pSitus->attack == ATTACK_RANDOM )
          mudlog( LOG_DEBUG, "Load_a_situs﹕部位攻擊旗標不能為 %d。", ATTACK_RANDOM );

        if ( pSitus->wear == WEAR_NONE )
          mudlog( LOG_DEBUG, "Load_a_situs﹕部位穿戴位置不能為 %d。", WEAR_NONE );

        RETURN( pSitus );
      }

      break;

    case 'I':
      KEY( "ItemType", pSitus->item_type, fread_number( pFile ) );
      break;

    case 'L':
      KEY( "Location", pSitus->location, fread_number( pFile ) );
      break;

    case 'M':

      KEY( "Message1", pSitus->msg1, fread_string( pFile ) );
      KEY( "Message2", pSitus->msg2, fread_string( pFile ) );
      break;

    case 'N':
      KEY( "Name", pSitus->name, fread_string( pFile ) );
      break;

    case 'S':
      KEY( "Situs"     , pSitus->situs_name , fread_string( pFile ) );
      KEY( "ShortDescr", pSitus->short_descr, fread_string( pFile ) );
      break;

    case 'T':
      KEY( "Type", pSitus->type, fread_string( pFile ) );
      break;

    case 'W':
      KEY( "Wear" , pSitus->wear , fread_number( pFile ) );
      KEY( "Wield", pSitus->wield, fread_number( pFile ) );
      break;
    }

    /* 指令型態錯誤 */
    if ( !fMatch )
      mudlog( LOG_DEBUG , "Load_a_situs﹕命令 %s 不正確。" , word );
  }

  RETURN( NULL );
}

/* 讀取拍賣品檔案資料 */
void load_sale( const char * filename )
{
  SALE_DATA * pSale;
  FILE_DATA * pFile;
  char      * word;
  bool        fMatch;
  bool        Finish = FALSE;

  PUSH_FUNCTION( "load_sale" );

  if ( ( pFile = f_open( filename , "r" ) ) )
  {
    while ( !if_eof( pFile ) )
    {
      word   = if_eof( pFile ) ? "End" : fread_word( pFile );
      fMatch = FALSE;

      switch ( UPPER( word[0] ) )
      {
      /* 註解 */
      case '*':
        fMatch = TRUE;
        fread_to_eol( pFile );
        break;

      case '#':

        if ( !str_scmp( word, "#Sale", &fMatch ) )
        {
          pSale = load_a_sale( pFile );

          pSale->next = sale_list;
          sale_list   = pSale;

          if ( !fReload ) top_sale++;
          break;
        }

        break;

      case 'C':

        if ( !str_scmp( word, "Chance", &fMatch ) )
        {
          if ( ( SaleChance = fread_number( pFile ) ) <= 0 )
            mudlog( LOG_DEBUG , "Load_sale﹕拍賣機率 %d 不合法。", SaleChance );
          break;
        }

        break;

      case 'E':

        /* 結束 */
        if ( !str_scmp( word, "End", &fMatch ) )
        {
          if ( SaleChance <= 0 )
            mudlog( LOG_DEBUG , "Load_sale﹕拍賣機率 %d 不合法。", SaleChance );

          Finish = TRUE;
          break;
        }

        break;

      case 'L':

        if ( !str_scmp( word, "Limit", &fMatch ) )
        {
          if ( ( SaleLimit = fread_number( pFile ) ) <= 0 )
            mudlog( LOG_DEBUG , "Load_sale﹕拍賣下限人數 %d 不合法。", SaleLimit );

          break;
        }

        break;

      }

      /* 指令型態錯誤 */
      if ( !fMatch )
        mudlog( LOG_DEBUG , "Load_sale﹕命令 %s 不正確。" , word );

      if ( Finish ) break;
    }

    mudlog( LOG_INFO, "系統載入 %d 個拍賣品。" , top_sale );
    f_close( pFile );
  }

  else
  {
    mudlog( LOG_INFO, "沒有載入任何拍賣品的資料。" );
  }

  RETURN_NULL();
}

/* 讀取一個拍賣品資料 */
SALE_DATA * load_a_sale( FILE_DATA * pFile )
{
  OBJ_INDEX_DATA * pIndex;
  SALE_DATA      * pSale;
  char           * word;
  bool             fMatch;
  int              vnum;

  PUSH_FUNCTION( "load_a_sale" );

  set_sale_default( pSale = alloc_struct( STRUCT_SALE_DATA ) );

  for ( ;; )
  {
    word   = if_eof( pFile ) ? "End" : fread_word( pFile );
    fMatch = FALSE;

    switch ( UPPER( word[0] ) )
    {
    case '*':

      fMatch = TRUE;
      fread_to_eol( pFile );
      break;

    case 'C':

      if ( !str_scmp( word , "Cost", &fMatch ) )
      {
        if ( ( pSale->cost = fread_number( pFile ) ) <= 0 )
          mudlog( LOG_DEBUG, "Load_a_sale﹕拍賣品售價 %d 不合理。"
            , pSale->cost );

        break;
      }

      break;

    case 'E':

      if ( !str_scmp( word, "End", &fMatch ) )
      {
        if ( !pSale->obj ) mudlog( LOG_DEBUG, "Load_a_sale﹕沒有拍賣品。" );

        if ( pSale->obj->Takeable == FALSE )
          mudlog( LOG_DEBUG, "Load_a_sale﹕物品不是可以拍賣﹗" );

        if ( IS_ERROR( pSale->cost ) )
          mudlog( LOG_DEBUG, "Load_a_sale﹕物品沒有價格﹗" );

        RETURN( pSale );
      }

      break;

    case 'O':

      if ( !str_scmp( word, "Object", &fMatch ) )
      {
        if ( !( pIndex = get_obj_index( vnum = fread_number( pFile ) ) ) )
          mudlog( LOG_DEBUG, "Load_a_sale﹕沒有拍賣品號碼 %d 。", vnum );

        pSale->obj = pIndex;
        break;
      }

      break;

    case 'V':

      if ( !str_scmp( word , "Visible", &fMatch ) )
      {
        pSale->visible = ( fread_number( pFile ) == TRUE ) ? TRUE : FALSE;
        break;
      }

      break;
    }

    /* 指令型態錯誤 */
    if ( !fMatch )
      mudlog( LOG_DEBUG , "Load_a_sale﹕命令 %s 不正確。" , word );
  }

  RETURN( NULL );
}

/* 載入神族資料 */
void load_immlist( const char * filename )
{
  IMMLIST_DATA * pImmlist;
  FILE_DATA    * pFile;
  char         * word;
  bool           fMatch;
  bool           Finish = FALSE;

  PUSH_FUNCTION( "load_immlist" );

  if ( ( pFile = f_open( filename , "r" ) ) )
  {
    while ( !if_eof( pFile ) )
    {
      word   = if_eof( pFile ) ? "End" : fread_word( pFile );
      fMatch = FALSE;

      switch ( UPPER( word[0] ) )
      {
      /* 註解 */
      case '*':
        fMatch = TRUE;
        fread_to_eol( pFile );
        break;

      case '#':

        if ( !str_scmp( word, "#Immortal", &fMatch ) )
        {
          pImmlist = load_a_immlist( pFile );

          if ( !imm_list ) imm_list       = pImmlist;
          if ( imm_last  ) imm_last->next = pImmlist;
          imm_last       = pImmlist;
          pImmlist->next = NULL;

          if ( !fReload ) top_immlist++;
          break;
        }

        break;

      case 'E':

        /* 結束 */
        if ( !str_scmp( word, "End", &fMatch ) )
        {
          Finish = TRUE;
          break;
        }

        break;
      }

      /* 指令型態錯誤 */
      if ( !fMatch )
        mudlog( LOG_DEBUG , "Load_immlist﹕命令 %s 不正確。" , word );

      if ( Finish ) break;
    }

    mudlog( LOG_INFO, "系統載入 %d 個神族資料。" , top_immlist );
    f_close( pFile );
  }

  else
  {
    mudlog( LOG_INFO, "沒有載入任何神族的資料。" );
  }

  RETURN_NULL();
}

/* 讀取一個神族資料 */
IMMLIST_DATA * load_a_immlist( FILE_DATA * pFile )
{
  IMMLIST_DATA * pImmlist;
  char         * word;
  bool           fMatch;

  PUSH_FUNCTION( "load_a_immlist" );

  set_immlist_default( pImmlist = alloc_struct( STRUCT_IMMLIST_DATA ) );

  for ( ;; )
  {
    word   = if_eof( pFile ) ? "End" : fread_word( pFile );
    fMatch = FALSE;

    switch ( UPPER( word[0] ) )
    {
    case '*':

      fMatch = TRUE;
      fread_to_eol( pFile );
      break;

    case 'A':

      if ( !str_scmp( word, "Adviser", &fMatch ) )
      {
        int adviser;

        adviser = fread_number( pFile );
        if ( adviser < 0 || adviser > 1 )
          mudlog( LOG_DEBUG, "Load_a_immlist﹕顧問資格不合理。" );

        pImmlist->adviser = adviser;
        break;
      }
      break;

    case 'D':

      if ( !str_scmp( word , "Description", &fMatch ) )
      {
        fread_to_eol( pFile ); /* 一定先讀完一行 */
        pImmlist->description = fread_string( pFile );
        break;
      }

      break;

    case 'E':

      if ( !str_scmp( word, "End", &fMatch ) )
      {
        if ( !pImmlist->name )
          mudlog( LOG_DEBUG, "Load_a_immlist﹕沒有神族名稱。" );

        if ( !pImmlist->description )
          mudlog( LOG_DEBUG, "Load_a_immlist﹕沒有解說敘述。" );

        if ( IS_ERROR( pImmlist->level ) && IS_ERROR( pImmlist->trust ) )
          mudlog( LOG_DEBUG, "Load_a_immlist﹕沒有等級或是信賴等級。" );

        RETURN( pImmlist );
      }

      break;

    case 'L':

      if ( !str_scmp( word, "Level", &fMatch ) )
      {
        int level;

        level = fread_number( pFile );
        if ( level <= LEVEL_HERO || level > MAX_LEVEL )
          mudlog( LOG_DEBUG, "Load_a_immlist﹕等級不合理。" );

        pImmlist->level = level;
        break;
      }
      break;

    case 'N':

      if ( !str_scmp( word, "Name", &fMatch ) )
      {
        pImmlist->name = fread_string( pFile );

        if ( !is_exist( pImmlist->name ) )
          mudlog( LOG_DEBUG, "Load_a_immlist﹕名稱%s不存在。", pImmlist->name );

        break;
      }
      break;

    case 'T':

      if ( !str_scmp( word, "Trust", &fMatch ) )
      {
        int trust;

        trust = fread_number( pFile );
        if ( trust <= LEVEL_HERO || trust > MAX_LEVEL )
          mudlog( LOG_DEBUG, "Load_a_immlist﹕信賴等級不合理。" );

        pImmlist->trust = trust;
        break;
      }
      break;
    }

    /* 指令型態錯誤 */
    if ( !fMatch )
      mudlog( LOG_DEBUG , "Load_a_immlist﹕命令 %s 不正確。" , word );
  }

  RETURN( NULL );
}

/* 載入股票資料 */
void load_stock( const char * filename )
{
  extern int   stock_win;
  extern int   stock_lost;
  FILE_DATA  * pFile;
  char       * word;
  bool         fMatch;
  bool         Finish = FALSE;
  int          count = 0;
  int          loop;
  int          cycle;

  PUSH_FUNCTION( "load_stock" );

  /* 先清除陣列資料 */
  for ( loop = 0; loop < MAX_STOCK; loop++ )
  {
    stock_data[loop].cost       = 0;
    stock_data[loop].name       = NULL;
    stock_data[loop].buy        = 0;
    stock_data[loop].sell       = 0;
    stock_data[loop].today_sell = 0;
    stock_data[loop].today_buy  = 0;
    stock_data[loop].win        = stock_win;
    stock_data[loop].lost       = stock_lost;
    stock_data[loop].lock       = FALSE;

    for ( cycle = 0; cycle < MAX_STOCK_HISTORY; cycle++ )
      stock_data[loop].history[cycle] = -1;
  }

  if ( ( pFile = f_open( filename , "r" ) ) )
  {
    while ( !if_eof( pFile ) )
    {
      word   = if_eof( pFile ) ? "End" : fread_word( pFile );
      fMatch = FALSE;

      switch ( UPPER( word[0] ) )
      {
      /* 註解 */
      case '*':
        fMatch = TRUE;
        fread_to_eol( pFile );
        break;

      case 'E':

        /* 結束 */
        if ( !str_scmp( word, "End", &fMatch ) )
        {
          Finish = TRUE;
          break;
        }
        break;

      case 'S':

        if ( !str_scmp( word, "Stock", &fMatch ) )
        {
          int slot;

          slot = fread_number( pFile ) - 1;
          if ( slot < 0 || slot >= MAX_STOCK )
            mudlog( LOG_DEBUG, "Load_stock﹕號碼 %d 錯誤。", slot );

          if ( stock_data[slot].name )
            mudlog( LOG_DEBUG, "Load_stock﹕號碼 %d 重複。", slot );

          stock_data[slot].name       = str_dup( fread_word( pFile ) );
          stock_data[slot].cost       = fread_number( pFile );
          stock_data[slot].history[0] = stock_data[slot].cost;

          stock_data[slot].sell    = fread_number( pFile );
          stock_data[slot].buy     = fread_number( pFile );
          count++;
          break;
        }

        break;
      }

      /* 指令型態錯誤 */
      if ( !fMatch )
        mudlog( LOG_DEBUG , "Load_stock﹕命令 %s 不正確。" , word );

      if ( Finish ) break;
    }

    mudlog( LOG_INFO, "系統載入 %d 個股票資料。" , count );
    f_close( pFile );
  }

  else
  {
    mudlog( LOG_INFO, "沒有載入任何股票的資料。" );
  }

  RETURN_NULL();
}

/* 載入日期資料 */
void load_date( const char * filename )
{
  FILE_DATA  * pFile;
  char       * word;
  bool         fMatch;
  bool         Finish = FALSE;
  int          loop;

  PUSH_FUNCTION( "load_date" );

  /* 先清除資料 */
  for ( loop = 0; loop < MAX_DAY;   loop++ ) DayName[loop]   = str_dup( "" );
  for ( loop = 0; loop < MAX_MONTH; loop++ ) MonthName[loop] = str_dup( "" );
  for ( loop = 0; loop < MAX_HOUR;  loop++ ) HourName[loop]  = str_dup( "" );

  HoursPerDay   = 0;
  DaysPerMonth  = 0;
  MonthsPerYear = 0;

  if ( ( pFile = f_open( filename , "r" ) ) )
  {
    while ( !if_eof( pFile ) )
    {
      word   = if_eof( pFile ) ? "End" : fread_word( pFile );
      fMatch = FALSE;

      switch ( UPPER( word[0] ) )
      {
      /* 註解 */
      case '*':
        fMatch = TRUE;
        fread_to_eol( pFile );
        break;

      case 'D':

        if ( !str_scmp( word, "DayName", &fMatch ) )
        {
          if ( DaysPerMonth >= MAX_DAY )
            mudlog( LOG_ERR, "Load_date﹕日期數目已超過系統設定。" );

          DayName[DaysPerMonth++] = fread_string( pFile );
          break;
        }
        break;

      case 'E':

        /* 結束 */
        if ( !str_scmp( word, "End", &fMatch ) )
        {
          if ( DaysPerMonth <= 0 || MonthsPerYear <= 0 || HoursPerDay != 24 )
            mudlog( LOG_ERR, "Load_date﹕缺乏日期資料。" );

          Finish = TRUE;
          break;
        }

        break;

      case 'H':

        if ( !str_scmp( word, "HourName", &fMatch ) )
        {
          if ( HoursPerDay >= MAX_HOUR )
            mudlog( LOG_ERR, "Load_date﹕時辰數目已超過系統設定。" );

          HourName[HoursPerDay++] = fread_string( pFile );
          break;
        }
        break;

      case 'M':

        if ( !str_scmp( word, "MonthName", &fMatch ) )
        {
          if ( MonthsPerYear >= MAX_MONTH )
            mudlog( LOG_ERR, "Load_date﹕月份數目已超過系統設定。" );

          MonthName[MonthsPerYear++] = fread_string( pFile );
          break;
        }
        break;
      }

      /* 指令型態錯誤 */
      if ( !fMatch )
        mudlog( LOG_DEBUG , "Load_date﹕命令 %s 不正確。" , word );

      if ( Finish ) break;
    }

    f_close( pFile );
  }

  else
  {
    mudlog( LOG_ERR, "Load_date﹕沒有載入任何日期的資料。" );
  }

  RETURN_NULL();
}

/* 載入升級資料資料 */
void load_promotion( const char * filename )
{
  FILE_DATA * pFile;
  char * word;
  bool   fMatch;
  int    loop;
  int    level;
  int    count;

  PUSH_FUNCTION( "load_promotion" );

  /* 先清除陣列資料 */
  for ( loop = 0; loop < MAX_LEVEL; loop++ )
  {
    kill_table[loop].number    = 0;
    kill_table[loop].killed    = 0;
    kill_table[loop].promotion = 0;
    kill_table[loop].damage    = 0;
    kill_table[loop].dodge     = 0;
  }

  if ( ( pFile = f_open( filename , "r" ) ) )
  {
    while ( !if_eof( pFile ) )
    {
      word   = if_eof( pFile ) ? "End" : fread_word( pFile );
      fMatch = FALSE;

      switch ( UPPER( word[0] ) )
      {
      /* 註解 */
      case '*':

        fMatch = TRUE;
        fread_to_eol( pFile );
        break;

      case 'E':

        /* 結束 */
        if ( !str_scmp( word, "End", &fMatch ) )
        {
          f_close( pFile );
          RETURN_NULL();
        }
        break;

      case 'L':

        if ( !str_scmp( word, "Level", &fMatch ) )
        {
          if ( ( level = fread_number( pFile ) ) < 0 || level >= MAX_LEVEL )
            mudlog( LOG_DEBUG, "Load_promotion﹕等級 %d 不合理。", level );

          if ( ( count = fread_number( pFile ) ) < 0 )
            mudlog( LOG_DEBUG, "Load_promotion﹕數目 %d 不合理。", count );

          kill_table[level].promotion = count;

          if ( ( count = fread_number( pFile ) ) < 0 )
            mudlog( LOG_DEBUG, "Load_promotion﹕經驗值 %d 不合理。", count );

          kill_table[level].exp = count;

          kill_table[level].damage = fread_number( pFile );
          kill_table[level].dodge  = fread_number( pFile );

          break;
        }

        break;
      }

      /* 指令型態錯誤 */
      if ( !fMatch )
        mudlog( LOG_DEBUG , "Load_promotion﹕命令 %s 不正確。" , word );
    }

    f_close( pFile );
  }

  else
  {
    mudlog( LOG_DEBUG, "Load_promotion﹕沒有相關資料檔案。" );
  }

  RETURN_NULL();
}

/* 載入英雄資料 */
void load_hero( const char * filename )
{
  HERO_DATA * pHero;
  FILE_DATA * pFile;
  char      * word;
  bool        fMatch;
  bool        Finish = FALSE;
  int         count = 0;

  PUSH_FUNCTION( "load_hero" );

  if ( ( pFile = f_open( filename , "r" ) ) )
  {
    while ( !if_eof( pFile ) )
    {
      word   = if_eof( pFile ) ? "End" : fread_word( pFile );
      fMatch = FALSE;

      switch ( UPPER( word[0] ) )
      {
      /* 註解 */
      case '*':
        fMatch = TRUE;
        fread_to_eol( pFile );
        break;

      case 'E':

        /* 結束 */
        if ( !str_scmp( word, "End", &fMatch ) )
        {
          Finish = TRUE;
          break;
        }

        break;

      case 'H':

        if ( !str_scmp( word, "Hero", &fMatch ) )
        {
          pHero        = alloc_struct( STRUCT_HERO_DATA );
          pHero->name  = fread_string( pFile );
          pHero->cname = fread_string( pFile );
          pHero->class = fread_string( pFile );
          pHero->timer = fread_number( pFile );

          if ( is_exist( pHero->name ) )
          {
            if ( !hero_first ) hero_first      = pHero;
            if (  hero_last  ) hero_last->next = pHero;

            hero_last   = pHero;
            pHero->next = NULL;

            count++;
          }
          else
          {
            free_string( pHero->name  );
            free_string( pHero->cname );
            free_string( pHero->class );
            free_struct( pHero, STRUCT_HERO_DATA );
          }
          break;
        }

        break;
      }

      /* 指令型態錯誤 */
      if ( !fMatch )
        mudlog( LOG_DEBUG , "Load_hero﹕命令 %s 不正確。" , word );

      if ( Finish ) break;
    }

    mudlog( LOG_INFO, "系統載入 %d 個英雄資料。" , count );
    f_close( pFile );
    update_hero();
  }

  else
  {
    mudlog( LOG_INFO, "沒有載入任何英雄的資料。" );
  }

  RETURN_NULL();
}

/* 載入所有玩家資料庫資料 */
void load_database( const char * filename )
{
  DATABASE_DATA * pData;
  DATABASE_DATA * zData;
  FILE_DATA     * pFile;
  char          * word;
  bool            fMatch;
  bool            Finish = FALSE;
  int             count = 0;
  int             iHash;

  PUSH_FUNCTION( "load_database" );

  if ( ( pFile = f_open( filename , "r" ) ) )
  {
    while ( !if_eof( pFile ) )
    {
      word   = if_eof( pFile ) ? "End" : fread_word( pFile );
      fMatch = FALSE;

      switch ( UPPER( word[0] ) )
      {
      /* 註解 */
      case '*':
        fMatch = TRUE;
        fread_to_eol( pFile );
        break;

      case 'E':

        /* 結束 */
        if ( !str_scmp( word, "End", &fMatch ) )
        {
          Finish = TRUE;
          break;
        }

        break;

      case 'P':

        if ( !str_scmp( word, "Player", &fMatch ) )
        {
          set_database_default( pData = alloc_struct( STRUCT_DATABASE_DATA ) );

          pData->name        = str_dup( fread_word( pFile ) );
          pData->serial.high = fread_number( pFile );
          pData->serial.low  = fread_number( pFile );
          pData->exist       = fread_number( pFile );

          if ( !pData->exist )
          {
            iHash = pData->serial.low % MAX_KEY_HASH;
            for ( zData = data_index_hash[iHash]; zData; zData = zData->hash )
            {
              if ( zData->serial.low == pData->serial.low
                && zData->serial.high  == pData->serial.high
                && zData->exist )
              {
                zData->exist = FALSE;
                break;
              }
            }

            free_string( pData->name );
            free_struct( pData, STRUCT_DATABASE_DATA );
          }

          else
          {
            if ( !database_list ) database_list       = pData;
            if ( database_last  ) database_last->next = pData;

            database_last = pData;
            pData->next   = NULL;

            iHash                  = pData->serial.low % MAX_KEY_HASH;
            pData->hash            = data_index_hash[iHash];
            data_index_hash[iHash] = pData;

            count++;
          }

          break;
        }

        break;
      }

      /* 指令型態錯誤 */
      if ( !fMatch )
        mudlog( LOG_DEBUG , "Load_database﹕命令 %s 不正確。" , word );

      if ( Finish ) break;
    }

    mudlog( LOG_INFO, "系統載入 %d 個資料庫資料。" , count );
    f_close( pFile );
  }

  else
  {
    mudlog( LOG_INFO, "沒有載入任何資料庫的資料。" );
  }

  RETURN_NULL();
}


/* 載入贈與資料 */
void load_donate( const char * filename )
{
  FILE_DATA * pFile;
  char * word;
  bool   fMatch;
  bool   Finish = FALSE;

  PUSH_FUNCTION( "load_donate" );

  if ( ( pFile = f_open( filename , "r" ) ) )
  {
    while ( !if_eof( pFile ) )
    {
      word   = if_eof( pFile ) ? "End" : fread_word( pFile );
      fMatch = FALSE;

      switch ( UPPER( word[0] ) )
      {
      /* 註解 */
      case '*':
        fMatch = TRUE;
        fread_to_eol( pFile );
        break;

      case 'B':
        KEY( "Benifit", DonateBenifit, fread_number( pFile ) );
        break;

      case 'C':
        KEY( "Count", DonateCount, fread_number( pFile ) );
        break;

      case 'D':
        KEY( "Duration", DonateDuration, fread_number( pFile ) );
        break;

      case 'E':

        /* 結束 */
        if ( !str_scmp( word, "End", &fMatch ) )
        {
          if ( DonateBenifit < 0 )
            mudlog( LOG_ERR, "Load_donate﹕救濟金不合法。" );

          if ( DonateMoney < 0 )
            mudlog( LOG_ERR, "Load_donate﹕救濟金總金額不合法。" );

          if ( DonateLevel < 0 )
            mudlog( LOG_ERR, "Load_donate﹕救濟等級不合法。" );

          if ( DonateLimit < 0 )
            mudlog( LOG_ERR, "Load_donate﹕救濟對象不合法。" );

          if ( DonateDuration < 0 )
            mudlog( LOG_ERR, "Load_donate﹕救濟間隔不合法。" );

          if ( DonateCount < 0 )
            mudlog( LOG_ERR, "Load_donate﹕救濟次數不合法。" );

          DonateLock = FALSE;
          Finish     = TRUE;
          f_close( pFile );
          break;
        }

        break;

      case 'L':
        KEY( "Level", DonateLevel, fread_number( pFile ) );
        KEY( "Limit", DonateLimit, fread_number( pFile ) );
        break;

      case 'M':
        KEY( "Money", DonateMoney, fread_number( pFile ) );
        break;
      }

      /* 指令型態錯誤 */
      if ( !fMatch )
        mudlog( LOG_DEBUG , "Load_donate﹕命令 %s 不正確。", word );

      if ( Finish ) break;
    }

    f_close( pFile );
  }

  else
  {
    mudlog( LOG_INFO, "沒有載入任何贈與的資料。" );
    DonateLock = TRUE;
  }

  RETURN_NULL();
}

/* 讀取幫派資料 */
void load_club( const char * filename )
{
  CLUB_DATA * pClub;
  CLUB_DATA * zClub;
  FILE_DATA * pFile;
  char      * word;
  bool        fMatch;
  bool        Finish = FALSE;
  int         loop;

  PUSH_FUNCTION( "load_club" );

  if ( ( pFile = f_open( filename , "r" ) ) )
  {
    while ( !if_eof( pFile ) )
    {
      word   = if_eof( pFile ) ? "End" : fread_word( pFile );
      fMatch = FALSE;

      switch ( UPPER( word[0] ) )
      {
      /* 註解 */
      case '*':
        fMatch = TRUE;
        fread_to_eol( pFile );
        break;

      case '#':

        if ( !str_scmp( word, "#CLUB", &fMatch ) )
        {
          if ( club_count() >= max_club )
            mudlog( LOG_ERR, "Load_club﹕幫派數目超出限定值。" );

          pClub = load_a_club( pFile );

          /* 若幫派連署超過 20 天﹐或是沒有幫主則刪除之 */
          if ( ( pClub->status != CLUB_STATUS_UNIONIZE
            && pClub->timer < ( ( time( NULL ) - ClubCountersignDay ) * 86400 ) )
            || !is_exist( pClub->master ) )
          {
            free_string( pClub->name       );
            free_string( pClub->cname      );
            free_string( pClub->master     );
            free_string( pClub->vicemaster );

            for ( loop = 0; loop < MAX_CLUB_DOYEN; loop++ )
              free_string( pClub->doyen[loop] );

            for ( loop = 0; loop < MAX_CLUB_MEMBER; loop++ )
              free_string( pClub->member[loop] );

            free_struct( pClub, STRUCT_CLUB_DATA );
          }

          else
          {
            for ( zClub = club_list;
              zClub && zClub->next;
              zClub = zClub->next );

            if ( !zClub ) club_list   = pClub;
            else          zClub->next = pClub;

            /* 建立起幫派房間 */
            club_location( pClub );
          }

          break;
        }

        break;

      case 'E':

        /* 結束 */
        if ( !str_scmp( word, "End", &fMatch ) )
        {
          Finish = TRUE;
          break;
        }
        break;
      }

      /* 指令型態錯誤 */
      if ( !fMatch ) mudlog( LOG_DEBUG , "Load_club﹕命令 %s 不正確。" , word );
      if ( Finish ) break;
    }

    f_close( pFile );

    for ( pClub = club_list; pClub; pClub = pClub->next ) club_sort( pClub );

    save_club();
  }

  else
  {
    mudlog( LOG_INFO, "沒有載入任何幫派的資料。" );
  }

  RETURN_NULL();
}

/* 讀取一個幫派資料 */
CLUB_DATA * load_a_club( FILE_DATA * pFile )
{
  CLUB_DATA * pClub;
  char      * word;
  char      * pName;
  bool        fMatch;
  int         status;

  PUSH_FUNCTION( "load_a_club" );

  set_club_default( pClub = alloc_struct( STRUCT_CLUB_DATA ) );

  for ( ;; )
  {
    word   = if_eof( pFile ) ? "End" : fread_word( pFile );
    fMatch = FALSE;

    switch ( UPPER( word[0] ) )
    {
    case '*':

      fMatch = TRUE;
      fread_to_eol( pFile );
      break;

    case 'C':
      KEY( "Cname", pClub->cname, fread_string( pFile ) );
      break;

    case 'D':

      if ( !str_scmp( word, "Doyen", &fMatch ) )
      {
        pName = fread_string( pFile );

        /* 預防玩家名稱太長 */
        if ( str_len( pName ) <= NAME_LENGTH && is_exist( pName ) )
          char_to_club( pName, pClub, CLUB_DOYEN );

        free_string( pName );
        break;
      }
      break;

    case 'E':

      if ( !str_scmp( word, "End", &fMatch ) )
      {
        if ( !pClub->name )
          mudlog( LOG_ERR, "Load_a_club﹕沒有英文幫派名稱。" );

        if ( !pClub->cname )
          mudlog( LOG_ERR, "Load_a_club﹕沒有幫派中文名稱。" );

        if ( !pClub->master )
          mudlog( LOG_ERR, "Load_a_club﹕沒有幫派主人名稱。" );

        if ( pClub->status == CLUB_STATUS_UNKNOW )
          mudlog( LOG_ERR, "Load_a_club﹕狀態 %d 不合理。", pClub->status );

        RETURN( pClub );
      }

      break;

    case 'F':

      if ( !str_scmp( word, "Follower", &fMatch ) )
      {
        pName = fread_string( pFile );

        /* 預防玩家名稱太長 */
        if ( str_len( pName ) <= NAME_LENGTH && is_exist( pName ) )
          char_to_club( pName, pClub, CLUB_FOLLOWER );

        free_string( pName );
        break;
      }

      break;

    case 'M':

      if ( !str_scmp( word, "Member", &fMatch ) )
      {
        pName = fread_string( pFile );

        /* 預防玩家名稱太長 */
        if ( str_len( pName ) <= NAME_LENGTH && is_exist( pName ) )
          char_to_club( pName, pClub, CLUB_MEMBER );

        free_string( pName );
        break;
      }

      KEY( "Master", pClub->master, fread_string( pFile ) );
      KEY( "Money",  pClub->money,  fread_number( pFile ) );
      break;

    case 'N':
      KEY( "Name", pClub->name, fread_string( pFile ) );
      break;

    case 'S':

      if ( !str_scmp( word, "Status", &fMatch ) )
      {
        switch( status = fread_number( pFile ) )
        {
        default:
          mudlog( LOG_DEBUG , "Load_a_club﹕狀態 %d 不正確。" , status );
          break;

        case CLUB_STATUS_COUNTERSIGN:
        case CLUB_STATUS_UNIONIZE:
          break;
        }

        pClub->status = status;
      }
      break;

    case 'T':
      KEY( "Timer" , pClub->timer , fread_number( pFile ) );
      break;

    case 'V':

      if ( !str_scmp( word, "ViceMaster", &fMatch ) )
      {
        pName = fread_string( pFile );

        /* 預防玩家名稱太長 */
        if ( str_len( pName ) <= NAME_LENGTH && is_exist( pName ) )
          pClub->vicemaster = str_dup( pName );

        free_string( pName );
        break;
      }

      break;
    }

    /* 指令型態錯誤 */
    if ( !fMatch )
      mudlog( LOG_DEBUG , "Load_a_club﹕命令 %s 不正確。" , word );
  }

  RETURN( NULL );
}

/* 載入問路資料 */
ENQUIRE_DATA * load_enquire( FILE_DATA * pFile )
{
  ENQUIRE_DATA * pEnquire;
  char         * word;
  bool           fMatch;

  PUSH_FUNCTION( "load_enquire" );

  /* 配置記憶體以及設定預設值 */
  set_enquire_default( pEnquire = alloc_struct( STRUCT_ENQUIRE_DATA ) );

  for ( ;; )
  {
    word   = if_eof( pFile ) ? "End" : fread_word( pFile );
    fMatch = FALSE;

    switch ( UPPER( word[0] ) )
    {
    /* 註解 */
    case '*':
      fMatch = TRUE;
      fread_to_eol( pFile );
      break;

    case 'E':

      if ( !str_scmp( word, "End", &fMatch ) )
      {
        if ( !pEnquire->keyword )
          mudlog( LOG_DEBUG, "Load_enquire﹕沒有設定問路的關鍵字。" );

        RETURN( pEnquire );
      }

      break;

    case 'K':

      KEY( "Keyword", pEnquire->keyword, fread_string( pFile ) );
      break;
    }

    /* 指令型態錯誤 */
    if ( !fMatch ) mudlog( LOG_DEBUG , "Load_pEnquire﹕命令 %s 不正確。", word );
  }

  RETURN( pEnquire );
}

/* 載入觸發動作 */
JOB_DATA * load_job( FILE_DATA * pFile )
{
  JOB_DATA * pJob;
  char     * word;
  bool       fMatch;

  PUSH_FUNCTION( "load_job" );

  /* 配置記憶體以及設定預設值 */
  set_job_default( pJob = alloc_struct( STRUCT_JOB_DATA ) );

  for ( ;; )
  {
    word   = if_eof( pFile ) ? "End" : fread_word( pFile );
    fMatch = FALSE;

    switch ( UPPER( word[0] ) )
    {
    /* 註解 */
    case '*':
      fMatch = TRUE;
      fread_to_eol( pFile );
      break;

    case 'E':

      if ( !str_scmp( word, "End", &fMatch ) )
      {
        if ( !pJob->keyword )
        {
          mudlog( LOG_DEBUG, "Load_job﹕沒有設定觸發的本文。" );
          break;
        }

        if ( !pJob->function )
        {
          mudlog( LOG_DEBUG, "Load_job﹕沒有設定觸發的函數。" );
          break;
        }

        if ( IS_ERROR( pJob->position ) )
        {
          mudlog( LOG_DEBUG, "Load_job﹕沒有設定觸發的狀態。" );
          break;
        }

        RETURN( pJob );
      }
      break;

    case 'F':

      if ( !str_scmp( word, "Function", &fMatch ) )
      {
        char * func;

        func = fread_string( pFile );
        TEST_READ_ERROR

        if ( !( pJob->function  = job_lookup( func ) ) )
        {
          mudlog( LOG_DEBUG, "Load_job﹕找不到操作函數 %s。", func );
          free_string( func );
          break;
        }

        free_string( func );
        break;
      }

    case 'K':

      KEY( "Keyword", pJob->keyword, fread_string( pFile ) );
      break;

    case 'P':

      if ( !str_scmp( word, "Position", &fMatch ) )
      {
        pJob->position = fread_number( pFile );
        TEST_READ_ERROR

        switch( pJob->position )
        {
        default:
          mudlog( LOG_DEBUG, "Load_job﹕錯誤的狀態 %d。", pJob->position );
          break;

        case POS_DEAD:
        case POS_SLEEPING:
        case POS_RESTING:
        case POS_FIGHTING:
        case POS_STANDING:
          break;
        }

        break;
      }

      break;
    }

    /* 指令型態錯誤 */
    if ( !fMatch || FileError )
    {
      if ( !fMatch ) mudlog( LOG_DEBUG , "Load_job﹕命令 %s 不正確。", word );
      if ( fReload )
      {
        free_job( pJob );
        RETURN( NULL );
      }
    }
  }

  RETURN( NULL );
}

MPROG_DATA * load_program( FILE_DATA * pFile )
{
  MPROG_DATA * pProg;
  char         buf[MAX_STRING_LENGTH];
  char       * word;
  char       * pString;
  bool         fMatch;
  int          chance;

  PUSH_FUNCTION( "load_program" );

  /* 配置記憶體以及設定預設值 */
  set_mprog_default( pProg = alloc_struct( STRUCT_MPROG_DATA ) );

  for ( ;; )
  {
    word   = if_eof( pFile ) ? "End" : fread_word( pFile );
    fMatch = FALSE;

    switch ( UPPER( word[0] ) )
    {
    /* 註解 */
    case '*':
      fMatch = TRUE;
      fread_to_eol( pFile );
      break;

    case 'A':

      if ( !str_scmp( word, "Argument", &fMatch ) )
      {
        pProg->arglist = fread_string( pFile );
        break;
      }

      break;

    case 'C':

      if ( !str_scmp( word, "Chance", &fMatch ) )
      {
        chance = fread_number( pFile );
        TEST_READ_ERROR

        if ( chance <= 0 || chance > 100 )
        {
          mudlog( LOG_DEBUG, "Load_program﹕機率 %d 不合理。", chance );
          break;
        }

        sprintf( buf, "%d", chance );
        pProg->arglist = str_dup( buf );
        break;
      }

      break;

    case 'E':

      if ( !str_scmp( word, "End", &fMatch ) )
      {
        if ( IS_ERROR( pProg->type ) )
        {
          mudlog( LOG_DEBUG, "Load_program﹕缺乏程式的型態。" );
          break;
        }

        if ( !pProg->comlist || !*pProg->comlist )
        {
          mudlog( LOG_DEBUG, "Load_program﹕缺乏程式的本文部份。" );
          break;
        }

        if ( !pProg->arglist || !*pProg->arglist )
        {
          mudlog( LOG_DEBUG, "Load_program﹕缺乏程式的參數部份。" );
          break;
        }

        RETURN( pProg );
      }

      break;

    case 'G':

      if ( !str_scmp( word, "Gold", &fMatch ) )
      {
        chance = fread_number( pFile );
        TEST_READ_ERROR

        if ( chance < 0 )
        {
          mudlog( LOG_DEBUG, "Load_program﹕金錢 %d 不合理。", chance );
          break;
        }

        sprintf( buf, "%d", chance );
        pProg->arglist = str_dup( buf );
        break;
      }

      break;

    case 'T':

      if ( !str_scmp( word, "Type", &fMatch ) )
      {
        pString = fread_word( pFile );
        TEST_READ_ERROR

        if ( ( pProg->type = mprog_name_to_type( pString ) ) == ERROR_PROG )
        {
          mudlog( LOG_DEBUG, "Load_program﹕載入程式型態 %s 錯誤。", pString );
          break;
        }

        break;
      }

      if ( !str_scmp( word, "Text", &fMatch ) )
      {
        fread_to_eol( pFile ); /* 一定先讀完一行 */
        pProg->comlist = fread_string( pFile );
        break;
      }

      break;
    }

    /* 指令型態錯誤 */
    if ( !fMatch || FileError )
    {
      if ( !fMatch ) mudlog( LOG_DEBUG , "Load_prog﹕命令 %s 不正確。", word );
      if ( fReload )
      {
        free_mob_program( pProg );
        RETURN( NULL );
      }
    }
  }

  RETURN( NULL );
}

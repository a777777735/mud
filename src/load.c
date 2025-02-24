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
*  �o�O�ѻ��j�ƾǨt�s�@�s�Ҽ��g���C���M�D��� merc ��s�ӨӡM�Ҧ������v    *
*  �N�|�Q�O�d�M���w��j�a�ק�M���ڭ̤]�Ʊ�A�̤]�ണ�ѵ��j�a�M�Ҧ�����    *
*  �~�欰�N���Q���\�C                                                      *
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


/* �~���ܼ� */
extern SYMBOL_DATA *      symbol_hash[26][20];
extern bool               fReload;
extern unsigned int       FileError;

/* �~����� */
extern void    mprog_read_programs     args( ( FILE_DATA * , MOB_INDEX_DATA * ) );
extern void    set_exit_default        args( ( EXIT_DATA * ) );
extern int     glob_file        args( ( const char *, const char *, glob_t * ) );

/* �ϰ��� */
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

/* �]�w�ϰ��ɤ����ϰ쪺�w�]�� */
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

/* �]�w�ϰ��ɤ����Ǫ����w�]�� */
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

/* �]�w�ϰ��ɤ����Ǫ��оɪ��w�]�� */
void set_teach_default( TEACH_DATA * teach )
{
  PUSH_FUNCTION( "set_teach_default" );
  teach->slot      = ERRORCODE;
  teach->cost      =  0;
  teach->adept     = 20;
  teach->inventory =  0;
  RETURN_NULL();
}

/* �]�w�ϰ��ɤ����ж����w�]�� */
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

/* �Ʃw�ж��X�f���w�]�� */
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

/* �]�w���~���w�]�� */
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

/* �]�w���~�ݩʪ��w�]�� */
void set_affect_default( AFFECT_DATA * pAffect )
{
  PUSH_FUNCTION( "set_affect_default" );

  pAffect->type        = ERRORCODE;
  pAffect->duration    = ERRORCODE;
  pAffect->location    = ERRORCODE;
  pAffect->modifier    = 0;
  RETURN_NULL();
}

/* �]�w�ө����w�w�� */
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

/* �]�w�a�K��ƪ��w�]�� */
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
/* �]�w���ڳs�u���w�]�� */
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

/* �M����������� */
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

  /* �M����������� */
  for ( loop = 0; loop < MAX_CLUB_MEMBER; loop++ )
  {
    pClub->verify[loop] = MEMBER_NO_VERIFY;
    pClub->member[loop] = NULL;
  }

  /* �M�����Ѫ���� */
  for ( loop = 0; loop < MAX_CLUB_DOYEN; loop++ )
    pClub->doyen[loop] = NULL;

  /* �M�����w����� */
  for ( loop = 0; loop < MAX_LEVEL; loop++ )
  {
    for ( loop_2 = 0; loop_2 < MAX_STAFF; loop_2++ )
    {
      pClub->staff[loop][loop_2] = NULL;
    }
  }

  RETURN_NULL();
}

/* �]�w���~����c��� */
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

  else mudlog( LOG_DEBUG, "set_restrict_default�R�ǤJ���c���s�b�C" );

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
    mudlog( LOG_DEBUG, "set_vote_default�R�ʥF�ӷ��C" );
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

/* ���J�s�榡���ϰ��ɪ��ϰ�榡 */
AREA_DATA * load_area( char * filename )
{
  AREA_DATA * pArea;
  AREA_DATA * zArea;
  FILE_DATA * pFile;
  char      * word;
  bool        fMatch;

  PUSH_FUNCTION( "load_area" );

  if ( !( pFile = f_open( filename , "r" ) ) ) RETURN( NULL );

  /* �t�m�O����H�γ]�w�w�]�� */
  set_area_default( pArea = alloc_struct( STRUCT_AREA_DATA ) );

  /* ���J�ϰ쪺�D�{������ */
  for ( ;; )
  {
    word   = if_eof( pFile ) ? "End" : fread_word( pFile );
    fMatch = FALSE;

    switch ( UPPER(word[0]) )
    {
    /* ���� */
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
        fread_to_eol( pFile ); /* �@�w��Ū���@�� */
        pArea->description = fread_string( pFile );
        break;
      }

      break;

    case 'E':

      if ( !str_scmp( word, "End", &fMatch ) )
      {
        /* �ˬd�O�_�����~ */
        if ( !pArea->name )
        {
          mudlog( LOG_DEBUG , "Load_area�R�ɮ� %s �ʥF�ϰ�W�r�C", filename );
          break;
        }

        if ( !pArea->editor )
        {
          mudlog( LOG_DEBUG , "Load_area�R�ɮ� %s �ʥF�ϰ켶�g�̡C", filename );
          break;
        }

        if ( IS_ERROR( pArea->serial ) )
        {
          mudlog( LOG_DEBUG, "Load_area�R�ϰ� %s �ʥF�Ǹ��C", pArea->name );
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
             mudlog( LOG_DEBUG, "Load_area�R�ϰ�Ǹ� %d ���ơC", serial );
        }

        pArea->serial = serial;
        break;
      }
      break;

    case 'U':

      KEY( "Upper",   pArea->upper , fread_number( pFile ) );
      break;
    }

    /* ���O���A���~ */
    if ( !fMatch )
      mudlog( LOG_DEBUG , "Load_Area�R�ɮשR�O %s ���~�C" , word );
  }
  RETURN( NULL );
}

/* ���J�s�榡���ϰ��ɪ��Ǫ��榡 */
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
    mudlog( LOG_FAILLOAD , "Load_mobiles�R���J�Ǫ��ɮ� %s ���ѡC\n" , filename );
    RETURN( NULL );
  }

  /* �w�]�@�Ǽƭ� */
  set_mobile_default( pMobIndex = alloc_struct( STRUCT_MOB_INDEX_DATA ) );

  for ( ;; )
  {
    word   = if_eof( pFile ) ? "End" : fread_word( pFile );
    fMatch = FALSE;

    switch ( UPPER(word[0]) )
    {
    /* ���� */
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
          mudlog( LOG_DEBUG, "Load_mobile�R���J�Ǫ��S���ƥ��ѡC" );
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
          mudlog( LOG_DEBUG, "Load_mobile�R���J�Ǫ��оɥ��ѡC" );
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
        fread_to_eol( pFile ); /* �@�w��Ū���@�� */
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

        /* �p�G�ޯ�w�g���s�b */
        word = fread_word( pFile );
        TEST_READ_ERROR

        if ( !( pSkill = skill_isname( word ) ) )
        {
          mudlog( LOG_DEBUG , "Load_mobiles�R�Ǫ��ޯ�P����~�C" );
          break;
        }

        /* �ݬݳo�ӧޯ�O�_�i�H�Q�P�� */
        if ( !pSkill->enable )
        {
          mudlog( LOG_DEBUG, "Load_mobiles�R�Ǫ��P�� %s �O����P�઺�C"
            , pSkill->cname );
          break;
        }

        /* �p�G���� */
        if ( ( enable_repeat( pMobIndex->enable , pSkill ) ) )
        {
          mudlog( LOG_DEBUG , "Load_mobiles�R�Ǫ��ޯ�P�୫�ơC" );
          break;
        }

        if ( ( practice = get_adeptation( pSkill, pMobIndex ) ) < 0 )
        {
          mudlog( LOG_DEBUG , "Load_mobiles�R�ޯ� %s �L�k�۰ʭP��C"
            , pSkill->name );
          break;
        }

        if ( practice >= 100 )
        {
          mudlog( LOG_FAILENABLE, "�Ǫ��s�� %d �ޯ� %s �Ӯt�M��ĳ�����C"
            , pMobIndex->vnum, pSkill->name );

          practice = 100;
        }

        if ( practice <= 1 )
        {
          mudlog( LOG_FAILENABLE, "�Ǫ��s�� %d �ޯ� %s �Ӱ��M��ĳ�����C"
            , pMobIndex->vnum, pSkill->name );

          practice = 1;
        }

        /* �]�w���m�� */
        pMobIndex->skill[pSkill->slot] = practice;

        /* �p�G�s�b�~�t�m�O���鵹�L */
        pEnable        = alloc_struct( STRUCT_ENABLE_DATA );
        pEnable->skill = pSkill;
        pEnable->next  = NULL;

        /* ��X�̫�@�� */
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
          mudlog( LOG_DEBUG, "Load_mobiles�R���X %d ���Ǫ�¾�~ %d �q�ʡC"
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
        fread_to_eol( pFile ); /* �@�w��Ū���@�� */
        pMobIndex->description = fread_string( pFile );
        break;
      }

      if ( !str_scmp( word , "Deadmsg", &fMatch ) )
      {
        fread_to_eol( pFile ); /* �@�w��Ū���@�� */
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

        /* ��Ū�J���m�� */
        practice = fread_number( pFile );
        TEST_READ_ERROR

        if ( practice > 100 || practice <= 0 )
        {
          mudlog( LOG_DEBUG, "Load_mobiles�R�Ǫ��ޯ���m�צ����D�C" );
          break;
        }

        /* �p�G�ޯ�w�g���s�b */
        word = fread_word( pFile );
        TEST_READ_ERROR

        if ( !( pSkill = skill_isname( word ) ) )
        {
          mudlog( LOG_DEBUG , "Load_mobiles�R�Ǫ��ޯ�P����~�C" );
          break;
        }

        /* �ݬݳo�ӧޯ�O�_�i�H�Q�P�� */
        if ( !pSkill->enable )
        {
          mudlog( LOG_DEBUG, "Load_mobiles�R�Ǫ��P�� %s �O����P�઺�C"
            , pSkill->cname );
          break;
        }

        /* �p�G���� */
        if ( ( enable_repeat( pMobIndex->enable , pSkill ) ) )
        {
          mudlog( LOG_DEBUG , "Load_mobiles�R�Ǫ��ޯ�P�୫�ơC" );
          break;
        }

        /* �]�w���m�� */
        pMobIndex->skill[pSkill->slot] = practice;

        /* �p�G�s�b�~�t�m�O���鵹�L */
        pEnable        = alloc_struct( STRUCT_ENABLE_DATA );
        pEnable->skill = pSkill;
        pEnable->next  = NULL;

        /* ��X�̫�@�� */
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
          mudlog( LOG_DEBUG, "Load_mobiles�R�Ǫ� %d �S���ޯ� %s�C"
            , pMobIndex->vnum, name );
          break;
        }

        if ( !pSkill->affected )
        {
          mudlog( LOG_DEBUG, "Load_mobiles�R�Ǫ� %d �ޯ� %s ���ন�����C"
            , pMobIndex->vnum, pSkill->name );
          break;
        }

        for ( pAffect = pMobIndex->affected; pAffect; pAffect = pAffect->next )
        {
          if ( pAffect->type == pSkill->slot )
          {
            mudlog( LOG_DEBUG, "Load_mobiles�R�Ǫ� %d ���� %s ���СC"
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

        /* ��X�̫�@�� */
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
          mudlog( LOG_DEBUG , "Load_mobiles�R�Ǫ��S�����X�C" );
          break;
        }

        if ( !pMobIndex->name )
        {
          mudlog( LOG_DEBUG , "Load_mobiles�R�Ǫ� %d �S���W�r�C", vnum );
          break;
        }

        if ( !pMobIndex->short_descr )
        {
          mudlog( LOG_DEBUG , "Load_mobiles�R�Ǫ� %d �S���u�y�z�C", vnum );
          break;
        }

        if ( !pMobIndex->description )
        {
          mudlog( LOG_DEBUG , "Load_mobiles�R�Ǫ� %d �S���y�z�C", vnum );
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
          mudlog( LOG_DEBUG , "Load_mobiles�R�Ǫ� %d ���� %d ���X�z�C"
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
          mudlog( LOG_DEBUG, "Load_mobile�R���J�Ǫ��{�����~�C" );
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
          mudlog( LOG_DEBUG, "Load_mobiles�R�Ǫ��ʧO %d ���X�z�C", pMobIndex->sex );
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
          mudlog( LOG_DEBUG , "Load_mobiles�R�S���� %s ���s�b�C", word );
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

    /* ���O���A���~ */
    if ( !fMatch || FileError )
    {
      if ( !fMatch ) mudlog( LOG_DEBUG , "Load_mobile�R�R�O %s �����T�C" , word );
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

/* ���J�Ǫ��оɵ��c */
TEACH_DATA * load_mob_teach( FILE_DATA * pFile )
{
  TEACH_DATA * teach;
  char       * word;
  bool         fMatch;

  PUSH_FUNCTION( "load_mob_teach" );

  /* �t�m�оɩһݭn���O����H�γ]�w�w�]�� */
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
          mudlog( LOG_DEBUG, "Load_mob_teach�R�k�N���m�� %d ���X�z�C"
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
          mudlog( LOG_DEBUG, "Load_mob_teach�R�k�N�бª� %d ���X�z�C"
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
          mudlog( LOG_DEBUG, "Load_mob_teach�R�S���k�N���X�C" );
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
          mudlog( LOG_DEBUG, "Load_mob_teach�R�k�N���úX�Ф���C" );
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
          mudlog( LOG_DEBUG , "Load_mob_teach�R�k�N %s ����C", name_string );
          break;
        }

        teach->slot = pSkill->slot;
        break;
      }
      break;
    }

    /* ���O���A���~ */
    if ( !fMatch || FileError )
    {
      if ( !fMatch ) mudlog( LOG_DEBUG , "Load_mob_teach�R�R�O %s �����T�C" , word );
      if ( fReload )
      {
        free_teach( teach );
        RETURN( NULL );
      }
    }
  }

  RETURN( NULL );
}

/* ���J�s�榡���ϰ��ɪ��ж��榡 */
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
    mudlog( LOG_FAILLOAD , "���J�ж��ɮ� %s ���ѡC\n" , filename );
    RETURN( NULL );
  }

  /* �t�m�ж��Ŷ��H�γ]�w�w�]�� */
  set_room_default( pRoomIndex = alloc_struct( STRUCT_ROOM_INDEX_DATA ) );

  /* �������J�ϰ��ɤ����ж� */
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
        /* �קK�S���ж����X�N���J�X�f */
        if ( IS_ERROR( pRoomIndex->vnum ) )
          mudlog( LOG_DEBUG , "Load_room�R�ж����X�٨S�]�w, �L�k�]�w�X�f�C" );

        /* ���J�ж��X�f�榡 */
        pExit = load_exit( pFile );

        pRoomIndex->exit[pExit->direction] = pExit;
        if ( !fReload ) top_exit++;

        break;
      }

      /* ���J�ж�������r���� */
      if ( !str_scmp( word , "#Keyword", &fMatch ) )
      {
        /* �קK�S���ж����X�N���J����r */
        if ( IS_ERROR( pRoomIndex->vnum ) )
          mudlog( LOG_DEBUG , "Load_room�R�ж����X�٨S�]�w, �L�k�]�w����r�C" );

        if ( !( pExtra = load_extra_descr( pFile ) ) )
        {
          mudlog( LOG_DEBUG, "Load_room�R�ж��B�~�y�z���J���ѡC" );
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
          mudlog( LOG_DEBUG, "Load_room�R���J�ж��S���ƥ��ѡC" );
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
        fread_to_eol( pFile ); /* �@�w��Ū���@�� */
        pRoomIndex->description = fread_string( pFile );
        break;
      }
      break;

    case 'E':

      if ( !str_scmp( word, "End", &fMatch ) )
      {
        /* �קK���ǭ��n�ȩ|�����J�N���� */
        if ( !pRoomIndex->name )
          mudlog( LOG_DEBUG , "Load_room�R�ж��S���W�١C" );

        if ( !pRoomIndex->description )
          mudlog( LOG_DEBUG , "Load_room�R�ж��S���y�z�C" );

        if ( IS_ERROR( pRoomIndex->vnum ) )
          mudlog( LOG_DEBUG , "Load_room�R�ж��S�����X�C" );

        if ( !pRoomIndex->sector )
          mudlog( LOG_DEBUG , "Load_room�R�ж��S���a�ΡC" );

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
          mudlog( LOG_DEBUG , "Load_room�R�ж��a���s�� %d ���s�b�C" , iSector );

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

        /* �קK�ж����X�|������ */
        if ( get_room_index( pRoomIndex->vnum ) )
          mudlog( LOG_DEBUG , "Load_room�R�ж����X %d ���ơC"
            , pRoomIndex->vnum );

        break;
      }

      break;
    }

    /* ���O���A���~ */

    if ( !fMatch )
      mudlog( LOG_DEBUG , "Load_room�R�R�O %s �����T�C" , word );
  }
}

/* ���J�ж����X�f */
EXIT_DATA * load_exit( FILE_DATA * pFile )
{
  EXIT_DATA * pExit;
  char      * word;
  bool        fMatch;
  int         direction;

  PUSH_FUNCTION( "load_exit" );

  /* �t�m�O����H�γ]�w�w�]�� */
  set_exit_default( pExit = alloc_struct( STRUCT_EXIT_DATA ) );

  /* ���J�ϰ��ɩж��X�f���� */
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
          mudlog( LOG_DEBUG, "Load_exit�R���~����V %d�C", direction );
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
        /* �קK���ǭ��n�ȩ|��Ū���N���} */
        if ( IS_ERROR( direction ) )
          mudlog( LOG_DEBUG , "Load_exit�R�X�f��V�S���]�w�C" );

        if ( IS_ERROR( pExit->vnum ) )
          mudlog( LOG_DEBUG , "Load_exit�R�ж��X�f�S�����X�C" );

        if ( !pExit->description )
          mudlog( LOG_DEBUG , "Load_exit�R�ж��X�f�S���y�z�C" );

        if ( !pExit->keyword )
          mudlog( LOG_DEBUG , "Load_exit�R�ж��X�f�S������r�C" );

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

    /* ���O���A���~ */
    if ( !fMatch )
      mudlog( LOG_DEBUG , "Load_exit�R�R�O %s �����T�C" , word );
  }

  RETURN( NULL );
}

/* ���J�ж�������r */
EXTRA_DESCR_DATA * load_extra_descr( FILE_DATA * pFile )
{
  EXTRA_DESCR_DATA * ed;
  char             * word;
  bool               fMatch;

  PUSH_FUNCTION( "load_extra_descr" );

  /* �t�m�O���� */
  ed = alloc_struct( STRUCT_EXTRA_DESCR_DATA );

  /* �]�w�w�]�� */
  ed->keyword     = NULL;
  ed->description = NULL;

  /*���J�ϰ��ɩж�������r���� */

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
        fread_to_eol( pFile ); /* �@�w��Ū���@�� */
        ed->description = fread_string( pFile );
        break;
      }

      break;

    case 'E':

      if ( !str_scmp( word, "End", &fMatch ) )
      {
        if ( !ed->description )
        {
          mudlog( LOG_DEBUG , "Load_extra_descr�R�S���y�z�C" );
          break;
        }

        if ( !ed->keyword )
        {
          mudlog( LOG_DEBUG , "Load_extra_descr�R�y�z�S������r�C" );
          break;
        }

        RETURN( ed );
      }

      break;

    case 'K':

      KEY( "Keyword", ed->keyword , fread_string( pFile ) );
      break;
    }

    /* ���O���A���~ */
    if ( !fMatch || FileError )
    {
      if ( !fMatch ) mudlog( LOG_DEBUG , "Load_extra_descr�R�R�O %s �����T�C" , word );
      if ( FileError )
      {
        free_extra_descr( ed );
        RETURN( NULL );
      }
    }
  }

  RETURN( NULL );
}

/* ���J�s�榡���ө��榡 */
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
    mudlog( LOG_FAILLOAD , "���J�ө��ɮ� %s ���ѡC\n" , filename );
    RETURN( NULL );
  }

  /* �t�m�ө��һݭn���O����H�γ]�w�w�]�� */
  set_shop_default( pShop = alloc_struct( STRUCT_SHOP_DATA ) );

  /* ���J�ϰ��ɸ̭����ө����� */
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
        /* �קK�S�����������~ */
        if ( IS_ERROR( pShop->keeper ) )
          mudlog( LOG_DEBUG , "Load_shop�R�S�����J���D�C" );

        if ( IS_ERROR( pShop->type ) )
          mudlog( LOG_DEBUG, "Load_shop�R�S�����J���a���A�C" );

        if ( !( pMobIndex = get_mob_index( pShop->keeper ) ) )
          mudlog( LOG_DEBUG, "Load_shop�R�S�����D���X %d�C", pShop->keeper );

        if ( pMobIndex->pShop )
          mudlog( LOG_DEBUG, "Load_shop�R�Ǫ� %d �w���ө����c�C"
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
              mudlog( LOG_DEBUG, "Load_shop�R�ħL���X %d ���s�b�C", slot );
          }

          break;

        case SHOP_COPER:

          for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
          {
            int slot;

            if ( ( slot = pShop->buy_type[iTrade] ) == 0 ) continue;

            if ( !( pVictim = get_mob_index( slot ) ) )
              mudlog( LOG_DEBUG, "Load_shop�R�y�M���X %d ���s�b�C", slot );

            if ( pVictim->tractable <= 0 )
              mudlog( LOG_DEBUG, "Load_shop�R���X %d ���O�y�M�C", slot );
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
          mudlog( LOG_DEBUG, "Load_shop�R�䤣�쩱���ж����X %d�C", vnum );

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
          mudlog( LOG_DEBUG, "Load_shop�R���A %d ���~�C", iTrade );
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

    /* ���O���A���~ */
    if ( !fMatch )
      mudlog( LOG_DEBUG , "Load_shop�R�R�O %s �����T�C" , word );
  }

  RETURN( NULL );
}

/* ���J�s�榡���q���榡 */
MINERAL_DATA * load_mineral( const char * filename )
{
  MINERAL_DATA * pMineral;
  FILE_DATA    * pFile;
  char         * word;
  bool           fMatch;

  PUSH_FUNCTION( "load_mineral" );

  if ( !( pFile = f_open( filename , "r" ) ) )
  {
    mudlog( LOG_FAILLOAD , "���J�q���ɮ� %s ���ѡC\n" , filename );
    RETURN( NULL );
  }

  /* �t�m�ө��һݭn���O����H�γ]�w�w�]�� */
  pMineral = alloc_struct( STRUCT_MINERAL_DATA );
  set_mineral_default( pMineral );

  /* ���J�ϰ��ɸ̭����ө����� */
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
          mudlog( LOG_DEBUG , "Load_mineral�R�S���ƶq�C" );

        if ( !pMineral->mineral )
          mudlog( LOG_DEBUG , "Load_mineral�R�S���q���C" );

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
          mudlog( LOG_DEBUG , "Load_mineral�R�q�����X %d ���s�b�C", slot );

        break;
      }

      break;
    }

    /* ���O���A���~ */
    if ( !fMatch )
      mudlog( LOG_DEBUG , "Load_mineral�R�R�O %s �����T�C" , word );
  }

  RETURN( NULL );
}

/* ���J���~ */
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
    mudlog( LOG_FAILLOAD , "���J���~�ɮ� %s ���ѡC\n" , filename );
    RETURN( NULL );
  }

  /* �t�m���~�һݭn���O����H�γ]�w�w�]�� */
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
        /* �קK�S�����~���X�N���J�ݩ� */
        if ( IS_ERROR( pObjIndex->vnum ) )
        {
          mudlog( LOG_DEBUG, "Load_object�R���~���X�٨S�]�w�M�L�k�]�w�ݩʡC" );
          break;
        }

        /* ���J���~�ݩʮ榡 */
        if ( !( pAffect = load_affect( pFile ) ) )
        {
          mudlog( LOG_DEBUG, "Load_object�R���J���~�v�T���ѡT" );
          break;
        }

        pAffect->next       = pObjIndex->affected;
        pObjIndex->affected = pAffect;
        break;
      }

      if ( !str_scmp( word , "#Impact", &fMatch ) )
      {
        /* �קK�S�����~���X�N���J�ݩ� */
        if ( IS_ERROR( pObjIndex->vnum ) )
        {
          mudlog( LOG_DEBUG, "Load_object�R���~���X�٨S�]�w�M�L�k�]�w�ݩʡC" );
          break;
        }

        /* ���J���~�ݩʮ榡 */
        if ( !( pAffect = load_affect( pFile ) ) )
        {
          mudlog( LOG_DEBUG, "Load_object�R���J���~�]�ۮ������ѡT" );
          break;
        }

        pAffect->next     = pObjIndex->impact;
        pObjIndex->impact = pAffect;
        break;
      }

      if ( !str_scmp( word, "#Restrict", &fMatch ) )
      {
        /* ���J���~�˳ƭ��� */
        if ( !( pRestrict = load_restrict( pFile ) ) )
        {
          mudlog( LOG_DEBUG, "Load_object�R���J���~����A���ѡT" );
          break;
        }

        for ( zRestrict = pObjIndex->restrict; zRestrict; zRestrict = zRestrict->next )
        {
          if ( zRestrict->type == pRestrict->type
            && zRestrict->type != RES_SKILL )
          {
            mudlog( LOG_DEBUG , "Load_object�R���~����A %d ���ơC"
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
        /* ���J���~�˳Ʊԭz */
        if ( !( pMessage = load_message( pFile ) ) )
        {
          mudlog( LOG_DEBUG, "Load_object�R���J���~�ԭz���ѡT" );
          break;
        }

        pMessage->next     = pObjIndex->message;
        pObjIndex->message = pMessage;
        break;
      }

      if ( !str_scmp( word , "#Extra", &fMatch ) )
      {
        /* �קK�S�����~���X�N���J�B�~�y�z */
        if ( IS_ERROR( pObjIndex->vnum ) )
        {
          mudlog( LOG_DEBUG ,
            "Load_object�R���~���X�٨S�]�w�M�L�k�]�w�B�~�y�z�C" );

          break;
        }

        /* ���J���~�ݩʮ榡 */
        if ( !( pExtra = load_extra_descr( pFile ) ) )
        {
          mudlog( LOG_DEBUG, "Load_object�R���J���~�B�~�y�z���ѡT" );
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
          mudlog( LOG_DEBUG, "Load_object�R���J���~�S���ƥ��ѡT" );
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
        fread_to_eol( pFile ); /* �@�w��Ū���@�� */
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
          mudlog( LOG_DEBUG , "Load_object�R���~�S�����X�C" );
          break;
        }

        if ( !pObjIndex->name )
        {
          mudlog( LOG_DEBUG , "Load_object�R���~ %d �S���W�r�C", vnum );
          break;
        }

        if ( !pObjIndex->short_descr )
        {
          mudlog( LOG_DEBUG , "Load_object�R���~ %d �S���u�ԭz�C", vnum );
          break;
        }

        if ( !pObjIndex->description )
        {
          mudlog( LOG_DEBUG , "Load_object�R���~ %d �S�����ԭz�C", vnum );
          break;
        }

        if ( IS_ERROR( pObjIndex->item_type ) )
        {
          mudlog( LOG_DEBUG , "Load_object�R���~ %d �S�����~���A�C", vnum );
          break;
        }

        if ( pObjIndex->weight < 0 )
        {
          mudlog( LOG_DEBUG, "Load_object�R���~ %d �S�����q�C", vnum );
          break;
        }

        switch( pObjIndex->item_type )
        {
        default:

          if ( pObjIndex->cost > pObjIndex->level * 20 )
            mudlog( LOG_INFO, "Load_object�R[���~�ӶQ] ���~ %d �����Ӱ��C", vnum );

          if ( pObjIndex->item_type == ITEM_MAGICSTONE && !pObjIndex->impact )
            mudlog( LOG_INFO, "Load_obj�R�]�� %d �S�����w�����C", vnum );

          if ( pObjIndex->item_type != ITEM_MAGICSTONE && pObjIndex->impact )
            mudlog( LOG_INFO, "Load_object�R�D�]�� %d �����w�����C", vnum );

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
                mudlog( LOG_DEBUG, "Load_object�R���~ %-5d ���� %-3d �W�["
                  "�I�� %4d �Ӧh�C", vnum, pObjIndex->level, paf->modifier );
              break;

            case APPLY_STR:
            case APPLY_INT:
            case APPLY_WIS:
            case APPLY_DEX:
            case APPLY_CON:
              if ( paf->modifier > 3 )
                mudlog( LOG_DEBUG, "Load_object�R���~ %-5d ���� %-3d �W�[��"
                  "�� %2d �Ӧh�C", vnum, pObjIndex->level, paf->modifier );
              count += paf->modifier;
              break;
            }
          }

          if ( count > 2 )
          {
            mudlog( LOG_DEBUG,
              "Load_object�R���~ %-5d ���� %-3d �W�[�ݩ� %2d �Ӧh�C"
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

        /* �ˬd���~�ƭ� */
        switch( pObjIndex->item_type )
        {
        default:
          break;

        case ITEM_ARROW:
          pObjIndex->value[0] = UMAX( 1, pObjIndex->value[0] );
          pObjIndex->value[1] = UMAX( 1, pObjIndex->value[1] );

         break;

        case ITEM_WEAPON:

          /* ��Z����ˤp�󵥩�s�ɪ��B�z */
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
            mudlog( LOG_DEBUG , "Load_object�R�I�k���~ %d �ƭȿ��~�C", vnum );
            break;

          /* ���ݭn�]�w�ޯู�X */
          case OBJ_CAST_HIT:
          case OBJ_CAST_MANA:
          case OBJ_CAST_MOVE:
          case OBJ_CAST_ALL:
            break;

          /* ���ݭn�]�w�ޯู�X */
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
              mudlog( LOG_DEBUG , "Load_object�R�I�k���~ %d �ޯ���~�C", vnum );

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
          mudlog( LOG_DEBUG , "Load_object�R���~���� %d ���~�C", pObjIndex->level );
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
        fread_to_eol( pFile ); /* �@�w��Ū���@�� */
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
        fread_to_eol( pFile ); /* �@�w��Ū���@�� */
        pObjIndex->wearmsg = fread_string( pFile );
        break;
      }

      break;

    }

    /* ���O���A���~ */
    if ( !fMatch || FileError )
    {
      if ( !fMatch ) mudlog( LOG_DEBUG , "Load_object�R�R�O %s �����T�C" , word );
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

/* ���J���~���v�T */
AFFECT_DATA * load_affect( FILE_DATA * pFile )
{
  AFFECT_DATA * pAffect;
  char        * word;
  bool          fMatch;

  PUSH_FUNCTION( "load_affect" );

  /* �t�m���~�ݩʪ��O����M�]�w�w�]�� */
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
          mudlog( LOG_DEBUG , "Load_affect�R�S���]�w�v�T�ݩʪ����A�C" );

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

    /* ���O���A���~ */
    if ( !fMatch || FileError )
    {
      if ( !fMatch ) mudlog( LOG_DEBUG , "Load_affect�R�R�O %s �����T�C", word );
      if ( fReload )
      {
        free_affect( pAffect );
        RETURN( NULL );
      }
    }
  }

  RETURN( NULL );
}

/* ���J���~�˳ơB�ޯध���� */
RESTRICT_DATA * load_restrict( FILE_DATA * pFile )
{
  RESTRICT_DATA * pRestrict;
  char          * word;
  bool            fMatch;

  PUSH_FUNCTION( "load_restrict" );

  /* �t�m���~�ݩʪ��O����M�]�w�w�]�� */
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
          mudlog( LOG_DEBUG, "Load_restrict�R����ʥF���A�C" );
          break;
        }

        if ( IS_ERROR( pRestrict->value ) )
        {
          mudlog( LOG_DEBUG, "Load_restrict�R����S���ƭȡC" );
          break;
        }

        switch( pRestrict->type )
        {
        case RES_STR:
          if ( pRestrict->value > MaxStr || pRestrict->value < 0 )
            mudlog( LOG_DEBUG, "Load_restrict�R�O�q�ƭ� %d ���X�z�C"
              , pRestrict->value );
           break;

        case RES_INT:
          if ( pRestrict->value > MaxInt || pRestrict->value < 0 )
            mudlog( LOG_DEBUG, "Load_restrict�R���O�ƭ� %d ���X�z�C"
              , pRestrict->value );
           break;

        case RES_WIS:
          if ( pRestrict->value > MaxWis || pRestrict->value < 0 )
            mudlog( LOG_DEBUG, "Load_restrict�R���Ѽƭ� %d ���X�z�C"
              , pRestrict->value );
           break;

        case RES_DEX:
          if ( pRestrict->value > MaxDex || pRestrict->value < 0 )
            mudlog( LOG_DEBUG, "Load_restrict�R�ӱ��ƭ� %d ���X�z�C"
              , pRestrict->value );
           break;

        case RES_CON:
          if ( pRestrict->value > MaxCon || pRestrict->value < 0 )
            mudlog( LOG_DEBUG, "Load_restrict�R���ƭ� %d ���X�z�C"
              , pRestrict->value );
           break;

        case RES_TRUST:
          if ( pRestrict->value > MAX_LEVEL || pRestrict->value < 0 )
            mudlog( LOG_DEBUG, "Load_restrict�R�H���ƭ� %d ���X�z�C"
              , pRestrict->value );
           break;

        case RES_SEX:

          switch( pRestrict->value )
          {
          default:
            mudlog( LOG_DEBUG, "Load_restrict�R�ʧO %d ���~�C"
              , pRestrict->value );
            break;

          case SEX_FEMALE:
          case SEX_MALE:
          case SEX_NEUTRAL:
            break;
          }

        case RES_LEVEL:
          if ( pRestrict->value > MAX_LEVEL || pRestrict->value < 0 )
            mudlog( LOG_DEBUG, "Load_restrict�R���żƭ� %d ���X�z�C"
              , pRestrict->value );

          break;

        case RES_ALIGN:

          if ( pRestrict->value < -1000 || pRestrict->value > 1000 )
            mudlog( LOG_DEBUG, "Load_restrict�R�}��ƭ� %d ���X�z�C"
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
          mudlog( LOG_DEBUG, "Load_restrict�R�ɾ� %d ���~�C"
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
          mudlog( LOG_DEBUG, "Load_restrict�R�S���o�اޯ� %s�C", pWord );
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
          mudlog( LOG_DEBUG, "Load_restrict�R����A %d ���~�C"
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

    /* ���O���A���~ */
    if ( !fMatch || FileError )
    {
      if ( !fMatch ) mudlog( LOG_DEBUG , "Load_restrict�R�R�O %s �����T�C", word );
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

  /* �t�m���~�ݩʪ��O����M�]�w�w�]�� */
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
          mudlog( LOG_DEBUG, "Load_message�R�ԭz�S���κA�C" );
          break;
        }

        if ( !pMessage->self )
        {
          mudlog( LOG_DEBUG, "Load_message�R�ԭz�S���ԭz�r��C" );
          break;
        }

        if ( !pMessage->others )
        {
          mudlog( LOG_DEBUG, "Load_message�R�ԭz�S���ԭz�r��C" );
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
          mudlog( LOG_DEBUG, "Load_message�R�ԭz���Щw�q�C" );
          break;
        }

        fread_to_eol( pFile ); /* �@�w��Ū���@�� */
        pMessage->others = fread_string( pFile );
        break;
      }

      break;

    case 'S':

      if ( !str_scmp( word , "Self", &fMatch ) )
      {
        if ( pMessage->self )
        {
          mudlog( LOG_DEBUG, "Load_message�R�ԭz���Щw�q�C" );
          break;
        }

        fread_to_eol( pFile ); /* �@�w��Ū���@�� */
        pMessage->self = fread_string( pFile );
        break;
      }

      if ( !str_scmp( word , "String", &fMatch ) )
      {
        if ( pMessage->self )
        {
          mudlog( LOG_DEBUG, "Load_message�R�ԭz���Щw�q�C" );
          break;
        }

        if ( pMessage->others )
        {
          mudlog( LOG_DEBUG, "Load_message�R�ԭz���Щw�q�C" );
          break;
        }

        fread_to_eol( pFile ); /* �@�w��Ū���@�� */
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
          mudlog( LOG_DEBUG, "Load_message�R�ԭz���A %d ���~�C"
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

    /* ���O���A���~ */
    if ( !fMatch || FileError )
    {
      if ( !fMatch ) mudlog( LOG_DEBUG , "Load_message�R�R�O %s �����T�C", word );
      if ( FileError )
      {
        free_message( pMessage );
        RETURN( NULL );
      }
    }
  }

  RETURN( NULL );
}

/* Ū�J�ϰ��ɪ� reset */
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
    mudlog( LOG_FAILLOAD , "���J�ɮ� %s ���ѡC\n" , filename );
    RETURN_NULL();
  }

  if ( !pArea )
    mudlog( LOG_DEBUG , "Load_resets�R�S������ #AREA �ҥH���వ reset �C" );

  for ( ;; )
  {
    if ( if_eof( pFile ) ) break;
    letter = fread_letter( pFile );

    if ( letter == '*' )
    {
      fread_to_eol( pFile );
      continue;
    }

    /* �t�m�O���� */
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
      mudlog( LOG_DEBUG , "Load_resets�R���w�q�� RESET �R�O '%c'�C", letter );
      break;

    case 'M':

      if ( !get_mob_index ( pReset->arg1 ) )
        mudlog( LOG_DEBUG, "Load_reset�R(M) �S���Ǫ��s�� %d�C", pReset->arg1 );

      if ( !get_room_index( pReset->arg3 ) )
        mudlog( LOG_DEBUG, "Load_reset�R(M) �S���ж��s�� %d�C", pReset->arg3 );

      break;

    case 'O':

      if ( !get_obj_index ( pReset->arg1 ) )
        mudlog( LOG_DEBUG, "Load_reset�R(O) �S�����~���X %d�C", pReset->arg1 );

      if ( !get_room_index( pReset->arg3 ) )
        mudlog( LOG_DEBUG, "Load_reset�R(O)  �S���ж��s�� %d�C", pReset->arg3 );

      break;

    case 'P':

      if ( !get_obj_index( pReset->arg1 ) )
        mudlog( LOG_DEBUG, "Load_reset�R(P) �S�����~�@���X %d�C", pReset->arg1 );

      if ( !get_obj_index( pReset->arg3 ) )
        mudlog( LOG_DEBUG, "Load_reset�R(P) �S�����~�G���X %d�C", pReset->arg1 );

      break;

    case 'E':
    case 'G':

      if ( !get_obj_index( pReset->arg1 ) )
        mudlog( LOG_DEBUG, "Load_reset�R(EG) �S�����~���X %d�C", pReset->arg1 );

      break;

    case 'A':

      if ( !get_mob_index( pReset->arg1 ) )
        mudlog( LOG_DEBUG, "Load_reset�R(A) �S���Ǫ����X %d�C", pReset->arg1 );

      break;

    case 'D':

      if ( !( pRoomIndex = get_room_index( pReset->arg1 ) ) )
        mudlog( LOG_DEBUG, "Load_reset�R(D) �S���ж����X %d�C", pReset->arg1 );

      if ( pReset->arg2 < 0
        || pReset->arg2 > ( DIR_MAX - 1 )
        || ( pExit = pRoomIndex->exit[pReset->arg2] ) == NULL
        || !pExit->isdoor )
      {
        mudlog( LOG_DEBUG , "Load_resets�R'D'�R�X�f %d ���O�@�Ӫ��C"
          , pReset->arg2 );
      }

      if ( pReset->arg3 < 0 || pReset->arg3 > 2 )
        mudlog( LOG_DEBUG , "Load_resets�R'D'�R���~����R%d�C", pReset->arg3 );

      break;

    case 'R':

      if ( !( pRoomIndex = get_room_index( pReset->arg1 ) ) )
        mudlog( LOG_DEBUG, "Load_reset�R(R) �S���ж����X %d�C", pReset->arg1 );


      if ( pReset->arg2 < 0 || pReset->arg2 > 6 )
        mudlog( LOG_DEBUG , "Load_resets�R'R'�R���~�X�f %d�C", pReset->arg2 );

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

  /* �B�z�ؿ� */
  fill_path( strcpy( directory, path ) );

  if ( ( rc = glob_file( directory, note_ext, &result ) ) >= 0 )
  {
    for ( count = 0; count < rc; count++ )
    {
      sprintf( buf, "%s%s", directory, result.gl_pathv[count] );

      if ( is_regular( buf ) )
      {
        if ( !( pFile = f_open( buf , "r" ) ) )
          mudlog( LOG_ERR , "Load_mail�R�}�ҫH���� %s �����D�C" , buf );

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
    mudlog( LOG_ERR , "Load_mail�R�S���l��l�ؿ� %s�C", path );
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
        if ( !pNote->sender  ) mudlog( LOG_DEBUG , "Load_a_note�R�ʥF�H�H�̡C" );
        if ( !pNote->date    ) mudlog( LOG_DEBUG , "Load_a_note�R�ʥF�ɶ��C"   );
        if ( !pNote->to_list ) mudlog( LOG_DEBUG , "Load_a_note�R�ʥF���H�̡C" );
        if ( !pNote->subject ) mudlog( LOG_DEBUG , "Load_a_note�R�ʥF�D�D�C"   );
        if ( !pNote->text    ) mudlog( LOG_DEBUG , "Load_a_note�R�ʥF���e�C"   );

        if ( IS_ERROR( pNote->date_stamp ) )
          mudlog( LOG_DEBUG , "Load_a_note�R�ʥF�l�W�C" );

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
        fread_to_eol( pFile ); /* �@�w��Ū���@�� */
        pNote->text = fread_string( pFile );
        break;
      }

      break;
    }

    /* ���O���A���~ */
    if ( !fMatch )
      mudlog( LOG_ERR , "Load_a_note�R�R�O %s �����T�C" , word );
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

  /* �B�z�ؿ� */
  fill_path( strcpy( directory, path ) );

  if ( ( rc = glob_file( directory, joke_ext, &result ) ) >= 0 )
  {
    for ( count = 0; count < rc; count++ )
    {
      sprintf( buf, "%s%s", directory, result.gl_pathv[count] );

      if ( is_regular( buf ) )
      {
        if ( !( pFile = f_open( buf , "r" ) ) )
          mudlog( LOG_DEBUG , "Load_joke�R�}�ү����� %s �����D�C" , buf );

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
    mudlog( LOG_DEBUG , "Load_joke�R�S�����ܤl�ؿ� %s�C", path );
  }

  mudlog( LOG_INFO , "�t�θ��J %d �ӯ����ɡC" , top_joke );
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
          mudlog( LOG_DEBUG , "Load_a_joke�R�ʥF���D�C" );

        if ( !pJoke->org )
          mudlog( LOG_DEBUG, "Load_a_joke�R�ʥF�X�B�C" );

        if ( !pJoke->text || !*pJoke->text )
          mudlog( LOG_DEBUG , "Load_a_joke�R�ʥF����C" );

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
        fread_to_eol( pFile ); /* �@�w��Ū���@�� */
        pJoke->text = fread_string( pFile );

        break;
      }
      break;

    }

    /* ���O���A���~ */
    if ( !fMatch )
      mudlog( LOG_DEBUG , "Load_a_joke�R�R�O %s �����T�C" , word );
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

  /* �B�z�ؿ� */
  fill_path( strcpy( directory, path ) );

  if ( ( rc = glob_file( directory, vote_ext, &result ) ) >= 0 )
  {
    for ( count = 0; count < rc; count++ )
    {
      sprintf( buf, "%s%s", directory, result.gl_pathv[count] );
      if ( is_regular( buf ) )
      {
        if ( !( pFile = f_open( buf , "r" ) ) )
          mudlog( LOG_DEBUG , "Load_vote�R�}�ҧ벼�� %s �����D�C" , buf );

        pVote = load_a_vote( pFile );
        f_close( pFile );

        /* �p�G�w�g�}���h�H�H��覡���G */
        if ( vote_timeup( pVote ) )
        {
          vote_to_note( pVote );

          if ( unlink( buf ) != 0 )
            mudlog( LOG_INFO, "Load_vote�R�����벼�� %s ���ѡC", buf );

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
    mudlog( LOG_DEBUG , "Load_vote�R�S���벼�l�ؿ� %s�C", path );
  }

  mudlog( LOG_INFO , "�t�θ��J %d �ӧ벼�ɡC" , top_vote );
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
        fread_to_eol( pFile ); /* �@�w��Ū���@�� */
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

    /* ���O���A���~ */
    if ( !fMatch )
      mudlog( LOG_ERR , "Load_a_vote�R�R�O %s �����T�C" , word );
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
    mudlog( LOG_ERR , "�L�k�}�ұ`���ഫ�� %s�C" , filename );

  for ( ; ; )
  {
    word = fread_word( fp );
    if ( if_eof( fp ) ) break;

    /* symbol.def �C��H*���}�Y������@�O�аO���|Ū�� */
    if ( word[0] == '*' )
    {
      fread_to_eol( fp );
      continue;
    }

    /* symbol.def �Y�O�X�{ #END �h�N���ɮ׵����M���᪺���|�AŪ�� */
    if ( !str_cmp( word , "#END" ) ) break;

    /* �t�m�O���� */
    pSymbol = alloc_struct( STRUCT_SYMBOL_DATA );

    pSymbol->str   = str_dup( word );
    pSymbol->num   = fread_number( fp );

    field          = fread_word( fp );
    pSymbol->field = str_dup( field );

    count++;

    if ( !isalpha( ( int ) pSymbol->str[0] ) )
      mudlog( LOG_ERR , "Load_symbol�R%s ���O�^��Ÿ��C" , pSymbol->str );

    letter                    = LOWER( pSymbol->str[0] ) - 'a';
    iLen                      = UMIN( 19, str_len( pSymbol->str ) - 1 );
    pSymbol->next             = symbol_hash[letter][iLen];
    symbol_hash[letter][iLen] = pSymbol;

    pSymbol->next_content = symbol_list;
    symbol_list           = pSymbol;
  }

  f_close( fp );
  mudlog( LOG_INFO , "�t�θ��J %d �ӱ`�ƲŸ��C" , count );
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

  /* �B�z�ؿ� */
  fill_path( strcpy( directory, path ) );

  if ( ( rc = glob_file( directory, help_ext, &result ) ) >= 0 )
  {
    for ( count = 0; count < rc; count++ )
    {
      sprintf( buf, "%s%s", directory, result.gl_pathv[count] );

      /* ���ɦW�� hlp ���ɮ� */
      if ( is_regular( buf ) )
      {
        if ( !( pFile = f_open( buf , "r" ) ) )
          mudlog( LOG_ERR , "Load_help�R�}�ҨD�U�� %s �����D" , buf );

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
    mudlog( LOG_ERR , "Load_help�R�S���D�U�l�ؿ� %s�C", path );
  }

  mudlog( LOG_INFO , "�t�θ��J %d �ӨD�U�ɡC" , top_help );
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

  /* �B�z�ؿ� */
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
        /* �t�m�O���� */
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
        mudlog( LOG_ERR , "Load_social�R�L�k�}�_����ʰ��� %s�C" , buf );
      }
    }
  }
  else
  {
    mudlog( LOG_ERR , "Load_social�R�L�k�}�Ҫ�������� %s�C" , indexfile );
  }

  f_close( aFile );
  mudlog( LOG_INFO , "�t�θ��J %d �Ӫ���ʧ@�ɡC" , top_social );
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

  /* �B�z�ؿ� */
  fill_path( strcpy( directory, path ) );

  if ( ( rc = glob_file( directory, angel_ext, &result ) ) >= 0 )
  {
    for ( count = 0; count < rc; count++ )
    {
      sprintf( buf, "%s%s", directory, result.gl_pathv[count] );
      if ( is_regular( buf ) )
      {
        if ( !( pFile = f_open( buf , "r" ) ) )
          mudlog( LOG_ERR , "Load_angel�R�}�Ҧu�@���� %s �����D�C" , buf );

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
    mudlog( LOG_ERR , "Load_angel�R�S���u�@���l�ؿ� %s�C", path );
  }

  mudlog( LOG_INFO , "�t�θ��J %d �Ӧu�@������ɡC" , top_angel );
  RETURN_NULL();
}

/* Ū���@�Ӧu�@����� */
ANGEL_DATA * load_a_angel( FILE_DATA * pFile )
{
  ANGEL_DATA * pAngel;
  char       * word;
  bool         fMatch;

  PUSH_FUNCTION( "load_a_angel" );

  /* �t�m�O����H�γ]�w�w�]�� */
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
        fread_to_eol( pFile ); /* �@�w��Ū���@�� */
        pAngel->description = fread_string( pFile );
        break;
      }

      break;

    case 'E':

      if ( !str_scmp( word, "End", &fMatch ) )
      {
        if ( pAngel->description && pAngel->function )
          mudlog( LOG_DEBUG, "Load_a_angel�R�u�@������ƤS���y�z�C" );

        if ( !pAngel->description && !pAngel->function )
          mudlog( LOG_DEBUG, "Load_a_angel�R�u�@���S����ƤS�S���y�z�C" );

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
            mudlog( LOG_DEBUG , "Load_a_angel�R�u�@������� %s �䤣��C" , name );

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
          mudlog( LOG_DEBUG, "Load_a_angel�R�u�@�̰����� %d ����C", level );

        pAngel->higher = level;
        break;
      }

      break;

    case 'L':

      if ( !str_scmp( word, "Lower", &fMatch ) )
      {
        int level;

        if ( ( level = fread_number( pFile ) ) < 0 || level > MAX_LEVEL )
          mudlog( LOG_DEBUG, "Load_a_angel�R�u�@�̧C���� %d ����C", level );

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
           mudlog( LOG_DEBUG, "Load_a_angel�R�u�@���A����C", position );
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

    /* ���O���A���~ */
    if ( !fMatch )
      mudlog( LOG_DEBUG , "Load_a_angel�R�R�O %s �����T�C" , word );
  }

  RETURN( NULL );
}

/* ���J�i���e�� */
void load_welcome_immortal( const char * filename )
{
  FILE_DATA * pFile;

  PUSH_FUNCTION( "load_welcome_immortal" );

  if ( !( pFile = f_open( filename , "r" ) ) )
    mudlog( LOG_ERR , "Load_welcome_immortal�R�}���w�ﯫ�ڵe���� %s �����D�C"
    , filename );

  welcome_immortal = str_dup( map_file( pFile ) );

  f_close( pFile );
  RETURN_NULL();
}

/* ���J�i���e�� */
void load_welcome( const char * filename )
{
  FILE_DATA * pFile;

  PUSH_FUNCTION( "load_welcome" );

  if ( !( pFile = f_open( filename , "r" ) ) )
    mudlog( LOG_ERR , "Load_welcome�R�}���w��e���� %s �����D�C" , filename );

  welcome_message = str_dup( map_file( pFile ) );
  f_close( pFile );
  RETURN_NULL();
}

void load_motd( const char * filename )
{
  FILE_DATA * pFile;

  PUSH_FUNCTION( "load_motd" );

  if ( !( pFile = f_open( filename , "r" ) ) )
    mudlog( LOG_ERR , "Load_motd�R�}�Ҥ�������� %s �����D�C" , filename );

  motd_message = str_dup( map_file( pFile ) );
  f_close( pFile );

  RETURN_NULL();
}

/* ���J�i���e�� */
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

  /* �B�z�ؿ� */
  fill_path( strcpy( directory, path ) );

  if ( ( rc = glob_file( directory, greet_ext, &result ) ) >= 0 )
  {
    for ( count = 0; count < rc; count++ )
    {
      sprintf( buf, "%s%s", directory, result.gl_pathv[count] );

      if ( is_regular( buf ) )
      {
        if ( !( pFile = f_open( buf , "r" ) ) )
          mudlog( LOG_ERR , "Load_greeting�R�}�Ҷi���e���� %s �����D�C" , buf );

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
    mudlog( LOG_ERR , "Load_greeting�R�S���i���e���l�ؿ� %s�C", path );
  }

  if ( top_greeting <= 0 )
    mudlog( LOG_ERR, "Load_greeting�R�ܤ֭n���@�Ӷi���e���C" );

  mudlog( LOG_INFO , "�t�θ��J %d �Ӷi���e���ɡC" , top_greeting );
  RETURN_NULL();
}

/* ���J�a�θ���ɮ�, ���ɦW�� def */
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

  /* �B�z�ؿ� */
  fill_path( strcpy( directory, path ) );

  if ( ( rc = glob_file( directory, sector_ext, &result ) ) >= 0 )
  {
    for ( count = 0; count < rc; count++ )
    {
      sprintf( buf, "%s%s", directory, result.gl_pathv[count] );

      if ( is_regular( buf ) )
      {
        if ( !( pFile = f_open( buf , "r" ) ) )
          mudlog( LOG_ERR , "Load_sector�R�}�Ҧa���ɮ� %s �����D�C" , buf );

        pSector = alloc_struct( STRUCT_SECTOR_DATA );
        set_sector_default( pSector );

        for ( finish = FALSE; finish != TRUE; )
        {
          word   = if_eof( pFile ) ? "End" : fread_word( pFile );
          fMatch = FALSE;

          switch ( UPPER( word[0] ) )
          {
          /* ���� */
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
                  mudlog( LOG_DEBUG, "Load_sector�R�w�]�a�έ��СC" );

                DefaultSector = pSector;
              }

              break;
            }
            break;

          case 'E':

            if ( !str_scmp( word, "End", &fMatch ) )
            {
              if ( IS_ERROR( pSector->vnum ) )
                mudlog( LOG_DEBUG , "Load_sector�R�a���ɮרS���s���C" );

              if ( IS_ERROR( pSector->movement ) )
                mudlog( LOG_DEBUG , "Load_sector�R�a���ɮרS�����ʮ��ӼƭȡC" );

              if ( !pSector->cname )
                mudlog( LOG_DEBUG , "Load_sector�R�a���ɮרS������W�١C" );

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
                mudlog( LOG_DEBUG , "Load_sector�R�s�� %d ���a�����X���СC"
                  , vnum );

              if ( vnum < 0 )
                mudlog( LOG_DEBUG , "Load_sector�R�s�� %d ���a���ɮת��s�����X�k�C"
                  , vnum );

              pSector->vnum = vnum;
              break;
            }

            break;
          }

          /* ���O���A���~ */
          if ( !fMatch )
            mudlog( LOG_DEBUG , "Load_sector�R�R�O %s �����T�C" , word );
        }

        f_close( pFile );
      }
    }
    globfree( &result );
  }

  else
  {
    mudlog( LOG_ERR , "Load_sector�R�S���a�θ�ƪ��l�ؿ� %s�C", path );
  }

  if ( !DefaultSector ) mudlog( LOG_ERR, "Load_sector�R�S���w�]�a�ΡC" );

  mudlog( LOG_INFO , "�t�θ��J %d �Ӧa�θ���ɡC" , top_sector );
  RETURN_NULL();
}

/* ���J¾�~����ɮ�, ���ɦW�� def */
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

  /* �B�z�ؿ� */
  fill_path( strcpy( directory, path ) );

  if ( ( rc = glob_file( directory, class_ext, &result ) ) >= 0 )
  {
    for ( count = 0; count < rc; count++ )
    {
      sprintf( buf, "%s%s", directory, result.gl_pathv[count] );

      if ( is_regular( buf ) )
      {
        if ( !( pFile = f_open( buf , "r" ) ) )
          mudlog( LOG_ERR , "Load_class�R�}��¾�~�ɮ� %s �����D�C" , buf );

        pClass = alloc_struct( STRUCT_CLASS_DATA );
        set_class_default( pClass );

        for ( finish = FALSE; finish != TRUE; )
        {
          word   = if_eof( pFile ) ? "End" : fread_word( pFile );
          fMatch = FALSE;

          switch ( UPPER( word[0] ) )
          {
          /* ���� */
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
                mudlog( LOG_DEBUG , "Load_class�R¾�~�ɨS���s���C" );

              if ( pClass->mask == 0 )
                mudlog( LOG_DEBUG , "Load_class�R¾�~�ɨS���B�n�X�СC" );

              if ( !pClass->name )
                mudlog( LOG_DEBUG, "Load_class�R¾�~�ɨS���^��W�١C" );

              if ( !pClass->cname )
                mudlog( LOG_DEBUG, "Load_class�R¾�~�ɨS������W�١C" );

              if ( !pClass->title )
                mudlog( LOG_DEBUG, "Load_class�R¾�~�ɨS�����Y�C" );

              if ( pClass->limit > 0 && !pClass->msg_limit )
                mudlog( LOG_DEBUG, "Load_class�R¾�~�ɨS������ԭz�C" );

              if ( pClass->multiplier <= 1 )
                mudlog( LOG_DEBUG, "Load_class�R¾�~�ɷ������Ƥ��X�z�C" );

              if ( pClass->nskill <= 0 )
                mudlog( LOG_DEBUG, "Load_class�R¾�~�ɪ��ޯ��`�Ƥ��X�z�C" );

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
                mudlog( LOG_DEBUG , "Load_class�R�s�� %d ��¾�~�ɮת���¾�����X�k�C"
                  , amount );

              pClass->rebirth_gold = amount;
              break;
            }

            if ( !str_scmp( word, "Rank", &fMatch ) )
            {
              if ( ( amount = fread_number( pFile ) ) < 0
                || amount > MAX_LEVEL )
                mudlog( LOG_DEBUG, "Load_class�R�s�� %d �����Y���X�k�C", amount );

              if ( pClass->rank[amount] )
                mudlog( LOG_DEBUG, "Load_class�R�s�� %d �����Y���Ʃw�q�C", amount );

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
                mudlog( LOG_DEBUG , "Load_class�R�s�� %d ��¾�~�ɮת��W�����Ť��X�k�C"
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
                mudlog( LOG_DEBUG , "Load_class�R�s�� %d ��¾�~���X���СC" , amount );

              if ( amount < 0 || amount >= 32 )
                mudlog( LOG_DEBUG , "Load_class�R�s�� %d ��¾�~�ɮת��s�����X�k�C"
                  , amount );

              if ( amount < 0 || amount >= 32 )
                mudlog( LOG_DEBUG, "Load_class�R¾�~�s�����X�z�C" );

              mask = 1 << amount;

              for ( zClass = class_list; zClass; zClass = zClass->next )
              {
                if ( mask == zClass->mask )
                  mudlog( LOG_DEBUG, "Load_class�R¾�~�B�n�X�Э��СC" );
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

          /* ���O���A���~ */
          if ( !fMatch )
            mudlog( LOG_DEBUG , "Load_class�R�R�O %s �����T�C" , word );
        }

        f_close( pFile );
      }
    }
    globfree( &result );

    /* ��M�w�]��¾�~ */
    for ( amount = 0, class_force = class_demos = NULL, pClass = class_list;
      pClass;
      pClass = pClass->next )
    {
      if ( amount >= MAX_DEFAULT_CLASS )
        mudlog( LOG_ERR, "Load_class�R�w�]¾�~�Ӧh�C" );

      if ( pClass->select ) iClass[amount++] = pClass;

      if ( pClass->rudiment )
      {
        if ( class_demos ) mudlog( LOG_ERR, "Load_class�R�w�]¾�~ %d ���ơC"
          , pClass->vnum );
        class_demos = pClass;
      }

      if ( pClass->force )
      {
        if ( class_force ) mudlog( LOG_ERR, "Load_class�R�j��¾�~ %d ���ơC"
          , pClass->vnum );
        class_force = pClass;
      }
    }

    if ( !class_demos ) mudlog( LOG_ERR, "Load_class�R�S���w�]��¾�~�C" );
    if ( !class_force ) mudlog( LOG_ERR, "Load_class�R�S���j�¾�~�C" );
    if ( amount <= 0  ) mudlog( LOG_ERR, "Load_class�R�S����ܪ�¾�~�C" );
  }

  else
  {
    mudlog( LOG_ERR , "Load_class�R�S��¾�~��ƪ��l�ؿ� %s�C", path );
  }

  mudlog( LOG_INFO , "�t�θ��J %d ��¾�~����ɡC" , top_class );
  RETURN_NULL();
}

/* ���J�G�����ɮ�, ���ɦW�� def */
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

  /* �B�z�ؿ� */
  fill_path( strcpy( directory, path ) );

  if ( ( rc = glob_file( directory, liquid_ext, &result ) ) >= 0 )
  {
    for ( count = 0; count < rc; count++ )
    {
      sprintf( buf, "%s%s", directory, result.gl_pathv[count] );

      if ( is_regular( buf ) )
      {
        if ( !( pFile = f_open( buf , "r" ) ) )
          mudlog( LOG_ERR , "Load_liq�R�}�ҲG���ɮ� %s �����D�C" , buf );

        set_liq_default( pLiq = alloc_struct( STRUCT_LIQ_DATA ) );

        for ( finish = FALSE; finish != TRUE; )
        {
          word   = if_eof( pFile ) ? "End" : fread_word( pFile );
          fMatch = FALSE;

          switch ( UPPER( word[0] ) )
          {
          /* ���� */
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
              if ( !pLiq->name     ) mudlog( LOG_DEBUG, "Load_liq�R�G���ɨS���W�١C" );
              if ( !pLiq->color    ) mudlog( LOG_DEBUG, "Load_liq�R�G���ɨS���C��C" );
              if ( pLiq->slot <= 0 ) mudlog( LOG_DEBUG, "Load_liq�R�G��S���s���C"  );

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
                mudlog( LOG_DEBUG, "Load_liq�R�G��s�� %d ���ơC", pLiq->slot );

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

          /* ���O���A���~ */
          if ( !fMatch )
             mudlog( LOG_DEBUG , "Load_liq�R�R�O %s �����T�C" , word );
        }

        f_close( pFile );
      }
    }
    globfree( &result );

    /* ��M�������G�� */
    for ( liq_water = NULL, pLiq = liq_list; pLiq; pLiq = pLiq->next )
    {
      if ( pLiq->water )
      {
        if ( liq_water ) mudlog( LOG_ERR, "Load_liq�R�G��� %d ���ơC", pLiq->slot );
        liq_water = pLiq;
      }
    }

    if ( !liq_water ) mudlog( LOG_ERR, "Load_liq�R�S�������G��C" );
  }

  else
  {
    mudlog( LOG_ERR , "Load_liq�R�S���G���ƪ��l�ؿ� %s�C", path );
  }

  mudlog( LOG_INFO , "�t�θ��J %d �ӲG�����ɡC" , top_liq );
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
    mudlog( LOG_ERR , "Load_site�R�L�k�}�ҸT��s�u�ɮ� %s�C" , filename );

  for ( ;; )
  {
    if ( if_eof( File ) ) break;

    word = fread_word( File );

    /* �C��H * ���}�Y������@�O�аO���|Ū�� */
    if ( word[0] == '*' )
    {
      fread_to_eol( File );
      continue;
    }

    /* �Y�O�X�{ #END �h�N���ɮ׵����M���᪺���|�AŪ�� */
    if ( !str_cmp( word , "#END" ) ) break;

    ban_type = fread_number( File );

    switch( ban_type )
    {
    default :
      mudlog( LOG_DEBUG , "Load_site�R���J��} %s ��T���~ %d�C" , word , ban_type );
      break;

    case BAN_LOGIN  :
    case BAN_FQDN   :
    case BAN_FINGER :
      break;
    }

    /* �t�m�O���� */
    pBan = alloc_struct( STRUCT_BAN_DATA );

    pBan->name = str_dup( word );
    pBan->type = ban_type;
    pBan->next = ban_list;
    ban_list   = pBan;
    mudlog( LOG_INFO , "�t�γ]�w %s ����}�X�� %d�C" , word , ban_type );
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
    mudlog( LOG_ERR , "Load_server�R�L�k�}�Ҥu�@���ɮ� %s�C", filename );

  for ( count = 0; ; )
  {
    if ( if_eof( File ) ) break;
    word = fread_word( File );

    /* �C��H * ���}�Y������@�O�аO���|Ū�� */
    if ( word[0] == '*' )
    {
      fread_to_eol( File );
      continue;
    }

    /* �Y�O�X�{ #END �h�N���ɮ׵����M���᪺���|�AŪ�� */
    if ( !str_cmp( word , "#END" ) ) break;

    /* �t�m�O���� */
    set_server_default( pServer = alloc_struct( STRUCT_SERVER_DATA ) );
    pServer->address = str_dup( word );
    pServer->next    = server_list;
    server_list      = pServer;
    count++;

  }

  f_close( File );
  mudlog( LOG_INFO, "�t�γ]�w %d �Ӥu�@����}�C", count );

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
    mudlog( LOG_ERR , "Load_xnames�R�L�k�}�ҫO�d�r�ɮ� %s�C" , filename );

  for ( ;; )
  {
    if ( if_eof( File ) ) break;
    word = fread_word( File );

    /* symbol.def �C��H*���}�Y������@�O�аO���|Ū�� */
    if ( word[0] == '*' )
    {
      fread_to_eol( File );
      continue;
    }

    /* �Y�O�X�{ #END �h�N���ɮ׵����M���᪺���|�AŪ�� */
    if ( !str_cmp( word , "#END" ) ) break;

    switch( ( xnames_type = fread_number( File ) ) )
    {
    default :
      mudlog( LOG_DEBUG , "Load_xnames�R���J�O�d�r %s ���~�X�� %d�C"
        , word , xnames_type );

      break;

    case XNAMES_SYSTEM  :
    case XNAMES_CACONYM :
    case XNAMES_CHAT    :
    case XNAMES_CNAME   :
      break;
    }

    /* �t�m�O���� */
    pXnames = alloc_struct( STRUCT_XNAME_DATA );

    pXnames->word = str_dup( word );
    pXnames->type = xnames_type;
    pXnames->next = xnames_first;
    xnames_first  = pXnames;
    count++;
  }

  mudlog( LOG_INFO , "�t�θ��J %d �ӫO�d�r�C" , count );
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

  /* �B�z�ؿ� */
  str_cpy( directory , path );
  fill_path( directory );

  /* �B�z�����ɮצW�� */
  sprintf( indexfile , "%s%s" , directory , index );

  /* �}�ү����ɮ�, ����S���o���ɮ� */
  if ( ( pFile = f_open( indexfile , "r" ) ) )
  {
    /* �@��Ū�����ɮץ��� */
    while ( !if_eof( pFile ) )
    {
      /* Ū���R�O�ɮת��W�� */
      word = fread_word( pFile );
      fread_to_eol( pFile );

      if ( word[0] == '*' ) continue;

      /* �B�z���}�ҩR�O�ɮת��W�� */
      sprintf( buf , "%s%c/%s.%s"
        , directory , LOWER( word[0] ) , word, skill_ext );

      if ( ( aFile = f_open( buf , "r" ) ) )
      {
        /* �t�m�O���� */
        set_skill_default( pSkill = alloc_struct( STRUCT_SKILL_DATA ) );

        /* ���J�ޯ઺�D�{������ */
        for ( finish = FALSE; finish != TRUE; )
        {
          word   = if_eof( aFile ) ? "End" : fread_word( aFile );
          fMatch = FALSE;

          switch ( UPPER( word[0] ) )
          {
          /* ���� */
          case '*':
            fMatch = TRUE;
            fread_to_eol( aFile );
            break;

          case '#':

            if ( !str_scmp( word, "#DAMAGE", &fMatch ) )
            {
              pDamage = load_damage( aFile );

              /* ��M�̫�@�� */
              for ( zDamage = pSkill->damage;
                    zDamage && zDamage->next;
                    zDamage = zDamage->next );

              if ( !zDamage ) pSkill->damage = pDamage;
              else            zDamage->next  = pDamage;

              break;
            }

            if ( !str_scmp( word, "#Restrict", &fMatch ) )
            {
              /* ���J�ޯ୭�� */
              if ( !( pRestrict = load_restrict( aFile ) ) )
              {
                mudlog( LOG_DEBUG, "Load_skill�R���J�ޯ୭��ѡC" );
                break;
              }

              for ( zRestrict = pSkill->restrict; zRestrict; zRestrict = zRestrict->next )
              {
                if ( zRestrict->type == pRestrict->type
                  && zRestrict->type != RES_SKILL )
                {
                  mudlog( LOG_DEBUG , "Load_skill�R�ޯ୭��A %d ���ơC"
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
                mudlog( LOG_ERR, "Load_skill�R%s �w���v�T�����C", pSkill->name );

              if ( !( pSkill->affected = load_affect( aFile ) ) )
              {
                mudlog( LOG_DEBUG, "Load_skill�R���J�k�N�v�T���ѡC" );
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
                  "Load_skill�R�ޯ�۫g���� %d ���X�z�C" ,pSkill->antirating );
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
                mudlog( LOG_DEBUG , "Load_skill�R������m��%d���X�z�C"
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
                mudlog( LOG_DEBUG , "Load_skill�R�ޯ��ɪ������� %s �䤣��C" , name );

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
                    "Load_skill�R�ޯत��W�� %s ���СC" , name );
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
                mudlog( LOG_DEBUG , "Load_skill�R���ӫ��A %d ���~�C", type );
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
                mudlog( LOG_DEBUG , "Load_skill�R�{�� %d ���X�k�C" , degree );

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
                mudlog( LOG_DEBUG, "Load_skill�R�ޯ�S���s���C" );

              if ( !pSkill->name )
                mudlog( LOG_DEBUG , "Load_skill�R�ޯ�S���W�١C" );

              if ( !pSkill->cname )
                mudlog( LOG_DEBUG , "Load_skill�R�ޯ�S������W�١C" );

              if ( pSkill->function && pSkill->damage )
                mudlog( LOG_DEBUG, "Load_skill�R����ƤS����˱ԭz�C" );

              if ( !pSkill->function && !pSkill->damage )
                mudlog( LOG_DEBUG , "Load_skill�R�ޯ�S���ާ@��ơC" );

              if ( !fReload ) top_skill++;

              if ( top_skill >= MAX_SKILL )
                mudlog( LOG_DEBUG, "Load_skill�R�ޯ�ƥضW�L�]�w�C" );

              if ( pSkill->innate && pSkill->adeptation <= 0 )
                mudlog( LOG_DEBUG, "Load_skill�R������o�S���m�סC" );

              /* �p����v�`�M */
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
                  mudlog( LOG_DEBUG , "Load_skill�R�ޯ��ɪ���� %s �䤣��C" , name );

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
                mudlog( LOG_DEBUG, "Load_skill�R�ޯ��ɤ�¾�~ %d ���s�b�C", iClass );

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

              /* �@�w��Ū���@�� */
              fread_to_eol( aFile );
              pSource = temp = fread_string( aFile );

              if ( str_len( temp ) > sizeof( buf ) - 10 )
                mudlog( LOG_DEBUG, "Load_skill�R�T���Ӫ��C" );

              /* ��������r�� */
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
                  mudlog( LOG_DEBUG, "Load_skill�R�ޯ�W�� %s ���СC" , name );
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
                  "Load_skill�R�ޯ�̧C���A %d ���X�z�C" , position );
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
                  "Load_skill�R�ޯ��ѧO���� %d ���X�z�C" ,pSkill->rating );
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
                mudlog( LOG_DEBUG, "Load_skill�R�ޯ�ʧO %d ���X�z�C", pSkill->sex );
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
                mudlog( LOG_DEBUG, "Load_skill�R�ޯ�s�� %d ���X�z�C"
                  , pSkill->slot );

              if ( get_skill( pSkill->slot ) )
                mudlog( LOG_DEBUG , "Load_skill�R�ޯ�s�� %d ���ơC"
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
                mudlog( LOG_DEBUG , "Load_skill�R�ޯ�κA %d ����C" , type );
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
                mudlog( LOG_DEBUG ,"Load_skill�R�Z���κA %d ����C", weapon );
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

          /* ���O���A���~ */
          if ( !fMatch )
            mudlog( LOG_DEBUG , "Load_skill�R�R�O %s �����T�C" , word );
        }

        f_close( aFile );
      }

      else
      {
        mudlog( LOG_ERR , "Load_skill�R�L�k�}�ҧޯ��� %s�C" , buf );
      }
    }
  }

  else
  {
    mudlog( LOG_ERR , "Load_skill�R�S���ޯ�ؿ��C�� %s�C", buf );
  }

  f_close( pFile );
  mudlog( LOG_INFO , "�t�θ��J %d �ӧޯ��ɡC" , top_skill );
  RETURN_NULL();
}


DAMAGE_DATA * load_damage( FILE_DATA * pFile )
{
  DAMAGE_DATA * pDamage;
  char        * word;
  bool          fMatch;

  PUSH_FUNCTION( "load_damage" );

  /* �t�m�оɩһݭn���O����H�γ]�w�w�]�� */
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

        /* �@�w��Ū���@�� */
        fread_to_eol( pFile );
        pSource = temp = fread_string( pFile );

        if ( str_len( temp ) > sizeof( buf ) - 10 )
          mudlog( LOG_DEBUG, "Loa_damage�R��˴y�z�Ӫ��C" );

        /* ��������r�� */
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
          mudlog( LOG_DEBUG, "Load_damage�R��˨S���ԭz�C" );

        if ( IS_ERROR( pDamage->chance ) || pDamage->chance <= 0 )
          mudlog( LOG_DEBUG, "Load_damage�R��˨S�����v�Τ����T�C" );

        if ( IS_ERROR( pDamage->value )  || pDamage->value < 0 )
          mudlog( LOG_DEBUG, "Loa_damage�R��˨S���ˮ`�ȩΤ����T�C" );

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
          mudlog( LOG_DEBUG, "Load_damage�R�������A���~�C" );
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
          mudlog( LOG_DEBUG, "Load_damage�R�v�T���v %d �L�j�ιL�p�C", pEffect->value );
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
            mudlog( LOG_DEBUG , "Load_damage�R�ޯ��ɪ���� %s �䤣��C" , name );

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

    /* ���O���A���~ */
    if ( !fMatch ) mudlog( LOG_DEBUG , "Load_damage�R�R�O %s �����T�C" , word );
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

  /* �B�z�ؿ� */
  str_cpy( directory , path );
  fill_path( directory );

  /* �B�z�����ɮצW�� */
  sprintf( indexfile , "%s%s" , directory , index );

  /* �}�ү����ɮ�, ����S���o���ɮ� */
  if ( ( pFile = f_open( indexfile , "r" ) ) )
  {
    /* �@��Ū�����ɮץ��� */
    while ( !if_eof( pFile ) )
    {
      /* Ū���R�O�ɮת��W�� */
      word = fread_word( pFile );
      fread_to_eol( pFile );

      if ( word[0] == '*' ) continue;

      /* �B�z���}�ҩR�O�ɮת��W�� */
      sprintf( buf , "%s%c" , directory , LOWER( word[0] ) );

      fill_path( buf            );
      str_cat( buf, word        );
      str_cat( buf, "."         );
      str_cat( buf, command_ext );

      if ( ( aFile = f_open( buf , "r" ) ) )
      {
        /* �t�m�O���� */
        pCommand = alloc_struct( STRUCT_CMD_DATA );

        /* ���m�w�]�� */
        set_command_default( pCommand );

        /* ���J���O���D�{������ */
        for ( finish = FALSE; finish != TRUE; )
        {
          word   = if_eof( aFile ) ? "End" : fread_word( aFile );
          fMatch = FALSE;

          switch ( UPPER( word[0] ) )
          {
          /* ���� */
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
                mudlog( LOG_DEBUG , "Load_instrument�R���O�ɨS���W�١C" );

              if ( !pCommand->cname )
                mudlog( LOG_DEBUG , "Load_instrument�R���O�ɨS������W�١C" );

              if ( !pCommand->function )
                mudlog( LOG_DEBUG , "Load_instrument�R���O�ɨS�������ơC" );

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
                mudlog( LOG_DEBUG , "Load_instrument�R���O�ɪ���� %s �䤣��C"
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
                mudlog( LOG_DEBUG , "Load_instrument�R���O�ɪ��аO�ܼ� %d ���X�z�C" , log );
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
                  mudlog( LOG_DEBUG , "Load_instrument�R���O�ɫ��O %s ���СC"
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
                mudlog( LOG_DEBUG ,"Load_instrument�R�ޯ�̧C���A %d ���X�z�C"
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

          /* ���O���A���~ */
          if ( !fMatch )
            mudlog( LOG_DEBUG , "Load_instrument�R�R�O %s �����T�C" , word );
        }

        f_close( aFile );
      }

      else
      {
        mudlog( LOG_ERR , "Load_instrument�R�L�k�}�ҩR�O�� %s�C" , buf );
      }
    }
  }

  else
  {
    mudlog( LOG_ERR , "Load_instrument�R�S���R�O�ؿ��C�� %s�C", path );
  }

  f_close( pFile );
  mudlog( LOG_INFO , "�t�θ��J %d �ӫ��O�ɡC" , top_cmd );
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
      /* ���� */
      case '*':
        fMatch = TRUE;
        fread_to_eol( pFile );
        break;

      case '#':

        if ( !str_scmp( word, "#Station", &fMatch ) )
        {
          /* ���J�ɮ� */
          pNet = load_station( pFile );

          pNet->next = net_list;
          net_list   = pNet;

          /* �p�G�i�H���J */
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
          mudlog( LOG_ERR, "Load_internal�R�A���q�T�� %d ���X�k�C\n" , internal_port );

          break;
        }
        break;
      }

      /* ���O���A���~ */
      if ( !fMatch )
        mudlog( LOG_DEBUG , "Load_internal�R�R�O %s �����T�C" , word );

      if ( Finish ) break;
    }

    f_close( pFile );
  }

  else
  {
    mudlog( LOG_INFO, "�S�����J������ں�������ơC" );
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

        /* �קK���ƪ���} */
        if ( get_net_data( pNet->address, NET_ADDRESS ) )
          mudlog( LOG_ERR, "Load_station�R���} %s ���ơC", pNet->address );

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
        if ( !pNet->name    ) mudlog( LOG_ERR, "Load_station�R�ʥF���W�C"   );
        if ( !pNet->address ) mudlog( LOG_ERR, "Load_station�R�ʥF��}�C"   );
        if ( pNet->port < 0 ) mudlog( LOG_ERR, "Load_station�R�ʥF�q�T��C" );

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

        /* �קK���ƪ����W */
        if ( get_net_data( pNet->name, NET_NAME ) )
          mudlog( LOG_ERR, "Load_station�R���W %s ���ơC", pNet->name );

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

    /* ���O���A���~ */
    if ( !fMatch )
      mudlog( LOG_DEBUG , "Load_station�R�R�O %s �����T�C" , word );
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
      /* ���� */
      case '*':
        fMatch = TRUE;
        fread_to_eol( pFile );
        break;

      case 'B':

        if ( !str_scmp( word, "Bus", &fMatch ) )
        {
          /* �t�m�O����H�γ]�w�w�]�� */
          set_bus_default( ( pBus = alloc_struct( STRUCT_BUS_DATA ) ) );

          /* Ū�J���W */
          pBus->name = str_dup( fread_word( pFile ) );

          /* Ū�J�W�U���B, ���B���ݥ��s�b, �B���঳�V�U�X�f */
          location = fread_number( pFile );

          if ( !( pBus->station = get_room_index( location ) ) )
            mudlog( LOG_DEBUG, "Load_bus�R�䤣��a�K�� %d�C" , location );

          /* �ˬd�O�_���V�U�X�f, ���h���� */
          if ( !check_station( pBus->station ) )
            mudlog( LOG_DEBUG, "Load_bus�R�a�K�� %d ���X�k�C", location );

          /* Ū����x, ����s�b, �t�Ϋإ� */
          platform = get_room_index( location = fread_number( pFile ) );

          if ( platform )
            mudlog( LOG_DEBUG, "Load_bus�R�a�K����x %d �s�b�C", location );

          if ( !( pBus->platform = create_platform( location
            , pBus->name, pBus->station->area ) ) )
          {
            mudlog( LOG_DEBUG, "Load_bus�R�إߦa�K�� %d ���ѡC", location );
          }

          /* �W�U�s������ */
          if ( !link_path( pBus->station, pBus->platform ) )
            mudlog( LOG_DEBUG, "Load_bus�R�s���a�K�����ѡC" );

          /* Ū�����[, ����s�b, �t�Ϋإ� */
          loge    = get_room_index( location = fread_number( pFile ) );

          if ( loge )
            mudlog( LOG_DEBUG, "Load_bus�R�a�K�����[ %d �s�b�C", location );

          if ( !( pBus->loge = create_loge( location
            , pBus->name, pBus->station->area ) ) )
          {
            mudlog( LOG_DEBUG, "Load_bus�R�إߦa�K�� %d ���ѡC", location );
          }

          /* Ū���O�� */
          if ( ( pBus->cost = fread_number( pFile ) ) <= 0 )
            mudlog( LOG_DEBUG, "Load_bus�R�a�K���O %d ���X�z�C", pBus->cost );

          fread_to_eol( pFile );

          /* �إߦ�C */
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

        /* ���� */
        if ( !str_scmp( word, "End", &fMatch ) )
        {
          Finish = TRUE;
          break;
        }
        break;

      case 'L':

        if ( !str_scmp( word , "Loge", &fMatch ) )
        {
          fread_to_eol( pFile ); /* �@�w��Ū���@�� */
          loge_descr = fread_string( pFile );
          break;
        }

        KEY( "LogeShort", loge_short, fread_string( pFile ) );
        break;

      case 'P':

        if ( !str_scmp( word , "Platform", &fMatch ) )
        {
          fread_to_eol( pFile ); /* �@�w��Ū���@�� */
          platform_descr = fread_string( pFile );
          break;
        }

        KEY( "PlatformShort", platform_short, fread_string( pFile ) );
        break;
      }

      /* ���O���A���~ */
      if ( !fMatch )
        mudlog( LOG_DEBUG , "Load_bus�R�R�O %s �����T�C" , word );

      if ( Finish ) break;
    }

    mudlog( LOG_INFO, "�t�θ��J %d �Ӧa�K��ơC" , count );
    f_close( pFile );
  }

  else
  {
    mudlog( LOG_INFO, "�S�����J����a�K����ơC" );
  }

  RETURN_NULL();
}

/* ���J§����� */
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
      /* ���� */
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

        /* ���� */
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

      /* ���O���A���~ */
      if ( !fMatch )
        mudlog( LOG_DEBUG , "Load_bounty�R�R�O %s �����T�C" , word );

      if ( Finish ) break;
    }

    mudlog( LOG_INFO, "�t�θ��J %d ���a���ơC" , top_bounty );
    f_close( pFile );
  }

  else
  {
    mudlog( LOG_INFO, "�S�����J�����a�઺��ơC" );
  }

  RETURN_NULL();
}

/* Ū���@��§����� */
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
          mudlog( LOG_DEBUG, "Load_a_bounty�R�X�{���v %d ���X�z�C", slot );

        pBounty->chance = slot;
        break;
      }

      break;

    case 'E':

      if ( !str_scmp( word, "End", &fMatch ) )
      {
        if ( !pBounty->mob )
          mudlog( LOG_DEBUG, "Load_a_bounty�R�ʥF�Ǫ����Ǹ��C" );

        if ( !pBounty->msg || !*pBounty->msg )
          mudlog( LOG_DEBUG, "Load_a_bounty�R�ʥF��X�ԭz�C" );

        if ( IS_ERROR( pBounty->chance ) )
          mudlog( LOG_DEBUG, "Load_a_bounty�R�ʥF�X�{���v�C" );

        if ( IS_ERROR( pBounty->migration ) )
          mudlog( LOG_DEBUG, "Load_a_bounty�R�ʥF�E�p���v�C" );

        if ( IS_ERROR( pBounty->type ) )
          mudlog( LOG_DEBUG, "Load_a_bounty�R�ʥF���૬�A�C" );

        if ( IS_ERROR( pBounty->max ) )
          mudlog( LOG_DEBUG, "Load_a_bounty�R�ʥF�̤j�X�{�q�C" );

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
          mudlog( LOG_DEBUG, "Load_a_bounty�R�E�p���v %d ���X�z�C", slot );

        pBounty->migration = slot;
        break;
      }

      if ( !str_scmp( word, "Mobile", &fMatch ) )
      {
        if ( !( pMob = get_mob_index( slot = fread_number( pFile ) ) ) )
          mudlog( LOG_DEBUG, "Load_a_bounty�R�a��� %d ���s�b�C", slot );

        pBounty->mob = pMob;
        break;
      }

      if ( !str_scmp( word, "Message", &fMatch ) )
      {
        fread_to_eol( pFile ); /* �@�w��Ū���@�� */
        pBounty->msg = fread_string( pFile );
        break;
      }

      if ( !str_scmp( word, "MaxNumber", &fMatch ) )
      {
        if ( ( slot = fread_number( pFile ) ) <= 0 )
          mudlog( LOG_DEBUG, "Load_a_bounty�R�̤j�ƥ� %d ���X�z�C", slot );

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
            mudlog( LOG_DEBUG, "Load_a_bounty�R�ж� %d ���s�b�C", slot );

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
          mudlog( LOG_DEBUG, "Load_a_bounty�R���૬�A %d ���X�z�C", slot );
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

    /* ���O���A���~ */
    if ( !fMatch )
      mudlog( LOG_DEBUG , "Load_a_bounty�R�R�O %s �����T�C" , word );
  }

  RETURN( NULL );
}

/* ���J§����� */
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
      /* ���� */
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

        /* ���� */
        if ( !str_scmp( word, "End" , &fMatch ) )
        {
          Finish = TRUE;
          break;
        }

        break;
      }

      /* ���O���A���~ */
      if ( !fMatch )
        mudlog( LOG_DEBUG , "Load_gift�R�R�O %s �����T�C" , word );

      if ( Finish ) break;
    }

    mudlog( LOG_INFO, "�t�θ��J %d ��§����ơC" , top_gift );
    f_close( pFile );
  }

  else
  {
    mudlog( LOG_INFO, "�S�����J����§������ơC" );
  }

  RETURN_NULL();
}

/* Ū���@��§����� */
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
          mudlog( LOG_DEBUG, "Load_a_gift�R���j�ɶ� %d ���X�z�C", level );

        pGift->duration = level;
        break;
      }

      if ( !str_scmp( word, "Days", &fMatch ) )
      {
        if ( ( level = fread_number( pFile ) ) <= 0 )
          mudlog( LOG_DEBUG, "Load_a_gift�R�|��Ѽ� %d ���X�z�C", level );

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
          mudlog( LOG_DEBUG, "Load_a_gift�R§���H�e������X�z�C" );

        switch( month )
        {
        default:
          mudlog( LOG_DEBUG, "Load_a_gift�R§���H�e������X�z�C" );
          break;

        case  1: case  3: case 5: case 7: case 8:
        case 10: case 12:

          if ( day <= 0 || day > 31 )
            mudlog( LOG_DEBUG, "Load_a_gift�R§���H�e������X�z�C" );

          break;

        case 2:

          if ( day <= 0 || day > 29 )
            mudlog( LOG_DEBUG, "Load_a_gift�R§���H�e������X�z�C" );

          break;

        case 4: case 6: case 9: case 11:

          if ( day <= 0 || day > 30 )
            mudlog( LOG_DEBUG, "Load_a_gift�R§���H�e������X�z�C" );

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
          mudlog( LOG_DEBUG, "Load_a_gift�R�ʥF�̰����šC" );

        if ( IS_ERROR( pGift->low ) )
          mudlog( LOG_DEBUG, "Load_a_gift�R�ʥF�̧C���šC" );

        if ( IS_ERROR( pGift->stamp ) )
          mudlog( LOG_DEBUG, "Load_a_gift�R�ʥF§���Ǹ��C" );

        if ( IS_ERROR( pGift->sender ) )
          mudlog( LOG_DEBUG, "Load_a_gift�R�ʥF§�����e�̡C" );

        if ( IS_ERROR( pGift->duration ) )
          mudlog( LOG_DEBUG, "Load_a_gift�R�ʥF§�����j�ɶ��C" );

        if ( IS_ERROR( pGift->days ) )
          mudlog( LOG_DEBUG, "Load_a_gift�R�ʥF§���|��ѼơC" );

        if ( IS_ERROR( pGift->day ) || IS_ERROR( pGift->month ) )
          mudlog( LOG_DEBUG, "Load_a_gift�R�ʥF§�����e����C" );

        if ( !pGift->gift )
          mudlog( LOG_DEBUG, "Load_a_gift�R�ʥF§���C" );

        if ( !pGift->title || !*pGift->title )
          mudlog( LOG_DEBUG, "Load_a_gift�R�ʥF§�����D�C" );

        if ( !pGift->message || !*pGift->message )
          mudlog( LOG_DEBUG, "Load_a_gift�R�ʥF§���T���C" );

        if ( pGift->high < pGift->low ) SWAP( pGift->high, pGift->low );

        RETURN( pGift );
      }

      break;

    case 'G':

      if ( !str_scmp( word, "Gold", &fMatch ) )
      {
        if ( ( level = fread_number( pFile ) ) < 0 )
          mudlog( LOG_DEBUG, "Load_a_gift�R§�� %d ���X�z�C", level );

        pGift->gold = level;
        break;
      }

      if ( !str_scmp( word, "Gift", &fMatch ) )
      {
        OBJ_INDEX_DATA * pIndex;

        level = fread_number( pFile );

        if ( !( pIndex = get_obj_index( level ) ) )
          mudlog( LOG_DEBUG, "Load_a_gift�R§�����X %d ���s�b�C", level );

        pGift->gift = pIndex;
        break;
      }

      break;

    case 'H':

      if ( !str_scmp( word, "High", &fMatch ) )
      {
        if ( ( level = fread_number( pFile ) ) <= 0 || level > MAX_LEVEL )
          mudlog( LOG_DEBUG, "Load_a_gift�R�̰����� %d ���X�z�C", level );

        pGift->high = level;
        break;
      }
      break;

    case 'L':

      if ( !str_scmp( word, "Low", &fMatch ) )
      {
        if ( ( level = fread_number( pFile ) ) <= 0 || level > MAX_LEVEL )
          mudlog( LOG_DEBUG, "Load_a_gift�R�̧C���� %d ���X�z�C", level );

        pGift->low = level;
        break;
      }

      break;

    case 'M':

      if ( !str_scmp( word, "Message", &fMatch ) )
      {
        fread_to_eol( pFile ); /* �@�w��Ū���@�� */
        pGift->message = fread_string( pFile );
        break;
      }

      break;

    case 'S':

      if ( !str_scmp( word, "Sender", &fMatch ) )
      {
        if ( !get_mob_index( level = fread_number( pFile ) ) )
          mudlog( LOG_DEBUG, "Load_a_gift�R§�����e�� %d ���s�b�C", level );

        pGift->sender = level;
        break;
      }

      if ( !str_scmp( word, "Stamp", &fMatch ) )
      {
        level = fread_number( pFile );

        for ( aGift = gift_list; aGift; aGift = aGift->next )
        {
          if ( aGift->stamp == level )
            mudlog( LOG_DEBUG, "Load_a_gift�R�Ǹ� %d ���ơC", level );
        }

        pGift->stamp = level;
        break;
      }
      break;

    case 'T':
      KEY( "Title", pGift->title, fread_string( pFile ) );
      break;
    }

    /* ���O���A���~ */
    if ( !fMatch )
      mudlog( LOG_DEBUG , "Load_a_gift�R�R�O %s �����T�C" , word );
  }

  RETURN( NULL );
}

/* ���J���D��� */
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
      /* ���� */
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

        /* ���� */
        if ( !str_scmp( word, "End", &fMatch ) )
        {
          Finish = TRUE;
          break;
        }

        break;
      }

      /* ���O���A���~ */
      if ( !fMatch )
        mudlog( LOG_DEBUG , "Load_question�R�R�O %s �����T�C" , word );

      if ( Finish ) break;
    }

    mudlog( LOG_INFO, "�t�θ��J %d �Ӱ��D��ơC" , top_question );
    f_close( pFile );
  }

  else
  {
    mudlog( LOG_INFO, "�S�����J������D����ơC" );
  }

  RETURN_NULL();
}

/* Ū���@�Ӹ������ */
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
          mudlog( LOG_DEBUG, "Load_a_question�R�S�����D�D�ءC" );

        for ( ans = loop = count = 0; loop < MAX_QUESTION; loop++ )
        {
          if ( pQuestion->question[loop] && *pQuestion->question[loop] )
          {
            if ( pQuestion->answer[loop] == TRUE ) ans++;
            count++;
          }
        }

        if ( count <= 1 )
          mudlog( LOG_DEBUG, "Load_a_question�R���D�Ӥ� %d�C", count );

        if ( ans <= 0 )
          mudlog( LOG_DEBUG, "Load_a_question�R�ܤ֭n���@�ӥH�W�����סC" );

        RETURN( pQuestion );
      }

      break;

    case 'Q':

      if ( !str_scmp( word, "Question", &fMatch ) )
      {
        for ( loop = 0; loop < MAX_QUESTION; loop++ )
          if ( !pQuestion->question[loop] ) break;

        if ( loop >= MAX_QUESTION )
          mudlog( LOG_DEBUG, "Load_a_question�R�Ӧh�D�ءM�L�k�x�s�C" );

        pQuestion->question[loop] = fread_string( pFile );
        pQuestion->answer[loop]   = fread_number( pFile );

        if ( !pQuestion->question[loop] || !*pQuestion->question[loop] )
          mudlog( LOG_DEBUG, "Load_a_question�R�D�ؤ�²�u�C" );

        break;
      }

      break;

    case 'T':

      if ( !str_scmp( word, "Title", &fMatch ) )
      {
        fread_to_eol( pFile ); /* �@�w��Ū���@�� */
        pQuestion->title = fread_string( pFile );
        break;
      }

      break;
    }

    /* ���O���A���~ */
    if ( !fMatch )
      mudlog( LOG_DEBUG , "Load_a_question�R�R�O %s �����T�C" , word );
  }

  RETURN( NULL );
}

/* ���J������� */
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
      /* ���� */
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

        /* ���� */
        if ( !str_scmp( word, "End", &fMatch ) )
        {
          Finish = TRUE;
          break;
        }

        break;
      }

      /* ���O���A���~ */
      if ( !fMatch )
        mudlog( LOG_DEBUG , "Load_quest�R�R�O %s �����T�C" , word );

      if ( Finish ) break;
    }

    mudlog( LOG_INFO, "�t�θ��J %d �Ӹ�����ơC" , top_quest );
    f_close( pFile );
  }

  else
  {
    mudlog( LOG_INFO, "�S�����J�����������ơC" );
  }

  RETURN_NULL();
}

/* Ū���@�Ӹ������ */
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
          mudlog( LOG_DEBUG, "Load_a_quest�R�S����������r�C" );

        if ( !pQuest->info )
          mudlog( LOG_DEBUG, "Load_a_quest�R�S�������ѻ���T�C" );

        if ( pQuest->show && !pQuest->help[0] )
          mudlog( LOG_DEBUG, "Load_a_quest�R��ܪ����ȥ����n�����ܡC" );

        RETURN( pQuest );
      }

      break;

    case 'H':

      if ( !str_scmp( word , "Help" , &fMatch ) )
      {
        fread_to_eol( pFile ); /* �@�w��Ū���@�� */
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
          mudlog( LOG_ERR, "Load_a_quest�R��������r%s���СC", pQuest->mark );

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

    /* ���O���A���~ */
    if ( !fMatch )
      mudlog( LOG_DEBUG , "Load_a_quest�R�R�O %s �����T�C" , word );
  }

  RETURN( NULL );
}

/* ���J������� */
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
      /* ���� */
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

        /* ���� */
        if ( !str_scmp( word, "End", &fMatch ) )
        {
          Finish = TRUE;
          break;
        }

        break;
      }

      /* ���O���A���~ */
      if ( !fMatch )
        mudlog( LOG_DEBUG , "Load_event�R�R�O %s �����T�C" , word );

      if ( Finish ) break;
    }

    mudlog( LOG_INFO, "�t�θ��J %d ��Ĳ�o�ƥ�C" , top_event );
    f_close( pFile );
  }

  else
  {
    mudlog( LOG_INFO, "�S�����J����Ĳ�o�ƥ�C" );
  }

  RETURN_NULL();
}

/* Ū���@��Ĳ�o�ƥ� */
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
          mudlog( LOG_DEBUG, "Load_a_event�R�X�{���v %d ���X�z�C", chance );

        pEvent->chance = chance;
        break;
      }

      break;

    case 'E':

      if ( !str_scmp( word, "End", &fMatch ) )
      {
        if ( IS_ERROR( pEvent->chance ) )
          mudlog( LOG_DEBUG, "Load_a_event�R�S���]�wĲ�o���v�C" );

        if ( !pEvent->function )
          mudlog( LOG_DEBUG, "Load_a_event�R�S��Ĳ�o�B�z��ơC" );

        if ( !pEvent->title )
          mudlog( LOG_DEBUG, "Load_a_event�R�S��Ĳ�o�W�١C" );

        if ( !pEvent->keyword )
          mudlog( LOG_DEBUG, "Load_a_event�R�ʥFĲ�o����r�C" );

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
            mudlog( LOG_DEBUG , "Load_a_event�RĲ�o�ƥ󪺨�� %s �䤣��C"
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
            mudlog( LOG_DEBUG, "Load_a_event�R����r %s ���ơC", keyword );
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

    /* ���O���A���~ */
    if ( !fMatch )
      mudlog( LOG_DEBUG , "Load_a_event�R�R�O %s �����T�C" , word );
  }

  RETURN( NULL );
}

/* ���J����� */
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
      /* ���� */
      case '*':
        fMatch = TRUE;
        fread_to_eol( pFile );
        break;

      case '#':

        if ( !str_scmp( word, "#Ship", &fMatch ) )
        {
          /* �t�m�O����H�γ]�w�w�]�� */
          pShip = load_a_ship( pFile );

          pShip->next = ship_list;
          ship_list   = pShip;

          if ( !fReload ) top_ship++;
          pShip->waiting_tick = pShip->waiting;
          break;
        }

        break;

      case 'E':

        /* ���� */
        if ( !str_scmp( word, "End", &fMatch ) )
        {
          Finish = TRUE;
          break;
        }

        break;
      }

      /* ���O���A���~ */
      if ( !fMatch )
        mudlog( LOG_DEBUG , "Load_ship�R�R�O %s �����T�C", word );

      if ( Finish ) break;
    }

    mudlog( LOG_INFO, "�t�θ��J %d �Ӵ���ơC" , top_ship );
    f_close( pFile );
  }

  else
  {
    mudlog( LOG_INFO, "�S�����J�������ơC" );
  }

  RETURN_NULL();
}

/* Ū���@������� */
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
          mudlog( LOG_DEBUG, "Load_a_ship�R����ơC" );

        if ( get_room_index( slot = fread_number( pFile ) ) )
          mudlog( LOG_DEBUG, "Load_a_ship�R������s�b�C", slot );

        if ( !pShip->description )
          mudlog( LOG_DEBUG, "Load_a_ship�R�S����W�١C" );

        if ( !pShip->description )
          mudlog( LOG_DEBUG, "Load_a_ship�R�S����ԭz�C" );

        set_room_default( pRoom = alloc_struct( STRUCT_ROOM_INDEX_DATA ) );

        pRoom->people          = NULL;
        pRoom->contents        = NULL;
        pRoom->extra_descr     = NULL;
        pRoom->club            = NULL;
        pRoom->job             = NULL;
        pRoom->area            = DefaultArea;
        pRoom->name            = str_dup( pShip->name );
        pRoom->description     = str_dup( pShip->description );
        pRoom->filename        = str_dup( "�t�Ϋإ�" );
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
          mudlog( LOG_DEBUG, "Load_a_ship�R��O %d ���X�z�C", slot );

        pShip->cost = slot;
        break;
      }

      break;

    case 'D':

      if ( !str_scmp( word , "Description", &fMatch ) )
      {
        if ( pShip->description )
          mudlog( LOG_DEBUG, "Load_a_ship�R��ԭz���ơC" );

        fread_to_eol( pFile ); /* �@�w��Ū���@�� */
        pShip->description = fread_string( pFile );
        break;
      }

      if ( !str_scmp( word, "Destination" , &fMatch ) )
      {
        if ( pShip->destination )
          mudlog( LOG_DEBUG, "Load_a_ship�R�ت��a���ơC" );

        if ( !( pRoom = get_room_index( slot = fread_number( pFile ) ) ) )
          mudlog( LOG_DEBUG, "Load_a_ship�R�䤣��ت��a�I %d�C", slot );

        for ( aShip = ship_list; aShip; aShip = aShip->next )
        {
          if ( aShip->destination == pRoom )
            mudlog( LOG_DEBUG, "Load_a_ship�R�ت��a %d ���ơC", slot );
        }

        pShip->destination = pRoom;
        break;
      }

      if ( !str_scmp( word, "Delay", &fMatch ) )
      {
        if ( ( slot = fread_number( pFile ) ) < 0 || slot >= 10000 )
          mudlog( LOG_DEBUG, "Load_a_ship�R�������v %d ���X�z�C", slot );

        pShip->delay = slot;
        break;
      }

      break;

    case 'E':

      if ( !str_scmp( word, "End", &fMatch ) )
      {
        if ( !pShip->name )
          mudlog( LOG_DEBUG, "Load_a_ship�R�S�����W�١C" );

        if ( !pShip->msg_entrance )
          mudlog( LOG_DEBUG, "Load_a_ship�R�S���a���ԭz�C" );

        if ( !pShip->msg_land )
          mudlog( LOG_DEBUG, "Load_a_ship�R�S���J��ԭz�C" );

        if ( !pShip->description )
          mudlog( LOG_DEBUG, "Load_a_ship�R�S����ԭz�C" );

        if ( !pShip->starting )
          mudlog( LOG_DEBUG, "Load_a_ship�R�S���X�o�I�C" );

        if ( !pShip->destination )
          mudlog( LOG_DEBUG, "Load_a_ship�R�S���ت��a�C" );

        if ( !pShip->cabin )
          mudlog( LOG_DEBUG, "Load_a_ship�R�S����C" );

        if ( IS_ERROR( pShip->cost ) )
          mudlog( LOG_DEBUG, "Load_a_ship�R�S���O�ΡC" );

        if ( IS_ERROR( pShip->sailing ) )
          mudlog( LOG_DEBUG, "Load_a_ship�R�S�����ɶ��C" );

        if ( IS_ERROR( pShip->waiting ) )
          mudlog( LOG_DEBUG, "Load_a_ship�R�S�����ݮɶ��C" );

        RETURN( pShip );
      }

      if ( !str_scmp( word , "Entrance", &fMatch ) )
      {
        if ( pShip->msg_entrance )
          mudlog( LOG_DEBUG, "Load_a_ship�R�a��ԭz���ơC" );

        fread_to_eol( pFile ); /* �@�w��Ū���@�� */
        pShip->msg_entrance = fread_string( pFile );
        break;
      }

      break;

    case 'L':

      if ( !str_scmp( word , "Land", &fMatch ) )
      {
        if ( pShip->msg_land )
          mudlog( LOG_DEBUG, "Load_a_ship�R�J��ԭz���ơC" );

        fread_to_eol( pFile ); /* �@�w��Ū���@�� */
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
          mudlog( LOG_DEBUG, "Load_a_ship�R���s�X�{���v %d ���X�z�C", slot );

        pShip->pirate = slot;
        break;
      }

      break;

    case 'S':

      if ( !str_scmp( word, "Sailing", &fMatch ) )
      {
        if ( ( slot = fread_number( pFile ) ) <= 0 )
          mudlog( LOG_DEBUG, "Load_a_ship�R���ɶ� %d ���X�z�C", slot );

        pShip->sailing = slot;
        break;
      }

      if ( !str_scmp( word, "Starting", &fMatch ) )
      {
        if ( pShip->starting )
          mudlog( LOG_DEBUG, "Load_a_ship�R�X�o�I���ơC" );

        if ( !( pRoom = get_room_index( slot = fread_number( pFile ) ) ) )
          mudlog( LOG_DEBUG, "Load_a_ship�R�䤣��X�o�I %d�C", slot );

        for ( aShip = ship_list; aShip; aShip = aShip->next )
        {
          if ( aShip->starting == pRoom )
            mudlog( LOG_DEBUG, "Load_a_ship�R�X�o�I %d ���ơC", slot );
        }

        pShip->starting = pRoom;
        break;
      }

      break;

    case 'W':

      if ( !str_scmp( word, "Waiting", &fMatch ) )
      {
        if ( ( slot = fread_number( pFile ) ) <= 0 )
          mudlog( LOG_DEBUG, "Load_a_ship�R���ݴ��ɶ� %d ���X�z�C", slot );

        pShip->waiting = slot;
        break;
      }
      break;
    }

    /* ���O���A���~ */
    if ( !fMatch )
      mudlog( LOG_DEBUG , "Load_a_ship�R�R�O %s �����T�C" , word );
  }

  RETURN( NULL );
}

/* Ū�����~�ɮ׸�� */
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
      /* ���� */
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

        /* ���� */
        if ( !str_scmp( word, "End", &fMatch ) )
        {
          Finish = TRUE;
          break;
        }

        break;
      }

      /* ���O���A���~ */
      if ( !fMatch )
        mudlog( LOG_DEBUG , "Load_situs�R�R�O %s �����T�C" , word );

      if ( Finish ) break;
    }

    mudlog( LOG_INFO, "�t�θ��J %d �ӳ����ơC" , top_situs );
    f_close( pFile );

    for ( pSitus = situs_list; pSitus; pSitus = pSitus->next )
    {
      if ( pSitus->associate_vnum != ERRORCODE )
      {
        for ( zSitus = situs_list; zSitus; zSitus = zSitus->next )
          if ( zSitus->location == pSitus->associate_vnum ) break;

        if ( !zSitus || zSitus == pSitus )
          mudlog( LOG_DEBUG, "Load_situs�R%d �����p�����~�C", pSitus->location );

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
    mudlog( LOG_INFO, "�S�����J���󳡦쪺��ơC" );
  }

  RETURN_NULL();
}

/* Ū���@�ӳ����� */
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
          mudlog( LOG_DEBUG, "Load_a_situs�R�����m���]�w�C" );

        if ( IS_ERROR( pSitus->chance ) || pSitus->chance < 0 )
          mudlog( LOG_DEBUG, "Load_a_situs�R������v���]�w�C" );

        if ( IS_ERROR( pSitus->wear ) )
          mudlog( LOG_DEBUG, "Load_a_situs�R�����ۺX�Х��]�w�C" );

        if ( IS_ERROR( pSitus->attack ) )
          mudlog( LOG_DEBUG, "Load_a_situs�R��������X�Х��]�w�C" );

        if ( !pSitus->eq_name || !*pSitus->eq_name )
          mudlog( LOG_DEBUG, "Load_a_situs�R����˳ƦW�٥��]�w�C" );

        if ( !pSitus->situs_name || !*pSitus->situs_name )
          mudlog( LOG_DEBUG, "Load_a_situs�R����W�٥��]�w�C" );

        if ( !pSitus->short_descr || !*pSitus->short_descr )
          mudlog( LOG_DEBUG, "Load_a_situs�R����W�٥��]�w�C" );

        if ( !pSitus->name || !*pSitus->name )
          mudlog( LOG_DEBUG, "Load_a_situs�R����W�٥��]�w�C" );

        if ( !pSitus->msg1 || !*pSitus->msg1 )
          mudlog( LOG_DEBUG, "Load_a_situs�R�����۱ԭz�@���]�w�C" );

        if ( !pSitus->msg2 || !*pSitus->msg2 )
          mudlog( LOG_DEBUG, "Load_a_situs�R�����۱ԭz�G���]�w�C" );

        if ( pSitus->apply_ac < 0 )
          mudlog( LOG_DEBUG, "Load_a_situs�R���쨾�m�[�j����(%d)���X�k�C"
             , pSitus->apply_ac );

        if ( pSitus->attack == ATTACK_RANDOM )
          mudlog( LOG_DEBUG, "Load_a_situs�R��������X�Ф��ର %d�C", ATTACK_RANDOM );

        if ( pSitus->wear == WEAR_NONE )
          mudlog( LOG_DEBUG, "Load_a_situs�R���������m���ର %d�C", WEAR_NONE );

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

    /* ���O���A���~ */
    if ( !fMatch )
      mudlog( LOG_DEBUG , "Load_a_situs�R�R�O %s �����T�C" , word );
  }

  RETURN( NULL );
}

/* Ū�����~�ɮ׸�� */
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
      /* ���� */
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
            mudlog( LOG_DEBUG , "Load_sale�R�����v %d ���X�k�C", SaleChance );
          break;
        }

        break;

      case 'E':

        /* ���� */
        if ( !str_scmp( word, "End", &fMatch ) )
        {
          if ( SaleChance <= 0 )
            mudlog( LOG_DEBUG , "Load_sale�R�����v %d ���X�k�C", SaleChance );

          Finish = TRUE;
          break;
        }

        break;

      case 'L':

        if ( !str_scmp( word, "Limit", &fMatch ) )
        {
          if ( ( SaleLimit = fread_number( pFile ) ) <= 0 )
            mudlog( LOG_DEBUG , "Load_sale�R���U���H�� %d ���X�k�C", SaleLimit );

          break;
        }

        break;

      }

      /* ���O���A���~ */
      if ( !fMatch )
        mudlog( LOG_DEBUG , "Load_sale�R�R�O %s �����T�C" , word );

      if ( Finish ) break;
    }

    mudlog( LOG_INFO, "�t�θ��J %d �ө��~�C" , top_sale );
    f_close( pFile );
  }

  else
  {
    mudlog( LOG_INFO, "�S�����J������~����ơC" );
  }

  RETURN_NULL();
}

/* Ū���@�ө��~��� */
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
          mudlog( LOG_DEBUG, "Load_a_sale�R���~��� %d ���X�z�C"
            , pSale->cost );

        break;
      }

      break;

    case 'E':

      if ( !str_scmp( word, "End", &fMatch ) )
      {
        if ( !pSale->obj ) mudlog( LOG_DEBUG, "Load_a_sale�R�S�����~�C" );

        if ( pSale->obj->Takeable == FALSE )
          mudlog( LOG_DEBUG, "Load_a_sale�R���~���O�i�H���T" );

        if ( IS_ERROR( pSale->cost ) )
          mudlog( LOG_DEBUG, "Load_a_sale�R���~�S������T" );

        RETURN( pSale );
      }

      break;

    case 'O':

      if ( !str_scmp( word, "Object", &fMatch ) )
      {
        if ( !( pIndex = get_obj_index( vnum = fread_number( pFile ) ) ) )
          mudlog( LOG_DEBUG, "Load_a_sale�R�S�����~���X %d �C", vnum );

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

    /* ���O���A���~ */
    if ( !fMatch )
      mudlog( LOG_DEBUG , "Load_a_sale�R�R�O %s �����T�C" , word );
  }

  RETURN( NULL );
}

/* ���J���ڸ�� */
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
      /* ���� */
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

        /* ���� */
        if ( !str_scmp( word, "End", &fMatch ) )
        {
          Finish = TRUE;
          break;
        }

        break;
      }

      /* ���O���A���~ */
      if ( !fMatch )
        mudlog( LOG_DEBUG , "Load_immlist�R�R�O %s �����T�C" , word );

      if ( Finish ) break;
    }

    mudlog( LOG_INFO, "�t�θ��J %d �ӯ��ڸ�ơC" , top_immlist );
    f_close( pFile );
  }

  else
  {
    mudlog( LOG_INFO, "�S�����J���󯫱ڪ���ơC" );
  }

  RETURN_NULL();
}

/* Ū���@�ӯ��ڸ�� */
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
          mudlog( LOG_DEBUG, "Load_a_immlist�R�U�ݸ�椣�X�z�C" );

        pImmlist->adviser = adviser;
        break;
      }
      break;

    case 'D':

      if ( !str_scmp( word , "Description", &fMatch ) )
      {
        fread_to_eol( pFile ); /* �@�w��Ū���@�� */
        pImmlist->description = fread_string( pFile );
        break;
      }

      break;

    case 'E':

      if ( !str_scmp( word, "End", &fMatch ) )
      {
        if ( !pImmlist->name )
          mudlog( LOG_DEBUG, "Load_a_immlist�R�S�����ڦW�١C" );

        if ( !pImmlist->description )
          mudlog( LOG_DEBUG, "Load_a_immlist�R�S���ѻ��ԭz�C" );

        if ( IS_ERROR( pImmlist->level ) && IS_ERROR( pImmlist->trust ) )
          mudlog( LOG_DEBUG, "Load_a_immlist�R�S�����ũάO�H�൥�šC" );

        RETURN( pImmlist );
      }

      break;

    case 'L':

      if ( !str_scmp( word, "Level", &fMatch ) )
      {
        int level;

        level = fread_number( pFile );
        if ( level <= LEVEL_HERO || level > MAX_LEVEL )
          mudlog( LOG_DEBUG, "Load_a_immlist�R���Ť��X�z�C" );

        pImmlist->level = level;
        break;
      }
      break;

    case 'N':

      if ( !str_scmp( word, "Name", &fMatch ) )
      {
        pImmlist->name = fread_string( pFile );

        if ( !is_exist( pImmlist->name ) )
          mudlog( LOG_DEBUG, "Load_a_immlist�R�W��%s���s�b�C", pImmlist->name );

        break;
      }
      break;

    case 'T':

      if ( !str_scmp( word, "Trust", &fMatch ) )
      {
        int trust;

        trust = fread_number( pFile );
        if ( trust <= LEVEL_HERO || trust > MAX_LEVEL )
          mudlog( LOG_DEBUG, "Load_a_immlist�R�H�൥�Ť��X�z�C" );

        pImmlist->trust = trust;
        break;
      }
      break;
    }

    /* ���O���A���~ */
    if ( !fMatch )
      mudlog( LOG_DEBUG , "Load_a_immlist�R�R�O %s �����T�C" , word );
  }

  RETURN( NULL );
}

/* ���J�Ѳ���� */
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

  /* ���M���}�C��� */
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
      /* ���� */
      case '*':
        fMatch = TRUE;
        fread_to_eol( pFile );
        break;

      case 'E':

        /* ���� */
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
            mudlog( LOG_DEBUG, "Load_stock�R���X %d ���~�C", slot );

          if ( stock_data[slot].name )
            mudlog( LOG_DEBUG, "Load_stock�R���X %d ���ơC", slot );

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

      /* ���O���A���~ */
      if ( !fMatch )
        mudlog( LOG_DEBUG , "Load_stock�R�R�O %s �����T�C" , word );

      if ( Finish ) break;
    }

    mudlog( LOG_INFO, "�t�θ��J %d �ӪѲ���ơC" , count );
    f_close( pFile );
  }

  else
  {
    mudlog( LOG_INFO, "�S�����J����Ѳ�����ơC" );
  }

  RETURN_NULL();
}

/* ���J������ */
void load_date( const char * filename )
{
  FILE_DATA  * pFile;
  char       * word;
  bool         fMatch;
  bool         Finish = FALSE;
  int          loop;

  PUSH_FUNCTION( "load_date" );

  /* ���M����� */
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
      /* ���� */
      case '*':
        fMatch = TRUE;
        fread_to_eol( pFile );
        break;

      case 'D':

        if ( !str_scmp( word, "DayName", &fMatch ) )
        {
          if ( DaysPerMonth >= MAX_DAY )
            mudlog( LOG_ERR, "Load_date�R����ƥؤw�W�L�t�γ]�w�C" );

          DayName[DaysPerMonth++] = fread_string( pFile );
          break;
        }
        break;

      case 'E':

        /* ���� */
        if ( !str_scmp( word, "End", &fMatch ) )
        {
          if ( DaysPerMonth <= 0 || MonthsPerYear <= 0 || HoursPerDay != 24 )
            mudlog( LOG_ERR, "Load_date�R�ʥF�����ơC" );

          Finish = TRUE;
          break;
        }

        break;

      case 'H':

        if ( !str_scmp( word, "HourName", &fMatch ) )
        {
          if ( HoursPerDay >= MAX_HOUR )
            mudlog( LOG_ERR, "Load_date�R�ɨ��ƥؤw�W�L�t�γ]�w�C" );

          HourName[HoursPerDay++] = fread_string( pFile );
          break;
        }
        break;

      case 'M':

        if ( !str_scmp( word, "MonthName", &fMatch ) )
        {
          if ( MonthsPerYear >= MAX_MONTH )
            mudlog( LOG_ERR, "Load_date�R����ƥؤw�W�L�t�γ]�w�C" );

          MonthName[MonthsPerYear++] = fread_string( pFile );
          break;
        }
        break;
      }

      /* ���O���A���~ */
      if ( !fMatch )
        mudlog( LOG_DEBUG , "Load_date�R�R�O %s �����T�C" , word );

      if ( Finish ) break;
    }

    f_close( pFile );
  }

  else
  {
    mudlog( LOG_ERR, "Load_date�R�S�����J����������ơC" );
  }

  RETURN_NULL();
}

/* ���J�ɯŸ�Ƹ�� */
void load_promotion( const char * filename )
{
  FILE_DATA * pFile;
  char * word;
  bool   fMatch;
  int    loop;
  int    level;
  int    count;

  PUSH_FUNCTION( "load_promotion" );

  /* ���M���}�C��� */
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
      /* ���� */
      case '*':

        fMatch = TRUE;
        fread_to_eol( pFile );
        break;

      case 'E':

        /* ���� */
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
            mudlog( LOG_DEBUG, "Load_promotion�R���� %d ���X�z�C", level );

          if ( ( count = fread_number( pFile ) ) < 0 )
            mudlog( LOG_DEBUG, "Load_promotion�R�ƥ� %d ���X�z�C", count );

          kill_table[level].promotion = count;

          if ( ( count = fread_number( pFile ) ) < 0 )
            mudlog( LOG_DEBUG, "Load_promotion�R�g��� %d ���X�z�C", count );

          kill_table[level].exp = count;

          kill_table[level].damage = fread_number( pFile );
          kill_table[level].dodge  = fread_number( pFile );

          break;
        }

        break;
      }

      /* ���O���A���~ */
      if ( !fMatch )
        mudlog( LOG_DEBUG , "Load_promotion�R�R�O %s �����T�C" , word );
    }

    f_close( pFile );
  }

  else
  {
    mudlog( LOG_DEBUG, "Load_promotion�R�S����������ɮסC" );
  }

  RETURN_NULL();
}

/* ���J�^����� */
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
      /* ���� */
      case '*':
        fMatch = TRUE;
        fread_to_eol( pFile );
        break;

      case 'E':

        /* ���� */
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

      /* ���O���A���~ */
      if ( !fMatch )
        mudlog( LOG_DEBUG , "Load_hero�R�R�O %s �����T�C" , word );

      if ( Finish ) break;
    }

    mudlog( LOG_INFO, "�t�θ��J %d �ӭ^����ơC" , count );
    f_close( pFile );
    update_hero();
  }

  else
  {
    mudlog( LOG_INFO, "�S�����J����^������ơC" );
  }

  RETURN_NULL();
}

/* ���J�Ҧ����a��Ʈw��� */
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
      /* ���� */
      case '*':
        fMatch = TRUE;
        fread_to_eol( pFile );
        break;

      case 'E':

        /* ���� */
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

      /* ���O���A���~ */
      if ( !fMatch )
        mudlog( LOG_DEBUG , "Load_database�R�R�O %s �����T�C" , word );

      if ( Finish ) break;
    }

    mudlog( LOG_INFO, "�t�θ��J %d �Ӹ�Ʈw��ơC" , count );
    f_close( pFile );
  }

  else
  {
    mudlog( LOG_INFO, "�S�����J�����Ʈw����ơC" );
  }

  RETURN_NULL();
}


/* ���J�ػP��� */
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
      /* ���� */
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

        /* ���� */
        if ( !str_scmp( word, "End", &fMatch ) )
        {
          if ( DonateBenifit < 0 )
            mudlog( LOG_ERR, "Load_donate�R���٪����X�k�C" );

          if ( DonateMoney < 0 )
            mudlog( LOG_ERR, "Load_donate�R���٪��`���B���X�k�C" );

          if ( DonateLevel < 0 )
            mudlog( LOG_ERR, "Load_donate�R���ٵ��Ť��X�k�C" );

          if ( DonateLimit < 0 )
            mudlog( LOG_ERR, "Load_donate�R���ٹ�H���X�k�C" );

          if ( DonateDuration < 0 )
            mudlog( LOG_ERR, "Load_donate�R���ٶ��j���X�k�C" );

          if ( DonateCount < 0 )
            mudlog( LOG_ERR, "Load_donate�R���٦��Ƥ��X�k�C" );

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

      /* ���O���A���~ */
      if ( !fMatch )
        mudlog( LOG_DEBUG , "Load_donate�R�R�O %s �����T�C", word );

      if ( Finish ) break;
    }

    f_close( pFile );
  }

  else
  {
    mudlog( LOG_INFO, "�S�����J�����ػP����ơC" );
    DonateLock = TRUE;
  }

  RETURN_NULL();
}

/* Ū��������� */
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
      /* ���� */
      case '*':
        fMatch = TRUE;
        fread_to_eol( pFile );
        break;

      case '#':

        if ( !str_scmp( word, "#CLUB", &fMatch ) )
        {
          if ( club_count() >= max_club )
            mudlog( LOG_ERR, "Load_club�R�����ƥضW�X���w�ȡC" );

          pClub = load_a_club( pFile );

          /* �Y�����s�p�W�L 20 �ѡM�άO�S�����D�h�R���� */
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

            /* �إ߰_�����ж� */
            club_location( pClub );
          }

          break;
        }

        break;

      case 'E':

        /* ���� */
        if ( !str_scmp( word, "End", &fMatch ) )
        {
          Finish = TRUE;
          break;
        }
        break;
      }

      /* ���O���A���~ */
      if ( !fMatch ) mudlog( LOG_DEBUG , "Load_club�R�R�O %s �����T�C" , word );
      if ( Finish ) break;
    }

    f_close( pFile );

    for ( pClub = club_list; pClub; pClub = pClub->next ) club_sort( pClub );

    save_club();
  }

  else
  {
    mudlog( LOG_INFO, "�S�����J������������ơC" );
  }

  RETURN_NULL();
}

/* Ū���@��������� */
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

        /* �w�����a�W�٤Ӫ� */
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
          mudlog( LOG_ERR, "Load_a_club�R�S���^�������W�١C" );

        if ( !pClub->cname )
          mudlog( LOG_ERR, "Load_a_club�R�S����������W�١C" );

        if ( !pClub->master )
          mudlog( LOG_ERR, "Load_a_club�R�S�������D�H�W�١C" );

        if ( pClub->status == CLUB_STATUS_UNKNOW )
          mudlog( LOG_ERR, "Load_a_club�R���A %d ���X�z�C", pClub->status );

        RETURN( pClub );
      }

      break;

    case 'F':

      if ( !str_scmp( word, "Follower", &fMatch ) )
      {
        pName = fread_string( pFile );

        /* �w�����a�W�٤Ӫ� */
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

        /* �w�����a�W�٤Ӫ� */
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
          mudlog( LOG_DEBUG , "Load_a_club�R���A %d �����T�C" , status );
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

        /* �w�����a�W�٤Ӫ� */
        if ( str_len( pName ) <= NAME_LENGTH && is_exist( pName ) )
          pClub->vicemaster = str_dup( pName );

        free_string( pName );
        break;
      }

      break;
    }

    /* ���O���A���~ */
    if ( !fMatch )
      mudlog( LOG_DEBUG , "Load_a_club�R�R�O %s �����T�C" , word );
  }

  RETURN( NULL );
}

/* ���J�ݸ���� */
ENQUIRE_DATA * load_enquire( FILE_DATA * pFile )
{
  ENQUIRE_DATA * pEnquire;
  char         * word;
  bool           fMatch;

  PUSH_FUNCTION( "load_enquire" );

  /* �t�m�O����H�γ]�w�w�]�� */
  set_enquire_default( pEnquire = alloc_struct( STRUCT_ENQUIRE_DATA ) );

  for ( ;; )
  {
    word   = if_eof( pFile ) ? "End" : fread_word( pFile );
    fMatch = FALSE;

    switch ( UPPER( word[0] ) )
    {
    /* ���� */
    case '*':
      fMatch = TRUE;
      fread_to_eol( pFile );
      break;

    case 'E':

      if ( !str_scmp( word, "End", &fMatch ) )
      {
        if ( !pEnquire->keyword )
          mudlog( LOG_DEBUG, "Load_enquire�R�S���]�w�ݸ�������r�C" );

        RETURN( pEnquire );
      }

      break;

    case 'K':

      KEY( "Keyword", pEnquire->keyword, fread_string( pFile ) );
      break;
    }

    /* ���O���A���~ */
    if ( !fMatch ) mudlog( LOG_DEBUG , "Load_pEnquire�R�R�O %s �����T�C", word );
  }

  RETURN( pEnquire );
}

/* ���JĲ�o�ʧ@ */
JOB_DATA * load_job( FILE_DATA * pFile )
{
  JOB_DATA * pJob;
  char     * word;
  bool       fMatch;

  PUSH_FUNCTION( "load_job" );

  /* �t�m�O����H�γ]�w�w�]�� */
  set_job_default( pJob = alloc_struct( STRUCT_JOB_DATA ) );

  for ( ;; )
  {
    word   = if_eof( pFile ) ? "End" : fread_word( pFile );
    fMatch = FALSE;

    switch ( UPPER( word[0] ) )
    {
    /* ���� */
    case '*':
      fMatch = TRUE;
      fread_to_eol( pFile );
      break;

    case 'E':

      if ( !str_scmp( word, "End", &fMatch ) )
      {
        if ( !pJob->keyword )
        {
          mudlog( LOG_DEBUG, "Load_job�R�S���]�wĲ�o������C" );
          break;
        }

        if ( !pJob->function )
        {
          mudlog( LOG_DEBUG, "Load_job�R�S���]�wĲ�o����ơC" );
          break;
        }

        if ( IS_ERROR( pJob->position ) )
        {
          mudlog( LOG_DEBUG, "Load_job�R�S���]�wĲ�o�����A�C" );
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
          mudlog( LOG_DEBUG, "Load_job�R�䤣��ާ@��� %s�C", func );
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
          mudlog( LOG_DEBUG, "Load_job�R���~�����A %d�C", pJob->position );
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

    /* ���O���A���~ */
    if ( !fMatch || FileError )
    {
      if ( !fMatch ) mudlog( LOG_DEBUG , "Load_job�R�R�O %s �����T�C", word );
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

  /* �t�m�O����H�γ]�w�w�]�� */
  set_mprog_default( pProg = alloc_struct( STRUCT_MPROG_DATA ) );

  for ( ;; )
  {
    word   = if_eof( pFile ) ? "End" : fread_word( pFile );
    fMatch = FALSE;

    switch ( UPPER( word[0] ) )
    {
    /* ���� */
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
          mudlog( LOG_DEBUG, "Load_program�R���v %d ���X�z�C", chance );
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
          mudlog( LOG_DEBUG, "Load_program�R�ʥF�{�������A�C" );
          break;
        }

        if ( !pProg->comlist || !*pProg->comlist )
        {
          mudlog( LOG_DEBUG, "Load_program�R�ʥF�{�������峡���C" );
          break;
        }

        if ( !pProg->arglist || !*pProg->arglist )
        {
          mudlog( LOG_DEBUG, "Load_program�R�ʥF�{�����ѼƳ����C" );
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
          mudlog( LOG_DEBUG, "Load_program�R���� %d ���X�z�C", chance );
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
          mudlog( LOG_DEBUG, "Load_program�R���J�{�����A %s ���~�C", pString );
          break;
        }

        break;
      }

      if ( !str_scmp( word, "Text", &fMatch ) )
      {
        fread_to_eol( pFile ); /* �@�w��Ū���@�� */
        pProg->comlist = fread_string( pFile );
        break;
      }

      break;
    }

    /* ���O���A���~ */
    if ( !fMatch || FileError )
    {
      if ( !fMatch ) mudlog( LOG_DEBUG , "Load_prog�R�R�O %s �����T�C", word );
      if ( fReload )
      {
        free_mob_program( pProg );
        RETURN( NULL );
      }
    }
  }

  RETURN( NULL );
}

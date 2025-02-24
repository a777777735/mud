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
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include "merc.h"

/* �����ܼ� */
NET_DATA      *         net_list;

AUCTION_DATA            pAuction;
AUCTION_DATA *          auction_info = &pAuction;

FIGHT_DATA              Fight_Data;
FIGHT_DATA   *          fight_info = &Fight_Data;

HELP_DATA *             help_first;
HELP_DATA *             help_last;

CLUB_DATA *             club_list;
BUS_DATA  *             bus_first;
BUS_DATA  *             bus_last;

HERO_DATA *             hero_first;
HERO_DATA *             hero_last;

SHOP_DATA *             shop_first;
SHOP_DATA *             shop_last;

AREA_DATA *             area_first;
AREA_DATA *             area_last;

XNAMES_DATA *           xnames_first;
GREETING_DATA *         greeting_list;
SOCIAL_DATA *           social_first;
SOCIAL_DATA *           social_last;

ANGEL_DATA  *           angel_list;
BAN_DATA *              ban_list;
TOP_DATA *              top_list;
SYMBOL_DATA *           symbol_list;
SERVER_DATA *           server_list;
IMMLIST_DATA *          imm_list;
IMMLIST_DATA *          imm_last;

SALE_DATA    *          sale_list;
SITUS_DATA   *          situs_list;
WANTED_DATA  *          wanted_list;
VARIABLE_DATA *         variable_list;
SHIP_DATA    *          ship_list;
QUEST_INFO *            quest_list;
QUESTION_DATA *         question_list;
GIFT_DATA *             gift_list;
BOUNTY_DATA *           bounty_list;
EVENT_DATA *            event_list;

SECTOR_DATA *           sector_list;
SECTOR_DATA *           DefaultSector;

CLASS_DATA *            class_list;
CLASS_DATA *            class_demos;
CLASS_DATA *            class_force;
LIQ_DATA *              liq_list;
LIQ_DATA *              liq_water;
CHAR_DATA *             char_list;
SKILL_DATA *            skill_list;
SKILL_DATA *            skill_last;
ADDRESS_DATA *          address_list;
DATABASE_DATA *         database_list;
DATABASE_DATA *         database_last;
CMD_DATA *              cmd_list;
CMD_DATA *              cmd_last;
KILL_DATA               kill_table      [MAX_LEVEL];
NOTE_DATA *             note_list;
VOTE_DATA *             vote_list;
JOKE_DATA *             joke_list;

BOARD_DATA *            board_list;
BOARD_DATA *            board_last;

OBJ_DATA *              object_list;
TIME_INFO_DATA          time_info;
WEATHER_DATA            weather_info;
AUCTION_DATA *          auction_info;
SYMBOL_DATA *           symbol_hash     [26][20];
PW_DATA     *           pw_list;
GR_DATA     *           gr_list;

FILE * fpReserve;
char   str_empty       [1];
int    RecallPlace     [MAX_RECALL];
int    iInnate         [MAX_INNATE];
int    nInnate;
int    aInnate;

CLASS_DATA * iClass    [MAX_DEFAULT_CLASS];

OBJ_INDEX_DATA  *       obj_index_hash          [MAX_KEY_HASH];
ROOM_INDEX_DATA *       room_index_hash         [MAX_KEY_HASH];
MOB_INDEX_DATA  *       mob_index_hash          [MAX_KEY_HASH];
DATABASE_DATA   *       data_index_hash         [MAX_KEY_HASH];
STOCK_DATA              stock_data              [MAX_STOCK];

/* �ϰ��ܼ� */
char * welcome_message;
char * welcome_immortal;
char * motd_message;
int    top_affect     =  0;
int    top_restrict   =  0;
int    top_area       =  0;
int    top_board      =  0;
int    top_ed         =  0;
int    top_exit       =  0;
int    top_help       =  0;
int    top_vote       =  0;
int    top_joke       =  0;
int    top_angel      =  0;
int    top_sector     =  0;
int    top_class      =  0;
int    top_liq        =  0;
int    top_mob_index  =  0;
int    top_obj_index  =  0;
int    top_reset      =  0;
int    top_room       =  0;
int    top_shop       =  0;
int    top_mineral    =  0;
int    top_social     =  0;
int    top_teach      =  0;
int    top_greeting   =  0;
int    top_skill      =  0;
int    top_cmd        =  0;
int    top_quest      =  0;
int    top_question   =  0;
int    top_event      =  0;
int    top_gift       =  0;
int    top_bounty     =  0;
int    top_immlist    =  0;
int    top_sale       =  0;
int    top_situs      =  0;
int    top_ship       =  0;
int    merc_id        =  0;
pid_t  merc_pid       =  0;
bool   fBootDb;
bool   arena_busy     = FALSE;
bool   fqdn_limit     = FALSE;
size_t hold_day       = PLAYER_HOLD_DAY;
size_t file_quota     = PLAYER_FILE_QUOTA;
int    FileMode       = S_IRUSR | S_IWUSR;
bool   SetFileMode    = FALSE;
bool   GroupSpiltExp  = FALSE;
bool   AttackValue    = FALSE;
bool   SkillValue     = FALSE;
bool   PlayerAngel    = TRUE;
int    MobDecrease    = 20;

/* �ϰ��� */
void    check_skill             args( ( void ) );
void    check_area_capital      args( ( void ) );
void    check_class             args( ( void ) );
void    check_ironclad          args( ( void ) );
void    fix_exits               args( ( void ) );
void    fix_recall              args( ( void ) );
void    load_innate             args( ( void ) );
void    fix_reborn_number       args( ( void ) );
bool    symbol_lookup           args( ( char * , int * ) );
void    check_skill_innate      args( ( void ) );
void    check_room              args( ( void ) );
void    check_object            args( ( void ) );
void    check_mobile            args( ( void ) );
void    weather_setting         args( ( void ) );
void    fix_object_value        args( ( void ) );
void    init_mask               args( ( void ) );

void boot_db( void )
{
  int loop;

  PUSH_FUNCTION( "boot_db" );

  /* ��l�ưt�m�Ŷ��ܼ� */
  for ( loop = 0; loop < MAX_MEM_LIST; loop++ ) rgFreeList[loop] = NULL;
  fBootDb = TRUE;

  /* �ҩl�Ʃ���ܼ� */
  init_auction();

  /* �_�l�Ƭy���Ǹ� */
  initial_serial_time();

  /* �ҩl�ƽ�� */
  initial_gamble();

  /* �ҩl�ƶü� */
  init_mm();

  /* �_�l�Ʊm�� */
  generate_ticket( TicketTotal );

  /* �_�l�ֳz */
  lotto_generate( -1 );

  /* ��Ȥƪ�� */
  tablize( FORMAT_CLEAN, NULL, 0, NULL, 0 );

  /* �_�l�ƪѲ��̤j������ */
  if ( MAX_STOCK_COST <= 0 )
    mudlog( LOG_ERR, "boot_db: �Ѳ��̤j���� %d ���X�z.", MAX_STOCK_COST );

  if ( ( max_stock_buy = ( MAX_ASSET - 1 ) / MAX_STOCK_COST ) <= 0 )
    mudlog( LOG_ERR, "boot_db: �Ѳ��̤j������ %d ���X�z.", max_stock_buy );

  /* Ū�J symbol.def */
  load_symbol( symbol_file );

  /* Ū�J������ */
  load_date( date_file );
  weather_setting();

  /* Ū�J help ����� */
  load_help( help_dir );

  /* Ū�J social ����� */
  load_social( social_dir , social_file );

  /* Ū�J�u�@����� */
  load_angel( angel_dir );

  /* Ū�J�i���e�� */
  load_greeting( greet_dir );
  load_welcome( welcome_file );
  load_welcome_immortal( welcome_imm );

  /* Ū�J��������ɮ� */
  load_motd( motd_file );

  /* Ū�J site ����� */
  load_site( site_file );

  /* Ū�J�u�@������ɮ� */
  load_server( station_file );

  /* Ū�JID�O�d�r */
  load_xnames( xname_file );

  /* ���J�ɯŸ�� */
  load_promotion( promotion_file );

  /* Ū�J�Ѳ�����ɮ� */
  load_stock( stock_file );

  /* Ū�J�Ҧ����a��Ʈw */
  load_database( database_file );

  /* Ū�J�^����� */
  load_hero( hero_file );

  /* Ū��¾�~����ɮ� */
  load_class( class_dir );

  /* Ū���ޯ� */
  load_skill( skill_dir , skill_file );

  /* Ū���G�� */
  load_liq( liq_dir );

  /* �ˬd�ޯ�¾�~�O�_���T */
  check_skill();

  /* �ˬd¾�~�O�_���T */
  check_class();

  /* �����n�����ޯ� */
  check_ironclad();

  /* �W���a�θ���ɮ� */
  load_sector( sector_dir );

  /* Ū�����O�ɮ� */
  load_instrument( ins_dir , ins_file );

  /* Ū��Ĳ�o�ƥ��ɮ� */
  load_event( event_file );

  /* Ū�������� */
  load_situs( situs_file );

  /* �ҩl�ƾB�n */
  init_mask();

  /* Ū�J�ϰ��� */
  open_area_directory();

  /* �ˬd���n���ж� */
  check_room();

  /* �ˬd���n�Ǫ� */
  check_mobile();

  /* �ˬd�w�]���~ */
  check_object();

  /* Ū�J���� */
  open_board_directory();

  /* Ū���a�K��� */
  load_bus( bus_file );

  /* Ū������� */
  load_ship( ship_file );

  /* Ū���Ѱg��� */
  load_quest( quest_file );

  /* Ū�����D����� */
  load_question( question_file );

  /* Ū�����ڸ�� */
  load_immlist( immlist_file );

  /* Ū�����~��� */
  load_sale( sale_file );

  /* Ū�����ظ���ɮ� */
  load_donate( donate_file );

  /* Ū��������� */
  load_club( club_file );

  /* Ū�J�벼�ɮ� */
  load_vote( vote_dir );

  /* Ū�J�����ɮ� */
  load_joke( joke_dir );

  /* Ū�J§���ɮ� */
  load_gift( gift_file );
  check_gift();

  /* Ū�J�a���ɮ׸�� */
  load_bounty( bounty_file );

  /* Ū�����گ��W���ɮ׸�� */
  load_internal( internal_file );

  /* �ץ��ϰ��ɥX�f�O�_���T */
  fix_exits();

  /* �_�ϤƪZ���j�|��� */
  init_fight( fight_info );
  fight_info->time = 0;
  fight_info->open = 0;

  /* �ˬd�O�_�ܤ֤@�ӥX�ͦa */
  check_area_capital();

  /* �ŧi db booting ���� */
  fBootDb = FALSE;

  /* �ˬd�O�_�����~�ƭȿ��~ */
  fix_object_value();

  /* �ץ��O�_�ǰe�I�����D */
  fix_recall();

  /* �ץ��O�_���e�ޯ঳���D */
  load_innate();

  /* ��ݭ��ͪ����X�O�_���T */
  fix_reborn_number();

  /* �ץ��ޯ઺�S���~ */
  check_skill_innate();

  /* �Ұʰϰ쭫�m�ʧ@ */
  area_update();

  /* Ū�J�H���ɮ� */
  load_mail( note_dir );

  /* �]�w MobProgram */
  MOBtrigger = TRUE;

  RETURN_NULL();
}

void fix_exits( void )
{
  extern const sh_int   rev_dir [];
  ROOM_INDEX_DATA     * pRoomIndex;
  ROOM_INDEX_DATA     * to_room;
  EXIT_DATA           * pexit;
  EXIT_DATA           * pexit_rev;
  bool                  fexit;
  int                   iHash;
  int                   door;

  PUSH_FUNCTION( "fix_exits" );

  for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
  {
    for ( pRoomIndex = room_index_hash[iHash]; pRoomIndex;
          pRoomIndex = pRoomIndex->next )
    {
      for ( fexit = FALSE, door = 0; door < DIR_MAX; door++ )
      {
        if ( ( pexit = pRoomIndex->exit[door] ) )
        {
          fexit = TRUE;
          if ( pexit->vnum <= 0 )
          {
            pexit->to_room = NULL;
          }
          else
          {
            if ( !( pexit->to_room = get_room_index( pexit->vnum ) ) )
            {
              mudlog( LOG_DEBUG, "fix_exits: �ж� %d �X�f%s(%d) ���s�b."
                , pRoomIndex->vnum
                , direction_name( door )
                , pexit->vnum );
            }
          }
        }
      }

      if ( !fexit ) pRoomIndex->NoMob = TRUE;
    }
  }

  for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
  {
    for ( pRoomIndex  = room_index_hash[iHash]; pRoomIndex;
          pRoomIndex  = pRoomIndex->next )
    {
      for ( door = 0; door < DIR_MAX; door++ )
      {
        if ( ( pexit     = pRoomIndex->exit[door]       )
          && ( to_room   = pexit->to_room               )
          && ( pexit_rev = to_room->exit[rev_dir[door]] )
          && pexit_rev->to_room != pRoomIndex )
        {
          mudlog( LOG_FAILEXIT, "Fix_exits: %d:%d �ץ��� %d:%d -> %d.",
            pRoomIndex->vnum, door, to_room->vnum, rev_dir[door],
            ( pexit_rev->to_room == NULL ) ? 0 : pexit_rev->to_room->vnum );
        }
      }
    }
  }

  RETURN_NULL();
}

void load_innate( void )
{
  SKILL_DATA * pSkill;
  int          loop;

  PUSH_FUNCTION( "load_innate" );

  for ( loop = 0; loop < MAX_INNATE; loop++ ) iInnate[loop] = -1;

  /* �w�] */
  for ( loop = 0; loop < MAX_DEFAULT_CLASS; loop++ ) iClass[loop] = NULL;

  for ( nInnate = 0, pSkill = skill_list; pSkill; pSkill = pSkill->next )
  {
    if ( pSkill->innate )
    {
      if ( nInnate >= MAX_INNATE )
        mudlog( LOG_ERR , "Load_innate: �W�X�d�� %d.", MAX_INNATE );

      iInnate[nInnate++] = pSkill->slot;
    }
  }

  if ( nInnate < aInnate )
    mudlog( LOG_ERR , "Load_innate: �w�]�ޯ� %d �p��]�w�� %d."
      , nInnate, aInnate );

  mudlog( LOG_INFO, "�t�θ��J %d �ӹw�]�ޯ�.", nInnate );
  RETURN_NULL();
}

void fix_recall( void )
{
  int               loop;
  ROOM_INDEX_DATA * pRoom;

  PUSH_FUNCTION( "fix_recall" );

  for ( loop = 0; loop < MAX_RECALL; loop++ )
  {
    if ( RecallPlace[loop] == 0 ) continue;

     if ( !( pRoom = get_room_index( RecallPlace[loop] ) )
      || pRoom->Memorize == FALSE )
    {
      mudlog( LOG_DEBUG, "fix_recall: �� %d �ǰe�I %d ���~."
        , loop, RecallPlace[loop] );

      RecallPlace[loop] = 0;
    }
  }

  RETURN_NULL();
}

void fix_reborn_number( void )
{
  int              vnum;
  MOB_INDEX_DATA * pMobIndex;

  PUSH_FUNCTION( "fix_reborn_number" );

  for ( vnum = 0; vnum < MAX_KEY_HASH; vnum++ )
  {
    for ( pMobIndex = mob_index_hash[vnum % MAX_KEY_HASH]; pMobIndex;
          pMobIndex = pMobIndex->next )
    {
      if ( pMobIndex->reborn_vnum > 0
        && !get_mob_index( pMobIndex->reborn_vnum ) )
      {
        mudlog( LOG_DEBUG , "fix_reborn_number: �Ǫ����X %d �����͸��X %d "
          "�Ǫ����s�b.", pMobIndex->vnum, pMobIndex->reborn_vnum );

        pMobIndex->reborn_vnum = 0;
      }
    }
  }

  RETURN_NULL();
}

void mine_reset_area( AREA_DATA * pArea )
{
  MINERAL_DATA    * pMineral;
  MINE_DATA       * pMine;
  MINE_DATA       * zMine;
  ROOM_INDEX_DATA * pRoom;
  ROOM_INDEX_DATA * zRoom;
  int               loop;
  int               count;

  PUSH_FUNCTION( "mine_reset_area" );

  for ( pRoom = pArea->list; pRoom; pRoom = pRoom->next_in_area )
  {
    for ( pMine = pRoom->mine; pMine; pMine = zMine )
    {
      zMine = pMine->next;
      free_struct( pMine, STRUCT_MINE_DATA );
    }

    pRoom->mine = NULL;
  }

  for ( pMineral = pArea->mineral; pMineral; pMineral = pMineral->next )
  {
    for ( loop = 0; loop < pMineral->count; loop++ )
    {
      zRoom = NULL;
      count = 0;
      for ( pRoom = pArea->list; pRoom; pRoom = pRoom->next_in_area )
        if ( !pRoom->people && number_range( 0, count++ ) == 0 )
          zRoom = pRoom;

      if ( zRoom )
      {
        pMine          = alloc_struct( STRUCT_MINE_DATA );
        pMine->mineral = pMineral;
        pMine->next    = zRoom->mine;
        zRoom->mine    = pMine;
      }
    }
  }

  RETURN_NULL();
}

void reset_area( AREA_DATA * pArea )
{
  ROOM_INDEX_DATA * pRoomIndex;
  MOB_INDEX_DATA  * pMobIndex;
  MOB_INDEX_DATA  * pMount;
  OBJ_INDEX_DATA  * pObjIndex;
  OBJ_INDEX_DATA  * pObjToIndex;
  OBJ_DATA        * obj;
  OBJ_DATA        * obj_to;
  RESET_DATA      * pReset;
  AFFECT_DATA       pAffect;
  CHAR_DATA       * mob   = NULL;
  CHAR_DATA       * horse;
  EXIT_DATA       * pexit;
  bool              last  = TRUE;
  int               level = 0;
  int               d0;
  int               d1;

  PUSH_FUNCTION( "reset_area" );

  for ( pReset = pArea->reset_first; pReset; pReset = pReset->next )
  {
    switch ( pReset->command )
    {
    default:

      mudlog( LOG_DEBUG , "Reset_area: ���~���R�O %c.", pReset->command );
      break;

    case 'M':

      if ( ( pMobIndex = get_mob_index( pReset->arg1 ) ) == NULL )
      {
        mudlog( LOG_DEBUG , "Reset_area: 'M': ���~���X %d.", pReset->arg1 );
        continue;
      }

      if ( ( pRoomIndex = get_room_index( pReset->arg3 ) ) == NULL )
      {
        mudlog( LOG_DEBUG , "Reset_area: 'R': ���~���X %d.", pReset->arg3 );
        continue;
      }

      level = URANGE( 0, pMobIndex->level - 2, LEVEL_HERO );

      if ( pMobIndex->count >= pReset->arg2 )
      {
        last = FALSE;
        break;
      }

      /* ���|���� */
      if ( merc_exec && IS_SET( pMobIndex->act, ACT_NOREBORN ) )
      {
        last = FALSE;
        break;
      }

      mob = create_mobile( pMobIndex, -1 );

      if ( room_is_dark( pRoomIndex ) && !is_affected( mob, SLOT_INFRAVISION ) )
      {
        pAffect.type      = SLOT_INFRAVISION;
        pAffect.duration  = -1;
        pAffect.location  = APPLY_NONE;
        pAffect.modifier  = 0;
        pAffect.level     = pMobIndex->level;
        affect_to_char( mob, &pAffect );
      }

      char_to_room( mob, pRoomIndex );
      level = URANGE( 0, mob->level - 2, LEVEL_HERO );
      last  = TRUE;
      break;

    case 'O':

      if ( ( pObjIndex = get_obj_index( pReset->arg1 ) ) == NULL )
      {
        mudlog( LOG_DEBUG , "Reset_area: 'O': ���~���X %d.", pReset->arg1 );
        continue;
      }

      if ( ( pRoomIndex = get_room_index( pReset->arg3 ) ) == NULL )
      {
        mudlog( LOG_DEBUG , "Reset_area: 'R': ���~���X %d.", pReset->arg3 );
        continue;
      }

      if ( pArea->nplayer > 0
        || count_obj_list( pObjIndex, pRoomIndex->contents ) > 0 )
      {
        last = FALSE;
        break;
      }

      obj       = create_object( pObjIndex, -1 );
      obj_to_room( obj, pRoomIndex );
      last = TRUE;
      break;

    case 'P':
      if ( ( pObjIndex = get_obj_index( pReset->arg1 ) ) == NULL )
      {
        mudlog( LOG_DEBUG , "Reset_area: 'P': ���~���X %d.", pReset->arg1 );
        continue;
      }

      if ( ( pObjToIndex = get_obj_index( pReset->arg3 ) ) == NULL )
      {
        mudlog( LOG_DEBUG , "Reset_area: 'P': ���~���X %d.", pReset->arg3 );
        continue;
      }

      if ( pArea->nplayer > 0
        || ( obj_to = get_obj_type( pObjToIndex ) ) == NULL
        || count_obj_list( pObjIndex, obj_to->contains ) > 0 )
      {
        last = FALSE;
        break;
      }

      obj = create_object( pObjIndex, -1 );
      obj_to_obj( obj, obj_to );
      last = TRUE;
      break;

    case 'G':
    case 'E':

      if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
      {
        mudlog( LOG_DEBUG , "Reset_area: 'E' �� 'G': ���~���X %d."
          , pReset->arg1 );
        continue;
      }

      if ( !last ) break;

      if ( !mob )
      {
        mudlog( LOG_DEBUG , "Reset_area: 'E' or 'G': �S�����X %d ���Ǫ�."
          , pReset->arg1 );

        last = FALSE;
        break;
      }

      if ( mob->pIndexData->pShop )
      {
        obj = create_object( pObjIndex, -1 );
        obj->Inventory = TRUE;
      }

      else
      {
        obj = create_object( pObjIndex, -1 );
      }

      obj_to_char( obj, mob );
      if ( pReset->command == 'E' ) equip_char( mob, obj, pReset->arg3 );
      last = TRUE;
      break;

    case 'A':

      if ( !( pMount = get_mob_index( pReset->arg1 ) ) )
      {
        mudlog( LOG_DEBUG, "reset_area: 'A' ���M %d ���~", pReset->arg1 );
        continue;
      }

      if ( !last ) break;

      if ( pMount->tractable <= 0 )
      {
        mudlog( LOG_DEBUG, "reset_area: 'A' %d ���O���M", pReset->arg1 );
        last = FALSE;
        continue;
      }

      if ( !mob || !mob->in_room )
      {
        mudlog( LOG_DEBUG, "reset_area: 'A' ���X %d ���e���~", pReset->arg1 );
        last = FALSE;
        break;
      }

      if ( mob->mount )
      {
        mudlog( LOG_DEBUG, "reset_area: 'A' ���X %d ���M����", pReset->arg1 );
        last = FALSE;
        break;
      }

      horse = create_mobile( pMount, -1 );

      if ( room_is_dark( mob->in_room )
         && !is_affected( horse, SLOT_INFRAVISION ) )
      {
        pAffect.type      = SLOT_INFRAVISION;
        pAffect.duration  = -1;
        pAffect.location  = APPLY_NONE;
        pAffect.modifier  = 0;
        pAffect.level     = pMount->level;
        affect_to_char( horse, &pAffect );
      }

      char_to_room( horse, mob->in_room );

      mob->mount      = horse;
      horse->mount_by = mob;

      last = TRUE;
      break;

    case 'D':
      if ( ( pRoomIndex = get_room_index( pReset->arg1 ) ) == NULL )
      {
        mudlog( LOG_DEBUG , "Reset_area: 'D': ���~���X %d.", pReset->arg1 );
        continue;
      }

      if ( ( pexit = pRoomIndex->exit[pReset->arg2] ) == NULL ) break;

      switch ( pReset->arg3 )
      {
      case DOOR_OPEN:
        pexit->closed = FALSE;
        pexit->locked = FALSE;
        break;

      case DOOR_CLOSED:
        pexit->closed = TRUE;
        pexit->locked = FALSE;
        break;

      case DOOR_LOCKED:
        pexit->closed = TRUE;
        pexit->locked = TRUE;
        break;
      }

      last = TRUE;
      break;

    case 'R':
      if ( ( pRoomIndex = get_room_index( pReset->arg1 ) ) == NULL )
      {
        mudlog( LOG_DEBUG , "Reset_area: 'R': ���~���X %d.", pReset->arg1 );
        continue;
      }

      for ( d0 = 0; d0 < pReset->arg2 - 1; d0++ )
      {
        d1                   = number_range( d0, pReset->arg2-1 );
        pexit                = pRoomIndex->exit[d0];
        pRoomIndex->exit[d0] = pRoomIndex->exit[d1];
        pRoomIndex->exit[d1] = pexit;
      }
      break;
    }
  }

  RETURN_NULL();
}

CHAR_DATA * create_mobile( MOB_INDEX_DATA * pMobIndex, int level )
{
  CHAR_DATA * mob;
  int         loop;

  PUSH_FUNCTION( "create_mobile" );

  if ( pMobIndex == NULL )
    mudlog( LOG_ERR , "Create_mobile: �쫬���X���Ū�." );

  /* �t�m�O���鵹�L */
  clear_char( mob = alloc_struct( STRUCT_CHAR_DATA ) );

  mob->pIndexData  = pMobIndex;

  mob->name        = str_dup( pMobIndex->name        );
  mob->byname      = str_dup( pMobIndex->short_descr );
  mob->long_descr  = str_dup( pMobIndex->long_descr  );
  mob->description = str_dup( pMobIndex->description );
  mob->prompt      = str_dup( "<^hp1 ^mp1 ^mv1>" );
  mob->email       = str_dup( "<none>" );
  mob->spec_fun    = pMobIndex->spec_fun;

  mob->act           = pMobIndex->act;

  mob->alignment     = pMobIndex->alignment;
  mob->sex           = pMobIndex->sex;
  mob->class         = pMobIndex->class;
  mob->speak         = pMobIndex->speak;
  mob->disable_prog  = FALSE;

  if ( level < 0 ) mob->level = pMobIndex->level;
  else             mob->level = level;

  if ( IS_SET( mob->act, AUTOSET_VALUE ) )
  {
    mob->now_str = mob->level * LOAD_ATTRIB_PLUS;
    mob->now_int = mob->level * LOAD_ATTRIB_PLUS;
    mob->now_wis = mob->level * LOAD_ATTRIB_PLUS;
    mob->now_dex = mob->level * LOAD_ATTRIB_PLUS;
    mob->now_con = mob->level * LOAD_ATTRIB_PLUS;

    mob->armor       = - mob->level * LOAD_ARMOR_PLUS;

    gold_to_char( mob, mob->level * LOAD_GOLD_PLUS );
    mob->hitroll     = 0;
    mob->damroll     = mob->level * LOAD_DAMROLL_PLUS;

    mob->hit  = mob->max_hit  = mob->level * load_hit_plus;
    mob->mana = mob->max_mana = mob->level * load_mana_plus;
    mob->move = mob->max_move = mob->level * load_move_plus;

    mob->mod_hit     = 0;
    mob->mod_move    = 0;
    mob->mod_mana    = 0;
  }

  else
  {
    mob->now_str = pMobIndex->now_str == ERRORCODE
                 ? mob->level * LOAD_ATTRIB_PLUS : pMobIndex->now_str;

    mob->now_int = pMobIndex->now_int == ERRORCODE
                 ? mob->level * LOAD_ATTRIB_PLUS : pMobIndex->now_int;

    mob->now_wis = pMobIndex->now_wis == ERRORCODE
                 ? mob->level * LOAD_ATTRIB_PLUS : pMobIndex->now_wis;

    mob->now_dex = pMobIndex->now_dex == ERRORCODE
                 ? mob->level * LOAD_ATTRIB_PLUS : pMobIndex->now_dex;

    mob->now_con = pMobIndex->now_con == ERRORCODE
                 ? mob->level * LOAD_ATTRIB_PLUS : pMobIndex->now_con;

    mob->armor   = pMobIndex->ac == ERRORCODE
                 ? mob->level * LOAD_ARMOR_PLUS : pMobIndex->ac;

    gold_to_char( mob, pMobIndex->gold == ERRORCODE
      ? mob->level * LOAD_GOLD_PLUS : pMobIndex->gold );

    mob->hitroll = pMobIndex->hitroll;

    mob->damroll = pMobIndex->damplus == ERRORCODE
                 ? mob->level * LOAD_DAMROLL_PLUS : pMobIndex->damplus;

    mob->max_hit = pMobIndex->hit == ERRORCODE
                 ? mob->level * load_hit_plus : pMobIndex->hit;

    mob->max_mana = pMobIndex->mana == ERRORCODE
                  ? mob->level * load_mana_plus : pMobIndex->mana;

    mob->max_move = pMobIndex->move == ERRORCODE
                  ? mob->level * load_move_plus : pMobIndex->move;

    mob->hit      = mob->max_hit;
    mob->mana     = mob->max_mana;
    mob->move     = mob->max_move;
    mob->mod_mana = 0;
    mob->mod_hit  = 0;
    mob->mod_move = 0;
  }

  mob->enable    = pMobIndex->enable;
  mob->tractable = pMobIndex->tractable;
  mob->migrate   = pMobIndex->migrate;

  /* �]�w���m�� */
  for ( loop = 0; loop < MAX_SKILL; loop++ )
    mob->skill[loop] = pMobIndex->skill[loop];

  pMobIndex->count++;
  RETURN( mob );
}

OBJ_DATA * create_object( OBJ_INDEX_DATA * pObjIndex, int level )
{
  OBJ_DATA * obj;
  int        loop;

  PUSH_FUNCTION( "create_object" );

  if ( !pObjIndex ) mudlog( LOG_ERR, "Create_object: �Ū����~�쫬." );

  /* �t�m�O���� */
  obj = alloc_struct( STRUCT_OBJ_DATA );

  obj->pIndexData = pObjIndex;
  obj->wear_loc   = -1;
  obj->venom      = -1;

  obj->name        = str_dup( pObjIndex->name        );
  obj->cname       = str_dup( pObjIndex->short_descr );
  obj->description = str_dup( pObjIndex->description );

  obj->item_type   = pObjIndex->item_type;
  obj->wear_flags  = pObjIndex->wear_flags;
  obj->weight      = pObjIndex->weight;
  obj->disappear   = pObjIndex->disappear;
  obj->address     = NULL;
  obj->club        = NULL;

  obj->Glow        = pObjIndex->Glow;
  obj->Hum         = pObjIndex->Hum;
  obj->Dark        = pObjIndex->Dark;
  obj->Lock        = pObjIndex->Lock;
  obj->Evil        = pObjIndex->Evil;
  obj->Invis       = pObjIndex->Invis;
  obj->Magic       = pObjIndex->Magic;
  obj->NoDrop      = pObjIndex->NoDrop;
  obj->Bless       = pObjIndex->Bless;
  obj->AntiGood    = pObjIndex->AntiGood;
  obj->AntiEvil    = pObjIndex->AntiEvil;
  obj->AntiNeutral = pObjIndex->AntiNeutral;
  obj->NoRemove    = pObjIndex->NoRemove;
  obj->Inventory   = pObjIndex->Inventory;
  obj->Cancachet   = pObjIndex->Cancachet;
  obj->UserSet     = pObjIndex->UserSet;
  obj->NoSave      = pObjIndex->NoSave;
  obj->Contraband  = pObjIndex->Contraband;
  obj->Loadable    = pObjIndex->Loadable;
  obj->Takeable    = pObjIndex->Takeable;

  obj->serial.high = 0;
  obj->serial.low  = 0;
  obj->owner.high  = 0;
  obj->owner.low   = 0;

  if ( level < 0 )
    obj->level = UMAX( 1, UMIN( MAX_LEVEL, pObjIndex->level ) );
  else
    obj->level = URANGE( 1, level, MAX_LEVEL );

  obj->cost = ( pObjIndex->cost < 0 )
    ? number_fuzzy( 10 ) * number_fuzzy( UMIN( 1, level ) )
    : pObjIndex->cost;

  for ( loop = 0; loop < MAX_OBJECT_VALUE; loop++ )
    obj->value[loop]= pObjIndex->value[loop];

  /* ���~��� */
  obj->unit = pObjIndex->unit ? pObjIndex->unit : DefaultUnit;

  switch ( obj->item_type )
  {
  default:
    mudlog( LOG_DEBUG , "create_object: ���~���X %d ���A %d ���~."
      , pObjIndex->vnum, obj->item_type );
    break;

  case ITEM_LIGHT:
  case ITEM_TREASURE:
  case ITEM_FURNITURE:
  case ITEM_TRASH:
  case ITEM_CONTAINER:
  case ITEM_DRINK_CON:
  case ITEM_KEY:
  case ITEM_BOAT:
  case ITEM_CORPSE_NPC:
  case ITEM_CORPSE_PC:
  case ITEM_FOUNTAIN:
  case ITEM_MAGICSTONE:
  case ITEM_POTION:
  case ITEM_PILL:
  case ITEM_SPIRITJADE:
  case ITEM_MYSTERY:
  case ITEM_LETTER:
  case ITEM_ORE:
  case ITEM_WEAPON:
  case ITEM_ARMOR:
  case ITEM_SIGIL:
  case ITEM_VELLUM:
  case ITEM_URN:
  case ITEM_ARROW:
    break;

  case ITEM_BACKBAG:
    obj->value[1] = 0;
    obj->value[2] = 0;
    break;

  case ITEM_FIREWORK:
    obj->value[0] = UMIN( MAX_SCAN_DEPTH -1, UMAX( 1, obj->value[0] ) );
    obj->value[1] = UMIN( 10000, UMAX( 0, obj->value[0] ) );
    break;

  case ITEM_GOLDMINE:
    obj->value[1] = 0;
    obj->value[2] = UMIN( MAX_LEVEL, UMIN( 1, obj->value[2] ) );
    break;

  case ITEM_FOOD:
    if ( obj->value[0] > 0 ) obj->timer = obj->value[0];
    else                     obj->timer = FoodDegrade;

  case ITEM_SCROLL:
    obj->value[0] = number_fuzzy( UMAX( 1, obj->value[0] ) );
    break;

  case ITEM_WAND:
  case ITEM_STAFF:
    obj->value[0] = number_fuzzy( UMAX( 1, obj->value[0] ) );
    obj->value[1] = number_fuzzy( UMAX( 1, obj->value[1] ) );
    obj->value[2] = UMAX( 1, obj->value[1] );
    break;

  case ITEM_MONEY:
    obj->value[0] = UMAX( 0, obj->cost );
    break;
  }

  /* �p�G�O���������˳� */
  if ( is_armor( obj ) )
  {
    obj->max_armor   = pObjIndex->armor;
    obj->armor       = pObjIndex->armor;
  }

  pObjIndex->count++;
  RETURN( obj );
}

/* �M���s���H�� */
void clear_char( CHAR_DATA * ch )
{
  PUSH_FUNCTION( "clear_char" );

  ch->name          = str_dup( "" );
  ch->cname         = str_dup( "" );
  ch->byname        = str_dup( "" );
  ch->long_descr    = str_dup( "" );
  ch->description   = str_dup( "" );
  ch->prompt        = str_dup( "" );
  ch->email         = str_dup( "" );
  ch->editing       = str_dup( "" );
  ch->stack         = str_dup( "" );
  ch->last_note     = 0;
  ch->last_joke     = 0;
  ch->last_clubnote = 0;
  ch->logon         = current_time;
  ch->armor         = 0;
  ch->position      = POS_STANDING;
  ch->practice      = DefaultPractice;
  ch->hit           = DefaultHit;
  ch->max_hit       = DefaultHit;
  ch->mod_hit       = 0;
  ch->mana          = DefaultMana;
  ch->max_mana      = DefaultMana;
  ch->mod_mana      = 0;
  ch->move          = DefaultMove;
  ch->max_move      = DefaultMove;
  ch->mod_move      = 0;
  ch->alignment     = DefaultAlign;
  ch->gold          = 0;
  ch->bank          = 0;
  ch->turn          = 0;
  ch->notify        = 0;
  ch->mpactnum      = 0;
  ch->spiritjade    = 0;
  ch->migrate       = 0;
  ch->nskill        = 0;
  ch->adviser       = 0;
  ch->eq_capcity    = 50;
  ch->donate        = 0;
  ch->mask          = ERRORCODE;
  ch->talk_mode     = MODE_ABSENT;
  ch->bounty        = NULL;
  ch->alias         = NULL;
  ch->connect       = NULL;
  ch->failcode      = NULL;
  ch->enable        = NULL;
  ch->deposit       = NULL;
  ch->mpact         = NULL;
  ch->social        = NULL;
  ch->mount         = NULL;
  ch->mount_by      = NULL;
  ch->club          = NULL;
  ch->class         = NULL;
  ch->affected      = NULL;
  ch->quest         = NULL;
  ch->enemy         = NULL;
  ch->friend        = NULL;
  ch->badfriend     = NULL;
  ch->talk_buf      = NULL;
  ch->hirer         = NULL;
  ch->mercenary     = NULL;
  ch->spirit        = NULL;
  ch->boss          = NULL;
  ch->vote          = NULL;
  ch->stamp         = NULL;
  ch->basic         = NULL;
  ch->answer        = NULL;
  ch->clublog       = FALSE;
  ch->delete        = FALSE;  /* �]�w�|���M�� */
  ch->speak         = TRUE;
  ch->cease         = FALSE;
  ch->trade         = TRUE;
  ch->disable_prog  = TRUE;
  ch->release       = FALSE;
  ch->inprocess     = FALSE;
  ch->fight_tick    = 0;
  ch->ante[PK_FIRST]= 0;
  ch->ante[PK_SECOND]= 0;

  ch->serial.high   = 0;
  ch->serial.low    = 0;

  ch->jade.high     = 0;
  ch->jade.low      = 0;

  ch->home          = -1;
  ch->hitroll       = 0;
  ch->damroll       = 0;

  RETURN_NULL();
}

void free_char( CHAR_DATA * ch )
{
  int           loop;
  OBJ_DATA    * obj;
  OBJ_DATA    * obj_next;
  AFFECT_DATA * paf;
  AFFECT_DATA * paf_next;
  ENROLL_DATA * pEnroll;
  ENROLL_DATA * zEnroll;
  QUEST_DATA  * pQuest;
  QUEST_DATA  * zQuest;
  NOTE_DATA   * pNote;
  VOTE_DATA   * pVote;
  IP_DATA     * pIp;
  IP_DATA     * zIp;

  PUSH_FUNCTION( "free_char" );

  if ( !ch || ch->delete )
  {
    mudlog( LOG_DEBUG, "free_char: �ӷ������T�Τw�g�Q����F." );
    RETURN_NULL();
  }

  /* ��H�񪫵����� */
  for ( obj = ch->deposit; obj; obj = obj_next )
  {
    obj_next = obj->next_content;
    obj_from_char_deposit( obj );
    obj_to_char( obj, ch );
  }

  /* ����a���~������ */
  for ( obj = ch->carrying; obj; obj = obj_next )
  {
    obj_next = obj->next_content;
    extract_obj( obj );
  }

  /* �⨭�W���v�T������ */
  for ( paf = ch->affected; paf; paf = paf_next )
  {
    paf_next = paf->next;
    affect_remove( ch, paf );
  }

  ch->affected = NULL;

  if ( ch->pcdata )
  {
    free_string( ch->pcdata->pwd      );
    free_string( ch->pcdata->bamfin   );
    free_string( ch->pcdata->bamfout  );
    free_string( ch->pcdata->title    );
    free_string( ch->pcdata->lasthost );
    free_string( ch->pcdata->mater    );

    ch->pcdata->pwd      = str_dup( "" );
    ch->pcdata->bamfout  = str_dup( "" );
    ch->pcdata->bamfin   = str_dup( "" );
    ch->pcdata->title    = str_dup( "" );
    ch->pcdata->lasthost = str_dup( "" );
    ch->pcdata->mater    = NULL;

    /* ����O�ƥ� */
    for ( loop = 0; loop < MAX_NOTEPAD; loop++ )
    {
      free_string( ch->pcdata->notepad[loop] );
      ch->pcdata->notepad[loop] = str_dup( "" );
    }

    if ( ch->pcdata->trace )
    {
      free_struct( ch->pcdata->trace, STRUCT_TRACE_DATA );
      ch->pcdata->trace = NULL;
    }

    for ( loop = 0; loop < MAX_TRACE; loop++ )
    {
      if ( ch->pcdata->map[loop] )
      {
        free_struct( ch->pcdata->map[loop], STRUCT_TRACE_DATA );
        ch->pcdata->map[loop] = NULL;
      }
    }

    /* ���񵲺c���O���� */
    free_struct( ch->pcdata , STRUCT_PC_DATA );
  }

  /* ���񥨶����O */
  extract_alias( ch );

  /* ������~�K�X���c */
  if ( ch->failcode && !IS_NPC( ch ) ) extract_failcode( ch );

  /* ����n�ͦW�� */
  extract_friend( ch );
  extract_badfriend( ch );

  /* ���񭭨��} */
  for ( pIp = ch->connect; pIp; pIp = zIp )
  {
    zIp = pIp->next;
    free_string( pIp->address );
    free_struct( pIp, STRUCT_IP_DATA );
  }

  ch->connect = NULL;

  if ( ch->answer )
  {
    free_struct( ch->answer, STRUCT_ANSWER_DATA );
    ch->answer = NULL;
  }

  /* ����w�]�ޯ� */
  if ( !IS_NPC( ch ) ) extract_enable( ch );

  /* �����յ��c��� */
  extract_gamble( ch );

  /* �����H�󳡥� */
  if ( ( pNote = ch->pnote ) )
  {
    free_string( pNote->sender   );
    free_string( pNote->date     );
    free_string( pNote->to_list  );
    free_string( pNote->subject  );
    free_string( pNote->text     );
    free_string( pNote->filename );

    free_struct( pNote, STRUCT_NOTE_DATA );
    ch->pnote = NULL;
  }

  /* �����벼���� */
  if ( ( pVote = ch->vote ) )
  {
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
    ch->vote = NULL;
  }

  /* �����O������ */
  for ( pEnroll = ch->enroll; pEnroll; pEnroll = zEnroll )
  {
    zEnroll = pEnroll->next;
    free_struct( pEnroll, STRUCT_ENROLL_DATA );
  }

  ch->enroll = NULL;

  /* �]�w���骺�֦��� */
  for ( obj = object_list; obj; obj = obj->next )
  {
    if ( verify_obj( obj ) && obj->corpse_owner == ch )
      obj->corpse_owner = NULL;
  }

  /* ������� */
  for ( pQuest = ch->quest; pQuest; pQuest = zQuest )
  {
    zQuest = pQuest->next;
    free_struct( pQuest, STRUCT_QUEST_DATA );
  }

  ch->quest = NULL;

  /* ���񵲺c���O���� */
  free_struct( ch , STRUCT_CHAR_DATA );

  RETURN_NULL();
}

char * get_extra_descr( const char * name, EXTRA_DESCR_DATA * ed )
{
  PUSH_FUNCTION( "get_extra_descr" );

  for ( ; ed; ed = ed->next )
    if ( is_name( name, ed->keyword ) ) RETURN( ed->description );

  RETURN( NULL );
}

SECTOR_DATA * get_sector_index( int vnum )
{
  SECTOR_DATA * pSector;

  PUSH_FUNCTION( "get_sector_index" );

  for ( pSector = sector_list; pSector; pSector = pSector->next )
    if ( pSector->vnum == vnum ) RETURN( pSector );

  RETURN( NULL );
}

MOB_INDEX_DATA * get_mob_index( int vnum )
{
  MOB_INDEX_DATA * pMobIndex;

  PUSH_FUNCTION( "get_mob_index" );

  for ( pMobIndex = mob_index_hash[vnum % MAX_KEY_HASH]; pMobIndex;
        pMobIndex = pMobIndex->next )
  {
    if ( pMobIndex->vnum == vnum ) RETURN( pMobIndex );
  }

  RETURN( NULL );
}

/* ���o�Y�s���Ǫ����ƶq */
int get_mob_count( int vnum )
{
  int         count;
  CHAR_DATA * man;

  PUSH_FUNCTION( "get_mob_count" );

  /* ���o���ĤH�� */
  for ( count = 0, man = char_list; man; man = man->next )
  {
    if ( !verify_char( man ) ) continue;

    if ( IS_NPC( man ) && man->pIndexData && man->pIndexData->vnum == vnum )
      count++;
  }
  RETURN( count );
}

OBJ_INDEX_DATA * get_obj_index( int vnum )
{
  OBJ_INDEX_DATA * pObjIndex;

  PUSH_FUNCTION( "get_obj_index" );

  for ( pObjIndex = obj_index_hash[vnum % MAX_KEY_HASH]; pObjIndex;
        pObjIndex = pObjIndex->next )
  {
    if ( pObjIndex->vnum == vnum ) RETURN( pObjIndex );
  }

  RETURN( NULL );
}

ROOM_INDEX_DATA * get_room_index( int vnum )
{
  ROOM_INDEX_DATA * pRoomIndex;

  PUSH_FUNCTION( "get_room_index" );

  for ( pRoomIndex  = room_index_hash[vnum % MAX_KEY_HASH]; pRoomIndex;
        pRoomIndex  = pRoomIndex->next )
  {
    if ( pRoomIndex->vnum == vnum ) RETURN( pRoomIndex );
  }

  RETURN( NULL );
}

AREA_DATA * get_room_area( int vnum )
{
  AREA_DATA       * pArea;
  ROOM_INDEX_DATA * pRoom;

  PUSH_FUNCTION( "get_room_area" );

  if ( vnum <= 0 ) RETURN( NULL );

  for ( pArea = area_first; pArea; pArea = pArea->next )
  {
    for ( pRoom = pArea->list; pRoom; pRoom = pRoom->next_in_area )
      if ( pRoom->vnum == vnum ) RETURN( pArea );
  }

  RETURN( NULL );
}

ROOM_INDEX_DATA * random_room( AREA_DATA * pArea )
{
  ROOM_INDEX_DATA * pRoom;
  ROOM_INDEX_DATA * zRoom;
  AREA_DATA       * zArea;
  int               count;

  PUSH_FUNCTION( "random_room" );

  zRoom = NULL;
  count = 0;

  for ( zArea = area_first; zArea; zArea = zArea->next )
  {
    if ( pArea && pArea != zArea ) continue;

    for ( pRoom = zArea->list; pRoom; pRoom = pRoom->next_in_area )
    {
      if ( ( number_range( 1, ++count ) ) == 1 ) zRoom = pRoom;
    }
  }

  RETURN( zRoom );
}

FUNCTION( do_areas )
{
  AREA_DATA       * pArea;
  ROOM_INDEX_DATA * room;
  int               count;
  int               sn;
  char              arg[MAX_INPUT_LENGTH];

  PUSH_FUNCTION( "do_areas" );

  clear_buffer();
  argument = one_argument( argument, arg );

  if ( arg[0] == '\x0' )
  {
    send_to_buffer(
      "\e[1;33;44m    �s���        �ϰ�W��     �ж� �Ǫ� ���~ ��"
      "�� ���m �j�� ���a ��  ��\e[0m\n\r"
      VERTICAL_LINE );

    for ( count = 0, pArea = area_first; pArea; pArea = pArea->next )
    {
      if ( pArea->attribute || IS_IMMORTAL( ch ) )
      {
        send_to_buffer( "%2d. %-12s %s%-12s %4d %4d %4d "
                        "%4d %4d %4d %4d %2d-%-2d\n\r"
          , ++count
          , pArea->editor
          , ( pArea->attribute ) ? " " : "\e[1;32m*\e[0m"
          , pArea->name
          , pArea->room
          , pArea->mob
          , pArea->obj
          , pArea->shop
          , pArea->reset
          , pArea->foggy
          , IS_IMMORTAL( ch ) ? pArea->nplayer : -1
          , pArea->lower
          , pArea->upper );

        if ( buffer_full() ) break;
      }
    }

    send_to_buffer( "%s", VERTICAL_LINE );
    print_buffer( ch );
  }

  else if ( is_number( arg ) && IS_IMMORTAL( ch ) )
  {
    if ( ( sn = atoi( arg ) ) < 0 )
    {
      send_to_char( "�A�����Ǥ��X�z�M�Ьd�� areas ���ϥΤ�k�C\n\r", ch );
      RETURN_NULL();
    }

    for ( count = 1, pArea = area_first; pArea; pArea = pArea->next, count++ )
    {
      if ( count == sn )
      {
        send_to_buffer(
          "�ϰ�\e[1;36m%s\e[0m  �Ǹ��R%d  �����R%d\n\r�Ҧ��ж����X�R"
          , pArea->name, pArea->serial, pArea->capital_no );

        for ( count = 0, room = pArea->list; room; room = room->next_in_area )
        {
          if ( ( count++ % 6 ) == 0 ) send_to_buffer( "\n\r" );
          send_to_buffer( "%3d. [\e[1;32m%5d\e[0m] ", count, room->vnum );
          if ( buffer_full() ) break;
        }

        if ( count == 0 ) send_to_buffer( "�o�Ӱϰ�S�����󪺩ж��C" );
        send_to_buffer( "\n\r" );

        print_buffer( ch );
        RETURN_NULL();
      }
    }

    send_to_char( "�S���o�Ӱϰ�C\n\r", ch );
  }

  else if ( !str_cmp( arg, "here" ) && IS_IMMORTAL( ch ) )
  {
    if ( !ch->in_room || !( pArea = ch->in_room->area ) )
    {
      send_to_char( "�t�εL�k�P�w�A�ثe����m�T\n\r", ch );
      RETURN_NULL();
    }

    send_to_buffer(
      "�ϰ�\e[1;36m%s\e[0m  �Ǹ��R%d  �����R%d\n\r�Ҧ��ж����X�R"
      , pArea->name, pArea->serial, pArea->capital_no );

    for ( count = 0, room = pArea->list; room; room = room->next_in_area )
    {
      if ( ( count++ % 6 ) == 0 ) send_to_buffer( "\n\r" );
      send_to_buffer( "%3d. [\e[1;32m%5d\e[0m] ", count, room->vnum );
      if ( buffer_full() ) break;
    }

    if ( count == 0 ) send_to_buffer( "�o�Ӱϰ�S�����󪺩ж��C" );
    send_to_buffer( "\n\r" );

    print_buffer( ch );
  }

  else if ( !str_prefix( arg, "help" ) )
  {
    one_argument( argument, arg );
    if ( arg[0] == '\x0' || !is_number( arg ) )
    {
      send_to_char( "�A�n�d�߭��@�Ӱϰ쪺�������S\n\r", ch );
      RETURN_NULL();
    }

    if ( ( sn = atoi( arg ) ) < 0 )
    {
      send_to_char( "�A���ϰ쪺���Ǥ��X�k�M�Ьd�� areas ���ϥΤ�k�C\n\r", ch );
      RETURN_NULL();
    }

    for ( count = 0, pArea = area_first; pArea; pArea = pArea->next )
    {
      if ( ( pArea->attribute || IS_IMMORTAL( ch ) ) && ++count == sn )
      {
        if ( !pArea->description || !*pArea->description )
        {
          act( "�藍�_�M$t�S�������������C", ch, pArea->name, NULL, TO_CHAR );
          RETURN_NULL();
        }

        act( "$t���������p�U�R$A$T"
          , ch, pArea->name, pArea->description, TO_CHAR );

        RETURN_NULL();
      }
    }

    send_to_char( "�藍�_�M�S���o�Ӱϰ�C\n\r", ch );
  }

  else
  {
    send_to_char( "�y�k���~�M�Ьd�� areas ���ϥΤ�k�C\n\r", ch );
  }

  RETURN_NULL();
}

/* ���� % �o�ӲŸ� */
void smash_point( char * str )
{
  PUSH_FUNCTION( "smash_point" );

  for ( ; *str; str++ )
  {
    switch( *str )
    {
    default:
      break;

    case '%':
    case '$':
      *str = '^';
      break;
    }
  }

  RETURN_NULL();
}

/* ���� ~ �o�ӲŸ� */
void smash_tilde( char * str )
{
  bool fChinese = FALSE;
  bool fLast;

  PUSH_FUNCTION( "smash_tilde" );

  for ( ; *str != '\0'; str++ )
  {
    fLast = fChinese;
    if (fChinese) fChinese = FALSE;
    else if ( ( *str >= '\x81') && ( *str <= '\xFF' ) )
         fChinese = TRUE;
    else fChinese = FALSE;

    if ( *str == '~' && !fLast ) *str = '-';
    if ( *str == '\xff' )        *str = '-';
  }

  if ( fChinese ) *--str = '\x0';
  RETURN_NULL();
}

void tail_chain( void )
{
  return;
}

int mprog_name_to_type( char * name )
{
  PUSH_FUNCTION( "mprog_name_to_type" );

  if ( !str_cmp( name , "in_file_prog"   )) RETURN( IN_FILE_PROG   );
  if ( !str_cmp( name , "act_prog"       )) RETURN( ACT_PROG       );
  if ( !str_cmp( name , "speech_prog"    )) RETURN( SPEECH_PROG    );
  if ( !str_cmp( name , "rand_prog"      )) RETURN( RAND_PROG      );
  if ( !str_cmp( name , "fight_prog"     )) RETURN( FIGHT_PROG     );
  if ( !str_cmp( name , "hitprcnt_prog"  )) RETURN( HITPRCNT_PROG  );
  if ( !str_cmp( name , "death_prog"     )) RETURN( DEATH_PROG     );
  if ( !str_cmp( name , "entry_prog"     )) RETURN( ENTRY_PROG     );
  if ( !str_cmp( name , "greet_prog"     )) RETURN( GREET_PROG     );
  if ( !str_cmp( name , "all_greet_prog" )) RETURN( ALL_GREET_PROG );
  if ( !str_cmp( name , "give_prog"      )) RETURN( GIVE_PROG      );
  if ( !str_cmp( name , "bribe_prog"     )) RETURN( BRIBE_PROG     );

  RETURN( ERROR_PROG );
}

char * symbol_string( int num, char * field )
{
  static char   buf[MAX_STRING_LENGTH];
  SYMBOL_DATA * pSymbol;

  PUSH_FUNCTION( "symbol_string" );

  sprintf( buf, "%d", num );

  if ( field && *field )
  {
    for ( pSymbol = symbol_list; pSymbol; pSymbol = pSymbol->next_content )
    {
      if ( pSymbol->num == num && !strcmp( pSymbol->field, field ) )
      {
        str_cpy( buf, pSymbol->str );
        RETURN( buf );
      }
    }
  }

  RETURN( buf );
}

bool symbol_lookup( char * symbol, int * num )
{
  SYMBOL_DATA * pSymbol;
  int           letter;
  int           len;

  PUSH_FUNCTION( "symbol_lookup" );

  if ( !symbol || !*symbol || !isalpha( letter = symbol[0] ) ) RETURN( FALSE );

  letter = LOWER( letter ) - 'a';
  len    = UMIN( 19, str_len( symbol ) - 1 );

  for ( pSymbol = symbol_hash[letter][len]; pSymbol; pSymbol = pSymbol->next )
  {
    if ( !str_cmp( pSymbol->str, symbol ) )
    {
      *num = pSymbol->num;
      RETURN( TRUE );
    }
  }

  RETURN( FALSE );
}

void check_ironclad( void )
{
  PUSH_FUNCTION( "check_ironclad" );

  if ( !get_skill( SLOT_BLINDNESS ) )
    mudlog( LOG_DEBUG, "check_ironclad: �S�������ޯ�." );

  if ( !get_skill( SLOT_CHARM_PERSON ) )
    mudlog( LOG_DEBUG, "check_ironclad: �S���g�b�ޯ�." );

  if ( !get_skill( SLOT_CURSE ) )
    mudlog( LOG_DEBUG, "check_ironclad: �S���A�G�ޯ�." );

  if ( !get_skill( SLOT_INVIS ) )
    mudlog( LOG_DEBUG, "check_ironclad: �S�����Χޯ�." );

  if ( !get_skill( SLOT_POISON ) )
    mudlog( LOG_DEBUG, "check_ironclad: �S���U�r�ޯ�." );

  if ( !get_skill( SLOT_SLEEP ) )
    mudlog( LOG_DEBUG, "check_ironclad: �S�����Χޯ�." );

  if ( !get_skill( SLOT_SNEAK ) )
    mudlog( LOG_DEBUG, "check_ironclad: �S�����ޯ�." );

  if ( !get_skill( SLOT_MASS_INVIS ) )
    mudlog( LOG_DEBUG, "check_ironclad: �S�������ޯ�." );

  RETURN_NULL();
}

/* �ˬd¾�~�O�_�����~ */
void check_class( void )
{
  CLASS_DATA * pClass;

  PUSH_FUNCTION( "check_class" );

  for ( pClass = class_list; pClass; pClass = pClass->next )
  {
    if ( !( class_lookup( pClass->associate ) ) )
      pClass->associate = ERRORCODE;
  }

  RETURN_NULL();
}

/* �ˬd�O�_�ܤ֭n���@�ӥX�ͦa */
void check_area_capital( void )
{
  AREA_DATA * pArea;

  PUSH_FUNCTION( "check_area_capital" );

  for ( pArea = area_first; pArea; pArea = pArea->next )
    if ( pArea->capital ) RETURN_NULL();

  mudlog( LOG_ERR , "check_area_capital: �ܤ֭n���@�ӥX�ͦa." );
  RETURN_NULL();
}

/* �ˬd�ޯ�O�_�����s���X�z */
void check_skill( void )
{
  SKILL_DATA    * pSkill;
  RESTRICT_DATA * pRest;
  int             associate;

  PUSH_FUNCTION( "check_skill" );

  for ( pSkill = skill_list; pSkill; pSkill = pSkill->next )
  {
    /* �s���쥼�w�q���k�N���X�W */
    if ( ( associate = pSkill->associate ) > 0 )
    {
      if ( associate >= MAX_SKILL || ( get_skill( associate ) == NULL ) )
        mudlog( LOG_ERR , "�k�N���X %d ���~�s��(%d)."
          , pSkill->slot, associate );

      /* �s���쥻���k�N�W */
      if ( associate == pSkill->slot )
        mudlog( LOG_ERR, "�k�N���X %d ���~�s���쥻��.", pSkill->slot );

      /* �קK���k�N���X�o�S�������� */
      if ( associate > 0 && pSkill->degree <= 0 )
        mudlog( LOG_ERR, "check_skill: �k�N���X %d �S���ޯ������."
          , pSkill->slot );
    }
    else
    {
      pSkill->associate = ERRORCODE;
      pSkill->degree    = ERRORCODE;
    }

    for ( pRest = pSkill->restrict; pRest; pRest = pRest->next )
    {
      switch( pRest->type )
      {
      default:
        break;

      case RES_SKILL:
        if ( !get_skill( pRest->value ) )
          mudlog( LOG_ERR, "check_skill: �k�N���X %d ������ޯ� %d ���s�b."
            , pSkill->slot, pRest->value );

        if ( pRest->vicevalue < 0 || pRest->vicevalue > 100 )
          mudlog( LOG_ERR, "check_skill: �k�N���X %d ����ޯ�ƭ� %d ���X�z."
            , pSkill->slot, pRest->vicevalue );
        break;

      case RES_NOSKILL:

        if ( !get_skill( pRest->value ) )
          mudlog( LOG_ERR, "check_skill: �k�N���X %d ������ޯ� %d ���s�b."
            , pSkill->slot, pRest->value );

        if ( pRest->vicevalue < 0 || pRest->vicevalue > 100 )
          mudlog( LOG_ERR, "check_skill: �k�N���X %d ����ޯ�ƭ� %d ���X�z."
            , pSkill->slot, pRest->vicevalue );
        break;
      }
    }
  }

  RETURN_NULL();
}

/* �ץ��ޯ઺�S���~ */
void check_skill_innate( void )
{
  SKILL_DATA  * pSkill;
  DAMAGE_DATA * pDam;

  PUSH_FUNCTION( "check_skill_innate" );

  for ( pSkill = skill_list; pSkill; pSkill = pSkill->next )
  {
    for ( pDam = pSkill->damage; pDam; pDam = pDam->next )
    {
      pDam->innate = NULL;

      if ( pDam->obj_vnum > 0 )
      {
        if ( !( pDam->innate = get_obj_index( pDam->obj_vnum ) ) )
        {
          pDam->obj_vnum = ERRORCODE;
          mudlog( LOG_DEBUG, "check_skill_innate: �ޯ�%s���S���~���s�b."
            , pSkill->name );
        }
      }
    }
  }

  RETURN_NULL();
}

void check_room( void )
{
  PUSH_FUNCTION( "check_room" );

  if ( !( RoomJail = get_room_index( RoomJailVnum ) ) )
    mudlog( LOG_ERR, "check_room: �S���ʺ��ж����X %d.", RoomJailVnum );

  if ( !( RoomLimbo = get_room_index( RoomLimboVnum ) ) )
    mudlog( LOG_ERR, "check_room: �S�� Limbo �ж����X %d.", RoomLimboVnum );

  if ( !( RoomChat = get_room_index( RoomChatVnum ) ) )
    mudlog( LOG_ERR, "check_room: �S����ѫǸ��X %d.", RoomChatVnum );

  if ( !( RoomDead = get_room_index( RoomDeadVnum ) ) )
    mudlog( LOG_ERR, "check_room: �S�����`�ж����X %d.", RoomDeadVnum );

  if ( !( RoomCorpse = get_room_index( RoomCorpseVnum ) ) )
    mudlog( LOG_ERR, "check_room: �S�����Ͷ����X %d.", RoomCorpseVnum );

  if ( !( RoomRecall = get_room_index( RoomRecallVnum ) ) )
    mudlog( LOG_ERR, "check_room: �S�� Recall �ж����X %d.", RoomRecallVnum );

  if ( !( RoomSchool = get_room_index( RoomSchoolVnum ) ) )
    mudlog( LOG_ERR, "check_room: �S���Ǯթж����X %d.", RoomSchoolVnum );

  if ( !( RoomFail = get_room_index( RoomFailVnum ) ) )
    mudlog( LOG_ERR, "check_room: �S���r�C�x���X %d.", RoomFailVnum );

  RETURN_NULL();
}

void check_object( void )
{
  int              loop;
  OBJ_INDEX_DATA * pIndex;

  PUSH_FUNCTION( "check_object" );

  for ( loop = 0; loop < MAX_DEFAULT_OBJECT; loop++ )
  {
    if ( DefaultObject[loop] <= 0 ) break;

    if ( !( pIndex = get_obj_index( DefaultObject[loop] ) ) )
    {
      mudlog( LOG_ERR, "check_object: �S���w�]���~���X %d."
        , DefaultObject[loop] );
    }

    if ( pIndex->Takeable == FALSE )
      mudlog( LOG_ERR, "check_object: �w�]���~���X %d �O���i���� %d."
        , DefaultObject[loop], pIndex->wear_flags );
  }

  if ( !( ObjProtype    = get_obj_index( ObjProtypeVnum    ) ) )
    mudlog( LOG_ERR, "check_object: �S�����~�쫬 :%d.", ObjProtypeVnum );

  if ( !( ObjMoney      = get_obj_index( ObjMoneyVnum      ) ) )
    mudlog( LOG_ERR, "check_object: �S�����~��: %d.", ObjMoneyVnum );

  if ( !( ObjMoneySome  = get_obj_index( ObjMoneySomeVnum  ) ) )
    mudlog( LOG_ERR, "check_object: �S�����~�� : %d.", ObjMoneySomeVnum );

  if ( !( ObjCorpseNPC  = get_obj_index( ObjCorpseNPCVnum  ) ) )
    mudlog( LOG_ERR, "check_object: �S���Ǫ�����: %d.", ObjCorpseNPCVnum );

  if ( !( ObjCorpsePC   = get_obj_index( ObjCorpsePCVnum   ) ) )
    mudlog( LOG_ERR, "check_object: �S�����a����: %d.", ObjCorpsePCVnum );

  if ( !( ObjHead       = get_obj_index( ObjHeadVnum       ) ) )
    mudlog( LOG_ERR, "check_object: �S�����~(�Y): %d.", ObjHeadVnum );

  if ( !( ObjHeart      = get_obj_index( ObjHeartVnum      ) ) )
    mudlog( LOG_ERR, "check_object: �S�����~(��Ŧ): %d.", ObjHeartVnum );

  if ( !( ObjArm        = get_obj_index( ObjArmVnum        ) ) )
    mudlog( LOG_ERR, "check_object: �S�����~(���u): %d.", ObjArmVnum );

  if ( !( ObjLeg        = get_obj_index( ObjLegVnum        ) ) )
    mudlog( LOG_ERR, "check_object: �S�����~(�L) : %d.", ObjLegVnum );

  if ( !( ObjTurd       = get_obj_index( ObjTurdVnum       ) ) )
    mudlog( LOG_ERR, "check_object: �S�����~(��): %d.", ObjTurdVnum );

  if ( !( ObjMushroom   = get_obj_index( ObjMushroomVnum   ) ) )
    mudlog( LOG_ERR, "check_object: �S�����~(Ĩۣ) : %d.", ObjMushroomVnum );

  if ( !( ObjLight      = get_obj_index( ObjLightVnum      ) ) )
    mudlog( LOG_ERR, "check_object: �S�����~(�OŢ) : %d.", ObjLightVnum );

  if ( !( ObjSpring     = get_obj_index( ObjSpringVnum     ) ) )
    mudlog( LOG_ERR, "check_object: �S�����~(�u��) : %d.", ObjSpringVnum );

  if ( !( ObjDumpling   = get_obj_index( ObjDumplingVnum   ) ) )
    mudlog( LOG_ERR, "check_object: �S�����~(�]�l) : %d.", ObjDumplingVnum );

  if ( !( ObjBougi      = get_obj_index( ObjBougiVnum      ) ) )
    mudlog( LOG_ERR, "check_object: �S�����~(�]�l) : %d.", ObjBougiVnum );

  if ( !( ObjPon        = get_obj_index( ObjPonVnum        ) ) )
    mudlog( LOG_ERR, "check_object: �S�����~(�ѥ]) : %d.", ObjPonVnum );

  if ( !( ObjChicken    = get_obj_index( ObjChickenVnum    ) ) )
    mudlog( LOG_ERR, "check_object: �S�����~(����) : %d.", ObjChickenVnum );

  if ( !( ObjMagicStone = get_obj_index( ObjMagicStoneVnum ) ) )
    mudlog( LOG_ERR, "check_object: �S�����~(�]��) : %d.", ObjMagicStoneVnum );

  if ( !( ObjMeat       = get_obj_index( ObjMeatVnum       ) ) )
    mudlog( LOG_ERR, "check_object: �S�����~(�פ�) : %d.", ObjMeatVnum );

  if ( !( ObjLetter     = get_obj_index( ObjLetterVnum     ) ) )
    mudlog( LOG_ERR, "check_object: �S�����~(�H��): %d.", ObjLetterVnum );

  if ( !( ObjUrn        = get_obj_index( ObjUrnVnum        ) ) )
    mudlog( LOG_ERR, "check_object: �S�����~(����): %d.", ObjUrnVnum );

  RETURN_NULL();
}

void check_mobile( void )
{
  PUSH_FUNCTION( "check_mobile" );

  if ( !( MobVampire = get_mob_index( MobVampireVnum ) ) )
    mudlog( LOG_ERR, "check_mobile: �S���p�����X %d.", MobVampireVnum );

  if ( !( MobPractice = get_mob_index( MobPracticeVnum ) ) )
    mudlog( LOG_ERR, "check_mobile: �S���m�\\�Ǫ����X %d.", MobPracticeVnum );

  RETURN_NULL();
}

void weather_setting( void )
{
  long lhour;
  long lday;
  long lmonth;

  PUSH_FUNCTION( "weather_setting" );

  if ( DaysPerMonth <= 0 || MonthsPerYear <= 0 )
    mudlog( LOG_ERR, "weather_setting: ����]�w���~�M������X�k�C" );

  /* �]�w�ɶ��H�ΤѮ� */
  lhour           = ( current_time - 650336715 )
                  / ( PULSE_TICK / PULSE_PER_SECOND );

  time_info.hour  = lhour  % 24;
  lday            = lhour  / 24;
  time_info.day   = lday   % DaysPerMonth;
  lmonth          = lday   / DaysPerMonth;
  time_info.month = lmonth % MonthsPerYear;
  time_info.year  = lmonth / MonthsPerYear;
  time_info.week  = 0;

       if ( time_info.hour <  5 ) weather_info.sunlight = SUN_DARK;
  else if ( time_info.hour <  6 ) weather_info.sunlight = SUN_RISE;
  else if ( time_info.hour < 19 ) weather_info.sunlight = SUN_LIGHT;
  else if ( time_info.hour < 20 ) weather_info.sunlight = SUN_SET;
  else                            weather_info.sunlight = SUN_DARK;

  /* �վ��� */
  weather_info.change = 0;
  weather_info.mmhg   = 960;

  /*
  if ( time_info.month >= 7 && time_info.month <= 12 )
    weather_info.mmhg += number_range( 1, 50 );
  else
    weather_info.mmhg += number_range( 1, 80 );
  */

  weather_info.mmhg += number_range( 1, 70 );

       if ( weather_info.mmhg <=  980 ) weather_info.sky = SKY_LIGHTNING;
  else if ( weather_info.mmhg <= 1000 ) weather_info.sky = SKY_RAINING;
  else if ( weather_info.mmhg <= 1020 ) weather_info.sky = SKY_CLOUDY;
  else                                  weather_info.sky = SKY_CLOUDLESS;

  RETURN_NULL();
}

void fix_object_value( void )
{
  OBJ_INDEX_DATA * pObjIndex;
  FILE           * pFile;
  extern int       top_obj_index;
  char             buf[MAX_STRING_LENGTH];
  int              loop;
  int              vnum;
  int              nMatch;
  int              ret;

  PUSH_FUNCTION( "fix_object_value" );

  /* �����ɮ׵����� */
  if ( ( pFile = FOPEN( badobject_file, "w" ) ) ) FCLOSE( pFile );

  for ( nMatch = vnum = 0; nMatch < top_obj_index; vnum++ )
  {
    if ( ( pObjIndex = get_obj_index( vnum ) ) )
    {
      nMatch++;

      for ( loop = 0; loop < MAX_OBJECT_VALUE; loop++ )
      {
        str_cpy( buf, check_obj_value( pObjIndex->item_type, loop
          , pObjIndex->value[loop], &ret, TRUE ) );

        if ( buf[0] != '\x0' )
        {
          mudlog( LOG_BADOBJECT, "���~���X�R%-5d ���~�W�١R%s(%s) ���~���A�R%s"
            , pObjIndex->vnum, pObjIndex->short_descr
            , pObjIndex->name
            , item_kind_name( pObjIndex->item_type ) );

          mudlog( LOG_BADOBJECT, "���~���|�R%s", pObjIndex->filename );

          mudlog( LOG_BADOBJECT, "�ƭ� %2d�R %-8d ��ĳ�ȡR%-8d"
            , loop, pObjIndex->value[loop], ret );

          mudlog( LOG_BADOBJECT, "��]�R%s", buf );
          mudlog( LOG_BADOBJECT, "" );
        }
      }
    }
  }

  RETURN_NULL();
}

void init_mask( void )
{
  SITUS_DATA * pSitus;

  PUSH_FUNCTION( "init_mask" );

  for ( ObjWearMask = 0, pSitus = situs_list; pSitus; pSitus = pSitus->next )
    ObjWearMask |= pSitus->wear;

  for ( AttackMask = 0, pSitus = situs_list; pSitus; pSitus = pSitus->next )
    AttackMask |= pSitus->attack;

  RETURN_NULL();
}

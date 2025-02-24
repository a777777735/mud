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
#include <string.h>
#include "merc.h"

#define OID                     OBJ_INDEX_DATA
#define MID                     MOB_INDEX_DATA

bool    replace_obj_index       args( ( OID *, OID * ) );
bool    replace_mob_index       args( ( MID *, MID * ) );
void    reset_object_value      args( ( OBJ_DATA *, OID *, bool ) );

bool            fReload;
extern char     FileOop[];

void            init_reload_status      args( ( void ) );

void clear_reload_status( void )
{
  PUSH_FUNCTION( "clear_reload_status" );

  fReload    = FALSE;
  FileError  = 0;
  FileOop[0] = '\x0';

  RETURN_NULL();
}

void init_reload_status( void )
{
  PUSH_FUNCTION( "init_reload_status" );

  fReload    = TRUE;
  FileError  = 0;
  FileOop[0] = '\x0';

  RETURN_NULL();
}

FUNCTION( do_reload )
{
  OBJ_INDEX_DATA * pObjIndex;
  OBJ_INDEX_DATA * nObjIndex;
  MOB_INDEX_DATA * pMobIndex;
  MOB_INDEX_DATA * nMobIndex;
  char             arg[MAX_INPUT_LENGTH];
  char             buf[MAX_STRING_LENGTH];
  int              vnum;
  int              loop;
  int              ret;

  PUSH_FUNCTION( "do_reload" );

  argument = one_argument( argument ,arg );

  if ( arg[0] == '\x0' )
  {
    send_to_char( "你要重新載入哪種格式的檔案呢﹖\n\r", ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg, "object" ) )
  {
    if ( argument[0] == '\x0' || !is_number( argument ) )
    {
      send_to_char( "你必須註明載入物品的號碼﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !( pObjIndex = get_obj_index( vnum = atoi( argument ) ) ) )
    {
      act( "原本就沒有號碼 $x 的物品﹗", ch, &vnum, NULL, TO_CHAR );
      RETURN_NULL();
    }

    init_reload_status();

    if ( !( nObjIndex = load_object( pObjIndex->filename ) ) )
    {
      print_to_char( ch,
        "\e[1;33m載入失敗﹐詳細資料如下﹕\n\r\e[0m"
        "名稱﹕%s\e[0m(%s)\n\r"
        "號碼﹕%d\n\r"
        "行數﹕%d\n\r"
        "字數﹕%d\n\r"
        "原因﹕\n\r%s%s%s"
        , pObjIndex->short_descr
        , pObjIndex->name
        , vnum
        , FileLine
        , FileWord
        , VERTICAL_LINE
        , FileOop[0] ? FileOop : "原因不明"
        , VERTICAL_LINE );

      clear_reload_status();
      RETURN_NULL();
    }

    for ( loop = 0; loop < MAX_OBJECT_VALUE; loop++ )
    {
      str_cpy( buf, check_obj_value( nObjIndex->item_type, loop
        , nObjIndex->value[loop], &ret, TRUE ) );

      if ( buf[0] != '\x0' )
      {
        clear_reload_status();
        clear_buffer();

        send_to_buffer(
          "物品數值不合法﹐資料如下﹕\n\r"
          "物品號碼﹕%-5d\n\r"
          "物品名稱﹕%s(%s)\n\r"
          "物品型態﹕%s\n\r"
          "物品路徑﹕%s\n\r"
          "數值 %3d﹕ %-8d\n\r"
          "建 議 值﹕%-8d\n\r"
          "原    因﹕%s\n\r"
          , nObjIndex->vnum, nObjIndex->short_descr
          , nObjIndex->name
          , item_kind_name( nObjIndex->item_type )
          , nObjIndex->filename
          , loop, nObjIndex->value[loop], ret
          , buf );

        print_buffer( ch );
        free_obj_index( nObjIndex );
        RETURN_NULL();
      }
    }

    /* 有些數值必須保留 */
    nObjIndex->vnum          = pObjIndex->vnum;
    nObjIndex->count         = pObjIndex->count;
    nObjIndex->auction_times = pObjIndex->auction_times;
    nObjIndex->auction_gold  = pObjIndex->auction_gold;

    if ( replace_obj_index( pObjIndex, nObjIndex ) )
    {
      clear_reload_status();
      free_obj_index( pObjIndex );

      act( "你已經重新載入號碼 $x 的物品﹗資料如下﹕", ch, &vnum, NULL, TO_CHAR );
      sprintf( buf, "%d", vnum );
      do_olist( ch, buf );
    }
    else
    {
      clear_reload_status();
      free_obj_index( nObjIndex );
      act( "無法置換物品號碼 $x﹗", ch, &vnum, NULL, TO_CHAR );
    }
  }

  else if ( !str_prefix( arg, "mobile" ) )
  {
    if ( argument[0] == '\x0' || !is_number( argument ) )
    {
      send_to_char( "你必須註明載入怪物的號碼﹗\n\r", ch );
      RETURN_NULL();
    }

    if ( !( pMobIndex = get_mob_index( vnum = atoi( argument ) ) ) )
    {
      act( "原本就沒有號碼 $x 的怪物﹗", ch, &vnum, NULL, TO_CHAR );
      RETURN_NULL();
    }

    init_reload_status();

    if ( !( nMobIndex = load_mobiles( pMobIndex->filename ) ) )
    {
      clear_reload_status();
      print_to_char( ch,
        "\e[1;33m載入失敗﹐詳細資料如下﹕\n\r\e[0m"
        "名稱﹕%s\e[0m(%s)\n\r"
        "號碼﹕%d\n\r"
        "行數﹕%d\n\r"
        "字數﹕%d\n\r"
        "原因﹕\n\r%s%s%s"
        , pMobIndex->short_descr
        , pMobIndex->name
        , vnum
        , FileLine
        , FileWord
        , VERTICAL_LINE
        , FileOop[0] ? FileOop : "原因不明"
        , VERTICAL_LINE );

      RETURN_NULL();
    }

    /* 有些數值必須保留 */
    nMobIndex->vnum          = pMobIndex->vnum;
    nMobIndex->count         = pMobIndex->count;
    nMobIndex->pShop         = pMobIndex->pShop;
    nMobIndex->area          = pMobIndex->area;
    nMobIndex->killed        = pMobIndex->killed;

    kill_table[URANGE(0, pMobIndex->level, MAX_LEVEL-1)].number--;
    kill_table[URANGE(0, nMobIndex->level, MAX_LEVEL-1)].number++;

    if ( nMobIndex->reborn_vnum > 0
      && !get_mob_index( nMobIndex->reborn_vnum ) )
      nMobIndex->reborn_vnum = 0;

    if ( replace_mob_index( pMobIndex, nMobIndex ) )
    {
      clear_reload_status();
      free_mob_index( pMobIndex );

      act( "你已經重新載入怪物 $x 的物品﹗資料如下﹕", ch, &vnum, NULL, TO_CHAR );
      sprintf( buf, "%d", vnum );
      do_mlist( ch, buf );
    }
    else
    {
      clear_reload_status();
      act( "無法置換怪物號碼 $x﹗", ch, &vnum, NULL, TO_CHAR );
      free_mob_index( pMobIndex );
    }
  }

  else
  {
    send_to_char( "載入格式錯誤﹐請查詢 reload 的正確用法﹗\n\r", ch );
  }
  RETURN_NULL();
}

bool replace_obj_index( OID * pOld, OID * pNew )
{
  ROOM_INDEX_DATA * pRoom;
  OBJ_INDEX_DATA  * pIndex;
  OBJ_DATA        * pObj;
  SALE_DATA       * pSale;
  GIFT_DATA       * pGift;
  SKILL_DATA      * pSkill;
  DAMAGE_DATA     * pDamage;
  AREA_DATA       * pArea;
  MINERAL_DATA    * pMineral;
  MINE_DATA       * pMine;
  CHAR_DATA       * victim;
  AFFECT_DATA     * pAffect;
  int               iHash;
  int               loop;

  PUSH_FUNCTION( "replace_obj_index" );

  /* 如果號碼不一樣 */
  if ( pOld->vnum != pNew->vnum ) RETURN( FALSE );

  /* 置換拍賣 */
  for ( pSale = sale_list; pSale; pSale = pSale->next )
    if ( pSale->obj == pOld ) pSale->obj = pNew;

  /* 置換禮物 */
  for ( pGift = gift_list; pGift; pGift = pGift->next )
    if ( pGift->gift == pOld ) pGift->gift = pNew;

  /* 置換礦物 */
  for ( pArea = area_first; pArea; pArea = pArea->next )
  {
    for ( pMineral = pArea->mineral; pMineral; pMineral = pMineral->next )
      if ( pMineral->mineral == pOld ) pMineral->mineral = pNew;

    for ( pRoom = pArea->list; pRoom; pRoom = pRoom->next_in_area )
    {
      for ( pMine = pRoom->mine; pMine; pMine = pMine->next )
        for ( pMineral = pMine->mineral; pMineral; pMineral = pMineral->next )
          if ( pMineral->mineral == pOld ) pMineral->mineral = pNew;
    }
  }

  /* 置換傷害 */
  for ( pSkill = skill_list; pSkill; pSkill = pSkill->next )
  {
    for ( pDamage = pSkill->damage; pDamage; pDamage = pDamage->next )
      if ( pDamage->innate == pOld ) pDamage->innate = pNew;
  }

  /* 置換原型 */
  iHash = pOld->vnum % MAX_KEY_HASH;

  /* 先移除 */
  if ( pOld == obj_index_hash[iHash] )
  {
    obj_index_hash[iHash] = pOld->next;
  }
  else
  {
    for ( pIndex = obj_index_hash[iHash]; pIndex; pIndex = pIndex->next )
    {
      if ( pIndex->next == pOld )
      {
        pIndex->next = pOld->next;
        break;
      }
    }
  }

  /* 再插入 */
  pNew->next            = obj_index_hash[iHash];
  obj_index_hash[iHash] = pNew;

  for ( loop = 0; loop < MAX_DEFAULT_OBJECT; loop++ )
  {
    if ( DefaultObject[loop] <= 0 ) break;
    if ( DefaultObject[loop] == pNew->vnum ) pNew->Takeable = TRUE;
  }

  if ( pNew->vnum == ObjProtypeVnum    ) ObjProtype    = pNew;
  if ( pNew->vnum == ObjMoneyVnum      ) ObjMoney      = pNew;
  if ( pNew->vnum == ObjMoneySomeVnum  ) ObjMoneySome  = pNew;
  if ( pNew->vnum == ObjCorpseNPCVnum  ) ObjCorpseNPC  = pNew;
  if ( pNew->vnum == ObjCorpsePCVnum   ) ObjCorpsePC   = pNew;
  if ( pNew->vnum == ObjHeadVnum       ) ObjHead       = pNew;
  if ( pNew->vnum == ObjHeartVnum      ) ObjHeart      = pNew;
  if ( pNew->vnum == ObjArmVnum        ) ObjArm        = pNew;
  if ( pNew->vnum == ObjLegVnum        ) ObjLeg        = pNew;
  if ( pNew->vnum == ObjMushroomVnum   ) ObjMushroom   = pNew;
  if ( pNew->vnum == ObjLightVnum      ) ObjLight      = pNew;
  if ( pNew->vnum == ObjSpringVnum     ) ObjSpring     = pNew;
  if ( pNew->vnum == ObjDumplingVnum   ) ObjDumpling   = pNew;
  if ( pNew->vnum == ObjBougiVnum      ) ObjBougi      = pNew;
  if ( pNew->vnum == ObjPonVnum        ) ObjPon        = pNew;
  if ( pNew->vnum == ObjChickenVnum    ) ObjChicken    = pNew;
  if ( pNew->vnum == ObjMagicStoneVnum ) ObjMagicStone = pNew;
  if ( pNew->vnum == ObjMeatVnum       ) ObjMeat       = pNew;
  if ( pNew->vnum == ObjLetterVnum     ) ObjLetter     = pNew;
  if ( pNew->vnum == ObjUrnVnum        ) ObjUrn        = pNew;

  /* 置換物品 */
  for ( pObj = object_list; pObj; pObj = pObj->next )
  {
    if ( verify_obj( pObj ) && pObj->pIndexData == pOld )
    {
      if ( pObj->unit == pOld->unit )
        pObj->unit = pNew->unit ? pNew->unit : DefaultUnit;

      pObj->pIndexData = pNew;
      pObj->item_type  = pNew->item_type;
      reset_object_value( pObj, pNew, ( pNew == pOld ) );

      pObj->disappear  = pNew->disappear;
      pObj->Glow       = pNew->Glow;
      pObj->Hum        = pNew->Hum;
      pObj->Dark       = pNew->Dark;
      pObj->Lock       = pNew->Lock;
      pObj->Evil       = pNew->Evil;
      pObj->Invis      = pNew->Invis;
      pObj->Magic      = pNew->Magic;
      pObj->NoDrop     = pNew->NoDrop;
      pObj->Bless      = pNew->Bless;
      pObj->AntiGood   = pNew->AntiGood;
      pObj->AntiEvil   = pNew->AntiEvil;
      pObj->AntiNeutral= pNew->AntiNeutral;
      pObj->NoRemove   = pNew->NoRemove;
      pObj->Inventory  = pNew->Inventory;
      pObj->Cancachet  = pNew->Cancachet;
      pObj->UserSet    = pNew->UserSet;
      pObj->NoSave     = pNew->NoSave;
      pObj->Contraband = pNew->Contraband;
      pObj->Loadable   = pNew->Loadable;
      pObj->Takeable   = pNew->Takeable;
      pObj->level      = pNew->level;

      free_string( pObj->name );
      pObj->name = str_dup( pNew->name );

      /* 避免有些有 %s 這些字串 */
      if ( strstr( pNew->description, "%" ) == NULL )
      {
        free_string( pObj->description );
        pObj->description = str_dup( pNew->description );
      }

      if ( ( victim = pObj->carried_by ) && verify_char( victim ) )
      {
        /* 修正重量 */
        victim->carry_weight -= pOld->weight;
        victim->carry_weight += pNew->weight;
        victim->carry_weight  = UMAX( 1, victim->carry_weight );

        /* 修正影響 */
        if ( pObj->wear_loc != WEAR_NONE )
        {
          for ( pAffect = pOld->affected; pAffect; pAffect = pAffect->next )
            affect_modify( victim, pAffect, FALSE );

          for ( pAffect = pOld->impact; pAffect; pAffect = pAffect->next )
            affect_modify( victim, pAffect, FALSE );

          for ( pAffect = pNew->affected; pAffect; pAffect = pAffect->next )
            affect_modify( victim, pAffect, TRUE );

          for ( pAffect = pNew->impact; pAffect; pAffect = pAffect->next )
            affect_modify( victim, pAffect, TRUE );
        }
      }

      /* 修正型態 */
      if ( pNew->wear_flags != pOld->wear_flags )
      {
        /* 正被裝備著 */
        if ( ( victim = pObj->carried_by )
          && verify_char( victim )
          && pObj->wear_loc != WEAR_NONE )
        {
          unequip_char( victim, pObj, FALSE );
        }

        pObj->wear_flags = pNew->wear_flags;
      }
    }
  }

  RETURN( TRUE );
}

bool replace_mob_index( MID * pOld, MID * pNew )
{
  MID         * pIndex;
  BOUNTY_DATA * pBounty;
  CHAR_DATA   * mob;
  int           iHash;
  int           loop;

  PUSH_FUNCTION( "replace_mob_index" );

  /* 如果號碼不一樣 */
  if ( pOld->vnum != pNew->vnum ) RETURN( FALSE );

  for ( pBounty = bounty_list; pBounty; pBounty = pBounty->next )
    if ( pBounty->mob == pOld ) pBounty->mob = pNew;

  if ( pNew->vnum == MobVampireVnum  ) MobVampire  = pNew;
  if ( pNew->vnum == MobPracticeVnum ) MobPractice = pNew;
  /* 置換原型 */
  iHash = pOld->vnum % MAX_KEY_HASH;

  /* 先移除 */
  if ( pOld == mob_index_hash[iHash] )
  {
    mob_index_hash[iHash] = pOld->next;
  }
  else
  {
    for ( pIndex = mob_index_hash[iHash]; pIndex; pIndex = pIndex->next )
    {
      if ( pIndex->next == pOld )
      {
        pIndex->next = pOld->next;
        break;
      }
    }
  }

  /* 再插入 */
  pNew->next            = mob_index_hash[iHash];
  mob_index_hash[iHash] = pNew;

  for ( mob = char_list; mob; mob = mob->next )
  {
    if ( !verify_char( mob ) ) continue;

    if ( mob->pIndexData == pOld && IS_NPC( mob ) )
    {
      mob->pIndexData = pNew;
      mob->spec_fun   = pNew->spec_fun;
      mob->class      = pNew->class;
      mob->enable     = pNew->enable;

      /* 不是小鬼 */
      if ( !mob->boss )
      {
        mob->tractable  = pNew->tractable;
        mob->migrate    = pNew->migrate;
        mob->level      = pNew->level;
        mob->act        = pNew->act;
        mob->alignment  = pNew->alignment;
        mob->sex        = pNew->sex;
        mob->speak      = pNew->speak;

        free_string( mob->name );
        mob->name = str_dup( pNew->name );

        free_string( mob->byname );
        mob->byname = str_dup( pNew->short_descr );

        free_string( mob->long_descr );
        mob->long_descr = str_dup( pNew->long_descr );

        free_string( mob->description );
        mob->description = str_dup( pNew->description );

        for ( loop = 0; loop < MAX_SKILL; loop++ )
          mob->skill[loop] = pNew->skill[loop];
      }
    }
  }

  RETURN( TRUE );
}

void reset_object_value( OBJ_DATA * pObj, OBJ_INDEX_DATA * pIndex, bool bSame )
{
  int loop;

  PUSH_FUNCTION( "reset_object_value" );

  if ( !bSame )
  {
    for ( loop = 0; loop < MAX_OBJECT_VALUE; loop++ )
      pObj->value[loop] = pIndex->value[loop];
  }

  switch( pObj->item_type )
  {
  default:
  case ITEM_LIGHT:
  case ITEM_FURNITURE:
  case ITEM_TRASH:
  case ITEM_KEY:
  case ITEM_BOAT:
  case ITEM_CORPSE_PC:
  case ITEM_CORPSE_NPC:
  case ITEM_FOUNTAIN:
  case ITEM_TREASURE:
  case ITEM_SIGIL:
  case ITEM_VELLUM:
  case ITEM_URN:
  case ITEM_LETTER:
  case ITEM_ARMOR:
  case ITEM_MONEY:
  case ITEM_SPIRITJADE:
    break;

  case ITEM_MYSTERY:
    pObj->value[0] = pIndex->value[0];
    pObj->value[1] = pIndex->value[1];
    break;

  case ITEM_SCROLL:
  case ITEM_POTION:
  case ITEM_PILL:
    pObj->value[0] = pIndex->value[0];
    pObj->value[1] = pIndex->value[1];
    pObj->value[2] = pIndex->value[2];
    pObj->value[3] = pIndex->value[3];
    break;

  /* ???? */
  case ITEM_STAFF:
  case ITEM_WAND:
    pObj->value[4] = pIndex->value[4];
    pObj->value[5] = pIndex->value[5];
    break;

  case ITEM_WEAPON:
    break;

  case ITEM_ARROW:
    pObj->value[0] = pIndex->value[0];
    pObj->value[1] = pIndex->value[1];
    break;

  case ITEM_FIREWORK:
    pObj->value[0] = pIndex->value[0];
    pObj->value[1] = pIndex->value[1];
    break;

  case ITEM_ORE:
    pObj->value[0] = pIndex->value[0];
    pObj->value[1] = pIndex->value[1];
    pObj->value[2] = pIndex->value[2];
    pObj->value[3] = pIndex->value[3];
    break;

  case ITEM_GOLDMINE:
    pObj->value[0] = pIndex->value[0];
    pObj->value[2] = pIndex->value[2];
    break;

  case ITEM_BACKBAG:
    pObj->value[0] = pIndex->value[0];
    break;

  /* ???? */
  case ITEM_CONTAINER:
    pObj->value[0] = pIndex->value[0];
    pObj->value[2] = pIndex->value[2];
    break;

  case ITEM_DRINK_CON:
    pObj->value[0] = pIndex->value[0];
    break;

  case ITEM_FOOD:
    pObj->value[2] = pIndex->value[2];
    pObj->value[3] = pIndex->value[3];
    pObj->value[4] = pIndex->value[4];
    break;

  /* 未調整 ???? */
  case ITEM_MAGICSTONE:
    break;
  }
  RETURN_NULL();
}

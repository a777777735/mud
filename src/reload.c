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
    send_to_char( "�A�n���s���J���خ榡���ɮשO�S\n\r", ch );
    RETURN_NULL();
  }

  if ( !str_prefix( arg, "object" ) )
  {
    if ( argument[0] == '\x0' || !is_number( argument ) )
    {
      send_to_char( "�A�����������J���~�����X�T\n\r", ch );
      RETURN_NULL();
    }

    if ( !( pObjIndex = get_obj_index( vnum = atoi( argument ) ) ) )
    {
      act( "�쥻�N�S�����X $x �����~�T", ch, &vnum, NULL, TO_CHAR );
      RETURN_NULL();
    }

    init_reload_status();

    if ( !( nObjIndex = load_object( pObjIndex->filename ) ) )
    {
      print_to_char( ch,
        "\e[1;33m���J���ѡM�ԲӸ�Ʀp�U�R\n\r\e[0m"
        "�W�١R%s\e[0m(%s)\n\r"
        "���X�R%d\n\r"
        "��ơR%d\n\r"
        "�r�ơR%d\n\r"
        "��]�R\n\r%s%s%s"
        , pObjIndex->short_descr
        , pObjIndex->name
        , vnum
        , FileLine
        , FileWord
        , VERTICAL_LINE
        , FileOop[0] ? FileOop : "��]����"
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
          "���~�ƭȤ��X�k�M��Ʀp�U�R\n\r"
          "���~���X�R%-5d\n\r"
          "���~�W�١R%s(%s)\n\r"
          "���~���A�R%s\n\r"
          "���~���|�R%s\n\r"
          "�ƭ� %3d�R %-8d\n\r"
          "�� ĳ �ȡR%-8d\n\r"
          "��    �]�R%s\n\r"
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

    /* ���Ǽƭȥ����O�d */
    nObjIndex->vnum          = pObjIndex->vnum;
    nObjIndex->count         = pObjIndex->count;
    nObjIndex->auction_times = pObjIndex->auction_times;
    nObjIndex->auction_gold  = pObjIndex->auction_gold;

    if ( replace_obj_index( pObjIndex, nObjIndex ) )
    {
      clear_reload_status();
      free_obj_index( pObjIndex );

      act( "�A�w�g���s���J���X $x �����~�T��Ʀp�U�R", ch, &vnum, NULL, TO_CHAR );
      sprintf( buf, "%d", vnum );
      do_olist( ch, buf );
    }
    else
    {
      clear_reload_status();
      free_obj_index( nObjIndex );
      act( "�L�k�m�����~���X $x�T", ch, &vnum, NULL, TO_CHAR );
    }
  }

  else if ( !str_prefix( arg, "mobile" ) )
  {
    if ( argument[0] == '\x0' || !is_number( argument ) )
    {
      send_to_char( "�A�����������J�Ǫ������X�T\n\r", ch );
      RETURN_NULL();
    }

    if ( !( pMobIndex = get_mob_index( vnum = atoi( argument ) ) ) )
    {
      act( "�쥻�N�S�����X $x ���Ǫ��T", ch, &vnum, NULL, TO_CHAR );
      RETURN_NULL();
    }

    init_reload_status();

    if ( !( nMobIndex = load_mobiles( pMobIndex->filename ) ) )
    {
      clear_reload_status();
      print_to_char( ch,
        "\e[1;33m���J���ѡM�ԲӸ�Ʀp�U�R\n\r\e[0m"
        "�W�١R%s\e[0m(%s)\n\r"
        "���X�R%d\n\r"
        "��ơR%d\n\r"
        "�r�ơR%d\n\r"
        "��]�R\n\r%s%s%s"
        , pMobIndex->short_descr
        , pMobIndex->name
        , vnum
        , FileLine
        , FileWord
        , VERTICAL_LINE
        , FileOop[0] ? FileOop : "��]����"
        , VERTICAL_LINE );

      RETURN_NULL();
    }

    /* ���Ǽƭȥ����O�d */
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

      act( "�A�w�g���s���J�Ǫ� $x �����~�T��Ʀp�U�R", ch, &vnum, NULL, TO_CHAR );
      sprintf( buf, "%d", vnum );
      do_mlist( ch, buf );
    }
    else
    {
      clear_reload_status();
      act( "�L�k�m���Ǫ����X $x�T", ch, &vnum, NULL, TO_CHAR );
      free_mob_index( pMobIndex );
    }
  }

  else
  {
    send_to_char( "���J�榡���~�M�Ьd�� reload �����T�Ϊk�T\n\r", ch );
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

  /* �p�G���X���@�� */
  if ( pOld->vnum != pNew->vnum ) RETURN( FALSE );

  /* �m����� */
  for ( pSale = sale_list; pSale; pSale = pSale->next )
    if ( pSale->obj == pOld ) pSale->obj = pNew;

  /* �m��§�� */
  for ( pGift = gift_list; pGift; pGift = pGift->next )
    if ( pGift->gift == pOld ) pGift->gift = pNew;

  /* �m���q�� */
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

  /* �m���ˮ` */
  for ( pSkill = skill_list; pSkill; pSkill = pSkill->next )
  {
    for ( pDamage = pSkill->damage; pDamage; pDamage = pDamage->next )
      if ( pDamage->innate == pOld ) pDamage->innate = pNew;
  }

  /* �m���쫬 */
  iHash = pOld->vnum % MAX_KEY_HASH;

  /* ������ */
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

  /* �A���J */
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

  /* �m�����~ */
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

      /* �קK���Ǧ� %s �o�Ǧr�� */
      if ( strstr( pNew->description, "%" ) == NULL )
      {
        free_string( pObj->description );
        pObj->description = str_dup( pNew->description );
      }

      if ( ( victim = pObj->carried_by ) && verify_char( victim ) )
      {
        /* �ץ����q */
        victim->carry_weight -= pOld->weight;
        victim->carry_weight += pNew->weight;
        victim->carry_weight  = UMAX( 1, victim->carry_weight );

        /* �ץ��v�T */
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

      /* �ץ����A */
      if ( pNew->wear_flags != pOld->wear_flags )
      {
        /* ���Q�˳Ƶ� */
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

  /* �p�G���X���@�� */
  if ( pOld->vnum != pNew->vnum ) RETURN( FALSE );

  for ( pBounty = bounty_list; pBounty; pBounty = pBounty->next )
    if ( pBounty->mob == pOld ) pBounty->mob = pNew;

  if ( pNew->vnum == MobVampireVnum  ) MobVampire  = pNew;
  if ( pNew->vnum == MobPracticeVnum ) MobPractice = pNew;
  /* �m���쫬 */
  iHash = pOld->vnum % MAX_KEY_HASH;

  /* ������ */
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

  /* �A���J */
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

      /* ���O�p�� */
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

  /* ���վ� ???? */
  case ITEM_MAGICSTONE:
    break;
  }
  RETURN_NULL();
}

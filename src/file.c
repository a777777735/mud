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
#include <sys/stat.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <glob.h>
#include "file.h"
#include "merc.h"

AREA_DATA * load_zone   args( ( char * ) );
void check_capital      args( ( AREA_DATA * ) );
int  index_boot         args( ( char * , int , AREA_DATA * ) );
char LastFileName[MAX_FILE_LENGTH] = { '\x0' };

extern int      glob_file       args( ( const char *, const char *, glob_t * ) );

FILE * FOPEN( const char * path , const char * mode )
{
  FILE * pFile;

  PUSH_FUNCTION( "FOPEN" );

  if ( RecordFile ) mudlog( LOG_INFO, "�}���ɮסR%s", path );

  if ( fpReserve ) fclose( fpReserve );

  if ( !( pFile = fopen( path , mode ) ) )
  {
    mudlog( LOG_INFO , "�L�k�}���ɮ� %s %s", path, strerror( errno ) );
    fpReserve = fopen( null_file , "r" );
    RETURN( NULL );
  }

  fpReserve = NULL;
  RETURN( pFile );
}

void FCLOSE( FILE * stream )
{
  PUSH_FUNCTION( "FCLOSE" );

  fclose( stream );
  if ( !fpReserve ) fpReserve = fopen( null_file , "r" );

  RETURN_NULL();
}

/* �}�ҩҦ��ϰ쪺�D�n�{������ */
void open_area_directory( void )
{
  char        directory    [ MAX_FILE_LENGTH ];
  char        buf          [ MAX_FILE_LENGTH ];
  char        filename     [ MAX_FILE_LENGTH ];
  char        work_dir_name[ MAX_FILE_LENGTH ];
  char        index        [ MAX_FILE_LENGTH ];
  char      * word;
  FILE_DATA * pFile;
  AREA_DATA * pArea;

  PUSH_FUNCTION( "open_area_directory" );

  /* �����ؿ��åB��ݬO�_�n��J�ؿ����O�� */
  fill_path( str_cpy( directory, area_dir ) );

  /* ���ϰ��ɮץؿ��C���� */
  sprintf( filename , "%s%s" , directory , dir_file );

  /* �ݬݬO�_�i�H�}�� */
  if ( ( pFile = f_open( filename , "r" ) ) )
  {
    /* �@��Ū���ɮץ��ݤ~���� */
    while ( !if_eof( pFile ) )
    {
      /* Ū�J�Q�nŪ�����ϰ�ؿ� */
      word = fread_word( pFile );
      if ( word[0] == '*' )
      {
        fread_to_eol( pFile );
        continue;
      }

      sprintf( buf , "%s%s" , directory , word );

      /* �ˬd�O�_���i�H�s�����l�ؿ� */
      if ( is_directory( buf ) )
      {
        /* �A��ݬO�_�������� */
        sprintf( work_dir_name , "%s%s" , directory , word );
        fill_path( work_dir_name );
        sprintf( index , "%s%s" , work_dir_name , index_file );

        /* �p�G�������ɫhŪ�J�Ҧ���� */
        if ( is_regular( index ) )
        {
          pArea = load_zone( work_dir_name );
          if ( !DefaultArea ) DefaultArea = pArea;
        }

        /* ���M�e�X���~���T�� */
        else
        {
          mudlog( LOG_ERR , "�ϰ�ؿ� %s �S��������" , work_dir_name );
        }
      }

      /* �ϰ�ؿ��ڥ�����s�� */
      else
      {
        mudlog( LOG_ERR , "�ϰ�ؿ� %s �L�k�s��" , buf );
      }
    }
    f_close( pFile );
  }

  /* �ڥ��S���ϰ�ؿ��C����, �e�X���~�T�� */
  else
  {
    mudlog( LOG_ERR , "�S���ϰ�l�ؿ��C����" );
  }

  if ( !DefaultArea ) mudlog( LOG_ERR , "�S���w�]�ϰ�." );

  /* Ū�J���a�g���ϰ� */
  if ( PlayerArea[0] )
  {
    /* �ˬd�O�_���i�H�s�����l�ؿ� */
    if ( is_directory( PlayerArea ) )
    {
      /* �A��ݬO�_�������� */
      str_cpy( work_dir_name, PlayerArea );
      fill_path( work_dir_name );
      sprintf( index , "%s%s" , work_dir_name , index_file );

      /* �p�G�������ɫhŪ�J�Ҧ���� */
      if ( is_regular( index ) )
      {
        PlayerZone = load_zone( work_dir_name );
      }

      /* ���M�e�X���~���T�� */
      else
      {
        mudlog( LOG_ERR , "���a�ϰ�ؿ� %s �S��������" , PlayerArea );
      }
    }

    /* �ϰ�ؿ��ڥ�����s�� */
    else
    {
      mudlog( LOG_ERR , "���a�ϰ�ؿ� %s �L�k�s��" , PlayerArea );
    }
  }

  RETURN_NULL();
}

/* �ݬݬO�_�����n��J�ؿ������Ÿ� '/' */
void fill_path( char * pathname )
{
  PUSH_FUNCTION( "fill_path" );

  /* �ؿ��W�٪��׬��s, �@�w���~ */
  if ( !pathname || !*pathname ) mudlog( LOG_ERR , "���J�ؿ������~" );

  /* �ݬݬO�_�����n��J�ؿ������Ÿ� '/' */
  if ( pathname[ str_len( pathname ) - 1 ] != '/' ) str_cat( pathname , "/" );

  RETURN_NULL();
}

/* �d�߬O�_�u�����o���ɮ� */
bool is_regular( const char * filename )
{
  struct stat st;

  PUSH_FUNCTION( "is_regular" );

  if ( stat( filename, &st ) == 0 && S_ISREG( st.st_mode ) )
    RETURN( TRUE );

  RETURN( FALSE );
}

/* �d�߬O�_�u�����o�ӥؿ� */
bool is_directory( const char * path )
{
  struct stat st;

  PUSH_FUNCTION( "is_directory" );

  if ( stat( path, &st ) == 0 && S_ISDIR( st.st_mode ) ) RETURN( TRUE );
  RETURN( FALSE );
}

/* ���J��ӥؿ��Ҧ������ */
AREA_DATA * load_zone( char * pathname )
{
  AREA_DATA * pArea;
  char        index_file_name[ MAX_FILE_LENGTH ];

  PUSH_FUNCTION( "load_zone" );

  /* ���]�w�����ɪ��ɦW */
  sprintf( index_file_name , "%s%s" , pathname , index_file );

  /* ���J�����ɮ� */
  if ( !( pArea = load_area( index_file_name ) ) )
    mudlog( LOG_ERR , "���J�ؿ� %s �����ɦ����D" , pathname );

  if ( !area_first ) area_first      = pArea;
  if (  area_last  ) area_last->next = pArea;

  area_last   = pArea;
  pArea->next = NULL;
  top_area++;

  /* ���J�ж� */
  pArea->room += index_boot( pathname , ROOM_MODE, pArea );

  /* ���J�Ǫ� */
  pArea->mob += index_boot( pathname , MOB_MODE, pArea );

  /* ���J���~ */
  pArea->obj += index_boot( pathname , OBJ_MODE, pArea );

  /* ���J�ө� */
  pArea->shop += index_boot( pathname , SHOP_MODE, pArea );

  /* ���J���m */
  pArea->reset += index_boot( pathname , RESET_MODE, pArea );

  /* ���J�q�� */
  pArea->reset += index_boot( pathname , MINERAL_MODE, pArea );

  /* �ˬd���� */
  check_capital( pArea );
  RETURN( pArea );
}

/* ���J�U���ɮ׮榡���D�{�� */
int index_boot( char * path , int mode , AREA_DATA * pArea )
{
  MOB_INDEX_DATA  * pMobIndex;
  OBJ_INDEX_DATA  * pObjIndex;
  ROOM_INDEX_DATA * pRoomIndex;
  SHOP_DATA       * pShop;
  MINERAL_DATA    * pMineral;
  glob_t            result;
  char              dir_name[ MAX_FILE_LENGTH ];
  char              filename[ MAX_FILE_LENGTH ];
  char              ext[ MAX_FILE_LENGTH];
  int               count;
  int               loop;
  int               rc;
  int               iHash;

  PUSH_FUNCTION( "index_boot" );

  /* ���ݬݻݭn���@�ت��ؿ��W�� */
  switch( mode )
  {
  /* ���~���Ҧ� */
  default:
    mudlog( LOG_ERR , "Index_boot: ���w�q������ %d." , mode );

  /* ���J�ж� */
  case ROOM_MODE:
    sprintf( dir_name , "%s%s" , path , room_dir );
    strcpy( ext, room_ext );
    break;

  /* ���J�Ǫ� */
  case MOB_MODE:
    sprintf( dir_name , "%s%s" , path , mob_dir );
    strcpy( ext, mob_ext );
    break;

  /* ���J���~ */
  case OBJ_MODE:
    sprintf( dir_name , "%s%s" , path , obj_dir );
    strcpy( ext, obj_ext );
    break;

  /* ���J�ө� */
  case SHOP_MODE:
    sprintf( dir_name , "%s%s" , path , shop_dir );
    strcpy( ext, shop_ext );
    break;

  /* ���J���m */
  case RESET_MODE:
    sprintf( dir_name , "%s%s" , path , reset_dir );
    strcpy( ext, reset_ext );
    break;

  /* ���J�q�� */
  case MINERAL_MODE:
    sprintf( dir_name , "%s%s" , path , mineral_dir );
    strcpy( ext, mineral_ext );
    break;
  }

  fill_path( dir_name );
  count = 0;

  if ( ( rc = glob_file( dir_name, ext, &result ) ) >= 0 )
  {
    for ( loop = 0; loop < rc; loop++ )
    {
      sprintf( filename, "%s%s", dir_name, result.gl_pathv[loop] );
      if ( !is_regular( filename ) ) continue;

      /* �A�ݬݸոխ��@�بӨM�w���J���{�� */
      switch( mode )
      {
      /* ���~���Ҧ� */
      default :
        mudlog( LOG_ERR , "Index_boot: ���w�q������" );

      /* ���J�ж� */
      case ROOM_MODE:

        /* ���J��� */
        pRoomIndex = load_room( filename );

        pRoomIndex->area       = pArea;
        pRoomIndex->filename   = str_dup( filename );
        iHash                  = pRoomIndex->vnum % MAX_KEY_HASH;
        pRoomIndex->next       = room_index_hash[iHash];
        room_index_hash[iHash] = pRoomIndex;
        top_room++;
        count++;

        if ( pArea->list )
        {
          pRoomIndex->next_in_area = pArea->list;
          pArea->list              = pRoomIndex;
        }
        else
        {
          pArea->list = pRoomIndex;
        }

        break;

      /* ���J�Ǫ� */
      case MOB_MODE:

        /* �}���ɮרåB���J��� */
        if ( !( pMobIndex = load_mobiles( filename ) ) )
          mudlog( LOG_DEBUG, "index_boot: ���J�Ǫ�����." );

        if ( get_mob_index( pMobIndex->vnum ) )
          mudlog( LOG_DEBUG, "index_boot: ���J���ƩǪ� %s ���X %d."
            , filename, pMobIndex->vnum );

        pMobIndex->area       = pArea;
        iHash                 = pMobIndex->vnum % MAX_KEY_HASH;
        pMobIndex->next       = mob_index_hash[iHash];
        mob_index_hash[iHash] = pMobIndex;
        top_mob_index++;
        kill_table[URANGE(0, pMobIndex->level, MAX_LEVEL-1)].number++;

        count++;
        break;

      /* ���J���~ */
      case OBJ_MODE:

        if ( !( pObjIndex = load_object( filename ) ) )
          mudlog( LOG_DEBUG, "index_boot: ���J���~����." );

        if ( get_obj_index( pObjIndex->vnum ) )
          mudlog( LOG_DEBUG, "index_boot: ���J���ƪ��~ %s ���X %d."
            , filename, pObjIndex->vnum );

        iHash                 = pObjIndex->vnum % MAX_KEY_HASH;
        pObjIndex->next       = obj_index_hash[iHash];
        obj_index_hash[iHash] = pObjIndex;
        top_obj_index++;

        count++;
        break;

      /* ���J�ө� */
      case SHOP_MODE:

        /* �}���ɮרåB���J��� */
        pShop = load_shop( filename );

        pShop->filename  = str_dup( filename );

        if ( !shop_first ) shop_first      = pShop;
        if (  shop_last  ) shop_last->next = pShop;

        shop_last   = pShop;
        pShop->next = NULL;
        top_shop++;
        count++;
        break;

      /* ���J�q�� */
      case MINERAL_MODE:

        /* �}���ɮרåB���J��� */
        pMineral = load_mineral( filename );
        pMineral->next = pArea->mineral;
        pArea->mineral = pMineral;
        top_mineral++;
        count++;
        break;

      /* ���J���m */
      case RESET_MODE:

        /* �}���ɮרåB���J��� */
        load_resets( filename , pArea );
        count++;
        break;
      }
    }
    globfree( &result );
  }

  RETURN( count );
}

void check_capital( AREA_DATA * pArea )
{
  ROOM_INDEX_DATA * pRoom;
  int               capital;

  PUSH_FUNCTION( "check_capital" );

  if ( !pArea ) mudlog( LOG_ERR, "check_capital: �ӷ������T." );

  if ( ( capital = pArea->capital_no ) <= 0 ) RETURN_NULL();

  for ( pRoom = pArea->list; pRoom; pRoom = pRoom->next_in_area )
  {
    if ( pRoom->vnum == capital )
    {
      pArea->capital = pRoom;
      RETURN_NULL();
    }
  }
  mudlog( LOG_ERR, "�ϰ� %s ������ %d ���s�b.", pArea->name, capital );
  RETURN_NULL();
}

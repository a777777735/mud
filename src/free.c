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

#define OID                     OBJ_INDEX_DATA

void free_obj_index( OID * pIndex )
{
  EXTRA_DESCR_DATA * pExtra;
  EXTRA_DESCR_DATA * zExtra;
  RESTRICT_DATA    * pRest;
  RESTRICT_DATA    * zRest;
  MESSAGE_DATA     * pMessage;
  MESSAGE_DATA     * zMessage;
  AFFECT_DATA      * pAffect;
  AFFECT_DATA      * zAffect;
  JOB_DATA         * pJob;
  JOB_DATA         * zJob;

  PUSH_FUNCTION( "free_obj_index" );

  for ( pExtra = pIndex->extra_descr; pExtra; pExtra = zExtra )
  {
    zExtra = pExtra->next;
    free_extra_descr( pExtra );
  }

  for ( pAffect = pIndex->affected; pAffect; pAffect = zAffect )
  {
    zAffect = pAffect->next;
    free_affect( pAffect );
  }

  for ( pAffect = pIndex->impact; pAffect; pAffect = zAffect )
  {
    zAffect = pAffect->next;
    free_affect( pAffect );
  }

  for ( pJob = pIndex->job; pJob; pJob = zJob )
  {
    zJob = pJob->next;
    free_job( pJob );
  }

  for ( pRest = pIndex->restrict; pRest; pRest = zRest )
  {
    zRest = pRest->next;
    free_restrict( pRest );
  }

  for ( pMessage = pIndex->message; pMessage; pMessage = zMessage )
  {
    zMessage = pMessage->next;
    free_message( pMessage );
  }

  free_string( pIndex->name        );
  free_string( pIndex->short_descr );
  free_string( pIndex->description );
  free_string( pIndex->filename    );
  free_string( pIndex->wearmsg     );

  if ( pIndex->unit != DefaultUnit ) free_string( pIndex->unit        );

  free_struct( pIndex, STRUCT_OBJ_INDEX_DATA );
  RETURN_NULL();
}

void free_affect( AFFECT_DATA * pAffect )
{
  PUSH_FUNCTION( "free_affect" );

  free_struct( pAffect, STRUCT_AFFECT_DATA );
  RETURN_NULL();
}

void free_restrict( RESTRICT_DATA * pRestrict )
{
  PUSH_FUNCTION( "free_restrict" );
  free_struct( pRestrict, STRUCT_RESTRICT_DATA );
  RETURN_NULL();
}

void free_message( MESSAGE_DATA * pMessage )
{
  PUSH_FUNCTION( "free_message" );

  free_string( pMessage->self   );
  free_string( pMessage->others );
  free_struct( pMessage, STRUCT_MESSAGE_DATA );
  RETURN_NULL();
}

void free_extra_descr( EXTRA_DESCR_DATA * pExtra )
{
  PUSH_FUNCTION( "free_extra_descr" );

  free_string( pExtra->keyword );
  free_string( pExtra->description );
  free_struct( pExtra, STRUCT_EXTRA_DESCR_DATA );
  RETURN_NULL();
}

void free_job( JOB_DATA * pJob )
{
  PUSH_FUNCTION( "free_job" );

  free_string( pJob->keyword );
  free_struct( pJob, STRUCT_JOB_DATA );

  RETURN_NULL();
}

void free_mob_index( MOB_INDEX_DATA * pMobIndex )
{
  MPROG_DATA  * pProg;
  MPROG_DATA  * zProg;
  AFFECT_DATA * pAffect;
  AFFECT_DATA * zAffect;
  TEACH_DATA  * pTeach;
  TEACH_DATA  * zTeach;
  ENABLE_DATA * pEnable;
  ENABLE_DATA * zEnable;
  JOB_DATA    * pJob;
  JOB_DATA    * zJob;

  PUSH_FUNCTION( "free_mob_index" );

  free_string( pMobIndex->name        );
  free_string( pMobIndex->short_descr );
  free_string( pMobIndex->long_descr  );
  free_string( pMobIndex->description );
  free_string( pMobIndex->filename    );
  free_string( pMobIndex->deadmsg     );
  free_string( pMobIndex->auction     );
  free_string( pMobIndex->temp        );

  /* 釋放教導 */
  for ( pTeach = pMobIndex->teach; pTeach; pTeach = zTeach )
  {
    zTeach = pTeach->next;
    free_teach( pTeach );
  }

  /* 釋放致能 */
  for ( pEnable = pMobIndex->enable; pEnable; pEnable = zEnable )
  {
    zEnable = pEnable->next;
    free_enable( pEnable );
  }

  for ( pJob = pMobIndex->job; pJob; pJob = zJob )
  {
    zJob = pJob->next;
    free_job( pJob );
  }

  for ( pAffect = pMobIndex->affected; pAffect; pAffect = zAffect )
  {
    zAffect = pAffect->next;
    free_affect( pAffect );
  }

  for ( pProg = pMobIndex->mobprogs; pProg; pProg = zProg )
  {
    zProg = pProg->next;
    free_mob_program( pProg );
  }

  RETURN_NULL();
}

void free_teach( TEACH_DATA * pTeach )
{
  PUSH_FUNCTION( "free_teach" );
  free_struct( pTeach, STRUCT_TEACH_DATA );
  RETURN_NULL();
}

void free_enable( ENABLE_DATA * pEnable )
{
  PUSH_FUNCTION( "free_enable" );
  free_struct( pEnable, STRUCT_ENABLE_DATA );
  RETURN_NULL();
}

void free_mob_program( MPROG_DATA * pProg )
{
  PUSH_FUNCTION( "free_mob_program" );

  free_string( pProg->arglist );
  free_string( pProg->comlist );
  free_struct( pProg, STRUCT_MPROG_DATA );
  RETURN_NULL();
}

void free_basic( BASIC_DATA * pBasic )
{
  PUSH_FUNCTION( "free_basic" );

  free_string( pBasic->name    );
  free_string( pBasic->cname   );
  free_string( pBasic->email   );
  free_string( pBasic->address );
  free_string( pBasic->club    );
  free_string( pBasic->class   );
  free_struct( pBasic, STRUCT_BASIC_DATA );

  RETURN_NULL();
}

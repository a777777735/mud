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
#include "merc.h"

/*
第一個參數是攻擊者
第二個是被攻擊者
第三個參數是攻擊者的熟練度
*/

bool routine_none( CHAR_DATA * ch, CHAR_DATA * victim, int adpet )
{
  PUSH_FUNCTION( "routine_none" );

  if ( !ch || !victim )
  {
    mudlog( LOG_DEBUG, "routine_none: 缺乏來源." );
    RETURN( FALSE );
  }

  RETURN( TRUE );
}

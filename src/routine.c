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
#include "merc.h"

/*
�Ĥ@�ӰѼƬO������
�ĤG�ӬO�Q������
�ĤT�ӰѼƬO�����̪����m��
*/

bool routine_none( CHAR_DATA * ch, CHAR_DATA * victim, int adpet )
{
  PUSH_FUNCTION( "routine_none" );

  if ( !ch || !victim )
  {
    mudlog( LOG_DEBUG, "routine_none: �ʥF�ӷ�." );
    RETURN( FALSE );
  }

  RETURN( TRUE );
}

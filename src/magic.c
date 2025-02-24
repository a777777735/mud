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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "merc.h"

SKILL_DATA * skill_isname( const char * name )
{
  SKILL_DATA * pSkill;
  char       * pSource;
  char       * pTarget;
  char         source[ MAX_INPUT_LENGTH ];
  char         target[ MAX_INPUT_LENGTH ];
  char         arg1  [ MAX_INPUT_LENGTH ];
  char         arg2  [ MAX_INPUT_LENGTH ];

  PUSH_FUNCTION( "skill_isname" );

  for ( pSkill = skill_list; pSkill; pSkill = pSkill->next )
  {
    if ( LOWER( pSkill->name[0] == LOWER( name[0] ) ) )
    {
      str_cpy( ( pSource = source ), pSkill->name );
      str_cpy( ( pTarget = target ), name         );

      do
      {
        pTarget = one_argument( pTarget , arg2 );
        if ( !arg2[0] ) RETURN( pSkill );

        pSource = one_argument( pSource , arg1 );
        if ( !arg1[0] || str_prefix( arg2, arg1 ) ) break;
      } while ( arg1[0] && arg2[0] );
    }
  }
  RETURN( NULL );
}

SKILL_DATA * get_skill( int sn )
{
  SKILL_DATA * pSkill;

  PUSH_FUNCTION( "get_skill" );

  for ( pSkill = skill_list; pSkill; pSkill = pSkill->next )
    if ( pSkill->slot == sn ) RETURN( pSkill );

  RETURN( NULL );
}

/* Compute a saving throw. Negative apply's make saving throw better. */
bool saves_spell( int level, CHAR_DATA *victim )
{
  int save;

  PUSH_FUNCTION( "saves_spell" );

  save = 50 + ( victim->level - level - victim->saving_throw ) * 5;
  save = URANGE( 5, save, 95 );

  RETURN( number_percent() < save );
}

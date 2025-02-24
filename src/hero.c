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
#include <time.h>
#include "merc.h"

void    hero_remove             args( ( HERO_DATA * ) );

FUNCTION( do_hero )
{
  HERO_DATA * pHero;
  CHAR_DATA * victim;
  time_t      timer;
  struct tm * sTime;
  char        arg[MAX_INPUT_LENGTH];
  char        timebuf[MAX_STRING_LENGTH];
  char        buf1[MAX_INPUT_LENGTH];
  char        buf2[MAX_INPUT_LENGTH];
  char        buf3[MAX_INPUT_LENGTH];
  char        buf4[MAX_INPUT_LENGTH];
  char        buf5[MAX_INPUT_LENGTH];
  int         loop;
  int         total;

  PUSH_FUNCTION( "do_hero" );

  argument = one_argument( argument , arg );

  if ( arg[0] == '\x0' || arg[0] == '*' )
  {
    if ( !hero_first )
    {
      act( "$t�٨S�����ͥ���^���C�T", ch, mud_name, NULL, TO_CHAR );
      RETURN_NULL();
    }

    if ( arg[0] == '*' )
    {
      total = -MaxHeroShow;
    }
    else
    {
      for ( total = 0, pHero = hero_first; pHero; pHero = pHero->next ) total++;
    }

    clear_buffer();
    send_to_buffer( "\e[1;33;44m�s�� �^  �� �W �� ¾  �~ ��    ��    �W    ��"
      " ���W�ɶ�                          \e[0m\n\r" );

    for ( loop = 0, pHero = hero_first; pHero; pHero = pHero->next, loop++ )
    {
      timer = pHero->timer;

      if ( loop < total - MaxHeroShow
        || !pHero->name
        || !*pHero->name
        || !pHero->cname
        || !*pHero->cname
        || !( sTime = localtime( &timer ) ) )
        continue;

      chinese_number( sTime->tm_year - 11 , buf1 );
      chinese_number( sTime->tm_mon  +  1 , buf2 );
      chinese_number( sTime->tm_mday      , buf3 );
      chinese_number( sTime->tm_min       , buf5 );
      chinese_number( sTime->tm_hour > 12
        ? sTime->tm_hour - 12 : sTime->tm_hour, buf4 );

      sprintf( timebuf, "%s�~%s��%s��%s%s�I%s��"
        , buf1, buf2, buf3, sTime->tm_hour < 12 ? "���W" : "�U��", buf4, buf5 );

      timebuf[34] = '\x0';

      send_to_buffer( "%4d ", loop );
      send_to_buffer( "%-s "   , format_string( pHero->name , 12 ) );
      send_to_buffer( "%-s "   , format_string( pHero->class,  6 ) );
      send_to_buffer( "%-s "   , format_string( pHero->cname, 20 ) );
      send_to_buffer( "%s\n\r" , format_string( timebuf     , 34 ) );

      if ( buffer_full() ) break;
    }

    print_buffer( ch );
  }

  else if ( !str_prefix( arg, "!add" ) && IS_IMMORTAL( ch ) )
  {
    one_argument( argument, arg );

    if ( arg[0] == '\x0' )
    {
      send_to_char( "�A�n�W�[���@�쪱�a���^���O�S\n\r", ch );
      RETURN_NULL();
    }

    if ( !( victim = get_char_world( ch, arg ) ) )
    {
      act( "�䤣��A�n�ɮ檺�H $2$T$0�C", ch, NULL, arg, TO_CHAR );
      RETURN_NULL();
    }

    if ( IS_NPC( victim ) )
    {
      act( "�A�����Ǫ�$N�ɮ欰�^���C", ch, NULL, victim, TO_CHAR );
      RETURN_NULL();
    }

    for ( pHero = hero_first; pHero; pHero = pHero->next )
    {
      if ( !str_cmp( pHero->name, victim->name ) )
      {
        act( "$N�w�g�b�^���]�̤F�T", ch, NULL, victim, TO_CHAR );
        RETURN_NULL();
      }
    }

    chinese_number( hero_count() + 1, buf1 );
    sprintf( timebuf, "���H%s����%s\e[0m��%s��^���T\n\r"
      , mob_name( NULL, victim ), mud_name, buf1 );

    send_to_all_char( timebuf );

    act( "���H�A����$t$0��$T��^���T", victim, mud_name, buf1, TO_CHAR );
    if ( ch != victim ) send_to_char( "�W�[�^���]�����C\n\r", ch );

    /* �W�[�^�� */
    hero_add( victim );

    /* �g�J�ɮ� */
    update_hero();
  }

  else if ( !str_prefix( arg, "!delete" ) && IS_IMMORTAL( ch ) )
  {
    one_argument( argument, arg );

    if ( arg[0] == '\x0' )
    {
      send_to_char( "�A�n����@���X�^���]�~�S\n\r", ch );
      RETURN_NULL();
    }

    for ( pHero = hero_first; pHero; pHero = pHero->next )
      if ( !str_cmp( pHero->name, arg ) ) break;

    if ( !pHero )
    {
      act( "�藍�_�M�^���]���S�� $2$T$0�T", ch, NULL, arg, TO_CHAR );
      RETURN_NULL();
    }

    /* �R���^�� */
    hero_remove( pHero );

    /* �g�J�ɮ� */
    update_hero();

    send_to_char( "�R���^���T\n\r", ch );

    for ( victim = char_list; victim; victim = victim->next )
    {
      if ( verify_char( victim )
        && !IS_NPC( victim )
        && !str_cmp( victim->name, arg ) )
      {
        send_to_char( "�藍�_�M�A�Q��X�^���]�~�F�T\n\r", victim );
        break;
      }
    }
  }

  else
  {
    send_to_char( "�y�k���~�M�Ьd�� \e[1;32mhero\e[0m ���T���Ϊk�C\n\r", ch );
  }
  RETURN_NULL();
}

size_t hero_count( void )
{
  HERO_DATA * pHero;
  size_t      count;

  PUSH_FUNCTION( "hero_count" );
  for ( count = 0, pHero = hero_first; pHero; pHero = pHero->next ) count++;
  RETURN( count );
}

/* �W�[�άO���N */
void hero_add( CHAR_DATA * ch )
{
  HERO_DATA * pHero;

  PUSH_FUNCTION( "hero_add" );

  if ( !ch || IS_NPC( ch ) )
  {
    mudlog( LOG_DEBUG, "hero_add: �ӷ������~." );
    RETURN_NULL();
  }

  /* �M��O�_�w�g���L�F */
  for ( pHero = hero_first; pHero; pHero = pHero->next )
    if ( !str_cmp( pHero->name, ch->name ) ) break;

  /* ���h�i����N */
  if ( pHero )
  {
    free_string( pHero->name  );
    free_string( pHero->cname );
    free_string( pHero->class );

    pHero->name  = str_dup( ch->name  );
    pHero->cname = str_dup( ch->cname );
    pHero->class = str_dup( class_name( ch->class, FALSE ) );
    pHero->timer = time( NULL );
  }

  /* ���M�t�m�@�ӷs�� */
  else
  {
    /* ��X�̫�@�� */
    for ( pHero = hero_first; pHero; pHero = pHero->next ) hero_last = pHero;

    pHero        = alloc_struct( STRUCT_HERO_DATA );
    pHero->name  = str_dup( ch->name  );
    pHero->cname = str_dup( ch->cname );
    pHero->class = str_dup( class_name( ch->class, FALSE ) );
    pHero->timer = time( NULL );

    if ( !hero_first ) hero_first        = pHero;
    if (  hero_last  ) hero_last->next   = pHero;

    hero_last   = pHero;
    pHero->next = NULL;
  }

  RETURN_NULL();
}

/* �����^����� */
void hero_remove( HERO_DATA * pHero )
{
  HERO_DATA * aHero;
  HERO_DATA * zHero;

  PUSH_FUNCTION( "hero_remove" );

  if ( !pHero )
  {
    mudlog( LOG_DEBUG, "hero_remove: �ӷ����~." );
    RETURN_NULL();
  }

  for ( zHero = NULL, aHero = hero_first;
        aHero;
        zHero = aHero, aHero = aHero->next )
  {
    if ( aHero == pHero )
    {
      if ( !zHero ) hero_first  = aHero->next;
      else          zHero->next = aHero->next;

      free_string( pHero->name  );
      free_string( pHero->cname );
      free_string( pHero->class );

      free_struct( pHero , STRUCT_HERO_DATA );
      RETURN_NULL();
    }
  }

  mudlog( LOG_DEBUG, "hero_remove: ���o�{����R�����^��." );
  RETURN_NULL();
}

/* �g�J�ɮ� */
void update_hero( void )
{
  HERO_DATA * pHero;
  FILE      * pFile;

  PUSH_FUNCTION( "update_hero" );

  if ( !( pFile = FOPEN( hero_file, "w" ) ) )
  {
    mudlog( LOG_DEBUG, "update_hero: �L�k�g�J�^���ɮ�%s.", hero_file );
    RETURN_NULL();
  }

  for ( pHero = hero_first; pHero; pHero = pHero->next )
  {
    fprintf( pFile, "Hero    %s~ %s~ %s~ %d\n"
      , pHero->name, pHero->cname, pHero->class, pHero->timer );
  }

  fprintf( pFile, "End" );
  FCLOSE( pFile );

  RETURN_NULL();
}

void check_hero( char * name )
{
  HERO_DATA * pHero;

  PUSH_FUNCTION( "check_hero" );

  if ( !name || !*name )
  {
    mudlog( LOG_DEBUG, "check_hero: �ӷ����~." );
    RETURN_NULL();
  }

  for ( pHero = hero_first; pHero; pHero = pHero->next )
  {
    if ( !str_cmp( pHero->name, name ) )
    {
      /* �R���^�� */
      hero_remove( pHero );

      /* �g�J�ɮ� */
      update_hero();

      RETURN_NULL();
    }
  }

  RETURN_NULL();
}

/* �W�[�ݩ� */
void add_hero_bonus( CHAR_DATA * ch )
{
  int          loop;
  int          max;
  int          value;
  int          old;
  CLASS_DATA * pClass;
  char         buf[MAX_STRING_LENGTH];

  PUSH_FUNCTION( "add_hero_bonus" );

  if ( !ch || !( pClass = ch->class ) )
  {
    mudlog( LOG_DEBUG, "add_hero_bonus: �ӷ����~." );
    RETURN_NULL();
  }

  if ( ch->level != LEVEL_HERO || IS_NPC( ch ) ) RETURN_NULL();

  if ( HeroFirman > 0 )
  {
    chinese_number( add_firman( ch, HeroFirman ), buf );
    act( "�t�ή��H�A�ɨ�^���M�ҥH�e�A$t�i�K�����P�C", ch, buf, NULL, TO_CHAR );
  }

  for ( loop = 0; loop < MAX_ATTR; loop++ )
  {
    if ( ( value = pClass->hero[loop] ) > 0 )
    {
      max = pClass->attr[loop];

      switch( loop )
      {
      case STR_ITEM:

        old         = ch->now_str;
        ch->now_str = UMIN( max, old + value );

        if ( ch->now_str > old )
          send_to_char( "���H�A�M�A���O�q�ܤj�F�T\n\r", ch );

        break;

      case INT_ITEM:

        old         = ch->now_int;
        ch->now_int = UMIN( max, old + value );

        if ( ch->now_int > old )
          send_to_char( "���H�A�M�A�����O�W�[�F�T\n\r", ch );

        break;

      case WIS_ITEM:

        old         = ch->now_wis;
        ch->now_wis = UMIN( max, old + value );

        if ( ch->now_wis > old )
          send_to_char( "���H�A�M�A�����ѼW�s�F�T\n\r", ch );

        break;

      case DEX_ITEM:

        old         = ch->now_dex;
        ch->now_dex = UMIN( max, old + value );

        if ( ch->now_dex > old )
          send_to_char( "���H�A�M�A���ӱ��W���F�T\n\r", ch );

        break;

      case CON_ITEM:

        old         = ch->now_con;
        ch->now_con = UMIN( max, old + value );

        if ( ch->now_con > old )
          send_to_char( "���H�A�M�A������ܧ��F�T\n\r", ch );

        break;
      }
    }
  }

  RETURN_NULL();
}

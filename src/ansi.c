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
#include "merc.h"

void   translate_ansi     args( ( char * ) );
void   purge_token        args( ( char * ) );
char * ansi_string        args( ( char *, char * ) );
size_t ansi_strlen        args( ( const char * ) );

#define SMASH_TOKEN     '-';
#define CHAR_FILL       ' ';
#define MAX_ELEMENT     82 * 100        /* �̦h�i�H���h�֭Ӥ��� */
#define MAX_ANSI_LENGTH 16
#define NO_COLOR        ERRORCODE

const   struct  ansi_data       ansi_info       []      =
{
  { "\e[30m"   },  /* �e���C��¦�       */
  { "\e[31m"   },  /* �e���C�����       */
  { "\e[32m"   },  /* �e���C����       */
  { "\e[33m"   },  /* �e���C�����       */
  { "\e[34m"   },  /* �e���C���Ŧ�       */
  { "\e[35m"   },  /* �e���C�⵵��       */
  { "\e[36m"   },  /* �e���C��Ѫ�       */
  { "\e[37m"   },  /* �e���C��զ�       */
  { "\e[1;31m" },  /* �e�����G���C����� */
  { "\e[1;32m" },  /* �e�����G���C���� */
  { "\e[1;33m" },  /* �e�����G���C����� */
  { "\e[1;34m" },  /* �e�����G���C���Ŧ� */
  { "\e[1;35m" },  /* �e�����G���C�⵵�� */
  { "\e[1;36m" },  /* �e�����G���C��Ѫ� */
  { "\e[1;37m" },  /* �e�����G���C��զ� */
  { "\e[5;31m" },  /* �e�����G���C����� */
  { "\e[5;32m" },  /* �e�����G���C���� */
  { "\e[5;33m" },  /* �e�����G���C����� */
  { "\e[5;34m" },  /* �e�����G���C���Ŧ� */
  { "\e[5;35m" },  /* �e�����G���C�⵵�� */
  { "\e[5;36m" },  /* �e�����G���C��Ѫ� */
  { "\e[5;37m" },  /* �e�����G���C��զ� */
  { "\e[31;1m" },  /* �e�����G���C����� */
  { "\e[32;1m" },  /* �e�����G���C���� */
  { "\e[33;1m" },  /* �e�����G���C����� */
  { "\e[34;1m" },  /* �e�����G���C���Ŧ� */
  { "\e[35;1m" },  /* �e�����G���C�⵵�� */
  { "\e[36;1m" },  /* �e�����G���C��Ѫ� */
  { "\e[37;1m" },  /* �e�����G���C��զ� */
  { "\e[31;5m" },  /* �e�����G���C����� */
  { "\e[32;5m" },  /* �e�����G���C���� */
  { "\e[33;5m" },  /* �e�����G���C����� */
  { "\e[34;5m" },  /* �e�����G���C���Ŧ� */
  { "\e[35;5m" },  /* �e�����G���C�⵵�� */
  { "\e[36;5m" },  /* �e�����G���C��Ѫ� */
  { "\e[37;5m" },  /* �e�����G���C��զ� */
  { "\e[5;1;31m" },  /* �e�����G���C����� */
  { "\e[5;1;32m" },  /* �e�����G���C���� */
  { "\e[5;1;33m" },  /* �e�����G���C����� */
  { "\e[5;1;34m" },  /* �e�����G���C���Ŧ� */
  { "\e[5;1;35m" },  /* �e�����G���C�⵵�� */
  { "\e[5;1;36m" },  /* �e�����G���C��Ѫ� */
  { "\e[5;1;37m" },  /* �e�����G���C��զ� */
  { "\e[1;5;31m" },  /* �e�����G���C����� */
  { "\e[1;5;32m" },  /* �e�����G���C���� */
  { "\e[1;5;33m" },  /* �e�����G���C����� */
  { "\e[1;5;34m" },  /* �e�����G���C���Ŧ� */
  { "\e[1;5;35m" },  /* �e�����G���C�⵵�� */
  { "\e[1;5;36m" },  /* �e�����G���C��Ѫ� */
  { "\e[1;5;37m" },  /* �e�����G���C��զ� */
  { "\e[41;1m"   },  /* �I�����G���C����� */
  { "\e[42;1m"   },  /* �I�����G���C���� */
  { "\e[43;1m"   },  /* �I�����G���C����� */
  { "\e[44;1m"   },  /* �I�����G���C���Ŧ� */
  { "\e[45;1m"   },  /* �I�����G���C�⵵�� */
  { "\e[46;1m"   },  /* �I�����G���C��Ѫ� */
  { "\e[47;1m"   },  /* �I�����G���C��զ� */
  { "\e[40m"     },  /* �I���C��¦�       */
  { "\e[41m"     },  /* �I���C�����       */
  { "\e[42m"     },  /* �I���C����       */
  { "\e[43m"     },  /* �I���C�����       */
  { "\e[44m"     },  /* �I���C���Ŧ�       */
  { "\e[45m"     },  /* �I���C�⵵��       */
  { "\e[46m"     },  /* �I���C��Ѫ�       */
  { "\e[47m"     },  /* �I���C��զ�       */
  { "\e[0m"      },  /* �٭�               */
  { "\e[1;33;44m"},  /* �e���G���I���`�Ŧ� */
  { ""         }
};

struct ansi_table
{
  char * token;
  char * code;
};

const   struct  ansi_table      ansi_token      []      =
{
  { "$BLK$", "\e[30m"   },
  { "$RED$", "\e[31m"   },
  { "$GRN$", "\e[32m"   },
  { "$YEL$", "\e[33m"   },
  { "$BLU$", "\e[34m"   },
  { "$MAG$", "\e[35m"   },
  { "$CYN$", "\e[36m"   },
  { "$WHT$", "\e[37m"   },
  { "$NOR$", "\e[0m"    },
  { "$HIR$", "\e[1;31m" },
  { "$HIG$", "\e[1;32m" },
  { "$HIY$", "\e[1;33m" },
  { "$HIB$", "\e[1;34m" },
  { "$HIM$", "\e[1;35m" },
  { "$HIC$", "\e[1;36m" },
  { "$HIW$", "\e[1;37m" },
  { ""      }
};

/* �ഫ >> �� ESC ����X */
void translate_ansi( char * argument )
{
  PUSH_FUNCTION( "translate_ansi" );

  for ( ; *argument; argument++ )
  {
    if ( *argument == '>' && *(argument + 1 ) == '>' )
    {
      *argument++ = '\e';
      *argument   =  '[';
    }
  }

  RETURN_NULL();
}

/* �M�� << �t�Τ��w���Y�g�Ÿ� */
void purge_token( char * argument )
{
  PUSH_FUNCTION( "purge_token" );

  for ( ; *argument; argument++ )
  {
    if ( *argument == '<' && *(argument + 1 ) == '<' )
    {
      *argument++ = SMASH_TOKEN;
      *argument   = SMASH_TOKEN;
    }
  }

  RETURN_NULL();
}

/* �L�o�@�Ǽv�T�O�H��X�� ANSI ����X */
void filter_ansi( char * argument )
{
  PUSH_FUNCTION( "filter_ansi" );

  /* �ഫ >> �� ESC ����X */
  translate_ansi( argument );

  /* �M�� << �t�Τ��w���Y�g�Ÿ� */
  purge_token( argument );

  for ( ; *argument; argument++ )
  {
    if ( *argument == '\e' && *( argument + 1 ) == '[' )
    {
      for ( argument += 2 * sizeof( char ) ;; argument++ )
        if ( *argument != ';' && !isdigit( ( int ) *argument ) ) break;

      switch( *argument )
      {
      case 'A': /* ANSI ��ФW��           */
      case 'B': /* ANSI ��ФU��           */
      case 'C': /* ANSI ��Хk��           */
      case 'D': /* ANSI ��Х���           */
      case 'H': /* ANSI ��Щw��           */
      case 'J': /* ANSI �M���ù�           */
      case 'K': /* ANSI �M����ХH�᪺��r */
      case 's': /* ANSI ��ЫO�s�_��       */
      case 'u': /* ANSI ��Ц^�s           */
        *argument = CHAR_FILL;
        break;

      case 'm': case 'M':
        break;
      }
    }
  }

  RETURN_NULL();
}

size_t ansi_strlen( const char * pString )
{
  size_t len;

  PUSH_FUNCTION( "ansi_strlen" );

  if ( !pString || !*pString ) RETURN( 0 );

  for ( len = 0; *pString; pString++ )
  {
    if ( *pString == '\e' && *( pString + 1 ) == '[' )
    {
      for ( pString += 2; !isalpha( ( int ) *pString ); pString++ )
        if ( *pString == '\x0' ) RETURN( len );
    }

    else
    {
      len++;
    }
  }

  RETURN( len );
}

char * format_string( const char * pString, int len )
{
  static char   buf[MAX_STRING_LENGTH];
  char        * target;
  int           loop;
  int           real_len;
  int           diff_len;

  PUSH_FUNCTION( "format_string" );

  buf[0] = '\x0';

  /* �W�X�d�� */
  if ( len > sizeof( buf ) || len <= 0 || str_len( pString ) > sizeof( buf ) )
  {
    mudlog( LOG_DEBUG, "format_string: �ӷ��W�X�d��άO���X�z." );
    RETURN( "\x0" );
  }

  /* �Ŧr�� */
  if ( !pString || !*pString )
  {
    for ( loop = 0; len > 0; loop++ ) buf[loop] = ' ';
    buf[loop] = '\x0';
    RETURN( buf );
  }

  /* ���פ񭭩w���p�άO���� */
  if ( ( diff_len = len - ( real_len = ansi_strlen( pString ) ) ) >= 0 )
  {
    str_cpy( buf, pString );

    for ( target = buf + str_len( buf ); diff_len > 0; diff_len--, target++ )
      *target = ' ';

    *target = '\x0';
    str_cat( buf, "\e[0m" );
  }

  /* ���פ񭭩w���j */
  else
  {
    str_cpy( buf, pString );

    for ( target = buf + str_len( buf ) - 1; target >= buf; target-- )
    {
      *target = '\x0';
      if ( ansi_strlen( buf ) <= len ) break;
    }

    str_cat( buf, "\e[0m" );
  }

  RETURN( buf );
}

char * ansi_string( char * source, char * target )
{
  int iLen = 0;

  PUSH_FUNCTION( "ansi_string" );

  if ( *source == '\e' && *( source + 1 ) == '[' )
  {
    while ( iLen++ < MAX_ANSI_LENGTH - 1 && *source )
    {
      switch ( ( *target++ = *source++ ) )
      {
      case 'A': /* ANSI ��ФW��           */
      case 'B': /* ANSI ��ФU��           */
      case 'C': /* ANSI ��Хk��           */
      case 'D': /* ANSI ��Х���           */
      case 'H': /* ANSI ��Щw��           */
      case 'J': /* ANSI �M���ù�           */
      case 'K': /* ANSI �M����ХH�᪺��r */
      case 's': /* ANSI ��ЫO�s�_��       */
      case 'u': /* ANSI ��Ц^�s           */
      case 'm':
      case 'M':

        *target = '\x0';
        RETURN( source );
      }
    }
  }

  *target = '\x0';
  RETURN( source );
}

char * ansi_code( char * source, char * character, int * color )
{
  char ansi_control[MAX_ANSI_LENGTH + 1];
  int  loop;

  PUSH_FUNCTION( "ansi_code" );

  if ( color ) *color = NO_COLOR;

  for ( ;; )
  {
    source = ansi_string( source, ansi_control );

    if ( !ansi_control[0] )
    {
      if ( character && ( *character = *source ) ) source++;
      break;
    }

    for ( loop = 0; ansi_info[loop].code[0]; loop++ )
    {
      if ( !str_cmp( ansi_control, ansi_info[loop].code ) )
      {
        if ( color ) *color = loop;
        break;
      }
    }
  }

  RETURN( source );
}

/* �榡�Ʃw���X */
void tablize( int type, char * pString, int iNum, char * key, int len )
{
  static PALETTE_DATA   die[MAX_ELEMENT];
  char                * pAnsi;
  char                  sNumber[MAX_INPUT_LENGTH];
  int                   loop;
  int                   shift;
  int                   iLen;

  PUSH_FUNCTION( "tablize" );

  switch( type )
  {
  default:
    mudlog( LOG_DEBUG, "tablize: �ﶵ���~ %d.", type );
    break;

  /* �M���쥻�]�w */
  case FORMAT_CLEAN:

    for ( loop = 0; loop < MAX_ELEMENT; loop++ )
    {
      die[loop].character = '\x0';
      die[loop].color     = NO_COLOR;
    }

    break;

  /* �]�w */
  case FORMAT_SET:

    /* �M���쥻�]�w */
    tablize( FORMAT_CLEAN, NULL, 0, NULL, 0 );

    for ( loop = 0; *pString; loop++ )
    {
      if ( loop >= MAX_ELEMENT - 1 )
      {
        mudlog( LOG_DEBUG, "tablize: �����W�L�F�w�w�� %d.", MAX_ELEMENT );
        break;
      }

      pString = ansi_code( pString, &die[loop].character, &die[loop].color );
    }

    die[loop].character = '\x0';
    die[loop].color     = NO_COLOR;
    break;

  /* �Ǧ^�}�C */
  case FORMAT_RETURN:

    if ( !pString )
    {
      mudlog( LOG_DEBUG, "tablize: �]�w�Ȭ��Ū�." );
      break;
    }

    shift = NO_COLOR;
    for ( loop = 0; loop < MAX_ELEMENT && len > 1; loop++, len-- )
    {
      if ( die[loop].color != NO_COLOR && die[loop].color != shift )
      {
        shift = die[loop].color;
        pAnsi = ansi_info[ shift ].code;
        while ( *pAnsi && len-- > 2 ) *pString++ = *pAnsi++;
      }

      if ( !( *pString++ = die[loop].character ) ) break;
    }

    *pString = '\x0';
    break;

  /* �w��]�w�r�� */
  case FORMAT_STRING:

    if ( !pString )
    {
      mudlog( LOG_DEBUG, "tablize: �ӷ��r��O�Ū�." );
      break;
    }

    if ( !key || !*key )
    {
      mudlog( LOG_DEBUG, "tablize: ����r��O�Ū�, �L�k���." );
      break;
    }

    for ( loop = 0; loop < MAX_ELEMENT && die[loop].character; loop++ )
    {
      for ( shift = 0; loop + shift < MAX_ELEMENT; shift++ )
      {
        /* �r��ۦP */
        if ( *( key + shift ) == '\x0' )
        {
          sprintf( sNumber, "%d", loop );
          tablize( FORMAT_LOCATE_STRING, pString, 0, sNumber, len );
          RETURN_NULL();
        }

        if ( !die[loop+shift].character
          || *( key + shift ) != die[loop+shift].character ) break;
      }
    }

    mudlog( LOG_DEBUG, "tablize: �䤣�����r�� %s.", key );
    break;

  /* �Ʀr */
  case FORMAT_NUM:

    sprintf( sNumber, "%d", iNum );
    tablize( FORMAT_STRING, sNumber, 0, key, -len );
    break;

  /* �w��]�w�r�� */
  case FORMAT_LOCATE_STRING:

    if ( !pString )
    {
      mudlog( LOG_DEBUG, "tablize: �ӷ��r��O�Ū�." );
      break;
    }

    if ( !key || !is_number( key ) )
    {
      mudlog( LOG_DEBUG, "tablize: �w��r�ꤣ���T." );
      break;
    }

    if ( ( shift = atoi( key ) ) < 0 || shift >= MAX_ELEMENT )
    {
      mudlog( LOG_DEBUG, "tablize: �w��r�ꤣ�X�k %d.", shift );
      RETURN_NULL();
    }

    /* �ˬd���׬O�_�X�z */
    if ( len == 0 || shift + len - 1 >= MAX_ELEMENT )
    {
      mudlog( LOG_DEBUG, "tablize: �]�w���� %d ���X�k.", len );
      RETURN_NULL();
    }

    /* �ץ��Y�O�n�V�k��� */
    iLen = ( len < 0 ) ? ( len = -len ) - ansi_str_len( pString ) : 0;

    for ( ; len > 0 ;len--, shift++ )
    {
      if ( shift >= MAX_ELEMENT || !die[shift].character ) break;

      if ( iLen-- > 0 )
      {
        die[shift].character = CHAR_FILL;
        continue;
      }

      pString = ansi_code( pString, &die[shift].character, &die[shift].color );
      if ( !die[shift].character ) die[shift].character = CHAR_FILL;
    }

    break;

  case FORMAT_LOCATE_NUM:

    sprintf( sNumber, "%d", iNum );
    tablize( FORMAT_LOCATE_STRING, sNumber, 0, key, -len );
    break;
  }

  RETURN_NULL();
}

void ansi_transcribe( char * source, char * target )
{
  int    loop;
  bool   bChange;
  char * pTarget;

  PUSH_FUNCTION( "ansi_transcribe" );

  if ( !source || !target )
  {
    mudlog( LOG_DEBUG, "ansi_transcribe: �ʥF�ӷ�." );
    RETURN_NULL();
  }

  while ( *source )
  {
    for ( bChange = FALSE, loop = 0; ansi_token[loop].token[0]; loop++ )
    {
      if ( !str_prefix( ansi_token[loop].token, source ) )
      {
        for ( pTarget = ansi_token[loop].code; *pTarget; )
          *target++ = *pTarget++;

        source  += str_len( ansi_token[loop].token );
        bChange  = TRUE;
        break;
      }
    }

    if ( !bChange ) *target++ = *source++;
  }

  *target = '\x0';
  RETURN_NULL();
}

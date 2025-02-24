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
#define MAX_ELEMENT     82 * 100        /* 最多可以有多少個元素 */
#define MAX_ANSI_LENGTH 16
#define NO_COLOR        ERRORCODE

const   struct  ansi_data       ansi_info       []      =
{
  { "\e[30m"   },  /* 前景顏色黑色       */
  { "\e[31m"   },  /* 前景顏色紅色       */
  { "\e[32m"   },  /* 前景顏色綠色       */
  { "\e[33m"   },  /* 前景顏色黃色       */
  { "\e[34m"   },  /* 前景顏色藍色       */
  { "\e[35m"   },  /* 前景顏色紫色       */
  { "\e[36m"   },  /* 前景顏色天空       */
  { "\e[37m"   },  /* 前景顏色白色       */
  { "\e[1;31m" },  /* 前景高亮度顏色紅色 */
  { "\e[1;32m" },  /* 前景高亮度顏色綠色 */
  { "\e[1;33m" },  /* 前景高亮度顏色黃色 */
  { "\e[1;34m" },  /* 前景高亮度顏色藍色 */
  { "\e[1;35m" },  /* 前景高亮度顏色紫色 */
  { "\e[1;36m" },  /* 前景高亮度顏色天空 */
  { "\e[1;37m" },  /* 前景高亮度顏色白色 */
  { "\e[5;31m" },  /* 前景高亮度顏色紅色 */
  { "\e[5;32m" },  /* 前景高亮度顏色綠色 */
  { "\e[5;33m" },  /* 前景高亮度顏色黃色 */
  { "\e[5;34m" },  /* 前景高亮度顏色藍色 */
  { "\e[5;35m" },  /* 前景高亮度顏色紫色 */
  { "\e[5;36m" },  /* 前景高亮度顏色天空 */
  { "\e[5;37m" },  /* 前景高亮度顏色白色 */
  { "\e[31;1m" },  /* 前景高亮度顏色紅色 */
  { "\e[32;1m" },  /* 前景高亮度顏色綠色 */
  { "\e[33;1m" },  /* 前景高亮度顏色黃色 */
  { "\e[34;1m" },  /* 前景高亮度顏色藍色 */
  { "\e[35;1m" },  /* 前景高亮度顏色紫色 */
  { "\e[36;1m" },  /* 前景高亮度顏色天空 */
  { "\e[37;1m" },  /* 前景高亮度顏色白色 */
  { "\e[31;5m" },  /* 前景高亮度顏色紅色 */
  { "\e[32;5m" },  /* 前景高亮度顏色綠色 */
  { "\e[33;5m" },  /* 前景高亮度顏色黃色 */
  { "\e[34;5m" },  /* 前景高亮度顏色藍色 */
  { "\e[35;5m" },  /* 前景高亮度顏色紫色 */
  { "\e[36;5m" },  /* 前景高亮度顏色天空 */
  { "\e[37;5m" },  /* 前景高亮度顏色白色 */
  { "\e[5;1;31m" },  /* 前景高亮度顏色紅色 */
  { "\e[5;1;32m" },  /* 前景高亮度顏色綠色 */
  { "\e[5;1;33m" },  /* 前景高亮度顏色黃色 */
  { "\e[5;1;34m" },  /* 前景高亮度顏色藍色 */
  { "\e[5;1;35m" },  /* 前景高亮度顏色紫色 */
  { "\e[5;1;36m" },  /* 前景高亮度顏色天空 */
  { "\e[5;1;37m" },  /* 前景高亮度顏色白色 */
  { "\e[1;5;31m" },  /* 前景高亮度顏色紅色 */
  { "\e[1;5;32m" },  /* 前景高亮度顏色綠色 */
  { "\e[1;5;33m" },  /* 前景高亮度顏色黃色 */
  { "\e[1;5;34m" },  /* 前景高亮度顏色藍色 */
  { "\e[1;5;35m" },  /* 前景高亮度顏色紫色 */
  { "\e[1;5;36m" },  /* 前景高亮度顏色天空 */
  { "\e[1;5;37m" },  /* 前景高亮度顏色白色 */
  { "\e[41;1m"   },  /* 背景高亮度顏色紅色 */
  { "\e[42;1m"   },  /* 背景高亮度顏色綠色 */
  { "\e[43;1m"   },  /* 背景高亮度顏色黃色 */
  { "\e[44;1m"   },  /* 背景高亮度顏色藍色 */
  { "\e[45;1m"   },  /* 背景高亮度顏色紫色 */
  { "\e[46;1m"   },  /* 背景高亮度顏色天空 */
  { "\e[47;1m"   },  /* 背景高亮度顏色白色 */
  { "\e[40m"     },  /* 背景顏色黑色       */
  { "\e[41m"     },  /* 背景顏色紅色       */
  { "\e[42m"     },  /* 背景顏色綠色       */
  { "\e[43m"     },  /* 背景顏色黃色       */
  { "\e[44m"     },  /* 背景顏色藍色       */
  { "\e[45m"     },  /* 背景顏色紫色       */
  { "\e[46m"     },  /* 背景顏色天空       */
  { "\e[47m"     },  /* 背景顏色白色       */
  { "\e[0m"      },  /* 還原               */
  { "\e[1;33;44m"},  /* 前景亮黃背景深藍色 */
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

/* 轉換 >> 為 ESC 控制碼 */
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

/* 清除 << 系統內定的縮寫符號 */
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

/* 過濾一些影響別人輸出的 ANSI 控制碼 */
void filter_ansi( char * argument )
{
  PUSH_FUNCTION( "filter_ansi" );

  /* 轉換 >> 為 ESC 控制碼 */
  translate_ansi( argument );

  /* 清除 << 系統內定的縮寫符號 */
  purge_token( argument );

  for ( ; *argument; argument++ )
  {
    if ( *argument == '\e' && *( argument + 1 ) == '[' )
    {
      for ( argument += 2 * sizeof( char ) ;; argument++ )
        if ( *argument != ';' && !isdigit( ( int ) *argument ) ) break;

      switch( *argument )
      {
      case 'A': /* ANSI 游標上移           */
      case 'B': /* ANSI 游標下移           */
      case 'C': /* ANSI 游標右移           */
      case 'D': /* ANSI 游標左移           */
      case 'H': /* ANSI 游標定位           */
      case 'J': /* ANSI 清除螢幕           */
      case 'K': /* ANSI 清除游標以後的文字 */
      case 's': /* ANSI 游標保存起來       */
      case 'u': /* ANSI 游標回存           */
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

  /* 超出範圍 */
  if ( len > sizeof( buf ) || len <= 0 || str_len( pString ) > sizeof( buf ) )
  {
    mudlog( LOG_DEBUG, "format_string: 來源超出範圍或是不合理." );
    RETURN( "\x0" );
  }

  /* 空字串 */
  if ( !pString || !*pString )
  {
    for ( loop = 0; len > 0; loop++ ) buf[loop] = ' ';
    buf[loop] = '\x0';
    RETURN( buf );
  }

  /* 長度比限定的小或是等於 */
  if ( ( diff_len = len - ( real_len = ansi_strlen( pString ) ) ) >= 0 )
  {
    str_cpy( buf, pString );

    for ( target = buf + str_len( buf ); diff_len > 0; diff_len--, target++ )
      *target = ' ';

    *target = '\x0';
    str_cat( buf, "\e[0m" );
  }

  /* 長度比限定的大 */
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
      case 'A': /* ANSI 游標上移           */
      case 'B': /* ANSI 游標下移           */
      case 'C': /* ANSI 游標右移           */
      case 'D': /* ANSI 游標左移           */
      case 'H': /* ANSI 游標定位           */
      case 'J': /* ANSI 清除螢幕           */
      case 'K': /* ANSI 清除游標以後的文字 */
      case 's': /* ANSI 游標保存起來       */
      case 'u': /* ANSI 游標回存           */
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

/* 格式化定位輸出 */
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
    mudlog( LOG_DEBUG, "tablize: 選項錯誤 %d.", type );
    break;

  /* 清除原本設定 */
  case FORMAT_CLEAN:

    for ( loop = 0; loop < MAX_ELEMENT; loop++ )
    {
      die[loop].character = '\x0';
      die[loop].color     = NO_COLOR;
    }

    break;

  /* 設定 */
  case FORMAT_SET:

    /* 清除原本設定 */
    tablize( FORMAT_CLEAN, NULL, 0, NULL, 0 );

    for ( loop = 0; *pString; loop++ )
    {
      if ( loop >= MAX_ELEMENT - 1 )
      {
        mudlog( LOG_DEBUG, "tablize: 元素超過了預定值 %d.", MAX_ELEMENT );
        break;
      }

      pString = ansi_code( pString, &die[loop].character, &die[loop].color );
    }

    die[loop].character = '\x0';
    die[loop].color     = NO_COLOR;
    break;

  /* 傳回陣列 */
  case FORMAT_RETURN:

    if ( !pString )
    {
      mudlog( LOG_DEBUG, "tablize: 設定值為空的." );
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

  /* 定位設定字串 */
  case FORMAT_STRING:

    if ( !pString )
    {
      mudlog( LOG_DEBUG, "tablize: 來源字串是空的." );
      break;
    }

    if ( !key || !*key )
    {
      mudlog( LOG_DEBUG, "tablize: 比較字串是空的, 無法比較." );
      break;
    }

    for ( loop = 0; loop < MAX_ELEMENT && die[loop].character; loop++ )
    {
      for ( shift = 0; loop + shift < MAX_ELEMENT; shift++ )
      {
        /* 字串相同 */
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

    mudlog( LOG_DEBUG, "tablize: 找不到比較字串 %s.", key );
    break;

  /* 數字 */
  case FORMAT_NUM:

    sprintf( sNumber, "%d", iNum );
    tablize( FORMAT_STRING, sNumber, 0, key, -len );
    break;

  /* 定位設定字串 */
  case FORMAT_LOCATE_STRING:

    if ( !pString )
    {
      mudlog( LOG_DEBUG, "tablize: 來源字串是空的." );
      break;
    }

    if ( !key || !is_number( key ) )
    {
      mudlog( LOG_DEBUG, "tablize: 定位字串不正確." );
      break;
    }

    if ( ( shift = atoi( key ) ) < 0 || shift >= MAX_ELEMENT )
    {
      mudlog( LOG_DEBUG, "tablize: 定位字串不合法 %d.", shift );
      RETURN_NULL();
    }

    /* 檢查長度是否合理 */
    if ( len == 0 || shift + len - 1 >= MAX_ELEMENT )
    {
      mudlog( LOG_DEBUG, "tablize: 設定長度 %d 不合法.", len );
      RETURN_NULL();
    }

    /* 修正若是要向右對齊 */
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
    mudlog( LOG_DEBUG, "ansi_transcribe: 缺乏來源." );
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

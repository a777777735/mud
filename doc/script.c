/*
 * Nimud scripts.
 *      This script parser was begun on 5/7/94 by Herb Gilliland
 * (aka Locke of The Isles).  It was written to allow a more advanced mud
 * software to come into play.  The next_command() and multi_interpret()
 * routines were derived from the MOBprograms in Merc 2.2, and the work of
 * the buggy code of Natasha.  I found the whole MOBprograms to be bugged
 * to hell, and, since I myself didn't want to remain with the primitive
 * and crude script language that I developed, I wanted to make something
 * that was really useful.  I must thank the writers of MudOS (LpMUD) and
 * all MOOs, MUSEs, MUSHes and MUCKs, this really is based on the ideas
 * made by them.
 *                                      Enjoy.
 * See adjoining text for information on the syntax.
 */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "script.h"
#include "merc.h"

int get_dir( char * arg )
{
    int door = DIR_MAX;

       if ( !str_prefix( arg, "north" ) ) door = DIR_NORTH;
  else if ( !str_prefix( arg, "east"  ) ) door = DIR_EAST;
  else if ( !str_prefix( arg, "south" ) ) door = DIR_SOUTH;
  else if ( !str_prefix( arg, "west"  ) ) door = DIR_WEST;
  else if ( !str_prefix( arg, "up"    ) ) door = DIR_UP;
  else if ( !str_prefix( arg, "down"  ) ) door = DIR_DOWN;
  else if ( !str_prefix( arg, "enter" ) ) door = DIR_ENTER;

  return door;
}

/*
 * This little command helps a user debug the scripts online.  It can give
 * information about a single trigger, or general info on the mob's current
 * state of execution.
 */
void do_script( CHAR_DATA * ch , char * argument )
{
  char            buf[MAX_STRING_LENGTH];
  char            arg[MAX_INPUT_LENGTH];
  char            arg2[MAX_INPUT_LENGTH];
  CHAR_DATA     * victim;
  TRIGGER_DATA  * trig;
  VARIABLE_DATA * var;

  argument = one_argument( argument, arg );
  one_argument( argument, arg2 );

  if ( !arg[0] )
  {
    send_to_char( "Script whom (which trigger)?\n\r", ch );
    return;
  }

  if ( ( victim = get_char_world( ch, arg ) ) == NULL )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }

  if ( IS_NPC(victim) )
  {
    sprintf( buf, "Name: '%s'\n\rArea: [%3d] '%s'\n\r"
      , NAME(victim)
      , victim->pIndexData->area->vnum
      , victim->pIndexData->area->name );
    send_to_char( buf, ch );

    sprintf( buf, "Vnum: %d  Sex: %s  Room: %d\n\r"
      , victim->pIndexData->vnum
      , victim->sex == SEX_MALE    ? "male"   :
        victim->sex == SEX_FEMALE  ? "female" : "neutral"
      , victim->in_room == NULL    ?        0 : victim->in_room->vnum );
    send_to_char( buf, ch );
  }

  if ( arg2[0] )
  {
    for ( trig = victim->triggers; trig; trig = trig->next )
    {
      if ( !str_prefix( trig->name, arg2 ) ) break;
    }

    if ( trig == NULL )
    {
      send_to_char( "No such trigger.\n\r", ch );
    }
    else
    {
      SCRIPT_DATA * scr;
      char          buf[MAX_STRING_LENGTH];

      for ( scr = trig->script; scr != NULL; scr = scr->next )
      {
        if ( scr == trig->current )
        {
          sprintf( buf, "##\n\r%s   <--\n\r", scr->command );
        }

        else
        {
          sprintf( buf, "##\n\r%s\n\r",       scr->command );
        }

        send_to_char( buf, ch );
      }
    }
  }

  else
  {
    for ( trig = victim->triggers; trig; trig = trig->next )
    {
      char buf[MAX_STRING_LENGTH];

      if ( trig )
      {
        sprintf( buf, "Trigger '%s' (key '%s')\n\r'%s' "
                      "w/tra/typ: %d %d %d  %s %s\n\r"
          , trig->name
          , trig->keywords
          , trig->current ? trig->current->command :"not running"
          , trig->waiting
          , trig->tracer
          , trig->trigger_type
          , IS_SET(trig->bits, SCRIPT_ADVANCE) ? "" : "stopped"
          , IS_SET(trig->bits, SCRIPT_HALT)    ? "halted" : "" );
        send_to_char( buf, ch );
      }
    }

    if ( victim->variables ) send_to_char( "Variables:\n\r", ch );

    for ( var = victim->variables; var;  var = var->next )
    {
      char buf[MAX_STRING_LENGTH];

      if ( var )
      {
        sprintf( buf, "'%s' = '%s'\n\r", var->name, var->value );
        send_to_char( buf, ch );
      }
    }

    if ( IS_NPC( victim ) )
    {
      sprintf( buf, "Current spec tracer set to %d, loads with file '%s'\n\r"
        , victim->spec_tracer, victim->pIndexData->script_fn );
      send_to_char( buf, ch );
    }
  }

  send_to_char( "\n\r", ch );
  return;
}

/*
 * This is a quick and dirty utility for adding a few fields to a trigger
 * quickly.  When a script is triggered, information is sent into variables
 * using this procedure.  The arg, from, to are preceded by the command
 * string, thus:
 * act_trigger( bill, "Argyle", "Red", NULL, NULL );
 * would put the following variables on the mob bill:
 * %Argyle-arg%      = Red
 */
void act_trigger( CHAR_DATA * mob, char * command, char * argument,
                  char * from, char * to, char * obj)
{
  char arg[MAX_STRING_LENGTH];
  char buf[MAX_STRING_LENGTH];

  if ( mob->deleted || !HAS_SCRIPT(mob) || !command ) return;

  if ( from )
  {
    one_argument( from , arg );
    sprintf( buf, "%%%s-from%%", command );
    assign_variable( buf, arg, mob );
  }

  if ( to )
  {
    one_argument( to , arg );
    sprintf( buf, "%%%s-to%%", command );
    assign_variable( buf, arg, mob );
  }

  if ( obj )
  {
    one_argument( obj , arg );
    sprintf( buf, "%%%s-obj%%", command );
    assign_variable( buf, arg, mob );
  }

  if ( argument )
  {
    sprintf( buf, "%%%s-arg%%", command );
    assign_variable( buf, argument, mob );
  }

  return;
}

/*
 * This searches a list of variables for a variable with a certain name,
 * returning NULL if the variable happens to not currently exist.
 */
VARIABLE_DATA * get_variable( char * var_name, CHAR_DATA * mob )
{
  VARIABLE_DATA * var;

  for ( var = mob->variables; var; var = var->next )
    if ( !str_cmp( var_name, var->name ) ) return var;

  return NULL;
}

/*
 * Unlike the above, this takes a variable name and returns a value string
 * no matter if the variable exists or not.  This is for func evaluations
 * and if-thens mostly.
 */
char * varval( char * var_name, CHAR_DATA * mob )
{
  VARIABLE_DATA * var;

  var = get_variable( var_name, mob );
  if ( !var ) return str_dup( "" );

  return var->value;
}

/*
 * Assigns or creates a variable with a value.   Translates interior variables.
 */
void assign_variable( char * var_name, char * value, CHAR_DATA * mob )
{
  VARIABLE_DATA * var;

  if ( ( var = get_variable( var_name, mob ) ) )
  {
    free_string( var->value );
    var->value = translate_variables( value, mob );
  }
  else
  {
    if ( variable_free == NULL )
    {
      var = alloc_perm( sizeof( *var ) );
    }

    else
    {
      var           = variable_free;
      variable_free = variable_free->next;
    }

    var->next      = mob->variables;
    mob->variables = var;
    var->name      = str_dup( var_name );
    var->value     = translate_variables( value, mob );
  }
  return;
}

/*
 * Pick off one argument from a string and return the rest.
 * Understands quotes, parenthesis (barring ) ('s) and percentages.
 */
char * first_arg( char * argument, char * arg_first, bool fCase )
{
  char cEnd;

  while ( *argument == ' ' ) argument++;

  cEnd = ' ';

  if ( *argument == '\'' || *argument == '"'
    || *argument == '%'  || *argument == '(' )
  {
    if ( *argument == '(' )
    {
      cEnd = ')';
      argument++;
    }

    else
    {
      cEnd = *argument++;
    }
  }

  while ( *argument != '\x0' )
  {
    if ( *argument == cEnd )
    {
      argument++;
      break;
    }

    if ( fCase ) *arg_first = LOWER(*argument);
    else         *arg_first = *argument;

    arg_first++;
    argument++;
  }

  *arg_first = '\x0';
  while ( *argument == ' ' ) argument++;
  return argument;
}

/*
 * Translates all variables in a string, note the usage of replace_string()..
 * which is as good as any.
 */
char * translate_variables( char * argument, CHAR_DATA * mob )
{
  VARIABLE_DATA * var;
  char          * newstr = str_dup( argument );

  for ( var = mob->variables; var; var = var->next )
  {
    while ( strstr(newstr, var->name) )
      newstr = replace_string( newstr, var->name, var->value );
  }

  return newstr;
}

/*
 * Used to get sequential lines of a multi line string (separated by "\n\r")
 * Thus its like one_argument(), but a trifle different. It is destructive
 * to the multi line string argument, and thus clist must not be shared.
 */
char * next_command( char * argument,  char * arg_first )
{
  while ( *argument )
  {
    if ( *argument == '\n' )
    {
      argument++;
      break;
    }

    *arg_first = *argument;
    arg_first++;
    argument++;
  }

  *arg_first = '\x0';
  return argument;
}

/*
 * Evaluates multiple line command parses and does them all in one pulse.
 * Kinda kludgy, but useful.
 */
void multi_interpret ( CHAR_DATA * mob, char * com_list )
{
  char buf[MAX_STRING_LENGTH];

  while ( com_list[0] )
  {
    com_list = next_command( com_list, buf );
    interpret( mob, buf );
  }
  return;
}

/*
 * Takes a plain and simple 1 + 2 + 4 * 2 and does a infix left to right
 * evaluation, returning the final numeric.
 */
int expression( char *ex, CHAR_DATA *ch )
{
  char buf[MAX_STRING_LENGTH];
  char op[MAX_STRING_LENGTH];
  int  num;
  int  val;

  if ( !ex ) return 0;

  ex = first_arg( ex, buf, FALSE );
  if ( buf[0] == '%' ) strcpy( buf, varval( buf, ch ) );
  num = atoi( buf );

  while ( ex[0] )
  {
    ex = first_arg( ex, op, TRUE );
    ex = first_arg( ex, buf, TRUE );
    if ( buf[0] == '%' ) strcpy( buf, varval( buf, ch ) );

    val = atoi( buf );
    switch( op[0] )
    {
    case '+': num = num + val; break;
    case '-': num = num - val; break;
    case '/': num = num / val; break;
    case '*': num = num * val; break;
    case '&': num = num & val; break;
    case '|': num = num | val; break;
    case '^': num = num ^ val; break;
    default :                  break;
    }
  }

  return num;
}

/*
 * This procedure is the 'let' routine. It assigns variables on a mobile,
 * allowing the translation of expressions with variables in them.
 */
void eval_assign( CHAR_DATA *ch, TRIGGER_DATA * trigger, char * argument )
{
  char var[MAX_STRING_LENGTH];
  char op[MAX_STRING_LENGTH];
  char val[MAX_STRING_LENGTH];
  char num[MAX_STRING_LENGTH];
  char nval[MAX_STRING_LENGTH];

  argument = one_argument( argument, var );
  argument = first_arg( argument, op, TRUE );
  one_argument( argument, val );
  strcpy( val, argument );

  if ( !str_cmp( op, "==" ) )
  {
    assign_variable( var, val, ch );
  }

  else if ( !str_cmp( op, "="  ) )
  {
    sprintf( num, "%d", expression( val, ch )  );
    assign_variable( var, num, ch );
  }

  else if ( !str_cmp( op, "+=" ) )
  {
    int iVal;
    int iOld;

    iVal = atoi( val );
    iOld = get_variable( var, ch ) == NULL ? 0
           : atoi( get_variable( var, ch )->value );
    sprintf( val, "%d", iVal + iOld );
    assign_variable( var, val, ch );
  }

  else if ( !str_cmp( op, "-=" ) )
  {
    int iVal;
    int iOld;

    iVal = atoi( val );
    iOld = get_variable( var, ch ) == NULL ? 0
           : atoi( get_variable( var, ch )->value );
    sprintf( val, "%d", iOld - iVal );
    assign_variable( var, val, ch );
  }

  else if ( !str_cmp( op, "*=" ) )
  {
    int iVal;
    int iOld;

    iVal = atoi( val );
    iOld = get_variable( var, ch ) == NULL ? 0
      : atoi( get_variable( var, ch )->value );
    sprintf( val, "%d", iOld * iVal );
    assign_variable( var, val, ch );
  }

  else if ( !str_cmp( op, "/=" ) )
  {
    int iVal;
    int iOld;

    iVal = atoi( val );
    iOld = get_variable( var, ch ) == NULL ? 0
           : atoi( get_variable( var, ch )->value );
    sprintf( val, "%d", iOld / iVal );
    assign_variable( var, val, ch );
  }

  else if ( !str_cmp( op, "|=" ) )
  {
    int iVal;
    int iOld;

    iVal = atoi( val );
    iOld = get_variable( var, ch ) == NULL ? 0
           : atoi( get_variable( var, ch )->value );
    sprintf( val, "%d", iVal | iOld );
    assign_variable( var, val, ch );
  }

  else if ( !str_cmp( op, "&=" ) )
  {
    int iVal;
    int iOld;

    iVal = atoi( val );
    iOld = get_variable( var, ch ) == NULL ? 0
           : atoi( get_variable( var, ch )->value );
    sprintf( val, "%d", iVal & iOld );
    assign_variable( var, val, ch );
  }

  else if ( !str_cmp( op, "^=" ) )
  {
    int iVal;
    int iOld;

    iVal = atoi( val );
    iOld = get_variable( var, ch ) == NULL ? 0
           : atoi( get_variable( var, ch )->value );
    sprintf( val, "%d", iVal ^ iOld );
    assign_variable( var, val, ch );
  }

  else if ( !str_cmp( op, "++" ) )
  {
    sprintf( nval, "%d", atoi( get_variable( var, ch )->value ) + 1 );
    assign_variable( var, nval, ch );
  }

  else if ( !str_cmp( op, "--" ) )
  {
    sprintf( nval, "%d", atoi( get_variable( var, ch )->value ) - 1 );
    assign_variable( var, nval, ch );
  }

  return;
}

/*
 * This procedure is the 'let' routine. It assigns variables on a mobile,
 * allowing the translation of expressions with variables in them.
 */
void eval_massign( CHAR_DATA *ch, TRIGGER_DATA * trigger, char * argument )
{
  char        var[MAX_STRING_LENGTH];
  char      * cmd;
  CHAR_DATA * mob;

  argument = one_argument( argument, var );
  cmd = translate_variables(argument,ch );
  mob = get_char_world( ch , cmd);
  free_string(cmd);

  if ( !mob || IS_NPC(mob) ) return;
  eval_assign( mob , trigger, argument );
  return;
}

/*
 * the 'int' command -> does an interpret at the script level
 */
void eval_interp( CHAR_DATA * ch, TRIGGER_DATA * trigger, char * argument )
{
  char * cmd;

  cmd = translate_variables( argument, ch );
  FORCE_LEVEL = MAX_LEVEL;   /* change as desired */
  interpret( ch, cmd );
  free_string( cmd );
  FORCE_LEVEL = 1;
  return;
}

/*
 * This procedure does a few different 'halt' functions:
 *
 * 1)   triggers  -> will halt the single trigger until mob dies
 * 2)   all       -> halts the whole mob
 * 3)   statement -> aborts a single 'Sc' statement from continuing
 * 4)   once      -> abort it this time
 */
bool eval_halt( CHAR_DATA * ch, TRIGGER_DATA * trigger, char * argument )
{
  char arg[MAX_STRING_LENGTH];

  one_argument( argument, arg );

  if ( !str_cmp( arg, "once" ) )
  {
    trigger->current = NULL;
    return FALSE;
  }

  else if ( !str_cmp( arg, "trigger" ) )
  {
    trigger->current = NULL;
    SET_BIT( trigger->bits, SCRIPT_HALT );
    return FALSE;
  }

  else if ( !str_cmp( arg, "all" ) )
  {
    TRIGGER_DATA * Trig;

    SET_BIT( ch->act, ACT_HALT );
    for ( Trig = ch->triggers; Trig; Trig = Trig->next ) Trig->current = NULL;
    return FALSE;
  }

  else if ( !str_cmp( arg, "statement" ) )
  {
    return FALSE;
  }

  return TRUE;
}

/*
 *  This procedure sets the pulse-wait state on mobs ( num = 2*arg1 )
 */
void eval_wait( CHAR_DATA * ch, TRIGGER_DATA * trigger, char * argument )
{
  char arg[MAX_STRING_LENGTH];
  int  waittime;

  one_argument( argument, arg );

  if ( !arg[0] ) return;

  if ( is_number( arg ) )
  {
    trigger->waiting = atoi( arg );
    return;
  }

  else if ( !str_cmp( arg, "time" ) )
  {
    argument = one_argument( argument, arg );
    if ( !is_number( arg ) ) return;
    waittime = atoi( arg );
    if ( waittime != time_info.hour ) trigger->tracer--;
  }

  return;
}


/*
 *  This goto evaluates things one of two ways:
 *  1)  Will go to a specific script entry (counting 0 as first)
 *  2)  Go to a label ( define labels as the first part of a script ': label')
 */
void eval_goto( CHAR_DATA *ch, TRIGGER_DATA *trigger, char* argument )
{
  char          arg[MAX_STRING_LENGTH];
  SCRIPT_DATA * scr = trigger->script;

  one_argument( argument, arg );

  if ( is_number( arg ) )
  {
    int count = 0;
    int jump_loc;

    jump_loc = atoi( arg );

    while ( count < jump_loc && scr )
    {
      count++;
      scr = scr->next;
    }
  }

  else
  {
    char   arg2[MAX_STRING_LENGTH];
    char   arg3[MAX_STRING_LENGTH];
    char * cmd;

    while ( scr )
    {
      cmd = next_command( scr->command, arg3 );
      if ( arg3[0] == ':' )
      {
        one_argument( arg3, arg2 );
        if ( !str_cmp( arg2, arg ) ) break;
      }
      scr = scr->next;
    }
  }

  if ( scr == NULL ) scr = trigger->script;
  trigger->current = scr;
  return;
}

/*
 * Assigns a single value on a mobile:
 * usage:
 * msign <mobile/variable of mobile's name>  <value/variable w/ value> <what>
 */
void eval_msign( CHAR_DATA * ch, TRIGGER_DATA * trigger, char * argument )
{
  char arg1[MAX_STRING_LENGTH];
  char arg2[MAX_STRING_LENGTH];
  char arg3[MAX_STRING_LENGTH];
  CHAR_DATA *victim;
  int val;

  argument = one_argument( argument, arg1 );    /* mob to assign */
  argument = one_argument( argument, arg2 );    /* value to assign */
  argument = one_argument( argument, arg3 );    /* deliminator */

  if ( arg1[0] == '%' ) strcpy( arg1, varval( arg1, ch ) );
  if ( arg2[0] == '%' ) strcpy( arg2, varval( arg2, ch ) );
  victim = get_char_world( ch, arg1 );
  val    = atoi( arg2 );

  if ( victim == NULL || victim->deleted  )
  {
    eval_halt( ch, trigger, "once" );
    return;
  }

  else if ( !str_cmp(arg3, "position") )
  {
    victim->position = val;
  }

  else if ( !str_cmp(arg3, "gold" ) || !str_cmp( arg3, "money" ) )
  {
    if ( val > 0 ) create_amount( val, victim, NULL, NULL );
    else if ( val < 0 ) sub_coins( abs(val), victim );
  }

  else if ( !str_cmp(arg3, "hunting" ) )
  {
    victim->hunting = str_dup( arg2 );
  }

  else if ( !str_cmp(arg3, "spectracer" ) )
  {
    victim->spec_tracer = val;
  }

  else if ( !str_cmp(arg3, "exp" ) )
  {
    victim->exp = val;
  }

  else if ( !str_cmp(arg3, "act" ) )
  {
    victim->act = val;
  }

  else if ( !str_cmp(arg3, "affected_by" ) )
  {
    victim->affected_by = val;
  }

  else if ( !str_cmp(arg3, "practice" ) )
  {
    victim->practice = val;
  }

  else if ( !str_cmp(arg3, "sex" ) )
  {
    victim->sex = val;
  }

  else if ( !str_cmp(arg3, "race" ) )
  {
    victim->race = val;
  }

  else if ( !str_cmp(arg3, "class" ) )
  {
    victim->class = val;
  }

  else if ( !str_cmp(arg3, "level" ) )
  {
    victim->level = val;
  }

  else if ( !str_cmp(arg3, "hit" ) )
  {
    victim->hit = val;
  }

  else if ( !str_cmp(arg3, "maxhit" ) )
  {
    victim->max_hit = val;
  }

  else if ( !str_cmp(arg3, "move" ) )
  {
    victim->move = val;
  }

  else if ( !str_cmp(arg3, "maxmana" ) )
  {
    victim->max_mana = val;
  }

  else if ( !str_cmp(arg3, "mana" ) )
  {
    victim->mana = val;
  }

  else if ( !str_cmp(arg3, "maxmove" ) )
  {
    victim->max_move = val;
  }

  else if ( !str_cmp(arg3, "align" ) )
  {
    victim->alignment = val;
  }

  else if ( !str_cmp(arg3, "str") && !IS_NPC(ch))
  {
    victim->pcdata->perm_str = val;
  }

  else if ( !str_cmp(arg3, "int") && !IS_NPC(ch))
  {
    victim->pcdata->perm_int = val;
  }

  else if ( !str_cmp(arg3, "dex") && !IS_NPC(ch))
  {
    victim->pcdata->perm_dex = val;
  }

  else if ( !str_cmp(arg3, "wis") && !IS_NPC(ch))
  {
    victim->pcdata->perm_wis = val;
  }

  else if ( !str_cmp(arg3, "con") && !IS_NPC(ch))
  {
    victim->pcdata->perm_con = val;
  }

  return;
}

void eval_osign( CHAR_DATA * ch, TRIGGER_DATA * trigger, char * argument )
{
  char       arg1[MAX_STRING_LENGTH];
  char       arg2[MAX_STRING_LENGTH];
  char       arg3[MAX_STRING_LENGTH];
  OBJ_DATA * obj;
  int        val;

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );
  argument = one_argument( argument, arg3 );

  if ( arg1[0] == '%' ) strcpy( arg1, varval( arg1, ch ) );
  if ( arg2[0] == '%' ) strcpy( arg2, varval( arg2, ch ) );
  obj = get_obj_world( ch, arg1 );
  val = atoi( arg2 );

  if ( obj == NULL || obj->deleted  )
  {
    eval_halt( ch, trigger, "once" );
    return;
  }

  else if ( !str_cmp(arg3, "cost" ) )
  {
    obj->cost = val;
  }

  else if ( !str_cmp(arg3, "wear_flags" ) )
  {
    obj->wear_flags = val;
  }

  else if ( !str_cmp(arg3, "wear_loc" ) )
  {
    obj->wear_loc = val;
  }

  else if ( !str_cmp(arg3, "weight" ) )
  {
    obj->weight = val;
  }

  else if ( !str_cmp(arg3, "item_type" ) )
  {
    obj->item_type = val;
  }

  else if ( !str_cmp(arg3, "level" ) )
  {
    obj->level = val;
  }

  else if ( !str_cmp(arg3, "timer" ) )
  {
    obj->timer = val;
  }

  else if ( !str_cmp(arg3, "extra_flags" ) )
  {
    obj->extra_flags = val;
  }

  else if ( is_number( arg3 ) )
  {
    obj->value[URANGE(0, atoi(arg3), 9)] = val;
  }

  return;
}

void eval_wsign( CHAR_DATA * ch, TRIGGER_DATA * trigger, char * argument )
{
  char arg2[MAX_STRING_LENGTH];
  char arg3[MAX_STRING_LENGTH];
  int  val;

  argument = one_argument( argument, arg2 );
  argument = one_argument( argument, arg3 );

  if ( arg2[0] == '%' ) strcpy( arg2, varval( arg2, ch ) );
  val = atoi( arg2 );

       if ( !str_cmp( arg3, "hour"     ) ) time_info.hour        = val;
  else if ( !str_cmp( arg3, "day"      ) ) time_info.day         = val;
  else if ( !str_cmp( arg3, "month"    ) ) time_info.month       = val;
  else if ( !str_cmp( arg3, "year"     ) ) time_info.year        = val;
  else if ( !str_cmp( arg3, "sunlight" ) ) weather_info.sunlight = val;
  else if ( !str_cmp( arg3, "mmhg"     ) ) weather_info.mmhg     = val;
  else if ( !str_cmp( arg3, "weather"  ) ) weather_info.sky      = val;

  return;
}

/*
 * Grabs world stuff/random numbers.
 * Usage:
 * wfunc %variable-to-go-in% <what> [<min random> <max random>]
 */
void eval_wfunc( CHAR_DATA * ch, TRIGGER_DATA * trigger, char * argument )
{
  char arg1[MAX_STRING_LENGTH];
  char arg3[MAX_STRING_LENGTH];
  char buf [MAX_STRING_LENGTH];

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg3 );

  if ( !str_cmp( arg3, "hour" ) )
    sprintf( buf, "%d", time_info.hour );

  else if ( !str_cmp( arg3, "day" ) )
    sprintf( buf, "%d", time_info.day );

  else if ( !str_cmp( arg3, "month" ) )
    sprintf( buf, "%d", time_info.month );

  else if ( !str_cmp( arg3, "year" ) )
    sprintf( buf, "%d", time_info.year );

  else if ( !str_cmp( arg3, "sunlight" ) )
    sprintf( buf, "%d", weather_info.sunlight );

  else if ( !str_cmp( arg3, "mmhg" ) )
    sprintf( buf, "%d", weather_info.mmhg );

  else if ( !str_cmp( arg3, "weather" ) )
    sprintf( buf, "%d", weather_info.sky );

  else if ( !str_cmp( arg3, "topmob" ) )
    sprintf( buf, "%d", top_vnum_mob );

  else if ( !str_cmp( arg3, "topobj" ) )
    sprintf( buf, "%d", top_vnum_obj );

  else if ( !str_cmp( arg3, "toproom" ) )
    sprintf( buf, "%d", top_vnum_room );

  else if ( !str_cmp( arg3, "random" ) )
  {
    char arg4[MAX_STRING_LENGTH];
    char arg5[MAX_STRING_LENGTH];

    argument = one_argument( argument, arg4 );
    if ( arg4[0] == '%' ) strcpy( arg4, varval( arg4, ch ) );

    argument = one_argument( argument, arg5 );
    if ( arg5[0] == '%' ) strcpy( arg5, varval( arg5, ch ) );

    sprintf( buf, "%d", number_range(atoi( arg4 ), atoi( arg5 )) );
  }

  else
  {
    int door;

    door = get_dir( arg3 );
    sprintf( buf, "%d", ch->in_room->exit[door] != NULL
      && ch->in_room->exit[door]->to_room != NULL ?
      ch->in_room->exit[door]->to_room->vnum :
      ROOM_VNUM_TEMPLATE );
  }

  assign_variable( arg1,  buf, ch );
  return;
}

void eval_mfunc( CHAR_DATA * ch, TRIGGER_DATA * trigger, char * argument )
{
  char        arg1[MAX_STRING_LENGTH];
  char        arg2[MAX_STRING_LENGTH];
  char        arg3[MAX_STRING_LENGTH];
  char        buf [MAX_STRING_LENGTH];
  CHAR_DATA * victim;

  argument = one_argument( argument, arg1 );   /* mob name */
  argument = one_argument( argument, arg2 );   /* variable to put val in */
  argument = one_argument( argument, arg3 );   /* deliminator */

  if ( arg1[0] == '%' ) strcpy( arg1, varval( arg1, ch ) );

  victim = !str_cmp( arg1, "self" ) ? ch : get_char_world( ch, arg1 );

  if ( victim == NULL || victim->deleted  )
    sprintf( buf, "NOONE" );

  else if ( !str_cmp(arg3, "vnum") )
    sprintf( buf, "%d", IS_NPC( victim ) ? victim->pIndexData->vnum : 0 );

  else if ( !str_cmp(arg3, "position") )
    sprintf( buf, "%d", victim->position );

  else if ( !str_cmp(arg3, "gold" ) )
    sprintf( buf, "%d", tally_coins( victim ) );

  else if ( !str_cmp(arg3, "name" ) )
    sprintf( buf, "%s", NAME(victim) );

  else if ( !str_cmp(arg3, "hunting" ) )
    sprintf( buf, "%s", victim->hunting );

  else if ( !str_cmp(arg3, "room" ) )
    sprintf( buf, "%d", victim->in_room->vnum );

  else if ( !str_cmp(arg3, "spectracer" ) )
    sprintf( buf, "%d", victim->spec_tracer );

  else if ( !str_cmp(arg3, "exp" ) )
    sprintf( buf, "%d", victim->exp );

  else if ( !str_cmp(arg3, "act" ) )
    sprintf( buf, "%d", victim->act );

  else if ( !str_cmp(arg3, "affected_by" ) )
    sprintf( buf, "%d", victim->affected_by );

  else if ( !str_cmp(arg3, "practice" ) )
    sprintf( buf, "%d", victim->practice );

  else if ( !str_cmp(arg3, "hitroll" ) )
    sprintf( buf, "%d", GET_HITROLL(victim) );

  else if ( !str_cmp(arg3, "damroll" ) )
    sprintf( buf, "%d", GET_DAMROLL(victim) );

  else if ( !str_cmp(arg3, "age" ) )
    sprintf( buf, "%d", get_age(victim) );

  else if ( !str_cmp(arg3, "sex" ) )
    sprintf( buf, "%d", victim->sex );

  else if ( !str_cmp(arg3, "race" ) )
    sprintf( buf, "%d", victim->race );

  else if ( !str_cmp(arg3, "class" ) )
    sprintf( buf, "%d", victim->class );

  else if ( !str_cmp(arg3, "level" ) )
    sprintf( buf, "%d", victim->level );

  else if ( !str_cmp(arg3, "hit" ) )
    sprintf( buf, "%d", victim->hit );

  else if ( !str_cmp(arg3, "maxhit" ) )
    sprintf( buf, "%d", victim->max_hit );

  else if ( !str_cmp(arg3, "mana" ) )
    sprintf( buf, "%d", victim->mana );

  else if ( !str_cmp(arg3, "maxmana" ) )
    sprintf( buf, "%d", victim->max_mana );

  else if ( !str_cmp(arg3, "move" ) )
    sprintf( buf, "%d", victim->move );

  else if ( !str_cmp(arg3, "maxmove" ) )
    sprintf( buf, "%d", victim->max_move );

  else if ( !str_cmp(arg3, "align" ) )
    sprintf( buf, "%d", victim->alignment );

  else if ( !str_cmp(arg3, "str" ) )
    sprintf( buf, "%d", get_curr_str( victim ) );

  else if ( !str_cmp(arg3, "int" ) )
    sprintf( buf, "%d", get_curr_int( victim ) );

  else if ( !str_cmp(arg3, "dex" ) )
    sprintf( buf, "%d", get_curr_dex( victim ) );

  else if ( !str_cmp(arg3, "wis" ) )
    sprintf( buf, "%d", get_curr_wis( victim ) );

  else if ( !str_cmp(arg3, "con" ) )
    sprintf( buf, "%d", get_curr_con( victim ) );

  else return;

  assign_variable( arg2,  buf, ch );
  return;
}

void eval_ofunc( CHAR_DATA * ch, TRIGGER_DATA * trigger, char * argument )
{
  char       arg1[MAX_STRING_LENGTH];
  char       arg2[MAX_STRING_LENGTH];
  char       arg3[MAX_STRING_LENGTH];
  char       buf [MAX_STRING_LENGTH];
  OBJ_DATA * obj;

  argument = one_argument( argument, arg1 );     /* object name */
  argument = one_argument( argument, arg2 );     /* variable to put val in */
  argument = one_argument( argument, arg3 );     /* deliminator */

  if ( arg1[0] == '%' ) strcpy( arg1, varval( arg1, ch ) );

  obj = get_obj_world( ch, arg1 );

  if ( obj == NULL || obj->deleted ) sprintf( buf, "NOTHING" );
  else if ( !str_cmp(arg3, "vnum" ) )
    sprintf( buf, "%d", obj->pIndexData->vnum );

  else if ( !str_cmp(arg3, "cost" ) )
    sprintf( buf, "%d", obj->cost  );

  else if ( !str_cmp(arg3, "name" ) )
    sprintf( buf, "%s", obj->short_descr );

  else if ( !str_cmp(arg3, "action" ) )
    sprintf( buf, "%s", STR(obj, action_descr) );

  else if ( !str_cmp(arg3, "room" ) )
    sprintf( buf, "%d", obj->in_room != NULL ? obj->in_room->vnum : 0 );

  else if ( !str_cmp(arg3, "inobj" ) )
    sprintf( buf, "%s", obj->in_obj ? STR(obj->in_obj, name) : "none" );

  else if ( !str_cmp(arg3, "carried_by" ) )
    sprintf( buf, "%s", obj->carried_by ? obj->carried_by->name : "none" );

  else if ( !str_cmp(arg3, "wear_flags" ) )
    sprintf( buf, "%d", obj->wear_flags );

  else if ( !str_cmp(arg3, "wear_loc" ) )
    sprintf( buf, "%d", obj->wear_loc );

  else if ( !str_cmp(arg3, "weight" ) )
    sprintf( buf, "%d", obj->weight );

  else if ( !str_cmp(arg3, "item_type" ) )
    sprintf( buf, "%d", obj->item_type );

  else if ( !str_cmp(arg3, "level" ) )
    sprintf( buf, "%d", obj->level );

  else if ( !str_cmp(arg3, "timer" ) )
    sprintf( buf, "%d", obj->timer );

  else if ( !str_cmp(arg3, "extra_flags" ) )
    sprintf( buf, "%d", obj->extra_flags );

  else if ( is_number( arg3 ) )
    sprintf( buf, "%d", obj->value[URANGE(0, atoi(arg3), 9)] );

  else return;

  assign_variable( arg2,  buf, ch );
  return;
}

void eval_sfunc( CHAR_DATA *ch, TRIGGER_DATA *trigger, char *argument )
{
  char   arg1[MAX_STRING_LENGTH];
  char   arg2[MAX_STRING_LENGTH];
  char   arg3[MAX_STRING_LENGTH];
  char   buf[MAX_STRING_LENGTH];
  char * st;
  int    count;

  argument = one_argument( argument, arg1 );    /* string */
  argument = one_argument( argument, arg2 );    /* variable to put it in */
  argument = one_argument( argument, arg3 );    /* arg deliminator */
  if ( arg1[0] == '%' ) sprintf( arg1, "%s", varval( arg1, ch ) );

  if ( !str_cmp(arg3, "money") )
  {
    sprintf( buf, "%s", name_amount( atoi( arg1 ) ) );
  }

  else if ( is_number(arg3) )
  {
    st = one_argument( arg1, buf );
    for ( count = atoi( arg3 ); count > 0; count-- )
    {
      st = one_argument( st, buf );
    }
  }

  assign_variable( arg2,  buf, ch );
  return;
}

bool boolchk( char *v1, char *v2, char *s )
{
  if ( !str_cmp( s, "in"     ) ) return ( str_infix(v1, v2)    );
  if ( !str_cmp( s, "pre"    ) ) return ( str_prefix(v1, v2)   );
  if ( !str_cmp( s, "cmp"    ) ) return ( !str_cmp(v1, v2)     );
  if ( !str_cmp( s, "substr" ) ) return ( strstr(v2, v1)!=NULL );
  if ( !str_cmp( s, "num"    ) ) return ( is_number(v1)        );
  if ( !str_cmp( s, ">"      ) ) return ( atoi(v1) >  atoi(v2) );
  if ( !str_cmp( s, "<"      ) ) return ( atoi(v1) <  atoi(v2) );
  if ( !str_cmp( s, ">="     ) ) return ( atoi(v1) >= atoi(v2) );
  if ( !str_cmp( s, "<="     ) ) return ( atoi(v1) <= atoi(v2) );
  if ( !str_cmp( s, "=="     ) ) return ( atoi(v1) == atoi(v2) );
  if ( !str_cmp( s, "!="     ) ) return ( atoi(v1) != atoi(v2) );
  if ( !str_cmp( s, "&"      ) ) return ( atoi(v1) &  atoi(v2) );
  if ( !str_cmp( s, "|"      ) ) return ( atoi(v1) |  atoi(v2) );
  if ( !str_cmp( s, "isset"  ) ) return ( IS_SET(atoi(v1), atoi(v2)));
  if ( s[0] == '\x0' )           return ( atoi(v1));
  return FALSE;
}

#define C_AND     1
#define C_OR      2
#define C_NOT     4

bool eval_if( CHAR_DATA * ch, TRIGGER_DATA * trigger, char * argument )
{
  char   buf[MAX_STRING_LENGTH];
  char * arg;
  char   v1[MAX_STRING_LENGTH];
  char   v2[MAX_STRING_LENGTH];
  char   s[MAX_STRING_LENGTH];
  bool   ifstatement = TRUE;
  bool   prev = TRUE;
  int    andornot;

  while ( argument[0] )
  {
    argument = first_arg( argument, buf, TRUE );
    arg = buf;

    while ( arg[0] )          /* do a single ( ) */
    {
      andornot = 0;
      arg = one_argument( arg, v1 );

      if ( !str_cmp( v1, "not" ) || !str_cmp( v1, "!" ) )
      {
        arg = one_argument( arg, v1 );
        SET_BIT(andornot, C_NOT);
        prev = ifstatement;
      }

      else if ( !str_cmp( v1, "or" ) || !str_cmp( v1, "||" ) )
      {
        arg = one_argument( arg, v1 );
        SET_BIT(andornot, C_OR);
        prev = ifstatement;
      }

      else if ( !str_cmp( v1, "and" ) || !str_cmp( v1, "&&" ) )
      {
        arg = one_argument( arg, v1 );
        SET_BIT(andornot, C_AND);
        prev = ifstatement;
      }

      else
      {
        arg = one_argument( arg, s  );
        arg = one_argument( arg, v2 );

        if ( v1[0] == '%' )  strcpy(v1, varval( v1, ch ));
        if ( v2[0] == '%' )  strcpy(v2, varval( v2, ch ));
        if (  s[0] == '%' )  strcpy( s, varval( s,  ch ));

        if ( IS_SET(ch->act, ACT_TRACKSCR) )
        {
          sprintf( log_buf, "NOTIFY Script> boolchk: v1'%s' "
                            "v2'%s' s'%s'", v1, v2, s );
          NOTIFY(log_buf, LEVEL_IMMORTAL, WIZ_NOTIFY_SCRIPT);
        }

        ifstatement = boolchk( v1, v2, s );
      }

      if ( IS_SET(andornot, C_NOT) ) ifstatement = !ifstatement;
      if ( IS_SET(andornot, C_AND) ) ifstatement = ifstatement && prev;
      if ( IS_SET(andornot, C_OR)  ) ifstatement = ifstatement || prev;
    }
  }
  return ifstatement;
}

bool eval_while( CHAR_DATA *ch, TRIGGER_DATA *trigger, char *argument )
{
  char   buf[MAX_STRING_LENGTH];
  char * arg;
  char   v1[MAX_STRING_LENGTH];
  char   v2[MAX_STRING_LENGTH];
  char   s[MAX_STRING_LENGTH];
  bool   ifstatement = TRUE;
  bool   prev = TRUE;
  int    andornot;

  while ( argument[0] )
  {
    argument = first_arg( argument, buf, TRUE );
    arg = buf;

    while ( arg[0] )          /* do a single ( ) */
    {
      andornot = 0;
      arg = one_argument( arg, v1 );

      if ( !str_cmp( v1, "not" ) || !str_cmp( v1, "!" ) )
      {
        arg = one_argument( arg, v1 );
        SET_BIT(andornot, C_NOT);
        prev = ifstatement;
      }

      else if ( !str_cmp( v1, "or" ) || !str_cmp( v1, "||" ) )
      {
        arg = one_argument( arg, v1 );
        SET_BIT(andornot, C_OR);
        prev = ifstatement;
      }

      else if ( !str_cmp( v1, "and" ) || !str_cmp( v1, "&&" ) )
      {
        arg = one_argument( arg, v1 );
        SET_BIT(andornot, C_AND);
        prev = ifstatement;
      }

      else
      {
        arg = one_argument( arg, s  );
        arg = one_argument( arg, v2 );

        if ( v1[0] == '%' )  strcpy(v1, varval( v1, ch ));
        if ( v2[0] == '%' )  strcpy(v2, varval( v2, ch ));
        if (  s[0] == '%' )  strcpy( s, varval( s,  ch ));

        ifstatement = boolchk( v1, v2, s );
      }

      if ( IS_SET(andornot, C_NOT) ) ifstatement = !ifstatement;
      if ( IS_SET(andornot, C_AND) ) ifstatement = ifstatement && prev;
      if ( IS_SET(andornot, C_OR)  ) ifstatement = ifstatement || prev;

    }
  }

  if ( !ifstatement ) SET_BIT( trigger->bits, SCRIPT_ADVANCE );
  else             REMOVE_BIT( trigger->bits, SCRIPT_ADVANCE );

  return ifstatement;
}

#undef C_NOT
#undef C_AND
#undef C_OR

void eval_walkto( CHAR_DATA * ch, TRIGGER_DATA * trigger, char * argument)
{
  char              buf[MAX_STRING_LENGTH];
  ROOM_INDEX_DATA * pRoom;
  int               dir;

  one_argument( argument, buf );
  REMOVE_BIT( trigger->bits, SCRIPT_ADVANCE );

  if ( ( pRoom = get_room_index( atoi( buf ) ) ) == NULL ) return;

  dir = find_first_step(ch->in_room, pRoom );

  if ( dir < 0 )
  {
    do_emote( ch, "has left." );
    char_from_room( ch );
    char_to_room( ch, pRoom );
    SET_BIT( trigger->bits, SCRIPT_ADVANCE );
    return;
  }

  else
  {
    move_char( ch, dir );
    if ( ch->in_room == pRoom )
    SET_BIT( trigger->bits, SCRIPT_ADVANCE );
    return;
  }
}

void eval_mob( CHAR_DATA * ch, TRIGGER_DATA * trigger, char * argument )
{
  char        arg1[MAX_STRING_LENGTH];
  char        arg2[MAX_STRING_LENGTH];
  char        arg3[MAX_STRING_LENGTH];
  CHAR_DATA * victim;

  argument = one_argument( argument, arg1 );    /* string */
  if ( arg1[0] == '%' ) strcpy( arg1, varval( arg1, ch ) );

  argument = one_argument( argument, arg2 );    /* string */
  if ( arg2[0] == '%' ) strcpy( arg2, varval( arg2, ch ) );

  argument = one_argument( argument, arg3 );    /* string */
  if ( arg3[0] == '%' ) strcpy( arg3, varval( arg3, ch ) );

  victim = !str_cmp( arg2, "self" ) ? ch : get_char_world( ch, arg2 );

  if ( !str_cmp( arg1, "lock" ) )
  {
    int door;

    door = get_dir( arg2 );

    if ( ch->in_room->exit[door] == NULL
      || !IS_SET(ch->in_room->exit[door]->exit_info, EX_ISDOOR ) )
    {
      eval_halt( ch, trigger, "once" );
      return;
    }

    SET_BIT( ch->in_room->exit[door]->exit_info, EX_LOCKED );

    if ( ch->in_room->exit[door]->to_room != NULL
      && ch->in_room->exit[door]->to_room->exit[rev_dir[door]] != NULL
      && IS_SET( ch->in_room->exit[door]->to_room->exit[rev_dir[door]]->exit_info, EX_ISDOOR ) )
    SET_BIT( ch->in_room->exit[door]->to_room->exit[rev_dir[door]]->exit_info, EX_LOCKED );
  }

  else if ( !str_cmp( arg1, "unlock" ) )
  {
    int door;

    door = get_dir( arg2 );
    if ( ch->in_room->exit[door] == NULL
      || !IS_SET(ch->in_room->exit[door]->exit_info, EX_ISDOOR) )
    {
      eval_halt( ch, trigger, "once" );
      return;
    }

    REMOVE_BIT( ch->in_room->exit[door]->exit_info, EX_LOCKED );

    if ( ch->in_room->exit[door]->to_room != NULL
      && ch->in_room->exit[door]->to_room->exit[rev_dir[door]] != NULL
      && IS_SET( ch->in_room->exit[door]->to_room->exit[rev_dir[door]]->exit_info, EX_ISDOOR ) )
    REMOVE_BIT( ch->in_room->exit[door]->to_room->exit[rev_dir[door]]->exit_info, EX_LOCKED );
  }

  else if ( !str_cmp( arg1, "open" ) )
  {
    int door;

    door = get_dir( arg2 );
    if ( ch->in_room->exit[door] == NULL
      || !IS_SET(ch->in_room->exit[door]->exit_info, EX_ISDOOR) )
    {
      eval_halt( ch, trigger, "once" );
      return;
    }

    REMOVE_BIT( ch->in_room->exit[door]->exit_info, EX_CLOSED );

    if ( ch->in_room->exit[door]->to_room != NULL
      && ch->in_room->exit[door]->to_room->exit[rev_dir[door]] != NULL
      && IS_SET( ch->in_room->exit[door]->to_room->exit[rev_dir[door]]->exit_info, EX_ISDOOR ) )
    REMOVE_BIT( ch->in_room->exit[door]->to_room->exit[rev_dir[door]]->exit_info, EX_CLOSED );
  }

  else if ( !str_cmp( arg1, "close" ) )
  {
    int door;

    door = get_dir( arg2 );

    if ( ch->in_room->exit[door] == NULL
      || !IS_SET(ch->in_room->exit[door]->exit_info, EX_ISDOOR) )
    {
      eval_halt( ch, trigger, "once" );
      return;
    }

    SET_BIT( ch->in_room->exit[door]->exit_info, EX_CLOSED );

    if ( ch->in_room->exit[door]->to_room != NULL
      && ch->in_room->exit[door]->to_room->exit[rev_dir[door]] != NULL
      && IS_SET( ch->in_room->exit[door]->to_room->exit[rev_dir[door]]->exit_info, EX_ISDOOR ) )
    SET_BIT( ch->in_room->exit[door]->to_room->exit[rev_dir[door]]->exit_info, EX_CLOSED );
  }

  else if ( !str_cmp( arg1, "trans" ) )
  {
    ROOM_INDEX_DATA * pRoom;

    if ( victim == NULL || ( pRoom = get_room_index( atoi(arg3) ) ) == NULL )
    {
      eval_halt( ch, trigger, "once" );
      return;
    }

    char_from_room( victim );
    char_to_room( victim, pRoom );
  }

  else if ( !str_cmp( arg1, "cash" ) )
  {
    int cost;

    cost = atoi(arg3);

    if ( victim == NULL )
    {
      eval_halt( ch, trigger, "once" );
      return;
    }

    if ( cost > 0 ) create_amount( cost, victim, NULL, NULL );
  }

  return;
}

void eval_obj( CHAR_DATA * ch, TRIGGER_DATA * trigger, char * argument )
{
  char       arg1[MAX_STRING_LENGTH];
  char       arg2[MAX_STRING_LENGTH];
  char       arg3[MAX_STRING_LENGTH];
  OBJ_DATA * obj;

  argument = one_argument( argument, arg1 );    /* string */
  if ( arg1[0] == '%' ) strcpy( arg1, varval( arg1, ch ) );

  argument = one_argument( argument, arg2 );    /* string */
  if ( arg2[0] == '%' ) strcpy( arg2, varval( arg2, ch ) );

  argument = one_argument( argument, arg3 );    /* string */
  if ( arg3[0] == '%' ) strcpy( arg3, varval( arg3, ch ) );

  obj = get_obj_world( ch, arg2 );

  if ( !str_cmp( arg1, "purge" ) )
  {
    if ( obj ) extract_obj(obj);
  }
}

void script_interpret( CHAR_DATA * ch, TRIGGER_DATA * trigger )
{
  char * buf;
  char   buf2[MAX_STRING_LENGTH];
  char   buf3[MAX_STRING_LENGTH];
  char * cmd;

  if ( trigger->current == NULL ) return;
  trigger->tracer++;

  cmd = next_command( trigger->current->command, buf3 );
  buf = one_argument( buf3, buf2 );

  while ( ( buf3[0] ) || ( cmd[0] ) )
  {
    if ( buf2[0] == ':' ); /* Ignore label */

    else if ( !str_cmp( buf2, "loop" ) )
    {
      eval_goto   ( ch, trigger, "0"  );
      cmd = trigger->current->command;
    }

    else if ( !str_cmp( buf2, "let"  ) )
      eval_assign( ch, trigger, buf );

    else if ( !str_cmp( buf2, "walkto" ) )
      eval_walkto( ch, trigger, buf );

    else if ( !str_cmp( buf2, "mob"  ) )
      eval_mob( ch, trigger, buf );

    else if ( !str_cmp( buf2, "obj"  ) )
      eval_obj( ch, trigger, buf );

    else if ( !str_cmp( buf2, "ofunc" ) )
      eval_ofunc( ch, trigger, buf );

    else if ( !str_cmp( buf2, "osign" ) )
      eval_osign( ch, trigger, buf );

    else if ( !str_cmp( buf2, "mfunc" ) )
      eval_mfunc( ch, trigger, buf );

    else if ( !str_cmp( buf2, "msign" ) )
      eval_msign( ch, trigger, buf );

    else if ( !str_cmp( buf2, "mlet"  ) )
      eval_massign( ch, trigger, buf );

    else if ( !str_cmp( buf2, "wfunc" ) )
      eval_wfunc( ch, trigger, buf );

    else if ( !str_cmp( buf2, "wsign" ) )
      eval_wsign( ch, trigger, buf );

    else if ( !str_cmp( buf2, "sfunc" ) )
      eval_sfunc( ch, trigger, buf );

    else if ( !str_cmp( buf2, "jump" ) )
    {
      eval_goto( ch, trigger, buf );
      cmd = trigger->current->command;
    }

    else if ( !str_cmp( buf2, "halt" ) )
    {
      if ( !eval_halt( ch, trigger, buf ) ) return;
    }

    else if ( !str_cmp( buf2, "while" )   )
    {
      if ( !eval_while( ch, trigger, buf ) ) return;
    }

    else if ( !str_cmp( buf2, "if" )   )
    {
      if (!eval_if     ( ch, trigger, buf )) return;
      else NOTIFY( "NOTIFY Script> true", LEVEL_IMMORTAL, WIZ_NOTIFY_SCRIPT );
    }

    else
    {
      eval_interp( ch, trigger, buf3 );
    }

    cmd = next_command( cmd, buf3 );
    buf = one_argument( buf3, buf2 );
  }

  return;
}

/*
 * Called from the mobile_update() procedure in update.c
 * This should be called every mobile pulse or so.
 */
void script_update( CHAR_DATA * ch, int timetype )
{
  TRIGGER_DATA * trig;

  if ( !IS_NPC(ch) || ch->deleted ) return;

  for ( trig = ch->triggers; trig; trig = trig->next )
  {
    if ( IS_SET( ch->act, ACT_HALT ) || IS_SET( trig->bits, SCRIPT_HALT ) )
    {
      trig->current = NULL;
      continue;
    }

    if ( trig->waiting > 0 )
    {
      trig->waiting--;
      continue;
    }

    if ( trig->trigger_type == timetype && trig->current == NULL )
    {
      trig->current = trig->script;
      trig->bits    = SCRIPT_ADVANCE;
    }

    if ( trig->current == NULL || !IS_SET(trig->bits, SCRIPT_ADVANCE) )
    {
      continue;
    }

    script_interpret( ch, trig );

    if ( trig->current ) trig->current = trig->current->next;
    else                 trig->tracer  = 0;
  }

  return;
}

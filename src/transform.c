/***************************************************************************
*  這是由輔大化學系製作群所撰寫的遊戲﹐主體由 merc 改編而來﹐所有的版權    *
*  將會被保留﹐但歡迎大家修改﹐但我們也希望你們也能提供給大家﹐所有的商    *
*  業行為將不被允許。                                                      *
*                                                                          *
*  paul@mud.ch.fju.edu.tw                                                  *
*  lc@mud.ch.fju.edu.tw                                                    *
*                                                                          *
***************************************************************************/

#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

#define PLAYER_DIR 	"/home/mud/mud/player"
#define BACKUP_DIR  	"/home/mud/mud/backup"
#define FAILCODE_DIR 	"/home/mud/mud/failcode"

void 	create_directory( const char * );

void main( void )
{
  DIR           * reading;
  struct dirent * next;
  struct stat     file_stat;
  char            head;
  char            dir_name[BUFSIZ];
  char            file_name[BUFSIZ];
  char            player_file[BUFSIZ];
  char            player[BUFSIZ];
  char            backup_file[BUFSIZ];
  char            backup[BUFSIZ];
  char            failcode_file[BUFSIZ];
  char            failcode[BUFSIZ];
  char            player_name[BUFSIZ];
  char            new_dir[BUFSIZ];
  char            exec_cmd[BUFSIZ];

  for ( head = 'a'; head <= 'z'; head++ )
  {
    sprintf( dir_name, "%s/%c", PLAYER_DIR, head );

    if ( ( reading = opendir( dir_name ) ) )
    {
      while ( ( next = readdir( reading ) ) )
      {
        sprintf( file_name, "%s/%s", dir_name, next->d_name );

        if ( lstat( file_name, &file_stat ) != 0 )
        {
          printf( "Error file stat %s\n\r", file_name );
          continue;
        }

        if ( !S_ISREG( file_stat.st_mode ) ) continue;

        str_cpy( player_name, next->d_name );

        if ( player_name[0] >= 'A' && player_name[0] <= 'Z' )
          player_name[0] += 'a' - 'A';

        sprintf( new_dir, "%s/%c/%s", PLAYER_DIR, head, player_name );

        create_directory( new_dir );

        sprintf( player_file, "%s/%c/%s"
          , PLAYER_DIR, head, next->d_name );

        sprintf( player, "%s/data", new_dir );

        if ( lstat( player_file, &file_stat ) == 0
          && S_ISREG( file_stat.st_mode ) )
        {
          sprintf( exec_cmd, "cp -fp %s %s", player_file, player );
          system( exec_cmd );

          printf( "%s\n\r", exec_cmd );
        }

        sprintf( backup_file, "%s/%c/%s.bak"
          , BACKUP_DIR, head, next->d_name );

        sprintf( backup, "%s/backup", new_dir );

        if ( lstat( backup_file, &file_stat ) == 0
          && S_ISREG( file_stat.st_mode ) )
        {
          sprintf( exec_cmd, "cp -fp %s %s", backup_file, backup );
          system( exec_cmd );

          printf( "%s\n\r", exec_cmd );
        }

        sprintf( failcode_file, "%s/%c/%s"
          , FAILCODE_DIR, head, next->d_name );

        sprintf( failcode, "%s/failcode", new_dir );

        if ( lstat( failcode_file, &file_stat ) == 0
          && S_ISREG( file_stat.st_mode ) )
        {
          sprintf( exec_cmd, "cp -fp %s %s", failcode_file, failcode );
          system( exec_cmd );

          printf( "%s\n\r", exec_cmd );
        }
      }

      closedir( reading );
    }

    else
    {
      printf( "Error open directory %s\n\r", dir_name );
    }
  }
}

void create_directory( const char * name )
{
  struct stat file_stat;

  if ( lstat( name, & file_stat ) != 0 )
  {
    if ( mkdir( name, S_IRWXU | S_IRWXG ) != 0 )
    {
      printf( "error create directory %s.\n\r", name );
      return;
    }

    return;
  }

  if ( !S_ISDIR( file_stat.st_mode ) )
  {
    printf( "%s is not a directory.\n\r", name );
    return;
  }

  return;
}

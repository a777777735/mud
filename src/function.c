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
#include "merc.h"

#if defined(FUNC_NAME)
#undef FUNC_NAME
#endif

#define FUNC_NAME( nFunction )                             \
  if ( !str_cmp( name, #nFunction ) ) RETURN( nFunction ); \

void * command_function_name( const char * name )
{
  PUSH_FUNCTION( "command_function_name" );

  switch( tolower( name[3] ) )
  {
  case 'a':
    FUNC_NAME( do_advance      );
    FUNC_NAME( do_aecho        );
    FUNC_NAME( do_alias        );
    FUNC_NAME( do_allow        );
    FUNC_NAME( do_angel        );
    FUNC_NAME( do_answer       );
    FUNC_NAME( do_areas        );
    FUNC_NAME( do_ask          );
    FUNC_NAME( do_at           );
    FUNC_NAME( do_auction      );
    FUNC_NAME( do_auto         );
    FUNC_NAME( do_autoexit     );
    FUNC_NAME( do_autoloot     );
    FUNC_NAME( do_automap      );
    FUNC_NAME( do_autosac      );
    FUNC_NAME( do_address      );
    break;

  case 'b':
    FUNC_NAME( do_backup       );
    FUNC_NAME( do_badfriend    );
    FUNC_NAME( do_bamfin       );
    FUNC_NAME( do_bamfout      );
    FUNC_NAME( do_ban          );
    FUNC_NAME( do_bet          );
    FUNC_NAME( do_blank        );
    FUNC_NAME( do_board        );
    FUNC_NAME( do_bounty       );
    FUNC_NAME( do_brief        );
    FUNC_NAME( do_bug          );
    FUNC_NAME( do_buy          );
    FUNC_NAME( do_bus          );
    break;

  case 'c':
    FUNC_NAME( do_cachet       );
    FUNC_NAME( do_call         );
    FUNC_NAME( do_cast         );
    FUNC_NAME( do_cat          );
    FUNC_NAME( do_cchat        );
    FUNC_NAME( do_cd           );
    FUNC_NAME( do_chance       );
    FUNC_NAME( do_channels     );
    FUNC_NAME( do_chat         );
    FUNC_NAME( do_check        );
    FUNC_NAME( do_cleanup      );
    FUNC_NAME( do_close        );
    FUNC_NAME( do_cls          );
    FUNC_NAME( do_cname        );
    FUNC_NAME( do_combine      );
    FUNC_NAME( do_commands     );
    FUNC_NAME( do_compare      );
    FUNC_NAME( do_config       );
    FUNC_NAME( do_consider     );
    FUNC_NAME( do_cp           );
    FUNC_NAME( do_crave        );
    FUNC_NAME( do_credit       );
    FUNC_NAME( do_club         );
    FUNC_NAME( do_clubset      );
    FUNC_NAME( do_clubchat     );
    break;

  case 'd':
    FUNC_NAME( do_data         );
    FUNC_NAME( do_damage       );
    FUNC_NAME( do_debug        );
    FUNC_NAME( do_deny         );
    FUNC_NAME( do_deposit      );
    FUNC_NAME( do_description  );
    FUNC_NAME( do_directory    );
    FUNC_NAME( do_disconnect   );
    FUNC_NAME( do_dormancy     );
    FUNC_NAME( do_donate       );
    FUNC_NAME( do_down         );
    FUNC_NAME( do_drink        );
    FUNC_NAME( do_drop         );
    break;

  case 'e':
    FUNC_NAME( do_edit         );
    FUNC_NAME( do_enter        );
    FUNC_NAME( do_endow        );
    FUNC_NAME( do_enquire      );
    FUNC_NAME( do_email        );
    FUNC_NAME( do_event        );
    FUNC_NAME( do_east         );
    FUNC_NAME( do_eat          );
    FUNC_NAME( do_echo         );
    FUNC_NAME( do_emote        );
    FUNC_NAME( do_enable       );
    FUNC_NAME( do_equipment    );
    FUNC_NAME( do_examine      );
    FUNC_NAME( do_exits        );
    break;

  case 'f':
    FUNC_NAME( do_fcntl        );
    FUNC_NAME( do_feed         );
    FUNC_NAME( do_fight        );
    FUNC_NAME( do_find         );
    FUNC_NAME( do_filename     );
    FUNC_NAME( do_filestat     );
    FUNC_NAME( do_fill         );
    FUNC_NAME( do_finger       );
    FUNC_NAME( do_fire         );
    FUNC_NAME( do_flee         );
    FUNC_NAME( do_flow         );
    FUNC_NAME( do_follow       );
    FUNC_NAME( do_force        );
    FUNC_NAME( do_forsake      );
    FUNC_NAME( do_free         );
    FUNC_NAME( do_freeze       );
    FUNC_NAME( do_friend       );
    break;

  case 'g':
    FUNC_NAME( do_gamble       );
    FUNC_NAME( do_gchat        );
    FUNC_NAME( do_get          );
    FUNC_NAME( do_gift         );
    FUNC_NAME( do_give         );
    FUNC_NAME( do_goto         );
    FUNC_NAME( do_grep         );
    FUNC_NAME( do_group        );
    FUNC_NAME( do_gtell        );
    break;

  case 'h':
    FUNC_NAME( do_habilitate   );
    FUNC_NAME( do_help         );
    FUNC_NAME( do_hero         );
    FUNC_NAME( do_hire         );
    FUNC_NAME( do_history      );
    FUNC_NAME( do_holylight    );
    FUNC_NAME( do_home         );
    FUNC_NAME( do_hp           );
    break;

  case 'i':
    FUNC_NAME( do_idea         );
    FUNC_NAME( do_immtalk      );
    FUNC_NAME( do_inventory    );
    FUNC_NAME( do_invis        );
    FUNC_NAME( do_ipcs         );
    break;

  case 'j':
    FUNC_NAME( do_jail         );
    FUNC_NAME( do_joke         );
    break;

  case 'k':
    FUNC_NAME( do_kill         );
    break;

  case 'l':
    FUNC_NAME( do_learn        );
    FUNC_NAME( do_leftskill    );
    FUNC_NAME( do_level        );
    FUNC_NAME( do_lfind        );
    FUNC_NAME( do_liquid       );
    FUNC_NAME( do_list         );
    FUNC_NAME( do_llink        );
    FUNC_NAME( do_llookup      );
    FUNC_NAME( do_load         );
    FUNC_NAME( do_lock         );
    FUNC_NAME( do_log          );
    FUNC_NAME( do_look         );
    FUNC_NAME( do_lore         );
    FUNC_NAME( do_lotto        );
    FUNC_NAME( do_ls           );
    FUNC_NAME( do_lset         );
    FUNC_NAME( do_lockcmd      );
    break;

  case 'm':
    FUNC_NAME( do_map          );
    FUNC_NAME( do_marry        );
    FUNC_NAME( do_memory       );
    FUNC_NAME( do_md           );
    FUNC_NAME( do_mfind        );
    FUNC_NAME( do_mode         );
    FUNC_NAME( do_miset        );
    FUNC_NAME( do_mlevel       );
    FUNC_NAME( do_mlist        );
    FUNC_NAME( do_mload        );
    FUNC_NAME( do_mpadd        );
    FUNC_NAME( do_mpasound     );
    FUNC_NAME( do_mpat         );
    FUNC_NAME( do_mpchat       );
    FUNC_NAME( do_mpecho       );
    FUNC_NAME( do_mpechoaround );
    FUNC_NAME( do_mpechoat     );
    FUNC_NAME( do_mpclean      );
    FUNC_NAME( do_mpforce      );
    FUNC_NAME( do_mpgoto       );
    FUNC_NAME( do_mpjunk       );
    FUNC_NAME( do_mpkill       );
    FUNC_NAME( do_mpmload      );
    FUNC_NAME( do_mplset       );
    FUNC_NAME( do_mpoload      );
    FUNC_NAME( do_mppurge      );
    FUNC_NAME( do_mpremenemy   );
    FUNC_NAME( do_mpremquest   );
    FUNC_NAME( do_mpsay        );
    FUNC_NAME( do_mpsetenemy   );
    FUNC_NAME( do_mpsetquest   );
    FUNC_NAME( do_mpstat       );
    FUNC_NAME( do_mptransfer   );
    FUNC_NAME( do_mset         );
    FUNC_NAME( do_mstat        );
    FUNC_NAME( do_multi        );
    FUNC_NAME( do_murder       );
    FUNC_NAME( do_music        );
    FUNC_NAME( do_mwhere       );
    FUNC_NAME( do_mount        );
    FUNC_NAME( do_mud          );
    break;

  case 'n':
    FUNC_NAME( do_noemote      );
    FUNC_NAME( do_nodeath      );
    FUNC_NAME( do_north        );
    FUNC_NAME( do_note         );
    FUNC_NAME( do_notell       );
    FUNC_NAME( do_notepad      );
    FUNC_NAME( do_notify       );
    FUNC_NAME( do_nuke         );
    FUNC_NAME( do_netstat      );
    break;

  case 'o':
    FUNC_NAME( do_oaf          );
    FUNC_NAME( do_ofind        );
    FUNC_NAME( do_okip         );
    FUNC_NAME( do_olevel       );
    FUNC_NAME( do_olist        );
    FUNC_NAME( do_oload        );
    FUNC_NAME( do_open         );
    FUNC_NAME( do_order        );
    FUNC_NAME( do_oset         );
    FUNC_NAME( do_ostat        );
    FUNC_NAME( do_otype        );
    FUNC_NAME( do_out          );
    FUNC_NAME( do_owhere       );
    break;

  case 'p':
    FUNC_NAME( do_pagelen      );
    FUNC_NAME( do_paint        );
    FUNC_NAME( do_pardon       );
    FUNC_NAME( do_password     );
    FUNC_NAME( do_peace        );
    FUNC_NAME( do_perm         );
    FUNC_NAME( do_pick         );
    FUNC_NAME( do_phone        );
    FUNC_NAME( do_ply          );
    FUNC_NAME( do_post         );
    FUNC_NAME( do_prompt       );
    FUNC_NAME( do_prof         );
    FUNC_NAME( do_pry          );
    FUNC_NAME( do_purge        );
    FUNC_NAME( do_put          );
    FUNC_NAME( do_ps           );
    FUNC_NAME( do_pwd          );
    break;

  case 'q':
    FUNC_NAME( do_qset         );
    FUNC_NAME( do_qstat        );
    FUNC_NAME( do_query        );
    FUNC_NAME( do_question     );
    FUNC_NAME( do_quit         );
    break;

  case 'r':
    FUNC_NAME( do_read         );
    FUNC_NAME( do_release      );
    FUNC_NAME( do_rebirth      );
    FUNC_NAME( do_reboot       );
    FUNC_NAME( do_recall       );
    FUNC_NAME( do_recho        );
    FUNC_NAME( do_recite       );
    FUNC_NAME( do_reload       );
    FUNC_NAME( do_remove       );
    FUNC_NAME( do_rename       );
    FUNC_NAME( do_reply        );
    FUNC_NAME( do_report       );
    FUNC_NAME( do_rest         );
    FUNC_NAME( do_restore      );
    FUNC_NAME( do_retire       );
    FUNC_NAME( do_rd           );
    FUNC_NAME( do_rlimit       );
    FUNC_NAME( do_rm           );
    FUNC_NAME( do_rset         );
    FUNC_NAME( do_rstat        );
    FUNC_NAME( do_repair       );
    break;

  case 's':
    FUNC_NAME( do_sacrifice    );
    FUNC_NAME( do_sale         );
    FUNC_NAME( do_save         );
    FUNC_NAME( do_say          );
    FUNC_NAME( do_score        );
    FUNC_NAME( do_see          );
    FUNC_NAME( do_search       );
    FUNC_NAME( do_sefind       );
    FUNC_NAME( do_sell         );
    FUNC_NAME( do_semote       );
    FUNC_NAME( do_serial       );
    FUNC_NAME( do_shout        );
    FUNC_NAME( do_ship         );
    FUNC_NAME( do_shutdown     );
    FUNC_NAME( do_silence      );
    FUNC_NAME( do_slay         );
    FUNC_NAME( do_sleep        );
    FUNC_NAME( do_snoop        );
    FUNC_NAME( do_socials      );
    FUNC_NAME( do_south        );
    FUNC_NAME( do_split        );
    FUNC_NAME( do_spy          );
    FUNC_NAME( do_sset         );
    FUNC_NAME( do_stand        );
    FUNC_NAME( do_status       );
    FUNC_NAME( do_stock        );
    FUNC_NAME( do_store        );
    FUNC_NAME( do_study        );
    FUNC_NAME( do_suicide      );
    FUNC_NAME( do_sysconf      );
    FUNC_NAME( do_sysinfo      );
    FUNC_NAME( do_system       );
    FUNC_NAME( do_sector       );
    FUNC_NAME( do_send         );
    FUNC_NAME( do_shop         );
    FUNC_NAME( do_site         );
    break;

  case 't':
    FUNC_NAME( do_task         );
    FUNC_NAME( do_tcp          );
    FUNC_NAME( do_tell         );
    FUNC_NAME( do_test         );
    FUNC_NAME( do_tick         );
    FUNC_NAME( do_time         );
    FUNC_NAME( do_title        );
    FUNC_NAME( do_top          );
    FUNC_NAME( do_toss         );
    FUNC_NAME( do_touch        );
    FUNC_NAME( do_trade        );
    FUNC_NAME( do_train        );
    FUNC_NAME( do_transfer     );
    FUNC_NAME( do_trust        );
    FUNC_NAME( do_turn         );
    FUNC_NAME( do_typo         );
    break;

  case 'u':
    FUNC_NAME( do_uname        );
    FUNC_NAME( do_unlock       );
    FUNC_NAME( do_up           );
    FUNC_NAME( do_users        );
    FUNC_NAME( do_unmount      );
    break;

  case 'v':
    FUNC_NAME( do_value        );
    FUNC_NAME( do_visible      );
    FUNC_NAME( do_vocation     );
    FUNC_NAME( do_vote         );
    break;

  case 'w':
    FUNC_NAME( do_wake         );
    FUNC_NAME( do_wanted       );
    FUNC_NAME( do_wear         );
    FUNC_NAME( do_weather      );
    FUNC_NAME( do_west         );
    FUNC_NAME( do_where        );
    FUNC_NAME( do_whereis      );
    FUNC_NAME( do_who          );
    FUNC_NAME( do_wimpy        );
    FUNC_NAME( do_withdraw     );
    FUNC_NAME( do_wizhelp      );
    FUNC_NAME( do_wizlist      );
    FUNC_NAME( do_wizlock      );
    break;

  case 'x':
    FUNC_NAME( do_xnames       );
    break;

  case 'y':
    FUNC_NAME( do_yell         );
    break;
  }

  RETURN( NULL );
}

void * skill_function_name( const char * name )
{
  PUSH_FUNCTION( "skill_function_name" );

  FUNC_NAME( skill_sneak              );
  FUNC_NAME( skill_hide               );
  FUNC_NAME( skill_mask               );
  FUNC_NAME( spell_make_spring        );
  FUNC_NAME( spell_make_light         );
  FUNC_NAME( spell_cast_armor         );
  FUNC_NAME( spell_cast_bless         );
  FUNC_NAME( spell_cast_fire          );
  FUNC_NAME( spell_cast_lightning     );
  FUNC_NAME( cast_abstract            );
  FUNC_NAME( cast_acid_blast          );
  FUNC_NAME( cast_acid_breath         );
  FUNC_NAME( cast_blindness           );
  FUNC_NAME( cast_burning_hands       );
  FUNC_NAME( cast_call_lightning      );
  FUNC_NAME( cast_cause_critical      );
  FUNC_NAME( cast_cause_light         );
  FUNC_NAME( cast_cause_serious       );
  FUNC_NAME( cast_change_sex          );
  FUNC_NAME( cast_charm_person        );
  FUNC_NAME( cast_chill_touch         );
  FUNC_NAME( cast_colour_spray        );
  FUNC_NAME( cast_control_weather     );
  FUNC_NAME( cast_create_food         );
  FUNC_NAME( cast_create_water        );
  FUNC_NAME( cast_cure_blindness      );
  FUNC_NAME( cast_cure_critical       );
  FUNC_NAME( cast_cure_light          );
  FUNC_NAME( cast_cure_poison         );
  FUNC_NAME( cast_cure_serious        );
  FUNC_NAME( cast_curse               );
  FUNC_NAME( cast_detect_evil         );
  FUNC_NAME( cast_detect_hidden       );
  FUNC_NAME( cast_detect_invis        );
  FUNC_NAME( cast_detect_magic        );
  FUNC_NAME( cast_detect_mask         );
  FUNC_NAME( cast_detect_poison       );
  FUNC_NAME( cast_dispel_evil         );
  FUNC_NAME( cast_dispel_magic        );
  FUNC_NAME( cast_earthquake          );
  FUNC_NAME( cast_enchant_weapon      );
  FUNC_NAME( cast_energy_drain        );
  FUNC_NAME( cast_faerie_fire         );
  FUNC_NAME( cast_faerie_fog          );
  FUNC_NAME( cast_fire_breath         );
  FUNC_NAME( cast_fireball            );
  FUNC_NAME( cast_flamestrike         );
  FUNC_NAME( cast_fly                 );
  FUNC_NAME( cast_frost_breath        );
  FUNC_NAME( cast_gas_breath          );
  FUNC_NAME( cast_gate                );
  FUNC_NAME( cast_general_purpose     );
  FUNC_NAME( cast_giant_strength      );
  FUNC_NAME( cast_harm                );
  FUNC_NAME( cast_heal                );
  FUNC_NAME( cast_high_explosive      );
  FUNC_NAME( cast_identify            );
  FUNC_NAME( cast_infravision         );
  FUNC_NAME( cast_invis               );
  FUNC_NAME( cast_judge               );
  FUNC_NAME( cast_know_alignment      );
  FUNC_NAME( cast_lightning_bolt      );
  FUNC_NAME( cast_lightning_breath    );
  FUNC_NAME( cast_locate_object       );
  FUNC_NAME( cast_magic_missile       );
  FUNC_NAME( cast_mass_invis          );
  FUNC_NAME( cast_null                );
  FUNC_NAME( cast_pass_door           );
  FUNC_NAME( cast_poison              );
  FUNC_NAME( cast_protection          );
  FUNC_NAME( cast_pry                 );
  FUNC_NAME( cast_refresh             );
  FUNC_NAME( cast_remove_curse        );
  FUNC_NAME( cast_sanctuary           );
  FUNC_NAME( cast_sparkler            );
  FUNC_NAME( cast_search              );
  FUNC_NAME( cast_scan                );
  FUNC_NAME( cast_shield              );
  FUNC_NAME( cast_shocking_grasp      );
  FUNC_NAME( cast_sleep               );
  FUNC_NAME( cast_stone_skin          );
  FUNC_NAME( cast_summon              );
  FUNC_NAME( cast_teleport            );
  FUNC_NAME( cast_ventriloquate       );
  FUNC_NAME( cast_weaken              );
  FUNC_NAME( cast_word_of_recall      );
  FUNC_NAME( cast_seven_key_numen     );
  FUNC_NAME( cast_fire_elf            );
  FUNC_NAME( cast_dormancy            );
  FUNC_NAME( cast_physique            );
  FUNC_NAME( cast_meditation          );
  FUNC_NAME( cast_anti_physique       );
  FUNC_NAME( cast_fixity              );
  FUNC_NAME( cast_venom               );
  FUNC_NAME( cast_develop             );
  FUNC_NAME( cast_repair              );
  FUNC_NAME( cast_cook                );
  FUNC_NAME( cast_steal               );
  FUNC_NAME( cast_pick                );
  FUNC_NAME( cast_defloration         );
  FUNC_NAME( cast_cannibal            );
  FUNC_NAME( cast_wakeup              );
  FUNC_NAME( cast_evil_power          );
  FUNC_NAME( cast_crash_weapon        );
  FUNC_NAME( cast_bystander_fist      );
  FUNC_NAME( cast_flying_stone        );
  FUNC_NAME( cast_slow                );
  FUNC_NAME( cast_mint                );
  FUNC_NAME( cast_power_word          );
  FUNC_NAME( cast_preserve            );
  FUNC_NAME( cast_cure_overdose       );
  FUNC_NAME( cast_gold_shield         );
  FUNC_NAME( cast_iron_shield         );
  FUNC_NAME( cast_deva_shield         );
  FUNC_NAME( cast_remeday             );
  FUNC_NAME( cast_magic_power         );
  FUNC_NAME( cast_restore_staff       );
  FUNC_NAME( cast_wind_blade          );
  FUNC_NAME( cast_vacuum_wind         );
  FUNC_NAME( cast_haste               );
  FUNC_NAME( cast_super_refresh       );
  FUNC_NAME( cast_dark_hell           );
  FUNC_NAME( cast_six_god             );
  FUNC_NAME( cast_backstab            );
  FUNC_NAME( cast_water_cloud_blast   );
  FUNC_NAME( cast_fun_wu_blast        );
  FUNC_NAME( cast_make_sword          );
  RETURN( NULL );
}

void * check_function_name( const char * name )
{
  PUSH_FUNCTION( "limit_function_name" );

  FUNC_NAME( check_sword_attack   );
  FUNC_NAME( check_unrigid_attack );
  FUNC_NAME( check_pc_attack      );
  FUNC_NAME( check_spear_attack   );
  FUNC_NAME( check_blade_attack   );
  FUNC_NAME( check_whip_attack    );
  FUNC_NAME( check_dagger_attack  );
  FUNC_NAME( check_axe_attack     );
  FUNC_NAME( check_pen_attack     );
  FUNC_NAME( check_bow_attack     );
  FUNC_NAME( check_hammer_attack  );
  FUNC_NAME( check_club_attack    );

  RETURN( NULL );
}

void * check_routine_name( const char * name )
{
  PUSH_FUNCTION( "check_routine_name" );

  FUNC_NAME( routine_none );

  RETURN( NULL );
}

void * angel_function( const char * name )
{
  PUSH_FUNCTION( "angel_function" );

  FUNC_NAME( angel_mount        );
  FUNC_NAME( angel_sleep        );
  FUNC_NAME( angel_drink        );
  FUNC_NAME( angel_eat          );
  FUNC_NAME( angel_marry        );
  FUNC_NAME( angel_wedding      );
  FUNC_NAME( angel_alias        );
  FUNC_NAME( angel_auction      );
  FUNC_NAME( angel_bet          );
  FUNC_NAME( angel_areas        );
  FUNC_NAME( angel_xname        );
  FUNC_NAME( angel_immortal     );
  FUNC_NAME( angel_deposit_gold );
  FUNC_NAME( angel_deposit_obj  );
  FUNC_NAME( angel_stock        );
  FUNC_NAME( angel_gamble       );
  FUNC_NAME( angel_direction    );
  FUNC_NAME( angel_shop         );
  FUNC_NAME( angel_smith        );
  FUNC_NAME( angel_mercenary    );
  FUNC_NAME( angel_coper        );
  FUNC_NAME( angel_kill         );
  FUNC_NAME( angel_bank         );
  FUNC_NAME( angel_obj_bank     );
  FUNC_NAME( angel_get          );
  FUNC_NAME( angel_drop         );
  FUNC_NAME( angel_withdraw     );
  FUNC_NAME( angel_unmount      );
  FUNC_NAME( angel_call         );
  FUNC_NAME( angel_ship         );
  FUNC_NAME( angel_bus          );
  FUNC_NAME( angel_lore         );
  FUNC_NAME( angel_badman       );
  FUNC_NAME( angel_wake         );
  FUNC_NAME( angel_lotto        );
  FUNC_NAME( angel_hero         );
  FUNC_NAME( angel_autosac      );
  FUNC_NAME( angel_autoloot     );
  FUNC_NAME( angel_friend       );
  FUNC_NAME( angel_wimpy        );
  FUNC_NAME( angel_enable       );
  FUNC_NAME( angel_flee         );
  FUNC_NAME( angel_attack       );
  FUNC_NAME( angel_meditation   );
  FUNC_NAME( angel_description  );
  FUNC_NAME( angel_club         );
  FUNC_NAME( angel_group        );
  FUNC_NAME( angel_nogroup      );
  FUNC_NAME( angel_reply        );
  FUNC_NAME( angel_fountain     );
  FUNC_NAME( angel_food         );
  FUNC_NAME( angel_gambleset    );
  FUNC_NAME( angel_weather      );
  FUNC_NAME( angel_repair       );
  FUNC_NAME( angel_crave        );
  FUNC_NAME( angel_sac          );
  FUNC_NAME( angel_wear         );
  FUNC_NAME( angel_study        );
  FUNC_NAME( angel_learn        );
  FUNC_NAME( angel_recall       );
  FUNC_NAME( angel_recallset    );
  FUNC_NAME( angel_split        );
  FUNC_NAME( angel_train        );
  FUNC_NAME( angel_forsake      );
  FUNC_NAME( angel_enemy        );
  FUNC_NAME( angel_blind        );
  FUNC_NAME( angel_poison       );
  FUNC_NAME( angel_board        );
  FUNC_NAME( angel_firman       );
  FUNC_NAME( angel_money        );
  FUNC_NAME( angel_fighting_1   );
  FUNC_NAME( angel_fighting_2   );
  FUNC_NAME( angel_cname        );
  FUNC_NAME( angel_nothing      );
  FUNC_NAME( angel_reboot       );
  FUNC_NAME( angel_message      );
  FUNC_NAME( angel_config_flee  );
  FUNC_NAME( angel_exact        );
  FUNC_NAME( angel_mobile       );
  FUNC_NAME( angel_note         );
  FUNC_NAME( angel_leftskill    );
  FUNC_NAME( angel_north        );
  FUNC_NAME( angel_south        );
  FUNC_NAME( angel_east         );
  FUNC_NAME( angel_west         );
  FUNC_NAME( angel_up           );
  FUNC_NAME( angel_down         );
  FUNC_NAME( angel_enter        );
  FUNC_NAME( angel_out          );
  FUNC_NAME( angel_spy          );
  FUNC_NAME( angel_cast         );
  FUNC_NAME( angel_ply          );
  FUNC_NAME( angel_exits        );
  FUNC_NAME( angel_search       );
  FUNC_NAME( angel_inventory    );
  FUNC_NAME( angel_fight_join   );
  FUNC_NAME( angel_fight_bet    );

  RETURN( NULL );
}

EVENT_FUN * event_function( const char * name )
{
  PUSH_FUNCTION( "event_function" );

  FUNC_NAME( event_thunder       );
  FUNC_NAME( event_solar_eclipse );
  FUNC_NAME( event_month_eclipse );
  FUNC_NAME( event_drought       );
  FUNC_NAME( event_locust        );
  RETURN( NULL );
}

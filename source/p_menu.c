#include "g_local.h"

void StartGame(edict_t* ent);

void V_Free(void* mem)
{
/*
#ifndef GDS_NOMULTITHREADING
	pthread_mutex_lock(&MemMutex_Free);
#endif
*/
	gi.TagFree(mem);
/*
#ifndef GDS_NOMULTITHREADING
	pthread_mutex_unlock(&MemMutex_Free);
#endif
*/
}

void clearmenu(edict_t* ent) {
	int		i = 0;

	if (ent->client->menustorage.menu_active) // checks to see if the menu is showing
		return;

	for (i = 0; i < MAX_LINES; i++) {
		ent->client->menustorage.messages[i].option = 0;
		if (ent->client->menustorage.messages[i].msg != NULL) {
			V_Free(ent->client->menustorage.messages[i].msg);
			//GHz START
			ent->client->menustorage.messages[i].msg = NULL;
			//GHz END
		}
	}
	//GHz START
	// keep record of last known menu for multi-purpose menus that have more than one handler
	if (ent->client->menustorage.optionselected)
		ent->client->menustorage.oldmenuhandler = ent->client->menustorage.optionselected;
	if (ent->client->menustorage.currentline)
		ent->client->menustorage.oldline = ent->client->menustorage.currentline;
	//GHz END
	ent->client->menustorage.optionselected = NULL;
	ent->client->menustorage.currentline = 0;
	ent->client->menustorage.num_of_lines = 0;
	ent->client->menustorage.menu_index = 0; // 3.45
}

void closemenu(edict_t* ent)
{
//	if (debuginfo->value)
		gi.dprintf("DEBUG: closemenu()\n");

	clearmenu(ent); // reset all menu variables and strings
	ent->client->showscores = false;
	ent->client->menustorage.menu_active = false;
	ent->client->menustorage.displaymsg = false;
	ent->client->showinventory = false;
//	ent->client->trading = false; // done trading
}

void JoinTheGame(edict_t* ent) {
	int		returned;

	if (ent->client->menustorage.menu_active) {
		closemenu(ent);
		return;
	}

	if (gds->value)	returned = GDS_OpenConfigFile(ent);
	else			returned = OpenConfigFile(ent);

	switch (returned)
	{
	case -1:	//bad password
		gi.cprintf(ent, PRINT_HIGH, "Access denied. Incorrect password.\n");
		return;
	case -2:	//below minimum level
		gi.cprintf(ent, PRINT_HIGH, "You have to be at least level %d to play on this server.\n", ((int)min_level->value));
		return;
	case -3:	//above maximum level
		gi.cprintf(ent, PRINT_HIGH, "You have to be level %d or below to play here.\n", ((int)max_level->value));
		if (strcmp(reconnect_ip->string, "0") != 0) {
			gi.cprintf(ent, PRINT_HIGH, "You are being sent to an alternate server where you can play.\n");
			stuffcmd(ent, va("connect %s\n", reconnect_ip->string));
		}
		return;
	case -4:	//invalid player name
		gi.cprintf(ent, PRINT_HIGH, "Your name must be greater than 2 characters long.\n");
		return;
	case -5:	//playing too much
		gi.cprintf(ent, PRINT_HIGH, "Can't join: %d hour play-time limit reached.\n", MAX_HOURS);
		gi.cprintf(ent, PRINT_HIGH, "Please try a different character, or try again tommorow.\n");
		return;
	case -6:	//newbie basher can't play
		gi.cprintf(ent, PRINT_HIGH, "Unable to join: The current maximum level is %d.\n", NEWBIE_BASHER_MAX);
		gi.cprintf(ent, PRINT_HIGH, "Please return at a later time, or try a different character.\n");
		gi.dprintf("INFO: %s exceeds maximum level allowed by server (level %d)!", ent->client->pers.netname, NEWBIE_BASHER_MAX);
		return;
	case -7:	//boss can't play
		gi.cprintf(ent, PRINT_HIGH, "Unable to join: Bosses are not allowed unless the server is at least half capacity.\n");
		gi.cprintf(ent, PRINT_HIGH, "Please come back at a later time, or try a different character.\n");
		return;
	default:	//passed every check
		break;
	}

/* Disabled for now -JukS (15.03.2021)
	if (ptr->value) {
		OpenPTRJoinMenu(ent);
		return;
	}

	if (domination->value) {
		OpenDOMJoinMenu(ent);
		return;
	}

	if (ctf->value) {
		CTF_OpenJoinMenu(ent);
		return;
	}

	if (ent->myskills.class_num == 0) {
		OpenClassMenu(ent, 1); //GHz
		return;
	}
*/

	StartGame(ent);

	if (ent->myskills.inuse) {
		gi.cprintf(ent, PRINT_HIGH, "WARNING: Your character file is marked as already being open!\n");
		gi.cprintf(ent, PRINT_HIGH, "Logging into a server twice is not permitted. A message will be sent to an administrator.\n");
//		gi.dprintf("WARNING: %s's file is marked as already open at %s on %s.\n", ent->client->pers.netname, CURRENT_TIME, CURRENT_DATE);
	}
	ent->myskills.inuse = 1;

}

/*
=============
Returns false if the client has another menu open
=============
*/
qboolean ShowMenu(edict_t* ent)
{
	if (ent->client->showscores || ent->client->showinventory
		|| ent->client->menustorage.menu_active)
		return false;
	return true;
}


void Pick_respawnweapon(edict_t* ent) {
	gitem_t* item;

	switch (ent->myskills.respawn_weapon)
	{
	case 1:
		item = GET_ITEM(MK23_NUM);
		break;
	case 2:
		item = GET_ITEM(MP5_NUM);
		break;
	case 3:
		item = GET_ITEM(M4_NUM);
		break;
	case 4:
		item = GET_ITEM(M3_NUM);
		break;
	case 5:
		item = GET_ITEM(HC_NUM);
		break;
	case 6:
		item = GET_ITEM(SNIPER_NUM);
		break;
	case 7:
		item = GET_ITEM(DUAL_NUM);
		break;
	case 8:
		item = GET_ITEM(KNIFE_NUM);
		break;
	default:
		item = GET_ITEM(MK23_NUM);
		break;
	}

	ent->client->pers.selected_item = ITEM_INDEX(item);
	ent->client->pers.weapon = item;
	ent->client->pers.lastweapon = item;
	ent->client->newweapon = item;
	ChangeWeapon(ent);
}


void StartGame(edict_t* ent)
{
	int		i;
	gitem_t* item = itemlist;

	ent->svflags &= ~SVF_NOCLIENT;
	ent->client->resp.spectator = false;
	ent->client->pers.spectator = false;
	ent->client->ps.stats[STAT_SPECTATOR] = 0;
	PutClientInServer(ent);

	ent->client->pers.combat_changed = ent->myskills.respawns;//4.5 set changed combat preferences to default

	average_player_level = AveragePlayerLevel();
	ent->health = ent->myskills.current_health;
	for (i = 0; i < game.num_items; i++, item++)
		ent->client->pers.inventory[ITEM_INDEX(item)] = ent->myskills.inventory[ITEM_INDEX(item)];
//	ent->client->pers.inventory[flag_index] = 0; // NO FLAG FOR YOU!!!
//	ent->client->pers.inventory[red_flag_index] = 0;
//	ent->client->pers.inventory[blue_flag_index] = 0;
	//4.2 remove techs
//	ent->client->pers.inventory[resistance_index] = 0;
//	ent->client->pers.inventory[strength_index] = 0;
//	ent->client->pers.inventory[haste_index] = 0;
//	ent->client->pers.inventory[regeneration_index] = 0;
//	ent->touch = player_touch;
	modify_max(ent);
	Pick_respawnweapon(ent);

	// add a teleportation effect
	ent->s.event = EV_PLAYER_TELEPORT;
	// hold in place briefly
	ent->client->ps.pmove.pm_flags = PMF_TIME_TELEPORT;
	ent->client->ps.pmove.pm_time = 14;

	gi.bprintf(PRINT_HIGH, "%s starts their reign.\n", ent->client->pers.netname);


	//Set the player's name
	strcpy(ent->myskills.player_name, ent->client->pers.netname);
/* Most probably no need in AQ2 -JukS (15.03.2021)
	if (level.time < pregame_time->value) {
		gi.centerprintf(ent, "This map is currently in pre-game\nPlease warm up, upgrade and\naccess the Armory now\n");
		ent->s.effects |= EF_COLOR_SHELL;
		ent->s.renderfx |= (RF_SHELL_RED | RF_SHELL_GREEN | RF_SHELL_BLUE);
	} */
	gi.sound(ent, CHAN_VOICE, gi.soundindex("misc/startup.wav"), 1, ATTN_NORM, 0);
	WriteToLogfile(ent, "Logged in.\n");

	// LOAD PLAYER HERE -JukS (15.03.2021)
	openPlayer(ent);
	// end -JukS
}
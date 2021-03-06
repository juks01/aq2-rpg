#include "g_local.h"
#define SAVE_VERSION "v1.0"

qboolean SavePlayer(edict_t *ent);	//Called by savePlayer(). Don't call this directly.
int CountAbilities(edict_t* player);
int CountRunes(edict_t* player);
int CountWeapons(edict_t* player);
int FindAbilityIndex(int index, edict_t* player);
int FindWeaponIndex(int index, edict_t* player);
int FindRuneIndex(int index, edict_t* player);

#if defined(_WIN32)
#include <process.h>

long ReadLong(FILE* fptr) {
	long Value;
	if (fread(&Value, sizeof(long), 1, fptr) == 0)
		gi.dprintf("Error in %s\n", __func__);
	return Value;
}

int ReadInteger(FILE* fptr) {
	int Value;
	if (fread(&Value, sizeof(int), 1, fptr) == 0)
		gi.dprintf("Error in %s\n", __func__);
	return Value;
}

char ReadChar(FILE* fptr) {
	char Value;
	if (fread(&Value, sizeof(char), 1, fptr) == 0)
		gi.dprintf("Error in %s\n", __func__);
	return Value;
}

void ReadString(char* buf, FILE* fptr) {
	int Length;
	Length = ReadChar(fptr);	//get the string length
	if (Length == 0)			//If string is empty, abort
		buf[0] = 0;
	buf[Length] = 0;			//Null terminate the string just read
}


void WriteChar(FILE* fptr, char Value) {
	fwrite(&Value, sizeof(char), 1, fptr);
}

void WriteString(FILE* fptr, char* String) {
	int Length = strlen(String);
	WriteChar(fptr, (char)Length);
	fwrite(String, Length, 1, fptr);
}

void WriteInteger(FILE* fptr, int Value) {
	fwrite(&Value, sizeof(int), 1, fptr);
}

void WriteLong(FILE* fptr, long Value) {
	fwrite(&Value, sizeof(long), 1, fptr);
}

int FindAbilityIndex(int index, edict_t* player) {
	int i;
	int count = 0;
	for (i = 0; i < MAX_ABILITIES; ++i) {
		if (!player->myskills.abilities[i].disable) {
			++count;
			if (count == index)
				return i;
		}
	}
	return -1;	//just in case something messes up
}


/*
	_beginthread creates threads that automatically terminate when they are finished.
	This is according to the online documentation. If memory leaks are a problem, 
	try using _endthread()

	The best part of these thread calls is that windows.h is not required.

	syntax:	_beginthread(func, stacksize, args)
	func		= thread function. Must have __cdecl identifier.
	stacksize	= size of stack to use. Ususally set to the default value (0).
	args		= void pointer to any arguments you want to send the thread.

	syntax: _endthread()
*/

//Thread function for loading players
void __cdecl OpenPlayerThread(void *arg)
{
	edict_t *ent = (edict_t *)arg;

//	if(debuginfo->value)
		gi.dprintf(va("Threading OpenPlayerThread() for %s.\n", ent->client->pers.netname));

//	memset(&ent->myskills,0,sizeof(skills_t));
	if(openPlayer(ent))
	{
		gi.cprintf(ent, PRINT_HIGH, "File found! Entering the game.\n");
        ent->threadReturnVal = 0;		//success
	}
	else	ent->threadReturnVal = -1;	//failure

	ent->hThreadFinishTime = level.time;
}

//Thread function for saving players
void __cdecl SavePlayerThread(void *arg)
{
	edict_t *ent = (edict_t *)arg;

//	if(debuginfo->value)
		gi.dprintf(va("Threading SavePlayerThread() for %s.\n", ent->client->pers.netname));

	if(SavePlayer(ent))
		ent->threadReturnVal = 0;		//success
	else	ent->threadReturnVal = -1;	//failure

	ent->hThreadFinishTime = level.time;
}
#endif

//Creates an OpenPlayerThread() for loading a player (GDS)
void createOpenPlayerThread(edict_t *ent)
{
#if defined(_WIN32)
	ent->isLoading = true;
	ent->hThread = _beginthread(OpenPlayerThread, 0, ent);

	// Check the return value for success.
	if (ent->hThread == 0)
		gi.dprintf("ERROR: Thread not created.\n");

#else
	gi.dprintf("Multi-threading not supported for non-windows builds.\n");
#endif
} 

//Creates an SavePlayerThread() for saving a player (GDS)
void createSavePlayerThread(edict_t *ent)
{
#if defined(_WIN32)
	ent->isSaving = true;
	ent->hThread = _beginthread(SavePlayerThread, 0, ent);

	// Check the return value for success.
	if (ent->hThread == 0)
		gi.dprintf("ERROR: Thread not created.\n");
	
#else
	gi.dprintf("Multi-threading not supported for non-windows builds.\n");
#endif
} 

//Encrypt the player's password
char *CryptPassword(char *text)
{
	int i;

	if (!text)
		return NULL;
	for (i=0; i<strlen(text) ; i++)
		if ((byte)text[i] > 127)
			text[i]=(byte)text[i]-128;
		else
			text[i]=(byte)text[i]+128;

	return text;
}

//***********************************************************************
//	Load player v 1.0
//***********************************************************************
qboolean ReadPlayer_v1(FILE * fRead, edict_t *player) {
//	int numAbilities, numWeapons, numRunes;
//	int i;

    ReadString(player->myskills.title, fRead);					//player's title
	ReadString(player->myskills.player_name, fRead);			//player's in-game name
	ReadString(player->myskills.password, fRead);				//password
	ReadString(player->myskills.email, fRead);					//email address
	ReadString(player->myskills.owner, fRead);					//owner
	ReadString(player->myskills.member_since, fRead);			//creation date
	ReadString(player->myskills.last_played, fRead);			//last played date
	player->myskills.total_playtime =  ReadInteger(fRead);		//playing time total
	player->myskills.playingtime =  ReadInteger(fRead);			//playing time today

	// For AQ2 stats -JukS (15.03.2021)
	player->client->resp.totalscore = ReadInteger(fRead);
	player->client->resp.totalkills = ReadInteger(fRead);
	player->client->resp.totaldeaths = ReadInteger(fRead);
	//	player->client->resp.totalctf_caps);
	// end -JukS



// ---------------- From Vortex. For later use? -JukS (15.03.2021) ==============================
/*
    //begin talents
	player->myskills.talents.count = ReadInteger(fRead);
	for (i = 0; i < player->myskills.talents.count; ++i)
	{
		//don't crash.
        if (i > MAX_TALENTS)
			return false;

		player->myskills.talents.talent[i].id = ReadInteger(fRead);
		player->myskills.talents.talent[i].upgradeLevel = ReadInteger(fRead);
		player->myskills.talents.talent[i].maxLevel = ReadInteger(fRead);
	}
	//end talents

	//begin abilities
	numAbilities = ReadInteger(fRead);
	for (i = 0; i < numAbilities; ++i)
	{
		int index;
		index = ReadInteger(fRead);

		if ((index >= 0) && (index < MAX_ABILITIES))
		{
			player->myskills.abilities[index].level			= ReadInteger(fRead);
			player->myskills.abilities[index].max_level		= ReadInteger(fRead);
			player->myskills.abilities[index].hard_max		= ReadInteger(fRead);
			player->myskills.abilities[index].modifier		= ReadInteger(fRead);
			player->myskills.abilities[index].disable		= (qboolean)ReadInteger(fRead);
			player->myskills.abilities[index].general_skill = (qboolean)ReadInteger(fRead);
		}
		else
		{
			gi.dprintf("Error loading player: %s. Ability index not loaded correctly!\n", player->client->pers.netname);
			WriteToLogfile(player, "ERROR during loading: Ability index not loaded correctly!");
			return false;
		}
	}
	//end abilities

	//begin weapons
    numWeapons = ReadInteger(fRead);
	for (i = 0; i < numWeapons; ++i)
	{
		int index;
		index = ReadInteger(fRead);

		if ((index >= 0 ) && (index < MAX_WEAPONS))
		{
			int j;
			player->myskills.weapons[index].disable = ReadInteger(fRead);

			for (j = 0; j < MAX_WEAPONMODS; ++j)
			{
				player->myskills.weapons[index].mods[j].level = ReadInteger(fRead);
				player->myskills.weapons[index].mods[j].soft_max = ReadInteger(fRead);
				player->myskills.weapons[index].mods[j].hard_max = ReadInteger(fRead);
			}
		}
		else
		{
			gi.dprintf("Error loading player: %s. Weapon index not loaded correctly!\n", player->myskills.player_name);
			WriteToLogfile(player, "ERROR during loading: Weapon index not loaded correctly!");
			return false;
		}
	}
	//end weapons

	//begin runes
	numRunes = ReadInteger(fRead);
	for (i = 0; i < numRunes; ++i)
	{
		int index;
		index = ReadInteger(fRead);
		if ((index >= 0) && (index < MAX_VRXITEMS))
		{
			int j;
			player->myskills.items[index].itemtype = ReadInteger(fRead);
			player->myskills.items[index].itemLevel = ReadInteger(fRead);
			player->myskills.items[index].quantity = ReadInteger(fRead);
			player->myskills.items[index].untradeable = ReadInteger(fRead);
			ReadString(player->myskills.items[index].id, fRead);
			ReadString(player->myskills.items[index].name, fRead);
			player->myskills.items[index].numMods = ReadInteger(fRead);
			player->myskills.items[index].setCode = ReadInteger(fRead);
			player->myskills.items[index].classNum = ReadInteger(fRead);

			for (j = 0; j < MAX_VRXITEMMODS; ++j)
			{
				player->myskills.items[index].modifiers[j].type = ReadInteger(fRead);
				player->myskills.items[index].modifiers[j].index = ReadInteger(fRead);
				player->myskills.items[index].modifiers[j].value = ReadInteger(fRead);
				player->myskills.items[index].modifiers[j].set = ReadInteger(fRead);
			}
		}
	}
	//end runes


	//*****************************
	//standard stats
	//*****************************

	//Exp
	player->myskills.experience =  ReadLong(fRead);
	//next_level
	player->myskills.next_level =  ReadLong(fRead);
	//Level
	player->myskills.level =  ReadInteger(fRead);
	//Class number
	player->myskills.class_num =  ReadInteger(fRead);
	//skill points
	player->myskills.speciality_points =  ReadInteger(fRead);
	//credits
	player->myskills.credits =  ReadInteger(fRead);
	//weapon points
	player->myskills.weapon_points =  ReadInteger(fRead);
	//respawn weapon
	player->myskills.respawn_weapon =  ReadInteger(fRead);
	//talent points
	player->myskills.talents.talentPoints =  ReadInteger(fRead);

	//*****************************
	//in-game stats
	//*****************************
	//respawns
	player->myskills.respawns =  ReadInteger(fRead);
	//health
	player->myskills.current_health =  ReadInteger(fRead);
	//max health
	player->myskills.max_health =  ReadInteger(fRead);
	//armour
	player->myskills.current_armor =  ReadInteger(fRead);
	//max armour
	player->myskills.max_armor =  ReadInteger(fRead);
	//nerfme			(cursing a player maybe?)
	player->myskills.nerfme =  ReadInteger(fRead);

	//*****************************
	//flags
	//*****************************
	//admin flag
	player->myskills.administrator =  ReadInteger(fRead);
	//boss flag
	player->myskills.boss =  ReadInteger(fRead);

	//*****************************
	//stats
	//*****************************
	//shots fired
	player->myskills.shots =  ReadInteger(fRead);
	//shots hit
	player->myskills.shots_hit =  ReadInteger(fRead);
	//frags
	player->myskills.frags =  ReadInteger(fRead);
	//deaths
	player->myskills.fragged =  ReadInteger(fRead);
	//number of sprees
	player->myskills.num_sprees =  ReadInteger(fRead);
	//max spree
	player->myskills.max_streak =  ReadInteger(fRead);
	//number of wars
	player->myskills.spree_wars =  ReadInteger(fRead);
	//number of sprees broken
	player->myskills.break_sprees =  ReadInteger(fRead);
	//number of wars broken
	player->myskills.break_spree_wars =  ReadInteger(fRead);
	//suicides
	player->myskills.suicides =  ReadInteger(fRead);
	//teleports			(link this to "use tballself" maybe?)
	player->myskills.teleports =  ReadInteger(fRead);
	//number of 2fers
	player->myskills.num_2fers =  ReadInteger(fRead);

	//CTF statistics
	player->myskills.flag_pickups =  ReadInteger(fRead);
	player->myskills.flag_captures =  ReadInteger(fRead);
	player->myskills.flag_returns =  ReadInteger(fRead);
	player->myskills.flag_kills =  ReadInteger(fRead);
	player->myskills.offense_kills =  ReadInteger(fRead);
	player->myskills.defense_kills =  ReadInteger(fRead);
	player->myskills.assists =  ReadInteger(fRead);
	//End CTF

	//standard iD inventory
	if (fread(player->myskills.inventory, sizeof(int), MAX_ITEMS, fRead))
		gi.dprintf("%s loaded inventory.\n", __func__);

	//Apply runes
//	V_ResetAllStats(player);
//	for (i = 0; i < 3; ++i)
//		V_ApplyRune(player, &player->myskills.items[i]);

	//Apply health
	if (player->myskills.current_health > MAX_HEALTH(player))
		player->myskills.current_health = MAX_HEALTH(player);

//	//Apply armor
	if (player->myskills.current_armor > MAX_ARMOR(player))
		player->myskills.current_armor = MAX_ARMOR(player);
//	player->myskills.inventory[body_armor_index] = player->myskills.current_armor;

	//done
// ---------------- END: From Vortex. For later use? -JukS (15.03.2021) ==============================
*/
	return true;
}

//***********************************************************************
//	Save player v 1.0
//***********************************************************************
void WritePlayer_v1(FILE * fWrite, char *playername, edict_t *player)
{
//	int i;
//	int numAbilities = CountAbilities(player);
//	int numWeapons = CountWeapons(player);
//	int numRunes = CountRunes(player);

	//save header
	WriteString(fWrite, "ActionRPG Player File v1.0");			//player's title
	WriteString(fWrite, player->myskills.title);				//player's in-game name
	WriteString(fWrite, playername);
	WriteString(fWrite, player->myskills.password);				//password
	WriteString(fWrite, player->myskills.email);				//email address
	WriteString(fWrite, player->myskills.owner);				//owner
	WriteString(fWrite, player->myskills.member_since);			//creation date
	WriteString(fWrite, player->myskills.last_played);			//last played date
	WriteInteger(fWrite, player->myskills.total_playtime);		//playing time total
	WriteInteger(fWrite, player->myskills.playingtime);			//playing time today

	// For AQ2 stats -JukS (15.03.2021)
	WriteInteger(fWrite, player->client->resp.totalscore);
	WriteInteger(fWrite, player->client->resp.totalkills);
	WriteInteger(fWrite, player->client->resp.totaldeaths);
//	WriteInteger(fWrite, player->client->resp.totalctf_caps);
	// end -JukS

// ---------------- From Vortex. For later use? -JukS (15.03.2021) ==============================
/*
	//begin talents
	WriteInteger(fWrite, player->myskills.talents.count);
	for (i = 0; i < player->myskills.talents.count; ++i)
	{
		WriteInteger(fWrite, player->myskills.talents.talent[i].id);
		WriteInteger(fWrite, player->myskills.talents.talent[i].upgradeLevel);
		WriteInteger(fWrite, player->myskills.talents.talent[i].maxLevel);
	}
	//end talents

	//begin abilities
	WriteInteger(fWrite, numAbilities);
	for (i = 0; i < numAbilities; ++i)
	{
		int index = FindAbilityIndex(i+1, player);
		if (index != -1)
		{
			WriteInteger(fWrite, index);
			WriteInteger(fWrite, player->myskills.abilities[index].level);
			WriteInteger(fWrite, player->myskills.abilities[index].max_level);
			WriteInteger(fWrite, player->myskills.abilities[index].hard_max);
			WriteInteger(fWrite, player->myskills.abilities[index].modifier);
			WriteInteger(fWrite, (int)player->myskills.abilities[index].disable);
			WriteInteger(fWrite, (int)player->myskills.abilities[index].general_skill);
		}
	}
	//end abilities

	//begin weapons
    WriteInteger(fWrite, numWeapons);
	for (i = 0; i < numWeapons; ++i)
	{
		int index = FindWeaponIndex(i+1, player);
		if (index != -1)
		{
			int j;
			WriteInteger(fWrite, index);
			WriteInteger(fWrite, player->myskills.weapons[index].disable);

			for (j = 0; j < MAX_WEAPONMODS; ++j)
			{
				WriteInteger(fWrite, player->myskills.weapons[index].mods[j].level);
				WriteInteger(fWrite, player->myskills.weapons[index].mods[j].soft_max);
				WriteInteger(fWrite, player->myskills.weapons[index].mods[j].hard_max);
			}
		}
	}
	//end weapons

	//begin runes
    WriteInteger(fWrite, numRunes);
	for (i = 0; i < numRunes; ++i)
	{
		int index = FindRuneIndex(i+1, player);
		if (index != -1)
		{
			int j;
			WriteInteger(fWrite, index);
			WriteInteger(fWrite, player->myskills.items[index].itemtype);
			WriteInteger(fWrite, player->myskills.items[index].itemLevel);
			WriteInteger(fWrite, player->myskills.items[index].quantity);
			WriteInteger(fWrite, player->myskills.items[index].untradeable);
			WriteString(fWrite, player->myskills.items[index].id);
			WriteString(fWrite, player->myskills.items[index].name);
			WriteInteger(fWrite, player->myskills.items[index].numMods);
			WriteInteger(fWrite, player->myskills.items[index].setCode);
			WriteInteger(fWrite, player->myskills.items[index].classNum);

			for (j = 0; j < MAX_VRXITEMMODS; ++j)
			{
				WriteInteger(fWrite, player->myskills.items[index].modifiers[j].type);
				WriteInteger(fWrite, player->myskills.items[index].modifiers[j].index);
				WriteInteger(fWrite, player->myskills.items[index].modifiers[j].value);
				WriteInteger(fWrite, player->myskills.items[index].modifiers[j].set);
			}
		}
	}
	//end runes

	//*****************************
	//standard stats
	//*****************************

	//Exp
	WriteLong(fWrite, player->myskills.experience);
	//next_level
	WriteLong(fWrite, player->myskills.next_level);
	//Level
	WriteInteger(fWrite, player->myskills.level);
	//Class number
	WriteInteger(fWrite, player->myskills.class_num);
	//skill points
	WriteInteger(fWrite, player->myskills.speciality_points);
	//credits
	WriteInteger(fWrite, player->myskills.credits);
	//weapon points
	WriteInteger(fWrite, player->myskills.weapon_points);
	//respawn weapon
	WriteInteger(fWrite, player->myskills.respawn_weapon);
	//talent points
	WriteInteger(fWrite, player->myskills.talents.talentPoints);

	//*****************************
	//in-game stats
	//*****************************
	//respawns
	WriteInteger(fWrite, player->myskills.respawns);
	//health
	WriteInteger(fWrite, player->myskills.current_health);
	//max health
	WriteInteger(fWrite, MAX_HEALTH(player));
	//armour
//	WriteInteger(fWrite, player->client->pers.inventory[body_armor_index]);
	//max armour
	WriteInteger(fWrite, MAX_ARMOR(player));
	//nerfme			(cursing a player maybe?)
	WriteInteger(fWrite, player->myskills.nerfme);

	//*****************************
	//flags
	//*****************************
	//admin flag
	WriteInteger(fWrite, player->myskills.administrator);
	//boss flag
	WriteInteger(fWrite, player->myskills.boss);

	//*****************************
	//stats
	//*****************************
	//shots fired
	WriteInteger(fWrite, player->myskills.shots);
	//shots hit
	WriteInteger(fWrite, player->myskills.shots_hit);
	//frags
	WriteInteger(fWrite, player->myskills.frags);
	//deaths
	WriteInteger(fWrite, player->myskills.fragged);
	//number of sprees
	WriteInteger(fWrite, player->myskills.num_sprees);
	//max spree
	WriteInteger(fWrite, player->myskills.max_streak);
	//number of wars
	WriteInteger(fWrite, player->myskills.spree_wars);
	//number of sprees broken
	WriteInteger(fWrite, player->myskills.break_sprees);
	//number of wars broken
	WriteInteger(fWrite, player->myskills.break_spree_wars);
	//suicides
	WriteInteger(fWrite, player->myskills.suicides);
	//teleports			(link this to "use tball self" maybe?)
	WriteInteger(fWrite, player->myskills.teleports);
	//number of 2fers
	WriteInteger(fWrite, player->myskills.num_2fers);

	//CTF statistics
	WriteInteger(fWrite, player->myskills.flag_pickups);
	WriteInteger(fWrite, player->myskills.flag_captures);
	WriteInteger(fWrite, player->myskills.flag_returns);
	WriteInteger(fWrite, player->myskills.flag_kills);
	WriteInteger(fWrite, player->myskills.offense_kills);
	WriteInteger(fWrite, player->myskills.defense_kills);
	WriteInteger(fWrite, player->myskills.assists);
	//End CTF
// No need in AQ2 -JukS (15.03.2021)
//	//Don't let the player have > max cubes
//	if (player->client->pers.inventory[power_cube_index] > player->client->pers.max_powercubes)
//		player->client->pers.inventory[power_cube_index] = player->client->pers.max_powercubes;

	//standard iD inventory
	fwrite(player->client->pers.inventory, sizeof(int), MAX_ITEMS, fWrite);
// ---------------- END: From Vortex. For later use? -JukS (15.03.2021) ==============================
*/
}

//***********************************************************************
//		Basic file I/O functions
//***********************************************************************

//Returns the number of lines in a text file
int textFile_CountLines(FILE *fptr, long size)
{
	int count = 0;
	char temp;
	int i = 0;
	
	do
	{
		temp = getc(fptr);
		if (temp == '\n')
			count++;
	}
	while (++i < size);

	rewind(fptr);
	return count;
}

//***********************************************************************

//Iterates to a specific line in a text file (or EOF)
void textFile_GotoLine(FILE *fptr, int linenumber, long size)
{
	int count = 0;
	char temp;
	int i = 0;
	
	do
	{
		temp = fgetc(fptr);
		if (temp == '\n')
			count++;
		if (count == linenumber)
			return;
	}
	while (++i < size);
		return;
}

//************************************************

//***********************************************************************
//		Utility functions
//***********************************************************************

//************************************************

int CountAbilities(edict_t *player)
{
	int i;
	int count = 0;
	for (i = 0; i < MAX_ABILITIES; ++i)
	{
		if (!player->myskills.abilities[i].disable)
			++count;
	}
	return count;
}

//************************************************
//************************************************

int CountWeapons(edict_t *player) {
	int i;
	int count = 0;
	for (i = 0; i < MAX_WEAPONS; ++i) {
//		if (V_WeaponUpgradeVal(player, i) > 0)
			count++;
	}
	return count;
}

//************************************************

int FindWeaponIndex(int index, edict_t *player) {
	int i;
	int count = 0;
	for (i = 0; i < MAX_WEAPONS; ++i) {
//		if (V_WeaponUpgradeVal(player, i) > 0) {
			count++;
			if (count == index)
				return i;
//		}
	}
	return -1;	//just in case something messes up
}

//************************************************
//************************************************

int CountRunes(edict_t *player) {
	int count = 0;
	int i;

	for (i = 0; i < MAX_VRXITEMS; ++i) {
		if (player->myskills.items[i].itemtype != NO_NUM)
			++count;
	}
	return count;
}

//************************************************

int FindRuneIndex(int index, edict_t *player) {
	int i;
	int count = 0;

	for (i = 0; i < MAX_VRXITEMS; ++i) {
		if (player->myskills.items[i].itemtype != NO_NUM) {
			++count;
			if (count == index)
				return i;
		}
	}
	return -1;	//just in case something messes up
}

//************************************************
//************************************************

//***********************************************************************
//		Save player to file
//		Max value of a signed int32 (4 bytes) = 2147483648
//		That should be plenty for vrx	-doomie
//***********************************************************************

/*
char* V_FormatFileName(char* name) {
	char filename[64];
	char buffer[64];
	int i, j = 0;

	//This bit of code formats invalid filename chars, like the '?' and 
	//reformats them into a saveable format.
	Q_strncpy(buffer, name, sizeof(buffer) - 1);

	for (i = 0; i < strlen(buffer); ++i) {
		char tmp;
		switch (buffer[i])
		{
		case '\\':		tmp = '1';		break;
		case '/':		tmp = '2';		break;
		case '?':		tmp = '3';		break;
		case '|':		tmp = '4';		break;
		case '"':		tmp = '5';		break;
		case ':':		tmp = '6';		break;
		case '*':		tmp = '7';		break;
		case '<':		tmp = '8';		break;
		case '>':		tmp = '9';		break;
		case '_':		tmp = '_';		break;
		default:		tmp = '0';		break;
		}

		if (tmp != '0') {
			filename[j++] = '_';
			filename[j++] = tmp;
		}
		else
			filename[j++] = buffer[i];
	}

	//make sure string is null-terminated
	filename[j] = 0;

	return va("%s", filename);
}
*/

//Called by savePlayer(). Don't call this directly.
qboolean SavePlayer(edict_t *ent)
{
	char	path[100];
	FILE	*fwrite;

	//Make sure this is a client
	if (!ent->client)
	{
		gi.dprintf("ERROR: entity not a client!! (%s)\n",ent->classname);
		return false;
	}

//	if(debuginfo->value)
		gi.dprintf("savePlayer called to save: %s\n", ent->client->pers.netname);

	//determine path
		Com_sprintf(path, sizeof path, "%s/%s.dat", save_path->string, V_FormatFileName(ent->client->pers.netname));

	//Open file for saving
	if ((fwrite = fopen(path, "wb")) == NULL)
	{
        gi.dprintf("ERROR: savePlayer() can't open %s.\n", path);
		return false;		
	}
	
	//save the player
	WritePlayer_v1(fwrite, ent->client->pers.netname, ent);	

	//done
	fclose(fwrite);
	return true;
}

//***********************************************************************
//		Save player (via multi-threading or via local access)
//***********************************************************************

qboolean savePlayer(edict_t *ent) {
//	if(gds->value) {
//		//Don't create more than one save thread.
//		if(!ent->isSaving)
//			createSavePlayerThread(ent);
//		return true;
//	}
	return SavePlayer(ent);
}

//***********************************************************************
//		open player from file
//***********************************************************************

qboolean openPlayer(edict_t *ent) {
	char	path[100];
	FILE	*fread;
	int		i;
	char	version[64];


	if (!ent->client) {		//Make sure this is a client
		gi.dprintf("ERROR: entity not a client!! (%s)\n",ent->classname);
		return false;
	}

//	if(debuginfo->value)
		gi.dprintf("openPlayer called to open: %s\n", ent->client->pers.netname);
	
	memset(&ent->myskills,0,sizeof(skills_t));		//Reset the player's skills_t
	Com_sprintf(path, sizeof path, "%s/%s.dat", save_path->string, V_FormatFileName(ent->client->pers.netname));	//determine path

	
	if ((fread = fopen(path, "rb")) == NULL) {		//Open file for loading
		gi.dprintf("INFO: openPlayer can't open %s. This probably means the file does not exist.\n", path);
		return false;		
	}	

	
	for (i = 0; i < MAX_ABILITIES; ++i) {			//disable all abilities
		ent->myskills.abilities[i].disable = true;
	}

	ReadString(version, fread);						//read header (player file version number)

	//check for correct version number
	if (Q_stricmp(version + 19, "v1.0") == 0) {		//"Vortex Player File " is 19 chars long, start comparison at the end
		if (ReadPlayer_v1(fread, ent) == false) {		//begin reading player, if there was an error, return false :)
			fclose(fread);
			return false;
		}
	}
	else { // <-- put newer loading code here
		//bad file version/type
		fclose(fread);
		return false;
	}
		
	//done
	fclose(fread);
	return true;
}

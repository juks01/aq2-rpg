#include "g_local.h"

int MAX_BULLETS(edict_t* ent) {
	if (ent->myskills.abilities[MAX_AMMO].disable)
		return 0;
	return (100 * ent->myskills.abilities[MAX_AMMO].current_level);
}

int MAX_SHELLS(edict_t* ent) {
	if (ent->myskills.abilities[MAX_AMMO].disable)
		return 0;
	return (50 * ent->myskills.abilities[MAX_AMMO].current_level);
}

int MAX_ROCKETS(edict_t* ent) {
	if (ent->myskills.abilities[MAX_AMMO].disable)
		return 0;
	return (25 * ent->myskills.abilities[MAX_AMMO].current_level);
}

int MAX_GRENADES(edict_t* ent) {
	if (ent->myskills.abilities[MAX_AMMO].disable)
		return 0;
	return (25 * ent->myskills.abilities[MAX_AMMO].current_level);
}

int MAX_CELLS(edict_t* ent) {
	if (ent->myskills.abilities[MAX_AMMO].disable)
		return 0;
	return (100 * ent->myskills.abilities[MAX_AMMO].current_level);
}

int MAX_SLUGS(edict_t* ent) {
	if (ent->myskills.abilities[MAX_AMMO].disable)
		return 0;
	return (25 * ent->myskills.abilities[MAX_AMMO].current_level);
}

void modify_max(edict_t* ent)
{
	ent->max_health = MAX_HEALTH(ent);
	ent->client->pers.max_health = ent->max_health;

	ent->client->pers.max_bullets = 200 + MAX_BULLETS(ent);
	ent->client->pers.max_shells = 100 + MAX_SHELLS(ent);
	ent->client->pers.max_rockets = 50 + MAX_ROCKETS(ent);
	ent->client->pers.max_grenades = 50 + MAX_GRENADES(ent);
	ent->client->pers.max_cells = 200 + MAX_CELLS(ent);
	ent->client->pers.max_slugs = 50 + MAX_SLUGS(ent);

//	ent->client->pers.max_powercubes = MAX_POWERCUBES(ent);

}

void modify_health(edict_t* ent)
{
	ent->health = MAX_HEALTH(ent);
	ent->client->pers.health = ent->health;
}

int total_players(void) {
	int		i, total = 0;
	edict_t* cl_ent;

	for (i = 0; i < game.maxclients; i++) {
		cl_ent = g_edicts + 1 + i;
		if (!cl_ent->inuse)
			continue;
		if (G_IsSpectator(cl_ent))
			continue;
		total++;
	}
	return total;
}

void WriteServerMsg(char* s, char* error_string, qboolean print_msg, qboolean save_to_logfile) {
	cvar_t* port;
	char	buf[512];
	char	path[256];
	const char *s1, *s2;
	FILE* fptr;

	if (!s || !error_string || !save_to_logfile)
		return;

	// create the log message 
	Com_sprintf(buf, sizeof buf, "%s %s %s: %s", CURRENT_DATE, CURRENT_TIME, error_string, s);
	if (print_msg)
		gi.dprintf("* %s *\n", buf);

	port = gi.cvar("port", "0", CVAR_SERVERINFO);

	//determine path  
	Com_sprintf(path, sizeof path, "%s/%d.log", game_path->string, (int)port->value);

	if ((fptr = fopen(path, "a")) != NULL) // append text to log  
	{
		//3.0 make sure there is a line feed  
		if (buf[strlen(buf) - 1] != '\n')
			strcat(buf, "\n");

		fprintf(fptr, "%s", buf);
		fclose(fptr);
		return;
	}
	gi.dprintf("ERROR: Failed to write to server log.\n");
}


char* V_FormatFileName(char* name) {
	char filename[64];
	char buffer[64];
	int i, j = 0;

	//This bit of code formats invalid filename chars, like the '?' and 
	//reformats them into a saveable format.
	Q_strncpy(buffer, name, sizeof(buffer) - 1);

	for (i = 0; i < strlen(buffer); ++i)
	{
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

		if (tmp != '0')
		{
			filename[j++] = '_';
			filename[j++] = tmp;
		}
		else
		{
			filename[j++] = buffer[i];
		}
	}

	//make sure string is null-terminated
	filename[j] = 0;

	return va("%s", filename);
}

void WriteToLogfile(edict_t* ent, char* s)
{
	char* ip, buf[512];
	char     path[256];
	const char* s1, * s2;
	FILE* fptr;

	if (!s || !ent || !ent->client)
		return;

	if (strlen(ent->client->pers.netname) < 1)
		return;

	//Create the log message  
	ip = Info_ValueForKey(ent->client->pers.userinfo, "ip");
	Com_sprintf(buf, sizeof buf, "%s %s [%s]: %s", CURRENT_DATE, CURRENT_TIME, ip, s);

	//determine path  
	Com_sprintf(path, sizeof path, "%s/%s.log", save_path->string, V_FormatFileName(ent->client->pers.netname));

	if ((fptr = fopen(path, "a")) != NULL) // append text to log
	{
		//3.0 make sure there is a line feed
		if (buf[strlen(buf) - 1] != '\n')
			strcat(buf, "\n");

		fprintf(fptr, "%s", buf);
		fclose(fptr);
		return;
	}
	gi.dprintf("ERROR: Failed to write to player log.\n");
}
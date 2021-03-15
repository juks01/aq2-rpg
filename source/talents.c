#include "g_local.h"

//Gives the player a new talent.
void addTalent(edict_t *ent, int talentID, int maxLevel)
{
	int nextEmptySlot = ent->myskills.talents.count;
	int i = 0;

	//Don't add too many talents.
	if(!(nextEmptySlot < MAX_TALENTS))
		return;

	//Don't add a talent more than once.
	for(i = 0; i < nextEmptySlot; ++i)
		if(ent->myskills.talents.talent[nextEmptySlot].id == talentID)
			return;

	ent->myskills.talents.talent[nextEmptySlot].id = talentID;
	ent->myskills.talents.talent[nextEmptySlot].maxLevel = maxLevel;
	ent->myskills.talents.talent[nextEmptySlot].upgradeLevel = 0;		//Just in case it's not already zero.

	ent->myskills.talents.count++;
}

//Adds all the required talents for said class.
void setTalents(edict_t *ent)
{
	switch(ent->myskills.class_num)
	{
		case CLASS_SOLDIER:
			addTalent(ent, TALENT_IMP_STRENGTH, 5);
			addTalent(ent, TALENT_IMP_RESIST, 5);
			addTalent(ent, TALENT_BLOOD_OF_ARES, 5);
			addTalent(ent, TALENT_BASIC_HA, 5);
			addTalent(ent, TALENT_BOMBARDIER, 5);
			return;
		case CLASS_POLTERGEIST:
			addTalent(ent, TALENT_MORPHING, 5);
			addTalent(ent, TALENT_MORE_AMMO, 5);
			addTalent(ent, TALENT_SUPERIORITY, 5);
			addTalent(ent, TALENT_RETALIATION, 5);
			addTalent(ent, TALENT_PACK_ANIMAL, 5);
			return;
		case CLASS_VAMPIRE:
			addTalent(ent, TALENT_IMP_CLOAK, 4);
			addTalent(ent, TALENT_ARMOR_VAMP, 3);
			addTalent(ent, TALENT_SECOND_CHANCE, 4);
			addTalent(ent, TALENT_IMP_MINDABSORB, 4);
			addTalent(ent, TALENT_CANNIBALISM, 5);
			addTalent(ent, TALENT_FATAL_WOUND, 5);
			return;
		case CLASS_MAGE:
			addTalent(ent, TALENT_ICE_BOLT, 5);
			addTalent(ent, TALENT_FROST_NOVA, 5);
			addTalent(ent, TALENT_IMP_MAGICBOLT, 5);
			addTalent(ent, TALENT_MANASHIELD, 5);
			addTalent(ent, TALENT_MEDITATION, 5);
			addTalent(ent, TALENT_OVERLOAD, 5);
			return;
		case CLASS_ENGINEER:
			addTalent(ent, TALENT_LASER_PLATFORM, 5);
            addTalent(ent, TALENT_ALARM, 5);
			addTalent(ent, TALENT_RAPID_ASSEMBLY, 5);
			addTalent(ent, TALENT_PRECISION_TUNING, 5);
			addTalent(ent, TALENT_STORAGE_UPGRADE, 5);
			return;
		case CLASS_KNIGHT:
			addTalent(ent, TALENT_REPEL, 5);
			addTalent(ent, TALENT_MAG_BOOTS, 5);
			addTalent(ent, TALENT_LEAP_ATTACK, 5);
			addTalent(ent, TALENT_MOBILITY, 5);
			addTalent(ent, TALENT_DURABILITY, 5);
			return;
		case CLASS_CLERIC:
			addTalent(ent, TALENT_BALANCESPIRIT, 5);
			addTalent(ent, TALENT_HOLY_GROUND, 5);
			addTalent(ent, TALENT_UNHOLY_GROUND, 5);
			addTalent(ent, TALENT_BOOMERANG, 5);
			addTalent(ent, TALENT_PURGE, 5);
			return;
		case CLASS_WEAPONMASTER:
			addTalent(ent, TALENT_BASIC_AMMO_REGEN, 5);
			addTalent(ent, TALENT_COMBAT_EXP, 5);
			addTalent(ent, TALENT_TACTICS, 3);
			addTalent(ent, TALENT_SIDEARMS, 3);
			return;
		case CLASS_NECROMANCER:
			addTalent(ent, TALENT_CHEAPER_CURSES, 5);
			addTalent(ent, TALENT_CORPULENCE, 5);
			addTalent(ent, TALENT_LIFE_TAP, 5);
			addTalent(ent, TALENT_DIM_VISION, 5);
			addTalent(ent, TALENT_EVIL_CURSE, 5);
			addTalent(ent, TALENT_FLIGHT, 5);
			return;
		case CLASS_SHAMAN:
			addTalent(ent, TALENT_ICE, 4);
			addTalent(ent, TALENT_WIND, 4);
			addTalent(ent, TALENT_STONE, 4);
			addTalent(ent, TALENT_SHADOW, 4);
			addTalent(ent, TALENT_PEACE, 4);
			addTalent(ent, TALENT_TOTEM, 6);
			addTalent(ent, TALENT_VOLCANIC, 5);
			return;
		case CLASS_ALIEN:
			addTalent(ent, TALENT_PHANTOM_OBSTACLE, 5);
			addTalent(ent, TALENT_SUPER_HEALER, 5);
			addTalent(ent, TALENT_PHANTOM_COCOON, 5);
			addTalent(ent, TALENT_SWARMING, 5);
			addTalent(ent, TALENT_EXPLODING_BODIES, 5);
			return;
	default: return;
	}
}

//Erases all talent information.
void eraseTalents(edict_t *ent)
{
	memset(&ent->myskills.talents, 0, sizeof(talentlist_t));
}

//Returns the talent slot with matching talentID.
//Returns -1 if there is no matching talent.
int getTalentSlot(edict_t *ent, int talentID)
{
	int i;

	//Make sure the ent is valid
	if(!ent)
	{
//		WriteServerMsg(va("getTalentSlot() called with a NULL entity. talentID = %d", talentID), "CRITICAL ERROR", true, true);
		return -1;
	}

	//Make sure we are a player
	if(!ent->client)
	{
		//gi.dprintf(va("WARNING: getTalentSlot() called with a non-player entity! talentID = %d\n", talentID));
		return -1;
	}

	for(i = 0; i < ent->myskills.talents.count; ++i)
		if(ent->myskills.talents.talent[i].id == talentID)
			return i;
	return -1;
}

//Returns the talent upgrade level matching talentID.
//Returns -1 if there is no matching talent.
int getTalentLevel(edict_t *ent, int talentID)
{
	int slot = getTalentSlot(ent, talentID);
	
	if(slot < 0)	return 0;//-1;

	return ent->myskills.talents.talent[slot].upgradeLevel;	
}

//Upgrades the talent with a matching talentID
void upgradeTalent(edict_t *ent, int talentID)
{
	int slot = getTalentSlot(ent, talentID);
	talent_t *talent;

	if(slot == -1)
		return;

	talent = &ent->myskills.talents.talent[slot];

	// check for conflicting talents
	if (talentID == TALENT_RAPID_ASSEMBLY && getTalentLevel(ent, TALENT_PRECISION_TUNING) > 0)
	{
		gi.cprintf(ent, PRINT_HIGH, "Rapid Assembly can't be combined with Precision Tuning.\n");
		return;
	}
	if (talentID == TALENT_PRECISION_TUNING && getTalentLevel(ent, TALENT_RAPID_ASSEMBLY) > 0)
	{
		gi.cprintf(ent, PRINT_HIGH, "Precision Tuning can't be combined with Rapid Assembly.\n");
		return;
	}
	if (talentID == TALENT_CORPULENCE && getTalentLevel(ent, TALENT_LIFE_TAP) > 0)
	{
		gi.cprintf(ent, PRINT_HIGH, "Corpulence can't be combined with Life Tap.\n");
		return;
	}
	if (talentID == TALENT_LIFE_TAP && getTalentLevel(ent, TALENT_CORPULENCE) > 0)
	{
		gi.cprintf(ent, PRINT_HIGH, "Life Tap can't be combined with Corpulence.\n");
		return;
	}

	if(talent->upgradeLevel == talent->maxLevel)
	{
		gi.cprintf(ent, PRINT_HIGH, "You can not upgrade this talent any further.\n");
		return;
	}
	if(ent->myskills.talents.talentPoints < 1)
	{
		gi.cprintf(ent, PRINT_HIGH, "You do not have enough talent points.\n");
		return;
	}

    //We can upgrade.
	talent->upgradeLevel++;
	ent->myskills.talents.talentPoints--;
//	gi.cprintf(ent, PRINT_HIGH, va("%s upgraded to level %d/%d.\n", GetTalentString(talent->id), talent->upgradeLevel, talent->maxLevel));
	gi.cprintf(ent, PRINT_HIGH, va("Talent points remaining: %d\n", ent->myskills.talents.talentPoints));
	savePlayer(ent);
}

//****************************************
//************* Talent Menus *************
//****************************************

void TalentUpgradeMenu_handler(edict_t *ent, int option)
{
	//Not upgrading
	if(option > 0) {
//		OpenTalentUpgradeMenu(ent, getTalentSlot(ent, option-1)+1);
	}
	else {	//upgrading
		upgradeTalent(ent, (option * -1)-1);
	}
}

/* Thread-safe wrapper for gi.TagMalloc */
void* V_Malloc(size_t Size, int Tag)
{
	void* Memory;
/*
#if (!defined GDS_NOMULTITHREADING) && (!defined NO_GDS)
	pthread_mutex_lock(&MemMutex_Malloc);
#endif
*/
	Memory = gi.TagMalloc(Size, Tag);
/*
#if (!defined GDS_NOMULTITHREADING) && (!defined NO_GDS)
	pthread_mutex_unlock(&MemMutex_Malloc);
#endif
*/
	return Memory;
}


void setmenuhandler(edict_t* ent, void (*optionselected)(edict_t* ent, int option)) {
	ent->client->menustorage.optionselected = optionselected;
}

void addlinetomenu(edict_t* ent, char* line, int option) {
	size_t size;

	if (ent->client->menustorage.menu_active) // checks to see if the menu is showing
		return;
	if (ent->client->menustorage.num_of_lines >= MAX_LINES) // checks to see if there is space
		return;

	size = strlen(line);
	if (size > MENU_MAX_LINE_LEN)
		gi.dprintf("In %s, line \"%s\" length (%i) exceeds %i characters.\n", __func__, line, size, MENU_MAX_LINE_LEN);

	ent->client->menustorage.num_of_lines++; // adds to the number of lines that can be seen
	ent->client->menustorage.messages[ent->client->menustorage.num_of_lines].msg = V_Malloc(MENU_MAX_LINE_LEN, TAG_GAME);
	Q_strncpy(ent->client->menustorage.messages[ent->client->menustorage.num_of_lines].msg, line, MENU_MAX_LINE_LEN - 1);
	ent->client->menustorage.messages[ent->client->menustorage.num_of_lines].option = option;
}

void showmenu(edict_t* ent)
{
	int		i, j;  // general purpose integer for temporary use :)
	char	finalmenu[1024]; // where the control strings for the menu are assembled.
	char	tmp[80]; // temporary storage strings
	int		center;

	//if (debuginfo->value)
		gi.dprintf("DEBUG: showmenu()\n");
	if ((ent->client->menustorage.num_of_lines < 1) || (ent->client->menustorage.num_of_lines > MAX_LINES))
	{
		gi.dprintf("WARNING: showmenu() called with %d lines\n", ent->client->menustorage.num_of_lines);
		return;
	}

	// copy menu bg control strings to our final menu
	Com_sprintf(finalmenu, sizeof finalmenu, "xv 32 yv 8 picn inventory ");
	// get y coord of text based on the number of lines we want to create
	// this keeps the text vertically centered on our screen
	j = 24 + LINE_SPACING * (ceil((float)(20 - ent->client->menustorage.num_of_lines) / 2));
	// cycle through all lines and add their control codes to our final menu
	// nothing is actually displayed until the very end
	for (i = 1; i < (ent->client->menustorage.num_of_lines + 1); i++)
	{
		// get x coord of screen based on the length of the string for
		// text that should be centered
		center = 216 / 2 - strlen(ent->client->menustorage.messages[i].msg) * 4 + 52;
		if (ent->client->menustorage.messages[i].option == 0)							// print white text
			Com_sprintf(tmp, sizeof tmp, "xv 52 yv %i string \"%s\" ", j, ent->client->menustorage.messages[i].msg);
		else if (ent->client->menustorage.messages[i].option == MENU_GREEN_LEFT)		// print green text
			Com_sprintf(tmp, sizeof tmp, "xv 52 yv %i string2 \"%s\" ", j, ent->client->menustorage.messages[i].msg);
		else if (ent->client->menustorage.messages[i].option == MENU_GREEN_CENTERED)	// print centered green text
			Com_sprintf(tmp, sizeof tmp, "xv %d yv %i string2 \"%s\" ", center, j, ent->client->menustorage.messages[i].msg);
		else if (ent->client->menustorage.messages[i].option == MENU_WHITE_CENTERED)	// print centered white text
			Com_sprintf(tmp, sizeof tmp, "xv %d yv %i string \"%s\" ", center, j, ent->client->menustorage.messages[i].msg);
		else if (ent->client->menustorage.messages[i].option == MENU_GREEN_RIGHT) {		// print right-aligned green text
			center = 216 - strlen(ent->client->menustorage.messages[i].msg) * 8 + 52;
			Com_sprintf(tmp, sizeof tmp, "xv %d yv %i string2 \"%s\" ", center, j, ent->client->menustorage.messages[i].msg);
		}
		else if (i == ent->client->menustorage.currentline)
			Com_sprintf(tmp, sizeof tmp, "xv 52 yv %i string2 \">> %s\" ", j, ent->client->menustorage.messages[i].msg);
		else
			Com_sprintf(tmp, sizeof tmp, "xv 52 yv %i string \"   %s\" ", j, ent->client->menustorage.messages[i].msg);
		// add the control string to our final menu space
		strcat(finalmenu, tmp);
		j += LINE_SPACING;
	}

	// actually display the final menu
	ent->client->menustorage.menu_active = true;
	ent->client->menustorage.displaymsg = false;
	ent->client->showinventory = false;
	ent->client->showscores = true;
	gi.WriteByte(svc_layout);
	gi.WriteString(finalmenu);
	gi.unicast(ent, true);
}




//Adds menu lines that describe the general use of the talent.
int writeTalentDescription(edict_t *ent, int talentID)
{
	switch(talentID)
	{
	//Soldier talents
	case TALENT_IMP_STRENGTH:
		addlinetomenu(ent, "Increases damage,", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "but reduces resist.", MENU_WHITE_CENTERED);
		return 2;
	case TALENT_IMP_RESIST:
		addlinetomenu(ent, "Increases resist,", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "but reduces damage.", MENU_WHITE_CENTERED);
		return 2;
	case TALENT_BLOOD_OF_ARES:
		addlinetomenu(ent, "Increases the damage you", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "give/take as you spree.", MENU_WHITE_CENTERED);
		return 2;
	case TALENT_BASIC_HA:
		addlinetomenu(ent, "Increases ammo pickups.", MENU_WHITE_CENTERED);
		return 1;
	case TALENT_BOMBARDIER:
		addlinetomenu(ent, "Reduces self-inflicted", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "grenade damage and", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "reduces cost.", MENU_WHITE_CENTERED);
		return 3;
	//Poltergeist talents
    case TALENT_MORPHING:
		addlinetomenu(ent, "Reduces the cost", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "of your morphs.", MENU_WHITE_CENTERED);
		return 2;
	case TALENT_MORE_AMMO:
		addlinetomenu(ent, "Increases maximum ammo", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "capacity for", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "tank/caco/flyer/medic.", MENU_WHITE_CENTERED);
		return 3;
	case TALENT_SUPERIORITY:
		addlinetomenu(ent, "Increased damage and", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "resistance to monsters.", MENU_WHITE_CENTERED);
		return 2;
	case TALENT_RETALIATION:
		addlinetomenu(ent, "Damage increases as", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "health is reduced.", MENU_WHITE_CENTERED);
		return 2;
	case TALENT_PACK_ANIMAL:
	    addlinetomenu(ent, "Increased damage and", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "resistance when near", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "friendly morphed", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "players.", MENU_WHITE_CENTERED);
		return 4;
	//Vampire talents
	case TALENT_IMP_CLOAK:
		addlinetomenu(ent, "Move while cloaked", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "(must be crouching).", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "1/3 pc cost at night!", MENU_WHITE_CENTERED);
		return 3;
	case TALENT_ARMOR_VAMP:
		addlinetomenu(ent, "Also gain armor using", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "your vampire skill.", MENU_WHITE_CENTERED);
		return 2;
	case TALENT_FATAL_WOUND:
		addlinetomenu(ent, "Adds chance for flesh", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "eater to cause a fatal", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "wound.", MENU_WHITE_CENTERED);
		return 3;
	case TALENT_SECOND_CHANCE:
		addlinetomenu(ent, "100% chance of ghost", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "working when hit.", MENU_WHITE_CENTERED);
		return 2;
	case TALENT_IMP_MINDABSORB:
		addlinetomenu(ent, "Increases frequency of", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "mind absorb attacks.", MENU_WHITE_CENTERED);
		return 2;
	case TALENT_CANNIBALISM:
		addlinetomenu(ent, "Increases your maximum", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "health using corpse eater.", MENU_WHITE_CENTERED);
		return 2;
	//Mage talents
	case TALENT_ICE_BOLT:
		addlinetomenu(ent, "Use 'icebolt' instead of", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "fireball to chill targets.", MENU_WHITE_CENTERED);
		return 2;
	case TALENT_MEDITATION:
		addlinetomenu(ent, "Meditate to build power", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "cubes (cmd 'meditate').", MENU_WHITE_CENTERED);
		return 2;
	case TALENT_OVERLOAD:
		addlinetomenu(ent, "Use extra power cubes", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "to overload spells,", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "increasing their", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "effectiveness", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "(cmd 'overload').", MENU_WHITE_CENTERED);
		return 5;
	case TALENT_FROST_NOVA:
		addlinetomenu(ent, "Special nova spell", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "that chills players.", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "(cmd frostnova)", MENU_WHITE_CENTERED);
		return 3;
	case TALENT_IMP_MAGICBOLT:
		addlinetomenu(ent, "Power cubes are refunded", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "on successful hits.", MENU_WHITE_CENTERED);
		return 2;
	case TALENT_MANASHIELD:
		addlinetomenu(ent, "Reduces physical damage", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "by 80%%. All damage", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "absorbed consumes power", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "cubes. (cmd manashield)", MENU_WHITE_CENTERED);
		return 4;
	//Engineer talents
	case TALENT_LASER_PLATFORM:
		addlinetomenu(ent, "Create a laser platform", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "(cmd 'laserplatform').", MENU_WHITE_CENTERED);
		return 2;
	case TALENT_ALARM:
		addlinetomenu(ent, "Use 'lasertrap' instead", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "of detector to build a", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "laser trap.", MENU_WHITE_CENTERED);
		return 3;
	case TALENT_RAPID_ASSEMBLY:
		addlinetomenu(ent, "Reduces build time.", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "Can't be combined with", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "Precision Tune.", MENU_WHITE_CENTERED);
		return 3;
	case TALENT_PRECISION_TUNING:
		addlinetomenu(ent, "Increased cost and", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "build time to build", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "higher level devices.", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "Can't be combined with", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "Rapid Assembly.", MENU_WHITE_CENTERED);
		return 3;
	case TALENT_STORAGE_UPGRADE:
		addlinetomenu(ent, "Increases ammunition", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "capacity of SS/sentry/AC.", MENU_WHITE_CENTERED);
		return 2;
	//Knight talents
	case TALENT_REPEL:
		addlinetomenu(ent, "Adds chance for projectiles", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "to deflect from shield.", MENU_WHITE_CENTERED);
		return 2;
	case TALENT_MAG_BOOTS:
		addlinetomenu(ent, "Reduces effect of knockback.", MENU_WHITE_CENTERED);
		return 1;
	case TALENT_LEAP_ATTACK:
		addlinetomenu(ent, "Adds stun/knockback effect", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "to boost spell when landing.", MENU_WHITE_CENTERED);
		return 2;
	case TALENT_MOBILITY:
		addlinetomenu(ent, "Reduces boost cooldown", MENU_WHITE_CENTERED);
		return 1;
	case TALENT_DURABILITY:
		addlinetomenu(ent, "Increases your health", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "per level bonus!", MENU_WHITE_CENTERED);
		return 2;
	//Cleric talents
	case TALENT_BALANCESPIRIT:
		addlinetomenu(ent, "New spirit that can", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "use the skills of both", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "yin and yang spirits.", MENU_WHITE_CENTERED);
		return 3;
	case TALENT_HOLY_GROUND:
		addlinetomenu(ent, "Designate an area as", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "holy ground to regenerate", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "teammates (cmd 'holyground').", MENU_WHITE_CENTERED);
		return 3;
	case TALENT_UNHOLY_GROUND:
		addlinetomenu(ent, "Designate an area as", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "unholy ground to damage", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "enemies (cmd 'unholyground').", MENU_WHITE_CENTERED);
		return 3;
	case TALENT_BOOMERANG:
		addlinetomenu(ent, "Turns blessed hammers", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "into boomerangs", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "(cmd 'boomerang').", MENU_WHITE_CENTERED);
		return 3;
	case TALENT_PURGE:
		addlinetomenu(ent, "Removes curses and grants", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "temporary invincibility", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "and immunity to curses", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "(cmd 'purge').", MENU_WHITE_CENTERED);
		return 4;
	//Weaponmaster talents
	case TALENT_BASIC_AMMO_REGEN:
		addlinetomenu(ent, "Basic ammo regeneration.", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "Regenerates one ammo pack", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "for the weapon in use.", MENU_WHITE_CENTERED);
		return 3;
	case TALENT_COMBAT_EXP:
	    addlinetomenu(ent, "Increases physical,", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "damage, but reduces", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "resistance.", MENU_WHITE_CENTERED);
		return 3;
	case TALENT_TACTICS:
	    addlinetomenu(ent, "Increases your levelup", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "health and armor bonus!", MENU_WHITE_CENTERED);
		return 2;
	case TALENT_SIDEARMS:
	    addlinetomenu(ent, "Gives you additional", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "respawn weapons. Weapon", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "choice is determined by", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "weapon upgrade level.", MENU_WHITE_CENTERED);
		return 4;
	//Necromancer talents
	case TALENT_EVIL_CURSE:
		addlinetomenu(ent, "Increases curse duration.", MENU_WHITE_CENTERED);
		return 1;
	case TALENT_CHEAPER_CURSES:
		addlinetomenu(ent, "Reduces curse cost.", MENU_WHITE_CENTERED);
		return 1;
	case TALENT_CORPULENCE:
		addlinetomenu(ent, "Increases hellspawn", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "and monster health/armor,", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "but reduces damage. Can't,", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "combine with Life Tap.", MENU_WHITE_CENTERED);
		return 4;
	case TALENT_LIFE_TAP:
		addlinetomenu(ent, "Increases hellspawn", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "and monster damage,", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "but slowly saps life.", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "Can't combine with", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "Corpulence.", MENU_WHITE_CENTERED);
		return 5;
	case TALENT_DIM_VISION:
		addlinetomenu(ent, "Adds chance to", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "automatically curse", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "enemies that shoot you.", MENU_WHITE_CENTERED);
		return 3;
	case TALENT_FLIGHT:
		addlinetomenu(ent, "Reduces jetpack cost.", MENU_WHITE_CENTERED);
		return 1;
	//Shaman talents
	case TALENT_TOTEM:
		addlinetomenu(ent, "Allows you to spawn an", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "extra totem. Totem can not", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "be of the opposite element.", MENU_WHITE_CENTERED);
		return 3;
	case TALENT_ICE:
		addlinetomenu(ent, "Allows your water totem", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "to damage its targets.", MENU_WHITE_CENTERED);
		return 2;
	case TALENT_WIND:
		addlinetomenu(ent, "Allows your air totem to", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "ghost attacks for you.", MENU_WHITE_CENTERED);
		return 2;
	case TALENT_STONE:
		addlinetomenu(ent, "Allows your earth totem to", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "increase your resistance.", MENU_WHITE_CENTERED);
		return 2;
	case TALENT_SHADOW:
		addlinetomenu(ent, "Allows your darkness totem", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "to let you vamp beyond your", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "maximum health limit.", MENU_WHITE_CENTERED);
		return 3;
	case TALENT_PEACE:
		addlinetomenu(ent, "Allows your nature totem to", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "regenerate your power cubes.", MENU_WHITE_CENTERED);
		return 2;
	case TALENT_VOLCANIC:
		addlinetomenu(ent, "Adds a chance for fire", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "totem to shoot a fireball.", MENU_WHITE_CENTERED);
		return 2;
	//Alien talents
	case TALENT_PHANTOM_OBSTACLE:
		addlinetomenu(ent, "Reduces time to cloak.", MENU_WHITE_CENTERED);
		return 1;
	case TALENT_SUPER_HEALER:
		addlinetomenu(ent, "Allows healer to heal", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "beyond maximum health.", MENU_WHITE_CENTERED);
		return 2;
	case TALENT_PHANTOM_COCOON:
		addlinetomenu(ent, "Allows cocoon to cloak.", MENU_WHITE_CENTERED);
		return 1;
	case TALENT_SWARMING:
		addlinetomenu(ent, "Increases spore count,", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "but reduces damage.", MENU_WHITE_CENTERED);
		return 2;
	case TALENT_EXPLODING_BODIES:
		addlinetomenu(ent, "Makes alien-summons'", MENU_WHITE_CENTERED);
		addlinetomenu(ent, "corpses explode.", MENU_WHITE_CENTERED);
		return 2;
	default:	return 0;
	}
}


void openTalentMenu(edict_t *ent, int talentID)
{
	talent_t *talent	= &ent->myskills.talents.talent[getTalentSlot(ent, talentID)];
	//int level			= talent->upgradeLevel;
	int lineCount = 7;//12;

	if (!ShowMenu(ent))
       return;
	clearmenu(ent);

	addlinetomenu(ent, "Talent", MENU_GREEN_CENTERED);
//	addlinetomenu(ent, GetTalentString(talentID), MENU_WHITE_CENTERED);
	addlinetomenu(ent, " ", 0);

	lineCount += writeTalentDescription(ent, talentID);

	addlinetomenu(ent, " ", 0);
	addlinetomenu(ent, " ", 0);
	
	if(talent->upgradeLevel < talent->maxLevel)
		addlinetomenu(ent, "Upgrade this talent.", -1*(talentID+1));
	else addlinetomenu(ent, " ", 0);

	addlinetomenu(ent, "Previous menu.", talentID+1);

	setmenuhandler(ent, TalentUpgradeMenu_handler);
	ent->client->menustorage.currentline = lineCount;
	showmenu(ent);
}

//****************************************
//*********** Main Talent Menu ***********
//****************************************

void openTalentMenu_handler(edict_t *ent, int option)
{
	switch(option)
	{
	case 9999:	//Exit
		{
			closemenu(ent);
			return;
		}
	default: openTalentMenu(ent, option);
	}
}
/*
void OpenTalentUpgradeMenu (edict_t *ent, int lastline)
{
	talent_t	*talent;
	char		buffer[30];
	int			i;
    
	if (!ShowMenu(ent))
       return;
	clearmenu(ent);

	// menu header
	addlinetomenu(ent, "Talents", MENU_GREEN_CENTERED);
	addlinetomenu(ent, " ", 0);

	for (i = 0; i < ent->myskills.talents.count; i++)
	{
		talent = &ent->myskills.talents.talent[i];

		//create menu string
		strcpy(buffer, GetTalentString(talent->id));
		strcat(buffer, ":");
		padRight(buffer, 15);

		addlinetomenu(ent, va("%d. %s %d/%d", i+1, buffer, talent->upgradeLevel, talent->maxLevel), talent->id);
	}

	// menu footer
	addlinetomenu(ent, " ", 0);
	addlinetomenu(ent, va("You have %d talent points.", ent->myskills.talents.talentPoints), 0);
	addlinetomenu(ent, " ", 0);

	addlinetomenu(ent, "Exit", 9999);
	setmenuhandler(ent, openTalentMenu_handler);

	if (!lastline)	ent->client->menustorage.currentline = ent->myskills.talents.count + 6;
	else			ent->client->menustorage.currentline = lastline + 2;

	showmenu(ent);

	// try to shortcut to chat-protect mode
//	if (ent->client->idle_frames < CHAT_PROTECT_FRAMES-51)
//		ent->client->idle_frames = CHAT_PROTECT_FRAMES-51;
}
*/

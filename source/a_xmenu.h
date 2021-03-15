//-----------------------------------------------------------------------------
// Menu Stuff
//
// $Id: a_xmenu.h,v 1.2 2001/09/28 13:48:34 ra Exp $
//
//-----------------------------------------------------------------------------
// $Log: a_xmenu.h,v $
// Revision 1.2  2001/09/28 13:48:34  ra
// I ran indent over the sources. All .c and .h files reindented.
//
// Revision 1.1.1.1  2001/05/06 17:25:35  igor_rock
// This is the PG Bund Edition V1.25 with all stuff laying around here...
//
//-----------------------------------------------------------------------------

#define XMENU_MAX_ENTRIES 6
#define XMENU_TOTAL_ENTRIES XMENU_MAX_ENTRIES+2
#define XMENU_END_ENTRY XMENU_MAX_ENTRIES+12
#define XMENU_TITLE_MAX 30

// ----------------------------------------------------------------------
// For talents.c -JukS (14.03.2021)
#define MENU_WHITE_CENTERED		20001
#define	MENU_GREEN_CENTERED		20002
#define MENU_GREEN_RIGHT		20003
#define MENU_GREEN_LEFT			20004

#define MAX_LINES				21	// Lines per menu.
#define MENU_MAX_LINE_LEN		28	// Limit 27 chars in menu line.
#define LINE_SPACING			8

void addlinetomenu(edict_t* ent, char* line, int option);
void clearmenu(edict_t* ent);
void setmenuhandler(edict_t* ent, void (*optionselected)(edict_t* ent, int option));
void ClearAllMenus(void);

void menuup(edict_t* ent);
void menudown(edict_t* ent);
void menuselect(edict_t* ent);

void initmenu(edict_t* ent);
void showmenu(edict_t* ent);
void closemenu(edict_t* ent);

typedef struct menumsg_s
{
	char* msg;
	int		option;
} menumsg_t;

typedef struct menusystem_s
{
	void 		(*optionselected)(edict_t* ent, int option);
	void		(*oldmenuhandler)(edict_t* ent, int option);
	qboolean 	menu_active;
	qboolean	displaymsg;
	int			oldline;
	menumsg_t	messages[MAX_LINES * MENU_MAX_LINE_LEN];
	int 		currentline;
	int			num_of_lines;
	int			menu_index;
} menusystem_t;
// ----------------------------------------------------------------------
// End -JukS

typedef char XMENU_TITLE[XMENU_TITLE_MAX];

typedef struct
{
  XMENU_TITLE name;
  void (*SelectFunc) (edict_t * ent, pmenu_t * p);
}
XMENU_ENTRY;

typedef struct
{
  XMENU_ENTRY xmenuentries[XMENU_TOTAL_ENTRIES];
  pmenu_t themenu[XMENU_END_ENTRY];
  int xmenutop;
  int xmenucount;
  void (*DoAddMenu) (edict_t * ent, int fromix);
}
xmenu_t;

qboolean xMenu_New (edict_t * ent, char *title, char *subtitle,
		    void (*DoAddMenu) (edict_t * ent, int fromix));
qboolean xMenu_Add (edict_t * ent, char *name,
		    void (*SelectFunc) (edict_t * ent, pmenu_t * p));

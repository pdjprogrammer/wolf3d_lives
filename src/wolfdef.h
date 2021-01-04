#include "common/common.h"

#define XRES		640
#define YRES		480
#define SIZE_2D_X	640
#define SIZE_2D_Y	480

#include "build.h"
#include "math.h"
#include "opengl.h"
#include "video.h"
#include "sound.h"
#include "keys.h"
#include "q_fns.h"
#include "texman.h"
#include "fileio.h"
#include "client.h"
#include "game.h"
#include "level.h"
#include "areas.h"
#include "doors.h"
#include "sprites.h"
#include "actors.h"
#include "player.h"
#include "ai_com.h"
#include "actor_ai.h"
#include "fontman.h"
#include "menu.h"
#include "render.h"
#include "raycast.h"
#include "map.h"
#include "automap.h"
#include "version.h"
#include "vgapics.h"
#include "powerups.h"
#include "pwalls.h"
#include "wl_text.h"
#include "saveload.h"
#include "particles.h"

void LoadRealLevel(int level);
char CanCloseDoor(int x, int y, char vert);
void ScanInfoPlane(int level);
void SpawnStand(enemy_t which, int tilex, int tiley, int dir);
void SpawnPatrol(enemy_t which, int tilex, int tiley, int dir);
void SpawnDeadGuard(enemy_t which, int x, int y);
void SpawnBoss(enemy_t which, int x, int y);
void SpawnGhosts(enemy_t which, int x, int y);
void ProcessGuards(void);
void T_BJRun(Guard_struct *Guard);
void T_BJJump(Guard_struct *Guard);
void T_BJYell(Guard_struct *Guard);
void T_BJDone(Guard_struct *Guard);
void SpawnBJVictory(void);
bool CheckLine(Guard_struct *Guard);

#define MINACTORDIST		0x10000		// minimum dist from player center to any actor center
#define TILEGLOBAL			0x10000
#define HALFTILE			0x8000
#define FLOATTILE			65536.0f
#define TILESHIFT			16			// 1<<TILESHIFT=0x10000 (TILEGLOBAL)
#define HORIZON				240			// YRES/2
#define VIEWSIZE			412			// where to start Control Panel
#define MINDIST				(0x5800)
#define PLAYERSIZE			MINDIST		// player radius
#define CLOSEWALL			MINDIST		// Space between wall & player
#define PLAYERSPEED			3000
#define RUNSPEED			6000
#define	TEX_CON				0
#define TEX_FLOOR			1
#define TEX_CEL				2
#define TEX_WALL_BEG		3
#define SECRET_BLOCK		0x62
#define ELEVATOR_SWITCH		21
#define ELEVATOR_PRESSED	22
#define ALTELEVATORTILE		107
#define MAPSHIFT			2
#define MAPXSTART			192
#define MAPYSTART			100
#define SIZEOF_SPRITES		24
#define MAXACTORS			150			// max number of nazis, etc / map
#define MAXSTATS			400			// max number of lamps, bonus, etc
#define MAXDOORS			64			// max number of sliding doors
#define MAXWALLTILES		64			// max number of wall tiles
#define TURNTICS			10
#define SPDPATROL			512
#define SPDDOG				1500
#define FL_SHOOTABLE		1
#define FL_BONUS			2
#define FL_NEVERMARK		4
#define FL_VISABLE			8
#define FL_ATTACKMODE		16
#define FL_FIRSTATTACK		32
#define FL_AMBUSH			64
#define FL_NONMARK			128

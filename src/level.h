#define TILE_IS_WALL		1
#define TILE_IS_PWALL		(1 << 20)
#define TILE_IS_DOOR		2
#define TILE_IS_SECRET		4
#define TILE_IS_DRESS		8
#define TILE_IS_BLOCK		16
#define TILE_IS_ACTOR		32
#define TILE_IS_DEADACTOR	64
#define TILE_IS_POWERUP		128
#define TILE_IS_AMBUSH		256
#define TILE_IS_EXIT		512
#define TILE_IS_SLEVEL		1024
#define TILE_IS_ELEVATOR	(1 << 11)
#define TILE_IS_E_TURN		(1 << 12)
#define TILE_IS_NE_TURN		(1 << 13)
#define TILE_IS_N_TURN		(1 << 14)
#define TILE_IS_NW_TURN		(1 << 15)
#define TILE_IS_W_TURN		(1 << 16)
#define TILE_IS_SW_TURN		(1 << 17)
#define TILE_IS_S_TURN		(1 << 18)
#define TILE_IS_SE_TURN		(1 << 19)
#define TILE_IS_SOLIDTILE	(TILE_IS_WALL | TILE_IS_BLOCK | TILE_IS_PWALL)
#define TILE_BLOCKS_MOVE	(TILE_IS_WALL | TILE_IS_BLOCK | TILE_IS_PWALL | TILE_IS_ACTOR)
#define TILE_IS_WAYPOINT	(TILE_IS_E_TURN | TILE_IS_NE_TURN | TILE_IS_N_TURN | TILE_IS_NW_TURN | TILE_IS_W_TURN | TILE_IS_SW_TURN | TILE_IS_S_TURN | TILE_IS_SE_TURN)

typedef struct
{
	int mapid;
	unsigned short layer1[64*64];
	unsigned short layer2[64*64];
	unsigned short layer3[64*64];
	long tile_info[64][64];
	int wall_tex_x[64][64];
	int wall_tex_y[64][64];
	int areas[64][64];
	placeonplane_t pSpawn;
	char name[17];
	int music;
	RGBcolor celing, floor;
} leveldef;

extern leveldef CurMapData;
extern int TotalLevels;
extern int Lvl_Init(void);
extern int Lvl_LoadLevel(int zbmn);

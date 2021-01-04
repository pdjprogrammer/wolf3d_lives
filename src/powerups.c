#include <windows.h>
#include <stdio.h>
#include "wolfdef.h"
#include "sprt_def.h"

typedef struct powerup_s
{
	int x, y;
	pow_t type;
	int sprite;
	struct powerup_s *prev, *next;
} powerup_t;

powerup_t *powerups = NULL;
powerup_t *Pow_Remove(powerup_t *powerup);
powerup_t *Pow_AddNew(void);

bool spearflag = false;
placeonplane_t spearlocation;

int Pow_Texture[pow_last] =
{
	SPR_STAT_34,	//pow_gibs
	SPR_STAT_38,	//pow_gibs2
	SPR_STAT_6,		//pow_alpo
	SPR_STAT_25,	//pow_firstaid
	SPR_STAT_20,	//pow_key1
	SPR_STAT_21,	//pow_key2
	SPR_STAT_20,	//pow_key3
	SPR_STAT_20,	//pow_key4
	SPR_STAT_29,	//pow_cross
	SPR_STAT_30,	//pow_chalice
	SPR_STAT_31,	//pow_bible
	SPR_STAT_32,	//pow_crown
	SPR_STAT_26,	//pow_clip
	SPR_STAT_26,	//pow_clip2
	SPR_STAT_27,	//pow_machinegun
	SPR_STAT_28,	//pow_chaingun
	SPR_STAT_24,	//pow_food
	SPR_STAT_33,	//pow_fullheal
// spear
	SPR_STAT_49,	//pow_25clip
	SPR_STAT_51,	//pow_spear
};

int Pow_Init(void)
{
	Pow_Reset();
	return 1;
}

void Pow_Reset(void)
{
	powerup_t *powerup = powerups;

	while(powerup)
		powerup = Pow_Remove(powerup);
	powerups = NULL;
}

void Pow_Save(FILE *fp)
{
	powerup_t *powerup;
	int n;

	for(n = 0, powerup = powerups; powerup; n++, powerup = powerup->next);
	fwrite(&n, sizeof(n), 1, fp);
	for(powerup = powerups; powerup; powerup = powerup->next)
		fwrite(powerup, sizeof(powerup_t), 1, fp);
}

void Pow_Load(FILE *fp)
{
	powerup_t *powerup, pow;
	int n, tmp;

	fread(&tmp, sizeof(tmp), 1, fp);
	for(n = 0; n < tmp; n++)
	{
		fread(&pow, sizeof(powerup_t), 1, fp);
		powerup = Pow_AddNew();
		powerup->sprite = pow.sprite;
		powerup->type = pow.type;
		powerup->x = pow.x;
		powerup->y = pow.y;
	}
}

powerup_t *Pow_Remove(powerup_t *powerup)
{
	powerup_t *next;

	if(powerup == NULL)
		return NULL;
	if(powerup->prev)
		powerup->prev->next = powerup->next;
	if(powerup->next)
		powerup->next->prev = powerup->prev;
	next = powerup->next;
	if(powerups == powerup)
		powerups = next;
	free(powerup);
	return next;
}

powerup_t *Pow_AddNew(void)
{
	powerup_t *newp;

	newp = malloc(sizeof(powerup_t));
	newp->prev = NULL;
	newp->next = powerups;
	if(powerups)
		powerups->prev = newp;
	powerups = newp;
	return newp;
}

//	Pow_Give
//	returns 1 powerup is picked up
int Pow_Give(pow_t type)
{
	char *keynames[] = {"gold", "silver", "?", "?"};

	switch(type)
	{
	case pow_key1:
	case pow_key2:
	case pow_key3:
	case pow_key4:
		type -= pow_key1;
		PL_GiveKey(&Player, type);
		SD_PlaySound(GETKEYSND, CHAN_ITEM);
		Msg_Printf("Picked up a %s key.", keynames[type]);
		break;

	case pow_cross:
		PL_GivePoints(&Player, 100);
		SD_PlaySound(BONUS1SND, CHAN_ITEM);
		gamestate.treasurecount++;
		Msg_Printf("Picked up a cross.");
		break;
	case pow_chalice:
		PL_GivePoints(&Player, 500);
		SD_PlaySound(BONUS2SND, CHAN_ITEM);
		gamestate.treasurecount++;
		Msg_Printf("Picked up a chalice.");
		break;
	case pow_bible:
		PL_GivePoints(&Player, 1000);
		SD_PlaySound(BONUS3SND, CHAN_ITEM);
		gamestate.treasurecount++;
		Msg_Printf("Picked up a chest.");
		break;
	case pow_crown:
		PL_GivePoints(&Player, 5000);
		SD_PlaySound(BONUS4SND, CHAN_ITEM);
		gamestate.treasurecount++;
		Msg_Printf("Picked up a crown.");
		break;

	case pow_gibs:
		if(!PL_GiveHealth(&Player, 1, 9999))
			return 0;
		SD_PlaySound(SLURPIESND, CHAN_ITEM);
		Msg_Printf("BLOOD!");
		break;
	case pow_alpo:
		if(!PL_GiveHealth(&Player, 4, 9999))
			return 0;
		SD_PlaySound(HEALTH1SND, CHAN_ITEM);
		Msg_Printf("Picked up dog's food.");
		break;
	case pow_food:
		if(!PL_GiveHealth(&Player, 10, 9999))
			return 0;
		SD_PlaySound(HEALTH1SND, CHAN_ITEM);
		Msg_Printf("Picked up cold food.");
		break;
	case pow_firstaid:
		if(!PL_GiveHealth(&Player, 25, 9999))
			return 0;
		SD_PlaySound(HEALTH2SND, CHAN_ITEM);
		Msg_Printf("Picked up a medikit.");
		break;

	case pow_clip:
		if(!PL_GiveAmmo(&Player, AMMO_BULLETS, 8))
			return 0;
		SD_PlaySound(GETAMMOSND, CHAN_ITEM);
		Msg_Printf("Picked up a clip.");
		break;
	case pow_clip2:
		if(!PL_GiveAmmo(&Player, AMMO_BULLETS, 4))
			return 0;
		SD_PlaySound(GETAMMOSND, CHAN_ITEM);
		Msg_Printf("Picked up a used clip.");
		break;
	case pow_25clip:
		if(!PL_GiveAmmo(&Player, AMMO_BULLETS, 25))
			return 0;
		SD_PlaySound(GETAMMOBOXSND, CHAN_ITEM);
		Msg_Printf("Picked up an ammo box.");
		break;

	case pow_machinegun:
		PL_GiveWeapon(&Player, WEAPON_AUTO);
		SD_PlaySound(GETMACHINESND, CHAN_ITEM);
		Msg_Printf("You got the machine gun!");
		break;
	case pow_chaingun:
		PL_GiveWeapon(&Player, WEAPON_CHAIN);
		SD_PlaySound(GETGATLINGSND, CHAN_ITEM);
		Msg_Printf("You got the chain gun!");
		Player.facecount = 0;
		Player.face_gotgun = true;
		break;

	case pow_fullheal:
		PL_GiveHealth(&Player, 50, 9999);
		PL_GiveAmmo(&Player, AMMO_BULLETS, 25);
		PL_GiveLife(&Player);
		gamestate.treasurecount++;
		Msg_Printf("Extra life!");
		break;

	case pow_spear:
	{
		Msg_Printf("Picked up the Spear of Destiny!");
		spearflag = true;
		spearlocation = Player.position;
		break;
	}

	default:
		Con_Printf("Warning: Unknown item type: %d\n", type);
		break;
	}
	return 1;
}

//	Pow_Spawn
//	x and y are in TILES
void Pow_Spawn(int x, int y, int type)
{
	powerup_t *newp;

	CurMapData.tile_info[x][y] |= TILE_IS_POWERUP;
	newp = Pow_AddNew();
	newp->sprite = Spr_GetNewSprite();
	Spr_SetPos(newp->sprite, TILE2POS(newp->x = x), TILE2POS(newp->y = y), 0);
	newp->type = type;
	Spr_SetTex(newp->sprite, -1, Pow_Texture[type]);
	CurMapData.tile_info[x][y] |= TILE_IS_POWERUP;
}

//	Pow_PickUp
//	x, y, is in TILES
//	we will get here only if tile powerup flag is set!
void Pow_PickUp(int x, int y)
{
	powerup_t *pow;
	bool p_left = false, p_pick = false;

	for(pow = powerups; pow; pow = pow->next)
	{
	check_again:
		if(pow->x == x && pow->y == y)
		{
			if(Pow_Give(pow->type))
			{
				p_pick = true;
				Spr_RemoveSprite(pow->sprite);
				pow = Pow_Remove(pow);
				if(pow)
					goto check_again;
				else
					break;
			}
			else
			{
				p_left = true;
			}
		}
	}
	if(p_pick)
		R_BonusFlash();
	if(p_left)
		CurMapData.tile_info[x][y] |= TILE_IS_POWERUP;
	else
		CurMapData.tile_info[x][y] &= ~TILE_IS_POWERUP;
	if(spearflag)
	{
		spearflag = false;
		gamestate.map = 21;
		LoadRealLevel(gamestate.map - 1);
		PL_Spawn(spearlocation);
	}
}

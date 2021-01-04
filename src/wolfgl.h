char skill = 2;

void DrawDebug(void)
{
	FNT_SetFont(FNT_CONSOLE);
	FNT_SetStyle(0, 0, 0);
	FNT_SetColor(0, 180, 180, 255);
	FNT_SetScale(1, 1);
	FNT_PrintfPos(1, 1, "Player: [%2d; %2d @ %3d]",
		Player.tilex, Player.tiley, FINE2DEG(Player.position.angle));
	FNT_PrintfPos(54, 1, "%3d fps", r_fps);
	FNT_PrintfPos(1, 2, "Kills:%d/%d Treasure:%d/%d Secrets:%d/%d",
		gamestate.killcount,	gamestate.killtotal,
		gamestate.treasurecount,	gamestate.treasuretotal,
		gamestate.secretcount,		gamestate.secrettotal);
	FNT_PrintfPos(1, 3, "Health:%d Ammo:%d Lives: %d Score: %d", Player.health, Player.ammo[AMMO_BULLETS], Player.lives, Player.score);
}

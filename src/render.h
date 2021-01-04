#define BONUS_FLASH_MAX	64

void R_Init(void);
void R_DrawHUD(void);
void R_UpdateScreen(void);
void R_ResetFlash(void);
void R_BonusFlash(void);
void R_DamageFlash(int damage);

extern int r_fps;
extern int r_polys;
extern cvar_t *crosshair_enabled;
extern cvar_t *crosshair_outer_radius;
extern cvar_t *crosshair_inner_radius;
extern cvar_t *crosshair_thickness;
extern cvar_t *crosshair_alpha;
extern cvar_t *crosshair_bonus_scale;

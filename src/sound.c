#include "wolfdef.h"
#include "win32/win32.h"
#include "fmopl.h"

#define MAX_CHANNELS	32		// Limit is 64 (in My Sound Library)

snd_mode SD_mode;

HINSTANCE hInstDS = NULL;
static FM_OPL *hAdLib = NULL, *hAdLibMus = NULL;
char sound_on = 0, adlib_on = 0, adlib_mus_on = 0;
short DigiMap[LASTSOUND];
word *DigiList, NumDigi, DigiLeft, DigiPage;
int last_song = -1;

struct ChannelStruct
{
	int Sound;			// Loaded Sound
	char Name[128];		// Loaded Sound Name "" if channel is free
	char Volume;		// 0~100 %
	char Position;		// use positioning?
	int x, y, z;		// Position in Space. Long until will be used...
} Channels[MAX_CHANNELS];

typedef	struct
{
	char length[4];
	char priority[2];
} SoundCommon;

typedef	struct
{
	byte mChar, cChar,
		mScale,	cScale,
		mAttack, cAttack,
		mSus, cSus,
		mWave, cWave,
		nConn,
		// These are only for Muse - these bytes are really unused
		voice,
		mode,
		unused[3];
} Instrument;

typedef	struct
{
	SoundCommon	common;
	Instrument inst;
	byte block, data[1];
} AdLibSound;

#define ADLIB_SND_SPEED	22050
#define ADLIB_MUS_SPEED 22050
#define ADLIB_MUS_BYPS	(ADLIB_MUS_SPEED * 2)

WAVEFORMATEX id_snd_fmt =
{
	WAVE_FORMAT_PCM,
	1,
	7000,
	7000,
	1,
	8,
	0
},

al_snd_fmt =
{
	WAVE_FORMAT_PCM,
	1,
	ADLIB_SND_SPEED,
	ADLIB_SND_SPEED * 2,
	2,
	16,
	0
},

al_mus_fmt =
{
	WAVE_FORMAT_PCM,
	1,
	ADLIB_MUS_SPEED,
	ADLIB_MUS_SPEED * 2,
	2,
	16,
	0
};

typedef int (*hNotify) (int pos, int size, void *buffer);
int UpdateMusic(int pos, int size, void *buffer);

HANDLE adlib_in_use;

int (*SND_StartUp)(HWND hWnd);
int (*SND_ShutDown)(void);
int (*SND_CreateChannel)(int Channel, WAVEFORMATEX *wavefmt, unsigned long Size, char *data);
int (*SND_CreateFromWav)(int Channel, char *FileName);
int (*SND_Play)(int Channel, int loop);
int (*SND_Stop)(int Channel);
int (*SND_GetPos)(int Channel);
int (*SND_Volume)(int Channel, int Vol);
int (*SND_Pan)(int Channel, int Pan);
int (*SND_PlayStream)(void);
int (*SND_StopStream)(void);
int (*SND_DestroyStream)(void);
int (*SND_CreateStreamChannel)(unsigned long NotifySize, WAVEFORMATEX *wavefmt, hNotify Proc);
int	leftchannel, rightchannel;
int SD_LoadALSound(int n, int chn);

#define SND_LIB_NAME "sndlib.dll"

int SD_Init(void)
{
	int n, res;

	memset(&SD_mode, 0, sizeof(SD_mode));
	SD_mode.avail[0][0] = SD_mode.avail[1][0] = SD_mode.avail[2][0] = true;
	SD_mode.sfx = sfx_none;
	SD_mode.digi = digi_none;
	SD_mode.mus = mus_none;

	Con_Printf("SD_Init: Starting Sound...\n");
	if(!hInstDS)
	{
		Con_Printf("-> Loading library %s: ", SND_LIB_NAME);
		hInstDS = LoadLibrary(SND_LIB_NAME);
		if(hInstDS == NULL)
		{
			Con_Printf("Failed\n");
			Con_Printf("-> Running without sound support.\n");
			return 0;
		}
		Con_Printf("Done!\n");
		SND_StartUp = (void *)GetProcAddress(hInstDS, "SND_StartUp");
		SND_ShutDown = (void *)GetProcAddress(hInstDS, "SND_ShutDown");
		SND_CreateFromWav = (void *)GetProcAddress(hInstDS, "SND_CreateChannelFromFile");
		SND_CreateChannel = (void *)GetProcAddress(hInstDS, "SND_CreateChannel");
		SND_Play = (void *)GetProcAddress(hInstDS, "SND_PlayChannel");
		SND_Stop = (void *)GetProcAddress(hInstDS, "SND_StopChannel");
		SND_GetPos = (void *)GetProcAddress(hInstDS, "SND_GetPosition");
		SND_Volume = (void *)GetProcAddress(hInstDS, "SND_SetVolume");
		SND_Pan = (void *)GetProcAddress(hInstDS, "SND_SetPan");
		SND_PlayStream = (void *)GetProcAddress(hInstDS, "SND_PlayStream");
		SND_StopStream = (void *)GetProcAddress(hInstDS, "SND_StopStream");
		SND_DestroyStream = (void *)GetProcAddress(hInstDS, "SND_DestroyStream");
		SND_CreateStreamChannel = (void *)GetProcAddress(hInstDS, "SND_CreateStreamChannel");
	}
	memset(Channels, 0, sizeof(Channels));
	for(n = 0; n < MAX_CHANNELS; n++)
	{
		Channels[n].Sound = -1;
	}
	Con_Printf("-> Starting Sound System: ");
	if(!SND_StartUp)
	{
		Con_Printf("Failed\n");
		Con_Printf("-> Sound support disabled!\n");
		FreeLibrary(hInstDS);
		return 0;
	}
	res = SND_StartUp(sys_hwnd);
	if(res)
	{
		Con_Printf("Failed\n-> Error code: %d\n", res);
		Con_Printf("-> Sound support disabled!\n");
		FreeLibrary(hInstDS);
		return 0;
	}
	Con_Printf("Done!\n");

	Con_Printf("-> Starting AdLib Support: ");
	hAdLib = OPLCreate(OPL_TYPE_YM3812, 3579545, ADLIB_SND_SPEED);
	if(hAdLib == NULL)
	{
		Con_Printf("Failed\n");
	}
	else
	{
		OPLWrite(hAdLib, 0x01, 0x20);
		OPLWrite(hAdLib, 0x08, 0x00);
		Con_Printf("Done!\n");
		adlib_on = 1;
	}

	Con_Printf("-> Starting AdLib Music Support: ");
	hAdLibMus = OPLCreate(OPL_TYPE_YM3812, 3579545, ADLIB_MUS_SPEED);
	if(hAdLibMus == NULL)
	{
		Con_Printf("Failed\n");
	}
	else
	{
		OPLWrite(hAdLibMus, 0x01, 0x20);
		OPLWrite(hAdLibMus, 0x08, 0x00);
		Con_Printf("Done!\n");
		res = SND_CreateStreamChannel(ADLIB_MUS_BYPS, &al_mus_fmt, UpdateMusic);
		if(res == 0)
		{
			OPLDestroy(hAdLibMus);
			Con_Printf("-> Failed to create DirectSound Stream...\n");
		}
		adlib_mus_on = 1;
	}
	if(adlib_on || adlib_mus_on)
	{
		adlib_in_use = CreateMutex(NULL, FALSE, NULL);
		if(adlib_in_use == NULL)
			Con_Printf("Error Creating Mutex\n");
	}

	SD_InitDigiMap();

	SD_mode.avail[1][1] = true;
	SD_mode.digi = digi_sb;

	if(adlib_on)
	{
		SD_mode.avail[0][2] = true;
		SD_mode.sfx = sfx_adlib;
	}
	if(adlib_mus_on)
	{
		SD_mode.avail[2][1] = true;
		SD_mode.mus = mus_none;
	}

	Cmd_AddCommand("playimf", SD_PlayImf_f);
	Con_Printf("-> Sound initialized!\n");
	sound_on = 1;
	return 1;
}

void SD_Config(int option, int setting)
{
	int n;

	if(!SD_mode.avail[option][setting])
		return;
	switch(option)
	{
	case 0:
		if(SD_mode.sfx == setting)
			return;
		switch(setting)
		{
		case sfx_none:
			for(n = 0; n < MAX_CHANNELS; n++)
				Channels[n].Sound = -1;
			SD_mode.sfx = sfx_none;
			SD_UnCacheAllSnds();
			break;
		case sfx_adlib:
			for(n = 0; n < MAX_CHANNELS; n++)
				Channels[n].Sound = -1;
			SD_mode.sfx = sfx_adlib;
			break;
		}
		break;
	case 1:
		if(SD_mode.digi == setting)
			return;
		switch(setting)
		{
		case digi_none:
			for(n = 0; n < MAX_CHANNELS; n++)
				Channels[n].Sound = -1;
			SD_mode.digi = digi_none;
			break;
		case digi_sb:
			for(n = 0; n < MAX_CHANNELS; n++)
				Channels[n].Sound = -1;
			SD_mode.digi = digi_sb;
			break;
		}
		break;
	case 2:
		if(SD_mode.mus == setting)
			return;
		switch(setting)
		{
		case mus_none:
			SD_mode.mus = mus_none;
			SD_StopMusic();
			break;
		case mus_adlib:
			SD_mode.mus = mus_adlib;
			SD_PlayMusic(last_song, true);
			break;
		}
		break;
	}
}

void SD_ShutDown(void)
{
	if(!sound_on)
		return;
	if(adlib_on || adlib_mus_on)
	{
		WaitForSingleObject(adlib_in_use, 2000);
	}
	SD_StopAllSound();
	SD_UnCacheAllSnds();
	SD_StopMusic();
	if(hAdLib)
	{
		OPLDestroy(hAdLib);
		hAdLib = NULL;
	}
	if(hAdLibMus)
	{
		OPLDestroy(hAdLibMus);
		hAdLibMus = NULL;
	}
	if(hInstDS == NULL)
		return;

	SND_ShutDown();
	FreeLibrary(hInstDS);
	hInstDS = NULL;
	if(DigiList)
		free(DigiList);
	if(adlib_in_use)
	{
		ReleaseMutex(adlib_in_use);
		CloseHandle(adlib_in_use);
	}
	sound_on = 0;
}

void SD_InitDigiMap(void)
{
	int n;
	int *map;

	for(n = 0; n < LASTSOUND; n++)
		DigiMap[n] = -1;

	if(File_PML_LoadDigiMap(&DigiList) == -1)
	{
		Con_Printf("Error Loading DigiMap\n");
		return;
	}

	for(map = wolfdigimap; *map != LASTSOUND; map += 2)
		DigiMap[map[0]] = map[1];
}

bool SD_SoundPlaying(soundnames SoundNum)
{
	int Chn;

	if(!sound_on)
		return false;

	for(Chn = 0; Chn < MAX_CHANNELS; Chn++)
	{
		if(Channels[Chn].Sound == SoundNum && SND_GetPos(Chn) != 1000)
			return true;
	}
	return false;
}

void SD_PlaySound(soundnames SoundNum, int channel)
{
	int Chn;

	if(!sound_on)
		return;

	if((Chn=SD_ChooseChannel(SoundNum, channel)) == -1)
		return;

	Channels[Chn].Position = 0;

	SND_Volume(Chn, 0);
	SND_Pan(Chn, 0);
	SND_Play(Chn, 0);
}

void SD_PlaySound3D(soundnames SoundNum, int channel, int x, int y)
{
	int Chn;

	if(!sound_on)
		return;

	if((Chn=SD_ChooseChannel(SoundNum, channel)) == -1)
		return;

#if 1
	Channels[Chn].Position = 1;
	Channels[Chn].x = x;
	Channels[Chn].y = y;

	SD_SetSoundLoc(x, y);

	leftchannel = leftchannel * 375;
	rightchannel = rightchannel * 375;

	SND_Volume(Chn, leftchannel <= rightchannel ? -leftchannel : -rightchannel);
	SND_Pan(Chn, leftchannel - rightchannel);
#else
	Channels[Chn].Position = 0;
	SND_Volume(Chn, 0);
	SND_Pan(Chn, 0);
#endif

	SND_Play(Chn, 0);
}

void SD_StopAllSound(void)
{
	int n;

	if(!sound_on)
		return;

	for(n = 0; n < MAX_CHANNELS; n++)
	{
		if(Channels[n].Sound != -1)
			SND_Stop(n);
	}
}

void SD_UpdateSounds3D(void)
{
	int Chn;

	if(!sound_on)
		return;

	for(Chn = 0; Chn < MAX_CHANNELS; Chn++)
	{
		if(Channels[Chn].Position)
		{
			if(SND_GetPos(Chn) == 1000)
			{
				Channels[Chn].Position = 0;
				continue;
			}
			SD_SetSoundLoc(Channels[Chn].x, Channels[Chn].y);
			leftchannel *= 375;
			rightchannel *= 375;
			SND_Volume(Chn, leftchannel <= rightchannel ? -leftchannel : -rightchannel);
			SND_Pan(Chn, leftchannel - rightchannel);
		}
	}
}

unsigned char sndbuff[1024 * 256];
int SD_LoadIdSound(int n, int chn)
{
	int slen, digin, p1, p2;

	if(DigiMap[n] == -1 || SD_mode.digi == digi_none)
	{
		if(SD_mode.sfx == sfx_adlib)
			if(SD_LoadALSound(SND_REMAP[n], chn))
				return 1;
		return 0;
	}

	digin = DigiMap[n];
	p1 = DigiList[digin * 2];
	p2 = DigiList[digin * 2 + 1];

	slen = File_PML_LoadSound(p1, p2, sndbuff);

	SND_CreateChannel(chn, &id_snd_fmt, slen, sndbuff);

	Channels[chn].Sound = n;

	return 1;
}

int SD_SynthALSound(cache_snd *snd)
{
	AdLibSound *sound;
	Instrument inst;
	byte alBlock;
	int alLengthLeft;
	byte *alSound, s;
	word *ptr;
	DWORD mut_ret;

	if(!adlib_on)
		return 0;
	if(!File_AUD_ReadChunk(snd->name + STARTADLIBSOUNDS, sndbuff))
		return 0;
	sound = (AdLibSound*)sndbuff;

	inst = sound->inst;
	alBlock = ((sound->block & 7) << 2) | 0x20;
	alLengthLeft = *((unsigned long *)sound->common.length);
	alSound = sound->data;
	snd->length = alLengthLeft * 157 * 2;
	snd->data = malloc(snd->length);
	ptr = (word *)snd->data;

	mut_ret = WaitForSingleObject(adlib_in_use, 1000);
	if(mut_ret != WAIT_OBJECT_0)
	{
		free(snd->data);
		return 0;
	}

	OPLWrite(hAdLib, alFreqL, 0);
	OPLWrite(hAdLib, alFreqH, 0);
	OPLWrite(hAdLib, 0 + alChar, inst.mChar);
	OPLWrite(hAdLib, 0 + alScale, inst.mScale);
	OPLWrite(hAdLib, 0 + alAttack, inst.mAttack);
	OPLWrite(hAdLib, 0 + alSus, inst.mSus);
	OPLWrite(hAdLib, 0 + alWave, inst.mWave);
	OPLWrite(hAdLib, 3 + alChar, inst.cChar);
	OPLWrite(hAdLib, 3 + alScale, inst.cScale);
	OPLWrite(hAdLib, 3 + alAttack, inst.cAttack);
	OPLWrite(hAdLib, 3 + alSus, inst.cSus);
	OPLWrite(hAdLib, 3 + alWave, inst.cWave);
	OPLWrite(hAdLib, alFeedCon, 0);

	while(alLengthLeft)
	{
		s = *alSound++;
		if(!s)
			OPLWrite(hAdLib, alFreqH + 0, 0);
		else
		{
			OPLWrite(hAdLib, alFreqL + 0, s);
			OPLWrite(hAdLib, alFreqH + 0, alBlock);
		}
		if(!(--alLengthLeft))
		{
			OPLWrite(hAdLib, alFreqH + 0, 0);
		}
		YM3812UpdateOne(hAdLib, ptr, 157);
		ptr += 157;
	}

	ReleaseMutex(adlib_in_use);
	return 1;
}

int SD_LoadALSound(int n, int chn)
{
	cache_snd *snd;

	snd = SD_CacheSND(n);
	if(snd == NULL)
		return 0;
	SND_CreateChannel(chn, &al_snd_fmt, snd->length, snd->data);

	Channels[chn].Sound = n;

	return 1;
}

byte musbuffer[256 * 1024];
int current_song = -1;

typedef	struct
{
	word length,
	values[1];
} MusicGroup;

MusicGroup *music;
word *sqHack, *sqHackPtr, sqHackLen, sqHackSeqLen;
long sqHackTime;
long alTimeCount;

void SD_StopMusic(void)
{
	if(!adlib_on)
		return;
	WaitForSingleObject(adlib_in_use, 1000);
	SND_StopStream();
	ReleaseMutex(adlib_in_use);
}

void SD_PlayMusic(int song, bool force_restart)
{
	song = MUS_REMAP[song];
	last_song = song;
	if(!adlib_mus_on)
		return;
	if(SD_mode.mus == mus_none)
		return;
	if(song == current_song && !force_restart)
		return;
	WaitForSingleObject(adlib_in_use, 1000);
	SND_StopStream();
	current_song = -1;
	if(!File_AUD_ReadChunk(song+STARTMUSIC, musbuffer))
		return;
	music = (MusicGroup*)musbuffer;
	sqHackPtr = sqHack = music->values;
	sqHackSeqLen = sqHackLen = music->length;
	sqHackTime = 0;
	alTimeCount = 0;
	SND_PlayStream();
	current_song = song;
	ReleaseMutex(adlib_in_use);
}

void SD_PlayImf_f(void)
{
	FILE *fp;
	char *fname;
	int len;

	if(!adlib_mus_on)
	{
		Con_Printf("Can't play music.\n");
		return;
	}

	if(Cmd_Argc() == 1)
	{
		Con_Printf("use: playimf <filename>\n");
		return;
	}
	fname = va("music/%s", Cmd_Argv(1));
	len = File_COM_FileOpenRead(FS_ExpandFilename(fname), &fp);
	if(len == -1)
	{
		Con_Printf("File %s not found\n", fname);
		return;
	}
	if(len > sizeof(musbuffer))
	{
		Con_Printf("File %s is too large\n", fname);
		fclose(fp);
		return;
	}

	WaitForSingleObject(adlib_in_use, 1000);
	SND_StopStream();
	current_song = -1;
	fread(musbuffer, 1, len, fp);
	fclose(fp);
	music = (MusicGroup*)musbuffer;
	sqHackPtr = sqHack = music->values;
	sqHackSeqLen = sqHackLen = music->length;
	sqHackTime = 0;
	alTimeCount = 0;
	SND_PlayStream();
	current_song = -1;
	ReleaseMutex(adlib_in_use);
}

int UpdateMusic(int pos, int size, void *buffer)
{
	byte *al;
	int n;
	int unity = 0;
	short *ptr;
	int AdLibTicks;
	DWORD mut_ret;

	if(size == ADLIB_MUS_BYPS)
		AdLibTicks = 700;
	else
		AdLibTicks = (size / ADLIB_MUS_BYPS) * 700;

	if(!adlib_on)
		return 0;
	mut_ret = WaitForSingleObject(adlib_in_use, 1000);
	if(mut_ret != WAIT_OBJECT_0)
		return 0;
	ptr = (short *)buffer;

	for(n = 0; n < AdLibTicks; n++)
	{
		while(sqHackLen && (sqHackTime <= alTimeCount))
		{
			al = (byte *)sqHackPtr++;
			sqHackTime = alTimeCount + *sqHackPtr++;
			OPLWrite(hAdLibMus, al[0], al[1]);
			sqHackLen -= 4;
		}
		alTimeCount++;
		if(!sqHackLen)
		{
			sqHackPtr = sqHack;
			sqHackLen = sqHackSeqLen;
			alTimeCount = sqHackTime = 0;
		}

		if(ADLIB_MUS_SPEED == 44100)
		{
			YM3812UpdateOne(hAdLibMus, ptr, 63);
			ptr += 63;
		}
		else
		{
			YM3812UpdateOne(hAdLibMus, ptr, 31 + unity);
			ptr += 31 + unity;
			unity = 1 - unity;
		}

	}

	ReleaseMutex(adlib_in_use);
	return 1;
}

int SD_ChooseChannel(soundnames SoundNum, int channel)
{
	int n, chs0 = -1, chs1 = -1, chs2 = -1;
	int Pos, MaxPos0 = 0, MaxPos2 = 0;

	if(channel != CHAN_AUTO)
	{
		if(Channels[channel].Sound != SoundNum)
			if(!SD_LoadIdSound(SoundNum, channel))
				return -1;
		return channel;
	}

	for(n = CHAN_FIRST; n < MAX_CHANNELS; n++)
	{
		if(Channels[n].Sound == -1)
		{
			chs1 = n;
			continue;
		}
		if(Channels[n].Sound == SoundNum)
		{
			if((Pos = SND_GetPos(n)) == 1000)
				return n;
			if (Pos >= MaxPos0)
			{
				MaxPos0 = Pos;
				chs0 = n;
			}
		}
		else
		{
			if((Pos = SND_GetPos(n)) == 1000)
			{
				if(chs1 == -1)
					chs1 = n;
				continue;
			}
			if (Pos >= MaxPos2)
			{
				MaxPos2 = Pos;
				chs2 = n;
			}
		}
	}

	if(chs1 != -1)
	{
		if(!SD_LoadIdSound(SoundNum, chs1))
			return -1;
		return chs1;
	}

	if(chs2 == -1 && chs0 == -1)
		return -1;
	if(MaxPos0 >= MaxPos2)
	{
		return chs0;
	}
	else
	{
		if(!SD_LoadIdSound(SoundNum, chs2))
			return -1;
		return chs2;
	}
}

#define ATABLEMAX 15
char righttable[ATABLEMAX][ATABLEMAX * 2] =
{
{ 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 7, 7, 7, 6, 0, 0, 0, 0, 0, 1, 3, 5, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 7, 7, 6, 4, 0, 0, 0, 0, 0, 2, 4, 6, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 7, 6, 6, 4, 1, 0, 0, 0, 1, 2, 4, 6, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 7, 6, 5, 4, 2, 1, 0, 1, 2, 3, 5, 7, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 6, 5, 4, 3, 2, 2, 3, 3, 5, 6, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 6, 6, 5, 4, 4, 4, 4, 5, 6, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 6, 6, 5, 5, 5, 6, 6, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 6, 6, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8}
};

char lefttable[ATABLEMAX][ATABLEMAX * 2] =
{
{ 8, 8, 8, 8, 8, 8, 8, 8, 5, 3, 1, 0, 0, 0, 0, 0, 6, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 6, 4, 2, 0, 0, 0, 0, 0, 4, 6, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 6, 4, 2, 1, 0, 0, 0, 1, 4, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 7, 5, 3, 2, 1, 0, 1, 2, 4, 5, 6, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 6, 5, 3, 3, 2, 2, 3, 4, 5, 6, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 7, 6, 5, 4, 4, 4, 4, 5, 6, 6, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7, 6, 6, 5, 5, 5, 6, 6, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7, 7, 6, 6, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
{ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8}
};

void SD_SetSoundLoc(int gx, int gy)
{
	int xt, yt;
	int x, y;

	gx -= Player.position.origin[0];
	gy -= Player.position.origin[1];
	gy = -gy;

	xt = (int)(gx * CosTable[Player.position.angle]);
	yt = (int)(gy * SinTable[Player.position.angle]);
	x = POS2TILE(xt + yt);

	xt = (int)(gx * SinTable[Player.position.angle]);
	yt = (int)(gy * CosTable[Player.position.angle]);
	y = POS2TILE(yt + xt);

	if (y >= ATABLEMAX)
		y = ATABLEMAX - 1;
	else if(y <= -ATABLEMAX)
		y = -ATABLEMAX;
	if (x < 0)
		x = -x;
	if (x >= ATABLEMAX)
		x = ATABLEMAX - 1;

	leftchannel = lefttable[x][y + ATABLEMAX];
	rightchannel = righttable[x][y + ATABLEMAX];
}

#define MAX_CACHED_SNDS	128
cache_snd cachesnds[MAX_CACHED_SNDS];
int numcachesnds = 0;

cache_snd *SD_CacheSND(int name)
{
	cache_snd *snd;
	int i;

	for(snd = cachesnds, i = 0; i < numcachesnds; snd++, i++)
		if(name == snd->name)
			return snd;

	if(numcachesnds == MAX_CACHED_SNDS)
	{
		Con_Printf("Warning! numcachesnds == MAX_CACHED_SNDS\n");
		return NULL;
	}

	numcachesnds++;
	snd->name = name;
	SD_SynthALSound(snd);
	return snd;
}

void SD_UnCacheSnd(int name)
{
	cache_snd *snd;
	int i;

	for(snd = cachesnds, i = 0; i < numcachesnds; snd++, i++)
		if(name == snd->name)
			break;
	if(name != snd->name)
		return;
	if(snd->data)
		free(snd->data);
	memmove(snd, snd + 1, (byte*)&cachesnds[--numcachesnds] - (byte*)snd);
}

void SD_UnCacheAllSnds(void)
{
	cache_snd *snd;
	int i;

	for(snd = cachesnds, i = 0; i < numcachesnds; snd++, i++)
		if(snd->data)
			free(snd->data);
	numcachesnds = 0;
}

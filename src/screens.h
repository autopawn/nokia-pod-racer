#ifndef SCREENS_H
#define SCREENS_H


//----------------------------------------------------------------------------------
// Nokia screen details
//----------------------------------------------------------------------------------
#define SCREEN_BORDER 24
#define SCREEN_SCALE_MULT 8
#define SCREEN_W 84
#define SCREEN_H 48
#define SCREEN_COLOR_BG (Color){0x87, 0x91, 0x88, 0xff}
#define SCREEN_COLOR_LIT (Color){0x1a, 0x19, 0x14, 0xff}

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef enum GameScreen { UNKNOWN = -1, LOGO = 0, HAREMONIC, TITLE, OPTIONS, GAMEPLAY, ENDING } GameScreen;

typedef enum LevelArea {
    LEVEL_CITY = 0,
    LEVEL_FOREST = 1,
    LEVEL_LIGHTS = 2,
} LevelArea;

#define LEVEL_COUNT 3

//----------------------------------------------------------------------------------
// Global Variables Declaration (shared by several modules)
//----------------------------------------------------------------------------------
extern GameScreen currentScreen;
extern LevelArea currentLevel;
extern Font font;
extern Music music;
extern Sound fxCoin;
extern int lastGameTime;
extern bool lastGameComplete;

#ifdef __cplusplus
extern "C" {            // Prevents name mangling of functions
#endif

//----------------------------------------------------------------------------------
// Raylib Logo Screen Functions Declaration
//----------------------------------------------------------------------------------
void InitLogoScreen(void);
void UpdateLogoScreen(void);
void DrawLogoScreen(void);
void UnloadLogoScreen(void);
int FinishLogoScreen(void);

//----------------------------------------------------------------------------------
// Haremonic Logo Screen Functions Declaration
//----------------------------------------------------------------------------------
void InitHaremonicScreen(void);
void UpdateHaremonicScreen(void);
void DrawHaremonicScreen(void);
void UnloadHaremonicScreen(void);
int FinishHaremonicScreen(void);

//----------------------------------------------------------------------------------
// Title Screen Functions Declaration
//----------------------------------------------------------------------------------
void InitTitleScreen(void);
void UpdateTitleScreen(void);
void DrawTitleScreen(void);
void UnloadTitleScreen(void);
int FinishTitleScreen(void);

//----------------------------------------------------------------------------------
// Options Screen Functions Declaration
//----------------------------------------------------------------------------------
void InitOptionsScreen(void);
void UpdateOptionsScreen(void);
void DrawOptionsScreen(void);
void UnloadOptionsScreen(void);
int FinishOptionsScreen(void);

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Declaration
//----------------------------------------------------------------------------------
void InitGameplayScreen(void);
void UpdateGameplayScreen(void);
void DrawGameplayScreen(void);
void UnloadGameplayScreen(void);
int FinishGameplayScreen(void);

//----------------------------------------------------------------------------------
// Ending Screen Functions Declaration
//----------------------------------------------------------------------------------
void InitEndingScreen(void);
void UpdateEndingScreen(void);
void DrawEndingScreen(void);
void UnloadEndingScreen(void);
int FinishEndingScreen(void);

//----------------------------------------------------------------------------------
// Utility functions
//----------------------------------------------------------------------------------
static inline int mod(int a, int b)
{
    int r = a % b;
    return r < 0 ? r + b : r;
}

static inline float absf(float a)
{
    return a < 0 ? -a : a;
}

static inline float maxf(float a, float b)
{
    return a > b ? a : b;
}

#ifdef __cplusplus
}
#endif

#endif // SCREENS_H

#include "raylib.h"
#include "screens.h"

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;
static const int DURATION = 100;

static Texture2D haremonicLogo;

//----------------------------------------------------------------------------------
// Haremonic Screen Functions Definition
//----------------------------------------------------------------------------------

// Haremonic Screen Initialization logic
void InitHaremonicScreen(void)
{
    finishScreen = 0;
    framesCounter = 0;

    haremonicLogo = LoadTexture("resources/logo_haremonic.png");
}

// Haremonic Screen Update logic
void UpdateHaremonicScreen(void)
{
    framesCounter++;

    if (framesCounter >= DURATION)
        finishScreen = true;
}

// Haremonic Screen Draw logic
void DrawHaremonicScreen(void)
{
    DrawTexture(haremonicLogo, 0, 0, WHITE);

    int move_x = (SCREEN_W + SCREEN_H)*(DURATION*DURATION - (framesCounter - DURATION)*(framesCounter - DURATION))
            /(DURATION*DURATION);

    DrawLine(move_x + 0, 0, move_x + 0 - SCREEN_H, SCREEN_H, SCREEN_COLOR_BG);
    DrawLine(move_x + 1, 0, move_x + 1 - SCREEN_H, SCREEN_H, SCREEN_COLOR_BG);
    DrawLine(move_x + 2, 0, move_x + 2 - SCREEN_H, SCREEN_H, SCREEN_COLOR_BG);
    DrawLine(move_x + 3, 0, move_x + 3 - SCREEN_H, SCREEN_H, SCREEN_COLOR_BG);
    DrawLine(move_x + 6, 0, move_x + 6 - SCREEN_H, SCREEN_H, SCREEN_COLOR_BG);
    DrawLine(move_x + 7, 0, move_x + 7 - SCREEN_H, SCREEN_H, SCREEN_COLOR_BG);
}

// Haremonic Screen Unload logic
void UnloadHaremonicScreen(void)
{
    UnloadTexture(haremonicLogo);
}

// Haremonic Screen should finish?
int FinishHaremonicScreen(void)
{
    return finishScreen;
}

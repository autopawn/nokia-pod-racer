/**********************************************************************************************
*
*   raylib - Advance Game template
*
*   Ending Screen Functions Definitions (Init, Update, Draw, Unload)
*
*   Copyright (c) 2014-2022 Ramon Santamaria (@raysan5)
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

#include "raylib.h"
#include "screens.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;

//----------------------------------------------------------------------------------
// Ending Screen Functions Definition
//----------------------------------------------------------------------------------

// Ending Screen Initialization logic
void InitEndingScreen(void)
{
    // TODO: Initialize ENDING screen variables here!
    framesCounter = 0;
    finishScreen = 0;
}

// Ending Screen Update logic
void UpdateEndingScreen(void)
{
    if (GetKeyPressed())
    {
        finishScreen = 1;
        PlaySound(fxCoin);
    }
}

// Ending Screen Draw logic
void DrawEndingScreen(void)
{
    const int font_size = 8;

    DrawRectangleLines(1, 1, SCREEN_W - 2, SCREEN_H - 2, SCREEN_COLOR_LIT);

    char buffer[200];
    int w;

    if (!lastGameComplete)
    {
        sprintf(buffer, "You crashed!");
        w = MeasureText(buffer, font_size);
        DrawText(buffer, SCREEN_W/2 - w/2, 12, font_size, SCREEN_COLOR_LIT);
    }
    else
    {
        sprintf(buffer, "Complete!");
        w = MeasureText(buffer, font_size);
        DrawText(buffer, SCREEN_W/2 - w/2, 12, font_size, SCREEN_COLOR_LIT);

        sprintf(buffer, "Time: %02d:%02d", lastGameComplete/60, lastGameComplete%60);
        w = MeasureText(buffer, font_size);
        DrawText(buffer, SCREEN_W/2 - w/2, 32, font_size, SCREEN_COLOR_LIT);
    }
}

// Ending Screen Unload logic
void UnloadEndingScreen(void)
{
    // TODO: Unload ENDING screen variables here!
}

// Ending Screen should finish?
int FinishEndingScreen(void)
{
    return finishScreen;
}

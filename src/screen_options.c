/**********************************************************************************************
*
*   raylib - Advance Game template
*
*   Options Screen Functions Definitions (Init, Update, Draw, Unload)
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

#include <stdio.h>

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;
static int lastJoyMovementFrame = 0;

static const char *levelNames[LEVEL_COUNT] =
{
    "City",
    "Forest",
    "Lights",
    "Ice",
};

//----------------------------------------------------------------------------------
// Options Screen Functions Definition
//----------------------------------------------------------------------------------

// Options Screen Initialization logic
void InitOptionsScreen(void)
{
    framesCounter = 0;
    finishScreen = 0;
    lastJoyMovementFrame = -10;
}

// Options Screen Update logic
void UpdateOptionsScreen(void)
{
    framesCounter++;

    if (lastJoyMovementFrame + 10 <= framesCounter)
    {
        if (GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) > 0.5)
        {
            currentLevel = (currentLevel + 1) % LEVEL_COUNT;
            lastJoyMovementFrame = framesCounter;
        }
        if (GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) < -0.5)
        {
            currentLevel = (currentLevel + LEVEL_COUNT - 1) % LEVEL_COUNT;
            lastJoyMovementFrame = framesCounter;
        }
    }

    if (IsKeyPressed(KEY_DOWN) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN))
        currentLevel = (currentLevel + 1) % LEVEL_COUNT;
    if (IsKeyPressed(KEY_UP) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_UP))
        currentLevel = (currentLevel + LEVEL_COUNT - 1) % LEVEL_COUNT;

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_Z) || IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN))
        finishScreen = true;
}

// Options Screen Draw logic
void DrawOptionsScreen(void)
{
    DrawText("- Level Select -", 0, -1, 8, SCREEN_COLOR_LIT);
    for (int i = 0; i < LEVEL_COUNT; ++i)
    {
        if (i == currentLevel)
        {
            DrawRectangle(0, 10*i + 8, SCREEN_W, 10, SCREEN_COLOR_LIT);
            DrawText(levelNames[i], 1, 10*i + 8, 8, SCREEN_COLOR_BG);
        }
        else
        {
            DrawText(levelNames[i], 1, 10*i + 8, 8, SCREEN_COLOR_LIT);
        }

        if (persistentData.time[i] != 0)
        {
            char buffer[200];

            sprintf(buffer, "%02d:%02d", persistentData.time[i]/60, persistentData.time[i]%60);
            int w = MeasureText(buffer, 8);
            DrawText(buffer, SCREEN_W - w - 1, 10*i + 8, 8, i == currentLevel ? SCREEN_COLOR_BG : SCREEN_COLOR_LIT);
        }
    }
}

// Options Screen Unload logic
void UnloadOptionsScreen(void)
{
}

// Options Screen should finish?
int FinishOptionsScreen(void)
{
    return finishScreen;
}

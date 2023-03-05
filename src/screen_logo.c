/**********************************************************************************************
*
*   raylib - Advance Game template
*
*   Logo Screen Functions Definitions (Init, Update, Draw, Unload)
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

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;

static int logoPositionX = 0;
static int logoPositionY = 0;

static int lettersCount = 0;

static int topSideRecWidth = 0;
static int leftSideRecHeight = 0;

static int bottomSideRecWidth = 0;
static int rightSideRecHeight = 0;

static int state = 0;              // Logo animation states

//----------------------------------------------------------------------------------
// Logo Screen Functions Definition
//----------------------------------------------------------------------------------

// Logo Screen Initialization logic
void InitLogoScreen(void)
{
    finishScreen = 0;
    framesCounter = 0;
    lettersCount = 0;

    logoPositionX = SCREEN_W/2 - 18;
    logoPositionY = 12;

    topSideRecWidth = 0;
    leftSideRecHeight = 0;
    bottomSideRecWidth = 0;
    rightSideRecHeight = 0;

    state = 0;
}

// Logo Screen Update logic
void UpdateLogoScreen(void)
{
    if (state == 0)                 // State 0: Top-left square corner blink logic
    {
        framesCounter++;

        if (framesCounter == 24)
        {
            state = 1;
            framesCounter = 0;      // Reset counter... will be used later...
        }
    }
    else if (state == 1)            // State 1: Bars animation logic: top and left
    {
        topSideRecWidth += 2;
        leftSideRecHeight += 2;

        if (topSideRecWidth == 34) state = 2;
    }
    else if (state == 2)            // State 2: Bars animation logic: bottom and right
    {
        bottomSideRecWidth += 2;
        rightSideRecHeight += 2;

        if (bottomSideRecWidth == 34) state = 3;
    }
    else if (state == 3)            // State 3: "raylib" text-write animation logic
    {
        framesCounter++;

        if (lettersCount < 8)
        {
            if (framesCounter/2)   // Every 2 frames, one more letter!
            {
                lettersCount++;
                framesCounter = 0;
            }
        }
        else    // When all letters have appeared, just fade out everything
        {
            if (framesCounter > 60)
                finishScreen = 1;   // Jump to next screen
        }
    }
}

// Logo Screen Draw logic
void DrawLogoScreen(void)
{
    if (state == 0)         // Draw blinking top-left square corner
    {
        if ((framesCounter/4)%2)
            DrawRectangle(logoPositionX, logoPositionY, 1, 1, SCREEN_COLOR_LIT);
    }
    else if (state == 1)    // Draw bars animation: top and left
    {
        DrawRectangle(logoPositionX, logoPositionY, topSideRecWidth, 1, SCREEN_COLOR_LIT);
        DrawRectangle(logoPositionX, logoPositionY, 1, leftSideRecHeight, SCREEN_COLOR_LIT);
    }
    else if (state == 2)    // Draw bars animation: bottom and right
    {
        DrawRectangle(logoPositionX, logoPositionY, topSideRecWidth, 1, SCREEN_COLOR_LIT);
        DrawRectangle(logoPositionX, logoPositionY, 1, leftSideRecHeight, SCREEN_COLOR_LIT);

        DrawRectangle(logoPositionX + 33, logoPositionY, 1, rightSideRecHeight, SCREEN_COLOR_LIT);
        DrawRectangle(logoPositionX, logoPositionY + 33, bottomSideRecWidth, 1, SCREEN_COLOR_LIT);
    }
    else if (state == 3)    // Draw "raylib" text-write animation + "powered by"
    {
        DrawRectangle(logoPositionX, logoPositionY, topSideRecWidth, 1, SCREEN_COLOR_LIT);
        DrawRectangle(logoPositionX, logoPositionY + 1, 1, leftSideRecHeight - 2, SCREEN_COLOR_LIT);

        DrawRectangle(logoPositionX + 33, logoPositionY + 1, 1, rightSideRecHeight - 2, SCREEN_COLOR_LIT);
        DrawRectangle(logoPositionX, logoPositionY + 33, bottomSideRecWidth, 1, SCREEN_COLOR_LIT);

        DrawText(TextSubtext("raylib", 0, lettersCount), logoPositionX + 4, logoPositionY + 22, 8, SCREEN_COLOR_LIT);

        if (lettersCount > 6)
            DrawText("powered by", logoPositionX - 20, logoPositionY - 11, 6, SCREEN_COLOR_LIT);
    }
}

// Logo Screen Unload logic
void UnloadLogoScreen(void)
{
    // Unload LOGO screen variables here!
}

// Logo Screen should finish?
int FinishLogoScreen(void)
{
    return finishScreen;
}

/**********************************************************************************************
*
*   raylib - Advance Game template
*
*   Title Screen Functions Definitions (Init, Update, Draw, Unload)
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

#include <stdlib.h>

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;
static Texture2D textureBunny;
static Texture2D textureBunnyAlt;
static Texture2D texturePod;
static Texture2D textureText;

//----------------------------------------------------------------------------------
// Title Screen Functions Definition
//----------------------------------------------------------------------------------

// Title Screen Initialization logic
void InitTitleScreen(void)
{
    // TODO: Initialize TITLE screen variables here!
    framesCounter = 0;
    finishScreen = 0;

    textureBunny = LoadTexture("resources/title_bunny.png");
    textureBunnyAlt = LoadTexture("resources/title_bunny_alt.png");
    texturePod = LoadTexture("resources/title_pod.png");
    textureText = LoadTexture("resources/title_text.png");
}

// Title Screen Update logic
void UpdateTitleScreen(void)
{
    framesCounter++;

    if (GetKeyPressed())
    {
        finishScreen = 1;
        PlaySound(fxCoin);
    }
}

// Title Screen Draw logic
void DrawTitleScreen(void)
{
    if (framesCounter >= 60)
    {
        int delta_x = (rand()%5 - 2)/2;
        int delta_y = (rand()%5 - 2)/2;

        DrawTexture(textureText, 0, 0, WHITE);
        DrawTexture(textureText, delta_x, delta_y, WHITE);
        DrawTexture(textureText, -delta_x, -delta_y, WHITE);
        DrawTexture(texturePod, 0, 0, WHITE);

        if (framesCounter < 160)
            DrawTexture(textureBunny, 0, 0, WHITE);
        else
            DrawTexture(textureBunnyAlt, 0, 0, WHITE);
    }
    else
    {
        int offset = 80*(60 - framesCounter)*(60 - framesCounter)/(60*60);

        DrawTexture(texturePod, offset, 0, WHITE);
        DrawTexture(textureBunny, -offset, 0, WHITE);
    }

}

// Title Screen Unload logic
void UnloadTitleScreen(void)
{
    UnloadTexture(textureBunny);
    UnloadTexture(textureBunnyAlt);
    UnloadTexture(texturePod);
    UnloadTexture(textureText);
}

// Title Screen should finish?
int FinishTitleScreen(void)
{
    return finishScreen;
}

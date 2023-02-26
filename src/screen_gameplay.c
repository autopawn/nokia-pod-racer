/**********************************************************************************************
*
*   raylib - Advance Game template
*
*   Gameplay Screen Functions Definitions (Init, Update, Draw, Unload)
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

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;

static Texture2D textureDriver;

typedef struct
{
    float x, y;
    int turbo_l, turbo_r;
} Player;

static Player player;
//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------

static void DrawTile(Texture2D tex, int tile_size_x, int tile_size_y, int tile_x, int tile_y,
        int pos_x, int pos_y)
{
    Rectangle src = {tile_size_x*tile_x, tile_size_y*tile_y, tile_size_x, tile_size_y};

    DrawTextureRec(tex, src, (Vector2){pos_x, pos_y}, WHITE);
}

// Gameplay Screen Initialization logic
void InitGameplayScreen(void)
{
    framesCounter = 0;
    finishScreen = 0;

    textureDriver = LoadTexture("resources/driver.png");

    memset(&player, 0, sizeof(player));
}

// Gameplay Screen Update logic
void UpdateGameplayScreen(void)
{
    if (IsKeyDown(KEY_A))
        player.turbo_l = 2;
    else if (IsKeyDown(KEY_Z))
        player.turbo_l = 1;
    else
        player.turbo_l = 0;

    if (IsKeyDown(KEY_K))
        player.turbo_r = 2;
    else if (IsKeyDown(KEY_M))
        player.turbo_r = 1;
    else
        player.turbo_r = 0;

    // // Press enter or tap to change to ENDING screen
    // if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
    // {
    //     finishScreen = 1;
    //     PlaySound(fxCoin);
    // }
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void)
{
    DrawTile(textureDriver, 12, 12, player.turbo_l, player.turbo_r, 36, 36);
    DrawTile(textureDriver, 12, 12, 3, player.turbo_l, 36 - 10, 36);
    DrawTile(textureDriver, 12, 12, 4, player.turbo_r, 36 + 10, 36);
}

// Gameplay Screen Unload logic
void UnloadGameplayScreen(void)
{
    UnloadTexture(textureDriver);
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void)
{
    return finishScreen;
}

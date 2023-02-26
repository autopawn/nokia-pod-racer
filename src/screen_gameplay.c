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
#include "raymath.h"
#include "screens.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;

static Texture2D textureDriver;
static Texture2D textureBackground;

typedef struct
{
    float ang, ang_spd;

    Vector3 pos, pos_spd;

    int turbo_l, turbo_r;
} Player;

static Player player;

static float absf(float a)
{
    return a < 0 ? -a : a;
}

static void UpdatePlayer(Player *player)
{
    // Target velocity
    float tgt_ang_spd = (player->turbo_r - player->turbo_l) * 0.04;
    float tgt_front_spd = (player->turbo_l + player->turbo_r - 0.4*absf(player->turbo_r - player->turbo_l)) * 0.1;
    Vector3 tgt_spd = (Vector3){tgt_front_spd*cosf(player->ang), -10, -tgt_front_spd*sinf(player->ang)};

    // Accelerate towards target velocity (not phyisically accurate at all)
    player->ang_spd = 0.9 * player->ang_spd + 0.1 * tgt_ang_spd;
    player->pos_spd = Vector3Add(Vector3Scale(player->pos_spd, 0.9), Vector3Scale(tgt_spd, 0.1));

    // Move according to speed
    player->ang += player->ang_spd;
    player->pos = Vector3Add(player->pos, player->pos_spd);

    if (player->pos.y < 0)
    {
        player->pos.y = 0;
        player->pos_spd.y = 0;
    }
}

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
    textureBackground = LoadTexture("resources/night_city.png");

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

    UpdatePlayer(&player);

    // // Press enter or tap to change to ENDING screen
    // if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
    // {
    //     finishScreen = 1;
    //     PlaySound(fxCoin);
    // }
}

static void DrawBorderedCube(Vector3 position, float width, float height, float length)
{
    DrawCube(position, width, height, length, SCREEN_COLOR_BG);

    BoundingBox box;
    box.min.x = position.x - 0.5*width - 0.02;
    box.max.x = position.x + 0.5*width + 0.02;
    box.min.y = position.y - 0.5*height - 0.02;
    box.max.y = position.y + 0.5*height + 0.02;
    box.min.z = position.z - 0.5*length - 0.02;
    box.max.z = position.z + 0.5*length + 0.02;

    DrawBoundingBox(box, SCREEN_COLOR_LIT);
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void)
{
    const float camera_y = 0.8;
    const float camera_d = 3.0;
    const float camera_behind = 1.0;

    Vector3 player_pointing = Vector3RotateByAxisAngle((Vector3){1,0,0}, (Vector3){0,1,0}, player.ang);

    Camera camera = { 0 };
    camera.position = Vector3Subtract(Vector3Add(player.pos, (Vector3){0, camera_y, 0}), Vector3Scale(player_pointing, camera_behind));
    camera.target = Vector3Add(player.pos, Vector3Scale(player_pointing, camera_d));
    camera.up = (Vector3){0, 1, 0};
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;


    int background_x = (int) roundf(-player.ang / (2 * PI) * textureBackground.width);
    background_x = mod(background_x, textureBackground.width);

    DrawTexture(textureBackground, -background_x, 0, WHITE);
    DrawTexture(textureBackground, -background_x + textureBackground.width, 0, WHITE);

    BeginMode3D(camera);
        DrawBorderedCube((Vector3){10, 0.5, 0}, 1, 1, 1);

    EndMode3D();

    DrawTile(textureDriver, 12, 12, player.turbo_l, player.turbo_r, 36, 34);
    DrawTile(textureDriver, 12, 12, 3, player.turbo_l, 36 - 10, 34);
    DrawTile(textureDriver, 12, 12, 4, player.turbo_r, 36 + 10, 34);
}

// Gameplay Screen Unload logic
void UnloadGameplayScreen(void)
{
    UnloadTexture(textureDriver);
    UnloadTexture(textureBackground);
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void)
{
    return finishScreen;
}

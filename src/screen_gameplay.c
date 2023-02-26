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

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

static const int MAP_SIZE = 1000;
static const int N_MAP_OBSTACLES = 4000;
static const int PLAYER_DEATH_ANIMATION_TIME = 200;
static const float CARROT_SPAN_DIST = 300;
static const int UI_FONT_SIZE = 8;

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;

static Texture2D textureDriver;
static Texture2D textureBackground;

static Sound fxBreak;

typedef struct
{
    float ang, ang_spd;

    Vector3 pos, pos_spd;

    int turbo_l, turbo_r;

    int time_death;
} Player;

typedef struct
{
    bool type;
    Vector3 pos;
} Obstacle;

typedef struct
{
    Obstacle *objs;
    unsigned int objs_count;

    Vector3 carrot_pos;
} Level;

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------

static bool LevelCheckCollision(const Level *level, Vector3 point, float rad)
{
    for (int i = 0; i < level->objs_count; ++i)
    {
        if (point.x + rad <= level->objs[i].pos.x - 0.5)
            continue;
        if (point.x - rad >= level->objs[i].pos.x + 0.5)
            continue;
        if (point.z + rad <= level->objs[i].pos.z - 0.5)
            continue;
        if (point.z - rad >= level->objs[i].pos.z + 0.5)
            continue;

        return true;
    }
    return false;
}

static void LevelRespawnCarrot(Level *level, const Player *player)
{
    assert(CARROT_SPAN_DIST < 0.9 * MAP_SIZE);

    while(1)
    {
        float angle = 2*PI*(rand() % 30000)/30000.0f;

        float pos_x = player->pos.x + CARROT_SPAN_DIST * cosf(angle);
        float pos_z = player->pos.z + CARROT_SPAN_DIST * sinf(angle);

        if (0 < pos_x && pos_x < MAP_SIZE && 0 < pos_z && pos_z < MAP_SIZE)
        {
            level->carrot_pos = (Vector3){pos_x, 0, pos_z};

            if (!LevelCheckCollision(level, level->carrot_pos, 2))
                break;
        }
    }
}

static Level *LevelGenerate()
{
    Level *level = MemAlloc(sizeof(*level));
    assert(level);

    level->objs = MemAlloc(sizeof(*level->objs) * N_MAP_OBSTACLES);
    level->objs_count = 0;

    for (int i = 0; i < N_MAP_OBSTACLES; ++i)
    {
        Obstacle obs = {0};

        while (1)
        {
            int pos_x = rand()%MAP_SIZE;
            int pos_z = rand()%MAP_SIZE;

            obs.pos.x = pos_x;
            obs.pos.y = 0;
            obs.pos.z = pos_z;

            if (!LevelCheckCollision(level, obs.pos, 0.2))
                break;
        }

        level->objs[level->objs_count++] = obs;
    }

    return level;
}

static void UnloadLevel(Level *level)
{
    MemFree(level->objs);
    MemFree(level);
}

static void UpdatePlayer(Level *level, Player *player)
{
    const float PLAYER_RAD = 0.3;

    if (!player->time_death)
    {
        // React to controls
        if (IsKeyDown(KEY_A))
            player->turbo_l = 2;
        else if (IsKeyDown(KEY_Z))
            player->turbo_l = 1;
        else
            player->turbo_l = 0;

        if (IsKeyDown(KEY_K))
            player->turbo_r = 2;
        else if (IsKeyDown(KEY_M))
            player->turbo_r = 1;
        else
            player->turbo_r = 0;

        // Target velocity
        float tgt_ang_spd = (player->turbo_r - player->turbo_l) * 0.04;
        float tgt_front_spd = (player->turbo_l + player->turbo_r - 0.4*absf(player->turbo_r - player->turbo_l)) * 0.1;
        Vector3 tgt_spd = (Vector3){tgt_front_spd*cosf(player->ang), -10, -tgt_front_spd*sinf(player->ang)};

        // Accelerate towards target velocity (not phyisically accurate at all)
        player->ang_spd = 0.9 * player->ang_spd + 0.1 * tgt_ang_spd;
        player->pos_spd = Vector3Add(Vector3Scale(player->pos_spd, 0.9), Vector3Scale(tgt_spd, 0.1));
    }
    else
    {
        player->time_death++;

        player->pos_spd = Vector3Scale(player->pos_spd, 0.95);
        player->ang_spd *= 0.95;
    }

    // Collide with floor
    if (player->pos.y + player->pos_spd.y < 0)
    {
        player->pos.y = 0;
        player->pos_spd.y = 0;
    }

    // Mario Kart 64 collision
    Vector3 pos_spd_x = (Vector3){player->pos_spd.x, 0, 0};
    Vector3 pos_spd_z = (Vector3){0, 0, player->pos_spd.z};

    Vector3 old_pos_spd = player->pos_spd;

    if (LevelCheckCollision(level, Vector3Add(player->pos, player->pos_spd), PLAYER_RAD))
    {

        // Remove one speed component
        if (absf(player->pos_spd.x) >= absf(player->pos_spd.y))
        {
            if (LevelCheckCollision(level, Vector3Add(player->pos, pos_spd_x), PLAYER_RAD))
                player->pos_spd.x = 0;
            else if (LevelCheckCollision(level, Vector3Add(player->pos, pos_spd_z), PLAYER_RAD))
                player->pos_spd.z = 0;
        }
        else
        {
            if (LevelCheckCollision(level, Vector3Add(player->pos, pos_spd_z), PLAYER_RAD))
                player->pos_spd.z = 0;
            else if (LevelCheckCollision(level, Vector3Add(player->pos, pos_spd_x), PLAYER_RAD))
                player->pos_spd.x = 0;
        }

        // Halt
        if (LevelCheckCollision(level, Vector3Add(player->pos, player->pos_spd), PLAYER_RAD))
        {
            player->pos_spd.x = 0;
            player->pos_spd.z = 0;
        }

        if (!player->time_death)
        {
            // Is collision fatal?
            float collision_magnitude = Vector3Distance(player->pos_spd, old_pos_spd);

            if (collision_magnitude > 0.15)
            {
                player->time_death += 1;
                StopMusicStream(music);
                PlaySound(fxBreak);
            }
        }
    }

    // Move according to speed
    player->ang += player->ang_spd;
    player->ang = fmodf(player->ang, 2*PI);
    player->pos = Vector3Add(player->pos, player->pos_spd);
}

static void DrawTile(Texture2D tex, int tile_size_x, int tile_size_y, int tile_x, int tile_y,
        int pos_x, int pos_y)
{
    Rectangle src = {tile_size_x*tile_x, tile_size_y*tile_y, tile_size_x, tile_size_y};

    DrawTextureRec(tex, src, (Vector2){pos_x, pos_y}, WHITE);
}

static void DrawTextOutline(int pos_x, int pos_y, const char *text)
{
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            if (x == 0 && y == 0)
                continue;
            DrawText(text, pos_x - x, pos_y - y, UI_FONT_SIZE, SCREEN_COLOR_LIT);
        }
    }
    DrawText(text, pos_x, pos_y, UI_FONT_SIZE, SCREEN_COLOR_BG);
}

//----------------------------------------------------------------------------------
// Local variables only for Gameplay Screen Functions
//----------------------------------------------------------------------------------

static Level *level;
static Player player;

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------

// Gameplay Screen Initialization logic
void InitGameplayScreen(void)
{
    framesCounter = 0;
    finishScreen = 0;

    textureDriver = LoadTexture("resources/driver.png");
    textureBackground = LoadTexture("resources/night_city.png");

    level = LevelGenerate();
    memset(&player, 0, sizeof(player));
    player.pos.x = -10;
    player.pos.z = MAP_SIZE/2.0;
    player.pos.y = 200;

    LevelRespawnCarrot(level, &player);

    fxBreak = LoadSound("resources/break.mp3");

    PlayMusicStream(music);
}

// Gameplay Screen Update logic
void UpdateGameplayScreen(void)
{
    UpdatePlayer(level, &player);

    if (player.time_death >= PLAYER_DEATH_ANIMATION_TIME)
        finishScreen = 1;

    if (player.time_death > 0)
        StopMusicStream(music);

    // // Press enter or tap to change to ENDING screen
    // if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
    // {
    //     finishScreen = 1;
    //     PlaySound(fxCoin);
    // }
}

float CarrotAngle(const Level *level, const Player *player)
{
    float angle = atan2f(- (level->carrot_pos.z - player->pos.z), level->carrot_pos.x - player->pos.x);

    float delta = angle - player->ang;

    if (delta < -PI)
        return 2*PI + delta;

    if (delta > PI)
        return delta - 2*PI;

    return delta;
}

float CarrotDistance(const Level *level, const Player *player)
{
    return Vector3Distance(player->pos, level->carrot_pos);
}

static void DrawBorderedCube(Vector3 position, float width, float height, float length)
{
    DrawCube(position, width, height, length, SCREEN_COLOR_BG);

    BoundingBox box;
    box.min.x = position.x - 0.5*width - 0.022;
    box.max.x = position.x + 0.5*width + 0.022;
    box.min.y = position.y - 0.5*height - 0.022;
    box.max.y = position.y + 0.5*height + 0.022;
    box.min.z = position.z - 0.5*length - 0.022;
    box.max.z = position.z + 0.5*length + 0.022;

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

        for (int i = 0; i < level->objs_count; ++i)
        {
            DrawBorderedCube((Vector3){level->objs[i].pos.x , 1, level->objs[i].pos.z}, 1, 2, 1);
        }

        // Draw Carrot
        DrawBorderedCube((Vector3){level->carrot_pos.x , 0.3, level->carrot_pos.z}, 0.2, 0.2, 0.2);

    EndMode3D();

    if (!player.time_death)
    {
        DrawTile(textureDriver, 12, 12, player.turbo_l, player.turbo_r, 36, 34);
        DrawTile(textureDriver, 12, 12, 3, player.turbo_l, 36 - 10, 34);
        DrawTile(textureDriver, 12, 12, 4, player.turbo_r, 36 + 10, 34);

        // Carrot seeker
        float carrot_angle = CarrotAngle(level, &player);

        const char *text;

        text = "";
        if (carrot_angle > 0.1)
            text = "<";
        if (carrot_angle > 0.2)
            text = "<<";
        if (carrot_angle > 0.4)
            text = "<<<";
        DrawTextOutline(1, 24, text);

        text = "";
        if (carrot_angle < -0.1)
            text = ">";
        if (carrot_angle < -0.2)
            text = ">>";
        if (carrot_angle < -0.4)
            text = ">>>";
        DrawTextOutline(SCREEN_W - 4 * strlen(text), 24, text);

        char buffer[80];
        sprintf(buffer, "%dm", (int) roundf(CarrotDistance(level, &player)));
        int w = MeasureText(buffer, UI_FONT_SIZE);
        DrawTextOutline(SCREEN_W/2 - w/2, 1, buffer);
    }
    else
    {
        int anim = player.time_death * 12 / PLAYER_DEATH_ANIMATION_TIME;

        DrawTile(textureDriver, 12, 12, anim, 4, 36, 34);
        DrawTile(textureDriver, 12, 12, 3, 3, 36 - 10, 34);
        DrawTile(textureDriver, 12, 12, 4, 3, 36 + 10, 34);
    }
}

// Gameplay Screen Unload logic
void UnloadGameplayScreen(void)
{
    UnloadTexture(textureDriver);
    UnloadTexture(textureBackground);

    UnloadLevel(level);

    StopMusicStream(music);
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void)
{
    return finishScreen;
}

/*******************************************************************************************
*
*   Nokia Pod Racer
*   Game made for the 5th annual Nokia 3310 Jam, with the Nokia 3310 screen restrictions.
*
*   This game has been created using raylib (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2023 Francisco Casas (@autopawn)
*
********************************************************************************************/

#include <stdio.h>

#include "raylib.h"
#include "screens.h"    // NOTE: Declares global (extern) variables and screens functions
#include "web.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

//----------------------------------------------------------------------------------
// Shared Variables Definition (global)
// NOTE: Those variables are shared between modules through screens.h
//----------------------------------------------------------------------------------
GamePersistentData persistentData = {0};
GameScreen currentScreen = LOGO;
LevelArea currentLevel = LEVEL_CITY;
Font font = { 0 };
Music music = { 0 };
Sound fxCoin = { 0 };
int lastGameTime = { 0 };
bool lastGameComplete = { 0 };
bool isMusicOn = true;

//----------------------------------------------------------------------------------
// Local Variables Definition (local to this module)
//----------------------------------------------------------------------------------
static RenderTexture2D nokiaScreen;
static bool pixelSeparation = false;
static const int screenWidth = 2*SCREEN_BORDER + SCREEN_SCALE_MULT*SCREEN_W;
static const int screenHeight = 2*SCREEN_BORDER + SCREEN_SCALE_MULT*SCREEN_H;

// Required variables to manage screen transitions (fade-in, fade-out)
static int transAlpha = 0;
static int transLength = 8;
static bool onTransition = false;
static bool transFadeOut = false;
static int transFromScreen = -1;
static GameScreen transToScreen = UNKNOWN;
int triggerLeftAxis = -1, triggerRightAxis = -1;
bool triggerAxisDetected = false;

//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------
static void ChangeToScreen(int screen);     // Change to screen, no transition effect

static void TransitionToScreen(int screen); // Request transition to next screen
static void UpdateTransition(void);         // Update transition effect
static void DrawTransition(void);           // Draw transition effect (full-screen rectangle)

static void UpdateDrawFrame(void);          // Update and draw one frame


//----------------------------------------------------------------------------------
// Save and load game
//----------------------------------------------------------------------------------
bool SaveGame(void)
{
    #ifndef PLATFORM_WEB
        return SaveFileData("savegame.dat", (void *) &persistentData, sizeof(persistentData));
    #else
        return saveGameToIndexedDB((void*)&persistentData, sizeof(persistentData));
    #endif
}

bool LoadGame(void)
{
    unsigned int bytesRead;
    void *data;

    #ifndef PLATFORM_WEB
        data = LoadFileData("savegame.dat", &bytesRead);
    #else
        data = loadGameFromIndexedDB(&bytesRead);
    #endif

    if (data)
        persistentData = *(GamePersistentData *)data;

    return bytesRead;
}

static inline bool hareDetectTriggerAxis()
{
    triggerLeftAxis = -1;
    triggerRightAxis = -1;
    triggerAxisDetected = false;

    if (IsGamepadAvailable(0))
    {
        if (GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_TRIGGER) < -0.5)
            triggerLeftAxis = GAMEPAD_AXIS_LEFT_TRIGGER;
        if (GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_TRIGGER) < -0.5)
            triggerRightAxis = GAMEPAD_AXIS_RIGHT_TRIGGER;

        if (triggerLeftAxis == -1)
        {
            for (int i = 0; i < GetGamepadAxisCount(0); ++i)
            {
                if (i != triggerRightAxis && GetGamepadAxisMovement(0, i) < -0.5)
                {
                    triggerLeftAxis = i;
                    break;
                }
            }
        }

        if (triggerRightAxis == -1)
        {
            for (int i = 0; i < GetGamepadAxisCount(0); ++i)
            {
                if (i != triggerLeftAxis && GetGamepadAxisMovement(0, i) < -0.5)
                {
                    triggerRightAxis = i;
                    break;
                }
            }
        }

        if (triggerLeftAxis == -1 || triggerRightAxis == -1)
            return false;

        fprintf(stderr, "JOYSTICK: Detected triggers: %d %d\n", triggerLeftAxis, triggerRightAxis);
        triggerAxisDetected = true;
        return true;
    }
    return false;
}

//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //---------------------------------------------------------
    InitWindow(screenWidth, screenHeight, "raylib game template");

    InitAudioDevice();      // Initialize audio device

    // Load global data (assets that must be available in all screens, i.e. font)
    font = LoadFont("resources/mecha.png");
    music = LoadMusicStream("resources/music2.mp3");
    fxCoin = LoadSound("resources/coin.mp3");

    nokiaScreen = LoadRenderTexture(SCREEN_W, SCREEN_H);

    SetMusicVolume(music, isMusicOn);

    // Setup and init first screen
    currentScreen = LOGO;
    InitLogoScreen();

    LoadGame();

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetTargetFPS(60);       // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------


    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        UpdateDrawFrame();
    }
#endif

    // De-Initialization
    //--------------------------------------------------------------------------------------
    // Unload current screen data before closing
    switch (currentScreen)
    {
        case LOGO: UnloadLogoScreen(); break;
        case HAREMONIC: UnloadHaremonicScreen(); break;
        case TITLE: UnloadTitleScreen(); break;
        case OPTIONS: UnloadOptionsScreen(); break;
        case GAMEPLAY: UnloadGameplayScreen(); break;
        case ENDING: UnloadEndingScreen(); break;
        default: break;
    }

    // Unload global data loaded
    UnloadFont(font);
    UnloadMusicStream(music);
    UnloadSound(fxCoin);
    UnloadRenderTexture(nokiaScreen);

    CloseAudioDevice();     // Close audio context

    CloseWindow();          // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

//----------------------------------------------------------------------------------
// Module specific Functions Definition
//----------------------------------------------------------------------------------
// Change to next screen, no transition
static void ChangeToScreen(GameScreen screen)
{
    // Unload current screen
    switch (currentScreen)
    {
        case LOGO: UnloadLogoScreen(); break;
        case HAREMONIC: UnloadHaremonicScreen(); break;
        case TITLE: UnloadTitleScreen(); break;
        case OPTIONS: UnloadOptionsScreen(); break;
        case GAMEPLAY: UnloadGameplayScreen(); break;
        case ENDING: UnloadEndingScreen(); break;
        default: break;
    }

    // Init next screen
    switch (screen)
    {
        case LOGO: InitLogoScreen(); break;
        case HAREMONIC: InitHaremonicScreen(); break;
        case TITLE: InitTitleScreen(); break;
        case OPTIONS: InitOptionsScreen(); break;
        case GAMEPLAY: InitGameplayScreen(); break;
        case ENDING: InitEndingScreen(); break;
        default: break;
    }

    currentScreen = screen;
}

// Request transition to next screen
static void TransitionToScreen(GameScreen screen)
{
    onTransition = true;
    transFadeOut = false;
    transFromScreen = currentScreen;
    transToScreen = screen;
    transAlpha = 0;
}

// Update transition effect (fade-in, fade-out)
static void UpdateTransition(void)
{
    if (!transFadeOut)
    {
        transAlpha += 1;

        if (transAlpha > transLength)
        {
            transAlpha = transLength;

            // Unload current screen
            switch (transFromScreen)
            {
                case LOGO: UnloadLogoScreen(); break;
                case HAREMONIC: UnloadHaremonicScreen(); break;
                case TITLE: UnloadTitleScreen(); break;
                case OPTIONS: UnloadOptionsScreen(); break;
                case GAMEPLAY: UnloadGameplayScreen(); break;
                case ENDING: UnloadEndingScreen(); break;
                default: break;
            }

            // Load next screen
            switch (transToScreen)
            {
                case LOGO: InitLogoScreen(); break;
                case HAREMONIC: InitHaremonicScreen(); break;
                case TITLE: InitTitleScreen(); break;
                case OPTIONS: InitOptionsScreen(); break;
                case GAMEPLAY: InitGameplayScreen(); break;
                case ENDING: InitEndingScreen(); break;
                default: break;
            }

            currentScreen = transToScreen;

            // Activate fade out effect to next loaded screen
            transFadeOut = true;
        }
    }
    else  // Transition fade out logic
    {
        transAlpha -= 1;

        if (transAlpha < 0)
        {
            transAlpha = 0;
            transFadeOut = false;
            onTransition = false;
            transFromScreen = -1;
            transToScreen = UNKNOWN;
        }
    }
}

// Draw transition effect (full-screen rectangle)
static void DrawTransition(void)
{
    for (int x = 0; x < SCREEN_W; ++x)
    {
        for (int y = 0; y < SCREEN_W; ++y)
        {
            int pixel_order = (int[]){2, 0, 1, 3}[2 * (y % 2) + (x % 2)];

            if (transLength * pixel_order < 4 * transAlpha)
                DrawPixel(x, y, SCREEN_COLOR_LIT);
        }
    }
}

// Update and draw game frame
static void UpdateDrawFrame(void)
{
    // Update
    //----------------------------------------------------------------------------------
    if (!triggerAxisDetected)
        hareDetectTriggerAxis();

    UpdateMusicStream(music);       // NOTE: Music keeps playing between screens

    if (!onTransition)
    {
        // Toggle pixel separation
        if (IsKeyPressed(KEY_P) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_1))
            pixelSeparation = !pixelSeparation;
        // Toggle music
        if (IsKeyPressed(KEY_O) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_TRIGGER_1))
        {
            PlaySound(fxCoin);
            isMusicOn = !isMusicOn;
        }

        switch(currentScreen)
        {
            case LOGO:
            {
                UpdateLogoScreen();

                if (FinishLogoScreen()) TransitionToScreen(HAREMONIC);

            } break;
            case HAREMONIC:
            {
                UpdateHaremonicScreen();

                if (FinishHaremonicScreen()) TransitionToScreen(TITLE);

            } break;
            case TITLE:
            {
                UpdateTitleScreen();

                if (FinishTitleScreen()) TransitionToScreen(OPTIONS);

            } break;
            case OPTIONS:
            {
                UpdateOptionsScreen();

                if (FinishOptionsScreen()) TransitionToScreen(GAMEPLAY);

            } break;
            case GAMEPLAY:
            {
                UpdateGameplayScreen();

                if (FinishGameplayScreen())
                {
                    TransitionToScreen(ENDING);

                    // #ifndef PLATFORM_WEB
                    //     return LoadFileData(PROGRESS_SAVE_FILE_ROUTE, bytes_read);
                    // #else
                    //     return loadGameFromIndexedDB(bytes_read);
                    // #endif
                }

            } break;
            case ENDING:
            {
                UpdateEndingScreen();

                if (FinishEndingScreen() == 1)
                {
                    if (lastGameComplete)
                        TransitionToScreen(TITLE);
                    else
                        TransitionToScreen(OPTIONS);
                }

            } break;
            default: break;
        }
    }
    else UpdateTransition();    // Update transition (fade-in, fade-out)
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    BeginTextureMode(nokiaScreen);

        ClearBackground(SCREEN_COLOR_BG);

        switch(currentScreen)
        {
            case LOGO: DrawLogoScreen(); break;
            case HAREMONIC: DrawHaremonicScreen(); break;
            case TITLE: DrawTitleScreen(); break;
            case OPTIONS: DrawOptionsScreen(); break;
            case GAMEPLAY: DrawGameplayScreen(); break;
            case ENDING: DrawEndingScreen(); break;
            default: break;
        }

        // Draw full screen rectangle in front of everything
        if (onTransition) DrawTransition();

    EndTextureMode();

    BeginDrawing();
        Color color_bg = SCREEN_COLOR_BG;
        color_bg.r /= 4;
        color_bg.g /= 4;
        color_bg.b /= 4;

        ClearBackground(color_bg);

        DrawTexturePro(nokiaScreen.texture, (Rectangle){0, 0, SCREEN_W, -SCREEN_H},
                (Rectangle){SCREEN_BORDER, SCREEN_BORDER, SCREEN_SCALE_MULT*SCREEN_W, SCREEN_SCALE_MULT*SCREEN_H},
                (Vector2){0, 0}, 0, WHITE);

        if (pixelSeparation)
        {
            Color line_color = ColorAlpha(SCREEN_COLOR_BG, 0.2);

            for (int y = 0; y <= SCREEN_H; ++y)
                DrawLine(SCREEN_BORDER, SCREEN_BORDER + y*SCREEN_SCALE_MULT,
                        SCREEN_BORDER + SCREEN_W*SCREEN_SCALE_MULT, SCREEN_BORDER + y*SCREEN_SCALE_MULT,
                        line_color);

            for (int x = 0; x <= SCREEN_W; ++x)
                DrawLine(SCREEN_BORDER + x*SCREEN_SCALE_MULT, SCREEN_BORDER,
                        SCREEN_BORDER + x*SCREEN_SCALE_MULT, SCREEN_BORDER + SCREEN_H*SCREEN_SCALE_MULT,
                        line_color);
        }
    EndDrawing();
    //----------------------------------------------------------------------------------
}

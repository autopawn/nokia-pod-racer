#include "raylib.h"
#include "screens.h"

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;

//----------------------------------------------------------------------------------
// Haremonic Screen Functions Definition
//----------------------------------------------------------------------------------

// Haremonic Screen Initialization logic
void InitHaremonicScreen(void)
{
    finishScreen = 0;
    framesCounter = 0;
}

// Haremonic Screen Update logic
void UpdateHaremonicScreen(void)
{
    framesCounter++;

    if (framesCounter > 60)
        finishScreen = true;


}

// Haremonic Screen Draw logic
void DrawHaremonicScreen(void)
{

}

// Haremonic Screen Unload logic
void UnloadHaremonicScreen(void)
{
}

// Haremonic Screen should finish?
int FinishHaremonicScreen(void)
{
    return finishScreen;
}

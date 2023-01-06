// screw you code formatter, this one needs to stay on top
#include <stdlib.h>
// now we need to have libgte.h
#include <libgte.h>
// then, these can work; the psyq library is badly written and won't include what it uses
#include <libetc.h>
#include <libgpu.h>
#include <stdio.h>
#include <types.h>

#include "dcMath.h"
#include "dcCamera.h"
#include "dcRender.h"
#include "dcLevel.h"
#include "character.h"
#include "dcMemory.h"
#include "dcMisc.h"
#include "dcSprite.h"
#include "dcCollision.h"
#include "dcRender.h"
#include "../third_party/modplayer/modplayer.h"
#include "meshes/crash.h"
#include "meshes/Box001.h"

#define CUBESIZE 196 


SDC_Render Render;
SDC_Camera Camera;
SDC_Level MainLevel;

// Display and draw environments, double buffered

extern unsigned long _binary_textures_colorpallete_tim_start[];
extern unsigned long _binary_textures_smile_tim_start[];

void InitGame()
{
    dcMemory_Init();
    PadInit(0);
    InitGeom();   

    int  width = 640;
    int  height = 240;

     CVECTOR bgColor = {60, 120, 120};
    dcRender_Init(&Render, width, height, bgColor, 4096 * 4, 8192 * 10, RENDER_MODE_PAL);
    dcCamera_SetScreenResolution(&Camera, width, height);

}

void InitLevel()
{

    //Camera Init
    long CameraDistance = 200;
    dcCamera_SetCameraPosition(&Camera, 0, 0, CameraDistance);
    VECTOR LookAt = {0,0, 0};
    dcCamera_LookAt(&Camera, &LookAt);

    CVECTOR  AmbientColor = {50, 50, 50, 0};
    dcLevel_InitLight(&MainLevel, &AmbientColor);
    MainLevel.NumObjects = 0;
    //Ambient Light
    dcRender_SetAmbientColor(&Render, &MainLevel.AmbientColor);
    SVECTOR LightDirection = {-ONE, -ONE, 0};
    SVECTOR LightColor = {ONE, ONE, ONE};
    dcLevel_SetLight(&MainLevel, 0, &LightDirection, &LightColor);

    //SVECTOR LightDirection2 = {ONE, 0, ONE};
    //SVECTOR LightColor2 = {0, ONE, ONE};
    //dcLevel_SetLight(&MainLevel, 1, &LightDirection2, &LightColor2);

    //SVECTOR LightDirection3 = {0, ONE, 0};
    //SVECTOR LightColor3 = {ONE, 0, ONE};
    //dcLevel_SetLight(&MainLevel, 2, &LightDirection3, &LightColor3);

    // Set Color matrix
    SetColorMatrix(&MainLevel.ColorMatrix);

    SDC_Mesh3D* TeapotMesh = &crash_Mesh;


    TIM_IMAGE* tim_crash = (TIM_IMAGE*)malloc3(sizeof(TIM_IMAGE));    
    TIM_IMAGE* tim_smile = (TIM_IMAGE*)malloc3(sizeof(TIM_IMAGE));
    SDC_DrawParams DrawParamsCrash = {
        .tim = tim_crash,
        .constantColor = {255, 255, 255},
        .bLighting = 1,
        .bUseConstantColor = 1
    };
    SDC_DrawParams DrawParams = {
        .tim = tim_smile,
        .constantColor = {255, 255, 255},
        .bLighting = 1,
        .bUseConstantColor = 1
    };
    SDC_DrawParams* DrawParamsCrashPtr = (SDC_DrawParams*)malloc3(sizeof(SDC_DrawParams));
    *DrawParamsCrashPtr = DrawParamsCrash;
    dcRender_LoadTexture(tim_crash, _binary_textures_colorpallete_tim_start);

    dcRender_LoadTexture(tim_smile, _binary_textures_smile_tim_start);
    SDC_DrawParams* DrawParamsPtr = (SDC_DrawParams*)malloc3(sizeof(SDC_DrawParams));
    *DrawParamsPtr = DrawParams;

    VECTOR SphereLocation = {0,0, 0, 0};
 VECTOR CharInitialLocation = {0,-100, 0, 0};
    dcLevel_AddObject(&MainLevel, &Box001_Mesh, &SphereLocation, DrawParamsPtr);
    dcLevel_InitCharacter(&MainLevel, TeapotMesh, &CharInitialLocation, DrawParamsCrashPtr);
    
    SDC_DrawParams DrawParams2 = {
        .tim = tim_smile,
        .constantColor = {255, 255, 255},
        .bLighting = 1,
        .bUseConstantColor = 1
    };
    SDC_DrawParams* DrawParamsPtr2 = (SDC_DrawParams*)malloc3(sizeof(SDC_DrawParams));
    *DrawParamsPtr2 = DrawParams2;

    VECTOR SphereLocation2 = {200,0, 0, 0};
    dcLevel_AddObject(&MainLevel, &Box001_Mesh, &SphereLocation2, DrawParamsPtr2);

}
       


void Display()
{
    MATRIX Transform;

    UpdateCharacter(&MainLevel);
    dcCamera_SetCameraPosition(&Camera, MainLevel.Character.Location.vx , MainLevel.Character.Location.vy + 500, MainLevel.Character.Location.vz + 400);
    VECTOR LookAt =  MainLevel.Character.Location;
    dcCamera_LookAt(&Camera, &LookAt);
    
    dcRender_PreDrawMesh(&MainLevel, &Camera, &MainLevel.Character.Location, &MainLevel.Character.Rotation, &Transform);
    dcRender_DrawMesh(&Render, MainLevel.Character.Mesh, &Transform, MainLevel.Character.DrawParams);
//
   for(int i = 0; i < MainLevel.NumObjects; i++)
    {        
        //Logic Update HERE
        //
        //-----------------
        dcRender_PreDrawMesh(&MainLevel, &Camera, &MainLevel.Objects[i].Location, &MainLevel.Objects[i].Rotation, &Transform);

        /*  RotMatrix_gte(&MainLevel.LightAngle, &MainLevel.RotLight);
        RotMatrix_gte(&MainLevel.Objects[i].Rotation, &MainLevel.RotObject);  
        // RotMatrix cube * RotMatrix light
        MulMatrix0(&MainLevel.RotObject, &MainLevel.RotLight, &MainLevel.RotLight);
        // Light Matrix * RotMatrix light 
        MulMatrix0(&MainLevel.LocalLightMatrix, &MainLevel.RotLight, &MainLevel.WorldLightMatrix);
        // Set new light matrix 
        SetLightMatrix(&MainLevel.WorldLightMatrix);

        RotMatrix(&MainLevel.Objects[i].Rotation, &Transform);
        TransMatrix(&Transform, &MainLevel.Objects[i].Location);
        dcCamera_ApplyCameraTransform(&Camera, &Transform, &Transform);
*/
        dcRender_DrawMesh(&Render, MainLevel.Objects[i].Mesh, &Transform, MainLevel.Objects[i].DrawParams);
    }

    dcRender_SwapBuffers(&Render);
}

void Input()
{
}

int main(void) 
{    

    InitGame();   
    InitLevel();

    while (1) 
    {    
        Input();
        Display();
    }

    return 0;
}
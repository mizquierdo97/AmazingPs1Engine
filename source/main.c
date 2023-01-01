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
#include "dcMemory.h"
#include "dcMisc.h"
#include "dcCollision.h"
#include "../third_party/modplayer/modplayer.h"

#define CUBESIZE 196 


 SDC_Render Render;
SDC_Camera Camera;
SDC_Level MainLevel;

// Display and draw environments, double buffered

void InitGame()
{
    dcMemory_Init();
    PadInit(0);
    InitGeom();   

    int  width = 640;
    int  height = 240;

     CVECTOR bgColor = {60, 120, 120};
    dcRender_Init(&Render, width, height, bgColor, 4096, 8192, RENDER_MODE_PAL);
    dcCamera_SetScreenResolution(&Camera, width, height);

}

void InitLevel()
{

    CVECTOR  AmbientColor = {0, 0, 0, 0};
    dcLevel_InitLight(&MainLevel, &AmbientColor);

   /*
    SVECTOR LightDirection = {-ONE, ONE, 0};
    SVECTOR LightColor = {ONE, ONE, ONE};
    dcLevel_SetLight(&MainLevel, 0, &LightDirection, &LightColor);
    //Ambient Light
    dcRender_SetAmbientColor(&Render, &MainLevel.AmbientColor);
    // Set Color matrix
    SetColorMatrix(&MainLevel.ColorMatrix);*/
}

int main(void) 
{
    InitGame();   
    InitLevel();

//To level
    SDC_Mesh3D* sphereMesh = dcMisc_generateSphereMesh(CUBESIZE, 7, 7);

    long CameraDistance = 800;
    dcCamera_SetCameraPosition(&Camera, 0, 0, CameraDistance);
    dcCamera_LookAt(&Camera, &VECTOR_ZERO);

    SVECTOR rotation = {0};
    VECTOR translation = {0, 0, 0, 0};
    MATRIX transform;
    //

    SDC_DrawParams drawParams = {
        .tim = NULL,
        .constantColor = {255, 255, 255},
        .bLighting = 1,
        .bUseConstantColor = 1
    };


    while (1) { 
        //rotation.vy += 16;
        //translation.vx -= 10;
        //translation.vz += 10;

// TODO: Rotation inside the object struct
//Other stuff of the light in LevelStruct?


        // Find and apply light rotation matrix
        // Find rotmat from light angles
        RotMatrix_gte(&MainLevel.LightAngle, &MainLevel.RotLight);
        // Find rotmat from cube angles
        RotMatrix_gte(&rotation, &MainLevel.RotObject);  
        // RotMatrix cube * RotMatrix light
        MulMatrix0(&MainLevel.RotObject, &MainLevel.RotLight, &MainLevel.RotLight);
        // Light Matrix * RotMatrix light 
        MulMatrix0(&MainLevel.LocalLightMatrix, &MainLevel.RotLight, &MainLevel.WorldLightMatrix);
        // Set new light matrix 
        SetLightMatrix(&MainLevel.WorldLightMatrix);

        RotMatrix(&rotation, &transform);
        TransMatrix(&transform, &translation);
        dcCamera_ApplyCameraTransform(&Camera, &transform, &transform);

        FntPrint("GameDev Challenge Sphere Demo\n");

        SVECTOR rayDir = { Camera.viewMatrix.m[2][0], Camera.viewMatrix.m[2][1], Camera.viewMatrix.m[2][2] };
        VectorNormalSS(&rayDir, &rayDir);


        dcRender_DrawMesh(&Render, sphereMesh, &transform, &drawParams);
       

        dcRender_SwapBuffers(&Render);

     
    }

    return 0;
}
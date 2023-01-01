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
#include "dcSprite.h"
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
    dcRender_Init(&Render, width, height, bgColor, 4096, 8192 * 8, RENDER_MODE_PAL);
    dcCamera_SetScreenResolution(&Camera, width, height);

}

void InitLevel()
{

    CVECTOR  AmbientColor = {0, 0, 0, 0};
    dcLevel_InitLight(&MainLevel, &AmbientColor);
    MainLevel.NumObjects = 0;
    //Ambient Light
    dcRender_SetAmbientColor(&Render, &MainLevel.AmbientColor);
    SVECTOR LightDirection = {-ONE, ONE, 0};
    SVECTOR LightColor = {ONE, ONE, 0};
    dcLevel_SetLight(&MainLevel, 0, &LightDirection, &LightColor);

        SVECTOR LightDirection2 = {ONE, 0, 0};
    SVECTOR LightColor2 = {0, ONE, 0};
    dcLevel_SetLight(&MainLevel, 1, &LightDirection2, &LightColor2);

            SVECTOR LightDirection3 = {0, -ONE, 0};
    SVECTOR LightColor3 = {ONE, 0, 0};
    dcLevel_SetLight(&MainLevel, 2, &LightDirection3, &LightColor3);
    // Set Color matrix
    SetColorMatrix(&MainLevel.ColorMatrix);

    SDC_Mesh3D* SphereMesh = dcMisc_generateSphereMesh(CUBESIZE, 3, 3);

int num = 8;
for(int i = -num/2 ; i < num/2; i++)
{
    for(int n = -num/2; n < num/2; n++)
    {
    VECTOR SphereLocation = {n * 500,0,  i*500, 0};
    dcLevel_AddObject(&MainLevel,SphereMesh, SphereLocation);
    }

}
  
}

int main(void) 
{
    InitGame();   
    InitLevel();

//To level

    long CameraDistance = 800;
    dcCamera_SetCameraPosition(&Camera, 0, 800, CameraDistance);
    dcCamera_LookAt(&Camera, &VECTOR_ZERO);

    MATRIX transform;
    //

    SDC_DrawParams drawParams = {
        .tim = NULL,
        .constantColor = {255, 255, 255},
        .bLighting = 1,
        .bUseConstantColor = 1
    };


    while (1) { 
       
for(int i = 0; i < MainLevel.NumObjects; i++)
{    
        RotMatrix_gte(&MainLevel.LightAngle, &MainLevel.RotLight);
      MainLevel.Objects[i].Rotation.vy += 4;
        RotMatrix_gte(&MainLevel.Objects[i].Rotation, &MainLevel.RotObject);  
        // RotMatrix cube * RotMatrix light
        MulMatrix0(&MainLevel.RotObject, &MainLevel.RotLight, &MainLevel.RotLight);
        // Light Matrix * RotMatrix light 
        MulMatrix0(&MainLevel.LocalLightMatrix, &MainLevel.RotLight, &MainLevel.WorldLightMatrix);
        // Set new light matrix 
        SetLightMatrix(&MainLevel.WorldLightMatrix);

        RotMatrix(&MainLevel.Objects[i].Rotation, &transform);
        TransMatrix(&transform, & MainLevel.Objects[i].Location);
        dcCamera_ApplyCameraTransform(&Camera, &transform, &transform);

        SVECTOR rayDir = { Camera.viewMatrix.m[2][0], Camera.viewMatrix.m[2][1], Camera.viewMatrix.m[2][2] };
        VectorNormalSS(&rayDir, &rayDir);
        dcRender_DrawMesh(&Render, MainLevel.Objects[i].Mesh, &transform, &drawParams);     
    }

        dcRender_SwapBuffers(&Render);
    }

    return 0;
}
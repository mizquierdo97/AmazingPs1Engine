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
#include "dcRender.h"
#include "../third_party/modplayer/modplayer.h"
#include "meshes/crash.h"

#define CUBESIZE 196 


SDC_Render Render;
SDC_Camera Camera;
SDC_Level MainLevel;

// Display and draw environments, double buffered

extern unsigned long _binary_textures_colorpallete_tim_start[];

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
    long CameraDistance = 400;
    dcCamera_SetCameraPosition(&Camera, 0, 500, CameraDistance);
    VECTOR LookAt = {0,-0, 0};
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

    //SDC_Mesh3D* SphereMesh = dcMisc_generateSphereMesh(CUBESIZE, 3, 3);   
    SDC_Mesh3D* TeapotMesh = &crash_Mesh;


    TIM_IMAGE* tim_smile = (TIM_IMAGE*)malloc3(sizeof(TIM_IMAGE));
    SDC_DrawParams DrawParams = {
        .tim = tim_smile,
        .constantColor = {255, 255, 255},
        .bLighting = 1,
        .bUseConstantColor = 1
    };
    SDC_DrawParams* DrawParamsPtr = (SDC_DrawParams*)malloc3(sizeof(SDC_DrawParams));
    *DrawParamsPtr = DrawParams;
    dcRender_LoadTexture(tim_smile, _binary_textures_colorpallete_tim_start);    

    Render.DrawParameters = DrawParams;

    VECTOR SphereLocation = {0,0, 0, 0};
    dcLevel_AddObject(&MainLevel, TeapotMesh, &SphereLocation, DrawParamsPtr);
}
       


void Display()
{
    MATRIX Transform;
   for(int i = 0; i < MainLevel.NumObjects; i++)
    {        
        //Logic Update HERE
        //
        //-----------------

        RotMatrix_gte(&MainLevel.LightAngle, &MainLevel.RotLight);
        RotMatrix_gte(&MainLevel.Objects[i].Rotation, &MainLevel.RotObject);  
        // RotMatrix cube * RotMatrix light
        MulMatrix0(&MainLevel.RotObject, &MainLevel.RotLight, &MainLevel.RotLight);
        // Light Matrix * RotMatrix light 
        MulMatrix0(&MainLevel.LocalLightMatrix, &MainLevel.RotLight, &MainLevel.WorldLightMatrix);
        // Set new light matrix 
        SetLightMatrix(&MainLevel.WorldLightMatrix);

        RotMatrix(&MainLevel.Objects[i].Rotation, &Transform);
        TransMatrix(&Transform, & MainLevel.Objects[i].Location);
        dcCamera_ApplyCameraTransform(&Camera, &Transform, &Transform);

        dcRender_DrawMesh(&Render, MainLevel.Objects[i].Mesh, &Transform, MainLevel.Objects[i].DrawParams);     
    }

    dcRender_SwapBuffers(&Render);
}

void Input()
{
       for(int i = 0; i < MainLevel.NumObjects; i++)
    {        
    u_long PadState = PadRead(0);

    SVECTOR IncrementalPosition = {0,0,0};
    if( _PAD(0,PADLup ) & PadState )
    {
        IncrementalPosition.vz -=10;
    }
    if( _PAD(0,PADLdown ) & PadState )
    {
        IncrementalPosition.vz +=10;
    }
    if( _PAD(0,PADLright ) & PadState )
    {
        IncrementalPosition.vx +=10;
    }
    if( _PAD(0,PADLleft ) & PadState )
    {
        IncrementalPosition.vx -=10;
    }
    if( _PAD(0,PADRup ) & PadState )
    {
        printf("Triangle\n");
    }
    if( _PAD(0,PADRdown ) & PadState )
    {
        printf("Cross\n");
    }
    if( _PAD(0,PADRright ) & PadState )
    {
        printf("Circle\n");
    }
    if( _PAD(0,PADRleft ) & PadState )
    {
        printf("Square\n");
    }
    if(IncrementalPosition.vx != 0 || IncrementalPosition.vz != 0)
    {
    SVECTOR WorldForward = {ONE,0,0};
    SVECTOR Dir;    
    VectorNormalSS(&IncrementalPosition, &Dir);
    SVECTOR Diff = {Dir.vx -  MainLevel.Objects[i].Direction.vx,Dir.vy -  MainLevel.Objects[i].Direction.vy,Dir.vz -  MainLevel.Objects[i].Direction.vz};
    int LerpSpeed = 8;
    SVECTOR DIffDivided = {Diff.vx / LerpSpeed, Diff.vy / LerpSpeed, Diff.vz / LerpSpeed};
    SVECTOR Summ = {MainLevel.Objects[i].Direction.vx + DIffDivided.vx,MainLevel.Objects[i].Direction.vy + DIffDivided.vy,MainLevel.Objects[i].Direction.vz + DIffDivided.vz};
    MainLevel.Objects[i].Direction =  Summ;

    long atan2 = ratan2(MainLevel.Objects[i].Direction.vx, MainLevel.Objects[i].Direction.vz);
    int dot = dcMath_DotProduct(&WorldForward, &MainLevel.Objects[i].Direction);

    MainLevel.Objects[i].Rotation.vy = atan2;
    MainLevel.Objects[i].Location.vx += IncrementalPosition.vx;    
    MainLevel.Objects[i].Location.vz += IncrementalPosition.vz;

    dcCamera_SetCameraPosition(&Camera, MainLevel.Objects[i].Location.vx , MainLevel.Objects[i].Location.vy + 500, MainLevel.Objects[i].Location.vz + 400);
    VECTOR LookAt =  MainLevel.Objects[i].Location;
    dcCamera_LookAt(&Camera, &LookAt);

    }
    }
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
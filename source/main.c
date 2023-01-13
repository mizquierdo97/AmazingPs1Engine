// screw you code formatter, this one needs to stay on top
#include <stdlib.h>
// now we need to have libgte.h
#include <libgte.h>
// then, these can work; the psyq library is badly written and won't include what it uses
#include <libetc.h>
#include <libgpu.h>
#include <stdio.h>
#include <types.h>
#include <stdbool.h>

#include "dcMath.h"
#include "dcCamera.h"
#include "dcRender.h"
#include "dcLevel.h"
#include "character.h"
#include "dcMemory.h"
#include "dcMisc.h"
#include "dcSprite.h"
#include "dcCollision.h"
#include "dcAudio.h"
#include "dcRender.h"
#include "../third_party/modplayer/modplayer.h"
#include "meshes/Box001.h"

#define CUBESIZE 196 


SDC_Render Render;
SDC_Render FirstPlayerRender;
SDC_Render SecondPlayerRender;
SDC_Camera Camera;
SDC_Camera FirstPlayerCamera;
SDC_Level MainLevel;

// Display and draw environments, double buffered

extern unsigned long _binary_textures_colorpallete_tim_start[];
extern unsigned long _binary_textures_smile_tim_start[];

extern unsigned long _binary_data_accept_vag_start[];
extern unsigned long _binary_data_beep_vag_start[];
extern unsigned long _binary_data_bell_vag_start[];

void InitGame()
{
    dcMemory_Init();
    PadInit(0);
    InitGeom();   

    int  width = 640;
    int  height = 240;

    CVECTOR bgColor = {60, 120, 120};        
    dcRender_Init(&Render, width, height, bgColor, 4096 * 4, 8192 * 10, RENDER_MODE_PAL, 1);
    dcRender_Init(&FirstPlayerRender, width, height, bgColor, 4096 * 4, 8192 * 10, RENDER_MODE_PAL, 0);
    dcCamera_SetScreenResolution(&Camera, width, height);
    Camera.PlayerCameraIndex = 0;
    dcCamera_SetScreenResolution(&FirstPlayerCamera, width, height);
    FirstPlayerCamera.PlayerCameraIndex = 1;
}

//This should go to a separate .h so we can modify the level without having conflicts
void InitLevel()
{    


    //dcAudio_MusicPlay(&audio, 0);

    //Camera Init
    long CameraDistance = 500;
    dcCamera_SetCameraPosition(&Camera, 0, 0, CameraDistance);
    VECTOR LookAt = {0,0, 0};
    dcCamera_LookAt(&Camera, &LookAt);

    dcCamera_SetCameraPosition(&FirstPlayerCamera, 0, 200, CameraDistance);

    dcCamera_LookAt(&FirstPlayerCamera, &LookAt);

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

    SetColorMatrix(&MainLevel.ColorMatrix);

    TIM_IMAGE* tim_crash = (TIM_IMAGE*)malloc3(sizeof(TIM_IMAGE));    
    TIM_IMAGE* tim_smile = (TIM_IMAGE*)malloc3(sizeof(TIM_IMAGE));

    //We can move this structure initialization to a function
    SDC_DrawParams DrawParamsCrash = {
        .tim = tim_smile,
        .constantColor = {255, 0, 255},
        .bLighting = 1,
        .bUseConstantColor = 1
    };    
    SDC_DrawParams DrawParamsCrash2 = {
        .tim = tim_smile,
        .constantColor = {0, 255, 255},
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
        SDC_DrawParams* DrawParamsCrashPtr2 = (SDC_DrawParams*)malloc3(sizeof(SDC_DrawParams));
    *DrawParamsCrashPtr2 = DrawParamsCrash2;
    SDC_DrawParams* DrawParamsPtr = (SDC_DrawParams*)malloc3(sizeof(SDC_DrawParams));
    *DrawParamsPtr = DrawParams;

    dcRender_LoadTexture(tim_crash, _binary_textures_colorpallete_tim_start);
    dcRender_LoadTexture(tim_smile, _binary_textures_smile_tim_start);

    VECTOR BoxLocation = {000,0, 0, 0};
    VECTOR BoxLocation2 = {200,0, 0, 0};
    VECTOR CharacterInitialLocation = {100,0, 0, 0};
    VECTOR Character2InitialLocation = {-100, 0, 0, 0};
    
    SDC_Object* Parent = dcLevel_AddObject(&MainLevel, &Box001_Mesh, &BoxLocation, DrawParamsPtr, NULL);
    dcLevel_AddObject(&MainLevel, &Box001_Mesh, &BoxLocation2, DrawParamsPtr, Parent);    
    dcLevel_InitCharacter(&MainLevel, &Box001_Mesh, &CharacterInitialLocation, DrawParamsCrashPtr);
    dcLevel_InitCharacter(&MainLevel, &Box001_Mesh, &Character2InitialLocation, DrawParamsCrashPtr2);
}       

void Display(SDC_Render* InRender, SDC_Camera* InCamera)
{

    MATRIX CharacterTransform;
    for(int i = 0; i < MainLevel.NumCharacters; i++)
    {
        bool bIsLocalPlayer = InCamera->PlayerCameraIndex == i;
        if(bIsLocalPlayer)
        {
            UpdateCharacter(MainLevel.Characters[i]);
            dcCamera_SetCameraPosition(InCamera, MainLevel.Characters[i]->Location.vx , MainLevel.Characters[i]->Location.vy + 100, MainLevel.Characters[i]->Location.vz + 200);
            VECTOR LookAt =  MainLevel.Characters[i]->Location;
            dcCamera_LookAt(InCamera, &LookAt);
        }
        RotMatrix(&MainLevel.Characters[i]->Rotation, &CharacterTransform);
        TransMatrix(&CharacterTransform,  &MainLevel.Characters[i]->Location);
        dcRender_PreDrawMesh(&MainLevel, InCamera, &MainLevel.Characters[i]->Location, &MainLevel.Characters[i]->Rotation, &CharacterTransform);
        dcRender_DrawMesh(InRender, MainLevel.Characters[i]->Mesh, &CharacterTransform, MainLevel.Characters[i]->DrawParams);
    }

    //Draw Objects
    MATRIX Transform;
   for(int i = 0; i < MainLevel.NumObjects; i++)
    {
        /*
        UPDATE OBJECTS?¿?¿
        */
        MATRIX WorldTransform;
        //MainLevel.Objects[i]->Rotation.vy += 10;
        RotMatrix(&MainLevel.Objects[i]->Rotation, &Transform);
        TransMatrix(&Transform,  &MainLevel.Objects[i]->Location);
        GetParentTransform(MainLevel.Objects[i], &Transform, &WorldTransform);

        dcRender_PreDrawMesh(&MainLevel, InCamera, &MainLevel.Objects[i]->Location, &MainLevel.Objects[i]->Rotation, &WorldTransform);
        dcRender_DrawMesh(InRender, MainLevel.Objects[i]->Mesh, &WorldTransform, MainLevel.Objects[i]->DrawParams);
    }
    dcRender_SwapBuffers(InRender);
}

void Input()
{
    
    u_long PadState = PadRead(0);
    if( _PAD(0,PADLup ) & PadState )
    {
    }
    if( _PAD(0,PADLdown ) & PadState )
    {
    }
    if( _PAD(0,PADLright ) & PadState )
    {
    }
    if( _PAD(0,PADLleft ) & PadState )
    {
    }
    if( _PAD(0,PADRup ) & PadState )
    {
    }
    if( _PAD(0,PADRdown ) & PadState )
    {
    }
    if( _PAD(0,PADRright ) & PadState )
    {
    }
    if( _PAD(0,PADRleft ) & PadState )
    {
    }
}

int main(void) 
{    

    SDC_Audio audio;
    dcAudio_Init(&audio, 16);
    
    InitGame();   
    InitLevel();


    //dcAudio_SfxLoad(&audio, &bellSfx, (u_char *)_binary_data_bell_vag_start);
    //dcAudio_SfxLoad(&audio, &acceptSfx, (u_char *)_binary_data_accept_vag_start);
    //dcAudio_SfxLoad(&audio, &beepSfx, (u_char *)_binary_data_beep_vag_start); 

    while (1) 
    {            
    // Init Pad
   
    // Store input values    

        VSync( 0 );
        DrawSync( 0 );
        SetDispMask( 1 );

        Input(); 
        Display(&Render, &Camera);       
        Display(&FirstPlayerRender, &FirstPlayerCamera);
    }

    return 0;
}
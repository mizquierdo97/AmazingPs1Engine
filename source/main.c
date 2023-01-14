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
#include "projectile.h"
#include "../third_party/modplayer/modplayer.h"
//#include "meshes/Box001.h"

// cubo letras👍
#include "meshes/Box003.h"

//tank pieces
#include "meshes/body.h"
//#include "meshes/cannon.h"
//#include "meshes/head.h"
//#include "meshes/left_eye.h"
//#include "meshes/right_eye.h"
#include "meshes/Bonifacio.h"
#include "meshes/floor_mesh.h"




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
extern unsigned long _binary_textures_Letra_A_tim_start[];
extern unsigned long _binary_textures_Pala_tim_start[];
extern unsigned long _binary_textures_floor_texture_tim_start[];


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
    TIM_IMAGE* tim_Pala = (TIM_IMAGE*)malloc3(sizeof(TIM_IMAGE));    
    TIM_IMAGE* tim_floor = (TIM_IMAGE*)malloc3(sizeof(TIM_IMAGE));


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
       
            SDC_DrawParams DrawParamsFloor = {
        .tim = tim_floor,
        .constantColor = {255, 255, 255},
        .bLighting = 1,
        .bUseConstantColor = 1
    };
 SDC_DrawParams DrawPalaParams = {
        .tim = tim_Pala,
        .constantColor = {255, 255, 255},
        .bLighting = 1,
        .bUseConstantColor = 1
    };
//printf("\n", DrawParamsFloor.bLighting);
    SDC_DrawParams* DrawParamsFloorPtr = (SDC_DrawParams*)malloc3(sizeof(SDC_DrawParams));
    *DrawParamsFloorPtr = DrawParamsFloor;
    SDC_DrawParams* DrawParamsCrashPtr = (SDC_DrawParams*)malloc3(sizeof(SDC_DrawParams));
    *DrawParamsCrashPtr = DrawParamsCrash;

    SDC_DrawParams* DrawParamsPalaPtr = (SDC_DrawParams*)malloc3(sizeof(SDC_DrawParams));
    *DrawParamsPalaPtr = DrawPalaParams;


        SDC_DrawParams* DrawParamsCrashPtr2 = (SDC_DrawParams*)malloc3(sizeof(SDC_DrawParams));
    *DrawParamsCrashPtr2 = DrawParamsCrash2;
    SDC_DrawParams* DrawParamsPtr = (SDC_DrawParams*)malloc3(sizeof(SDC_DrawParams));
    *DrawParamsPtr = DrawParams;

    dcRender_LoadTexture(tim_crash, _binary_textures_colorpallete_tim_start);
    dcRender_LoadTexture(tim_smile, _binary_textures_Letra_A_tim_start);
    dcRender_LoadTexture(tim_Pala, _binary_textures_Pala_tim_start);
    dcRender_LoadTexture(tim_floor, _binary_textures_floor_texture_tim_start);



    //localizaciones cajas para luego crear los actores
    VECTOR BoxLocarion = {000,0, 0, 0};
    VECTOR BoxLocation4 = {375,125, 0, 0};
    VECTOR BoxLocation5 = {-125,0, 125, 0};
    VECTOR BoxLocation6 = {-125,0, 250, 0};
    VECTOR BoxLocation7 = {-125,0, 375, 0};
    VECTOR BoxLocation8 = {0,125, 0, 0};
    VECTOR BoxLocation9 = {375,0, 0, 0};
    VECTOR BoxLocation10 = {500,0, 125, 0};
    VECTOR BoxLocation11 = {500,0, 250, 0};
    VECTOR BoxLocation12 = {500,0, 375, 0};

    VECTOR BoxLocation13 = {0,0, -500, 0};
    VECTOR BoxLocation16 = {375,0, -500, 0};
    VECTOR BoxLocation17 = {0,125, -500, 0};
    VECTOR BoxLocation18 = {375,125, -500, 0};
    VECTOR BoxLocation19 = {-125,0, -625, 0};
    VECTOR BoxLocation20 = {-125,0, -750, 0};
    VECTOR BoxLocation21 = {-125,0, -875, 0};

    VECTOR BoxLocation22 = {500,0, -625, 0};
    VECTOR BoxLocation23 = {500,0, -750, 0};
    VECTOR BoxLocation24 = {500,0, -875, 0};


    




    VECTOR palaLocation = {10,50, 0, 0};   


    VECTOR CharacterInitialLocation = {150,0, 0, 0};
    VECTOR Character2InitialLocation = {-100, 0, 0, 0};
    
    VECTOR BoxHalfSize = {20,20,20};
    SVECTOR BoxRotation = {0,0,0};
    SDC_Object* Parent = dcLevel_AddObject(&MainLevel, &Box003_Mesh, &BoxLocarion, &BoxRotation, DrawParamsPtr, NULL, 1 ,&BoxHalfSize);
    dcLevel_AddObject(&MainLevel, &Box003_Mesh, &BoxLocation4,&BoxRotation, DrawParamsPtr, Parent, 1 ,&BoxHalfSize);
    dcLevel_AddObject(&MainLevel, &Box003_Mesh, &BoxLocation5, &BoxRotation,DrawParamsPtr, Parent, 1 ,&BoxHalfSize);
    dcLevel_AddObject(&MainLevel, &Box003_Mesh, &BoxLocation6,&BoxRotation, DrawParamsPtr, Parent, 1 ,&BoxHalfSize);
    dcLevel_AddObject(&MainLevel, &Box003_Mesh, &BoxLocation7,&BoxRotation, DrawParamsPtr, Parent, 1 ,&BoxHalfSize);
    dcLevel_AddObject(&MainLevel, &Box003_Mesh, &BoxLocation8, &BoxRotation,DrawParamsPtr, Parent, 1 ,&BoxHalfSize);
    dcLevel_AddObject(&MainLevel, &Box003_Mesh, &BoxLocation9,&BoxRotation, DrawParamsPtr, Parent, 1 ,&BoxHalfSize);
    dcLevel_AddObject(&MainLevel, &Box003_Mesh, &BoxLocation10,&BoxRotation, DrawParamsPtr, Parent, 1 ,&BoxHalfSize);
    dcLevel_AddObject(&MainLevel, &Box003_Mesh, &BoxLocation11,&BoxRotation, DrawParamsPtr, Parent, 1 ,&BoxHalfSize);
    dcLevel_AddObject(&MainLevel, &Box003_Mesh, &BoxLocation12,&BoxRotation, DrawParamsPtr, Parent, 1 ,&BoxHalfSize);
    dcLevel_AddObject(&MainLevel, &Box003_Mesh, &BoxLocation13,&BoxRotation, DrawParamsPtr, Parent, 1 ,&BoxHalfSize);
    dcLevel_AddObject(&MainLevel, &Box003_Mesh, &BoxLocation16,&BoxRotation, DrawParamsPtr, Parent, 1 ,&BoxHalfSize);
    dcLevel_AddObject(&MainLevel, &Box003_Mesh, &BoxLocation17, &BoxRotation,DrawParamsPtr, Parent, 1 ,&BoxHalfSize);
    dcLevel_AddObject(&MainLevel, &Box003_Mesh, &BoxLocation18,&BoxRotation, DrawParamsPtr, Parent, 1 ,&BoxHalfSize);
    dcLevel_AddObject(&MainLevel, &Box003_Mesh, &BoxLocation19,&BoxRotation, DrawParamsPtr, Parent, 1 ,&BoxHalfSize);
    dcLevel_AddObject(&MainLevel, &Box003_Mesh, &BoxLocation20,&BoxRotation, DrawParamsPtr, Parent, 1 ,&BoxHalfSize);
    dcLevel_AddObject(&MainLevel, &Box003_Mesh, &BoxLocation21,&BoxRotation, DrawParamsPtr, Parent, 1 ,&BoxHalfSize);
    dcLevel_AddObject(&MainLevel, &Box003_Mesh, &BoxLocation22,&BoxRotation, DrawParamsPtr, Parent, 1 ,&BoxHalfSize);
    dcLevel_AddObject(&MainLevel, &Box003_Mesh, &BoxLocation23,&BoxRotation, DrawParamsPtr, Parent, 1 ,&BoxHalfSize);
    dcLevel_AddObject(&MainLevel, &Box003_Mesh, &BoxLocation24,&BoxRotation, DrawParamsPtr, Parent, 1 ,&BoxHalfSize);


    
    


VECTOR nullbox = {0,0,0};
    dcLevel_AddObject(&MainLevel, &floor_mesh_Mesh, &BoxLocarion,&BoxRotation, DrawParamsFloorPtr, NULL, 0 ,&nullbox);
    
        
    SDC_Character* FirstCharacter = dcLevel_InitCharacter(&MainLevel, &body_Mesh, &CharacterInitialLocation, DrawParamsCrashPtr);
    dcLevel_InitCharacter(&MainLevel, &body_Mesh, &Character2InitialLocation, DrawParamsCrashPtr2);    
    //el bonifacio

    SVECTOR PalaRotation = {2048, 0 ,1024};
    SDC_Object* Pala = dcLevel_AddObjectOnCharacter(&MainLevel, &Bonifacio_Mesh, &palaLocation, &PalaRotation, DrawParamsPalaPtr, FirstCharacter, 1 ,&BoxHalfSize);
    FirstCharacter->Pala = Pala;
}       

void Display(SDC_Render* InRender, SDC_Camera* InCamera)
{

    MATRIX CharacterTransform;
    for(int i = 0; i < MainLevel.NumCharacters; i++)
    {
        bool bIsLocalPlayer = InCamera->PlayerCameraIndex == i;
        if(bIsLocalPlayer)
        {
            UpdateCharacter(MainLevel.Characters[i], &MainLevel);
            SVECTOR CameraLocation = MainLevel.Characters[i]->FrontVector;
            dcCamera_SetCameraPosition(InCamera, MainLevel.Characters[i]->Location.vx + CameraLocation.vx / 20, MainLevel.Characters[i]->Location.vy + 200, MainLevel.Characters[i]->Location.vz + CameraLocation.vz / 20);
            VECTOR LookAt =  MainLevel.Characters[i]->Location;
            LookAt.vy += 150;
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
    {         /*
        UPDATE OBJECTS?¿?¿
        */
        MATRIX WorldTransform;
        //MainLevel.Objects[i]->Rotation.vy += 10;
        RotMatrix(&MainLevel.Objects[i]->Rotation, &Transform);
        TransMatrix(&Transform,  &MainLevel.Objects[i]->Location);
        ScaleMatrixL(&Transform, &MainLevel.Objects[i]->Scale);
        if(MainLevel.Objects[i]->CharacterParent != NULL)
        {
            
            RotMatrix(&MainLevel.Objects[i]->CharacterParent->Rotation, &WorldTransform);
            TransMatrix(&WorldTransform, &MainLevel.Objects[i]->CharacterParent->Location);            
            
            CompMatrix(&WorldTransform, &Transform, &WorldTransform);
        }
        else
        {
        GetParentTransform(MainLevel.Objects[i], &Transform, &WorldTransform);
        }

        dcRender_PreDrawMesh(&MainLevel, InCamera, &MainLevel.Objects[i]->Location, &MainLevel.Objects[i]->Rotation, &WorldTransform);
        dcRender_DrawMesh(InRender, MainLevel.Objects[i]->Mesh, &WorldTransform, MainLevel.Objects[i]->DrawParams);
    }

    for(int i = 0; i < MainLevel.NumProjectiles; i++)
    {
        MATRIX WorldTransform;
        //MainLevel.Objects[i]->Rotation.vy += 10;
        RotMatrix(&MainLevel.Projectiles[i]->Rotation, &WorldTransform);
        TransMatrix(&WorldTransform,  &MainLevel.Projectiles[i]->Location);
        //GetParentTransform(MainLevel.Projectiles[i], &Transform, &WorldTransform);

        dcRender_PreDrawMesh(&MainLevel, InCamera, &MainLevel.Projectiles[i]->Location, &MainLevel.Projectiles[i]->Rotation, &WorldTransform);
        dcRender_DrawMesh(InRender, MainLevel.Projectiles[i]->Mesh, &WorldTransform, MainLevel.Projectiles[i]->DrawParams);
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


printf("HERE2!!!\n");
    for(int i = 0; i < MainLevel.NumProjectiles; i++)
    {
        UpdateProjectile(&MainLevel, MainLevel.Projectiles[i], i);
    }
        Input(); 
        Display(&Render, &Camera);       
        Display(&FirstPlayerRender, &FirstPlayerCamera);
        
printf("HERE!!!\n");
    }

    return 0;
}
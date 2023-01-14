#include "character.h"
#include "dcCollision.h"
#include <stdio.h>
#include "dcRender.h"
#include "meshes/Box001.h"

void UpdateCharacter(SDC_Character* Character, SDC_Level* Level)
{
    int PlayerIndex = Character->PlayerIndex;
    u_long PadState = PadRead(0);

    SVECTOR IncrementalPosition = {0,0,0};
    if( _PAD(PlayerIndex,PADLup ) & PadState )
    {
        IncrementalPosition.vz += (-Character->FrontVector.vz/ 400);        
        IncrementalPosition.vx += (-Character->FrontVector.vx / 400);
    }
    if( _PAD(PlayerIndex,PADLdown ) & PadState )
    {
        IncrementalPosition.vz += (Character->FrontVector.vz / 400);        
        IncrementalPosition.vx += (Character->FrontVector.vx / 400);
    }
    if( _PAD(PlayerIndex,PADLright ) & PadState )
    {        
        IncrementalPosition.vx += (Character->FrontVector.vz/ 400);        
        IncrementalPosition.vz += (-Character->FrontVector.vx / 400);
    }
    if( _PAD(PlayerIndex,PADLleft ) & PadState )
    {
        IncrementalPosition.vx += (-Character->FrontVector.vz/ 400);        
        IncrementalPosition.vz += (Character->FrontVector.vx / 400);
    }
    if( _PAD(0,PADR1 ) & PadState )
    {
        Character->CameraAngle += 20;
    }
    if( _PAD(0,PADL1 ) & PadState )
    {
        Character->CameraAngle -= 20;
    }
    if( _PAD(PlayerIndex,PADRleft ) & PadState )
    {
        //Dash
        if(!Character->bDoingDash)
        {
        Character->bDoingDash = 1;
        Character->DashRemaining = 600;//Character->DashDistance;
        
        }

    }
    if( _PAD(PlayerIndex,PADRright ) & PadState )
    {
        Character->bHoldingFire = 1;
        Character->ProjectileStrength += 1;

    }
    else
    {
        if(Character->bHoldingFire)
        {
            printf("%i \n", &Box001_Mesh.numIndices);
            //FIRE



            //We can move this structure initialization to a function
            SDC_DrawParams DrawParams = {
                .tim = NULL,
                .constantColor = {255, 255, 255},
                .bLighting = 1,
                .bUseConstantColor = 1
            };

            SDC_DrawParams* DrawParamsPtr = (SDC_DrawParams*)malloc3(sizeof(SDC_DrawParams));
            *DrawParamsPtr = DrawParams;



            //dcLevel_AddObject(&MainLevel, &Box001_Mesh, &MainLevel.Characters[i]->Location, DrawParamsPtr, NULL); 
            dcLevel_AddProjectile(Level, &Box001_Mesh, &Character->Location, &Character->FrontVector, Character->ProjectileStrength, DrawParamsPtr); 
            Character->bHoldingFire = 0;
            Character->ProjectileStrength = 0;
        }

    }
     VECTOR FutureLocation = {Character->Location.vx, Character->Location.vy, Character->Location.vz};
    if(Character->bDoingDash)
    {
        FutureLocation.vx = Character->Location.vx + ((Character->Direction.vx * 50) >> 12);    
        FutureLocation.vz = Character->Location.vz + ((Character->Direction.vz * 50) >> 12);
        Character->DashRemaining -= 50;//Character->DashVelocity;
        //Character->Rotation.vx += ((Character->Direction.vz * 400) >> 12);        
        //Character->Rotation.vz -= ((Character->Direction.vx * 400) >> 12);
        if(Character->DashRemaining <= 0)
        {
            Character->Rotation.vx = 0;            
            Character->Rotation.vz = 0;
            
            Character->bDoingDash = 0;
        }
    }
    else if(IncrementalPosition.vx != 0 || IncrementalPosition.vz != 0)
    {
        
    SVECTOR Dir;    
    VectorNormalSS(&IncrementalPosition, &Dir);
    SVECTOR Diff = {Dir.vx -  Character->Direction.vx,Dir.vy -  Character->Direction.vy,Dir.vz -  Character->Direction.vz};
    int LerpSpeed = 8;
    SVECTOR DIffDivided = {Diff.vx / LerpSpeed, Diff.vy / LerpSpeed, Diff.vz / LerpSpeed};
    SVECTOR Summ = {Character->Direction.vx + DIffDivided.vx, Character->Direction.vy + DIffDivided.vy, Character->Direction.vz + DIffDivided.vz};
    Character->Direction =  Summ;

    long atan2 = ratan2(Character->Direction.vx, Character->Direction.vz);
    Character->Rotation.vy = atan2;
       
        FutureLocation.vx = Character->Location.vx + IncrementalPosition.vx;    
        FutureLocation.vz =  Character->Location.vz + IncrementalPosition.vz;
    }

     int bCanMove = 1;
    for(int i = 0; i < Level->NumObjects; i++)
    {
       
        if(dcCollision_SphereOverlapBox(&FutureLocation, 50, &Level->Objects[i]->Location, &Level->Objects[i]->BoxHalfSize))
        {
            bCanMove = 0;
            break;
        }

    }
    if(bCanMove)
    {
        Character->Location = FutureLocation;
    }
    short S = csin(Character->CameraAngle);
    short C = ccos(Character->CameraAngle);
    Character->FrontVector.vx = C;
    Character->FrontVector.vz = S;
}

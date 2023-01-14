#include "dcLevel.h"
#include "dcRender.h"
#include <malloc.h>
#include <libetc.h>
#include <stdio.h>
#include <assert.h>
#include <memory.h>
#include "dcMath.h"
#include "dcMisc.h"

void dcLevel_InitLight(SDC_Level *Level, CVECTOR *AmbientColor)
{
    Level->AmbientColor = *AmbientColor;
}

//Maybe we should use the Render SetLightFunction, but this way maybe we can store more easy the light info of every level (?)
void dcLevel_SetLight(SDC_Level* Level, int LightIndex, SVECTOR* LightDirection, SVECTOR* LightColor)
{
    if(LightIndex >= 3)
    {
        //BRO :(
        return;
    }

    Level->LocalLightMatrix.m[LightIndex][0]= LightDirection->vx;
    Level->LocalLightMatrix.m[LightIndex][1]= LightDirection->vy;
    Level->LocalLightMatrix.m[LightIndex][2]= LightDirection->vz;

    Level->ColorMatrix.m[0][LightIndex] = LightColor->vx;
    Level->ColorMatrix.m[1][LightIndex] = LightColor->vy;
    Level->ColorMatrix.m[2][LightIndex] = LightColor->vz;

}

SDC_Object* dcLevel_AddObject(SDC_Level *Level, SDC_Mesh3D* Mesh, VECTOR* Location, SVECTOR* Rotation, SDC_DrawParams* DrawParams, SDC_Object* Parent, int bHasCollision, VECTOR* BoxHalfCollision)
{
    SDC_Object* Obj = malloc3(sizeof(SDC_Object));
    Obj->Location = *Location; //<----- to Transform
    Obj->Rotation = *Rotation;
    Obj->Scale = (VECTOR){ONE,ONE,ONE};
    Obj->Mesh = Mesh;
    Obj->DrawParams = DrawParams;
    Obj->Parent = Parent;
    Obj->bHasCollision = bHasCollision;
    Obj->BoxHalfSize = *BoxHalfCollision;
    Obj->CharacterParent = NULL;
    //Malloc for every object? or with MaxArray size?
    Level->Objects = realloc3(Level->Objects, (Level->NumObjects + 1) * sizeof(SDC_Object*));

    Level->Objects[Level->NumObjects] = Obj;    
    Level->NumObjects++;
    Level->MaxObjects++;   
    return Obj;
}

SDC_Object* dcLevel_AddObjectOnCharacter(SDC_Level *Level, SDC_Mesh3D* Mesh, VECTOR* Location, SVECTOR* Rotation, SDC_DrawParams* DrawParams, SDC_Character* Parent, int bHasCollision, VECTOR* BoxHalfCollision)

{    
    SDC_Object* Obj = malloc3(sizeof(SDC_Object));
    Obj->Location = *Location; //<----- to Transform
    Obj->Rotation = *Rotation;
    Obj->Mesh = Mesh;
    Obj->DrawParams = DrawParams;
    Obj->CharacterParent = Parent;
    Obj->bHasCollision = bHasCollision;
    Obj->BoxHalfSize = *BoxHalfCollision;
    
    //Malloc for every object? or with MaxArray size?
    Level->Objects = realloc3(Level->Objects, (Level->NumObjects + 1) * sizeof(SDC_Object*));

    Level->Objects[Level->NumObjects] = Obj;    
    Level->NumObjects++;
    Level->MaxObjects++;   
    return Obj;
}

SDC_Character* dcLevel_InitCharacter(SDC_Level *Level, SDC_Mesh3D *Mesh, VECTOR *Location, SDC_DrawParams *DrawParams)
{   
    SDC_Character* NewCharacter = malloc3(sizeof(SDC_Character));
    NewCharacter->Location = *Location; //<----- to Transform
    SVECTOR Rot = {0,0,0};
    NewCharacter->Rotation =Rot;
    NewCharacter->Mesh = Mesh;
    NewCharacter->DrawParams = DrawParams;
    NewCharacter->PlayerIndex = Level->NumCharacters;
    NewCharacter->bDoingParry = 0;
    NewCharacter->bHoldingFire = 0;
    NewCharacter->bDoingDash = 0;

    NewCharacter->ParryFrames = 15;
     NewCharacter->ParryCooldown = 60;
     NewCharacter->ParryCurrentCooldown =0;

    NewCharacter->Lives = 3;

     NewCharacter->DashCooldown = 60;
     NewCharacter->DashCurrentCooldown = 0;
          NewCharacter->FireCooldown = 15;
     NewCharacter->FireCurrentCooldown = 0;
    
    //Malloc for every object? or with MaxArray size?
    Level->Characters = realloc3(Level->Characters, (Level->NumCharacters + 1) * sizeof(SDC_Object*));

    Level->Characters[Level->NumCharacters] = NewCharacter;    
    Level->NumCharacters++;
    return NewCharacter;
}
void dcLevel_AddProjectile(SDC_Level* Level, SDC_Mesh3D* Mesh, VECTOR* Location, SVECTOR* Direction,int Strength, SDC_DrawParams* DrawParams)
{
    SDC_Projectile* NewProjectile = malloc3(sizeof(SDC_Projectile));
    NewProjectile->Location = *Location; //<----- to Transform
    SVECTOR Rot = {0,0,0};
    NewProjectile->Rotation =Rot;
    NewProjectile->Mesh = Mesh;
    NewProjectile->DrawParams = DrawParams;
    NewProjectile->Vox = DC_MIN(DC_MAX(Strength * 2, 10), 200);
    NewProjectile->Voy = DC_MIN(DC_MAX(Strength, 5), 10);;
    NewProjectile->Vy = NewProjectile->Voy;
    NewProjectile->Direction = *Direction;
    NewProjectile->ExplosionRange = 70;
    NewProjectile->Dmg = 1;
   // NewProjectile->Init;
    //NewProjectile->PlayerIndex = Level->NumCharacters;
    //Malloc for every object? or with MaxArray size?
    Level->Projectiles = realloc3(Level->Projectiles, (Level->NumProjectiles + 1) * sizeof(SDC_Projectile*));
    Level->Projectiles[Level->NumProjectiles] = NewProjectile;
    Level->NumProjectiles++;
}
void dcLevel_AddExplotion(SDC_Level *Level, SDC_Mesh3D* Mesh, VECTOR Location, SDC_DrawParams* DrawParams)
{
    SDC_Explotion* Explotion = malloc3(sizeof(SDC_Explotion));
    Explotion->Location = Location; //<----- to Transform
    SVECTOR Rot = {0,0,0};
    Explotion->Rotation =Rot;
    Explotion->Mesh = Mesh;
    Explotion->DrawParams = DrawParams;
    Explotion->InitRadius = 10;
    Explotion->MaxRadius = 200;
    Explotion->CurrentRadius = Explotion->InitRadius;
    Explotion->ExpansionSpeed = 10;

    
    //Malloc for every object? or with MaxArray size?
    Level->Explotions = realloc3(Level->Explotions, (Level->NumExplotions + 1) * sizeof(SDC_Explotion*));
    Level->Explotions[Level->NumExplotions] = Explotion;    
    Level->NumExplotions++;
}
void dcLevel_DestroyProjectile(SDC_Level* Level, int i)
{
    //Explode
    //Change DrawParams in the future
    VECTOR Location = Level->Projectiles[i]->Location;
    dcLevel_AddExplotion(Level, Level->ExplosionMesh, Location, Level->Projectiles[i]->DrawParams);
    //dcLevel_AddExplotion(Level, sphere, &Level->Projectiles[i]->Location, Level->Projectiles[i]->DrawParams, NULL, 0 ,NULL);


    SDC_Projectile** temp = malloc3((Level->NumProjectiles - 1) * sizeof(SDC_Projectile*)); // allocate an array with a size 1 less than the current one

    if (i != 0)
        memcpy(temp, Level->Projectiles, i * sizeof(SDC_Projectile*)); // copy everything BEFORE the index

    if (i != (Level->NumProjectiles - 1))
        memcpy(temp+i, Level->Projectiles+i+1, (Level->NumProjectiles - i - 1) * sizeof(SDC_Projectile*)); // copy everything AFTER the index
    
    free3 (Level->Projectiles[i]);
    free3 (Level->Projectiles);
    
    Level->NumProjectiles--;
    Level->Projectiles = temp;
}

void dcLevel_DestroyExplotion(SDC_Level* Level, int i)
{

    SDC_Explotion** temp = malloc3((Level->NumExplotions - 1) * sizeof(SDC_Explotion*)); // allocate an array with a size 1 less than the current one

    if (i != 0)
        memcpy(temp, Level->Explotions, i * sizeof(SDC_Projectile*)); // copy everything BEFORE the index

    if (i != (Level->NumExplotions - 1))
        memcpy(temp+i, Level->Explotions+i+1, (Level->NumExplotions - i - 1) * sizeof(SDC_Explotion*)); // copy everything AFTER the index
    
    free3 (Level->Explotions[i]);
    free3 (Level->Explotions);
    
    Level->NumExplotions--;
    Level->Explotions = temp;
}

void GetParentTransform(SDC_Object* Object, MATRIX *Transform, MATRIX *OutTransform)
{ 
    if(Object->Parent != NULL)
    {
        GetParentTransform(Object->Parent, Transform, OutTransform);        
        CompMatrix(OutTransform, Transform, OutTransform);        
    }
    else
    {
        RotMatrix(&Object->Rotation, OutTransform);
        TransMatrix(OutTransform, &Object->Location);
        
        ScaleMatrixL(OutTransform, &Object->Scale);
    }
}



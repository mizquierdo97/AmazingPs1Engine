#include "dcLevel.h"
#include "dcRender.h"
#include <malloc.h>
#include <libetc.h>
#include <stdio.h>
#include <assert.h>
#include <memory.h>

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

void dcLevel_AddObject(SDC_Level *Level, SDC_Mesh3D* Mesh, VECTOR* Location, SDC_DrawParams* DrawParams)
{
    SDC_Object Obj;
    Obj.Location = *Location; //<----- to Transform
    SVECTOR Rot = {0,0,0};
    Obj.Rotation =Rot;
    Obj.Mesh = Mesh;
    Obj.DrawParams = DrawParams;

    //Malloc for every object? or with MaxArray size?
    Level->Objects = realloc3(Level->Objects, (Level->NumObjects + 1) * sizeof(SDC_Object));

    Level->Objects[Level->NumObjects] = Obj;
    Level->NumObjects++;
    Level->MaxObjects++;   
}

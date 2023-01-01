#include "dcLevel.h"
#include <malloc.h>
#include <libetc.h>
#include <stdio.h>
#include <assert.h>
#include <memory.h>

void dcLevel_InitLight(SDC_Level *Level, CVECTOR *AmbientColor)
{
    Level->AmbientColor = *AmbientColor;

   /* Level->LocalLightMatrix = {
        0,0,0,0,0,0,0,0,0,
    };
    Level->ColorMatrix = {
        0,0,0,0,0,0,0,0,0,
    };
*/
}

void dcLevel_SetLight(SDC_Level* Level, int LightIndex, SVECTOR* LightDirection, SVECTOR* LightColor)
{
    if(LightIndex >= 3)
    {
        //BRO :(
        return;
    }

    Level->LocalLightMatrix.m[0][LightIndex]= LightDirection->vx;
    Level->LocalLightMatrix.m[1][LightIndex]= LightDirection->vx;
    Level->LocalLightMatrix.m[2][LightIndex]= LightDirection->vx;

    Level->ColorMatrix.m[LightIndex][0] = LightColor->vx;
    Level->ColorMatrix.m[LightIndex][0] = LightColor->vy;
    Level->ColorMatrix.m[LightIndex][0] = LightColor->vz;

}

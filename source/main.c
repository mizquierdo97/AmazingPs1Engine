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
#include "dcMemory.h"
#include "dcMisc.h"
#include "dcCollision.h"
#include "../third_party/modplayer/modplayer.h"

#define CUBESIZE 196 


extern const uint8_t _binary_HIT_STAR_HIT_start[];
#define HITFILE _binary_HIT_STAR_HIT_start

int main(void) 
{
    dcMemory_Init();
    PadInit(0);
    InitGeom();

    SDC_Mesh3D* sphereMesh = dcMisc_generateSphereMesh(CUBESIZE, 7, 7);

    SDC_Render render;
    SDC_Camera camera;
    long distance = 800;
    int  width = 640;
    int  height = 240;

    CVECTOR bgColor = {60, 120, 120};
    dcRender_Init(&render, width, height, bgColor, 4096, 8192, RENDER_MODE_PAL);
    dcCamera_SetScreenResolution(&camera, width, height);
    dcCamera_SetCameraPosition(&camera, 0, 0, distance);
    dcCamera_LookAt(&camera, &VECTOR_ZERO);

    SVECTOR rotation = {0};
    VECTOR translation = {0, 0, 0, 0};
    MATRIX transform;

    SDC_DrawParams drawParams = {
        .tim = NULL,
        .constantColor = {255, 255, 255},
        .bLighting = 1,
        .bUseConstantColor = 1
    };

    CVECTOR ambientColor = {128, 128, 128};
    dcRender_SetAmbientColor(&render, &ambientColor );

    //SVECTOR lightDir = {DC_ONE, 0, 0 };
   // SVECTOR lightColor = {DC_ONE, 0, 0};
    //dcRender_SetLight(&render, 0, &lightDir, &lightColor);

CVECTOR BGc = {150, 50, 75, 0};  
// Back color  
VECTOR  BKc = {128, 128, 128, 0};
// Light rotation angle
SVECTOR lgtang = {0, 0, 0}; 
// These will be used to store the light rotation matrix, cube rotation matrix, and composite light matrix.
MATRIX  rotlgt, rotcube, light;

  
 // Local Light Matrix : Direction and reach of each light source. 
// Each light points in the direction aligned with the axis, hence direction is in the same coordinate system as the PSX (see l.23-30 of this file)
// Negative/positive value denotes light direction on corresponding axis
// -4096 > Value < 4096 denotes reach/intensity of light source
MATRIX lgtmat = {
//      X      Y      Z
        -ONE,  -ONE,   ONE, // Lightsource 1 : here, the light source is at the Bottom-Left of the screen, and points into the screen.
        0,     0,     0, // Lightsource 2
        0,     0,     0, // Lightsource 3
    };
// Local Color Matrix
// Set color of each light source (L)
// Value range : 0 > x < 4096
MATRIX cmat = {
//   L1    L2   L3
    4096,   0,   0, // R
    4096,   0,   0, // G
    4096,   0,   0  // B
    };
        // Set light env
    // Set far color
    SetFarColor( BGc.r, BGc.g, BGc.b );
    // Set Ambient color
    SetBackColor( BKc.vx, BKc.vy, BKc.vz );
    // Set Color matrix
    SetColorMatrix(&cmat);
    // Set Fog settings

    MOD_Load((struct MODFileFormat*)HITFILE);
    while (1) { 
        rotation.vy += 16;

        // Find and apply light rotation matrix
        // Find rotmat from light angles
        RotMatrix_gte(&lgtang, &rotlgt);
        // Find rotmat from cube angles
        RotMatrix_gte(&rotation, &rotcube);  
        // RotMatrix cube * RotMatrix light
        MulMatrix0(&rotcube, &rotlgt, &rotlgt);
        // Light Matrix * RotMatrix light 
        MulMatrix0(&lgtmat, &rotlgt, &light);
        // Set new light matrix 
        SetLightMatrix(&light);

        RotMatrix(&rotation, &transform);
        TransMatrix(&transform, &translation);
        dcCamera_ApplyCameraTransform(&camera, &transform, &transform);

        FntPrint("GameDev Challenge Sphere Demo\n");

        SVECTOR rayDir = { camera.viewMatrix.m[2][0], camera.viewMatrix.m[2][1], camera.viewMatrix.m[2][2] };
        VectorNormalSS(&rayDir, &rayDir);


        dcRender_DrawMesh(&render, sphereMesh, &transform, &drawParams);
       

        dcRender_SwapBuffers(&render);

     
    }

    return 0;
}
#ifndef _CHARACTER_
#define _CHARACTER_

#include <stdlib.h>
#include <libgte.h>
#include "dcLibPsx/include/dcLevel.h"

typedef struct 
{
    VECTOR Location;
    SVECTOR Rotation;
    SDC_Mesh3D Mesh;
    //TEXTURE
    //Life
    //Speed
    //.
    //.
    //.
}SDC_Character;

void UpdateCharacter(SDC_Character* Character)
{
    //Get input

    //Do stuff with input

    //DRAW (Here or with other objects?)
}

#endif
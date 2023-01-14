#include "projectile.h"
#include "dcMath.h"
#include "dcLevel.h"

void UpdateProjectile(SDC_Level* Level, SDC_Projectile* Projectile, int IndexInArray)
{
     Projectile->Location.vx -= DC_MUL(Projectile->Vox , Projectile->Direction.vx);     
     Projectile->Location.vz -= DC_MUL(Projectile->Vox , Projectile->Direction.vz);
     Projectile->Vy = Projectile->Vy - 1;
     Projectile->Location.vy = Projectile->Location.vy + Projectile->Vy;

     if( Projectile->Location.vy < 0)
     {          
          dcLevel_DestroyProjectile(Level, IndexInArray);
     }
}
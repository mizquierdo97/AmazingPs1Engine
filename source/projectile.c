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
          for(int i = 0; i < Level->NumCharacters; i++)
          {
               //if(dcCollision_SphereOverlapBox(Projectile->Location,Projectile->ExplosionRange,Level->Characters[i]->Location,Level->Characters[i]->))
               SVECTOR Diff = {Projectile->Location.vx -  Level->Characters[i]->Location.vx, Projectile->Location.vz - Level->Characters[i]->Location.vz};
               int Dist =  SquareRoot12( DC_MUL(Diff.vx , Diff.vx) + DC_MUL(Diff.vz , Diff.vz));
               //printf("Tank: %d \n",i);
               //printf("Dist: %d \n",Dist);
               if(Dist < Projectile->ExplosionRange){
                    printf("Tank received hit\n");
               }
          }
          dcLevel_DestroyProjectile(Level, IndexInArray);
     }
      
}
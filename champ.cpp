#include "champ.h"
#include <float.h>
#include "Tables.h"

Champ::Champ()
{
  // Maximum de 5 triangles par cube et 3 points par triangle
  pointsMesh=new Vecteur3d[TAILLE_CHAMP*TAILLE_CHAMP*TAILLE_CHAMP*5*3];
  normalesMesh=new Vecteur3d[TAILLE_CHAMP*TAILLE_CHAMP*TAILLE_CHAMP*5*3];

  // nombre de sommets generes pour les triangles
  nbPoints=0;

  // on reserver 1000 cubes pour la pile de l'ago optimisé
  cubesNonExplores.reserve(1000);
};

Champ::~Champ()
{
  delete[] pointsMesh;
  delete[] normalesMesh;
};

// calcul des scalaires du champ
void Champ::calcul(Metaball *liste, int nombre_mb)
{
  int x,y,z,i;
  Vecteur3d pos;

  // pour chaque element du cube
  for (z=0;z<TAILLE_CHAMP;z++)
    {
      for (y=0;y<TAILLE_CHAMP;y++)
	{
	  for (x=0;x<TAILLE_CHAMP;x++)
	    {
	      pos.x=x;
	      pos.y=y;
	      pos.z=z;

	      champ[x][y][z]=0.0f;

	      // on ajoute l'influence de chaque metaball pour cette position dans le cube
	      for (i=0;i<nombre_mb;i++)
		champ[x][y][z]+=liste[i].fonction(pos);
	    }
	}
    }
  
  // pour chaque element du cube, sauf ceux situ�s sur les tranches
  for (z=1;z<TAILLE_CHAMP-1;z++)
    {
      for (y=1;y<TAILLE_CHAMP-1;y++)
	{
	  for (x=1;x<TAILLE_CHAMP-1;x++)
	    {
	      // methode des differences finies
	      gradient[x][y][z].x=champ[x+1][y][z]-champ[x-1][y][z];
	      gradient[x][y][z].y=champ[x][y+1][z]-champ[x][y-1][z];
	      gradient[x][y][z].z=champ[x][y][z+1]-champ[x][y][z-1];
	      gradient[x][y][z]*=-0.5;
	    }
	}
    }
};  

void Champ::ajouteVoisins(int x,int y,int z)
{
  if (x+1<TAILLE_CHAMP)
    cubesNonExplores.push_back(CubeCoord(x+1,y,z));
  if (x-1>=0)
    cubesNonExplores.push_back(CubeCoord(x-1,y,z));

  if (y+1<TAILLE_CHAMP)
    cubesNonExplores.push_back(CubeCoord(x,y+1,z));
  if (y-1>=0)
    cubesNonExplores.push_back(CubeCoord(x,y-1,z));

  if (z+1<TAILLE_CHAMP)
    cubesNonExplores.push_back(CubeCoord(x,y,z+1));
  if (z-1>=0)
    cubesNonExplores.push_back(CubeCoord(x,y,z-1));
};


void Champ::triangulation(float valeurReference)
{
  int x,y,z;

  nbPoints=0;

  for(x=0;x<TAILLE_CHAMP-1;x++)
    for(y=0;y<TAILLE_CHAMP-1;y++)
      for(z=0;z<TAILLE_CHAMP-1;z++)
	marchingCubes(x,y,z,valeurReference);
};

void Champ::triangulationOptimisee(Metaball *liste,int nombre_mb,float valeurReference)
{
  int x,y,z,cx,cy,cz;
  CubeCoord last(0,0,0);

  nbPoints=0;

  //init à 0 du tableau des cubes explores
  for(z=0;z<TAILLE_CHAMP;z++)
    for(y=0;y<TAILLE_CHAMP;y++)
      for(x=0;x<TAILLE_CHAMP;x++)
	cubeTraite[x][y][z]=false;

  for (int n=0;n<nombre_mb;n++)
    {
      // on part du centre
      bool trouveSurface=false;

      cx=(int)liste[n].centre.x;
      cy=(int)liste[n].centre.y;
      cz=(int)liste[n].centre.z;

      // on stoppe lorsqu'on trouve une surface
      // on teste cx car c'est la seule composante qui est incrementee
      // le centre de la sphere est suppose etre dans les limites du champ
      while (!trouveSurface && cx < TAILLE_CHAMP)
	{
	  // si le cube etait deja traite on arrete
	  if (cubeTraite[cx][cy][cz])
	    trouveSurface=true;
	  else
	    {
	      // autrement on le considere comme explore et on regarde
	      // s'il contient des triangles, sinon on avance sur l'axe X
	      cubeTraite[cx][cy][cz]=true;
	      if (marchingCubes(cx,cy,cz,valeurReference))
		trouveSurface=true;
	      else
		cx++;
	    }
	}
   
  
      // une fois la surface trouvee on ajoute les voisins du cube concerne
      ajouteVoisins(cx,cy,cz);
  
      // on commence une boucle d'exploration tant que la pile n'est pas vide
      do {
	// on prend le derniere cube et on regarde s'il avait ete traite
	// sinon on l'enleve de la pile
	last = cubesNonExplores.back();
	if (cubeTraite[last.x][last.y][last.z])
	  cubesNonExplores.pop_back();
	else
	  {
	    // autrement, on l'analyse et s'il n'est pas vide, on rajoute
	    // ses voisins qui sont sans doutes sur la surface
	    if (marchingCubes(last.x,last.y,last.z,valeurReference))
	      {
		cubesNonExplores.pop_back();
		ajouteVoisins(last.x,last.y,last.z);
	      }
	    
	    cubeTraite[last.x][last.y][last.z]=true;
	  }
      } while (!cubesNonExplores.empty());
    }
}

bool Champ::marchingCubes(int x,int y,int z,float valeurReference)
{
  static int n,index,masque;
  static int aretes,A,B;
  static float rapport;

  const static unsigned int sommetDebut[12]={0,1,2,3,4,5,6,7,0,1,2,3};
  const static unsigned int sommetFin[12]={1,2,3,0,5,6,7,4,4,5,6,7};

  static float valeur[8];

  static Vecteur3d position[8];
  static Vecteur3d grad[8];

  static Vecteur3d points[12];
  static Vecteur3d normales[12];

  // on stock les valeurs du champs scalaire pour les sommets
  valeur[7]=champ[x][y][z];
  valeur[4]=champ[x][y][z+1];
  valeur[6]=champ[x+1][y][z];
  valeur[5]=champ[x+1][y][z+1];
  valeur[3]=champ[x][y+1][z];
  valeur[0]=champ[x][y+1][z+1];
  valeur[2]=champ[x+1][y+1][z];
  valeur[1]=champ[x+1][y+1][z+1];

  grad[7]=gradient[x][y][z];
  grad[4]=gradient[x][y][z+1];
  grad[6]=gradient[x+1][y][z];
  grad[5]=gradient[x+1][y][z+1];
  grad[3]=gradient[x][y+1][z];
  grad[0]=gradient[x][y+1][z+1];
  grad[2]=gradient[x+1][y+1][z];
  grad[1]=gradient[x+1][y+1][z+1];

  position[7].set(x,y,z);
  position[4].set(x,y,z+1);
  position[6].set(x+1,y,z);
  position[5].set(x+1,y,z+1);
  position[3].set(x,y+1,z);
  position[0].set(x,y+1,z+1);
  position[2].set(x+1,y+1,z);
  position[1].set(x+1,y+1,z+1);

  masque=1;
  index=0;

  for (n=0;n<8;n++)
    {
      if (valeur[n]>valeurReference)
	index|=masque;
      masque<<=1;
    }
  
  if (index==0 || index==255)
    return false;

  aretes=tableAretes[index];

  for (int n=0,masque=1;n<12;n++)
    {
      A=sommetDebut[n];
      B=sommetFin[n];

      if (aretes & masque) 
	{
	  if (valeur[B]==valeur[A]) 
	    rapport=0.0f;
	  else
	    rapport=(valeurReference-valeur[A])/(valeur[B]-valeur[A]);
	  
	  normales[n]=grad[A]+(grad[B]-grad[A])*rapport;
	  points[n]=position[A]+(position[B]-position[A])*rapport;	  
	}
      masque<<=1;
    }
  
  n=0;
  while (tableTriangles[index][n]!=-1)
    {
      pointsMesh[nbPoints]=points[tableTriangles[index][n]];
      pointsMesh[nbPoints+1]=points[tableTriangles[index][n+1]];
      pointsMesh[nbPoints+2]=points[tableTriangles[index][n+2]];

      normalesMesh[nbPoints]=normales[tableTriangles[index][n]];
      normalesMesh[nbPoints+1]=normales[tableTriangles[index][n+1]];
      normalesMesh[nbPoints+2]=normales[tableTriangles[index][n+2]];

      nbPoints+=3;
      n+=3;
    }
  
  return true;
};


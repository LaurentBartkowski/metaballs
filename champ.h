#ifndef _champs_h
#define _champs_h

#include "headers.h"
#include "Metaball.h"
#include "Vecteur3d.h"

#define TAILLE_CHAMP 32

class CubeCoord
{
 public:
  // attribut
  int x,y,z;

  // constructeurs
  CubeCoord()
    {
    }

  CubeCoord(int X,int Y,int Z):x(X),y(Y),z(Z)
    {
    }

};

class Champ
{
 public:
  // Attributs
  int nbPoints;
  Vecteur3d *pointsMesh;
  Vecteur3d *normalesMesh;

  // Constructeur, destructeur
  Champ();
  ~Champ();
  
  // Méthodes
  void calcul(Metaball *liste, int nombre_mb);
  void triangulation(float valeurReference);
  void triangulationOptimisee(Metaball *liste, int nombre_mb, float valeurReference);

 private:
  float champ[TAILLE_CHAMP][TAILLE_CHAMP][TAILLE_CHAMP];
  Vecteur3d gradient[TAILLE_CHAMP][TAILLE_CHAMP][TAILLE_CHAMP];
  bool cubeTraite[TAILLE_CHAMP][TAILLE_CHAMP][TAILLE_CHAMP];
  vector <CubeCoord> cubesNonExplores;
  
  bool marchingCubes(int x, int y, int z, float valeurReference);
  void ajouteVoisins(int x, int y, int z);
};

#endif

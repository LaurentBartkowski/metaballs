#ifndef _metaball_h_
#define _metaball_h_

#include "Vecteur3d.h"

class Metaball
{
 public:
  // constructeur
  Metaball();

  // attributs
  Vecteur3d centre;

  // méthodes
  float fonction(Vecteur3d pos);

  // destructeur
  ~Metaball();

  
};

#endif

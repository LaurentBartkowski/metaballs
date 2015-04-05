#include "Metaball.h"

// Constructeur
Metaball::Metaball()
{
};

// Methode
// voir doc
float Metaball::fonction(Vecteur3d pos)
{
  static Vecteur3d diff;

  diff=pos-centre;
  
  return 1.0/(1.0+diff.carre_norme());
};

// Desctructeur
Metaball::~Metaball()
{
};

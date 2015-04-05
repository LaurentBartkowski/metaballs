#ifndef _vecteur3d_hpp_
#define _vecteur3d_hpp_

#include <math.h>
#include <float.h>

class Vecteur3d
{
 public:
  // Attributs //
  float x,y,z;

  // Constructeurs //
  Vecteur3d();
  Vecteur3d(float X,float Y, float Z);
  Vecteur3d(const Vecteur3d& v);

  // Methodes //
  void set(float X,float Y,float Z);
  void reset();
  float norme() const;
  float carre_norme() const;
  float normalize();
  
  // surcharges d'operateurs //
  Vecteur3d& operator=(const Vecteur3d& v);
  Vecteur3d operator+(const Vecteur3d& v) const;
  Vecteur3d operator-(const Vecteur3d& v) const;
  Vecteur3d operator-() const;
  Vecteur3d operator*(float scalaire) const;
  Vecteur3d& operator+=(const Vecteur3d& v);
  Vecteur3d& operator-=(const Vecteur3d& v);
  Vecteur3d& operator*=(float scalaire);

  // Destructeur
  ~Vecteur3d();
};

#endif

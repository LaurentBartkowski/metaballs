#include <math.h>
#include <float.h>
#include "Vecteur3d.h"

// Constructeurs //
Vecteur3d::Vecteur3d():x(0.0f),y(0.0f),z(0.0f)
{
};

Vecteur3d::Vecteur3d(float X,float Y, float Z):x(X),y(Y),z(Z)
{
};

Vecteur3d::Vecteur3d(const Vecteur3d& v):x(v.x),y(v.y),z(v.z)
{
};

// Methodes //
void Vecteur3d::set(float X,float Y,float Z)
{
  x=X;
  y=Y;
  z=Z;
};

void Vecteur3d::reset()
{
  x=y=z=0.0f;
};

float Vecteur3d::norme() const
{
  return sqrt(x*x+y*y+z*z);
};

float Vecteur3d::carre_norme() const
{
  return x*x+y*y+z*z;
};

float Vecteur3d::normalize()
{
  float n=norme();
  
  if (n>FLT_EPSILON)
    {
      x/=n;
      y/=n;
      z/=n;
    }
  
  return n;
};
  
// surcharges d'operateurs //
Vecteur3d& Vecteur3d::operator=(const Vecteur3d& v)
{
  x=v.x;
  y=v.y;
  z=v.z;
  
  return *this;
};

Vecteur3d Vecteur3d::operator+(const Vecteur3d& v) const
{
  Vecteur3d t;
  
  t.x=x+v.x;
  t.y=y+v.y;
  t.z=z+v.z;
  
  return t;
};

Vecteur3d Vecteur3d::operator-(const Vecteur3d& v) const
{
  Vecteur3d t;
  
  t.x=x-v.x;
  t.y=y-v.y;
  t.z=z-v.z;
  
  return t;
};

Vecteur3d Vecteur3d::operator-() const
{
  Vecteur3d t;
  
  t.x=-x;
  t.y=-y;
  t.z=-z;
  
  return t;
};

Vecteur3d Vecteur3d::operator*(float scalaire) const
{
  Vecteur3d t;
  
  t.x=scalaire*x;
  t.y=scalaire*y;
  t.z=scalaire*z;
  
  return t;
};

Vecteur3d& Vecteur3d::operator+=(const Vecteur3d& v)
{
  x+=v.x;
  y+=v.y;
  z+=v.z;
  
  return *this;
};

Vecteur3d& Vecteur3d::operator-=(const Vecteur3d& v)
{
  x-=v.x;
  y-=v.y;
  z-=v.z;
  
  return *this;
};

Vecteur3d& Vecteur3d::operator*=(float scalaire)
{
  x*=scalaire;
  y*=scalaire;
  z*=scalaire;
  
  return *this;
};

// Destructeur
Vecteur3d::~Vecteur3d()
{
};

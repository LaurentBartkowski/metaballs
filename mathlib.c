#include "mathlib.h"

#include <math.h>

void ReduceToUnit(float vector[3])
{
  float length;

  length = (float)sqrt((vector[0]*vector[0]) + (vector[1]*vector[1]) + (vector[2]*vector[2]));
  
  if(length == 0.0f)
    length = 1.0f;
  
  vector[0] /= length;
  vector[1] /= length;
  vector[2] /= length;
}

void calcNormal(float v[3][3], float out[3])
{
  float v1[3],v2[3];
  static const int x = 0;
  static const int y = 1;
  static const int z = 2;
  
  v1[x] = v[0][x] - v[1][x];
  v1[y] = v[0][y] - v[1][y];
  v1[z] = v[0][z] - v[1][z];

  v2[x] = v[1][x] - v[2][x];
  v2[y] = v[1][y] - v[2][y];
  v2[z] = v[1][z] - v[2][z];

  out[x] = v1[y]*v2[z] - v1[z]*v2[y];
  out[y] = v1[z]*v2[x] - v1[x]*v2[z];
  out[z] = v1[x]*v2[y] - v1[y]*v2[x];
  
  ReduceToUnit(out);
}


#include "objets.h"
#include "textures.h"
#include <GL/glx.h>
#include <GL/gl.h>
#include <X11/extensions/xf86vmode.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <math.h>

GLuint  texture[1];
GLfloat tetra_rot_x,tetra_rot_y,tetra_rot_z;

int tetraedreInitialiser()
{
  tetra_rot_x=0.0;
  tetra_rot_y=0.0;
  tetra_rot_z=0.0;

  if (!loadGLTextures(&texture[0]))
    {
      printf("Erreur au chargement de la texture - tetrahedreInitialiser\n");
      return False;
    }
  
    return True;
}

void tetraedreUpdater()
{
  tetra_rot_x+=0.2;
  tetra_rot_y+=0.2;
  tetra_rot_z+=0.2;
}

void tetraedreAfficher()
{
  /* Tetrahedre contenu dans un cercle de rayon 1 et centré en (0,0,0) */
  GLfloat unit=sqrt(3)/3;

  GLfloat glfMaterialColor[]={0.5f,0.5f,0.5f,1.0f};
  GLfloat specular[]={1.0f,1.0f,1.0f,1.0f};
  
  glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,glfMaterialColor);
  glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,specular);

  glPushMatrix();
  glLoadIdentity();
  
  glTranslatef(0.0f,0.0f,-6.0f);
  glRotatef(tetra_rot_x,1.0f,0.0f,0.0f);
  glRotatef(tetra_rot_y,0.0f,1.0f,0.0f);
  glRotatef(tetra_rot_z,0.0f,0.0f,1.0f);
 
  /* le tetrahedre en question */
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D,texture[0]);

  glColor3f(1.0f,1.0f,1.0f);

  glBegin(GL_TRIANGLES);


  glTexCoord2f(0.0f,0.0f);
  glVertex3f(unit,unit,unit);
  glTexCoord2f(0.0f,1.0f);
  glVertex3f(-unit,-unit,unit);
  glTexCoord2f(1.0f,0.0f);
  glVertex3f(unit,-unit,-unit);

  glTexCoord2f(0.0f,1.0f);
  glVertex3f(-unit,-unit,unit);
  glTexCoord2f(1.0f,0.0f);
  glVertex3f(unit,-unit,-unit);
  glTexCoord2f(1.0f,1.0f);
  glVertex3f(-unit,unit,-unit);

  glTexCoord2f(0.0f,0.0f);
  glVertex3f(unit,unit,unit);
  glTexCoord2f(1.0f,1.0f);
  glVertex3f(-unit,unit,-unit);
  glTexCoord2f(1.0f,0.0f);
  glVertex3f(unit,-unit,-unit);

  glTexCoord2f(1.0f,1.0f);
  glVertex3f(-unit,unit,-unit);
  glTexCoord2f(0.0f,0.0f);
  glVertex3f(unit,unit,unit);
  glTexCoord2f(0.0f,1.0f);
  glVertex3f(-unit,-unit,unit);

  glEnd();

  glPopMatrix();
}

#include <iostream>
#include <string>
using namespace std;

#include <SDL.h>
#include <SDL_image.h>

#include "headers.h"
#include <GL/glu.h>
#include "textures.h"
#include "mathlib.h"
#include "objets.h"
#include "Vecteur3d.h"
#include "Metaball.h"
#include "champ.h"

#define CODE_ERREUR 1
#define CODE_OK     0

# define PI 3.141592654

// Affichage-------------
#define LARGEUR 800
#define HAUTEUR 600
#define ECRAN_BPP 24
//#define FULLSCREEN


// SDL--------------------
long temps;
SDL_Surface *surface=NULL;

// Metaballs--------------
#define NB_BALLS 7
#define FOV 45.0f
float valeurReference=0.1f;
Metaball listeMetaballs[NB_BALLS];
Champ *champ=NULL;

// lumiere
GLfloat lpos[]={16.0f,0.0f,-4.0f,1.0f};
GLfloat ldif[]={1.0f,1.0f,1.0f,1.0f};
GLfloat lamb[]={0.0f,0.0f,0.0f,1.0f};
GLfloat lspe[]={1.0f,1.0f,1.0f,1.0f};

// material
GLfloat ambientMaterial[]={0.11f,0.06f,0.11f,1.0f};
GLfloat diffuseMaterial[]={0.43f,0.47f,0.54f,1.0f};
GLfloat specularMaterial[]={0.33f,0.33f,0.52f,1.0f};
GLfloat emissionMaterial[]={0.0f,0.0f,0.0f,0.0f};
GLfloat shininessMaterial=10.0f;

GLfloat lwhite[]={1.0f,1.0f,1.0f,0.0f};
GLfloat lred[]={1.0f,0.0f,0.0f,0.0f};
GLfloat lblue[]={0.0f,0.0f,1.0f,0.0f};
GLfloat lgreen[]={0.0f,1.0f,0.0f,0.0f};
GLfloat lblack[]={0.0f,0.0f,0.0f,0.0f};

// Spherical Environment Mapping
GLuint texture_name;
GLuint objet;
SDL_Surface * sdltexture;

// Divers
GLUquadricObj *sphere;
GLuint lsphere; // liste


/********************************************************************************/
// INIT
/********************************************************************************/
SDL_Surface* initSDL()
{
  SDL_Surface* surface=NULL;

  int res=SDL_Init(SDL_INIT_VIDEO);

  if (res==-1)
    {
      cout << "Res = -1\n";
      SDL_Quit();
    }

  const SDL_VideoInfo* videoInfo=NULL;
  videoInfo=SDL_GetVideoInfo();

  if (!videoInfo)
    {
      cout<<"Pas de videoInfo!\n";
      SDL_Quit();
    }
  
  int videoParams=SDL_OPENGL|SDL_GL_DOUBLEBUFFER|SDL_HWPALETTE;
  
#ifdef FULLSCREEN
  videoParams|=SDL_FULLSCREEN;
#endif

  videoParams|=((videoInfo->hw_available)?SDL_HWSURFACE:SDL_SWSURFACE);
  if (videoInfo->blit_hw)
    videoParams|=SDL_HWACCEL;

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);

  surface=SDL_SetVideoMode(LARGEUR,HAUTEUR,ECRAN_BPP,videoParams);
  
  if (!surface)
    {
      cout<<"Pas de surface !\n";
      SDL_Quit();
    }

  return surface;
}

/********************************************************************************/
// OPENGL
/********************************************************************************/

void redimensionner(int largeur, int hauteur)
{
  largeur=max(largeur,1);
  hauteur=max(hauteur,1);

  GLfloat rapport=(GLfloat)largeur/(GLfloat)hauteur;

  glViewport(0,0,(GLint)largeur,(GLint)hauteur);
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  /*
  glMaterialfv(GL_FRONT,GL_AMBIENT,ambientMaterial);
  glMaterialfv(GL_FRONT,GL_DIFFUSE,diffuseMaterial);
  glMaterialfv(GL_FRONT,GL_SPECULAR,specularMaterial);
  glMaterialf(GL_FRONT,GL_SHININESS,shininessMaterial);
  glMaterialfv(GL_FRONT,GL_EMISSION,emissionMaterial);
  */

  gluPerspective(FOV,rapport,0.1f,1000.0f);  
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}


/********************************************************************************/
// AFFICHAGE
/********************************************************************************/

void drawGL_Repere(void)
{
  glPushMatrix();

  glScalef(1.0f,1.0f,1.0f);
  
  glBegin(GL_LINES);

  glColor4f(1.0f,0.0f,0.0f,1.0f);
  glVertex3f(0.0f,0.0f,0.0f);
  glVertex3f(1.0f,0.0f,0.0f);

  glColor4f(0.0f,1.0f,0.0f,1.0f);
  glVertex3f(0.0f,0.0f,0.0f);
  glVertex3f(0.0f,1.0f,0.0f);

  glColor4f(0.0f,0.0f,1.0f,1.0f);
  glVertex3f(0.0f,0.0f,0.0f);
  glVertex3f(0.0f,0.0f,1.0f);

  glEnd();

  glPopMatrix();
}

/********************************************************************************/
// METABALLS
/********************************************************************************/

void initMetaballs()
{
  champ=new Champ();
}

void detruireMetaballs()
{
  delete champ;
}

void initOpenGLforMetaballs()
{
  cout<<"Init OpenGL for metaballs...\n";

  glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
  glClearColor(0.0f,0.0f,0.0f,0.0f);
  glClearDepth(1.0f);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_NORMALIZE);

  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);

  glLightfv(GL_LIGHT1,GL_DIFFUSE,ldif);
  glLightfv(GL_LIGHT1,GL_AMBIENT,lamb);
  glLightfv(GL_LIGHT1,GL_SPECULAR,lspe);
  glLightfv(GL_LIGHT1,GL_POSITION,lpos);

  sphere=gluNewQuadric();
  gluQuadricDrawStyle(sphere,GLU_FILL);
  gluQuadricNormals(sphere,GLU_SMOOTH);
  
  lsphere=glGenLists(1);
  glNewList(lsphere,GL_COMPILE);
  gluSphere(sphere,1.0,15,10);
  glEndList();

  // La texture pour l'environment mapping

  cout<<"Textures loading...\n";

  glGenTextures (1, &texture_name);
  glBindTexture (GL_TEXTURE_2D, texture_name);
	
  // Parametrage de la texture.
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  glTexGeni(GL_S,GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
  glTexGeni(GL_T,GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

  // Chargement du fichier.
  sdltexture = IMG_Load ("./media/sphere.png");
  
  // Jonction entre OpenGL et SDL.
  glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, sdltexture->w, sdltexture->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, sdltexture->pixels);  
}

void calculMetaballs()
{
  for (int n=0;n<NB_BALLS;n++)
    {
      listeMetaballs[n].centre.x=16+8*sin(n+temps*0.0005);
      listeMetaballs[n].centre.y=16+8*cos(2*n+temps*0.0005);
      listeMetaballs[n].centre.z=16+8*sin(3*n+temps*0.0001);
    }
  
  champ->calcul((Metaball *)&listeMetaballs,NB_BALLS);
  //champ->triangulation(valeurReference);
  champ->triangulationOptimisee((Metaball *)&listeMetaballs,NB_BALLS,valeurReference);

}

void afficherMetaballs(float zoom)
{
  glLoadIdentity();

  // Fil de fer      
  //glPolygonMode(GL_FRONT,GL_LINE);
  //
  // Surfaces pleines
  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

  // Gouraud
   glShadeModel(GL_SMOOTH);
  // Flat
  // glShadeModel(GL_FLAT);

  glEnable(GL_DEPTH_TEST);

  glTranslatef(-0.5f,-0.5f,zoom);
  glRotatef(temps*0.01,1,1,1);

  glScalef(1.0f/TAILLE_CHAMP,1.0f/TAILLE_CHAMP,1.0f/TAILLE_CHAMP);

  //  drawGL_Repere();

  glEnable(GL_LIGHT1);
  glEnable(GL_LIGHTING);

  // material traditionnel
  /*
  glMaterialfv(GL_FRONT,GL_AMBIENT,ambientMaterial);
  glMaterialfv(GL_FRONT,GL_DIFFUSE,diffuseMaterial);
  glMaterialfv(GL_FRONT,GL_SPECULAR,specularMaterial);
  glMaterialf(GL_FRONT,GL_SHININESS,shininessMaterial);
  glMaterialfv(GL_FRONT,GL_EMISSION,emissionMaterial);
  */

  // Environment mapping
  glDisable(GL_CULL_FACE);
  
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D,texture_name);

  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

  glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

  glEnable(GL_TEXTURE_GEN_S);
  glEnable(GL_TEXTURE_GEN_T);

  glBegin(GL_TRIANGLES);

  for (int n=0;n<champ->nbPoints;n+=3)
    {
      glNormal3f(champ->normalesMesh[n].x,champ->normalesMesh[n].y,champ->normalesMesh[n].z);
      glVertex3f(champ->pointsMesh[n].x,champ->pointsMesh[n].y,champ->pointsMesh[n].z);
      
      glNormal3f(champ->normalesMesh[n+1].x,champ->normalesMesh[n+1].y,champ->normalesMesh[n+1].z);
      glVertex3f(champ->pointsMesh[n+1].x,champ->pointsMesh[n+1].y,champ->pointsMesh[n+1].z);
      
      glNormal3f(champ->normalesMesh[n+2].x,champ->normalesMesh[n+2].y,champ->normalesMesh[n+2].z);
      glVertex3f(champ->pointsMesh[n+2].x,champ->pointsMesh[n+2].y,champ->pointsMesh[n+2].z);
    }

  glEnd();

  glDisable(GL_LIGHTING);
  glDisable(GL_TEXTURE_2D);
}


/********************************************************************************/
// MAIN
/********************************************************************************/

int main()
{
  int fini=1;
  int frames=0;
  long tempsDebut=SDL_GetTicks();
  float fps=0.0f;

  SDL_Event evenement;
  
  surface=initSDL();

  // metaballs
  initOpenGLforMetaballs();
  initMetaballs();
  
  redimensionner(LARGEUR,HAUTEUR);
  
  while(fini!=0)
    {
      temps=SDL_GetTicks();
      frames++;
      
      while(SDL_PollEvent(&evenement))
	{
	  switch(evenement.type)
	    {
	    case SDL_KEYDOWN:
	      if (evenement.key.keysym.sym == SDLK_ESCAPE);
	      fini=0;
	      break;
	    case SDL_QUIT:
	      fini=0;
	      break;
	    default:
	      break;
	    }
	}

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      glEnable(GL_DEPTH_TEST);

      // Equation parametrique d'une sphere
      // pour le déplacement de la lumiere
      // u et v varient de 0 a 2pi
      lpos[0] = 24*(0.5+cos(temps*0.001)*cos(temps*0.002));
      lpos[1] = 24*(0.5+cos(temps*0.001)*sin(temps*0.002));
      lpos[2] = 24*(0.5+sin(temps*0.001));
      glLightfv(GL_LIGHT1,GL_POSITION,lpos);
      
      glPushMatrix();
      glTranslatef(lpos[0],lpos[2],lpos[3]);
      glColor3f(1.0f,1.0f,1.0f);
      glCallList(lsphere);
      glPopMatrix();


      calculMetaballs();
      afficherMetaballs(-2.0);


      if (SDL_GetTicks()-tempsDebut>1000)
	{
	  fps=1000.0f * (float)frames / (float) (SDL_GetTicks()-tempsDebut);
	  frames=0;
	  tempsDebut=SDL_GetTicks();
	  
	  cout<<fps<<" fps\n";
	}
      
      SDL_GL_SwapBuffers();
    }

  detruireMetaballs();
  gluDeleteQuadric(sphere);
  SDL_Quit();
  return CODE_OK;
}


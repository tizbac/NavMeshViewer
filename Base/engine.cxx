#include "engine.h"
#include <cmath>
#include <sys/time.h>


GLboolean CheckExtension( char *extName )
{
        /*
         ** Search for extName in the extensions string.  Use of strstr()
         ** is not sufficient because extension names can be prefixes of
         ** other extension names.  Could use strtok() but the constant
         ** string returned by glGetString can be in read-only memory.
         */
        char *p = (char *) glGetString(GL_EXTENSIONS);
        char *end;
        int extNameLen;

        extNameLen = strlen(extName);
        end = p + strlen(p);
    
        while (p < end) {
            int n = strcspn(p, " ");
            if ((extNameLen == n) && (strncmp(extName, p, n) == 0)) {
                return GL_TRUE;
            }
            p += (n + 1);
        }
        return GL_FALSE;
}
double getcurrenttime()
{
  struct timeval t;
  gettimeofday(&t,NULL);
  double ti = 0.0;
  ti = t.tv_sec;
  ti += t.tv_usec/1000000.0;
  return ti;
}
unsigned int MT_rand()
{
  return rand();
}
bool engine::Init3d(int width,int height,int bpp,bool fullscreen)
{
	//this->world = dWorldCreate();
	this->bpp = bpp;
	this->height = height;
	this->width = width;
	this->fullscreen = fullscreen;
	int r = SDL_Init(SDL_INIT_VIDEO);
	if ( r != 0)
	{
	  printf("Errore di inizializzazione di SDL: Codice %i :\n",r,SDL_GetError());
	  exit(1);
	}
	this->screen = SDL_SetVideoMode(this->width,this->height,this->bpp,SDL_OPENGL);//Avvia SDL
	if ( not this->screen)
	{
	  printf("Impossibile impostare la modalità video: %s\n",SDL_GetError());
	  exit(1);
	}
	glewInit();//Avvia GLEW per poter utilizzare GLSL
	if (this->fullscreen)
		SDL_WM_ToggleFullScreen(this->screen);

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE,5);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,5);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,5);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,32);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);//Abilita il controllo della profondità
	//glEnable(GL_CULL_FACE);//Abilita il rendering solo per le facce frontali
	glCullFace(GL_BACK);
	//glEnable(GL_COLOR_MATERIAL);//Abilita i materiali personalizzati
	glClearColor(0.0,0.0,0.0,1);
	glEnable(GL_LIGHTING);//Illuminazione
	glDisable(GL_TEXTURE_2D);//texture
	glEnable(GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);//Goraud shading
	GLfloat h = (GLfloat) this->height / (GLfloat) this->width;
	glViewport(0,0,this->width,this->height);
	glGetIntegerv(GL_VIEWPORT,this->view);
	glMatrixMode(GL_PROJECTION);//Imposta la proiezione
	glLoadIdentity();
        fov = 80.0;
        gluPerspective(fov,(GLfloat)this->width/(GLfloat)this->height,0.1f,2000.0f);
	this->up = false;
	this->down = false;
	this->left = false;
	this->right = false;
        
	glMatrixMode(GL_MODELVIEW);
	glGetDoublev(GL_PROJECTION_MATRIX, this->proj);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
	//glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glPolygonOffset(-3.0f, -3.0f);
	glLoadIdentity();
	return true;
}
inline void engine::ExtractFrustum()
{
	float   proj[16];
	float   modl[16];
	float   clip[16];
	float   t;

	/* Get the current PROJECTION matrix from OpenGL */
	glGetFloatv( GL_PROJECTION_MATRIX, proj );

	/* Get the current MODELVIEW matrix from OpenGL */
	glGetFloatv( GL_MODELVIEW_MATRIX, modl );

	/* Combine the two matrices (multiply projection by modelview) */
	clip[ 0] = modl[ 0] * proj[ 0] + modl[ 1] * proj[ 4] + modl[ 2] * proj[ 8] + modl[ 3] * proj[12];
	clip[ 1] = modl[ 0] * proj[ 1] + modl[ 1] * proj[ 5] + modl[ 2] * proj[ 9] + modl[ 3] * proj[13];
	clip[ 2] = modl[ 0] * proj[ 2] + modl[ 1] * proj[ 6] + modl[ 2] * proj[10] + modl[ 3] * proj[14];
	clip[ 3] = modl[ 0] * proj[ 3] + modl[ 1] * proj[ 7] + modl[ 2] * proj[11] + modl[ 3] * proj[15];

	clip[ 4] = modl[ 4] * proj[ 0] + modl[ 5] * proj[ 4] + modl[ 6] * proj[ 8] + modl[ 7] * proj[12];
	clip[ 5] = modl[ 4] * proj[ 1] + modl[ 5] * proj[ 5] + modl[ 6] * proj[ 9] + modl[ 7] * proj[13];
	clip[ 6] = modl[ 4] * proj[ 2] + modl[ 5] * proj[ 6] + modl[ 6] * proj[10] + modl[ 7] * proj[14];
	clip[ 7] = modl[ 4] * proj[ 3] + modl[ 5] * proj[ 7] + modl[ 6] * proj[11] + modl[ 7] * proj[15];

	clip[ 8] = modl[ 8] * proj[ 0] + modl[ 9] * proj[ 4] + modl[10] * proj[ 8] + modl[11] * proj[12];
	clip[ 9] = modl[ 8] * proj[ 1] + modl[ 9] * proj[ 5] + modl[10] * proj[ 9] + modl[11] * proj[13];
	clip[10] = modl[ 8] * proj[ 2] + modl[ 9] * proj[ 6] + modl[10] * proj[10] + modl[11] * proj[14];
	clip[11] = modl[ 8] * proj[ 3] + modl[ 9] * proj[ 7] + modl[10] * proj[11] + modl[11] * proj[15];

	clip[12] = modl[12] * proj[ 0] + modl[13] * proj[ 4] + modl[14] * proj[ 8] + modl[15] * proj[12];
	clip[13] = modl[12] * proj[ 1] + modl[13] * proj[ 5] + modl[14] * proj[ 9] + modl[15] * proj[13];
	clip[14] = modl[12] * proj[ 2] + modl[13] * proj[ 6] + modl[14] * proj[10] + modl[15] * proj[14];
	clip[15] = modl[12] * proj[ 3] + modl[13] * proj[ 7] + modl[14] * proj[11] + modl[15] * proj[15];

	/* Extract the numbers for the RIGHT plane */
	frustum[0][0] = clip[ 3] - clip[ 0];
	frustum[0][1] = clip[ 7] - clip[ 4];
	frustum[0][2] = clip[11] - clip[ 8];
	frustum[0][3] = clip[15] - clip[12];

	/* Normalize the result */
	t = sqrt( frustum[0][0] * frustum[0][0] + frustum[0][1] * frustum[0][1] + frustum[0][2] * frustum[0][2] );
	frustum[0][0] /= t;
	frustum[0][1] /= t;
	frustum[0][2] /= t;
	frustum[0][3] /= t;

	/* Extract the numbers for the LEFT plane */
	frustum[1][0] = clip[ 3] + clip[ 0];
	frustum[1][1] = clip[ 7] + clip[ 4];
	frustum[1][2] = clip[11] + clip[ 8];
	frustum[1][3] = clip[15] + clip[12];

	/* Normalize the result */
	t = sqrt( frustum[1][0] * frustum[1][0] + frustum[1][1] * frustum[1][1] + frustum[1][2] * frustum[1][2] );
	frustum[1][0] /= t;
	frustum[1][1] /= t;
	frustum[1][2] /= t;
	frustum[1][3] /= t;

	/* Extract the BOTTOM plane */
	frustum[2][0] = clip[ 3] + clip[ 1];
	frustum[2][1] = clip[ 7] + clip[ 5];
	frustum[2][2] = clip[11] + clip[ 9];
	frustum[2][3] = clip[15] + clip[13];

	/* Normalize the result */
	t = sqrt( frustum[2][0] * frustum[2][0] + frustum[2][1] * frustum[2][1] + frustum[2][2] * frustum[2][2] );
	frustum[2][0] /= t;
	frustum[2][1] /= t;
	frustum[2][2] /= t;
	frustum[2][3] /= t;

	/* Extract the TOP plane */
	frustum[3][0] = clip[ 3] - clip[ 1];
	frustum[3][1] = clip[ 7] - clip[ 5];
	frustum[3][2] = clip[11] - clip[ 9];
	frustum[3][3] = clip[15] - clip[13];

	/* Normalize the result */
	t = sqrt( frustum[3][0] * frustum[3][0] + frustum[3][1] * frustum[3][1] + frustum[3][2] * frustum[3][2] );
	frustum[3][0] /= t;
	frustum[3][1] /= t;
	frustum[3][2] /= t;
	frustum[3][3] /= t;

	/* Extract the FAR plane */
	frustum[4][0] = clip[ 3] - clip[ 2];
	frustum[4][1] = clip[ 7] - clip[ 6];
	frustum[4][2] = clip[11] - clip[10];
	frustum[4][3] = clip[15] - clip[14];

	/* Normalize the result */
	t = sqrt( frustum[4][0] * frustum[4][0] + frustum[4][1] * frustum[4][1] + frustum[4][2] * frustum[4][2] );
	frustum[4][0] /= t;
	frustum[4][1] /= t;
	frustum[4][2] /= t;
	frustum[4][3] /= t;

	/* Extract the NEAR plane */
	frustum[5][0] = clip[ 3] + clip[ 2];
	frustum[5][1] = clip[ 7] + clip[ 6];
	frustum[5][2] = clip[11] + clip[10];
	frustum[5][3] = clip[15] + clip[14];

	/* Normalize the result */
	t = sqrt( frustum[5][0] * frustum[5][0] + frustum[5][1] * frustum[5][1] + frustum[5][2] * frustum[5][2] );
	frustum[5][0] /= t;
	frustum[5][1] /= t;
	frustum[5][2] /= t;
	frustum[5][3] /= t;
}
bool engine::pointinview( float x, float y, float z )
{
	int p;

	for( p = 0; p < 6; p++ )
		if( frustum[p][0] * x + frustum[p][1] * y + frustum[p][2] * z + frustum[p][3] <= 0 )
			return false;
	return true;
}
void engine::clearscreen(float r,float g,float b,float a,bool tks)
{
  glClearColor(r,g,b,a);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  if ( tks)
  {
    this->cleartime = SDL_GetTicks();
	

    this->mouserelx = 0;
    this->mouserely = 0;

	while ( SDL_PollEvent(&this->ev)){
        switch (this->ev.type)
        {
            case SDL_MOUSEMOTION:
                this->mousex = ev.motion.x;
                this->mousey = ev.motion.y;
                this->mouserelx = ev.motion.xrel;
                this->mouserely = ev.motion.yrel;
                break;
            case SDL_MOUSEBUTTONDOWN:
                this->mouseleft = ev.button.button == SDL_BUTTON_LEFT && ev.button.state == SDL_PRESSED;
                this->mouseright = ev.button.button == SDL_BUTTON_RIGHT && ev.button.state == SDL_PRESSED;
                break;
            case SDL_MOUSEBUTTONUP:
                this->mouseleft = ev.button.button == SDL_BUTTON_LEFT && ev.button.state == SDL_PRESSED;
                this->mouseright = ev.button.button == SDL_BUTTON_RIGHT && ev.button.state == SDL_PRESSED;
                break;

             case SDL_KEYDOWN:
             // printf("keydown %i\n" , ev.key.keysym.sym);
              switch(ev.key.keysym.sym){
                case SDLK_LEFT:
                   this->left = ev.key.state == SDL_PRESSED;
                  break;
                case SDLK_RIGHT:
                  this->right = ev.key.state == SDL_PRESSED;
                  break;
                case SDLK_UP:
                  this->up = ev.key.state == SDL_PRESSED;
                  break;
                case SDLK_DOWN:
                  this->down = ev.key.state == SDL_PRESSED;
                  break;
                default:
                  break;}
            case SDL_KEYUP:
             // printf("keydown %i\n" , ev.key.keysym.sym);
              switch(ev.key.keysym.sym){
                case SDLK_LEFT:
                   this->left = ev.key.state == SDL_PRESSED;
                  break;
                case SDLK_RIGHT:
                  this->right = ev.key.state == SDL_PRESSED;
                  break;
                case SDLK_UP:
                  this->up = ev.key.state == SDL_PRESSED;
                  break;
                case SDLK_DOWN:
                  this->down = ev.key.state == SDL_PRESSED;
                  break;
                default:
                  break;}


        }
	}
  }
}
bool engine::getkey(char key)
{

	this->keys = SDL_GetKeyState(NULL);
	if (this->keys[key])
	{
		return true;
	}else{
		return false;
	}
}
void engine::display(void)
{
	SDL_GL_SwapBuffers();
	this->rendertime = SDL_GetTicks()-this->cleartime;
}
bool engine::Terminate(void)
{
	SDL_Quit();
	//dWorldDestroy(this->world);
	return true;
}
bool engine::ToggleFullScreen(void)
{
	if (this->screen == NULL)
		return false;

	SDL_WM_ToggleFullScreen(this->screen);
	return true;
}
float engine::getfps(void)
{
	float tmp = 0.0;
	tmp = (this->rendertime > 0) ? 1000.0f / rendertime : 0.0f;
	return tmp;
}
void engine::PushODETranform(const float pos[3],const float R[12])
{
  glPushMatrix();
  GLfloat matrix[16];
  matrix[0]=R[0];
  matrix[1]=R[4];
  matrix[2]=R[8];
  matrix[3]=0;
  matrix[4]=R[1];
  matrix[5]=R[5];
  matrix[6]=R[9];
  matrix[7]=0;
  matrix[8]=R[2];
  matrix[9]=R[6];
  matrix[10]=R[10];
  matrix[11]=0;
  matrix[12]=pos[0];
  matrix[13]=pos[1];
  matrix[14]=pos[2];
  matrix[15]=1;
  glMultMatrixf (matrix);
}
void engine::PushTransform(float x,float y,float z,float rx,float ry,float rz)
{


		glPushMatrix();
		glTranslatef(x,y,z);
		glRotatef(rx,1,0,0);
		glRotatef(ry,0,1,0);
		glRotatef(rz,0,0,1);


}
void engine::Transform(float x,float y,float z,float rx,float ry,float rz)
{



		glTranslatef(x,y,z);
		glRotatef(rx,1,0,0);
		glRotatef(ry,0,1,0);
		glRotatef(rz,0,0,1);


}
void engine::PopTransform()
{
  glPopMatrix();

}
void engine::SetCameraPos(float x,float y,float z)
{


    this->camx = x;
    this->camy = y;
    this->camz = z;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	gluPerspective(fov,(GLfloat)this->width/(GLfloat)this->height,0.1f,1000.0f);
    glRotatef(this->camrx,1.0,0.0,0.0);
    glRotatef(this->camry,0.0,1.0,0.0);
    glRotatef(this->camrz,0.0,0.0,1.0);
    glTranslatef(-this->camx,-this->camy,-this->camz);
    glGetDoublev(GL_PROJECTION_MATRIX, this->proj);
    glMatrixMode(GL_MODELVIEW);
    ExtractFrustum();


}
void engine::SetCameraRotation(float x,float y,float z)
{

    this->camrx = x;
    this->camry = y;
    this->camrz = z;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	gluPerspective(fov,(GLfloat)this->width/(GLfloat)this->height,0.1f,1000.0f);
    glRotatef(this->camrx,1.0,0.0,0.0);
    glRotatef(this->camry,0.0,1.0,0.0);
    glRotatef(this->camrz,0.0,0.0,1.0);
    glTranslatef(-this->camx,-this->camy,-this->camz);
    glGetDoublev(GL_PROJECTION_MATRIX, this->proj);
    glMatrixMode(GL_MODELVIEW);
    ExtractFrustum();

}
void engine::SetCameraLookAt(float x,float y,float z)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(fov,(GLfloat)this->width/(GLfloat)this->height,0.1f,1000.0f);
  glRotatef(this->camrx,1.0,0.0,0.0);
  glRotatef(this->camry,0.0,1.0,0.0);
  glRotatef(this->camrz,0.0,0.0,1.0);
  glTranslatef(-this->camx,-this->camy,-this->camz);
  gluLookAt(this->camx,this->camy,this->camz,x,y,z,0,1,0);
  
  glGetDoublev(GL_PROJECTION_MATRIX, this->proj);
  glMatrixMode(GL_MODELVIEW);
  ExtractFrustum();
}
void engine::OrthoMode()
{
      glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
  gluOrtho2D(0,this->width,0,this->height);
  glMatrixMode(GL_MODELVIEW);
  glDisable(GL_LIGHTING);
  ExtractFrustum();
}
void engine::NormalMode()
{
   glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(fov,(GLfloat)this->width/(GLfloat)this->height,0.1f,1000.0f);
  glRotatef(this->camrx,1.0,0.0,0.0);
  glRotatef(this->camry,0.0,1.0,0.0);
  glRotatef(this->camrz,0.0,0.0,1.0);
  glTranslatef(-this->camx,-this->camy,-this->camz);
  
  glGetDoublev(GL_PROJECTION_MATRIX, this->proj);
  glMatrixMode(GL_MODELVIEW);
    ExtractFrustum();
}
int engine::GetMouseX()
{




}
int engine::GetMouseY()
{






}



/* MD5
 converted to C++ class by Frank Thilo (thilo@unix-ag.org)
 for bzflag (http://www.bzflag.org)

   based on:

   md5.h and md5.c
   reference implemantion of RFC 1321

   Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
rights reserved.

License to copy and use this software is granted provided that it
is identified as the "RSA Data Security, Inc. MD5 Message-Digest
Algorithm" in all material mentioning or referencing this software
or this function.

License is also granted to make and use derivative works provided
that such works are identified as "derived from the RSA Data
Security, Inc. MD5 Message-Digest Algorithm" in all material
mentioning or referencing the derived work.

RSA Data Security, Inc. makes no representations concerning either
the merchantability of this software or the suitability of this
software for any particular purpose. It is provided "as is"
without express or implied warranty of any kind.

These notices must be retained in any copies of any part of this
documentation and/or software.

*/

/* interface header */

/* system implementation headers */
#include <stdio.h>


// Constants for MD5Transform routine.
#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

///////////////////////////////////////////////

// F, G, H and I are basic MD5 functions.
inline MD5::uint4 MD5::F(uint4 x, uint4 y, uint4 z) {
  return x&y | ~x&z;
}

inline MD5::uint4 MD5::G(uint4 x, uint4 y, uint4 z) {
  return x&z | y&~z;
}

inline MD5::uint4 MD5::H(uint4 x, uint4 y, uint4 z) {
  return x^y^z;
}

inline MD5::uint4 MD5::I(uint4 x, uint4 y, uint4 z) {
  return y ^ (x | ~z);
}

// rotate_left rotates x left n bits.
inline MD5::uint4 MD5::rotate_left(uint4 x, int n) {
  return (x << n) | (x >> (32-n));
}

// FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
// Rotation is separate from addition to prevent recomputation.
inline void MD5::FF(uint4 &a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac) {
  a = rotate_left(a+ F(b,c,d) + x + ac, s) + b;
}

inline void MD5::GG(uint4 &a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac) {
  a = rotate_left(a + G(b,c,d) + x + ac, s) + b;
}

inline void MD5::HH(uint4 &a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac) {
  a = rotate_left(a + H(b,c,d) + x + ac, s) + b;
}

inline void MD5::II(uint4 &a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac) {
  a = rotate_left(a + I(b,c,d) + x + ac, s) + b;
}

//////////////////////////////////////////////

// default ctor, just initailize
MD5::MD5()
{
  init();
}

//////////////////////////////////////////////

// nifty shortcut ctor, compute MD5 for string and finalize it right away
MD5::MD5(const std::string &text)
{
  init();
  update(text.c_str(), text.length());
  finalize();
}

//////////////////////////////

void MD5::init()
{
  finalized=false;

  count[0] = 0;
  count[1] = 0;

  // load magic initialization constants.
  state[0] = 0x67452301;
  state[1] = 0xefcdab89;
  state[2] = 0x98badcfe;
  state[3] = 0x10325476;
}

//////////////////////////////

// decodes input (unsigned char) into output (uint4). Assumes len is a multiple of 4.
void MD5::decode(uint4 output[], const uint1 input[], size_type len)
{
  for (unsigned int i = 0, j = 0; j < len; i++, j += 4)
    output[i] = ((uint4)input[j]) | (((uint4)input[j+1]) << 8) |
      (((uint4)input[j+2]) << 16) | (((uint4)input[j+3]) << 24);
}

//////////////////////////////

// encodes input (uint4) into output (unsigned char). Assumes len is
// a multiple of 4.
void MD5::encode(uint1 output[], const uint4 input[], size_type len)
{
  for (size_type i = 0, j = 0; j < len; i++, j += 4) {
    output[j] = input[i] & 0xff;
    output[j+1] = (input[i] >> 8) & 0xff;
    output[j+2] = (input[i] >> 16) & 0xff;
    output[j+3] = (input[i] >> 24) & 0xff;
  }
}

//////////////////////////////

// apply MD5 algo on a block
void MD5::transform(const uint1 block[blocksize])
{
  uint4 a = state[0], b = state[1], c = state[2], d = state[3], x[16];
  decode (x, block, blocksize);

  /* Round 1 */
  FF (a, b, c, d, x[ 0], S11, 0xd76aa478); /* 1 */
  FF (d, a, b, c, x[ 1], S12, 0xe8c7b756); /* 2 */
  FF (c, d, a, b, x[ 2], S13, 0x242070db); /* 3 */
  FF (b, c, d, a, x[ 3], S14, 0xc1bdceee); /* 4 */
  FF (a, b, c, d, x[ 4], S11, 0xf57c0faf); /* 5 */
  FF (d, a, b, c, x[ 5], S12, 0x4787c62a); /* 6 */
  FF (c, d, a, b, x[ 6], S13, 0xa8304613); /* 7 */
  FF (b, c, d, a, x[ 7], S14, 0xfd469501); /* 8 */
  FF (a, b, c, d, x[ 8], S11, 0x698098d8); /* 9 */
  FF (d, a, b, c, x[ 9], S12, 0x8b44f7af); /* 10 */
  FF (c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
  FF (b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
  FF (a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
  FF (d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
  FF (c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
  FF (b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

  /* Round 2 */
  GG (a, b, c, d, x[ 1], S21, 0xf61e2562); /* 17 */
  GG (d, a, b, c, x[ 6], S22, 0xc040b340); /* 18 */
  GG (c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
  GG (b, c, d, a, x[ 0], S24, 0xe9b6c7aa); /* 20 */
  GG (a, b, c, d, x[ 5], S21, 0xd62f105d); /* 21 */
  GG (d, a, b, c, x[10], S22,  0x2441453); /* 22 */
  GG (c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
  GG (b, c, d, a, x[ 4], S24, 0xe7d3fbc8); /* 24 */
  GG (a, b, c, d, x[ 9], S21, 0x21e1cde6); /* 25 */
  GG (d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
  GG (c, d, a, b, x[ 3], S23, 0xf4d50d87); /* 27 */
  GG (b, c, d, a, x[ 8], S24, 0x455a14ed); /* 28 */
  GG (a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
  GG (d, a, b, c, x[ 2], S22, 0xfcefa3f8); /* 30 */
  GG (c, d, a, b, x[ 7], S23, 0x676f02d9); /* 31 */
  GG (b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

  /* Round 3 */
  HH (a, b, c, d, x[ 5], S31, 0xfffa3942); /* 33 */
  HH (d, a, b, c, x[ 8], S32, 0x8771f681); /* 34 */
  HH (c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
  HH (b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
  HH (a, b, c, d, x[ 1], S31, 0xa4beea44); /* 37 */
  HH (d, a, b, c, x[ 4], S32, 0x4bdecfa9); /* 38 */
  HH (c, d, a, b, x[ 7], S33, 0xf6bb4b60); /* 39 */
  HH (b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
  HH (a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
  HH (d, a, b, c, x[ 0], S32, 0xeaa127fa); /* 42 */
  HH (c, d, a, b, x[ 3], S33, 0xd4ef3085); /* 43 */
  HH (b, c, d, a, x[ 6], S34,  0x4881d05); /* 44 */
  HH (a, b, c, d, x[ 9], S31, 0xd9d4d039); /* 45 */
  HH (d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
  HH (c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
  HH (b, c, d, a, x[ 2], S34, 0xc4ac5665); /* 48 */

  /* Round 4 */
  II (a, b, c, d, x[ 0], S41, 0xf4292244); /* 49 */
  II (d, a, b, c, x[ 7], S42, 0x432aff97); /* 50 */
  II (c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
  II (b, c, d, a, x[ 5], S44, 0xfc93a039); /* 52 */
  II (a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
  II (d, a, b, c, x[ 3], S42, 0x8f0ccc92); /* 54 */
  II (c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
  II (b, c, d, a, x[ 1], S44, 0x85845dd1); /* 56 */
  II (a, b, c, d, x[ 8], S41, 0x6fa87e4f); /* 57 */
  II (d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
  II (c, d, a, b, x[ 6], S43, 0xa3014314); /* 59 */
  II (b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
  II (a, b, c, d, x[ 4], S41, 0xf7537e82); /* 61 */
  II (d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
  II (c, d, a, b, x[ 2], S43, 0x2ad7d2bb); /* 63 */
  II (b, c, d, a, x[ 9], S44, 0xeb86d391); /* 64 */

  state[0] += a;
  state[1] += b;
  state[2] += c;
  state[3] += d;

  // Zeroize sensitive information.
  memset(x, 0, sizeof x);
}

//////////////////////////////

// MD5 block update operation. Continues an MD5 message-digest
// operation, processing another message block
void MD5::update(const unsigned char input[], size_type length)
{
  // compute number of bytes mod 64
  size_type index = count[0] / 8 % blocksize;

  // Update number of bits
  if ((count[0] += (length << 3)) < (length << 3))
    count[1]++;
  count[1] += (length >> 29);

  // number of bytes we need to fill in buffer
  size_type firstpart = 64 - index;

  size_type i;

  // transform as many times as possible.
  if (length >= firstpart)
  {
    // fill buffer first, transform
    memcpy(&buffer[index], input, firstpart);
    transform(buffer);

    // transform chunks of blocksize (64 bytes)
    for (i = firstpart; i + blocksize <= length; i += blocksize)
      transform(&input[i]);

    index = 0;
  }
  else
    i = 0;

  // buffer remaining input
  memcpy(&buffer[index], &input[i], length-i);
}

//////////////////////////////

// for convenience provide a verson with signed char
void MD5::update(const char input[], size_type length)
{
  update((const unsigned char*)input, length);
}

//////////////////////////////

// MD5 finalization. Ends an MD5 message-digest operation, writing the
// the message digest and zeroizing the context.
MD5& MD5::finalize()
{
  static unsigned char padding[64] = {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  };

  if (!finalized) {
    // Save number of bits
    unsigned char bits[8];
    encode(bits, count, 8);

    // pad out to 56 mod 64.
    size_type index = count[0] / 8 % 64;
    size_type padLen = (index < 56) ? (56 - index) : (120 - index);
    update(padding, padLen);

    // Append length (before padding)
    update(bits, 8);

    // Store state in digest
    encode(digest, state, 16);

    // Zeroize sensitive information.
    memset(buffer, 0, sizeof buffer);
    memset(count, 0, sizeof count);

    finalized=true;
  }

  return *this;
}

//////////////////////////////

// return hex representation of digest as string
std::string MD5::hexdigest() const
{
  if (!finalized)
    return "";

  char buf[33];
  for (int i=0; i<16; i++)
    sprintf(buf+i*2, "%02x", digest[i]);
  buf[32]=0;

  return std::string(buf);
}

//////////////////////////////

std::ostream& operator<<(std::ostream& out, MD5 md5)
{
  return out << md5.hexdigest();
}

//////////////////////////////

std::string md5(const std::string str)
{
    MD5 md5 = MD5(str);

    return md5.hexdigest();
}

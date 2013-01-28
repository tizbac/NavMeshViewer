#ifndef ENGINE_H
#define ENGINE_H
#include <SDL/SDL.h>
#define GL3_PROTOTYPES 1
#include <GL/glew.h>
#include <GL3/gl3.h>
#include <GL/glu.h>

#include <cmath>
#include <sys/time.h>
GLboolean CheckExtension( char *extName );

/* MD5
 converted to C++ class by Frank Thilo (thilo@unix-ag.org)
 for bzflag (http://www.bzflag.org)

   based on:

   md5.h and md5.c
   reference implementation of RFC 1321

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

#ifndef BZF_MD5_H
#define BZF_MD5_H

#include <string>
#include <iostream>


// a small class for calculating MD5 hashes of strings or byte arrays
// it is not meant to be fast or secure
//
// usage: 1) feed it blocks of uchars with update()
//      2) finalize()
//      3) get hexdigest() string
//      or
//      MD5(std::string).hexdigest()
//
// assumes that char is 8 bit and int is 32 bit
class MD5
{
public:
  typedef unsigned char uint1; //  8bit
  typedef unsigned int uint4;  // 32bit
  typedef unsigned int size_type; // must be 32bit

  MD5();
  MD5(const std::string& text);
  void update(const unsigned char *buf, size_type length);
  void update(const char *buf, size_type length);
  MD5& finalize();
  std::string hexdigest() const;
  friend std::ostream& operator<<(std::ostream&, MD5 md5);
  uint1 digest[16]; // the result
private:
  void init();
  
  enum {blocksize = 64}; // VC6 won't eat a const static int here

  void transform(const uint1 block[blocksize]);
  static void decode(uint4 output[], const uint1 input[], size_type len);
  static void encode(uint1 output[], const uint4 input[], size_type len);

  bool finalized;
  uint1 buffer[blocksize]; // bytes that didn't fit in last 64 byte chunk
  uint4 count[2];   // 64bit counter for number of bits (lo, hi)
  uint4 state[4];   // digest so far
  

  // low level logic operations
  static inline uint4 F(uint4 x, uint4 y, uint4 z);
  static inline uint4 G(uint4 x, uint4 y, uint4 z);
  static inline uint4 H(uint4 x, uint4 y, uint4 z);
  static inline uint4 I(uint4 x, uint4 y, uint4 z);
  static inline uint4 rotate_left(uint4 x, int n);
  static inline void FF(uint4 &a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac);
  static inline void GG(uint4 &a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac);
  static inline void HH(uint4 &a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac);
  static inline void II(uint4 &a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac);
};

std::string md5(const std::string str);

#endif



double getcurrenttime();
class v3{
    public:
        float x;
        float z;
        float y;
        v3()
        {
            this->x = 0;
            this->y = 0;
            this->z = 0;
        }
        v3(float x,float y,float z)
        {
            this->x = x;
            this->y = y;
            this->z = z;

        }
	
	void normalize(void)
	{
	  float m =  this->mag();
	  this->x = this->x / m;
	  this->y = this->y / m;
	  this->z = this->z / m;
	  
	  
	  
	  
	}
        v3 crossproduct(v3 o2)
        {
            v3 res;
            res.x = y*o2.z-z*o2.y;
            res.y = z*o2.x-x*o2.z;
            res.z = x*o2.y-y*o2.x;
            return (res);

        }
        v3 operator+(v3 o2)
        {
            v3 res;
            res.x = x + o2.x;
            res.y = y + o2.y;
            res.z = z + o2.z;
            return (res);
        }
        v3 operator-(v3 o2)
        {
            v3 res;
            res.x = x - o2.x;
            res.y = y - o2.y;
            res.z = z - o2.z;
            return (res);
        }
        v3 operator*(float m)
        {
            v3 res;
            res.x = x*m;
            res.y = y*m;
            res.z = z*m;
            return (res);
        }
        v3 operator*(v3 o2)
        {
            v3 res;
            res.x = x*o2.x;
            res.y = y*o2.y;
            res.z = z*o2.z;
            return (res);
        }
        inline uint64_t hash() const
        {
		  uint64_t h = 0;
		  /*MD5 md5;
		  md5.update((char*)&x,4);
		  md5.update((char*)&y,4);
		  md5.update((char*)&z,4);
		  md5.finalize();*/
		  //memcpy(&h,md5.digest,8);
		  int64_t XX,YY,ZZ;
                  XX = x*100+893475;
                  YY = y*100+2385732;
                  ZZ = z*100+23895;
		  h = 1;
		  h *= 99194853094755497L;
		  h *= XX;
		  h *= 87178291199L;
		  h *= YY;
		  h *= 16785407L;
		  h *= ZZ;
		  return h;
		}
        bool operator<(v3 o2)const 
		{
		  return hash() < o2.hash();
		}
		bool operator>(v3 o2) const
		{
		  return ( hash() > o2.hash());
		}
		bool operator==(v3 o2) const
		{
		  return hash() == o2.hash();
		}
        float mag() const
        {
            return sqrt(x*x+y*y+z*z);
        }
};

class engine{
    public:
	int height;
	int width;
	int bpp;
	float fps;
	float camx;
	float camy;
	float camz;
    float camrx;
	float camry;
	bool mouseleft;
	bool mouseright;
	bool mousecenter;
	GLdouble proj[16];
	GLint view[3];
	float camrz;
	int mousex;
	int mousey;
	int mouserelx;
	int mouserely;
        float fov;
	bool up;
	bool down;
	bool left;
	bool right;
	float frustum[6][4];
	SDL_Event ev;
	Uint32 cleartime;
	Uint32 rendertime;
	bool fullscreen;
	Uint8 *keys;
	public:
	SDL_Surface* screen;
	bool getkey(char key);
	void OrthoMode();
	void NormalMode();
	void PushODETranform(const float pos[3],const float R[12]);
	float getfps(void);
	inline void ExtractFrustum();
	bool Init3d(int width = 800,int height = 600,int bpp = 24,bool fullscreen = false);
	void clearscreen(float r = 0.0,float g = 0.0,float b = 0.0,float a = 1.0,bool tks=true);
	void PushTransform(float x,float y,float z,float rx,float ry,float rz);
	bool pointinview(float x, float y , float z);
	void Transform(float x,float y,float z,float rx,float ry,float rz);
	void SetCameraPos(float x,float y,float z);
	void SetCameraRotation(float x,float y,float z);
	void SetCameraLookAt(float x,float y,float z);
	void Make2DPositionInto3DPosition (float x, float y, double *ox, double *oy, double *oz)
    {
        GLint viewport[4];
        GLdouble modelview[16],projection[16];
        GLfloat wx=x,wy,wz;

        glGetIntegerv(GL_VIEWPORT,viewport);
        y=viewport[3]-y;
        wy=y;
        glGetDoublev(GL_MODELVIEW_MATRIX,modelview);
        glGetDoublev(GL_PROJECTION_MATRIX,projection);
        glReadPixels(x,y,1,1,GL_DEPTH_COMPONENT,GL_FLOAT,&wz);
        gluUnProject(wx,wy,wz,modelview,projection,viewport,ox,oy,oz);
    }
	int GetMouseX();
	int GetMouseY();
	void PopTransform();
	void display(void);
	bool Terminate(void);
	bool ToggleFullScreen(void);
};

#endif
#include "light.h"
void light::setambcolor(float r,float g,float b)
{
	GLfloat color[4];
	color [0] = r;
	color [1] = g;
	color [2] = b;
	color [3] = 1.0;
	glLightfv(this->id,GL_AMBIENT,color);


}
void light::setdiffcolor(float r,float g,float b)
{
	GLfloat color[4];
	color [0] = r;
	color [1] = g;
	color [2] = b;
	color [3] = 1.0;
	glLightfv(this->id,GL_DIFFUSE,color);


}
void light::setspeccolor(float r,float g,float b)
{
	GLfloat color[4];
	color [0] = r;
	color [1] = g;
	color [2] = b;
	color [3] = 1.0;
	glLightfv(this->id,GL_SPECULAR,color);


}

void light::lightinit(GLenum lgt,float x, float y,float z,float r,float g,float b,float a)
{
	GLfloat pos[3];
	GLfloat color[4];


	glEnable(lgt);
	pos[0] = x;
	pos[1] = y;
	pos[2] = z;
	color[0] = r;
	color[1] = g;
	color[2] = b;
	color[3] = a;
	glLightfv(lgt,GL_POSITION,pos);
	glLightfv(lgt,GL_DIFFUSE,color);
	this->id = lgt;

}

void light::setpos(float x = 0,float y = 20,float z = 0)
{
	GLfloat pos[4];
	pos [0] = x;
	pos [1] = y;
	pos [2] = z;
	pos [3] = 1.0;
	glLightfv(this->id,GL_POSITION,pos);


}

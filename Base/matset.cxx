#include "matset.h"

void matset::setspecular(float r,float g,float b,float a)
{
	GLfloat color[4];
	color[0] = r;
	color[1] = g;
	color[2] = b;
	color[3] = a;
	glMaterialfv(GL_FRONT,GL_SPECULAR,color);
}
void matset::setdiffuse(float r,float g,float b,float a)
{
	GLfloat color[4];
	color[0] = r;
	color[1] = g;
	color[2] = b;
	color[3] = a;
	glMaterialfv(GL_FRONT,GL_DIFFUSE,color);
}
void matset::setambient(float r,float g,float b,float a)
{
	GLfloat color[4];
	color[0] = r;
	color[1] = g;
	color[2] = b;
	color[3] = a;
	glMaterialfv(GL_FRONT,GL_AMBIENT,color);
}


#include "texture.h"


bool texture::LoadFromFile(char * filename)
{
	printf("Loading Texture %s...",filename);
	this->image = SDL_LoadBMP(filename);
	if (this->image == NULL)
	{
		printf("\033[21;35mFAILED!\n\033[21;39m");
		return false;
	}
	printf("\033[21;33mOK\n\033[21;39m");
	glPixelStorei(GL_UNPACK_ALIGNMENT,4);
	glGenTextures(1,&this->id);
	glBindTexture(GL_TEXTURE_2D,this->id);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	gluBuild2DMipmaps(GL_TEXTURE_2D,3,this->image->w,this->image->h,GL_BGR_EXT,GL_UNSIGNED_BYTE,this->image->pixels);
	glMatrixMode(GL_TEXTURE);
	glRotatef(180.0f,0.0f,0.0f,1.0f);
	glScalef(-1.0f,1.0f,1.0f);
 	glMatrixMode(GL_MODELVIEW);
	//sleep(1);
	printf("Texture loaded on Gfx Card!\n");
	return true;

}
bool texture::Update(void)
{
	if (this->image == NULL)
		return false;
	glPixelStorei(GL_UNPACK_ALIGNMENT,4);
	//glGenTextures(1,&tmpt.id);
	glBindTexture(GL_TEXTURE_2D,this->id);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	gluBuild2DMipmaps(GL_TEXTURE_2D,3,this->image->w,this->image->h,GL_BGR_EXT,GL_UNSIGNED_BYTE,this->image->pixels);
	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glLoadIdentity();
    glRotatef(180.0f,0.0f,0.0f,1.0f);
    glScalef(-1.0f,1.0f,1.0f);
	glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
	return true;
}
bool texture::DestroyTexture(void)
{
	SDL_FreeSurface(this->image);
	glDeleteTextures(1,&this->id);
	return true;

}
bool texture::usetexture(void)
{
	if (this->image == NULL)
		return false;
	glBindTexture(GL_TEXTURE_2D,this->id);
	return true;
}

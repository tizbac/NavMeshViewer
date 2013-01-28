#include "fbo.h"
#include <stdio.h>
#include <GL/glew.h>
bool FBO_checkstatus(char * stage)
{
  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER_EXT);
	if(status != GL_FRAMEBUFFER_COMPLETE_EXT)
	{
	  printf("Error, framebuffer object failed (%s): ",stage);
	  switch (status)
	  {
	    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
	      printf("FRAMEBUFFER_INCOMPLETE_ATTACHMENT\n");
	      break;
	    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
	      printf("FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT\n");
	      break;
	    case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
	      printf("FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT\n");
	      break;
	    case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
	      printf("FRAMEBUFFER_INCOMPLETE_FORMATS_EXT\n");
	      break;
	    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
	      printf("FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT\n");
	      break;
	    case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
	      printf("FRAMEBUFFER_UNSUPPORTED\n");
	      break;
	    default:
	      printf("Unknown error\n");
	      break;
	  }
	  return false;
	}
	return true;
}
void FBO::Create(int  width,int height,int type,int filter)
{
	

	
glGenTextures(1,&this->texture);
	glBindTexture(GL_TEXTURE_2D,this->texture);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,  width, height, 0, GL_RGBA, type, NULL);
	//glGenerateMipmapEXT(GL_TEXTURE_2D);
	glGenFramebuffers(1, &this->fbo);
	glBindFramebuffer(GL_FRAMEBUFFER_EXT, this->fbo);
	glGenRenderbuffers(1, &this->depthbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER_EXT, this->depthbuffer);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, this->depthbuffer);

	glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, this->texture, 0);
	
	
	glRenderbufferStorage(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, width,height);
	
	
	

	
	
	

	

	glGenTextures(1,&this->depthtexture);
	
	
	glBindTexture(GL_TEXTURE_2D,this->depthtexture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, this->depthtexture, 0);
	if (not FBO_checkstatus("3"))
	  return;
	
	

	this->w = width;
	this->h = height;
	FBO_checkstatus("4");
	 // return;
	


	
	
	
}
void FBO::BindFBO(void)
{

	glBindFramebuffer(GL_FRAMEBUFFER_EXT, this->fbo);
	glBindRenderbuffer(GL_RENDERBUFFER_EXT, this->depthbuffer);
	FBO_checkstatus("frame");
	
	glPushAttrib(GL_VIEWPORT_BIT);
	
	glViewport(0,0,this->w, this->h);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0, this->w, 0.0, this->h, -1.0, 1.0); 
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();



	
}
void FBO::UnBindFBO(void)
{

	glPopAttrib();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);

}
void FBO::BindTexture(void)
{
  glBindTexture(GL_TEXTURE_2D,this->texture);
}
void FBO::BindDepthTexture(void)
{
  glBindTexture(GL_TEXTURE_2D,this->depthtexture);
}


#include <GL/glew.h>
//#include <stdio.h>
//#include <unistd.h>
class FBO{
	public:
	int w;
	int h;
	GLuint fbo;
	GLuint texture;
	GLuint depthtexture;
	GLuint depthbuffer;
	void BindFBO(void);
	void UnBindFBO(void);
	void Create(int  width,int height,int type=GL_UNSIGNED_BYTE,int filter=GL_LINEAR);
	void BindTexture(void);
	void BindDepthTexture(void);
	
	
};

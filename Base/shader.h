#include <GL/glew.h>
#include <iostream>
class shader{
	GLuint program;
	GLuint fs;
	GLuint vs;
	GLuint gs;
	public:
	void useshader(void);
	void setuniformfloat(char *uniname,float value);
	void setuniform2f(char *uniname,float v1,float v2);
	void setuniform3f(char *uniname,float v1,float v2,float v3);
	void setuniform4f(char *uniname,float v1,float v2,float v3,float v4);
	void setuniformsampler(char *uniname,int value);
	void setuniformint(char *uniname,int value);
	bool createfromfiles(char * fsfile,char * vsfile,char * gsfile);
};

#include "shader.h"
#include "engine.h"
#include <stdio.h>
#include <stdlib.h>
char *file2string(const char *path)
{
    FILE *fd;
    long len,r;
    char *str;
    if (!(fd = fopen(path,"r")))
    {
        fprintf(stderr,"Cannot open file %s\n",path);
        return NULL;
    }
    fseek(fd,0,SEEK_END);
    len = ftell(fd);
    printf("Shader %ld bytes\n",len);
    if (!(str = (char*)malloc(len*sizeof(char))))
    {
        printf("Cannot allocate memory");
        return NULL;
    }
    fseek(fd,0,SEEK_SET);
    r = fread(str,sizeof(char),len,fd);
    str[r-1] = '\0';
    fclose(fd);
    return str;
}
void PrintLog(GLuint obj)
{
    int inlogl = 0;
    char infoLog[1024];
    if (glIsShader(obj))
        glGetShaderInfoLog(obj,1024,&inlogl,infoLog);
    else
        glGetProgramInfoLog(obj,1024,&inlogl,infoLog);
    if (inlogl > 0)
        printf("%s\n",infoLog);
}

bool shader::createfromfiles(char * fsfile,char * vsfile,char * gfile)
{
    char * fss = file2string(fsfile);
    char * vss = file2string(vsfile);
    bool has_geom = CheckExtension("GL_ARB_geometry_shader4");
    if ( gfile && !has_geom )
    {
        std::cerr << "ERROR: Cannot compile geometry shader on this hardware" << std::endl;
        return false;
    }
    char * gss = NULL;
    if ( gfile )
    {
        gss = file2string(gfile);
        
    }
    
    /*if (GLEW_VERSION_2_0)
    	printf("OpenGL 2.0\n");
    else
    	printf("DANGER: No OpenGL 2.0\n");*/
    this->vs = glCreateShader(GL_VERTEX_SHADER);
    this->fs = glCreateShader(GL_FRAGMENT_SHADER);
    if ( gss )
    {
        gs = glCreateShader(GL_GEOMETRY_SHADER_EXT);
        glShaderSource(gs,1,(const char **)&gss,NULL);
    }
    glShaderSource(this->vs,1,(const char **)&vss,NULL);
    glShaderSource(this->fs,1,(const char **)&fss,NULL);
    
    glCompileShader(this->vs);
    PrintLog(this->vs);
    glCompileShader(this->fs);
    PrintLog(this->fs);
    if ( gss )
    {
        glCompileShader(this->gs);
        PrintLog(this->gs);
    }
    this->program = glCreateProgram();
    glAttachShader(this->program,this->vs);
    glAttachShader(this->program,this->fs);
    
    if ( gss )
    {
        glAttachShader(this->program,this->gs);
        glProgramParameteriEXT(this->program,GL_GEOMETRY_INPUT_TYPE_EXT,GL_TRIANGLES);
        glProgramParameteriEXT(this->program,GL_GEOMETRY_OUTPUT_TYPE_EXT,GL_TRIANGLE_STRIP);
        int temp;
        glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES_EXT,&temp);
        glProgramParameteriEXT(this->program,GL_GEOMETRY_VERTICES_OUT_EXT,temp);
    }
    
    /*glBindAttribLocation(program, 0, "in_Position"); // Bind a constant attribute location for positions of vertices
    glBindAttribLocation(program, 1, "in_Color"); // Bind another constant attribute location, this time for color
    glBindFragDataLocation(program, 0, "f_color");*/
    PrintLog(this->program);
    glLinkProgram(this->program);

    glUseProgram(0);
    free(fss);
    free(vss);
    if ( gss )
        free(gss);
    return true;
}
void shader::useshader(void)
{
    glUseProgram(this->program);
}
void shader::setuniformfloat(char *uniname,float value)
{
    glUseProgram(this->program);
    GLint loc;
    loc = glGetUniformLocation(this->program,uniname);
    glUniform1f(loc,value);
    //glUseProgram(0);
}
void shader::setuniform2f(char* uniname, float v1, float v2)
{
    glUseProgram(this->program);
    GLint loc;
    loc = glGetUniformLocation(this->program,uniname);
    glUniform2f(loc,v1,v2);
    glUseProgram(0);

}
void shader::setuniform3f(char* uniname, float v1, float v2,float v3)
{
    glUseProgram(this->program);
    GLint loc;
    loc = glGetUniformLocation(this->program,uniname);
    glUniform3f(loc,v1,v2,v3);
    glUseProgram(0);

}
void shader::setuniform4f(char* uniname, float v1, float v2,float v3,float v4)
{
    glUseProgram(this->program);
    GLint loc;
    loc = glGetUniformLocation(this->program,uniname);
    glUniform4f(loc,v1,v2,v3,v4);
    glUseProgram(0);

}
void shader::setuniformsampler(char *uniname,int value)
{
    glUseProgram(this->program);
    GLint loc;
    loc = glGetUniformLocation(this->program,uniname);
    glUniform1i(loc,value);

}
void shader::setuniformint(char *uniname,int value)
{
    glUseProgram(this->program);
    GLint loc;
    loc = glGetUniformLocation(this->program,uniname);
    glUniform1i(loc,value);



}

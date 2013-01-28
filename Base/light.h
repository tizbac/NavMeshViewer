#include <GL/glew.h>
struct l_color{
	float r;
	float g;
	float b;
	float a;
	
};
struct l_pos{
	float x;
	float y;
	float z;
};
class light{
	private:
	
		l_color diffuse;
		l_color specular;
		l_color ambient;
		GLenum id;
	
	public:
	
		void lightinit(GLenum lgt,float x = 0, float y = 20,float z = 0,float r = 1.0,float g = 1.0,float b = 1.0,float a = 1);
		void setpos(float x, float y,float z);
		void setspeccolor(float r = 1.0,float g = 1.0,float b = 1.0);
		void setdiffcolor(float r = 1.0,float g = 1.0,float b = 1.0);
		void setambcolor(float r = 1.0,float g = 1.0,float b = 1.0);
		bool showlamp;
	
};		

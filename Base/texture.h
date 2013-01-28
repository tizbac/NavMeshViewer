#include <SDL/SDL.h>
#include <GL/glew.h>
#include <stdio.h>
#include <unistd.h>
class texture{
	
	GLuint id;
	public:
	SDL_Surface* image;
	bool LoadFromFile(char * filename);
	bool Update(void);
	bool DestroyTexture(void);
	bool usetexture(void);
};
		

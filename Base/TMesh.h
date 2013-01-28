/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) <year>  <name of author>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef TMESH_H
#define TMESH_H
#include <vector>
#ifndef HEADLESS
#include <engine.h>
#include <boost/thread.hpp>
#include <Thread.h>
#include <GL3/gl3.h>
#else
#include <string>
typedef void engine;

#endif
class TMesh 
{

public:
    TMesh();
    TMesh(const TMesh& other);
    void Render(engine* e,float x, float y, float z,float rx, float ry,float rz);
	void RenderVBO(engine* e,float x, float y, float z,float rx, float ry,float rz);
    void AddMesh(TMesh* m2, bool update=true);
    void PermTranslate(float x, float y, float z);
	void generateTrisTexCoords();
    void generateNormals();
	void removeDoubles();
	void decimate();
	void launchdecimate(float frac);
	void fixmesh();
	void Fstopdecimate();
	void UploadData();
	void AcquireDirectAccess();
	void ReleaseDirectAccess();
#ifndef HEADLESS
	v3 GetVertAt(uint32_t index);
#endif
    virtual ~TMesh();
    virtual TMesh& operator=(const TMesh& other);
    virtual bool operator==(const TMesh& other) const;
    std::vector<float> verts;
    std::vector<int> indexes;
    std::vector<float> texcoords;
    std::vector<float> normals;
    std::string info;
	
  private:
#ifndef HEADLESS
	GLuint vertexbufferobj;
#endif
	bool decimate_canupload;
	bool stopdecimate;
#ifndef HEADLESS
	Thread * decimatethread;
	boost::mutex decimatelock;
	boost::mutex datalock;
#endif
	float decimatefrac;
#ifndef HEADLESS
	GLuint indexbufferobj;
#endif
    float * TriNorm(float x, float y , float z,float x2, float y2 , float z2,float x3, float y3 , float z3);
};

#endif // TMESH_H

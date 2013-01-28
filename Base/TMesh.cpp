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

#include <stdlib.h>
#include "TMesh.h"
#include <math.h>
#define BUFFER_OFFSET(i) ((char *)NULL + (i))
#include <stdint.h>
#include <map>
#include <iostream>

#ifndef HEADLESS
#include <LOD_decimation.h>
#endif
#include <list>
#ifndef HEADLESS
#include <boost/bind.hpp>
#include <GL3/gl3.h>
#endif
#define RELAUNCHDECIMATE if (decimatethread && !decimate_canupload) { Fstopdecimate(); launchdecimate(decimatefrac); }
#include <string>
#ifdef HEADLESS
#undef RELAUNCHDECIMATE
#define RELAUNCHDECIMATE {}
using namespace std;
class v3{
    public:
        float x;
        float z;
        float y;
        v3()
        {
            this->x = 0;
            this->y = 0;
            this->z = 0;
        }
        v3(float x,float y,float z)
        {
            this->x = x;
            this->y = y;
            this->z = z;

        }
        
        void normalize(void)
        {
          float m =  this->mag();
          this->x = this->x / m;
          this->y = this->y / m;
          this->z = this->z / m;
          
          
          
          
        }
        v3 crossproduct(v3 o2)
        {
            v3 res;
            res.x = y*o2.z-z*o2.y;
            res.y = z*o2.x-x*o2.z;
            res.z = x*o2.y-y*o2.x;
            return (res);

        }
        v3 operator+(v3 o2)
        {
            v3 res;
            res.x = x + o2.x;
            res.y = y + o2.y;
            res.z = z + o2.z;
            return (res);
        }
        v3 operator-(v3 o2)
        {
            v3 res;
            res.x = x - o2.x;
            res.y = y - o2.y;
            res.z = z - o2.z;
            return (res);
        }
        v3 operator*(float m)
        {
            v3 res;
            res.x = x*m;
            res.y = y*m;
            res.z = z*m;
            return (res);
        }
        v3 operator*(v3 o2)
        {
            v3 res;
            res.x = x*o2.x;
            res.y = y*o2.y;
            res.z = z*o2.z;
            return (res);
        }
        inline uint64_t hash() const
        {
                  int64_t XX,YY,ZZ;
                  uint64_t h;
                  XX = x*100+893475;
                  YY = y*100+2385732;
                  ZZ = z*100+23895;
                  h = 1;
                  h *= 99194853094755497L;
                  h *= XX;
                  h *= 87178291199L;
                  h *= YY;
                  h *= 16785407L;
                  h *= ZZ;
                  return h;
                }
        bool operator<(v3 o2)const 
                {
                  return hash() < o2.hash();
                }
                bool operator>(v3 o2) const
                {
                  return ( hash() > o2.hash());
                }
                bool operator==(v3 o2) const
                {
                  return hash() == o2.hash();
                }
        float mag() const
        {
            return sqrt(x*x+y*y+z*z);
        }
};

#endif
float media(std::vector<float> valori)
{
  float tot = 0;
  for ( int i = 0; i < valori.size(); i++)
	tot += valori[i];
  return tot/valori.size();
}
float mediaX2(std::vector<float> valori)
{
  float tot = 0;
  for ( int i = 0; i < valori.size(); i++)
	tot += pow(valori[i],2);
  return tot/valori.size();
}
#ifndef HEADLESS
void TMesh::generateTrisTexCoords()
{
  boost::mutex::scoped_lock lock(datalock);
  texcoords.resize((verts.size()/3)*2);
  for ( int k = 0; k < indexes.size()/3; k++ )
  {
	uint32_t vindex1 = indexes[k*3+0];
	uint32_t vindex2 = indexes[k*3+1];
	uint32_t vindex3 = indexes[k*3+2];
	
	texcoords[vindex1*2+0] = 0;
	texcoords[vindex1*2+1] = 0;
	
	texcoords[vindex2*2+0] = 1.0;
	texcoords[vindex2*2+1] = 0;
	
	texcoords[vindex3*2+0] = 0;
	texcoords[vindex3*2+1] = 1.0;
  }
}
#endif
    #ifndef HEADLESS
void TMesh::Render(engine * e,float x,float y , float z,float rx,float ry,float rz)
{

  boost::mutex::scoped_lock lock(datalock);

  if ( normals.size() != verts.size() )
    normals.resize(verts.size());
  if ( (verts.size()/3)*2 != texcoords.size() )
  {
	printf("\nNo texcoords ( %u )\n",texcoords.size());
	texcoords.resize((verts.size()/3)*2);
	
	for ( int l = 0; l < texcoords.size(); l++ )
	  texcoords[l] = 0.0f;
  }
  glBegin(GL_TRIANGLES);
  for ( int k = 0; k < indexes.size()/3; k++ )
  {
    bool cr = true;
    for ( int i = 0; i < 9; i++ )
    {
      if ( indexes[k*3+i] >= verts.size() )
      {
        cr = false;
        //printf("Invalid vertex index indexes[%d] > verts.size()(%d)\n",k*3+i,verts.size());
        break;
      }
      
    }
    if ( cr )
    {
      
      
      glNormal3f(normals[indexes[k*3]*3],normals[indexes[k*3]*3+1],normals[indexes[k*3]*3+2]);
	  glTexCoord2f(0,0);
      glVertex3f(verts[indexes[k*3]*3]+x,verts[indexes[k*3]*3+1]+y,verts[indexes[k*3]*3+2]+z);
      glNormal3f(normals[indexes[k*3+1]*3],normals[indexes[k*3+1]*3+1],normals[indexes[k*3+1]*3+2]);
	  glTexCoord2f(1,0);
      glVertex3f(verts[indexes[k*3+1]*3]+x,verts[indexes[k*3+1]*3+1]+y,verts[indexes[k*3+1]*3+2]+z);
      glNormal3f(normals[indexes[k*3+2]*3],normals[indexes[k*3+2]*3+1],normals[indexes[k*3+2]*3+2]);
	  glTexCoord2f(1,1);
      glVertex3f(verts[indexes[k*3+2]*3]+x,verts[indexes[k*3+2]*3+1]+y,verts[indexes[k*3+2]*3+2]+z);
    }
  }
  glEnd();

}
#endif
void TMesh::RenderVBO(engine* e, float x, float y, float z, float rx, float ry, float rz)
{
#ifndef HEADLESS
  if ( decimate_canupload )
  {
	generateNormals();
	UploadData();
	decimate_canupload = false;
  }
  glBindBuffer(GL_ARRAY_BUFFER,vertexbufferobj);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,indexbufferobj);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState( GL_TEXTURE_COORD_ARRAY );
  glEnableClientState( GL_NORMAL_ARRAY);
  glVertexPointer(3,GL_FLOAT,32,BUFFER_OFFSET(0));
  glNormalPointer(GL_FLOAT,32,BUFFER_OFFSET(12));
  glClientActiveTexture(GL_TEXTURE0);
  glTexCoordPointer(2,GL_FLOAT,32,BUFFER_OFFSET(24));
  glDrawElements(GL_TRIANGLES,indexes.size(),GL_UNSIGNED_INT,0);
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState( GL_TEXTURE_COORD_ARRAY );
  glDisableClientState( GL_NORMAL_ARRAY);
  glBindBuffer(GL_ARRAY_BUFFER,0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
#endif
}

void TMesh::AddMesh(TMesh *m2,bool update)
{
  
  {
#ifndef HEADLESS
	boost::mutex::scoped_lock lock(datalock);
#endif
    unsigned int vertsoffset = verts.size();
    unsigned int indexesoffset = indexes.size();
    verts.reserve(verts.size()+m2->verts.size());
    for ( int i = 0; i < m2->verts.size(); i++)
        verts.push_back(m2->verts[i]);
    indexes.reserve(indexes.size()+m2->indexes.size());
    for ( int i = 0; i < m2->indexes.size(); i++)
        indexes.push_back(m2->indexes[i]+vertsoffset/3);
  }
#ifndef HEADLESS
if ( update )
{
    generateNormals();
	UploadData();
        
	RELAUNCHDECIMATE
}
#endif
}
void TMesh::PermTranslate(float x, float y, float z)
{
  {
#ifndef HEADLESS
	boost::mutex::scoped_lock lock(datalock);
#endif
    for ( int i = 0; i < verts.size()/3; i++)
    {
        float * vert = &verts[i*3];
        vert[0] += x;
        vert[1] += y;
        vert[2] += z;

    }
  }
#ifndef HEADLESS
	UploadData();
	RELAUNCHDECIMATE
#endif
}
void TMesh::fixmesh()
{
  std::vector<uint32_t> newindexes;
  int broken = 0;
  for ( int i = 0; i < indexes.size(); i++ )
  {
	if ( indexes[i] < verts.size() )
	  newindexes.push_back(indexes[i]);
	else
	{
	  newindexes.push_back(0);
	  broken++;}
	
  }
  //cout << broken << " Broken Indexes" << std::endl;
   for ( int i = 0; i < verts.size(); i++ )
  {
	if ( isinf(verts[i]) || isnan(verts[i]) )
	  verts[i] = 0;
	
  }
  if ( ! (verts.size() % 3 == 0 && indexes.size() % 3 == 0))
	abort();
}

void TMesh::removeDoubles()
{
  {
#ifndef HEADLESS
	boost::mutex::scoped_lock lock(datalock);
#endif
	std::map<v3,uint32_t> vertsmap;
	unsigned int c = 0;
	for ( int i = 0; i < indexes.size(); i++)
	{
	  if (vertsmap.find(v3(verts[indexes[i]*3+0],verts[indexes[i]*3+1],verts[indexes[i]*3+2])) == vertsmap.end())
	  {
		vertsmap.insert(std::pair<v3,uint32_t>(v3(verts[indexes[i]*3+0],verts[indexes[i]*3+1],verts[indexes[i]*3+2]),c));
		c++;
	  }
	}
	//std::cout << "Removing " << verts.size()/3 - vertsmap.size() << " Vertices" << std::endl;
	for ( int i = 0; i < indexes.size(); i++ )
	{
	  std::map<v3,uint32_t>::const_iterator it = vertsmap.find(v3(verts[indexes[i]*3+0],verts[indexes[i]*3+1],verts[indexes[i]*3+2]));
	  if ( it == vertsmap.end() )
	  {
		std::cout << "Cannot find vertex on vertsmap" << std::endl;
		indexes[i] = 0;
		continue;
	  }
	  indexes[i] = (*it).second;
	}
	verts.clear();
	verts.resize(vertsmap.size()*3);
	for ( std::map<v3,uint32_t>::iterator it = vertsmap.begin(); it != vertsmap.end() ; it++ )
	{
	  verts[(*it).second*3+0] = (*it).first.x;
	  verts[(*it).second*3+1] = (*it).first.y;
	  verts[(*it).second*3+2] = (*it).first.z;
	}
  }
  /*std::vector <uint32_t> newindexes;
  std::map<v3,uint32_t> indexmap;
  for ( int i = 0; i < indexes.size()/3; i++ )
  {
	if ( indexmap.find(v3(indexes[i*3+0]<<8,indexes[i*3+1]<<16,indexes[i*3+2]<<32)) != indexmap.end() )
	{
	  //std::cerr << "Duplicate triangle" << std::endl;
	}else{
	  indexmap.insert(std::pair<v3,uint32_t>(v3(indexes[i*3+0]<<8,indexes[i*3+1]<<16,indexes[i*3+2]<<32),i));
	  newindexes.push_back(indexes[i*3+0]);
	  newindexes.push_back(indexes[i*3+1]);
	  newindexes.push_back(indexes[i*3+2]);
	}
	
  }
  indexes = newindexes;*/
#ifndef HEADLESS
  RELAUNCHDECIMATE
#endif
}
#ifndef HEADLESS
void TMesh::Fstopdecimate()
{
  if ( decimatethread != NULL )
  {
	stopdecimate = true;
	decimatethread->Join();
	//delete decimatethread;
  }
  decimatethread = NULL;
  stopdecimate = false;
}

#endif

#ifndef HEADLESS
void *decimate_threadproxy(void * inst)
{
  ((TMesh*)inst)->decimate();
}
void TMesh::launchdecimate(float frac)
{
  Fstopdecimate();
  decimatefrac = frac;
  if ( verts.size() > 9 && indexes.size() > 9 )
	decimatethread = new Thread(decimate_threadproxy,(void*)this);
}
void TMesh::decimate()
{
  boost::mutex::scoped_lock lock(decimatelock);
   float frac = decimatefrac;
  if ( frac > 0.95 )
  {
	printf("frac > 0.95\n");
	return;
  }
  uint32_t tris;
  LOD_Decimation_Info lod;
  {
	boost::mutex::scoped_lock lock2(datalock);
	fixmesh();
	lod.vertex_buffer = (float*)malloc(verts.size()*sizeof(float));
	lod.vertex_normal_buffer = (float*)malloc(normals.size()*sizeof(float));
	lod.triangle_index_buffer = (int*)malloc(indexes.size()*sizeof(unsigned int));
	lod.vertex_num = verts.size()/3;
	lod.face_num = indexes.size()/3;
	memcpy(lod.vertex_buffer,&verts[0],verts.size()*sizeof(float));
	memcpy(lod.vertex_normal_buffer,&normals[0],normals.size()*sizeof(float));
	memcpy(lod.triangle_index_buffer,&indexes[0],indexes.size()*sizeof(unsigned int));
	tris = (indexes.size()/3);
  }
   if ( !LOD_LoadMesh(&lod) )
	 return;
   if ( !LOD_PreprocessMesh(&lod) )
	 return;
  uint32_t collapsed = 0;
  
  while(lod.face_num > tris*frac && stopdecimate == false) {
	collapsed++;
	  if( LOD_CollapseEdge(&lod)==0) break;
	  if ( collapsed % 1000 == 0)
	  {
		
		{
		  boost::mutex::scoped_lock lock(datalock);
		  indexes.resize(lod.face_num*3);
		  verts.resize(lod.vertex_num*3);
		  memcpy(&verts[0],lod.vertex_buffer,lod.vertex_num*3*sizeof(float));
		  memcpy(&indexes[0],lod.triangle_index_buffer,lod.face_num*3*sizeof(int));
		  decimate_canupload = true;
		}
		
		
		
	  }
  }
  {
	
	if (!stopdecimate)
	{
	  boost::mutex::scoped_lock lock(datalock);
	  indexes.resize(lod.face_num*3);
	  verts.resize(lod.vertex_num*3);
	  memcpy(&verts[0],lod.vertex_buffer,lod.vertex_num*3*sizeof(float));
	  memcpy(&indexes[0],lod.triangle_index_buffer,lod.face_num*3*sizeof(int));
	  decimate_canupload = true;
	}
  }
  
  printf("%u edges collapsed\n",collapsed);
  
  free(lod.vertex_buffer);
  free(lod.vertex_normal_buffer);
  free(lod.triangle_index_buffer);
  printf("Decimate thread exiting(stopdecimate=%s)\n",stopdecimate ? "true" :"false");
}
#endif
#ifndef HEADLESS
void TMesh::generateNormals()
{
  {
  boost::mutex::scoped_lock lock(datalock);
	normals.clear();
	normals.resize(verts.size());
	for ( int k = 0; k < indexes.size()/3; k++ )
	{
	  bool cr = true;
	  for ( int i = 0; i < 9; i++ )
	  {
		if ( indexes[k*3+i] >= verts.size() )
		{
		  cr = false;
		  break;
		}
		
	  }
	  if ( cr )
	  {
		float * n = TriNorm(verts[indexes[k*3]*3],verts[indexes[k*3]*3+1],verts[indexes[k*3]*3+2],verts[indexes[k*3+1]*3],verts[indexes[k*3+1]*3+1],verts[indexes[k*3+1]*3+2],verts[indexes[k*3+2]*3],verts[indexes[k*3+2]*3+1],verts[indexes[k*3+2]*3+2]);
		normals[indexes[k*3]*3] = n[0];
		normals[indexes[k*3]*3+1] = n[1];
		normals[indexes[k*3]*3+2] = n[2];
		normals[indexes[k*3+1]*3] = n[0];
		normals[indexes[k*3+1]*3+1] = n[1];
		normals[indexes[k*3+1]*3+2] = n[2];
		normals[indexes[k*3+2]*3] = n[0];
		normals[indexes[k*3+2]*3+1] = n[1];
		normals[indexes[k*3+2]*3+2] = n[2];
	  }
	}
  }
  UploadData();
  RELAUNCHDECIMATE
}
#endif

float * TMesh::TriNorm(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3)
{
  float nx1 = (y3-y1)*(z2-z1)-(y2-y1)*(z3-z1);
  float ny1 = (z3-z1)*(x2-x1)-(z2-z1)*(x3-x1);
  float nz1 = (x3-x1)*(y2-y1)-(x2-x1)*(y3-y1);
  float fac1=sqrt((nx1*nx1)+(ny1*ny1)+(nz1*nz1));
  nx1 = nx1/fac1;
  ny1 = ny1/fac1;
  nz1 = nz1/fac1;
  static float norm[3];
  norm[0] = nx1;
  norm[1] = ny1;
  norm[2] = nz1;
  return norm;
}

TMesh::TMesh()
{
#ifndef HEADLESS
  decimatethread = NULL;
  glGenBuffers(1,&vertexbufferobj);
  glGenBuffers(1,&indexbufferobj);
#endif
}

TMesh::~TMesh()
{
    #ifndef HEADLESS
  glDeleteBuffers(1,&vertexbufferobj);
  glDeleteBuffers(1,&indexbufferobj);
  Fstopdecimate();
  #endif
}
void TMesh::AcquireDirectAccess()
{
#ifndef HEADLESS
  datalock.lock();
#endif
}
void TMesh::ReleaseDirectAccess()
{
    #ifndef HEADLESS
  datalock.unlock();
  #endif
}
#ifndef HEADLESS
void TMesh::UploadData()
{
  boost::mutex::scoped_lock lock(datalock);
  glBindBuffer(GL_ARRAY_BUFFER,vertexbufferobj);
  char * buffer = (char*)malloc((sizeof(float)*8)*(verts.size()/3));
  float * fPtr = (float*)buffer;
  for ( int i = 0; i < verts.size()/3; i++ )
  {
	fPtr[i*8+0] = verts[i*3+0];
	fPtr[i*8+1] = verts[i*3+1];
	fPtr[i*8+2] = verts[i*3+2];
	
	fPtr[i*8+3] = normals[i*3+0];
	fPtr[i*8+4] = normals[i*3+1];
	fPtr[i*8+5] = normals[i*3+2];
	
	switch ( (i) % 3 )
	{
	  case 0:
		fPtr[i*8+6] = 0;
		fPtr[i*8+7] = 0;
		break;
	  case 1:
		fPtr[i*8+6] = 0;
		fPtr[i*8+7] = 1;
		break;
	  case 2:
		fPtr[i*8+6] = 1;
		fPtr[i*8+7] = 0;
		break;
	}
  }
  /*for ( int k = 0; k < indexes.size()/3; k++ )
  {
	uint32_t v1index = indexes[k*3+0];
	uint32_t v2index = indexes[k*3+1];
	uint32_t v3index = indexes[k*3+2];
	fPtr[v1index*8+6] = 0;
	fPtr[v1index*8+7] = 0;
	fPtr[v2index*8+6] = 1;
	fPtr[v2index*8+7] = 0;
	fPtr[v3index*8+6] = 1;
	fPtr[v3index*8+7] = 1;
  }*/
	  
  glBufferData(GL_ARRAY_BUFFER,(sizeof(float)*8)*(verts.size()/3),buffer,GL_STATIC_DRAW);
  free(buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,indexbufferobj);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(uint32_t)*indexes.size(),&indexes[0],GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER,0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
}
#endif
TMesh& TMesh::operator=(const TMesh& other)
{
    return *this;
}

bool TMesh::operator==(const TMesh& other) const
{
///TODO: return ...;
}


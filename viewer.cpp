/*
	This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>
    Copyright (C) Tiziano Bacocco
	
*/

#include "MapTree.h"
#include "ModelInstance.h"
#include <fbo.h>
#include <engine.h>
#include <matset.h>
#include <shader.h>
#include <light.h>
#include <FTGL/ftgl.h>
#include "Recast.h"
#include "DetourNavMeshBuilder.h"
#include "DetourNavMesh.h"
#include "DetourCommon.h"
#include <TMesh.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <string>
#include <ctype.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

#ifndef DISABLE_PROCESS_ATTACH
#ifdef __WIN32__
#include <windows.h>
#include <tlhelp32.h>
#else
#include <sys/ptrace.h>
#endif
#endif
#include "DetourNavMesh.h"
#include "DetourCommon.h"
#include "DetourNavMeshBuilder.h"
#include "DetourAlloc.h"
#include "DetourAssert.h"
#ifndef __WIN32__
#include <sys/wait.h>
#endif
#include <sstream>
#define READ_BUF_SIZE 256
#define xrealloc realloc
/*#define ILUT_USE_OPENGL
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>*/
#include <list>
#include "MMapCommon.h"
#include "MapBuilder.h"
#include "VMapManager2.h"
std::list<unsigned char> consolebuffer;
enum Modes{
    MODE_VIEW_WORLD,
    MODE_VIEW_POLYDETAIL,
    
};
Modes mode = MODE_VIEW_WORLD;
TMesh * polyDetail = NULL;
using namespace std;
/*class Image {
public:
    ILuint image;
    int w;
    int h;
    int d;
    unsigned char * datapointer;
    Image()
    {
        ilGenImages(1,&image);



    }
    Image(char * filename)
    {
        ilGenImages(1,&image);
        ilBindImage(image);
        ilLoadImage(filename);
        ConvertToRGB();
        w = ilGetInteger(IL_IMAGE_WIDTH);
        h = ilGetInteger(IL_IMAGE_HEIGHT);
        d = ilGetInteger(IL_IMAGE_DEPTH);
        datapointer = ilGetData();
    }
    void Save(char * filename)
    {
        ilBindImage(image);
        ilSaveImage(filename);
    }
    void AllocateLUM(int x, int y,char * data=NULL)
    {
        ilBindImage(image);
        ilTexImage(x,y,1,1,IL_LUMINANCE,IL_UNSIGNED_BYTE,data);
        datapointer = ilGetData();
        w = ilGetInteger(IL_IMAGE_WIDTH);
        h = ilGetInteger(IL_IMAGE_HEIGHT);
        d = ilGetInteger(IL_IMAGE_DEPTH);
    }
    void AllocateRGB(int x, int y,char * data=NULL)
    {
        ilBindImage(image);
        ilTexImage(x,y,1,3,IL_RGB,IL_UNSIGNED_BYTE,data);
        datapointer = ilGetData();
        w = ilGetInteger(IL_IMAGE_WIDTH);
        h = ilGetInteger(IL_IMAGE_HEIGHT);
        d = ilGetInteger(IL_IMAGE_DEPTH);
    }
    void Rescale(int x , int y)
    {
        ilBindImage(image);
        iluScale(x,y,1);
        datapointer = ilGetData();
        w = ilGetInteger(IL_IMAGE_WIDTH);
        h = ilGetInteger(IL_IMAGE_HEIGHT);
        d = ilGetInteger(IL_IMAGE_DEPTH);
    }
    void GetPixelRGB(int x_,int y_,unsigned char * pix)
    {

        int x = x_ % w;
        int y = y_ % h;

        if ( datapointer )
        {
            pix[0] = datapointer[w*y*3+x*3];
            pix[1] = datapointer[w*y*3+x*3+1];
            pix[2] = datapointer[w*y*3+x*3+2];
        } else {
            printf("GetPixelRGB(%i,%i): datapointer is NULL\n",x_,y_);
        }
        //printf("%i %i %i\n",(int)pix[0],(int)pix[1],int(pix[2]));
    }
    void SetPixelRGB(int x_, int y_,char r, char g, char b)
    {
        int x = x_ % w;
        int y = y_ % h;
        datapointer[w*y*3+x*3] = r;
        datapointer[w*y*3+x*3+1] = g;
        datapointer[w*y*3+x*3+2] = b;


    }
    void SetPixelLUM(int x_, int y_, char val)
    {
        int x = x_ % w;
        int y = y_ % h;
        datapointer[y*w+x] = val;


    }
    void GetPixelLUM(int x_, int y_,unsigned char * p)
    {
        //printf("GetPixelLUM(%i,%i)\n",x_,y_);
        int x = x_ % w;
        int y = y_ % h;
        p[0] = datapointer[y*w+x];

    }
    void ConvertToLUM()
    {
        ilBindImage(image);
        ilConvertImage(IL_LUMINANCE,IL_UNSIGNED_BYTE);
        datapointer = ilGetData();
        iluFlipImage();
    }
    void ConvertToRGB()
    {
        ilBindImage(image);
        ilConvertImage(IL_RGB,IL_UNSIGNED_BYTE);
        datapointer = ilGetData();
    }
    GLuint GenerateGLTexture()
    {
        ilBindImage(image);
        return ilutGLBindMipmaps();

    }
    ~Image()
    {
        ilDeleteImages(1,&image);
    }
};*/

void perror_msg_and_die(const char *s, ...)
{
    va_list p;

    va_start(p, s);
    fprintf(stderr,s,p);
    va_end(p);
    exit(EXIT_FAILURE);
}
#ifndef DISABLE_PROCESS_ATTACH
#ifdef __WIN32__
unsigned int W32GetWowPid()
{
 //   OutputDebugString("W32GetWowPidEnter\n");
    unsigned int p = 0;
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    if (Process32First(snapshot, &entry) == TRUE)
    {
        while (Process32Next(snapshot, &entry) == TRUE)
        {
            if (stricmp(entry.szExeFile, "Wow.exe") == 0)
            {  
                p = entry.th32ProcessID;
		break;
            }
        }
    }
    
    CloseHandle(snapshot);
   // OutputDebugString("W32GetWowPidExit\n");
    return p;
}

#endif
pid_t* find_pid_by_name( char* pidName)
{
    DIR *dir;
    struct dirent *next;
    pid_t* pidList=NULL;
    int i=0;

    dir = opendir("/proc");
    if (!dir)
        perror_msg_and_die("Cannot open /proc");

    while ((next = readdir(dir)) != NULL) {
        FILE *status;
        char filename[READ_BUF_SIZE];
        char buffer[READ_BUF_SIZE];
        char name[READ_BUF_SIZE];

        /* Must skip ".." since that is outside /proc */
        if (strcmp(next->d_name, "..") == 0)
            continue;

        /* If it isn't a number, we don't want it */
        if (!isdigit(*next->d_name))
            continue;

        sprintf(filename, "/proc/%s/status", next->d_name);
        if (! (status = fopen(filename, "r")) ) {
            continue;
        }
        if (fgets(buffer, READ_BUF_SIZE-1, status) == NULL) {
            fclose(status);
            continue;
        }
        fclose(status);

        /* Buffer should contain a string like "Name:   binary_name" */
        sscanf(buffer, "%*s %s", name);
        if (strcmp(name, pidName) == 0) {
            pidList=(pid_t*)xrealloc( pidList, sizeof(pid_t) * (i+2));
            pidList[i++]=strtol(next->d_name, NULL, 0);
        }
    }

    if (pidList)
        pidList[i]=0;
    else if ( strcmp(pidName, "init")==0) {
        /* If we found nothing and they were trying to kill "init",
         * guess PID 1 and call it good...  Perhaps we should simply
         * exit if /proc isn't mounted, but this will do for now. */
        pidList=(pid_t*)xrealloc( pidList, sizeof(pid_t));
        pidList[0]=1;
    } else {
        pidList=(pid_t*)xrealloc( pidList, sizeof(pid_t));
        pidList[0]=-1;
    }
    closedir(dir);
    return pidList;
}
char * V_ReadProvessMemory(unsigned int pid,unsigned long int addr,unsigned int size)
{
 // OutputDebugString("VREADMEM\n");
#ifndef __WIN32__
    if (ptrace(PTRACE_ATTACH,pid,NULL,NULL))
    {
        printf("Errore %d\n",errno);
        ptrace(PTRACE_DETACH, pid, NULL, NULL);
        return 0x0;
    }
    wait(NULL);
#endif
    char * data = (char*)malloc((size+(sizeof(unsigned long)-1)) & ~(sizeof(unsigned long)-1));
#ifndef __WIN32__
    for ( unsigned long int x = addr; x-addr < size; x+=sizeof(unsigned long int))
    {
        unsigned long word = ptrace(PTRACE_PEEKDATA,pid,x,NULL);
        memcpy(&data[x-addr],&word,sizeof(word));



    }
    ptrace(PTRACE_DETACH, pid, NULL, NULL);
#else
    //OutputDebugString("Opening process...");
    HANDLE hProcess = OpenProcess(PROCESS_VM_READ|PROCESS_VM_OPERATION,FALSE,pid);
  //  OutputDebugString("Reading...");
    ReadProcessMemory(hProcess,(void*)addr,data,size,NULL);
  //  OutputDebugString("Closing...");
    CloseHandle(hProcess);
   // OutputDebugString("Done...");
#endif
    return data;
}
template <class T> T ReadFromProcess(unsigned int pid,unsigned long int addr)
{
    T val;
    char * data = V_ReadProvessMemory(pid,addr,sizeof(T));
    if (!data)
        return val;
    memcpy(&val,data,sizeof(T));
    free(data);
    return val;
}

#endif

using namespace VMAP;
using namespace MMAP;
inline void copyVertices(vector<Vector3> source, G3D::Array<float> &dest)
{
    for (vector<Vector3>::iterator it = source.begin(); it != source.end(); ++it)
    {
        dest.push_back((*it).y);
        dest.push_back((*it).z);
        dest.push_back((*it).x);
    }
}
inline void transform(vector<Vector3> source, vector<Vector3> &transformedVertices, float scale, G3D::Matrix3 rotation, Vector3 position)
{
    for (vector<Vector3>::iterator it = source.begin(); it != source.end(); ++it)
    {
        // apply tranform, then mirror along the horizontal axes
        Vector3 v((*it) * rotation * scale + position);
        v.x *= -1.f;
        v.y *= -1.f;
        transformedVertices.push_back(v);
    }
}
inline void copyIndices(vector<MeshTriangle> source, G3D::Array<int> &dest, int offset, bool flip)
{
    if (flip)
    {
        for (vector<MeshTriangle>::iterator it = source.begin(); it != source.end(); ++it)
        {
            dest.push_back((*it).idx2+offset);
            dest.push_back((*it).idx1+offset);
            dest.push_back((*it).idx0+offset);
        }
    }
    else
    {
        for (vector<MeshTriangle>::iterator it = source.begin(); it != source.end(); ++it)
        {
            dest.push_back((*it).idx0+offset);
            dest.push_back((*it).idx1+offset);
            dest.push_back((*it).idx2+offset);
        }
    }
}
float agerad = 1.0f;
float client_x=0,client_y=0,client_z=0;
unsigned int client_map_id = 0x0;
bool clientattached = false;
std::string playername_s;
int pid = -1;
//-----------------------------------------------------------------------------
// Name: renderSphere()
// Desc: Create a sphere centered at cy, cx, cz with radius r, and
//       precision p. Based on a function Written by Paul Bourke.
//       http://astronomy.swin.edu.au/~pbourke/opengl/sphere/
//-----------------------------------------------------------------------------
void renderSphere( float cx, float cy, float cz, float r, int p )
{
    const float PI     = 3.14159265358979f;
    const float TWOPI  = 6.28318530717958f;
    const float PIDIV2 = 1.57079632679489f;

    float theta1 = 0.0;
    float theta2 = 0.0;
    float theta3 = 0.0;

    float ex = 0.0f;
    float ey = 0.0f;
    float ez = 0.0f;

    float px = 0.0f;
    float py = 0.0f;
    float pz = 0.0f;

    // Disallow a negative number for radius.
    if ( r < 0 )
        r = -r;

    // Disallow a negative number for precision.
    if ( p < 0 )
        p = -p;

    // If the sphere is too small, just render a OpenGL point instead.
    if ( p < 4 || r <= 0 )
    {
        glBegin( GL_POINTS );
        glVertex3f( cx, cy, cz );
        glEnd();
        return;
    }

    for ( int i = 0; i < p/2; ++i )
    {
        theta1 = i * TWOPI / p - PIDIV2;
        theta2 = (i + 1) * TWOPI / p - PIDIV2;

        glBegin( GL_TRIANGLE_STRIP );
        {
            for ( int j = 0; j <= p; ++j )
            {
                theta3 = j * TWOPI / p;

                ex = cosf(theta2) * cosf(theta3);
                ey = sinf(theta2);
                ez = cosf(theta2) * sinf(theta3);
                px = cx + r * ex;
                py = cy + r * ey;
                pz = cz + r * ez;

                glNormal3f( ex, ey, ez );
                glTexCoord2f( -(j/(float)p) , 2*(i+1)/(float)p );
                glVertex3f( px, py, pz );

                ex = cosf(theta1) * cosf(theta3);
                ey = sinf(theta1);
                ez = cosf(theta1) * sinf(theta3);
                px = cx + r * ex;
                py = cy + r * ey;
                pz = cz + r * ez;

                glNormal3f( ex, ey, ez );
                glTexCoord2f( -(j/(float)p), 2*i/(float)p );
                glVertex3f( px, py, pz );
            }
        }
        glEnd();
    }
}


std::vector<float> navverts;
int tileX,tileY;
std::vector<int> navindexes;
std::vector<int64_t> polyflags;
std::vector<uint16_t> navflags;
std::vector<TMesh*> vmap_meshes;
unsigned int polycount;
void RegenNavmeshOnthefly(TMesh * ground,TMesh * water)
{
    /*float cellSize = .5f;       // larger number => less voxels => faster build time
    float bmin[3],bmax[3];
    if (ground)
        rcCalcBounds(&ground->verts[0], ground->verts.size()/3, bmin, bmax);
    else
    {
        bmin[1] = FLT_MIN;
        bmax[1] = FLT_MAX;
    }

    // this is for width and depth
    bmax[0] = (32 - int(tileY)) * GRID_SIZE;
    bmax[2] = (32 - int(tileX)) * GRID_SIZE;
    bmin[0] = bmax[0] - GRID_SIZE;
    bmin[2] = bmax[2] - GRID_SIZE;


    float agentHeight = 1.5f;
    float agentRadius = 0.5f;
    float agentMaxClimb = 1.65f;
    rcConfig config;
    memset(&config, 0, sizeof(rcConfig));
    config.maxVertsPerPoly = (int)6;

    // these are WORLD UNIT based metrics
    config.cs = cellSize;
    config.ch = .3f;
    config.walkableSlopeAngle = 50.0;
    rcVcopy(config.bmin, bmin);
    rcVcopy(config.bmax, bmax);

    // these are VOXEL-based metrics
    rcCalcGridSize(config.bmin, config.bmax, config.cs, &config.width, &config.height);
    config.tileSize = (int)ceilf(GRID_SIZE / config.cs);
    config.walkableRadius = (int)ceilf(agentRadius / config.cs);
    config.borderSize = config.walkableRadius + 3;
    config.maxEdgeLen = (int)(15.f / config.cs);
    config.walkableHeight = (int)ceilf(agentHeight / config.ch);
    config.walkableClimb = (int)floorf(agentMaxClimb / config.ch);
    config.minRegionSize = (int)rcSqr(50);
    config.mergeRegionSize = (int)rcSqr(20);

    // unknown metric
    config.maxSimplificationError = 1.3f;
    config.detailSampleDist = 6.f < 0.9f ? 0 : config.cs * 6.f;
    config.detailSampleMaxError = config.ch * 1.f;

    // pad bounds with a border
    config.bmin[0] -= config.borderSize*config.cs;
    config.bmin[2] -= config.borderSize*config.cs;
    config.bmax[0] += config.borderSize*config.cs;
    config.bmax[2] += config.borderSize*config.cs;
    rcCalcGridSize(config.bmin, config.bmax, config.cs, &config.width, &config.height);
    rcHeightfield * heightfield = NEW(rcHeightfield);
    if (!heightfield || !rcCreateHeightfield(*heightfield, config.width, config.height, config.bmin, config.bmax, config.cs, config.ch))
        return;
    unsigned char*triFlags = NEW_ARRAY(unsigned char,(ground->indexes.size()/3));
    memset(triFlags, 0, (ground->indexes.size()/3)*sizeof(unsigned char));
    rcMarkWalkableTriangles(config.walkableSlopeAngle,&ground->verts[0], ground->verts.size()/3,(const int*)&ground->indexes[0], ground->indexes.size()/3, triFlags);
    rcRasterizeTriangles(&ground->verts[0], ground->verts.size()/3, (const int*)&ground->indexes[0],triFlags , 1, ground->indexes.size()/3, *heightfield, config.walkableClimb);
    rcFilterLowHangingWalkableObstacles(config.walkableClimb, *heightfield);
    rcFilterLedgeSpans(config.walkableHeight, config.walkableClimb, *heightfield);
    rcFilterWalkableLowHeightSpans(config.walkableHeight, *heightfield);
    rcCompactHeightfield* compactHeightfield = NEW(rcCompactHeightfield);
    if (!compactHeightfield || !rcBuildCompactHeightfield(config.walkableHeight, config.walkableClimb, RC_WALKABLE, *heightfield, *compactHeightfield))
    {
        return;

    }
    DELETE(heightfield);
    rcErodeArea(1 /*GROUND*//*, config.walkableRadius, *compactHeightfield);

    //printf("%sBuilding distance field...              \r", tileString);
    if (!rcBuildDistanceField(*compactHeightfield))
        return;

    // bottleneck is here
    //printf("%sBuilding regions...                     \r", tileString);
    if (!rcBuildRegions(*compactHeightfield, config.borderSize, config.minRegionSize, config.mergeRegionSize))
    {
        printf("Failed building regions!                \n");
        return;
    }
    rcContourSet * contours = NEW(rcContourSet);
    if (!contours || !rcBuildContours(*compactHeightfield, config.maxSimplificationError, config.maxEdgeLen, *contours))
    {
        printf("Failed building contours!               \n");
        return;
    }
    rcPolyMesh * polyMesh = NEW(rcPolyMesh);
    if (!polyMesh || !rcBuildPolyMesh(*contours, config.maxVertsPerPoly, *polyMesh))
    {
        printf("Failed building polymesh!               \n");
        return;
    }
    rcPolyMeshDetail * polyMeshDetail = NEW(rcPolyMeshDetail);
    if (!polyMeshDetail || !rcBuildPolyMeshDetail(*polyMesh, *compactHeightfield, config.detailSampleDist, config.detailSampleMaxError, *polyMeshDetail))
    {
        printf("Failed building polymesh detail!        \n");
        return;
    }

    polycount = polyMesh->npolys;
    navverts.clear();
    navindexes.clear();
    dtNavMeshCreateParams params;
    memset(&params, 0, sizeof(params));
    params.verts = polyMesh->verts;
    params.vertCount = polyMesh->nverts;
    params.polys = polyMesh->polys;
    params.polyAreas = polyMesh->areas;
    params.polyFlags = polyMesh->flags;
    params.polyCount = polyMesh->npolys;
    params.nvp = polyMesh->nvp;
    params.detailMeshes = polyMeshDetail->meshes;
    params.detailVerts = polyMeshDetail->verts;
    params.detailVertsCount = polyMeshDetail->nverts;
    params.detailTris = polyMeshDetail->tris;
    params.detailTriCount = polyMeshDetail->ntris;
    params.walkableHeight = agentHeight;
    params.walkableRadius = agentRadius;
    params.walkableClimb = agentMaxClimb;
    params.tileX = tileX;
    params.tileY = tileY;
    rcVcopy(params.bmin, bmin);
    rcVcopy(params.bmax, bmax);
    params.cs = config.cs;
    params.ch = config.ch;
    params.tileSize = config.tileSize;

    // will hold final navmesh
    unsigned char* navData = 0;
    int navDataSize = 0;
    dtCreateNavMeshData(&params,&navData,&navDataSize);
    if (navData)
    {
        dtMeshHeader* header = (dtMeshHeader*)navData;
        if (header->magic != DT_NAVMESH_MAGIC)
        {
            printf("Bad magic\n");
            return;
        }
        if (header->version != DT_NAVMESH_VERSION)
        {
            printf("Bad version\n");
            return;
        }

        unsigned char * data2 = navData + dtAlign4(sizeof(dtMeshHeader));

        for ( int i = 0; i < header->vertCount; i++ )
        {
            navverts.push_back(*((float*)&data2[i*sizeof(float)*3]));
            navverts.push_back(*((float*)&data2[i*sizeof(float)*3+sizeof(float)]));
            navverts.push_back(*((float*)&data2[i*sizeof(float)*3+sizeof(float)*2]));
        }
        data2 += dtAlign4(sizeof(float)*3*header->vertCount);
        for ( int i = 0; i < header->polyCount; i++ )
        {
            dtPoly * poly = (dtPoly*)data2;
            for ( int x = 0; x < poly->vertCount; x++)
            {
                navindexes.push_back((int)poly->verts[x]);
                navflags.push_back(poly->flags);
            }
            navflags.push_back(poly->flags);
            navindexes.push_back(-1);
            data2 += sizeof(dtPoly);
        }
        polycount = header->polyCount;

    }*/

}
#ifndef DISABLE_PROCESS_ATTACH
unsigned int GetClientCameraPtr(unsigned int pid)
{
    unsigned int dword_B7436C = ReadFromProcess<unsigned int>(pid,0xB7436C);
    unsigned int camera_pointer = ReadFromProcess<unsigned int>(pid,dword_B7436C+0x7E20);
    return camera_pointer;


}
#endif
float client_cam_x,client_cam_y,client_cam_z;
void UpdateClientStatus()
{
  #ifndef DISABLE_PROCESS_ATTACH

    pid = -1;
    unsigned int STATIC_PLAYER   = 0xCD87A8;
    const unsigned int PlayerBaseOffset1 = 0x34;
    const unsigned int  PlayerBaseOffset2 = 0x24;
    const unsigned int    XOffset = 0x798;                // PlayerX
    const unsigned int    YOffset = 0x79c;                // PlayerY
    const unsigned int    ZOffset = 0x7a0;                // PlayerZ
#ifndef __WIN32__
    pid_t* pidlist = find_pid_by_name("Wow.exe");
    if ( pidlist )
    {
        pid = *pidlist;

        free(pidlist);
    }

    //clientstatus << "PID del client: " << pid << " - ";
    if ( pid == -1)
        return;
#else
    //OutputDebugString("UpdateClientStatus1\n");
    pid = W32GetWowPid();
   // OutputDebugString("UpdateClientStatus2\n");
    if ( pid == 0 )
    {
      pid = -1;
      return;
    }
    //OutputDebugString("UpdateClientStatus3\n");

	
#endif

    char * playername = V_ReadProvessMemory(pid,0xC79D18,30);

    if (!playername)
        playername_s = "@Errore@";
    else {
        playername[29] = 0;

        playername_s = playername;
        free(playername);
    }
    unsigned long playerbase = ReadFromProcess<unsigned int>(pid,ReadFromProcess<unsigned int>(pid,ReadFromProcess<unsigned int>(pid,STATIC_PLAYER)+PlayerBaseOffset1)+PlayerBaseOffset2);

    client_x = ReadFromProcess<float>(pid,playerbase+XOffset);
    client_y = ReadFromProcess<float>(pid,playerbase+YOffset);
    client_z = ReadFromProcess<float>(pid,playerbase+ZOffset);
    unsigned int camera = GetClientCameraPtr(pid);
    client_cam_x = ReadFromProcess<float>(pid,camera+0x8);
    client_cam_y = ReadFromProcess<float>(pid,camera+0xC);
    client_cam_z = ReadFromProcess<float>(pid,camera+0x10);
    client_map_id = ReadFromProcess<unsigned int>(pid,0xAB63BC);
	//char * dataraw = V_ReadProvessMemory(pid,0xA6CF1D-4,1024*10);
	/*FILE * f = fopen("memdump.bin","wb");
	fwrite(dataraw,1024*10,1,f);
	fclose(f);
	free(dataraw);*/
//clientstatus << " X: " << x << " Y: " << y << " Z: " << z;
#endif



}
float centerx=0.0f,centery=0.0f,centerz=0.0f;
bool firstload = true;
engine * Main = NULL;
#define LOADINGMSG(msg) Main->clearscreen(0.5,0.5,0.8,1.0);font->Render(msg,-1,FTPoint(0,0));Main->display();
void renderprogressbar(float _x , float _y, float height , float width , float frac)
{
  glDisable(GL_COLOR_MATERIAL);
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);
  glBindTexture(GL_TEXTURE_2D,0);
  float x=0,y =0;
  Main->clearscreen(0.0,0.0,0.3,1.0);
  Main->OrthoMode();
  glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
  glColor4f(1,1,1,1);
  glBegin(GL_QUADS);
  {
	glVertex3f(x,y,0);
	glVertex3f(x+width,y,0);
	glVertex3f(x+width,y+height,0);
	glVertex3f(x,y+height,0);
  }
  glEnd();
  glColor4f(0,0,1,1);
  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  glBegin(GL_QUADS);
  {
	glVertex3f(x,y,0);
	glVertex3f(x+width*frac,y,0);
	glVertex3f(x+width*frac,y+height,0);
	glVertex3f(x,y+height,0);
  }
  glEnd();
  glColor4f(1,1,1,1);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_LIGHTING);
  glEnable(GL_DEPTH_TEST);
  glBindTexture(GL_TEXTURE_2D,0);
  Main->NormalMode();
  Main->display();
}
FTGLTextureFont * font = NULL;

void deccb(float f)
{
  char msg[256];
  sprintf(msg,"Decimazione: %0.2f%% Completata",f*100.0);
  //LOADINGMSG(msg)
  
}
void FreeVmapSelectModels()
{
    for ( std::vector<TMesh*>::iterator it = vmap_meshes.begin(); it != vmap_meshes.end(); it++ )
    {
        delete (*it);
        
    }
    vmap_meshes.clear();
}

void LoadNavMesh(unsigned int mapid,unsigned int gx,unsigned int gy)
{
    char filename[512];
    char drop[124];
    unsigned int offset = navverts.size();
    sprintf(filename,"mmaps/%03u%02u%02u_1.mmtile",mapid, gx, gy);
    printf("%s\n",filename);
    FILE * mmtile = fopen(filename,"rb");
    if ( mmtile )
    {
        fseek(mmtile,0,SEEK_END);
        int dataSize = ftell(mmtile);
        fseek(mmtile,0,SEEK_SET);
        unsigned char * data = (unsigned char*)malloc(dataSize);

        fread(data,dataSize,1,mmtile);
        fclose(mmtile);
        dtMeshHeader* header = (dtMeshHeader*)data;
        if (header->magic != DT_NAVMESH_MAGIC)
            return;
        if (header->version != DT_NAVMESH_VERSION)
            return;

        unsigned char * data2 = data + dtAlign4(sizeof(dtMeshHeader));

        for ( int i = 0; i < header->vertCount; i++ )
        {
            navverts.push_back(*((float*)&data2[i*sizeof(float)*3]));
            navverts.push_back(*((float*)&data2[i*sizeof(float)*3+sizeof(float)]));
            navverts.push_back(*((float*)&data2[i*sizeof(float)*3+sizeof(float)*2]));
        }
        data2 += dtAlign4(sizeof(float)*3*header->vertCount);
        for ( int i = 0; i < header->polyCount; i++ )
        {
            dtPoly * poly = (dtPoly*)data2;
            for ( int x = 0; x < poly->vertCount; x++)
            {
                navflags.push_back(poly->flags);
                navindexes.push_back((int)poly->verts[x]+offset/3);
                if ( polyflags.size() > 0 && polyflags[polyflags.size()-1] != -1 )
                {
                    polyflags.push_back(poly->neis[x] | polyflags[polyflags.size()-1]);
                }else{
                    polyflags.push_back(poly->neis[x]);
                }
                
            }
            
 
            
            
            
            navindexes.push_back(-1);
            polyflags.push_back(-1);
            navflags.push_back(poly->flags);
            data2 += sizeof(dtPoly);
        }
        polycount = header->polyCount;
        
        unsigned char* d = data;
        d += dtAlign4(sizeof(dtMeshHeader));
        const int vertsSize = dtAlign4(sizeof(float)*3*header->vertCount);
        const int polysSize = dtAlign4(sizeof(dtPoly)*header->polyCount);
        const int linksSize = dtAlign4(sizeof(dtLink)*header->maxLinkCount);
        const int detailMeshesSize = dtAlign4(sizeof(dtPolyDetail)*header->polyCount);
        const int detailVertsSize = dtAlign4(sizeof(float)*3*header->detailVertCount);
        const int detailTrisSize = dtAlign4(sizeof(unsigned char)*4*header->detailVertCount);
        float* navVerts = (float*)d; d += vertsSize;
        dtPoly* navPolys = (dtPoly*)d; d += polysSize;
        d += linksSize;
        dtPolyDetail* navDMeshes = (dtPolyDetail*)d; d += detailMeshesSize;
        float* navDVerts = (float*)d; d += detailVertsSize;
        unsigned char* navDTris = (unsigned char*)d; d += detailTrisSize;
       

        /*
         * dtPolyDetail* navDMeshes = (dtPolyDetail*)d; d += detailMeshesSize;
        
        */
        
        
        

        for ( int i = 0; i < header->polyCount; i++ )
        {
            TMesh * temp = new TMesh;
            temp->AcquireDirectAccess();
            dtPolyDetail * pd = &navDMeshes[i];
            dtPoly * poly = &navPolys[i];
            if ( pd->vertCount > 0xFF )
            {
                printf("Warning: polygon has overflow detail vertices count!");
            }
            /*temp->verts.resize((navDMeshes[i].vertCount+navPolys[i].vertCount)*3);
            for ( int k = 0; k < navPolys[i].vertCount; k++ )
            {
                temp->verts[k*3+0] = navVerts[navPolys[i].verts[k]*3+0]; 
                temp->verts[k*3+1] = navVerts[navPolys[i].verts[k]*3+1]; 
                temp->verts[k*3+2] = navVerts[navPolys[i].verts[k]*3+2]; 
            }
            memcpy(&temp->verts[navPolys[i].vertCount],&navDVerts[navDMeshes[i].vertBase],navDMeshes[i].vertCount*sizeof(float)*3);
            for ( int c = 0; c < navDMeshes[c].triCount; c++ )
            {
                unsigned char * t = &navDTris[(navDMeshes[i].triBase+c)*4];
                temp->indexes.push_back(t[0]);
                temp->indexes.push_back(t[1]);
                temp->indexes.push_back(t[2]);
                
            }*/
            for (int j = 0; j < pd->triCount; ++j)
            {
                const unsigned char* t = &navDTris[(pd->triBase+j)*4];
                const float* v[3];
                for (int k = 0; k < 3; ++k)
                {
                        if (t[k] < poly->vertCount)
                                v[k] = &navVerts[poly->verts[t[k]]*3];
                        else
                                v[k] = &navDVerts[(pd->vertBase+(t[k]-poly->vertCount))*3];
                        
                }
                for ( int k = 0; k < 3; k++ )
                {
                    
                    temp->verts.push_back(v[k][0]);
                    temp->verts.push_back(v[k][1]);
                    temp->verts.push_back(v[k][2]);
                    //printf("%f %f %f\n",v[k][0],v[k][1],v[k][2]);
                    
                }
                temp->indexes.push_back(j*3+0);
                temp->indexes.push_back(j*3+1);
                temp->indexes.push_back(j*3+2);
                
            }
            temp->ReleaseDirectAccess();
            //temp->generateNormals();
            //temp->removeDoubles();
            //printf("Detail mesh: %d tris\n",temp->indexes.size()/3);
            polyDetail->AddMesh(temp,false);
            delete temp;
        }
        
        free(data);
    } else {
        printf("\033[31mImpossibile caricare la mmap %s\033[0m\n",filename);
    };
    
    
}
int LoadTile(TMesh * landmesh,TMesh * vmap, TMesh * liqmesh,unsigned int mapid , unsigned int gx,unsigned int gy)
{
	if (!font)
	  font = new FTTextureFont("Vera.ttf");
	font->FaceSize(12);
	printf("Loading tile [%u,%u]\n",gx,gy);
    VMAP::VMapManager2 * vmapmanager = new VMAP::VMapManager2();
    MMAP::TileBuilder * tb = new MMAP::TileBuilder(true);
    G3D::Array<float> modelVerts;
    G3D::Array<int> modelTris;
    uint32 count = 0;
    ModelInstance* models = 0;
    FreeVmapSelectModels();
	
	
	
	//LOADINGMSG("Caricamento vmap...")
	renderprogressbar(200,200,50,200,0);
    vmapmanager->loadMap("vmaps",mapid,gx,gy);
    
    InstanceTreeMap instanceTrees;
    ((VMAP::VMapManager2*)vmapmanager)->getInstanceMapTree(instanceTrees);
    if ( instanceTrees.find(mapid) == instanceTrees.end() )
        return 0;
    instanceTrees[mapid]->getModelInstances(models, count);
    
    for (int i = 0; i < count; ++i)
    {
        ModelInstance instance = models[i];

        // model instances exist in tree even though there are instances of that model in this tile
        WorldModel* worldModel = instance.getWorldModel();
        if (!worldModel)
            continue;

        vector<GroupModel> groupModels;
        worldModel->getGroupModels(groupModels);

        // all M2s need to have triangle indices reversed
        bool isM2 = instance.name.find(".m2") != instance.name.npos || instance.name.find(".M2") != instance.name.npos;

        // transform data
        float scale = models[i].iScale;
        G3D::Matrix3 rotation = G3D::Matrix3::fromEulerAnglesZYX(-1*G3D::pi()*instance.iRot.y/180.f, -1*G3D::pi()*instance.iRot.x/180.f, -1*G3D::pi()*instance.iRot.z/180.f);
        Vector3 position = instance.iPos;
        position.x -= 32*533.33333f;
        position.y -= 32*533.33333f;

        for (vector<GroupModel>::iterator it = groupModels.begin(); it != groupModels.end(); ++it)
        {
            vector<Vector3> tempVertices;
            vector<Vector3> transformedVertices;
            vector<MeshTriangle> tempTriangles;

            (*it).getMeshData(tempVertices, tempTriangles);

            transform(tempVertices, transformedVertices, scale, rotation, position);

            int offset = modelVerts.size() / 3;

            copyVertices(transformedVertices, modelVerts);
            copyIndices(tempTriangles, modelTris, offset, isM2);
            
            
            TMesh * gm = new TMesh;
            gm->AcquireDirectAccess();
            for ( std::vector<Vector3>::iterator it2 = transformedVertices.begin(); it2!= transformedVertices.end(); it2++ )
            {
                gm->verts.push_back((*it2).y);
                gm->verts.push_back((*it2).z);
                gm->verts.push_back((*it2).x);
            }
            for ( std::vector<MeshTriangle>::iterator it2 = tempTriangles.begin(); it2!= tempTriangles.end(); it2++ )
            {
                gm->indexes.push_back((*it2).idx0);
                gm->indexes.push_back((*it2).idx1);;
                gm->indexes.push_back((*it2).idx2);
            }
            gm->ReleaseDirectAccess();
            gm->fixmesh();
            gm->removeDoubles();
            gm->generateNormals();
            gm->UploadData();
            std::stringstream info;
            info << "File: '" << instance.name << "' Posizione(Centro)(GroupModel): " << (*it).GetBound().center().toString() << " Orientamento(Instance): " << instance.iRot.toString() << " Posizione(Instance) " << instance.iPos.toString();
            gm->info = info.str();
            vmap_meshes.push_back(gm);
        }
    }
    int vertCount = modelVerts.size()/3;
	if ( firstload )
	{
	  for ( int k = 0; k < vertCount; k++ )
	  {
		  centery+=modelVerts[k*3+1];


	  }
	  centerx = (float(63-gx)-32+0.5f)*533.33333f;
	  centerz = (float(63-gy)-32+0.5f)*533.33333f;
	  centery /= vertCount > 0 ? vertCount : 1;
	  firstload = false;
	}
	vmap->AcquireDirectAccess();
    vmap->verts.clear();
    vmap->indexes.clear();

    for ( int i = 0; i < modelVerts.size()/3; i++ )
    {
        vmap->verts.push_back(modelVerts[i*3]);
        vmap->verts.push_back(modelVerts[i*3+1]);
        vmap->verts.push_back(modelVerts[i*3+2]);
    }
    for ( int i = 0; i < modelTris.size(); i++ )
    {
        vmap->indexes.push_back(modelTris[i]);
    }
    //LOADINGMSG("Rimozione vertici doppi...")
	renderprogressbar(200,200,50,200,0.1);
	vmap->ReleaseDirectAccess();
    vmap->removeDoubles();
	//LOADINGMSG("Generazione normali")
	renderprogressbar(200,200,50,200,0.3);
    vmap->generateNormals();
	//LOADINGMSG("Generazione coordinate...")
	renderprogressbar(200,200,50,200,0.5);
    vmap->generateTrisTexCoords();
	//LOADINGMSG("Creazione VBO...")
	renderprogressbar(200,200,50,200,0.6);
    vmap->UploadData();
	vmap->launchdecimate(0.3);
    MeshData meshData;
	//LOADINGMSG("Caricamento terreno...")
	renderprogressbar(200,200,50,200,0.65);
    tb->loadMap(mapid, gy, gx, meshData);
    liqmesh->indexes.clear();
    liqmesh->verts.clear();
	landmesh->AcquireDirectAccess();
    landmesh->indexes.clear();
    landmesh->verts.clear();
    tb->cleanVertices(meshData.solidVerts, meshData.solidTris);
    tb->cleanVertices(meshData.liquidVerts, meshData.liquidTris);
    for ( int i = 0; i < meshData.solidVerts.size()/3; i++ )
    {
        landmesh->verts.push_back(meshData.solidVerts[i*3]);
        landmesh->verts.push_back(meshData.solidVerts[i*3+1]);
        landmesh->verts.push_back(meshData.solidVerts[i*3+2]);
    }
    for ( int i = 0; i < meshData.solidTris.size(); i++ )
    {
        landmesh->indexes.push_back(meshData.solidTris[i]);
    }
	liqmesh->AcquireDirectAccess();
    for ( int i = 0; i < meshData.liquidVerts.size()/3; i++ )
    {
        liqmesh->verts.push_back(meshData.liquidVerts[i*3]);
        liqmesh->verts.push_back(meshData.liquidVerts[i*3+1]);
        liqmesh->verts.push_back(meshData.liquidVerts[i*3+2]);
    }
    for ( int i = 0; i < meshData.liquidTris.size(); i++ )
    {
        liqmesh->indexes.push_back(meshData.liquidTris[i]);
    }
    
    liqmesh->ReleaseDirectAccess();
    //LOADINGMSG("Rimozione vertici duplicati...")
	renderprogressbar(200,200,50,200,0.68);
	landmesh->ReleaseDirectAccess();
    landmesh->removeDoubles();
    landmesh->generateNormals();
    landmesh->generateTrisTexCoords();
    printf("\033[1m\033[3mLiquido , vertici finali: %u\033[0m\n",liqmesh->verts.size());
    liqmesh->generateNormals();
	//liqmesh->launchdecimate(0.2);
	//LOADINGMSG("Creazione VBO...")
	renderprogressbar(200,200,50,200,0.7);
	landmesh->launchdecimate(0.3);
	
	landmesh->removeDoubles();
	landmesh->generateNormals();
    landmesh->generateTrisTexCoords();
    landmesh->UploadData();
    delete vmapmanager;
    delete tb;
    //LOADINGMSG("Caricamento NavMesh...")
    
    
    navverts.clear();
    navindexes.clear();
    polyflags.clear();
    navflags.clear();
    
    if ( !polyDetail )
            polyDetail = new TMesh();
        polyDetail->AcquireDirectAccess();
        polyDetail->normals.clear();
        polyDetail->verts.clear();;
        polyDetail->indexes.clear();
        polyDetail->ReleaseDirectAccess();
    LoadNavMesh(mapid,gx,gy);
    LoadNavMesh(mapid,gx,gy-1);
    LoadNavMesh(mapid,gx,gy+1);
    renderprogressbar(200,200,50,200,0.75);
    LoadNavMesh(mapid,gx-1,gy);
    LoadNavMesh(mapid,gx-1,gy-1);
    LoadNavMesh(mapid,gx-1,gy+1);
    renderprogressbar(200,200,50,200,0.85);
    LoadNavMesh(mapid,gx+1,gy);
    LoadNavMesh(mapid,gx+1,gy-1);
    LoadNavMesh(mapid,gx+1,gy+1);
    renderprogressbar(200,200,50,200,1.0);
    polyDetail->removeDoubles();
    
    polyDetail->generateNormals();
        
        polyDetail->UploadData();
}

int main(int argc,char** argv)
{
    /*ilInit();
    ilutRenderer(ILUT_OPENGL);*/
    float bmin[3], bmax[3];
    GLuint selection_buffer[64] = {0};
    GLint hits , view[4];










    Main = new engine();
    Main->Init3d(800,600);
    if ( not (GL_EXT_geometry_shader4))
    {
        printf("Supporto agli shader geometrici mancante\n");
        return 1;

    }

  FTGLTextureFont font("Vera.ttf");
    font.FaceSize(12);
	FTGLTextureFont font2("Vera.ttf");
    font2.FaceSize(60);
    char txt[1024];
    bool quit = false;
    float rx=0.0,ry=0.0;
	FBO tilefb;
	tilefb.Create(600,600);
    // the texture wraps over at the edges (repeat)
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glSelectBuffer(64,selection_buffer);
    TMesh * landmesh = new TMesh;
    TMesh * msh = new TMesh;
    TMesh * liqmesh = new TMesh;
    int gx=0,gy=0,mapid=0;
    if ( argc == 4 )
    {
        gx=atoi(argv[1]);
        gy=atoi(argv[2]);
        mapid=atoi(argv[3]);
    }else{
        printf("Coordinate e mappa non specificate , tento di agganciare il client...\n");
        
    }
	 Main->clearscreen(0.0,0.0,0.0,1.0);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0, Main->width, 0.0, Main->height, -1.0, 1.0); 
	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_DEPTH_TEST);
	glColor4f(1,1,1,1);
	glDisable(GL_LIGHTING);
	
	font.Render("Caricamento tile...",-1,FTPoint(0,0));
	Main->display();
    LoadTile(landmesh,msh,liqmesh,mapid,gx,gy);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
    UpdateClientStatus();
    if ( pid != -1 )
    {
        double x_offset = (double(client_x) - 533.33333f/2.0)/533.333333f;
        double y_offset = (double(client_y) - 533.33333f/2.0)/533.333333f;

        int newgx = 63-int(x_offset+32 + 0.5);
        int newgy = 63-int(y_offset+32 + 0.5);
        if ( newgx != gx || newgy != gy || mapid != client_map_id)
        {
            gx = newgx;
                    gy = newgy;
                    Main->clearscreen(0.0,0.0,0.0,1.0);
					glLoadIdentity();
                    glMatrixMode(GL_PROJECTION);
					glPushMatrix();
					glLoadIdentity();
					glOrtho(0.0, Main->width, 0.0, Main->height, -1.0, 1.0); 
					glMatrixMode(GL_MODELVIEW);
					glDisable(GL_DEPTH_TEST);
					glColor4f(1,1,1,1);
					glDisable(GL_LIGHTING);
					
                    font.Render("Caricamento tile...",-1,FTPoint(0,0));
                    Main->display();
                    mapid = client_map_id;
                    LoadTile(landmesh,msh,liqmesh,mapid,gx,gy);
					glMatrixMode(GL_PROJECTION);
					glPopMatrix();
					glMatrixMode(GL_MODELVIEW);

        }
    }



    //gluLookAt(centerx,centery,centerz,centerx,centery,centerz-20,0,1,0);
    //Main->SetCameraPos(centerx,centery,centerz+70);
    //Main->SetCameraLookAt(centerx,centery,centerz);


    /*FBO * navmeshdepth = new FBO;
    navmeshdepth->Create(1024,768);*/

    float r = 0.0;
    //shader * s = new shader;
    shader * wireframe = new shader;
    bool wireshaderok = wireframe->createfromfiles("wireframe.frag","wireframe.vert","wireframe.geom");
    //s->createfromfiles("wf.frag","wf.vert");
    int tx=0,ty=0;
    if ( argc == 4 )
    {
        tx = atoi(argv[2]);
        ty = atoi(argv[3]);
    }
    bool grab = false;
    glDisable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    //glShadeModel(GL_SMOOTH);
    //glFrontFace(GL_CCW);
    light * l = new light;
    l->lightinit(GL_LIGHT0,0,0,0);
    l->setspeccolor(1.0,0,0);
    l->setambcolor(0.5,0.5,0.5);
    l->setdiffcolor(1,1,1);
   // matset m;
    //s->setuniformsampler("navmeshdepth",navmeshdepth->depthtexture);
   // m.setambient(0.7,0.7,0.7,1.0);
    float ox=centerx,oy=centery,oz=centerz;
    glColorMaterial(GL_FRONT, GL_DIFFUSE);
    int x = 0;
    l->setpos(0,-1.0,0);
    double lastframetime = getcurrenttime();
    while (quit == false )
    {
        x++;
        double framestart = getcurrenttime();

        if ( x % 10 == 0 )
        {
            UpdateClientStatus();
            if ( pid != -1 )
            {
                double x_offset = (double(client_x) - 533.33333f/2.0)/533.333333f;
                double y_offset = (double(client_y) - 533.33333f/2.0)/533.333333f;

                int newgx = 63-int(x_offset+32 + 0.5);
                int newgy = 63-int(y_offset+32 + 0.5);
                if ( newgx != gx || newgy != gy || mapid != client_map_id)
                {
                    gx = newgx;
                    gy = newgy;
                    Main->clearscreen(0.0,0.0,0.0,1.0);
					glLoadIdentity();
                    glMatrixMode(GL_PROJECTION);
					glPushMatrix();
					glLoadIdentity();
					glOrtho(0.0, Main->width, 0.0, Main->height, -1.0, 1.0); 
					glMatrixMode(GL_MODELVIEW);
					glDisable(GL_DEPTH_TEST);
					glColor4f(1,1,1,1);
					glDisable(GL_LIGHTING);
					
                   // font.Render("Caricamento tile...",-1,FTPoint(0,0));
                    Main->display();
                    mapid = client_map_id;
                    LoadTile(landmesh,msh,liqmesh,mapid,gx,gy);
					glMatrixMode(GL_PROJECTION);
					glPopMatrix();
					glMatrixMode(GL_MODELVIEW);
                    l->setpos(0,-1.0,0);

                }
            } else {
                double x_offset = (double(centerz) - 533.33333f/2.0)/533.333333f;
                double y_offset = (double(centerx) - 533.33333f/2.0)/533.333333f;

                int newgx = 63-int(x_offset+32 + 0.5);
                int newgy = 63-int(y_offset+32 + 0.5);
                if ( newgx != gx || newgy != gy)
                {
                    gx = newgx;
                    gy = newgy;
                    Main->clearscreen(0.0,0.0,0.0,1.0);
					glLoadIdentity();
                    glMatrixMode(GL_PROJECTION);
					glPushMatrix();
					glLoadIdentity();
					glOrtho(0.0, Main->width, 0.0, Main->height, -1.0, 1.0); 
					glMatrixMode(GL_MODELVIEW);
					glDisable(GL_DEPTH_TEST);
					glColor4f(1,1,1,1);
					glDisable(GL_LIGHTING);
					
                    font.Render("Caricamento tile...",-1,FTPoint(0,0));
                    Main->display();
                    LoadTile(landmesh,msh,liqmesh,mapid,gx,gy);
					glMatrixMode(GL_PROJECTION);
					glPopMatrix();
					glMatrixMode(GL_MODELVIEW);
                    l->setpos(0,-1.0,0);


                }


            }
        }

        glLoadIdentity();


        
	glRenderMode(GL_SELECT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //wireframe->useshader();
        glMatrixMode(GL_PROJECTION);
        
        glPushMatrix();
        glLoadIdentity();
        glGetIntegerv(GL_VIEWPORT,view);
        gluPickMatrix(Main->mousex,  Main->height-Main->mousey, 3, 3, view);
        gluPerspective(Main->fov, (float)Main->width/(float)Main->height, 1.0, 1000.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        //glPushMatrix();
        if ( pid == -1 )
        {
            glRotatef(rx,1.0,0.0,0.0);
            glRotatef(ry,0.0,1.0,0.0);

            glTranslatef(-centerx,-centery,-centerz);
        } else {
            gluLookAt(client_cam_y,client_cam_z,client_cam_x,client_y,client_z,client_x,0,1,0);

        }
        glInitNames();
        glPushName(0);
        for ( int i = 0; i < vmap_meshes.size(); i++)
        {
            glLoadName(i);
            vmap_meshes[i]->RenderVBO(Main,0,0,0,0,0,0);
        }
        //SDL_GL_SwapBuffers();
        //Main->display();
        hits = glRenderMode(GL_RENDER);
        //glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);

        Main->clearscreen(0.5,0.5,0.5,1.0);
        if ( Main->getkey(SDLK_ESCAPE))
            quit = true;
        if ( Main->getkey('w'))
        {
            float xrotrad, yrotrad;
            yrotrad = (ry / 180 * 3.141592654f);
            xrotrad = (rx / 180 * 3.141592654f);
            centerx += float(sin(yrotrad))*lastframetime*1000.0*0.3 ;
            centerz -= float(cos(yrotrad))*lastframetime*1000.0*0.3  ;
            centery -= float(sin(xrotrad))*lastframetime*1000.0*0.3  ;
        }
        if ( Main->getkey('e'))
        {
            std::cout << "Exporting to obj file..." << std::endl;
            FILE * objfile = fopen("out.obj","w");
            if ( objfile )
            {
                for ( int i = 0; i < msh->verts.size()/3; i++ )
                {
                    fprintf(objfile,"v %f %f %f\n",msh->verts[i*3+0],msh->verts[i*3+1],msh->verts[i*3+2]);
                }
                for ( int i = 0; i < landmesh->verts.size()/3; i++ )
                {
                    fprintf(objfile,"v %f %f %f\n",landmesh->verts[i*3+0],landmesh->verts[i*3+1],landmesh->verts[i*3+2]);
                }
                for ( int i = 0; i < msh->indexes.size()/3; i++ )
                {
                    fprintf(objfile,"f %u %u %u\n",msh->indexes[i*3+0]+1,msh->indexes[i*3+1]+1,msh->indexes[i*3+2]+1);

                }
                for ( int i = 0; i < landmesh->indexes.size()/3; i++ )
                {
                    fprintf(objfile,"f %u %u %u\n",landmesh->indexes[i*3+0]+msh->verts.size()/3+1,landmesh->indexes[i*3+1]+msh->verts.size()/3+1,landmesh->indexes[i*3+2]+msh->verts.size()/3+1);
                }
                fclose(objfile);
            }



        }
        if ( Main->getkey('s'))
        {
            float xrotrad, yrotrad;
            yrotrad = (ry / 180 * 3.141592654f);
            xrotrad = (rx / 180 * 3.141592654f);
            centerx -= float(sin(yrotrad))*lastframetime*1000.0*0.3 ;
            centerz += float(cos(yrotrad))*lastframetime*1000.0*0.3 ;
            centery += float(sin(xrotrad))*lastframetime*1000.0*0.3 ;
        }
        if ( Main->getkey('b'))
        {
            grab = false;
            SDL_WM_GrabInput(SDL_GRAB_OFF);
            SDL_ShowCursor(1);
        }

        if ( Main->getkey('g'))
        {
            grab = true;
            SDL_WM_GrabInput(SDL_GRAB_ON);
            SDL_ShowCursor(0);
        }
        
        for (int GX = gx -1; GX < gx +2; GX++ )
		{
		  for (int GY = gy -1; GY < gy +2; GY++ )
		  {
			//printf("RENDER %d %d\n",GX,GY);
			if ( not ( GX == gx && GY == gy ) )
			{
			  glEnable(GL_TEXTURE_2D);
			  glBindTexture(GL_TEXTURE_2D,0);
			  glPushMatrix();
			  glLoadIdentity();
			  glDisable(GL_BLEND);
			  
			  glDisable(GL_LIGHTING);
			  glDisable(GL_DEPTH_TEST);
			  tilefb.BindFBO();
			  glClearColor(0.5,0.0,0.5,1);
			  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
			 /* glColor4f(1,0,1,1);
			  glBegin(GL_QUADS);
			  glVertex2f(0,0);
			  glVertex2f(1,0);
			  glVertex2f(1,1);
			  glVertex2f(0,1);
			  
			  glEnd();*/
			  float Y = centery -20;
			  if ( !isnan(client_cam_z) )
				Y = client_cam_z;
			  char tstr[128];
			  sprintf(tstr,"TILE [%d || %d]",GX,GY);
			  font2.Render(tstr);
			  tilefb.UnBindFBO();
			  glEnable(GL_DEPTH_TEST);
			  glColor4f(1,1,1,1);
			  glBindTexture(GL_TEXTURE_2D,tilefb.texture);
			  glPopMatrix();
			  glDisable(GL_COLOR_MATERIAL);
			  glBegin(GL_QUADS);
			  glTexCoord2f(0,5);
			  glVertex3f(533.333333f*32.0-(GY+0)*533.333333f,0.0,533.333333f*32.0-(GX+0)*533.333333f);
			  glTexCoord2f(5,5);
			  glVertex3f(533.333333f*32.0-(GY+1)*533.333333f,0.0,533.333333f*32.0-(GX+0)*533.333333f);
			  glTexCoord2f(5,0);
			  glVertex3f(533.333333f*32.0-(GY+1)*533.333333f,0.0,533.333333f*32.0-(GX+1)*533.333333f);
			  glTexCoord2f(0,0);
			  glVertex3f(533.333333f*32.0-(GY+0)*533.333333f,0.0,533.333333f*32.0-(GX+1)*533.333333f);
			  glEnd();
			  glEnable(GL_COLOR_MATERIAL);
			  glEnable(GL_LIGHTING);
			}
			
		  }
		}
        //l->setpos(-centerx+100,-centery,-centerz+1000);

        // navmeshdepth->BindFBO();
        // Main->clearscreen(0.5,0.5,0.5,1.0);
        //
        if (!isnan(client_x) && !isnan(client_y) && !isnan(client_z) && pid != -1)
        {
            glColor3f(1.0,0.0,0.0);
            renderSphere( client_y, client_z, client_x, 1.5, 8 );
        }
        
        
        
        
        
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D,0);
        glColor4f(1.0,1.0,1.0,1.0);
        /*m.setdiffuse(0.7,0.7,0.7,1);
        m.setambient(0.2,0.2,0.2,1);*/
        if ( wireshaderok )
        {
            wireframe->useshader();
            wireframe->setuniform3f("lightPosition",0,0,0);
            wireframe->useshader();
        }
        glEnable(GL_LIGHTING);
        //msh->RenderVBO(Main,0.0,0.0,0.0,0.0,0,0);
        int32_t nearest = 999999999;
        uint32_t mouseoverobj = 0;
        for ( int i = 0; i < hits; i++ )
        {
            uint32_t h =selection_buffer[i*4+3];
            int32_t depth = selection_buffer[i*4+1];
            if ( depth < nearest )
            {
                mouseoverobj = h;
                nearest = depth;
            }
        }
        glDepthFunc(GL_LEQUAL);
        glPolygonOffset(1.0,1.0);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glDisable(GL_LIGHTING);
        if ( mode == MODE_VIEW_POLYDETAIL )
        {
            if ( polyDetail )
            {
                glColor4f(1.0,1.0,0.0,1.0);
                polyDetail->RenderVBO(Main,0,0.0,0,0,0,0);
            }
        }else{
            for ( int i = 0; i < vmap_meshes.size(); i++)
            {
                //glLoadName(i);
                glColor4f(1.0,1.0,1.0,1.0);
                if ( hits > 0 )
                {
                    if ( i == mouseoverobj)
                    {
                        glColor4f(1.0,0.1,0.3,1.0);
                    }
                }
                
                vmap_meshes[i]->RenderVBO(Main,0,0,0,0,0,0);
                if ( !wireshaderok )
                {
                    glColor4f(0,0,0,1);
                    glDepthFunc(GL_LEQUAL);
                    
                    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
                    vmap_meshes[i]->RenderVBO(Main,0,0,0,0,0,0);
                    glDepthFunc(GL_LEQUAL);
                    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
                }
            }
        /* m.setdiffuse(0,01,0,1);
            m.setambient(0,0.2,0,1);*/
            glColor4f(0.0,0.7,0.0,1.0);
            // m.setambient(0.5,0.5,0.5,1.0);
            
            
            landmesh->RenderVBO(Main,0,0,0,0,0,0);
            if ( !wireshaderok )
            {
                glColor4f(0,0,0,1);
                glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
                landmesh->RenderVBO(Main,0,0,0,0,0,0);
                glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
            }
    
            // printf("Landmesh tris: %d\n",landmesh->indexes.size()/3);
            glColor4f(0.5,0.5,1,1.0);
            // m.setambient(0.5,0.5,0.5,1.0);
            liqmesh->Render(Main,0,0.0,0,0,0,0);
            if ( !wireshaderok )
            {
                glColor4f(0,0,0,1);
                glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
                liqmesh->Render(Main,0,0.0,0,0,0,0);
                glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
            }
            }
            glUseProgram(0);
            glBindTexture(GL_TEXTURE_2D,0);
                        glDisable(GL_TEXTURE_2D);
            glEnable (GL_BLEND);
            glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable( GL_COLOR_MATERIAL);
            glDisable(GL_LIGHTING);
        //  m.setambient(0.4,0.4,0.4,1.0);
            glBegin(GL_POLYGON);
            for ( int i = 0; i < navindexes.size(); i++ )
            {
                if ( navindexes[i] == -1 )
                {
                    glEnd();
                    glBegin(GL_POLYGON);
                } else {
                    if ( !( polyflags[i] & DT_EXT_LINK ))
                    {
                        if ( navflags[i] & NAV_GROUND )
                        glColor4f(0,1.0,0,0.5);
                        else if ( navflags[i] & NAV_WATER )
                        {
                            if ( navflags[i] & NAV_SHALLOW_WATER )
                                glColor4f(0.0,0.8,0.2,0.5);
                            if ( navflags[i] & NAV_AVERAGE_WATER )
                                glColor4f(0.0,0.5,0.5,0.5);
                            if ( navflags[i] & NAV_DEEP_WATER )
                                glColor4f(0.0,0.3,0.7,0.5);
                            if ( navflags[i] & NAV_SWIM_WATER )
                                glColor4f(0.0,0.0,1.0,0.5);
                        }
                        
                    }else{
                        glColor4f(1.0,0,0,1.0);
                    }
                    

                    //glTexCoord2f(((63-tx)*533.33f-navverts[navindexes[i]*3])/533.33f*50.0,((63-ty)*533.33f-navverts[navindexes[i]*3+1])/533.33f*50.0);
                    glVertex3f(navverts[navindexes[i]*3],navverts[navindexes[i]*3+1],navverts[navindexes[i]*3+2]);

                }



        }
        glEnd();
        glDisable(GL_BLEND);
        // navmeshdepth->UnBindFBO();
        if ( Main->getkey('k') )
        {
            TMesh * ground = new TMesh();
            ground->AddMesh(landmesh);
            ground->AddMesh(msh);
            ground->AddMesh(liqmesh);
            //ground->Render(Main,0.0,0.0,0.0,0.0,0,0);
            RegenNavmeshOnthefly(ground,NULL);
            delete ground;
        }
        if ( Main->getkey('p') )
            mode = MODE_VIEW_POLYDETAIL;
        if ( Main->getkey('l') )
            mode = MODE_VIEW_WORLD;

        //s->useshader();

        /*glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
        glEnable(GL_POLYGON_OFFSET_LINE);
        glColor3f(0.0,0.0,0.0);
        msh->Render(Main,0.0,0.0,0.0,0.0,0,0);
        landmesh->Render(Main,0,0,0,0,0,0);
        liqmesh->Render(Main,0,0,0,0,0,0);
        glDisable(GL_POLYGON_OFFSET_LINE);
        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);*/
        /*glBegin(GL_POLYGON);
        for ( int i = 0; i < navindexes.size(); i++ )
        {
          if ( navindexes[i] == -1 )
          {
            glEnd();
            glBegin(GL_POLYGON);
          }else{
            glVertex3f(navverts[navindexes[i]*3],navverts[navindexes[i]*3+1],navverts[navindexes[i]*3+2]);
          }



        }
        glEnd();*/



        //glEnable(GL_DEPTH_TEST);
        glColor3f(0.0,1.0,1.0);
        //glDisable(GL_DEPTH_TEST);
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
        glDisable(GL_LIGHTING);
        glBegin(GL_POLYGON);
        for ( int i = 0; i < navindexes.size(); i++ )
        {
            if ( navindexes[i] == -1 )
            {
                glEnd();
                glBegin(GL_POLYGON);
            } else {
                glTexCoord2f(((63-tx)*533.33f-navverts[navindexes[i]*3])/533.33f*50.0,((63-ty)*533.33f-navverts[navindexes[i]*3+1])/533.33f*50.0);
                if ( navflags[i] & NAV_GROUND )
                    glColor4f(0.0,0.7,0.7,0.5);
                else if ( navflags[i] & NAV_WATER )
                    glColor4f(0.7,0.7,0.0,0.5);
                glVertex3f(navverts[navindexes[i]*3],navverts[navindexes[i]*3+1],navverts[navindexes[i]*3+2]);

            }



        }
        glEnd();
        glEnable(GL_LIGHTING);
        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
        //glEnable(GL_DEPTH_TEST);

        //
        if ( grab )
        {
            if ( rx >= 90.0 )
            {
                if ( Main->mouserely < 0 )
                    rx += Main->mouserely/4.0;
            } else if ( rx <= -90.0  )
            {
                if ( Main->mouserely > 0 )
                    rx += Main->mouserely/4.0;
            } else {
                rx += Main->mouserely/4.0;
            }
            ry += Main->mouserelx/4.0;
        }

        glLoadIdentity();
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0.0, Main->width, 0.0, Main->height, -1.0, 1.0); 
		
		glDisable(GL_DEPTH_TEST);
		glColor4f(1,1,1,1);
		glDisable(GL_LIGHTING);
        if ( hits > 0 )
        {
            uint32_t h = mouseoverobj;
            if ( h < vmap_meshes.size() )
            {
                sprintf(txt,"Object: %s",vmap_meshes[h]->info.c_str());
                font.Render(txt,-1,FTPoint(0,60));
            }
        }
        sprintf(txt,"Camera X:%f Y:%f Z:%f Rotation: X:%f Y:%f Z:%f GX=%d GY=%d",centerx,centery,centerz,rx,ry,0.0,gx,gy);
        font.Render(txt,-1,FTPoint(0,0));
        sprintf(txt,"Poligoni: %u",polycount);
        font.Render(txt,-1,FTPoint(0,20));
        sprintf(txt,"PID del client: %d , Coordinate[ %f , %f , %f ] Camera[%f , %f , %f] PG:%s",pid,client_x,client_y,client_z,client_cam_x,client_cam_y,client_cam_z,playername_s.c_str());
        font.Render(txt,-1,FTPoint(0,40));
        Main->display();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
        lastframetime = getcurrenttime()-framestart;
    }
    return 0;
}



/*int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance, char* command_line, int show_command)
{

    int    argc;
    char** argv;

    char*  arg;
    int    index;
    int    result;

    // count the arguments
    
    argc = 1;
    arg  = command_line;
    
    while (arg[0] != 0) {

        while (arg[0] != 0 && arg[0] == ' ') {
            arg++;
        }

        if (arg[0] != 0) {
        
            argc++;
        
            while (arg[0] != 0 && arg[0] != ' ') {
                arg++;
            }
        
        }
    
    }    
    
    // tokenize the arguments

    argv = (char**)malloc(argc * sizeof(char*));

    arg = command_line;
    index = 1;

    while (arg[0] != 0) {

        while (arg[0] != 0 && arg[0] == ' ') {
            arg++;
        }

        if (arg[0] != 0) {
        
            argv[index] = arg;
            index++;
        
            while (arg[0] != 0 && arg[0] != ' ') {
                arg++;
            }
        
            if (arg[0] != 0) {
                arg[0] = 0;    
                arg++;
            }
        
        }
    
    }    

    // put the program name into argv[0]

    char filename[_MAX_PATH];
    
    GetModuleFileName(NULL, filename, _MAX_PATH);
    argv[0] = filename;

    // call the user specified main function    
    
    result = main(argc, argv);
    
    free(argv);
    return result;

}*/

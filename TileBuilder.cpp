#include <sstream>

#include "TileBuilder.h"

#include "IVMapManager.h"
#include "VMapManager2.h"
#include "MapTree.h"
#include "ModelInstance.h"
#include "G3D/Array.h"

using namespace VMAP;
using namespace MaNGOS;

// see following files:
// contrib/extractor/system.cpp
// src/game/GridMap.cpp
char const* MAP_VERSION_MAGIC = "v1.3";
namespace MMAP
{
TileBuilder::TileBuilder(bool hiRes) :
    m_hiResHeightMaps   (hiRes)
{
}

TileBuilder::~TileBuilder()
{
}

void TileBuilder::getLoopVars(Spot portion, int &loopStart, int &loopEnd, int &loopInc)
{
    switch(portion)
    {
    case ENTIRE:
        loopStart = 0;
        loopEnd = V8_SIZE_SQ;
        loopInc = 1;
        break;
    case TOP:
        loopStart = 0;
        loopEnd = V8_SIZE;
        loopInc = 1;
        break;
    case LEFT:
        loopStart = 0;
        loopEnd = V8_SIZE_SQ - V8_SIZE + 1;
        loopInc = V8_SIZE;
        break;
    case RIGHT:
        loopStart = V8_SIZE - 1;
        loopEnd = V8_SIZE_SQ;
        loopInc = V8_SIZE;
        break;
    case BOTTOM:
        loopStart = V8_SIZE_SQ - V8_SIZE;
        loopEnd = V8_SIZE_SQ;
        loopInc = 1;
        break;
    }
}

void TileBuilder::loadMap(uint32 mapID, uint32 tileX, uint32 tileY, MeshData &meshData)
{
    if(loadMap(mapID, tileX, tileY, meshData, ENTIRE))
    {
        loadMap(mapID, tileX+1, tileY, meshData, LEFT);
        loadMap(mapID, tileX-1, tileY, meshData, RIGHT);
        loadMap(mapID, tileX, tileY+1, meshData, TOP);
        loadMap(mapID, tileX, tileY-1, meshData, BOTTOM);
    }
}


void TileBuilder::cleanVertices(G3D::Array<float> &verts, G3D::Array<int> &tris)
{
    map<int, int> vertMap;

    int* t = tris.getCArray();
    float* v = verts.getCArray();

    // collect all the vertex indices from triangle
    for (int i = 0; i < tris.size(); ++i)
    {
        if (vertMap.find(t[i]) != vertMap.end())
            continue;

        vertMap.insert(std::pair<int, int>(t[i], 0));
    }

    // collect the vertices
    G3D::Array<float> cleanVerts;
    int index, count = 0;
    for (map<int, int>::iterator it = vertMap.begin(); it != vertMap.end(); ++it)
    {
        index = (*it).first;
        (*it).second = count;
        cleanVerts.append(v[index*3], v[index*3+1], v[index*3+2]);
        count++;
    }
    verts.fastClear();
    verts.append(cleanVerts);
    cleanVerts.clear();

    // update triangles to use new indices
    for (int i = 0; i < tris.size(); ++i)
    {
        map<int, int>::iterator it;
        if ((it = vertMap.find(t[i])) == vertMap.end())
            continue;

        t[i] = (*it).second;
    }

    vertMap.clear();
}

bool TileBuilder::loadMap(uint32 mapID, uint32 tileX, uint32 tileY, MeshData &meshData, Spot portion)
{
    m_skipLiquid = false;
    char mapFileName[255];
    char holeFileName[255];
    uint16 holes[16][16];
    sprintf(mapFileName, "maps/%03u%02u%02u.map", mapID, tileY, tileX);
    sprintf(holeFileName, "maps/%03u%02u%02u.hole", mapID, tileY, tileX);
    fprintf(stderr,"\033[1m\033[37mLoading map %s...\033[0m\n",mapFileName);
    FILE* mapFile = fopen(mapFileName, "rb");
    if(!mapFile)
        return true;


    GridMapFileHeader fheader;
    fread(&fheader, sizeof(GridMapFileHeader), 1, mapFile);

    if(fheader.versionMagic != *((uint32 const*)(MAP_VERSION_MAGIC)))
    {
        fclose(mapFile);
        printf("%s is the wrong version, it may cause problems\n", mapFileName);
        return false;
    }
    FILE* holeFile = fopen(holeFileName,"rb");
    if (holeFile)
    {
        fread(holes,sizeof(uint16)*16*16,1,holeFile);
        fclose(holeFile);
    }
    else
    {
        fseek(mapFile,fheader.holesOffset,SEEK_SET);
        fread(holes,fheader.holesSize,1,mapFile);
    }
    GridMapHeightHeader hheader;
    fseek(mapFile, fheader.heightMapOffset, SEEK_SET);
    fread(&hheader, sizeof(GridMapHeightHeader), 1, mapFile);

    bool haveTerrain = !(hheader.flags & MAP_HEIGHT_NO_HEIGHT);
    bool haveLiquid = fheader.liquidMapOffset && !m_skipLiquid;

    // no data in this map file
    if (!haveTerrain && !haveLiquid)
    {
        fclose(mapFile);
        return false;
    }

    // data used later
    uint16 liquid_type[16][16];
    memset(liquid_type, 0, sizeof(liquid_type));
    G3D::Array<int> ltriangles;
    G3D::Array<int> ttriangles;

    // terrain data
    if (haveTerrain)
    {
        int i;
        float heightMultiplier;
        float V9[V9_SIZE_SQ], V8[V8_SIZE_SQ];

        if (hheader.flags & MAP_HEIGHT_AS_INT8)
        {
            uint8 v9[V9_SIZE_SQ];
            uint8 v8[V8_SIZE_SQ];
            fread(v9, sizeof(uint8), V9_SIZE_SQ, mapFile);
            fread(v8, sizeof(uint8), V8_SIZE_SQ, mapFile);
            heightMultiplier = (hheader.gridMaxHeight - hheader.gridHeight) / 255;

            for (i = 0; i < V9_SIZE_SQ; ++i)
                V9[i] = (float)v9[i]*heightMultiplier + hheader.gridHeight;

            for (i = 0; i < V8_SIZE_SQ; ++i)
                V8[i] = (float)v8[i]*heightMultiplier + hheader.gridHeight;
        }
        else if (hheader.flags & MAP_HEIGHT_AS_INT16)
        {
            uint16 v9[V9_SIZE_SQ];
            uint16 v8[V8_SIZE_SQ];
            fread(v9, sizeof(uint16), V9_SIZE_SQ, mapFile);
            fread(v8, sizeof(uint16), V8_SIZE_SQ, mapFile);
            heightMultiplier = (hheader.gridMaxHeight - hheader.gridHeight) / 65535;

            for (i = 0; i < V9_SIZE_SQ; ++i)
                V9[i] = (float)v9[i]*heightMultiplier + hheader.gridHeight;

            for (i = 0; i < V8_SIZE_SQ; ++i)
                V8[i] = (float)v8[i]*heightMultiplier + hheader.gridHeight;
        }
        else
        {
            fread (V9, sizeof(float), V9_SIZE_SQ, mapFile);
            fread(V8, sizeof(float), V8_SIZE_SQ, mapFile);
        }

        int count = meshData.solidVerts.size() / 3;
        float xoffset = (float(tileX)-32)*GRID_SIZE;
        float yoffset = (float(tileY)-32)*GRID_SIZE;

        float coord[3];

        for (i = 0; i < V9_SIZE_SQ; ++i)
        {
            getHeightCoord(i, GRID_V9, xoffset, yoffset, coord, V9);
            meshData.solidVerts.append(coord[0]);
            meshData.solidVerts.append(coord[2]);
            meshData.solidVerts.append(coord[1]);
        }

        for (i = 0; i < V8_SIZE_SQ; ++i)
        {
            getHeightCoord(i, GRID_V8, xoffset, yoffset, coord, V8);
            meshData.solidVerts.append(coord[0]);
            meshData.solidVerts.append(coord[2]);
            meshData.solidVerts.append(coord[1]);
        }

        int j, indices[3], loopStart, loopEnd, loopInc;
        getLoopVars(portion, loopStart, loopEnd, loopInc);
        for (i = loopStart; i < loopEnd; i+=loopInc)
            for (j = TOP; j <= BOTTOM; j+=1)
            {
                getHeightTriangle(i, Spot(j), indices);
                ttriangles.append(indices[2] + count);
                ttriangles.append(indices[1] + count);
                ttriangles.append(indices[0] + count);
            }
    }

    // liquid data
    if (haveLiquid)
    {
        GridMapLiquidHeader lheader;
        fseek(mapFile, fheader.liquidMapOffset, SEEK_SET);
        fread(&lheader, sizeof(GridMapLiquidHeader), 1, mapFile);

        float* liquid_map = NULL;

        if (!(lheader.flags & MAP_LIQUID_NO_TYPE))
            fread(liquid_type, sizeof(liquid_type), 1, mapFile);

        if (!(lheader.flags & MAP_LIQUID_NO_HEIGHT))
        {

            liquid_map = new float [lheader.width*lheader.height];
            fread(liquid_map, sizeof(float), lheader.width*lheader.height, mapFile);
        }
        printf("C! %u %u %f\n",liquid_type , liquid_map,lheader.liquidLevel);
        if (!liquid_map )
        {
            int count = meshData.liquidVerts.size() / 3;
            float xoffset = (float(tileX)-32)*GRID_SIZE;
            float yoffset = (float(tileY)-32)*GRID_SIZE;

            float coord[3];
            int row, col;
            int j = 0;
            for (int i = 0; i < V9_SIZE_SQ; ++i)
            {
                row = i / V9_SIZE;
                col = i % V9_SIZE;
                meshData.liquidVerts.append((xoffset+col*GRID_PART_SIZE)*-1, lheader.liquidLevel, (yoffset+row*GRID_PART_SIZE)*-1);
            }
            int indices[3], loopStart, loopEnd, loopInc, triInc;
            getLoopVars(portion, loopStart, loopEnd, loopInc);
            triInc = BOTTOM-TOP;

            // generate triangles
            for (int i = loopStart; i < loopEnd; i+=loopInc)
                for (int j = TOP; j <= BOTTOM; j+= triInc)
                {
                    getHeightTriangle(i, Spot(j), indices, true);
                    ltriangles.append(indices[2] + count);
                    ltriangles.append(indices[1] + count);
                    ltriangles.append(indices[0] + count);
                }

        }
        if (liquid_type && liquid_map)
        {
            int count = meshData.liquidVerts.size() / 3;
            float xoffset = (float(tileX)-32)*GRID_SIZE;
            float yoffset = (float(tileY)-32)*GRID_SIZE;

            float coord[3];
            int row, col;

            // generate coordinates
            if (!(lheader.flags & MAP_LIQUID_NO_HEIGHT))
            {
                int j = 0;
                for (int i = 0; i < V9_SIZE_SQ; ++i)
                {
                    row = i / V9_SIZE;
                    col = i % V9_SIZE;

                    if (row < lheader.offsetY || row >= lheader.offsetY + lheader.height ||
                            col < lheader.offsetX || col >= lheader.offsetX + lheader.width)
                    {
                        // dummy vert using invalid height
                        meshData.liquidVerts.append((xoffset+col*GRID_PART_SIZE)*-1, INVALID_MAP_LIQ_HEIGHT, (yoffset+row*GRID_PART_SIZE)*-1);
                        continue;
                    }

                    getLiquidCoord(i, j, xoffset, yoffset, coord, liquid_map);
                    meshData.liquidVerts.append(coord[0]);
                    meshData.liquidVerts.append(coord[2]);
                    meshData.liquidVerts.append(coord[1]);
                    j++;
                }
            }
            else
            {
                for (int i = 0; i < V9_SIZE_SQ; ++i)
                {
                    row = i / V9_SIZE;
                    col = i % V9_SIZE;
                    meshData.liquidVerts.append((xoffset+col*GRID_PART_SIZE)*-1, lheader.liquidLevel, (yoffset+row*GRID_PART_SIZE)*-1);
                }
            }

            delete [] liquid_map;

            int indices[3], loopStart, loopEnd, loopInc, triInc;
            getLoopVars(portion, loopStart, loopEnd, loopInc);
            triInc = BOTTOM-TOP;

            // generate triangles
            for (int i = loopStart; i < loopEnd; i+=loopInc)
                for (int j = TOP; j <= BOTTOM; j+= triInc)
                {
                    getHeightTriangle(i, Spot(j), indices, true);
                    ltriangles.append(indices[2] + count);
                    ltriangles.append(indices[1] + count);
                    ltriangles.append(indices[0] + count);
                }
        }
    }

    fclose(mapFile);

    // now that we have gathered the data, we can figure out which parts to keep:
    // liquid above ground, ground above liquid
    int loopStart, loopEnd, loopInc, tTriCount = 4;
    bool useTerrain, useLiquid;

    float* lverts = meshData.liquidVerts.getCArray();
    int* ltris = ltriangles.getCArray();

    float* tverts = meshData.solidVerts.getCArray();
    int* ttris = ttriangles.getCArray();

    if (ltriangles.size() + ttriangles.size() == 0)
        return false;

    // make a copy of liquid vertices
    // used to pad right-bottom frame due to lost vertex data at extraction
    float* lverts_copy = NULL;
    if(meshData.liquidVerts.size())
    {
        lverts_copy = new float[meshData.liquidVerts.size()];
        memcpy(lverts_copy, lverts, sizeof(float)*meshData.liquidVerts.size());
    }

    getLoopVars(portion, loopStart, loopEnd, loopInc);
    for (int i = loopStart; i < loopEnd; i+=loopInc)
    {
        for (int j = 0; j < 2; ++j)
        {
            // default is true, will change to false if needed
            useTerrain = true;
            useLiquid = true;
            uint8 liquidType = MAP_LIQUID_TYPE_NO_WATER;

            // if there is no liquid, don't use liquid
            if (!liquid_type || !meshData.liquidVerts.size() || !ltriangles.size())
            {
                // printf("%u %u %u\n",!liquid_type , !meshData.liquidVerts.size() , !ltriangles.size());
                useLiquid = false;
            }
            else
            {
                liquidType = getLiquidType(i, liquid_type);
                switch (liquidType)
                {
                default:
                    useLiquid = false;
                    break;
                case MAP_LIQUID_TYPE_WATER:
                case MAP_LIQUID_TYPE_OCEAN:
                    // merge different types of water
                    liquidType = NAV_WATER;
                    break;
                case MAP_LIQUID_TYPE_MAGMA:
                    liquidType = NAV_MAGMA;
                    break;
                case MAP_LIQUID_TYPE_SLIME:
                    liquidType = NAV_SLIME;
                    break;
                case MAP_LIQUID_TYPE_DARK_WATER:
                    // players should not be here, so logically neither should creatures
                    //printf("Darkwater\n");
                    useTerrain = false;
                    useLiquid = false;
                    break;
                case 0x78:
                    useLiquid = false;
                    printf("Unkliquid %x\n",liquidType);
                    /* case MAP_LIQUID_TYPE_NO_WATER:
                         useLiquid = false;*/
                }
            }

            // if there is no terrain, don't use terrain
            if (!ttriangles.size())
                useTerrain = false;

            // while extracting ADT data we are losing right-bottom vertices
            // this code adds fair approximation of lost data
            if (useLiquid)
            {
                float quadHeight = 0;
                uint32 validCount = 0;
                for(uint32 idx = 0; idx < 3; idx++)
                {
                    float h = lverts_copy[ltris[idx]*3 + 1];
                    if(h != INVALID_MAP_LIQ_HEIGHT && h < INVALID_MAP_LIQ_HEIGHT_MAX)
                    {
                        quadHeight += h;
                        validCount++;
                    }
                }

                // update vertex height data
                if(validCount > 0 && validCount < 3)
                {
                    quadHeight /= validCount;
                    for(uint32 idx = 0; idx < 3; idx++)
                    {
                        float h = lverts[ltris[idx]*3 + 1];
                        if(h == INVALID_MAP_LIQ_HEIGHT || h > INVALID_MAP_LIQ_HEIGHT_MAX)
                            lverts[ltris[idx]*3 + 1] = quadHeight;
                    }
                }

                // no valid vertexes - don't use this poly at all
                if(validCount == 0)
                {
                    //printf("Invalid liquid vert!");
                    useLiquid = false;
                }
            }

            // if there is a hole here, don't use the terrain
            if (useTerrain)
                useTerrain = !isHole(i, holes);

            // we use only one terrain kind per quad - pick higher one
            if (useTerrain && useLiquid)
            {
                float minLLevel = INVALID_MAP_LIQ_HEIGHT_MAX;
                float maxLLevel = INVALID_MAP_LIQ_HEIGHT;
                for(uint32 x = 0; x < 3; x++)
                {
                    float h = lverts[ltris[x]*3 + 1];
                    if(minLLevel > h)
                        minLLevel = h;

                    if(maxLLevel < h)
                        maxLLevel = h;
                }

                float maxTLevel = INVALID_MAP_LIQ_HEIGHT;
                float minTLevel = INVALID_MAP_LIQ_HEIGHT_MAX;
                for(uint32 x = 0; x < 6; x++)
                {
                    float h = tverts[ttris[x]*3 + 1];
                    if(maxTLevel < h)
                        maxTLevel = h;

                    if(minTLevel > h)
                        minTLevel = h;
                }
                //printf("minLLevel=%f minTLevel=%f\n",minLLevel,minTLevel);
                // terrain under the liquid?
                if(minLLevel > maxTLevel)
                    useTerrain = false;

                //liquid under the terrain?

                if(minTLevel > maxLLevel)
                    useLiquid = false;
            }

            // store the result
            if (useLiquid)
            {
                meshData.liquidType.append(liquidType);
                for (int k = 0; k < 3; ++k)
                    meshData.liquidTris.append(ltris[k]);
            }

            if (useTerrain)
                for (int k = 0; k < 3*tTriCount/2; ++k)
                    meshData.solidTris.append(ttris[k]);

            // advance to next set of triangles
            ltris += 3;
            ttris += 3*tTriCount/2;
        }
    }

    if(lverts_copy)
        delete [] lverts_copy;
    printf("===========LOADED %u %u=============\n",meshData.solidTris.size(), meshData.liquidTris.size());
    return meshData.solidTris.size() || meshData.liquidTris.size();
}

inline void TileBuilder::getHeightCoord(int index, Grid grid, float xOffset, float yOffset, float* coord, float* v)
{
    // wow coords: x, y, height
    // coord is mirroed about the horizontal axes
    switch(grid)
    {
    case GRID_V9:
        coord[0] = (xOffset + index%(V9_SIZE)*GRID_PART_SIZE) * -1.f;
        coord[1] = (yOffset + (int)(index/(V9_SIZE))*GRID_PART_SIZE) * -1.f;
        coord[2] = v[index];
        break;
    case GRID_V8:
        coord[0] = (xOffset + index%(V8_SIZE)*GRID_PART_SIZE + GRID_PART_SIZE/2.f) * -1.f;
        coord[1] = (yOffset + (int)(index/(V8_SIZE))*GRID_PART_SIZE + GRID_PART_SIZE/2.f) * -1.f;
        coord[2] = v[index];
        break;
    }
}

inline void TileBuilder::getHeightTriangle(int square, Spot triangle, int* indices, bool liquid/* = false*/)
{
    int rowOffset = square/V8_SIZE;
    if(m_hiResHeightMaps && !liquid)
        switch(triangle)
        {
        case TOP:
            indices[0] = square+rowOffset;                  //           0-----1 .... 128
            indices[1] = square+1+rowOffset;                //           |\ T /|
            indices[2] = (V9_SIZE_SQ)+square;               //           | \ / |
            break;                                          //           |L 0 R| .. 127
        case LEFT:                                          //           | / \ |
            indices[0] = square+rowOffset;                  //           |/ B \|
            indices[1] = (V9_SIZE_SQ)+square;               //          129---130 ... 386
            indices[2] = square+V9_SIZE+rowOffset;          //           |\   /|
            break;                                          //           | \ / |
        case RIGHT:                                         //           | 128 | .. 255
            indices[0] = square+1+rowOffset;                //           | / \ |
            indices[1] = square+V9_SIZE+1+rowOffset;        //           |/   \|
            indices[2] = (V9_SIZE_SQ)+square;               //          258---259 ... 515
            break;
        case BOTTOM:
            indices[0] = (V9_SIZE_SQ)+square;
            indices[1] = square+V9_SIZE+1+rowOffset;
            indices[2] = square+V9_SIZE+rowOffset;
            break;
        }
    else
        switch(triangle)
        {   //           0-----1 .... 128
        case TOP:                                               //           |\    |
            indices[0] = square+rowOffset;                      //           | \ T |
            indices[1] = square+1+rowOffset;                    //           |  \  |
            indices[2] = square+V9_SIZE+1+rowOffset;            //           | B \ |
            break;                                              //           |    \|
        case BOTTOM:                                            //          129---130 ... 386
            indices[0] = square+rowOffset;                      //           |\    |
            indices[1] = square+V9_SIZE+1+rowOffset;            //           | \   |
            indices[2] = square+V9_SIZE+rowOffset;              //           |  \  |
            break;                                              //           |   \ |
        }                                                           //           |    \|
    //          258---259 ... 515
}

inline void TileBuilder::getLiquidCoord(int index, int index2, float xOffset, float yOffset, float* coord, float* v)
{
    // wow coords: x, y, height
    // coord is mirroed about the horizontal axes
    coord[0] = (xOffset + index%(V9_SIZE)*GRID_PART_SIZE) * -1.f;
    coord[1] = (yOffset + (int)(index/(V9_SIZE))*GRID_PART_SIZE) * -1.f;
    coord[2] = v[index2];
}


inline void TileBuilder::getLiquidTriangle(int square, Spot triangle, int* indices, uint8 width)
{
    int rowOffset = square/V8_SIZE;
    switch(triangle)
    {
    case TOP:
        indices[0] = square+rowOffset;
        indices[1] = square+1+rowOffset;
        indices[2] = square+V9_SIZE+1+rowOffset;
        break;
    case BOTTOM:
        indices[0] = square+rowOffset;
        indices[1] = square+V9_SIZE+1+rowOffset;
        indices[2] = square+V9_SIZE+rowOffset;
        break;
    }
}

static uint16 holetab_h[4] = {0x1111, 0x2222, 0x4444, 0x8888};
static uint16 holetab_v[4] = {0x000F, 0x00F0, 0x0F00, 0xF000};

bool TileBuilder::isHole(int square, const uint16 holes[16][16])
{
    int row = square / 128;
    int col = square % 128;
    int cellRow = row / 8;     // 8 squares per cell
    int cellCol = col / 8;
    int holeRow = row % 8 / 2;
    int holeCol = (square - (row * 128 + cellCol * 8)) / 2;

    uint16 hole = holes[cellRow][cellCol];

    bool isHole = (hole & holetab_h[holeCol] & holetab_v[holeRow]) != 0;

    return isHole;
}

uint8 TileBuilder::getLiquidType(int square, const uint16 liquid_type[16][16])
{
    int row = square / 128;
    int col = square % 128;
    int cellRow = row / 8;     // 8 squares per cell
    int cellCol = col / 8;

    return liquid_type[cellRow][cellCol];
}
}

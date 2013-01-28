
########### next target ###############

SET(detour_STAT_SRCS
   DetourCommon.cpp
   DetourCommon.h
   DetourAssert.h
   DetourNavMeshBuilder.cpp
   DetourNavMeshBuilder.h
   DetourNavMesh.cpp
   DetourNavMesh.h
   DetourNode.cpp
   DetourNode.h
   DetourNavMeshQuery.cpp
   DetourNavMeshQuery.h
   DetourAlloc.cpp
   DetourAlloc.h
)

add_library(detour STATIC ${detour_STAT_SRCS})

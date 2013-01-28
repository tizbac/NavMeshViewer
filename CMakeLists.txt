
cmake_minimum_required(VERSION 2.8)
project ( generator )
set(Boost_USE_MULTITHREADED ON) 

add_subdirectory( Detour )
add_subdirectory( Recast )

#add_subdirectory ( acelite )
link_directories( . )
#find_package(Boost 1.45.0 COMPONENTS thread REQUIRED)
find_package(ZLIB REQUIRED )
find_package(Freetype REQUIRED )
include_directories ( ${ZLIB_INCLUDE_DIRS}  ${FREETYPE_INCLUDE_DIRS} . ./Models ./Maps ./vmap/Management ./Dynamic ./Dynamic/LinkedReference ./vmap /usr/include/SDL ./Base ./Base/ftgl ../../../src/framework ./irrlicht ../../../src/shared/vmap ./Debug . ./Detour ./Recast ../../../dep/include/g3dlite ./DebugUtils ../../../src/shared/)

SET(commonsources ChunkyTriMesh.cpp  TileBuilder.cpp
DebugAlloc.cpp     MeshLoaderObj.cpp  VMapExtensions.cpp 


Models/ModelInstance.cpp
Models/ModelInstance.h
Models/WorldModel.cpp
Models/WorldModel.h
Maps/MapTree.cpp
Maps/MapTree.h
Maps/TileAssembler.cpp
Maps/TileAssembler.h
./vmap/BoundingIntervalHierarchy.cpp
./vmap/BoundingIntervalHierarchy.h

vmap/Management/IVMapManager.h
vmap/Management/VMapFactory.cpp
vmap/Management/VMapFactory.h
vmap/Management/VMapManager2.cpp
vmap/Management/VMapManager2.h
vmap/VMapDefinitions.h
vmap/VMapTools.h

Dynamic/FactoryHolder.h
Dynamic/LinkedList.h
Dynamic/LinkedReference/Reference.h
Dynamic/LinkedReference/RefManager.h
Dynamic/ObjectRegistry.h
Dynamic/TypeContainerFunctions.h
Dynamic/TypeContainerFunctionsPtr.h
Dynamic/TypeContainer.h
Dynamic/TypeContainerVisitor.h
Dynamic/TypeList.h
Dynamic/UnorderedMap.h





./G3D/AABox.cpp
./G3D/AABox.h
./G3D/Any.cpp
./G3D/Any.h
./G3D/AnyVal.h
./G3D/AreaMemoryManager.h
./G3D/Array.h
./G3D/AtomicInt32.h
./G3D/BinaryFormat.cpp
./G3D/BinaryFormat.h
./G3D/BinaryInput.cpp
./G3D/BinaryInput.h
./G3D/BinaryOutput.cpp
./G3D/BinaryOutput.h
./G3D/BoundsTrait.h
./G3D/Box2D.h
./G3D/Box.cpp
./G3D/Box.h
./G3D/BumpMapPreprocess.h
./G3D/Capsule.cpp
./G3D/Capsule.h
./G3D/CMakeLists.txt
./G3D/CollisionDetection.cpp
./G3D/CollisionDetection.h
./G3D/Color1.h
./G3D/Color1uint8.h
./G3D/Color3.h
./G3D/Color3uint8.h
./G3D/Color4.h
./G3D/Color4uint8.h
./G3D/Cone.h
./G3D/constants.h
./G3D/ConvexPolyhedron.h
./G3D/CoordinateFrame.cpp
./G3D/CoordinateFrame.h
./G3D/Crypto.cpp
./G3D/Crypto.h
./G3D/Cylinder.cpp
./G3D/Cylinder.h
./G3D/debugAssert.cpp
./G3D/debugAssert.h
./G3D/debug.h
./G3D/debugPrintf.h
./G3D/enumclass.h
./G3D/EqualsTrait.h
./G3D/fileutils.cpp
./G3D/fileutils.h
./G3D/filter.h
./G3D/format.cpp
./G3D/format.h
./G3D/G3DAll.h
./G3D/g3dfnmatch.cpp
./G3D/g3dfnmatch.h
./G3D/G3DGameUnits.h
./G3D/G3D.h
./G3D/g3dmath.cpp
./G3D/g3dmath.h
./G3D/g3dmath.inl
./G3D/GCamera.h
./G3D/GImage.h
./G3D/GLight.h
./G3D/GMutex.h
./G3D/GThread.h
./G3D/GUniqueID.h
./G3D/HashTrait.h
./G3D/Image1.h
./G3D/Image1uint8.h
./G3D/Image3.h
./G3D/Image3uint8.h
./G3D/Image4.h
./G3D/Image4uint8.h
./G3D/ImageFormat.h
./G3D/Intersect.h
./G3D/KDTree.h
./G3D/Line.cpp
./G3D/Line.h
./G3D/LineSegment.cpp
./G3D/LineSegment.h
./G3D/Log.cpp
./G3D/Log.h
./G3D/Map2D.h
./G3D/Matrix2.h
./G3D/Matrix3.cpp
./G3D/Matrix3.h
./G3D/Matrix4.cpp
./G3D/Matrix4.h
./G3D/Matrix.h
./G3D/MemoryManager.cpp
./G3D/MemoryManager.h
./G3D/MeshAlg.h
./G3D/MeshBuilder.h
./G3D/NetAddress.h
./G3D/NetworkDevice.h
./G3D/ParseError.h
./G3D/PhysicsFrame.h
./G3D/Plane.cpp
./G3D/Plane.h
./G3D/platform.h
./G3D/Pointer.h
./G3D/PointHashGrid.h
./G3D/PointKDTree.h
./G3D/PositionTrait.h
./G3D/PrecomputedRandom.h
./G3D/prompt.cpp
./G3D/prompt.h
./G3D/Quat.cpp
./G3D/Quat.h
./G3D/Quat.inl
./G3D/Queue.h
./G3D/Random.cpp
./G3D/Random.h
./G3D/Ray.cpp
./G3D/Ray.h
./G3D/Rect2D.h
./G3D/ReferenceCount.cpp
./G3D/ReferenceCount.h
./G3D/RegistryUtil.cpp
./G3D/RegistryUtil.h
./G3D/serialize.h
./G3D/Set.h
./G3D/SmallArray.h
./G3D/Sphere.cpp
./G3D/Sphere.h
./G3D/splinefunc.h
./G3D/Spline.h
./G3D/Stopwatch.h
./G3D/stringutils.cpp
./G3D/stringutils.h
./G3D/System.cpp
./G3D/System.h
./G3D/Table.h
./G3D/TextInput.cpp
./G3D/TextInput.h
./G3D/TextOutput.cpp
./G3D/TextOutput.h
./G3D/ThreadSet.h
./G3D/Triangle.cpp
./G3D/Triangle.h
./G3D/uint128.h
./G3D/units.h
./G3D/UprightFrame.cpp
./G3D/UprightFrame.h
./G3D/Vector2.cpp
./G3D/Vector2.h
./G3D/Vector2.inl
./G3D/Vector2int16.h
./G3D/Vector3.cpp
./G3D/Vector3.h
./G3D/Vector3.inl
./G3D/Vector3int16.h
./G3D/Vector3int32.h
./G3D/Vector4.cpp
./G3D/Vector4.h
./G3D/Vector4.inl
./G3D/Vector4int8.h
./G3D/vectorMath.h
./G3D/WeakCache.h
./G3D/Welder.h
./G3D/WrapMode.h

#./DebugUtils/DebugDraw.cpp
#./DebugUtils/DebugDraw.h
#./DebugUtils/DetourDebugDraw.cpp
#./DebugUtils/DetourDebugDraw.h
#./DebugUtils/RecastDebugDraw.cpp
#./DebugUtils/RecastDebugDraw.h
#./DebugUtils/RecastDump.cpp
#./DebugUtils/RecastDump.h
#./Debug/Debug.cpp 


)

#add_executable( generator ${commonsources} generator.cpp MapBuilder.cpp Base/TMesh.cpp)

add_subdirectory( ./Base )
#add_subdirectory ( ./RecastDemo )

add_executable( viewer ${commonsources} viewer.cpp )
#SET_TARGET_PROPERTIES( generator PROPERTIES COMPILE_FLAGS -DHEADLESS )
#target_link_libraries ( generator detour recast ${ZLIB_LIBRARIES} ${FREETYPE_LIBRARIES}) 
if ( MINGW )
 target_link_libraries ( viewer mingw32 SDLmain SDL  detour recast ftglS glew32 glu32 opengl32                                                                                                                                                                                                                 
 ${CMAKE_THREAD_LIBS_INIT} gl3dengine ${Boost_LIBRARIES} ${ZLIB_LIBRARIES} ${FREETYPE_LIBRARIES})
 

else ( MINGW )
target_link_libraries ( viewer  GLEW GL GLU  SDL  detour recast ftglS                                                                                                                                                                                                                    
 ${CMAKE_THREAD_LIBS_INIT} gl3dengine ${Boost_LIBRARIES} ${ZLIB_LIBRARIES} ${FREETYPE_LIBRARIES})
endif (MINGW )
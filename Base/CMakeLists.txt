project ( gl3dengine )
subdirs ( ftgl )

include_directories( ./LOD ../boost )
add_library ( gl3dengine SHARED TMesh.cpp engine.cxx Thread.cpp  fbo.cc  light.cxx  matset.cxx  shader.cxx  texture.cxx  wavefront.cxx ./LOD/LOD_DecimationClass.h
./LOD/LOD_decimation.cpp
./LOD/LOD_decimation.h
./LOD/LOD_EdgeCollapser.cpp
./LOD/LOD_EdgeCollapser.h
./LOD/LOD_ExternBufferEditor.h
./LOD/LOD_ExternNormalEditor.cpp
./LOD/LOD_ExternNormalEditor.h
./LOD/LOD_FaceNormalEditor.cpp
./LOD/LOD_FaceNormalEditor.h
./LOD/LOD_ManMesh2.cpp
./LOD/LOD_ManMesh2.h
./LOD/LOD_MeshBounds.h
./LOD/LOD_MeshException.h
./LOD/LOD_MeshPrimitives.cpp
./LOD/LOD_MeshPrimitives.h
./LOD/LOD_QSDecimator.cpp
./LOD/LOD_QSDecimator.h
./LOD/LOD_QuadricEditor.cpp
./LOD/LOD_QuadricEditor.h
./LOD/LOD_Quadric.h 
./LOD/MT_Matrix3x3.cpp
./LOD/MT_Matrix4x4.inl
./LOD/MT_Plane3.inl
./LOD/MT_Point2.inl
./LOD/MT_Point3.inl
./LOD/MT_Quaternion.inl
./LOD/MT_Vector2.inl
./LOD/MT_Vector3.cpp
./LOD/MT_Vector4.inl
)
if ( MINGW )
target_link_libraries ( gl3dengine glew32 opengl32 glu32 SDL)
else ( MINGW )

endif ( MINGW )
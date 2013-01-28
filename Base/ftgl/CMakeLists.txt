cmake_minimum_required(VERSION 2.6)

project (ftgl)

IF (NOT MINGW)

ENDIF (NOT MINGW)
INCLUDE_DIRECTORIES(. ./freetype/include)
add_library ( ftglS SHARED ./FTBuffer.cpp                                                         
./FTBuffer.cpp                                                         
./FTCharmap.cpp                                                        
./FTContour.cpp                                                        
./FTFace.cpp                                                           
./FTFont/FTBitmapFont.cpp                                              
./FTFont/FTBufferFont.cpp                                              
./FTFont/FTExtrudeFont.cpp                                             
./FTFont/FTFont.cpp                                                    
./FTFont/FTFontGlue.cpp                                                
./FTFont/FTOutlineFont.cpp                                             
./FTFont/FTPixmapFont.cpp                                              
./FTFont/FTPolygonFont.cpp                                             
./FTFont/FTTextureFont.cpp                                             
./FTGlyph/FTBitmapGlyph.cpp                                            
./FTGlyph/FTBufferGlyph.cpp                                            
./FTGlyph/FTExtrudeGlyph.cpp                                           
./FTGlyph/FTGlyph.cpp                                                  
./FTGlyph/FTGlyphGlue.cpp                                              
./FTGlyph/FTOutlineGlyph.cpp                                           
./FTGlyph/FTPixmapGlyph.cpp
./FTGlyph/FTPolygonGlyph.cpp
./FTGlyph/FTTextureGlyph.cpp
./FTGlyphContainer.cpp
./FTLayout/FTLayout.cpp
./FTLayout/FTLayoutGlue.cpp
./FTLayout/FTSimpleLayout.cpp
./FTLibrary.cpp
./FTPoint.cpp
./FTSize.cpp
./FTVectoriser.cpp

)
if ( MINGW )
target_link_libraries ( ftglS ${FREETYPE_LIBRARIES} glew32 opengl32 glu32 )
else ( MINGW )
target_link_libraries ( ftglS ${FREETYPE_LIBRARIES} )
endif( MINGW)
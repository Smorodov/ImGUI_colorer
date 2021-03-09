# ----------------------------------------------------------------------------
# 
# ----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.2)

message(STATUS "CMAKE_BINARY_DIR = ${CMAKE_BINARY_DIR}")
set (build_dir ${CMAKE_BINARY_DIR})

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}) 
set(CMAKE_C_FLAGS "/EHsc")
set(CMAKE_CXX_FLAGS "/EHsc")

add_subdirectory(glfw)
 
add_definitions(-DUNICODE -D_UNICODE)
# set(CMAKE_DEBUG_POSTFIX _d)
 
project(ImGUI_colorer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


################################################################################
# Source groups
################################################################################
set(Header_Files
    "shared/colorer/_template.h"
    "shared/colorer/editor/BaseEditor.h"
    "shared/colorer/editor/OutlineItem.h"
    "shared/colorer/editor/Outliner.h"
    "shared/colorer/editor/PairMatch.h"
    "shared/colorer/ErrorHandler.h"
    "shared/colorer/FileType.h"
    "shared/colorer/handlers/DefaultErrorHandler.h"
    "shared/colorer/handlers/ErrorHandlerWriter.h"
    "shared/colorer/handlers/FileErrorHandler.h"
    "shared/colorer/handlers/LineRegion.h"
    "shared/colorer/handlers/LineRegionsCompactSupport.h"
    "shared/colorer/handlers/LineRegionsSupport.h"
    "shared/colorer/handlers/RegionDefine.h"
    "shared/colorer/handlers/RegionMapper.h"
    "shared/colorer/handlers/RegionMapperImpl.h"
    "shared/colorer/handlers/StyledHRDMapper.h"
    "shared/colorer/handlers/StyledRegion.h"
    "shared/colorer/handlers/TextHRDMapper.h"
    "shared/colorer/handlers/TextRegion.h"
    "shared/colorer/HRCParser.h"
    "shared/colorer/LineSource.h"
    "shared/colorer/ParserFactory.h"
    "shared/colorer/ParserFactoryException.h"
    "shared/colorer/parsers/helpers/FileTypeImpl.h"
    "shared/colorer/parsers/helpers/HRCParserHelpers.h"
    "shared/colorer/parsers/helpers/TextParserHelpers.h"
    "shared/colorer/parsers/HRCParserImpl.h"
    "shared/colorer/parsers/TextParserImpl.h"
    "shared/colorer/Region.h"
    "shared/colorer/RegionHandler.h"
    "shared/colorer/Scheme.h"
    "shared/colorer/TextParser.h"
    "shared/colorer/viewer/ConsoleTools.h"
    "shared/colorer/viewer/ParsedLineWriter.h"
    "shared/colorer/viewer/TextConsoleViewer.h"
    "shared/colorer/viewer/TextLinesStore.h"
    "shared/common/Common.h"
    "shared/common/Exception.h"
    "shared/common/Features.h"
    "shared/common/Hashtable.h"
    "shared/common/HashtableCore.h"
    "shared/common/io/FileInputSource.h"
    "shared/common/io/FileWriter.h"
    "shared/common/io/HTTPInputSource.h"
    "shared/common/io/InputSource.h"
    "shared/common/io/JARInputSource.h"
    "shared/common/io/MemoryFile.h"
    "shared/common/io/SharedInputSource.h"
    "shared/common/io/StreamWriter.h"
    "shared/common/io/StringWriter.h"
    "shared/common/io/Writer.h"
    "shared/common/Logging.h"
    "shared/common/MemoryChunks.h"
    "shared/common/MemoryOperator.h"
    "shared/common/Vector.h"
    "shared/cregexp/cregexp.h"
    "shared/farplugin/farcolor.hpp"
    "shared/farplugin/farkeys.hpp"
    "shared/farplugin/fmt.hpp"
    "shared/farplugin/plugin.hpp"
    "shared/unicode/BitArray.h"
    "shared/unicode/Character.h"
    "shared/unicode/CharacterClass.h"
    "shared/unicode/DString.h"
    "shared/unicode/Encodings.h"
    "shared/unicode/SString.h"
    "shared/unicode/String.h"
    "shared/unicode/StringBuffer.h"
    "shared/unicode/UnicodeTools.h"
    "shared/unicode/x_charcategory.h"
    "shared/unicode/x_charcategory2.h"
    "shared/unicode/x_charcategory_names.h"
    "shared/unicode/x_defines.h"
    "shared/unicode/x_encodings.h"
    "shared/unicode/x_tables.h"
    "shared/xml/xmldom.h"
    "zlib/contrib/minizip/ioapi.h"
    "zlib/contrib/minizip/unzip.h"
    "zlib/contrib/minizip/zconf.h"
    "zlib/infblock.h"
    "zlib/infcodes.h"
    "zlib/inffast.h"
    "zlib/inffixed.h"
    "zlib/inftrees.h"
    "zlib/infutil.h"
    "zlib/zconf.h"
    "zlib/zlib.h"
    "zlib/zutil.h"
)
source_group("Header Files" FILES ${Header_Files})

set(Source_Files__colorer
    "shared/colorer/ParserFactory.cpp"
    #"ConsoleToolsRunner.cpp"
)
source_group("Source Files\\colorer" FILES ${Source_Files__colorer})

set(Source_Files__colorer__editor
    "shared/colorer/editor/BaseEditor.cpp"
    "shared/colorer/editor/Outliner.cpp"
)
source_group("Source Files\\colorer\\editor" FILES ${Source_Files__colorer__editor})

set(Source_Files__colorer__handlers
    "shared/colorer/handlers/FileErrorHandler.cpp"
    "shared/colorer/handlers/LineRegionsCompactSupport.cpp"
    "shared/colorer/handlers/LineRegionsSupport.cpp"
    "shared/colorer/handlers/RegionMapperImpl.cpp"
    "shared/colorer/handlers/StyledHRDMapper.cpp"
    "shared/colorer/handlers/TextHRDMapper.cpp"
)
source_group("Source Files\\colorer\\handlers" FILES ${Source_Files__colorer__handlers})

set(Source_Files__colorer__parser
    "shared/colorer/parsers/helpers/HRCParserHelpers.cpp"
    "shared/colorer/parsers/helpers/TextParserHelpers.cpp"
    "shared/colorer/parsers/HRCParserImpl.cpp"
    "shared/colorer/parsers/TextParserImpl.cpp"
)
source_group("Source Files\\colorer\\parser" FILES ${Source_Files__colorer__parser})

set(Source_Files__colorer__viewer
    "shared/colorer/viewer/ConsoleTools.cpp"
    "shared/colorer/viewer/TextConsoleViewer.cpp"
    "shared/colorer/viewer/TextLinesStore.cpp"
    
)
source_group("Source Files\\colorer\\viewer" FILES ${Source_Files__colorer__viewer})

set(Source_Files__common
    "shared/colorer/handlers/ErrorHandlerWriter.cpp"
    "shared/common/Exception.cpp"
    "shared/common/io/FileInputSource.cpp"
    "shared/common/io/FileWriter.cpp"
    "shared/common/io/HTTPInputSource.cpp"
    "shared/common/io/InputSource.cpp"
    "shared/common/io/JARInputSource.cpp"
    "shared/common/io/SharedInputSource.cpp"
    "shared/common/io/StreamWriter.cpp"
    "shared/common/io/StringWriter.cpp"
    "shared/common/io/Writer.cpp"
    "shared/common/Logging.cpp"
    "shared/common/MemoryChunks.cpp"
    "shared/cregexp/cregexp.cpp"
    "shared/misc/malloc.c"
    "shared/misc/registry.cpp"
)
source_group("Source Files\\common" FILES ${Source_Files__common})

set(Source_Files__unicode
    "shared/unicode/BitArray.cpp"
    "shared/unicode/Character.cpp"
    "shared/unicode/CharacterClass.cpp"
    "shared/unicode/DString.cpp"
    "shared/unicode/Encodings.cpp"
    "shared/unicode/SString.cpp"
    "shared/unicode/String.cpp"
    "shared/unicode/StringBuffer.cpp"
    "shared/unicode/UnicodeTools.cpp"
)
source_group("Source Files\\unicode" FILES ${Source_Files__unicode})

set(Source_Files__unzip
    "zlib/adler32.c"
    "zlib/contrib/minizip/ioapi.c"
    "zlib/contrib/minizip/unzip.c"
    "zlib/crc32.c"
    "zlib/infblock.c"
    "zlib/infcodes.c"
    "zlib/inffast.c"
    "zlib/inflate.c"
    "zlib/inftrees.c"
    "zlib/infutil.c"
    "zlib/uncompr.c"
    "zlib/zutil.c"
)
source_group("Source Files\\unzip" FILES ${Source_Files__unzip})

set(Source_Files__xml
    "shared/xml/xmldom.cpp"
)
source_group("Source Files\\xml" FILES ${Source_Files__xml})
add_definitions(-DNOMINMAX -D_CRT_SECURE_NO_WARNINGS -DWIN32)
set(ALL_COLORER_FILES
    ${Header_Files}
    ${Source_Files__colorer}
    ${Source_Files__colorer__editor}
    ${Source_Files__colorer__handlers}
    ${Source_Files__colorer__parser}
    ${Source_Files__colorer__viewer}
    ${Source_Files__common}
    ${Source_Files__unicode}
    ${Source_Files__unzip}
    ${Source_Files__xml}
)


#----------------------------------------------------------------------------  
#  
# ----------------------------------------------------------------------------
SET(SOURCE
main/main.cpp
main/LogWindow.cpp
main/TextEditor.cpp
main/FileBrowser/ImGuiFileBrowser.cpp
imgui/examples/libs/gl3w/GL/gl3w.c
imgui/examples/imgui_impl_glfw.cpp
imgui/examples/imgui_impl_opengl3.cpp
imgui/imgui.cpp
imgui/imgui_draw.cpp
imgui/imgui_widgets.cpp
)
#----------------------------------------------------------------------------  
#  
# ----------------------------------------------------------------------------
SET(HEADERS
main/LogWindow.h
main/FileBrowser/ImGuiFileBrowser.h
)
#----------------------------------------------------------------------------  
#  
# ----------------------------------------------------------------------------
file(GLOB ADD_HEADERS "./imgui/*.h*")
SET(HEADERS ${HEADERS} ${ADD_HEADERS})

SOURCE_GROUP("Source Files" FILES ${SOURCE})
SOURCE_GROUP("Header Files" FILES ${HEADERS})
# ----------------------------------------------------------------------------  
# 
# ----------------------------------------------------------------------------
  ADD_EXECUTABLE( ${PROJECT_NAME}
  				  ${SOURCE}
  				  ${HEADERS}
                  ${ALL_COLORER_FILES}
  				  )
target_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE "./")
target_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE "./shared")
target_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE "./shared/common/io")
target_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE "imgui")
target_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE "imgui/examples")
target_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE "imgui/examples/libs/gl3w")
target_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE "imgui/examples/libs/glfw/include")
target_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE "main")
target_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE "main/CText")
target_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/zlib)
target_include_directories(${PROJECT_NAME} PRIVATE main/FileBrowser/Dirent)
target_include_directories(${PROJECT_NAME} PRIVATE main/FileBrowser)

# target_include_directories(${PROJECT_NAME} PRIVATE icu/source/common)
# ----------------------------------------------------------------------------
# 
# ----------------------------------------------------------------------------
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
					  optimized ${LIBRARY_OUTPUT_PATH}/Release/glfw3.lib
                      debug ${LIBRARY_OUTPUT_PATH}/Debug/glfw3.lib
					  opengl32.lib
                      "wininet"
					  )
# ----------------------------------------------------------------------------   
# 
# ----------------------------------------------------------------------------
set(Boost_USE_STATIC_LIBS   ON)
set(Boost_USE_STATIC_RUNTIME   ON)
find_package(Boost 1.54 REQUIRED COMPONENTS  date_time)
target_include_directories(${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIRS})
target_link_directories(${PROJECT_NAME} PRIVATE ${Boost_LIBRARY_DIRS})
get_directory_property(_my_link_dirs LINK_DIRECTORIES)
message(STATUS "_my_link_dirs = ${_my_link_dirs}") 
get_directory_property(_my_inc_dirs INCLUDE_DIRECTORIES)
message(STATUS "_my_inc_dirs = ${_my_inc_dirs}") 

# ----------------------------------------------------------------------------
# 
# ----------------------------------------------------------------------------
 file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/fonts DESTINATION ${CMAKE_BINARY_DIR}/Release)
 file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/fonts DESTINATION ${CMAKE_BINARY_DIR}/Debug)

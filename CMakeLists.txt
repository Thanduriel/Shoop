cmake_minimum_required (VERSION 3.8)
project (shoop)
set (CMAKE_CXX_STANDARD 17)

# find all source and header files
file(GLOB_RECURSE SOURCE_FILES "src/*.hpp" "src/*.cpp" "src/*.inl")

# organize the files according to the organisation in the file system
# from: http://cmake.3232098.n2.nabble.com/Keep-folder-structure-in-Visual-Studio-Project-generated-with-CMake-td7586044.html
macro(GroupSources curdir)
   file(GLOB children RELATIVE ${PROJECT_SOURCE_DIR}/${curdir} ${PROJECT_SOURCE_DIR}/${curdir}/*)
   foreach(child ${children})
          if(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/${curdir}/${child})
                  GroupSources(${curdir}/${child})
          else()
                  string(REPLACE "/" "\\" groupname ${curdir})
            source_group(${groupname} FILES ${PROJECT_SOURCE_DIR}/${curdir}/${child})
          endif()
   endforeach()
endmacro()

# execute tha macro
GroupSources(src)

if(MSVC)
	add_compile_options( /MP )
endif(MSVC)

# SFML config: use dynamic libs; skip examples and the network module
set(SFML_BUILD_GRAPHICS TRUE)
set(SFML_BUILD_WINDOW TRUE)
set(BUILD_SHARED_LIBS TRUE)
set(SFML_BUILD_AUDIO TRUE)
set(SFML_BUILD_EXAMPLES FALSE)
set(SFML_BUILD_DOC FALSE)
set(SFML_BUILD_NETWORK FALSE)

add_subdirectory("dependencies/SFML")

# faster compilation with multithreading
# cmake does not support vs compiler switches for std
if(MSVC)
	add_compile_options( /std:c++17 )
	
	set(OPTIMIZATION_OPTIONS 
    $<$<CONFIG:Debug>:>
    $<$<CONFIG:RelWithDebInfo>:>
    $<$<CONFIG:Release>: /O2 /Ob2 /Oi /Ot /GL > 
    $<$<CONFIG:MinSizeRel>:>
    )
	
	add_compile_options( "${OPTIMIZATION_OPTIONS}" )
endif(MSVC)

include_directories("src")
add_executable(shoop ${SOURCE_FILES})

#SFML
include_directories("dependencies/SFML/include")
target_link_libraries(shoop sfml-system sfml-graphics sfml-window)

#box2d
include_directories("dependencies/Box2D/Box2D")
file(GLOB_RECURSE BOX2D_SOURCE_FILES "dependencies/Box2D/Box2D/Box2D/*.h" "dependencies/Box2D/Box2D/Box2D/*.cpp")
add_library(Box2D STATIC ${BOX2D_SOURCE_FILES})
target_link_libraries(shoop Box2D)
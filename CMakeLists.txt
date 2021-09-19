cmake_minimum_required (VERSION 3.12)
project (shoop)
set (CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

include_directories("src")
add_executable(shoop ${SOURCE_FILES})

# faster compilation with multithreading
# cmake does not support vs compiler switches for std
if(MSVC)
	set(OPTIMIZATION_OPTIONS 
		$<$<CONFIG:Debug>:>
		$<$<CONFIG:RelWithDebInfo>:>
		$<$<CONFIG:Release>: /O2 /Ob2 /Oi /Ot /GL > 
		$<$<CONFIG:MinSizeRel>:>
    )
	
	add_compile_options( "${OPTIMIZATION_OPTIONS}" )
	add_compile_options( /MP )
else()
	set(OPTIMIZATION_OPTIONS 
		$<$<CONFIG:Debug>:-Wall;-pedantic>
		$<$<CONFIG:RelWithDebInfo>:>
		$<$<CONFIG:Release>: -O3;-DNDEBUG > 
		$<$<CONFIG:MinSizeRel>:>
    )
	target_compile_options(shoop PUBLIC "${OPTIMIZATION_OPTIONS}")
endif(MSVC)

#SFML
find_package(SFML COMPONENTS system graphics window QUIET)
if (NOT SFML_FOUND)
	message("SFML not found. Attempting to use local submodule version.")
	set(SFML_BUILD_GRAPHICS TRUE)
	set(SFML_BUILD_WINDOW TRUE)
	set(BUILD_SHARED_LIBS FALSE)
	set(SFML_STATIC_LIBRARIES TRUE)
	set(SFML_BUILD_AUDIO FALSE)
	set(SFML_BUILD_EXAMPLES FALSE)
	set(SFML_BUILD_DOC FALSE)
	set(SFML_BUILD_NETWORK FALSE)
	target_include_directories(shoop PRIVATE "dependencies/SFML/include")
	add_subdirectory("dependencies/SFML")
endif(NOT SFML_FOUND)
target_link_libraries(shoop sfml-system sfml-graphics sfml-window)

#box2d
include_directories("dependencies/Box2D/Box2D")
file(GLOB_RECURSE BOX2D_SOURCE_FILES "dependencies/Box2D/Box2D/Box2D/*.h" "dependencies/Box2D/Box2D/Box2D/*.cpp")
add_library(Box2D STATIC ${BOX2D_SOURCE_FILES})
target_link_libraries(shoop Box2D)

#spdlog
include_directories("dependencies/spdlog/include")
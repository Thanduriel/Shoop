cmake_minimum_required (VERSION 3.12)
project (shoop)

# additional features
OPTION(WITH_LEARNING "Enable the RL AI." OFF)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

# find all source and header files
file(GLOB_RECURSE SOURCE_FILES "src/*.hpp" "src/*.cpp" "src/*.inl")

if (NOT WITH_LEARNING)
	list(REMOVE_ITEM SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/src/gameplay/bots/learning.cpp")
endif()

# organize the files according to the organization in the file system
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

# execute the macro
GroupSources(src)

add_executable(shoop ${SOURCE_FILES})
target_include_directories(shoop PRIVATE "src")

# not c++20 because torch is incompatible with it
set_property(TARGET shoop PROPERTY CXX_STANDARD 17)

if(MSVC)
	set(OPTIMIZATION_OPTIONS 
		$<$<CONFIG:Debug>:>
		$<$<CONFIG:RelWithDebInfo>:>
		$<$<CONFIG:Release>: /O2 /Ob2 /Oi /Ot > 
		$<$<CONFIG:MinSizeRel>:>
	)
	
	# faster compilation with multi-threading
	add_compile_options( /MP )

	# set game as startup project
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT shoop)
else()
	set(OPTIMIZATION_OPTIONS 
		$<$<CONFIG:Debug>:-Wall;-pedantic>
		$<$<CONFIG:RelWithDebInfo>:>
		$<$<CONFIG:Release>: -O3;-DNDEBUG > 
		$<$<CONFIG:MinSizeRel>:>
	)
endif(MSVC)
target_compile_options( shoop PUBLIC "${OPTIMIZATION_OPTIONS}" )

include(CheckIPOSupported)
check_ipo_supported(RESULT result)
if(result)
	set_property(TARGET shoop PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
endif()

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
set(BOX2D_BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(BOX2D_BUILD_TESTBED OFF CACHE BOOL "" FORCE)
add_subdirectory("dependencies/Box2D")
target_include_directories(shoop PRIVATE "dependencies/box2d/include")
target_link_libraries(shoop box2d)

#spdlog
target_include_directories(shoop PRIVATE "dependencies/spdlog/include")

# torch
if (WITH_LEARNING)
	target_compile_definitions(shoop PUBLIC "LEARNING_AI")
	find_package(Torch REQUIRED)
	target_link_libraries(shoop "${TORCH_LIBRARIES}")
endif()
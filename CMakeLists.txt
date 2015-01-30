cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
set(CMAKE_LEGACY_CYGWIN_WIN32 0)

project("TopDown-Project")
# Version number.
set(TopDown-Project_VERSION_MAJOR 1)
set(TopDown-Project_VERSION_MINOR 0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# Include directory
include_directories(${CMAKE_SOURCE_DIR}/include/)

# Link Boost libraries
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost COMPONENTS filesystem system serialization program_options)
include_directories(${Boost_INCLUDE_DIR})
if(Boost_FOUND)
	set(LIBS ${LIBS} ${Boost_LIBRARIES})
endif()

# Link SFML library
find_package(SFML COMPONENTS graphics window system audio)
include_directories(${SFML_INCLUDE_DIR})
set(LIBS ${LIBS} ${SFML_LIBRARIES})

# Link Box2D library
find_package(Box2D)
include_directories(${BOX2D_INCLUDE_DIR})
set(LIBS ${LIBS} ${BOX2D_LIBRARY})

# Path to external libraries directory
set(EXT_LIBS_PATH ${CMAKE_SOURCE_DIR}/ext/)

# Link AngelScript library
set(AngelScriptSDK_ROOT ${EXT_LIBS_PATH}/angelscript_sdk)
set(AngelScript_INCLUDE_DIR ${AngelScriptSDK_ROOT}/angelscript/include)
set(AngelScript_LIBRARY ${AngelScriptSDK_ROOT}/angelscript/lib/libAngelscript.a)
include_directories(${AngelScript_INCLUDE_DIR})
set(LIBS ${LIBS} ${AngelScript_LIBRARY})

enable_testing()

add_executable(TopDown src/Utils.cpp
					   src/Math/Math.cpp
					   src/Math/Vector2.cpp
					   src/Math/Vector3.cpp
					   src/Math/Matrix3.cpp
					   src/Math/Matrix4.cpp
					   src/Math/Quaternion.cpp
					   src/Filesystem/Assets/AssetManager.cpp
					   src/Scene/Component.cpp
					   src/Scene/GameObject.cpp
					   src/Scene/Scene.cpp
					   src/Core.cpp
					   src/main.cpp)

target_link_libraries(TopDown ${LIBS})

add_test(topDownTest TopDown)

# Main cmake file 
cmake_minimum_required(VERSION 2.6)

# policy for relative subfolders
cmake_policy(SET CMP0015 NEW)


set(AppName PolyhedronTest)

message(STATUS "Preparing ${AppName}")

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(OUTPUT_BINDIR ${CMAKE_SOURCE_DIR}/bin)
make_directory(${OUTPUT_BINDIR})


# Set the project architecture for this example
set( PROJECT_ARCH "x64" )

# All output files are copied to bin
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

# All library output files are copied to dep/lib
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/../dep/lib)

# Set the runtime output path
set( RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../bin")
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin")
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin")



set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# Find packages
#FIND_PACKAGE(Cuda REQUIRED)
#FIND_PACKAGE(Cub REQUIRED)
FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(glew REQUIRED)
FIND_PACKAGE(glm REQUIRED)
FIND_PACKAGE(GLFW3 REQUIRED)
FIND_PACKAGE(Eigen3 )
FIND_PACKAGE(OpenCV REQUIRED)

#if (CUDA_FOUND)
#message(STATUS "Found CUDA in ${CUDA_INCLUDE_DIRS} and ${CUDA_LIBRARIES}")
#endif()


#if (CUB_FOUND)
#message(STATUS "Found CUB in ${CUB_INCLUDE_DIR} ")
#else(CUB_FOUND)
#message(FATAL_ERROR  "Did not find CUB")
#endif(CUB_FOUND)


if (GLEW_FOUND)
message(STATUS "Found GLEW ${GLEW_INCLUDE_DIR} and ${GLEW_LIBRARIES}")
else(GLEW_FOUND)
message(FATAL_ERROR  "Did not find GLEW")
endif(GLEW_FOUND)

if (OPENGL_FOUND)
message(STATUS "Found OPENGL ${OPENGL_INCLUDE_DIR} and ${OPENGL_LIBRARIES}")
else(OPENGL_FOUND)
message(FATAL_ERROR  "Did not find OPENGL")
endif(OPENGL_FOUND)

if (GLFW_FOUND)
message(STATUS "Found GLEW ${GLFW_INCLUDE_DIRS} and ${GLFW3_LIBRARY}")
else()
IF(WIN32)
ELSE(WIN32)
include_directories("/usr/local/Cellar/glfw/3.2.1/include")
include_directories("/usr/local/Cellar/glfw/1.11.0/include")
set( GLFW3_LIBRARY glfw)
ENDIF(WIN32)
endif()

if (EIGEN3_FOUND)
message(STATUS "Found Eigen in ${EIGEN3_INCLUDE_DIR}")
else(EIGEN3_FOUND)
message(FATAL_ERROR  "Error: Did not find Eigen3")
endif(EIGEN3_FOUND)


# Include dirs
include_directories("/usr/local/include/eigen3")
#include_directories(${CUB_INCLUDE_DIR})
#include_directories(${CUDA_INCLUDE_DIRS})

include_directories("C:/SDK/opencv-3.4.5/build/include")
include_directories(${OPENCV_INCLUDE_DIR})
include_directories(${EIGEN3_INCLUDE_DIR})
include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${GLEW_INCLUDE_DIR})
include_directories(${GLFW3_INCLUDE_DIR})
include_directories(${GLM_INCLUDE_DIR})
include_directories(../gl_common_ext)
include_directories(../test_src)
include_directories(../src)
#include_directories(./loader)
#include_directories(./detection)
#include_directories(./kdtree)


# Add all files to the configuration
set(MAIN_SRC
	test_main.cpp
	Polyhedron.h
	Polyhedron.cpp
	../src/PolyhedronGeometry.h
	../src/PolyhedronGeometry.cpp
)


set(SHADER_SRC
	../src/lit_scene.fs
	../src/lit_scene.vs
)


set(UTILS_SRC

)





set(OPENGL_SRC
	../gl_common_ext/OpenGLDefaults.h
	../gl_common_ext/OpenGLDefaults.cpp
	../gl_common_ext/VertexBuffers.h
	../gl_common_ext/VertexBuffers.cpp
	../gl_common_ext/ShaderProgram.h
	../gl_common_ext/ShaderProgram.cpp
	../gl_common_ext/TrackballControls.h
	../gl_common_ext/TrackballControls.cpp
	../gl_common_ext/ModelCoordinateSystem.cpp
	../gl_common_ext/ModelCoordinateSystem.h
	../gl_common_ext/Window.cpp
	../gl_common_ext/Window.h
	../gl_common_ext/ModelPlane.h
	../gl_common_ext/ModelPlane.cpp
    ../gl_common_ext/Texture2D.h
	../gl_common_ext/Texture2D.cpp
	../gl_common_ext/BMPLoader.h
	../gl_common_ext/BMPLoader.cpp
	../gl_common_ext/ModelOBJ.h
	../gl_common_ext/ModelOBJ.cpp
	../gl_common_ext/ModelSphere.h
	../gl_common_ext/ModelSphere.cpp
	../gl_common_ext/RenderToTexture.h
	../gl_common_ext/CameraControls.h
	../gl_common_ext/CameraControls.cpp
	../gl_common_ext/CommonTypes.h
)


source_group(MAIN FILES ${MAIN_SRC})
source_group(OpenGL FILES ${OPENGL_SRC})
source_group(Shaders FILES ${SHADER_SRC})
source_group(Utils FILES ${UTILS_SRC})




add_compile_definitions(_WITH_CAMERA)
add_compile_definitions(GLM_ENABLE_EXPERIMENTAL)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)


# Create an executable
add_executable(${AppName} 
${MAIN_SRC} ${OPENGL_SRC} ${UTILS_SRC} ${SHADER_SRC})


# Add link directories
# Note required for this project
IF(WIN32)
	link_directories(${CMAKE_SOURCE_DIR}/../SDK/lib)
ELSE(WIN32)
	link_directories(${CMAKE_SOURCE_DIR}/../SDK/lib)
	link_directories(/usr/local/lib)
	link_directories(/usr/local/Cellar/glfw3/3.0.4/lib)
	link_directories(/usr/local/Cellar/glfw3/3.0.4/lib)
	link_directories(/usr/local/Cellar/glew/1.11.0/lib)
	link_directories(/usr/local/Cellar/glfw/3.2.1/lib)
ENDIF(WIN32)


# Add libraries
target_link_libraries(${AppName}   ${GLEW_LIBRARIES} ${GLEW_LIBRARY} ${GLFW3_LIBRARY} ${OPENGL_LIBRARIES} ${OPENCV_LIBRARIES}  )

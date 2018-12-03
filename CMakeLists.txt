cmake_minimum_required(VERSION 3.1)
project(FaceMeshRT)

set(CMAKE_CXX_STANDARD 14)
###############################################################################
# Settings
################################################################################
if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to Release")
    set(CMAKE_BUILD_TYPE "Release")
endif()

# use opencv and eigen3 in the system directory
FIND_PACKAGE(OpenCV REQUIRED)
FIND_PACKAGE(EIGEN3 REQUIRED)








set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)







#make an alias of of directory storing non-system external libraries
set(FaceMeshRT_EXTERN "${CMAKE_SOURCE_DIR}/extern")


##########################################################################
#libigl
##########################################################################
option(LIBIGL_WITH_OPENGL            "Use OpenGL"         ON)
option(LIBIGL_WITH_OPENGL_GLFW       "Use GLFW"           ON)
option(LIBIGL_WITH_VIEWER            "Use OpenGL viewer"  ON)
FIND_PACKAGE(LIBIGL REQUIRED QUIET)


#########################################################################
#dlib
#########################################################################
FIND_PACKAGE(DLIB REQUIRED)
add_subdirectory(${DLIB_INCLUDE_DIR} ${DLIB_INCLUDE_DIR}/build)



#########################################################################
#eos, unfortunatly in th a old fashion
#########################################################################
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/extern/eos)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/extern/eos/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/extern/eos/3rdparty/cereal/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/extern/eos/3rdparty/glm)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/extern/eos/3rdparty/nanoflann/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/extern/eos/3rdparty/eigen/Eigen)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/extern/eos/3rdparty/eigen3-nnls/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/extern/eos/3rdparty/toml11)



##########################################################################
#FaceMeshRT static library
##########################################################################

add_library(libFaceMeshRT STATIC
        ###  A LOT INPUT HERE
        src/FaceMeshRT/Face2Landmark.h
        src/FaceMeshRT/Face2Landmark.cpp
        src/FaceMeshRT/Landmark2Mesh.cpp
        src/FaceMeshRT/Landmark2Mesh.h
        src/FaceMeshRT/LandmarkRegistration.cpp
        src/FaceMeshRT/LandmarkRegistration.h)
target_link_libraries(libFaceMeshRT
        PUBLIC
        ${OpenCV_LIBS}
        igl::core
        igl::opengl_glfw
        dlib::dlib
        PRIVATE
        Eigen3::Eigen
        )





##########################################################################
#FaceMeshRT executable
##########################################################################
add_executable(${PROJECT_NAME}
        src/main.cpp
        src/FaceMeshRT/Face2Landmark.cpp
        src/FaceMeshRT/Face2Landmark.h)
target_link_libraries(${PROJECT_NAME}
        ${OpenCV_LIBS}
        libFaceMeshRT
        igl::core
        igl::opengl_glfw
        )

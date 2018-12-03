# - Try to find the DLIB library
# Once done this will define
#
#  DLIB_FOUND - system has DLIB
#  DLIB_INCLUDE_DIR - **the** DILB include directory
if(DLIB_FOUND)
    return()
endif()

find_path(DLIB_INCLUDE_DIR dlib/opencv.h
        HINTS
        ENV DLIB
        ENV DLIBROOT
        ENV DLIB_ROOT
        ENV DLIB_DIR
        PATHS
        ${CMAKE_SOURCE_DIR}/../..
        ${CMAKE_SOURCE_DIR}/..
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/dlib
        ${CMAKE_SOURCE_DIR}/../dlib
        ${CMAKE_SOURCE_DIR}/../../dlib
        /usr
        /usr/local
        PATH_SUFFIXES dlib
        )


include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(DLIB
        "\ndlib not found --- You can download it using:\n\thttps://github.com/davisking/dlib.git ${CMAKE_SOURCE_DIR}/../dlib"
        DLIB_INCLUDE_DIR)
mark_as_advanced(DLIB_INCLUDE_DIR)



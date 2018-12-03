# - Try to find the EOS library
# Once done this will define
#
#  EOS_FOUND - system has EOS
#  EOS_INCLUDE_DIR - **the** EOS include directory
if(EOS_FOUND)
    return()
    message(${EOS_INCLUDE_DIR})
endif()

find_path(EOS_INCLUDE_DIR eos/core/Image.hpp
        PATHS
        ${CMAKE_SOURCE_DIR}/eos/eos
        ${CMAKE_SOURCE_DIR}/../eos/eos
        ${CMAKE_SOURCE_DIR}/../../eos/eos
        PATH_SUFFIXES include
        )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(EOS
        "\ndlib not found --- You can download it using:\n\thttps://github.com/patrikhuber/eos.git ${CMAKE_SOURCE_DIR}/../dlib"
        EOS_INCLUDE_DIR)
mark_as_advanced(EOS_INCLUDE_DIR)



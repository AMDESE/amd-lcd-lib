#
# Minimum version of cmake required
#
cmake_minimum_required(VERSION 3.5.0)

## Set default module path if not already set
if(NOT DEFINED CMAKE_MODULE_PATH)
    set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/")
endif()
## Include common cmake modules
include(utils)

set(LCDLIB "lcdlib")
set(LCDLIB_COMPONENT "lib${LCDLIB}")
set(LCDLIB_TARGET "${LCDLIB}32")
set(SRC_DIR "src/lcdlib")
set(INC_DIR "include/lcdlib")

# The following default version values should be updated as appropriate for
# ABI breaks (update MAJOR and MINOR), and ABI/API additions (update MINOR).
# Until ABI stabilizes VERSION_MAJOR will be 0. This should be over-ridden
# by git tags (through "git describe") when they are present.
set(LCDLIB_LIB_VERSION_MAJOR 0)
set(LCDLIB_LIB_VERSION_MINOR 1)

################# Determine the library version #########################
## Setup the package version based on git tags.
set(LIB_SO_VERSION_STR
              "${LCDLIB_LIB_VERSION_MAJOR}.${LCDLIB_LIB_VERSION_MINOR}.0")
get_version(${LIB_SO_VERSION_STR} "lcdlib")

# VERSION_* variables should be set by get_version
set(LIB_SO_VERSION_STR ${VERSION_STRING})
set(${LCDLIB}_VERSION_MAJOR "${VERSION_MAJOR}")
set(${LCDLIB}_VERSION_MINOR "${VERSION_MINOR}")
set(${LCDLIB}_VERSION_PATCH "${VERSION_PATCH}")
set(${LCDLIB}_VERSION_BUILD "${VERSION_BUILD}")

# Debian package specific variables
# Set a default value for the package version
set(VERSION_STRING "1.0.0")
get_version(${VERSION_STRING} "e")

# VERSION_* variables should be set by get_version
set(BUILD_VERSION_STRING "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

project(${LCDLIB_TARGET})

# Create a configure file to get version info from within library
#configure_file(
#  "${PROJECT_SOURCE_DIR}/${SRC_DIR}/${LCDLIB_TARGET}Config.in"
#  "${PROJECT_SOURCE_DIR}/${INC_DIR}/${LCDLIB_TARGET}Config.h")

if (NOT DEFINED CPACK_PACKAGE_VENDOR)
    set(CPACK_PACKAGE_VENDOR "AMD")
endif()

if (NOT DEFINED CPACK_PACKAGE_CONTACT)
    set(CPACK_PACKAGE_CONTACT "Advanced Micro Devices Inc.")
endif()

if (NOT DEFINED CPACK_PACKAGE_DESCRIPTION_SUMMARY)
    # Conditionally set the description summary based on the selected version
    if (USE_OLED_VERSION)
        set(CPACK_PACKAGE_DESCRIPTION_SUMMARY
            "SP7 LCD display Interface library")
    else()
        set(CPACK_PACKAGE_DESCRIPTION_SUMMARY
            "SP5 LCD display Interface library")
    endif()
endif()

set(CPACK_PACKAGE_FILE_NAME "lcdlib_lib32-${BUILD_VERSION_STRING}")
## Verbose output.
set(CMAKE_VERBOSE_MAKEFILE on)

## Compiler flags
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -fno-rtti -m64")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse -msse2 -std=c++11 ")
# Use this instead of above for 32 bit
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")

if ("${CMAKE_BUILD_TYPE}" STREQUAL Release)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb -O0 -DDEBUG")
endif ()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
# Conditionally include source and header files based on the selected version
if (USE_OLED_VERSION)
    set(LCDLIB_SRC_LIST
        "${SRC_DIR}/lcdlib_common_oled.c"
    )
    set(LCD_INC_LIST
        "${INC_DIR}/lcdlib_common_oled.h"
    )
    message(STATUS "Building lcdlib for SP7 (SPI OLED)")
else()
    set(LCDLIB_SRC_LIST
        "${SRC_DIR}/lcdlib_common.c"
    )
    set(LCD_INC_LIST
        "${INC_DIR}/lcdlib_common.h"
    )
    message(STATUS "Building lcdlib for SP5 (I2C)")
endif()


#set(LCD_TOOL "lcdlib_tool")

#add_executable(${LCD_TOOL} "example/lcdlib_tool.c")

#target_link_libraries(${LCD_TOOL} ${LCDLIB_TARGET})

add_library(${LCDLIB_TARGET} SHARED ${LCDLIB_SRC_LIST} ${LCD_INC_LIST})
target_link_libraries(${LCDLIB_TARGET} pthread rt m i2c)

## Set the VERSION and SOVERSION values
set_property(TARGET ${LCDLIB_TARGET}
                             PROPERTY VERSION "${LIB_SO_VERSION_STR}")
set_property(TARGET ${LCDLIB_TARGET}
             PROPERTY SOVERSION "${LCDLIB_LIB_VERSION_MAJOR}")

## If the library is a release, strip the target library
if ("${CMAKE_BUILD_TYPE}" STREQUAL Release)
    add_custom_command(
          TARGET ${LCDLIB_TARGET}
          POST_BUILD COMMAND ${CMAKE_STRIP} lib${LCDLIB_TARGET}.so)
endif ()

## Define default variable and variables for the optional build target
##  lcdlib_lib-dev
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	CACHE STRING "Location of LCDLIB source code.")
set(CMAKE_INSTALL_PREFIX "/opt/lcdlib"
                                CACHE STRING "Default installation directory.")
set(CPACK_PACKAGING_INSTALL_PREFIX "/opt/lcdlib"
                                      CACHE STRING "Default packaging prefix.")

## Add the install directives for the runtime library.
install(TARGETS ${LCDLIB_TARGET}
          LIBRARY DESTINATION ${LCDLIB}/lib COMPONENT ${LCDLIB_COMPONENT})
install(FILES ${SOURCE_DIR}/include/lcdlib/common.h
                                        DESTINATION lcdlib/include/lcdlib)

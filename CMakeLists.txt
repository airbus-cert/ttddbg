# Minimum CMake required
cmake_minimum_required(VERSION 3.11)

# Project
project(ttddbg LANGUAGES CXX VERSION 1.0.0)

# Opions
option(BUILD_TESTS "Build test programs" OFF)

# Config
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

if (MSVC)
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT /WX")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd /WX")
endif(MSVC)

# Add cmake folder for 
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")


# The IDA plugin
add_subdirectory(ttddbg)

# Tests
if(BUILD_TESTS)
    add_subdirectory(tests)
endif(BUILD_TESTS)

# Summary
message(STATUS "Configuration summary")
message(STATUS "Project name                 : ${PROJECT_NAME}")
message(STATUS "Project version              : ${PROJECT_VERSION}")

# CPack part
if(WIN32)
set(CPACK_GENERATOR "WIX")
set(CPACK_WIX_UPGRADE_GUID "3fceb3d1-542b-469d-a5af-7068f95e7a68")
set(CPACK_WIX_UNINSTALL "1")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "IDA Pro 7.6")
else()
set(CPACK_SOURCE_GENERATOR "TGZ")
endif()
include(CPack)

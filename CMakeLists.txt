# Minimum CMake required
cmake_minimum_required(VERSION 3.11)

# Project
project(ttddbg LANGUAGES CXX VERSION 1.0.0)

# Opions
option(BUILD_TESTS "Build test programs" OFF)
option(INSTALL_TTDDLL "Try to find and install ttd dll from Windbg Preview install folder" OFF)

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

# Install ttd dlls if it's requested
# Use when generate installer
if (INSTALL_TTDDLL)
	find_package(TTDReplay)
	INSTALL_TTDREPLAY(".")
endif (INSTALL_TTDDLL)

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
message(STATUS "Build Tests                  : ${BUILD_TESTS}")
message(STATUS "Install TTD Dlls             : ${INSTALL_TTDDLL}")

# CPack part
if(WIN32)
set(CPACK_GENERATOR "WIX")
set(CPACK_WIX_UPGRADE_GUID "01fe726f-0a68-4913-8193-020d6911ce42")
set(CPACK_WIX_UNINSTALL "1")
endif()
include(CPack)

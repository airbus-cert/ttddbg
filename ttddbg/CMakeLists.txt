#####################################################
########### Static library use for test #############
#####################################################
# needs IDA SDK for backend
find_package(IdaSdk)

set(ttddbg_STATIC_SRC
	../ttd-bindings/TTD/TTD.cpp
	../ttd-bindings/TTD/sha256.cpp
	../ttd-bindings/TTD/utils.cpp
	src/ttddbg_plugin.cc
	src/ttddbg_debugger.cc
	src/ttddbg_debugger_x86.cc
	src/ttddbg_debugger_x86_64.cc
	src/ttddbg_debugger_manager.cc
	src/ttddbg_logger_ida.cc
	src/ttddbg_event_deque.cc
	src/ttddbg_strings.cc
	src/ttddbg_action.cc
	src/ttddbg_position_chooser.cc
)

set(ttddbg_STATIC_INCLUDE
	../ttd-bindings/TTD/TTD.hpp
	../ttd-bindings/TTD/sha256.h
	../ttd-bindings/TTD/utils.h
	include/ttddbg_debugger.hh
	include/ttddbg_debugger_x86.hh
	include/ttddbg_debugger_x86_64.hh
	include/ttddbg_debugger_manager_interface.hh
	include/ttddbg_debugger_manager.hh
	include/ttddbg_plugin.hh
	include/ttddbg_logger.hh
	include/ttddbg_logger_ida.hh
	include/ttddbg_event_deque.hh
	include/ttddbg_strings.hh
	include/ttddbg_action.hh
	include/ttddbg_position_chooser.hh
	include/ttddbg_x86_registers.hh
)

add_library(ttddbg_static STATIC ${ttddbg_STATIC_INCLUDE} ${ttddbg_STATIC_SRC})
target_compile_features(ttddbg_static PRIVATE cxx_std_17)

target_include_directories(
	ttddbg_static
	INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${IDA_SDK_INCLUDE_DIRS} include
)

target_link_libraries(ttddbg_static)

# ttddbg sources
set(ttddbg_SRC
	${ttddbg_STATIC_SRC}
)

set(ttddbg_INCLUDE
	${ttddbg_STATIC_INCLUDE}
)

if(MSVC)
	add_definitions(
		/wd4267
		/wd4244
		/wd4099
	)
endif()

##########################################################
############# build target ttddbg64 ##################
##########################################################
add_library(ttddbg64 SHARED ${ttddbg_INCLUDE} ${ttddbg_SRC})
target_compile_features(ttddbg64 PRIVATE cxx_std_17)

target_include_directories(
	ttddbg64
	INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${IDA_SDK_INCLUDE_DIRS} include
)

install(TARGETS ttddbg64
       RUNTIME DESTINATION plugins
       LIBRARY DESTINATION plugins
)

target_compile_definitions(ttddbg64 PRIVATE __EA64__)

set_target_properties(ttddbg64 PROPERTIES 
	OUTPUT_NAME "ttddbg64"
	PREFIX ""
)

if(MSVC)
	target_compile_definitions(ttddbg64 PRIVATE __NT__)
endif()

target_link_libraries(ttddbg64 ${IDA_SDK_LIBS_IDA64})

#####################################################
# Target for Ida 32, but need to be built in 64 bits#
#####################################################

add_library(ttddbg SHARED ${ttddbg_INCLUDE} ${ttddbg_SRC})

target_compile_features(ttddbg PRIVATE cxx_std_17)

target_include_directories(
	ttddbg
	INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${IDA_SDK_INCLUDE_DIRS} include
)

install(TARGETS ttddbg
	RUNTIME DESTINATION plugins
	LIBRARY DESTINATION plugins
)

set_target_properties(ttddbg PROPERTIES 
	OUTPUT_NAME "ttddbg"
	PREFIX ""
)

if(MSVC)	
	target_compile_definitions(ttddbg PRIVATE __NT__)
endif()

target_link_libraries(ttddbg ${IDA_SDK_LIBS_IDA32})

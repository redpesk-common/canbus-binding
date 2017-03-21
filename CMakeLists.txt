cmake_minimum_required(VERSION 3.3)
project(can-config-generator)

if (CMAKE_VERSION VERSION_LESS "3.1")
	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
		set (CMAKE_CXX_FLAGS "--std=gnu++11 ${CMAKE_CXX_FLAGS}")
	elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		set (CMAKE_CXX_FLAGS "--std=c++11 ${CMAKE_CXX_FLAGS}")
	endif ()
else ()
	set (CMAKE_CXX_STANDARD 11)
endif ()

include_directories(SYSTEM 3rdparty/json)

find_package(Boost REQUIRED COMPONENTS program_options filesystem system)
if(Boost_FOUND)
	add_definitions(${Boost_LIB_DIAGNOSTIC_DEFINITIONS})
	include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

	add_executable(can-config-generator
		src/main.cpp
		src/openxc/message_set.cpp
		src/openxc/can_bus.cpp
		src/openxc/can_message.cpp
		src/openxc/command.cpp
		src/openxc/diagnostic_message.cpp
		src/openxc/mapping.cpp
		src/openxc/signal.cpp)

	target_link_libraries(can-config-generator ${Boost_LIBRARIES})
	target_compile_features(can-config-generator PRIVATE cxx_range_for cxx_constexpr cxx_nullptr)
endif()

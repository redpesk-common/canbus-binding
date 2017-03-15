cmake_minimum_required(VERSION 3.0)
project(can-config-generator)

include_directories(SYSTEM 3rdparty/json)

#add_definitions( -DBOOST_ALL_NO_LIB )
#set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS program_options filesystem)
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

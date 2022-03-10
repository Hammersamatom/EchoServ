cmake_minimum_required(VERSION 3.12)
project(echoserv VERSION 0.0.1 DESCRIPTION "A non-compliant and simple IRC implementation using socklib/SKS.")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS_RELEASE "-Os ${CMAKE_CXX_FLAGS_RELEASE}")

#set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include") #Local include directory for headers
include(GNUInstallDirs) #GNUInstallDirs (a default module) is required for this CMake

#Options
#option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

#Default build type
if (NOT CMAKE_BUILD_TYPE) #If the user hasn't set it, we set it
	set(CMAKE_BUILD_TYPE Release)
endif()

#Source files:
set(SOURCE_FILES 
	"main.cpp"
)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCE_FILES})

find_package(fmt REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME}
			PRIVATE
			fmt::fmt
			socks
)

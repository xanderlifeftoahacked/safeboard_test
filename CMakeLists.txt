cmake_minimum_required(VERSION 3.27)
project(antivirus LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-deprecated-declarations")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


file(GLOB SRC CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.cpp")
list(REMOVE_ITEM SRC "${CMAKE_SOURCE_DIR}/src/main.cpp")

add_library(AntivirusLib SHARED ${SRC})
target_include_directories(AntivirusLib PUBLIC ${CMAKE_SOURCE_DIR}/include)

if(UNIX)
  set_target_properties(AntivirusLib PROPERTIES POSITION_INDEPENDENT_CODE ON)
elseif(WIN32)
  set_target_properties(AntivirusLib PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

find_package(OpenSSL REQUIRED)
target_link_libraries(AntivirusLib PUBLIC OpenSSL::Crypto)

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE AntivirusLib)

add_subdirectory(tests)

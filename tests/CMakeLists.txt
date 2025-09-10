cmake_minimum_required(VERSION 3.27)

enable_testing()

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/heads/main.zip
)

FetchContent_MakeAvailable(googletest)

file(GLOB TEST_SOURCES *.cpp)

add_executable(antivirus_tests
    test_csvparser.cpp
		test_utils.cpp
		test_logger.cpp
		test_scanner.cpp
)

target_include_directories(antivirus_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(antivirus_tests
    PRIVATE
		AntivirusLib
        gtest
        gtest_main
)

if(WIN32)
	add_custom_command(TARGET antivirus_tests POST_BUILD
		COMMAND ${CMAKE_COMMAND} 
		-E copy $<TARGET_FILE:AntivirusLib> 
		$<TARGET_FILE_DIR:antivirus_tests>
	)
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

endif()

include(GoogleTest)
gtest_discover_tests(antivirus_tests)


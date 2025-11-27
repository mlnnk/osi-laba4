# CMakeList.txt: проект CMake для osi-laba4; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#
cmake_minimum_required (VERSION 3.8)

# Включение горячей перезагрузки для компиляторов MSVC, если поддерживается.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("osi-laba4")

# Добавьте источник в исполняемый файл этого проекта.
add_subdirectory(cpp2017)
add_subdirectory(cpp2011)

add_executable(queue_tests17 test/QueueOperationsTest.cpp cpp2017/src/QueueOperations.cpp )
add_executable(queue_tests11 test/QueueOperationsTest.cpp cpp2011/src/QueueOperations.cpp )

include(FetchContent)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
target_include_directories(queue_tests17 PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp2017/src
    ${CMAKE_CURRENT_SOURCE_DIR}/test
    ${gtest_SOURCE_DIR}/include
    ${gtest_BINARY_DIR}/include
)

target_include_directories(queue_tests11 PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp2011/src
    ${CMAKE_CURRENT_SOURCE_DIR}/test
    ${gtest_SOURCE_DIR}/include
    ${gtest_BINARY_DIR}/include
)

target_link_libraries(queue_tests17 GTest::gtest GTest::gtest_main)
target_link_libraries(queue_tests11 GTest::gtest GTest::gtest_main)

set_target_properties(queue_tests17 PROPERTIES CXX_STANDARD 17)


set_target_properties(queue_tests11 PROPERTIES CXX_STANDARD 11)

include(GoogleTest)
gtest_discover_tests(queue_tests11)
gtest_discover_tests(queue_tests17)


# TODO: Добавьте тесты и целевые объекты, если это необходимо.

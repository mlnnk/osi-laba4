# CMakeList.txt: проект CMake для cpp2017; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#
cmake_minimum_required (VERSION 3.8)

# Включение горячей перезагрузки для компиляторов MSVC, если поддерживается.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project(cpp2017)


add_executable(receiver17 receiver.cpp "src/QueueOperations.cpp" "src/QueueOperations.h")


add_executable(sender17 src/Sender.cpp)

target_include_directories(receiver17 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(sender17 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
set_target_properties(receiver17 sender17 PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)
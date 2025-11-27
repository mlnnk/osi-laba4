cmake_minimum_required(VERSION 3.8)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project(cpp2011)


add_executable(receiver11 receiver.cpp src/QueueOperations.cpp)


add_executable(sender11 src/Sender.cpp src/QueueOperations.cpp)


target_include_directories(receiver11 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(sender11 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

set_target_properties(receiver11 sender11 PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)
cmake_minimum_required(VERSION 3.28)
project(Proj)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")

# Config
set(Python_EXECUTABLE "C:/Projects/Tools/python.exe")
find_package(Python 3.11 REQUIRED COMPONENTS Interpreter Development NumPy)

add_executable(main main.cc)

set_target_properties(main PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

include_directories(${Python_INCLUDE_DIRS})

link_directories(${Python_LIBRARY_DIRS})

add_subdirectory(uni)
add_subdirectory(dp)
add_subdirectory(brains)
add_subdirectory(eval)


target_link_libraries(main 
    ${Python_LIBRARIES}
    uni_library
    dp_library
    brain_library
    eval_library
)
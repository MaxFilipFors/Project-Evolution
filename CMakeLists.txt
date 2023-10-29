cmake_minimum_required(VERSION 3.28)
project(Proj)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")

# Python Configuration
find_package(Python3 3.12.0 REQUIRED COMPONENTS Interpreter Development)

# Targets
add_executable(main main.cc)

set_target_properties(main PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)
# Include python
include_directories(${Python3_INCLUDE_DIRS})

# Link Python libraries
link_directories(${Python3_LIBRARY_DIRS})

# Subdirectories
add_subdirectory(handler)
add_subdirectory(uni)
add_subdirectory(env)
add_subdirectory(dp)

# Linking all libraries in a single call
target_link_libraries(main 
    PRIVATE 
    ${Python3_LIBRARIES}
    uni_library
    dp_library
    env_library
    handler_library
)

cmake_minimum_required(VERSION 3.28)
project(Proj)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
add_executable(main main.cc )
add_executable(handler handler/handler.cc )

# Add subdirectories
add_subdirectory(handler)
add_subdirectory(uni)
add_subdirectory(env)
add_subdirectory(dp)

target_link_libraries(main uni_library)
target_link_libraries(main dp_library)
target_link_libraries(main env_library)

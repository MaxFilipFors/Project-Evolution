project(env)

set(ENV_SOURCES
    environment.cc
    environment.hpp
)

add_library(env_library ${ENV_SOURCES})
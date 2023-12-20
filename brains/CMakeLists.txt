project(brain)

set(BRAIN_SOURCES
    brainImpl.cc
    brainImpl.hpp
)

add_library(brain_library ${BRAIN_SOURCES})

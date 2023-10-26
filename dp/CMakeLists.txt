project(dp)

set(DP_SOURCES
    dataProcessor.cc
    dataProcessor.hpp
)

add_library(dp_library ${DP_SOURCES})
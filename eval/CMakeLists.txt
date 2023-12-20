project(eval)

set(EVAL_SOURCES
    evalHandlerImpl.cc
    evalHandlerImpl.hpp
)

add_library(eval_library ${EVAL_SOURCES})
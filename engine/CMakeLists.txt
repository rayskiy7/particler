add_library(
    engine STATIC
    src/handler.cpp
    src/worker.cpp
    src/engine.cpp
    src/graphics.cpp
)

target_include_directories(
    engine
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/int
)

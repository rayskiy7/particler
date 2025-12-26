add_executable(
    gl
    src/main.cpp
    src/test.cpp
)

target_include_directories(
    gl
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/int
)

if(WIN32)
    target_link_libraries(gl PRIVATE engine glfw opengl32)
else()
    find_package(OpenGL REQUIRED)
    target_link_libraries(gl PRIVATE engine glfw OpenGL::GL)
endif()

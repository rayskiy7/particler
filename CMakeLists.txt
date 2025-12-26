cmake_minimum_required(VERSION 3.16)
project(Particler)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)



include(FetchContent)
# ===== Fetch GLFW =====
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.10
)
FetchContent_MakeAvailable(glfw)

# ======================
add_subdirectory(engine)
add_subdirectory(gl)


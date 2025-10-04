cmake_minimum_required(VERSION 3.16)

project(fractalio VERSION 0.1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-c23-extensions")

include(FetchContent)

FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG        v2.0.8
    GIT_PROGRESS   TRUE
    GIT_SHALLOW    TRUE
    SOURCE_SUBDIR  cmake
)
FetchContent_MakeAvailable(glad)

add_executable(fractalio)

file(GLOB fractalio_files CONFIGURE_DEPENDS "src/**/*.cpp" "src/*.cpp")
target_sources(fractalio PRIVATE ${fractalio_files})

find_package(glfw3 REQUIRED)
find_package(OpenGL)
find_package(glm CONFIG REQUIRED)

glad_add_library(glad_gl_core_46 REPRODUCIBLE API gl:core=4.6)

target_link_libraries(fractalio PRIVATE
    glad_gl_core_46
    glfw
    ${OpenGL_LIBRARIES}
    glm::glm
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -fsanitize=address")
    target_link_libraries(fractalio PRIVATE -fsanitize=address)
endif()

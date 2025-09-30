cmake_minimum_required(VERSION 3.16)

project(fractalio VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(fractalio)

file(GLOB fractalio_files CONFIGURE_DEPENDS "src/**/*.cpp" "src/*.cpp")
target_sources(fractalio PRIVATE ${fractalio_files})

# Uncomment to add link library
# target_link_libraries(fractalio PRIVATE #[[ link library here ]])

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -fsanitize=address")
    target_link_libraries(fractalio PRIVATE -fsanitize=address)
endif()

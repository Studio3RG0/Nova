cmake_minimum_required(VERSION 3.30)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/external/vcpkg/scripts/buildsystems/vcpkg.cmake)

project(Nova VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE NOVA_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/nova/src/*.cpp
)

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Vulkan REQUIRED)

add_library(Nova SHARED)

target_link_libraries(Nova PRIVATE glfw glm::glm-header-only Vulkan::Headers Vulkan::Vulkan)
target_sources(Nova PRIVATE ${NOVA_SOURCES})
target_include_directories(Nova PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/core/src)
target_compile_options(Nova PRIVATE -Wall -Wextra -Wpedantic -Werror -std=c++2c)

file(GLOB_RECURSE LUMA_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/luma/src/*.cpp
)

add_executable(Luma)

target_link_libraries(Luma PRIVATE Nova)
target_sources(Luma PRIVATE ${LUMA_SOURCES})
target_include_directories(Luma PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/app/src)
target_compile_options(Luma PRIVATE -Wall -Wextra -Wpedantic -Werror -std=c++2c)
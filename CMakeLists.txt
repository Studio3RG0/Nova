cmake_minimum_required(VERSION 3.30)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/external/vcpkg/scripts/buildsystems/vcpkg.cmake)

project(NovaEngine3D VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE NOVA_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/NovaEngine3D/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/NovaEngine3D/src/*.hpp
)

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(VulkanHeaders CONFIG REQUIRED)

add_library(NovaEngine3D SHARED)

target_link_libraries(NovaEngine3D PRIVATE glfw glm::glm-header-only Vulkan::Headers Vulkan::Vulkan)
target_sources(NovaEngine3D PRIVATE ${NOVA_SOURCES})
target_include_directories(NovaEngine3D PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/NovaEngine3D/src)
target_compile_options(NovaEngine3D PRIVATE -Wall -Wextra -Wpedantic -Werror -std=c++2c)

file(GLOB_RECURSE LUMA_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/LumaEditor/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LumaEditor/src/*.hpp
)

add_executable(LumaEditor)

target_link_libraries(LumaEditor PRIVATE NovaEngine3D)
target_sources(LumaEditor PRIVATE ${LUMA_SOURCES})
target_include_directories(LumaEditor PRIVATE 
${CMAKE_CURRENT_SOURCE_DIR}/LumaEditor/src
${CMAKE_CURRENT_SOURCE_DIR}/NovaEngine3D/src
)
target_compile_options(LumaEditor PRIVATE -Wall -Wextra -Wpedantic -Werror -std=c++2c)
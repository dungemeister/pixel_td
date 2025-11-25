cmake_minimum_required(VERSION 3.15)

project(anime_td
        VERSION 0.0.1
        LANGUAGES C CXX
        DESCRIPTION "anime tower defence")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCES main.cpp
            game.cpp
            render_system.cpp
)

find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
find_package(SDL3_image REQUIRED)
find_package(SDL3_ttf REQUIRED)

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE m)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3_image::SDL3_image SDL3_ttf::SDL3_ttf SDL3::SDL3)
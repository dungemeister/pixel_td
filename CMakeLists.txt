cmake_minimum_required(VERSION 3.15)

project(pixel_td
        VERSION 0.0.1
        LANGUAGES C CXX
        DESCRIPTION "anime tower defence")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCES main.cpp
            game.cpp
            render_system.cpp
            level.cpp
            spawn_system.cpp
            ui/ui_layout.cpp
            ui/ui_widget.cpp
            ui/ui_label.cpp
            ui/ui_image.cpp
            ui/ui_slider.cpp
            ui/ui_circle.cpp
)

set(INCLUDE_DIRS ui)
set(SDL3_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/vender/sdl3-3.2.20)
set(SDL3_LIB_PATH ${SDL3_INCLUDE_DIR}/libSDL3.so)

set(SDL3_IMAGE_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/vender/sdl_image-3.2.0)
set(SDL3_IMAGE_LIB_PATH ${SDL3_IMAGE_INCLUDE_DIR}/libSDL3_image.so)

set(SDL3_TTF_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/vender/sdl3_ttf-3.1.0)
set(SDL3_TTFLIB_PATH ${SDL3_TTF_INCLUDE_DIR}/libSDL3_ttf.so)

# find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
# find_package(SDL3_image REQUIRED)
# find_package(SDL3_ttf REQUIRED)

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR} ${INCLUDE_DIRS} ${SDL3_INCLUDE_DIR} ${SDL3_IMAGE_INCLUDE_DIR} ${SDL3_TTF_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE m)
# target_link_libraries(${PROJECT_NAME} PRIVATE SDL3_image::SDL3_image SDL3_ttf::SDL3_ttf ${SDL3_LIB})
target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL3_LIB_PATH} ${SDL3_IMAGE_LIB_PATH} ${SDL3_TTFLIB_PATH})
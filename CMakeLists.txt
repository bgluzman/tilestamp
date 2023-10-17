cmake_minimum_required(VERSION 3.16)
project(tilestamp VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
        SDL2
        URL https://github.com/libsdl-org/SDL/releases/download/release-2.28.4/SDL2-devel-2.28.4-mingw.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
)
FetchContent_Declare(
        SDL2_image
        URL https://github.com/libsdl-org/SDL_image/releases/download/release-2.6.3/SDL2_image-devel-2.6.3-mingw.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
)
FetchContent_Declare(
        SDL2_ttf
        URL https://github.com/libsdl-org/SDL_ttf/releases/download/release-2.20.2/SDL2_ttf-devel-2.20.2-mingw.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
)
FetchContent_Declare(
        imgui
        URL https://github.com/ocornut/imgui/archive/refs/tags/v1.89.9.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
)
FetchContent_MakeAvailable(SDL2 SDL2_image SDL2_ttf imgui)

find_package(
        SDL2
        REQUIRED
        PATHS "${sdl2_SOURCE_DIR}/x86_64-w64-mingw32/lib/cmake/SDL2"
)
find_package(
        SDL2_image
        REQUIRED
        PATHS "${sdl2_image_SOURCE_DIR}/x86_64-w64-mingw32/lib/cmake/SDL2_image"
)
find_package(
        SDL2_ttf
        REQUIRED
        PATHS "${sdl2_ttf_SOURCE_DIR}/x86_64-w64-mingw32/lib/cmake/SDL2_ttf"
)

add_library(
        imgui
        OBJECT
        "${imgui_SOURCE_DIR}/imgui.cpp"
        "${imgui_SOURCE_DIR}/imgui_draw.cpp"
        "${imgui_SOURCE_DIR}/imgui_tables.cpp"
        "${imgui_SOURCE_DIR}/imgui_widgets.cpp"
        "${imgui_SOURCE_DIR}/imstb_truetype.h"
        "${imgui_SOURCE_DIR}/imstb_rectpack.h"
        "${imgui_SOURCE_DIR}/imstb_textedit.h"
        "${imgui_SOURCE_DIR}/imgui_internal.h"
        "${imgui_SOURCE_DIR}/imgui.h"
        "${imgui_SOURCE_DIR}/imconfig.h"
        "${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp"
        "${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.h"
        "${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer2.cpp"
        "${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer2.h"
        # TODO (bgluzman): removeme!
        "${imgui_SOURCE_DIR}/imgui_demo.cpp"
)
target_include_directories(imgui PUBLIC "${imgui_SOURCE_DIR}" "${imgui_SOURCE_DIR}/backends")
target_link_libraries(imgui PUBLIC SDL2::SDL2-static)

add_executable(
        tilestamp
        src/main.cpp
        src/App.cpp
        src/App.h
)
target_link_libraries(
        tilestamp
        PRIVATE
        SDL2::SDL2-static
        SDL2_image::SDL2_image-static
        SDL2_ttf::SDL2_ttf-static
        imgui)

add_executable(demo src/demo.cpp)
target_link_libraries(demo PRIVATE imgui)
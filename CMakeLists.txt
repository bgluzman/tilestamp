cmake_minimum_required(VERSION 3.16)
project(tilestamp VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (WIN32)
    set(CMAKE_CXX_COMPILER g++)
    set(SDL_ARTIFACT_SUFFIX mingw)
else ()
    set(CMAKE_CXX_COMPILER clang++)
    set(SDL_ARTIFACT_SUFFIX VC)
endif ()

function(LOAD_SDL_LIB_CONFIG LIB_NAME)
    if (WIN32)
        set(CMAKE_DIR "x86_64-w64-mingw32/lib/cmake/${LIB_NAME}")
        set(LIB_TARGET "${LIB_NAME}::${LIB_NAME}-static")
    else ()
        set(CMAKE_DIR "cmake")
        set(LIB_TARGET "${LIB_NAME}::${LIB_NAME}")
    endif ()
    string(TOLOWER "${LIB_NAME}" LIB_NAME_LOWER)
    set(${LIB_NAME}_FIND_PACKAGE_DIR "${${LIB_NAME_LOWER}_SOURCE_DIR}/${CMAKE_DIR}" PARENT_SCOPE)
    set(${LIB_NAME}_LIB_TARGET "${LIB_TARGET}" PARENT_SCOPE)
endfunction()

include(FetchContent)
FetchContent_Declare(
        SDL2
        URL "https://github.com/libsdl-org/SDL/releases/download/release-2.28.4/SDL2-devel-2.28.4-${SDL_ARTIFACT_SUFFIX}.zip"
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
)
FetchContent_Declare(
        SDL2_image
        URL "https://github.com/libsdl-org/SDL_image/releases/download/release-2.6.3/SDL2_image-devel-2.6.3-${SDL_ARTIFACT_SUFFIX}.zip"
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
)
FetchContent_Declare(
        SDL2_ttf
        URL "https://github.com/libsdl-org/SDL_ttf/releases/download/release-2.20.2/SDL2_ttf-devel-2.20.2-${SDL_ARTIFACT_SUFFIX}.zip"
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
)
FetchContent_Declare(
        imgui
        URL "https://github.com/ocornut/imgui/archive/refs/tags/v1.89.9.zip"
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
)
FetchContent_Declare(
        json
        URL "https://github.com/nlohmann/json/releases/download/v3.11.2/json.hpp"
        DOWNLOAD_NO_EXTRACT TRUE
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
)
FetchContent_MakeAvailable(SDL2 SDL2_image SDL2_ttf imgui json)

load_sdl_lib_config(SDL2)
load_sdl_lib_config(SDL2_image)
load_sdl_lib_config(SDL2_ttf)

find_package(
        SDL2
        REQUIRED
        PATHS "${SDL2_FIND_PACKAGE_DIR}"
)
find_package(
        SDL2_image
        REQUIRED
        PATHS "${SDL2_image_FIND_PACKAGE_DIR}"
)
find_package(
        SDL2_ttf
        REQUIRED
        PATHS "${SDL2_ttf_FIND_PACKAGE_DIR}"
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
        "${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp"
        "${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.h"
        "${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp"
        "${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.h"
        "${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer2.cpp"
        "${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer2.h"
        # TODO (bgluzman): removeme!
        "${imgui_SOURCE_DIR}/imgui_demo.cpp"
)
target_include_directories(imgui PUBLIC "${imgui_SOURCE_DIR}" "${imgui_SOURCE_DIR}/backends")
target_link_libraries(imgui PUBLIC "${SDL2_LIB_TARGET}")

add_library(json INTERFACE)
target_include_directories(json INTERFACE "${json_SOURCE_DIR}")

add_executable(
        tilestamp
        src/main.cpp
        src/App.cpp
        src/App.h
)
target_link_libraries(
        tilestamp
        PRIVATE
        "${SDL2_LIB_TARGET}"
        "${SDL2_image_LIB_TARGET}"
        "${SDL2_ttf_LIB_TARGET}"
        imgui
        json
)

add_executable(demo src/demo.cpp)
target_link_libraries(demo PRIVATE imgui)
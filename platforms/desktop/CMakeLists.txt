cmake_minimum_required(VERSION 3.11)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(
  sdl2
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG        2a25b69ba580ff63ede93f2b19fa1bca09dab484
)
set( SDL_STATIC ON CACHE BOOL "" FORCE )
set( SDL_SHARED OFF CACHE BOOL "" FORCE )
FetchContent_MakeAvailable(sdl2)

include(FetchContent)
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG        ad5aa5416630059c9213a4cfe00a827c3991e964
)
FetchContent_MakeAvailable(imgui)

message(${imgui_SOURCE_DIR})

# Compile as static library 
add_library(imgui ${imgui_SOURCE_DIR}/imgui.cpp
                  ${imgui_SOURCE_DIR}/imgui_demo.cpp
                  ${imgui_SOURCE_DIR}/imgui_draw.cpp
                  ${imgui_SOURCE_DIR}/imgui_tables.cpp
                  ${imgui_SOURCE_DIR}/imgui_widgets.cpp           
                  ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl.cpp
                  ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer.cpp)
target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)
target_link_libraries(imgui SDL2main SDL2-static)

set(HEADERS
  src/imfilebrowser.h
  src/gui.h
  src/audio_handler.h
)

set(SOURCES
    src/main.cpp
    src/gui.cpp
    src/audio_handler.cpp
)

add_executable(gb_app ${HEADERS} ${SOURCES})
target_link_libraries(gb_app gb_lib SDL2main SDL2-static imgui)
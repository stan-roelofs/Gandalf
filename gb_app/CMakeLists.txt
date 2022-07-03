cmake_minimum_required(VERSION 3.11)

include(FetchContent)
FetchContent_Declare(
  sdl2
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG        2a25b69ba580ff63ede93f2b19fa1bca09dab484
)
set( SDL_STATIC ON CACHE BOOL "" FORCE )
set( SDL_SHARED OFF CACHE BOOL "" FORCE )
FetchContent_MakeAvailable(sdl2)

set(SOURCES
    src/Main.cpp
)

add_executable(gb_app ${SOURCES})
target_link_libraries(gb_app gb_lib SDL2main SDL2-static)
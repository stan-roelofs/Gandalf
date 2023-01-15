cmake_minimum_required(VERSION 3.11)
set(CMAKE_CXX_STANDARD 17)
 
project(Gandalf)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

include(FetchContent)
FetchContent_Declare(
  sdl2
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG 2a25b69ba580ff63ede93f2b19fa1bca09dab484
)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(sdl2)

add_subdirectory(external/libgb)

# Compile as static library
add_library(imgui external/imgui/imgui.cpp
  external/imgui/imgui_demo.cpp
  external/imgui/imgui_draw.cpp
  external/imgui/imgui_tables.cpp
  external/imgui/imgui_widgets.cpp
  external/imgui/backends/imgui_impl_sdl.cpp
  external/imgui/backends/imgui_impl_opengl3.cpp)
target_include_directories(imgui PUBLIC external/imgui external/imgui/backends)
target_link_libraries(imgui SDL2main SDL2-static)

FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG bc889afb4c5bf1c0d8ee29ef35eaaf4c8bef8a5d
)
FetchContent_MakeAvailable(nlohmann_json)

FetchContent_Declare(
  nfd
  GIT_REPOSITORY https://github.com/btzy/nativefiledialog-extended.git
  GIT_TAG d4df2b6ad5420f5300c00f418bf28d86291fa675
)
FetchContent_MakeAvailable(nfd)

set(HEADERS
  src/audio_handler.h
  src/gui_context.h
  src/main_window.h
  src/settings.h
  src/settings_window.h
  src/text.h
  src/views/cpu_view.h
  src/views/debugger.h
  src/views/gui_element.h
  src/views/gameboy_view.h
  src/views/io_view.h
  src/views/memory_view.h
  src/views/cartridge_view.h
  src/views/apu_view.h
  src/views/vram_view.h
)

set(SOURCES
  src/audio_handler.cpp
  src/gui_context.cpp
  src/main.cpp
  src/main_window.cpp
  src/settings.cpp
  src/settings_window.cpp
  src/text.cpp
  src/views/apu_view.cpp
  src/views/cpu_view.cpp
  src/views/debugger.cpp
  src/views/gameboy_view.cpp
  src/views/io_view.cpp
  src/views/memory_view.cpp
  src/views/cartridge_view.cpp
  src/views/vram_view.cpp
)

add_executable(gb_app ${HEADERS} ${SOURCES})
target_link_libraries(gb_app libgb SDL2main SDL2-static imgui nlohmann_json::nlohmann_json nfd ${OPENGL_LIBRARIES})
target_include_directories(gb_app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

configure_file(data/imgui.ini ${CMAKE_CURRENT_BINARY_DIR}/imgui.ini COPYONLY)
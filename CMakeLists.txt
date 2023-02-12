cmake_minimum_required(VERSION 3.11)
set(CMAKE_CXX_STANDARD 17)
 
project(Gandalf)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
add_subdirectory(external/SDL)

add_subdirectory(external/gandalf-lib)

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

add_subdirectory(external/json)
add_subdirectory(external/nativefiledialog-extended)

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

add_executable(gandalf ${HEADERS} ${SOURCES})
target_link_libraries(gandalf gandalf-lib SDL2main SDL2-static imgui nlohmann_json::nlohmann_json nfd ${OPENGL_LIBRARIES})
target_include_directories(gandalf PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

configure_file(data/imgui.ini ${CMAKE_CURRENT_BINARY_DIR}/imgui.ini COPYONLY)
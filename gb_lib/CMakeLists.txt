cmake_minimum_required(VERSION 3.11)

set(HEADERS
    include/gandalf/bus.h
    include/gandalf/cartridge.h
    include/gandalf/cpu.h
    include/gandalf/cpu_registers.h
    include/gandalf/gameboy.h
    include/gandalf/model.h
    include/gandalf/timer.h
    include/gandalf/timing_handler.h
    include/gandalf/types.h
    src/bootrom.h
)

set(SOURCES
    src/bootrom.cpp
    src/bus.cpp
    src/cartridge.cpp
    src/cpu.cpp
    src/gameboy.cpp
    src/timer.cpp
)

add_library(gb_lib ${SOURCES} ${HEADERS})

if(MSVC)
  target_compile_options(gb_lib PRIVATE /W4 /WX)
else()
  target_compile_options(gb_lib PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_include_directories(gb_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(test)
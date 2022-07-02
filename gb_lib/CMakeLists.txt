set(HEADERS
    include/gandalf/gameboy.h
    include/gandalf/model.h
    include/gandalf/cartridge.h
    include/gandalf/types.h
    src/bootrom.h
    src/cpu.h
    src/cpu_registers.h
    src/memory.h
    src/timing_handler.h
)

set(SOURCES
    src/bootrom.cpp
    src/cartridge.cpp
    src/cpu.cpp
    src/gameboy.cpp
    src/memory.cpp
)

add_library(gb_lib ${SOURCES} ${HEADERS})

if(MSVC)
  target_compile_options(gb_lib PRIVATE /W4 /WX)
else()
  target_compile_options(gb_lib PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_include_directories(gb_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(test)
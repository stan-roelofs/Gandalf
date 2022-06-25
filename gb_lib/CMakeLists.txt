set(HEADERS
    include/Gameboy/Gameboy.h
    include/Gameboy/Model.h
    src/Cpu.h
    src/Memory.h
    src/BootROM.h
)

set(SOURCES
    src/BootROM.cpp
    src/Cpu.cpp
    src/Gameboy.cpp
    src/Memory.cpp
)

add_library(gb_lib ${SOURCES} ${HEADERS})

target_include_directories(gb_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(test)
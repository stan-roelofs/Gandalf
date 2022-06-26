set(HEADERS
    include/gameboy/gameboy.h
    include/gameboy/model.h
    src/cpu.h
    src/memory.h
    src/bootROM.h
)

set(SOURCES
    src/bootROM.cpp
    src/cpu.cpp
    src/gameboy.cpp
    src/memory.cpp
)

add_library(gb_lib ${SOURCES} ${HEADERS})

target_include_directories(gb_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(test)
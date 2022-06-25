set(SOURCES
    src/Gameboy.cpp
)

add_library(gb_lib ${SOURCES})

target_include_directories(gb_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
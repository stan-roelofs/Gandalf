cmake_minimum_required(VERSION 3.11)

set(CMAKE_CXX_STANDARD 17)

set(HEADERS
    include/gandalf/apu.h
    include/gandalf/bus.h
    include/gandalf/cartridge.h
    include/gandalf/cpu.h
    include/gandalf/cpu_registers.h
    include/gandalf/constants.h
    include/gandalf/dma.h
    include/gandalf/gameboy.h
    include/gandalf/hram.h
    include/gandalf/io.h
    include/gandalf/joypad.h
    include/gandalf/lcd.h
    include/gandalf/mbc.h
    include/gandalf/model.h
    include/gandalf/ppu.h
    include/gandalf/serial.h
    include/gandalf/sound/sound_channel.h
    include/gandalf/sound/frequency_sweep_unit.h
    include/gandalf/sound/length_counter.h
    include/gandalf/sound/square_wave.h
    include/gandalf/timer.h
    include/gandalf/types.h
    include/gandalf/util.h
    include/gandalf/wram.h
    src/bootrom.h
    src/cartridge/mbc1.h
    src/cartridge/rom_only.h
)

set(SOURCES
    src/apu.cpp
    src/bootrom.cpp
    src/bus.cpp
    src/cartridge.cpp
    src/cartridge/mbc.cpp
    src/cartridge/mbc1.cpp
    src/cartridge/rom_only.cpp
    src/cpu.cpp
    src/dma.cpp
    src/gameboy.cpp
    src/hram.cpp
    src/io.cpp
    src/joypad.cpp
    src/lcd.cpp
    src/ppu.cpp
    src/serial.cpp
    src/sound/frequency_sweep_unit.cpp
    src/sound/length_counter.cpp
    src/sound/sound_channel.cpp
    src/sound/square_wave.cpp
    src/timer.cpp
    src/wram.cpp
)

add_library(gb_lib ${SOURCES} ${HEADERS})

if(MSVC)
  target_compile_options(gb_lib PRIVATE /W4 /WX)
else()
  target_compile_options(gb_lib PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_include_directories(gb_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(test)
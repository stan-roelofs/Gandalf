set(SOURCES
    src/Main.cpp
)

add_executable(gb_app ${SOURCES})
target_link_libraries(gb_app gb_lib)
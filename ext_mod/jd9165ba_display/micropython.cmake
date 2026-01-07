# JD9165BA Display C Extension Module for MicroPython

# Add the C extension module source files
set(JD9165BA_DISPLAY_SRC
    ${CMAKE_CURRENT_LIST_DIR}/jd9165ba_display.c
)

# Create library for the module
add_library(jd9165ba_display INTERFACE)

target_sources(jd9165ba_display INTERFACE ${JD9165BA_DISPLAY_SRC})

target_include_directories(jd9165ba_display INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Link to micropython library
target_link_libraries(usermod INTERFACE jd9165ba_display)

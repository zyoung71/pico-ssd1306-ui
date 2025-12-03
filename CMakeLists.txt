if (NOT TARGET pico-ssd1306-ui)

    add_library(pico-ssd1306-ui STATIC
        lib/pico-ssd1306/ssd1306.c
        src/display/SSD1306.cpp
    )

    target_include_directories(pico-ssd1306-ui PUBLIC
        include
        lib/pico-ssd1306
    )

    add_subdirectory(lib/pico-interactive-ui)

    target_link_libraries(pico-ssd1306-ui
        pico_stdlib
        pico-interactive-ui
        hardware_i2c
    )

endif()
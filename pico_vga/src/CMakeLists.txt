add_executable(nevil_sbc_vga)

# compile pio headers
#pico_generate_pio_header(nevil_sbc_vga
#    ${CMAKE_CURRENT_SOURCE_DIR}/vga.pio)

pico_generate_pio_header(nevil_sbc_vga
    ${CMAKE_CURRENT_SOURCE_DIR}/bus.pio)

target_sources(nevil_sbc_vga PUBLIC
   main.c
   buffers.c
   render.c
   render_text.c
   textfont.c
   test_screen.c
   vga.c
   bus.c
)

# Add current directory to include directories. (.h files)
target_include_directories(nevil_sbc_vga PUBLIC ${CMAKE_CURRENT_SOURCE_dir})

target_link_libraries(nevil_sbc_vga PUBLIC
    pico_multicore
    pico_stdlib
    hardware_dma
    hardware_pio
)

pico_add_extra_outputs(nevil_sbc_vga)
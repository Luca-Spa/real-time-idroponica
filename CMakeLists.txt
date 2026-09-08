add_executable(idroponica
    ${CMAKE_CURRENT_LIST_DIR}/main.c
    ${CMAKE_CURRENT_LIST_DIR}/src/sensor.c
    ${CMAKE_CURRENT_LIST_DIR}/src/actuator.c
    ${CMAKE_CURRENT_LIST_DIR}/src/communication.c
    
    # Percorso assoluto per la libreria DHT
    ${CMAKE_CURRENT_LIST_DIR}/lib/pico_dht/dht/dht.c
)

pico_generate_pio_header(idroponica ${CMAKE_CURRENT_LIST_DIR}/lib/pico_dht/dht/dht.pio)

target_include_directories(idroponica PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include
    ${CMAKE_CURRENT_LIST_DIR}/lib/pico_dht/dht/include
)

target_link_libraries(idroponica
    pico_stdlib
    common_utils
    hardware_uart
    hardware_adc
    hardware_gpio
    hardware_pio
    hardware_dma
    FreeRTOS-Kernel FreeRTOS-Kernel-Heap4
    microros
)

pico_enable_stdio_usb(idroponica 1)
pico_enable_stdio_uart(idroponica 0)

target_compile_definitions(idroponica PRIVATE
    PICO_UART_ENABLE_CRLF_SUPPORT=0
    PICO_STDIO_ENABLE_CRLF_SUPPORT=0
    PICO_STDIO_DEFAULT_CRLF=0
)

# Generazione file .uf2
set_target_properties(idroponica PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

pico_add_extra_outputs(idroponica)
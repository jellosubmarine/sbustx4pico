add_executable(sbusInterface
        main.cpp
        )

# Pull in our pico_stdlib which pulls in commonly used features
target_link_libraries(sbusInterface pico_stdlib)

# create map/bin/hex file etc.
pico_add_extra_outputs(sbusInterface)
pico_enable_stdio_usb(sbusInterface 1)

# add url via pico_set_program_url
example_auto_set_url(sbusInterface)

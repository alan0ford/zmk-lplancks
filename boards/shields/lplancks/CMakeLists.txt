list(APPEND SHIELD_SRC
    caps_lock_led.c
)

zephyr_library_sources(${SHIELD_SRC})
zephyr_include_directories(include)

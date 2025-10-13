zephyr_library()

target_sources(app PRIVATE
  caps_lock_led.c
  )

zephyr_include_directories(include)

idf_component_register(SRCS "blink_example_main.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES led_strip)
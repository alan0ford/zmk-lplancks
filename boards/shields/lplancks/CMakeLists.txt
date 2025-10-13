zephyr_library()

target_sources(app PRIVATE
  caps_lock_led.c
  )

zephyr_include_directories(include)
set(EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/examples/common_components/led_strip)
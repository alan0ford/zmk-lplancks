zephyr_library()
target_sources(app PRIVATE
  caps_lock_led.c
  behavior_battery_printer.c
  )
zephyr_include_directories(include)
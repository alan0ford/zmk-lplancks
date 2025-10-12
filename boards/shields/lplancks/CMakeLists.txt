list(APPEND SHIELD_SRC
    caps_lock_led.c
)

zephyr_library_sources(${SHIELD_SRC})

zephyr_include_directories(
  ${ZMK_BASE}/app/dts/overlays
  ${ZMK_BASE}/app/dts/behaviors
  ${ZMK_BASE}/app/dts
  ${ZMK_BASE}/app/include
  ${ZMK_BASE}/app/include/zmk
)


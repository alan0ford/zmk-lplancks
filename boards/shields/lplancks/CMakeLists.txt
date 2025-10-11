cmake_minimum_required(VERSION 3.20.0)

# Dichiara la libreria Zephyr
zephyr_library()

# Percorso verso la root ZMK (modifica in base alla tua struttura)
set(ZMK_BASE ${CMAKE_CURRENT_LIST_DIR}/../../..)

# Includi sorgenti comportamenti e eventi necessari di ZMK
zephyr_library_sources(
  ${ZMK_BASE}/app/src/event_manager.c
  ${ZMK_BASE}/app/src/events/hid_indicators_changed.c
  ${ZMK_BASE}/app/src/behaviors/behavior_rgb_underglow.c
)

# Aggiungi il tuo file personalizzato
zephyr_library_sources(
  caps_lock_led.c
)

# Includi le directory degli header di ZMK
zephyr_include_directories(
  ${ZMK_BASE}/app/include
  ${ZMK_BASE}/app/include/zmk
)


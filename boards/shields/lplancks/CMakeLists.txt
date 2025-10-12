zephyr_library()

list(APPEND SHIELD_SRC
    caps_lock_led.c
)

zephyr_library_sources(${SHIELD_SRC})

zephyr_include_directories(
  ${ZMK_BASE}/app/include
  ${ZMK_BASE}/app/include/zmk
)

# Questa aggiunta è fondamentale per far vedere i file .dtsi in devicetree overlays/includes
set(DTC_OVERLAY_SEARCH_PATHS
    ${ZMK_BASE}/app/dts
    ${ZMK_BASE}/app/dts/behaviors
    ${ZMK_BASE}/app/dts/overlays
)

# Dice a Zephyr dove cercare i file .dtsi inclusi nei devicetree
target_include_directories(app PRIVATE ${DTC_OVERLAY_SEARCH_PATHS})


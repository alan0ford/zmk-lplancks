#zephyr_library()

#target_sources(app PRIVATE
#  caps_lock_led.c
#  )

#zephyr_include_directories(include)

if ((NOT CONFIG_ZMK_SPLIT) OR CONFIG_ZMK_SPLIT_ROLE_CENTRAL)

target_sources(app PRIVATE
  caps_lock_led.c
)

endif()
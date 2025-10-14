#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/led_strip.h>
#include <zephyr/init.h>
#include <zmk/rgb_underglow.h>
#include <zmk/event_manager.h>
#include <zmk/events/hid_indicators_changed.h>
#include <zmk/hid_indicators.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/init.h>
#include <zmk/rgb_underglow.h>
#include <zmk/event_manager.h>
#include <zmk/events/hid_indicators_changed.h>
#include <zmk/hid_indicators.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(caps_led_debug, LOG_LEVEL_DBG);

static int caps_lock_led_listener_cb(const zmk_event_t *eh) {
    LOG_DBG("Evento HID indicator ricevuto!");

    zmk_hid_indicators_t indicators = zmk_hid_indicators_get_current_profile();

    if (indicators & HID_USAGE_LED_CAPS_LOCK) {
        LOG_INF("Caps Lock ATTIVO. Imposto il LED a blu.");
        struct hsb_color color = {.hue = 240, .saturation = 100, .brightness = 100};
        zmk_rgb_underglow_set_hsb(color);
    } else {
        LOG_INF("Caps Lock NON attivo. Spengo il LED.");
        struct hsb_color color = {.hue = 0, .saturation = 0, .brightness = 0};
        zmk_rgb_underglow_set_hsb(color);
    }

    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(caps_lock_led_listener, caps_lock_led_listener_cb);
ZMK_SUBSCRIPTION(caps_lock_led_listener, zmk_hid_indicators_changed);

static int caps_led_init(const struct device *dev) {
    ARG_UNUSED(dev);
    LOG_ERR(">>> PROVA DEFINITIVA: Il file 'caps_lock_led.c' E' STATO COMPILATO! <<<");
    return 0;
}

SYS_INIT(caps_led_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

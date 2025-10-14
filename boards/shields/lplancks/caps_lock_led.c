#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/init.h>
#include <zmk/rgb_underglow.h>
#include <zmk/rgb_types.h>  
#include <zmk/event_manager.h>
#include <zmk/events/hid_indicators_changed.h>
#include <zmk/hid_indicators.h>
#include <zephyr/logging/log.h>


LOG_MODULE_REGISTER(caps_led_debug, LOG_LEVEL_DBG);

static int caps_lock_led_listener_cb(const zmk_event_t *eh) {
    LOG_DBG("Evento HID indicator ricevuto!");

    zmk_hid_indicators_t indicators = zmk_hid_indicators_get_current_profile();

    struct zmk_led_hsb color;
	
    if (indicators & HID_USAGE_LED_CAPS_LOCK) {
        color.h = (uint8_t)((240 * 255) / 360);  // scala da 0-360 a 0-255
        color.s = 255;                          // max saturazione
        color.b = 255;                          // max brightness
    } else {
        color.h = 0;
        color.s = 0;
        color.b = 0;
    }

    zmk_rgb_underglow_set_hsb(color);
	
    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(caps_lock_led_listener, caps_lock_led_listener_cb);
ZMK_SUBSCRIPTION(caps_lock_led_listener, zmk_hid_indicators_changed);

static int caps_led_init(const struct device *dev) {
    ARG_UNUSED(dev);
    LOG_INF("caps_lock_led driver avviato.");
    return 0;
}

SYS_INIT(caps_led_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

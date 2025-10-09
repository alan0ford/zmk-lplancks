#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zmk/rgb_underglow.h>
#include <zmk/event_manager.h>
#include <zmk/events/hid_indicators_changed.h>
#include <zmk/hid_indicators.h>
#include <drivers/led_strip.h>

static const struct device *led_strip = DEVICE_DT_GET(DT_ALIAS(smartled));

static const struct led_rgb COLOR_CAPS_ON = { .r = 0xFF, .g = 0xFF, .b = 0xFF };   
static const struct led_rgb COLOR_CAPS_OFF = { .r = 0x00, .g = 0x00, .b = 0x00 };  

static int caps_lock_led_listener_cb(const zmk_event_t *eh) {
    if (!device_is_ready(led_strip)) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    zmk_hid_indicators_t indicators = zmk_hid_indicators_get_current_profile();

    if (indicators & HID_USAGE_LED_CAPS_LOCK) {
        zmk_rgb_underglow_set_all(led_strip, &COLOR_CAPS_ON);
    } else {
        zmk_rgb_underglow_set_all(led_strip, &COLOR_CAPS_OFF);
    }

    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(caps_lock_led_listener, caps_lock_led_listener_cb);
ZMK_SUBSCRIPTION(caps_lock_led_listener, zmk_hid_indicators_changed);
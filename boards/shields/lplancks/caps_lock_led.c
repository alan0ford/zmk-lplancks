#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/init.h>
#include <zmk/event_manager.h>
#include <zmk/events/hid_indicators_changed.h>
#include <zmk/hid_indicators.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/led_strip.h>

LOG_MODULE_REGISTER(caps_led_debug, LOG_LEVEL_DBG);

#define CAPS_LED_NODE DT_ALIAS(smartled)
static const struct device *caps_led = DEVICE_DT_GET(CAPS_LED_NODE);

static void set_caps_led(bool caps_on) {
    if (!device_is_ready(caps_led)) {
        LOG_ERR("LED strip device not ready");
        return;
    }
    struct led_rgb leds[1];
    if (caps_on) {
        leds[0].r = 0;
        leds[0].g = 0;
        leds[0].b = 255; // blu
    } else {
        leds[0].r = 0;
        leds[0].g = 0;
        leds[0].b = 0; // spento
    }
    led_strip_update_rgb(caps_led, leds, ARRAY_SIZE(leds));
}

static int caps_lock_led_listener_cb(const zmk_event_t *eh) {
    LOG_DBG("Evento HID indicator ricevuto!");
    zmk_hid_indicators_t indicators = zmk_hid_indicators_get_current_profile();
    set_caps_led(indicators & HID_USAGE_LED_CAPS_LOCK);
    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(caps_lock_led_listener, caps_lock_led_listener_cb);
ZMK_SUBSCRIPTION(caps_lock_led_listener, zmk_hid_indicators_changed);

static int caps_led_init(const struct device *dev) {
    ARG_UNUSED(dev);
    LOG_INF("caps_lock_led driver avviato.");
    set_caps_led(false); // all'avvio LED spento
    return 0;
}

SYS_INIT(caps_led_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>
#include <zmk/event_manager.h>
#include <zmk/events/hid_indicators_changed.h>
#include <zmk/hid_indicators.h>
#include <zmk/rgb_underglow.h>

LOG_MODULE_REGISTER(capslock_indicator, LOG_LEVEL_DBG);

#define COLOR_OFF   {0,0,0}         // spento
#define COLOR_ON    {240, 100, 100} // blu acceso (HSB: 240°)

static void set_capslock_led(bool caps_on) {
    struct zmk_led_hsb color = caps_on ? (struct zmk_led_hsb)COLOR_ON : (struct zmk_led_hsb)COLOR_OFF;
    zmk_rgb_underglow_set_hsb(color);
    if (caps_on) {
        zmk_rgb_underglow_on();
        LOG_DBG("Capslock ON: LED blu acceso");
    } else {
        zmk_rgb_underglow_off();
        LOG_DBG("Capslock OFF: LED spento");
    }
}

static int capslock_listener_cb(const zmk_event_t *eh) {
    zmk_hid_indicators_t indicators = zmk_hid_indicators_get_current_profile();
    bool caps_on = indicators & HID_USAGE_LED_CAPS_LOCK;
    set_capslock_led(caps_on);
    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(capslock_listener, capslock_listener_cb);
ZMK_SUBSCRIPTION(capslock_listener, zmk_hid_indicators_changed);

static int capslock_init(const struct device *dev) {
    ARG_UNUSED(dev);
    set_capslock_led(false); // spento all'avvio
    LOG_INF("Modulo Capslock Indicator inizializzato.");
    return 0;
}

SYS_INIT(capslock_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
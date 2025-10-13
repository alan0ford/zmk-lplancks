#include <zephyr/device.h>
#include <zephyr/drivers/led_strip.h>
#include <zmk/event_manager.h>
#include <zmk/events/hid_indicators_changed.h>
#include <zmk/hid_indicators.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(caps_lock_led, LOG_LEVEL_INF);

static const struct device *led_strip = DEVICE_DT_GET(DT_ALIAS(smartled));

static const struct led_rgb COLOR_CAPS_ON = { .r = 255, .g = 255, .b = 255 };
static const struct led_rgb COLOR_CAPS_OFF = { .r = 0, .g = 0, .b = 0 };

static void caps_lock_set_led(bool on) {
    if (!device_is_ready(led_strip)) {
        LOG_ERR("LED strip device non pronto");
        return;
    }

    int rc;
    if (on) {
        rc = led_strip_write_rgb(led_strip, 0, 1, &COLOR_CAPS_ON);
    } else {
        rc = led_strip_write_rgb(led_strip, 0, 1, &COLOR_CAPS_OFF);
    }

    if (rc) {
        LOG_ERR("Errore scrittura led_strip: %d", rc);
    }
}

static int caps_lock_listener_cb(const zmk_event_t *eh) {
    zmk_hid_indicators_t indicators = zmk_hid_indicators_get_current_profile();
    bool caps_on = (indicators & HID_USAGE_LED_CAPS_LOCK) != 0;

    caps_lock_set_led(caps_on);

    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(caps_lock_led_listener, caps_lock_listener_cb);
ZMK_SUBSCRIPTION(caps_lock_led_listener, zmk_hid_indicators_changed);

static int caps_lock_led_init(const struct device *dev) {
    ARG_UNUSED(dev);
    LOG_INF("caps_lock_led driver avviato");
    return 0;
}

SYS_INIT(caps_lock_led_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

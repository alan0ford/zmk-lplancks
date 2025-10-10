#include <zephyr/logging/log.h>
#include <zephyr/device.h>
#include <zmk/rgb_underglow.h>
#include <zmk/events/hid_indicators_changed.h>
#include <zmk/event_manager.h>

LOG_MODULE_REGISTER(caps_lock_test, LOG_LEVEL_INF);

static const struct device *led_strip = DEVICE_DT_GET(DT_ALIAS(smartled));
static const struct led_rgb COLOR_ON = { .r = 0xFF, .g = 0xFF, .b = 0xFF };

static int simple_listener_cb(const zmk_event_t *eh) {
    printk("Listener chiamato - accendo LED");

    if (!device_is_ready(led_strip)) {
        printk("Device LED non pronto");
        return ZMK_EV_EVENT_BUBBLE;
    }

    zmk_rgb_underglow_set_all(led_strip, &COLOR_ON);

    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(caps_lock_test_listener, simple_listener_cb);
ZMK_SUBSCRIPTION(caps_lock_test_listener, zmk_hid_indicators_changed);

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/led_strip.h>
#include <zephyr/init.h>
#include <zmk/rgb_underglow.h>
#include <zmk/event_manager.h>
#include <zmk/events/hid_indicators_changed.h>
#include <zmk/hid_indicators.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(caps_led_debug, LOG_LEVEL_DBG);

static const struct device *led_strip = DEVICE_DT_GET(DT_ALIAS(smartled));
// static const struct led_rgb COLOR_CAPS_ON = { .r = 0xFF, .g = 0xFF, .b = 0xFF };
// static const struct led_rgb COLOR_CAPS_OFF = { .r = 0x00, .g = 0x00, .b = 0x00 };

static int caps_lock_led_listener_cb(const zmk_event_t *eh) {
    LOG_DBG("Evento HID indicator ricevuto!");

    if (!device_is_ready(led_strip)) {
        LOG_ERR("Dispositivo LED 'smartled' non pronto.");
        return ZMK_EV_EVENT_BUBBLE;
    }

    zmk_hid_indicators_t indicators = zmk_hid_indicators_get_current_profile();

    if (indicators & HID_USAGE_LED_CAPS_LOCK) {
        LOG_INF("Caps Lock ATTIVO. Imposto il LED a blu.");
        zmk_rgb_underglow_set_hsb(240, 100, 100);
    } else {
        LOG_INF("Caps Lock NON attivo. Spengo il LED.");
        zmk_rgb_underglow_set_hsb(0, 0, 0);
    }

    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(caps_lock_led_listener, caps_lock_led_listener_cb);
ZMK_SUBSCRIPTION(caps_lock_led_listener, zmk_hid_indicators_changed);

static int caps_led_init(void) {
    LOG_ERR(">>> PROVA DEFINITIVA: Il file 'caps_lock_led.c' E' STATO COMPILATO! <<<");
    return 0;
}

SYS_INIT(caps_led_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
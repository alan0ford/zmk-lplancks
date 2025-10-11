#include <zephyr/device.h>
#include <zephyr/drivers/led_strip.h>
#include <zmk/event_manager.h>
#include <zmk/events/hid_indicators_changed.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(caps_lock_led, LOG_LEVEL_DBG);

static const struct device *led_strip = DEVICE_DT_GET(DT_ALIAS(smartled));

static void set_led_white(bool on) {
    if (!device_is_ready(led_strip)) {
        LOG_ERR("LED strip device non pronto");
        return;
    }
    uint8_t color[3] = {0, 0, 0};
    if (on) {
        // Colore bianco: massimo rosso, verde e blu
        color[0] = 0xFF; // R
        color[1] = 0xFF; // G
        color[2] = 0xFF; // B
    }
    // Scrivi colore al primo LED della striscia
    int err = led_strip_write(led_strip, 0, 1, color);
    if (err) {
        LOG_ERR("Errore scrittura LED: %d", err);
    }
}

static int caps_lock_led_listener_cb(const zmk_event_t *eh) {
    LOG_DBG("Evento HID indicator ricevuto");

    zmk_hid_indicators_t indicators = zmk_hid_indicators_get_current_profile();
    bool caps_on = (indicators & HID_USAGE_LED_CAPS_LOCK) != 0;

    if (caps_on) {
        LOG_INF("Caps Lock attivo: accendo LED");
        set_led_white(true);
    } else {
        LOG_INF("Caps Lock non attivo: spengo LED");
        set_led_white(false);
    }

    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(caps_lock_led_listener, caps_lock_led_listener_cb);
ZMK_SUBSCRIPTION(caps_lock_led_listener, zmk_hid_indicators_changed);

// Funzione init per debug - sarà chiamata all'avvio
static int caps_led_init(const struct device *dev) {
    ARG_UNUSED(dev);
    LOG_INF("Caps lock LED init");
    return 0;
}
SYS_INIT(caps_led_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

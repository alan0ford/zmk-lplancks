#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <drivers/led_strip.h>
#include <zephyr/logging/log.h>

#include <zmk/event_manager.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/events/hid_indicators_changed.h>
#include <zmk/hid_indicators.h>
#include <zmk/rgb_underglow.h>

LOG_MODULE_REGISTER(custom_led, LOG_LEVEL_INF);

// Definisci qui i tuoi colori in formato (Rosso, Verde, Blu)
static const struct led_rgb COLOR_OFF = { .r = 0x00, .g = 0x00, .b = 0x00 };
static const struct led_rgb COLOR_LAYER_1 = { .r = 0x00, .g = 0x00, .b = 0xFF }; // Blu per il layer 1
static const struct led_rgb COLOR_CAPS = { .r = 0xFF, .g = 0xFF, .b = 0xFF };    // Bianco per il Caps Lock

// Prendiamo il riferimento al nostro LED WS2812 dal devicetree
static const struct device *led_strip = DEVICE_DT_GET(DT_ALIAS(led-strip));

// Variabili globali per tenere traccia dello stato
static bool caps_lock_on = false;
static uint8_t current_layer = 0;

// Funzione centrale che decide il colore del LED
static void update_led_status(void) {
    if (!device_is_ready(led_strip)) {
        return;
    }

    struct led_rgb color_to_set = COLOR_OFF;

    if (caps_lock_on) {
        color_to_set = COLOR_CAPS;
    } else if (current_layer == 1) { // Cambia '1' con l'indice del layer che vuoi indicare
        color_to_set = COLOR_LAYER_1;
    }
    // Puoi aggiungere altri 'else if' per altri layer qui
    // else if (current_layer == 2) { color_to_set = ... }

    // Applica il colore a tutti i LED della striscia (nel tuo caso, solo uno)
    zmk_rgb_underglow_set_all(led_strip, &color_to_set);
}

// Listener per il cambio di stato dei layer
static int layer_listener_cb(const zmk_event_t *eh) {
    const struct zmk_layer_state_changed *ev = as_zmk_layer_state_changed(eh);
    if (ev) {
        current_layer = ev->layer;
        update_led_status();
    }
    return ZMK_EV_EVENT_BUBBLE;
}

// Listener per il cambio degli indicatori HID (Caps Lock, Num Lock, etc.)
static int caps_lock_listener_cb(const zmk_event_t *eh) {
    const struct zmk_hid_indicators_changed *ev = as_zmk_hid_indicators_changed(eh);
    if (ev) {
        zmk_hid_indicators_t indicators = zmk_hid_indicators_get_current_profile();
        caps_lock_on = (indicators & HID_USAGE_LED_CAPS_LOCK) != 0;
        update_led_status();
    }
    return ZMK_EV_EVENT_BUBBLE;
}

// Registra i nostri listener
ZMK_LISTENER(layer_listener, layer_listener_cb);
ZMK_SUBSCRIPTION(layer_listener, zmk_layer_state_changed);

ZMK_LISTENER(caps_lock_listener, caps_lock_listener_cb);
ZMK_SUBSCRIPTION(caps_lock_listener, zmk_hid_indicators_changed);
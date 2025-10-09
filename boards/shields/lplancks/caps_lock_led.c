#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zmk/rgb_underglow.h>
#include <zmk/event_manager.h>
#include <zmk/events/hid_indicators_changed.h>
#include <zmk/hid_indicators.h>

// Prendiamo il riferimento al nostro LED dal DeviceTree usando l'alias "smartled"
static const struct device *led_strip = DEVICE_DT_GET(DT_ALIAS(smartled));

// Definiamo i colori che useremo
static const struct led_rgb COLOR_CAPS_ON = { .r = 0xFF, .g = 0xFF, .b = 0xFF };   // Bianco
static const struct led_rgb COLOR_CAPS_OFF = { .r = 0x00, .g = 0x00, .b = 0x00 };  // Spento

// Questa funzione viene chiamata ogni volta che lo stato degli indicatori (Caps, Num, etc.) cambia
static int caps_lock_led_listener_cb(const zmk_event_t *eh) {
    // Controlliamo che il nostro dispositivo LED sia pronto
    if (!device_is_ready(led_strip)) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    // Prendiamo lo stato attuale degli indicatori
    zmk_hid_indicators_t indicators = zmk_hid_indicators_get_current_profile();

    // Controlliamo se il bit del Caps Lock è attivo
    if (indicators & HID_USAGE_LED_CAPS_LOCK) {
        // Se è attivo, impostiamo il colore a ON
        zmk_rgb_underglow_set_all(led_strip, &COLOR_CAPS_ON);
    } else {
        // Altrimenti, lo impostiamo a OFF
        zmk_rgb_underglow_set_all(led_strip, &COLOR_CAPS_OFF);
    }

    return ZMK_EV_EVENT_BUBBLE;
}

// Registriamo la nostra funzione come "ascoltatore" dell'evento giusto
ZMK_LISTENER(caps_lock_led_listener, caps_lock_led_listener_cb);
ZMK_SUBSCRIPTION(caps_lock_led_listener, zmk_hid_indicators_changed);
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/init.h>
#include <zmk/rgb_underglow.h>
#include <zmk/event_manager.h>
#include <zmk/events/hid_indicators_changed.h>
#include <zmk/hid_indicators.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(caps_led_debug, LOG_LEVEL_DBG);

static int caps_lock_led_listener_cb(const zmk_event_t *eh) {
    LOG_DBG("Evento HID indicator ricevuto!");

    zmk_hid_indicators_t indicators = zmk_hid_indicators_get_current_profile();

    if (indicators & HID_USAGE_LED_CAPS_LOCK) {
        // --- Questa è la correzione ---
        // 1. Selezioniamo l'effetto "colore solido". Questo dice al motore
        //    di ZMK di smettere di eseguire animazioni e di mostrare solo il nostro colore.
        zmk_rgb_underglow_select_effect(ZMK_RGB_UNDERGLOW_EFFECT_SOLID);

        // 2. Definiamo il colore (Blu, per essere ben visibile durante il test)
        //    Formato HSB: Hue (tonalità 0-359), Saturation (saturazione 0-100), Brightness (luminosità 0-100)
        struct zmk_led_hsb color = { .h = 240, .s = 100, .b = 100 };

        // 3. Impostiamo il colore desiderato
        zmk_rgb_underglow_set_hsb(color);

        // 4. Ci assicuriamo che il sistema underglow sia ACCESO
        zmk_rgb_underglow_on();

    } else {
        // Quando il Caps Lock è disattivato, spegniamo completamente l'underglow.
        zmk_rgb_underglow_off();
    }
	
    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(caps_lock_led_listener, caps_lock_led_listener_cb);
ZMK_SUBSCRIPTION(caps_lock_led_listener, zmk_hid_indicators_changed);

// Questa funzione serve solo per confermare nei log che il nostro codice è stato avviato
static int caps_led_init(void) {
    LOG_INF(">>> Driver personalizzato per LED Caps Lock avviato <<<");
    return 0;
}

SYS_INIT(caps_led_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
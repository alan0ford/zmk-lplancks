#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zmk/events/hid_indicators_changed.h>
#include <zmk/hid_indicators.h>
#include <zmk/rgb_underglow.h>
#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#define COLOR_RESET 0
#define COLOR_BLUE 4

struct blink_item {
    uint8_t color;
    uint16_t duration_ms;
    uint16_t sleep_ms;
};

K_MSGQ_DEFINE(led_msgq, sizeof(struct blink_item), 16, 1);
static bool initialized = false;

static void set_rgb_leds(uint8_t color) {
    static const struct zmk_led_hsb color_def[8] = {
        {0,0,0},      // black
        {0,100,100},  // red
        {120,100,100},// green
        {60,100,100}, // yellow
        {240,100,100},// blue
        {300,100,100},// magenta
        {180,100,100},// cyan
        {0,0,100}     // white
    };
    zmk_rgb_underglow_select_effect(UNDERGLOW_EFFECT_SOLID);
    zmk_rgb_underglow_set_hsb(color_def[color]);
    if(color != COLOR_RESET){
        zmk_rgb_underglow_on();
    } else {
        zmk_rgb_underglow_off();
    }
}

extern void led_process_thread(void *d0, void *d1, void *d2) {
    ARG_UNUSED(d0);
    ARG_UNUSED(d1);
    ARG_UNUSED(d2);

    initialized = true;
    while (true) {
        struct blink_item blink;
        k_msgq_get(&led_msgq, &blink, K_FOREVER);
        set_rgb_leds(blink.color);
    }
}

K_THREAD_DEFINE(led_process_tid, 1024, led_process_thread, NULL, NULL, NULL,
                K_LOWEST_APPLICATION_THREAD_PRIO, 0, 100);

static int led_capslock_listener_cb(const zmk_event_t *eh) {
    if (!initialized) return 0;
    zmk_hid_indicators_t indicators = zmk_hid_indicators_get_current_profile();
    // Se Capslock attivo -> blu, altrimenti nero
    struct blink_item blink = {
        .color = (indicators & HID_USAGE_LED_CAPS_LOCK) ? COLOR_BLUE : COLOR_RESET,
        .duration_ms = 0,
        .sleep_ms = 0
    };
    k_msgq_put(&led_msgq, &blink, K_NO_WAIT);
    return 0;
}

ZMK_LISTENER(led_capslock_listener, led_capslock_listener_cb);
ZMK_SUBSCRIPTION(led_capslock_listener, zmk_hid_indicators_changed);
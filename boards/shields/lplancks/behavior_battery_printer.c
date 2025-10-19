/*
 * Minimal ZMK behavior: types battery percentage digits when activated.
 * Uses zmk_battery_state_of_charge().
 *
 * Stepwise approach: this file only. Add to repo first, then in a next step
 * we will update app/CMakeLists.txt to include it in the build.
 */

#define DT_DRV_COMPAT zmk_behavior_battery_printer

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>
#include <drivers/behavior.h>
#include <zmk/behavior.h>
#include <zmk/events/keycode_state_changed.h>
#include <zmk/battery.h>
#include <dt-bindings/zmk/keys.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#define MAX_CHARS 8
#define TYPE_DELAY_MS 10

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

struct behavior_battery_printer_data {
    struct k_work_delayable typing_work;
    uint8_t chars[MAX_CHARS];
    uint8_t chars_len;
    uint8_t current_idx;
    bool key_pressed;
};

struct behavior_battery_printer_config {
    /* reserved for future options */
};

static inline void reset_typing_state(struct behavior_battery_printer_data *data) {
    data->current_idx = 0;
    data->key_pressed = false;
    data->chars_len = 0;
    memset(data->chars, 0, sizeof(data->chars));
}

static void send_key(struct behavior_battery_printer_data *data) {
    if (data->current_idx >= data->chars_len || data->current_idx >= ARRAY_SIZE(data->chars)) {
        reset_typing_state(data);
        return;
    }

    uint8_t ch = data->chars[data->current_idx];
    uint32_t keycode = 0;

    if (ch >= '0' && ch <= '9') {
        keycode = NUMBER_0 + (ch - '0');
    } else {
        LOG_WRN("behavior_battery_printer: unsupported char '%c'", ch);
        reset_typing_state(data);
        return;
    }

    bool pressed = !data->key_pressed;
    raise_zmk_keycode_state_changed_from_encoded(keycode, pressed, k_uptime_get());
    data->key_pressed = pressed;

    if (pressed) {
        /* schedule release */
        k_work_schedule(&data->typing_work, K_MSEC(TYPE_DELAY_MS));
        return;
    } else {
        /* released -> next char */
        data->current_idx++;
        if (data->current_idx < data->chars_len) {
            k_work_schedule(&data->typing_work, K_MSEC(TYPE_DELAY_MS));
        } else {
            reset_typing_state(data);
        }
    }
}

static void type_keys_work(struct k_work *work) {
    struct k_work_delayable *dwork = CONTAINER_OF(work, struct k_work_delayable, work);
    struct behavior_battery_printer_data *data = CONTAINER_OF(dwork, struct behavior_battery_printer_data, typing_work);
    send_key(data);
}

static int behavior_battery_printer_init(const struct device *dev) {
    struct behavior_battery_printer_data *data = dev->data;
    k_work_init_delayable(&data->typing_work, type_keys_work);
    reset_typing_state(data);
    return 0;
}

/* convert small uint -> ascii digits (0..999) */
static void uint_to_chars(uint32_t v, uint8_t *buffer, uint8_t *len) {
    char tmp[4];
    int t = 0;
    if (v == 0) {
        buffer[0] = '0';
        *len = 1;
        return;
    }
    while (v > 0 && t < (int)sizeof(tmp)) {
        tmp[t++] = '0' + (v % 10);
        v /= 10;
    }
    for (int i = 0; i < t; i++) {
        buffer[i] = tmp[t - 1 - i];
    }
    *len = t;
}

/* Replace the on_pressed function with this test version that forces percent = 95 */

static int on_pressed(struct zmk_behavior_binding *binding, struct zmk_behavior_binding_event event) {
    const struct device *dev = zmk_behavior_get_binding(binding->behavior_dev);
    struct behavior_battery_printer_data *data = dev->data;

    if (data->key_pressed || data->current_idx) {
        /* typing in progress, ignore */
        return ZMK_BEHAVIOR_OPAQUE;
    }

    /* TEST: force a known percentage to validate typing logic */
    uint8_t percent = 95; /* <- change this value if you want to test other cases */
    if (percent > 100) percent = 100;

    reset_typing_state(data);
    uint_to_chars(percent, data->chars, &data->chars_len);

    /* start typing */
    send_key(data);
    return ZMK_BEHAVIOR_OPAQUE;
}

static int on_released(struct zmk_behavior_binding *binding, struct zmk_behavior_binding_event event) {
    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api behavior_battery_printer_api = {
    .binding_pressed = on_pressed,
    .binding_released = on_released,
};

#define BAT_INST(idx) \
    static struct behavior_battery_printer_data behavior_battery_printer_data_##idx; \
    static const struct behavior_battery_printer_config behavior_battery_printer_config_##idx = {}; \
    BEHAVIOR_DT_INST_DEFINE(idx, behavior_battery_printer_init, NULL, \
                            &behavior_battery_printer_data_##idx, \
                            &behavior_battery_printer_config_##idx, \
                            POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, \
                            &behavior_battery_printer_api);

DT_INST_FOREACH_STATUS_OKAY(BAT_INST)

#endif /* DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT) */
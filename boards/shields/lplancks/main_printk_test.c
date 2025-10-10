#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/init.h>
#include <zephyr/sys/printk.h>

// Questa funzione viene chiamata all'avvio del firmware, appena dopo il boot.
static int hello_init(void) {
    printk(">>>>>> PROVA PRINTK: Firmware custom caricato correttamente! <<<<<<\n");
    return 0;
}

// Il macro INIT definiamo a livello di sistema, dopo i driver principali (post kernel)
SYS_INIT(hello_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

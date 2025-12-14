#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

int main(void)
{
    printk("Hello from Zephyr motor-sim-demo (native_sim)!\n");

    while (1) {
        k_sleep(K_SECONDS(1));
        printk("Tick: %lld ms\n", k_uptime_get());
    }
    return 0;
}


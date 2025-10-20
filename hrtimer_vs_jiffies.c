#include <linux/module.h>
#include <linux/init.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/delay.h>
#include <linux/timekeeping.h>

#define HRTIMER_DELAY_US 500  // 500 microseconds
#define JIFFIES_DELAY_MS 1    // 1 millisecond

static struct hrtimer my_hrtimer;
static ktime_t start_time;
static ktime_t kt_delay;

// === HRTIMER CALLBACK ===
static enum hrtimer_restart my_hrtimer_callback(struct hrtimer *timer)
{
    ktime_t now = ktime_get();
    s64 delay_ns = ktime_to_ns(ktime_sub(now, start_time));
		
		pr_info("[MyTest][hrtimer] Delay = %lld ns = %lld µs = %lld ms\n",
        delay_ns,
        delay_ns / 1000,
        delay_ns / 1000000);


    return HRTIMER_NORESTART;
}

// === MODULE INIT ===
static int __init my_module_init(void)
{
    pr_info("[MyTest] Starting hrtimer vs jiffies test...\n");

    // Lưu thời điểm bắt đầu
    start_time = ktime_get();

    // Hẹn hrtimer sau 500 µs
    kt_delay = ktime_set(0, HRTIMER_DELAY_US * 1000); // µs → ns
    hrtimer_init(&my_hrtimer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    my_hrtimer.function = my_hrtimer_callback;
    hrtimer_start(&my_hrtimer, kt_delay, HRTIMER_MODE_REL);

    // Jiffies (delay thường)
    msleep(JIFFIES_DELAY_MS);

    // Sau khi sleep, đo lại delay
    ktime_t now = ktime_get();
    s64 delay_ns = ktime_to_ns(ktime_sub(now, start_time));

		pr_info("[MyTest][jiffies] Delay = %lld ns = %lld µs = %lld ms\n",
        delay_ns,
        delay_ns / 1000,
        delay_ns / 1000000);

    return 0;
}

// === MODULE EXIT ===
static void __exit my_module_exit(void)
{
    pr_info("[MyTest] Exiting test module.\n");
    hrtimer_cancel(&my_hrtimer);
}

module_init(my_module_init);
module_exit(my_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nhân");
MODULE_DESCRIPTION("Compare hrtimer and jiffies delay precision");

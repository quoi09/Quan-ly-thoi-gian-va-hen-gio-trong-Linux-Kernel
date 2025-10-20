#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/timekeeping.h> // dùng cho ktime_get_real_ts64

#define TIMER_INTERVAL_SEC 1

static struct timer_list my_timer;

void timer_callback(struct timer_list *t)
{
    struct timespec64 ts;
    struct tm tm_time;

    // Lấy thời gian thực tại thời điểm ngắt
    ktime_get_real_ts64(&ts);
    time64_to_tm(ts.tv_sec, 0, &tm_time);

    pr_info("Timer interrupt at %02d:%02d:%02d | jiffies = %lu\n",
            tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec, jiffies);

    // Tái khởi động timer
    mod_timer(&my_timer, jiffies + TIMER_INTERVAL_SEC * HZ);
}

static int __init timer_module_init(void)
{
    pr_info("Loading timer interrupt module...\n");

    timer_setup(&my_timer, timer_callback, 0);
    mod_timer(&my_timer, jiffies + TIMER_INTERVAL_SEC * HZ);

    return 0;
}

static void __exit timer_module_exit(void)
{
    pr_info("Unloading timer interrupt module...\n");
    del_timer(&my_timer);
}

module_init(timer_module_init);
module_exit(timer_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Quoc");
MODULE_DESCRIPTION("Timer Interrupt with Real Time Clock Display");
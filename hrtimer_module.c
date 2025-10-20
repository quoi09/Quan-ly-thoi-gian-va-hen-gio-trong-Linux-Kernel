#include <linux/module.h>
#include <linux/init.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/timekeeping.h>
#include <linux/rtc.h>  // Để chuyển thời gian sang hh:mm:ss

#define TIMER_INTERVAL_NS 1e9 // 1 giây = 1 * 10^9 nanosecond

static struct hrtimer hr_timer;
static ktime_t kt_periode;

enum hrtimer_restart timer_callback(struct hrtimer *timer) //callback
{
struct timespec64 ts;
struct rtc_time tm;

```
// Lấy thời gian thực tại thời điểm ngắt
ktime_get_real_ts64(&ts);
rtc_time64_to_tm(ts.tv_sec, &tm); // Chuyển về định dạng giờ:phút:giây

pr_info("hrtimer interrupt at %02d:%02d:%02d | jiffies = %lu\\n",
        tm.tm_hour, tm.tm_min, tm.tm_sec, jiffies);

// Lập lại hẹn giờ
hrtimer_forward_now(timer, kt_periode);
return HRTIMER_RESTART;


}

static int __init hrtimer_init_module(void)
{
pr_info("hrtimer module loaded\n");

// Đặt chu kỳ timer
kt_periode = ktime_set(0, TIMER_INTERVAL_NS);

// Khởi tạo timer
hrtimer_init(&hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
hr_timer.function = timer_callback;

// Bắt đầu timer
hrtimer_start(&hr_timer, kt_periode, HRTIMER_MODE_REL);

return 0;


}

static void __exit hrtimer_exit_module(void)
{
pr_info("hrtimer module unloaded\n");
hrtimer_cancel(&hr_timer);
}

module_init(hrtimer_init_module);
module_exit(hrtimer_exit_module);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Quoc");
MODULE_DESCRIPTION("High-resolution timer interrupt module with hh:mm:ss logging");
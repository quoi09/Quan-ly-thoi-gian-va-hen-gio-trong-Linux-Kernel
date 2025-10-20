#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/jiffies.h>
#include <linux/timer.h>

static struct timer_list jiffies_timer;

static void jiffies_timer_callback(struct timer_list *timer) {
    printk(KERN_INFO "Jiffies hiện tại: %lu\n", jiffies);
    mod_timer(&jiffies_timer, jiffies + msecs_to_jiffies(1000));
}

static int __init jiffies_init(void) {
    printk(KERN_INFO "Khởi tạo Jiffies Timer Module\n");
    timer_setup(&jiffies_timer, jiffies_timer_callback, 0);
    mod_timer(&jiffies_timer, jiffies + msecs_to_jiffies(1000));
    return 0;
}

static void __exit jiffies_exit(void) {
    del_timer(&jiffies_timer);
    printk(KERN_INFO "Thoát Jiffies Timer Module\n");
}

module_init(jiffies_init);
module_exit(jiffies_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nhân");
MODULE_DESCRIPTION("In giá trị Jiffies mỗi giây");
#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <sys/wait.h>
#include <errno.h>

void run_rr_process(int id, int priority) {
    struct sched_param param;
    param.sched_priority = priority;
    if (sched_setscheduler(0, SCHED_RR, &param) == -1) {
        perror("Lỗi khi đặt lịch SCHED_RR");
        exit(EXIT_FAILURE);
    }
    printf("Tiến trình %d [PID: %d] bắt đầu với SCHED_RR, ưu tiên = %d\n", id, getpid(), priority);
    for (int i = 0; i < 3; i++) {
        printf("Tiến trình %d [PID: %d] đang chạy vòng lặp %d\n", id, getpid(), i + 1);
        usleep(500000);
    }
    printf("Tiến trình %d [PID: %d] kết thúc\n", id, getpid());
    exit(0);
}

void print_rr_timeslice() {
    FILE *ts_file = fopen("/proc/sys/kernel/sched_rr_timeslice_ms", "r");
    if (ts_file) {
        int timeslice;
        fscanf(ts_file, "%d", &timeslice);
        fclose(ts_file);
        printf("Timeslice của SCHED_RR hệ thống: %d ms\n", timeslice);
    } else {
        perror("Không thể đọc timeslice");
    }
}

void print_proc_sched_info(pid_t pid) {
    char path[64], buffer[512];
    snprintf(path, sizeof(path), "/proc/%d/sched", pid);
    FILE *f = fopen(path, "r");
    if (f) {
        printf("\nThông tin lập lịch của tiến trình PID %d \n", pid);
        while (fgets(buffer, sizeof(buffer), f)) {
            if (strstr(buffer, "policy") || strstr(buffer, "prio") || strstr(buffer, "sum_exec_runtime") || strstr(buffer, "nr_switches")) {
                printf("%s", buffer);
            }
        }
        fclose(f);
    } else {
        perror("Không thể mở /proc/[pid]/sched");
    }
}

void print_timer_interrupts() {
    FILE *intr_file = fopen("/proc/interrupts", "r");
    if (intr_file) {
        printf("\nThông tin ngắt timer (/proc/interrupts) \n");
        char line[1024];
        while (fgets(line, sizeof(line), intr_file)) {
            if (strstr(line, "timer") || strstr(line, "hrtimer")) {
                printf("%s", line);
            }
        }
        fclose(intr_file);
    } else {
        perror("Không thể mở /proc/interrupts");
    }
}

int main() {
    int priority = 20;
    pid_t pids[3];
    print_rr_timeslice();
    for (int i = 0; i < 3; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            run_rr_process(i + 1, priority);
        } else if (pid > 0) {
            pids[i] = pid;
        } else {
            perror("Lỗi khi fork tiến trình");
            exit(EXIT_FAILURE);
        }
    }
    sleep(1);
    for (int i = 0; i < 3; i++) {
        print_proc_sched_info(pids[i]);
    }
    print_timer_interrupts();
    for (int i = 0; i < 3; i++) {
        wait(NULL);
    }
    return 0;
}

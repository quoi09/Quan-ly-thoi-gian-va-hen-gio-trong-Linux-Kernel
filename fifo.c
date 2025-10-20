#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>
#include <errno.h>

#define SO_TIEN_TRINH 4
#define UU_TIEN 50  // Ưu tiên FIFO (1-99)
#define THOI_GIAN_LAM_VIEC_NS 200000000  // 200ms

void in_thong_tin_jiffies(pid_t pid) {
char path[256];
snprintf(path, sizeof(path), "/proc/%d/stat", pid);
FILE *fp = fopen(path, "r");
if (!fp) {
perror(" Không thể mở /proc/[pid]/stat");
return;
}


unsigned long utime, stime;
char comm[256], state;
int skip;
fscanf(fp,
       "%d %s %c", &skip, comm, &state); // đọc 3 đầu tiên
for (int i = 0; i < 11; i++) fscanf(fp, "%d", &skip); // bỏ qua đến cột 14
fscanf(fp, "%lu %lu", &utime, &stime);  // cột 14 và 15

printf("  [PID %d] jiffies (utime + stime): %lu\n", pid, utime + stime);
fclose(fp);



}

void in_thong_tin_interrupt() {
FILE *fp = fopen("/proc/interrupts", "r");
if (!fp) {
perror(" Không thể đọc /proc/interrupts");
return;
}


char line[256];
while (fgets(line, sizeof(line), fp)) {
    if (strstr(line, "timer") || strstr(line, "hrtimer")) {
        printf("    -> %s", line);
    }
}

fclose(fp);


}

//Hàm về FIFO
void run_fifo_process(int id, int priority) {
struct sched_param param;
param.sched_priority = priority;


if (sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
    perror(" Lỗi sched_setscheduler");
    exit(EXIT_FAILURE);
}

pid_t pid = getpid();
printf("Tiến trình %d [PID: %d] bắt đầu với SCHED_FIFO, ưu tiên = %d\n",
       id, pid, priority);
fflush(stdout);

// In thông tin hệ thống
char cmd[256];
snprintf(cmd, sizeof(cmd), "ps -o pid,comm,cls,pri,rtprio -p %d", pid);
system(cmd);

// Đo thời gian
struct timespec bat_dau, ket_thuc;
clock_gettime(CLOCK_MONOTONIC, &bat_dau);

// Mô phỏng công việc chiếm CPU
while (1) {
    clock_gettime(CLOCK_MONOTONIC, &ket_thuc);
    long elapsed_ns = (ket_thuc.tv_sec - bat_dau.tv_sec) * 1e9 +
                      (ket_thuc.tv_nsec - bat_dau.tv_nsec);
    if (elapsed_ns >= THOI_GIAN_LAM_VIEC_NS)
        break;
}

long tong_ns = (ket_thuc.tv_sec - bat_dau.tv_sec) * 1e9 +
               (ket_thuc.tv_nsec - bat_dau.tv_nsec);

printf("Tiến trình %d [PID: %d] kết thúc sau %.2f ms\n",
       id, pid, tong_ns / 1e6);

in_thong_tin_jiffies(pid);
printf("  [PID %d] Ngắt timer/hrtimmer liên quan:\n", pid);
in_thong_tin_interrupt();

fflush(stdout);



}

int main() {
long hz = sysconf(_SC_CLK_TCK);
printf("Cấu hình HZ (tick/s): %ld\n", hz);
printf("Bắt đầu mô phỏng FIFO với %d tiến trình.\n", SO_TIEN_TRINH);
int dem=UU_TIEN;
for (int i = 0; i < SO_TIEN_TRINH; ++i) {
dem--;
pid_t pid = fork();

    if (pid == 0) {
        run_fifo_process(i + 1, dem);
        exit(0);
    } else if (pid > 0) {
        waitpid(pid, NULL, 0);
    } else {
        perror(" Lỗi ");
        exit(EXIT_FAILURE);
    }
}

printf("Tất cả tiến trình đã hoàn thành.\n");
return 0;


}
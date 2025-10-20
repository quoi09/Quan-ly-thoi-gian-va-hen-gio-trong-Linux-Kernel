#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>

#define N_LOOP 2
#define N_PROC 2

const char* policy_to_string(int policy) {
switch (policy) {
case SCHED_OTHER: return "SCHED_OTHER";
case SCHED_BATCH: return "SCHED_BATCH";
case SCHED_IDLE: return "SCHED_IDLE";
case SCHED_FIFO: return "SCHED_FIFO";
case SCHED_RR: return "SCHED_RR";
default: return "UNKNOWN";
}
}

void print_vruntime(pid_t pid) {
char path[128];
snprintf(path, sizeof(path), "/proc/%d/sched", pid);
FILE *fp = fopen(path, "r");
if (!fp) return;

```
char line[256];
while (fgets(line, sizeof(line), fp)) {
    if (strstr(line, "se.vruntime")) {
        printf("  [PID %d] vruntime: %s", pid, line);
        break;
    }
}
fclose(fp);

```

}

void print_jiffies(pid_t pid) {
FILE *fp = fopen("/proc/timer_list", "r");
if (!fp) return;
char line[256];
while (fgets(line, sizeof(line), fp)) {
if (strstr(line, "jiffies:")) {
printf("  [PID %d] jiffies: %s", pid, line);
break;
}
}
fclose(fp);
}

void print_timer_interrupt(pid_t pid) {
FILE *fp = fopen("/proc/interrupts", "r");
if (!fp) return;
char line[256];
while (fgets(line, sizeof(line), fp)) {
if (strstr(line, "timer")) {
printf("  [PID %d] Ngắt timer: %s", pid, line);
break;
}
}
fclose(fp);
}

void print_thread_cputime(pid_t pid) {
struct timespec t;
clock_gettime(CLOCK_THREAD_CPUTIME_ID, &t);
printf("  [PID %d] Thời gian CPU thread: %.6f giây\n", pid, t.tv_sec + t.tv_nsec / 1e9);
}

void print_sched_info(pid_t pid) {
int policy = sched_getscheduler(pid);
printf("  [PID %d] Chính sách lịch trình: %d (%s)\n", pid, policy, policy_to_string(policy));
}

void thuc_hien_congviec(const char *label) {
pid_t pid = getpid();
print_sched_info(pid);
printf("\n[PID %d]  Bắt đầu tiến trình %s (Policy: %s)   n", pid, label, policy_to_string(sched_getscheduler(pid)));

```
for (int i = 0; i < N_LOOP; i++) {
    struct timespec t1, t2;
    clock_gettime(CLOCK_REALTIME, &t1);
    printf("\\n  [PID %d] Vòng lặp %d bắt đầu: %lf\\n", pid, i + 1, t1.tv_sec + t1.tv_nsec / 1e9);

    usleep(200000); // 200ms workload

    clock_gettime(CLOCK_REALTIME, &t2);
    printf("  [PID %d] Vòng lặp %d kết thúc: %lf\\n", pid, i + 1, t2.tv_sec + t2.tv_nsec / 1e9);

    print_vruntime(pid);
    print_thread_cputime(pid);
    print_jiffies(pid);
    print_timer_interrupt(pid);

    printf("--------------------------------------------------\\n");
}

printf("\\n[PID %d]  Kết thúc tiến trình %s   \\n", pid, label);

```

}

int tao_tien_trinh_batch(const char *label) {
pid_t pid = fork();
if (pid == 0) {
// Đặt policy SCHED_BATCH
struct sched_param sp;
sp.sched_priority = 0;
if (sched_setscheduler(0, SCHED_BATCH, &sp) < 0) {
perror("sched_setscheduler (SCHED_BATCH) thất bại");
exit(1);
}

```
    thuc_hien_congviec(label);
    exit(0);
}
return pid;

```

}

int main() {
printf("  BẮT ĐẦU LỊCH TRÌNH BATCH (SCHED_BATCH)    \n\n");

```
pid_t pids[N_PROC];
for (int i = 0; i < N_PROC; i++) {
    char label[16];
    snprintf(label, sizeof(label), "BATCH_%d", i + 1);
    pids[i] = tao_tien_trinh_batch(label);
}

for (int i = 0; i < N_PROC; i++) {
    waitpid(pids[i], NULL, 0);
    printf("<< Đã kết thúc tiến trình BATCH_%d (PID %d) >>\\n", i + 1, pids[i]);
}

printf("\\n  KẾT THÚC LỊCH TRÌNH BATCH   \\n");
return 0;

```

}
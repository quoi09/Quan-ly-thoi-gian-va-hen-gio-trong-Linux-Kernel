🧭 ĐỀ TÀI: QUẢN LÝ THỜI GIAN VÀ HẸN GIỜ TRONG LINUX KERNEL
Giới thiệu

Trong nhân (kernel) của hệ điều hành Linux, bộ lập lịch (scheduler) và các cơ chế hẹn giờ (timer) đóng vai trò cốt lõi trong việc quản lý thời gian thực thi của các tiến trình.
Đề tài này tập trung nghiên cứu các khía cạnh:

Quản lý thời gian CPU thông qua các thuật toán lập lịch như FIFO, Round Robin, CFS.

Cơ chế jiffies, hrtimer, và interrupt timer trong kernel.

Mối liên hệ giữa ngắt thời gian (interrupt) và việc đo đạc hoạt động của tiến trình.

Thư mục chứa các tệp mã nguồn mô phỏng và minh họa:

cfs.c
fifo.c
hrtimer_interrupt_module.c
hrtimer_module.c
hrtimer_vs_jiffies.c
jiffies_timer.c
round_robin.c


Mỗi tệp tương ứng với một mô hình hoặc thuật toán khác nhau trong quản lý thời gian.
ví dụ hướng dẫn chạy fifo.c

1. Mục tiêu của chương trình fifo.c

File fifo.c mô phỏng cơ chế lập lịch SCHED_FIFO (First-In-First-Out) của Linux Real-Time Scheduler.

Cụ thể:

Tạo ra nhiều tiến trình con (số lượng do hằng SO_TIEN_TRINH xác định).

Gán cho mỗi tiến trình một độ ưu tiên (priority) khác nhau trong phạm vi 1–99.

Đo thời gian thực thi và ghi nhận giá trị jiffies của từng tiến trình.

Đọc thông tin về ngắt timer và hrtimer từ /proc/interrupts.

2. Nội dung chính của mã nguồn

Chương trình bao gồm:

Hàm run_fifo_process() — thiết lập và thực thi tiến trình theo SCHED_FIFO.

Hàm in_thong_tin_jiffies() — đọc thông tin CPU time (utime + stime) từ /proc/[pid]/stat.

Hàm in_thong_tin_interrupt() — hiển thị các dòng liên quan đến timer trong /proc/interrupts.

Hàm main() — quản lý tạo tiến trình con, đo thời gian, và in kết quả.

3. Hướng dẫn chạy ví dụ fifo.c trên Ubuntu
Bước 1: Mở terminal
Ctrl + Alt + T

Bước 2: Tạo file mã nguồn

Mở nano và dán toàn bộ nội dung code:

nano fifo.c


Dán đoạn code bạn có ở trên, rồi lưu lại:

Ctrl + O → Enter
Ctrl + X

Bước 3: Biên dịch chương trình
gcc fifo.c -o fifo -lrt


Tùy chọn -lrt giúp liên kết thư viện real-time (cần cho clock_gettime).

Bước 4: Chạy chương trình với quyền root

Bởi vì việc thay đổi scheduler policy (SCHED_FIFO) yêu cầu quyền cao:

sudo ./fifo

Bước 5: Kết quả 
<img width="665" height="435" alt="image" src="https://github.com/user-attachments/assets/067290ab-831f-46f5-ae03-d3afe39b9c61" />
<img width="678" height="406" alt="image" src="https://github.com/user-attachments/assets/31bfa60a-40f3-4609-ab6a-0a1e5a2295a0" />



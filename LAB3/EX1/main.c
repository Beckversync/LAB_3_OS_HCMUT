#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "seqlock.h"

// Khai báo biến toàn cục
pthread_seqlock_t lock; // Sequence lock

int main() {
    int val = 0; // Biến dùng trong vùng găng (critical section)

    // Khởi tạo sequence lock
    pthread_seqlock_init(&lock);

    // Writer cập nhật giá trị
    pthread_seqlock_wrlock(&lock);
    val++; // Tăng giá trị của val
    pthread_seqlock_wrunlock(&lock);

    // Reader đọc giá trị
    if (pthread_seqlock_rdlock(&lock)) { // Kiểm tra nếu reader lấy được lock
        printf("val = %d\n", val); // In giá trị val
        pthread_seqlock_rdunlock(&lock); // Nhả khóa reader
    } else {
        printf("Reader lock failed due to writer activity.\n");
    }

    return 0;
}

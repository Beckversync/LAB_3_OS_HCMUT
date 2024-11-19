#ifndef SEQLOCK_H
#define SEQLOCK_H

#include <pthread.h>
#include <stdio.h>

// Cấu trúc pthread_seqlock
typedef struct pthread_seqlock {
    pthread_mutex_t lock;  // Mutex đảm bảo chỉ có một writer truy cập
    unsigned int sequence; // Sequence để đồng bộ giữa reader và writer
} pthread_seqlock_t;

// Khởi tạo sequence lock
static inline void pthread_seqlock_init(pthread_seqlock_t *rw) {
    pthread_mutex_init(&rw->lock, NULL);
    rw->sequence = 0; // Khởi tạo sequence với giá trị chẵn
}

// Writer lấy khóa
static inline void pthread_seqlock_wrlock(pthread_seqlock_t *rw) {
    pthread_mutex_lock(&rw->lock); // Lấy mutex để đảm bảo độc quyền
    rw->sequence++;               // Tăng sequence lên số lẻ (bắt đầu ghi)
    printf("[Writer] Lock acquired. Sequence: %u\n", rw->sequence);
}

// Writer nhả khóa
static inline void pthread_seqlock_wrunlock(pthread_seqlock_t *rw) {
    rw->sequence++;               // Tăng sequence lên số chẵn (kết thúc ghi)
    pthread_mutex_unlock(&rw->lock); // Nhả mutex
    printf("[Writer] Lock released. Sequence: %u\n", rw->sequence);
}

// Reader lấy khóa
static inline int pthread_seqlock_rdlock(pthread_seqlock_t *rw) {
    unsigned seq;
    do {
        seq = __atomic_load_n(&rw->sequence, __ATOMIC_ACQUIRE); // Đọc giá trị sequence
        if (seq % 2 != 0) {                                    // Nếu sequence là lẻ, writer đang ghi
            printf("[Reader] Writer active, waiting...\n");
            return 0; // Thất bại khi có writer
        }
    } while (seq != __atomic_load_n(&rw->sequence, __ATOMIC_ACQUIRE)); // Đảm bảo không bị ghi giữa chừng
    printf("[Reader] Lock acquired. Sequence: %u\n", seq);
    return 1; // Thành công
}

// Reader nhả khóa
static inline void pthread_seqlock_rdunlock(pthread_seqlock_t *rw) {
    printf("[Reader] Lock released.\n");
}

#endif // SEQLOCK_H

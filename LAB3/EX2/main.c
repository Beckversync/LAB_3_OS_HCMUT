#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>   // Thêm header cho waitpid
#include "utils.h"

struct appconfig appconf;  // Define appconf here (only once)
long sumbuf = 0;          // Global sum buffer
int *shrdarrbuf;          // Shared array buffer
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;  // Mutex để bảo vệ bộ đệm chia sẻ

// Định nghĩa hàm sum_worker
void* sum_worker(void* arg) {
    struct _range* idx_range = (struct _range*)arg;
    long local_sum = 0;
    for (int i = idx_range->start; i <= idx_range->end; i++) {
        local_sum += shrdarrbuf[i];
    }

    // Lock mutex để cập nhật biến toàn cục
    pthread_mutex_lock(&mtx);
    sumbuf += local_sum;
    pthread_mutex_unlock(&mtx);

    return NULL;
}

long validate_sum(int arraysize) {
    long validsum = 0;
    for (int i = 0; i < arraysize; i++) {
        validsum += shrdarrbuf[i];
    }
    return validsum;
}

int main(int argc, char *argv[]) {
    int i, pid;
    struct _range *thread_idx_range;
    pthread_t *tid;

    if (argc < 3 || argc > 4) {
        help(EXIT_SUCCESS);  // Hiển thị trợ giúp nếu tham số không hợp lệ
    }

    processopts(argc, argv, &appconf);  // Xử lý tham số từ dòng lệnh

    fprintf(stdout, "%s runs with %s=%d \t %s=%d \t %s=%d\n", PACKAGE,
            ARG1, appconf.arrsz, ARG2, appconf.tnum, ARG3, appconf.seednum);

    thread_idx_range = malloc(appconf.tnum * sizeof(struct _range));
    if (thread_idx_range == NULL) {
        printf("Error! Memory for index storage not allocated.\n");
        exit(-1);
    }

    if (validate_and_split_argarray(appconf.arrsz, appconf.tnum, thread_idx_range) < 0) {
        printf("Error! Array index not splitable.\n");
        exit(-1);
    }

    // Sinh mảng dữ liệu
    shrdarrbuf = malloc(appconf.arrsz * sizeof(int));
    if (shrdarrbuf == NULL) {
        printf("Error! Memory for array buffer not allocated.\n");
        exit(-1);
    }

    if (generate_array_data(shrdarrbuf, appconf.arrsz, appconf.seednum) < 0) {
        printf("Error! Array generation failed.\n");
        exit(-1);
    }

    pid = fork();
    if (pid < 0) {
        printf("Error! Fork failed.\n");
        exit(-1);
    }

    if (pid == 0) {  // Quá trình con: Tính tổng chuẩn
        printf("Sequence sum result: %ld\n", validate_sum(appconf.arrsz));
        exit(0);
    }

    // Quá trình cha: Tạo các thread để tính tổng từng phần
    tid = malloc(appconf.tnum * sizeof(pthread_t));
    for (i = 0; i < appconf.tnum; i++) {
        pthread_create(&tid[i], NULL, sum_worker, (void *)&thread_idx_range[i]);  // Ép kiểu thành void*
    }

    for (i = 0; i < appconf.tnum; i++) {
        pthread_join(tid[i], NULL);
    }

    printf("%s gives sum result: %ld\n", PACKAGE, sumbuf);
    waitpid(pid, NULL, 0);  // Đợi quá trình con kết thúc

    free(thread_idx_range);
    free(tid);
    free(shrdarrbuf);

    return 0;
}

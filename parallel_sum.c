#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <pthread.h>
#include <time.h>

typedef struct {
    const int* data;
    size_t start, end;
    uint64_t result;
} ThreadArgs;

void* worker(void* arg) {
    ThreadArgs* args = arg;
    uint64_t sum = 0;
    for (size_t i = args->start; i < args->end; i++) {
        sum += args->data[i];
    }
    args->result = sum;
    return NULL;
}

double elapsed(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec)
        + (end.tv_nsec - start.tv_nsec) / 1e9;
}

static uintmax_t parse_positive(const char* s) {
    if (*s < '0' || *s > '9') return 0;
    char* end;
    errno = 0;
    uintmax_t value = strtoumax(s, &end, 10);
    return (errno || *end != '\0') ? 0 : value;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <elements> <threads>\n", argv[0]);
        return 1;
    }
    uintmax_t count = parse_positive(argv[1]);
    uintmax_t workers = parse_positive(argv[2]);
    if (count == 0 || workers == 0 || workers > 64 ||
        count < workers || count > SIZE_MAX / sizeof(int) ||
        count > UINT64_MAX / 99) {
        fprintf(stderr, "Invalid arguments\n");
        return 1;
    }
    size_t n = (size_t)count;
    int num_threads = (int)workers;
    int* data = malloc(n * sizeof(int));
    if (!data) {
        fprintf(stderr, "Allocation failed\n");
        return 1;
    }
    for (size_t i = 0; i < n; i++) data[i] = (int)(i % 100);

    pthread_t threads[64];
    ThreadArgs args[64];
    size_t chunk = n / (size_t)num_threads;
    struct timespec start, end;
    if (clock_gettime(CLOCK_MONOTONIC, &start) != 0) {
        perror("clock_gettime");
        free(data);
        return 1;
    }
    for (int i = 0; i < num_threads; i++) {
        args[i].data = data;
        args[i].start = (size_t)i * chunk;
        args[i].end = (i == num_threads - 1) ? n : (size_t)(i + 1) * chunk;
        args[i].result = 0;
        if (pthread_create(&threads[i], NULL, worker, &args[i]) != 0) {
            fprintf(stderr, "Thread creation failed\n");
            exit(EXIT_FAILURE);
        }
    }
    uint64_t total = 0;
    for (int i = 0; i < num_threads; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            fprintf(stderr, "Thread join failed\n");
            exit(EXIT_FAILURE);
        }
        total += args[i].result;
    }
    if (clock_gettime(CLOCK_MONOTONIC, &end) != 0) {
        perror("clock_gettime");
        free(data);
        return 1;
    }
    printf("Elements: %zu\nThreads: %d\nResult: %" PRIu64 "\nTime: %.6f seconds\n",
           n, num_threads, total, elapsed(start, end));
    free(data);
    return 0;
}

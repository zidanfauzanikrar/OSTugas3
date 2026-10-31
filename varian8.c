#define _GNU_SOURCE
#include "varian8.h"
#include "memstat.h" // Header dari Anggota 2
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/stat.h>

// Helper ringkas untuk mengambil nilai VmRSS via modul Anggota 2
static long get_current_vmrss_kb(void) {
    long vmsize = 0, vmrss = 0;
    if (read_vm_metrics(&vmsize, &vmrss) == 0) {
        return vmrss;
    }
    return 0;
}

// Helper menghitung selisih waktu dalam detik (double presisi tinggi)
static double calculate_elapsed_seconds(struct timespec start, struct timespec end) {
    return (double)(end.tv_sec - start.tv_sec) + 
           (double)(end.tv_nsec - start.tv_nsec) / 1000000000.0;
}

// 1. Membuat berkas biner 50 MB terisi byte 1 (0x01)
static int prepare_binary_file(const char *filename, size_t size) {
    printf("[Info] Menyiapkan berkas biner %zu bytes...\n", size);

    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("Error open berkas uji");
        return -1;
    }

    size_t chunk_size = 64 * 1024;
    uint8_t *chunk_buf = malloc(chunk_size);
    if (!chunk_buf) {
        perror("Gagal malloc chunk pembuatan berkas");
        close(fd);
        return -1;
    }
    memset(chunk_buf, 1, chunk_size);

    size_t total_written = 0;
    while (total_written < size) {
        size_t to_write = (size - total_written > chunk_size) ? chunk_size : (size - total_written);
        ssize_t written = write(fd, chunk_buf, to_write);
        if (written <= 0) {
            perror("Error saat write berkas uji");
            free(chunk_buf);
            close(fd);
            return -1;
        }
        total_written += written;
    }

    free(chunk_buf);
    close(fd);
    return 0;
}

// 2. Metode 1: read() & malloc
static void execute_method1_read(const char *filename, size_t size, uint64_t *out_sum, double *out_time, long *out_rss_diff) {
    struct timespec start_time, end_time;

    long rss_before = get_current_vmrss_kb();
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    uint8_t *buffer = (uint8_t *)malloc(size);
    if (!buffer) {
        perror("Gagal malloc pada Metode 1");
        return;
    }

    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("Gagal open pada Metode 1");
        free(buffer);
        return;
    }

    size_t total_read = 0;
    while (total_read < size) {
        ssize_t bytes = read(fd, buffer + total_read, size - total_read);
        if (bytes <= 0) break;
        total_read += bytes;
    }
    close(fd);

    uint64_t sum = 0;
    for (size_t i = 0; i < total_read; i++) {
        sum += buffer[i];
    }

    clock_gettime(CLOCK_MONOTONIC, &end_time);
    long rss_after = get_current_vmrss_kb();

    *out_sum = sum;
    *out_time = calculate_elapsed_seconds(start_time, end_time);
    *out_rss_diff = rss_after - rss_before;

    free(buffer);
}

// 3. Metode 2: mmap()
static void execute_method2_mmap(const char *filename, size_t size, uint64_t *out_sum, double *out_time, long *out_rss_diff) {
    struct timespec start_time, end_time;

    long rss_before = get_current_vmrss_kb();
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("Gagal open pada Metode 2");
        return;
    }

    uint8_t *mapped_data = (uint8_t *)mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (mapped_data == MAP_FAILED) {
        perror("Gagal mmap pada Metode 2");
        close(fd);
        return;
    }

    uint64_t sum = 0;
    for (size_t i = 0; i < size; i++) {
        sum += mapped_data[i];
    }

    clock_gettime(CLOCK_MONOTONIC, &end_time);
    long rss_after = get_current_vmrss_kb();

    *out_sum = sum;
    *out_time = calculate_elapsed_seconds(start_time, end_time);
    *out_rss_diff = rss_after - rss_before;

    munmap(mapped_data, size);
    close(fd);
}

// Fungsi utama Varian 8
void run_varian8(void) {
    printf("\n--- Varian 8: Pemetaan Berkas vs System Call I/O (Sequential Sum) ---\n");

    const char *file_path = VARIAN8_FILE_NAME;
    size_t file_size = VARIAN8_FILE_SIZE;

    if (prepare_binary_file(file_path, file_size) != 0) {
        printf("[Error] Gagal menyiapkan berkas biner.\n");
        return;
    }

    uint64_t sum1 = 0, sum2 = 0;
    double time1 = 0.0, time2 = 0.0;
    long rss_diff1 = 0, rss_diff2 = 0;

    // Eksekusi Metode 1: read() & malloc
    execute_method1_read(file_path, file_size, &sum1, &time1, &rss_diff1);
    printf("\n[Metode 1: read() & malloc]\n");
    printf("Waktu Eksekusi : %.6f detik\n", time1);
    printf("Total Sum      : %lu\n", (unsigned long)sum1);
    printf("Lonjakan VmRSS : %ld KB\n", rss_diff1 > 0 ? rss_diff1 : 0);

    // Eksekusi Metode 2: mmap()
    execute_method2_mmap(file_path, file_size, &sum2, &time2, &rss_diff2);
    printf("\n[Metode 2: mmap()]\n");
    printf("Waktu Eksekusi : %.6f detik\n", time2);
    printf("Total Sum      : %lu\n", (unsigned long)sum2);
    printf("Lonjakan VmRSS : %ld KB\n", rss_diff2 > 0 ? rss_diff2 : 0);

    // Validasi kesimpulan
    printf("\n[Kesimpulan] ");
    if (sum1 == sum2 && sum1 == (uint64_t)file_size) {
        printf("Total sum sama (%lu), Kedua metode memproses data yang sama secara valid.\n\n", (unsigned long)sum1);
    } else {
        printf("Peringatan: Total sum tidak cocok (Sum1: %lu vs Sum2: %lu)\n\n", 
               (unsigned long)sum1, (unsigned long)sum2);
    }

    unlink(file_path);
}
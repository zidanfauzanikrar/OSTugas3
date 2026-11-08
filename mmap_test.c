#define _GNU_SOURCE
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#include "logger.h"
#include "memstat.h"
#include "mmap_test.h"

/* Kompatibilitas jika MAP_ANONYMOUS bernama MAP_ANON di sistem tertentu */
#ifndef MAP_ANONYMOUS
#ifdef MAP_ANON
#define MAP_ANONYMOUS MAP_ANON
#endif
#endif

/* Buang sisa input di buffer stdin sampai akhir baris. */
static void flush_stdin(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

void run_mmap_test(void)
{
    long vs_before, rss_before, vs_after, rss_after;
    long long n;

    log_open("mmap");
    log_printf("You picked mmap/munmap:\n");

    /* 1. Metrik SEBELUM alokasi */
    if (read_vm_metrics(&vs_before, &rss_before) != 0) {
        log_printf("Gagal membaca /proc/self/status.\n");
        log_close();
        return;
    }
    log_printf("VmSize Before: %ld KB | VmRSS Before: %ld KB\n",
               vs_before, rss_before);

    /* 2. Minta jumlah integer. Prompt hanya ke terminal, nilai dicatat ke log. */
    printf("How many integers to allocate: ");
    fflush(stdout);
    if (scanf("%lld", &n) != 1 || n <= 0 || n > INT_MAX) {
        flush_stdin();
        log_printf("\nInput tidak valid (harus 1 sampai %d).\n", INT_MAX);
        log_close();
        return;
    }
    flush_stdin();
    log_record("How many integers to allocate: %lld\n", n);

    size_t count = (size_t)n;
    if (count > SIZE_MAX / sizeof(int)) {
        log_printf("\nUkuran terlalu besar.\n");
        log_close();
        return;
    }

    size_t total_bytes = count * sizeof(int);

    /* 3. Alokasi langsung ke kernel via system call mmap anonymous */
    int *arr = (int *)mmap(NULL, total_bytes, PROT_READ | PROT_WRITE,
                           MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (arr == MAP_FAILED) {
        log_printf("\nmmap gagal mengalokasikan %zu byte.\n", total_bytes);
        log_close();
        return;
    }

    /*
     * 4. Isi array dengan 1..n.
     * Saat mmap dipanggil, kernel hanya membuat VMA (Virtual Memory Area)
     * tanpa mengalokasikan RAM fisik secara langsung (Demand Paging / Lazy Allocation).
     * Saat penulisan arr[i] dilakukan, terjadi page fault (minor fault) yang
     * memaksa kernel mengalokasikan physical frame (RAM), sehingga VmRSS meningkat.
     */
    for (size_t i = 0; i < count; i++)
        arr[i] = (int)(i + 1);

    log_printf("\nStarting memory address: %p\n", (void *)arr);
    log_printf("Printing first 10 values in reverse order:\n");
    for (size_t k = 0; k < 10 && k < count; k++)
        log_printf("%d\n", arr[count - 1 - k]);

    /* 5. Metrik SESUDAH alokasi dan penulisan (sebelum munmap) */
    if (read_vm_metrics(&vs_after, &rss_after) == 0) {
        log_printf("\nVmSize After: %ld KB | VmRSS After: %ld KB\n",
                   vs_after, rss_after);
    } else {
        log_printf("\nGagal membaca metrik sesudah alokasi.\n");
    }

    /* 6. Dealokasi memori kembali ke kernel */
    munmap(arr, total_bytes);
    log_close();
}

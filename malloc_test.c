#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "logger.h"
#include "malloc_test.h"
#include "memstat.h"

/* Buang sisa input di buffer stdin sampai akhir baris. */
static void flush_stdin(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

void run_malloc_test(void)
{
    long vs_before, rss_before, vs_after, rss_after;
    long long n;

    log_open("malloc");
    log_printf("You picked malloc/free.\n");

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

    /* 3. Alokasi lewat C library */
    int *arr = malloc(count * sizeof(int));
    if (arr == NULL) {
        log_printf("\nmalloc gagal mengalokasikan %zu byte.\n",
                   count * sizeof(int));
        log_close();
        return;
    }

    /*
     * 4. Isi array dengan 1..n. Menulis ke setiap elemen memaksa kernel
     * menyediakan halaman fisik (page fault), sehingga VmRSS ikut naik.
     * Tanpa langkah ini VmSize naik tetapi VmRSS hampir tidak berubah.
     */
    for (size_t i = 0; i < count; i++)
        arr[i] = (int)(i + 1);

    log_printf("\nStarting memory address: %p\n", (void *)arr);
    log_printf("Printing first 10 values in reverse order:\n");
    for (size_t k = 0; k < 10 && k < count; k++)
        log_printf("%d\n", arr[count - 1 - k]);

    /* 5. Metrik SESUDAH alokasi (sebelum free) */
    if (read_vm_metrics(&vs_after, &rss_after) == 0) {
        log_printf("\nVmSize After: %ld KB | VmRSS After: %ld KB\n",
                   vs_after, rss_after);
    } else {
        log_printf("\nGagal membaca metrik sesudah alokasi.\n");
    }

    free(arr);
    log_close();
}

#ifndef MALLOC_TEST_H
#define MALLOC_TEST_H

/*
 * Menu 1: alokasi blok integer dengan malloc/free.
 *
 * Alur: cetak VmSize/VmRSS sebelum, minta jumlah integer, malloc, isi
 * array dengan 1..n, cetak alamat awal dan 10 nilai terakhir secara
 * terbalik, cetak VmSize/VmRSS sesudah, lalu free.
 * Semua output ikut dicatat lewat logger (log_open/log_printf/log_close).
 */
void run_malloc_test(void);

#endif /* MALLOC_TEST_H */

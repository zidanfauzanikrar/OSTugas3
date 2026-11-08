#ifndef MMAP_TEST_H
#define MMAP_TEST_H

/*
 * Menu 2: alokasi blok integer dengan mmap/munmap (anonymous mapping).
 *
 * Alur: cetak VmSize/VmRSS sebelum, minta jumlah integer, mmap anonymous,
 * isi array dengan 1..n, cetak alamat awal dan 10 nilai terakhir secara
 * terbalik, cetak VmSize/VmRSS sesudah, lalu munmap.
 * Seluruh output dicatat lewat modul logger (log_open/log_printf/log_close).
 */
void run_mmap_test(void);

#endif /* MMAP_TEST_H */

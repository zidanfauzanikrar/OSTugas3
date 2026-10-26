#ifndef LOGGER_H
#define LOGGER_H

/*
 * KONTRAK LOGGER (implementasi asli milik Anggota 4).
 *
 * Anggota lain cukup memanggil fungsi di bawah ini. Anggota 4 yang
 * menentukan isinya: membuat dua berkas log
 *     XX-[metode]_std-[timestamp].txt   (fopen/fprintf/fclose)
 *     XX-[metode]_sys-[timestamp].txt   (open/write/close)
 */

/* Membuka kedua berkas log. method = "malloc" atau "mmap". */
void log_open(const char *method);

/* Cetak ke terminal DAN tulis ke kedua berkas log. Format seperti printf. */
void log_printf(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

/* Tulis ke kedua berkas log SAJA (tidak ke terminal). Format seperti printf.
 * Dipakai untuk mencatat input pengguna yang sudah tampil lewat echo terminal. */
void log_record(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

/* Menutup kedua berkas log. */
void log_close(void);

#endif /* LOGGER_H */

#ifndef MEMSTAT_H
#define MEMSTAT_H

/*
 * Membaca VmSize dan VmRSS (dalam KB) dari /proc/self/status.
 *
 * Mengembalikan 0 jika berhasil, -1 jika gagal (berkas tidak bisa dibuka
 * atau salah satu field tidak ditemukan). Nilai output hanya diisi saat
 * fungsi mengembalikan 0.
 *
 * Dipakai oleh: menu malloc (Anggota 2), menu mmap (Anggota 3),
 * dan Varian 8 (Anggota 5).
 */
int read_vm_metrics(long *vmsize_kb, long *vmrss_kb);

#endif /* MEMSTAT_H */

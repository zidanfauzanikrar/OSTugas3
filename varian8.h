#ifndef VARIAN8_H
#define VARIAN8_H

#define VARIAN8_FILE_NAME "temp_varian8_50MB.bin"
#define VARIAN8_FILE_SIZE 52428800L // Tepat 50 MB (50 * 1024 * 1024 bytes)

/**
 * Menjalankan seluruh alur eksperimen Varian 8:
 * 1. Menyiapkan berkas biner 50 MB terisi byte bernilai 1.
 * 2. Metode 1: read() via system call + buffer malloc().
 * 3. Metode 2: mmap() pemetaan berkas ke virtual address space.
 * 4. Menghitung waktu (clock_gettime), total sum, dan lonjakan VmRSS.
 * 5. Menampilkan kesimpulan validasi dan membersihkan berkas uji.
 */
void run_varian8(void);

#endif // VARIAN8_H
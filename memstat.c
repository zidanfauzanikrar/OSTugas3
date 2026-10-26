#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "memstat.h"

#define STATUS_BUF_SIZE 8192

/*
 * Mengambil angka (KB) dari baris "<key>:   <angka> kB" di dalam teks status.
 * Mengembalikan 0 jika ketemu, -1 jika tidak.
 */
static int parse_field(const char *text, const char *key, long *out)
{
    const char *p = strstr(text, key);
    if (p == NULL)
        return -1;

    p += strlen(key);               /* lompati nama field, misal "VmSize:" */
    char *end = NULL;
    long value = strtol(p, &end, 10); /* strtol melewati spasi di depan */
    if (end == p)
        return -1;

    *out = value;
    return 0;
}

int read_vm_metrics(long *vmsize_kb, long *vmrss_kb)
{
    /*
     * Sengaja memakai open/read/close dan buffer di stack, bukan fopen.
     * fopen memanggil malloc untuk buffer FILE, sehingga pembacaan metrik
     * itu sendiri bisa ikut mengubah VmSize/VmRSS yang sedang diukur.
     */
    char buf[STATUS_BUF_SIZE];
    size_t total = 0;

    if (vmsize_kb == NULL || vmrss_kb == NULL)
        return -1;

    int fd = open("/proc/self/status", O_RDONLY);
    if (fd < 0)
        return -1;

    /* /proc dapat mengembalikan data dalam beberapa read(), jadi loop. */
    while (total < sizeof(buf) - 1) {
        ssize_t n = read(fd, buf + total, sizeof(buf) - 1 - total);
        if (n < 0) {
            close(fd);
            return -1;
        }
        if (n == 0)
            break;
        total += (size_t)n;
    }
    close(fd);
    buf[total] = '\0';

    long vs, rss;
    if (parse_field(buf, "VmSize:", &vs) != 0)
        return -1;
    if (parse_field(buf, "VmRSS:", &rss) != 0)
        return -1;

    *vmsize_kb = vs;
    *vmrss_kb = rss;
    return 0;
}

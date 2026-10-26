/*
 * STUB SEMENTARA untuk logger.h, hanya untuk tes lokal Anggota 2.
 * Jangan ikut di-zip. Ganti dengan logger.c milik Anggota 4 saat integrasi.
 */
#include <stdarg.h>
#include <stdio.h>

#include "logger.h"

void log_open(const char *method)
{
    (void)method;
}

void log_printf(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
}

void log_record(const char *fmt, ...)
{
    (void)fmt; /* stub: tidak menulis ke berkas */
}

void log_close(void)
{
}

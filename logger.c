#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
 
#include "logger.h"
 

#ifndef LOG_PREFIX
#define LOG_PREFIX "A08"
#endif
 
#define LOG_LINE_MAX 1024
 
static FILE *log_std = NULL; /* handle stdio */
static int log_sys = -1;     /* file descriptor system call */
 

static void write_to_files(const char *buf, size_t len)
{
    if (log_std != NULL)
        fprintf(log_std, "%s", buf);
 
    if (log_sys >= 0) {
        size_t done = 0;

        while (done < len) {
            ssize_t w = write(log_sys, buf + done, len - done);
 
            if (w < 0) {
                if (errno == EINTR)
                    continue;
                break;
            }
            done += (size_t)w;
        }
    }
}
 
void log_open(const char *method)
{
    char stamp[32];
    char path[128];
    time_t now = time(NULL);
    struct tm tm_now;
 
    log_close();
 
    localtime_r(&now, &tm_now);
    strftime(stamp, sizeof stamp, "%Y%m%d-%H%M%S", &tm_now);
 
    /* Metode 1: Standard C Library. */
    snprintf(path, sizeof path, "%s-%s_std-%s.txt", LOG_PREFIX, method, stamp);
    log_std = fopen(path, "w");
    if (log_std == NULL)
        fprintf(stderr, "[logger] gagal membuka %s: %s\n", path,
                strerror(errno));
 
    /* Metode 2: System call. */
    snprintf(path, sizeof path, "%s-%s_sys-%s.txt", LOG_PREFIX, method, stamp);
    log_sys = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (log_sys < 0)
        fprintf(stderr, "[logger] gagal membuka %s: %s\n", path,
                strerror(errno));
}
 
void log_printf(const char *fmt, ...)
{
    char buf[LOG_LINE_MAX];
    va_list ap;
    int n;
 
    va_start(ap, fmt);
    n = vsnprintf(buf, sizeof buf, fmt, ap);
    va_end(ap);
    if (n < 0)
        return;
    if ((size_t)n >= sizeof buf)
        n = (int)sizeof buf - 1; 
 
    fputs(buf, stdout);
    write_to_files(buf, (size_t)n);
}
 
void log_record(const char *fmt, ...)
{
    char buf[LOG_LINE_MAX];
    va_list ap;
    int n;
 
    va_start(ap, fmt);
    n = vsnprintf(buf, sizeof buf, fmt, ap);
    va_end(ap);
    if (n < 0)
        return;
    if ((size_t)n >= sizeof buf)
        n = (int)sizeof buf - 1;
 
    write_to_files(buf, (size_t)n); 
}
 
void log_close(void)
{
    if (log_std != NULL) {
        fclose(log_std);
        log_std = NULL;
    }
    if (log_sys >= 0) {
        close(log_sys);
        log_sys = -1;
    }
}
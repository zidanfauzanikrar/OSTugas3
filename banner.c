#include "banner.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_banner(void)
{
    puts("=== KELOMPOK A08 - TUGAS 3 MEMORY ===");
    puts("Sistem Operasi - Varian 8: Pemetaan Berkas vs System Call I/O");
}

void print_menu(void)
{
    print_banner();
    puts("1. Allocate with malloc/free");
    puts("2. Allocate with mmap/munmap");
    puts("3. Eksekusi fitur Varian 8");
    puts("4. Exit");
}

int read_menu_choice(void)
{
    char input[64];
    char *end;
    long choice;

    fputs("Input: ", stdout);
    fflush(stdout);

    if (fgets(input, sizeof input, stdin) == NULL)
        return MENU_END_OF_INPUT;

    /* Buang sisa baris yang terlalu panjang agar tidak terbaca sebagai
     * pilihan berikutnya. */
    if (strchr(input, '\n') == NULL) {
        int ch;
        int has_extra = 0;

        while ((ch = getchar()) != '\n' && ch != EOF)
            has_extra = 1;
        if (has_extra)
            return MENU_INVALID;
    }

    choice = strtol(input, &end, 10);
    if (end == input)
        return MENU_INVALID;
    while (isspace((unsigned char)*end))
        ++end;
    if (*end != '\0' || choice < MENU_MALLOC || choice > MENU_EXIT)
        return MENU_INVALID;

    return (int)choice;
}

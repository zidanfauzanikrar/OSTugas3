#ifndef BANNER_H
#define BANNER_H

/* Nilai pilihan yang dikembalikan oleh read_menu_choice(). */
enum menu_choice {
    MENU_END_OF_INPUT = -1,
    MENU_INVALID = 0,
    MENU_MALLOC = 1,
    MENU_MMAP = 2,
    MENU_VARIAN8 = 3,
    MENU_EXIT = 4
};

void print_banner(void);
void print_menu(void);

/* Membaca satu pilihan. Mengembalikan MENU_INVALID untuk input tidak valid,
 * atau MENU_END_OF_INPUT jika stdin telah habis/ditutup. */
int read_menu_choice(void);

#endif /* BANNER_H */

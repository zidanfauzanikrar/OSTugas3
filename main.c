#include <stdio.h>

#include "banner.h"
#include "malloc_test.h"
#include "mmap_test.h"
#include "varian8.h"

int main(void)
{
    for (;;) {
        int choice;

        putchar('\n');
        print_menu();
        choice = read_menu_choice();

        switch (choice) {
        case MENU_MALLOC:
            run_malloc_test();
            break;
        case MENU_MMAP:
            run_mmap_test();
            break;
        case MENU_VARIAN8:
            run_varian8();
            break;
        case MENU_EXIT:
            puts("Program selesai.");
            return 0;
        case MENU_END_OF_INPUT:
            putchar('\n');
            return 0;
        default:
            puts("Pilihan tidak valid. Masukkan angka 1 sampai 4.");
            break;
        }
    }
}

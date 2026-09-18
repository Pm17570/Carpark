#include <stdio.h>
#include <string.h>

/* The headers have no include guards, so the order matters
   and each one may be included only once. */

#include "file.h"
#include "user.h"
#include "admin.h"


#define ADMIN_COMMAND  "admin"
#define ADMIN_PASSWORD "KVIS1234"


int main() {

    char input[100];

    int running = 1;

    while (running) {

        printf("\n");
        printf("========================================\n");
        printf("           PARKING SYSTEM\n");
        printf("========================================\n");
        printf("1. Reserve parking\n");
        printf("2. Exit parking\n");
        printf("3. Close program\n");
        printf("========================================\n");
        printf("Choice: ");

        if (scanf("%99s", input) != 1) {

            /* End of input (Ctrl+D / Ctrl+Z): close cleanly */
            printf("\nClosing program...\n");
            break;
        }

        flushInput();


        /* =========================================
           Normal User
           ========================================= */

        if (strcmp(input, "1") == 0) {

            reserveParking();
        }

        else if (strcmp(input, "2") == 0) {

            exitParking();
        }

        else if (strcmp(input, "3") == 0 ||
                 strcmp(input, "exit") == 0 ||
                 strcmp(input, "quit") == 0) {

            char confirm[10];

            printf("Close the program? (y/n): ");

            if (scanf("%9s", confirm) != 1) break;
            flushInput();

            if (confirm[0] == 'y' || confirm[0] == 'Y') {

                printf("Closing program. Goodbye!\n");
                running = 0;
            }
        }


        /* =========================================
           Hidden Admin Command
           ========================================= */

        else if (strcmp(input, ADMIN_COMMAND) == 0) {

            char password[100];

            printf("Admin password: ");

            if (scanf("%99s", password) != 1) break;
            flushInput();

            if (strcmp(password, ADMIN_PASSWORD) == 0) {

                printf("\nAdmin login successful.\n");

                adminMenu();
            }
            else {

                printf("Incorrect password.\n");
            }
        }


        else {

            printf("Invalid choice.\n");
        }
    }

    return 0;
}
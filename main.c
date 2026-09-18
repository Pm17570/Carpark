#include <stdio.h>
#include <string.h>

#include "file.h"
#include "user.h"
#include "admin.h"


#define ADMIN_COMMAND "admin"
#define ADMIN_PASSWORD "KVIS1234"


int main() {

    char input[100];

    while (1) {

        printf("\n");
        printf("========================================\n");
        printf("           PARKING SYSTEM\n");
        printf("========================================\n");
        printf("1. Reserve parking\n");
        printf("2. Exit parking\n");
        printf("========================================\n");
        printf("Choice: ");

        scanf("%99s", input);


        /* =========================================
           Normal User
           ========================================= */

        if (strcmp(input, "1") == 0) {

            reserveParking();
        }

        else if (strcmp(input, "2") == 0) {

            exitParking();
        }


        /* =========================================
           Hidden Admin Command
           ========================================= */

        else if (strcmp(input, ADMIN_COMMAND) == 0) {

            char password[100];

            printf("Admin password: ");
            scanf("%99s", password);

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
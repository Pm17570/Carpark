#include <stdio.h>
#include <string.h>

#include "file.h"
#include "user.h"
#include "admin.h"

#define ADMIN_COMMAND "admin"
#define ADMIN_PASSWORD "KVIS1234"

int main() {
    char input[100];
    int running = 1;

    while (running)
    {
        printf("\n");
        printf("========================================\n");
        printf("           PARKING SYSTEM\n");
        printf("========================================\n");
        printf("1. Reserve parking\n");
        printf("2. Exit parking\n");
        printf("3. Close program\n");
        printf("========================================\n");
        printf("Choice: ");

        // If there is no input left we stop instead of looping forever.
        if (scanf("%99s", input) != 1) {
            printf("\nClosing program...\n");
            break;
        }

        if (strcmp(input, "1") == 0) {
            reserveParking();
        } else if (strcmp(input, "2") == 0) {
            exitParking();
        } else if (strcmp(input, "3") == 0 || strcmp(input, "exit") == 0 ||
                   strcmp(input, "quit") == 0) {
            char confirm[10];

            printf("Close the program? (y/n): ");
            if (scanf("%9s", confirm) != 1) {
                break;
            }

            if (confirm[0] == 'y' || confirm[0] == 'Y') {
                printf("Closing program. Goodbye!\n");
                running = 0;
            }
        } else if (strcmp(input, ADMIN_COMMAND) == 0) {
            // This is the secret word, it is not shown in the menu.
            char password[100];

            printf("Admin password: ");
            if (scanf("%99s", password) != 1) {
                break;
            }

            if (strcmp(password, ADMIN_PASSWORD) == 0) {
                printf("\nAdmin login successful.\n");
                adminMenu();
            } else {
                printf("Incorrect password.\n");
            }
        } else {
            printf("Invalid choice.\n");
        }
    }

    return 0;
}
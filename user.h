#ifndef USER_H
#define USER_H

#include "file.h"


/* =========================================================
   RESERVE PARKING
   ========================================================= */

void reserveParking() {

    char plate[MAX_PLATE];
    int people;

    printf("\n===== Reserve Parking =====\n");

    printf("License plate: ");
    scanf("%49s", plate);

    printf("Number of people: ");
    scanf("%d", &people);

    if (people <= 0) {

        printf("Invalid number of people.\n");
        return;
    }

    /* Check whether this plate already exists */

    if (plateExists(plate)) {

        printf("This license plate already has a parking slot.\n");
        return;
    }


    FILE *parkFile = fopen("Park.txt", "r");

    if (parkFile == NULL) {

        printf("No car parks available.\n");
        return;
    }

    char filename[150];
    char location[100];
    int capacity;

    int found = 0;

    while (fscanf(parkFile,
                  "%s %s %d",
                  filename,
                  location,
                  &capacity) == 3) {

        Slot slots[MAX_SLOTS];

        int count =
            loadSlots(filename,
                      slots,
                      MAX_SLOTS);

        for (int i = 0; i < count; i++) {

            if (!slots[i].occupied) {

                /*
                   Found an available slot
                */

                slots[i].occupied = 1;

                strcpy(slots[i].plate, plate);

                slots[i].people = people;

                saveSlots(filename,
                          slots,
                          count);

                printf("\n");
                printf("============================\n");
                printf(" Parking Reserved!\n");
                printf("============================\n");
                printf("Car park : %s\n", location);
                printf("Slot     : %d\n", slots[i].number);
                printf("Plate    : %s\n", plate);
                printf("People   : %d\n", people);
                printf("============================\n");

                found = 1;
                break;
            }
        }

        if (found)
            break;
    }

    fclose(parkFile);

    if (!found) {

        printf("\nSorry, all parking slots are full.\n");
    }
}


/* =========================================================
   EXIT PARKING
   ========================================================= */

void exitParking() {

    char plate[MAX_PLATE];

    printf("\n===== Exit Parking =====\n");

    printf("License plate: ");
    scanf("%49s", plate);

    FILE *parkFile = fopen("Park.txt", "r");

    if (parkFile == NULL) {

        printf("Park.txt not found.\n");
        return;
    }

    char filename[150];
    char location[100];
    int capacity;

    int found = 0;

    while (fscanf(parkFile,
                  "%s %s %d",
                  filename,
                  location,
                  &capacity) == 3) {

        Slot slots[MAX_SLOTS];

        int count =
            loadSlots(filename,
                      slots,
                      MAX_SLOTS);

        for (int i = 0; i < count; i++) {

            if (slots[i].occupied &&
                strcmp(slots[i].plate, plate) == 0) {

                printf("\n");
                printf("Car park : %s\n", location);
                printf("Slot     : %d\n", slots[i].number);
                printf("Plate    : %s\n", slots[i].plate);
                printf("People   : %d\n", slots[i].people);

                /*
                   Reset slot
                */

                slots[i].occupied = 0;
                slots[i].plate[0] = '\0';
                slots[i].people = 0;

                saveSlots(filename,
                          slots,
                          count);

                printf("\nParking slot released successfully.\n");

                found = 1;
                break;
            }
        }

        if (found)
            break;
    }

    fclose(parkFile);

    if (!found) {

        printf("License plate not found.\n");
    }
}

#endif
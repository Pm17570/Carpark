#ifndef ADMIN_H
#define ADMIN_H

#include "file.h"


/* =========================================================
   DASHBOARD
   ========================================================= */

void dashboard() {

    FILE *parkFile = fopen("Park.txt", "r");

    if (parkFile == NULL) {

        printf("No car parks available.\n");
        return;
    }

    char filename[150];
    char location[100];
    int capacity;

    int totalSlots = 0;
    int totalAvailable = 0;
    int totalPeople = 0;

    printf("\n");
    printf("=================================================\n");
    printf("                  DASHBOARD\n");
    printf("=================================================\n");

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

        int available = 0;
        int people = 0;

        for (int i = 0; i < count; i++) {

            if (!slots[i].occupied) {

                available++;
            }
            else {

                people += slots[i].people;
            }
        }

        printf("%-15s | Total: %3d | Available: %3d | People: %3d\n",
               location,
               count,
               available,
               people);

        totalSlots += count;
        totalAvailable += available;
        totalPeople += people;
    }

    fclose(parkFile);

    printf("-------------------------------------------------\n");
    printf("Total slots      : %d\n", totalSlots);
    printf("Available slots  : %d\n", totalAvailable);
    printf("Occupied slots   : %d\n",
           totalSlots - totalAvailable);
    printf("People in event  : %d\n", totalPeople);
    printf("=================================================\n");
}


/* =========================================================
   CAR PARK DETAILS
   ========================================================= */

void viewCarparkDetails() {

    char name[100];

    printf("\nCar park name: ");
    scanf("%99s", name);

    char filename[150];

    makeFileName(name, filename);

    FILE *file = fopen(filename, "r");

    if (file == NULL) {

        printf("Car park not found.\n");
        return;
    }

    Slot slots[MAX_SLOTS];

    int count =
        loadSlots(filename,
                  slots,
                  MAX_SLOTS);

    printf("\n");
    printf("=============================================\n");
    printf("             CAR PARK: %s\n", name);
    printf("=============================================\n");

    int totalPeople = 0;
    int available = 0;

    for (int i = 0; i < count; i++) {

        if (slots[i].occupied) {

            printf("Slot %3d : OCCUPIED | Plate: %-15s | People: %d\n",
                   slots[i].number,
                   slots[i].plate,
                   slots[i].people);

            totalPeople += slots[i].people;
        }
        else {

            printf("Slot %3d : AVAILABLE\n",
                   slots[i].number);

            available++;
        }
    }

    printf("---------------------------------------------\n");
    printf("Total slots     : %d\n", count);
    printf("Available       : %d\n", available);
    printf("Occupied        : %d\n", count - available);
    printf("People          : %d\n", totalPeople);
    printf("=============================================\n");

    fclose(file);
}


/* =========================================================
   FORCE EDIT SINGLE SLOT
   ========================================================= */

void forceEditSlot() {

    char name[100];
    int slotNumber;

    printf("\nCar park name: ");
    scanf("%99s", name);

    char filename[150];
    makeFileName(name, filename);

    Slot slots[MAX_SLOTS];

    int count =
        loadSlots(filename,
                  slots,
                  MAX_SLOTS);

    if (count == 0) {

        printf("Car park not found.\n");
        return;
    }

    printf("Slot number: ");
    scanf("%d", &slotNumber);

    if (slotNumber < 1 ||
        slotNumber > count) {

        printf("Invalid slot number.\n");
        return;
    }

    Slot *slot = &slots[slotNumber - 1];

    int choice;

    printf("\n");
    printf("1. Set AVAILABLE\n");
    printf("2. Set OCCUPIED\n");
    printf("Choice: ");
    scanf("%d", &choice);

    if (choice == 1) {

        slot->occupied = 0;
        slot->plate[0] = '\0';
        slot->people = 0;

        printf("Slot set to AVAILABLE.\n");
    }

    else if (choice == 2) {

        char plate[MAX_PLATE];
        int people;

        printf("License plate: ");
        scanf("%49s", plate);

        printf("Number of people: ");
        scanf("%d", &people);

        if (people <= 0) {

            printf("Invalid number of people.\n");
            return;
        }

        slot->occupied = 1;

        strcpy(slot->plate, plate);

        slot->people = people;

        printf("Slot set to OCCUPIED.\n");
    }

    else {

        printf("Invalid choice.\n");
        return;
    }

    saveSlots(filename,
              slots,
              count);
}


/* =========================================================
   SET ENTIRE CAR PARK AVAILABLE
   ========================================================= */

void setCarparkAvailable() {

    char name[100];

    printf("\nCar park name: ");
    scanf("%99s", name);

    char filename[150];

    makeFileName(name, filename);

    Slot slots[MAX_SLOTS];

    int count =
        loadSlots(filename,
                  slots,
                  MAX_SLOTS);

    if (count == 0) {

        printf("Car park not found.\n");
        return;
    }

    for (int i = 0; i < count; i++) {

        slots[i].occupied = 0;
        slots[i].plate[0] = '\0';
        slots[i].people = 0;
    }

    saveSlots(filename,
              slots,
              count);

    printf("All slots are now AVAILABLE.\n");
}


/* =========================================================
   SET ENTIRE CAR PARK OCCUPIED
   ========================================================= */

void setCarparkOccupied() {

    char name[100];

    printf("\nCar park name: ");
    scanf("%99s", name);

    char filename[150];

    makeFileName(name, filename);

    Slot slots[MAX_SLOTS];

    int count =
        loadSlots(filename,
                  slots,
                  MAX_SLOTS);

    if (count == 0) {

        printf("Car park not found.\n");
        return;
    }

    printf("\nThis will mark every slot as occupied.\n");
    printf("You must enter information for every slot.\n\n");

    for (int i = 0; i < count; i++) {

        printf("Slot %d\n", slots[i].number);

        printf("  License plate: ");
        scanf("%49s", slots[i].plate);

        printf("  Number of people: ");
        scanf("%d", &slots[i].people);

        if (slots[i].people <= 0) {

            printf("Invalid number of people.\n");
            slots[i].people = 1;
        }

        slots[i].occupied = 1;
    }

    saveSlots(filename,
              slots,
              count);

    printf("\nAll slots are now OCCUPIED.\n");
}


/* =========================================================
   ADMIN MENU
   ========================================================= */

void adminMenu() {

    int choice;

    do {

        printf("\n");
        printf("========================================\n");
        printf("              ADMIN MENU\n");
        printf("========================================\n");
        printf("1. Add car park\n");
        printf("2. Edit car park\n");
        printf("3. Delete car park\n");
        printf("4. Dashboard\n");
        printf("5. Car park details\n");
        printf("6. Force edit slot\n");
        printf("7. Set entire car park AVAILABLE\n");
        printf("8. Set entire car park OCCUPIED\n");
        printf("9. Back\n");
        printf("========================================\n");
        printf("Choice: ");

        scanf("%d", &choice);

        switch (choice) {

            case 1: {
                char name[100];
                char location[100];
                int capacity;

                printf("Car park name: ");
                scanf("%99s", name);

                printf("Location: ");
                scanf("%99s", location);

                printf("Capacity: ");
                scanf("%d", &capacity);

                addCarpark(name,
                           location,
                           capacity);

                break;
            }


            case 2: {

                char name[100];
                char location[100];

                int capacity;

                char newname[100];
                char newlocation[100];

                int newcapacity;

                printf("Current car park name: ");
                scanf("%99s", name);

                printf("Current location: ");
                scanf("%99s", location);

                printf("Current capacity: ");
                scanf("%d", &capacity);

                printf("New name: ");
                scanf("%99s", newname);

                printf("New location: ");
                scanf("%99s", newlocation);

                printf("New capacity: ");
                scanf("%d", &newcapacity);

                editCarpark(name,
                            location,
                            capacity,
                            newname,
                            newlocation,
                            newcapacity);

                break;
            }


            case 3: {

                char name[100];

                printf("Car park name to delete: ");
                scanf("%99s", name);

                deleteCarpark(name);

                break;
            }


            case 4:

                dashboard();

                break;


            case 5:

                viewCarparkDetails();

                break;


            case 6:

                forceEditSlot();

                break;


            case 7:

                setCarparkAvailable();

                break;


            case 8:

                setCarparkOccupied();

                break;


            case 9:

                printf("Leaving admin menu...\n");

                break;


            default:

                printf("Invalid choice.\n");
        }

    } while (choice != 9);
}

#endif
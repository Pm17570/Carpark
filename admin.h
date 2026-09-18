void dashboard() {
    FILE *parkFile = fopen(PARK_FILE, "r");
    if (parkFile == NULL) {
        printf("No car parks available.\n");
        return;
    }

    char name[MAX_NAME];
    char location[MAX_LOCATION];
    int capacity;

    int totalSlots = 0;
    int totalAvailable = 0;
    int totalRepair = 0;
    int totalPeople = 0;

    printf("\n");
    printf("======================================================================\n");
    printf("                             DASHBOARD\n");
    printf("======================================================================\n");

    while (fscanf(parkFile, "%99s %99s %d", name, location, &capacity) == 3) {
        Slot slots[MAX_SLOTS];
        int count = loadSlots(name, slots, MAX_SLOTS);

        int available = 0;
        int repair = 0;
        int people = 0;

        // We count the slots of this car park one by one.
        for (int i = 0; i < count; i++) {
            if (slots[i].status == AVAILABLE) {
                available++;
            } else if (slots[i].status == REPAIR) {
                repair++;
            } else {
                people = people + slots[i].people;
            }
        }

        printf("%-12s | %-12s | Total:%4d | Free:%4d | Repair:%4d | People:%4d\n",
               name, location, count, available, repair, people);

        totalSlots = totalSlots + count;
        totalAvailable = totalAvailable + available;
        totalRepair = totalRepair + repair;
        totalPeople = totalPeople + people;
    }

    fclose(parkFile);

    printf("----------------------------------------------------------------------\n");
    printf("Total slots      : %d\n", totalSlots);
    printf("Available slots  : %d\n", totalAvailable);
    printf("Under repair     : %d\n", totalRepair);
    printf("Occupied slots   : %d\n", totalSlots - totalAvailable - totalRepair);
    printf("People in event  : %d\n", totalPeople);
    printf("======================================================================\n");
}

void viewCarparkDetails() {
    char name[MAX_NAME];
    char location[MAX_LOCATION];
    int capacity;

    printf("\nCar park name: ");
    if (scanf("%99s", name) != 1) {
        return;
    }
    flushInput();

    if (!findCarpark(name, location, &capacity)) {
        printf("Car park not found.\n");
        return;
    }

    Slot slots[MAX_SLOTS];
    int count = loadSlots(name, slots, MAX_SLOTS);

    printf("\n");
    printf("=============================================\n");
    printf(" CAR PARK: %s  (%s)\n", name, location);
    printf("=============================================\n");

    int totalPeople = 0;
    int available = 0;
    int repair = 0;

    for (int i = 0; i < count; i++) {
        if (slots[i].status == OCCUPIED) {
            printf("Slot %3d : OCCUPIED  | Plate: %-15s | People: %d\n",
                   slots[i].number, slots[i].plate, slots[i].people);

            totalPeople = totalPeople + slots[i].people;
        } else if (slots[i].status == REPAIR) {
            printf("Slot %3d : REPAIR    | under maintenance\n",
                   slots[i].number);

            repair++;
        } else {
            printf("Slot %3d : AVAILABLE\n", slots[i].number);

            available++;
        }
    }

    printf("---------------------------------------------\n");
    printf("Total slots     : %d\n", count);
    printf("Available       : %d\n", available);
    printf("Under repair    : %d\n", repair);
    printf("Occupied        : %d\n", count - available - repair);
    printf("People          : %d\n", totalPeople);
    printf("=============================================\n");
}

// The admin can change one slot even if the user did not ask.
void forceEditSlot() {
    char name[MAX_NAME];
    int slotNumber;

    printf("\nCar park name: ");
    if (scanf("%99s", name) != 1) {
        return;
    }
    flushInput();

    Slot slots[MAX_SLOTS];
    int count = loadSlots(name, slots, MAX_SLOTS);

    if (count == 0) {
        printf("Car park not found.\n");
        return;
    }

    printf("Slot number: ");
    if (!readInt(&slotNumber) || slotNumber < 1 || slotNumber > count) {
        printf("Invalid slot number.\n");
        return;
    }

    Slot *slot = &slots[slotNumber - 1];
    int choice;

    printf("\nCurrent status: %s\n", statusText(slot->status));
    printf("1. Set AVAILABLE\n");
    printf("2. Set OCCUPIED\n");
    printf("3. Set REPAIR\n");
    printf("Choice: ");

    if (!readInt(&choice)) {
        printf("Invalid choice.\n");
        return;
    }

    if (choice == 1) {
        slot->status = AVAILABLE;
        slot->plate[0] = '\0';
        slot->people = 0;

        printf("Slot set to AVAILABLE.\n");
    } else if (choice == 2) {
        char plate[MAX_PLATE];
        int people;

        printf("License plate: ");
        if (scanf("%49s", plate) != 1) {
            return;
        }
        flushInput();

        printf("Number of people: ");
        if (!readInt(&people) || people <= 0) {
            printf("Invalid number of people.\n");
            return;
        }

        slot->status = OCCUPIED;
        strcpy(slot->plate, plate);
        slot->people = people;

        printf("Slot set to OCCUPIED.\n");
    } else if (choice == 3) {
        slot->status = REPAIR;
        slot->plate[0] = '\0';
        slot->people = 0;

        printf("Slot set to REPAIR.\n");
    } else {
        printf("Invalid choice.\n");
        return;
    }

    saveSlots(name, slots, count);
}

void setCarparkAvailable() {
    char name[MAX_NAME];

    printf("\nCar park name: ");
    if (scanf("%99s", name) != 1) {
        return;
    }
    flushInput();

    Slot slots[MAX_SLOTS];
    int count = loadSlots(name, slots, MAX_SLOTS);

    if (count == 0) {
        printf("Car park not found.\n");
        return;
    }

    // Everything becomes empty, like at the start of the event.
    for (int i = 0; i < count; i++) {
        slots[i].status = AVAILABLE;
        slots[i].plate[0] = '\0';
        slots[i].people = 0;
    }

    saveSlots(name, slots, count);
    printf("All slots are now AVAILABLE.\n");
}

// A slot with the status REPAIR is broken, so nobody can park there.
void repairCarpark() {
    char name[MAX_NAME];

    printf("\nCar park name: ");
    if (scanf("%99s", name) != 1) {
        return;
    }
    flushInput();

    Slot slots[MAX_SLOTS];
    int count = loadSlots(name, slots, MAX_SLOTS);

    if (count == 0) {
        printf("Car park not found.\n");
        return;
    }

    int choice;

    printf("\n");
    printf("1. Close ONE slot for repair\n");
    printf("2. Close the WHOLE car park for repair\n");
    printf("3. Finish repair on ONE slot\n");
    printf("4. Finish repair on the WHOLE car park\n");
    printf("Choice: ");

    if (!readInt(&choice)) {
        printf("Invalid choice.\n");
        return;
    }

    if (choice == 1 || choice == 3) {
        int slotNumber;

        printf("Slot number: ");
        if (!readInt(&slotNumber) || slotNumber < 1 || slotNumber > count) {
            printf("Invalid slot number.\n");
            return;
        }

        Slot *slot = &slots[slotNumber - 1];

        if (choice == 1) {
            // We cannot repair a slot while a car is still on it.
            if (slot->status == OCCUPIED) {
                printf("Slot %d is occupied by %s. Release it first.\n",
                       slot->number, slot->plate);
                return;
            }

            slot->status = REPAIR;
            printf("Slot %d is closed for repair.\n", slot->number);
        } else {
            if (slot->status != REPAIR) {
                printf("Slot %d is not under repair.\n", slot->number);
                return;
            }

            slot->status = AVAILABLE;
            printf("Slot %d is repaired and available again.\n", slot->number);
        }
    } else if (choice == 2) {
        // First we check that the whole car park is empty.
        for (int i = 0; i < count; i++) {
            if (slots[i].status == OCCUPIED) {
                printf("Cannot close the car park: Slot %d is still occupied "
                       "by %s.\n",
                       slots[i].number, slots[i].plate);
                return;
            }
        }

        for (int i = 0; i < count; i++) {
            slots[i].status = REPAIR;
        }

        printf("All slots are now under REPAIR.\n");
    } else if (choice == 4) {
        int fixed = 0;

        for (int i = 0; i < count; i++) {
            if (slots[i].status == REPAIR) {
                slots[i].status = AVAILABLE;
                slots[i].plate[0] = '\0';
                slots[i].people = 0;

                fixed++;
            }
        }

        printf("%d slot(s) repaired and available again.\n", fixed);
    } else {
        printf("Invalid choice.\n");
        return;
    }

    saveSlots(name, slots, count);
}

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
        printf("8. Repair slots / close for maintenance\n");
        printf("9. Back\n");
        printf("========================================\n");
        printf("Choice: ");

        if (!readInt(&choice)) {
            printf("Invalid choice.\n");
            continue;
        }

        switch (choice) {
            case 1: {
                char name[MAX_NAME];
                char location[MAX_LOCATION];
                int capacity;

                printf("Car park name: ");
                if (scanf("%99s", name) != 1) {
                    break;
                }
                flushInput();

                printf("Location: ");
                if (scanf("%99s", location) != 1) {
                    break;
                }
                flushInput();

                printf("Capacity: ");
                if (!readInt(&capacity)) {
                    printf("Invalid capacity.\n");
                    break;
                }

                addCarpark(name, location, capacity);
                break;
            }

            case 2: {
                char name[MAX_NAME];
                char newname[MAX_NAME];
                char newlocation[MAX_LOCATION];
                char oldLocation[MAX_LOCATION];
                int oldCapacity;
                int newcapacity;

                // We only ask for the name because the name is the key.
                printf("Car park name to edit: ");
                if (scanf("%99s", name) != 1) {
                    break;
                }
                flushInput();

                if (!findCarpark(name, oldLocation, &oldCapacity)) {
                    printf("Car park \"%s\" not found.\n", name);
                    break;
                }

                printf("Current location : %s\n", oldLocation);
                printf("Current capacity : %d\n", oldCapacity);

                printf("New name: ");
                if (scanf("%99s", newname) != 1) {
                    break;
                }
                flushInput();

                printf("New location: ");
                if (scanf("%99s", newlocation) != 1) {
                    break;
                }
                flushInput();

                printf("New capacity: ");
                if (!readInt(&newcapacity)) {
                    printf("Invalid capacity.\n");
                    break;
                }

                editCarpark(name, newname, newlocation, newcapacity);
                break;
            }

            case 3: {
                char name[MAX_NAME];

                printf("Car park name to delete: ");
                if (scanf("%99s", name) != 1) {
                    break;
                }
                flushInput();

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
                repairCarpark();
                break;

            case 9:
                printf("Leaving admin menu...\n");
                break;

            default:
                printf("Invalid choice.\n");
        }
    } while (choice != 9);
}
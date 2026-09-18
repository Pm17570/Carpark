void reserveParking() {
    char plate[MAX_PLATE];
    int people;

    printf("\n===== Reserve Parking =====\n");

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

    if (plateExists(plate)) {
        printf("This license plate already has a parking slot.\n");
        return;
    }

    FILE *parkFile = fopen(PARK_FILE, "r");
    if (parkFile == NULL) {
        printf("No car parks available.\n");
        return;
    }

    char name[MAX_NAME];
    char location[MAX_LOCATION];
    int capacity;
    int found = 0;

    // We look in every car park until we find a free slot.
    while (fscanf(parkFile, "%99s %99s %d", name, location, &capacity) == 3) {
        Slot slots[MAX_SLOTS];
        int count = loadSlots(name, slots, MAX_SLOTS);

        for (int i = 0; i < count; i++) {
            // A slot under repair is skipped so no car parks on it.
            if (slots[i].status != AVAILABLE) {
                continue;
            }

            slots[i].status = OCCUPIED;
            strcpy(slots[i].plate, plate);
            slots[i].people = people;

            saveSlots(name, slots, count);

            printf("\n");
            printf("============================\n");
            printf(" Parking Reserved!\n");
            printf("============================\n");
            printf("Car park : %s\n", name);
            printf("Location : %s\n", location);
            printf("Slot     : %d\n", slots[i].number);
            printf("Plate    : %s\n", plate);
            printf("People   : %d\n", people);
            printf("============================\n");

            found = 1;
            break;
        }

        if (found) {
            break;
        }
    }

    fclose(parkFile);

    if (!found) {
        printf("\nSorry, no usable parking slot is free right now.\n");
    }
}

void exitParking() {
    char plate[MAX_PLATE];

    printf("\n===== Exit Parking =====\n");

    printf("License plate: ");
    if (scanf("%49s", plate) != 1) {
        return;
    }
    flushInput();

    FILE *parkFile = fopen(PARK_FILE, "r");
    if (parkFile == NULL) {
        printf("%s not found.\n", PARK_FILE);
        return;
    }

    char name[MAX_NAME];
    char location[MAX_LOCATION];
    int capacity;
    int found = 0;

    // We search for the car by its plate.
    while (fscanf(parkFile, "%99s %99s %d", name, location, &capacity) == 3) {
        Slot slots[MAX_SLOTS];
        int count = loadSlots(name, slots, MAX_SLOTS);

        for (int i = 0; i < count; i++) {
            if (slots[i].status == OCCUPIED &&
                strcmp(slots[i].plate, plate) == 0) {
                printf("\n");
                printf("Car park : %s\n", name);
                printf("Location : %s\n", location);
                printf("Slot     : %d\n", slots[i].number);
                printf("Plate    : %s\n", slots[i].plate);
                printf("People   : %d\n", slots[i].people);

                // The car leaves so the slot becomes empty again.
                slots[i].status = AVAILABLE;
                slots[i].plate[0] = '\0';
                slots[i].people = 0;

                saveSlots(name, slots, count);

                printf("\nParking slot released successfully.\n");

                found = 1;
                break;
            }
        }

        if (found) {
            break;
        }
    }

    fclose(parkFile);

    if (!found) {
        printf("License plate not found.\n");
    }
}
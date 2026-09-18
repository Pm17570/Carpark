#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 100
#define MAX_LOCATION 100
#define MAX_PLATE 50
#define MAX_SLOTS 1000

#define PARK_FILE "Park.txt"

//3 possible states of slots
#define AVAILABLE 0
#define OCCUPIED 1
#define REPAIR 2

typedef struct slot{
    int number;
    int status;
    char plate[MAX_PLATE];
    int people;
} Slot;

//Save the car park slots into a file
void makeFileName(const char *name, char *filename) {
    snprintf(filename, 150, "%s.txt", name);
}

//Remove newline and spaces at the end of a string.
void trim(char *s) {
    int len = strlen(s);

    while (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r' ||
                       s[len - 1] == ' ' || s[len - 1] == '\t')) {
        len--;
        s[len] = '\0';
    }
}

//Clean the input buffer to avoid problems with scanf.
void flushInput() {
    int c;

    while ((c = getchar()) != '\n' && c != EOF) {
    }
}

//Reads number, otherwise return 0.
int readInt(int *value) {
    if (scanf("%d", value) != 1) {
        return 0;
    }

    return 1;
}

const char *statusText(int status) {
    if (status == OCCUPIED) {
        return "OCCUPIED";
    }
    if (status == REPAIR) {
        return "REPAIR";
    }
    return "AVAILABLE";
}

//Reads all the slots of one car park into the array.
int loadSlots(const char *name, Slot slots[], int maxSlots) {
    char filename[150];
    makeFileName(name, filename);

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        return 0;
    }

    char line[300];
    int count = 0;

    while (fgets(line, sizeof(line), file) != NULL && count < maxSlots) {
        char *token = strtok(line, "|");
        if (token == NULL) {
            continue;
        }

        if (sscanf(token, "Slot %d", &slots[count].number) != 1) {
            continue;
        }

        token = strtok(NULL, "|");
        if (token == NULL) {
            continue;
        }
        trim(token);

        slots[count].plate[0] = '\0';
        slots[count].people = 0;

        if (strcmp(token, "OCCUPIED") == 0) {
            slots[count].status = OCCUPIED;

            token = strtok(NULL, "|");
            if (token != NULL) {
                trim(token);
                strncpy(slots[count].plate, token, MAX_PLATE - 1);
                slots[count].plate[MAX_PLATE - 1] = '\0';
            }

            token = strtok(NULL, "|");
            if (token != NULL) {
                sscanf(token, "%d", &slots[count].people);
            }
        } else if (strcmp(token, "REPAIR") == 0) {
            slots[count].status = REPAIR;
        } else {
            slots[count].status = AVAILABLE;
        }

        count++;
    }

    fclose(file);
    return count;
}

// This writes the slots back into the file.
int saveSlots(const char *name, Slot slots[], int count) {
    char filename[150];
    makeFileName(name, filename);

    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        return 0;
    }

    for (int i = 0; i < count; i++) {
        if (slots[i].status == OCCUPIED) {
            fprintf(file, "Slot %d|OCCUPIED|%s|%d\n", slots[i].number,
                    slots[i].plate, slots[i].people);
        } else if (slots[i].status == REPAIR) {
            fprintf(file, "Slot %d|REPAIR\n", slots[i].number);
        } else {
            fprintf(file, "Slot %d|AVAILABLE\n", slots[i].number);
        }
    }

    fclose(file);
    return 1;
}

//Find carpark by using name as primary key.
int findCarpark(const char *name, char *location, int *capacity) {
    FILE *parkFile = fopen(PARK_FILE, "r");
    if (parkFile == NULL) {
        return 0;
    }

    char currentName[MAX_NAME];
    char currentLocation[MAX_LOCATION];
    int currentCapacity;

    while (fscanf(parkFile, "%99s %99s %d", currentName, currentLocation,
                  &currentCapacity) == 3) {
        if (strcmp(currentName, name) == 0) {
            if (location != NULL) {
                strcpy(location, currentLocation);
            }
            if (capacity != NULL) {
                *capacity = currentCapacity;
            }

            fclose(parkFile);
            return 1;
        }
    }

    fclose(parkFile);
    return 0;
}

// One car cannot park in two places.
int plateExists(const char *plate) {
    FILE *parkFile = fopen(PARK_FILE, "r");
    if (parkFile == NULL) {
        return 0;
    }

    char name[MAX_NAME];
    char location[MAX_LOCATION];
    int capacity;

    while (fscanf(parkFile, "%99s %99s %d", name, location, &capacity) == 3) {
        Slot slots[MAX_SLOTS];
        int count = loadSlots(name, slots, MAX_SLOTS);

        for (int i = 0; i < count; i++) {
            if (slots[i].status == OCCUPIED &&
                strcmp(slots[i].plate, plate) == 0) {
                fclose(parkFile);
                return 1;
            }
        }
    }

    fclose(parkFile);
    return 0;
}

int addCarpark(char *name, char *location, int capacity) {
    if (capacity <= 0 || capacity > MAX_SLOTS) {
        printf("Invalid capacity.\n");
        return 0;
    }

    // We only compare the name because the name is the key.
    if (findCarpark(name, NULL, NULL)) {
        printf("Car park \"%s\" already exists.\n", name);
        return 0;
    }

    FILE *parkFile = fopen(PARK_FILE, "a");
    if (parkFile == NULL) {
        printf("Error opening %s.\n", PARK_FILE);
        return 0;
    }

    fprintf(parkFile, "%s %s %d\n", name, location, capacity);
    fclose(parkFile);

    // Now make the file that keeps the slots.
    char filename[150];
    makeFileName(name, filename);

    FILE *slotFile = fopen(filename, "w");
    if (slotFile == NULL) {
        printf("Error creating car park file.\n");
        return 0;
    }

    for (int i = 1; i <= capacity; i++) {
        fprintf(slotFile, "Slot %d|AVAILABLE\n", i);
    }

    fclose(slotFile);
    printf("Car park added successfully.\n");
    return 1;
}

int editCarpark(char *name, char *newname, char *newlocation, int newcapacity) {
    if (!findCarpark(name, NULL, NULL)) {
        printf("Car park \"%s\" not found.\n", name);
        return 0;
    }

    if (newcapacity <= 0 || newcapacity > MAX_SLOTS) {
        printf("Invalid capacity.\n");
        return 0;
    }

    // Two car parks are not allowed to have the same name.
    if (strcmp(name, newname) != 0 && findCarpark(newname, NULL, NULL)) {
        printf("A car park named \"%s\" already exists.\n", newname);
        return 0;
    }

    Slot oldSlots[MAX_SLOTS];
    int oldCount = loadSlots(name, oldSlots, MAX_SLOTS);

    // If the car park gets smaller we cannot delete a car that is parked.
    if (newcapacity < oldCount) {
        for (int i = newcapacity; i < oldCount; i++) {
            if (oldSlots[i].status == OCCUPIED) {
                printf("Cannot reduce capacity: Slot %d is occupied.\n",
                       oldSlots[i].number);
                return 0;
            }
        }
    }

    Slot newSlots[MAX_SLOTS];

    for (int i = 0; i < newcapacity; i++) {
        newSlots[i].number = i + 1;
        newSlots[i].status = AVAILABLE;
        newSlots[i].plate[0] = '\0';
        newSlots[i].people = 0;
    }

    // We copy the old slots so nobody loses their parking.
    int copyCount = oldCount;
    if (newcapacity < copyCount) {
        copyCount = newcapacity;
    }

    for (int i = 0; i < copyCount; i++) {
        newSlots[i] = oldSlots[i];
        newSlots[i].number = i + 1;
    }

    // Now we rewrite Park.txt with a temporary file.
    FILE *parkFile = fopen(PARK_FILE, "r");
    FILE *tempFile = fopen("Park_temp.txt", "w");

    if (parkFile == NULL || tempFile == NULL) {
        printf("Error opening %s.\n", PARK_FILE);

        if (parkFile != NULL) {
            fclose(parkFile);
        }
        if (tempFile != NULL) {
            fclose(tempFile);
        }
        return 0;
    }

    char currentName[MAX_NAME];
    char currentLocation[MAX_LOCATION];
    int currentCapacity;

    while (fscanf(parkFile, "%99s %99s %d", currentName, currentLocation,
                  &currentCapacity) == 3) {
        if (strcmp(currentName, name) == 0) {
            fprintf(tempFile, "%s %s %d\n", newname, newlocation, newcapacity);
        } else {
            fprintf(tempFile, "%s %s %d\n", currentName, currentLocation,
                    currentCapacity);
        }
    }

    fclose(parkFile);
    fclose(tempFile);

    remove(PARK_FILE);
    rename("Park_temp.txt", PARK_FILE);

    // If the name changed then the slot file must change too.
    if (strcmp(name, newname) != 0) {
        char oldFilename[150];
        char newFilename[150];

        makeFileName(name, oldFilename);
        makeFileName(newname, newFilename);

        if (rename(oldFilename, newFilename) != 0) {
            printf("Error renaming car park file.\n");
            return 0;
        }
    }

    if (!saveSlots(newname, newSlots, newcapacity)) {
        printf("Error saving slots.\n");
        return 0;
    }

    printf("Car park edited successfully.\n");
    return 1;
}

int deleteCarpark(char *name) {
    FILE *parkFile = fopen(PARK_FILE, "r");
    if (parkFile == NULL) {
        printf("%s not found.\n", PARK_FILE);
        return 0;
    }

    FILE *tempFile = fopen("Park_temp.txt", "w");
    if (tempFile == NULL) {
        fclose(parkFile);
        return 0;
    }

    char currentName[MAX_NAME];
    char location[MAX_LOCATION];
    int capacity;
    int found = 0;

    // We copy every line except the one we want to delete.
    while (fscanf(parkFile, "%99s %99s %d", currentName, location,
                  &capacity) == 3) {
        if (strcmp(currentName, name) == 0) {
            found = 1;
            continue;
        }

        fprintf(tempFile, "%s %s %d\n", currentName, location, capacity);
    }

    fclose(parkFile);
    fclose(tempFile);

    if (!found) {
        remove("Park_temp.txt");
        printf("Car park \"%s\" not found.\n", name);
        return 0;
    }

    remove(PARK_FILE);
    rename("Park_temp.txt", PARK_FILE);

    char filename[150];
    makeFileName(name, filename);
    remove(filename);

    printf("Car park deleted successfully.\n");
    return 1;
}
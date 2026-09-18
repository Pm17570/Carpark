/* =========================================================
   file.h
   NOTE: there are no include guards any more.
   This file must be included EXACTLY ONCE, and it must be
   included BEFORE user.h and admin.h (see main.c).
   ========================================================= */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME     100
#define MAX_LOCATION 100
#define MAX_PLATE    50
#define MAX_SLOTS    1000

#define PARK_FILE "Park.txt"

/* Slot status */
#define AVAILABLE 0
#define OCCUPIED  1
#define REPAIR    2

typedef struct {
    int  number;
    int  status;              /* AVAILABLE / OCCUPIED / REPAIR */
    char plate[MAX_PLATE];
    int  people;
} Slot;


/* =========================================================
   Utility
   ========================================================= */

void makeFileName(const char *name, char *filename) {
    snprintf(filename, 150, "%s.txt", name);
}

void trim(char *s) {

    int len = (int)strlen(s);

    while (len > 0 &&
           (s[len - 1] == '\n' ||
            s[len - 1] == '\r' ||
            s[len - 1] == ' '  ||
            s[len - 1] == '\t')) {

        s[--len] = '\0';
    }
}

void flushInput() {

    int c;

    while ((c = getchar()) != '\n' && c != EOF) {
        /* discard */
    }
}

/* Reads an int safely. Returns 1 on success, 0 on bad input. */
int readInt(int *value) {

    if (scanf("%d", value) != 1) {
        flushInput();
        return 0;
    }

    flushInput();
    return 1;
}

const char *statusText(int status) {

    if (status == OCCUPIED) return "OCCUPIED";
    if (status == REPAIR)   return "REPAIR";

    return "AVAILABLE";
}


/* =========================================================
   Load Slots            (key = car park NAME)
   ========================================================= */

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

        char *token;

        token = strtok(line, "|");
        if (token == NULL) continue;

        if (sscanf(token, "Slot %d", &slots[count].number) != 1) continue;

        token = strtok(NULL, "|");
        if (token == NULL) continue;

        trim(token);

        slots[count].plate[0] = '\0';
        slots[count].people   = 0;

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
        }
        else if (strcmp(token, "REPAIR") == 0) {

            slots[count].status = REPAIR;
        }
        else {

            slots[count].status = AVAILABLE;
        }

        count++;
    }

    fclose(file);

    return count;
}


/* =========================================================
   Save Slots            (key = car park NAME)
   ========================================================= */

int saveSlots(const char *name, Slot slots[], int count) {

    char filename[150];
    makeFileName(name, filename);

    FILE *file = fopen(filename, "w");

    if (file == NULL) {
        return 0;
    }

    for (int i = 0; i < count; i++) {

        if (slots[i].status == OCCUPIED) {

            fprintf(file,
                    "Slot %d|OCCUPIED|%s|%d\n",
                    slots[i].number,
                    slots[i].plate,
                    slots[i].people);
        }
        else if (slots[i].status == REPAIR) {

            fprintf(file,
                    "Slot %d|REPAIR\n",
                    slots[i].number);
        }
        else {

            fprintf(file,
                    "Slot %d|AVAILABLE\n",
                    slots[i].number);
        }
    }

    fclose(file);

    return 1;
}


/* =========================================================
   Look up a car park by NAME (the primary key)
   ========================================================= */

int findCarpark(const char *name, char *location, int *capacity) {

    FILE *parkFile = fopen(PARK_FILE, "r");

    if (parkFile == NULL) {
        return 0;
    }

    char currentName[MAX_NAME];
    char currentLocation[MAX_LOCATION];
    int  currentCapacity;

    while (fscanf(parkFile, "%99s %99s %d",
                  currentName,
                  currentLocation,
                  &currentCapacity) == 3) {

        if (strcmp(currentName, name) == 0) {

            if (location) strcpy(location, currentLocation);
            if (capacity) *capacity = currentCapacity;

            fclose(parkFile);
            return 1;
        }
    }

    fclose(parkFile);

    return 0;
}


/* =========================================================
   Check duplicate plate (across every car park)
   ========================================================= */

int plateExists(const char *plate) {

    FILE *parkFile = fopen(PARK_FILE, "r");

    if (parkFile == NULL) {
        return 0;
    }

    char name[MAX_NAME];
    char location[MAX_LOCATION];
    int  capacity;

    while (fscanf(parkFile, "%99s %99s %d",
                  name,
                  location,
                  &capacity) == 3) {

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


/* =========================================================
   ADD CAR PARK
   ========================================================= */

int addCarpark(char *name, char *location, int capacity) {

    if (capacity <= 0 || capacity > MAX_SLOTS) {
        printf("Invalid capacity.\n");
        return 0;
    }

    /* Primary key check: name only */

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

    /* Create parking slot file */

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


/* =========================================================
   EDIT CAR PARK        (identified by NAME only)
   ========================================================= */

int editCarpark(char *name,
                char *newname,
                char *newlocation,
                int newcapacity) {

    if (!findCarpark(name, NULL, NULL)) {
        printf("Car park \"%s\" not found.\n", name);
        return 0;
    }

    if (newcapacity <= 0 || newcapacity > MAX_SLOTS) {
        printf("Invalid capacity.\n");
        return 0;
    }

    /* New name must not collide with another car park */

    if (strcmp(name, newname) != 0 &&
        findCarpark(newname, NULL, NULL)) {

        printf("A car park named \"%s\" already exists.\n", newname);
        return 0;
    }

    Slot oldSlots[MAX_SLOTS];

    int oldCount = loadSlots(name, oldSlots, MAX_SLOTS);

    /*
       If decreasing capacity, don't let used slots disappear.
    */

    if (newcapacity < oldCount) {

        for (int i = newcapacity; i < oldCount; i++) {

            if (oldSlots[i].status == OCCUPIED) {

                printf("Cannot reduce capacity: Slot %d is occupied.\n",
                       oldSlots[i].number);
                return 0;
            }
        }
    }

    /* Build the new slot array */

    Slot newSlots[MAX_SLOTS];

    for (int i = 0; i < newcapacity; i++) {

        newSlots[i].number   = i + 1;
        newSlots[i].status   = AVAILABLE;
        newSlots[i].plate[0] = '\0';
        newSlots[i].people   = 0;
    }

    int copyCount = oldCount < newcapacity ? oldCount : newcapacity;

    for (int i = 0; i < copyCount; i++) {

        newSlots[i]        = oldSlots[i];
        newSlots[i].number = i + 1;
    }

    /* Update Park.txt */

    FILE *parkFile = fopen(PARK_FILE, "r");
    FILE *tempFile = fopen("Park_temp.txt", "w");

    if (parkFile == NULL || tempFile == NULL) {

        printf("Error opening %s.\n", PARK_FILE);

        if (parkFile) fclose(parkFile);
        if (tempFile) fclose(tempFile);

        return 0;
    }

    char currentName[MAX_NAME];
    char currentLocation[MAX_LOCATION];
    int  currentCapacity;

    while (fscanf(parkFile, "%99s %99s %d",
                  currentName,
                  currentLocation,
                  &currentCapacity) == 3) {

        if (strcmp(currentName, name) == 0) {

            fprintf(tempFile, "%s %s %d\n",
                    newname, newlocation, newcapacity);
        }
        else {

            fprintf(tempFile, "%s %s %d\n",
                    currentName, currentLocation, currentCapacity);
        }
    }

    fclose(parkFile);
    fclose(tempFile);

    remove(PARK_FILE);
    rename("Park_temp.txt", PARK_FILE);

    /* Rename the slot file if the name changed */

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


/* =========================================================
   DELETE CAR PARK      (identified by NAME only)
   ========================================================= */

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
    int  capacity;

    int found = 0;

    while (fscanf(parkFile, "%99s %99s %d",
                  currentName,
                  location,
                  &capacity) == 3) {

        if (strcmp(currentName, name) == 0) {
            found = 1;
            continue;
        }

        fprintf(tempFile, "%s %s %d\n",
                currentName, location, capacity);
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
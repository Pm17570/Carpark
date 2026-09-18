#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 100
#define MAX_LOCATION 100
#define MAX_PLATE 50
#define MAX_SLOTS 1000

typedef struct {
    int number;
    int occupied;
    char plate[MAX_PLATE];
    int people;
} Slot;


/* =========================================================
   Utility
   ========================================================= */

void makeFileName(const char *name, char *filename) {
    snprintf(filename, 150, "%s.txt", name);
}


/* =========================================================
   Load Slots
   ========================================================= */

int loadSlots(const char *filename, Slot slots[], int maxSlots) {

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

        sscanf(token, "Slot %d", &slots[count].number);

        token = strtok(NULL, "|");
        if (token == NULL) continue;

        if (strcmp(token, "AVAILABLE\n") == 0 ||
            strcmp(token, "AVAILABLE") == 0) {

            slots[count].occupied = 0;
            slots[count].plate[0] = '\0';
            slots[count].people = 0;
        }
        else if (strcmp(token, "OCCUPIED") == 0) {

            slots[count].occupied = 1;

            token = strtok(NULL, "|");

            if (token != NULL) {
                strcpy(slots[count].plate, token);
            }

            token = strtok(NULL, "|");

            if (token != NULL) {
                sscanf(token, "%d", &slots[count].people);
            }
        }

        count++;
    }

    fclose(file);

    return count;
}


/* =========================================================
   Save Slots
   ========================================================= */

int saveSlots(const char *filename, Slot slots[], int count) {

    FILE *file = fopen(filename, "w");

    if (file == NULL) {
        return 0;
    }

    for (int i = 0; i < count; i++) {

        if (slots[i].occupied == 0) {

            fprintf(file,
                    "Slot %d|AVAILABLE\n",
                    slots[i].number);

        }
        else {

            fprintf(file,
                    "Slot %d|OCCUPIED|%s|%d\n",
                    slots[i].number,
                    slots[i].plate,
                    slots[i].people);
        }
    }

    fclose(file);

    return 1;
}


/* =========================================================
   Check duplicate plate
   ========================================================= */

int plateExists(const char *plate) {

    FILE *parkFile = fopen("Park.txt", "r");

    if (parkFile == NULL) {
        return 0;
    }

    char filename[150];
    char location[100];
    int capacity;

    while (fscanf(parkFile, "%s %s %d",
                  filename,
                  location,
                  &capacity) == 3) {

        Slot slots[MAX_SLOTS];

        int count = loadSlots(filename, slots, MAX_SLOTS);

        for (int i = 0; i < count; i++) {

            if (slots[i].occupied &&
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

    char filename[150];
    makeFileName(name, filename);

    /* Check duplicate car park */

    FILE *parkFile = fopen("Park.txt", "r");

    if (parkFile != NULL) {

        char existingName[150];
        char existingLocation[100];
        int existingCapacity;

        while (fscanf(parkFile,
                      "%s %s %d",
                      existingName,
                      existingLocation,
                      &existingCapacity) == 3) {

            if (strcmp(existingName, filename) == 0) {

                printf("Car park already exists.\n");
                fclose(parkFile);
                return 0;
            }
        }

        fclose(parkFile);
    }

    /* Add to Park.txt */

    parkFile = fopen("Park.txt", "a");

    if (parkFile == NULL) {
        printf("Error opening Park.txt.\n");
        return 0;
    }

    fprintf(parkFile,
            "%s %s %d\n",
            filename,
            location,
            capacity);

    fclose(parkFile);


    /* Create parking slot file */

    FILE *slotFile = fopen(filename, "w");

    if (slotFile == NULL) {
        printf("Error creating car park file.\n");
        return 0;
    }

    for (int i = 1; i <= capacity; i++) {

        fprintf(slotFile,
                "Slot %d|AVAILABLE\n",
                i);
    }

    fclose(slotFile);

    printf("Car park added successfully.\n");

    return 1;
}


/* =========================================================
   EDIT CAR PARK
   ========================================================= */

int editCarpark(char *name,
                char *location,
                int capacity,
                char *newname,
                char *newlocation,
                int newcapacity) {

    char oldFilename[150];
    char newFilename[150];

    makeFileName(name, oldFilename);
    makeFileName(newname, newFilename);

    Slot oldSlots[MAX_SLOTS];

    int oldCount = loadSlots(oldFilename,
                             oldSlots,
                             MAX_SLOTS);

    if (oldCount == 0) {
        printf("Car park not found.\n");
        return 0;
    }

    /* Create new slot array */

    Slot newSlots[MAX_SLOTS];

    for (int i = 0; i < newcapacity; i++) {

        newSlots[i].number = i + 1;
        newSlots[i].occupied = 0;
        newSlots[i].plate[0] = '\0';
        newSlots[i].people = 0;
    }

    /*
       Preserve old slot information
       when capacity is increased/decreased.
    */

    int copyCount =
        oldCount < newcapacity ?
        oldCount : newcapacity;

    for (int i = 0; i < copyCount; i++) {

        newSlots[i] = oldSlots[i];
        newSlots[i].number = i + 1;
    }

    /*
       If decreasing capacity,
       don't allow occupied slots to disappear.
    */

    if (newcapacity < oldCount) {

        for (int i = newcapacity; i < oldCount; i++) {

            if (oldSlots[i].occupied) {

                printf(
                    "Cannot reduce capacity: "
                    "Slot %d is occupied.\n",
                    oldSlots[i].number
                );

                return 0;
            }
        }
    }


    /* Update Park.txt */

    FILE *parkFile = fopen("Park.txt", "r");
    FILE *tempFile = fopen("Park_temp.txt", "w");

    if (parkFile == NULL || tempFile == NULL) {

        printf("Error opening Park.txt.\n");

        if (parkFile) fclose(parkFile);
        if (tempFile) fclose(tempFile);

        return 0;
    }

    char currentName[150];
    char currentLocation[100];
    int currentCapacity;

    while (fscanf(parkFile,
                  "%s %s %d",
                  currentName,
                  currentLocation,
                  &currentCapacity) == 3) {

        if (strcmp(currentName, oldFilename) == 0) {

            fprintf(tempFile,
                    "%s %s %d\n",
                    newFilename,
                    newlocation,
                    newcapacity);
        }
        else {

            fprintf(tempFile,
                    "%s %s %d\n",
                    currentName,
                    currentLocation,
                    currentCapacity);
        }
    }

    fclose(parkFile);
    fclose(tempFile);

    remove("Park.txt");
    rename("Park_temp.txt", "Park.txt");


    /* Rename file if necessary */

    if (strcmp(oldFilename, newFilename) != 0) {

        if (rename(oldFilename, newFilename) != 0) {

            printf("Error renaming car park file.\n");
            return 0;
        }
    }

    /* Save new slots */

    if (!saveSlots(newFilename,
                   newSlots,
                   newcapacity)) {

        printf("Error saving slots.\n");
        return 0;
    }

    printf("Car park edited successfully.\n");

    return 1;
}


/* =========================================================
   DELETE CAR PARK
   ========================================================= */

int deleteCarpark(char *name) {

    char filename[150];

    makeFileName(name, filename);

    /* Check whether it exists */

    FILE *parkFile = fopen("Park.txt", "r");

    if (parkFile == NULL) {
        printf("Park.txt not found.\n");
        return 0;
    }

    FILE *tempFile = fopen("Park_temp.txt", "w");

    if (tempFile == NULL) {
        fclose(parkFile);
        return 0;
    }

    char currentName[150];
    char location[100];
    int capacity;

    int found = 0;

    while (fscanf(parkFile,
                  "%s %s %d",
                  currentName,
                  location,
                  &capacity) == 3) {

        if (strcmp(currentName, filename) == 0) {

            found = 1;
            continue;
        }

        fprintf(tempFile,
                "%s %s %d\n",
                currentName,
                location,
                capacity);
    }

    fclose(parkFile);
    fclose(tempFile);

    if (!found) {

        remove("Park_temp.txt");

        printf("Car park not found.\n");

        return 0;
    }

    remove("Park.txt");
    rename("Park_temp.txt", "Park.txt");

    remove(filename);

    printf("Car park deleted successfully.\n");

    return 1;
}

#endif
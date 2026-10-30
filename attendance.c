#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAME_LEN 50
#define SUBJECT_LEN 50
#define FILE_NAME "attendance_records.dat"

typedef struct Student {
    int regNo;
    char name[NAME_LEN];
    char subject[SUBJECT_LEN];
    int totalClasses;
    int attendedClasses;
    struct Student *next;
} Student;

Student *head = NULL;
float minimumAttendance = 75.0f;

float attendancePercentage(int attended, int total) {
    if (total == 0)
        return 0.0f;

    return (float)attended * 100.0f / total;
}

void clearInputBuffer(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);
}

void readLine(char *buffer, int size) {
    if (fgets(buffer, size, stdin) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n')
            buffer[len - 1] = '\0';
        else
            clearInputBuffer();
    }
}

Student *findStudent(int regNo) {
    Student *current = head;

    while (current != NULL) {
        if (current->regNo == regNo)
            return current;
        current = current->next;
    }
    return NULL;
}

void saveRecords(void) {
    FILE *file = fopen(FILE_NAME, "wb");
    Student *current = head;

    if (file == NULL) {
        printf("Warning: Could not save records.\n");
        return;
    }

    while (current != NULL) {
        fwrite(&current->regNo, sizeof(int), 1, file);
        fwrite(current->name, sizeof(current->name), 1, file);
        fwrite(current->subject, sizeof(current->subject), 1, file);
        fwrite(&current->totalClasses, sizeof(int), 1, file);
        fwrite(&current->attendedClasses, sizeof(int), 1, file);
        current = current->next;
    }

    fclose(file);
}

void loadRecords(void) {
    FILE *file = fopen(FILE_NAME, "rb");
    if (file == NULL)
        return;

    while (1) {
        Student temp;

        if (fread(&temp.regNo, sizeof(int), 1, file) != 1)
            break;
        if (fread(temp.name, sizeof(temp.name), 1, file) != 1)
            break;
        if (fread(temp.subject, sizeof(temp.subject), 1, file) != 1)
            break;
        if (fread(&temp.totalClasses, sizeof(int), 1, file) != 1)
            break;
        if (fread(&temp.attendedClasses, sizeof(int), 1, file) != 1)
            break;

        Student *node = malloc(sizeof(Student));
        if (node == NULL)
            break;

        node->regNo = temp.regNo;
        strcpy(node->name, temp.name);
        strcpy(node->subject, temp.subject);
        node->totalClasses = temp.totalClasses;
        node->attendedClasses = temp.attendedClasses;
        node->next = NULL;

        if (head == NULL)
            head = node;
        else {
            Student *current = head;
            while (current->next != NULL)
                current = current->next;
            current->next = node;
        }
    }

    fclose(file);
}

void addStudent(void) {
    Student *node = malloc(sizeof(Student));

    if (node == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    printf("Enter register number: ");
    if (scanf("%d", &node->regNo) != 1) {
        printf("Invalid register number.\n");
        clearInputBuffer();
        free(node);
        return;
    }
    clearInputBuffer();

    if (findStudent(node->regNo) != NULL) {
        printf("Register number already exists.\n");
        free(node);
        return;
    }

    printf("Enter student name: ");
    readLine(node->name, NAME_LEN);

    printf("Enter subject name: ");
    readLine(node->subject, SUBJECT_LEN);

    printf("Enter total classes conducted: ");
    if (scanf("%d", &node->totalClasses) != 1 ||
        node->totalClasses < 0) {
        printf("Invalid total classes.\n");
        clearInputBuffer();
        free(node);
        return;
    }

    printf("Enter classes attended: ");
    if (scanf("%d", &node->attendedClasses) != 1 ||
        node->attendedClasses < 0 ||
        node->attendedClasses > node->totalClasses) {
        printf("Invalid attended classes.\n");
        clearInputBuffer();
        free(node);
        return;
    }
    clearInputBuffer();

    node->next = NULL;

    if (head == NULL)
        head = node;
    else {
        Student *current = head;
        while (current->next != NULL)
            current = current->next;
        current->next = node;
    }

    saveRecords();
    printf("Attendance record added successfully.\n");
}

void displayOne(Student *s) {
    float percentage =
        attendancePercentage(s->attendedClasses, s->totalClasses);

    printf("%-12d %-18s %-12s %-7d %-8d %7.2f%%  %s\n",
           s->regNo, s->name, s->subject,
           s->totalClasses, s->attendedClasses, percentage,
           percentage >= minimumAttendance
               ? "SAFE" : "SHORTAGE WARNING");
}

void displayAll(void) {
    Student *current = head;

    if (current == NULL) {
        printf("No student records found.\n");
        return;
    }

    printf("\nMinimum attendance: %.2f%%\n", minimumAttendance);
    printf("%-12s %-18s %-12s %-7s %-8s %-8s %s\n",
           "Register No", "Name", "Subject", "Total",
           "Attended", "Percent", "Status");

    while (current != NULL) {
        displayOne(current);
        current = current->next;
    }
}

void searchStudent(void) {
    int regNo;

    printf("Enter register number to search: ");
    if (scanf("%d", &regNo) != 1) {
        printf("Invalid register number.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    Student *s = findStudent(regNo);

    if (s == NULL)
        printf("Student record not found.\n");
    else
        displayOne(s);
}

void updateAttendance(void) {
    int regNo, total, attended;

    printf("Enter register number to update: ");
    if (scanf("%d", &regNo) != 1) {
        printf("Invalid register number.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    Student *s = findStudent(regNo);

    if (s == NULL) {
        printf("Student record not found.\n");
        return;
    }

    printf("Enter updated total classes: ");
    if (scanf("%d", &total) != 1 || total < 0) {
        printf("Invalid total classes.\n");
        clearInputBuffer();
        return;
    }

    printf("Enter updated classes attended: ");
    if (scanf("%d", &attended) != 1 ||
        attended < 0 || attended > total) {
        printf("Invalid attended classes.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    s->totalClasses = total;
    s->attendedClasses = attended;

    saveRecords();

    printf("Attendance updated. Percentage: %.2f%%\n",
           attendancePercentage(attended, total));
}

void sortByAttendance(void) {
    if (head == NULL) {
        printf("No student records found.\n");
        return;
    }

    int swapped;
    do {
        swapped = 0;
        Student *current = head;

        while (current->next != NULL) {
            Student *next = current->next;

            float p1 = attendancePercentage(
                current->attendedClasses, current->totalClasses);
            float p2 = attendancePercentage(
                next->attendedClasses, next->totalClasses);

            if (p1 > p2) {
                int r = current->regNo;
                char n[NAME_LEN], sub[SUBJECT_LEN];
                int t = current->totalClasses;
                int a = current->attendedClasses;

                strcpy(n, current->name);
                strcpy(sub, current->subject);

                current->regNo = next->regNo;
                strcpy(current->name, next->name);
                strcpy(current->subject, next->subject);
                current->totalClasses = next->totalClasses;
                current->attendedClasses = next->attendedClasses;

                next->regNo = r;
                strcpy(next->name, n);
                strcpy(next->subject, sub);
                next->totalClasses = t;
                next->attendedClasses = a;

                swapped = 1;
            }

            current = current->next;
        }
    } while (swapped);

    saveRecords();
    printf("Sorted by attendance: lowest first.\n");
    displayAll();
}

void setMinimumAttendance(void) {
    float limit;

    printf("Enter minimum attendance percentage (0-100): ");
    if (scanf("%f", &limit) != 1 ||
        limit < 0.0f || limit > 100.0f) {
        printf("Invalid percentage.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    minimumAttendance = limit;
    printf("Minimum attendance set to %.2f%%.\n", limit);
    displayAll();
}

void freeRecords(void) {
    Student *current = head;

    while (current != NULL) {
        Student *next = current->next;
        free(current);
        current = next;
    }

    head = NULL;
}

int main(void) {
    int choice;
    loadRecords();

    do {
        printf("\n===== ATTENDANCE SHORTAGE EARLY WARNING SYSTEM =====\n");
        printf("1. Add student attendance record\n");
        printf("2. Display all records\n");
        printf("3. Search student\n");
        printf("4. Update attendance\n");
        printf("5. Sort by attendance\n");
        printf("6. Set minimum attendance\n");
        printf("7. Exit\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid choice.\n");
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();

        switch (choice) {
            case 1: addStudent(); break;
            case 2: displayAll(); break;
            case 3: searchStudent(); break;
            case 4: updateAttendance(); break;
            case 5: sortByAttendance(); break;
            case 6: setMinimumAttendance(); break;
            case 7:
                saveRecords();
                freeRecords();
                printf("Records saved. Thank you!\n");
                break;
            default:
                printf("Choose a number from 1 to 7.\n");
        }
    } while (choice != 7);

    return 0;
}

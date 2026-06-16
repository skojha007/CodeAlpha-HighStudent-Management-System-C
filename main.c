/* Student Management System
 * Single-file C program using binary file I/O
 * File: main.c
 * Includes: add, delete, update, search, display, count operations
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <dirent.h>

#define MAX_STUDENTS 100
#define DATA_FILE "students.dat"

/* ─── Data Structure ─────────────────────────────────────────────────────── */

struct Student {
    int   rollNumber;
    char  name[50];
    char  branch[30];
    int   year;
    float marks;
    char  grade;
};

/* ─── Forward Declarations ───────────────────────────────────────────────── */

void  addStudent(void);
void  deleteStudent(void);
void  updateStudent(void);
void  searchStudent(void);
void  displayAllStudents(void);
int   countStudents(void);
char  calculateGrade(float marks);
void  displayMenu(void);
void  sortStudents(void);
void  exportToCSV(void);
void  generateReport(void);
void  searchByName(void);
void  filterByBranch(void);
void  filterByYear(void);
void  backupData(void);
void  restoreData(void);
void  deleteAllStudents(void);
void  displayTopStudents(void);
void  findFailingStudents(void);

/* ─── calculateGrade ─────────────────────────────────────────────────────── */
/* Takes marks as input and returns the appropriate grade character */

char calculateGrade(float marks) {
    if (marks >= 90.0f) return 'A';
    if (marks >= 75.0f) return 'B';
    if (marks >= 60.0f) return 'C';
    if (marks >= 50.0f) return 'D';
    return 'F';
}

/* ─── countStudents ──────────────────────────────────────────────────────── */
/* Returns total number of student records stored in the binary file */

int countStudents(void) {
    FILE *fp = fopen(DATA_FILE, "rb");
    if (fp == NULL) return 0;

    fseek(fp, 0, SEEK_END);
    long fileSize = ftell(fp);
    fclose(fp);

    return (int)(fileSize / sizeof(struct Student));
}

/* ─── displayMenu ────────────────────────────────────────────────────────── */
/* Prints the main menu options to the console */

void displayMenu(void) {
    printf("\n============================================\n");
    printf("       STUDENT MANAGEMENT SYSTEM\n");
    printf("============================================\n");
    printf("  1. Add Student\n");
    printf("  2. Delete Student\n");
    printf("  3. Update Student\n");
    printf("  4. Search Student\n");
    printf("  5. Display All Students\n");
    printf("  6. Sort Students\n");
    printf("  7. Export to CSV\n");
    printf("  8. Generate Report\n");
    printf("  9. Search by Name\n");
    printf(" 10. Filter by Branch\n");
    printf(" 11. Filter by Year\n");
    printf(" 12. Backup Data\n");
    printf(" 13. Restore Data\n");
    printf(" 14. Delete All Students\n");
    printf(" 15. Top N Students\n");
    printf(" 16. Failing Students\n");
    printf(" 17. Exit\n");
    printf("============================================\n");
    printf("Enter your choice: ");
}

/* ─── addStudent ─────────────────────────────────────────────────────────── */
/* Collects student data, validates input, checks for duplicate roll number,
 * auto-calculates grade, and appends the record to "students.dat" */

void addStudent(void) {
    struct Student s;

    /* Roll number */
    printf("\n--- Add New Student ---\n");
    printf("Enter Roll Number: ");
    if (scanf("%d", &s.rollNumber) != 1 || s.rollNumber <= 0) {
        printf("Error: Roll number must be a positive integer.\n");
        while (getchar() != '\n');
        return;
    }

    /* Duplicate check */
    FILE *fp = fopen(DATA_FILE, "rb");
    if (fp != NULL) {
        struct Student temp;
        while (fread(&temp, sizeof(struct Student), 1, fp) == 1) {
            if (temp.rollNumber == s.rollNumber) {
                printf("Error: Roll number %d already exists.\n", s.rollNumber);
                fclose(fp);
                return;
            }
        }
        fclose(fp);
    }

    /* Name */
    printf("Enter Name: ");
    while (getchar() != '\n');
    if (fgets(s.name, sizeof(s.name), stdin) == NULL || s.name[0] == '\n') {
        printf("Error: Name cannot be empty.\n");
        return;
    }
    s.name[strcspn(s.name, "\n")] = '\0';
    if (strlen(s.name) == 0) {
        printf("Error: Name cannot be empty.\n");
        return;
    }

    /* Branch */
    printf("Enter Branch: ");
    if (fgets(s.branch, sizeof(s.branch), stdin) == NULL || s.branch[0] == '\n') {
        printf("Error: Branch cannot be empty.\n");
        return;
    }
    s.branch[strcspn(s.branch, "\n")] = '\0';
    if (strlen(s.branch) == 0) {
        printf("Error: Branch cannot be empty.\n");
        return;
    }

    /* Year */
    printf("Enter Year (1-4): ");
    if (scanf("%d", &s.year) != 1 || s.year < 1 || s.year > 4) {
        printf("Error: Year must be between 1 and 4.\n");
        while (getchar() != '\n');
        return;
    }

    /* Marks */
    printf("Enter Marks (0-100): ");
    if (scanf("%f", &s.marks) != 1 || s.marks < 0.0f || s.marks > 100.0f) {
        printf("Error: Marks must be between 0 and 100.\n");
        while (getchar() != '\n');
        return;
    }

    /* Auto-calculate grade */
    s.grade = calculateGrade(s.marks);

    /* Append to file */
    fp = fopen(DATA_FILE, "ab");
    if (fp == NULL) {
        printf("Error: Could not open data file for writing.\n");
        return;
    }
    fwrite(&s, sizeof(struct Student), 1, fp);
    fclose(fp);

    printf("Student added successfully! (Grade: %c)\n", s.grade);
}

/* ─── deleteStudent ──────────────────────────────────────────────────────── */
/* Reads all records into a temp array, skips the target roll number,
 * then rewrites the remaining records back to "students.dat" */

void deleteStudent(void) {
    int rollToDelete;
    printf("\n--- Delete Student ---\n");
    printf("Enter Roll Number to delete: ");
    if (scanf("%d", &rollToDelete) != 1 || rollToDelete <= 0) {
        printf("Error: Roll number must be a positive integer.\n");
        while (getchar() != '\n');
        return;
    }

    FILE *fp = fopen(DATA_FILE, "rb");
    if (fp == NULL) {
        printf("No student records found!\n");
        return;
    }

    struct Student records[MAX_STUDENTS];
    int count = 0;
    int found = 0;

    struct Student temp;
    while (count < MAX_STUDENTS && fread(&temp, sizeof(struct Student), 1, fp) == 1) {
        if (temp.rollNumber == rollToDelete) {
            found = 1;
        } else {
            records[count++] = temp;
        }
    }
    fclose(fp);

    if (!found) {
        printf("Record not found!\n");
        return;
    }

    /* Rewrite file without the deleted record */
    fp = fopen(DATA_FILE, "wb");
    if (fp == NULL) {
        printf("Error: Could not open data file for writing.\n");
        return;
    }
    for (int i = 0; i < count; i++) {
        fwrite(&records[i], sizeof(struct Student), 1, fp);
    }
    fclose(fp);

    printf("Student deleted!\n");
}

/* ─── updateStudent ──────────────────────────────────────────────────────── */
/* Loads the matching record, prompts user to choose a field to update,
 * recalculates grade if marks change, then saves updated data back to file */

void updateStudent(void) {
    int rollToUpdate;
    printf("\n--- Update Student ---\n");
    printf("Enter Roll Number to update: ");
    if (scanf("%d", &rollToUpdate) != 1 || rollToUpdate <= 0) {
        printf("Error: Roll number must be a positive integer.\n");
        while (getchar() != '\n');
        return;
    }

    FILE *fp = fopen(DATA_FILE, "rb");
    if (fp == NULL) {
        printf("No student records found!\n");
        return;
    }

    struct Student records[MAX_STUDENTS];
    int count = 0;
    int foundIdx = -1;

    struct Student temp;
    while (count < MAX_STUDENTS && fread(&temp, sizeof(struct Student), 1, fp) == 1) {
        records[count] = temp;
        if (temp.rollNumber == rollToUpdate) foundIdx = count;
        count++;
    }
    fclose(fp);

    if (foundIdx == -1) {
        printf("Record not found!\n");
        return;
    }

    printf("\nWhat would you like to update?\n");
    printf("  1. Name\n");
    printf("  2. Branch\n");
    printf("  3. Year\n");
    printf("  4. Marks\n");
    printf("Enter choice: ");

    int choice;
    if (scanf("%d", &choice) != 1) {
        printf("Error: Invalid choice.\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    switch (choice) {
        case 1:
            printf("Enter new Name: ");
            if (fgets(records[foundIdx].name, sizeof(records[foundIdx].name), stdin) == NULL) {
                printf("Error reading name.\n");
                return;
            }
            records[foundIdx].name[strcspn(records[foundIdx].name, "\n")] = '\0';
            if (strlen(records[foundIdx].name) == 0) {
                printf("Error: Name cannot be empty.\n");
                return;
            }
            break;

        case 2:
            printf("Enter new Branch: ");
            if (fgets(records[foundIdx].branch, sizeof(records[foundIdx].branch), stdin) == NULL) {
                printf("Error reading branch.\n");
                return;
            }
            records[foundIdx].branch[strcspn(records[foundIdx].branch, "\n")] = '\0';
            if (strlen(records[foundIdx].branch) == 0) {
                printf("Error: Branch cannot be empty.\n");
                return;
            }
            break;

        case 3: {
            int newYear;
            printf("Enter new Year (1-4): ");
            if (scanf("%d", &newYear) != 1 || newYear < 1 || newYear > 4) {
                printf("Error: Year must be between 1 and 4.\n");
                while (getchar() != '\n');
                return;
            }
            records[foundIdx].year = newYear;
            break;
        }

        case 4: {
            float newMarks;
            printf("Enter new Marks (0-100): ");
            if (scanf("%f", &newMarks) != 1 || newMarks < 0.0f || newMarks > 100.0f) {
                printf("Error: Marks must be between 0 and 100.\n");
                while (getchar() != '\n');
                return;
            }
            records[foundIdx].marks = newMarks;
            records[foundIdx].grade = calculateGrade(newMarks);
            printf("Grade auto-updated to: %c\n", records[foundIdx].grade);
            break;
        }

        default:
            printf("Error: Invalid choice.\n");
            return;
    }

    /* Rewrite entire file with updated records */
    fp = fopen(DATA_FILE, "wb");
    if (fp == NULL) {
        printf("Error: Could not open data file for writing.\n");
        return;
    }
    for (int i = 0; i < count; i++) {
        fwrite(&records[i], sizeof(struct Student), 1, fp);
    }
    fclose(fp);

    printf("Record updated successfully!\n");
}

/* ─── searchStudent ──────────────────────────────────────────────────────── */
/* Reads the file sequentially and displays full details of the matching
 * roll number using a formatted box; prints an error if not found */

void searchStudent(void) {
    int rollToSearch;
    printf("\n--- Search Student ---\n");
    printf("Enter Roll Number to search: ");
    if (scanf("%d", &rollToSearch) != 1 || rollToSearch <= 0) {
        printf("Error: Roll number must be a positive integer.\n");
        while (getchar() != '\n');
        return;
    }

    FILE *fp = fopen(DATA_FILE, "rb");
    if (fp == NULL) {
        printf("No student records found!\n");
        return;
    }

    struct Student s;
    int found = 0;
    while (fread(&s, sizeof(struct Student), 1, fp) == 1) {
        if (s.rollNumber == rollToSearch) {
            found = 1;
            break;
        }
    }
    fclose(fp);

    if (!found) {
        printf("Student not found!\n");
        return;
    }

    printf("\n  \xe2\x95\x94\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x97\n");
    printf("  \xe2\x95\x91     STUDENT DETAILS          \xe2\x95\x91\n");
    printf("  \xe2\x95\xa0\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\xa3\n");
    printf("  \xe2\x95\x91 Roll Number : %-14d \xe2\x95\x91\n", s.rollNumber);
    printf("  \xe2\x95\x91 Name        : %-14s \xe2\x95\x91\n", s.name);
    printf("  \xe2\x95\x91 Branch      : %-14s \xe2\x95\x91\n", s.branch);
    printf("  \xe2\x95\x91 Year        : %-14d \xe2\x95\x91\n", s.year);
    printf("  \xe2\x95\x91 Marks       : %-14.2f \xe2\x95\x91\n", s.marks);
    printf("  \xe2\x95\x91 Grade       : %-14c \xe2\x95\x91\n", s.grade);
    printf("  \xe2\x95\x9a\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x9d\n");
}

/* ─── displayAllStudents ─────────────────────────────────────────────────── */
/* Reads all records from "students.dat" and prints them in a formatted
 * table; reports if no records exist */

void displayAllStudents(void) {
    FILE *fp = fopen(DATA_FILE, "rb");
    if (fp == NULL) {
        printf("\nNo student records found!\n");
        return;
    }

    struct Student s;
    int count = 0;

    /* Buffer all records to get count first */
    struct Student records[MAX_STUDENTS];
    while (count < MAX_STUDENTS && fread(&s, sizeof(struct Student), 1, fp) == 1) {
        records[count++] = s;
    }
    fclose(fp);

    if (count == 0) {
        printf("\nNo student records found!\n");
        return;
    }

    printf("\nTotal Students: %d\n", count);
    printf("---------------------------------------------------------------\n");
    printf("%-6s | %-15s | %-8s | %-4s | %-7s | %-5s\n",
           "Roll", "Name", "Branch", "Year", "Marks", "Grade");
    printf("---------------------------------------------------------------\n");

    for (int i = 0; i < count; i++) {
        printf("%-6d | %-15s | %-8s | %-4d | %-7.2f | %-5c\n",
               records[i].rollNumber,
               records[i].name,
               records[i].branch,
               records[i].year,
               records[i].marks,
               records[i].grade);
    }
    printf("---------------------------------------------------------------\n");
}

/* ─── sortStudents ───────────────────────────────────────────────────────── */
/* Loads all records, asks the user to choose a sort key (marks, name, or
 * roll number), sorts in-place using qsort(), and displays the sorted table */

/* Comparators for qsort */
static int cmpByMarksDesc(const void *a, const void *b) {
    const struct Student *sa = (const struct Student *)a;
    const struct Student *sb = (const struct Student *)b;
    if (sb->marks > sa->marks) return  1;
    if (sb->marks < sa->marks) return -1;
    return 0;
}

static int cmpByNameAsc(const void *a, const void *b) {
    const struct Student *sa = (const struct Student *)a;
    const struct Student *sb = (const struct Student *)b;
    return strcmp(sa->name, sb->name);
}

static int cmpByRollAsc(const void *a, const void *b) {
    const struct Student *sa = (const struct Student *)a;
    const struct Student *sb = (const struct Student *)b;
    return sa->rollNumber - sb->rollNumber;
}

void sortStudents(void) {
    FILE *fp = fopen(DATA_FILE, "rb");
    if (fp == NULL) {
        printf("\nNo student records found!\n");
        return;
    }

    struct Student records[MAX_STUDENTS];
    int count = 0;
    while (count < MAX_STUDENTS &&
           fread(&records[count], sizeof(struct Student), 1, fp) == 1) {
        count++;
    }
    fclose(fp);

    if (count == 0) {
        printf("\nNo student records found!\n");
        return;
    }

    printf("\n--- Sort Students ---\n");
    printf("Sort by:\n");
    printf("  1. Marks   (highest first)\n");
    printf("  2. Name    (A to Z)\n");
    printf("  3. Roll Number (lowest first)\n");
    printf("Enter choice: ");

    int choice;
    if (scanf("%d", &choice) != 1 || choice < 1 || choice > 3) {
        printf("Error: Invalid choice.\n");
        while (getchar() != '\n');
        return;
    }

    switch (choice) {
        case 1:
            qsort(records, (size_t)count, sizeof(struct Student), cmpByMarksDesc);
            printf("\nSorted by Marks (highest first):\n");
            break;
        case 2:
            qsort(records, (size_t)count, sizeof(struct Student), cmpByNameAsc);
            printf("\nSorted by Name (A to Z):\n");
            break;
        case 3:
            qsort(records, (size_t)count, sizeof(struct Student), cmpByRollAsc);
            printf("\nSorted by Roll Number (lowest first):\n");
            break;
    }

    printf("Total Students: %d\n", count);
    printf("---------------------------------------------------------------\n");
    printf("%-6s | %-15s | %-8s | %-4s | %-7s | %-5s\n",
           "Roll", "Name", "Branch", "Year", "Marks", "Grade");
    printf("---------------------------------------------------------------\n");
    for (int i = 0; i < count; i++) {
        printf("%-6d | %-15s | %-8s | %-4d | %-7.2f | %-5c\n",
               records[i].rollNumber,
               records[i].name,
               records[i].branch,
               records[i].year,
               records[i].marks,
               records[i].grade);
    }
    printf("---------------------------------------------------------------\n");
}

/* ─── exportToCSV ────────────────────────────────────────────────────────── */
/* Reads all records from "students.dat" and writes them to "students.csv"
 * with a header row; each field is quoted to handle names with commas */

void exportToCSV(void) {
    FILE *fp = fopen(DATA_FILE, "rb");
    if (fp == NULL) {
        printf("\nNo student records found!\n");
        return;
    }

    struct Student records[MAX_STUDENTS];
    int count = 0;
    while (count < MAX_STUDENTS &&
           fread(&records[count], sizeof(struct Student), 1, fp) == 1) {
        count++;
    }
    fclose(fp);

    if (count == 0) {
        printf("\nNo student records found!\n");
        return;
    }

    FILE *csv = fopen("students.csv", "w");
    if (csv == NULL) {
        printf("Error: Could not create students.csv.\n");
        return;
    }

    /* Header row */
    fprintf(csv, "Roll Number,Name,Branch,Year,Marks,Grade\n");

    /* Data rows — quote string fields to handle embedded commas/spaces */
    for (int i = 0; i < count; i++) {
        fprintf(csv, "%d,\"%s\",\"%s\",%d,%.2f,%c\n",
                records[i].rollNumber,
                records[i].name,
                records[i].branch,
                records[i].year,
                records[i].marks,
                records[i].grade);
    }

    fclose(csv);
    printf("\nExported %d record(s) to students.csv successfully!\n", count);
}

/* ─── generateReport ─────────────────────────────────────────────────────── */
/* Reads all records and prints summary statistics: total students, average
 * marks, highest/lowest scorer, and a full grade distribution breakdown */

void generateReport(void) {
    FILE *fp = fopen(DATA_FILE, "rb");
    if (fp == NULL) {
        printf("\nNo student records found!\n");
        return;
    }

    struct Student records[MAX_STUDENTS];
    int count = 0;
    while (count < MAX_STUDENTS &&
           fread(&records[count], sizeof(struct Student), 1, fp) == 1) {
        count++;
    }
    fclose(fp);

    if (count == 0) {
        printf("\nNo student records found!\n");
        return;
    }

    /* Compute statistics */
    float total    = 0.0f;
    float highest  = records[0].marks;
    float lowest   = records[0].marks;
    int   topIdx   = 0;
    int   botIdx   = 0;

    int gradeCount[5] = {0}; /* index: 0=A, 1=B, 2=C, 3=D, 4=F */

    for (int i = 0; i < count; i++) {
        total += records[i].marks;

        if (records[i].marks > highest) { highest = records[i].marks; topIdx = i; }
        if (records[i].marks < lowest)  { lowest  = records[i].marks; botIdx = i; }

        switch (records[i].grade) {
            case 'A': gradeCount[0]++; break;
            case 'B': gradeCount[1]++; break;
            case 'C': gradeCount[2]++; break;
            case 'D': gradeCount[3]++; break;
            case 'F': gradeCount[4]++; break;
        }
    }

    float average = total / (float)count;

    /* Print report */
    printf("\n===============================================\n");
    printf("          STUDENT PERFORMANCE REPORT\n");
    printf("===============================================\n");
    printf("  Total Students    : %d\n", count);
    printf("  Average Marks     : %.2f\n", average);
    printf("-----------------------------------------------\n");
    printf("  Highest Scorer    : %s (Roll %d) — %.2f (%c)\n",
           records[topIdx].name, records[topIdx].rollNumber,
           records[topIdx].marks, records[topIdx].grade);
    printf("  Lowest  Scorer    : %s (Roll %d) — %.2f (%c)\n",
           records[botIdx].name, records[botIdx].rollNumber,
           records[botIdx].marks, records[botIdx].grade);
    printf("-----------------------------------------------\n");
    printf("  Grade Distribution:\n");

    const char *labels[5] = {"A (90-100)", "B (75-89) ", "C (60-74) ",
                              "D (50-59) ", "F (< 50)  "};
    for (int g = 0; g < 5; g++) {
        float pct = (count > 0) ? (gradeCount[g] * 100.0f / count) : 0.0f;

        /* Bar: one '#' per student in that grade (visual proportion) */
        printf("    Grade %s : %3d student(s) (%5.1f%%)  [",
               labels[g], gradeCount[g], pct);
        for (int b = 0; b < gradeCount[g]; b++) printf("#");
        for (int b = gradeCount[g]; b < count; b++) printf(".");
        printf("]\n");
    }
    printf("===============================================\n");
}

/* ─── searchByName ───────────────────────────────────────────────────────── */
/* Accepts a partial or full name string (case-insensitive) and prints every
 * matching student record; useful when the roll number is unknown */

void searchByName(void) {
    char query[50];

    printf("\n--- Search by Name ---\n");
    printf("Enter name (or partial name) to search: ");
    while (getchar() != '\n');   /* flush leftover newline */
    if (fgets(query, sizeof(query), stdin) == NULL || query[0] == '\n') {
        printf("Error: Search term cannot be empty.\n");
        return;
    }
    query[strcspn(query, "\n")] = '\0';
    if (strlen(query) == 0) {
        printf("Error: Search term cannot be empty.\n");
        return;
    }

    /* Convert query to lowercase for case-insensitive comparison */
    char lowerQuery[50];
    for (int i = 0; query[i]; i++)
        lowerQuery[i] = (char)(query[i] >= 'A' && query[i] <= 'Z'
                                ? query[i] + 32 : query[i]);
    lowerQuery[strlen(query)] = '\0';

    FILE *fp = fopen(DATA_FILE, "rb");
    if (fp == NULL) {
        printf("No student records found!\n");
        return;
    }

    struct Student s;
    int matchCount = 0;

    printf("\nSearch results for \"%s\":\n", query);
    printf("---------------------------------------------------------------\n");
    printf("%-6s | %-15s | %-8s | %-4s | %-7s | %-5s\n",
           "Roll", "Name", "Branch", "Year", "Marks", "Grade");
    printf("---------------------------------------------------------------\n");

    while (fread(&s, sizeof(struct Student), 1, fp) == 1) {
        /* Convert stored name to lowercase */
        char lowerName[50];
        for (int i = 0; s.name[i]; i++)
            lowerName[i] = (char)(s.name[i] >= 'A' && s.name[i] <= 'Z'
                                   ? s.name[i] + 32 : s.name[i]);
        lowerName[strlen(s.name)] = '\0';

        /* Substring match */
        if (strstr(lowerName, lowerQuery) != NULL) {
            printf("%-6d | %-15s | %-8s | %-4d | %-7.2f | %-5c\n",
                   s.rollNumber, s.name, s.branch,
                   s.year, s.marks, s.grade);
            matchCount++;
        }
    }
    fclose(fp);

    printf("---------------------------------------------------------------\n");
    if (matchCount == 0)
        printf("No students found matching \"%s\".\n", query);
    else
        printf("%d match(es) found.\n", matchCount);
}

/* ─── filterByBranch ─────────────────────────────────────────────────────── */
/* Accepts a branch name (full or partial, case-insensitive) and displays all
 * matching students with a per-branch summary: count and average marks */

void filterByBranch(void) {
    char query[30];

    printf("\n--- Filter by Branch ---\n");
    printf("Enter branch name (or partial, e.g. \"CS\", \"ECE\"): ");
    while (getchar() != '\n');   /* flush leftover newline */
    if (fgets(query, sizeof(query), stdin) == NULL || query[0] == '\n') {
        printf("Error: Branch name cannot be empty.\n");
        return;
    }
    query[strcspn(query, "\n")] = '\0';
    if (strlen(query) == 0) {
        printf("Error: Branch name cannot be empty.\n");
        return;
    }

    /* Convert query to lowercase */
    char lowerQuery[30];
    for (int i = 0; query[i]; i++)
        lowerQuery[i] = (char)(query[i] >= 'A' && query[i] <= 'Z'
                                ? query[i] + 32 : query[i]);
    lowerQuery[strlen(query)] = '\0';

    FILE *fp = fopen(DATA_FILE, "rb");
    if (fp == NULL) {
        printf("No student records found!\n");
        return;
    }

    struct Student s;
    int    matchCount = 0;
    float  totalMarks = 0.0f;

    printf("\nStudents in branch matching \"%s\":\n", query);
    printf("---------------------------------------------------------------\n");
    printf("%-6s | %-15s | %-8s | %-4s | %-7s | %-5s\n",
           "Roll", "Name", "Branch", "Year", "Marks", "Grade");
    printf("---------------------------------------------------------------\n");

    while (fread(&s, sizeof(struct Student), 1, fp) == 1) {
        /* Convert stored branch to lowercase */
        char lowerBranch[30];
        for (int i = 0; s.branch[i]; i++)
            lowerBranch[i] = (char)(s.branch[i] >= 'A' && s.branch[i] <= 'Z'
                                     ? s.branch[i] + 32 : s.branch[i]);
        lowerBranch[strlen(s.branch)] = '\0';

        if (strstr(lowerBranch, lowerQuery) != NULL) {
            printf("%-6d | %-15s | %-8s | %-4d | %-7.2f | %-5c\n",
                   s.rollNumber, s.name, s.branch,
                   s.year, s.marks, s.grade);
            totalMarks += s.marks;
            matchCount++;
        }
    }
    fclose(fp);

    printf("---------------------------------------------------------------\n");
    if (matchCount == 0) {
        printf("No students found in branch matching \"%s\".\n", query);
    } else {
        printf("  Total : %d student(s)\n", matchCount);
        printf("  Avg   : %.2f marks\n", totalMarks / (float)matchCount);
    }
}

/* ─── filterByYear ───────────────────────────────────────────────────────── */
/* Accepts a year (1-4) and displays all students in that year with a
 * summary footer showing count, average marks, and grade breakdown */

void filterByYear(void) {
    int year;

    printf("\n--- Filter by Year ---\n");
    printf("Enter year (1-4): ");
    if (scanf("%d", &year) != 1 || year < 1 || year > 4) {
        printf("Error: Year must be between 1 and 4.\n");
        while (getchar() != '\n');
        return;
    }

    FILE *fp = fopen(DATA_FILE, "rb");
    if (fp == NULL) {
        printf("No student records found!\n");
        return;
    }

    struct Student s;
    int   matchCount  = 0;
    float totalMarks  = 0.0f;
    int   gradeCount[5] = {0}; /* A, B, C, D, F */

    printf("\nYear %d students:\n", year);
    printf("---------------------------------------------------------------\n");
    printf("%-6s | %-15s | %-8s | %-4s | %-7s | %-5s\n",
           "Roll", "Name", "Branch", "Year", "Marks", "Grade");
    printf("---------------------------------------------------------------\n");

    while (fread(&s, sizeof(struct Student), 1, fp) == 1) {
        if (s.year == year) {
            printf("%-6d | %-15s | %-8s | %-4d | %-7.2f | %-5c\n",
                   s.rollNumber, s.name, s.branch,
                   s.year, s.marks, s.grade);
            totalMarks += s.marks;
            matchCount++;

            switch (s.grade) {
                case 'A': gradeCount[0]++; break;
                case 'B': gradeCount[1]++; break;
                case 'C': gradeCount[2]++; break;
                case 'D': gradeCount[3]++; break;
                case 'F': gradeCount[4]++; break;
            }
        }
    }
    fclose(fp);

    printf("---------------------------------------------------------------\n");
    if (matchCount == 0) {
        printf("No students found in Year %d.\n", year);
    } else {
        printf("  Total  : %d student(s)\n", matchCount);
        printf("  Avg    : %.2f marks\n", totalMarks / (float)matchCount);
        printf("  Grades : A=%d  B=%d  C=%d  D=%d  F=%d\n",
               gradeCount[0], gradeCount[1], gradeCount[2],
               gradeCount[3], gradeCount[4]);
    }
}

/* ─── backupData ─────────────────────────────────────────────────────────── */
/* Copies students.dat byte-for-byte into a timestamped backup file of the
 * form students_backup_YYYYMMDD_HHMMSS.dat using the current local time */

void backupData(void) {
    FILE *src = fopen(DATA_FILE, "rb");
    if (src == NULL) {
        printf("\nNo student records found — nothing to back up.\n");
        return;
    }

    /* Build timestamped filename */
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char backupName[64];
    snprintf(backupName, sizeof(backupName),
             "students_backup_%04d%02d%02d_%02d%02d%02d.dat",
             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
             t->tm_hour, t->tm_min, t->tm_sec);

    FILE *dst = fopen(backupName, "wb");
    if (dst == NULL) {
        printf("Error: Could not create backup file.\n");
        fclose(src);
        return;
    }

    /* Copy in 4 KB chunks */
    unsigned char buf[4096];
    size_t bytesRead;
    long totalBytes = 0;

    while ((bytesRead = fread(buf, 1, sizeof(buf), src)) > 0) {
        fwrite(buf, 1, bytesRead, dst);
        totalBytes += (long)bytesRead;
    }

    fclose(src);
    fclose(dst);

    int records = (int)(totalBytes / (long)sizeof(struct Student));
    printf("\nBackup created: %s\n", backupName);
    printf("  Records backed up : %d\n", records);
    printf("  File size         : %ld byte(s)\n", totalBytes);
}

/* ─── restoreData ────────────────────────────────────────────────────────── */
/* Scans the current directory for students_backup_*.dat files, lists them
 * numbered, lets the user pick one, then copies it over students.dat */

void restoreData(void) {
    char backups[50][64]; /* up to 50 backup filenames */
    int  count = 0;

    /* Scan current directory for backup files */
    DIR *dir = opendir(".");
    if (dir == NULL) {
        printf("Error: Could not open current directory.\n");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL && count < 50) {
        /* Match files starting with "students_backup_" and ending in ".dat" */
        const char *name = entry->d_name;
        size_t len = strlen(name);
        if (strncmp(name, "students_backup_", 16) == 0 &&
            len > 4 &&
            strcmp(name + len - 4, ".dat") == 0) {
            strncpy(backups[count], name, 63);
            backups[count][63] = '\0';
            count++;
        }
    }
    closedir(dir);

    if (count == 0) {
        printf("\nNo backup files found in the current directory.\n");
        return;
    }

    /* Sort filenames alphabetically (newest timestamp sorts last) */
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (strcmp(backups[i], backups[j]) > 0) {
                char tmp[64];
                strncpy(tmp, backups[i], 63); tmp[63] = '\0';
                strncpy(backups[i], backups[j], 63); backups[i][63] = '\0';
                strncpy(backups[j], tmp, 63); backups[j][63] = '\0';
            }
        }
    }

    printf("\n--- Restore Data ---\n");
    printf("Available backups:\n");
    for (int i = 0; i < count; i++) {
        /* Show record count for each backup */
        FILE *tmp = fopen(backups[i], "rb");
        int recs = 0;
        if (tmp) {
            fseek(tmp, 0, SEEK_END);
            recs = (int)(ftell(tmp) / (long)sizeof(struct Student));
            fclose(tmp);
        }
        printf("  %2d. %s  (%d record(s))\n", i + 1, backups[i], recs);
    }

    printf("\nEnter number to restore (0 to cancel): ");
    int choice;
    if (scanf("%d", &choice) != 1) {
        while (getchar() != '\n');
        printf("Error: Invalid input.\n");
        return;
    }
    if (choice == 0) {
        printf("Restore cancelled.\n");
        return;
    }
    if (choice < 1 || choice > count) {
        printf("Error: Invalid selection.\n");
        return;
    }

    const char *chosen = backups[choice - 1];

    /* Warn and confirm before overwriting live data */
    printf("\nWARNING: This will overwrite the current students.dat!\n");
    printf("Restore from \"%s\"? (y/n): ", chosen);
    while (getchar() != '\n');
    char confirm = (char)getchar();
    if (confirm != 'y' && confirm != 'Y') {
        printf("Restore cancelled.\n");
        return;
    }

    /* Copy chosen backup → students.dat */
    FILE *src = fopen(chosen, "rb");
    if (src == NULL) {
        printf("Error: Could not open backup file.\n");
        return;
    }
    FILE *dst = fopen(DATA_FILE, "wb");
    if (dst == NULL) {
        printf("Error: Could not write to students.dat.\n");
        fclose(src);
        return;
    }

    unsigned char buf[4096];
    size_t bytesRead;
    long totalBytes = 0;
    while ((bytesRead = fread(buf, 1, sizeof(buf), src)) > 0) {
        fwrite(buf, 1, bytesRead, dst);
        totalBytes += (long)bytesRead;
    }
    fclose(src);
    fclose(dst);

    int restored = (int)(totalBytes / (long)sizeof(struct Student));
    printf("\nRestore complete!\n");
    printf("  Source  : %s\n", chosen);
    printf("  Records : %d student(s) restored\n", restored);
}

/* ─── deleteAllStudents ──────────────────────────────────────────────────── */
/* Wipes every record from students.dat after showing the current record count
 * and requiring the user to type the exact confirmation code "DELETE ALL" */

void deleteAllStudents(void) {
    /* Count existing records first */
    int total = countStudents();

    if (total == 0) {
        printf("\nNo student records found — nothing to delete.\n");
        return;
    }

    printf("\n--- Delete All Students ---\n");
    printf("WARNING: This will permanently erase ALL %d student record(s)!\n",
           total);
    printf("This action cannot be undone (consider Backup first).\n\n");
    printf("To confirm, type exactly:  DELETE ALL\n");
    printf("> ");

    /* Flush and read confirmation code */
    while (getchar() != '\n');
    char input[20];
    if (fgets(input, sizeof(input), stdin) == NULL) {
        printf("Error reading input. Aborted.\n");
        return;
    }
    input[strcspn(input, "\n")] = '\0';

    if (strcmp(input, "DELETE ALL") != 0) {
        printf("Confirmation code incorrect. No records were deleted.\n");
        return;
    }

    /* Overwrite file with zero bytes */
    FILE *fp = fopen(DATA_FILE, "wb");
    if (fp == NULL) {
        printf("Error: Could not open students.dat for writing.\n");
        return;
    }
    fclose(fp);

    printf("\nAll %d record(s) have been permanently deleted.\n", total);
    printf("students.dat has been cleared.\n");
}

/* ─── displayTopStudents ─────────────────────────────────────────────────── */
/* Asks how many top scorers to show (N), sorts a copy of all records by marks
 * descending, then prints them in a ranked table with a medal for ranks 1-3 */

void displayTopStudents(void) {
    FILE *fp = fopen(DATA_FILE, "rb");
    if (fp == NULL) {
        printf("\nNo student records found!\n");
        return;
    }

    struct Student records[MAX_STUDENTS];
    int count = 0;
    while (count < MAX_STUDENTS &&
           fread(&records[count], sizeof(struct Student), 1, fp) == 1) {
        count++;
    }
    fclose(fp);

    if (count == 0) {
        printf("\nNo student records found!\n");
        return;
    }

    printf("\n--- Top N Students ---\n");
    printf("Total records available: %d\n", count);
    printf("Enter N (how many top scorers to display): ");

    int n;
    if (scanf("%d", &n) != 1 || n <= 0) {
        printf("Error: N must be a positive integer.\n");
        while (getchar() != '\n');
        return;
    }
    if (n > count) {
        printf("Note: Only %d record(s) exist — showing all.\n", count);
        n = count;
    }

    /* Sort a local copy by marks descending (bubble sort — preserves file) */
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (records[j].marks < records[j + 1].marks) {
                struct Student tmp = records[j];
                records[j]         = records[j + 1];
                records[j + 1]     = tmp;
            }
        }
    }

    printf("\nTop %d Student(s) by Marks:\n", n);
    printf("---------------------------------------------------------------\n");
    printf("%-5s | %-6s | %-15s | %-8s | %-4s | %-7s | %-5s\n",
           "Rank", "Roll", "Name", "Branch", "Year", "Marks", "Grade");
    printf("---------------------------------------------------------------\n");

    for (int i = 0; i < n; i++) {
        /* Medal label for top 3 */
        const char *medal;
        if      (i == 0) medal = " #1 ";
        else if (i == 1) medal = " #2 ";
        else if (i == 2) medal = " #3 ";
        else             medal = "    ";

        printf("%-5s | %-6d | %-15s | %-8s | %-4d | %-7.2f | %-5c\n",
               medal,
               records[i].rollNumber,
               records[i].name,
               records[i].branch,
               records[i].year,
               records[i].marks,
               records[i].grade);
    }
    printf("---------------------------------------------------------------\n");

    /* Summary: score range across top N */
    printf("  Highest : %.2f  |  Lowest in top %d : %.2f\n",
           records[0].marks, n, records[n - 1].marks);
}

/* ─── findFailingStudents ────────────────────────────────────────────────── */
/* Lists every student with Grade F, showing their current marks and exactly
 * how many more marks they need to reach the passing threshold of 50 */

void findFailingStudents(void) {
    FILE *fp = fopen(DATA_FILE, "rb");
    if (fp == NULL) {
        printf("\nNo student records found!\n");
        return;
    }

    struct Student s;
    int failCount = 0;

    printf("\n--- Failing Students (Grade F) ---\n");
    printf("------------------------------------------------------------------\n");
    printf("%-6s | %-15s | %-8s | %-4s | %-7s | %-10s\n",
           "Roll", "Name", "Branch", "Year", "Marks", "Need");
    printf("------------------------------------------------------------------\n");

    while (fread(&s, sizeof(struct Student), 1, fp) == 1) {
        if (s.grade == 'F') {
            float needed = 50.0f - s.marks;
            printf("%-6d | %-15s | %-8s | %-4d | %-7.2f | +%-9.2f\n",
                   s.rollNumber, s.name, s.branch,
                   s.year, s.marks, needed);
            failCount++;
        }
    }
    fclose(fp);

    printf("------------------------------------------------------------------\n");
    if (failCount == 0) {
        printf("  No failing students found.\n");
    } else {
        printf("  Total failing : %d student(s)\n", failCount);
        printf("  Pass mark     : 50.00  (Grade D threshold)\n");
        printf("  \"Need\" column shows additional marks required to pass.\n");
    }
}

/* ─── main ───────────────────────────────────────────────────────────────── */
/* Entry point: runs an infinite menu loop using switch-case until the user
 * chooses to exit */

int main(void) {
    int choice;

    while (1) {
        displayMenu();

        if (scanf("%d", &choice) != 1) {
            printf("Error: Please enter a valid number.\n");
            while (getchar() != '\n');
            continue;
        }

        switch (choice) {
            case 1:
                addStudent();
                break;
            case 2:
                deleteStudent();
                break;
            case 3:
                updateStudent();
                break;
            case 4:
                searchStudent();
                break;
            case 5:
                displayAllStudents();
                break;
            case 6:
                sortStudents();
                break;
            case 7:
                exportToCSV();
                break;
            case 8:
                generateReport();
                break;
            case 9:
                searchByName();
                break;
            case 10:
                filterByBranch();
                break;
            case 11:
                filterByYear();
                break;
            case 12:
                backupData();
                break;
            case 13:
                restoreData();
                break;
            case 14:
                deleteAllStudents();
                break;
            case 15:
                displayTopStudents();
                break;
            case 16:
                findFailingStudents();
                break;
            case 17:
                printf("\nGoodbye! Data saved successfully.\n");
                exit(0);
            default:
                printf("Error: Invalid choice. Please enter 1-17.\n");
        }
    }

    return 0;
}

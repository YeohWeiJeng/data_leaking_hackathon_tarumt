#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>

#pragma warning(disable:4996)

#define MAX_LENGTH 256
#define MAX_ROW 100

typedef struct {
	char staffID[10], password[50];
}Staff;
Staff input;

void menu(FILE* staffFile, FILE* logFile);
int nextStaffID(FILE* staffFile);
void addStaff(FILE* staffFile, FILE* logFile);
void encrypt(char buffer[][MAX_LENGTH], int total, char filename[], FILE* logFile);
void decrypt(char buffer[][MAX_LENGTH], int total, char filename[], FILE* logFile);
void readFile(FILE* logFile);

void main() {
    SYSTEMTIME t;
    GetLocalTime(&t);

    FILE* logFile;
    logFile = fopen("auditlog.txt", "a");
    FILE* staffFile;
    staffFile = fopen("staff.bin", "rb");
    int idFound = 0, pwFound = 0;
    if (logFile == NULL || staffFile == NULL)
    {
        printf("\aCan't open file\n");
        exit(-1);
    }

    if (t.wHour < 7 || t.wHour > 20) // If out of working hour (07:00 - 20:00)
    {
        fprintf(logFile, "%02d-%02d-%d %02d:%02d - Someone try to login outside working hour\n", t.wDay, t.wMonth, t.wYear, t.wHour, t.wMinute);
        printf("Sorry, the working hour is from 7:00 to 20:00. You can't access this system outside of the working hour.\n");
        fclose(logFile);
        exit(-1);
    }

    Staff s;

    printf("Please enter your Staff ID : ");
    rewind(stdin);
    scanf("%s", input.staffID);

    printf("Please enter your password : ");
    int i = 0;
    char ch;
    while (1) {
        ch = getch();
        if (ch == 13) { // Enter key
            input.password[i] = '\0';
            break;
        }
        else if (ch == 8) { // Backspace key
            if (i > 0) {
                i--;
                printf("\b \b");
            }
        }
        else if (i < 49) { // Limit input to 49 characters
            input.password[i++] = ch;
            printf("*");
        }
    }

    fread(&s, sizeof(Staff), 1, staffFile);
    while (!feof(staffFile)) {
        if (strcmp(s.staffID, input.staffID) == 0) {
            if (strcmp(s.password, input.password) == 0) {
                fprintf(logFile, "%02d-%02d-%d %02d:%02d - Login activity detected\n", t.wDay, t.wMonth, t.wYear, t.wHour, t.wMinute);
                pwFound++;
                menu(staffFile, logFile);
            }
            idFound++;
        }
        fread(&s, sizeof(Staff), 1, staffFile);
    }
    if (idFound == 0)
    {
        fprintf(logFile, "%02d-%02d-%d %02d:%02d - Someone entered a wrong Staff ID\n", t.wDay, t.wMonth, t.wYear, t.wHour, t.wMinute);
        printf("\nWrong ID. Please try again later.\n");
        fclose(logFile);
        exit(-1);
    }
    if (pwFound == 0)
    {
        fprintf(logFile, "%02d-%02d-%d %02d:%02d - Someone entered a wrong password\n", t.wDay, t.wMonth, t.wYear, t.wHour, t.wMinute);
        printf("\nWrong password. Please try again later.\n");
        fclose(logFile);
        exit(-1);
    }
    fclose(staffFile);
    fclose(logFile);
}

void menu(FILE* staffFile, FILE* logFile) {
    int choice;
    system("cls");
    do {
        printf("1. Add Staff\n2. File Processing\n3. Exit\nYour Choice: ");
        scanf("%d", &choice);
        if (choice == 1)
            addStaff(staffFile, logFile);
        else if (choice == 2)
            readFile(logFile);
        else if (choice == 3)
            break;
    } while (choice < 1 || choice > 3);
}

int nextStaffID(FILE* staffFile) {
    Staff temp;
    int lastID = 0, id;

    rewind(staffFile);
    while (fread(&temp, sizeof(Staff), 1, staffFile) == 1) {
        sscanf(temp.staffID, "S%d", &id);
        if (id > lastID)
            lastID = id;
    }

    return lastID + 1;
}

void addStaff(FILE* staffFile, FILE* logFile) {
    SYSTEMTIME t;
    GetLocalTime(&t);

    Staff new;
    FILE* fptr;
    fptr = fopen("staff.bin", "ab");
    int lastID;
    char choice;
    lastID = nextStaffID(staffFile);
    sprintf(new.staffID, "S%04d", lastID);
    printf("Staff ID: %s\n", new.staffID);

    printf("Enter password: ");
    int i = 0;
    char ch;
    while (1) {
        ch = getch();
        if (ch == 13) { // Enter key
            new.password[i] = '\0';
            break;
        }
        else if (ch == 8) { // Backspace key
            if (i > 0) {
                i--;
                printf("\b \b");
            }
        }
        else if (i < 49) { // Limit input to 49 characters
            new.password[i++] = ch;
            printf("*");
        }
    }

    fprintf(logFile, "%02d-%02d-%d %02d:%02d - Staff %s added Staff %s\n", t.wDay, t.wMonth, t.wYear, t.wHour, t.wMinute, input.staffID, new.staffID);

    fwrite(&new, sizeof(Staff), 1, fptr);

    fclose(fptr);
    printf("\nAdd more staff? (Y/N) ");
    rewind(stdin);
    scanf("%c", &choice);
    if (toupper(choice) == 'Y')
        addStaff(staffFile, logFile);
    else
        menu(staffFile, logFile);
}

void encrypt(char buffer[][MAX_LENGTH], int total, char filename[], FILE* logFile) {
	SYSTEMTIME t;
	GetLocalTime(&t);

	int key;
	FILE* ptr = fopen(filename, "w");
	printf("Enter key > ");
	scanf("%d", &key);
	for (int i = 0; i < total; i++) {
		for (int j = 0; j < strlen(buffer[i]); j++) {
			buffer[i][j] = buffer[i][j] - key;
		}
		fprintf(ptr, "%s\n", buffer[i]);
	}
	printf("\n%s encrypted successfully\n", filename);
	fprintf(logFile, "%02d-%02d-%d %02d:%02d - %s had successfully encrypted\n", t.wDay, t.wMonth, t.wYear, t.wHour, t.wMinute, filename);
	fclose(ptr);
	system("pause");
}

void decrypt(char buffer[][MAX_LENGTH], int total, char filename[], FILE* logFile) {
	SYSTEMTIME t;
	GetLocalTime(&t);

	int key;
	printf("Enter key > ");
	scanf("%d", &key);
	printf("Decrypted info: \n");
	for (int i = 0; i < total; i++) {
		for (int j = 0; j < strlen(buffer[i]); j++) {
			buffer[i][j] = buffer[i][j] + key;
		}
		printf("%s\n", buffer[i]);
	}
	fprintf(logFile, "%02d-%02d-%d %02d:%02d - %s had been decrypted\n", t.wDay, t.wMonth, t.wYear, t.wHour, t.wMinute, filename);
	system("pause");
}

void readFile(FILE* logFile)
{
	FILE* fptr;
	int ch;
	do {
		system("cls");
		char buffer[MAX_ROW][MAX_LENGTH];
		int i = 0, total = 0;
		char filename[50] = "";
		
		printf("Enter file name (XXX to exit) > ");
		rewind(stdin);
		gets(filename);
		rewind(stdin);

		if (strcmp(filename, "XXX") == 0)
		{
			return;
		}

		strcat(filename, ".txt");
		fptr = fopen(filename, "r");

		if (fptr == NULL) {
			printf("file unavailable\n");
			exit(-1);
		}

		i = total = 0;

		while (fgets(buffer[i], MAX_LENGTH, fptr)) {
			buffer[i][strcspn(buffer[i], "\r\n")] = '\0';
			i++;
			total = i;
		}

		printf("1. Encryption\n2. Decryption\n3. Logout and Exit\n");

		printf("Enter choices (1/2/3) > ");
		scanf("%d", &ch);

		switch (ch) {
		case 1:
			encrypt(buffer, total, filename, logFile);
			break;
		case 2:
			decrypt(buffer, total, filename, logFile);
			break;
		case 3:
			break;
		default:
			printf("Invalid Input. Please try again later.\n");
		}

	} while (ch != 3);

	fclose(fptr);
}
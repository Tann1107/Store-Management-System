#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <windows.h>

#define ANS 15
#define ACS 4

COORD coord = {0, 0}; // global variable for cursor control

void gotoxy(int x, int y) {
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

typedef struct {
    char name[ANS], code[ACS];
    float rate;
    int quantity;
} rec;

rec item;

/* Function Declarations */
void displayMenu(const char *menu[], int numOptions, int x, int y, const char *title);
void handleMainMenu();
void handleSearchMenu();
void bill();
void add();
void edit();
void deleteItem();
void displayAll();
void displayByCode();
void displayByRate();
void displayByQuantity();
int checkCode(char code[ACS]);
void displayItem(rec *item, int i, int j);
void createWindow(int a, int b, int c, int d);

int main() {
    handleMainMenu();
    return 0;
}

void handleMainMenu() {
    const char *menu[] = {
        "Calculate Bill", "Add Goods", "Edit Goods", "Display All",
        "Search", "Delete Goods", "Exit"
    };
    displayMenu(menu, 7, 30, 22, "MAIN MENU");
}

void handleSearchMenu() {
    const char *menu[] = {
        "By Code", "By Rate", "By Quantity", "Back to Main Menu"
    };
    displayMenu(menu, 4, 30, 22, "SEARCH MENU");
}

void displayMenu(const char *menu[], int numOptions, int x, int y, const char *title) {
    int i, count = 1;
    char ch = '0';
    system("cls");
    createWindow(25, 50, 20, 32);
    gotoxy(33, 18);
    printf("%s", title);
    
    for (i = 0; i < numOptions; i++) {
        gotoxy(x, y + i);
        printf("   %s\n", menu[i]);
    }

    gotoxy(x, y + count - 1);
    while (1) {
        ch = getch();
        switch (ch) {
        case 80: // down arrow
            count = (count % numOptions) + 1;
            break;
        case 72: // up arrow
            count = (count == 1) ? numOptions : count - 1;
            break;
        case 13: // enter key
            switch (count) {
            case 1: bill(); break;
            case 2: add(); break;
            case 3: edit(); break;
            case 4: displayAll(); break;
            case 5: handleSearchMenu(); break;
            case 6: deleteItem(); break;
            case 7: exit(0);
            }
            return;
        }
    }
}

void bill() {
    char code[ACS];
    int quantity;
    float total, grandTotal = 0;
    FILE *file = fopen("record.txt", "r+b");
    if (!file) return;

    system("cls");
    createWindow(25, 50, 10, 32);
    gotoxy(26, 15);
    printf("Enter \"end\" to finish input");
    int row = 18;
    while (1) {
        gotoxy(25, row++);
        printf("Enter item code: ");
        scanf("%s", code);
        if (strcmp(code, "end") == 0) break;
        
        gotoxy(25, row++);
        printf("Enter quantity: ");
        scanf("%d", &quantity);

        rewind(file);
        while (fread(&item, sizeof(item), 1, file)) {
            if (strcmp(item.code, code) == 0) {
                total = item.rate * quantity;
                grandTotal += total;
                item.quantity -= quantity;
                fseek(file, -sizeof(item), SEEK_CUR);
                fwrite(&item, sizeof(item), 1, file);
                break;
            }
        }
    }

    if (grandTotal > 0) {
        gotoxy(30, row + 2);
        printf("TOTAL AMOUNT = NRs. %.2f", grandTotal);
    }
    fclose(file);
    getch();
    handleMainMenu();
}

void add() {
    FILE *file;
    system("cls");
    char code[ACS];
    
    file = fopen("record.txt", "ab");
    while (1) {
        system("cls");
        createWindow(25, 50, 20, 32);
        gotoxy(33, 18);
        printf("ADD GOODS");
        
        gotoxy(22, 25);
        printf("Enter new code of the article: ");
        scanf("%s", code);
        
        if (checkCode(code) == 0) {
            printf("Code already exists. Try again.");
            getch();
            continue;
        }
        
        strcpy(item.code, code);
        gotoxy(22, 27);
        printf("Enter rate of the item: ");
        scanf("%f", &item.rate);
        
        gotoxy(22, 29);
        printf("Enter quantity of the item: ");
        scanf("%d", &item.quantity);
        
        gotoxy(22, 31);
        printf("Enter name of the item: ");
        scanf("%s", item.name);
        
        fwrite(&item, sizeof(item), 1, file);
        
        gotoxy(22, 33);
        printf("Enter new record(Y/N)?");
        if (toupper(getche()) == 'N') break;
    }
    fclose(file);
    handleMainMenu();
}

void edit() {
    char code[ACS];
    FILE *file = fopen("record.txt", "r+b");
    int choice;

    system("cls");
    createWindow(20, 63, 20, 46);
    gotoxy(35, 18);
    printf("EDIT RECORDS");
    
    gotoxy(25, 23);
    printf("Enter item code: ");
    scanf("%s", code);

    if (checkCode(code) == 1) {
        gotoxy(32, 30);
        printf("Item does not exist.");
        getch();
        return;
    }

    while (fread(&item, sizeof(item), 1, file)) {
        if (strcmp(item.code, code) == 0) {
            gotoxy(25, 27);
            printf("Name: %s", item.name);
            gotoxy(25, 28);
            printf("Code: %s", item.code);
            gotoxy(25, 29);
            printf("Rate: %.2f", item.rate);
            gotoxy(25, 30);
            printf("Quantity: %d", item.quantity);
            gotoxy(25, 32);
            printf("Do you want to edit this record? (Y/N): ");
            
            if (toupper(getche()) == 'Y') {
                gotoxy(25, 34);
                printf("1 - Edit name\n2 - Edit code\n3 - Edit rate\n4 - Edit quantity");
                gotoxy(25, 39);
                printf("Enter your choice: ");
                scanf("%d", &choice);
                
                switch (choice) {
                case 1:
                    gotoxy(25, 24);
                    printf("Enter new name: ");
                    scanf("%s", item.name);
                    break;
                case 2:
                    gotoxy(25, 24);
                    printf("Enter new code: ");
                    scanf("%s", item.code);
                    break;
                case 3:
                    gotoxy(25, 24);
                    printf("Enter new rate: ");
                    scanf("%f", &item.rate);
                    break;
                case 4:
                    gotoxy(25, 24);
                    printf("Enter new quantity: ");
                    scanf("%d", &item.quantity);
                    break;
                }
                
                fseek(file, -sizeof(item), SEEK_CUR);
                fwrite(&item, sizeof(item), 1, file);
                gotoxy(27, 30);
                printf("--- Item edited ---");
                break;
            }
        }
    }
    fclose(file);
    getch();
    handleMainMenu();
}

void deleteItem() {
    char code[ACS];
    FILE *file = fopen("record.txt", "rb");
    FILE *tempFile = fopen("temp.txt", "wb");

    system("cls");
    createWindow(23, 51, 25, 34);
    gotoxy(29, 18);
    printf("DELETE ARTICLES");
    
    gotoxy(27, 27);
    printf("Enter item code: ");
    scanf("%s", code);

    if (checkCode(code) == 1) {
        gotoxy(25, 29);
        printf("--- Item does not exist ---");
        getch();
        return;
    }

    while (fread(&item, sizeof(item), 1, file)) {
        if (strcmp(item.code, code) != 0) {
            fwrite(&item, sizeof(item), 1, tempFile);
        }
    }
    
    fclose(file);
    fclose(tempFile);
    remove("record.txt");
    rename("temp.txt", "record.txt");
    
    gotoxy(27, 30);
    printf("--- Item deleted ---");
    getch();
    handleMainMenu();
}

void displayAll() {
    int i, j = 1;
    FILE *file = fopen("record.txt", "rb");
    system("cls");
    
    createWindow(20, 76, 22, 36);
    gotoxy(32, 18);
    printf("RECORDS");

    gotoxy(19, 20);
    printf("S.No.   Item Name   Item Code   Rate   Quantity");
    
    i = 24;
    while (fread(&item, sizeof(item), 1, file)) {
        displayItem(&item, i, j++);
        i++;
        if (i > 38) {
            gotoxy(27, 40);
            printf("Press any key to see more...");
            getch();
            system("cls");
            i = 24;
            gotoxy(30, 18);
            printf("RECORDS");
            gotoxy(19, 20);
            printf("S.No.   Item Name   Item Code   Rate   Quantity");
        }
    }
    
    getch();
    fclose(file);
    handleMainMenu();
}

void displayByCode() {
    char code[ACS];
    int j = 1, flag = 0;
    FILE *file = fopen("record.txt", "rb");
    
    system("cls");
    createWindow(25, 50, 20, 32);
    gotoxy(33, 18);
    printf("SEARCH BY CODE");
    
    gotoxy(25, 27);
    printf("Enter item code: ");
    scanf("%s", code);
    
    while (fread(&item, sizeof(item), 1, file)) {
        if (strcmp(item.code, code) == 0) {
            gotoxy(19, 20);
            printf("S.No.   Item Name   Item Code   Rate   Quantity");
            displayItem(&item, 26, j++);
            flag = 1;
            break;
        }
    }
    
    if (!flag) {
        gotoxy(30, 30);
        printf("No item found with the provided code.");
    }
    
    getch();
    fclose(file);
    handleMainMenu();
}

void displayByRate() {
    float rate;
    int j = 1, flag = 0;
    FILE *file = fopen("record.txt", "rb");
    
    system("cls");
    createWindow(25, 50, 20, 32);
    gotoxy(33, 18);
    printf("SEARCH BY RATE");
    
    gotoxy(25, 27);
    printf("Enter rate: ");
    scanf("%f", &rate);
    
    while (fread(&item, sizeof(item), 1, file)) {
        if (item.rate == rate) {
            if (!flag) {
                gotoxy(19, 20);
                printf("S.No.   Item Name   Item Code   Rate   Quantity");
                flag = 1;
            }
            displayItem(&item, 26, j++);
        }
    }
    
    if (!flag) {
        gotoxy(30, 30);
        printf("No item found with the provided rate.");
    }
    
    getch();
    fclose(file);
    handleMainMenu();
}

void displayByQuantity() {
    int quantity, j = 1, flag = 0;
    FILE *file = fopen("record.txt", "rb");
    
    system("cls");
    createWindow(25, 50, 20, 32);
    gotoxy(33, 18);
    printf("SEARCH BY QUANTITY");
    
    gotoxy(25, 27);
    printf("Enter quantity: ");
    scanf("%d", &quantity);
    
    while (fread(&item, sizeof(item), 1, file)) {
        if (item.quantity == quantity) {
            if (!flag) {
                gotoxy(19, 20);
                printf("S.No.   Item Name   Item Code   Rate   Quantity");
                flag = 1;
            }
            displayItem(&item, 26, j++);
        }
    }
    
    if (!flag) {
        gotoxy(30, 30);
        printf("No item found with the provided quantity.");
    }
    
    getch();
    fclose(file);
    handleMainMenu();
}

void createWindow(int a, int b, int c, int d) {
    int i;
    gotoxy(20, 10);
    for (i = 1; i <= 10; i++) printf("*");
    for (i = 1; i <= 10; i++) printf("*");

    for (i = 1; i <= c; i++) {
        gotoxy(a, b + i); printf("*");
        gotoxy(a + d, b + i); printf("*");
    }

    gotoxy(20, 32);
    for (i = 1; i <= 10; i++) printf("*");
    for (i = 1; i <= 10; i++) printf("*");
}

void displayItem(rec *item, int i, int j) {
    gotoxy(20, i);
    printf("%d", j);
    gotoxy(28, i);
    printf("%s", item->name);
    gotoxy(40, i);
    printf("%s", item->code);
    gotoxy(50, i);
    printf("%.2f", item->rate);
    gotoxy(62, i);
    printf("%d", item->quantity);
}

int checkCode(char code[ACS]) {
    FILE *file = fopen("record.txt", "rb");
    while (fread(&item, sizeof(item), 1, file)) {
        if (strcmp(item.code, code) == 0) {
            fclose(file);
            return 0; // Code exists
        }
    }
    fclose(file);
    return 1; // Code does not exist
}
#include <graphics.h>
#include <winbgim.h>
#include <conio.h>
#include <string.h>
#include <stdio.h>
#include<time.h>

#define MAX_BUTTONS 10
#define MAX_ORDER 100
#define CONTINUE_BUTTON_INDEX 10
#define SAVE_BUTTON_INDEX 11
#define PRINT_BUTTON_INDEX 12
#define LEAVE_BUTTON_INDEX 13
#define LAST_BILL_BUTTON_INDEX 14
#define BILL_HISTORY_BUTTON_INDEX 15


typedef struct {
    char name[20];
    int price;
    int quantity;
} MenuItem;

// Button positions (hardcoded values)
int buttonX1[MAX_BUTTONS + 4] = {100, 260, 420, 580, 740, 100, 260, 420, 580, 740, 500, 200, 400, 600};
int buttonY1[MAX_BUTTONS + 4] = {100, 100, 100, 100 ,100, 180, 180, 180, 180, 180, 500, 400, 400, 400};
int buttonX2[MAX_BUTTONS + 4] = {200, 360, 520, 680, 840, 200, 360, 520, 680, 840, 700, 300, 500, 700};
int buttonY2[MAX_BUTTONS + 4] = {160, 160, 160, 160, 160, 240, 240, 240, 240, 240, 550, 440, 440, 440};

int buttonColors[MAX_BUTTONS + 4] = {
    YELLOW, GREEN, BLUE, LIGHTMAGENTA, LIGHTCYAN,
    RED, BROWN, LIGHTGREEN, LIGHTBLUE, LIGHTRED, CYAN,
    LIGHTRED, LIGHTGREEN, LIGHTMAGENTA
};

char *buttonLabels[MAX_BUTTONS + 4] = {
    "Pizza", "Burger", "Coke", "Fries", "Dabeli",
    "Pepsi", "Sandwich", "Popcorn", "Fanta", "Vadapav", "Continue",
    "Save", "Print", "Leave"
};

int itemPrices[MAX_BUTTONS] = {
    250, 120, 60, 100, 80,
    50, 150, 90, 70, 40
};

MenuItem orderList[MAX_ORDER];
int orderCount = 0;
int totalPrice = 0;

// Function to draw a button
void drawButton(int x1, int y1, int x2, int y2, int color, char *label) {
    setfillstyle(SOLID_FILL, color);
    bar(x1, y1, x2, y2);
    setcolor(WHITE);
    rectangle(x1, y1, x2, y2);
    settextstyle(BOLD_FONT, HORIZ_DIR, 1);
    int textX = x1 + (x2 - x1 - textwidth(label)) / 2;
    int textY = y1 + (y2 - y1 - textheight(label)) / 2;
    outtextxy(textX, textY, label);
}

void drawButtonNormal(int index) {
    drawButton(buttonX1[index], buttonY1[index], buttonX2[index], buttonY2[index], buttonColors[index], buttonLabels[index]);
}

void drawButtonPressed(int index) {
    drawButton(buttonX1[index], buttonY1[index], buttonX2[index], buttonY2[index], LIGHTGRAY, buttonLabels[index]);
}

int isInsideRect(int x1, int y1, int x2, int y2, int x, int y) {
    return (x >= x1 && x <= x2 && y >= y1 && y <= y2);
}

int isInsideButton(int index, int x, int y) {
    return isInsideRect(buttonX1[index], buttonY1[index], buttonX2[index], buttonY2[index], x, y);
}

void showCart() {
    setcolor(WHITE);
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
    outtextxy(50, 330, "--------Items in Cart--------");

    int y = 360;
    for (int i = 0; i < orderCount; i++) {
        char itemLine[100];
        sprintf(itemLine, "%s x %d", orderList[i].name, orderList[i].quantity);
        outtextxy(60, y, itemLine);
        y += 25;
    }
}

void saveBillToFile() {
    // 1. Count previous bills from bill_history.txt
    int billNumber = 1;
    FILE *historyReader = fopen("bill_history.txt", "r");
    if (historyReader) {
        char line[256];
        while (fgets(line, sizeof(line), historyReader)) {
            if (strstr(line, "Bill No:")) {
                billNumber++;
            }
        }
        fclose(historyReader);
    }

    // 2. Open output files
    FILE *file = fopen("bill.txt", "w");
    FILE *history = fopen("bill_history.txt", "a");

    if (!file || !history) {
        setcolor(RED);
        outtextxy(100, 500, "Error saving the bill!");
        if (file) fclose(file);
        if (history) fclose(history);
        return;
    }

    // 3. Write headers with bill number
    fprintf(file, "------------------- Your BILL -------------------\n");
    fprintf(file, "Bill No: %d\n\n", billNumber);
    fprintf(history, "------------------- Your BILL -------------------\n");
    fprintf(history, "Bill No: %d\n\n", billNumber);

    fprintf(file, "%-15s %-10s %-15s %-15s\n", "Item", "Qty", "Base Price", "Net Amount");
    fprintf(history, "%-15s %-10s %-15s %-15s\n", "Item", "Qty", "Base Price", "Net Amount");

    fprintf(file, "-------------------------------------------------------------\n");
    fprintf(history, "-------------------------------------------------------------\n");

    // 4. Consolidate duplicate items
    MenuItem uniqueItems[MAX_ORDER];
    int uniqueCount = 0;

    for (int i = 0; i < orderCount; i++) {
        int found = 0;
        for (int j = 0; j < uniqueCount; j++) {
            if (strcmp(orderList[i].name, uniqueItems[j].name) == 0) {
                uniqueItems[j].quantity += orderList[i].quantity;
                found = 1;
                break;
            }
        }
        if (!found) {
            strcpy(uniqueItems[uniqueCount].name, orderList[i].name);
            uniqueItems[uniqueCount].price = orderList[i].price;
            uniqueItems[uniqueCount].quantity = orderList[i].quantity;
            uniqueCount++;
        }
    }

    // 5. Write items
    for (int i = 0; i < uniqueCount; i++) {
        int netAmount = uniqueItems[i].price * uniqueItems[i].quantity;
        fprintf(file, "%-15s %-10d Rs. %-11d Rs. %-10d\n", uniqueItems[i].name,
                uniqueItems[i].quantity, uniqueItems[i].price, netAmount);
        fprintf(history, "%-15s %-10d Rs. %-11d Rs. %-10d\n", uniqueItems[i].name,
                uniqueItems[i].quantity, uniqueItems[i].price, netAmount);
    }

    fprintf(file, "-------------------------------------------------------------\n");
    fprintf(file, "%-15s %-10s %-15s Rs. %-10d\n", "Total", "", "", totalPrice);

    fprintf(history, "-------------------------------------------------------------\n");
    fprintf(history, "%-15s %-10s %-15s Rs. %-10d\n\n", "Total", "", "", totalPrice);

    fclose(file);
    fclose(history);

    setcolor(GREEN);
    outtextxy(100, 500, "Bill saved with Bill No.!");
}



void showFinalBill() {
    cleardevice();
    setcolor(WHITE);
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
    outtextxy(50, 30, "--------Your BILL-------");

    setcolor(YELLOW); outtextxy(60, 70, "Item");
    setcolor(LIGHTMAGENTA); outtextxy(220, 70, "Qty");
    setcolor(CYAN); outtextxy(300, 70, "Base Price");
    setcolor(LIGHTGREEN); outtextxy(470, 70, "Net Amount");

    MenuItem uniqueItems[MAX_ORDER];
    int uniqueCount = 0;

    for (int i = 0; i < orderCount; i++) {
        int found = 0;
        for (int j = 0; j < uniqueCount; j++) {
            if (strcmp(orderList[i].name, uniqueItems[j].name) == 0) {
                uniqueItems[j].quantity += orderList[i].quantity;
                found = 1;
                break;
            }
        }
        if (!found) {
            strcpy(uniqueItems[uniqueCount].name, orderList[i].name);
            uniqueItems[uniqueCount].price = orderList[i].price;
            uniqueItems[uniqueCount].quantity = orderList[i].quantity;
            uniqueCount++;
        }
    }

    int y = 100;
    for (int i = 0; i < uniqueCount; i++) {
        char itemLine[100], qtyLine[10], basePriceLine[20], totalLine[20];

        sprintf(itemLine, "%s", uniqueItems[i].name);
        sprintf(qtyLine, "%d", uniqueItems[i].quantity);
        sprintf(basePriceLine, "Rs. %d", uniqueItems[i].price);
        sprintf(totalLine, "Rs. %d", uniqueItems[i].price * uniqueItems[i].quantity);

        setcolor(WHITE); outtextxy(60, y, itemLine);
        setcolor(LIGHTMAGENTA); outtextxy(220, y, qtyLine);
        setcolor(CYAN); outtextxy(300, y, basePriceLine);
        setcolor(LIGHTGREEN); outtextxy(470, y, totalLine);

        y += 30;
    }

    setcolor(WHITE);
    line(50, y + 10, 700, y + 10);

    setcolor(GREEN);
    outtextxy(60, y + 30, "Total:");
    char totalLine[100];
    sprintf(totalLine, "Rs. %d", totalPrice);
    outtextxy(470, y + 30, totalLine);

    for (int i = SAVE_BUTTON_INDEX; i <= LEAVE_BUTTON_INDEX; i++) {
        drawButtonNormal(i);
    }

    while (1) {
        if (ismouseclick(WM_LBUTTONDOWN)) {
            int x, y;
            getmouseclick(WM_LBUTTONDOWN, x, y);

            if (isInsideButton(SAVE_BUTTON_INDEX, x, y)) {
                drawButtonPressed(SAVE_BUTTON_INDEX);
                delay(50);
                drawButtonNormal(SAVE_BUTTON_INDEX);
                saveBillToFile();
            } else if (isInsideButton(PRINT_BUTTON_INDEX, x, y)) {
                drawButtonPressed(PRINT_BUTTON_INDEX);
                delay(50);
                drawButtonNormal(PRINT_BUTTON_INDEX);
                
            } else if (isInsideButton(LEAVE_BUTTON_INDEX, x, y)) {
                drawButtonPressed(LEAVE_BUTTON_INDEX);
                delay(50);
                drawButtonNormal(LEAVE_BUTTON_INDEX);
                return;
            }

            clearmouseclick(WM_LBUTTONDOWN);
        }
    }
}

void increaseQuantity(char *item) {
    for (int i = 0; i < orderCount; i++) {
        if (strcmp(orderList[i].name, item) == 0) {
            orderList[i].quantity++;
            totalPrice += orderList[i].price;
            return;
        }
    }

    for (int i = 0; i < MAX_BUTTONS; i++) {
        if (strcmp(item, buttonLabels[i]) == 0) {
            strcpy(orderList[orderCount].name, item);
            orderList[orderCount].price = itemPrices[i];
            orderList[orderCount].quantity = 1;
            totalPrice += itemPrices[i];
            orderCount++;
            break;
        }
    }
}

void showMainMenu() {
    cleardevice();

    // Define button dimensions
    int x1 = 450, x2 = 750;
    int y1_menu = 180, y2_menu = y1_menu + 70;
    int y1_exit = y2_menu + 20, y2_exit = y1_exit + 70;
    int y1_last = y2_exit + 20, y2_last = y1_last + 70;
    int y1_hist = y2_last + 20, y2_hist = y1_hist + 70;

    // Draw buttons
    drawButton(x1, y1_menu, x2, y2_menu, CYAN, "MENU");
    drawButton(x1, y1_exit, x2, y2_exit, RED, "EXIT");
    drawButton(x1, y1_last, x2, y2_last, LIGHTBLUE, "Last Bill");
    drawButton(x1, y1_hist, x2, y2_hist, YELLOW, "Bill History");

    while (1) {
        if (ismouseclick(WM_LBUTTONDOWN)) {
            int x, y;
            getmouseclick(WM_LBUTTONDOWN, x, y);

            if (isInsideRect(x1, y1_menu, x2, y2_menu, x, y)) {
                delay(100);
                return;  // MENU
            } else if (isInsideRect(x1, y1_exit, x2, y2_exit, x, y)) {
                closegraph();
                exit(0);  // EXIT
            } else if (isInsideRect(x1, y1_last, x2, y2_last, x, y)) {
                drawButton(x1, y1_last, x2, y2_last, LIGHTGRAY, "Last Bill");
                delay(100);
                drawButton(x1, y1_last, x2, y2_last, LIGHTBLUE, "Last Bill");
                system("start bill.txt");  // Opens latest bill
            } else if (isInsideRect(x1, y1_hist, x2, y2_hist, x, y)) {
                drawButton(x1, y1_hist, x2, y2_hist, LIGHTGRAY, "Bill History");
                delay(100);
                drawButton(x1, y1_hist, x2, y2_hist, YELLOW, "Bill History");
                system("start bill_history.txt");  // Opens bill history
            }

            clearmouseclick(WM_LBUTTONDOWN);
        }
    }
}

int main() {
    initwindow(1200, 650, "Food Ordering System");

    setbkcolor(BLACK);
    cleardevice();

    while (1) {
        showMainMenu();

        cleardevice();
        for (int i = 0; i <= MAX_BUTTONS; i++) {
            drawButtonNormal(i);
        }

        orderCount = 0; // Reset the order count when starting a new order
        totalPrice = 0; // Reset the total price

        int running = 1;
        while (running) {
            if (ismouseclick(WM_LBUTTONDOWN)) {
                int x, y;
                getmouseclick(WM_LBUTTONDOWN, x, y);

                for (int i = 0; i <= MAX_BUTTONS; i++) {
                    if (isInsideButton(i, x, y)) {
                        drawButtonPressed(i);
                        delay(50);
                        drawButtonNormal(i);

                        if (i == CONTINUE_BUTTON_INDEX) {
                            running = 0;
                        } else {
                            increaseQuantity(buttonLabels[i]);
                            setcolor(WHITE);
                            settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
                            char msg[100];
                            sprintf(msg, "Added: %s - Rs. %d", buttonLabels[i], itemPrices[i]);
                            outtextxy(300, 580, msg);
                            delay(600);
                            setfillstyle(SOLID_FILL, BLACK);
                            bar(300, 580, 1000, 600);
                        }
                    }
                }

                clearmouseclick(WM_LBUTTONDOWN);
            }

            showCart();
        }

        showFinalBill();
    }

    closegraph();
    return 0;
}
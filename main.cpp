#include "functions.h"

void mainMenu() {
    int choice;
    do {
        cout << "\n";
        cout << "╔════════════════════════════════════════╗\n";
        cout << "║   PHARMACY SYSTEM — BSEE25010          ║\n";
        cout << "╠════════════════════════════════════════╣\n";
        cout << "║  1. Staff Timesheet (Clock In / Out)   ║\n";
        cout << "║  2. Customer Purchase                  ║\n";
        cout << "║  0. Exit                               ║\n";
        cout << "╚════════════════════════════════════════╝\n";
        cout << "Choice: ";
        cin >> choice;

        switch (choice) {
            case 1: timesheetMenu(); break;
            case 2: purchaseMenu();  break;
            case 0: cout << "Goodbye!\n"; break;
            default: cout << "Invalid choice.\n";
        }
    } while (choice != 0);
}

int main() {
    loadStaff();
    loadTimesheet();

    mainMenu();

    saveTimesheet();
    freeMemory();

    return 0;
}

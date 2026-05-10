#include "comparison.h"
#include <fstream>
#include <iomanip>
#include <cmath>

void comparisonModuleMenu() {
    int choice;
    do {
        cout << "\n========================================\n";
        cout << "   PHARMACY MARKET ANALYSIS SYSTEM\n";
        cout << "========================================\n";
        cout << "1. Compare Prices with Competitors\n";
        cout << "2. Check Clinical Staff Availability\n";
        cout << "0. Exit Module\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
            case 1: runPriceComparison(); break;
            case 2: checkClinicalStaff(); break;
            case 0: break;
            default: cout << "Invalid choice!\n";
        }
    } while (choice != 0);
}

void runPriceComparison() {
    ifstream prodFile("Products.json");
    ifstream compFile("competitors.json");

    if (!prodFile || !compFile) {
        cout << "\n[!] Error: Ensure Products.json and competitors.json are in the folder.\n";
        return;
    }

    json productsData, competitorsData;
    prodFile >> productsData;
    compFile >> competitorsData;
    prodFile.close();
    compFile.close();

    cout << "\n" << left << setw(23) << "Medicine" << setw(10) << "Our Rs" << setw(12) << "Pharma.A" << setw(12) << "Pharma.B" << "Market Status" << endl;
    cout << string(80, '-') << endl;

    for (auto& category : productsData["categories"]) {
        for (auto& product : category["products"]) {
            string pName = product["name"];
            double ourPrice = round(product["price"].get<double>());
            double compA = 0, compB = 0;
            bool foundInMarket = false;

            for (auto& comp : competitorsData["competitors"]) {
                if (comp["name"] == pName) {
                    compA = round(comp["pharmacyA"].get<double>());
                    compB = round(comp["pharmacyB"].get<double>());
                    foundInMarket = true;
                    break;
                }
            }

            if (foundInMarket) {
                cout << left << setw(23) << pName 
                     << fixed << setprecision(0) 
                     << setw(10) << ourPrice 
                     << setw(12) << compA 
                     << setw(12) << compB;

                if (ourPrice <= compA && ourPrice <= compB) {
                    cout << "[CHEAPEST]";
                } else if (ourPrice > compA || ourPrice > compB) {
                    cout << "[OVERPRICED]";
                } else {
                    cout << "[COMPETITIVE]";
                }
                cout << endl;
            }
        }
    }
}

void checkClinicalStaff() {
    StaffMember staffList[3] = {
        {"BP Specialist", true, "Now"},
        {"Urine Technician", false, "04:00 PM"},
        {"Blood Sugar Tech", true, "Now"}
    };

    cout << "\n--- Clinical Staff Availability ---\n";
    cout << left << setw(20) << "Service Role" << setw(15) << "Status" << "Returns At" << endl;
    cout << string(55, '-') << endl;

    for (int i = 0; i < 3; i++) {
        cout << left << setw(20) << staffList[i].role;
        cout << setw(15) << (staffList[i].isAvailable ? "AVAILABLE" : "AWAY") << staffList[i].returnTime << endl;
    }
}
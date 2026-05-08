#include "finance.h"

int billCounter = 1000;

void menu() {
    int choice;

    do {
        cout << "\n===== Finance & Billing =====\n";
        cout << "1. Generate Bill\n";
        cout << "2. View Sales\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch(choice) {
            
            case 1: generateBill(); 
            break;
            case 2: viewSales(); 
            break;

        }

    } while(choice != 0);
}

// GENERATE BILL
void generateBill() {

    // READ PRODUCTS.JSON
    ifstream productFile("products.json");

    if(!productFile) {
        cout << "products.json file not found!\n";
        return;
    }

    json productsData;
    productFile >> productsData;
    productFile.close();
   

cout << "\n========== AVAILABLE PRODUCTS ==========\n";

for(int i = 0; i < productsData["categories"].size(); i++) {

    cout << "\n\nCategory: "
         << productsData["categories"][i]["category"]
         << endl;

    for(int j = 0;j < productsData["categories"][i]["products"].size();j++) {

        cout << "ID: "
             << productsData["categories"][i]["products"][j]["id"]

             << " | Name: "
             << productsData["categories"][i]["products"][j]["name"]

             << " | Price: "
             << productsData["categories"][i]["products"][j]["price"]

             << " | Stock: "
             << productsData["categories"][i]["products"][j]["stock"]

             << endl;
    }
}

cout << "\n========================================\n";

    int n;
    cout << "Enter number of items: ";
    cin >> n;

    // DMA
    string *name = new string[n];
    int *qty = new int[n];
    float *price = new float[n];
    float *total = new float[n];

    for(int i = 0; i < n; i++) {

        int productId;
        bool found = false;

        cout << "\nEnter Product ID: ";
        cin >> productId;

        cout << "Enter Quantity: ";
        cin >> qty[i];

        // SEARCH PRODUCT FROM JSON
        for(auto &category : productsData["categories"]) {

            for(auto &product : category["products"]) {

                if(product["id"] == productId) {

                    found = true;

                    int stock = product["stock"];

                    if(stock < qty[i]) {
                        cout << "Insufficient stock!\n";

                        delete[] name;
                        delete[] qty;
                        delete[] price;
                        delete[] total;

                        return;
                    }

                    // READ DATA FROM FILE
                    name[i] = product["name"];
                    price[i] = product["price"];

                    total[i] = qty[i] * price[i];

                    // UPDATE STOCK
                    product["stock"] = stock - qty[i];

                    cout << "\nProduct: " << name[i];
                    cout << "\nPrice: " << price[i];
                    cout << "\nRemaining Stock: "
                         << product["stock"] << endl;

                    break;
                }
            }
        }

        if(!found) {
            cout << "Product not found!\n";

            delete[] name;
            delete[] qty;
            delete[] price;
            delete[] total;

            return;
        }
    }

    // SAVE UPDATED STOCK
    ofstream updateFile("products.json");
    updateFile << setw(4) << productsData << endl;
    updateFile.close();

    float grandTotal = recursiveTotal(total, n);

    grandTotal = applyDiscount(grandTotal);
    grandTotal = addTax(grandTotal);

    cout << "\n===== FINAL BILL =====\n";

    for(int i = 0; i < n; i++) {

        cout << name[i]
             << " | Qty: " << qty[i]
             << " | Price: " << price[i]
             << " | Total: " << total[i]
             << endl;
    }

    cout << "\nBill ID: " << billCounter++;
    cout << "\nFinal Total: " << grandTotal << endl;

    saveToFile(n, name, qty, price, total, grandTotal);

    // STORE COMPLETE BILL IN bill.json
    saveJSON(n, name, qty, price, total, grandTotal);

    // FREE MEMORY
    delete[] name;
    delete[] qty;
    delete[] price;
    delete[] total;
}

// NORMAL TOTAL
float calculateTotal(float *total, int n) {

    float sum = 0;

    for(int i = 0; i < n; i++)
        sum += total[i];

    return sum;
}

// RECURSIVE TOTAL
float recursiveTotal(float *total, int n) {

    if(n == 0)
        return 0;

    return total[n-1] + recursiveTotal(total, n-1);
}

// SAVE TO TEXT FILE
void saveToFile(int n,string name[],int qty[],float price[],float total[],float grandTotal) {

    ofstream file("bills.txt", ios::app);

    file << "\n===== BILL =====\n";

    for(int i = 0; i < n; i++) {

        file << name[i] << " "
             << qty[i] << " "
             << price[i] << " "
             << total[i] << endl;
    }

    file << "Grand Total: "
         << grandTotal << endl;

    file << "=====================\n";

    file.close();
}

// SAVE TO JSON
void saveJSON(int n,string name[],int qty[],float price[],float total[],float grandTotal) {

    json bill;

    bill["bill_id"] = billCounter;

    for(int i = 0; i < n; i++) {

        bill["items"][i]["name"] = name[i];
        bill["items"][i]["qty"] = qty[i];
        bill["items"][i]["price"] = price[i];
        bill["items"][i]["total"] = total[i];
    }

    bill["grand_total"] = grandTotal;

    // APPEND BILL TO bills.json
    ofstream file("bills.json", ios::app);

    file << bill.dump(4) << endl;

    file.close();
}

// VIEW SALES
void viewSales() {

    ifstream file("bills.txt");

    string line;

    cout << "\n===== Sales Records =====\n";

    while(getline(file, line)) {
        cout << line << endl;
    }

    file.close();
}

// DISCOUNT
float applyDiscount(float total) {

    if(total > 5000)
        total *= 0.9;

    return total;
}

// TAX
float addTax(float total) {

    return total * 1.17;
}
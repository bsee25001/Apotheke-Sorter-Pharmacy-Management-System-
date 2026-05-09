#include "inventory.h"

// GLOBALS
int   totalProducts = 0;
float lowStockThreshold = 10.0f;
int   expiryWarnDays = 30;

// MENU
void inventoryMenu() {

    int     capacity = 100;
    string* names = new string[capacity];
    string* categories = new string[capacity];
    int* ids = new int[capacity];
    float* prices = new float[capacity];
    int* qty = new int[capacity];
    string* expiry = new string[capacity];
    int* salesCount = new int[capacity];
    int     count = 0;

    for (int i = 0; i < capacity; i++) salesCount[i] = 0;

    loadProducts(names, categories, ids, prices, qty, expiry, count);

    int choice;
    do {
        cout << "\n===== Inventory & Marketing =====\n";
        cout << "1. View Full Inventory\n";
        cout << "2. Browse Products (Customer)\n";
        cout << "3. Search & Buy\n";
        cout << "4. High Stock Deals\n";
        cout << "5. Slow Moving Promotions\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
        case 1: viewInventory(names, categories, ids, prices, qty, expiry, salesCount, count);  break;
        case 2: customerBrowse(names, categories, ids, prices, qty, expiry, salesCount, count); break;
        case 3: searchAndBuy(names, categories, ids, prices, qty, expiry, salesCount, count);   break;
        case 4: advertiseHighStock(names, categories, prices, qty, expiry, count);              break;
        case 5: advertiseSlowMoving(names, categories, prices, qty, expiry, salesCount, count); break;
        }

    } while (choice != 0);

    freeArrays(names, categories, ids, prices, qty, expiry, salesCount);
}

// LOAD PRODUCTS
void loadProducts(string* names, string* categories, int* ids,
    float* prices, int* qty, string* expiry, int& count) {

    ifstream file("Products.json");

    if (!file) {
        cout << "Products.json not found!\n";
        return;
    }

    json data;
    file >> data;
    file.close();

    count = 0;

    for (auto& cat : data["categories"]) {

        string catName = cat["category"].get<string>();

        for (auto& p : cat["products"]) {
            ids[count] = p["id"].get<int>();
            names[count] = p["name"].get<string>();
            categories[count] = catName;
            prices[count] = p["price"].get<float>();
            qty[count] = p["stock"].get<int>();
            expiry[count] = p["expiry_date"].get<string>();
            count++;
        }
    }

    totalProducts = count;
    cout << "Loaded " << count << " products.\n";
}

// UPDATE STOCK IN Products.json
void updateStock(int* ids, int* qty, int count) {

    ifstream fileIn("Products.json");
    if (!fileIn) return;

    json data;
    fileIn >> data;
    fileIn.close();

    for (auto& cat : data["categories"])
        for (auto& p : cat["products"])
            for (int i = 0; i < count; i++)
                if (ids[i] == p["id"].get<int>()) {
                    p["stock"] = qty[i];
                    break;
                }

    ofstream fileOut("Products.json");
    fileOut << setw(4) << data << endl;
    fileOut.close();
}

// VIEW FULL INVENTORY
void viewInventory(string* names, string* categories, int* ids,
    float* prices, int* qty, string* expiry,
    int* salesCount, int count) {

    if (count == 0) { cout << "No products loaded.\n"; return; }

    float totalValue = recursiveTotalValue(prices, qty, count);

    cout << "\n===== Full Inventory =====\n";
    cout << left;
    cout.width(4);  cout << "ID";
    cout.width(22); cout << "Name";
    cout.width(20); cout << "Category";
    cout.width(8);  cout << "Price";
    cout.width(7);  cout << "Stock";
    cout.width(13); cout << "Expiry";
    cout << "Status\n";
    cout << string(85, '-') << "\n";

    for (int i = 0; i < count; i++) {

        cout.width(4);  cout << ids[i];
        cout.width(22); cout << names[i];
        cout.width(20); cout << categories[i];
        cout.width(8);  cout << prices[i];
        cout.width(7);  cout << qty[i];
        cout.width(13); cout << expiry[i];

        if (isExpired(expiry[i]))
            cout << "EXPIRED";
        else if (isNearExpiry(expiry[i]))
            cout << "Near Expiry (" << daysUntilExpiry(expiry[i]) << " days)";
        else if (qty[i] <= (int)lowStockThreshold)
            cout << "Low Stock";
        else
            cout << "OK";

        cout << "\n";
    }

    cout << string(85, '-') << "\n";
    cout << "Total Inventory Value: Rs. " << totalValue << "\n";
}

// CUSTOMER BROWSE
void customerBrowse(string* names, string* categories, int* ids,
    float* prices, int* qty, string* expiry,
    int* salesCount, int count) {

    cout << "\n===== Available Products =====\n";
    cout << left;
    cout.width(4);  cout << "ID";
    cout.width(22); cout << "Name";
    cout.width(20); cout << "Type";
    cout.width(10); cout << "Price";
    cout.width(7);  cout << "Stock";
    cout << "Note\n";
    cout << string(70, '-') << "\n";

    bool anyShown = false;

    for (int i = 0; i < count; i++) {

        if (isExpired(expiry[i])) continue;
        if (qty[i] == 0) continue;

        anyShown = true;
        cout.width(4);  cout << ids[i];
        cout.width(22); cout << names[i];
        cout.width(20); cout << categories[i];
        cout.width(10); cout << prices[i];
        cout.width(7);  cout << qty[i];

        if (isNearExpiry(expiry[i]))
            cout << "Expires in " << daysUntilExpiry(expiry[i]) << " days";

        cout << "\n";
    }

    if (!anyShown)
        cout << "No products available.\n";
}

// SEARCH & BUY
void searchAndBuy(string* names, string* categories, int* ids,
    float* prices, int* qty, string* expiry,
    int* salesCount, int count) {

    int productId;
    cout << "\nEnter Product ID: ";
    cin >> productId;

    int index = recursiveFindById(ids, count, productId, 0);

    if (index == -1) {
        cout << "Product ID " << productId << " not found.\n";
        return;
    }

    if (isExpired(expiry[index])) {
        cout << "This product is unavailable.\n";
        return;
    }

    cout << "\nID       : " << ids[index] << "\n";
    cout << "Name     : " << names[index] << "\n";
    cout << "Type     : " << categories[index] << "\n";
    cout << "Price    : Rs. " << prices[index] << "\n";
    cout << "In Stock : " << qty[index] << " units\n";

    if (isNearExpiry(expiry[index]))
        cout << "WARNING: Expires in " << daysUntilExpiry(expiry[index]) << " days.\n";

    if (qty[index] == 0) {
        cout << "Out of stock.\n";
        return;
    }

    int want;
    cout << "How many units? ";
    cin >> want;

    if (want <= 0) { cout << "Cancelled.\n"; return; }

    if (want > qty[index]) {
        cout << "Insufficient stock. Only " << qty[index] << " available.\n";
        return;
    }

    float bill = want * prices[index];
    char  confirm;
    cout << "Confirm: " << want << " x " << names[index]
        << " = Rs. " << bill << " ? (y/n): ";
    cin >> confirm;

    if (confirm == 'y' || confirm == 'Y') {

        qty[index] -= want;
        salesCount[index] += want;

        cout << "Purchase confirmed.\n";
        cout << "Total     : Rs. " << bill << "\n";
        cout << "Remaining : " << qty[index] << " units\n";

        updateStock(ids, qty, count);

    }
    else {
        cout << "Purchase cancelled.\n";
    }
}

// HIGH STOCK DEALS
void advertiseHighStock(string* names, string* categories,
    float* prices, int* qty, string* expiry, int count) {

    if (count == 0) return;

    int totalQty = 0;
    for (int i = 0; i < count; i++) totalQty += qty[i];
    float avg = (float)totalQty / count;

    cout << "\n>>> High Stock Deals (10% OFF) <<<\n";
    bool found = false;

    for (int i = 0; i < count; i++) {

        if (isExpired(expiry[i])) continue;
        if (qty[i] == 0) continue;

        if (qty[i] > avg * 2.0f) {
            cout << "  [BULK] " << names[i] << " (" << categories[i] << ")"
                << "  Rs. " << prices[i] * 0.90f << " per unit\n";
            found = true;
        }
    }

    if (!found) cout << "  No bulk deals right now.\n";
}

// SLOW MOVING PROMOTIONS
void advertiseSlowMoving(string* names, string* categories,
    float* prices, int* qty, string* expiry,
    int* salesCount, int count) {

    if (count == 0) return;

    int totalSales = 0;
    for (int i = 0; i < count; i++) totalSales += salesCount[i];
    float avg = (float)totalSales / count;

    cout << "\n>>> Special Promotions (15% OFF) <<<\n";
    bool found = false;

    for (int i = 0; i < count; i++) {

        if (isExpired(expiry[i])) continue;
        if (qty[i] == 0) continue;

        if (salesCount[i] < avg * 0.5f) {
            cout << "  [PROMO] " << names[i] << " (" << categories[i] << ")"
                << "  Was: Rs. " << prices[i]
                << "  Now: Rs. " << prices[i] * 0.85f << "\n";
            found = true;
        }
    }

    if (!found) cout << "  No promotions today.\n";
}

// DAYS UNTIL EXPIRY
int daysUntilExpiry(string expiry) {

    int year = stoi(expiry.substr(0, 4));
    int month = stoi(expiry.substr(5, 2));
    int day = stoi(expiry.substr(8, 2));

    int todayYear = 2025, todayMonth = 5, todayDay = 8;

    return (year * 365 + month * 30 + day) -
        (todayYear * 365 + todayMonth * 30 + todayDay);
}

bool isExpired(string expiry) {
    return daysUntilExpiry(expiry) < 0;
}

bool isNearExpiry(string expiry) {
    int d = daysUntilExpiry(expiry);
    return d >= 0 && d <= expiryWarnDays;
}

// RECURSIVE SEARCH BY ID
int recursiveFindById(int* ids, int count, int targetId, int index) {
    if (index >= count) return -1;
    if (ids[index] == targetId) return index;
    return recursiveFindById(ids, count, targetId, index + 1);
}

// RECURSIVE TOTAL VALUE
float recursiveTotalValue(float* prices, int* qty, int n) {
    if (n == 0) return 0.0f;
    return (prices[n - 1] * qty[n - 1]) + recursiveTotalValue(prices, qty, n - 1);
}

// RESIZE ARRAYS
void resizeArrays(string*& names, string*& categories, int*& ids,
    float*& prices, int*& qty, string*& expiry,
    int*& salesCount, int oldSize, int newSize) {

    string* newNames = new string[newSize];
    string* newCategories = new string[newSize];
    int* newIds = new int[newSize];
    float* newPrices = new float[newSize];
    int* newQty = new int[newSize];
    string* newExpiry = new string[newSize];
    int* newSalesCount = new int[newSize];

    for (int i = 0; i < oldSize; i++) {
        newNames[i] = names[i];
        newCategories[i] = categories[i];
        newIds[i] = ids[i];
        newPrices[i] = prices[i];
        newQty[i] = qty[i];
        newExpiry[i] = expiry[i];
        newSalesCount[i] = salesCount[i];
    }

    freeArrays(names, categories, ids, prices, qty, expiry, salesCount);

    names = newNames;
    categories = newCategories;
    ids = newIds;
    prices = newPrices;
    qty = newQty;
    expiry = newExpiry;
    salesCount = newSalesCount;
}

// FREE ARRAYS
void freeArrays(string* names, string* categories, int* ids,
    float* prices, int* qty, string* expiry, int* salesCount) {
    delete[] names;
    delete[] categories;
    delete[] ids;
    delete[] prices;
    delete[] qty;
    delete[] expiry;
    delete[] salesCount;
}

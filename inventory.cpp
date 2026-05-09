#include "inventory.h"

// ── Global Variable Definitions ───────────────────────────────────
int    totalProducts     = 0;
float  lowStockThreshold = 10.0f;
int    expiryWarnDays    = 30;
string inventoryFile     = "inventory.json";

// ─────────────────────────────────────────────────────────────────
// MENU
// ─────────────────────────────────────────────────────────────────
void inventoryMenu() {
    // DMA: allocate arrays for up to 100 products
    int capacity       = 100;
    string* names      = new string[capacity];
    int*    qty        = new int[capacity];
    float*  prices     = new float[capacity];
    string* expiry     = new string[capacity];
    int*    salesCount = new int[capacity];
    int     count      = 0;

    loadInventory(names, qty, prices, expiry, salesCount, count);

    int choice;
    do {
        cout << "\n===== Inventory & Marketing =====\n";
        cout << "1. Add Product\n";
        cout << "2. View Full Inventory (Admin)\n";
        cout << "3. Search Product (Customer)\n";
        cout << "4. Browse Available Products (Customer View)\n";
        cout << "5. View Slow-Moving Promotions\n";
        cout << "6. View High-Stock Bulk Deals\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch(choice) {
            case 1: addProduct(names, qty, prices, expiry, salesCount, count);           break;
            case 2: viewInventory(names, qty, prices, expiry, salesCount, count);        break;
            case 3: searchProduct(names, qty, prices, expiry, salesCount, count);        break;
            case 4: customerBrowse(names, qty, prices, expiry, salesCount, count);       break;
            case 5: advertiseSlowMoving(names, qty, prices, expiry, salesCount, count);  break;
            case 6: advertiseHighStock(names, qty, prices, expiry, count);               break;
        }

    } while(choice != 0);

    // FREE MEMORY
    freeArrays(names, qty, prices, expiry, salesCount);
}

// ─────────────────────────────────────────────────────────────────
// LOAD INVENTORY FROM JSON
// ─────────────────────────────────────────────────────────────────
void loadInventory(string*& names, int*& qty, float*& prices,
                   string*& expiry, int*& salesCount, int& count) {
    ifstream file(inventoryFile);
    if(!file.is_open()) {
        cout << "[INFO] No existing inventory file found. Starting fresh.\n";
        return;
    }

    json data;
    file >> data;
    file.close();

    count = 0;
    for(auto& item : data["products"]) {
        names[count]      = item["name"].get<string>();
        qty[count]        = item["qty"].get<int>();
        prices[count]     = item["price"].get<float>();
        expiry[count]     = item["expiry"].get<string>();
        salesCount[count] = item["salesCount"].get<int>();
        count++;
    }

    totalProducts = count;
    cout << "[INFO] Loaded " << count << " products from inventory.\n";
}

// ─────────────────────────────────────────────────────────────────
// SAVE INVENTORY TO JSON
// ─────────────────────────────────────────────────────────────────
void saveInventory(string* names, int* qty, float* prices,
                   string* expiry, int* salesCount, int count) {
    json data;
    for(int i = 0; i < count; i++) {
        data["products"][i]["name"]       = names[i];
        data["products"][i]["qty"]        = qty[i];
        data["products"][i]["price"]      = prices[i];
        data["products"][i]["expiry"]     = expiry[i];
        data["products"][i]["salesCount"] = salesCount[i];
    }
    ofstream file(inventoryFile);
    file << data.dump(4) << endl;
    file.close();
    cout << "[INFO] Inventory saved.\n";
}

// ─────────────────────────────────────────────────────────────────
// ADD PRODUCT
// ─────────────────────────────────────────────────────────────────
void addProduct(string*& names, int*& qty, float*& prices,
                string*& expiry, int*& salesCount, int& count) {
    cout << "\n--- Add New Product ---\n";
    cout << "Product Name: ";         cin >> names[count];
    cout << "Quantity: ";             cin >> qty[count];
    cout << "Price per unit: ";       cin >> prices[count];
    cout << "Expiry (YYYY-MM-DD): ";  cin >> expiry[count];
    salesCount[count] = 0;
    count++;
    totalProducts = count;
    saveInventory(names, qty, prices, expiry, salesCount, count);
    cout << "[SUCCESS] Product added.\n";
}

// ─────────────────────────────────────────────────────────────────
// VIEW FULL INVENTORY (Admin)
// ─────────────────────────────────────────────────────────────────
void viewInventory(string* names, int* qty, float* prices,
                   string* expiry, int* salesCount, int count) {
    if(count == 0) { cout << "[INFO] Inventory is empty.\n"; return; }

    float totalValue = recursiveTotalValue(prices, qty, count);

    cout << "\n===== Full Inventory =====\n";
    cout << left;
    cout.width(20); cout << "Name";
    cout.width(8);  cout << "Qty";
    cout.width(10); cout << "Price";
    cout.width(14); cout << "Expiry";
    cout.width(8);  cout << "Sales";
    cout << "Status\n";
    cout << string(72, '-') << "\n";

    for(int i = 0; i < count; i++) {
        cout.width(20); cout << names[i];
        cout.width(8);  cout << qty[i];
        cout.width(10); cout << prices[i];
        cout.width(14); cout << expiry[i];
        cout.width(8);  cout << salesCount[i];

        if(isExpired(expiry[i]))
            cout << "*** EXPIRED ***";
        else if(isNearExpiry(expiry[i]))
            cout << "! Near Expiry (" << daysUntilExpiry(expiry[i]) << " days)";
        else if(qty[i] <= (int)lowStockThreshold)
            cout << "Low Stock";
        else
            cout << "OK";

        cout << "\n";
    }

    cout << string(72, '-') << "\n";
    cout << "Total Inventory Value: Rs. " << totalValue << "\n";
}

// ─────────────────────────────────────────────────────────────────
// SEARCH PRODUCT — Recursive (Customer-facing)
// ─────────────────────────────────────────────────────────────────
void searchProduct(string* names, int* qty, float* prices,
                   string* expiry, int* salesCount, int count) {
    string query;
    cout << "\nEnter product name to search: ";
    cin >> query;

    int index = recursiveFindProduct(names, count, query, 0);

    if(index == -1) {
        cout << "[NOT FOUND] \"" << query << "\" is not in our inventory.\n";
        return;
    }

    // Never reveal expired products to customer
    if(isExpired(expiry[index])) {
        cout << "[UNAVAILABLE] Sorry, this product is currently unavailable.\n";
        return;
    }

    cout << "\n--- Product Found ---\n";
    cout << "Name    : " << names[index]  << "\n";
    cout << "Price   : Rs. " << prices[index] << " per unit\n";
    cout << "In Stock: " << qty[index]   << " units\n";

    if(isNearExpiry(expiry[index]))
        cout << "[WARNING] This product expires in "
             << daysUntilExpiry(expiry[index])
             << " days. Please use promptly.\n";

    if(qty[index] == 0) {
        cout << "[OUT OF STOCK] This product is currently out of stock.\n";
        return;
    }

    int want;
    cout << "How many units would you like? ";
    cin >> want;

    if(want <= 0) {
        cout << "[CANCELLED] No units selected.\n";
    } else if(want > qty[index]) {
        cout << "[SORRY] Only " << qty[index] << " units available.\n";
    } else {
        qty[index]        -= want;
        salesCount[index] += want;
        float bill         = want * prices[index];
        cout << "[ADDED] " << want << " x " << names[index]
             << " = Rs. " << bill << "\n";
        saveInventory(names, qty, prices, expiry, salesCount, count);
    }
}

// ─────────────────────────────────────────────────────────────────
// CUSTOMER BROWSE — hides expired, flags near-expiry
// ─────────────────────────────────────────────────────────────────
void customerBrowse(string* names, int* qty, float* prices,
                    string* expiry, int* salesCount, int count) {
    cout << "\n===== Available Products =====\n";
    cout << left;
    cout.width(20); cout << "Name";
    cout.width(12); cout << "Price";
    cout.width(8);  cout << "Stock";
    cout << "Note\n";
    cout << string(58, '-') << "\n";

    bool anyShown = false;
    for(int i = 0; i < count; i++) {
        if(isExpired(expiry[i])) continue;   // silently skip expired
        if(qty[i] == 0) continue;            // skip out-of-stock

        anyShown = true;
        cout.width(20); cout << names[i];
        cout.width(12); cout << "Rs. " + to_string((int)prices[i]);
        cout.width(8);  cout << qty[i];

        if(isNearExpiry(expiry[i]))
            cout << "** Expires in " << daysUntilExpiry(expiry[i]) << " days - use promptly";

        cout << "\n";
    }

    if(!anyShown) {
        cout << "[INFO] No products currently available.\n";
        return;
    }

    cout << "\n";
    advertiseSlowMoving(names, qty, prices, expiry, salesCount, count);
    advertiseHighStock(names, qty, prices, expiry, count);
}

// ─────────────────────────────────────────────────────────────────
// ADVERTISE SLOW-MOVING PRODUCTS (15% off)
// ─────────────────────────────────────────────────────────────────
void advertiseSlowMoving(string* names, int* qty, float* prices,
                         string* expiry, int* salesCount, int count) {
    if(count == 0) return;

    int totalSales = 0;
    for(int i = 0; i < count; i++)
        totalSales += salesCount[i];
    float avgSales = (float)totalSales / count;

    cout << "\n>>> Special Promotions (Limited Time) <<<\n";
    bool found = false;

    for(int i = 0; i < count; i++) {
        if(isExpired(expiry[i])) continue;
        if(qty[i] == 0) continue;

        // Slow-moving: sales below 50% of average
        if(salesCount[i] < avgSales * 0.5f) {
            float discounted = prices[i] * 0.85f;
            cout << "  [PROMO] " << names[i]
                 << "  |  Was: Rs. " << prices[i]
                 << "  ->  NOW Rs. " << discounted
                 << "  (15% OFF!)\n";
            found = true;
        }
    }

    if(!found)
        cout << "  No special promotions today. Check back soon!\n";
}

// ─────────────────────────────────────────────────────────────────
// ADVERTISE HIGH-STOCK PRODUCTS (10% bulk deal)
// ─────────────────────────────────────────────────────────────────
void advertiseHighStock(string* names, int* qty, float* prices,
                        string* expiry, int count) {
    if(count == 0) return;

    int totalQty = 0;
    for(int i = 0; i < count; i++)
        totalQty += qty[i];
    float avgQty = (float)totalQty / count;

    cout << "\n>>> Bulk Buy Deals <<<\n";
    bool found = false;

    for(int i = 0; i < count; i++) {
        if(isExpired(expiry[i])) continue;
        if(qty[i] == 0) continue;

        // High stock: qty more than 2x the average
        if(qty[i] > avgQty * 2.0f) {
            float bulkPrice = prices[i] * 0.90f;
            cout << "  [BULK]  " << names[i]
                 << "  |  Buy more, save more! Rs. " << bulkPrice
                 << " per unit  (10% OFF on bulk)\n";
            found = true;
        }
    }

    if(!found)
        cout << "  No bulk deals available right now.\n";
}

// ─────────────────────────────────────────────────────────────────
// UTILITY: DAYS UNTIL EXPIRY  (format: YYYY-MM-DD)
// ─────────────────────────────────────────────────────────────────
int daysUntilExpiry(string expiry) {
    int year  = stoi(expiry.substr(0, 4));
    int month = stoi(expiry.substr(5, 2));
    int day   = stoi(expiry.substr(8, 2));

    // Today: 2025-05-07
    int todayYear = 2025, todayMonth = 5, todayDay = 7;

    int expiryDays = year * 365 + month * 30 + day;
    int todayDays  = todayYear * 365 + todayMonth * 30 + todayDay;

    return expiryDays - todayDays;
}

bool isExpired(string expiry) {
    return daysUntilExpiry(expiry) < 0;
}

bool isNearExpiry(string expiry) {
    int days = daysUntilExpiry(expiry);
    return (days >= 0 && days <= expiryWarnDays);
}

// ─────────────────────────────────────────────────────────────────
// RECURSIVE SEARCH  (searches names array by exact match)
// ─────────────────────────────────────────────────────────────────
int recursiveFindProduct(string* names, int count, string target, int index) {
    if(index >= count) return -1;             // base case: not found
    if(names[index] == target) return index;  // base case: match
    return recursiveFindProduct(names, count, target, index + 1);
}

// ─────────────────────────────────────────────────────────────────
// RECURSIVE TOTAL INVENTORY VALUE
// ─────────────────────────────────────────────────────────────────
float recursiveTotalValue(float* prices, int* qty, int n) {
    if(n == 0) return 0.0f;                   // base case
    return (prices[n-1] * qty[n-1]) + recursiveTotalValue(prices, qty, n-1);
}

// ─────────────────────────────────────────────────────────────────
// DMA: RESIZE ARRAYS
// ─────────────────────────────────────────────────────────────────
void resizeArrays(string*& names, int*& qty, float*& prices,
                  string*& expiry, int*& salesCount,
                  int oldSize, int newSize) {
    string* newNames      = new string[newSize];
    int*    newQty        = new int[newSize];
    float*  newPrices     = new float[newSize];
    string* newExpiry     = new string[newSize];
    int*    newSalesCount = new int[newSize];

    for(int i = 0; i < oldSize; i++) {
        newNames[i]      = names[i];
        newQty[i]        = qty[i];
        newPrices[i]     = prices[i];
        newExpiry[i]     = expiry[i];
        newSalesCount[i] = salesCount[i];
    }

    freeArrays(names, qty, prices, expiry, salesCount);

    names      = newNames;
    qty        = newQty;
    prices     = newPrices;
    expiry     = newExpiry;
    salesCount = newSalesCount;
}

// ─────────────────────────────────────────────────────────────────
// DMA: FREE ALL ARRAYS
// ─────────────────────────────────────────────────────────────────
void freeArrays(string* names, int* qty, float* prices,
                string* expiry, int* salesCount) {
    delete[] names;
    delete[] qty;
    delete[] prices;
    delete[] expiry;
    delete[] salesCount;
}

#include "functions.h"

int staffCount  = 0;
int billCounter = 1000;

Staff*      staffList  = nullptr;
ClockEntry* clockLog   = nullptr;
int         clockCount = 0;
int         clockCap   = 0;


void loadStaff() {
    ifstream f("staff.json");
    if (!f) {
        cout << "staff.json not found.\n";
        return;
    }

    json data;
    f >> data;
    f.close();

    staffCount = (int)data.size();
    delete[] staffList;
    staffList = new Staff[staffCount];

    for (int i = 0; i < staffCount; i++) {
        staffList[i].id   = data[i]["id"];
        staffList[i].name = data[i]["name"];
        staffList[i].role = data[i]["role"];
    }

    cout << staffCount << " staff member(s) loaded.\n";
}

static void growLog() {
    int newCap = (clockCap == 0) ? 10 : clockCap * 2;
    ClockEntry* temp = new ClockEntry[newCap];
    for (int i = 0; i < clockCount; i++)
        temp[i] = clockLog[i];
    delete[] clockLog;
    clockLog = temp;
    clockCap = newCap;
}

static int findStaff(int id) {
    for (int i = 0; i < staffCount; i++)
        if (staffList[i].id == id) return i;
    return -1;
}

void clockIn() {
    if (staffCount == 0) {
        cout << "No staff loaded.\n";
        return;
    }

    cout << "\nStaff List:\n";
    for (int i = 0; i < staffCount; i++)
        cout << "  " << staffList[i].id << " - " << staffList[i].name
             << " (" << staffList[i].role << ")\n";

    int    id;
    string date, time;
    cout << "\nStaff ID : ";        cin >> id;
    cout << "Date (YYYY-MM-DD): "; cin >> date;
    cout << "Time (HH:MM)     : "; cin >> time;

    int idx = findStaff(id);
    if (idx == -1) { cout << "Staff not found.\n"; return; }

    for (int i = 0; i < clockCount; i++) {
        if (clockLog[i].staffId == id && clockLog[i].date == date && clockLog[i].clockOut.empty()) {
            cout << staffList[idx].name << " is already clocked in on " << date << ".\n";
            return;
        }
    }

    if (clockCount == clockCap) growLog();

    ClockEntry& e = clockLog[clockCount++];
    e.staffId     = id;
    e.staffName   = staffList[idx].name;
    e.date        = date;
    e.clockIn     = time;
    e.clockOut    = "";
    e.hoursWorked = 0.0;

    cout << staffList[idx].name << " clocked in at " << time << " on " << date << ".\n";
    saveTimesheet();
}

void clockOut() {
    int    id;
    string date, time;
    cout << "Staff ID : ";        cin >> id;
    cout << "Date (YYYY-MM-DD): "; cin >> date;
    cout << "Time (HH:MM)     : "; cin >> time;

    int idx = findStaff(id);
    if (idx == -1) { cout << "Staff not found.\n"; return; }

    for (int i = 0; i < clockCount; i++) {
        if (clockLog[i].staffId == id && clockLog[i].date == date && clockLog[i].clockOut.empty()) {

            clockLog[i].clockOut = time;

            int hIn  = stoi(clockLog[i].clockIn.substr(0, 2));
            int mIn  = stoi(clockLog[i].clockIn.substr(3, 2));
            int hOut = stoi(time.substr(0, 2));
            int mOut = stoi(time.substr(3, 2));

            double inMins  = hIn  * 60.0 + mIn;
            double outMins = hOut * 60.0 + mOut;
            if (outMins < inMins) outMins += 24 * 60;

            clockLog[i].hoursWorked = (outMins - inMins) / 60.0;

            cout << staffList[idx].name << " clocked out at " << time
                 << ". Hours: " << fixed << setprecision(2) << clockLog[i].hoursWorked << "\n";

            saveTimesheet();
            return;
        }
    }
    cout << "No open clock-in found for " << staffList[idx].name << " on " << date << ".\n";
}

double totalHours(int idx, int staffId) {
    if (idx >= clockCount) return 0.0;
    double here = (clockLog[idx].staffId == staffId) ? clockLog[idx].hoursWorked : 0.0;
    return here + totalHours(idx + 1, staffId);
}

void printEntries(int idx) {
    if (idx >= clockCount) return;
    ClockEntry& e = clockLog[idx];
    cout << left
         << setw(5)  << e.staffId
         << setw(18) << e.staffName
         << setw(13) << e.date
         << setw(8)  << e.clockIn
         << setw(8)  << (e.clockOut.empty() ? "---" : e.clockOut)
         << fixed << setprecision(2) << e.hoursWorked << " hrs\n";
    printEntries(idx + 1);
}

void viewTimesheet() {
    if (clockCount == 0) { cout << "No entries yet.\n"; return; }

    cout << "\n" << left
         << setw(5)  << "ID"
         << setw(18) << "Name"
         << setw(13) << "Date"
         << setw(8)  << "In"
         << setw(8)  << "Out"
         << "Hours\n"
         << string(60, '-') << "\n";

    printEntries(0);

    cout << "\nTotal Hours:\n";
    for (int i = 0; i < staffCount; i++) {
        double total = totalHours(0, staffList[i].id);
        if (total > 0)
            cout << "  " << staffList[i].name << ": "
                 << fixed << setprecision(2) << total << " hrs\n";
    }
}

void saveTimesheet() {
    json arr = json::array();
    for (int i = 0; i < clockCount; i++) {
        json e;
        e["staff_id"]     = clockLog[i].staffId;
        e["staff_name"]   = clockLog[i].staffName;
        e["date"]         = clockLog[i].date;
        e["clock_in"]     = clockLog[i].clockIn;
        e["clock_out"]    = clockLog[i].clockOut;
        e["hours_worked"] = clockLog[i].hoursWorked;
        arr.push_back(e);
    }
    ofstream f("timesheet.json");
    f << setw(4) << arr << endl;
    f.close();
}

void loadTimesheet() {
    ifstream f("timesheet.json");
    if (!f) return;

    json arr;
    f >> arr;
    f.close();

    int n = (int)arr.size();
    if (n == 0) return;

    delete[] clockLog;
    clockLog   = new ClockEntry[n * 2];
    clockCap   = n * 2;
    clockCount = n;

    for (int i = 0; i < n; i++) {
        clockLog[i].staffId     = arr[i]["staff_id"];
        clockLog[i].staffName   = arr[i]["staff_name"];
        clockLog[i].date        = arr[i]["date"];
        clockLog[i].clockIn     = arr[i]["clock_in"];
        clockLog[i].clockOut    = arr[i]["clock_out"];
        clockLog[i].hoursWorked = arr[i]["hours_worked"];
    }
}

void timesheetMenu() {
    int choice;
    do {
        cout << "\n===== Staff Timesheet =====\n"
             << "1. Clock In\n"
             << "2. Clock Out\n"
             << "3. View Timesheet\n"
             << "0. Back\n"
             << "Choice: ";
        cin >> choice;
        switch (choice) {
            case 1: clockIn();       break;
            case 2: clockOut();      break;
            case 3: viewTimesheet(); break;
        }
    } while (choice != 0);
}


float calcTotal(float* totals, int n) {
    if (n == 0) return 0.0f;
    return totals[n - 1] + calcTotal(totals, n - 1);
}

void printReceipt(CartItem* cart, int n, float grandTotal) {
    cout << "\n========== RECEIPT ==========\n";
    cout << "Bill ID: " << billCounter << "\n";
    cout << left
         << setw(22) << "Item"
         << setw(6)  << "Qty"
         << setw(10) << "Price"
         << "Total\n"
         << string(45, '-') << "\n";

    for (int i = 0; i < n; i++)
        cout << left
             << setw(22) << cart[i].name
             << setw(6)  << cart[i].qty
             << setw(10) << cart[i].price
             << cart[i].total << "\n";

    cout << string(45, '-') << "\n"
         << "Grand Total: PKR " << fixed << setprecision(2) << grandTotal << "\n"
         << "=============================\n";
}

void saveBill(CartItem* cart, int n, float grandTotal) {
    json bill;
    bill["bill_id"]     = billCounter;
    bill["grand_total"] = grandTotal;

    for (int i = 0; i < n; i++) {
        json item;
        item["product_id"] = cart[i].productId;
        item["name"]       = cart[i].name;
        item["qty"]        = cart[i].qty;
        item["price"]      = cart[i].price;
        item["total"]      = cart[i].total;
        bill["items"].push_back(item);
    }

    ofstream f("bills.json", ios::app);
    f << setw(4) << bill << "\n";
    f.close();
}

void customerPurchase() {
    ifstream pf("Products.json");
    if (!pf) { cout << "Products.json not found.\n"; return; }

    json products;
    pf >> products;
    pf.close();

    cout << "\n===== Products =====\n"
         << left
         << setw(5)  << "ID"
         << setw(25) << "Name"
         << setw(12) << "Brand"
         << setw(10) << "Price"
         << "Stock\n"
         << string(60, '-') << "\n";

    for (auto& cat : products["categories"]) {
        cout << "[" << cat["category"].get<string>() << "]\n";
        for (auto& p : cat["products"])
            cout << setw(5)  << p["id"].get<int>()
                 << setw(25) << p["name"].get<string>()
                 << setw(12) << p["brand"].get<string>()
                 << setw(10) << p["price"].get<float>()
                 << p["stock"].get<int>() << "\n";
    }

    int n;
    cout << "\nHow many items do you want to buy? ";
    cin >> n;
    if (n <= 0) { cout << "Nothing to buy.\n"; return; }

    CartItem* cart   = new CartItem[n];
    float*    totals = new float[n];

    for (int i = 0; i < n; i++) {
        int  productId, qty;
        bool found = false;

        cout << "\nItem " << (i + 1) << ":\n";
        cout << "  Product ID : "; cin >> productId;
        cout << "  Quantity   : "; cin >> qty;

        for (auto& cat : products["categories"]) {
            for (auto& p : cat["products"]) {
                if (p["id"].get<int>() == productId) {
                    found = true;

                    int stock = p["stock"].get<int>();
                    if (qty > stock) {
                        cout << "  Only " << stock << " in stock.\n";
                        delete[] cart;
                        delete[] totals;
                        return;
                    }

                    cart[i].productId = productId;
                    cart[i].name      = p["name"].get<string>();
                    cart[i].qty       = qty;
                    cart[i].price     = p["price"].get<float>();
                    cart[i].total     = qty * cart[i].price;
                    totals[i]         = cart[i].total;
                    p["stock"]        = stock - qty;

                    cout << "  Added: " << cart[i].name << " x" << qty
                         << " = PKR " << cart[i].total << "\n";
                    break;
                }
            }
            if (found) break;
        }

        if (!found) {
            cout << "  Product not found.\n";
            delete[] cart;
            delete[] totals;
            return;
        }
    }

    float grandTotal = calcTotal(totals, n);

    printReceipt(cart, n, grandTotal);

    ofstream uf("Products.json");
    uf << setw(4) << products << endl;
    uf.close();

    saveBill(cart, n, grandTotal);
    cout << "Bill saved.\n";

    billCounter++;

    delete[] cart;
    delete[] totals;
}

void purchaseMenu() {
    int choice;
    do {
        cout << "\n===== Customer Purchase =====\n"
             << "1. Buy Products\n"
             << "0. Back\n"
             << "Choice: ";
        cin >> choice;
        if (choice == 1) customerPurchase();
    } while (choice != 0);
}

void freeMemory() {
    delete[] staffList;
    delete[] clockLog;
    staffList = nullptr;
    clockLog  = nullptr;
}

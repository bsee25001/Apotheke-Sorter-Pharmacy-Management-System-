#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

extern int staffCount;
extern int billCounter;

struct Staff {
    int    id;
    string name;
    string role;
};

struct ClockEntry {
    int    staffId;
    string staffName;
    string date;
    string clockIn;
    string clockOut;
    double hoursWorked;
};

struct CartItem {
    int    productId;
    string name;
    int    qty;
    float  price;
    float  total;
};

extern Staff*      staffList;
extern ClockEntry* clockLog;
extern int         clockCount;
extern int         clockCap;

void loadStaff();
void clockIn();
void clockOut();
void viewTimesheet();
void saveTimesheet();
void loadTimesheet();
double totalHours(int idx, int staffId);
void printEntries(int idx);
void timesheetMenu();

void customerPurchase();
float calcTotal(float* totals, int n);
void saveBill(CartItem* cart, int n, float grandTotal);
void printReceipt(CartItem* cart, int n, float grandTotal);
void purchaseMenu();

void freeMemory();

#endif

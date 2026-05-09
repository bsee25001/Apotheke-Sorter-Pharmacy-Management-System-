#ifndef INVENTORY_H
#define INVENTORY_H

#include <iostream>
#include <fstream>
#include <string>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

// ── Global Variables (defined in inventory.cpp) ───────────────────
extern int    totalProducts;       // total products currently in inventory
extern float  lowStockThreshold;   // qty at or below this triggers Low Stock
extern int    expiryWarnDays;      // days-to-expiry below this triggers warning
extern string inventoryFile;       // path to JSON inventory file

// ── Function Declarations ─────────────────────────────────────────

// Menu driver
void inventoryMenu();

// Inventory management
void loadInventory(string*& names, int*& qty, float*& prices,
                   string*& expiry, int*& salesCount, int& count);
void saveInventory(string* names, int* qty, float* prices,
                   string* expiry, int* salesCount, int count);
void addProduct(string*& names, int*& qty, float*& prices,
                string*& expiry, int*& salesCount, int& count);
void viewInventory(string* names, int* qty, float* prices,
                   string* expiry, int* salesCount, int count);

// Customer-facing
void searchProduct(string* names, int* qty, float* prices,
                   string* expiry, int* salesCount, int count);
void customerBrowse(string* names, int* qty, float* prices,
                    string* expiry, int* salesCount, int count);

// Marketing
void advertiseSlowMoving(string* names, int* qty, float* prices,
                         string* expiry, int* salesCount, int count);
void advertiseHighStock(string* names, int* qty, float* prices,
                        string* expiry, int count);

// Utility
int   daysUntilExpiry(string expiry);
bool  isExpired(string expiry);
bool  isNearExpiry(string expiry);

// Recursion
int   recursiveFindProduct(string* names, int count, string target, int index);
float recursiveTotalValue(float* prices, int* qty, int n);

// DMA
void resizeArrays(string*& names, int*& qty, float*& prices,
                  string*& expiry, int*& salesCount, int oldSize, int newSize);
void freeArrays(string* names, int* qty, float* prices,
                string* expiry, int* salesCount);

#endif

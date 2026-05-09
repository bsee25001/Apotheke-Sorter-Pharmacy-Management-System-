#ifndef INVENTORY_H
#define INVENTORY_H

#include <iostream>
#include <fstream>
#include <iomanip>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

// GLOBALS
extern int   totalProducts;
extern float lowStockThreshold;
extern int   expiryWarnDays;

// MENU
void inventoryMenu();

// LOAD / SAVE
void loadProducts(string* names, string* categories, int* ids,
    float* prices, int* qty, string* expiry, int& count);
void updateStock(int* ids, int* qty, int count);

// ADMIN
void viewInventory(string* names, string* categories, int* ids,
    float* prices, int* qty, string* expiry,
    int* salesCount, int count);

// CUSTOMER
void customerBrowse(string* names, string* categories, int* ids,
    float* prices, int* qty, string* expiry,
    int* salesCount, int count);
void searchAndBuy(string* names, string* categories, int* ids,
    float* prices, int* qty, string* expiry,
    int* salesCount, int count);

// MARKETING
void advertiseHighStock(string* names, string* categories,
    float* prices, int* qty, string* expiry, int count);
void advertiseSlowMoving(string* names, string* categories,
    float* prices, int* qty, string* expiry,
    int* salesCount, int count);

// UTILITY
int  daysUntilExpiry(string expiry);
bool isExpired(string expiry);
bool isNearExpiry(string expiry);

// RECURSION
int   recursiveFindById(int* ids, int count, int targetId, int index);
float recursiveTotalValue(float* prices, int* qty, int n);

// DMA
void resizeArrays(string*& names, string*& categories, int*& ids,
    float*& prices, int*& qty, string*& expiry,
    int*& salesCount, int oldSize, int newSize);
void freeArrays(string* names, string* categories, int* ids,
    float* prices, int* qty, string* expiry, int* salesCount);

#endif
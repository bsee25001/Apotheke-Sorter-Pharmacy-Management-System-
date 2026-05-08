#ifndef FINANCE_H
#define FINANCE_H

#include <iostream>
#include <fstream>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

void menu();
void generateBill();
float calculateTotal(float* , int);
float recursiveTotal(float* , int);
void saveToFile(int, string[], int[], float[], float[],float[]);
void saveJson(int, string[], int[], float[], float[],float[]);
void viewSales();
float applyDiscount(float);
float addTax(float);

#endif
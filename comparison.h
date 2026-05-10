#ifndef COMPARISON_H
#define COMPARISON_H

#include <iostream>
#include <string>
#include <vector>
#include "json.hpp" 

using namespace std;
using json = nlohmann::json;

// Struct for Price Comparison
struct MedPrice {
    string name;
    double ourPrice;
    double competitorA;
    double competitorB;
};

// Struct for Clinical Staff
struct StaffMember {
    string role;
    bool isAvailable;
    string returnTime;
};

// Function Declarations
void comparisonModuleMenu();
void runPriceComparison();
void checkClinicalStaff();

#endif
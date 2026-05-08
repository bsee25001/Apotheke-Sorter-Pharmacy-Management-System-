#include "finance.h"

int billCounter = 1000;

void menu(){
    int choice;
    do{
      











    } while(choice != 0);

} 

// recursive total
float recursiveTotal(float *total, int n) {

    if(n == 0)
        return 0;

    return total[n-1] + recursiveTotal(total, n-1);
}

// Stores the bill records in txtfile
// n for size of an array, name for medicine name, qty for medicines qty, each medicines
// price and total for each medicine category and grand total sum of all medicines price
// it will store the records
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

// function to write to a json file 
// n for size of an array, name for medicine name, qty for medicines qty, each medicines
// price and total for each medicine category and grand total sum of all medicines price
// it will store the records
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
// function to view all sales
void viewSales(){
    ifstream file("bills.txt");
    if(!file){ // no file condition
        cout << "File not found!";
    }
    else{
    cout << "\n===== Sales Records =====\n";
    string line;
    while(getline(file,line)){ // runs until it found the end of file
    cout << line << endl;
    }
        }
    file.close(); //closing the file
}
// Discount function
float applydiscount(float total){
    if (total > 5000)
    total = total * 0.9;
    return total;
}
// Tax function
float addTax(float total){
    return total * 1.17;    
}
#include "DataManager.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>

// Private parsing helpers inside an anonymous namespace
namespace {
// Splits a string into delimiter-separated parts
std::vector<std::string> splitString(const std::string& str, char delim) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delim)) {
        result.push_back(token);
    }
    return result;
}

// Ensures directory path format ends with a slash
std::string sanitizePath(const std::string& path) {
    if (path.empty()) return "./";
    if (path.back() == '/' || path.back() == '\\') return path;
    return path + "/";
}
}

// Save all restaurant transactions to respective files
bool DataManager::saveData(const std::string& directoryPath,
                         const std::vector<DineInOrder>& orders,
                         const std::vector<Bill>& bills,
                         const std::vector<Payment>& payments,
                         std::string* errorMessage) {
    std::string base = sanitizePath(directoryPath);

    // 1. Save Orders
    std::string ordersFile = base + "orders.txt";
    std::ofstream oFile(ordersFile);
    if (!oFile) {
        if (errorMessage) *errorMessage = "Failed to open orders file: " + ordersFile;
        return false;
    }
    for (const auto& order : orders) {
        oFile << order.getOrderID() << "|"
              << order.getTableNumber() << "|"
              << order.getOrderDate() << "|"
              << order.getStatus() << "|";
        // Serialize map: itemID:qty,itemID:qty
        bool first = true;
        for (const auto& itemPair : order.getOrderedItems()) {
            if (!first) oFile << ",";
            oFile << itemPair.first << ":" << itemPair.second;
            first = false;
        }
        oFile << "\n";
    }
    oFile.close();

    // 2. Save Bills
    std::string billsFile = base + "bills.txt";
    std::ofstream bFile(billsFile);
    if (!bFile) {
        if (errorMessage) *errorMessage = "Failed to open bills file: " + billsFile;
        return false;
    }
    bFile << std::fixed << std::setprecision(2);
    for (const auto& bill : bills) {
        bFile << bill.getBillID() << "|"
              << bill.getOrderID() << "|"
              << bill.getSubtotal() << "|"
              << bill.getTax() << "|"
              << bill.getTotal() << "\n";
    }
    bFile.close();

    // 3. Save Payments
    std::string paymentsFile = base + "payments.txt";
    std::ofstream pFile(paymentsFile);
    if (!pFile) {
        if (errorMessage) *errorMessage = "Failed to open payments file: " + paymentsFile;
        return false;
    }
    pFile << std::fixed << std::setprecision(2);
    for (const auto& payment : payments) {
        pFile << payment.getPaymentID() << "|"
              << payment.getBillID() << "|"
              << payment.getPaymentMethod() << "|"
              << payment.getAmount() << "\n";
    }
    pFile.close();

    return true;
}

// Load all restaurant transactions from files
bool DataManager::loadData(const std::string& directoryPath,
                         std::vector<DineInOrder>& orders,
                         std::vector<Bill>& bills,
                         std::vector<Payment>& payments,
                         std::string* errorMessage) {
    std::string base = sanitizePath(directoryPath);
    std::string line;

    // Temporary storage to prevent partial state on load failure
    std::vector<DineInOrder> tempOrders;
    std::vector<Bill> tempBills;
    std::vector<Payment> tempPayments;

    // 1. Load Orders
    std::string ordersFile = base + "orders.txt";
    std::ifstream oFile(ordersFile);
    if (oFile) { // If file exists, load it
        while (std::getline(oFile, line)) {
            if (line.empty()) continue;
            auto parts = splitString(line, '|');
            if (parts.size() < 4) continue;

            std::string orderID = parts[0];
            int tableNumber = std::stoi(parts[1]);
            std::string date = parts[2];
            std::string status = parts[3];

            DineInOrder order(orderID, tableNumber, date, status);
            if (parts.size() >= 5 && !parts[4].empty()) {
                auto items = splitString(parts[4], ',');
                for (const auto& itemStr : items) {
                    auto itemParts = splitString(itemStr, ':');
                    if (itemParts.size() == 2) {
                        order.addOrUpdateItem(itemParts[0], std::stoi(itemParts[1]));
                    }
                }
            }
            tempOrders.push_back(order);
        }
        oFile.close();
    }

    // 2. Load Bills
    std::string billsFile = base + "bills.txt";
    std::ifstream bFile(billsFile);
    if (bFile) {
        while (std::getline(bFile, line)) {
            if (line.empty()) continue;
            auto parts = splitString(line, '|');
            if (parts.size() < 5) continue;

            std::string billID = parts[0];
            std::string orderID = parts[1];
            double subtotal = std::stod(parts[2]);
            double tax = std::stod(parts[3]);
            double total = std::stod(parts[4]);

            // Reconstruct Bill
            Bill bill(billID, orderID, subtotal);
            // Re-apply correct tax value from storage
            bill.calculateBill(subtotal > 0 ? (tax / subtotal) : 0.0);
            tempBills.push_back(bill);
        }
        bFile.close();
    }

    // 3. Load Payments
    std::string paymentsFile = base + "payments.txt";
    std::ifstream pFile(paymentsFile);
    if (pFile) {
        while (std::getline(pFile, line)) {
            if (line.empty()) continue;
            auto parts = splitString(line, '|');
            if (parts.size() < 4) continue;

            std::string paymentID = parts[0];
            std::string billID = parts[1];
            std::string method = parts[2];
            double amount = std::stod(parts[3]);

            tempPayments.push_back(Payment(paymentID, billID, method, amount));
        }
        pFile.close();
    }

    // Assign temporary collections to output arguments
    orders = std::move(tempOrders);
    bills = std::move(tempBills);
    payments = std::move(tempPayments);

    return true;
}

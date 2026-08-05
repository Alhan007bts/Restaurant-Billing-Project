#include "Bill.h"
#include <sstream>
#include <iomanip>

Bill::Bill()
    : billID(""), orderID(""), subtotal(0.0), tax(0.0), total(0.0) {}

Bill::Bill(std::string billID, std::string orderID, double subtotal, double taxRate)
    : billID(billID), orderID(orderID), subtotal(subtotal) {
    calculateBill(taxRate);
}

std::string Bill::getBillID() const { return billID; }
std::string Bill::getOrderID() const { return orderID; }
double Bill::getSubtotal() const { return subtotal; }
double Bill::getTax() const { return tax; }
double Bill::getTotal() const { return total; }

void Bill::calculateBill(double taxRate) {
    if (taxRate < 0.0) taxRate = 0.0;
    if (taxRate > 1.0) taxRate = 1.0;
    tax = subtotal * taxRate;
    total = subtotal + tax;
}

std::string Bill::displayBill() const {
    std::stringstream ss;
    ss << "========================================\n";
    ss << "             BILL INVOICE               \n";
    ss << "========================================\n";
    ss << "Bill ID  : " << billID << "\n";
    ss << "Order ID : " << orderID << "\n";
    ss << "----------------------------------------\n";
    ss << std::fixed << std::setprecision(2);
    ss << "Subtotal : $" << subtotal << "\n";
    ss << "Tax      : $" << tax << "\n";
    ss << "----------------------------------------\n";
    ss << "TOTAL    : $" << total << "\n";
    ss << "========================================\n";
    return ss.str();
}

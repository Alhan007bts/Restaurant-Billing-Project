#include "Payment.h"
#include <sstream>
#include <iomanip>

Payment::Payment()
    : paymentID(""), billID(""), paymentMethod(""), amount(0.0) {}

Payment::Payment(std::string paymentID, std::string billID, std::string paymentMethod, double amount)
    : paymentID(paymentID), billID(billID), paymentMethod(paymentMethod), amount(amount) {}

std::string Payment::getPaymentID() const { return paymentID; }
std::string Payment::getBillID() const { return billID; }
std::string Payment::getPaymentMethod() const { return paymentMethod; }
double Payment::getAmount() const { return amount; }

bool Payment::processPayment() {
    return (amount > 0.0 && !paymentID.empty() && !billID.empty());
}

std::string Payment::displayPayment() const {
    std::stringstream ss;
    ss << "========================================\n";
    ss << "            PAYMENT RECEIPT             \n";
    ss << "========================================\n";
    ss << "Payment ID : " << paymentID << "\n";
    ss << "Bill ID    : " << billID << "\n";
    ss << "Method     : " << paymentMethod << "\n";
    ss << std::fixed << std::setprecision(2);
    ss << "Amount     : $" << amount << "\n";
    ss << "Status     : Processed successfully\n";
    ss << "========================================\n";
    return ss.str();
}

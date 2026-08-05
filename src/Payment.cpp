#include "Payment.h"
#include <sstream>
#include <iomanip>

Payment::Payment()
    : paymentID(""), billID(""), paymentMethod(""), amount(0.0), cashGiven(0.0) {}

Payment::Payment(std::string paymentID, std::string billID, std::string paymentMethod, double amount, double cashGiven)
    : paymentID(paymentID), billID(billID), paymentMethod(paymentMethod), amount(amount) {
    if (cashGiven < 0.0) {
        this->cashGiven = amount;
    } else {
        this->cashGiven = cashGiven;
    }
}

std::string Payment::getPaymentID() const { return paymentID; }
std::string Payment::getBillID() const { return billID; }
std::string Payment::getPaymentMethod() const { return paymentMethod; }
double Payment::getAmount() const { return amount; }
double Payment::getCashGiven() const { return cashGiven; }
double Payment::getChange() const { return cashGiven - amount; }

bool Payment::processPayment() {
    return (amount > 0.0 && !paymentID.empty() && !billID.empty() && cashGiven >= amount);
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
    ss << "Bill Total : $" << amount << "\n";
    if (paymentMethod == "Cash") {
        ss << "Cash Given : $" << cashGiven << "\n";
        ss << "Change     : $" << getChange() << "\n";
    }
    ss << "Status     : Processed successfully\n";
    ss << "========================================\n";
    return ss.str();
}

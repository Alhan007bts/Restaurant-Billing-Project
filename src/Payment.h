#pragma once
#include <string>

class Payment {
private:
    std::string paymentID;
    std::string billID;
    std::string paymentMethod; // "Cash", "Card"
    double amount;

public:
    Payment();
    Payment(std::string paymentID, std::string billID, std::string paymentMethod, double amount);

    std::string getPaymentID() const;
    std::string getBillID() const;
    std::string getPaymentMethod() const;
    double getAmount() const;

    bool processPayment();
    std::string displayPayment() const;
};

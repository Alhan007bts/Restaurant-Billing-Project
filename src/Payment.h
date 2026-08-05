#pragma once
#include <string>

class Payment {
private:
    std::string paymentID;
    std::string billID;
    std::string paymentMethod; // "Cash", "Card"
    double amount;
    double cashGiven;

public:
    Payment();
    Payment(std::string paymentID, std::string billID, std::string paymentMethod, double amount, double cashGiven = -1.0);

    std::string getPaymentID() const;
    std::string getBillID() const;
    std::string getPaymentMethod() const;
    double getAmount() const;
    double getCashGiven() const;
    double getChange() const;

    bool processPayment();
    std::string displayPayment() const;
};

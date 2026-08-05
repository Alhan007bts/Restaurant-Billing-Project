#pragma once
#include <string>

class Bill {
private:
    std::string billID;
    std::string orderID;
    double subtotal;
    double tax;
    double total;

public:
    Bill();
    Bill(std::string billID, std::string orderID, double subtotal, double taxRate = 0.13);

    std::string getBillID() const;
    std::string getOrderID() const;
    double getSubtotal() const;
    double getTax() const;
    double getTotal() const;

    void calculateBill(double taxRate = 0.13);
    std::string displayBill() const;
};

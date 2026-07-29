#pragma once

#include <memory>
#include <string>
#include <vector>
#include "MenuItem.h"

struct OrderLine {
    std::shared_ptr<MenuItem> item;
    int quantity;

    OrderLine(std::shared_ptr<MenuItem> menuItem, int qty);
    double getSubtotal() const;
};

class CustomerOrder {
private:
    std::string orderID;
    int tableNumber;
    std::vector<OrderLine> items;

public:
    CustomerOrder(std::string orderID, int tableNumber);

    const std::string& getOrderID() const;
    int getTableNumber() const;
    const std::vector<OrderLine>& getItems() const;

    bool addItem(const std::shared_ptr<MenuItem>& item, int quantity = 1);
    bool removeItem(const std::string& itemID);
    bool updateQuantity(const std::string& itemID, int quantity);
    void clear();
    bool isEmpty() const;

    int getTotalQuantity() const;
    double calculateSubtotal() const;
    double calculateDiscount(double discountPercent) const;
    double calculateTax(double taxRate, double discountPercent = 0.0) const;
    double calculateGrandTotal(double taxRate = 0.13,
                               double discountPercent = 0.0) const;
};

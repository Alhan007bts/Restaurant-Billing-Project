#pragma once
#include <string>
#include <vector>
#include <memory>
#include "MenuItem.h"

class Order {
protected:
    std::string orderID;
    int tableNumber;
    std::string orderDate;
    std::string status; // "Open", "Paid", "Cancelled"

public:
    static std::vector<std::shared_ptr<MenuItem>> menuRegistry;

    Order(std::string orderID, int tableNumber, std::string orderDate, std::string status = "Open");
    virtual ~Order() = default;

    std::string getOrderID() const;
    int getTableNumber() const;
    std::string getOrderDate() const;
    std::string getStatus() const;
    void setStatus(const std::string& newStatus);

    virtual double computeTotal() = 0;
    virtual void displayOrder();
};

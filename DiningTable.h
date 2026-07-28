#pragma once

#include <memory>
#include "CustomerOrder.h"

class DiningTable {
private:
    int tableNumber;
    int capacity;
    bool occupied;
    std::unique_ptr<CustomerOrder> currentOrder;

public:
    DiningTable(int tableNumber, int capacity = 4);

    int getTableNumber() const;
    int getCapacity() const;
    bool isOccupied() const;
    bool hasActiveOrder() const;

    CustomerOrder* getCurrentOrder();
    const CustomerOrder* getCurrentOrder() const;

    bool openOrder(const std::string& orderID);
    std::unique_ptr<CustomerOrder> closeOrder();
    void setOccupied(bool value);
};

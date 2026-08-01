// Defines a dining table and its active customer order.
#pragma once

#include <memory>
#include "CustomerOrder.h"

// Tracks table details, occupancy, and order ownership.
class DiningTable {
private:
    int tableNumber;
    int capacity;
    bool occupied;
    std::unique_ptr<CustomerOrder> currentOrder;

public:
    // Create a table with a number and seating capacity.
    DiningTable(int tableNumber, int capacity = 4);

    // Return table information and current order state.
    int getTableNumber() const;
    int getCapacity() const;
    bool isOccupied() const;
    bool hasActiveOrder() const;

    CustomerOrder* getCurrentOrder();
    const CustomerOrder* getCurrentOrder() const;

    // Open, close, or reset the table order.
    bool openOrder(const std::string& orderID);
    std::unique_ptr<CustomerOrder> closeOrder();
    void setOccupied(bool value);
};

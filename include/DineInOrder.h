#pragma once
#include "Order.h"
#include <map>

class DineInOrder : public Order {
private:
    std::map<std::string, int> orderedItems;

public:
    DineInOrder(std::string orderID, int tableNumber, std::string orderDate, std::string status = "Open");
    virtual ~DineInOrder() = default;

    const std::map<std::string, int>& getOrderedItems() const;
    void addOrUpdateItem(const std::string& itemID, int qty);
    void removeItem(const std::string& itemID);
    void clearItems();

    double computeTotal() override;
    void displayOrder() override;
};

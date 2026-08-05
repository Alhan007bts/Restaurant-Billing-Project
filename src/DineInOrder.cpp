#include "DineInOrder.h"
#include "FoodItem.h"
#include <iostream>

DineInOrder::DineInOrder(std::string orderID, int tableNumber, std::string orderDate, std::string status)
    : Order(orderID, tableNumber, orderDate, status) {}

const std::map<std::string, int>& DineInOrder::getOrderedItems() const { return orderedItems; }

void DineInOrder::addOrUpdateItem(const std::string& itemID, int qty) {
    if (qty <= 0) {
        removeItem(itemID);
    } else {
        orderedItems[itemID] = qty;
    }
}

void DineInOrder::removeItem(const std::string& itemID) {
    orderedItems.erase(itemID);
}

void DineInOrder::clearItems() {
    orderedItems.clear();
}

double DineInOrder::computeTotal() {
    double subtotal = 0.0;
    for (const auto& pair : orderedItems) {
        const std::string& itemID = pair.first;
        int qty = pair.second;
        std::shared_ptr<MenuItem> foundItem = nullptr;
        for (const auto& menuInst : menuRegistry) {
            if (menuInst && menuInst->getItemID() == itemID) {
                foundItem = menuInst;
                break;
            }
        }
        if (foundItem) {
            double price = foundItem->getPrice();
            double prepFee = 0.0;
            if (auto food = std::dynamic_pointer_cast<FoodItem>(foundItem)) {
                prepFee = food->calculatePreparationFee();
            }
            subtotal += (price + prepFee) * qty;
        }
    }
    return subtotal;
}

void DineInOrder::displayOrder() {
    Order::displayOrder();
    std::cout << "Ordered Items:" << std::endl;
    for (const auto& pair : orderedItems) {
        std::cout << "  - Item ID: " << pair.first << " | Qty: " << pair.second << std::endl;
    }
}

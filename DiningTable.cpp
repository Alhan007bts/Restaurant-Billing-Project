#include "DiningTable.h"

DiningTable::DiningTable(int tableNumber, int capacity)
    : tableNumber(tableNumber > 0 ? tableNumber : 1),
      capacity(capacity > 0 ? capacity : 1),
      occupied(false),
      currentOrder(nullptr) {}

int DiningTable::getTableNumber() const { return tableNumber; }
int DiningTable::getCapacity() const { return capacity; }
bool DiningTable::isOccupied() const { return occupied; }
bool DiningTable::hasActiveOrder() const { return currentOrder != nullptr; }

CustomerOrder* DiningTable::getCurrentOrder() { return currentOrder.get(); }
const CustomerOrder* DiningTable::getCurrentOrder() const { return currentOrder.get(); }

bool DiningTable::openOrder(const std::string& orderID) {
    if (currentOrder || orderID.empty()) return false;
    currentOrder = std::make_unique<CustomerOrder>(orderID, tableNumber);
    occupied = true;
    return true;
}

std::unique_ptr<CustomerOrder> DiningTable::closeOrder() {
    occupied = false;
    return std::move(currentOrder);
}

void DiningTable::setOccupied(bool value) {
    occupied = value;
    if (!occupied) currentOrder.reset();
}

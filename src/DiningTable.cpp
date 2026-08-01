// Implements dining-table state and order ownership.
#include "DiningTable.h"

// Create a table with validated number and capacity values.
DiningTable::DiningTable(int tableNumber, int capacity)
    : tableNumber(tableNumber > 0 ? tableNumber : 1),
      capacity(capacity > 0 ? capacity : 1),
      occupied(false),
      currentOrder(nullptr) {}

// Return table details and occupancy state.
int DiningTable::getTableNumber() const { return tableNumber; }
int DiningTable::getCapacity() const { return capacity; }
bool DiningTable::isOccupied() const { return occupied; }
bool DiningTable::hasActiveOrder() const { return currentOrder != nullptr; }

// Provide access to the active order without transferring ownership.
CustomerOrder* DiningTable::getCurrentOrder() { return currentOrder.get(); }
const CustomerOrder* DiningTable::getCurrentOrder() const { return currentOrder.get(); }

// Open a new order only when the table has none.
bool DiningTable::openOrder(const std::string& orderID) {
    if (currentOrder || orderID.empty()) return false;
    currentOrder = std::make_unique<CustomerOrder>(orderID, tableNumber);
    occupied = true;
    return true;
}

// Close the order and transfer ownership to the caller.
std::unique_ptr<CustomerOrder> DiningTable::closeOrder() {
    occupied = false;
    return std::move(currentOrder);
}

// Update occupancy and clear the order when unoccupied.
void DiningTable::setOccupied(bool value) {
    occupied = value;
    if (!occupied) currentOrder.reset();
}

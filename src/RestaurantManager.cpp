// Implements restaurant data management and sales analytics.
#include "RestaurantManager.h"
#include <algorithm>
#include <utility>

// Add a valid menu item when its ID is not already stored.
bool RestaurantManager::addMenuItem(const std::shared_ptr<MenuItem>& item) {
    if (!item || findMenuItem(item->getItemID())) return false;
    menuItems.push_back(item);
    return true;
}

// Remove a menu item by ID.
bool RestaurantManager::removeMenuItem(const std::string& itemID) {
    const auto oldSize = menuItems.size();
    menuItems.erase(std::remove_if(menuItems.begin(), menuItems.end(),
        [&itemID](const std::shared_ptr<MenuItem>& item) {
            return item && item->getItemID() == itemID;
        }), menuItems.end());
    return menuItems.size() != oldSize;
}

// Find and return a menu item by ID.
std::shared_ptr<MenuItem> RestaurantManager::findMenuItem(const std::string& itemID) const {
    for (const auto& item : menuItems) {
        if (item && item->getItemID() == itemID) return item;
    }
    return nullptr;
}

// Return the complete menu collection.
const std::vector<std::shared_ptr<MenuItem>>& RestaurantManager::getMenuItems() const {
    return menuItems;
}

// Replace the current menu with another collection.
void RestaurantManager::replaceMenuItems(std::vector<std::shared_ptr<MenuItem>> items) {
    menuItems = std::move(items);
}

// Add a uniquely numbered dining table.
bool RestaurantManager::addTable(int tableNumber, int capacity) {
    if (tableNumber <= 0 || tables.count(tableNumber) != 0) return false;
    tables[tableNumber] = std::make_unique<DiningTable>(tableNumber, capacity);
    return true;
}

// Find a modifiable dining table by number.
DiningTable* RestaurantManager::getTable(int tableNumber) {
    const auto it = tables.find(tableNumber);
    return it == tables.end() ? nullptr : it->second.get();
}

// Find a read-only dining table by number.
const DiningTable* RestaurantManager::getTable(int tableNumber) const {
    const auto it = tables.find(tableNumber);
    return it == tables.end() ? nullptr : it->second.get();
}

// Open a customer order for the selected table.
bool RestaurantManager::openTableOrder(int tableNumber, const std::string& orderID) {
    DiningTable* table = getTable(tableNumber);
    return table && table->openOrder(orderID);
}

// Add a menu item to an active table order.
bool RestaurantManager::addItemToTable(int tableNumber, const std::string& itemID, int quantity) {
    DiningTable* table = getTable(tableNumber);
    auto item = findMenuItem(itemID);
    if (!table || !table->getCurrentOrder() || !item) return false;
    return table->getCurrentOrder()->addItem(item, quantity);
}

// Remove a menu item from an active table order.
bool RestaurantManager::removeItemFromTable(int tableNumber, const std::string& itemID) {
    DiningTable* table = getTable(tableNumber);
    if (!table || !table->getCurrentOrder()) return false;
    return table->getCurrentOrder()->removeItem(itemID);
}

// Close a non-empty order and store it for analytics.
bool RestaurantManager::completeTableOrder(int tableNumber) {
    DiningTable* table = getTable(tableNumber);
    if (!table || !table->hasActiveOrder()) return false;
    auto order = table->closeOrder();
    if (!order || order->isEmpty()) return false;
    completedOrders.push_back(std::move(order));
    return true;
}

// Count sold quantities for every menu item.
std::map<std::string, int> RestaurantManager::calculateItemPopularity() const {
    std::map<std::string, int> counts;
    for (const auto& order : completedOrders) {
        if (!order) continue;
        for (const auto& line : order->getItems()) {
            if (line.item) counts[line.item->getItemID()] += line.quantity;
        }
    }
    return counts;
}

// Calculate completed orders, sales, averages, and popularity.
SalesAnalytics RestaurantManager::calculateAnalytics(double taxRate,
                                                      double discountPercent) const {
    SalesAnalytics result;
    result.completedOrders = static_cast<int>(completedOrders.size());

    for (const auto& order : completedOrders) {
        if (!order) continue;
        result.itemsSold += order->getTotalQuantity();
        result.grossSales += order->calculateGrandTotal(taxRate, discountPercent);
    }

    if (result.completedOrders > 0) {
        result.averageOrderValue = result.grossSales / result.completedOrders;
    }

    const auto popularity = calculateItemPopularity();
    for (const auto& entry : popularity) {
        if (entry.second > result.mostPopularItemQuantity) {
            result.mostPopularItemID = entry.first;
            result.mostPopularItemQuantity = entry.second;
        }
    }

    return result;
}

// Return all completed orders without transferring ownership.
const std::vector<std::unique_ptr<CustomerOrder>>& RestaurantManager::getCompletedOrders() const {
    return completedOrders;
}

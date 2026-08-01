// Defines the central restaurant data and analytics manager.
#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>
#include "DiningTable.h"
#include "MenuItem.h"

// Stores calculated sales-performance results.
struct SalesAnalytics {
    int completedOrders = 0;
    int itemsSold = 0;
    double grossSales = 0.0;
    double averageOrderValue = 0.0;
    std::string mostPopularItemID;
    int mostPopularItemQuantity = 0;
};

// Organizes menu items, tables, completed orders, and analytics.
class RestaurantManager {
private:
    std::vector<std::shared_ptr<MenuItem>> menuItems;
    std::map<int, std::unique_ptr<DiningTable>> tables;
    std::vector<std::unique_ptr<CustomerOrder>> completedOrders;

public:
    // Manage the restaurant menu collection.
    bool addMenuItem(const std::shared_ptr<MenuItem>& item);
    bool removeMenuItem(const std::string& itemID);
    std::shared_ptr<MenuItem> findMenuItem(const std::string& itemID) const;
    const std::vector<std::shared_ptr<MenuItem>>& getMenuItems() const;
    void replaceMenuItems(std::vector<std::shared_ptr<MenuItem>> items);

    // Create and retrieve dining tables.
    bool addTable(int tableNumber, int capacity = 4);
    DiningTable* getTable(int tableNumber);
    const DiningTable* getTable(int tableNumber) const;

    // Manage customer orders through their assigned tables.
    bool openTableOrder(int tableNumber, const std::string& orderID);
    bool addItemToTable(int tableNumber, const std::string& itemID, int quantity = 1);
    bool removeItemFromTable(int tableNumber, const std::string& itemID);
    bool completeTableOrder(int tableNumber);

    // Calculate sales totals and item popularity.
    SalesAnalytics calculateAnalytics(double taxRate = 0.13,
                                      double discountPercent = 0.0) const;
    std::map<std::string, int> calculateItemPopularity() const;
    const std::vector<std::unique_ptr<CustomerOrder>>& getCompletedOrders() const;
};

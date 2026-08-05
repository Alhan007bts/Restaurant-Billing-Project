#include "Order.h"
#include <iostream>

std::vector<std::shared_ptr<MenuItem>> Order::menuRegistry;

Order::Order(std::string orderID, int tableNumber, std::string orderDate, std::string status)
    : orderID(orderID), tableNumber(tableNumber), orderDate(orderDate), status(status) {}

std::string Order::getOrderID() const { return orderID; }
int Order::getTableNumber() const { return tableNumber; }
std::string Order::getOrderDate() const { return orderDate; }
std::string Order::getStatus() const { return status; }
void Order::setStatus(const std::string& newStatus) { status = newStatus; }

void Order::displayOrder() {
    std::cout << "Order ID: " << orderID << " | Table: " << tableNumber
              << " | Date: " << orderDate << " | Status: " << status << std::endl;
}

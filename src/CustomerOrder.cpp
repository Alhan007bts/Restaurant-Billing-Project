#include "CustomerOrder.h"
#include <algorithm>
#include <utility>

OrderLine::OrderLine(std::shared_ptr<MenuItem> menuItem, int qty)
    : item(std::move(menuItem)), quantity(qty > 0 ? qty : 1) {}

double OrderLine::getSubtotal() const {
    return item ? item->getPrice() * quantity : 0.0;
}

CustomerOrder::CustomerOrder(std::string orderID, int tableNumber)
    : orderID(std::move(orderID)), tableNumber(tableNumber > 0 ? tableNumber : 1) {}

const std::string& CustomerOrder::getOrderID() const { return orderID; }
int CustomerOrder::getTableNumber() const { return tableNumber; }
const std::vector<OrderLine>& CustomerOrder::getItems() const { return items; }

bool CustomerOrder::addItem(const std::shared_ptr<MenuItem>& item, int quantity) {
    if (!item || quantity <= 0) return false;

    for (auto& line : items) {
        if (line.item && line.item->getItemID() == item->getItemID()) {
            line.quantity += quantity;
            return true;
        }
    }

    items.emplace_back(item, quantity);
    return true;
}

bool CustomerOrder::removeItem(const std::string& itemID) {
    const auto oldSize = items.size();
    items.erase(std::remove_if(items.begin(), items.end(),
        [&itemID](const OrderLine& line) {
            return line.item && line.item->getItemID() == itemID;
        }), items.end());
    return items.size() != oldSize;
}

bool CustomerOrder::updateQuantity(const std::string& itemID, int quantity) {
    if (quantity <= 0) return removeItem(itemID);

    for (auto& line : items) {
        if (line.item && line.item->getItemID() == itemID) {
            line.quantity = quantity;
            return true;
        }
    }
    return false;
}

void CustomerOrder::clear() { items.clear(); }
bool CustomerOrder::isEmpty() const { return items.empty(); }

int CustomerOrder::getTotalQuantity() const {
    int total = 0;
    for (const auto& line : items) total += line.quantity;
    return total;
}

double CustomerOrder::calculateSubtotal() const {
    double subtotal = 0.0;
    for (const auto& line : items) subtotal += line.getSubtotal();
    return subtotal;
}

double CustomerOrder::calculateDiscount(double discountPercent) const {
    if (discountPercent < 0.0) discountPercent = 0.0;
    if (discountPercent > 100.0) discountPercent = 100.0;
    return calculateSubtotal() * (discountPercent / 100.0);
}

double CustomerOrder::calculateTax(double taxRate, double discountPercent) const {
    if (taxRate < 0.0) taxRate = 0.0;
    if (taxRate > 1.0) taxRate = 1.0;
    const double taxable = calculateSubtotal() - calculateDiscount(discountPercent);
    return taxable * taxRate;
}

double CustomerOrder::calculateGrandTotal(double taxRate, double discountPercent) const {
    const double subtotal = calculateSubtotal();
    const double discount = calculateDiscount(discountPercent);
    return subtotal - discount + calculateTax(taxRate, discountPercent);
}

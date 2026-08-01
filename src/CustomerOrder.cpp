// Implements customer-order storage and calculations.
#include "CustomerOrder.h"
#include <algorithm>
#include <utility>

// Store an item and ensure its quantity is positive.
OrderLine::OrderLine(std::shared_ptr<MenuItem> menuItem, int qty)
    : item(std::move(menuItem)), quantity(qty > 0 ? qty : 1) {}

// Calculate the subtotal for one order line.
double OrderLine::getSubtotal() const {
    return item ? item->getPrice() * quantity : 0.0;
}

// Create an order and validate its table number.
CustomerOrder::CustomerOrder(std::string orderID, int tableNumber)
    : orderID(std::move(orderID)), tableNumber(tableNumber > 0 ? tableNumber : 1) {}

// Return basic order information.
const std::string& CustomerOrder::getOrderID() const { return orderID; }
int CustomerOrder::getTableNumber() const { return tableNumber; }
const std::vector<OrderLine>& CustomerOrder::getItems() const { return items; }

// Add a new item or increase its existing quantity.
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

// Remove an item by its unique menu ID.
bool CustomerOrder::removeItem(const std::string& itemID) {
    const auto oldSize = items.size();
    items.erase(std::remove_if(items.begin(), items.end(),
        [&itemID](const OrderLine& line) {
            return line.item && line.item->getItemID() == itemID;
        }), items.end());
    return items.size() != oldSize;
}

// Update quantity or remove the item when quantity is non-positive.
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

// Clear the order and check whether it is empty.
void CustomerOrder::clear() { items.clear(); }
bool CustomerOrder::isEmpty() const { return items.empty(); }

// Count all individual items in the order.
int CustomerOrder::getTotalQuantity() const {
    int total = 0;
    for (const auto& line : items) total += line.quantity;
    return total;
}

// Add all order-line subtotals.
double CustomerOrder::calculateSubtotal() const {
    double subtotal = 0.0;
    for (const auto& line : items) subtotal += line.getSubtotal();
    return subtotal;
}

// Calculate a validated percentage discount.
double CustomerOrder::calculateDiscount(double discountPercent) const {
    if (discountPercent < 0.0) discountPercent = 0.0;
    if (discountPercent > 100.0) discountPercent = 100.0;
    return calculateSubtotal() * (discountPercent / 100.0);
}

// Calculate tax after the discount is applied.
double CustomerOrder::calculateTax(double taxRate, double discountPercent) const {
    if (taxRate < 0.0) taxRate = 0.0;
    if (taxRate > 1.0) taxRate = 1.0;
    const double taxable = calculateSubtotal() - calculateDiscount(discountPercent);
    return taxable * taxRate;
}

// Calculate the final amount including discount and tax.
double CustomerOrder::calculateGrandTotal(double taxRate, double discountPercent) const {
    const double subtotal = calculateSubtotal();
    const double discount = calculateDiscount(discountPercent);
    return subtotal - discount + calculateTax(taxRate, discountPercent);
}

#pragma once
#include <map>
#include <string>
#include <vector>

template <typename T> class AnalyticsEngine {
public:
  // Computes daily sales: sum of total amounts in the collection.
  // Overloaded internally using helper functions.
  double computeDailySales(const std::vector<T> &collection) {
    double sum = 0.0;
    for (const auto &item : collection) {
      sum += getItemValue(item);
    }
    return sum;
  }

  // Computes average bill amount.
  double computeAverageBillAmount(const std::vector<T> &collection) {
    if (collection.empty())
      return 0.0;
    return computeDailySales(collection) / collection.size();
  }

  // Count orders/items by status.
  int computeOrdersByStatus(const std::vector<T> &collection,
                            const std::string &status) {
    int count = 0;
    for (const auto &item : collection) {
      if (getItemStatus(item) == status) {
        count++;
      }
    }
    return count;
  }

  // Calculates item popularity count.
  std::map<std::string, int>
  computePopularItems(const std::vector<T> &collection) {
    std::map<std::string, int> counts;
    for (const auto &order : collection) {
      addItemCounts(order, counts);
    }
    return counts;
  }

private:
  // Overloads using SFINAE/decltype to extract values based on type T
  template <typename U>
  auto getItemValue(const U &item) -> decltype(item.getTotal()) {
    return item.getTotal();
  }

  // Fallback if no getTotal(), e.g. Payment
  template <typename U>
  auto getItemValue(const U &item) -> decltype(item.getAmount()) {
    return item.getAmount();
  }

  double getItemValue(...) { return 0.0; }

  template <typename U>
  auto getItemStatus(const U &item) -> decltype(item.getStatus()) {
    return item.getStatus();
  }

  std::string getItemStatus(...) { return ""; }

  template <typename U>
  auto addItemCounts(const U &order, std::map<std::string, int> &counts)
      -> decltype(order.getOrderedItems(), void()) {
    for (const auto &pair : order.getOrderedItems()) {
      counts[pair.first] += pair.second;
    }
  }

  void addItemCounts(...) {}
};

#pragma once
#include "Bill.h"
#include "DineInOrder.h"
#include "Payment.h"
#include <string>
#include <vector>

class DataManager {
public:
  static bool saveData(const std::string &directoryPath,
                       const std::vector<DineInOrder> &orders,
                       const std::vector<Bill> &bills,
                       const std::vector<Payment> &payments,
                       std::string *errorMessage = nullptr);

  static bool loadData(const std::string &directoryPath,
                       std::vector<DineInOrder> &orders,
                       std::vector<Bill> &bills, std::vector<Payment> &payments,
                       std::string *errorMessage = nullptr);
};

#include "MenuFileIO.h"
#include "FoodItem.h"
#include "BeverageItem.h"
#include <fstream>
#include <iomanip>
#include <sstream>

namespace {
std::vector<std::string> split(const std::string& line, char delimiter) {
    std::vector<std::string> parts;
    std::stringstream stream(line);
    std::string part;
    while (std::getline(stream, part, delimiter)) parts.push_back(part);
    return parts;
}

bool parseBool(const std::string& value, bool& result) {
    if (value == "1" || value == "true" || value == "TRUE") {
        result = true;
        return true;
    }
    if (value == "0" || value == "false" || value == "FALSE") {
        result = false;
        return true;
    }
    return false;
}
}

bool MenuFileIO::saveMenu(const std::string& fileName,
                          const std::vector<std::shared_ptr<MenuItem>>& menuItems,
                          std::string* errorMessage) {
    std::ofstream file(fileName);
    if (!file) {
        if (errorMessage) *errorMessage = "Could not open file for writing: " + fileName;
        return false;
    }

    file << "TYPE|ID|NAME|CATEGORY|PRICE|EXTRA1|EXTRA2|EXTRA3\n";
    file << std::fixed << std::setprecision(2);

    for (const auto& item : menuItems) {
        if (!item) continue;

        if (const auto food = std::dynamic_pointer_cast<FoodItem>(item)) {
            file << "FOOD|" << food->getItemID() << '|'
                 << food->getName() << '|' << food->getCategory() << '|'
                 << food->getPrice() << '|' << food->getCuisineType() << '|'
                 << food->getPreparationTime() << '|'
                 << (food->getIsSpicy() ? 1 : 0) << '\n';
        } else if (const auto beverage = std::dynamic_pointer_cast<BeverageItem>(item)) {
            file << "BEVERAGE|" << beverage->getItemID() << '|'
                 << beverage->getName() << '|' << beverage->getCategory() << '|'
                 << beverage->getPrice() << '|' << beverage->getVolume() << '|'
                 << beverage->getBeverageType() << '|'
                 << (beverage->getIsCarbonated() ? 1 : 0) << '\n';
        }
    }

    if (!file.good()) {
        if (errorMessage) *errorMessage = "An error occurred while writing: " + fileName;
        return false;
    }
    return true;
}

bool MenuFileIO::loadMenu(const std::string& fileName,
                          std::vector<std::shared_ptr<MenuItem>>& menuItems,
                          std::string* errorMessage) {
    std::ifstream file(fileName);
    if (!file) {
        if (errorMessage) *errorMessage = "Could not open file for reading: " + fileName;
        return false;
    }

    std::vector<std::shared_ptr<MenuItem>> loaded;
    std::string line;
    int lineNumber = 0;

    while (std::getline(file, line)) {
        ++lineNumber;
        if (line.empty() || line.rfind("TYPE|", 0) == 0) continue;

        const auto fields = split(line, '|');
        if (fields.size() != 8) {
            if (errorMessage) *errorMessage = "Invalid field count on line " + std::to_string(lineNumber);
            return false;
        }

        try {
            const double price = std::stod(fields[4]);
            bool flag = false;
            if (!parseBool(fields[7], flag)) throw std::invalid_argument("invalid boolean");

            if (fields[0] == "FOOD") {
                const int prepTime = std::stoi(fields[6]);
                loaded.push_back(std::make_shared<FoodItem>(
                    fields[1], fields[2], fields[3], price,
                    fields[5], prepTime, flag));
            } else if (fields[0] == "BEVERAGE") {
                const double volume = std::stod(fields[5]);
                loaded.push_back(std::make_shared<BeverageItem>(
                    fields[1], fields[2], fields[3], price,
                    volume, fields[6], flag));
            } else {
                if (errorMessage) *errorMessage = "Unknown item type on line " + std::to_string(lineNumber);
                return false;
            }
        } catch (const std::exception&) {
            if (errorMessage) *errorMessage = "Invalid data on line " + std::to_string(lineNumber);
            return false;
        }
    }

    menuItems = std::move(loaded);
    return true;
}

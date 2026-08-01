// Defines menu loading and saving operations.
#pragma once

#include <memory>
#include <string>
#include <vector>
#include "MenuItem.h"

// Provides static text-file operations for menu items.
class MenuFileIO {
public:
    // Save all supported menu items to a text file.
    static bool saveMenu(const std::string& fileName,
                         const std::vector<std::shared_ptr<MenuItem>>& menuItems,
                         std::string* errorMessage = nullptr);

    // Load menu items safely from a text file.
    static bool loadMenu(const std::string& fileName,
                         std::vector<std::shared_ptr<MenuItem>>& menuItems,
                         std::string* errorMessage = nullptr);
};

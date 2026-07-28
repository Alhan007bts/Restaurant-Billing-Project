#pragma once

#include <memory>
#include <string>
#include <vector>
#include "MenuItem.h"

class MenuFileIO {
public:
    static bool saveMenu(const std::string& fileName,
                         const std::vector<std::shared_ptr<MenuItem>>& menuItems,
                         std::string* errorMessage = nullptr);

    static bool loadMenu(const std::string& fileName,
                         std::vector<std::shared_ptr<MenuItem>>& menuItems,
                         std::string* errorMessage = nullptr);
};

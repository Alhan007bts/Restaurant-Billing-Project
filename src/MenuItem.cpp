#include "MenuItem.h"
#include <iomanip>
#include <cctype>

using namespace std;

// Helper function to trim whitespace
string trim(const string& str) {
	// Find first non-whitespace character from the start
	size_t first = str.find_first_not_of(" \t\n\r");
	if (first == string::npos) return "";

	// Find last non-whitespace character from the end
	size_t last = str.find_last_not_of(" \t\n\r");

	// Return substring from first to last (inclusive)
	return str.substr(first, last - first + 1);
}

// ===========================
// CONSTRUCTORS AND DESTRUCTOR
// ===========================

// Default Constructor
MenuItem::MenuItem() : itemID(""), name(""), category(""), price(0.0) {}

// Parameterized Constructor
MenuItem::MenuItem(string id, string name, string category, double price) {
	setItemID(id);				// Validate and set ID
	setName(name);				// Validate and set name
	setCategory(category);		// Validate and set category
	setPrice(price);			// Validate and set price
}

// Virtual Destructor
MenuItem::~MenuItem() {}

// =========================
//          GETTERS
// =========================
string MenuItem::getItemID() const { return itemID; }
string MenuItem::getName() const { return name; }
string MenuItem::getCategory() const { return category; }
double MenuItem::getPrice() const { return price; }

// =======================
// SETTERS WITH VALIDATION
// =======================

/*
 * Set item ID with validation
 * Valid format: ITEM-XXXX where X are digits (0-9)
 * Example: ITEM-0001, ITEM-0123, ITEM-9999
 */
void MenuItem::setItemID(string id) {
	string trimmed = trim(id);
	// Validated format: ITEM-XXXX (where XXXX are digits)
	if (trimmed.length() == 9 &&
		trimmed.substr(0, 5) == "ITEM-" &&
		isdigit(trimmed[5]) && isdigit(trimmed[6]) &&
		isdigit(trimmed[7]) && isdigit(trimmed[8])) {
		itemID = trimmed;
	}
	else {
		itemID = "ITEM-0000"; // Default ID if validation fails
	}
}

/*
 * Set item name with validation
 * Rules: 2-100 characters, at least one alphabetic character
 */
void MenuItem::setName(string name) {
	// Basic validation: Name should not be empty
	string trimmed = trim(name);

	// Check length requirement (2-100 characters)
	if (trimmed.length() >= 2 && trimmed.length() <= 100) {
		// Check for at least one  character
		bool hasAlpha = false;
		for (char c : trimmed) {
			if (isalpha(c)) {
				hasAlpha = true;
				break;
			}
		}
		if (hasAlpha) {
			this->name = trimmed;
			return;
		}
	}
	this->name = "Unknown Item"; // Default name if validation fails
}

/*
 * Set category with validation
 * Allowed: Appetizer, Main Course, Dessert, Beverage
 */
void MenuItem::setCategory(string category) {
	string trimmed = trim(category);
	// Categories for allowed food items
	string allowedFoodCategories[] = {"Appetizer", "Main Course", "Dessert", "Beverage"};
	bool valid = false;
	
	// check if category is in the allowed list
	for (const string& cat : allowedFoodCategories) {
		if (trimmed == cat) {
			valid = true;
			break;
		}
	}
	// Set category or default to "Other"
	this->category = valid ? trimmed : "Other"; 
}

/*
 * Set price with validation
 * Rules: Must be between $0.01 and $10,000 (restaurant realistic range)
 */
void MenuItem::setPrice(double price) {
	if (price > 0.0 && price <= 10000.0) { // Assuming a reasonable price range
		this->price = price;
	}
	else {
		this->price = 10.0; // Default price if validation fails
	}
}

// ==========================
// DYNAMIC PRICE CALCULATIONS
// ==========================

/* 
 * Calculate discounted price based on a percentage
 * Valid discounted range: 0% to 100%
 */
double MenuItem::calculateDiscountedPrice(double discountPercentage) const {
	// Clamp discount to valid range
	if (discountPercentage < 0.0) discountPercentage = 0.0;
	if (discountPercentage > 100.0) discountPercentage = 100.0;

	// apply discount: price * (1 - discount/100)
	return price * (1.0 - discountPercentage / 100.0);
}

/*
 * Calculate final price including tax
 * Tax rate should be between 0 and 1 (e.g., 0.13 for 13%)
 */
double MenuItem::calculateFinalPrice(double taxRate) const {
	// clamp tax rate to valid range
	if (taxRate < 0.0) taxRate = 0.0;
	if (taxRate > 1.0) taxRate = 1.0; 

	// apply tax: price * (1 + taxRate)
	return price * (1.0 + taxRate);
}

/*
 * Calculate special offer price
 * Default: 10% off for special promotions
 */
double MenuItem::calculateSpecialOfferPrice() const {
	// Default implementation: 10% off for special offers
	return calculateDiscountedPrice(10.0);
}

/*
 * Update price with validation
 * Delegates to setPrice() for validation
 */
void MenuItem::updatePrice(double newPrice) {
	setPrice(newPrice); // Reuse the setter for validation
}

// ====================
// OPERATOR OVERLOADING
// ====================

/*
 * Equality operator - compares items by ID
 * Used for finding and comparing menu items
 */
bool MenuItem::operator==(const MenuItem& other) const {
	return this->itemID == other.itemID;
}

/*
 * Inequality operator - opposite of ==
 */
bool MenuItem::operator!=(const MenuItem& other) const {
	return !(*this == other);
}

// ====================
//   FRIEND FUNCTIONS
// ====================

/*
 * Friend function: Stream insertion operator for printing
 * Enables: cout << menuItem;
 * Format: Item ID: ITEM-0001 | Name: Chicken Burger | Category: Main Course | Price: $15.99
 */
ostream& operator<<(ostream& os, const MenuItem& item) {
	os << "Item ID: " << item.itemID 
	   << "| Name: " << item.name
	   << "| Category: " << item.category 
	   << "| Price: $" << fixed << setprecision(2) << item.price;
	return os;
}

/*
 * Friend function: Custom print details with decorative borders
 * Provides a more visually appealing output for receipts or menus
 */
void printItemDetails(const MenuItem& item) {
	cout << "===========================" << endl;
	cout << "       ITEM DETAILS" << endl;
	cout << "===========================" << endl;
	cout << item << endl;
	cout << "===========================" << endl;
}
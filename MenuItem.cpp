#include "MenuItem.h"
#include <iomanip>
#include <cctype>

using namespace std;

// Helper function to trim whitespace
string trim(const string& str) {
	size_t first = str.find_first_not_of(" \t\n\r");
	if (first == string::npos) return "";
	size_t last = str.find_last_not_of(" \t\n\r");
	return str.substr(first, last - first + 1);
}

// ===========================
// CONSTRUCTORS AND DESTRUCTOR
// ===========================

// Default Constructor
MenuItem::MenuItem() : itemID(""), name(""), category(""), price(0.0) {}

// Parameterized Constructor
MenuItem::MenuItem(string id, string name, string category, double price) {
	setItemID(id);
	setName(name);
	setCategory(category);
	setPrice(price);
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
void MenuItem::setItemID(string id) {
	string trimmed = trim(id);
	// Basic validation: ID should not be empty and should be alphanumeric
	if (!trimmed.empty()) {
		itemID = trimmed;
	}
	else {
		itemID = "ITEM-0000"; // Default ID if validation fails
	}
}

void MenuItem::setName(string name) {
	// Basic validation: Name should not be empty
	string trimmed = trim(name);
	if (trimmed.length() >= 2 && trimmed.length() <= 100) {
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

void MenuItem::setCategory(string category) {
	string trimmed = trim(category);
	// Categories for food items
	string allowedFoodCategories[] = {"Appetizer", "Main Course", "Dessert", "Beverage"};
	bool valid = false;
	for (const string& cat : allowedFoodCategories) {
		if (trimmed == cat) {
			valid = true;
			break;
		}
	}
	this->category = valid ? trimmed : "Other"; // Default category if validation fails
}

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

// Calculate discounted price based on a percentage
double MenuItem::calculateDiscountedPrice(double discountPercentage) const {
	if (discountPercentage < 0.0) discountPercentage = 0.0;
	if (discountPercentage > 100.0) discountPercentage = 100.0;
	return price * (1.0 - discountPercentage / 100.0);
}

// Calculate final price including tax
double MenuItem::calculateFinalPrice(double taxRate) const {
	if (taxRate < 0.0) taxRate = 0.0;
	if (taxRate > 1.0) taxRate = 1.0; // Tax rate should be between 0 and 1
	return price * (1.0 + taxRate);
}

// Calculate special offer price 
double MenuItem::calculateSpecialOfferPrice() const {
	// Default implementation: 10% off for special offers
	return calculateDiscountedPrice(10.0);
}

// Update price with validation
void MenuItem::updatePrice(double newPrice) {
	setPrice(newPrice); // Reuse the setter for validation
}

// ====================
// OPERATOR OVERLOADING
// ====================

// Opertaor == (compare by ID)
bool MenuItem::operator==(const MenuItem& other) const {
	return this->itemID == other.itemID;
}

// Operator != (compare by ID)
bool MenuItem::operator!=(const MenuItem& other) const {
	return !(*this == other);
}

// ====================
//   FRIEND FUNCTIONS
// ====================

// Friend function: operator<< for printing
ostream& operator<<(ostream& os, const MenuItem& item) {
	os << "Item ID: " << item.itemID 
	   << "| Name: " << item.name
	   << "| Category: " << item.category 
	   << "| Price: $" << fixed << setprecision(2) << item.price;
	return os;
}

// Friend function: custom print details
void printItemDetails(const MenuItem& item) {
	cout << "===========================" << endl;
	cout << "       ITEM DETAILS" << endl;
	cout << "===========================" << endl;
	cout << item << endl;
	cout << "===========================" << endl;
}
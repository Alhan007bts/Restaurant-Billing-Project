#include "FoodItem.h"
#include <iostream>

using namespace std;

// =========================
// CONSTRUCTORS & DESTRUCTOR
// =========================

// default constructor
FoodItem::FoodItem()
    : MenuItem(), cuisineType("Unknown"), preparationTime(0), isSpicy(false) {}

// Parameterized constructor
FoodItem::FoodItem(string id, string name, string category, double price,
                   string cuisineType, int prepTime, bool isSpicy)
    : MenuItem(id, name, category, price), cuisineType(cuisineType),
      preparationTime(prepTime), isSpicy(isSpicy) {
  // use setters for validation
  setCuisineType(cuisineType);
  setPreparationTime(prepTime);
}

// destructor
FoodItem::~FoodItem() {
  // Cleanup if needed
}

// ===========
//   GETTERS
// ===========

string FoodItem::getCuisineType() const { return cuisineType; }
int FoodItem::getPreparationTime() const { return preparationTime; }
bool FoodItem::getIsSpicy() const { return isSpicy; }

// ===========
//   SETTERS
// ===========

/*
 * Set cuisine type
 * if the cuisine name is empty, defaults to "Unknown"
 */
void FoodItem::setCuisineType(string cuisine) {
  cuisineType = cuisine.empty() ? "Unknown" : cuisine;
}

/*
 * Set preparation time with validation.
 * Must be greater than 0 minutes. Negative or zero values
 * are set to 0 (invalid, but keeps system stable).
 */
void FoodItem::setPreparationTime(int minutes) {
  preparationTime = (minutes > 0) ? minutes : 0;
}

// Set spiciness status
void FoodItem::setIsSpicy(bool spicy) { isSpicy = spicy; }

// ============================
// OVERRIDDEN VIRTUAL FUNCTIONS
// ============================

// Display food item with all details
void FoodItem::displayItem() const {
  cout << *this; // uses friend operator<< from base class
  cout << " | Cuisine: " << cuisineType << " | Prep Time: " << preparationTime
       << " min";
  if (isSpicy) {
    cout << " | SPICY";
  }
  cout << endl;
}

// Calculate discounted price for food items
double FoodItem::calculateDiscountedPrice(double discountPercentage) const {
  // Food items: max 30% discount
  if (discountPercentage > 30.0)
    discountPercentage = 30.0;
  if (discountPercentage < 0.0)
    discountPercentage = 0.0;
  return getPrice() * (1.0 - discountPercentage / 100.0);
}

// Calculate final price including tax for foood items
double FoodItem::calculateFinalPrice(double taxRate) const {
  // Clamp tax rate to valid range
  if (taxRate < 0.0)
    taxRate = 0.0;
  if (taxRate > 1.0)
    taxRate = 1.0;

  // Food items: add preparation fee before tax
  double prepFee = calculatePreparationFee();
  double subtotal = getPrice() + prepFee;
  return subtotal * (1.0 + taxRate);
}

// Calculate special offer price for food items
double FoodItem::calculateSpecialOfferPrice() const {
  // Lunch special: 20% off food items during lunch hours
  return calculateDiscountedPrice(20.0);
}

// Update price with convenience fee for food items
// NOTE FOR SYED: Please remove the convenienceFee logic here so that prices(removed)
// updated through updatePrice match the input newPrice exactly without adding
// hidden surcharges.
void FoodItem::updatePrice(double newPrice) {
    MenuItem::setPrice(newPrice);
}

// ==========================
// FOOD ITEM SPECIFIC METHODS
// ==========================

// Calculate preparation fee based on time
double FoodItem::calculatePreparationFee() const {
  // $0.25 per minute of preparation time
  return preparationTime * 0.25;
}
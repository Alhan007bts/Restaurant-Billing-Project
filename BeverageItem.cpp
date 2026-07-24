#include "BeverageItem.h"
#include <iostream>
#include <iomanip>

using namespace std;

// ===========================
//  CONSTRUCTORS & DESTRUCTOR
// ===========================

/*
* Default constructor - initializes beverage with default values
* Chains to MenuItem default constructor
*/
BeverageItem::BeverageItem()
	: MenuItem(), volume(0.0), beverageType("Unknown"), isCarbonated(false) {}

/*
* Parameterized constructor - creates a beverage with specified attributes
* Chains to MenuItem parameterized constructor for base class initialization
*/
BeverageItem::BeverageItem(string id, string name, string category, double price, double volume, string beverageType, bool isCarbonated)
	: MenuItem(id, name, category, price),
	volume(volume),
	beverageType(beverageType),
	isCarbonated(isCarbonated) {
	setVolume(volume);
	setBeverageType(beverageType);
}

// Destructor
BeverageItem::~BeverageItem() {
	// Cleanup if needed
}

// ===========
//   GETTERS
// ===========
double BeverageItem::getVolume() const { 
	return volume; 
}

string BeverageItem::getBeverageType() const {
	return beverageType; 
}

bool BeverageItem::getIsCarbonated() const {
	return isCarbonated;
}

// ===========
//   SETTERS
// ===========

/*
* Set volume with validation - must be positive
*/
void BeverageItem::setVolume(double volume) {
	this->volume = (volume > 0) ? volume : 0;
}

/*
* Set beverage type - defaults to "Unknown" if empty
*/
void BeverageItem::setBeverageType(string beverageType) {
	this->beverageType = beverageType.empty() ? "Unknown" : beverageType;
}

/*
* Set carbonation status
*/
void BeverageItem::setIsCarbonated(bool isCarbonated) {
	this->isCarbonated = isCarbonated;
}

// =============================
//  OVERRIDEN VIRTUAL FUNCTIONS
// =============================

/*
* Display beverage item details
*/
void BeverageItem::displayItem() const {
	cout << *this; // Uses friend operator<< from the base class for common fields
	cout << " | Volume: " << volume << " ml"
		<< " | Type: " << beverageType;
	if (isCarbonated) {
		cout << " | Carbonated";
	}
	cout << endl;
}

/*
* Calculate discounted price for beverages
* Beverages have a maximum discount of 15% (restaurant policy)
*/
double BeverageItem::calculateDiscountedPrice(double discountPercentage) const {
	// Beverage: max 15% discount
	if (discountPercentage > 15.0) discountPercentage = 15.0;
	if (discountPercentage < 0.0) discountPercentage = 0.0;
	return getPrice() * (1.0 - discountPercentage / 100.0);
}

/*
* Calculate final price including tax
* Beverages have no preparation fee, just tax
*/
double BeverageItem::calculateFinalPrice(double taxRate) const {
	if (taxRate < 0.0) taxRate = 0.0;
	if (taxRate > 1.0) taxRate = 1.0;

	// Beverages: no preparation fee, just tax
	return getPrice() * (1.0 + taxRate);
}

/*
* Calculate special offer price for beverages
* happy hour: 50% off beverages
*/
double BeverageItem::calculateSpecialOfferPrice() const {
	// Happy hour: 50% off beverages
	return calculateDiscountedPrice(50.0);
}

/*
* Update price for beverages
* Beverages have no extra fees (unlike food items)
*/
void BeverageItem::updatePrice(double newPrice) {
	// Beverages: no extra fees
	MenuItem::setPrice(newPrice);
}

// ================================
//  BEVERAGE ITEM SPECIFIC METHODS
// ================================

/*
* Calculate price per millimeter - useful for comparing value
* Example: 355ml can of soda at $3.50 = $0.0099/ml
*/
double BeverageItem::calculatePricePerMilliliter() const {
	if (volume <= 0) return 0;
	return getPrice() / volume;
}
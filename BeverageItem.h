#pragma once
#include <string>
#include "MenuItem.h"

class BeverageItem : public MenuItem {
private:
	double volume;				 // in milliliters
	std::string beverageType;    // e.g., Soft Drink, Juice, Coffee, Tea, etc..
	bool isCarbonated;			 // true if the beverage is carbonated, false otherwise

public:
	// ========================================
	// CONSTRUCTORS & DESTRUCTOR
	// ========================================

	/*
	 * Default constructor
	 * Initializes BeverageItem with default values:
     * - Volume: 0 ml
     * - Type: "Unknown"
     * - Not carbonated
	 * Chains to MenuItem default constructor
	 */
	BeverageItem();

	/*
	* Parameterized constructor
	* Creates a beverage item with all attributes specified
	* Validates beverages-specific attributes through setters
	*/
	BeverageItem(std::string id, std::string name, std::string category, double price,
		         double volume, std::string beverageType, bool isCarbonated);

	// Destructor - Cleans up resources if needed
	~BeverageItem();

	// =========
	//  GETTERS
	// =========
	double getVolume() const;
	std::string getBeverageType() const;
	bool getIsCarbonated() const;

	// =========
	//  SETTERS
	// =========

	// Set volume with validation
	void setVolume(double volume);

	// Set beverage type
	void setBeverageType(std::string beverageType);

	// Set carbonation
	void setIsCarbonated(bool carbonated);

	// ====================================
	//     OVERRIDDEN VIRTUAL FUNCTIONS
	// ====================================

	/*
	 * Display beverage item details.
	 * Shows base item info + beverage-specific details:
	 * - Volume
	 * - Type
	 * - Carbonation status
	 */
	void displayItem() const override;

	/*
	*  Calculate discounted price for beverages.
    * Beverages have a maximum discount of 15% (happy hour policy).
	*/
	double calculateDiscountedPrice(double discountPercentage) const override;

	/*
	* Calculate final price including tax for beverages. 
    * Beverages have no preparation fee, just tax.
	*/
	double calculateFinalPrice(double taxRate = 0.13) const override;

	/*
	* Calculate special offer price for beverages.
	* Happy hour: 50% off beverages.
	*/
	double calculateSpecialOfferPrice() const override;

	/*
	* Update price for beverages. 
    * Beverages have no extra fees.
	*/
	void updatePrice(double newPrice) override;

	// ========================================
	// BEVERAGE ITEM SPECIFIC METHODS
	// ========================================

	/* 
	* Calculate price per milliliter based on volume and price
	* Useful for comparing value between different beverages.
	* Examples:
    * - 355ml soda at $3.50 = $0.0099/ml
    * - 500ml juice at $4.99 = $0.00998/ml
    * - 240ml coffee at $5.50 = $0.0229/ml (more expensive per ml)
	*/

	double calculatePricePerMilliliter() const; 
};
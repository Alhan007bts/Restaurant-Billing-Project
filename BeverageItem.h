#pragma once
#include <string>
#include "MenuItem.h"

class BeverageItem : public MenuItem {
private:
	double volume;           // in milliliters
	std::string beverageType;    // e.g., Soft Drink, Juice, Coffee, Tea, etc..
	bool isCarbonated;      // true if the beverage is carbonated, false otherwise

public:
	// Constructors
	BeverageItem();
	BeverageItem(std::string id, std::string name, std::string category, double price,
		         double volume, std::string beverageType, bool isCarbonated);

	// Destructor
	~BeverageItem();

	// Getters
	double getVolume() const;
	std::string getBeverageType() const;
	bool getIsCarbonated() const;

	// Setters
	void setVolume(double volume);
	void setBeverageType(std::string beverageType);
	void setIsarbonated(bool carbonated);

	// Override virtual functions
	void displayItem() const override;
	double calculateDiscountedPrice(double discountPercentage) const override;
	double calculateFinalPrice(double taxRate = 0.13) const override;
	double calculateSpecialOfferPrice() const override;
	void updatePrice(double newPrice) override;

	// Beverage item specific methods
	// Calculate price per milliliter based on volume and price
	double calculateOricePerMilliliter() const; 
};
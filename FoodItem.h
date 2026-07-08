#pragma once
#include <string>
#include "MenuItem.h"

class FoodItem : public MenuItem {
private:
	std::string cuisineType;		// e.g., Italian, Chinese, Indian, Mexican, etc.
	int preparationTime;	// in minutes
	bool isSpicy;			// true if the food item is spicy, false otherwise

public:
	// Constructors
	FoodItem();
	FoodItem(std::string id, std::string name, std::string category, double price,
		     std::string cuisineType, int prepTime, bool isSpicy);

	// Destructor
	~FoodItem();

	// Getters
	std::string getCuisineType() const;
	int getPreperationTime() const;
	bool getIsSpicy() const;

	// Setters
	void setCuisineType(std::string cuisine);
	void setPreperationTime(int minutes);
	void setIsSpicy(bool spicy);

	// Override virtual functions
	void displayItem() const override;
	double calculateDiscountedPrice(double discountPercentage) const override;
	double calculateFinalPrice(double taxRate = 0.13) const override;
	double calculateSpecialOfferPrice() const override;
	void updatePrice(double newPrice) override;

	// Food item speific methods
	double calculatePreparationFee() const; // Calculate preparation fee based on preparation time
};

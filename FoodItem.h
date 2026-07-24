#pragma once
#include <string>
#include "MenuItem.h"

class FoodItem : public MenuItem {
private:
	std::string cuisineType;		// e.g., Italian, Chinese, Indian, Mexican, etc.
	int preparationTime;			// in minutes
	bool isSpicy;					// true if the food item is spicy, false otherwise

public:
	/*
     * Default constructor.
     * Initializes FoodItem with default values:
     * - Cuisine: "Unknown"
     * - Preparation time: 0 minutes
     * - Not spicy
     * 
     * Chains to MenuItem default constructor.
     */
	FoodItem();
	
	/* 
	 * Parameterized constructor
     * Creates a food item with all attributes specified.
     * Validates food-specific attributes through setters.
     */
	FoodItem(std::string id, std::string name, std::string category, double price,
		     std::string cuisineType, int prepTime, bool isSpicy);

	// Destructor - Cleans up any resources if needed
	~FoodItem();

	// =========
	//  GETTERS
	// =========
	std::string getCuisineType() const;
	int getPreparationTime() const;
	bool getIsSpicy() const;

	// =========
    //  SETTERS
    // =========
	void setCuisineType(std::string cuisine);
	void setPreparationTime(int minutes);
	void setIsSpicy(bool spicy);

	// ========================================
	// OVERRIDDEN VIRTUAL FUNCTIONS
	// ========================================

	/*
	* Display food item details
	* Shows base item info + food-specific details:
	* - Cuisine type
	* - Preparation time
	* - Spiciness indicator
	*/
	void displayItem() const override;

	/*
	* Calculate discounted price for food items.
    * Food items have a maximum discount of 30% (restaurant policy).
	*/
	double calculateDiscountedPrice(double discountPercentage) const override;

	/* 
	* Calculate final price including tax.
	* Food items add a preparation fee before tax.
	*/
	double calculateFinalPrice(double taxRate = 0.13) const override;

	/*
	 * Calculate special offer price for food items. 
     * Lunch special: 20% off food items.
     */
	double calculateSpecialOfferPrice() const override;

	/*
	 * Update price for food items.
     * Adds a $0.50 convenience fee when updating food prices.
	 */
	void updatePrice(double newPrice) override;

	/*
     * Calculate preparation fee.
     * Fee is $0.25 per minute of preparation time.
     * Example: 20 minutes = $5.00 preparation fee.
	 */
	double calculatePreparationFee() const; 
};

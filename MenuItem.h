#pragma once
#include <iostream>
#include <iomanip>
#include <memory>
#include <string>

class MenuItem {
private:
	std::string itemID;
	std::string name;
	std::string category;
	double price;

protected:
	// protected constuctor for the derived classes to use
	MenuItem();

public:
	// parameterized constructor
	MenuItem(std::string id, std::string name, std::string category, double price);

	// virtual destructor (for inheritance)
	virtual ~MenuItem();

	// Getters
	std::string getItemID() const;
	std::string getName() const;
	std::string getCategory() const;
	double getPrice() const;

	// Setters
	void setItemID(std::string id);
	void setName(std::string name);
	void setCategory(std::string category);
	void setPrice(double price);

	// ==========================
	// Dynamic price calculations
	// ==========================

	// Calculate final price with discount
	virtual double calculateDiscountedPrice(double discountPercentage) const;

	// Calculate final price with tax
	virtual double calculateFinalPrice(double taxRate = 0.13) const;

	// Calculate price with special offers
	virtual double calculateSpecialOfferPrice() const;

	// Virtual method to update price (can be overridden in derived classes)
	virtual void updatePrice(double newPrice);

	// Pure Virtual Function (makes MenuItem an abstract class)
	virtual void displayItem() const = 0;

	// Operator overloading 
	bool operator==(const MenuItem& other) const;
	bool operator!=(const MenuItem& other) const;

	// Friend function for printing (integration)
	friend std::ostream& operator<<(std::ostream& os, const MenuItem& item);

	// Additional friend function for custom printing
	friend void printItemDetails(const MenuItem& item);
};

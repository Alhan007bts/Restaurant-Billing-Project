# BiteKing System Test Plan

This document outlines the test cases and step-by-step instructions to verify the features, branding adjustments, and custom controls of the BiteKing Restaurant Billing & Ordering System.

---

## Compilation & Launch

Before running the test cases, ensure you have the latest executable built:
```bash
# 1. Navigate to the project root directory
# 2. Build the project
cmake -B build -S .
cmake --build build

# 3. Launch the application
./build/restaurant_billing_qt
```

---

## Core Test Cases

### Test Case 1: Dining Table Order Isolation
**Objective:** Verify that orders are correctly isolated and maintained per dining table.
1. Select Table 1 from the dining table dropdown at the top.
2. Select Margherita Pizza from the Menu Items list, set quantity to 2 using the up arrow button, and click Add to Order.
3. Select Table 2 from the dining table dropdown.
4. Verify that the Current Order table for Table 2 is completely empty.
5. Select Coca Cola from the Menu Items list and click Add to Order.
6. Switch back to Table 1. Verify that the two Margherita Pizzas are still in the order list.
7. Click Generate Receipt for Table 1 and verify the receipt displays Margherita Pizza.

---

### Test Case 2: Convenience Fee Bypass & Direct Price Editing
**Objective:** Verify that manual menu price changes in the UI are applied exactly as entered, bypassing convenience fee logic, and saved to data/menu.txt.
1. Double-click the price cell for Fresh Orange Juice in the Menu Items list.
2. Change the price to exactly 5.50 (or type 5.50) and press Enter.
3. Select Fresh Orange Juice, set quantity to 1, and click Add to Order.
4. Set Discount (%) to 0 and Tax Rate (%) to 0.
5. Click Generate Receipt.
6. Verify in the receipt display area that:
   - The price for Orange Juice is exactly 5.50.
   - The Subtotal is exactly 5.50.
   - The GRAND TOTAL is exactly 5.50 (meaning no convenience fee/surcharge was appended).
7. Open the file data/menu.txt in a text editor or view it. Verify that the entry for Fresh Orange Juice is saved with the updated price of 5.50.

---

### Test Case 3: Custom Up/Down Spinner Buttons
**Objective:** Verify that the custom vertical up (▲) and down (▼) buttons correctly adjust the item quantity.
1. Select any item in the Menu Items table.
2. Click the small ▲ button next to the quantity input box. Verify the quantity increases to 2.
3. Click the small ▼ button next to the quantity input box. Verify the quantity decreases to 1.
4. Click the ▼ button again. Verify the quantity remains at 1 (minimum value boundary check).
5. Type 99 directly into the quantity number box. Click ▲. Verify the quantity is 100.
6. Click ▲ again. Verify the quantity remains at 100 (maximum value boundary check).

---

### Test Case 4: Item Removal
**Objective:** Verify that items can be removed from an active order and the receipt recalculates.
1. With Table 1 selected, add Margherita Pizza and Iced Latte to the order.
2. In the Current Order table (bottom left), click on the row containing Iced Latte.
3. Click the red Remove Selected button at the very bottom.
4. Verify that Iced Latte is removed from the Current Order table.
5. Click Generate Receipt and verify that only Margherita Pizza appears in the invoice layout.

---

### Test Case 5: Receipt Calculations (Discounts & Taxes)
**Objective:** Verify that discount percentages and tax rates are calculated correctly on the receipt.
1. Select any table and add items with a subtotal of exactly 20.00 (e.g. 4 copies of Orange Juice at 5.00 each).
2. Set Tax Rate (%) to 10.00.
3. Set Discount (%) to 10.00.
4. Click Generate Receipt.
5. Verify the arithmetic in the generated receipt:
   - Subtotal: 20.00
   - Discount (10%): -2.00
   - Preparation Fees: 3.00 (static)
   - Tax (10% on discounted subtotal + prep fee, i.e., 18.00 + 3.00 = 21.00): 2.10
   - GRAND TOTAL: 20.00 - 2.00 + 3.00 + 2.10 = 23.10.

---

### Test Case 6: Theme Switching & Vector Custom Controls
**Objective:** Verify Light and Dark theme visuals, border radiuses, and dropdown rendering.
1. Click Theme in the top menu bar, and select Dark Theme.
2. Verify:
   * The top BiteKing title card has a clean Slate background with a light blue vertical accent bar on the left and no gradient.
   * All tables and the invoice text edit box have soft 12px rounded corners.
   * The dining table selector has a rounded background and native arrow rendering with no sharp corners sticking out.
3. Click Theme in the top menu bar, and select Light Theme.
4. Verify:
   * The top card updates to a solid white background with a royal blue vertical accent bar on the left.
   * The background changes to Slate-white #f8fafc and all panels maintain soft 12px rounded corners.

---

### Test Case 7: Menu Import & Export
**Objective:** Verify that the menu can be saved to and loaded from external text files.
1. Click Theme -> Dark Theme.
2. Click File in the top menu bar, and select Export Menu.
3. Save the menu as test_menu.txt.
4. Double click several prices in the Menu Items table and change them.
5. Click File in the top menu bar, and select Import Menu.
6. Select the saved test_menu.txt file.
7. Verify that all menu item prices restore to their original exported values.

---

## Verification Checklist

| Test Case | Description | Expected Result | Status (Pass/Fail) |
|---|---|---|---|
| TC-1 | Order Isolation | Tables 1 and 2 maintain separate orders | [ ] |
| TC-2 | Convenience Fee Bypass | Manual prices are exact; Orange Juice saved to file | [ ] |
| TC-3 | Up/Down Spinner Buttons | Custom ▲/▼ buttons increment/decrement (1-100) | [ ] |
| TC-4 | Item Removal | Remove Selected updates order and receipt in real time | [ ] |
| TC-5 | Calculations | Subtotal, discounts, preparation fees, and taxes sum up correctly | [ ] |
| TC-6 | Theme & Rounding | Banner left accent displays; 12px rounded corners match | [ ] |
| TC-7 | Menu Import/Export | Menu configurations export and restore correctly | [ ] |

#pragma once

#include "MenuItem.h"
#include "RestaurantManager.h"
#include "DineInOrder.h"
#include "Bill.h"
#include "Payment.h"
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QListWidget>
#include <QMainWindow>
#include <QPushButton>
#include <QSpinBox>
#include <QTableWidget>
#include <QTextEdit>
#include <QTabWidget>
#include <QLineEdit>
#include <memory>
#include <vector>

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

protected:
  // Overridden to intercept window closing for unsaved changes detection
  void closeEvent(QCloseEvent *event) override;

private slots:
  // Existing Slots
  void onAddItemClicked();
  void onRemoveItemClicked();
  void onCalculateBillClicked();
  void onClearBillClicked();
  void onTableChanged(int index);
  void onAddMenuItemClicked();
  void onImportMenuTriggered();
  void onExportMenuTriggered();
  void onImportReceiptTriggered();
  void onExportReceiptTriggered();
  void onDarkThemeTriggered();
  void onLightThemeTriggered();
  void onDeleteMenuItemClicked();
  void onMenuItemChanged(QTableWidgetItem *item);
  void onMinusClicked();
  void onPlusClicked();

  // New Slots for PDF specs
  void onSearchFilterChanged();
  void onProcessPaymentClicked();
  void onSaveDataTriggered();
  void onLoadDataTriggered();

private:
  void setupUI();
  void populateMenu();
  void updateOrderTable();
  void refreshMenuTable();
  void applyTheme(bool dark);

  // New layout setup helpers
  void setupMenuTab(QWidget* tab);
  void setupOrdersTab(QWidget* tab);
  void setupBillingTab(QWidget* tab);
  void setupPaymentsTab(QWidget* tab);
  void setupAnalyticsTab(QWidget* tab);

  // Helper logic for updating views
  void refreshBillingOrderCombo();
  void refreshPaymentsBillCombo();
  void updateAnalyticsDisplay();
  void checkUnsavedChanges();

  // Core Data
  std::vector<std::shared_ptr<MenuItem>> menuItems;
  RestaurantManager manager;
  int currentTableNumber;

  // New spec-specific data collections
  std::vector<DineInOrder> dineInOrders;
  std::vector<Bill> restaurantBills;
  std::vector<Payment> restaurantPayments;
  bool unsavedChanges; // Flag for Feature 5 - Unsaved Changes Detection

  // GUI Tabs
  QTabWidget *tabWidget;

  // Tab 1: Menu Widgets
  QTableWidget *menuTable;
  QLineEdit *searchEdit;
  QComboBox *categoryFilterCombo;

  // Tab 2: Orders Widgets
  QTableWidget *orderTable;
  QComboBox *tableComboBox;
  QSpinBox *quantitySpinner;
  QPushButton *addButton;
  QPushButton *removeButton;
  QLabel *orderSubtotalLabel;

  // Tab 3: Billing Widgets
  QComboBox *billingOrderCombo;
  QDoubleSpinBox *taxSpinner;
  QDoubleSpinBox *discountSpinner;
  QPushButton *calculateButton;
  QTextEdit *receiptDisplay;
  QLabel *totalLabel;
  QLabel *largeBillWarningLabel; // Feature 3: Large Bill Warning

  // Tab 4: Payments Widgets
  QComboBox *paymentsBillCombo;
  QComboBox *paymentMethodCombo;
  QDoubleSpinBox *paymentAmountSpinner;
  QTextEdit *paymentReceiptDisplay;
  QPushButton *processPaymentButton;

  // Tab 5: Analytics Widgets
  QLabel *dailySalesLabel;
  QLabel *avgBillLabel;
  QLabel *ordersOpenLabel;
  QLabel *ordersPaidLabel;
  QLabel *ordersCancelledLabel;
  QListWidget *popularItemsList;
};

#pragma once

#include "MenuItem.h"
#include "RestaurantManager.h"
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QListWidget>
#include <QMainWindow>
#include <QPushButton>
#include <QSpinBox>
#include <QTableWidget>
#include <QTextEdit>
#include <memory>
#include <vector>

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private slots:
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

private:
  void setupUI();
  void populateMenu();
  void updateOrderTable();
  void refreshMenuTable();

  // Data members
  std::vector<std::shared_ptr<MenuItem>> menuItems;
  RestaurantManager manager;
  int currentTableNumber;

  // UI Widgets
  QTableWidget *menuTable;
  QTableWidget *orderTable;
  QComboBox *tableComboBox;
  QSpinBox *quantitySpinner;
  QDoubleSpinBox *discountSpinner;
  QDoubleSpinBox *taxSpinner;
  QPushButton *addButton;
  QPushButton *removeButton;
  QPushButton *calculateButton;
  QPushButton *clearButton;
  QPushButton *addMenuItemButton;
  QTextEdit *receiptDisplay;
  QLabel *totalLabel;
};

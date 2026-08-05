#include "MainWindow.h"
#include "BeverageItem.h"
#include "FoodItem.h"
#include "MenuFileIO.h"
#include "Order.h"
#include "DineInOrder.h"
#include "Bill.h"
#include "Payment.h"
#include "AnalyticsEngine.h"
#include "DataManager.h"
#include <QAction>
#include <QActionGroup>
#include <QCheckBox>
#include <QDateTime>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDir>
#include <QFileDialog>
#include <QFormLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLineEdit>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QCloseEvent>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>

// Dialog form for creating a new food or beverage item
class AddMenuItemDialog : public QDialog {
private:
  QLineEdit *nameEdit;
  QDoubleSpinBox *priceSpinner;
  QComboBox *typeCombo;

  QWidget *foodWidget;
  QLineEdit *cuisineEdit;
  QSpinBox *prepTimeSpinner;
  QCheckBox *spicyCheck;

  QWidget *bevWidget;
  QDoubleSpinBox *volumeSpinner;
  QLineEdit *bevTypeEdit;
  QCheckBox *carbonatedCheck;

public:
  AddMenuItemDialog(QWidget *parent = nullptr) : QDialog(parent) {
    setWindowTitle("Add New Menu Item");
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QFormLayout *formLayout = new QFormLayout();
    nameEdit = new QLineEdit(this);
    priceSpinner = new QDoubleSpinBox(this);
    priceSpinner->setRange(0.01, 10000.0);
    priceSpinner->setValue(5.00);

    typeCombo = new QComboBox(this);
    typeCombo->addItem("Food", "FOOD");
    typeCombo->addItem("Beverage", "BEVERAGE");

    formLayout->addRow("Name:", nameEdit);
    formLayout->addRow("Price ($):", priceSpinner);
    formLayout->addRow("Category Type:", typeCombo);
    mainLayout->addLayout(formLayout);

    // --- Food Fields ---
    foodWidget = new QWidget(this);
    QFormLayout *foodLayout = new QFormLayout(foodWidget);
    cuisineEdit = new QLineEdit(foodWidget);
    cuisineEdit->setText("Unknown");
    prepTimeSpinner = new QSpinBox(foodWidget);
    prepTimeSpinner->setRange(1, 180);
    prepTimeSpinner->setValue(10);
    spicyCheck = new QCheckBox("Is Spicy", foodWidget);
    foodLayout->addRow("Cuisine Type:", cuisineEdit);
    foodLayout->addRow("Prep Time (min):", prepTimeSpinner);
    foodLayout->addRow(spicyCheck);
    mainLayout->addWidget(foodWidget);

    // --- Beverage Fields ---
    bevWidget = new QWidget(this);
    QFormLayout *bevLayout = new QFormLayout(bevWidget);
    volumeSpinner = new QDoubleSpinBox(bevWidget);
    volumeSpinner->setRange(0.1, 5000.0);
    volumeSpinner->setValue(330.0);
    bevTypeEdit = new QLineEdit(bevWidget);
    bevTypeEdit->setText("Unknown");
    carbonatedCheck = new QCheckBox("Is Carbonated", bevWidget);
    bevLayout->addRow("Volume (ml):", volumeSpinner);
    bevLayout->addRow("Beverage Type:", bevTypeEdit);
    bevLayout->addRow(carbonatedCheck);
    mainLayout->addWidget(bevWidget);

    foodWidget->show();
    bevWidget->hide();

    connect(typeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [this](int index) {
              QString selected = typeCombo->itemData(index).toString();
              if (selected == "FOOD") {
                foodWidget->show();
                bevWidget->hide();
              } else {
                foodWidget->hide();
                bevWidget->show();
              }
              adjustSize();
            });

    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttonBox);

    setStyleSheet(
        "QDialog { background-color: #1a1a24; color: #e2e8f0; }"
        "QLabel { color: #63b3ed; font-weight: bold; }"
        "QLineEdit, QSpinBox, QDoubleSpinBox, QComboBox { background-color: "
        "#262635; border: 1px solid #3f3f5a; border-radius: 5px; padding: 4px; "
        "color: white; }"
        "QPushButton { background-color: #3182ce; color: white; border: none; "
        "padding: 6px 12px; border-radius: 5px; font-weight: bold; }"
        "QPushButton:hover { background-color: #4299e1; }"
        "QCheckBox { color: #e2e8f0; }");
  }

  QString getName() const { return nameEdit->text(); }
  double getPrice() const { return priceSpinner->value(); }
  QString getType() const { return typeCombo->currentData().toString(); }

  QString getCuisine() const { return cuisineEdit->text(); }
  int getPrepTime() const { return prepTimeSpinner->value(); }
  bool getIsSpicy() const { return spicyCheck->isChecked(); }

  double getVolume() const { return volumeSpinner->value(); }
  QString getBeverageType() const { return bevTypeEdit->text(); }
  bool getIsCarbonated() const { return carbonatedCheck->isChecked(); }
};

// MainWindow Implementation
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), currentTableNumber(1), unsavedChanges(false) {
  for (int i = 1; i <= 8; ++i) {
    manager.addTable(i);
  }
  setupUI();
  populateMenu();

  // Feature 4: Try loading previously saved transaction data on startup
  std::string err;
  DataManager::loadData("data", dineInOrders, restaurantBills, restaurantPayments, &err);

  // Sync menu registry for order class calculations
  Order::menuRegistry = menuItems;

  refreshBillingOrderCombo();
  refreshPaymentsBillCombo();
  updateAnalyticsDisplay();
}

MainWindow::~MainWindow() {}

// Feature 5: Prompt to save when closing the window with unsaved transactions
void MainWindow::closeEvent(QCloseEvent *event) {
  if (unsavedChanges) {
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Unsaved Changes",
        "You have unsaved transaction data. Would you like to save before exiting?",
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    
    if (reply == QMessageBox::Save) {
      std::string err;
      QDir().mkpath("data");
      if (DataManager::saveData("data", dineInOrders, restaurantBills, restaurantPayments, &err)) {
        QMessageBox::information(this, "Saved", "All transactions saved successfully!");
        event->accept();
      } else {
        QMessageBox::critical(this, "Save Error", QString::fromStdString(err));
        event->ignore();
      }
    } else if (reply == QMessageBox::Cancel) {
      event->ignore();
    } else {
      event->accept();
    }
  } else {
    event->accept();
  }
}

void MainWindow::setupUI() {
  setWindowTitle("BiteKing - Restaurant Billing & Ordering System");
  resize(1100, 750);
  applyTheme(true); // Default to dark theme

  // Create menu bar actions
  QMenuBar *mBar = this->menuBar();
  QMenu *fileMenu = mBar->addMenu("&File");

  QAction *importMenuAct = new QAction("&Import Menu...", this);
  connect(importMenuAct, &QAction::triggered, this, &MainWindow::onImportMenuTriggered);
  fileMenu->addAction(importMenuAct);

  QAction *exportMenuAct = new QAction("&Export Menu...", this);
  connect(exportMenuAct, &QAction::triggered, this, &MainWindow::onExportMenuTriggered);
  fileMenu->addAction(exportMenuAct);

  fileMenu->addSeparator();

  QAction *saveDataAct = new QAction("&Save Transaction Data", this);
  connect(saveDataAct, &QAction::triggered, this, &MainWindow::onSaveDataTriggered);
  fileMenu->addAction(saveDataAct);

  QAction *loadDataAct = new QAction("&Load Transaction Data", this);
  connect(loadDataAct, &QAction::triggered, this, &MainWindow::onLoadDataTriggered);
  fileMenu->addAction(loadDataAct);

  fileMenu->addSeparator();

  QAction *importReceiptAct = new QAction("Import &Receipt...", this);
  connect(importReceiptAct, &QAction::triggered, this, &MainWindow::onImportReceiptTriggered);
  fileMenu->addAction(importReceiptAct);

  QAction *exportReceiptAct = new QAction("Export R&eceipt...", this);
  connect(exportReceiptAct, &QAction::triggered, this, &MainWindow::onExportReceiptTriggered);
  fileMenu->addAction(exportReceiptAct);

  fileMenu->addSeparator();

  QAction *exitAct = new QAction("E&xit", this);
  connect(exitAct, &QAction::triggered, this, &QWidget::close);
  fileMenu->addAction(exitAct);

  QMenu *themeMenu = mBar->addMenu("&Theme");
  QActionGroup *themeGroup = new QActionGroup(this);

  QAction *darkThemeAct = new QAction("&Dark Theme", this);
  darkThemeAct->setCheckable(true);
  darkThemeAct->setChecked(true);
  connect(darkThemeAct, &QAction::triggered, this, &MainWindow::onDarkThemeTriggered);
  themeMenu->addAction(darkThemeAct);
  themeGroup->addAction(darkThemeAct);

  QAction *lightThemeAct = new QAction("&Light Theme", this);
  lightThemeAct->setCheckable(true);
  connect(lightThemeAct, &QAction::triggered, this, &MainWindow::onLightThemeTriggered);
  themeMenu->addAction(lightThemeAct);
  themeGroup->addAction(lightThemeAct);

  QMenu *editMenu = mBar->addMenu("&Edit");
  QAction *addMenuItemAct = new QAction("&Add Menu Item...", this);
  connect(addMenuItemAct, &QAction::triggered, this, &MainWindow::onAddMenuItemClicked);
  editMenu->addAction(addMenuItemAct);

  QAction *deleteMenuItemAct = new QAction("&Delete Selected Menu Item", this);
  connect(deleteMenuItemAct, &QAction::triggered, this, &MainWindow::onDeleteMenuItemClicked);
  editMenu->addAction(deleteMenuItemAct);

  // Central widget and top banner
  QWidget *centralWidget = new QWidget(this);
  centralWidget->setObjectName("centralWidget");
  setCentralWidget(centralWidget);
  QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

  QWidget *headerWidget = new QWidget(centralWidget);
  headerWidget->setObjectName("headerWidget");
  headerWidget->setFixedHeight(65);
  QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
  headerLayout->setContentsMargins(15, 5, 15, 5);

  QLabel *logoLabel = new QLabel("👑", centralWidget);
  logoLabel->setObjectName("logoLabel");
  logoLabel->setStyleSheet("font-size: 28px;");
  headerLayout->addWidget(logoLabel);

  QVBoxLayout *titleTextLayout = new QVBoxLayout();
  titleTextLayout->setSpacing(2);
  QLabel *titleLabel = new QLabel("BiteKing", centralWidget);
  titleLabel->setObjectName("titleLabel");
  QLabel *subtitleLabel = new QLabel("Smart Restaurant Billing & Ordering System", centralWidget);
  subtitleLabel->setObjectName("subtitleLabel");
  titleTextLayout->addWidget(titleLabel);
  titleTextLayout->addWidget(subtitleLabel);
  headerLayout->addLayout(titleTextLayout);

  headerLayout->addStretch();
  mainLayout->addWidget(headerWidget);

  // Redesign: Organize features under Tabbed panels
  tabWidget = new QTabWidget(centralWidget);
  mainLayout->addWidget(tabWidget);

  QWidget *menuTab = new QWidget();
  QWidget *ordersTab = new QWidget();
  QWidget *billingTab = new QWidget();
  QWidget *paymentsTab = new QWidget();
  QWidget *analyticsTab = new QWidget();

  tabWidget->addTab(menuTab, "Menu");
  tabWidget->addTab(ordersTab, "Orders");
  tabWidget->addTab(billingTab, "Billing");
  tabWidget->addTab(paymentsTab, "Payments");
  tabWidget->addTab(analyticsTab, "Analytics");

  // Build each tab layout
  setupMenuTab(menuTab);
  setupOrdersTab(ordersTab);
  setupBillingTab(billingTab);
  setupPaymentsTab(paymentsTab);
  setupAnalyticsTab(analyticsTab);

  // Dynamic reload tab contents on change
  connect(tabWidget, &QTabWidget::currentChanged, this, [this](int index) {
    if (index == 2) { // Billing
      refreshBillingOrderCombo();
    } else if (index == 3) { // Payments
      refreshPaymentsBillCombo();
    } else if (index == 4) { // Analytics
      updateAnalyticsDisplay();
    }
  });
}

// 1. Menu Management Tab (Editing, addition, deletion of items)
void MainWindow::setupMenuTab(QWidget* tab) {
  QVBoxLayout *layout = new QVBoxLayout(tab);

  // Search & Filter controls
  QHBoxLayout *filterLayout = new QHBoxLayout();
  filterLayout->addWidget(new QLabel("Search Menu:", tab));
  searchEdit = new QLineEdit(tab);
  searchEdit->setPlaceholderText("Type name to search...");
  filterLayout->addWidget(searchEdit);

  filterLayout->addWidget(new QLabel("Category:", tab));
  categoryFilterCombo = new QComboBox(tab);
  categoryFilterCombo->addItem("All Categories", "");
  categoryFilterCombo->addItem("Appetizer", "Appetizer");
  categoryFilterCombo->addItem("Main Course", "Main Course");
  categoryFilterCombo->addItem("Dessert", "Dessert");
  categoryFilterCombo->addItem("Beverage", "Beverage");
  filterLayout->addWidget(categoryFilterCombo);

  layout->addLayout(filterLayout);

  // Menu items list
  menuTable = new QTableWidget(tab);
  menuTable->setColumnCount(5);
  menuTable->setHorizontalHeaderLabels({"Item ID", "Name", "Category", "Price", "Extra Details"});
  menuTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  menuTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  menuTable->setSelectionMode(QAbstractItemView::SingleSelection);
  layout->addWidget(menuTable);

  QHBoxLayout *buttonsLayout = new QHBoxLayout();
  QPushButton *addBtn = new QPushButton("Add New Item", tab);
  connect(addBtn, &QPushButton::clicked, this, &MainWindow::onAddMenuItemClicked);
  buttonsLayout->addWidget(addBtn);

  QPushButton *deleteBtn = new QPushButton("Delete Selected Item", tab);
  connect(deleteBtn, &QPushButton::clicked, this, &MainWindow::onDeleteMenuItemClicked);
  buttonsLayout->addWidget(deleteBtn);

  layout->addLayout(buttonsLayout);

  // Set up search and filter signals
  connect(searchEdit, &QLineEdit::textChanged, this, &MainWindow::onSearchFilterChanged);
  connect(categoryFilterCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onSearchFilterChanged);
}

// 2. Orders Tab: Select table, search menu, select quantity, add to active order list
void MainWindow::setupOrdersTab(QWidget* tab) {
  QHBoxLayout *layout = new QHBoxLayout(tab);

  // Left Pane: Menu selection specifically integrated into the same view
  QVBoxLayout *leftPane = new QVBoxLayout();
  layout->addLayout(leftPane, 3); // 3/5 ratio

  // Table Selector
  QHBoxLayout *tableSelectLayout = new QHBoxLayout();
  tableSelectLayout->addWidget(new QLabel("Select Dining Table:", tab));
  tableComboBox = new QComboBox(tab);
  for (int i = 1; i <= 8; ++i) {
    tableComboBox->addItem(QString("Table %1").arg(i), i);
  }
  connect(tableComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onTableChanged);
  tableSelectLayout->addWidget(tableComboBox);
  leftPane->addLayout(tableSelectLayout);

  // Search & Category Filters for ordering menu
  QHBoxLayout *filterLayout = new QHBoxLayout();
  filterLayout->addWidget(new QLabel("Search Item:", tab));
  orderSearchEdit = new QLineEdit(tab);
  orderSearchEdit->setPlaceholderText("Type name to search...");
  filterLayout->addWidget(orderSearchEdit);

  filterLayout->addWidget(new QLabel("Category:", tab));
  orderCategoryFilterCombo = new QComboBox(tab);
  orderCategoryFilterCombo->addItem("All Categories", "");
  orderCategoryFilterCombo->addItem("Appetizer", "Appetizer");
  orderCategoryFilterCombo->addItem("Main Course", "Main Course");
  orderCategoryFilterCombo->addItem("Dessert", "Dessert");
  orderCategoryFilterCombo->addItem("Beverage", "Beverage");
  filterLayout->addWidget(orderCategoryFilterCombo);
  leftPane->addLayout(filterLayout);

  // Menu selector table
  orderMenuTable = new QTableWidget(tab);
  orderMenuTable->setColumnCount(4);
  orderMenuTable->setHorizontalHeaderLabels({"Item ID", "Name", "Category", "Price"});
  orderMenuTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  orderMenuTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  orderMenuTable->setSelectionMode(QAbstractItemView::SingleSelection);
  leftPane->addWidget(orderMenuTable);

  // Quantity controls and addition
  QHBoxLayout *qtyLayout = new QHBoxLayout();
  qtyLayout->addWidget(new QLabel("Qty:", tab));
  quantitySpinner = new QSpinBox(tab);
  quantitySpinner->setRange(1, 100);
  quantitySpinner->setValue(1);
  qtyLayout->addWidget(quantitySpinner);

  QPushButton *upBtn = new QPushButton("▲", tab);
  upBtn->setObjectName("spinnerBtn");
  upBtn->setFixedSize(25, 25);
  connect(upBtn, &QPushButton::clicked, this, &MainWindow::onPlusClicked);
  qtyLayout->addWidget(upBtn);

  QPushButton *downBtn = new QPushButton("▼", tab);
  downBtn->setObjectName("spinnerBtn");
  downBtn->setFixedSize(25, 25);
  connect(downBtn, &QPushButton::clicked, this, &MainWindow::onMinusClicked);
  qtyLayout->addWidget(downBtn);

  addButton = new QPushButton("Add to Order", tab);
  connect(addButton, &QPushButton::clicked, this, &MainWindow::onAddItemClicked);
  qtyLayout->addWidget(addButton);
  leftPane->addLayout(qtyLayout);

  // Connect filter signals for active ordering menu
  connect(orderSearchEdit, &QLineEdit::textChanged, this, &MainWindow::onOrderSearchFilterChanged);
  connect(orderCategoryFilterCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onOrderSearchFilterChanged);


  // Right Pane: Active dining table order lines list
  QVBoxLayout *rightPane = new QVBoxLayout();
  layout->addLayout(rightPane, 2); // 2/5 ratio

  rightPane->addWidget(new QLabel("Current Order Items:", tab));
  orderTable = new QTableWidget(tab);
  orderTable->setColumnCount(4);
  orderTable->setHorizontalHeaderLabels({"Name", "Price", "Qty", "Subtotal"});
  orderTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  orderTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  orderTable->setSelectionMode(QAbstractItemView::SingleSelection);
  rightPane->addWidget(orderTable);

  orderSubtotalLabel = new QLabel("Subtotal: $0.00", tab);
  orderSubtotalLabel->setObjectName("totalLabel");
  rightPane->addWidget(orderSubtotalLabel);

  // Order line deletion and clearing
  QHBoxLayout *orderControlLayout = new QHBoxLayout();
  removeButton = new QPushButton("Remove Selected", tab);
  removeButton->setObjectName("removeOrderBtn");
  connect(removeButton, &QPushButton::clicked, this, &MainWindow::onRemoveItemClicked);
  orderControlLayout->addWidget(removeButton);

  QPushButton *clearAllBtn = new QPushButton("Clear Table Order", tab);
  clearAllBtn->setObjectName("removeOrderBtn");
  connect(clearAllBtn, &QPushButton::clicked, this, &MainWindow::onClearBillClicked);
  orderControlLayout->addWidget(clearAllBtn);
  rightPane->addLayout(orderControlLayout);
}

// 3. Billing Tab
void MainWindow::setupBillingTab(QWidget* tab) {
  QHBoxLayout *layout = new QHBoxLayout(tab);
  QVBoxLayout *leftPane = new QVBoxLayout();
  layout->addLayout(leftPane, 2);

  leftPane->addWidget(new QLabel("Active Orders Ready for Billing:", tab));
  billingOrderCombo = new QComboBox(tab);
  leftPane->addWidget(billingOrderCombo);

  // Tax/discount selectors
  QGridLayout *calcGrid = new QGridLayout();
  calcGrid->addWidget(new QLabel("Tax Rate (%):", tab), 0, 0);
  taxSpinner = new QDoubleSpinBox(tab);
  taxSpinner->setRange(0.0, 50.0);
  taxSpinner->setValue(13.0);
  calcGrid->addWidget(taxSpinner, 0, 1);

  calcGrid->addWidget(new QLabel("Discount (%):", tab), 1, 0);
  discountSpinner = new QDoubleSpinBox(tab);
  discountSpinner->setRange(0.0, 100.0);
  discountSpinner->setValue(0.0);
  calcGrid->addWidget(discountSpinner, 1, 1);
  leftPane->addLayout(calcGrid);

  calculateButton = new QPushButton("Generate Receipt", tab);
  connect(calculateButton, &QPushButton::clicked, this, &MainWindow::onCalculateBillClicked);
  leftPane->addWidget(calculateButton);

  leftPane->addStretch();

  // Right pane shows Receipt View and Warnings
  QVBoxLayout *rightPane = new QVBoxLayout();
  layout->addLayout(rightPane, 3);

  // Feature 3: Red warning message when Bill exceeds $500
  largeBillWarningLabel = new QLabel(tab);
  largeBillWarningLabel->setStyleSheet("color: #ef4444; font-weight: bold; border: 2px solid #ef4444; border-radius: 8px; padding: 6px; background-color: #fef2f2;");
  largeBillWarningLabel->hide();
  rightPane->addWidget(largeBillWarningLabel);

  rightPane->addWidget(new QLabel("Bill Receipt:", tab));
  receiptDisplay = new QTextEdit(tab);
  receiptDisplay->setReadOnly(true);
  rightPane->addWidget(receiptDisplay);

  totalLabel = new QLabel("Total: $0.00", tab);
  totalLabel->setObjectName("totalLabel");
  totalLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  rightPane->addWidget(totalLabel);
}

// 4. Payments Tab
void MainWindow::setupPaymentsTab(QWidget* tab) {
  QHBoxLayout *layout = new QHBoxLayout(tab);
  QVBoxLayout *leftPane = new QVBoxLayout();
  layout->addLayout(leftPane, 2);

  leftPane->addWidget(new QLabel("Select Bill to Pay:", tab));
  paymentsBillCombo = new QComboBox(tab);
  leftPane->addWidget(paymentsBillCombo);

  leftPane->addWidget(new QLabel("Payment Method:", tab));
  paymentMethodCombo = new QComboBox(tab);
  paymentMethodCombo->addItem("Cash");
  paymentMethodCombo->addItem("Card");
  leftPane->addWidget(paymentMethodCombo);

  leftPane->addWidget(new QLabel("Payment Amount ($):", tab));
  paymentAmountSpinner = new QDoubleSpinBox(tab);
  paymentAmountSpinner->setRange(0.01, 100000.0);
  paymentAmountSpinner->setValue(10.0);
  leftPane->addWidget(paymentAmountSpinner);

  processPaymentButton = new QPushButton("Process Payment", tab);
  connect(processPaymentButton, &QPushButton::clicked, this, &MainWindow::onProcessPaymentClicked);
  leftPane->addWidget(processPaymentButton);

  leftPane->addStretch();

  QVBoxLayout *rightPane = new QVBoxLayout();
  layout->addLayout(rightPane, 3);
  rightPane->addWidget(new QLabel("Payment Confirmed details:", tab));
  paymentReceiptDisplay = new QTextEdit(tab);
  paymentReceiptDisplay->setReadOnly(true);
  rightPane->addWidget(paymentReceiptDisplay);
}

// 5. Analytics Tab
void MainWindow::setupAnalyticsTab(QWidget* tab) {
  QHBoxLayout *layout = new QHBoxLayout(tab);
  QVBoxLayout *leftPane = new QVBoxLayout();
  layout->addLayout(leftPane, 2);

  leftPane->addWidget(new QLabel("Financial Analytics summary:", tab));

  dailySalesLabel = new QLabel("Daily Sales Total: $0.00", tab);
  dailySalesLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #3b82f6;");
  leftPane->addWidget(dailySalesLabel);

  avgBillLabel = new QLabel("Average Bill Amount: $0.00", tab);
  avgBillLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #10b981;");
  leftPane->addWidget(avgBillLabel);

  leftPane->addWidget(new QLabel("Orders Count by Status:", tab));
  ordersOpenLabel = new QLabel("Open Orders: 0", tab);
  leftPane->addWidget(ordersOpenLabel);
  ordersPaidLabel = new QLabel("Paid Orders: 0", tab);
  leftPane->addWidget(ordersPaidLabel);
  ordersCancelledLabel = new QLabel("Cancelled Orders: 0", tab);
  leftPane->addWidget(ordersCancelledLabel);

  leftPane->addStretch();

  // Right pane displays popular items
  QVBoxLayout *rightPane = new QVBoxLayout();
  layout->addLayout(rightPane, 2);
  rightPane->addWidget(new QLabel("Menu Items Popularity List:", tab));
  popularItemsList = new QListWidget(tab);
  rightPane->addWidget(popularItemsList);
}

// Populate menu items on startup
void MainWindow::populateMenu() {
  QDir().mkpath("data");
  std::string err;
  if (!MenuFileIO::loadMenu("data/menu.txt", menuItems, &err)) {
    // Generate default template items
    menuItems.push_back(std::make_shared<FoodItem>("ITEM-0001", "Margherita Pizza", "Main Course", 12.99, "Italian", 12, false));
    menuItems.push_back(std::make_shared<FoodItem>("ITEM-0002", "Spicy Chicken Burger", "Main Course", 8.99, "American", 8, true));
    menuItems.push_back(std::make_shared<FoodItem>("ITEM-0003", "Bruschetta", "Appetizer", 6.50, "Italian", 5, false));
    menuItems.push_back(std::make_shared<FoodItem>("ITEM-0004", "Chocolate Lava Cake", "Dessert", 7.25, "French", 10, false));
    menuItems.push_back(std::make_shared<BeverageItem>("ITEM-0005", "Iced Latte", "Beverage", 4.50, 350.0, "Coffee", false));
    menuItems.push_back(std::make_shared<BeverageItem>("ITEM-0006", "Coca Cola", "Beverage", 2.50, 330.0, "Soft Drink", true));
    menuItems.push_back(std::make_shared<BeverageItem>("ITEM-0007", "Fresh Orange Juice", "Beverage", 5.00, 400.0, "Juice", false));

    MenuFileIO::saveMenu("data/menu.txt", menuItems);
  }

  manager.replaceMenuItems(menuItems);
  Order::menuRegistry = menuItems; // sync registry
  refreshMenuTable();
}

// Refresh Menu table in the GUI
void MainWindow::refreshMenuTable() {
  disconnect(menuTable, &QTableWidget::itemChanged, this, &MainWindow::onMenuItemChanged);

  // 1. Populate the Menu management table
  menuTable->setRowCount(menuItems.size());
  for (size_t i = 0; i < menuItems.size(); ++i) {
    auto item = menuItems[i];

    QTableWidgetItem *idItem = new QTableWidgetItem(QString::fromStdString(item->getItemID()));
    idItem->setFlags(idItem->flags() & ~Qt::ItemIsEditable);
    menuTable->setItem(i, 0, idItem);

    QTableWidgetItem *nameItem = new QTableWidgetItem(QString::fromStdString(item->getName()));
    nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
    menuTable->setItem(i, 1, nameItem);

    QTableWidgetItem *catItem = new QTableWidgetItem(QString::fromStdString(item->getCategory()));
    catItem->setFlags(catItem->flags() & ~Qt::ItemIsEditable);
    menuTable->setItem(i, 2, catItem);

    QTableWidgetItem *priceItem = new QTableWidgetItem(QString("$%1").arg(item->getPrice(), 0, 'f', 2));
    priceItem->setFlags(priceItem->flags() | Qt::ItemIsEditable);
    menuTable->setItem(i, 3, priceItem);

    QString details = "";
    if (auto food = std::dynamic_pointer_cast<FoodItem>(item)) {
      details = QString("Cuisine: %1 | Prep: %2 min").arg(QString::fromStdString(food->getCuisineType())).arg(food->getPreparationTime());
      if (food->getIsSpicy()) details += " | SPICY";
    } else if (auto bev = std::dynamic_pointer_cast<BeverageItem>(item)) {
      details = QString("Vol: %1 ml | Type: %2").arg(bev->getVolume()).arg(QString::fromStdString(bev->getBeverageType()));
      if (bev->getIsCarbonated()) details += " | CARB";
    }
    QTableWidgetItem *detailsItem = new QTableWidgetItem(details);
    detailsItem->setFlags(detailsItem->flags() & ~Qt::ItemIsEditable);
    menuTable->setItem(i, 4, detailsItem);
  }

  // 2. Populate the Orders Selection Menu table
  orderMenuTable->setRowCount(menuItems.size());
  for (size_t i = 0; i < menuItems.size(); ++i) {
    auto item = menuItems[i];

    QTableWidgetItem *idItem = new QTableWidgetItem(QString::fromStdString(item->getItemID()));
    idItem->setFlags(idItem->flags() & ~Qt::ItemIsEditable);
    orderMenuTable->setItem(i, 0, idItem);

    QTableWidgetItem *nameItem = new QTableWidgetItem(QString::fromStdString(item->getName()));
    nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
    orderMenuTable->setItem(i, 1, nameItem);

    QTableWidgetItem *catItem = new QTableWidgetItem(QString::fromStdString(item->getCategory()));
    catItem->setFlags(catItem->flags() & ~Qt::ItemIsEditable);
    orderMenuTable->setItem(i, 2, catItem);

    QTableWidgetItem *priceItem = new QTableWidgetItem(QString("$%1").arg(item->getPrice(), 0, 'f', 2));
    priceItem->setFlags(priceItem->flags() & ~Qt::ItemIsEditable); // Non-editable inside the ordering tab
    orderMenuTable->setItem(i, 3, priceItem);
  }

  connect(menuTable, &QTableWidget::itemChanged, this, &MainWindow::onMenuItemChanged);
}

// Handles searching and filtering the Menu list inside the Menu tab
void MainWindow::onSearchFilterChanged() {
  QString filterText = searchEdit->text().trimmed().toLower();
  QString filterCategory = categoryFilterCombo->currentData().toString();

  for (int r = 0; r < menuTable->rowCount(); ++r) {
    bool matchesName = menuTable->item(r, 1)->text().toLower().contains(filterText);
    bool matchesCategory = filterCategory.isEmpty() || (menuTable->item(r, 2)->text() == filterCategory);

    if (matchesName && matchesCategory) {
      menuTable->showRow(r);
    } else {
      menuTable->hideRow(r);
    }
  }
}

// Handles searching and filtering the Menu list inside the Orders tab
void MainWindow::onOrderSearchFilterChanged() {
  QString filterText = orderSearchEdit->text().trimmed().toLower();
  QString filterCategory = orderCategoryFilterCombo->currentData().toString();

  for (int r = 0; r < orderMenuTable->rowCount(); ++r) {
    bool matchesName = orderMenuTable->item(r, 1)->text().toLower().contains(filterText);
    bool matchesCategory = filterCategory.isEmpty() || (orderMenuTable->item(r, 2)->text() == filterCategory);

    if (matchesName && matchesCategory) {
      orderMenuTable->showRow(r);
    } else {
      orderMenuTable->hideRow(r);
    }
  }
}

// Add Item to active Dining Table order
void MainWindow::onAddItemClicked() {
  int selectedRow = orderMenuTable->currentRow();
  if (selectedRow < 0) {
    QMessageBox::warning(this, "No Selection", "Please select a menu item from the select menu list.");
    return;
  }

  std::string itemID = orderMenuTable->item(selectedRow, 0)->text().toStdString();
  std::shared_ptr<MenuItem> selectedItem = nullptr;
  for (const auto& item : menuItems) {
    if (item && item->getItemID() == itemID) {
      selectedItem = item;
      break;
    }
  }

  if (!selectedItem) return;
  int qty = quantitySpinner->value();

  // Validate limits (Maximum quantity 100 per item)
  if (qty <= 0 || qty > 100) {
    QMessageBox::warning(this, "Quantity Error", "Quantity must be between 1 and 100.");
    return;
  }

  auto table = manager.getTable(currentTableNumber);
  if (table && !table->hasActiveOrder()) {
    std::string orderID = "ORD-" + std::to_string(currentTableNumber) + "-" +
                          std::to_string(QDateTime::currentMSecsSinceEpoch());
    manager.openTableOrder(currentTableNumber, orderID);
  }

  manager.addItemToTable(currentTableNumber, selectedItem->getItemID(), qty);
  updateOrderTable();
  quantitySpinner->setValue(1);
}

// Remove item from active Dining Table order
void MainWindow::onRemoveItemClicked() {
  int selectedRow = orderTable->currentRow();
  if (selectedRow < 0) {
    QMessageBox::warning(this, "No Selection", "Please select an item from the order list to remove.");
    return;
  }

  auto table = manager.getTable(currentTableNumber);
  if (table && table->hasActiveOrder()) {
    auto order = table->getCurrentOrder();
    if (order && selectedRow < static_cast<int>(order->getItems().size())) {
      std::string itemID = order->getItems()[selectedRow].item->getItemID();
      manager.removeItemFromTable(currentTableNumber, itemID);
    }
  }
  updateOrderTable();
}

// Update Active Order panel display in Orders tab
void MainWindow::updateOrderTable() {
  auto table = manager.getTable(currentTableNumber);
  if (!table || !table->hasActiveOrder()) {
    orderTable->setRowCount(0);
    orderSubtotalLabel->setText("Subtotal: $0.00");
    return;
  }

  auto order = table->getCurrentOrder();
  const auto &items = order->getItems();

  orderTable->setRowCount(items.size());
  double tempSubtotal = 0.0;
  for (size_t i = 0; i < items.size(); ++i) {
    auto item = items[i].item;
    int qty = items[i].quantity;
    double subtotal = items[i].getSubtotal();
    tempSubtotal += subtotal;

    orderTable->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(item->getName())));
    orderTable->setItem(i, 1, new QTableWidgetItem(QString("$%1").arg(item->getPrice(), 0, 'f', 2)));
    orderTable->setItem(i, 2, new QTableWidgetItem(QString::number(qty)));
    orderTable->setItem(i, 3, new QTableWidgetItem(QString("$%1").arg(subtotal, 0, 'f', 2)));
  }

  orderSubtotalLabel->setText(QString("Subtotal: $%1").arg(tempSubtotal, 0, 'f', 2));
}

// Generate receipt and create a new bill
void MainWindow::onCalculateBillClicked() {
  int selectedIdx = billingOrderCombo->currentIndex();
  if (selectedIdx < 0) {
    QMessageBox::warning(this, "No Active Order", "Please select an active table order to bill.");
    return;
  }

  int tableNum = billingOrderCombo->itemData(selectedIdx).toInt();
  auto table = manager.getTable(tableNum);
  if (!table || !table->hasActiveOrder()) return;

  auto custOrder = table->getCurrentOrder();

  // Create and sync new abstract DineInOrder subclass object for data tracking
  std::string orderDate = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString();
  DineInOrder dineInOrder(custOrder->getOrderID(), tableNum, orderDate, "Open");
  for (const auto& line : custOrder->getItems()) {
    dineInOrder.addOrUpdateItem(line.item->getItemID(), line.quantity);
  }

  // Update dineInOrders collection
  bool orderExists = false;
  for (auto& existing : dineInOrders) {
    if (existing.getOrderID() == dineInOrder.getOrderID()) {
      existing = dineInOrder;
      orderExists = true;
      break;
    }
  }
  if (!orderExists) {
    dineInOrders.push_back(dineInOrder);
  }

  // Perform Calculations
  double subtotal = dineInOrder.computeTotal(); // Includes base prices + prep fees
  double taxRate = taxSpinner->value() / 100.0;
  double discountRate = discountSpinner->value() / 100.0;
  double discountAmount = subtotal * discountRate;
  double taxableAmount = subtotal - discountAmount;

  // Generate Bill Entity
  std::string billID = "BILL-" + std::to_string(restaurantBills.size() + 1001);
  Bill bill(billID, dineInOrder.getOrderID(), taxableAmount, taxRate);
  
  // Save bill locally
  bool billExists = false;
  for (auto& existing : restaurantBills) {
    if (existing.getBillID() == billID) {
      existing = bill;
      billExists = true;
      break;
    }
  }
  if (!billExists) {
    restaurantBills.push_back(bill);
  }

  // Render receipt layout
  receiptDisplay->setPlainText(QString::fromStdString(bill.displayBill()));
  totalLabel->setText(QString("Grand Total: $%1").arg(bill.getTotal(), 0, 'f', 2));

  // Feature 3: Show red warning label if Bill exceeds $500
  if (bill.getTotal() > 500.0) {
    largeBillWarningLabel->setText(QString("⚠️ Large Bill Warning: Total exceeds $500.00! ($%1)")
                                  .arg(bill.getTotal(), 0, 'f', 2));
    largeBillWarningLabel->show();
  } else {
    largeBillWarningLabel->hide();
  }

  unsavedChanges = true;
  refreshPaymentsBillCombo();
  updateAnalyticsDisplay();
}

// Clear table order entirely
void MainWindow::onClearBillClicked() {
  auto table = manager.getTable(currentTableNumber);
  if (table) {
    table->setOccupied(false);
  }
  updateOrderTable();
  receiptDisplay->clear();
  totalLabel->setText("Total: $0.00");
  discountSpinner->setValue(0.0);
}

// Switch tables
void MainWindow::onTableChanged(int index) {
  currentTableNumber = tableComboBox->itemData(index).toInt();
  updateOrderTable();
}

// Validate, create, and append a new menu item
void MainWindow::onAddMenuItemClicked() {
  AddMenuItemDialog dialog(this);
  if (dialog.exec() == QDialog::Accepted) {
    QString name = dialog.getName().trimmed();

    // Feature 1: Check Input Validation
    if (name.isEmpty() || name.length() < 2 || name.length() > 100) {
      QMessageBox::warning(this, "Validation Error", "Item name must be between 2 and 100 characters.");
      return;
    }

    bool hasAlpha = false;
    for (const auto& ch : name) {
      if (ch.isLetter()) { hasAlpha = true; break; }
    }
    if (!hasAlpha) {
      QMessageBox::warning(this, "Validation Error", "Item name must contain at least one alphabetic character.");
      return;
    }

    double price = dialog.getPrice();
    if (price <= 0.0 || price > 10000.0) {
      QMessageBox::warning(this, "Validation Error", "Price must be greater than zero and under $10,000.");
      return;
    }

    QString categoryType = dialog.getType();

    // Auto-generate ID
    int maxNum = 0;
    for (const auto &item : menuItems) {
      if (item) {
        std::string id = item->getItemID();
        if (id.length() == 9 && id.substr(0, 5) == "ITEM-") {
          try {
            int num = std::stoi(id.substr(5));
            if (num > maxNum) maxNum = num;
          } catch (...) {}
        }
      }
    }
    int nextNum = maxNum + 1;
    std::stringstream ss;
    ss << "ITEM-" << std::setfill('0') << std::setw(4) << nextNum;
    std::string newID = ss.str();

    // Feature 2: Verify duplicate menu item IDs
    bool duplicate = false;
    for (const auto& item : menuItems) {
      if (item && item->getItemID() == newID) { duplicate = true; break; }
    }
    if (duplicate) {
      QMessageBox::critical(this, "Duplicate Item ID", "Generated Item ID already exists.");
      return;
    }

    std::shared_ptr<MenuItem> newItem;
    if (categoryType == "FOOD") {
      newItem = std::make_shared<FoodItem>(newID, name.toStdString(), "Main Course",
                                           price, dialog.getCuisine().toStdString(),
                                           dialog.getPrepTime(), dialog.getIsSpicy());
    } else {
      newItem = std::make_shared<BeverageItem>(newID, name.toStdString(), "Beverage",
                                               price, dialog.getVolume(),
                                               dialog.getBeverageType().toStdString(),
                                               dialog.getIsCarbonated());
    }

    menuItems.push_back(newItem);
    manager.addMenuItem(newItem);
    Order::menuRegistry = menuItems; // sync registry

    // Save menu persistently
    std::string saveErr;
    if (!MenuFileIO::saveMenu("data/menu.txt", menuItems, &saveErr)) {
      QMessageBox::critical(this, "Save Error", QString::fromStdString(saveErr));
    }

    refreshMenuTable();
    QMessageBox::information(this, "Success", "Menu item added successfully!");
  }
}

// Delete Selected Menu item
void MainWindow::onDeleteMenuItemClicked() {
  int selectedRow = menuTable->currentRow();
  if (selectedRow < 0) {
    QMessageBox::warning(this, "No Selection", "Please select a menu item to delete.");
    return;
  }

  std::string itemID = menuItems[selectedRow]->getItemID();

  QMessageBox::StandardButton reply = QMessageBox::question(
      this, "Confirm Deletion",
      "Are you sure you want to delete this menu item?",
      QMessageBox::Yes | QMessageBox::No);

  if (reply == QMessageBox::Yes) {
    for (auto it = menuItems.begin(); it != menuItems.end(); ++it) {
      if ((*it)->getItemID() == itemID) {
        menuItems.erase(it);
        break;
      }
    }

    manager.removeMenuItem(itemID);
    Order::menuRegistry = menuItems; // sync registry

    std::string saveErr;
    if (!MenuFileIO::saveMenu("data/menu.txt", menuItems, &saveErr)) {
      QMessageBox::critical(this, "Save Error", QString::fromStdString(saveErr));
    }

    refreshMenuTable();
    QMessageBox::information(this, "Success", "Menu item deleted successfully!");
  }
}

// Edit menu price directly from table view
void MainWindow::onMenuItemChanged(QTableWidgetItem *item) {
  if (!item || item->column() != 3) return;

  int row = item->row();
  if (row < 0 || row >= static_cast<int>(menuItems.size())) return;

  QString txt = item->text().trimmed();
  if (txt.startsWith('$')) txt = txt.mid(1);

  bool ok = false;
  double newPrice = txt.toDouble(&ok);
  if (ok && newPrice > 0.0 && newPrice <= 10000.0) {
    menuItems[row]->setPrice(newPrice);
    Order::menuRegistry = menuItems; // sync registry

    std::string saveErr;
    if (!MenuFileIO::saveMenu("data/menu.txt", menuItems, &saveErr)) {
      QMessageBox::critical(this, "Save Error", QString::fromStdString(saveErr));
    }
  } else {
    QMessageBox::warning(this, "Invalid Price", "Price must be a valid positive number under $10,000.");
  }

  disconnect(menuTable, &QTableWidget::itemChanged, this, &MainWindow::onMenuItemChanged);
  item->setText(QString("$%1").arg(menuItems[row]->getPrice(), 0, 'f', 2));
  connect(menuTable, &QTableWidget::itemChanged, this, &MainWindow::onMenuItemChanged);
}

// Feature 6: Confirm and process order bill payment
void MainWindow::onProcessPaymentClicked() {
  int index = paymentsBillCombo->currentIndex();
  if (index < 0) {
    QMessageBox::warning(this, "No Selection", "Please select a bill to pay.");
    return;
  }

  std::string billID = paymentsBillCombo->itemData(index).toString().toStdString();
  Bill* selectedBill = nullptr;
  for (auto& b : restaurantBills) {
    if (b.getBillID() == billID) {
      selectedBill = &b;
      break;
    }
  }

  if (!selectedBill) return;

  double totalAmount = selectedBill->getTotal();
  double paymentAmount = paymentAmountSpinner->value();

  // Validate payment amount match
  if (std::abs(paymentAmount - totalAmount) > 0.01) {
    QMessageBox::warning(this, "Amount Mismatch",
                         QString("Payment amount ($%1) must match the bill total ($%2).")
                         .arg(paymentAmount, 0, 'f', 2).arg(totalAmount, 0, 'f', 2));
    return;
  }

  // Confirmation dialog
  QMessageBox::StandardButton reply = QMessageBox::question(
      this, "Confirm Payment",
      QString("Bill Total: $%1\nDo you want to process this payment?").arg(totalAmount, 0, 'f', 2),
      QMessageBox::Yes | QMessageBox::No);

  if (reply != QMessageBox::Yes) return;

  // Process payment
  std::string paymentID = "PAY-" + std::to_string(restaurantPayments.size() + 1001);
  std::string method = paymentMethodCombo->currentText().toStdString();

  Payment payment(paymentID, billID, method, paymentAmount);
  if (payment.processPayment()) {
    restaurantPayments.push_back(payment);

    // Update matching DineInOrder status to paid
    for (auto& order : dineInOrders) {
      if (order.getOrderID() == selectedBill->getOrderID()) {
        order.setStatus("Paid");
        break;
      }
    }

    // Also close the dining table order
    for (int i = 1; i <= 8; ++i) {
      auto table = manager.getTable(i);
      if (table && table->hasActiveOrder() && table->getCurrentOrder()->getOrderID() == selectedBill->getOrderID()) {
        table->setOccupied(false);
        break;
      }
    }

    paymentReceiptDisplay->setPlainText(QString::fromStdString(payment.displayPayment()));
    QMessageBox::information(this, "Success", "Payment processed successfully!");

    unsavedChanges = true;
    refreshPaymentsBillCombo();
    refreshBillingOrderCombo();
    updateOrderTable();
    updateAnalyticsDisplay();
  }
}

// Save transactions slot
void MainWindow::onSaveDataTriggered() {
  std::string err;
  QDir().mkpath("data");
  if (DataManager::saveData("data", dineInOrders, restaurantBills, restaurantPayments, &err)) {
    QMessageBox::information(this, "Saved", "Transaction records saved successfully!");
    unsavedChanges = false;
  } else {
    QMessageBox::critical(this, "Save Error", QString::fromStdString(err));
  }
}

// Load transactions slot
void MainWindow::onLoadDataTriggered() {
  std::string err;
  if (DataManager::loadData("data", dineInOrders, restaurantBills, restaurantPayments, &err)) {
    unsavedChanges = false;
    refreshBillingOrderCombo();
    refreshPaymentsBillCombo();
    updateAnalyticsDisplay();
    QMessageBox::information(this, "Loaded", "Transaction records loaded successfully!");
  } else {
    QMessageBox::critical(this, "Load Error", QString::fromStdString(err));
  }
}

// Refresh Billing combo boxes
void MainWindow::refreshBillingOrderCombo() {
  billingOrderCombo->clear();
  for (int i = 1; i <= 8; ++i) {
    auto table = manager.getTable(i);
    if (table && table->hasActiveOrder() && !table->getCurrentOrder()->isEmpty()) {
      billingOrderCombo->addItem(QString("Table %1 - %2")
                                 .arg(i)
                                 .arg(QString::fromStdString(table->getCurrentOrder()->getOrderID())),
                                 i);
    }
  }
}

// Refresh Payments combo box
void MainWindow::refreshPaymentsBillCombo() {
  paymentsBillCombo->clear();
  for (const auto& bill : restaurantBills) {
    std::string status = "Open";
    for (const auto& order : dineInOrders) {
      if (order.getOrderID() == bill.getOrderID()) {
        status = order.getStatus();
        break;
      }
    }
    // Only display unpaid bills
    if (status == "Open") {
      paymentsBillCombo->addItem(QString("%1 (Total: $%2)")
                                 .arg(QString::fromStdString(bill.getBillID()))
                                 .arg(bill.getTotal(), 0, 'f', 2),
                                 QString::fromStdString(bill.getBillID()));
    }
  }
}

// Refresh and update Analytics panel
void MainWindow::updateAnalyticsDisplay() {
  AnalyticsEngine<Bill> billEngine;
  AnalyticsEngine<DineInOrder> orderEngine;

  double totalSales = billEngine.computeDailySales(restaurantBills);
  double avgBill = billEngine.computeAverageBillAmount(restaurantBills);

  int openCount = orderEngine.computeOrdersByStatus(dineInOrders, "Open");
  int paidCount = orderEngine.computeOrdersByStatus(dineInOrders, "Paid");
  int cancelledCount = orderEngine.computeOrdersByStatus(dineInOrders, "Cancelled");

  dailySalesLabel->setText(QString("Daily Sales Total: $%1").arg(totalSales, 0, 'f', 2));
  avgBillLabel->setText(QString("Average Bill Amount: $%1").arg(avgBill, 0, 'f', 2));

  ordersOpenLabel->setText(QString("Open Orders: %1").arg(openCount));
  ordersPaidLabel->setText(QString("Paid Orders: %1").arg(paidCount));
  ordersCancelledLabel->setText(QString("Cancelled Orders: %1").arg(cancelledCount));

  popularItemsList->clear();
  auto popularity = orderEngine.computePopularItems(dineInOrders);
  std::vector<std::pair<std::string, int>> popularVec(popularity.begin(), popularity.end());
  std::sort(popularVec.begin(), popularVec.end(), [](const auto& a, const auto& b) {
    return a.second > b.second;
  });

  for (const auto& pair : popularVec) {
    std::string itemName = pair.first;
    for (const auto& item : menuItems) {
      if (item && item->getItemID() == pair.first) {
        itemName = item->getName();
        break;
      }
    }
    popularItemsList->addItem(QString("%1: %2 sold").arg(QString::fromStdString(itemName)).arg(pair.second));
  }
}

// Menu bar triggers for import/export
void MainWindow::onImportMenuTriggered() {
  QString fileName = QFileDialog::getOpenFileName(
      this, "Import Menu File", "", "Text Files (*.txt);;All Files (*)");
  if (fileName.isEmpty()) return;

  std::string err;
  std::vector<std::shared_ptr<MenuItem>> newItems;
  if (MenuFileIO::loadMenu(fileName.toStdString(), newItems, &err)) {
    menuItems = std::move(newItems);
    manager.replaceMenuItems(menuItems);
    Order::menuRegistry = menuItems; // sync registry
    refreshMenuTable();
    QMessageBox::information(this, "Success", "Menu imported successfully!");
  } else {
    QMessageBox::critical(this, "Import Error", QString::fromStdString(err));
  }
}

void MainWindow::onExportMenuTriggered() {
  QString fileName = QFileDialog::getSaveFileName(
      this, "Export Menu File", "menu_export.txt", "Text Files (*.txt);;All Files (*)");
  if (fileName.isEmpty()) return;

  std::string err;
  if (MenuFileIO::saveMenu(fileName.toStdString(), menuItems, &err)) {
    QMessageBox::information(this, "Success", "Menu exported successfully!");
  } else {
    QMessageBox::critical(this, "Export Error", QString::fromStdString(err));
  }
}

void MainWindow::onImportReceiptTriggered() {
  QString fileName = QFileDialog::getOpenFileName(
      this, "Import Receipt File", "", "Text Files (*.txt);;All Files (*)");
  if (fileName.isEmpty()) return;

  std::ifstream file(fileName.toStdString());
  if (!file) {
    QMessageBox::critical(this, "Error", "Could not open receipt file.");
    return;
  }

  std::stringstream ss;
  ss << file.rdbuf();
  receiptDisplay->setPlainText(QString::fromStdString(ss.str()));
  QMessageBox::information(this, "Success", "Receipt loaded successfully!");
}

void MainWindow::onExportReceiptTriggered() {
  QString receiptText = receiptDisplay->toPlainText();
  if (receiptText.isEmpty()) {
    QMessageBox::warning(this, "Empty Receipt", "There is no receipt to export.");
    return;
  }

  QString fileName = QFileDialog::getSaveFileName(
      this, "Export Receipt File", "receipt.txt", "Text Files (*.txt);;All Files (*)");
  if (fileName.isEmpty()) return;

  std::ofstream file(fileName.toStdString());
  if (!file) {
    QMessageBox::critical(this, "Error", "Could not write to receipt file.");
    return;
  }

  file << receiptText.toStdString();
  QMessageBox::information(this, "Success", "Receipt exported successfully!");
}

void MainWindow::onDarkThemeTriggered() { applyTheme(true); }
void MainWindow::onLightThemeTriggered() { applyTheme(false); }

void MainWindow::onPlusClicked() {
  int val = quantitySpinner->value();
  if (val < 100) quantitySpinner->setValue(val + 1);
}

void MainWindow::onMinusClicked() {
  int val = quantitySpinner->value();
  if (val > 1) quantitySpinner->setValue(val - 1);
}

// Application theme manager
void MainWindow::applyTheme(bool dark) {
  if (dark) {
    this->setStyleSheet(
        "QMainWindow { background-color: #0f172a; }"
        "QWidget { color: #f8fafc; font-family: 'Segoe UI', Arial, sans-serif; font-size: 14px; }"
        "QMenuBar { background-color: #1e293b; color: #f8fafc; border-bottom: 1px solid #334155; }"
        "QMenuBar::item { background-color: transparent; padding: 6px 12px; }"
        "QMenuBar::item:selected { background-color: #334155; color: #60a5fa; border-radius: 4px; }"
        "QMenu { background-color: #1e293b; color: #f8fafc; border: 1px solid #334155; border-radius: 6px; padding: 4px; }"
        "QMenu::item:selected { background-color: #334155; color: #60a5fa; border-radius: 4px; }"
        "QTabWidget::pane { border: 1px solid #334155; border-radius: 12px; background-color: #1e293b; padding: 10px; }"
        "QTabBar::tab { background-color: #1e293b; color: #94a3b8; border: 1px solid #334155; border-bottom: none; border-top-left-radius: 6px; border-top-right-radius: 6px; padding: 8px 16px; margin-right: 2px; }"
        "QTabBar::tab:selected { background-color: #2563eb; color: white; border-color: #2563eb; }"
        "QTableWidget { background-color: #1e293b; border: 1px solid #334155; border-radius: 12px; gridline-color: #334155; selection-background-color: #2563eb; selection-color: white; }"
        "QTableWidget::item { padding: 6px; }"
        "QHeaderView { background-color: #1e293b; }"
        "QHeaderView::section { background-color: #1e293b; color: #60a5fa; padding: 8px; font-weight: bold; border: 1px solid #334155; }"
        "QHeaderView::section:horizontal:first { border-top-left-radius: 11px; }"
        "QHeaderView::section:horizontal:last { border-top-right-radius: 11px; }"
        "QTextEdit { background-color: #1e293b; border: 1px solid #334155; border-radius: 12px; color: #cbd5e1; font-family: 'Courier New', monospace; font-size: 13px; padding: 8px; }"
        "QPushButton { background-color: #2563eb; color: white; border: none; padding: 9px 18px; border-radius: 6px; font-weight: bold; }"
        "QPushButton:hover { background-color: #3b82f6; }"
        "QPushButton:pressed { background-color: #1d4ed8; }"
        "QPushButton#spinnerBtn { background-color: #1e293b; border: 1px solid #334155; color: #60a5fa; font-size: 8px; padding: 0px; border-radius: 2px; }"
        "QPushButton#spinnerBtn:hover { background-color: #334155; }"
        "QPushButton#spinnerBtn:pressed { background-color: #2563eb; color: white; }"
        "QComboBox { background-color: #1e293b; border: 1px solid #334155; border-radius: 8px; padding: 6px 12px; color: #f8fafc; min-height: 20px; }"
        "QComboBox::drop-down { subcontrol-origin: padding; subcontrol-position: top right; width: 25px; border: none; border-top-right-radius: 8px; border-bottom-right-radius: 8px; }"
        "QComboBox QAbstractItemView { background-color: #1e293b; border: 1px solid #334155; selection-background-color: #334155; selection-color: #60a5fa; color: #f8fafc; }"
        "QSpinBox, QDoubleSpinBox { background-color: #1e293b; border: 1px solid #334155; border-radius: 8px; padding: 6px 12px; color: #f8fafc; }"
        "QSpinBox::up-button, QSpinBox::down-button, QDoubleSpinBox::up-button, QDoubleSpinBox::down-button { width: 0px; height: 0px; border: none; }"
        "QListWidget { background-color: #1e293b; border: 1px solid #334155; border-radius: 12px; color: #cbd5e1; padding: 8px; }"
        "QLabel { font-weight: 500; }"
        "QWidget#headerWidget { background-color: #1e293b; border: 1px solid #334155; border-left: 5px solid #60a5fa; border-radius: 12px; }"
        "QLabel#titleLabel { font-size: 22px; font-weight: bold; color: #60a5fa; }"
        "QLabel#subtitleLabel { font-size: 11px; color: #94a3b8; }"
        "QLabel#totalLabel { font-size: 20px; font-weight: bold; color: #10b981; }"
    );
  } else {
    this->setStyleSheet(
        "QMainWindow { background-color: #f8fafc; }"
        "QWidget { color: #0f172a; font-family: 'Segoe UI', Arial, sans-serif; font-size: 14px; }"
        "QMenuBar { background-color: #ffffff; color: #0f172a; border-bottom: 1px solid #e2e8f0; }"
        "QMenuBar::item { background-color: transparent; padding: 6px 12px; }"
        "QMenuBar::item:selected { background-color: #f1f5f9; color: #2563eb; border-radius: 4px; }"
        "QMenu { background-color: #ffffff; color: #0f172a; border: 1px solid #e2e8f0; border-radius: 6px; padding: 4px; }"
        "QMenu::item:selected { background-color: #f1f5f9; color: #2563eb; border-radius: 4px; }"
        "QTabWidget::pane { border: 1px solid #e2e8f0; border-radius: 12px; background-color: #ffffff; padding: 10px; }"
        "QTabBar::tab { background-color: #f1f5f9; color: #64748b; border: 1px solid #e2e8f0; border-bottom: none; border-top-left-radius: 6px; border-top-right-radius: 6px; padding: 8px 16px; margin-right: 2px; }"
        "QTabBar::tab:selected { background-color: #2563eb; color: white; border-color: #2563eb; }"
        "QTableWidget { background-color: #ffffff; border: 1px solid #e2e8f0; border-radius: 12px; gridline-color: #f1f5f9; selection-background-color: #dbeafe; selection-color: #1e40af; }"
        "QTableWidget::item { padding: 6px; }"
        "QHeaderView { background-color: #ffffff; }"
        "QHeaderView::section { background-color: #f1f5f9; color: #1e40af; padding: 8px; font-weight: bold; border: 1px solid #e2e8f0; }"
        "QHeaderView::section:horizontal:first { border-top-left-radius: 11px; }"
        "QHeaderView::section:horizontal:last { border-top-right-radius: 11px; }"
        "QTextEdit { background-color: #ffffff; border: 1px solid #e2e8f0; border-radius: 12px; color: #334155; font-family: 'Courier New', monospace; font-size: 13px; padding: 8px; }"
        "QPushButton { background-color: #2563eb; color: white; border: none; padding: 9px 18px; border-radius: 6px; font-weight: bold; }"
        "QPushButton:hover { background-color: #3b82f6; }"
        "QPushButton:pressed { background-color: #1d4ed8; }"
        "QPushButton#spinnerBtn { background-color: #ffffff; border: 1px solid #e2e8f0; color: #2563eb; font-size: 8px; padding: 0px; border-radius: 2px; }"
        "QPushButton#spinnerBtn:hover { background-color: #f1f5f9; }"
        "QPushButton#spinnerBtn:pressed { background-color: #1d4ed8; color: white; }"
        "QComboBox { background-color: #ffffff; border: 1px solid #e2e8f0; border-radius: 8px; padding: 6px 12px; color: #0f172a; min-height: 20px; }"
        "QComboBox::drop-down { subcontrol-origin: padding; subcontrol-position: top right; width: 25px; border: none; border-top-right-radius: 8px; border-bottom-right-radius: 8px; }"
        "QComboBox QAbstractItemView { background-color: #ffffff; border: 1px solid #e2e8f0; selection-background-color: #f1f5f9; selection-color: #2563eb; color: #0f172a; }"
        "QSpinBox, QDoubleSpinBox { background-color: #ffffff; border: 1px solid #e2e8f0; border-radius: 8px; padding: 6px 12px; color: #0f172a; }"
        "QSpinBox::up-button, QSpinBox::down-button, QDoubleSpinBox::up-button, QDoubleSpinBox::down-button { width: 0px; height: 0px; border: none; }"
        "QListWidget { background-color: #ffffff; border: 1px solid #e2e8f0; border-radius: 12px; color: #334155; padding: 8px; }"
        "QLabel { font-weight: 500; }"
        "QWidget#headerWidget { background-color: #ffffff; border: 1px solid #e2e8f0; border-left: 5px solid #2563eb; border-radius: 12px; }"
        "QLabel#titleLabel { font-size: 22px; font-weight: bold; color: #2563eb; }"
        "QLabel#subtitleLabel { font-size: 11px; color: #64748b; }"
        "QLabel#totalLabel { font-size: 20px; font-weight: bold; color: #059669; }"
    );
  }
}

#include "MainWindow.h"
#include "FoodItem.h"
#include "BeverageItem.h"
#include "MenuFileIO.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QDateTime>
#include <sstream>
#include <fstream>
#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QActionGroup>
#include <QFileDialog>
#include <QDir>

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
        priceSpinner->setRange(0.0, 1000.0);
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
        prepTimeSpinner->setRange(0, 180);
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
        volumeSpinner->setRange(0.0, 5000.0);
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

        connect(typeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index) {
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
            "QLineEdit, QSpinBox, QDoubleSpinBox, QComboBox { background-color: #262635; border: 1px solid #3f3f5a; border-radius: 5px; padding: 4px; color: white; }"
            "QPushButton { background-color: #3182ce; color: white; border: none; padding: 6px 12px; border-radius: 5px; font-weight: bold; }"
            "QPushButton:hover { background-color: #4299e1; }"
            "QCheckBox { color: #e2e8f0; }"
        );
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

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), currentTableNumber(1) {
    for (int i = 1; i <= 8; ++i) {
        manager.addTable(i);
    }
    setupUI();
    populateMenu();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI() {
    // Set window properties
    setWindowTitle("BiteKing - Restaurant Billing & Ordering System");
    resize(1000, 700);

    // Apply premium dark mode styles
    // Apply default Dark Theme
    applyTheme(true);

    // Menu Bar Setup
    QMenuBar *mBar = this->menuBar();
    QMenu *fileMenu = mBar->addMenu("&File");

    QAction *importMenuAct = new QAction("&Import Menu...", this);
    connect(importMenuAct, &QAction::triggered, this, &MainWindow::onImportMenuTriggered);
    fileMenu->addAction(importMenuAct);

    QAction *exportMenuAct = new QAction("&Export Menu...", this);
    connect(exportMenuAct, &QAction::triggered, this, &MainWindow::onExportMenuTriggered);
    fileMenu->addAction(exportMenuAct);

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

    // Main layout structures
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setObjectName("centralWidget");
    setCentralWidget(centralWidget);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // Title Banner
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
    QLabel *titleLabel = new QLabel("BiteKing OS", centralWidget);
    titleLabel->setObjectName("titleLabel");
    QLabel *subtitleLabel = new QLabel("Smart Restaurant Billing & Ordering System", centralWidget);
    subtitleLabel->setObjectName("subtitleLabel");
    titleTextLayout->addWidget(titleLabel);
    titleTextLayout->addWidget(subtitleLabel);
    headerLayout->addLayout(titleTextLayout);

    headerLayout->addStretch();
    mainLayout->addWidget(headerWidget);

    // Horizonal split: Left is Menu & Order, Right is Receipt
    QHBoxLayout *splitLayout = new QHBoxLayout();
    mainLayout->addLayout(splitLayout);

    // Left pane
    QVBoxLayout *leftPane = new QVBoxLayout();
    splitLayout->addLayout(leftPane, 3); // 3/5 ratio

    // Table Selector
    QHBoxLayout *tableSelectLayout = new QHBoxLayout();
    tableSelectLayout->addWidget(new QLabel("Select Dining Table:", this));
    tableComboBox = new QComboBox(this);
    for (int i = 1; i <= 8; ++i) {
        tableComboBox->addItem(QString("Table %1").arg(i), i);
    }
    connect(tableComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onTableChanged);
    tableSelectLayout->addWidget(tableComboBox);
    leftPane->addLayout(tableSelectLayout);

    // 1. Menu Section
    leftPane->addWidget(new QLabel("Menu Items:", this));
    menuTable = new QTableWidget(this);
    menuTable->setColumnCount(5);
    menuTable->setHorizontalHeaderLabels({"ID", "Name", "Category", "Price", "Details"});
    menuTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    menuTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    menuTable->setSelectionMode(QAbstractItemView::SingleSelection);
    leftPane->addWidget(menuTable);

    // Control bar for adding items
    QHBoxLayout *addControlLayout = new QHBoxLayout();
    addControlLayout->addWidget(new QLabel("Quantity:", this));
    quantitySpinner = new QSpinBox(this);
    quantitySpinner->setRange(1, 100);
    quantitySpinner->setValue(1);
    addControlLayout->addWidget(quantitySpinner);

    addButton = new QPushButton("Add to Order", this);
    connect(addButton, &QPushButton::clicked, this, &MainWindow::onAddItemClicked);
    addControlLayout->addWidget(addButton);

    leftPane->addLayout(addControlLayout);

    // 2. Order Section
    leftPane->addWidget(new QLabel("Current Order:", this));
    orderTable = new QTableWidget(this);
    orderTable->setColumnCount(4);
    orderTable->setHorizontalHeaderLabels({"Name", "Price", "Qty", "Subtotal"});
    orderTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    orderTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    orderTable->setSelectionMode(QAbstractItemView::SingleSelection);
    leftPane->addWidget(orderTable);

    // Control bar for removing items
    QHBoxLayout *orderControlLayout = new QHBoxLayout();
    removeButton = new QPushButton("Remove Selected", this);
    removeButton->setObjectName("clearBtn"); // red button
    connect(removeButton, &QPushButton::clicked, this, &MainWindow::onRemoveItemClicked);
    orderControlLayout->addWidget(removeButton);
    leftPane->addLayout(orderControlLayout);

    // Right pane: Receipt & Calculation Controls
    QVBoxLayout *rightPane = new QVBoxLayout();
    splitLayout->addLayout(rightPane, 2); // 2/5 ratio

    rightPane->addWidget(new QLabel("Tax & Discount:", this));
    
    QGridLayout *calcGrid = new QGridLayout();
    calcGrid->addWidget(new QLabel("Tax Rate (%):", this), 0, 0);
    taxSpinner = new QDoubleSpinBox(this);
    taxSpinner->setRange(0.0, 50.0);
    taxSpinner->setValue(13.0); // 13% default tax
    calcGrid->addWidget(taxSpinner, 0, 1);

    calcGrid->addWidget(new QLabel("Discount (%):", this), 1, 0);
    discountSpinner = new QDoubleSpinBox(this);
    discountSpinner->setRange(0.0, 100.0);
    discountSpinner->setValue(0.0);
    calcGrid->addWidget(discountSpinner, 1, 1);
    rightPane->addLayout(calcGrid);

    // Buttons
    QHBoxLayout *actionBtnLayout = new QHBoxLayout();
    calculateButton = new QPushButton("Generate Receipt", this);
    connect(calculateButton, &QPushButton::clicked, this, &MainWindow::onCalculateBillClicked);
    actionBtnLayout->addWidget(calculateButton);

    clearButton = new QPushButton("Clear All", this);
    clearButton->setObjectName("clearBtn");
    connect(clearButton, &QPushButton::clicked, this, &MainWindow::onClearBillClicked);
    actionBtnLayout->addWidget(clearButton);
    rightPane->addLayout(actionBtnLayout);

    // Receipt View
    rightPane->addWidget(new QLabel("Invoice/Receipt:", this));
    receiptDisplay = new QTextEdit(this);
    receiptDisplay->setReadOnly(true);
    rightPane->addWidget(receiptDisplay);

    totalLabel = new QLabel("Total: $0.00", this);
    totalLabel->setObjectName("totalLabel");
    totalLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    rightPane->addWidget(totalLabel);
}

void MainWindow::populateMenu() {
    QDir().mkpath("data"); // Ensure the data directory exists
    std::string err;
    if (!MenuFileIO::loadMenu("data/menu.txt", menuItems, &err)) {
        // File not found or failed to load. Create default menu items:
        menuItems.push_back(std::make_shared<FoodItem>("ITEM-0001", "Margherita Pizza", "Main Course", 12.99, "Italian", 12, false));
        menuItems.push_back(std::make_shared<FoodItem>("ITEM-0002", "Spicy Chicken Burger", "Main Course", 8.99, "American", 8, true));
        menuItems.push_back(std::make_shared<FoodItem>("ITEM-0003", "Bruschetta", "Appetizer", 6.50, "Italian", 5, false));
        menuItems.push_back(std::make_shared<FoodItem>("ITEM-0004", "Chocolate Lava Cake", "Dessert", 7.25, "French", 10, false));
        menuItems.push_back(std::make_shared<BeverageItem>("ITEM-0005", "Iced Latte", "Beverage", 4.50, 350.0, "Coffee", false));
        menuItems.push_back(std::make_shared<BeverageItem>("ITEM-0006", "Coca Cola", "Beverage", 2.50, 330.0, "Soft Drink", true));
        menuItems.push_back(std::make_shared<BeverageItem>("ITEM-0007", "Fresh Orange Juice", "Beverage", 5.00, 400.0, "Juice", false));

        // Save it so we have a template for future loads
        MenuFileIO::saveMenu("data/menu.txt", menuItems);
    }

    // Register these items in RestaurantManager
    manager.replaceMenuItems(menuItems);

    refreshMenuTable();
}

void MainWindow::refreshMenuTable() {
    // Disconnect signal during bulk population to prevent recursive triggers
    disconnect(menuTable, &QTableWidget::itemChanged, this, &MainWindow::onMenuItemChanged);

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
        if (auto foodItem = std::dynamic_pointer_cast<FoodItem>(item)) {
            details = QString("%1 | Prep: %2m | Spicy: %3")
                      .arg(QString::fromStdString(foodItem->getCuisineType()))
                      .arg(foodItem->getPreparationTime())
                      .arg(foodItem->getIsSpicy() ? "Yes" : "No");
        } else if (auto bevItem = std::dynamic_pointer_cast<BeverageItem>(item)) {
            details = QString("%1ml | %2 | Carb: %3")
                      .arg(bevItem->getVolume())
                      .arg(QString::fromStdString(bevItem->getBeverageType()))
                      .arg(bevItem->getIsCarbonated() ? "Yes" : "No");
        }
        
        QTableWidgetItem *detailsItem = new QTableWidgetItem(details);
        detailsItem->setFlags(detailsItem->flags() & ~Qt::ItemIsEditable);
        menuTable->setItem(i, 4, detailsItem);
    }

    // Re-connect the signal
    connect(menuTable, &QTableWidget::itemChanged, this, &MainWindow::onMenuItemChanged);
}

void MainWindow::onAddItemClicked() {
    int selectedRow = menuTable->currentRow();
    if (selectedRow < 0) {
        QMessageBox::warning(this, "No Selection", "Please select a menu item from the list.");
        return;
    }

    auto selectedItem = menuItems[selectedRow];
    int qty = quantitySpinner->value();

    auto table = manager.getTable(currentTableNumber);
    if (table && !table->hasActiveOrder()) {
        std::string orderID = "ORD-" + std::to_string(currentTableNumber) + "-" + std::to_string(QDateTime::currentMSecsSinceEpoch());
        manager.openTableOrder(currentTableNumber, orderID);
    }

    manager.addItemToTable(currentTableNumber, selectedItem->getItemID(), qty);

    updateOrderTable();
    quantitySpinner->setValue(1); // reset spinner
}

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

void MainWindow::updateOrderTable() {
    auto table = manager.getTable(currentTableNumber);
    if (!table || !table->hasActiveOrder()) {
        orderTable->setRowCount(0);
        totalLabel->setText("Total: $0.00");
        return;
    }

    auto order = table->getCurrentOrder();
    const auto& items = order->getItems();
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

    totalLabel->setText(QString("Subtotal: $%1").arg(tempSubtotal, 0, 'f', 2));
}

void MainWindow::onCalculateBillClicked() {
    auto table = manager.getTable(currentTableNumber);
    if (!table || !table->hasActiveOrder() || table->getCurrentOrder()->isEmpty()) {
        QMessageBox::warning(this, "Empty Order", "No items added to the order yet.");
        return;
    }

    auto order = table->getCurrentOrder();
    const auto& items = order->getItems();

    double subtotal = 0.0;
    double totalPrepFee = 0.0;

    std::stringstream ss;
    ss << "========================================\n";
    ss << "  RESTAURANT BILLING & ORDERING SYSTEM  \n";
    ss << "            INVOICE / RECEIPT           \n";
    ss << "========================================\n";
    ss << "Table ID: " << currentTableNumber << "\n";
    ss << "Order ID: " << order->getOrderID() << "\n";
    ss << "Date/Time: " << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString() << "\n";
    ss << "----------------------------------------\n";
    ss << std::left << std::setw(20) << "Item Name" 
       << std::setw(8) << "Price" 
       << std::setw(5) << "Qty" 
       << "Subtotal\n";
    ss << "----------------------------------------\n";

    for (const auto &line : items) {
        auto item = line.item;
        int qty = line.quantity;
        double price = item->getPrice();
        double itemSubtotal = line.getSubtotal();
        subtotal += itemSubtotal;

        ss << std::left << std::setw(20) << (item->getName().length() > 18 ? item->getName().substr(0, 17) + "..." : item->getName())
           << "$" << std::setw(7) << std::fixed << std::setprecision(2) << price
           << std::setw(5) << qty
           << "$" << itemSubtotal << "\n";

        // Add special fees/calculations if applicable
        if (auto foodItem = std::dynamic_pointer_cast<FoodItem>(item)) {
            double prepFee = foodItem->calculatePreparationFee() * qty;
            totalPrepFee += prepFee;
        }
    }

    ss << "----------------------------------------\n";

    double taxRate = taxSpinner->value() / 100.0;
    double discountRate = discountSpinner->value() / 100.0;

    // Calculations
    double discountAmount = subtotal * discountRate;
    double taxableAmount = subtotal - discountAmount + totalPrepFee;
    double taxAmount = taxableAmount * taxRate;
    double grandTotal = taxableAmount + taxAmount;

    ss << std::left << std::setw(30) << "Subtotal:" << "$" << subtotal << "\n";
    if (discountAmount > 0) {
        ss << std::left << std::setw(30) << "Discount (" + std::to_string((int)discountSpinner->value()) + "%):" << "-$" << discountAmount << "\n";
    }
    if (totalPrepFee > 0) {
        ss << std::left << std::setw(30) << "Preparation Fees:" << "$" << totalPrepFee << "\n";
    }
    ss << std::left << std::setw(30) << "Tax (" + std::to_string((int)taxSpinner->value()) + "%):" << "$" << taxAmount << "\n";
    ss << "========================================\n";
    ss << std::left << std::setw(30) << "GRAND TOTAL:" << "$" << grandTotal << "\n";
    ss << "========================================\n";
    ss << "       Thank you for dining with us!    \n";

    receiptDisplay->setPlainText(QString::fromStdString(ss.str()));
    totalLabel->setText(QString("Grand Total: $%1").arg(grandTotal, 0, 'f', 2));
}

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

void MainWindow::onTableChanged(int index) {
    currentTableNumber = tableComboBox->itemData(index).toInt();
    updateOrderTable();
    
    // Check if the switched table has an active order
    auto table = manager.getTable(currentTableNumber);
    if (table && table->hasActiveOrder() && !table->getCurrentOrder()->isEmpty()) {
        onCalculateBillClicked();
    } else {
        receiptDisplay->clear();
        totalLabel->setText("Total: $0.00");
    }
}

void MainWindow::onAddMenuItemClicked() {
    AddMenuItemDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QString name = dialog.getName().trimmed();
        if (name.isEmpty()) {
            QMessageBox::warning(this, "Input Error", "Item name cannot be empty.");
            return;
        }
        double price = dialog.getPrice();
        QString categoryType = dialog.getType();

        // Calculate the next ID (ITEM-XXXX format)
        int maxNum = 0;
        for (const auto& item : menuItems) {
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

        std::shared_ptr<MenuItem> newItem;
        if (categoryType == "FOOD") {
            newItem = std::make_shared<FoodItem>(
                newID, name.toStdString(), "Main Course", price,
                dialog.getCuisine().toStdString(),
                dialog.getPrepTime(),
                dialog.getIsSpicy()
            );
        } else {
            newItem = std::make_shared<BeverageItem>(
                newID, name.toStdString(), "Beverage", price,
                dialog.getVolume(),
                dialog.getBeverageType().toStdString(),
                dialog.getIsCarbonated()
            );
        }

        // Add to collections
        menuItems.push_back(newItem);
        manager.addMenuItem(newItem);

        // Save menu persistently
        std::string saveErr;
        if (!MenuFileIO::saveMenu("data/menu.txt", menuItems, &saveErr)) {
            QMessageBox::critical(this, "Save Error", QString::fromStdString(saveErr));
        }

        // Refresh UI table
        refreshMenuTable();
    }
}

void MainWindow::onImportMenuTriggered() {
    QString fileName = QFileDialog::getOpenFileName(this, "Import Menu File", "", "Text Files (*.txt);;All Files (*)");
    if (fileName.isEmpty()) return;

    std::string err;
    std::vector<std::shared_ptr<MenuItem>> newItems;
    if (MenuFileIO::loadMenu(fileName.toStdString(), newItems, &err)) {
        menuItems = std::move(newItems);
        manager.replaceMenuItems(menuItems);
        refreshMenuTable();
        QMessageBox::information(this, "Success", "Menu imported successfully!");
    } else {
        QMessageBox::critical(this, "Import Error", QString::fromStdString(err));
    }
}

void MainWindow::onExportMenuTriggered() {
    QString fileName = QFileDialog::getSaveFileName(this, "Export Menu File", "menu_export.txt", "Text Files (*.txt);;All Files (*)");
    if (fileName.isEmpty()) return;

    std::string err;
    if (MenuFileIO::saveMenu(fileName.toStdString(), menuItems, &err)) {
        QMessageBox::information(this, "Success", "Menu exported successfully!");
    } else {
        QMessageBox::critical(this, "Export Error", QString::fromStdString(err));
    }
}

void MainWindow::onImportReceiptTriggered() {
    QString fileName = QFileDialog::getOpenFileName(this, "Import Receipt File", "", "Text Files (*.txt);;All Files (*)");
    if (fileName.isEmpty()) return;

    std::ifstream file(fileName.toStdString());
    if (!file) {
        QMessageBox::critical(this, "Error", "Could not open receipt file for reading.");
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

    QString fileName = QFileDialog::getSaveFileName(this, "Export Receipt File", "receipt.txt", "Text Files (*.txt);;All Files (*)");
    if (fileName.isEmpty()) return;

    std::ofstream file(fileName.toStdString());
    if (!file) {
        QMessageBox::critical(this, "Error", "Could not open file for writing.");
        return;
    }

    file << receiptText.toStdString();
    QMessageBox::information(this, "Success", "Receipt exported successfully!");
}

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
            
            // Table styling
            "QTableWidget { background-color: #1e293b; border: 1px solid #334155; border-radius: 8px; gridline-color: #334155; selection-background-color: #2563eb; selection-color: white; }"
            "QTableWidget::item { padding: 6px; }"
            "QHeaderView { background-color: #1e293b; }"
            "QHeaderView::section { background-color: #1e293b; color: #60a5fa; padding: 8px; font-weight: bold; border: 1px solid #334155; }"
            "QTableCornerButton::section { background-color: #1e293b; border: 1px solid #334155; }"
            
            // Invoice Area
            "QTextEdit { background-color: #1e293b; border: 1px solid #334155; border-radius: 8px; color: #cbd5e1; font-family: 'Courier New', monospace; font-size: 13px; padding: 8px; }"
            
            // Buttons
            "QPushButton { background-color: #2563eb; color: white; border: none; padding: 9px 18px; border-radius: 6px; font-weight: bold; }"
            "QPushButton:hover { background-color: #3b82f6; }"
            "QPushButton:pressed { background-color: #1d4ed8; }"
            "QPushButton#clearBtn { background-color: #dc2626; }"
            "QPushButton#clearBtn:hover { background-color: #ef4444; }"
            "QPushButton#clearBtn:pressed { background-color: #b91c1c; }"
            "QPushButton#removeOrderBtn { background-color: #dc2626; }"
            "QPushButton#removeOrderBtn:hover { background-color: #ef4444; }"
            "QPushButton#removeOrderBtn:pressed { background-color: #b91c1c; }"
            
            // Custom QComboBox
            "QComboBox { background-color: #1e293b; border: 1px solid #334155; border-radius: 6px; padding: 6px 30px 6px 12px; color: #f8fafc; min-height: 20px; }"
            "QComboBox::drop-down { subcontrol-origin: padding; subcontrol-position: top right; width: 25px; border-left: 1px solid #334155; border-top-right-radius: 6px; border-bottom-right-radius: 6px; }"
            "QComboBox::down-arrow { border-left: 5px solid transparent; border-right: 5px solid transparent; border-top: 5px solid #60a5fa; width: 0; height: 0; }"
            "QComboBox QAbstractItemView { background-color: #1e293b; border: 1px solid #334155; selection-background-color: #334155; selection-color: #60a5fa; color: #f8fafc; }"
            
            // Custom QSpinBox / QDoubleSpinBox
            "QSpinBox, QDoubleSpinBox { background-color: #1e293b; border: 1px solid #334155; border-radius: 6px; padding: 6px 25px 6px 12px; color: #f8fafc; }"
            "QSpinBox::up-button, QDoubleSpinBox::up-button { subcontrol-origin: border; subcontrol-position: top right; width: 20px; border-left: 1px solid #334155; border-bottom: 1px solid #334155; border-top-right-radius: 6px; background-color: #1e293b; }"
            "QSpinBox::up-button:hover, QDoubleSpinBox::up-button:hover { background-color: #334155; }"
            "QSpinBox::up-arrow, QDoubleSpinBox::up-arrow { border-left: 4px solid transparent; border-right: 4px solid transparent; border-bottom: 4px solid #60a5fa; width: 0; height: 0; }"
            "QSpinBox::down-button, QDoubleSpinBox::down-button { subcontrol-origin: border; subcontrol-position: bottom right; width: 20px; border-left: 1px solid #334155; border-bottom-right-radius: 6px; background-color: #1e293b; }"
            "QSpinBox::down-button:hover, QDoubleSpinBox::down-button:hover { background-color: #334155; }"
            "QSpinBox::down-arrow, QDoubleSpinBox::down-arrow { border-left: 4px solid transparent; border-right: 4px solid transparent; border-top: 4px solid #60a5fa; width: 0; height: 0; }"
            
            // Labels & Titles
            "QLabel { font-weight: 500; }"
            "QWidget#headerWidget { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #1d4ed8, stop:1 #7c3aed); border-radius: 8px; }"
            "QLabel#titleLabel { font-size: 22px; font-weight: bold; color: #ffffff; }"
            "QLabel#subtitleLabel { font-size: 11px; color: #cbd5e1; }"
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
            
            // Table styling
            "QTableWidget { background-color: #ffffff; border: 1px solid #e2e8f0; border-radius: 8px; gridline-color: #f1f5f9; selection-background-color: #dbeafe; selection-color: #1e40af; }"
            "QTableWidget::item { padding: 6px; }"
            "QHeaderView { background-color: #ffffff; }"
            "QHeaderView::section { background-color: #f1f5f9; color: #1e40af; padding: 8px; font-weight: bold; border: 1px solid #e2e8f0; }"
            "QTableCornerButton::section { background-color: #f1f5f9; border: 1px solid #e2e8f0; }"
            
            // Invoice Area
            "QTextEdit { background-color: #ffffff; border: 1px solid #e2e8f0; border-radius: 8px; color: #334155; font-family: 'Courier New', monospace; font-size: 13px; padding: 8px; }"
            
            // Buttons
            "QPushButton { background-color: #2563eb; color: white; border: none; padding: 9px 18px; border-radius: 6px; font-weight: bold; }"
            "QPushButton:hover { background-color: #3b82f6; }"
            "QPushButton:pressed { background-color: #1d4ed8; }"
            "QPushButton#clearBtn { background-color: #dc2626; }"
            "QPushButton#clearBtn:hover { background-color: #ef4444; }"
            "QPushButton#clearBtn:pressed { background-color: #b91c1c; }"
            "QPushButton#removeOrderBtn { background-color: #dc2626; }"
            "QPushButton#removeOrderBtn:hover { background-color: #ef4444; }"
            "QPushButton#removeOrderBtn:pressed { background-color: #b91c1c; }"
            
            // Custom QComboBox
            "QComboBox { background-color: #ffffff; border: 1px solid #e2e8f0; border-radius: 6px; padding: 6px 30px 6px 12px; color: #0f172a; min-height: 20px; }"
            "QComboBox::drop-down { subcontrol-origin: padding; subcontrol-position: top right; width: 25px; border-left: 1px solid #e2e8f0; border-top-right-radius: 6px; border-bottom-right-radius: 6px; }"
            "QComboBox::down-arrow { border-left: 5px solid transparent; border-right: 5px solid transparent; border-top: 5px solid #2563eb; width: 0; height: 0; }"
            "QComboBox QAbstractItemView { background-color: #ffffff; border: 1px solid #e2e8f0; selection-background-color: #f1f5f9; selection-color: #2563eb; color: #0f172a; }"
            
            // Custom QSpinBox / QDoubleSpinBox
            "QSpinBox, QDoubleSpinBox { background-color: #ffffff; border: 1px solid #e2e8f0; border-radius: 6px; padding: 6px 25px 6px 12px; color: #0f172a; }"
            "QSpinBox::up-button, QDoubleSpinBox::up-button { subcontrol-origin: border; subcontrol-position: top right; width: 20px; border-left: 1px solid #e2e8f0; border-bottom: 1px solid #e2e8f0; border-top-right-radius: 6px; background-color: #ffffff; }"
            "QSpinBox::up-button:hover, QDoubleSpinBox::up-button:hover { background-color: #f1f5f9; }"
            "QSpinBox::up-arrow, QDoubleSpinBox::up-arrow { border-left: 4px solid transparent; border-right: 4px solid transparent; border-bottom: 4px solid #2563eb; width: 0; height: 0; }"
            "QSpinBox::down-button, QDoubleSpinBox::down-button { subcontrol-origin: border; subcontrol-position: bottom right; width: 20px; border-left: 1px solid #e2e8f0; border-bottom-right-radius: 6px; background-color: #ffffff; }"
            "QSpinBox::down-button:hover, QDoubleSpinBox::down-button:hover { background-color: #f1f5f9; }"
            "QSpinBox::down-arrow, QDoubleSpinBox::down-arrow { border-left: 4px solid transparent; border-right: 4px solid transparent; border-top: 4px solid #2563eb; width: 0; height: 0; }"
            
            // Labels & Titles
            "QLabel { font-weight: 500; }"
            "QWidget#headerWidget { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #2563eb, stop:1 #8b5cf6); border-radius: 8px; }"
            "QLabel#titleLabel { font-size: 22px; font-weight: bold; color: #ffffff; }"
            "QLabel#subtitleLabel { font-size: 11px; color: #f1f5f9; }"
            "QLabel#totalLabel { font-size: 20px; font-weight: bold; color: #059669; }"
        );
    }
}

void MainWindow::onDarkThemeTriggered() {
    applyTheme(true);
}

void MainWindow::onLightThemeTriggered() {
    applyTheme(false);
}

void MainWindow::onDeleteMenuItemClicked() {
    int selectedRow = menuTable->currentRow();
    if (selectedRow < 0) {
        QMessageBox::warning(this, "Selection Error", "Please select a menu item to delete.");
        return;
    }

    std::string itemID = menuTable->item(selectedRow, 0)->text().toStdString();
    std::string itemName = menuTable->item(selectedRow, 1)->text().toStdString();

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Confirm Deletion",
        QString("Are you sure you want to permanently delete '%1' (ID: %2) from the menu?")
            .arg(QString::fromStdString(itemName))
            .arg(QString::fromStdString(itemID)),
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        // Remove from local vector
        for (auto it = menuItems.begin(); it != menuItems.end(); ++it) {
            if ((*it)->getItemID() == itemID) {
                menuItems.erase(it);
                break;
            }
        }

        // Remove from RestaurantManager
        manager.removeMenuItem(itemID);

        // Save menu persistently
        std::string saveErr;
        if (!MenuFileIO::saveMenu("data/menu.txt", menuItems, &saveErr)) {
            QMessageBox::critical(this, "Save Error", QString::fromStdString(saveErr));
        }

        // Refresh UI table
        refreshMenuTable();
        QMessageBox::information(this, "Success", "Menu item deleted successfully!");
    }
}

void MainWindow::onMenuItemChanged(QTableWidgetItem *item) {
    if (!item) return;
    int col = item->column();
    if (col == 3) { // Price column
        int row = item->row();
        if (row < 0 || row >= static_cast<int>(menuItems.size())) return;

        QString txt = item->text().trimmed();
        if (txt.startsWith('$')) {
            txt = txt.mid(1);
        }

        bool ok = false;
        double newPrice = txt.toDouble(&ok);
        if (ok && newPrice >= 0.0) {
            // Update price in memory directly to bypass convenience fee logic
            menuItems[row]->setPrice(newPrice);

            // Re-save menu persistently
            std::string saveErr;
            if (!MenuFileIO::saveMenu("data/menu.txt", menuItems, &saveErr)) {
                QMessageBox::critical(this, "Save Error", QString::fromStdString(saveErr));
            }

            // Recalculate bill for active table to update order total in real time
            auto table = manager.getTable(currentTableNumber);
            if (table && table->hasActiveOrder() && !table->getCurrentOrder()->isEmpty()) {
                onCalculateBillClicked();
            }
        } else {
            QMessageBox::warning(this, "Invalid Price", "Please enter a valid positive decimal number for the price.");
        }

        // Reformat text to display nicely (e.g. $12.99)
        disconnect(menuTable, &QTableWidget::itemChanged, this, &MainWindow::onMenuItemChanged);
        item->setText(QString("$%1").arg(menuItems[row]->getPrice(), 0, 'f', 2));
        connect(menuTable, &QTableWidget::itemChanged, this, &MainWindow::onMenuItemChanged);
    }
}
